// ==WindhawkMod==
// @id              windhawk-mawaqit
// @name            Mawaqit Prayer Clock
// @description     Mawaqit prayer clock with live countdown, multi-monitor taskbar support, prayer schedule popup, and customizable themes.
// @version         0.7.1
// @author          HHeitlager
// @github          https://github.com/hheitlager
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lshell32 -luuid -lgdi32 -lwinhttp
// ==/WindhawkMod==


// ==WindhawkModReadme==
/*
# Mawaqit Prayer Clock

## Bring your daily prayer times to the Windows 11 taskbar.
Mawaqit Prayer Clock displays the next prayer, its scheduled time, and a live countdown directly on your taskbar.
Click the clock to view today's full prayer schedule, using prayer times published by your selected mosque on [Mawaqit](https://mawaqit.net).

## Features
- **Prayer times at a glance** - See the next prayer and how much time remains.
- **Daily prayer schedule** - Click or hover over the clock to view today's prayer times and Iqamah delays.
- **Personalize your clock** - Choose from 13 themes, customize colors, and adjust its appearance.
- **Flexible taskbar placement** - Choose where the clock appears, including on multiple monitors.
- **Customizable popup** - Adjust its appearance, transparency, and display options.

## Screenshots
Choose a style that matches your Windows desktop.

| Windows / System | Traditional Islamic |
|:---:|:---:|
| ![Windows](https://raw.githubusercontent.com/hheitlager/windhawk-mawaqit/main/screenshots/Mawaqit-Windows-System-Default-v071.png) | ![Traditional](https://raw.githubusercontent.com/hheitlager/windhawk-mawaqit/main/screenshots/Mawaqit-Traditional-Islamic-v071.png) |

| Catppuccin Mocha | Emberveil |
|:---:|:---:|
| ![Catppuccin](https://raw.githubusercontent.com/hheitlager/windhawk-mawaqit/main/screenshots/Mawaqit-Catppuccin-Mocha-v071.png) | ![Emberveil](https://raw.githubusercontent.com/hheitlager/windhawk-mawaqit/main/screenshots/Mawaqit-Emberveil-v071.png) |

## Desktop Integration
See how Mawaqit Prayer Clock looks on a customized Windows 11 desktop.

![Desktop](https://raw.githubusercontent.com/hheitlager/windhawk-mawaqit/main/screenshots/My-Desktop-v071.png)

## Getting Started
**1. Find your mosque**
Visit [Mawaqit](https://mawaqit.net/) or [search Google for a Mawaqit mosque near you](https://www.google.com/search?q=mawaqit+mosque+near+me). Open your mosque's Mawaqit page.

**2. Configure your mosque**
Copy the full page URL or its mosque ID (the last part of the URL) into the mod's **Mosque** setting.

![Where to find the mosque ID](https://raw.githubusercontent.com/hheitlager/windhawk-mawaqit/main/screenshots/Mawaqit-Mosque-Identifier.png)

**3. Make it yours**
Choose your preferred theme and taskbar position. You're ready!

## Good to Know
- **Windows 11 only.** Supports primary and secondary monitors.
- **Internet access.** The mod connects to Mawaqit to retrieve your mosque's published prayer times and stores a local copy for reuse.
- **No account required.** Simply select your mosque.
- **Mosque-specific schedules.** Prayer times reflect the schedule and adjustments published by your selected mosque.

## More Information
For additional screenshots, troubleshooting, technical details, and release information, visit the [GitHub project](https://github.com/hheitlager/windhawk-mawaqit).

### Network access and privacy
The mod retrieves your selected mosque's published prayer schedule from Mawaqit over HTTPS and caches it locally. Mawaqit can see your IP address and selected mosque identifier when the schedule is requested. No account is required.
For refresh behavior, caching details, and technical implementation, see the [project documentation](https://github.com/hheitlager/windhawk-mawaqit).

## Credits and Acknowledgements
- Mawaqit Prayer Clock is an independent, open-source community project built for [Windhawk](https://windhawk.net).
- Special thanks to the Windhawk community and the developers whose work helped inspire this mod, including the contributors to [Taskbar Folder Menus](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/taskbar-folder-menus.wh.cpp).
- Additional acknowledgements and third-party license information can be found in the project's [GitHub repository](https://github.com/hheitlager/windhawk-mawaqit).

## License
MIT License.

## Disclaimer
This is an unofficial community project and is not affiliated with or endorsed by Mawaqit.
*/
// ==/WindhawkModReadme==


// ==WindhawkModSettings==
/*
- General:
  - Mosque: al-haram-makkah-saudi-arabia
    $name: Mawaqit mosque identifier or URL
  - Theme: system
    $name: Theme
    $options:
    - system: Windows / System
    - traditional: Traditional Islamic
    - catppuccin: Catppuccin Mocha
    - emberveil: Emberveil
    - emberveil-midnight: Emberveil Midnight
    - emberveil-daybreak: Emberveil Daybreak
    - emerald-mosque: Emerald Mosque
    - dracula: Dracula
    - nord: Nord
    - gruvbox: Gruvbox Dark
    - tokyo-night: Tokyo Night
    - solarized-dark: Solarized Dark
    - solarized-light: Solarized Light
    - custom: Custom
  - Position: before-tray
    $name: Taskbar position
    $description: Choose where the clock appears on the taskbar. Dynamic center may overlap applications when the taskbar is full.
    $options:
    - before-tray: Before system tray
    - after-tray: After system tray
    - before-icons: Before taskbar icons
    - after-icons: After taskbar icons
    - center: Dynamic center (experimental)
  $name: General
- Display:
  - ShowNextPrayer: true
    $name: Show next prayer
  - ShowCountdown: true
    $name: Show countdown
  - ShowIqamahTime: true
    $name: Show Iqamah time
    $description: Show the mosque's Iqamah delay (+N minutes) next to the countdown and each prayer in the popup.
  - ShowFajrWindowCountdown: false
    $name: Show Shuruq and Imsak
    $description: Include Shuruq (sunrise) and, when provided by your mosque, Imsak in the popup schedule. After Fajr begins, the countdown shows the time remaining until Shuruq. Imsak is informational and does not start a prayer countdown. Disabled by default.
  - ShowPrimaryMonitor: true
    $name: Show clock on primary monitor
  - ShowSecondaryMonitors: true
    $name: Show clock on secondary monitors
  $name: Display settings
- Advanced:
  - LayoutSettings:
    - Width: 0
      $name: Width (0 = Auto)
      $description: Set to 0 to size the clock automatically for its text, font size and padding; or choose a fixed width of 180–400 px.
    - PillHeight: 0
      $name: Height
      $description: Set to 0 to adjust automatically to the taskbar, or choose a height between 1 and 100 px.
    - Margins: "0, 0, 0, 0"
      $name: Margin (left, top, right, bottom)
      $description: Move the clock left, right, up, or down (−100 to 100 px per side).
    - Padding: "0, 0, 0, 0"
      $name: Padding (left, top, right, bottom)
      $description: Adjust the space between the clock edges and its text (0–100 px per side).
    - TrayGap: 5
      $name: Gap before system tray
      $description: Choose how much space appears between the clock and system tray (0–100 px).
    - CornerRadius: 13
      $name: Corner radius
      $description: Choose how rounded the clock's corners appear (0–30 px). Set to 0 for square corners; higher values create rounder corners.
    - BackgroundStyle: solid
      $name: Background style
      $description: Choose the clock background material. Blur and Acrylic are experimental and depend on Windows composition support.
      $options:
      - solid: Solid
      - transparent: Transparent
      - blur: Blur (experimental)
      - acrylic: Acrylic (experimental)
    - BackgroundOpacity: 100
      $name: Background opacity (%)
      $description: Pill background opacity from 0 (fully transparent) to 100 (opaque). Transparent keeps the pill transparent but uses a subtle acrylic-tinted popup for readability. Blur and Acrylic may interpret opacity differently.
    - ShowPillBorder: false
      $name: Show pill border
      $description: Draw a 1 px border using the theme countdown accent (or the custom border color).
    - TextShadow: false
      $name: Pill text shadow
      $description: Add a subtle dark shadow behind the pill text for readability on transparent or blurred taskbars. Does not affect the popup.
    - FontSize: 15
      $name: Fontsize
      $description: Adjust the clock's text size (10–24 px). This does not affect the popup.
    - IqamahFontScale: 80
      $name: Iqamah font scale (%)
      $description: Size of the Iqamah suffix relative to the pill font (50–100%, default 80%).
    $name: Layout settings
  - PopupAppearanceSettings:
    - EnablePopup: true
      $name: Show popup
    - OpenPopupOnHover: false
      $name: Open popup when hovering over clock
      $description: Open without clicking. The popup closes after the cursor leaves both clock and popup.
    - HoverCloseDelayMs: 500
      $name: Hover popup close delay (milliseconds)
      $description: Delay after leaving the clock and popup (0–5000 ms).
    - PopupAutoCloseSeconds: 5
      $name: Popup auto-close (seconds, 0 = disabled)
      $description: Automatically close popup after 1–300 seconds. Opening it again resets the timer.
    - BackgroundStyle: follow-pill
      $name: Popup background material
      $description: Choose the popup material independently of the taskbar pill. Follow pill preserves the previous behavior.
      $options:
      - follow-pill: Follow pill
      - solid: Solid
      - transparent: Transparent
      - blur: Blur (experimental)
      - acrylic: Acrylic (experimental)
    - BackgroundOpacity: -1
      $name: Popup background opacity (%)
      $description: -1 follows the pill opacity (legacy behavior); 0–100 sets the popup independently. For Transparent, Blur and Acrylic this controls the backdrop tint strength.
    - ShowBorder: true
      $name: Show popup border
    - ShowSeparators: true
      $name: Show popup separators
    $name: Popup appearance
  - CustomThemeSettings:
    - PillBackground: ""
      $name: Pill background
    - PillText: ""
      $name: Pill prayer text
    - PillSeparator: ""
      $name: Pill separator
    - PillCountdown: ""
      $name: Pill countdown
    - PillIqamah: ""
      $name: Pill Iqamah
      $description: Optional hex color for the Iqamah suffix; empty inherits the resolved countdown color.
    - PillBorder: ""
      $name: Pill border color
      $description: Optional hex override; empty uses the countdown accent color.
    - PopupBackground: ""
      $name: Popup background
    - PopupBorder: ""
      $name: Popup border
    - PopupText: ""
      $name: Popup main text
    - PopupMuted: ""
      $name: Popup secondary text
    - PopupRule: ""
      $name: Popup dividers
    - PopupFooter: ""
      $name: Popup footer
    $name: Custom theme settings
    $description: Optional color overrides for any selected theme. Empty fields inherit the selected palette. Non-empty overrides remain active when switching themes until cleared.
  - ModSettingsAdvanced:
    - RefreshHours: 24
      $name: Refresh interval (hours)
      $description: Choose how often the prayer schedule is updated from Mawaqit (1–168 hours).
    $name: Mod settings
  $name: Advanced
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <dwmapi.h>
#undef GetCurrentTime
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.h>
#include <winrt/Windows.UI.Xaml.Media.Imaging.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <windhawk_utils.h>
#include <atomic>
#include <algorithm>
#include <cmath>
#include <string>
#include <cstdint>
#include <winhttp.h>
#include <map>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <vector>
#include <ctime>
#include <cwchar>
#include <iterator>
#include <cstdlib>
#include <cstring>
#include <cstdio>

using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Controls;
using namespace winrt::Windows::UI::Xaml::Media;

namespace folder_menus_slot_lease
{

using winrt::Windows::UI::Xaml::FrameworkElement;
using winrt::Windows::UI::Xaml::GridUnitType;
using winrt::Windows::UI::Xaml::Controls::ColumnDefinition;
using winrt::Windows::UI::Xaml::Controls::Grid;
using winrt::Windows::UI::Xaml::Controls::Panel;
using winrt::Windows::UI::Xaml::Controls::StackPanel;

enum class Anchor
{
    BeforeIcons,
    AfterShowDesktop,
};

// Which layout contract the live tray panel follows.
enum class Kind
{
    Unsupported, // some other Panel: do not guess its layout semantics.
    Columns,     // Grid. A slot is a column index.
    Order,       // StackPanel. A slot is a child index.
};

struct Lease
{
    std::wstring markerName;
    int slot = -1;
    Kind kind = Kind::Unsupported;
    bool createdBaseColumn = false;
};

inline Kind Classify(FrameworkElement const& parent)
{
    if (!parent)
        return Kind::Unsupported;
    if (parent.try_as<Grid>())
        return Kind::Columns;
    if (parent.try_as<StackPanel>())
        return Kind::Order;
    return Kind::Unsupported;
}

// Class name of an unexpected panel, so a mod can log what it actually got
// instead of reporting a bare "not found" for an element that is right there.
inline std::wstring ClassName(FrameworkElement const& element)
{
    if (!element)
        return L"(null)";
    try
    {
        return std::wstring(winrt::get_class_name(element));
    }
    catch (...)
    {
        return L"(unknown)";
    }
}

// Named direct children, for logging when an anchor cannot be resolved. A tray
// restructure shows up here as missing or renamed names, which is the one thing
// a user's debug log otherwise cannot tell us.
inline std::wstring DescribeChildren(Panel const& parent)
{
    if (!parent)
        return L"(none)";
    std::wstring names;
    try
    {
        for (auto const& child : parent.Children())
        {
            auto element = child.try_as<FrameworkElement>();
            if (!element || element.Name().empty())
                continue;
            if (!names.empty())
                names += L", ";
            names += element.Name();
        }
    }
    catch (...)
    {
        return L"(unreadable)";
    }
    return names.empty() ? L"(no named children)" : names;
}

inline FrameworkElement FindDirectChild(Panel const& parent, wchar_t const* name)
{
    if (!parent || !name)
        return nullptr;
    for (auto const& child : parent.Children())
    {
        auto element = child.try_as<FrameworkElement>();
        if (element && element.Name() == name)
            return element;
    }
    return nullptr;
}

inline int IndexOfChild(Panel const& parent, FrameworkElement const& child)
{
    if (!parent || !child)
        return -1;
    for (uint32_t i = 0; i < parent.Children().Size(); ++i)
    {
        if (parent.Children().GetAt(i).try_as<FrameworkElement>() == child)
            return static_cast<int>(i);
    }
    return -1;
}

inline bool ResolveSlot(Panel const& parent, Anchor anchor, int& slot)
{
    Kind kind = Classify(parent);
    if (kind == Kind::Unsupported)
        return false;

    if (anchor == Anchor::BeforeIcons)
    {
        slot = 0;
        return true;
    }

    wchar_t const* referenceName = nullptr;
    bool after = false;
    switch (anchor)
    {
    case Anchor::AfterShowDesktop:
        referenceName = L"ShowDesktopStack";
        after = true;
        break;
    case Anchor::BeforeIcons:
        break;
    }

    auto reference = FindDirectChild(parent, referenceName);
    if (!reference)
        return false; // Never silently turn an unavailable anchor into slot 0.

    if (kind == Kind::Columns)
    {
        slot =
            Grid::GetColumn(reference) + (after ? std::max(1, Grid::GetColumnSpan(reference)) : 0);
        return true;
    }

    // Order: the reference's own child index is the slot. There is no span to
    // step over -- a StackPanel child occupies exactly one position.
    int index = IndexOfChild(parent, reference);
    if (index < 0)
        return false;
    slot = index + (after ? 1 : 0);
    return true;
}

inline bool Release(Panel const& parent, Lease& lease);

inline bool AcquireAt(Panel const& parent, int slot, std::wstring const& markerName, Lease& lease)
{
    Kind kind = Classify(parent);
    if (kind == Kind::Unsupported || slot < 0 || markerName.empty())
        return false;

    // A marker with this name already in the tray is either this caller's own
    // live lease — refuse, acquiring twice would strand the first slot — or a
    // leftover from an instance whose teardown never reached the UI thread.
    // Refusing the leftover would block every later apply until Explorer
    // restarts, so release it and take the slot fresh, correcting the
    // requested slot if the released one sat before it.
    if (auto stale = FindDirectChild(parent, markerName.c_str()))
    {
        if (lease.markerName == markerName)
            return false;
        int staleSlot =
            kind == Kind::Columns ? Grid::GetColumn(stale) : IndexOfChild(parent, stale);
        Lease leftover{markerName, staleSlot, kind};
        if (!Release(parent, leftover))
            return false;
        if (staleSlot >= 0 && staleSlot < slot)
            --slot;
    }

    Grid marker;
    marker.Name(markerName);
    marker.Width(0.0);
    marker.Height(0.0);
    marker.IsHitTestVisible(false);

    if (kind == Kind::Columns)
    {
        auto grid = parent.try_as<Grid>();
        if (!grid)
            return false;
        // A Grid without ColumnDefinitions uses one implicit full-width column.
        // Materialize that implicit column before inserting a lease column;
        // otherwise the repeater is shifted to column 1 in a ONE-column grid.
        bool createdBaseColumn = grid.ColumnDefinitions().Size() == 0;
        if (createdBaseColumn)
        {
            ColumnDefinition base;
            base.Width({1.0, GridUnitType::Star});
            grid.ColumnDefinitions().Append(base);
        }
        ColumnDefinition definition;
        definition.Width({1.0, GridUnitType::Auto});
        slot = std::min(slot, static_cast<int>(grid.ColumnDefinitions().Size()));
        if (static_cast<uint32_t>(slot) < grid.ColumnDefinitions().Size())
            grid.ColumnDefinitions().InsertAt(slot, definition);
        else
            grid.ColumnDefinitions().Append(definition);

        for (auto const& child : grid.Children())
        {
            auto element = child.try_as<FrameworkElement>();
            if (!element)
                continue;
            int start = Grid::GetColumn(element);
            int span = Grid::GetColumnSpan(element);
            if (start >= slot)
                Grid::SetColumn(element, start + 1);
            else if (start + span > slot)
                Grid::SetColumnSpan(element, span + 1);
        }

        Grid::SetColumn(marker, slot);
        grid.Children().Append(marker);
        lease.createdBaseColumn = createdBaseColumn;
    }
    else
    {
        // Order: no column is created or owned. The marker simply holds the
        // position, and PlaceChild drops the content next to it.
        uint32_t index = std::min(static_cast<uint32_t>(slot), parent.Children().Size());
        parent.Children().InsertAt(index, marker);
        slot = static_cast<int>(index);
    }

    lease = {markerName, slot, kind, lease.createdBaseColumn};
    return true;
}

// Live index of the lease marker. Other mods inject and remove siblings around
// us, so the acquire-time index is a hint, never the truth at removal time.
inline bool FindMarker(Panel const& parent, Lease const& lease, uint32_t& index)
{
    if (!parent || lease.markerName.empty())
        return false;
    for (uint32_t i = 0; i < parent.Children().Size(); ++i)
    {
        auto element = parent.Children().GetAt(i).try_as<FrameworkElement>();
        if (element && element.Name() == lease.markerName)
        {
            index = i;
            return true;
        }
    }
    return false;
}

// Put mod content into the leased slot. On a Grid the content joins the leased
// column; on a StackPanel it is inserted directly after the marker, so the
// marker's position is the content's position.
inline bool PlaceChild(Panel const& parent, Lease const& lease, FrameworkElement const& content)
{
    if (!parent || !content || lease.kind == Kind::Unsupported)
        return false;

    uint32_t markerIndex = 0;
    if (!FindMarker(parent, lease, markerIndex))
        return false;

    if (lease.kind == Kind::Columns)
    {
        auto marker = parent.Children().GetAt(markerIndex).try_as<FrameworkElement>();
        Grid::SetColumn(content, marker ? Grid::GetColumn(marker) : lease.slot);
        parent.Children().Append(content);
        return true;
    }

    parent.Children().InsertAt(markerIndex + 1, content);
    return true;
}

inline bool Release(Panel const& parent, Lease& lease)
{
    if (!parent || lease.markerName.empty())
        return false;

    uint32_t markerIndex = 0;
    if (!FindMarker(parent, lease, markerIndex))
        return false;

    if (lease.kind == Kind::Order)
    {
        parent.Children().RemoveAt(markerIndex);
        lease = {};
        return true;
    }

    auto grid = parent.try_as<Grid>();
    auto marker = parent.Children().GetAt(markerIndex).try_as<FrameworkElement>();
    int liveColumn = marker ? Grid::GetColumn(marker) : lease.slot;
    if (!grid || liveColumn < 0)
        return false;

    grid.Children().RemoveAt(markerIndex);
    if (static_cast<uint32_t>(liveColumn) < grid.ColumnDefinitions().Size())
        grid.ColumnDefinitions().RemoveAt(liveColumn);

    for (auto const& child : grid.Children())
    {
        auto element = child.try_as<FrameworkElement>();
        if (!element)
            continue;
        int start = Grid::GetColumn(element);
        int span = Grid::GetColumnSpan(element);
        if (start > liveColumn)
            Grid::SetColumn(element, start - 1);
        else if (start < liveColumn && start + span > liveColumn)
            Grid::SetColumnSpan(element, std::max(1, span - 1));
    }

    // Restore the original implicit-column layout only for columns we created.
    if (lease.createdBaseColumn && grid.ColumnDefinitions().Size() == 1)
        grid.ColumnDefinitions().Clear();
    lease = {};
    return true;
}

} // namespace folder_menus_slot_lease
// Enumerate live primary and secondary Explorer taskbar windows.
namespace mawaqit_monitor_discovery
{
struct TaskbarRecord
{
    HWND hwnd = nullptr;
    HMONITOR monitor = nullptr;
    bool primaryTaskbar = false;
    std::wstring device;
    bool primaryMonitor = false;
};

inline std::vector<TaskbarRecord> EnumerateTaskbars()
{
    std::vector<TaskbarRecord> result;
    EnumWindows(
        [](HWND hwnd, LPARAM context) -> BOOL
        {
            DWORD pid = 0;
            wchar_t className[64]{};
            if (GetWindowThreadProcessId(hwnd, &pid) == 0 || pid != GetCurrentProcessId() ||
                !GetClassNameW(hwnd, className, ARRAYSIZE(className)))
                return TRUE;
            bool primary = _wcsicmp(className, L"Shell_TrayWnd") == 0;
            if (!primary && _wcsicmp(className, L"Shell_SecondaryTrayWnd") != 0)
                return TRUE;
            HMONITOR monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONULL);
            MONITORINFOEXW info{};
            info.cbSize = sizeof(info);
            TaskbarRecord entry{};
            entry.hwnd = hwnd;
            entry.monitor = monitor;
            entry.primaryTaskbar = primary;
            if (monitor && GetMonitorInfoW(monitor, &info))
            {
                entry.device = info.szDevice;
                entry.primaryMonitor = (info.dwFlags & MONITORINFOF_PRIMARY) != 0;
            }
            reinterpret_cast<std::vector<TaskbarRecord>*>(context)->push_back(std::move(entry));
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&result));
    return result;
}

inline void LogInventory()
{
    auto taskbars = EnumerateTaskbars();
    Wh_Log(L"[Mawaqit][Monitor] Taskbar discovery: count=%u", (unsigned)taskbars.size());
    for (auto const& bar : taskbars)
        Wh_Log(L"[Mawaqit][Monitor] hwnd=%p class=%s monitor=%s primaryMonitor=%d uiThread=%lu",
               bar.hwnd, bar.primaryTaskbar ? L"Shell_TrayWnd" : L"Shell_SecondaryTrayWnd",
               bar.device.empty() ? L"(unresolved)" : bar.device.c_str(),
               bar.primaryMonitor ? 1 : 0,
               (unsigned long)GetWindowThreadProcessId(bar.hwnd, nullptr));
}
} // namespace mawaqit_monitor_discovery

namespace folder_menus_taskbar_window
{

// ---- Window discovery -------------------------------------------------------

inline HWND FindCurrentProcessTaskbarWnd()
{
    HWND result = nullptr;
    EnumWindows(
        [](HWND window, LPARAM parameter) -> BOOL
        {
            DWORD processId = 0;
            WCHAR className[32];
            if (GetWindowThreadProcessId(window, &processId) &&
                processId == GetCurrentProcessId() &&
                GetClassName(window, className, ARRAYSIZE(className)) &&
                _wcsicmp(className, L"Shell_TrayWnd") == 0)
            {
                *reinterpret_cast<HWND*>(parameter) = window;
                return FALSE;
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&result));
    return result;
}

// Shell_TrayWnd can be recreated inside Explorer. A cache is useful only while
// it names a live window; otherwise rediscover before dispatch or teardown.
inline HWND ResolveTaskbarWnd(HWND cached)
{
    if (cached && IsWindow(cached))
        return cached;
    return FindCurrentProcessTaskbarWnd();
}

} // namespace folder_menus_taskbar_window
namespace folder_menus_dispatch
{

// ---- UI-thread marshalling --------------------------------------------------
//
// XAML may only be touched from the thread that owns it. This posts work onto
// the taskbar's thread with a CALLWNDPROC hook and a private registered
// message, and reports whether the callback actually ran — a caller that
// assumes it did will corrupt its own state when the dispatch failed.

using ThreadProc = void (*)(void*);
using ExceptionLogFn = void (*)(PCWSTR context);

inline ExceptionLogFn g_logException = nullptr;

// Point this at the mod's logger once in Wh_ModInit so failures inside a UI
// callback are reported in the mod's own voice.
inline void SetExceptionLogger(ExceptionLogFn logger)
{
    g_logException = logger;
}

inline bool Invoke(ThreadProc proc, void* parameter)
{
    try
    {
        proc(parameter);
        return true;
    }
    catch (...)
    {
        if (g_logException)
            g_logException(L"UI callback");
    }
    return false;
}

// A dispatch is owned by a registry rather than by the calling worker's stack.
// A timed-out SendMessage must never leave a hook with a dangling Dispatch*.
struct Dispatch
{
    ThreadProc proc;
    void* parameter;
    std::mutex executionMutex;
    bool ran = false;
    bool cancelled = false;
    bool succeeded = false;
};

inline std::mutex g_registryMutex;
inline std::unordered_map<ULONG_PTR, std::shared_ptr<Dispatch>> g_requests;
inline std::atomic<ULONG_PTR> g_nextRequestId{1};
inline std::atomic<UINT> g_dispatchMessage{0};

// Returns false if the taskbar cannot service the request promptly. Any
// callback already executing is allowed to finish before the caller returns,
// keeping the caller-owned parameter valid for the entire invocation.
inline bool RunFromWindowThread(HWND window, ThreadProc proc, void* parameter, PCWSTR messageName,
                                DWORD timeoutMs = 2000)
{
    UINT message = RegisterWindowMessageW(messageName);
    if (!message || !IsWindow(window))
        return false;

    DWORD threadId = GetWindowThreadProcessId(window, nullptr);
    if (!threadId)
        return false;
    if (threadId == GetCurrentThreadId())
        return Invoke(proc, parameter);

    g_dispatchMessage.store(message, std::memory_order_release);
    HHOOK hook = SetWindowsHookExW(
        WH_CALLWNDPROC,
        [](int code, WPARAM wParam, LPARAM lParam) -> LRESULT
        {
            if (code == HC_ACTION)
            {
                auto const* call = reinterpret_cast<CWPSTRUCT const*>(lParam);
                UINT expected = g_dispatchMessage.load(std::memory_order_acquire);
                if (expected && call->message == expected)
                {
                    auto id = static_cast<ULONG_PTR>(call->lParam);
                    std::shared_ptr<Dispatch> request;
                    {
                        std::lock_guard<std::mutex> lock(g_registryMutex);
                        auto it = g_requests.find(id);
                        if (it != g_requests.end())
                            request = it->second;
                    }
                    if (request)
                    {
                        // Serializes duplicate hooks and cancellation. Holding
                        // this lock through Invoke ensures a timed-out caller
                        // cannot return while its stack parameter is in use.
                        std::lock_guard<std::mutex> lock(request->executionMutex);
                        if (!request->ran && !request->cancelled)
                        {
                            request->ran = true;
                            request->succeeded = Invoke(request->proc, request->parameter);
                        }
                    }
                }
            }
            return CallNextHookEx(nullptr, code, wParam, lParam);
        },
        nullptr, threadId);
    if (!hook)
        return false;

    auto request = std::make_shared<Dispatch>();
    request->proc = proc;
    request->parameter = parameter;
    ULONG_PTR id = g_nextRequestId.fetch_add(1, std::memory_order_relaxed);
    {
        std::lock_guard<std::mutex> lock(g_registryMutex);
        g_requests.emplace(id, request);
    }

    DWORD_PTR ignored = 0;
    BOOL delivered = FALSE;
    if (timeoutMs == INFINITE)
    {
        // Shutdown must finish removing callbacks and windows before the mod unloads.
        SendMessageW(window, message, 0, static_cast<LPARAM>(id));
        delivered = TRUE;
    }
    else
    {
        delivered = SendMessageTimeoutW(window, message, 0, static_cast<LPARAM>(id),
                                        SMTO_ABORTIFHUNG | SMTO_BLOCK, timeoutMs, &ignored);
    }
    {
        std::lock_guard<std::mutex> lock(g_registryMutex);
        g_requests.erase(id);
    }
    // Even if the message is processed late, it can no longer find the
    // request. A hook which already found it must finish before we return.
    bool succeeded = false;
    {
        std::lock_guard<std::mutex> lock(request->executionMutex);
        request->cancelled = true;
        succeeded = delivered && request->ran && request->succeeded;
    }
    UnhookWindowsHookEx(hook);
    return succeeded;
}

} // namespace folder_menus_dispatch
namespace folder_menus_taskbar_xaml
{

using winrt::Windows::UI::Xaml::FrameworkElement;
using winrt::Windows::UI::Xaml::XamlRoot;

// ---- XamlRoot ---------------------------------------------------------------

using CTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void*, void*);
using TaskbarHost_FrameHeight_t = int(WINAPI*)(void*);
using Ref_count_base_Decref_t = void(WINAPI*)(void*);
using TrayUI_StartTaskbar_t = void(WINAPI*)(void*);

inline CTaskBand_GetTaskbarHost_t CTaskBand_GetTaskbarHost_Original = nullptr;
inline TaskbarHost_FrameHeight_t TaskbarHost_FrameHeight_Original = nullptr;
inline Ref_count_base_Decref_t Ref_count_base_Decref_Original = nullptr;
inline TrayUI_StartTaskbar_t TrayUI_StartTaskbar_Original = nullptr;
inline void* CTaskBand_ITaskListWndSite_vftable = nullptr;
inline void* CSecondaryTaskBand_ITaskListWndSite_vftable = nullptr;
inline CTaskBand_GetTaskbarHost_t CSecondaryTaskBand_GetTaskbarHost_Original = nullptr;

// The mod's rebuild callback, invoked after Explorer rebuilds the taskbar.
inline void (*g_onTaskbarRebuilt)() = nullptr;

inline void WINAPI TrayUI_StartTaskbar_Hook(void* self)
{
    TrayUI_StartTaskbar_Original(self);
    try
    {
        if (g_onTaskbarRebuilt)
            g_onTaskbarRebuilt();
    }
    catch (...)
    {
        if (folder_menus_dispatch::g_logException)
            folder_menus_dispatch::g_logException(L"TrayUI::StartTaskbar hook");
    }
}

inline bool HookTaskbarSymbols(void (*onTaskbarRebuilt)())
{
    g_onTaskbarRebuilt = onTaskbarRebuilt;
    HMODULE taskbar = LoadLibraryExW(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!taskbar)
        return false;
    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {{LR"(const CTaskBand::`vftable'{for `ITaskListWndSite'})"},
         &CTaskBand_ITaskListWndSite_vftable},
        {{LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const )"},
         &CTaskBand_GetTaskbarHost_Original},
        {{LR"(public: int __cdecl TaskbarHost::FrameHeight(void)const )"},
         &TaskbarHost_FrameHeight_Original},
        {{LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))"},
         &Ref_count_base_Decref_Original},
        {{LR"(public: virtual void __cdecl TrayUI::StartTaskbar(void))"},
         &TrayUI_StartTaskbar_Original,
         TrayUI_StartTaskbar_Hook},
        // Secondary symbols are optional. Never fail the working primary clock.
        {{LR"(const CSecondaryTaskBand::`vftable'{for `ITaskListWndSite'})"},
         &CSecondaryTaskBand_ITaskListWndSite_vftable,
         nullptr,
         true},
        {{LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CSecondaryTaskBand::GetTaskbarHost(void)const )"},
         &CSecondaryTaskBand_GetTaskbarHost_Original,
         nullptr,
         true},
    };
    if (!WindhawkUtils::HookSymbols(taskbar, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks)))
        return false;
    bool secondaryOk = CSecondaryTaskBand_ITaskListWndSite_vftable &&
                       CSecondaryTaskBand_GetTaskbarHost_Original;
    Wh_Log(L"[Mawaqit][Monitor][Probe] secondary-symbols status=%s vtable=%p host=%p",
           secondaryOk ? L"ok" : L"missing", CSecondaryTaskBand_ITaskListWndSite_vftable,
           (void*)CSecondaryTaskBand_GetTaskbarHost_Original);
    return true;
}

// The FrameworkElement lives at an offset inside TaskbarHost that MOVES
// between Windows builds, so it is read out of TaskbarHost::FrameHeight's
// prologue at runtime rather than hardcoded.
inline size_t FrameworkElementOffset()
{
    size_t offset = 0x10;
#if defined(_M_X64)
    BYTE const* code = reinterpret_cast<BYTE const*>(TaskbarHost_FrameHeight_Original);
    if (code[0] == 0x48 && code[1] == 0x83 && code[2] == 0xEC && code[4] == 0x48 &&
        code[5] == 0x83 && code[6] == 0xC1 && code[7] <= 0x7F)
    {
        offset = code[7];
    }
#elif defined(_M_ARM64)
    DWORD const* code = reinterpret_cast<DWORD const*>(TaskbarHost_FrameHeight_Original);
    if (code[0] == 0xD503237F && (code[1] & 0xFFC07FFF) == 0xA9807BFD && code[2] == 0x910003FD &&
        (code[3] & 0xFFF00FE0) == 0xF8400C00)
    {
        offset = (code[3] >> 12) & 0xFF;
    }
#else
#error "Unsupported architecture"
#endif
    return offset;
}

inline XamlRoot GetTaskbarXamlRoot(HWND taskbarWnd, bool diagnostic = false)
{
    if (!CTaskBand_GetTaskbarHost_Original || !TaskbarHost_FrameHeight_Original ||
        !Ref_count_base_Decref_Original || !CTaskBand_ITaskListWndSite_vftable)
    {
        if (diagnostic)
            Wh_Log(L"[Mawaqit][Monitor][Probe] stage=symbols status=missing");
        return nullptr;
    }
    if (diagnostic)
        Wh_Log(L"[Mawaqit][Monitor][Probe] stage=symbols status=ok");

    HWND taskSwWnd = (HWND)GetProp(taskbarWnd, L"TaskbandHWND");
    if (!taskSwWnd)
    {
        if (diagnostic)
            Wh_Log(L"[Mawaqit][Monitor][Probe] stage=TaskbandHWND status=missing taskbar=%p",
                   taskbarWnd);
        return nullptr;
    }
    if (diagnostic)
        Wh_Log(L"[Mawaqit][Monitor][Probe] stage=TaskbandHWND status=ok hwnd=%p", taskSwWnd);
    void* taskBand = (void*)GetWindowLongPtr(taskSwWnd, 0);
    if (!taskBand)
    {
        if (diagnostic)
            Wh_Log(L"[Mawaqit][Monitor][Probe] stage=taskBand status=null");
        return nullptr;
    }
    if (diagnostic)
        Wh_Log(L"[Mawaqit][Monitor][Probe] stage=taskBand status=ok");

    void* site = taskBand;
    for (int i = 0; *(void**)site != CTaskBand_ITaskListWndSite_vftable; ++i)
    {
        if (i == 20)
        {
            if (diagnostic)
                Wh_Log(L"[Mawaqit][Monitor][Probe] stage=site-vtable status=not-found");
            return nullptr;
        }
        site = (void**)site + 1;
    }

    if (diagnostic)
        Wh_Log(L"[Mawaqit][Monitor][Probe] stage=site-vtable status=ok");
    void* host[2]{};
    CTaskBand_GetTaskbarHost_Original(site, host);
    if (!host[0] || !host[1])
    {
        if (diagnostic)
            Wh_Log(L"[Mawaqit][Monitor][Probe] stage=taskbar-host status=missing");
        if (host[1])
            Ref_count_base_Decref_Original(host[1]);
        return nullptr;
    }

    if (diagnostic)
        Wh_Log(L"[Mawaqit][Monitor][Probe] stage=taskbar-host status=ok");
    auto* unknown = *(IUnknown**)((BYTE*)host[0] + FrameworkElementOffset());
    if (!unknown)
    {
        if (diagnostic)
            Wh_Log(L"[Mawaqit][Monitor][Probe] stage=framework-element status=null");
        Ref_count_base_Decref_Original(host[1]);
        return nullptr;
    }
    if (diagnostic)
        Wh_Log(L"[Mawaqit][Monitor][Probe] stage=framework-element status=pointer-present");
    FrameworkElement element = nullptr;
    unknown->QueryInterface(winrt::guid_of<FrameworkElement>(), winrt::put_abi(element));
    if (diagnostic)
        Wh_Log(L"[Mawaqit][Monitor][Probe] stage=framework-element-QI status=%s",
               element ? L"ok" : L"failed");
    auto result = element ? element.XamlRoot() : nullptr;
    if (diagnostic)
        Wh_Log(L"[Mawaqit][Monitor][Probe] stage=xaml-root status=%s", result ? L"ok" : L"null");
    Ref_count_base_Decref_Original(host[1]);
    return result;
}

// Secondary taskbars use WorkerW and CSecondaryTaskBand, not TaskbandHWND.
// Mirrors the established Windhawk multirow taskbar resolution path.
inline XamlRoot GetSecondaryTaskbarXamlRoot(HWND taskbarWnd, bool diagnostic = false)
{
    if (!CSecondaryTaskBand_ITaskListWndSite_vftable ||
        !CSecondaryTaskBand_GetTaskbarHost_Original || !TaskbarHost_FrameHeight_Original ||
        !Ref_count_base_Decref_Original)
    {
        if (diagnostic)
            Wh_Log(L"[Mawaqit][Monitor][Probe] stage=secondary-symbols status=missing");
        return nullptr;
    }
    HWND worker = FindWindowExW(taskbarWnd, nullptr, L"WorkerW", nullptr);
    if (diagnostic)
        Wh_Log(L"[Mawaqit][Monitor][Probe] stage=secondary-WorkerW hwnd=%p", worker);
    if (!worker)
        return nullptr;
    void* taskBand = (void*)GetWindowLongPtrW(worker, 0);
    if (diagnostic)
        Wh_Log(L"[Mawaqit][Monitor][Probe] stage=secondary-taskBand ptr=%p", taskBand);
    if (!taskBand)
        return nullptr;
    void* site = taskBand;
    int offset = 0;
    for (; offset < 20; ++offset)
    {
        if (*(void**)site == CSecondaryTaskBand_ITaskListWndSite_vftable)
            break;
        site = (void**)site + 1;
    }
    if (diagnostic)
        Wh_Log(L"[Mawaqit][Monitor][Probe] stage=secondary-site offset=%d found=%d", offset,
               offset < 20);
    if (offset == 20)
        return nullptr;
    void* host[2]{};
    CSecondaryTaskBand_GetTaskbarHost_Original(site, host);
    if (diagnostic)
        Wh_Log(L"[Mawaqit][Monitor][Probe] stage=secondary-host ptr=%p ref=%p", host[0], host[1]);
    if (!host[0] || !host[1])
    {
        if (host[1])
            Ref_count_base_Decref_Original(host[1]);
        return nullptr;
    }
    auto* unknown = *(IUnknown**)((BYTE*)host[0] + FrameworkElementOffset());
    FrameworkElement element = nullptr;
    if (unknown)
        unknown->QueryInterface(winrt::guid_of<FrameworkElement>(), winrt::put_abi(element));
    auto root = element ? element.XamlRoot() : nullptr;
    if (diagnostic)
        Wh_Log(L"[Mawaqit][Monitor][Probe] stage=secondary-XamlRoot status=%s",
               root ? L"ok" : L"null");
    Ref_count_base_Decref_Original(host[1]);
    return root;
}

} // namespace folder_menus_taskbar_xaml

// Windhawk-managed per-mod cache, removed when the mod is uninstalled.
static std::wstring CacheDir()
{
    WCHAR path[MAX_PATH]{};
    if (!Wh_GetModStoragePath(path, ARRAYSIZE(path)))
        return L"";
    return path;
}

// Shared configuration ownership: refresh worker reads, settings loader writes.
// The generation changes only when the schedule's mosque or refresh interval changes.
static SRWLOCK g_settingsLock = SRWLOCK_INIT;
static std::atomic<unsigned long long> g_scheduleConfigGeneration{0};

namespace NativeUpdater
{

static void fail(const char* s)
{
    throw std::runtime_error(s);
}
static std::string narrow(const std::wstring& s)
{
    if (s.empty())
        return {};
    int n = WideCharToMultiByte(CP_UTF8, 0, s.data(), (int)s.size(), nullptr, 0, nullptr, nullptr);
    std::string r(n, 0);
    WideCharToMultiByte(CP_UTF8, 0, s.data(), (int)s.size(), r.data(), n, nullptr, nullptr);
    return r;
}
static std::string quote(const std::string& s)
{
    std::string o = "\"";
    for (unsigned char c : s)
    {
        switch (c)
        {
        case '"':
            o += "\\\"";
            break;
        case '\\':
            o += "\\\\";
            break;
        case '\n':
            o += "\\n";
            break;
        case '\r':
            o += "\\r";
            break;
        case '\t':
            o += "\\t";
            break;
        default:
            if (c < 32)
            {
                char b[7];
                snprintf(b, sizeof(b), "\\u%04x", c);
                o += b;
            }
            else
                o += (char)c;
        }
    }
    return o + "\"";
}

struct J
{
    enum Kind
    {
        Null,
        Str,
        Arr,
        Obj,
        Other
    } kind = Null;
    std::string s;
    std::vector<J> a;
    std::map<std::string, J> o;
    const J& key(const std::string& k) const
    {
        auto it = o.find(k);
        if (it == o.end())
            fail("Missing JSON key");
        return it->second;
    }
    const J& idx(size_t i) const
    {
        if (i >= a.size())
            fail("JSON array index missing");
        return a[i];
    }
};

struct Parser
{
    const std::string& s;
    size_t p = 0;
    int depth = 0;
    void ws()
    {
        while (p < s.size() && (s[p] == ' ' || s[p] == '\r' || s[p] == '\n' || s[p] == '\t'))
            p++;
    }
    void expect(char c)
    {
        ws();
        if (p >= s.size() || s[p++] != c)
            fail("Malformed JSON");
    }
    static void utf8(std::string& out, unsigned cp)
    {
        if (cp < 128)
            out += (char)cp;
        else if (cp < 2048)
        {
            out += (char)(0xC0 | (cp >> 6));
            out += (char)(0x80 | (cp & 63));
        }
        else if (cp < 65536)
        {
            out += (char)(0xE0 | (cp >> 12));
            out += (char)(0x80 | ((cp >> 6) & 63));
            out += (char)(0x80 | (cp & 63));
        }
        else
        {
            out += (char)(0xF0 | (cp >> 18));
            out += (char)(0x80 | ((cp >> 12) & 63));
            out += (char)(0x80 | ((cp >> 6) & 63));
            out += (char)(0x80 | (cp & 63));
        }
    }
    unsigned hex4()
    {
        if (p + 4 > s.size())
            fail("Invalid Unicode escape");
        unsigned v = 0;
        for (int i = 0; i < 4; i++)
        {
            char c = s[p++];
            v <<= 4;
            if (c >= '0' && c <= '9')
                v += c - '0';
            else if (c >= 'a' && c <= 'f')
                v += c - 'a' + 10;
            else if (c >= 'A' && c <= 'F')
                v += c - 'A' + 10;
            else
                fail("Invalid Unicode hex");
        }
        return v;
    }
    std::string str()
    {
        expect('"');
        std::string o;
        bool done = false;
        while (p < s.size())
        {
            unsigned char c = s[p++];
            if (c == '"')
            {
                done = true;
                break;
            }
            if (c < 32)
                fail("Control char in JSON string");
            if (c != '\\')
            {
                o += (char)c;
                continue;
            }
            if (p >= s.size())
                fail("Invalid JSON escape");
            char e = s[p++];
            switch (e)
            {
            case '"':
            case '\\':
            case '/':
                o += e;
                break;
            case 'n':
                o += '\n';
                break;
            case 'r':
                o += '\r';
                break;
            case 't':
                o += '\t';
                break;
            case 'b':
                o += '\b';
                break;
            case 'f':
                o += '\f';
                break;
            case 'u':
            {
                unsigned cp = hex4();
                if (cp >= 0xD800 && cp <= 0xDBFF)
                {
                    if (p + 2 > s.size() || s[p++] != '\\' || s[p++] != 'u')
                        fail("Bad surrogate");
                    unsigned low = hex4();
                    if (low < 0xDC00 || low > 0xDFFF)
                        fail("Bad low surrogate");
                    cp = 0x10000 + ((cp - 0xD800) << 10) + (low - 0xDC00);
                }
                else if (cp >= 0xDC00 && cp <= 0xDFFF)
                    fail("Bad surrogate");
                utf8(o, cp);
                break;
            }
            default:
                fail("Unknown JSON escape");
            }
        }
        if (!done)
            fail("Unterminated JSON string");
        return o;
    }
    J value()
    {
        ws();
        if (++depth > 80)
            fail("JSON nesting too deep");
        if (p >= s.size())
            fail("Truncated JSON");
        J v;
        char c = s[p];
        if (c == '"')
        {
            v.kind = J::Str;
            v.s = str();
        }
        else if (c == '{')
        {
            v.kind = J::Obj;
            p++;
            ws();
            if (p < s.size() && s[p] == '}')
                p++;
            else
                for (;;)
                {
                    std::string k = str();
                    expect(':');
                    v.o.emplace(std::move(k), value());
                    ws();
                    if (p >= s.size())
                        fail("Truncated object");
                    char t = s[p++];
                    if (t == '}')
                        break;
                    if (t != ',')
                        fail("Invalid object separator");
                }
        }
        else if (c == '[')
        {
            v.kind = J::Arr;
            p++;
            ws();
            if (p < s.size() && s[p] == ']')
                p++;
            else
                for (;;)
                {
                    v.a.push_back(value());
                    ws();
                    if (p >= s.size())
                        fail("Truncated array");
                    char t = s[p++];
                    if (t == ']')
                        break;
                    if (t != ',')
                        fail("Invalid array separator");
                }
        }
        else
        {
            v.kind = J::Other;
            size_t b = p;
            while (p < s.size() && s[p] != ',' && s[p] != ']' && s[p] != '}' && s[p] != ' ' &&
                   s[p] != '\r' && s[p] != '\n')
                p++;
            if (b == p)
                fail("Invalid JSON token");
            v.s = s.substr(b, p - b);
        }
        depth--;
        return v;
    }
};

struct Http
{
    HINTERNET h = nullptr;
    explicit Http(HINTERNET x) : h(x)
    {
        if (!h)
            fail("WinHTTP failed");
    }
    ~Http()
    {
        if (h)
            WinHttpCloseHandle(h);
    }
    Http(const Http&) = delete;
    Http& operator=(const Http&) = delete;
};

// Network access and cached schedules
//
// The mod retrieves the selected mosque's public prayer schedule
// using an HTTPS GET request to:
//   https://mawaqit.net/<language>/<mosque-identifier>
//
// Mawaqit can observe the requesting IP address and selected
// mosque identifier. No Mawaqit account is required.
//
// Schedules are cached locally in Windhawk-managed mod storage.
// Normal refresh: every RefreshHours hours (default: 24).
// Failed download: retry after approximately one hour.
// Mosque or refresh interval changed: request a fresh schedule.
// Cache format upgrade to v4: download a fresh schedule once.
//
// Network requests use WinHTTP over HTTPS with redirects disabled.
// The maximum accepted response size is 8 MB.
static std::string fetch(const std::wstring& path)
{
    Http session(WinHttpOpen(L"EmberveilMawaqit/0.5", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
                             WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0));
    WinHttpSetTimeouts(session.h, 5000, 5000, 15000, 15000);
    Http conn(WinHttpConnect(session.h, L"mawaqit.net", INTERNET_DEFAULT_HTTPS_PORT, 0));
    Http req(WinHttpOpenRequest(conn.h, L"GET", path.c_str(), nullptr, WINHTTP_NO_REFERER,
                                WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE));
    DWORD redirects = WINHTTP_OPTION_REDIRECT_POLICY_NEVER;
    WinHttpSetOption(req.h, WINHTTP_OPTION_REDIRECT_POLICY, &redirects, sizeof(redirects));
    if (!WinHttpSendRequest(req.h, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0,
                            0) ||
        !WinHttpReceiveResponse(req.h, nullptr))
        fail("HTTP request failed");
    DWORD status = 0, len = sizeof(status);
    if (!WinHttpQueryHeaders(req.h, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
                             WINHTTP_HEADER_NAME_BY_INDEX, &status, &len,
                             WINHTTP_NO_HEADER_INDEX) ||
        status != 200)
        fail("Mawaqit did not return HTTP 200");
    std::string result;
    char b[8192];
    for (;;)
    {
        DWORD got = 0;
        if (!WinHttpReadData(req.h, b, sizeof(b), &got))
            fail("HTTP read failed");
        if (!got)
            break;
        if (result.size() + got > 8 * 1024 * 1024)
            fail("HTML too large");
        result.append(b, got);
    }
    return result;
}

static std::string extract(const std::string& html)
{
    size_t pos = html.find("let confData");
    if (pos == std::string::npos)
        fail("Mawaqit confData not found");
    pos = html.find('=', pos);
    if (pos == std::string::npos)
        fail("confData assignment missing");
    pos = html.find('{', pos);
    if (pos == std::string::npos)
        fail("confData object missing");
    // The parser consumes exactly one JSON object, so nested braces and quoted strings are safe.
    std::string rest = html.substr(pos);
    Parser p{rest};
    J root = p.value();
    if (root.kind != J::Obj)
        fail("confData is not an object");
    return rest.substr(0, p.p);
}

static bool leap(int y)
{
    return y % 4 == 0 && (y % 100 != 0 || y % 400 == 0);
}
static int days(int y, int m)
{
    static const int d[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    return d[m - 1] + (m == 2 && leap(y));
}
static std::string date(int y, int m, int d)
{
    char b[16];
    snprintf(b, sizeof(b), "%04d-%02d-%02d", y, m, d);
    return b;
}
static bool validTime(const std::string& t)
{
    return t.size() == 5 && t[2] == ':' && t[0] >= '0' && t[0] <= '2' && t[1] >= '0' &&
           t[1] <= '9' && t[3] >= '0' && t[3] <= '5' && t[4] >= '0' && t[4] <= '9' &&
           ((t[0] - '0') * 10 + t[1] - '0') < 24;
}
static void writeFile(const std::wstring& path, const std::string& content)
{
    HANDLE h = CreateFileW(path.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_NEW,
                           FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h == INVALID_HANDLE_VALUE)
        fail("Cannot create temporary cache file");
    DWORD written = 0;
    BOOL ok = WriteFile(h, content.data(), (DWORD)content.size(), &written, nullptr);
    FlushFileBuffers(h);
    CloseHandle(h);
    if (!ok || written != content.size())
        fail("Cache write failed");
}
static void replace(const std::wstring& from, const std::wstring& to)
{
    if (!MoveFileExW(from.c_str(), to.c_str(), MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH))
        fail("Cache replace failed");
}

static std::wstring normalize(std::wstring input, std::wstring& slug)
{
    // Accept public and admin page links, but always download from the public host.
    for (const wchar_t* prefix : {L"https://mawaqit.net/", L"https://admin.mawaqit.net/"})
    {
        std::wstring candidate(prefix);
        if (input.compare(0, candidate.size(), candidate) == 0)
        {
            input.erase(0, candidate.size());
            break;
        }
    }
    if (input.empty() || input.find_first_of(L"?#\\@") != std::wstring::npos ||
        input.find(L"://") != std::wstring::npos)
        fail("Invalid mosque URL/identifier");
    if (input.back() == L'/')
        input.pop_back();
    std::wstring lang = L"nl";
    size_t slash = input.find(L'/');
    if (slash != std::wstring::npos)
    {
        lang = input.substr(0, slash);
        slug = input.substr(slash + 1);
    }
    else
        slug = input;
    // Both /<language>/<slug> and /<language>/m/<slug> identify the same mosque.
    if (slug.compare(0, 2, L"m/") == 0)
        slug.erase(0, 2);
    auto valid = [](wchar_t c)
    {
        return (c >= L'a' && c <= L'z') || (c >= L'A' && c <= L'Z') || (c >= L'0' && c <= L'9') ||
               c == L'_' || c == L'-';
    };
    if (lang.size() != 2 ||
        !std::all_of(lang.begin(), lang.end(), [](wchar_t c) { return c >= L'a' && c <= L'z'; }) ||
        slug.empty() || slug.size() > 120 || !std::all_of(slug.begin(), slug.end(), valid))
        fail("Invalid mosque slug or language");
    return L"/" + lang + L"/" + slug;
}

static bool Run(const std::wstring& mosque, unsigned long long expectedGeneration)
{
    std::wstring slug;
    std::wstring path = normalize(mosque, slug);
    std::string html = fetch(path);
    std::string json = extract(html);
    Parser parser{json};
    J root = parser.value();
    const J& cal = root.key("calendar");
    if (cal.kind != J::Arr || cal.a.size() != 12)
        fail("Calendar must have 12 months");
    // Iqamah is optional. Respect the mosque's explicit enable flag.
    const auto enabledIt = root.o.find("iqamaEnabled");
    const bool iqamaEnabled = enabledIt != root.o.end() &&
                              enabledIt->second.s == "true";
    const auto iqamaIt = root.o.find("iqamaCalendar");
    const J* iqamaCal = iqamaEnabled && iqamaIt != root.o.end() &&
                        iqamaIt->second.kind == J::Arr && iqamaIt->second.a.size() == 12
                            ? &iqamaIt->second : nullptr;
    const J& nameVal = root.key("name");
    const J& tzVal = root.key("timezone");
    if (tzVal.kind != J::Str || tzVal.s.empty())
        fail("Missing timezone");
    std::string name = nameVal.kind == J::Str && !nameVal.s.empty() ? nameVal.s : narrow(slug);
    // Use the local Windows date for the cached schedule year.
    SYSTEMTIME now;
    GetLocalTime(&now);
    int year = now.wYear;
    std::ostringstream sched;
    sched << "# Emberveil Mawaqit schedule v4\n# Source: https://mawaqit.net" << narrow(path)
          << "\n# Year: " << year << "\n";
    for (int m = 1; m <= 12; m++)
    {
        const J& month = cal.idx(m - 1);
        if (month.kind != J::Obj)
            fail("Month is not an object");
        for (int d = 1; d <= days(year, m); d++)
        {
            const J& entry = month.key(std::to_string(d));
            if (entry.kind != J::Arr || (entry.a.size() != 6 && entry.a.size() != 7))
                fail("Invalid calendar day");
            // Seven-entry calendars include Imsak before Fajr and Shuruq.
            // Six-entry calendars begin with Fajr and Shuruq.
            const bool hasImsak = entry.a.size() == 7;
            sched << date(year, m, d);
            for (int i : {0, 2, 3, 4, 5})
            {
                const J& t = entry.idx(i + (hasImsak ? 1 : 0));
                if (t.kind != J::Str || !validTime(t.s))
                    fail("Invalid prayer time");
                sched << '|' << t.s;
            }
            // Five optional Iqamah values follow the five prayer times.
            // Preserve +N or HH:mm; invalid/missing values become empty fields.
            const J* iqamaDay = nullptr;
            if (iqamaCal)
            {
                const J& iqamaMonth = iqamaCal->idx(m - 1);
                if (iqamaMonth.kind == J::Obj)
                {
                    auto it = iqamaMonth.o.find(std::to_string(d));
                    if (it != iqamaMonth.o.end() && it->second.kind == J::Arr &&
                        it->second.a.size() == 5)
                        iqamaDay = &it->second;
                }
            }
            for (int i = 0; i < 5; ++i)
            {
                sched << '|';
                if (!iqamaDay)
                {
                    sched << '-';
                    continue;
                }
                const J& value = iqamaDay->idx(i);
                if (value.kind != J::Str)
                {
                    sched << '-';
                    continue;
                }
                const std::string& raw = value.s;
                bool relative = raw.size() >= 2 && raw.size() <= 4 && raw[0] == '+' &&
                    std::all_of(raw.begin() + 1, raw.end(), [](char c) {
                        return c >= '0' && c <= '9';
                    });
                if ((relative && std::stoi(raw.substr(1)) <= 180) || validTime(raw))
                    sched << raw;
                else
                    sched << '-';
            }
            // Shuruq follows Fajr; seven-entry calendars have an Imsak prefix.
            const J& sunrise = entry.idx(hasImsak ? 2 : 1);
            if (sunrise.kind != J::Str || !validTime(sunrise.s))
                fail("Invalid Shuruq time");
            sched << '|' << sunrise.s;
            // Append optional Imsak after the existing v3 fields.
            sched << '|';
            if (hasImsak)
            {
                const J& imsak = entry.idx(0);
                if (imsak.kind != J::Str || !validTime(imsak.s))
                    fail("Invalid Imsak time");
                sched << imsak.s;
            }
            else
            {
                sched << '-';
            }
            sched << '\n';
        }
    }
    std::wstring dir = CacheDir();
    if (dir.empty())
        fail("Windhawk mod storage unavailable");
    if (!CreateDirectoryW(dir.c_str(), nullptr) && GetLastError() != ERROR_ALREADY_EXISTS)
        fail("Cannot create cache folder");
    // Retain existing Hijri date metadata only if it is from the same mosque and day.
    std::string hijri = "", hijriSource = "unavailable";
    std::wstring old = dir + L"\\metadata.json";
    {
        HANDLE h = CreateFileW(old.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
                               nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (h != INVALID_HANDLE_VALUE)
        {
            DWORD sz = GetFileSize(h, nullptr);
            if (sz < 65536)
            {
                std::string data(sz, 0);
                DWORD got = 0;
                if (ReadFile(h, data.data(), sz, &got, nullptr))
                {
                    try
                    {
                        Parser mp{data};
                        J prev = mp.value();
                        if (prev.key("mosqueIdentifier").s == narrow(slug) &&
                            prev.key("localDate").s == date(now.wYear, now.wMonth, now.wDay))
                        {
                            hijri = prev.key("hijriDate").s;
                            hijriSource = prev.key("hijriDateSource").s;
                        }
                    }
                    catch (...)
                    {
                    }
                }
            }
            CloseHandle(h);
        }
    }
    FILETIME ft;
    GetSystemTimeAsFileTime(&ft);
    SYSTEMTIME utc;
    GetSystemTime(&utc);
    char stamp[40];
    snprintf(stamp, sizeof(stamp), "%04u-%02u-%02uT%02u:%02u:%02uZ", utc.wYear, utc.wMonth,
             utc.wDay, utc.wHour, utc.wMinute, utc.wSecond);
    std::ostringstream meta;
    meta << "{\n  \"schemaVersion\": 1,\n  \"mosqueName\": " << quote(name)
         << ",\n  \"mosqueIdentifier\": " << quote(narrow(slug))
         << ",\n  \"mosqueUrl\": " << quote("https://mawaqit.net" + narrow(path))
         << ",\n  \"timezone\": " << quote(tzVal.s) << ",\n  \"scheduleYear\": " << year
         << ",\n  \"localDate\": " << quote(date(now.wYear, now.wMonth, now.wDay))
         << ",\n  \"hijriDate\": " << quote(hijri)
         << ",\n  \"hijriDateSource\": " << quote(hijriSource)
         << ",\n  \"updatedAtUtc\": " << quote(stamp) << "\n}\n";
    std::wstring tmp1 = dir + L"\\schedule.native.tmp", tmp2 = dir + L"\\metadata.native.tmp";
    DeleteFileW(tmp1.c_str());
    DeleteFileW(tmp2.c_str());
    try
    {
        writeFile(tmp1, sched.str());
        writeFile(tmp2, meta.str());
        // A settings change while fetching must never publish the obsolete
        // mosque's schedule. Serialize validation and both cache replacements
        // against settings updates.
        AcquireSRWLockShared(&g_settingsLock);
        if (g_scheduleConfigGeneration.load(std::memory_order_acquire) != expectedGeneration)
        {
            ReleaseSRWLockShared(&g_settingsLock);
            DeleteFileW(tmp1.c_str());
            DeleteFileW(tmp2.c_str());
            Wh_Log(L"[Mawaqit] Discarded obsolete schedule download before cache commit");
            return false;
        }
        try
        {
            replace(tmp1, dir + L"\\schedule.txt");
            replace(tmp2, dir + L"\\metadata.json");
        }
        catch (...)
        {
            ReleaseSRWLockShared(&g_settingsLock);
            throw;
        }
        ReleaseSRWLockShared(&g_settingsLock);
    }
    catch (...)
    {
        DeleteFileW(tmp1.c_str());
        DeleteFileW(tmp2.c_str());
        throw;
    }
    return true;
}
} // namespace NativeUpdater

static std::wstring g_mosqueName = L"Mosque", g_hijriDate = L"";
struct Prayer
{
    std::wstring name;
    int minute;
    int iqamahDelay = -1; // -1 = unavailable, otherwise minutes after adhan.
};
static bool g_showIqamah = true;
static bool g_showFajrWindowCountdown = false;
static int g_shuruqMinute = -1;
static int g_imsakMinute = -1;
static int ParseIqamahDelay(const std::wstring& raw, int prayerMinute)
{
    if (raw.size() >= 2 && raw.size() <= 4 && raw[0] == L'+')
    {
        int delay = 0;
        for (size_t i = 1; i < raw.size(); ++i)
        {
            if (raw[i] < L'0' || raw[i] > L'9')
                return -1;
            delay = delay * 10 + (raw[i] - L'0');
        }
        return delay <= 180 ? delay : -1;
    }
    if (raw.size() == 5 && raw[2] == L':' &&
        raw[0] >= L'0' && raw[0] <= L'2' && raw[1] >= L'0' && raw[1] <= L'9' &&
        raw[3] >= L'0' && raw[3] <= L'5' && raw[4] >= L'0' && raw[4] <= L'9')
    {
        int h = (raw[0] - L'0') * 10 + raw[1] - L'0';
        if (h > 23)
            return -1;
        int absolute = h * 60 + (raw[3] - L'0') * 10 + raw[4] - L'0';
        int delay = (absolute - prayerMinute + 1440) % 1440;
        return delay <= 180 ? delay : -1;
    }
    return -1;
}
static std::wstring IqamahSuffix(const Prayer& prayer)
{
    return g_showIqamah && prayer.iqamahDelay >= 0
        ? L"+" + std::to_wstring(prayer.iqamahDelay) : L"";
}
static const wchar_t* kNames[] = {L"Fajr", L"Dhuhr", L"Asr", L"Maghrib", L"Isha"};
static std::wstring g_label = L"Loading schedule", g_countdown = L"--:--";
static std::wstring g_iqamahSuffix;
static bool g_showCountdown = true;
static std::wstring PillCountdown()
{
    return g_countdown;
}
static std::wstring PillIqamah()
{
    return g_showCountdown && g_showIqamah ? g_iqamahSuffix : L"";
}
static std::wstring g_loadedDate;
static std::vector<Prayer> g_yesterday, g_today, g_tomorrow;

static std::wstring DateKey(const SYSTEMTIME& t)
{
    wchar_t b[16];
    swprintf(b, 16, L"%04u-%02u-%02u", t.wYear, t.wMonth, t.wDay);
    return b;
}

static std::wstring CachePath()
{
    std::wstring dir = CacheDir();
    return dir.empty() ? L"" : dir + L"\\schedule.txt";
}

static std::wstring Utf8ToWide(const std::string& s)
{
    int n = MultiByteToWideChar(CP_UTF8, 0, s.data(), (int)s.size(), nullptr, 0);
    if (n <= 0)
        return L"";
    std::wstring result(n, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, s.data(), (int)s.size(), &result[0], n);
    return result;
}
// Minimal JSON string reader for the two fields written by our trusted updater.
// Handles standard JSON escapes and Unicode escapes, including surrogate pairs.

static std::wstring JsonField(const std::wstring& json, const wchar_t* key)
{
    std::wstring token = L"\"" + std::wstring(key) + L"\"";
    size_t at = json.find(token);
    if (at == std::wstring::npos)
        return L"";
    at = json.find(L':', at + token.size());
    if (at == std::wstring::npos)
        return L"";
    at = json.find_first_not_of(L" \r\n\t", at + 1);
    if (at == std::wstring::npos || json[at] != L'"')
        return L"";
    std::wstring out;
    for (size_t i = at + 1; i < json.size(); i++)
    {
        wchar_t c = json[i];
        if (c == L'"')
            break;
        if (c != L'\\')
        {
            out += c;
            continue;
        }
        if (++i >= json.size())
            break;
        c = json[i];
        if (c == L'u' && i + 4 < json.size())
        {
            unsigned code = 0;
            bool valid = true;
            for (int j = 0; j < 4; j++)
            {
                wchar_t h = json[i + 1 + j];
                unsigned v;
                if (h >= L'0' && h <= L'9')
                    v = h - L'0';
                else if (h >= L'a' && h <= L'f')
                    v = h - L'a' + 10;
                else if (h >= L'A' && h <= L'F')
                    v = h - L'A' + 10;
                else
                {
                    valid = false;
                    break;
                }
                code = code * 16 + v;
            }
            if (valid)
            {
                out += (wchar_t)code;
                i += 4;
            }
            continue;
        }
        switch (c)
        {
        case L'n':
            out += L'\n';
            break;
        case L'r':
            out += L'\r';
            break;
        case L't':
            out += L'\t';
            break;
        default:
            out += c;
            break;
        }
    }
    return out;
}

static void LoadMetadata()
{
    g_mosqueName = L"Mosque";
    g_hijriDate = L"";
    std::wstring path = CachePath();
    if (path.empty())
        return;
    size_t slash = path.find_last_of(L'\\');
    if (slash == std::wstring::npos)
        return;
    path = path.substr(0, slash + 1) + L"metadata.json";
    std::ifstream f(path.c_str(), std::ios::binary);
    if (!f)
        return;
    std::string bytes((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
    if (bytes.size() > 65536)
        return;
    std::wstring json = Utf8ToWide(bytes);
    std::wstring name = JsonField(json, L"mosqueName");
    if (!name.empty())
        g_mosqueName = name;
    std::wstring hijri = JsonField(json, L"hijriDate");
    if (hijri.size() == 10 && hijri[4] == L'-' && hijri[7] == L'-')
    {
        static const wchar_t* months[] = {L"Muharram",       L"Safar",         L"Rabi' al-Awwal",
                                          L"Rabi' al-Thani", L"Jumada al-Ula", L"Jumada al-Akhirah",
                                          L"Rajab",          L"Sha'ban",       L"Ramadan",
                                          L"Shawwal",        L"Dhu al-Qi'dah", L"Dhu al-Hijjah"};
        int month = _wtoi(hijri.substr(5, 2).c_str()), day = _wtoi(hijri.substr(8, 2).c_str());
        if (month >= 1 && month <= 12 && day >= 1 && day <= 30)
        {
            wchar_t text[120];
            swprintf(text, 120, L"%d %ls %ls AH", day, months[month - 1],
                     hijri.substr(0, 4).c_str());
            g_hijriDate = text;
        }
    }
}

static bool ParseDay(const std::wstring& line, const std::wstring& date, std::vector<Prayer>& out)
{
    if (line.size() < date.size() + 1 || line.compare(0, date.size(), date) != 0 ||
        line[date.size()] != L'|')
        return false;
    std::wistringstream s(line.substr(date.size() + 1));
    std::wstring item;
    std::vector<Prayer> result;
    for (int i = 0; i < 5; i++)
    {
        if (!std::getline(s, item, L'|') || item.size() != 5 || item[2] != L':' || item[0] < L'0' ||
            item[0] > L'2' || item[1] < L'0' || item[1] > L'9' || item[3] < L'0' ||
            item[3] > L'5' || item[4] < L'0' || item[4] > L'9')
            return false;
        int hour = (item[0] - L'0') * 10 + item[1] - L'0';
        int minute = (item[3] - L'0') * 10 + item[4] - L'0';
        if (hour > 23)
            return false;
        result.push_back({kNames[i], hour * 60 + minute});
    }
    // v1 caches contain only prayer times; v2 adds five Iqamah fields.
    if (std::getline(s, item, L'|'))
    {
        result[0].iqamahDelay = ParseIqamahDelay(item, result[0].minute);
        for (int i = 1; i < 5; ++i)
        {
            if (!std::getline(s, item, L'|'))
                return false;
            result[i].iqamahDelay = ParseIqamahDelay(item, result[i].minute);
        }
        // v3 adds Shuruq; newer caches optionally append Imsak.
        if (std::getline(s, item, L'|'))
        {
            if (std::getline(s, item, L'|') && std::getline(s, item, L'|'))
                return false;
        }
    }
    out = result;
    return true;
}

static int ParseShuruq(const std::wstring& line)
{
    // v3: date | five adhan times | five Iqamah fields | Shuruq.
    std::vector<std::wstring> fields;
    std::wistringstream stream(line);
    std::wstring part;
    while (std::getline(stream, part, L'|'))
        fields.push_back(part);
    if (fields.size() != 12 && fields.size() != 13)
        return -1;
    const std::wstring& time = fields[11];
    if (time.size() != 5 || time[2] != L':' ||
        time[0] < L'0' || time[0] > L'2' ||
        time[1] < L'0' || time[1] > L'9' ||
        time[3] < L'0' || time[3] > L'5' ||
        time[4] < L'0' || time[4] > L'9')
        return -1;
    int hour = (time[0] - L'0') * 10 + (time[1] - L'0');
    return hour <= 23 ? hour * 60 + (time[3] - L'0') * 10 + time[4] - L'0' : -1;
}

static int ParseImsak(const std::wstring& line)
{
    // date | five adhan | five Iqamah | Shuruq | optional Imsak.
    std::vector<std::wstring> fields;
    std::wistringstream stream(line);
    std::wstring part;
    while (std::getline(stream, part, L'|'))
        fields.push_back(part);
    if (fields.size() != 13)
        return -1;
    const std::wstring& time = fields[12];
    if (time.size() != 5 || time[2] != L':' ||
        time[0] < L'0' || time[0] > L'2' ||
        time[1] < L'0' || time[1] > L'9' ||
        time[3] < L'0' || time[3] > L'5' ||
        time[4] < L'0' || time[4] > L'9')
        return -1;
    int hour = (time[0] - L'0') * 10 + time[1] - L'0';
    return hour <= 23 ? hour * 60 + (time[3] - L'0') * 10 + time[4] - L'0' : -1;
}

static void LoadSchedule(const SYSTEMTIME& now)
{
    std::wstring today = DateKey(now);
    if (today == g_loadedDate)
        return;
    FILETIME ft;
    SystemTimeToFileTime(&now, &ft);
    ULARGE_INTEGER v;
    v.LowPart = ft.dwLowDateTime;
    v.HighPart = ft.dwHighDateTime;
    SYSTEMTIME previous, next;
    v.QuadPart -= 864000000000ULL;
    ft.dwLowDateTime = v.LowPart;
    ft.dwHighDateTime = v.HighPart;
    FileTimeToSystemTime(&ft, &previous);
    std::wstring yesterday = DateKey(previous);
    v.QuadPart += 2ULL * 864000000000ULL;
    ft.dwLowDateTime = v.LowPart;
    ft.dwHighDateTime = v.HighPart;
    FileTimeToSystemTime(&ft, &next);
    std::wstring tomorrow = DateKey(next);
    std::wstring path = CachePath();
    std::wifstream file(path.c_str());
    if (!file)
        return;
    std::wstring line;
    std::vector<Prayer> previousDay, a, b;
    int sunrise = -1;
    int imsak = -1;
    while (std::getline(file, line))
    {
        if (previousDay.empty())
            ParseDay(line, yesterday, previousDay);
        if (a.empty() && ParseDay(line, today, a))
        {
            sunrise = ParseShuruq(line);
            imsak = ParseImsak(line);
        }
        if (b.empty())
            ParseDay(line, tomorrow, b);
        if (!previousDay.empty() && !a.empty() && !b.empty())
            break;
    }
    if (a.size() != 5)
        return;
    g_yesterday = previousDay;
    g_today = a;
    g_shuruqMinute = sunrise;
    g_imsakMinute = imsak;
    g_tomorrow = b;
    g_loadedDate = today;
}

static void UpdateCountdown()
{
    SYSTEMTIME now;
    GetLocalTime(&now);
    LoadSchedule(now);
    g_iqamahSuffix.clear();
    if (g_today.empty())
    {
        g_label = L"Schedule unavailable";
        g_countdown = L"--:--";
        return;
    }
    int nowSec = now.wHour * 3600 + now.wMinute * 60 + now.wSecond;
    if (g_showFajrWindowCountdown && g_shuruqMinute >= 0 &&
        !g_today.empty() && nowSec >= g_today.front().minute * 60 &&
        nowSec < g_shuruqMinute * 60)
    {
        int sec = g_shuruqMinute * 60 - nowSec;
        wchar_t label[80], count[32];
        swprintf(label, 80, L"Fajr ends at %02d:%02d",
                 g_shuruqMinute / 60, g_shuruqMinute % 60);
        swprintf(count, 32, L"-%d:%02d:%02d", sec / 3600, (sec / 60) % 60, sec % 60);
        g_label = label;
        g_countdown = count;
        return;
    }
    // After midnight, yesterday's Isha may still be awaiting its scheduled Iqamah.
    // Use only the cached source schedule; never synthesize a missing Iqamah.
    if (g_yesterday.size() == 5)
    {
        const auto& isha = g_yesterday.back();
        if (isha.iqamahDelay > 0)
        {
            int adhanSec = isha.minute * 60;
            int iqamahSec = adhanSec + isha.iqamahDelay * 60;
            int nextAdhanSec = 86400 + g_today.front().minute * 60;
            iqamahSec = std::min(iqamahSec, nextAdhanSec);
            int currentSec = 86400 + nowSec;
            if (iqamahSec > 86400 && currentSec < iqamahSec)
            {
                int iqamahMinute = (isha.minute + isha.iqamahDelay) % 1440;
                wchar_t label[80];
                swprintf(label, 80, L"%ls  %02d:%02d", isha.name.c_str(),
                         iqamahMinute / 60, iqamahMinute % 60);
                g_label = label;
                g_countdown = L"--:--";
                return;
            }
        }
    }
    // Keep the current prayer active from its Adhan until its scheduled Iqamah.
    // This is schedule-driven and independent of the Iqamah display preference.
    // Missing/invalid Iqamah data preserves the existing Adhan-to-Adhan behavior.
    for (size_t i = 0; i < g_today.size(); ++i)
    {
        const auto& p = g_today[i];
        if (p.iqamahDelay <= 0)
            continue;
        int adhanSec = p.minute * 60;
        int iqamahSec = adhanSec + p.iqamahDelay * 60;
        // Do not keep a prayer active past the following scheduled Adhan.
        if (i + 1 < g_today.size())
            iqamahSec = std::min(iqamahSec, g_today[i + 1].minute * 60);
        if (nowSec >= adhanSec && nowSec < iqamahSec)
        {
            int iqamahMinute = (p.minute + p.iqamahDelay) % 1440;
            wchar_t label[80];
            swprintf(label, 80, L"%ls  %02d:%02d", p.name.c_str(),
                     iqamahMinute / 60, iqamahMinute % 60);
            g_label = label;
            g_countdown = L"--:--";
            return;
        }
    }
    for (const auto& p : g_today)
    {
        int sec = p.minute * 60 - nowSec;
        if (sec > 0)
        {
            wchar_t label[80], count[32];
            swprintf(label, 80, L"%ls  %02d:%02d", p.name.c_str(), p.minute / 60, p.minute % 60);
            swprintf(count, 32, L"-%d:%02d:%02d", sec / 3600, (sec / 60) % 60, sec % 60);
            g_label = label;
            g_countdown = count;
            g_iqamahSuffix = IqamahSuffix(p);
            return;
        }
    }
    if (!g_tomorrow.empty())
    {
        const auto& p = g_tomorrow.front();
        int sec = 86400 - nowSec + p.minute * 60;
        wchar_t label[80], count[32];
        swprintf(label, 80, L"%ls  %02d:%02d", p.name.c_str(), p.minute / 60, p.minute % 60);
        swprintf(count, 32, L"-%d:%02d:%02d", sec / 3600, (sec / 60) % 60, sec % 60);
        g_label = label;
        g_countdown = count;
        g_iqamahSuffix = IqamahSuffix(p);
    }
    else
    {
        g_label = L"Tomorrow unavailable";
        g_countdown = L"--:--";
    }
}

namespace slot = folder_menus_slot_lease;
namespace taskwin = folder_menus_taskbar_window;
namespace taskxaml = folder_menus_taskbar_xaml;

static std::atomic<bool> g_stopping{false};
// TrayUI::StartTaskbar can recreate XAML without restarting explorer.exe.
static std::atomic<bool> g_taskbarRebuildPending{false};
static std::atomic<bool> g_settingsPending{false};

static slot::Lease g_lease{};
static winrt::weak_ref<FrameworkElement> g_placeholder;
static winrt::weak_ref<Panel> g_parent;
struct Insets
{
    int left = 0, top = 0, right = 0, bottom = 0;
};
static Insets ParseInsets(const std::wstring& value, int minVal, int maxVal)
{
    Insets result{};
    int values[4]{};
    if (swscanf_s(value.c_str(), L" %d , %d , %d , %d ", &values[0], &values[1], &values[2],
                  &values[3]) == 4)
    {
        for (int& v : values)
            v = std::clamp(v, minVal, maxVal);
        result = {values[0], values[1], values[2], values[3]};
    }
    return result;
}
static int g_width = 0, g_pillHeight = 0, g_trayGap = 5, g_cornerRadius = 13, g_fontSize = 15;
static int g_iqamahFontScale = 80;
static Insets g_margins{}, g_padding{};
static std::wstring g_theme = L"system";
static std::wstring g_backgroundStyle = L"solid";
static int g_backgroundOpacity = 100;
static bool g_pillTextShadow = false;
static bool g_showPillBorder = false;
// Popup options are independent of the XAML pill. -1 inherits legacy opacity.
static std::wstring g_popupBackgroundStyle = L"follow-pill";
static int g_popupBackgroundOpacity = -1;
static bool g_popupShowBorder = true, g_popupShowSeparators = true;
static const std::wstring& PopupMaterial()
{
    return g_popupBackgroundStyle == L"follow-pill" ? g_backgroundStyle : g_popupBackgroundStyle;
}
static int PopupOpacity()
{
    return g_popupBackgroundOpacity < 0 ? g_backgroundOpacity : g_popupBackgroundOpacity;
}
static std::wstring g_position = L"before-tray";
static bool g_showPrimaryMonitor = true, g_showSecondaryMonitors = true;
struct PillPalette
{
    winrt::Windows::UI::Color background, foreground, countdown, separator;
};
// Taskbar pill background material is independent from popup composition.
// XAML AcrylicBrush may render FallbackColor when hosted inside Explorer's taskbar.
static Brush CreatePillBackground(winrt::Windows::UI::Color color)
{
    if (g_backgroundStyle == L"transparent" || g_backgroundOpacity == 0)
        return nullptr;
    const double opacity = (double)g_backgroundOpacity / 100.0;
    if (g_backgroundStyle == L"blur" || g_backgroundStyle == L"acrylic")
    {
        try
        {
            AcrylicBrush material;
            // Backdrop is used because HostBackdrop may render as a solid
            // fallback inside Explorer. Tint strengths distinguish the materials.
            auto source = AcrylicBackgroundSource::Backdrop;
            material.BackgroundSource(source);
            material.TintColor(color);
            material.TintOpacity(g_backgroundStyle == L"blur" ? 0.12 * opacity
                                                              : 0.45 * opacity);
            material.Opacity(opacity);
            material.FallbackColor(color);
            Brush assigned = material;
            auto actual = assigned.try_as<AcrylicBrush>();
            Wh_Log(L"[Mawaqit][PillMaterial] requested=%s opacity=%d "
                   L"acrylicBrush=%d source=%s tintOpacity=%.3f "
                   L"brushOpacity=%.3f fallback=#%02X%02X%02X",
                   g_backgroundStyle.c_str(), g_backgroundOpacity,
                   actual ? 1 : 0,
                   source == AcrylicBackgroundSource::Backdrop ? L"Backdrop" : L"HostBackdrop",
                   material.TintOpacity(), material.Opacity(),
                   (unsigned)color.R, (unsigned)color.G, (unsigned)color.B);
            return assigned;
        }
        catch (winrt::hresult_error const& e)
        {
            Wh_Log(L"[Mawaqit][PillMaterial] %s acrylic failed HRESULT=0x%08X: %s; "
                   L"using solid fallback", g_backgroundStyle.c_str(),
                   (unsigned)e.code().value, e.message().c_str());
        }
        catch (...)
        {
            Wh_Log(L"[Mawaqit][PillMaterial] %s acrylic failed (unknown exception); "
                   L"using solid fallback", g_backgroundStyle.c_str());
        }
    }
    SolidColorBrush brush;
    brush.Color(color);
    brush.Opacity(opacity);
    return brush;
}

static winrt::Windows::UI::Color Rgb(unsigned c)
{
    return winrt::Windows::UI::Color{255, (uint8_t)(c >> 16), (uint8_t)(c >> 8), (uint8_t)c};
}
static COLORREF ParseThemeHex(const std::wstring& input, COLORREF fallback)
{
    if (input.size() != 7 || input[0] != L'#')
        return fallback;
    unsigned value = 0;
    for (size_t i = 1; i < 7; i++)
    {
        wchar_t c = input[i];
        unsigned digit = (c >= L'0' && c <= L'9')   ? (unsigned)(c - L'0')
                         : (c >= L'a' && c <= L'f') ? (unsigned)(c - L'a' + 10)
                         : (c >= L'A' && c <= L'F') ? (unsigned)(c - L'A' + 10)
                                                    : 16;
        if (digit > 15)
            return fallback;
        value = (value << 4) | digit;
    }
    return RGB((value >> 16) & 255, (value >> 8) & 255, value & 255);
}
static std::wstring ReadSetting(PCWSTR key);
static unsigned ReadCustomHex(PCWSTR key, unsigned fallback)
{
    COLORREF c = ParseThemeHex(ReadSetting(key),
                               RGB((fallback >> 16) & 255, (fallback >> 8) & 255, fallback & 255));
    return (unsigned)GetRValue(c) << 16 | (unsigned)GetGValue(c) << 8 | GetBValue(c);
}
// Empty or invalid overrides inherit the selected theme. All colors are RGB hex.
static bool TryThemeOverride(PCWSTR key, unsigned& rgb)
{
    const std::wstring value = ReadSetting(key);
    if (value.size() != 7 || value[0] != L'#')
        return false;
    for (size_t i = 1; i < value.size(); ++i)
        if (!((value[i] >= L'0' && value[i] <= L'9') ||
              (value[i] >= L'a' && value[i] <= L'f') ||
              (value[i] >= L'A' && value[i] <= L'F')))
            return false;
    COLORREF color = ParseThemeHex(value, RGB(0, 0, 0));
    rgb = (unsigned)GetRValue(color) << 16 |
          (unsigned)GetGValue(color) << 8 | GetBValue(color);
    return true;
}
static COLORREF OverridePopupColor(PCWSTR key, COLORREF base)
{
    unsigned rgb = 0;
    return TryThemeOverride(key, rgb) ? RGB((rgb >> 16) & 255,
                                           (rgb >> 8) & 255, rgb & 255) : base;
}
static bool WindowsLightMode()
{
    HKEY key = nullptr;
    if (RegOpenKeyExW(HKEY_CURRENT_USER,
                      L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", 0,
                      KEY_QUERY_VALUE, &key) != ERROR_SUCCESS)
        return false;
    DWORD value = 0, bytes = sizeof(value);
    LONG result = RegQueryValueExW(key, L"AppsUseLightTheme", nullptr, nullptr,
                                   reinterpret_cast<BYTE*>(&value), &bytes);
    RegCloseKey(key);
    return result == ERROR_SUCCESS && value != 0;
}
static PillPalette ResolvePillPaletteBase(const std::wstring& theme)
{
    if (theme == L"traditional")
        return {Rgb(0x123D34), Rgb(0xF5EAD3), Rgb(0xE4C784), Rgb(0xC9B789)};
    if (theme == L"catppuccin")
        return {Rgb(0x211E32), Rgb(0xCDD6F4), Rgb(0xCFA1FF), Rgb(0xA6ADC8)};
    if (theme == L"emberveil")
        return {Rgb(0x171B20), Rgb(0xF5E6C8), Rgb(0xFFBC5E), Rgb(0xD9A441)};
    if (theme == L"emberveil-midnight")
        return {Rgb(0x0A0C0E), Rgb(0xF5E6C8), Rgb(0xFFBC5E), Rgb(0xB8A98A)};
    if (theme == L"emberveil-daybreak")
        return {Rgb(0xFFF2D8), Rgb(0x1A1F26), Rgb(0xB87924), Rgb(0x8A642D)};
    if (theme == L"emerald-mosque")
        return {Rgb(0x102B27), Rgb(0xF3E8CF), Rgb(0xD4AF70), Rgb(0xB6C9BD)};
    if (theme == L"dracula")
        return {Rgb(0x282A36), Rgb(0xF8F8F2), Rgb(0xBD93F9), Rgb(0x6272A4)};
    if (theme == L"nord")
        return {Rgb(0x2E3440), Rgb(0xECEFF4), Rgb(0x88C0D0), Rgb(0x81A1C1)};
    if (theme == L"gruvbox")
        return {Rgb(0x282828), Rgb(0xEBDBB2), Rgb(0xFABD2F), Rgb(0x928374)};
    if (theme == L"tokyo-night")
        return {Rgb(0x1A1B26), Rgb(0xC0CAF5), Rgb(0x7AA2F7), Rgb(0x565F89)};
    if (theme == L"solarized-dark")
        return {Rgb(0x002B36), Rgb(0x93A1A1), Rgb(0xB58900), Rgb(0x586E75)};
    if (theme == L"solarized-light")
        return {Rgb(0xFDF6E3), Rgb(0x586E75), Rgb(0x268BD2), Rgb(0x93A1A1)};
    if (theme == L"custom")
        return {Rgb(ReadCustomHex(L"Advanced.CustomThemeSettings.PillBackground",
                                  0x211E32)),
                Rgb(ReadCustomHex(L"Advanced.CustomThemeSettings.PillText",
                                  0xF5E6C8)),
                Rgb(ReadCustomHex(L"Advanced.CustomThemeSettings.PillCountdown",
                                  0xFFBC5E)),
                Rgb(ReadCustomHex(L"Advanced.CustomThemeSettings.PillSeparator",
                                  0xB8A98A))};
    COLORREF c = GetSysColor(COLOR_HIGHLIGHT);
    auto accent = winrt::Windows::UI::Color{255, GetRValue(c), GetGValue(c), GetBValue(c)};
    // Windows/System Default: accent-colored pill with a contrasting foreground.
    // Preserve the previous transparent-pill treatment when no background is drawn.
    if (g_backgroundStyle == L"transparent" || g_backgroundOpacity == 0)
    {
        if (WindowsLightMode())
            return {Rgb(0xF2F2F2), Rgb(0x202020), accent, Rgb(0x666666)};
        return {Rgb(0x292929), Rgb(0xF4F4F4), accent, Rgb(0xB8B8B8)};
    }
    // The accent-filled Windows/System pill deliberately uses white text.
    // Users choosing a very light accent can adjust the text in custom overrides.
    return {accent, Rgb(0xFFFFFF), Rgb(0xFFFFFF), Rgb(0xFFFFFF)};
}
static PillPalette ResolvePillPalette(const std::wstring& theme)
{
    auto palette = ResolvePillPaletteBase(theme);
    unsigned rgb = 0;
    if (TryThemeOverride(L"Advanced.CustomThemeSettings.PillBackground", rgb))
        palette.background = Rgb(rgb);
    if (TryThemeOverride(L"Advanced.CustomThemeSettings.PillText", rgb))
        palette.foreground = Rgb(rgb);
    if (TryThemeOverride(L"Advanced.CustomThemeSettings.PillCountdown", rgb))
        palette.countdown = Rgb(rgb);
    if (TryThemeOverride(L"Advanced.CustomThemeSettings.PillSeparator", rgb))
        palette.separator = Rgb(rgb);
    return palette;
}
static bool g_showNext = true, g_enablePopup = true;
static std::atomic<int> g_popupAutoCloseSeconds{5};
static ULONGLONG g_popupAutoCloseDeadline = 0;
static bool g_openPopupOnHover = false;
static int g_hoverCloseDelayMs = 500;
// Accessed only from the taskbar UI thread (the popup is created on that thread).
static bool g_hoverPopupActive = false;
static ULONGLONG g_hoverOutsideSince = 0;
static RECT g_hoverAnchorRect{};
static std::wstring g_mosque = L"al-haram-makkah-saudi-arabia";
static int g_refreshHours = 24;
static HANDLE g_stopEvent = nullptr, g_refreshEvent = nullptr;
static HANDLE g_updateWorker = nullptr;
static HANDLE g_countdownWorker = nullptr;
static void UpdateClockOnUiThread();
[[clang::no_destroy]] static TextBlock g_prayerText{nullptr};
[[clang::no_destroy]] static TextBlock g_countdownText{nullptr};
[[clang::no_destroy]] static TextBlock g_iqamahText{nullptr};
[[clang::no_destroy]] static TextBlock g_iqamahShadow{nullptr};
[[clang::no_destroy]] static TextBlock g_prayerShadow{nullptr};
[[clang::no_destroy]] static TextBlock g_countdownShadow{nullptr};
[[clang::no_destroy]] static Grid g_pill{nullptr};
static HWND g_popup = nullptr;
static winrt::event_token g_tapToken{};
static bool g_tapRegistered = false;
static winrt::event_token g_hoverToken{};
static bool g_hoverRegistered = false;

// Popup is a small independent Win32 window owned by the taskbar UI thread.
// The native taskbar slot remains XAML; this preserves the original v0.6.6
// popup behavior without introducing another XAML layout lease.
static COLORREF PopupRgb(unsigned v)
{
    return RGB((v >> 16) & 255, (v >> 8) & 255, v & 255);
}
struct PopupColors
{
    COLORREF bg, border, text, muted, rule, footer;
};
static PopupColors PopupThemeBase()
{
    if (g_theme == L"custom")
        return {PopupRgb(ReadCustomHex(
                    L"Advanced.CustomThemeSettings.PopupBackground", 0x1D1D2D)),
                PopupRgb(ReadCustomHex(
                    L"Advanced.CustomThemeSettings.PopupBorder", 0x3B3B58)),
                PopupRgb(ReadCustomHex(L"Advanced.CustomThemeSettings.PopupText",
                                       0xD3D0E8)),
                PopupRgb(ReadCustomHex(
                    L"Advanced.CustomThemeSettings.PopupMuted", 0xB8B3D1)),
                PopupRgb(ReadCustomHex(L"Advanced.CustomThemeSettings.PopupRule",
                                       0x424158)),
                PopupRgb(ReadCustomHex(
                    L"Advanced.CustomThemeSettings.PopupFooter", 0xA29DBB))};
    if (g_theme == L"traditional")
        return {PopupRgb(0x102F29), PopupRgb(0x527361), PopupRgb(0xF5EAD3),
                PopupRgb(0xC9B789), PopupRgb(0x527361), PopupRgb(0xE4C784)};
    if (g_theme == L"catppuccin")
        return {PopupRgb(0x302D47), PopupRgb(0x58536F), PopupRgb(0xCDD6F4),
                PopupRgb(0xA6ADC8), PopupRgb(0x58536F), PopupRgb(0xB4BEFE)};
    if (g_theme == L"emberveil")
        return {PopupRgb(0x171B20), PopupRgb(0x212832), PopupRgb(0xF5E6C8),
                PopupRgb(0xF5E6C8), PopupRgb(0x212832), PopupRgb(0xD9A441)};
    if (g_theme == L"emberveil-midnight")
        return {PopupRgb(0x101418), PopupRgb(0x353A3E), PopupRgb(0xF5E6C8),
                PopupRgb(0xB8A98A), PopupRgb(0x353A3E), PopupRgb(0xFFBC5E)};
    if (g_theme == L"emberveil-daybreak")
        return {PopupRgb(0xF5E6C8), PopupRgb(0xE2CFA8), PopupRgb(0x1A1F26),
                PopupRgb(0x6F624F), PopupRgb(0xE2CFA8), PopupRgb(0xB87924)};
    if (g_theme == L"emerald-mosque")
        return {PopupRgb(0x102B27), PopupRgb(0x386057), PopupRgb(0xF3E8CF),
                PopupRgb(0xB6C9BD), PopupRgb(0x386057), PopupRgb(0xD4AF70)};
    if (g_theme == L"dracula")
        return {PopupRgb(0x282A36), PopupRgb(0x44475A), PopupRgb(0xF8F8F2),
                PopupRgb(0xBFBFD0), PopupRgb(0x44475A), PopupRgb(0xBD93F9)};
    if (g_theme == L"nord")
        return {PopupRgb(0x2E3440), PopupRgb(0x4C566A), PopupRgb(0xECEFF4),
                PopupRgb(0xD8DEE9), PopupRgb(0x4C566A), PopupRgb(0x88C0D0)};
    if (g_theme == L"gruvbox")
        return {PopupRgb(0x282828), PopupRgb(0x504945), PopupRgb(0xEBDBB2),
                PopupRgb(0xA89984), PopupRgb(0x504945), PopupRgb(0xFABD2F)};
    if (g_theme == L"tokyo-night")
        return {PopupRgb(0x1A1B26), PopupRgb(0x414868), PopupRgb(0xC0CAF5),
                PopupRgb(0xA9B1D6), PopupRgb(0x414868), PopupRgb(0x7AA2F7)};
    if (g_theme == L"solarized-dark")
        return {PopupRgb(0x002B36), PopupRgb(0x586E75), PopupRgb(0x93A1A1),
                PopupRgb(0x839496), PopupRgb(0x586E75), PopupRgb(0xB58900)};
    if (g_theme == L"solarized-light")
        return {PopupRgb(0xFDF6E3), PopupRgb(0xEEE8D5), PopupRgb(0x586E75),
                PopupRgb(0x657B83), PopupRgb(0x93A1A1), PopupRgb(0x268BD2)};
    if (WindowsLightMode())
        return {PopupRgb(0xFAFAFA), PopupRgb(0xD1D1D1), PopupRgb(0x202020),
                PopupRgb(0x666666), PopupRgb(0xD1D1D1), GetSysColor(COLOR_HIGHLIGHT)};
    return {PopupRgb(0x202020), PopupRgb(0x505050), PopupRgb(0xF4F4F4),
            PopupRgb(0xB8B8B8), PopupRgb(0x505050), GetSysColor(COLOR_HIGHLIGHT)};
}
static PopupColors PopupTheme()
{
    auto colors = PopupThemeBase();
    colors.bg = OverridePopupColor(L"Advanced.CustomThemeSettings.PopupBackground", colors.bg);
    colors.border = OverridePopupColor(L"Advanced.CustomThemeSettings.PopupBorder", colors.border);
    colors.text = OverridePopupColor(L"Advanced.CustomThemeSettings.PopupText", colors.text);
    colors.muted = OverridePopupColor(L"Advanced.CustomThemeSettings.PopupMuted", colors.muted);
    colors.rule = OverridePopupColor(L"Advanced.CustomThemeSettings.PopupRule", colors.rule);
    colors.footer = OverridePopupColor(L"Advanced.CustomThemeSettings.PopupFooter", colors.footer);
    return colors;
}
// The popup uses layered alpha only for translucent Solid. Transparent
// uses its own lightly tinted acrylic backdrop; the XAML pill is untouched.
static bool PopupUsesLayeredAlpha()
{
    return PopupMaterial() == L"solid" && PopupOpacity() < 100;
}

static void ConfigurePopupAlpha(HWND hwnd)
{
    const bool layered = PopupUsesLayeredAlpha();
    LONG_PTR style = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    const LONG_PTR wanted = layered ? (style | WS_EX_LAYERED) : (style & ~WS_EX_LAYERED);
    if (style != wanted)
    {
        SetWindowLongPtrW(hwnd, GWL_EXSTYLE, wanted);
        SetWindowPos(hwnd, nullptr, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE |
                         SWP_FRAMECHANGED);
    }
}

// Popup backdrop uses the dynamically resolved Windows accent API.
// Keep the GDI foreground rendering path independent of composition support.
struct MawaqitAccentPolicy
{
    int state;
    int flags;
    DWORD gradientColor;
    int animationId;
};
struct MawaqitWindowCompositionAttributeData
{
    int attribute;
    PVOID data;
    SIZE_T size;
};
static void ConfigurePopupComposition(HWND hwnd)
{
    if (!hwnd) return;
    using SetComposition = BOOL (WINAPI*)(HWND, MawaqitWindowCompositionAttributeData*);
    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    auto setComposition = user32 ? reinterpret_cast<SetComposition>(
        GetProcAddress(user32, "SetWindowCompositionAttribute")) : nullptr;
    if (!setComposition)
    {
        Wh_Log(L"[Mawaqit][PopupBackdrop] Composition API unavailable; using GDI popup rendering");
        return;
    }
    // ACCENT_DISABLED=0, BLURBEHIND=3, ACRYLICBLURBEHIND=4.
    // Gradient is AABBGGRR (alpha is the high byte).
    const bool blur = PopupMaterial() == L"blur";
    const bool acrylic = PopupMaterial() == L"acrylic";
    const bool transparent = PopupMaterial() == L"transparent";
    MawaqitAccentPolicy policy{};
    policy.state = blur ? 3 : (acrylic || transparent) ? 4 : 0;
    // ACCENT_POLICY gradientColor is AABBGGRR. Use a neutral dark surface
    // instead of the purple fallback tint, matching Windows 11 dark flyouts.
    // Acrylic gets a stronger tint for legibility; Blur stays more translucent.
    // These are intentionally popup-only: XAML pill brushes are unchanged.
    policy.gradientColor = acrylic ? 0xCC272020 :
                           transparent ? 0x50272020 :
                           blur ? 0x60272020 : 0;
    unsigned tintRgb = 0;
    if (policy.state != 0 &&
        TryThemeOverride(L"Advanced.CustomThemeSettings.PopupBackground", tintRgb))
    {
        // ACCENT_POLICY expects AABBGGRR, not AARRGGBB. Preserve each
        // material's existing alpha while overriding only the tint RGB.
        policy.gradientColor = (policy.gradientColor & 0xFF000000u) |
                               ((tintRgb & 0xFFu) << 16) |
                               (tintRgb & 0x00FF00u) |
                               ((tintRgb >> 16) & 0xFFu);
    }
    // The high byte controls tint strength; preserve the original appearance
    // whenever opacity inherits the pill's setting.
    if (policy.state != 0 && g_popupBackgroundOpacity >= 0)
    {
        const DWORD baseAlpha = (policy.gradientColor >> 24) & 0xFFu;
        const DWORD alpha = (baseAlpha * static_cast<DWORD>(PopupOpacity()) + 50u) / 100u;
        policy.gradientColor = (policy.gradientColor & 0x00FFFFFFu) | (alpha << 24);
    }
    MawaqitWindowCompositionAttributeData data{19, &policy, sizeof(policy)};
    SetLastError(0);
    const BOOL ok = setComposition(hwnd, &data);
    if (!ok)
        Wh_Log(L"[Mawaqit][PopupBackdrop] Composition rejected style=%ls accent=%d "
               L"error=%lu; retaining GDI popup rendering",
               PopupMaterial().c_str(), policy.state, GetLastError());
    else
        Wh_Log(L"[Mawaqit][PopupBackdrop] style=%ls accent=%d "
               L"gradient=0x%08lX; GDI foreground retained",
               PopupMaterial().c_str(), policy.state,
               static_cast<unsigned long>(policy.gradientColor));
}

// DWM accent backdrops use Windows' standard rounded-corner geometry.
// Keep Transparent, Blur and Acrylic popup geometry at 8 DIP for
// consistent corners. Solid retains the configurable radius.
// Transparent now uses the popup-only accent composition path.
static int PopupRadiusDip()
{
    return (PopupMaterial() == L"transparent" ||
            PopupMaterial() == L"blur" ||
            PopupMaterial() == L"acrylic")
               ? 8 : std::max(0, g_cornerRadius);
}

static void ConfigurePopupDwmCorners(HWND hwnd)
{
    if (!hwnd || (PopupMaterial() != L"blur" &&
                  PopupMaterial() != L"acrylic" &&
                  PopupMaterial() != L"transparent"))
        return;
    HMODULE dwm = LoadLibraryW(L"dwmapi.dll");
    if (!dwm)
        return;
    using DwmSetAttribute = HRESULT (WINAPI*)(HWND, DWORD, LPCVOID, DWORD);
    auto setAttribute = reinterpret_cast<DwmSetAttribute>(
        GetProcAddress(dwm, "DwmSetWindowAttribute"));
    if (setAttribute)
    {
        constexpr DWORD kCornerPreferenceAttribute = 33;
        constexpr DWORD kRound = 2; // DWMWCP_ROUND, Windows-defined radius.
        const HRESULT hr = setAttribute(hwnd, kCornerPreferenceAttribute,
                                        &kRound, sizeof(kRound));
        if (FAILED(hr))
            Wh_Log(L"[Mawaqit][Popup] DWM rounding failed: 0x%08lX",
                   static_cast<unsigned long>(hr));
    }
    FreeLibrary(dwm);
}

static int PopupCornerRadiusPx(int width, int height, UINT dpi)
{
    const int scaled = MulDiv(PopupRadiusDip(), static_cast<int>(dpi ? dpi : 96), 96);
    return std::clamp(scaled, 0, std::min(width, height) / 2);
}

// Measure the mosque name with the same GDI font and width used for painting.
// Bound extreme names to five lines so a malformed title cannot consume the screen.
static int PopupMosqueNameHeight()
{
    HDC dc = GetDC(nullptr);
    if (!dc)
        return 20;
    HFONT font = CreateFontW(-15, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                             DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                             CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
    HGDIOBJ oldFont = font ? SelectObject(dc, font) : nullptr;
    TEXTMETRICW metrics{};
    GetTextMetricsW(dc, &metrics);
    const int lineHeight = std::max(1, static_cast<int>(metrics.tmHeight + metrics.tmExternalLeading));
    RECT bounds{0, 0, 242, 0};
    DrawTextW(dc, g_mosqueName.c_str(), -1, &bounds,
              DT_CALCRECT | DT_WORDBREAK | DT_NOPREFIX);
    if (font)
    {
        SelectObject(dc, oldFont);
        DeleteObject(font);
    }
    ReleaseDC(nullptr, dc);
    return std::clamp(static_cast<int>(bounds.bottom - bounds.top), lineHeight, lineHeight * 5);
}

struct PopupGeometry
{
    int nameHeight;
    int topRule;
    int firstRow;
    int bottomRule;
    int height;
};

static PopupGeometry GetPopupGeometry()
{
    constexpr int nameTop = 12;
    constexpr int headerGap = 18;
    constexpr int rowGap = 18;
    constexpr int rowHeight = 22;
    constexpr int scheduleBottomGap = 18;
    constexpr int footerHeight = 42;
    PopupGeometry layout{};
    layout.nameHeight = PopupMosqueNameHeight();
    // GDI's measured rectangle includes extra font leading for wrapped lines.
    // Compensate optically without changing the single-line header spacing.
    // Keep a minimum gap for long names (up to five lines).
    const int estimatedLines = std::clamp((layout.nameHeight + 17) / 18, 1, 5);
    const int opticalHeaderGap = std::max(8, headerGap - 4 * (estimatedLines - 1));
    const bool hijri = !g_hijriDate.empty();
    layout.topRule = nameTop + layout.nameHeight + (hijri ? 22 : 0) + opticalHeaderGap;
    layout.firstRow = layout.topRule + rowGap;
    const int optionalRows = g_showFajrWindowCountdown ?
        1 + (g_imsakMinute >= 0 ? 1 : 0) : 0;
    layout.bottomRule = layout.firstRow + (5 + optionalRows) * rowHeight + scheduleBottomGap;
    layout.height = layout.bottomRule + footerHeight;
    return layout;
}

static int PopupLogicalHeight()
{
    return GetPopupGeometry().height;
}

static void PaintPrayerPopup(HWND hwnd)
{
    PAINTSTRUCT ps{};
    HDC dc = BeginPaint(hwnd, &ps);
    if (!dc)
        return;
    // Render the popup into an off-screen GDI bitmap first.
    // Opaque styles use the original GDI rendering path; translucent Solid
    // uses a 32-bit DIB to supply per-pixel alpha to UpdateLayeredWindow.
    RECT pixelClient{};
    GetClientRect(hwnd, &pixelClient);
    const int pixelWidth = pixelClient.right - pixelClient.left;
    const int pixelHeight = pixelClient.bottom - pixelClient.top;
    if (pixelWidth <= 0 || pixelHeight <= 0)
    {
        EndPaint(hwnd, &ps);
        return;
    }
    const bool layered = PopupUsesLayeredAlpha();
    HDC bufferDc = CreateCompatibleDC(dc);
    HBITMAP bitmap = nullptr;
    void* dibPixels = nullptr;
    if (bufferDc && layered)
    {
        BITMAPINFO bi{};
        bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bi.bmiHeader.biWidth = pixelWidth;
        bi.bmiHeader.biHeight = -pixelHeight; // top-down BGRA
        bi.bmiHeader.biPlanes = 1;
        bi.bmiHeader.biBitCount = 32;
        bi.bmiHeader.biCompression = BI_RGB;
        bitmap = CreateDIBSection(bufferDc, &bi, DIB_RGB_COLORS, &dibPixels, nullptr, 0);
    }
    else if (bufferDc)
        bitmap = CreateCompatibleBitmap(dc, pixelWidth, pixelHeight);
    if (!bufferDc || !bitmap)
    {
        if (bitmap) DeleteObject(bitmap);
        if (bufferDc) DeleteDC(bufferDc);
        Wh_Log(L"[Mawaqit][PopupAlpha] buffer allocation failed; using direct GDI");
        bufferDc = nullptr;
        bitmap = nullptr;
    }
    HGDIOBJ previousBitmap = bitmap ? SelectObject(bufferDc, bitmap) : nullptr;
    HDC renderDc = bufferDc ? bufferDc : dc;
    // Draw in a fixed 270x252 logical coordinate space. The GDI viewport
    // scales the entire popup, including text, consistently with its DPI size.
    RECT client{};
    GetClientRect(hwnd, &client);
    int savedDc = SaveDC(renderDc);
    SetMapMode(renderDc, MM_ANISOTROPIC);
    SetWindowExtEx(renderDc, 270, PopupLogicalHeight(), nullptr);
    SetViewportExtEx(renderDc, client.right - client.left, client.bottom - client.top, nullptr);
    RECT r{0, 0, 270, PopupLogicalHeight()};
    auto colors = PopupTheme();
    HBRUSH bg = CreateSolidBrush(colors.bg);
    FillRect(renderDc, &r, bg);
    DeleteObject(bg);
    // Draw the outline just INSIDE the clipped popup region. Drawing
    // on the outermost pixel row/column caused GDI's non-antialiased stroke
    // to be cut by SetWindowRgn, leaving uneven/square corner fragments.
    // Keep the geometry in the same 270x252 logical viewport as the content.
    constexpr int borderInset = 1;
    const int logicalRadius = std::clamp(PopupRadiusDip(), 0,
                                         static_cast<int>(std::min(r.right, r.bottom) / 2));
    const int innerRadius = std::max(0, logicalRadius - borderInset);
    HPEN border = g_popupShowBorder ? CreatePen(PS_SOLID, 1, colors.border) : nullptr;
    if (border)
    {
        HGDIOBJ prevPen = SelectObject(renderDc, border);
        HGDIOBJ oldBrush = SelectObject(renderDc, GetStockObject(NULL_BRUSH));
        const int left = borderInset;
        const int top = borderInset;
        const int right = r.right - borderInset;
        const int bottom = r.bottom - borderInset;
        if (innerRadius == 0)
            Rectangle(renderDc, left, top, right, bottom);
        else
            RoundRect(renderDc, left, top, right, bottom,
                      innerRadius * 2, innerRadius * 2);
        SelectObject(renderDc, oldBrush);
        SelectObject(renderDc, prevPen);
        DeleteObject(border);
    }
    SetBkMode(renderDc, TRANSPARENT);
    HFONT font = CreateFontW(-15, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                             OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                             DEFAULT_PITCH, L"Segoe UI");
    HGDIOBJ oldFont = SelectObject(renderDc, font);
    auto line = [&](const std::wstring& value, int left, int top, int right, int height,
                    COLORREF color, UINT flags)
    {
        RECT rect{left, top, right, top + height};
        SetTextColor(renderDc, color);
        DrawTextW(renderDc, value.c_str(), -1, &rect,
                  flags | DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX | DT_END_ELLIPSIS);
    };
    const PopupGeometry layout = GetPopupGeometry();
    RECT nameRect{14, 12, r.right - 14, 12 + layout.nameHeight};
    SetTextColor(renderDc, colors.text);
    DrawTextW(renderDc, g_mosqueName.c_str(), -1, &nameRect,
              DT_CENTER | DT_WORDBREAK | DT_NOPREFIX | DT_WORD_ELLIPSIS);
    bool hijri = !g_hijriDate.empty();
    if (hijri)
        line(g_hijriDate, 14, nameRect.bottom + 4, r.right - 14, 18, colors.muted, DT_CENTER);
    const int topRule = layout.topRule, bottomRule = layout.bottomRule;
    if (g_popupShowSeparators)
    {
        HPEN rule = CreatePen(PS_SOLID, 1, colors.rule);
        if (rule)
        {
            HGDIOBJ prevRulePen = SelectObject(renderDc, rule);
            MoveToEx(renderDc, 12, topRule, nullptr);
            LineTo(renderDc, r.right - 12, topRule);
            MoveToEx(renderDc, 12, bottomRule, nullptr);
            LineTo(renderDc, r.right - 12, bottomRule);
            SelectObject(renderDc, prevRulePen);
            DeleteObject(rule);
        }
    }
    // Keep informational Imsak/Shuruq times in the same Adhan column as
    // prayers whenever an Iqamah suffix column is present.
    bool hasIqamahColumn = g_showIqamah;
    if (hasIqamahColumn)
    {
        hasIqamahColumn = false;
        for (const auto& prayer : g_today)
        {
            if (!IqamahSuffix(prayer).empty())
            {
                hasIqamahColumn = true;
                break;
            }
        }
    }
    const int adhanColumnRight = hasIqamahColumn ? r.right - 66 : r.right - 20;
    const bool showImsak = g_showFajrWindowCountdown && g_imsakMinute >= 0;
    const int rowCount = 5 + (g_showFajrWindowCountdown ? 1 : 0) + (showImsak ? 1 : 0);
    const int firstRow = layout.firstRow;
    for (int row = 0; row < rowCount; row++)
    {
        if (showImsak && row == 0)
        {
            wchar_t time[16];
            swprintf(time, 16, L"%02d:%02d", g_imsakMinute / 60, g_imsakMinute % 60);
            const int y = firstRow + row * 22;
            line(L"Imsak", 20, y, r.right - 20, 21, colors.text, DT_LEFT);
            line(time, 20, y, adhanColumnRight, 21, colors.text, DT_RIGHT);
            continue;
        }
        const int prayerRow = row - (showImsak ? 1 : 0);
        if (g_showFajrWindowCountdown && prayerRow == 1)
        {
            std::wstring sunrise = L"--:--";
            if (g_shuruqMinute >= 0)
            {
                wchar_t b[16];
                swprintf(b, 16, L"%02d:%02d", g_shuruqMinute / 60, g_shuruqMinute % 60);
                sunrise = b;
            }
            int y = firstRow + row * 22;
            line(L"Shuruq", 20, y, r.right - 20, 21, colors.text, DT_LEFT);
            // Align Shuruq with the Adhan-time column; keep Iqamah column blank.
            line(sunrise, 20, y, adhanColumnRight, 21, colors.text, DT_RIGHT);
            continue;
        }
        int i = g_showFajrWindowCountdown && prayerRow > 1 ? prayerRow - 1 : prayerRow;
        std::wstring time = L"--:--";
        if (i < (int)g_today.size())
        {
            wchar_t b[16];
            swprintf(b, 16, L"%02d:%02d", g_today[i].minute / 60, g_today[i].minute % 60);
            time = b;
        }
        int y = firstRow + row * 22;
        line(kNames[i], 20, y, r.right - 20, 23, colors.text, DT_LEFT);
        if (i < (int)g_today.size() && !IqamahSuffix(g_today[i]).empty())
        {
            line(time, 20, y, r.right - 66, 23, colors.text, DT_RIGHT);
            line(IqamahSuffix(g_today[i]), r.right - 65, y, r.right - 20, 23,
                 colors.muted, DT_RIGHT);
        }
        else
            line(time, 20, y, r.right - 20, 23, colors.text, DT_RIGHT);
    }
    std::wstring footerCountdown = g_countdown;
    if (footerCountdown != L"--:--" && !footerCountdown.empty() && footerCountdown.front() == L'-')
        footerCountdown.erase(0, 1);

    line(g_label + L" in " + footerCountdown, 12, bottomRule + 8, r.right - 12, 24,
        colors.footer, DT_CENTER);
    SelectObject(renderDc, oldFont);
    DeleteObject(font);
    RestoreDC(renderDc, savedDc);
    if (bufferDc)
    {
        if (layered && dibPixels)
        {
            // GDI writes RGB but not useful alpha. Reconstruct approximate
            // foreground coverage from the rendered pixel versus theme BG.
            // Preserve contrasting foreground pixels while fading the background.
            // Matching foreground/background RGB cannot be distinguished here.
            const COLORREF bgColor = colors.bg;
            const int bgR = GetRValue(bgColor), bgG = GetGValue(bgColor);
            const int bgB = GetBValue(bgColor);
            // Layered alpha is used only by translucent Solid popups.
            const int bgAlpha =
                std::max(0, std::min(255, (PopupOpacity() * 255 + 50) / 100));
            auto* pixels = static_cast<unsigned char*>(dibPixels);
            for (int i = 0; i < pixelWidth * pixelHeight; ++i)
            {
                unsigned char* p = pixels + i * 4; // BGRA
                const int dB = std::abs((int)p[0] - bgB);
                const int dG = std::abs((int)p[1] - bgG);
                const int dR = std::abs((int)p[2] - bgR);
                // Differences are a conservative estimate of foreground
                // coverage. The exact text pixels stay fully opaque.
                const int difference = std::max(dR, std::max(dG, dB));
                const int coverage = difference == 0 ? 0 :
                    std::min(255, difference * 4);
                const int alpha = bgAlpha + ((255 - bgAlpha) * coverage + 127) / 255;
                // Convert the original GDI RGB into premultiplied BGRA.
                // This preserves solid glyph pixels and softens antialias edges.
                p[0] = (unsigned char)((p[0] * alpha + 127) / 255);
                p[1] = (unsigned char)((p[1] * alpha + 127) / 255);
                p[2] = (unsigned char)((p[2] * alpha + 127) / 255);
                p[3] = (unsigned char)alpha;
            }
            RECT wr{};
            GetWindowRect(hwnd, &wr);
            POINT dst{wr.left, wr.top}, src{0, 0};
            SIZE size{pixelWidth, pixelHeight};
            BLENDFUNCTION blend{AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
            if (!UpdateLayeredWindow(hwnd, nullptr, &dst, &size, bufferDc, &src,
                                     0, &blend, ULW_ALPHA))
                Wh_Log(L"[Mawaqit][PopupAlpha] UpdateLayeredWindow failed: %lu",
                       GetLastError());
        }
        else
            BitBlt(dc, 0, 0, pixelWidth, pixelHeight, bufferDc, 0, 0, SRCCOPY);
        SelectObject(bufferDc, previousBitmap);
        DeleteObject(bitmap);
        DeleteDC(bufferDc);
    }
    EndPaint(hwnd, &ps);
}
// The popup is an activatable flyout: Windows notifies it when focus leaves.
// No session-wide mouse hook or cross-application Escape polling is needed.
// All animation state belongs to the taskbar/popup UI thread.
static constexpr UINT_PTR kPopupAnimationTimer = 2;
static bool g_popupAnimating = false;
static bool g_popupClosing = false;
static bool g_popupMotionEnabled = true;
static ULONGLONG g_popupAnimationStarted = 0;
static DWORD g_popupAnimationDuration = 200;
static int g_popupStartY = 0;
static int g_popupTargetY = 0;
static int g_popupTargetX = 0;
static int g_popupWidth = 0;
static int g_popupHeight = 0;
static int g_popupSlideDistance = 0;
static ULONGLONG g_popupLastDeactivated = 0;

static bool PopupMotionAllowed()
{
    BOOL enabled = TRUE;
    return SystemParametersInfoW(SPI_GETCLIENTAREAANIMATION, 0, &enabled, 0) && enabled;
}

static void StopPopupAnimation()
{
    if (g_popup && IsWindow(g_popup))
        KillTimer(g_popup, kPopupAnimationTimer);
    g_popupAnimating = false;
    g_popupClosing = false;
}

static void FinishPopupHide()
{
    StopPopupAnimation();
    if (g_popup && IsWindow(g_popup))
    {
        KillTimer(g_popup, 1);
        ShowWindow(g_popup, SW_HIDE);
    }
}

static void BeginPopupCloseAnimation()
{
    if (!g_popup || !IsWindowVisible(g_popup) || g_popupClosing)
        return;
    g_popupAutoCloseDeadline = 0;
    g_hoverPopupActive = false;
    g_hoverOutsideSince = 0;
    KillTimer(g_popup, 1);
    if (!g_popupMotionEnabled)
    {
        FinishPopupHide();
        return;
    }
    RECT current{};
    if (!GetWindowRect(g_popup, &current))
    {
        FinishPopupHide();
        return;
    }
    g_popupStartY = current.top;
    g_popupTargetY = current.top + g_popupSlideDistance;
    g_popupAnimationStarted = GetTickCount64();
    g_popupAnimationDuration = 140;
    g_popupAnimating = true;
    g_popupClosing = true;
    if (!SetTimer(g_popup, kPopupAnimationTimer, 15, nullptr))
    {
        Wh_Log(L"[Mawaqit] Popup close animation timer failed: %lu; hiding immediately",
               GetLastError());
        FinishPopupHide();
    }
}

static void HidePrayerPopup(bool deactivated = false)
{
    g_popupAutoCloseDeadline = 0;
    g_hoverPopupActive = false;
    g_hoverOutsideSince = 0;
    // Focus loss must hide immediately so the flyout never floats above a new app.
    if (deactivated)
    {
        FinishPopupHide();
        g_popupLastDeactivated = GetTickCount64();
    }
    else
        BeginPopupCloseAnimation();
}
static LRESULT CALLBACK PrayerPopupProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
    switch (msg)
    {
    case WM_ACTIVATE:
        if (LOWORD(wp) == WA_INACTIVE && IsWindowVisible(hwnd))
            HidePrayerPopup(true);
        return 0;
    case WM_KEYDOWN:
        if (wp == VK_ESCAPE)
        {
            HidePrayerPopup();
            return 0;
        }
        break;
    case WM_PAINT:
        PaintPrayerPopup(hwnd);
        return 0;
    case WM_ERASEBKGND:
        return 1;
    case WM_CLOSE:
        HidePrayerPopup();
        return 0;
    case WM_TIMER:
    {
        if (wp == kPopupAnimationTimer && g_popupAnimating)
        {
            ULONGLONG elapsed = GetTickCount64() - g_popupAnimationStarted;
            double t = std::min(1.0, static_cast<double>(elapsed) / g_popupAnimationDuration);
            // Cubic ease-out on entry; cubic ease-in on exit.
            double eased = g_popupClosing ? t * t * t : 1.0 - (1.0 - t) * (1.0 - t) * (1.0 - t);
            int y = g_popupStartY +
                    static_cast<int>(std::lround((g_popupTargetY - g_popupStartY) * eased));
            SetWindowPos(hwnd, nullptr, g_popupTargetX, y, 0, 0,
                         SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
            if (t >= 1.0)
            {
                bool closing = g_popupClosing;
                StopPopupAnimation();
                if (closing)
                    FinishPopupHide();
            }
            return 0;
        }
        if (wp == 1)
        {
            if (g_hoverPopupActive && !g_popupAnimating && IsWindowVisible(hwnd))
            {
                POINT cursor{};
                RECT popupRect{};
                if (GetCursorPos(&cursor) && GetWindowRect(hwnd, &popupRect))
                {
                    bool inside =
                        PtInRect(&g_hoverAnchorRect, cursor) || PtInRect(&popupRect, cursor);
                    if (inside)
                        g_hoverOutsideSince = 0;
                    else
                    {
                        ULONGLONG now = GetTickCount64();
                        if (!g_hoverOutsideSince)
                            g_hoverOutsideSince = now;
                        if (now - g_hoverOutsideSince >=
                            static_cast<ULONGLONG>(g_hoverCloseDelayMs))
                        {
                            HidePrayerPopup();
                            return 0;
                        }
                    }
                }
            }
            if (g_popupAutoCloseDeadline && GetTickCount64() >= g_popupAutoCloseDeadline)
            {
                Wh_Log(L"[Mawaqit] Popup auto-close timeout reached");
                HidePrayerPopup();
                return 0;
            }
            InvalidateRect(hwnd, nullptr, FALSE);
        }
        return 0;
    }
    case WM_DESTROY:
        StopPopupAnimation();
        KillTimer(hwnd, 1);
        if (g_popup == hwnd)
            g_popup = nullptr;
        return 0;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}
static wchar_t g_popupClassName[128]{};
static HINSTANCE g_popupClassInstance = nullptr;
static bool g_popupClassRegistered = false;
static void DestroyPrayerPopup()
{
    if (g_popup)
    {
        // A rebuild/unload is a forced close, never an animated transition.
        FinishPopupHide();
        g_popupAutoCloseDeadline = 0;
        g_hoverPopupActive = false;
        g_hoverOutsideSince = 0;
        DestroyWindow(g_popup);
        g_popup = nullptr;
    }
    if (g_popupClassRegistered)
    {
        if (!UnregisterClassW(g_popupClassName, g_popupClassInstance))
            Wh_Log(L"[Mawaqit] Popup class unregister failed: %lu", GetLastError());
        else
            g_popupClassRegistered = false;
    }
}
[[maybe_unused]] static void TogglePrayerPopup(Grid const& anchorPill, HWND anchorTaskbar,
                                               bool hover = false)
{

    if (!g_enablePopup)
    {
        Wh_Log(L"[Mawaqit] Popup click ignored: disabled by settings");
        return;
    }
    // Avoid reopening immediately after losing activation.
    if (g_popupLastDeactivated && GetTickCount64() - g_popupLastDeactivated < 300)
        return;
    if (g_popup && IsWindowVisible(g_popup))
    {
        if (!hover)
            HidePrayerPopup();
        return;
    }
    if (hover && !g_openPopupOnHover)
        return;
    if (!g_popup)
    {

        // Register a unique class per loaded mod instance. A previous Windhawk
        // hot-reload can leave a process-wide class pointing at unloaded code.
        if (!g_popupClassName[0])
        {
            LARGE_INTEGER stamp{};
            QueryPerformanceCounter(&stamp);
            swprintf_s(g_popupClassName, L"MawaqitPopup_%lu_%llX_%p", GetCurrentProcessId(),
                       (unsigned long long)stamp.QuadPart,
                       reinterpret_cast<void*>(&PrayerPopupProc));
        }
        if (!g_popupClassInstance)
        {
            HMODULE module = nullptr;
            if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                                        GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                                    reinterpret_cast<LPCWSTR>(&PrayerPopupProc), &module))
            {
                Wh_Log(L"[Mawaqit] Popup module lookup failed: %lu", GetLastError());
                return;
            }
            g_popupClassInstance = module;
        }
        WNDCLASSEXW wc{};
        wc.cbSize = sizeof(wc);
        wc.lpfnWndProc = PrayerPopupProc;
        wc.hInstance = g_popupClassInstance;
        wc.lpszClassName = g_popupClassName;
        wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
        if (!g_popupClassRegistered)
        {
            ATOM atom = RegisterClassExW(&wc);
            Wh_Log(L"[Mawaqit] class-register result=%u error=%lu", (unsigned)atom,
                   atom ? 0 : GetLastError());
            if (!atom)
                return;
            g_popupClassRegistered = true;
        }

        g_popup = CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_TOPMOST,
                                  g_popupClassName,
                                  L"Mawaqit Prayer Times", WS_POPUP, 0, 0, 270, 252, nullptr,
                                  nullptr, wc.hInstance, nullptr);

        if (!g_popup)
        {
            Wh_Log(L"[Mawaqit] Popup creation failed: %lu", GetLastError());
            return;
        }
    }

    LoadMetadata();

    UpdateCountdown();

    HWND taskbar = anchorTaskbar && IsWindow(anchorTaskbar)
                       ? anchorTaskbar
                       : taskwin::FindCurrentProcessTaskbarWnd();
    RECT bar{};
    if (taskbar)
        GetWindowRect(taskbar, &bar);
    RECT monitor{};
    MONITORINFO mi{};
    mi.cbSize = sizeof(mi);
    HMONITOR mon = MonitorFromWindow(taskbar ? taskbar : g_popup, MONITOR_DEFAULTTONEAREST);
    if (GetMonitorInfoW(mon, &mi))
        monitor = mi.rcMonitor;
    // Win32 window rectangles are physical pixels; XAML coordinates are DIPs.
    UINT dpi = taskbar ? GetDpiForWindow(taskbar) : 96;
    if (!dpi)
        dpi = 96;
    const double scale = static_cast<double>(dpi) / 96.0;
    const int w = MulDiv(270, dpi, 96), h = MulDiv(PopupLogicalHeight(), dpi, 96);
    const int edgeGap = MulDiv(11, dpi, 96);
    const int screenMargin = MulDiv(4, dpi, 96);
    bool top = bar.top <= monitor.top + 10;
    int x = bar.left + (bar.right - bar.left - w) / 2;
    // Center below the XAML pill if possible, otherwise use taskbar center.
    if (anchorPill)
    {
        try
        {
            auto transform = anchorPill.TransformToVisual(nullptr);
            auto pt = transform.TransformPoint({0, 0});
            x = bar.left + (int)std::lround((pt.X + anchorPill.ActualWidth() / 2.0) * scale) -
                w / 2;
        }
        catch (...)
        {
        }
    }
    int y = top ? bar.bottom + edgeGap : bar.top - h - edgeGap;
    x = std::max<int>(monitor.left + screenMargin,
                      std::min<int>(x, monitor.right - w - screenMargin));
    y = std::max<int>(monitor.top + screenMargin,
                      std::min<int>(y, monitor.bottom - h - screenMargin));
    // Apply the final popup geometry before configuring its composition surface
    // so accent backdrops respect the rounded window region. Solid retains
    // its layered-alpha path; Transparent uses a lighter acrylic tint.
    ConfigurePopupAlpha(g_popup);

    // Animate the whole native window, leaving its paint/layout and themes unchanged.
    StopPopupAnimation();
    g_popupMotionEnabled = PopupMotionAllowed();
    g_popupTargetX = x;
    g_popupTargetY = y;
    g_popupWidth = w;
    g_popupHeight = h;
    g_popupSlideDistance = top ? -MulDiv(22, dpi, 96) : MulDiv(22, dpi, 96);
    g_popupStartY = g_popupMotionEnabled ? y + g_popupSlideDistance : y;
    // Size first: a region installed on an old-sized HWND can disagree with
    // the composition surface when the window is resized or shown again.
    SetWindowPos(g_popup, HWND_TOPMOST, x, g_popupStartY, w, h,
                 SWP_SHOWWINDOW | (hover ? SWP_NOACTIVATE : 0));
    const int popupRadius = PopupCornerRadiusPx(w, h, dpi);
    HRGN rounded = popupRadius == 0
        ? CreateRectRgn(0, 0, w, h)
        : CreateRoundRectRgn(0, 0, w, h, popupRadius * 2, popupRadius * 2);
    if (rounded)
    {
        if (!SetWindowRgn(g_popup, rounded, TRUE))
        {
            Wh_Log(L"[Mawaqit][PopupCorners] SetWindowRgn failed: %lu", GetLastError());
            DeleteObject(rounded);
        }
        else
            Wh_Log(L"[Mawaqit][Popup] style=%ls radius=%d DIP (%d px), dpi=%u",
                   PopupMaterial().c_str(), PopupRadiusDip(), popupRadius, dpi);
    }
    ConfigurePopupDwmCorners(g_popup);
    ConfigurePopupComposition(g_popup);
    InvalidateRect(g_popup, nullptr, FALSE);
    UpdateWindow(g_popup);
    if (g_popupMotionEnabled)
    {
        g_popupAnimating = true;
        g_popupClosing = false;
        g_popupAnimationStarted = GetTickCount64();
        g_popupAnimationDuration = 200;
        if (!SetTimer(g_popup, kPopupAnimationTimer, 15, nullptr))
        {
            Wh_Log(L"[Mawaqit] Popup open animation timer failed: %lu; showing at final position",
                   GetLastError());
            StopPopupAnimation();
            SetWindowPos(g_popup, nullptr, g_popupTargetX, g_popupTargetY, 0, 0,
                         SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
        }
    }
    if (!hover)
    {
        SetForegroundWindow(g_popup);
        SetFocus(g_popup);
    }
    g_hoverPopupActive = hover;
    g_hoverOutsideSince = 0;
    if (hover)
    {
        // Translate the XAML pill bounds into physical screen coordinates.
        // The same monitor-aware coordinate space is used for popup placement.
        try
        {
            auto transform = anchorPill.TransformToVisual(nullptr);
            auto pt = transform.TransformPoint({0, 0});
            g_hoverAnchorRect = {
                bar.left + static_cast<LONG>(std::lround(pt.X * scale)),
                bar.top + static_cast<LONG>(std::lround(pt.Y * scale)),
                bar.left +
                    static_cast<LONG>(std::lround((pt.X + anchorPill.ActualWidth()) * scale)),
                bar.top +
                    static_cast<LONG>(std::lround((pt.Y + anchorPill.ActualHeight()) * scale))};
        }
        catch (...)
        {
            g_hoverAnchorRect = bar;
        }
    }
    SetTimer(g_popup, 1, 50, nullptr);
    // Arm only after the popup becomes visible; a subsequent opening re-arms.
    int autoClose = g_popupAutoCloseSeconds.load();
    g_popupAutoCloseDeadline =
        autoClose > 0 ? GetTickCount64() + static_cast<ULONGLONG>(autoClose) * 1000 : 0;
    Wh_Log(L"[Mawaqit][Monitor] Popup opened: taskbar=%p anchor=%s x=%d y=%d auto-close=%d seconds",
           taskbar, anchorTaskbar ? L"secondary" : L"primary", x, y, autoClose);

    InvalidateRect(g_popup, nullptr, FALSE);
}

static constexpr wchar_t kSlotName[] = L"MawaqitTaskbarSlot";
static constexpr wchar_t kLeaseName[] = L"MawaqitTaskbarLease";

static FrameworkElement FindNamed(FrameworkElement const& root, wchar_t const* name, int depth = 0)
{
    if (!root || depth > 24)
        return nullptr;
    if (root.Name() == name)
        return root;
    int count = VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < count; i++)
    {
        auto child = VisualTreeHelper::GetChild(root, i).try_as<FrameworkElement>();
        if (auto found = FindNamed(child, name, depth + 1))
            return found;
    }
    return nullptr;
}

static FrameworkElement LiveTaskbarRoot()
{
    HWND wnd = taskwin::FindCurrentProcessTaskbarWnd();
    if (!wnd)
        return nullptr;
    auto xamlRoot = taskxaml::GetTaskbarXamlRoot(wnd);
    return xamlRoot ? xamlRoot.Content().try_as<FrameworkElement>() : nullptr;
}

static Panel LiveTray()
{
    auto root = LiveTaskbarRoot();
    return root ? FindNamed(root, L"SystemTrayFrameGrid").try_as<Panel>() : nullptr;
}

// Locate the application repeater's actual parent for icon-area insertion.
// Do not substitute a tray slot when the XAML hierarchy differs.
static bool FindApplicationRepeater(FrameworkElement const& root, Panel& parent,
                                    FrameworkElement& repeater, int depth = 0)
{
    if (!root || depth > 24)
        return false;
    // Windows 11 taskbar applications are hosted by TaskbarFrameRepeater.
    // Only lease beside a direct child of a supported panel.
    if (auto panel = root.try_as<Panel>())
    {
        for (auto const& child : panel.Children())
        {
            auto element = child.try_as<FrameworkElement>();
            if (element && element.Name() == L"TaskbarFrameRepeater" &&
                slot::Classify(panel) != slot::Kind::Unsupported)
            {
                parent = panel;
                repeater = element;
                return true;
            }
        }
    }
    int count = VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < count; i++)
    {
        auto child = VisualTreeHelper::GetChild(root, i).try_as<FrameworkElement>();
        if (child && FindApplicationRepeater(child, parent, repeater, depth + 1))
            return true;
    }
    return false;
}

// Center uses a full-width taskbar root Grid overlay rather than a leased
// application/tray slot. Only accept a root that actually spans the taskbar;
// otherwise skip rather than accidentally center inside a small subpanel.
static Panel ResolveCenterParent()
{
    auto root = LiveTaskbarRoot();
    auto grid = root.try_as<Grid>();
    HWND wnd = taskwin::FindCurrentProcessTaskbarWnd();
    RECT rect{};
    if (!grid || !wnd || !GetWindowRect(wnd, &rect))
        return nullptr;
    double dpi = (double)GetDpiForWindow(wnd);
    double taskbarWidth = (rect.right - rect.left) * 96.0 / dpi;
    if (taskbarWidth <= 0 || grid.ActualWidth() < taskbarWidth * 0.75)
    {
        Wh_Log(L"[Mawaqit] Center skipped: root Grid width %.1f vs taskbar %.1f DIP",
               grid.ActualWidth(), taskbarWidth);
        return nullptr;
    }
    Panel iconsParent{nullptr};
    FrameworkElement repeater{nullptr};
    auto tray = FindNamed(root, L"SystemTrayFrameGrid");
    if (!tray || tray.ActualWidth() <= 0 || !FindApplicationRepeater(root, iconsParent, repeater) ||
        !repeater || repeater.ActualWidth() <= 0)
    {
        Wh_Log(L"[Mawaqit] Center waiting for application/tray layout");
        return nullptr;
    }
    return grid;
}

static Panel ResolvePositionParent(int& slotIndex)
{
    slotIndex = -1;
    if (g_position == L"after-icons")
    {
        // Unlike before-icons, the repeater's column spans nearly the whole
        // taskbar. Use a non-layout-affecting overlay anchored to realized icons.
        auto root = LiveTaskbarRoot();
        auto grid = root.try_as<Grid>();
        Panel iconsParent{nullptr};
        FrameworkElement repeater{nullptr};
        if (grid && grid.ActualWidth() > 0 &&
            FindApplicationRepeater(root, iconsParent, repeater) && repeater &&
            repeater.ActualWidth() > 0)
        {
            slotIndex = 0;
            return grid;
        }
        return nullptr;
    }
    if (g_position == L"center")
    {
        auto center = ResolveCenterParent();
        if (center)
            slotIndex = 0;
        return center;
    }
    if (g_position == L"before-tray")
    {
        auto tray = LiveTray();
        if (tray && slot::ResolveSlot(tray, slot::Anchor::BeforeIcons, slotIndex))
            return tray;
        return nullptr;
    }
    if (g_position == L"after-tray")
    {
        auto tray = LiveTray();
        if (!tray)
            return nullptr;
        if (slot::ResolveSlot(tray, slot::Anchor::AfterShowDesktop, slotIndex))
            return tray;
        Wh_Log(L"[Mawaqit] After-tray anchor unavailable; named children: %s",
               slot::DescribeChildren(tray).c_str());
        return nullptr;
    }
    auto root = LiveTaskbarRoot();
    Panel parent{nullptr};
    FrameworkElement repeater{nullptr};
    if (root && FindApplicationRepeater(root, parent, repeater))
    {
        // During Explorer startup the repeater can exist before its first
        // layout pass. Inserting at that point can lease column zero and
        // overlay the Start area. Let the startup worker retry instead.
        if (root.ActualWidth() <= 0 || parent.ActualWidth() <= 0 || repeater.ActualWidth() <= 0 ||
            repeater.ActualHeight() <= 0)
        {
            Wh_Log(L"[Mawaqit] Icon anchor waiting for layout: root=%.1f parent=%.1f "
                   L"repeater=%.1fx%.1f",
                   root.ActualWidth(), parent.ActualWidth(), repeater.ActualWidth(),
                   repeater.ActualHeight());
            return nullptr;
        }
        try
        {
            auto point = repeater.TransformToVisual(parent).TransformPoint({0, 0});
            if (!std::isfinite(point.X) || !std::isfinite(point.Y) || point.X < -1.0 ||
                point.X > parent.ActualWidth() + 1.0)
            {
                Wh_Log(L"[Mawaqit] Icon anchor waiting for stable coordinates: x=%.1f", point.X);
                return nullptr;
            }
        }
        catch (...)
        {
            Wh_Log(L"[Mawaqit] Icon anchor transform unavailable; retrying");
            return nullptr;
        }
        auto kind = slot::Classify(parent);
        if (kind == slot::Kind::Columns)
        {
            int column = Grid::GetColumn(repeater);
            int span = std::max(1, Grid::GetColumnSpan(repeater));
            slotIndex = column + (g_position == L"after-icons" ? span : 0);
        }
        else if (kind == slot::Kind::Order)
        {
            int index = slot::IndexOfChild(parent, repeater);
            if (index >= 0)
                slotIndex = index + (g_position == L"after-icons" ? 1 : 0);
        }
        Wh_Log(L"[Mawaqit] Icon anchor: parent=%s repeater=%s slot=%d children=%s",
               slot::ClassName(parent).c_str(), slot::ClassName(repeater).c_str(), slotIndex,
               slot::DescribeChildren(parent).c_str());
        if (slotIndex >= 0)
            return parent;
    }
    Wh_Log(L"[Mawaqit] Icon position '%s' unavailable: no supported TaskbarFrameRepeater parent; "
           L"insertion skipped",
           g_position.c_str());
    return nullptr;
}

// Keep the overlay centered until the live application repeater reaches it,
// then translate right, stopping before the tray. Windows icon layout and
// overflow behavior remain unchanged.
// UI-thread-only diagnostic state; centering still recalculates on every tick.
struct CenterDiagnosticState
{
    bool valid = false;
    bool awaitingButtons = false;
    double values[9]{};
    bool room = false;
};
static CenterDiagnosticState g_centerDiagnostic;

static void UpdateDynamicCenterOnUiThread()
{
    if (g_position != L"center" || !g_pill)
        return;
    auto root = LiveTaskbarRoot();
    auto grid = root.try_as<Grid>();
    auto tray = LiveTray();
    if (!grid || !tray || grid.ActualWidth() <= 0)
        return;
    Panel iconsParent{nullptr};
    FrameworkElement repeater{nullptr};
    if (!FindApplicationRepeater(root, iconsParent, repeater) || !repeater)
        return;
    try
    {
        auto point = winrt::Windows::Foundation::Point{0, 0};
        auto appOrigin = repeater.TransformToVisual(grid).TransformPoint(point);
        auto trayOrigin = tray.TransformToVisual(grid).TransformPoint(point);
        double pillWidth = g_pill.ActualWidth();
        if (pillWidth <= 0)
            pillWidth = g_pill.Width();
        double centeredLeft = (grid.ActualWidth() - pillWidth) / 2.0;
        // ItemsRepeater can occupy the entire taskbar even when its realized
        // buttons use only a small portion. Measure the realized children.
        double appRight = appOrigin.X;
        unsigned measuredButtons = 0;
        int count = VisualTreeHelper::GetChildrenCount(repeater);
        for (int i = 0; i < count; ++i)
        {
            auto child = VisualTreeHelper::GetChild(repeater, i).try_as<FrameworkElement>();
            if (!child || child.Visibility() != Visibility::Visible || child.ActualWidth() <= 0 ||
                child.ActualWidth() > 400.0)
                continue;
            auto pos = child.TransformToVisual(grid).TransformPoint({0, 0});
            if (std::isfinite(pos.X) && pos.X >= 0 &&
                pos.X + child.ActualWidth() <= grid.ActualWidth() + 1.0)
            {
                appRight = std::max(appRight, pos.X + child.ActualWidth());
                ++measuredButtons;
            }
        }
        // If buttons are not realized yet, do not mistake the full-width
        // repeater for occupied space; defer visibility until the next tick.
        if (measuredButtons == 0)
        {
            if (!g_centerDiagnostic.awaitingButtons)
                Wh_Log(L"[Mawaqit] Center awaiting realized buttons: repeaterW=%.1f children=%d",
                       repeater.ActualWidth(), count);
            g_centerDiagnostic.awaitingButtons = true;
            g_centerDiagnostic.valid = false;
            g_pill.Visibility(Visibility::Collapsed);
            return;
        }
        double gap = 5.0;
        double maximumLeft = trayOrigin.X - pillWidth - gap;
        double wantedLeft = std::max(centeredLeft, appRight + gap);
        // If the available region is exhausted, hide rather than overlap the
        // system tray or the application buttons. This does not force overflow.
        bool room = maximumLeft >= centeredLeft && wantedLeft <= maximumLeft;
        const double diagnosticValues[] = {
            grid.ActualWidth(), appOrigin.X, repeater.ActualWidth(), appRight,
            trayOrigin.X, pillWidth, centeredLeft, wantedLeft, maximumLeft};
        bool layoutChanged = !g_centerDiagnostic.valid ||
                             g_centerDiagnostic.room != room;
        for (size_t i = 0; i < 9 && !layoutChanged; ++i)
            layoutChanged = !std::isfinite(diagnosticValues[i]) ||
                            !std::isfinite(g_centerDiagnostic.values[i]) ||
                            std::abs(diagnosticValues[i] - g_centerDiagnostic.values[i]) >= 0.5;
        if (layoutChanged)
        {
            Wh_Log(
                L"[Mawaqit] Center layout: root=%.1f repeaterX=%.1f repeaterW=%.1f appRight=%.1f "
                L"trayX=%.1f pillW=%.1f centered=%.1f wanted=%.1f max=%.1f room=%d",
                grid.ActualWidth(), appOrigin.X, repeater.ActualWidth(), appRight, trayOrigin.X,
                pillWidth, centeredLeft, wantedLeft, maximumLeft, room ? 1 : 0);
            for (size_t i = 0; i < 9; ++i)
                g_centerDiagnostic.values[i] = diagnosticValues[i];
            g_centerDiagnostic.room = room;
            g_centerDiagnostic.valid = true;
        }
        g_centerDiagnostic.awaitingButtons = false;
        g_pill.Visibility(room ? Visibility::Visible : Visibility::Collapsed);
        double left = std::min(wantedLeft, maximumLeft);
        if (room)
        {
            auto translation = g_pill.RenderTransform().try_as<TranslateTransform>();
            if (!translation)
            {
                translation = TranslateTransform();
                g_pill.RenderTransform(translation);
            }
            translation.X(left - centeredLeft);
        }
    }
    catch (...)
    {
        // Do not disturb the stable anchored positions on unsupported trees.
        Wh_Log(L"[Mawaqit] Dynamic center bounds unavailable; retaining last placement");
    }
}

// After-icons is visually anchored to the last realized taskbar button.
// Never resize or re-column the taskbar's full-width ItemsRepeater.
static int g_afterIconsDiagnosticCount = 0;
static void UpdateAfterIconsOnUiThread()
{
    if (g_position != L"after-icons" || !g_pill)
        return;
    auto root = LiveTaskbarRoot();
    auto grid = root.try_as<Grid>();
    if (!grid || grid.ActualWidth() <= 0)
        return;
    Panel iconsParent{nullptr};
    FrameworkElement repeater{nullptr};
    if (!FindApplicationRepeater(root, iconsParent, repeater) || !repeater)
        return;
    try
    {
        auto origin = repeater.TransformToVisual(grid).TransformPoint({0, 0});
        double appRight = origin.X;
        unsigned measured = 0;
        int count = VisualTreeHelper::GetChildrenCount(repeater);
        for (int i = 0; i < count; ++i)
        {
            auto child = VisualTreeHelper::GetChild(repeater, i).try_as<FrameworkElement>();
            if (!child || child.Visibility() != Visibility::Visible || child.ActualWidth() <= 0 ||
                child.ActualWidth() > 400.0)
                continue;
            auto pos = child.TransformToVisual(grid).TransformPoint({0, 0});
            if (std::isfinite(pos.X) && pos.X >= 0 &&
                pos.X + child.ActualWidth() <= grid.ActualWidth() + 1.0)
            {
                appRight = std::max(appRight, pos.X + child.ActualWidth());
                ++measured;
            }
        }
        double pillWidth = g_pill.ActualWidth();
        if (pillWidth <= 0)
            pillWidth = g_pill.Width();
        double centeredLeft = (grid.ActualWidth() - pillWidth) / 2.0;
        double desiredLeft = appRight + 5.0;
        double rightLimit = grid.ActualWidth();
        if (auto tray = LiveTray())
        {
            auto trayPos = tray.TransformToVisual(grid).TransformPoint({0, 0});
            rightLimit = trayPos.X;
        }
        bool room = measured > 0 && desiredLeft + pillWidth + 5.0 <= rightLimit;
        g_pill.Visibility(room ? Visibility::Visible : Visibility::Collapsed);
        if (room)
        {
            auto transform = g_pill.RenderTransform().try_as<TranslateTransform>();
            if (!transform)
            {
                transform = TranslateTransform();
                g_pill.RenderTransform(transform);
            }
            transform.X(desiredLeft - centeredLeft);
        }
        if (g_afterIconsDiagnosticCount < 15)
        {
            Wh_Log(L"[Mawaqit] After-icons: repeaterW=%.1f measured=%u appRight=%.1f desired=%.1f "
                   L"trayLeft=%.1f pillW=%.1f room=%d",
                   repeater.ActualWidth(), measured, appRight, desiredLeft, rightLimit, pillWidth,
                   room ? 1 : 0);
            ++g_afterIconsDiagnosticCount;
        }
    }
    catch (...)
    {
        Wh_Log(L"[Mawaqit] After-icons measurement unavailable; retrying");
    }
}

// Secondary taskbars own independent XAML objects, leases, and event tokens.
// Never reuse the primary widget's XAML objects, lease, or parent.
struct SecondaryClockInstance
{
    HWND taskbar = nullptr;
    std::wstring monitor;
    winrt::weak_ref<Panel> parent;
    winrt::weak_ref<Grid> pill;
    winrt::weak_ref<TextBlock> prayer;
    winrt::weak_ref<TextBlock> countdown;
    winrt::weak_ref<TextBlock> iqamah;
    winrt::weak_ref<TextBlock> iqamahShadow;
    winrt::weak_ref<TextBlock> prayerShadow;
    winrt::weak_ref<TextBlock> countdownShadow;
    slot::Lease lease{};
    winrt::event_token tapToken{};
    bool tapRegistered = false;
    winrt::event_token hoverToken{};
    bool hoverRegistered = false;
};
static std::vector<SecondaryClockInstance> g_secondaryClocks;
static constexpr wchar_t kSecondarySlotName[] = L"MawaqitSecondaryClockSlot";
static constexpr wchar_t kSecondaryLeaseName[] = L"MawaqitSecondaryClockLease";

// The shadow is a second, non-hit-testable TextBlock underneath the actual text.
// It is offset by 1 DIP, with no effect on the foreground color or pill opacity.
static Grid MakePillTextWithShadow(TextBlock const& foreground, TextBlock& shadow)
{
    Grid layer;
    layer.VerticalAlignment(VerticalAlignment::Center);
    shadow = TextBlock();
    shadow.FontSize(foreground.FontSize());
    shadow.VerticalAlignment(VerticalAlignment::Center);
    shadow.Margin(foreground.Margin());
    shadow.IsHitTestVisible(false);
    shadow.Opacity(0.50);
    SolidColorBrush ink;
    ink.Color(winrt::Windows::UI::Color{255, 0, 0, 0});
    shadow.Foreground(ink);
    TranslateTransform offset;
    offset.X(0.75);
    offset.Y(1.0);
    shadow.RenderTransform(offset);
    layer.Children().Append(shadow);
    layer.Children().Append(foreground);
    return layer;
}

static TextBlock MakeIqamahText(winrt::Windows::UI::Color countdownColor)
{
    TextBlock iqamah;
    iqamah.FontSize((double)g_fontSize * g_iqamahFontScale / 100.0);
    iqamah.VerticalAlignment(VerticalAlignment::Center);
    iqamah.Margin({3, 0, 5, 0});
    unsigned rgb;
    if (TryThemeOverride(L"Advanced.CustomThemeSettings.PillIqamah", rgb))
        countdownColor = Rgb(rgb);
    SolidColorBrush brush;
    brush.Color(countdownColor);
    iqamah.Foreground(brush);
    return iqamah;
}

// Auto width is measured in XAML DIPs on the taskbar UI thread. Use representative
// maximum-length countdown and Iqamah strings so the pill does not jitter each second.
// Fixed widths bypass this calculation entirely.
static double EffectivePillWidth()
{
    if (g_width != 0)
        return (double)g_width;

    auto measure = [](std::wstring const& value, double fontSize) -> double
    {
        TextBlock sample;
        sample.FontSize(fontSize);
        sample.Text(value);
        sample.Measure(winrt::Windows::Foundation::Size{10000.0f, 1000.0f});
        return sample.DesiredSize().Width;
    };

    double width = (double)g_padding.left + g_padding.right;
    if (g_showNext)
        width += measure(g_label.size() > 7 ? g_label : L"Maghrib", (double)g_fontSize) + 9.0;
    if (g_theme == L"custom" && g_showNext && g_showCountdown)
        width += measure(L"|", (double)g_fontSize);
    if (g_showCountdown)
    {
        width += measure(L"-23:59:59", (double)g_fontSize) + 9.0;
        if (g_showIqamah)
            width += measure(L"+180", (double)g_fontSize * g_iqamahFontScale / 100.0) + 8.0;
    }
    // Leave a small allowance for glyph rounding, borders, and the shadow offset.
    return std::max(32.0, std::ceil(width + 4.0));
}

static void RemoveSecondaryClocksOnUiThread()
{
    for (auto& instance : g_secondaryClocks)
    {
        auto pill = instance.pill.get();
        if (pill)
        {
            if (instance.hoverRegistered)
            {
                try
                {
                    pill.PointerEntered(instance.hoverToken);
                }
                catch (...)
                {
                }
                instance.hoverRegistered = false;
            }
            if (instance.tapRegistered)
            {
                try
                {
                    pill.Tapped(instance.tapToken);
                }
                catch (...)
                {
                }
                instance.tapRegistered = false;
            }
        }
        auto parent = instance.parent.get();
        if (parent)
        {
            for (int i = (int)parent.Children().Size() - 1; i >= 0; --i)
            {
                auto child = parent.Children().GetAt(i).try_as<FrameworkElement>();
                if (child && child.Name() == kSecondarySlotName)
                    parent.Children().RemoveAt(i);
            }
            if (!instance.lease.markerName.empty())
                slot::Release(parent, instance.lease);
        }
    }
    g_secondaryClocks.clear();
}

// A weak reference can remain alive after Explorer detaches a taskbar tree.
// Validate that the tracked pill still belongs to its recorded parent.
static bool SecondaryClockAttachedOnUiThread(SecondaryClockInstance const& instance)
{
    if (!IsWindow(instance.taskbar))
        return false;
    auto parent = instance.parent.get();
    auto pill = instance.pill.get();
    if (!parent || !pill)
        return false;
    try
    {
        for (auto const& child : parent.Children())
            if (child.try_as<Grid>() == pill)
                return true;
    }
    catch (...)
    {
    }
    return false;
}

static void EnsureSecondaryClocksOnUiThread()
{
    if (g_stopping.load() || !g_showSecondaryMonitors)
        return;
    // Retire detached or destroyed secondary instances before rediscovery.
    // Removal uses the existing token-revocation and slot-release path.
    bool stale = false;
    for (auto const& instance : g_secondaryClocks)
        if (!SecondaryClockAttachedOnUiThread(instance))
        {
            stale = true;
            break;
        }
    if (stale)
    {
        Wh_Log(L"[Mawaqit][Monitor] Stale secondary clock detected; rebuilding secondary slots");
        RemoveSecondaryClocksOnUiThread();
    }
    for (auto const& record : mawaqit_monitor_discovery::EnumerateTaskbars())
    {
        if (record.primaryTaskbar || !IsWindow(record.hwnd))
            continue;
        bool existing = false;
        for (auto const& instance : g_secondaryClocks)
            if (instance.taskbar == record.hwnd && SecondaryClockAttachedOnUiThread(instance))
            {
                existing = true;
                break;
            }
        if (existing)
            continue;
        // Resolve the secondary XAML tree without development-only hierarchy dumps.
        auto xamlRoot = taskxaml::GetSecondaryTaskbarXamlRoot(record.hwnd, false);
        auto root = xamlRoot ? xamlRoot.Content().try_as<FrameworkElement>() : nullptr;
        auto trayElement = root ? FindNamed(root, L"SystemTrayFrameGrid") : nullptr;
        auto parent = trayElement ? trayElement.try_as<Panel>() : nullptr;
        auto kind = parent ? slot::Classify(parent) : slot::Kind::Unsupported;
        if (!parent || kind == slot::Kind::Unsupported)
        {
            continue;
        }
        // Before-icons requires a real layout lease in the secondary icon
        // repeater's parent; overlays cannot make room to its left.
        bool overlay = g_position == L"center" || g_position == L"after-icons";
        int index = -1;
        if (g_position == L"before-icons")
        {
            Panel iconsParent{nullptr};
            FrameworkElement repeater{nullptr};
            if (!root || !FindApplicationRepeater(root, iconsParent, repeater) || !iconsParent ||
                !repeater || iconsParent.ActualWidth() <= 0 || repeater.ActualWidth() <= 0 ||
                repeater.ActualHeight() <= 0)
            {
                Wh_Log(L"[Mawaqit][Monitor][BeforeIcons] waiting for secondary icon repeater on %s",
                       record.device.c_str());
                continue;
            }
            auto iconKind = slot::Classify(iconsParent);
            if (iconKind == slot::Kind::Columns)
                index = Grid::GetColumn(repeater);
            else if (iconKind == slot::Kind::Order)
                index = slot::IndexOfChild(iconsParent, repeater);
            if (index < 0)
            {
                Wh_Log(L"[Mawaqit][Monitor][BeforeIcons] no supported icon slot on %s",
                       record.device.c_str());
                continue;
            }
            parent = iconsParent;
            Wh_Log(L"[Mawaqit][Monitor][BeforeIcons] monitor=%s parent=%s slot=%d repeaterW=%.1f "
                   L"children=%s",
                   record.device.c_str(), slot::ClassName(parent).c_str(), index,
                   repeater.ActualWidth(), slot::DescribeChildren(parent).c_str());
        }
        else if (overlay)
        {
            auto rootGrid = root.try_as<Grid>();
            RECT bar{};
            double expectedWidth = 0;
            if (GetWindowRect(record.hwnd, &bar))
                expectedWidth =
                    (bar.right - bar.left) * 96.0 / std::max(96u, GetDpiForWindow(record.hwnd));
            if (!rootGrid || expectedWidth <= 0 || rootGrid.ActualWidth() < expectedWidth * 0.75)
            {
                Wh_Log(L"[Mawaqit][Monitor] Overlay awaiting full-width secondary root: %s",
                       record.device.c_str());
                continue;
            }
            parent = rootGrid;
        }
        else
        {
            slot::Anchor anchor = g_position == L"after-tray" ? slot::Anchor::AfterShowDesktop
                                                              : slot::Anchor::BeforeIcons;
            if (!slot::ResolveSlot(parent, anchor, index))
            {
                Wh_Log(L"[Mawaqit][Monitor] Anchor %s unavailable on %s; using before-icons",
                       g_position.c_str(), record.device.c_str());
                anchor = slot::Anchor::BeforeIcons;
                if (!slot::ResolveSlot(parent, anchor, index))
                    continue;
            }
        }
        Wh_Log(L"[Mawaqit][Monitor] Secondary position: monitor=%s requested=%s mode=%s slot=%d",
               record.device.c_str(), g_position.c_str(), overlay ? L"overlay" : L"lease", index);
        SecondaryClockInstance instance{};
        instance.taskbar = record.hwnd;
        instance.monitor = record.device;
        if (!overlay && !slot::AcquireAt(parent, index, kSecondaryLeaseName, instance.lease))
            continue;
        try
        {
            Grid box;
            box.Name(kSecondarySlotName);
            box.Width(EffectivePillWidth() + g_trayGap);
            box.Height(g_pillHeight > 0 ? (double)g_pillHeight : 30.0);
            box.VerticalAlignment(VerticalAlignment::Center);
            Border surface;
            auto palette = ResolvePillPalette(g_theme);
            surface.Background(CreatePillBackground(palette.background));
            if (g_showPillBorder)
            {
                unsigned int borderRgb;
                auto borderColor = palette.countdown;
                if (TryThemeOverride(L"Advanced.CustomThemeSettings.PillBorder", borderRgb))
                    borderColor = Rgb(borderRgb);
                surface.BorderBrush(winrt::Windows::UI::Xaml::Media::SolidColorBrush(borderColor));
                surface.BorderThickness({1, 1, 1, 1});
            }
            double r = (double)g_cornerRadius;
            surface.CornerRadius({r, r, r, r});
            surface.Margin({0, 0, (double)g_trayGap, 0});
            // Match the primary pill's visual margins and position corrections.
            // Keep the native secondary taskbar slot unchanged.
            double positionOffsetX = 0.0;
            if (g_position == L"after-tray")
                positionOffsetX = -5.0;
            else if (g_position == L"before-icons")
                positionOffsetX = 5.0;
            TranslateTransform offset;
            offset.X((double)(g_margins.left - g_margins.right) + positionOffsetX);
            offset.Y((double)(g_margins.top - g_margins.bottom));
            surface.RenderTransform(offset);
            surface.Padding({(double)g_padding.left, (double)g_padding.top, (double)g_padding.right,
                             (double)g_padding.bottom});
            StackPanel content;
            content.Orientation(Orientation::Horizontal);
            content.HorizontalAlignment(HorizontalAlignment::Center);
            content.VerticalAlignment(VerticalAlignment::Center);
            TextBlock prayer;
            prayer.FontSize((double)g_fontSize);
            prayer.Margin({5, 0, 4, 0});
            prayer.VerticalAlignment(VerticalAlignment::Center);
            SolidColorBrush fg;
            fg.Color(palette.foreground);
            prayer.Foreground(fg);
            TextBlock countdown;
            countdown.FontSize((double)g_fontSize);
            countdown.Margin({4, 0, 5, 0});
            countdown.VerticalAlignment(VerticalAlignment::Center);
            SolidColorBrush accent;
            accent.Color(palette.countdown);
            countdown.Foreground(accent);
            TextBlock iqamah = MakeIqamahText(palette.countdown);
            TextBlock iqamahShadow{nullptr};
            TextBlock prayerShadow{nullptr};
            TextBlock countdownShadow{nullptr};
            // Hidden components must not reserve margins or shadow-layer space.
            prayer.Visibility(g_showNext ? Visibility::Visible : Visibility::Collapsed);
            countdown.Visibility(g_showCountdown ? Visibility::Visible : Visibility::Collapsed);
            iqamah.Visibility((g_showCountdown && g_showIqamah) ? Visibility::Visible : Visibility::Collapsed);
            if (g_pillTextShadow)
            {
                auto layer = MakePillTextWithShadow(prayer, prayerShadow);
                layer.Visibility(g_showNext ? Visibility::Visible : Visibility::Collapsed);
                content.Children().Append(layer);
            }
            else
                content.Children().Append(prayer);
            if (g_theme == L"custom" && g_showNext && g_showCountdown)
            {
                TextBlock separator;
                separator.Text(L"|");
                separator.FontSize((double)g_fontSize);
                SolidColorBrush separatorBrush;
                separatorBrush.Color(palette.separator);
                separator.Foreground(separatorBrush);
                content.Children().Append(separator);
            }
            if (g_pillTextShadow)
            {
                auto layer = MakePillTextWithShadow(countdown, countdownShadow);
                layer.Visibility(g_showCountdown ? Visibility::Visible : Visibility::Collapsed);
                content.Children().Append(layer);
            }
            else
                content.Children().Append(countdown);
            if (g_pillTextShadow)
            {
                auto layer = MakePillTextWithShadow(iqamah, iqamahShadow);
                layer.Visibility((g_showCountdown && g_showIqamah) ? Visibility::Visible : Visibility::Collapsed);
                content.Children().Append(layer);
            }
            else
                content.Children().Append(iqamah);
            surface.Child(content);
            box.Children().Append(surface);
            if (overlay)
            {
                auto grid = parent.try_as<Grid>();
                Grid::SetColumn(box, 0);
                Grid::SetColumnSpan(box, std::max(1, (int)grid.ColumnDefinitions().Size()));
                Grid::SetRow(box, 0);
                Grid::SetRowSpan(box, std::max(1, (int)grid.RowDefinitions().Size()));
                box.HorizontalAlignment(HorizontalAlignment::Center);
                Canvas::SetZIndex(box, 100);
                grid.Children().Append(box);
            }
            else if (!slot::PlaceChild(parent, instance.lease, box))
            {
                slot::Release(parent, instance.lease);
                continue;
            }
            // The popup is still shared; anchor refinement follows after the
            // secondary rendering contract is verified on a real machine.
            // Use the event sender rather than strongly capturing the event source.
            // Store each token immediately so partial insertion can revoke it.
            instance.pill = box;
            instance.tapToken = box.Tapped(
                [secondaryTaskbar = record.hwnd](auto const& sender, auto const&)
                {
                    if (auto pill = sender.template try_as<Grid>())
                        TogglePrayerPopup(pill, secondaryTaskbar);
                });
            instance.tapRegistered = true;
            instance.hoverToken = box.PointerEntered(
                [secondaryTaskbar = record.hwnd](auto const& sender, auto const&)
                {
                    if (g_openPopupOnHover)
                        if (auto pill = sender.template try_as<Grid>())
                            TogglePrayerPopup(pill, secondaryTaskbar, true);
                });
            instance.hoverRegistered = true;
            instance.parent = parent;
            instance.prayer = prayer;
            instance.countdown = countdown;
            instance.iqamah = iqamah;
            instance.iqamahShadow = iqamahShadow;
            instance.prayerShadow = prayerShadow;
            instance.countdownShadow = countdownShadow;
            prayer.Text(g_showNext ? g_label : L"");
            countdown.Text(g_showCountdown ? PillCountdown() : L"");
            iqamah.Text(PillIqamah());
            if (iqamahShadow)
                iqamahShadow.Text(PillIqamah());
            g_secondaryClocks.push_back(std::move(instance));
            Wh_Log(L"[Mawaqit][Monitor] Secondary clock inserted on %s hwnd=%p",
                   record.device.c_str(), record.hwnd);
        }
        catch (...)
        {
            // Roll back subscriptions before detaching partially inserted XAML.
            if (auto pill = instance.pill.get())
            {
                if (instance.hoverRegistered)
                {
                    try
                    {
                        pill.PointerEntered(instance.hoverToken);
                    }
                    catch (...)
                    {
                    }
                }
                if (instance.tapRegistered)
                {
                    try
                    {
                        pill.Tapped(instance.tapToken);
                    }
                    catch (...)
                    {
                    }
                }
            }
            for (int i = (int)parent.Children().Size() - 1; i >= 0; --i)
            {
                auto child = parent.Children().GetAt(i).try_as<FrameworkElement>();
                if (child && child.Name() == kSecondarySlotName)
                    parent.Children().RemoveAt(i);
            }
            slot::Release(parent, instance.lease);
            Wh_Log(L"[Mawaqit][Monitor] Secondary insertion failed on %s", record.device.c_str());
        }
    }
}

static void UpdateSecondaryClocksOnUiThread()
{
    EnsureSecondaryClocksOnUiThread();
    for (auto& instance : g_secondaryClocks)
    {
        if (auto prayer = instance.prayer.get())
            prayer.Text(g_showNext ? g_label : L"");
        if (auto countdown = instance.countdown.get())
            countdown.Text(g_showCountdown ? PillCountdown() : L"");
        if (auto iqamah = instance.iqamah.get())
            iqamah.Text(PillIqamah());
        if (auto shadow = instance.iqamahShadow.get())
            shadow.Text(PillIqamah());
        if (auto shadow = instance.prayerShadow.get())
            shadow.Text(g_showNext ? g_label : L"");
        if (auto shadow = instance.countdownShadow.get())
            shadow.Text(g_showCountdown ? PillCountdown() : L"");
        if (g_width == 0)
            if (auto pill = instance.pill.get())
                pill.Width(EffectivePillWidth() + g_trayGap);
        if ((g_position == L"center" || g_position == L"after-icons") && instance.pill.get())
        {
            try
            {
                auto pill = instance.pill.get();
                auto grid = instance.parent.get().try_as<Grid>();
                if (!grid || grid.ActualWidth() <= 0)
                    continue;
                double width = pill.ActualWidth() > 0 ? pill.ActualWidth() : pill.Width();
                double centered = (grid.ActualWidth() - width) / 2.0;
                double desired = centered;
                auto tray = FindNamed(grid, L"SystemTrayFrameGrid");
                double trayLeft = grid.ActualWidth();
                if (tray)
                    trayLeft = tray.TransformToVisual(grid).TransformPoint({0, 0}).X;
                Panel iconsParent{nullptr};
                FrameworkElement repeater{nullptr};
                double appRight = 0;
                bool measured = false;
                if (FindApplicationRepeater(grid, iconsParent, repeater) && repeater)
                {
                    appRight = repeater.TransformToVisual(grid).TransformPoint({0, 0}).X;
                    int count = VisualTreeHelper::GetChildrenCount(repeater);
                    for (int i = 0; i < count; ++i)
                    {
                        auto child =
                            VisualTreeHelper::GetChild(repeater, i).try_as<FrameworkElement>();
                        if (!child || child.Visibility() != Visibility::Visible ||
                            child.ActualWidth() <= 0 || child.ActualWidth() > 400)
                            continue;
                        auto p = child.TransformToVisual(grid).TransformPoint({0, 0});
                        if (std::isfinite(p.X) && p.X >= 0 &&
                            p.X + child.ActualWidth() <= grid.ActualWidth() + 1)
                        {
                            appRight = std::max(appRight, p.X + child.ActualWidth());
                            measured = true;
                        }
                    }
                }
                desired = g_position == L"after-icons" ? appRight + 5.0
                                                       : std::max(centered, appRight + 5.0);
                bool room = measured && desired + width + 5.0 <= trayLeft;
                pill.Visibility(room ? Visibility::Visible : Visibility::Collapsed);
                if (room)
                {
                    auto translation = pill.RenderTransform().try_as<TranslateTransform>();
                    if (!translation)
                    {
                        translation = TranslateTransform();
                        pill.RenderTransform(translation);
                    }
                    translation.X(desired - centered);
                }
            }
            catch (...)
            {
                Wh_Log(L"[Mawaqit][Monitor] Secondary overlay layout retry");
            }
        }
    }
}

static void RemoveOnUiThread()
{
    RemoveSecondaryClocksOnUiThread();
    g_centerDiagnostic = {};
    g_afterIconsDiagnosticCount = 0;
    DestroyPrayerPopup();
    if (g_pill)
    {
        if (g_hoverRegistered)
        {
            try
            {
                g_pill.PointerEntered(g_hoverToken);
            }
            catch (...)
            {
            }
            g_hoverRegistered = false;
        }
        if (g_tapRegistered)
        {
            try
            {
                g_pill.Tapped(g_tapToken);
            }
            catch (...)
            {
            }
            g_tapRegistered = false;
        }
    }
    auto parent = g_parent.get();
    if (!parent)
        parent = LiveTray();
    if (parent)
    {
        for (int i = (int)parent.Children().Size() - 1; i >= 0; --i)
        {
            auto fe = parent.Children().GetAt(i).try_as<FrameworkElement>();
            if (fe && fe.Name() == kSlotName)
                parent.Children().RemoveAt(i);
        }
        if (!g_lease.markerName.empty())
        {
            if (!slot::Release(parent, g_lease))
                Wh_Log(L"[Mawaqit] Could not release old lease; taskbar may have rebuilt");
        }
    }
    g_placeholder = {};
    g_parent = {};
    g_lease = {};
    g_prayerText = nullptr;
    g_countdownText = nullptr;
    g_iqamahText = nullptr;
    g_iqamahShadow = nullptr;
    g_prayerShadow = nullptr;
    g_countdownShadow = nullptr;
    g_pill = nullptr;
}

static void InsertOnUiThread()
{
    if (g_stopping.load())
        return;
    if (!g_showPrimaryMonitor)
    {
        Wh_Log(L"[Mawaqit][Monitor] Primary clock disabled by settings");
        UpdateSecondaryClocksOnUiThread();
        return;
    }
    int slotIndex = -1;
    auto parent = ResolvePositionParent(slotIndex);
    if (!parent)
    {
        Wh_Log(L"[Mawaqit] Position parent unavailable: %s", g_position.c_str());
        return;
    }
    if (slot::Classify(parent) == slot::Kind::Unsupported)
    {
        Wh_Log(L"[Mawaqit] Unsupported tray panel: %s", slot::ClassName(parent).c_str());
        return;
    }
    // Existing slot? This prevents duplicates during repeated initialization.
    for (auto child : parent.Children())
    {
        auto fe = child.try_as<FrameworkElement>();
        if (fe && fe.Name() == kSlotName)
        {
            Wh_Log(L"[Mawaqit] Slot already exists");
            return;
        }
    }
    // Slot is resolved from the selected position before creating XAML content.
    // Layout settings are DIPs, so XAML performs monitor DPI scaling.
    double dpi = 96.0;
    if (auto wnd = taskwin::FindCurrentProcessTaskbarWnd())
        dpi = (double)GetDpiForWindow(wnd);
    double widthDip = EffectivePillWidth();
    Grid box;
    box.Name(kSlotName);
    box.Width(widthDip + (double)g_trayGap);
    box.Height(g_pillHeight > 0 ? (double)g_pillHeight : 30.0);
    box.HorizontalAlignment(HorizontalAlignment::Center);
    box.VerticalAlignment(VerticalAlignment::Center);
    auto palette = ResolvePillPalette(g_theme);
    Border pillSurface;
    pillSurface.Background(CreatePillBackground(palette.background));
    if (g_showPillBorder)
    {
        unsigned int borderRgb;
        auto borderColor = palette.countdown;
        if (TryThemeOverride(L"Advanced.CustomThemeSettings.PillBorder", borderRgb))
            borderColor = Rgb(borderRgb);
        pillSurface.BorderBrush(winrt::Windows::UI::Xaml::Media::SolidColorBrush(borderColor));
        pillSurface.BorderThickness({1, 1, 1, 1});
    }
    double radius = (double)g_cornerRadius;
    pillSurface.CornerRadius({radius, radius, radius, radius});
    pillSurface.Margin({0, 0, (double)g_trayGap, 0});
    // Position offsets are visual only; the native slot remains reserved.
    // Add subtle position-specific spacing without overriding custom margins.
    double positionOffsetX = 0.0;
    if (g_position == L"after-tray")
        positionOffsetX = -5.0; // Equivalent to additional margins: 5, 0, 10, 0.
    else if (g_position == L"before-icons")
        positionOffsetX = 5.0; // Equivalent to additional margins: 10, 0, 5, 0.
    TranslateTransform offset;
    offset.X((double)(g_margins.left - g_margins.right) + positionOffsetX);
    offset.Y((double)(g_margins.top - g_margins.bottom));
    pillSurface.RenderTransform(offset);
    pillSurface.Padding({(double)g_padding.left, (double)g_padding.top, (double)g_padding.right,
                         (double)g_padding.bottom});
    pillSurface.HorizontalAlignment(HorizontalAlignment::Stretch);
    pillSurface.VerticalAlignment(VerticalAlignment::Stretch);
    // XAML text controls in the native taskbar layout.
    StackPanel content;
    content.Orientation(Orientation::Horizontal);
    content.HorizontalAlignment(HorizontalAlignment::Center);
    content.VerticalAlignment(VerticalAlignment::Center);
    TextBlock prayer;
    prayer.FontSize((double)g_fontSize);
    prayer.VerticalAlignment(VerticalAlignment::Center);
    prayer.Margin({5, 0, 4, 0});
    SolidColorBrush foreground;
    foreground.Color(palette.foreground);
    prayer.Foreground(foreground);
    TextBlock countdown;
    countdown.FontSize((double)g_fontSize);
    countdown.VerticalAlignment(VerticalAlignment::Center);
    countdown.Margin({4, 0, 5, 0});
    SolidColorBrush accent;
    accent.Color(palette.countdown);
    countdown.Foreground(accent);
    prayer.Text(L"Loading...");
    countdown.Text(L"--:--");
    TextBlock iqamah = MakeIqamahText(palette.countdown);
    TextBlock iqamahShadow{nullptr};
    TextBlock prayerShadow{nullptr};
    TextBlock countdownShadow{nullptr};
    // Hidden components must not reserve margins or shadow-layer space.
    prayer.Visibility(g_showNext ? Visibility::Visible : Visibility::Collapsed);
    countdown.Visibility(g_showCountdown ? Visibility::Visible : Visibility::Collapsed);
    iqamah.Visibility((g_showCountdown && g_showIqamah) ? Visibility::Visible : Visibility::Collapsed);
    if (g_pillTextShadow)
    {
        auto layer = MakePillTextWithShadow(prayer, prayerShadow);
        layer.Visibility(g_showNext ? Visibility::Visible : Visibility::Collapsed);
        content.Children().Append(layer);
    }
    else
        content.Children().Append(prayer);
    if (g_theme == L"custom" && g_showNext && g_showCountdown)
    {
        TextBlock separator;
        separator.Text(L"|");
        separator.FontSize((double)g_fontSize);
        separator.VerticalAlignment(VerticalAlignment::Center);
        separator.Margin({0, 0, 0, 0});
        SolidColorBrush separatorBrush;
        separatorBrush.Color(palette.separator);
        separator.Foreground(separatorBrush);
        content.Children().Append(separator);
    }
    if (g_pillTextShadow)
    {
        auto layer = MakePillTextWithShadow(countdown, countdownShadow);
        layer.Visibility(g_showCountdown ? Visibility::Visible : Visibility::Collapsed);
        content.Children().Append(layer);
    }
    else
        content.Children().Append(countdown);
    if (g_pillTextShadow)
    {
        auto layer = MakePillTextWithShadow(iqamah, iqamahShadow);
        layer.Visibility((g_showCountdown && g_showIqamah) ? Visibility::Visible : Visibility::Collapsed);
        content.Children().Append(layer);
    }
    else
        content.Children().Append(iqamah);
    pillSurface.Child(content);
    box.Children().Append(pillSurface);
    if (g_position == L"center" || g_position == L"after-icons")
    {
        // A full-width Grid child is horizontally centered, then dynamically shifted.
        // It owns no grid column and must not change existing taskbar layout.
        // The pill remains the only hit-testable part of this overlay.
        auto grid = parent.try_as<Grid>();
        if (!grid)
            return;
        Grid::SetColumn(box, 0);
        Grid::SetColumnSpan(box, std::max(1, (int)grid.ColumnDefinitions().Size()));
        Grid::SetRow(box, 0);
        Grid::SetRowSpan(box, std::max(1, (int)grid.RowDefinitions().Size()));
        box.HorizontalAlignment(HorizontalAlignment::Center);
        box.VerticalAlignment(VerticalAlignment::Center);
        Canvas::SetZIndex(box, 100);
        grid.Children().Append(box);
    }
    else
    {
        if (!slot::AcquireAt(parent, slotIndex, kLeaseName, g_lease))
        {
            Wh_Log(L"[Mawaqit] Lease acquisition failed");
            return;
        }
        bool placed = false;
        try
        {
            placed = slot::PlaceChild(parent, g_lease, box);
        }
        catch (...)
        {
            slot::Release(parent, g_lease);
            throw;
        }
        if (!placed)
        {
            slot::Release(parent, g_lease);
            return;
        }
    }
    g_placeholder = box;
    g_pill = box;
    try
    {
        g_tapToken =
            box.Tapped([](auto const&, auto const&) { TogglePrayerPopup(g_pill, nullptr); });
        g_tapRegistered = true;
        g_hoverToken = box.PointerEntered(
            [](auto const&, auto const&)
            {
                if (g_openPopupOnHover)
                    TogglePrayerPopup(g_pill, nullptr, true);
            });
        g_hoverRegistered = true;
    }
    catch (...)
    {
        // Roll back a partially registered primary pill before propagating.
        if (g_hoverRegistered)
        {
            try
            {
                box.PointerEntered(g_hoverToken);
            }
            catch (...)
            {
            }
            g_hoverRegistered = false;
        }
        if (g_tapRegistered)
        {
            try
            {
                box.Tapped(g_tapToken);
            }
            catch (...)
            {
            }
            g_tapRegistered = false;
        }
        throw;
    }
    g_prayerText = prayer;
    g_countdownText = countdown;
    g_iqamahText = iqamah;
    g_iqamahShadow = iqamahShadow;
    g_prayerShadow = prayerShadow;
    g_countdownShadow = countdownShadow;
    Wh_Log(L"[Mawaqit] Text controls attached, prayer=%d countdown=%d", g_prayerText ? 1 : 0,
           g_countdownText ? 1 : 0);
    g_parent = parent;
    if (g_position == L"before-icons" || g_position == L"after-icons")
    {
        auto grid = parent.try_as<Grid>();
        Panel iconsParent{nullptr};
        FrameworkElement repeater{nullptr};
        if (grid && FindApplicationRepeater(LiveTaskbarRoot(), iconsParent, repeater))
        {
            Wh_Log(L"[Mawaqit] Icon layout: requested=%s columns=%u repeaterColumn=%d "
                   L"repeaterSpan=%d boxColumn=%d boxSpan=%d parentW=%.1f repeaterW=%.1f boxW=%.1f",
                   g_position.c_str(), grid.ColumnDefinitions().Size(), Grid::GetColumn(repeater),
                   Grid::GetColumnSpan(repeater), Grid::GetColumn(box), Grid::GetColumnSpan(box),
                   grid.ActualWidth(), repeater.ActualWidth(), box.ActualWidth());
        }
    }
    UpdateDynamicCenterOnUiThread();
    UpdateAfterIconsOnUiThread();
    UpdateCountdown();
    UpdateSecondaryClocksOnUiThread();
    prayer.Text(g_showNext ? g_label : L"");
    countdown.Text(g_showCountdown ? PillCountdown() : L"");
    iqamah.Text(PillIqamah());
    if (iqamahShadow)
        iqamahShadow.Text(PillIqamah());
    if (prayerShadow)
        prayerShadow.Text(g_showNext ? g_label : L"");
    if (countdownShadow)
        countdownShadow.Text(g_showCountdown ? PillCountdown() : L"");
    Wh_Log(L"[Mawaqit] Native XAML position %s inserted: %d physical px, %.1f DIP, DPI %.0f",
           g_position.c_str(), g_width, widthDip, dpi);
}

static void LogException(PCWSTR context)
{
    try
    {
        throw;
    }
    catch (winrt::hresult_error const& e)
    {
        Wh_Log(L"[Mawaqit] %s failed HRESULT 0x%08X: %s", context, (unsigned)e.code().value,
               e.message().c_str());
    }
    catch (...)
    {
        Wh_Log(L"[Mawaqit] %s failed", context);
    }
}

static bool OnTaskbarThread(folder_menus_dispatch::ThreadProc proc)
{
    HWND wnd = taskwin::FindCurrentProcessTaskbarWnd();
    return wnd && folder_menus_dispatch::RunFromWindowThread(wnd, proc, nullptr,
                                                             L"Windhawk_Mawaqit_TaskbarDispatch");
}

// Explorer can inject the mod before creating its taskbar HWNDs. Bootstrap
// waits for ownership rather than permanently skipping workers at AfterInit.
static DWORD WINAPI ScheduleWorker(void*);
static DWORD WINAPI CountdownWorker(void*);
static DWORD WINAPI StartWorker(void*)
{
    bool workersStarted = false;
    // Keep the bootstrap bounded for file-manager Explorer processes.
    for (int i = 0; i < 40 && !g_stopping.load(); ++i)
    {
        if (!mawaqit_monitor_discovery::EnumerateTaskbars().empty())
        {
            if (!workersStarted)
            {
                workersStarted = true;
                mawaqit_monitor_discovery::LogInventory();
                Wh_Log(L"[Mawaqit] Taskbar ownership detected; starting workers");
                if (!g_stopping.load())
                {
                    g_updateWorker = CreateThread(nullptr, 0, ScheduleWorker, nullptr, 0, nullptr);
                    if (!g_updateWorker)
                        Wh_Log(L"[Mawaqit] Failed to start schedule worker: error=%lu",
                               GetLastError());
                    g_countdownWorker =
                        CreateThread(nullptr, 0, CountdownWorker, nullptr, 0, nullptr);
                    if (!g_countdownWorker)
                        Wh_Log(L"[Mawaqit] Failed to start countdown worker: error=%lu",
                               GetLastError());
                }
            }
            bool dispatched = OnTaskbarThread([](void*) { InsertOnUiThread(); });
            // Secondary-only configurations intentionally have no primary placeholder.
            if (dispatched && (g_placeholder.get() || !g_showPrimaryMonitor))
                return 0;
        }
        if (WaitForSingleObject(g_stopEvent, 1500) != WAIT_TIMEOUT)
            break;
    }
    return 0;
}
static HANDLE g_worker = nullptr;

// Runs only on the taskbar XAML thread. Re-resolve the live parent rather
// than trusting weak references to the previous taskbar visual tree.
static void EnsureTaskbarSlotOnUiThread()
{
    if (g_stopping.load())
        return;
    if (!g_showPrimaryMonitor)
    {
        UpdateSecondaryClocksOnUiThread();
        g_taskbarRebuildPending.store(false, std::memory_order_release);
        return;
    }
    int index = -1;
    auto parent = ResolvePositionParent(index);
    if (!parent)
        return; // New taskbar tree may still be initializing.
    // A matching name alone isn't sufficient: it might be a leftover slot
    // from a detached instance. Verify the tracked XAML object and parent.
    bool present = false;
    auto tracked = g_placeholder.get();
    auto trackedParent = g_parent.get();
    if (tracked && trackedParent == parent)
    {
        for (auto const& child : parent.Children())
            if (child.try_as<FrameworkElement>() == tracked)
            {
                present = true;
                break;
            }
    }
    if (present && g_showPrimaryMonitor)
    {
        UpdateSecondaryClocksOnUiThread();
        g_taskbarRebuildPending.store(false, std::memory_order_release);
        return;
    }
    Wh_Log(L"[Mawaqit] Taskbar slot missing after recreation; restoring");
    RemoveOnUiThread();
    InsertOnUiThread();
    // Keep checking if insertion was deferred because the tree is incomplete.
    g_taskbarRebuildPending.store(false, std::memory_order_release);
}

static void LoadModSettings();
static void ApplyPendingSettingsOnUiThread()
{
    if (!g_settingsPending.exchange(false, std::memory_order_acq_rel))
        return;
    try
    {
        LoadModSettings();
        // Removal destroys the popup immediately, so don't schedule a closing
        // animation or re-arm an auto-close timer for a soon-to-be-dead window.
        RemoveOnUiThread();
        InsertOnUiThread();
        Wh_Log(L"[Mawaqit] Pending settings applied on taskbar thread");
    }
    catch (...)
    {
        // Keep the request for the next countdown tick if rebuilding throws.
        g_settingsPending.store(true, std::memory_order_release);
        LogException(L"Applying pending settings");
    }
}

static void UpdateClockOnUiThread()
{
    if (g_stopping.load())
        return;
    ApplyPendingSettingsOnUiThread();
    // Check occasionally even without a StartTaskbar notification: some
    // Windows shell changes replace the XAML tree through other paths.
    static unsigned healthTick = 0;
    if (g_taskbarRebuildPending.exchange(false, std::memory_order_acq_rel) || ++healthTick >= 5)
    {
        healthTick = 0;
        EnsureTaskbarSlotOnUiThread();
    }
    UpdateCountdown();
    auto prayer = g_prayerText;
    auto countdown = g_countdownText;
    if (prayer)
        prayer.Text(g_showNext ? g_label : L"");
    if (countdown)
        countdown.Text(g_showCountdown ? PillCountdown() : L"");
    if (g_iqamahText)
        g_iqamahText.Text(PillIqamah());
    if (g_iqamahShadow)
        g_iqamahShadow.Text(PillIqamah());
    if (g_prayerShadow)
        g_prayerShadow.Text(g_showNext ? g_label : L"");
    if (g_countdownShadow)
        g_countdownShadow.Text(g_showCountdown ? PillCountdown() : L"");
    if (g_width == 0 && g_pill)
        g_pill.Width(EffectivePillWidth() + g_trayGap);
    UpdateDynamicCenterOnUiThread();
    UpdateAfterIconsOnUiThread();
    UpdateSecondaryClocksOnUiThread();
}

// XAML DispatcherTimer did not tick in the taskbar's hosting context on the
// tested Windows build. A Win32 waitable worker supplies the cadence; all
// TextBlock writes still execute on the owning taskbar/XAML thread.
static DWORD WINAPI CountdownWorker(void*)
{
    while (!g_stopping.load())
    {
        DWORD wait = WaitForSingleObject(g_stopEvent, 1000);
        if (wait != WAIT_TIMEOUT)
            break;
        if (g_stopping.load())
            break;
        if (!OnTaskbarThread([](void*) { UpdateClockOnUiThread(); }))
        {
            static unsigned failures = 0;
            if (++failures <= 3 || failures % 60 == 0)
                Wh_Log(L"[Mawaqit] Countdown UI dispatch failed (%u)", failures);
        }
    }
    return 0;
}

// Return remaining cache lifetime only when it belongs to the selected mosque,
// includes today's date, and both cache files are present.
static DWORD FreshScheduleCacheRemainingMs(const std::wstring& mosque, int hours)
{
    try
    {
        std::wstring dir = CacheDir();
        if (dir.empty())
            return 0;
        std::wstring slug;
        NativeUpdater::normalize(mosque, slug);
        std::ifstream metadata((dir + L"\\metadata.json").c_str(), std::ios::binary);
        if (!metadata)
            return 0;
        std::string json((std::istreambuf_iterator<char>(metadata)),
                         std::istreambuf_iterator<char>());
        if (json.size() > 65536)
            return 0;
        NativeUpdater::Parser parser{json};
        auto root = parser.value();
        if (root.key("mosqueIdentifier").s != NativeUpdater::narrow(slug))
            return 0;
        SYSTEMTIME now;
        GetLocalTime(&now);
        if (root.key("scheduleYear").s != std::to_string(now.wYear))
            return 0;
        std::wstring schedulePath = dir + L"\\schedule.txt";
        std::ifstream schedule(schedulePath.c_str(), std::ios::binary);
        if (!schedule)
            return 0;
        std::string header;
        if (!std::getline(schedule, header) ||
            header != "# Emberveil Mawaqit schedule v4")
            return 0;
        std::string today = NativeUpdater::date(now.wYear, now.wMonth, now.wDay) + "|";
        bool found = false;
        std::string line;
        while (std::getline(schedule, line))
            if (line.compare(0, today.size(), today) == 0)
            {
                found = true;
                break;
            }
        if (!found)
            return 0;
        WIN32_FILE_ATTRIBUTE_DATA attrs{};
        if (!GetFileAttributesExW((dir + L"\\metadata.json").c_str(), GetFileExInfoStandard,
                                  &attrs))
            return 0;
        FILETIME current;
        GetSystemTimeAsFileTime(&current);
        ULARGE_INTEGER updated{}, nowTicks{};
        updated.LowPart = attrs.ftLastWriteTime.dwLowDateTime;
        updated.HighPart = attrs.ftLastWriteTime.dwHighDateTime;
        nowTicks.LowPart = current.dwLowDateTime;
        nowTicks.HighPart = current.dwHighDateTime;
        if (nowTicks.QuadPart < updated.QuadPart)
            return 0;
        ULONGLONG elapsedMs = (nowTicks.QuadPart - updated.QuadPart) / 10000ULL;
        ULONGLONG lifetimeMs = (ULONGLONG)hours * 60 * 60 * 1000;
        return elapsedMs < lifetimeMs ? (DWORD)(lifetimeMs - elapsedMs) : 0;
    }
    catch (...)
    {
        return 0;
    }
}

// A schedule covers one calendar year. Never sleep past local New Year's
// midnight with the previous year's data, even when the refresh interval
// (or its remaining cached lifetime) would otherwise cross that boundary.
// A short December 31 check also accommodates clock adjustments and sleep.
static DWORD LimitRefreshDelayAtYearEnd(DWORD delay)
{
    SYSTEMTIME now{};
    GetLocalTime(&now);
    if (now.wMonth != 12 || now.wDay != 31)
        return delay;
    ULONGLONG secondsLeft =
        (23u - now.wHour) * 3600ULL + (59u - now.wMinute) * 60ULL + (60u - now.wSecond);
    // Wake just after midnight, allowing Windows to advance the local date.
    ULONGLONG untilNewYearMs = (secondsLeft + 2ULL) * 1000ULL;
    DWORD capped = (DWORD)std::min<ULONGLONG>(untilNewYearMs, 3600000ULL);
    return std::min(delay, capped);
}

static DWORD WINAPI ScheduleWorker(void*)
{
    std::wstring initialMosque;
    int initialHours;
    AcquireSRWLockShared(&g_settingsLock);
    initialMosque = g_mosque;
    initialHours = g_refreshHours;
    ReleaseSRWLockShared(&g_settingsLock);
    DWORD delay = FreshScheduleCacheRemainingMs(initialMosque, initialHours);
    if (delay)
        Wh_Log(L"[Mawaqit] Fresh cached schedule; next download in %lu ms", (unsigned long)delay);
    while (!g_stopping.load())
    {
        HANDLE events[] = {g_stopEvent, g_refreshEvent};
        DWORD result = WaitForMultipleObjects(2, events, FALSE, LimitRefreshDelayAtYearEnd(delay));
        if (result == WAIT_OBJECT_0 || result == WAIT_FAILED)
            return 0;
        // A capped wait may expire before midnight. Recheck the cache and
        // retain its remaining lifetime instead of downloading every hour.
        if (result == WAIT_TIMEOUT && delay > 3600000u)
        {
            SYSTEMTIME current{};
            GetLocalTime(&current);
            if (current.wMonth == 12 && current.wDay == 31)
            {
                std::wstring cachedMosque;
                int cachedHours;
                AcquireSRWLockShared(&g_settingsLock);
                cachedMosque = g_mosque;
                cachedHours = g_refreshHours;
                ReleaseSRWLockShared(&g_settingsLock);
                DWORD remaining = FreshScheduleCacheRemainingMs(cachedMosque, cachedHours);
                if (remaining)
                {
                    delay = remaining;
                    continue;
                }
            }
        }
        std::wstring mosque;
        int hours;
        AcquireSRWLockShared(&g_settingsLock);
        mosque = g_mosque;
        hours = g_refreshHours;
        const auto generation = g_scheduleConfigGeneration.load(std::memory_order_acquire);
        ReleaseSRWLockShared(&g_settingsLock);
        Wh_Log(L"[Mawaqit] Refresh started: mosque=%s interval=%d hours", mosque.c_str(), hours);
        bool ok = false;
        try
        {
            ok = NativeUpdater::Run(mosque, generation);
        }
        catch (const std::exception& e)
        {
            Wh_Log(L"[Mawaqit] Schedule update: %hs", e.what());
        }
        catch (...)
        {
            Wh_Log(L"[Mawaqit] Schedule update failed");
        }
        if (g_stopping.load())
            return 0;
        // The settings may change after cache commit, but before UI dispatch.
        // Skip obsolete UI work and immediately retry with the latest snapshot.
        const bool obsolete =
            g_scheduleConfigGeneration.load(std::memory_order_acquire) != generation;
        if (obsolete)
        {
            Wh_Log(L"[Mawaqit] Refresh configuration changed; retrying latest settings");
            delay = 0;
            continue;
        }
        if (ok)
        {
            Wh_Log(L"[Mawaqit] Schedule updated");
            OnTaskbarThread(
                [](void*)
                {
                    g_loadedDate.clear();
                    g_yesterday.clear();
                    g_today.clear();
                    g_tomorrow.clear();
                    LoadMetadata();
                    UpdateClockOnUiThread();
                });
        }
        delay = (DWORD)(ok ? hours : 1) * 60u * 60u * 1000u;
        Wh_Log(L"[Mawaqit] Refresh finished: success=%d next-delay=%lu ms", ok ? 1 : 0,
               (unsigned long)delay);
    }
    return 0;
}

static std::wstring ReadSetting(PCWSTR key)
{
    PCWSTR raw = Wh_GetStringSetting(key);
    std::wstring result = raw ? raw : L"";
    if (raw)
        Wh_FreeStringSetting(raw);
    return result;
}
static void LoadModSettings()
{
    std::wstring position = ReadSetting(L"General.Position");
    if (position != L"after-tray" && position != L"before-icons" && position != L"after-icons" &&
        position != L"center")
        position = L"before-tray";
    g_position = position;
    g_showPrimaryMonitor = Wh_GetIntSetting(L"Display.ShowPrimaryMonitor") != 0;
    g_showSecondaryMonitors = Wh_GetIntSetting(L"Display.ShowSecondaryMonitors") != 0;
    Wh_Log(L"[Mawaqit][Monitor] Visibility: primary=%d secondary=%d unified-position=%s",
           g_showPrimaryMonitor ? 1 : 0, g_showSecondaryMonitors ? 1 : 0, g_position.c_str());
    auto theme = ReadSetting(L"General.Theme");
    if (theme != L"traditional" && theme != L"catppuccin" && theme != L"emberveil" &&
        theme != L"emberveil-midnight" && theme != L"emberveil-daybreak" &&
        theme != L"emerald-mosque" && theme != L"dracula" &&
        theme != L"nord" && theme != L"gruvbox" && theme != L"tokyo-night" &&
        theme != L"solarized-dark" && theme != L"solarized-light" && theme != L"custom")
        theme = L"system";
    g_theme = theme;
    auto backgroundStyle = ReadSetting(L"Advanced.LayoutSettings.BackgroundStyle");
    if (backgroundStyle != L"transparent" && backgroundStyle != L"blur" &&
        backgroundStyle != L"acrylic")
        backgroundStyle = L"solid";
    g_backgroundStyle = backgroundStyle;
    g_pillTextShadow = Wh_GetIntSetting(L"Advanced.LayoutSettings.TextShadow") != 0;
    g_showPillBorder = Wh_GetIntSetting(L"Advanced.LayoutSettings.ShowPillBorder") != 0;
    g_backgroundOpacity = std::clamp(
        Wh_GetIntSetting(L"Advanced.LayoutSettings.BackgroundOpacity"), 0, 100);
    auto popupStyle = ReadSetting(L"Advanced.PopupAppearanceSettings.BackgroundStyle");
    if (popupStyle != L"solid" && popupStyle != L"transparent" &&
        popupStyle != L"blur" && popupStyle != L"acrylic")
        popupStyle = L"follow-pill";
    g_popupBackgroundStyle = popupStyle;
    g_popupBackgroundOpacity = std::clamp(
        Wh_GetIntSetting(L"Advanced.PopupAppearanceSettings.BackgroundOpacity"), -1, 100);
    g_popupShowBorder = Wh_GetIntSetting(L"Advanced.PopupAppearanceSettings.ShowBorder") != 0;
    g_popupShowSeparators = Wh_GetIntSetting(L"Advanced.PopupAppearanceSettings.ShowSeparators") != 0;

    int width = Wh_GetIntSetting(L"Advanced.LayoutSettings.Width");
    g_width = (width == 0 || (width >= 180 && width <= 400)) ? width : 0;
    int height = Wh_GetIntSetting(L"Advanced.LayoutSettings.PillHeight");
    g_pillHeight = (height >= 0 && height <= 100) ? height : 0;
    int gap = Wh_GetIntSetting(L"Advanced.LayoutSettings.TrayGap");
    g_trayGap = (gap >= 0 && gap <= 100) ? gap : 5;
    int radius = Wh_GetIntSetting(L"Advanced.LayoutSettings.CornerRadius");
    g_cornerRadius = (radius >= 0 && radius <= 30) ? radius : 13;
    int font = Wh_GetIntSetting(L"Advanced.LayoutSettings.FontSize");
    g_fontSize = (font >= 10 && font <= 24) ? font : 15;
    int iqamahScale = Wh_GetIntSetting(L"Advanced.LayoutSettings.IqamahFontScale");
    g_iqamahFontScale = std::clamp(iqamahScale, 50, 100);
    PCWSTR marginSetting = Wh_GetStringSetting(L"Advanced.LayoutSettings.Margins");
    if (marginSetting)
    {
        g_margins = ParseInsets(marginSetting, -100, 100);
        Wh_FreeStringSetting(marginSetting);
    }
    PCWSTR paddingSetting = Wh_GetStringSetting(L"Advanced.LayoutSettings.Padding");
    if (paddingSetting)
    {
        g_padding = ParseInsets(paddingSetting, 0, 100);
        Wh_FreeStringSetting(paddingSetting);
    }
    g_showNext = Wh_GetIntSetting(L"Display.ShowNextPrayer") != 0;
    g_showCountdown = Wh_GetIntSetting(L"Display.ShowCountdown") != 0;
    g_showIqamah = Wh_GetIntSetting(L"Display.ShowIqamahTime") != 0;
    g_showFajrWindowCountdown = Wh_GetIntSetting(L"Display.ShowFajrWindowCountdown") != 0;
    g_enablePopup = Wh_GetIntSetting(L"Advanced.PopupAppearanceSettings.EnablePopup") != 0;
    g_openPopupOnHover = Wh_GetIntSetting(L"Advanced.PopupAppearanceSettings.OpenPopupOnHover") != 0;
    g_hoverCloseDelayMs = std::clamp(Wh_GetIntSetting(L"Advanced.PopupAppearanceSettings.HoverCloseDelayMs"), 0, 5000);
    int autoClose = std::clamp(Wh_GetIntSetting(L"Advanced.PopupAppearanceSettings.PopupAutoCloseSeconds"), 0, 300);
    g_popupAutoCloseSeconds.store(autoClose);
    Wh_Log(L"[Mawaqit] Popup auto-close configured: %d seconds", autoClose);
    std::wstring mosque = ReadSetting(L"General.Mosque");
    if (mosque.empty())
        mosque = L"al-haram-makkah-saudi-arabia";
    int hours = Wh_GetIntSetting(L"Advanced.ModSettingsAdvanced.RefreshHours");
    if (hours < 1 || hours > 168)
        hours = 24;
    AcquireSRWLockExclusive(&g_settingsLock);
    bool changed = g_mosque != mosque || g_refreshHours != hours;
    g_mosque = mosque;
    g_refreshHours = hours;
    if (changed)
        g_scheduleConfigGeneration.fetch_add(1, std::memory_order_release);
    ReleaseSRWLockExclusive(&g_settingsLock);
    Wh_Log(L"[Mawaqit] Settings: next=%d countdown=%d popup=%d theme=%s mosque=%s refresh=%d h "
           L"changed=%d",
           g_showNext ? 1 : 0, g_showCountdown ? 1 : 0, g_enablePopup ? 1 : 0, g_theme.c_str(),
           mosque.c_str(), hours, changed ? 1 : 0);
    if (changed && g_refreshEvent)
    {
        Wh_Log(L"[Mawaqit] Requesting schedule refresh after settings change");
        SetEvent(g_refreshEvent);
    }
}

BOOL Wh_ModInit()
{
    g_stopping = false;
    LoadModSettings();
    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_refreshEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (!g_stopEvent || !g_refreshEvent)
    {
        if (g_refreshEvent)
        {
            CloseHandle(g_refreshEvent);
            g_refreshEvent = nullptr;
        }
        if (g_stopEvent)
        {
            CloseHandle(g_stopEvent);
            g_stopEvent = nullptr;
        }
        return FALSE;
    }
    folder_menus_dispatch::SetExceptionLogger(LogException);
    if (!taskxaml::HookTaskbarSymbols(
            []()
            {
                // Hook may fire while Explorer is constructing the new taskbar.
                // Defer all XAML access to the regular UI-thread countdown tick.
                g_taskbarRebuildPending.store(true, std::memory_order_release);
            }))
    {
        Wh_Log(L"[Mawaqit] Failed to hook taskbar symbols");
        CloseHandle(g_refreshEvent);
        g_refreshEvent = nullptr;
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
        return FALSE;
    }
    return TRUE;
}
void Wh_ModAfterInit()
{
    // Do not decide taskbar ownership from a single early snapshot: on Explorer
    // restart the shell windows may appear several seconds after injection.
    g_worker = CreateThread(nullptr, 0, StartWorker, nullptr, 0, nullptr);
    if (!g_worker)
        Wh_Log(L"[Mawaqit] Failed to start taskbar bootstrap worker: error=%lu", GetLastError());
}
void Wh_ModSettingsChanged()
{
    // Windhawk also loads into Explorer instances that don't own taskbars.
    // They have no workers to retry a UI dispatch and no taskbar UI to rebuild.
    if (mawaqit_monitor_discovery::EnumerateTaskbars().empty())
        return;
    Wh_Log(L"[Mawaqit] Settings change received; scheduling native slot rebuild");
    g_settingsPending.store(true, std::memory_order_release);
    bool dispatched = OnTaskbarThread([](void*) { ApplyPendingSettingsOnUiThread(); });
    if (!dispatched)
        Wh_Log(L"[Mawaqit] Settings dispatch unavailable; pending update will retry on countdown "
               L"tick");
    else
        Wh_Log(L"[Mawaqit] Settings rebuild dispatched successfully");
}
void Wh_ModUninit()
{
    g_stopping = true;
    if (g_stopEvent)
        SetEvent(g_stopEvent);
    if (g_refreshEvent)
        SetEvent(g_refreshEvent);
    if (g_worker)
    {
        WaitForSingleObject(g_worker, INFINITE);
        CloseHandle(g_worker);
        g_worker = nullptr;
    }
    if (g_updateWorker)
    {
        WaitForSingleObject(g_updateWorker, INFINITE);
        CloseHandle(g_updateWorker);
        g_updateWorker = nullptr;
    }
    if (g_countdownWorker)
    {
        WaitForSingleObject(g_countdownWorker, INFINITE);
        CloseHandle(g_countdownWorker);
        g_countdownWorker = nullptr;
    }
    if (HWND wnd = taskwin::FindCurrentProcessTaskbarWnd())
    {
        if (!folder_menus_dispatch::RunFromWindowThread(
                wnd, [](void*) { RemoveOnUiThread(); }, nullptr,
                L"Windhawk_Mawaqit_TaskbarDispatch", INFINITE))
            Wh_Log(L"[Mawaqit] Teardown could not reach taskbar thread");
    }
    if (g_refreshEvent)
    {
        CloseHandle(g_refreshEvent);
        g_refreshEvent = nullptr;
    }
    if (g_stopEvent)
    {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }
}
