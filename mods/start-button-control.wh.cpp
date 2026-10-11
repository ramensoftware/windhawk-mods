// ==WindhawkMod==
// @id              start-button-control
// @name            Windows 11 Start Button Launcher
// @description     Built-in icons, custom Start launcher and position among app icons
// @version         1.3.2
// @author          Stheno
// @github          https://github.com/sth3no
// @license         GPL-3.0
// @include         explorer.exe
// @include         StartMenuExperienceHost.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lshell32 -lshlwapi -lcomctl32
// ==/WindhawkMod==

// Derived from m417z's Start button always on the left (GPL-3.0).
// https://github.com/ramensoftware/windhawk-mods/blob/main/mods/taskbar-start-button-position.wh.cpp
// Original launcher, icon presets and slot positioning by Stheno, with ChatGPT assistance.
// Source code is published under The GNU General Public License v3.0.

// ==WindhawkModReadme==
/*
# Windows 11 Start Button Launcher

<!-- public-previews:start -->

![Illustrated preview](https://raw.githubusercontent.com/sth3no/windhawk-mods/0cd1ef48ff138199e1bfc3be7c3d9d1da8e26503/previews/start-button-control/cover.png)

*Illustration; example glass styling is configured with Taskbar Styler.*

<!-- public-previews:end -->

Make the Start button your own: choose a clean built-in icon, launch your favorite
app, and place Start among your taskbar app icons.

## Features

- Eight bundled vector icons: Home, Tiles, Apps, Spark, Orbit, Bolt, Command and
  Compass. No separate image downloads or icon fonts are required.
- Theme-aware icon color, custom hex colors, and 16–32 logical pixel sizing.
- Custom PNG icons and click targets: executables, shortcuts, folders or registered
  URIs. Environment variables such as `%LOCALAPPDATA%` are supported.
- Start before, between or after app icons, or after a fixed number of app icons.
- Optional hide mode and a separate layout with Start at the left screen edge.
- Optional blocking of Start activation by the Windows key and Ctrl+Esc.

## Quick start

1. Keep **Mode → Normal (icons, launcher and position enabled)**.
2. Choose **Icon → Home**, or another preset. **Automatic** uses your configured
   PNG when present and otherwise keeps the Windows logo.
3. Set **Start button launch target** to your app's executable or shortcut. For
   Raycast, use its installed shortcut or executable. Put command-line arguments
   in **Launch arguments**, separately from the target path.
4. Choose **Start icon position in the dock**. **Custom slot → 3**, for example,
   places Start after three app icons. Leave the launch target empty for native Start.

Custom icon paths must be absolute. A missing or undecodable PNG leaves the native
icon visible. Selecting a built-in icon keeps your saved PNG path for later use.
The custom launch action applies to pointer clicks; it does not remap the Windows key.

## Taskbar Styler and compatibility

Designed for the native Windows 11 x64 taskbar. Local release testing uses Windows
11 25H2, build 26200.9457, Windhawk 1.7.3, and Taskbar Styler 1.10 with FrostyGlass.
Other Windows builds and themes have not been exhaustively tested.
ARM64 taskbar offsets are decoded using the upstream instruction pattern; ARM64
hardware has not been tested locally.

Taskbar Styler can continue to control backgrounds, corners and taskbar dimensions.
Normal mode leaves Start visibility and margins alone. A Styler rule that hides
Start will also hide this launcher. Icon replacement owns the native icon's opacity
while active; do not style that same property from another mod at the same time.
On unload, the mod restores the local values or bindings it replaced when they
still match its own override.

Do not combine position modes with another Start-position mod. The middle position
is a slot based on app count, not exact screen center with variable-width labels.
Start itself cannot be dragged like a pinned app. Position selection applies only
in Normal mode. The overlay does not implement a replacement right-click menu.
Moving the button does not move the native Start menu popup.

Use only one mod to replace the Start icon or intercept its click. In particular,
disable this mod's icon replacement when using Start Button Replacer or a native
Start icon colorizer. Use only one mod to block the Windows key / Ctrl+Esc.

This mod does not add glass styling; configure that in
[Windows 11 Taskbar Styler](https://windhawk.net/mods/windows-11-taskbar-styler).

## Relationship to other mods

This mod combines a custom click launcher, eight bundled vector icons and an
adjustable slot among app buttons. These are its main additions.

For a single existing feature, consider
[Hide Start Button](https://windhawk.net/mods/hide-start-button),
[Start button always on the left](https://windhawk.net/mods/taskbar-start-button-position),
[Start Button Replacer](https://windhawk.net/mods/start-button-replacer),
[Start button colorizer](https://windhawk.net/mods/taskbar-start-button-colorizer), or
[Disable Windows Shortcuts](https://windhawk.net/mods/disable-windows-shortcuts).
The optional Hide/Left modes, custom PNG and Start-blocking option overlap with
these focused mods. They are off by default. Built-in icon color changes this mod's
vector artwork, rather than recoloring the original Windows glyph.

## Credits

By [Stheno](https://github.com/sth3no), developed with ChatGPT assistance.
The taskbar layout and XAML access code derives from m417z's
[Start button always on the left](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/taskbar-start-button-position.wh.cpp).
Distributed under GPL-3.0.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- mode: off
  $name: Mode
  $options:
    - off: Normal (icons, launcher and position enabled)
    - hide: Hide Start button
    - left: Start button left, icons centered
- iconPosition: native
  $name: Start icon position in the dock
  $description: Applies in Normal mode. Center means the middle slot among app icons; left/right mean before/after app icons.
  $options:
    - native: Native Windows order
    - left: Before app icons
    - center: Middle of app icons
    - right: After app icons
    - custom: Custom slot
- iconSlot: 0
  $name: App icons before Start (custom slot)
  $description: 0 puts Start before the first app. Larger values are clamped to the current app count. This is separate from Windows pinned-app drag ordering.
- iconPreset: auto
  $name: Icon
  $description: Built-in vector icons work without any image files. Automatic preserves an existing custom PNG, otherwise the Windows logo. Changing the icon does not change the launch target.
  $options:
    - auto: Automatic (custom PNG or Windows logo)
    - home: Home
    - tiles: Tiles
    - apps: Apps
    - spark: Spark
    - orbit: Orbit
    - bolt: Bolt
    - command: Command
    - compass: Compass
    - native: Windows logo
    - custom: Custom PNG
- iconColor: auto
  $name: Built-in icon color
  $description: auto follows the Windows theme. Or use a hex color such as "#FFFFFF", "#75CFFF" or "#CCFFFFFF" (alpha first). Only affects built-in icons.
- iconSize: 24
  $name: Icon size
  $description: 16 to 32 logical pixels. Applies to built-in icons and custom PNGs, without changing the button's hit area.
- customIconPath: ""
  $name: Custom icon (PNG)
  $description: Absolute PNG path. Environment variables are supported. Blank keeps the Windows icon.
- launchTarget: ""
  $name: Start button launch target
  $description: Executable, shortcut, folder or registered URI. Blank keeps the native Start action. For Raycast select its installed shortcut or executable.
- launchArguments: ""
  $name: Launch arguments
  $description: Separate arguments for the executable. Do not put arguments in the target path.
- disableWindowsKeyStartMenu: false
  $name: Disable Start menu
  $description: >-
    Prevent opening Start from the Windows key, Ctrl+Esc, or the Start button
    while keeping normal Windows-key shortcuts such as Win+R working
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <atomic>
#include <functional>
#include <limits>
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <string>
#include <string_view>
#include <memory>
#include <vector>
#include <shellapi.h>
#include <shlwapi.h>
#include <commctrl.h>

#include <roapi.h>
#include <winstring.h>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Shapes.h>
#include <winrt/Windows.UI.Xaml.Media.Imaging.h>
#include <winrt/Windows.UI.Xaml.Markup.h>
#include <winrt/Windows.UI.Xaml.Data.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/base.h>

using namespace winrt::Windows::UI::Xaml;

enum class Mode {
    Off,
    Hide,
    Left,
};

enum class Target {
    Explorer,
    StartMenuExperienceHost,
};

namespace {

constexpr double kWidgetOffsetLeft = 44.0;

Target g_target = Target::Explorer;
std::atomic<int> g_mode = static_cast<int>(Mode::Off);
std::atomic<bool> g_disableWindowsKeyStartMenu;
std::atomic<bool> g_taskbarViewDllLoaded;
std::atomic<bool> g_unloading;

thread_local bool g_inArrangeOverride;
enum class IconPosition { Native, Left, Center, Right, Custom };
IconPosition g_iconPosition = IconPosition::Native;
int g_iconSlot = 0;
thread_local std::vector<winrt::weak_ref<FrameworkElement>> g_repeaters;
thread_local std::unordered_map<void*, double> g_orderOffsets;
void ScheduleTaskbarUpdate();
void RemoveTaskbarSubclasses();
thread_local bool g_applyingSettings = false;

HHOOK g_lowLevelKeyboardHook;
struct WindowsKeyState { bool down = false; bool injected = false; };
WindowsKeyState g_windowsKeys[2];
bool g_blockedEscape = false;

HANDLE g_keyboardThread = nullptr;
HANDLE g_keyboardReadyEvent = nullptr;
HANDLE g_keyboardStopEvent = nullptr;
std::atomic<bool> g_keyboardInitialized = false;

void* g_ctaskBandTaskListWndSiteVftable;
void* g_csecondaryTaskBandTaskListWndSiteVftable;
void* g_taskbarHostFrameHeight;

using CTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void* pThis, void** result);
CTaskBand_GetTaskbarHost_t g_ctaskBandGetTaskbarHost;

using CSecondaryTaskBand_GetTaskbarHost_t =
    void*(WINAPI*)(void* pThis, void** result);
CSecondaryTaskBand_GetTaskbarHost_t g_csecondaryTaskBandGetTaskbarHost;

using std__Ref_count_base__Decref_t = void(WINAPI*)(void* pThis);
std__Ref_count_base__Decref_t g_refCountBaseDecref;

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t g_loadLibraryExWOriginal;

using RoGetActivationFactory_t = decltype(&RoGetActivationFactory);
RoGetActivationFactory_t g_roGetActivationFactoryOriginal;

using IUIElement_Arrange_t =
    HRESULT(WINAPI*)(void* pThis, winrt::Windows::Foundation::Rect rect);
IUIElement_Arrange_t g_iuiElementArrangeOriginal;

using TaskbarCollapsibleLayoutXamlTraits_ArrangeOverride_t =
    HRESULT(WINAPI*)(void* pThis,
                     void* context,
                     winrt::Windows::Foundation::Size size,
                     winrt::Windows::Foundation::Size* resultSize);
TaskbarCollapsibleLayoutXamlTraits_ArrangeOverride_t
    g_taskbarCollapsibleLayoutArrangeOverrideOriginal;

using ExperienceToggleButton_UpdateButtonPadding_t =
    void(WINAPI*)(void* pThis);
ExperienceToggleButton_UpdateButtonPadding_t
    g_experienceToggleButtonUpdateButtonPaddingOriginal;

using AugmentedEntryPointButton_UpdateButtonPadding_t =
    void(WINAPI*)(void* pThis);
AugmentedEntryPointButton_UpdateButtonPadding_t
    g_augmentedEntryPointButtonUpdateButtonPaddingOriginal;

Mode GetMode() {
    return static_cast<Mode>(g_mode.load(std::memory_order_relaxed));
}

void SetMode(Mode mode) {
    g_mode.store(static_cast<int>(mode), std::memory_order_relaxed);
}

bool IsWindowsKeyStartMenuDisabled() {
    return g_disableWindowsKeyStartMenu.load(std::memory_order_relaxed);
}

Mode ParseModeSetting(PCWSTR value) {
    if (!value || !*value || wcscmp(value, L"off") == 0) {
        return Mode::Off;
    }

    if (wcscmp(value, L"hide") == 0) {
        return Mode::Hide;
    }

    if (wcscmp(value, L"left") == 0) {
        return Mode::Left;
    }

    Wh_Log(L"Unknown mode setting: %s", value);
    return Mode::Off;
}

HWND FindCurrentProcessTaskbarWnd() {
    HWND taskbarWnd = nullptr;

    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            DWORD processId = 0;
            WCHAR className[32];

            if (GetWindowThreadProcessId(hWnd, &processId) &&
                processId == GetCurrentProcessId() &&
                GetClassName(hWnd, className, ARRAYSIZE(className)) &&
                _wcsicmp(className, L"Shell_TrayWnd") == 0) {
                *reinterpret_cast<HWND*>(lParam) = hWnd;
                return FALSE;
            }

            return TRUE;
        },
        reinterpret_cast<LPARAM>(&taskbarWnd));

    return taskbarWnd;
}

FrameworkElement EnumChildElements(
    FrameworkElement element,
    std::function<bool(FrameworkElement)> enumCallback) {
    if (!element) {
        return nullptr;
    }

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

FrameworkElement FindChildByClassName(FrameworkElement element,
                                      PCWSTR className) {
    return EnumChildElements(element, [className](FrameworkElement child) {
        return winrt::get_class_name(child) == className;
    });
}

bool IsStartButton(FrameworkElement element) {
    if (!element ||
        winrt::get_class_name(element) != L"Taskbar.ExperienceToggleButton") {
        return false;
    }

    auto automationId = Automation::AutomationProperties::GetAutomationId(
        element);
    if (automationId != L"StartButton") {
        return false;
    }

    auto name = element.Name();
    return name.empty() || name == L"LaunchListButton";
}

FrameworkElement FindTaskbarFrameRepeater(XamlRoot xamlRoot) {
    FrameworkElement rootContent = xamlRoot.Content().try_as<FrameworkElement>();
    FrameworkElement child = rootContent;

    if (child &&
        (child = FindChildByClassName(child, L"Taskbar.TaskbarFrame")) &&
        (child = FindChildByName(child, L"RootGrid")) &&
        (child = FindChildByName(child, L"TaskbarFrameRepeater"))) {
        return child;
    }

    return nullptr;
}

FrameworkElement FindStartButton(FrameworkElement taskbarFrameRepeater) {
    return EnumChildElements(taskbarFrameRepeater, [](FrameworkElement child) {
        return IsStartButton(child);
    });
}

FrameworkElement FindWidgetButton(FrameworkElement taskbarFrameRepeater) {
    return EnumChildElements(taskbarFrameRepeater, [](FrameworkElement child) {
        if (winrt::get_class_name(child) !=
                L"Taskbar.AugmentedEntryPointButton" ||
            child.Name() != L"AugmentedEntryPointButton") {
            return false;
        }

        auto margin = child.Margin();
        auto offset = child.ActualOffset();
        return offset.x == margin.Left && offset.y == 0;
    });
}

std::wstring g_customIconPath, g_launchTarget, g_launchArguments;
std::wstring g_iconPreset = L"auto", g_iconColor = L"auto";
int g_iconSize = 24;

// Original, font-independent silhouettes on a 24 x 24 canvas. Keep these paths
// self-contained: the mod catalog distributes this single source file.
struct BuiltinIcon {
    PCWSTR id;
    PCWSTR label;
    PCWSTR path;
};
constexpr BuiltinIcon kBuiltinIcons[] = {
    {L"home", L"Home", L"M10.7 3.1 Q12 2 13.3 3.1 L21.3 10.1 Q22.1 10.8 21.4 11.6 Q20.8 12.3 20 11.6 L19.5 11.2 V19 Q19.5 21 17.5 21 H14.5 V14.8 Q14.5 14 13.7 14 H10.3 Q9.5 14 9.5 14.8 V21 H6.5 Q4.5 21 4.5 19 V11.2 L4 11.6 Q3.2 12.3 2.6 11.6 Q1.9 10.8 2.7 10.1 Z"},
    {L"tiles", L"Tiles", L"M5 3 H8.5 Q10.5 3 10.5 5 V8.5 Q10.5 10.5 8.5 10.5 H5 Q3 10.5 3 8.5 V5 Q3 3 5 3 Z M15.5 3 H19 Q21 3 21 5 V8.5 Q21 10.5 19 10.5 H15.5 Q13.5 10.5 13.5 8.5 V5 Q13.5 3 15.5 3 Z M5 13.5 H8.5 Q10.5 13.5 10.5 15.5 V19 Q10.5 21 8.5 21 H5 Q3 21 3 19 V15.5 Q3 13.5 5 13.5 Z M15.5 13.5 H19 Q21 13.5 21 15.5 V19 Q21 21 19 21 H15.5 Q13.5 21 13.5 19 V15.5 Q13.5 13.5 15.5 13.5 Z"},
    {L"apps", L"Apps", L"M3 5 A2 2 0 1 0 7 5 A2 2 0 1 0 3 5 Z M10 5 A2 2 0 1 0 14 5 A2 2 0 1 0 10 5 Z M17 5 A2 2 0 1 0 21 5 A2 2 0 1 0 17 5 Z M3 12 A2 2 0 1 0 7 12 A2 2 0 1 0 3 12 Z M10 12 A2 2 0 1 0 14 12 A2 2 0 1 0 10 12 Z M17 12 A2 2 0 1 0 21 12 A2 2 0 1 0 17 12 Z M3 19 A2 2 0 1 0 7 19 A2 2 0 1 0 3 19 Z M10 19 A2 2 0 1 0 14 19 A2 2 0 1 0 10 19 Z M17 19 A2 2 0 1 0 21 19 A2 2 0 1 0 17 19 Z"},
    {L"spark", L"Spark", L"M11.2 2.7 Q12 1.3 12.8 2.7 L15 8.3 Q15.2 8.8 15.7 9 L21.3 11.2 Q22.7 12 21.3 12.8 L15.7 15 Q15.2 15.2 15 15.7 L12.8 21.3 Q12 22.7 11.2 21.3 L9 15.7 Q8.8 15.2 8.3 15 L2.7 12.8 Q1.3 12 2.7 11.2 L8.3 9 Q8.8 8.8 9 8.3 Z"},
    {L"orbit", L"Orbit", L"M19.1 9.6 A7.5 7.5 0 1 1 14.4 4.9 L15.1 3 A9.5 9.5 0 1 0 21 8.9 Z M16.5 5.2 A2.3 2.3 0 1 0 21.1 5.2 A2.3 2.3 0 1 0 16.5 5.2 Z M10 12 A2 2 0 1 0 14 12 A2 2 0 1 0 10 12 Z"},
    {L"bolt", L"Bolt", L"M13.8 2.3 Q14.6 1.4 14.4 2.8 L13.5 9 H19.5 Q20.7 9 19.9 10 L10.2 21.7 Q9.4 22.6 9.6 21.2 L10.5 15 H4.5 Q3.3 15 4.1 14 Z"},
    {L"command", L"Command", L"M5.5 2.5 A3 3 0 0 0 5.5 8.5 H7 V15.5 H5.5 A3 3 0 1 0 8.5 18.5 V17 H15.5 V18.5 A3 3 0 1 0 18.5 15.5 H17 V8.5 H18.5 A3 3 0 1 0 15.5 5.5 V7 H8.5 V5.5 A3 3 0 0 0 5.5 2.5 Z M5.5 4 A1.5 1.5 0 0 1 7 5.5 V7 H5.5 A1.5 1.5 0 0 1 5.5 4 Z M17 5.5 A1.5 1.5 0 1 1 18.5 7 H17 Z M8.5 8.5 H15.5 V15.5 H8.5 Z M5.5 17 H7 V18.5 A1.5 1.5 0 1 1 5.5 17 Z M17 17 H18.5 A1.5 1.5 0 1 1 17 18.5 Z"},
    {L"compass", L"Compass", L"M12 2 A10 10 0 1 0 12 22 A10 10 0 1 0 12 2 Z M12 4 A8 8 0 1 1 12 20 A8 8 0 1 1 12 4 Z M16.4 7.1 Q17.4 6.6 16.9 7.6 L13.8 13.8 L7.6 16.9 Q6.6 17.4 7.1 16.4 L10.2 10.2 Z M12 10.8 A1.2 1.2 0 1 0 12 13.2 A1.2 1.2 0 1 0 12 10.8 Z"},
};

const BuiltinIcon* FindBuiltinIcon(std::wstring_view id) {
    for (auto const& icon : kBuiltinIcons) {
        if (id == icon.id) return &icon;
    }
    return nullptr;
}

bool IsIconHexColor(std::wstring_view value) {
    if ((value.size() != 7 && value.size() != 9) || value[0] != L'#') return false;
    return std::all_of(value.begin() + 1, value.end(), [](wchar_t c) {
        return (c >= L'0' && c <= L'9') || (c >= L'a' && c <= L'f') ||
               (c >= L'A' && c <= L'F');
    });
}

FrameworkElement CreateBuiltinIcon(const BuiltinIcon& preset) {
    // Only static path data and a validated hex color enter this XAML string.
    // ThemeResource follows light/dark and high contrast changes automatically.
    const std::wstring color = g_iconColor == L"auto"
        ? L"{ThemeResource SystemControlForegroundBaseHighBrush}" : g_iconColor;
    const std::wstring xaml =
        L"<Viewbox xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/presentation\" "
        L"Stretch=\"Uniform\"><Canvas Width=\"24\" Height=\"24\"><Path Fill=\"" +
        color + L"\" Data=\"F0 " + preset.path + L"\" /></Canvas></Viewbox>";
    auto icon = Markup::XamlReader::Load(xaml).as<FrameworkElement>();
    icon.Width(g_iconSize);
    icon.Height(g_iconSize);
    icon.HorizontalAlignment(HorizontalAlignment::Center);
    icon.VerticalAlignment(VerticalAlignment::Center);
    icon.IsHitTestVisible(false);
    return icon;
}

std::wstring ExpandSetting(PCWSTR key) {
    PCWSTR raw = Wh_GetStringSetting(key);
    std::wstring value = raw ? raw : L"";
    Wh_FreeStringSetting(raw);
    DWORD size = ExpandEnvironmentStringsW(value.c_str(), nullptr, 0);
    if (size) {
        std::wstring expanded(size, L'\0');
        DWORD written = ExpandEnvironmentStringsW(value.c_str(), expanded.data(), size);
        if (written && written <= size) {
            expanded.resize(written - 1);
            return expanded;
        }
    }
    return value;
}

std::wstring IconFileUri(std::wstring const& path) {
    if (path.empty() || PathIsRelativeW(path.c_str())) return {};
    // UrlCreateFromPath escapes spaces, # and % and handles UNC paths. A raw
    // file:/// prefix would interpret a # in the filename as a URI fragment.
    DWORD capacity = static_cast<DWORD>(path.size() * 3 + 128);
    std::wstring uri(capacity, L'\0');
    if (FAILED(UrlCreateFromPathW(path.c_str(), uri.data(), &capacity, 0))) return {};
    uri.resize(wcslen(uri.c_str()));
    return uri;
}

template<typename T>
struct PropertyOverride {
    bool owned = false;
    winrt::Windows::Foundation::IInspectable original{nullptr};
    Data::Binding binding{nullptr};
    T applied{};

    void Set(FrameworkElement element, DependencyProperty property, T value) {
        if (winrt::unbox_value<T>(element.GetValue(property)) == value) return;
        if (!owned) {
            original = element.ReadLocalValue(property);
            if (auto expression = element.GetBindingExpression(property)) binding = expression.ParentBinding();
            owned = true;
        }
        applied = value;
        element.SetValue(property, winrt::box_value(value));
    }

    void Restore(FrameworkElement element, DependencyProperty property) {
        if (!owned) return;
        owned = false;
        // A later Styler update owns its replacement. Preserve bindings and
        // style inheritance instead of restoring a hard-coded effective value.
        if (element.GetBindingExpression(property) ||
            element.ReadLocalValue(property) == DependencyProperty::UnsetValue() ||
            winrt::unbox_value<T>(element.GetValue(property)) != applied) return;
        if (binding) element.SetBinding(property, binding);
        else if (original == DependencyProperty::UnsetValue()) element.ClearValue(property);
        else element.SetValue(property, original);
    }
};

struct ButtonState {
    winrt::weak_ref<FrameworkElement> element;
    PropertyOverride<Visibility> visibility;
    PropertyOverride<bool> hitTest;
    PropertyOverride<Thickness> margin;
    Controls::Grid panel{nullptr};
    FrameworkElement overlay{nullptr};
    winrt::event_token clickToken{};
    Controls::Image customIcon{nullptr};
    winrt::event_token imageOpenedToken{};
    FrameworkElement nativeIcon{nullptr};
    std::shared_ptr<PropertyOverride<double>> iconOpacity;

};
thread_local std::vector<ButtonState> g_buttonStates;

void RestoreLauncher(ButtonState& state) {
    try {
    if (state.customIcon && state.imageOpenedToken) {
        state.customIcon.ImageOpened(state.imageOpenedToken);
    }
    } catch (...) { Wh_Log(L"Image event cleanup: %08X", winrt::to_hresult()); }
    state.imageOpenedToken = {};
    try {
    if (auto button = state.overlay.try_as<Controls::Button>(); button && state.clickToken) {
        button.Click(state.clickToken);
    }
    } catch (...) { Wh_Log(L"Click event cleanup: %08X", winrt::to_hresult()); }
    state.clickToken = {};
    if (state.nativeIcon && state.iconOpacity) {
        try { state.iconOpacity->Restore(state.nativeIcon, UIElement::OpacityProperty()); }
        catch (...) { Wh_Log(L"Icon restoration: %08X", winrt::to_hresult()); }
    }
    try { if (state.panel && state.overlay) {
        uint32_t index;
        if (state.panel.Children().IndexOf(state.overlay, index)) state.panel.Children().RemoveAt(index);
    } } catch (...) { Wh_Log(L"Overlay cleanup: %08X", winrt::to_hresult()); }
    state.panel = nullptr; state.overlay = nullptr; state.customIcon = nullptr;
    state.nativeIcon = nullptr; state.iconOpacity.reset();
}

ButtonState& TrackButton(FrameworkElement element) {
    std::erase_if(g_buttonStates, [](auto& state) {
        if (state.element.get()) return false;
        try { RestoreLauncher(state); }
        catch (...) { Wh_Log(L"Detached launcher cleanup: %08X", winrt::to_hresult()); }
        return true;
    });
    for (auto& state : g_buttonStates) {
        if (state.element.get() == element) return state;
    }
    g_buttonStates.push_back({winrt::make_weak(element)});
    return g_buttonStates.back();
}

void RestoreButtonStates() {
    for (auto& state : g_buttonStates) {
        try {
            RestoreLauncher(state);
            if (auto element = state.element.get()) {
                state.visibility.Restore(element, UIElement::VisibilityProperty());
                state.hitTest.Restore(element, UIElement::IsHitTestVisibleProperty());
                state.margin.Restore(element, FrameworkElement::MarginProperty());
            }
        } catch (...) { Wh_Log(L"Start restoration: %08X", winrt::to_hresult()); }
    }
    g_buttonStates.clear();
}

void ApplyLauncher(FrameworkElement startButton, ButtonState& state) {
    const auto* preset = FindBuiltinIcon(g_iconPreset);
    const bool usePng = (g_iconPreset == L"auto" || g_iconPreset == L"custom") &&
                        !g_customIconPath.empty();
    if (!preset && !usePng && g_launchTarget.empty()) return;
    auto panel = FindChildByName(startButton, L"ExperienceToggleButtonRootPanel")
                     .try_as<Controls::Grid>();
    if (!panel) return;
    if (state.overlay && state.panel == panel) return;
    if (state.overlay) RestoreLauncher(state);  // Template rebuilt on the same Start control.
    FrameworkElement icon{nullptr};
    Controls::Image png{nullptr};
    Media::Imaging::BitmapImage bitmap{nullptr};
    if (preset) {
        try { icon = CreateBuiltinIcon(*preset); }
        catch (...) { Wh_Log(L"Cannot create built-in icon: %08X", winrt::to_hresult()); }
    } else if (usePng) {
        // Verify the path before starting XAML image decoding.
        DWORD attributes = GetFileAttributesW(g_customIconPath.c_str());
        if (attributes != INVALID_FILE_ATTRIBUTES && !(attributes & FILE_ATTRIBUTE_DIRECTORY)) {
            try {
                png = Controls::Image();
                bitmap = Media::Imaging::BitmapImage();
                std::wstring uri = IconFileUri(g_customIconPath);
                if (uri.empty()) throw winrt::hresult_invalid_argument();
                bitmap.UriSource(winrt::Windows::Foundation::Uri(uri));
                // Attach ImageOpened before assigning Source, including cached images.
                png.Width(g_iconSize); png.Height(g_iconSize);
                png.Stretch(Media::Stretch::Uniform);
                png.HorizontalAlignment(HorizontalAlignment::Center);
                png.VerticalAlignment(VerticalAlignment::Center);
                icon = png;
            } catch (...) { icon = nullptr; png = nullptr; bitmap = nullptr; Wh_Log(L"Invalid icon: %08X", winrt::to_hresult()); }
        } else Wh_Log(L"Cannot open custom icon: %lu", GetLastError());
    }
    if (icon) {
        state.nativeIcon = FindChildByName(panel, L"Icon");
        if (state.nativeIcon) {
            state.iconOpacity = std::make_shared<PropertyOverride<double>>();
            auto nativeIcon = state.nativeIcon;
            if (png) {
                state.customIcon = png;
                state.imageOpenedToken = png.ImageOpened([nativeIcon, opacity = state.iconOpacity](auto const&, auto const&) {
                    if (!g_unloading) {
                        try { opacity->Set(nativeIcon, UIElement::OpacityProperty(), 0.0); }
                        catch (...) { Wh_Log(L"Icon replacement: %08X", winrt::to_hresult()); }
                    }
                });
            }
        }
    }
    FrameworkElement overlay{nullptr};
    if (!g_launchTarget.empty()) {
        Controls::Button button;
        button.Name(L"WindhawkStartLauncher");
        button.Padding(Thickness{0});
        button.BorderThickness(Thickness{0});
        button.Background(Media::SolidColorBrush(winrt::Windows::UI::Colors::Transparent()));
        button.HorizontalAlignment(HorizontalAlignment::Stretch);
        button.VerticalAlignment(VerticalAlignment::Stretch);
        if (icon) button.Content(icon);
        Automation::AutomationProperties::SetName(button, L"Open custom launcher");
        state.clickToken = button.Click([](auto const&, RoutedEventArgs const&) {
            if (g_unloading || g_launchTarget.empty()) return;
            SHELLEXECUTEINFOW info{sizeof(info)};
            info.fMask = SEE_MASK_ASYNCOK;
            info.lpVerb = L"open";
            info.lpFile = g_launchTarget.c_str();
            info.lpParameters = g_launchArguments.empty() ? nullptr : g_launchArguments.c_str();
            info.nShow = SW_SHOWNORMAL;
            if (!ShellExecuteExW(&info)) Wh_Log(L"Launcher failed: %lu", GetLastError());
        });
        overlay = button;
    } else if (icon) {
        icon.IsHitTestVisible(false);
        overlay = icon;
    }
    if (overlay) {
        Controls::Grid::SetColumnSpan(overlay, 99);
        Controls::Grid::SetRowSpan(overlay, 99);
        panel.Children().Append(overlay);
        state.panel = panel; state.overlay = overlay;
        if (png) png.Source(bitmap);
        else if (icon && state.nativeIcon) state.iconOpacity->Set(state.nativeIcon, UIElement::OpacityProperty(), 0.0);
    }
}

bool ApplyModeToTaskbarXamlRoot(XamlRoot xamlRoot) {
    if (g_applyingSettings) return false;
    struct Guard { Guard() { g_applyingSettings = true; } ~Guard() { g_applyingSettings = false; } } guard;
    auto repeater = FindTaskbarFrameRepeater(xamlRoot);
    if (!repeater) return false;
    bool known = false;
    for (auto const& weak : g_repeaters) if (weak.get() == repeater) known = true;
    if (!known) g_repeaters.push_back(winrt::make_weak(repeater));
    bool unloading = g_unloading.load(std::memory_order_relaxed);
    if (unloading) {
        RestoreButtonStates();
        g_orderOffsets.clear();
        for (auto const& weak : g_repeaters) if (auto element = weak.get()) element.InvalidateArrange();
        g_repeaters.clear();
        return true;
    }
    Mode mode = GetMode();
    if (auto start = FindStartButton(repeater)) {
        auto& state = TrackButton(start);
        if (mode == Mode::Hide) {
            state.visibility.Set(start, UIElement::VisibilityProperty(), Visibility::Collapsed);
        }
        if (IsWindowsKeyStartMenuDisabled() && g_launchTarget.empty()) {
            state.hitTest.Set(start, UIElement::IsHitTestVisibleProperty(), false);
        }
        if (mode == Mode::Left) {
            auto margin = start.Margin();
            double width = start.ActualWidth();
            if (FindWidgetButton(repeater)) margin.Right = -width;
            else {
                double minX = std::numeric_limits<double>::infinity();
                EnumChildElements(repeater, [&](FrameworkElement child) {
                    if (child != start && child.ActualOffset().x >= 0)
                        minX = std::min(minX, static_cast<double>(child.ActualOffset().x));
                    return false;
                });
                if (minX < width) margin.Right = 0;
                else if (minX > width * 2) margin.Right = -width;
            }
            state.margin.Set(start, FrameworkElement::MarginProperty(), margin);
        }
        ApplyLauncher(start, state);
    }
    if (mode == Mode::Left) {
        if (auto widget = FindWidgetButton(repeater)) {
            auto& state = TrackButton(widget);
            auto margin = widget.Margin(); margin.Left = kWidgetOffsetLeft;
            state.margin.Set(widget, FrameworkElement::MarginProperty(), margin);
        }
    }
    // Native app changes already arrange the repeater and rebuild the slot plan.
    // Invalidating on every padding notification can create an endless layout loop.
    if (!known && g_iconPosition != IconPosition::Native && mode == Mode::Off)
        repeater.InvalidateArrange();
    return true;
}

bool TryGetTaskbarElementOffset(size_t& offset) {
    if (!g_taskbarHostFrameHeight) return false;
#if defined(_M_X64)
    {
        const BYTE* bytes = static_cast<const BYTE*>(g_taskbarHostFrameHeight);
        if (bytes[0] == 0x48 && bytes[1] == 0x83 && bytes[2] == 0xEC &&
            bytes[4] == 0x48 && bytes[5] == 0x83 && bytes[6] == 0xC1 &&
            bytes[7] <= 0x7F) {
            offset = bytes[7];
        } else {
            return false;
        }
    }
#elif defined(_M_ARM64)
    {
        // Match the reference mod's ARM64 prologue: pacibsp, stp, mov, ldr.
        const DWORD* p = static_cast<const DWORD*>(g_taskbarHostFrameHeight);
        if (p[0] == 0xD503237F && (p[1] & 0xFFC07FFF) == 0xA9807BFD &&
            p[2] == 0x910003FD && (p[3] & 0xFFF00FE0) == 0xF8400C00) {
            offset = (p[3] >> 12) & 0xFF;
        } else {
            return false;
        }
    }
#else
#error "Unsupported architecture"
#endif
    return true;
}

XamlRoot XamlRootFromTaskbarHostSharedPtr(void* taskbarHostSharedPtr[2]) {
    if (!taskbarHostSharedPtr[0]) {
        if (taskbarHostSharedPtr[1]) g_refCountBaseDecref(taskbarHostSharedPtr[1]);
        return nullptr;
    }

    size_t taskbarElementIUnknownOffset = 0;
    if (!TryGetTaskbarElementOffset(taskbarElementIUnknownOffset)) {
        Wh_Log(L"Unsupported TaskbarHost::FrameHeight");
        if (taskbarHostSharedPtr[1]) g_refCountBaseDecref(taskbarHostSharedPtr[1]);
        return nullptr;
    }

    auto* taskbarElementIUnknown = *reinterpret_cast<IUnknown**>(
        static_cast<BYTE*>(taskbarHostSharedPtr[0]) +
        taskbarElementIUnknownOffset);
    if (!taskbarElementIUnknown) {
        if (taskbarHostSharedPtr[1]) g_refCountBaseDecref(taskbarHostSharedPtr[1]);
        return nullptr;
    }

    FrameworkElement taskbarElement = nullptr;
    taskbarElementIUnknown->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                           winrt::put_abi(taskbarElement));

    if (taskbarHostSharedPtr[1]) {
        g_refCountBaseDecref(taskbarHostSharedPtr[1]);
    }

    return taskbarElement ? taskbarElement.XamlRoot() : nullptr;
}

XamlRoot GetTaskbarXamlRoot(HWND taskbarWnd) {
    HWND taskSwWnd = reinterpret_cast<HWND>(
        GetProp(taskbarWnd, L"TaskbandHWND"));
    if (!taskSwWnd) {
        return nullptr;
    }

    void* taskBand = reinterpret_cast<void*>(GetWindowLongPtr(taskSwWnd, 0));
    if (!taskBand) return nullptr;
    void* taskBandForTaskListWndSite = taskBand;

    for (int i = 0; *reinterpret_cast<void**>(taskBandForTaskListWndSite) !=
                        g_ctaskBandTaskListWndSiteVftable;
         i++) {
        if (i == 20) {
            return nullptr;
        }

        taskBandForTaskListWndSite =
            static_cast<void**>(taskBandForTaskListWndSite) + 1;
    }

    void* taskbarHostSharedPtr[2]{};
    g_ctaskBandGetTaskbarHost(taskBandForTaskListWndSite, taskbarHostSharedPtr);
    return XamlRootFromTaskbarHostSharedPtr(taskbarHostSharedPtr);
}

XamlRoot GetSecondaryTaskbarXamlRoot(HWND secondaryTaskbarWnd) {
    HWND taskSwWnd = FindWindowEx(secondaryTaskbarWnd, nullptr, L"WorkerW",
                                  nullptr);
    if (!taskSwWnd) {
        return nullptr;
    }

    void* taskBand = reinterpret_cast<void*>(GetWindowLongPtr(taskSwWnd, 0));
    if (!taskBand) return nullptr;
    void* taskBandForTaskListWndSite = taskBand;

    for (int i = 0; *reinterpret_cast<void**>(taskBandForTaskListWndSite) !=
                        g_csecondaryTaskBandTaskListWndSiteVftable;
         i++) {
        if (i == 20) {
            return nullptr;
        }

        taskBandForTaskListWndSite =
            static_cast<void**>(taskBandForTaskListWndSite) + 1;
    }

    void* taskbarHostSharedPtr[2]{};
    g_csecondaryTaskBandGetTaskbarHost(taskBandForTaskListWndSite,
                                       taskbarHostSharedPtr);
    return XamlRootFromTaskbarHostSharedPtr(taskbarHostSharedPtr);
}

using RunFromWindowThreadProc_t = void(WINAPI*)(void* parameter);

bool RunFromWindowThread(HWND hWnd,
                         RunFromWindowThreadProc_t proc,
                         void* procParam) {
    static const UINT kRegisteredMsg =
        RegisterWindowMessage(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);

    struct RunFromWindowThreadParam {
        RunFromWindowThreadProc_t proc;
        void* procParam;
    };

    DWORD threadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (!threadId) {
        return false;
    }

    if (threadId == GetCurrentThreadId()) {
        proc(procParam);
        return true;
    }

    HHOOK hook = SetWindowsHookEx(
        WH_CALLWNDPROC,
        [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (nCode == HC_ACTION) {
                const auto* cwp = reinterpret_cast<const CWPSTRUCT*>(lParam);
                if (cwp->message == kRegisteredMsg) {
                    auto* param =
                        reinterpret_cast<RunFromWindowThreadParam*>(cwp->lParam);
                    param->proc(param->procParam);
                }
            }

            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        },
        nullptr, threadId);
    if (!hook) {
        return false;
    }

    RunFromWindowThreadParam param{
        .proc = proc,
        .procParam = procParam,
    };
    SendMessage(hWnd, kRegisteredMsg, 0, reinterpret_cast<LPARAM>(&param));
    UnhookWindowsHookEx(hook);

    return true;
}

namespace StartMenuBlock {

winrt::event_token g_visibilityChangedToken;
winrt::event_token g_layoutUpdatedToken;

HWND GetCoreWnd() {
    struct EnumWindowsParam {
        HWND* hWnd;
    };

    HWND hWnd = nullptr;
    EnumWindowsParam param{
        .hWnd = &hWnd,
    };

    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            auto& param = *reinterpret_cast<EnumWindowsParam*>(lParam);

            DWORD processId = 0;
            if (!GetWindowThreadProcessId(hWnd, &processId) ||
                processId != GetCurrentProcessId()) {
                return TRUE;
            }

            WCHAR className[32];
            if (!GetClassName(hWnd, className, ARRAYSIZE(className))) {
                return TRUE;
            }

            if (_wcsicmp(className, L"Windows.UI.Core.CoreWindow") == 0) {
                *param.hWnd = hWnd;
                return FALSE;
            }

            return TRUE;
        },
        reinterpret_cast<LPARAM>(&param));

    return hWnd;
}

void HideStartMenu() {
    if (!IsWindowsKeyStartMenuDisabled()) {
        return;
    }

    HWND hWnd = GetCoreWnd();
    if (!hWnd) {
        return;
    }

    ShowWindow(hWnd, SW_HIDE);
}

void Init() {
    auto window = Window::Current();
    if (!window) {
        return;
    }

    if (!g_visibilityChangedToken) {
        g_visibilityChangedToken = window.VisibilityChanged(
            [](winrt::Windows::Foundation::IInspectable const&,
               winrt::Windows::UI::Core::VisibilityChangedEventArgs const&
                   args) {
                if (args.Visible()) {
                    HideStartMenu();
                }
            });
    }

    auto content = window.Content().try_as<FrameworkElement>();
    if (content && !g_layoutUpdatedToken) {
        g_layoutUpdatedToken = content.LayoutUpdated(
            [](winrt::Windows::Foundation::IInspectable const&,
               winrt::Windows::Foundation::IInspectable const&) {
                HideStartMenu();
            });
    }

    HideStartMenu();
}

void Uninit() {
    auto window = Window::Current();
    if (!window) {
        return;
    }

    if (g_visibilityChangedToken) {
        window.VisibilityChanged(g_visibilityChangedToken);
        g_visibilityChangedToken = {};
    }

    auto content = window.Content().try_as<FrameworkElement>();
    if (content && g_layoutUpdatedToken) {
        content.LayoutUpdated(g_layoutUpdatedToken);
        g_layoutUpdatedToken = {};
    }
}

HRESULT WINAPI RoGetActivationFactory_Hook(HSTRING activatableClassId,
                                           REFIID iid,
                                           void** factory) {
    thread_local static bool isInHook;

    if (isInHook) {
        return g_roGetActivationFactoryOriginal(activatableClassId, iid,
                                                factory);
    }

    isInHook = true;

    if (wcscmp(WindowsGetStringRawBuffer(activatableClassId, nullptr),
               L"Windows.UI.Xaml.Hosting.XamlIsland") == 0) {
        try {
            Init();
        } catch (...) {
            Wh_Log(L"StartMenuBlock init failed: %08X", winrt::to_hresult());
        }
    }

    HRESULT result =
        g_roGetActivationFactoryOriginal(activatableClassId, iid, factory);

    isInHook = false;
    return result;
}

}  // namespace StartMenuBlock

void ApplySettingsFromTaskbarThread() {
    EnumThreadWindows(
        GetCurrentThreadId(),
        [](HWND hWnd, LPARAM) -> BOOL {
            WCHAR className[32];
            if (!GetClassName(hWnd, className, ARRAYSIZE(className))) {
                return TRUE;
            }

            try {
            XamlRoot xamlRoot = nullptr;
            if (_wcsicmp(className, L"Shell_TrayWnd") == 0) {
                xamlRoot = GetTaskbarXamlRoot(hWnd);
            } else if (_wcsicmp(className, L"Shell_SecondaryTrayWnd") == 0) {
                xamlRoot = GetSecondaryTaskbarXamlRoot(hWnd);
            } else {
                return TRUE;
            }

                if (xamlRoot) ApplyModeToTaskbarXamlRoot(xamlRoot);
            } catch (...) { Wh_Log(L"Taskbar settings: %08X", winrt::to_hresult()); }

            return TRUE;
        },
        0);
}

thread_local std::vector<HWND> g_subclassedTaskbars;
thread_local std::unordered_map<HWND, bool> g_pendingUpdates;
UINT TaskbarUpdateMessage() {
    static const UINT message = RegisterWindowMessageW(L"Windhawk_StartUpdate_" WH_MOD_ID);
    return message;
}
LRESULT CALLBACK TaskbarUpdateSubclass(HWND window, UINT message, WPARAM wParam,
                                     LPARAM lParam, UINT_PTR id, DWORD_PTR) {
    if (message == TaskbarUpdateMessage()) {
        g_pendingUpdates.erase(window);
        if (!g_unloading) ApplySettingsFromTaskbarThread();
        return 0;
    }
    if (message == WM_NCDESTROY) {
        RemoveWindowSubclass(window, TaskbarUpdateSubclass, id);
        std::erase(g_subclassedTaskbars, window);
        g_pendingUpdates.erase(window);
    }
    return DefSubclassProc(window, message, wParam, lParam);
}
void ScheduleTaskbarUpdate() {
    if (g_unloading || g_applyingSettings) return;
    // Queue only an integer message, never a delegate pointing into an unloaded
    // DLL. Removing the subclass makes any queued update harmless on unload.
    EnumThreadWindows(GetCurrentThreadId(), [](HWND window, LPARAM) -> BOOL {
        WCHAR name[32]{};
        if (!GetClassNameW(window, name, ARRAYSIZE(name)) ||
            (wcscmp(name, L"Shell_TrayWnd") != 0 && wcscmp(name, L"Shell_SecondaryTrayWnd") != 0)) return TRUE;
        if (std::find(g_subclassedTaskbars.begin(), g_subclassedTaskbars.end(), window) == g_subclassedTaskbars.end()) {
            if (!SetWindowSubclass(window, TaskbarUpdateSubclass,
                reinterpret_cast<UINT_PTR>(&TaskbarUpdateSubclass), 0)) return TRUE;
            g_subclassedTaskbars.push_back(window);
        }
        if (g_pendingUpdates.emplace(window, true).second &&
            !PostMessageW(window, TaskbarUpdateMessage(), 0, 0)) g_pendingUpdates.erase(window);
        return FALSE;  // One message updates all taskbars on this UI thread.
    }, 0);
}
void RemoveTaskbarSubclasses() {
    for (HWND window : g_subclassedTaskbars)
        RemoveWindowSubclass(window, TaskbarUpdateSubclass, reinterpret_cast<UINT_PTR>(&TaskbarUpdateSubclass));
    g_subclassedTaskbars.clear(); g_pendingUpdates.clear();
}

void ApplySettingsToTaskbars() {
    std::vector<DWORD> threads;
    EnumWindows([](HWND window, LPARAM param) -> BOOL {
        DWORD pid = 0;
        DWORD thread = GetWindowThreadProcessId(window, &pid);
        WCHAR name[32]{};
        if (pid != GetCurrentProcessId() || !GetClassNameW(window, name, ARRAYSIZE(name)) ||
            (wcscmp(name, L"Shell_TrayWnd") != 0 && wcscmp(name, L"Shell_SecondaryTrayWnd") != 0)) return TRUE;
        auto& visited = *reinterpret_cast<std::vector<DWORD>*>(param);
        if (std::find(visited.begin(), visited.end(), thread) != visited.end()) return TRUE;
        visited.push_back(thread);
        RunFromWindowThread(window, [](void*) {
            if (g_unloading) {
                RemoveTaskbarSubclasses();
                RestoreButtonStates();
                g_orderOffsets.clear();
                for (auto const& weak : g_repeaters) if (auto element = weak.get()) element.InvalidateArrange();
                g_repeaters.clear();
            } else ApplySettingsFromTaskbarThread();
        }, nullptr);
        return TRUE;
    }, reinterpret_cast<LPARAM>(&threads));
}

LRESULT CALLBACK LowLevelKeyboardProc(int nCode,
                                      WPARAM wParam,
                                      LPARAM lParam) {
    if (nCode == HC_ACTION && IsWindowsKeyStartMenuDisabled()) {
        const auto* keyInfo = reinterpret_cast<const KBDLLHOOKSTRUCT*>(lParam);
        if (keyInfo->flags & LLKHF_INJECTED) {
            return CallNextHookEx(g_lowLevelKeyboardHook, nCode, wParam,
                                  lParam);
        }

        bool keyDown = wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN;
        bool keyUp = wParam == WM_KEYUP || wParam == WM_SYSKEYUP;
        bool isWindowsKey =
            keyInfo->vkCode == VK_LWIN || keyInfo->vkCode == VK_RWIN;
        bool ctrlDown =
            (GetAsyncKeyState(VK_LCONTROL) & 0x8000) ||
            (GetAsyncKeyState(VK_RCONTROL) & 0x8000);

        if (keyInfo->vkCode == VK_ESCAPE) {
            if (keyDown && ctrlDown) { g_blockedEscape = true; return 1; }
            if (keyUp && g_blockedEscape) { g_blockedEscape = false; return 1; }
        }

        if (isWindowsKey) {
            auto& state = g_windowsKeys[keyInfo->vkCode == VK_RWIN ? 1 : 0];
            if (keyDown) {
                state.down = true;  // Auto-repeat must not reset injected state.
                return 1;
            }
            if (keyUp && state.down) {
                if (state.injected) {
                    INPUT input{
                        .type = INPUT_KEYBOARD,
                        .ki =
                            {
                                .wVk = static_cast<WORD>(keyInfo->vkCode),
                                .dwFlags =
                                    KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP,
                            },
                    };
                    SendInput(1, &input, sizeof(input));
                }

                state = {};
                return 1;
            }
        } else if (keyDown) {
            INPUT input[3]{};
            int states[2]{}, count = 0;
            for (int i = 0; i < 2; ++i) {
                if (!g_windowsKeys[i].down || g_windowsKeys[i].injected) continue;
                states[count] = i;
                input[count].type = INPUT_KEYBOARD;
                input[count].ki.wVk = i ? VK_RWIN : VK_LWIN;
                input[count++].ki.dwFlags = KEYEVENTF_EXTENDEDKEY;
            }
            if (count) {
                // Inject the modifier AND triggering key as one ordered batch;
                // letting the original key through can deliver it before Win.
                input[count].type = INPUT_KEYBOARD;
                input[count].ki.wVk = static_cast<WORD>(keyInfo->vkCode);
                input[count].ki.wScan = static_cast<WORD>(keyInfo->scanCode);
                input[count].ki.dwFlags = keyInfo->flags & LLKHF_EXTENDED ? KEYEVENTF_EXTENDEDKEY : 0;
                UINT sent = SendInput(count + 1, input, sizeof(INPUT));
                if (sent == static_cast<UINT>(count + 1)) {
                    for (int j = 0; j < count; ++j) g_windowsKeys[states[j]].injected = true;
                    return 1;
                }
                // A partial/blocked injection must never leave Win held down.
                for (UINT j = 0; j < std::min<UINT>(sent, count); ++j) {
                    input[j].ki.dwFlags |= KEYEVENTF_KEYUP;
                    SendInput(1, &input[j], sizeof(INPUT));
                }
            }
        }
    }

    return CallNextHookEx(g_lowLevelKeyboardHook, nCode, wParam, lParam);
}

bool InstallWindowsKeyHook() {
    if (g_lowLevelKeyboardHook || !IsWindowsKeyStartMenuDisabled()) {
        return true;
    }

    HMODULE currentModule = nullptr;
    if (!GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                               GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           reinterpret_cast<LPCTSTR>(&LowLevelKeyboardProc),
                           &currentModule)) {
        Wh_Log(L"GetModuleHandleEx failed for keyboard hook");
        return false;
    }

    g_windowsKeys[0] = {}; g_windowsKeys[1] = {}; g_blockedEscape = false;
    g_lowLevelKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL,
                                              LowLevelKeyboardProc,
                                              currentModule,
                                              0);
    if (!g_lowLevelKeyboardHook) {
        Wh_Log(L"SetWindowsHookEx failed for keyboard hook: %u", GetLastError());
        return false;
    }

    return true;
}

void UninstallWindowsKeyHook() {
    if (g_lowLevelKeyboardHook) {
        UnhookWindowsHookEx(g_lowLevelKeyboardHook);
        g_lowLevelKeyboardHook = nullptr;
    }

    for (int i = 0; i < 2; ++i) {
        if (g_windowsKeys[i].injected) {
            INPUT input{};
            input.type = INPUT_KEYBOARD;
            input.ki.wVk = i ? VK_RWIN : VK_LWIN;
            input.ki.dwFlags = KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP;
            SendInput(1, &input, sizeof(input));
        }
        g_windowsKeys[i] = {};
    }
    g_blockedEscape = false;
}

DWORD WINAPI WindowsKeyThreadProc(void*) {
    MSG message{};
    // Create the queue before announcing readiness. The stop event also works
    // before any messages arrive, so unload never depends on a PostThreadMessage race.
    PeekMessageW(&message, nullptr, 0, 0, PM_NOREMOVE);
    const bool installed = InstallWindowsKeyHook();
    g_keyboardInitialized.store(installed);
    SetEvent(g_keyboardReadyEvent);
    if (installed) {
        while (WaitForSingleObject(g_keyboardStopEvent, 0) == WAIT_TIMEOUT) {
            DWORD result = MsgWaitForMultipleObjects(1, &g_keyboardStopEvent,
                FALSE, INFINITE, QS_ALLINPUT);
            if (result != WAIT_OBJECT_0 + 1) break;
            while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
                if (message.message == WM_QUIT) {
                    SetEvent(g_keyboardStopEvent);
                    break;
                }
                TranslateMessage(&message);
                DispatchMessageW(&message);
                if (WaitForSingleObject(g_keyboardStopEvent, 0) != WAIT_TIMEOUT) break;
            }
        }
    }
    UninstallWindowsKeyHook();
    return installed ? 0 : 1;
}

void StopWindowsKeyThread() {
    if (g_keyboardThread) {
        SetEvent(g_keyboardStopEvent);
        WaitForSingleObject(g_keyboardThread, INFINITE);
        CloseHandle(g_keyboardThread);
        g_keyboardThread = nullptr;
    }
    if (g_keyboardReadyEvent) CloseHandle(g_keyboardReadyEvent);
    if (g_keyboardStopEvent) CloseHandle(g_keyboardStopEvent);
    g_keyboardReadyEvent = g_keyboardStopEvent = nullptr;
    g_keyboardInitialized = false;
}

bool StartWindowsKeyThread() {
    if (!IsWindowsKeyStartMenuDisabled()) return true;
    if (g_keyboardThread) return true;
    g_keyboardReadyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_keyboardStopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_keyboardReadyEvent || !g_keyboardStopEvent) {
        StopWindowsKeyThread();
        return false;
    }
    g_keyboardThread = CreateThread(nullptr, 0, WindowsKeyThreadProc, nullptr, 0, nullptr);
    if (!g_keyboardThread) {
        StopWindowsKeyThread();
        return false;
    }
    HANDLE handles[]{g_keyboardReadyEvent, g_keyboardThread};
    if (WaitForMultipleObjects(ARRAYSIZE(handles), handles, FALSE, INFINITE) != WAIT_OBJECT_0 ||
        !g_keyboardInitialized.load()) {
        StopWindowsKeyThread();
        return false;
    }
    return true;
}

void BuildStartOrderPlan() {
    g_orderOffsets.clear();
    if (g_unloading || GetMode() != Mode::Off || g_iconPosition == IconPosition::Native) return;
    std::erase_if(g_repeaters, [](auto const& weak) { return !weak.get(); });
    for (auto const& weak : g_repeaters) {
        auto repeater = weak.get();
        if (!repeater) continue;
        struct Item { FrameworkElement element; double width; bool start; bool app; };
        std::vector<Item> items;
        auto logicalRepeater = repeater.try_as<winrt::Microsoft::UI::Xaml::Controls::ItemsRepeater>();
        if (!logicalRepeater) continue;
        auto source = logicalRepeater.ItemsSourceView();
        int count = source ? source.Count() : 0;
        auto addItem = [&](FrameworkElement child) {
            bool start = IsStartButton(child);
            bool app = winrt::get_class_name(child) == L"Taskbar.TaskListButton";
            auto className = winrt::get_class_name(child);
            bool system = className == L"Taskbar.ExperienceToggleButton" || className == L"Taskbar.SearchBoxButton";
            if ((!app && !system) || child.Visibility() != Visibility::Visible) return false;
            auto margin = child.Margin();
            double width = child.ActualWidth();
            // Measure precedes Arrange, while ActualWidth can still be zero
            // when Windows has just created/rebuilt this item.
            if (width <= 0) width = child.DesiredSize().Width;
            if (system && !start && Media::VisualTreeHelper::GetChildrenCount(child) > 0) {
                if (auto content = Media::VisualTreeHelper::GetChild(child, 0).try_as<FrameworkElement>())
                    width = content.DesiredSize().Width;
            }
            width += margin.Left + margin.Right;
            if (std::isfinite(width) && width > 0) items.push_back({child, width, start, app});
            return false;
        };
        for (int index = 0; index < count; ++index) {
            if (auto element = logicalRepeater.TryGetElement(index))
                if (auto child = element.try_as<FrameworkElement>()) addItem(child);
        }
        auto startIt = std::find_if(items.begin(), items.end(), [](auto const& item) { return item.start; });
        if (startIt == items.end()) continue;
        size_t appCount = std::count_if(items.begin(), items.end(), [](auto const& item) { return item.app; });
        size_t slot = 0;
        if (g_iconPosition == IconPosition::Center) slot = appCount / 2;
        else if (g_iconPosition == IconPosition::Right) slot = appCount;
        else if (g_iconPosition == IconPosition::Custom) slot = std::min<size_t>(g_iconSlot, appCount);
        std::unordered_map<void*, double> nativeX;
        double x = 0;
        for (auto const& item : items) { nativeX[winrt::get_abi(item.element)] = x; x += item.width; }
        Item start = *startIt; items.erase(startIt);
        auto insertIt = items.end();
        size_t appsBefore = 0;
        for (auto it = items.begin(); it != items.end(); ++it) {
            if (it->app) {
                if (appsBefore == slot) { insertIt = it; break; }
                ++appsBefore;
            }
        }
        items.insert(insertIt, start);
        x = 0;
        for (auto const& item : items) {
            void* key = winrt::get_abi(item.element);
            g_orderOffsets[key] = x - nativeX[key]; x += item.width;
        }
    }
}

HRESULT WINAPI IUIElement_Arrange_Hook(
    void* pThis,
    winrt::Windows::Foundation::Rect rect) {
    auto original = [=]() {
        return g_iuiElementArrangeOriginal(pThis, rect);
    };

    if (!g_inArrangeOverride || g_unloading.load(std::memory_order_relaxed)) {
        return original();
    }

    FrameworkElement element = nullptr;
    reinterpret_cast<IUnknown*>(pThis)->QueryInterface(
        winrt::guid_of<FrameworkElement>(), winrt::put_abi(element));
    if (GetMode() == Mode::Off && element) {
        auto found = g_orderOffsets.find(winrt::get_abi(element));
        if (found != g_orderOffsets.end()) {
            rect.X += static_cast<float>(found->second);
            return g_iuiElementArrangeOriginal(pThis, rect);
        }
        return original();
    }
    if (GetMode() != Mode::Left || !IsStartButton(element)) return original();

    ScheduleTaskbarUpdate();

    auto newRect = rect;
    newRect.X = 0;
    return g_iuiElementArrangeOriginal(pThis, newRect);
}

HRESULT WINAPI TaskbarCollapsibleLayoutXamlTraits_ArrangeOverride_Hook(
    void* pThis,
    void* context,
    winrt::Windows::Foundation::Size size,
    winrt::Windows::Foundation::Size* resultSize) {
    if (g_inArrangeOverride) {
        return g_taskbarCollapsibleLayoutArrangeOverrideOriginal(pThis, context, size, resultSize);
    }
    [[maybe_unused]] static bool hookedArrange = [] {
        Shapes::Rectangle rectangle;
        IUIElement element = rectangle;

        void** vtable = *reinterpret_cast<void***>(winrt::get_abi(element));
        auto arrange = reinterpret_cast<IUIElement_Arrange_t>(vtable[92]);

        WindhawkUtils::SetFunctionHook(arrange, IUIElement_Arrange_Hook,
                                       &g_iuiElementArrangeOriginal);
        Wh_ApplyHookOperations();
        return true;
    }();

    try { BuildStartOrderPlan(); }
    catch (...) { g_orderOffsets.clear(); Wh_Log(L"Start order plan: %08X", winrt::to_hresult()); }
    struct ArrangeFlag {
        bool previous = g_inArrangeOverride;
        ArrangeFlag() { g_inArrangeOverride = true; }
        ~ArrangeFlag() { g_inArrangeOverride = previous; }
    } flag;

    HRESULT result = g_taskbarCollapsibleLayoutArrangeOverrideOriginal(
        pThis, context, size, resultSize);

    return result;
}

void WINAPI ExperienceToggleButton_UpdateButtonPadding_Hook(void* pThis) {
    g_experienceToggleButtonUpdateButtonPaddingOriginal(pThis);
    if (!g_unloading) ScheduleTaskbarUpdate();
}

void WINAPI AugmentedEntryPointButton_UpdateButtonPadding_Hook(void* pThis) {
    g_augmentedEntryPointButtonUpdateButtonPaddingOriginal(pThis);
    if (!g_unloading && GetMode() == Mode::Left) ScheduleTaskbarUpdate();
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
            &g_ctaskBandTaskListWndSiteVftable,
        },
        {
            {LR"(const CSecondaryTaskBand::`vftable'{for `ITaskListWndSite'})"},
            &g_csecondaryTaskBandTaskListWndSiteVftable,
        },
        {
            {LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const )"},
            &g_ctaskBandGetTaskbarHost,
        },
        {
            {LR"(public: int __cdecl TaskbarHost::FrameHeight(void)const )"},
            &g_taskbarHostFrameHeight,
        },
        {
            {LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CSecondaryTaskBand::GetTaskbarHost(void)const )"},
            &g_csecondaryTaskBandGetTaskbarHost,
        },
        {
            {LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))"},
            &g_refCountBaseDecref,
        },
    };

    return HookSymbols(module, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks));
}

bool HookTaskbarViewDllSymbols(HMODULE module) {
    // Taskbar.View.dll, ExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskbarCollapsibleLayout,struct winrt::Microsoft::UI::Xaml::Controls::IVirtualizingLayoutOverrides>::ArrangeOverride(void *,struct winrt::Windows::Foundation::Size,struct winrt::Windows::Foundation::Size *))"},
            &g_taskbarCollapsibleLayoutArrangeOverrideOriginal,
            TaskbarCollapsibleLayoutXamlTraits_ArrangeOverride_Hook,
        },
        {
            {LR"(protected: virtual void __cdecl winrt::Taskbar::implementation::ExperienceToggleButton::UpdateButtonPadding(void))"},
            &g_experienceToggleButtonUpdateButtonPaddingOriginal,
            ExperienceToggleButton_UpdateButtonPadding_Hook,
        },
        {
            {LR"(protected: virtual void __cdecl winrt::Taskbar::implementation::AugmentedEntryPointButton::UpdateButtonPadding(void))"},
            &g_augmentedEntryPointButtonUpdateButtonPaddingOriginal,
            AugmentedEntryPointButton_UpdateButtonPadding_Hook,
        },
    };

    return HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks));
}

HMODULE GetTaskbarViewModuleHandle() {
    HMODULE module = GetModuleHandle(L"Taskbar.View.dll");
    if (!module) {
        module = GetModuleHandle(L"ExplorerExtensions.dll");
    }

    return module;
}

void HandleLoadedModuleIfTaskbarView(HMODULE module, LPCWSTR moduleFileName) {
    if (!g_taskbarViewDllLoaded && GetTaskbarViewModuleHandle() == module &&
        !g_taskbarViewDllLoaded.exchange(true)) {
        Wh_Log(L"Loaded %s", moduleFileName);

        if (HookTaskbarViewDllSymbols(module)) {
            Wh_ApplyHookOperations();
            ApplySettingsToTaskbars();
        }
    }
}

HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName,
                                   HANDLE hFile,
                                   DWORD dwFlags) {
    HMODULE module = g_loadLibraryExWOriginal(lpLibFileName, hFile, dwFlags);
    if (module) {
        HandleLoadedModuleIfTaskbarView(module, lpLibFileName);
    }

    return module;
}

}  // namespace

void LoadSettings() {
    PCWSTR position = Wh_GetStringSetting(L"iconPosition");
    g_iconPosition = IconPosition::Native;
    if (wcscmp(position, L"left") == 0) g_iconPosition = IconPosition::Left;
    else if (wcscmp(position, L"center") == 0) g_iconPosition = IconPosition::Center;
    else if (wcscmp(position, L"right") == 0) g_iconPosition = IconPosition::Right;
    else if (wcscmp(position, L"custom") == 0) g_iconPosition = IconPosition::Custom;
    Wh_FreeStringSetting(position);
    g_iconSlot = std::clamp(Wh_GetIntSetting(L"iconSlot"), 0, 1000);
    PCWSTR preset = Wh_GetStringSetting(L"iconPreset");
    g_iconPreset = preset && *preset ? preset : L"auto";
    Wh_FreeStringSetting(preset);
    if (g_iconPreset != L"auto" && g_iconPreset != L"native" &&
        g_iconPreset != L"custom" && !FindBuiltinIcon(g_iconPreset)) {
        Wh_Log(L"Unknown icon preset; using Automatic");
        g_iconPreset = L"auto";
    }
    PCWSTR color = Wh_GetStringSetting(L"iconColor");
    g_iconColor = color && *color ? color : L"auto";
    Wh_FreeStringSetting(color);
    if (g_iconColor != L"auto" && !IsIconHexColor(g_iconColor)) {
        Wh_Log(L"Invalid icon color; following the Windows theme");
        g_iconColor = L"auto";
    }
    const int iconSize = Wh_GetIntSetting(L"iconSize");
    g_iconSize = iconSize ? std::clamp(iconSize, 16, 32) : 24;
    g_customIconPath = ExpandSetting(L"customIconPath");
    g_launchTarget = ExpandSetting(L"launchTarget");
    g_launchArguments = ExpandSetting(L"launchArguments");
    PCWSTR modeSetting = Wh_GetStringSetting(L"mode");
    SetMode(ParseModeSetting(modeSetting));
    Wh_FreeStringSetting(modeSetting);

    g_disableWindowsKeyStartMenu.store(
        Wh_GetIntSetting(L"disableWindowsKeyStartMenu") != 0,
        std::memory_order_relaxed);
}

BOOL Wh_ModInit() {
    LoadSettings();

    WCHAR moduleFilePath[MAX_PATH];
    if (GetModuleFileName(nullptr, moduleFilePath, ARRAYSIZE(moduleFilePath))) {
        if (PCWSTR moduleFileName = wcsrchr(moduleFilePath, L'\\')) {
            moduleFileName++;
            if (_wcsicmp(moduleFileName, L"StartMenuExperienceHost.exe") == 0) {
                g_target = Target::StartMenuExperienceHost;
            }
        }
    }

    if (g_target == Target::StartMenuExperienceHost) {
        if (!IsWindowsKeyStartMenuDisabled()) return FALSE;
        HMODULE winrtModule =
            GetModuleHandle(L"api-ms-win-core-winrt-l1-1-0.dll");
        if (!winrtModule) {
            Wh_Log(L"Failed to get WinRT module");
            return FALSE;
        }

        auto roGetActivationFactory = reinterpret_cast<decltype(&RoGetActivationFactory)>(
            GetProcAddress(winrtModule, "RoGetActivationFactory"));
        if (!roGetActivationFactory) {
            Wh_Log(L"Failed to resolve RoGetActivationFactory");
            return FALSE;
        }

        WindhawkUtils::SetFunctionHook(roGetActivationFactory,
                                       StartMenuBlock::RoGetActivationFactory_Hook,
                                       &g_roGetActivationFactoryOriginal);
        return TRUE;
    }

    if (!HookTaskbarDllSymbols()) {
        return FALSE;
    }

    if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
        g_taskbarViewDllLoaded = true;
        if (!HookTaskbarViewDllSymbols(taskbarViewModule)) {
            return FALSE;
        }
    } else {
        HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
        if (!kernelBaseModule) {
            Wh_Log(L"Failed to get kernelbase.dll");
            return FALSE;
        }

        auto loadLibraryExW = reinterpret_cast<decltype(&LoadLibraryExW)>(
            GetProcAddress(kernelBaseModule, "LoadLibraryExW"));
        if (!loadLibraryExW) {
            Wh_Log(L"Failed to resolve LoadLibraryExW");
            return FALSE;
        }

        WindhawkUtils::SetFunctionHook(loadLibraryExW, LoadLibraryExW_Hook,
                                       &g_loadLibraryExWOriginal);
    }

    if (!StartWindowsKeyThread()) {
        Wh_Log(L"Cannot start optional Windows-key hook thread");
        return FALSE;
    }
    return TRUE;
}

void Wh_ModAfterInit() {
    if (g_target == Target::StartMenuExperienceHost) {
        HWND hCoreWnd = StartMenuBlock::GetCoreWnd();
        if (hCoreWnd) {
            RunFromWindowThread(
                hCoreWnd, [](void*) { StartMenuBlock::Init(); }, nullptr);
        }
        return;
    }

    if (!g_taskbarViewDllLoaded) {
        if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
            if (!g_taskbarViewDllLoaded.exchange(true)) {
                if (HookTaskbarViewDllSymbols(taskbarViewModule)) {
                    Wh_ApplyHookOperations();
                }
            }
        }
    }

    ApplySettingsToTaskbars();
}

void Wh_ModBeforeUninit() {
    if (g_target == Target::StartMenuExperienceHost) {
        HWND hCoreWnd = StartMenuBlock::GetCoreWnd();
        if (hCoreWnd) {
            RunFromWindowThread(
                hCoreWnd, [](void*) { StartMenuBlock::Uninit(); }, nullptr);
        }
        return;
    }

    g_unloading.store(true, std::memory_order_relaxed);
    ApplySettingsToTaskbars();
}

void Wh_ModUninit() {
    StopWindowsKeyThread();
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    if (bReload) *bReload = TRUE;
    return TRUE;
}
