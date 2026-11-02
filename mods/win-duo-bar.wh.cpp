// ==WindhawkMod==
// @id              win-duo-bar
// @name            Win Duo Bar
// @description     A compact ring widget for the Windows 11 taskbar clock: battery gauge ring, Wi-Fi symbol, time and date, charging animation, and tray icon toggles. 任务栏圆环时钟小组件（电量/WiFi/时间日期），含充电动画和托盘图标开关。
// @version         1.0.0
// @author          Felinyu
// @github          https://github.com/Felinyu
// @license         GPL-3.0
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lversion
// ==/WindhawkMod==

// Published under the GNU General Public License v3.0.
// The tray icon tweaks part is derived from the "Taskbar tray system
// icon tweaks" Windhawk mod by m417z.

// ==WindhawkModReadme==
/*
# Win Duo Bar

A compact, minimal ring widget that replaces the Windows 11 notification-area
clock. It shows the device status at a glance:

- **Battery ring** - a 263 degree gauge with round caps and a translucent track.
  The filled part follows the battery level; the ring is green while charging,
  red when the battery is low (20% or less), yellow in battery saver mode and
  uses the taskbar text colour otherwise.
- **Wi-Fi symbol** in the middle - two thick rounded arcs plus a solid dot,
  traced from the original 09 Wi-Fi icon. It switches to the wired/VPN icon
  when the active connection is not wireless and dims when offline.
- **Time and date** - two rows of four bold digits (`1427` = 14:27, `1102` =
  November 2nd), no separators.
- **Charging animation** - a notch opens at the top of the ring, a white bolt
  grows out of it, the Wi-Fi symbol slides down a little and the colours
  cross-fade (about 0.5 s, 60 fps).

Every visual parameter (ring diameter/thickness/gauge span/track opacity/notch
angle/bolt size, Wi-Fi size/thickness/opening/position, digits size and row
positions) can be adjusted in the mod settings, and all settings are bilingual
(English/Chinese).

The mod also carries the tray icon tweaks: hide the volume, network, battery,
microphone, location/GPS, Studio Effects, Recall, language bar, language
supplementary and bell icons, show the battery icon in grayscale instead of
its coloured variant, hide the time/date when the widget is disabled, and hide
or resize the "Show desktop" button.

Only Windows 11 is supported.

# 中文说明

Win Duo Bar 是一个替换 Windows 11 通知区域时钟的圆环小组件：

- **电量圆环**：263° 圆环，圆头、带半透明轨道；电量部分随电量变化，充电时绿色、
  低电量（≤20%）红色、节电模式黄色，其他情况跟随任务栏文字颜色。
- **Wi-Fi 图标**：中间两条粗圆头弧线 + 实心圆点（按原版 09 图标描出）；当前连接
  不是无线时显示有线/VPN 图标，断网时变暗。
- **时间日期**：下方两行四位数字（`1427`=14:27、`1102`=11月2日），无分隔符。
- **充电动画**：充电时顶部张开豁口、白色闪电长出、Wi-Fi 图标下移、颜色渐变，
  约 0.5 秒 60fps。

所有视觉参数（圆环外径/粗细/弧度范围/轨道透明度/豁口角度/闪电大小、Wi-Fi 尺寸/
粗细/开口/位置、数字字号和两行位置）都能在设置里调整，设置项中英双语。

另外包含托盘图标开关：隐藏音量/网络/电池/麦克风/定位/工作室效果/Recall/语言栏/
语言栏附加图标/通知铃铛，电池图标单色显示，widget 关闭时隐藏时间日期，以及
"显示桌面"按钮的隐藏/宽度。

仅支持 Windows 11。

## Credits / 致谢

The tray icon tweaks part is derived from the
[Taskbar tray system icon tweaks](https://github.com/m417z/my-windhawk-mods)
Windhawk mod by [m417z](https://m417z.com/) (GPLv3).

托盘图标隐藏部分派生自 m417z 的 Taskbar tray system icon tweaks 模组（GPLv3）。
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- enabled: true
  $name: Enable the Win Duo Bar widget（启用 Win Duo Bar 小组件）
  $description: >-
    Replace the notification-area clock with the Win Duo Bar widget.
    用 Win Duo Bar 小组件替换通知区域的时钟。
- size: 38
  $name: Widget size, DIP（小组件尺寸，DIP）
- timeFormat: auto
  $name: Time format（时间格式）
  $options:
  - auto: System default（跟随系统）
  - h24: 24-hour, e.g. 1427（24 小时制）
  - h12: 12-hour, e.g. 0227（12 小时制）
- dateFormat: monthDay
  $name: Date format（日期格式）
  $options:
  - monthDay: Month then day, e.g. 1102（月在前，如 1102）
  - dayMonth: Day then month, e.g. 0211（日在前，如 0211）
- showDate: true
  $name: Show the date row in the widget（显示日期行）
- showNetwork: true
  $name: Show the network icon in the widget（显示网络图标）
- ringDiameter: "35"
  $name: Ring outer diameter, DIP（圆环外径，DIP）
  $description: >-
    Outer diameter of the battery ring in DIP (the widget canvas is 38 DIP).
    电量圆环的外径（DIP，画布默认 38 DIP）。
- ringThickness: "3.7"
  $name: Ring thickness, DIP（圆环粗细，DIP）
  $description: >-
    Stroke width of the ring.
    圆环线条的粗细。
- ringSweepDeg: "263"
  $name: Battery arc span, degrees（电量弧范围，度）
  $description: >-
    How many degrees of the circle the battery gauge covers; 360 is a full
    circle, 240 is the default look with a 120 degree gap at the bottom.
    电量弧覆盖的角度，360 为整圆，240 为默认（底部留 120° 缺口）。
- ringTrackOpacity: "28"
  $name: Track opacity, %（轨道不透明度，%）
  $description: >-
    Opacity of the translucent track behind the battery level; 0 hides it.
    电量填充后面那条半透明轨道的透明度，0 表示不显示。
- chargingNotchDeg: "48"
  $name: Charging notch angle, degrees（充电豁口角度，度）
  $description: >-
    Width of the notch that opens at the top while charging, where the bolt
    sits.
    充电时顶部张开、放闪电的豁口宽度（度）。
- boltWidth: "7.2"
  $name: Bolt width, DIP（闪电宽度，DIP）
- boltHeight: "9.1"
  $name: Bolt height, DIP（闪电高度，DIP）
- wifiHeight: "10.5"
  $name: Wi-Fi symbol height, DIP（Wi-Fi 图标高度，DIP）
- wifiWidth: "14.5"
  $name: Wi-Fi symbol width, DIP（Wi-Fi 图标宽度，DIP）
- wifiThickness: "2"
  $name: Wi-Fi line thickness, DIP（Wi-Fi 线条粗细，DIP）
- wifiOpeningDeg: "75"
  $name: Wi-Fi opening angle, degrees（Wi-Fi 开口角度，度）
  $description: >-
    Total angle between the two arc ends of the Wi-Fi symbol.
    Wi-Fi 图标两条弧线端点之间的总角度。
- wifiCenterPct: "41"
  $name: Wi-Fi vertical position, %（Wi-Fi 垂直位置，%）
  $description: >-
    Vertical centre of the Wi-Fi symbol, as a percentage of the widget height.
    Wi-Fi 图标中心在小组件高度上的百分比位置。
- wifiChargingDrop: "1.5"
  $name: Wi-Fi drop while charging, DIP（充电时 Wi-Fi 下移量，DIP）
- digitsSize: "7"
  $name: Digits font size, DIP（数字字号，DIP）
- timeRowPct: "73"
  $name: Time row position, %（时间行位置，%）
  $description: >-
    Vertical centre of the time row, as a percentage of the widget height.
    时间行中心在小组件高度上的百分比位置。
- dateRowPct: "89.5"
  $name: Date row position, %（日期行位置，%）
  $description: >-
    Vertical centre of the date row, as a percentage of the widget height.
    日期行中心在小组件高度上的百分比位置。
- digitsSpacing: 110
  $name: Digits letter spacing, 1/1000 em（数字字间距）
  $description: >-
    Letter spacing between the digits, in 1/1000 em units.
    数字之间的字间距，单位 1/1000 em。
- hideVolumeIcon: false
  $name: Hide volume icon（隐藏音量图标）
  $description: >-
    Hide the volume icon in the notification area.
    隐藏通知区域的音量图标。
- hideNetworkIcon: true
  $name: Hide network icon（隐藏网络图标）
  $description: >-
    Hide the network icon in the notification area.
    隐藏通知区域的网络图标。
- hideBatteryIcon: true
  $name: Hide battery icon（隐藏电池图标）
  $description: >-
    Hide the battery icon in the notification area.
    隐藏通知区域的电池图标。
- grayscaleBatteryIcon: false
  $name: Grayscale battery icon（电池图标单色）
  $description: >-
    Show the battery icon using the standard text color instead of its colored
    (e.g. green/yellow) variant.
    电池图标使用普通文字颜色，而不是绿/黄等彩色。
- hideMicrophoneIcon: false
  $name: Hide microphone icon（隐藏麦克风图标）
  $description: >-
    Hide the microphone icon in the notification area.
    隐藏通知区域的麦克风图标。
- hideGeolocationIcon: false
  $name: Hide location (GPS) icon（隐藏定位图标）
  $description: >-
    Hide the location (e.g. GPS) icon in the notification area.
    隐藏通知区域的定位（GPS）图标。
- hideStudioEffectsIcon: false
  $name: Hide Studio Effects icon（隐藏工作室效果图标）
  $description: >-
    Hide the Studio Effects icon in the notification area.
    隐藏通知区域的工作室效果图标。
- hideRecallIcon: false
  $name: Hide Recall icon（隐藏 Recall 图标）
  $description: >-
    Hide the Recall icon in the notification area.
    隐藏通知区域的 Recall 图标。
- hideLanguageBar: true
  $name: Hide language bar（隐藏语言栏）
  $description: >-
    Hide the language bar in the notification area.
    隐藏通知区域的语言栏。
- hideLanguageSupplementaryIcons: false
  $name: Hide language supplementary icons（隐藏语言栏附加图标）
  $description: >-
    Hide the supplementary icons of the language bar.
    隐藏语言栏旁边的附加图标。
- hideBellIcon: never
  $name: Hide bell icon（隐藏通知铃铛）
  $description: >-
    Hide the notification bell icon, always or only when there is nothing new.
    隐藏通知区域铃铛图标：总是隐藏，或只在没有新通知时隐藏。
  $options:
  - never: Never（从不隐藏）
  - whenInactive: When there are no new notifications（没有新通知时）
  - whenInactiveAndNoDnd: >-
      No new notifications and "Do not disturb" is off（没有新通知且勿扰关闭）
  - always: Always（总是隐藏）
- clockVisibility: showAll
  $name: Time and date (only when the widget is off)（时间日期，仅 widget 关闭时生效）
  $description: >-
    Hide the time and/or the date shown in the notification area, or hide the
    whole clock to reclaim its space. Only applies when the Win Duo Bar widget above is
    disabled.
    隐藏通知区域的时间/日期，或整块隐藏时钟以收回空间。只在上面「启用 Win Duo Bar 小组件」关闭时生效。
  $options:
  - showAll: Show time and date（显示时间与日期）
  - hideTime: Hide time（隐藏时间）
  - hideDate: Hide date（隐藏日期）
  - hideAll: Hide time and date（隐藏时间与日期）
- showDesktopButtonWidth: 8
  $name: '"Show desktop" button width（"显示桌面"按钮宽度）'
  $description: >-
    Width of the "Show desktop" button in DIP; 0 hides it.
    "显示桌面"按钮的宽度（DIP），设为 0 则隐藏。
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <algorithm>
#include <atomic>
#include <functional>
#include <list>
#include <optional>
#include <string>
#include <vector>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/base.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Networking.Connectivity.h>
#include <winrt/Windows.UI.Text.h>
#include <winrt/Windows.UI.Xaml.Shapes.h>

#include <chrono>
#include <cmath>

using namespace winrt::Windows::UI::Xaml;

enum class HideBellIcon {
    never,
    whenInactive,
    whenInactiveAndNoDnd,
    always,
};

enum class ClockVisibility {
    showAll,
    hideTime,
    hideDate,
    hideAll,
};

enum class TimeFormatSetting {
    autoFormat,
    h24,
    h12,
};

enum class DateFormatSetting {
    monthDay,
    dayMonth,
    chinese,
};

struct {
    bool hideVolumeIcon;
    bool hideNetworkIcon;
    bool hideBatteryIcon;
    bool grayscaleBatteryIcon;
    bool hideMicrophoneIcon;
    bool hideGeolocationIcon;
    bool hideStudioEffectsIcon;
    bool hideRecallIcon;
    bool hideLanguageBar;
    bool hideLanguageSupplementaryIcons;
    HideBellIcon hideBellIcon;
    ClockVisibility clockVisibility;
    int showDesktopButtonWidth;

    // Win Duo Bar widget
    bool enabled;
    int size;
    TimeFormatSetting timeFormat;
    DateFormatSetting dateFormat;
    bool showDate;
    bool showNetwork;
} g_settings;

std::atomic<bool> g_systemTrayModuleHooked;
std::atomic<bool> g_unloading;

using FrameworkElementLoadedEventRevoker = winrt::impl::event_revoker<
    IFrameworkElement,
    &winrt::impl::abi<IFrameworkElement>::type::remove_Loaded>;

[[clang::no_destroy]] std::optional<
    std::list<FrameworkElementLoadedEventRevoker>> g_autoRevokerList{
    std::in_place};

winrt::weak_ref<Controls::TextBlock> g_mainStackInnerTextBlock;
int64_t g_mainStackTextChangedToken;

winrt::weak_ref<FrameworkElement> g_bellSystemTrayIconElement;
int64_t g_bellAutomationNameChangedToken;

winrt::weak_ref<FrameworkElement> g_controlCenterStackPanel;
winrt::event_token g_controlCenterStackPanelSizeChangedToken;

struct BatteryTextBlockState {
    winrt::weak_ref<Controls::TextBlock> textBlock;
    Media::Brush savedForeground{nullptr};
    int64_t foregroundChangedToken{0};
};

[[clang::no_destroy]] std::optional<std::vector<BatteryTextBlockState>>
    g_batteryTextBlockStates{std::in_place};

// Local MinWidth/MaxWidth values replaced by the show desktop width override,
// nullopt for a property that had no local value.
struct ShowDesktopWidthState {
    winrt::weak_ref<FrameworkElement> element;
    std::optional<double> savedMinWidth;
    std::optional<double> savedMaxWidth;
};

std::vector<ShowDesktopWidthState> g_showDesktopWidthStates;

// Local Visibility values replaced by the clock visibility override, nullopt
// for an element that had no local value.
struct ClockVisibilityState {
    winrt::weak_ref<FrameworkElement> element;
    std::optional<Visibility> savedVisibility;
    int64_t visibilityChangedToken;
    bool hasVisibilityChangedToken;
};

[[clang::no_destroy]] std::optional<std::vector<ClockVisibilityState>>
    g_clockVisibilityStates{std::in_place};

HWND FindCurrentProcessTaskbarWnd() {
    HWND hTaskbarWnd = nullptr;

    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            DWORD dwProcessId;
            WCHAR className[32];
            if (GetWindowThreadProcessId(hWnd, &dwProcessId) &&
                dwProcessId == GetCurrentProcessId() &&
                GetClassName(hWnd, className, ARRAYSIZE(className)) &&
                _wcsicmp(className, L"Shell_TrayWnd") == 0) {
                *reinterpret_cast<HWND*>(lParam) = hWnd;
                return FALSE;
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&hTaskbarWnd));

    return hTaskbarWnd;
}

FrameworkElement EnumParentElements(
    FrameworkElement element,
    std::function<bool(FrameworkElement)> enumCallback) {
    auto parent = element;
    while (true) {
        parent = Media::VisualTreeHelper::GetParent(parent)
                     .try_as<FrameworkElement>();
        if (!parent) {
            return nullptr;
        }

        if (enumCallback(parent)) {
            return parent;
        }
    }
}

FrameworkElement GetParentElementByName(FrameworkElement element, PCWSTR name) {
    return EnumParentElements(element, [name](FrameworkElement parent) {
        return parent.Name() == name;
    });
}

bool IsChildOfElementByName(FrameworkElement element, PCWSTR name) {
    return !!GetParentElementByName(element, name);
}

FrameworkElement GetParentElementByClassName(FrameworkElement element,
                                             PCWSTR className) {
    return EnumParentElements(element, [className](FrameworkElement parent) {
        return winrt::get_class_name(parent) == className;
    });
}

bool IsChildOfElementByClassName(FrameworkElement element, PCWSTR className) {
    return !!GetParentElementByClassName(element, className);
}

FrameworkElement EnumChildElements(
    FrameworkElement element,
    std::function<bool(FrameworkElement)> enumCallback) {
    int childrenCount = Media::VisualTreeHelper::GetChildrenCount(element);

    for (int i = 0; i < childrenCount; i++) {
        auto child = Media::VisualTreeHelper::GetChild(element, i)
                         .try_as<FrameworkElement>();
        if (!child) {
            Wh_Log(L"Failed to get child %d of %d", i + 1, childrenCount);
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

FrameworkElement FindChildByClassName(FrameworkElement element,
                                      PCWSTR className) {
    return EnumChildElements(element, [className](FrameworkElement child) {
        return winrt::get_class_name(child) == className;
    });
}

FrameworkElement FindDescendantByName(FrameworkElement element, PCWSTR name) {
    FrameworkElement found = nullptr;
    EnumChildElements(element, [&found, name](FrameworkElement child) {
        if (child.Name() == name) {
            found = child;
            return true;
        }
        found = FindDescendantByName(child, name);
        return found != nullptr;
    });
    return found;
}

FrameworkElement FindDescendantByClassName(FrameworkElement element,
                                           PCWSTR className) {
    FrameworkElement found = nullptr;
    EnumChildElements(element, [&found, className](FrameworkElement child) {
        if (winrt::get_class_name(child) == className) {
            found = child;
            return true;
        }
        found = FindDescendantByClassName(child, className);
        return found != nullptr;
    });
    return found;
}

// https://stackoverflow.com/a/3382894
std::wstring StringToHex(std::wstring_view input) {
    static const WCHAR kHexDigits[] = L"0123456789ABCDEF";

    std::wstring output;
    output.reserve(input.length() * 5);
    for (WCHAR c : input) {
        output.push_back(kHexDigits[(c >> 12) & 15]);
        output.push_back(kHexDigits[(c >> 8) & 15]);
        output.push_back(kHexDigits[(c >> 4) & 15]);
        output.push_back(kHexDigits[c & 15]);
        output.push_back(L' ');
    }

    if (!output.empty() && output.back() == L' ') {
        output.resize(output.length() - 1);
    }

    return output;
}

enum class SystemTrayIconIdent {
    kUnknown,
    kNone,
    kVolume,
    kNetwork,
    kBattery,
    kMicrophone,
    kGeolocation,
    kMicrophoneAndGeolocation,
    kBellEmpty,
    kBellEmptyDnd,
    kBellFull,
    kBellFullDnd,
    kLanguage,
    kStudioEffects,
    kRecall,
};

SystemTrayIconIdent IdentifySystemTrayIconFromText(std::wstring_view text) {
    switch (text.length()) {
        case 0:
            return SystemTrayIconIdent::kNone;

        case 1:
            break;

        default:
            return SystemTrayIconIdent::kUnknown;
    }

    switch (text[0]) {
        case L'\uE74F':  // Mute
        case L'\uE992':  // Volume0
        case L'\uE993':  // Volume1
        case L'\uE994':  // Volume2
        case L'\uE995':  // Volume3
        case L'\uEA85':  // VolumeDisabled
        case L'\uEBC5':  // VolumeBars
            return SystemTrayIconIdent::kVolume;

        case L'\uE709':  // Airplane
        case L'\uE7F4':  // TVMonitor
        case L'\uE839':  // Ethernet
        case L'\uE86C':  // SignalBars1
        case L'\uE86D':  // SignalBars2
        case L'\uE86E':  // SignalBars3
        case L'\uE86F':  // SignalBars4
        case L'\uE870':  // SignalBars5
        case L'\uEAA1':  // Drinks
        case L'\uEAA2':  // DropShot
        case L'\uEAA3':  // FlavorProfile
        case L'\uEAA4':  // Float
        case L'\uEAA5':  // FluteGlass
        case L'\uEAA8':  // GobletGlass
        case L'\uEC1E':  // SignalRoaming
        case L'\uEC3C':  // MobWifi1
        case L'\uEC3D':  // MobWifi2
        case L'\uEC3E':  // MobWifi3
        case L'\uEC3F':  // MobWifi4
        case L'\uF384':  // NetworkOffline
        case L'\uF8C0':  // SysLocationArrow
        case L'\uF8C1':  // SysMicrophone
        case L'\uF8C2':  // SysVideo
        case L'\uF8C3':  // SysWifi4
        case L'\uF8C4':  // SysWifi3
        case L'\uF8C5':  // SysWifi2
        case L'\uF8C6':  // SysWifi1
        case L'\uF8C7':  // SysSignalBars5
        case L'\uF8C8':  // SysSignalBars4
        case L'\uF8C9':  // SysSignalBars3
        case L'\uF8CA':  // SysSignalBars2
        case L'\uF8CB':  // SysSignalBars1
        case L'\uF8CC':  // SysNetworkOffline
            return SystemTrayIconIdent::kNetwork;

        // Charging levels.
        case L'\uE3C1':  // Private Use
        case L'\uE3C2':  // Private Use
        case L'\uE3C3':  // Private Use
        case L'\uE3C4':  // Private Use
        case L'\uE3C5':  // Private Use
        case L'\uE3C6':  // Private Use
        case L'\uE3C7':  // Private Use
        case L'\uE3C8':  // Private Use
        case L'\uE3C9':  // Private Use
        case L'\uE3CA':  // Private Use
        case L'\uE3CB':  // Private Use
        case L'\uE408':  // Private Use
        case L'\uE409':  // Private Use
        case L'\uE40A':  // Private Use
        case L'\uE40B':  // Private Use
        case L'\uE40C':  // Private Use
        case L'\uE40D':  // Private Use
        case L'\uE40E':  // Private Use
        case L'\uE40F':  // Private Use
        case L'\uE410':  // Private Use
        case L'\uE411':  // Private Use
        case L'\uE412':  // Private Use
        case L'\uE413':  // Private Use
        case L'\uE414':  // Private Use
        case L'\uE415':  // Private Use
        case L'\uE416':  // Private Use
        case L'\uE417':  // Private Use
        case L'\uE418':  // Private Use
        case L'\uE419':  // Private Use
        case L'\uE41A':  // Private Use
        case L'\uE41B':  // Private Use
        case L'\uE41C':  // Private Use
        case L'\uE41D':  // Private Use
        case L'\uEBA0':  // MobBattery0
        case L'\uEBA1':  // MobBattery1
        case L'\uEBA2':  // MobBattery2
        case L'\uEBA3':  // MobBattery3
        case L'\uEBA4':  // MobBattery4
        case L'\uEBA5':  // MobBattery5
        case L'\uEBA6':  // MobBattery6
        case L'\uEBA7':  // MobBattery7
        case L'\uEBA8':  // MobBattery8
        case L'\uEBA9':  // MobBattery9
        case L'\uEBAA':  // MobBattery10
        case L'\uEBAB':  // MobBatteryCharging0
        case L'\uEBAC':  // MobBatteryCharging1
        case L'\uEBAD':  // MobBatteryCharging2
        case L'\uEBAE':  // MobBatteryCharging3
        case L'\uEBAF':  // MobBatteryCharging4
        case L'\uEBB0':  // MobBatteryCharging5
        case L'\uEBB1':  // MobBatteryCharging6
        case L'\uEBB2':  // MobBatteryCharging7
        case L'\uEBB3':  // MobBatteryCharging8
        case L'\uEBB4':  // MobBatteryCharging9
        case L'\uEBB5':  // MobBatteryCharging10
        case L'\uEBB6':  // MobBatterySaver0
        case L'\uEBB7':  // MobBatterySaver1
        case L'\uEBB8':  // MobBatterySaver2
        case L'\uEBB9':  // MobBatterySaver3
        case L'\uEBBA':  // MobBatterySaver4
        case L'\uEBBB':  // MobBatterySaver5
        case L'\uEBBC':  // MobBatterySaver6
        case L'\uEBBD':  // MobBatterySaver7
        case L'\uEBBE':  // MobBatterySaver8
        case L'\uEBBF':  // MobBatterySaver9
        case L'\uEBC0':  // MobBatterySaver10
        // Misc.
        case L'\uEB17':
        case L'\uEC02':
        case L'\uF1E8':
            return SystemTrayIconIdent::kBattery;

        case L'\uE361':  // Private Use
        case L'\uE720':  // Microphone
        case L'\uEC71':  // MicOn
            return SystemTrayIconIdent::kMicrophone;

        case L'\uE37A':
            return SystemTrayIconIdent::kGeolocation;

        case L'\uF47F':
            return SystemTrayIconIdent::kMicrophoneAndGeolocation;

        case L'\uF2A3':  // Empty bell
            return SystemTrayIconIdent::kBellEmpty;

        case L'\uF285':  // Empty bell, Do Not Disturb
            return SystemTrayIconIdent::kBellEmptyDnd;

        case L'\uF2A5':  // Full bell
            return SystemTrayIconIdent::kBellFull;

        case L'\uF2A8':  // Full bell, Do Not Disturb
            return SystemTrayIconIdent::kBellFullDnd;

        // Language supplementary icons.
        // Found by installing all the built-in input methods from:
        // https://learn.microsoft.com/en-us/windows-hardware/manufacture/desktop/windows-language-pack-default-values?view=windows-11#input-method-editors
        // and identifying the icon code in the fonts Segoe Fluent and
        // AXPIcons.ttf.
        // https://learn.microsoft.com/en-us/windows/apps/design/style/segoe-fluent-icons-font
        // %SystemRoot%\SystemApps\MicrosoftWindows.Client.Core_cw5n1h2txyewy\SystemTray\Assets\AXPIcons.ttf
        case L'\uE4D7':  // (Maybe) English Private mode
        case L'\uE4D8':  // (Maybe) Chinese Private mode
        case L'\uE5BF':  // (Maybe) English mode locked
        case L'\uE97E':  // HalfAlpha
        case L'\uE97F':  // FullAlpha
        case L'\uE980':  // Key12On (Korean mode)
        case L'\uE982':  // QWERTYOn (Chinese mode)
        case L'\uE983':  // QWERTYOff (English mode)
        case L'\uE986':  // FullHiragana
        case L'\uE987':  // FullKatakana
        case L'\uE988':  // HalfKatakana
        case L'\uEB90':  // StatusErrorFull (Input method disabled)
        case L'\uEE41':  // FullHiraganaPrivateMode
        case L'\uEE42':  // FullKatakanaPrivateMode
        case L'\uEE43':  // HalfAlphaPrivateMode
        case L'\uEE44':  // HalfKatakanaPrivateMode
        case L'\uEE45':  // FullAlphaPrivateMode
        case L'\uEE75':  // (Maybe) HalfAlpha
        case L'\uEE76':  // (Maybe) HalfAlphaPrivateMode
            return SystemTrayIconIdent::kLanguage;

        case L'\uEABC':
            return SystemTrayIconIdent::kStudioEffects;

        // From:
        // C:\Windows\SystemApps\MicrosoftWindows.Client.Core_cw5n1h2txyewy\SystemTray\Assets\RecallIcons.ttf
        case L'\uEC83':
        case L'\uEADD':
        case L'\uEB16':
        case L'\uEF97':
        case L'\uF1C6':
            return SystemTrayIconIdent::kRecall;
    }

    return SystemTrayIconIdent::kUnknown;
}

void ApplyMainStackIconViewStyle(FrameworkElement notifyIconViewElement) {
    FrameworkElement systemTrayTextIconContent = nullptr;

    FrameworkElement child = notifyIconViewElement;
    if ((child = FindChildByName(child, L"ContainerGrid")) &&
        (child = FindChildByName(child, L"ContentPresenter")) &&
        (child = FindChildByName(child, L"ContentGrid")) &&
        (child = FindChildByClassName(child, L"SystemTray.TextIconContent"))) {
        systemTrayTextIconContent = child;
    } else {
        Wh_Log(L"Failed to get SystemTray.TextIconContent");
        return;
    }

    Controls::TextBlock innerTextBlock = nullptr;

    child = systemTrayTextIconContent;
    if ((child = FindChildByName(child, L"ContainerGrid")) &&
        (child = FindChildByName(child, L"Base")) &&
        (child = FindChildByName(child, L"InnerTextBlock"))) {
        innerTextBlock = child.as<Controls::TextBlock>();
    } else {
        Wh_Log(L"Failed to get InnerTextBlock");
        return;
    }

    auto shouldHide = [](Controls::TextBlock innerTextBlock) {
        auto text = innerTextBlock.Text();
        auto systemTrayIconIdent = IdentifySystemTrayIconFromText(text);

        bool hide = false;
        if (!g_unloading) {
            switch (systemTrayIconIdent) {
                case SystemTrayIconIdent::kMicrophone:
                    hide = g_settings.hideMicrophoneIcon;
                    break;

                case SystemTrayIconIdent::kGeolocation:
                    hide = g_settings.hideGeolocationIcon;
                    break;

                case SystemTrayIconIdent::kMicrophoneAndGeolocation:
                    hide = g_settings.hideMicrophoneIcon &&
                           g_settings.hideGeolocationIcon;
                    break;

                case SystemTrayIconIdent::kStudioEffects:
                    hide = g_settings.hideStudioEffectsIcon;
                    break;

                case SystemTrayIconIdent::kRecall:
                    hide = g_settings.hideRecallIcon;
                    break;

                case SystemTrayIconIdent::kNone:
                    // Happens when the icon is about to disappear.
                    break;

                default:
                    Wh_Log(L"Failed");
                    break;
            }
        }

        Wh_Log(L"Main stack icon %d (%s), hide=%d", (int)systemTrayIconIdent,
               StringToHex(text).c_str(), hide);

        return hide;
    };

    bool hide = shouldHide(innerTextBlock);

    notifyIconViewElement.Visibility(hide ? Visibility::Collapsed
                                          : Visibility::Visible);

    if (!g_unloading && !g_mainStackInnerTextBlock.get()) {
        auto notifyIconViewElementWeakRef =
            winrt::make_weak(notifyIconViewElement);
        g_mainStackInnerTextBlock = innerTextBlock;
        g_mainStackTextChangedToken =
            innerTextBlock.RegisterPropertyChangedCallback(
                Controls::TextBlock::TextProperty(),
                [notifyIconViewElementWeakRef, shouldHide](
                    DependencyObject sender, DependencyProperty property) {
                    auto innerTextBlock = sender.try_as<Controls::TextBlock>();
                    if (!innerTextBlock) {
                        return;
                    }

                    auto notifyIconViewElement =
                        notifyIconViewElementWeakRef.get();
                    if (!notifyIconViewElement) {
                        return;
                    }

                    bool hide = shouldHide(innerTextBlock);

                    Wh_Log(L"Main stack icon, hide=%d", hide);

                    notifyIconViewElement.Visibility(
                        hide ? Visibility::Collapsed : Visibility::Visible);
                });
    }
}

void ApplyNonActivatableStackIconViewStyle(
    FrameworkElement notifyIconViewElement) {
    FrameworkElement child = notifyIconViewElement;
    bool hide = false;
    if ((child = FindChildByName(child, L"ContainerGrid")) &&
        (child = FindChildByName(child, L"ContentPresenter")) &&
        (child = FindChildByName(child, L"ContentGrid"))) {
        child = EnumChildElements(child, [&hide](FrameworkElement child) {
            auto className = winrt::get_class_name(child);
            if (className == L"SystemTray.TextIconContent") {
                Controls::TextBlock innerTextBlock = nullptr;

                if ((child = FindChildByName(child, L"ContainerGrid")) &&
                    (child = FindChildByName(child, L"Base")) &&
                    (child = FindChildByName(child, L"InnerTextBlock"))) {
                    innerTextBlock = child.as<Controls::TextBlock>();
                } else {
                    Wh_Log(L"Failed to get InnerTextBlock");
                    return false;
                }

                auto text = innerTextBlock.Text();
                auto systemTrayIconIdent = IdentifySystemTrayIconFromText(text);

                if (systemTrayIconIdent == SystemTrayIconIdent::kLanguage) {
                    Wh_Log(L"Language supplementary icon %d (%s)",
                           (int)systemTrayIconIdent, StringToHex(text).c_str());
                    hide = g_settings.hideLanguageSupplementaryIcons;
                    return true;
                } else {
                    Wh_Log(L"Language bar unknown icon %d (%s)",
                           (int)systemTrayIconIdent, StringToHex(text).c_str());
                    return false;
                }
            } else if (className == L"SystemTray.ImageIconContent") {
                Wh_Log(L"Language supplementary icon (image)");
                hide = g_settings.hideLanguageSupplementaryIcons;
                return true;
            } else if (className == L"SystemTray.LanguageTextIconContent" ||
                       className == L"SystemTray.LanguageImageIconContent") {
                Wh_Log(L"Language bar main icon");
                hide = g_settings.hideLanguageBar;
                return true;
            }

            Wh_Log(L"Unsupported class name %s of child", className.c_str());
            return false;
        });
    }

    if (!child) {
        // Some input methods use LanguageImageIconContent/ImageIconContent
        // instead of LanguageTextIconContent/TextIconContent with icon fonts.
        // If the language bar is hidden and the user switches from a "text"
        // input method to a "image" input method, the invisible element will
        // not be populated with the new type of icon content but become empty
        // instead. Then the icon will be permanently hidden even after
        // disabling the mod. This code forces the empty element to become
        // visible and populated, fixing this issue.
        if (Media::VisualTreeHelper::GetChildrenCount(notifyIconViewElement) ==
            0) {
            notifyIconViewElement.Visibility(Visibility::Visible);
        }
        return;
    }

    hide = !g_unloading && hide;

    Wh_Log(L"Language bar, hide=%d", hide);

    notifyIconViewElement.Visibility(hide ? Visibility::Collapsed
                                          : Visibility::Visible);
}

// Path inside SystemTray.BatteryIconContent:
// Grid#ContainerGrid > StackPanel > Grid > TextBlock.
//
// The system sets a colored Foreground brush on each battery TextBlock.
// Clearing the local Foreground value lets the inherited TextFillColorPrimary
// take over, giving a grayscale look. The system re-applies the colored brush
// on theme changes (and similar updates), so we listen for ForegroundProperty
// changes, save the latest value (so we can restore it on disable), and clear
// it again.
void ApplyBatteryIconGrayscaleStyle(FrameworkElement batteryIconContent) {
    bool grayscale = !g_unloading && g_settings.grayscaleBatteryIcon;

    // Drop any state for TextBlocks that no longer exist.
    std::erase_if(
        *g_batteryTextBlockStates,
        [](const BatteryTextBlockState& s) { return !s.textBlock.get(); });

    // Nothing to apply and nothing to restore - skip the tree traversal.
    if (!grayscale && g_batteryTextBlockStates->empty()) {
        return;
    }

    FrameworkElement grid = nullptr;

    FrameworkElement child = batteryIconContent;
    if ((child = FindChildByName(child, L"ContainerGrid")) &&
        (child = FindChildByClassName(
             child, L"Windows.UI.Xaml.Controls.StackPanel")) &&
        (child =
             FindChildByClassName(child, L"Windows.UI.Xaml.Controls.Grid"))) {
        grid = child;
    } else {
        Wh_Log(L"Failed to navigate to battery Grid");
        return;
    }

    EnumChildElements(grid, [grayscale](FrameworkElement textChild) {
        auto textBlock = textChild.try_as<Controls::TextBlock>();
        if (!textBlock) {
            return false;
        }

        auto it = g_batteryTextBlockStates->begin();
        for (; it != g_batteryTextBlockStates->end(); ++it) {
            if (it->textBlock.get() == textBlock) {
                break;
            }
        }
        bool managed = (it != g_batteryTextBlockStates->end());

        if (grayscale && !managed) {
            auto localForeground =
                textBlock
                    .ReadLocalValue(Controls::TextBlock::ForegroundProperty())
                    .try_as<Media::Brush>();
            if (!localForeground) {
                // No local Foreground set, so clearing would be a no-op and
                // there'd be nothing meaningful to restore later.
                return false;
            }

            BatteryTextBlockState state;
            state.textBlock = textBlock;
            state.savedForeground = localForeground;
            state.foregroundChangedToken =
                textBlock.RegisterPropertyChangedCallback(
                    Controls::TextBlock::ForegroundProperty(),
                    [](DependencyObject sender, DependencyProperty) {
                        auto tb = sender.try_as<Controls::TextBlock>();
                        if (!tb) {
                            return;
                        }
                        auto fg =
                            tb.ReadLocalValue(
                                  Controls::TextBlock::ForegroundProperty())
                                .try_as<Media::Brush>();
                        if (!fg) {
                            return;
                        }
                        for (auto& s : *g_batteryTextBlockStates) {
                            if (s.textBlock.get() == tb) {
                                s.savedForeground = fg;
                                break;
                            }
                        }
                        tb.as<DependencyObject>().ClearValue(
                            Controls::TextBlock::ForegroundProperty());
                    });
            g_batteryTextBlockStates->push_back(std::move(state));
            textBlock.as<DependencyObject>().ClearValue(
                Controls::TextBlock::ForegroundProperty());
        } else if (!grayscale && managed) {
            textBlock.UnregisterPropertyChangedCallback(
                Controls::TextBlock::ForegroundProperty(),
                it->foregroundChangedToken);
            textBlock.Foreground(it->savedForeground);
            g_batteryTextBlockStates->erase(it);
        }
        return false;
    });
}

void SetItemContainerMargin(FrameworkElement container,
                            double left,
                            double right) {
    if (left == 0 && right == 0) {
        container.ClearValue(FrameworkElement::MarginProperty());
    } else {
        container.Margin(Thickness{left, 0, right, 0});
    }
}

// A hidden icon keeps its (empty) item container in the panel, and the panel
// adds Spacing for every child regardless of visibility or size. Cancel the
// extra spacing with negative margins on the neighboring visible items, and
// hide the button if all icons are hidden.
void UpdateControlCenterButtonStackPanel(FrameworkElement stackPanel) {
    double spacing = 0;
    if (auto panel = stackPanel.try_as<Controls::StackPanel>()) {
        spacing = panel.Spacing();
    }

    // Margins are assigned once the next visible item (or the end) is known,
    // so that the trailing hidden items can be folded into the right margin.
    FrameworkElement pendingContainer = nullptr;
    double pendingLeft = 0;
    int hiddenRun = 0;

    EnumChildElements(stackPanel, [&](FrameworkElement child) {
        auto childClassName = winrt::get_class_name(child);
        if (childClassName != L"Windows.UI.Xaml.Controls.ContentPresenter") {
            Wh_Log(L"Unsupported class name %s of child",
                   childClassName.c_str());
            return false;
        }

        auto systemTrayIconElement = FindChildByName(child, L"SystemTrayIcon")
                                         .try_as<Controls::Control>();
        if (!systemTrayIconElement) {
            Wh_Log(L"Failed to get SystemTrayIcon of child");
            return false;
        }

        if (!systemTrayIconElement.IsEnabled()) {
            hiddenRun++;
            return false;
        }

        if (pendingContainer) {
            SetItemContainerMargin(pendingContainer, pendingLeft, 0);
        }

        pendingContainer = child;
        pendingLeft = -spacing * hiddenRun;
        hiddenRun = 0;
        return false;
    });

    if (pendingContainer) {
        SetItemContainerMargin(pendingContainer, pendingLeft,
                               -spacing * hiddenRun);
    }

    // The button template has some width of its own (e.g. the background
    // border margins), so an empty panel alone doesn't make it disappear.
    FrameworkElement controlCenterButton =
        GetParentElementByName(stackPanel, L"ControlCenterButton");
    if (!controlCenterButton) {
        Wh_Log(L"Failed to get ControlCenterButton");
        return;
    }

    controlCenterButton.Visibility(pendingContainer ? Visibility::Visible
                                                    : Visibility::Collapsed);
}

void ApplyControlCenterButtonIconStyle(FrameworkElement systemTrayIconElement) {
    FrameworkElement contentGrid = nullptr;

    FrameworkElement child = systemTrayIconElement;
    if ((child = FindChildByName(child, L"ContainerGrid")) &&
        (child = FindChildByName(child, L"ContentGrid"))) {
        contentGrid = child;
    } else {
        Wh_Log(L"Failed to get ContentGrid");
        return;
    }

    bool hide = false;
    FrameworkElement systemTrayTextIconContent =
        FindChildByClassName(contentGrid, L"SystemTray.BatteryIconContent");
    if (systemTrayTextIconContent) {
        if (!g_unloading) {
            hide = g_settings.hideBatteryIcon;
        }

        Wh_Log(L"System battery tray icon, hide=%d", hide);

        ApplyBatteryIconGrayscaleStyle(systemTrayTextIconContent);
    } else {
        systemTrayTextIconContent =
            FindChildByClassName(contentGrid, L"SystemTray.TextIconContent");
        if (!systemTrayTextIconContent) {
            Wh_Log(L"Failed to get SystemTray.TextIconContent");
            return;
        }

        Controls::TextBlock innerTextBlock = nullptr;

        child = systemTrayTextIconContent;
        if ((child = FindChildByName(child, L"ContainerGrid")) &&
            (child = FindChildByName(child, L"Base")) &&
            (child = FindChildByName(child, L"InnerTextBlock"))) {
            innerTextBlock = child.as<Controls::TextBlock>();
        } else {
            Wh_Log(L"Failed to get InnerTextBlock");
            return;
        }

        auto text = innerTextBlock.Text();
        auto systemTrayIconIdent = IdentifySystemTrayIconFromText(text);

        if (!g_unloading) {
            switch (systemTrayIconIdent) {
                case SystemTrayIconIdent::kVolume:
                    hide = g_settings.hideVolumeIcon;
                    break;

                case SystemTrayIconIdent::kNetwork:
                    hide = g_settings.hideNetworkIcon;
                    break;

                case SystemTrayIconIdent::kBattery:
                    hide = g_settings.hideBatteryIcon;
                    break;

                default:
                    Wh_Log(L"Failed");
                    break;
            }
        }

        Wh_Log(L"System tray icon %d (%s), hide=%d", (int)systemTrayIconIdent,
               StringToHex(text).c_str(), hide);
    }

    bool hidden =
        systemTrayTextIconContent.Visibility() == Visibility::Collapsed;
    if (hide != hidden) {
        systemTrayTextIconContent.Visibility(hide ? Visibility::Collapsed
                                                  : Visibility::Visible);
        if (auto control = systemTrayIconElement.try_as<Controls::Control>()) {
            control.IsEnabled(!hide);
        } else {
            Wh_Log(L"Failed");
        }
    }

    // The layout depends on the siblings too, e.g. a newly added visible icon
    // must show the button even if this icon's own state is unchanged.
    FrameworkElement parent = systemTrayIconElement;
    if ((parent = Media::VisualTreeHelper::GetParent(parent)
                      .try_as<FrameworkElement>()) &&
        winrt::get_class_name(parent) ==
            L"Windows.UI.Xaml.Controls.ContentPresenter" &&
        (parent = Media::VisualTreeHelper::GetParent(parent)
                      .try_as<FrameworkElement>()) &&
        winrt::get_class_name(parent) ==
            L"Windows.UI.Xaml.Controls.StackPanel") {
        // Removing an item container changes the panel size, which is the
        // only signal that the margins of the remaining items are stale.
        if (!g_unloading && !g_controlCenterStackPanel.get()) {
            g_controlCenterStackPanel = parent;
            g_controlCenterStackPanelSizeChangedToken = parent.SizeChanged(
                [](winrt::Windows::Foundation::IInspectable const& sender,
                   SizeChangedEventArgs const& e) {
                    Wh_Log(L">");

                    auto stackPanel = sender.try_as<FrameworkElement>();
                    if (!stackPanel) {
                        Wh_Log(L"Failed to get sender");
                        return;
                    }

                    UpdateControlCenterButtonStackPanel(stackPanel);
                });
        }

        UpdateControlCenterButtonStackPanel(parent);
    } else {
        Wh_Log(L"Failed");
    }
}

void ApplyBellIconStyle(FrameworkElement systemTrayIconElement);

// When the clock is hidden, the bell icon behaves differently - its content is
// recreated each time the bell icon changes. Therefore we retry until the
// content is there.
void ApplyBellIconStyleWithRetry(FrameworkElement systemTrayIconElement,
                                 int attempt) {
    Wh_Log(L"> %d", attempt);

    if (attempt == 10 || g_unloading) {
        return;
    }

    FrameworkElement containerGrid =
        FindChildByName(systemTrayIconElement, L"ContainerGrid");
    if (!containerGrid) {
        systemTrayIconElement.Dispatcher().TryRunAsync(
            winrt::Windows::UI::Core::CoreDispatcherPriority::Low,
            [systemTrayIconElement, attempt]() {
                ApplyBellIconStyleWithRetry(systemTrayIconElement, attempt + 1);
            });
        return;
    }

    ApplyBellIconStyle(systemTrayIconElement);
}

void ApplyBellIconStyle(FrameworkElement systemTrayIconElement) {
    FrameworkElement containerGrid =
        FindChildByName(systemTrayIconElement, L"ContainerGrid");
    if (!containerGrid) {
        Wh_Log(L"Failed to get ContainerGrid");
        return;
    }

    FrameworkElement systemTrayTextIconContent = nullptr;
    bool hasContentPresenterForMissingClock = true;

    // When the clock is hidden, the element path is:
    //
    // #ContainerGrid > #ContentPresenter > #ContentGrid >
    // SystemTray.TextIconContent
    //
    // When the clock is visible, ContentPresenter is missing:
    //
    // #ContainerGrid > #ContentGrid > SystemTray.TextIconContent
    FrameworkElement child =
        FindChildByName(containerGrid, L"ContentPresenter");
    if (!child) {
        hasContentPresenterForMissingClock = false;
        child = containerGrid;
    }

    if ((child = FindChildByName(child, L"ContentGrid")) &&
        (child = FindChildByClassName(child, L"SystemTray.TextIconContent"))) {
        systemTrayTextIconContent = child;
    } else {
        // Wh_Log(L"Failed to get SystemTray.TextIconContent");
        return;
    }

    auto contentPresenter =
        Media::VisualTreeHelper::GetParent(systemTrayIconElement)
            .try_as<FrameworkElement>();

    bool hide = false;

    if (!g_unloading) {
        if (g_settings.hideBellIcon == HideBellIcon::always) {
            hide = true;
        } else if (g_settings.hideBellIcon == HideBellIcon::whenInactive ||
                   g_settings.hideBellIcon ==
                       HideBellIcon::whenInactiveAndNoDnd) {
            Controls::TextBlock innerTextBlock = nullptr;

            child = systemTrayTextIconContent;
            if ((child = FindChildByName(child, L"ContainerGrid")) &&
                (child = FindChildByName(child, L"Base")) &&
                (child = FindChildByName(child, L"InnerTextBlock"))) {
                innerTextBlock = child.as<Controls::TextBlock>();
            } else {
                Wh_Log(L"Failed to get InnerTextBlock");
                return;
            }

            auto shouldHide = [](Controls::TextBlock innerTextBlock) {
                auto text = innerTextBlock.Text();
                auto systemTrayIconIdent = IdentifySystemTrayIconFromText(text);
                switch (systemTrayIconIdent) {
                    case SystemTrayIconIdent::kBellEmpty:
                        return true;

                    case SystemTrayIconIdent::kBellEmptyDnd:
                        return g_settings.hideBellIcon !=
                               HideBellIcon::whenInactiveAndNoDnd;

                    case SystemTrayIconIdent::kBellFull:
                    case SystemTrayIconIdent::kBellFullDnd:
                        return false;

                    default:
                        Wh_Log(L"Unknown bell icon %d (%s)",
                               (int)systemTrayIconIdent,
                               StringToHex(text).c_str());
                        return false;
                }
            };

            hide = shouldHide(innerTextBlock);

            if (!g_bellSystemTrayIconElement.get()) {
                g_bellSystemTrayIconElement = systemTrayIconElement;
                g_bellAutomationNameChangedToken =
                    systemTrayIconElement.RegisterPropertyChangedCallback(
                        Automation::AutomationProperties::NameProperty(),
                        [](DependencyObject sender,
                           DependencyProperty property) {
                            Wh_Log(L">");

                            auto bellSystemTrayIconElement =
                                sender.try_as<FrameworkElement>();
                            if (!bellSystemTrayIconElement) {
                                Wh_Log(L"Failed to get sender");
                                return;
                            }

                            ApplyBellIconStyleWithRetry(
                                bellSystemTrayIconElement, 0);
                        });
            }
        }
    }

    Wh_Log(L"Bell icon, hide=%d", hide);

    systemTrayTextIconContent.Visibility(hide ? Visibility::Collapsed
                                              : Visibility::Visible);

    if (contentPresenter) {
        if (hide && hasContentPresenterForMissingClock) {
            contentPresenter.MaxWidth(0);
        } else {
            contentPresenter.ClearValue(FrameworkElement::MaxWidthProperty());
        }
    }
}


// The clock is a system tray icon whose content is SystemTray.DateTimeIconContent
// with a template of:
//
// Grid#ContainerGrid > StackPanel > TextBlock#TimeInnerTextBlock
//                                > TextBlock#DateInnerTextBlock
//
// The content is hosted by a SystemTray.IconView, which is hidden together with
// the content to also reclaim the space that the clock occupies.
void ApplyClockElementVisibility(FrameworkElement element, bool hide) {
    auto it = std::find_if(
        g_clockVisibilityStates->begin(), g_clockVisibilityStates->end(),
        [element](const ClockVisibilityState& state) {
            return state.element.get() == element;
        });
    bool managed = (it != g_clockVisibilityStates->end());

    if (hide) {
        if (!managed) {
            ClockVisibilityState state;
            state.element = element;
            state.savedVisibility = element
                                        .ReadLocalValue(
                                            UIElement::VisibilityProperty())
                                        .try_as<Visibility>();
            state.visibilityChangedToken = 0;
            state.hasVisibilityChangedToken = false;

            // The system re-applies the visibility of the clock text blocks
            // whenever it updates the clock text, so keep forcing it back.
            state.visibilityChangedToken =
                element.RegisterPropertyChangedCallback(
                    UIElement::VisibilityProperty(),
                    [](DependencyObject sender, DependencyProperty) {
                        auto element = sender.try_as<FrameworkElement>();
                        if (!element) {
                            return;
                        }

                        if (element.Visibility() != Visibility::Collapsed) {
                            element.Visibility(Visibility::Collapsed);
                        }
                    });
            state.hasVisibilityChangedToken = true;

            g_clockVisibilityStates->push_back(std::move(state));
            it = g_clockVisibilityStates->end();
            --it;
        }

        element.Visibility(Visibility::Collapsed);
    } else if (managed) {
        if (it->hasVisibilityChangedToken) {
            element.UnregisterPropertyChangedCallback(
                UIElement::VisibilityProperty(), it->visibilityChangedToken);
            it->hasVisibilityChangedToken = false;
            it->visibilityChangedToken = 0;
        }

        if (it->savedVisibility) {
            element.Visibility(*it->savedVisibility);
        } else {
            element.ClearValue(UIElement::VisibilityProperty());
        }
    }
}

void RestoreClockVisibility() {
    for (auto& state : *g_clockVisibilityStates) {
        auto element = state.element.get();
        if (!element) {
            continue;
        }

        if (state.hasVisibilityChangedToken) {
            element.UnregisterPropertyChangedCallback(
                UIElement::VisibilityProperty(), state.visibilityChangedToken);
            state.hasVisibilityChangedToken = false;
            state.visibilityChangedToken = 0;
        }

        if (state.savedVisibility) {
            element.Visibility(*state.savedVisibility);
        } else {
            element.ClearValue(UIElement::VisibilityProperty());
        }
    }
    g_clockVisibilityStates->clear();
}

// Finds the SystemTray.IconView that hosts the clock content and the
// ContentPresenter item container of the hosting stack.
bool ResolveClockIconView(FrameworkElement clockContent,
                          FrameworkElement* clockIconView,
                          FrameworkElement* clockContainer) {
    FrameworkElement element = clockContent;
    for (int i = 0; i < 8; i++) {
        auto parent = Media::VisualTreeHelper::GetParent(element)
                          .try_as<FrameworkElement>();
        if (!parent) {
            return false;
        }

        element = parent;
        auto className = winrt::get_class_name(element);

        if (!*clockIconView && className == L"SystemTray.IconView") {
            *clockIconView = element;
            continue;
        }

        if (*clockIconView && !*clockContainer &&
            className == L"Windows.UI.Xaml.Controls.ContentPresenter") {
            *clockContainer = element;
            continue;
        }

        if (*clockIconView &&
            (className == L"SystemTray.Stack" ||
             element.Name() == L"SystemTrayFrameGrid")) {
            break;
        }
    }

    return !!*clockIconView;
}

void ApplyClockStyle(FrameworkElement clockContent, int attempt) {
    std::erase_if(*g_clockVisibilityStates,
                  [](const ClockVisibilityState& state) {
                      return !state.element.get();
                  });

    bool hideTime = false;
    bool hideDate = false;
    bool hideClock = false;

    if (!g_unloading) {
        hideTime = g_settings.clockVisibility == ClockVisibility::hideTime ||
                   g_settings.clockVisibility == ClockVisibility::hideAll;
        hideDate = g_settings.clockVisibility == ClockVisibility::hideDate ||
                   g_settings.clockVisibility == ClockVisibility::hideAll;
        hideClock = g_settings.clockVisibility == ClockVisibility::hideAll;
    }

    Wh_Log(L"Clock, hideClock=%d, hideTime=%d, hideDate=%d", hideClock, hideTime,
           hideDate);

    Controls::TextBlock timeTextBlock =
        FindDescendantByName(clockContent, L"TimeInnerTextBlock")
            .try_as<Controls::TextBlock>();
    if (timeTextBlock) {
        ApplyClockElementVisibility(timeTextBlock, hideTime);
    } else {
        Wh_Log(L"Failed to get TimeInnerTextBlock");
    }

    Controls::TextBlock dateTextBlock =
        FindDescendantByName(clockContent, L"DateInnerTextBlock")
            .try_as<Controls::TextBlock>();
    if (dateTextBlock) {
        ApplyClockElementVisibility(dateTextBlock, hideDate);
    } else {
        Wh_Log(L"Failed to get DateInnerTextBlock");
    }

    FrameworkElement clockIconView = nullptr;
    FrameworkElement clockContainer = nullptr;
    bool resolved =
        ResolveClockIconView(clockContent, &clockIconView, &clockContainer);

    if (!resolved && hideClock && !g_unloading && attempt < 10) {
        // The content can be templated before it's attached to the visual
        // tree, so retry until the hosting icon view is available.
        clockContent.Dispatcher().TryRunAsync(
            winrt::Windows::UI::Core::CoreDispatcherPriority::Low,
            [clockContent, attempt]() {
                ApplyClockStyle(clockContent, attempt + 1);
            });
    } else {
        ApplyClockElementVisibility(clockContent, hideClock);

        if (clockIconView) {
            ApplyClockElementVisibility(clockIconView, hideClock);
        } else if (hideClock) {
            Wh_Log(L"Failed to get the clock icon view");
        }

        if (clockContainer) {
            ApplyClockElementVisibility(clockContainer, hideClock);
        }
    }
}

// ===== Win Duo Bar widget =====

// Every visual parameter of the widget. The settings page exposes these.
struct WidgetTuning {
    double ringDiameter = 35.0;     // DIP, outer diameter of the ring
    double ringThickness = 4.0;     // DIP, stroke width
    double ringSweepDeg = 240.0;    // degrees covered by the battery gauge
    double trackOpacityPct = 28.0;  // % opacity of the track
    double chargingNotchDeg = 48.0;  // degrees of the bolt notch
    double boltWidth = 7.2;          // DIP
    double boltHeight = 9.1;         // DIP
    double wifiHeight = 10.5;        // DIP
    double wifiWidth = 14.5;         // DIP
    double wifiThickness = 2.2;      // DIP
    double wifiOpeningDeg = 76.8;    // degrees of the Wi-Fi opening
    double wifiCenterPct = 41.0;     // % of the widget height
    double wifiChargingDrop = 1.5;   // DIP drop while charging
    double digitsSize = 6.66;        // DIP
    double timeRowPct = 73.0;        // % of the widget height
    double dateRowPct = 89.5;        // % of the widget height
    int digitsSpacing = 110;         // 1/1000 em
};
WidgetTuning g_tuning;

double GetDoubleSetting(PCWSTR name,
                        double defaultValue,
                        double minValue,
                        double maxValue) {
    PCWSTR value = Wh_GetStringSetting(name);
    double result = defaultValue;
    if (value && *value) {
        wchar_t* end = nullptr;
        double parsed = wcstod(value, &end);
        if (end && end != value) {
            result = parsed;
        }
    }
    Wh_FreeStringSetting(value);
    return std::clamp(result, minValue, maxValue);
}

bool TuningEquals(WidgetTuning const& a, WidgetTuning const& b) {
    auto close = [](double x, double y) {
        return std::abs(x - y) < 1e-6;
    };
    return close(a.ringDiameter, b.ringDiameter) &&
           close(a.ringThickness, b.ringThickness) &&
           close(a.ringSweepDeg, b.ringSweepDeg) &&
           close(a.trackOpacityPct, b.trackOpacityPct) &&
           close(a.chargingNotchDeg, b.chargingNotchDeg) &&
           close(a.boltWidth, b.boltWidth) &&
           close(a.boltHeight, b.boltHeight) &&
           close(a.wifiHeight, b.wifiHeight) &&
           close(a.wifiWidth, b.wifiWidth) &&
           close(a.wifiThickness, b.wifiThickness) &&
           close(a.wifiOpeningDeg, b.wifiOpeningDeg) &&
           close(a.wifiCenterPct, b.wifiCenterPct) &&
           close(a.wifiChargingDrop, b.wifiChargingDrop) &&
           close(a.digitsSize, b.digitsSize) &&
           close(a.timeRowPct, b.timeRowPct) &&
           close(a.dateRowPct, b.dateRowPct) &&
           a.digitsSpacing == b.digitsSpacing;
}

constexpr double kPi = 3.14159265358979323846;

// The gauge is centred on the top of the widget (270 degrees, screen
// coordinates: 0 = east, 90 = south, 270 = north).
constexpr double kRingTopDeg = 270.0;

// The wired/VPN icon still uses the system icon font glyph.
constexpr double kNetworkGlyphFrac = 0.310;
constexpr PCWSTR kGlyphEthernet = L"\uE839";

// Ring colours: charging = green, low battery = red, battery saver = yellow,
// otherwise the taskbar text colour.
constexpr uint32_t kColorCharging = 0xFF6EB85A;
constexpr uint32_t kColorLow = 0xFFFF453A;
constexpr uint32_t kColorSaver = 0xFFFFD60A;
constexpr int kLowBatteryPercent = 20;
constexpr uint32_t kColorLightText = 0xFF1C1C1E;

// Initial opacity of the translucent gauge track; the settings value takes
// over on the first frame.
constexpr uint8_t kTrackAlpha = 71;

// Internal proportions of the Wi-Fi symbol (the dot outline is traced from the
// original 09 icon, in units of the symbol height).
constexpr double kWifiArcCenterYFrac = 1.0502;
constexpr double kWifiArcOuterRadiusFrac = 0.9454;
constexpr double kWifiArcInnerRadiusFrac = 0.5974;

// Charging-state animation: the bolt grows out of the ring when charging
// starts, melts back into it when charging stops, and the colours cross-fade.
constexpr double kChargeAnimMs = 520.0;
constexpr double kAnimTickMs = 16.0;

struct BatteryInfo {
    bool valid = false;
    bool hasBattery = true;
    int percent = 100;
    bool plugged = false;
    bool charging = false;
    bool saver = false;
};

enum class NetworkKind {
    offline,
    wifi,
    ethernet,
};

struct NetworkInfo {
    NetworkKind kind = NetworkKind::offline;
    int bars = 0;
};

struct WidgetState {
    Controls::Canvas canvas{nullptr};
    Shapes::Path progressPath{nullptr};
    Shapes::Path trackPath{nullptr};
    Shapes::Path chargingBolt{nullptr};
    Controls::TextBlock networkText{nullptr};
    Controls::Canvas wifiSymbol{nullptr};
    Controls::TextBlock timeText{nullptr};
    Controls::TextBlock dateText{nullptr};
    Media::SolidColorBrush foregroundBrush{nullptr};
    Media::SolidColorBrush progressBrush{nullptr};
    Media::SolidColorBrush trackBrush{nullptr};
    winrt::weak_ref<Controls::Panel> hostPanel;
    winrt::weak_ref<FrameworkElement> nativeContent;
    DispatcherTimer timer{nullptr};
    winrt::event_token timerToken{};
    DispatcherTimer animTimer{nullptr};
    winrt::event_token animToken{};
    // 0 = notch open with the bolt out (charging), 1 = merged / no bolt.
    double notchT = 1.0;
    double notchTarget = 1.0;
    winrt::Windows::UI::Color ringColor{255, 255, 255, 255};
    winrt::Windows::UI::Color colorTarget{255, 255, 255, 255};
    winrt::Windows::UI::Color foreground{255, 255, 255, 255};
    bool ringColorSet = false;
    int size = 0;
    int64_t tick = 0;
    bool dark = true;
    BatteryInfo battery{};
    NetworkInfo network{};
    WidgetTuning tuning{};
    bool tuningValid = false;
    std::wstring timeString;
    std::wstring dateString;
};

// WinRT objects are released explicitly while unloading (like the other
// Windhawk mods do for their ref-counted state), so the global destructor is
// skipped on purpose.
[[clang::no_destroy]] std::optional<WidgetState> g_widget;

winrt::Windows::UI::Color MakeColor(uint32_t argb) {
    return winrt::Windows::UI::Color{
        static_cast<uint8_t>((argb >> 24) & 0xFF),
        static_cast<uint8_t>((argb >> 16) & 0xFF),
        static_cast<uint8_t>((argb >> 8) & 0xFF),
        static_cast<uint8_t>(argb & 0xFF)};
}

winrt::Windows::UI::Color WithAlpha(winrt::Windows::UI::Color color,
                                    uint8_t alpha) {
    color.A = alpha;
    return color;
}

std::wstring Pad2(int value) {
    return value < 10 ? L"0" + std::to_wstring(value) : std::to_wstring(value);
}

std::wstring FormatTimeString(const SYSTEMTIME& st) {
    // Four digits without a separator: 1357 means 13:57.
    int hour = st.wHour;

    if (g_settings.timeFormat == TimeFormatSetting::h12) {
        hour = st.wHour % 12;
        if (hour == 0) {
            hour = 12;
        }
    } else if (g_settings.timeFormat == TimeFormatSetting::autoFormat) {
        WCHAR buffer[64]{};
        if (GetTimeFormatEx(LOCALE_NAME_USER_DEFAULT, 0, &st, nullptr, buffer,
                            ARRAYSIZE(buffer)) > 0) {
            std::wstring systemFormat(buffer);
            if (systemFormat.find(L"AM") != std::wstring::npos ||
                systemFormat.find(L"PM") != std::wstring::npos) {
                hour = st.wHour % 12;
                if (hour == 0) {
                    hour = 12;
                }
            }
        }
    }

    return Pad2(hour) + Pad2(st.wMinute);
}

std::wstring FormatDateString(const SYSTEMTIME& st) {
    // Four digits without a separator: 1102 means November 2nd.
    if (g_settings.dateFormat == DateFormatSetting::dayMonth) {
        return Pad2(st.wDay) + Pad2(st.wMonth);
    }
    return Pad2(st.wMonth) + Pad2(st.wDay);
}

BatteryInfo ReadBatteryInfo() {
    BatteryInfo info{};

    SYSTEM_POWER_STATUS status{};
    if (!GetSystemPowerStatus(&status)) {
        return info;
    }

    info.valid = true;
    info.hasBattery = (status.BatteryFlag & 128) == 0;
    info.percent =
        status.BatteryLifePercent <= 100 ? status.BatteryLifePercent : 100;
    info.plugged = status.ACLineStatus == 1;
    info.charging = info.plugged && (status.BatteryFlag & 8) != 0;
    info.saver = status.SystemStatusFlag == 1;
    if (!info.hasBattery) {
        info.percent = 100;
    }

    return info;
}

NetworkInfo ReadNetworkInfo() {
    NetworkInfo info{};

    // Follow the system's active internet connection, so the widget shows the
    // same state as the Windows tray icon: Wi-Fi when the active profile is
    // wireless, the computer / link icon when it goes through Ethernet or a
    // VPN / proxy tunnel, and a dimmed icon when there is no connection.
    try {
        using namespace winrt::Windows::Networking::Connectivity;

        auto profile = NetworkInformation::GetInternetConnectionProfile();
        if (!profile) {
            return info;
        }

        if (profile.GetNetworkConnectivityLevel() ==
            NetworkConnectivityLevel::None) {
            return info;
        }

        if (profile.IsWlanConnectionProfile()) {
            info.kind = NetworkKind::wifi;
            auto bars = profile.GetSignalBars();
            if (bars) {
                info.bars = bars.Value();
            }
        } else {
            info.kind = NetworkKind::ethernet;
        }
    } catch (...) {
        Wh_Log(L"Failed to read the network info");
    }

    return info;
}

winrt::Windows::Foundation::Point PointOnCircle(double centerX,
                                               double centerY,
                                               double radius,
                                               double degrees) {
    double radians = degrees * kPi / 180.0;
    return {(float)(centerX + radius * std::cos(radians)),
            (float)(centerY + radius * std::sin(radians))};
}

void AppendArcGeometry(Media::PathGeometry const& geometry,
                       double centerX,
                       double centerY,
                       double radius,
                       double startDegrees,
                       double sweepDegrees) {
    if (sweepDegrees <= 0.01) {
        return;
    }

    Media::PathFigure figure;
    figure.StartPoint(PointOnCircle(centerX, centerY, radius, startDegrees));

    Media::ArcSegment segment;
    segment.Point(PointOnCircle(centerX, centerY, radius,
                                startDegrees + sweepDegrees));
    segment.Size({(float)radius, (float)radius});
    segment.SweepDirection(Media::SweepDirection::Clockwise);
    segment.IsLargeArc(sweepDegrees > 180.0);
    figure.Segments().Append(segment);

    geometry.Figures().Append(figure);
}

Media::PathGeometry MakeRingProgressGeometry(double centerX,
                                             double centerY,
                                             double radius,
                                             double level,
                                             double notchHalfDeg,
                                             double halfSweepDeg) {
    level = std::clamp(level, 0.0, 1.0);
    halfSweepDeg = std::clamp(halfSweepDeg, 45.0, 180.0);

    double progress = halfSweepDeg * 2.0 * level;

    Media::PathGeometry geometry;
    if (notchHalfDeg < 0.05) {
        // No notch: one continuous arc with round ends.
        AppendArcGeometry(geometry, centerX, centerY, radius,
                          kRingTopDeg - halfSweepDeg, progress);
        return geometry;
    }

    // Charging: split the gauge around the top notch so the bolt sits in it.
    double segSweep = std::max(0.5, halfSweepDeg - notchHalfDeg);
    double leftSweep = std::min(progress, segSweep);
    AppendArcGeometry(geometry, centerX, centerY, radius,
                      kRingTopDeg - halfSweepDeg, leftSweep);

    double rightSweep = std::min(
        std::max(0.0, progress - segSweep - notchHalfDeg * 2.0), segSweep);
    AppendArcGeometry(geometry, centerX, centerY, radius,
                      kRingTopDeg + notchHalfDeg, rightSweep);
    return geometry;
}

// Filled lightning bolt with rounded corners, drawn inside the top notch
// while charging.
Media::PathGeometry MakeBoltGeometry(double centerX,
                                     double centerY,
                                     double width,
                                     double height) {
    constexpr int kPointCount = 6;
    const double px[kPointCount] = {62, 0, 40, 26, 100, 58};
    const double py[kPointCount] = {0, 82, 82, 140, 58, 58};

    double x[kPointCount];
    double y[kPointCount];
    for (int i = 0; i < kPointCount; i++) {
        x[i] = centerX + (px[i] - 50.0) / 100.0 * width;
        y[i] = centerY + (py[i] - 70.0) / 140.0 * height;
    }

    double cornerRadius = height * 0.12;

    Media::PathFigure figure;
    bool started = false;
    for (int i = 0; i < kPointCount; i++) {
        int prev = (i + kPointCount - 1) % kPointCount;
        int next = (i + 1) % kPointCount;

        double v1x = x[prev] - x[i];
        double v1y = y[prev] - y[i];
        double v2x = x[next] - x[i];
        double v2y = y[next] - y[i];
        double len1 = std::sqrt(v1x * v1x + v1y * v1y);
        double len2 = std::sqrt(v2x * v2x + v2y * v2y);
        if (len1 < 1e-6 || len2 < 1e-6) {
            continue;
        }

        double radius = std::min({cornerRadius, len1 / 2.0, len2 / 2.0});
        double ax = x[i] + v1x / len1 * radius;
        double ay = y[i] + v1y / len1 * radius;
        double bx = x[i] + v2x / len2 * radius;
        double by = y[i] + v2y / len2 * radius;

        if (!started) {
            figure.StartPoint({(float)ax, (float)ay});
            started = true;
        } else {
            Media::LineSegment line;
            line.Point({(float)ax, (float)ay});
            figure.Segments().Append(line);
        }

        Media::QuadraticBezierSegment corner;
        corner.Point1({(float)x[i], (float)y[i]});
        corner.Point2({(float)bx, (float)by});
        figure.Segments().Append(corner);
    }

    figure.IsClosed(true);

    Media::PathGeometry geometry;
    geometry.Figures().Append(figure);
    return geometry;
}

Controls::TextBlock MakeTextBlock(double fontSize, double width) {
    Controls::TextBlock textBlock;
    textBlock.FontSize(fontSize);
    textBlock.Width(width);
    textBlock.TextAlignment(TextAlignment::Center);
    textBlock.TextWrapping(TextWrapping::NoWrap);
    textBlock.IsTextScaleFactorEnabled(false);
    return textBlock;
}

Shapes::Path MakeArcPath(Media::Brush brush,
                         double strokeThickness,
                         Media::Geometry geometry) {
    Shapes::Path path;
    path.Stroke(brush);
    path.StrokeThickness(strokeThickness);
    path.StrokeStartLineCap(Media::PenLineCap::Round);
    path.StrokeEndLineCap(Media::PenLineCap::Round);
    path.Stretch(Media::Stretch::None);
    path.Data(geometry);
    return path;
}

bool RemoveFromHost(Controls::Panel panel, UIElement element) {
    uint32_t index = 0;
    if (panel.Children().IndexOf(element, index)) {
        panel.Children().RemoveAt(index);
        return true;
    }
    return false;
}

void UpdateTheme(bool force);

void UpdateTimeAndDate(bool force) {
    if (!g_widget || !g_widget->canvas) {
        return;
    }

    auto& widget = *g_widget;

    SYSTEMTIME st{};
    GetLocalTime(&st);

    std::wstring timeString = FormatTimeString(st);
    if (force || timeString != widget.timeString) {
        widget.timeString = timeString;
        if (widget.timeText) {
            widget.timeText.Text(timeString);
        }
    }

    std::wstring dateString = FormatDateString(st);
    if (force || dateString != widget.dateString) {
        widget.dateString = dateString;
        if (widget.dateText) {
            widget.dateText.Text(dateString);
        }
    }
}


Media::PathGeometry MakeWifiArcGeometry(double centerX,
                                        double centerY,
                                        double midRadius,
                                        double halfDeg) {
    Media::PathGeometry geometry;
    AppendArcGeometry(geometry, centerX, centerY, midRadius,
                      kRingTopDeg - halfDeg, halfDeg * 2.0);
    return geometry;
}

Media::PathGeometry MakeWifiDotGeometry(double height, double halfDeg) {
    // Dot outline traced from the original 09 icon; coordinates are relative
    // to the icon ink box, in units of its ink height. The top corners are
    // placed on the same rays as the arc ends so everything lines up.
    static const double xs[] = {
        0.6661, 0.5651, 0.5147, 0.4902, 0.4805, 0.4756, 0.4772, 0.4853,
        0.5016, 0.5212, 0.5407, 0.5603, 0.5798, 0.5993, 0.6189, 0.6450,
        0.6743, 0.7085, 0.7378, 0.7638, 0.7834, 0.8029, 0.8225, 0.8420,
        0.8616, 0.8811, 0.8974, 0.9039, 0.9072, 0.9023, 0.8925, 0.8681,
        0.8192, 0.7166};
    static const double ys[] = {
        0.6954, 0.7150, 0.7345, 0.7541, 0.7736, 0.7932, 0.8127, 0.8322,
        0.8518, 0.8713, 0.8909, 0.9104, 0.9300, 0.9495, 0.9691, 0.9886,
        0.9984, 0.9984, 0.9886, 0.9691, 0.9495, 0.9300, 0.9104, 0.8909,
        0.8713, 0.8518, 0.8322, 0.8127, 0.7932, 0.7736, 0.7541, 0.7345,
        0.7150, 0.6954};

    constexpr int kCount = sizeof(xs) / sizeof(xs[0]);

    // Horizontal scale follows the original 09 geometry for the standard
    // opening; a different opening angle squashes/stretches it horizontally.
    double angle = std::clamp(halfDeg, 10.0, 90.0) * kPi / 180.0;
    double scaleX = std::sin(38.4 * kPi / 180.0) / std::sin(angle);
    if (!std::isfinite(scaleX) || scaleX <= 0.0) {
        scaleX = 1.0;
    }

    Media::PathFigure figure;
    figure.StartPoint({(float)(xs[0] * height * scaleX),
                       (float)(ys[0] * height)});
    for (int i = 1; i < kCount; i++) {
        Media::LineSegment segment;
        segment.Point({(float)(xs[i] * height * scaleX),
                       (float)(ys[i] * height)});
        figure.Segments().Append(segment);
    }
    figure.IsClosed(true);

    Media::PathGeometry geometry;
    geometry.Figures().Append(figure);
    return geometry;
}

double SmoothStep(double t) {
    t = std::clamp(t, 0.0, 1.0);
    return t * t * (3.0 - 2.0 * t);
}

winrt::Windows::UI::Color LerpColor(winrt::Windows::UI::Color from,
                                    winrt::Windows::UI::Color to,
                                    double t) {
    auto mix = [t](uint8_t a, uint8_t b) {
        return static_cast<uint8_t>(std::lround(a + (b - a) * t));
    };
    return winrt::Windows::UI::Color{mix(from.A, to.A), mix(from.R, to.R),
                                     mix(from.G, to.G), mix(from.B, to.B)};
}

double ColorDistance(winrt::Windows::UI::Color a,
                     winrt::Windows::UI::Color b) {
    double dr = static_cast<double>(a.R) - b.R;
    double dg = static_cast<double>(a.G) - b.G;
    double db = static_cast<double>(a.B) - b.B;
    return std::sqrt(dr * dr + dg * dg + db * db);
}

winrt::Windows::UI::Color RingColorTarget() {
    auto& widget = *g_widget;
    if (widget.battery.plugged) {
        return MakeColor(kColorCharging);
    }
    if (widget.battery.hasBattery &&
        widget.battery.percent <= kLowBatteryPercent) {
        return MakeColor(kColorLow);
    }
    if (widget.battery.saver) {
        return MakeColor(kColorSaver);
    }
    return widget.foreground;
}

// Draws one animation frame: ring geometry with the current notch, ring color
// and the bolt that grows out of / melts back into the ring.
void ApplyChargeFrame() {
    if (!g_widget || !g_widget->canvas || !g_widget->progressPath) {
        return;
    }

    auto& widget = *g_widget;

    double size = widget.canvas.Width();
    double centerX = size / 2.0;
    double centerY = size / 2.0;
    double stroke = std::min(g_tuning.ringThickness, size * 0.3);
    double radius = std::max(1.0, g_tuning.ringDiameter / 2.0 - stroke / 2.0);
    double halfSweepDeg = g_tuning.ringSweepDeg / 2.0;
    double notchHalfDeg = g_tuning.chargingNotchDeg / 2.0;

    double openFactor = 1.0 - SmoothStep(widget.notchT);

    widget.progressPath.Data(MakeRingProgressGeometry(
        centerX, centerY, radius,
        widget.battery.hasBattery ? widget.battery.percent / 100.0 : 1.0,
        notchHalfDeg * openFactor, halfSweepDeg));

    if (widget.progressBrush) {
        widget.progressBrush.Color(widget.ringColor);
    }

    // The track follows the same notch and color animation, so the bolt never
    // sits on top of a translucent arc.
    if (widget.trackPath) {
        widget.trackPath.Data(MakeRingProgressGeometry(
            centerX, centerY, radius, 1.0, notchHalfDeg * openFactor,
            halfSweepDeg));
    }
    if (widget.trackBrush) {
        uint8_t alpha = static_cast<uint8_t>(std::lround(
            std::clamp(g_tuning.trackOpacityPct, 0.0, 100.0) / 100.0 * 255.0));
        widget.trackBrush.Color(WithAlpha(widget.ringColor, alpha));
    }

    if (widget.wifiSymbol) {
        double wifiTop = size * (g_tuning.wifiCenterPct / 100.0) -
                         g_tuning.wifiHeight / 2.0;
        double drop = g_tuning.wifiChargingDrop * openFactor;
        Controls::Canvas::SetTop(widget.wifiSymbol, wifiTop + drop);
    }

    if (widget.chargingBolt) {
        if (openFactor > 0.01) {
            double boltWidth = g_tuning.boltWidth * openFactor;
            double boltHeight = g_tuning.boltHeight * openFactor;
            double boltCenterY = centerY - radius + stroke / 2.0 -
                                 stroke / 2.0 * SmoothStep(widget.notchT);
            widget.chargingBolt.Data(MakeBoltGeometry(
                centerX, boltCenterY, boltWidth, boltHeight));
            widget.chargingBolt.Visibility(Visibility::Visible);
            widget.chargingBolt.Opacity(std::min(1.0, openFactor * 1.6));
        } else {
            widget.chargingBolt.Visibility(Visibility::Collapsed);
        }
    }
}

void StopChargeAnim() {
    if (!g_widget || !g_widget->animTimer) {
        return;
    }

    g_widget->animTimer.Stop();
}

void StartChargeAnim() {
    if (!g_widget || !g_widget->animTimer) {
        return;
    }

    g_widget->animTimer.Start();
}

void OnAnimTick() {
    if (g_unloading || !g_widget || !g_widget->canvas) {
        return;
    }

    auto& widget = *g_widget;

    bool done = true;

    if (widget.notchT != widget.notchTarget) {
        done = false;
        double step = kAnimTickMs / kChargeAnimMs;
        if (widget.notchT < widget.notchTarget) {
            widget.notchT = std::min(widget.notchT + step, widget.notchTarget);
        } else {
            widget.notchT = std::max(widget.notchT - step, widget.notchTarget);
        }
    }

    if (ColorDistance(widget.ringColor, widget.colorTarget) > 1.5) {
        done = false;
        widget.ringColor =
            LerpColor(widget.ringColor, widget.colorTarget, 0.22);
    } else {
        widget.ringColor = widget.colorTarget;
    }

    ApplyChargeFrame();

    if (done) {
        StopChargeAnim();
    }
}

void UpdateBattery(bool force) {
    if (!g_widget || !g_widget->canvas || !g_widget->progressPath) {
        return;
    }

    auto& widget = *g_widget;

    BatteryInfo info = ReadBatteryInfo();
    bool changed = force || info.valid != widget.battery.valid ||
                   info.hasBattery != widget.battery.hasBattery ||
                   info.percent != widget.battery.percent ||
                   info.plugged != widget.battery.plugged ||
                   info.charging != widget.battery.charging ||
                   info.saver != widget.battery.saver;
    widget.battery = info;

    if (!changed) {
        return;
    }

    widget.notchTarget = info.plugged ? 0.0 : 1.0;
    widget.colorTarget = RingColorTarget();

    if (!widget.ringColorSet) {
        // First layout after building: show the current state right away.
        widget.ringColorSet = true;
        widget.notchT = widget.notchTarget;
        widget.ringColor = widget.colorTarget;
        ApplyChargeFrame();
        return;
    }

    if (widget.notchT != widget.notchTarget ||
        ColorDistance(widget.ringColor, widget.colorTarget) > 1.5) {
        StartChargeAnim();
    }
}

void UpdateNetwork(bool force) {
    if (!g_widget || !g_widget->canvas || !g_widget->networkText ||
        !g_widget->wifiSymbol) {
        return;
    }

    auto& widget = *g_widget;

    NetworkInfo info = ReadNetworkInfo();
    bool changed = force || info.kind != widget.network.kind ||
                   info.bars != widget.network.bars;
    widget.network = info;

    if (!changed) {
        return;
    }

    bool show = g_settings.showNetwork;
    bool showWifi = show && info.kind != NetworkKind::ethernet;
    bool showEthernet = show && info.kind == NetworkKind::ethernet;
    double wifiOpacity = info.kind == NetworkKind::offline ? 0.32 : 1.0;

    widget.wifiSymbol.Visibility(showWifi ? Visibility::Visible
                                          : Visibility::Collapsed);
    widget.wifiSymbol.Opacity(wifiOpacity);
    widget.networkText.Visibility(showEthernet ? Visibility::Visible
                                               : Visibility::Collapsed);
    widget.networkText.Opacity(1.0);
}

void UpdateTheme(bool force) {
    if (!g_widget || !g_widget->canvas) {
        return;
    }

    auto& widget = *g_widget;

    bool dark = widget.canvas.ActualTheme() != ElementTheme::Light;
    if (!force && dark == widget.dark) {
        return;
    }

    widget.dark = dark;

    widget.foreground =
        dark ? MakeColor(0xFFFFFFFF) : MakeColor(kColorLightText);
    widget.foregroundBrush.Color(widget.foreground);

    widget.colorTarget = RingColorTarget();

    if (!widget.ringColorSet) {
        widget.ringColor = widget.colorTarget;
        ApplyChargeFrame();
    } else if (ColorDistance(widget.ringColor, widget.colorTarget) > 1.5) {
        StartChargeAnim();
    }
}


void RefreshAll(bool force) {
    UpdateTheme(force);
    UpdateTimeAndDate(force);
    UpdateBattery(force);
    UpdateNetwork(force);
}

void OnTimerTick() {
    if (g_unloading || !g_widget || !g_widget->canvas) {
        return;
    }

    auto& widget = *g_widget;
    widget.tick++;

    UpdateTheme(false);
    UpdateTimeAndDate(false);
    if (widget.tick % 2 == 0) {
        UpdateBattery(false);
    }
    if (widget.tick % 2 == 1) {
        UpdateNetwork(false);
    }
}


void EnsureTimer() {
    if (!g_widget || g_widget->timer) {
        return;
    }

    auto timer = DispatcherTimer();
    timer.Interval(winrt::Windows::Foundation::TimeSpan{
        std::chrono::seconds(1)});
    g_widget->timerToken = timer.Tick(
        [](winrt::Windows::Foundation::IInspectable const&,
           winrt::Windows::Foundation::IInspectable const&) { OnTimerTick(); });
    g_widget->timer = timer;
    timer.Start();
}

void StopTimer() {
    if (!g_widget || !g_widget->timer) {
        return;
    }

    g_widget->timer.Stop();
    g_widget->timer.Tick(g_widget->timerToken);
    g_widget->timer = nullptr;
    g_widget->timerToken = {};
}

void BuildWidget() {
    if (!g_widget) {
        g_widget.emplace();
    }

    auto& widget = *g_widget;

    StopTimer();
    StopChargeAnim();
    widget.ringColorSet = false;
    widget.notchT = 1.0;
    widget.notchTarget = 1.0;

    int requestedSize = std::clamp(g_settings.size, 28, 48);
    double size = requestedSize;
    double centerX = size / 2.0;
    double centerY = size / 2.0;
    double stroke = std::min(g_tuning.ringThickness, size * 0.3);
    double radius = std::max(1.0, g_tuning.ringDiameter / 2.0 - stroke / 2.0);
    double halfSweepDeg = g_tuning.ringSweepDeg / 2.0;
    double notchHalfDeg = g_tuning.chargingNotchDeg / 2.0;

    Controls::Canvas canvas;
    canvas.Width(size);
    canvas.Height(size);
    canvas.IsHitTestVisible(false);
    canvas.HorizontalAlignment(HorizontalAlignment::Center);
    canvas.VerticalAlignment(VerticalAlignment::Center);

    auto foregroundBrush = Media::SolidColorBrush(MakeColor(0xFFFFFFFF));
    auto progressBrush = Media::SolidColorBrush(MakeColor(0xFFFFFFFF));
    auto trackBrush = Media::SolidColorBrush(
        WithAlpha(MakeColor(0xFFFFFFFF), kTrackAlpha));

    // Translucent track: the whole 240 degree gauge always stays visible so a
    // low battery level never leaves a gaping hole in the ring.
    auto trackPath = MakeArcPath(
        trackBrush, stroke,
        MakeRingProgressGeometry(centerX, centerY, radius, 1.0, 0.0,
                                 halfSweepDeg));
    Controls::Canvas::SetLeft(trackPath, 0);
    Controls::Canvas::SetTop(trackPath, 0);

    // Battery ring: 240 degree gauge, green while charging.
    auto progressPath = MakeArcPath(
        progressBrush, stroke,
        MakeRingProgressGeometry(centerX, centerY, radius, 1.0, 0.0,
                                 halfSweepDeg));
    Controls::Canvas::SetLeft(progressPath, 0);
    Controls::Canvas::SetTop(progressPath, 0);

    // Charging bolt in the top notch, shown only while charging.
    double boltWidth = g_tuning.boltWidth;
    double boltHeight = g_tuning.boltHeight;
    Shapes::Path chargingBolt;
    chargingBolt.Fill(foregroundBrush);
    chargingBolt.Stretch(Media::Stretch::None);
    chargingBolt.Data(MakeBoltGeometry(centerX,
                                       centerY - radius + stroke / 2.0,
                                       boltWidth, boltHeight));
    chargingBolt.Visibility(Visibility::Collapsed);
    Controls::Canvas::SetLeft(chargingBolt, 0);
    Controls::Canvas::SetTop(chargingBolt, 0);

    // Custom Wi-Fi symbol in the middle of the ring: two thick rounded arcs
    // plus a rounded triangle dot, built around one shared circle center.
    double wifiHeight = g_tuning.wifiHeight;
    double wifiWidth = g_tuning.wifiWidth;
    double wifiCenterYPos = size * (g_tuning.wifiCenterPct / 100.0);
    double wifiTop = wifiCenterYPos - wifiHeight / 2.0;
    double wifiArcCenterX = wifiWidth / 2.0;
    double wifiArcCenterY = wifiHeight * kWifiArcCenterYFrac;
    double wifiArcStroke = std::min(g_tuning.wifiThickness, wifiHeight * 0.45);
    double wifiHalfDeg = std::clamp(g_tuning.wifiOpeningDeg, 20.0, 180.0) / 2.0;

    Controls::Canvas wifiSymbol;
    wifiSymbol.Width(wifiWidth);
    wifiSymbol.Height(wifiHeight);
    wifiSymbol.IsHitTestVisible(false);

    auto wifiArcOuter = MakeArcPath(
        foregroundBrush, wifiArcStroke,
        MakeWifiArcGeometry(wifiArcCenterX, wifiArcCenterY,
                            wifiHeight * kWifiArcOuterRadiusFrac, wifiHalfDeg));
    Controls::Canvas::SetLeft(wifiArcOuter, 0);
    Controls::Canvas::SetTop(wifiArcOuter, 0);

    auto wifiArcInner = MakeArcPath(
        foregroundBrush, wifiArcStroke,
        MakeWifiArcGeometry(wifiArcCenterX, wifiArcCenterY,
                            wifiHeight * kWifiArcInnerRadiusFrac, wifiHalfDeg));
    Controls::Canvas::SetLeft(wifiArcInner, 0);
    Controls::Canvas::SetTop(wifiArcInner, 0);

    Shapes::Path wifiDot;
    wifiDot.Fill(foregroundBrush);
    wifiDot.Stretch(Media::Stretch::None);
    wifiDot.Data(MakeWifiDotGeometry(wifiHeight, wifiHalfDeg));
    Controls::Canvas::SetLeft(wifiDot, 0);
    Controls::Canvas::SetTop(wifiDot, 0);

    wifiSymbol.Children().Append(wifiArcOuter);
    wifiSymbol.Children().Append(wifiArcInner);
    wifiSymbol.Children().Append(wifiDot);
    Controls::Canvas::SetLeft(wifiSymbol, centerX - wifiWidth / 2.0);
    Controls::Canvas::SetTop(wifiSymbol, wifiTop);

    // Ethernet still uses the system icon font glyph.
    double networkFont = size * kNetworkGlyphFrac;
    auto networkText = MakeTextBlock(networkFont, size);
    networkText.FontFamily(Media::FontFamily(L"Segoe Fluent Icons"));
    networkText.Text(kGlyphEthernet);
    networkText.Foreground(foregroundBrush);
    networkText.Visibility(Visibility::Collapsed);
    Controls::Canvas::SetLeft(networkText, 0);
    Controls::Canvas::SetTop(networkText, centerY - networkFont * 0.72);

    // Two rows of four bold digits with wide letter spacing.
    double digitsFont = g_tuning.digitsSize;

    auto timeText = MakeTextBlock(digitsFont, size);
    timeText.Foreground(foregroundBrush);
    timeText.FontWeight(winrt::Windows::UI::Text::FontWeights::Bold());
    timeText.CharacterSpacing(g_tuning.digitsSpacing);
    Controls::Canvas::SetLeft(timeText, 0);
    Controls::Canvas::SetTop(
        timeText, size * (g_tuning.timeRowPct / 100.0) - digitsFont * 0.64);

    auto dateText = MakeTextBlock(digitsFont, size);
    dateText.Foreground(foregroundBrush);
    dateText.FontWeight(winrt::Windows::UI::Text::FontWeights::Bold());
    dateText.CharacterSpacing(g_tuning.digitsSpacing);
    Controls::Canvas::SetLeft(dateText, 0);
    Controls::Canvas::SetTop(
        dateText, size * (g_tuning.dateRowPct / 100.0) - digitsFont * 0.64);

    canvas.Children().Append(trackPath);
    canvas.Children().Append(progressPath);
    canvas.Children().Append(wifiSymbol);
    canvas.Children().Append(networkText);
    canvas.Children().Append(chargingBolt);
    canvas.Children().Append(timeText);
    canvas.Children().Append(dateText);

    widget.canvas = canvas;
    widget.progressPath = progressPath;
    widget.trackPath = trackPath;
    widget.chargingBolt = chargingBolt;
    widget.networkText = networkText;
    widget.wifiSymbol = wifiSymbol;
    widget.timeText = timeText;
    widget.dateText = dateText;
    widget.foregroundBrush = foregroundBrush;
    widget.progressBrush = progressBrush;
    widget.trackBrush = trackBrush;
    widget.foreground = MakeColor(0xFFFFFFFF);
    widget.size = requestedSize;
    widget.timeString.clear();
    widget.dateString.clear();
    widget.tick = 0;
    widget.battery = BatteryInfo{};
    widget.network = NetworkInfo{};
    widget.tuning = g_tuning;
    widget.tuningValid = true;

    dateText.Visibility(g_settings.showDate ? Visibility::Visible
                                            : Visibility::Collapsed);

    auto animTimer = DispatcherTimer();
    animTimer.Interval(winrt::Windows::Foundation::TimeSpan{
        std::chrono::milliseconds(16)});
    widget.animToken = animTimer.Tick(
        [](winrt::Windows::Foundation::IInspectable const&,
           winrt::Windows::Foundation::IInspectable const&) { OnAnimTick(); });
    widget.animTimer = animTimer;

    UpdateNetwork(true);
}


void CleanupWidget() {
    if (!g_widget) {
        return;
    }

    auto& widget = *g_widget;

    StopTimer();

    if (widget.animTimer) {
        widget.animTimer.Stop();
        widget.animTimer.Tick(widget.animToken);
        widget.animTimer = nullptr;
        widget.animToken = {};
    }
    widget.ringColorSet = false;
    widget.notchT = 1.0;
    widget.notchTarget = 1.0;

    if (widget.canvas) {
        if (auto host = widget.hostPanel.get()) {
            RemoveFromHost(host, widget.canvas);
        }
    }

    if (auto nativeContent = widget.nativeContent.get()) {
        nativeContent.Visibility(Visibility::Visible);
    }

    widget.canvas = nullptr;
    widget.progressPath = nullptr;
    widget.trackPath = nullptr;
    widget.chargingBolt = nullptr;
    widget.networkText = nullptr;
    widget.wifiSymbol = nullptr;
    widget.timeText = nullptr;
    widget.dateText = nullptr;
    widget.foregroundBrush = nullptr;
    widget.progressBrush = nullptr;
    widget.trackBrush = nullptr;
    widget.hostPanel = nullptr;
    widget.nativeContent = nullptr;
    widget.size = 0;
    widget.timeString.clear();
    widget.dateString.clear();
}


void ApplyClockWidget(FrameworkElement nativeClockContent) {
    if (g_unloading) {
        CleanupWidget();
        return;
    }

    if (!g_settings.enabled) {
        CleanupWidget();
        return;
    }

    auto parent = Media::VisualTreeHelper::GetParent(nativeClockContent)
                      .try_as<Controls::Panel>();
    if (!parent) {
        Wh_Log(L"Failed to get the clock content parent");
        return;
    }

    if (!g_widget) {
        g_widget.emplace();
    }
    auto& widget = *g_widget;

    int requestedSize = std::clamp(g_settings.size, 28, 48);
    bool tuningChanged =
        !widget.tuningValid || !TuningEquals(widget.tuning, g_tuning);
    if (!widget.canvas || widget.size != requestedSize || tuningChanged) {
        if (widget.canvas) {
            if (auto host = widget.hostPanel.get()) {
                RemoveFromHost(host, widget.canvas);
            }
        }
        BuildWidget();
    }

    if (!widget.canvas) {
        return;
    }

    if (widget.hostPanel.get() != parent) {
        if (auto oldHost = widget.hostPanel.get()) {
            RemoveFromHost(oldHost, widget.canvas);
        }
        parent.Children().Append(widget.canvas);
        widget.hostPanel = parent;
    }

    nativeClockContent.Visibility(Visibility::Collapsed);
    widget.nativeContent = nativeClockContent;

    if (widget.dateText) {
        widget.dateText.Visibility(g_settings.showDate ? Visibility::Visible
                                                       : Visibility::Collapsed);
    }
    EnsureTimer();
    RefreshAll(true);
}


// ===== End of Win Duo Bar widget =====

using DateTimeIconContent_OnApplyTemplate_t = HRESULT(WINAPI*)(void* pThis);
DateTimeIconContent_OnApplyTemplate_t
    DateTimeIconContent_OnApplyTemplate_Original;

HRESULT WINAPI DateTimeIconContent_OnApplyTemplate_Hook(void* pThis) {
    Wh_Log(L">");

    HRESULT ret = DateTimeIconContent_OnApplyTemplate_Original(pThis);

    try {
        FrameworkElement clockContent = nullptr;
        ((IUnknown*)pThis)
            ->QueryInterface(winrt::guid_of<FrameworkElement>(),
                             winrt::put_abi(clockContent));
        if (clockContent) {
            ApplyClockWidget(clockContent);
            if (!g_settings.enabled) {
                ApplyClockStyle(clockContent, 0);
            }
        }
    } catch (...) {
        HRESULT hr = winrt::to_hresult();
        Wh_Log(L"Error %08X", hr);
    }

    return ret;
}

// MinWidth/MaxWidth take precedence over an explicit Width, so they override
// the width without having to replace the Width value itself. The local values
// are saved the first time an element is seen, for restoring in ApplySettings.
void OverrideShowDesktopWidth(FrameworkElement element, double width) {
    bool managed = false;
    for (const auto& state : g_showDesktopWidthStates) {
        if (state.element.get() == element) {
            managed = true;
            break;
        }
    }

    if (!managed) {
        g_showDesktopWidthStates.push_back({
            .element = element,
            .savedMinWidth =
                element.ReadLocalValue(FrameworkElement::MinWidthProperty())
                    .try_as<double>(),
            .savedMaxWidth =
                element.ReadLocalValue(FrameworkElement::MaxWidthProperty())
                    .try_as<double>(),
        });
    }

    element.MinWidth(width);
    element.MaxWidth(width);
}

void ApplyShowDesktopStyle(FrameworkElement systemTrayIconElement) {
    if (g_unloading) {
        // Restored in ApplySettings.
        return;
    }

    auto showDesktopStack =
        GetParentElementByName(systemTrayIconElement, L"ShowDesktopStack");
    if (!showDesktopStack) {
        Wh_Log(L"Failed");
        return;
    }

    // Drop any state for elements that no longer exist.
    std::erase_if(g_showDesktopWidthStates, [](const ShowDesktopWidthState& s) {
        return !s.element.get();
    });

    int width = g_settings.showDesktopButtonWidth;
    Wh_Log(L"Show desktop button, width=%d", width);

    OverrideShowDesktopWidth(systemTrayIconElement, width);
    OverrideShowDesktopWidth(showDesktopStack, width);

    // The container grid has an explicit width, which keeps its content at the
    // default size regardless of the width of the icon view.
    auto containerGrid =
        FindChildByName(systemTrayIconElement, L"ContainerGrid");
    if (containerGrid) {
        OverrideShowDesktopWidth(containerGrid, width);
    } else {
        Wh_Log(L"Failed to get ContainerGrid");
    }
}

bool ApplyMainStackStyle(FrameworkElement container) {
    FrameworkElement stackPanel = nullptr;

    FrameworkElement child = container;
    if ((child = FindChildByName(child, L"Content")) &&
        (child = FindChildByName(child, L"IconStack")) &&
        (child = FindChildByClassName(
             child, L"Windows.UI.Xaml.Controls.ItemsPresenter")) &&
        (child = FindChildByClassName(
             child, L"Windows.UI.Xaml.Controls.StackPanel"))) {
        stackPanel = child;
    }

    if (!stackPanel) {
        return false;
    }

    EnumChildElements(stackPanel, [](FrameworkElement child) {
        auto childClassName = winrt::get_class_name(child);
        if (childClassName != L"Windows.UI.Xaml.Controls.ContentPresenter") {
            Wh_Log(L"Unsupported class name %s of child",
                   childClassName.c_str());
            return false;
        }

        FrameworkElement systemTrayIconElement =
            FindChildByName(child, L"SystemTrayIcon");
        if (!systemTrayIconElement) {
            Wh_Log(L"Failed to get SystemTrayIcon of child");
            return false;
        }

        ApplyMainStackIconViewStyle(systemTrayIconElement);
        return false;
    });

    return true;
}

bool ApplyNonActivatableStackStyle(FrameworkElement container) {
    FrameworkElement stackPanel = nullptr;

    FrameworkElement child = container;
    if ((child = FindChildByName(child, L"Content")) &&
        (child = FindChildByName(child, L"IconStack")) &&
        (child = FindChildByClassName(
             child, L"Windows.UI.Xaml.Controls.ItemsPresenter")) &&
        (child = FindChildByClassName(
             child, L"Windows.UI.Xaml.Controls.StackPanel"))) {
        stackPanel = child;
    }

    if (!stackPanel) {
        return false;
    }

    EnumChildElements(stackPanel, [](FrameworkElement child) {
        auto childClassName = winrt::get_class_name(child);
        if (childClassName != L"Windows.UI.Xaml.Controls.ContentPresenter") {
            Wh_Log(L"Unsupported class name %s of child",
                   childClassName.c_str());
            return false;
        }

        FrameworkElement systemTrayIconElement =
            FindChildByName(child, L"SystemTrayIcon");
        if (!systemTrayIconElement) {
            Wh_Log(L"Failed to get SystemTrayIcon of child");
            return false;
        }

        ApplyNonActivatableStackIconViewStyle(systemTrayIconElement);
        return false;
    });

    return true;
}

bool ApplyControlCenterButtonStyle(FrameworkElement controlCenterButton) {
    FrameworkElement stackPanel = nullptr;

    FrameworkElement child = controlCenterButton;
    if ((child =
             FindChildByClassName(child, L"Windows.UI.Xaml.Controls.Grid")) &&
        (child = FindChildByName(child, L"ContentPresenter")) &&
        (child = FindChildByClassName(
             child, L"Windows.UI.Xaml.Controls.ItemsPresenter")) &&
        (child = FindChildByClassName(
             child, L"Windows.UI.Xaml.Controls.StackPanel"))) {
        stackPanel = child;
    }

    if (!stackPanel) {
        return false;
    }

    EnumChildElements(stackPanel, [](FrameworkElement child) {
        auto childClassName = winrt::get_class_name(child);
        if (childClassName != L"Windows.UI.Xaml.Controls.ContentPresenter") {
            Wh_Log(L"Unsupported class name %s of child",
                   childClassName.c_str());
            return false;
        }

        FrameworkElement systemTrayIconElement =
            FindChildByName(child, L"SystemTrayIcon");
        if (!systemTrayIconElement) {
            Wh_Log(L"Failed to get SystemTrayIcon of child");
            return false;
        }

        ApplyControlCenterButtonIconStyle(systemTrayIconElement);
        return false;
    });

    return true;
}

bool ApplyNotificationCenterButtonStyle(FrameworkElement controlCenterButton) {
    FrameworkElement stackPanel = nullptr;

    FrameworkElement child = controlCenterButton;
    if ((child =
             FindChildByClassName(child, L"Windows.UI.Xaml.Controls.Grid")) &&
        (child = FindChildByName(child, L"ContentPresenter")) &&
        (child = FindChildByClassName(
             child, L"Windows.UI.Xaml.Controls.ItemsPresenter")) &&
        (child = FindChildByClassName(
             child, L"Windows.UI.Xaml.Controls.StackPanel"))) {
        stackPanel = child;
    }

    if (!stackPanel) {
        return false;
    }

    EnumChildElements(stackPanel, [](FrameworkElement child) {
        auto childClassName = winrt::get_class_name(child);
        if (childClassName != L"Windows.UI.Xaml.Controls.ContentPresenter") {
            Wh_Log(L"Unsupported class name %s of child",
                   childClassName.c_str());
            return false;
        }

        FrameworkElement systemTrayIconElement =
            FindChildByName(child, L"SystemTrayIcon");
        if (!systemTrayIconElement) {
            Wh_Log(L"Failed to get SystemTrayIcon of child");
            return false;
        }

        ApplyBellIconStyle(systemTrayIconElement);
        return false;
    });

    return true;
}

bool ApplyShowDesktopStackStyle(FrameworkElement container) {
    FrameworkElement stackPanel = nullptr;

    FrameworkElement child = container;
    if ((child = FindChildByName(child, L"Content")) &&
        (child = FindChildByName(child, L"IconStack")) &&
        (child = FindChildByClassName(
             child, L"Windows.UI.Xaml.Controls.ItemsPresenter")) &&
        (child = FindChildByClassName(
             child, L"Windows.UI.Xaml.Controls.StackPanel"))) {
        stackPanel = child;
    }

    if (!stackPanel) {
        return false;
    }

    EnumChildElements(stackPanel, [](FrameworkElement child) {
        auto childClassName = winrt::get_class_name(child);
        if (childClassName != L"Windows.UI.Xaml.Controls.ContentPresenter") {
            Wh_Log(L"Unsupported class name %s of child",
                   childClassName.c_str());
            return false;
        }

        FrameworkElement systemTrayIconElement =
            FindChildByName(child, L"SystemTrayIcon");
        if (!systemTrayIconElement) {
            Wh_Log(L"Failed to get SystemTrayIcon of child");
            return false;
        }

        ApplyShowDesktopStyle(systemTrayIconElement);
        return false;
    });

    return true;
}

bool ApplyStyle(XamlRoot xamlRoot) {
    FrameworkElement systemTrayFrameGrid = nullptr;

    FrameworkElement child = xamlRoot.Content().try_as<FrameworkElement>();
    if (child &&
        (child = FindChildByClassName(child, L"SystemTray.SystemTrayFrame")) &&
        (child = FindChildByName(child, L"SystemTrayFrameGrid"))) {
        systemTrayFrameGrid = child;
    }

    if (!systemTrayFrameGrid) {
        return false;
    }

    bool somethingSucceeded = false;

    FrameworkElement mainStack =
        FindChildByName(systemTrayFrameGrid, L"MainStack");
    if (mainStack) {
        somethingSucceeded |= ApplyMainStackStyle(mainStack);
    }

    FrameworkElement nonActivatableStack =
        FindChildByName(systemTrayFrameGrid, L"NonActivatableStack");
    if (nonActivatableStack) {
        somethingSucceeded |=
            ApplyNonActivatableStackStyle(nonActivatableStack);
    }

    FrameworkElement controlCenterButton =
        FindChildByName(systemTrayFrameGrid, L"ControlCenterButton");
    if (controlCenterButton) {
        somethingSucceeded |=
            ApplyControlCenterButtonStyle(controlCenterButton);
    }

    FrameworkElement notificationCenterButton =
        FindChildByName(systemTrayFrameGrid, L"NotificationCenterButton");
    if (notificationCenterButton) {
        somethingSucceeded |=
            ApplyNotificationCenterButtonStyle(notificationCenterButton);
    }

    FrameworkElement showDesktopStack =
        FindChildByName(systemTrayFrameGrid, L"ShowDesktopStack");
    if (showDesktopStack) {
        somethingSucceeded |= ApplyShowDesktopStackStyle(showDesktopStack);
    }

    FrameworkElement clockContent = FindDescendantByClassName(
        systemTrayFrameGrid, L"SystemTray.DateTimeIconContent");
    if (clockContent) {
        ApplyClockWidget(clockContent);
        if (!g_settings.enabled) {
            ApplyClockStyle(clockContent, 0);
        }
        somethingSucceeded = true;
    }

    return somethingSucceeded;
}

using IconView_IconView_t = void*(WINAPI*)(void* pThis);
IconView_IconView_t IconView_IconView_Original;
void* WINAPI IconView_IconView_Hook(void* pThis) {
    Wh_Log(L">");

    void* ret = IconView_IconView_Original(pThis);

    // A new icon view has nothing to restore, and the revoker list is released
    // while unloading.
    if (g_unloading) {
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
                        RoutedEventArgs const& e) {
            Wh_Log(L">");

            g_autoRevokerList->erase(autoRevokerIt);

            auto iconView = sender.try_as<FrameworkElement>();
            if (!iconView) {
                return;
            }

            auto className = winrt::get_class_name(iconView);
            Wh_Log(L"className: %s", className.c_str());

            if (className == L"SystemTray.IconView") {
                if (iconView.Name() == L"SystemTrayIcon") {
                    if (IsChildOfElementByName(iconView, L"MainStack")) {
                        ApplyMainStackIconViewStyle(iconView);
                    } else if (IsChildOfElementByName(iconView,
                                                      L"NonActivatableStack")) {
                        ApplyNonActivatableStackIconViewStyle(iconView);
                    } else if (IsChildOfElementByName(iconView,
                                                      L"ControlCenterButton")) {
                        ApplyControlCenterButtonIconStyle(iconView);
                    } else if (IsChildOfElementByName(
                                   iconView, L"NotificationCenterButton")) {
                        ApplyBellIconStyle(iconView);
                    } else if (IsChildOfElementByName(iconView,
                                                      L"ShowDesktopStack")) {
                        ApplyShowDesktopStyle(iconView);
                    }
                }
            }
        });

    return ret;
}

void* CTaskBand_ITaskListWndSite_vftable;

using CTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void* pThis, void** result);
CTaskBand_GetTaskbarHost_t CTaskBand_GetTaskbarHost_Original;

void* TaskbarHost_FrameHeight_Original;

using std__Ref_count_base__Decref_t = void(WINAPI*)(void* pThis);
std__Ref_count_base__Decref_t std__Ref_count_base__Decref_Original;

XamlRoot GetTaskbarXamlRoot(HWND hTaskbarWnd) {
    HWND hTaskSwWnd = (HWND)GetProp(hTaskbarWnd, L"TaskbandHWND");
    if (!hTaskSwWnd) {
        return nullptr;
    }

    void* taskBand = (void*)GetWindowLongPtr(hTaskSwWnd, 0);
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

#if defined(_M_X64)
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
#elif defined(_M_ARM64)
    {
        // 7f2303d5 pacibsp
        // fd7bbfa9 stp     fp, lr, [sp, #-0x10]!
        // fd030091 mov     fp, sp
        // 080c41f8 ldr     x8, [x0, #0x10]!
        const DWORD* p = (const DWORD*)TaskbarHost_FrameHeight_Original;
        if (p[0] == 0xD503237F && (p[1] & 0xFFC07FFF) == 0xA9807BFD &&
            p[2] == 0x910003FD && (p[3] & 0xFFF00FE0) == 0xF8400C00) {
            taskbarElementIUnknownOffset = (p[3] >> 12) & 0xFF;
        } else {
            Wh_Log(L"Unsupported TaskbarHost::FrameHeight");
        }
    }
#else
#error "Unsupported architecture"
#endif

    auto* taskbarElementIUnknown =
        *(IUnknown**)((BYTE*)taskbarHostSharedPtr[0] +
                      taskbarElementIUnknownOffset);

    FrameworkElement taskbarElement = nullptr;
    taskbarElementIUnknown->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                           winrt::put_abi(taskbarElement));

    auto result = taskbarElement ? taskbarElement.XamlRoot() : nullptr;

    std__Ref_count_base__Decref_Original(taskbarHostSharedPtr[1]);

    return result;
}

using RunFromWindowThreadProc_t = void(WINAPI*)(void* parameter);

bool RunFromWindowThread(HWND hWnd,
                         RunFromWindowThreadProc_t proc,
                         void* procParam) {
    static const UINT runFromWindowThreadRegisteredMsg =
        RegisterWindowMessage(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);

    struct RUN_FROM_WINDOW_THREAD_PARAM {
        RunFromWindowThreadProc_t proc;
        void* procParam;
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

void LoadSettings() {
    g_settings.enabled = Wh_GetIntSetting(L"enabled");
    g_settings.size = std::clamp(Wh_GetIntSetting(L"size"), 28, 48);
    g_settings.showDate = Wh_GetIntSetting(L"showDate");
    g_settings.showNetwork = Wh_GetIntSetting(L"showNetwork");

    PCWSTR duoTimeFormat = Wh_GetStringSetting(L"timeFormat");
    g_settings.timeFormat = TimeFormatSetting::autoFormat;
    if (wcscmp(duoTimeFormat, L"h24") == 0) {
        g_settings.timeFormat = TimeFormatSetting::h24;
    } else if (wcscmp(duoTimeFormat, L"h12") == 0) {
        g_settings.timeFormat = TimeFormatSetting::h12;
    }
    Wh_FreeStringSetting(duoTimeFormat);

    PCWSTR duoDateFormat = Wh_GetStringSetting(L"dateFormat");
    g_settings.dateFormat = DateFormatSetting::monthDay;
    if (wcscmp(duoDateFormat, L"dayMonth") == 0) {
        g_settings.dateFormat = DateFormatSetting::dayMonth;
    }
    Wh_FreeStringSetting(duoDateFormat);

    g_tuning.ringDiameter = GetDoubleSetting(L"ringDiameter", 35.0, 10.0, 48.0);
    g_tuning.ringThickness =
        GetDoubleSetting(L"ringThickness", 4.0, 0.5, 12.0);
    g_tuning.ringSweepDeg =
        GetDoubleSetting(L"ringSweepDeg", 240.0, 90.0, 360.0);
    g_tuning.trackOpacityPct =
        GetDoubleSetting(L"ringTrackOpacity", 28.0, 0.0, 100.0);
    g_tuning.chargingNotchDeg =
        GetDoubleSetting(L"chargingNotchDeg", 48.0, 0.0, 120.0);
    g_tuning.boltWidth = GetDoubleSetting(L"boltWidth", 7.2, 0.5, 24.0);
    g_tuning.boltHeight = GetDoubleSetting(L"boltHeight", 9.1, 0.5, 28.0);
    g_tuning.wifiHeight = GetDoubleSetting(L"wifiHeight", 10.5, 3.0, 40.0);
    g_tuning.wifiWidth = GetDoubleSetting(L"wifiWidth", 14.5, 3.0, 56.0);
    g_tuning.wifiThickness =
        GetDoubleSetting(L"wifiThickness", 2.2, 0.3, 12.0);
    g_tuning.wifiOpeningDeg =
        GetDoubleSetting(L"wifiOpeningDeg", 76.8, 20.0, 180.0);
    g_tuning.wifiCenterPct =
        GetDoubleSetting(L"wifiCenterPct", 41.0, 5.0, 95.0);
    g_tuning.wifiChargingDrop =
        GetDoubleSetting(L"wifiChargingDrop", 1.5, 0.0, 12.0);
    g_tuning.digitsSize = GetDoubleSetting(L"digitsSize", 6.66, 2.0, 16.0);
    g_tuning.timeRowPct = GetDoubleSetting(L"timeRowPct", 73.0, 5.0, 98.0);
    g_tuning.dateRowPct = GetDoubleSetting(L"dateRowPct", 89.5, 5.0, 100.0);
    g_tuning.digitsSpacing =
        std::clamp(Wh_GetIntSetting(L"digitsSpacing"), 0, 400);

    g_settings.hideVolumeIcon = Wh_GetIntSetting(L"hideVolumeIcon");
    g_settings.hideNetworkIcon = Wh_GetIntSetting(L"hideNetworkIcon");
    g_settings.hideBatteryIcon = Wh_GetIntSetting(L"hideBatteryIcon");
    g_settings.grayscaleBatteryIcon = Wh_GetIntSetting(L"grayscaleBatteryIcon");
    g_settings.hideMicrophoneIcon = Wh_GetIntSetting(L"hideMicrophoneIcon");
    g_settings.hideGeolocationIcon = Wh_GetIntSetting(L"hideGeolocationIcon");
    g_settings.hideStudioEffectsIcon =
        Wh_GetIntSetting(L"hideStudioEffectsIcon");
    g_settings.hideRecallIcon = Wh_GetIntSetting(L"hideRecallIcon");
    g_settings.hideLanguageBar = Wh_GetIntSetting(L"hideLanguageBar");
    g_settings.hideLanguageSupplementaryIcons =
        Wh_GetIntSetting(L"hideLanguageSupplementaryIcons");

    PCWSTR hideBellIcon = Wh_GetStringSetting(L"hideBellIcon");
    g_settings.hideBellIcon = HideBellIcon::never;
    if (wcscmp(hideBellIcon, L"whenInactive") == 0) {
        g_settings.hideBellIcon = HideBellIcon::whenInactive;
    } else if (wcscmp(hideBellIcon, L"whenInactiveAndNoDnd") == 0) {
        g_settings.hideBellIcon = HideBellIcon::whenInactiveAndNoDnd;
    } else if (wcscmp(hideBellIcon, L"always") == 0) {
        g_settings.hideBellIcon = HideBellIcon::always;
    }
    Wh_FreeStringSetting(hideBellIcon);

    PCWSTR clockVisibility = Wh_GetStringSetting(L"clockVisibility");
    g_settings.clockVisibility = ClockVisibility::showAll;
    if (wcscmp(clockVisibility, L"hideTime") == 0) {
        g_settings.clockVisibility = ClockVisibility::hideTime;
    } else if (wcscmp(clockVisibility, L"hideDate") == 0) {
        g_settings.clockVisibility = ClockVisibility::hideDate;
    } else if (wcscmp(clockVisibility, L"hideAll") == 0) {
        g_settings.clockVisibility = ClockVisibility::hideAll;
    }
    Wh_FreeStringSetting(clockVisibility);

    g_settings.showDesktopButtonWidth =
        std::clamp(Wh_GetIntSetting(L"showDesktopButtonWidth"), 0, 800);
}

void ApplySettings() {
    struct ApplySettingsParam {
        HWND hTaskbarWnd;
    };

    Wh_Log(L"Applying settings");

    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (!hTaskbarWnd) {
        Wh_Log(L"No taskbar found");
        return;
    }

    ApplySettingsParam param{
        .hTaskbarWnd = hTaskbarWnd,
    };

    RunFromWindowThread(
        hTaskbarWnd,
        [](void* pParam) {
            ApplySettingsParam& param = *(ApplySettingsParam*)pParam;

            g_autoRevokerList->clear();

            if (auto bellSystemTrayIconElement =
                    g_bellSystemTrayIconElement.get()) {
                bellSystemTrayIconElement.UnregisterPropertyChangedCallback(
                    Automation::AutomationProperties::NameProperty(),
                    g_bellAutomationNameChangedToken);
                g_bellSystemTrayIconElement = nullptr;
                g_bellAutomationNameChangedToken = 0;
            }

            if (auto mainStackInnerTextBlock =
                    g_mainStackInnerTextBlock.get()) {
                mainStackInnerTextBlock.UnregisterPropertyChangedCallback(
                    Controls::TextBlock::TextProperty(),
                    g_mainStackTextChangedToken);
                g_mainStackInnerTextBlock = nullptr;
                g_mainStackTextChangedToken = 0;
            }

            if (auto controlCenterStackPanel =
                    g_controlCenterStackPanel.get()) {
                controlCenterStackPanel.SizeChanged(
                    g_controlCenterStackPanelSizeChangedToken);
                g_controlCenterStackPanel = nullptr;
                g_controlCenterStackPanelSizeChangedToken = {};
            }

            // Unregister and restore battery TextBlock foregrounds. ApplyStyle
            // below will re-register if grayscale is still enabled. This also
            // covers stale entries whose icon view was destroyed.
            for (auto& state : *g_batteryTextBlockStates) {
                auto textBlock = state.textBlock.get();
                if (!textBlock) {
                    continue;
                }
                textBlock.UnregisterPropertyChangedCallback(
                    Controls::TextBlock::ForegroundProperty(),
                    state.foregroundChangedToken);
                textBlock.Foreground(state.savedForeground);
            }
            g_batteryTextBlockStates->clear();

            // Restore the show desktop width overrides. ApplyStyle below
            // re-applies them unless the mod is unloading.
            for (auto& state : g_showDesktopWidthStates) {
                auto element = state.element.get();
                if (!element) {
                    continue;
                }
                if (state.savedMinWidth) {
                    element.MinWidth(*state.savedMinWidth);
                } else {
                    element.ClearValue(FrameworkElement::MinWidthProperty());
                }
                if (state.savedMaxWidth) {
                    element.MaxWidth(*state.savedMaxWidth);
                } else {
                    element.ClearValue(FrameworkElement::MaxWidthProperty());
                }
            }
            g_showDesktopWidthStates.clear();

            // Restore the clock visibility overrides. ApplyStyle below
            // re-applies them unless the mod is unloading.
            RestoreClockVisibility();

            auto xamlRoot = GetTaskbarXamlRoot(param.hTaskbarWnd);
            if (!xamlRoot) {
                Wh_Log(L"Getting XamlRoot failed");
            } else if (!ApplyStyle(xamlRoot)) {
                Wh_Log(L"ApplyStyle failed");
            }

            if (g_unloading) {
                g_autoRevokerList.reset();
                g_batteryTextBlockStates.reset();
                g_clockVisibilityStates.reset();
            }
        },
        &param);
}

bool HookSystemTraySymbols(HMODULE module) {
    // SystemTray.dll, Taskbar.View.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(public: __cdecl winrt::SystemTray::implementation::IconView::IconView(void))"},
            &IconView_IconView_Original,
            IconView_IconView_Hook,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::SystemTray::implementation::DateTimeIconContent,struct winrt::Windows::UI::Xaml::IFrameworkElementOverrides>::OnApplyTemplate(void))"},
            &DateTimeIconContent_OnApplyTemplate_Original,
            DateTimeIconContent_OnApplyTemplate_Hook,
            true,
        },
    };

    if (!HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }

    return true;
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
            // Starting with Taskbar.View.dll 2604.8002.200.6000, the SystemTray
            // types moved out of Taskbar.View.dll into SystemTray.dll, so don't
            // hook Taskbar.View.dll at this version and above.
            VS_FIXEDFILEINFO* fixedFileInfo =
                GetModuleVersionInfo(module, nullptr);
            WORD moduleMajor =
                fixedFileInfo ? HIWORD(fixedFileInfo->dwFileVersionMS) : 0;
            if (!moduleMajor || moduleMajor >= 2604) {
                Wh_Log(L"Skipping Taskbar.View.dll version %d", moduleMajor);
                module = nullptr;
            }
        }
    }
    if (!module) {
        module = GetModuleHandle(L"ExplorerExtensions.dll");
    }

    return module;
}

void HandleLoadedModuleIfSystemTray(HMODULE module, LPCWSTR lpLibFileName) {
    if (!g_systemTrayModuleHooked && GetSystemTrayModuleHandle() == module &&
        !g_systemTrayModuleHooked.exchange(true)) {
        Wh_Log(L"Loaded %s", lpLibFileName);

        if (HookSystemTraySymbols(module)) {
            Wh_ApplyHookOperations();
        }
    }
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;
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

    return HookSymbols(module, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks));
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    if (HMODULE systemTrayModule = GetSystemTrayModuleHandle()) {
        g_systemTrayModuleHooked = true;
        if (!HookSystemTraySymbols(systemTrayModule)) {
            return FALSE;
        }
    } else {
        Wh_Log(L"System tray module not loaded yet");

        HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
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
    Wh_Log(L">");

    if (!g_systemTrayModuleHooked) {
        if (HMODULE systemTrayModule = GetSystemTrayModuleHandle()) {
            if (!g_systemTrayModuleHooked.exchange(true)) {
                Wh_Log(L"Got system tray module");

                if (HookSystemTraySymbols(systemTrayModule)) {
                    Wh_ApplyHookOperations();
                }
            }
        }
    }

    ApplySettings();
}

void Wh_ModBeforeUninit() {
    Wh_Log(L">");

    g_unloading = true;

    ApplySettings();
}

void Wh_ModUninit() {
    Wh_Log(L">");

    CleanupWidget();
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    LoadSettings();

    ApplySettings();
}
