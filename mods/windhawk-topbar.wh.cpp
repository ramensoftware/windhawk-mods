// ==WindhawkMod==
// @id              windhawk-topbar
// @name            TopBar for Windhawk
// @description     A feature-rich top taskbar hosted by a dedicated Explorer tool process
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

Adds a **second, fully independent taskbar docked to the top of the screen**, hosted by a
dedicated Explorer tool process.

[![Patreon](https://i.imgur.com/JJ0TluA.png)](https://www.patreon.com/WasiXGamer/join)

## Themes

Themes are collections of styles that can be selected from the **Theme** dropdown in the mod settings. The following themes are available:

| Theme | Preview |
|-------|---------|
| [GreenBar](https://github.com/wasixgamer/windhawk-topbar-styling-guide/tree/main/Themes/GreenBar) | [![GreenBar](https://raw.githubusercontent.com/wasixgamer/windhawk-topbar-styling-guide/main/Themes/GreenBar/screenshot.png)](https://github.com/wasixgamer/windhawk-topbar-styling-guide/tree/main/Themes/GreenBar) |
| [NoIslands](https://github.com/wasixgamer/windhawk-topbar-styling-guide/tree/main/Themes/NoIslands) | [![NoIslands](https://raw.githubusercontent.com/wasixgamer/windhawk-topbar-styling-guide/main/Themes/NoIslands/screenshot.png)](https://github.com/wasixgamer/windhawk-topbar-styling-guide/tree/main/Themes/NoIslands) |

More themes can be found and contributed from:
- **[Windhawk TopBar Styling Guide](https://github.com/wasixgamer/windhawk-topbar-styling-guide)**

## Features

- **Task list** — window icons, titles, click-to-activate, double-click maximize
- **Control centre** — Display (brightness, Night Light, Dark Mode), Sound (volume, per-app mixer, device picker, media controls), Wi-Fi (scan/connect), Bluetooth (connect/disconnect), and Tray (notification area)
- **Full styling** via Control styles
- **Background translucency** with acrylic/blur

## Process model

The bar runs inside the main Explorer process (`explorer.exe`). It creates its own window
and thread, but does not spawn a separate process. XAML Islands require a real Explorer host.

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
| `DisplayFlyoutRoot` `SoundFlyoutRoot` `WifiFlyoutRoot` `BluetoothFlyoutRoot` `TrayFlyoutRoot` | Flyout panel roots |
| `FlyoutTitle` `FlyoutListRow` `FlyoutFooterLink` `FlyoutDivider` | Shared flyout parts |
| `ExpanderHeader` / `QuickToggleTile` | Collapsible section headers, and the Display tiles |
| `SoundMuteButton` | Mute button in the Sound panel |
| `WifiPasswordBox` `WifiConnectButton` `WifiCancelButton` | The inline Wi-Fi password prompt |

More targets can be discovered with **UWPSpy** by attaching to the top bar process
(`explorer.exe -tool-mod windhawk-topbar`).

Style syntax: `Property=Value`, `Property:=<Xaml/>`, `$name` constants.
`TaskButton` also accepts `IconTintColor` and `IconTintOpacity`.

## Background transparency

Set **Bar background** to an `#AARRGGBB` colour with alpha below `FF`. The mod
swallows `WM_ERASEBKGND`, extends the DWM frame, applies an acrylic accent policy,
and strips island root backgrounds.

## Known limitations

- Tray clicks are synthesized (cursor moved to real icon).
- Night Light uses an undocumented CloudStore blob.
- Audio device switching uses undocumented `IPolicyConfig`.
- Bluetooth pairing opens Settings (needs consent UI).
- Brightness needs a WMI-capable panel or a DDC/CI monitor.
- Task list refreshes on a 5‑second timer.
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
    Select a pre‑configured theme. Styles from the theme are applied before your custom
    control styles.
- barHeightDip: 40
  $name: Bar height (DIP)
  $description: Height of the top bar in device-independent pixels.
- monitorIndex: 0
  $name: Monitor
  $description: 0 = primary monitor. Otherwise the 1-based monitor number.
- cornerRadius: 6
  $name: Corner radius
  $description: Rounded corner radius used for buttons and cards, matching WinUI 3.


- topBarBackgroundColor: "#000000"
  $name: Top bar background color
  $description: >-
    Color of the tint over the blurred wallpaper. Use 6-digit hex (#RRGGBB) or a color name (red, blue, green, etc.).
- topBarBackgroundOpacity: 50
  $name: Top bar background opacity
  $description: >-
    Opacity percentage (0 = transparent, 100 = fully opaque). Combined with the background color.
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
- showTrayButton: true
  $name: Show tray button
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
    Target a name (TopBarRoot, StartButton, SearchButton, TaskListPanel, TaskButton,
    TrayPanel, ClockText, DisplayButton, SoundButton, WifiButton, BluetoothButton,
    TrayButton, ...), a bare class name, ClassName#Name, or a parent chain with " > ".
    Styles use Property=Value or Property:=<XamlValue/>. TaskButton also accepts
    IconTintColor and IconTintOpacity to recolour task icons.
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
#include <shlwapi.h>
#include <shellscalingapi.h>
#include <objbase.h>

// Deliberately NOT including <initguid.h>: it would make every DEFINE_GUID in
// the headers below emit a definition into this translation unit, which risks
// duplicate symbols. Nothing here needs it -- the WMI/PolicyConfig CLSIDs and
// the audio property keys are declared by hand, everything else uses __uuidof.

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

#include <algorithm>
#include <chrono>
#include <functional>
#include <map>
#include <set>
#include <string>
#include <string_view>
#include <thread>
#include <mutex>
#include <vector>

using namespace winrt::Windows::UI::Xaml;
namespace wuxh = winrt::Windows::UI::Xaml::Hosting;
namespace wuxc = winrt::Windows::UI::Xaml::Controls;
namespace wuxm = winrt::Windows::UI::Xaml::Media;
namespace wf = winrt::Windows::Foundation;
namespace wui = winrt::Windows::UI;

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

// ============================================================================
// Settings
// ============================================================================

struct ControlStyleRule {
    std::wstring target;
    std::vector<std::wstring> styles;
};

struct {
    int barHeightDip = 40;
    // topBarBackgroundBlurAmount removed – not used
    std::wstring topBarBackgroundColor = L"#000000";
    int topBarBackgroundOpacity = 70;
    int monitorIndex = 0;
    int cornerRadius = 6;
    // Removed barBackground; replaced by topBarBackgroundColor and Opacity
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
    bool showClock = true;
    std::wstring timeFormat = L"🕑hh:mm tt";
    bool showDate = true;
    std::wstring dateFormat = L"📅ddd, MMM dd";
    std::wstring iconColor = L"#FFFFFF";
} g_settings;

std::vector<std::pair<std::wstring, std::wstring>> g_styleConstants;
std::vector<ControlStyleRule> g_controlStyleRules;

// The bundled look. Applied ahead of the user's own Control styles so anything
// they write for the same target overrides it. Kept in code rather than as a
// YAML default so that upgrading the mod actually picks up changes here, and so
// an existing install doesn't have to be reset to get the intended appearance.
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
wuxc::Grid g_wallpaperLayer{nullptr};  // Store the wallpaper layer for updates
std::wstring g_lastWallpaperPath;       // For change detection
DispatcherTimer g_wallpaperTimer{nullptr}; // Polling timer
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
HWINEVENTHOOK g_foregroundHook;
HWND g_lastForegroundHwnd;

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
UINT g_taskbarCreatedMsg = 0;
bool g_appBarRegistered = false;

// UWPSpy sometimes walks the XAML tree from a non-UI thread, causing a stowed
// exception (0xc000027b) in Windows.UI.Xaml.dll. This handler catches it and
// logs it instead of letting Explorer crash.


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
//
// Elements are looked up by the name given at construction (fast path), then by
// walking the live visual tree so anything actually present can be targeted,
// including elements this mod never explicitly named (auto-generated
// ContentPresenters, wrapper Borders, and so on).
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
    
    // ':root > ' prefix — next matcher must be a root element (no parent)
    std::wstring_view trimmedPart = part;
    if (trimmedPart.starts_with(L":root > ")) {
        m.rootRequired = true;
        trimmedPart = trimmedPart.substr(8);
    }
    
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
    while (pos <= target.size()) {
        size_t arrow = target.find(L" > ", pos);
        auto partSv = target.substr(
            pos, arrow == std::wstring_view::npos ? std::wstring_view::npos : arrow - pos);
        result.push_back(ParseMatcherPart(TrimWs(partSv)));
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
    if (chain.size() <= 1) {
        return true;
    }
    DependencyObject current = element;
    int chainIndex = static_cast<int>(chain.size()) - 2;
    
    while (chainIndex >= 0) {
        // Skip leading wildcards (they match any number of intermediate parents)
        if (chain[chainIndex].wildcard) {
            chainIndex--;
            continue;
        }
        
        // For ':root' constraints, just skip (handled in TestTreeMatcher)
        if (chain[chainIndex].rootRequired) {
            chainIndex--;
            continue;
        }
        
        // Walk up one parent
        current = wuxm::VisualTreeHelper::GetParent(current);
        if (!current) {
            return false;
        }
        auto fe = current.try_as<FrameworkElement>();
        if (fe && TestTreeMatcher(fe, chain[chainIndex])) {
            chainIndex--;
            continue;
        }
        
        // If there's a wildcard immediately before this matcher, try skipping
        // multiple ancestors to find a match
        if (chainIndex > 0 && chain[chainIndex - 1].wildcard) {
            DependencyObject ancestor = current;
            bool found = false;
            while (ancestor) {
                auto ancestorFe = ancestor.try_as<FrameworkElement>();
                if (ancestorFe && TestTreeMatcher(ancestorFe, chain[chainIndex])) {
                    current = ancestor;
                    chainIndex -= 2; // skip the * and this matcher
                    found = true;
                    break;
                }
                ancestor = wuxm::VisualTreeHelper::GetParent(ancestor);
            }
            if (found) {
                continue;
            }
        }
        
        return false;
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
//
// Every icon is drawn from path data rather than a font glyph. Segoe Fluent
// Icons codepoints are easy to get subtly wrong, and a wrong one ships a tofu
// box; drawn geometry also lets the whole set share one stroke weight and one
// brush, which is what makes the bar read as a single WinUI 3 system.
// ============================================================================

namespace icons {

// The exact path data from Search.svg (360x360 viewport). The original file is
// a black square with the magnifier carved out of it, plus a second path
// filling the lens interior back in -- so the *white* part the user actually
// wants is "magnifier outline minus lens", reproduced by putting both figures
// in one EvenOdd GeometryGroup. The leading 360x360 rectangle subpath of the
// original path is deliberately dropped.
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

constexpr PCWSTR kNightLightFill =
    L"M13 6 C9.7 6.5 7.2 9.4 7.2 12.9 C7.2 16.8 10.3 19.9 14.2 19.9 "
    L"C16.8 19.9 19.1 18.4 20.2 16.2 C19.5 16.5 18.6 16.6 17.8 16.6 "
    L"C14.3 16.6 11.5 13.8 11.5 10.3 C11.5 8.7 12.1 7.2 13 6 Z "
    L"M6 2.4 L6.9 4.6 L9.1 5.5 L6.9 6.4 L6 8.6 L5.1 6.4 L2.9 5.5 L5.1 4.6 Z";

constexpr PCWSTR kLockFill = L"M6.6 10.6 L17.4 10.6 L17.4 20.4 L6.6 20.4 Z";
constexpr PCWSTR kLockShackle = L"M9.2 10.6 L9.2 7.7 A2.8 2.8 0 0 1 14.8 7.7 L14.8 10.6";

constexpr PCWSTR kPlayFill = L"M8 5 L18.5 12 L8 19 Z";
constexpr PCWSTR kPauseFill = L"M8 5 L11 5 L11 19 L8 19 Z M13 5 L16 5 L16 19 L13 19 Z";
constexpr PCWSTR kPrevFill = L"M17 5 L17 19 L7.5 12 Z M6 5 L8 5 L8 19 L6 19 Z";
constexpr PCWSTR kNextFill = L"M7 5 L7 19 L16.5 12 Z M16 5 L18 5 L18 19 L16 19 Z";

constexpr PCWSTR kHeadphoneStroke = L"M4.6 15.2 L4.6 12 A7.4 7.4 0 0 1 19.4 12 L19.4 15.2";
constexpr PCWSTR kHeadphoneFill =
    L"M3 14.4 L6.6 14.4 L6.6 20.2 L3 20.2 Z M17.4 14.4 L21 14.4 L21 20.2 L17.4 20.2 Z";

constexpr PCWSTR kAppFill = L"M5 5 L19 5 L19 19 L5 19 Z";
constexpr PCWSTR kCheckStroke = L"M5 12.5 L10 17.5 L19 6.5";

}  // namespace icons
// Windows 11 Start logo: 4 blue rounded squares in a 2x2 grid.
// Drawn from XAML Rectangle elements so the color is fixed to the
// official blue regardless of the theme or the icon color setting.
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
// Builds <Viewbox><Grid><Path fill/><Path stroke/></Grid></Viewbox>. Either
// path may be empty. Everything is one XamlReader parse so the geometry
// mini-language is handled by XAML itself rather than by hand.
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

// The search glyph needs the two Search.svg figures combined with an EvenOdd
// fill rule and a translate on the second one.
//
// WinRT XAML's PathGeometry has no string mini-language on its Figures
// property, so the two path strings are parsed by handing them to throwaway
// Path elements -- Path.Data *does* accept the mini-language -- and their
// resulting Geometry objects are then re-parented into a GeometryGroup built in
// code. (Credit: this is the user's fix.)
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
// Crisp task icons
//
// The icon is pulled from the owning executable at the exact physical pixel
// size it will be drawn at, using PrivateExtractIconsW, which picks the
// best-matching image out of the icon group the same way Explorer does. Only if
// that fails do we fall back to the window's own icon handles.
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

// Renders a HICON into a fixed-size square canvas via DrawIconEx, then hands
// the result to XAML as an in-memory 32bpp BMP. An earlier attempt used
// SoftwareBitmap + IMemoryBufferByteAccess, which needs <MemoryBuffer.h> -- not
// shipped by this toolchain, and its absence corrupted the rest of the
// translation unit's parsing.
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

    // Many classic icons carry no real alpha in the colour plane (the alpha
    // byte stays 0 everywhere even though the icon isn't actually invisible);
    // DrawIconEx already composited the mask for us, so in that case treat
    // near-black as transparent and everything else as opaque. Icons that do
    // have real alpha get premultiplied so edges don't show a dark fringe.
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
// Window enumeration / task list
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

// SetForegroundWindow is refused for processes that don't own the foreground.
// Briefly attaching to the current foreground thread's input queue lifts that
// restriction, which is the standard workaround and is what makes clicking a
// task button actually raise the window from a separate process.
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

// Clicking a task button activates the bar, so by the time the handler runs
// GetForegroundWindow() is our own window and "is this window already focused?"
// can never be true -- that is exactly why minimize-on-click never fired.
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

// Icon and text live in their own Grid columns, so the icon size is
// independent of the label and icon-only / text-only modes just collapse the
// column they don't need.
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

[[clang::no_destroy]] winrt::com_ptr<IWbemServices> g_cachedWmiServices;

winrt::com_ptr<IWbemServices> ConnectWmi() {
    if (g_cachedWmiServices) {
        return g_cachedWmiServices;
    }

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
    g_cachedWmiServices = services;
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
    return g_lastKnown >= 0 ? g_lastKnown : Get();
}

void Set(int percent) {
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
// Dark mode / Night Light
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

// Night Light has no public API at all. Its state lives in a CloudStore blob
// whose layout is undocumented: a 2-byte "enabled" marker sits at offset 18 and
// is simply absent when it's off. Every third-party toggle does this same
// insert/remove. The blob is sanity-checked before writing and the original is
// put back if the write fails, so a format change on a future build degrades to
// "the toggle does nothing" rather than corrupting the value.
namespace nightlight {

constexpr PCWSTR kStateKey =
    L"Software\\Microsoft\\Windows\\CurrentVersion\\CloudStore\\Store\\DefaultAccount\\Current\\"
    L"default$windows.data.bluelightreduction.bluelightreductionstate\\"
    L"windows.data.bluelightreduction.bluelightreductionstate";

bool ReadBlob(std::vector<uint8_t>* out) {
    HKEY key = nullptr;
    if (RegOpenKeyEx(HKEY_CURRENT_USER, kStateKey, 0, KEY_READ, &key) != ERROR_SUCCESS) {
        return false;
    }
    DWORD type = 0;
    DWORD size = 0;
    bool ok = RegQueryValueEx(key, L"Data", nullptr, &type, nullptr, &size) == ERROR_SUCCESS &&
              type == REG_BINARY && size > 0;
    if (ok) {
        out->resize(size);
        ok = RegQueryValueEx(key, L"Data", nullptr, nullptr, out->data(), &size) ==
             ERROR_SUCCESS;
        out->resize(size);
    }
    RegCloseKey(key);
    return ok;
}

bool WriteBlob(const std::vector<uint8_t>& data) {
    HKEY key = nullptr;
    if (RegOpenKeyEx(HKEY_CURRENT_USER, kStateKey, 0, KEY_SET_VALUE, &key) != ERROR_SUCCESS) {
        return false;
    }
    bool ok = RegSetValueEx(key, L"Data", 0, REG_BINARY, data.data(),
                            static_cast<DWORD>(data.size())) == ERROR_SUCCESS;
    RegCloseKey(key);
    return ok;
}

// A blob we recognise: version byte 0x02 and long enough to hold the marker.
bool LooksValid(const std::vector<uint8_t>& data) {
    return data.size() >= 25 && data[0] == 0x02;
}

bool Supported() {
    std::vector<uint8_t> data;
    return ReadBlob(&data) && LooksValid(data);
}

bool IsEnabled() {
    std::vector<uint8_t> data;
    if (!ReadBlob(&data) || !LooksValid(data)) {
        return false;
    }
    return data[18] == 0x10 && data[19] == 0x00;
}

bool SetEnabled(bool enabled) {
    std::vector<uint8_t> data;
    if (!ReadBlob(&data) || !LooksValid(data)) {
        return false;
    }

    bool currentlyEnabled = data[18] == 0x10 && data[19] == 0x00;
    if (currentlyEnabled == enabled) {
        return true;
    }

    std::vector<uint8_t> original = data;
    if (enabled) {
        data.insert(data.begin() + 18, {0x10, 0x00});
    } else {
        data.erase(data.begin() + 18, data.begin() + 20);
    }

    if (!WriteBlob(data)) {
        WriteBlob(original);
        return false;
    }
    return true;
}

bool Toggle() {
    return SetEnabled(!IsEnabled());
}

}  // namespace nightlight

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
};

bool Available() {
    // Only check if the API exists, NOT if the radio handle exists.
    // This keeps the toggle enabled when the radio is off, so you can turn it back on.
    return GetApi().valid();
}

bool IsRadioOn() {
#if TOPBAR_HAS_RADIOS
    try {
        auto radios = winrt::Windows::Devices::Radios::Radio::GetRadiosAsync().get();
        for (auto&& radio : radios) {
            if (radio.Kind() == winrt::Windows::Devices::Radios::RadioKind::Bluetooth) {
                return radio.State() == winrt::Windows::Devices::Radios::RadioState::On;
            }
        }
    } catch (...) {
    }
#endif
    // Fall back to "a radio device exists and answers": not the same thing as
    // the software switch, but the best available without the WinRT API.
    return Available();
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

// "Connect" and "disconnect" are the same call with a different state flag:
// every service the device has installed is turned on or off. Blocking, so it
// belongs on a worker thread.
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

// Set while a panel is writing its own controls, so the ValueChanged /Toggled
// handlers that XAML raises during construction don't get mistaken for the user
// actually moving something.
bool g_populatingPanel = false;

// Non-zero once the mod starts tearing down; background workers check it before
// touching XAML or dispatching back to the UI thread.
volatile LONG g_shuttingDown = 0;
volatile LONG g_backgroundJobs = 0;
volatile LONG g_crashedOnce = 0;

// (Crash flag mechanism removed – no registry key needed)

bool IsCrashFlagSet() {
    // Crash flag disabled: a transient failure should not permanently disable the mod.
    return false;
}

void SetCrashFlag(bool value) {
    // Crash flag disabled: a transient failure should not permanently disable the mod.
    (void)value;
}

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
std::vector<std::thread> g_backgroundThreads;
std::mutex g_backgroundMutex;

void RunInBackground(std::function<void()> work) {
    if (InterlockedCompareExchange(&g_shuttingDown, 0, 0) != 0) {
        return;
    }
    std::thread t([work = std::move(work)]() {
        HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
        try {
            work();
        } catch (...) {
        }
        if (SUCCEEDED(hr)) {
            CoUninitialize();
        }
    });
    std::lock_guard<std::mutex> lock(g_backgroundMutex);
    g_backgroundThreads.push_back(std::move(t));
}

// Panels tear themselves down from inside their own click handlers, which would
// destroy the very button that is still dispatching. Rebuilding on the next
// dispatcher turn avoids that.
void RepopulateLater(const std::function<void()>& populate) {
    RunOnUiThread(populate);
}

wuxm::SolidColorBrush FlyoutBackgroundBrush() {
    return MakeBrush(0xF2, 0x20, 0x20, 0x20);
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

// The square accent tile used for Night light / Dark mode: filled when on,
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

    bool nightLightOn = nightlight::IsEnabled();
    bool darkMode = IsAppsDarkMode();

    // Directly toggle Night Light, no settings page fallback!
    // Compact, side-by-side buttons
    auto nightTile = MakeGhostButton(L"QuickToggleTile", kTileCorner);
    nightTile.HorizontalAlignment(HorizontalAlignment::Stretch);
    nightTile.Padding(Thickness{8, 6, 8, 6}); // Much smaller padding = shorter button
    nightTile.Background(nightLightOn ? MakeBrush(0xFF, 0x4C, 0x8E, 0xE0) : MakeBrush(0x18, 0xFF, 0xFF, 0xFF));

    wuxc::StackPanel nightStack;
    nightStack.Orientation(wuxc::Orientation::Horizontal);
    nightStack.Spacing(8);
    if (auto icon = BuildVectorIcon(nullptr, icons::kNightLightFill, L"", 24, 16, 1.7, L"#FFFFFF")) {
        icon.VerticalAlignment(VerticalAlignment::Center);
        nightStack.Children().Append(icon);
    }
    nightStack.Children().Append(MakeText(nullptr, L"Night light", 12));
    nightTile.Content(nightStack);
    nightTile.Click([](auto&&, auto&&) {
        if (!nightlight::Toggle()) { Wh_Log(L"Night light toggle failed"); }
        RepopulateLater(PopulateDisplayPanel);
    });

    auto darkTile = MakeGhostButton(L"QuickToggleTile", kTileCorner);
    darkTile.HorizontalAlignment(HorizontalAlignment::Stretch);
    darkTile.Padding(Thickness{8, 6, 8, 6});
    darkTile.Background(darkMode ? MakeBrush(0xFF, 0x4C, 0x8E, 0xE0) : MakeBrush(0x18, 0xFF, 0xFF, 0xFF));

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
    children.Append(MakeTileRow(nightTile, darkTile));

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
        audio::SetMasterVolume(value);
        RefreshSoundButtonIcon();
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
    if (!connected) {
        return BuildVectorIcon(nullptr, icons::kWifiDot, icons::kWifiStroke, 24, size, 1.7);
    }
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
    wifiProgress.Foreground(MakeBrush(0xFF, 0x03, 0x6B, 0xD7));
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
    toggle.MinWidth(0);
    toggle.Width(32);
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
    g_bluetoothScanning = includeUnpaired;
    RunInBackground([includeUnpaired] {
        auto devices = bluetooth::Enumerate(includeUnpaired);
        RunOnUiThread([devices = std::move(devices)]() mutable {
            g_bluetoothDevices = std::move(devices);
            g_bluetoothScanning = false;
            PopulateBluetoothPanel();
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
    btProgress.Foreground(MakeBrush(0xFF, 0x03, 0x6B, 0xD7));
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
    toggle.MinWidth(0);
    toggle.Width(32);
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
            bluetooth::SetRadio(on);
            Sleep(600);
            auto devices = on ? bluetooth::Enumerate(false)
                              : std::vector<bluetooth::Device>{};
            RunOnUiThread([devices = std::move(devices)]() mutable {
                g_bluetoothDevices = std::move(devices);
                RefreshBluetoothButtonIcon();
                PopulateBluetoothPanel();
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
    if (!g_trayPanel) {
        return;
    }

    g_populatingPanel = true;
    struct Guard {
        ~Guard() { g_populatingPanel = false; }
    } guard;

    auto children = g_trayPanel.Children();
    children.Clear();

    children.Append(MakePanelTitle(L"System tray"));

    auto items = tray::Enumerate();
    if (items.empty()) {
        children.Append(MakeStatusText(
            L"No tray icons were found. The Windows taskbar has to be running and visible "
            L"for its notification icons to be readable."));
    }

    for (const auto& item : items) {
        FrameworkElement leading{nullptr};
        if (auto bitmap = CaptureScreenRect(item.bounds)) {
            wuxc::Image image;
            image.Source(bitmap);
            image.Width(18);
            image.Height(18);
            image.Stretch(wuxm::Stretch::Uniform);
            leading = image;
        } else {
            leading = BuildVectorIcon(nullptr, icons::kAppFill, L"", 24, 16, 1.5);
        }

        tray::Item captured = item;
        auto row = MakeListRow(leading, item.name, L"", nullptr, [captured] {
            HideAllFlyouts();
            tray::InvokeItem(captured);
        });

        // Right-click forwards to the app's own tray menu, which is the only
        // menu the app itself defines.
        row.RightTapped([captured](auto&&, Input::RightTappedRoutedEventArgs const& args) {
            args.Handled(true);
            HideAllFlyouts();
            tray::ShowItemContextMenu(captured);
        });

        children.Append(row);
    }

    children.Append(MakeDivider());
    children.Append(
        MakeSettingsLink(L"Taskbar notification settings", L"ms-settings:taskbar"));
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

        auto hover = MakeBrush(0x24, 0xFF, 0xFF, 0xFF);
        auto pressed = MakeBrush(0x14, 0xFF, 0xFF, 0xFF);

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

        set(L"MenuFlyoutPresenterBackground", FlyoutBackgroundBrush());
        set(L"MenuFlyoutPresenterBorderBrush", MakeBrush(0x30, 0xFF, 0xFF, 0xFF));

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
        auto hover = MakeBrush(0x24, 0xFF, 0xFF, 0xFF);
        auto pressed = MakeBrush(0x14, 0xFF, 0xFF, 0xFF);
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
// Bar layout
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
                    audio::SetMasterVolume(newVolume);
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
                    int newBrightness = std::clamp(brightness::GetFast() + step * direction, 0, 100);
                    RunInBackground([newBrightness] { brightness::Set(newBrightness); });
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
        g_wifiAutoRefreshTimer.Interval(std::chrono::seconds(5));
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
        g_bluetoothAutoRefreshTimer.Interval(std::chrono::seconds(10));
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
        wallpaperLayer.Opacity(1); // Slightly transparent so the top bar tint shows
        barRoot.Children().Append(wallpaperLayer);
        g_wallpaperLayer = wallpaperLayer; // Save reference
        g_wallpaperLayer = wallpaperLayer; // Save reference for updates
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
        g_displayFlyout, [] { PopulateDisplayPanel(); });
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
            g_wifiNetworks = wifi::EnumerateNetworks();
            g_wifiPasswordPrompt = false;
            g_wifiPromptError.clear();
            PopulateWifiPanel();
            StartWifiScan();
            EnsureAutoRefreshTimers();
            if (g_wifiAutoRefreshTimer) {
                g_wifiAutoRefreshTimer.Start();
            }
        });
        rightPanel.Children().Append(g_wifiButton);
    }

    {
        auto icon = BuildVectorIcon(nullptr, L"", icons::kBluetoothStroke, 24, 18, 1.8);
    g_bluetoothButton = MakeControlButton(L"BluetoothButton", icon, g_bluetoothFlyout, [] {
        g_bluetoothDevices = bluetooth::Enumerate(false);
        PopulateBluetoothPanel();
        EnsureAutoRefreshTimers();
        if (g_bluetoothAutoRefreshTimer) {
            g_bluetoothAutoRefreshTimer.Start();
        }
    });
        rightPanel.Children().Append(g_bluetoothButton);
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

bool WantsTranslucentBar() {
    // Always return true because the WindhawkBlur brush on TopBarRoot handles the tint
    return true;
}

void ApplyWindowBackdrop(HWND hwnd) {
    if (!WantsTranslucentBar()) {
        return;
    }



    auto setAttribute = GetSetWindowCompositionAttribute();
    if (!setAttribute) {
        Wh_Log(L"SetWindowCompositionAttribute unavailable; the bar will not be translucent");
        return;
    }
    // Force disable DWM shadow completely (this kills the bottom shadow)
    BOOL disableShadow = TRUE;
    DwmSetWindowAttribute(hwnd, DWMWA_NCRENDERING_POLICY, &disableShadow, sizeof(disableShadow));
    ACCENT_POLICY policy{};
    // Acrylic gives the frosted look; it falls back to a plain blur on builds
    // where the acrylic state isn't honoured.
    policy.AccentState = ACCENT_ENABLE_ACRYLICBLURBEHIND;
    policy.AccentFlags = 0x20 | 0x40 | 0x80 | 0x100;  // draw all four borders
    // Use a fully transparent gradient color so the accent policy doesn't add its own tint.
    policy.GradientColor = 0;

    WINDOWCOMPOSITIONATTRIBDATA data{};
    data.Attrib = WCA_ACCENT_POLICY;
    data.pvData = &policy;
    data.cbData = sizeof(policy);
    if (!setAttribute(hwnd, &data)) {
        policy.AccentState = ACCENT_ENABLE_BLURBEHIND;
        if (!setAttribute(hwnd, &data)) {
            Wh_Log(L"Accent policy rejected; the bar background will stay opaque");
        }
    }
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

void ForegroundEventProcInstall();

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
}

DWORD WINAPI TopBarThreadProc(LPVOID) {
    try {
        // Wait for the stop event (or timeout). If stop is requested, exit now.
        if (g_stopEvent) {
            if (WaitForSingleObject(g_stopEvent, 5000) == WAIT_OBJECT_0) {
                return 1;  // shutdown requested before we start
            }
        } else {
            Sleep(5000);
        }

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
    // Register global hotkeys (Ctrl+Alt+1..5) to open control flyouts.
        // Hotkeys are disabled by default. Ctrl+Alt+digit is a common app binding.
        // Uncomment the lines below to re-enable them.
        // RegisterHotKey(g_topBarHwnd, HOTKEY_ID_DISPLAY, MOD_CONTROL | MOD_ALT, '1');
        // RegisterHotKey(g_topBarHwnd, HOTKEY_ID_SOUND, MOD_CONTROL | MOD_ALT, '2');
        // RegisterHotKey(g_topBarHwnd, HOTKEY_ID_WIFI, MOD_CONTROL | MOD_ALT, '3');
        // RegisterHotKey(g_topBarHwnd, HOTKEY_ID_BLUETOOTH, MOD_CONTROL | MOD_ALT, '4');
        // RegisterHotKey(g_topBarHwnd, HOTKEY_ID_TRAY, MOD_CONTROL | MOD_ALT, '5');
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
            } catch (...) {
            }
        });
        g_clockTimer.Start();

        g_taskListTimer = DispatcherTimer();
        g_taskListTimer.Interval(std::chrono::milliseconds(5000)); // 5s fallback
        g_taskListTimer.Tick([](wf::IInspectable const&, wf::IInspectable const&) {
            try {
                RefreshTaskList(false);
            } catch (...) {
            }
        });
        g_taskListTimer.Start();

        // Start wallpaper polling timer (every 2 seconds)
        g_wallpaperTimer = DispatcherTimer();
        g_wallpaperTimer.Interval(std::chrono::seconds(2));
        g_wallpaperTimer.Tick([](wf::IInspectable const&, wf::IInspectable const&) {
            try {
                UpdateWallpaperIfChanged();
            } catch (...) {
            }
        });
        g_wallpaperTimer.Start();

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
        if (g_wallpaperTimer) g_wallpaperTimer.Stop();
        if (g_taskListTimer) g_taskListTimer.Stop();
        if (g_wifiAutoRefreshTimer) g_wifiAutoRefreshTimer.Stop();
        if (g_bluetoothAutoRefreshTimer) g_bluetoothAutoRefreshTimer.Stop();
        if (g_taskClickTimer) g_taskClickTimer.Stop();
        if (g_volumeRevertTimer) g_volumeRevertTimer.Stop();
        if (g_brightnessRevertTimer) g_brightnessRevertTimer.Stop();

        // Release XAML and COM objects on this thread (before it exits)
        if (g_clockTimer) g_clockTimer = nullptr;
        if (g_wallpaperTimer) g_wallpaperTimer = nullptr;
        if (g_wifiAutoRefreshTimer) g_wifiAutoRefreshTimer = nullptr;
        if (g_bluetoothAutoRefreshTimer) g_bluetoothAutoRefreshTimer = nullptr;
        if (g_taskClickTimer) g_taskClickTimer = nullptr;
        if (g_volumeRevertTimer) g_volumeRevertTimer = nullptr;
        if (g_brightnessRevertTimer) g_brightnessRevertTimer = nullptr;

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
    g_settings.barHeightDip = Wh_GetIntSetting(L"barHeightDip");
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
        RefreshTaskList(true);
    });
}

void WhTool_ModUninit() {
    InterlockedExchange(&g_shuttingDown, 1);

    // Signal the stop event so background workers can exit early.
    if (g_stopEvent) {
        SetEvent(g_stopEvent);
    }

    // Join all background threads **before** the UI thread, to avoid races.
    std::vector<std::thread> threads;
    {
        std::lock_guard<std::mutex> lock(g_backgroundMutex);
        threads.swap(g_backgroundThreads);
    }
    for (auto& t : threads) {
        if (t.joinable()) {
            t.join();
        }
    }

    // Now stop the UI thread.
    if (g_topBarHwnd) {
        PostMessage(g_topBarHwnd, WM_CLOSE, 0, 0);
    }
    if (g_topBarThreadId) {
        PostThreadMessage(g_topBarThreadId, WM_QUIT, 0, 0);
    }

    if (g_topBarThread) {
        WaitForSingleObject(g_topBarThread, INFINITE);
        CloseHandle(g_topBarThread);
        g_topBarThread = nullptr;
    }

    // Close the stop event handle
    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }
}

// ============================================================================
// Regular mod entry points (run in the main Explorer process)
// These call the tool-mod functions so the same logic is used either way.
// ============================================================================

BOOL Wh_ModInit() {
    WhTool_ModInit();
    return TRUE;
}

void Wh_ModAfterInit() {
    // No extra actions needed; the thread is already created in WhTool_ModInit.
}

void Wh_ModSettingsChanged() {
    WhTool_ModSettingsChanged();
}

void Wh_ModUninit() {
    WhTool_ModUninit();
}


