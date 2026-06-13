// ==WindhawkMod==
// @id              taskbar-dock-like
// @name            TAI (taskbar as island) for Windows 11
// @description     Centers and floats the taskbar, moves the system tray next to the task area, and serves as an all-in-one, one-click mod to transform the taskbar into an animated dock.
// @version         1.5.247
// @author          DarkionAvey
// @github          https://github.com/DarkionAvey
// @include         explorer.exe
// @include         StartMenuExperienceHost.exe
// @include         ShellHost.exe
// @include         ShellExperienceHost.exe
// @compilerOptions -ldwmapi -lole32 -loleaut32 -lruntimeobject -lshcore -lcomctl32 -lversion
// @architecture    x86-64
// ==/WindhawkMod==
// This mod is based on code released under the GNU General Public License v3.0.
// For bug reports and feedback:
// https://github.com/DarkionAvey/windhawk-taskbar-centered-condensed
// ==WindhawkModReadme==
/*
![Screenshot](https://github.com/DarkionAvey/windhawk-taskbar-centered-condensed/raw/main/screenshot.gif)
# TAI (Taskbar as (an) island) for Windows 11
TAI lets you transform your Windows 11 taskbar into a smooth floating dock without losing any of the original taskbar functionality!
# Bug reports
Please file bug reports here: https://github.com/DarkionAvey/windhawk-taskbar-centered-condensed/issues
---
## 🚀 How to Install (Development Build)
⚠️ **Note:** Please disable any mods that affect taskbar height or taskbar icons—this mod already includes those
features.
1. [Install Windhawk](https://windhawk.net/) if you haven't already.
2. Copy the contents of [
   `assembled-mod.cpp`](https://raw.githubusercontent.com/DarkionAvey/windhawk-taskbar-centered-condensed/main/assembled-mod.cpp)
   to your clipboard.
3. Open **WindHawk** and navigate to: `Explore` → `Create a new mod`.
4. Press `Ctrl+A` to select all, then `Ctrl+V` to paste.
5. Click **Compile Mod** button on the top left corner.
6. Change the mod's settings to fit your preference.
---
## 🛠 Source Code
The actual mod code is split into files under [
`mod-parts/`](https://github.com/DarkionAvey/windhawk-taskbar-centered-condensed/blob/main/mod-parts/), which are later merged together using a Python script.
❗ **Do not edit `assembled-mod.cpp` manually**, as any changes will be overwritten in the next build cycle. Instead,
modify the source files in the `mod-parts` directory.
---
## 🙌 Credits
Huge thanks to these awesome developers who made this mod possible -- your contributions to modding Windows are truly appreciated!:
- [`Michael Maltsev (m417z)`](https://github.com/m417z)
- [`Valentin Radu (valinet)`](https://github.com/valinet)
- [`TranslucentTB team`](https://github.com/translucenttb/translucenttb)
---
## 🔥 Recommended Mods
- [Taskbar Fluent Media Player](https://windhawk.net/mods/taskbar-fluent-media-player) \[Set player position to Tray\]
- [Smart Auto Hide for Taskbar](https://windhawk.net/mods/taskbar-auto-hide-when-maximized)
- [Taskbar Auto-Hide Speed/Frame Rate](https://windhawk.net/mods/taskbar-auto-hide-speed)
- [Show All Tray Icons](https://windhawk.net/mods/taskbar-notification-icons-show-all)
---
# Options
| Property | Name | Description | Accepted values |
| --- | --- | --- | --- |
| `TaskbarHeight` | Taskbar height | Set the height of the taskbar. Default is 74 | Non-negative integer |
| `TaskbarIconSize` | Taskbar icon size | Set the width and height of taskbar icons. Values below 8 are clamped to 8; values above the current taskbar/button size are clamped to fit. Default is 42 | Non-negative integer |
| `TaskbarButtonSize` | Taskbar button size | Set the size (width and height) of taskbar buttons. Default is 72 | Non-negative integer |
| `TaskbarOffsetY` | Taskbar vertical offset | Move the taskbar up or down. Padding of the same value is applied to the top. Default is 6 | Non-negative integer |
| `TrayTaskGap` | Tray task gap | Adjust the space between the task area and the tray area. Default is 10 | Non-negative integer |
| `TaskbarBackgroundHorizontalPadding` | Taskbar background horizontal padding | Set the horizontal padding on both sides of the taskbar background. Default is 2 | Non-negative integer |
| `FullWidthTaskbarBackground` | Full-width taskbar background | When enabled, the taskbar background fills the entire width of the screen, similar to the default Windows behavior. Default is off | Boolean (true/false) |
| `IgnoreShowDesktopButton` | Ignore "Show Desktop" button | When enabled, the "Show Desktop" button is ignored in width calculations. Default is off | Boolean (true/false) |
| `TaskbarCornerRadius` | Taskbar corner radius | Controls how rounded the taskbar corners appear. Default is 22 | Non-negative integer |
| `TaskButtonCornerRadius` | Task button corner radius | Controls how rounded the corners of individual task buttons are. Default is 18 | Non-negative integer |
| `FlatTaskbarBottomCorners` | Flat bottom corners | When enabled, the bottom corners of the taskbar will be squared and the taskbar will dock to the screen edge. This overrides the taskbar offset; this is always on with the full-width taskbar background option. Default is off | Boolean (true/false) |
| `CustomizeTaskbarBackground` | Stylize the taskbar background | When enabled, this mod applies its taskbar background visuals. When disabled, this mod skips all taskbar background changes so other Windhawk mods can provide their own background. Default is on | Boolean (true/false) |
| `DisableCustomBlurBackground` | Disable custom blur background | When enabled, the WindhawkBlur brush is skipped and only the fallback color is used, producing a solid background. Background blur, tint, tint color, luminosity, saturation, and inversion settings are ignored. This also activates automatically when background tint is 100, background blur amount is 0, or background inversion is 100. Default is off | Boolean (true/false) |
| `TaskbarBackgroundOpacity` | Background opacity | Adjust the opacity of the taskbar background. 0 = fully transparent, 100 = fully opaque. Default is 100 | Non-negative integer |
| `TaskbarBackgroundTint` | Background tint | Modify the taskbar tint level. Higher values = more tint. Range 0-100. Default is 0 | Non-negative integer |
| `TaskbarBackgroundLuminosity` | Background luminosity | Adjust luminosity of the taskbar background. Higher values = more opaque, lower values = more glass-like. Range 0-100. Default is 20 | Non-negative integer |
| `TaskbarBackgroundBlurAmount` | Background blur amount | WindhawkBlur Gaussian blur amount. Higher values make the taskbar glass blur stronger. Must be non-negative. Default is 30 | Non-negative integer |
| `TaskbarBackgroundTintColor` | Background tint color | WindhawkBlur tint color. Accepts `#RRGGBB`, `#AARRGGBB`, or {ThemeResource Name}. Default is {ThemeResource CardStrokeColorDefaultSolid}. Use {ThemeResource SystemBaseHighColor} if you want white color when Windows is in dark mode, or black in light mode. | Text |
| `TaskbarBackgroundTintSaturation` | Background saturation | WindhawkBlur saturation applied before tint. 0 = grayscale, 100 = normal, 200-500 = boosted saturation. Must be non-negative. Default is 200. Max is 500 | Non-negative integer |
| `TaskbarBackgroundInversion` | Background inversion | Inverts the blurred background behind the taskbar to enhance contrast. 0 = off, 100 = fully inverted. Default is 10 | Non-negative integer |
| `TaskbarBackgroundFallbackColor` | Background fallback color | Color used when transparency effects or energy saver disable blur, or when the custom blur background is disabled. Accepts `#RRGGBB`, `#AARRGGBB`, or {ThemeResource Name}. Default is {ThemeResource CardStrokeColorDefaultSolid} | Text |
| `TaskbarBorderOpacity` | Border opacity | Set the opacity of the taskbar border, as well as the app dividers. Range 0-100. Default is 10 | Non-negative integer |
| `TaskbarBorderColorHex` | Border color (HEX) | Set the color of the taskbar border and app dividers, Hex color as `#RRGGBB`. Default is `#ffffff` | string hex color |
| `TaskbarBorderThickness` | Taskbar border thickness scale (%) | Set the scale of the taskbar border. Range 0-100. Default is 8 | unsigned int percentage |
| `AppsDividerThickness` | Apps divider thickness scale (%) | Set the thickness scale of the taskbar dividers. Range 0-100. Default is 8 | unsigned int percentage |
| `AppsDividerVerticalScale` | Apps divider vertical scale (%) | Set the vertical scale of the taskbar dividers. Range 0-100. Default is 40 | unsigned int percentage |
| `AppsDividerAlignment` | Choose the side on which the app dividers should appear |  |  |
| `DividedAppNames` | App names for divider placement | Type partial app names where you'd like a divider to appear. Use ; to separate multiple entries (e.g., Steam; Notepad\+\+; Settings). Case-insensitive and supports regex. | string regex |
| `TrayAreaDivider` | Tray area divider | When enabled, the tray area will be separated by a divider. Default is off | Boolean (true/false) |
| `StyleTrayArea` | Modify the tray area appearance | When enabled, the options for tray icon size will take effect. Default is off | Boolean (true/false) |
| `TrayIconSize` | Tray icon size | Set the width and height of tray icons. Minimum is 15. Default is 15 | Non-negative integer |
| `TrayButtonSize` | Tray button size | Set the size (width and height) of tray buttons. Minimum is 20. Default is 30 | Non-negative integer |
| `MoveFlyoutStartMenu` | Move Start Menu with Taskbar | When enabled, the Start and Search menus are moved to align with taskbar size and location. Default is on. | Boolean (true/false) |
| `MoveFlyoutControlCenter` | Move Control Center with Taskbar | When enabled, the Control Center is moved to align with taskbar size and location. Default is on. | Boolean (true/false) |
| `MoveFlyoutNotificationCenter` | Move Notification Center with Taskbar | When enabled, the Notification Center is moved to align with taskbar size and location. Default is on. | Boolean (true/false) |
| `AlignFlyoutInner` | Align flyout windows to the inside of the taskbar | When enabled, the flyout windows will be aligned within the bounds of the taskbar. When off, they will be 50% inside the taskbar bounds. Default is on. | Boolean (true/false) |
*/
// ==/WindhawkModReadme==
// ==WindhawkModSettings==
/*
- TaskbarHeight: 74
  $name: Taskbar height
  $description: Set the height of the taskbar. Default is 74
- TaskbarIconSize: 42
  $name: Taskbar icon size
  $description: Set the width and height of taskbar icons. Values below 8 are clamped to 8; values above the current taskbar/button size are clamped to fit. Default is 42
- TaskbarButtonSize: 72
  $name: Taskbar button size
  $description: Set the size (width and height) of taskbar buttons. Default is 72
- TaskbarOffsetY: 6
  $name: Taskbar vertical offset
  $description: Move the taskbar up or down. Padding of the same value is applied to the top. Default is 6
- TrayTaskGap: 10
  $name: Tray task gap
  $description: Adjust the space between the task area and the tray area. Default is 10
- TaskbarBackgroundHorizontalPadding: 2
  $name: Taskbar background horizontal padding
  $description: Set the horizontal padding on both sides of the taskbar background. Default is 2
- FullWidthTaskbarBackground: false
  $name: Full-width taskbar background
  $description: When enabled, the taskbar background fills the entire width of the screen, similar to the default Windows behavior. Default is off
- IgnoreShowDesktopButton: false
  $name: Ignore "Show Desktop" button
  $description: When enabled, the "Show Desktop" button is ignored in width calculations. Default is off
- TaskbarCornerRadius: 22
  $name: Taskbar corner radius
  $description: Controls how rounded the taskbar corners appear. Default is 22
- TaskButtonCornerRadius: 18
  $name: Task button corner radius
  $description: Controls how rounded the corners of individual task buttons are. Default is 18
- FlatTaskbarBottomCorners: false
  $name: Flat bottom corners
  $description: When enabled, the bottom corners of the taskbar will be squared and the taskbar will dock to the screen edge. This overrides the taskbar offset; this is always on with the full-width taskbar background option. Default is off
- CustomizeTaskbarBackground: true
  $name: Stylize the taskbar background
  $description: When enabled, this mod applies its taskbar background visuals. When disabled, this mod skips all taskbar background changes so other Windhawk mods can provide their own background. Default is on
- DisableCustomBlurBackground: false
  $name: Disable custom blur background
  $description: When enabled, the WindhawkBlur brush is skipped and only the fallback color is used, producing a solid background. Background blur, tint, tint color, luminosity, saturation, and inversion settings are ignored. This also activates automatically when background tint is 100, background blur amount is 0, or background inversion is 100. Default is off
- TaskbarBackgroundOpacity: 100
  $name: Background opacity
  $description: Adjust the opacity of the taskbar background. 0 = fully transparent, 100 = fully opaque. Default is 100
- TaskbarBackgroundTint: 0
  $name: Background tint
  $description: Modify the taskbar tint level. Higher values = more tint. Range 0-100. Default is 0
- TaskbarBackgroundLuminosity: 20
  $name: Background luminosity
  $description: Adjust luminosity of the taskbar background. Higher values = more opaque, lower values = more glass-like. Range 0-100. Default is 20
- TaskbarBackgroundBlurAmount: 30
  $name: Background blur amount
  $description: WindhawkBlur Gaussian blur amount. Higher values make the taskbar glass blur stronger. Must be non-negative. Default is 30
- TaskbarBackgroundTintColor: "{ThemeResource CardStrokeColorDefaultSolid}"
  $name: Background tint color
  $description: WindhawkBlur tint color. Accepts `#RRGGBB`, `#AARRGGBB`, or {ThemeResource Name}. Default is {ThemeResource CardStrokeColorDefaultSolid}. Use {ThemeResource SystemBaseHighColor} if you want white color when Windows is in dark mode, or black in light mode.
- TaskbarBackgroundTintSaturation: 200
  $name: Background saturation
  $description: WindhawkBlur saturation applied before tint. 0 = grayscale, 100 = normal, 200-500 = boosted saturation. Must be non-negative. Default is 200. Max is 500
- TaskbarBackgroundInversion: 10
  $name: Background inversion
  $description: Inverts the blurred background behind the taskbar to enhance contrast. 0 = off, 100 = fully inverted. Default is 10
- TaskbarBackgroundFallbackColor: "{ThemeResource CardStrokeColorDefaultSolid}"
  $name: Background fallback color
  $description: Color used when transparency effects or energy saver disable blur, or when the custom blur background is disabled. Accepts `#RRGGBB`, `#AARRGGBB`, or {ThemeResource Name}. Default is {ThemeResource CardStrokeColorDefaultSolid}
- TaskbarBorderOpacity: 10
  $name: Border opacity
  $description: Set the opacity of the taskbar border, as well as the app dividers. Range 0-100. Default is 10
- TaskbarBorderColorHex: "#ffffff"
  $name: Border color (HEX)
  $description: Set the color of the taskbar border and app dividers, Hex color as `#RRGGBB`. Default is `#ffffff`
- TaskbarBorderThickness: 8
  $name: Taskbar border thickness scale (%)
  $description: Set the scale of the taskbar border. Range 0-100. Default is 8
- AppsDividerThickness: 8
  $name: Apps divider thickness scale (%)
  $description: Set the thickness scale of the taskbar dividers. Range 0-100. Default is 8
- AppsDividerVerticalScale: 40
  $name: Apps divider vertical scale (%)
  $description: Set the vertical scale of the taskbar dividers. Range 0-100. Default is 40
- AppsDividerAlignment: ""
  $name: Choose the side on which the app dividers should appear
  $options:
  - left: Left side
  - right: Right side
- DividedAppNames: ""
  $name: App names for divider placement
  $description: Type partial app names where you'd like a divider to appear. Use ; to separate multiple entries (e.g., Steam; Notepad\+\+; Settings). Case-insensitive and supports regex.
- TrayAreaDivider: false
  $name: Tray area divider
  $description: When enabled, the tray area will be separated by a divider. Default is off
- StyleTrayArea: false
  $name: Modify the tray area appearance
  $description: When enabled, the options for tray icon size will take effect. Default is off
- TrayIconSize: 15
  $name: Tray icon size
  $description: Set the width and height of tray icons. Minimum is 15. Default is 15
- TrayButtonSize: 30
  $name: Tray button size
  $description: Set the size (width and height) of tray buttons. Minimum is 20. Default is 30
- MoveFlyoutStartMenu: true
  $name: Move Start Menu with Taskbar
  $description: When enabled, the Start and Search menus are moved to align with taskbar size and location. Default is on.
- MoveFlyoutControlCenter: true
  $name: Move Control Center with Taskbar
  $description: When enabled, the Control Center is moved to align with taskbar size and location. Default is on.
- MoveFlyoutNotificationCenter: true
  $name: Move Notification Center with Taskbar
  $description: When enabled, the Notification Center is moved to align with taskbar size and location. Default is on.
- AlignFlyoutInner: true
  $name: Align flyout windows to the inside of the taskbar
  $description: When enabled, the flyout windows will be aligned within the bounds of the taskbar. When off, they will be 50% inside the taskbar bounds. Default is on.
*/
// ==/WindhawkModSettings==
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////  .___________..______    __    ______   ______   .__   __.      _______. __   ________   _______   ////
////  |           ||   _  \  |  |  /      | /  __  \  |  \ |  |     /       ||  | |       /  |   ____|  ////
////  `---|  |----`|  |_)  | |  | |  ,----'|  |  |  | |   \|  |    |   (----`|  | `---/  /   |  |__     ////
////      |  |     |   _  <  |  | |  |     |  |  |  | |  . `  |     \   \    |  |    /  /    |   __|    ////
////      |  |     |  |_)  | |  | |  `----.|  `--'  | |  |\   | .----)   |   |  |   /  /----.|  |____   ////
////      |__|     |______/  |__|  \______| \______/  |__| \__| |_______/    |__|  /________||_______|  ////
////                                                                                                    ////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <windhawk_utils.h>
#undef GetCurrentTime
#include <initguid.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <atomic>
#include <functional>
#include <limits>
#include <optional>
#include <regex>
#include <string>
#include <string_view>
using namespace winrt::Windows::UI::Xaml;
struct {
    int taskbarHeight;
    int iconSize;
    int taskbarButtonWidth;
    int iconSizeSmall;
    int taskbarButtonWidthSmall;
} g_settings_tbiconsize;
std::atomic<bool> g_systemTrayModuleHooked;
std::atomic<bool> g_taskbarViewDllLoadedTBIconSize;
std::atomic<bool> g_searchUxUiDllLoaded;
std::atomic<bool> g_applyingSettings;
std::atomic<bool> g_pendingMeasureOverride;
std::atomic<bool> g_unloading;
std::atomic<int> g_hookCallCounter;
bool g_hasDynamicIconScaling;
bool g_smallIconSize;
int g_originalTaskbarHeight;
int g_taskbarHeight;
std::atomic<DWORD> g_shellIconLoaderV2_LoadAsyncIcon__ResumeCoro_ThreadId;
bool g_inSystemTrayController_UpdateFrameSize;
bool g_taskbarButtonWidthCustomized;
constexpr int kDefaultTaskbarHeight = 74;
constexpr int kDefaultTaskbarIconSize = 42;
constexpr int kDefaultTaskbarButtonSize = 74;
constexpr int kDefaultTaskbarOffsetY = 6;
constexpr int kDefaultTrayIconSize = 15;
constexpr int kDefaultTrayButtonSize = 30;
constexpr int kSystemSmallTaskbarIconSize = 16;
constexpr int kSystemMediumTaskbarIconSize = 24;
constexpr int kSystemSmallTaskbarButtonSize = 32;
constexpr int kSystemMediumTaskbarButtonSize = 44;
constexpr int kMinTaskbarHeight = kSystemMediumTaskbarButtonSize;
constexpr int kMaxTaskbarHeight = 200;
constexpr int kMinTaskbarButtonSize = kSystemMediumTaskbarButtonSize;
constexpr int kMaxTaskbarButtonSize = 300;
constexpr int kMinTaskbarIconSize = 8;
constexpr int kMaxTaskbarIconSize = 300;
constexpr int kMinTrayIconSize = 15;
constexpr int kMinTrayButtonSize = 20;
constexpr double kLayoutToleranceDip = 0.5;
constexpr int kWorkerShutdownPollMs = 10;
constexpr int kDelayedApplyWorkerShutdownTimeoutMs = 5000;
constexpr int kAnimationFollowupWorkerShutdownTimeoutMs = 2000;
constexpr int kTaskbarMeasurePollIntervalMs = 100;
constexpr int kTaskbarMeasureOverrideTimeoutMs = 10000;
constexpr int kHookDrainPollIntervalMs = 100;
constexpr int kHookDrainTimeoutMs = 10000;
bool WaitForConditionWithTimeout(std::function<bool()> condition,
                                 int timeoutMs,
                                 int pollIntervalMs);
FrameworkElement TryQueryFrameworkElement(IUnknown* unknown,
                                          PCWSTR context = L"FrameworkElement") {
    if (!unknown) {
        return nullptr;
    }
    FrameworkElement element{nullptr};
    try {
        HRESULT hr = unknown->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                             winrt::put_abi(element));
        if (FAILED(hr) || !element) {
            if (context) {
                Wh_Log(L"%s QueryInterface failed: %08X", context, hr);
            }
            return nullptr;
        }
    } catch (winrt::hresult_error const& ex) {
        if (context) {
            Wh_Log(L"%s QueryInterface threw %08X: %s", context, ex.code(), ex.message().c_str());
        }
        return nullptr;
    } catch (...) {
        if (context) {
            Wh_Log(L"%s QueryInterface threw: %08X", context, winrt::to_hresult());
        }
        return nullptr;
    }
    return element;
}
int ClampInt(int value, int minValue, int maxValue) {
    return value < minValue ? minValue : (value > maxValue ? maxValue : value);
}
int ReadPositiveIntSettingOrDefault(const wchar_t* key, int defaultValue) {
    int value = Wh_GetIntSetting(key);
    return value > 0 ? value : defaultValue;
}
int GetMaxTaskbarIconSizeForLayout(int taskbarHeight, int taskbarButtonSize) {
    int maxIconSize = std::min(kMaxTaskbarIconSize, std::min(taskbarHeight, taskbarButtonSize));
    return std::max(kMinTaskbarIconSize, maxIconSize);
}
bool g_inAugmentedEntryPointButton_UpdateButtonPadding;
double* double_48_value_Original;
WINUSERAPI UINT WINAPI GetDpiForWindow(HWND hwnd);
typedef enum MONITOR_DPI_TYPE {
    MDT_EFFECTIVE_DPI = 0,
    MDT_ANGULAR_DPI = 1,
    MDT_RAW_DPI = 2,
    MDT_DEFAULT = MDT_EFFECTIVE_DPI
} MONITOR_DPI_TYPE;
#include <mutex>
#include <regex>
struct ModSettings {
  int userDefinedTrayTaskGap;
  int userDefinedTaskbarBackgroundHorizontalPadding;
  unsigned int userDefinedTaskbarOffsetY;
  unsigned int userDefinedTaskbarHeight;
  unsigned int userDefinedTaskbarIconSize;
  unsigned int userDefinedTrayIconSize;
  unsigned int userDefinedTaskbarButtonSize;
  unsigned int userDefinedTrayButtonSize;
  float userDefinedTaskbarCornerRadius;
  unsigned int userDefinedTaskButtonCornerRadius;
  bool userDefinedFlatTaskbarBottomCorners;
  unsigned int userDefinedTaskbarBackgroundOpacity;
  unsigned int userDefinedTaskbarBackgroundTint;
  unsigned int userDefinedTaskbarBackgroundLuminosity;
  unsigned int userDefinedTaskbarBackgroundBlurAmount;
  std::wstring userDefinedTaskbarBackgroundTintColor;
  unsigned int userDefinedTaskbarBackgroundTintSaturation;
  unsigned int userDefinedTaskbarBackgroundInversion;
  std::wstring userDefinedTaskbarBackgroundFallbackColor;
  uint8_t userDefinedTaskbarBorderOpacity;
  double userDefinedTaskbarBorderThickness;
  bool userDefinedFullWidthTaskbarBackground;
  bool userDefinedIgnoreShowDesktopButton;
  bool userDefinedStyleTrayArea;
  bool userDefinedTrayAreaDivider;
  unsigned int borderColorR, borderColorG, borderColorB;
  std::vector<std::wregex> compiledDividedAppPatterns;
  bool userDefinedAlignFlyoutInner;
  bool userDefinedCustomizeTaskbarBackground;
  bool userDefinedDisableCustomBlurBackground;
  double userDefinedAppsDividerThickness;
  float userDefinedAppsDividerVerticalScale{0.7};
  bool userDefinedDividerLeftAligned=false;
};
ModSettings g_settings;
std::recursive_mutex g_settingsMutex;
bool GetUserDefinedAlignFlyoutInner() {
  std::lock_guard<std::recursive_mutex> lock(g_settingsMutex);
  return g_settings.userDefinedAlignFlyoutInner;
}
#include <cstdint>
#include <memory>
#include <mutex>
#include <vector>
struct TaskbarChildStyleCache {
  uint64_t generation{0};
  uint64_t signature{0};
  bool valid{false};
};
struct TaskbarState {
  std::recursive_mutex mutex;
  std::chrono::steady_clock::time_point lastApplyStyleTime{};
  struct Data {
    int childrenCount;
    int rightMostEdge;
    unsigned int childrenWidth;
  } lastTaskbarData{};
  unsigned int lastChildrenWidthTaskbar{0};
  unsigned int lastTrayFrameWidth{0};
  float lastTargetWidth{0};
  float lastTargetOffsetX{0};
  float lastTargetOffsetY{0};
  float initOffsetX{-1};
  bool wasOverflowing{false};
  uintptr_t lastOverflowButtonIdentity{0};
  bool overflowButtonSuppressionKnown{false};
  bool overflowButtonSuppressed{false};
  float lastStartButtonXCalculated=0.0f;
  float lastStartButtonXActual=0.0f;
  float lastStartButtonAnchorLeft{0.0f};
  float lastStartButtonAnchorTop{0.0f};
  float lastStartButtonAnchorWidth{0.0f};
  float lastStartButtonAnchorHeight{0.0f};
  bool hasLastStartButtonAnchorRect{false};
  float stableStartButtonAnchorLeft{0.0f};
  float stableStartButtonAnchorTop{0.0f};
  float stableStartButtonAnchorWidth{0.0f};
  float stableStartButtonAnchorHeight{0.0f};
  bool hasStableStartButtonAnchorRect{false};
  int startButtonAnchorStablePasses{0};
  float lastRootWidth=0.0f;
  float lastTargetTaskFrameOffsetX=0.0f;
  bool hasLastTargetTaskFrameOffsetX{false};
  float lastTargetTaskbarIslandScale{1.0f};
  float lastTaskbarIslandScaleCenterX{0.0f};
  bool hasLastTargetTaskbarIslandScale{false};
  float lastObservedRootWidth{0.0f};
  float lastObservedRootHeight{0.0f};
  float lastObservedRasterizationScale{0.0f};
  float lastObservedTaskFrameWidth{0.0f};
  float lastObservedTaskFrameHeight{0.0f};
  bool hasLastDisplayGeometrySignature{false};
  float lastTargetTrayOffsetX{0.0f};
  bool hasLastTargetTrayOffsetX{false};
  float lastTargetWidgetOffsetX{0.0f};
  float lastTargetWidgetOffsetY{0.0f};
  bool hasLastTargetWidgetOffset{false};
  float lastLeftMostEdgeTray{0};
  int lastRightMostEdgeTray{0};
  float lastBackgroundShapeTargetWidth{0.0f};
  float lastBackgroundShapeTargetHeight{0.0f};
  float lastBackgroundShapeTargetOffsetX{0.0f};
  float lastBackgroundShapeTargetOffsetY{0.0f};
  float backgroundAnimationFromWidth{0.0f};
  float backgroundAnimationToWidth{0.0f};
  float backgroundAnimationFromOffsetX{0.0f};
  float backgroundAnimationToOffsetX{0.0f};
  float backgroundAnimationFromOffsetY{0.0f};
  float backgroundAnimationToOffsetY{0.0f};
  int64_t backgroundAnimationStartMs{0};
  uintptr_t backgroundFillIdentity{0};
  uint64_t lastBackgroundStyleGeneration{0};
  bool hasCustomTaskbarBackgroundVisuals{false};
  uint64_t lastDimensionInvalidationGeneration{0};
  TaskbarChildStyleCache taskbarChildStyleCache;
  TaskbarChildStyleCache trayChildStyleCache;
};
struct TaskbarFlyoutStateSnapshot {
  float lastStartButtonXCalculated{0.0f};
  float lastRootWidth{0.0f};
  float lastTargetWidth{0.0f};
  float lastLeftMostEdgeTray{0.0f};
  int lastRightMostEdgeTray{0};
};
static std::mutex g_taskbarStatesMutex;
static std::unordered_map<std::wstring, std::shared_ptr<TaskbarState>> g_taskbarStates;
static std::atomic<uint64_t> g_dimensionInvalidationGeneration{1};
static std::atomic<uint64_t> g_taskbarChildStyleGeneration{1};
static std::atomic<uintptr_t> g_recentTaskbarInvocationMonitor{0};
static std::atomic<ULONGLONG> g_recentTaskbarInvocationTime{0};
bool IsTaskbarWindowClassTai(HWND window) {
  if (!window) {
    return false;
  }
  wchar_t className[64]{};
  if (!GetClassNameW(window, className, ARRAYSIZE(className))) {
    return false;
  }
  return _wcsicmp(className, L"Shell_TrayWnd") == 0 ||
         _wcsicmp(className, L"Shell_SecondaryTrayWnd") == 0;
}
HMONITOR GetTaskbarMonitorFromPointTai(POINT point) {
  for (HWND window = WindowFromPoint(point); window;
       window = GetParent(window)) {
    if (IsTaskbarWindowClassTai(window)) {
      return MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST);
    }
  }
  struct EnumContext {
    POINT point;
    HMONITOR monitor;
  } context{point, nullptr};
  EnumWindows(
      [](HWND window, LPARAM lParam) -> BOOL {
        auto* context = reinterpret_cast<EnumContext*>(lParam);
        if (!context || !IsTaskbarWindowClassTai(window)) {
          return TRUE;
        }
        RECT rect{};
        if (GetWindowRect(window, &rect) &&
            PtInRect(&rect, context->point)) {
          context->monitor =
              MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST);
          return FALSE;
        }
        return TRUE;
      },
      reinterpret_cast<LPARAM>(&context));
  return context.monitor;
}
bool IsTaskbarInvocationInputMessageTai(UINT message) {
  switch (message) {
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_NCLBUTTONDOWN:
    case WM_NCLBUTTONUP:
    case WM_POINTERDOWN:
    case WM_POINTERUP:
    case WM_TOUCH:
      return true;
    default:
      return false;
  }
}
POINT GetCurrentMessagePointTai() {
  const DWORD messagePosition = GetMessagePos();
  return {
      static_cast<short>(LOWORD(messagePosition)),
      static_cast<short>(HIWORD(messagePosition)),
  };
}
void RecordTaskbarInvocationMonitorTai(HWND taskbarWindow, UINT message) {
  if (!IsTaskbarInvocationInputMessageTai(message)) {
    return;
  }
  HMONITOR monitor =
      GetTaskbarMonitorFromPointTai(GetCurrentMessagePointTai());
  if (!monitor && taskbarWindow) {
    monitor =
        MonitorFromWindow(taskbarWindow, MONITOR_DEFAULTTONEAREST);
  }
  if (!monitor) {
    return;
  }
  g_recentTaskbarInvocationMonitor.store(
      reinterpret_cast<uintptr_t>(monitor), std::memory_order_release);
  g_recentTaskbarInvocationTime.store(GetTickCount64(),
                                      std::memory_order_release);
}
HMONITOR ResolveFlyoutMonitorTai(HWND flyoutWindow) {
  constexpr DWORD kInvocationMessageTtlMs = 2500;
  const DWORD messageTime = static_cast<DWORD>(GetMessageTime());
  if (messageTime &&
      GetTickCount() - messageTime <= kInvocationMessageTtlMs) {
    if (HMONITOR monitor =
            GetTaskbarMonitorFromPointTai(GetCurrentMessagePointTai())) {
      return monitor;
    }
  }
  constexpr ULONGLONG kInvocationMonitorTtlMs = 2500;
  const ULONGLONG invocationTime =
      g_recentTaskbarInvocationTime.load(std::memory_order_acquire);
  const ULONGLONG now = GetTickCount64();
  if (invocationTime && now >= invocationTime &&
      now - invocationTime <= kInvocationMonitorTtlMs) {
    HMONITOR monitor = reinterpret_cast<HMONITOR>(
        g_recentTaskbarInvocationMonitor.load(std::memory_order_acquire));
    MONITORINFO monitorInfo{.cbSize = sizeof(MONITORINFO)};
    if (monitor && GetMonitorInfoW(monitor, &monitorInfo)) {
      return monitor;
    }
  }
  return flyoutWindow
             ? MonitorFromWindow(flyoutWindow, MONITOR_DEFAULTTONEAREST)
             : nullptr;
}
std::shared_ptr<TaskbarState> GetOrCreateTaskbarState(const std::wstring& monitorName) {
  std::lock_guard<std::mutex> lock(g_taskbarStatesMutex);
  auto& state = g_taskbarStates[monitorName];
  if (!state) {
    state = std::make_shared<TaskbarState>();
  }
  return state;
}
std::vector<std::shared_ptr<TaskbarState>> GetTaskbarStatesSnapshot() {
  std::lock_guard<std::mutex> lock(g_taskbarStatesMutex);
  std::vector<std::shared_ptr<TaskbarState>> states;
  states.reserve(g_taskbarStates.size());
  for (const auto& [monitorName, state] : g_taskbarStates) {
    if (state) {
      states.push_back(state);
    }
  }
  return states;
}
bool TryGetTaskbarFlyoutStateSnapshot(
    const std::wstring& monitorName,
    TaskbarFlyoutStateSnapshot* snapshot) {
  if (!snapshot) {
    return false;
  }
  std::shared_ptr<TaskbarState> state;
  {
    std::lock_guard<std::mutex> lock(g_taskbarStatesMutex);
    auto it = g_taskbarStates.find(monitorName);
    if (it == g_taskbarStates.end() || !it->second) {
      return false;
    }
    state = it->second;
  }
  std::lock_guard<std::recursive_mutex> lock(state->mutex);
  snapshot->lastStartButtonXCalculated = state->lastStartButtonXCalculated;
  snapshot->lastRootWidth = state->lastRootWidth;
  snapshot->lastTargetWidth = state->lastTargetWidth;
  snapshot->lastLeftMostEdgeTray = state->lastLeftMostEdgeTray;
  snapshot->lastRightMostEdgeTray = state->lastRightMostEdgeTray;
  return true;
}
void ClearTaskbarStates() {
  std::lock_guard<std::mutex> lock(g_taskbarStatesMutex);
  g_taskbarStates.clear();
}
void RequestTaskbarDimensionInvalidation() {
  g_dimensionInvalidationGeneration.fetch_add(1, std::memory_order_acq_rel);
}
void RequestTaskbarChildStyleRefresh() {
  g_taskbarChildStyleGeneration.fetch_add(1, std::memory_order_acq_rel);
}
void ApplySettingsDebounced(int delayMs);
void ApplySettingsDebounced();
void ApplySettingsFromTaskbarThreadIfRequired();
void ApplySettingsFromTaskbarThreadImmediately();
void ApplySettingsFromTaskbarThreadGeometryChanged();
extern std::atomic<int> g_high_priority_dispatch_passes;
void RequestTaskbarButtonSizeRelayout();
void ArmInitialExplorerStyleApplyDelay();
void ScheduleInitialExplorerStyleApply();
int g_lastRecordedStartMenuWidth=0;
std::atomic<bool> g_already_requested_debounce_initializing = false;
STDAPI GetDpiForMonitor(HMONITOR hmonitor, MONITOR_DPI_TYPE dpiType, UINT* dpiX, UINT* dpiY);
#include <Windows.h>
bool IsStartMenuOrbLeftAligned() {
    DWORD value = 0;
    DWORD size = sizeof(value);
    HKEY hKey;
    if (RegOpenKeyExW(HKEY_CURRENT_USER,
                     LR"(Software\Microsoft\Windows\CurrentVersion\Explorer\Advanced)",
                      0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        if (RegQueryValueExW(hKey, L"TaskbarAl", nullptr, nullptr,
                             reinterpret_cast<LPBYTE>(&value), &size) == ERROR_SUCCESS) {
            RegCloseKey(hKey);
            return value == 0;
        }
        RegCloseKey(hKey);
    }
    return false;
}
int GetFlyoutTaskbarBottomGapPx(float dpiScaleY) {
    std::lock_guard<std::recursive_mutex> lock(g_settingsMutex);
    if (g_unloading || g_settings.userDefinedFlatTaskbarBottomCorners ||
        g_settings.userDefinedFullWidthTaskbarBackground) {
        return 0;
    }
    int offsetY = static_cast<int>(g_settings.userDefinedTaskbarOffsetY);
    if (offsetY >= 0) {
        return 0;
    }
    return static_cast<int>((-offsetY * dpiScaleY) + 0.5f);
}
int GetFlyoutTaskbarHeightPx(float dpiScaleY) {
    std::lock_guard<std::recursive_mutex> lock(g_settingsMutex);
    int taskbarHeight = static_cast<int>(g_settings.userDefinedTaskbarHeight);
    if (taskbarHeight <= 0) {
        taskbarHeight = g_taskbarHeight > 0 ? g_taskbarHeight : kSystemMediumTaskbarButtonSize;
    }
    return std::max(1, static_cast<int>((taskbarHeight * dpiScaleY) + 0.5f));
}
int GetFlyoutInnerPaddingPx(float dpiScale) {
    std::lock_guard<std::recursive_mutex> lock(g_settingsMutex);
    if (dpiScale <= 0.0f) {
        dpiScale = 1.0f;
    }
    constexpr int kMaxFlyoutInnerPaddingDip = 32;
    const float logicalPadding =
        std::min<float>(kMaxFlyoutInnerPaddingDip,
                        static_cast<float>(g_settings.userDefinedTaskbarBackgroundHorizontalPadding) +
                            (g_settings.userDefinedTaskbarCornerRadius * 0.5f));
    return std::max(0, static_cast<int>((logicalPadding * dpiScale) + 0.5f));
}
bool IsVerticalTaskbar();
bool TryCalculateFlyoutYAboveTaskbar(const MONITORINFO& monitorInfo,
                                     int flyoutHeight,
                                     float dpiScaleY,
                                     int& y) {
    if (flyoutHeight <= 0 || dpiScaleY <= 0.0f || IsVerticalTaskbar()) {
        return false;
    }
    const int monitorTop = monitorInfo.rcMonitor.top;
    const int monitorBottom = monitorInfo.rcMonitor.bottom;
    if (monitorBottom <= monitorTop) {
        return false;
    }
    const int taskbarHeightPx = GetFlyoutTaskbarHeightPx(dpiScaleY);
    const int taskbarBottomGapPx = GetFlyoutTaskbarBottomGapPx(dpiScaleY);
    const int taskbarTop = monitorBottom - taskbarHeightPx - taskbarBottomGapPx;
    if (taskbarTop <= monitorTop) {
        return false;
    }
    y = taskbarTop - flyoutHeight;
    if (y < monitorTop) {
        y = monitorTop;
    }
    return true;
}
std::wstring GetMonitorName(HMONITOR monitor) {
    MONITORINFOEX monitorInfo = {};
    monitorInfo.cbSize = sizeof(MONITORINFOEX);
    if (monitor && GetMonitorInfo(monitor, &monitorInfo)) {
        return std::wstring(monitorInfo.szDevice);
    }
    return L"default";
}
std::wstring GetMonitorName(HWND hwnd) {
    HMONITOR monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    return GetMonitorName(monitor);
}
STDAPI GetDpiForMonitor
(HMONITOR hmonitor,
                        MONITOR_DPI_TYPE dpiType,
                        UINT* dpiX,
                        UINT* dpiY);
size_t OffsetFromAssemblyRegex(void* func,
                               size_t defValue,
                               std::regex regex,
                               int limit = 30) {
    BYTE* p = (BYTE*)func;
    for (int i = 0; i < limit; i++) {
        WH_DISASM_RESULT result;
        if (!Wh_Disasm(p, &result)) {
            break;
        }
        p += result.length;
        std::string_view s = result.text;
        if (s == "ret") {
            break;
        }
        std::match_results<std::string_view::const_iterator> match;
        if (std::regex_match(s.begin(), s.end(), match, regex)) {
            // Wh_Log(L"%S", result.text);
            return std::stoull(match[1], nullptr, 16);
        }
    }
    Wh_Log(L"Failed for %p", func);
    return defValue;
}
std::optional<bool> IsOsFeatureEnabled(UINT32 featureId) {
    enum FEATURE_ENABLED_STATE {
        FEATURE_ENABLED_STATE_DEFAULT = 0,
        FEATURE_ENABLED_STATE_DISABLED = 1,
        FEATURE_ENABLED_STATE_ENABLED = 2,
    };
#pragma pack(push, 1)
    struct RTL_FEATURE_CONFIGURATION {
        unsigned int featureId;
        unsigned __int32 group : 4;
        FEATURE_ENABLED_STATE enabledState : 2;
        unsigned __int32 enabledStateOptions : 1;
        unsigned __int32 unused1 : 1;
        unsigned __int32 variant : 6;
        unsigned __int32 variantPayloadKind : 2;
        unsigned __int32 unused2 : 16;
        unsigned int payload;
    };
#pragma pack(pop)
    using RtlQueryFeatureConfiguration_t =
        int(NTAPI*)(UINT32, int, INT64*, RTL_FEATURE_CONFIGURATION*);
    static RtlQueryFeatureConfiguration_t pRtlQueryFeatureConfiguration = []() {
        HMODULE hNtDll = GetModuleHandle(L"ntdll.dll");
        return hNtDll ? (RtlQueryFeatureConfiguration_t)GetProcAddress(
                            hNtDll, "RtlQueryFeatureConfiguration")
                      : nullptr;
    }();
    if (!pRtlQueryFeatureConfiguration) {
        Wh_Log(L"RtlQueryFeatureConfiguration not found");
        return std::nullopt;
    }
    RTL_FEATURE_CONFIGURATION feature = {0};
    INT64 changeStamp = 0;
    HRESULT hr =
        pRtlQueryFeatureConfiguration(featureId, 1, &changeStamp, &feature);
    if (SUCCEEDED(hr)) {
        Wh_Log(L"RtlQueryFeatureConfiguration result for %u: %d", featureId,
               feature.enabledState);
        switch (feature.enabledState) {
            case FEATURE_ENABLED_STATE_DISABLED:
                return false;
            case FEATURE_ENABLED_STATE_ENABLED:
                return true;
            case FEATURE_ENABLED_STATE_DEFAULT:
                return std::nullopt;
        }
    } else {
        Wh_Log(L"RtlQueryFeatureConfiguration error for %u: %08X", featureId,
               hr);
    }
    return std::nullopt;
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
bool IsVerticalTaskbar() {
    APPBARDATA appBarData = {
        .cbSize = sizeof(APPBARDATA),
    };
    if (!SHAppBarMessage(ABM_GETTASKBARPOS, &appBarData)) {
        Wh_Log(L"SHAppBarMessage(ABM_GETTASKBARPOS) failed");
        return false;
    }
    return appBarData.uEdge == ABE_LEFT || appBarData.uEdge == ABE_RIGHT;
}
void OverrideResourceDirectoryLookup(
    PCSTR sourceFunctionName,
    const winrt::Windows::Foundation::IInspectable* key,
    winrt::Windows::Foundation::IInspectable* value) {
    if (g_unloading || !key || !value) {
        return;
    }
    const auto keyString = key->try_as<winrt::hstring>();
    if (!keyString) {
        return;
    }
    double newValueDouble;
    if (*keyString == L"MediumTaskbarButtonExtent") {
        newValueDouble = g_settings_tbiconsize.taskbarButtonWidth;
    } else if (*keyString == L"SmallTaskbarButtonExtent") {
        newValueDouble = g_settings_tbiconsize.taskbarButtonWidthSmall;
    } else {
        return;
    }
    const auto valueDouble = value->try_as<double>();
    if (!valueDouble) {
        return;
    }
    if (newValueDouble != *valueDouble) {
        Wh_Log(L"[%S] Overriding value %s: %f->%f", sourceFunctionName,
               keyString->c_str(), *valueDouble, newValueDouble);
        *value = winrt::box_value(newValueDouble);
    }
}
using ResourceDictionary_Lookup_TaskbarView_t =
    winrt::Windows::Foundation::IInspectable*(
        WINAPI*)(void* pThis,
                 void** result,
                 winrt::Windows::Foundation::IInspectable* key);
ResourceDictionary_Lookup_TaskbarView_t
    ResourceDictionary_Lookup_TaskbarView_Original;
winrt::Windows::Foundation::IInspectable* WINAPI
ResourceDictionary_Lookup_TaskbarView_Hook(
    void* pThis,
    void** result,
    winrt::Windows::Foundation::IInspectable* key) {
    //
    auto ret = ResourceDictionary_Lookup_TaskbarView_Original
        ? ResourceDictionary_Lookup_TaskbarView_Original(pThis, result, key)
        : nullptr;
    if (!ret || !*ret) {
        return ret;
    }
    OverrideResourceDirectoryLookup(__FUNCTION__, key, ret);
    return ret;
}
using ResourceDictionary_Lookup_SearchUxUi_t =
    winrt::Windows::Foundation::IInspectable*(
        WINAPI*)(void* pThis,
                 void** result,
                 winrt::Windows::Foundation::IInspectable* key);
ResourceDictionary_Lookup_SearchUxUi_t
    ResourceDictionary_Lookup_SearchUxUi_Original;
winrt::Windows::Foundation::IInspectable* WINAPI
ResourceDictionary_Lookup_SearchUxUi_Hook(
    void* pThis,
    void** result,
    winrt::Windows::Foundation::IInspectable* key) {
    //
    auto ret = ResourceDictionary_Lookup_SearchUxUi_Original
        ? ResourceDictionary_Lookup_SearchUxUi_Original(pThis, result, key)
        : nullptr;
    if (!ret || !*ret) {
        return ret;
    }
    OverrideResourceDirectoryLookup(__FUNCTION__, key, ret);
    return ret;
}
using IconUtils_GetIconSize_t = void(WINAPI*)(bool isSmall,
                                              int type,
                                              SIZE* size);
IconUtils_GetIconSize_t IconUtils_GetIconSize_Original;
void WINAPI IconUtils_GetIconSize_Hook(bool isSmall, int type, SIZE* size) {
    [[maybe_unused]] static bool logged = [] {
        Wh_Log(L"> [%S] First call, hasDynamicIconScaling=%d",
               __PRETTY_FUNCTION__, g_hasDynamicIconScaling);
        return true;
    }();
    if (g_hasDynamicIconScaling) {
        IconUtils_GetIconSize_Original(isSmall, type, size);
        return;
    }
    IconUtils_GetIconSize_Original(isSmall, type, size);
    if (!g_unloading && !isSmall) {
        size->cx = MulDiv(size->cx, g_settings_tbiconsize.iconSize, 24);
        size->cy = MulDiv(size->cy, g_settings_tbiconsize.iconSize, 24);
    }
}
using IconContainer_IsStorageRecreationRequired_t = bool(WINAPI*)(void* pThis,
                                                                  void* param1,
                                                                  int flags);
IconContainer_IsStorageRecreationRequired_t
    IconContainer_IsStorageRecreationRequired_Original;
bool WINAPI IconContainer_IsStorageRecreationRequired_Hook(void* pThis,
                                                           void* param1,
                                                           int flags) {
    [[maybe_unused]] static bool logged = [] {
        Wh_Log(L"> [%S] First call, hasDynamicIconScaling=%d",
               __PRETTY_FUNCTION__, g_hasDynamicIconScaling);
        return true;
    }();
    if (g_hasDynamicIconScaling) {
        return IconContainer_IsStorageRecreationRequired_Original(pThis, param1,
                                                                  flags);
    }
    if (g_applyingSettings) {
        return true;
    }
    return IconContainer_IsStorageRecreationRequired_Original(pThis, param1,
                                                              flags);
}
using TrayUI_GetMinSize_t = void(WINAPI*)(void* pThis,
                                          HMONITOR monitor,
                                          SIZE* size);
TrayUI_GetMinSize_t TrayUI_GetMinSize_Original;
void WINAPI TrayUI_GetMinSize_Hook(void* pThis, HMONITOR monitor, SIZE* size) {
    TrayUI_GetMinSize_Original(pThis, monitor, size);
    // Reassign min height to fix displaced secondary taskbar when auto-hide is
    // enabled.
    if (!IsVerticalTaskbar() && g_taskbarHeight) {
        UINT dpiX = 0;
        UINT dpiY = 0;
        GetDpiForMonitor(monitor, MDT_DEFAULT, &dpiX, &dpiY);
        size->cy = MulDiv(g_taskbarHeight, dpiY, 96);
    }
}
using CIconLoadingFunctions_GetClassLongPtrW_t = ULONG_PTR(WINAPI*)(void* pThis,
                                                                    HWND hWnd,
                                                                    int nIndex);
CIconLoadingFunctions_GetClassLongPtrW_t
    CIconLoadingFunctions_GetClassLongPtrW_Original;
ULONG_PTR WINAPI CIconLoadingFunctions_GetClassLongPtrW_Hook(void* pThis,
                                                             HWND hWnd,
                                                             int nIndex) {
    Wh_Log(L"> hasDynamicIconScaling=%d, nIndex=%d", g_hasDynamicIconScaling,
           nIndex);
    if (g_hasDynamicIconScaling) {
        return CIconLoadingFunctions_GetClassLongPtrW_Original(pThis, hWnd,
                                                               nIndex);
    }
    if (!g_unloading && nIndex == GCLP_HICON && g_settings_tbiconsize.iconSize <= 16) {
        nIndex = GCLP_HICONSM;
    }
    ULONG_PTR ret =
        CIconLoadingFunctions_GetClassLongPtrW_Original(pThis, hWnd, nIndex);
    return ret;
}
using CIconLoadingFunctions_SendMessageCallbackW_t =
    BOOL(WINAPI*)(void* pThis,
                  HWND hWnd,
                  UINT Msg,
                  WPARAM wParam,
                  LPARAM lParam,
                  SENDASYNCPROC lpResultCallBack,
                  ULONG_PTR dwData);
CIconLoadingFunctions_SendMessageCallbackW_t
    CIconLoadingFunctions_SendMessageCallbackW_Original;
BOOL WINAPI
CIconLoadingFunctions_SendMessageCallbackW_Hook(void* pThis,
                                                HWND hWnd,
                                                UINT Msg,
                                                WPARAM wParam,
                                                LPARAM lParam,
                                                SENDASYNCPROC lpResultCallBack,
                                                ULONG_PTR dwData) {
    Wh_Log(L"> hasDynamicIconScaling=%d, Msg=%u, wParam=%zu, lParam=%zu",
           g_hasDynamicIconScaling, Msg, wParam, lParam);
    if (g_hasDynamicIconScaling) {
        return CIconLoadingFunctions_SendMessageCallbackW_Original(
            pThis, hWnd, Msg, wParam, lParam, lpResultCallBack, dwData);
    }
    if (!g_unloading && Msg == WM_GETICON && wParam == ICON_BIG &&
        g_settings_tbiconsize.iconSize <= 16) {
        wParam = ICON_SMALL2;
    }
    BOOL ret = CIconLoadingFunctions_SendMessageCallbackW_Original(
        pThis, hWnd, Msg, wParam, lParam, lpResultCallBack, dwData);
    return ret;
}
using ShellIconLoaderV2_LoadAsyncIcon__ResumeCoro_t =
    void(WINAPI*)(void* pThis);
ShellIconLoaderV2_LoadAsyncIcon__ResumeCoro_t
    ShellIconLoaderV2_LoadAsyncIcon__ResumeCoro_Original;
void WINAPI ShellIconLoaderV2_LoadAsyncIcon__ResumeCoro_Hook(void* pThis) {
    Wh_Log(L"> hasDynamicIconScaling=%d", g_hasDynamicIconScaling);
    if (g_hasDynamicIconScaling) {
        ShellIconLoaderV2_LoadAsyncIcon__ResumeCoro_Original(pThis);
        return;
    }
    g_shellIconLoaderV2_LoadAsyncIcon__ResumeCoro_ThreadId =
        GetCurrentThreadId();
    ShellIconLoaderV2_LoadAsyncIcon__ResumeCoro_Original(pThis);
    g_shellIconLoaderV2_LoadAsyncIcon__ResumeCoro_ThreadId = 0;
}
using TrayUI__StuckTrayChange_t = void(WINAPI*)(void* pThis);
TrayUI__StuckTrayChange_t TrayUI__StuckTrayChange_Original;
using TrayUI__HandleSettingChange_t = void(WINAPI*)(void* pThis,
                                                    void* param1,
                                                    void* param2,
                                                    void* param3,
                                                    void* param4);
TrayUI__HandleSettingChange_t TrayUI__HandleSettingChange_Original;
void WINAPI TrayUI__HandleSettingChange_Hook(void* pThis,
                                             void* param1,
                                             void* param2,
                                             void* param3,
                                             void* param4) {
    TrayUI__HandleSettingChange_Original(pThis, param1, param2, param3, param4);
    if (g_applyingSettings) {
        TrayUI__StuckTrayChange_Original(pThis);
    }
}
using TaskListItemViewModel_GetIconHeight_t = int(WINAPI*)(void* pThis,
                                                           void* param1,
                                                           double* iconHeight);
TaskListItemViewModel_GetIconHeight_t
    TaskListItemViewModel_GetIconHeight_Original;
int WINAPI TaskListItemViewModel_GetIconHeight_Hook(void* pThis,
                                                    void* param1,
                                                    double* iconHeight) {
    [[maybe_unused]] static bool logged = [] {
        Wh_Log(L"> [%S] First call, hasDynamicIconScaling=%d",
               __PRETTY_FUNCTION__, g_hasDynamicIconScaling);
        return true;
    }();
    if (g_hasDynamicIconScaling) {
        return TaskListItemViewModel_GetIconHeight_Original(pThis, param1,
                                                            iconHeight);
    }
    int ret =
        TaskListItemViewModel_GetIconHeight_Original(pThis, param1, iconHeight);
    if (!g_unloading) {
        *iconHeight = g_settings_tbiconsize.iconSize;
    }
    return ret;
}
using TaskListGroupViewModel_GetIconHeight_t = int(WINAPI*)(void* pThis,
                                                            void* param1,
                                                            double* iconHeight);
TaskListGroupViewModel_GetIconHeight_t
    TaskListGroupViewModel_GetIconHeight_Original;
int WINAPI TaskListGroupViewModel_GetIconHeight_Hook(void* pThis,
                                                     void* param1,
                                                     double* iconHeight) {
    Wh_Log(L"> hasDynamicIconScaling=%d", g_hasDynamicIconScaling);
    if (g_hasDynamicIconScaling) {
        return TaskListGroupViewModel_GetIconHeight_Original(pThis, param1,
                                                             iconHeight);
    }
    int ret = TaskListGroupViewModel_GetIconHeight_Original(pThis, param1,
                                                            iconHeight);
    if (!g_unloading) {
        *iconHeight = g_settings_tbiconsize.iconSize;
    }
    return ret;
}
using TaskbarConfiguration_GetIconHeightInViewPixels_taskbarSizeEnum_t =
    double(WINAPI*)(int enumTaskbarSize);
TaskbarConfiguration_GetIconHeightInViewPixels_taskbarSizeEnum_t
    TaskbarConfiguration_GetIconHeightInViewPixels_taskbarSizeEnum_Original;
double WINAPI
TaskbarConfiguration_GetIconHeightInViewPixels_taskbarSizeEnum_Hook(
    int enumTaskbarSize) {
    Wh_Log(L"> hasDynamicIconScaling=%d, enumTaskbarSize=%d",
           g_hasDynamicIconScaling, enumTaskbarSize);
    // Even if the feature flag is enabled, the feature may not be actually
    // enabled for some reason. Handle this here by resetting the flag.
    if (g_hasDynamicIconScaling) {
        Wh_Log(L"Setting hasDynamicIconScaling to false");
        g_hasDynamicIconScaling = false;
    }
    if (!g_unloading && (enumTaskbarSize == 1 || enumTaskbarSize == 2)) {
        return g_settings_tbiconsize.iconSize ;
    }
    return TaskbarConfiguration_GetIconHeightInViewPixels_taskbarSizeEnum_Original(
        enumTaskbarSize);
}
using TaskbarConfiguration_GetIconHeightInViewPixels_double_t =
    double(WINAPI*)(double baseHeight);
TaskbarConfiguration_GetIconHeightInViewPixels_double_t
    TaskbarConfiguration_GetIconHeightInViewPixels_double_Original;
double WINAPI
TaskbarConfiguration_GetIconHeightInViewPixels_double_Hook(double baseHeight) {
    Wh_Log(L"> hasDynamicIconScaling=%d, baseHeight=%f",
           g_hasDynamicIconScaling, baseHeight);
    // Even if the feature flag is enabled, the feature may not be actually
    // enabled for some reason. Handle this here by resetting the flag.
    if (g_hasDynamicIconScaling) {
        Wh_Log(L"Setting hasDynamicIconScaling to false");
        g_hasDynamicIconScaling = false;
    }
    if (!g_unloading) {
        return g_settings_tbiconsize.iconSize;
    }
    return TaskbarConfiguration_GetIconHeightInViewPixels_double_Original(
        baseHeight);
}
using TaskbarConfiguration_GetIconHeightInViewPixels_method_t =
    double(WINAPI*)(void* pThis);
TaskbarConfiguration_GetIconHeightInViewPixels_method_t
    TaskbarConfiguration_GetIconHeightInViewPixels_method_Original;
double WINAPI
TaskbarConfiguration_GetIconHeightInViewPixels_method_Hook(void* pThis) {
    [[maybe_unused]] static bool logged = [] {
        Wh_Log(L"> [%S] First call, hasDynamicIconScaling=%d",
               __PRETTY_FUNCTION__, g_hasDynamicIconScaling);
        return true;
    }();
    double iconSize =
        TaskbarConfiguration_GetIconHeightInViewPixels_method_Original(pThis);
    if (!g_unloading) {
        return iconSize <= 16 ? g_settings_tbiconsize.iconSizeSmall : g_settings_tbiconsize.iconSize;
    }
    return iconSize;
}
using TaskListButton_IconHeight_t = void(WINAPI*)(void* pThis, double height);
TaskListButton_IconHeight_t TaskListButton_IconHeight_Original;
size_t GetIconHeightOffset() {
    static size_t iconHeightOffset = []() -> size_t {
        if (!TaskListButton_IconHeight_Original) {
            Wh_Log(L"Error: TaskListButton_IconHeight_Original is null");
            return 0;
        }
        size_t offset =
#if defined(_M_X64)
            OffsetFromAssemblyRegex(
                (void*)TaskListButton_IconHeight_Original, 0,
                std::regex(R"(movsd xmm\d+, qword ptr \[rcx\+0x([0-9a-f]+)\])",
                           std::regex_constants::icase),
                30);
#elif defined(_M_ARM64)
            OffsetFromAssemblyRegex(
                (void*)TaskListButton_IconHeight_Original, 0,
                std::regex(R"(ldr\s+d\d+, \[x\d+, #0x([0-9a-f]+)\])",
                           std::regex_constants::icase),
                30);
#else
#error "Unsupported architecture"
#endif
        Wh_Log(L"iconHeightOffset=0x%X", offset);
        return offset > 0xFFFF ? 0 : offset;
    }();
    return iconHeightOffset;
}
void TaskListButton_IconHeight_InitOffsets() {
    GetIconHeightOffset();
}
using SystemTrayController_GetFrameSize_t =
    double(WINAPI*)(void* pThis, int enumTaskbarSize);
SystemTrayController_GetFrameSize_t SystemTrayController_GetFrameSize_Original;
double WINAPI SystemTrayController_GetFrameSize_Hook(void* pThis,
                                                     int enumTaskbarSize) {
    Wh_Log(L"> %d", enumTaskbarSize);
    if (!IsVerticalTaskbar() && g_taskbarHeight &&
        (enumTaskbarSize == 1 || enumTaskbarSize == 2)) {
        return g_taskbarHeight;
    }
    return SystemTrayController_GetFrameSize_Original(pThis, enumTaskbarSize);
}
using SystemTraySecondaryController_GetFrameSize_t =
    double(WINAPI*)(void* pThis, int enumTaskbarSize);
SystemTraySecondaryController_GetFrameSize_t
    SystemTraySecondaryController_GetFrameSize_Original;
double WINAPI
SystemTraySecondaryController_GetFrameSize_Hook(void* pThis,
                                                int enumTaskbarSize) {
    Wh_Log(L"> %d", enumTaskbarSize);
    if (!IsVerticalTaskbar() && g_taskbarHeight &&
        (enumTaskbarSize == 1 || enumTaskbarSize == 2)) {
        return g_taskbarHeight;
    }
    return SystemTraySecondaryController_GetFrameSize_Original(pThis,
                                                               enumTaskbarSize);
}
using TaskbarConfiguration_GetFrameSize_t =
    double(WINAPI*)(int enumTaskbarSize);
TaskbarConfiguration_GetFrameSize_t TaskbarConfiguration_GetFrameSize_Original;
double WINAPI TaskbarConfiguration_GetFrameSize_Hook(int enumTaskbarSize) {
    Wh_Log(L"> %d", enumTaskbarSize);
    if (!g_originalTaskbarHeight &&
        (enumTaskbarSize == 1 || enumTaskbarSize == 2)) {
        g_originalTaskbarHeight =
            TaskbarConfiguration_GetFrameSize_Original(enumTaskbarSize);
    }
    if (!IsVerticalTaskbar() && g_taskbarHeight &&
        (enumTaskbarSize == 1 || enumTaskbarSize == 2)) {
        return g_taskbarHeight;
    }
    return TaskbarConfiguration_GetFrameSize_Original(enumTaskbarSize);
}
#ifdef _M_ARM64
thread_local double* g_TaskbarConfiguration_UpdateFrameSize_frameSize;
using TaskbarConfiguration_UpdateFrameSize_t = void(WINAPI*)(void* pThis);
TaskbarConfiguration_UpdateFrameSize_t
    TaskbarConfiguration_UpdateFrameSize_SymbolAddress;
LONG GetFrameSizeOffset() {
    static LONG frameSizeOffset = []() -> LONG {
        if (!TaskbarConfiguration_UpdateFrameSize_SymbolAddress) {
            Wh_Log(
                L"Error: TaskbarConfiguration_UpdateFrameSize_SymbolAddress is "
                L"null");
            return 0;
        }
        // Find the offset to the frame size.
        // str d16, [x19, #0x50]
        const DWORD* start =
            (const DWORD*)TaskbarConfiguration_UpdateFrameSize_SymbolAddress;
        const DWORD* end = start + 0x80;
        std::regex regex1(R"(str\s+d\d+, \[x\d+, #0x([0-9a-f]+)\])");
        for (const DWORD* p = start; p != end; p++) {
            WH_DISASM_RESULT result1;
            if (!Wh_Disasm((void*)p, &result1)) {
                break;
            }
            std::string_view s1 = result1.text;
            if (s1 == "ret") {
                break;
            }
            std::match_results<std::string_view::const_iterator> match1;
            if (!std::regex_match(s1.begin(), s1.end(), match1, regex1)) {
                continue;
            }
            // Wh_Log(L"%S", result1.text);
            LONG offset = std::stoull(match1[1], nullptr, 16);
            Wh_Log(L"frameSizeOffset=0x%X", offset);
            return (offset < 0 || offset > 0xFFFF) ? 0 : offset;
        }
        Wh_Log(L"frameSizeOffset not found");
        return 0;
    }();
    return frameSizeOffset;
}
void TaskbarConfiguration_UpdateFrameSize_InitOffsets() {
    GetFrameSizeOffset();
}
TaskbarConfiguration_UpdateFrameSize_t
    TaskbarConfiguration_UpdateFrameSize_Original;
void WINAPI TaskbarConfiguration_UpdateFrameSize_Hook(void* pThis) {
    LONG frameSizeOffset = GetFrameSizeOffset();
    if (!frameSizeOffset) {
        Wh_Log(L"Error: frameSizeOffset is invalid");
        TaskbarConfiguration_UpdateFrameSize_Original(pThis);
        return;
    }
    g_TaskbarConfiguration_UpdateFrameSize_frameSize =
        (double*)((BYTE*)pThis + frameSizeOffset);
    TaskbarConfiguration_UpdateFrameSize_Original(pThis);
    g_TaskbarConfiguration_UpdateFrameSize_frameSize = nullptr;
}
using Event_operator_call_t = void(WINAPI*)(void* pThis);
Event_operator_call_t Event_operator_call_Original;
void WINAPI Event_operator_call_Hook(void* pThis) {
    if (g_TaskbarConfiguration_UpdateFrameSize_frameSize) {
        if (!g_originalTaskbarHeight) {
            g_originalTaskbarHeight =
                *g_TaskbarConfiguration_UpdateFrameSize_frameSize;
        }
        if (!IsVerticalTaskbar() && g_taskbarHeight) {
            *g_TaskbarConfiguration_UpdateFrameSize_frameSize = g_taskbarHeight;
        }
    }
    Event_operator_call_Original(pThis);
}
#endif  // _M_ARM64
using SystemTrayController_UpdateFrameSize_t = void(WINAPI*)(void* pThis);
SystemTrayController_UpdateFrameSize_t
    SystemTrayController_UpdateFrameSize_SymbolAddress;
LONG GetLastHeightOffset() {
    static LONG lastHeightOffset = []() -> LONG {
        if (!SystemTrayController_UpdateFrameSize_SymbolAddress) {
            Wh_Log(
                L"Error: SystemTrayController_UpdateFrameSize_SymbolAddress is "
                L"null");
            return 0;
        }
        // Find the last height offset to reset the height value.
#if defined(_M_X64)
        // 66 0f 2e b3 b0 00 00 00 UCOMISD    uVar4,qword ptr [RBX + 0xb0]
        // 7a 4c                   JP         LAB_180075641
        // 75 4a                   JNZ        LAB_180075641
        //
        // Newer insider builds (first seen in 2126.5501.20.6000):
        // 660f2e87b0000000 ucomisd xmm0, mmword ptr [rdi+0B0h]
        // 7a02             jp      18006c931
        // 7410             je      18006c941
        //
        // Newer insider builds (first seen in 2604.8002.400.0):
        // 66 0f 2e b7 b0 00 00 00   UCOMISD    XMM6,qword ptr [RDI + 0xb0]
        // 7a 06                     JP         LAB_1800828e3
        // 0f 84 84 00 00 00         JZ         LAB_180082967
        const BYTE* start =
            (const BYTE*)SystemTrayController_UpdateFrameSize_SymbolAddress;
        const BYTE* end = start + 0x400;
        for (const BYTE* p = start; p != end; p++) {
            if (p[0] == 0x66 && p[1] == 0x0F && p[2] == 0x2E &&
                (p[3] & 0xC0) == 0x80 && p[8] == 0x7A &&
                (p[10] == 0x74 || p[10] == 0x75 ||
                 (p[10] == 0x0F && (p[11] == 0x84 || p[11] == 0x85)))) {
                LONG offset = *(LONG*)(p + 4);
                Wh_Log(L"lastHeightOffset=0x%X", offset);
                return (offset < 0 || offset > 0xFFFF) ? 0 : offset;
            }
        }
#elif defined(_M_ARM64)
        // fd405a70 ldr  d16,[x19,#0xB0]
        // 1e702000 fcmp d0,d16
        // 54000080 beq  [...]::UpdateFrameSize+0x6c
        const DWORD* start =
            (const DWORD*)SystemTrayController_UpdateFrameSize_SymbolAddress;
        const DWORD* end = start + 0x100;
        std::regex regex1(R"(ldr\s+d\d+, \[x\d+, #0x([0-9a-f]+)\])");
        std::regex regex2(R"(fcmp\s+d\d+, d\d+)");
        std::regex regex3(R"(b\.eq\s+0x[0-9a-f]+)");
        for (const DWORD* p = start; p != end; p++) {
            WH_DISASM_RESULT result1;
            if (!Wh_Disasm((void*)p, &result1)) {
                break;
            }
            std::string_view s1 = result1.text;
            if (s1 == "ret") {
                break;
            }
            std::match_results<std::string_view::const_iterator> match1;
            if (!std::regex_match(s1.begin(), s1.end(), match1, regex1)) {
                continue;
            }
            WH_DISASM_RESULT result2;
            if (!Wh_Disasm((void*)(p + 1), &result2)) {
                break;
            }
            std::string_view s2 = result2.text;
            if (!std::regex_match(s2.begin(), s2.end(), regex2)) {
                continue;
            }
            WH_DISASM_RESULT result3;
            if (!Wh_Disasm((void*)(p + 2), &result3)) {
                break;
            }
            std::string_view s3 = result3.text;
            if (!std::regex_match(s3.begin(), s3.end(), regex3)) {
                continue;
            }
            // Wh_Log(L"%S", result1.text);
            // Wh_Log(L"%S", result2.text);
            // Wh_Log(L"%S", result3.text);
            LONG offset = std::stoull(match1[1], nullptr, 16);
            Wh_Log(L"lastHeightOffset=0x%X", offset);
            return (offset < 0 || offset > 0xFFFF) ? 0 : offset;
        }
#else
#error "Unsupported architecture"
#endif
        Wh_Log(L"lastHeightOffset not found");
        return 0;
    }();
    return lastHeightOffset;
}
void SystemTrayController_UpdateFrameSize_InitOffsets() {
    GetLastHeightOffset();
}
SystemTrayController_UpdateFrameSize_t
    SystemTrayController_UpdateFrameSize_Original;
void WINAPI SystemTrayController_UpdateFrameSize_Hook(void* pThis) {
    if (IsVerticalTaskbar()) {
        SystemTrayController_UpdateFrameSize_Original(pThis);
        return;
    }
    LONG lastHeightOffset = GetLastHeightOffset();
    if (lastHeightOffset) {
        *(double*)((BYTE*)pThis + lastHeightOffset) = 0;
    } else {
        Wh_Log(L"Error: lastHeightOffset is invalid");
    }
    g_inSystemTrayController_UpdateFrameSize = true;
    SystemTrayController_UpdateFrameSize_Original(pThis);
    g_inSystemTrayController_UpdateFrameSize = false;
ApplySettingsFromTaskbarThreadGeometryChanged();
}
using TaskbarFrame_MaxHeight_double_t = void(WINAPI*)(void* pThis,
                                                      double value);
TaskbarFrame_MaxHeight_double_t TaskbarFrame_MaxHeight_double_Original;
using TaskbarFrame_Height_double_t = void(WINAPI*)(void* pThis, double value);
TaskbarFrame_Height_double_t TaskbarFrame_Height_double_Original;
void WINAPI TaskbarFrame_Height_double_Hook(void* pThis, double value) {
    if (IsVerticalTaskbar()) {
        TaskbarFrame_Height_double_Original(pThis, value);
        return;
    }
    if (TaskbarFrame_MaxHeight_double_Original) {
        TaskbarFrame_MaxHeight_double_Original(
            pThis, std::numeric_limits<double>::infinity());
    }
    return TaskbarFrame_Height_double_Original(pThis, value);
}
void* TaskbarController_OnGroupingModeChanged_Original;
LONG GetTaskbarFrameOffset() {
    static LONG taskbarFrameOffset = []() -> LONG {
        if (!TaskbarController_OnGroupingModeChanged_Original) {
            Wh_Log(
                L"Error: TaskbarController_OnGroupingModeChanged_Original is "
                L"null");
            return 0;
        }
#if defined(_M_X64)
        // 48:83EC 28               | sub rsp,28
        // 48:8B81 88020000         | mov rax,qword ptr ds:[rcx+288]
        // or
        // 4C:8B81 80020000         | mov r8,qword ptr ds:[rcx+280]
        const BYTE* p =
            (const BYTE*)TaskbarController_OnGroupingModeChanged_Original;
        if (p && p[0] == 0x48 && p[1] == 0x83 && p[2] == 0xEC &&
            (p[4] == 0x48 || p[4] == 0x4C) && p[5] == 0x8B &&
            (p[6] & 0xC0) == 0x80) {
            LONG offset = *(LONG*)(p + 7);
            Wh_Log(L"taskbarFrameOffset=0x%X", offset);
            return (offset < 0 || offset > 0xFFFF) ? 0 : offset;
        }
#elif defined(_M_ARM64)
        // 00000001`806b1810 a9bf7bfd stp fp,lr,[sp,#-0x10]!
        // 00000001`806b1814 910003fd mov fp,sp
        // 00000001`806b1818 aa0003e8 mov x8,x0
        // 00000001`806b181c f9414500 ldr x0,[x8,#0x288]
        const DWORD* start =
            (const DWORD*)TaskbarController_OnGroupingModeChanged_Original;
        const DWORD* end = start + 10;
        std::regex regex1(R"(ldr\s+x\d+, \[x\d+, #0x([0-9a-f]+)\])");
        for (const DWORD* p = start; p != end; p++) {
            WH_DISASM_RESULT result1;
            if (!Wh_Disasm((void*)p, &result1)) {
                break;
            }
            std::string_view s1 = result1.text;
            if (s1 == "ret") {
                break;
            }
            std::match_results<std::string_view::const_iterator> match1;
            if (!std::regex_match(s1.begin(), s1.end(), match1, regex1)) {
                continue;
            }
            // Wh_Log(L"%S", result1.text);
            LONG offset = std::stoull(match1[1], nullptr, 16);
            Wh_Log(L"taskbarFrameOffset=0x%X", offset);
            return (offset < 0 || offset > 0xFFFF) ? 0 : offset;
        }
#else
#error "Unsupported architecture"
#endif
        Wh_Log(L"taskbarFrameOffset not found");
        return 0;
    }();
    return taskbarFrameOffset;
}
void TaskbarController_OnGroupingModeChanged_InitOffsets() {
    GetTaskbarFrameOffset();
}
using TaskbarController_OnGroupingModeChanged_t = void(WINAPI*)(void* pThis);
TaskbarController_OnGroupingModeChanged_t
    TaskbarController_OnGroupingModeChanged_Hook_Original;
void WINAPI TaskbarController_OnGroupingModeChanged_Hook(void* pThis) {
    Wh_Log(L"TaskbarController::OnGroupingModeChanged Hook");
    TaskbarController_OnGroupingModeChanged_Hook_Original(pThis);
    ApplySettingsFromTaskbarThreadGeometryChanged();
}
using TaskbarController_UpdateFrameHeight_t = void(WINAPI*)(void* pThis);
TaskbarController_UpdateFrameHeight_t
    TaskbarController_UpdateFrameHeight_Original;
void WINAPI TaskbarController_UpdateFrameHeight_Hook(void* pThis) {
    if (IsVerticalTaskbar()) {
        TaskbarController_UpdateFrameHeight_Original(pThis);
        return;
    }
    LONG taskbarFrameOffset = GetTaskbarFrameOffset();
    if (!taskbarFrameOffset) {
        Wh_Log(L"Error: taskbarFrameOffset is invalid");
        TaskbarController_UpdateFrameHeight_Original(pThis);
        return;
    }
    void* taskbarFrame = *(void**)((BYTE*)pThis + taskbarFrameOffset);
    if (!taskbarFrame) {
        Wh_Log(L"Error: taskbarFrame is null");
        TaskbarController_UpdateFrameHeight_Original(pThis);
        return;
    }
    FrameworkElement taskbarFrameElement = nullptr;
    ((IUnknown**)taskbarFrame)[1]->QueryInterface(
        winrt::guid_of<FrameworkElement>(),
        winrt::put_abi(taskbarFrameElement));
    if (!taskbarFrameElement) {
        Wh_Log(L"Error: taskbarFrameElement is null");
        TaskbarController_UpdateFrameHeight_Original(pThis);
        return;
    }
    taskbarFrameElement.MaxHeight(std::numeric_limits<double>::infinity());
    TaskbarController_UpdateFrameHeight_Original(pThis);
    // Adjust parent grid height if needed.
    auto contentGrid = Media::VisualTreeHelper::GetParent(taskbarFrameElement)
                           .try_as<FrameworkElement>();
    if (contentGrid) {
        double height = taskbarFrameElement.Height();
        double contentGridHeight = contentGrid.Height();
        if (contentGridHeight > 0 && contentGridHeight != height) {
            Wh_Log(L"Adjusting contentGrid.Height: %f->%f", contentGridHeight,
                   height);
            contentGrid.Height(height);
        }
    }
}
using SystemTraySecondaryController_UpdateFrameSize_t =
    void(WINAPI*)(void* pThis);
SystemTraySecondaryController_UpdateFrameSize_t
    SystemTraySecondaryController_UpdateFrameSize_Original;
void WINAPI SystemTraySecondaryController_UpdateFrameSize_Hook(void* pThis) {
    g_inSystemTrayController_UpdateFrameSize = true;
    SystemTraySecondaryController_UpdateFrameSize_Original(pThis);
    g_inSystemTrayController_UpdateFrameSize = false;ApplySettingsFromTaskbarThreadGeometryChanged();
}
using SystemTrayFrame_Height_t = void(WINAPI*)(void* pThis, double value);
SystemTrayFrame_Height_t SystemTrayFrame_Height_Original;
void WINAPI SystemTrayFrame_Height_Hook(void* pThis, double value) {
    //
    if (!IsVerticalTaskbar() && g_inSystemTrayController_UpdateFrameSize) {
        // Set the system tray height to NaN, otherwise it may not match the
        // custom taskbar height.
        value = std::numeric_limits<double>::quiet_NaN();
    }
    SystemTrayFrame_Height_Original(pThis, value);
}
using TaskbarFrame_MeasureOverride_t =
    int(WINAPI*)(void* pThis,
                 winrt::Windows::Foundation::Size size,
                 winrt::Windows::Foundation::Size* resultSize);
TaskbarFrame_MeasureOverride_t TaskbarFrame_MeasureOverride_Original;
int WINAPI TaskbarFrame_MeasureOverride_Hook(
    void* pThis,
    winrt::Windows::Foundation::Size size,
    winrt::Windows::Foundation::Size* resultSize) {
    g_hookCallCounter++;
    int ret = TaskbarFrame_MeasureOverride_Original(pThis, size, resultSize);
    g_pendingMeasureOverride = false;
    g_hookCallCounter--;
    return ret;
}
using TaskListButton_UpdateButtonPadding_t = void(WINAPI*)(void* pThis);
TaskListButton_UpdateButtonPadding_t
    TaskListButton_UpdateButtonPadding_Original;
void WINAPI TaskListButton_UpdateButtonPadding_Hook(void* pThis) {
    Wh_Log(L"> hasDynamicIconScaling=%d", g_hasDynamicIconScaling);
    if (!g_hasDynamicIconScaling || g_unloading) {
        TaskListButton_UpdateButtonPadding_Original(pThis);
        return;
    }
    // Make sure to use a different value for other calculations such as
    // padding. Value 16 and 32 have special treatment.
    double* iconHeight = nullptr;
    double prevIconHeight;
    if (size_t iconHeightOffset = GetIconHeightOffset()) {
        iconHeight = (double*)((BYTE*)pThis + iconHeightOffset);
        prevIconHeight = *iconHeight;
        double newIconHeight = g_smallIconSize ? 16 : 24;
        Wh_Log(L"Setting iconHeight: %f->%f", prevIconHeight, newIconHeight);
        *iconHeight = newIconHeight;
    }
    TaskListButton_UpdateButtonPadding_Original(pThis);
    if (iconHeight) {
        *iconHeight = prevIconHeight;
    }
}
using TaskListButton_OverlayIcon_t = void(WINAPI*)(void* pThis, void* param1);
TaskListButton_OverlayIcon_t TaskListButton_OverlayIcon_Original;
void WINAPI TaskListButton_OverlayIcon_Hook(void* pThis, void* param1) {
    Wh_Log(L"> hasDynamicIconScaling=%d", g_hasDynamicIconScaling);
    if (!g_hasDynamicIconScaling || g_unloading) {
        TaskListButton_OverlayIcon_Original(pThis, param1);
        return;
    }
    // Value 16 causes badges to be shown as a small dot. There are still some
    // glitches with the badges, e.g. switching from large icons to small icons
    // doesn't update from the badge to the dot, but new badges are shown as
    // dots with small icons. Fixing it might require hooking several additional
    // functions. Maybe one day...
    //
    // This hook handles non-UWP badges (e.g. the Win7 taskbar sample).
    double* iconHeight = nullptr;
    double prevIconHeight;
    if (size_t iconHeightOffset = GetIconHeightOffset()) {
        iconHeight = (double*)((BYTE*)pThis + iconHeightOffset);
        prevIconHeight = *iconHeight;
        double newIconHeight = 24;
        Wh_Log(L"Setting iconHeight: %f->%f", prevIconHeight, newIconHeight);
        *iconHeight = newIconHeight;
    }
    TaskListButton_OverlayIcon_Original(pThis, param1);
    if (iconHeight) {
        *iconHeight = prevIconHeight;
    }
}
using TaskListButton_UpdateBadge_t = void(WINAPI*)(void* pThis);
TaskListButton_UpdateBadge_t TaskListButton_UpdateBadge_Original;
void WINAPI TaskListButton_UpdateBadge_Hook(void* pThis) {
    Wh_Log(L"> hasDynamicIconScaling=%d", g_hasDynamicIconScaling);
    if (!g_hasDynamicIconScaling || g_unloading) {
        TaskListButton_UpdateBadge_Original(pThis);
        return;
    }
    // Value 16 causes badges to be shown as a small dot. There are still some
    // glitches with the badges, e.g. switching from large icons to small icons
    // doesn't update from the badge to the dot, but new badges are shown as
    // dots with small icons. Fixing it might require hooking several additional
    // functions. Maybe one day...
    //
    // This hook handles UWP badges (e.g. Unigram).
    double* iconHeight = nullptr;
    double prevIconHeight;
    if (size_t iconHeightOffset = GetIconHeightOffset()) {
        iconHeight = (double*)((BYTE*)pThis + iconHeightOffset);
        prevIconHeight = *iconHeight;
        double newIconHeight = 24;
        Wh_Log(L"Setting iconHeight: %f->%f", prevIconHeight, newIconHeight);
        *iconHeight = newIconHeight;
    }
    TaskListButton_UpdateBadge_Original(pThis);
    if (iconHeight) {
        *iconHeight = prevIconHeight;
    }
}
void* TaskListButton_UpdateIconColumnDefinition_Original;
LONG GetMediumTaskbarButtonExtentOffset() {
    static LONG mediumTaskbarButtonExtentOffset = []() -> LONG {
#if defined(_M_X64)
        // Search for movsd followed by subsd. In newer builds with vertical
        // taskbar support, there may be an additional movsd without a matching
        // subsd above.
        //
        // f20f10b648030000 movsd   xmm6,mmword ptr [rsi+348h]
        // f20f5cb680030000 subsd   xmm6,mmword ptr [rsi+380h]
        // f20f5cb690030000 subsd   xmm6,mmword ptr [rsi+390h]
        const BYTE* start =
            (const BYTE*)TaskListButton_UpdateIconColumnDefinition_Original;
        const BYTE* end = start + 0x200;
        LONG offsetCandidate = 0;
        LONG offset = 0;
        for (const BYTE* p = start; p != end; p++) {
            if (p[0] == 0xF2 && p[1] == 0x0F && p[2] == 0x10 &&
                (p[3] & 0xC0) == 0x80) {
                offsetCandidate = *(LONG*)(p + 4);
            }
            if (p[0] == 0xF2 && p[1] == 0x44 && p[2] == 0x0F && p[3] == 0x10 &&
                (p[4] & 0xC0) == 0x80) {
                offsetCandidate = *(LONG*)(p + 5);
            }
            if (p[0] == 0xF2 && p[1] == 0x0F && p[2] == 0x5C &&
                (p[3] & 0xC0) == 0x80) {
                offset = offsetCandidate;
                break;
            }
            if (p[0] == 0xF2 && p[1] == 0x44 && p[2] == 0x0F && p[3] == 0x5C &&
                (p[4] & 0xC0) == 0x80) {
                offset = offsetCandidate;
                break;
            }
        }
        Wh_Log(L"mediumTaskbarButtonExtentOffset=0x%X", offset);
        return (offset < 0 || offset > 0xFFFF) ? 0 : offset;
#elif defined(_M_ARM64)
        // ...
        // fd41b670 ldr  d16,[x19,#0x368]
        // fd419e71 ldr  d17,[x19,#0x338]
        // 1e703a31 fsub d17,d17,d16
        // fd41be70 ldr  d16,[x19,#0x378]
        // 1e703a28 fsub d8,d17,d16
        const DWORD* start =
            (const DWORD*)TaskListButton_UpdateIconColumnDefinition_Original;
        const DWORD* end = start + 0x80;
        std::regex regexLdr(R"(ldr\s+(d\d+), \[(x\d+), #0x([0-9a-f]+)\])");
        std::regex regexLdrOther(R"(ldr\s+(d\d+),.*)");
        std::regex regexFsub(R"(fsub\s+d\d+, (d\d+), (d\d+))");
        struct {
            std::string reg;
            std::string regSrc;
            LONG offset;
        } ldrs[32];
        size_t ldrCount = 0;
        for (const DWORD* p = start; p != end; p++) {
            WH_DISASM_RESULT result;
            if (!Wh_Disasm((void*)p, &result)) {
                break;
            }
            std::string_view s = result.text;
            if (s == "ret") {
                break;
            }
            if (ldrCount == ARRAYSIZE(ldrs)) {
                Wh_Log(L"Too many ldr instructions");
                break;
            }
            std::match_results<std::string_view::const_iterator> matchLdr;
            if (std::regex_match(s.begin(), s.end(), matchLdr, regexLdr)) {
                // Wh_Log(L"%S", result.text);
                std::string reg = matchLdr[1];
                std::string regSrc = matchLdr[2];
                LONG offset = std::stoull(matchLdr[3], nullptr, 16);
                ldrs[ldrCount++] = {std::move(reg), std::move(regSrc), offset};
                continue;
            }
            std::match_results<std::string_view::const_iterator> matchLdrOther;
            if (std::regex_match(s.begin(), s.end(), matchLdrOther,
                                 regexLdrOther)) {
                // Wh_Log(L"%S", result.text);
                std::string reg = matchLdrOther[1];
                ldrs[ldrCount++] = {std::move(reg), std::string(), 0};
                continue;
            }
            std::match_results<std::string_view::const_iterator> matchFsub;
            if (std::regex_match(s.begin(), s.end(), matchFsub, regexFsub)) {
                // Wh_Log(L"%S", result.text);
                std::string regA = matchFsub[1];
                std::string regB = matchFsub[2];
                std::remove_reference_t<decltype(ldrs[0])>* ldrA = nullptr;
                std::remove_reference_t<decltype(ldrs[0])>* ldrB = nullptr;
                for (size_t i = 0; i < ldrCount; i++) {
                    const auto& [ldrReg, ldrRegSrc, ldrOffset] =
                        ldrs[ldrCount - 1 - i];
                    if (!ldrA && ldrReg == regA) {
                        ldrA = &ldrs[ldrCount - 1 - i];
                    }
                    if (!ldrB && ldrReg == regB) {
                        ldrB = &ldrs[ldrCount - 1 - i];
                    }
                }
                if (ldrA && ldrB && ldrA->regSrc == ldrB->regSrc) {
                    LONG offset = ldrA->offset;
                    Wh_Log(L"mediumTaskbarButtonExtentOffset=0x%X", offset);
                    return (offset < 0 || offset > 0xFFFF) ? 0 : offset;
                }
            }
        }
#else
#error "Unsupported architecture"
#endif
        Wh_Log(L"Error: mediumTaskbarButtonExtentOffset not found");
        return 0;
    }();
    return mediumTaskbarButtonExtentOffset;
}
void TaskListButton_UpdateIconColumnDefinition_InitOffsets() {
    GetMediumTaskbarButtonExtentOffset();
}
using TaskListButton_UpdateVisualStates_t = void(WINAPI*)(void* pThis);
TaskListButton_UpdateVisualStates_t TaskListButton_UpdateVisualStates_Original;
void WINAPI TaskListButton_UpdateVisualStates_Hook(void* pThis) {
    if (TaskListButton_UpdateIconColumnDefinition_Original &&
        (g_applyingSettings || g_taskbarButtonWidthCustomized)) {
        LONG mediumTaskbarButtonExtentOffset =
            GetMediumTaskbarButtonExtentOffset();
        if (mediumTaskbarButtonExtentOffset) {
            bool updateButtonPadding = false;
            double* mediumTaskbarButtonExtent =
                (double*)((BYTE*)pThis + mediumTaskbarButtonExtentOffset);
            if (*mediumTaskbarButtonExtent >= 1 &&
                *mediumTaskbarButtonExtent < 10000) {
                double newValue =
                    g_unloading ? 44 : g_settings_tbiconsize.taskbarButtonWidth;
                if (newValue != *mediumTaskbarButtonExtent) {
                    Wh_Log(
                        L"Updating MediumTaskbarButtonExtent for "
                        L"TaskListButton: %f->%f",
                        *mediumTaskbarButtonExtent, newValue);
                    *mediumTaskbarButtonExtent = newValue;
                    updateButtonPadding = true;
                }
            }
            double* smallTaskbarButtonExtent =
                g_hasDynamicIconScaling ? mediumTaskbarButtonExtent - 1
                                        : nullptr;
            if (smallTaskbarButtonExtent && *smallTaskbarButtonExtent >= 1 &&
                *smallTaskbarButtonExtent < 10000) {
                double newValue =
                    g_unloading ? 32 : g_settings_tbiconsize.taskbarButtonWidthSmall;
                if (newValue != *smallTaskbarButtonExtent) {
                    Wh_Log(
                        L"Updating SmallTaskbarButtonExtent for "
                        L"TaskListButton: %f->%f",
                        *smallTaskbarButtonExtent, newValue);
                    *smallTaskbarButtonExtent = newValue;
                    updateButtonPadding = true;
                }
            }
            if (updateButtonPadding) {
                g_taskbarButtonWidthCustomized = true;
                TaskListButton_UpdateButtonPadding_Hook(pThis);
            }
        } else {
            Wh_Log(L"Error: mediumTaskbarButtonExtentOffset is invalid");
        }
    }
    TaskListButton_UpdateVisualStates_Original(pThis);
    if (g_applyingSettings && !g_hasDynamicIconScaling) {
        FrameworkElement taskListButtonElement = nullptr;
        ((IUnknown*)pThis + 3)
            ->QueryInterface(winrt::guid_of<FrameworkElement>(),
                             winrt::put_abi(taskListButtonElement));
        if (taskListButtonElement) {
            if (auto iconPanelElement =
                    FindChildByName(taskListButtonElement, L"IconPanel")) {
                if (auto iconElement =
                        FindChildByName(iconPanelElement, L"Icon")) {
                    double iconSize = g_unloading ? 24 : g_settings_tbiconsize.iconSize;
                    iconElement.Width(iconSize);
                    iconElement.Height(iconSize);
                }
            }
        }
    }
}
using LaunchListItemViewModel_IconHeight_t = void(WINAPI*)(void* pThis,
                                                           double iconHeight);
LaunchListItemViewModel_IconHeight_t
    LaunchListItemViewModel_IconHeight_Original;
void WINAPI LaunchListItemViewModel_IconHeight_Hook(void* pThis,
                                                    double iconHeight) {
    Wh_Log(L"> iconHeight=%f", iconHeight);
    g_smallIconSize = iconHeight == g_settings_tbiconsize.iconSizeSmall &&
                      iconHeight != g_settings_tbiconsize.iconSize;
    LaunchListItemViewModel_IconHeight_Original(pThis, iconHeight);
}
using ExperienceToggleButton_UpdateButtonPadding_t = void(WINAPI*)(void* pThis);
ExperienceToggleButton_UpdateButtonPadding_t
    ExperienceToggleButton_UpdateButtonPadding_Original;double GetEffectiveTaskbarButtonTargetWidth();
bool EnsureElementTaskbarButtonWidth(FrameworkElement const& element,
                                     double targetWidth,
                                     bool allowHardWidth);
void WINAPI ExperienceToggleButton_UpdateButtonPadding_Hook(void* pThis) {
ExperienceToggleButton_UpdateButtonPadding_Original(pThis);
if (g_hasDynamicIconScaling && g_unloading) {
    return;
}
FrameworkElement toggleButtonElement = nullptr;
((IUnknown**)pThis)[1]->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                       winrt::put_abi(toggleButtonElement));
if (!toggleButtonElement) {
    return;
}
auto panelElement =
    FindChildByName(toggleButtonElement, L"ExperienceToggleButtonRootPanel")
        .try_as<Controls::Grid>();
if (!panelElement) {
    return;
}
auto className = winrt::get_class_name(toggleButtonElement);
if (className != L"Taskbar.ExperienceToggleButton" &&
    className != L"Taskbar.SearchBoxButton") {
    return;
}
if (className == L"Taskbar.SearchBoxButton" && panelElement.Margin() != Thickness{}) {
    return;
}
const double targetWidth = GetEffectiveTaskbarButtonTargetWidth();
const bool allowHardWidth =
    className != L"Taskbar.SearchBoxButton" ||
    !FindChildByName(panelElement, L"SearchBoxTextBlock");
bool changed = EnsureElementTaskbarButtonWidth(toggleButtonElement, targetWidth, allowHardWidth);
changed = EnsureElementTaskbarButtonWidth(panelElement, targetWidth, allowHardWidth) || changed;
if (changed) {
    Wh_Log(L"Updating taskbar button width for %s to %f", className.c_str(), targetWidth);
    panelElement.UpdateLayout();
}
}
using SearchButtonBase_UpdateButtonPadding_t = void(WINAPI*)(void* pThis);
SearchButtonBase_UpdateButtonPadding_t
    SearchButtonBase_UpdateButtonPadding_Original;
void WINAPI SearchButtonBase_UpdateButtonPadding_Hook(void* pThis) {
SearchButtonBase_UpdateButtonPadding_Original(pThis);
if (g_hasDynamicIconScaling && g_unloading) {
    return;
}
FrameworkElement toggleButtonElement = nullptr;
((IUnknown**)pThis)[1]->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                       winrt::put_abi(toggleButtonElement));
if (!toggleButtonElement) {
    return;
}
auto panelElement =
    FindChildByName(toggleButtonElement, L"SearchBoxButtonRootPanel")
        .try_as<Controls::Grid>();
if (!panelElement) {
    return;
}
if (FindChildByName(panelElement, L"SearchBoxTextBlock")) {
    return;
}
const double targetWidth = GetEffectiveTaskbarButtonTargetWidth();
bool changed = EnsureElementTaskbarButtonWidth(toggleButtonElement, targetWidth, true);
changed = EnsureElementTaskbarButtonWidth(panelElement, targetWidth, true) || changed;
if (changed) {
    Wh_Log(L"Updating search button width to %f", targetWidth);
    panelElement.UpdateLayout();
}
}
using AugmentedEntryPointButton_UpdateButtonPadding_t =
    void(WINAPI*)(void* pThis);
AugmentedEntryPointButton_UpdateButtonPadding_t
    AugmentedEntryPointButton_UpdateButtonPadding_Original;
void WINAPI AugmentedEntryPointButton_UpdateButtonPadding_Hook(void* pThis) {
    g_inAugmentedEntryPointButton_UpdateButtonPadding = true;
    AugmentedEntryPointButton_UpdateButtonPadding_Original(pThis);
    g_inAugmentedEntryPointButton_UpdateButtonPadding = false;
}
using RepeatButton_Width_t = void(WINAPI*)(void* pThis, double width);
RepeatButton_Width_t RepeatButton_Width_Original;
void WINAPI RepeatButton_Width_Hook(void* pThis, double width) {
    Wh_Log(L"> width=%f", width);
    RepeatButton_Width_Original(pThis, width);
    if (!g_inAugmentedEntryPointButton_UpdateButtonPadding) {
        return;
    }
    FrameworkElement button = nullptr;
    (*(IUnknown**)pThis)
        ->QueryInterface(winrt::guid_of<FrameworkElement>(),
                         winrt::put_abi(button));
    if (!button) {
        return;
    }
    FrameworkElement augmentedEntryPointContentGrid =
        FindChildByName(button, L"AugmentedEntryPointContentGrid");
    if (!augmentedEntryPointContentGrid) {
        return;
    }
    double marginValue = static_cast<double>(40 - g_settings_tbiconsize.iconSize) / 2;
    if (marginValue < 0) {
        marginValue = 0;
    }
    EnumChildElements(augmentedEntryPointContentGrid, [marginValue](
                                                          FrameworkElement
                                                              child) {
        if (winrt::get_class_name(child) != L"Windows.UI.Xaml.Controls.Grid") {
            return false;
        }
        FrameworkElement panelGrid =
            FindChildByClassName(child, L"Windows.UI.Xaml.Controls.Grid");
        if (!panelGrid) {
            return false;
        }
        FrameworkElement panel = FindChildByClassName(
            panelGrid, L"AdaptiveCards.Rendering.Uwp.WholeItemsPanel");
        if (!panel) {
            return false;
        }
        Wh_Log(L"Processing %f x %f widget", panelGrid.Width(),
               panelGrid.Height());
        double labelsTopBorderExtraMargin = 0;
        bool widePanel = panelGrid.Width() > panelGrid.Height();
        if (widePanel) {
            auto margin = Thickness{3, 3, 3, 3};
            if (!g_unloading && marginValue <= 3) {
                labelsTopBorderExtraMargin = 3 - marginValue;
                margin.Left = marginValue;
                margin.Top = marginValue;
                // Logically these should be marginValue too, but having no
                // right/bottom margin doesn't seem to matter, while having
                // values which are too tight sometimes cause the icon to
                // disappear for some reason. Relevant issue:
                // https://github.com/ramensoftware/windhawk-mods/issues/726
                margin.Right = 0;
                margin.Bottom = 0;
            }
            Wh_Log(L"Setting Margin=%f,%f,%f,%f for panel", margin.Left,
                   margin.Top, margin.Right, margin.Bottom);
            panel.Margin(margin);
            panelGrid.VerticalAlignment(g_unloading
                                            ? VerticalAlignment::Stretch
                                            : VerticalAlignment::Center);
        } else {
            auto margin = Thickness{8, 8, 8, 8};
            if (!g_unloading) {
                margin.Left = marginValue;
                margin.Top = marginValue;
                // Logically these should be marginValue too, but having no
                // right/bottom margin doesn't seem to matter, while having
                // values which are too tight sometimes cause the icon to
                // disappear for some reason. Relevant issue:
                // https://github.com/ramensoftware/windhawk-mods/issues/726
                margin.Right = 0;
                margin.Bottom = 0;
                if (g_taskbarHeight < 48) {
                    margin.Top -= static_cast<double>(48 - g_taskbarHeight) / 2;
                    if (margin.Top < 0) {
                        margin.Top = 0;
                    }
                }
            }
            Wh_Log(L"Setting Margin=%f,%f,%f,%f for panel", margin.Left,
                   margin.Top, margin.Right, margin.Bottom);
            panel.Margin(margin);
        }
        FrameworkElement tickerGrid = panel;
        if ((tickerGrid = FindChildByClassName(
                 tickerGrid, L"Windows.UI.Xaml.Controls.Border")) &&
            (tickerGrid = FindChildByClassName(
                 tickerGrid, L"AdaptiveCards.Rendering.Uwp.WholeItemsPanel")) &&
            (tickerGrid = FindChildByClassName(
                 tickerGrid, L"Windows.UI.Xaml.Controls.Grid"))) {
            // OK.
        } else {
            return false;
        }
        double badgeMaxValue = g_unloading ? 24 : 40 - marginValue * 2;
        FrameworkElement badgeSmall = tickerGrid;
        if ((badgeSmall = FindChildByName(badgeSmall, L"SmallTicker1")) &&
            (badgeSmall = FindChildByClassName(
                 badgeSmall, L"AdaptiveCards.Rendering.Uwp.WholeItemsPanel")) &&
            (badgeSmall =
                 FindChildByName(badgeSmall, L"BadgeAnchorSmallTicker"))) {
            Wh_Log(L"Setting MaxWidth=%f, MaxHeight=%f for small badge",
                   badgeMaxValue, badgeMaxValue);
            badgeSmall.MaxWidth(badgeMaxValue);
            badgeSmall.MaxHeight(badgeMaxValue);
        }
        FrameworkElement badgeLarge = tickerGrid;
        if ((badgeLarge = FindChildByName(badgeLarge, L"LargeTicker1")) &&
            (badgeLarge = FindChildByClassName(
                 badgeLarge, L"AdaptiveCards.Rendering.Uwp.WholeItemsPanel")) &&
            (badgeLarge =
                 FindChildByName(badgeLarge, L"BadgeAnchorLargeTicker"))) {
            Wh_Log(L"Setting MaxWidth=%f, MaxHeight=%f for large badge",
                   badgeMaxValue, badgeMaxValue);
            badgeLarge.MaxWidth(badgeMaxValue);
            badgeLarge.MaxHeight(badgeMaxValue);
        }
        FrameworkElement labelsBorder = tickerGrid;
        if ((labelsBorder = FindChildByName(labelsBorder, L"LargeTicker2"))) {
            auto margin = Thickness{0, labelsTopBorderExtraMargin, 0, 0};
            Wh_Log(L"Setting Margin=%f,%f,%f,%f for labels border", margin.Left,
                   margin.Top, margin.Right, margin.Bottom);
            labelsBorder.Margin(margin);
        }
        return false;
    });
}
using SHAppBarMessage_t = decltype(&SHAppBarMessage);
SHAppBarMessage_t SHAppBarMessage_Original;
auto WINAPI SHAppBarMessage_Hook(DWORD dwMessage, PAPPBARDATA pData) {
    auto ret = SHAppBarMessage_Original(dwMessage, pData);
    // This is used to position secondary taskbars.
    if (dwMessage == ABM_QUERYPOS && ret && !IsVerticalTaskbar() &&
        g_taskbarHeight) {
        pData->rc.top =
            pData->rc.bottom -
            MulDiv(g_taskbarHeight, GetDpiForWindow(pData->hWnd), 96);
    }
    return ret;
}
static bool TryCorrectShellHookMinRectMessageTai(UINT Msg, WPARAM wParam, LPARAM lParam);using SendMessageTimeoutW_t = decltype(&SendMessageTimeoutW);
SendMessageTimeoutW_t SendMessageTimeoutW_Original;
LRESULT WINAPI SendMessageTimeoutW_Hook(HWND hWnd,
                                        UINT Msg,
                                        WPARAM wParam,
                                        LPARAM lParam,
                                        UINT fuFlags,
                                        UINT uTimeout,
                                        PDWORD_PTR lpdwResult) {
    if (g_shellIconLoaderV2_LoadAsyncIcon__ResumeCoro_ThreadId ==
            GetCurrentThreadId() &&
        !g_unloading && Msg == WM_GETICON && wParam == ICON_BIG &&
        (g_smallIconSize ? g_settings_tbiconsize.iconSizeSmall : g_settings_tbiconsize.iconSize) <=
            16) {
        wParam = ICON_SMALL2;
    }
    LRESULT ret = SendMessageTimeoutW_Original(hWnd, Msg, wParam, lParam,
                                               fuFlags, uTimeout, lpdwResult);
TryCorrectShellHookMinRectMessageTai(Msg, wParam, lParam);
    return ret;
}
void LoadSettingsTBIconSize() {
    const int requestedHeight =
        ReadPositiveIntSettingOrDefault(L"TaskbarHeight", kDefaultTaskbarHeight);
    int taskbarHeight = ClampInt(abs(requestedHeight), kMinTaskbarHeight, kMaxTaskbarHeight);
    const int taskbarOffsetY =
        std::max(0, Wh_GetIntSetting(L"TaskbarOffsetY"));
    const int heightExpansion =
        ((Wh_GetIntSetting(L"FlatTaskbarBottomCorners") ||
          Wh_GetIntSetting(L"FullWidthTaskbarBackground"))
             ? 0
             : (taskbarOffsetY * 2));
    g_settings_tbiconsize.taskbarHeight = ClampInt(
        taskbarHeight + heightExpansion,
        kMinTaskbarHeight,
        kMaxTaskbarHeight + (kDefaultTaskbarOffsetY * 2));
    const int requestedButtonSize =
        ReadPositiveIntSettingOrDefault(L"TaskbarButtonSize", kDefaultTaskbarButtonSize);
    const int taskbarButtonSize =
        ClampInt(abs(requestedButtonSize), kMinTaskbarButtonSize, kMaxTaskbarButtonSize);
    g_settings_tbiconsize.taskbarButtonWidth = taskbarButtonSize;
    g_settings_tbiconsize.taskbarButtonWidthSmall = taskbarButtonSize;
    const int requestedIconSize =
        ReadPositiveIntSettingOrDefault(L"TaskbarIconSize", kDefaultTaskbarIconSize);
    const int maxIconSize =
        GetMaxTaskbarIconSizeForLayout(g_settings_tbiconsize.taskbarHeight, taskbarButtonSize);
    g_settings_tbiconsize.iconSize = ClampInt(abs(requestedIconSize), kMinTaskbarIconSize, maxIconSize);
    g_settings_tbiconsize.iconSizeSmall =
        std::min(g_settings_tbiconsize.iconSize, kSystemSmallTaskbarIconSize);
}
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
bool ProtectAndMemcpy(DWORD protect, void* dst, const void* src, size_t size) {
    DWORD oldProtect;
    if (!VirtualProtect(dst, size, protect, &oldProtect)) {
        return false;
    }
    memcpy(dst, src, size);
    VirtualProtect(dst, size, oldProtect, &oldProtect);
    return true;
}
void ApplySettingsTBIconSize(int taskbarHeight) {
    if (taskbarHeight < 2) {
        taskbarHeight = 2;
    }
    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (!hTaskbarWnd) {
        Wh_Log(L"No taskbar found");
        g_taskbarHeight = taskbarHeight;
        return;
    }
    Wh_Log(L"Applying settings for taskbar %08X",
           (DWORD)(DWORD_PTR)hTaskbarWnd);
    if (!g_taskbarHeight) {
        RECT taskbarRect{};
        GetWindowRect(hTaskbarWnd, &taskbarRect);
        g_taskbarHeight = MulDiv(taskbarRect.bottom - taskbarRect.top, 96,
                                 GetDpiForWindow(hTaskbarWnd));
    }
    g_applyingSettings = true;
    if (!IsVerticalTaskbar() && taskbarHeight == g_taskbarHeight) {
        g_pendingMeasureOverride = true;
        // Temporarily change the height to force a UI refresh.
        g_taskbarHeight = taskbarHeight - 1;
        if (!TaskbarConfiguration_GetFrameSize_Original &&
            double_48_value_Original) {
            double tempTaskbarHeight = g_taskbarHeight;
            ProtectAndMemcpy(PAGE_READWRITE, double_48_value_Original,
                             &tempTaskbarHeight, sizeof(double));
        }
        // Trigger TrayUI::_HandleSettingChange.
        SendMessage(hTaskbarWnd, WM_SETTINGCHANGE, SPI_SETLOGICALDPIOVERRIDE,
                    0);
        // Wait for the change to apply.
        WaitForConditionWithTimeout(
        [] { return !g_pendingMeasureOverride.load(); },
        kTaskbarMeasureOverrideTimeoutMs,
        kTaskbarMeasurePollIntervalMs);
    }
    g_pendingMeasureOverride = true;
    g_taskbarHeight = taskbarHeight;
    if (!TaskbarConfiguration_GetFrameSize_Original &&
        double_48_value_Original) {
        double tempTaskbarHeight = g_taskbarHeight;
        ProtectAndMemcpy(PAGE_READWRITE, double_48_value_Original,
                         &tempTaskbarHeight, sizeof(double));
    }
    // Trigger TrayUI::_HandleSettingChange.
    SendMessage(hTaskbarWnd, WM_SETTINGCHANGE, SPI_SETLOGICALDPIOVERRIDE, 0);
    if (!IsVerticalTaskbar()) {
        // Wait for the change to apply.
        WaitForConditionWithTimeout(
        [] { return !g_pendingMeasureOverride.load(); },
        kTaskbarMeasureOverrideTimeoutMs,
        kTaskbarMeasurePollIntervalMs);
    } else {
        g_pendingMeasureOverride = false;
    }
    HWND hReBarWindow32 =
        FindWindowEx(hTaskbarWnd, nullptr, L"ReBarWindow32", nullptr);
    if (hReBarWindow32) {
        HWND hMSTaskSwWClass =
            FindWindowEx(hReBarWindow32, nullptr, L"MSTaskSwWClass", nullptr);
        if (hMSTaskSwWClass) {
            // Trigger CTaskBand::_HandleSyncDisplayChange.
            SendMessage(hMSTaskSwWClass, 0x452, 3, 0);
        }
    }
    g_applyingSettings = false;
}
bool HookSystemTraySymbols(HMODULE module) {
    // SystemTray.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(private: double __cdecl winrt::SystemTray::implementation::SystemTrayController::GetFrameSize(enum winrt::WindowsUdk::UI::Shell::TaskbarSize))"},
            &SystemTrayController_GetFrameSize_Original,
            SystemTrayController_GetFrameSize_Hook,
            true,  // From Windows 11 version 22H2, inlined sometimes.
        },
        {
            {LR"(private: double __cdecl winrt::SystemTray::implementation::SystemTraySecondaryController::GetFrameSize(enum winrt::WindowsUdk::UI::Shell::TaskbarSize))"},
            &SystemTraySecondaryController_GetFrameSize_Original,
            SystemTraySecondaryController_GetFrameSize_Hook,
            true,  // From Windows 11 version 22H2.
        },
        {
            {LR"(private: void __cdecl winrt::SystemTray::implementation::SystemTrayController::UpdateFrameSize(void))"},
            &SystemTrayController_UpdateFrameSize_SymbolAddress,
            nullptr,  // Hooked manually, we need the symbol address.
            true,     // Missing in older Windows 11 versions.
        },
        {
            {LR"(private: void __cdecl winrt::SystemTray::implementation::SystemTraySecondaryController::UpdateFrameSize(void))"},
            &SystemTraySecondaryController_UpdateFrameSize_Original,
            SystemTraySecondaryController_UpdateFrameSize_Hook,
            true,  // Missing in older Windows 11 versions.
        },
        {
            {LR"(public: __cdecl winrt::impl::consume_Windows_UI_Xaml_IFrameworkElement<struct winrt::SystemTray::SystemTrayFrame>::Height(double)const )"},
            &SystemTrayFrame_Height_Original,
            SystemTrayFrame_Height_Hook,
            true,  // From Windows 11 version 22H2.
        },
    };
    if (!HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }
    if (SystemTrayController_UpdateFrameSize_SymbolAddress) {
        SystemTrayController_UpdateFrameSize_InitOffsets();
        WindhawkUtils::Wh_SetFunctionHookT(
            SystemTrayController_UpdateFrameSize_SymbolAddress,
            SystemTrayController_UpdateFrameSize_Hook,
            &SystemTrayController_UpdateFrameSize_Original);
    }
    return true;
}
bool HookTaskbarViewDllSymbols(HMODULE module,
                               bool hookSystemTraySymbolsInline) {
    // Taskbar.View.dll, ExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] =  //
        {
            {
                // For Windows 11 version 21H2.
                {LR"(__real@4048000000000000)"},
                &double_48_value_Original,
                nullptr,
                true,
            },
            {
                {
                    LR"(public: __cdecl winrt::impl::consume_Windows_Foundation_Collections_IMap<struct winrt::Windows::UI::Xaml::ResourceDictionary,struct winrt::Windows::Foundation::IInspectable,struct winrt::Windows::Foundation::IInspectable>::Lookup(struct winrt::Windows::Foundation::IInspectable const &)const )",
                    // Windows 11 version 21H2.
                    LR"(public: struct winrt::Windows::Foundation::IInspectable __cdecl winrt::impl::consume_Windows_Foundation_Collections_IMap<struct winrt::Windows::UI::Xaml::ResourceDictionary,struct winrt::Windows::Foundation::IInspectable,struct winrt::Windows::Foundation::IInspectable>::Lookup(struct winrt::Windows::Foundation::IInspectable const &)const )",
                },
                &ResourceDictionary_Lookup_TaskbarView_Original,
                ResourceDictionary_Lookup_TaskbarView_Hook,
            },
            {
                // Pre-DynamicIconScaling.
                {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListItemViewModel,struct winrt::Taskbar::ITaskListItemViewModel>::GetIconHeight(void *,double *))"},
                &TaskListItemViewModel_GetIconHeight_Original,
                TaskListItemViewModel_GetIconHeight_Hook,
                true,  // Gone in KB5040527 (Taskbar.View.dll 2124.16310.10.0).
            },
            {
                // Pre-DynamicIconScaling.
                {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListGroupViewModel,struct winrt::Taskbar::ITaskbarAppItemViewModel>::GetIconHeight(void *,double *))"},
                &TaskListGroupViewModel_GetIconHeight_Original,
                TaskListGroupViewModel_GetIconHeight_Hook,
                true,  // Missing in older Windows 11 versions.
            },
            {
                // Pre-DynamicIconScaling.
                {LR"(public: static double __cdecl winrt::Taskbar::implementation::TaskbarConfiguration::GetIconHeightInViewPixels(enum winrt::WindowsUdk::UI::Shell::TaskbarSize))"},
                &TaskbarConfiguration_GetIconHeightInViewPixels_taskbarSizeEnum_Original,
                TaskbarConfiguration_GetIconHeightInViewPixels_taskbarSizeEnum_Hook,
            },
            {
                // Pre-DynamicIconScaling.
                {LR"(public: static double __cdecl winrt::Taskbar::implementation::TaskbarConfiguration::GetIconHeightInViewPixels(double))"},
                &TaskbarConfiguration_GetIconHeightInViewPixels_double_Original,
                TaskbarConfiguration_GetIconHeightInViewPixels_double_Hook,
                true,  // From Windows 11 version 22H2.
            },
            {
                {LR"(public: double __cdecl winrt::Taskbar::implementation::TaskbarConfiguration::GetIconHeightInViewPixels(void))"},
                &TaskbarConfiguration_GetIconHeightInViewPixels_method_Original,
                TaskbarConfiguration_GetIconHeightInViewPixels_method_Hook,
                true,  // From KB5044384 (October 2024).
            },
            {
                {LR"(public: void __cdecl winrt::Taskbar::implementation::TaskListButton::IconHeight(double))"},
                &TaskListButton_IconHeight_Original,
                nullptr,
                true,  // From KB5058499 (May 2025).
            },
            {
                {LR"(public: static double __cdecl winrt::Taskbar::implementation::TaskbarConfiguration::GetFrameSize(enum winrt::WindowsUdk::UI::Shell::TaskbarSize))"},
                &TaskbarConfiguration_GetFrameSize_Original,
                TaskbarConfiguration_GetFrameSize_Hook,
                true,  // From Windows 11 version 22H2.
            },
#ifdef _M_ARM64
            // In ARM64, the TaskbarConfiguration::GetFrameSize function is
            // inlined. As a workaround, hook
            // TaskbarConfiguration::UpdateFrameSize which its inlined in and do
            // some ugly assembly tinkering.
            {
                {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskbarConfiguration::UpdateFrameSize(void))"},
                &TaskbarConfiguration_UpdateFrameSize_SymbolAddress,
                nullptr,  // Hooked manually, we need the symbol address.
            },
            {
                {LR"(public: void __cdecl winrt::event<struct winrt::delegate<> >::operator()<>(void))"},
                &Event_operator_call_Original,
                Event_operator_call_Hook,
            },
#endif
            {
                {LR"(public: __cdecl winrt::impl::consume_Windows_UI_Xaml_IFrameworkElement<struct winrt::Taskbar::implementation::TaskbarFrame>::MaxHeight(double)const )"},
                &TaskbarFrame_MaxHeight_double_Original,
                nullptr,
                true,  // From Windows 11 version 22H2.
            },
            {
                {
                    LR"(public: __cdecl winrt::impl::consume_Windows_UI_Xaml_IFrameworkElement<struct winrt::Taskbar::implementation::TaskbarFrame>::Height(double)const )",
                    // Windows 11 version 21H2.
                    LR"(public: void __cdecl winrt::impl::consume_Windows_UI_Xaml_IFrameworkElement<struct winrt::Taskbar::implementation::TaskbarFrame>::Height(double)const )",
                },
                &TaskbarFrame_Height_double_Original,
                TaskbarFrame_Height_double_Hook,
                true,  // Gone in Windows 11 version 24H2.
            },
            {
                {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskbarController::OnGroupingModeChanged(void))"},
                &TaskbarController_OnGroupingModeChanged_Original,
                nullptr,
                true,  // Missing in older Windows 11 versions.
            },
            {
                {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskbarController::UpdateFrameHeight(void))"},
                &TaskbarController_UpdateFrameHeight_Original,
                TaskbarController_UpdateFrameHeight_Hook,
                true,  // Missing in older Windows 11 versions.
            },
            {
                {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskbarFrame,struct winrt::Windows::UI::Xaml::IFrameworkElementOverrides>::MeasureOverride(struct winrt::Windows::Foundation::Size,struct winrt::Windows::Foundation::Size *))"},
                &TaskbarFrame_MeasureOverride_Original,
                TaskbarFrame_MeasureOverride_Hook,
            },
            {
                {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateButtonPadding(void))"},
                &TaskListButton_UpdateButtonPadding_Original,
                TaskListButton_UpdateButtonPadding_Hook,
            },
            {
                {LR"(public: void __cdecl winrt::Taskbar::implementation::TaskListButton::OverlayIcon(struct winrt::Windows::Storage::Streams::IRandomAccessStream const &))"},
                &TaskListButton_OverlayIcon_Original,
                TaskListButton_OverlayIcon_Hook,
            },
            {
                {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateBadge(void))"},
                &TaskListButton_UpdateBadge_Original,
                TaskListButton_UpdateBadge_Hook,
            },
            {
                {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateIconColumnDefinition(void))"},
                &TaskListButton_UpdateIconColumnDefinition_Original,
                nullptr,
                true,  // Missing in older Windows 11 versions.
            },
            {
                {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateVisualStates(void))"},
                &TaskListButton_UpdateVisualStates_Original,
                TaskListButton_UpdateVisualStates_Hook,
            },
            {
                {LR"(public: virtual void __cdecl winrt::Taskbar::implementation::LaunchListItemViewModel::IconHeight(double))"},
                &LaunchListItemViewModel_IconHeight_Original,
                LaunchListItemViewModel_IconHeight_Hook,
                true,
            },
            {
                {LR"(protected: virtual void __cdecl winrt::Taskbar::implementation::ExperienceToggleButton::UpdateButtonPadding(void))"},
                &ExperienceToggleButton_UpdateButtonPadding_Original,
                ExperienceToggleButton_UpdateButtonPadding_Hook,
            },
            {
                {LR"(protected: virtual void __cdecl winrt::Taskbar::implementation::AugmentedEntryPointButton::UpdateButtonPadding(void))"},
                &AugmentedEntryPointButton_UpdateButtonPadding_Original,
                AugmentedEntryPointButton_UpdateButtonPadding_Hook,
            },
            {
                {LR"(public: __cdecl winrt::impl::consume_Windows_UI_Xaml_IFrameworkElement<struct winrt::Windows::UI::Xaml::Controls::Primitives::RepeatButton>::Width(double)const )"},
                &RepeatButton_Width_Original,
                RepeatButton_Width_Hook,
                true,  // From Windows 11 version 22H2.
            },
        };
    // On older Taskbar.View.dll versions (before the SystemTray types moved out
    // into SystemTray.dll), these SystemTray symbols live in Taskbar.View.dll
    // itself, so include them in the same hook batch when
    // hookSystemTraySymbolsInline is set.
    // Taskbar.View.dll, ExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooksSystemTray[] = {
        {
            {LR"(private: double __cdecl winrt::SystemTray::implementation::SystemTrayController::GetFrameSize(enum winrt::WindowsUdk::UI::Shell::TaskbarSize))"},
            &SystemTrayController_GetFrameSize_Original,
            SystemTrayController_GetFrameSize_Hook,
            true,  // From Windows 11 version 22H2, inlined sometimes.
        },
        {
            {LR"(private: double __cdecl winrt::SystemTray::implementation::SystemTraySecondaryController::GetFrameSize(enum winrt::WindowsUdk::UI::Shell::TaskbarSize))"},
            &SystemTraySecondaryController_GetFrameSize_Original,
            SystemTraySecondaryController_GetFrameSize_Hook,
            true,  // From Windows 11 version 22H2.
        },
        {
            {LR"(private: void __cdecl winrt::SystemTray::implementation::SystemTrayController::UpdateFrameSize(void))"},
            &SystemTrayController_UpdateFrameSize_SymbolAddress,
            nullptr,  // Hooked manually, we need the symbol address.
            true,     // Missing in older Windows 11 versions.
        },
        {
            {LR"(private: void __cdecl winrt::SystemTray::implementation::SystemTraySecondaryController::UpdateFrameSize(void))"},
            &SystemTraySecondaryController_UpdateFrameSize_Original,
            SystemTraySecondaryController_UpdateFrameSize_Hook,
            true,  // Missing in older Windows 11 versions.
        },
        {
            {LR"(public: __cdecl winrt::impl::consume_Windows_UI_Xaml_IFrameworkElement<struct winrt::SystemTray::SystemTrayFrame>::Height(double)const )"},
            &SystemTrayFrame_Height_Original,
            SystemTrayFrame_Height_Hook,
            true,  // From Windows 11 version 22H2.
        },
    };
    // Alias for the extract_mod_symbols.py script.
    using COMBINED_SH = WindhawkUtils::SYMBOL_HOOK;
    COMBINED_SH allHooks[  //
        ARRAYSIZE(symbolHooks) + ARRAYSIZE(symbolHooksSystemTray)];
    int index = 0;
    for (auto& hook : symbolHooks) {
        allHooks[index++] = std::move(hook);
    }
    if (hookSystemTraySymbolsInline) {
        for (auto& hook : symbolHooksSystemTray) {
            allHooks[index++] = std::move(hook);
        }
    }
    if (!HookSymbols(module, allHooks, index)) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }
    if (TaskListButton_IconHeight_Original) {
        TaskListButton_IconHeight_InitOffsets();
    }
#ifdef _M_ARM64
    if (TaskbarConfiguration_UpdateFrameSize_SymbolAddress) {
        TaskbarConfiguration_UpdateFrameSize_InitOffsets();
        WindhawkUtils::Wh_SetFunctionHookT(
            TaskbarConfiguration_UpdateFrameSize_SymbolAddress,
            TaskbarConfiguration_UpdateFrameSize_Hook,
            &TaskbarConfiguration_UpdateFrameSize_Original);
    }
#endif
    if (hookSystemTraySymbolsInline &&
        SystemTrayController_UpdateFrameSize_SymbolAddress) {
        SystemTrayController_UpdateFrameSize_InitOffsets();
        WindhawkUtils::Wh_SetFunctionHookT(
            SystemTrayController_UpdateFrameSize_SymbolAddress,
            SystemTrayController_UpdateFrameSize_Hook,
            &SystemTrayController_UpdateFrameSize_Original);
    }
    if (TaskbarController_OnGroupingModeChanged_Original) {
        TaskbarController_OnGroupingModeChanged_InitOffsets();
WindhawkUtils::Wh_SetFunctionHookT(
            reinterpret_cast<TaskbarController_OnGroupingModeChanged_t>(
                TaskbarController_OnGroupingModeChanged_Original),
            TaskbarController_OnGroupingModeChanged_Hook,
            &TaskbarController_OnGroupingModeChanged_Hook_Original);
    }
    if (TaskListButton_UpdateIconColumnDefinition_Original) {
        TaskListButton_UpdateIconColumnDefinition_InitOffsets();
    }
    constexpr UINT kDynamicIconScaling = 29785184;
    if (TaskbarConfiguration_GetIconHeightInViewPixels_method_Original &&
        IsOsFeatureEnabled(kDynamicIconScaling).value_or(true)) {
        g_hasDynamicIconScaling = true;
        Wh_Log(L"Dynamic icon scaling is enabled");
    }
    return true;
}
bool HookSearchUxUiDllSymbols(HMODULE module) {
    // SearchUx.UI.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(public: __cdecl winrt::impl::consume_Windows_Foundation_Collections_IMap<struct winrt::Windows::UI::Xaml::ResourceDictionary,struct winrt::Windows::Foundation::IInspectable,struct winrt::Windows::Foundation::IInspectable>::Lookup(struct winrt::Windows::Foundation::IInspectable const &)const )"},
            &ResourceDictionary_Lookup_SearchUxUi_Original,
            ResourceDictionary_Lookup_SearchUxUi_Hook,
        },
        {
            {LR"(protected: virtual void __cdecl winrt::SearchUx::SearchUI::implementation::SearchButtonBase::UpdateButtonPadding(void))"},
            &SearchButtonBase_UpdateButtonPadding_Original,
            SearchButtonBase_UpdateButtonPadding_Hook,
        },
    };
    if (!HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }
    return true;
}
bool HookTaskbarDllSymbolsTBIconSize() {
    bool loadedTaskbarDllForHooking = false;
    HMODULE module = GetModuleHandle(L"taskbar.dll");
    if (!module) {
        module = LoadLibraryEx(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
        loadedTaskbarDllForHooking = module != nullptr;
    }
    if (!module) {
        Wh_Log(L"Failed to load taskbar.dll");
        return false;
    }
    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {
            // Pre-DynamicIconScaling.
            {LR"(void __cdecl IconUtils::GetIconSize(bool,enum IconUtils::IconType,struct tagSIZE *))"},
            &IconUtils_GetIconSize_Original,
            IconUtils_GetIconSize_Hook,
        },
        {
            // Pre-DynamicIconScaling.
            {LR"(public: virtual bool __cdecl IconContainer::IsStorageRecreationRequired(class CCoSimpleArray<unsigned int,4294967294,class CSimpleArrayStandardCompareHelper<unsigned int> > const &,enum IconContainerFlags))"},
            &IconContainer_IsStorageRecreationRequired_Original,
            IconContainer_IsStorageRecreationRequired_Hook,
        },
        {
            {LR"(public: virtual void __cdecl TrayUI::GetMinSize(struct HMONITOR__ *,struct tagSIZE *))"},
            &TrayUI_GetMinSize_Original,
            TrayUI_GetMinSize_Hook,
            true,
        },
        {
            // Pre-DynamicIconScaling.
            {LR"(public: virtual unsigned __int64 __cdecl CIconLoadingFunctions::GetClassLongPtrW(struct HWND__ *,int))"},
            &CIconLoadingFunctions_GetClassLongPtrW_Original,
            CIconLoadingFunctions_GetClassLongPtrW_Hook,
        },
        {
            // Pre-DynamicIconScaling.
            {LR"(public: virtual int __cdecl CIconLoadingFunctions::SendMessageCallbackW(struct HWND__ *,unsigned int,unsigned __int64,__int64,void (__cdecl*)(struct HWND__ *,unsigned int,unsigned __int64,__int64),unsigned __int64))"},
            &CIconLoadingFunctions_SendMessageCallbackW_Original,
            CIconLoadingFunctions_SendMessageCallbackW_Hook,
        },
        {
            // Pre-DynamicIconScaling.
            {LR"(static  ShellIconLoaderV2::LoadAsyncIcon$_ResumeCoro$1())"},
            &ShellIconLoaderV2_LoadAsyncIcon__ResumeCoro_Original,
            ShellIconLoaderV2_LoadAsyncIcon__ResumeCoro_Hook,
            true,
        },
        {
            {LR"(public: void __cdecl TrayUI::_StuckTrayChange(void))"},
            &TrayUI__StuckTrayChange_Original,
        },
        {
            {LR"(public: void __cdecl TrayUI::_HandleSettingChange(struct HWND__ *,unsigned int,unsigned __int64,__int64))"},
            &TrayUI__HandleSettingChange_Original,
            TrayUI__HandleSettingChange_Hook,
        },
    };
if (!HookSymbols(module, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks))) {
        Wh_Log(L"HookSymbols failed");
        if (loadedTaskbarDllForHooking) {
            FreeLibrary(module);
        }
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
HMODULE GetTaskbarViewModuleHandle() {
    HMODULE module = GetModuleHandle(L"Taskbar.View.dll");
    if (!module) {
        module = GetModuleHandle(L"ExplorerExtensions.dll");
    }
    return module;
}
HMODULE GetSystemTrayModuleHandle() {
    HMODULE module = GetModuleHandle(L"SystemTray.dll");
    if (!module) {
        module = GetModuleHandle(L"Taskbar.View.dll");
        if (module) {
            // Starting with Taskbar.View.dll 2604.8002.200.6000, the SystemTray
            // types moved out of Taskbar.View.dll into SystemTray.dll, so don't
            // treat Taskbar.View.dll as the host at this version and above.
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
HMODULE GetSearchUxUiModuleHandle() {
    return GetModuleHandle(L"SearchUx.UI.dll");
}
using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName,
                                   HANDLE hFile,
                                   DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (!module) {
        return module;
    }
    // SystemTray.dll - skipped here when the resolved module is actually an
    // older Taskbar.View.dll (the block below hooks both in a single batch).
    if (!g_systemTrayModuleHooked && GetSystemTrayModuleHandle() == module &&
        module != GetTaskbarViewModuleHandle() &&
        !g_systemTrayModuleHooked.exchange(true)) {
        Wh_Log(L"Loaded %s", lpLibFileName);
        if (HookSystemTraySymbols(module)) {
            Wh_ApplyHookOperations();
        }
    }
    if (!g_taskbarViewDllLoadedTBIconSize && GetTaskbarViewModuleHandle() == module &&
        !g_taskbarViewDllLoadedTBIconSize.exchange(true)) {
        Wh_Log(L"Loaded %s", lpLibFileName);
        // If SystemTray.dll wasn't loaded above and this Taskbar.View.dll is an
        // older version that hosts SystemTray symbols inline, hook them in the
        // same batch.
        bool hookSystemTraySymbolsInline =
            !g_systemTrayModuleHooked &&
            GetSystemTrayModuleHandle() == module &&
            !g_systemTrayModuleHooked.exchange(true);
        if (HookTaskbarViewDllSymbols(module, hookSystemTraySymbolsInline)) {
            Wh_ApplyHookOperations();
        }
    }
    if (!g_searchUxUiDllLoaded && GetSearchUxUiModuleHandle() == module &&
        !g_searchUxUiDllLoaded.exchange(true)) {
        Wh_Log(L"Loaded %s", lpLibFileName);
        if (HookSearchUxUiDllSymbols(module)) {
            Wh_ApplyHookOperations();
        }
    }
    return module;
}
BOOL Wh_ModInitTBIconSize() {
    LoadSettingsTBIconSize();
    if (!HookTaskbarDllSymbolsTBIconSize()) {
        return FALSE;
    }
    bool delayLoadingNeeded = false;
    if (HMODULE systemTrayModule = GetSystemTrayModuleHandle()) {
        // For older Taskbar.View.dll builds the resolved module is the same
        // Taskbar.View.dll handle - in that case, defer hooking SystemTray
        // symbols until HookTaskbarViewDllSymbols runs below so it can do them
        // in a single HookSymbols batch.
        if (systemTrayModule != GetTaskbarViewModuleHandle()) {
            g_systemTrayModuleHooked = true;
            if (!HookSystemTraySymbols(systemTrayModule)) {
                return FALSE;
            }
        }
    }
    if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
        g_taskbarViewDllLoadedTBIconSize = true;
        bool hookSystemTraySymbolsInline =
            !g_systemTrayModuleHooked &&
            GetSystemTrayModuleHandle() == taskbarViewModule;
        if (hookSystemTraySymbolsInline) {
            g_systemTrayModuleHooked = true;
        }
        if (!HookTaskbarViewDllSymbols(taskbarViewModule,
                                       hookSystemTraySymbolsInline)) {
            return FALSE;
        }
    } else {
        Wh_Log(L"Taskbar view module not loaded yet");
        delayLoadingNeeded = true;
    }
    // SystemTray.dll may load after Taskbar.View.dll on newer Windows 11
    // builds, so make sure the LoadLibraryExW hook is installed to catch it.
    if (!g_systemTrayModuleHooked) {
        delayLoadingNeeded = true;
    }
    if (HMODULE searchUxUiModule = GetSearchUxUiModuleHandle()) {
        g_searchUxUiDllLoaded = true;
        if (!HookSearchUxUiDllSymbols(searchUxUiModule)) {
            return FALSE;
        }
    } else {
        Wh_Log(L"Search UX UI module not loaded yet");
        delayLoadingNeeded = true;
    }
    if (delayLoadingNeeded) {
        HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
        auto pKernelBaseLoadLibraryExW =
            (decltype(&LoadLibraryExW))GetProcAddress(kernelBaseModule,
                                                      "LoadLibraryExW");
        WindhawkUtils::Wh_SetFunctionHookT(pKernelBaseLoadLibraryExW,
                                           LoadLibraryExW_Hook,
                                           &LoadLibraryExW_Original);
    }
    WindhawkUtils::Wh_SetFunctionHookT(SHAppBarMessage, SHAppBarMessage_Hook,
                                       &SHAppBarMessage_Original);
    WindhawkUtils::Wh_SetFunctionHookT(SendMessageTimeoutW,
                                       SendMessageTimeoutW_Hook,
                                       &SendMessageTimeoutW_Original);
    return TRUE;
}
void Wh_ModAfterInitTBIconSize() {
    if (!g_systemTrayModuleHooked) {
        if (HMODULE systemTrayModule = GetSystemTrayModuleHandle()) {
            if (systemTrayModule != GetTaskbarViewModuleHandle() &&
                !g_systemTrayModuleHooked.exchange(true)) {
                Wh_Log(L"Got system tray module");
                if (HookSystemTraySymbols(systemTrayModule)) {
                    Wh_ApplyHookOperations();
                }
            }
        }
    }
    if (!g_taskbarViewDllLoadedTBIconSize) {
        if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
            if (!g_taskbarViewDllLoadedTBIconSize.exchange(true)) {
                Wh_Log(L"Got Taskbar.View.dll");
                bool hookSystemTraySymbolsInline =
                    !g_systemTrayModuleHooked &&
                    GetSystemTrayModuleHandle() == taskbarViewModule &&
                    !g_systemTrayModuleHooked.exchange(true);
                if (HookTaskbarViewDllSymbols(taskbarViewModule,
                                              hookSystemTraySymbolsInline)) {
                    Wh_ApplyHookOperations();
                }
            }
        }
    }
    if (!g_searchUxUiDllLoaded) {
        if (HMODULE searchUxUiModule = GetSearchUxUiModuleHandle()) {
            if (!g_searchUxUiDllLoaded.exchange(true)) {
                Wh_Log(L"Got SearchUx.UI.dll");
                if (HookSearchUxUiDllSymbols(searchUxUiModule)) {
                    Wh_ApplyHookOperations();
                }
            }
        }
    }
    Wh_Log(L"Deferring taskbar icon size settings until delayed initial apply");
}
void Wh_ModBeforeUninitTBIconSize() {
    g_unloading = true;
    ApplySettingsTBIconSize(g_originalTaskbarHeight ? g_originalTaskbarHeight : kSystemMediumTaskbarButtonSize);
}
void Wh_ModUninitTBIconSize() {
    if (!WaitForConditionWithTimeout(
        [] { return g_hookCallCounter.load() <= 0; },
        kHookDrainTimeoutMs,
        kHookDrainPollIntervalMs)) {
    Wh_Log(L"Timed out waiting for taskbar icon size hooks to drain");
}
}
void Wh_ModSettingsChangedTBIconSize() {
const int oldTaskbarButtonWidth = g_settings_tbiconsize.taskbarButtonWidth;
    const int oldSmallTaskbarButtonWidth = g_settings_tbiconsize.taskbarButtonWidthSmall;
LoadSettingsTBIconSize();
if (!g_unloading &&
        ((oldTaskbarButtonWidth > 0 && oldTaskbarButtonWidth != g_settings_tbiconsize.taskbarButtonWidth) ||
         (oldSmallTaskbarButtonWidth > 0 && oldSmallTaskbarButtonWidth != g_settings_tbiconsize.taskbarButtonWidthSmall))) {
        RequestTaskbarButtonSizeRelayout();
    }
    ApplySettingsTBIconSize(g_settings_tbiconsize.taskbarHeight);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////       _______..___________.     ___      .______      .___________..______    __    __  .___________..___________.  ______   .__   __. .______     ______        _______. __  .___________. __    ______   .__   __.   ////
////      /       ||           |    /   \     |   _  \     |           ||   _  \  |  |  |  | |           ||           | /  __  \  |  \ |  | |   _  \   /  __  \      /       ||  | |           ||  |  /  __  \  |  \ |  |   ////
////     |   (----``---|  |----`   /  ^  \    |  |_)  |    `---|  |----`|  |_)  | |  |  |  | `---|  |----``---|  |----`|  |  |  | |   \|  | |  |_)  | |  |  |  |    |   (----`|  | `---|  |----`|  | |  |  |  | |   \|  |   ////
////      \   \        |  |       /  /_\  \   |      /         |  |     |   _  <  |  |  |  |     |  |         |  |     |  |  |  | |  . `  | |   ___/  |  |  |  |     \   \    |  |     |  |     |  | |  |  |  | |  . `  |   ////
////  .----)   |       |  |      /  _____  \  |  |\  \----.    |  |     |  |_)  | |  `--'  |     |  |         |  |     |  `--'  | |  |\   | |  |      |  `--'  | .----)   |   |  |     |  |     |  | |  `--'  | |  |\   |   ////
////  |_______/        |__|     /__/     \__\ | _| `._____|    |__|     |______/   \______/      |__|         |__|      \______/  |__| \__| | _|       \______/  |_______/    |__|     |__|     |__|  \______/  |__| \__|   ////
////                                                                                                                                                                                                                        ////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool ApplyStyle(FrameworkElement const& element, std::wstring monitorName);
bool InitializeDebounce();
DispatcherTimer debounceTimer{nullptr};
#include <windhawk_utils.h>
#include <atomic>
#include <functional>
#include <string>
#include <dwmapi.h>
#include <roapi.h>
#include <winstring.h>
#undef GetCurrentTime
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Shapes.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/base.h>
using namespace winrt::Windows::UI::Xaml;
struct {
    bool startMenuOnTheLeft;
    bool MoveFlyoutNotificationCenter=true;
} g_settings_startbuttonposition;
enum class Target {
    Explorer,
    StartMenuExperienceHost,
};
Target g_target;
std::atomic<bool> g_taskbarViewDllLoadedStartButtonPosition;
thread_local bool g_TaskbarCollapsibleLayoutXamlTraits_ArrangeOverride;
HWND g_searchMenuWnd, g_startMenuWnd;
int g_searchMenuOriginalX, g_startMenuOriginalWidth;
STDAPI GetDpiForMonitor(HMONITOR hmonitor,
                        MONITOR_DPI_TYPE dpiType,
                        UINT* dpiX,
                        UINT* dpiY);
bool ApplyStyle(XamlRoot xamlRoot) {if(true)return false;
    FrameworkElement xamlRootContent =
        xamlRoot.Content().try_as<FrameworkElement>();
    FrameworkElement taskbarFrameRepeater = nullptr;
    FrameworkElement child = xamlRootContent;
    if (child &&
        (child = FindChildByClassName(child, L"Taskbar.TaskbarFrame")) &&
        (child = FindChildByName(child, L"RootGrid")) &&
        (child = FindChildByName(child, L"TaskbarFrameRepeater"))) {
        taskbarFrameRepeater = child;
    }
    if (!taskbarFrameRepeater) {
        return false;
    }
    auto startButton =
        EnumChildElements(taskbarFrameRepeater, [](FrameworkElement child) {
            auto childClassName = winrt::get_class_name(child);
            if (childClassName != L"Taskbar.ExperienceToggleButton") {
                return false;
            }
            auto automationId =
                Automation::AutomationProperties::GetAutomationId(child);
            return automationId == L"StartButton";
        });
    if (startButton) {
        double startButtonWidth = startButton.ActualWidth();
        Thickness startButtonMargin = startButton.Margin();
        startButtonMargin.Right = g_unloading ? 0 : -startButtonWidth;
        startButton.Margin(startButtonMargin);
    }
    auto widgetElement =
        EnumChildElements(taskbarFrameRepeater, [](FrameworkElement child) {
            auto childClassName = winrt::get_class_name(child);
            if (childClassName != L"Taskbar.AugmentedEntryPointButton") {
                return false;
            }
            if (child.Name() != L"AugmentedEntryPointButton") {
                return false;
            }
            auto margin = child.Margin();
            auto offset = child.ActualOffset();
            if (offset.x != margin.Left || offset.y != 0) {
                return false;
            }
            return true;
        });
    if (widgetElement) {
        auto margin = widgetElement.Margin();
        margin.Left = g_unloading ? 0 : 44;
        widgetElement.Margin(margin);
    }
    return true;
}
void* CTaskBand_ITaskListWndSite_vftable;
void* CSecondaryTaskBand_ITaskListWndSite_vftable;
using CTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void* pThis, void** result);
CTaskBand_GetTaskbarHost_t CTaskBand_GetTaskbarHost_Original;
void* TaskbarHost_FrameHeight_Original;
using CSecondaryTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void* pThis,
                                                           void** result);
CSecondaryTaskBand_GetTaskbarHost_t CSecondaryTaskBand_GetTaskbarHost_Original;
using std__Ref_count_base__Decref_t = void(WINAPI*)(void* pThis);
std__Ref_count_base__Decref_t std__Ref_count_base__Decref_Original;
XamlRoot XamlRootFromTaskbarHostSharedPtr(void* taskbarHostSharedPtr[2]) {
    if (!taskbarHostSharedPtr[0] && !taskbarHostSharedPtr[1]) {
        return nullptr;
    }
    size_t taskbarElementIUnknownOffset = 0x48;
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
    // Just use the default offset which will hopefully work in most cases.
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
    return XamlRootFromTaskbarHostSharedPtr(taskbarHostSharedPtr);
}
XamlRoot GetSecondaryTaskbarXamlRoot(HWND hSecondaryTaskbarWnd) {
    HWND hTaskSwWnd =
        (HWND)FindWindowEx(hSecondaryTaskbarWnd, nullptr, L"WorkerW", nullptr);
    if (!hTaskSwWnd) {
        return nullptr;
    }
    void* taskBand = (void*)GetWindowLongPtr(hTaskSwWnd, 0);
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
void ApplySettingsFromTaskbarThread() {
    Wh_Log(L"Applying settings");
    EnumThreadWindows(
        GetCurrentThreadId(),
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            WCHAR szClassName[32];
            if (GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName)) == 0) {
                return TRUE;
            }
            XamlRoot xamlRoot = nullptr;
            if (_wcsicmp(szClassName, L"Shell_TrayWnd") == 0) {
                xamlRoot = GetTaskbarXamlRoot(hWnd);
            } else if (_wcsicmp(szClassName, L"Shell_SecondaryTrayWnd") == 0) {
                xamlRoot = GetSecondaryTaskbarXamlRoot(hWnd);
            } else {
                return TRUE;
            }
            if (!xamlRoot) {
                Wh_Log(L"Getting XamlRoot failed");
                return TRUE;
            }
            const auto xamlRootContent = xamlRoot.Content().try_as<FrameworkElement>();
if (!xamlRootContent) {
    Wh_Log(L"XamlRoot content is null");
    return TRUE;
}
auto dispatcher = xamlRootContent.Dispatcher();
if (!dispatcher) {
    Wh_Log(L"XamlRoot content dispatcher is null");
    return TRUE;
}
std::wstring monitorName = GetMonitorName(hWnd);
auto applyOnDispatcher = [xamlRootContent, monitorName]() {
    if (!ApplyStyle(xamlRootContent, monitorName)) {
        Wh_Log(L"ApplyStyles failed");
    }
};
if (dispatcher.HasThreadAccess()) {
    applyOnDispatcher();
} else if (!g_unloading) {
    auto priority = winrt::Windows::UI::Core::CoreDispatcherPriority::Low;
    int highPriorityPasses = g_high_priority_dispatch_passes.load();
    while (highPriorityPasses > 0) {
        if (g_high_priority_dispatch_passes.compare_exchange_weak(
                highPriorityPasses, highPriorityPasses - 1)) {
            priority = winrt::Windows::UI::Core::CoreDispatcherPriority::High;
            break;
        }
    }
    dispatcher.TryRunAsync(priority, applyOnDispatcher);
}
            return TRUE;
        },
        0);
}
void ApplySettingsStartButtonPosition(HWND hTaskbarWnd) {
      if (hTaskbarWnd && IsWindow(hTaskbarWnd)) {
        RunFromWindowThread(
            hTaskbarWnd, [](void* pParam) { ApplySettingsFromTaskbarThread(); }, 0);
    }
}
using IUIElement_Arrange_t =
    HRESULT(WINAPI*)(void* pThis, winrt::Windows::Foundation::Rect rect);
IUIElement_Arrange_t IUIElement_Arrange_Original;
HRESULT WINAPI IIUIElement_Arrange_Hook_StartButtonPosition(void* pThis,
                                       winrt::Windows::Foundation::Rect rect) {
    auto original = [=] { return IUIElement_Arrange_Original(pThis, rect); };
if(true)return original();
    if (!g_TaskbarCollapsibleLayoutXamlTraits_ArrangeOverride || g_unloading) {
        return original();
    }
    FrameworkElement element = nullptr;
    ((IUnknown*)pThis)
        ->QueryInterface(winrt::guid_of<FrameworkElement>(),
                         winrt::put_abi(element));
    if (!element) {
        return original();
    }
    auto className = winrt::get_class_name(element);
    if (className != L"Taskbar.ExperienceToggleButton") {
        return original();
    }
    auto automationId =
        Automation::AutomationProperties::GetAutomationId(element);
    if (automationId != L"StartButton") {
        return original();
    }
    auto taskbarFrameRepeater =
        Media::VisualTreeHelper::GetParent(element).as<FrameworkElement>();
    auto widgetElement =
        EnumChildElements(taskbarFrameRepeater, [](FrameworkElement child) {
            auto childClassName = winrt::get_class_name(child);
            if (childClassName != L"Taskbar.AugmentedEntryPointButton") {
                return false;
            }
            if (child.Name() != L"AugmentedEntryPointButton") {
                return false;
            }
            auto margin = child.Margin();
            auto offset = child.ActualOffset();
            if (offset.x != margin.Left || offset.y != 0) {
                return false;
            }
            return true;
        });
    if (!widgetElement) {
        element.Dispatcher().TryRunAsync(
            winrt::Windows::UI::Core::CoreDispatcherPriority::High,
            [element]() {
                double width = element.ActualWidth();
                double minX = std::numeric_limits<double>::infinity();
                auto taskbarFrameRepeater =
                    Media::VisualTreeHelper::GetParent(element)
                        .as<FrameworkElement>();
                EnumChildElements(taskbarFrameRepeater,
                                  [&element, &minX](FrameworkElement child) {
                                      if (child == element) {
                                          return false;
                                      }
                                      auto offset = child.ActualOffset();
                                      if (offset.x >= 0 && offset.x < minX) {
                                          minX = offset.x;
                                      }
                                      return false;
                                  });
                if (minX < width) {
                    Thickness margin = element.Margin();
                    element.Margin(margin);
                } else if (minX > width * 2) {
                    Thickness margin = element.Margin();
                    element.Margin(margin);
                }
            });
    }
    // Force the start button to have X = 0.
    winrt::Windows::Foundation::Rect newRect = rect;
    newRect.X = 0;
    return IUIElement_Arrange_Original(pThis, newRect);
}
using TaskbarCollapsibleLayoutXamlTraits_ArrangeOverride_t =
    HRESULT(WINAPI*)(void* pThis,
                     void* context,
                     winrt::Windows::Foundation::Size size,
                     winrt::Windows::Foundation::Size* resultSize);
TaskbarCollapsibleLayoutXamlTraits_ArrangeOverride_t
    TaskbarCollapsibleLayoutXamlTraits_ArrangeOverride_Original;
HRESULT WINAPI TaskbarCollapsibleLayoutXamlTraits_ArrangeOverride_Hook(
    void* pThis,
    void* context,
    winrt::Windows::Foundation::Size size,
    winrt::Windows::Foundation::Size* resultSize) {
    [[maybe_unused]] static bool hooked = [] {
        Shapes::Rectangle rectangle;
        IUIElement element = rectangle;
        void** vtable = *(void***)winrt::get_abi(element);
        auto arrange = (IUIElement_Arrange_t)vtable[92];
        WindhawkUtils::SetFunctionHook(arrange, IIUIElement_Arrange_Hook_StartButtonPosition,
                                       &IUIElement_Arrange_Original);
        Wh_ApplyHookOperations();
        return true;
    }();
    g_TaskbarCollapsibleLayoutXamlTraits_ArrangeOverride = true;
    HRESULT ret = TaskbarCollapsibleLayoutXamlTraits_ArrangeOverride_Original(
        pThis, context, size, resultSize);
    g_TaskbarCollapsibleLayoutXamlTraits_ArrangeOverride = false;
    return ret;
}
using ExperienceToggleButton_UpdateButtonPadding_t = void(WINAPI*)(void* pThis);
using AugmentedEntryPointButton_UpdateButtonPadding_t =
    void(WINAPI*)(void* pThis);
void WINAPI AugmentedEntryPointButton_UpdateButtonPadding_Hook_StartButtonPosition(void* pThis) {
    AugmentedEntryPointButton_UpdateButtonPadding_Original(pThis);
    if (g_unloading) {
        return;
    }
    FrameworkElement button = nullptr;
    ((IUnknown**)pThis)[1]->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                           winrt::put_abi(button));
    if (!button) {
        return;
    }
    button.Dispatcher().TryRunAsync(
        winrt::Windows::UI::Core::CoreDispatcherPriority::High, [button]() {
            auto offset = button.ActualOffset();
            if (offset.x != 0 || offset.y != 0) {
                return;
            }
            auto margin = button.Margin();
            margin.Left = 44;
            button.Margin(margin);
        });
}
bool IsTaskbarGeometryMessage(UINT msg) {
  switch (msg) {
    case WM_SIZE:
    case WM_WINDOWPOSCHANGED:
    case WM_SETTINGCHANGE:
    case WM_DISPLAYCHANGE:
    case WM_DPICHANGED:
      return true;
    default:
      return false;
  }
}
using TrayUI__Hide_t = void(WINAPI*)(void* pThis);
TrayUI__Hide_t TrayUI__Hide_Original;
void WINAPI TrayUI__Hide_Hook(void* pThis) {
  if (TrayUI__Hide_Original) {
    TrayUI__Hide_Original(pThis);
  }
  ApplySettingsFromTaskbarThreadIfRequired();
}
using CSecondaryTray__AutoHide_t = void(WINAPI*)(void* pThis, bool param1);
CSecondaryTray__AutoHide_t CSecondaryTray__AutoHide_Original;
void WINAPI CSecondaryTray__AutoHide_Hook(void* pThis, bool param1) {
  if (CSecondaryTray__AutoHide_Original) {
    CSecondaryTray__AutoHide_Original(pThis, param1);
  }
  ApplySettingsFromTaskbarThreadIfRequired();
}
#ifndef HSHELL_GETMINRECT
#define HSHELL_GETMINRECT 5
#endif
static UINT g_shellHookMessageTai = 0;
static bool TryCorrectShellHookMinRectMessageTai(UINT Msg, WPARAM wParam, LPARAM lParam);
using TrayUI_WndProc_t = LRESULT(WINAPI*)(void* pThis, HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam, bool* flag);
TrayUI_WndProc_t TrayUI_WndProc_Original;
LRESULT WINAPI TrayUI_WndProc_Hook(void* pThis, HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam, bool* flag) {
  RecordTaskbarInvocationMonitorTai(hWnd, Msg);
  LRESULT ret = TrayUI_WndProc_Original
      ? TrayUI_WndProc_Original(pThis, hWnd, Msg, wParam, lParam, flag)
      : 0;
    if (TryCorrectShellHookMinRectMessageTai(Msg, wParam, lParam)) {
        return ret;
      }
  if (IsTaskbarGeometryMessage(Msg)) {
    ApplySettingsFromTaskbarThreadGeometryChanged();
  } else {
    ApplySettingsFromTaskbarThreadIfRequired();
  }
  return ret;
}
using CSecondaryTray_v_WndProc_t = LRESULT(WINAPI*)(void* pThis, HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
CSecondaryTray_v_WndProc_t CSecondaryTray_v_WndProc_Original;
LRESULT WINAPI CSecondaryTray_v_WndProc_Hook(void* pThis, HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
  RecordTaskbarInvocationMonitorTai(hWnd, Msg);
  LRESULT ret = CSecondaryTray_v_WndProc_Original
      ? CSecondaryTray_v_WndProc_Original(pThis, hWnd, Msg, wParam, lParam)
      : 0;
       if (TryCorrectShellHookMinRectMessageTai(Msg, wParam, lParam)) {
    return ret;
  }
  if (IsTaskbarGeometryMessage(Msg)) {
    ApplySettingsFromTaskbarThreadGeometryChanged();
  } else {
    ApplySettingsFromTaskbarThreadIfRequired();
  }
  return ret;
}
using CTaskBand__ProcessWindowDestroyed_t = void(WINAPI*)(void* pThis, void* pHwnd);
CTaskBand__ProcessWindowDestroyed_t CTaskBand__ProcessWindowDestroyed_Original;
void WINAPI CTaskBand__ProcessWindowDestroyed_Hook(void* pThis, void* pHwnd) {
  Wh_Log(L"CTaskBand::CTaskBand__ProcessWindowDestroyed_Hook Hook");
  if (CTaskBand__ProcessWindowDestroyed_Original) {
    CTaskBand__ProcessWindowDestroyed_Original(pThis, pHwnd);
  }
  ApplySettingsFromTaskbarThreadImmediately();
}
using CTaskBand__InsertItem_t = long(WINAPI*)(void* pThis, void* pHwnd, void** ppTaskItem, void* pHwnd1, void* pHwnd2);
CTaskBand__InsertItem_t CTaskBand__InsertItem_Original;
long WINAPI CTaskBand__InsertItem_Hook(void* pThis, void* pHwnd, void** ppTaskItem, void* pHwnd1, void* pHwnd2) {
  Wh_Log(L"CTaskBand::_InsertItem Hook");
  auto original_call = CTaskBand__InsertItem_Original
      ? CTaskBand__InsertItem_Original(pThis, pHwnd, ppTaskItem, pHwnd1, pHwnd2)
      : E_FAIL;
  ApplySettingsFromTaskbarThreadImmediately();
  return original_call;
}
using CTaskBand__UpdateAllIcons_t = void(WINAPI*)(void* pThis);
CTaskBand__UpdateAllIcons_t CTaskBand__UpdateAllIcons_Original;
void WINAPI CTaskBand__UpdateAllIcons_Hook(void* pThis) {
  Wh_Log(L"CTaskBand::_UpdateAllIcons Hook");
  if (CTaskBand__UpdateAllIcons_Original) {
    CTaskBand__UpdateAllIcons_Original(pThis);
  }
  ApplySettingsFromTaskbarThreadIfRequired();
}
using CTaskBand__TaskOrderChanged_t = void(WINAPI*)(void* pThis, void* pTaskGroup, int param);
CTaskBand__TaskOrderChanged_t CTaskBand__TaskOrderChanged_Original;
void WINAPI CTaskBand__TaskOrderChanged_Hook(void* pThis, void* pTaskGroup, int param) {
  Wh_Log(L"CTaskBand::TaskOrderChanged Hook");
  if (CTaskBand__TaskOrderChanged_Original) {
    CTaskBand__TaskOrderChanged_Original(pThis, pTaskGroup, param);
  }
  ApplySettingsFromTaskbarThreadImmediately();
}
using CImpWndProc__WndProc_t = __int64(WINAPI*)(void* pThis, void* pHwnd, unsigned int msg, unsigned __int64 wParam, __int64 lParam);
CImpWndProc__WndProc_t CImpWndProc__WndProc_Original;
__int64 WINAPI CImpWndProc__WndProc_Hook(void* pThis, void* pHwnd, unsigned int msg, unsigned __int64 wParam, __int64 lParam) {
  RecordTaskbarInvocationMonitorTai(reinterpret_cast<HWND>(pHwnd), msg);
  __int64 ret = CImpWndProc__WndProc_Original
      ? CImpWndProc__WndProc_Original(pThis, pHwnd, msg, wParam, lParam)
      : 0;
  if (TryCorrectShellHookMinRectMessageTai(msg, static_cast<WPARAM>(wParam), static_cast<LPARAM>(lParam))) {
    return ret;
  }
  ApplySettingsFromTaskbarThreadIfRequired();
  return ret;
}
using CTaskBand__WndProc_t = __int64(WINAPI*)(void* pThis, void* pHwnd, unsigned int msg, unsigned __int64 wParam, __int64 lParam);
CTaskBand__WndProc_t CTaskBand__WndProc_Original;
__int64 WINAPI CTaskBand__WndProc_Hook(void* pThis, void* pHwnd, unsigned int msg, unsigned __int64 wParam, __int64 lParam) {
  RecordTaskbarInvocationMonitorTai(reinterpret_cast<HWND>(pHwnd), msg);
  __int64 ret = CTaskBand__WndProc_Original
      ? CTaskBand__WndProc_Original(pThis, pHwnd, msg, wParam, lParam)
      : 0;
  if (TryCorrectShellHookMinRectMessageTai(msg, static_cast<WPARAM>(wParam), static_cast<LPARAM>(lParam))) {
    return ret;
  }
  if (IsTaskbarGeometryMessage(msg)) {
    ApplySettingsFromTaskbarThreadGeometryChanged();
  } else {
    ApplySettingsFromTaskbarThreadIfRequired();
  }
  return ret;
}
using CTaskListWnd__WndProc_t = __int64(WINAPI*)(void* pThis, void* pHwnd, unsigned int msg, unsigned __int64 wParam, __int64 lParam);
CTaskListWnd__WndProc_t CTaskListWnd__WndProc_Original;
__int64 WINAPI CTaskListWnd__WndProc_Hook(void* pThis, void* pHwnd, unsigned int msg, unsigned __int64 wParam, __int64 lParam) {
  RecordTaskbarInvocationMonitorTai(reinterpret_cast<HWND>(pHwnd), msg);
  __int64 ret = CTaskListWnd__WndProc_Original
      ? CTaskListWnd__WndProc_Original(pThis, pHwnd, msg, wParam, lParam)
      : 0;
  if (TryCorrectShellHookMinRectMessageTai(msg, static_cast<WPARAM>(wParam), static_cast<LPARAM>(lParam))) {
    return ret;
  }
  if (IsTaskbarGeometryMessage(msg)) {
    ApplySettingsFromTaskbarThreadGeometryChanged();
  } else {
    ApplySettingsFromTaskbarThreadIfRequired();
  }
  return ret;
}
using CSecondaryTaskBand__WndProc_t = __int64(WINAPI*)(void* pThis, void* pHwnd, unsigned int msg, unsigned __int64 wParam, __int64 lParam);
CSecondaryTaskBand__WndProc_t CSecondaryTaskBand__WndProc_Original;
__int64 WINAPI CSecondaryTaskBand__WndProc_Hook(void* pThis, void* pHwnd, unsigned int msg, unsigned __int64 wParam, __int64 lParam) {
  RecordTaskbarInvocationMonitorTai(reinterpret_cast<HWND>(pHwnd), msg);
  __int64 ret = CSecondaryTaskBand__WndProc_Original
      ? CSecondaryTaskBand__WndProc_Original(pThis, pHwnd, msg, wParam, lParam)
      : 0;
  if (TryCorrectShellHookMinRectMessageTai(msg, static_cast<WPARAM>(wParam), static_cast<LPARAM>(lParam))) {
    return ret;
  }
  if (IsTaskbarGeometryMessage(msg)) {
    ApplySettingsFromTaskbarThreadGeometryChanged();
  } else {
    ApplySettingsFromTaskbarThreadIfRequired();
  }
  return ret;
}
using CTraySearchControl__WndProc_t = __int64(WINAPI*)(void* pThis, void* pHwnd, unsigned int msg, unsigned __int64 wParam, __int64 lParam);
CTraySearchControl__WndProc_t CTraySearchControl__WndProc_Original;
__int64 WINAPI CTraySearchControl__WndProc_Hook(void* pThis, void* pHwnd, unsigned int msg, unsigned __int64 wParam, __int64 lParam) {
  RecordTaskbarInvocationMonitorTai(reinterpret_cast<HWND>(pHwnd), msg);
  ApplySettingsFromTaskbarThreadIfRequired();
  return CTraySearchControl__WndProc_Original
      ? CTraySearchControl__WndProc_Original(pThis, pHwnd, msg, wParam, lParam)
      : 0;
}
interface ITaskGroup;
interface ITaskItem;
using CTaskBand__UpdateItemIcon_WithArgs_t = void(WINAPI*)(void* pThis, ITaskGroup* param1, ITaskItem* param2);
CTaskBand__UpdateItemIcon_WithArgs_t CTaskBand__UpdateItemIcon_WithArgs_Original;
void WINAPI CTaskBand__UpdateItemIcon_WithArgs_Hook(void* pThis, ITaskGroup* param1, ITaskItem* param2) {
  Wh_Log(L"Method called: CTaskBand__UpdateItemIcon");
  if (CTaskBand__UpdateItemIcon_WithArgs_Original) {
    CTaskBand__UpdateItemIcon_WithArgs_Original(pThis, param1, param2);
  }
  ApplySettingsFromTaskbarThreadIfRequired();
}
using CTaskBand_RemoveIcon_WithArgs_t = void(WINAPI*)(void* pThis, ITaskItem* param1);
CTaskBand_RemoveIcon_WithArgs_t CTaskBand_RemoveIcon_WithArgs_Original;
void WINAPI CTaskBand_RemoveIcon_WithArgs_Hook(void* pThis, ITaskItem* param1) {
  Wh_Log(L"Method called: CTaskBand_RemoveIcon");
  if (CTaskBand_RemoveIcon_WithArgs_Original) {
    CTaskBand_RemoveIcon_WithArgs_Original(pThis, param1);
  }
  ApplySettingsFromTaskbarThreadImmediately();
}
using ITaskbarSettings_get_Alignment_t = HRESULT(WINAPI*)(void* pThis, int* alignment);
ITaskbarSettings_get_Alignment_t ITaskbarSettings_get_Alignment_Original;
HRESULT WINAPI ITaskbarSettings_get_Alignment_Hook(void* pThis, int* alignment) {
  HRESULT ret = ITaskbarSettings_get_Alignment_Original
      ? ITaskbarSettings_get_Alignment_Original(pThis, alignment)
      : E_FAIL;
  Wh_Log(L"Method called: ITaskbarSettings_get_Alignment_Hook alignment: %d", alignment ? *alignment : -1);
  if (alignment && SUCCEEDED(ret)) {
    *alignment = 1;
  }
  return ret;
}
#include <windowsx.h>
using CTaskListWnd_ComputeJumpViewPosition_t = HRESULT(WINAPI*)(void* pThis, void* taskBtnGroup, int param2, winrt::Windows::Foundation::Point* point, HorizontalAlignment* horizontalAlignment, VerticalAlignment* verticalAlignment);
CTaskListWnd_ComputeJumpViewPosition_t CTaskListWnd_ComputeJumpViewPosition_Original;
HRESULT WINAPI CTaskListWnd_ComputeJumpViewPosition_Hook(void* pThis, void* taskBtnGroup, int param2, winrt::Windows::Foundation::Point* point, HorizontalAlignment* horizontalAlignment, VerticalAlignment* verticalAlignment) {
  HRESULT ret = CTaskListWnd_ComputeJumpViewPosition_Original
      ? CTaskListWnd_ComputeJumpViewPosition_Original(pThis, taskBtnGroup, param2, point, horizontalAlignment, verticalAlignment)
      : E_FAIL;
  if (point) {
    DWORD messagePos = GetMessagePos();
    POINT pt{
        GET_X_LPARAM(messagePos),
        GET_Y_LPARAM(messagePos),
    };
    point->X = pt.x;
  }
  return ret;
}
using TrayUI__OnDPIChanged_WithoutArgs_t = void(WINAPI*)(void* pThis);
TrayUI__OnDPIChanged_WithoutArgs_t TrayUI__OnDPIChanged_WithoutArgs_Original;
void WINAPI TrayUI__OnDPIChanged_WithoutArgs_Hook(void* pThis) {
               if (TrayUI__OnDPIChanged_WithoutArgs_Original) {
                   TrayUI__OnDPIChanged_WithoutArgs_Original(pThis);
               }
               RequestTaskbarDimensionInvalidation();
            }
bool HookTaskbarDllSymbolsStartButtonPosition() {
    bool loadedTaskbarDllForHooking = false;
    HMODULE module = GetModuleHandle(L"taskbar.dll");
    if (!module) {
        module = LoadLibraryEx(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
        loadedTaskbarDllForHooking = module != nullptr;
    }
    if (!module) {
        Wh_Log(L"Failed to load taskbar.dll");
        return false;
    }
    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {{{LR"(public: virtual void __cdecl CTaskBand::RemoveIcon(struct ITaskItem *))"}, &CTaskBand_RemoveIcon_WithArgs_Original, CTaskBand_RemoveIcon_WithArgs_Hook},
    {{LR"(protected: void __cdecl CTaskBand::_UpdateItemIcon(struct ITaskGroup *,struct ITaskItem *))"}, &CTaskBand__UpdateItemIcon_WithArgs_Original, CTaskBand__UpdateItemIcon_WithArgs_Hook},
    {
        {LR"(protected: static __int64 __cdecl CImpWndProc::s_WndProc(struct HWND__ *,unsigned int,unsigned __int64,__int64))"},
        &CImpWndProc__WndProc_Original,
        CImpWndProc__WndProc_Hook,
    },
    {
        {LR"(protected: static __int64 __cdecl CTraySearchControl::s_WndProc(struct HWND__ *,unsigned int,unsigned __int64,__int64))"},
        &CTraySearchControl__WndProc_Original,
        CTraySearchControl__WndProc_Hook,
    },
    {
        {LR"(private: virtual __int64 __cdecl CSecondaryTaskBand::v_WndProc(struct HWND__ *,unsigned int,unsigned __int64,__int64))"},
        &CSecondaryTaskBand__WndProc_Original,
        CSecondaryTaskBand__WndProc_Hook,
    },
    {
        {LR"(protected: virtual __int64 __cdecl CTaskBand::v_WndProc(struct HWND__ *,unsigned int,unsigned __int64,__int64))"},
        &CTaskBand__WndProc_Original,
        CTaskBand__WndProc_Hook,
    },
    {
        {LR"(protected: virtual __int64 __cdecl CTaskListWnd::v_WndProc(struct HWND__ *,unsigned int,unsigned __int64,__int64))"},
        &CTaskListWnd__WndProc_Original,
        CTaskListWnd__WndProc_Hook,
    },
    /////////////////////////////////////
    {
        {LR"(protected: long __cdecl CTaskBand::_InsertItem(struct HWND__ *,struct ITaskItem * *,struct HWND__ *,struct HWND__ *))"},
        &CTaskBand__InsertItem_Original,
        CTaskBand__InsertItem_Hook,
    },
    {
        {LR"(protected: void __cdecl CTaskBand::_UpdateAllIcons(void))"},
        &CTaskBand__UpdateAllIcons_Original,
        CTaskBand__UpdateAllIcons_Hook,
    },
    {
        {LR"(public: virtual void __cdecl CTaskBand::TaskOrderChanged(struct ITaskGroup *,int))"},
        &CTaskBand__TaskOrderChanged_Original,
        CTaskBand__TaskOrderChanged_Hook,
    },
    {
        {LR"(protected: void __cdecl CTaskBand::_ProcessWindowDestroyed(struct HWND__ *))"},
        &CTaskBand__ProcessWindowDestroyed_Original,
        CTaskBand__ProcessWindowDestroyed_Hook,
    },
    {
        {LR"(public: void __cdecl TrayUI::_Hide(void))"},
        &TrayUI__Hide_Original,
        TrayUI__Hide_Hook,
    },
    {
        {LR"(private: void __cdecl CSecondaryTray::_AutoHide(bool))"},
        &CSecondaryTray__AutoHide_Original,
        CSecondaryTray__AutoHide_Hook,
    },
    {
        {LR"(public: virtual __int64 __cdecl TrayUI::WndProc(struct HWND__ *,unsigned int,unsigned __int64,__int64,bool *))"},
        &TrayUI_WndProc_Original,
        TrayUI_WndProc_Hook,
    },
    {
        {LR"(private: virtual __int64 __cdecl CSecondaryTray::v_WndProc(struct HWND__ *,unsigned int,unsigned __int64,__int64))"},
        &CSecondaryTray_v_WndProc_Original,
        CSecondaryTray_v_WndProc_Hook,
    },
    {
        {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::WindowsUdk::UI::Shell::implementation::TaskbarSettings,struct winrt::WindowsUdk::UI::Shell::ITaskbarSettings>::get_Alignment(int *))"},
        &ITaskbarSettings_get_Alignment_Original,
        ITaskbarSettings_get_Alignment_Hook,
    },
    {
        {LR"(protected: long __cdecl CTaskListWnd::_ComputeJumpViewPosition(struct ITaskBtnGroup *,int,struct Windows::Foundation::Point &,enum Windows::UI::Xaml::HorizontalAlignment &,enum Windows::UI::Xaml::VerticalAlignment &)const )"},
        &CTaskListWnd_ComputeJumpViewPosition_Original,
        CTaskListWnd_ComputeJumpViewPosition_Hook,
    },
    {{LR"(public: void __cdecl TrayUI::_OnDPIChanged(void))"}, &TrayUI__OnDPIChanged_WithoutArgs_Original, TrayUI__OnDPIChanged_WithoutArgs_Hook},
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
     if (!HookSymbols(module, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks))) {
        Wh_Log(L"HookSymbols failed");
        if (loadedTaskbarDllForHooking) {
            FreeLibrary(module);
        }
        return false;
    }
    return true;
}
using TaskbarTelemetry_StartItemEntranceAnimation_t = void(WINAPI*)(const bool&);
static TaskbarTelemetry_StartItemEntranceAnimation_t orig_StartItemEntranceAnimation = nullptr;
using TaskbarTelemetry_StartItemPlateEntranceAnimation_t = void(WINAPI*)(const bool&);
static TaskbarTelemetry_StartItemPlateEntranceAnimation_t orig_StartItemPlateEntranceAnimation = nullptr;
void WINAPI Hook_StartItemEntranceAnimation_call(const bool& b) {
  Wh_Log(L"[Hook] TaskbarTelemetry::StartItemEntranceAnimation(%d)", b);
  if (orig_StartItemEntranceAnimation) {
    orig_StartItemEntranceAnimation(b);
  }
  ApplySettingsFromTaskbarThreadImmediately();
}
void WINAPI Hook_StartItemPlateEntranceAnimation_call(const bool& b) {
  Wh_Log(L"[Hook] TaskbarTelemetry::StartItemPlateEntranceAnimation(%d)", b);
  if (orig_StartItemPlateEntranceAnimation) {
    orig_StartItemPlateEntranceAnimation(b);
  }
  ApplySettingsFromTaskbarThreadImmediately();
}
using TaskbarTelemetry_StartEntranceAnimationCompleted_WithoutArgs_t = void(WINAPI*)(void* pThis);
TaskbarTelemetry_StartEntranceAnimationCompleted_WithoutArgs_t TaskbarTelemetry_StartEntranceAnimationCompleted_WithoutArgs_Original;
static void WINAPI TaskbarTelemetry_StartEntranceAnimationCompleted_WithoutArgs_Hook(void* pThis) {
  Wh_Log(L"Method called: TaskbarTelemetry_StartEntranceAnimationCompleted");
  if (TaskbarTelemetry_StartEntranceAnimationCompleted_WithoutArgs_Original) {
    TaskbarTelemetry_StartEntranceAnimationCompleted_WithoutArgs_Original(pThis);
  }
  return;
}
using TaskbarTelemetry_StartHideAnimationCompleted_WithoutArgs_t = void(WINAPI*)(void* pThis);
TaskbarTelemetry_StartHideAnimationCompleted_WithoutArgs_t TaskbarTelemetry_StartHideAnimationCompleted_WithoutArgs_Original;
static void WINAPI TaskbarTelemetry_StartHideAnimationCompleted_WithoutArgs_Hook(void* pThis) {
  if (TaskbarTelemetry_StartHideAnimationCompleted_WithoutArgs_Original) {
    TaskbarTelemetry_StartHideAnimationCompleted_WithoutArgs_Original(pThis);
  }
  Wh_Log(L"Method called: TaskbarTelemetry_StartHideAnimationCompleted");
  return;
}
bool HookTaskbarViewDllSymbolsStartButtonPosition(HMODULE module) {
    // Taskbar.View.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {{{LR"(public: static void __cdecl TaskbarTelemetry::StartItemEntranceAnimation<bool const &>(bool const &))"}, &orig_StartItemEntranceAnimation, Hook_StartItemEntranceAnimation_call},
    {{LR"(public: static void __cdecl TaskbarTelemetry::StartItemPlateEntranceAnimation<bool const &>(bool const &))"}, &orig_StartItemPlateEntranceAnimation, Hook_StartItemPlateEntranceAnimation_call},
    {{LR"(public: static void __cdecl TaskbarTelemetry::StartHideAnimationCompleted(void))"}, &TaskbarTelemetry_StartHideAnimationCompleted_WithoutArgs_Original, TaskbarTelemetry_StartHideAnimationCompleted_WithoutArgs_Hook},
    {{LR"(public: static void __cdecl TaskbarTelemetry::StartEntranceAnimationCompleted(void))"}, &TaskbarTelemetry_StartEntranceAnimationCompleted_WithoutArgs_Original, TaskbarTelemetry_StartEntranceAnimationCompleted_WithoutArgs_Hook},
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskbarCollapsibleLayout,struct winrt::Microsoft::UI::Xaml::Controls::IVirtualizingLayoutOverrides>::ArrangeOverride(void *,struct winrt::Windows::Foundation::Size,struct winrt::Windows::Foundation::Size *))"},
            &TaskbarCollapsibleLayoutXamlTraits_ArrangeOverride_Original,
            TaskbarCollapsibleLayoutXamlTraits_ArrangeOverride_Hook,
        },
        {
            {LR"(protected: virtual void __cdecl winrt::Taskbar::implementation::ExperienceToggleButton::UpdateButtonPadding(void))"},
            &ExperienceToggleButton_UpdateButtonPadding_Original,
            ExperienceToggleButton_UpdateButtonPadding_Hook,
        },
        {
            {LR"(protected: virtual void __cdecl winrt::Taskbar::implementation::AugmentedEntryPointButton::UpdateButtonPadding(void))"},
            &AugmentedEntryPointButton_UpdateButtonPadding_Original,
            AugmentedEntryPointButton_UpdateButtonPadding_Hook_StartButtonPosition,
        },
    };
    return HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks));
}
void HandleLoadedModuleIfTaskbarView(HMODULE module, LPCWSTR lpLibFileName) {
    if (!g_taskbarViewDllLoadedStartButtonPosition && GetTaskbarViewModuleHandle() == module &&
        !g_taskbarViewDllLoadedStartButtonPosition.exchange(true)) {
        Wh_Log(L"Loaded %s", lpLibFileName);
        if (HookTaskbarViewDllSymbolsStartButtonPosition(module)) {
            Wh_ApplyHookOperations();
        }
    }
}
using LoadLibraryExW_t = decltype(&LoadLibraryExW);
HMODULE WINAPI LoadLibraryExW_Hook_StartButtonPosition(LPCWSTR lpLibFileName,
                                   HANDLE hFile,
                                   DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (module) {
        HandleLoadedModuleIfTaskbarView(module, lpLibFileName);
    }
    return module;
}
std::wstring GetProcessFileName(DWORD dwProcessId) {
    HANDLE hProcess =
        OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, dwProcessId);
    if (!hProcess) {
        return std::wstring{};
    }
    WCHAR processPath[MAX_PATH];
    DWORD dwSize = ARRAYSIZE(processPath);
    if (!QueryFullProcessImageName(hProcess, 0, processPath, &dwSize)) {
        CloseHandle(hProcess);
        return std::wstring{};
    }
    CloseHandle(hProcess);
    PCWSTR processFileNameUpper = wcsrchr(processPath, L'\\');
    if (!processFileNameUpper) {
        return std::wstring{};
    }
    processFileNameUpper++;
    return processFileNameUpper;
}
static bool TextEqualsOrdinalIgnoreCaseFlyoutTai(
    std::wstring_view left,
    std::wstring_view right) {
    if (left.size() != right.size() ||
        left.size() > static_cast<size_t>(std::numeric_limits<int>::max())) {
        return false;
    }
    return CompareStringOrdinal(
               left.data(),
               static_cast<int>(left.size()),
               right.data(),
               static_cast<int>(right.size()),
               TRUE) == CSTR_EQUAL;
}
static std::wstring GetWindowTitleForFlyoutTai(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd)) {
        return L"";
    }
    int length = GetWindowTextLengthW(hwnd);
    if (length <= 0 || length > 32767) {
        return L"";
    }
    std::wstring title(static_cast<size_t>(length) + 1, L'\0');
    int copied = GetWindowTextW(hwnd, title.data(), length + 1);
    if (copied <= 0) {
        return L"";
    }
    title.resize(static_cast<size_t>(copied));
    return title;
}
static bool IsNotificationCenterTitleForFlyoutTai(std::wstring_view title) {
    if (title.empty()) {
        return false;
    }
    // Keep this intentionally conservative. ShellExperienceHost hosts other
    // CoreWindow surfaces such as Share, and those must be left untouched.
    // If a locale is missing, the window is ignored instead of being moved.
    static constexpr PCWSTR kNotificationCenterTitles[] = {
        L"Notification Center",
        L"Notification Centre",
        L"Action Center",
        L"Action Centre",
        L"Centro de notificaciones",
        L"Centre de notifications",
        L"Centro notifiche",
        L"Centro de notificações",
        L"Benachrichtigungscenter",
        L"Benachrichtigungszentrum",
        L"Meldingscentrum",
        L"Centrum powiadomień",
        L"Centrum oznámení",
        L"Centrum upozornění",
        L"Centrum upozornení",
        L"Értesítési központ",
        L"Centru de notificări",
        L"Centar za obavijesti",
        L"Središče za obvestila",
        L"Centar za obaveštenja",
        L"Notifikationscenter",
        L"Meddelelsescenter",
        L"Varslingssenter",
        L"Ilmoituskeskus",
        L"Aviseringscenter",
        L"Teavituskeskus",
        L"Paziņojumu centrs",
        L"Pranešimų centras",
        L"Κέντρο ειδοποιήσεων",
        L"Центр уведомлений",
        L"Центр сповіщень",
        L"Център за известия",
        L"Bildirim merkezi",
        L"Pusat pemberitahuan",
        L"Trung tâm thông báo",
        L"ศูนย์การแจ้งเตือน",
        L"מרכז ההודעות",
        L"מרכז ההתראות",
        L"مركز الإشعارات",
        L"通知中心",
        L"通知センター",
        L"알림 센터",
    };
    for (PCWSTR knownTitle : kNotificationCenterTitles) {
        if (TextEqualsOrdinalIgnoreCaseFlyoutTai(title, knownTitle)) {
            return true;
        }
    }
    return false;
}
static bool IsNotificationCenterShellExperienceHostWindowTai(HWND hwnd) {
    std::wstring title = GetWindowTitleForFlyoutTai(hwnd);
    bool isNotificationCenter = IsNotificationCenterTitleForFlyoutTai(title);
    if (!isNotificationCenter) {
        Wh_Log(
            L"[Flyout] Ignoring ShellExperienceHost CoreWindow hwnd=%08X title='%s'",
            (DWORD)(DWORD_PTR)hwnd,
            title.c_str());
    }
    return isNotificationCenter;
}
using DwmSetWindowAttribute_t = decltype(&DwmSetWindowAttribute);
DwmSetWindowAttribute_t DwmSetWindowAttribute_Original;
HRESULT WINAPI DwmSetWindowAttribute_Hook(HWND hwnd,
                                          DWORD dwAttribute,
                                          LPCVOID pvAttribute,
                                          DWORD cbAttribute) {
    auto original = [=]() {
        return DwmSetWindowAttribute_Original(hwnd, dwAttribute, pvAttribute,
                                              cbAttribute);
    };
    if (dwAttribute != DWMWA_CLOAK || cbAttribute != sizeof(BOOL)) {
        return original();
    }
    BOOL cloak = *(BOOL*)pvAttribute;
    if (cloak) {
        return original();
    }
    Wh_Log(L"> %08X", (DWORD)(DWORD_PTR)hwnd);
    DWORD processId = 0;
    if (!hwnd || !GetWindowThreadProcessId(hwnd, &processId)) {
        return original();
    }
    TCHAR className[256];
    GetClassName(hwnd, className, 256);
    std::wstring windowClassName(className);
    std::wstring processFileName = GetProcessFileName(processId);
    Wh_Log(L"process: %s, windowClassName: %s",processFileName.c_str(),windowClassName.c_str());
    enum class DwmTarget {
        StartMenu,
        ShellExperienceHost,
        SearchHost,
    };
    DwmTarget target;
    if (_wcsicmp(processFileName.c_str(), L"StartMenuExperienceHost.exe") == 0) {
        target = DwmTarget::StartMenu;
    } else if (_wcsicmp(processFileName.c_str(), L"SearchHost.exe") == 0) {
        target = DwmTarget::SearchHost;
    }else if (_wcsicmp(processFileName.c_str(), L"ShellExperienceHost.exe") == 0) {
        if (_wcsicmp(windowClassName.c_str(), L"Windows.UI.Core.CoreWindow") != 0 ||
            !IsNotificationCenterShellExperienceHostWindowTai(hwnd)) {
            return original();
        }
        target = DwmTarget::ShellExperienceHost;
    }  else {
        return original();
    }
    HMONITOR monitor = ResolveFlyoutMonitorTai(hwnd);
    UINT monitorDpiX = 96;
    UINT monitorDpiY = 96;
    if (!monitor ||
        FAILED(GetDpiForMonitor(monitor, MDT_DEFAULT, &monitorDpiX,
                                &monitorDpiY)) ||
        monitorDpiX == 0 || monitorDpiY == 0) {
        monitorDpiX = 96;
        monitorDpiY = 96;
    }
    MONITORINFO monitorInfo{
        .cbSize = sizeof(MONITORINFO),
    };
    if (!GetMonitorInfo(monitor, &monitorInfo)) {
    return original();
}
HMONITOR windowMonitor =
    MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
UINT windowDpiX = monitorDpiX;
UINT windowDpiY = monitorDpiY;
if (windowMonitor &&
    (FAILED(GetDpiForMonitor(windowMonitor, MDT_DEFAULT, &windowDpiX,
                             &windowDpiY)) ||
     windowDpiX == 0 || windowDpiY == 0)) {
    windowDpiX = monitorDpiX;
    windowDpiY = monitorDpiY;
}
auto monitorName = GetMonitorName(monitor);
TaskbarFlyoutStateSnapshot taskbarState;
if (!TryGetTaskbarFlyoutStateSnapshot(monitorName, &taskbarState)) {
    return original();
}
    RECT targetRect;
    if (!GetWindowRect(hwnd, &targetRect)) {
        return original();
    }
    int x = targetRect.left;
    int y = targetRect.top;
    int cx = targetRect.right - targetRect.left;
    int cy = targetRect.bottom - targetRect.top;
    float dpiScale = monitorDpiX / 96.0f;
float dpiScaleY = monitorDpiY / 96.0f;
if (windowDpiX != monitorDpiX) {
  cx = MulDiv(cx, monitorDpiX, windowDpiX);
}
if (windowDpiY != monitorDpiY) {
  cy = MulDiv(cy, monitorDpiY, windowDpiY);
}
const bool alignFlyoutInner = GetUserDefinedAlignFlyoutInner();
float absStartX = taskbarState.lastStartButtonXCalculated * dpiScale;
float absRootWidth = taskbarState.lastRootWidth * dpiScale;
float absTargetWidth = taskbarState.lastTargetWidth * dpiScale;
const int monitorLeft = monitorInfo.rcMonitor.left;
int taskbarAlignedY = y;
bool hasTaskbarAlignedY = TryCalculateFlyoutYAboveTaskbar(monitorInfo, cy, dpiScaleY, taskbarAlignedY);
Wh_Log(L"original: monitor=%s left=%d sourceDpi=%ux%u targetDpi=%ux%u taskbarState.lastLeftMostEdgeTray: %f, lastStartButtonXCalculated: %f g_lastRootWidth %f cx: %d, x:%d;cy: %d; y: %d; target:%d g_lastTargetWidth: %f, absStartX: %f; absRootWidth: %f; absTargetWidth: %f", monitorName.c_str(), monitorLeft, windowDpiX, windowDpiY, monitorDpiX, monitorDpiY, taskbarState.lastLeftMostEdgeTray, taskbarState.lastStartButtonXCalculated, taskbarState.lastRootWidth, cx, x, cy, y, target, taskbarState.lastTargetWidth, absStartX, absRootWidth, absTargetWidth);
const int flyoutInnerPaddingPx = GetFlyoutInnerPaddingPx(dpiScale);
if (target == DwmTarget::StartMenu) {
  const int recordedStartMenuWidthDip = Wh_GetIntValue(L"lastRecordedStartMenuWidth", g_lastRecordedStartMenuWidth);
  if (recordedStartMenuWidthDip > 0) {
    g_lastRecordedStartMenuWidth = recordedStartMenuWidthDip;
  }
  const int startMenuWidthPx =
      (g_lastRecordedStartMenuWidth > 0)
          ? static_cast<int>((g_lastRecordedStartMenuWidth * dpiScale) + 0.5f)
          : cx;
  if (g_settings_startbuttonposition.startMenuOnTheLeft && !g_unloading) {
    g_startMenuWnd = hwnd;
    g_startMenuOriginalWidth = cx;
    cx = startMenuWidthPx;
    constexpr int kStartMenuTransparentInsetDip = 12;
    const int startMenuTransparentInsetPx = static_cast<int>(
        (kStartMenuTransparentInsetDip * dpiScale) + 0.5f);
    int localX = static_cast<int>(
        absStartX -
        (alignFlyoutInner ? flyoutInnerPaddingPx
                          : (startMenuWidthPx / 2.0f))) +
        startMenuTransparentInsetPx;
    localX = std::max(
        -startMenuTransparentInsetPx,
        std::min(localX,
                 static_cast<int>(absRootWidth - startMenuWidthPx) +
                     startMenuTransparentInsetPx));
    x = monitorLeft + localX;
    if (hasTaskbarAlignedY) {
      y = taskbarAlignedY;
    }
  } else {
    if (g_startMenuOriginalWidth) {
      cx = g_startMenuOriginalWidth;
    }
    g_startMenuWnd = nullptr;
    g_startMenuOriginalWidth = 0;
    x = monitorLeft;
  }
} else if (target == DwmTarget::SearchHost) {
  if (g_settings_startbuttonposition.startMenuOnTheLeft && !g_unloading) {
    g_searchMenuWnd = hwnd;
    g_searchMenuOriginalX = x;
    int localX = static_cast<int>(absStartX - (alignFlyoutInner ? flyoutInnerPaddingPx : (cx / 2.0f)));
    localX = std::max(0, std::min(localX, static_cast<int>(absRootWidth - cx)));
    x = monitorLeft + localX;
    if (hasTaskbarAlignedY) {
      y = taskbarAlignedY;
    }
  } else {
    x = g_unloading && IsStartMenuOrbLeftAligned()
            ? g_searchMenuOriginalX
            : monitorLeft + static_cast<int>((absRootWidth - cx) / 2);
    g_searchMenuWnd = nullptr;
    g_searchMenuOriginalX = 0;
  }
} else if (target == DwmTarget::ShellExperienceHost) {
  int lastRecordedTrayRightMostEdgeForMonitor = taskbarState.lastRightMostEdgeTray;
  if (y != 0) {
    return original();
  }
  if (g_settings_startbuttonposition.MoveFlyoutNotificationCenter && !g_unloading) {
    int localX = static_cast<int>(lastRecordedTrayRightMostEdgeForMonitor * dpiScale - (alignFlyoutInner ? (cx - flyoutInnerPaddingPx) : (cx / 2.0f)));
    localX = std::max(0, std::min(localX, static_cast<int>(absRootWidth - cx)));
    x = monitorLeft + localX;
  } else {
    x = monitorLeft + static_cast<int>(absRootWidth - cx);
  }
}
Wh_Log(L"Recalc: monitor=%s taskbarState.lastLeftMostEdgeTray: %f, lastStartButtonXCalculated: %f g_lastRootWidth %f cx: %d, x:%d;cy: %d; y: %d; target:%d g_lastTargetWidth: %f, absStartX: %f; absRootWidth: %f; absTargetWidth: %f", monitorName.c_str(), taskbarState.lastLeftMostEdgeTray, taskbarState.lastStartButtonXCalculated, taskbarState.lastRootWidth, cx, x, cy, y, target, taskbarState.lastTargetWidth, absStartX, absRootWidth, absTargetWidth);
SetWindowPos(hwnd, nullptr, x, y, cx, cy, SWP_NOZORDER | SWP_NOACTIVATE);
    return original();
}
namespace StartMenuUI {
bool g_applyStylePending;
bool g_inApplyStyle;
std::optional<double> g_previousCanvasLeft;
winrt::weak_ref<DependencyObject> g_startSizingFrameWeakRef;
int64_t g_canvasTopPropertyChangedToken;
int64_t g_canvasLeftPropertyChangedToken;
std::optional<HorizontalAlignment> g_previousHorizontalAlignment;
winrt::event_token g_layoutUpdatedToken;
winrt::event_token g_visibilityChangedToken;
HWND GetCoreWnd() {
    struct ENUM_WINDOWS_PARAM {
        HWND* hWnd;
    };
    HWND hWnd = nullptr;
    ENUM_WINDOWS_PARAM param = {&hWnd};
    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            ENUM_WINDOWS_PARAM& param = *(ENUM_WINDOWS_PARAM*)lParam;
            DWORD dwProcessId = 0;
            if (!GetWindowThreadProcessId(hWnd, &dwProcessId) ||
                dwProcessId != GetCurrentProcessId()) {
                return TRUE;
            }
            WCHAR szClassName[32];
            if (GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName)) == 0) {
                return TRUE;
            }
            if (_wcsicmp(szClassName, L"Windows.UI.Core.CoreWindow") == 0) {
                *param.hWnd = hWnd;
                return FALSE;
            }
            return TRUE;
        },
        (LPARAM)&param);
    return hWnd;
}
void ApplyStyleClassicStartMenu(FrameworkElement content, HMONITOR monitor) {
    ApplyStyle(content, GetMonitorName(monitor));
}
void ApplyStyleRedesignedStartMenu(FrameworkElement content) {
    FrameworkElement frameRoot = FindChildByName(content, L"FrameRoot");
    if (!frameRoot) {
        Wh_Log(L"Failed to find Start menu frame root");
        return;
    }
    if (g_unloading) {
        frameRoot.HorizontalAlignment(g_previousHorizontalAlignment.value_or(
            HorizontalAlignment::Center));
    } else {
        if (!g_previousHorizontalAlignment) {
            g_previousHorizontalAlignment = frameRoot.HorizontalAlignment();
        }
        frameRoot.HorizontalAlignment(HorizontalAlignment::Center);
    }
}
void ApplyStyle() {
    g_inApplyStyle = true;
    HWND coreWnd = GetCoreWnd();
    HMONITOR monitor = MonitorFromWindow(coreWnd, MONITOR_DEFAULTTONEAREST);
    Wh_Log(L"Applying Start menu style for monitor %p", monitor);
    auto window = Window::Current();
    FrameworkElement content = window.Content().as<FrameworkElement>();
    winrt::hstring contentClassName = winrt::get_class_name(content);
    Wh_Log(L"Start menu content class name: %s", contentClassName.c_str());
    if (contentClassName == L"Windows.UI.Xaml.Controls.Canvas") {
        ApplyStyleClassicStartMenu(content, monitor);
    } else if (contentClassName == L"StartMenu.StartBlendedFlexFrame") {
        ApplyStyleClassicStartMenu(content, monitor);
    } else {
        Wh_Log(L"Error: Unsupported Start menu content class name");
    }
    g_inApplyStyle = false;
}
void Init() {
    if (g_layoutUpdatedToken) {
        return;
    }
    auto window = Window::Current();
    if (!window) {
        return;
    }
    if (!g_visibilityChangedToken) {
        g_visibilityChangedToken = window.VisibilityChanged(
            [](winrt::Windows::Foundation::IInspectable const& sender,
               winrt::Windows::UI::Core::VisibilityChangedEventArgs const&
                   args) {
                Wh_Log(L"Window visibility changed: %d", args.Visible());
                if (args.Visible()) {
                    g_applyStylePending = true;
                }
            });
    }
    auto contentUI = window.Content();
    if (!contentUI) {
        return;
    }
    auto content = contentUI.as<FrameworkElement>();
    g_layoutUpdatedToken = content.LayoutUpdated(
        [](winrt::Windows::Foundation::IInspectable const&,
           winrt::Windows::Foundation::IInspectable const&) {
            if (g_applyStylePending) {
                g_applyStylePending = false;
                ApplyStyle();
            }
        });
    ApplyStyle();
}
void Uninit() {
    if (!g_layoutUpdatedToken) {
        return;
    }
    auto window = Window::Current();
    if (!window) {
        return;
    }
    if (g_visibilityChangedToken) {
        window.VisibilityChanged(g_visibilityChangedToken);
        g_visibilityChangedToken = {};
    }
    auto contentUI = window.Content();
    if (!contentUI) {
        return;
    }
    auto content = contentUI.as<FrameworkElement>();
    content.LayoutUpdated(g_layoutUpdatedToken);
    g_layoutUpdatedToken = {};
    auto startSizingFrameDo = g_startSizingFrameWeakRef.get();
    if (startSizingFrameDo) {
        if (g_canvasTopPropertyChangedToken) {
            startSizingFrameDo.UnregisterPropertyChangedCallback(
                Controls::Canvas::TopProperty(),
                g_canvasTopPropertyChangedToken);
            g_canvasTopPropertyChangedToken = 0;
        }
        if (g_canvasLeftPropertyChangedToken) {
            startSizingFrameDo.UnregisterPropertyChangedCallback(
                Controls::Canvas::LeftProperty(),
                g_canvasLeftPropertyChangedToken);
            g_canvasLeftPropertyChangedToken = 0;
        }
    }
    g_startSizingFrameWeakRef = nullptr;
    ApplyStyle();
}
void SettingsChanged() {
    ApplyStyle();
}
using RoGetActivationFactory_t = decltype(&RoGetActivationFactory);
RoGetActivationFactory_t RoGetActivationFactory_Original;
HRESULT WINAPI RoGetActivationFactory_Hook(HSTRING activatableClassId,
                                           REFIID iid,
                                           void** factory) {
    thread_local static bool isInHook;
    if (isInHook) {
        return RoGetActivationFactory_Original(activatableClassId, iid,
                                               factory);
    }
    isInHook = true;
    if (wcscmp(WindowsGetStringRawBuffer(activatableClassId, nullptr),
               L"Windows.UI.Xaml.Hosting.XamlIsland") == 0) {
        try {
            Init();
        } catch (...) {
            HRESULT hr = winrt::to_hresult();
            Wh_Log(L"Error %08X", hr);
        }
    }
    HRESULT ret =
        RoGetActivationFactory_Original(activatableClassId, iid, factory);
    isInHook = false;
    return ret;
}
}  // namespace StartMenuUI
void RestoreMenuPositions() {
    if (g_searchMenuWnd && g_searchMenuOriginalX) {
        RECT rect;
        if (GetWindowRect(g_searchMenuWnd, &rect)) {
            int x = rect.left;
            int y = rect.top;
            int cx = rect.right - rect.left;
            int cy = rect.bottom - rect.top;
            if (g_searchMenuOriginalX != x) {
                x = g_searchMenuOriginalX;
                SetWindowPos(g_searchMenuWnd, nullptr, x, y, cx, cy,
                             SWP_NOZORDER | SWP_NOACTIVATE);
            }
        }
        g_searchMenuWnd = nullptr;
        g_searchMenuOriginalX = 0;
    }
}
void LoadSettingsStartButtonPosition() {
    g_settings_startbuttonposition.startMenuOnTheLeft = Wh_GetIntSetting(L"MoveFlyoutStartMenu");
g_settings_startbuttonposition.MoveFlyoutNotificationCenter = Wh_GetIntSetting(L"MoveFlyoutNotificationCenter");
}
BOOL Wh_ModInitStartButtonPosition() {
    LoadSettingsStartButtonPosition();
    g_target = Target::Explorer;
    WCHAR moduleFilePath[MAX_PATH];
    switch (
        GetModuleFileName(nullptr, moduleFilePath, ARRAYSIZE(moduleFilePath))) {
        case 0:
        case ARRAYSIZE(moduleFilePath):
            Wh_Log(L"GetModuleFileName failed");
            break;
        default:
            if (PCWSTR moduleFileName = wcsrchr(moduleFilePath, L'\\')) {
                moduleFileName++;
                if (_wcsicmp(moduleFileName, L"StartMenuExperienceHost.exe") ==
                    0) {
                    g_target = Target::StartMenuExperienceHost;
                }
            } else {
                Wh_Log(L"GetModuleFileName returned an unsupported path");
            }
            break;
    }
    if (g_target == Target::StartMenuExperienceHost) {
        if (!g_settings_startbuttonposition.startMenuOnTheLeft) {
            return FALSE;
        }
        HMODULE winrtModule =
            GetModuleHandle(L"api-ms-win-core-winrt-l1-1-0.dll");
        auto pRoGetActivationFactory =
            (decltype(&RoGetActivationFactory))GetProcAddress(
                winrtModule, "RoGetActivationFactory");
        WindhawkUtils::SetFunctionHook(
            pRoGetActivationFactory, StartMenuUI::RoGetActivationFactory_Hook,
            &StartMenuUI::RoGetActivationFactory_Original);
        return TRUE;
    }
    if (!HookTaskbarDllSymbolsStartButtonPosition()) {
        return FALSE;
    }
    if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
        g_taskbarViewDllLoadedStartButtonPosition = true;
        if (!HookTaskbarViewDllSymbolsStartButtonPosition(taskbarViewModule)) {
            return FALSE;
        }
    } else {
        Wh_Log(L"Taskbar view module not loaded yet");
        HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
        auto pKernelBaseLoadLibraryExW =
            (decltype(&LoadLibraryExW))GetProcAddress(kernelBaseModule,
                                                      "LoadLibraryExW");
        WindhawkUtils::SetFunctionHook(pKernelBaseLoadLibraryExW,
                                       LoadLibraryExW_Hook_StartButtonPosition,
                                       &LoadLibraryExW_Original);
    }
    HMODULE dwmapiModule =
        LoadLibraryEx(L"dwmapi.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (dwmapiModule) {
        auto pDwmSetWindowAttribute =
            (decltype(&DwmSetWindowAttribute))GetProcAddress(
                dwmapiModule, "DwmSetWindowAttribute");
        if (pDwmSetWindowAttribute) {
            WindhawkUtils::SetFunctionHook(pDwmSetWindowAttribute,
                                           DwmSetWindowAttribute_Hook,
                                           &DwmSetWindowAttribute_Original);
        }
    }
    return TRUE;
}
void Wh_ModAfterInitStartButtonPosition() {
    if (g_target == Target::Explorer) {
        if (!g_taskbarViewDllLoadedStartButtonPosition) {
            if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
                if (!g_taskbarViewDllLoadedStartButtonPosition.exchange(true)) {
                    Wh_Log(L"Got Taskbar.View.dll");
                    if (HookTaskbarViewDllSymbolsStartButtonPosition(taskbarViewModule)) {
                        Wh_ApplyHookOperations();
                    }
                }
            }
        }
        HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
        if (hTaskbarWnd) {
            ApplySettingsStartButtonPosition(hTaskbarWnd);
        }
    } else if (g_target == Target::StartMenuExperienceHost) {
        HWND hCoreWnd = StartMenuUI::GetCoreWnd();
        if (hCoreWnd) {
            Wh_Log(L"Initializing - Found core window");
            RunFromWindowThread(
                hCoreWnd, [](PVOID) { StartMenuUI::Init(); }, nullptr);
        }
    }
}
void Wh_ModBeforeUninitStartButtonPosition() {
    g_unloading = true;
    if (g_target == Target::Explorer) {
        HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
        if (hTaskbarWnd) {
            ApplySettingsStartButtonPosition(hTaskbarWnd);
        }
    } else if (g_target == Target::StartMenuExperienceHost) {
        HWND hCoreWnd = StartMenuUI::GetCoreWnd();
        if (hCoreWnd) {
            Wh_Log(L"Uninitializing - Found core window");
            RunFromWindowThread(
                hCoreWnd, [](PVOID) { StartMenuUI::Uninit(); }, nullptr);
        }
    }
}
void Wh_ModUninitStartButtonPosition() {if(true)return;
    if (g_target == Target::Explorer) {
        RestoreMenuPositions();
    }
}
BOOL Wh_ModSettingsChangedStartButtonPosition() {
    if (g_target == Target::Explorer) {
        RestoreMenuPositions();
    }
    LoadSettingsStartButtonPosition();
    if (g_target == Target::StartMenuExperienceHost) {
        if (!g_settings_startbuttonposition.startMenuOnTheLeft) {
            return FALSE;
        }
        HWND hCoreWnd = StartMenuUI::GetCoreWnd();
        if (hCoreWnd) {
            Wh_Log(L"Applying settings - Found core window");
            RunFromWindowThread(
                hCoreWnd, [](PVOID) { StartMenuUI::SettingsChanged(); },
                nullptr);
        }
    }
    return TRUE;
}
// ---- WindhawkBlur support ---------------------------------------------------
// Based on the WindhawkBlur/XamlBlurBrush approach used by Windows 11 Taskbar Styler.
// Yoinked from ramensoftware windows-11-taskbar-styler
// which is yoinked from https://github.com/TranslucentTB/TranslucentTB/blob/release/ExplorerTAP/XamlBlurBrush.cpp
#include <dwmapi.h>
#include <chrono>
#include <string>
#include <regex>
#include <sstream>
#include <algorithm>
#include <unordered_map>
#include <limits>
#include <utility>
#include <windhawk_api.h>
#include <windhawk_utils.h>
#include <functional>
#undef GetCurrentTime
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Text.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Data.h>
#include <winrt/Windows.UI.Xaml.Hosting.h>
#include <winrt/Windows.UI.Xaml.Markup.h>
#include <winrt/Windows.UI.Xaml.Media.Animation.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/base.h>
#include <commctrl.h>
#include <roapi.h>
#include <winstring.h>
#include <string_view>
#include <vector>
#include <algorithm>
#include <atomic>
#include <cmath>
#include <winrt/Windows.Graphics.Imaging.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.Storage.h>
#include <winrt/Windows.UI.Xaml.Media.Imaging.h>
#include <winrt/Windows.Storage.Search.h>
#include <chrono>
#include <thread>
#include <windows.h>
#include <psapi.h>
#include <winrt/Windows.UI.Xaml.Shapes.h>
#include <mutex>
#include <cmath>
#include <cwctype>
using namespace winrt::Windows::UI::Xaml;
#include <cmath>
#include <d2d1_1.h>
#include <d2d1effects.h>
#include <list>
#include <winrt/Windows.Graphics.Effects.h>
#include <winrt/Windows.System.Power.h>
#include <winrt/Windows.UI.ViewManagement.h>
namespace wf = winrt::Windows::Foundation;
namespace wge = winrt::Windows::Graphics::Effects;
namespace wuc = winrt::Windows::UI::Composition;
namespace wuxh = winrt::Windows::UI::Xaml::Hosting;
#ifndef BUILD_WINDOWS
namespace ABI {
#endif
namespace Windows {
namespace Graphics {
namespace Effects {
typedef interface IGraphicsEffectSource IGraphicsEffectSource;
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
DECLARE_INTERFACE_IID_(IGraphicsEffectD2D1Interop, IUnknown, "2FC57384-A068-44D7-A331-30982FCF7177") {
  STDMETHOD(GetEffectId)(_Out_ GUID* id) PURE;
  STDMETHOD(GetNamedPropertyMapping)(LPCWSTR name, _Out_ UINT* index, _Out_ GRAPHICS_EFFECT_PROPERTY_MAPPING* mapping) PURE;
  STDMETHOD(GetPropertyCount)(_Out_ UINT* count) PURE;
  STDMETHOD(GetProperty)(UINT index, _Outptr_ winrt::impl::abi_t<wf::IPropertyValue>** value) PURE;
  STDMETHOD(GetSource)(UINT index, _Outptr_ IGraphicsEffectSource** source) PURE;
  STDMETHOD(GetSourceCount)(_Out_ UINT* count) PURE;
};
}  // namespace Effects
}  // namespace Graphics
}  // namespace Windows
#ifndef BUILD_WINDOWS
}  // namespace ABI
#endif
template <>
inline constexpr winrt::guid winrt::impl::guid_v<ABI::Windows::Graphics::Effects::IGraphicsEffectSource>{
    winrt::impl::guid_v<wge::IGraphicsEffectSource>};
template <>
inline constexpr winrt::guid winrt::impl::guid_v<ABI::Windows::Graphics::Effects::IGraphicsEffectD2D1Interop>{
    0x2FC57384, 0xA068, 0x44D7, {0xA3, 0x31, 0x30, 0x98, 0x2F, 0xCF, 0x71, 0x77}};
namespace awge = ABI::Windows::Graphics::Effects;
inline void CopyWindhawkBlurPropertyValueToAbi(wf::IPropertyValue const& propertyValue, winrt::impl::abi_t<wf::IPropertyValue>** value) {
  winrt::copy_to_abi(propertyValue, *reinterpret_cast<void**>(value));
}
typedef enum MY_D2D1_GAUSSIANBLUR_OPTIMIZATION {
  MY_D2D1_GAUSSIANBLUR_OPTIMIZATION_SPEED = 0,
  MY_D2D1_GAUSSIANBLUR_OPTIMIZATION_BALANCED = 1,
  MY_D2D1_GAUSSIANBLUR_OPTIMIZATION_QUALITY = 2,
  MY_D2D1_GAUSSIANBLUR_OPTIMIZATION_FORCE_DWORD = 0xffffffff
} MY_D2D1_GAUSSIANBLUR_OPTIMIZATION;
struct CompositeEffect : winrt::implements<CompositeEffect, wge::IGraphicsEffect, wge::IGraphicsEffectSource, awge::IGraphicsEffectD2D1Interop> {
  std::vector<wge::IGraphicsEffectSource> Sources;
  D2D1_COMPOSITE_MODE Mode = D2D1_COMPOSITE_MODE_SOURCE_OVER;
  winrt::hstring m_name = L"CompositeEffect";
  HRESULT STDMETHODCALLTYPE GetEffectId(GUID* id) noexcept override { if (!id) return E_INVALIDARG; *id = CLSID_D2D1Composite; return S_OK; }
  HRESULT STDMETHODCALLTYPE GetNamedPropertyMapping(LPCWSTR name, UINT* index, awge::GRAPHICS_EFFECT_PROPERTY_MAPPING* mapping) noexcept override {
    if (!index || !mapping) return E_INVALIDARG;
    if (std::wstring_view(name) == L"Mode") { *index = D2D1_COMPOSITE_PROP_MODE; *mapping = awge::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT; return S_OK; }
    return E_INVALIDARG;
  }
  HRESULT STDMETHODCALLTYPE GetPropertyCount(UINT* count) noexcept override { if (!count) return E_INVALIDARG; *count = 1; return S_OK; }
  HRESULT STDMETHODCALLTYPE GetProperty(UINT index, winrt::impl::abi_t<wf::IPropertyValue>** value) noexcept override try {
    if (!value) return E_INVALIDARG;
    if (index != D2D1_COMPOSITE_PROP_MODE) return E_BOUNDS;
    CopyWindhawkBlurPropertyValueToAbi(wf::PropertyValue::CreateUInt32(static_cast<UINT32>(Mode)).as<wf::IPropertyValue>(), value);
    return S_OK;
  } catch (...) { return winrt::to_hresult(); }
  HRESULT STDMETHODCALLTYPE GetSource(UINT index, awge::IGraphicsEffectSource** source) noexcept override try {
    if (!source) return E_INVALIDARG;
    winrt::copy_to_abi(Sources.at(index), *reinterpret_cast<void**>(source));
    return S_OK;
  } catch (...) { return winrt::to_hresult(); }
  HRESULT STDMETHODCALLTYPE GetSourceCount(UINT* count) noexcept override { if (!count) return E_INVALIDARG; *count = static_cast<UINT>(Sources.size()); return S_OK; }
  winrt::hstring Name() { return m_name; }
  void Name(winrt::hstring value) { m_name = std::move(value); }
};
struct FloodEffect : winrt::implements<FloodEffect, wge::IGraphicsEffect, wge::IGraphicsEffectSource, awge::IGraphicsEffectD2D1Interop> {
  winrt::Windows::UI::Color Color{};
  winrt::hstring m_name = L"FloodEffect";
  HRESULT STDMETHODCALLTYPE GetEffectId(GUID* id) noexcept override { if (!id) return E_INVALIDARG; *id = CLSID_D2D1Flood; return S_OK; }
  HRESULT STDMETHODCALLTYPE GetNamedPropertyMapping(LPCWSTR name, UINT* index, awge::GRAPHICS_EFFECT_PROPERTY_MAPPING* mapping) noexcept override {
    if (!index || !mapping) return E_INVALIDARG;
    if (std::wstring_view(name) == L"Color") { *index = D2D1_FLOOD_PROP_COLOR; *mapping = awge::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT; return S_OK; }
    return E_INVALIDARG;
  }
  HRESULT STDMETHODCALLTYPE GetPropertyCount(UINT* count) noexcept override { if (!count) return E_INVALIDARG; *count = 1; return S_OK; }
  HRESULT STDMETHODCALLTYPE GetProperty(UINT index, winrt::impl::abi_t<wf::IPropertyValue>** value) noexcept override try {
    if (!value) return E_INVALIDARG;
    if (index != D2D1_FLOOD_PROP_COLOR) return E_BOUNDS;
    float rgba[] = {Color.R / 255.0f, Color.G / 255.0f, Color.B / 255.0f, Color.A / 255.0f};
    CopyWindhawkBlurPropertyValueToAbi(wf::PropertyValue::CreateSingleArray(winrt::array_view<float const>(rgba, rgba + 4)).as<wf::IPropertyValue>(), value);
    return S_OK;
  } catch (...) { return winrt::to_hresult(); }
  HRESULT STDMETHODCALLTYPE GetSource(UINT, awge::IGraphicsEffectSource** source) noexcept override { if (!source) return E_INVALIDARG; return E_BOUNDS; }
  HRESULT STDMETHODCALLTYPE GetSourceCount(UINT* count) noexcept override { if (!count) return E_INVALIDARG; *count = 0; return S_OK; }
  winrt::hstring Name() { return m_name; }
  void Name(winrt::hstring value) { m_name = std::move(value); }
};
struct GaussianBlurEffect : winrt::implements<GaussianBlurEffect, wge::IGraphicsEffect, wge::IGraphicsEffectSource, awge::IGraphicsEffectD2D1Interop> {
  wge::IGraphicsEffectSource Source{nullptr};
  float BlurAmount = 30.0f;
  MY_D2D1_GAUSSIANBLUR_OPTIMIZATION Optimization = MY_D2D1_GAUSSIANBLUR_OPTIMIZATION_BALANCED;
  D2D1_BORDER_MODE BorderMode = D2D1_BORDER_MODE_SOFT;
  winrt::hstring m_name = L"GaussianBlurEffect";
  HRESULT STDMETHODCALLTYPE GetEffectId(GUID* id) noexcept override { if (!id) return E_INVALIDARG; *id = CLSID_D2D1GaussianBlur; return S_OK; }
  HRESULT STDMETHODCALLTYPE GetNamedPropertyMapping(LPCWSTR name, UINT* index, awge::GRAPHICS_EFFECT_PROPERTY_MAPPING* mapping) noexcept override {
    if (!index || !mapping) return E_INVALIDARG;
    const std::wstring_view n(name);
    if (n == L"BlurAmount") { *index = D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION; *mapping = awge::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT; return S_OK; }
    if (n == L"Optimization") { *index = D2D1_GAUSSIANBLUR_PROP_OPTIMIZATION; *mapping = awge::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT; return S_OK; }
    if (n == L"BorderMode") { *index = D2D1_GAUSSIANBLUR_PROP_BORDER_MODE; *mapping = awge::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT; return S_OK; }
    return E_INVALIDARG;
  }
  HRESULT STDMETHODCALLTYPE GetPropertyCount(UINT* count) noexcept override { if (!count) return E_INVALIDARG; *count = 3; return S_OK; }
  HRESULT STDMETHODCALLTYPE GetProperty(UINT index, winrt::impl::abi_t<wf::IPropertyValue>** value) noexcept override try {
    if (!value) return E_INVALIDARG;
    switch (index) {
      case D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION: CopyWindhawkBlurPropertyValueToAbi(wf::PropertyValue::CreateSingle(BlurAmount).as<wf::IPropertyValue>(), value); break;
      case D2D1_GAUSSIANBLUR_PROP_OPTIMIZATION: CopyWindhawkBlurPropertyValueToAbi(wf::PropertyValue::CreateUInt32(static_cast<UINT32>(Optimization)).as<wf::IPropertyValue>(), value); break;
      case D2D1_GAUSSIANBLUR_PROP_BORDER_MODE: CopyWindhawkBlurPropertyValueToAbi(wf::PropertyValue::CreateUInt32(static_cast<UINT32>(BorderMode)).as<wf::IPropertyValue>(), value); break;
      default: return E_BOUNDS;
    }
    return S_OK;
  } catch (...) { return winrt::to_hresult(); }
  HRESULT STDMETHODCALLTYPE GetSource(UINT index, awge::IGraphicsEffectSource** source) noexcept override {
    if (!source) return E_INVALIDARG;
    if (index == 0 && Source) { winrt::copy_to_abi(Source, *reinterpret_cast<void**>(source)); return S_OK; }
    return E_BOUNDS;
  }
  HRESULT STDMETHODCALLTYPE GetSourceCount(UINT* count) noexcept override { if (!count) return E_INVALIDARG; *count = 1; return S_OK; }
  winrt::hstring Name() { return m_name; }
  void Name(winrt::hstring value) { m_name = std::move(value); }
};
struct ColorMatrixEffect : winrt::implements<ColorMatrixEffect, wge::IGraphicsEffect, wge::IGraphicsEffectSource, awge::IGraphicsEffectD2D1Interop> {
  wge::IGraphicsEffectSource Source{nullptr};
  float Matrix[20] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0};
  uint32_t AlphaMode = D2D1_COLORMATRIX_ALPHA_MODE_PREMULTIPLIED;
  bool ClampOutput = false;
  winrt::hstring m_name = L"ColorMatrixEffect";
  HRESULT STDMETHODCALLTYPE GetEffectId(GUID* id) noexcept override { if (!id) return E_INVALIDARG; *id = CLSID_D2D1ColorMatrix; return S_OK; }
  HRESULT STDMETHODCALLTYPE GetNamedPropertyMapping(LPCWSTR name, UINT* index, awge::GRAPHICS_EFFECT_PROPERTY_MAPPING* mapping) noexcept override {
    if (!index || !mapping) return E_INVALIDARG;
    const std::wstring_view n(name);
    if (n == L"ColorMatrix") { *index = D2D1_COLORMATRIX_PROP_COLOR_MATRIX; *mapping = awge::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT; return S_OK; }
    if (n == L"AlphaMode") { *index = D2D1_COLORMATRIX_PROP_ALPHA_MODE; *mapping = awge::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT; return S_OK; }
    if (n == L"ClampOutput") { *index = D2D1_COLORMATRIX_PROP_CLAMP_OUTPUT; *mapping = awge::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT; return S_OK; }
    return E_INVALIDARG;
  }
  HRESULT STDMETHODCALLTYPE GetPropertyCount(UINT* count) noexcept override { if (!count) return E_INVALIDARG; *count = 3; return S_OK; }
  HRESULT STDMETHODCALLTYPE GetProperty(UINT index, winrt::impl::abi_t<wf::IPropertyValue>** value) noexcept override try {
    if (!value) return E_INVALIDARG;
    switch (index) {
      case D2D1_COLORMATRIX_PROP_COLOR_MATRIX: CopyWindhawkBlurPropertyValueToAbi(wf::PropertyValue::CreateSingleArray(winrt::array_view<float const>(Matrix, Matrix + 20)).as<wf::IPropertyValue>(), value); break;
      case D2D1_COLORMATRIX_PROP_ALPHA_MODE: CopyWindhawkBlurPropertyValueToAbi(wf::PropertyValue::CreateUInt32(AlphaMode).as<wf::IPropertyValue>(), value); break;
      case D2D1_COLORMATRIX_PROP_CLAMP_OUTPUT: CopyWindhawkBlurPropertyValueToAbi(wf::PropertyValue::CreateBoolean(ClampOutput).as<wf::IPropertyValue>(), value); break;
      default: return E_BOUNDS;
    }
    return S_OK;
  } catch (...) { return winrt::to_hresult(); }
  HRESULT STDMETHODCALLTYPE GetSource(UINT index, awge::IGraphicsEffectSource** source) noexcept override {
    if (!source) return E_INVALIDARG;
    if (index == 0 && Source) { winrt::copy_to_abi(Source, *reinterpret_cast<void**>(source)); return S_OK; }
    return E_BOUNDS;
  }
  HRESULT STDMETHODCALLTYPE GetSourceCount(UINT* count) noexcept override { if (!count) return E_INVALIDARG; *count = 1; return S_OK; }
  winrt::hstring Name() { return m_name; }
  void Name(winrt::hstring value) { m_name = std::move(value); }
};
class XamlBlurBrush : public Media::XamlCompositionBrushBaseT<XamlBlurBrush> {
 public:
  XamlBlurBrush(UIElement element,
                float blurAmount,
                winrt::Windows::UI::Color tint,
                std::optional<uint8_t> tintOpacity,
                winrt::hstring tintThemeResourceKey,
                std::optional<float> tintLuminosityOpacity,
                std::optional<float> tintSaturation,
                std::optional<float> inversionAmount,
                std::optional<winrt::Windows::UI::Color> fallbackColor,
                winrt::hstring fallbackThemeResourceKey)
      : m_compositor(wuxh::ElementCompositionPreview::GetElementVisual(element).Compositor()),
        m_blurAmount(blurAmount),
        m_tint(tint),
        m_tintOpacity(tintOpacity),
        m_tintThemeResourceKey(std::move(tintThemeResourceKey)),
        m_tintLuminosityOpacity(tintLuminosityOpacity),
        m_tintSaturation(tintSaturation),
        m_inversionAmount(inversionAmount),
        m_fallbackColor(fallbackColor),
        m_fallbackThemeResourceKey(std::move(fallbackThemeResourceKey)) {
    auto fe = element.try_as<FrameworkElement>();
    auto createProxy = [&](winrt::hstring const& themeResourceKey) -> Media::SolidColorBrush {
      if (!fe || themeResourceKey.empty()) return nullptr;
      std::wstring xaml = LR"(<SolidColorBrush xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation" Color="{ThemeResource )";
      xaml += std::wstring(themeResourceKey.c_str());
      xaml += LR"(}"/>)";
      try {
        return Markup::XamlReader::Load(winrt::hstring(xaml)).try_as<Media::SolidColorBrush>();
      } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"Failed to create WindhawkBlur theme proxy brush %08X", ex.code());
        return nullptr;
      }
    };
    static std::atomic<unsigned int> s_proxyCounter{0};
    if (!m_tintThemeResourceKey.empty()) {
      if (auto proxyBrush = createProxy(m_tintThemeResourceKey)) {
        auto proxyKey = winrt::hstring(L"__WinDockWindhawkBlurTint_" + std::to_wstring(++s_proxyCounter));
        fe.Resources().Insert(winrt::box_value(proxyKey), proxyBrush);
        m_proxyBrush = proxyBrush;
        m_weakProxyElement = winrt::make_weak(fe);
        m_proxyKey = proxyKey;
        m_proxyBrushColorChangedToken = m_proxyBrush.RegisterPropertyChangedCallback(
            Media::SolidColorBrush::ColorProperty(),
            [weakThis = get_weak()](auto&&, auto&&) {
              if (g_unloading) return;
              if (auto self = weakThis.get()) self->RefreshBrush();
            });
      }
    }
    if (!m_fallbackThemeResourceKey.empty()) {
      if (auto proxyBrush = createProxy(m_fallbackThemeResourceKey)) {
        auto proxyKey = winrt::hstring(L"__WinDockWindhawkBlurFallback_" + std::to_wstring(++s_proxyCounter));
        fe.Resources().Insert(winrt::box_value(proxyKey), proxyBrush);
        m_fallbackProxyBrush = proxyBrush;
        if (!m_weakProxyElement.get()) m_weakProxyElement = winrt::make_weak(fe);
        m_fallbackProxyKey = proxyKey;
        m_fallbackProxyBrushColorChangedToken = m_fallbackProxyBrush.RegisterPropertyChangedCallback(
            Media::SolidColorBrush::ColorProperty(),
            [weakThis = get_weak()](auto&&, auto&&) {
              if (g_unloading) return;
              if (auto self = weakThis.get()) self->RefreshBrush();
            });
      }
    }
    if (m_fallbackColor || !m_fallbackThemeResourceKey.empty()) {
      m_dispatcher = winrt::Windows::System::DispatcherQueue::GetForCurrentThread();
      try {
        m_uiSettings = winrt::Windows::UI::ViewManagement::UISettings();
        auto dispatcher = m_dispatcher;
        m_advancedEffectsEnabledChangedToken = m_uiSettings.AdvancedEffectsEnabledChanged(
            [weakThis = get_weak(), dispatcher](auto&&, auto&&) {
              if (g_unloading || !dispatcher) return;
              dispatcher.TryEnqueue([weakThis] {
                if (g_unloading) return;
                if (auto self = weakThis.get()) self->RefreshBrush();
              });
            });
        m_energySaverStatusChangedToken =
            winrt::Windows::System::Power::PowerManager::EnergySaverStatusChanged(
                [weakThis = get_weak(), dispatcher](auto&&, auto&&) {
                  if (g_unloading || !dispatcher) return;
                  dispatcher.TryEnqueue([weakThis] {
                    if (g_unloading) return;
                    if (auto self = weakThis.get()) self->RefreshBrush();
                  });
                });
      } catch (...) {
        Wh_Log(L"Failed to register WindhawkBlur fallback listeners: %08X", winrt::to_hresult());
      }
      LONG regStatus = RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\Control\\Power", 0, KEY_NOTIFY, &m_powerKey);
      if (regStatus == ERROR_SUCCESS) {
        m_regNotifyEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
        if (m_regNotifyEvent) {
          regStatus = RegNotifyChangeKeyValue(m_powerKey, FALSE, REG_NOTIFY_CHANGE_LAST_SET, m_regNotifyEvent, TRUE);
          if (regStatus == ERROR_SUCCESS) {
            if (!RegisterWaitForSingleObject(&m_regWaitHandle, m_regNotifyEvent, OnEnergySaverRegistryChanged, this, INFINITE, WT_EXECUTEINWAITTHREAD)) {
              Wh_Log(L"RegisterWaitForSingleObject failed: %lu", GetLastError());
              m_regWaitHandle = nullptr;
            }
          }
        }
      }
    }
  }
  ~XamlBlurBrush() {
    if (m_regWaitHandle) {
      UnregisterWaitEx(m_regWaitHandle, INVALID_HANDLE_VALUE);
      m_regWaitHandle = nullptr;
    }
    if (m_regNotifyEvent) {
      CloseHandle(m_regNotifyEvent);
      m_regNotifyEvent = nullptr;
    }
    if (m_powerKey) {
      RegCloseKey(m_powerKey);
      m_powerKey = nullptr;
    }
    if (m_proxyBrush && m_proxyBrushColorChangedToken) {
      try {
        m_proxyBrush.UnregisterPropertyChangedCallback(
            Media::SolidColorBrush::ColorProperty(),
            m_proxyBrushColorChangedToken);
      } catch (...) {}
      m_proxyBrushColorChangedToken = 0;
    }
    if (m_fallbackProxyBrush && m_fallbackProxyBrushColorChangedToken) {
      try {
        m_fallbackProxyBrush.UnregisterPropertyChangedCallback(
            Media::SolidColorBrush::ColorProperty(),
            m_fallbackProxyBrushColorChangedToken);
      } catch (...) {}
      m_fallbackProxyBrushColorChangedToken = 0;
    }
    if (m_uiSettings && m_advancedEffectsEnabledChangedToken.value) {
      try { m_uiSettings.AdvancedEffectsEnabledChanged(m_advancedEffectsEnabledChangedToken); } catch (...) {}
      m_advancedEffectsEnabledChangedToken = {};
    }
    if (m_energySaverStatusChangedToken.value) {
      try { winrt::Windows::System::Power::PowerManager::EnergySaverStatusChanged(m_energySaverStatusChangedToken); } catch (...) {}
      m_energySaverStatusChangedToken = {};
    }
    if (auto element = m_weakProxyElement.get()) {
      try {
        if (!m_proxyKey.empty()) element.Resources().Remove(winrt::box_value(m_proxyKey));
        if (!m_fallbackProxyKey.empty()) element.Resources().Remove(winrt::box_value(m_fallbackProxyKey));
      } catch (...) {}
    }
    m_proxyBrush = nullptr;
    m_fallbackProxyBrush = nullptr;
    m_weakProxyElement = nullptr;
    m_dispatcher = nullptr;
  }
  void OnConnected() {
    if (!CompositionBrush()) {
      RefreshThemeTint();
      RefreshFallbackColor();
      CompositionBrush(ShouldUseFallback() ? CreateFallbackBrush() : CreateEffectBrush());
    }
  }
  void OnDisconnected() {
    if (const auto brush = CompositionBrush()) {
      brush.Close();
      CompositionBrush(nullptr);
    }
  }
 private:
  static void CALLBACK OnEnergySaverRegistryChanged(PVOID context, BOOLEAN) {
    auto* self = static_cast<XamlBlurBrush*>(context);
    if (g_unloading) return;
    if (self->m_powerKey && self->m_regNotifyEvent) {
      RegNotifyChangeKeyValue(self->m_powerKey, FALSE, REG_NOTIFY_CHANGE_LAST_SET, self->m_regNotifyEvent, TRUE);
    }
    if (self->m_dispatcher) {
      auto weakThis = self->get_weak();
      self->m_dispatcher.TryEnqueue([weakThis] {
        if (g_unloading) return;
        if (auto self = weakThis.get()) self->RefreshBrush();
      });
    }
  }
  void RefreshThemeTint() {
    if (!m_proxyBrush) return;
    m_tint = m_proxyBrush.Color();
    if (m_tintOpacity) m_tint.A = *m_tintOpacity;
  }
  void RefreshFallbackColor() {
    if (!m_fallbackProxyBrush) return;
    m_fallbackColor = m_fallbackProxyBrush.Color();
  }
  bool ShouldUseFallback() const {
    if (!m_fallbackColor && m_fallbackThemeResourceKey.empty()) return false;
    bool energySaverActive = false;
    HKEY key{};
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\Control\\Power", 0, KEY_QUERY_VALUE, &key) == ERROR_SUCCESS) {
      DWORD value = 0, type = 0, size = sizeof(value);
      if (RegQueryValueExW(key, L"EnergySaverState", nullptr, &type, reinterpret_cast<LPBYTE>(&value), &size) == ERROR_SUCCESS && type == REG_DWORD) {
        energySaverActive = (value == 1);
      }
      RegCloseKey(key);
    }
    if (!energySaverActive) {
      SYSTEM_POWER_STATUS powerStatus{};
      if (GetSystemPowerStatus(&powerStatus) && powerStatus.SystemStatusFlag != 0) energySaverActive = true;
    }
    bool advancedEffectsOff = false;
    if (m_uiSettings) {
      try { advancedEffectsOff = !m_uiSettings.AdvancedEffectsEnabled(); } catch (...) {}
    }
    return energySaverActive || advancedEffectsOff;
  }
  void RefreshBrush() {
    if (const auto brush = CompositionBrush()) {
      brush.Close();
      CompositionBrush(nullptr);
      OnConnected();
    }
  }
  wuc::CompositionBrush CreateFallbackBrush() {
    return m_compositor.CreateColorBrush(m_fallbackColor.value_or(m_tint));
  }
  wuc::CompositionBrush CreateEffectBrush() {
    auto backdropBrush = m_compositor.CreateBackdropBrush();
    constexpr float kLumaR = 0.2126f;
    constexpr float kLumaG = 0.7152f;
    constexpr float kLumaB = 0.0722f;
    auto blurEffect = winrt::make_self<GaussianBlurEffect>();
    blurEffect->Source = wuc::CompositionEffectSourceParameter(L"backdrop");
    blurEffect->BlurAmount = m_blurAmount;
    blurEffect->Name(L"BlurEffect");
    wge::IGraphicsEffectSource topOfStack = *blurEffect;
    if (m_tintSaturation && *m_tintSaturation != 1.0f) {
      float s = std::max(*m_tintSaturation, 0.0f);
      float invS = 1.0f - s;
      auto satMatrix = winrt::make_self<ColorMatrixEffect>();
      satMatrix->Source = topOfStack;
      auto& m = satMatrix->Matrix;
      m[0] = invS * kLumaR + s; m[1] = invS * kLumaR;     m[2] = invS * kLumaR;     m[3] = 0.0f;
      m[4] = invS * kLumaG;     m[5] = invS * kLumaG + s; m[6] = invS * kLumaG;     m[7] = 0.0f;
      m[8] = invS * kLumaB;     m[9] = invS * kLumaB;     m[10] = invS * kLumaB + s; m[11] = 0.0f;
      m[12] = 0.0f;             m[13] = 0.0f;             m[14] = 0.0f;             m[15] = 1.0f;
      satMatrix->Name(L"SaturationEffect");
      topOfStack = *satMatrix;
    }
    if (m_tintLuminosityOpacity && *m_tintLuminosityOpacity > 0.0f) {
      float op = std::clamp(*m_tintLuminosityOpacity, 0.0f, 1.0f);
      float tintLum = (m_tint.R / 255.0f) * kLumaR + (m_tint.G / 255.0f) * kLumaG + (m_tint.B / 255.0f) * kLumaB;
      auto lumMatrix = winrt::make_self<ColorMatrixEffect>();
      lumMatrix->Source = topOfStack;
      auto& m = lumMatrix->Matrix;
      m[0] = 1.0f - (kLumaR * op); m[1] = -(kLumaR * op);       m[2] = -(kLumaR * op);       m[3] = 0.0f;
      m[4] = -(kLumaG * op);       m[5] = 1.0f - (kLumaG * op); m[6] = -(kLumaG * op);       m[7] = 0.0f;
      m[8] = -(kLumaB * op);       m[9] = -(kLumaB * op);       m[10] = 1.0f - (kLumaB * op); m[11] = 0.0f;
      m[12] = 0.0f;                m[13] = 0.0f;                m[14] = 0.0f;                m[15] = 1.0f;
      m[16] = tintLum * op;        m[17] = tintLum * op;        m[18] = tintLum * op;        m[19] = 0.0f;
      lumMatrix->Name(L"LuminosityBlend");
      topOfStack = *lumMatrix;
    }
    if (m_inversionAmount && *m_inversionAmount > 0.0f) {
      float inv = std::clamp(*m_inversionAmount, 0.0f, 1.0f);
      float scale = 1.0f - (2.0f * inv);
      auto inversionMatrix = winrt::make_self<ColorMatrixEffect>();
      inversionMatrix->Source = topOfStack;
      auto& m = inversionMatrix->Matrix;
      m[0] = scale; m[1] = 0.0f;  m[2] = 0.0f;  m[3] = 0.0f;
      m[4] = 0.0f;  m[5] = scale; m[6] = 0.0f;  m[7] = 0.0f;
      m[8] = 0.0f;  m[9] = 0.0f;  m[10] = scale; m[11] = 0.0f;
      m[12] = 0.0f; m[13] = 0.0f; m[14] = 0.0f; m[15] = 1.0f;
      m[16] = inv;  m[17] = inv;  m[18] = inv;  m[19] = 0.0f;
      inversionMatrix->ClampOutput = true;
      inversionMatrix->Name(L"InversionEffect");
      topOfStack = *inversionMatrix;
    }
    auto floodEffect = winrt::make_self<FloodEffect>();
    floodEffect->Color = m_tint;
    floodEffect->Name(L"FloodEffect");
    auto compositeEffect = winrt::make_self<CompositeEffect>();
    compositeEffect->Mode = D2D1_COMPOSITE_MODE_SOURCE_OVER;
    compositeEffect->Sources.push_back(topOfStack);
    compositeEffect->Sources.push_back(*floodEffect);
    auto factory = m_compositor.CreateEffectFactory(*compositeEffect);
    auto brush = factory.CreateBrush();
    brush.SetSourceParameter(L"backdrop", backdropBrush);
    return brush;
  }
  wuc::Compositor m_compositor{nullptr};
  float m_blurAmount = 30.0f;
  winrt::Windows::UI::Color m_tint{};
  std::optional<uint8_t> m_tintOpacity;
  winrt::hstring m_tintThemeResourceKey;
  std::optional<float> m_tintLuminosityOpacity;
  std::optional<float> m_tintSaturation;
  std::optional<float> m_inversionAmount;
  std::optional<winrt::Windows::UI::Color> m_fallbackColor;
  winrt::hstring m_fallbackThemeResourceKey;
  Media::SolidColorBrush m_proxyBrush{nullptr};
  Media::SolidColorBrush m_fallbackProxyBrush{nullptr};
  winrt::weak_ref<FrameworkElement> m_weakProxyElement;
  winrt::hstring m_proxyKey;
  winrt::hstring m_fallbackProxyKey;
  winrt::Windows::UI::ViewManagement::UISettings m_uiSettings{nullptr};
  int64_t m_proxyBrushColorChangedToken{};
  int64_t m_fallbackProxyBrushColorChangedToken{};
  winrt::event_token m_advancedEffectsEnabledChangedToken{};
  winrt::event_token m_energySaverStatusChangedToken{};
  winrt::Windows::System::DispatcherQueue m_dispatcher{nullptr};
  HKEY m_powerKey{nullptr};
  HANDLE m_regNotifyEvent{nullptr};
  HANDLE m_regWaitHandle{nullptr};
};
std::wstring TrimWide(std::wstring value) {
  const auto start = value.find_first_not_of(L" \t\r\n");
  if (start == std::wstring::npos) return L"";
  const auto end = value.find_last_not_of(L" \t\r\n");
  return value.substr(start, end - start + 1);
}
struct ParsedWindhawkColorSetting {
  winrt::Windows::UI::Color color;
  std::wstring themeResourceKey;
};
ParsedWindhawkColorSetting ParseWindhawkColorSetting(std::wstring value, winrt::Windows::UI::Color fallbackColor) {
  value = TrimWide(std::move(value));
  constexpr std::wstring_view prefix = L"{ThemeResource ";
  if (value.size() > prefix.size() + 1 && value.rfind(prefix, 0) == 0 && value.back() == L'}') {
    auto key = TrimWide(value.substr(prefix.size(), value.size() - prefix.size() - 1));
    if (!key.empty()) return {fallbackColor, key};
  }
  if (!value.empty() && value[0] == L'#') value.erase(value.begin());
  if (value.size() == 6 || value.size() == 8) {
    try {
      const unsigned long parsed = std::stoul(value, nullptr, 16);
      if (value.size() == 8) {
        return {winrt::Windows::UI::Color{static_cast<uint8_t>((parsed >> 24) & 0xFF), static_cast<uint8_t>((parsed >> 16) & 0xFF), static_cast<uint8_t>((parsed >> 8) & 0xFF), static_cast<uint8_t>(parsed & 0xFF)}, L""};
      }
      return {winrt::Windows::UI::Color{255, static_cast<uint8_t>((parsed >> 16) & 0xFF), static_cast<uint8_t>((parsed >> 8) & 0xFF), static_cast<uint8_t>(parsed & 0xFF)}, L""};
    } catch (...) {
    }
  }
  return {fallbackColor, L""};
}
bool ShouldUseSolidTaskbarBackgroundFill() {
  std::lock_guard<std::recursive_mutex> settingsLock(g_settingsMutex);
  return g_settings.userDefinedDisableCustomBlurBackground ||
         g_settings.userDefinedTaskbarBackgroundTint >= 100 ||
         g_settings.userDefinedTaskbarBackgroundBlurAmount == 0 ||
         g_settings.userDefinedTaskbarBackgroundInversion >= 100;
}
Media::Brush CreateSolidBackgroundBrushFromSetting(ParsedWindhawkColorSetting const& setting) {
  if (!setting.themeResourceKey.empty()) {
    std::wstring xaml = LR"(<SolidColorBrush xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation" Color="{ThemeResource )";
    xaml += setting.themeResourceKey;
    xaml += LR"(}"/>)";
    try {
      if (auto themeBrush = Markup::XamlReader::Load(winrt::hstring(xaml)).try_as<Media::SolidColorBrush>()) {
        return themeBrush;
      }
    } catch (winrt::hresult_error const& ex) {
      Wh_Log(L"Failed to create solid background theme brush %08X", ex.code());
    }
  }
  auto solidBrush = Media::SolidColorBrush();
  solidBrush.Color(setting.color);
  return solidBrush;
}
void ClearWindhawkBlurFromBackgroundFill(FrameworkElement const& backgroundFillChild) {
  if (!backgroundFillChild) return;
  auto rectangle = backgroundFillChild.try_as<winrt::Windows::UI::Xaml::Shapes::Rectangle>();
  if (!rectangle) {
    return;
  }
  try {
    // Do not leave a mod-implemented XamlCompositionBrushBase attached to
    // Explorer's XAML tree while Windhawk unloads/reloads this DLL.
    // Otherwise Explorer can later call into the old brush object's vtable after
    // the module that implemented it has been unloaded.
    auto oldFill = rectangle.Fill();
    rectangle.Fill(Media::Brush{nullptr});
    rectangle.ClearValue(winrt::Windows::UI::Xaml::Shapes::Shape::FillProperty());
    rectangle.ClearValue(UIElement::OpacityProperty());
    oldFill = nullptr;
  } catch (winrt::hresult_error const& ex) {
    Wh_Log(L"WindhawkBlur cleanup failed %08X: %s", ex.code(), ex.message().c_str());
  } catch (...) {
    Wh_Log(L"WindhawkBlur cleanup failed: %08X", winrt::to_hresult());
  }
}
bool ApplyWindhawkBlurToBackgroundFill(FrameworkElement const& backgroundFillChild) {
  if (!backgroundFillChild) return false;
  std::lock_guard<std::recursive_mutex> settingsLock(g_settingsMutex);
  auto rectangle = backgroundFillChild.try_as<winrt::Windows::UI::Xaml::Shapes::Rectangle>();
  if (!rectangle) {
    Wh_Log(L"WindhawkBlur: BackgroundFill is not a Rectangle");
    return false;
  }
  if (g_unloading) {
    ClearWindhawkBlurFromBackgroundFill(backgroundFillChild);
    return false;
  }
  const float backgroundOpacity = std::clamp(g_settings.userDefinedTaskbarBackgroundOpacity / 100.0f, 0.0f, 1.0f);
  rectangle.Opacity(backgroundOpacity);
  auto fallbackSetting = ParseWindhawkColorSetting(
      g_settings.userDefinedTaskbarBackgroundFallbackColor,
      winrt::Windows::UI::Color{255, 32, 32, 32});
  const bool useSolidBackground = ShouldUseSolidTaskbarBackgroundFill();
  try {
    auto oldFill = rectangle.Fill();
    rectangle.Fill(Media::Brush{nullptr});
    oldFill = nullptr;
    if (useSolidBackground) {
      rectangle.Fill(CreateSolidBackgroundBrushFromSetting(fallbackSetting));
    } else {
      auto tintSetting = ParseWindhawkColorSetting(
          g_settings.userDefinedTaskbarBackgroundTintColor,
          winrt::Windows::UI::Color{0, 255, 255, 255});
      const float tintOpacity = std::clamp(g_settings.userDefinedTaskbarBackgroundTint / 100.0f, 0.0f, 1.0f);
      const auto tintAlpha = static_cast<uint8_t>(std::round(tintOpacity * 255.0f));
      tintSetting.color.A = tintAlpha;
      const float luminosityOpacity = std::clamp(g_settings.userDefinedTaskbarBackgroundLuminosity / 100.0f, 0.0f, 1.0f);
      const float saturation = std::clamp(g_settings.userDefinedTaskbarBackgroundTintSaturation / 100.0f, 0.0f, 5.0f);
      const float inversion = std::clamp(g_settings.userDefinedTaskbarBackgroundInversion / 100.0f, 0.0f, 1.0f);
      auto blurBrush = winrt::make<XamlBlurBrush>(
          rectangle,
          static_cast<float>(g_settings.userDefinedTaskbarBackgroundBlurAmount),
          tintSetting.color,
          std::optional<uint8_t>(tintAlpha),
          winrt::hstring(tintSetting.themeResourceKey),
          std::optional<float>(luminosityOpacity),
          std::optional<float>(saturation),
          inversion > 0.0f ? std::optional<float>(inversion) : std::nullopt,
          fallbackSetting.themeResourceKey.empty() ? std::optional<winrt::Windows::UI::Color>(fallbackSetting.color) : std::nullopt,
          winrt::hstring(fallbackSetting.themeResourceKey));
      rectangle.Fill(blurBrush);
    }
    return true;
  } catch (winrt::hresult_error const& ex) {
    Wh_Log(L"WindhawkBlur failed %08X: %s", ex.code(), ex.message().c_str());
  } catch (...) {
    Wh_Log(L"WindhawkBlur failed: %08X", winrt::to_hresult());
  }
  return false;
}
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
/////      .___________.     ___       __        /////
/////      |           |    /   \     |  |       /////
/////      `---|  |----`   /  ^  \    |  |       /////
/////          |  |       /  /_\  \   |  |       /////
/////          |  |      /  _____  \  |  |       /////
/////          |__|     /__/     \__\ |__|       /////
/////                                            /////
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
#include <dwmapi.h>
#include <chrono>
#include <string>
#include <regex>
#include <sstream>
#include <algorithm>
#include <unordered_map>
#include <limits>
#include <utility>
#include <windhawk_api.h>
#include <windhawk_utils.h>
#include <functional>
#undef GetCurrentTime
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Text.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Data.h>
#include <winrt/Windows.UI.Xaml.Hosting.h>
#include <winrt/Windows.UI.Xaml.Markup.h>
#include <winrt/Windows.UI.Xaml.Media.Animation.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/base.h>
#include <commctrl.h>
#include <roapi.h>
#include <winstring.h>
#include <string_view>
#include <vector>
#include <atomic>
#include <cmath>
#include <winrt/Windows.Graphics.Imaging.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.Storage.h>
#include <winrt/Windows.UI.Xaml.Media.Imaging.h>
#include <winrt/Windows.Storage.Search.h>
#include <thread>
#include <windows.h>
#include <psapi.h>
#include <winrt/Windows.UI.Xaml.Shapes.h>
#include <mutex>
#include <condition_variable>
#include <memory>
using namespace winrt::Windows::UI::Xaml;
#ifndef HSHELL_GETMINRECT
#define HSHELL_GETMINRECT 5
#endif
struct SHELLHOOKINFO_TAI {
  HWND hwnd;
  RECT rc;
};
struct MinimizeAnimationMeasuredButtonTai {
  RECT visibleRectPx{};
  std::wstring matchName;
};
struct MinimizeAnimationCorrectionTai {
  double layoutOffsetXDip{0.0};
  double visualScale{1.0};
  double scaleCenterXDip{0.0};
  double rasterizationScale{1.0};
  RECT monitorRect{};
  RECT clampXRect{};
  bool hasClampXRect{false};
  std::vector<MinimizeAnimationMeasuredButtonTai> measuredButtons;
};
static std::mutex g_minimizeAnimationCorrectionMutexTai;
static std::unordered_map<
    std::wstring,
    std::shared_ptr<const MinimizeAnimationCorrectionTai>>
    g_minimizeAnimationCorrectionByMonitorNameTai;
static std::atomic_bool g_minimizeAnimationCorrectionReadyTai{false};
static std::atomic_bool g_minimizeAnimationCorrectionUninitializingTai{false};
static thread_local LPARAM g_minimizeAnimationLastCorrectedLParamTai = 0;
static thread_local HWND g_minimizeAnimationLastCorrectedHwndTai = nullptr;
static thread_local RECT g_minimizeAnimationLastCorrectedRawTai{};
static thread_local DWORD g_minimizeAnimationLastCorrectedTickTai = 0;
using SendMessageW_t = LRESULT(WINAPI*)(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
static SendMessageW_t SendMessageW_OriginalTai = nullptr;
static bool ReadShellHookInfoTai(LPARAM lParam, SHELLHOOKINFO_TAI* shellHookInfo) {
  if (!lParam || !shellHookInfo) {
    return false;
  }
#if defined(_MSC_VER)
  __try {
    *shellHookInfo = *reinterpret_cast<const SHELLHOOKINFO_TAI*>(lParam);
  } __except (EXCEPTION_EXECUTE_HANDLER) {
    return false;
  }
#else
  *shellHookInfo = *reinterpret_cast<const SHELLHOOKINFO_TAI*>(lParam);
#endif
  return true;
}
static bool WriteShellHookRectTai(LPARAM lParam, const RECT& rect) {
  if (!lParam) {
    return false;
  }
#if defined(_MSC_VER)
  __try {
    reinterpret_cast<SHELLHOOKINFO_TAI*>(lParam)->rc = rect;
  } __except (EXCEPTION_EXECUTE_HANDLER) {
    return false;
  }
#else
  reinterpret_cast<SHELLHOOKINFO_TAI*>(lParam)->rc = rect;
#endif
  return true;
}
static bool GetMonitorRectByNameTai(const std::wstring& monitorName, RECT* rect) {
  if (!rect || monitorName.empty()) {
    return false;
  }
  struct EnumContext {
    const std::wstring* monitorName;
    RECT* rect;
    bool found;
  } context{&monitorName, rect, false};
  EnumDisplayMonitors(
      nullptr,
      nullptr,
      [](HMONITOR monitor, HDC, LPRECT, LPARAM lParam) -> BOOL {
        auto* context = reinterpret_cast<EnumContext*>(lParam);
        if (!context || !context->monitorName || !context->rect) {
          return TRUE;
        }
        MONITORINFOEXW monitorInfo{};
        monitorInfo.cbSize = sizeof(monitorInfo);
        if (GetMonitorInfoW(monitor, &monitorInfo) &&
            _wcsicmp(monitorInfo.szDevice, context->monitorName->c_str()) == 0) {
          *context->rect = monitorInfo.rcMonitor;
          context->found = true;
          return FALSE;
        }
        return TRUE;
      },
      reinterpret_cast<LPARAM>(&context));
  return context.found;
}
static int RectWidthTai(const RECT& rect) {
  return rect.right - rect.left;
}
static int ClampIntTai(int value, int minValue, int maxValue) {
  if (value < minValue) {
    return minValue;
  }
  if (value > maxValue) {
    return maxValue;
  }
  return value;
}
static void ClampRectCenterXToBoundsTai(RECT* rect, const RECT& bounds) {
  if (!rect || IsRectEmpty(rect) || IsRectEmpty(&bounds)) {
    return;
  }
  const int width = std::max(1, RectWidthTai(*rect));
  const int currentCenterX = rect->left + width / 2;
  int minCenterX = bounds.left + width / 2;
  int maxCenterX = bounds.right - width / 2;
  if (minCenterX > maxCenterX) {
    minCenterX = maxCenterX = (bounds.left + bounds.right) / 2;
  }
  const int newCenterX = ClampIntTai(currentCenterX, minCenterX, maxCenterX);
  OffsetRect(rect, newCenterX - currentCenterX, 0);
}
static std::wstring TrimTai(const std::wstring& value) {
  const auto first = value.find_first_not_of(L" \t\r\n");
  if (first == std::wstring::npos) {
    return L"";
  }
  const auto last = value.find_last_not_of(L" \t\r\n");
  return value.substr(first, last - first + 1);
}
static std::wstring GetWindowTextSafeTai(HWND hwnd) {
  if (!hwnd || !IsWindow(hwnd)) {
    return L"";
  }
  int length = GetWindowTextLengthW(hwnd);
  if (length <= 0 || length > 32767) {
    return L"";
  }
  std::wstring text(static_cast<size_t>(length) + 1, L'\0');
  int copied = GetWindowTextW(hwnd, text.data(), length + 1);
  if (copied <= 0) {
    return L"";
  }
  text.resize(static_cast<size_t>(copied));
  return TrimTai(text);
}
static std::wstring StripExeExtensionTai(std::wstring value) {
  value = TrimTai(value);
  if (value.size() > 4 && _wcsicmp(value.c_str() + value.size() - 4, L".exe") == 0) {
    value.resize(value.size() - 4);
  }
  return value;
}
static bool TryGetTargetWindowProcessBaseNameTai(HWND hwnd, std::wstring* processBaseName) {
  if (!hwnd || !processBaseName || !IsWindow(hwnd)) {
    return false;
  }
  DWORD processId = 0;
  GetWindowThreadProcessId(hwnd, &processId);
  if (!processId) {
    return false;
  }
  std::wstring processName = StripExeExtensionTai(GetProcessFileName(processId));
  if (processName.empty()) {
    return false;
  }
  *processBaseName = processName;
  return true;
}
static bool IsUsableMeasuredButtonRectTai(const RECT& rect) {
  return rect.right > rect.left && rect.bottom > rect.top;
}
static bool TextEqualsOrdinalIgnoreCaseTai(
    std::wstring_view left,
    std::wstring_view right) {
  if (left.size() != right.size() ||
      left.size() > static_cast<size_t>(std::numeric_limits<int>::max())) {
    return false;
  }
  return CompareStringOrdinal(
             left.data(),
             static_cast<int>(left.size()),
             right.data(),
             static_cast<int>(right.size()),
             TRUE) == CSTR_EQUAL;
}
static bool TextContainsOrdinalIgnoreCaseTai(
    std::wstring_view haystack,
    std::wstring_view needle) {
  if (haystack.empty() || needle.empty() || needle.size() > haystack.size() ||
      needle.size() > static_cast<size_t>(std::numeric_limits<int>::max())) {
    return false;
  }
  const int needleLength = static_cast<int>(needle.size());
  const size_t lastStart = haystack.size() - needle.size();
  for (size_t start = 0; start <= lastStart; ++start) {
    if (CompareStringOrdinal(
            haystack.data() + start,
            needleLength,
            needle.data(),
            needleLength,
            TRUE) == CSTR_EQUAL) {
      return true;
    }
  }
  return false;
}
static const MinimizeAnimationMeasuredButtonTai* FindMeasuredButtonForTargetWindowTai(
    HWND targetWindow,
    const RECT& currentRect,
    const MinimizeAnimationCorrectionTai& correction,
    int* matchTier) {
  if (!targetWindow || !IsWindow(targetWindow) || correction.measuredButtons.empty()) {
    return nullptr;
  }
  if (matchTier) {
    *matchTier = 0;
  }
  const std::wstring title = GetWindowTextSafeTai(targetWindow);
  const int currentCenterX = currentRect.left + std::max(1, RectWidthTai(currentRect)) / 2;
  const MinimizeAnimationMeasuredButtonTai* best = nullptr;
  int bestTier = 0;
  LONG bestDistance = LONG_MAX;
  auto considerButton = [&](const MinimizeAnimationMeasuredButtonTai& button,
                            int tier) {
    if (tier <= 0) {
      return;
    }
    const int buttonCenterX = button.visibleRectPx.left + RectWidthTai(button.visibleRectPx) / 2;
    const LONG distance = buttonCenterX >= currentCenterX ? buttonCenterX - currentCenterX : currentCenterX - buttonCenterX;
    if (!best || tier > bestTier || (tier == bestTier && distance < bestDistance)) {
      best = &button;
      bestTier = tier;
      bestDistance = distance;
    }
  };
  if (!title.empty()) {
    for (const auto& button : correction.measuredButtons) {
      if (!IsUsableMeasuredButtonRectTai(button.visibleRectPx) ||
          button.matchName.empty()) {
        continue;
      }
      if (TextEqualsOrdinalIgnoreCaseTai(button.matchName, title)) {
        considerButton(button, 3);
      } else if (
          TextContainsOrdinalIgnoreCaseTai(button.matchName, title) ||
          TextContainsOrdinalIgnoreCaseTai(title, button.matchName)) {
        considerButton(button, 2);
      }
    }
  }
  if (best) {
    if (matchTier) {
      *matchTier = bestTier;
    }
    return best;
  }
  std::wstring processBaseName;
  if (!TryGetTargetWindowProcessBaseNameTai(
          targetWindow,
          &processBaseName)) {
    return nullptr;
  }
  for (const auto& button : correction.measuredButtons) {
    if (!IsUsableMeasuredButtonRectTai(button.visibleRectPx) ||
        button.matchName.empty()) {
      continue;
    }
    if (TextContainsOrdinalIgnoreCaseTai(
            button.matchName,
            processBaseName)) {
      considerButton(button, 1);
    }
  }
  if (matchTier) {
    *matchTier = bestTier;
  }
  return best;
}
static bool TryUseMeasuredTaskbarButtonRectTai(
    HWND targetWindow,
    RECT* rect,
    const MinimizeAnimationCorrectionTai& correction,
    int* matchTier) {
  if (!rect || correction.measuredButtons.empty()) {
    return false;
  }
  const auto* button = FindMeasuredButtonForTargetWindowTai(targetWindow, *rect, correction, matchTier);
  if (!button || !IsUsableMeasuredButtonRectTai(button->visibleRectPx)) {
    return false;
  }
  rect->left = button->visibleRectPx.left;
  rect->right = button->visibleRectPx.right;
  return true;
}
static LONG TransformMinimizeRectXTai(LONG xPx, const MinimizeAnimationCorrectionTai& correction) {
  const double dpiScale = std::max(0.01, correction.rasterizationScale);
  double xDip = (static_cast<double>(xPx) - static_cast<double>(correction.monitorRect.left)) / dpiScale;
  // First apply the RootGrid translation that visually brings the virtual
  // taskbar surface back to the real screen, then apply the same island scale.
  xDip += correction.layoutOffsetXDip;
  xDip = correction.scaleCenterXDip + ((xDip - correction.scaleCenterXDip) * correction.visualScale);
  return static_cast<LONG>(std::lround(static_cast<double>(correction.monitorRect.left) + xDip * dpiScale));
}
static std::shared_ptr<const MinimizeAnimationCorrectionTai>
GetMinimizeAnimationCorrectionForWindowTai(HWND targetWindow) {
  if (!targetWindow ||
      !g_minimizeAnimationCorrectionReadyTai.load(std::memory_order_acquire) ||
      g_minimizeAnimationCorrectionUninitializingTai.load(std::memory_order_acquire)) {
    return nullptr;
  }
  if (!IsWindow(targetWindow)) {
    return nullptr;
  }
  HMONITOR monitor = MonitorFromWindow(targetWindow, MONITOR_DEFAULTTONEAREST);
  if (!monitor) {
    return nullptr;
  }
  const std::wstring monitorName = GetMonitorName(monitor);
  if (monitorName.empty()) {
    return nullptr;
  }
  std::lock_guard<std::mutex> lock(g_minimizeAnimationCorrectionMutexTai);
  auto it = g_minimizeAnimationCorrectionByMonitorNameTai.find(monitorName);
  if (it == g_minimizeAnimationCorrectionByMonitorNameTai.end()) {
    return nullptr;
  }
  return it->second;
}
static bool WasMinimizeAnimationPayloadAlreadyCorrectedTai(
    LPARAM lParam,
    const SHELLHOOKINFO_TAI& shellHookInfo) {
  if (!lParam) {
    return false;
  }
  if (g_minimizeAnimationLastCorrectedLParamTai != lParam ||
      g_minimizeAnimationLastCorrectedHwndTai != shellHookInfo.hwnd ||
      !EqualRect(&g_minimizeAnimationLastCorrectedRawTai, &shellHookInfo.rc)) {
    return false;
  }
  return GetTickCount() - g_minimizeAnimationLastCorrectedTickTai < 1000;
}
static void RememberMinimizeAnimationCorrectedPayloadTai(
    LPARAM lParam,
    const SHELLHOOKINFO_TAI& shellHookInfo) {
  if (!lParam) {
    return;
  }
  g_minimizeAnimationLastCorrectedLParamTai = lParam;
  g_minimizeAnimationLastCorrectedHwndTai = shellHookInfo.hwnd;
  g_minimizeAnimationLastCorrectedRawTai = shellHookInfo.rc;
  g_minimizeAnimationLastCorrectedTickTai = GetTickCount();
}
static SHORT SignedLowWordTai(LONG value) {
  return static_cast<SHORT>(static_cast<WORD>(value & 0xFFFF));
}
static SHORT SignedHighWordTai(LONG value) {
  return static_cast<SHORT>(static_cast<WORD>((static_cast<DWORD>(value) >> 16) & 0xFFFF));
}
static LONG PackSignedPointTai(LONG x, LONG y) {
  return static_cast<LONG>((static_cast<DWORD>(static_cast<WORD>(y)) << 16) |
                           static_cast<DWORD>(static_cast<WORD>(x)));
}
static bool TryDecodePackedMinimizeRectTai(const SHELLHOOKINFO_TAI& shellHookInfo, RECT* decodedRect) {
  if (!decodedRect) {
    return false;
  }
  // On recent Windows 11 builds, the SHELLHOOKINFO rc for HSHELL_GETMINRECT can
  // arrive as two packed POINTS rather than a normal RECT:
  //   rc.left = MAKELONG(left, top)
  //   rc.top  = MAKELONG(right, bottom)
  //   rc.right/rc.bottom = 0
  // Example raw log: rc=(87304471,93399320,0,0)
  // Decoded: (10519,1332,10520,1425)
  if (shellHookInfo.rc.right != 0 || shellHookInfo.rc.bottom != 0) {
    return false;
  }
  RECT candidate{};
  candidate.left = SignedLowWordTai(shellHookInfo.rc.left);
  candidate.top = SignedHighWordTai(shellHookInfo.rc.left);
  candidate.right = SignedLowWordTai(shellHookInfo.rc.top);
  candidate.bottom = SignedHighWordTai(shellHookInfo.rc.top);
  if (candidate.right < candidate.left) {
    std::swap(candidate.left, candidate.right);
  }
  if (candidate.bottom < candidate.top) {
    std::swap(candidate.top, candidate.bottom);
  }
  if (candidate.left == 0 && candidate.top == 0 &&
      candidate.right == 0 && candidate.bottom == 0) {
    return false;
  }
  // The packed format is often point-like or very narrow, so do not reject
  // small widths. Only reject obviously unusable vertical geometry.
  if (candidate.bottom <= candidate.top) {
    return false;
  }
  *decodedRect = candidate;
  return true;
}
static RECT EncodePackedMinimizeRectTai(const RECT& rect) {
  RECT encoded{};
  encoded.left = PackSignedPointTai(rect.left, rect.top);
  encoded.top = PackSignedPointTai(rect.right, rect.bottom);
  encoded.right = 0;
  encoded.bottom = 0;
  return encoded;
}
static void CorrectMinimizeAnimationRectTai(HWND targetWindow, RECT* rect) {
  if (!targetWindow || !rect || IsRectEmpty(rect)) {
    return;
  }
  auto correction =
      GetMinimizeAnimationCorrectionForWindowTai(targetWindow);
  if (!correction) {
    return;
  }
  RECT before = *rect;
  LONG transformedLeft = TransformMinimizeRectXTai(rect->left, *correction);
  LONG transformedRight = TransformMinimizeRectXTai(rect->right, *correction);
  if (transformedRight < transformedLeft) {
    std::swap(transformedLeft, transformedRight);
  }
  rect->left = transformedLeft;
  rect->right = transformedRight;
  int measuredMatchTier = 0;
  const bool usedMeasuredButtonRect =
      TryUseMeasuredTaskbarButtonRectTai(
          targetWindow,
          rect,
          *correction,
          &measuredMatchTier);
  if (correction->hasClampXRect) {
    ClampRectCenterXToBoundsTai(rect, correction->clampXRect);
  }
  if (!EqualRect(&before, rect)) {
    Wh_Log(L"[MinRectFix] hwnd=%p before=(%ld,%ld,%ld,%ld) after=(%ld,%ld,%ld,%ld) mode=%s matchTier=%d offsetDip=%.2f scale=%.4f centerDip=%.2f dpiScale=%.3f measuredButtons=%zu",
           targetWindow,
           before.left,
           before.top,
           before.right,
           before.bottom,
           rect->left,
           rect->top,
           rect->right,
           rect->bottom,
           usedMeasuredButtonRect ? L"measured" : L"transform",
           measuredMatchTier,
           correction->layoutOffsetXDip,
           correction->visualScale,
           correction->scaleCenterXDip,
           correction->rasterizationScale,
           correction->measuredButtons.size());
  }
}
static bool TryCorrectShellHookMinRectMessageTai(UINT Msg, WPARAM wParam, LPARAM lParam) {
  if (!g_shellHookMessageTai || Msg != g_shellHookMessageTai ||
      wParam != HSHELL_GETMINRECT || !lParam ||
      g_minimizeAnimationCorrectionUninitializingTai.load(std::memory_order_acquire)) {
    return false;
  }
  SHELLHOOKINFO_TAI shellHookInfo{};
  if (!ReadShellHookInfoTai(lParam, &shellHookInfo)) {
    Wh_Log(L"[MinRectFix] skipped unreadable HSHELL_GETMINRECT payload");
    return true;
  }
  if (!shellHookInfo.hwnd || !IsWindow(shellHookInfo.hwnd)) {
    return true;
  }
  if (WasMinimizeAnimationPayloadAlreadyCorrectedTai(lParam, shellHookInfo)) {
    return true;
  }
  Wh_Log(L"[MinRectFix] HSHELL_GETMINRECT received target=%p raw=(%ld,%ld,%ld,%ld)",
         shellHookInfo.hwnd,
         shellHookInfo.rc.left,
         shellHookInfo.rc.top,
         shellHookInfo.rc.right,
         shellHookInfo.rc.bottom);
  if (!IsRectEmpty(&shellHookInfo.rc)) {
    RECT corrected = shellHookInfo.rc;
    CorrectMinimizeAnimationRectTai(shellHookInfo.hwnd, &corrected);
    if (!EqualRect(&shellHookInfo.rc, &corrected) && WriteShellHookRectTai(lParam, corrected)) {
      SHELLHOOKINFO_TAI remembered = shellHookInfo;
      remembered.rc = corrected;
      RememberMinimizeAnimationCorrectedPayloadTai(lParam, remembered);
    }
    return true;
  }
  RECT decodedPackedRect{};
  if (TryDecodePackedMinimizeRectTai(shellHookInfo, &decodedPackedRect)) {
    RECT before = decodedPackedRect;
    CorrectMinimizeAnimationRectTai(shellHookInfo.hwnd, &decodedPackedRect);
    if (!EqualRect(&before, &decodedPackedRect)) {
      RECT encoded = EncodePackedMinimizeRectTai(decodedPackedRect);
      if (WriteShellHookRectTai(lParam, encoded)) {
        SHELLHOOKINFO_TAI remembered = shellHookInfo;
        remembered.rc = encoded;
        RememberMinimizeAnimationCorrectedPayloadTai(lParam, remembered);
        Wh_Log(L"[MinRectFix] packed decoded before=(%ld,%ld,%ld,%ld) after=(%ld,%ld,%ld,%ld) encoded=(%ld,%ld,%ld,%ld)",
               before.left,
               before.top,
               before.right,
               before.bottom,
               decodedPackedRect.left,
               decodedPackedRect.top,
               decodedPackedRect.right,
               decodedPackedRect.bottom,
               encoded.left,
               encoded.top,
               encoded.right,
               encoded.bottom);
      }
    } else {
      Wh_Log(L"[MinRectFix] packed decoded no-op rect=(%ld,%ld,%ld,%ld)",
             decodedPackedRect.left,
             decodedPackedRect.top,
             decodedPackedRect.right,
             decodedPackedRect.bottom);
    }
    return true;
  }
  Wh_Log(L"[MinRectFix] skipped empty/unrecognized min rect payload");
  return true;
}
static void ClearMinimizeAnimationCorrectionForMonitorTai(const std::wstring& monitorName) {
  if (monitorName.empty()) {
    return;
  }
  std::shared_ptr<const MinimizeAnimationCorrectionTai> removedCorrection;
  {
    std::lock_guard<std::mutex> lock(g_minimizeAnimationCorrectionMutexTai);
    auto it = g_minimizeAnimationCorrectionByMonitorNameTai.find(monitorName);
    if (it != g_minimizeAnimationCorrectionByMonitorNameTai.end()) {
      removedCorrection = std::move(it->second);
      g_minimizeAnimationCorrectionByMonitorNameTai.erase(it);
    }
  }
}
static void SetMinimizeAnimationCorrectionForMonitorTai(
    const std::wstring& monitorName,
    const RECT& monitorRect,
    double layoutOffsetXDip,
    double visualScale,
    double scaleCenterXDip,
    double rasterizationScale,
    const RECT* clampXRect,
    std::vector<MinimizeAnimationMeasuredButtonTai> measuredButtons) {
  if (monitorName.empty() ||
      IsRectEmpty(&monitorRect) ||
      g_minimizeAnimationCorrectionUninitializingTai.load(std::memory_order_acquire)) {
    return;
  }
  if (!std::isfinite(layoutOffsetXDip) ||
      !std::isfinite(visualScale) ||
      !std::isfinite(scaleCenterXDip) ||
      !std::isfinite(rasterizationScale)) {
    return;
  }
  auto correction = std::make_shared<MinimizeAnimationCorrectionTai>();
  correction->layoutOffsetXDip = layoutOffsetXDip;
  correction->visualScale = std::clamp(visualScale, 0.01, 4.0);
  correction->scaleCenterXDip = scaleCenterXDip;
  correction->rasterizationScale = std::clamp(rasterizationScale, 0.25, 8.0);
  correction->monitorRect = monitorRect;
  if (clampXRect && !IsRectEmpty(clampXRect) &&
      clampXRect->right > clampXRect->left) {
    correction->clampXRect = *clampXRect;
    correction->hasClampXRect = true;
  }
  correction->measuredButtons = std::move(measuredButtons);
  std::shared_ptr<const MinimizeAnimationCorrectionTai> previousCorrection;
  {
    std::lock_guard<std::mutex> lock(g_minimizeAnimationCorrectionMutexTai);
    auto it = g_minimizeAnimationCorrectionByMonitorNameTai.find(monitorName);
    if (it != g_minimizeAnimationCorrectionByMonitorNameTai.end() &&
        it->second) {
      previousCorrection = it->second;
    }
    g_minimizeAnimationCorrectionByMonitorNameTai[monitorName] = correction;
  }
  bool shouldLog = true;
  if (previousCorrection) {
    const auto& previous = *previousCorrection;
    shouldLog =
        std::abs(previous.layoutOffsetXDip - correction->layoutOffsetXDip) > 0.01 ||
        std::abs(previous.visualScale - correction->visualScale) > 0.0001 ||
        std::abs(previous.scaleCenterXDip - correction->scaleCenterXDip) > 0.01 ||
        std::abs(previous.rasterizationScale - correction->rasterizationScale) > 0.0001 ||
        previous.hasClampXRect != correction->hasClampXRect ||
        previous.measuredButtons.size() != correction->measuredButtons.size() ||
        !EqualRect(&previous.monitorRect, &correction->monitorRect) ||
        (correction->hasClampXRect &&
         !EqualRect(&previous.clampXRect, &correction->clampXRect));
  }
  if (shouldLog) {
    Wh_Log(L"[MinRectFix] correction monitor=%s offsetDip=%.2f scale=%.4f centerDip=%.2f dpiScale=%.3f clamp=%d [%ld..%ld] measuredButtons=%zu",
           monitorName.c_str(),
           correction->layoutOffsetXDip,
           correction->visualScale,
           correction->scaleCenterXDip,
           correction->rasterizationScale,
           correction->hasClampXRect ? 1 : 0,
           correction->hasClampXRect ? correction->clampXRect.left : 0,
           correction->hasClampXRect ? correction->clampXRect.right : 0,
           correction->measuredButtons.size());
  }
}
static LRESULT WINAPI SendMessageW_HookTai(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
  LRESULT result = SendMessageW_OriginalTai
      ? SendMessageW_OriginalTai(hWnd, Msg, wParam, lParam)
      : 0;
  TryCorrectShellHookMinRectMessageTai(Msg, wParam, lParam);
  return result;
}
static void InitMinimizeAnimationCorrectionTai() {
  g_minimizeAnimationCorrectionUninitializingTai.store(false, std::memory_order_release);
  g_minimizeAnimationCorrectionReadyTai.store(false, std::memory_order_release);
  g_shellHookMessageTai = RegisterWindowMessageW(L"SHELLHOOK");
  HMODULE user32 = GetModuleHandleW(L"user32.dll");
  if (user32) {
    auto sendMessageW = reinterpret_cast<SendMessageW_t>(GetProcAddress(user32, "SendMessageW"));
    if (sendMessageW) {
      if (WindhawkUtils::Wh_SetFunctionHookT(sendMessageW,
                                             SendMessageW_HookTai,
                                             &SendMessageW_OriginalTai)) {
        Wh_Log(L"[MinRectFix] Successfully hooked SendMessageW");
      } else {
        Wh_Log(L"[MinRectFix] Failed to hook SendMessageW");
      }
    }
  }
  g_minimizeAnimationCorrectionReadyTai.store(g_shellHookMessageTai != 0, std::memory_order_release);
  Wh_Log(L"[MinRectFix] initialized without persistent window subclassing");
}
static void UninitMinimizeAnimationCorrectionTai() {
  g_minimizeAnimationCorrectionUninitializingTai.store(true, std::memory_order_release);
  g_minimizeAnimationCorrectionReadyTai.store(false, std::memory_order_release);
  decltype(g_minimizeAnimationCorrectionByMonitorNameTai) correctionsToDestroy;
  {
    std::lock_guard<std::mutex> lock(g_minimizeAnimationCorrectionMutexTai);
    correctionsToDestroy.swap(
        g_minimizeAnimationCorrectionByMonitorNameTai);
  }
  g_shellHookMessageTai = 0;
}
std::wstring EscapeXmlAttribute(std::wstring_view data) {
  std::wstring buffer;
  buffer.reserve(data.size());
  for (wchar_t c : data) buffer.append((c == L'&') ? L"&amp;" : (c == L'\"') ? L"&quot;" : (c == L'<') ? L"&lt;" : (c == L'>') ? L"&gt;" : std::wstring(1, c));
  return buffer;
}
Style GetStyleFromXamlSetters(const std::wstring_view type, const std::wstring_view xamlStyleSetters, std::wstring& outXaml) {
  std::wstring xaml =
      LR"(<ResourceDictionary
    xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"
    xmlns:x="http://schemas.microsoft.com/winfx/2006/xaml"
    xmlns:d="http://schemas.microsoft.com/expression/blend/2008"
    xmlns:mc="http://schemas.openxmlformats.org/markup-compatibility/2006"
    xmlns:muxc="using:Microsoft.UI.Xaml.Controls")";
  if (auto pos = type.rfind('.'); pos != type.npos) {
    auto typeNamespace = std::wstring_view(type).substr(0, pos);
    auto typeName = std::wstring_view(type).substr(pos + 1);
    xaml += L"\n    xmlns:windhawkstyler=\"using:";
    xaml += EscapeXmlAttribute(typeNamespace);
    xaml += L"\">\n    <Style TargetType=\"windhawkstyler:";
    xaml += EscapeXmlAttribute(typeName);
    xaml += L"\">\n";
  } else {
    xaml += L">\n    <Style TargetType=\"";
    xaml += EscapeXmlAttribute(type);
    xaml += L"\">\n";
  }
  xaml += xamlStyleSetters;
  xaml +=
      L"    </Style>\n"
      L"</ResourceDictionary>";
  outXaml = xaml;
  auto resourceDictionary = Markup::XamlReader::Load(xaml).as<ResourceDictionary>();
  auto [styleKey, styleInspectable] = resourceDictionary.First().Current();
  return styleInspectable.as<Style>();
}
void SetElementPropertyFromString(FrameworkElement obj, const std::wstring& type, const std::wstring& propertyName, const std::wstring& propertyValue, bool isXamlValue) {
  if(!obj) return;
  std::wstring outXamlResult;
  try {
    std::wstring xamlSetter = L"<Setter Property=\"";
    xamlSetter += EscapeXmlAttribute(propertyName);
    xamlSetter += L"\"";
    if (isXamlValue) {
      xamlSetter +=
          L">\n"
          L"    <Setter.Value>\n";
      xamlSetter += propertyValue;
      xamlSetter += L"\n    </Setter.Value>\n";
      xamlSetter += L"</Setter>";
    } else {
      xamlSetter += L" Value=\"";
      xamlSetter += EscapeXmlAttribute(propertyValue);
      xamlSetter += L"\"/>";
    }
    auto style = GetStyleFromXamlSetters(type, xamlSetter, outXamlResult);
    for (uint32_t i = 0; i < style.Setters().Size(); ++i) {
      auto setter = style.Setters().GetAt(i).as<Setter>();
      obj.SetValue(setter.Property(), setter.Value());
    }
  } catch (const std::exception& ex) {
    if (!outXamlResult.empty()) {
      Wh_Log(L"Error: %S. Xaml Result: %s", ex.what(), outXamlResult.c_str());
    } else {
      Wh_Log(L"Error: %S", ex.what());
    }
  } catch (const winrt::hresult_error& ex) {
    if (!outXamlResult.empty()) {
      Wh_Log(L"Error %08X: %s. Xaml Result: %s", ex.code(), ex.message().c_str(), outXamlResult.c_str());
    } else {
      Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
    }
  } catch (...) {
    if (!outXamlResult.empty()) {
      Wh_Log(L"Unknown error occurred while setting property. Xaml Result: %s", outXamlResult.c_str());
    } else {
      Wh_Log(L"Unknown error occurred while setting property.");
    }
  }
}
void SetElementPropertyFromString(FrameworkElement obj, const std::wstring& type, const std::wstring& propertyName, const std::wstring& propertyValue) { return SetElementPropertyFromString(obj, type, propertyName, propertyValue, false); }
std::vector<std::wstring> SplitAndTrim(PCWSTR input) {
  std::vector<std::wstring> result;
  if (!input || *input == L'\0') {
    return result;
  }
  std::wstringstream ss(input);
  std::wstring item;
  while (std::getline(ss, item, L';')) {
    size_t start = item.find_first_not_of(L" \t");
    size_t end = item.find_last_not_of(L" \t");
    if (start != std::wstring::npos && end != std::wstring::npos) {
      std::wstring trimmed = item.substr(start, end - start + 1);
      if (!trimmed.empty()) {
        result.push_back(trimmed);
      }
    }
  }
  return result;
}
void CompileDividedAppPatternsTai(
    std::vector<std::wstring> const& patterns) {
  g_settings.compiledDividedAppPatterns.clear();
  g_settings.compiledDividedAppPatterns.reserve(patterns.size());
  for (const auto& pattern : patterns) {
    try {
      g_settings.compiledDividedAppPatterns.emplace_back(
          pattern,
          std::regex_constants::icase |
              std::regex_constants::optimize);
    } catch (const std::regex_error&) {
      Wh_Log(L"Invalid DividedAppNames regex ignored: %s", pattern.c_str());
    }
  }
}
bool MatchesDividedAppPatternTai(
    std::wstring const& value,
    std::wregex const& pattern) {
  try {
    return std::regex_search(value, pattern);
  } catch (const std::regex_error&) {
    return false;
  }
}
std::atomic<bool> g_scheduled_low_priority_update = false;
std::atomic<bool> g_delayed_apply_worker_running = false;
std::atomic<int64_t> g_delayed_apply_due_ms = 0;
std::atomic<unsigned long long> g_delayed_apply_generation = 0;
std::atomic<bool> g_initial_style_apply_completed = false;
std::atomic<bool> g_initial_taskbar_size_apply_done = false;
std::atomic<int64_t> g_initial_style_apply_not_before_ms = 0;
std::atomic<int> g_force_style_apply_passes = 0;
std::atomic<int> g_high_priority_dispatch_passes = 0;
std::atomic<int> g_reset_animation_target_passes = 0;
std::atomic<int64_t> g_last_geometry_critical_apply_ms = 0;
std::atomic<bool> g_animation_followup_worker_running = false;
std::atomic<int64_t> g_suppress_low_priority_apply_until_ms = 0;
std::atomic<bool> g_worker_threads_stopping = false;
std::mutex g_delayed_apply_worker_thread_mutex;
std::thread g_delayed_apply_worker_thread;
std::mutex g_delayed_apply_worker_wait_mutex;
std::condition_variable g_delayed_apply_worker_wake;
std::mutex g_animation_followup_worker_thread_mutex;
std::thread g_animation_followup_worker_thread;
constexpr int kDefaultStyleDebounceDelayMs = 150;
constexpr int kTaskbarIslandAnimationDurationMs = 250;
constexpr int kStartButtonAnchorStablePassesRequired = 2;
constexpr double kTaskbarVirtualSurfaceMaxPhysicalWidth =
    static_cast<double>(std::numeric_limits<SHORT>::max()) / 2.0;
constexpr int kLowPriorityStyleDelayMs =
    kDefaultStyleDebounceDelayMs + (kTaskbarIslandAnimationDurationMs * 3);
constexpr int kExplorerStartupSettleAnimationWindows = 6;
constexpr int kInitialExplorerStyleDelayMs =
    kLowPriorityStyleDelayMs + kDefaultStyleDebounceDelayMs +
    (kTaskbarIslandAnimationDurationMs * kExplorerStartupSettleAnimationWindows);
constexpr int kGeometryCriticalApplyMinIntervalMs = kDefaultStyleDebounceDelayMs / 2;
constexpr int kInitialExplorerStyleRetryDelayMs = kTaskbarIslandAnimationDurationMs * 2;
constexpr int kButtonSizeLowPrioritySuppressionMs = kTaskbarIslandAnimationDurationMs;
constexpr int kGeometryCriticalLowPrioritySuppressionMs =
    (kTaskbarIslandAnimationDurationMs * 2) + kDefaultStyleDebounceDelayMs;
constexpr int kScheduledLowPriorityFlagTtlMs =
    kLowPriorityStyleDelayMs + kDefaultStyleDebounceDelayMs + kTaskbarIslandAnimationDurationMs;
constexpr double kMinimumTrustedRefreshHz = 24.0;
constexpr double kMaximumTrustedRefreshHz = 1000.0;
constexpr int kDefaultFrameIntervalMs = 16;
constexpr int kMinimumFrameIntervalMs = 1;
constexpr int kMaximumFrameIntervalMs = 42;
constexpr int kAnimationFollowupGraceFrames = 2;
constexpr float kDisplayGeometryChangeToleranceDip = 0.5f;
constexpr float kDisplayRasterizationChangeTolerance = 0.001f;
template <typename TAnimation>
void ConfigureTaskbarIslandAnimation(TAnimation const& animation) {
  animation.Duration(winrt::Windows::Foundation::TimeSpan(std::chrono::milliseconds(kTaskbarIslandAnimationDurationMs)));
  animation.DelayTime(winrt::Windows::Foundation::TimeSpan(std::chrono::milliseconds(0)));
}
winrt::Windows::UI::Composition::CompositionEasingFunction CreateTaskbarIslandEasingFunction(
    winrt::Windows::UI::Composition::Compositor const& compositor) {
  if (!compositor) {
    return nullptr;
  }
  // Keep every moving piece on the same curve. The explicit ease-out avoids
  // one visual catching up linearly while another decelerates, which reads as
  // rubber-banding when the taskbar island is resized, translated, and scaled
  // at the same time.
  return compositor.CreateCubicBezierEasingFunction(
      winrt::Windows::Foundation::Numerics::float2{0.16f, 1.0f},
      winrt::Windows::Foundation::Numerics::float2{0.30f, 1.0f});
}
template <typename TAnimation, typename TValue>
void InsertTaskbarIslandKeyFrame(TAnimation const& animation,
                                 float progress,
                                 TValue const& value) {
  if (auto compositor = animation.Compositor()) {
    if (auto easing = CreateTaskbarIslandEasingFunction(compositor)) {
      animation.InsertKeyFrame(progress, value, easing);
      return;
    }
  }
  animation.InsertKeyFrame(progress, value);
}
float ApplyTaskbarIslandEasingForEstimate(float progress) {
  progress = std::clamp(progress, 0.0f, 1.0f);
  // Approximation of the compositor ease-out used above. This is only for
  // calculating a safe start value when an in-flight background shape animation
  // is retargeted; all real animations use the compositor easing function.
  const float inv = 1.0f - progress;
  return 1.0f - (inv * inv * inv);
}
float LerpFloat(float from, float to, float progress) {
  return from + ((to - from) * progress);
}
float EstimateAnimationValue(float from, float to, int64_t startedMs, int64_t nowMs) {
  if (startedMs <= 0 || nowMs <= startedMs) {
    return from;
  }
  float progress = static_cast<float>(nowMs - startedMs) / static_cast<float>(kTaskbarIslandAnimationDurationMs);
  if (progress <= 0.0f) {
    return from;
  }
  if (progress >= 1.0f) {
    return to;
  }
   return LerpFloat(from, to, ApplyTaskbarIslandEasingForEstimate(progress));
}
static float SnapToPhysicalPixel(float value, float rasterizationScale);
float SnapScaleForPhysicalPixels(float scale,
                                 float unscaledWidth,
                                 float rasterizationScale) {
  if (unscaledWidth <= 0.0f || !std::isfinite(unscaledWidth) ||
      !std::isfinite(scale)) {
    return 1.0f;
  }
  const float snappedWidth =
      SnapToPhysicalPixel(unscaledWidth * scale, rasterizationScale);
  if (snappedWidth <= 0.0f || !std::isfinite(snappedWidth)) {
    return scale;
  }
  return snappedWidth / unscaledWidth;
}
float CalculateTaskbarIslandScale(float screenLeft,
                                  float screenRight,
                                  float screenWidth,
                                  float scaleCenterX,
                                  float rasterizationScale) {
  if (screenWidth <= 0.0f || screenRight <= screenLeft ||
      !std::isfinite(screenLeft) || !std::isfinite(screenRight) ||
      !std::isfinite(screenWidth) || !std::isfinite(scaleCenterX)) {
    return 1.0f;
  }
  const float unscaledWidth = screenRight - screenLeft;
  float targetScale = 1.0f;
  // Scale only as much as needed to keep the island inside the current screen.
  // This is intentionally not limited by a user setting: once the task area is
  // wider than the monitor, shrinking to any size is safer than allowing
  // Explorer's native overflow layout to appear and destabilize the taskbar.
  if (screenLeft < 0.0f && scaleCenterX > screenLeft) {
    targetScale = std::min(targetScale,
                           scaleCenterX / (scaleCenterX - screenLeft));
  }
  if (screenRight > screenWidth && screenRight > scaleCenterX) {
    targetScale = std::min(targetScale,
                           (screenWidth - scaleCenterX) /
                               (screenRight - scaleCenterX));
  }
  if (!std::isfinite(targetScale) || targetScale <= 0.0f) {
    targetScale = screenWidth / unscaledWidth;
  }
  if (!std::isfinite(targetScale) || targetScale <= 0.0f) {
    return 1.0f;
  }
  targetScale = std::min(targetScale, 1.0f);
  const float snappedScale = SnapScaleForPhysicalPixels(targetScale,
                                                        unscaledWidth,
                                                        rasterizationScale);
  if (std::isfinite(snappedScale) && snappedScale > 0.0f) {
    // Pixel snapping can round the scaled width up by a fraction of a pixel.
    // Never let snapping pick a larger scale than the geometric fit.
    targetScale = std::min(targetScale, snappedScale);
  }
  return std::min(targetScale, 1.0f);
}
float ApplyScaleToScreenX(float screenX, float scaleCenterX, float scale) {
  return scaleCenterX + ((screenX - scaleCenterX) * scale);
}
void SetVisualScaleCenterAndAnimate(
    winrt::Windows::UI::Composition::Visual const& visual,
    float targetScale,
    float localCenterX,
    float localCenterY,
    float visualOffsetTolerance,
    bool animate) {
  if (!visual) {
    return;
  }
  if (!std::isfinite(targetScale) || targetScale <= 0.0f) {
    targetScale = 1.0f;
  }
  if (!std::isfinite(localCenterX)) {
    localCenterX = 0.0f;
  }
  if (!std::isfinite(localCenterY)) {
    localCenterY = 0.0f;
  }
  visual.CenterPoint({localCenterX, localCenterY, visual.CenterPoint().z});
  const auto currentScale = visual.Scale();
  if (std::abs(currentScale.x - targetScale) <= visualOffsetTolerance &&
      std::abs(currentScale.y - targetScale) <= visualOffsetTolerance) {
    return;
  }
  if (animate) {
    if (auto compositor = visual.Compositor()) {
      auto scaleAnimation = compositor.CreateVector3KeyFrameAnimation();
      ConfigureTaskbarIslandAnimation(scaleAnimation);
      InsertTaskbarIslandKeyFrame(
          scaleAnimation,
          1.0f,
          winrt::Windows::Foundation::Numerics::float3{
              targetScale, targetScale, currentScale.z});
      visual.StartAnimation(L"Scale", scaleAnimation);
      return;
    }
  }
  visual.StopAnimation(L"Scale");
  visual.Scale({targetScale, targetScale, currentScale.z});
}
void ResetBackgroundVisualTargetCache(TaskbarState& state) {
  state.lastBackgroundShapeTargetWidth = 0.0f;
  state.lastBackgroundShapeTargetHeight = 0.0f;
  state.lastBackgroundShapeTargetOffsetX = 0.0f;
  state.lastBackgroundShapeTargetOffsetY = 0.0f;
  state.backgroundAnimationFromWidth = 0.0f;
  state.backgroundAnimationToWidth = 0.0f;
  state.backgroundAnimationFromOffsetX = 0.0f;
  state.backgroundAnimationToOffsetX = 0.0f;
  state.backgroundAnimationFromOffsetY = 0.0f;
  state.backgroundAnimationToOffsetY = 0.0f;
  state.backgroundAnimationStartMs = 0;
}
void ResetBackgroundVisualCache(TaskbarState& state) {
  ResetBackgroundVisualTargetCache(state);
  state.backgroundFillIdentity = 0;
  state.lastBackgroundStyleGeneration = 0;
}
struct TaskbarBackgroundCompositionResourcesTai {
  winrt::Windows::UI::Composition::CompositionRoundedRectangleGeometry clipGeometry{nullptr};
  winrt::Windows::UI::Composition::CompositionGeometricClip clip{nullptr};
  winrt::Windows::UI::Composition::ShapeVisual borderVisual{nullptr};
  winrt::Windows::UI::Composition::CompositionRoundedRectangleGeometry borderGeometry{nullptr};
  winrt::Windows::UI::Composition::CompositionSpriteShape borderShape{nullptr};
  winrt::Windows::UI::Composition::CompositionColorBrush borderBrush{nullptr};
};
bool TryGetTaskbarBackgroundCompositionResourcesTai(
    FrameworkElement const& backgroundFillChild,
    winrt::Windows::UI::Composition::Visual const& backgroundFillVisual,
    TaskbarBackgroundCompositionResourcesTai* resources) {
  if (!backgroundFillChild || !backgroundFillVisual || !resources) {
    return false;
  }
  try {
    auto clip = backgroundFillVisual.Clip().try_as<
        winrt::Windows::UI::Composition::CompositionGeometricClip>();
    auto clipGeometry = clip
        ? clip.Geometry().try_as<
              winrt::Windows::UI::Composition::CompositionRoundedRectangleGeometry>()
        : nullptr;
    auto borderVisual =
        winrt::Windows::UI::Xaml::Hosting::ElementCompositionPreview::
            GetElementChildVisual(backgroundFillChild)
                .try_as<winrt::Windows::UI::Composition::ShapeVisual>();
    if (!clipGeometry || !borderVisual || borderVisual.Shapes().Size() != 1) {
      return false;
    }
    auto borderShape = borderVisual.Shapes()
        .GetAt(0)
        .try_as<winrt::Windows::UI::Composition::CompositionSpriteShape>();
    auto borderGeometry = borderShape
        ? borderShape.Geometry().try_as<
              winrt::Windows::UI::Composition::CompositionRoundedRectangleGeometry>()
        : nullptr;
    auto borderBrush = borderShape
        ? borderShape.StrokeBrush().try_as<
              winrt::Windows::UI::Composition::CompositionColorBrush>()
        : nullptr;
    if (!borderShape || !borderGeometry || !borderBrush) {
      return false;
    }
    resources->clipGeometry = clipGeometry;
    resources->clip = clip;
    resources->borderVisual = borderVisual;
    resources->borderGeometry = borderGeometry;
    resources->borderShape = borderShape;
    resources->borderBrush = borderBrush;
    return true;
  } catch (...) {
    return false;
  }
}
bool CreateTaskbarBackgroundCompositionResourcesTai(
    FrameworkElement const& backgroundFillChild,
    winrt::Windows::UI::Composition::Visual const& backgroundFillVisual,
    winrt::Windows::UI::Composition::Compositor const& compositor,
    TaskbarBackgroundCompositionResourcesTai* resources) {
  if (!backgroundFillChild || !backgroundFillVisual || !compositor ||
      !resources) {
    return false;
  }
  try {
    resources->clipGeometry = compositor.CreateRoundedRectangleGeometry();
    resources->clip = compositor.CreateGeometricClip(resources->clipGeometry);
    resources->borderVisual = compositor.CreateShapeVisual();
    resources->borderGeometry = compositor.CreateRoundedRectangleGeometry();
    resources->borderShape =
        compositor.CreateSpriteShape(resources->borderGeometry);
    resources->borderBrush = compositor.CreateColorBrush();
    resources->borderShape.StrokeBrush(resources->borderBrush);
    resources->borderShape.FillBrush(nullptr);
    resources->borderVisual.Shapes().Append(resources->borderShape);
    backgroundFillVisual.Clip(resources->clip);
    winrt::Windows::UI::Xaml::Hosting::ElementCompositionPreview::
        SetElementChildVisual(backgroundFillChild, resources->borderVisual);
    return true;
  } catch (...) {
    *resources = {};
    return false;
  }
}
void ResetAnimationTargetCache(TaskbarState& state) {
  state.hasLastTargetTaskFrameOffsetX = false;
  state.hasLastTargetTaskbarIslandScale = false;
  state.lastTargetTaskbarIslandScale = 1.0f;
  state.lastTaskbarIslandScaleCenterX = 0.0f;
  state.hasLastTargetTrayOffsetX = false;
  state.hasLastTargetWidgetOffset = false;
  state.hasLastStartButtonAnchorRect = false;
  state.hasStableStartButtonAnchorRect = false;
  state.startButtonAnchorStablePasses = 0;
  ResetBackgroundVisualTargetCache(state);
}
bool CheckAndUpdateDisplayGeometrySignature(TaskbarState& state,
                                            FrameworkElement const& xamlRootContent,
                                            FrameworkElement const& taskFrame,
                                            float rasterizationScale) {
  const float rootWidth = static_cast<float>(xamlRootContent.ActualWidth());
  const float rootHeight = static_cast<float>(xamlRootContent.ActualHeight());
  const float taskFrameWidth = taskFrame ? static_cast<float>(taskFrame.ActualWidth()) : 0.0f;
  const float taskFrameHeight = taskFrame ? static_cast<float>(taskFrame.ActualHeight()) : 0.0f;
  const bool validSignature =
      std::isfinite(rootWidth) && rootWidth > 0.0f &&
      std::isfinite(rootHeight) && rootHeight > 0.0f &&
      std::isfinite(rasterizationScale) && rasterizationScale > 0.0f;
  if (!validSignature) {
    return false;
  }
  const bool changed =
      !state.hasLastDisplayGeometrySignature ||
      std::abs(state.lastObservedRootWidth - rootWidth) > kDisplayGeometryChangeToleranceDip ||
      std::abs(state.lastObservedRootHeight - rootHeight) > kDisplayGeometryChangeToleranceDip ||
      std::abs(state.lastObservedRasterizationScale - rasterizationScale) > kDisplayRasterizationChangeTolerance ||
      std::abs(state.lastObservedTaskFrameWidth - taskFrameWidth) > kDisplayGeometryChangeToleranceDip ||
      std::abs(state.lastObservedTaskFrameHeight - taskFrameHeight) > kDisplayGeometryChangeToleranceDip;
  state.lastObservedRootWidth = rootWidth;
  state.lastObservedRootHeight = rootHeight;
  state.lastObservedRasterizationScale = rasterizationScale;
  state.lastObservedTaskFrameWidth = taskFrameWidth;
  state.lastObservedTaskFrameHeight = taskFrameHeight;
  state.hasLastDisplayGeometrySignature = true;
  return changed;
}
void ApplySettings(HWND hTaskbarWnd);
int64_t DelayedApplyNowMs() {
  return std::chrono::duration_cast<std::chrono::milliseconds>(
             std::chrono::steady_clock::now().time_since_epoch())
      .count();
}
int GetCompositionFrameIntervalMs(HWND hwnd) {
  DWM_TIMING_INFO timing{};
  timing.cbSize = sizeof(timing);
  if (SUCCEEDED(DwmGetCompositionTimingInfo(hwnd, &timing)) &&
      timing.rateRefresh.uiNumerator > 0 &&
      timing.rateRefresh.uiDenominator > 0) {
    const double detectedRefreshHz =
        static_cast<double>(timing.rateRefresh.uiNumerator) /
        static_cast<double>(timing.rateRefresh.uiDenominator);
    const double clampedRefreshHz = std::clamp(detectedRefreshHz,
                                               kMinimumTrustedRefreshHz,
                                               kMaximumTrustedRefreshHz);
    return ClampInt(static_cast<int>((1000.0 / clampedRefreshHz) + 0.5),
                    kMinimumFrameIntervalMs,
                    kMaximumFrameIntervalMs);
  }
  return kDefaultFrameIntervalMs;
}
int GetTaskbarIslandFollowupPassCount(HWND hwnd) {
  const int frameIntervalMs = GetCompositionFrameIntervalMs(hwnd);
  const int followupWindowMs =
      kTaskbarIslandAnimationDurationMs +
      (frameIntervalMs * kAnimationFollowupGraceFrames);
  return std::max(1, (followupWindowMs + frameIntervalMs - 1) / frameIntervalMs);
}
void ArmStyleApplyPasses(int passCount, bool resetAnimationTargets = false) {
  passCount = std::max(1, passCount);
  g_force_style_apply_passes.store(passCount);
  g_high_priority_dispatch_passes.store(passCount);
  if (resetAnimationTargets) {
    g_reset_animation_target_passes.store(passCount);
  }
}
void ArmStyleFollowupPasses(HWND hwnd, bool resetAnimationTargets = false) {
  ArmStyleApplyPasses(GetTaskbarIslandFollowupPassCount(hwnd), resetAnimationTargets);
}
void ArmSingleStyleApplyPass(bool resetAnimationTargets = false) {
  ArmStyleApplyPasses(1, resetAnimationTargets);
}
bool WaitForConditionWithTimeout(std::function<bool()> condition,
                                 int timeoutMs,
                                 int pollIntervalMs) {
  const int64_t startMs = DelayedApplyNowMs();
  while (!condition()) {
    if (g_unloading && timeoutMs > 0) {
      // During unload we still wait briefly for workers/hooks to exit, but never
      // spin forever inside code that may be unloaded by recompilation.
    }
    const int64_t elapsedMs = DelayedApplyNowMs() - startMs;
    if (elapsedMs >= timeoutMs) {
      return false;
    }
    Sleep(static_cast<DWORD>(std::max(1, pollIntervalMs)));
  }
  return true;
}
void QueueTaskbarAnimationFollowup(HWND hTaskbarWnd) {
  if (g_unloading || g_worker_threads_stopping.load() ||
      !hTaskbarWnd || !IsWindow(hTaskbarWnd)) {
    return;
  }
  bool expected = false;
  if (!g_animation_followup_worker_running.compare_exchange_strong(expected, true)) {
    return;
  }
  std::lock_guard<std::mutex> lock(g_animation_followup_worker_thread_mutex);
  if (g_unloading || g_worker_threads_stopping.load()) {
    g_animation_followup_worker_running = false;
    return;
  }
  if (g_animation_followup_worker_thread.joinable()) {
    g_animation_followup_worker_thread.join();
  }
  try {
    g_animation_followup_worker_thread = std::thread([hTaskbarWnd]() {
      struct FollowupWorkerGuard {
        ~FollowupWorkerGuard() { g_animation_followup_worker_running = false; }
      } followupWorkerGuard;
      try {
        const int frameIntervalMs = GetCompositionFrameIntervalMs(hTaskbarWnd);
        const int followupWindowMs =
            kTaskbarIslandAnimationDurationMs +
            (frameIntervalMs * kAnimationFollowupGraceFrames);
        int elapsedMs = 0;
        while (elapsedMs < followupWindowMs) {
          Sleep(static_cast<DWORD>(frameIntervalMs));
          elapsedMs += frameIntervalMs;
          if (g_unloading || g_worker_threads_stopping.load() ||
              !hTaskbarWnd || !IsWindow(hTaskbarWnd)) {
            break;
          }
          ArmSingleStyleApplyPass();
          ApplySettings(hTaskbarWnd);
        }
      } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"Animation follow-up worker failed %08X: %s", ex.code(), ex.message().c_str());
      } catch (...) {
        Wh_Log(L"Animation follow-up worker failed: %08X", winrt::to_hresult());
      }
    });
  } catch (std::exception const& ex) {
    g_animation_followup_worker_running = false;
    Wh_Log(L"Failed to create animation follow-up worker: %S", ex.what());
  } catch (...) {
    g_animation_followup_worker_running = false;
    Wh_Log(L"Failed to create animation follow-up worker");
  }
}
void DelayedApplyWorker();
void EnsureDelayedApplyWorker() {
  if (g_unloading || g_worker_threads_stopping.load()) {
    return;
  }
  bool expected = false;
  if (g_delayed_apply_worker_running.compare_exchange_strong(expected, true)) {
    std::lock_guard<std::mutex> lock(g_delayed_apply_worker_thread_mutex);
    if (g_unloading || g_worker_threads_stopping.load()) {
      g_delayed_apply_worker_running = false;
      return;
    }
    if (g_delayed_apply_worker_thread.joinable()) {
      g_delayed_apply_worker_thread.join();
    }
    try {
      g_delayed_apply_worker_thread = std::thread(DelayedApplyWorker);
    } catch (std::exception const& ex) {
      g_delayed_apply_worker_running = false;
      Wh_Log(L"Failed to create delayed apply worker: %S", ex.what());
      return;
    } catch (...) {
      g_delayed_apply_worker_running = false;
      Wh_Log(L"Failed to create delayed apply worker");
      return;
    }
  }
  g_delayed_apply_worker_wake.notify_one();
}
void RequestTaskbarButtonSizeRelayout() {
  if (g_unloading) {
    return;
  }
  // Taskbar buttons are virtualized/recycled. Instead of forcing an arbitrary
  // number of relayout passes, mark button widths as customized and kick one
  // immediate style pass. Every realized/recycled button is validated in the
  // normal taskbar child loop and fixed only if its width is wrong.
  g_taskbarButtonWidthCustomized = true;
  ArmSingleStyleApplyPass(true);
  g_scheduled_low_priority_update = false;
  g_suppress_low_priority_apply_until_ms =
      DelayedApplyNowMs() + kButtonSizeLowPrioritySuppressionMs;
  HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
  if (hTaskbarWnd && IsWindow(hTaskbarWnd)) {
    ApplySettings(hTaskbarWnd);
  }
}
void ArmInitialExplorerStyleApplyDelay() {
  g_initial_style_apply_completed = false;
  g_initial_taskbar_size_apply_done = false;
  g_initial_style_apply_not_before_ms =
      DelayedApplyNowMs() + kInitialExplorerStyleDelayMs;
  Wh_Log(L"Initial Explorer style apply armed");
}
bool InitializeDebounce() {
  // Kept as a compatibility shim for older call sites. The old DispatcherTimer
  // debounce was removed because timer creation/stop could race Explorer/XAML
  // initialization and crash. Scheduling is now handled by DelayedApplyWorker.
  g_worker_threads_stopping = false;
  g_already_requested_debounce_initializing = false;
  return true;
}
void CleanupDebounce() {
  g_worker_threads_stopping = true;
  g_already_requested_debounce_initializing = false;
  g_scheduled_low_priority_update = false;
  g_delayed_apply_due_ms = 0;
  g_delayed_apply_generation.fetch_add(1);
  g_delayed_apply_worker_wake.notify_all();
  std::thread animationFollowupWorker;
  {
    std::lock_guard<std::mutex> lock(g_animation_followup_worker_thread_mutex);
    animationFollowupWorker = std::move(g_animation_followup_worker_thread);
  }
  std::thread delayedApplyWorker;
  {
    std::lock_guard<std::mutex> lock(g_delayed_apply_worker_thread_mutex);
    delayedApplyWorker = std::move(g_delayed_apply_worker_thread);
  }
  if (animationFollowupWorker.joinable()) {
    animationFollowupWorker.join();
  }
  if (delayedApplyWorker.joinable()) {
    delayedApplyWorker.join();
  }
  g_animation_followup_worker_running = false;
  g_delayed_apply_worker_running = false;
}
void DelayedApplyWorker() {
  struct DelayedWorkerGuard {
    ~DelayedWorkerGuard() {
      g_delayed_apply_worker_running = false;
    }
  } delayedWorkerGuard;
  std::unique_lock<std::mutex> waitLock(g_delayed_apply_worker_wait_mutex);
  try {
    for (;;) {
      if (g_unloading || g_worker_threads_stopping.load()) {
        break;
      }
      int64_t dueMs = g_delayed_apply_due_ms.load();
      if (dueMs <= 0) {
        g_delayed_apply_worker_wake.wait(waitLock, [] {
          return g_unloading || g_worker_threads_stopping.load() ||
                 g_delayed_apply_due_ms.load() > 0;
        });
        continue;
      }
      int64_t nowMs = DelayedApplyNowMs();
      if (nowMs < dueMs) {
        g_delayed_apply_worker_wake.wait_for(
            waitLock,
            std::chrono::milliseconds(std::max<int64_t>(1, dueMs - nowMs)),
            [dueMs] {
              return g_unloading || g_worker_threads_stopping.load() ||
                     g_delayed_apply_due_ms.load() != dueMs;
            });
        continue;
      }
      unsigned long long generation = g_delayed_apply_generation.load();
      waitLock.unlock();
      HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
      if (!hTaskbarWnd || !IsWindow(hTaskbarWnd)) {
        Wh_Log(L"Delayed apply postponed: taskbar window is not ready");
        g_delayed_apply_generation.fetch_add(1);
        g_delayed_apply_due_ms = DelayedApplyNowMs() + kInitialExplorerStyleRetryDelayMs;
        waitLock.lock();
        continue;
      }
      g_scheduled_low_priority_update = false;
      Wh_Log(L"Delayed apply triggered");
      const bool initialStyleApply = !g_initial_style_apply_completed.load();
      if (initialStyleApply) {
        ArmStyleFollowupPasses(hTaskbarWnd, true);
      }
      if (!g_initial_style_apply_completed.load() &&
          !g_initial_taskbar_size_apply_done.exchange(true)) {
        ApplySettingsTBIconSize(g_settings_tbiconsize.taskbarHeight);
      }
      ApplySettings(hTaskbarWnd);
      if (!g_initial_style_apply_completed.load() && !g_unloading) {
        Wh_Log(L"Initial ApplyStyle did not complete; retrying delayed apply");
        g_delayed_apply_generation.fetch_add(1);
        g_delayed_apply_due_ms = DelayedApplyNowMs() + kInitialExplorerStyleRetryDelayMs;
        waitLock.lock();
        continue;
      }
      if (g_delayed_apply_generation.load() == generation) {
        int64_t expectedDueMs = dueMs;
        g_delayed_apply_due_ms.compare_exchange_strong(expectedDueMs, 0);
      }
      waitLock.lock();
    }
  } catch (winrt::hresult_error const& ex) {
    Wh_Log(L"Delayed apply worker failed %08X: %s", ex.code(), ex.message().c_str());
  } catch (...) {
    Wh_Log(L"Delayed apply worker failed: %08X", winrt::to_hresult());
  }
}
void ApplySettingsDebounced(int delayMs) {
  if (g_unloading) {
    return;
  }
  if (delayMs <= 0) {
    delayMs = kLowPriorityStyleDelayMs;
  }
  if (delayMs < 1) {
    delayMs = kDefaultStyleDebounceDelayMs;
  }
  int64_t nowMs = DelayedApplyNowMs();
  int64_t dueMs = nowMs + delayMs;
  int64_t initialNotBeforeMs = g_initial_style_apply_not_before_ms.load();
  if (!g_initial_style_apply_completed.load() && initialNotBeforeMs > dueMs) {
    dueMs = initialNotBeforeMs;
  }
  g_delayed_apply_generation.fetch_add(1);
  g_delayed_apply_due_ms = dueMs;
  Wh_Log(L"Scheduled delayed apply in %lld ms", static_cast<long long>(std::max<int64_t>(0, dueMs - nowMs)));
  EnsureDelayedApplyWorker();
}
void ApplySettingsDebounced() {
  ApplySettingsDebounced(kDefaultStyleDebounceDelayMs);
}
void ApplySettingsFromTaskbarThreadImmediately() {
  if (g_unloading) {
    return;
  }
  int64_t initialNotBeforeMs = g_initial_style_apply_not_before_ms.load();
  if (!g_initial_style_apply_completed.load() && initialNotBeforeMs > DelayedApplyNowMs()) {
    ApplySettingsDebounced(1);
    return;
  }
  g_scheduled_low_priority_update = false;
  g_delayed_apply_due_ms = 0;
  g_delayed_apply_generation.fetch_add(1);
  HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
  ArmStyleFollowupPasses(hTaskbarWnd);
  g_suppress_low_priority_apply_until_ms =
      DelayedApplyNowMs() + kGeometryCriticalLowPrioritySuppressionMs;
  if (!hTaskbarWnd || !IsWindow(hTaskbarWnd)) {
    Wh_Log(L"Immediate apply skipped: taskbar window is not ready");
    return;
  }
  Wh_Log(L"Immediate taskbar animation apply");
  ApplySettings(hTaskbarWnd);
  QueueTaskbarAnimationFollowup(hTaskbarWnd);
}
void ApplySettingsFromTaskbarThreadGeometryChanged() {
  if (g_unloading) {
    return;
  }
  int64_t nowMs = DelayedApplyNowMs();
  int64_t initialNotBeforeMs = g_initial_style_apply_not_before_ms.load();
  if (!g_initial_style_apply_completed.load() && initialNotBeforeMs > nowMs) {
    ApplySettingsDebounced(1);
    return;
  }
  HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
  int64_t lastMs = g_last_geometry_critical_apply_ms.load();
  while (nowMs - lastMs >= kGeometryCriticalApplyMinIntervalMs) {
    if (g_last_geometry_critical_apply_ms.compare_exchange_weak(lastMs, nowMs)) {
      g_scheduled_low_priority_update = false;
      ArmStyleFollowupPasses(hTaskbarWnd);
      g_suppress_low_priority_apply_until_ms =
          nowMs + kGeometryCriticalLowPrioritySuppressionMs;
      ApplySettingsDebounced(1);
      if (hTaskbarWnd && IsWindow(hTaskbarWnd)) {
        QueueTaskbarAnimationFollowup(hTaskbarWnd);
      }
      return;
    }
  }
  ArmStyleFollowupPasses(hTaskbarWnd);
}
void ScheduleInitialExplorerStyleApply() {
  ApplySettingsDebounced(kInitialExplorerStyleDelayMs);
}
bool IsWeirdFrameworkElement(winrt::Windows::UI::Xaml::FrameworkElement const& element) {
  if (!element) return true;
  try {
    auto transform = element.TransformToVisual(nullptr);
    winrt::Windows::Foundation::Rect rect = transform.TransformBounds(
        winrt::Windows::Foundation::Rect(0, 0, element.ActualWidth(), element.ActualHeight()));
    return rect.Width <= 0.0 || rect.Height <= 0.0 || rect.X < -kLayoutToleranceDip || rect.Y < -kLayoutToleranceDip;
  } catch (...) {
    // Overflow/recycled taskbar elements can briefly be disconnected from the
    // visual tree while Explorer is rebuilding the task list. Treat them as
    // invalid instead of letting a transient XAML exception take down Explorer.
    return true;
  }
}
bool IsTaskbarWidgetsEnabled() {
    DWORD value = 0;
    DWORD size = sizeof(value);
    HKEY hKey;
    if (RegOpenKeyExW(HKEY_CURRENT_USER,
                      L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced",
                      0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        if (RegQueryValueExW(hKey, L"TaskbarDa", nullptr, nullptr,
                             reinterpret_cast<LPBYTE>(&value), &size) == ERROR_SUCCESS) {
            RegCloseKey(hKey);
            return value == 1;
        }
        RegCloseKey(hKey);
    }
    return false;
}
static float SnapToPhysicalPixel(float value, float rasterizationScale = 1.0f) {
  if (rasterizationScale <= 0.0f) {
    rasterizationScale = 1.0f;
  }
  float scaledValue = value * rasterizationScale;
  float snappedScaledValue =
      (scaledValue >= 0.0f)
          ? static_cast<float>(static_cast<long>(scaledValue + 0.5f))
          : -static_cast<float>(static_cast<long>(-scaledValue + 0.5f));
  return snappedScaledValue / rasterizationScale;
}
static float GetRasterizationScale(FrameworkElement const& element) {
  if (element) {
    auto xamlRoot = element.XamlRoot();
    if (xamlRoot) {
      float rasterizationScale = static_cast<float>(xamlRoot.RasterizationScale());
      if (rasterizationScale > 0.0f) {
        return rasterizationScale;
      }
    }
  }
  return 1.0f;
}
using StartDocked__StartSizingFrame_UpdateWindowRegion_WithArgs_t = void(WINAPI*)(void* pThis, winrt::Windows::Foundation::Size param1);
StartDocked__StartSizingFrame_UpdateWindowRegion_WithArgs_t StartDocked__StartSizingFrame_UpdateWindowRegion_WithArgs_Original;
void WINAPI StartDocked__StartSizingFrame_UpdateWindowRegion_WithArgs_Hook(void* pThis, winrt::Windows::Foundation::Size param1) {
 if (StartDocked__StartSizingFrame_UpdateWindowRegion_WithArgs_Original) {
    StartDocked__StartSizingFrame_UpdateWindowRegion_WithArgs_Original(pThis, param1);
  }
    Wh_Log(L"Method called: StartDocked__StartSizingFrame_UpdateWindowRegion (Width: %.2f, Height: %.2f)", param1.Width, param1.Height);
  const int measuredStartMenuWidth = static_cast<int>(param1.Width + 0.5f);
  if (measuredStartMenuWidth > 0 && g_lastRecordedStartMenuWidth != measuredStartMenuWidth) {
    g_lastRecordedStartMenuWidth = measuredStartMenuWidth;
    Wh_SetIntValue(L"lastRecordedStartMenuWidth", g_lastRecordedStartMenuWidth);
  }
}
std::atomic<int64_t> g_update_flag_set_time_ms = 0;
int64_t NowMs() { return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count(); }
void ResetFlagAfterDelay() {
  std::this_thread::sleep_for(std::chrono::milliseconds(kScheduledLowPriorityFlagTtlMs));
  int64_t now = NowMs();
  int64_t set_time = g_update_flag_set_time_ms.load();
  if (g_scheduled_low_priority_update && (now - set_time >= kScheduledLowPriorityFlagTtlMs)) {
    g_scheduled_low_priority_update = false;
  }
}
void ApplySettingsFromTaskbarThreadIfRequired() {
  if (!g_unloading && DelayedApplyNowMs() < g_suppress_low_priority_apply_until_ms.load()) {
    return;
  }
  if (!g_scheduled_low_priority_update.exchange(true)) {
    g_update_flag_set_time_ms = NowMs();
    Wh_Log(L"Scheduled low priority update");
    ApplySettingsDebounced(-1);
  }
}
void SetDividerForElement(FrameworkElement const& element, float const& panelHeight, bool dividerVisible, bool dividerShouldBeOnLeft = g_settings.userDefinedDividerLeftAligned) {
  if (!element) return;
  if (panelHeight <= 0.0f) return;
  auto visual = winrt::Windows::UI::Xaml::Hosting::ElementCompositionPreview::GetElementVisual(element);
  if (!visual) return;
  auto compositor = visual.Compositor();
  if (!compositor) return;
  auto shapeVisual = compositor.CreateShapeVisual();
  if (!shapeVisual) return;
  dividerVisible = dividerVisible && !g_unloading;
  if (dividerVisible) {
    auto lineGeometry = compositor.CreateLineGeometry();
    if (!lineGeometry) return;
    auto lineShape = compositor.CreateSpriteShape(lineGeometry);
    if (!lineShape) return;
    float borderThicknessFloat = static_cast<float>(g_settings.userDefinedAppsDividerThickness) * 2.0f;
    float scaledHeight = panelHeight * g_settings.userDefinedAppsDividerVerticalScale;
    float yOffset = (panelHeight - scaledHeight) * 0.5f;
    auto size = visual.Size();
    float xOffset = (dividerShouldBeOnLeft) ? 0.0f : (size.x - borderThicknessFloat / 2.0f);
    shapeVisual.Size({borderThicknessFloat, scaledHeight});
    shapeVisual.Offset({xOffset, yOffset, 0.0f});
    lineGeometry.Start({0.0f, 0.0f});
    lineGeometry.End({0.0f, scaledHeight});
    winrt::Windows::UI::Color borderColor = {g_settings.userDefinedTaskbarBorderOpacity, static_cast<BYTE>(g_settings.borderColorR), static_cast<BYTE>(g_settings.borderColorG), static_cast<BYTE>(g_settings.borderColorB)};
    auto strokeBrush = compositor.CreateColorBrush(borderColor);
    if (!strokeBrush) return;
    lineShape.StrokeBrush(strokeBrush);
    lineShape.StrokeThickness(borderThicknessFloat);
    shapeVisual.Shapes().Append(lineShape);
  }
  winrt::Windows::UI::Xaml::Hosting::ElementCompositionPreview::SetElementChildVisual(element, shapeVisual);
}
void ChangeControlCenterIconSize(FrameworkElement const& systemTrayFrameGrid) {
  if (!g_settings.userDefinedStyleTrayArea) return;
  if (auto ControlCenterButton = FindChildByName(systemTrayFrameGrid, L"ControlCenterButton")) {
    if (auto innerGrid = FindChildByClassName(ControlCenterButton, L"Windows.UI.Xaml.Controls.Grid")) {
      if (auto ContentPresenter = FindChildByName(innerGrid, L"ContentPresenter")) {
        if (auto innerItemPresenter = FindChildByClassName(ContentPresenter, L"Windows.UI.Xaml.Controls.ItemsPresenter")) {
          if (auto innerStackPanel = FindChildByClassName(innerItemPresenter, L"Windows.UI.Xaml.Controls.StackPanel")) {
            auto userDefinedTrayIconSizeStr = std::to_wstring(g_settings.userDefinedTrayIconSize);
            int childCount = Media::VisualTreeHelper::GetChildrenCount(innerStackPanel);
            for (int i = 0; i < childCount; ++i) {
              auto child = Media::VisualTreeHelper::GetChild(innerStackPanel, i).try_as<FrameworkElement>();
              if (!child) continue;
              auto SystemTrayIcon = FindChildByName(child, L"SystemTrayIcon");
              if (!SystemTrayIcon) continue;
              auto ContainerGrid = FindChildByName(SystemTrayIcon, L"ContainerGrid");
              if (!ContainerGrid) continue;
              auto ContentGrid = FindChildByName(ContainerGrid, L"ContentGrid");
              if (!ContentGrid) continue;
              auto TextIconContent = FindChildByClassName(ContentGrid, L"SystemTray.TextIconContent");
              if (!TextIconContent) continue;
              auto ContainerGridInner = FindChildByName(TextIconContent, L"ContainerGrid");
              if (!ContainerGridInner) continue;
              if (auto Layer = FindChildByName(ContainerGridInner, L"Underlay")) {
                if (auto InnerTextBlock = FindChildByName(Layer, L"InnerTextBlock")) {
                  SetElementPropertyFromString(InnerTextBlock, L"Windows.UI.Xaml.Controls.TextBlock", L"FontSize", userDefinedTrayIconSizeStr);
                }
              }
              if (auto Layer = FindChildByName(ContainerGridInner, L"Base")) {
                if (auto InnerTextBlock = FindChildByName(Layer, L"InnerTextBlock")) {
                  SetElementPropertyFromString(InnerTextBlock, L"Windows.UI.Xaml.Controls.TextBlock", L"FontSize", userDefinedTrayIconSizeStr);
                }
              }
              if (auto Layer = FindChildByName(ContainerGridInner, L"AccentOverlay")) {
                if (auto InnerTextBlock = FindChildByName(Layer, L"InnerTextBlock")) {
                  SetElementPropertyFromString(InnerTextBlock, L"Windows.UI.Xaml.Controls.TextBlock", L"FontSize", userDefinedTrayIconSizeStr);
                }
              }
            }
          }
        }
      }
    }
  }
}
void ProcessStackPanelChildren(FrameworkElement const& stackPanel, float const& panelHeight) {
  if (!g_settings.userDefinedStyleTrayArea) return;
  auto userDefinedTaskButtonCornerRadius = std::to_wstring(g_settings.userDefinedTaskButtonCornerRadius);
  int childCount = Media::VisualTreeHelper::GetChildrenCount(stackPanel);
  for (int i = 0; i < childCount; ++i) {
    auto child = Media::VisualTreeHelper::GetChild(stackPanel, i).try_as<FrameworkElement>();
    if (!child) continue;
    auto notifyItemIcon = FindChildByName(child, L"NotifyItemIcon");
    if (!notifyItemIcon) continue;
    auto containerGrid = FindChildByName(notifyItemIcon, L"ContainerGrid");
    if (!containerGrid) continue;
    auto innerContentPresenter = FindChildByName(containerGrid, L"ContentPresenter");
    if (!innerContentPresenter) continue;
    auto contentGrid = FindChildByName(innerContentPresenter, L"ContentGrid");
    if (!contentGrid) continue;
    auto imageIconContent = FindChildByClassName(contentGrid, L"SystemTray.ImageIconContent");
    if (!imageIconContent) continue;
    auto innerContainerGrid = FindChildByName(imageIconContent, L"ContainerGrid");
    if (!innerContainerGrid) continue;
    auto image = FindChildByClassName(innerContainerGrid, L"Windows.UI.Xaml.Controls.Image");
    if (!image) continue;
    auto imageCtrl = image.try_as<winrt::Windows::UI::Xaml::Controls::Image>();
    if (!imageCtrl) continue;
    if (g_settings.userDefinedStyleTrayArea) {
      child.Width(g_settings.userDefinedTrayButtonSize);
      child.Height(g_settings.userDefinedTaskbarHeight);
      SetElementPropertyFromString(containerGrid, L"Windows.UI.Xaml.Controls.Grid", L"CornerRadius", userDefinedTaskButtonCornerRadius);
      image.Width(g_settings.userDefinedTrayIconSize);
      image.Height(g_settings.userDefinedTrayIconSize);
    }
  }
}
void StyleNativeDividerElement(winrt::Windows::UI::Xaml::FrameworkElement const& element) {
  if (!element) return;
  using namespace winrt::Windows::UI::Xaml::Hosting;
  using namespace winrt::Windows::Foundation::Numerics;
  element.Opacity(g_unloading ? 1.0f : std::min(1.0f, static_cast<float>(g_settings.userDefinedTaskbarBorderOpacity / 255.0f)));
  element.Width(std::max(0.0, g_settings.userDefinedAppsDividerThickness * 0.99));
  if (auto visual = ElementCompositionPreview::GetElementVisual(element)) {
    if (auto compositor = visual.Compositor()) {
      visual.CenterPoint({0.0f, static_cast<float>(element.ActualHeight()) / 2.0f, 0.0f});
      visual.Scale({1.0f, g_unloading ? 1.0f : g_settings.userDefinedAppsDividerVerticalScale, 1.0f});
    }
  }
  PCWSTR originalHex = Wh_GetStringSetting(L"TaskbarBorderColorHex");
  PCWSTR hex = originalHex;
  if (!hex || *hex == L'\0') {
    hex = L"#ffffff";
  }
  if (*hex == L'#') ++hex;
  std::wstring fillBrush = L"<SolidColorBrush Color=\"#" + std::wstring(hex) + L"\"/>";
  SetElementPropertyFromString(element, L"Windows.UI.Xaml.Shapes.Rectangle", L"Fill", fillBrush.c_str(), true);
  if (originalHex) {
    Wh_FreeStringSetting(originalHex);
  }
}
void InvalidateTaskbarButtonLayoutElement(FrameworkElement const& element) {
  if (!element) {
    return;
  }
  try {
    element.InvalidateMeasure();
    element.InvalidateArrange();
  } catch (...) {
  }
}
double GetEffectiveTaskbarButtonTargetWidth() {
  const double defaultWidth = g_smallIconSize
                                  ? kSystemSmallTaskbarButtonSize
                                  : kSystemMediumTaskbarButtonSize;
  return g_unloading
             ? defaultWidth
             : static_cast<double>(g_smallIconSize
                                       ? g_settings_tbiconsize.taskbarButtonWidthSmall
                                       : g_settings_tbiconsize.taskbarButtonWidth);
}
bool IsExpectedTaskbarButtonDimension(double value, double expected) {
  if (!std::isfinite(value) || value <= 0.0) {
    return false;
  }
  return std::abs(value - expected) <= kLayoutToleranceDip;
}
bool EnsureElementTaskbarButtonWidth(FrameworkElement const& element,
                                     double targetWidth,
                                     bool allowHardWidth) {
  if (!element) {
    return false;
  }
  bool changed = false;
  try {
    if (!IsExpectedTaskbarButtonDimension(element.MinWidth(), targetWidth)) {
      element.MinWidth(targetWidth);
      changed = true;
    }
    if (allowHardWidth) {
      if (!IsExpectedTaskbarButtonDimension(element.MaxWidth(), targetWidth)) {
        element.MaxWidth(targetWidth);
        changed = true;
      }
      const double currentWidth = element.Width();
      const double actualWidth = element.ActualWidth();
      const bool explicitWidthWrong =
          !std::isfinite(currentWidth) ||
          !IsExpectedTaskbarButtonDimension(currentWidth, targetWidth);
      const bool actualWidthWrong =
          actualWidth > 0.0 &&
          !IsExpectedTaskbarButtonDimension(actualWidth, targetWidth);
      if (explicitWidthWrong || actualWidthWrong) {
        element.Width(targetWidth);
        changed = true;
      }
    }
    if (changed) {
      InvalidateTaskbarButtonLayoutElement(element);
    }
  } catch (...) {
  }
  return changed;
}
bool IsStartButtonElement(FrameworkElement const& child,
                          winrt::hstring const& className) {
  if (!child || className != L"Taskbar.ExperienceToggleButton") {
    return false;
  }
  try {
    return Automation::AutomationProperties::GetAutomationId(child) == L"StartButton";
  } catch (...) {
    return false;
  }
}
bool ApplyTaskbarButtonSizeToElement(FrameworkElement const& child,
                                     winrt::hstring const& className) {
  if (!child) {
    return false;
  }
  const bool isTaskbarButton =
      className == L"Taskbar.TaskListButton" ||
      className == L"Taskbar.ExperienceToggleButton" ||
      className == L"Taskbar.OverflowToggleButton" ||
      className == L"Taskbar.SearchBoxButton";
  if (!isTaskbarButton) {
    return false;
  }
  const double targetWidth = static_cast<double>(g_settings.userDefinedTaskbarButtonSize);
  const bool allowHardWidth =
      className != L"Taskbar.SearchBoxButton" ||
      !FindChildByName(child, L"SearchBoxTextBlock");
  bool changed = EnsureElementTaskbarButtonWidth(child, targetWidth, allowHardWidth);
  FrameworkElement innerElementChild = nullptr;
  if (className == L"Taskbar.SearchBoxButton") {
    innerElementChild = FindChildByClassName(child, L"Taskbar.TaskListButtonPanel");
    if (!innerElementChild) {
      innerElementChild = FindChildByName(child, L"SearchBoxButtonRootPanel");
    }
  } else if (className == L"Taskbar.TaskListButton") {
    innerElementChild = FindChildByClassName(child, L"Taskbar.TaskListLabeledButtonPanel");
    if (!innerElementChild) {
      innerElementChild = FindChildByClassName(child, L"Taskbar.TaskListButtonPanel");
    }
  } else if (className == L"Taskbar.ExperienceToggleButton") {
    innerElementChild = FindChildByName(child, L"ExperienceToggleButtonRootPanel");
  } else if (className == L"Taskbar.OverflowToggleButton") {
    innerElementChild = FindChildByName(child, L"OverflowToggleButtonRootPanel");
  } else {
    innerElementChild = FindChildByClassName(child, L"Taskbar.TaskListButtonPanel");
  }
  if (innerElementChild) {
    changed = EnsureElementTaskbarButtonWidth(innerElementChild, targetWidth, allowHardWidth) || changed;
  }
  return changed;
}
struct ChildLayoutObservationTai {
  FrameworkElement element{nullptr};
  winrt::hstring className;
  winrt::Windows::Foundation::Rect rect{};
  winrt::Windows::Foundation::Rect rootRect{};
  std::wstring automationName;
  bool hasValidRect{false};
  bool hasValidRootRect{false};
  bool isStartButton{false};
};
struct ChildrenLayoutMeasurementTai {
  std::vector<ChildLayoutObservationTai> children;
  double capturedContentWidth{0.0};
  double totalWidth{0.0};
  double leftMostEdge{0.0};
  double rightMostEdge{0.0};
  int validChildrenCount{0};
  int visualChildCount{0};
};
ChildrenLayoutMeasurementTai CaptureChildrenLayoutTai(
    FrameworkElement const& element) {
  ChildrenLayoutMeasurementTai measurement;
  if (!element) {
    return measurement;
  }
  int childrenCount = Media::VisualTreeHelper::GetChildrenCount(element);
  measurement.visualChildCount = childrenCount;
  measurement.children.reserve(static_cast<size_t>(std::max(0, childrenCount)));
  for (int i = 0; i < childrenCount; i++) {
    ChildLayoutObservationTai observation;
    observation.element =
        Media::VisualTreeHelper::GetChild(element, i).try_as<FrameworkElement>();
    if (!observation.element) {
      Wh_Log(L"Failed to get child %d of %d", i + 1, childrenCount);
      continue;
    }
    observation.className = winrt::get_class_name(observation.element);
    observation.isStartButton =
        IsStartButtonElement(observation.element, observation.className);
    if (observation.className != L"Taskbar.AugmentedEntryPointButton") {
      const double actualWidth = observation.element.ActualWidth();
      if (std::isfinite(actualWidth) && actualWidth > 0.0) {
        measurement.capturedContentWidth += actualWidth;
      }
    }
    if (observation.className == L"Taskbar.TaskListButton") {
      try {
        auto value = observation.element.GetValue(
            Automation::AutomationProperties::NameProperty());
        observation.automationName =
            winrt::unbox_value_or<winrt::hstring>(value, L"").c_str();
      } catch (...) {
        observation.automationName.clear();
      }
    }
    measurement.children.push_back(std::move(observation));
  }
  return measurement;
}
bool IsValidChildLayoutRectTai(
    winrt::Windows::Foundation::Rect const& rect) {
  return rect.Width > 0.0 &&
         rect.Height > 0.0 &&
         std::isfinite(rect.X) &&
         std::isfinite(rect.Y) &&
         std::isfinite(rect.Width) &&
         std::isfinite(rect.Height) &&
         rect.X >= -kLayoutToleranceDip &&
         rect.Y >= -kLayoutToleranceDip;
}
void RefreshChildrenLayoutMeasurementTai(
    FrameworkElement const& boundsRelativeTo,
    FrameworkElement const& rootBoundsRelativeTo,
    ChildrenLayoutMeasurementTai* measurement) {
  if (!boundsRelativeTo || !measurement) {
    return;
  }
  measurement->totalWidth = 0.0;
  measurement->leftMostEdge = 0.0;
  measurement->rightMostEdge = 0.0;
  measurement->validChildrenCount = 0;
  double minEdge = std::numeric_limits<double>::infinity();
  double maxEdge = -std::numeric_limits<double>::infinity();
  for (auto& observation : measurement->children) {
    observation.hasValidRect = false;
    observation.hasValidRootRect = false;
    try {
      auto transform =
          observation.element.TransformToVisual(boundsRelativeTo);
      observation.rect = transform.TransformBounds(
          winrt::Windows::Foundation::Rect(
              0,
              0,
              observation.element.ActualWidth(),
              observation.element.ActualHeight()));
      observation.hasValidRect = IsValidChildLayoutRectTai(observation.rect);
    } catch (...) {
      observation.hasValidRect = false;
    }
    if (rootBoundsRelativeTo) {
      try {
        auto transform =
            observation.element.TransformToVisual(rootBoundsRelativeTo);
        observation.rootRect = transform.TransformBounds(
            winrt::Windows::Foundation::Rect(
                0,
                0,
                observation.element.ActualWidth(),
                observation.element.ActualHeight()));
        observation.hasValidRootRect =
            IsValidChildLayoutRectTai(observation.rootRect);
      } catch (...) {
        observation.hasValidRootRect = false;
      }
    }
    if (observation.hasValidRect &&
        observation.className != L"Taskbar.AugmentedEntryPointButton") {
      measurement->totalWidth += observation.rect.Width;
      minEdge = std::min(minEdge, static_cast<double>(observation.rect.X));
      maxEdge = std::max(
          maxEdge,
          static_cast<double>(observation.rect.X + observation.rect.Width));
      measurement->validChildrenCount++;
    }
  }
  if (measurement->validChildrenCount > 0 &&
      minEdge != std::numeric_limits<double>::infinity()) {
    measurement->leftMostEdge = minEdge;
    measurement->rightMostEdge = maxEdge;
  }
}
ChildrenLayoutMeasurementTai MeasureValidChildren(
    FrameworkElement const& element,
    FrameworkElement const& boundsRelativeTo = nullptr) {
  auto measurement = CaptureChildrenLayoutTai(element);
  RefreshChildrenLayoutMeasurementTai(
      boundsRelativeTo ? boundsRelativeTo : element,
      nullptr,
      &measurement);
  return measurement;
}
FrameworkElement FindCapturedChildByClassNameTai(
    ChildrenLayoutMeasurementTai const& measurement,
    PCWSTR className) {
  for (const auto& observation : measurement.children) {
    if (observation.className == className) {
      return observation.element;
    }
  }
  return nullptr;
}
const ChildLayoutObservationTai* FindCapturedStartButtonTai(
    ChildrenLayoutMeasurementTai const& measurement) {
  for (const auto& observation : measurement.children) {
    if (observation.isStartButton) {
      return &observation;
    }
  }
  return nullptr;
}
uint64_t AppendChildStyleSignatureTai(
    uint64_t signature,
    std::wstring_view value) {
  constexpr uint64_t kFnvPrime = 1099511628211ULL;
  for (wchar_t ch : value) {
    signature ^= static_cast<uint64_t>(ch);
    signature *= kFnvPrime;
  }
  return signature;
}
uint64_t GetChildStyleSignatureTai(
    ChildrenLayoutMeasurementTai const& measurement) {
  constexpr uint64_t kFnvOffsetBasis = 14695981039346656037ULL;
  constexpr uint64_t kFnvPrime = 1099511628211ULL;
  uint64_t signature = kFnvOffsetBasis;
  for (const auto& child : measurement.children) {
    signature ^= static_cast<uint64_t>(
        reinterpret_cast<uintptr_t>(winrt::get_abi(child.element)));
    signature *= kFnvPrime;
    signature = AppendChildStyleSignatureTai(
        signature,
        std::wstring_view(child.className.c_str(), child.className.size()));
    signature = AppendChildStyleSignatureTai(signature, child.automationName);
    signature ^= child.hasValidRect ? 1ULL : 0ULL;
    signature *= kFnvPrime;
  }
  signature ^= static_cast<uint64_t>(measurement.children.size());
  signature *= kFnvPrime;
  return signature;
}
bool ApplyTaskbarButtonSizing(
    ChildrenLayoutMeasurementTai const& measurement) {
  bool layoutChanged = false;
  for (const auto& child : measurement.children) {
    layoutChanged =
        ApplyTaskbarButtonSizeToElement(child.element, child.className) ||
        layoutChanged;
  }
  return layoutChanged;
}
void ApplyMeasuredChildStyles(
    ChildrenLayoutMeasurementTai const& measurement) {
  const float tbHeightFloat = static_cast<float>(g_settings.userDefinedTaskbarHeight);
  auto userDefinedTaskButtonCornerRadius = std::to_wstring(g_settings.userDefinedTaskButtonCornerRadius);
  for (const auto& observation : measurement.children) {
    if (!observation.hasValidRect) {
      continue;
    }
    auto const& child = observation.element;
    auto const& className = observation.className;
    SetElementPropertyFromString(
        child,
        className.c_str(),
        L"CornerRadius",
        userDefinedTaskButtonCornerRadius);
    if (className == L"Taskbar.SearchBoxButton") {
      // Search only needs the common sizing helper above.
    } else if (className == L"Taskbar.TaskListButton") {
      auto innerElementChild = FindChildByClassName(child, L"Taskbar.TaskListLabeledButtonPanel");
      if (innerElementChild) {
        auto iconElementChild = FindChildByName(innerElementChild, L"Icon");
        if (iconElementChild) {
          iconElementChild.Width(g_settings.userDefinedTaskbarIconSize);
          iconElementChild.Height(g_settings.userDefinedTaskbarIconSize);
          SetDividerForElement(child, tbHeightFloat, false);
          if (!observation.automationName.empty()) {
            for (const auto& pattern :
                 g_settings.compiledDividedAppPatterns) {
              if (MatchesDividedAppPatternTai(
                      observation.automationName,
                      pattern)) {
                SetDividerForElement(child, tbHeightFloat, true);
                break;
              }
            }
          }
        }
      }
    } else if (className == L"Taskbar.AugmentedEntryPointButton") {  // widget element
      child.Margin(Thickness{0, 0, 0, 0});
      auto ExperienceToggleButtonRootPanelElement = FindChildByName(child, L"ExperienceToggleButtonRootPanel");
      if (ExperienceToggleButtonRootPanelElement) {
        ExperienceToggleButtonRootPanelElement.Margin(Thickness{0, 0, 0, 0});
      }
      continue;
    } else if (className == L"Taskbar.OverflowToggleButton") {  // overflow button
      if (auto OverflowToggleButtonRootPanel = FindChildByName(child, L"OverflowToggleButtonRootPanel")) {
        if (auto RightOverflowButtonDivider = FindChildByName(OverflowToggleButtonRootPanel, L"RightOverflowButtonDivider")) {
          if (g_settings.userDefinedTrayAreaDivider) {
            RightOverflowButtonDivider.Opacity(0);
          } else {
            StyleNativeDividerElement(RightOverflowButtonDivider);
          }
        }
      }
    }
    if (auto iconPanelElement = FindChildByName(child, L"IconPanel")) {
      if (auto mostRecentlyUsedDivider = FindChildByName(iconPanelElement, L"MostRecentlyUsedDivider")) {
        StyleNativeDividerElement(mostRecentlyUsedDivider);
      }
      if (auto progressIndicator = FindChildByName(iconPanelElement, L"ProgressIndicator")) {
        if (auto layoutRoot = FindChildByName(progressIndicator, L"LayoutRoot")) {
          if (auto progressBarRoot = FindChildByName(layoutRoot, L"ProgressBarRoot")) {
            if (auto border = FindChildByClassName(progressBarRoot, L"Windows.UI.Xaml.Controls.Border")) {
              if (auto grid = FindChildByClassName(border, L"Windows.UI.Xaml.Controls.Grid")) {
                grid.Height(3.8);
                if (auto progressBarTrack = FindChildByName(grid, L"ProgressBarTrack")) {
                  progressBarTrack.Opacity(0.5);
                }
              }
            }
          }
        }
      } else if (auto runningIndicator = FindChildByName(iconPanelElement, L"RunningIndicator")) {
        runningIndicator.Height(3.5);
        runningIndicator.Opacity(1);
      }
    }
  }
}
void ApplyChildStylesIfRequired(
    FrameworkElement const& container,
    FrameworkElement const& rootBoundsRelativeTo,
    ChildrenLayoutMeasurementTai* measurement,
    TaskbarChildStyleCache* cache,
    uint64_t styleGeneration) {
  if (!container || !measurement || !cache) {
    return;
  }
  uint64_t signature = GetChildStyleSignatureTai(*measurement);
  if (cache->valid &&
      cache->generation == styleGeneration &&
      cache->signature == signature) {
    return;
  }
  if (ApplyTaskbarButtonSizing(*measurement)) {
    container.UpdateLayout();
    *measurement = CaptureChildrenLayoutTai(container);
    RefreshChildrenLayoutMeasurementTai(
        container,
        rootBoundsRelativeTo,
        measurement);
    signature = GetChildStyleSignatureTai(*measurement);
  }
  ApplyMeasuredChildStyles(*measurement);
  cache->generation = styleGeneration;
  cache->signature = signature;
  cache->valid = true;
}
void DisableElementClip(FrameworkElement const& element) {
  if (!element) {
    return;
  }
  try {
    element.Clip(nullptr);
  } catch (...) {
  }
}
void DisableClipForAncestorChain(FrameworkElement element, int maxDepth = 8) {
  for (int i = 0; element && i < maxDepth; ++i) {
    DisableElementClip(element);
    try {
      element = Media::VisualTreeHelper::GetParent(element).try_as<FrameworkElement>();
    } catch (...) {
      break;
    }
  }
}
bool SetVirtualLayoutWidth(FrameworkElement const& element, double width) {
  if (!element || !std::isfinite(width) || width <= 0.0) {
    return false;
  }
  bool changed = false;
  try {
    if (!IsExpectedTaskbarButtonDimension(element.Width(), width)) {
      element.Width(width);
      changed = true;
    }
    if (!IsExpectedTaskbarButtonDimension(element.MinWidth(), width)) {
      element.MinWidth(width);
      changed = true;
    }
    if (!IsExpectedTaskbarButtonDimension(element.MaxWidth(), width)) {
      element.MaxWidth(width);
      changed = true;
    }
    element.HorizontalAlignment(HorizontalAlignment::Left);
    DisableElementClip(element);
    if (changed) {
      element.InvalidateMeasure();
      element.InvalidateArrange();
    }
  } catch (...) {
  }
  return changed;
}
double CalculateDynamicTaskbarVirtualSurfaceWidth(int visualChildCount,
                                                  double capturedContentWidth,
                                                  double rootWidth,
                                                  double rasterizationScale) {
  if (g_unloading ||
      !std::isfinite(rootWidth) ||
      rootWidth <= 0.0 ||
      !std::isfinite(rasterizationScale) ||
      rasterizationScale <= 0.0) {
    return rootWidth;
  }
  const double buttonSize = std::max<double>(
      1.0, static_cast<double>(g_settings.userDefinedTaskbarButtonSize));
  const double childCountEstimate =
      static_cast<double>(std::max(visualChildCount, 1)) * buttonSize;
  const double measuredContentWidth =
      std::isfinite(capturedContentWidth) && capturedContentWidth > 0.0
          ? capturedContentWidth
          : 0.0;
  const double requiredContentWidth =
      std::max(measuredContentWidth, childCountEstimate);
  // Keep one monitor width free so ItemsRepeater can realize newly added or
  // previously overflowed buttons on subsequent layout passes.
  const double requestedVirtualWidth = requiredContentWidth + rootWidth;
  // Packed HSHELL_GETMINRECT coordinates use signed 16-bit components. Limit
  // the virtual surface to half that physical span so centering and scaling
  // retain coordinate headroom on either side of the island.
  const double coordinateSafeWidth =
      kTaskbarVirtualSurfaceMaxPhysicalWidth / rasterizationScale;
  const double maximumVirtualWidth =
      std::max(rootWidth, coordinateSafeWidth);
  const double virtualWidth =
      std::clamp(requestedVirtualWidth, rootWidth, maximumVirtualWidth);
  return std::isfinite(virtualWidth) && virtualWidth > rootWidth
      ? virtualWidth
      : rootWidth;
}
void ApplyVirtualTaskbarLayoutSurface(FrameworkElement const& xamlRootContent,
                                      FrameworkElement const& taskFrame,
                                      FrameworkElement const& rootGridTaskBar,
                                      FrameworkElement const& taskbarFrameRepeater,
                                      FrameworkElement const& taskbarBackground,
                                      FrameworkElement const& backgroundFillParent,
                                      FrameworkElement const& backgroundFillChild,
                                      double virtualWidth) {
  if (!std::isfinite(virtualWidth) || virtualWidth <= 0.0) {
    return;
  }
  // Keep the physical taskbar frame at the monitor width. Only the taskbar
  // content surface is virtualized. This gives ItemsRepeater more measure space
  // without making the whole taskbar window logically wider.
  DisableElementClip(xamlRootContent);
  DisableElementClip(taskFrame);
  DisableClipForAncestorChain(rootGridTaskBar);
  SetVirtualLayoutWidth(rootGridTaskBar, virtualWidth);
  SetVirtualLayoutWidth(taskbarFrameRepeater, virtualWidth);
  SetVirtualLayoutWidth(taskbarBackground, virtualWidth);
  SetVirtualLayoutWidth(backgroundFillParent, virtualWidth);
  SetVirtualLayoutWidth(backgroundFillChild, virtualWidth);
}
void SetTaskbarOverflowButtonSuppressed(FrameworkElement const& overflowButton,
                                        bool suppress,
                                        TaskbarState* state) {
  if (!state) {
    return;
  }
  if (!overflowButton) {
    state->lastOverflowButtonIdentity = 0;
    state->overflowButtonSuppressionKnown = false;
    return;
  }
  try {
    const uintptr_t overflowButtonIdentity =
        reinterpret_cast<uintptr_t>(winrt::get_abi(overflowButton));
    const bool suppressionTransition =
        !state->overflowButtonSuppressionKnown ||
        state->lastOverflowButtonIdentity != overflowButtonIdentity ||
        state->overflowButtonSuppressed != suppress;
    if (suppress) {
      overflowButton.Opacity(0.0);
      overflowButton.IsHitTestVisible(false);
      overflowButton.MinWidth(0.0);
      overflowButton.MaxWidth(0.0);
      overflowButton.Width(0.0);
      overflowButton.Clip(nullptr);
      if (suppressionTransition) {
        overflowButton.InvalidateMeasure();
        overflowButton.InvalidateArrange();
        overflowButton.UpdateLayout();
      }
    } else {
      overflowButton.Opacity(1.0);
      overflowButton.IsHitTestVisible(true);
      overflowButton.ClearValue(FrameworkElement::WidthProperty());
      overflowButton.ClearValue(FrameworkElement::MinWidthProperty());
      overflowButton.ClearValue(FrameworkElement::MaxWidthProperty());
    }
    state->lastOverflowButtonIdentity = overflowButtonIdentity;
    state->overflowButtonSuppressionKnown = true;
    state->overflowButtonSuppressed = suppress;
  } catch (...) {
  }
}
// ---- Temporary taskbar geometry diagnostics -------------------------------
// Set to false after collecting logs. The throttle keeps ApplyStyle from
// flooding Windhawk logs during insertion/removal animations.
constexpr bool kDebugTaskbarGeometry = true;
constexpr int64_t kDebugTaskbarGeometryMinIntervalMs = 250;
std::atomic<int64_t> g_lastTaskbarGeometryDebugLogMs = 0;
bool ShouldLogTaskbarGeometry(bool interesting) {
  if constexpr (!kDebugTaskbarGeometry) {
    return false;
  }
  if (!interesting) {
    return false;
  }
  const int64_t nowMs = DelayedApplyNowMs();
  int64_t lastMs = g_lastTaskbarGeometryDebugLogMs.load();
  while (nowMs - lastMs >= kDebugTaskbarGeometryMinIntervalMs) {
    if (g_lastTaskbarGeometryDebugLogMs.compare_exchange_weak(lastMs, nowMs)) {
      return true;
    }
  }
  return false;
}
bool TryGetDebugBoundsRelativeTo(FrameworkElement const& element,
                                 winrt::Windows::UI::Xaml::UIElement const& relativeTo,
                                 winrt::Windows::Foundation::Rect& rect) {
  if (!element) {
    return false;
  }
  try {
    auto transform = element.TransformToVisual(relativeTo);
    rect = transform.TransformBounds(
        winrt::Windows::Foundation::Rect(0, 0, element.ActualWidth(), element.ActualHeight()));
    return std::isfinite(rect.X) &&
           std::isfinite(rect.Y) &&
           std::isfinite(rect.Width) &&
           std::isfinite(rect.Height);
  } catch (...) {
    return false;
  }
}
static bool BuildMeasuredMinimizeAnimationButtonsTai(
    ChildrenLayoutMeasurementTai const& taskbarChildrenMeasurement,
    const RECT& monitorRect,
    double targetTaskRootOffsetXDip,
    double targetTaskbarIslandScale,
    double targetScaleCenterScreenXDip,
    double rasterizationScale,
    std::vector<MinimizeAnimationMeasuredButtonTai>* measuredButtons) {
  if (!measuredButtons ||
      IsRectEmpty(&monitorRect) ||
      !std::isfinite(targetTaskRootOffsetXDip) ||
      !std::isfinite(targetTaskbarIslandScale) ||
      !std::isfinite(targetScaleCenterScreenXDip) ||
      !std::isfinite(rasterizationScale) ||
      rasterizationScale <= 0.0) {
    return false;
  }
  measuredButtons->clear();
  measuredButtons->reserve(taskbarChildrenMeasurement.children.size());
  for (const auto& observation : taskbarChildrenMeasurement.children) {
    if (observation.className != L"Taskbar.TaskListButton" ||
        !observation.hasValidRootRect) {
      continue;
    }
    auto const& layoutRect = observation.rootRect;
    const double unscaledLeftDip = layoutRect.X + targetTaskRootOffsetXDip;
    const double unscaledRightDip = layoutRect.X + layoutRect.Width + targetTaskRootOffsetXDip;
    const double visibleLeftDip = ApplyScaleToScreenX(
        static_cast<float>(unscaledLeftDip),
        static_cast<float>(targetScaleCenterScreenXDip),
        static_cast<float>(targetTaskbarIslandScale));
    const double visibleRightDip = ApplyScaleToScreenX(
        static_cast<float>(unscaledRightDip),
        static_cast<float>(targetScaleCenterScreenXDip),
        static_cast<float>(targetTaskbarIslandScale));
    if (!std::isfinite(visibleLeftDip) || !std::isfinite(visibleRightDip)) {
      continue;
    }
    RECT visibleRectPx{};
    visibleRectPx.left = monitorRect.left + static_cast<LONG>(std::lround(std::min(visibleLeftDip, visibleRightDip) * rasterizationScale));
    visibleRectPx.right = monitorRect.left + static_cast<LONG>(std::lround(std::max(visibleLeftDip, visibleRightDip) * rasterizationScale));
    // Preserve only the measured X interval. The shell min-rect already carries
    // the correct bottom-taskbar Y coordinates; root-relative XAML Y values are
    // not in the same coordinate space on every monitor/taskbar edge.
    visibleRectPx.top = 1;
    visibleRectPx.bottom = 2;
    if (!IsUsableMeasuredButtonRectTai(visibleRectPx)) {
      continue;
    }
    measuredButtons->push_back(MinimizeAnimationMeasuredButtonTai{
        visibleRectPx,
        TrimTai(observation.automationName)});
  }
  return !measuredButtons->empty();
}
static void UpdateMinimizeAnimationCorrectionForMonitorTai(
    const std::wstring& monitorName,
    ChildrenLayoutMeasurementTai const& taskbarChildrenMeasurement,
    bool useVirtualTaskbarSurface,
    double targetTaskRootOffsetXDip,
    double targetTaskbarIslandScale,
    double targetScaleCenterScreenXDip,
    double rasterizationScale,
    double scaledBackgroundLeftScreenDip,
    double scaledBackgroundRightScreenDip) {
  if (g_unloading ||
      (!useVirtualTaskbarSurface &&
       targetTaskbarIslandScale >= static_cast<double>(0.999f))) {
    ClearMinimizeAnimationCorrectionForMonitorTai(monitorName);
    return;
  }
  RECT monitorRect{};
  RECT taskbarClampRect{};
  const bool haveMonitorRect = GetMonitorRectByNameTai(monitorName, &monitorRect);
  if (haveMonitorRect) {
    taskbarClampRect.left = monitorRect.left +
        static_cast<LONG>(std::lround(scaledBackgroundLeftScreenDip * rasterizationScale));
    taskbarClampRect.right = monitorRect.left +
        static_cast<LONG>(std::lround(scaledBackgroundRightScreenDip * rasterizationScale));
    taskbarClampRect.top = monitorRect.top;
    taskbarClampRect.bottom = monitorRect.bottom;
    if (taskbarClampRect.right <= taskbarClampRect.left) {
      taskbarClampRect = monitorRect;
    }
  }
  std::vector<MinimizeAnimationMeasuredButtonTai> measuredButtons;
  if (haveMonitorRect) {
    BuildMeasuredMinimizeAnimationButtonsTai(
        taskbarChildrenMeasurement,
        monitorRect,
        targetTaskRootOffsetXDip,
        targetTaskbarIslandScale,
        targetScaleCenterScreenXDip,
        rasterizationScale,
        &measuredButtons);
  }
  // The transform remains the fallback for shell payloads that can't be
  // matched to a freshly measured taskbar button.
  SetMinimizeAnimationCorrectionForMonitorTai(
      monitorName,
      monitorRect,
      targetTaskRootOffsetXDip,
      targetTaskbarIslandScale,
      targetScaleCenterScreenXDip,
      rasterizationScale,
      haveMonitorRect ? &taskbarClampRect : nullptr,
      std::move(measuredButtons));
}
void LogDebugRect(PCWSTR label, bool ok, winrt::Windows::Foundation::Rect const& rect) {
  if (!ok) {
    Wh_Log(L"    %-18s unavailable", label);
    return;
  }
  Wh_Log(L"    %-18s X=%8.2f Y=%8.2f W=%8.2f H=%8.2f R=%8.2f B=%8.2f",
         label,
         rect.X,
         rect.Y,
         rect.Width,
         rect.Height,
         rect.X + rect.Width,
         rect.Y + rect.Height);
}
void LogElementGeometry(PCWSTR tag,
                        FrameworkElement const& element,
                        FrameworkElement const& rootGridTaskBar,
                        FrameworkElement const& taskbarFrameRepeater,
                        FrameworkElement const& taskFrame) {
  if (!element) {
    Wh_Log(L"[TBGEOM] %s: <null>", tag);
    return;
  }
  std::wstring className;
  std::wstring name;
  std::wstring automationId;
  try { className = std::wstring(winrt::get_class_name(element).c_str()); } catch (...) { className = L"<class?>"; }
  try { name = std::wstring(element.Name().c_str()); } catch (...) { name = L""; }
  try { automationId = std::wstring(Automation::AutomationProperties::GetAutomationId(element).c_str()); } catch (...) { automationId = L""; }
  auto parent = Media::VisualTreeHelper::GetParent(element).try_as<FrameworkElement>();
  std::wstring parentClass;
  std::wstring parentName;
  if (parent) {
    try { parentClass = std::wstring(winrt::get_class_name(parent).c_str()); } catch (...) { parentClass = L"<class?>"; }
    try { parentName = std::wstring(parent.Name().c_str()); } catch (...) { parentName = L""; }
  } else {
    parentClass = L"<null>";
  }
  Thickness margin{};
  winrt::Windows::Foundation::Numerics::float3 actualOffset{};
  try { margin = element.Margin(); } catch (...) {}
  try { actualOffset = element.ActualOffset(); } catch (...) {}
  winrt::Windows::Foundation::Numerics::float3 visualOffset{};
  winrt::Windows::Foundation::Numerics::float2 visualSize{};
  if (auto visual = winrt::Windows::UI::Xaml::Hosting::ElementCompositionPreview::GetElementVisual(element)) {
    visualOffset = visual.Offset();
    visualSize = visual.Size();
  }
  Wh_Log(L"[TBGEOM] %s class=%s name=%s aid=%s parent=%s/%s",
         tag,
         className.c_str(),
         name.c_str(),
         automationId.c_str(),
         parentClass.c_str(),
         parentName.c_str());
  Wh_Log(L"    size Actual=%8.2fx%-8.2f W=%8.2f H=%8.2f MinW=%8.2f MaxW=%8.2f Margin=(%.2f,%.2f,%.2f,%.2f)",
         element.ActualWidth(),
         element.ActualHeight(),
         element.Width(),
         element.Height(),
         element.MinWidth(),
         element.MaxWidth(),
         margin.Left,
         margin.Top,
         margin.Right,
         margin.Bottom);
  Wh_Log(L"    ActualOffset=(%.2f,%.2f,%.2f) VisualOffset=(%.2f,%.2f,%.2f) VisualSize=(%.2f,%.2f)",
         actualOffset.x,
         actualOffset.y,
         actualOffset.z,
         visualOffset.x,
         visualOffset.y,
         visualOffset.z,
         visualSize.x,
         visualSize.y);
  winrt::Windows::Foundation::Rect rect{};
  LogDebugRect(L"to-parent", parent ? TryGetDebugBoundsRelativeTo(element, parent, rect) : false, rect);
  LogDebugRect(L"to-repeater", taskbarFrameRepeater ? TryGetDebugBoundsRelativeTo(element, taskbarFrameRepeater, rect) : false, rect);
  LogDebugRect(L"to-rootGrid", rootGridTaskBar ? TryGetDebugBoundsRelativeTo(element, rootGridTaskBar, rect) : false, rect);
  LogDebugRect(L"to-taskFrame", taskFrame ? TryGetDebugBoundsRelativeTo(element, taskFrame, rect) : false, rect);
  LogDebugRect(L"to-xamlRoot", TryGetDebugBoundsRelativeTo(element, winrt::Windows::UI::Xaml::UIElement{nullptr}, rect), rect);
}
void LogAncestorGeometryChain(PCWSTR tag,
                              FrameworkElement element,
                              FrameworkElement const& rootGridTaskBar,
                              int maxDepth = 8) {
  Wh_Log(L"[TBGEOM] ancestor-chain for %s", tag);
  for (int depth = 0; element && depth < maxDepth; ++depth) {
    std::wstring className;
    std::wstring name;
    try { className = std::wstring(winrt::get_class_name(element).c_str()); } catch (...) { className = L"<class?>"; }
    try { name = std::wstring(element.Name().c_str()); } catch (...) { name = L""; }
    Thickness margin{};
    winrt::Windows::Foundation::Numerics::float3 actualOffset{};
    winrt::Windows::Foundation::Numerics::float3 visualOffset{};
    try { margin = element.Margin(); } catch (...) {}
    try { actualOffset = element.ActualOffset(); } catch (...) {}
    if (auto visual = winrt::Windows::UI::Xaml::Hosting::ElementCompositionPreview::GetElementVisual(element)) {
      visualOffset = visual.Offset();
    }
    winrt::Windows::Foundation::Rect rect{};
    const bool okRoot = rootGridTaskBar && TryGetDebugBoundsRelativeTo(element, rootGridTaskBar, rect);
    Wh_Log(L"    #%d class=%s name=%s Actual=%6.2fx%-6.2f ActualOffset=(%.2f,%.2f,%.2f) VisualOffset=(%.2f,%.2f,%.2f) Margin=(%.2f,%.2f,%.2f,%.2f) rootRect=%s X=%.2f Y=%.2f W=%.2f H=%.2f",
           depth,
           className.c_str(),
           name.c_str(),
           element.ActualWidth(),
           element.ActualHeight(),
           actualOffset.x,
           actualOffset.y,
           actualOffset.z,
           visualOffset.x,
           visualOffset.y,
           visualOffset.z,
           margin.Left,
           margin.Top,
           margin.Right,
           margin.Bottom,
           okRoot ? L"ok" : L"--",
           okRoot ? rect.X : 0.0,
           okRoot ? rect.Y : 0.0,
           okRoot ? rect.Width : 0.0,
           okRoot ? rect.Height : 0.0);
    element = Media::VisualTreeHelper::GetParent(element).try_as<FrameworkElement>();
  }
}
void LogTaskbarGeometryProbe(PCWSTR reason,
                             std::wstring const& monitorName,
                             FrameworkElement const& xamlRootContent,
                             FrameworkElement const& taskFrame,
                             FrameworkElement const& rootGridTaskBar,
                             FrameworkElement const& taskbarFrameRepeater,
                             FrameworkElement const& trayFrame,
                             FrameworkElement const& systemTrayFrameGrid,
                             FrameworkElement const& taskbarBackground,
                             FrameworkElement const& backgroundFillChild,
                             FrameworkElement const& startButton,
                             double rootWidth,
                             double childrenWidthTaskbarDbl,
                             double taskbarLeftEdge,
                             double taskbarRightEdge,
                             double startButtonLeft,
                             double startButtonTop,
                             double startButtonWidth,
                             double startButtonHeight,
                             double predictedCenteredTaskbarLeft,
                             bool isOverflowing,
                             bool taskbarLayoutIsEdgeClamped,
                             bool useStableStartButtonAnchor,
                             float targetContentLeft,
                             float targetContentWidth,
                             float targetTaskRootOffsetX,
                             float targetOffsetXTray,
                             float targetBackgroundLeftScreen,
                             float targetBackgroundRightScreen,
                             TaskbarState const& state) {
  Wh_Log(L"[TBGEOM] ===== %s monitor=%s overflow=%d edgeClamp=%d useStableStart=%d =====",
         reason,
         monitorName.c_str(),
         isOverflowing ? 1 : 0,
         taskbarLayoutIsEdgeClamped ? 1 : 0,
         useStableStartButtonAnchor ? 1 : 0);
  Wh_Log(L"[TBGEOM] scalars rootWidth=%.2f taskChildren=%.2f measuredEdges=[%.2f..%.2f] predictedLeft=%.2f",
         rootWidth,
         childrenWidthTaskbarDbl,
         taskbarLeftEdge,
         taskbarRightEdge,
         predictedCenteredTaskbarLeft);
  Wh_Log(L"[TBGEOM] startSample X=%.2f Y=%.2f W=%.2f H=%.2f stable=%d stableRect X=%.2f Y=%.2f W=%.2f H=%.2f stablePasses=%d",
         startButtonLeft,
         startButtonTop,
         startButtonWidth,
         startButtonHeight,
         state.hasStableStartButtonAnchorRect ? 1 : 0,
         state.stableStartButtonAnchorLeft,
         state.stableStartButtonAnchorTop,
         state.stableStartButtonAnchorWidth,
         state.stableStartButtonAnchorHeight,
         state.startButtonAnchorStablePasses);
  Wh_Log(L"[TBGEOM] targets contentLeft=%.2f contentWidth=%.2f rootOffsetX=%.2f trayOffsetX=%.2f bgScreen=[%.2f..%.2f]",
         targetContentLeft,
         targetContentWidth,
         targetTaskRootOffsetX,
         targetOffsetXTray,
         targetBackgroundLeftScreen,
         targetBackgroundRightScreen);
  LogElementGeometry(L"xamlRootContent", xamlRootContent, rootGridTaskBar, taskbarFrameRepeater, taskFrame);
  LogElementGeometry(L"taskFrame", taskFrame, rootGridTaskBar, taskbarFrameRepeater, taskFrame);
  LogElementGeometry(L"rootGridTaskBar", rootGridTaskBar, rootGridTaskBar, taskbarFrameRepeater, taskFrame);
  LogElementGeometry(L"repeater", taskbarFrameRepeater, rootGridTaskBar, taskbarFrameRepeater, taskFrame);
  LogElementGeometry(L"startButton", startButton, rootGridTaskBar, taskbarFrameRepeater, taskFrame);
  LogElementGeometry(L"trayFrame", trayFrame, rootGridTaskBar, taskbarFrameRepeater, taskFrame);
  LogElementGeometry(L"trayGrid", systemTrayFrameGrid, rootGridTaskBar, taskbarFrameRepeater, taskFrame);
  LogElementGeometry(L"taskbarBackground", taskbarBackground, rootGridTaskBar, taskbarFrameRepeater, taskFrame);
  LogElementGeometry(L"backgroundFill", backgroundFillChild, rootGridTaskBar, taskbarFrameRepeater, taskFrame);
  LogAncestorGeometryChain(L"startButton", startButton, rootGridTaskBar);
  LogAncestorGeometryChain(L"repeater", taskbarFrameRepeater, rootGridTaskBar);
  Wh_Log(L"[TBGEOM] ===== end =====");
}
void UpdateGlobalSettings() {
  std::lock_guard<std::recursive_mutex> settingsLock(g_settingsMutex);
  auto getInt = [&](PCWSTR key) { return Wh_GetIntSetting(key); };
  auto clamp = [](int v, int lo, int hi) { return v < lo ? lo : v > hi ? hi : v; };
  // Booleans
  g_settings.userDefinedFlatTaskbarBottomCorners = (getInt(L"FlatTaskbarBottomCorners") != 0);
  g_settings.userDefinedFullWidthTaskbarBackground = (getInt(L"FullWidthTaskbarBackground") != 0) || g_unloading;
  if (g_settings.userDefinedFullWidthTaskbarBackground) g_settings.userDefinedFlatTaskbarBottomCorners = true;
  g_settings.userDefinedIgnoreShowDesktopButton = (getInt(L"IgnoreShowDesktopButton") != 0);
  g_settings.userDefinedTrayAreaDivider = (getInt(L"TrayAreaDivider") != 0) && !g_unloading;
  g_settings.userDefinedStyleTrayArea = (getInt(L"StyleTrayArea") != 0);
  g_settings.userDefinedAlignFlyoutInner = (getInt(L"AlignFlyoutInner") != 0);
  g_settings.userDefinedCustomizeTaskbarBackground = (getInt(L"CustomizeTaskbarBackground") != 0);
  g_settings.userDefinedDisableCustomBlurBackground = (getInt(L"DisableCustomBlurBackground") != 0);
  PCWSTR appsDividerAlignment = Wh_GetStringSetting(L"AppsDividerAlignment");
  g_settings.userDefinedDividerLeftAligned =
      appsDividerAlignment && _wcsicmp(appsDividerAlignment, L"left") == 0;
  if (appsDividerAlignment) {
    Wh_FreeStringSetting(appsDividerAlignment);
  }
  // Gaps & Padding (non-negative)
  g_settings.userDefinedTrayTaskGap = g_unloading ? 0 : std::max(0, getInt(L"TrayTaskGap"));
  g_settings.userDefinedTaskbarBackgroundHorizontalPadding = g_unloading ? 0 : std::max(0, getInt(L"TaskbarBackgroundHorizontalPadding"));
  // Offset Y (negative up; non-positive settings sit on the screen edge)
  const int offsetY = std::max(-2, getInt(L"TaskbarOffsetY"));
  g_settings.userDefinedTaskbarOffsetY =
      (g_unloading || g_settings.userDefinedFlatTaskbarBottomCorners)
          ? 0
          : -offsetY;
  // Height & Sizes
  int h = ClampInt(abs(ReadPositiveIntSettingOrDefault(L"TaskbarHeight", kDefaultTaskbarHeight)),
                   kMinTaskbarHeight,
                   kMaxTaskbarHeight);
  g_settings.userDefinedTaskbarHeight = g_unloading ? kSystemMediumTaskbarButtonSize : h;
  int taskbarButtonSize = ClampInt(abs(ReadPositiveIntSettingOrDefault(L"TaskbarButtonSize", kDefaultTaskbarButtonSize)),
                                   kMinTaskbarButtonSize,
                                   kMaxTaskbarButtonSize);
  g_settings.userDefinedTaskbarButtonSize = g_unloading ? kSystemMediumTaskbarButtonSize : taskbarButtonSize;
  const int maxTaskbarIconSize =
      GetMaxTaskbarIconSizeForLayout(static_cast<int>(g_settings.userDefinedTaskbarHeight),
                                     static_cast<int>(g_settings.userDefinedTaskbarButtonSize));
  int taskbarIconSize = ClampInt(abs(ReadPositiveIntSettingOrDefault(L"TaskbarIconSize", kDefaultTaskbarIconSize)),
                                 kMinTaskbarIconSize,
                                 maxTaskbarIconSize);
  g_settings.userDefinedTaskbarIconSize = g_unloading ? kSystemMediumTaskbarIconSize : taskbarIconSize;
  g_settings.userDefinedTrayIconSize = std::max(kMinTrayIconSize, getInt(L"TrayIconSize"));
  g_settings.userDefinedTrayButtonSize = std::max(kMinTrayButtonSize, getInt(L"TrayButtonSize"));
  // Corner radii
  float tcr = float(fmax(0.0f, getInt(L"TaskbarCornerRadius")));
  tcr = fmin(tcr, g_settings.userDefinedTaskbarHeight / 2.0f);
  g_settings.userDefinedTaskbarCornerRadius = g_unloading ? 0.0f : tcr;
  int btnCr = clamp(abs(getInt(L"TaskButtonCornerRadius")), 0, g_settings.userDefinedTaskbarHeight / 2);
  g_settings.userDefinedTaskButtonCornerRadius = g_unloading ? 0 : btnCr;
  // Opacities & tints (0–100)
  int bgOp = clamp(abs(getInt(L"TaskbarBackgroundOpacity")), 0, 100);
  g_settings.userDefinedTaskbarBackgroundOpacity = bgOp;
  g_settings.userDefinedTaskbarBackgroundTint = clamp(abs(getInt(L"TaskbarBackgroundTint")), 0, 100);
  g_settings.userDefinedTaskbarBackgroundLuminosity = clamp(abs(getInt(L"TaskbarBackgroundLuminosity")), 0, 100);
  g_settings.userDefinedTaskbarBackgroundBlurAmount = clamp(abs(getInt(L"TaskbarBackgroundBlurAmount")), 0, 100);
  g_settings.userDefinedTaskbarBackgroundTintSaturation = clamp(abs(getInt(L"TaskbarBackgroundTintSaturation")), 0, 500);
  g_settings.userDefinedTaskbarBackgroundInversion = clamp(abs(getInt(L"TaskbarBackgroundInversion")), 0, 100);
  PCWSTR bgTintColor = Wh_GetStringSetting(L"TaskbarBackgroundTintColor");
  g_settings.userDefinedTaskbarBackgroundTintColor = (bgTintColor && *bgTintColor) ? bgTintColor : L"{ThemeResource CardStrokeColorDefaultSolid}";
  if (bgTintColor) {
    Wh_FreeStringSetting(bgTintColor);
  }
  PCWSTR bgFallbackColor = Wh_GetStringSetting(L"TaskbarBackgroundFallbackColor");
  g_settings.userDefinedTaskbarBackgroundFallbackColor = (bgFallbackColor && *bgFallbackColor) ? bgFallbackColor : L"{ThemeResource CardStrokeColorDefaultSolid}";
  if (bgFallbackColor) {
    Wh_FreeStringSetting(bgFallbackColor);
  }
  // Border opacity: 0–255
  int bOp = clamp(abs(getInt(L"TaskbarBorderOpacity")), 0, 100);
  g_settings.userDefinedTaskbarBorderOpacity = uint8_t(round(bOp * 2.55f));
  // Border thickness: 0.0–10.0 (10% of [0–100])
  g_settings.userDefinedTaskbarBorderThickness = g_unloading ? 0.0f : (clamp(abs(getInt(L"TaskbarBorderThickness")), 0, 100) * 0.1f);
  g_settings.userDefinedAppsDividerThickness = g_unloading ? 0.0f : (clamp(abs(getInt(L"AppsDividerThickness")), 0, 100) * 0.1f);
  g_settings.userDefinedAppsDividerVerticalScale = g_unloading ? 0.0f : (clamp(abs(getInt(L"AppsDividerVerticalScale")), 0, 100) / 100.0f);
  // Border color
  PCWSTR originalHex = Wh_GetStringSetting(L"TaskbarBorderColorHex");
  PCWSTR hex = originalHex;
  if (!hex || *hex == L'\0') {
    hex = L"#ffffff";
  }
  if (*hex == L'#') ++hex;
  unsigned int r = 255, g = 255, b = 255;
  if (swscanf_s(hex, L"%02x%02x%02x", &r, &g, &b) != 3) {
    r = g = b = 255;
  }
  g_settings.borderColorR = r;
  g_settings.borderColorG = g;
  g_settings.borderColorB = b;
  if (originalHex) {
    Wh_FreeStringSetting(originalHex);
  }
  // String list
  PCWSTR dividerAppNames = Wh_GetStringSetting(L"DividedAppNames");
  CompileDividedAppPatternsTai(SplitAndTrim(dividerAppNames));
  if (dividerAppNames) {
    Wh_FreeStringSetting(dividerAppNames);
  }
}
bool HasInvalidSettings() {
  std::lock_guard<std::recursive_mutex> settingsLock(g_settingsMutex);
  if (g_settings.userDefinedTrayTaskGap < 0) return true;
  if (g_settings.userDefinedTaskbarBackgroundHorizontalPadding < 0) return true;
  if ((int)g_settings.userDefinedTaskbarOffsetY < -2 && !g_settings.userDefinedFlatTaskbarBottomCorners) return true;
  if (g_settings.userDefinedTaskbarHeight < kMinTaskbarHeight || g_settings.userDefinedTaskbarHeight > kMaxTaskbarHeight) return true;
  if (g_settings.userDefinedTaskbarIconSize <= 0) return true;
  if (g_settings.userDefinedTrayIconSize <= 0) return true;
  if (g_settings.userDefinedTaskbarButtonSize <= 0) return true;
  if (g_settings.userDefinedTrayButtonSize <= 0) return true;
  if (g_settings.userDefinedTaskbarCornerRadius < 0.0f || g_settings.userDefinedTaskbarCornerRadius > (g_settings.userDefinedTaskbarHeight / 2.0f)) return true;
  if (g_settings.userDefinedTaskButtonCornerRadius < 0 || g_settings.userDefinedTaskButtonCornerRadius > (g_settings.userDefinedTaskbarHeight / 2)) return true;
  if (g_settings.userDefinedTaskbarBackgroundOpacity > 100) return true;
  if (g_settings.userDefinedTaskbarBackgroundTint > 100) return true;
  if (g_settings.userDefinedTaskbarBackgroundLuminosity > 100) return true;
  if (g_settings.userDefinedTaskbarBackgroundBlurAmount > 100) return true;
  if (g_settings.userDefinedTaskbarBackgroundTintSaturation > 500) return true;
  if (g_settings.userDefinedTaskbarBackgroundInversion > 100) return true;
  if (g_settings.userDefinedTaskbarBorderOpacity > 255) return true;
  if (g_settings.userDefinedTaskbarBorderThickness < 0.0 || g_settings.userDefinedTaskbarBorderThickness > 10.0) return true;
  return false;
}
void LogAllSettings() {
  std::lock_guard<std::recursive_mutex> settingsLock(g_settingsMutex);
  Wh_Log(L"setting %d %s", g_settings.userDefinedTrayTaskGap, L"userDefinedTrayTaskGap");
  Wh_Log(L"setting %d %s", g_settings.userDefinedTaskbarBackgroundHorizontalPadding, L"userDefinedTaskbarBackgroundHorizontalPadding");
  Wh_Log(L"setting %d %s", g_settings.userDefinedTaskbarOffsetY, L"userDefinedTaskbarOffsetY");
  Wh_Log(L"setting %d %s", g_settings.userDefinedTaskbarHeight, L"userDefinedTaskbarHeight");
  Wh_Log(L"setting %d %s", g_settings.userDefinedTaskbarIconSize, L"userDefinedTaskbarIconSize");
  Wh_Log(L"setting %d %s", g_settings.userDefinedTrayIconSize, L"userDefinedTrayIconSize");
  Wh_Log(L"setting %d %s", g_settings.userDefinedTaskbarButtonSize, L"userDefinedTaskbarButtonSize");
  Wh_Log(L"setting %d %s", g_settings.userDefinedTrayButtonSize, L"userDefinedTrayButtonSize");
  Wh_Log(L"setting %d %s", (int)g_settings.userDefinedTaskbarCornerRadius, L"userDefinedTaskbarCornerRadius");
  Wh_Log(L"setting %d %s", g_settings.userDefinedTaskButtonCornerRadius, L"userDefinedTaskButtonCornerRadius");
  Wh_Log(L"setting %d %s", g_settings.userDefinedFlatTaskbarBottomCorners ? 1 : 0, L"userDefinedFlatTaskbarBottomCorners");
  Wh_Log(L"setting %d %s", g_settings.userDefinedTaskbarBackgroundOpacity, L"userDefinedTaskbarBackgroundOpacity");
  Wh_Log(L"setting %d %s", g_settings.userDefinedTaskbarBackgroundTint, L"userDefinedTaskbarBackgroundTint");
  Wh_Log(L"setting %d %s", g_settings.userDefinedTaskbarBackgroundLuminosity, L"userDefinedTaskbarBackgroundLuminosity");
  Wh_Log(L"setting %d %s", g_settings.userDefinedTaskbarBackgroundBlurAmount, L"userDefinedTaskbarBackgroundBlurAmount");
  Wh_Log(L"setting %s %s", g_settings.userDefinedTaskbarBackgroundTintColor.c_str(), L"userDefinedTaskbarBackgroundTintColor");
  Wh_Log(L"setting %d %s", g_settings.userDefinedTaskbarBackgroundTintSaturation, L"userDefinedTaskbarBackgroundTintSaturation");
  Wh_Log(L"setting %d %s", g_settings.userDefinedTaskbarBackgroundInversion, L"userDefinedTaskbarBackgroundInversion");
  Wh_Log(L"setting %s %s", g_settings.userDefinedTaskbarBackgroundFallbackColor.c_str(), L"userDefinedTaskbarBackgroundFallbackColor");
  Wh_Log(L"setting %d %s", g_settings.userDefinedTaskbarBorderOpacity, L"userDefinedTaskbarBorderOpacity");
  Wh_Log(L"setting %d %s", (int)(g_settings.userDefinedTaskbarBorderThickness * 100.0 / 10.0), L"userDefinedTaskbarBorderThickness (scaled)");
  Wh_Log(L"setting %d %s", g_settings.userDefinedFullWidthTaskbarBackground ? 1 : 0, L"userDefinedFullWidthTaskbarBackground");
  Wh_Log(L"setting %d %s", g_settings.userDefinedIgnoreShowDesktopButton ? 1 : 0, L"userDefinedIgnoreShowDesktopButton");
  Wh_Log(L"setting %d %s", g_settings.userDefinedStyleTrayArea ? 1 : 0, L"userDefinedStyleTrayArea");
  Wh_Log(L"setting %d %s", g_settings.userDefinedTrayAreaDivider ? 1 : 0, L"userDefinedTrayAreaDivider");
  Wh_Log(L"setting %d %s", g_settings.borderColorR, L"borderColorR");
  Wh_Log(L"setting %d %s", g_settings.borderColorG, L"borderColorG");
  Wh_Log(L"setting %d %s", g_settings.borderColorB, L"borderColorB");
  Wh_Log(L"setting %d %s", g_settings.userDefinedCustomizeTaskbarBackground ? 1 : 0, L"userDefinedCustomizeTaskbarBackground");
  Wh_Log(L"setting %d %s", g_settings.userDefinedDisableCustomBlurBackground ? 1 : 0, L"userDefinedDisableCustomBlurBackground");
}
bool ApplyStyle(FrameworkElement const& xamlRootContent, std::wstring monitorName) {
  if (!xamlRootContent) {
    Wh_Log(L"xamlRootContent is null");
    return false;
  }
  try {
  std::lock_guard<std::recursive_mutex> settingsLock(g_settingsMutex);
  auto stateHandle = GetOrCreateTaskbarState(monitorName);
  if (!stateHandle) {
    Wh_Log(L"Failed to get taskbar state for monitor: %s", monitorName.c_str());
    return false;
  }
  std::lock_guard<std::recursive_mutex> stateLock(stateHandle->mutex);
  auto& state = *stateHandle;
  Wh_Log(L"ApplyStyle for monitor: %s", monitorName.c_str());
  g_scheduled_low_priority_update = false;
  bool forceStyleApply = false;
  int forceStyleApplyPasses = g_force_style_apply_passes.load();
  while (forceStyleApplyPasses > 0) {
    if (g_force_style_apply_passes.compare_exchange_weak(forceStyleApplyPasses, forceStyleApplyPasses - 1)) {
      forceStyleApply = true;
      break;
    }
  }
  bool resetAnimationTargetsThisPass = false;
  int resetAnimationTargetPasses = g_reset_animation_target_passes.load();
  while (resetAnimationTargetPasses > 0) {
    if (g_reset_animation_target_passes.compare_exchange_weak(resetAnimationTargetPasses, resetAnimationTargetPasses - 1)) {
      resetAnimationTargetsThisPass = true;
      forceStyleApply = true;
      break;
    }
  }
  if (resetAnimationTargetsThisPass) {
    ResetAnimationTargetCache(state);
  }
  const float rasterizationScale = GetRasterizationScale(xamlRootContent);
  auto snapPx = [rasterizationScale](float value) -> float {
    return SnapToPhysicalPixel(value, rasterizationScale);
  };
  auto taskFrame = FindChildByClassName(xamlRootContent, L"Taskbar.TaskbarFrame");
  if (!taskFrame) {
    Wh_Log(L"Failed to find Taskbar.TaskbarFrame");
    return false;
  }
   const bool displayGeometryChanged = CheckAndUpdateDisplayGeometrySignature(
      state, xamlRootContent, taskFrame, rasterizationScale);
  if (displayGeometryChanged && !g_unloading) {
    Wh_Log(L"Taskbar display geometry changed; invalidating cached layout targets");
    ResetAnimationTargetCache(state);
    forceStyleApply = true;
    RequestTaskbarDimensionInvalidation();
    if (HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd()) {
      ArmStyleFollowupPasses(hTaskbarWnd, true);
    }
  }
  auto now = std::chrono::steady_clock::now();
  if (!forceStyleApply && !g_unloading && now - state.lastApplyStyleTime < std::chrono::milliseconds(200)) {
    return true;
  }
  state.lastApplyStyleTime = now;
  auto rootGridTaskBar = FindChildByName(taskFrame, L"RootGrid");
  if (!rootGridTaskBar) {
    Wh_Log(L"Failed to find RootGrid in taskFrame");
    return false;
  }
  auto taskbarFrameRepeater = FindChildByName(rootGridTaskBar, L"TaskbarFrameRepeater");
  if (!taskbarFrameRepeater) {
    Wh_Log(L"Failed to find TaskbarFrameRepeater in rootGridTaskBar");
    return false;
  }
  auto trayFrame = FindChildByClassName(xamlRootContent, L"SystemTray.SystemTrayFrame");
  if (!trayFrame) {
    Wh_Log(L"Failed to find SystemTray.SystemTrayFrame");
    return false;
  }
  auto systemTrayFrameGrid = FindChildByName(trayFrame, L"SystemTrayFrameGrid");
  if (!systemTrayFrameGrid) {
    Wh_Log(L"Failed to find SystemTrayFrameGrid in trayFrame");
    return false;
  }
  auto showDesktopButton = FindChildByName(systemTrayFrameGrid, L"ShowDesktopStack");
  if (!showDesktopButton) {
    Wh_Log(L"Failed to find ShowDesktopStack in systemTrayFrameGrid");
    return false;
  }
  auto taskbarBackground = FindChildByClassName(rootGridTaskBar, L"Taskbar.TaskbarBackground");
  if (!taskbarBackground) {
    Wh_Log(L"Failed to find Taskbar.TaskbarBackground in rootGridTaskBar");
    return false;
  }
  auto backgroundFillParent = FindChildByClassName(taskbarBackground, L"Windows.UI.Xaml.Controls.Grid");
  if (!backgroundFillParent) {
    Wh_Log(L"Failed to find backgroundFillParent in taskbarBackground");
    return false;
  }
  auto backgroundFillChild = FindChildByName(backgroundFillParent, L"BackgroundFill");
  if (!backgroundFillChild) {
    Wh_Log(L"Failed to find BackgroundFill in backgroundFillParent");
    return false;
  }
  auto notificationAreaIcons = FindChildByName(systemTrayFrameGrid, L"NotificationAreaIcons");
  if (!notificationAreaIcons) {
    Wh_Log(L"Failed to find NotificationAreaIcons in systemTrayFrameGrid");
    return false;
  }
  auto itemsPresenter = FindChildByClassName(notificationAreaIcons, L"Windows.UI.Xaml.Controls.ItemsPresenter");
  if (!itemsPresenter) {
    Wh_Log(L"Failed to find ItemsPresenter in notificationAreaIcons");
    return false;
  }
  auto stackPanel = FindChildByClassName(itemsPresenter, L"Windows.UI.Xaml.Controls.StackPanel");
  if (!stackPanel) {
    Wh_Log(L"Failed to find StackPanel in itemsPresenter");
    return false;
  }
  auto taskbarChildrenMeasurement =
      CaptureChildrenLayoutTai(taskbarFrameRepeater);
  bool widgetPresent = IsTaskbarWidgetsEnabled();
  auto widgetElement = widgetPresent
      ? FindCapturedChildByClassNameTai(
            taskbarChildrenMeasurement,
            L"Taskbar.AugmentedEntryPointButton")
      : nullptr;
  auto widgetMainView = widgetElement ? FindChildByName(widgetElement, L"ExperienceToggleButtonRootPanel") : widgetElement;
  widgetPresent = widgetPresent && widgetMainView != nullptr;
  auto widgetElementWidth = widgetPresent && widgetMainView ? widgetMainView.ActualWidth() : 0;
  if (widgetPresent && widgetElementWidth <= 0) {
    Wh_Log(L"Error: widgetPresent && widgetElementWidth<=0");
    return false;
  }
  auto widgetElementInnerChild = widgetPresent ? FindChildByClassName(widgetElement, L"Taskbar.TaskListButtonPanel") : nullptr;
  auto widgetElementVisibleWidth = widgetElementInnerChild ? widgetElementInnerChild.ActualWidth() : 0;
  auto widgetElementVisibleHeight = widgetElementInnerChild ? widgetElementInnerChild.ActualHeight() : 0;
  if (widgetElementInnerChild && widgetElementVisibleWidth <= 0) {
    Wh_Log(L"Error: widgetElementInnerChild && widgetElementVisibleWidth<=0");
    return false;
  }
  if (widgetElementInnerChild && widgetElementVisibleHeight <= 0) {
    Wh_Log(L"Error: widgetElementInnerChild && widgetElementVisibleHeight<=0");
    return false;
  }
  auto overflowButton = FindCapturedChildByClassNameTai(
      taskbarChildrenMeasurement,
      L"Taskbar.OverflowToggleButton");
  bool isOverflowing = overflowButton != nullptr && !IsWeirdFrameworkElement(overflowButton);
  double rootWidth = xamlRootContent.ActualWidth();
  state.lastRootWidth = static_cast<float>(rootWidth);
  const double minimumRootWidth =
      std::max<double>(1.0, static_cast<double>(g_settings.userDefinedTaskbarButtonSize));
  if (!g_unloading && rootWidth < minimumRootWidth) {
    Wh_Log(L"root width is too small");
    return false;
  }
   const double taskbarVirtualSurfaceWidth = g_unloading
      ? rootWidth
      : CalculateDynamicTaskbarVirtualSurfaceWidth(
            taskbarChildrenMeasurement.visualChildCount,
            taskbarChildrenMeasurement.capturedContentWidth,
            rootWidth,
            rasterizationScale);
  const bool useVirtualTaskbarSurface =
      !g_unloading &&
      std::isfinite(taskbarVirtualSurfaceWidth) &&
      taskbarVirtualSurfaceWidth > rootWidth + kLayoutToleranceDip;
  if (useVirtualTaskbarSurface) {
    ApplyVirtualTaskbarLayoutSurface(xamlRootContent,
                                     taskFrame,
                                     rootGridTaskBar,
                                     taskbarFrameRepeater,
                                     taskbarBackground,
                                     backgroundFillParent,
                                     backgroundFillChild,
                                     taskbarVirtualSurfaceWidth);
    SetTaskbarOverflowButtonSuppressed(overflowButton, true, &state);
    isOverflowing = false;
  } else {
    SetTaskbarOverflowButtonSuppressed(overflowButton, false, &state);
  }
  const double taskbarLayoutSurfaceWidth = useVirtualTaskbarSurface
      ? std::max(rootWidth, taskbarFrameRepeater.ActualWidth())
      : rootWidth;
  int childrenCountTaskbar = 0;
  double taskbarLeftEdge = 0.0;
  double taskbarRightEdge = 0.0;
  double startButtonLeft = 0.0;
  double startButtonTop = 0.0;
  double startButtonWidth = 0.0;
  double startButtonHeight = 0.0;
  const uint64_t childStyleGeneration =
      g_taskbarChildStyleGeneration.load(std::memory_order_acquire);
  RefreshChildrenLayoutMeasurementTai(
      taskbarFrameRepeater,
      rootGridTaskBar,
      &taskbarChildrenMeasurement);
  ApplyChildStylesIfRequired(
      taskbarFrameRepeater,
      rootGridTaskBar,
      &taskbarChildrenMeasurement,
      &state.taskbarChildStyleCache,
      childStyleGeneration);
  const double childrenWidthTaskbarDbl =
      taskbarChildrenMeasurement.totalWidth;
  childrenCountTaskbar = taskbarChildrenMeasurement.validChildrenCount;
  taskbarLeftEdge = taskbarChildrenMeasurement.leftMostEdge;
  taskbarRightEdge = taskbarChildrenMeasurement.rightMostEdge;
  winrt::Windows::Foundation::Rect startButtonAnchorRect{};
  auto startButtonObservation =
      FindCapturedStartButtonTai(taskbarChildrenMeasurement);
  auto startButtonElement = startButtonObservation
      ? startButtonObservation->element
      : nullptr;
  if (startButtonObservation && startButtonObservation->hasValidRootRect) {
    startButtonAnchorRect = startButtonObservation->rootRect;
    startButtonLeft = startButtonAnchorRect.X;
    startButtonTop = startButtonAnchorRect.Y;
    startButtonWidth = startButtonAnchorRect.Width;
    startButtonHeight = startButtonAnchorRect.Height;
    state.lastStartButtonXActual = static_cast<float>(startButtonAnchorRect.X);
  }
  if (!g_unloading && childrenWidthTaskbarDbl <= 0) {
    Wh_Log(L"Error: childrenWidthTaskbarDbl <= 0");
    return false;
  }
  double actualTaskbarBoundsWidthDbl = taskbarRightEdge - taskbarLeftEdge;
  if (actualTaskbarBoundsWidthDbl <= 0.0) {
    actualTaskbarBoundsWidthDbl = childrenWidthTaskbarDbl;
    taskbarLeftEdge = (rootWidth - actualTaskbarBoundsWidthDbl) / 2.0;
    taskbarRightEdge = taskbarLeftEdge + actualTaskbarBoundsWidthDbl;
  }
  // Use the sum of valid child widths as the logical task width. The measured
  // min/max bounds are still useful as an anchor near overflow, but during
  // insert/remove animations they can include temporary gaps from recycled
  // views and make the island width overshoot.
  unsigned int childrenWidthTaskbar = static_cast<unsigned int>(childrenWidthTaskbarDbl + 0.5);
  signed int rightMostEdgeTaskbar = static_cast<signed int>(taskbarRightEdge + 0.5);
  if (!g_unloading && childrenCountTaskbar < 1) {
    Wh_Log(L"Error: childrenCountTaskbar < 1");
    return false;
  }
  const unsigned int minimumTaskbarChildrenWidth =
      static_cast<unsigned int>(std::max<double>(1.0, g_settings.userDefinedTaskbarButtonSize * 0.25));
  if (!g_unloading && childrenWidthTaskbar <= minimumTaskbarChildrenWidth) {
    Wh_Log(L"Error: childrenWidthTaskbar is too small");
    return false;
  }
  if (!g_unloading && rightMostEdgeTaskbar < 0) {
    Wh_Log(L"Error: rightMostEdgeTaskbar < 0");
    return false;
  }
  bool rightMostEdgeChangedTaskbar = (state.lastTaskbarData.rightMostEdge != rightMostEdgeTaskbar);
  if (isOverflowing != state.wasOverflowing) {
    ResetAnimationTargetCache(state);
    forceStyleApply = true;
  }
  if (rightMostEdgeChangedTaskbar || state.lastTaskbarData.rightMostEdge == 0.0 || isOverflowing) {
    state.lastTaskbarData.childrenCount = childrenCountTaskbar;
    state.lastTaskbarData.rightMostEdge = rightMostEdgeTaskbar;
    state.lastTaskbarData.childrenWidth = childrenWidthTaskbar;
  }
  trayFrame.Clip(nullptr);
    DisableElementClip(systemTrayFrameGrid);
  DisableClipForAncestorChain(systemTrayFrameGrid);
  auto trayHorizontalAlignmentRef = trayFrame
      .GetValue(FrameworkElement::HorizontalAlignmentProperty())
      .try_as<winrt::Windows::Foundation::IReference<HorizontalAlignment>>();
  if (trayHorizontalAlignmentRef && trayHorizontalAlignmentRef.Value() == HorizontalAlignment::Center) {
    trayFrame.SetValue(FrameworkElement::HorizontalAlignmentProperty(), winrt::box_value(HorizontalAlignment::Right));
  }
  int childrenCountTray = 0;
  auto trayChildrenMeasurement =
      MeasureValidChildren(systemTrayFrameGrid);
  ApplyChildStylesIfRequired(
      systemTrayFrameGrid,
      nullptr,
      &trayChildrenMeasurement,
      &state.trayChildStyleCache,
      childStyleGeneration);
  double trayFrameWidthDbl = trayChildrenMeasurement.totalWidth;
  childrenCountTray = trayChildrenMeasurement.validChildrenCount;
  if (!g_unloading && trayFrameWidthDbl <= 0) {
    Wh_Log(L"Error: trayFrameWidthDbl <= 0");
    return false;
  }
  if (!g_unloading && childrenCountTray <= 0) {
    Wh_Log(L"Error: childrenCountTray <= 0");
    return false;
  }
  const double showDesktopButtonActualWidth = showDesktopButton ? showDesktopButton.ActualWidth() : 0.0;
  if (g_settings.userDefinedIgnoreShowDesktopButton && showDesktopButtonActualWidth > 0.0) {
    trayFrameWidthDbl = std::max(0.0, trayFrameWidthDbl - showDesktopButtonActualWidth);
  }
  int trayGapPlusExtras = g_settings.userDefinedTrayTaskGap + widgetElementVisibleWidth + (widgetPresent ? -6 + g_settings.userDefinedTrayTaskGap : 0);
  const double trayFrameWidthSafeDbl = std::max(0.0, trayFrameWidthDbl + static_cast<double>(trayGapPlusExtras));
  const unsigned int trayFrameWidth = static_cast<unsigned int>(trayFrameWidthSafeDbl + 0.5);
  if (!g_unloading && childrenCountTray == 0) {
    Wh_Log(L"Error: childrenCountTray == 0");
    return false;
  }
  if (!g_unloading && trayFrameWidth <= 1) {
    Wh_Log(L"Error: trayFrameWidth <= 1");
    return false;
  }
  const float targetContentWidth = static_cast<float>(childrenWidthTaskbar + trayFrameWidth);
  const float targetContentLeft = snapPx(static_cast<float>((rootWidth - targetContentWidth) / 2.0f));
  const float targetTrayLogicalLeft = snapPx(targetContentLeft + static_cast<float>(childrenWidthTaskbar));
  float targetOffsetXTray = snapPx(targetTrayLogicalLeft - static_cast<float>(rootWidth - trayFrameWidth));
  // tray animations
  auto systemTrayFrameGridVisual = winrt::Windows::UI::Xaml::Hosting::ElementCompositionPreview::GetElementVisual(systemTrayFrameGrid);
  if (!systemTrayFrameGridVisual) {
    Wh_Log(L"Error: !SystemTrayFrameGridVisual");
    return false;
  }
  auto originalOffset = systemTrayFrameGridVisual.Offset();
  if (state.initOffsetX == -1) {
    state.initOffsetX = originalOffset.x;
  }
  auto rootGridTaskBarVisual = winrt::Windows::UI::Xaml::Hosting::ElementCompositionPreview::GetElementVisual(rootGridTaskBar);
  if (!rootGridTaskBarVisual) {
    Wh_Log(L"Error: !rootGridTaskBarVisual");
    return false;
  }
  auto taskbarFrameRepeaterVisual = winrt::Windows::UI::Xaml::Hosting::ElementCompositionPreview::GetElementVisual(taskbarFrameRepeater);
  if (!taskbarFrameRepeaterVisual) {
    Wh_Log(L"Error: !taskbarFrameRepeaterVisual");
    return false;
  }
  const float visualOffsetTolerance =
      std::max<float>(static_cast<float>(kLayoutToleranceDip),
                      2.0f / GetRasterizationScale(xamlRootContent));
  // Move the stable RootGrid instead of TaskbarFrameRepeater. The repeater is
  // virtualized and can expose transient/recycled child offsets during insert
  // and remove animations, which makes compositor Offset animations overshoot.
  // RootGrid remains the authoritative visual transform; the repeater itself is
  // kept horizontally neutral below.
  //
  // The Start button rect is sampled in RootGrid coordinates, but it is not used
  // as a live target. During task insertion/removal, Explorer can animate the
  // repeater's internal layout; chasing that transient Start rect causes a
  // feedback loop where RootGrid keeps catching up to Windows' own animation.
  // Only promote the sampled rect to an anchor after it has been stable for a
  // couple of ApplyStyle passes. Until then, keep using the last stable anchor
  // or the edge-clamp fallback from the root-grid version.
  const bool hasStartButtonAnchorSample =
      startButtonWidth > 0.0 &&
      startButtonHeight > 0.0 &&
      std::isfinite(startButtonLeft) &&
      std::isfinite(startButtonTop);
  const bool startButtonSampleChanged =
      hasStartButtonAnchorSample &&
      (!state.hasLastStartButtonAnchorRect ||
       std::abs(state.lastStartButtonAnchorLeft - static_cast<float>(startButtonLeft)) > visualOffsetTolerance ||
       std::abs(state.lastStartButtonAnchorTop - static_cast<float>(startButtonTop)) > visualOffsetTolerance ||
       std::abs(state.lastStartButtonAnchorWidth - static_cast<float>(startButtonWidth)) > visualOffsetTolerance ||
       std::abs(state.lastStartButtonAnchorHeight - static_cast<float>(startButtonHeight)) > visualOffsetTolerance);
  if (hasStartButtonAnchorSample) {
    if (startButtonSampleChanged) {
      state.startButtonAnchorStablePasses = 0;
    } else if (state.startButtonAnchorStablePasses < kStartButtonAnchorStablePassesRequired) {
      state.startButtonAnchorStablePasses++;
    }
    state.lastStartButtonAnchorLeft = static_cast<float>(startButtonLeft);
    state.lastStartButtonAnchorTop = static_cast<float>(startButtonTop);
    state.lastStartButtonAnchorWidth = static_cast<float>(startButtonWidth);
    state.lastStartButtonAnchorHeight = static_cast<float>(startButtonHeight);
    state.hasLastStartButtonAnchorRect = true;
    if (state.startButtonAnchorStablePasses >= kStartButtonAnchorStablePassesRequired) {
      state.stableStartButtonAnchorLeft = static_cast<float>(startButtonLeft);
      state.stableStartButtonAnchorTop = static_cast<float>(startButtonTop);
      state.stableStartButtonAnchorWidth = static_cast<float>(startButtonWidth);
      state.stableStartButtonAnchorHeight = static_cast<float>(startButtonHeight);
      state.hasStableStartButtonAnchorRect = true;
    }
  } else {
    state.hasLastStartButtonAnchorRect = false;
    state.hasStableStartButtonAnchorRect = false;
    state.startButtonAnchorStablePasses = 0;
  }
  const double predictedCenteredTaskbarLeft =
      (taskbarLayoutSurfaceWidth - static_cast<double>(childrenWidthTaskbar)) / 2.0;
  const double taskbarEdgeClampTolerance =
      std::max<double>(visualOffsetTolerance,
                       static_cast<double>(g_settings.userDefinedTaskbarButtonSize) * 0.25);
  const bool taskbarLayoutIsEdgeClamped =
      isOverflowing ||
      taskbarLeftEdge <= taskbarEdgeClampTolerance ||
      taskbarRightEdge >= taskbarLayoutSurfaceWidth - taskbarEdgeClampTolerance ||
      predictedCenteredTaskbarLeft <= taskbarEdgeClampTolerance;
  // Windows can shift the taskbar item group left before the official overflow
  // button appears, especially when the centered item group would get too close
  // to the tray/suffixed area. Do not turn that into a second live anchor state
  // machine: just broaden the original stable Start-button anchor path for this
  // specific tray-collision condition.
  const double trayFrameLeft = rootWidth - static_cast<double>(trayFrameWidth);
  const double predictedTaskbarRightOnScreen =
      targetContentLeft + static_cast<double>(childrenWidthTaskbar);
  const double predictedGapToTray =
      trayFrameLeft - predictedTaskbarRightOnScreen;
  const double minimumTaskbarToTrayGap =
      std::max<double>(static_cast<double>(visualOffsetTolerance) * 2.0,
                       static_cast<double>(g_settings.userDefinedTaskbarButtonSize) * 0.75);
  const bool taskbarLayoutIsTrayConstrained =
      !useVirtualTaskbarSurface &&
      trayFrameWidth > 0 &&
      taskbarRightEdge > taskbarLeftEdge &&
      predictedGapToTray < minimumTaskbarToTrayGap &&
      std::abs(taskbarLeftEdge - predictedCenteredTaskbarLeft) >
          visualOffsetTolerance;
  const bool stableStartAnchorMatchesCurrentSample =
      state.hasStableStartButtonAnchorRect &&
      hasStartButtonAnchorSample &&
      std::abs(state.stableStartButtonAnchorLeft -
               static_cast<float>(startButtonLeft)) <= visualOffsetTolerance &&
      std::abs(state.stableStartButtonAnchorWidth -
               static_cast<float>(startButtonWidth)) <= visualOffsetTolerance;
  const double fallbackTaskbarAnchorLeft =
      taskbarLayoutIsEdgeClamped ? taskbarLeftEdge : predictedCenteredTaskbarLeft;
  const bool useStableStartButtonAnchor =
      state.hasStableStartButtonAnchorRect &&
      (taskbarLayoutIsEdgeClamped ||
       isOverflowing ||
       (taskbarLayoutIsTrayConstrained && stableStartAnchorMatchesCurrentSample));
  const double taskbarAnchorLeft = useStableStartButtonAnchor
      ? static_cast<double>(state.stableStartButtonAnchorLeft)
      : fallbackTaskbarAnchorLeft;
  float targetTaskRootOffsetX = snapPx(targetContentLeft - static_cast<float>(taskbarAnchorLeft));
  float centeredTaskbarRightEdge = targetTrayLogicalLeft;
  const float taskbarBackgroundPadding =
      static_cast<float>(g_settings.userDefinedTaskbarBackgroundHorizontalPadding);
  float targetWidth = g_unloading
      ? static_cast<float>(rootWidth)
      : (static_cast<float>(childrenWidthTaskbar + trayFrameWidth) +
         (taskbarBackgroundPadding * 2.0f));
  if (targetWidth < 1) {
    Wh_Log(L"Error: targetWidth<1");
    return false;
  }
  const float targetBackgroundLeftScreen =
      snapPx(targetContentLeft - taskbarBackgroundPadding);
  const float targetBackgroundRightScreen =
      snapPx(targetBackgroundLeftScreen + targetWidth);
  const float targetScaleCenterScreenX =
      snapPx((targetBackgroundLeftScreen + targetBackgroundRightScreen) * 0.5f);
  const float targetTaskbarIslandScale = g_unloading
      ? 1.0f
      : CalculateTaskbarIslandScale(targetBackgroundLeftScreen,
                                    targetBackgroundRightScreen,
                                    static_cast<float>(rootWidth),
                                    targetScaleCenterScreenX,
                                    rasterizationScale);
  const float scaledBackgroundLeftScreen = snapPx(
      ApplyScaleToScreenX(targetBackgroundLeftScreen,
                          targetScaleCenterScreenX,
                          targetTaskbarIslandScale));
  const float scaledBackgroundRightScreen = snapPx(
      ApplyScaleToScreenX(targetBackgroundRightScreen,
                          targetScaleCenterScreenX,
                          targetTaskbarIslandScale));
  const bool taskbarIslandScaleTargetChanged =
      !state.hasLastTargetTaskbarIslandScale ||
      std::abs(state.lastTargetTaskbarIslandScale - targetTaskbarIslandScale) > 0.001f ||
      std::abs(state.lastTaskbarIslandScaleCenterX - targetScaleCenterScreenX) > visualOffsetTolerance;
  const uint64_t dimensionInvalidationGeneration =
      g_dimensionInvalidationGeneration.load(std::memory_order_acquire);
  const bool invalidateDimensionsThisPass =
      state.lastDimensionInvalidationGeneration != dimensionInvalidationGeneration;
  UpdateMinimizeAnimationCorrectionForMonitorTai(
      monitorName,
      taskbarChildrenMeasurement,
      useVirtualTaskbarSurface,
      static_cast<double>(targetTaskRootOffsetX),
      static_cast<double>(targetTaskbarIslandScale),
      static_cast<double>(targetScaleCenterScreenX),
      static_cast<double>(rasterizationScale),
      static_cast<double>(scaledBackgroundLeftScreen),
      static_cast<double>(scaledBackgroundRightScreen));
  if (!forceStyleApply && !invalidateDimensionsThisPass && !g_unloading &&
      std::abs(targetOffsetXTray - systemTrayFrameGridVisual.Offset().x) <= visualOffsetTolerance &&
      childrenWidthTaskbar == state.lastChildrenWidthTaskbar &&
      trayFrameWidth == state.lastTrayFrameWidth &&
      std::abs(targetTaskRootOffsetX - rootGridTaskBarVisual.Offset().x) <= visualOffsetTolerance &&
      std::abs(taskbarFrameRepeaterVisual.Offset().x) <= visualOffsetTolerance &&
      !taskbarIslandScaleTargetChanged) {
    Wh_Log(L"taskbar root/tray offsets/scale are within tolerance %f; widths didn't change: %d, %d", visualOffsetTolerance, childrenWidthTaskbar, state.lastTrayFrameWidth);
    return true;
  }
  if (childrenWidthTaskbar < 1) {
    state.lastChildrenWidthTaskbar = 1;
  } else {
    state.lastChildrenWidthTaskbar = static_cast<unsigned int>(childrenWidthTaskbar);
  }
  if (trayFrameWidth < 1) {
    state.lastTrayFrameWidth = 1;
  } else {
    state.lastTrayFrameWidth = static_cast<unsigned int>(trayFrameWidth);
  }
  signed int userDefinedTaskbarOffsetY = (g_settings.userDefinedFlatTaskbarBottomCorners || g_settings.userDefinedFullWidthTaskbarBackground) ? 0 : g_settings.userDefinedTaskbarOffsetY;
  if (ShouldLogTaskbarGeometry(isOverflowing || taskbarLayoutIsEdgeClamped || taskbarLayoutIsTrayConstrained || useStableStartButtonAnchor || targetTaskbarIslandScale < 0.999f || forceStyleApply)) {
    Wh_Log(L"[TBGEOM] virtualSurface=%d virtualWidth=%.2f actualRepeaterWidth=%.2f layoutSurfaceWidth=%.2f overflowSuppressed=%d",
           useVirtualTaskbarSurface ? 1 : 0,
           taskbarVirtualSurfaceWidth,
           taskbarFrameRepeater.ActualWidth(),
           taskbarLayoutSurfaceWidth,
           (overflowButton && useVirtualTaskbarSurface) ? 1 : 0);
    Wh_Log(L"[TBGEOM] scale target=%.4f centerX=%.2f rawBg=[%.2f..%.2f] scaledBg=[%.2f..%.2f]",
           targetTaskbarIslandScale,
           targetScaleCenterScreenX,
           targetBackgroundLeftScreen,
           targetBackgroundRightScreen,
           scaledBackgroundLeftScreen,
           scaledBackgroundRightScreen);
    Wh_Log(L"[TBGEOM] trayConstraint=%d predictedGapToTray=%.2f minGap=%.2f stableStartMatches=%d",
           taskbarLayoutIsTrayConstrained ? 1 : 0,
           predictedGapToTray,
           minimumTaskbarToTrayGap,
           stableStartAnchorMatchesCurrentSample ? 1 : 0);
    LogTaskbarGeometryProbe(
        L"ApplyStyle",
        monitorName,
        xamlRootContent,
        taskFrame,
        rootGridTaskBar,
        taskbarFrameRepeater,
        trayFrame,
        systemTrayFrameGrid,
        taskbarBackground,
        backgroundFillChild,
        startButtonElement,
        rootWidth,
        childrenWidthTaskbarDbl,
        taskbarLeftEdge,
        taskbarRightEdge,
        startButtonLeft,
        startButtonTop,
        startButtonWidth,
        startButtonHeight,
        predictedCenteredTaskbarLeft,
        isOverflowing,
        taskbarLayoutIsEdgeClamped,
        useStableStartButtonAnchor,
        targetContentLeft,
        targetContentWidth,
        targetTaskRootOffsetX,
        targetOffsetXTray,
        targetBackgroundLeftScreen,
        targetBackgroundRightScreen,
        state);
  }
  const float unscaledStartButtonScreenX = useStableStartButtonAnchor
      ? snapPx(state.stableStartButtonAnchorLeft + targetTaskRootOffsetX)
      : targetBackgroundLeftScreen;
  state.lastStartButtonXCalculated = snapPx(
      ApplyScaleToScreenX(unscaledStartButtonScreenX,
                          targetScaleCenterScreenX,
                          targetTaskbarIslandScale));
  auto heightValue = (g_settings.userDefinedTaskbarHeight + abs(userDefinedTaskbarOffsetY * 2));
  if (heightValue < g_settings.userDefinedTaskbarHeight / 2) {
    Wh_Log(L"Error: heightValue<g_settings.userDefinedTaskbarHeight/2");
    return false;
  }
  if (invalidateDimensionsThisPass) {
    if (g_settings.userDefinedTaskbarHeight <= 0) {
      Wh_Log(L"Invalid size detected! Panel Height");
      return false;
    }
    if (heightValue <= 0) {
      Wh_Log(L"Invalid size detected! Panel Height");
      return false;
    }
    trayFrame.Height(g_settings.userDefinedTaskbarHeight);
    trayFrame.MaxHeight(g_settings.userDefinedTaskbarHeight);
    taskFrame.Height(heightValue);
    taskFrame.MaxHeight(heightValue);
    taskbarFrameRepeater.Height(g_settings.userDefinedTaskbarHeight);
    taskbarFrameRepeater.MaxHeight(g_settings.userDefinedTaskbarHeight);
    ApplyVirtualTaskbarLayoutSurface(xamlRootContent,
                                     taskFrame,
                                     rootGridTaskBar,
                                     taskbarFrameRepeater,
                                     taskbarBackground,
                                     backgroundFillParent,
                                     backgroundFillChild,
                                     taskbarVirtualSurfaceWidth);
    state.lastDimensionInvalidationGeneration = dimensionInvalidationGeneration;
  }
  // Any previous version of the mod may have left a horizontal Offset
  // animation on the repeater. Clear only X so RootGrid owns task-area X
  // motion, but preserve the repeater's native Y offset. Windows/XAML can use
  // that Y component to vertically align the task buttons with the tray.
  if (std::abs(taskbarFrameRepeaterVisual.Offset().x) > visualOffsetTolerance) {
    auto repeaterOffset = taskbarFrameRepeaterVisual.Offset();
    taskbarFrameRepeaterVisual.StopAnimation(L"Offset");
    taskbarFrameRepeaterVisual.Offset({0.0f, repeaterOffset.y, repeaterOffset.z});
  }
  const float rootGridScaleCenterLocalX =
      snapPx(targetScaleCenterScreenX - targetTaskRootOffsetX);
  const float rootGridScaleCenterLocalY =
      snapPx(static_cast<float>(rootGridTaskBar.ActualHeight()) * 0.5f);
  const float trayScaleCenterLocalX =
      snapPx(targetScaleCenterScreenX - targetTrayLogicalLeft);
  const float trayScaleCenterLocalY =
      snapPx(static_cast<float>(systemTrayFrameGrid.ActualHeight()) * 0.5f);
  SetVisualScaleCenterAndAnimate(rootGridTaskBarVisual,
                                 targetTaskbarIslandScale,
                                 rootGridScaleCenterLocalX,
                                 rootGridScaleCenterLocalY,
                                 0.001f,
                                 !g_unloading);
  SetVisualScaleCenterAndAnimate(systemTrayFrameGridVisual,
                                 targetTaskbarIslandScale,
                                 trayScaleCenterLocalX,
                                 trayScaleCenterLocalY,
                                 0.001f,
                                 !g_unloading);
  state.lastTargetTaskbarIslandScale = targetTaskbarIslandScale;
  state.lastTaskbarIslandScaleCenterX = targetScaleCenterScreenX;
  state.hasLastTargetTaskbarIslandScale = true;
  if (auto taskRootVisualCompositor = rootGridTaskBarVisual.Compositor()) {
    if (!g_unloading) {
      const bool taskRootOffsetChanged =
          invalidateDimensionsThisPass ||
          !state.hasLastTargetTaskFrameOffsetX ||
          std::abs(state.lastTargetTaskFrameOffsetX - targetTaskRootOffsetX) > visualOffsetTolerance;
      if (taskRootOffsetChanged) {
        auto taskRootOffsetAnimation = taskRootVisualCompositor.CreateVector3KeyFrameAnimation();
        ConfigureTaskbarIslandAnimation(taskRootOffsetAnimation);
        InsertTaskbarIslandKeyFrame(taskRootOffsetAnimation, 1.0f, winrt::Windows::Foundation::Numerics::float3{targetTaskRootOffsetX, rootGridTaskBarVisual.Offset().y, rootGridTaskBarVisual.Offset().z});
        rootGridTaskBarVisual.StartAnimation(L"Offset", taskRootOffsetAnimation);
        state.lastTargetTaskFrameOffsetX = targetTaskRootOffsetX;
        state.hasLastTargetTaskFrameOffsetX = true;
      }
    } else {
      rootGridTaskBarVisual.StopAnimation(L"Offset");
      rootGridTaskBarVisual.Offset({0.0f, 0.0f, rootGridTaskBarVisual.Offset().z});
      taskbarFrameRepeaterVisual.StopAnimation(L"Offset");
      taskbarFrameRepeaterVisual.Offset({0.0f, 0.0f, taskbarFrameRepeaterVisual.Offset().z});
      state.hasLastTargetTaskFrameOffsetX = false;
    }
  }
  auto taskbarVisual = rootGridTaskBarVisual;
  auto trayVisualCompositor = systemTrayFrameGridVisual.Compositor();
    if (trayVisualCompositor) {
    if (!g_unloading) {
      const bool trayOffsetChanged =
          invalidateDimensionsThisPass ||
          !state.hasLastTargetTrayOffsetX ||
          std::abs(state.lastTargetTrayOffsetX - targetOffsetXTray) > visualOffsetTolerance;
      if (trayOffsetChanged) {
        auto trayAnimation = trayVisualCompositor.CreateVector3KeyFrameAnimation();
        ConfigureTaskbarIslandAnimation(trayAnimation);
        InsertTaskbarIslandKeyFrame(trayAnimation, 1.0f, winrt::Windows::Foundation::Numerics::float3{targetOffsetXTray, systemTrayFrameGridVisual.Offset().y, systemTrayFrameGridVisual.Offset().z});
        systemTrayFrameGridVisual.StartAnimation(L"Offset", trayAnimation);
        state.lastTargetTrayOffsetX = targetOffsetXTray;
        state.hasLastTargetTrayOffsetX = true;
      }
    } else {
      systemTrayFrameGridVisual.Offset({0.0f, 0.0f, 0.0f});
      state.hasLastTargetTrayOffsetX = false;
    }
  }
  if (widgetPresent && widgetMainView) {
    if (widgetElement) {
      auto widgetVisualParent = winrt::Windows::UI::Xaml::Hosting::ElementCompositionPreview::GetElementVisual(widgetElement);
      if (widgetVisualParent && widgetVisualParent.Offset().x != 0.0f) {
        widgetVisualParent.Offset({0.0f, widgetVisualParent.Offset().y, widgetVisualParent.Offset().z});
      }
    }
    auto widgetVisual = winrt::Windows::UI::Xaml::Hosting::ElementCompositionPreview::GetElementVisual(widgetMainView);
     if (widgetVisual) {
      if (!g_unloading) {
        auto compositorWidget = widgetVisual.Compositor();
        if (compositorWidget) {
          // Widget visual is inside the RootGrid-shifted subtree, so its local
          // target must exclude the RootGrid compositor offset. Its screen
          // position still lands after the centered task buttons.
          float targetOffsetXWidget = centeredTaskbarRightEdge - targetTaskRootOffsetX - 8.0f + g_settings.userDefinedTrayTaskGap;
          targetOffsetXWidget = snapPx(targetOffsetXWidget);
          float targetOffsetYWidget = snapPx(static_cast<float>(abs(g_settings.userDefinedTaskbarHeight - widgetElementVisibleHeight)));
          const bool widgetOffsetChanged =
              invalidateDimensionsThisPass ||
              !state.hasLastTargetWidgetOffset ||
              std::abs(state.lastTargetWidgetOffsetX - targetOffsetXWidget) > visualOffsetTolerance ||
              std::abs(state.lastTargetWidgetOffsetY - targetOffsetYWidget) > visualOffsetTolerance;
          if (widgetOffsetChanged) {
            auto widgetOffsetAnimation = compositorWidget.CreateVector3KeyFrameAnimation();
            ConfigureTaskbarIslandAnimation(widgetOffsetAnimation);
            InsertTaskbarIslandKeyFrame(widgetOffsetAnimation, 1.0f, winrt::Windows::Foundation::Numerics::float3{targetOffsetXWidget, targetOffsetYWidget, taskbarVisual.Offset().z});
            widgetVisual.StartAnimation(L"Offset", widgetOffsetAnimation);
            state.lastTargetWidgetOffsetX = targetOffsetXWidget;
            state.lastTargetWidgetOffsetY = targetOffsetYWidget;
            state.hasLastTargetWidgetOffset = true;
          }
        }
      } else {
        widgetVisual.Offset({0.0f, 0.0f, 0.0f});
        state.hasLastTargetWidgetOffset = false;
      }
    }
  }
  if (state.lastTargetWidth <= static_cast<float>(minimumTaskbarChildrenWidth)) {
    state.lastTargetWidth = static_cast<float>(rootWidth);
    if (!g_unloading && state.lastTargetWidth <= 0) {
      Wh_Log(L"Error: g_unloading && state.lastTargetWidth <= 0");
      return false;
    }
  }
  const float targetWidthRect = !g_settings.userDefinedFullWidthTaskbarBackground ? targetWidth : static_cast<float>(rootWidth);
  if (!g_unloading && targetWidthRect <= 0) {
    Wh_Log(L"Error: targetWidthRect<=0");
    return false;
  }
  int rightMostEdgeTray = static_cast<int>(scaledBackgroundRightScreen + 0.5f);
  if (state.lastRightMostEdgeTray != rightMostEdgeTray) {
    state.lastRightMostEdgeTray = rightMostEdgeTray;
    Wh_SetIntValue((L"lastRightMostEdgeTray_" + monitorName).c_str(), rightMostEdgeTray);
  }
  float leftMostEdgeTray = scaledBackgroundRightScreen - (static_cast<float>(trayFrameWidth) * targetTaskbarIslandScale);
  if (leftMostEdgeTray != state.lastLeftMostEdgeTray) {
    state.lastLeftMostEdgeTray = leftMostEdgeTray;
    Wh_SetIntValue((L"lastLeftMostEdgeTray_" + monitorName).c_str(), static_cast<int>(leftMostEdgeTray));
  }
  const auto targetHeightPrelim = (!g_settings.userDefinedFullWidthTaskbarBackground ? g_settings.userDefinedTaskbarHeight : xamlRootContent.ActualHeight());
  if (!g_unloading && targetHeightPrelim <= 0) {
    Wh_Log(L"Error: targetHeightPrelim<=0");
    return false;
  }
  const auto clipHeight = static_cast<float>(targetHeightPrelim + ((g_settings.userDefinedFlatTaskbarBottomCorners) ? (targetHeightPrelim - g_settings.userDefinedTaskbarCornerRadius) : 0.0f));
  if (!g_unloading && clipHeight <= 0) {
    Wh_Log(L"Error: clipHeight<=0");
    return false;
  }
  ProcessStackPanelChildren(stackPanel, clipHeight);
  ChangeControlCenterIconSize(systemTrayFrameGrid);
  auto trayOverflowArrowNotifyIconStack = FindChildByName(systemTrayFrameGrid, L"NotifyIconStack");
  if (trayOverflowArrowNotifyIconStack) {
    SetDividerForElement(trayOverflowArrowNotifyIconStack, clipHeight, g_settings.userDefinedTrayAreaDivider, true);
  } else {
    SetDividerForElement(stackPanel, clipHeight, g_settings.userDefinedTrayAreaDivider, true);
  }
  //  if (widgetPresent && widgetElementInnerChild) {
  //    SetDividerForElement(widgetElementInnerChild, clipHeight, widgetPresent && g_settings.userDefinedTrayAreaDivider, true);
  //  }
  const bool shouldApplyCustomTaskbarBackground = !g_unloading && g_settings.userDefinedCustomizeTaskbarBackground;
  const bool shouldClearCustomTaskbarBackground = g_unloading || (!shouldApplyCustomTaskbarBackground && state.hasCustomTaskbarBackgroundVisuals);
  auto taskbarStroke = FindChildByName(backgroundFillParent, L"BackgroundStroke");
  auto screenEdgeStroke = FindChildByName(rootGridTaskBar, L"ScreenEdgeStroke");
  // you can also try SystemAccentColor
  auto backgroundFillVisual = winrt::Windows::UI::Xaml::Hosting::ElementCompositionPreview::GetElementVisual(backgroundFillChild);
  auto compositorTaskBackground = backgroundFillVisual
      ? backgroundFillVisual.Compositor()
      : nullptr;
  const uintptr_t backgroundFillIdentity =
      reinterpret_cast<uintptr_t>(winrt::get_abi(backgroundFillChild));
  const bool backgroundFillChanged =
      state.backgroundFillIdentity != backgroundFillIdentity;
  const bool backgroundStyleChanged =
      backgroundFillChanged ||
      state.lastBackgroundStyleGeneration != childStyleGeneration;
  bool backgroundBrushReady = true;
  if (shouldApplyCustomTaskbarBackground) {
    if (taskbarStroke) {
      taskbarStroke.Opacity(0.0);
    }
    if (screenEdgeStroke) {
      screenEdgeStroke.Opacity(0.0);
    }
    if (!state.hasCustomTaskbarBackgroundVisuals || backgroundStyleChanged) {
      backgroundBrushReady =
          ApplyWindhawkBlurToBackgroundFill(backgroundFillChild);
    }
    state.backgroundFillIdentity = backgroundFillIdentity;
    state.hasCustomTaskbarBackgroundVisuals = true;
//    For custom brush
//    auto compositor = winrt::Windows::UI::Xaml::Hosting::ElementCompositionPreview::GetElementVisual(backgroundFillChild).Compositor();
//    float blurAmount = float(g_settings.userDefinedTaskbarBackgroundLuminosity);
//    winrt::Windows::Foundation::Numerics::float4 tint = {0,0,0,0};
//    auto blurBrush = winrt::make<XamlBlurBrush>(compositor, blurAmount, tint);
//    auto rectangle = backgroundFillChild.try_as<winrt::Windows::UI::Xaml::Shapes::Rectangle>();
//    if (rectangle){
//    rectangle.Fill(blurBrush);
//    }
  } else if (shouldClearCustomTaskbarBackground) {
    if (taskbarStroke) {
      taskbarStroke.ClearValue(UIElement::OpacityProperty());
    }
    if (screenEdgeStroke) {
      screenEdgeStroke.ClearValue(UIElement::OpacityProperty());
    }
    ClearWindhawkBlurFromBackgroundFill(backgroundFillChild);
    if (backgroundFillVisual) {
      backgroundFillVisual.Clip(nullptr);
    }
    winrt::Windows::UI::Xaml::Hosting::ElementCompositionPreview::
        SetElementChildVisual(
            backgroundFillChild,
            winrt::Windows::UI::Composition::Visual{nullptr});
    ResetBackgroundVisualCache(state);
    state.hasCustomTaskbarBackgroundVisuals = false;
  }
  // borders and corners
  if (shouldApplyCustomTaskbarBackground) {
    if (backgroundFillVisual) {
      if (compositorTaskBackground) {
        const float userDefinedTaskbarBorderThicknessFloat = static_cast<float>(g_settings.userDefinedTaskbarBorderThickness);
        // BackgroundFill belongs to RootGrid, and RootGrid is now translated.
        // Convert the desired screen-space island left into RootGrid-local
        // coordinates so the island background and task/tray content stay in
        // the same coordinate system.
        const float offsetXRect = snapPx(targetBackgroundLeftScreen - targetTaskRootOffsetX);
        const float newOffsetYRect = snapPx(static_cast<float>(abs(userDefinedTaskbarOffsetY)) );
        const bool backgroundShapeTargetChanged =
            std::abs(state.lastBackgroundShapeTargetWidth - targetWidthRect) > visualOffsetTolerance ||
            std::abs(state.lastBackgroundShapeTargetHeight - clipHeight) > visualOffsetTolerance ||
            std::abs(state.lastBackgroundShapeTargetOffsetX - offsetXRect) > visualOffsetTolerance ||
            std::abs(state.lastBackgroundShapeTargetOffsetY - newOffsetYRect) > visualOffsetTolerance;
        TaskbarBackgroundCompositionResourcesTai backgroundResources;
        bool createdBackgroundResources = false;
        if (backgroundFillChanged ||
            !TryGetTaskbarBackgroundCompositionResourcesTai(
                backgroundFillChild,
                backgroundFillVisual,
                &backgroundResources)) {
          createdBackgroundResources =
              CreateTaskbarBackgroundCompositionResourcesTai(
                  backgroundFillChild,
                  backgroundFillVisual,
                  compositorTaskBackground,
                  &backgroundResources);
        }
        if (backgroundResources.clipGeometry &&
            backgroundResources.borderVisual &&
            backgroundResources.borderGeometry &&
            backgroundResources.borderShape &&
            backgroundResources.borderBrush) {
          if (createdBackgroundResources || backgroundStyleChanged) {
            const float borderCornerRadius =
                g_settings.userDefinedTaskbarCornerRadius -
                userDefinedTaskbarBorderThicknessFloat / 2.0f;
            backgroundResources.clipGeometry.CornerRadius({
                g_settings.userDefinedTaskbarCornerRadius,
                g_settings.userDefinedTaskbarCornerRadius});
            backgroundResources.borderGeometry.CornerRadius({
                borderCornerRadius,
                borderCornerRadius});
            backgroundResources.borderGeometry.Offset({
                userDefinedTaskbarBorderThicknessFloat / 2.0f,
                userDefinedTaskbarBorderThicknessFloat / 2.0f});
            backgroundResources.borderShape.StrokeThickness(
                g_settings.userDefinedTaskbarBorderThickness);
            backgroundResources.borderShape.FillBrush(nullptr);
            backgroundResources.borderBrush.Color({
                g_settings.userDefinedTaskbarBorderOpacity,
                static_cast<BYTE>(g_settings.borderColorR),
                static_cast<BYTE>(g_settings.borderColorG),
                static_cast<BYTE>(g_settings.borderColorB)});
            backgroundResources.borderGeometry.Size({
                std::max(
                    0.0f,
                    targetWidthRect - userDefinedTaskbarBorderThicknessFloat),
                std::max(
                    0.0f,
                    clipHeight - userDefinedTaskbarBorderThicknessFloat)});
          }
          if (createdBackgroundResources ||
              backgroundShapeTargetChanged ||
              backgroundStyleChanged) {
            float animationStartWidth = targetWidthRect;
            float animationStartOffsetX = offsetXRect;
            float animationStartOffsetY = newOffsetYRect;
            const int64_t animationNowMs = DelayedApplyNowMs();
            const bool canContinueRunningBackgroundAnimation =
                !invalidateDimensionsThisPass &&
                state.backgroundAnimationStartMs > 0 &&
                animationNowMs - state.backgroundAnimationStartMs <
                    kTaskbarIslandAnimationDurationMs;
            if (!g_settings.userDefinedFullWidthTaskbarBackground) {
              if (canContinueRunningBackgroundAnimation) {
                animationStartWidth = EstimateAnimationValue(state.backgroundAnimationFromWidth, state.backgroundAnimationToWidth, state.backgroundAnimationStartMs, animationNowMs);
                animationStartOffsetX = EstimateAnimationValue(state.backgroundAnimationFromOffsetX, state.backgroundAnimationToOffsetX, state.backgroundAnimationStartMs, animationNowMs);
                animationStartOffsetY = EstimateAnimationValue(state.backgroundAnimationFromOffsetY, state.backgroundAnimationToOffsetY, state.backgroundAnimationStartMs, animationNowMs);
              } else if (state.lastTargetWidth > static_cast<float>(minimumTaskbarChildrenWidth) && std::abs(state.lastTargetWidth - rootWidth) > visualOffsetTolerance) {
                animationStartWidth = state.lastTargetWidth;
                animationStartOffsetX = state.lastTargetOffsetX == 0.0f ? offsetXRect : snapPx(state.lastTargetOffsetX);
                animationStartOffsetY = snapPx(state.lastTargetOffsetY);
              }
            }
            backgroundResources.clipGeometry.StopAnimation(L"Size");
            backgroundResources.clipGeometry.StopAnimation(L"Offset");
            backgroundResources.borderVisual.StopAnimation(L"Size");
            backgroundResources.borderVisual.StopAnimation(L"Offset");
            backgroundResources.borderGeometry.StopAnimation(L"Size");
            if (!g_settings.userDefinedFullWidthTaskbarBackground) {
              backgroundResources.clipGeometry.Size(
                  {animationStartWidth, clipHeight});
              backgroundResources.borderVisual.Size(
                  {animationStartWidth, clipHeight});
              backgroundResources.borderGeometry.Size({
                  std::max(
                      0.0f,
                      animationStartWidth -
                          userDefinedTaskbarBorderThicknessFloat),
                  std::max(
                      0.0f,
                      clipHeight - userDefinedTaskbarBorderThicknessFloat)});
              backgroundResources.clipGeometry.Offset(
                  {animationStartOffsetX, animationStartOffsetY});
              backgroundResources.borderVisual.Offset(
                  {animationStartOffsetX, animationStartOffsetY, 0.0f});
              auto sizeAnimationRect = compositorTaskBackground.CreateVector2KeyFrameAnimation();
              ConfigureTaskbarIslandAnimation(sizeAnimationRect);
              sizeAnimationRect.InsertKeyFrame(0.0f, {animationStartWidth, clipHeight});
              InsertTaskbarIslandKeyFrame(sizeAnimationRect, 1.0f, winrt::Windows::Foundation::Numerics::float2{targetWidthRect, clipHeight});
              auto sizeAnimationBorderGeometry = compositorTaskBackground.CreateVector2KeyFrameAnimation();
              ConfigureTaskbarIslandAnimation(sizeAnimationBorderGeometry);
              sizeAnimationBorderGeometry.InsertKeyFrame(0.0f, {animationStartWidth - userDefinedTaskbarBorderThicknessFloat, clipHeight - userDefinedTaskbarBorderThicknessFloat});
              InsertTaskbarIslandKeyFrame(sizeAnimationBorderGeometry, 1.0f, winrt::Windows::Foundation::Numerics::float2{targetWidthRect - userDefinedTaskbarBorderThicknessFloat, clipHeight - userDefinedTaskbarBorderThicknessFloat});
              backgroundResources.clipGeometry.StartAnimation(
                  L"Size",
                  sizeAnimationRect);
              backgroundResources.borderVisual.StartAnimation(
                  L"Size",
                  sizeAnimationRect);
              backgroundResources.borderGeometry.StartAnimation(
                  L"Size",
                  sizeAnimationBorderGeometry);
              auto offsetAnimationRect = compositorTaskBackground.CreateVector2KeyFrameAnimation();
              ConfigureTaskbarIslandAnimation(offsetAnimationRect);
              offsetAnimationRect.InsertKeyFrame(0.0f, {animationStartOffsetX, animationStartOffsetY});
              InsertTaskbarIslandKeyFrame(offsetAnimationRect, 1.0f, winrt::Windows::Foundation::Numerics::float2{offsetXRect, newOffsetYRect});
              auto offsetAnimationRect3V = compositorTaskBackground.CreateVector3KeyFrameAnimation();
              ConfigureTaskbarIslandAnimation(offsetAnimationRect3V);
              offsetAnimationRect3V.InsertKeyFrame(0.0f, {animationStartOffsetX, animationStartOffsetY, 0.0f});
              InsertTaskbarIslandKeyFrame(offsetAnimationRect3V, 1.0f, winrt::Windows::Foundation::Numerics::float3{offsetXRect, newOffsetYRect, 0.0f});
              backgroundResources.clipGeometry.StartAnimation(
                  L"Offset",
                  offsetAnimationRect);
              backgroundResources.borderVisual.StartAnimation(
                  L"Offset",
                  offsetAnimationRect3V);
              state.backgroundAnimationFromWidth = animationStartWidth;
              state.backgroundAnimationToWidth = targetWidthRect;
              state.backgroundAnimationFromOffsetX = animationStartOffsetX;
              state.backgroundAnimationToOffsetX = offsetXRect;
              state.backgroundAnimationFromOffsetY = animationStartOffsetY;
              state.backgroundAnimationToOffsetY = newOffsetYRect;
              state.backgroundAnimationStartMs = animationNowMs;
              state.lastTargetOffsetX = offsetXRect;
              state.lastTargetOffsetY = newOffsetYRect;
            } else {
              state.lastTargetOffsetX = 0;
              state.lastTargetOffsetY = 0;
              state.backgroundAnimationStartMs = 0;
              backgroundResources.clipGeometry.Size(
                  {targetWidthRect, clipHeight});
              backgroundResources.borderVisual.Size({targetWidthRect, clipHeight});
              if(rootGridTaskBarVisual){
              backgroundResources.clipGeometry.Offset({static_cast<float>(-rootGridTaskBarVisual.Offset().x), 0.0f});
              backgroundResources.borderVisual.Offset({static_cast<float>(-rootGridTaskBarVisual.Offset().x), 0.0f, 0.0f});
              }
              backgroundResources.borderGeometry.Size({
                  std::max(
                      0.0f,
                      targetWidthRect - userDefinedTaskbarBorderThicknessFloat),
                  std::max(
                      0.0f,
                      clipHeight - userDefinedTaskbarBorderThicknessFloat)});
            }
            state.lastBackgroundShapeTargetWidth = targetWidthRect;
            state.lastBackgroundShapeTargetHeight = clipHeight;
            state.lastBackgroundShapeTargetOffsetX = offsetXRect;
            state.lastBackgroundShapeTargetOffsetY = newOffsetYRect;
          }
          if (backgroundBrushReady) {
            state.lastBackgroundStyleGeneration = childStyleGeneration;
          }
        }
      }
    }
  }
  state.wasOverflowing = isOverflowing;
  state.lastTargetWidth = targetWidth;
  g_initial_style_apply_completed = true;
  g_initial_style_apply_not_before_ms = 0;
  return true;
  } catch (winrt::hresult_error const& ex) {
    Wh_Log(L"ApplyStyle failed %08X: %s", ex.code(), ex.message().c_str());
  } catch (std::exception const& ex) {
    Wh_Log(L"ApplyStyle failed: %S", ex.what());
  } catch (...) {
    Wh_Log(L"ApplyStyle failed: %08X", winrt::to_hresult());
  }
  return false;
}
void ApplySettings(HWND hTaskbarWnd) {
  if (hTaskbarWnd && IsWindow(hTaskbarWnd)) {
    RunFromWindowThread(hTaskbarWnd, [](void* pParam) { ApplySettingsFromTaskbarThread(); }, 0);
  }
}
void RefreshSettings() {
  unsigned int oldTaskbarButtonSize;
  {
    std::lock_guard<std::recursive_mutex> settingsLock(g_settingsMutex);
    oldTaskbarButtonSize = g_settings.userDefinedTaskbarButtonSize;
  }
  Wh_ModSettingsChangedTBIconSize();
  Wh_ModSettingsChangedStartButtonPosition();
  UpdateGlobalSettings();
  RequestTaskbarChildStyleRefresh();
  RequestTaskbarDimensionInvalidation();
  unsigned int newTaskbarButtonSize;
  {
    std::lock_guard<std::recursive_mutex> settingsLock(g_settingsMutex);
    newTaskbarButtonSize = g_settings.userDefinedTaskbarButtonSize;
  }
  if (!g_unloading && oldTaskbarButtonSize > 0 &&
      oldTaskbarButtonSize != newTaskbarButtonSize) {
    RequestTaskbarButtonSizeRelayout();
  }
}
void ResetGlobalVars() {
  for (const auto& stateHandle : GetTaskbarStatesSnapshot()) {
    std::lock_guard<std::recursive_mutex> stateLock(stateHandle->mutex);
    auto& state = *stateHandle;
    state.lastTaskbarData.childrenCount = 0;
    state.lastTaskbarData.rightMostEdge = 0;
    // state.lastTaskbarData.childrenWidth = 0;
    state.lastChildrenWidthTaskbar = 0;
    // state.lastTrayFrameWidth = 0;
    state.wasOverflowing = false;
  }
  RequestTaskbarDimensionInvalidation();
}
bool g_PartialMode = false;
void Wh_ModSettingsChanged() {
  if (g_PartialMode) {
    return;
  }
  Wh_Log(L"Settings Changed");
  ResetGlobalVars();
  RefreshSettings();
  ApplySettings(FindCurrentProcessTaskbarWnd());
}
bool IsExplorer() {
  wchar_t processPath[MAX_PATH];
  if (GetModuleFileName(NULL, processPath, MAX_PATH)) {
    const wchar_t* processName = wcsrchr(processPath, L'\\');
    if (processName && _wcsicmp(processName + 1, L"explorer.exe") == 0) {
      return true;
    }
  }
  return false;
}
using SetWindowPos_t = BOOL(WINAPI*)(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags);
SetWindowPos_t SetWindowPos_Original = nullptr;
std::wstring GetProcessExeName(DWORD processId) {
  std::wstring result = L"<unknown>";
  HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processId);
  if (hProcess) {
    WCHAR path[MAX_PATH];
    DWORD size = ARRAYSIZE(path);
    if (QueryFullProcessImageNameW(hProcess, 0, path, &size)) {
      std::wstring fullPath = path;
      size_t pos = fullPath.find_last_of(L"\\/");
      result = (pos != std::wstring::npos) ? fullPath.substr(pos + 1) : fullPath;
    }
    CloseHandle(hProcess);
  }
  return result;
}
BOOL WINAPI SetWindowPos_Hook(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags) {
  DWORD processId = 0;
  const bool userDefinedMoveFlyoutControlCenter =
      Wh_GetIntSetting(L"MoveFlyoutControlCenter") != 0;
  auto callOriginal = [&]() -> BOOL {
    return SetWindowPos_Original
        ? SetWindowPos_Original(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags)
        : FALSE;
  };
  if (!hWnd || !GetWindowThreadProcessId(hWnd, &processId)) {
    return callOriginal();
  }
  WCHAR className[256] = L"<unknown>";
  GetClassNameW(hWnd, className, ARRAYSIZE(className));
  const std::wstring windowClassName = className;
  const std::wstring processFileName = GetProcessExeName(processId);
  Wh_Log(L"[SetWindowPos] PID: %lu | EXE: %s | Class: %s | HWND: 0x%p | Pos: (%d,%d) Size: %dx%d Flags: 0x%08X",
         processId,
         processFileName.c_str(),
         windowClassName.c_str(),
         hWnd,
         X,
         Y,
         cx,
         cy,
         uFlags);
  if (!g_unloading && userDefinedMoveFlyoutControlCenter && _wcsicmp(processFileName.c_str(), L"ShellHost.exe") == 0 && _wcsicmp(windowClassName.c_str(), L"ControlCenterWindow") == 0) {
    HMONITOR monitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
    if (!monitor) {
      return callOriginal();
    }
    auto monitorName = GetMonitorName(monitor);
    if (monitorName.empty()) {
      Wh_Log(L"[SetWindowPos] Failed to resolve monitor name");
      return callOriginal();
    }
    int lastRecordedTrayRightMostEdgeForMonitor = Wh_GetIntValue((L"lastRightMostEdgeTray_" + monitorName).c_str(), -1);
    if (lastRecordedTrayRightMostEdgeForMonitor > 0) {
      UINT monitorDpiX = 96;
      UINT monitorDpiY = 96;
      if (FAILED(GetDpiForMonitor(monitor, MDT_DEFAULT, &monitorDpiX, &monitorDpiY)) ||
          monitorDpiX == 0) {
        monitorDpiX = 96;
        monitorDpiY = 96;
      }
      float dpiScale = static_cast<float>(monitorDpiX) / 96.0f;
      const int flyoutInnerPaddingPx = GetFlyoutInnerPaddingPx(dpiScale);
      X = static_cast<int>(lastRecordedTrayRightMostEdgeForMonitor * dpiScale + flyoutInnerPaddingPx - (Wh_GetIntSetting(L"AlignFlyoutInner") ? cx : (cx / 2.0f)));
      Wh_Log(L"[SetWindowPos] New X %d", X);
    } else {
      Wh_Log(L"[SetWindowPos] No reference state for monitor %s", monitorName.c_str());
    }
  }
  return callOriginal();
}
BOOL Wh_ModInit() {
  Wh_Log(L"======================================================");
  HMODULE moduleUser32 = GetModuleHandleW(L"user32.dll");
  if (moduleUser32) {
    auto pSetWindowPos = (SetWindowPos_t)GetProcAddress(moduleUser32, "SetWindowPos");
    if (pSetWindowPos) {
      if (WindhawkUtils::Wh_SetFunctionHookT(pSetWindowPos, SetWindowPos_Hook, &SetWindowPos_Original)) {
        Wh_Log(L"Successfully hooked SetWindowPos");
      } else {
        Wh_Log(L"Failed to hook SetWindowPos");
      }
    } else {
      Wh_Log(L"Failed to get address of SetWindowPos");
    }
  } else {
    Wh_Log(L"Failed to load user32.dll");
  }
  if (!IsExplorer()) {
    g_PartialMode = true;
    Wh_Log(L"Not explorer.exe; setting g_PartialMode to true");
    // StartDocked.dll
    HMODULE moduleStartDocked = GetModuleHandle(L"StartDocked.dll");
    if (moduleStartDocked) {
      WindhawkUtils::SYMBOL_HOOK StartDockedDll_hook[] = {{{LR"(private: void __cdecl StartDocked::StartSizingFrame::UpdateWindowRegion(class Windows::Foundation::Size))"}, &StartDocked__StartSizingFrame_UpdateWindowRegion_WithArgs_Original, StartDocked__StartSizingFrame_UpdateWindowRegion_WithArgs_Hook}};
      return WindhawkUtils::HookSymbols(moduleStartDocked, StartDockedDll_hook, ARRAYSIZE(StartDockedDll_hook));
    }
    return true;
  }
  g_unloading = false;
  g_worker_threads_stopping = false;
  InitMinimizeAnimationCorrectionTai();
  ArmInitialExplorerStyleApplyDelay();
  if (!Wh_ModInitTBIconSize()) {
    Wh_Log(L"Wh_ModInitTBIconSize failed");
    UninitMinimizeAnimationCorrectionTai();
    return FALSE;
  }
  if (!Wh_ModInitStartButtonPosition()) {
    Wh_Log(L"Wh_ModInitStartButtonPosition failed");
    UninitMinimizeAnimationCorrectionTai();
    return FALSE;
  }
  return TRUE;
}
void Wh_ModAfterInit() {
  if (g_PartialMode) {
    g_lastRecordedStartMenuWidth = Wh_GetIntValue(L"lastRecordedStartMenuWidth", g_lastRecordedStartMenuWidth);
    return;
  }
  Wh_ModAfterInitTBIconSize();
  ResetGlobalVars();
  LoadSettingsTBIconSize();
  LoadSettingsStartButtonPosition();
  UpdateGlobalSettings();
  ScheduleInitialExplorerStyleApply();
}
void Wh_ModBeforeUninit() {
  if (g_PartialMode) {
    return;
  }
  g_unloading = true;
  CleanupDebounce();
  UninitMinimizeAnimationCorrectionTai();
  Wh_ModBeforeUninitTBIconSize();
  Wh_ModBeforeUninitStartButtonPosition();
  RefreshSettings();
  HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
  if (hTaskbarWnd) {
    ApplySettings(hTaskbarWnd);
  }
}
void Wh_ModUninit() {
  if (g_PartialMode) {
    return;
  }
  UninitMinimizeAnimationCorrectionTai();
  CleanupDebounce();
  Wh_ModUninitTBIconSize();
  ResetGlobalVars();
  ClearTaskbarStates();
  {
    std::lock_guard<std::recursive_mutex> settingsLock(g_settingsMutex);
    g_settings.compiledDividedAppPatterns.clear();
  }
  Wh_Log(L"... detached");
}