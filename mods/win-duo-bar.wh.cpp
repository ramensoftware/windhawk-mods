// ==WindhawkMod==
// @id              win-duo-bar
// @name            Win Duo Bar
// @description     A compact ring widget for the Windows 11 taskbar clock: a battery gauge ring, a Wi-Fi symbol, the time and date, and a charging animation
// @description:zh-CN 一个替换 Windows 11 任务栏时钟的圆环小组件：电量圆环、Wi-Fi 图标、时间日期，以及充电动画
// @version         1.1.0
// @author          Felinyu
// @github          https://github.com/Felinyu
// @license         GPL-3.0
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lversion
// ==/WindhawkMod==

// Published under the GNU General Public License v3.0.
// The taskbar XAML access code is derived from the Windhawk mods by m417z
// (https://github.com/m417z/my-windhawk-mods), which are also licensed under
// the GPLv3.

// ==WindhawkModReadme==
/*
# Win Duo Bar

![Win Duo Bar in the notification area](https://raw.githubusercontent.com/Felinyu/win-duo-bar/main/screenshots/hero.png)

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
- **One widget per taskbar** - every taskbar clock gets its own widget, so
  multi-monitor setups keep a clock on every taskbar.

![Win Duo Bar states](https://raw.githubusercontent.com/Felinyu/win-duo-bar/main/screenshots/states.png)

Every visual parameter (ring diameter/thickness/gauge span/track opacity/notch
angle/bolt size, Wi-Fi size/thickness/opening/position, digits size and row
positions) can be adjusted in the mod settings. The settings are localized
(English and Simplified Chinese).

The widget is 38 DIP by default. The DIP geometry values below are relative to
that default size and scale together with the "Widget size" setting.

This mod replaces the clock only. To hide the system battery and network tray
icons, which the widget makes redundant, use the
[Taskbar tray system icon tweaks](https://github.com/ramensoftware/windhawk-mods/blob/master/mods/taskbar-tray-system-icon-tweaks.wh.cpp)
mod instead.

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
- **每个任务栏一个小组件**：每个任务栏时钟都有独立的小组件，多显示器任务栏
  都会显示。

所有视觉参数（圆环外径/粗细/弧度范围/轨道透明度/豁口角度/闪电大小、Wi-Fi 尺寸/
粗细/开口/位置、数字字号和两行位置）都能在设置里调整，设置项已本地化
（英文/简体中文）。

小组件默认 38 DIP；下面的 DIP 几何参数以该默认尺寸为基准，并随「小组件尺寸」
设置一起缩放。

本模组只替换时钟。如需隐藏系统托盘的电量和网络图标（小组件已替代它们），
请使用
[Taskbar tray system icon tweaks](https://github.com/ramensoftware/windhawk-mods/blob/master/mods/taskbar-tray-system-icon-tweaks.wh.cpp)
模组。

仅支持 Windows 11。

## Credits / 致谢

The taskbar XAML access code (XamlRoot lookup, symbol hooks, running code on the
taskbar thread) is derived from the Windhawk mods by
[m417z](https://m417z.com/) (GPLv3), including the
[Taskbar tray system icon tweaks](https://github.com/m417z/my-windhawk-mods)
mod. The Wi-Fi symbol geometry is traced from a "09"-style Wi-Fi icon reference.

任务栏 XAML 访问代码（XamlRoot 获取、符号挂钩、任务栏线程执行）派生自
[m417z](https://m417z.com/) 的 Windhawk 模组（GPLv3）。Wi-Fi 图标几何按 09 风格
参考图标描出。
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- size: 38
  $name: Widget size, DIP
  $name:zh-CN: 小组件尺寸（DIP）
  $description: >-
    The widget span in DIP. All other DIP values are relative to the default
    38 DIP size and scale together with it.
  $description:zh-CN: >-
    小组件的边长（DIP）。其他 DIP 参数以默认 38 DIP 为基准，并随该尺寸一起缩放。
- timeFormat: auto
  $name: Time format
  $name:zh-CN: 时间格式
  $options:
  - auto: System default
  - h24: 24-hour, e.g. 1427
  - h12: 12-hour, e.g. 0227
  $options:zh-CN:
  - auto: 跟随系统
  - h24: 24 小时制，如 1427
  - h12: 12 小时制，如 0227
- dateFormat: monthDay
  $name: Date format
  $name:zh-CN: 日期格式
  $options:
  - monthDay: Month then day, e.g. 1102
  - dayMonth: Day then month, e.g. 0211
  $options:zh-CN:
  - monthDay: 月在前，如 1102
  - dayMonth: 日在前，如 0211
- showDate: true
  $name: Show the date row in the widget
  $name:zh-CN: 显示日期行
- showNetwork: true
  $name: Show the network icon in the widget
  $name:zh-CN: 显示网络图标
- ringDiameter: "35"
  $name: Ring outer diameter, DIP
  $name:zh-CN: 圆环外径（DIP）
  $description: >-
    Outer diameter of the battery ring in DIP, relative to the default 38 DIP
    widget.
  $description:zh-CN: >-
    电量圆环的外径（DIP），以默认 38 DIP 小组件为基准。
- ringThickness: "3.7"
  $name: Ring thickness, DIP
  $name:zh-CN: 圆环粗细（DIP）
  $description: >-
    Stroke width of the ring.
  $description:zh-CN: >-
    圆环线条的粗细。
- ringSweepDeg: "263"
  $name: Battery arc span, degrees
  $name:zh-CN: 电量弧范围（度）
  $description: >-
    How many degrees of the circle the battery gauge covers; 360 is a full
    circle, 263 is the default look with a gap at the bottom.
  $description:zh-CN: >-
    电量弧覆盖的角度，360 为整圆，263 为默认（底部留缺口）。
- ringTrackOpacity: "28"
  $name: Track opacity, %
  $name:zh-CN: 轨道不透明度（%）
  $description: >-
    Opacity of the translucent track behind the battery level; 0 hides it.
  $description:zh-CN: >-
    电量填充后面那条半透明轨道的透明度，0 表示不显示。
- chargingNotchDeg: "48"
  $name: Charging notch angle, degrees
  $name:zh-CN: 充电豁口角度（度）
  $description: >-
    Width of the notch that opens at the top while charging, where the bolt
    sits.
  $description:zh-CN: >-
    充电时顶部张开、放闪电的豁口宽度（度）。
- boltWidth: "7.2"
  $name: Bolt width, DIP
  $name:zh-CN: 闪电宽度（DIP）
- boltHeight: "9.1"
  $name: Bolt height, DIP
  $name:zh-CN: 闪电高度（DIP）
- wifiHeight: "10.5"
  $name: Wi-Fi symbol height, DIP
  $name:zh-CN: Wi-Fi 图标高度（DIP）
- wifiWidth: "14.5"
  $name: Wi-Fi symbol width, DIP
  $name:zh-CN: Wi-Fi 图标宽度（DIP）
- wifiThickness: "2"
  $name: Wi-Fi line thickness, DIP
  $name:zh-CN: Wi-Fi 线条粗细（DIP）
- wifiOpeningDeg: "75"
  $name: Wi-Fi opening angle, degrees
  $name:zh-CN: Wi-Fi 开口角度（度）
  $description: >-
    Total angle between the two arc ends of the Wi-Fi symbol.
  $description:zh-CN: >-
    Wi-Fi 图标两条弧线端点之间的总角度。
- wifiCenterPct: "41"
  $name: Wi-Fi vertical position, %
  $name:zh-CN: Wi-Fi 垂直位置（%）
  $description: >-
    Vertical centre of the Wi-Fi symbol, as a percentage of the widget height.
  $description:zh-CN: >-
    Wi-Fi 图标中心在小组件高度上的百分比位置。
- wifiChargingDrop: "1.5"
  $name: Wi-Fi drop while charging, DIP
  $name:zh-CN: 充电时 Wi-Fi 下移量（DIP）
- digitsSize: "7"
  $name: Digits font size, DIP
  $name:zh-CN: 数字字号（DIP）
- timeRowPct: "73"
  $name: Time row position, %
  $name:zh-CN: 时间行位置（%）
  $description: >-
    Vertical centre of the time row, as a percentage of the widget height.
  $description:zh-CN: >-
    时间行中心在小组件高度上的百分比位置。
- dateRowPct: "89.5"
  $name: Date row position, %
  $name:zh-CN: 日期行位置（%）
  $description: >-
    Vertical centre of the date row, as a percentage of the widget height.
  $description:zh-CN: >-
    日期行中心在小组件高度上的百分比位置。
- digitsSpacing: 110
  $name: Digits letter spacing, 1/1000 em
  $name:zh-CN: 数字字间距（1/1000 em）
  $description: >-
    Letter spacing between the digits, in 1/1000 em units.
  $description:zh-CN: >-
    数字之间的字间距，单位 1/1000 em。
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <list>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Networking.Connectivity.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Text.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Shapes.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/base.h>

using namespace winrt::Windows::UI::Xaml;

// ===== Settings =====

enum class TimeFormatSetting {
    autoFormat,
    h24,
    h12,
};

enum class DateFormatSetting {
    monthDay,
    dayMonth,
};

struct Settings {
    int size = 38;
    TimeFormatSetting timeFormat = TimeFormatSetting::autoFormat;
    DateFormatSetting dateFormat = DateFormatSetting::monthDay;
    bool showDate = true;
    bool showNetwork = true;
};

Settings g_settings;

// Every visual parameter of the widget. The settings page exposes these; DIP
// values are relative to the default 38 DIP widget and are scaled by the
// widget size.
struct WidgetTuning {
    double ringDiameter = 35.0;      // DIP, outer diameter of the ring
    double ringThickness = 3.7;      // DIP, stroke width
    double ringSweepDeg = 263.0;     // degrees covered by the battery gauge
    double trackOpacityPct = 28.0;   // % opacity of the track
    double chargingNotchDeg = 48.0;  // degrees of the bolt notch
    double boltWidth = 7.2;          // DIP
    double boltHeight = 9.1;         // DIP
    double wifiHeight = 10.5;        // DIP
    double wifiWidth = 14.5;         // DIP
    double wifiThickness = 2.0;      // DIP
    double wifiOpeningDeg = 75.0;    // degrees of the Wi-Fi opening
    double wifiCenterPct = 41.0;     // % of the widget height
    double wifiChargingDrop = 1.5;   // DIP drop while charging
    double digitsSize = 7.0;         // DIP
    double timeRowPct = 73.0;        // % of the widget height
    double dateRowPct = 89.5;        // % of the widget height
    int digitsSpacing = 110;         // 1/1000 em
};

WidgetTuning g_tuning;

double GetDoubleSetting(PCWSTR name,
                        double defaultValue,
                        double minValue,
                        double maxValue) {
    auto value = WindhawkUtils::StringSetting::make(name);
    double result = defaultValue;
    if (*value.get()) {
        wchar_t* end = nullptr;
        double parsed = wcstod(value.get(), &end);
        if (end && end != value.get()) {
            result = parsed;
        }
    }
    return std::clamp(result, minValue, maxValue);
}

// ===== Shared state =====

std::atomic<bool> g_systemTrayModuleHooked{false};
std::atomic<bool> g_unloading{false};

// ===== Constants =====

constexpr int kMinWidgetSize = 28;
constexpr int kMaxWidgetSize = 48;

// DIP values in the settings are relative to this widget size.
constexpr double kBaseSize = 38.0;

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
constexpr int kAnimTickMs = 16;

// The name of the injected canvas. Used to remove a possible leftover from a
// previous load of the mod.
constexpr WCHAR kWidgetRootName[] = L"WinDuoBarWidget";

// ===== Device state =====

struct BatteryInfo {
    bool valid = false;
    bool hasBattery = false;
    int percent = 100;
    bool plugged = false;
    bool saver = false;
};

enum class NetworkKind {
    offline,
    wifi,
    ethernet,
};

struct NetworkInfo {
    NetworkKind kind = NetworkKind::offline;
};

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

        info.kind = profile.IsWlanConnectionProfile() ? NetworkKind::wifi
                                                     : NetworkKind::ethernet;
    } catch (...) {
        Wh_Log(L"Failed to read the network info");
    }

    return info;
}

std::wstring Pad2(int value) {
    return value < 10 ? L"0" + std::to_wstring(value) : std::to_wstring(value);
}

// The taskbar clock follows the short time format of the user locale. In a
// format picture, a lowercase "h" is a 12-hour clock hour and an uppercase "H"
// is a 24-hour one.
bool SystemUses12HourClock() {
    WCHAR format[32]{};
    if (!GetLocaleInfoEx(LOCALE_NAME_USER_DEFAULT, LOCALE_SSHORTTIME, format,
                         ARRAYSIZE(format))) {
        return false;
    }
    return wcschr(format, L'h') != nullptr;
}

std::wstring FormatTimeString(const SYSTEMTIME& st) {
    // Four digits without a separator: 1357 means 13:57.
    int hour = st.wHour;

    bool use12Hour =
        g_settings.timeFormat == TimeFormatSetting::h12 ||
        (g_settings.timeFormat == TimeFormatSetting::autoFormat &&
         SystemUses12HourClock());
    if (use12Hour) {
        hour %= 12;
        if (hour == 0) {
            hour = 12;
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

// ===== Widget geometry =====

// The settings geometry scaled for the current widget size, so that the widget
// stays proportional when the size changes.
struct ScaledGeometry {
    double size = 0;
    double ringDiameter = 0;
    double ringThickness = 0;
    double ringSweepDeg = 0;
    double trackOpacityPct = 0;
    double chargingNotchDeg = 0;
    double boltWidth = 0;
    double boltHeight = 0;
    double wifiHeight = 0;
    double wifiWidth = 0;
    double wifiThickness = 0;
    double wifiOpeningDeg = 0;
    double wifiCenterPct = 0;
    double wifiChargingDrop = 0;
    double digitsSize = 0;
    double timeRowPct = 0;
    double dateRowPct = 0;
    int digitsSpacing = 0;
};

ScaledGeometry GetScaledGeometry() {
    ScaledGeometry g;

    int requestedSize =
        std::clamp(g_settings.size, kMinWidgetSize, kMaxWidgetSize);
    double size = requestedSize;
    double scale = size / kBaseSize;

    g.size = size;
    g.ringThickness = std::clamp(g_tuning.ringThickness * scale, 0.5,
                                 size * 0.35);
    g.ringDiameter = std::clamp(g_tuning.ringDiameter * scale,
                                g.ringThickness + 2.0, size);
    g.ringSweepDeg = g_tuning.ringSweepDeg;
    g.trackOpacityPct = g_tuning.trackOpacityPct;
    g.chargingNotchDeg = g_tuning.chargingNotchDeg;
    g.boltWidth = g_tuning.boltWidth * scale;
    g.boltHeight = g_tuning.boltHeight * scale;
    g.wifiHeight = g_tuning.wifiHeight * scale;
    g.wifiWidth = g_tuning.wifiWidth * scale;
    g.wifiThickness = g_tuning.wifiThickness * scale;
    g.wifiOpeningDeg = g_tuning.wifiOpeningDeg;
    g.wifiCenterPct = g_tuning.wifiCenterPct;
    g.wifiChargingDrop = g_tuning.wifiChargingDrop * scale;
    g.digitsSize = g_tuning.digitsSize * scale;
    g.timeRowPct = g_tuning.timeRowPct;
    g.dateRowPct = g_tuning.dateRowPct;
    g.digitsSpacing = g_tuning.digitsSpacing;

    return g;
}

bool TuningEquals(const WidgetTuning& a, const WidgetTuning& b) {
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

// ===== Widget instances =====

// One per taskbar clock, i.e. one per monitor that shows a clock. All the
// members are only accessed from the taskbar UI thread; the instances are
// released there when the mod unloads.
struct WidgetInstance {
    // The native SystemTray.DateTimeIconContent of the clock, the instance key.
    winrt::weak_ref<FrameworkElement> nativeContent;
    // The parent panel of the native content, which hosts the widget canvas.
    winrt::weak_ref<Controls::Panel> hostPanel;
    // The local Visibility value of the native content, for restoring it.
    std::optional<Visibility> savedNativeVisibility;

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

    ScaledGeometry geometry{};
    int size = 0;
    bool dark = true;
    bool ringColorSet = false;
    double notchT = 1.0;
    double notchTarget = 1.0;
    winrt::Windows::UI::Color ringColor{255, 255, 255, 255};
    winrt::Windows::UI::Color colorTarget{255, 255, 255, 255};
    winrt::Windows::UI::Color foreground{255, 255, 255, 255};
    BatteryInfo battery{};
    NetworkInfo network{};
    WidgetTuning tuning{};
    bool tuningValid = false;
    std::wstring timeString;
    std::wstring dateString;
};

// The following globals hold XAML objects, which must not be released on
// another thread when the process exits, so they're never destroyed
// automatically. They're released on the taskbar UI thread when the mod
// unloads.
[[clang::no_destroy]] std::optional<std::vector<WidgetInstance>> g_instances{
    std::in_place};

using FrameworkElementLoadedEventRevoker = winrt::impl::event_revoker<
    IFrameworkElement,
    &winrt::impl::abi<IFrameworkElement>::type::remove_Loaded>;

[[clang::no_destroy]] std::optional<
    std::list<FrameworkElementLoadedEventRevoker>>
    g_autoRevokerList{std::in_place};

[[clang::no_destroy]] DispatcherTimer g_timer{nullptr};
winrt::event_token g_timerToken{};
[[clang::no_destroy]] DispatcherTimer g_animTimer{nullptr};
winrt::event_token g_animToken{};
int g_tickCounter = 0;
int g_startupScansLeft = 0;
std::atomic<bool> g_clockHookApplied{false};

// Set by the network status change handler, which runs on a Windows Runtime
// thread pool thread, and consumed by the 1 s UI timer. All the other network
// state is UI-thread only.
std::atomic<bool> g_networkDirty{true};
bool g_networkSubscribed = false;
winrt::event_token g_networkStatusToken{};

// Keeps the network status handler alive while it may still be invoked, so
// that unloading can wait for an in-flight invocation, since it runs mod code.
struct NetworkSubscriptionMarker {};
std::shared_ptr<NetworkSubscriptionMarker> g_networkCallbackRef;

void OnAnimTick();
void EnsureAnimTimer();

// ===== Drawing helpers =====

winrt::Windows::Foundation::Point PointOnCircle(double centerX,
                                               double centerY,
                                               double radius,
                                               double degrees) {
    double radians = degrees * kPi / 180.0;
    return {static_cast<float>(centerX + radius * std::cos(radians)),
            static_cast<float>(centerY + radius * std::sin(radians))};
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
    segment.Size({static_cast<float>(radius), static_cast<float>(radius)});
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
            figure.StartPoint(
                {static_cast<float>(ax), static_cast<float>(ay)});
            started = true;
        } else {
            Media::LineSegment line;
            line.Point({static_cast<float>(ax), static_cast<float>(ay)});
            figure.Segments().Append(line);
        }

        Media::QuadraticBezierSegment corner;
        corner.Point1({static_cast<float>(x[i]), static_cast<float>(y[i])});
        corner.Point2({static_cast<float>(bx), static_cast<float>(by)});
        figure.Segments().Append(corner);
    }

    figure.IsClosed(true);

    Media::PathGeometry geometry;
    geometry.Figures().Append(figure);
    return geometry;
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
    figure.StartPoint({static_cast<float>(xs[0] * height * scaleX),
                       static_cast<float>(ys[0] * height)});
    for (int i = 1; i < kCount; i++) {
        Media::LineSegment segment;
        segment.Point({static_cast<float>(xs[i] * height * scaleX),
                       static_cast<float>(ys[i] * height)});
        figure.Segments().Append(segment);
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

// ===== Widget visuals =====

void BuildInstanceVisuals(WidgetInstance& instance) {
    ScaledGeometry g = GetScaledGeometry();
    double size = g.size;
    double centerX = size / 2.0;
    double centerY = size / 2.0;
    double stroke = g.ringThickness;
    double radius = std::max(1.0, g.ringDiameter / 2.0 - stroke / 2.0);
    double halfSweepDeg = g.ringSweepDeg / 2.0;

    Controls::Canvas canvas;
    canvas.Name(kWidgetRootName);
    canvas.Width(size);
    canvas.Height(size);
    canvas.IsHitTestVisible(false);
    canvas.HorizontalAlignment(HorizontalAlignment::Center);
    canvas.VerticalAlignment(VerticalAlignment::Center);

    auto foregroundBrush = Media::SolidColorBrush(MakeColor(0xFFFFFFFF));
    auto progressBrush = Media::SolidColorBrush(MakeColor(0xFFFFFFFF));
    auto trackBrush = Media::SolidColorBrush(
        WithAlpha(MakeColor(0xFFFFFFFF), kTrackAlpha));

    // Translucent track: the whole gauge always stays visible so a low battery
    // level never leaves a gaping hole in the ring.
    auto trackPath = MakeArcPath(
        trackBrush, stroke,
        MakeRingProgressGeometry(centerX, centerY, radius, 1.0, 0.0,
                                 halfSweepDeg));
    Controls::Canvas::SetLeft(trackPath, 0);
    Controls::Canvas::SetTop(trackPath, 0);

    // Battery ring, green while charging.
    auto progressPath = MakeArcPath(
        progressBrush, stroke,
        MakeRingProgressGeometry(centerX, centerY, radius, 1.0, 0.0,
                                 halfSweepDeg));
    Controls::Canvas::SetLeft(progressPath, 0);
    Controls::Canvas::SetTop(progressPath, 0);

    // Charging bolt in the top notch, shown only while charging.
    Shapes::Path chargingBolt;
    chargingBolt.Fill(foregroundBrush);
    chargingBolt.Stretch(Media::Stretch::None);
    chargingBolt.Data(MakeBoltGeometry(centerX,
                                       centerY - radius + stroke / 2.0,
                                       g.boltWidth, g.boltHeight));
    chargingBolt.Visibility(Visibility::Collapsed);
    Controls::Canvas::SetLeft(chargingBolt, 0);
    Controls::Canvas::SetTop(chargingBolt, 0);

    // Custom Wi-Fi symbol in the middle of the ring: two thick rounded arcs
    // plus a rounded triangle dot, built around one shared circle center.
    double wifiHeight = g.wifiHeight;
    double wifiWidth = g.wifiWidth;
    double wifiCenterYPos = size * (g.wifiCenterPct / 100.0);
    double wifiTop = wifiCenterYPos - wifiHeight / 2.0;
    double wifiArcCenterX = wifiWidth / 2.0;
    double wifiArcCenterY = wifiHeight * kWifiArcCenterYFrac;
    double wifiArcStroke = std::min(g.wifiThickness, wifiHeight * 0.45);
    double wifiHalfDeg = std::clamp(g.wifiOpeningDeg, 20.0, 180.0) / 2.0;

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
    double digitsFont = g.digitsSize;

    auto timeText = MakeTextBlock(digitsFont, size);
    timeText.Foreground(foregroundBrush);
    timeText.FontWeight(winrt::Windows::UI::Text::FontWeights::Bold());
    timeText.CharacterSpacing(g.digitsSpacing);
    Controls::Canvas::SetLeft(timeText, 0);
    Controls::Canvas::SetTop(
        timeText, size * (g.timeRowPct / 100.0) - digitsFont * 0.64);

    auto dateText = MakeTextBlock(digitsFont, size);
    dateText.Foreground(foregroundBrush);
    dateText.FontWeight(winrt::Windows::UI::Text::FontWeights::Bold());
    dateText.CharacterSpacing(g.digitsSpacing);
    Controls::Canvas::SetLeft(dateText, 0);
    Controls::Canvas::SetTop(
        dateText, size * (g.dateRowPct / 100.0) - digitsFont * 0.64);

    canvas.Children().Append(trackPath);
    canvas.Children().Append(progressPath);
    canvas.Children().Append(wifiSymbol);
    canvas.Children().Append(networkText);
    canvas.Children().Append(chargingBolt);
    canvas.Children().Append(timeText);
    canvas.Children().Append(dateText);

    instance.canvas = canvas;
    instance.progressPath = progressPath;
    instance.trackPath = trackPath;
    instance.chargingBolt = chargingBolt;
    instance.networkText = networkText;
    instance.wifiSymbol = wifiSymbol;
    instance.timeText = timeText;
    instance.dateText = dateText;
    instance.foregroundBrush = foregroundBrush;
    instance.progressBrush = progressBrush;
    instance.trackBrush = trackBrush;
    instance.geometry = g;
    instance.size = std::clamp(g_settings.size, kMinWidgetSize, kMaxWidgetSize);
    instance.foreground = MakeColor(0xFFFFFFFF);
    instance.dark = true;
    instance.ringColorSet = false;
    instance.notchT = 1.0;
    instance.notchTarget = 1.0;
    instance.battery = BatteryInfo{};
    instance.network = NetworkInfo{};
    instance.tuning = g_tuning;
    instance.tuningValid = true;
    instance.timeString.clear();
    instance.dateString.clear();

    dateText.Visibility(g_settings.showDate ? Visibility::Visible
                                            : Visibility::Collapsed);
}

void ReleaseInstanceVisuals(WidgetInstance& instance) {
    instance.canvas = nullptr;
    instance.progressPath = nullptr;
    instance.trackPath = nullptr;
    instance.chargingBolt = nullptr;
    instance.networkText = nullptr;
    instance.wifiSymbol = nullptr;
    instance.timeText = nullptr;
    instance.dateText = nullptr;
    instance.foregroundBrush = nullptr;
    instance.progressBrush = nullptr;
    instance.trackBrush = nullptr;
    instance.geometry = ScaledGeometry{};
    instance.size = 0;
    instance.tuning = WidgetTuning{};
    instance.tuningValid = false;
    instance.ringColorSet = false;
    instance.notchT = 1.0;
    instance.notchTarget = 1.0;
    instance.timeString.clear();
    instance.dateString.clear();
}

winrt::Windows::UI::Color RingColorTarget(const WidgetInstance& instance) {
    auto const& battery = instance.battery;
    if (battery.hasBattery && battery.plugged) {
        return MakeColor(kColorCharging);
    }
    if (battery.hasBattery && battery.percent <= kLowBatteryPercent) {
        return MakeColor(kColorLow);
    }
    if (battery.hasBattery && battery.saver) {
        return MakeColor(kColorSaver);
    }
    return instance.foreground;
}

// Draws one animation frame: ring geometry with the current notch, ring color
// and the bolt that grows out of / melts back into the ring.
void ApplyChargeFrame(WidgetInstance& instance) {
    if (!instance.canvas || !instance.progressPath) {
        return;
    }

    auto const& g = instance.geometry;
    double size = g.size;
    double centerX = size / 2.0;
    double centerY = size / 2.0;
    double stroke = g.ringThickness;
    double radius = std::max(1.0, g.ringDiameter / 2.0 - stroke / 2.0);
    double halfSweepDeg = g.ringSweepDeg / 2.0;
    double notchHalfDeg = g.chargingNotchDeg / 2.0;

    double openFactor = 1.0 - SmoothStep(instance.notchT);

    instance.progressPath.Data(MakeRingProgressGeometry(
        centerX, centerY, radius,
        instance.battery.hasBattery ? instance.battery.percent / 100.0 : 1.0,
        notchHalfDeg * openFactor, halfSweepDeg));

    if (instance.progressBrush) {
        instance.progressBrush.Color(instance.ringColor);
    }

    // The track follows the same notch and color animation, so the bolt never
    // sits on top of a translucent arc.
    if (instance.trackPath) {
        instance.trackPath.Data(MakeRingProgressGeometry(
            centerX, centerY, radius, 1.0, notchHalfDeg * openFactor,
            halfSweepDeg));
    }
    if (instance.trackBrush) {
        uint8_t alpha = static_cast<uint8_t>(std::lround(
            std::clamp(g.trackOpacityPct, 0.0, 100.0) / 100.0 * 255.0));
        instance.trackBrush.Color(WithAlpha(instance.ringColor, alpha));
    }

    if (instance.wifiSymbol) {
        double wifiTop = size * (g.wifiCenterPct / 100.0) - g.wifiHeight / 2.0;
        double drop = g.wifiChargingDrop * openFactor;
        Controls::Canvas::SetTop(instance.wifiSymbol, wifiTop + drop);
    }

    if (instance.chargingBolt) {
        if (openFactor > 0.01) {
            double boltWidth = g.boltWidth * openFactor;
            double boltHeight = g.boltHeight * openFactor;
            double boltCenterY = centerY - radius + stroke / 2.0 -
                                 stroke / 2.0 * SmoothStep(instance.notchT);
            instance.chargingBolt.Data(MakeBoltGeometry(
                centerX, boltCenterY, boltWidth, boltHeight));
            instance.chargingBolt.Visibility(Visibility::Visible);
            instance.chargingBolt.Opacity(std::min(1.0, openFactor * 1.6));
        } else {
            instance.chargingBolt.Visibility(Visibility::Collapsed);
        }
    }
}

// ===== Widget updates =====

void UpdateTheme(WidgetInstance& instance, bool force) {
    if (!instance.canvas) {
        return;
    }

    bool dark = instance.canvas.ActualTheme() != ElementTheme::Light;
    if (!force && dark == instance.dark) {
        return;
    }

    instance.dark = dark;
    instance.foreground =
        dark ? MakeColor(0xFFFFFFFF) : MakeColor(kColorLightText);
    if (instance.foregroundBrush) {
        instance.foregroundBrush.Color(instance.foreground);
    }

    instance.colorTarget = RingColorTarget(instance);

    if (!instance.ringColorSet) {
        instance.ringColor = instance.colorTarget;
        ApplyChargeFrame(instance);
    } else if (ColorDistance(instance.ringColor, instance.colorTarget) > 1.5) {
        EnsureAnimTimer();
    }
}

void UpdateTimeAndDate(WidgetInstance& instance,
                       const std::wstring& timeString,
                       const std::wstring& dateString,
                       bool force) {
    if (!instance.canvas) {
        return;
    }

    if (force || timeString != instance.timeString) {
        instance.timeString = timeString;
        if (instance.timeText) {
            instance.timeText.Text(timeString);
        }
    }

    if (force || dateString != instance.dateString) {
        instance.dateString = dateString;
        if (instance.dateText) {
            instance.dateText.Text(dateString);
        }
    }
}

void UpdateBattery(WidgetInstance& instance,
                   const BatteryInfo& info,
                   bool force) {
    if (!instance.canvas || !instance.progressPath) {
        return;
    }

    bool changed = force || info.valid != instance.battery.valid ||
                   info.hasBattery != instance.battery.hasBattery ||
                   info.percent != instance.battery.percent ||
                   info.plugged != instance.battery.plugged ||
                   info.saver != instance.battery.saver;
    instance.battery = info;

    if (!changed) {
        return;
    }

    // A desktop PC always reports AC power, but it has no battery, so it must
    // not show the charging look.
    bool chargingLook = info.hasBattery && info.plugged;
    instance.notchTarget = chargingLook ? 0.0 : 1.0;
    instance.colorTarget = RingColorTarget(instance);

    if (!instance.ringColorSet) {
        // First layout after building: show the current state right away.
        instance.ringColorSet = true;
        instance.notchT = instance.notchTarget;
        instance.ringColor = instance.colorTarget;
        ApplyChargeFrame(instance);
        return;
    }

    if (instance.notchT != instance.notchTarget ||
        ColorDistance(instance.ringColor, instance.colorTarget) > 1.5) {
        EnsureAnimTimer();
    }
}

void UpdateNetwork(WidgetInstance& instance,
                   const NetworkInfo& info,
                   bool force) {
    if (!instance.canvas || !instance.networkText || !instance.wifiSymbol) {
        return;
    }

    bool changed = force || info.kind != instance.network.kind;
    instance.network = info;

    if (!changed) {
        return;
    }

    bool show = g_settings.showNetwork;
    bool showWifi = show && info.kind != NetworkKind::ethernet;
    bool showEthernet = show && info.kind == NetworkKind::ethernet;
    double wifiOpacity = info.kind == NetworkKind::offline ? 0.32 : 1.0;

    instance.wifiSymbol.Visibility(showWifi ? Visibility::Visible
                                            : Visibility::Collapsed);
    instance.wifiSymbol.Opacity(wifiOpacity);
    instance.networkText.Visibility(showEthernet ? Visibility::Visible
                                                 : Visibility::Collapsed);
    instance.networkText.Opacity(1.0);
}

void RefreshInstance(WidgetInstance& instance, bool force) {
    if (!instance.canvas) {
        return;
    }

    SYSTEMTIME st{};
    GetLocalTime(&st);

    UpdateTheme(instance, force);
    UpdateTimeAndDate(instance, FormatTimeString(st), FormatDateString(st),
                      force);
    UpdateBattery(instance, ReadBatteryInfo(), force);
    UpdateNetwork(instance, ReadNetworkInfo(), force);
}

// ===== Charging animation timer =====

void StopAnimTimer() {
    if (!g_animTimer) {
        return;
    }

    try {
        g_animTimer.Stop();
        g_animTimer.Tick(g_animToken);
    } catch (...) {
    }
    g_animTimer = nullptr;
    g_animToken = {};
}

// Advances one instance, returns true when it has nothing left to animate.
bool AnimateInstance(WidgetInstance& instance) {
    if (!instance.canvas) {
        return true;
    }

    bool done = true;

    if (instance.notchT != instance.notchTarget) {
        done = false;
        double step = kAnimTickMs / kChargeAnimMs;
        if (instance.notchT < instance.notchTarget) {
            instance.notchT =
                std::min(instance.notchT + step, instance.notchTarget);
        } else {
            instance.notchT =
                std::max(instance.notchT - step, instance.notchTarget);
        }
    }

    if (ColorDistance(instance.ringColor, instance.colorTarget) > 1.5) {
        done = false;
        instance.ringColor =
            LerpColor(instance.ringColor, instance.colorTarget, 0.22);
    } else {
        instance.ringColor = instance.colorTarget;
    }

    ApplyChargeFrame(instance);

    return done;
}

void OnAnimTick() {
    if (g_unloading || !g_instances) {
        StopAnimTimer();
        return;
    }

    bool allDone = true;
    for (auto& instance : *g_instances) {
        if (!AnimateInstance(instance)) {
            allDone = false;
        }
    }

    if (allDone) {
        StopAnimTimer();
    }
}

void EnsureAnimTimer() {
    if (g_animTimer) {
        return;
    }

    try {
        DispatcherTimer timer;
        timer.Interval(winrt::Windows::Foundation::TimeSpan{
            std::chrono::milliseconds(kAnimTickMs)});
        g_animToken = timer.Tick(
            [](winrt::Windows::Foundation::IInspectable const&,
               winrt::Windows::Foundation::IInspectable const&) {
                OnAnimTick();
            });
        timer.Start();
        g_animTimer = timer;
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"Failed to start the animation timer: %08X", e.code().value);
    } catch (...) {
        Wh_Log(L"Failed to start the animation timer");
    }
}

// ===== XAML tree helpers =====

FrameworkElement FindChildByName(DependencyObject const& parent,
                                 std::wstring_view name,
                                 int maxDepth) {
    if (maxDepth <= 0) {
        return nullptr;
    }

    int count = Media::VisualTreeHelper::GetChildrenCount(parent);
    for (int i = 0; i < count; i++) {
        auto child = Media::VisualTreeHelper::GetChild(parent, i);
        if (auto element = child.try_as<FrameworkElement>()) {
            if (element.Name() == name) {
                return element;
            }
        }
        if (auto found = FindChildByName(child, name, maxDepth - 1)) {
            return found;
        }
    }

    return nullptr;
}

void FindChildrenByClassName(DependencyObject const& parent,
                             std::wstring_view className,
                             int maxDepth,
                             std::vector<FrameworkElement>& results) {
    if (maxDepth <= 0) {
        return;
    }

    int count = Media::VisualTreeHelper::GetChildrenCount(parent);
    for (int i = 0; i < count; i++) {
        auto child = Media::VisualTreeHelper::GetChild(parent, i);
        if (auto element = child.try_as<FrameworkElement>()) {
            if (winrt::get_class_name(element) == className) {
                results.push_back(element);
                continue;
            }
        }
        FindChildrenByClassName(child, className, maxDepth - 1, results);
    }
}

// ===== Taskbar XAML access =====

void* CTaskBand_ITaskListWndSite_vftable;
void* CSecondaryTaskBand_ITaskListWndSite_vftable;

using CTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void* pThis, void** result);
CTaskBand_GetTaskbarHost_t CTaskBand_GetTaskbarHost_Original;

using CSecondaryTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void* pThis,
                                                          void** result);
CSecondaryTaskBand_GetTaskbarHost_t CSecondaryTaskBand_GetTaskbarHost_Original;

// Only read as code bytes, never called.
void* TaskbarHost_FrameHeight_Original;

using std__Ref_count_base__Decref_t = void(WINAPI*)(void* pThis);
std__Ref_count_base__Decref_t std__Ref_count_base__Decref_Original;

bool IsReadableMemory(const void* address, size_t size) {
    MEMORY_BASIC_INFORMATION info{};
    if (!address || !VirtualQuery(address, &info, sizeof(info)) ||
        info.State != MEM_COMMIT ||
        (info.Protect & (PAGE_GUARD | PAGE_NOACCESS))) {
        return false;
    }
    auto start = reinterpret_cast<uintptr_t>(address);
    auto regionEnd =
        reinterpret_cast<uintptr_t>(info.BaseAddress) + info.RegionSize;
    return size <= regionEnd - start;
}

struct TaskbarWindow {
    HWND hWnd;
    bool secondary;
};

void CollectTaskbarWindows(std::vector<TaskbarWindow>& taskbars) {
    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            DWORD processId = 0;
            WCHAR className[32];
            if (!GetWindowThreadProcessId(hWnd, &processId) ||
                processId != GetCurrentProcessId() ||
                !GetClassName(hWnd, className, ARRAYSIZE(className))) {
                return TRUE;
            }

            auto& taskbars =
                *reinterpret_cast<std::vector<TaskbarWindow>*>(lParam);
            if (_wcsicmp(className, L"Shell_TrayWnd") == 0) {
                // The primary taskbar first.
                taskbars.insert(taskbars.begin(), {hWnd, false});
            } else if (_wcsicmp(className, L"Shell_SecondaryTrayWnd") == 0) {
                taskbars.push_back({hWnd, true});
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&taskbars));
}

HWND FindCurrentProcessTaskbarWnd() {
    std::vector<TaskbarWindow> taskbars;
    CollectTaskbarWindows(taskbars);

    for (const auto& taskbar : taskbars) {
        if (!taskbar.secondary) {
            return taskbar.hWnd;
        }
    }

    return taskbars.empty() ? nullptr : taskbars.front().hWnd;
}

XamlRoot GetTaskbarXamlRoot(HWND hTaskbarWnd, bool secondary) {
    HWND hTaskSwWnd =
        secondary ? FindWindowEx(hTaskbarWnd, nullptr, L"WorkerW", nullptr)
                  : reinterpret_cast<HWND>(
                        GetProp(hTaskbarWnd, L"TaskbandHWND"));
    if (!hTaskSwWnd) {
        return nullptr;
    }

    void* taskBand = reinterpret_cast<void*>(GetWindowLongPtr(hTaskSwWnd, 0));
    void* expectedVftable = secondary
                                ? CSecondaryTaskBand_ITaskListWndSite_vftable
                                : CTaskBand_ITaskListWndSite_vftable;
    auto getTaskbarHost = secondary ? CSecondaryTaskBand_GetTaskbarHost_Original
                                    : CTaskBand_GetTaskbarHost_Original;
    if (!taskBand || !expectedVftable || !getTaskbarHost ||
        !TaskbarHost_FrameHeight_Original ||
        !std__Ref_count_base__Decref_Original) {
        return nullptr;
    }

    void* taskBandForTaskListWndSite = taskBand;
    for (int i = 0;; i++) {
        if (i == 20 ||
            !IsReadableMemory(taskBandForTaskListWndSite, sizeof(void*))) {
            return nullptr;
        }
        if (*static_cast<void**>(taskBandForTaskListWndSite) ==
            expectedVftable) {
            break;
        }
        taskBandForTaskListWndSite =
            static_cast<void**>(taskBandForTaskListWndSite) + 1;
    }

    void* taskbarHostSharedPtr[2]{};
    getTaskbarHost(taskBandForTaskListWndSite, taskbarHostSharedPtr);
    if (!taskbarHostSharedPtr[0] && !taskbarHostSharedPtr[1]) {
        return nullptr;
    }

    XamlRoot result = nullptr;

    // The offset of the taskbar element in TaskbarHost, decoded from the start
    // of TaskbarHost::FrameHeight.
    std::optional<size_t> taskbarElementIUnknownOffset;
#if defined(_M_X64)
    {
        // 48:83EC 28 | sub rsp,28
        // 48:83C1 48 | add rcx,48
        const BYTE* b =
            static_cast<const BYTE*>(TaskbarHost_FrameHeight_Original);
        if (IsReadableMemory(b, 8) && b[0] == 0x48 && b[1] == 0x83 &&
            b[2] == 0xEC && b[4] == 0x48 && b[5] == 0x83 && b[6] == 0xC1 &&
            b[7] <= 0x7F) {
            taskbarElementIUnknownOffset = b[7];
        }
    }
#elif defined(_M_ARM64)
    {
        // 7f2303d5 pacibsp
        // fd7bbfa9 stp     fp, lr, [sp, #-0x10]!
        // fd030091 mov     fp, sp
        // 080c41f8 ldr     x8, [x0, #0x10]!
        const DWORD* p =
            static_cast<const DWORD*>(TaskbarHost_FrameHeight_Original);
        if (IsReadableMemory(p, sizeof(DWORD) * 4) && p[0] == 0xD503237F &&
            (p[1] & 0xFFC07FFF) == 0xA9807BFD && p[2] == 0x910003FD &&
            (p[3] & 0xFFF00FE0) == 0xF8400C00) {
            taskbarElementIUnknownOffset = (p[3] >> 12) & 0xFF;
        }
    }
#else
#error "Unsupported architecture"
#endif

    if (!taskbarElementIUnknownOffset) {
        Wh_Log(L"Unsupported TaskbarHost::FrameHeight");
    }

    if (taskbarHostSharedPtr[0] && taskbarElementIUnknownOffset) {
        void* slot = static_cast<BYTE*>(taskbarHostSharedPtr[0]) +
                     *taskbarElementIUnknownOffset;
        if (IsReadableMemory(slot, sizeof(IUnknown*))) {
            IUnknown* taskbarElementIUnknown = *static_cast<IUnknown**>(slot);
            if (taskbarElementIUnknown) {
                FrameworkElement taskbarElement = nullptr;
                taskbarElementIUnknown->QueryInterface(
                    winrt::guid_of<FrameworkElement>(),
                    winrt::put_abi(taskbarElement));
                if (taskbarElement) {
                    result = taskbarElement.XamlRoot();
                }
            }
        }
    }

    if (taskbarHostSharedPtr[1]) {
        std__Ref_count_base__Decref_Original(taskbarHostSharedPtr[1]);
    }

    return result;
}

// ===== Running code on the taskbar UI thread =====

using RunFromWindowThreadProc_t = void (*)(void* parameter);

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
                const CWPSTRUCT* cwp =
                    reinterpret_cast<const CWPSTRUCT*>(lParam);
                if (cwp->message == runFromWindowThreadRegisteredMsg) {
                    RUN_FROM_WINDOW_THREAD_PARAM* param =
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

    RUN_FROM_WINDOW_THREAD_PARAM param;
    param.proc = proc;
    param.procParam = procParam;
    SendMessage(hWnd, runFromWindowThreadRegisteredMsg, 0,
                reinterpret_cast<LPARAM>(&param));

    UnhookWindowsHookEx(hook);

    return true;
}

// ===== Network status subscription =====

void SubscribeNetworkStatusChanged() {
    if (g_networkSubscribed) {
        return;
    }

    // The guard keeps the handler alive while it may still be invoked, so that
    // unloading can wait for an in-flight invocation before the mod is
    // unmapped.
    auto guard = std::make_shared<NetworkSubscriptionMarker>();

    try {
        g_networkStatusToken =
            winrt::Windows::Networking::Connectivity::NetworkInformation::
                NetworkStatusChanged(
                    [guard](winrt::Windows::Foundation::IInspectable const&) {
                        g_networkDirty.store(true,
                                             std::memory_order_relaxed);
                    });
        g_networkCallbackRef = std::move(guard);
        g_networkSubscribed = true;
        g_networkDirty.store(true, std::memory_order_relaxed);
        Wh_Log(L"Subscribed to network status changes");
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"Failed to subscribe to network status changes: %08X",
               e.code().value);
    } catch (...) {
        Wh_Log(L"Failed to subscribe to network status changes");
    }
}

void UnsubscribeNetworkStatusChanged() {
    if (!g_networkSubscribed) {
        return;
    }

    g_networkSubscribed = false;

    try {
        winrt::Windows::Networking::Connectivity::NetworkInformation::
            NetworkStatusChanged(g_networkStatusToken);
    } catch (...) {
    }
    g_networkStatusToken = {};

    // Removing the handler doesn't wait for an invocation which is already in
    // flight, and the handler runs mod code, so wait for it before the mod
    // gets unmapped.
    for (int i = 0; i < 500 && g_networkCallbackRef.use_count() > 1; i++) {
        Sleep(1);
    }
    g_networkCallbackRef.reset();

    Wh_Log(L"Unsubscribed from network status changes");
}

// ===== Widget lifecycle =====

bool StartWidgetUi();

void RestoreInstance(WidgetInstance& instance) {
    try {
        if (auto host = instance.hostPanel.get()) {
            if (instance.canvas) {
                RemoveFromHost(host, instance.canvas);
            }
        }
    } catch (...) {
        Wh_Log(L"Failed to remove the widget canvas");
    }

    if (auto nativeContent = instance.nativeContent.get()) {
        try {
            if (instance.savedNativeVisibility) {
                nativeContent.Visibility(*instance.savedNativeVisibility);
            } else {
                nativeContent.ClearValue(UIElement::VisibilityProperty());
            }
        } catch (...) {
            Wh_Log(L"Failed to restore the native clock");
        }
    }

    ReleaseInstanceVisuals(instance);
    instance.nativeContent = nullptr;
    instance.hostPanel = nullptr;
    instance.savedNativeVisibility.reset();
}

bool InjectIntoClock(FrameworkElement clockContent) {
    if (g_unloading || !g_instances || !clockContent) {
        return false;
    }

    for (auto& instance : *g_instances) {
        if (instance.nativeContent.get() == clockContent) {
            return true;
        }
    }

    auto host =
        Media::VisualTreeHelper::GetParent(clockContent).try_as<Controls::Panel>();
    if (!host) {
        Wh_Log(L"Failed to get the clock content parent");
        return false;
    }

    WidgetInstance instance;
    instance.nativeContent = clockContent;
    instance.hostPanel = host;
    instance.savedNativeVisibility =
        clockContent.ReadLocalValue(UIElement::VisibilityProperty())
            .try_as<Visibility>();

    BuildInstanceVisuals(instance);
    if (!instance.canvas) {
        return false;
    }

    try {
        // Remove a leftover widget of a previous load of the mod, if any.
        auto children = host.Children();
        for (uint32_t i = children.Size(); i-- > 0;) {
            auto element = children.GetAt(i).try_as<FrameworkElement>();
            if (element && element.Name() == kWidgetRootName) {
                children.RemoveAt(i);
            }
        }

        children.Append(instance.canvas);
        clockContent.Visibility(Visibility::Collapsed);
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"Failed to attach the widget: %08X", e.code().value);
        ReleaseInstanceVisuals(instance);
        return false;
    } catch (...) {
        Wh_Log(L"Failed to attach the widget");
        ReleaseInstanceVisuals(instance);
        return false;
    }

    g_instances->push_back(std::move(instance));
    RefreshInstance(g_instances->back(), true);

    Wh_Log(L"Attached a widget to a taskbar clock");
    return true;
}

void InjectIntoClockWhenLoaded(FrameworkElement clockContent) {
    if (!g_autoRevokerList) {
        return;
    }

    // The revoker is kept until the mod unloads, which revokes the handler.
    g_autoRevokerList->emplace_back();
    auto autoRevokerIt = g_autoRevokerList->end();
    --autoRevokerIt;

    *autoRevokerIt = clockContent.Loaded(
        winrt::auto_revoke_t{},
        [](winrt::Windows::Foundation::IInspectable const& sender,
           RoutedEventArgs const&) {
            if (g_unloading || !g_instances) {
                return;
            }

            auto clockContent = sender.try_as<FrameworkElement>();
            if (!clockContent) {
                return;
            }

            try {
                if (!g_timer) {
                    StartWidgetUi();
                } else {
                    InjectIntoClock(clockContent);
                }
            } catch (winrt::hresult_error const& e) {
                Wh_Log(L"Failed to replace a clock: %08X", e.code().value);
            } catch (...) {
                Wh_Log(L"Failed to replace a clock");
            }
        });
}

void PruneInstances() {
    if (!g_instances) {
        return;
    }

    auto& instances = *g_instances;
    for (size_t i = instances.size(); i-- > 0;) {
        auto nativeContent = instances[i].nativeContent.get();
        if (!nativeContent || !nativeContent.IsLoaded()) {
            RestoreInstance(instances[i]);
            instances.erase(instances.begin() +
                            static_cast<std::ptrdiff_t>(i));
        }
    }
}

void RebuildInstance(WidgetInstance& instance) {
    auto host = instance.hostPanel.get();
    auto nativeContent = instance.nativeContent.get();
    if (!host || !nativeContent) {
        return;
    }

    try {
        if (instance.canvas) {
            RemoveFromHost(host, instance.canvas);
        }
    } catch (...) {
    }

    ReleaseInstanceVisuals(instance);
    BuildInstanceVisuals(instance);
    if (!instance.canvas) {
        return;
    }

    try {
        host.Children().Append(instance.canvas);
        nativeContent.Visibility(Visibility::Collapsed);
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"Failed to rebuild the widget: %08X", e.code().value);
        ReleaseInstanceVisuals(instance);
        return;
    } catch (...) {
        Wh_Log(L"Failed to rebuild the widget");
        ReleaseInstanceVisuals(instance);
        return;
    }

    RefreshInstance(instance, true);
}

void RebuildAllInstances() {
    if (!g_instances) {
        return;
    }

    int requestedSize =
        std::clamp(g_settings.size, kMinWidgetSize, kMaxWidgetSize);

    for (auto& instance : *g_instances) {
        if (instance.size != requestedSize || !instance.tuningValid ||
            !TuningEquals(instance.tuning, g_tuning)) {
            RebuildInstance(instance);
        }
    }
}

void ScanForClocks() {
    if (g_unloading || !g_instances) {
        return;
    }

    std::vector<TaskbarWindow> taskbars;
    CollectTaskbarWindows(taskbars);

    for (const auto& taskbar : taskbars) {
        XamlRoot xamlRoot = GetTaskbarXamlRoot(taskbar.hWnd, taskbar.secondary);
        if (!xamlRoot) {
            continue;
        }

        auto content = xamlRoot.Content();
        if (!content) {
            continue;
        }

        std::vector<FrameworkElement> clocks;
        FindChildrenByClassName(content, L"SystemTray.DateTimeIconContent", 64,
                                clocks);
        for (const auto& clock : clocks) {
            InjectIntoClock(clock);
        }
    }
}

void RefreshWidgets(bool force) {
    if (g_unloading || !g_instances) {
        return;
    }

    PruneInstances();

    bool scan = force;
    if (g_instances->empty() && g_startupScansLeft > 0) {
        g_startupScansLeft--;
        scan = true;
    } else if (!g_clockHookApplied && g_tickCounter % 4 == 0) {
        scan = true;
    }
    if (scan) {
        ScanForClocks();
    }

    if (g_instances->empty()) {
        return;
    }

    SYSTEMTIME st{};
    GetLocalTime(&st);
    std::wstring timeString = FormatTimeString(st);
    std::wstring dateString = FormatDateString(st);
    BatteryInfo battery = ReadBatteryInfo();

    // The network state is only re-read after a NetworkStatusChanged event
    // (or when forced), so no out-of-process calls run on the UI thread every
    // second.
    bool networkDirty = g_networkDirty.exchange(false);
    if (force) {
        networkDirty = true;
    }
    std::optional<NetworkInfo> network;
    if (networkDirty) {
        network = ReadNetworkInfo();
    }

    for (auto& instance : *g_instances) {
        UpdateTheme(instance, force);
        UpdateTimeAndDate(instance, timeString, dateString, force);
        UpdateBattery(instance, battery, force);
        if (network) {
            UpdateNetwork(instance, *network, force);
        }
    }
}

void OnTimerTick() {
    if (g_unloading || !g_instances) {
        return;
    }

    g_tickCounter++;

    try {
        RefreshWidgets(false);
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"Refresh failed: %08X", e.code().value);
    } catch (...) {
        Wh_Log(L"Refresh failed");
    }
}

bool StartWidgetUi() {
    if (g_unloading || !g_instances) {
        return false;
    }

    if (g_timer) {
        return true;
    }

    SubscribeNetworkStatusChanged();

    try {
        DispatcherTimer timer;
        timer.Interval(winrt::Windows::Foundation::TimeSpan{
            std::chrono::seconds(1)});
        g_timerToken = timer.Tick(
            [](winrt::Windows::Foundation::IInspectable const&,
               winrt::Windows::Foundation::IInspectable const&) {
                OnTimerTick();
            });
        timer.Start();
        g_timer = timer;
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"Failed to start the timer: %08X", e.code().value);
        UnsubscribeNetworkStatusChanged();
        return false;
    } catch (...) {
        Wh_Log(L"Failed to start the timer");
        UnsubscribeNetworkStatusChanged();
        return false;
    }

    g_startupScansLeft = 5;
    g_tickCounter = 0;

    ScanForClocks();
    RefreshWidgets(true);

    return true;
}

void StopWidgetUi() {
    Wh_Log(L"Stopping the widget UI");

    StopAnimTimer();

    if (g_timer) {
        try {
            g_timer.Stop();
            g_timer.Tick(g_timerToken);
        } catch (...) {
        }
        g_timer = nullptr;
        g_timerToken = {};
    }

    // Revoked on the UI thread, after the timers stopped.
    UnsubscribeNetworkStatusChanged();

    if (g_autoRevokerList) {
        g_autoRevokerList->clear();
        g_autoRevokerList.reset();
    }

    if (g_instances) {
        for (auto& instance : *g_instances) {
            RestoreInstance(instance);
        }
        // Release the XAML objects here, on the taskbar UI thread.
        g_instances->clear();
        g_instances.reset();
    }
}

// ===== System tray module hooks =====

using DateTimeIconContent_OnApplyTemplate_t = HRESULT(WINAPI*)(void* pThis);
DateTimeIconContent_OnApplyTemplate_t
    DateTimeIconContent_OnApplyTemplate_Original;

HRESULT WINAPI DateTimeIconContent_OnApplyTemplate_Hook(void* pThis) {
    Wh_Log(L">");

    HRESULT ret = DateTimeIconContent_OnApplyTemplate_Original(pThis);

    if (g_unloading || !g_instances) {
        return ret;
    }

    try {
        FrameworkElement clockContent = nullptr;
        static_cast<IUnknown*>(pThis)->QueryInterface(
            winrt::guid_of<FrameworkElement>(), winrt::put_abi(clockContent));
        if (!clockContent) {
            return ret;
        }

        // A clock was created, e.g. for a new monitor or a rebuilt taskbar.
        // This runs on the taskbar UI thread. If its parent isn't available
        // yet, retry once it's loaded.
        if (!g_timer) {
            StartWidgetUi();
        } else if (!InjectIntoClock(clockContent)) {
            InjectIntoClockWhenLoaded(clockContent);
        }
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"Failed to replace a clock: %08X", e.code().value);
    } catch (...) {
        Wh_Log(L"Failed to replace a clock");
    }

    return ret;
}

bool HookSystemTraySymbols(HMODULE module) {
    // SystemTray.dll, Taskbar.View.dll, ExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::SystemTray::implementation::DateTimeIconContent,struct winrt::Windows::UI::Xaml::IFrameworkElementOverrides>::OnApplyTemplate(void))"},
            &DateTimeIconContent_OnApplyTemplate_Original,
            DateTimeIconContent_OnApplyTemplate_Hook,
            true,
        },
    };

    if (!HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"HookSymbols failed for the system tray module");
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
            g_clockHookApplied =
                DateTimeIconContent_OnApplyTemplate_Original != nullptr;
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
        // Only needed for the clocks of secondary monitors.
        {
            {LR"(const CSecondaryTaskBand::`vftable'{for `ITaskListWndSite'})"},
            &CSecondaryTaskBand_ITaskListWndSite_vftable,
            nullptr,
            true,
        },
        {
            {LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CSecondaryTaskBand::GetTaskbarHost(void)const )"},
            &CSecondaryTaskBand_GetTaskbarHost_Original,
            nullptr,
            true,
        },
    };

    return HookSymbols(module, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks));
}

// ===== Settings =====

void LoadSettings() {
    g_settings.size = std::clamp(Wh_GetIntSetting(L"size"), kMinWidgetSize,
                                 kMaxWidgetSize);
    g_settings.showDate = Wh_GetIntSetting(L"showDate") != 0;
    g_settings.showNetwork = Wh_GetIntSetting(L"showNetwork") != 0;

    auto timeFormat = WindhawkUtils::StringSetting::make(L"timeFormat");
    g_settings.timeFormat = TimeFormatSetting::autoFormat;
    if (wcscmp(timeFormat.get(), L"h24") == 0) {
        g_settings.timeFormat = TimeFormatSetting::h24;
    } else if (wcscmp(timeFormat.get(), L"h12") == 0) {
        g_settings.timeFormat = TimeFormatSetting::h12;
    }

    auto dateFormat = WindhawkUtils::StringSetting::make(L"dateFormat");
    g_settings.dateFormat = DateFormatSetting::monthDay;
    if (wcscmp(dateFormat.get(), L"dayMonth") == 0) {
        g_settings.dateFormat = DateFormatSetting::dayMonth;
    }

    g_tuning.ringDiameter = GetDoubleSetting(L"ringDiameter", 35.0, 10.0, 48.0);
    g_tuning.ringThickness =
        GetDoubleSetting(L"ringThickness", 3.7, 0.5, 12.0);
    g_tuning.ringSweepDeg =
        GetDoubleSetting(L"ringSweepDeg", 263.0, 90.0, 360.0);
    g_tuning.trackOpacityPct =
        GetDoubleSetting(L"ringTrackOpacity", 28.0, 0.0, 100.0);
    g_tuning.chargingNotchDeg =
        GetDoubleSetting(L"chargingNotchDeg", 48.0, 0.0, 120.0);
    g_tuning.boltWidth = GetDoubleSetting(L"boltWidth", 7.2, 0.5, 24.0);
    g_tuning.boltHeight = GetDoubleSetting(L"boltHeight", 9.1, 0.5, 28.0);
    g_tuning.wifiHeight = GetDoubleSetting(L"wifiHeight", 10.5, 3.0, 40.0);
    g_tuning.wifiWidth = GetDoubleSetting(L"wifiWidth", 14.5, 3.0, 56.0);
    g_tuning.wifiThickness = GetDoubleSetting(L"wifiThickness", 2.0, 0.3, 12.0);
    g_tuning.wifiOpeningDeg =
        GetDoubleSetting(L"wifiOpeningDeg", 75.0, 20.0, 180.0);
    g_tuning.wifiCenterPct =
        GetDoubleSetting(L"wifiCenterPct", 41.0, 5.0, 95.0);
    g_tuning.wifiChargingDrop =
        GetDoubleSetting(L"wifiChargingDrop", 1.5, 0.0, 12.0);
    g_tuning.digitsSize = GetDoubleSetting(L"digitsSize", 7.0, 2.0, 16.0);
    g_tuning.timeRowPct = GetDoubleSetting(L"timeRowPct", 73.0, 5.0, 98.0);
    g_tuning.dateRowPct = GetDoubleSetting(L"dateRowPct", 89.5, 5.0, 100.0);
    g_tuning.digitsSpacing =
        std::clamp(Wh_GetIntSetting(L"digitsSpacing"), 0, 400);
}

void ApplySettingsOnTaskbarThread() {
    // Cleanup is unconditional and always runs on the taskbar UI thread, so
    // that no XAML object is touched from the engine thread on unload.
    if (g_unloading) {
        StopWidgetUi();
        return;
    }

    if (!g_instances) {
        return;
    }

    if (!StartWidgetUi()) {
        return;
    }

    RebuildAllInstances();
    RefreshWidgets(true);
}

void ApplySettings() {
    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (!hTaskbarWnd) {
        Wh_Log(L"No taskbar window found");
        return;
    }

    RunFromWindowThread(
        hTaskbarWnd,
        [](void*) {
            try {
                ApplySettingsOnTaskbarThread();
            } catch (winrt::hresult_error const& e) {
                Wh_Log(L"Failed to apply settings: %08X", e.code().value);
            } catch (...) {
                Wh_Log(L"Failed to apply settings");
            }
        },
        nullptr);
}

// ===== Windhawk entry points =====

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    if (HMODULE systemTrayModule = GetSystemTrayModuleHandle()) {
        g_systemTrayModuleHooked = true;
        if (HookSystemTraySymbols(systemTrayModule)) {
            g_clockHookApplied =
                DateTimeIconContent_OnApplyTemplate_Original != nullptr;
        }
        if (!g_clockHookApplied) {
            Wh_Log(L"DateTimeIconContent::OnApplyTemplate not found, clocks are "
                   L"found by scanning the taskbar");
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
                    g_clockHookApplied =
                        DateTimeIconContent_OnApplyTemplate_Original != nullptr;
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

    // Stops the timers and releases the XAML objects on the taskbar UI thread.
    ApplySettings();
}

void Wh_ModUninit() {
    Wh_Log(L">");

    // XAML objects are released in Wh_ModBeforeUninit -> ApplySettings ->
    // StopWidgetUi on the taskbar UI thread, never on this thread.
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    LoadSettings();

    ApplySettings();
}
