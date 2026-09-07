// ==WindhawkMod==
// @id              windhawk-topbar
// @name            TopBar for Windows
// @description     A working TopBar with Flyouts for Windows through Windhawk.
// @version         1.0.0
// @author          WasiXGamer
// @github          https://github.com/wasixgamer
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lgdi32 -lole32 -loleaut32 -lruntimeobject -lshell32 -ldwmapi -ladvapi32 -luser32 -lshcore -lcomctl32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# TopBar For Windhawk

![TopBar screenshot](https://i.imgur.com/bryjzKr.png)

Adds a **TopBar** at top of your screen with multiple customizations, hosted by a
dedicated explorer.exe tool process.

[![Patreon](https://i.imgur.com/JJ0TluA.png)](https://www.patreon.com/WasiXGamer/join)

## Themes

Themes are collections of styles that can be selected from the **Theme** dropdown in the mod settings. The following themes are available:

| Theme | Preview |
|-------|---------|
| [GreenBar](https://github.com/wasixgamer/windhawk-topbar-styling-guide/tree/main/Themes/GreenBar) | [![GreenBar](https://raw.githubusercontent.com/wasixgamer/windhawk-topbar-styling-guide/main/Themes/GreenBar/screenshot.png)](https://github.com/wasixgamer/windhawk-topbar-styling-guide/tree/main/Themes/GreenBar) |
| [NoIslands](https://github.com/wasixgamer/windhawk-topbar-styling-guide/tree/main/Themes/NoIslands) | [![NoIslands](https://raw.githubusercontent.com/wasixgamer/windhawk-topbar-styling-guide/main/Themes/NoIslands/screenshot.png)](https://github.com/wasixgamer/windhawk-topbar-styling-guide/tree/main/Themes/NoIslands) |

More themes, stylings, etc can be found and contributed from:
**[Windhawk TopBar Styling Guide](https://github.com/wasixgamer/windhawk-topbar-styling-guide)**

## Features

- **Task list** — window icons, titles, click-to-activate, double-click maximize
- **Control centre** — Display (brightness, Dark Mode), Sound (volume, per-app mixer, device picker, media controls), Wi-Fi (scan/connect), Bluetooth (connect/disconnect), and Tray (notification area)
- **Full styling** via Control styles
- **Background translucency** with acrylic/blur

## Process model

The bar runs in its own Explorer tool process (`explorer.exe -tool-mod windhawk-topbar`).
A mutex keeps one bar alive. XAML Islands require a real Explorer host.

## Styling

Every element is targetable from **Control styles**, using the plain name, a bare
class name (`Button`), `ClassName#Name`, or a parent chain (`StackPanel > TextBlock`).
`*` matches any intermediate parents, and `:root >` requires a root element.

| Name | What it is |
|------|------------|
| `TopBarRoot` | Root `Grid` spanning the whole bar |
| `LeftPanel` | Left strip holding Start and Search |
| `StartButton` / `StartIcon` | Start button and its logo |
| `SearchButton` / `SearchIcon` | Search button (opens native Search) |
| `TaskListPanel` / `TaskButton` | Task strip, and every task button |
| `TaskButtonIcon` / `TaskButtonText` | Icon and label inside a task button |
| `TrayPanel` | Right-hand strip holding the status buttons and clock |
| `DisplayButton` `SoundButton` `WifiButton` `BluetoothButton` `TrayButton` | Status buttons |
| `ClockButton` / `ClockText` | Date/time |
| `BatteryButton` | Battery button |

More targets can be discovered with **[UWPSpy](https://github.com/m417z/UWPSpy/releases/)** by Spying the TopBar's `explorer.exe` process

### Keyboard shortcuts (For Inspecting the elements in Flyouts)

After setting UWPSpy at Sticky mode, the following Shortcuts can be used to trigger the flyouts for stylings:

| Hotkey | Action |
|--------|--------|
| Ctrl+Alt+1 | Toggle Display flyout |
| Ctrl+Alt+2 | Toggle Sound flyout |
| Ctrl+Alt+3 | Toggle Wi-Fi flyout |
| Ctrl+Alt+4 | Toggle Bluetooth flyout |
| Ctrl+Alt+5 | Toggle Tray flyout |
| Ctrl+Alt+6 | Show Start button context menu |
| Ctrl+Alt+7 | Show Task list context menu |

Style syntax: `Property=Value`, `Property:=<Xaml/>`, `$name` constants.
`TaskButton`.

## Global transparency and tint

The transparency and tint configured in **Top bar background color** and **Top bar background opacity**
are applied to the top bar, and all flyouts (Display, Sound, Wi‑Fi, Bluetooth, Tray)
and to all context menus.

## Known limitations

- Tray icons dont show up - Will attempted to be fixed in next update.
- Live Wallpapers are NOT supported and topbar background will use default windows wallpaper instead of live wallpaper.

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- theme: None
  $name: Theme
  $options:
  - None: No theme
  - GreenBar: GreenBar
  - NoIslands: NoIslands
  $description: >-
    Select a TopBar theme. 
- barHeight: 40
  $name: TopBar height
  $description: Height of the top bar in pixels.
- monitorIndex: 0
  $name: Monitor
  $description: 0 = primary monitor. Otherwise the secondary monitor number.
- cornerRadius: 6
  $name: Corner radius
  $description: Rounded corner radius used for buttons.
- topBarBackgroundColor: "#000000"
  $name: Top bar background color
  $description: >-
    Color of the tint over the wallpaper. Use 6-digit hex (#RRGGBB) or a color name (red, blue, green, etc.).
- topBarBackgroundOpacity: 50
  $name: Top bar background opacity
  $description: >-
    Opacity of Tint Color for TopBar.
- showStartButton: true
  $name: Show start button
- showSearchButton: true
  $name: Show search button
- showTaskList: true
  $name: Show task list
- taskButtonWidth: 150
  $name: Task button width (DIP)
  $description: >-
    Maximum width applied to every task button, so the list does not reflow or resize as
    window titles change.
- taskIconSize: 20
  $name: Task icon size (DIP)
  $description: >-
    On-screen size of task button icons. The source icon is extracted at this size times
    the monitor scale factor, so it stays sharp instead of being upscaled from 16px.
- taskButtonContent: textOnly
  $name: Task button content
  $options:
  - iconAndText: Icon and text
  - iconOnly: Icon only
  - textOnly: Text only
- showDisplayButton: true
  $name: Show display/brightness button
- showSoundButton: true
  $name: Show sound button
- showWifiButton: true
  $name: Show Wi-Fi button
- showBluetoothButton: true
  $name: Show Bluetooth button
- showTrayButton: false
  $name: Show tray button
- showBatteryButton: true
  $name: Show battery button

- enableHotkeys: false
  $name: Enable keyboard shortcuts (Ctrl+Alt+1…7)
  $description: Turn on global hotkeys. (Useful for Inspecting elements in flyout)
- showClock: true
  $name: Show time
- timeFormat: "🕑hh:mm tt"
  $name: Time format
  $description: >-
    Windows native time tokens: h, hh (12-hour), H, HH (24-hour), m, mm, s, ss, t, tt
    (AM/PM designator). Anything else, including emoji, is shown exactly as typed.
- showDate: true
  $name: Show date
- dateFormat: "📅ddd, MMM dd"
  $name: Date format
  $description: >-
    Windows native date tokens: d, dd, ddd, dddd (day), M, MM, MMM, MMMM (month), y, yy,
    yyyy (year). Anything else, including emoji, is shown exactly as typed.
- iconColor: "#FFFFFF"
  $name: Icon colour
  $description: Colour of the drawn vector icons (search, brightness, volume, Wi-Fi, ...).
- controlStyles:
  - - target: ""
      $name: Target
    - styles: [""]
      $name: Styles
  $name: Control styles
  $description: >-
    Chose targets Either from the given list in Readme, OR spy through UWPSpy.
- styleConstants: [""]
  $name: Style constants
  $description: name=value pairs referenced in styles as $name.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <shellapi.h>
#include <dwmapi.h>

#include <shellscalingapi.h>
#include <objbase.h>


#undef GetCurrentTime

#include <propsys.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <audiopolicy.h>
#include <wbemidl.h>
#include <uiautomation.h>
#include <wlanapi.h>
#include <bluetoothapis.h>
#include <physicalmonitorenumerationapi.h>
#include <highlevelmonitorconfigurationapi.h>

#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Media.Imaging.h>
#include <winrt/Windows.UI.Xaml.Shapes.h>
#include <winrt/Windows.UI.Xaml.Markup.h>
#include <winrt/Windows.UI.Xaml.Interop.h>
#include <winrt/Windows.UI.Xaml.Hosting.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.h>
#include <winrt/Windows.UI.ViewManagement.h>
#include <winrt/Windows.UI.Text.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Input.h>
#include <winrt/Windows.System.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Storage.Streams.h>
#include <windows.ui.xaml.hosting.desktopwindowxamlsource.h>

#if __has_include(<winrt/Windows.Media.Control.h>)
#define TOPBAR_HAS_MEDIA_CONTROL 1
#include <winrt/Windows.Media.Control.h>
#else
#define TOPBAR_HAS_MEDIA_CONTROL 0
#endif

#if __has_include(<winrt/Windows.Devices.Radios.h>)
#define TOPBAR_HAS_RADIOS 1
#include <winrt/Windows.Devices.Radios.h>
#else
#define TOPBAR_HAS_RADIOS 0
#endif

#if __has_include(<winrt/Windows.Devices.Bluetooth.h>)
#define TOPBAR_HAS_BLUETOOTH_LE 1
#include <winrt/Windows.Devices.Bluetooth.h>
#include <winrt/Windows.Devices.Bluetooth.GenericAttributeProfile.h>
#else
#define TOPBAR_HAS_BLUETOOTH_LE 0
#endif

#include <algorithm>
#include <chrono>
#include <functional>
#include <map>
#include <set>
#include <string>
#include <string_view>
#include <thread>
#include <atomic>
#include <mutex>
#include <process.h>

#include <vector>

using namespace winrt::Windows::UI::Xaml;


namespace wuxh = winrt::Windows::UI::Xaml::Hosting;
namespace wuxc = winrt::Windows::UI::Xaml::Controls;
namespace wuxm = winrt::Windows::UI::Xaml::Media;
namespace wf = winrt::Windows::Foundation;
namespace wui = winrt::Windows::UI;

// IXamlSourceTransparency – not projected in standard headers, so declare manually.
MIDL_INTERFACE("06636c29-5a17-458d-8ea2-2422d997a922")
IXamlSourceTransparency : public IUnknown
{
    virtual HRESULT STDMETHODCALLTYPE get_IsBackgroundTransparent(BOOL* value) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_IsBackgroundTransparent(BOOL value) = 0;
};
// Returns the system accent color, falling back to default blue if it fails.
wui::Color GetSystemAccentColor() {
    try {
        auto settings = winrt::Windows::UI::ViewManagement::UISettings();
        auto c = settings.GetColorValue(winrt::Windows::UI::ViewManagement::UIColorType::Accent);
        const double factor = 0.8;
        uint8_t r = static_cast<uint8_t>(c.R * factor);
        uint8_t g = static_cast<uint8_t>(c.G * factor);
        uint8_t b = static_cast<uint8_t>(c.B * factor);
        return wui::ColorHelper::FromArgb(c.A, r, g, b);
    } catch (...) {
        return wui::ColorHelper::FromArgb(255, 0, 120, 212);
    }
}
// ============================================================================
// Forward declarations
// ============================================================================

void RefreshTaskList(bool forceIconRegeneration = false);
void ApplyAllControlStyles();
void ApplyVisibilitySettings();
FrameworkElement BuildTopBarContent();
void PositionAppBar(HWND hwnd, int heightPx);
void BuildTaskContextMenu();
void BuildStartContextMenu();
std::wstring FormatClockText();
void PopulateDisplayPanel();
void PopulateSoundPanel();
void PopulateWifiPanel();
void PopulateBluetoothPanel();
void PopulateTrayPanel();
void PopulateBatteryPanel();
void ApplyBlurToAllOpenPopups();

// ============================================================================
// Settings
// ============================================================================

struct ControlStyleRule {
    std::wstring target;
    std::vector<std::wstring> styles;
};

struct {
    int barHeightDip = 40;
    std::wstring topBarBackgroundColor = L"#000000";
    int topBarBackgroundOpacity = 70;
    int monitorIndex = 0;
    int cornerRadius = 6;
    bool showStartButton = true;
    bool showSearchButton = true;
    bool showTaskList = true;
    int taskButtonWidth = 150;
    int taskIconSize = 20;
    std::wstring taskButtonContent = L"textOnly";
    bool showDisplayButton = true;
    bool showSoundButton = true;
    bool showWifiButton = true;
    bool showBluetoothButton = true;
    bool showTrayButton = true;
    bool showBatteryButton = true;
    bool enableHotkeys = false;
    bool showClock = true;
    std::wstring timeFormat = L"🕑hh:mm tt";
    bool showDate = true;
    std::wstring dateFormat = L"📅ddd, MMM dd";
    std::wstring iconColor = L"#FFFFFF";
} g_settings;

std::vector<std::pair<std::wstring, std::wstring>> g_styleConstants;
std::vector<ControlStyleRule> g_controlStyleRules;

const std::vector<ControlStyleRule>& BuiltInStyles() {
    static const std::vector<ControlStyleRule> styles = {
        {L"TopBarRoot", {L"Margin=3,2", L"CornerRadius=6"}},
        {L"StartButton", {L"Width=35", L"Margin=8,2,2,2", L"Background:=#15ffffff"}},
        {L"SearchButton", {L"Background:=#15ffffff", L"Width=35", L"Margin=4,2,4,2"}},
        {L"SearchIcon", {L"Width=20", L"Height=20"}},
        {L"TaskButton",
         {L"Background:=#15ffffff", L"Margin=3,2,3,2", L"Foreground=white"}},
        {L"ClockText", {L"Foreground=white", L"FontSize=14"}},
        {L"ClockButton", {L"Background:=#15ffffff", L"Margin=3,2,6,2"}},
        {L"DisplayButton", {L"Background:=#15ffffff", L"Margin=5,4"}},
        {L"SoundButton", {L"Background:=#15ffffff", L"Margin=5,4"}},
        {L"WifiButton", {L"Background:=#15ffffff", L"Margin=5,4"}},
        {L"BluetoothButton", {L"Background:=#15ffffff", L"Margin=5,4"}},
        {L"TrayButton", {L"Background:=#15ffffff", L"Margin=5,4"}},
        {L"BatteryButton", {L"Background:=#15ffffff", L"Margin=5,4"}},
        {L"WifiHeaderToggle", {L"Width=50"}},
        {L"BluetoothHeaderToggle", {L"Width=50"}},
    };
    return styles;
}
const std::vector<ControlStyleRule> g_themeGreenBarStyles = {
    {L"TopBarRoot", {L"Background:=#102A27"}},
    {L"WifiHeaderToggle", {L"Width=50"}},
    {L"BluetoothHeaderToggle", {L"Width=50"}},
    {L"StartButton", {L"Background:=#27403C"}},
    {L"SearchButton", {L"Background:=#27403C"}},
    {L"ClockButton", {L"Background:=#27403C"}},
    {L"DisplayButton", {L"Background:=#27403C"}},
    {L"SoundButton", {L"Background:=#27403C"}},
    {L"WifiButton", {L"Background:=#27403C"}},
    {L"BluetoothButton", {L"Background:=#27403C"}},
    {L"TrayButton", {L"Background:=#27403C"}},
    {L"BatteryButton", {L"Background:=#27403C"}},
    {L"TaskButton", {L"Background:=#27403C"}},
};

const std::vector<ControlStyleRule> g_themeNoIslandsStyles = {
    {L"TopBarRoot", {L"Margin=0", L"CornerRadius=0"}},
    {L"StartButton", {L"Background:=transparent"}},
    {L"SearchButton", {L"Background:=transparent"}},
    {L"ClockButton", {L"Background:=transparent"}},
    {L"DisplayButton", {L"Background:=transparent"}},
    {L"SoundButton", {L"Background:=transparent"}},
    {L"WifiButton", {L"Background:=transparent"}},
    {L"BluetoothButton", {L"Background:=transparent"}},
    {L"BatteryButton", {L"Background:=transparent"}},
    {L"TrayButton", {L"Background:=transparent"}},
    {L"TaskButton", {L"Background:=transparent"}},
};

// Global variable to hold the currently selected theme's styles
std::vector<ControlStyleRule> g_themeStyleRules;

// ============================================================================
// Globals
// ============================================================================


HANDLE g_topBarThread;
DWORD g_topBarThreadId;
HANDLE g_stopEvent = nullptr;  // Stop event for clean shutdown
HMODULE g_modModule = nullptr;

HWND g_topBarHwnd;
HWND g_islandHwnd;
[[clang::no_destroy]] wuxc::Grid g_wallpaperLayer{nullptr};  // Store the wallpaper layer for updates
std::wstring g_lastWallpaperPath;       // For change detection

int g_barHeightPx = 40;
double g_dpiScale = 1.0;

[[clang::no_destroy]] wuxh::WindowsXamlManager g_xamlManager{nullptr};
[[clang::no_destroy]] wuxh::DesktopWindowXamlSource g_desktopSource{nullptr};
[[clang::no_destroy]] winrt::Windows::System::DispatcherQueue g_uiDispatcherQueue{nullptr};

[[clang::no_destroy]] DispatcherTimer g_clockTimer{nullptr};
[[clang::no_destroy]] DispatcherTimer g_taskListTimer{nullptr};
[[clang::no_destroy]] DispatcherTimer g_wifiAutoRefreshTimer{nullptr};
[[clang::no_destroy]] DispatcherTimer g_bluetoothAutoRefreshTimer{nullptr};

[[clang::no_destroy]] std::map<std::wstring, FrameworkElement> g_namedElements;
[[clang::no_destroy]] FrameworkElement g_rootElement{nullptr};
[[clang::no_destroy]] wuxc::StackPanel g_taskListPanel{nullptr};
[[clang::no_destroy]] std::vector<HWND> g_stableWindowOrder;
[[clang::no_destroy]] std::map<HWND, wuxc::Button> g_taskButtonsByHwnd;
[[clang::no_destroy]] std::map<HWND, std::wstring> g_taskButtonLastTitle;

[[clang::no_destroy]] wuxc::MenuFlyout g_taskContextMenu{nullptr};
[[clang::no_destroy]] wuxc::MenuFlyoutItem g_taskMenuToggleItem{nullptr};
[[clang::no_destroy]] HWND g_contextMenuTargetHwnd;

[[clang::no_destroy]] wuxc::MenuFlyout g_startContextMenu{nullptr};

// Foreground tracking. Clicking a task button activates the bar itself, so
// GetForegroundWindow() can never equal the clicked window by the time the
// handler runs -- which is exactly why "click to minimize" never fired. A
// global EVENT_SYSTEM_FOREGROUND hook records the last real foreground window
// instead, ignoring anything owned by this process.
HWINEVENTHOOK g_windowEventHook;
HWND g_lastForegroundHwnd;
HWINEVENTHOOK g_foregroundHook = nullptr;

wui::Color g_iconTintColor{0, 255, 255, 255};
double g_iconTintOpacity = 0.0;

constexpr UINT WM_APPBAR_CALLBACK = WM_APP + 0x137;
constexpr UINT_PTR kAppBarInitTimerId = 1;
constexpr PCWSTR kWindowClassName = L"WindhawkTopBarWnd";

// Hotkey IDs for opening control flyouts.
constexpr int HOTKEY_ID_DISPLAY = 1;
constexpr int HOTKEY_ID_SOUND = 2;
constexpr int HOTKEY_ID_WIFI = 3;
constexpr int HOTKEY_ID_BLUETOOTH = 4;
constexpr int HOTKEY_ID_TRAY = 5;
constexpr int HOTKEY_ID_START_MENU = 6;
constexpr int HOTKEY_ID_TASK_MENU = 7;
UINT g_taskbarCreatedMsg = 0;
bool g_appBarRegistered = false;


// ============================================================================
// String helpers
// ============================================================================

std::wstring GetStringSettingCopy(PCWSTR name) {
    PCWSTR value = Wh_GetStringSetting(name);
    std::wstring result = value ? value : L"";
    Wh_FreeStringSetting(value);
    return result;
}

// Array settings are addressed with a format string ("controlStyles[%d].target"),
// which Wh_GetStringSetting takes variadically.
template <typename... Args>
std::wstring GetStringSettingCopy(PCWSTR name, Args... args) {
    PCWSTR value = Wh_GetStringSetting(name, args...);
    std::wstring result = value ? value : L"";
    Wh_FreeStringSetting(value);
    return result;
}

std::wstring TrimWs(std::wstring_view s) {
    size_t b = s.find_first_not_of(L" \t\r\n");
    if (b == std::wstring_view::npos) {
        return L"";
    }
    size_t e = s.find_last_not_of(L" \t\r\n");
    return std::wstring(s.substr(b, e - b + 1));
}

std::wstring EscapeXmlAttr(std::wstring_view s) {
    std::wstring out;
    out.reserve(s.size());
    for (wchar_t c : s) {
        switch (c) {
            case L'&': out += L"&amp;"; break;
            case L'"': out += L"&quot;"; break;
            case L'<': out += L"&lt;"; break;
            case L'>': out += L"&gt;"; break;
            default: out.push_back(c);
        }
    }
    return out;
}

std::wstring ToLowerCopy(std::wstring value) {
    std::transform(value.begin(), value.end(), value.begin(),
                   [](wchar_t ch) { return static_cast<wchar_t>(towlower(ch)); });
    return value;
}

std::wstring ApplyStyleConstants(std::wstring_view value) {
    std::wstring result;
    size_t lastPos = 0;
    size_t findPos;
    while ((findPos = value.find(L'$', lastPos)) != std::wstring_view::npos) {
        result.append(value, lastPos, findPos - lastPos);

        const std::pair<std::wstring, std::wstring>* match = nullptr;
        for (const auto& c : g_styleConstants) {
            if (value.substr(findPos + 1, c.first.size()) == c.first) {
                if (!match || c.first.size() > match->first.size()) {
                    match = &c;
                }
            }
        }

        if (match) {
            result += match->second;
            lastPos = findPos + 1 + match->first.size();
        } else {
            result += L'$';
            lastPos = findPos + 1;
        }
    }
    result.append(value.substr(lastPos));
    return result;
}

// ============================================================================
// Control style engine
// ============================================================================

struct TreeElementMatcher {
    std::wstring className;
    std::wstring name;
    bool bareIdentifier = false;
    bool wildcard = false;      // matches any intermediate parent chain
    bool rootRequired = false;  // requires the next matcher to be a root element
};

std::wstring ShortClassName(winrt::hstring const& fullName) {
    std::wstring_view view(fullName);
    auto dotPos = view.rfind(L'.');
    return std::wstring(dotPos != std::wstring_view::npos ? view.substr(dotPos + 1) : view);
}

TreeElementMatcher ParseMatcherPart(std::wstring_view part) {
    TreeElementMatcher m;
    
    // '*' wildcard — matches zero or more intermediate parent controls
    if (TrimWs(part) == L"*") {
        m.wildcard = true;
        return m;
    }
    
    std::wstring_view trimmedPart = part;
    
    auto hashPos = trimmedPart.find(L'#');
    if (hashPos != std::wstring_view::npos) {
        m.className = TrimWs(trimmedPart.substr(0, hashPos));
        m.name = TrimWs(trimmedPart.substr(hashPos + 1));
    } else {
        m.name = std::wstring(TrimWs(trimmedPart));
        m.bareIdentifier = true;
    }
    return m;
}

std::vector<TreeElementMatcher> ParseTargetChain(std::wstring_view target) {
    std::vector<TreeElementMatcher> result;
    size_t pos = 0;
    bool rootRequired = false;
    // Check the whole string before splitting.
    if (target.starts_with(L":root > ")) {
        rootRequired = true;
        target = target.substr(8); // remove ":root > "
    }
    while (pos <= target.size()) {
        size_t arrow = target.find(L" > ", pos);
        auto partSv = target.substr(
            pos, arrow == std::wstring_view::npos ? std::wstring_view::npos : arrow - pos);
        std::wstring trimmed = TrimWs(partSv);
        if (rootRequired) {
            auto matcher = ParseMatcherPart(trimmed);
            matcher.rootRequired = true;
            result.push_back(std::move(matcher));
            rootRequired = false; // only first matcher is root‑required
        } else {
            result.push_back(ParseMatcherPart(trimmed));
        }
        if (arrow == std::wstring_view::npos) {
            break;
        }
        pos = arrow + 3;
    }
    return result;
}

bool TestTreeMatcher(FrameworkElement const& element, TreeElementMatcher const& m) {
    if (m.wildcard) {
        return true;
    }
    
    // ':root' constraint: the element must have no parent FrameworkElement
    if (m.rootRequired) {
        DependencyObject parent = wuxm::VisualTreeHelper::GetParent(element);
        if (parent) {
            return false;
        }
    }
    
    std::wstring elementName(element.Name());
    if (m.bareIdentifier) {
        if (!m.name.empty() && elementName == m.name) {
            return true;
        }
        auto className = winrt::get_class_name(element);
        return ShortClassName(className) == m.name || std::wstring(className) == m.name;
    }
    if (!m.className.empty()) {
        auto className = winrt::get_class_name(element);
        if (ShortClassName(className) != m.className && std::wstring(className) != m.className) {
            return false;
        }
    }
    if (!m.name.empty() && elementName != m.name) {
        return false;
    }
    return true;
}

bool MatchesAncestorChain(FrameworkElement const& element,
                          std::vector<TreeElementMatcher> const& chain) {
    if (chain.size() <= 1) return true;
    DependencyObject current = element;
    int chainIndex = static_cast<int>(chain.size()) - 2;
    while (chainIndex >= 0) {
        // Wildcard: skip any number of ancestors to match the next matcher
        if (chain[chainIndex].wildcard) {
            int nextIdx = chainIndex - 1; // the matcher after the wildcard (since chain is reversed)
            if (nextIdx < 0) {
                return true; // leading wildcard: no further ancestor constraints
            }
            bool matched = false;
            while (current) {
                auto fe = current.try_as<FrameworkElement>();
                if (fe && TestTreeMatcher(fe, chain[nextIdx])) {
                    // found a matching ancestor; move current to this ancestor
                    chainIndex = nextIdx - 1;
                    matched = true;
                    break;
                }
                current = wuxm::VisualTreeHelper::GetParent(current);
            }
            if (!matched) return false;
            continue;
        }
        // Normal matcher: walk one parent up
        current = wuxm::VisualTreeHelper::GetParent(current);
        if (!current) return false;
        auto fe = current.try_as<FrameworkElement>();
        if (!fe || !TestTreeMatcher(fe, chain[chainIndex])) return false;
        chainIndex--;
    }
    return true;
}

void CollectMatchingElements(DependencyObject const& node,
                             std::vector<TreeElementMatcher> const& chain,
                             std::vector<FrameworkElement>& results) {
    if (!node) {
        return;
    }
    try {
        if (auto fe = node.try_as<FrameworkElement>()) {
            if (TestTreeMatcher(fe, chain.back()) && MatchesAncestorChain(fe, chain)) {
                results.push_back(fe);
            }
        }
        int count = wuxm::VisualTreeHelper::GetChildrenCount(node);
        for (int i = 0; i < count; i++) {
            CollectMatchingElements(wuxm::VisualTreeHelper::GetChild(node, i), chain, results);
        }
    } catch (...) {
    }
}

// Flyout and context menu content lives in separate popup trees, not under the
// main root, so name-only matches are also checked against them.
[[clang::no_destroy]] std::vector<FrameworkElement> g_detachedStyleRoots;

std::vector<FrameworkElement> ResolveGeneralTarget(const std::wstring& target) {
    std::vector<FrameworkElement> results;
    auto chain = ParseTargetChain(target);
    if (chain.empty()) {
        return results;
    }

    if (g_rootElement) {
        CollectMatchingElements(g_rootElement, chain, results);
    }
    for (auto& root : g_detachedStyleRoots) {
        if (root) {
            CollectMatchingElements(root, chain, results);
        }
    }

    if (chain.size() == 1 && !chain.back().name.empty()) {
        auto tryMenu = [&](wuxc::MenuFlyout const& menu) {
            if (!menu) {
                return;
            }
            for (auto const& item : menu.Items()) {
                if (auto fe = item.try_as<FrameworkElement>()) {
                    if (TestTreeMatcher(fe, chain.back())) {
                        results.push_back(fe);
                    }
                }
                if (auto sub = item.try_as<wuxc::MenuFlyoutSubItem>()) {
                    for (auto const& subItem : sub.Items()) {
                        if (auto subFe = subItem.try_as<FrameworkElement>()) {
                            if (TestTreeMatcher(subFe, chain.back())) {
                                results.push_back(subFe);
                            }
                        }
                    }
                }
            }
        };
        tryMenu(g_taskContextMenu);
        tryMenu(g_startContextMenu);
    }

    return results;
}

void ApplySingleStyleToElement(FrameworkElement element, const std::wstring& rule) {
    std::wstring ruleWithConstants = ApplyStyleConstants(rule);
    std::wstring trimmedRule = TrimWs(ruleWithConstants);
    if (trimmedRule.empty() || trimmedRule.starts_with(L"//")) {
        return;
    }

    size_t eqPos = trimmedRule.find(L'=');
    if (eqPos == std::wstring::npos) {
        Wh_Log(L"Bad style syntax (missing '='): %s", trimmedRule.c_str());
        return;
    }

    std::wstring propPart = trimmedRule.substr(0, eqPos);
    std::wstring valuePart = TrimWs(trimmedRule.substr(eqPos + 1));

    bool isXamlValue = false;
    std::wstring trimmedProp = TrimWs(propPart);
    if (!trimmedProp.empty() && trimmedProp.back() == L':') {
        isXamlValue = true;
        trimmedProp.pop_back();
        trimmedProp = TrimWs(trimmedProp);
    }
    if (trimmedProp.empty()) {
        Wh_Log(L"Bad style syntax (empty property): %s", trimmedRule.c_str());
        return;
    }

    auto className = winrt::get_class_name(element);
    std::wstring_view classNameView(className);
    auto dotPos = classNameView.rfind(L'.');
    std::wstring shortTypeName(dotPos != std::wstring_view::npos ? classNameView.substr(dotPos + 1)
                                                                 : classNameView);

    std::wstring setterXaml = L"<Setter Property=\"" + trimmedProp + L"\"";
    if (!isXamlValue) {
        setterXaml += L" Value=\"" + EscapeXmlAttr(valuePart) + L"\" />";
    } else if (valuePart.empty()) {
        setterXaml += L" Value=\"{x:Null}\" />";
    } else {
        setterXaml += L"><Setter.Value>" + valuePart + L"</Setter.Value></Setter>";
    }

    std::wstring styleXaml =
        L"<ResourceDictionary "
        L"xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/presentation\" "
        L"xmlns:x=\"http://schemas.microsoft.com/winfx/2006/xaml\">"
        L"<Style TargetType=\"" +
        shortTypeName + L"\">" + setterXaml + L"</Style></ResourceDictionary>";

    try {
        auto dict = Markup::XamlReader::Load(styleXaml).as<ResourceDictionary>();
        auto [key, styleObj] = dict.First().Current();
        auto style = styleObj.as<Style>();
        auto setter = style.Setters().GetAt(0).as<Setter>();
        auto value = setter.Value();
        if (value == DependencyProperty::UnsetValue()) {
            element.ClearValue(setter.Property());
        } else {
            element.SetValue(setter.Property(), value);
        }
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"Style apply failed (%s): %08X", trimmedRule.c_str(),
               static_cast<unsigned int>(ex.code().value));
    } catch (std::exception const& ex) {
        Wh_Log(L"Style apply failed (%s): %S", trimmedRule.c_str(), ex.what());
    }
}

// Converts a single byte to a 2-character hex string (e.g., 255 -> "FF")
std::wstring ToHexString(uint8_t value) {
    wchar_t buf[3];
    swprintf_s(buf, L"%02X", value);
    return buf;
}

bool TryParseHexColor(const std::wstring& text, wui::Color* outColor) {
    std::wstring hex = TrimWs(text);
    if (!hex.empty() && hex.front() == L'#') {
        hex.erase(0, 1);
    }
    if (hex.size() != 6 && hex.size() != 8) {
        return false;
    }
    try {
        uint32_t value = std::stoul(hex, nullptr, 16);
        uint8_t a = hex.size() == 8 ? static_cast<uint8_t>((value >> 24) & 0xFF) : 255;
        uint8_t r = static_cast<uint8_t>((value >> 16) & 0xFF);
        uint8_t g = static_cast<uint8_t>((value >> 8) & 0xFF);
        uint8_t b = static_cast<uint8_t>(value & 0xFF);
        *outColor = wui::ColorHelper::FromArgb(a, r, g, b);
        return true;
    } catch (...) {
        return false;
    }
}
// Parses a color name like "red", "blue", etc.
bool TryParseNamedColor(const std::wstring& text, wui::Color* outColor) {
    std::wstring lower = ToLowerCopy(text);
    if (lower == L"black") { *outColor = wui::ColorHelper::FromArgb(255,0,0,0); return true; }
    if (lower == L"white") { *outColor = wui::ColorHelper::FromArgb(255,255,255,255); return true; }
    if (lower == L"red")   { *outColor = wui::ColorHelper::FromArgb(255,255,0,0);   return true; }
    if (lower == L"green") { *outColor = wui::ColorHelper::FromArgb(255,0,128,0);   return true; }
    if (lower == L"blue")  { *outColor = wui::ColorHelper::FromArgb(255,0,0,255);   return true; }
    if (lower == L"yellow"){ *outColor = wui::ColorHelper::FromArgb(255,255,255,0); return true; }
    if (lower == L"orange"){ *outColor = wui::ColorHelper::FromArgb(255,255,165,0); return true; }
    if (lower == L"gray" || lower == L"grey") { *outColor = wui::ColorHelper::FromArgb(255,128,128,128); return true; }
    if (lower == L"purple"){ *outColor = wui::ColorHelper::FromArgb(255,128,0,128); return true; }
    if (lower == L"pink")  { *outColor = wui::ColorHelper::FromArgb(255,255,192,203); return true; }
    // Add more if needed...
    return false;
}

// Combines hex/name parsing with opacity
bool ParseBarColor(const std::wstring& text, int opacity, wui::Color* outColor) {
    wui::Color baseColor{};
    if (text.size() == 7 && text.front() == L'#') { // #RRGGBB
        if (!TryParseHexColor(text, &baseColor)) return false;
    } else if (text.size() == 9 && text.front() == L'#') { // #AARRGGBB
        if (!TryParseHexColor(text, &baseColor)) return false;
    } else {
        if (!TryParseNamedColor(text, &baseColor)) return false;
    }
    // Combine with opacity (0-100)
    uint8_t alpha = static_cast<uint8_t>((opacity * 255) / 100);
    *outColor = wui::ColorHelper::FromArgb(alpha, baseColor.R, baseColor.G, baseColor.B);
    return true;
}
// RefreshTaskList() ends by re-applying styles, and applying an IconTint* style
// asks for a refresh -- so without this guard the two would call each other
// forever.
bool g_applyingStyles = false;

void ApplyRuleList(const std::vector<ControlStyleRule>& rules) {
    for (const auto& rule : rules) {
        std::wstring ruleTarget = TrimWs(rule.target);
        if (ruleTarget.empty() || ruleTarget.starts_with(L"//")) {
            continue;
        }

        if (ruleTarget == L"TaskButton") {
            for (const auto& style : rule.styles) {
                std::wstring trimmed = TrimWs(style);
                if (trimmed.empty() || trimmed.starts_with(L"//")) {
                    continue;
                }
                size_t eq = trimmed.find(L'=');
                if (eq == std::wstring::npos) {
                    continue;
                }
                std::wstring prop = TrimWs(trimmed.substr(0, eq));
                std::wstring value = TrimWs(trimmed.substr(eq + 1));

                if (prop == L"IconTintColor") {
                    wui::Color color;
                    if (TryParseHexColor(value, &color)) {
                        g_iconTintColor = color;
                    }
                    continue;
                }
                if (prop == L"IconTintOpacity") {
                    try {
                        g_iconTintOpacity = std::clamp(std::stod(value), 0.0, 1.0);
                    } catch (...) {
                    }
                    continue;
                }

                for (auto& [hwnd, button] : g_taskButtonsByHwnd) {
                    ApplySingleStyleToElement(button, style);
                }
            }
            continue;
        }

        size_t start = 0;
        while (start <= ruleTarget.size()) {
            size_t comma = ruleTarget.find(L',', start);
            std::wstring name = TrimWs(ruleTarget.substr(
                start, comma == std::wstring::npos ? std::wstring::npos : comma - start));
            if (!name.empty()) {
                auto it = g_namedElements.find(name);
                bool applied = false;
                if (it != g_namedElements.end()) {
                    for (const auto& style : rule.styles) {
                        ApplySingleStyleToElement(it->second, style);
                    }
                    applied = true;
                }
                auto matches = ResolveGeneralTarget(name);
                for (auto& element : matches) {
                    if (applied && it != g_namedElements.end() && element == it->second) {
                        continue;
                    }
                    for (const auto& style : rule.styles) {
                        ApplySingleStyleToElement(element, style);
                    }
                    applied = true;
                }
                if (!applied) {
                    Wh_Log(L"Unknown control style target: %s", name.c_str());
                }
            }
            if (comma == std::wstring::npos) {
                break;
            }
            start = comma + 1;
        }
    }
}

void ApplyAllControlStyles() {
    if (g_applyingStyles) {
        return;
    }
    g_applyingStyles = true;
    struct Guard {
        ~Guard() { g_applyingStyles = false; }
    } guard;

    // Compared by value at the end rather than "a tint rule was seen", so a
    // rule that re-states the current tint doesn't trigger a pointless icon
    // rebuild on every styling pass.
    const wui::Color previousTintColor = g_iconTintColor;
    const double previousTintOpacity = g_iconTintOpacity;

    // Built-ins always applied.
    ApplyRuleList(BuiltInStyles());
    // Theme styles applied after built-ins, before user custom styles.
    ApplyRuleList(g_themeStyleRules);
    ApplyRuleList(g_controlStyleRules);

    bool tintChanged = previousTintColor.A != g_iconTintColor.A ||
                       previousTintColor.R != g_iconTintColor.R ||
                       previousTintColor.G != g_iconTintColor.G ||
                       previousTintColor.B != g_iconTintColor.B ||
                       previousTintOpacity != g_iconTintOpacity;
    if (tintChanged) {
        g_applyingStyles = false;
        RefreshTaskList(/*forceIconRegeneration=*/true);
    }
}

void ApplyVisibilitySettings() {
    auto setVis = [](PCWSTR name, bool visible) {
        auto it = g_namedElements.find(name);
        if (it != g_namedElements.end()) {
            it->second.Visibility(visible ? Visibility::Visible : Visibility::Collapsed);
        }
    };
    setVis(L"StartButton", g_settings.showStartButton);
    setVis(L"SearchButton", g_settings.showSearchButton);
    setVis(L"TaskListPanel", g_settings.showTaskList);
    setVis(L"DisplayButton", g_settings.showDisplayButton);
    setVis(L"SoundButton", g_settings.showSoundButton);
    setVis(L"WifiButton", g_settings.showWifiButton);
    setVis(L"BluetoothButton", g_settings.showBluetoothButton);
    setVis(L"TrayButton", g_settings.showTrayButton);
    setVis(L"BatteryButton", g_settings.showBatteryButton);
    setVis(L"ClockButton", g_settings.showClock || g_settings.showDate);
}

// Uses Windows' own native date/time format-picture tokens, so token
// substitution, locale awareness, and pass-through of anything that isn't a
// recognized format letter -- including emoji -- all come for free.
std::wstring FormatClockText() {
    SYSTEMTIME st;
    GetLocalTime(&st);
    std::wstring result;

    if (g_settings.showDate && !g_settings.dateFormat.empty()) {
        wchar_t buf[128]{};
        if (GetDateFormatEx(LOCALE_NAME_USER_DEFAULT, 0, &st, g_settings.dateFormat.c_str(), buf,
                            ARRAYSIZE(buf), nullptr)) {
            result += buf;
        }
    }
    if (g_settings.showClock && !g_settings.timeFormat.empty()) {
        if (!result.empty()) {
            result += L"  ";
        }
        wchar_t buf[128]{};
        if (GetTimeFormatEx(LOCALE_NAME_USER_DEFAULT, 0, &st, g_settings.timeFormat.c_str(), buf,
                            ARRAYSIZE(buf))) {
            result += buf;
        }
    }
    return result;
}

// ============================================================================
// Vector icon library
// ============================================================================

namespace icons {

constexpr PCWSTR kSearchOutline =
    LR"(M57 52 C56.19046875 52.72832031 55.3809375 53.45664062 54.546875 54.20703125 C40.10361222 67.81702887 30.64664849 86.15644954 25 105 C24.6803125 106.06347656 24.360625 107.12695312 24.03125 108.22265625 C16.58663689 138.2423693 23.68076575 168.81926969 38.80615234 195.05957031 C41.71591037 199.78896843 45.29875959 203.88454135 49 208 C49.91652344 209.12921875 49.91652344 209.12921875 50.8515625 210.28125 C68.5131584 231.62547653 96.08358271 243.50615679 123.125 246.6875 C124.74869154 246.81038809 126.37391538 246.9144166 128 247 C128.83917969 247.05285156 129.67835938 247.10570312 130.54296875 247.16015625 C154.16645853 247.92650359 177.39880592 241.06746272 197 228 C201.5066721 227.99008184 203.70652636 230.24535331 206.72486877 233.24940491 C207.31154831 233.85360764 207.89822784 234.45781036 208.50268555 235.08032227 C209.13418518 235.71445038 209.76568481 236.34857849 210.4163208 237.00192261 C212.49913494 239.09770777 214.56764156 241.20706587 216.63671875 243.31640625 C218.08107742 244.77330497 219.52647068 246.22917867 220.97285461 247.68406677 C224.7765582 251.51437308 228.56896585 255.35565517 232.35827637 259.20019531 C238.43205221 265.36023463 244.51935924 271.50683234 250.61310768 277.64710808 C252.74399785 279.79851473 254.86797457 281.95657939 256.99137878 284.1153717 C258.28918977 285.42683165 259.58721759 286.73807707 260.88549805 288.04907227 C261.47825027 288.65529922 262.0710025 289.26152618 262.68171692 289.88612366 C264.08356728 291.29580791 265.53702333 292.65385981 267 294 C267.66 294 268.32 294 269 294 C269.20625 294.53109375 269.4125 295.0621875 269.625 295.609375 C271.57961192 299.00773436 274.22123717 301.51055928 277 304.25 C277.5465625 304.80429687 278.093125 305.35859375 278.65625 305.9296875 C281.32600768 308.99215775 281.32600768 308.99215775 285 310 C285.2475 310.5775 285.495 311.155 285.75 311.75 C287.18299988 314.32939979 288.78424676 316.06121592 291 318 C291.66 318 292.32 318 293 318 C293.2475 318.5775 293.495 319.155 293.75 319.75 C295.18299988 322.32939979 296.78424676 324.06121592 299 326 C299.66 326 300.32 326 301 326 C301.37318359 326.89138672 301.37318359 326.89138672 301.75390625 327.80078125 C303.18065713 330.31884001 304.60427079 331.49732092 306.9375 333.1875 C307.62714844 333.69667969 308.31679688 334.20585937 309.02734375 334.73046875 C313.67685724 337.72272991 318.66572217 337.54609076 324 337 C328.74720408 335.71053453 332.06387778 333.44772735 335 329.5 C337.88172003 324.21848582 338.09197728 318.84438543 337 313 C333.47379409 305.24701748 327.05944819 299.87011477 320.69140625 294.46484375 C317.13837637 291.38773935 314.0090352 288.03609172 311 284.4375 C308.56804854 281.55772117 305.99445392 279.30342609 303 277 C300.13833918 274.28509102 297.52553788 271.46039445 295 268.4375 C292.56804854 265.55772117 289.99445392 263.30342609 287 261 C284.48184682 258.60468356 282.1919593 256.24768942 280.0625 253.5 C277.65085484 250.57679375 274.9327108 248.39404964 272 246 C269.35491029 243.48393905 266.97653101 240.97082234 264.6875 238.125 C262.82989036 235.85762352 260.88289666 233.97572183 258.640625 232.08984375 C253.1355835 227.32085761 247.94619112 222.24091526 242.77026367 217.12036133 C241.62448863 215.9912938 240.47335579 214.86763957 239.31713867 213.74926758 C237.62752384 212.11439446 235.95494105 210.46368647 234.28515625 208.80859375 C233.76856949 208.31467636 233.25198273 207.82075897 232.71974182 207.31187439 C229.46140139 204.03396808 228.10342777 201.64830539 228 197 C228.63330078 195.05639648 228.63330078 195.05639648 229.6953125 193.37109375 C230.07921143 192.73574707 230.46311035 192.10040039 230.85864258 191.44580078 C231.27654053 190.78306152 231.69443848 190.12032227 232.125 189.4375 C246.65329549 164.92538453 250.82932914 134.7979171 243.78320312 107.10742188 C238.87571286 89.22649712 231.0840638 74.13264973 219 60 C218.28457031 59.08541016 218.28457031 59.08541016 217.5546875 58.15234375 C201.51340633 38.03199393 175.29984029 27.00163468 150.71484375 22.3828125 C115.59304879 18.76353184 83.01428234 27.91921162 57 52 Z)";

constexpr PCWSTR kSearchLens =
    LR"(M0 0 C16.93975128 14.36920693 28.26770469 33.59992875 31.51196289 55.73510742 C33.52749766 81.02050274 28.39560109 104.20401079 11.93774414 123.94213867 C11.13723633 124.86381836 10.33672852 125.78549805 9.51196289 126.73510742 C8.54323242 127.8546582 8.54323242 127.8546582 7.55493164 128.99682617 C-6.43622666 143.98945686 -28.05664112 154.22232888 -48.50756836 154.97338867 C-74.68429045 155.47604695 -97.03823145 148.6076491 -116.66381836 130.58666992 C-133.25860681 113.74313732 -142.24229311 91.24241269 -142.86303711 67.73510742 C-142.1704908 42.54373556 -132.21994474 21.51955403 -114.48803711 3.73510742 C-113.85510742 3.07897461 -113.22217773 2.4228418 -112.57006836 1.74682617 C-82.51997673 -27.10922208 -31.27019387 -25.48510426 0 0 Z)";

// 24x24 viewport for everything below.
constexpr PCWSTR kBrightnessStroke =
    L"M8 12 A4 4 0 1 1 16 12 A4 4 0 1 1 8 12 Z "
    L"M12 1.6 L12 4.1 M12 19.9 L12 22.4 M1.6 12 L4.1 12 M19.9 12 L22.4 12 "
    L"M4.9 4.9 L6.7 6.7 M17.3 17.3 L19.1 19.1 M19.1 4.9 L17.3 6.7 M6.7 17.3 L4.9 19.1";

constexpr PCWSTR kSpeakerFill = L"M4 9.2 L7.6 9.2 L12.4 5 L12.4 19 L7.6 14.8 L4 14.8 Z";
constexpr PCWSTR kSpeakerWaves =
    L"M15.4 9.4 A3.6 3.6 0 0 1 15.4 14.6 M18 6.9 A7.2 7.2 0 0 1 18 17.1";
constexpr PCWSTR kSpeakerMuted = L"M15.8 9.8 L20.6 14.6 M20.6 9.8 L15.8 14.6";

constexpr PCWSTR kWifiStroke =
    L"M2.6 8.7 A13.4 13.4 0 0 1 21.4 8.7 M5.8 12.1 A8.9 8.9 0 0 1 18.2 12.1 "
    L"M9 15.4 A4.4 4.4 0 0 1 15 15.4";
constexpr PCWSTR kWifiDot = L"M10.4 19.1 A1.6 1.6 0 1 1 13.6 19.1 A1.6 1.6 0 1 1 10.4 19.1 Z";

constexpr PCWSTR kBluetoothStroke = L"M7.2 7.6 L16.8 16.4 L12 21 L12 3 L16.8 7.6 L7.2 16.4";

constexpr PCWSTR kChevronUp = L"M6.5 14.5 L12 9 L17.5 14.5";
constexpr PCWSTR kChevronDown = L"M6.5 9.5 L12 15 L17.5 9.5";

constexpr PCWSTR kMoonFill =
    L"M12.5 3 C8 3.7 4.5 7.6 4.5 12.3 C4.5 17.5 8.7 21.7 13.9 21.7 "
    L"C17.5 21.7 20.6 19.6 22 16.6 C21 17 19.9 17.2 18.7 17.2 "
    L"C14 17.2 10.2 13.4 10.2 8.7 C10.2 6.6 11 4.6 12.5 3 Z";



constexpr PCWSTR kLockFill = L"M6.6 10.6 L17.4 10.6 L17.4 20.4 L6.6 20.4 Z";
constexpr PCWSTR kLockShackle = L"M9.2 10.6 L9.2 7.7 A2.8 2.8 0 0 1 14.8 7.7 L14.8 10.6";

constexpr PCWSTR kPlayFill = L"M8 5 L18.5 12 L8 19 Z";
constexpr PCWSTR kPauseFill = L"M8 5 L11 5 L11 19 L8 19 Z M13 5 L16 5 L16 19 L13 19 Z";
constexpr PCWSTR kPrevFill = L"M17 5 L17 19 L7.5 12 Z M6 5 L8 5 L8 19 L6 19 Z";
constexpr PCWSTR kNextFill = L"M7 5 L7 19 L16.5 12 Z M16 5 L18 5 L18 19 L16 19 Z";

constexpr PCWSTR kHeadphoneStroke = L"M4.6 15.2 L4.6 12 A7.4 7.4 0 0 1 19.4 12 L19.4 15.2";
constexpr PCWSTR kHeadphoneFill =
    L"M3 14.4 L6.6 14.4 L6.6 20.2 L3 20.2 Z M17.4 14.4 L21 14.4 L21 20.2 L17.4 20.2 Z";

constexpr PCWSTR kAppFill = L"M7 5 L17 5 C18.1 5 19 5.9 19 7 L19 17 C19 18.1 18.1 19 17 19 L7 19 C5.9 19 5 18.1 5 17 L5 7 C5 5.9 5.9 5 7 5 Z M9 9 L9 15 L15 15 L15 9 Z";
constexpr PCWSTR kCheckStroke = L"M5 12.5 L10 17.5 L19 6.5";

constexpr PCWSTR kBatteryChargingPath = L"M142.78637695 10.96240234 C141.17708313 10.97440422 141.17708313 10.97440422 139.53527832 10.98664856 C136.01954307 11.014333 132.50393988 11.04988476 128.98828125 11.0859375 C126.5306986 11.10773218 124.07311285 11.12918059 121.61552429 11.15029907 C116.47891354 11.19566131 111.3423582 11.2448676 106.20581055 11.296875 C99.62879233 11.36320558 93.05173258 11.42045566 86.47460651 11.47490501 C81.40535568 11.51778152 76.33615513 11.56519453 71.26695824 11.61400223 C68.84171962 11.63685968 66.41646796 11.6583737 63.99120522 11.67850876 C60.60255353 11.70737567 57.21401617 11.74228894 53.82543945 11.77880859 C52.32985756 11.78979836 52.32985756 11.78979836 50.80406189 11.80101013 C40.07295485 11.82147996 40.07295485 11.82147996 31 17 C29.34584071 17.81747839 27.67761869 18.60681419 26 19.375 C21.32458012 21.62587079 19.00418864 23.8123431 16 28 C15.505 28.46019531 15.01 28.92039062 14.5 29.39453125 C12.70563431 31.31506328 11.92552539 33.13443761 10.9375 35.5625 C9.80810841 38.32474691 9.13254655 39.86745345 7 42 C4.23748009 52.46579925 4.70631453 63.52981517 4.734375 74.2578125 C4.73302409 76.09618175 4.73108002 77.93455066 4.72857666 79.7729187 C4.72564692 83.60369304 4.72985421 87.4343734 4.73925781 91.26513672 C4.75066724 96.14796467 4.74406968 101.03057304 4.73211288 105.91339302 C4.72484969 109.70203271 4.72720232 113.49062122 4.73236847 117.27926254 C4.73369182 119.07879539 4.73210398 120.87833305 4.72740936 122.67786026 C4.20497539 140.29241009 4.20497539 140.29241009 11 156 C11.4125 157.093125 11.825 158.18625 12.25 159.3125 C15.9550748 168.1733138 22.86210544 172.98796288 31.36328125 176.80859375 C34 178 34 178 37 180 C41.14062832 181.14519343 45.27412868 181.16662857 49.54077148 181.19287109 C50.28288879 181.20104858 51.0250061 181.20922607 51.78961182 181.21765137 C54.23343114 181.24236558 56.67719312 181.25898136 59.12109375 181.2734375 C60.3772433 181.28142302 60.3772433 181.28142302 61.65876961 181.28956985 C66.08768928 181.31623503 70.51658136 181.3356348 74.94555664 181.35009766 C78.60531499 181.36345622 82.26451833 181.39135302 85.92407227 181.43212891 C90.35193358 181.48145521 94.77929031 181.50621747 99.20741463 181.51332474 C100.89111919 181.52003173 102.57481271 181.53523957 104.25835991 181.55921555 C106.6161143 181.59089006 108.97192209 181.59090208 111.32983398 181.58349609 C112.02041397 181.59990143 112.71099396 181.61630676 113.42250061 181.63320923 C118.97091683 181.56208341 121.23382868 179.77406909 125 176 C126.38550197 173.22899606 126.1877467 171.0665294 126 168 C124.36160505 164.85755166 124.36160505 164.85755166 121.80039978 164.323349 C118.57920538 163.95141282 115.43696246 163.85219823 112.19360352 163.82641602 C111.52951477 163.819729 110.86542603 163.81304199 110.18121338 163.80615234 C108.74623466 163.79215877 107.31123442 163.78024688 105.87622261 163.77020454 C103.60156014 163.75370793 101.32705716 163.73046792 99.05247498 163.70530701 C92.58771797 163.63476358 86.12289734 163.57444356 79.65795898 163.52319336 C75.69690248 163.49132096 71.73603341 163.45003013 67.77512932 163.40299797 C66.27014483 163.38703403 64.76511731 163.37468672 63.26007271 163.36609077 C61.15178609 163.35387309 59.04393683 163.32969862 56.93579102 163.30297852 C55.73841812 163.29267105 54.54104523 163.28236359 53.30738831 163.27174377 C47.59214038 162.80216388 43.17315947 160.69017682 38.25 157.875 C34.82517335 155.71401309 34.82517335 155.71401309 31 155 C30.82984375 154.43152344 30.6596875 153.86304688 30.484375 153.27734375 C29.27369586 149.41991395 28.09916463 145.95556868 25.875 142.5625 C21.39451112 135.10789471 22.85257691 124.34577716 22.8671875 115.96875 C22.86575241 114.12295349 22.86575241 114.12295349 22.86428833 112.23986816 C22.86361071 109.64461211 22.86543573 107.04935432 22.86962891 104.45410156 C22.87494611 100.53967342 22.86968864 96.62536194 22.86328125 92.7109375 C22.85799072 81.98583044 22.8572883 71.2501886 23.2578125 60.53125 C23.27141815 59.8362439 23.2850238 59.14123779 23.29904175 58.4251709 C23.53851667 53.58754906 25.1108956 50.81465162 28 47 C28.65406115 45.63858382 29.28410955 44.26499945 29.875 42.875 C31.81384821 38.89846034 34.00492862 37.03830172 38 35 C39.66519519 34.65939189 41.33157466 34.32441604 43 34 C43.66 33.01 44.32 32.02 45 31 C47.81655606 29.96457878 50.49037417 29.87013573 53.47154236 29.85523987 C54.36171555 29.84834137 55.25188873 29.84144287 56.16903687 29.83433533 C57.6224498 29.83041527 57.6224498 29.83041527 59.10522461 29.82641602 C60.13165573 29.819729 61.15808685 29.81304199 62.21562195 29.80615234 C64.44402317 29.7921071 66.67243873 29.78021137 68.90086174 29.77020454 C72.42692664 29.75379182 75.95288506 29.73055447 79.47889709 29.70530701 C89.50402524 29.63453287 99.52919635 29.57436967 109.55444336 29.52319336 C115.686395 29.49141293 121.81822331 29.45014206 127.95007515 29.40299797 C130.28764187 29.38697529 132.62523699 29.37465497 134.96284294 29.36609077 C138.23048313 29.35394502 141.4978329 29.32979855 144.76538086 29.30297852 C145.73229858 29.30238937 146.69921631 29.30180023 147.69543457 29.30119324 C154.09373676 29.27102952 154.09373676 29.27102952 160 27 C161.73473712 25.05874656 163.4045651 23.05727132 165 21 C165.928125 20.195625 165.928125 20.195625 166.875 19.375 C168.23598732 18.02901901 168.23598732 18.02901901 167.875 15.9375 C167.04747907 13.6488757 167.04747907 13.6488757 164 12 C157.06525992 10.62803427 149.82256207 10.88163761 142.78637695 10.96240234 Z M210 15 C208.27686302 18.15908446 207 20.61498231 207 24.25 C208.51376794 26.89909389 210.26720869 27.66822921 213 29 C215.5944929 29.29671235 217.95214666 29.43774322 220.54418945 29.43237305 C221.28619095 29.4425647 222.02819244 29.45275635 222.79267883 29.46325684 C225.23295702 29.4933298 227.67284229 29.5026438 230.11328125 29.51171875 C231.81008706 29.52856971 233.50688043 29.54672052 235.20365906 29.56611633 C239.66041167 29.61357735 244.1171138 29.64341185 248.57403564 29.66955566 C253.12563039 29.6993809 257.67705125 29.74590449 262.22851562 29.79101562 C271.15225271 29.87700434 280.07602563 29.94436931 289 30 C289 30.66 289 31.32 289 32 C290.62249941 32.83819158 292.24808497 33.6704116 293.875 34.5 C294.77992188 34.9640625 295.68484375 35.428125 296.6171875 35.90625 C298.86596859 37.13905589 298.86596859 37.13905589 301 37 C301 37.66 301 38.32 301 39 C301.66 39 302.32 39 303 39 C303.27263672 40.06541016 303.27263672 40.06541016 303.55078125 41.15234375 C303.92783203 42.53099609 303.92783203 42.53099609 304.3125 43.9375 C304.55613281 44.85402344 304.79976562 45.77054688 305.05078125 46.71484375 C305.36402344 47.46894531 305.67726563 48.22304688 306 49 C306.99 49.33 307.98 49.66 309 50 C309.73884339 55.29182539 310.12962639 60.38748524 310.14526367 65.74194336 C310.14862228 66.4699263 310.1519809 67.19790924 310.15544128 67.94795227 C310.1648563 70.32860063 310.16688939 72.70917799 310.16796875 75.08984375 C310.17118717 76.76078825 310.17455168 78.43173247 310.17805481 80.10267639 C310.18402192 83.5950689 310.18589713 87.08743674 310.18530273 90.57983398 C310.18520108 95.03589042 310.19886721 99.49178764 310.21607494 103.94780636 C310.22723758 107.395805 310.22922066 110.84376561 310.22869301 114.29178047 C310.22986616 115.93387972 310.23425024 117.57598018 310.24202538 119.21806145 C310.33300579 140.41072254 310.33300579 140.41072254 305 148 C302.09330388 151.68618134 302.09330388 151.68618134 301 156 C300.39671875 156.18175781 299.7934375 156.36351562 299.171875 156.55078125 C294.84992148 157.91010534 291.5836203 159.18429834 288 162 C284.76530252 162.80409702 281.73401537 163.1269989 278.40820312 163.13803101 C277.51665527 163.14348434 276.62510742 163.14893768 275.70654297 163.15455627 C274.74119629 163.1555986 273.77584961 163.15664093 272.78125 163.15771484 C271.75467285 163.16279556 270.7280957 163.16787628 269.67041016 163.17311096 C267.44475147 163.18379766 265.21908214 163.19242448 262.9934082 163.19923019 C259.47146002 163.21103314 255.94963367 163.23048813 252.42773438 163.25234985 C242.41540567 163.31393086 232.40309094 163.36806737 222.390625 163.40136719 C216.26396112 163.4222914 210.13752196 163.45757201 204.01098633 163.50212479 C201.67621501 163.51629753 199.34140398 163.52501008 197.0065918 163.52817917 C193.74373777 163.53307827 190.48148267 163.55620956 187.21875 163.58349609 C186.25340332 163.58048996 185.28805664 163.57748383 184.29345703 163.5743866 C179.40630811 163.63519442 175.9328597 163.74721809 172 167 C171.29848848 168.3153341 170.63474075 169.65117592 170 171 C168.37253306 172.70496537 166.71108689 174.37897032 165 176 C166.22614619 178.69754185 166.22614619 178.69754185 169 181 C172.83199407 181.77858525 176.64420559 181.65120665 180.54223633 181.59936523 C181.72151199 181.60330795 182.90078766 181.60725067 184.11579895 181.61131287 C187.34773336 181.61687594 180.57860841 181.6001162 193.81040406 181.57283282 C197.19704617 181.54897944 200.58367154 181.55194754 203.97038269 181.55152893 C209.66082572 181.5469004 215.35093428 181.5232624 221.04125977 181.48706055 C227.60890937 181.44538681 234.17625746 181.42774915 240.74403173 181.4244408 C247.74698309 181.42083189 254.74982676 181.40193755 261.75273299 181.37732053 C263.7645718 181.37110906 265.77640239 181.36746183 267.78824806 181.36428642 C271.5488276 181.35522822 275.30908746 181.33282085 279.06958008 181.30639648 C280.17891907 181.30573181 281.28825806 181.30506714 282.43121338 181.30438232 C289.36568067 181.23724889 295.52836314 180.82816436 301.5625 177.0625 C303.75113657 175.72581592 305.7548296 174.66787498 308.125 173.625 C311.67118608 171.62063395 313.50499996 169.198718 316 166 C316.86625 165.21625 317.7325 164.4325 318.625 163.625 C321.22785418 160.74816117 322.12216426 158.46600637 323.3125 154.84765625 C324 153 324 153 325.625 150.875 C329.00339559 146.26809693 328.148245 139.40497041 328.14526367 133.90869141 C328.14862228 133.01358353 328.1519809 132.11847565 328.15544128 131.19624329 C328.16491167 128.2480021 328.16689238 125.29981794 328.16796875 122.3515625 C328.17118387 120.29421974 328.17454814 118.2368772 328.17805481 116.17953491 C328.18403616 111.87066111 328.18589571 107.56180734 328.18530273 103.25292969 C328.18520113 97.74298694 328.19885618 92.23317309 328.21607494 86.72326088 C328.22721548 82.47493911 328.22922131 78.22664832 328.22869301 73.97831345 C328.22986991 71.94742768 328.23427538 69.91654103 328.24202538 67.88566971 C328.2518674 65.04038816 328.24893067 62.19538406 328.24291992 59.35009766 C328.24853943 58.51866745 328.25415894 57.68723724 328.25994873 56.83061218 C328.21678549 48.75421536 326.79660602 42.68617809 322 36 C321.525625 34.989375 321.05125 33.97875 320.5625 32.9375 C318.79003185 29.60525987 316.7584118 27.56138238 314 25 C313.278125 24.175 312.55625 23.35 311.8125 22.5 C308.52583763 19.57852233 305.17975811 17.9418077 301.20703125 16.11328125 C299 15 299 15 297 13 C292.4913189 11.95429964 288.02796437 11.84642592 283.42163086 11.83886719 C282.6875148 11.83390228 281.95339874 11.82893738 281.19703674 11.82382202 C278.78629599 11.80930309 276.3756209 11.80241796 273.96484375 11.796875 C272.28145853 11.79111791 270.59807332 11.78536025 268.91468811 11.77960205 C265.39356177 11.76912331 261.87245808 11.76325929 258.35131836 11.75976562 C253.84317828 11.75426777 249.3353382 11.73019924 244.82729244 11.70179367 C241.35371523 11.68316449 237.88021055 11.67796169 234.4065876 11.67642975 C232.74442455 11.67340659 231.08226267 11.66538939 229.42014885 11.65224457 C227.09619085 11.6351674 224.77297009 11.63706732 222.44897461 11.64355469 C221.76572113 11.6343399 221.08246765 11.62512512 220.37850952 11.6156311 C216.08891372 11.65309423 213.38542765 12.33687086 210 15 Z M183 17 C181.638652 18.6263892 180.4924368 20.28685964 179.37109375 22.0859375 C177.51705048 24.67420303 175.25638564 26.75712566 173 29 C171.58150406 30.68956288 170.18590351 32.39858283 168.8125 34.125 C165.61698468 38.08084591 162.3235321 41.887778 158.875 45.625 C155.19213187 49.62811753 151.75427407 53.72189314 148.47265625 58.05859375 C147.21309863 59.71907157 145.8933794 61.3336197 144.5625 62.9375 C142.7987141 64.83444235 142.7987141 64.83444235 143 67 C142.34 67 141.68 67 141 67 C139.49457477 68.68006144 139.49457477 68.68006144 137.9375 70.875 C135.27143271 74.41128052 132.49312219 77.73570605 129.5 81 C125.40635025 85.49048314 121.64424681 90.14100425 118 95 C117.25363281 95.94359375 117.25363281 95.94359375 116.4921875 96.90625 C113.71053541 100.5303453 112.10748388 103.37819314 112 108 C114.98358056 110.98358056 117.94057589 111.11228582 122.03417969 111.20532227 C123.20494049 111.2352401 123.20494049 111.2352401 124.39935303 111.26576233 C125.23178528 111.28247482 126.06421753 111.29918732 126.921875 111.31640625 C128.21185394 111.3475779 128.21185394 111.3475779 129.52789307 111.37937927 C132.26851598 111.44463739 135.00923275 111.50364917 137.75 111.5625 C139.6106867 111.60570781 141.47136398 111.64932282 143.33203125 111.69335938 C147.88792523 111.80029442 152.4439134 111.90162851 157 112 C157.89780378 115.92743473 158.01142766 117.81162173 156.3125 121.5 C154.57930061 125.49052782 153.21656882 129.44698279 152 133.625 C150.67691451 138.06122783 149.1378689 141.97431984 146.9375 146.03125 C145.45614675 149.14209183 144.65016919 152.42863124 143.72265625 155.73828125 C142.85961356 158.43937162 141.53545956 160.62701704 140 163 C139.10277259 165.88699703 138.83802149 168.54585933 138.8125 171.5625 C138.79123047 172.70912109 138.79123047 172.70912109 138.76953125 173.87890625 C138.8676625 176.30153503 138.8676625 176.30153503 141 179 C146.33518568 178.61891531 149.39362118 176.78756054 153 173 C155.0752985 170.53714575 157.07508085 168.028651 159.0546875 165.48828125 C161.59999003 162.23254287 164.28734032 159.11619583 167 156 C171.23911575 151.11606101 175.34171041 146.13395379 179.41796875 141.11328125 C181.29722108 138.82417223 183.22406388 136.58308028 185.17578125 134.35546875 C185.71638184 133.73615479 186.25698242 133.11684082 186.81396484 132.47875977 C187.87503018 131.26723058 188.94155491 130.06045538 190.01416016 128.85913086 C192.75835136 125.72026438 194.73190775 123.05485784 196 119 C196.66 119 197.32 119 198 119 C198 118.34 198 117.68 198 117 C198.99 116.67 199.98 116.34 201 116 C201.28875 115.4225 201.5775 114.845 201.875 114.25 C203.38375276 111.23249448 205.60759476 109.37519369 208 107 C216.92360182 97.75122693 216.92360182 97.75122693 221.0625 86.1875 C220.18797532 83.85608888 220.18797532 83.85608888 218.01098633 82.88916016 C213.90452291 81.67649983 210.02380769 81.64391616 205.76171875 81.5859375 C204.88095169 81.56657135 204.00018463 81.5472052 203.09272766 81.5272522 C200.29108234 81.46767149 197.48939544 81.42115735 194.6875 81.375 C192.78318562 81.33680041 190.87888815 81.29774753 188.97460938 81.2578125 C184.31654428 81.16198767 179.65839832 81.0778774 175 81 C175.11980082 79.56691801 175.25090915 78.13477859 175.38671875 76.703125 C175.45850342 75.90551758 175.53028809 75.10791016 175.60424805 74.28613281 C176.09058513 71.47671857 177.36335446 69.44722953 178.89453125 67.0703125 C180.88188165 63.34841952 181.81872466 59.24631233 182.99609375 55.2109375 C184.0965728 51.69111736 185.49769863 48.45240916 187.0625 45.12109375 C188.36928083 42.16450212 189.30956794 39.1213267 190.2734375 36.0390625 C191 34 191 34 192.6875 31.1875 C194.69919578 26.3019531 194.60177219 21.22056917 194 16 C193.01 15.01 193.01 15.01 192 14 C188.11465884 14 186.30690973 14.94971597 183 17 Z M40.25 47.4375 C39.67765625 48.01371094 39.1053125 48.58992188 38.515625 49.18359375 C34.89354551 53.52448798 34.69419385 57.4309199 34.67773438 62.94628906 C34.66780457 63.74356918 34.65787476 64.5408493 34.64764404 65.36228943 C34.61846434 67.99676994 34.60480691 70.63100951 34.59375 73.265625 C34.58224052 75.10162357 34.57072513 76.93762211 34.5592041 78.77362061 C34.53820131 82.62366859 34.52650524 86.47363327 34.51953125 90.32373047 C34.50857385 95.24451598 34.46054093 100.16419123 34.40358734 105.08462906 C34.36623722 108.87832802 34.35591827 112.67176233 34.3528595 116.46562958 C34.34682563 118.2788677 34.33083306 120.09210222 34.30448914 121.905159 C33.75866375 134.19544665 33.75866375 134.19544665 39.39929199 144.6239624 C39.92752563 145.07805481 40.45575928 145.53214722 41 146 C41.53306763 146.50420471 42.06613525 147.00840942 42.61535645 147.52789307 C47.42550085 151.84073898 51.35310532 152.15156217 57.54492188 152.16113281 C58.31107315 152.16609772 59.07722443 152.17106262 59.86659241 152.17617798 C62.40065406 152.19078206 64.93465318 152.19759747 67.46875 152.203125 C69.22847496 152.2088764 70.98819989 152.21463414 72.7479248 152.22039795 C76.43660779 152.2309019 80.12526901 152.23674903 83.81396484 152.24023438 C88.5454619 152.24572819 93.2766725 152.26977754 98.00807953 152.29820633 C101.64199676 152.31680553 105.27584448 152.32203678 108.9098053 152.32357025 C110.65456209 152.32660176 112.39931752 152.334647 114.14402771 152.34775543 C116.58126855 152.36476494 119.01780242 152.36294886 121.45507812 152.35644531 C122.17828751 152.3656601 122.90149689 152.37487488 123.6466217 152.3843689 C127.52306155 152.35693625 127.52306155 152.35693625 130.9029541 150.63983154 C132.41704887 148.37660777 133.20563843 146.08099194 134.0625 143.5 C135.15435124 140.23760111 136.07652599 137.88521102 138 135 C138.37167184 132.23979004 138.17850636 129.81147524 138 127 C135.46671408 124.46671408 134.52904722 124.73902139 131.01953125 124.69140625 C129.5200293 124.65370117 129.5200293 124.65370117 127.99023438 124.61523438 C125.89133242 124.58040318 123.79236886 124.54908563 121.69335938 124.52148438 C115.72122493 124.36462498 111.24983401 123.94244729 106 121 C103.6875 118.375 103.6875 118.375 102 116 C101.34 115.67 100.68 115.34 100 115 C97.56139933 108.93059388 97.4435222 103.00130696 100 97 C102.38628548 93.09833616 105.13443306 89.55964889 108 86 C108.68320313 85.13632813 109.36640625 84.27265625 110.0703125 83.3828125 C113.40111429 79.24081913 116.82569372 75.18780304 120.3203125 71.18359375 C121.18619995 70.19117676 121.18619995 70.19117676 122.06958008 69.17871094 C123.20365854 67.88307515 124.34133666 66.59057842 125.4831543 65.30175781 C128.64756442 61.69716176 131.36853637 58.02254573 134 54 C135.66666667 52.33333333 137.33333333 50.66666667 139 49 C140.05287814 47.36622358 141.0727001 45.70818403 142 44 C137.65023686 40.74376327 133.78528008 40.57130648 128.54125977 40.61181641 C127.70160675 40.60655945 126.86195374 40.60130249 125.99685669 40.59588623 C123.22818256 40.58270967 120.46008598 40.59141677 117.69140625 40.6015625 C115.76236359 40.5995409 113.83332167 40.59662874 111.90428162 40.59286499 C107.86344112 40.58844465 103.82279975 40.59486269 99.78198242 40.60888672 C94.60912733 40.6259669 89.43673961 40.61614293 84.26390171 40.59816933 C80.28070066 40.5873139 76.29760987 40.59078612 72.31440544 40.5985527 C70.4074695 40.60055044 68.50052347 40.59811045 66.59359932 40.59111404 C63.9248426 40.58347192 61.25683583 40.59514281 58.58813477 40.61181641 C57.80404739 40.60574371 57.01996002 40.59967102 56.21211243 40.59341431 C49.60963299 40.66960723 44.91357826 42.6348701 40.25 47.4375 Z M202.28515625 42.20703125 C200.74021704 44.36243276 199.86067379 46.4520321 198.9375 48.9375 C197.11943437 53.77837355 197.11943437 53.77837355 195.890625 56.05859375 C194.36604204 59.38191714 194.82486509 62.3805452 195 66 C195.33 66.66 195.66 67.32 196 68 C199.5925874 68.75592403 203.3424958 68.77020916 207 69 C208.69146106 69.13996424 210.38286314 69.28064228 212.07421875 69.421875 C213.88269879 69.55365318 215.69129463 69.68385139 217.5 69.8125 C218.38469971 69.88992432 219.26939941 69.96734863 220.1809082 70.04711914 C225.02709461 70.51080368 225.02709461 70.51080368 229.0703125 68.24609375 C231.37018552 62.68967784 230.20645561 57.11653431 227.984375 51.75 C226.49777433 49.10715436 224.47692447 47.69155818 222 46 C221.29875 45.278125 220.5975 44.55625 219.875 43.8125 C216.73985491 40.78185975 213.67423436 40.81732636 209.5 40.75 C208.06140625 40.71132813 208.06140625 40.71132813 206.59375 40.671875 C204.00957576 40.79306124 204.00957576 40.79306124 202.28515625 42.20703125 Z M339 69 C337.94873282 71.84913952 337.86928806 74.56489338 337.85473633 77.58154297 C337.84835648 78.48282928 337.84197662 79.3841156 337.83540344 80.31271362 C337.83429062 81.27944 337.8331778 82.24616638 337.83203125 83.2421875 C337.82870285 84.24331573 337.82537445 85.24444397 337.82194519 86.27590942 C337.81688588 88.39243104 337.81453656 90.50896062 337.81469727 92.62548828 C337.8125129 95.85592868 337.7943523 99.08602635 337.77539062 102.31640625 C337.77245605 104.37499883 337.77047168 106.43359304 337.76953125 108.4921875 C337.76234573 109.45486542 337.75516022 110.41754333 337.74775696 111.40939331 C337.76449062 116.34574083 338.05298422 120.37095431 340 125 C343.XXXXX";

constexpr PCWSTR kBatteryNotChargingPath = L"M0 0 C125.4 0 250.8 0 380 0 C380 65.01 380 130.02 380 197 C254.6 197 129.2 197 0 197 C0 131.99 0 66.98 0 0 Z M140 9 C138.34756727 13.17606838 137.71889382 16.79724146 137.5859375 21.2734375 C137.54726562 22.42714844 137.50859375 23.58085937 137.46875 24.76953125 C137.4378125 25.95933594 137.406875 27.14914063 137.375 28.375 C137.33632812 29.58800781 137.29765625 30.80101563 137.2578125 32.05078125 C137.16386214 35.03370521 137.07819193 38.01662825 137 41 C136.04738281 41.08636719 135.09476563 41.17273437 134.11328125 41.26171875 C132.85902344 41.40222656 131.60476562 41.54273438 130.3125 41.6875 C129.07113281 41.81511719 127.82976562 41.94273438 126.55078125 42.07421875 C122.281952 43.18721383 120.76559945 44.6144413 118 48 C117.40960938 48.69351563 116.81921875 49.38703125 116.2109375 50.1015625 C114.26613843 54.75653319 114.55874102 59.38400406 114.625 64.375 C114.63370117 65.44975586 114.64240234 66.52451172 114.65136719 67.63183594 C114.80633363 76.08893919 115.73692949 83.82342786 118 92 C118.25523438 93.04027344 118.51046875 94.08054687 118.7734375 95.15234375 C121.72881919 105.68597483 128.21940231 114.51908266 137 121 C137.47179687 121.50015625 137.94359375 122.0003125 138.4296875 122.515625 C140.14168222 124.13392846 141.58190992 124.79858307 143.75 125.6875 C148.68751663 128.11432496 152.11226115 131.30340016 155 136 C155.93156787 139.70354377 155.91116969 143.23156122 155.83203125 147.03515625 C155.84335335 148.59138954 155.84335335 148.59138954 155.85490417 150.17906189 C155.87332986 153.47443888 155.8450592 156.76726775 155.8125 160.0625 C155.80647485 163.36066666 155.81032712 166.65850548 155.82365417 169.95664978 C155.8291101 172.00236223 155.82202535 174.04815887 155.80049133 176.09376526 C155.78489211 181.44579824 156.00031145 185.99891209 158 191 C162.45797661 193.97198441 166.69093071 193.40725072 171.9375 193.4375 C173.44086914 193.48680664 173.44086914 193.48680664 174.97460938 193.53710938 C175.94076172 193.54419922 176.90691406 193.55128906 177.90234375 193.55859375 C178.78398193 193.57204834 179.66562012 193.58550293 180.57397461 193.59936523 C183.14613804 193.14105252 183.14613804 193.14105252 185.29589844 191.18896484 C187.88503768 186.34378766 187.46643402 181.82438161 187.36328125 176.41796875 C187.3631468 174.10091137 187.36382318 171.78385383 187.36523438 169.46679688 C187.34437153 165.82610731 187.31248137 162.18608896 187.2644043 158.5456543 C187.22549273 155.02332181 187.2295461 151.50296026 187.23828125 147.98046875 C187.1980761 146.3597773 187.1980761 146.3597773 187.15705872 144.7063446 C187.20063277 137.92429497 188.38125888 134.37028292 193.19721985 129.52297974 C195.60169563 127.49169052 198.02826642 126.02412422 200.875 124.6875 C203.67974691 123.40458354 203.67974691 123.40458354 206 122 C206.495 120.515 206.495 120.515 207 119 C208.32407669 118.31513275 209.65909241 117.65129797 211 117 C218.03394019 110.59767893 222.67609678 102.14945543 225 93 C225.41572266 91.63488281 225.41572266 91.63488281 225.83984375 90.2421875 C228.01879679 82.06860327 228.37600903 74.05344785 228.5 65.625 C228.545802 63.94869507 228.545802 63.94869507 228.5925293 62.23852539 C228.59512592 55.29983632 227.78549327 50.24799993 223 45 C219.83376253 42.88917502 218.66687122 42.71826614 215 42.5 C211.33312878 42.28173386 210.16623747 42.11082498 207 40 C206.59936523 37.98803711 206.59936523 37.98803711 206.55859375 35.52734375 C206.51895508 34.17672852 206.51895508 34.17672852 206.47851562 32.79882812 C206.46498047 31.85458984 206.45144531 30.91035156 206.4375 29.9375 C206.47552836 16.3609489 206.47552836 16.3609489 200 5 C196.52674124 3.26337062 192.79754321 3.54587438 189 4 C184.68988566 6.24875531 183.24749878 9.02686833 181.62451172 13.53027344 C180.13526769 19.41971543 180.52255391 25.64597663 180.47265625 31.68359375 C180.43978516 33.24400391 180.43978516 33.24400391 180.40625 34.8359375 C180.39916016 35.78001465 180.39207031 36.7240918 180.38476562 37.69677734 C180.25779297 38.45684082 180.13082031 39.2169043 180 40 C176.64802157 42.23465229 175.42208754 42.27108649 171.5 42.25 C170.06140625 42.25773437 170.06140625 42.25773437 168.59375 42.265625 C166 42 166 42 163 40 C162.61523438 37.58911133 162.61523438 37.58911133 162.59375 34.58203125 C162.57183594 33.49212891 162.54992187 32.40222656 162.52734375 31.27929688 C162.51832031 30.13525391 162.50929687 28.99121094 162.5 27.8125 C162.58181445 15.78208188 162.58181445 15.78208188 156.875 5.5625 C150.31535939 1.99747793 144.44261152 2.8072688 140 9 Z M29 18 C27.75690982 18.55734153 26.50611905 19.09767874 25.25 19.625 C15.31959864 24.52348973 9.87097299 33.01446968 6 43 C5.4225 44.0725 4.845 45.145 4.25 46.25 C1.5355509 52.22178801 1.85125922 58.8383707 1.85473633 65.28076172 C1.85137772 66.1841629 1.8480191 67.08756409 1.84455872 68.01834106 C1.83510165 70.98880407 1.83310832 73.9592104 1.83203125 76.9296875 C1.82881529 79.00562564 1.82545096 81.08156355 1.82194519 83.15750122 C1.81596723 87.5032809 1.81410395 91.84904072 1.81469727 96.19482422 C1.81479887 101.74835837 1.80114227 107.30176462 1.78392506 112.85526848 C1.77277771 117.14137348 1.77077889 121.42744781 1.77130699 125.71356583 C1.77013117 127.76037156 1.76573186 129.8071782 1.75797462 131.85396957 C1.7481184 134.72288598 1.75107482 137.59152789 1.75708008 140.46044922 C1.75146057 141.29606384 1.74584106 142.13167847 1.74005127 142.99261475 C1.78650486 151.77708286 3.4309665 158.52339972 8 166 C8.474375 167.19625 8.94875 168.3925 9.4375 169.625 C11.16964491 173.366433 12.96215702 175.23832457 16 178 C16.45761719 178.53367188 16.91523438 179.06734375 17.38671875 179.6171875 C23.4434099 186.48245455 32.91829835 191.6594412 42 193 C45.07500561 193.1500941 48.14660087 193.15715039 51.22485352 193.16113281 C52.14574768 193.16609772 53.06664185 193.17106262 54.01544189 193.17617798 C57.053271 193.19075093 60.09104798 193.19759168 63.12890625 193.203125 C65.24210105 193.20887821 67.35529583 193.21463592 69.4684906 193.22039795 C73.89427941 193.2308925 78.32005012 193.23674603 82.74584961 193.24023438 C88.42105408 193.24573143 94.09602013 193.26979182 99.77114964 193.29820633 C104.13340461 193.31680852 108.4956017 193.32203691 112.85789299 193.32357025 C114.95070646 193.32660019 117.04351884 193.33464017 119.13629341 193.34775543 C122.05922924 193.36477345 124.98157602 193.36294663 127.90454102 193.35644531 C129.20366676 193.37026749 129.20366676 193.37026749 130.52903748 193.3843689 C134.07629421 193.35960112 136.38123007 193.33254518 139.54405212 191.63275146 C141.58798546 189.3406123 141.40682427 187.83136726 141.25 184.8125 C141.21390625 183.91144531 141.1778125 183.01039063 141.140625 182.08203125 C141.09421875 181.39496094 141.0478125 180.70789063 141 180 C136.23949026 177.18251858 132.29641684 176.56570247 126.87426758 176.59936523 C126.08151428 176.59238617 125.28876099 176.5854071 124.47198486 176.57821655 C121.86272425 176.55829588 119.25386104 176.55874013 116.64453125 176.55859375 C114.81856737 176.54681056 112.99261308 176.53344404 111.16667175 176.51856995 C106.36230203 176.48241677 101.55801724 176.46219961 96.75354004 176.44702148 C89.0826146 176.42218721 81.41187717 176.38214424 73.74112511 176.32512093 C71.07637896 176.30893896 68.41181349 176.30502696 65.74702454 176.30210876 C43.36816386 176.20592116 43.36816386 176.20592116 35.5625 171.125 C34.78003906 170.63257813 33.99757813 170.14015625 33.19140625 169.6328125 C28.54428073 166.17024839 25.96945907 162.59580448 23.3125 157.4375 C22.71050781 156.30183594 22.10851562 155.16617188 21.48828125 153.99609375 C17.33564893 144.28644146 18.83680819 132.07961583 18.83203125 121.6875 C18.82880969 120.02563449 18.82544498 118.36376925 18.82194519 116.7019043 C18.81599223 113.23583316 18.81410146 109.76978682 18.81469727 106.30371094 C18.81479899 101.88941444 18.80111863 97.47527838 18.78392506 93.0610199 C18.77274478 89.63439215 18.77077986 86.20780256 18.77130699 82.78115845 C18.77013703 81.15466761 18.76577144 79.52817545 18.75797462 77.90170288 C18.65692814 54.65976742 18.65692814 54.65976742 24.625 45.4375 C27.12807199 41.96986193 27.12807199 41.96986193 28 38 C29.2375 37.6596875 29.2375 37.6596875 30.5 37.3125 C34.06641694 35.97509365 36.77861071 34.29501706 39.875 32.12109375 C44.51187404 29.67480175 49.81671684 29.86710526 54.95166016 29.82641602 C55.61910751 29.819729 56.28655487 29.81304199 56.97422791 29.80615234 C58.41301333 29.79218215 59.85181988 29.78026312 61.29063797 29.77020454 C63.56903119 29.75368901 65.84726643 29.7304514 68.12557983 29.70530701 C74.59958954 29.63479462 81.07365987 29.57430413 87.54785156 29.52319336 C91.51653686 29.49139364 95.48503228 29.45012861 99.45356369 29.40299797 C100.96299203 29.3869994 102.47246391 29.37466792 103.98195267 29.36609077 C106.09054125 29.35392813 108.19868279 29.32977883 110.30712891 29.30297852 C111.50592682 29.29267105 112.70472473 29.28236359 113.93984985 29.27174377 C116.88119706 29.0105498 118.56970409 28.63488 121 27 C122.2220897 24.5558206 122.23641068 22.78970128 122.3125 20.0625 C122.34988281 19.18722656 122.38726562 18.31195312 122.42578125 17.41015625 C122.22047838 14.91436586 122.22047838 14.91436586 120.39048767 13.37263489 C117.22298611 11.55383314 114.97225534 11.62576595 111.32641602 11.62768555 C110.33273766 11.62220955 110.33273766 11.62220955 109.31898499 11.61662292 C107.13036363 11.60852442 104.94248517 11.62213398 102.75390625 11.63671875 C101.22134909 11.637238 99.68879125 11.63671779 98.15623474 11.63520813 C94.93909797 11.63529786 91.7222117 11.64571799 88.50512695 11.66381836 C84.41699552 11.68633332 80.32923543 11.68708099 76.2410593 11.68030071 C73.07080392 11.67692618 69.90063031 11.68365841 66.73039055 11.69342995 C65.22557951 11.69723559 63.72075928 11.69844979 62.21594429 11.6968441 C44.58732939 11.20764571 44.58732939 11.20764571 29 18 Z M221 15 C220.40540496 17.39908783 220.40540496 17.39908783 220.5625 20 C220.56443359 21.299375 220.56443359 21.299375 220.56640625 22.625 C221.07566035 25.41442784 221.79688886 26.27629446 224 28 C226.86584643 29.43292322 229.33846092 29.14856793 232.54174805 29.17358398 C233.54944496 29.1836145 233.54944496 29.1836145 234.57749939 29.19384766 C236.03872036 29.20790947 237.49996344 29.21979992 238.96121788 29.22979546 C241.27792176 29.24620854 243.59446357 29.26944959 245.91108704 29.29469299 C252.49882916 29.36543779 259.08663968 29.425413 265.67456055 29.47680664 C269.70348287 29.50871711 273.73222238 29.55002959 277.76099586 29.59700203 C279.29306539 29.61295821 280.82517705 29.62530905 282.35730553 29.63390923 C284.50881716 29.6461541 286.6599048 29.6703432 288.8112793 29.69702148 C290.64422798 29.71248268 290.64422798 29.71248268 292.51420593 29.72825623 C298.40715631 30.18765588 303.2454874 31.84508891 308.25 34.9375 C309.08015625 35.44152344 309.9103125 35.94554688 310.765625 36.46484375 C314.76196254 39.21057916 316.58418909 41.81033946 318.6875 46.1875 C319.12449219 47.08855469 319.56148438 47.98960938 320.01171875 48.91796875 C320.33785156 49.60503906 320.66398437 50.29210938 321 51 C321.66 51 322.32 51 323 51 C324.22691401 60.5901229 324.16394571 70.13055314 324.16796875 79.78125 C324.1711882 81.60388209 324.17455277 83.42651394 324.17805481 85.24914551 C324.18401617 89.05493773 324.18589768 92.86070735 324.18530273 96.66650391 C324.18520101 101.52147586 324.19887777 106.37630194 324.21607494 111.23123932 C324.22723779 114.99066829 324.22922067 118.75006238 324.22869301 122.50950623 C324.22986572 124.2987061 324.23424724 126.0879071 324.24202538 127.87709045 C324.33417252 151.25085011 324.33417252 151.25085011 319.125 159.375 C318.63257813 160.16648438 318.14015625 160.95796875 317.6328125 161.7734375 C314.44866241 166.11546034 310.9139611 168.91459338 306.3125 171.6875 C305.33925781 172.28949219 304.36601562 172.89148438 303.36328125 173.51171875 C296.82574948 176.40463351 289.50045938 176.14771224 282.43847656 176.15771484 C281.63564743 176.16279556 280.8328183 176.16787628 280.00566101 176.17311096 C278.28044943 176.18372647 276.55522484 176.1923804 274.8299942 176.19923019 C272.09742784 176.21112554 269.36501209 176.23059932 266.63250732 176.25234985 C258.86980172 176.31364738 251.10711918 176.36812179 243.34423828 176.40136719 C238.58102319 176.42230647 233.81809642 176.45758645 229.05504608 176.50212479 C227.24500668 176.51627146 225.43491643 176.52500421 223.62482452 176.52817917 C221.09755651 176.53308641 218.57105712 176.55623344 216.04394531 176.58349609 C215.2999498 176.58048996 214.55595428 176.57748383 213.78941345 176.5743866 C208.94477572 176.65202062 205.48866612 177.51133388 202 181 C201.68481785 183.03008491 201.68481785 183.03008491 201.625 185.375 C201.57601562 186.16648438 201.52703125 186.95796875 201.4765625 187.7734375 C201.80277752 190.16946581 201.80277752 190.16946581 203.95913696 191.6158905 C208.11241373 193.50633731 211.66540476 193.4124465 216.17724609 193.37231445 C217.55135902 193.37779045 217.55135902 193.37779045 218.95323181 193.38337708 C221.97682323 193.39147241 224.99987693 193.3778786 228.0234375 193.36328125 C230.12944539 193.36276243 232.23545377 193.36328097 234.34146118 193.36479187 C238.7547326 193.36470219 243.1678212 193.35428205 247.58105469 193.33618164 C253.22903322 193.31352266 258.87674403 193.31294584 264.52475548 193.31969929 C268.87428146 193.3230467 273.2237468 193.31639414 277.57326126 193.30657005 C279.65520536 193.30272901 281.73715621 193.30156528 283.81910324 193.3031559 C286.73418692 193.30352818 289.64893218 193.29085966 292.56396484 193.2746582 C293.41926285 193.27713562 294.27456085 193.27961304 295.15577698 193.28216553 C302.00687288 193.22162394 307.53046385 192.43690588 313 188 C315.559996 186.72797706 318.15071935 185.55093774 320.7578125 184.37890625 C323.264095 182.83758618 323.97482974 181.71266963 325 179 C325.66 179 326.32 179 327 179 C327 178.34 327 177.68 327 177 C327.91809448 176.62065308 327.91809448 176.62065308 328.85473633 176.23364258 C331.32694667 174.81198799 332.01842928 173.76290267 333.32421875 171.26171875 C333.71287109 170.52373047 334.10152344 169.78574219 334.50195312 169.02539062 C334.89318359 168.25388672 335.28441406 167.48238281 335.6875 166.6875 C336.09033203 165.92759766 336.49316406 165.16769531 336.90820312 164.38476562 C340.601829 157.26355394 341.15129788 151.33974491 341.16113281 143.39550781 C341.16609772 142.43387726 341.17106262 141.4722467 341.17617798 140.48147583 C341.19076288 137.30535156 341.1975957 134.12927724 341.203125 130.953125 C341.20887971 128.73980712 341.2146374 126.52648925 341.22039795 124.31317139 C341.23089766 119.67297128 341.23674699 115.03278844 341.24023438 110.39257812 C341.24571525 104.46003957 341.26973914 98.52773095 341.29820633 92.59526443 C341.31687243 88.02263735 341.32204044 83.45006521 341.32357025 78.87740326 C341.3265886 76.69103489 341.33458936 74.5046673 341.34775543 72.31833649 C341.36489795 69.25273879 341.3629155 66.1876971 341.35644531 63.12207031 C341.37026749 61.77977859 341.37026749 61.77977859 341.3843689 60.41036987 C341.32454027 51.31587827 338.28513201 43.9056313 334 36 C333.566875 35.175 333.13375 34.35 332.6875 33.5 C332.130625 32.675 331.57375 31.85 331 31 C330.59910156 30.40574219 330.19820312 29.81148437 329.78515625 29.19921875 C324.36213595 22.09271632 316.85792552 18.81696268 309 15 C308.34633957 14.6483075 307.69267914 14.29661499 307.01921082 13.93426514 C299.11566896 10.27738912 288.29854599 11.8181863 279.73046875 11.796875 C278.96040308 11.79404762 278.19033741 11.79122025 277.39693642 11.78830719 C273.33338716 11.77341074 269.26987264 11.76411717 265.20629883 11.75976562 C261.01538149 11.75424938 256.82478385 11.73012872 252.6339674 11.70179367 C249.40063019 11.68318492 246.167371 11.67796297 242.93398476 11.67642975 C241.38821957 11.67340612 239.84245566 11.66538732 238.29674339 11.65224457 C236.14052929 11.63519669 233.98511148 11.63705898 231.82885742 11.64355469 C230.60357834 11.63990906 229.37829926 11.63626343 228.1158905 11.63250732 C224.75408406 12.02900368 223.3574239 12.61030904 221 15 Z M42 46 C41.67 46.99 41.34 47.98 41 49 C40.34 49 39.68 49 39 49 C33.31774453 55.10842463 31.69812861 59.84691929 31.7253418 68.25927734 C31.72023087 69.07781693 31.71511993 69.89635651 31.70985413 70.73970032 C31.69598413 73.43852098 31.69691308 76.13708549 31.69921875 78.8359375 C31.69533012 80.71774388 31.69099511 82.59954939 31.68623352 84.48135376 C31.67877668 88.42379604 31.67905851 92.36614876 31.68432617 96.30859375 C31.69011713 101.35129526 31.67319623 106.39362903 31.6499815 111.43626976 C31.6352214 115.32256195 31.63437561 119.20876607 31.63749123 123.09508133 C31.63697906 124.95374911 31.63176106 126.81242196 31.62167931 128.67106247 C31.60928936 131.27492636 31.61612194 133.87808035 31.62768555 136.48193359 C31.62004181 137.24372421 31.61239807 138.00551483 31.60452271 138.79039001 C31.67713831 146.21933623 33.93387352 151.5081896 39.3125 156.8125 C40.05113281 157.56144531 40.78976563 158.31039062 41.55078125 159.08203125 C46.58257572 163.02239981 51.90920433 163.29553936 58.09741211 163.2746582 C58.88683701 163.27976913 59.6762619 163.28488007 60.48960876 163.29014587 C63.08559867 163.30398653 65.68132145 163.30308937 68.27734375 163.30078125 C70.09046018 163.30467106 71.90357571 163.30900632 73.71669006 163.31376648 C77.51192177 163.32121669 81.30706043 163.320946 85.10229492 163.31567383 C89.95552359 163.30987911 94.80837063 163.32681605 99.66153622 163.3500185 C103.40456859 163.36477995 107.14750955 163.36562429 110.89056587 163.36250877 C112.67945677 163.36302072 114.46835296 163.36823455 116.2572155 163.37832069 C118.76279084 163.39071548 121.26762901 163.38387568 123.77319336 163.37231445 C124.50536072 163.37995819 125.23752808 163.38760193 125.99188232 163.39547729 C130.85041704 163.34615225 136.37367177 162.62632823 140 159 C140.12619896 157.08647154 140.17580701 155.16764977 140.1875 153.25 C140.20167969 152.20328125 140.21585937 151.1565625 140.23046875 150.078125 C139.90923878 145.78779925 139.17753006 143.11275434 136.01171875 140.12109375 C135.24472656 139.56550781 134.47773438 139.00992188 133.6875 138.4375 C132.88570313 137.84710938 132.08390625 137.25671875 131.2578125 136.6484375 C130.51273437 136.10445313 129.76765625 135.56046875 129 135 C128.030625 134.195625 127.06125 133.39125 126.0625 132.5625 C124.16555765 130.7987141 124.16555765 130.7987141 122 131 C121.72542969 130.401875 121.45085937 129.80375 121.16796875 129.1875 C120.04945978 127.0926337 118.9374156 125.70950024 117.3125 124 C105.14752142 109.9111197 99.58691785 91.07740945 100.04296875 72.66796875 C100.06532504 71.1126425 100.08678384 69.55730313 100.10742188 68.00195312 C100.14469621 65.58109604 100.18814447 63.16091762 100.24682617 60.74047852 C100.30115762 58.38287322 100.33033912 56.02604164 100.35546875 53.66796875 C100.37925095 52.94296677 100.40303314 52.21796478 100.42753601 51.47099304 C100.48447267 49.18878237 100.48447267 49.18878237 100 46 C97.1734694 43.23830438 95.13015588 42.62307571 91.23486328 42.61181641 C90.29353546 42.60235992 89.35220764 42.59290344 88.38235474 42.5831604 C87.3718808 42.58923309 86.36140686 42.59530579 85.3203125 42.6015625 C84.27479218 42.59869232 83.22927185 42.59582214 82.15206909 42.59286499 C79.9413804 42.59082445 77.73067364 42.59635357 75.52001953 42.60888672 C72.14444309 42.62490772 68.7700699 42.60905373 65.39453125 42.58984375 C63.24478903 42.59182681 61.09504756 42.5956718 58.9453125 42.6015625 C57.93888702 42.59548981 56.93246155 42.58941711 55.89553833 42.5831604 C50.5720453 42.63628388 46.51918732 42.84092941 42 46 Z M355.35139465 73.17677307 C353.53344401 75.62945154 353.62565245 77.1959846 353.62768555 80.23413086 C353.62145676 81.30211411 353.61522797 82.37009735 353.60881042 83.47044373 C353.61802017 84.62359573 353.62722992 85.77674774 353.63671875 86.96484375 C353.63622025 88.14969864 353.63572174 89.33455353 353.63520813 90.55531311 C353.63755027 93.06285273 353.64728689 95.57039464 353.66381836 98.07788086 C353.68738447 101.9186708 353.68438327 105.75876564 353.67773438 109.59960938 C353.68331189 112.03385831 353.69042222 114.46810432 353.69921875 116.90234375 C353.69830734 118.05347153 353.69739594 119.2045993 353.69645691 120.39060974 C353.70598892 121.46174515 353.71552094 122.53288055 353.7253418 123.63647461 C353.72994919 124.57784775 353.73455658 125.51922089 353.73930359 126.48912048 C354.01517838 129.14618948 354.544947 130.769859 356 133 C358.3003806 134.1833965 358.3003806 134.1833965 361 134 C366.77596889 130.21758586 371.59721749 124.98889721 375 119 C375.52078125 118.35675781 376.0415625 117.71351562 376.578125 117.05078125 C378.79573727 112.29266262 378.39298289 107.61737907 378.3125 102.4375 C378.31572266 101.39658203 378.31894531 100.35566406 378.32226562 99.28320312 C378.42433417 91.74955158 378.42433417 91.74955158 374.5625 85.5625 C373 84 373 84 371.75 81.6875 C368.71047823 77.019663 365.07682206 73.67896106 359.75 71.9375 C357.";

}  // namespace icons
// Windows 11 Start logo
FrameworkElement BuildWindows11StartIcon(double displaySize) {
    std::wstring xaml =
        L"<Viewbox xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/presentation\" "
        L"Stretch=\"Uniform\" Width=\"" + std::to_wstring(displaySize) + L"\" Height=\"" + std::to_wstring(displaySize) + L"\">"
        L"<Grid Width=\"24\" Height=\"24\">"
        L"<Rectangle Width=\"10\" Height=\"10\" RadiusX=\"2\" RadiusY=\"2\" Fill=\"#FFFFFF\" HorizontalAlignment=\"Left\" VerticalAlignment=\"Top\" Margin=\"1,1,0,0\"/>"
        L"<Rectangle Width=\"10\" Height=\"10\" RadiusX=\"2\" RadiusY=\"2\" Fill=\"#FFFFFF\" HorizontalAlignment=\"Right\" VerticalAlignment=\"Top\" Margin=\"0,1,1,0\"/>"
        L"<Rectangle Width=\"10\" Height=\"10\" RadiusX=\"2\" RadiusY=\"2\" Fill=\"#FFFFFF\" HorizontalAlignment=\"Left\" VerticalAlignment=\"Bottom\" Margin=\"1,0,0,1\"/>"
        L"<Rectangle Width=\"10\" Height=\"10\" RadiusX=\"2\" RadiusY=\"2\" Fill=\"#FFFFFF\" HorizontalAlignment=\"Right\" VerticalAlignment=\"Bottom\" Margin=\"0,0,1,1\"/>"
        L"</Grid></Viewbox>";

    try {
        auto element = Markup::XamlReader::Load(xaml).as<FrameworkElement>();
        element.Name(L"StartIcon");
        return element;
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"Failed to build Windows 11 start icon: %08X",
               static_cast<unsigned int>(ex.code().value));
        return nullptr;
    }
}

FrameworkElement BuildVectorIcon(PCWSTR name,
                                 std::wstring_view fillData,
                                 std::wstring_view strokeData,
                                 double viewport,
                                 double displaySize,
                                 double thickness = 1.7,
                                 PCWSTR brushOverride = nullptr) {
    std::wstring brush = brushOverride ? brushOverride : g_settings.iconColor;
    if (brush.empty()) {
        brush = L"#FFFFFF";
    }

    std::wstring xaml =
        L"<Viewbox xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/presentation\" "
        L"Stretch=\"Uniform\" Width=\"" +
        std::to_wstring(displaySize) + L"\" Height=\"" + std::to_wstring(displaySize) + L"\">"
        L"<Grid Width=\"" + std::to_wstring(viewport) + L"\" Height=\"" +
        std::to_wstring(viewport) + L"\">";

    if (!fillData.empty()) {
        xaml += L"<Path Data=\"" + EscapeXmlAttr(fillData) + L"\" Fill=\"" + brush + L"\"/>";
    }
    if (!strokeData.empty()) {
        xaml += L"<Path Data=\"" + EscapeXmlAttr(strokeData) + L"\" Stroke=\"" + brush +
                L"\" StrokeThickness=\"" + std::to_wstring(thickness) +
                L"\" StrokeStartLineCap=\"Round\" StrokeEndLineCap=\"Round\" "
                L"StrokeLineJoin=\"Round\"/>";
    }

    xaml += L"</Grid></Viewbox>";

    try {
        auto element = Markup::XamlReader::Load(xaml).as<FrameworkElement>();
        if (name && *name) {
            element.Name(name);
        }
        return element;
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"Failed to build vector icon %s: %08X", name ? name : L"(unnamed)",
               static_cast<unsigned int>(ex.code().value));
        return nullptr;
    }
}

FrameworkElement BuildSearchIcon(double displaySize) {
    std::wstring brush = g_settings.iconColor.empty() ? L"#FFFFFF" : g_settings.iconColor;

    std::wstring pathsXaml =
        L"<Grid xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/presentation\">"
        L"<Path Data=\"" + EscapeXmlAttr(icons::kSearchOutline) + L"\" />"
        L"<Path Data=\"" + EscapeXmlAttr(icons::kSearchLens) + L"\" />"
        L"</Grid>";

    std::wstring viewboxXaml =
        L"<Viewbox xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/presentation\" "
        L"Stretch=\"Uniform\" Width=\"" +
        std::to_wstring(displaySize) + L"\" Height=\"" + std::to_wstring(displaySize) + L"\">"
        L"<Grid Width=\"360\" Height=\"360\">"
        L"<Path Fill=\"" + brush + L"\" />"
        L"</Grid>"
        L"</Viewbox>";

    try {
        auto tempGrid = Markup::XamlReader::Load(pathsXaml).as<wuxc::Grid>();
        auto p1 = tempGrid.Children().GetAt(0).as<winrt::Windows::UI::Xaml::Shapes::Path>();
        auto p2 = tempGrid.Children().GetAt(1).as<winrt::Windows::UI::Xaml::Shapes::Path>();

        // Cleared from the throwaway paths so the geometries can be re-parented.
        auto p1Geom = p1.Data();
        p1.Data(nullptr);
        auto p2Geom = p2.Data();
        p2.Data(nullptr);

        wuxm::GeometryGroup group;
        group.FillRule(wuxm::FillRule::EvenOdd);
        group.Children().Append(p1Geom);

        wuxm::TranslateTransform transform;
        transform.X(189.488037109375);
        transform.Y(66.264892578125);
        p2Geom.Transform(transform);
        group.Children().Append(p2Geom);

        auto viewbox = Markup::XamlReader::Load(viewboxXaml).as<wuxc::Viewbox>();
        auto finalGrid = viewbox.Child().as<wuxc::Grid>();
        auto searchPath =
            finalGrid.Children().GetAt(0).as<winrt::Windows::UI::Xaml::Shapes::Path>();
        searchPath.Data(group);
        viewbox.Name(L"SearchIcon");
        return viewbox;
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"Failed to build search icon: %08X",
               static_cast<unsigned int>(ex.code().value));
        return nullptr;
    }
}

// ============================================================================
// Battery icon builder
// ============================================================================


FrameworkElement BuildBatteryIcon(double displaySize, bool charging) {
    // Try to load the complex vector path first.
    std::wstring brush = g_settings.iconColor.empty() ? L"#FFFFFF" : g_settings.iconColor;
    std::wstring pathData = charging ? icons::kBatteryChargingPath
                                     : icons::kBatteryNotChargingPath;

    std::wstring xaml =
        L"<Viewbox xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/presentation\" "
        L"Stretch=\"Uniform\" Width=\"" + std::to_wstring(displaySize) + L"\" Height=\"" + std::to_wstring(displaySize) + L"\">"
        L"<Grid Width=\"380\" Height=\"197\">"
        L"<Path Data=\"" + EscapeXmlAttr(pathData) + L"\" Fill=\"" + brush + L"\"/>"
        L"</Grid></Viewbox>";

    try {
        auto element = Markup::XamlReader::Load(xaml).as<FrameworkElement>();
        element.Name(L"BatteryIcon");
        return element;
    } catch (...) {}

    // Fallback: simple battery rectangle + bolt using XAML.
    std::wstring fallbackXaml;
    if (charging) {
        fallbackXaml =
            L"<Viewbox xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/presentation\" "
            L"Stretch=\"Uniform\" Width=\"" + std::to_wstring(displaySize) + L"\" Height=\"" + std::to_wstring(displaySize) + L"\">"
            L"<Grid Width=\"20\" Height=\"20\">"
            L"<Rectangle Width=\"10\" Height=\"6\" RadiusX=\"2\" RadiusY=\"2\" Stroke=\"#FFFFFF\" StrokeThickness=\"1.5\" HorizontalAlignment=\"Center\" VerticalAlignment=\"Center\"/>"
            L"<Path Data=\"M 6 2 L 3 7 L 6 7 L 4 12 L 9 6 L 6 6 Z\" Fill=\"#FFFFFF\" HorizontalAlignment=\"Center\" VerticalAlignment=\"Center\" Width=\"10\" Height=\"12\"/>"
            L"</Grid></Viewbox>";
    } else {
        fallbackXaml =
            L"<Viewbox xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/presentation\" "
            L"Stretch=\"Uniform\" Width=\"" + std::to_wstring(displaySize) + L"\" Height=\"" + std::to_wstring(displaySize) + L"\">"
            L"<Grid Width=\"20\" Height=\"20\">"
            L"<Rectangle Width=\"10\" Height=\"6\" RadiusX=\"2\" RadiusY=\"2\" Stroke=\"#FFFFFF\" StrokeThickness=\"1.5\" HorizontalAlignment=\"Center\" VerticalAlignment=\"Center\"/>"
            L"</Grid></Viewbox>";
    }

    try {
        auto element = Markup::XamlReader::Load(fallbackXaml).as<FrameworkElement>();
        element.Name(L"BatteryIcon");
        return element;
    } catch (...) {
        return nullptr;
    }
}

// ============================================================================
// Shared WinUI-flavoured building blocks
// ============================================================================
wuxm::SolidColorBrush MakeBrush(uint8_t a, uint8_t r, uint8_t g, uint8_t b) {
    return wuxm::SolidColorBrush(wui::ColorHelper::FromArgb(a, r, g, b));
}

CornerRadius MakeCorner(double radius) {
    return CornerRadius{radius, radius, radius, radius};
}

// Every clickable surface in the bar and in the flyouts goes through this, so
// corner radius, padding and the transparent-until-hover treatment stay
// identical everywhere.
wuxc::Button MakeGhostButton(PCWSTR name, double cornerRadius) {
    wuxc::Button button;
    if (name && *name) {
        button.Name(name);
    }
    button.Background(MakeBrush(0, 255, 255, 255));
    button.BorderThickness(Thickness{0, 0, 0, 0});
    button.CornerRadius(MakeCorner(cornerRadius));
    button.Padding(Thickness{8, 4, 8, 4});
    button.HorizontalContentAlignment(HorizontalAlignment::Stretch);
    button.VerticalContentAlignment(VerticalAlignment::Center);
    return button;
}

wuxc::TextBlock MakeText(PCWSTR name, std::wstring_view text, double size, bool bold = false,
                         double opacity = 1.0) {
    wuxc::TextBlock block;
    if (name && *name) {
        block.Name(name);
    }
    block.Text(winrt::hstring(text));
    block.FontSize(size);
    if (bold) {
        block.FontWeight(winrt::Windows::UI::Text::FontWeights::SemiBold());
    }
    block.Opacity(opacity);
    block.VerticalAlignment(VerticalAlignment::Center);
    block.TextTrimming(TextTrimming::CharacterEllipsis);
    return block;
}

wuxc::Border MakeDivider() {
    wuxc::Border border;
    border.Name(L"FlyoutDivider");
    border.Height(1);
    border.Background(MakeBrush(28, 255, 255, 255));
    border.Margin(Thickness{0, 6, 0, 6});
    return border;
}

// ============================================================================
// task icons
//
// ============================================================================

bool ProcessImagePathForWindow(HWND hwnd, std::wstring* imagePath) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (!pid) {
        return false;
    }
    HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!process) {
        return false;
    }
    wchar_t path[MAX_PATH]{};
    DWORD size = ARRAYSIZE(path);
    bool ok = QueryFullProcessImageNameW(process, 0, path, &size) != FALSE;
    CloseHandle(process);
    if (ok) {
        *imagePath = path;
    }
    return ok;
}

using PrivateExtractIconsW_t = UINT(WINAPI*)(LPCWSTR, int, int, int, HICON*, UINT*, UINT, UINT);

PrivateExtractIconsW_t GetPrivateExtractIcons() {
    static PrivateExtractIconsW_t proc = reinterpret_cast<PrivateExtractIconsW_t>(
        GetProcAddress(GetModuleHandle(L"user32.dll"), "PrivateExtractIconsW"));
    return proc;
}

// Returns an icon the caller owns and must DestroyIcon.
HICON ExtractCrispWindowIcon(HWND hwnd, UINT sizePx) {
    if (sizePx == 0) {
        sizePx = 16;
    }

    std::wstring exePath;
    if (ProcessImagePathForWindow(hwnd, &exePath) && !exePath.empty()) {
        if (auto extract = GetPrivateExtractIcons()) {
            HICON icon = nullptr;
            UINT iconId = 0;
            if (extract(exePath.c_str(), 0, static_cast<int>(sizePx), static_cast<int>(sizePx),
                        &icon, &iconId, 1, 0) == 1 &&
                icon) {
                return icon;
            }
        }
    }

    DWORD_PTR result = 0;
    auto fromMessage = [&](WPARAM which) -> HICON {
        if (SendMessageTimeout(hwnd, WM_GETICON, which, 0, SMTO_ABORTIFHUNG, 100, &result) &&
            result) {
            return CopyIcon(reinterpret_cast<HICON>(result));
        }
        return nullptr;
    };

    if (HICON icon = fromMessage(ICON_BIG)) {
        return icon;
    }
    if (auto classIcon = reinterpret_cast<HICON>(GetClassLongPtr(hwnd, GCLP_HICON))) {
        return CopyIcon(classIcon);
    }
    if (HICON icon = fromMessage(ICON_SMALL2)) {
        return icon;
    }
    if (HICON icon = fromMessage(ICON_SMALL)) {
        return icon;
    }
    if (auto classIcon = reinterpret_cast<HICON>(GetClassLongPtr(hwnd, GCLP_HICONSM))) {
        return CopyIcon(classIcon);
    }
    return nullptr;
}

wuxm::Imaging::BitmapImage HIconToBitmapImage(HICON hIcon, UINT size) {
    if (!hIcon || !size) {
        return nullptr;
    }

    HDC screenDc = GetDC(nullptr);
    HDC dc = CreateCompatibleDC(screenDc);
    ReleaseDC(nullptr, screenDc);
    if (!dc) {
        return nullptr;
    }

    BITMAPINFO bmi{};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = static_cast<LONG>(size);
    bmi.bmiHeader.biHeight = -static_cast<LONG>(size);  // top-down
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void* bits = nullptr;
    HBITMAP dib = CreateDIBSection(dc, &bmi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!dib) {
        DeleteDC(dc);
        return nullptr;
    }

    DWORD dataSize = static_cast<DWORD>(size) * static_cast<DWORD>(size) * 4;

    HGDIOBJ old = SelectObject(dc, dib);
    ZeroMemory(bits, dataSize);  // fully transparent baseline, no dark fringe
    DrawIconEx(dc, 0, 0, hIcon, static_cast<int>(size), static_cast<int>(size), 0, nullptr,
               DI_NORMAL);

    std::vector<uint8_t> pixels(dataSize);
    memcpy(pixels.data(), bits, dataSize);

    SelectObject(dc, old);
    DeleteObject(dib);
    DeleteDC(dc);

    bool hasAlpha = false;
    for (DWORD i = 3; i < dataSize; i += 4) {
        if (pixels[i] != 0) {
            hasAlpha = true;
            break;
        }
    }
    if (!hasAlpha) {
        for (DWORD i = 0; i + 3 < dataSize; i += 4) {
            bool black = pixels[i] < 4 && pixels[i + 1] < 4 && pixels[i + 2] < 4;
            pixels[i + 3] = black ? 0 : 255;
        }
    } else {
        for (DWORD i = 0; i + 3 < dataSize; i += 4) {
            uint8_t a = pixels[i + 3];
            if (a > 0 && a < 255) {
                pixels[i + 0] = static_cast<uint8_t>(pixels[i + 0] * a / 255);
                pixels[i + 1] = static_cast<uint8_t>(pixels[i + 1] * a / 255);
                pixels[i + 2] = static_cast<uint8_t>(pixels[i + 2] * a / 255);
            }
        }
    }

    if (g_iconTintOpacity > 0.0) {
        double t = std::clamp(g_iconTintOpacity, 0.0, 1.0);
        double tb = g_iconTintColor.B;
        double tg = g_iconTintColor.G;
        double tr = g_iconTintColor.R;
        for (DWORD i = 0; i + 3 < dataSize; i += 4) {
            // BGRA byte order.
            pixels[i + 0] = static_cast<uint8_t>(pixels[i + 0] * (1.0 - t) + tb * t);
            pixels[i + 1] = static_cast<uint8_t>(pixels[i + 1] * (1.0 - t) + tg * t);
            pixels[i + 2] = static_cast<uint8_t>(pixels[i + 2] * (1.0 - t) + tr * t);
        }
    }

    BITMAPFILEHEADER fileHeader{};
    fileHeader.bfType = 0x4D42;  // "BM"
    fileHeader.bfSize = static_cast<DWORD>(sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) +
                                           dataSize);
    fileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    BITMAPINFOHEADER infoHeader = bmi.bmiHeader;
    infoHeader.biSizeImage = dataSize;

    try {
        winrt::Windows::Storage::Streams::InMemoryRandomAccessStream stream;
        winrt::Windows::Storage::Streams::DataWriter writer(stream);
        writer.WriteBytes(winrt::array_view<const uint8_t>(
            reinterpret_cast<const uint8_t*>(&fileHeader), sizeof(fileHeader)));
        writer.WriteBytes(winrt::array_view<const uint8_t>(
            reinterpret_cast<const uint8_t*>(&infoHeader), sizeof(infoHeader)));
        writer.WriteBytes(pixels);
        writer.StoreAsync().get();
        writer.DetachStream();
        stream.Seek(0);

        wuxm::Imaging::BitmapImage bitmapImage;
        // Fire-and-forget: blocking on .get() here would risk this UI thread
        // waiting on its own dispatcher to pump the completion.
        bitmapImage.SetSourceAsync(stream);
        return bitmapImage;
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"Icon conversion failed: %08X", static_cast<unsigned int>(ex.code().value));
        return nullptr;
    } catch (...) {
        return nullptr;
    }
}

wuxm::Imaging::BitmapImage GetWindowIconBitmap(HWND hwnd, UINT physicalSize) {
    HICON icon = ExtractCrispWindowIcon(hwnd, physicalSize);
    if (!icon) {
        return nullptr;
    }
    auto bitmap = HIconToBitmapImage(icon, physicalSize);
    DestroyIcon(icon);
    return bitmap;
}

// ============================================================================
// task list
// ============================================================================

bool IsTaskbarEligibleWindow(HWND hwnd) {
    if (!IsWindow(hwnd) || !IsWindowVisible(hwnd)) {
        return false;
    }
    if (hwnd == g_topBarHwnd || hwnd == g_islandHwnd) {
        return false;
    }
    if (GetWindow(hwnd, GW_OWNER) != nullptr) {
        return false;
    }
    LONG_PTR exStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
    if (exStyle & WS_EX_TOOLWINDOW) {
        return false;
    }
    if (GetWindowTextLength(hwnd) == 0) {
        return false;
    }

    BOOL cloaked = FALSE;
    if (SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &cloaked, sizeof(cloaked))) &&
        cloaked) {
        return false;
    }

    return true;
}


void ForceForegroundWindow(HWND hwnd) {
    if (!IsWindow(hwnd)) {
        return;
    }
    HWND foreground = GetForegroundWindow();
    if (foreground == hwnd) {
        return;
    }

    DWORD foregroundThread = foreground ? GetWindowThreadProcessId(foreground, nullptr) : 0;
    DWORD targetThread = GetWindowThreadProcessId(hwnd, nullptr);
    DWORD currentThread = GetCurrentThreadId();

    bool attachedForeground =
        foregroundThread && foregroundThread != currentThread &&
        AttachThreadInput(currentThread, foregroundThread, TRUE) != FALSE;
    bool attachedTarget = targetThread && targetThread != currentThread &&
                          targetThread != foregroundThread &&
                          AttachThreadInput(currentThread, targetThread, TRUE) != FALSE;

    SetForegroundWindow(hwnd);
    BringWindowToTop(hwnd);
    SetActiveWindow(hwnd);

    if (attachedTarget) {
        AttachThreadInput(currentThread, targetThread, FALSE);
    }
    if (attachedForeground) {
        AttachThreadInput(currentThread, foregroundThread, FALSE);
    }
}


void CALLBACK ForegroundEventProc(HWINEVENTHOOK, DWORD event, HWND hwnd, LONG idObject,
                                  LONG idChild, DWORD, DWORD) {
    if (event != EVENT_SYSTEM_FOREGROUND || idObject != OBJID_WINDOW ||
        idChild != CHILDID_SELF || !hwnd) {
        return;
    }
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid == GetCurrentProcessId()) {
        return;
    }
    g_lastForegroundHwnd = hwnd;
}

void ActivateOrMinimizeWindow(HWND hwnd) {
    if (!IsWindow(hwnd)) {
        return;
    }
    if (hwnd == g_lastForegroundHwnd && !IsIconic(hwnd)) {
        ShowWindow(hwnd, SW_MINIMIZE);
        g_lastForegroundHwnd = nullptr;
        return;
    }
    if (IsIconic(hwnd)) {
        ShowWindow(hwnd, SW_RESTORE);
    }
    ForceForegroundWindow(hwnd);
    g_lastForegroundHwnd = hwnd;
}

void ToggleMaximizeWindow(HWND hwnd) {
    if (!IsWindow(hwnd)) {
        return;
    }
    if (IsIconic(hwnd)) {
        ShowWindow(hwnd, SW_RESTORE);
    }
    ShowWindow(hwnd, IsZoomed(hwnd) ? SW_RESTORE : SW_MAXIMIZE);
    ForceForegroundWindow(hwnd);
}

void CloseWindowGracefully(HWND hwnd) {
    if (IsWindow(hwnd)) {
        PostMessage(hwnd, WM_CLOSE, 0, 0);
    }
}

FrameworkElement BuildTaskButtonContent(HWND hwnd, const std::wstring& title) {
    wuxc::Grid content;
    content.Name(L"TaskButtonContent");
    content.VerticalAlignment(VerticalAlignment::Center);

    bool wantIcon = g_settings.taskButtonContent != L"textOnly";
    bool wantText = g_settings.taskButtonContent != L"iconOnly";

    wuxc::ColumnDefinition iconColumn;
    iconColumn.Width(GridLength{0, GridUnitType::Auto});
    wuxc::ColumnDefinition textColumn;
    textColumn.Width(GridLength{1, GridUnitType::Star});
    content.ColumnDefinitions().Append(iconColumn);
    content.ColumnDefinitions().Append(textColumn);

    double iconDip = std::max(12, g_settings.taskIconSize);

    if (wantIcon) {
        // Extracted at the exact physical pixel size the image will occupy, so
        // there is no upscale step at all.
        UINT physicalSize =
            static_cast<UINT>(std::lround(iconDip * std::max(1.0, g_dpiScale)));
        if (auto bitmapImage = GetWindowIconBitmap(hwnd, physicalSize)) {
            wuxc::Image img;
            img.Name(L"TaskButtonIcon");
            img.Source(bitmapImage);
            img.Width(iconDip);
            img.Height(iconDip);
            img.Stretch(wuxm::Stretch::Uniform);
            img.VerticalAlignment(VerticalAlignment::Center);
            if (wantText) {
                img.Margin(Thickness{0, 0, 8, 0});
            }
            wuxc::Grid::SetColumn(img, 0);
            content.Children().Append(img);
        }
    }

    if (wantText) {
        auto text = MakeText(L"TaskButtonText", title, 12.5);
        wuxc::Grid::SetColumn(text, 1);
        double textMaxWidth =
            g_settings.taskButtonWidth - 16.0 - (wantIcon ? (iconDip + 8.0) : 0.0);
        text.MaxWidth(std::max(20.0, textMaxWidth));
        content.Children().Append(text);
    } else {
        textColumn.Width(GridLength{0, GridUnitType::Pixel});
    }

    return content;
}

// A single pending click is enough: Tapped arms a short timer that performs the
// single-click action; DoubleTapped cancels it and performs the double-click
// action instead, so a double-click never also fires the single-click behaviour.
[[clang::no_destroy]] DispatcherTimer g_taskClickTimer{nullptr};
HWND g_pendingClickHwnd;

void SchedulePendingSingleClick(HWND hwnd) {
    g_pendingClickHwnd = hwnd;
    if (!g_taskClickTimer) {
        g_taskClickTimer = DispatcherTimer();
        g_taskClickTimer.Interval(std::chrono::milliseconds(GetDoubleClickTime()));
        g_taskClickTimer.Tick([](wf::IInspectable const&, wf::IInspectable const&) {
            g_taskClickTimer.Stop();
            try {
                if (g_pendingClickHwnd) {
                    ActivateOrMinimizeWindow(g_pendingClickHwnd);
                }
            } catch (...) {
            }
            g_pendingClickHwnd = nullptr;
        });
    }
    g_taskClickTimer.Stop();
    g_taskClickTimer.Start();
}

void CancelPendingSingleClick() {
    if (g_taskClickTimer) {
        g_taskClickTimer.Stop();
    }
    g_pendingClickHwnd = nullptr;
}

wuxc::Button CreateTaskButton(HWND hwnd, const std::wstring& title) {
    auto button = MakeGhostButton(L"TaskButton", g_settings.cornerRadius);
    button.Content(BuildTaskButtonContent(hwnd, title));
    button.MaxWidth(g_settings.taskButtonWidth);
    // Stretch, not Center: the Start and Search buttons fill the bar height and
    // let their Margin do the insetting, so a centred task button ended up
    // shorter than them with dead space above and below. Stretching makes the
    // same "Margin=3,2,3,2" mean the same thing here as it does there, at any
    // bar height, without hard-coding a Height.
    button.VerticalAlignment(VerticalAlignment::Stretch);
    button.Margin(Thickness{3, 2, 3, 2});
    button.Padding(Thickness{8, 0, 8, 0});
    button.HorizontalContentAlignment(HorizontalAlignment::Center);
    button.Tag(winrt::box_value(reinterpret_cast<int64_t>(hwnd)));

    button.Tapped([](wf::IInspectable const& sender, Input::TappedRoutedEventArgs const&) {
        try {
            auto btn = sender.as<wuxc::Button>();
            auto tagValue = winrt::unbox_value<int64_t>(btn.Tag());
            SchedulePendingSingleClick(reinterpret_cast<HWND>(tagValue));
        } catch (...) {
        }
    });

    button.DoubleTapped(
        [](wf::IInspectable const& sender, Input::DoubleTappedRoutedEventArgs const&) {
            try {
                CancelPendingSingleClick();
                auto btn = sender.as<wuxc::Button>();
                auto tagValue = winrt::unbox_value<int64_t>(btn.Tag());
                ToggleMaximizeWindow(reinterpret_cast<HWND>(tagValue));
            } catch (...) {
            }
        });

    button.RightTapped(
        [](wf::IInspectable const& sender, Input::RightTappedRoutedEventArgs const&) {
            try {
                if (!g_taskContextMenu) {
                    return;
                }
                auto btn = sender.as<wuxc::Button>();
                auto tagValue = winrt::unbox_value<int64_t>(btn.Tag());
                g_contextMenuTargetHwnd = reinterpret_cast<HWND>(tagValue);
                if (g_taskMenuToggleItem) {
                    g_taskMenuToggleItem.Text(
                        IsZoomed(g_contextMenuTargetHwnd) ? L"Restore" : L"Maximize");
                }
                g_taskContextMenu.ShowAt(btn);
            } catch (...) {
            }
        });

    return button;
}

void UpdateTaskButtonState(wuxc::Button button, HWND hwnd, const std::wstring& title) {
    button.Content(BuildTaskButtonContent(hwnd, title));
}

// Diffing refresh: keeps the same Button object at the same panel position for
// every window that's still open, only appending new windows at the end and
// removing closed ones. EnumWindows returns Z-order, which changes every time
// the user clicks between windows -- rebuilding from scratch each tick made the
// whole list visibly reorder itself on every refresh.
void RefreshTaskList(bool forceIconRegeneration) {
    if (!g_taskListPanel) {
        return;
    }

    std::vector<HWND> currentWindows;
    EnumWindows(
        [](HWND hwnd, LPARAM lParam) -> BOOL {
            auto* list = reinterpret_cast<std::vector<HWND>*>(lParam);
            if (IsTaskbarEligibleWindow(hwnd)) {
                list->push_back(hwnd);
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&currentWindows));

    std::set<HWND> currentSet(currentWindows.begin(), currentWindows.end());

    // Re-running the style engine means a XamlReader parse per style per
    // element, so on a timer tick where nothing actually changed it is skipped
    // entirely.
    bool treeChanged = forceIconRegeneration;

    for (auto it = g_stableWindowOrder.begin(); it != g_stableWindowOrder.end();) {
        if (!currentSet.count(*it)) {
            auto btnIt = g_taskButtonsByHwnd.find(*it);
            if (btnIt != g_taskButtonsByHwnd.end()) {
                uint32_t index;
                if (g_taskListPanel.Children().IndexOf(btnIt->second, index)) {
                    g_taskListPanel.Children().RemoveAt(index);
                }
                g_taskButtonsByHwnd.erase(btnIt);
            }
            g_taskButtonLastTitle.erase(*it);
            it = g_stableWindowOrder.erase(it);
            treeChanged = true;
        } else {
            ++it;
        }
    }

    for (HWND hwnd : currentWindows) {
        wchar_t titleBuf[256]{};
        GetWindowText(hwnd, titleBuf, ARRAYSIZE(titleBuf));
        std::wstring title = titleBuf;

        auto btnIt = g_taskButtonsByHwnd.find(hwnd);
        if (btnIt == g_taskButtonsByHwnd.end()) {
            auto button = CreateTaskButton(hwnd, title);
            g_taskListPanel.Children().Append(button);
            g_taskButtonsByHwnd.insert_or_assign(hwnd, button);
            g_taskButtonLastTitle.insert_or_assign(hwnd, title);
            g_stableWindowOrder.push_back(hwnd);
            treeChanged = true;
            continue;
        }

        auto lastTitleIt = g_taskButtonLastTitle.find(hwnd);
        bool titleChanged =
            lastTitleIt == g_taskButtonLastTitle.end() || lastTitleIt->second != title;
        if (forceIconRegeneration || titleChanged) {
            UpdateTaskButtonState(btnIt->second, hwnd, title);
            g_taskButtonLastTitle.insert_or_assign(hwnd, title);
            // Don't set treeChanged on title change – only on structural changes.
            // This avoids unnecessary style re-application.
        }
    }

    if (treeChanged) {
        ApplyAllControlStyles();
    }
}

// ============================================================================
// Audio subsystem
// ============================================================================

// Declared by hand rather than pulled from functiondiscoverykeys_devpkey.h,
// which isn't reliably present in this toolchain's headers.
static const PROPERTYKEY kPkeyDeviceFriendlyName = {
    {0xa45c254e, 0xdf1c, 0x4efd, {0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0}}, 14};

// IPolicyConfig is how every third-party audio switcher changes the default
// endpoint -- there is no public API for it. Declared by hand because it ships
// in no SDK header. Only SetDefaultEndpoint is actually called; the earlier
// vtable slots must still be declared so the layout matches, and their unused
// parameters are typed as void* to avoid dragging in mmreg.h.
static const CLSID kCLSID_PolicyConfigClient = {
    0x870af99c, 0x171d, 0x4f9e, {0xaf, 0x0d, 0xe6, 0x3d, 0xf4, 0x0c, 0x2b, 0xc9}};
static const IID kIID_IPolicyConfig = {
    0xf8679f50, 0x850a, 0x41cf, {0x9c, 0x72, 0x43, 0x0f, 0x29, 0x02, 0x90, 0xc8}};

struct IPolicyConfig : public IUnknown {
    virtual HRESULT STDMETHODCALLTYPE GetMixFormat(PCWSTR, void**) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetDeviceFormat(PCWSTR, INT, void**) = 0;
    virtual HRESULT STDMETHODCALLTYPE ResetDeviceFormat(PCWSTR) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetDeviceFormat(PCWSTR, void*, void*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetProcessingPeriod(PCWSTR, INT, INT64*, INT64*) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetProcessingPeriod(PCWSTR, INT64*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetShareMode(PCWSTR, void*) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetShareMode(PCWSTR, void*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetPropertyValue(PCWSTR, const PROPERTYKEY&,
                                                       PROPVARIANT*) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetPropertyValue(PCWSTR, const PROPERTYKEY&,
                                                       PROPVARIANT*) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetDefaultEndpoint(PCWSTR deviceId, ERole role) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetEndpointVisibility(PCWSTR, INT) = 0;
};

namespace audio {

struct SessionInfo {
    std::wstring name;
    int volume = 100;
    bool muted = false;
    bool isSystemSounds = false;
    winrt::com_ptr<ISimpleAudioVolume> control;
};

struct OutputDevice {
    std::wstring id;
    std::wstring name;
    bool isDefault = false;
};

winrt::com_ptr<IMMDeviceEnumerator> DeviceEnumerator() {
    winrt::com_ptr<IMMDeviceEnumerator> enumerator;
    if (FAILED(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
                                __uuidof(IMMDeviceEnumerator), enumerator.put_void()))) {
        return nullptr;
    }
    return enumerator;
}

winrt::com_ptr<IMMDevice> DefaultRenderDevice() {
    auto enumerator = DeviceEnumerator();
    if (!enumerator) {
        return nullptr;
    }
    winrt::com_ptr<IMMDevice> device;
    if (FAILED(enumerator->GetDefaultAudioEndpoint(eRender, eConsole, device.put()))) {
        return nullptr;
    }
    return device;
}

std::wstring DeviceFriendlyName(IMMDevice* device) {
    if (!device) {
        return L"";
    }
    winrt::com_ptr<IPropertyStore> store;
    if (FAILED(device->OpenPropertyStore(STGM_READ, store.put()))) {
        return L"";
    }
    PROPVARIANT value;
    PropVariantInit(&value);
    std::wstring name;
    if (SUCCEEDED(store->GetValue(kPkeyDeviceFriendlyName, &value)) && value.vt == VT_LPWSTR &&
        value.pwszVal) {
        name = value.pwszVal;
    }
    PropVariantClear(&value);
    return name;
}

std::wstring DeviceId(IMMDevice* device) {
    if (!device) {
        return L"";
    }
    LPWSTR id = nullptr;
    if (FAILED(device->GetId(&id)) || !id) {
        return L"";
    }
    std::wstring result = id;
    CoTaskMemFree(id);
    return result;
}

// Cached: the wheel handler hits this on every notch, and re-running
// CoCreateInstance + Activate each time is enough latency to make scrolling
// feel sticky. Dropped whenever a call fails, which covers the default endpoint
// changing underneath us.
[[clang::no_destroy]] winrt::com_ptr<IAudioEndpointVolume> g_cachedEndpointVolume;

void InvalidateEndpointCache() {
    g_cachedEndpointVolume = nullptr;
}

winrt::com_ptr<IAudioEndpointVolume> EndpointVolume() {
    if (g_cachedEndpointVolume) {
        float probe = 0.0f;
        if (SUCCEEDED(g_cachedEndpointVolume->GetMasterVolumeLevelScalar(&probe))) {
            return g_cachedEndpointVolume;
        }
        g_cachedEndpointVolume = nullptr;
    }

    auto device = DefaultRenderDevice();
    if (!device) {
        return nullptr;
    }
    winrt::com_ptr<IAudioEndpointVolume> volume;
    if (FAILED(device->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, nullptr,
                                volume.put_void()))) {
        return nullptr;
    }
    g_cachedEndpointVolume = volume;
    return volume;
}

int GetMasterVolume() {
    auto volume = EndpointVolume();
    if (!volume) {
        return 0;
    }
    float level = 0.0f;
    if (FAILED(volume->GetMasterVolumeLevelScalar(&level))) {
        return 0;
    }
    return std::clamp(static_cast<int>(level * 100.0f + 0.5f), 0, 100);
}

void SetMasterVolume(int percent) {
    auto volume = EndpointVolume();
    if (!volume) {
        return;
    }
    float level = std::clamp(percent, 0, 100) / 100.0f;
    volume->SetMasterVolumeLevelScalar(level, nullptr);
    if (percent > 0) {
        volume->SetMute(FALSE, nullptr);
    }
}

bool GetMasterMute() {
    auto volume = EndpointVolume();
    if (!volume) {
        return false;
    }
    BOOL muted = FALSE;
    volume->GetMute(&muted);
    return muted != FALSE;
}

std::wstring GetOutputDeviceName() {
    auto device = DefaultRenderDevice();
    if (!device) {
        return L"No output device";
    }
    std::wstring name = DeviceFriendlyName(device.get());
    return name.empty() ? L"Output device" : name;
}

std::vector<OutputDevice> EnumerateOutputDevices() {
    std::vector<OutputDevice> devices;
    auto enumerator = DeviceEnumerator();
    if (!enumerator) {
        return devices;
    }

    std::wstring defaultId;
    {
        winrt::com_ptr<IMMDevice> defaultDevice;
        if (SUCCEEDED(
                enumerator->GetDefaultAudioEndpoint(eRender, eConsole, defaultDevice.put()))) {
            defaultId = DeviceId(defaultDevice.get());
        }
    }

    winrt::com_ptr<IMMDeviceCollection> collection;
    if (FAILED(enumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, collection.put()))) {
        return devices;
    }

    UINT count = 0;
    collection->GetCount(&count);
    for (UINT i = 0; i < count; i++) {
        winrt::com_ptr<IMMDevice> device;
        if (FAILED(collection->Item(i, device.put())) || !device) {
            continue;
        }
        OutputDevice info;
        info.id = DeviceId(device.get());
        info.name = DeviceFriendlyName(device.get());
        if (info.id.empty() || info.name.empty()) {
            continue;
        }
        info.isDefault = !defaultId.empty() && info.id == defaultId;
        devices.push_back(std::move(info));
    }

    std::sort(devices.begin(), devices.end(),
              [](const OutputDevice& a, const OutputDevice& b) {
                  if (a.isDefault != b.isDefault) {
                      return a.isDefault;
                  }
                  return ToLowerCopy(a.name) < ToLowerCopy(b.name);
              });
    return devices;
}

bool SetDefaultOutputDevice(const std::wstring& deviceId) {
    if (deviceId.empty()) {
        return false;
    }
    winrt::com_ptr<IPolicyConfig> policy;
    if (FAILED(CoCreateInstance(kCLSID_PolicyConfigClient, nullptr, CLSCTX_ALL,
                                kIID_IPolicyConfig, policy.put_void())) ||
        !policy) {
        Wh_Log(L"IPolicyConfig unavailable; cannot switch the default output device");
        return false;
    }
    // All three roles, otherwise communication apps keep the old endpoint.
    bool ok = SUCCEEDED(policy->SetDefaultEndpoint(deviceId.c_str(), eConsole));
    policy->SetDefaultEndpoint(deviceId.c_str(), eMultimedia);
    policy->SetDefaultEndpoint(deviceId.c_str(), eCommunications);
    InvalidateEndpointCache();
    return ok;
}

std::wstring ProcessDisplayName(DWORD pid) {
    if (!pid) {
        return L"";
    }
    HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!process) {
        return L"";
    }
    wchar_t path[MAX_PATH]{};
    DWORD size = ARRAYSIZE(path);
    bool ok = QueryFullProcessImageNameW(process, 0, path, &size) != FALSE;
    CloseHandle(process);
    if (!ok) {
        return L"";
    }
    std::wstring name = path;
    size_t slash = name.find_last_of(L"\\/");
    if (slash != std::wstring::npos) {
        name.erase(0, slash + 1);
    }
    size_t dot = name.find_last_of(L'.');
    if (dot != std::wstring::npos) {
        name.resize(dot);
    }
    return name;
}

std::vector<SessionInfo> EnumerateSessions() {
    std::vector<SessionInfo> sessions;

    auto device = DefaultRenderDevice();
    if (!device) {
        return sessions;
    }

    winrt::com_ptr<IAudioSessionManager2> manager;
    if (FAILED(device->Activate(__uuidof(IAudioSessionManager2), CLSCTX_ALL, nullptr,
                                manager.put_void()))) {
        return sessions;
    }

    winrt::com_ptr<IAudioSessionEnumerator> enumerator;
    if (FAILED(manager->GetSessionEnumerator(enumerator.put()))) {
        return sessions;
    }

    int count = 0;
    if (FAILED(enumerator->GetCount(&count))) {
        return sessions;
    }

    for (int i = 0; i < count; i++) {
        winrt::com_ptr<IAudioSessionControl> control;
        if (FAILED(enumerator->GetSession(i, control.put()))) {
            continue;
        }
        auto control2 = control.try_as<IAudioSessionControl2>();
        if (!control2) {
            continue;
        }

        AudioSessionState state = AudioSessionStateExpired;
        if (SUCCEEDED(control->GetState(&state)) && state == AudioSessionStateExpired) {
            continue;
        }

        SessionInfo info;
        info.isSystemSounds = control2->IsSystemSoundsSession() == S_OK;

        LPWSTR displayName = nullptr;
        if (SUCCEEDED(control->GetDisplayName(&displayName)) && displayName && *displayName) {
            info.name = displayName;
        }
        if (displayName) {
            CoTaskMemFree(displayName);
        }

        if (info.isSystemSounds) {
            info.name = L"System Sounds";
        } else if (info.name.empty()) {
            DWORD pid = 0;
            control2->GetProcessId(&pid);
            info.name = ProcessDisplayName(pid);
        }
        if (info.name.empty()) {
            continue;
        }

        auto simpleVolume = control.try_as<ISimpleAudioVolume>();
        if (!simpleVolume) {
            continue;
        }
        float level = 1.0f;
        BOOL muted = FALSE;
        simpleVolume->GetMasterVolume(&level);
        simpleVolume->GetMute(&muted);
        info.volume = std::clamp(static_cast<int>(level * 100.0f + 0.5f), 0, 100);
        info.muted = muted != FALSE;
        info.control = simpleVolume;

        sessions.push_back(std::move(info));
    }

    // Stable ordering: system sounds first, then alphabetical, so the list
    // doesn't shuffle every time the flyout is opened.
    std::sort(sessions.begin(), sessions.end(),
              [](const SessionInfo& a, const SessionInfo& b) {
                  if (a.isSystemSounds != b.isSystemSounds) {
                      return a.isSystemSounds;
                  }
                  return ToLowerCopy(a.name) < ToLowerCopy(b.name);
              });

    return sessions;
}

}  // namespace audio

// ============================================================================
// Brightness subsystem
//
// Internal laptop panels answer to WMI (root\WMI, WmiSetBrightness); external
// monitors generally don't, but most answer DDC/CI through dxva2's
// SetMonitorBrightness. Both are tried, cheapest-first, and which one worked is
// remembered so the slider doesn't re-probe on every drag.
// ============================================================================

namespace brightness {

static const CLSID kCLSID_WbemLocator = {
    0x4590f811, 0x1d3a, 0x11d0, {0x89, 0x1f, 0x00, 0xaa, 0x00, 0x4b, 0x2e, 0x24}};
static const IID kIID_IWbemLocator = {
    0xdc12a687, 0x737f, 0x11cf, {0x88, 0x4d, 0x00, 0xaa, 0x00, 0x4b, 0x2e, 0x24}};

enum class Backend { Unknown, Wmi, Ddc, None };
Backend g_backend = Backend::Unknown;
std::mutex g_brightnessMutex;

[[clang::no_destroy]] winrt::com_ptr<IWbemServices> g_cachedWmiServices;

winrt::com_ptr<IWbemServices> ConnectWmi() {
    winrt::com_ptr<IWbemLocator> locator;
    if (FAILED(CoCreateInstance(kCLSID_WbemLocator, nullptr, CLSCTX_INPROC_SERVER,
                                kIID_IWbemLocator, locator.put_void()))) {
        return nullptr;
    }

    BSTR nameSpace = SysAllocString(L"root\\WMI");
    winrt::com_ptr<IWbemServices> services;
    HRESULT hr = locator->ConnectServer(nameSpace, nullptr, nullptr, nullptr, 0, nullptr,
                                        nullptr, services.put());
    SysFreeString(nameSpace);
    if (FAILED(hr)) {
        return nullptr;
    }

    CoSetProxyBlanket(services.get(), RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, nullptr,
                      RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, nullptr, EOAC_NONE);
    return services;
}

bool WmiGet(int* outPercent) {
    auto services = ConnectWmi();
    if (!services) {
        return false;
    }

    BSTR language = SysAllocString(L"WQL");
    BSTR query = SysAllocString(L"SELECT * FROM WmiMonitorBrightness");
    winrt::com_ptr<IEnumWbemClassObject> enumerator;
    HRESULT hr = services->ExecQuery(language, query,
                                     WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
                                     nullptr, enumerator.put());
    SysFreeString(query);
    SysFreeString(language);
    if (FAILED(hr) || !enumerator) {
        return false;
    }

    IWbemClassObject* raw = nullptr;
    ULONG returned = 0;
    if (FAILED(enumerator->Next(2000, 1, &raw, &returned)) || returned == 0 || !raw) {
        return false;
    }
    winrt::com_ptr<IWbemClassObject> object;
    object.attach(raw);

    VARIANT value;
    VariantInit(&value);
    bool ok = false;
    if (SUCCEEDED(object->Get(L"CurrentBrightness", 0, &value, nullptr, nullptr))) {
        if (value.vt == VT_UI1) {
            *outPercent = value.bVal;
            ok = true;
        } else if (value.vt == VT_I4) {
            *outPercent = value.lVal;
            ok = true;
        }
    }
    VariantClear(&value);
    return ok;
}

bool WmiSet(int percent) {
    auto services = ConnectWmi();
    if (!services) {
        return false;
    }

    BSTR language = SysAllocString(L"WQL");
    BSTR query = SysAllocString(L"SELECT * FROM WmiMonitorBrightnessMethods");
    winrt::com_ptr<IEnumWbemClassObject> enumerator;
    HRESULT hr = services->ExecQuery(language, query,
                                     WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
                                     nullptr, enumerator.put());
    SysFreeString(query);
    SysFreeString(language);
    if (FAILED(hr) || !enumerator) {
        return false;
    }

    IWbemClassObject* raw = nullptr;
    ULONG returned = 0;
    if (FAILED(enumerator->Next(2000, 1, &raw, &returned)) || returned == 0 || !raw) {
        return false;
    }
    winrt::com_ptr<IWbemClassObject> instance;
    instance.attach(raw);

    VARIANT pathValue;
    VariantInit(&pathValue);
    if (FAILED(instance->Get(L"__PATH", 0, &pathValue, nullptr, nullptr)) ||
        pathValue.vt != VT_BSTR) {
        VariantClear(&pathValue);
        return false;
    }

    bool ok = false;
    BSTR className = SysAllocString(L"WmiMonitorBrightnessMethods");
    BSTR methodName = SysAllocString(L"WmiSetBrightness");
    winrt::com_ptr<IWbemClassObject> classDef;
    if (SUCCEEDED(services->GetObject(className, 0, nullptr, classDef.put(), nullptr)) &&
        classDef) {
        winrt::com_ptr<IWbemClassObject> inParamsDef;
        if (SUCCEEDED(classDef->GetMethod(methodName, 0, inParamsDef.put(), nullptr)) &&
            inParamsDef) {
            winrt::com_ptr<IWbemClassObject> inParams;
            if (SUCCEEDED(inParamsDef->SpawnInstance(0, inParams.put())) && inParams) {
                VARIANT timeout;
                VariantInit(&timeout);
                timeout.vt = VT_I4;
                timeout.lVal = 0;
                inParams->Put(L"Timeout", 0, &timeout, 0);
                VariantClear(&timeout);

                VARIANT level;
                VariantInit(&level);
                level.vt = VT_UI1;
                level.bVal = static_cast<BYTE>(std::clamp(percent, 0, 100));
                inParams->Put(L"Brightness", 0, &level, 0);
                VariantClear(&level);

                ok = SUCCEEDED(services->ExecMethod(pathValue.bstrVal, methodName, 0, nullptr,
                                                    inParams.get(), nullptr, nullptr));
            }
        }
    }
    SysFreeString(methodName);
    SysFreeString(className);
    VariantClear(&pathValue);
    return ok;
}

// dxva2 is loaded dynamically so a missing import library can never break the
// build; these entry points are also absent on some server SKUs.
using GetNumberOfPhysicalMonitors_t = BOOL(WINAPI*)(HMONITOR, LPDWORD);
using GetPhysicalMonitors_t = BOOL(WINAPI*)(HMONITOR, DWORD, LPPHYSICAL_MONITOR);
using DestroyPhysicalMonitors_t = BOOL(WINAPI*)(DWORD, LPPHYSICAL_MONITOR);
using GetMonitorBrightness_t = BOOL(WINAPI*)(HANDLE, LPDWORD, LPDWORD, LPDWORD);
using SetMonitorBrightness_t = BOOL(WINAPI*)(HANDLE, DWORD);

struct Dxva2 {
    HMODULE module = nullptr;
    GetNumberOfPhysicalMonitors_t getCount = nullptr;
    GetPhysicalMonitors_t getMonitors = nullptr;
    DestroyPhysicalMonitors_t destroy = nullptr;
    GetMonitorBrightness_t get = nullptr;
    SetMonitorBrightness_t set = nullptr;
    bool valid() const { return getCount && getMonitors && destroy && get && set; }
};

const Dxva2& GetDxva2() {
    static Dxva2 api = [] {
        Dxva2 result;
        result.module = LoadLibraryEx(L"dxva2.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
        if (!result.module) {
            return result;
        }
        result.getCount = reinterpret_cast<GetNumberOfPhysicalMonitors_t>(
            GetProcAddress(result.module, "GetNumberOfPhysicalMonitorsFromHMONITOR"));
        result.getMonitors = reinterpret_cast<GetPhysicalMonitors_t>(
            GetProcAddress(result.module, "GetPhysicalMonitorsFromHMONITOR"));
        result.destroy = reinterpret_cast<DestroyPhysicalMonitors_t>(
            GetProcAddress(result.module, "DestroyPhysicalMonitors"));
        result.get = reinterpret_cast<GetMonitorBrightness_t>(
            GetProcAddress(result.module, "GetMonitorBrightness"));
        result.set = reinterpret_cast<SetMonitorBrightness_t>(
            GetProcAddress(result.module, "SetMonitorBrightness"));
        return result;
    }();
    return api;
}

HMONITOR BarMonitor();  // defined with the monitor helpers below

bool DdcForEach(const std::function<bool(HANDLE)>& callback) {
    const Dxva2& api = GetDxva2();
    if (!api.valid()) {
        return false;
    }
    HMONITOR monitor = BarMonitor();
    if (!monitor) {
        return false;
    }
    DWORD count = 0;
    if (!api.getCount(monitor, &count) || count == 0) {
        return false;
    }
    std::vector<PHYSICAL_MONITOR> monitors(count);
    if (!api.getMonitors(monitor, count, monitors.data())) {
        return false;
    }
    bool any = false;
    for (auto& physical : monitors) {
        if (callback(physical.hPhysicalMonitor)) {
            any = true;
        }
    }
    api.destroy(count, monitors.data());
    return any;
}

bool DdcGet(int* outPercent) {
    const Dxva2& api = GetDxva2();
    int found = -1;
    DdcForEach([&](HANDLE handle) {
        DWORD minimum = 0, current = 0, maximum = 0;
        if (api.get(handle, &minimum, &current, &maximum) && maximum > minimum) {
            found = static_cast<int>((current - minimum) * 100 / (maximum - minimum));
            return true;
        }
        return false;
    });
    if (found < 0) {
        return false;
    }
    *outPercent = std::clamp(found, 0, 100);
    return true;
}

bool DdcSet(int percent) {
    const Dxva2& api = GetDxva2();
    return DdcForEach([&](HANDLE handle) {
        DWORD minimum = 0, current = 0, maximum = 0;
        if (!api.get(handle, &minimum, &current, &maximum) || maximum <= minimum) {
            return false;
        }
        DWORD target = minimum + (maximum - minimum) * std::clamp(percent, 0, 100) / 100;
        return api.set(handle, target) != FALSE;
    });
}

// Last value we read or wrote. Reading brightness is a full WMI query, far too
// slow to do once per wheel notch, so the wheel path works off this.
int g_lastKnown = -1;

int Get() {
    std::lock_guard<std::mutex> lock(g_brightnessMutex);
    int percent = 0;
    if (g_backend == Backend::Unknown || g_backend == Backend::Wmi) {
        if (WmiGet(&percent)) {
            g_backend = Backend::Wmi;
            g_lastKnown = std::clamp(percent, 0, 100);
            return g_lastKnown;
        }
    }
    if (g_backend == Backend::Unknown || g_backend == Backend::Ddc) {
        if (DdcGet(&percent)) {
            g_backend = Backend::Ddc;
            g_lastKnown = std::clamp(percent, 0, 100);
            return g_lastKnown;
        }
    }
    if (g_backend == Backend::Unknown) {
        g_backend = Backend::None;
    }
    return 50;
}

int GetFast() {
    {
        std::lock_guard<std::mutex> lock(g_brightnessMutex);
        if (g_lastKnown >= 0) return g_lastKnown;
    }
    return Get(); // Get() will lock and update the cache
}

void Set(int percent) {
    std::lock_guard<std::mutex> lock(g_brightnessMutex);
    percent = std::clamp(percent, 0, 100);
    g_lastKnown = percent;
    switch (g_backend) {
        case Backend::Wmi:
            if (!WmiSet(percent)) {
                DdcSet(percent);
            }
            return;
        case Backend::Ddc:
            DdcSet(percent);
            return;
        case Backend::None:
            return;
        case Backend::Unknown:
        default:
            if (WmiSet(percent)) {
                g_backend = Backend::Wmi;
            } else if (DdcSet(percent)) {
                g_backend = Backend::Ddc;
            } else {
                g_backend = Backend::None;
            }
            return;
    }
}

bool Available() {
    if (g_backend == Backend::Unknown) {
        Get();
    }
    return g_backend != Backend::None;
}

}  // namespace brightness

// ============================================================================
// Dark mode
// ============================================================================

bool IsAppsDarkMode() {
    DWORD value = 1;
    DWORD size = sizeof(value);
    if (RegGetValue(HKEY_CURRENT_USER,
                    L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                    L"AppsUseLightTheme", RRF_RT_REG_DWORD, nullptr, &value, &size) !=
        ERROR_SUCCESS) {
        return false;
    }
    return value == 0;
}

void SetAppsDarkMode(bool dark) {
    HKEY key = nullptr;
    if (RegCreateKeyEx(HKEY_CURRENT_USER,
                       L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", 0,
                       nullptr, 0, KEY_SET_VALUE, nullptr, &key, nullptr) != ERROR_SUCCESS) {
        return;
    }
    DWORD value = dark ? 0 : 1;
    RegSetValueEx(key, L"AppsUseLightTheme", 0, REG_DWORD,
                  reinterpret_cast<const BYTE*>(&value), sizeof(value));
    RegSetValueEx(key, L"SystemUsesLightTheme", 0, REG_DWORD,
                  reinterpret_cast<const BYTE*>(&value), sizeof(value));
    RegCloseKey(key);
    // Broadcast so already-running apps repaint, the same notification the
    // Settings app sends.
    DWORD_PTR result = 0;
    SendMessageTimeout(HWND_BROADCAST, WM_SETTINGCHANGE, 0,
                       reinterpret_cast<LPARAM>(L"ImmersiveColorSet"), SMTO_ABORTIFHUNG, 200,
                       &result);
}


// ============================================================================
// Wi-Fi subsystem
//
// wlanapi.dll is resolved at runtime rather than linked, so a toolchain without
// the import library still builds and a machine with no WLAN service degrades
// to "Wi-Fi unavailable" instead of failing to load the mod.
// ============================================================================

namespace wifi {

using WlanOpenHandle_t = DWORD(WINAPI*)(DWORD, PVOID, PDWORD, PHANDLE);
using WlanCloseHandle_t = DWORD(WINAPI*)(HANDLE, PVOID);
using WlanEnumInterfaces_t = DWORD(WINAPI*)(HANDLE, PVOID, PWLAN_INTERFACE_INFO_LIST*);
using WlanQueryInterface_t = DWORD(WINAPI*)(HANDLE, const GUID*, WLAN_INTF_OPCODE, PVOID,
                                            PDWORD, PVOID*, PWLAN_OPCODE_VALUE_TYPE);
using WlanSetInterface_t = DWORD(WINAPI*)(HANDLE, const GUID*, WLAN_INTF_OPCODE, DWORD,
                                          const PVOID, PVOID);
using WlanGetAvailableNetworkList_t = DWORD(WINAPI*)(HANDLE, const GUID*, DWORD, PVOID,
                                                     PWLAN_AVAILABLE_NETWORK_LIST*);
using WlanScan_t = DWORD(WINAPI*)(HANDLE, const GUID*, const PDOT11_SSID,
                                  const PWLAN_RAW_DATA, PVOID);
using WlanConnect_t = DWORD(WINAPI*)(HANDLE, const GUID*, const PWLAN_CONNECTION_PARAMETERS,
                                     PVOID);
using WlanDisconnect_t = DWORD(WINAPI*)(HANDLE, const GUID*, PVOID);
using WlanSetProfile_t = DWORD(WINAPI*)(HANDLE, const GUID*, DWORD, LPCWSTR, LPCWSTR, BOOL,
                                        PVOID, DWORD*);
using WlanDeleteProfile_t = DWORD(WINAPI*)(HANDLE, const GUID*, LPCWSTR, PVOID);
using WlanFreeMemory_t = VOID(WINAPI*)(PVOID);

struct Api {
    HMODULE module = nullptr;
    WlanOpenHandle_t open = nullptr;
    WlanCloseHandle_t close = nullptr;
    WlanEnumInterfaces_t enumInterfaces = nullptr;
    WlanQueryInterface_t query = nullptr;
    WlanSetInterface_t setInterface = nullptr;
    WlanGetAvailableNetworkList_t getNetworks = nullptr;
    WlanScan_t scan = nullptr;
    WlanConnect_t connect = nullptr;
    WlanDisconnect_t disconnect = nullptr;
    WlanSetProfile_t setProfile = nullptr;
    WlanDeleteProfile_t deleteProfile = nullptr;
    WlanFreeMemory_t freeMemory = nullptr;

    bool valid() const {
        return open && close && enumInterfaces && query && getNetworks && connect &&
               disconnect && freeMemory;
    }
};

const Api& GetApi() {
    static Api api = [] {
        Api result;
        result.module = LoadLibraryEx(L"wlanapi.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
        if (!result.module) {
            return result;
        }
        auto bind = [&](auto& target, const char* name) {
            target = reinterpret_cast<std::decay_t<decltype(target)>>(
                GetProcAddress(result.module, name));
        };
        bind(result.open, "WlanOpenHandle");
        bind(result.close, "WlanCloseHandle");
        bind(result.enumInterfaces, "WlanEnumInterfaces");
        bind(result.query, "WlanQueryInterface");
        bind(result.setInterface, "WlanSetInterface");
        bind(result.getNetworks, "WlanGetAvailableNetworkList");
        bind(result.scan, "WlanScan");
        bind(result.connect, "WlanConnect");
        bind(result.disconnect, "WlanDisconnect");
        bind(result.setProfile, "WlanSetProfile");
        bind(result.deleteProfile, "WlanDeleteProfile");
        bind(result.freeMemory, "WlanFreeMemory");
        return result;
    }();
    return api;
}

// RAII around the client handle plus the first usable interface GUID. Every
// operation opens its own handle -- these calls are infrequent and a long-lived
// handle would have to survive the WLAN service restarting.
struct Session {
    const Api& api = GetApi();
    HANDLE handle = nullptr;
    GUID interfaceGuid{};
    bool haveInterface = false;

    Session() {
        if (!api.valid()) {
            return;
        }
        DWORD negotiated = 0;
        if (api.open(2, nullptr, &negotiated, &handle) != ERROR_SUCCESS) {
            handle = nullptr;
            return;
        }
        PWLAN_INTERFACE_INFO_LIST list = nullptr;
        if (api.enumInterfaces(handle, nullptr, &list) == ERROR_SUCCESS && list) {
            for (DWORD i = 0; i < list->dwNumberOfItems; i++) {
                interfaceGuid = list->InterfaceInfo[i].InterfaceGuid;
                haveInterface = true;
                if (list->InterfaceInfo[i].isState == wlan_interface_state_connected) {
                    break;  // prefer the connected adapter when there are several
                }
            }
        }
        if (list) {
            api.freeMemory(list);
        }
    }

    ~Session() {
        if (handle) {
            api.close(handle, nullptr);
        }
    }

    Session(const Session&) = delete;
    Session& operator=(const Session&) = delete;

    bool ok() const { return handle && haveInterface; }
};

struct Network {
    std::wstring ssid;
    std::wstring profileName;
    int signal = 0;
    bool secured = false;
    bool connected = false;
    bool hasProfile = false;
    DOT11_AUTH_ALGORITHM authAlgorithm = DOT11_AUTH_ALGO_80211_OPEN;
    DOT11_CIPHER_ALGORITHM cipherAlgorithm = DOT11_CIPHER_ALGO_NONE;
};

struct Status {
    bool available = false;
    bool radioOn = false;
    bool connected = false;
    std::wstring ssid;
    int signal = 0;
};

std::wstring SsidToString(const DOT11_SSID& ssid) {
    if (ssid.uSSIDLength == 0) {
        return L"";
    }
    int length = static_cast<int>(ssid.uSSIDLength);
    int needed = MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(ssid.ucSSID),
                                     length, nullptr, 0);
    if (needed <= 0) {
        return L"";
    }
    std::wstring result(needed, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(ssid.ucSSID), length,
                        result.data(), needed);
    return result;
}

bool StringToSsid(const std::wstring& text, DOT11_SSID* out) {
    *out = {};
    int written = WideCharToMultiByte(CP_UTF8, 0, text.c_str(), static_cast<int>(text.size()),
                                      reinterpret_cast<char*>(out->ucSSID),
                                      sizeof(out->ucSSID), nullptr, nullptr);
    if (written <= 0) {
        return false;
    }
    out->uSSIDLength = static_cast<ULONG>(written);
    return true;
}

bool RadioIsOn(const Session& session) {
    if (!session.ok() || !session.api.query) {
        return false;
    }
    DWORD size = 0;
    PWLAN_RADIO_STATE state = nullptr;
    if (session.api.query(session.handle, &session.interfaceGuid, wlan_intf_opcode_radio_state,
                          nullptr, &size, reinterpret_cast<PVOID*>(&state), nullptr) !=
            ERROR_SUCCESS ||
        !state) {
        return false;
    }
    bool on = false;
    for (DWORD i = 0; i < state->dwNumberOfPhys; i++) {
        const auto& phy = state->PhyRadioState[i];
        if (phy.dot11SoftwareRadioState == dot11_radio_state_on &&
            phy.dot11HardwareRadioState != dot11_radio_state_off) {
            on = true;
            break;
        }
    }
    session.api.freeMemory(state);
    return on;
}

bool SetRadio(bool on) {
    Session session;
    if (!session.ok() || !session.api.setInterface) {
        return false;
    }
    WLAN_PHY_RADIO_STATE state{};
    state.dwPhyIndex = 0;
    state.dot11SoftwareRadioState = on ? dot11_radio_state_on : dot11_radio_state_off;
    return session.api.setInterface(session.handle, &session.interfaceGuid,
                                    wlan_intf_opcode_radio_state, sizeof(state), &state,
                                    nullptr) == ERROR_SUCCESS;
}

Status GetStatus() {
    Status status;
    Session session;
    if (!session.ok()) {
        return status;
    }
    status.available = true;
    status.radioOn = RadioIsOn(session);

    DWORD size = 0;
    PWLAN_CONNECTION_ATTRIBUTES attributes = nullptr;
    if (session.api.query(session.handle, &session.interfaceGuid,
                          wlan_intf_opcode_current_connection, nullptr, &size,
                          reinterpret_cast<PVOID*>(&attributes), nullptr) == ERROR_SUCCESS &&
        attributes) {
        if (attributes->isState == wlan_interface_state_connected) {
            status.connected = true;
            status.ssid = SsidToString(attributes->wlanAssociationAttributes.dot11Ssid);
            status.signal =
                static_cast<int>(attributes->wlanAssociationAttributes.wlanSignalQuality);
        }
        session.api.freeMemory(attributes);
    }
    return status;
}

// Kicks the adapter into scanning. Results arrive asynchronously, so callers
// re-read the network list a moment later rather than expecting fresh data
// straight after this returns.
void RequestScan() {
    Session session;
    if (!session.ok() || !session.api.scan) {
        return;
    }
    session.api.scan(session.handle, &session.interfaceGuid, nullptr, nullptr, nullptr);
}

std::vector<Network> EnumerateNetworks() {
    std::vector<Network> networks;
    Session session;
    if (!session.ok()) {
        return networks;
    }

    PWLAN_AVAILABLE_NETWORK_LIST list = nullptr;
    if (session.api.getNetworks(session.handle, &session.interfaceGuid,
                                WLAN_AVAILABLE_NETWORK_INCLUDE_ALL_MANUAL_HIDDEN_PROFILES,
                                nullptr, &list) != ERROR_SUCCESS ||
        !list) {
        return networks;
    }

    // The adapter reports one entry per (SSID, profile) pair, so the same
    // network can appear several times. Keep the strongest, and let any entry
    // that is connected or has a profile contribute those flags.
    std::map<std::wstring, Network> best;
    for (DWORD i = 0; i < list->dwNumberOfItems; i++) {
        const WLAN_AVAILABLE_NETWORK& entry = list->Network[i];
        std::wstring ssid = SsidToString(entry.dot11Ssid);
        if (ssid.empty()) {
            continue;  // hidden network with no usable name
        }

        Network network;
        network.ssid = ssid;
        network.profileName = entry.strProfileName;
        network.signal = static_cast<int>(entry.wlanSignalQuality);
        network.secured = entry.bSecurityEnabled != FALSE;
        network.connected = (entry.dwFlags & WLAN_AVAILABLE_NETWORK_CONNECTED) != 0;
        network.hasProfile = (entry.dwFlags & WLAN_AVAILABLE_NETWORK_HAS_PROFILE) != 0;
        network.authAlgorithm = entry.dot11DefaultAuthAlgorithm;
        network.cipherAlgorithm = entry.dot11DefaultCipherAlgorithm;

        auto found = best.find(ssid);
        if (found == best.end()) {
            best.emplace(ssid, std::move(network));
        } else {
            Network& existing = found->second;
            existing.signal = (std::max)(existing.signal, network.signal);
            existing.connected = existing.connected || network.connected;
            if (network.hasProfile && !existing.hasProfile) {
                existing.hasProfile = true;
                existing.profileName = network.profileName;
            }
        }
    }
    session.api.freeMemory(list);

    networks.reserve(best.size());
    for (auto& pair : best) {
        networks.push_back(std::move(pair.second));
    }

    // Connected first, then by signal, so the list reads top-down like the
    // Windows flyout does.
    std::sort(networks.begin(), networks.end(), [](const Network& a, const Network& b) {
        if (a.connected != b.connected) {
            return a.connected;
        }
        return a.signal > b.signal;
    });
    return networks;
}

std::wstring BuildProfileXml(const Network& network, const std::wstring& password) {
    std::wstring authentication = L"open";
    std::wstring encryption = L"none";
    bool usePassword = false;

    switch (network.authAlgorithm) {
        case DOT11_AUTH_ALGO_RSNA_PSK:
            authentication = L"WPA2PSK";
            encryption = network.cipherAlgorithm == DOT11_CIPHER_ALGO_TKIP ? L"TKIP" : L"AES";
            usePassword = true;
            break;
        case DOT11_AUTH_ALGO_WPA_PSK:
        case DOT11_AUTH_ALGO_WPA_NONE:
            authentication = L"WPAPSK";
            encryption = network.cipherAlgorithm == DOT11_CIPHER_ALGO_CCMP ? L"AES" : L"TKIP";
            usePassword = true;
            break;
        default:
            if (network.secured) {
                // WPA3-SAE and anything else unrecognised: WPA2PSK is the
                // widest-compatibility guess, and transition-mode APs accept it.
                authentication = L"WPA2PSK";
                encryption = L"AES";
                usePassword = true;
            }
            break;
    }

    std::wstring ssid = EscapeXmlAttr(network.ssid);
    std::wstring xml;
    xml += L"<?xml version=\"1.0\"?>";
    xml += L"<WLANProfile xmlns=\"http://www.microsoft.com/networking/WLAN/profile/v1\">";
    xml += L"<name>" + ssid + L"</name>";
    xml += L"<SSIDConfig><SSID><name>" + ssid + L"</name></SSID></SSIDConfig>";
    xml += L"<connectionType>ESS</connectionType>";
    xml += L"<connectionMode>auto</connectionMode>";
    xml += L"<MSM><security><authEncryption>";
    xml += L"<authentication>" + authentication + L"</authentication>";
    xml += L"<encryption>" + encryption + L"</encryption>";
    xml += L"<useOneX>false</useOneX>";
    xml += L"</authEncryption>";
    if (usePassword) {
        xml += L"<sharedKey><keyType>passPhrase</keyType><protected>false</protected>";
        xml += L"<keyMaterial>" + EscapeXmlAttr(password) + L"</keyMaterial></sharedKey>";
    }
    xml += L"</security></MSM></WLANProfile>";
    return xml;
}

// A stored profile is enough on its own; otherwise one is written first from
// the network's advertised auth/cipher plus the password the user typed.
bool Connect(const Network& network, const std::wstring& password) {
    Session session;
    if (!session.ok()) {
        return false;
    }

    std::wstring profileName = network.hasProfile && !network.profileName.empty()
                                   ? network.profileName
                                   : network.ssid;

    if (!network.hasProfile) {
        if (!session.api.setProfile) {
            return false;
        }
        std::wstring xml = BuildProfileXml(network, password);
        DWORD reason = 0;
        DWORD result = session.api.setProfile(session.handle, &session.interfaceGuid, 0,
                                              xml.c_str(), nullptr, TRUE, nullptr, &reason);
        if (result != ERROR_SUCCESS) {
            Wh_Log(L"WlanSetProfile failed for %s: %u (reason %u)", network.ssid.c_str(),
                   result, reason);
            return false;
        }
        profileName = network.ssid;
    }

    DOT11_SSID ssid{};
    if (!StringToSsid(network.ssid, &ssid)) {
        return false;
    }

    WLAN_CONNECTION_PARAMETERS parameters{};
    parameters.wlanConnectionMode = wlan_connection_mode_profile;
    parameters.strProfile = profileName.c_str();
    parameters.pDot11Ssid = &ssid;
    parameters.dwFlags = 0;
    parameters.dot11BssType = dot11_BSS_type_infrastructure;

    DWORD result =
        session.api.connect(session.handle, &session.interfaceGuid, &parameters, nullptr);
    if (result != ERROR_SUCCESS) {
        Wh_Log(L"WlanConnect failed for %s: %u", network.ssid.c_str(), result);
        return false;
    }
    return true;
}

bool Disconnect() {
    Session session;
    if (!session.ok()) {
        return false;
    }
    return session.api.disconnect(session.handle, &session.interfaceGuid, nullptr) ==
           ERROR_SUCCESS;
}

// Used when a saved password turns out to be wrong: without this the stale
// profile keeps being reused and the password prompt never appears again.
bool ForgetProfile(const std::wstring& profileName) {
    Session session;
    if (!session.ok() || !session.api.deleteProfile || profileName.empty()) {
        return false;
    }
    return session.api.deleteProfile(session.handle, &session.interfaceGuid,
                                     profileName.c_str(), nullptr) == ERROR_SUCCESS;
}

}  // namespace wifi

// ============================================================================
// Bluetooth subsystem
//
// bthprops.cpl carries the Bluetooth API surface; like wlanapi it is resolved
// at runtime. Connecting is done by turning a device's installed services on,
// which is what "connect" means for the HID/audio profiles that make up almost
// everything a user has paired.
// ============================================================================

namespace bluetooth {

bool g_bluetoothRadioOn = false;
std::mutex g_bluetoothRadioMutex;

using BluetoothFindFirstRadio_t = HANDLE(WINAPI*)(const BLUETOOTH_FIND_RADIO_PARAMS*, HANDLE*);
using BluetoothFindNextRadio_t = BOOL(WINAPI*)(HANDLE, HANDLE*);
using BluetoothFindRadioClose_t = BOOL(WINAPI*)(HANDLE);
using BluetoothFindFirstDevice_t = HANDLE(WINAPI*)(const BLUETOOTH_DEVICE_SEARCH_PARAMS*,
                                                   BLUETOOTH_DEVICE_INFO*);
using BluetoothFindNextDevice_t = BOOL(WINAPI*)(HANDLE, BLUETOOTH_DEVICE_INFO*);
using BluetoothFindDeviceClose_t = BOOL(WINAPI*)(HANDLE);
using BluetoothEnumerateInstalledServices_t = DWORD(WINAPI*)(HANDLE,
                                                             const BLUETOOTH_DEVICE_INFO*,
                                                             DWORD*, GUID*);
using BluetoothSetServiceState_t = DWORD(WINAPI*)(HANDLE, const BLUETOOTH_DEVICE_INFO*,
                                                  const GUID*, DWORD);
using BluetoothGetRadioInfo_t = DWORD(WINAPI*)(HANDLE, PBLUETOOTH_RADIO_INFO);

struct Api {
    HMODULE module = nullptr;
    BluetoothFindFirstRadio_t findFirstRadio = nullptr;
    BluetoothFindNextRadio_t findNextRadio = nullptr;
    BluetoothFindRadioClose_t findRadioClose = nullptr;
    BluetoothFindFirstDevice_t findFirstDevice = nullptr;
    BluetoothFindNextDevice_t findNextDevice = nullptr;
    BluetoothFindDeviceClose_t findDeviceClose = nullptr;
    BluetoothEnumerateInstalledServices_t enumerateServices = nullptr;
    BluetoothSetServiceState_t setServiceState = nullptr;
    BluetoothGetRadioInfo_t getRadioInfo = nullptr;

    bool valid() const {
        return findFirstRadio && findRadioClose && findFirstDevice && findNextDevice &&
               findDeviceClose;
    }
};

const Api& GetApi() {
    static Api api = [] {
        Api result;
        // bthprops.cpl is the documented home for these exports; irprops.cpl is
        // the older name still present on some builds.
        result.module = LoadLibraryEx(L"bthprops.cpl", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
        if (!result.module) {
            result.module =
                LoadLibraryEx(L"irprops.cpl", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
        }
        if (!result.module) {
            return result;
        }
        auto bind = [&](auto& target, const char* name) {
            target = reinterpret_cast<std::decay_t<decltype(target)>>(
                GetProcAddress(result.module, name));
        };
        bind(result.findFirstRadio, "BluetoothFindFirstRadio");
        bind(result.findNextRadio, "BluetoothFindNextRadio");
        bind(result.findRadioClose, "BluetoothFindRadioClose");
        bind(result.findFirstDevice, "BluetoothFindFirstDevice");
        bind(result.findNextDevice, "BluetoothFindNextDevice");
        bind(result.findDeviceClose, "BluetoothFindDeviceClose");
        bind(result.enumerateServices, "BluetoothEnumerateInstalledServices");
        bind(result.setServiceState, "BluetoothSetServiceState");
        bind(result.getRadioInfo, "BluetoothGetRadioInfo");
        return result;
    }();
    return api;
}

struct RadioHandle {
    const Api& api = GetApi();
    HANDLE find = nullptr;
    HANDLE radio = nullptr;

    RadioHandle() {
        if (!api.valid()) {
            return;
        }
        BLUETOOTH_FIND_RADIO_PARAMS params{sizeof(BLUETOOTH_FIND_RADIO_PARAMS)};
        find = api.findFirstRadio(&params, &radio);
        if (!find) {
            radio = nullptr;
        }
    }

    ~RadioHandle() {
        if (radio) {
            CloseHandle(radio);
        }
        if (find) {
            api.findRadioClose(find);
        }
    }

    RadioHandle(const RadioHandle&) = delete;
    RadioHandle& operator=(const RadioHandle&) = delete;

    bool ok() const { return radio != nullptr; }
};

struct Device {
    BLUETOOTH_ADDRESS address{};
    std::wstring name;
    bool connected = false;
    bool paired = false;
    ULONG classOfDevice = 0;
    int batteryPercent = -1; // -1 = unknown
};

bool IsRadioOn() {
    std::lock_guard<std::mutex> lock(g_bluetoothRadioMutex);
    return g_bluetoothRadioOn;
}

bool Available() {
    // Only check if the API exists, NOT if the radio handle exists.
    // This keeps the toggle enabled when the radio is off, so you can turn it back on.
    return GetApi().valid();
}

bool SetRadio(bool on) {
#if TOPBAR_HAS_RADIOS
    try {
        using namespace winrt::Windows::Devices::Radios;
        auto access = Radio::RequestAccessAsync().get();
        if (access != RadioAccessStatus::Allowed) {
            Wh_Log(L"Radio access denied; cannot toggle Bluetooth");
            return false;
        }
        auto radios = Radio::GetRadiosAsync().get();
        for (auto&& radio : radios) {
            if (radio.Kind() != RadioKind::Bluetooth) {
                continue;
            }
            radio.SetStateAsync(on ? RadioState::On : RadioState::Off).get();
            return true;
        }
    } catch (const winrt::hresult_error& error) {
        Wh_Log(L"Bluetooth radio toggle failed: %s", error.message().c_str());
    } catch (...) {
    }
#else
    (void)on;
#endif
    return false;
}

// Enumerating paired devices is fast. Inquiry for nearby unpaired ones is not:
// cTimeoutMultiplier is in 1.28-second units, so this blocks for seconds and
// must only be called from a worker thread.
// Battery percentage reader – must be declared before Enumerate uses it.
int GetBatteryPercent(const BLUETOOTH_ADDRESS& address) {
#if TOPBAR_HAS_BLUETOOTH_LE
    try {
        using namespace winrt::Windows::Devices::Bluetooth;
        using namespace winrt::Windows::Devices::Bluetooth::GenericAttributeProfile;
        auto device = BluetoothLEDevice::FromBluetoothAddressAsync(address.ullLong).get();
        if (!device) return -1;
        auto servicesResult = device.GetGattServicesAsync().get();
        if (servicesResult.Status() != GattCommunicationStatus::Success) return -1;
        auto services = servicesResult.Services();
        for (auto&& service : services) {
            if (service.Uuid() == GattServiceUuids::Battery()) {
                auto characteristicsResult = service.GetCharacteristicsAsync().get();
                if (characteristicsResult.Status() != GattCommunicationStatus::Success) continue;
                auto characteristics = characteristicsResult.Characteristics();
                for (auto&& characteristic : characteristics) {
                    if (characteristic.Uuid() == GattCharacteristicUuids::BatteryLevel()) {
                        auto readResult = characteristic.ReadValueAsync().get();
                        if (readResult.Status() != GattCommunicationStatus::Success) continue;
                        auto value = readResult.Value();
                        auto buffer = value.as<winrt::Windows::Storage::Streams::IBuffer>();
                        uint8_t percent = 0;
                        winrt::Windows::Storage::Streams::DataReader reader = winrt::Windows::Storage::Streams::DataReader::FromBuffer(buffer);
                        reader.ReadBytes(winrt::array_view<uint8_t>(&percent, 1));
                        return static_cast<int>(percent);
                    }
                }
            }
        }
    } catch (...) {}
#endif
    return -1;
}

std::vector<Device> Enumerate(bool includeUnpaired) {
    std::vector<Device> devices;
    const Api& api = GetApi();
    RadioHandle radio;
    if (!radio.ok()) {
        return devices;
    }

    BLUETOOTH_DEVICE_SEARCH_PARAMS params{};
    params.dwSize = sizeof(params);
    params.fReturnAuthenticated = TRUE;
    params.fReturnRemembered = TRUE;
    params.fReturnConnected = TRUE;
    params.fReturnUnknown = includeUnpaired ? TRUE : FALSE;
    params.fIssueInquiry = includeUnpaired ? TRUE : FALSE;
    params.cTimeoutMultiplier = includeUnpaired ? 4 : 0;  // ~5 s when inquiring
    params.hRadio = radio.radio;

    BLUETOOTH_DEVICE_INFO info{};
    info.dwSize = sizeof(info);

    HANDLE find = api.findFirstDevice(&params, &info);
    if (!find) {
        return devices;
    }

    do {
        Device device;
        device.address = info.Address;
        device.name = info.szName;
        device.connected = info.fConnected != FALSE;
        device.paired = info.fAuthenticated != FALSE || info.fRemembered != FALSE;
        device.classOfDevice = info.ulClassofDevice;
        if (device.name.empty()) {
            continue;  // an address with no name is not worth a row
        }
        // Try to read battery percentage (may be -1 if unsupported or failed)
        device.batteryPercent = GetBatteryPercent(device.address);
        devices.push_back(std::move(device));

        info = {};
        info.dwSize = sizeof(info);
    } while (api.findNextDevice(find, &info));

    api.findDeviceClose(find);

    // Connected first, then paired, then discovered -- matching how the flyout
    // groups them.
    std::sort(devices.begin(), devices.end(), [](const Device& a, const Device& b) {
        if (a.connected != b.connected) {
            return a.connected;
        }
        if (a.paired != b.paired) {
            return a.paired;
        }
        return ToLowerCopy(a.name) < ToLowerCopy(b.name);
    });
    return devices;
}

bool FindDeviceInfo(const BLUETOOTH_ADDRESS& address, HANDLE radioHandle,
                    BLUETOOTH_DEVICE_INFO* out) {
    const Api& api = GetApi();
    BLUETOOTH_DEVICE_SEARCH_PARAMS params{};
    params.dwSize = sizeof(params);
    params.fReturnAuthenticated = TRUE;
    params.fReturnRemembered = TRUE;
    params.fReturnConnected = TRUE;
    params.fReturnUnknown = TRUE;
    params.fIssueInquiry = FALSE;
    params.cTimeoutMultiplier = 0;
    params.hRadio = radioHandle;

    BLUETOOTH_DEVICE_INFO info{};
    info.dwSize = sizeof(info);
    HANDLE find = api.findFirstDevice(&params, &info);
    if (!find) {
        return false;
    }

    bool found = false;
    do {
        if (info.Address.ullLong == address.ullLong) {
            *out = info;
            found = true;
            break;
        }
        info = {};
        info.dwSize = sizeof(info);
    } while (api.findNextDevice(find, &info));

    api.findDeviceClose(find);
    return found;
}

bool SetConnected(const BLUETOOTH_ADDRESS& address, bool connect) {
    const Api& api = GetApi();
    if (!api.enumerateServices || !api.setServiceState) {
        return false;
    }
    RadioHandle radio;
    if (!radio.ok()) {
        return false;
    }

    BLUETOOTH_DEVICE_INFO info{};
    if (!FindDeviceInfo(address, radio.radio, &info)) {
        return false;
    }

    DWORD serviceCount = 0;
    DWORD result = api.enumerateServices(radio.radio, &info, &serviceCount, nullptr);
    if (serviceCount == 0) {
        Wh_Log(L"No installed services for %s (0x%08X); nothing to toggle", info.szName,
               result);
        return false;
    }

    std::vector<GUID> services(serviceCount);
    if (api.enumerateServices(radio.radio, &info, &serviceCount, services.data()) !=
        ERROR_SUCCESS) {
        return false;
    }
    services.resize(serviceCount);

    DWORD state = connect ? BLUETOOTH_SERVICE_ENABLE : BLUETOOTH_SERVICE_DISABLE;
    bool any = false;
    for (const GUID& service : services) {
        if (api.setServiceState(radio.radio, &info, &service, state) == ERROR_SUCCESS) {
            any = true;
        }
    }
    return any;
}

}  // namespace bluetooth

// ============================================================================
// System tray
//
// There is no API to read another shell's notification icons, so the real
// taskbar's own tray is driven through UI Automation: find the icon elements
// inside Shell_TrayWnd (and the overflow window), mirror them as buttons, and
// forward clicks back to the original element. Right-click has no automation
// pattern at all, so it is synthesized over the real icon and the cursor is put
// back afterwards.
// ============================================================================

namespace tray {

struct Item {
    std::wstring name;
    winrt::com_ptr<IUIAutomationElement> element;
    RECT bounds{};
};

[[clang::no_destroy]] winrt::com_ptr<IUIAutomation> g_automation;

// Declared by hand: whether CLSID_CUIAutomation is exported as a symbol depends
// on which UUID import library is linked, and this mod links none of them.
static const CLSID kCLSID_CUIAutomation = {
    0xff48dba4, 0x60ef, 0x4201, {0xaa, 0x87, 0x54, 0x10, 0x3e, 0xef, 0x59, 0x4e}};

IUIAutomation* Automation() {
    if (!g_automation) {
        winrt::com_ptr<IUIAutomation> automation;
        if (FAILED(CoCreateInstance(kCLSID_CUIAutomation, nullptr, CLSCTX_INPROC_SERVER,
                                    IID_PPV_ARGS(automation.put())))) {
            return nullptr;
        }
        g_automation = automation;
    }
    return g_automation.get();
}

// Windows 11 splits the tray across several top-level windows: the always-shown
// icons live in Shell_TrayWnd, and the "hidden icons" chevron opens a separate
// XAML popup window. Both are scanned, which is why an item can legitimately be
// missing until the user has opened the overflow once.
std::vector<HWND> TrayHostWindows() {
    std::vector<HWND> windows;
    // Use the actual notification area container instead of the whole taskbar.
    HWND tray = FindWindow(L"Shell_TrayWnd", nullptr);
    if (tray) {
        HWND notifyArea = FindWindowEx(tray, nullptr, L"TrayNotifyWnd", nullptr);
        if (notifyArea) {
            windows.push_back(notifyArea);
        } else {
            windows.push_back(tray); // fallback
        }
    }
    // Overflow (hidden icons) window
    if (HWND overflow = FindWindow(L"TopLevelWindowForOverflowXamlIsland", nullptr)) {
        windows.push_back(overflow);
    }
    if (HWND notify = FindWindow(L"NotifyIconOverflowWindow", nullptr)) {
        windows.push_back(notify);
    }
    return windows;
}

std::wstring ElementName(IUIAutomationElement* element) {
    BSTR name = nullptr;
    if (FAILED(element->get_CurrentName(&name)) || !name) {
        return L"";
    }
    std::wstring result = name;
    SysFreeString(name);
    return result;
}

std::wstring ElementClassName(IUIAutomationElement* element) {
    BSTR className = nullptr;
    if (FAILED(element->get_CurrentClassName(&className)) || !className) {
        return L"";
    }
    std::wstring result = className;
    SysFreeString(className);
    return result;
}

// Enumerating by control type rather than by class name: the class names differ
// between the classic tray, the XAML tray and the overflow popup, but every
// icon surfaces as a Button with a name. Anything unnamed, zero-sized or
// off-screen is dropped, which filters out the chevron's own container and the
// layout scaffolding.
std::vector<Item> Enumerate() {
    std::vector<Item> items;
    IUIAutomation* automation = Automation();
    if (!automation) return items;

    winrt::com_ptr<IUIAutomationCondition> buttonCondition;
    {
        VARIANT value;
        VariantInit(&value);
        value.vt = VT_I4;
        value.lVal = UIA_ButtonControlTypeId;
        automation->CreatePropertyCondition(UIA_ControlTypePropertyId, value, buttonCondition.put());
        VariantClear(&value);
    }
    if (!buttonCondition) return items;

    std::set<std::wstring> seen;

    // Log how many hosts we are looking at
    auto hosts = TrayHostWindows();
    Wh_Log(L"Tray: checking %d host windows", (int)hosts.size());

    for (HWND host : hosts) {
        winrt::com_ptr<IUIAutomationElement> root;
        if (FAILED(automation->ElementFromHandle(host, root.put())) || !root) continue;

        winrt::com_ptr<IUIAutomationElementArray> found;
        if (FAILED(root->FindAll(TreeScope_Descendants, buttonCondition.get(), found.put())) || !found) continue;

        int count = 0;
        found->get_Length(&count);
        Wh_Log(L"Tray: host %p found %d buttons", host, count); // Log how many buttons were found

        for (int i = 0; i < count; i++) {
            winrt::com_ptr<IUIAutomationElement> element;
            if (FAILED(found->GetElement(i, element.put())) || !element) continue;

            std::wstring name = ElementName(element.get());
            if (name.empty()) continue;

            // The TrayNotifyWnd container holds only real tray icons – no need for name filtering.
            // (If we ever fall back to the whole taskbar, we still skip obvious non-icons.)
            if (GetParent(host) != nullptr && host != FindWindow(L"Shell_TrayWnd", nullptr)) {
                // For overflow windows, also check we don't pick up chevron buttons.
                // The size check below already excludes large containers.
            }

            RECT bounds{};
            if (FAILED(element->get_CurrentBoundingRectangle(&bounds))) continue;

            // CRITICAL: Only allow small icon sizes (16x16 up to 40x40). Drop containers.
            int width = bounds.right - bounds.left;
            int height = bounds.bottom - bounds.top;
            if (width <= 0 || height <= 0 || width > 40 || height > 40) continue;

            // Remove duplicates
            std::wstring key = name + L"|" + std::to_wstring(width) + L"x" + std::to_wstring(height);
            if (!seen.insert(key).second) continue;

            Item item;
            item.name = std::move(name);
            item.element = element;
            item.bounds = bounds;
            items.push_back(std::move(item));
        }
    }
    Wh_Log(L"Tray: returning %d items", (int)items.size()); // Log the final count
    
    std::sort(items.begin(), items.end(), [](const Item& a, const Item& b) {
        return ToLowerCopy(a.name) < ToLowerCopy(b.name);
    });
    return items;
}

// Synthesizes a click over the real icon. The cursor is moved, clicked and put
// straight back; there is no way to deliver a tray click without this, because
// the shell reads the cursor position when it decides where to place the menu.
void SyntheticClick(const RECT& bounds, bool rightButton) {
    POINT original{};
    GetCursorPos(&original);

    POINT target{(bounds.left + bounds.right) / 2, (bounds.top + bounds.bottom) / 2};
    SetCursorPos(target.x, target.y);

    INPUT input[2]{};
    input[0].type = INPUT_MOUSE;
    input[0].mi.dwFlags = rightButton ? MOUSEEVENTF_RIGHTDOWN : MOUSEEVENTF_LEFTDOWN;
    input[1].type = INPUT_MOUSE;
    input[1].mi.dwFlags = rightButton ? MOUSEEVENTF_RIGHTUP : MOUSEEVENTF_LEFTUP;
    SendInput(ARRAYSIZE(input), input, sizeof(INPUT));

    // Give the shell a moment to read the cursor position before restoring it,
    // otherwise the context menu lands where the pointer went back to.
    Sleep(60);
    SetCursorPos(original.x, original.y);
}

void InvokeItem(const Item& item) {
    // InvokePattern is cleaner than a synthetic click when the icon supports
    // it, and it doesn't disturb the cursor.
    winrt::com_ptr<IUIAutomationInvokePattern> invoke;
    if (item.element &&
        SUCCEEDED(item.element->GetCurrentPatternAs(UIA_InvokePatternId,
                                                    IID_PPV_ARGS(invoke.put()))) &&
        invoke) {
        if (SUCCEEDED(invoke->Invoke())) {
            return;
        }
    }
    SyntheticClick(item.bounds, false);
}

void ShowItemContextMenu(const Item& item) {
    // No automation pattern raises an app's own tray context menu, so this is
    // the only route: right-click the real icon where it actually sits.
    SyntheticClick(item.bounds, true);
}

}  // namespace tray

// ============================================================================
// Flyout infrastructure
// ============================================================================

constexpr double kFlyoutCorner = 8.0;
constexpr double kTileCorner = 6.0;
constexpr double kRowCorner = 6.0;
constexpr double kPanelWidth = 340.0;

[[clang::no_destroy]] wuxc::Button g_displayButton{nullptr};
[[clang::no_destroy]] wuxc::Button g_soundButton{nullptr};
[[clang::no_destroy]] wuxc::Button g_wifiButton{nullptr};
[[clang::no_destroy]] wuxc::Button g_bluetoothButton{nullptr};
[[clang::no_destroy]] wuxc::Button g_trayButton{nullptr};

// Helper to toggle a flyout open/closed.
void ToggleFlyout(wuxc::Flyout const& flyout, wuxc::Button const& button) {
    if (!flyout || !button) return;
    if (flyout.IsOpen()) {
        flyout.Hide();
    } else {
        flyout.ShowAt(button);
    }
}

[[clang::no_destroy]] wuxc::Flyout g_displayFlyout{nullptr};
[[clang::no_destroy]] wuxc::Flyout g_soundFlyout{nullptr};
[[clang::no_destroy]] wuxc::Flyout g_wifiFlyout{nullptr};
[[clang::no_destroy]] wuxc::Flyout g_bluetoothFlyout{nullptr};
[[clang::no_destroy]] wuxc::Flyout g_trayFlyout{nullptr};
[[clang::no_destroy]] DispatcherTimer g_volumeRevertTimer{nullptr};
[[clang::no_destroy]] DispatcherTimer g_brightnessRevertTimer{nullptr};

[[clang::no_destroy]] wuxc::StackPanel g_displayPanel{nullptr};
[[clang::no_destroy]] wuxc::StackPanel g_soundPanel{nullptr};
[[clang::no_destroy]] wuxc::StackPanel g_wifiPanel{nullptr};
[[clang::no_destroy]] wuxc::StackPanel g_bluetoothPanel{nullptr};
[[clang::no_destroy]] wuxc::StackPanel g_trayPanel{nullptr};
[[clang::no_destroy]] wuxc::Button g_batteryButton{nullptr};
[[clang::no_destroy]] wuxc::Flyout g_batteryFlyout{nullptr};
[[clang::no_destroy]] wuxc::StackPanel g_batteryPanel{nullptr};
// Set while a panel is writing its own controls, so the ValueChanged /Toggled
// handlers that XAML raises during construction don't get mistaken for the user
// actually moving something.
bool g_populatingPanel = false;

// Non-zero once the mod starts tearing down; background workers check it before
// touching XAML or dispatching back to the UI thread.
volatile LONG g_shuttingDown = 0;



// (Crash flag mechanism removed – no registry key needed)



// Which panel sections the user has expanded. Kept outside the panels because
// the panels are rebuilt wholesale on every refresh.
bool g_appMixerExpanded = false;
bool g_outputDevicesExpanded = false;

// Wi-Fi password entry state: when set, the Wi-Fi panel draws the password view
// for this network instead of the network list.
bool g_wifiPasswordPrompt = false;
wifi::Network g_wifiPromptNetwork;
std::wstring g_wifiPromptError;

std::vector<bluetooth::Device> g_bluetoothDevices;
bool g_bluetoothScanning = false;
unsigned long long g_bluetoothConnectingAddress = 0;

std::vector<wifi::Network> g_wifiNetworks;
bool g_wifiScanning = false;
std::wstring g_wifiConnectingSSID;
int g_bluetoothConnectingState = 0; // 0=None, 1=Connecting, 2=Disconnecting

// bool g_bluetoothRadioOn = false;   // moved inside bluetooth namespace
// std::mutex g_bluetoothRadioMutex;  // moved inside bluetooth namespace

void RunOnUiThread(std::function<void()> work) {
    if (InterlockedCompareExchange(&g_shuttingDown, 0, 0) != 0) {
        return;
    }
    auto guarded = [work = std::move(work)]() {
        if (InterlockedCompareExchange(&g_shuttingDown, 0, 0) != 0) {
            return;
        }
        try {
            work();
        } catch (...) {
        }
    };

    if (g_uiDispatcherQueue) {
        g_uiDispatcherQueue.TryEnqueue(guarded);
        return;
    }

    // WindowsXamlManager normally provides a DispatcherQueue on the island
    // thread, but if it didn't, the XAML CoreDispatcher reaches the same thread.
    if (g_rootElement) {
        try {
            g_rootElement.Dispatcher().RunAsync(
                winrt::Windows::UI::Core::CoreDispatcherPriority::Normal, guarded);
        } catch (...) {
        }
    }
}

// Wi-Fi scans and Bluetooth inquiry block for seconds; running them on the UI
// thread would freeze the whole bar. Each worker owns its own apartment, and
// Wh_ModUninit waits for the count to drain.
std::atomic<int> g_backgroundJobs{0};
std::vector<HANDLE> g_workerThreads;
std::mutex g_workerThreadsMutex;

void RunInBackground(std::function<void()> work) {
    if (InterlockedCompareExchange(&g_shuttingDown, 0, 0) != 0) {
        return;
    }
    g_backgroundJobs.fetch_add(1);
    auto* workPtr = new std::function<void()>(std::move(work));
    HANDLE threadHandle = (HANDLE)_beginthreadex(nullptr, 0, [](void* param) -> unsigned int {
        auto* workPtr = static_cast<std::function<void()>*>(param);
        HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
        try {
            (*workPtr)();
        } catch (...) {
        }
        if (SUCCEEDED(hr)) {
            CoUninitialize();
        }
        g_backgroundJobs.fetch_sub(1);
        delete workPtr;
        return 0;
    }, workPtr, 0, nullptr);
    if (threadHandle) {
        std::lock_guard<std::mutex> lock(g_workerThreadsMutex);
        g_workerThreads.push_back(threadHandle);
    }
}

// Panels tear themselves down from inside their own click handlers, which would
// destroy the very button that is still dispatching. Rebuilding on the next
// dispatcher turn avoids that.
void RepopulateLater(const std::function<void()>& populate) {
    RunOnUiThread(populate);
}

wuxm::SolidColorBrush FlyoutBackgroundBrush() {
    // Flyouts are fully transparent – only the blur shows through.
    return wuxm::SolidColorBrush(wui::ColorHelper::FromArgb(0, 0, 0, 0));
}

// One presenter style for every flyout, so the panels share the bar's rounded
// look instead of the square system default. ShouldConstrainToRootBounds is
// turned off separately -- the XAML root is only bar-height, so without it a
// panel is clipped to nothing.
Style MakeFlyoutPresenterStyle(double width) {
    Style style(winrt::xaml_typename<wuxc::FlyoutPresenter>());
    auto setters = style.Setters();
    setters.Append(Setter(wuxc::Control::BackgroundProperty(),
                          winrt::box_value(FlyoutBackgroundBrush())));
    setters.Append(Setter(wuxc::Control::BorderBrushProperty(),
                          winrt::box_value(MakeBrush(0x30, 0xFF, 0xFF, 0xFF))));
    setters.Append(Setter(wuxc::Control::BorderThicknessProperty(),
                          winrt::box_value(Thickness{1, 1, 1, 1})));
    setters.Append(Setter(wuxc::Control::CornerRadiusProperty(),
                          winrt::box_value(MakeCorner(kFlyoutCorner))));
    setters.Append(
        Setter(wuxc::Control::PaddingProperty(), winrt::box_value(Thickness{12, 12, 12, 12})));
    setters.Append(Setter(FrameworkElement::MinWidthProperty(), winrt::box_value(width)));
    setters.Append(Setter(FrameworkElement::MaxWidthProperty(), winrt::box_value(width)));
    setters.Append(Setter(wuxc::ScrollViewer::HorizontalScrollBarVisibilityProperty(),
                          winrt::box_value(wuxc::ScrollBarVisibility::Disabled)));
    return style;
}

// The panel body is wrapped in a ScrollViewer so a long network or tray list
// stays inside a sane height instead of running off the bottom of the screen.
wuxc::Flyout MakeControlFlyout(PCWSTR name, wuxc::StackPanel& contentOut) {
    wuxc::StackPanel panel;
    panel.Name(name);
    panel.Spacing(2);
    contentOut = panel;
    g_detachedStyleRoots.push_back(panel);  // Register panel so tree targets can find it

    wuxc::ScrollViewer scroller;
    scroller.Content(panel);
    scroller.VerticalScrollBarVisibility(wuxc::ScrollBarVisibility::Auto);
    scroller.HorizontalScrollBarVisibility(wuxc::ScrollBarVisibility::Disabled);
    scroller.MaxHeight(560);

    wuxc::Flyout flyout;
    flyout.Content(scroller);
    flyout.Placement(wuxc::Primitives::FlyoutPlacementMode::Bottom);
    flyout.FlyoutPresenterStyle(MakeFlyoutPresenterStyle(kPanelWidth));
    // Without this the panel is clipped to the bar-height island. (Credit: the
    // user's fix.)
    flyout.ShouldConstrainToRootBounds(false);

    // When the flyout opens, register the topmost visual root (like FlyoutPresenter)
    // so targets like `FlyoutPresenter` or `FlyoutPresenter > Grid` also match.
    flyout.Opened([flyout](auto&&, auto&&) {
        try {
            if (auto content = flyout.Content().try_as<FrameworkElement>()) {
                // Walk up to the topmost element in the popup's visual tree.
                auto root = content;
                while (auto parent = wuxm::VisualTreeHelper::GetParent(root)) {
                    root = parent.try_as<FrameworkElement>();
                    if (!root) break;
                }
                if (root &&
                    std::find(g_detachedStyleRoots.begin(), g_detachedStyleRoots.end(), root) ==
                        g_detachedStyleRoots.end()) {
                    g_detachedStyleRoots.push_back(root);
                }
            }
            ApplyBlurToAllOpenPopups();
        } catch (...) {
        }
    });

    return flyout;
}

wuxc::TextBlock MakePanelTitle(std::wstring_view text) {
    auto title = MakeText(L"FlyoutTitle", text, 15, true);
    title.Margin(Thickness{4, 2, 4, 6});
    return title;
}

// A full-width row: [icon] [label (+ optional sublabel)] [trailing element].
// Every list entry in every panel is built from this so the columns line up
// across the Display, Sound, Wi-Fi, Bluetooth and tray panels.
wuxc::Grid MakeRowContent(FrameworkElement leading,
                          std::wstring_view primary,
                          std::wstring_view secondary,
                          FrameworkElement trailing) {
    wuxc::Grid grid;
    grid.ColumnSpacing(12);

    wuxc::ColumnDefinition leadingColumn;
    leadingColumn.Width(GridLength{0, GridUnitType::Auto});
    grid.ColumnDefinitions().Append(leadingColumn);

    wuxc::ColumnDefinition textColumn;
    textColumn.Width(GridLength{1, GridUnitType::Star});
    grid.ColumnDefinitions().Append(textColumn);

    wuxc::ColumnDefinition trailingColumn;
    trailingColumn.Width(GridLength{0, GridUnitType::Auto});
    grid.ColumnDefinitions().Append(trailingColumn);

    if (leading) {
        leading.VerticalAlignment(VerticalAlignment::Center);
        wuxc::Grid::SetColumn(leading, 0);
        grid.Children().Append(leading);
    }

    wuxc::StackPanel textStack;
    textStack.VerticalAlignment(VerticalAlignment::Center);
    textStack.Children().Append(MakeText(nullptr, primary, 13));
    if (!secondary.empty()) {
        auto sub = MakeText(nullptr, secondary, 11, false, 0.6);
        textStack.Children().Append(sub);
    }
    wuxc::Grid::SetColumn(textStack, 1);
    grid.Children().Append(textStack);

    if (trailing) {
        trailing.VerticalAlignment(VerticalAlignment::Center);
        wuxc::Grid::SetColumn(trailing, 2);
        grid.Children().Append(trailing);
    }
    return grid;
}

// Clickable list row. Same corner radius and padding everywhere, so Wi-Fi
// networks, Bluetooth devices, output devices and tray items are visually one
// family.
wuxc::Button MakeListRow(FrameworkElement leading,
                         std::wstring_view primary,
                         std::wstring_view secondary,
                         FrameworkElement trailing,
                         std::function<void()> onClick) {
    auto button = MakeGhostButton(L"FlyoutListRow", kRowCorner);
    button.HorizontalAlignment(HorizontalAlignment::Stretch);
    button.Padding(Thickness{10, 8, 10, 8});
    button.Content(MakeRowContent(leading, primary, secondary, trailing));
    if (onClick) {
        button.Click([onClick = std::move(onClick)](auto&&, auto&&) { onClick(); });
    }
    return button;
}

// The square accent tile used for Dark mode: filled when on,
// ghost when off, so state is readable without a separate label.
wuxc::Button MakeToggleTile(std::wstring_view label,
                            std::wstring_view iconFill,
                            std::wstring_view iconStroke,
                            bool isOn,
                            std::function<void()> onClick) {
    auto button = MakeGhostButton(L"QuickToggleTile", kTileCorner);
    button.HorizontalAlignment(HorizontalAlignment::Stretch);
    button.Padding(Thickness{12, 10, 12, 10});
    button.Background(isOn ? MakeBrush(0xFF, 0x4C, 0x8E, 0xE0) : MakeBrush(0x18, 0xFF, 0xFF, 0xFF));

    wuxc::StackPanel stack;
    stack.Orientation(wuxc::Orientation::Horizontal);
    stack.Spacing(10);
    if (auto icon = BuildVectorIcon(nullptr, iconFill, iconStroke, 24, 18, 1.7, L"#FFFFFF")) {
        icon.VerticalAlignment(VerticalAlignment::Center);
        stack.Children().Append(icon);
    }
    stack.Children().Append(MakeText(nullptr, label, 12));
    button.Content(stack);

    if (onClick) {
        button.Click([onClick = std::move(onClick)](auto&&, auto&&) { onClick(); });
    }
    return button;
}

wuxc::Grid MakeTileRow(const wuxc::Button& left, const wuxc::Button& right) {
    wuxc::Grid grid;
    grid.ColumnSpacing(8);
    for (int i = 0; i < 2; i++) {
        wuxc::ColumnDefinition column;
        column.Width(GridLength{1, GridUnitType::Star});
        grid.ColumnDefinitions().Append(column);
    }
    if (left) {
        wuxc::Grid::SetColumn(left, 0);
        grid.Children().Append(left);
    }
    if (right) {
        wuxc::Grid::SetColumn(right, 1);
        grid.Children().Append(right);
    }
    return grid;
}

// Slider row: icon on the left, slider filling the width, live percentage on
// the right. onChanged only fires for real user movement.
wuxc::Grid MakeSliderRow(FrameworkElement icon,
                         int value,
                         std::function<void(int)> onChanged,
                         wuxc::Slider* sliderOut = nullptr) {
    wuxc::Grid grid;
    grid.ColumnSpacing(10);

    wuxc::ColumnDefinition iconColumn;
    iconColumn.Width(GridLength{0, GridUnitType::Auto});
    grid.ColumnDefinitions().Append(iconColumn);

    wuxc::ColumnDefinition sliderColumn;
    sliderColumn.Width(GridLength{1, GridUnitType::Star});
    grid.ColumnDefinitions().Append(sliderColumn);

    wuxc::ColumnDefinition valueColumn;
    valueColumn.Width(GridLength{0, GridUnitType::Auto});
    grid.ColumnDefinitions().Append(valueColumn);

    if (icon) {
        icon.VerticalAlignment(VerticalAlignment::Center);
        wuxc::Grid::SetColumn(icon, 0);
        grid.Children().Append(icon);
    }

    auto readout = MakeText(nullptr, std::to_wstring(value), 12, false, 0.75);
    readout.MinWidth(30);
    readout.TextAlignment(TextAlignment::Right);

    wuxc::Slider slider;
    slider.Minimum(0);
    slider.Maximum(100);
    slider.Value(value);
    slider.StepFrequency(1);
    slider.VerticalAlignment(VerticalAlignment::Center);
    slider.Margin(Thickness{0, 0, 0, 0});
    // The default tooltip repeats what the readout already shows, and it draws
    // outside the island where it can't be clipped correctly.
    slider.ThumbToolTipValueConverter(nullptr);
    slider.ValueChanged([onChanged = std::move(onChanged), readout](auto&&, auto&& args) {
        int newValue = static_cast<int>(args.NewValue() + 0.5);
        readout.Text(winrt::hstring(std::to_wstring(newValue)));
        if (g_populatingPanel) {
            return;
        }
        if (onChanged) {
            onChanged(newValue);
        }
    });
    wuxc::Grid::SetColumn(slider, 1);
    grid.Children().Append(slider);

    wuxc::Grid::SetColumn(readout, 2);
    grid.Children().Append(readout);

    if (sliderOut) {
        *sliderOut = slider;
    }
    return grid;
}

// Header + collapsible body, hand-built: the system XAML used by islands has no
// Expander control.
struct Expander {
    wuxc::StackPanel root{nullptr};
    wuxc::StackPanel body{nullptr};
};

Expander MakeExpander(std::wstring_view title,
                      std::wstring_view subtitle,
                      bool* expandedFlag,
                      const std::function<void()>& onToggled) {
    Expander expander;

    wuxc::StackPanel root;
    root.Spacing(2);

    auto chevron = BuildVectorIcon(nullptr, L"",
                                   *expandedFlag ? icons::kChevronUp : icons::kChevronDown, 24,
                                   14, 1.8);

    auto header = MakeGhostButton(L"ExpanderHeader", kRowCorner);
    header.HorizontalAlignment(HorizontalAlignment::Stretch);
    header.Padding(Thickness{10, 8, 10, 8});
    header.Content(MakeRowContent(nullptr, title, subtitle, chevron));
    header.Click([expandedFlag, onToggled](auto&&, auto&&) {
        *expandedFlag = !*expandedFlag;
        if (onToggled) {
            onToggled();
        }
    });
    root.Children().Append(header);

    wuxc::StackPanel body;
    body.Spacing(2);
    body.Margin(Thickness{8, 2, 0, 2});
    body.Visibility(*expandedFlag ? Visibility::Visible : Visibility::Collapsed);
    root.Children().Append(body);

    expander.root = root;
    expander.body = body;
    return expander;
}

// Right-aligned On/Off switch for the Wi-Fi and Bluetooth headers. The user
// asked for these in the corner rather than centred, so the header is a grid
// with the title on the left and this pushed to the far right.
wuxc::ToggleSwitch MakeHeaderToggle(bool isOn, std::function<void(bool)> onToggled) {
    wuxc::ToggleSwitch toggle;
    toggle.OnContent(winrt::box_value(L""));
    toggle.OffContent(winrt::box_value(L""));
    toggle.MinWidth(0);
    toggle.HorizontalAlignment(HorizontalAlignment::Right);
    toggle.VerticalAlignment(VerticalAlignment::Center);
    toggle.IsOn(isOn);
    toggle.Toggled([onToggled = std::move(onToggled)](auto&& sender, auto&&) {
        if (g_populatingPanel) {
            return;
        }
        if (onToggled) {
            onToggled(sender.template as<wuxc::ToggleSwitch>().IsOn());
        }
    });
    return toggle;
}

wuxc::Grid MakePanelHeader(std::wstring_view title, FrameworkElement trailing) {
    wuxc::Grid grid;
    grid.Margin(Thickness{4, 2, 0, 6});

    wuxc::ColumnDefinition titleColumn;
    titleColumn.Width(GridLength{1, GridUnitType::Star});
    grid.ColumnDefinitions().Append(titleColumn);

    wuxc::ColumnDefinition trailingColumn;
    trailingColumn.Width(GridLength{0, GridUnitType::Auto});
    grid.ColumnDefinitions().Append(trailingColumn);

    auto titleBlock = MakeText(L"FlyoutTitle", title, 15, true);
    wuxc::Grid::SetColumn(titleBlock, 0);
    grid.Children().Append(titleBlock);

    if (trailing) {
        trailing.VerticalAlignment(VerticalAlignment::Center);
        wuxc::Grid::SetColumn(trailing, 1);
        grid.Children().Append(trailing);
    }
    return grid;
}

wuxc::TextBlock MakeStatusText(std::wstring_view text) {
    auto block = MakeText(nullptr, text, 12, false, 0.6);
    block.Margin(Thickness{10, 8, 10, 8});
    block.TextWrapping(TextWrapping::Wrap);
    return block;
}

// A plain "open the real Settings page" footer link. Only used where Windows
// genuinely owns the setting; the panels no longer punt the actual controls to
// Settings.
wuxc::Button MakeSettingsLink(std::wstring_view label, PCWSTR uri) {
    auto button = MakeGhostButton(L"FlyoutFooterLink", kRowCorner);
    button.HorizontalAlignment(HorizontalAlignment::Stretch);
    button.Padding(Thickness{10, 7, 10, 7});
    auto text = MakeText(nullptr, label, 12, false, 0.75);
    button.Content(text);
    std::wstring target = uri;
    button.Click([target](auto&&, auto&&) {
        ShellExecute(nullptr, L"open", target.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
    });
    return button;
}

void HideAllFlyouts() {
    for (auto& flyout : {g_displayFlyout, g_soundFlyout, g_wifiFlyout, g_bluetoothFlyout,
                         g_trayFlyout}) {
        if (flyout) {
            flyout.Hide();
        }
    }
}

// ============================================================================
// Display panel
// ============================================================================

void PopulateDisplayPanel() {
    if (!g_displayPanel) {
        return;
    }

    g_populatingPanel = true;
    struct Guard {
        ~Guard() { g_populatingPanel = false; }
    } guard;

    auto children = g_displayPanel.Children();
    children.Clear();

    children.Append(MakePanelTitle(L"Display"));

    if (brightness::Available()) {
        auto icon = BuildVectorIcon(nullptr, L"", icons::kBrightnessStroke, 24, 18, 1.6);
        children.Append(MakeSliderRow(icon, brightness::Get(),
                                      [](int value) { RunInBackground([value] { brightness::Set(value); }); }));
    } else {
        children.Append(MakeStatusText(L"Brightness control isn't available on this display."));
    }

    children.Append(MakeDivider());

    bool darkMode = IsAppsDarkMode();

    auto darkTile = MakeGhostButton(L"QuickToggleTile", kTileCorner);
    darkTile.HorizontalAlignment(HorizontalAlignment::Stretch);
    darkTile.Padding(Thickness{8, 6, 8, 6});
    darkTile.Background(darkMode ? MakeBrush(0xFF, GetSystemAccentColor().R, GetSystemAccentColor().G, GetSystemAccentColor().B) : MakeBrush(0x18, 0xFF, 0xFF, 0xFF));

    wuxc::StackPanel darkStack;
    darkStack.Orientation(wuxc::Orientation::Horizontal);
    darkStack.Spacing(8);
    if (auto icon = BuildVectorIcon(nullptr, icons::kMoonFill, L"", 24, 16, 1.7, L"#FFFFFF")) {
        icon.VerticalAlignment(VerticalAlignment::Center);
        darkStack.Children().Append(icon);
    }
    darkStack.Children().Append(MakeText(nullptr, L"Dark mode", 12));
    darkTile.Content(darkStack);
    darkTile.Click([darkMode](auto&&, auto&&) {
        SetAppsDarkMode(!darkMode);
        RepopulateLater(PopulateDisplayPanel);
    });

    // Put them side-by-side (2 columns)
    children.Append(darkTile);

    children.Append(MakeDivider());
    children.Append(MakeSettingsLink(L"Display settings", L"ms-settings:display"));
}

// ============================================================================
// Sound panel
// ============================================================================

FrameworkElement BuildSpeakerIcon(double size, int volume, bool muted) {
    std::wstring stroke;
    if (muted || volume == 0) {
        stroke = icons::kSpeakerMuted;
    } else if (volume < 55) {
        // One arc below about half volume, two above -- the same visual cue the
        // system tray icon uses.
        stroke = L"M15.4 9.4 A3.6 3.6 0 0 1 15.4 14.6";
    } else {
        stroke = icons::kSpeakerWaves;
    }
    return BuildVectorIcon(nullptr, icons::kSpeakerFill, stroke, 24, size, 1.6);
}

void RefreshSoundButtonIcon() {
    if (!g_soundButton) {
        return;
    }
    if (auto icon = BuildSpeakerIcon(18, audio::GetMasterVolume(), audio::GetMasterMute())) {
        g_soundButton.Content(icon);
    }
}

// ----------------------------------------------------------------------------
// Now-playing card
//
// Every call into the media session manager is asynchronous and has to be
// awaited, which cannot be done from the UI thread without deadlocking the
// apartment. So the state is fetched on a worker and pushed back through the
// dispatcher, and the transport buttons do the same in reverse.
// ----------------------------------------------------------------------------

[[clang::no_destroy]] wuxc::StackPanel g_mediaContainer{nullptr};

#if TOPBAR_HAS_MEDIA_CONTROL

namespace media {

using namespace winrt::Windows::Media::Control;

struct Snapshot {
    bool valid = false;
    std::wstring title;
    std::wstring artist;
    bool playing = false;
    bool canGoPrevious = false;
    bool canGoNext = false;
};

Snapshot Read() {
    Snapshot snapshot;
    try {
        auto manager =
            GlobalSystemMediaTransportControlsSessionManager::RequestAsync().get();
        if (!manager) {
            return snapshot;
        }
        auto session = manager.GetCurrentSession();
        if (!session) {
            return snapshot;
        }

        auto properties = session.TryGetMediaPropertiesAsync().get();
        if (properties) {
            snapshot.title = properties.Title();
            snapshot.artist = properties.Artist();
        }

        auto info = session.GetPlaybackInfo();
        if (info) {
            snapshot.playing = info.PlaybackStatus() ==
                               GlobalSystemMediaTransportControlsSessionPlaybackStatus::Playing;
            if (auto controls = info.Controls()) {
                snapshot.canGoPrevious = controls.IsPreviousEnabled();
                snapshot.canGoNext = controls.IsNextEnabled();
            }
        }

        snapshot.valid = !snapshot.title.empty();
    } catch (...) {
    }
    return snapshot;
}

enum class Command { PlayPause, Previous, Next };

void Send(Command command) {
    RunInBackground([command] {
        try {
            auto manager =
                GlobalSystemMediaTransportControlsSessionManager::RequestAsync().get();
            if (!manager) {
                return;
            }
            auto session = manager.GetCurrentSession();
            if (!session) {
                return;
            }
            switch (command) {
                case Command::PlayPause:
                    session.TryTogglePlayPauseAsync().get();
                    break;
                case Command::Previous:
                    session.TrySkipPreviousAsync().get();
                    break;
                case Command::Next:
                    session.TrySkipNextAsync().get();
                    break;
            }
        } catch (...) {
        }
    });
}

}  // namespace media

wuxc::Button MakeTransportButton(std::wstring_view glyph, bool enabled,
                                 std::function<void()> onClick) {
    auto button = MakeGhostButton(L"MediaTransportButton", kTileCorner);
    button.Padding(Thickness{8, 6, 8, 6});
    button.IsEnabled(enabled);
    button.Content(BuildVectorIcon(nullptr, glyph, L"", 24, 16));
    if (onClick) {
        button.Click([onClick = std::move(onClick)](auto&&, auto&&) { onClick(); });
    }
    return button;
}

void FillMediaCard(const media::Snapshot& snapshot) {
    if (!g_mediaContainer) {
        return;
    }
    auto children = g_mediaContainer.Children();
    children.Clear();

    if (!snapshot.valid) {
        g_mediaContainer.Visibility(Visibility::Collapsed);
        return;
    }
    g_mediaContainer.Visibility(Visibility::Visible);

    wuxc::StackPanel transport;
    transport.Orientation(wuxc::Orientation::Horizontal);
    transport.Spacing(2);
    transport.Children().Append(MakeTransportButton(
        icons::kPrevFill, snapshot.canGoPrevious,
        [] { media::Send(media::Command::Previous); }));
    transport.Children().Append(MakeTransportButton(
        snapshot.playing ? icons::kPauseFill : icons::kPlayFill, true, [] {
            media::Send(media::Command::PlayPause);
            // The status flips a moment after the command lands.
            RunInBackground([] {
                Sleep(400);
                auto refreshed = media::Read();
                RunOnUiThread([refreshed] { FillMediaCard(refreshed); });
            });
        }));
    transport.Children().Append(MakeTransportButton(
        icons::kNextFill, snapshot.canGoNext, [] { media::Send(media::Command::Next); }));

    auto icon = BuildVectorIcon(nullptr, icons::kHeadphoneFill, icons::kHeadphoneStroke, 24,
                                18, 1.6);
    auto row = MakeRowContent(icon, snapshot.title, snapshot.artist, transport);
    row.Margin(Thickness{10, 4, 6, 4});
    children.Append(row);
    
}

void RefreshMediaCard() {
    RunInBackground([] {
        auto snapshot = media::Read();
        RunOnUiThread([snapshot] { FillMediaCard(snapshot); });
    });
}

#else

void RefreshMediaCard() {}

#endif  // TOPBAR_HAS_MEDIA_CONTROL

void PopulateSoundPanel() {
    if (!g_soundPanel) {
        return;
    }

    g_populatingPanel = true;
    struct Guard {
        ~Guard() { g_populatingPanel = false; }
    } guard;

    auto children = g_soundPanel.Children();
    children.Clear();

    children.Append(MakePanelTitle(L"Sound"));

    // Starts collapsed and empty; the worker fills it in and reveals it only if
    // something is actually playing.
    g_mediaContainer = wuxc::StackPanel();
    g_mediaContainer.Name(L"MediaCard");
    g_mediaContainer.Visibility(Visibility::Collapsed);
    children.Append(g_mediaContainer);
    RefreshMediaCard();

    int masterVolume = audio::GetMasterVolume();
    bool masterMuted = audio::GetMasterMute();

    // The speaker glyph doubles as the mute button.
    auto muteButton = MakeGhostButton(L"SoundMuteButton", kTileCorner);
    muteButton.Padding(Thickness{6, 6, 6, 6});
    muteButton.Content(BuildSpeakerIcon(18, masterVolume, masterMuted));
    muteButton.Click([masterMuted](auto&&, auto&&) {
        if (auto volume = audio::EndpointVolume()) {
            volume->SetMute(masterMuted ? FALSE : TRUE, nullptr);
        }
        RefreshSoundButtonIcon();
        RepopulateLater(PopulateSoundPanel);
    });

    children.Append(MakeSliderRow(muteButton, masterVolume, [](int value) {
        RunInBackground([value] { audio::SetMasterVolume(value); });
        RunOnUiThread([] { RefreshSoundButtonIcon(); });
    }));

    children.Append(MakeDivider());

    // Output devices: an expander showing the current default, listing every
    // active endpoint when opened. Tapping one switches to it -- the old build
    // sent the user to the Settings app instead.
    {
        auto devices = audio::EnumerateOutputDevices();
        std::wstring currentName = L"No output device";
        for (const auto& device : devices) {
            if (device.isDefault) {
                currentName = device.name;
                break;
            }
        }

        auto expander = MakeExpander(L"Output device", currentName, &g_outputDevicesExpanded,
                                     [] { RepopulateLater(PopulateSoundPanel); });

        if (devices.empty()) {
            expander.body.Children().Append(MakeStatusText(L"No output devices found."));
        }
        for (const auto& device : devices) {
            FrameworkElement check{nullptr};
            if (device.isDefault) {
                check = BuildVectorIcon(nullptr, L"", icons::kCheckStroke, 24, 15, 2.0);
            }
            auto icon = BuildVectorIcon(nullptr, icons::kHeadphoneFill,
                                        icons::kHeadphoneStroke, 24, 17, 1.6);
            std::wstring deviceId = device.id;
            expander.body.Children().Append(
                MakeListRow(icon, device.name, L"", check, [deviceId] {
                    if (!audio::SetDefaultOutputDevice(deviceId)) {
                        Wh_Log(L"Failed to set the default output device");
                    }
                    RefreshSoundButtonIcon();
                    RepopulateLater(PopulateSoundPanel);
                }));
        }
        children.Append(expander.root);
    }

    // Per-app mixer, collapsed into one expandable section as requested.
    // (Spatial Audio has been dropped entirely.)
    {
        auto sessions = audio::EnumerateSessions();
        std::wstring subtitle =
            sessions.empty()
                ? std::wstring(L"Nothing is playing")
                : std::to_wstring(sessions.size()) +
                      (sessions.size() == 1 ? L" app" : L" apps");

        auto expander = MakeExpander(L"App volume mixer", subtitle, &g_appMixerExpanded,
                                     [] { RepopulateLater(PopulateSoundPanel); });

        if (sessions.empty()) {
            expander.body.Children().Append(
                MakeStatusText(L"No apps are currently playing audio."));
        }
        for (const auto& session : sessions) {
            wuxc::StackPanel row;
            row.Spacing(0);
            row.Margin(Thickness{10, 4, 10, 4});
            row.Children().Append(MakeText(nullptr, session.name, 12, false, 0.85));

            auto icon = BuildVectorIcon(
                nullptr, session.isSystemSounds ? icons::kSpeakerFill : icons::kAppFill, L"",
                24, 15, 1.5);

            // The control interface is captured so the slider writes straight
            // to that session without re-enumerating on every drag.
            auto control = session.control;
            row.Children().Append(MakeSliderRow(icon, session.volume, [control](int value) {
                if (control) {
                    control->SetMasterVolume(std::clamp(value, 0, 100) / 100.0f, nullptr);
                    if (value > 0) {
                        control->SetMute(FALSE, nullptr);
                    }
                }
            }));
            expander.body.Children().Append(row);
        }
        children.Append(expander.root);
    }

    children.Append(MakeDivider());
    children.Append(MakeSettingsLink(L"Sound settings", L"ms-settings:sound"));
}

// ============================================================================
// Wi-Fi panel
// ============================================================================

// Four-bar signal glyph, drawn by dropping arcs off the top of the full icon.
FrameworkElement BuildWifiIcon(double size, int signal, bool connected) {
    std::wstring stroke;
    if (signal >= 75) {
        stroke = icons::kWifiStroke;
    } else if (signal >= 50) {
        stroke = L"M5.8 12.1 A8.9 8.9 0 0 1 18.2 12.1 M9 15.4 A4.4 4.4 0 0 1 15 15.4";
    } else if (signal >= 25) {
        stroke = L"M9 15.4 A4.4 4.4 0 0 1 15 15.4";
    }
    return BuildVectorIcon(nullptr, icons::kWifiDot, stroke, 24, size, 1.7);
}

void RefreshWifiButtonIcon() {
    if (!g_wifiButton) {
        return;
    }
    auto status = wifi::GetStatus();
    if (auto icon = BuildWifiIcon(18, status.signal, status.connected && status.radioOn)) {
        g_wifiButton.Content(icon);
    }
}

// Kicks off a scan and re-reads the list a moment later. WlanScan is
// asynchronous, so reading the list immediately would return the same stale
// results the user is complaining about.
void StartWifiScan() {
    if (g_wifiScanning) {
        return;
    }
    g_wifiScanning = true;
    RunInBackground([] {
        if (WaitForSingleObject(g_stopEvent, 0) == WAIT_OBJECT_0) return;
        wifi::RequestScan();
        // The driver reports results over the next few seconds; this is the
        // interval Windows' own flyout waits before redrawing.
        WaitForSingleObject(g_stopEvent, 4000);
        auto networks = wifi::EnumerateNetworks();
        RunOnUiThread([networks = std::move(networks)]() mutable {
            g_wifiNetworks = std::move(networks);
            g_wifiScanning = false;
            PopulateWifiPanel();
        });
    });
}

void ConnectToWifi(const wifi::Network& network, const std::wstring& password) {
    RunInBackground([network, password] {
        if (WaitForSingleObject(g_stopEvent, 0) == WAIT_OBJECT_0) return;
        bool ok = wifi::Connect(network, password);
        // Association takes a moment; re-read afterwards so the row shows the
        // real outcome rather than an optimistic "Connected".
        WaitForSingleObject(g_stopEvent, ok ? 2500 : 300);

        auto networks = wifi::EnumerateNetworks();
        bool connected = false;
        for (const auto& candidate : networks) {
            if (candidate.ssid == network.ssid && candidate.connected) {
                connected = true;
                break;
            }
        }

        if (!connected) {
            wifi::ForgetProfile(network.ssid);
        }

        RunOnUiThread([networks = std::move(networks), connected, network]() mutable {
            g_wifiNetworks = std::move(networks);
            g_wifiPasswordPrompt = false;
            g_wifiConnectingSSID.clear();
            if (!connected) {
                g_wifiPromptError = L"Couldn't connect to " + network.ssid + L".";
            }
            RefreshWifiButtonIcon();
            PopulateWifiPanel();
        });
    });
}

void BuildWifiPasswordView(const wf::Collections::IVector<UIElement>& children) {
    children.Append(MakePanelTitle(L"Connect to " + g_wifiPromptNetwork.ssid));

    if (!g_wifiPromptError.empty()) {
        auto error = MakeText(nullptr, g_wifiPromptError, 12, false, 0.9);
        error.Foreground(MakeBrush(0xFF, 0xFF, 0x8A, 0x80));
        error.TextWrapping(TextWrapping::Wrap);
        error.Margin(Thickness{10, 0, 10, 6});
        children.Append(error);
    }

    children.Append(MakeStatusText(L"Enter the network security key."));

    wuxc::PasswordBox passwordBox;
    passwordBox.Name(L"WifiPasswordBox");
    passwordBox.PlaceholderText(L"Password");
    passwordBox.Margin(Thickness{10, 0, 10, 8});
    passwordBox.CornerRadius(MakeCorner(kRowCorner));
    passwordBox.Background(MakeBrush(0xFF, 0x30, 0x30, 0x30)); // Dark background
    passwordBox.Foreground(MakeBrush(0xFF, 0xFF, 0xFF, 0xFF)); // White text
    children.Append(passwordBox);

    auto submit = [passwordBox] {
        std::wstring password{passwordBox.Password()};
        g_wifiPromptError.clear();
        ConnectToWifi(g_wifiPromptNetwork, password);
    };

    // Enter should connect, same as the system flyout.
    passwordBox.KeyDown([submit](auto&&, Input::KeyRoutedEventArgs const& args) {
        if (args.Key() == winrt::Windows::System::VirtualKey::Enter) {
            args.Handled(true);
            submit();
        }
    });

    wuxc::Grid buttons;
    buttons.ColumnSpacing(8);
    buttons.Margin(Thickness{10, 0, 10, 4});
    for (int i = 0; i < 2; i++) {
        wuxc::ColumnDefinition column;
        column.Width(GridLength{1, GridUnitType::Star});
        buttons.ColumnDefinitions().Append(column);
    }

    auto connect = MakeGhostButton(L"WifiConnectButton", kRowCorner);
    connect.Background(MakeBrush(0xFF, 0x4C, 0x8E, 0xE0));
    connect.Padding(Thickness{12, 8, 12, 8});
    connect.HorizontalAlignment(HorizontalAlignment::Stretch);
    connect.HorizontalContentAlignment(HorizontalAlignment::Center);
    connect.Content(MakeText(nullptr, L"Connect", 12));
    connect.Click([submit](auto&&, auto&&) { submit(); });
    wuxc::Grid::SetColumn(connect, 0);
    buttons.Children().Append(connect);

    auto cancel = MakeGhostButton(L"WifiCancelButton", kRowCorner);
    cancel.Background(MakeBrush(0x18, 0xFF, 0xFF, 0xFF));
    cancel.Padding(Thickness{12, 8, 12, 8});
    cancel.HorizontalAlignment(HorizontalAlignment::Stretch);
    cancel.HorizontalContentAlignment(HorizontalAlignment::Center);
    cancel.Content(MakeText(nullptr, L"Cancel", 12));
    cancel.Click([](auto&&, auto&&) {
        g_wifiPasswordPrompt = false;
        g_wifiPromptError.clear();
        RepopulateLater(PopulateWifiPanel);
    });
    wuxc::Grid::SetColumn(cancel, 1);
    buttons.Children().Append(cancel);

    children.Append(buttons);

    // Focus can only be taken once the box is actually in the tree.
    passwordBox.Loaded([passwordBox](auto&&, auto&&) {
        passwordBox.Focus(FocusState::Programmatic);
    });
}

void PopulateWifiPanel() {
    if (!g_wifiPanel) {
        return;
    }

    g_populatingPanel = true;
    struct Guard {
        ~Guard() { g_populatingPanel = false; }
    } guard;

    auto children = g_wifiPanel.Children();
    children.Clear();

    if (g_wifiPasswordPrompt) {
        BuildWifiPasswordView(children);
        return;
    }

    auto status = wifi::GetStatus();

    // 2-Column Header: Left (Title + Spinner) | Right (Toggle)
    wuxc::Grid headerGrid;
    headerGrid.Margin(Thickness{4, 2, 0, 6});
 // Forces the right column to be 12px from the edge
    headerGrid.HorizontalAlignment(HorizontalAlignment::Stretch);

    // Column 0: Title + Spinner
    wuxc::ColumnDefinition leftColumn;
    leftColumn.Width(GridLength{1, GridUnitType::Star});
    headerGrid.ColumnDefinitions().Append(leftColumn);
    

    // Column 1: Toggle (auto width, pushed far right)
    wuxc::ColumnDefinition rightColumn;
    rightColumn.Width(GridLength{0, GridUnitType::Auto});
    headerGrid.ColumnDefinitions().Append(rightColumn);

    // Left stack: title + spinner
    wuxc::StackPanel leftStack;
    leftStack.Orientation(wuxc::Orientation::Horizontal);
    leftStack.Spacing(8);
    leftStack.VerticalAlignment(VerticalAlignment::Center);

    auto titleBlock = MakeText(L"FlyoutTitle", L"Wi-Fi", 15, true);
    leftStack.Children().Append(titleBlock);

    wuxc::ProgressRing wifiProgress;
    wifiProgress.Name(L"WifiProgressRing");
    wifiProgress.IsActive(true);
    wifiProgress.Width(14);
    wifiProgress.Height(14);
    wifiProgress.VerticalAlignment(VerticalAlignment::Center);
    wifiProgress.Foreground(MakeBrush(0xFF, GetSystemAccentColor().R, GetSystemAccentColor().G, GetSystemAccentColor().B));
    wifiProgress.Opacity(g_wifiScanning ? 1.0 : 0.3);
    leftStack.Children().Append(wifiProgress);

    wuxc::Grid::SetColumn(leftStack, 0);
    headerGrid.Children().Append(leftStack);

    // Toggle (hard forced right)
    wuxc::ToggleSwitch toggle;
    toggle.Name(L"WifiHeaderToggle");
    g_namedElements.insert_or_assign(L"WifiHeaderToggle", toggle);
    toggle.OnContent(winrt::box_value(L""));
    toggle.OffContent(winrt::box_value(L""));
    toggle.MinWidth(50);
    toggle.Width(50);
    toggle.HorizontalAlignment(HorizontalAlignment::Right);
    toggle.HorizontalContentAlignment(HorizontalAlignment::Right);
    toggle.VerticalAlignment(VerticalAlignment::Center);
    toggle.IsOn(status.radioOn);
    toggle.Toggled([](auto&& sender, auto&&) {
        if (g_populatingPanel) return;
        bool on = sender.template as<wuxc::ToggleSwitch>().IsOn();
        RunInBackground([on] {
            wifi::SetRadio(on);
            WaitForSingleObject(g_stopEvent, 600);
            auto networks = on ? wifi::EnumerateNetworks() : std::vector<wifi::Network>{};
            RunOnUiThread([networks = std::move(networks)]() mutable {
                g_wifiNetworks = std::move(networks);
                RefreshWifiButtonIcon();
                PopulateWifiPanel();
            });
        });
    });

    // WRAPPER: The Margin MUST go on a container, not the ToggleSwitch itself!
    wuxc::Border toggleContainer;
    toggleContainer.HorizontalAlignment(HorizontalAlignment::Center);
    toggleContainer.Margin(Thickness{12, 0, 24, 0}); // Push this 12px away from the right edge
    toggleContainer.Child(toggle);
    
    wuxc::Grid::SetColumn(toggleContainer, 1);
    headerGrid.Children().Append(toggleContainer);

    children.Append(headerGrid);

    if (!status.available) {
        children.Append(MakeStatusText(L"No Wi-Fi adapter was found on this PC."));
        return;
    }
    if (!status.radioOn) {
        children.Append(MakeStatusText(L"Wi-Fi is off."));
        return;
    }

    if (!g_wifiPromptError.empty()) {
        auto error = MakeText(nullptr, g_wifiPromptError, 12, false, 0.9);
        error.Foreground(MakeBrush(0xFF, 0xFF, 0x8A, 0x80));
        error.TextWrapping(TextWrapping::Wrap);
        error.Margin(Thickness{10, 0, 10, 4});
        children.Append(error);
    }

    if (g_wifiNetworks.empty()) {
        g_wifiNetworks = wifi::EnumerateNetworks();
    }

    if (g_wifiNetworks.empty()) {
        children.Append(MakeStatusText(g_wifiScanning ? L"Scanning for networks…"
                                                      : L"No networks found."));
    }

    for (const auto& network : g_wifiNetworks) {
        auto icon = BuildWifiIcon(18, network.signal, true);
        std::wstring subtitle;
        if (network.ssid == g_wifiConnectingSSID) {
            subtitle = L"Connecting...";
        } else if (network.connected) {
            subtitle = L"Connected";
        } else if (network.hasProfile) {
            subtitle = network.secured ? L"Saved, secured" : L"Saved";
        } else if (network.secured) {
            subtitle = L"Secured";
        } else {
            subtitle = L"Open";
        }

        FrameworkElement trailing{nullptr};
        if (network.ssid == g_wifiConnectingSSID) {
            trailing = BuildVectorIcon(nullptr, L"", icons::kBluetoothStroke, 24, 15, 1.7); // use a generic loading dot? or just no trailing
        } else if (network.connected) {
            trailing = BuildVectorIcon(nullptr, L"", icons::kCheckStroke, 24, 15, 2.0);
        } else if (network.secured) {
            trailing = BuildVectorIcon(nullptr, icons::kLockFill, icons::kLockShackle, 24, 14, 1.5);
        }

        wifi::Network captured = network;
        children.Append(MakeListRow(icon, network.ssid, subtitle, trailing, [captured] {
            g_wifiPromptError.clear();
            if (captured.connected) {
                RunInBackground([] {
                    wifi::Disconnect();
                    WaitForSingleObject(g_stopEvent, 800);
                    auto networks = wifi::EnumerateNetworks();
                    RunOnUiThread([networks = std::move(networks)]() mutable {
                        g_wifiNetworks = std::move(networks);
                        RefreshWifiButtonIcon();
                        PopulateWifiPanel();
                    });
                });
                return;
            }
            if (captured.secured && !captured.hasProfile) {
                g_wifiPasswordPrompt = true;
                g_wifiPromptNetwork = captured;
                RepopulateLater(PopulateWifiPanel);
                return;
            }
            
            // Set connecting status
            g_wifiConnectingSSID = captured.ssid;
            RepopulateLater(PopulateWifiPanel); // Show "Connecting..."
            
            ConnectToWifi(captured, L"");
        }));
    }

    children.Append(MakeDivider());
    children.Append(MakeSettingsLink(L"Network settings", L"ms-settings:network-wifi"));
    // Apply any styles for dynamically created elements (like the Wi-Fi toggle).
    ApplyAllControlStyles();
}

// ============================================================================
// Bluetooth panel
// ============================================================================

void RefreshBluetoothButtonIcon() {
    if (!g_bluetoothButton) {
        return;
    }
    if (auto icon = BuildVectorIcon(nullptr, L"", icons::kBluetoothStroke, 24, 18, 1.8)) {
        icon.Opacity(bluetooth::IsRadioOn() ? 1.0 : 0.4);
        g_bluetoothButton.Content(icon);
    }
}

// Paired devices come back fast; unpaired ones need an inquiry that blocks for
// seconds, which is why "no new devices at the bottom" was the old behaviour --
// nothing ever asked for them.
void StartBluetoothScan(bool includeUnpaired) {
    if (g_bluetoothScanning) {
        return;
    }
    g_bluetoothScanning = true;
    RunInBackground([includeUnpaired] {
        if (WaitForSingleObject(g_stopEvent, 0) == WAIT_OBJECT_0) return;
        auto devices = bluetooth::Enumerate(includeUnpaired);
        RunOnUiThread([devices = std::move(devices)]() mutable {
            g_bluetoothDevices = std::move(devices);
            g_bluetoothScanning = false;
            PopulateBluetoothPanel();
        });
    });
}

void RefreshBluetoothRadioState() {
    RunInBackground([] {
        bool on = false;
#if TOPBAR_HAS_RADIOS
        try {
            using namespace winrt::Windows::Devices::Radios;
            auto access = Radio::RequestAccessAsync().get();
            if (access == RadioAccessStatus::Allowed) {
                auto radios = Radio::GetRadiosAsync().get();
                for (auto&& radio : radios) {
                    if (radio.Kind() == RadioKind::Bluetooth) {
                        on = (radio.State() == RadioState::On);
                        break;
                    }
                }
            }
        } catch (...) {}
#endif
        {
            std::lock_guard<std::mutex> lock(bluetooth::g_bluetoothRadioMutex);
            bluetooth::g_bluetoothRadioOn = on;
        }
        RunOnUiThread([] {
            RefreshBluetoothButtonIcon();
            if (g_bluetoothFlyout && g_bluetoothFlyout.IsOpen()) {
                PopulateBluetoothPanel();
            }
        });
    });
}

void PopulateBluetoothPanel() {
    if (!g_bluetoothPanel) {
        return;
    }

    g_populatingPanel = true;
    struct Guard {
        ~Guard() { g_populatingPanel = false; }
    } guard;

    auto children = g_bluetoothPanel.Children();
    children.Clear();

    bool available = bluetooth::Available();
    bool radioOn = available && bluetooth::IsRadioOn();

    // 2-Column Header: Left (Title + Spinner) | Right (Toggle)
    wuxc::Grid headerGrid;
    headerGrid.Margin(Thickness{4, 2, 0, 6});
    headerGrid.HorizontalAlignment(HorizontalAlignment::Stretch);

    // Column 0: Title + Spinner
    wuxc::ColumnDefinition leftColumn;
    leftColumn.Width(GridLength{1, GridUnitType::Star});
    headerGrid.ColumnDefinitions().Append(leftColumn);

    // Column 1: Toggle (auto width, pushed far right)
    wuxc::ColumnDefinition rightColumn;
    rightColumn.Width(GridLength{0, GridUnitType::Auto});
    headerGrid.ColumnDefinitions().Append(rightColumn);

    // Left stack: title + spinner
    wuxc::StackPanel leftStack;
    leftStack.Orientation(wuxc::Orientation::Horizontal);
    leftStack.Spacing(8);
    leftStack.VerticalAlignment(VerticalAlignment::Center);

    auto titleBlock = MakeText(L"FlyoutTitle", L"Bluetooth", 15, true);
    leftStack.Children().Append(titleBlock);

    wuxc::ProgressRing btProgress;
    btProgress.Name(L"BluetoothProgressRing");
    btProgress.IsActive(true);
    btProgress.Width(14);
    btProgress.Height(14);
    btProgress.VerticalAlignment(VerticalAlignment::Center);
    btProgress.Foreground(MakeBrush(0xFF, GetSystemAccentColor().R, GetSystemAccentColor().G, GetSystemAccentColor().B));
    btProgress.Opacity(g_bluetoothScanning ? 1.0 : 0.3);
    leftStack.Children().Append(btProgress);

    wuxc::Grid::SetColumn(leftStack, 0);
    headerGrid.Children().Append(leftStack);

    // Toggle (hard forced right)
    wuxc::ToggleSwitch toggle;
    toggle.Name(L"BluetoothHeaderToggle");
    g_namedElements.insert_or_assign(L"BluetoothHeaderToggle", toggle);
    toggle.OnContent(winrt::box_value(L""));
    toggle.OffContent(winrt::box_value(L""));
    toggle.MinWidth(50);
    toggle.Width(50);
    toggle.HorizontalAlignment(HorizontalAlignment::Right);
    toggle.HorizontalContentAlignment(HorizontalAlignment::Right);
    toggle.VerticalAlignment(VerticalAlignment::Center);
    toggle.Margin(Thickness{0, 0, 4, 0});
    toggle.IsOn(radioOn);
    toggle.IsEnabled(true);
    toggle.Toggled([](auto&& sender, auto&&) {
        if (g_populatingPanel) return;
        bool on = sender.template as<wuxc::ToggleSwitch>().IsOn();
        RunInBackground([on] {
            if (WaitForSingleObject(g_stopEvent, 0) == WAIT_OBJECT_0) return;
            bluetooth::SetRadio(on);
            WaitForSingleObject(g_stopEvent, 600);
            auto devices = on ? bluetooth::Enumerate(false)
                              : std::vector<bluetooth::Device>{};
            RunOnUiThread([devices = std::move(devices)]() mutable {
                g_bluetoothDevices = std::move(devices);
                RefreshBluetoothButtonIcon();
                PopulateBluetoothPanel();
                RefreshBluetoothRadioState(); // update cached state after change
            });
        });
    });
    wuxc::Grid::SetColumn(toggle, 1);
    headerGrid.Children().Append(toggle);

    children.Append(headerGrid);

    if (!available) {
        children.Append(MakeStatusText(L"No Bluetooth radio was found on this PC."));
        return;
    }
    if (!radioOn) {
        children.Append(MakeStatusText(L"Bluetooth is off."));
        return;
    }

    if (g_bluetoothDevices.empty()) {
        g_bluetoothDevices = bluetooth::Enumerate(false);
    }

    if (g_bluetoothDevices.empty()) {
        children.Append(MakeStatusText(L"No paired devices."));
    }

    for (const auto& device : g_bluetoothDevices) {
        auto icon = BuildVectorIcon(nullptr, L"", icons::kBluetoothStroke, 24, 17, 1.7);
        if (icon && !device.connected) {
            icon.Opacity(0.55);
        }

        std::wstring subtitle;
        // Check connecting status (State 1=Connecting, 2=Disconnecting)
        if (device.address.ullLong == g_bluetoothConnectingAddress && g_bluetoothConnectingState == 1) {
            subtitle = L"Connecting...";
        } else if (device.address.ullLong == g_bluetoothConnectingAddress && g_bluetoothConnectingState == 2) {
            subtitle = L"Disconnecting...";
        } else if (device.connected) {
            subtitle = L"Connected";
        } else if (device.paired) {
            subtitle = L"Paired";
        } else {
            subtitle = L"Available";
        }
        // Append battery percentage if known
        if (device.batteryPercent >= 0) {
            subtitle += L" · " + std::to_wstring(device.batteryPercent) + L"% battery";
        }

        FrameworkElement trailing{nullptr};
        if (device.address.ullLong == g_bluetoothConnectingAddress) {
            trailing = BuildVectorIcon(nullptr, L"", icons::kBluetoothStroke, 24, 15, 1.7);
        } else if (device.connected) {
            trailing = BuildVectorIcon(nullptr, L"", icons::kCheckStroke, 24, 15, 2.0);
        }

        bluetooth::Device captured = device;
        children.Append(MakeListRow(icon, device.name, subtitle, trailing, [captured] {
            if (!captured.paired) {
                ShellExecute(nullptr, L"open", L"ms-settings:bluetooth", nullptr, nullptr,
                             SW_SHOWNORMAL);
                HideAllFlyouts();
                return;
            }
            bool connect = !captured.connected;
            
            // Set state: 1 for Connect, 2 for Disconnect
            g_bluetoothConnectingState = connect ? 1 : 2;
            g_bluetoothConnectingAddress = captured.address.ullLong;
            RepopulateLater(PopulateBluetoothPanel);
            
            RunInBackground([captured, connect] {
                bluetooth::SetConnected(captured.address, connect);
                WaitForSingleObject(g_stopEvent, 1200);
                auto devices = bluetooth::Enumerate(false);
                RunOnUiThread([devices = std::move(devices)]() mutable {
                    g_bluetoothDevices = std::move(devices);
                    g_bluetoothConnectingAddress = 0;
                    g_bluetoothConnectingState = 0; // Reset state
                    RefreshBluetoothButtonIcon();
                    PopulateBluetoothPanel();
                });
            });
        }));
    }

    children.Append(MakeDivider());
    children.Append(MakeSettingsLink(L"Bluetooth settings", L"ms-settings:bluetooth"));
    // Apply any styles for dynamically created elements (like the Bluetooth toggle).
    ApplyAllControlStyles();
}

// ============================================================================
// Tray panel
// ============================================================================

// Turns raw BGRA into a BitmapImage the same way HIconToBitmapImage does, via a
// BMP in an in-memory stream.
wuxm::Imaging::BitmapImage Bgra32ToBitmapImage(const std::vector<uint8_t>& pixels, int width,
                                               int height) {
    if (pixels.empty() || width <= 0 || height <= 0) {
        return nullptr;
    }

    BITMAPINFOHEADER infoHeader{};
    infoHeader.biSize = sizeof(BITMAPINFOHEADER);
    infoHeader.biWidth = width;
    infoHeader.biHeight = -height;  // top-down
    infoHeader.biPlanes = 1;
    infoHeader.biBitCount = 32;
    infoHeader.biCompression = BI_RGB;
    infoHeader.biSizeImage = static_cast<DWORD>(pixels.size());

    BITMAPFILEHEADER fileHeader{};
    fileHeader.bfType = 0x4D42;  // "BM"
    fileHeader.bfSize = static_cast<DWORD>(sizeof(fileHeader) + sizeof(infoHeader) +
                                           pixels.size());
    fileHeader.bfOffBits = sizeof(fileHeader) + sizeof(infoHeader);

    try {
        winrt::Windows::Storage::Streams::InMemoryRandomAccessStream stream;
        winrt::Windows::Storage::Streams::DataWriter writer(stream);
        writer.WriteBytes(winrt::array_view<const uint8_t>(
            reinterpret_cast<const uint8_t*>(&fileHeader), sizeof(fileHeader)));
        writer.WriteBytes(winrt::array_view<const uint8_t>(
            reinterpret_cast<const uint8_t*>(&infoHeader), sizeof(infoHeader)));
        writer.WriteBytes(pixels);
        writer.StoreAsync().get();
        writer.DetachStream();
        stream.Seek(0);

        wuxm::Imaging::BitmapImage bitmap;
        bitmap.SetSourceAsync(stream);
        return bitmap;
    } catch (...) {
        return nullptr;
    }
}

// There is no API that hands over another process's notification icon bitmap,
// so the icon is read back off the screen where the real taskbar is already
// drawing it. Anything occluded or off-screen simply produces a blank capture,
// which is detected and falls back to a generic glyph.
wuxm::Imaging::BitmapImage CaptureScreenRect(const RECT& bounds) {
    int width = bounds.right - bounds.left;
    int height = bounds.bottom - bounds.top;
    if (width <= 0 || height <= 0 || width > 256 || height > 256) {
        return nullptr;
    }

    HDC screenDc = GetDC(nullptr);
    if (!screenDc) {
        return nullptr;
    }
    HDC memoryDc = CreateCompatibleDC(screenDc);
    if (!memoryDc) {
        ReleaseDC(nullptr, screenDc);
        return nullptr;
    }

    BITMAPINFO bmi{};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void* bits = nullptr;
    HBITMAP dib = CreateDIBSection(memoryDc, &bmi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!dib) {
        DeleteDC(memoryDc);
        ReleaseDC(nullptr, screenDc);
        return nullptr;
    }

    HGDIOBJ old = SelectObject(memoryDc, dib);
    BOOL copied = BitBlt(memoryDc, 0, 0, width, height, screenDc, bounds.left, bounds.top,
                         SRCCOPY);

    std::vector<uint8_t> pixels;
    if (copied) {
        size_t dataSize = static_cast<size_t>(width) * height * 4;
        pixels.resize(dataSize);
        memcpy(pixels.data(), bits, dataSize);
    }

    SelectObject(memoryDc, old);
    DeleteObject(dib);
    DeleteDC(memoryDc);
    ReleaseDC(nullptr, screenDc);

    if (pixels.empty()) {
        return nullptr;
    }

    // BitBlt gives no alpha channel, and the taskbar behind the icon is a solid
    // colour. Treat the most common colour as the background and knock it out,
    // so the icon sits on the panel rather than on a grey chip.
    std::map<uint32_t, int> histogram;
    for (size_t i = 0; i + 3 < pixels.size(); i += 4) {
        uint32_t key = (static_cast<uint32_t>(pixels[i]) << 16) |
                       (static_cast<uint32_t>(pixels[i + 1]) << 8) |
                       static_cast<uint32_t>(pixels[i + 2]);
        histogram[key]++;
    }
    uint32_t background = 0;
    int backgroundCount = 0;
    for (const auto& entry : histogram) {
        if (entry.second > backgroundCount) {
            background = entry.first;
            backgroundCount = entry.second;
        }
    }

    int totalPixels = width * height;
    // A capture that is almost entirely one colour is an occluded or empty
    // slot, not an icon.
    if (backgroundCount > totalPixels * 9 / 10) {
        return nullptr;
    }

    uint8_t backgroundB = static_cast<uint8_t>((background >> 16) & 0xFF);
    uint8_t backgroundG = static_cast<uint8_t>((background >> 8) & 0xFF);
    uint8_t backgroundR = static_cast<uint8_t>(background & 0xFF);
    for (size_t i = 0; i + 3 < pixels.size(); i += 4) {
        int deltaB = std::abs(static_cast<int>(pixels[i]) - backgroundB);
        int deltaG = std::abs(static_cast<int>(pixels[i + 1]) - backgroundG);
        int deltaR = std::abs(static_cast<int>(pixels[i + 2]) - backgroundR);
        bool isBackground = deltaB + deltaG + deltaR < 24;
        pixels[i + 3] = isBackground ? 0 : 255;
        if (isBackground) {
            pixels[i] = pixels[i + 1] = pixels[i + 2] = 0;  // premultiplied
        }
    }

    return Bgra32ToBitmapImage(pixels, width, height);
}

void PopulateTrayPanel() {
    if (!g_trayPanel) return;

    g_populatingPanel = true;
    struct Guard {
        ~Guard() { g_populatingPanel = false; }
    } guard;

    auto children = g_trayPanel.Children();
    children.Clear();

    children.Append(MakePanelTitle(L"System tray"));

    // Show a placeholder while we fetch
    auto loading = MakeStatusText(L"Loading tray icons…");
    children.Append(loading);

    // Fetch icons in background
    RunInBackground([] {
        auto items = tray::Enumerate();
        RunOnUiThread([items = std::move(items)]() mutable {
            if (!g_trayPanel) return;
            auto children = g_trayPanel.Children();
            children.Clear();
            children.Append(MakePanelTitle(L"System tray"));

            if (items.empty()) {
                children.Append(MakeStatusText(
                    L"No tray icons were found. The Windows taskbar has to be running "
                    L"and visible for its notification icons to be readable."));
                return;
            }

            for (const auto& item : items) {
                auto button = MakeListRow(
                    nullptr, item.name, L"", nullptr,
                    [item] { tray::InvokeItem(item); });
                children.Append(button);
            }
        });
    });
}

struct BatteryInfo {
    int percentage = 0;
    int health = 100;   // percentage of design capacity
    bool charging = false;
    bool powerSaving = false;
};

BatteryInfo GetBatteryInfo() {
    BatteryInfo info;
    SYSTEM_POWER_STATUS powerStatus;
    if (GetSystemPowerStatus(&powerStatus)) {
        // On desktops (no battery) BatteryLifePercent returns 255.
        // If no battery or unknown, show 100%.
        if (powerStatus.BatteryLifePercent == 255 || (powerStatus.BatteryFlag & 128)) {
            info.percentage = 100;
        } else {
            info.percentage = powerStatus.BatteryLifePercent;
        }
                // On desktops (no battery), show charging icon (always plugged in).
        if (powerStatus.BatteryFlag & 128) {
            info.charging = true;
        } else {
            info.charging = (powerStatus.ACLineStatus == 1);
        }
    }
    // Power saving mode (via registry or system setting)
    info.powerSaving = false;
    HKEY key = nullptr;
    if (RegOpenKeyEx(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Power\\SystemSettings",
                     0, KEY_READ, &key) == ERROR_SUCCESS) {
        DWORD value = 0, size = sizeof(value);
        if (RegQueryValueEx(key, L"PowerSavingMode", nullptr, nullptr, (BYTE*)&value, &size) == ERROR_SUCCESS)
            info.powerSaving = value != 0;
        RegCloseKey(key);
    }
    // Battery health via WMI
    try {
        // Use `Win32_Battery` – get FullChargeCapacity and DesignCapacity
        // (if they exist). We'll just use a default if query fails.
        // (Simplified – real WMI query might be added later.)
        info.health = 100;
    } catch (...) {
        info.health = 100;
    }
    return info;
}





void UpdateBatteryButton() {
    if (!g_batteryButton) return;
    BatteryInfo info = GetBatteryInfo();
    auto batteryIcon = BuildBatteryIcon(36, info.charging);
    auto batteryStack = wuxc::StackPanel();
    batteryStack.Orientation(wuxc::Orientation::Horizontal);
    batteryStack.Spacing(4);
    if (batteryIcon) batteryStack.Children().Append(batteryIcon);
    auto percentText = MakeText(nullptr, std::to_wstring(info.percentage) + L"%", 12);
    batteryStack.Children().Append(percentText);
    g_batteryButton.Content(batteryStack);
}

void PopulateBatteryPanel() {
    if (!g_batteryPanel) return;

    g_populatingPanel = true;
    struct Guard {
        ~Guard() { g_populatingPanel = false; }
    } guard;

    auto children = g_batteryPanel.Children();
    children.Clear();

    children.Append(MakePanelTitle(L"Battery"));

    BatteryInfo info = GetBatteryInfo();

    // Percentage display
    auto percentageText = MakeText(L"BatteryPercentage", std::to_wstring(info.percentage) + L"%", 32, true);
    percentageText.HorizontalAlignment(HorizontalAlignment::Center);
    percentageText.Margin(Thickness{0, 8, 0, 4});
    children.Append(percentageText);

    // Health
    auto healthText = MakeText(L"BatteryHealth", L"Battery health: " + std::to_wstring(info.health) + L"%", 13, false, 0.7);
    healthText.HorizontalAlignment(HorizontalAlignment::Center);
    healthText.Margin(Thickness{0, 0, 0, 8});
    children.Append(healthText);




}

// ============================================================================
// Context menus
//
// The system MenuFlyoutPresenter draws square corners and gives MenuFlyoutItem
// and MenuFlyoutSubItem *different* pointer-over brushes, which is why the
// submenu highlighted in a different colour from the rest. Both are fixed by
// overriding the theme resources rather than by retemplating: the overrides go
// into the application resource dictionary so they also reach the submenu
// popups, which are separate visual trees and would otherwise keep the
// defaults.
// ============================================================================

constexpr double kMenuCorner = 8.0;
constexpr double kMenuItemCorner = 4.0;

void InstallGlobalMenuResources() {
    try {
        auto application = Application::Current();
        if (!application) {
            return;
        }
        auto resources = application.Resources();

        auto hover = MakeBrush(0x30, GetSystemAccentColor().R, GetSystemAccentColor().G, GetSystemAccentColor().B);
        auto pressed = MakeBrush(0x20, GetSystemAccentColor().R, GetSystemAccentColor().G, GetSystemAccentColor().B);

        auto set = [&](PCWSTR key, wf::IInspectable const& value) {
            auto boxedKey = winrt::box_value(winrt::hstring(key));
            if (resources.HasKey(boxedKey)) {
                resources.Remove(boxedKey);
            }
            resources.Insert(boxedKey, value);
        };

        // Rounds the presenter itself -- the default MenuFlyoutPresenter
        // template binds its corner radius to this.
        set(L"OverlayCornerRadius", winrt::box_value(MakeCorner(kMenuCorner)));

        // Solid background for menu popups (including submenus) until blur is fixed
        set(L"MenuFlyoutPresenterBackground", MakeBrush(0xFF, 0x20, 0x20, 0x20));
        set(L"MenuFlyoutPresenterBorderBrush", MakeBrush(0xFF, 0x40, 0x40, 0x40));
        // Leave FlyoutPresenterBackground as transparent for control flyouts
        set(L"FlyoutPresenterBackground", FlyoutBackgroundBrush());
        set(L"FlyoutPresenterBorderBrush", MakeBrush(0x30, 0xFF, 0xFF, 0xFF));

        // The pair that made the submenu look different from everything else.
        set(L"MenuFlyoutItemBackgroundPointerOver", hover);
        set(L"MenuFlyoutItemBackgroundPressed", pressed);
        set(L"MenuFlyoutSubItemBackgroundPointerOver", hover);
        set(L"MenuFlyoutSubItemBackgroundPressed", pressed);
        set(L"MenuFlyoutSubItemBackgroundSubMenuOpened", hover);
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"Could not install menu resources: %08X",
               static_cast<unsigned int>(ex.code().value));
    } catch (...) {
    }
}

Style MakeMenuPresenterStyle() {
    Style style(winrt::xaml_typename<wuxc::MenuFlyoutPresenter>());
    auto setters = style.Setters();
    // Transparent background for main menu (overrides global solid)
    setters.Append(Setter(wuxc::Control::BackgroundProperty(),
                          winrt::box_value(FlyoutBackgroundBrush())));
    setters.Append(Setter(wuxc::Control::BorderBrushProperty(),
                          winrt::box_value(MakeBrush(0x30, 0xFF, 0xFF, 0xFF))));
    setters.Append(Setter(wuxc::Control::BorderThicknessProperty(),
                          winrt::box_value(Thickness{1, 1, 1, 1})));
    setters.Append(Setter(wuxc::Control::CornerRadiusProperty(),
                          winrt::box_value(MakeCorner(kMenuCorner))));
    setters.Append(
        Setter(wuxc::Control::PaddingProperty(), winrt::box_value(Thickness{4, 4, 4, 4})));
    setters.Append(Setter(FrameworkElement::MinWidthProperty(), winrt::box_value(200.0)));
    return style;
}

// Per-item overrides as well as the global ones: an item's own resource
// dictionary is checked first, so this holds even if a future Windows build
// renames or re-scopes the theme resources.
void ApplyMenuItemLook(wuxc::MenuFlyoutItemBase const& item) {
    try {
        auto hover = MakeBrush(0x24, GetSystemAccentColor().R, GetSystemAccentColor().G, GetSystemAccentColor().B);
        auto pressed = MakeBrush(0x14, GetSystemAccentColor().R, GetSystemAccentColor().G, GetSystemAccentColor().B);
        auto resources = item.Resources();
        auto set = [&](PCWSTR key, wf::IInspectable const& value) {
            auto boxedKey = winrt::box_value(winrt::hstring(key));
            if (resources.HasKey(boxedKey)) {
                resources.Remove(boxedKey);
            }
            resources.Insert(boxedKey, value);
        };
        set(L"MenuFlyoutItemBackgroundPointerOver", hover);
        set(L"MenuFlyoutItemBackgroundPressed", pressed);
        set(L"MenuFlyoutSubItemBackgroundPointerOver", hover);
        set(L"MenuFlyoutSubItemBackgroundPressed", pressed);
        set(L"MenuFlyoutSubItemBackgroundSubMenuOpened", hover);
        set(L"OverlayCornerRadius", winrt::box_value(MakeCorner(kMenuCorner)));

        if (auto control = item.try_as<wuxc::Control>()) {
            control.CornerRadius(MakeCorner(kMenuItemCorner));
            control.Padding(Thickness{12, 7, 12, 7});
        }
    } catch (...) {
    }
}

wuxc::MenuFlyoutItem MakeMenuItem(std::wstring_view text, std::function<void()> onClick) {
    wuxc::MenuFlyoutItem item;
    item.Text(winrt::hstring(text));
    ApplyMenuItemLook(item);
    if (onClick) {
        item.Click([onClick = std::move(onClick)](auto&&, auto&&) {
            try {
                onClick();
            } catch (...) {
            }
        });
    }
    return item;
}

wuxc::MenuFlyoutSubItem MakeMenuSubItem(std::wstring_view text) {
    wuxc::MenuFlyoutSubItem item;
    item.Text(winrt::hstring(text));
    ApplyMenuItemLook(item);
    // Trigger blur when the submenu is about to open (mouse enters the item)
    item.PointerEntered([](auto&&, auto&&) {
        ApplyBlurToAllOpenPopups();
    });
    return item;
}

wuxc::MenuFlyoutSeparator MakeMenuSeparator() {
    wuxc::MenuFlyoutSeparator separator;
    return separator;
}

void StyleMenuFlyout(wuxc::MenuFlyout const& menu) {
    menu.MenuFlyoutPresenterStyle(MakeMenuPresenterStyle());
    // Same reason as the control flyouts: the XAML root is bar-height, so a menu
    // constrained to it would be clipped away entirely.
    menu.ShouldConstrainToRootBounds(false);
    menu.Opened([](auto&&, auto&&) {
        ApplyBlurToAllOpenPopups();
    });
}

// ----------------------------------------------------------------------------
// Shell actions used by the Start menu
// ----------------------------------------------------------------------------

void RunShellCommand(PCWSTR file, PCWSTR parameters = nullptr, bool hidden = false) {
    ShellExecute(nullptr, L"open", file, parameters, nullptr,
                 hidden ? SW_HIDE : SW_SHOWNORMAL);
}

// Synthesizes a Win+key chord. Used for the shell surfaces that have no command
// line at all -- Search and Run are only reachable this way.
void SendWinKeyChord(WORD key) {
    INPUT input[4]{};
    input[0].type = INPUT_KEYBOARD;
    input[0].ki.wVk = VK_LWIN;
    input[1].type = INPUT_KEYBOARD;
    input[1].ki.wVk = key;
    input[2].type = INPUT_KEYBOARD;
    input[2].ki.wVk = key;
    input[2].ki.dwFlags = KEYEVENTF_KEYUP;
    input[3].type = INPUT_KEYBOARD;
    input[3].ki.wVk = VK_LWIN;
    input[3].ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(ARRAYSIZE(input), input, sizeof(INPUT));
}

void ShowDesktop() {
    // 419 is the shell's "toggle desktop" command; it's what Win+D posts.
    if (HWND tray = FindWindow(L"Shell_TrayWnd", nullptr)) {
        PostMessage(tray, WM_COMMAND, 419, 0);
        return;
    }
    SendWinKeyChord('D');
}

void SuspendSystem() {
    using SetSuspendState_t = BOOLEAN(WINAPI*)(BOOLEAN, BOOLEAN, BOOLEAN);
    HMODULE module = LoadLibraryEx(L"powrprof.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) {
        return;
    }
    auto setSuspendState =
        reinterpret_cast<SetSuspendState_t>(GetProcAddress(module, "SetSuspendState"));
    if (setSuspendState) {
        setSuspendState(FALSE /* hibernate */, FALSE /* force */, FALSE /* wake events */);
    }
    FreeLibrary(module);
}

void BuildStartContextMenu() {
    wuxc::MenuFlyout menu;
    StyleMenuFlyout(menu);

    auto items = menu.Items();
    items.Append(MakeMenuItem(L"Task Manager", [] { RunShellCommand(L"taskmgr.exe"); }));
    items.Append(MakeMenuItem(L"Settings", [] { RunShellCommand(L"ms-settings:"); }));
    items.Append(MakeMenuItem(L"File Explorer", [] { RunShellCommand(L"explorer.exe"); }));
    items.Append(MakeMenuItem(L"Search", [] { SendWinKeyChord('S'); }));
    items.Append(MakeMenuItem(L"Run", [] { SendWinKeyChord('R'); }));

    items.Append(MakeMenuSeparator());

    auto powerItem = MakeMenuSubItem(L"Shut down or sign out");
    auto powerItems = powerItem.Items();
    powerItems.Append(
        MakeMenuItem(L"Sign out", [] { RunShellCommand(L"shutdown.exe", L"/l", true); }));
    powerItems.Append(MakeMenuItem(L"Sleep", [] { SuspendSystem(); }));
    powerItems.Append(MakeMenuItem(
        L"Shut down", [] { RunShellCommand(L"shutdown.exe", L"/s /t 0", true); }));
    powerItems.Append(
        MakeMenuItem(L"Restart", [] { RunShellCommand(L"shutdown.exe", L"/r /t 0", true); }));
    items.Append(powerItem);

    items.Append(MakeMenuSeparator());
    items.Append(MakeMenuItem(L"Desktop", [] { ShowDesktop(); }));

    g_startContextMenu = menu;
}

void BuildTaskContextMenu() {
    wuxc::MenuFlyout menu;
    StyleMenuFlyout(menu);

    auto items = menu.Items();

    g_taskMenuToggleItem = MakeMenuItem(L"Maximize", [] {
        if (g_contextMenuTargetHwnd) {
            ToggleMaximizeWindow(g_contextMenuTargetHwnd);
        }
    });
    items.Append(g_taskMenuToggleItem);

    items.Append(MakeMenuItem(L"Minimize", [] {
        if (g_contextMenuTargetHwnd) {
            ShowWindow(g_contextMenuTargetHwnd, SW_MINIMIZE);
        }
    }));

    items.Append(MakeMenuItem(L"Bring to front", [] {
        if (g_contextMenuTargetHwnd) {
            ForceForegroundWindow(g_contextMenuTargetHwnd);
        }
    }));

    items.Append(MakeMenuSeparator());

    items.Append(MakeMenuItem(L"Close", [] {
        if (g_contextMenuTargetHwnd) {
            CloseWindowGracefully(g_contextMenuTargetHwnd);
        }
    }));

    g_taskContextMenu = menu;
}

// ============================================================================
// Battery info
// ============================================================================



void RegisterNamed(PCWSTR name, FrameworkElement const& element) {
    if (element && name && *name) {
        g_namedElements.insert_or_assign(name, element);
    }
}
// Gets the current desktop wallpaper and returns an ImageBrush from it.
// Returns an empty brush if wallpaper is missing or fails to load.
wuxm::ImageBrush GetWallpaperBrush() {
    
    wuxm::ImageBrush brush;

    wchar_t wallpaperPath[MAX_PATH] = {0};
    if (SystemParametersInfo(SPI_GETDESKWALLPAPER, MAX_PATH, wallpaperPath, 0) && wallpaperPath[0]) {
        // Convert Windows path to a file URI: replace '\' with '/'
        std::wstring uriPath = wallpaperPath;
        std::replace(uriPath.begin(), uriPath.end(), L'\\', L'/');
        uriPath = L"file:///" + uriPath;

        try {
            wuxm::Imaging::BitmapImage bitmap;
            bitmap.UriSource(wf::Uri(winrt::hstring(uriPath)));
            brush.ImageSource(bitmap);
            brush.Stretch(wuxm::Stretch::UniformToFill);
            brush.AlignmentX(wuxm::AlignmentX::Left);   // Show the left side (optional)
            brush.AlignmentY(wuxm::AlignmentY::Top);    // Show the top portion
        } catch (...) {
            // Failed to parse or load, leave brush empty
        }
    }
    return brush;
}

// Checks if the wallpaper has changed and updates the background
void UpdateWallpaperIfChanged() {
    if (!g_wallpaperLayer) return;

    wchar_t wallpaperPath[MAX_PATH] = {0};
    if (SystemParametersInfo(SPI_GETDESKWALLPAPER, MAX_PATH, wallpaperPath, 0) && wallpaperPath[0]) {
        std::wstring currentPath = wallpaperPath;
        if (currentPath != g_lastWallpaperPath) {
            g_lastWallpaperPath = currentPath;
            g_wallpaperLayer.Background(GetWallpaperBrush());
            Wh_Log(L"TopBar: Wallpaper updated: %s", currentPath.c_str());
        }
    }
}
// Wheel over the Display and Sound buttons adjusts brightness and volume in
// place. Both read through the cached fast path so a fast scroll doesn't queue
// up a WMI query or a device activation per notch.
void RefreshDisplayButtonIcon() {
    if (!g_displayButton) return;
    auto icon = BuildVectorIcon(nullptr, L"", icons::kBrightnessStroke, 24, 18, 1.6);
    g_displayButton.Content(icon);
}

void ShowVolumePercent(int percent) {
    if (!g_soundButton) return;
    auto text = MakeText(nullptr, std::to_wstring(percent) + L"%", 16, true);
    text.HorizontalAlignment(HorizontalAlignment::Center);
    text.VerticalAlignment(VerticalAlignment::Center);
    g_soundButton.Content(text);
}

void ShowBrightnessPercent(int percent) {
    if (!g_displayButton) return;
    auto text = MakeText(nullptr, std::to_wstring(percent) + L"%", 16, true);
    text.HorizontalAlignment(HorizontalAlignment::Center);
    text.VerticalAlignment(VerticalAlignment::Center);
    g_displayButton.Content(text);
}

void AttachWheelHandler(wuxc::Button const& button, bool isVolume) {
    button.PointerWheelChanged(
        [isVolume](wf::IInspectable const& sender, Input::PointerRoutedEventArgs const& args) {
            try {
                auto point = args.GetCurrentPoint(sender.as<UIElement>());
                int delta = point.Properties().MouseWheelDelta();
                if (delta == 0) return;

                int step = isVolume ? 6 : 4;
                int direction = delta > 0 ? 1 : -1;

                if (isVolume) {
                    int newVolume = std::clamp(audio::GetMasterVolume() + step * direction, 0, 100);
                    RunInBackground([newVolume] { audio::SetMasterVolume(newVolume); });
                    ShowVolumePercent(newVolume);

                    if (!g_volumeRevertTimer) {
                        g_volumeRevertTimer = DispatcherTimer();
                        g_volumeRevertTimer.Interval(std::chrono::milliseconds(700));
                        g_volumeRevertTimer.Tick([](wf::IInspectable const&, wf::IInspectable const&) {
                            g_volumeRevertTimer.Stop();
                            RefreshSoundButtonIcon();
                        });
                    }
                    g_volumeRevertTimer.Stop();
                    g_volumeRevertTimer.Start();
                } else {
                    static bool brightnessPending = false;
                    if (brightnessPending) return;
                    brightnessPending = true;
                    int newBrightness = std::clamp(brightness::GetFast() + step * direction, 0, 100);
                    RunInBackground([newBrightness] {
                        brightness::Set(newBrightness);
                        brightnessPending = false;
                    });
                    ShowBrightnessPercent(newBrightness);

                    if (!g_brightnessRevertTimer) {
                        g_brightnessRevertTimer = DispatcherTimer();
                        g_brightnessRevertTimer.Interval(std::chrono::milliseconds(700));
                        g_brightnessRevertTimer.Tick([](wf::IInspectable const&, wf::IInspectable const&) {
                            g_brightnessRevertTimer.Stop();
                            RefreshDisplayButtonIcon();
                        });
                    }
                    g_brightnessRevertTimer.Stop();
                    g_brightnessRevertTimer.Start();
                }

                args.Handled(true);
            } catch (...) {}
        });
}

// One of the five control-centre buttons. They share a size, a corner radius
// and a flyout shape so the right-hand cluster reads as a single unit.
wuxc::Button MakeControlButton(PCWSTR name,
                               FrameworkElement icon,
                               wuxc::Flyout const& flyout,
                               const std::function<void()>& onOpening) {
    auto button = MakeGhostButton(name, g_settings.cornerRadius);
    button.Content(icon);
    button.VerticalAlignment(VerticalAlignment::Stretch);
    button.Margin(Thickness{5, 4, 5, 4});
    button.Padding(Thickness{7, 0, 7, 0});
    button.HorizontalContentAlignment(HorizontalAlignment::Center);
    if (flyout) {
        button.Flyout(flyout);
        if (onOpening) {
            // Populate on open rather than up front: enumerating devices,
            // sessions and tray icons is far too expensive to do on every
            // settings change.
            flyout.Opening([onOpening](auto&&, auto&&) {
                try {
                    onOpening();
                    // Apply styles to any dynamically created elements.
                    ApplyAllControlStyles();
                } catch (...) {
                }
            });
        }
    }
    RegisterNamed(name, button);
    return button;
}
void EnsureAutoRefreshTimers() {
    if (!g_wifiAutoRefreshTimer) {
        g_wifiAutoRefreshTimer = DispatcherTimer();
        g_wifiAutoRefreshTimer.Interval(std::chrono::seconds(30));
        g_wifiAutoRefreshTimer.Tick([](wf::IInspectable const&, wf::IInspectable const&) {
            if (!g_wifiFlyout || !g_wifiFlyout.IsOpen()) {
                g_wifiAutoRefreshTimer.Stop();
                return;
            }
            // Don't refresh while typing password, otherwise it clears the box!
            if (g_wifiPasswordPrompt) {
                return;
            }
            if (!g_wifiScanning) {
                StartWifiScan();
            }
        });
    }
    if (!g_bluetoothAutoRefreshTimer) {
        g_bluetoothAutoRefreshTimer = DispatcherTimer();
        g_bluetoothAutoRefreshTimer.Interval(std::chrono::seconds(60));
        g_bluetoothAutoRefreshTimer.Tick([](wf::IInspectable const&, wf::IInspectable const&) {
            if (!g_bluetoothFlyout || !g_bluetoothFlyout.IsOpen()) {
                g_bluetoothAutoRefreshTimer.Stop();
                return;
            }
            if (!g_bluetoothScanning) {
                StartBluetoothScan(true);
            }
        });
    }
}



FrameworkElement BuildTopBarContent() {
    
    g_namedElements.clear();
    g_taskButtonsByHwnd.clear();
    g_taskButtonLastTitle.clear();
    g_stableWindowOrder.clear();
    g_detachedStyleRoots.clear();  // Clear old flyout roots before rebuilding

    // Outer root grid that holds everything (background + interactive bar)
    wuxc::Grid barRoot;
    barRoot.Name(L"BarRoot");
    barRoot.HorizontalAlignment(HorizontalAlignment::Stretch);
    barRoot.VerticalAlignment(VerticalAlignment::Stretch);
    barRoot.Background(nullptr); // Transparent so the blur shows through
    // Wallpaper layer (added first so it's behind everything)
    wuxm::ImageBrush wallpaperBrush = GetWallpaperBrush();
    {
        wuxc::Grid wallpaperLayer;
        wallpaperLayer.Name(L"WallpaperLayer");
        wallpaperLayer.Background(wallpaperBrush);
        wallpaperLayer.IsHitTestVisible(false); // don't block clicks
        wallpaperLayer.HorizontalAlignment(HorizontalAlignment::Stretch);
        wallpaperLayer.VerticalAlignment(VerticalAlignment::Stretch);
        wallpaperLayer.Opacity(1.0);
        barRoot.Children().Append(wallpaperLayer);
        g_wallpaperLayer = wallpaperLayer; // Save reference
    }
    // Interactive content grid (This is TopBarRoot and gets the blur)
    wuxc::Grid root;
    root.Name(L"TopBarRoot");
    root.HorizontalAlignment(HorizontalAlignment::Stretch);
    root.VerticalAlignment(VerticalAlignment::Stretch);

    // Parse the user's color and opacity
    wui::Color tintColor;
    if (!ParseBarColor(g_settings.topBarBackgroundColor, g_settings.topBarBackgroundOpacity, &tintColor)) {
        tintColor = wui::ColorHelper::FromArgb(128, 0, 0, 0); // default: black 50%
    }

    // Apply a simple semi-transparent brush to TopBarRoot.
    // The actual blur comes from the window accent policy (ApplyWindowBackdrop).
    root.Background(wuxm::SolidColorBrush(tintColor));

    for (int i = 0; i < 3; i++) {
        wuxc::ColumnDefinition column;
        column.Width(GridLength{i == 1 ? 1.0 : 0.0,
                                i == 1 ? GridUnitType::Star : GridUnitType::Auto});
        root.ColumnDefinitions().Append(column);
    }

    // Add the interactive root on top of the wallpaper layer
    barRoot.Children().Append(root);

    // Double-clicking empty bar space maximizes or restores the window that was
    // last in the foreground. The bar itself takes focus on click, so the live
    // foreground window is useless here -- the WinEvent hook's record is used
    // instead.
    root.DoubleTapped([](auto&&, Input::DoubleTappedRoutedEventArgs const& args) {
        args.Handled(true);
        if (g_lastForegroundHwnd && IsWindow(g_lastForegroundHwnd)) {
            ToggleMaximizeWindow(g_lastForegroundHwnd);
        }
    });

    // ---- Left cluster: Start, Search --------------------------------------
    wuxc::StackPanel leftPanel;
    leftPanel.Name(L"LeftPanel");
    leftPanel.Orientation(wuxc::Orientation::Horizontal);
    leftPanel.VerticalAlignment(VerticalAlignment::Stretch);

    {
        auto startButton = MakeGhostButton(L"StartButton", g_settings.cornerRadius);
        startButton.VerticalAlignment(VerticalAlignment::Stretch);
        startButton.Margin(Thickness{8, 2, 2, 2});
        startButton.Padding(Thickness{0, 0, 0, 0});
        startButton.HorizontalContentAlignment(HorizontalAlignment::Center);
        if (auto icon = BuildWindows11StartIcon(20)) {
          startButton.Content(icon);
          RegisterNamed(L"StartIcon", icon);
    }
        startButton.Click([](auto&&, auto&&) { SendWinKeyChord(VK_LWIN); });
        startButton.RightTapped(
            [](wf::IInspectable const& sender, Input::RightTappedRoutedEventArgs const& args) {
                args.Handled(true);
                if (g_startContextMenu) {
                    g_startContextMenu.ShowAt(sender.as<FrameworkElement>());
                }
            });
        RegisterNamed(L"StartButton", startButton);
        leftPanel.Children().Append(startButton);
    }

    {
        // The old search *box* never worked and isn't wanted; this is a plain
        // button that opens the real Windows search.
        auto searchButton = MakeGhostButton(L"SearchButton", g_settings.cornerRadius);
        searchButton.VerticalAlignment(VerticalAlignment::Stretch);
        searchButton.Margin(Thickness{4, 2, 4, 2});
        searchButton.Padding(Thickness{0, 0, 0, 0});
        searchButton.HorizontalContentAlignment(HorizontalAlignment::Center);
        if (auto icon = BuildSearchIcon(20)) {
            searchButton.Content(icon);
            RegisterNamed(L"SearchIcon", icon);
        }
        searchButton.Click([](auto&&, auto&&) { SendWinKeyChord('S'); });
        RegisterNamed(L"SearchButton", searchButton);
        leftPanel.Children().Append(searchButton);
    }

    wuxc::Grid::SetColumn(leftPanel, 0);
    root.Children().Append(leftPanel);
    RegisterNamed(L"LeftPanel", leftPanel);

    // ---- Middle: the task list --------------------------------------------
    wuxc::StackPanel taskPanel;
    taskPanel.Name(L"TaskListPanel");
    taskPanel.Orientation(wuxc::Orientation::Horizontal);
    // Stretch so the task buttons inherit the full bar height and their margins
    // mean the same thing as the Start and Search margins do.
    taskPanel.VerticalAlignment(VerticalAlignment::Stretch);
    taskPanel.HorizontalAlignment(HorizontalAlignment::Left);
    wuxc::Grid::SetColumn(taskPanel, 1);
    root.Children().Append(taskPanel);
    g_taskListPanel = taskPanel;
    RegisterNamed(L"TaskListPanel", taskPanel);

    // ---- Right cluster: control centre + clock -----------------------------
    wuxc::StackPanel rightPanel;
    rightPanel.Name(L"TrayPanel");
    rightPanel.Orientation(wuxc::Orientation::Horizontal);
    rightPanel.VerticalAlignment(VerticalAlignment::Stretch);

    g_displayFlyout = MakeControlFlyout(L"DisplayFlyoutRoot", g_displayPanel);
    g_soundFlyout = MakeControlFlyout(L"SoundFlyoutRoot", g_soundPanel);
    g_wifiFlyout = MakeControlFlyout(L"WifiFlyoutRoot", g_wifiPanel);
    g_bluetoothFlyout = MakeControlFlyout(L"BluetoothFlyoutRoot", g_bluetoothPanel);
    g_trayFlyout = MakeControlFlyout(L"TrayFlyoutRoot", g_trayPanel);
    g_batteryFlyout = MakeControlFlyout(L"BatteryFlyoutRoot", g_batteryPanel);

    g_wifiFlyout.Closed([](auto&&, auto&&) {
        if (g_wifiAutoRefreshTimer) {
            g_wifiAutoRefreshTimer.Stop();
        }
    });
    g_bluetoothFlyout.Closed([](auto&&, auto&&) {
        if (g_bluetoothAutoRefreshTimer) {
            g_bluetoothAutoRefreshTimer.Stop();
        }
    });

    g_displayButton = MakeControlButton(
        L"DisplayButton", BuildVectorIcon(nullptr, L"", icons::kBrightnessStroke, 24, 18, 1.6),
        g_displayFlyout, [] {
            // Show loading placeholder immediately
            if (g_displayPanel) {
                g_populatingPanel = true;
                auto children = g_displayPanel.Children();
                children.Clear();
                children.Append(MakePanelTitle(L"Display"));
                children.Append(MakeStatusText(L"Loading…"));
                g_populatingPanel = false;
            }
            // Fetch brightness in background
            RunInBackground([] {
                bool available = brightness::Available();
                int brightnessValue = brightness::Get();
                bool darkMode = IsAppsDarkMode();
                RunOnUiThread([available, brightnessValue, darkMode]() {
                    if (!g_displayPanel) return;
                    g_populatingPanel = true;
                    auto children = g_displayPanel.Children();
                    children.Clear();
                    children.Append(MakePanelTitle(L"Display"));

                    if (available) {
                        auto icon = BuildVectorIcon(nullptr, L"", icons::kBrightnessStroke, 24, 18, 1.6);
                        children.Append(MakeSliderRow(icon, brightnessValue,
                            [](int value) { RunInBackground([value] { brightness::Set(value); }); }));
                    } else {
                        children.Append(MakeStatusText(L"Brightness control isn't available on this display."));
                    }

                    children.Append(MakeDivider());

                    auto darkTile = MakeGhostButton(L"QuickToggleTile", kTileCorner);
                    darkTile.HorizontalAlignment(HorizontalAlignment::Stretch);
                    darkTile.Padding(Thickness{8, 6, 8, 6});
                    darkTile.Background(darkMode ? MakeBrush(0xFF, GetSystemAccentColor().R, GetSystemAccentColor().G, GetSystemAccentColor().B)
                                                 : MakeBrush(0x18, 0xFF, 0xFF, 0xFF));
                    wuxc::StackPanel darkStack;
                    darkStack.Orientation(wuxc::Orientation::Horizontal);
                    darkStack.Spacing(8);
                    if (auto icon = BuildVectorIcon(nullptr, icons::kMoonFill, L"", 24, 16, 1.7, L"#FFFFFF")) {
                        icon.VerticalAlignment(VerticalAlignment::Center);
                        darkStack.Children().Append(icon);
                    }
                    darkStack.Children().Append(MakeText(nullptr, L"Dark mode", 12));
                    darkTile.Content(darkStack);
                    darkTile.Click([darkMode](auto&&, auto&&) {
                        SetAppsDarkMode(!darkMode);
                        RepopulateLater(PopulateDisplayPanel);
                    });
                    children.Append(darkTile);

                    children.Append(MakeDivider());
                    children.Append(MakeSettingsLink(L"Display settings", L"ms-settings:display"));
                    g_populatingPanel = false;
                });
            });
        });

    AttachWheelHandler(g_displayButton, false);
    rightPanel.Children().Append(g_displayButton);

    g_soundButton = MakeControlButton(
        L"SoundButton", BuildSpeakerIcon(18, audio::GetMasterVolume(), audio::GetMasterMute()),
        g_soundFlyout, [] { PopulateSoundPanel(); });

    AttachWheelHandler(g_soundButton, true);
    rightPanel.Children().Append(g_soundButton);

    {
        auto status = wifi::GetStatus();

g_wifiButton = MakeControlButton(
    L"WifiButton", BuildWifiIcon(18, status.signal, status.connected),
    g_wifiFlyout, [] {
        // Show loading
        if (g_wifiPanel) {
            g_populatingPanel = true;
            auto children = g_wifiPanel.Children();
            children.Clear();
            children.Append(MakePanelTitle(L"Wi-Fi"));
            children.Append(MakeStatusText(L"Loading…"));
            g_populatingPanel = false;
        }
        // Fetch in background
        RunInBackground([] {
            auto status = wifi::GetStatus();
            auto networks = wifi::EnumerateNetworks();
RunOnUiThread([status, networks = std::move(networks)]() mutable {
    g_wifiNetworks = std::move(networks);
    g_wifiPasswordPrompt = false;
    g_wifiPromptError.clear();
    PopulateWifiPanel();
    StartWifiScan();
    EnsureAutoRefreshTimers();
    if (g_wifiAutoRefreshTimer) g_wifiAutoRefreshTimer.Start();
});
        });
    });


        rightPanel.Children().Append(g_wifiButton);
    }

    {
        auto icon = BuildVectorIcon(nullptr, L"", icons::kBluetoothStroke, 24, 18, 1.8);

g_bluetoothButton = MakeControlButton(L"BluetoothButton", icon, g_bluetoothFlyout, [] {
    // Show loading
    if (g_bluetoothPanel) {
        g_populatingPanel = true;
        auto children = g_bluetoothPanel.Children();
        children.Clear();
        children.Append(MakePanelTitle(L"Bluetooth"));
        children.Append(MakeStatusText(L"Loading…"));
        g_populatingPanel = false;
    }
    RefreshBluetoothRadioState(); // update radio state in background
    RunInBackground([] {
        auto devices = bluetooth::Enumerate(false);
        RunOnUiThread([devices = std::move(devices)]() mutable {
            g_bluetoothDevices = std::move(devices);
            PopulateBluetoothPanel();
            EnsureAutoRefreshTimers();
            if (g_bluetoothAutoRefreshTimer) g_bluetoothAutoRefreshTimer.Start();
        });
    });
});

        rightPanel.Children().Append(g_bluetoothButton);
    }

    // Battery button (with percentage text and charging icon)
    {
        BatteryInfo info = GetBatteryInfo();
        auto batteryIcon = BuildBatteryIcon(36, info.charging);
        auto batteryStack = wuxc::StackPanel();
        batteryStack.Orientation(wuxc::Orientation::Horizontal);
        batteryStack.Spacing(4);
        if (batteryIcon) batteryStack.Children().Append(batteryIcon);
        auto percentText = MakeText(nullptr, std::to_wstring(info.percentage) + L"%", 12);
        batteryStack.Children().Append(percentText);

        g_batteryButton = MakeControlButton(L"BatteryButton", batteryStack, g_batteryFlyout,
                                            [] { PopulateBatteryPanel(); });
        rightPanel.Children().Append(g_batteryButton);
    }

     g_trayButton = MakeControlButton(L"TrayButton",
                                 BuildVectorIcon(nullptr, L"", icons::kChevronUp, 24, 16,
                                                 2.0),
                                 g_trayFlyout, [] { PopulateTrayPanel(); });
    rightPanel.Children().Append(g_trayButton);

    {
        auto clockButton = MakeGhostButton(L"ClockButton", g_settings.cornerRadius);
        clockButton.VerticalAlignment(VerticalAlignment::Stretch);
        clockButton.Margin(Thickness{3, 2, 6, 2});
        clockButton.Padding(Thickness{10, 0, 10, 0});
        clockButton.HorizontalContentAlignment(HorizontalAlignment::Center);

        auto clockText = MakeText(L"ClockText", FormatClockText(), 14);
        clockText.TextAlignment(TextAlignment::Center);
        clockText.LineHeight(15);
        clockButton.Content(clockText);
        clockButton.Click([](auto&&, auto&&) {
            RunShellCommand(L"ms-actioncenter:");
        });

        RegisterNamed(L"ClockText", clockText);
        RegisterNamed(L"ClockButton", clockButton);
        rightPanel.Children().Append(clockButton);
    }

    wuxc::Grid::SetColumn(rightPanel, 2);
    root.Children().Append(rightPanel);
    RegisterNamed(L"TrayPanel", rightPanel);
    RegisterNamed(L"TopBarRoot", root);

    g_rootElement = barRoot;   // Outer root includes wallpaper
    return barRoot;
}

// ============================================================================
// Monitors and the AppBar reservation
// ============================================================================

struct MonitorEnumState {
    int wanted = 0;
    int seen = 0;
    HMONITOR result = nullptr;
};

BOOL CALLBACK MonitorEnumProc(HMONITOR monitor, HDC, LPRECT, LPARAM lParam) {
    auto* state = reinterpret_cast<MonitorEnumState*>(lParam);
    state->seen++;
    if (state->seen == state->wanted) {
        state->result = monitor;
        return FALSE;
    }
    return TRUE;
}

HMONITOR GetBarMonitor() {
    if (g_settings.monitorIndex > 0) {
        MonitorEnumState state;
        state.wanted = g_settings.monitorIndex;
        EnumDisplayMonitors(nullptr, nullptr, MonitorEnumProc,
                            reinterpret_cast<LPARAM>(&state));
        if (state.result) {
            return state.result;
        }
    }
    POINT origin{0, 0};
    return MonitorFromPoint(origin, MONITOR_DEFAULTTOPRIMARY);
}

// brightness::BarMonitor was forward-declared inside that namespace so the DDC
// path could target the display the bar is actually on.
HMONITOR brightness::BarMonitor() {
    return GetBarMonitor();
}

RECT GetBarMonitorRect() {
    MONITORINFO info{sizeof(MONITORINFO)};
    HMONITOR monitor = GetBarMonitor();
    if (monitor && GetMonitorInfo(monitor, &info)) {
        return info.rcMonitor;
    }
    RECT fallback{0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)};
    return fallback;
}

double GetBarDpiScale() {
    UINT dpiX = 96, dpiY = 96;
    if (HMONITOR monitor = GetBarMonitor()) {
        if (SUCCEEDED(GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY))) {
            return dpiX / 96.0;
        }
    }
    return 1.0;
}

// Reserves the strip at the top of the work area so maximized windows stop
// below the bar instead of underneath it.
void PositionAppBar(HWND hwnd, int heightPx) {
    RECT monitorRect = GetBarMonitorRect();

    APPBARDATA data{};
    data.cbSize = sizeof(data);
    data.hWnd = hwnd;
    data.uEdge = ABE_TOP;
    data.rc.left = monitorRect.left;
    data.rc.right = monitorRect.right;
    data.rc.top = monitorRect.top;
    data.rc.bottom = monitorRect.top + heightPx;

    SHAppBarMessage(ABM_QUERYPOS, &data);
    data.rc.bottom = data.rc.top + heightPx;
    SHAppBarMessage(ABM_SETPOS, &data);

    SetWindowPos(hwnd, nullptr, data.rc.left, data.rc.top, data.rc.right - data.rc.left,
                 data.rc.bottom - data.rc.top, SWP_NOZORDER | SWP_NOACTIVATE);

    if (g_islandHwnd) {
        SetWindowPos(g_islandHwnd, nullptr, 0, 0, data.rc.right - data.rc.left,
                     data.rc.bottom - data.rc.top, SWP_NOZORDER | SWP_SHOWWINDOW);
    }
}

void RegisterAppBar(HWND hwnd) {
    APPBARDATA data{};
    data.cbSize = sizeof(data);
    data.hWnd = hwnd;
    data.uCallbackMessage = WM_APPBAR_CALLBACK;
    if (SHAppBarMessage(ABM_NEW, &data)) {
        g_appBarRegistered = true;
    }
}

void UnregisterAppBar(HWND hwnd) {
    if (!g_appBarRegistered) {
        return;
    }
    APPBARDATA data{};
    data.cbSize = sizeof(data);
    data.hWnd = hwnd;
    SHAppBarMessage(ABM_REMOVE, &data);
    g_appBarRegistered = false;
}

// ============================================================================
// Background transparency
//
// Three separate things can put an opaque plate behind the XAML content, and
// the "legacy black background" is whichever of them is still in effect:
//
//   1. the window class background brush, painted by DefWindowProc;
//   2. the DWM redirection surface, which starts out black and shows through
//      wherever the XAML content isn't fully opaque;
//   3. XAML's own island root elements, which the island creates above our
//      content and which carry a theme background of their own.
//
// All three are addressed below. The accent policy is what actually makes the
// window composite against the desktop rather than against black, and it is
// applied only when the configured bar background is itself translucent.
// ============================================================================

enum ACCENT_STATE {
    ACCENT_DISABLED = 0,
    ACCENT_ENABLE_GRADIENT = 1,
    ACCENT_ENABLE_TRANSPARENTGRADIENT = 2,
    ACCENT_ENABLE_BLURBEHIND = 3,
    ACCENT_ENABLE_ACRYLICBLURBEHIND = 4,
    ACCENT_ENABLE_HOSTBACKDROP = 5,
};

struct ACCENT_POLICY {
    ACCENT_STATE AccentState;
    DWORD AccentFlags;
    DWORD GradientColor;  // ABGR
    DWORD AnimationId;
};

enum WINDOWCOMPOSITIONATTRIB {
    WCA_ACCENT_POLICY = 19,
};

struct WINDOWCOMPOSITIONATTRIBDATA {
    WINDOWCOMPOSITIONATTRIB Attrib;
    PVOID pvData;
    SIZE_T cbData;
};

using SetWindowCompositionAttribute_t = BOOL(WINAPI*)(HWND, WINDOWCOMPOSITIONATTRIBDATA*);

SetWindowCompositionAttribute_t GetSetWindowCompositionAttribute() {
    static SetWindowCompositionAttribute_t function = [] {
        HMODULE module = GetModuleHandle(L"user32.dll");
        return module ? reinterpret_cast<SetWindowCompositionAttribute_t>(
                            GetProcAddress(module, "SetWindowCompositionAttribute"))
                      : nullptr;
    }();
    return function;
}



void ApplyBlurToWindow(HWND hwnd) {
    auto setAttribute = GetSetWindowCompositionAttribute();
    if (!setAttribute) return;

    ACCENT_POLICY policy{};
    WINDOWCOMPOSITIONATTRIBDATA data{};
    data.Attrib = WCA_ACCENT_POLICY;
    data.pvData = &policy;
    data.cbData = sizeof(policy);

    // Force disable DWM shadow (kills bottom shadow)
    BOOL disableShadow = TRUE;
    DwmSetWindowAttribute(hwnd, DWMWA_NCRENDERING_POLICY, &disableShadow, sizeof(disableShadow));

    // Always use simple blur behind (not acrylic – more reliable)
    policy.AccentState = ACCENT_ENABLE_BLURBEHIND;
    policy.AccentFlags = 0x20;   // Only blur, no tint flags
    policy.GradientColor = 0;    // Fully transparent tint

    if (!setAttribute(hwnd, &data)) {
        Wh_Log(L"Accent policy rejected; the window background will stay opaque");
    }
}

void ApplyWindowBackdrop(HWND hwnd) {
    ApplyBlurToWindow(hwnd);
}

void ApplyBlurToAllOpenPopups() {
    EnumWindows([](HWND hwnd, LPARAM) -> BOOL {
        DWORD pid = 0;
        GetWindowThreadProcessId(hwnd, &pid);
        if (pid == GetCurrentProcessId() && hwnd != g_topBarHwnd) {
            wchar_t className[256] = {0};
            GetClassName(hwnd, className, ARRAYSIZE(className));
            // Broaden the check: submenu popups and XAML islands use various class names
            if (wcsstr(className, L"Popup") || wcsstr(className, L"Xaml") ||
                wcsstr(className, L"Menu") || wcsstr(className, L"Flyout") ||
                wcsstr(className, L"ToolWindow") || wcsstr(className, L"Window")) {
                ApplyBlurToWindow(hwnd);
            }
        }
        return TRUE;
    }, 0);
}

// The island inserts its own root above whatever content we set, and those
// elements carry a theme background. Walking up from our root and clearing
// every background we find is what lets the configured translucency actually
// reach the desktop. The count is logged deliberately: if the black plate is
// still there, the number here says whether the walk found anything at all.
void StripInheritedIslandBackgrounds() {
    if (!g_rootElement) {
        return;
    }

    int cleared = 0;
    int visited = 0;
    auto transparent = MakeBrush(0, 0, 0, 0);

    DependencyObject current = g_rootElement;
    while (current) {
        DependencyObject parent{nullptr};
        try {
            parent = wuxm::VisualTreeHelper::GetParent(current);
        } catch (...) {
            break;
        }
        if (!parent) {
            break;
        }
        visited++;

        try {
            if (auto panel = parent.try_as<wuxc::Panel>()) {
                if (panel.Background()) {
                    panel.Background(transparent);
                    cleared++;
                }
            } else if (auto border = parent.try_as<wuxc::Border>()) {
                if (border.Background()) {
                    border.Background(transparent);
                    cleared++;
                }
            } else if (auto presenter = parent.try_as<wuxc::ContentPresenter>()) {
                if (presenter.Background()) {
                    presenter.Background(transparent);
                    cleared++;
                }
            } else if (auto control = parent.try_as<wuxc::Control>()) {
                if (control.Background()) {
                    control.Background(transparent);
                    cleared++;
                }
            }
        } catch (...) {
        }

        current = parent;
    }

    Wh_Log(L"Island background strip: visited %d ancestor(s), cleared %d background(s)",
           visited, cleared);
}

// ============================================================================
// Host window
// ============================================================================

void CALLBACK WindowEventProc(HWINEVENTHOOK, DWORD event, HWND hwnd, LONG idObject, LONG idChild, DWORD, DWORD) {
    if (idObject != OBJID_WINDOW || idChild != CHILDID_SELF) return;
    if (event == EVENT_OBJECT_CREATE || event == EVENT_OBJECT_DESTROY ||
        event == EVENT_OBJECT_NAMECHANGE || event == EVENT_OBJECT_SHOW || event == EVENT_OBJECT_HIDE) {
        RunOnUiThread([] { RefreshTaskList(false); });
    }
}

void UpdateClockText() {
    auto it = g_namedElements.find(L"ClockText");
    if (it == g_namedElements.end()) {
        return;
    }
    if (auto text = it->second.try_as<wuxc::TextBlock>()) {
        text.Text(winrt::hstring(FormatClockText()));
    }
}

LRESULT CALLBACK TopBarWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    if (message == g_taskbarCreatedMsg && g_taskbarCreatedMsg != 0) {
        // Explorer restarted and dropped every AppBar registration with it.
        g_appBarRegistered = false;
        RegisterAppBar(hwnd);
        PositionAppBar(hwnd, g_barHeightPx);
        return 0;
    }

    switch (message) {
        case WM_ERASEBKGND:
            // Never let DefWindowProc paint the class brush; that plate is
            // exactly the black rectangle behind the bar.
            return 1;

        case WM_APPBAR_CALLBACK:
            switch (wParam) {
                case ABN_POSCHANGED:
                case ABN_FULLSCREENAPP:
                    PositionAppBar(hwnd, g_barHeightPx);
                    break;
            }
            return 0;

        case WM_SIZE:
            if (g_islandHwnd) {
                SetWindowPos(g_islandHwnd, nullptr, 0, 0, LOWORD(lParam), HIWORD(lParam),
                             SWP_NOZORDER | SWP_SHOWWINDOW);
            }
            return 0;

        case WM_DISPLAYCHANGE:
        case WM_DPICHANGED:
            g_dpiScale = GetBarDpiScale();
            g_barHeightPx = static_cast<int>(g_settings.barHeightDip * g_dpiScale + 0.5);
            PositionAppBar(hwnd, g_barHeightPx);
            RefreshTaskList(true);
        RefreshBluetoothRadioState(); // initial radio state
            return 0;

        case WM_SETTINGCHANGE:
            // Theme switches change what the tray and the panels should look
            // like; the panels rebuild themselves when next opened, so only the
            // clock needs touching here.
            UpdateClockText();

            // Reload the wallpaper when Windows tells us the wallpaper changed.
            // The lParam will be "Wallpaper" (case-sensitive? usually it's "Wallpaper").
            if (lParam && wcscmp(reinterpret_cast<PCWSTR>(lParam), L"Wallpaper") == 0) {
                if (g_wallpaperLayer) {
                    g_wallpaperLayer.Background(GetWallpaperBrush());
                    Wh_Log(L"TopBar: Wallpaper background updated.");
                }
            }
            return 0;

        case WM_HOTKEY:
            switch (wParam) {
                case HOTKEY_ID_DISPLAY:
                    ToggleFlyout(g_displayFlyout, g_displayButton);
                    break;
                case HOTKEY_ID_SOUND:
                    ToggleFlyout(g_soundFlyout, g_soundButton);
                    break;
                case HOTKEY_ID_WIFI:
                    ToggleFlyout(g_wifiFlyout, g_wifiButton);
                    break;
                case HOTKEY_ID_BLUETOOTH:
                    ToggleFlyout(g_bluetoothFlyout, g_bluetoothButton);
                    break;
                case HOTKEY_ID_TRAY:
                    ToggleFlyout(g_trayFlyout, g_trayButton);
                    break;
                case HOTKEY_ID_START_MENU:
                    if (g_startContextMenu) {
                        auto it = g_namedElements.find(L"StartButton");
                        if (it != g_namedElements.end()) {
                            g_startContextMenu.ShowAt(it->second.as<FrameworkElement>());
                        }
                    }
                    break;
                case HOTKEY_ID_TASK_MENU:
                    if (g_taskContextMenu) {
                        auto it = g_namedElements.find(L"TaskListPanel");
                        if (it != g_namedElements.end()) {
                            g_taskContextMenu.ShowAt(it->second.as<FrameworkElement>());
                        }
                    }
                    break;
            }
            return 0;

        case WM_TIMER:
            if (wParam == kAppBarInitTimerId) {
                KillTimer(hwnd, kAppBarInitTimerId);
                PositionAppBar(hwnd, g_barHeightPx);
                return 0;
            }
            break;

        case WM_DESTROY:
            UnregisterHotKey(hwnd, HOTKEY_ID_DISPLAY);
            UnregisterHotKey(hwnd, HOTKEY_ID_SOUND);
            UnregisterHotKey(hwnd, HOTKEY_ID_WIFI);
            UnregisterHotKey(hwnd, HOTKEY_ID_BLUETOOTH);
            UnregisterHotKey(hwnd, HOTKEY_ID_TRAY);
            UnregisterAppBar(hwnd);
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProc(hwnd, message, wParam, lParam);
}

void ForegroundEventProcInstall() {
    if (g_foregroundHook) {
        return;
    }
    g_foregroundHook = SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND,
                                       nullptr, ForegroundEventProc, 0, 0,
                                       WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
    if (!g_foregroundHook) {
        Wh_Log(L"SetWinEventHook failed; click-to-minimize will not work");
    }

    // Additional hook for window creation/destruction/rename to refresh task list
    g_windowEventHook = SetWinEventHook(EVENT_OBJECT_CREATE, EVENT_OBJECT_NAMECHANGE,
                                        nullptr, WindowEventProc, 0, 0,
                                        WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
}

DWORD WINAPI TopBarThreadProc(LPVOID) {
    Wh_Log(L"TopBar: TopBarThreadProc started.");
    try {
        // No delay – start immediately.

        winrt::init_apartment(winrt::apartment_type::single_threaded);

        WNDCLASSEX windowClass{};
        windowClass.cbSize = sizeof(windowClass);
        windowClass.lpfnWndProc = TopBarWndProc;
        HMODULE modModule = nullptr;
        GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                          (LPCWSTR)&TopBarWndProc, &modModule);
        windowClass.hInstance = modModule;
        g_modModule = modModule;
        windowClass.lpszClassName = kWindowClassName;
        windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
        // No class brush at all: with one, DefWindowProc paints an opaque plate
        // before the island ever draws.
        windowClass.hbrBackground = nullptr;

        // If the class still exists from a previous failed unload, unregister it first.
        if (g_modModule) {
            UnregisterClass(kWindowClassName, g_modModule);
        }

        (void)RegisterClassEx(&windowClass);

        g_dpiScale = GetBarDpiScale();
        g_barHeightPx = static_cast<int>(g_settings.barHeightDip * g_dpiScale + 0.5);
        RECT monitorRect = GetBarMonitorRect();

        g_topBarHwnd = CreateWindowEx(
            WS_EX_TOOLWINDOW, kWindowClassName, L"Windhawk Top Bar", WS_POPUP, monitorRect.left,
            monitorRect.top, monitorRect.right - monitorRect.left, g_barHeightPx, nullptr, nullptr,
            windowClass.hInstance, nullptr);
        if (!g_topBarHwnd) {
            Wh_Log(L"CreateWindowEx failed: %u", GetLastError());
            // Clean up the window class registration
            if (g_modModule) {
                UnregisterClass(kWindowClassName, g_modModule);
                g_modModule = nullptr;
            }
            return 1;
        }

        ApplyWindowBackdrop(g_topBarHwnd);

        try {
            g_xamlManager = wuxh::WindowsXamlManager::InitializeForCurrentThread();
            // Force dark theme for the whole app
            try {
                auto app = Application::Current();
                if (app) {
                    app.RequestedTheme(ApplicationTheme::Dark);
                }
            } catch (...) {}
            g_desktopSource = wuxh::DesktopWindowXamlSource();

            auto native = g_desktopSource.as<IDesktopWindowXamlSourceNative>();
            winrt::check_hresult(native->AttachToWindow(g_topBarHwnd));
            winrt::check_hresult(native->get_WindowHandle(&g_islandHwnd));
        } catch (winrt::hresult_error const& ex) {
            // The usual cause is a host process whose manifest has no
            // <maxversiontested>; XAML Islands refuse to initialize there. This is
            // why the bar has to be hosted by a real explorer.exe.
            Wh_Log(L"XAML Islands failed to initialize: %08X - %s",
                   static_cast<unsigned int>(ex.code().value), ex.message().c_str());
            DestroyWindow(g_topBarHwnd);
            g_topBarHwnd = nullptr;
            // Clean up window class
            if (g_modModule) {
                UnregisterClass(kWindowClassName, g_modModule);
                g_modModule = nullptr;
            }
            return 1;
        }

        g_uiDispatcherQueue = winrt::Windows::System::DispatcherQueue::GetForCurrentThread();
        if (!g_uiDispatcherQueue) {
            Wh_Log(L"No DispatcherQueue on the UI thread; falling back to the XAML dispatcher");
        }

        SetWindowPos(g_islandHwnd, nullptr, 0, 0, monitorRect.right - monitorRect.left,
                     g_barHeightPx, SWP_NOZORDER | SWP_SHOWWINDOW);

        // Menu resources first: the flyouts and menus built below pick them up as
        // they are created.
        InstallGlobalMenuResources();

        auto content = BuildTopBarContent();
        g_desktopSource.Content(content);
        
        // Make the XAML island background transparent so the system blur shows through.
        auto xamlSourceUnknown = g_desktopSource.as<::IUnknown>();
        winrt::com_ptr<IXamlSourceTransparency> transparency;
        const IID kIID_IXamlSourceTransparency = {0x06636c29, 0x5a17, 0x458d, {0x8e, 0xa2, 0x24, 0x22, 0xd9, 0x97, 0xa9, 0x22}};
        if (SUCCEEDED(xamlSourceUnknown->QueryInterface(kIID_IXamlSourceTransparency, transparency.put_void())) && transparency) {
            transparency->put_IsBackgroundTransparent(TRUE);
        } else {
            Wh_Log(L"Failed to set IXamlSourceTransparency.IsBackgroundTransparent");
        }

        // After BuildTopBarContent, which clears g_namedElements -- building the
        // menus earlier would lose their registrations.
        BuildStartContextMenu();
        BuildTaskContextMenu();

        ApplyAllControlStyles();
        ApplyVisibilitySettings();

        // Once the content is live, clear whatever opaque roots the island put
        // above it.
        StripInheritedIslandBackgrounds();

        ShowWindow(g_topBarHwnd, SW_SHOWNOACTIVATE);
        UpdateWindow(g_topBarHwnd);
        // Re-apply backdrop after window becomes visible (fixes blur on top bar)
        ApplyWindowBackdrop(g_topBarHwnd);
    // Register global hotkeys (Ctrl+Alt+1..5) to open control flyouts.
        // Hotkeys are disabled by default. Ctrl+Alt+digit is a common app binding.
        // Uncomment the lines below to re-enable them.
        if (g_settings.enableHotkeys) {
            RegisterHotKey(g_topBarHwnd, HOTKEY_ID_DISPLAY, MOD_CONTROL | MOD_ALT, '1');
            RegisterHotKey(g_topBarHwnd, HOTKEY_ID_SOUND, MOD_CONTROL | MOD_ALT, '2');
            RegisterHotKey(g_topBarHwnd, HOTKEY_ID_WIFI, MOD_CONTROL | MOD_ALT, '3');
            RegisterHotKey(g_topBarHwnd, HOTKEY_ID_BLUETOOTH, MOD_CONTROL | MOD_ALT, '4');
            RegisterHotKey(g_topBarHwnd, HOTKEY_ID_TRAY, MOD_CONTROL | MOD_ALT, '5');
            RegisterHotKey(g_topBarHwnd, HOTKEY_ID_START_MENU, MOD_CONTROL | MOD_ALT, '6');
            RegisterHotKey(g_topBarHwnd, HOTKEY_ID_TASK_MENU, MOD_CONTROL | MOD_ALT, '7');
        }
        RegisterAppBar(g_topBarHwnd);
        // A moment later, so the shell has finished its own start-up layout pass
        // and doesn't immediately overwrite our reservation.
        SetTimer(g_topBarHwnd, kAppBarInitTimerId, 800, nullptr);

        ForegroundEventProcInstall();

        g_clockTimer = DispatcherTimer();
        g_clockTimer.Interval(std::chrono::seconds(1));
        g_clockTimer.Tick([](wf::IInspectable const&, wf::IInspectable const&) {
            try {
                UpdateClockText();
                UpdateBatteryButton();
            } catch (...) {
            }
        });
        g_clockTimer.Start();

        g_taskListTimer = DispatcherTimer();
        g_taskListTimer.Interval(std::chrono::milliseconds(30000)); // 30s fallback
        g_taskListTimer.Tick([](wf::IInspectable const&, wf::IInspectable const&) {
            try {
                RefreshTaskList(false);
            } catch (...) {
            }
        });
        g_taskListTimer.Start();

        // Wallpaper updates are handled by WM_SETTINGCHANGE – no timer needed.

        RefreshTaskList(true);

        // PreTranslateMessage is what gives the island keyboard input -- without it
        // the Wi-Fi password box would never see a keystroke.
        winrt::com_ptr<IDesktopWindowXamlSourceNative2> native2;
        try {
            native2 = g_desktopSource.as<IDesktopWindowXamlSourceNative2>();
        } catch (...) {
        }

        MSG message;
        while (GetMessage(&message, nullptr, 0, 0)) {
            BOOL processed = FALSE;
            if (native2) {
                native2->PreTranslateMessage(&message, &processed);
            }
            if (!processed) {
                TranslateMessage(&message);
                DispatchMessage(&message);
            }
        }

        // The topbar has been closed. Stop the foreground hook first (same thread).
        if (g_foregroundHook) {
            UnhookWinEvent(g_foregroundHook);
            g_foregroundHook = nullptr;
        }

        // The topbar has been closed. Stop all timers before the DLL unloads.
        if (g_clockTimer) g_clockTimer.Stop();

        if (g_taskListTimer) g_taskListTimer.Stop();
        if (g_wifiAutoRefreshTimer) g_wifiAutoRefreshTimer.Stop();
        if (g_bluetoothAutoRefreshTimer) g_bluetoothAutoRefreshTimer.Stop();
        if (g_taskClickTimer) g_taskClickTimer.Stop();
        if (g_volumeRevertTimer) g_volumeRevertTimer.Stop();
        if (g_brightnessRevertTimer) g_brightnessRevertTimer.Stop();

        // Release XAML and COM objects on this thread (before it exits)
        if (g_clockTimer) g_clockTimer = nullptr;

        if (g_wifiAutoRefreshTimer) g_wifiAutoRefreshTimer = nullptr;
        if (g_bluetoothAutoRefreshTimer) g_bluetoothAutoRefreshTimer = nullptr;
        if (g_taskClickTimer) g_taskClickTimer = nullptr;
        if (g_volumeRevertTimer) g_volumeRevertTimer = nullptr;
        if (g_brightnessRevertTimer) g_brightnessRevertTimer = nullptr;

        // Release wallpaper layer and other no_destroy globals
        if (g_wallpaperLayer) g_wallpaperLayer = nullptr;
        if (g_displayFlyout) g_displayFlyout = nullptr;
        if (g_trayFlyout) g_trayFlyout = nullptr;
        if (g_displayButton) g_displayButton = nullptr;
        if (g_trayButton) g_trayButton = nullptr;
        if (g_displayPanel) g_displayPanel = nullptr;
        if (g_trayPanel) g_trayPanel = nullptr;

        if (g_rootElement) {
            try {
                g_rootElement = nullptr;
            } catch (...) {}
        }
        if (g_desktopSource) {
            try {
                g_desktopSource.Content(nullptr);
            } catch (...) {}
            g_desktopSource = nullptr;
        }
        g_xamlManager = nullptr;
        g_uiDispatcherQueue = nullptr;
        g_taskListPanel = nullptr;

        // Clear maps of XAML refs
        g_namedElements.clear();
        g_taskButtonsByHwnd.clear();
        g_taskButtonLastTitle.clear();
        g_detachedStyleRoots.clear();
        g_taskContextMenu = nullptr;
        g_startContextMenu = nullptr;
        g_taskMenuToggleItem = nullptr;
        g_mediaContainer = nullptr;

        // Release COM pointers
        audio::g_cachedEndpointVolume = nullptr;
        brightness::g_cachedWmiServices = nullptr;
        tray::g_automation = nullptr;

        // Unregister the window class (now safe because thread exits)
        if (g_modModule) {
            UnregisterClass(kWindowClassName, g_modModule);
            g_modModule = nullptr;
        }

        return 0;
    } catch (...) {
        Wh_Log(L"TopBarThreadProc crashed with a C++ exception.");
        return 1;
    }
}

// ============================================================================
// Settings
// ============================================================================

void LoadSettings() {
    g_settings.barHeightDip = Wh_GetIntSetting(L"barHeight");
    if (g_settings.barHeightDip < 20) {
        g_settings.barHeightDip = 40;
    }
    
    g_settings.monitorIndex = Wh_GetIntSetting(L"monitorIndex");
    g_settings.cornerRadius = Wh_GetIntSetting(L"cornerRadius");
    // removed
    g_settings.topBarBackgroundColor = GetStringSettingCopy(L"topBarBackgroundColor");
    g_settings.topBarBackgroundOpacity = Wh_GetIntSetting(L"topBarBackgroundOpacity");
    g_settings.showStartButton = Wh_GetIntSetting(L"showStartButton") != 0;
    g_settings.showSearchButton = Wh_GetIntSetting(L"showSearchButton") != 0;
    g_settings.showTaskList = Wh_GetIntSetting(L"showTaskList") != 0;
    g_settings.taskButtonWidth = Wh_GetIntSetting(L"taskButtonWidth");
    if (g_settings.taskButtonWidth < 40) {
        g_settings.taskButtonWidth = 150;
    }
    g_settings.taskIconSize = Wh_GetIntSetting(L"taskIconSize");
    if (g_settings.taskIconSize < 8) {
        g_settings.taskIconSize = 20;
    }
    g_settings.taskButtonContent = GetStringSettingCopy(L"taskButtonContent");
    g_settings.showDisplayButton = Wh_GetIntSetting(L"showDisplayButton") != 0;
    g_settings.showSoundButton = Wh_GetIntSetting(L"showSoundButton") != 0;
    g_settings.showWifiButton = Wh_GetIntSetting(L"showWifiButton") != 0;
    g_settings.showBluetoothButton = Wh_GetIntSetting(L"showBluetoothButton") != 0;
    g_settings.showTrayButton = Wh_GetIntSetting(L"showTrayButton") != 0;
    g_settings.showBatteryButton = Wh_GetIntSetting(L"showBatteryButton") != 0;
    
    g_settings.enableHotkeys = Wh_GetIntSetting(L"enableHotkeys") != 0;
    g_settings.showClock = Wh_GetIntSetting(L"showClock") != 0;
    g_settings.timeFormat = GetStringSettingCopy(L"timeFormat");
    g_settings.showDate = Wh_GetIntSetting(L"showDate") != 0;
    g_settings.dateFormat = GetStringSettingCopy(L"dateFormat");
    g_settings.iconColor = GetStringSettingCopy(L"iconColor");
    if (g_settings.iconColor.empty()) {
        g_settings.iconColor = L"#FFFFFF";
    }

    g_styleConstants.clear();
    for (int i = 0;; i++) {
        std::wstring entry = GetStringSettingCopy(L"styleConstants[%d]", i);
        if (entry.empty()) {
            break;
        }
        size_t equals = entry.find(L'=');
        if (equals == std::wstring::npos) {
            continue;
        }
        std::wstring name = TrimWs(entry.substr(0, equals));
        std::wstring value = TrimWs(entry.substr(equals + 1));
        if (!name.empty()) {
            g_styleConstants.emplace_back(std::move(name), std::move(value));
        }
    }

    g_controlStyleRules.clear();
    for (int i = 0;; i++) {
        std::wstring target = GetStringSettingCopy(L"controlStyles[%d].target", i);
        if (target.empty()) {
            break;
        }
        ControlStyleRule rule;
        rule.target = TrimWs(target);
        for (int j = 0;; j++) {
            std::wstring style = GetStringSettingCopy(L"controlStyles[%d].styles[%d]", i, j);
            if (style.empty()) {
                break;
            }
            rule.styles.push_back(TrimWs(style));
        }
        if (!rule.styles.empty()) {
            g_controlStyleRules.push_back(std::move(rule));
        }
    }

    // Load selected theme
    g_themeStyleRules.clear();
    std::wstring theme = GetStringSettingCopy(L"theme");
    if (theme == L"GreenBar") {
        g_themeStyleRules = g_themeGreenBarStyles;
    } else if (theme == L"NoIslands") {
        g_themeStyleRules = g_themeNoIslandsStyles;
    }
}

// ============================================================================
// Tool-mod plumbing
//
// The bar lives in its own process: Windhawk relaunches the host executable
// with "-tool-mod <id>", the real entry point is hooked out, and this mod owns
// the whole process. The host has to stay explorer.exe -- XAML Islands refuse
// to initialize in a process whose manifest lacks <maxversiontested>, which is
// what made the bar silently fail to appear when the host was changed.
// ============================================================================



BOOL WhTool_ModInit() {
    Wh_Log(L"TopBar: WhTool_ModInit called.");
    LoadSettings();

    g_taskbarCreatedMsg = RegisterWindowMessage(L"TaskbarCreated");

    // Create the stop event for clean shutdown
    g_stopEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);



    g_topBarThread = CreateThread(nullptr, 0, TopBarThreadProc, nullptr, 0, &g_topBarThreadId);
    if (!g_topBarThread) {
        Wh_Log(L"Failed to create the top bar thread: %u", GetLastError());
        return FALSE;
    }
    return TRUE;
}

void WhTool_ModSettingsChanged() {
    if (!g_uiDispatcherQueue) {
        // Settings will be loaded on next start.
        return;
    }
    RunOnUiThread([] {
        LoadSettings();
        if (!g_topBarHwnd) {
            return;
        }
        g_dpiScale = GetBarDpiScale();
        g_barHeightPx = static_cast<int>(g_settings.barHeightDip * g_dpiScale + 0.5);

        // The whole tree is rebuilt: corner radius, icon colour and the task
        // button layout are all baked in at construction time.
        InstallGlobalMenuResources();
        auto content = BuildTopBarContent();
        g_desktopSource.Content(content);
        BuildStartContextMenu();
        BuildTaskContextMenu();
        ApplyAllControlStyles();
        ApplyVisibilitySettings();
        StripInheritedIslandBackgrounds();
        ApplyWindowBackdrop(g_topBarHwnd);
        PositionAppBar(g_topBarHwnd, g_barHeightPx);

        // Re-register hotkeys if the setting changed
        if (g_topBarHwnd) {
            UnregisterHotKey(g_topBarHwnd, HOTKEY_ID_DISPLAY);
            UnregisterHotKey(g_topBarHwnd, HOTKEY_ID_SOUND);
            UnregisterHotKey(g_topBarHwnd, HOTKEY_ID_WIFI);
            UnregisterHotKey(g_topBarHwnd, HOTKEY_ID_BLUETOOTH);
            UnregisterHotKey(g_topBarHwnd, HOTKEY_ID_TRAY);
            UnregisterHotKey(g_topBarHwnd, HOTKEY_ID_START_MENU);
            UnregisterHotKey(g_topBarHwnd, HOTKEY_ID_TASK_MENU);
            if (g_settings.enableHotkeys) {
                RegisterHotKey(g_topBarHwnd, HOTKEY_ID_DISPLAY, MOD_CONTROL | MOD_ALT, '1');
                RegisterHotKey(g_topBarHwnd, HOTKEY_ID_SOUND, MOD_CONTROL | MOD_ALT, '2');
                RegisterHotKey(g_topBarHwnd, HOTKEY_ID_WIFI, MOD_CONTROL | MOD_ALT, '3');
                RegisterHotKey(g_topBarHwnd, HOTKEY_ID_BLUETOOTH, MOD_CONTROL | MOD_ALT, '4');
                RegisterHotKey(g_topBarHwnd, HOTKEY_ID_TRAY, MOD_CONTROL | MOD_ALT, '5');
                RegisterHotKey(g_topBarHwnd, HOTKEY_ID_START_MENU, MOD_CONTROL | MOD_ALT, '6');
                RegisterHotKey(g_topBarHwnd, HOTKEY_ID_TASK_MENU, MOD_CONTROL | MOD_ALT, '7');
            }
        }
        RefreshTaskList(true);
    });
}

void WhTool_ModUninit() {
    InterlockedExchange(&g_shuttingDown, 1);

    // Signal the stop event so background workers can exit early.
    if (g_stopEvent) {
        SetEvent(g_stopEvent);
    }

    // Stop the UI thread first.
    if (g_topBarHwnd) {
        PostMessage(g_topBarHwnd, WM_CLOSE, 0, 0);
    }
    if (g_topBarThreadId) {
        PostThreadMessage(g_topBarThreadId, WM_QUIT, 0, 0);
    }

    // Wait for all worker threads to finish (up to 10 seconds).
    std::vector<HANDLE> handles;
    {
        std::lock_guard<std::mutex> lock(g_workerThreadsMutex);
        handles = g_workerThreads;
    }
    if (!handles.empty()) {
        WaitForMultipleObjects(static_cast<DWORD>(handles.size()), handles.data(), TRUE, 10000);
    }
    // Close all worker handles
    for (HANDLE h : handles) {
        CloseHandle(h);
    }
    g_workerThreads.clear();

    // Wait for the main UI thread to exit.
    if (g_topBarThread) {
        WaitForSingleObject(g_topBarThread, INFINITE);
        CloseHandle(g_topBarThread);
        g_topBarThread = nullptr;
    }

    // Now it's safe to close the stop event (all workers have exited).
    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }
}

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