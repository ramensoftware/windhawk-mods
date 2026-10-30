// ==WindhawkMod==
// @id              windhawk-mawaqit
// @name            Mawaqit Prayer Clock
// @description     Mawaqit prayer clock with live countdown, multi-monitor taskbar support, prayer schedule popup, and customizable themes.
// @version         0.8.0
// @author          HHeitlager
// @github          https://github.com/hheitlager
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lshell32 -luuid -lgdi32 -lgdiplus -lwinhttp
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
| **Back up your settings before upgrading to v0.8.0** |
| :--- |
| Copy your current configuration before upgrading. Version 0.8.0 introduces a new settings structure, so existing appearance, layout, popup, and custom-color settings will reset to the new defaults. Your **mosque identifier, selected theme, and taskbar position** are preserved. |

# Mawaqit Prayer Clock

## Bring your daily prayer times to the Windows 11 taskbar.
Mawaqit Prayer Clock displays the next prayer, its scheduled time, and a live countdown directly on your taskbar.
Click the clock to view today's full prayer schedule, using prayer times published by your selected mosque on [Mawaqit](https://mawaqit.net).

## Features
- **Prayer times at a glance** - See the next prayer and how much time remains.
- **Daily prayer schedule** - Click or hover over the clock to view today's prayer times and Iqamah delays.
- **Personalize your clock** - Choose from 26 predefined themes, customize colors, and adjust its appearance.
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
    $description: '(Default: al-haram-makkah-saudi-arabia)'
  - Theme: system
    $name: Theme
    $options:
    - system: Windows / System
    - monochrome: Monochrome
    - traditional: Traditional Islamic
    - catppuccin: Catppuccin Mocha
    - catppuccin-latte: Catppuccin Latte
    - emberveil: Emberveil
    - emberveil-midnight: Emberveil Midnight
    - emberveil-daybreak: Emberveil Daybreak
    - emerald-mosque: Emerald Mosque
    - dracula: Dracula
    - dracula-light: Dracula Light
    - nord: Nord
    - nord-light: Nord Light
    - gruvbox: Gruvbox Dark
    - gruvbox-light: Gruvbox Light
    - tokyo-night: Tokyo Night
    - tokyo-night-day: Tokyo Night Day
    - solarized-dark: Solarized Dark
    - solarized-light: Solarized Light
    - rose-pine: Rosé Pine
    - everforest: Everforest
    - synthwave-84: SynthWave '84
    - flexoki: Flexoki
    - kanagawa: Kanagawa
    - ayu: Ayu
    - one-dark: One Dark
    - custom: Custom
    $description: '(Default: Windows / System)'
  - AutomaticTheme: false
    $name: Automatic light/dark theme
    $description: 'Switch between matching light and dark variants automatically when available. (Default: Off)'
  - Position: before-tray
    $name: Taskbar position
    $description: 'Choose where the clock appears on the taskbar. Dynamic center moves aside for applications and hides when there is no room. (Default:
      Before system tray)'
    $options:
    - before-tray: Before system tray
    - after-tray: After system tray
    - before-start: Before Windows start
    - before-icons: Before taskbar icons
    - after-icons: After taskbar icons
    - center: Dynamic center (experimental)
  - TimeFormat: 24h
    $name: Time format
    $options:
    - 24h: 24-hour
    - 12h: 12-hour
    $description: 'Choose how scheduled prayer times appear in the pill and popup. Countdowns and Iqamah offsets are unaffected. (Default: 24-hour)'
  - ShowAmPm: true
    $name: Show AM/PM
    $description: 'Show AM or PM after clock times when using 12-hour format. (Default: On)'
- TaskbarPill:
  - Monitors:
    - ShowPrimaryMonitor: true
      $name: Show clock on primary monitor
      $description: 'Display the clock on your main taskbar. (Default: On)'
    - ShowSecondaryMonitors: false
      $name: Show clock on secondary monitors
      $description: 'Also show the clock on taskbars on your other monitors. (Default: Off)'
  - Layout:
    - Style: floating
      $name: Pill layout
      $options:
      - floating: Floating pill
      - attached: Attached to taskbar
      $description: 'Floating creates a separate rounded pill; Attached blends the clock into the taskbar. (Default: Floating pill)'
    - Width: 0
      $name: Pill width
      $description: 'Set to 0 to size the clock automatically for its text, font size and padding; or choose a fixed width of 190–400 px. (Default:
        0)'
    - Height: 0
      $name: Pill height
      $description: 'Set to 0 to adjust automatically to the taskbar, or choose a height between 1 and 100 px. (Default: 0)'
    - Margins: "0, 0, 0, 0"
      $name: Margin (left, top, right, bottom)
      $description: 'Move the clock left, right, up, or down (−100 to 100 px per side). (Default: 0, 0, 0, 0)'
    - Padding: "0, 0, 0, 0"
      $name: Padding (left, top, right, bottom)
      $description: 'Adjust the space between the clock edges and its text (0–100 px per side). (Default: 0, 0, 0, 0)'
  - Content:
    - ShowNextPrayer: name
      $name: Show next prayer
      $options:
      - none: None
      - name: Name
      - icon: Icon
      - both: Name and icon
      $description: 'Choose how to show the next prayer. None hides its name and icon; countdown and date controls remain independent. (Default: Name)'
    - NextPrayerIconStyle: prayer-period
      $name: Next prayer icon style
      $options:
      - prayer-period: Prayer period
      - mosque: Mosque
      $description: 'Choose a changing prayer-period symbol or a fixed mosque icon. Applies when Show next prayer includes an icon. (Default: Prayer period)'
    - ShowCountdown: true
      $name: Show countdown
      $description: 'Show the time remaining until the next prayer. (Default: On)'
    - ShowIqamahTime: true
      $name: Show Iqamah time
      $description: 'Show the mosque''s Iqamah delay (+N minutes) next to the countdown and each prayer in the popup. (Default: On)'
    - ShowFajrWindowCountdown: false
      $name: Show Shuruq and Imsak
      $description: 'Include Shuruq (sunrise) and, when provided by your mosque, Imsak in the popup schedule. After Fajr begins, the countdown
        shows the time remaining until Shuruq. Imsak is informational and does not start a prayer countdown. Disabled by default. (Default: Off)'
    - HijriDate: "off"
      $name: Hijri date in pill
      $options:
      - "off": Off
      - numeric: Numeric
      - full: Full
      $description: 'Show the Islamic date in the pill, as numbers or full text. (Default: Off)'
  - Typography:
    - FontWeight: normal
      $name: Font weight
      $description: 'Choose the text weight. (Default: Normal)'
      $options:
      - normal: Normal
      - medium: Medium
      - semibold: Semi-bold
      - bold: Bold
    - FontSize: 15
      $name: Font size
      $description: 'Adjust the clock''s text size (10–24 px). This does not affect the popup. (Default: 15)'
    - IqamahFontScale: 80
      $name: Iqamah font scale (%)
      $description: 'Size of the Iqamah suffix relative to the pill font (50–100%, default 80%). (Default: 80)'
    - TextShadow: false
      $name: Pill text shadow
      $description: 'Add a subtle dark shadow behind the pill text for readability on transparent or blurred taskbars. Does not affect the popup.
        (Default: Off)'
  - Appearance:
    - BackgroundStyle: solid
      $name: Background style
      $description: 'Choose the clock background material. Blur and Acrylic are experimental and depend on Windows composition support. (Default:
        Solid)'
      $options:
      - solid: Solid
      - transparent: Transparent
      - blur: Blur (experimental)
      - acrylic: Acrylic (experimental)
    - BackgroundOpacity: 100
      $name: Background opacity (%)
      $description: 'Pill background opacity from 0 (fully transparent) to 100 (opaque). Transparent keeps the pill transparent but uses a subtle
        acrylic-tinted popup for readability. Blur and Acrylic may interpret opacity differently. (Default: 100)'
    - CornerRadius: 13
      $name: Corner radius
      $description: 'Choose how rounded the clock''s corners appear (0–30 px). Set to 0 for square corners; higher values create rounder corners.
        (Default: 13)'
    - ShowBorder: false
      $name: Show pill border
      $description: 'Draw a border using the theme countdown accent (or the custom border color). Attached layout shows only the left and right edges. (Default: Off)'
    - BorderOpacity: 20
      $name: Border opacity (%)
      $description: 'Set the pill border opacity from 0 (invisible) to 100 (opaque). Applies to theme and custom border colors in both layouts when Show pill border is enabled. (Default: 20)'
  $name: Taskbar pill
- Popup:
  - Behavior:
    - Enabled: true
      $name: Show popup
      $description: 'Show the daily prayer schedule when you interact with the clock. (Default: On)'
    - OpenOnHover: true
      $name: Open popup when hovering over clock
      $description: 'Open without clicking. The popup closes after the cursor leaves both clock and popup. (Default: On)'
    - AutoCloseSeconds: 5
      $name: Popup auto-close (seconds, 0 = disabled)
      $description: 'Close after the pointer stays outside the pill and popup for 1–300 seconds. Set to 0 to disable. Hover-opened popups may close sooner using the hover close delay. (Default: 5)'
    - HoverOpenDelayMs: 500
      $name: Hover open delay (ms)
      $description: 'Wait before opening the popup while hovering (0–5000 ms). Moving away or pressing a mouse button cancels the pending open. (Default: 500)'
    - HoverCloseDelayMs: 500
      $name: Hover close delay (ms)
      $description: 'Delay after leaving the clock and popup (0–5000 ms). (Default: 500)'
  - Layout:
    - Placement: floating
      $name: Popup placement
      $options:
      - floating: Floating
      - attached: Attached to pill
      $description: 'Floating shows a separate popup; Attached visually connects it to the pill. (Default: Floating)'
    - AttachedCornerStyle: adaptive
      $name: Attached corner style
      $options:
      - adaptive: Adaptive
      - rounded: Rounded
      - square: Square
      $description: 'When attached, Adaptive adjusts the shared corners; Rounded keeps them curved; Square makes them straight. (Default: Adaptive)'
    - ScalePercent: 100
      $name: Popup scale (%)
      $description: 'Scale the entire popup, including its text, icons and spacing. 100% uses the normal size. (Default: 100)'
  - Content:
    - ShowPrayerIcons: true
      $name: Show prayer icons
      $description: 'Show an icon beside each prayer in the schedule. (Default: On)'
    - HighlightNextPrayer: true
      $name: Highlight next prayer
      $description: 'Use the theme accent to make the upcoming prayer easier to spot. (Default: On)'
    - ShowNextPrayerBorder: true
      $name: Border around next prayer
      $description: 'Draw a border around the upcoming prayer row. (Default: On)'
    - HighlightOpacity: 20
      $name: Highlight opacity (%)
      $description: 'Control how strong the upcoming prayer highlight appears (0–100%). (Default: 20)'
    - BoldNextPrayer: true
      $name: Bold next prayer
      $description: 'Make the upcoming prayer stand out with bold text. (Default: On)'
    - HijriDate: full
      $name: Hijri date in popup
      $options:
      - "off": Off
      - numeric: Numeric
      - full: Full
      $description: 'Show the Islamic date in the popup, as numbers or full text. (Default: Full)'
  - Appearance:
    - BackgroundStyle: follow-pill
      $name: Popup background material
      $description: 'Choose the popup material independently of the taskbar pill. Follow pill preserves the previous behavior. (Default: Follow
        pill)'
      $options:
      - follow-pill: Follow pill
      - solid: Solid
      - transparent: Transparent
      - blur: Blur (experimental)
      - acrylic: Acrylic (experimental)
    - BackgroundOpacity: -1
      $name: Background opacity (%)
      $description: '-1 follows the pill opacity (legacy behavior); 0–100 sets the popup independently. For Transparent, Blur and Acrylic this
        controls the backdrop tint strength. (Default: -1)'
    - ShowBorder: true
      $name: Show popup border
      $description: 'Draw an outline around the popup. (Default: On)'
    - ShowSeparators: true
      $name: Show popup separators
      $description: 'Add subtle dividing lines between sections of the popup. (Default: On)'
  - Typography:
    - FontSize: 15
      $name: Font size
      $description: 'Set the popup base text size (10–24 px). Larger text expands the popup layout. Popup scale still zooms the entire popup. (Default: 15)'
    - FontWeight: normal
      $name: Font weight
      $description: 'Choose the text weight. (Default: Normal)'
      $options:
      - normal: Normal
      - medium: Medium
      - semibold: Semi-bold
      - bold: Bold
    - TextShadow: false
      $name: Popup text shadow
      $description: 'Add a subtle shadow to popup text for extra contrast. (Default: Off)'
- ThemeColorOverrides:
  - Pill:
    - Background: ''
      $name: Pill background
      $description: 'Leave empty to use the theme background. (Default: Empty)'
    - Border: ''
      $name: Pill border
      $description: 'Leave empty to use the countdown accent. (Default: Empty)'
    - Icon: ''
      $name: Pill icon
      $description: 'Leave empty to use the resolved prayer text color. (Default: Empty)'
    - Text: ''
      $name: Pill prayer text
      $description: 'Leave empty to use the theme text color. (Default: Empty)'
    - Countdown: ''
      $name: Pill countdown
      $description: 'Leave empty to use the theme countdown accent. (Default: Empty)'
    - Iqamah: ''
      $name: Pill Iqamah
      $description: 'Leave empty to use the theme secondary accent when available, otherwise the countdown color. (Default: Empty)'
    - HijriDate: ''
      $name: Pill Hijri date
      $description: 'Leave empty to use the resolved prayer text color. (Default: Empty)'
    - Separator: ''
      $name: Pill separator
      $description: 'Leave empty to use the theme separator color. (Default: Empty)'
  - Popup:
    - Background: ''
      $name: Popup background
      $description: 'Leave empty to use the theme background. (Default: Empty)'
    - Border: ''
      $name: Popup border
      $description: 'Leave empty to use the theme border color. (Default: Empty)'
    - MosqueName: ''
      $name: Mosque name
      $description: 'Leave empty to use the resolved popup main text color. (Default: Empty)'
    - HijriDate: ''
      $name: Popup Hijri date
      $description: 'Leave empty to use the resolved popup secondary text color. (Default: Empty)'
    - Icon: ''
      $name: Prayer icons
      $description: 'Leave empty to use each prayer row text color, including the highlighted row. (Default: Empty)'
    - Text: ''
      $name: Popup main text
      $description: 'Leave empty to use the theme text color. (Default: Empty)'
    - Muted: ''
      $name: Popup secondary text
      $description: 'Leave empty to use the theme secondary text color. (Default: Empty)'
    - Iqamah: ''
      $name: Popup Iqamah
      $description: 'Leave empty to use the resolved popup secondary text color. (Default: Empty)'
    - Rule: ''
      $name: Popup dividers
      $description: 'Leave empty to use the theme divider color. (Default: Empty)'
    - NextPrayerBackground: ''
      $name: Next prayer highlight background
      $description: 'Leave empty to use the existing theme highlight accent. (Default: Empty)'
    - NextPrayerBorder: ''
      $name: Next prayer highlight border
      $description: 'Leave empty to use the resolved next prayer highlight background color. (Default: Empty)'
    - NextPrayerText: ''
      $name: Next prayer highlight text
      $description: 'Leave empty to use the existing theme highlighted row text color; icons follow unless separately overridden. (Default: Empty)'
    - Footer: ''
      $name: Popup footer
      $description: 'Leave empty to use the theme footer color. (Default: Empty)'
  $name: Theme color overrides
  $description: 'Accepts #RRGGBB or RRGGBB (for example, #FFBC5E or FFBC5E). Leave a field empty to inherit its existing color.'
- Advanced:
  - RefreshHours: 168
    $name: Refresh interval (hours)
    $description: 'Choose how often to update the prayer schedule from Mawaqit (1–720 hours). Examples: 24 = daily, 168 = weekly, 720 = every 30 days. (Default: 168)'
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <gdiplus.h>
#include <dwmapi.h>
#undef GetCurrentTime
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Markup.h>
#include <winrt/Windows.UI.Xaml.Shapes.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.h>
#include <winrt/Windows.UI.Text.h>
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

inline Kind Classify(FrameworkElement const &parent)
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
inline std::wstring ClassName(FrameworkElement const &element)
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
inline std::wstring DescribeChildren(Panel const &parent)
{
    if (!parent)
        return L"(none)";
    std::wstring names;
    try
    {
        for (auto const &child : parent.Children())
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

inline FrameworkElement FindDirectChild(Panel const &parent, wchar_t const *name)
{
    if (!parent || !name)
        return nullptr;
    for (auto const &child : parent.Children())
    {
        auto element = child.try_as<FrameworkElement>();
        if (element && element.Name() == name)
            return element;
    }
    return nullptr;
}

inline int IndexOfChild(Panel const &parent, FrameworkElement const &child)
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

inline bool ResolveSlot(Panel const &parent, Anchor anchor, int &slot)
{
    Kind kind = Classify(parent);
    if (kind == Kind::Unsupported)
        return false;

    if (anchor == Anchor::BeforeIcons)
    {
        slot = 0;
        return true;
    }

    wchar_t const *referenceName = nullptr;
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
        slot = Grid::GetColumn(reference) + (after ? std::max(1, Grid::GetColumnSpan(reference)) : 0);
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

inline bool Release(Panel const &parent, Lease &lease);

inline bool AcquireAt(Panel const &parent, int slot, std::wstring const &markerName, Lease &lease)
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
        int staleSlot = kind == Kind::Columns ? Grid::GetColumn(stale) : IndexOfChild(parent, stale);
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

        for (auto const &child : grid.Children())
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
inline bool FindMarker(Panel const &parent, Lease const &lease, uint32_t &index)
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
inline bool PlaceChild(Panel const &parent, Lease const &lease, FrameworkElement const &content)
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

inline bool Release(Panel const &parent, Lease &lease)
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

    for (auto const &child : grid.Children())
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
        [](HWND hwnd, LPARAM context) -> BOOL {
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
            reinterpret_cast<std::vector<TaskbarRecord> *>(context)->push_back(std::move(entry));
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&result));
    return result;
}

inline void LogInventory()
{
    auto taskbars = EnumerateTaskbars();
    Wh_Log(L"[Mawaqit][Monitor] Taskbar discovery: count=%u", (unsigned)taskbars.size());
    for (auto const &bar : taskbars)
        Wh_Log(L"[Mawaqit][Monitor] hwnd=%p class=%s monitor=%s primaryMonitor=%d uiThread=%lu", bar.hwnd,
               bar.primaryTaskbar ? L"Shell_TrayWnd" : L"Shell_SecondaryTrayWnd",
               bar.device.empty() ? L"(unresolved)" : bar.device.c_str(), bar.primaryMonitor ? 1 : 0,
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
        [](HWND window, LPARAM parameter) -> BOOL {
            DWORD processId = 0;
            WCHAR className[32];
            if (GetWindowThreadProcessId(window, &processId) && processId == GetCurrentProcessId() &&
                GetClassName(window, className, ARRAYSIZE(className)) && _wcsicmp(className, L"Shell_TrayWnd") == 0)
            {
                *reinterpret_cast<HWND *>(parameter) = window;
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

using ThreadProc = void (*)(void *);
using ExceptionLogFn = void (*)(PCWSTR context);

inline ExceptionLogFn g_logException = nullptr;

// Point this at the mod's logger once in Wh_ModInit so failures inside a UI
// callback are reported in the mod's own voice.
inline void SetExceptionLogger(ExceptionLogFn logger)
{
    g_logException = logger;
}

inline bool Invoke(ThreadProc proc, void *parameter)
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
    void *parameter;
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
inline bool RunFromWindowThread(HWND window, ThreadProc proc, void *parameter, PCWSTR messageName,
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
        [](int code, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (code == HC_ACTION)
            {
                auto const *call = reinterpret_cast<CWPSTRUCT const *>(lParam);
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
        delivered = SendMessageTimeoutW(window, message, 0, static_cast<LPARAM>(id), SMTO_ABORTIFHUNG | SMTO_BLOCK,
                                        timeoutMs, &ignored);
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

using CTaskBand_GetTaskbarHost_t = void *(WINAPI *)(void *, void *);
using TaskbarHost_FrameHeight_t = int(WINAPI *)(void *);
using Ref_count_base_Decref_t = void(WINAPI *)(void *);
using TrayUI_StartTaskbar_t = void(WINAPI *)(void *);

inline CTaskBand_GetTaskbarHost_t CTaskBand_GetTaskbarHost_Original = nullptr;
inline TaskbarHost_FrameHeight_t TaskbarHost_FrameHeight_Original = nullptr;
inline Ref_count_base_Decref_t Ref_count_base_Decref_Original = nullptr;
inline TrayUI_StartTaskbar_t TrayUI_StartTaskbar_Original = nullptr;
inline void *CTaskBand_ITaskListWndSite_vftable = nullptr;
inline void *CSecondaryTaskBand_ITaskListWndSite_vftable = nullptr;
inline CTaskBand_GetTaskbarHost_t CSecondaryTaskBand_GetTaskbarHost_Original = nullptr;

// The mod's rebuild callback, invoked after Explorer rebuilds the taskbar.
inline void (*g_onTaskbarRebuilt)() = nullptr;

inline void WINAPI TrayUI_StartTaskbar_Hook(void *self)
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
        {{LR"(const CTaskBand::`vftable'{for `ITaskListWndSite'})"}, &CTaskBand_ITaskListWndSite_vftable},
        {{LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const )"},
         &CTaskBand_GetTaskbarHost_Original},
        {{LR"(public: int __cdecl TaskbarHost::FrameHeight(void)const )"}, &TaskbarHost_FrameHeight_Original},
        {{LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))"}, &Ref_count_base_Decref_Original},
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
    if (!CSecondaryTaskBand_ITaskListWndSite_vftable || !CSecondaryTaskBand_GetTaskbarHost_Original)
        Wh_Log(L"[Mawaqit] Secondary taskbar symbols unavailable; primary clock remains supported");
    return true;
}

// The FrameworkElement lives at an offset inside TaskbarHost that MOVES
// between Windows builds, so it is read out of TaskbarHost::FrameHeight's
// prologue at runtime rather than hardcoded.
inline size_t FrameworkElementOffset()
{
    size_t offset = 0x10;
#if defined(_M_X64)
    BYTE const *code = reinterpret_cast<BYTE const *>(TaskbarHost_FrameHeight_Original);
    if (code[0] == 0x48 && code[1] == 0x83 && code[2] == 0xEC && code[4] == 0x48 && code[5] == 0x83 &&
        code[6] == 0xC1 && code[7] <= 0x7F)
    {
        offset = code[7];
    }
#elif defined(_M_ARM64)
    DWORD const *code = reinterpret_cast<DWORD const *>(TaskbarHost_FrameHeight_Original);
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

inline XamlRoot GetTaskbarXamlRoot(HWND taskbarWnd)
{
    if (!CTaskBand_GetTaskbarHost_Original || !TaskbarHost_FrameHeight_Original || !Ref_count_base_Decref_Original ||
        !CTaskBand_ITaskListWndSite_vftable)
    {
        return nullptr;
    }

    HWND taskSwWnd = (HWND)GetProp(taskbarWnd, L"TaskbandHWND");
    if (!taskSwWnd)
    {
        return nullptr;
    }
    void *taskBand = (void *)GetWindowLongPtr(taskSwWnd, 0);
    if (!taskBand)
    {
        return nullptr;
    }

    void *site = taskBand;
    for (int i = 0; *(void **)site != CTaskBand_ITaskListWndSite_vftable; ++i)
    {
        if (i == 20)
        {
            return nullptr;
        }
        site = (void **)site + 1;
    }

    void *host[2]{};
    CTaskBand_GetTaskbarHost_Original(site, host);
    if (!host[0] || !host[1])
    {
        if (host[1])
            Ref_count_base_Decref_Original(host[1]);
        return nullptr;
    }

    auto *unknown = *(IUnknown **)((BYTE *)host[0] + FrameworkElementOffset());
    if (!unknown)
    {
        Ref_count_base_Decref_Original(host[1]);
        return nullptr;
    }
    FrameworkElement element = nullptr;
    unknown->QueryInterface(winrt::guid_of<FrameworkElement>(), winrt::put_abi(element));
    auto result = element ? element.XamlRoot() : nullptr;
    Ref_count_base_Decref_Original(host[1]);
    return result;
}

// Secondary taskbars use WorkerW and CSecondaryTaskBand, not TaskbandHWND.
// Mirrors the established Windhawk multirow taskbar resolution path.
inline XamlRoot GetSecondaryTaskbarXamlRoot(HWND taskbarWnd)
{
    if (!CSecondaryTaskBand_ITaskListWndSite_vftable || !CSecondaryTaskBand_GetTaskbarHost_Original ||
        !TaskbarHost_FrameHeight_Original || !Ref_count_base_Decref_Original)
    {
        return nullptr;
    }
    HWND worker = FindWindowExW(taskbarWnd, nullptr, L"WorkerW", nullptr);
    if (!worker)
        return nullptr;
    void *taskBand = (void *)GetWindowLongPtrW(worker, 0);
    if (!taskBand)
        return nullptr;
    void *site = taskBand;
    int offset = 0;
    for (; offset < 20; ++offset)
    {
        if (*(void **)site == CSecondaryTaskBand_ITaskListWndSite_vftable)
            break;
        site = (void **)site + 1;
    }
    if (offset == 20)
        return nullptr;
    void *host[2]{};
    CSecondaryTaskBand_GetTaskbarHost_Original(site, host);
    if (!host[0] || !host[1])
    {
        if (host[1])
            Ref_count_base_Decref_Original(host[1]);
        return nullptr;
    }
    auto *unknown = *(IUnknown **)((BYTE *)host[0] + FrameworkElementOffset());
    FrameworkElement element = nullptr;
    if (unknown)
        unknown->QueryInterface(winrt::guid_of<FrameworkElement>(), winrt::put_abi(element));
    auto root = element ? element.XamlRoot() : nullptr;
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

static void fail(const char *s)
{
    throw std::runtime_error(s);
}
static std::string narrow(const std::wstring &s)
{
    if (s.empty())
        return {};
    int n = WideCharToMultiByte(CP_UTF8, 0, s.data(), (int)s.size(), nullptr, 0, nullptr, nullptr);
    std::string r(n, 0);
    WideCharToMultiByte(CP_UTF8, 0, s.data(), (int)s.size(), r.data(), n, nullptr, nullptr);
    return r;
}
static std::string quote(const std::string &s)
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
    const J &key(const std::string &k) const
    {
        auto it = o.find(k);
        if (it == o.end())
            fail("Missing JSON key");
        return it->second;
    }
    const J &idx(size_t i) const
    {
        if (i >= a.size())
            fail("JSON array index missing");
        return a[i];
    }
};

struct Parser
{
    const std::string &s;
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
    static void utf8(std::string &out, unsigned cp)
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
            case 'u': {
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
            while (p < s.size() && s[p] != ',' && s[p] != ']' && s[p] != '}' && s[p] != ' ' && s[p] != '\r' &&
                   s[p] != '\n')
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
    Http(const Http &) = delete;
    Http &operator=(const Http &) = delete;
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
// Normal refresh: every RefreshHours hours (default: 168 / weekly).
// Failed download: retry after approximately one hour.
// Mosque or refresh interval changed: request a fresh schedule.
// Cache format upgrade to v4: download a fresh schedule once.
//
// Network requests use WinHTTP over HTTPS with redirects disabled.
// The maximum accepted response size is 8 MB.
static std::string fetch(const std::wstring &path)
{
    Http session(WinHttpOpen(L"EmberveilMawaqit/0.5", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME,
                             WINHTTP_NO_PROXY_BYPASS, 0));
    WinHttpSetTimeouts(session.h, 5000, 5000, 15000, 15000);
    Http conn(WinHttpConnect(session.h, L"mawaqit.net", INTERNET_DEFAULT_HTTPS_PORT, 0));
    Http req(WinHttpOpenRequest(conn.h, L"GET", path.c_str(), nullptr, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES,
                                WINHTTP_FLAG_SECURE));
    DWORD redirects = WINHTTP_OPTION_REDIRECT_POLICY_NEVER;
    WinHttpSetOption(req.h, WINHTTP_OPTION_REDIRECT_POLICY, &redirects, sizeof(redirects));
    if (!WinHttpSendRequest(req.h, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0) ||
        !WinHttpReceiveResponse(req.h, nullptr))
        fail("HTTP request failed");
    DWORD status = 0, len = sizeof(status);
    if (!WinHttpQueryHeaders(req.h, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER, WINHTTP_HEADER_NAME_BY_INDEX,
                             &status, &len, WINHTTP_NO_HEADER_INDEX) ||
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

static std::string extract(const std::string &html)
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
static bool validTime(const std::string &t)
{
    return t.size() == 5 && t[2] == ':' && t[0] >= '0' && t[0] <= '2' && t[1] >= '0' && t[1] <= '9' && t[3] >= '0' &&
           t[3] <= '5' && t[4] >= '0' && t[4] <= '9' && ((t[0] - '0') * 10 + t[1] - '0') < 24;
}
static void writeFile(const std::wstring &path, const std::string &content)
{
    HANDLE h = CreateFileW(path.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h == INVALID_HANDLE_VALUE)
        fail("Cannot create temporary cache file");
    DWORD written = 0;
    BOOL ok = WriteFile(h, content.data(), (DWORD)content.size(), &written, nullptr);
    FlushFileBuffers(h);
    CloseHandle(h);
    if (!ok || written != content.size())
        fail("Cache write failed");
}
static void replace(const std::wstring &from, const std::wstring &to)
{
    if (!MoveFileExW(from.c_str(), to.c_str(), MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH))
        fail("Cache replace failed");
}

static std::wstring normalize(std::wstring input, std::wstring &slug)
{
    // Accept public and admin page links, but always download from the public host.
    for (const wchar_t *prefix : {L"https://mawaqit.net/", L"https://admin.mawaqit.net/"})
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
    auto valid = [](wchar_t c) {
        return (c >= L'a' && c <= L'z') || (c >= L'A' && c <= L'Z') || (c >= L'0' && c <= L'9') || c == L'_' ||
               c == L'-';
    };
    if (lang.size() != 2 || !std::all_of(lang.begin(), lang.end(), [](wchar_t c) { return c >= L'a' && c <= L'z'; }) ||
        slug.empty() || slug.size() > 120 || !std::all_of(slug.begin(), slug.end(), valid))
        fail("Invalid mosque slug or language");
    return L"/" + lang + L"/" + slug;
}

static bool Run(const std::wstring &mosque, unsigned long long expectedGeneration)
{
    std::wstring slug;
    std::wstring path = normalize(mosque, slug);
    std::string html = fetch(path);
    std::string json = extract(html);
    Parser parser{json};
    J root = parser.value();
    const J &cal = root.key("calendar");
    if (cal.kind != J::Arr || cal.a.size() != 12)
        fail("Calendar must have 12 months");
    // Iqamah is optional. Respect the mosque's explicit enable flag.
    const auto enabledIt = root.o.find("iqamaEnabled");
    const bool iqamaEnabled = enabledIt != root.o.end() && enabledIt->second.s == "true";
    const auto iqamaIt = root.o.find("iqamaCalendar");
    const J *iqamaCal =
        iqamaEnabled && iqamaIt != root.o.end() && iqamaIt->second.kind == J::Arr && iqamaIt->second.a.size() == 12
            ? &iqamaIt->second
            : nullptr;
    const J &nameVal = root.key("name");
    const J &tzVal = root.key("timezone");
    if (tzVal.kind != J::Str || tzVal.s.empty())
        fail("Missing timezone");
    std::string name = nameVal.kind == J::Str && !nameVal.s.empty() ? nameVal.s : narrow(slug);
    // Use the local Windows date for the cached schedule year.
    SYSTEMTIME now;
    GetLocalTime(&now);
    int year = now.wYear;
    std::ostringstream sched;
    sched << "# Emberveil Mawaqit schedule v4\n# Source: https://mawaqit.net" << narrow(path) << "\n# Year: " << year
          << "\n";
    for (int m = 1; m <= 12; m++)
    {
        const J &month = cal.idx(m - 1);
        if (month.kind != J::Obj)
            fail("Month is not an object");
        for (int d = 1; d <= days(year, m); d++)
        {
            const J &entry = month.key(std::to_string(d));
            if (entry.kind != J::Arr || (entry.a.size() != 6 && entry.a.size() != 7))
                fail("Invalid calendar day");
            // Seven-entry calendars include Imsak before Fajr and Shuruq.
            // Six-entry calendars begin with Fajr and Shuruq.
            const bool hasImsak = entry.a.size() == 7;
            sched << date(year, m, d);
            for (int i : {0, 2, 3, 4, 5})
            {
                const J &t = entry.idx(i + (hasImsak ? 1 : 0));
                if (t.kind != J::Str || !validTime(t.s))
                    fail("Invalid prayer time");
                sched << '|' << t.s;
            }
            // Five optional Iqamah values follow the five prayer times.
            // Preserve +N or HH:mm; invalid/missing values become empty fields.
            const J *iqamaDay = nullptr;
            if (iqamaCal)
            {
                const J &iqamaMonth = iqamaCal->idx(m - 1);
                if (iqamaMonth.kind == J::Obj)
                {
                    auto it = iqamaMonth.o.find(std::to_string(d));
                    if (it != iqamaMonth.o.end() && it->second.kind == J::Arr && it->second.a.size() == 5)
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
                const J &value = iqamaDay->idx(i);
                if (value.kind != J::Str)
                {
                    sched << '-';
                    continue;
                }
                const std::string &raw = value.s;
                bool relative = raw.size() >= 2 && raw.size() <= 4 && raw[0] == '+' &&
                                std::all_of(raw.begin() + 1, raw.end(), [](char c) { return c >= '0' && c <= '9'; });
                if ((relative && std::stoi(raw.substr(1)) <= 180) || validTime(raw))
                    sched << raw;
                else
                    sched << '-';
            }
            // Shuruq follows Fajr; seven-entry calendars have an Imsak prefix.
            const J &sunrise = entry.idx(hasImsak ? 2 : 1);
            if (sunrise.kind != J::Str || !validTime(sunrise.s))
                fail("Invalid Shuruq time");
            sched << '|' << sunrise.s;
            // Append optional Imsak after the existing v3 fields.
            sched << '|';
            if (hasImsak)
            {
                const J &imsak = entry.idx(0);
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
    // Mawaqit publishes the mosque's moon-sighting adjustment in confData.
    // Cache it independently from the annual prayer schedule.
    int hijriAdjustment = 0;
    const J &adjustment = root.key("hijriAdjustment");
    if (adjustment.kind == J::Other || adjustment.kind == J::Str)
    {
        try
        {
            size_t consumed = 0;
            int parsed = std::stoi(adjustment.s, &consumed);
            if (consumed == adjustment.s.size() && parsed >= -5 && parsed <= 5)
                hijriAdjustment = parsed;
        }
        catch (...)
        {
        }
    }
    std::wstring old = dir + L"\\metadata.json";
    {
        HANDLE h = CreateFileW(old.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING,
                               FILE_ATTRIBUTE_NORMAL, nullptr);
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
    snprintf(stamp, sizeof(stamp), "%04u-%02u-%02uT%02u:%02u:%02uZ", utc.wYear, utc.wMonth, utc.wDay, utc.wHour,
             utc.wMinute, utc.wSecond);
    std::ostringstream meta;
    meta << "{\n  \"schemaVersion\": 1,\n  \"mosqueName\": " << quote(name)
         << ",\n  \"mosqueIdentifier\": " << quote(narrow(slug))
         << ",\n  \"mosqueUrl\": " << quote("https://mawaqit.net" + narrow(path))
         << ",\n  \"timezone\": " << quote(tzVal.s) << ",\n  \"scheduleYear\": " << year
         << ",\n  \"localDate\": " << quote(date(now.wYear, now.wMonth, now.wDay))
         << ",\n  \"hijriAdjustment\": " << quote(std::to_string(hijriAdjustment))
         << ",\n  \"hijriDate\": " << quote(hijri) << ",\n  \"hijriDateSource\": " << quote(hijriSource)
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

static std::wstring g_mosqueName = L"Mosque", g_hijriDate = L"", g_hijriNumeric = L"";
static std::wstring g_hijriMode = L"off";
static std::wstring g_popupHijriMode = L"full";
static std::wstring PopupHijriDate()
{
    return g_popupHijriMode == L"full" ? g_hijriDate : g_popupHijriMode == L"numeric" ? g_hijriNumeric : L"";
}
static std::wstring PillHijriDate()
{
    const std::wstring &date = g_hijriMode == L"full" ? g_hijriDate : g_hijriNumeric;
    return g_hijriMode != L"off" && !date.empty() ? L"• " + date : L"";
}
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
static int ParseIqamahDelay(const std::wstring &raw, int prayerMinute)
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
    if (raw.size() == 5 && raw[2] == L':' && raw[0] >= L'0' && raw[0] <= L'2' && raw[1] >= L'0' && raw[1] <= L'9' &&
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
static std::wstring IqamahSuffix(const Prayer &prayer)
{
    return g_showIqamah && prayer.iqamahDelay >= 0 ? L"+" + std::to_wstring(prayer.iqamahDelay) : L"";
}
static const wchar_t *kNames[] = {L"Fajr", L"Dhuhr", L"Asr", L"Maghrib", L"Isha"};
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

static std::wstring DateKey(const SYSTEMTIME &t)
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

static std::wstring Utf8ToWide(const std::string &s)
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

static std::wstring JsonField(const std::wstring &json, const wchar_t *key)
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
    g_hijriNumeric = L"";
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
    // Recalculate from the current local day, rather than retaining a stale
    // one-day snapshot in metadata.json across a week-long schedule cache.
    // The arithmetic Islamic calendar is shifted by the mosque's Mawaqit
    // hijriAdjustment. Moon-sighting changes are picked up on the next fetch.
    SYSTEMTIME today{};
    GetLocalTime(&today);
    const std::wstring adjustmentText = JsonField(json, L"hijriAdjustment");
    int adjustment = 0;
    if (!adjustmentText.empty())
    {
        wchar_t *end = nullptr;
        long value = wcstol(adjustmentText.c_str(), &end, 10);
        if (end != adjustmentText.c_str() && *end == 0 && value >= -5 && value <= 5)
            adjustment = (int)value;
    }
    const int a = (14 - today.wMonth) / 12;
    const int y = today.wYear + 4800 - a;
    const int m = today.wMonth + 12 * a - 3;
    const int julianDay = today.wDay + (153 * m + 2) / 5 + 365 * y + y / 4 - y / 100 + y / 400 - 32045 + adjustment;
    const int hijriYear = (30 * (julianDay - 1948439) + 10646) / 10631;
    auto monthStart = [hijriYear](int month) {
        return 1 + (59 * (month - 1) + 1) / 2 + (hijriYear - 1) * 354 + (3 + 11 * hijriYear) / 30 + 1948439;
    };
    int hijriMonth = 1;
    while (hijriMonth < 12 && julianDay >= monthStart(hijriMonth + 1))
        ++hijriMonth;
    const int hijriDay = julianDay - monthStart(hijriMonth) + 1;
    if (hijriDay >= 1 && hijriDay <= 30)
    {
        static const wchar_t *months[] = {
            L"Muharram", L"Safar",   L"Rabi' al-Awwal", L"Rabi' al-Thani", L"Jumada al-Ula", L"Jumada al-Akhirah",
            L"Rajab",    L"Sha'ban", L"Ramadan",        L"Shawwal",        L"Dhu al-Qi'dah", L"Dhu al-Hijjah"};
        wchar_t full[120], numeric[32];
        swprintf(full, 120, L"%d %ls %d AH", hijriDay, months[hijriMonth - 1], hijriYear);
        swprintf(numeric, 32, L"%02d-%02d-%04d", hijriDay, hijriMonth, hijriYear);
        g_hijriDate = full;
        g_hijriNumeric = numeric;
        Wh_Log(L"[Mawaqit][Hijri] date=%ls adjustment=%d source=arithmetic+mosque-adjustment", numeric, adjustment);
    }
}

static bool ParseDay(const std::wstring &line, const std::wstring &date, std::vector<Prayer> &out)
{
    if (line.size() < date.size() + 1 || line.compare(0, date.size(), date) != 0 || line[date.size()] != L'|')
        return false;
    std::wistringstream s(line.substr(date.size() + 1));
    std::wstring item;
    std::vector<Prayer> result;
    for (int i = 0; i < 5; i++)
    {
        if (!std::getline(s, item, L'|') || item.size() != 5 || item[2] != L':' || item[0] < L'0' || item[0] > L'2' ||
            item[1] < L'0' || item[1] > L'9' || item[3] < L'0' || item[3] > L'5' || item[4] < L'0' || item[4] > L'9')
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

static int ParseShuruq(const std::wstring &line)
{
    // v3: date | five adhan times | five Iqamah fields | Shuruq.
    std::vector<std::wstring> fields;
    std::wistringstream stream(line);
    std::wstring part;
    while (std::getline(stream, part, L'|'))
        fields.push_back(part);
    if (fields.size() != 12 && fields.size() != 13)
        return -1;
    const std::wstring &time = fields[11];
    if (time.size() != 5 || time[2] != L':' || time[0] < L'0' || time[0] > L'2' || time[1] < L'0' || time[1] > L'9' ||
        time[3] < L'0' || time[3] > L'5' || time[4] < L'0' || time[4] > L'9')
        return -1;
    int hour = (time[0] - L'0') * 10 + (time[1] - L'0');
    return hour <= 23 ? hour * 60 + (time[3] - L'0') * 10 + time[4] - L'0' : -1;
}

static int ParseImsak(const std::wstring &line)
{
    // date | five adhan | five Iqamah | Shuruq | optional Imsak.
    std::vector<std::wstring> fields;
    std::wistringstream stream(line);
    std::wstring part;
    while (std::getline(stream, part, L'|'))
        fields.push_back(part);
    if (fields.size() != 13)
        return -1;
    const std::wstring &time = fields[12];
    if (time.size() != 5 || time[2] != L':' || time[0] < L'0' || time[0] > L'2' || time[1] < L'0' || time[1] > L'9' ||
        time[3] < L'0' || time[3] > L'5' || time[4] < L'0' || time[4] > L'9')
        return -1;
    int hour = (time[0] - L'0') * 10 + time[1] - L'0';
    return hour <= 23 ? hour * 60 + (time[3] - L'0') * 10 + time[4] - L'0' : -1;
}

static void LoadSchedule(const SYSTEMTIME &now)
{
    std::wstring today = DateKey(now);
    if (today == g_loadedDate)
        return;
    LoadMetadata();
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

// Presentation only: schedule comparisons continue to use minutes since midnight.
static bool g_use12HourTime = false;
static bool g_showAmPm = true;
static std::wstring FormatClockTime(int minute)
{
    const int dayMinute = ((minute % 1440) + 1440) % 1440;
    const int hour = dayMinute / 60;
    wchar_t text[16];
    if (!g_use12HourTime)
        swprintf(text, 16, L"%02d:%02d", hour, dayMinute % 60);
    else
        swprintf(text, 16, L"%d:%02d%ls", hour % 12 == 0 ? 12 : hour % 12, dayMinute % 60,
                 g_showAmPm ? (hour < 12 ? L" AM" : L" PM") : L"");
    return text;
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
    if (g_showFajrWindowCountdown && g_shuruqMinute >= 0 && !g_today.empty() && nowSec >= g_today.front().minute * 60 &&
        nowSec < g_shuruqMinute * 60)
    {
        int sec = g_shuruqMinute * 60 - nowSec;
        wchar_t label[80], count[32];
        swprintf(label, 80, L"Fajr ends at %ls", FormatClockTime(g_shuruqMinute).c_str());
        swprintf(count, 32, L"-%d:%02d:%02d", sec / 3600, (sec / 60) % 60, sec % 60);
        g_label = label;
        g_countdown = count;
        return;
    }
    // After midnight, yesterday's Isha may still be awaiting its scheduled Iqamah.
    // Use only the cached source schedule; never synthesize a missing Iqamah.
    if (g_yesterday.size() == 5)
    {
        const auto &isha = g_yesterday.back();
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
                swprintf(label, 80, L"%ls  %ls", isha.name.c_str(), FormatClockTime(iqamahMinute).c_str());
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
        const auto &p = g_today[i];
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
            swprintf(label, 80, L"%ls  %ls", p.name.c_str(), FormatClockTime(iqamahMinute).c_str());
            g_label = label;
            g_countdown = L"--:--";
            return;
        }
    }
    for (const auto &p : g_today)
    {
        int sec = p.minute * 60 - nowSec;
        if (sec > 0)
        {
            wchar_t label[80], count[32];
            swprintf(label, 80, L"%ls  %ls", p.name.c_str(), FormatClockTime(p.minute).c_str());
            swprintf(count, 32, L"-%d:%02d:%02d", sec / 3600, (sec / 60) % 60, sec % 60);
            g_label = label;
            g_countdown = count;
            g_iqamahSuffix = IqamahSuffix(p);
            return;
        }
    }
    if (!g_tomorrow.empty())
    {
        const auto &p = g_tomorrow.front();
        int sec = 86400 - nowSec + p.minute * 60;
        wchar_t label[80], count[32];
        swprintf(label, 80, L"%ls  %ls", p.name.c_str(), FormatClockTime(p.minute).c_str());
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
static Insets ParseInsets(const std::wstring &value, int minVal, int maxVal)
{
    Insets result{};
    int values[4]{};
    if (swscanf_s(value.c_str(), L" %d , %d , %d , %d ", &values[0], &values[1], &values[2], &values[3]) == 4)
    {
        for (int &v : values)
            v = std::clamp(v, minVal, maxVal);
        result = {values[0], values[1], values[2], values[3]};
    }
    return result;
}
static bool g_attachedPill = false;
static int g_width = 0, g_pillHeight = 0, g_cornerRadius = 13, g_fontSize = 15;
static int g_iqamahFontScale = 80;
static int g_pillFontWeight = FW_NORMAL, g_popupFontWeight = FW_NORMAL;
static int g_popupFontSize = 15;
static Insets g_margins{}, g_padding{};
static std::wstring g_theme = L"system";
static bool g_dynamicLightDarkMode = false;
static std::wstring g_backgroundStyle = L"solid";
static int g_backgroundOpacity = 100;
static bool g_pillTextShadow = false;
static bool g_showPillBorder = false;
static int g_pillBorderOpacity = 20;
// Popup options are independent of the XAML pill. -1 inherits legacy opacity.
static std::wstring g_popupBackgroundStyle = L"follow-pill";
static int g_popupBackgroundOpacity = -1;
static bool g_popupShowBorder = true, g_popupShowSeparators = true;
static bool g_popupTextShadow = false;
static bool g_popupShowNextPrayerAccent = false;
static bool g_popupShowNextPrayerBorder = false;
static bool g_popupBoldNextPrayer = false;
static bool g_popupShowPrayerIcons = false;
static int g_popupNextPrayerAccentOpacity = 20;
static int g_popupScale = 100;
static std::wstring g_popupLayout = L"floating";
static std::wstring g_popupAttachedCorners = L"adaptive";
static bool g_popupAttachedAtTop = false; // true when popup meets top-mounted taskbar

// Kept as persisted advanced settings so they can be hidden from the UI later.
static const std::wstring &PopupMaterial()
{
    return g_popupBackgroundStyle == L"follow-pill" ? g_backgroundStyle : g_popupBackgroundStyle;
}
static int PopupOpacity()
{
    return g_popupBackgroundOpacity < 0 ? (PopupMaterial() == L"transparent" ? 0 : g_backgroundOpacity)
                                        : g_popupBackgroundOpacity;
}
static std::wstring g_position = L"before-tray";
static bool g_showPrimaryMonitor = true, g_showSecondaryMonitors = true;
// Six semantic source colors for the experimental SynthWave palette.
// Existing pill/popup structures and all other themes remain unchanged.
struct SynthWaveColors
{
    unsigned background, surface, primary, secondary, text, muted;
};
static constexpr SynthWaveColors kSynthWave{0x0E0B1F, 0x1A1038, 0xFF2D9C, 0x00E5FF, 0xF8F8FF, 0xB9A6FF};
// Optional six-role palette accents for established theme families.
// Legacy pill and popup palettes remain authoritative for all existing roles.
// No new settings, and Windows Default/Custom/Emberveil families are untouched.
struct ExtendedThemeColors
{
    unsigned background, surface, primary, secondary, text, muted;
};
static bool TryExtendedThemeColors(const std::wstring &theme, ExtendedThemeColors &out)
{
    if (theme == L"catppuccin")
    {
        out = {0x211E32, 0x302D47, 0xCFA1FF, 0x89DCEB, 0xCDD6F4, 0xA6ADC8};
        return true;
    }
    if (theme == L"catppuccin-latte")
    {
        out = {0xEFF1F5, 0xE6E9EF, 0x8839EF, 0x179299, 0x4C4F69, 0x6C6F85};
        return true;
    }
    if (theme == L"dracula-light")
    {
        out = {0xF8F7FC, 0xEAE7F2, 0x7545B5, 0xB72F79, 0x292537, 0x655D77};
        return true;
    }
    if (theme == L"nord-light")
    {
        out = {0xECEFF4, 0xE5E9F0, 0x3B668B, 0x5E81AC, 0x2E3440, 0x4C566A};
        return true;
    }
    if (theme == L"gruvbox-light")
    {
        out = {0xFBF1C7, 0xEBDBB2, 0x9D0006, 0xB57614, 0x3C3836, 0x665C54};
        return true;
    }
    if (theme == L"tokyo-night-day")
    {
        out = {0xE1E2E7, 0xD5D6DB, 0x2959AA, 0x7847BD, 0x343B58, 0x61709A};
        return true;
    }
    if (theme == L"dracula")
    {
        out = {0x282A36, 0x44475A, 0xBD93F9, 0xFF79C6, 0xF8F8F2, 0xBFBFD0};
        return true;
    }
    if (theme == L"nord")
    {
        out = {0x2E3440, 0x4C566A, 0x88C0D0, 0xA3BE8C, 0xECEFF4, 0xD8DEE9};
        return true;
    }
    if (theme == L"gruvbox")
    {
        out = {0x282828, 0x504945, 0xFABD2F, 0xFE8019, 0xEBDBB2, 0xA89984};
        return true;
    }
    if (theme == L"tokyo-night")
    {
        out = {0x1A1B26, 0x414868, 0x7AA2F7, 0xBB9AF7, 0xC0CAF5, 0xA9B1D6};
        return true;
    }
    if (theme == L"rose-pine")
    {
        out = {0x191724, 0x403B50, 0xEB6F92, 0xF6C177, 0xE0DEF4, 0x908CAA};
        return true;
    }
    if (theme == L"everforest")
    {
        out = {0x2D353B, 0x475258, 0xA7C080, 0xDBBC7F, 0xD3C6AA, 0x9DA9A0};
        return true;
    }
    if (theme == L"solarized-dark")
    {
        out = {0x002B36, 0x586E75, 0xB58900, 0x2AA198, 0x93A1A1, 0x839496};
        return true;
    }
    if (theme == L"solarized-light")
    {
        out = {0xFDF6E3, 0xEEE8D5, 0x268BD2, 0xCB4B16, 0x586E75, 0x657B83};
        return true;
    }
    if (theme == L"flexoki")
    {
        out = {0xFFFCF0, 0xDAD8CE, 0x205EA6, 0xBC5215, 0x100F0F, 0x6F6E69};
        return true;
    }
    if (theme == L"kanagawa")
    {
        out = {0x1F1F28, 0x54546D, 0x98BB6C, 0xE6C384, 0xDCD7BA, 0xC8C093};
        return true;
    }
    if (theme == L"ayu")
    {
        out = {0x0B0F14, 0x39424C, 0xFFB454, 0x59C2FF, 0xE6E1CF, 0xA6A39B};
        return true;
    }
    if (theme == L"one-dark")
    {
        out = {0x282C34, 0x4B5263, 0x61AFEF, 0x98C379, 0xABB2BF, 0x9DA5B4};
        return true;
    }
    return false;
}
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
            material.TintOpacity(g_backgroundStyle == L"blur" ? 0.12 * opacity : 0.45 * opacity);
            material.Opacity(opacity);
            material.FallbackColor(color);
            Brush assigned = material;
            auto actual = assigned.try_as<AcrylicBrush>();
            Wh_Log(L"[Mawaqit][PillMaterial] requested=%s opacity=%d "
                   L"acrylicBrush=%d source=%s tintOpacity=%.3f "
                   L"brushOpacity=%.3f fallback=#%02X%02X%02X",
                   g_backgroundStyle.c_str(), g_backgroundOpacity, actual ? 1 : 0,
                   source == AcrylicBackgroundSource::Backdrop ? L"Backdrop" : L"HostBackdrop", material.TintOpacity(),
                   material.Opacity(), (unsigned)color.R, (unsigned)color.G, (unsigned)color.B);
            return assigned;
        }
        catch (winrt::hresult_error const &e)
        {
            Wh_Log(L"[Mawaqit][PillMaterial] %s acrylic failed HRESULT=0x%08X: %s; "
                   L"using solid fallback",
                   g_backgroundStyle.c_str(), (unsigned)e.code().value, e.message().c_str());
        }
        catch (...)
        {
            Wh_Log(L"[Mawaqit][PillMaterial] %s acrylic failed (unknown exception); "
                   L"using solid fallback",
                   g_backgroundStyle.c_str());
        }
    }
    SolidColorBrush brush;
    brush.Color(color);
    brush.Opacity(opacity);
    return brush;
}

// Border opacity is independent of the background and foreground controls.
static SolidColorBrush CreatePillBorderBrush(winrt::Windows::UI::Color color)
{
    SolidColorBrush brush(color);
    brush.Opacity((double)g_pillBorderOpacity / 100.0);
    return brush;
}

static winrt::Windows::UI::Color Rgb(unsigned c)
{
    return winrt::Windows::UI::Color{255, (uint8_t)(c >> 16), (uint8_t)(c >> 8), (uint8_t)c};
}
// Accept six RGB hex digits, with or without a leading '#'. Surrounding
// whitespace is ignored for colors copied from code, websites, or text files.
static bool TryParseThemeHex(const std::wstring &input, unsigned &rgb)
{
    const size_t first = input.find_first_not_of(L" \t\r\n");
    if (first == std::wstring::npos)
        return false;
    const size_t last = input.find_last_not_of(L" \t\r\n");
    const size_t start = first + (input[first] == L'#' ? 1 : 0);
    if (last + 1 - start != 6)
        return false;
    unsigned value = 0;
    for (size_t i = start; i <= last; ++i)
    {
        wchar_t c = input[i];
        unsigned digit = (c >= L'0' && c <= L'9')   ? (unsigned)(c - L'0')
                         : (c >= L'a' && c <= L'f') ? (unsigned)(c - L'a' + 10)
                         : (c >= L'A' && c <= L'F') ? (unsigned)(c - L'A' + 10)
                                                    : 16;
        if (digit > 15)
            return false;
        value = (value << 4) | digit;
    }
    rgb = value;
    return true;
}
static COLORREF ParseThemeHex(const std::wstring &input, COLORREF fallback)
{
    unsigned rgb;
    return TryParseThemeHex(input, rgb) ? RGB((rgb >> 16) & 255, (rgb >> 8) & 255, rgb & 255) : fallback;
}
static std::wstring ReadSetting(PCWSTR key);
static unsigned ReadCustomHex(PCWSTR key, unsigned fallback)
{
    COLORREF c = ParseThemeHex(ReadSetting(key), RGB((fallback >> 16) & 255, (fallback >> 8) & 255, fallback & 255));
    return (unsigned)GetRValue(c) << 16 | (unsigned)GetGValue(c) << 8 | GetBValue(c);
}
// Empty or invalid overrides inherit the selected theme. All colors are RGB hex.
static bool TryThemeOverride(PCWSTR key, unsigned &rgb)
{
    return TryParseThemeHex(ReadSetting(key), rgb);
}
static winrt::Windows::UI::Color OverridePillColor(PCWSTR key, winrt::Windows::UI::Color base)
{
    unsigned rgb;
    return TryThemeOverride(key, rgb) ? Rgb(rgb) : base;
}
static COLORREF OverridePopupColor(PCWSTR key, COLORREF base)
{
    unsigned rgb = 0;
    return TryThemeOverride(key, rgb) ? RGB((rgb >> 16) & 255, (rgb >> 8) & 255, rgb & 255) : base;
}
static bool WindowsLightMode()
{
    HKEY key = nullptr;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", 0,
                      KEY_QUERY_VALUE, &key) != ERROR_SUCCESS)
        return false;
    DWORD value = 0, bytes = sizeof(value);
    LONG result =
        RegQueryValueExW(key, L"AppsUseLightTheme", nullptr, nullptr, reinterpret_cast<BYTE *>(&value), &bytes);
    RegCloseKey(key);
    return result == ERROR_SUCCESS && value != 0;
}
// System appearance is the taskbar mode, independent of Windows app mode.
static bool WindowsSystemLightMode()
{
    HKEY key = nullptr;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", 0,
                      KEY_QUERY_VALUE, &key) != ERROR_SUCCESS)
        return false;
    DWORD value = 0, bytes = sizeof(value), type = 0;
    LONG result =
        RegQueryValueExW(key, L"SystemUsesLightTheme", nullptr, &type, reinterpret_cast<BYTE *>(&value), &bytes);
    RegCloseKey(key);
    return result == ERROR_SUCCESS && type == REG_DWORD && value != 0;
}
// AccentColor is stored as a Windows COLORREF-style BGR value in a DWORD:
// e.g. 0xFFD47800 means RGB #0078D4, not #D47800.
static COLORREF WindowsAccentColor()
{
    struct Candidate
    {
        PCWSTR path;
        PCWSTR name;
    };
    static const Candidate candidates[] = {
        {L"Software\\Microsoft\\Windows\\DWM", L"AccentColor"},
        {L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Accent", L"AccentColorMenu"},
    };
    for (const auto &candidate : candidates)
    {
        HKEY key = nullptr;
        if (RegOpenKeyExW(HKEY_CURRENT_USER, candidate.path, 0, KEY_QUERY_VALUE, &key) != ERROR_SUCCESS)
            continue;
        DWORD value = 0, type = 0, bytes = sizeof(value);
        LONG status = RegQueryValueExW(key, candidate.name, nullptr, &type, reinterpret_cast<BYTE *>(&value), &bytes);
        RegCloseKey(key);
        if (status == ERROR_SUCCESS && type == REG_DWORD && bytes == sizeof(value))
            return RGB(value & 0xFF, (value >> 8) & 0xFF, (value >> 16) & 0xFF);
    }
    return GetSysColor(COLOR_HIGHLIGHT);
}
// Resolve appearance without changing the user's selected theme setting.
// Only established light/dark pairs switch; all other presets stay fixed.
// Emberveil Midnight is intentionally exempt, even in Windows Light mode.
static const std::wstring &EffectiveTheme()
{
    if (!g_dynamicLightDarkMode)
        return g_theme;
    const bool light = WindowsSystemLightMode();
    if (g_theme == L"monochrome")
    {
        static const std::wstring dark = L"monochrome", day = L"monochrome-light";
        return light ? day : dark;
    }
    if (g_theme == L"catppuccin" || g_theme == L"catppuccin-latte")
    {
        static const std::wstring dark = L"catppuccin", day = L"catppuccin-latte";
        return light ? day : dark;
    }
    if (g_theme == L"emberveil" || g_theme == L"emberveil-daybreak")
    {
        static const std::wstring dark = L"emberveil", day = L"emberveil-daybreak";
        return light ? day : dark;
    }
    if (g_theme == L"dracula" || g_theme == L"dracula-light")
    {
        static const std::wstring dark = L"dracula", day = L"dracula-light";
        return light ? day : dark;
    }
    if (g_theme == L"nord" || g_theme == L"nord-light")
    {
        static const std::wstring dark = L"nord", day = L"nord-light";
        return light ? day : dark;
    }
    if (g_theme == L"gruvbox" || g_theme == L"gruvbox-light")
    {
        static const std::wstring dark = L"gruvbox", day = L"gruvbox-light";
        return light ? day : dark;
    }
    if (g_theme == L"tokyo-night" || g_theme == L"tokyo-night-day")
    {
        static const std::wstring dark = L"tokyo-night", day = L"tokyo-night-day";
        return light ? day : dark;
    }
    if (g_theme == L"solarized-dark" || g_theme == L"solarized-light")
    {
        static const std::wstring dark = L"solarized-dark", day = L"solarized-light";
        return light ? day : dark;
    }
    return g_theme;
}

static PillPalette ResolvePillPaletteBase(const std::wstring &theme)
{
    if (theme == L"monochrome")
        return {Rgb(0x202020), Rgb(0xF4F4F4), Rgb(0xD0D0D0), Rgb(0xB8B8B8)};
    if (theme == L"monochrome-light")
        return {Rgb(0xF2F2F2), Rgb(0x202020), Rgb(0x404040), Rgb(0x666666)};
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
    if (theme == L"catppuccin-latte")
        return {Rgb(0xEFF1F5), Rgb(0x4C4F69), Rgb(0x8839EF), Rgb(0x6C6F85)};
    if (theme == L"dracula-light")
        return {Rgb(0xF8F7FC), Rgb(0x292537), Rgb(0x7545B5), Rgb(0x655D77)};
    if (theme == L"nord-light")
        return {Rgb(0xECEFF4), Rgb(0x2E3440), Rgb(0x3B668B), Rgb(0x4C566A)};
    if (theme == L"gruvbox-light")
        return {Rgb(0xFBF1C7), Rgb(0x3C3836), Rgb(0x9D0006), Rgb(0x665C54)};
    if (theme == L"tokyo-night-day")
        return {Rgb(0xE1E2E7), Rgb(0x343B58), Rgb(0x2959AA), Rgb(0x61709A)};
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
    if (theme == L"rose-pine")
        return {Rgb(0x191724), Rgb(0xE0DEF4), Rgb(0xEB6F92), Rgb(0x908CAA)};
    if (theme == L"everforest")
        return {Rgb(0x2D353B), Rgb(0xD3C6AA), Rgb(0xA7C080), Rgb(0x9DA9A0)};
    if (theme == L"synthwave-84")
        return {Rgb(kSynthWave.background), Rgb(kSynthWave.text), Rgb(kSynthWave.secondary), Rgb(kSynthWave.muted)};
    if (theme == L"flexoki")
        return {Rgb(0xFFFCF0), Rgb(0x100F0F), Rgb(0x205EA6), Rgb(0x6F6E69)};
    if (theme == L"kanagawa")
        return {Rgb(0x1F1F28), Rgb(0xDCD7BA), Rgb(0x98BB6C), Rgb(0xC8C093)};
    if (theme == L"ayu")
        return {Rgb(0x0B0F14), Rgb(0xE6E1CF), Rgb(0xFFB454), Rgb(0x8A9199)};
    if (theme == L"one-dark")
        return {Rgb(0x282C34), Rgb(0xABB2BF), Rgb(0x61AFEF), Rgb(0x7F848E)};
    if (theme == L"custom")
        return {Rgb(ReadCustomHex(L"ThemeColorOverrides.Pill.Background", 0x211E32)),
                Rgb(ReadCustomHex(L"ThemeColorOverrides.Pill.Text", 0xF5E6C8)),
                Rgb(ReadCustomHex(L"ThemeColorOverrides.Pill.Countdown", 0xFFBC5E)),
                Rgb(ReadCustomHex(L"ThemeColorOverrides.Pill.Separator", 0xB8A98A))};
    COLORREF c = WindowsAccentColor();
    auto accent = winrt::Windows::UI::Color{255, GetRValue(c), GetGValue(c), GetBValue(c)};
    // Windows/System Default: accent-colored pill with a contrasting foreground.
    // Preserve the previous transparent-pill treatment when no background is drawn.
    if (g_backgroundStyle == L"transparent" || g_backgroundOpacity == 0)
    {
        if (WindowsSystemLightMode())
            return {Rgb(0xF2F2F2), Rgb(0x202020), accent, Rgb(0x666666)};
        return {Rgb(0x292929), Rgb(0xF4F4F4), accent, Rgb(0xB8B8B8)};
    }
    // The accent-filled Windows/System pill deliberately uses white text.
    // Users choosing a very light accent can adjust the text in custom overrides.
    return {accent, Rgb(0xFFFFFF), Rgb(0xFFFFFF), Rgb(0xFFFFFF)};
}
static PillPalette ResolvePillPalette(const std::wstring &theme)
{
    auto palette = ResolvePillPaletteBase(theme);
    unsigned rgb = 0;
    if (TryThemeOverride(L"ThemeColorOverrides.Pill.Background", rgb))
        palette.background = Rgb(rgb);
    if (TryThemeOverride(L"ThemeColorOverrides.Pill.Text", rgb))
        palette.foreground = Rgb(rgb);
    if (TryThemeOverride(L"ThemeColorOverrides.Pill.Countdown", rgb))
        palette.countdown = Rgb(rgb);
    if (TryThemeOverride(L"ThemeColorOverrides.Pill.Separator", rgb))
        palette.separator = Rgb(rgb);
    return palette;
}
static bool g_showNext = true, g_enablePopup = true;
static std::wstring g_nextPrayerDisplay = L"name";
static std::wstring g_nextPrayerIconStyle = L"prayer-period";
static std::wstring g_primaryRenderedPrayerIcon;
static std::atomic<int> g_popupAutoCloseSeconds{5};
static ULONGLONG g_popupAutoCloseDeadline = 0;
static bool g_openPopupOnHover = false;
static int g_hoverOpenDelayMs = 500;
static int g_hoverCloseDelayMs = 500;
// Accessed only from the taskbar UI thread (the popup is created on that thread).
static bool g_hoverPopupActive = false;
static ULONGLONG g_hoverOutsideSince = 0;
static std::wstring g_mosque = L"al-haram-makkah-saudi-arabia";
static int g_refreshHours = 168;
static HANDLE g_stopEvent = nullptr, g_refreshEvent = nullptr;
static HANDLE g_updateWorker = nullptr;
static HANDLE g_countdownWorker = nullptr;
static void UpdateClockOnUiThread();
[[clang::no_destroy]] static TextBlock g_prayerText{nullptr};
[[clang::no_destroy]] static Viewbox g_prayerIcon{nullptr};
[[clang::no_destroy]] static TextBlock g_countdownText{nullptr};
[[clang::no_destroy]] static TextBlock g_iqamahText{nullptr};
[[clang::no_destroy]] static TextBlock g_hijriText{nullptr};
[[clang::no_destroy]] static TextBlock g_iqamahShadow{nullptr};
[[clang::no_destroy]] static TextBlock g_prayerShadow{nullptr};
[[clang::no_destroy]] static TextBlock g_countdownShadow{nullptr};
[[clang::no_destroy]] static Grid g_pill{nullptr};
static HWND g_popup = nullptr;
static winrt::event_token g_tapToken{};
static bool g_tapRegistered = false;
static winrt::event_token g_hoverToken{};
static bool g_hoverRegistered = false;
static winrt::event_token g_hoverExitToken{}, g_hoverPressToken{};
static bool g_hoverExitRegistered = false, g_hoverPressRegistered = false;

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
    if (EffectiveTheme() == L"monochrome")
        return {PopupRgb(0x202020), PopupRgb(0xD0D0D0), PopupRgb(0xF4F4F4),
                PopupRgb(0xB8B8B8), PopupRgb(0x505050), PopupRgb(0xD0D0D0)};
    if (EffectiveTheme() == L"monochrome-light")
        return {PopupRgb(0xF2F2F2), PopupRgb(0x404040), PopupRgb(0x202020),
                PopupRgb(0x666666), PopupRgb(0xD0D0D0), PopupRgb(0x404040)};
    if (EffectiveTheme() == L"custom")
        return {PopupRgb(ReadCustomHex(L"ThemeColorOverrides.Popup.Background", 0x1D1D2D)),
                PopupRgb(ReadCustomHex(L"ThemeColorOverrides.Popup.Border", 0x3B3B58)),
                PopupRgb(ReadCustomHex(L"ThemeColorOverrides.Popup.Text", 0xD3D0E8)),
                PopupRgb(ReadCustomHex(L"ThemeColorOverrides.Popup.Muted", 0xB8B3D1)),
                PopupRgb(ReadCustomHex(L"ThemeColorOverrides.Popup.Rule", 0x424158)),
                PopupRgb(ReadCustomHex(L"ThemeColorOverrides.Popup.Footer", 0xA29DBB))};
    if (EffectiveTheme() == L"traditional")
        return {PopupRgb(0x102F29), PopupRgb(0x527361), PopupRgb(0xF5EAD3),
                PopupRgb(0xC9B789), PopupRgb(0x527361), PopupRgb(0xE4C784)};
    if (EffectiveTheme() == L"catppuccin")
        return {PopupRgb(0x302D47), PopupRgb(0x58536F), PopupRgb(0xCDD6F4),
                PopupRgb(0xA6ADC8), PopupRgb(0x58536F), PopupRgb(0xB4BEFE)};
    if (EffectiveTheme() == L"emberveil")
        return {PopupRgb(0x171B20), PopupRgb(0x212832), PopupRgb(0xF5E6C8),
                PopupRgb(0xF5E6C8), PopupRgb(0x212832), PopupRgb(0xD9A441)};
    if (EffectiveTheme() == L"emberveil-midnight")
        return {PopupRgb(0x101418), PopupRgb(0x353A3E), PopupRgb(0xF5E6C8),
                PopupRgb(0xB8A98A), PopupRgb(0x353A3E), PopupRgb(0xFFBC5E)};
    if (EffectiveTheme() == L"emberveil-daybreak")
        return {PopupRgb(0xF5E6C8), PopupRgb(0xE2CFA8), PopupRgb(0x1A1F26),
                PopupRgb(0x6F624F), PopupRgb(0xE2CFA8), PopupRgb(0xB87924)};
    if (EffectiveTheme() == L"emerald-mosque")
        return {PopupRgb(0x102B27), PopupRgb(0x386057), PopupRgb(0xF3E8CF),
                PopupRgb(0xB6C9BD), PopupRgb(0x386057), PopupRgb(0xD4AF70)};
    if (EffectiveTheme() == L"catppuccin-latte")
        return {PopupRgb(0xEFF1F5), PopupRgb(0xDCE0E8), PopupRgb(0x4C4F69),
                PopupRgb(0x6C6F85), PopupRgb(0xDCE0E8), PopupRgb(0x8839EF)};
    if (EffectiveTheme() == L"dracula-light")
        return {PopupRgb(0xF8F7FC), PopupRgb(0xDCD6E9), PopupRgb(0x292537),
                PopupRgb(0x655D77), PopupRgb(0xDCD6E9), PopupRgb(0x7545B5)};
    if (EffectiveTheme() == L"nord-light")
        return {PopupRgb(0xECEFF4), PopupRgb(0xD8DEE9), PopupRgb(0x2E3440),
                PopupRgb(0x4C566A), PopupRgb(0xD8DEE9), PopupRgb(0x3B668B)};
    if (EffectiveTheme() == L"gruvbox-light")
        return {PopupRgb(0xFBF1C7), PopupRgb(0xD5C4A1), PopupRgb(0x3C3836),
                PopupRgb(0x665C54), PopupRgb(0xD5C4A1), PopupRgb(0x9D0006)};
    if (EffectiveTheme() == L"tokyo-night-day")
        return {PopupRgb(0xE1E2E7), PopupRgb(0xC4C8DA), PopupRgb(0x343B58),
                PopupRgb(0x61709A), PopupRgb(0xC4C8DA), PopupRgb(0x2959AA)};
    if (EffectiveTheme() == L"dracula")
        return {PopupRgb(0x282A36), PopupRgb(0x44475A), PopupRgb(0xF8F8F2),
                PopupRgb(0xBFBFD0), PopupRgb(0x44475A), PopupRgb(0xBD93F9)};
    if (EffectiveTheme() == L"nord")
        return {PopupRgb(0x2E3440), PopupRgb(0x4C566A), PopupRgb(0xECEFF4),
                PopupRgb(0xD8DEE9), PopupRgb(0x4C566A), PopupRgb(0x88C0D0)};
    if (EffectiveTheme() == L"gruvbox")
        return {PopupRgb(0x282828), PopupRgb(0x504945), PopupRgb(0xEBDBB2),
                PopupRgb(0xA89984), PopupRgb(0x504945), PopupRgb(0xFABD2F)};
    if (EffectiveTheme() == L"tokyo-night")
        return {PopupRgb(0x1A1B26), PopupRgb(0x414868), PopupRgb(0xC0CAF5),
                PopupRgb(0xA9B1D6), PopupRgb(0x414868), PopupRgb(0x7AA2F7)};
    if (EffectiveTheme() == L"solarized-dark")
        return {PopupRgb(0x002B36), PopupRgb(0x586E75), PopupRgb(0x93A1A1),
                PopupRgb(0x839496), PopupRgb(0x586E75), PopupRgb(0xB58900)};
    if (EffectiveTheme() == L"solarized-light")
        return {PopupRgb(0xFDF6E3), PopupRgb(0xEEE8D5), PopupRgb(0x586E75),
                PopupRgb(0x657B83), PopupRgb(0x93A1A1), PopupRgb(0x268BD2)};
    if (EffectiveTheme() == L"rose-pine")
        return {PopupRgb(0x191724), PopupRgb(0x403B50), PopupRgb(0xE0DEF4),
                PopupRgb(0x908CAA), PopupRgb(0x403B50), PopupRgb(0xF6C177)};
    if (EffectiveTheme() == L"everforest")
        return {PopupRgb(0x2D353B), PopupRgb(0x475258), PopupRgb(0xD3C6AA),
                PopupRgb(0x9DA9A0), PopupRgb(0x475258), PopupRgb(0xDBBC7F)};
    if (EffectiveTheme() == L"synthwave-84")
        return {PopupRgb(kSynthWave.background), PopupRgb(kSynthWave.primary), PopupRgb(kSynthWave.text),
                PopupRgb(kSynthWave.muted),      PopupRgb(kSynthWave.surface), PopupRgb(kSynthWave.secondary)};
    if (EffectiveTheme() == L"flexoki")
        return {PopupRgb(0xFFFCF0), PopupRgb(0xDAD8CE), PopupRgb(0x100F0F),
                PopupRgb(0x6F6E69), PopupRgb(0xDAD8CE), PopupRgb(0x205EA6)};
    if (EffectiveTheme() == L"kanagawa")
        return {PopupRgb(0x1F1F28), PopupRgb(0x54546D), PopupRgb(0xDCD7BA),
                PopupRgb(0xC8C093), PopupRgb(0x54546D), PopupRgb(0xE6C384)};
    if (EffectiveTheme() == L"ayu")
        return {PopupRgb(0x0B0F14), PopupRgb(0x39424C), PopupRgb(0xE6E1CF),
                PopupRgb(0xA6A39B), PopupRgb(0x39424C), PopupRgb(0x59C2FF)};
    if (EffectiveTheme() == L"one-dark")
        return {PopupRgb(0x282C34), PopupRgb(0x4B5263), PopupRgb(0xABB2BF),
                PopupRgb(0x9DA5B4), PopupRgb(0x4B5263), PopupRgb(0x98C379)};
    // The popup is a taskbar flyout: follow Windows/taskbar mode, not app mode.
    if (WindowsSystemLightMode())
        return {PopupRgb(0xFAFAFA), PopupRgb(0xD1D1D1), PopupRgb(0x202020),
                PopupRgb(0x666666), PopupRgb(0xD1D1D1), WindowsAccentColor()};
    return {PopupRgb(0x202020), PopupRgb(0x505050), PopupRgb(0xF4F4F4),
            PopupRgb(0xB8B8B8), PopupRgb(0x505050), WindowsAccentColor()};
}
// Cached popup palette. Only the taskbar UI thread accesses this state.
// Explicit invalidation handles settings changes and Windows mode/accent changes.
static bool g_popupPaletteValid = false;
static PopupColors g_popupPaletteCached{};
static COLORREF g_popupNextAccentCached = CLR_INVALID;
static COLORREF g_popupNextTextCached = CLR_INVALID;
static COLORREF g_popupMosqueColor, g_popupHijriColor, g_popupIqamahColor, g_popupNextBorderColor;
static COLORREF g_popupIconColor = CLR_INVALID;
static void InvalidatePopupPaletteCache()
{
    g_popupPaletteValid = false;
}
static PopupColors PopupThemeUncached()
{
    auto colors = PopupThemeBase();
    colors.bg = OverridePopupColor(L"ThemeColorOverrides.Popup.Background", colors.bg);
    colors.border = OverridePopupColor(L"ThemeColorOverrides.Popup.Border", colors.border);
    colors.text = OverridePopupColor(L"ThemeColorOverrides.Popup.Text", colors.text);
    colors.muted = OverridePopupColor(L"ThemeColorOverrides.Popup.Muted", colors.muted);
    colors.rule = OverridePopupColor(L"ThemeColorOverrides.Popup.Rule", colors.rule);
    colors.footer = OverridePopupColor(L"ThemeColorOverrides.Popup.Footer", colors.footer);
    return colors;
}
static void EnsurePopupPaletteCache()
{
    if (g_popupPaletteValid)
        return;
    g_popupPaletteCached = PopupThemeUncached();
    g_popupNextAccentCached = OverridePopupColor(L"ThemeColorOverrides.Popup.NextPrayerBackground",
                                                 EffectiveTheme() == L"synthwave-84" ? PopupRgb(kSynthWave.primary)
                                                                                     : g_popupPaletteCached.footer);
    unsigned int nextTextRgb = 0;
    g_popupNextTextCached = TryThemeOverride(L"ThemeColorOverrides.Popup.NextPrayerText", nextTextRgb)
                                ? RGB((nextTextRgb >> 16) & 0xFF, (nextTextRgb >> 8) & 0xFF, nextTextRgb & 0xFF)
                                : CLR_INVALID;
    g_popupMosqueColor = OverridePopupColor(L"ThemeColorOverrides.Popup.MosqueName", g_popupPaletteCached.text);
    g_popupHijriColor = OverridePopupColor(L"ThemeColorOverrides.Popup.HijriDate", g_popupPaletteCached.muted);
    g_popupIqamahColor = OverridePopupColor(L"ThemeColorOverrides.Popup.Iqamah", g_popupPaletteCached.muted);
    g_popupNextBorderColor = OverridePopupColor(L"ThemeColorOverrides.Popup.NextPrayerBorder", g_popupNextAccentCached);
    unsigned iconRgb;
    g_popupIconColor = TryThemeOverride(L"ThemeColorOverrides.Popup.Icon", iconRgb)
                           ? RGB((iconRgb >> 16) & 255, (iconRgb >> 8) & 255, iconRgb & 255)
                           : CLR_INVALID;
    g_popupPaletteValid = true;
}
static PopupColors PopupTheme()
{
    EnsurePopupPaletteCache();
    return g_popupPaletteCached;
}
// Use the existing per-pixel alpha renderer for translucent Solid and genuine
// Transparent. Foreground text and decorative elements remain visible.
static bool PopupUsesLayeredAlpha()
{
    return (PopupMaterial() == L"solid" || PopupMaterial() == L"transparent") && PopupOpacity() < 100;
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
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
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
    if (!hwnd)
        return;
    using SetComposition = BOOL(WINAPI *)(HWND, MawaqitWindowCompositionAttributeData *);
    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    auto setComposition =
        user32 ? reinterpret_cast<SetComposition>(GetProcAddress(user32, "SetWindowCompositionAttribute")) : nullptr;
    if (!setComposition)
    {
        Wh_Log(L"[Mawaqit][PopupBackdrop] Composition API unavailable; using GDI popup rendering");
        return;
    }
    // ACCENT_DISABLED=0, BLURBEHIND=3, ACRYLICBLURBEHIND=4.
    // Gradient is AABBGGRR (alpha is the high byte).
    const bool blur = PopupMaterial() == L"blur";
    const bool acrylic = PopupMaterial() == L"acrylic";
    MawaqitAccentPolicy policy{};
    policy.state = blur ? 3 : acrylic ? 4 : 0;
    // ACCENT_POLICY gradientColor is AABBGGRR. Use a neutral dark surface
    // instead of the purple fallback tint, matching Windows 11 dark flyouts.
    // Acrylic gets a stronger tint for legibility; Blur stays more translucent.
    // These are intentionally popup-only: XAML pill brushes are unchanged.
    policy.gradientColor = acrylic ? 0xCC272020 : blur ? 0x60272020 : 0;
    unsigned tintRgb = 0;
    if (policy.state != 0 && TryThemeOverride(L"ThemeColorOverrides.Popup.Background", tintRgb))
    {
        // ACCENT_POLICY expects AABBGGRR, not AARRGGBB. Preserve each
        // material's existing alpha while overriding only the tint RGB.
        policy.gradientColor = (policy.gradientColor & 0xFF000000u) | ((tintRgb & 0xFFu) << 16) |
                               (tintRgb & 0x00FF00u) | ((tintRgb >> 16) & 0xFFu);
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
               PopupMaterial().c_str(), policy.state, static_cast<unsigned long>(policy.gradientColor));
}

// DWM accent backdrops use Windows' standard rounded-corner geometry.
// Blur and Acrylic use Windows backdrop corners; transparent uses the
// configurable radius and the same GDI geometry as Solid.
static int PopupRadiusDip()
{
    return (PopupMaterial() == L"blur" || PopupMaterial() == L"acrylic") ? 8 : std::max(0, g_cornerRadius);
}

static void ConfigurePopupDwmCorners(HWND hwnd)
{
    if (!hwnd || (PopupMaterial() != L"blur" && PopupMaterial() != L"acrylic"))
        return;
    HMODULE dwm = LoadLibraryW(L"dwmapi.dll");
    if (!dwm)
        return;
    using DwmSetAttribute = HRESULT(WINAPI *)(HWND, DWORD, LPCVOID, DWORD);
    auto setAttribute = reinterpret_cast<DwmSetAttribute>(GetProcAddress(dwm, "DwmSetWindowAttribute"));
    if (setAttribute)
    {
        constexpr DWORD kCornerPreferenceAttribute = 33;
        constexpr DWORD kRound = 2;      // DWMWCP_ROUND
        constexpr DWORD kDoNotRound = 1; // DWMWCP_DONOTROUND
        const DWORD preference =
            g_popupLayout == L"attached" && g_popupAttachedCorners != L"rounded" ? kDoNotRound : kRound;
        const HRESULT hr = setAttribute(hwnd, kCornerPreferenceAttribute, &preference, sizeof(preference));
        if (FAILED(hr))
            Wh_Log(L"[Mawaqit][Popup] DWM rounding failed: 0x%08lX", static_cast<unsigned long>(hr));
    }
    FreeLibrary(dwm);
}

static int PopupCornerRadiusPx(int width, int height, UINT dpi)
{
    const int scaled = MulDiv(PopupRadiusDip() * g_popupScale, static_cast<int>(dpi ? dpi : 96), 9600);
    return std::clamp(scaled, 0, std::min(width, height) / 2);
}

// Typography controls the GDI text size. Grow the logical layout only as
// needed for larger text; icons, borders and outer padding keep their size.
// ScalePercent remains a separate zoom applied to the final viewport.
static int PopupTextGrowth()
{
    return std::max(0, g_popupFontSize - 15);
}

static int PopupLogicalWidth()
{
    // Reserve additional room for the AM/PM suffix in the schedule columns.
    // Both the font size and the final popup zoom/DPI scaling are respected.
    const int suffixWidth = g_use12HourTime && g_showAmPm ? g_popupFontSize * 3 : 0;
    return 270 + PopupTextGrowth() * 14 + suffixWidth;
}

static int PopupRowPitch()
{
    return 32 + PopupTextGrowth();
}

// The heading follows the popup base size with a 20% emphasis, capped
// before the independent popup scale and monitor DPI are applied.
static int PopupMosqueFontSize()
{
    return std::min(26, MulDiv(g_popupFontSize, 120, 100));
}

// Measure the mosque name with the same GDI font and width used for painting.
// Bound extreme names to five lines so a malformed title cannot consume the screen.
static int PopupMosqueNameHeight()
{
    HDC dc = GetDC(nullptr);
    if (!dc)
        return std::max(20, PopupMosqueFontSize() + 2);
    HFONT font = CreateFontW(-PopupMosqueFontSize(), 0, 0, 0, g_popupFontWeight, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                             OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
    HGDIOBJ oldFont = font ? SelectObject(dc, font) : nullptr;
    TEXTMETRICW metrics{};
    GetTextMetricsW(dc, &metrics);
    const int lineHeight = std::max(1, static_cast<int>(metrics.tmHeight + metrics.tmExternalLeading));
    RECT bounds{0, 0, PopupLogicalWidth() - 28, 0};
    DrawTextW(dc, g_mosqueName.c_str(), -1, &bounds, DT_CALCRECT | DT_WORDBREAK | DT_NOPREFIX);
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
    const int rowHeight = PopupRowPitch();
    // Optical balance: reduce excess space below the final prayer row.
    // Preserve row pitch, highlighted-row geometry, header and footer spacing.
    constexpr int scheduleBottomGap = 8;
    const int footerHeight = 42 + PopupTextGrowth();
    PopupGeometry layout{};
    layout.nameHeight = PopupMosqueNameHeight();
    // GDI's measured rectangle includes extra font leading for wrapped lines.
    // Compensate optically without changing the single-line header spacing.
    // Keep a minimum gap for long names (up to five lines).
    const int headerLineHeight = 18 + PopupTextGrowth();
    const int estimatedLines = std::clamp((layout.nameHeight + headerLineHeight - 1) / headerLineHeight, 1, 5);
    const int opticalHeaderGap = std::max(8, headerGap - 4 * (estimatedLines - 1));
    const bool hijri = !PopupHijriDate().empty();
    layout.topRule = nameTop + layout.nameHeight + (hijri ? 22 + PopupTextGrowth() : 0) + opticalHeaderGap;
    layout.firstRow = layout.topRule + rowGap;
    const int optionalRows = g_showFajrWindowCountdown ? 1 + (g_imsakMinute >= 0 ? 1 : 0) : 0;
    layout.bottomRule = layout.firstRow + (5 + optionalRows) * rowHeight + scheduleBottomGap;
    layout.height = layout.bottomRule + footerHeight;
    return layout;
}

static int PopupLogicalHeight()
{
    return GetPopupGeometry().height;
}

// Lucide icon geometry (24x24, round caps/joins, 2-unit stroke).
// Render with antialiased GDI+ at the popup's actual device-pixel size rather
// than rounding every SVG coordinate to a tiny integer GDI pixel.
// Source: https://lucide.dev/icons/{sunrise,sun,sun-dim,sunset,moon} (ISC).
static void DrawPopupPrayerIcon(HDC dc, int prayerIndex, int left, int top, COLORREF color)
{
    using namespace Gdiplus;
    static const ULONG_PTR gdiplusToken = []() -> ULONG_PTR {
        GdiplusStartupInput input;
        ULONG_PTR token = 0;
        return GdiplusStartup(&token, &input, nullptr) == Ok ? token : 0;
    }();
    if (!gdiplusToken)
        return;
    // The popup uses MM_ANISOTROPIC: convert logical icon bounds to device
    // pixels explicitly so GDI+ does not apply a second viewport transform.
    // Optical alignment with the prayer text: leave balanced space above and
    // below the icon, including inside the next-prayer highlight. Apply before
    // LPtoDP so the correction follows popup zoom and monitor DPI.
    ++top;
    POINT bounds[] = {{left, top}, {left + 20, top + 20}};
    LPtoDP(dc, bounds, 2);
    const REAL w = static_cast<REAL>(bounds[1].x - bounds[0].x);
    const REAL h = static_cast<REAL>(bounds[1].y - bounds[0].y);
    if (w <= 0 || h <= 0)
        return;

    // GDI+ inherits the HDC's anisotropic mapping. The icon bounds above
    // are already converted to device pixels, so temporarily use MM_TEXT
    // to prevent applying the popup scale a second time (which displaced
    // icons increasingly at 150% and 200%). Restore the mapping afterwards.
    const int savedMapping = SaveDC(dc);
    SetMapMode(dc, MM_TEXT);
    {
        Graphics graphics(dc);
        graphics.SetSmoothingMode(SmoothingModeAntiAlias);
        graphics.SetPixelOffsetMode(PixelOffsetModeHighQuality);
        graphics.TranslateTransform(static_cast<REAL>(bounds[0].x), static_cast<REAL>(bounds[0].y));
        graphics.ScaleTransform(w / 24.0f, h / 24.0f);
        Pen pen(Color(255, GetRValue(color), GetGValue(color), GetBValue(color)), 2.0f);
        pen.SetStartCap(LineCapRound);
        pen.SetEndCap(LineCapRound);
        pen.SetLineJoin(LineJoinRound);

        auto line = [&](REAL x1, REAL y1, REAL x2, REAL y2) { graphics.DrawLine(&pen, x1, y1, x2, y2); };
        auto sun = [&]() { graphics.DrawEllipse(&pen, 8.0f, 8.0f, 8.0f, 8.0f); };
        if (prayerIndex == 0 || prayerIndex == 3)
        {
            // Original Lucide sunrise / sunset: arrow above a semicircle,
            // horizontal horizon, and four rays. Arrow direction distinguishes them.
            line(12, 2, 12, 10);
            line(4.93f, 10.93f, 6.34f, 12.34f);
            line(2, 18, 4, 18);
            line(20, 18, 22, 18);
            line(19.07f, 10.93f, 17.66f, 12.34f);
            line(22, 22, 2, 22);
            if (prayerIndex == 0)
            {
                line(8, 6, 12, 2);
                line(12, 2, 16, 6);
            }
            else
            {
                line(8, 6, 12, 10);
                line(12, 10, 16, 6);
            }
            graphics.DrawArc(&pen, 8.0f, 14.0f, 8.0f, 8.0f, 180.0f, 180.0f);
        }
        else if (prayerIndex == 5)
        {
            // Shuruq: sun at the horizon, using the existing Fajr silhouette
            // without its upward arrow. Keep the short vertical ray close to
            // the top of the rising sun, with a small visible gap.
            line(12, 8, 12, 10);
            line(4.93f, 10.93f, 6.34f, 12.34f);
            line(2, 18, 4, 18);
            line(20, 18, 22, 18);
            line(19.07f, 10.93f, 17.66f, 12.34f);
            line(22, 22, 2, 22);
            graphics.DrawArc(&pen, 8.0f, 14.0f, 8.0f, 8.0f, 180.0f, 180.0f);
        }
        else if (prayerIndex == 1)
        {
            // Lucide sun: centered disc and eight evenly spaced rays.
            sun();
            line(12, 2, 12, 4);
            line(12, 20, 12, 22);
            line(4.93f, 4.93f, 6.34f, 6.34f);
            line(17.66f, 17.66f, 19.07f, 19.07f);
            line(2, 12, 4, 12);
            line(20, 12, 22, 12);
            line(6.34f, 17.66f, 4.93f, 19.07f);
            line(19.07f, 4.93f, 17.66f, 6.34f);
        }
        else if (prayerIndex == 2)
        {
            // Lucide sun-dim: disc with short rays and a quieter silhouette.
            sun();
            line(12, 4, 12, 2);
            line(12, 22, 12, 20);
            line(4.93f, 4.93f, 6.34f, 6.34f);
            line(17.66f, 17.66f, 19.07f, 19.07f);
            line(2, 12, 4, 12);
            line(20, 12, 22, 12);
            line(6.34f, 17.66f, 4.93f, 19.07f);
            line(19.07f, 4.93f, 17.66f, 6.34f);
        }
        else if (prayerIndex == 4)
        {
            // Lucide moon: one continuous crescent outline (not two nested moons).
            GraphicsPath crescent;
            crescent.StartFigure();
            crescent.AddBezier(20.985f, 12.486f, 19.2f, 18.0f, 16.0f, 21.0f, 11.514f, 21.0f);
            crescent.AddBezier(11.514f, 21.0f, 6.25f, 21.0f, 3.0f, 17.25f, 3.0f, 12.0f);
            crescent.AddBezier(3.0f, 12.0f, 3.0f, 7.514f, 6.0f, 4.3f, 11.514f, 3.015f);
            crescent.AddBezier(11.514f, 3.015f, 9.2f, 5.6f, 9.0f, 8.0f, 9.0f, 9.0f);
            crescent.AddBezier(9.0f, 9.0f, 9.0f, 12.5f, 11.5f, 15.0f, 15.0f, 15.0f);
            crescent.AddBezier(15.0f, 15.0f, 17.0f, 15.0f, 19.0f, 14.0f, 20.985f, 12.486f);
            graphics.DrawPath(&pen, &crescent);
        }
    } // Destroy Graphics before restoring the HDC mapping.
    RestoreDC(dc, savedMapping);
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
    void *dibPixels = nullptr;
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
        if (bitmap)
            DeleteObject(bitmap);
        if (bufferDc)
            DeleteDC(bufferDc);
        Wh_Log(L"[Mawaqit][PopupAlpha] buffer allocation failed; using direct GDI");
        bufferDc = nullptr;
        bitmap = nullptr;
    }
    HGDIOBJ previousBitmap = bitmap ? SelectObject(bufferDc, bitmap) : nullptr;
    HDC renderDc = bufferDc ? bufferDc : dc;
    // Draw in the typography-aware logical coordinate space. The GDI
    // viewport applies the independent popup zoom and monitor DPI.
    RECT client{};
    GetClientRect(hwnd, &client);
    int savedDc = SaveDC(renderDc);
    SetMapMode(renderDc, MM_ANISOTROPIC);
    SetWindowExtEx(renderDc, PopupLogicalWidth(), PopupLogicalHeight(), nullptr);
    SetViewportExtEx(renderDc, client.right - client.left, client.bottom - client.top, nullptr);
    RECT r{0, 0, PopupLogicalWidth(), PopupLogicalHeight()};
    auto colors = PopupTheme();
    HBRUSH bg = CreateSolidBrush(colors.bg);
    FillRect(renderDc, &r, bg);
    DeleteObject(bg);
    // Draw the outline just INSIDE the clipped popup region. Drawing
    // on the outermost pixel row/column caused GDI's non-antialiased stroke
    // to be cut by SetWindowRgn, leaving uneven/square corner fragments.
    // Keep the geometry in the same logical viewport as the content.
    constexpr int borderInset = 1;
    const int logicalRadius = std::clamp(PopupRadiusDip(), 0, static_cast<int>(std::min(r.right, r.bottom) / 2));
    const bool attached = g_popupLayout == L"attached";
    const bool adaptive = attached && g_popupAttachedCorners == L"adaptive";
    const bool square = attached && g_popupAttachedCorners == L"square";
    const int innerRadius = square ? 0 : std::max(0, logicalRadius - borderInset);
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
        else if (adaptive)
        {
            // Draw the rounded outline only away from the taskbar-facing
            // edge, then draw a straight line on that edge.
            const int savedClip = SaveDC(renderDc);
            if (g_popupAttachedAtTop)
                IntersectClipRect(renderDc, 0, innerRadius + 1, r.right, r.bottom);
            else
                IntersectClipRect(renderDc, 0, 0, r.right, r.bottom - innerRadius - 1);
            RoundRect(renderDc, left, top, right, bottom, innerRadius * 2, innerRadius * 2);
            RestoreDC(renderDc, savedClip);
            MoveToEx(renderDc, left, g_popupAttachedAtTop ? top : bottom - 1, nullptr);
            LineTo(renderDc, right, g_popupAttachedAtTop ? top : bottom - 1);
            // Keep the side edges connected to the straight taskbar-facing edge.
            MoveToEx(renderDc, left, g_popupAttachedAtTop ? top : bottom - innerRadius, nullptr);
            LineTo(renderDc, left, g_popupAttachedAtTop ? top + innerRadius : bottom);
            MoveToEx(renderDc, right - 1, g_popupAttachedAtTop ? top : bottom - innerRadius, nullptr);
            LineTo(renderDc, right - 1, g_popupAttachedAtTop ? top + innerRadius : bottom);
        }
        else
            RoundRect(renderDc, left, top, right, bottom, innerRadius * 2, innerRadius * 2);
        SelectObject(renderDc, oldBrush);
        SelectObject(renderDc, prevPen);
        DeleteObject(border);
    }
    SetBkMode(renderDc, TRANSPARENT);
    // Rasterize glyphs at their final physical size with MM_TEXT. Scaled
    // anisotropic GDI text can lose smoothing at popup zoom / monitor DPI.
    // Layered alpha surfaces use grayscale edges to avoid ClearType fringes.
    const DWORD textQuality = layered ? ANTIALIASED_QUALITY : CLEARTYPE_QUALITY;
    auto makePopupFont = [&](int logicalSize, int weight) {
        const int pixels = std::max(1, MulDiv(logicalSize, pixelHeight, PopupLogicalHeight()));
        return CreateFontW(-pixels, 0, 0, 0, weight, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                           OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, textQuality, DEFAULT_PITCH, L"Segoe UI");
    };
    HFONT font = makePopupFont(g_popupFontSize, g_popupFontWeight);
    HGDIOBJ oldFont = SelectObject(renderDc, font);
    HFONT boldFont = makePopupFont(g_popupFontSize, std::max(g_popupFontWeight, static_cast<int>(FW_BOLD)));
    HFONT headingFont = makePopupFont(PopupMosqueFontSize(), g_popupFontWeight);
    auto drawPopupText = [&](const std::wstring &value, RECT rect, COLORREF color, UINT flags) {
        POINT bounds[] = {{rect.left, rect.top}, {rect.right, rect.bottom}};
        if (!LPtoDP(renderDc, bounds, 2))
            return;
        RECT physical{bounds[0].x, bounds[0].y, bounds[1].x, bounds[1].y};
        const int textDc = SaveDC(renderDc);
        if (!textDc)
            return;
        SetMapMode(renderDc, MM_TEXT);
        if (g_popupTextShadow)
        {
            RECT shadowRect = physical;
            const int offset = std::max(1, MulDiv(1, pixelHeight, PopupLogicalHeight()));
            OffsetRect(&shadowRect, offset, offset);
            SetTextColor(renderDc, RGB(0, 0, 0));
            DrawTextW(renderDc, value.c_str(), -1, &shadowRect, flags);
        }
        SetTextColor(renderDc, color);
        DrawTextW(renderDc, value.c_str(), -1, &physical, flags);
        RestoreDC(renderDc, textDc);
    };
    auto line = [&](const std::wstring &value, int left, int top, int right, int height, COLORREF color, UINT flags) {
        RECT rect{left, top, right, top + height};
        drawPopupText(value, rect, color, flags | DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX | DT_END_ELLIPSIS);
    };
    const PopupGeometry layout = GetPopupGeometry();
    RECT nameRect{14, 12, r.right - 14, 12 + layout.nameHeight};
    if (headingFont)
        SelectObject(renderDc, headingFont);
    drawPopupText(g_mosqueName, nameRect, g_popupMosqueColor, DT_CENTER | DT_WORDBREAK | DT_NOPREFIX | DT_WORD_ELLIPSIS);
    SelectObject(renderDc, font);
    const std::wstring popupHijri = PopupHijriDate();
    if (!popupHijri.empty())
        line(popupHijri, 14, nameRect.bottom + 4, r.right - 14, 18 + PopupTextGrowth(), g_popupHijriColor, DT_CENTER);
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
        for (const auto &prayer : g_today)
        {
            if (!IqamahSuffix(prayer).empty())
            {
                hasIqamahColumn = true;
                break;
            }
        }
    }
    // At reduced viewport scales, GDI glyph rounding can clip the right-aligned
    // Iqamah suffix inside its original 45-unit logical column. Reserve a
    // little extra width on the LEFT while retaining the same right padding.
    // Keep 100%+ geometry pixel-identical to the approved popup.
    const int iqamahColumnExtra = (g_popupScale < 100 ? 12 : 0) + PopupTextGrowth() * 3;
    const int adhanColumnRight = hasIqamahColumn ? r.right - 66 - iqamahColumnExtra : r.right - 20;
    const bool showImsak = g_showFajrWindowCountdown && g_imsakMinute >= 0;
    const int rowCount = 5 + (g_showFajrWindowCountdown ? 1 : 0) + (showImsak ? 1 : 0);
    const int firstRow = layout.firstRow;
    const int prayerRowHeight = PopupRowPitch(); // Actual row pitch, including space between rows.
    // GDI popup: blend the theme accent against the popup background before
    // painting text. No extra layered window, so existing popup materials work.
    EnsurePopupPaletteCache();
    COLORREF accent = g_popupNextAccentCached;
    const int accentAlpha = g_popupNextPrayerAccentOpacity;
    COLORREF highlight = RGB((GetRValue(accent) * accentAlpha + GetRValue(colors.bg) * (100 - accentAlpha) + 50) / 100,
                             (GetGValue(accent) * accentAlpha + GetGValue(colors.bg) * (100 - accentAlpha) + 50) / 100,
                             (GetBValue(accent) * accentAlpha + GetBValue(colors.bg) * (100 - accentAlpha) + 50) / 100);
    for (int row = 0; row < rowCount; row++)
    {
        if (showImsak && row == 0)
        {
            const std::wstring time = FormatClockTime(g_imsakMinute);
            const int y = firstRow + row * prayerRowHeight;
            line(L"Imsak", 20, y, r.right - 20, 21 + PopupTextGrowth(), colors.text, DT_LEFT);
            line(time, 20, y, adhanColumnRight, 21 + PopupTextGrowth(), colors.text, DT_RIGHT);
            continue;
        }
        const int prayerRow = row - (showImsak ? 1 : 0);
        if (g_showFajrWindowCountdown && prayerRow == 1)
        {
            std::wstring sunrise = L"--:--";
            if (g_shuruqMinute >= 0)
            {
                sunrise = FormatClockTime(g_shuruqMinute);
            }
            int y = firstRow + row * prayerRowHeight;
            if (g_popupShowPrayerIcons)
                DrawPopupPrayerIcon(renderDc, 5, 20, y + 1 + PopupTextGrowth() / 2, g_popupIconColor != CLR_INVALID ? g_popupIconColor : colors.text);
            line(L"Shuruq", g_popupShowPrayerIcons ? 48 : 20, y, r.right - 20, 21 + PopupTextGrowth(), colors.text,
                 DT_LEFT);
            // Align Shuruq with the Adhan-time column; keep Iqamah column blank.
            line(sunrise, 20, y, adhanColumnRight, 21 + PopupTextGrowth(), colors.text, DT_RIGHT);
            continue;
        }
        int i = g_showFajrWindowCountdown && prayerRow > 1 ? prayerRow - 1 : prayerRow;
        std::wstring time = L"--:--";
        if (i < (int)g_today.size())
        {
            time = FormatClockTime(g_today[i].minute);
        }
        int y = firstRow + row * prayerRowHeight;
        // g_label already identifies the current/next prayer, including Iqamah.
        // Ignore non-prayer labels such as the Fajr window status.
        if (((g_popupShowNextPrayerAccent && accentAlpha > 0) || g_popupShowNextPrayerBorder) &&
            g_label.rfind(kNames[i], 0) == 0)
        {
            HBRUSH fill = (g_popupShowNextPrayerAccent && accentAlpha > 0) ? CreateSolidBrush(highlight) : nullptr;
            HPEN outline = g_popupShowNextPrayerBorder ? CreatePen(PS_SOLID, 1, g_popupNextBorderColor) : nullptr;
            HGDIOBJ oldPen = SelectObject(renderDc, outline ? static_cast<HGDIOBJ>(outline) : GetStockObject(NULL_PEN));
            HGDIOBJ oldBrush = SelectObject(renderDc, fill ? static_cast<HGDIOBJ>(fill) : GetStockObject(NULL_BRUSH));
            // Fixed 2 px bottom padding and natural gaps between 32 px rows.
            // The text columns stay unchanged; the row pitch creates genuine
            // separation from the previous and following prayer rows.
            const int left = 12;
            const int top = y - 2;
            const int right = r.right - 12;
            const int bottom = y + 26 + PopupTextGrowth();
            if (right > left && bottom > top)
                RoundRect(renderDc, left, top, right, bottom, 10, 10);
            SelectObject(renderDc, oldBrush);
            SelectObject(renderDc, oldPen);
            if (fill)
                DeleteObject(fill);
            if (outline)
                DeleteObject(outline);
        }
        // Emphasize only the next prayer's row, regardless of accent visibility.
        // Restore the normal font before drawing the next row or footer.
        const bool isNextPrayerRow = g_label.rfind(kNames[i], 0) == 0;
        // Accent 2 on the selected prayer row gives supported theme families
        // a subtle identity boost without changing existing backgrounds,
        // borders, footer colors, or any Windhawk settings.
        ExtendedThemeColors extended{};
        const bool hasExtended = TryExtendedThemeColors(EffectiveTheme(), extended);
        const COLORREF rowTextColor = isNextPrayerRow && g_popupNextTextCached != CLR_INVALID ? g_popupNextTextCached
                                      : isNextPrayerRow
                                          ? (EffectiveTheme() == L"synthwave-84" ? PopupRgb(kSynthWave.secondary)
                                             : hasExtended                       ? PopupRgb(extended.secondary)
                                                                                 : colors.text)
                                          : colors.text;
        const bool boldThisRow = g_popupBoldNextPrayer && boldFont && isNextPrayerRow;
        if (boldThisRow)
            SelectObject(renderDc, boldFont);
        if (g_popupShowPrayerIcons)
        {
            // Position relative to the prayer label; DrawPopupPrayerIcon applies
            // the shared optical offset before popup zoom and DPI conversion.
            // Preserve the 8-unit gap between icon and prayer name.
            DrawPopupPrayerIcon(renderDc, i, 20, y + 1 + PopupTextGrowth() / 2, g_popupIconColor != CLR_INVALID ? g_popupIconColor : rowTextColor);
        }
        line(kNames[i], g_popupShowPrayerIcons ? 48 : 20, y, r.right - 20, 23 + PopupTextGrowth(), rowTextColor,
             DT_LEFT);
        if (i < (int)g_today.size() && !IqamahSuffix(g_today[i]).empty())
        {
            line(time, 20, y, r.right - 66 - iqamahColumnExtra, 23 + PopupTextGrowth(), rowTextColor, DT_RIGHT);
            line(IqamahSuffix(g_today[i]), r.right - 65 - iqamahColumnExtra, y, r.right - 20, 23 + PopupTextGrowth(),
                 g_popupIqamahColor, DT_RIGHT);
        }
        else
            line(time, 20, y, r.right - 20, 23 + PopupTextGrowth(), rowTextColor, DT_RIGHT);
        if (boldThisRow)
            SelectObject(renderDc, font);
    }
    std::wstring footerCountdown = g_countdown;
    if (footerCountdown != L"--:--" && !footerCountdown.empty() && footerCountdown.front() == L'-')
        footerCountdown.erase(0, 1);

    line(g_label + L" in " + footerCountdown, 12, bottomRule + 8, r.right - 12, 24 + PopupTextGrowth(), colors.footer,
         DT_CENTER);
    SelectObject(renderDc, oldFont);
    if (headingFont)
        DeleteObject(headingFont);
    if (boldFont)
        DeleteObject(boldFont);
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
            // Transparent uses zero background alpha by default; explicit
            // popup opacity can add background while keeping text opaque.
            const int bgAlpha = std::max(0, std::min(255, (PopupOpacity() * 255 + 50) / 100));
            auto *pixels = static_cast<unsigned char *>(dibPixels);
            for (int i = 0; i < pixelWidth * pixelHeight; ++i)
            {
                unsigned char *p = pixels + i * 4; // BGRA
                const int dB = std::abs((int)p[0] - bgB);
                const int dG = std::abs((int)p[1] - bgG);
                const int dR = std::abs((int)p[2] - bgR);
                // Differences are a conservative estimate of foreground
                // coverage. The exact text pixels stay fully opaque.
                const int difference = std::max(dR, std::max(dG, dB));
                const int coverage = difference == 0 ? 0 : std::min(255, difference * 4);
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
            if (!UpdateLayeredWindow(hwnd, nullptr, &dst, &size, bufferDc, &src, 0, &blend, ULW_ALPHA))
                Wh_Log(L"[Mawaqit][PopupAlpha] UpdateLayeredWindow failed: %lu", GetLastError());
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
// Remember the originating XAML pill and taskbar while the native popup is open.
static winrt::weak_ref<Grid> g_popupAnchorPill;
// One pending hover belongs to the taskbar UI thread and is cancelled on
// pointer exit, interaction, rebuild, settings change and unload.
static UINT_PTR g_hoverOpenTimer = 0;
static winrt::weak_ref<Grid> g_pendingHoverPill;
static HWND g_pendingHoverTaskbar = nullptr;
static ULONGLONG g_hoverEnteredAt = 0;
static void TogglePrayerPopup(Grid const &anchorPill, HWND anchorTaskbar, bool hover);

static bool AnyMouseButtonDown()
{
    return ((GetAsyncKeyState(VK_LBUTTON) | GetAsyncKeyState(VK_RBUTTON) | GetAsyncKeyState(VK_MBUTTON) |
             GetAsyncKeyState(VK_XBUTTON1) | GetAsyncKeyState(VK_XBUTTON2)) & 0x8000) != 0;
}

static void CancelHoverOpen()
{
    if (g_hoverOpenTimer)
    {
        KillTimer(nullptr, g_hoverOpenTimer);
        g_hoverOpenTimer = 0;
    }
    g_pendingHoverPill = {};
    g_pendingHoverTaskbar = nullptr;
    g_hoverEnteredAt = 0;
}

static bool CursorInsidePill(Grid const &pill, HWND taskbar)
{
    if (!pill || !taskbar || !IsWindow(taskbar) || pill.Visibility() != Visibility::Visible)
        return false;
    RECT bar{};
    POINT cursor{};
    if (!GetWindowRect(taskbar, &bar) || !GetCursorPos(&cursor))
        return false;
    try
    {
        auto pt = pill.TransformToVisual(nullptr).TransformPoint({0, 0});
        UINT dpi = GetDpiForWindow(taskbar);
        double scale = static_cast<double>(dpi ? dpi : 96) / 96.0;
        RECT rect{bar.left + static_cast<LONG>(std::lround(pt.X * scale)),
                  bar.top + static_cast<LONG>(std::lround(pt.Y * scale)),
                  bar.left + static_cast<LONG>(std::lround((pt.X + pill.ActualWidth()) * scale)),
                  bar.top + static_cast<LONG>(std::lround((pt.Y + pill.ActualHeight()) * scale))};
        return PtInRect(&rect, cursor) != FALSE;
    }
    catch (...) { return false; }
}

static void CancelHoverForPill(Grid const &pill)
{
    auto pending = g_pendingHoverPill.get();
    if (pending && pending == pill)
        CancelHoverOpen();
}

static void CALLBACK HoverOpenTimerProc(HWND, UINT, UINT_PTR timerId, DWORD)
{
    if (timerId != g_hoverOpenTimer || !g_hoverOpenTimer)
        return;
    auto pending = g_pendingHoverPill.get();
    HWND bar = g_pendingHoverTaskbar;
    if (g_stopping.load() || !g_enablePopup || !g_openPopupOnHover || AnyMouseButtonDown() ||
        !CursorInsidePill(pending, bar) || (g_popup && IsWindowVisible(g_popup)))
    {
        CancelHoverOpen();
        return;
    }
    if (GetTickCount64() - g_hoverEnteredAt >= static_cast<ULONGLONG>(g_hoverOpenDelayMs))
    {
        CancelHoverOpen();
        try
        {
            TogglePrayerPopup(pending, bar, true);
        }
        catch (...)
        {
            Wh_Log(L"[Mawaqit] Hover popup open failed");
        }
    }
}

static void ScheduleHoverOpen(Grid const &pill, HWND taskbar)
{
    CancelHoverOpen();
    if (!g_enablePopup || !g_openPopupOnHover || g_stopping.load() || AnyMouseButtonDown() ||
        (g_popup && IsWindowVisible(g_popup)))
        return;
    if (!taskbar)
        taskbar = taskwin::FindCurrentProcessTaskbarWnd();
    if (!CursorInsidePill(pill, taskbar))
        return;
    g_pendingHoverPill = pill;
    g_pendingHoverTaskbar = taskbar;
    g_hoverEnteredAt = GetTickCount64();
    g_hoverOpenTimer = SetTimer(nullptr, 0, 25, HoverOpenTimerProc);
    if (!g_hoverOpenTimer)
    {
        Wh_Log(L"[Mawaqit] Hover open timer failed: %lu", GetLastError());
        CancelHoverOpen();
    }
}
static HWND g_popupAnchorTaskbar = nullptr;

// Poll hover/close at 50 ms, but redraw time-based popup content at most once per second.
static ULONGLONG g_popupLastPeriodicPaint = 0;

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
        Wh_Log(L"[Mawaqit] Popup close animation timer failed: %lu; hiding immediately", GetLastError());
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
    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_XBUTTONDOWN:
    case WM_MOUSEWHEEL:
        g_hoverPopupActive = false;
        g_hoverOutsideSince = 0;
        break;
    case WM_KEYDOWN:
        if (wp == VK_ESCAPE)
        {
            HidePrayerPopup();
            return 0;
        }
        g_hoverPopupActive = false;
        g_hoverOutsideSince = 0;
        break;
    case WM_PAINT:
        PaintPrayerPopup(hwnd);
        return 0;
    case WM_ERASEBKGND:
        return 1;
    case WM_CLOSE:
        HidePrayerPopup();
        return 0;
    case WM_TIMER: {
        if (wp == kPopupAnimationTimer && g_popupAnimating)
        {
            ULONGLONG elapsed = GetTickCount64() - g_popupAnimationStarted;
            double t = std::min(1.0, static_cast<double>(elapsed) / g_popupAnimationDuration);
            // Cubic ease-out on entry; cubic ease-in on exit.
            double eased = g_popupClosing ? t * t * t : 1.0 - (1.0 - t) * (1.0 - t) * (1.0 - t);
            int y = g_popupStartY + static_cast<int>(std::lround((g_popupTargetY - g_popupStartY) * eased));
            SetWindowPos(hwnd, nullptr, g_popupTargetX, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
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
            if (IsWindowVisible(hwnd) && !g_popupClosing)
            {
                POINT cursor{};
                RECT popupRect{};
                auto anchor = g_popupAnchorPill.get();
                bool pointerKnown = GetCursorPos(&cursor) && GetWindowRect(hwnd, &popupRect);
                bool inside = pointerKnown && (PtInRect(&popupRect, cursor) ||
                              CursorInsidePill(anchor, g_popupAnchorTaskbar));
                ULONGLONG now = GetTickCount64();
                int seconds = g_popupAutoCloseSeconds.load();
                if (inside || !pointerKnown)
                {
                    g_hoverOutsideSince = 0;
                    g_popupAutoCloseDeadline = seconds > 0 ? now + static_cast<ULONGLONG>(seconds) * 1000 : 0;
                }
                else
                {
                    if (g_hoverPopupActive && !g_popupAnimating)
                    {
                        if (!g_hoverOutsideSince)
                            g_hoverOutsideSince = now;
                        if (now - g_hoverOutsideSince >= static_cast<ULONGLONG>(g_hoverCloseDelayMs))
                        {
                            HidePrayerPopup();
                            return 0;
                        }
                    }
                    if (g_popupAutoCloseDeadline && now >= g_popupAutoCloseDeadline)
                    {
                        Wh_Log(L"[Mawaqit] Popup auto-close inactivity timeout reached");
                        HidePrayerPopup();
                        return 0;
                    }
                }
            }
            // Hover and auto-close checks need the responsive 50 ms timer; the
            // countdown and schedule do not need 20 full GDI repaints/second.
            ULONGLONG now = GetTickCount64();
            if (now - g_popupLastPeriodicPaint >= 1000)
            {
                g_popupLastPeriodicPaint = now;
                InvalidateRect(hwnd, nullptr, FALSE);
            }
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
// Find the painted taskbar background for the popup's owning monitor.
// Unlike the taskbar HWND, this reflects Taskbar Styler's XAML margins.
static bool StyledPopupTaskbarEdge(Grid const &pill, HWND taskbar, bool top, int &edgePx)
{
    if (!pill || !taskbar || g_popupLayout != L"attached")
        return false;
    try
    {
        winrt::Windows::UI::Xaml::DependencyObject root = pill;
        for (int i = 0; i < 20; ++i)
        {
            auto next = VisualTreeHelper::GetParent(root);
            if (!next)
                break;
            root = next;
        }
        struct Node
        {
            winrt::Windows::UI::Xaml::DependencyObject object;
            int depth;
        };
        std::vector<Node> pending{{root, 0}};
        int visited = 0;
        while (!pending.empty() && visited++ < 350)
        {
            auto node = pending.back();
            pending.pop_back();
            auto fe = node.object.try_as<FrameworkElement>();
            if (fe && fe.Name() == L"BackgroundControl" &&
                winrt::get_class_name(node.object) == L"Taskbar.TaskbarBackground")
            {
                const double height = fe.ActualHeight();
                if (height <= 15.0 || !std::isfinite(height))
                    return false;
                auto transform = fe.TransformToVisual(nullptr);
                auto origin = transform.TransformPoint({0, 0});
                const double edgeDip = origin.Y + (top ? height : 0.0);
                if (!std::isfinite(edgeDip))
                    return false;
                RECT bar{};
                if (!GetWindowRect(taskbar, &bar))
                    return false;
                UINT dpi = GetDpiForWindow(taskbar);
                if (!dpi)
                    dpi = 96;
                const double scale = static_cast<double>(dpi) / 96.0;
                const double candidate = bar.top + edgeDip * scale;
                // Reject uninitialized or unrelated visual-tree geometry.
                if (candidate < bar.top - 16 * scale || candidate > bar.bottom + 16 * scale)
                    return false;
                edgePx = static_cast<int>(std::lround(candidate));
                return true;
            }
            if (node.depth >= 12)
                continue;
            int count = VisualTreeHelper::GetChildrenCount(node.object);
            for (int i = count - 1; i >= 0; --i)
                pending.push_back({VisualTreeHelper::GetChild(node.object, i), node.depth + 1});
        }
    }
    catch (...)
    { /* Keep the native HWND fallback during XAML rebuilds. */
    }
    return false;
}

// Reposition an already-visible attached popup without recreating it or
// interfering with entry/exit animation, hover timers, or popup content.
static void RefreshAttachedPopupPositionOnUiThread()
{
    if (g_popupLayout != L"attached" || !g_popup || !IsWindowVisible(g_popup) || g_popupAnimating || g_popupClosing ||
        !g_popupAnchorTaskbar || !IsWindow(g_popupAnchorTaskbar))
        return;
    auto pill = g_popupAnchorPill.get();
    if (!pill)
        return;
    RECT bar{}, current{};
    if (!GetWindowRect(g_popupAnchorTaskbar, &bar) || !GetWindowRect(g_popup, &current))
        return;
    MONITORINFO mi{};
    mi.cbSize = sizeof(mi);
    if (!GetMonitorInfoW(MonitorFromWindow(g_popupAnchorTaskbar, MONITOR_DEFAULTTONEAREST), &mi))
        return;
    const bool top = bar.top <= mi.rcMonitor.top + 10;
    int edge = 0;
    if (!StyledPopupTaskbarEdge(pill, g_popupAnchorTaskbar, top, edge))
        return;
    UINT dpi = GetDpiForWindow(g_popupAnchorTaskbar);
    if (!dpi)
        dpi = 96;
    const double scale = static_cast<double>(dpi) / 96.0;
    const int w = current.right - current.left;
    const int h = current.bottom - current.top;
    const int margin = MulDiv(4, dpi, 96);
    int x = current.left;
    try
    {
        auto pt = pill.TransformToVisual(nullptr).TransformPoint({0, 0});
        x = bar.left + static_cast<int>(std::lround((pt.X + pill.ActualWidth() / 2.0) * scale)) - w / 2;
    }
    catch (...)
    {
    }
    int y = top ? edge : edge - h;
    x = std::clamp(x, static_cast<int>(mi.rcMonitor.left) + margin, static_cast<int>(mi.rcMonitor.right) - w - margin);
    y = std::clamp(y, static_cast<int>(mi.rcMonitor.top) + margin, static_cast<int>(mi.rcMonitor.bottom) - h - margin);
    if (x != current.left || y != current.top)
    {
        SetWindowPos(g_popup, nullptr, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
        g_popupTargetX = x;
        g_popupTargetY = y;
        Wh_Log(L"[Mawaqit][PopupFit][Live] x=%d y=%d edge=%d top=%d", x, y, edge, top ? 1 : 0);
    }
}

static void TogglePrayerPopup(Grid const &anchorPill, HWND anchorTaskbar, bool hover)
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
        if (hover)
            return;
        auto currentAnchor = g_popupAnchorPill.get();
        if (g_hoverPopupActive && currentAnchor && currentAnchor == anchorPill && !g_popupClosing)
        {
            g_hoverPopupActive = false;
            g_hoverOutsideSince = 0;
            int seconds = g_popupAutoCloseSeconds.load();
            g_popupAutoCloseDeadline = seconds > 0 ? GetTickCount64() + static_cast<ULONGLONG>(seconds) * 1000 : 0;
            SetForegroundWindow(g_popup);
            SetFocus(g_popup);
            return;
        }
        if (currentAnchor && currentAnchor != anchorPill)
            FinishPopupHide(); // Open against the newly clicked monitor below.
        else
        {
            HidePrayerPopup();
            return;
        }
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
                       (unsigned long long)stamp.QuadPart, reinterpret_cast<void *>(&PrayerPopupProc));
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
            Wh_Log(L"[Mawaqit] class-register result=%u error=%lu", (unsigned)atom, atom ? 0 : GetLastError());
            if (!atom)
                return;
            g_popupClassRegistered = true;
        }

        g_popup = CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_TOPMOST, g_popupClassName, L"Mawaqit Prayer Times", WS_POPUP,
                                  0, 0, 270, 252, nullptr, nullptr, wc.hInstance, nullptr);

        if (!g_popup)
        {
            Wh_Log(L"[Mawaqit] Popup creation failed: %lu", GetLastError());
            return;
        }
    }

    LoadMetadata();

    UpdateCountdown();

    HWND taskbar = anchorTaskbar && IsWindow(anchorTaskbar) ? anchorTaskbar : taskwin::FindCurrentProcessTaskbarWnd();
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
    // The GDI viewport maps the typography-aware logical layout to this scaled
    // physical window, scaling fonts, columns and row spacing together.
    const int w = MulDiv(PopupLogicalWidth() * g_popupScale, dpi, 9600);
    const int h = MulDiv(PopupLogicalHeight() * g_popupScale, dpi, 9600);
    const int edgeGap = g_popupLayout == L"attached" ? 0 : MulDiv(11 * g_popupScale, dpi, 9600);
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
            x = bar.left + (int)std::lround((pt.X + anchorPill.ActualWidth() / 2.0) * scale) - w / 2;
        }
        catch (...)
        {
        }
    }
    int styledEdge = 0;
    const bool styledEdgeFound = StyledPopupTaskbarEdge(anchorPill, taskbar, top, styledEdge);
    int y =
        styledEdgeFound ? (top ? styledEdge : styledEdge - h) : (top ? bar.bottom + edgeGap : bar.top - h - edgeGap);
    g_popupAnchorPill = anchorPill;
    g_popupAnchorTaskbar = taskbar;
    if (g_popupLayout == L"attached")
    {
        Wh_Log(L"[Mawaqit][PopupFit] styled=%d edge=%d y=%d top=%d", styledEdgeFound ? 1 : 0, styledEdge, y,
               top ? 1 : 0);
    }
    g_popupAttachedAtTop = g_popupLayout == L"attached" && top;
    x = std::max<int>(monitor.left + screenMargin, std::min<int>(x, monitor.right - w - screenMargin));
    y = std::max<int>(monitor.top + screenMargin, std::min<int>(y, monitor.bottom - h - screenMargin));
    // Apply the final popup geometry before configuring its composition surface
    // so accent backdrops respect the rounded window region. Solid retains
    // their composition backdrops; Transparent uses per-pixel alpha.
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
    SetWindowPos(g_popup, HWND_TOPMOST, x, g_popupStartY, w, h, SWP_SHOWWINDOW | (hover ? SWP_NOACTIVATE : 0));
    const bool attached = g_popupLayout == L"attached";
    const bool adaptive = attached && g_popupAttachedCorners == L"adaptive";
    const int popupRadius = attached && g_popupAttachedCorners == L"square" ? 0 : PopupCornerRadiusPx(w, h, dpi);
    HRGN rounded =
        popupRadius == 0 ? CreateRectRgn(0, 0, w, h) : CreateRoundRectRgn(0, 0, w, h, popupRadius * 2, popupRadius * 2);
    // Square only the two corners adjoining the taskbar. Unioning a narrow
    // rectangular strip preserves the opposite rounded pair without a custom
    // per-pixel region, and works with all existing popup materials.
    if (rounded && adaptive && popupRadius > 0)
    {
        HRGN strip =
            g_popupAttachedAtTop ? CreateRectRgn(0, 0, w, popupRadius) : CreateRectRgn(0, h - popupRadius, w, h);
        if (strip)
        {
            CombineRgn(rounded, rounded, strip, RGN_OR);
            DeleteObject(strip);
        }
    }
    if (rounded)
    {
        if (!SetWindowRgn(g_popup, rounded, TRUE))
        {
            Wh_Log(L"[Mawaqit][PopupCorners] SetWindowRgn failed: %lu", GetLastError());
            DeleteObject(rounded);
        }
        else
            Wh_Log(L"[Mawaqit][Popup] layout=%ls corners=%ls at-top=%d radius=%dpx dpi=%u", g_popupLayout.c_str(),
                   g_popupAttachedCorners.c_str(), g_popupAttachedAtTop ? 1 : 0, popupRadius, dpi);
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
            Wh_Log(L"[Mawaqit] Popup open animation timer failed: %lu; showing at final position", GetLastError());
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
    g_popupLastPeriodicPaint = GetTickCount64();
    SetTimer(g_popup, 1, 50, nullptr);
    // Arm at open; pointer presence refreshes the deadline until the user leaves.
    int autoClose = g_popupAutoCloseSeconds.load();
    g_popupAutoCloseDeadline = autoClose > 0 ? GetTickCount64() + static_cast<ULONGLONG>(autoClose) * 1000 : 0;
    Wh_Log(L"[Mawaqit][Monitor] Popup opened: taskbar=%p anchor=%s x=%d y=%d auto-close=%d seconds", taskbar,
           anchorTaskbar ? L"secondary" : L"primary", x, y, autoClose);

    InvalidateRect(g_popup, nullptr, FALSE);
}

static constexpr wchar_t kSlotName[] = L"MawaqitTaskbarSlot";
static constexpr wchar_t kLeaseName[] = L"MawaqitTaskbarLease";

static FrameworkElement FindNamed(FrameworkElement const &root, wchar_t const *name, int depth = 0)
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
static bool FindApplicationRepeater(FrameworkElement const &root, Panel &parent, FrameworkElement &repeater,
                                    int depth = 0)
{
    if (!root || depth > 24)
        return false;
    // Windows 11 taskbar applications are hosted by TaskbarFrameRepeater.
    // Only lease beside a direct child of a supported panel.
    if (auto panel = root.try_as<Panel>())
    {
        for (auto const &child : panel.Children())
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
        Wh_Log(L"[Mawaqit] Center skipped: root Grid width %.1f vs taskbar %.1f DIP", grid.ActualWidth(), taskbarWidth);
        return nullptr;
    }
    Panel iconsParent{nullptr};
    FrameworkElement repeater{nullptr};
    auto tray = FindNamed(root, L"SystemTrayFrameGrid");
    if (!tray || tray.ActualWidth() <= 0 || !FindApplicationRepeater(root, iconsParent, repeater) || !repeater ||
        repeater.ActualWidth() <= 0)
    {
        Wh_Log(L"[Mawaqit] Center waiting for application/tray layout");
        return nullptr;
    }
    return grid;
}

static Panel ResolvePositionParent(int &slotIndex)
{
    slotIndex = -1;
    if (g_position == L"before-icons" || g_position == L"before-start")
    {
        // The stock taskbar repeater occupies a star-sized Grid column. A
        // leased Auto column changes its layout and can displace the entire
        // taskbar. Anchor visually to the first realized button instead.
        auto root = LiveTaskbarRoot();
        auto grid = root.try_as<Grid>();
        Panel iconsParent{nullptr};
        FrameworkElement repeater{nullptr};
        if (grid && grid.ActualWidth() > 0 && FindApplicationRepeater(root, iconsParent, repeater) && repeater &&
            repeater.ActualWidth() > 0)
        {
            slotIndex = 0;
            return grid;
        }
        return nullptr;
    }
    if (g_position == L"after-icons")
    {
        // Unlike before-icons, the repeater's column spans nearly the whole
        // taskbar. Use a non-layout-affecting overlay anchored to realized icons.
        auto root = LiveTaskbarRoot();
        auto grid = root.try_as<Grid>();
        Panel iconsParent{nullptr};
        FrameworkElement repeater{nullptr};
        if (grid && grid.ActualWidth() > 0 && FindApplicationRepeater(root, iconsParent, repeater) && repeater &&
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
        Wh_Log(L"[Mawaqit] After-tray anchor unavailable; named children: %s", slot::DescribeChildren(tray).c_str());
        return nullptr;
    }
    return nullptr;
}

// Measure the actual button descendants, never the full-width ItemsRepeater host.
// Layout-only containers can extend to the tray even when no button occupies
// that region. A missing measurement is reported as unavailable, not guessed.
static void MeasureTaskbarButtonBounds(FrameworkElement const &node, Grid const &grid, double &right,
                                       unsigned &measured, int depth = 0)
{
    if (!node || depth > 12 || node.Visibility() != Visibility::Visible)
        return;
    auto name = std::wstring(node.Name());
    // Taskbar buttons are represented by named button roots across Windows 11
    // builds. Exclude full-width panels, presenters, and repeater placeholders.
    bool button =
        name.find(L"TaskbarButton") != std::wstring::npos || name.find(L"TaskListButton") != std::wstring::npos;
    if (button && node.ActualWidth() > 8.0 && node.ActualWidth() < 200.0)
    {
        try
        {
            auto pos = node.TransformToVisual(grid).TransformPoint({0, 0});
            if (std::isfinite(pos.X) && pos.X >= 0 && pos.X + node.ActualWidth() <= grid.ActualWidth() + 1.0)
            {
                right = std::max(right, pos.X + node.ActualWidth());
                ++measured;
                return; // Do not count nested content inside the same button.
            }
        }
        catch (...)
        {
        }
    }
    int count = VisualTreeHelper::GetChildrenCount(node);
    for (int i = 0; i < count; ++i)
    {
        auto child = VisualTreeHelper::GetChild(node, i).try_as<FrameworkElement>();
        if (child)
            MeasureTaskbarButtonBounds(child, grid, right, measured, depth + 1);
    }
}

// Find the leftmost native taskbar button, including Start.
// before-start selects the whole group; before-icons selects applications only.
static void FindBeforeIconsTargets(FrameworkElement const &node, Grid const &grid, FrameworkElement &first,
                                   double &firstLeft, double &launchRight, unsigned &launchCount, int depth = 0)
{
    if (!node || depth > 24 || node.Visibility() != Visibility::Visible)
        return;
    auto name = std::wstring(node.Name());
    auto type = std::wstring(winrt::get_class_name(node));
    bool app = name.find(L"TaskbarButton") != std::wstring::npos || name.find(L"TaskListButton") != std::wstring::npos;
    bool launch =
        name.find(L"LaunchListButton") != std::wstring::npos || type.find(L"LaunchListButton") != std::wstring::npos;
    if ((app || launch) && node.ActualWidth() > 8.0)
    {
        try
        {
            auto pos = node.TransformToVisual(grid).TransformPoint({0, 0});
            double x = static_cast<double>(pos.X);
            double right = x + node.ActualWidth();
            if (std::isfinite(x) && x >= 0 && right <= grid.ActualWidth() + 1.0)
            {
                if (launch)
                {
                    launchRight = std::max(launchRight, right);
                    ++launchCount;
                }
                if ((g_position == L"before-start" || !launch) && x < firstLeft)
                {
                    firstLeft = x;
                    first = node;
                }
                return;
            }
        }
        catch (...)
        {
        }
    }
    int count = VisualTreeHelper::GetChildrenCount(node);
    for (int i = 0; i < count; ++i)
    {
        auto child = VisualTreeHelper::GetChild(node, i).try_as<FrameworkElement>();
        if (child)
            FindBeforeIconsTargets(child, grid, first, firstLeft, launchRight, launchCount, depth + 1);
    }
}

// With no apps to reserve against, both icon-relative positions share the
// empty application area's leading edge. Re-measure apps on every update so
// their normal reservation/anchor takes over when the first app appears.
static void PlacePillAfterLaunchGroup(Grid const &grid, Grid const &pill, double launchRight)
{
    double width = pill.ActualWidth() > 0 ? pill.ActualWidth() : pill.Width();
    auto tray = FindNamed(grid, L"SystemTrayFrameGrid");
    if (!tray || !std::isfinite(width) || width <= 0)
    {
        pill.Visibility(Visibility::Collapsed);
        return;
    }
    double rightLimit = tray.TransformToVisual(grid).TransformPoint({0, 0}).X;
    double left = launchRight + 5.0;
    bool room = left >= 0 && left + width + 5.0 <= rightLimit;
    if (pill.Visibility() != (room ? Visibility::Visible : Visibility::Collapsed))
        pill.Visibility(room ? Visibility::Visible : Visibility::Collapsed);
    if (!room)
        return;
    auto translation = pill.RenderTransform().try_as<TranslateTransform>();
    if (!translation)
    {
        translation = TranslateTransform();
        pill.RenderTransform(translation);
    }
    double centered = (grid.ActualWidth() - width) / 2.0;
    double visualOffset = g_attachedPill ? 0.0
                                         : static_cast<double>(g_margins.left - g_margins.right) +
                                               (g_position == L"before-icons" ? 5.0 : 0.0);
    double offset = left - centered - visualOffset;
    if (std::abs(translation.X() - offset) > 0.01)
        translation.X(offset);
}

// Reversible margin on the leftmost taskbar button; restore on target change
// and through the existing unload/rebuild path.
static winrt::weak_ref<FrameworkElement> g_beforeIconsReservedRepeater;
static Thickness g_beforeIconsOriginalMargin{};
static double g_beforeIconsReservedWidth = 0.0;

static void ReleaseBeforeIconsReservation()
{
    auto repeater = g_beforeIconsReservedRepeater.get();
    if (repeater)
    {
        try
        {
            repeater.Margin(g_beforeIconsOriginalMargin);
        }
        catch (...)
        {
            Wh_Log(L"[Mawaqit] Before-icons reservation restore failed");
        }
    }
    g_beforeIconsReservedRepeater = {};
    g_beforeIconsReservedWidth = 0.0;
}

static bool EnsureBeforeIconsReservation(FrameworkElement const &repeater, double requestedWidth)
{
    if (!repeater || requestedWidth <= 0.0 || requestedWidth > 500.0)
        return false;
    auto previous = g_beforeIconsReservedRepeater.get();
    if (previous && previous != repeater)
        ReleaseBeforeIconsReservation();
    if (!g_beforeIconsReservedRepeater.get())
    {
        g_beforeIconsOriginalMargin = repeater.Margin();
        g_beforeIconsReservedWidth = 0.0;
        g_beforeIconsReservedRepeater = repeater;
    }
    if (std::abs(g_beforeIconsReservedWidth - requestedWidth) > 0.5)
    {
        auto margin = g_beforeIconsOriginalMargin;
        margin.Left += requestedWidth;
        repeater.Margin(margin);
        g_beforeIconsReservedWidth = requestedWidth;
        Wh_Log(L"[Mawaqit] Before-icons reserved %.1f DIP on leftmost taskbar button margin "
               L"(original L=%.1f R=%.1f)",
               requestedWidth, g_beforeIconsOriginalMargin.Left, g_beforeIconsOriginalMargin.Right);
    }
    return true;
}

static void UpdateBeforeIconsOnUiThread()
{
    if ((g_position != L"before-icons" && g_position != L"before-start") || !g_pill)
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
        double firstLeft = grid.ActualWidth();
        double launchRight = 0.0;
        unsigned launchCount = 0;
        FrameworkElement first{nullptr};
        FindBeforeIconsTargets(root, grid, first, firstLeft, launchRight, launchCount);
        double pillWidth = g_pill.ActualWidth();
        if (pillWidth <= 0)
            pillWidth = g_pill.Width();
        if (!first && launchCount > 0 && g_position == L"before-icons")
        {
            ReleaseBeforeIconsReservation();
            PlacePillAfterLaunchGroup(grid, g_pill, launchRight);
            return;
        }
        if (!first || !std::isfinite(pillWidth) || pillWidth <= 0 || launchCount == 0)
        {
            ReleaseBeforeIconsReservation();
            g_pill.Visibility(Visibility::Collapsed);
            return;
        }
        double centeredLeft = (grid.ActualWidth() - pillWidth) / 2.0;
        // Reserve ahead of the selected first button (Start or application), leaving
        // the pill inside the taskbar with a gap on both sides. XAML
        // lays out the new margin before the next update measures the slot.
        double requestedWidth = pillWidth + 10.0;
        if (g_beforeIconsReservedRepeater.get() != first || std::abs(g_beforeIconsReservedWidth - requestedWidth) > 0.5)
        {
            if (!EnsureBeforeIconsReservation(first, requestedWidth))
            {
                g_pill.Visibility(Visibility::Collapsed);
                return;
            }
            // Finish this reservation's layout before presenting the frame.
            // The same pill remains attached; do not hide it for a whole tick.
            grid.UpdateLayout();
            firstLeft = grid.ActualWidth();
            launchRight = 0.0;
            launchCount = 0;
            first = nullptr;
            FindBeforeIconsTargets(root, grid, first, firstLeft, launchRight, launchCount);
            if (!first || first != g_beforeIconsReservedRepeater.get() || launchCount == 0)
            {
                g_pill.Visibility(Visibility::Collapsed);
                return;
            }
        }
        double desiredLeft = firstLeft - pillWidth - 5.0;
        // Never draw outside the taskbar; log when Windows has not yet
        // completed the layout or cannot provide sufficient room.
        double visualOffset = g_attachedPill ? 0.0 : static_cast<double>(g_margins.left - g_margins.right) + 5.0;
        double visibleLeft = desiredLeft;
        double minimumLeft = g_position == L"before-start" ? 5.0 : launchRight + 5.0;
        bool room = visibleLeft >= minimumLeft && visibleLeft + pillWidth <= firstLeft - 5.0;
        if (g_pill.Visibility() != (room ? Visibility::Visible : Visibility::Collapsed))
            g_pill.Visibility(room ? Visibility::Visible : Visibility::Collapsed);
        if (room)
        {
            auto transform = g_pill.RenderTransform().try_as<TranslateTransform>();
            if (!transform)
            {
                transform = TranslateTransform();
                g_pill.RenderTransform(transform);
            }
            if (std::abs(transform.X() - (desiredLeft - centeredLeft - visualOffset)) > 0.01)
                transform.X(desiredLeft - centeredLeft - visualOffset);
        }
    }
    catch (...)
    {
        Wh_Log(L"[Mawaqit] Before-icons measurement unavailable; retrying");
    }
}

// Keep the overlay centered until the live application repeater reaches it,
// then translate right, stopping before the tray. Windows icon layout and
// overflow behavior remain unchanged.
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
        MeasureTaskbarButtonBounds(repeater, grid, appRight, measuredButtons);
        // An empty application list is valid (for example on a desktop with
        // no pinned/running apps). Measure the native launch group instead of
        // treating zero apps as an unfinished layout. Never use repeater width.
        if (measuredButtons == 0)
        {
            FrameworkElement first{nullptr};
            double firstLeft = grid.ActualWidth();
            double launchRight = 0.0;
            unsigned launchCount = 0;
            FindBeforeIconsTargets(root, grid, first, firstLeft, launchRight, launchCount);
            if (launchCount == 0)
            {
                g_pill.Visibility(Visibility::Collapsed);
                return; // Neither apps nor launch buttons have valid bounds yet.
            }
            appRight = launchRight;
        }
        double gap = 5.0;
        double maximumLeft = trayOrigin.X - pillWidth - gap;
        double wantedLeft = std::max(centeredLeft, appRight + gap);
        // If the available region is exhausted, hide rather than overlap the
        // system tray or the application buttons. This does not force overflow.
        bool room = maximumLeft >= centeredLeft && wantedLeft <= maximumLeft;
        if (g_pill.Visibility() != (room ? Visibility::Visible : Visibility::Collapsed))
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
            if (std::abs(translation.X() - (left - centeredLeft)) > 0.01)
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
        MeasureTaskbarButtonBounds(repeater, grid, appRight, measured);
        if (measured == 0)
        {
            FrameworkElement first{nullptr};
            double firstLeft = grid.ActualWidth();
            double launchRight = 0.0;
            unsigned launches = 0;
            FindBeforeIconsTargets(root, grid, first, firstLeft, launchRight, launches);
            if (launches > 0)
                PlacePillAfterLaunchGroup(grid, g_pill, launchRight);
            else
                g_pill.Visibility(Visibility::Collapsed);
            return;
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
        if (g_pill.Visibility() != (room ? Visibility::Visible : Visibility::Collapsed))
            g_pill.Visibility(room ? Visibility::Visible : Visibility::Collapsed);
        if (room)
        {
            auto transform = g_pill.RenderTransform().try_as<TranslateTransform>();
            if (!transform)
            {
                transform = TranslateTransform();
                g_pill.RenderTransform(transform);
            }
            if (std::abs(transform.X() - (desiredLeft - centeredLeft)) > 0.01)
                transform.X(desiredLeft - centeredLeft);
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
    winrt::weak_ref<Viewbox> prayerIcon;
    std::wstring renderedPrayerIcon;
    winrt::weak_ref<TextBlock> countdown;
    winrt::weak_ref<TextBlock> iqamah;
    winrt::weak_ref<TextBlock> hijri;
    winrt::weak_ref<TextBlock> iqamahShadow;
    winrt::weak_ref<TextBlock> prayerShadow;
    winrt::weak_ref<TextBlock> countdownShadow;
    slot::Lease lease{};
    // Each secondary owns its selected button reservation independently of the primary.
    winrt::weak_ref<FrameworkElement> reservedBeforeTarget;
    Thickness originalBeforeMargin{};
    double reservedBeforeWidth = 0.0;
    winrt::event_token tapToken{};
    bool tapRegistered = false;
    winrt::event_token hoverToken{};
    bool hoverRegistered = false;
    winrt::event_token hoverExitToken{}, hoverPressToken{};
    bool hoverExitRegistered = false, hoverPressRegistered = false;
};
static std::vector<SecondaryClockInstance> g_secondaryClocks;
static constexpr wchar_t kSecondarySlotName[] = L"MawaqitSecondaryClockSlot";
static constexpr wchar_t kSecondaryLeaseName[] = L"MawaqitSecondaryClockLease";
// Defined below, shared by primary and secondary taskbar pill creation.
static double PillSlotHeight(HWND taskbar);
static void RefreshAttachedFitsOnUiThread();
struct AttachedFit
{
    double height;
    double offsetY;
    bool detected;
};
static AttachedFit ResolveAttachedFit(Panel const &insertionParent, HWND taskbar, wchar_t const *monitorLabel);

// The shadow is a second, non-hit-testable TextBlock underneath the actual text.
// It is offset by 1 DIP, with no effect on the foreground color or pill opacity.
static Grid MakePillTextWithShadow(TextBlock const &foreground, TextBlock &shadow)
{
    Grid layer;
    layer.VerticalAlignment(VerticalAlignment::Center);
    shadow = TextBlock();
    shadow.FontSize(foreground.FontSize());
    shadow.FontWeight(foreground.FontWeight());
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

static winrt::Windows::UI::Color ResolvePillIqamahColor(winrt::Windows::UI::Color countdownColor)
{
    // Accent 2 is available for the expanded theme families and SynthWave.
    // Preserve legacy appearance for Windows Default, Custom, and untouched themes.
    if (EffectiveTheme() == L"synthwave-84")
        return Rgb(kSynthWave.secondary);
    ExtendedThemeColors extended{};
    if (TryExtendedThemeColors(EffectiveTheme(), extended))
        return Rgb(extended.secondary);
    return countdownColor;
}

static TextBlock MakeIqamahText(winrt::Windows::UI::Color countdownColor)
{
    TextBlock iqamah;
    iqamah.FontSize((double)g_fontSize * g_iqamahFontScale / 100.0);
    iqamah.FontWeight(winrt::Windows::UI::Text::FontWeight{static_cast<uint16_t>(g_pillFontWeight)});
    iqamah.VerticalAlignment(VerticalAlignment::Center);
    iqamah.Margin({3, 0, 5, 0});
    countdownColor = ResolvePillIqamahColor(countdownColor);
    unsigned rgb;
    if (TryThemeOverride(L"ThemeColorOverrides.Pill.Iqamah", rgb))
        countdownColor = Rgb(rgb);
    SolidColorBrush brush;
    brush.Color(countdownColor);
    iqamah.Foreground(brush);
    return iqamah;
}

// Taskbar-only Lucide geometry, in 24x24 coordinates. The popup renderer is untouched.
static std::wstring PillPrayerIconGeometry()
{
    if (g_nextPrayerIconStyle == L"mosque")
        // Prayer arch (preview E), on the shared 24-unit icon canvas.
        return L"M5 21 V13 C5 8 10 7 12 3 C14 7 19 8 19 13 V21 "
               L"M8 21 V14 C8 11 11 10 12 8 C13 10 16 11 16 14 V21 M4 21 H20";
    // Labels include scheduled time (e.g. "Maghrib 18:38").
    std::wstring name = g_label;
    if (name.find(L"Fajr") != std::wstring::npos)
        return L"M12 2 V10 M8 6 L12 2 L16 6 M4.93 10.93 L6.34 12.34 M2 18 H4 M20 18 H22 M19.07 10.93 L17.66 12.34 M22 "
               L"22 H2 M8 18 A4 4 0 0 1 16 18";
    if (name.find(L"Dhuhr") != std::wstring::npos)
        return L"M12 2 V4 M12 20 V22 M4.93 4.93 L6.34 6.34 M17.66 17.66 L19.07 19.07 M2 12 H4 M20 12 H22 M4.93 19.07 "
               L"L6.34 17.66 M17.66 6.34 L19.07 4.93 M16 12 A4 4 0 1 1 8 12 A4 4 0 1 1 16 12";
    if (name.find(L"Asr") != std::wstring::npos)
        return L"M12 4 V6 M12 18 V20 M4 12 H6 M18 12 H20 M6.34 6.34 L7.76 7.76 M16.24 16.24 L17.66 17.66 M6.34 17.66 "
               L"L7.76 16.24 M16.24 7.76 L17.66 6.34 M16 12 A4 4 0 1 1 8 12 A4 4 0 1 1 16 12";
    if (name.find(L"Maghrib") != std::wstring::npos)
        return L"M12 2 V10 M8 6 L12 10 L16 6 M4.93 10.93 L6.34 12.34 M2 18 H4 M20 18 H22 M19.07 10.93 L17.66 12.34 M22 "
               L"22 H2 M8 18 A4 4 0 0 1 16 18";
    if (name.find(L"Isha") != std::wstring::npos)
        return L"M20.985 12.486 C19.2 18 16 21 11.514 21 C6.25 21 3 17.25 3 12 C3 7.514 6 4.3 11.514 3.015 C9.2 5.6 9 "
               L"8 9 9 C9 12.5 11.5 15 15 15 C17 15 19 14 20.985 12.486";
    return L"";
}

// The arch occupies more of its canvas than the prayer-period symbols. Keep its
// painted height close to the text's capital height rather than the line box.
static double PillPrayerIconSize()
{
    return g_nextPrayerIconStyle == L"mosque" ? (double)g_fontSize * 0.9 : (double)g_fontSize + 3.0;
}

static winrt::Windows::UI::Xaml::Controls::Viewbox MakePillPrayerIcon(winrt::Windows::UI::Color color)
{
    using namespace winrt::Windows::UI::Xaml::Markup;
    using namespace winrt::Windows::UI::Xaml::Shapes;
    color = OverridePillColor(L"ThemeColorOverrides.Pill.Icon", color);
    Viewbox view;
    view.Width(PillPrayerIconSize());
    view.Height(PillPrayerIconSize());
    view.VerticalAlignment(VerticalAlignment::Center);
    view.Margin({5, 0, 0, 0});
    // TextBlock centers its line box, whose visible glyphs sit slightly lower.
    // Correct the optical center without changing layout height or spacing.
    TranslateTransform opticalOffset;
    opticalOffset.Y((double)g_fontSize * 0.08 - 1.0);
    view.RenderTransform(opticalOffset);
    Canvas canvas;
    canvas.Width(24);
    canvas.Height(24);
    Path path;
    // XAML's path mini-language parses the same geometry as the Lucide source.
    auto markup = L"<Path xmlns='http://schemas.microsoft.com/winfx/2006/xaml/presentation' Data='" +
                  PillPrayerIconGeometry() + L"' />";
    path = XamlReader::Load(markup).as<Path>();
    SolidColorBrush ink;
    ink.Color(color);
    path.Stroke(ink);
    path.StrokeThickness(2.0);
    path.StrokeStartLineCap(PenLineCap::Round);
    path.StrokeEndLineCap(PenLineCap::Round);
    path.StrokeLineJoin(PenLineJoin::Round);
    path.Fill(nullptr);
    canvas.Children().Append(path);
    view.Child(canvas);
    return view;
}

static void RefreshPillPrayerIcon(Viewbox const &icon, winrt::Windows::UI::Color color)
{
    if (!icon)
        return;
    auto replacement = MakePillPrayerIcon(color);
    auto child = replacement.Child();
    replacement.Child(nullptr);
    icon.Child(nullptr);
    icon.Child(child);
}

static std::wstring PillPrayerNameText()
{
    return g_showNext && g_nextPrayerDisplay != L"icon" ? g_label : L"";
}

// Auto width is measured in XAML DIPs on the taskbar UI thread. Use representative
// maximum-length countdown and Iqamah strings so the pill does not jitter each second.
// Fixed widths bypass this calculation entirely.
static double EffectivePillWidth()
{
    if (g_width != 0)
        return (double)g_width;

    auto measure = [](std::wstring const &value, double fontSize) -> double {
        TextBlock sample;
        sample.FontSize(fontSize);
        sample.FontWeight(winrt::Windows::UI::Text::FontWeight{static_cast<uint16_t>(g_pillFontWeight)});
        sample.Text(value);
        sample.Measure(winrt::Windows::Foundation::Size{10000.0f, 1000.0f});
        return sample.DesiredSize().Width;
    };

    double width = (double)g_padding.left + g_padding.right;
    if (g_showNext)
    {
        if (g_nextPrayerDisplay != L"icon")
            width += measure(g_label.size() > 7 ? g_label : L"Maghrib", (double)g_fontSize) + 9.0;
        if (g_showNext && g_nextPrayerDisplay != L"name")
            width += PillPrayerIconSize() + 5.0;
    }
    if (g_theme == L"custom" && g_showNext && g_showCountdown)
        width += measure(L"|", (double)g_fontSize);
    if (g_showCountdown)
    {
        width += measure(L"-23:59:59", (double)g_fontSize) + 9.0;
        if (g_showIqamah)
            width += measure(L"+180", (double)g_fontSize * g_iqamahFontScale / 100.0) + 8.0;
    }
    if (!PillHijriDate().empty())
        width += measure(PillHijriDate(), (double)g_fontSize) + 9.0;
    // Leave a small allowance for glyph rounding, borders, and the shadow offset.
    return std::max(32.0, std::ceil(width + 4.0));
}

static void ReleaseSecondaryBeforeReservation(SecondaryClockInstance &instance)
{
    if (auto target = instance.reservedBeforeTarget.get())
    {
        try
        {
            target.Margin(instance.originalBeforeMargin);
        }
        catch (...)
        {
            Wh_Log(L"[Mawaqit][Monitor] Button reservation restore failed on %s", instance.monitor.c_str());
        }
    }
    instance.reservedBeforeTarget = {};
    instance.reservedBeforeWidth = 0.0;
}

static void UpdateSecondaryBeforePosition(SecondaryClockInstance &instance)
{
    auto pill = instance.pill.get();
    auto parent = instance.parent.get();
    auto grid = parent ? parent.try_as<Grid>() : nullptr;
    if (!pill || !grid || grid.ActualWidth() <= 0)
        return;
    try
    {
        FrameworkElement first{nullptr};
        double firstLeft = grid.ActualWidth();
        double launchRight = 0.0;
        unsigned launches = 0;
        FindBeforeIconsTargets(grid, grid, first, firstLeft, launchRight, launches);
        double width = pill.ActualWidth() > 0 ? pill.ActualWidth() : pill.Width();
        double requested = width + 10.0;
        if (!first && launches > 0 && g_position == L"before-icons")
        {
            ReleaseSecondaryBeforeReservation(instance);
            PlacePillAfterLaunchGroup(grid, pill, launchRight);
            return;
        }
        if (!first || launches == 0 || !std::isfinite(width) || width <= 0 || requested > 500.0)
        {
            pill.Visibility(Visibility::Collapsed);
            ReleaseSecondaryBeforeReservation(instance);
            return;
        }
        if (instance.reservedBeforeTarget.get() != first)
        {
            ReleaseSecondaryBeforeReservation(instance);
            instance.originalBeforeMargin = first.Margin();
            instance.reservedBeforeTarget = first;
        }
        if (std::abs(instance.reservedBeforeWidth - requested) > 0.5)
        {
            auto margin = instance.originalBeforeMargin;
            margin.Left += requested;
            first.Margin(margin);
            instance.reservedBeforeWidth = requested;
            grid.UpdateLayout();
            firstLeft = grid.ActualWidth();
            launchRight = 0.0;
            launches = 0;
            first = nullptr;
            FindBeforeIconsTargets(grid, grid, first, firstLeft, launchRight, launches);
            if (!first || first != instance.reservedBeforeTarget.get() || launches == 0)
            {
                pill.Visibility(Visibility::Collapsed);
                return;
            }
        }
        double desired = firstLeft - width - 5.0;
        double minimumLeft = g_position == L"before-start" ? 5.0 : launchRight + 5.0;
        bool room = desired >= minimumLeft && desired + width <= firstLeft - 5.0;
        if (pill.Visibility() != (room ? Visibility::Visible : Visibility::Collapsed))
            pill.Visibility(room ? Visibility::Visible : Visibility::Collapsed);
        if (room)
        {
            auto translation = pill.RenderTransform().try_as<TranslateTransform>();
            if (!translation)
            {
                translation = TranslateTransform();
                pill.RenderTransform(translation);
            }
            double centered = (grid.ActualWidth() - width) / 2.0;
            double visualOffset = g_attachedPill ? 0.0 : static_cast<double>(g_margins.left - g_margins.right) + 5.0;
            if (std::abs(translation.X() - (desired - centered - visualOffset)) > 0.01)
                translation.X(desired - centered - visualOffset);
        }
    }
    catch (...)
    {
        pill.Visibility(Visibility::Collapsed);
        Wh_Log(L"[Mawaqit][Monitor] %s layout retry on %s", g_position.c_str(), instance.monitor.c_str());
    }
}

static void RemoveSecondaryClocksOnUiThread()
{
    CancelHoverOpen();
    for (auto &instance : g_secondaryClocks)
    {
        ReleaseSecondaryBeforeReservation(instance);
        auto pill = instance.pill.get();
        if (pill)
        {
            if (instance.hoverExitRegistered) { try { pill.PointerExited(instance.hoverExitToken); } catch (...) {} instance.hoverExitRegistered = false; }
            if (instance.hoverPressRegistered) { try { pill.PointerPressed(instance.hoverPressToken); } catch (...) {} instance.hoverPressRegistered = false; }
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
static bool SecondaryClockAttachedOnUiThread(SecondaryClockInstance const &instance)
{
    if (!IsWindow(instance.taskbar))
        return false;
    auto parent = instance.parent.get();
    auto pill = instance.pill.get();
    if (!parent || !pill)
        return false;
    try
    {
        for (auto const &child : parent.Children())
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
    for (auto const &instance : g_secondaryClocks)
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
    for (auto const &record : mawaqit_monitor_discovery::EnumerateTaskbars())
    {
        if (record.primaryTaskbar || !IsWindow(record.hwnd))
            continue;
        bool existing = false;
        for (auto const &instance : g_secondaryClocks)
            if (instance.taskbar == record.hwnd && SecondaryClockAttachedOnUiThread(instance))
            {
                existing = true;
                break;
            }
        if (existing)
            continue;
        // Resolve the secondary XAML tree without development-only hierarchy dumps.
        auto xamlRoot = taskxaml::GetSecondaryTaskbarXamlRoot(record.hwnd);
        auto root = xamlRoot ? xamlRoot.Content().try_as<FrameworkElement>() : nullptr;
        auto trayElement = root ? FindNamed(root, L"SystemTrayFrameGrid") : nullptr;
        auto parent = trayElement ? trayElement.try_as<Panel>() : nullptr;
        auto kind = parent ? slot::Classify(parent) : slot::Kind::Unsupported;
        if (!parent || kind == slot::Kind::Unsupported)
        {
            continue;
        }
        // Both before positions reserve on their selected native button.
        // Use the full-width root for the overlay; do not lease a Grid column.
        bool overlay = g_position == L"center" || g_position == L"after-icons" || g_position == L"before-start" ||
                       g_position == L"before-icons";
        int index = -1;
        if (overlay)
        {
            auto rootGrid = root.try_as<Grid>();
            RECT bar{};
            double expectedWidth = 0;
            if (GetWindowRect(record.hwnd, &bar))
                expectedWidth = (bar.right - bar.left) * 96.0 / std::max(96u, GetDpiForWindow(record.hwnd));
            if (!rootGrid || expectedWidth <= 0 || rootGrid.ActualWidth() < expectedWidth * 0.75)
            {
                Wh_Log(L"[Mawaqit][Monitor] Overlay awaiting full-width secondary root: %s", record.device.c_str());
                continue;
            }
            parent = rootGrid;
        }
        else
        {
            slot::Anchor anchor =
                g_position == L"after-tray" ? slot::Anchor::AfterShowDesktop : slot::Anchor::BeforeIcons;
            if (!slot::ResolveSlot(parent, anchor, index))
            {
                Wh_Log(L"[Mawaqit][Monitor] Anchor %s unavailable on %s; using before-icons", g_position.c_str(),
                       record.device.c_str());
                anchor = slot::Anchor::BeforeIcons;
                if (!slot::ResolveSlot(parent, anchor, index))
                    continue;
            }
        }
        Wh_Log(L"[Mawaqit][Monitor] Secondary position: monitor=%s requested=%s mode=%s slot=%d", record.device.c_str(),
               g_position.c_str(), overlay ? L"overlay" : L"lease", index);
        const auto attachedFit = ResolveAttachedFit(parent, record.hwnd, record.device.c_str());
        SecondaryClockInstance instance{};
        instance.taskbar = record.hwnd;
        instance.monitor = record.device;
        if (!overlay && !slot::AcquireAt(parent, index, kSecondaryLeaseName, instance.lease))
            continue;
        try
        {
            Grid box;
            box.Name(kSecondarySlotName);
            box.Width(EffectivePillWidth());
            box.Height(attachedFit.height);
            box.VerticalAlignment(VerticalAlignment::Center);
            Border surface;
            auto palette = ResolvePillPalette(EffectiveTheme());
            surface.Background(CreatePillBackground(palette.background));
            surface.BorderThickness(Thickness{0, 0, 0, 0});
            surface.BorderBrush(nullptr);
            if (g_showPillBorder)
            {
                unsigned int borderRgb;
                auto borderColor = palette.countdown;
                if (TryThemeOverride(L"ThemeColorOverrides.Pill.Border", borderRgb))
                    borderColor = Rgb(borderRgb);
                surface.BorderBrush(CreatePillBorderBrush(borderColor));
                surface.BorderThickness(g_attachedPill ? Thickness{1, 0, 1, 0} : Thickness{1, 1, 1, 1});
            }
            double r = g_attachedPill ? 0.0 : (double)g_cornerRadius;
            surface.CornerRadius({r, r, r, r});
            // Match the primary pill's visual margins and position corrections.
            // Keep the native secondary taskbar slot unchanged.
            double positionOffsetX = 0.0;
            if (g_position == L"after-tray")
                positionOffsetX = -5.0;
            else if (g_position == L"before-icons" || g_position == L"before-start")
                positionOffsetX = 5.0;
            TranslateTransform offset;
            offset.X(g_attachedPill ? 0.0 : (double)(g_margins.left - g_margins.right) + positionOffsetX);
            offset.Y(g_attachedPill ? attachedFit.offsetY : (double)(g_margins.top - g_margins.bottom));
            surface.RenderTransform(offset);
            surface.Padding({(double)g_padding.left, g_attachedPill ? 0.0 : (double)g_padding.top,
                             (double)g_padding.right, g_attachedPill ? 0.0 : (double)g_padding.bottom});
            StackPanel content;
            content.Orientation(Orientation::Horizontal);
            content.HorizontalAlignment(HorizontalAlignment::Center);
            content.VerticalAlignment(VerticalAlignment::Center);
            TextBlock prayer;
            prayer.Name(L"MawaqitSecondaryPrayer");
            prayer.FontSize((double)g_fontSize);
            prayer.FontWeight(winrt::Windows::UI::Text::FontWeight{static_cast<uint16_t>(g_pillFontWeight)});
            prayer.Margin({5, 0, 4, 0});
            prayer.VerticalAlignment(VerticalAlignment::Center);
            SolidColorBrush fg;
            fg.Color(palette.foreground);
            prayer.Foreground(fg);
            TextBlock countdown;
            countdown.Name(L"MawaqitSecondaryCountdown");
            countdown.FontSize((double)g_fontSize);
            countdown.FontWeight(winrt::Windows::UI::Text::FontWeight{static_cast<uint16_t>(g_pillFontWeight)});
            countdown.Margin({4, 0, 5, 0});
            countdown.VerticalAlignment(VerticalAlignment::Center);
            SolidColorBrush accent;
            accent.Color(palette.countdown);
            countdown.Foreground(accent);
            TextBlock iqamah = MakeIqamahText(palette.countdown);
            iqamah.Name(L"MawaqitSecondaryIqamah");
            TextBlock iqamahShadow{nullptr};
            TextBlock prayerShadow{nullptr};
            TextBlock countdownShadow{nullptr};
            // Hidden components must not reserve margins or shadow-layer space.
            prayer.Visibility(g_showNext ? Visibility::Visible : Visibility::Collapsed);
            countdown.Visibility(g_showCountdown ? Visibility::Visible : Visibility::Collapsed);
            iqamah.Visibility((g_showCountdown && g_showIqamah) ? Visibility::Visible : Visibility::Collapsed);
            Viewbox prayerIcon{nullptr};
            if (g_showNext && g_nextPrayerDisplay != L"name")
            {
                prayerIcon = MakePillPrayerIcon(palette.foreground);
                prayerIcon.Name(L"MawaqitSecondaryPrayerIcon");
                prayerIcon.Visibility(g_showNext ? Visibility::Visible : Visibility::Collapsed);
                content.Children().Append(prayerIcon);
                if (g_nextPrayerDisplay == L"icon")
                    prayer.Visibility(Visibility::Collapsed);
            }
            if (g_pillTextShadow)
            {
                auto layer = MakePillTextWithShadow(prayer, prayerShadow);
                layer.Visibility((g_showNext && g_nextPrayerDisplay != L"icon") ? Visibility::Visible
                                                                                : Visibility::Collapsed);
                content.Children().Append(layer);
            }
            else
                content.Children().Append(prayer);
            if (g_theme == L"custom" && g_showNext && g_showCountdown)
            {
                TextBlock separator;
                separator.Text(L"|");
                separator.FontSize((double)g_fontSize);
                separator.FontWeight(winrt::Windows::UI::Text::FontWeight{static_cast<uint16_t>(g_pillFontWeight)});
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
            TextBlock hijri;
            hijri.Name(L"MawaqitSecondaryHijri");
            hijri.FontSize((double)g_fontSize);
            hijri.FontWeight(winrt::Windows::UI::Text::FontWeight{static_cast<uint16_t>(g_pillFontWeight)});
            hijri.VerticalAlignment(VerticalAlignment::Center);
            hijri.Margin({4, 0, 5, 0});
            hijri.Foreground(SolidColorBrush(OverridePillColor(L"ThemeColorOverrides.Pill.HijriDate", palette.foreground)));
            hijri.Text(PillHijriDate());
            hijri.Visibility(PillHijriDate().empty() ? Visibility::Collapsed : Visibility::Visible);
            content.Children().Append(hijri);
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
                if (g_position == L"before-start" || g_position == L"before-icons")
                    box.Visibility(Visibility::Collapsed);
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
            instance.tapToken = box.Tapped([secondaryTaskbar = record.hwnd](auto const &sender, auto const &) {
                CancelHoverOpen();
                if (auto pill = sender.template try_as<Grid>())
                    TogglePrayerPopup(pill, secondaryTaskbar, false);
            });
            instance.tapRegistered = true;
            instance.hoverToken =
                box.PointerEntered([secondaryTaskbar = record.hwnd](auto const &sender, auto const &) {
                    if (g_openPopupOnHover)
                        if (auto pill = sender.template try_as<Grid>())
                            ScheduleHoverOpen(pill, secondaryTaskbar);
                });
            instance.hoverRegistered = true;
            instance.hoverExitToken = box.PointerExited([](auto const &sender, auto const &) {
                if (auto pill = sender.template try_as<Grid>())
                    CancelHoverForPill(pill);
            });
            instance.hoverExitRegistered = true;
            instance.hoverPressToken = box.PointerPressed([](auto const &sender, auto const &) {
                if (auto pill = sender.template try_as<Grid>())
                    CancelHoverForPill(pill);
            });
            instance.hoverPressRegistered = true;
            instance.parent = parent;
            instance.prayer = prayer;
            instance.prayerIcon = prayerIcon;
            instance.renderedPrayerIcon = g_label;
            instance.countdown = countdown;
            instance.iqamah = iqamah;
            instance.hijri = hijri;
            instance.iqamahShadow = iqamahShadow;
            instance.prayerShadow = prayerShadow;
            instance.countdownShadow = countdownShadow;
            prayer.Text(PillPrayerNameText());
            countdown.Text(g_showCountdown ? PillCountdown() : L"");
            iqamah.Text(PillIqamah());
            if (iqamahShadow)
                iqamahShadow.Text(PillIqamah());
            g_secondaryClocks.push_back(std::move(instance));
            Wh_Log(L"[Mawaqit][Monitor] Secondary clock inserted on %s hwnd=%p", record.device.c_str(), record.hwnd);
        }
        catch (...)
        {
            // Roll back subscriptions before detaching partially inserted XAML.
            if (auto pill = instance.pill.get())
            {
                if (instance.hoverExitRegistered) { try { pill.PointerExited(instance.hoverExitToken); } catch (...) {} instance.hoverExitRegistered = false; }
                if (instance.hoverPressRegistered) { try { pill.PointerPressed(instance.hoverPressToken); } catch (...) {} instance.hoverPressRegistered = false; }
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
    const auto prayerValue = PillPrayerNameText();
    const auto countdownValue = g_showCountdown ? PillCountdown() : L"";
    const auto iqamahValue = PillIqamah();
    const auto hijriValue = PillHijriDate();
    auto setTextIfChanged = [](TextBlock const &control, std::wstring const &value) {
        if (control && control.Text() != value)
            control.Text(value);
    };
    for (auto &instance : g_secondaryClocks)
    {
        // Secondary TextBlock weak_refs can expire even while their XAML pill
        // remains attached. Resolve live controls for the recurring tick too,
        // not just for appearance changes (perf.5).
        auto pillRoot = instance.pill.get();
        auto liveText = [&](winrt::weak_ref<TextBlock> const &weak, wchar_t const *name) -> TextBlock {
            if (auto control = weak.get())
                return control;
            if (!pillRoot)
                return nullptr;
            auto walk = [&](auto &&self, DependencyObject const &node, int depth) -> TextBlock {
                if (!node || depth > 16)
                    return nullptr;
                if (auto text = node.try_as<TextBlock>(); text && text.Name() == name)
                    return text;
                int count = VisualTreeHelper::GetChildrenCount(node);
                for (int i = 0; i < count; ++i)
                    if (auto found = self(self, VisualTreeHelper::GetChild(node, i), depth + 1))
                        return found;
                return nullptr;
            };
            return walk(walk, pillRoot, 0);
        };
        // Recover the live secondary icon when its weak reference has expired.
        // Text controls already use this fallback; icons must do so as well.
        if (instance.renderedPrayerIcon != g_label)
        {
            auto icon = instance.prayerIcon.get();
            if (!icon && pillRoot)
            {
                auto findIcon = [&](auto &&self, DependencyObject const &node, int depth) -> Viewbox {
                    if (!node || depth > 16)
                        return nullptr;
                    if (auto view = node.try_as<Viewbox>(); view && view.Name() == L"MawaqitSecondaryPrayerIcon")
                        return view;
                    int count = VisualTreeHelper::GetChildrenCount(node);
                    for (int i = 0; i < count; ++i)
                        if (auto found = self(self, VisualTreeHelper::GetChild(node, i), depth + 1))
                            return found;
                    return nullptr;
                };
                icon = findIcon(findIcon, pillRoot, 0);
                if (icon)
                    instance.prayerIcon = icon;
            }
            if (icon)
            {
                RefreshPillPrayerIcon(icon, ResolvePillPalette(EffectiveTheme()).foreground);
                instance.renderedPrayerIcon = g_label;
            }
        }
        if (auto prayer = liveText(instance.prayer, L"MawaqitSecondaryPrayer"))
            setTextIfChanged(prayer, prayerValue);
        if (auto countdown = liveText(instance.countdown, L"MawaqitSecondaryCountdown"))
            setTextIfChanged(countdown, countdownValue);
        if (auto iqamah = liveText(instance.iqamah, L"MawaqitSecondaryIqamah"))
            setTextIfChanged(iqamah, iqamahValue);
        if (auto hijri = liveText(instance.hijri, L"MawaqitSecondaryHijri"))
        {
            setTextIfChanged(hijri, hijriValue);
            hijri.Visibility(hijriValue.empty() ? Visibility::Collapsed : Visibility::Visible);
        }
        if (auto shadow = instance.iqamahShadow.get())
            setTextIfChanged(shadow, iqamahValue);
        if (auto shadow = instance.prayerShadow.get())
            setTextIfChanged(shadow, prayerValue);
        if (auto shadow = instance.countdownShadow.get())
            setTextIfChanged(shadow, countdownValue);
        if (g_width == 0)
            if (auto pill = instance.pill.get())
                pill.Width(EffectivePillWidth());
        if (g_position == L"before-start" || g_position == L"before-icons")
            UpdateSecondaryBeforePosition(instance);
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
                        auto child = VisualTreeHelper::GetChild(repeater, i).try_as<FrameworkElement>();
                        if (!child || child.Visibility() != Visibility::Visible || child.ActualWidth() <= 0 ||
                            child.ActualWidth() > 400)
                            continue;
                        auto p = child.TransformToVisual(grid).TransformPoint({0, 0});
                        if (std::isfinite(p.X) && p.X >= 0 && p.X + child.ActualWidth() <= grid.ActualWidth() + 1)
                        {
                            appRight = std::max(appRight, p.X + child.ActualWidth());
                            measured = true;
                        }
                    }
                }
                if (!measured && g_position == L"after-icons")
                {
                    FrameworkElement first{nullptr};
                    double firstLeft = grid.ActualWidth();
                    double launchRight = 0.0;
                    unsigned launches = 0;
                    FindBeforeIconsTargets(grid, grid, first, firstLeft, launchRight, launches);
                    if (launches > 0)
                        PlacePillAfterLaunchGroup(grid, pill, launchRight);
                    else
                        pill.Visibility(Visibility::Collapsed);
                    continue;
                }
                desired = g_position == L"after-icons" ? appRight + 5.0 : std::max(centered, appRight + 5.0);
                bool room = measured && desired + width + 5.0 <= trayLeft;
                if (pill.Visibility() != (room ? Visibility::Visible : Visibility::Collapsed))
                    pill.Visibility(room ? Visibility::Visible : Visibility::Collapsed);
                if (room)
                {
                    auto translation = pill.RenderTransform().try_as<TranslateTransform>();
                    if (!translation)
                    {
                        translation = TranslateTransform();
                        pill.RenderTransform(translation);
                    }
                    if (std::abs(translation.X() - (desired - centered)) > 0.01)
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
    CancelHoverOpen();
    RemoveSecondaryClocksOnUiThread();
    DestroyPrayerPopup();
    if (g_pill)
    {
        if (g_hoverExitRegistered) { try { g_pill.PointerExited(g_hoverExitToken); } catch (...) {} g_hoverExitRegistered = false; }
        if (g_hoverPressRegistered) { try { g_pill.PointerPressed(g_hoverPressToken); } catch (...) {} g_hoverPressRegistered = false; }
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
    ReleaseBeforeIconsReservation();
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
    g_prayerIcon = nullptr;
    g_primaryRenderedPrayerIcon.clear();
    g_countdownText = nullptr;
    g_iqamahText = nullptr;
    g_hijriText = nullptr;
    g_iqamahShadow = nullptr;
    g_prayerShadow = nullptr;
    g_countdownShadow = nullptr;
    g_pill = nullptr;
}

// Attached mode uses the owning taskbar's actual height in XAML DIPs.
// Floating mode keeps the legacy 30 DIP / explicit height behavior.
// Prefer the styled background container over the native taskbar allocation.
// All coordinates are XAML DIPs in the insertion parent's local coordinate space.
// A failed or ambiguous lookup preserves the previous full-height behavior.
static AttachedFit ResolveAttachedFit(Panel const &insertionParent, HWND taskbar, wchar_t const *monitorLabel)
{
    const double fallback = PillSlotHeight(taskbar);
    AttachedFit result{fallback, 0.0, false};
    if (!g_attachedPill || !insertionParent)
        return result;
    try
    {
        winrt::Windows::UI::Xaml::DependencyObject root = insertionParent;
        for (int i = 0; i < 14; ++i)
        {
            auto next = VisualTreeHelper::GetParent(root);
            if (!next)
                break;
            root = next;
        }
        struct Node
        {
            winrt::Windows::UI::Xaml::DependencyObject object;
            int depth;
        };
        std::vector<Node> pending{{root, 0}};
        int visited = 0;
        while (!pending.empty() && visited++ < 350)
        {
            auto node = pending.back();
            pending.pop_back();
            auto fe = node.object.try_as<FrameworkElement>();
            if (fe && fe.Name() == L"BackgroundControl" &&
                winrt::get_class_name(node.object) == L"Taskbar.TaskbarBackground")
            {
                double height = fe.ActualHeight();
                double parentHeight = insertionParent.ActualHeight();
                if (height > 15.0 && height <= fallback && parentHeight > 0.0)
                {
                    auto transform = fe.TransformToVisual(insertionParent);
                    auto origin = transform.TransformPoint(winrt::Windows::Foundation::Point{0, 0});
                    // The pill slot is vertically centered in its insertion parent.
                    double offset = origin.Y + (height - parentHeight) / 2.0;
                    if (std::isfinite(offset) && std::abs(offset) <= fallback)
                    {
                        // Cover the full styled background when borderless. With side
                        // borders, extend one physical pixel above and below it to
                        // close rasterization gaps. Keep the content vertically centered.
                        const double pixelDip = 96.0 / std::max(96u, GetDpiForWindow(taskbar));
                        const double fittedHeight = height + (g_showPillBorder ? 2.0 * pixelDip : 0.0);
                        result = {fittedHeight, offset, true};
                        if (monitorLabel)
                            Wh_Log(L"[Mawaqit][AttachedFit] %s background=%.1f fitted=%.1f parent=%.1f y=%.1f "
                                   L"correction=%.1f",
                                   monitorLabel, height, fittedHeight, parentHeight, origin.Y, offset);
                    }
                }
                break;
            }
            if (node.depth >= 12)
                continue;
            int count = VisualTreeHelper::GetChildrenCount(node.object);
            for (int i = count - 1; i >= 0; --i)
                pending.push_back({VisualTreeHelper::GetChild(node.object, i), node.depth + 1});
        }
    }
    catch (...)
    { /* Windows builds/themes may expose different XAML trees. */
    }
    if (!result.detected && monitorLabel)
        Wh_Log(L"[Mawaqit][AttachedFit] %s fallback=%.1f", monitorLabel, fallback);
    return result;
}

// Re-evaluate styled taskbar bounds on the existing countdown UI cadence.
// Update the live XAML controls in place; do not release slots or reinsert pills.
static void RefreshAttachedFitsOnUiThread()
{
    if (!g_attachedPill || g_stopping.load())
        return;
    auto apply = [](Panel const &parent, Grid const &box, HWND hwnd, wchar_t const *label) {
        if (!parent || !box || !hwnd)
            return;
        auto fit = ResolveAttachedFit(parent, hwnd, nullptr);
        if (!fit.detected)
            return; // Preserve the last valid fit during theme transitions.
        bool changed = std::abs(box.Height() - fit.height) > 0.1;
        if (changed)
            box.Height(fit.height);
        for (auto const &child : box.Children())
        {
            auto surface = child.try_as<Border>();
            if (!surface)
                continue;
            auto transform = surface.RenderTransform().try_as<TranslateTransform>();
            if (transform && std::abs(transform.Y() - fit.offsetY) > 0.1)
            {
                transform.Y(fit.offsetY);
                changed = true;
            }
            break;
        }
        if (changed)
            Wh_Log(L"[Mawaqit][AttachedFit][Live] %s height=%.1f offset=%.1f", label, fit.height, fit.offsetY);
    };
    if (g_showPrimaryMonitor)
        apply(g_parent.get(), g_pill, taskwin::FindCurrentProcessTaskbarWnd(), L"primary");
    for (auto const &instance : g_secondaryClocks)
        apply(instance.parent.get(), instance.pill.get(), instance.taskbar, instance.monitor.c_str());
}

static double PillSlotHeight(HWND taskbar)
{
    if (!g_attachedPill)
        return g_pillHeight > 0 ? (double)g_pillHeight : 30.0;
    RECT bounds{};
    if (taskbar && GetWindowRect(taskbar, &bounds) && bounds.bottom > bounds.top)
        return (bounds.bottom - bounds.top) * 96.0 / std::max(96u, GetDpiForWindow(taskbar));
    return 48.0; // Temporary fallback until the taskbar window is available.
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
    const auto attachedFit = ResolveAttachedFit(parent, taskwin::FindCurrentProcessTaskbarWnd(), L"primary");
    // Slot is resolved from the selected position before creating XAML content.
    // Layout settings are DIPs, so XAML performs monitor DPI scaling.
    double dpi = 96.0;
    if (auto wnd = taskwin::FindCurrentProcessTaskbarWnd())
        dpi = (double)GetDpiForWindow(wnd);
    double widthDip = EffectivePillWidth();
    Grid box;
    box.Name(kSlotName);
    box.Width(widthDip);
    box.Height(attachedFit.height);
    box.HorizontalAlignment(HorizontalAlignment::Center);
    box.VerticalAlignment(VerticalAlignment::Center);
    auto palette = ResolvePillPalette(EffectiveTheme());
    Border pillSurface;
    pillSurface.Background(CreatePillBackground(palette.background));
    pillSurface.BorderThickness(Thickness{0, 0, 0, 0});
    pillSurface.BorderBrush(nullptr);
    if (g_showPillBorder)
    {
        unsigned int borderRgb;
        auto borderColor = palette.countdown;
        if (TryThemeOverride(L"ThemeColorOverrides.Pill.Border", borderRgb))
            borderColor = Rgb(borderRgb);
        pillSurface.BorderBrush(CreatePillBorderBrush(borderColor));
        pillSurface.BorderThickness(g_attachedPill ? Thickness{1, 0, 1, 0} : Thickness{1, 1, 1, 1});
    }
    double radius = g_attachedPill ? 0.0 : (double)g_cornerRadius;
    pillSurface.CornerRadius({radius, radius, radius, radius});
    // Position offsets are visual only; the native slot remains reserved.
    // Add subtle position-specific spacing without overriding custom margins.
    double positionOffsetX = 0.0;
    if (g_position == L"after-tray")
        positionOffsetX = -5.0; // Equivalent to additional margins: 5, 0, 10, 0.
    else if (g_position == L"before-icons" || g_position == L"before-start")
        positionOffsetX = 5.0; // Equivalent to additional margins: 10, 0, 5, 0.
    TranslateTransform offset;
    offset.X(g_attachedPill ? 0.0 : (double)(g_margins.left - g_margins.right) + positionOffsetX);
    offset.Y(g_attachedPill ? attachedFit.offsetY : (double)(g_margins.top - g_margins.bottom));
    pillSurface.RenderTransform(offset);
    pillSurface.Padding({(double)g_padding.left, g_attachedPill ? 0.0 : (double)g_padding.top, (double)g_padding.right,
                         g_attachedPill ? 0.0 : (double)g_padding.bottom});
    pillSurface.HorizontalAlignment(HorizontalAlignment::Stretch);
    pillSurface.VerticalAlignment(VerticalAlignment::Stretch);
    // XAML text controls in the native taskbar layout.
    StackPanel content;
    content.Orientation(Orientation::Horizontal);
    content.HorizontalAlignment(HorizontalAlignment::Center);
    content.VerticalAlignment(VerticalAlignment::Center);
    TextBlock prayer;
    prayer.FontSize((double)g_fontSize);
    prayer.FontWeight(winrt::Windows::UI::Text::FontWeight{static_cast<uint16_t>(g_pillFontWeight)});
    prayer.VerticalAlignment(VerticalAlignment::Center);
    prayer.Margin({5, 0, 4, 0});
    SolidColorBrush foreground;
    foreground.Color(palette.foreground);
    prayer.Foreground(foreground);
    TextBlock countdown;
    countdown.FontSize((double)g_fontSize);
    countdown.FontWeight(winrt::Windows::UI::Text::FontWeight{static_cast<uint16_t>(g_pillFontWeight)});
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
    Viewbox prayerIcon{nullptr};
    if (g_showNext && g_nextPrayerDisplay != L"name")
    {
        prayerIcon = MakePillPrayerIcon(palette.foreground);
        prayerIcon.Visibility(g_showNext ? Visibility::Visible : Visibility::Collapsed);
        content.Children().Append(prayerIcon);
        if (g_nextPrayerDisplay == L"icon")
            prayer.Visibility(Visibility::Collapsed);
    }
    if (g_pillTextShadow)
    {
        auto layer = MakePillTextWithShadow(prayer, prayerShadow);
        layer.Visibility((g_showNext && g_nextPrayerDisplay != L"icon") ? Visibility::Visible : Visibility::Collapsed);
        content.Children().Append(layer);
    }
    else
        content.Children().Append(prayer);
    if (g_theme == L"custom" && g_showNext && g_showCountdown)
    {
        TextBlock separator;
        separator.Text(L"|");
        separator.FontSize((double)g_fontSize);
        separator.FontWeight(winrt::Windows::UI::Text::FontWeight{static_cast<uint16_t>(g_pillFontWeight)});
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
    TextBlock hijri;
    hijri.FontSize((double)g_fontSize);
    hijri.FontWeight(winrt::Windows::UI::Text::FontWeight{static_cast<uint16_t>(g_pillFontWeight)});
    hijri.VerticalAlignment(VerticalAlignment::Center);
    hijri.Margin({4, 0, 5, 0});
    hijri.Foreground(SolidColorBrush(OverridePillColor(L"ThemeColorOverrides.Pill.HijriDate", palette.foreground)));
    hijri.Text(PillHijriDate());
    hijri.Visibility(PillHijriDate().empty() ? Visibility::Collapsed : Visibility::Visible);
    content.Children().Append(hijri);
    pillSurface.Child(content);
    box.Children().Append(pillSurface);
    if (g_position == L"center" || g_position == L"after-icons" ||
        (g_position == L"before-icons" || g_position == L"before-start"))
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
        g_tapToken = box.Tapped([](auto const &, auto const &) { CancelHoverOpen(); TogglePrayerPopup(g_pill, nullptr, false); });
        g_tapRegistered = true;
        g_hoverToken = box.PointerEntered([](auto const &, auto const &) {
            if (g_openPopupOnHover)
                ScheduleHoverOpen(g_pill, nullptr);
        });
        g_hoverRegistered = true;
        g_hoverExitToken = box.PointerExited([](auto const &sender, auto const &) {
            if (auto pill = sender.template try_as<Grid>())
                CancelHoverForPill(pill);
        });
        g_hoverExitRegistered = true;
        g_hoverPressToken = box.PointerPressed([](auto const &sender, auto const &) {
            if (auto pill = sender.template try_as<Grid>())
                CancelHoverForPill(pill);
        });
        g_hoverPressRegistered = true;
    }
    catch (...)
    {
        // Roll back a partially registered primary pill before propagating.
        if (g_hoverExitRegistered) { try { box.PointerExited(g_hoverExitToken); } catch (...) {} g_hoverExitRegistered = false; }
        if (g_hoverPressRegistered) { try { box.PointerPressed(g_hoverPressToken); } catch (...) {} g_hoverPressRegistered = false; }
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
    g_prayerIcon = prayerIcon;
    g_primaryRenderedPrayerIcon = g_label;
    g_countdownText = countdown;
    g_iqamahText = iqamah;
    g_hijriText = hijri;
    g_iqamahShadow = iqamahShadow;
    g_prayerShadow = prayerShadow;
    g_countdownShadow = countdownShadow;
    g_parent = parent;
    UpdateDynamicCenterOnUiThread();
    UpdateAfterIconsOnUiThread();
    UpdateBeforeIconsOnUiThread();
    UpdateCountdown();
    UpdateSecondaryClocksOnUiThread();
    prayer.Text(PillPrayerNameText());
    countdown.Text(g_showCountdown ? PillCountdown() : L"");
    iqamah.Text(PillIqamah());
    if (iqamahShadow)
        iqamahShadow.Text(PillIqamah());
    if (prayerShadow)
        prayerShadow.Text(PillPrayerNameText());
    if (countdownShadow)
        countdownShadow.Text(g_showCountdown ? PillCountdown() : L"");
    Wh_Log(L"[Mawaqit] Native XAML position %s inserted: %d physical px, %.1f DIP, DPI %.0f", g_position.c_str(),
           g_width, widthDip, dpi);
}

static void LogException(PCWSTR context)
{
    try
    {
        throw;
    }
    catch (winrt::hresult_error const &e)
    {
        Wh_Log(L"[Mawaqit] %s failed HRESULT 0x%08X: %s", context, (unsigned)e.code().value, e.message().c_str());
    }
    catch (...)
    {
        Wh_Log(L"[Mawaqit] %s failed", context);
    }
}

static bool OnTaskbarThread(folder_menus_dispatch::ThreadProc proc)
{
    HWND wnd = taskwin::FindCurrentProcessTaskbarWnd();
    return wnd && folder_menus_dispatch::RunFromWindowThread(wnd, proc, nullptr, L"Windhawk_Mawaqit_TaskbarDispatch");
}

// Explorer can inject the mod before creating its taskbar HWNDs. Bootstrap
// waits for ownership rather than permanently skipping workers at AfterInit.
static DWORD WINAPI ScheduleWorker(void *);
static DWORD WINAPI CountdownWorker(void *);
static DWORD WINAPI StartWorker(void *)
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
                        Wh_Log(L"[Mawaqit] Failed to start schedule worker: error=%lu", GetLastError());
                    g_countdownWorker = CreateThread(nullptr, 0, CountdownWorker, nullptr, 0, nullptr);
                    if (!g_countdownWorker)
                        Wh_Log(L"[Mawaqit] Failed to start countdown worker: error=%lu", GetLastError());
                }
            }
            bool dispatched = OnTaskbarThread([](void *) { InsertOnUiThread(); });
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
        for (auto const &child : parent.Children())
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
        InvalidatePopupPaletteCache();
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

// Recolor the existing XAML objects in place. Do not release taskbar leases.
static void RefreshPillAppearanceOnUiThread()
{
    auto palette = ResolvePillPalette(EffectiveTheme());
    auto recolor = [&](Grid const &box, TextBlock const &prayer, TextBlock const &countdown, TextBlock const &iqamah,
                       TextBlock const &hijri, Viewbox const &icon) {
        if (box)
        {
            for (auto const &child : box.Children())
            {
                auto surface = child.try_as<Border>();
                if (!surface)
                    continue;
                surface.Background(CreatePillBackground(palette.background));
                if (g_showPillBorder)
                {
                    unsigned int borderRgb;
                    auto borderColor = palette.countdown;
                    if (TryThemeOverride(L"ThemeColorOverrides.Pill.Border", borderRgb))
                        borderColor = Rgb(borderRgb);
                    surface.BorderBrush(CreatePillBorderBrush(borderColor));
                    surface.BorderThickness(g_attachedPill ? Thickness{1, 0, 1, 0} : Thickness{1, 1, 1, 1});
                }
                else
                {
                    surface.BorderThickness(Thickness{0, 0, 0, 0});
                    surface.BorderBrush(nullptr);
                }
            }
        }
        if (prayer)
            prayer.Foreground(SolidColorBrush(palette.foreground));
        if (countdown)
            countdown.Foreground(SolidColorBrush(palette.countdown));
        if (hijri)
            hijri.Foreground(SolidColorBrush(OverridePillColor(L"ThemeColorOverrides.Pill.HijriDate", palette.foreground)));
        if (iqamah)
        {
            auto color = ResolvePillIqamahColor(palette.countdown);
            unsigned rgb;
            if (TryThemeOverride(L"ThemeColorOverrides.Pill.Iqamah", rgb))
                color = Rgb(rgb);
            iqamah.Foreground(SolidColorBrush(color));
        }
        if (icon)
            RefreshPillPrayerIcon(icon, palette.foreground);
    };
    recolor(g_pill, g_prayerText, g_countdownText, g_iqamahText, g_hijriText, g_prayerIcon);
    unsigned updated = g_pill ? 1 : 0;
    for (auto &instance : g_secondaryClocks)
    {
        auto box = instance.pill.get();
        if (!box)
            continue;
        // The secondary XAML controls can outlive their projected weak_ref handles.
        // Locate the live controls under the still-attached pill instead of trusting
        // weak_ref::get() during an appearance transition.
        auto findLiveText = [&](wchar_t const *name) -> TextBlock {
            auto walk = [&](auto &&self, DependencyObject const &node, int depth) -> TextBlock {
                if (!node || depth > 16)
                    return nullptr;
                if (auto text = node.try_as<TextBlock>(); text && text.Name() == name)
                    return text;
                int count = VisualTreeHelper::GetChildrenCount(node);
                for (int i = 0; i < count; ++i)
                {
                    if (auto found = self(self, VisualTreeHelper::GetChild(node, i), depth + 1))
                        return found;
                }
                return nullptr;
            };
            return walk(walk, box, 0);
        };
        auto prayer = findLiveText(L"MawaqitSecondaryPrayer");
        auto countdown = findLiveText(L"MawaqitSecondaryCountdown");
        auto iqamah = findLiveText(L"MawaqitSecondaryIqamah");
        auto hijri = findLiveText(L"MawaqitSecondaryHijri");
        recolor(box, prayer, countdown, iqamah, hijri, instance.prayerIcon.get());
        ++updated;
    }
    Wh_Log(L"[Mawaqit][Appearance] refreshed=%u theme=%s systemLight=%d appLight=%d", updated, EffectiveTheme().c_str(),
           WindowsSystemLightMode(), WindowsLightMode());
}

static void UpdateClockOnUiThread()
{
    if (g_stopping.load())
        return;
    ApplyPendingSettingsOnUiThread();
    // Windows can reapply XAML theme resources after its registry values change.
    // Retry appearance application on the next two countdown UI passes, without
    // touching taskbar slots or rebuilding controls.
    static unsigned appearanceSettleTicks = 0;
    // The countdown worker already visits the UI thread regularly. Detect
    // actual mode transitions without adding another worker or window hook.
    static int previousSystemLight = -1;
    static int previousAppLight = -1;
    static COLORREF previousAccent = CLR_INVALID;
    int currentSystemLight = WindowsSystemLightMode() ? 1 : 0;
    int currentAppLight = WindowsLightMode() ? 1 : 0;
    COLORREF currentAccent = WindowsAccentColor();
    if (previousSystemLight < 0)
    {
        previousSystemLight = currentSystemLight;
        previousAppLight = currentAppLight;
        previousAccent = currentAccent;
        Wh_Log(L"[Mawaqit][Appearance] initial systemLight=%d appLight=%d accent=#%02X%02X%02X", currentSystemLight,
               WindowsLightMode(), GetRValue(currentAccent), GetGValue(currentAccent), GetBValue(currentAccent));
    }
    else if (previousSystemLight != currentSystemLight || previousAppLight != currentAppLight ||
             previousAccent != currentAccent)
    {
        Wh_Log(L"[Mawaqit][Appearance] change systemLight=%d->%d accent=#%02X%02X%02X->#%02X%02X%02X",
               previousSystemLight, currentSystemLight, GetRValue(previousAccent), GetGValue(previousAccent),
               GetBValue(previousAccent), GetRValue(currentAccent), GetGValue(currentAccent), GetBValue(currentAccent));
        Wh_Log(L"[Mawaqit][Appearance] appLight=%d->%d", previousAppLight, currentAppLight);
        previousSystemLight = currentSystemLight;
        previousAppLight = currentAppLight;
        previousAccent = currentAccent;
        if (g_theme == L"system" || g_dynamicLightDarkMode)
        {
            InvalidatePopupPaletteCache();
            RefreshPillAppearanceOnUiThread();
            appearanceSettleTicks = 3;
            if (g_popup && IsWindow(g_popup))
                InvalidateRect(g_popup, nullptr, FALSE);
        }
    }
    if (appearanceSettleTicks != 0)
    {
        const unsigned pass = 4 - appearanceSettleTicks;
        --appearanceSettleTicks;
        // The first retry is deferred until the next UI update; do not run
        // twice in the same tick as the initial appearance change.
        if (pass > 1)
        {
            RefreshPillAppearanceOnUiThread();
            Wh_Log(L"[Mawaqit][Appearance][Settle] retry=%u remaining=%u", pass - 1, appearanceSettleTicks);
        }
    }
    // Check occasionally even without a StartTaskbar notification: some
    // Windows shell changes replace the XAML tree through other paths.
    static unsigned healthTick = 0;
    if (g_taskbarRebuildPending.exchange(false, std::memory_order_acq_rel) || ++healthTick >= 5)
    {
        healthTick = 0;
        EnsureTaskbarSlotOnUiThread();
    }
    // Poll styled geometry every two seconds, including after Taskbar Styler edits.
    static unsigned attachedFitTick = 0;
    if (++attachedFitTick >= 2)
    {
        attachedFitTick = 0;
        RefreshAttachedFitsOnUiThread();
        RefreshAttachedPopupPositionOnUiThread();
    }
    UpdateCountdown();
    const auto prayerValue = PillPrayerNameText();
    const auto countdownValue = g_showCountdown ? PillCountdown() : L"";
    const auto iqamahValue = PillIqamah();
    const auto hijriValue = PillHijriDate();
    auto setTextIfChanged = [](TextBlock const &control, std::wstring const &value) {
        if (control && control.Text() != value)
            control.Text(value);
    };
    auto prayer = g_prayerText;
    auto countdown = g_countdownText;
    if (g_prayerIcon && g_primaryRenderedPrayerIcon != g_label)
    {
        RefreshPillPrayerIcon(g_prayerIcon, ResolvePillPalette(EffectiveTheme()).foreground);
        g_primaryRenderedPrayerIcon = g_label;
    }
    if (prayer)
        setTextIfChanged(prayer, prayerValue);
    if (countdown)
        setTextIfChanged(countdown, countdownValue);
    if (g_iqamahText)
        setTextIfChanged(g_iqamahText, iqamahValue);
    if (g_hijriText)
    {
        setTextIfChanged(g_hijriText, hijriValue);
        const auto desiredVisibility = hijriValue.empty() ? Visibility::Collapsed : Visibility::Visible;
        if (g_hijriText.Visibility() != desiredVisibility)
            g_hijriText.Visibility(desiredVisibility);
    }
    if (g_iqamahShadow)
        setTextIfChanged(g_iqamahShadow, iqamahValue);
    if (g_prayerShadow)
        setTextIfChanged(g_prayerShadow, prayerValue);
    if (g_countdownShadow)
        setTextIfChanged(g_countdownShadow, countdownValue);
    if (g_width == 0 && g_pill)
    {
        const double desiredWidth = EffectivePillWidth();
        if (std::abs(g_pill.Width() - desiredWidth) > 0.01)
            g_pill.Width(desiredWidth);
    }
    UpdateDynamicCenterOnUiThread();
    UpdateAfterIconsOnUiThread();
    UpdateBeforeIconsOnUiThread();
    UpdateSecondaryClocksOnUiThread();
}

// XAML DispatcherTimer did not tick in the taskbar's hosting context on the
// tested Windows build. A Win32 waitable worker supplies the cadence; all
// TextBlock writes still execute on the owning taskbar/XAML thread.
static DWORD WINAPI CountdownWorker(void *)
{
    while (!g_stopping.load())
    {
        DWORD wait = WaitForSingleObject(g_stopEvent, 1000);
        if (wait != WAIT_TIMEOUT)
            break;
        if (g_stopping.load())
            break;
        if (!OnTaskbarThread([](void *) { UpdateClockOnUiThread(); }))
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
static DWORD FreshScheduleCacheRemainingMs(const std::wstring &mosque, int hours)
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
        std::string json((std::istreambuf_iterator<char>(metadata)), std::istreambuf_iterator<char>());
        if (json.size() > 65536)
            return 0;
        NativeUpdater::Parser parser{json};
        auto root = parser.value();
        if (root.key("mosqueIdentifier").s != NativeUpdater::narrow(slug))
            return 0;
        // One-time metadata upgrade when Hijri is enabled. Existing schedule
        // caches predate the mosque's hijriAdjustment field.
        if ((g_hijriMode != L"off" || g_popupHijriMode != L"off") &&
            root.key("hijriAdjustment").kind != NativeUpdater::J::Str)
        {
            Wh_Log(L"[Mawaqit][Hijri] Cached adjustment unavailable; refreshing mosque metadata");
            return 0;
        }
        SYSTEMTIME now;
        GetLocalTime(&now);
        if (root.key("scheduleYear").s != std::to_string(now.wYear))
            return 0;
        std::wstring schedulePath = dir + L"\\schedule.txt";
        std::ifstream schedule(schedulePath.c_str(), std::ios::binary);
        if (!schedule)
            return 0;
        std::string header;
        if (!std::getline(schedule, header) || header != "# Emberveil Mawaqit schedule v4")
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
        if (!GetFileAttributesExW((dir + L"\\metadata.json").c_str(), GetFileExInfoStandard, &attrs))
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
    ULONGLONG secondsLeft = (23u - now.wHour) * 3600ULL + (59u - now.wMinute) * 60ULL + (60u - now.wSecond);
    // Wake just after midnight, allowing Windows to advance the local date.
    ULONGLONG untilNewYearMs = (secondsLeft + 2ULL) * 1000ULL;
    DWORD capped = (DWORD)std::min<ULONGLONG>(untilNewYearMs, 3600000ULL);
    return std::min(delay, capped);
}

static DWORD WINAPI ScheduleWorker(void *)
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
        catch (const std::exception &e)
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
        const bool obsolete = g_scheduleConfigGeneration.load(std::memory_order_acquire) != generation;
        if (obsolete)
        {
            Wh_Log(L"[Mawaqit] Refresh configuration changed; retrying latest settings");
            delay = 0;
            continue;
        }
        if (ok)
        {
            Wh_Log(L"[Mawaqit] Schedule updated");
            OnTaskbarThread([](void *) {
                g_loadedDate.clear();
                g_yesterday.clear();
                g_today.clear();
                g_tomorrow.clear();
                LoadMetadata();
                UpdateClockOnUiThread();
            });
        }
        delay = (DWORD)(ok ? hours : 1) * 60u * 60u * 1000u;
        Wh_Log(L"[Mawaqit] Refresh finished: success=%d next-delay=%lu ms", ok ? 1 : 0, (unsigned long)delay);
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

// Keep public v0.7.1's General.Position key. Other reorganized v0.7.1
// appearance settings intentionally use v0.8 defaults. These aliases only
// preserve paths used during v0.8 development; new keys always take priority.
static std::wstring ResolveCompatibleSettingKey(PCWSTR key)
{
    struct Alias
    {
        PCWSTR current;
        PCWSTR previous;
    };
    static constexpr Alias exact[] = {
        {L"General.AutomaticTheme", L"General.AutomaticThemeMode"},
        {L"TaskbarPill.Layout.Style", L"TaskbarPill.Layout.PillLayout"},
        {L"TaskbarPill.Layout.Height", L"TaskbarPill.Layout.PillHeight"},
        {L"TaskbarPill.Appearance.ShowBorder", L"TaskbarPill.Appearance.ShowPillBorder"},
        {L"Popup.Behavior.Enabled", L"Popup.Behavior.EnablePopup"},
        {L"Popup.Behavior.OpenOnHover", L"Popup.Behavior.OpenPopupOnHover"},
        {L"Popup.Behavior.AutoCloseSeconds", L"Popup.Behavior.PopupAutoCloseSeconds"},
        {L"Popup.Layout.Placement", L"Popup.Layout.PopupLayout"},
        {L"Popup.Layout.ScalePercent", L"Popup.Layout.PopupScale"},
        {L"Popup.Content.HighlightNextPrayer", L"Popup.Content.ShowNextPrayerAccent"},
        {L"Popup.Content.HighlightOpacity", L"Popup.Content.NextPrayerAccentOpacity"},
        {L"Popup.Typography.TextShadow", L"Popup.Appearance.TextShadow"},
        {L"General.Position", L"TaskbarPill.PositionAndMonitors.Position"},
    };
    static constexpr Alias prefixes[] = {
        {L"TaskbarPill.Monitors.", L"TaskbarPill.PositionAndMonitors."},
        {L"TaskbarPill.Layout.", L"TaskbarPill.LayoutAndDimensions."},
        {L"TaskbarPill.Content.", L"TaskbarPill.PrayerInformation."},
        {L"Popup.Layout.", L"Popup.LayoutAndDimensions."},
        {L"Popup.Content.", L"Popup.ContentAndHighlighting."},
    };
    std::vector<std::wstring> candidates{key};
    for (size_t i = 0; i < candidates.size() && i < 32; ++i)
    {
        const std::wstring current = candidates[i];
        if (!ReadSetting(current.c_str()).empty())
            return current;
        auto append = [&](std::wstring previous) {
            if (std::find(candidates.begin(), candidates.end(), previous) == candidates.end())
                candidates.push_back(std::move(previous));
        };
        for (const auto &alias : exact)
            if (current == alias.current)
                append(alias.previous);
        for (const auto &alias : prefixes)
        {
            const size_t length = wcslen(alias.current);
            if (current.compare(0, length, alias.current) == 0)
                append(std::wstring(alias.previous) + current.substr(length));
        }
    }
    return key;
}

static std::wstring ReadCompatibleString(PCWSTR key)
{
    auto resolved = ResolveCompatibleSettingKey(key);
    return ReadSetting(resolved.c_str());
}

// Older development builds stored a boolean plus a separate display mode.
// The new string setting is authoritative once saved, including explicit None.
static std::wstring ReadNextPrayerDisplay()
{
    auto value = ReadCompatibleString(L"TaskbarPill.Content.ShowNextPrayer");
    if (value == L"none" || value == L"name" || value == L"icon" || value == L"both")
        return value;
    if (value == L"0" || value == L"false")
        return L"none";
    auto previous = ReadCompatibleString(L"TaskbarPill.Content.NextPrayerDisplay");
    if (previous == L"icon" || previous == L"both")
        return previous;
    return L"name";
}

static int ReadFontWeight(PCWSTR key)
{
    auto value = ReadCompatibleString(key);
    if (value == L"medium")
        return FW_MEDIUM;
    if (value == L"semibold")
        return FW_SEMIBOLD;
    if (value == L"bold")
        return FW_BOLD;
    return FW_NORMAL;
}

// The Windhawk settings API returns 0 for absent integer keys. Use the
// schema's actual default only when no value was saved; explicit 0 stays 0.
static int ModernDefault(PCWSTR key, int fallback)
{
    struct Entry
    {
        PCWSTR key;
        int value;
    };
    static constexpr Entry defaults[] = {
        {L"TaskbarPill.Monitors.ShowPrimaryMonitor", 1},
        {L"TaskbarPill.Monitors.ShowSecondaryMonitors", 0},
        {L"TaskbarPill.Content.ShowCountdown", 1},
        {L"TaskbarPill.Content.ShowIqamahTime", 1},
        {L"TaskbarPill.Content.ShowFajrWindowCountdown", 0},
        {L"TaskbarPill.Appearance.BackgroundOpacity", 100},
        {L"TaskbarPill.Appearance.BorderOpacity", 20},
        {L"TaskbarPill.Typography.IqamahFontScale", 80},
        {L"TaskbarPill.Appearance.CornerRadius", 13},
        {L"TaskbarPill.Typography.FontSize", 15},
        {L"General.ShowAmPm", 1},
        {L"Popup.Behavior.Enabled", 1},
        {L"Popup.Behavior.OpenOnHover", 1},
        {L"Popup.Behavior.HoverOpenDelayMs", 500},
        {L"Popup.Behavior.HoverCloseDelayMs", 500},
        {L"Popup.Behavior.AutoCloseSeconds", 5},
        {L"Popup.Appearance.BackgroundOpacity", -1},
        {L"Popup.Appearance.ShowBorder", 1},
        {L"Popup.Appearance.ShowSeparators", 1},
        {L"Popup.Layout.ScalePercent", 100},
        {L"Popup.Typography.FontSize", 15},
        {L"Popup.Content.HighlightNextPrayer", 1},
        {L"Popup.Content.ShowNextPrayerBorder", 1},
        {L"Popup.Content.BoldNextPrayer", 1},
        {L"Popup.Content.ShowPrayerIcons", 1},
        {L"Popup.Content.HighlightOpacity", 20},
        {L"Advanced.RefreshHours", 168},
    };
    for (const auto &entry : defaults)
        if (wcscmp(key, entry.key) == 0)
            return entry.value;
    return fallback;
}

static int ReadCompatibleInt(PCWSTR key, int missingDefault = 0)
{
    auto resolved = ResolveCompatibleSettingKey(key);
    if (!ReadSetting(resolved.c_str()).empty())
        return Wh_GetIntSetting(resolved.c_str());
    return ModernDefault(key, missingDefault);
}

static void LoadModSettings()
{
    g_use12HourTime = ReadCompatibleString(L"General.TimeFormat") == L"12h";
    g_showAmPm = ReadCompatibleInt(L"General.ShowAmPm", 1) != 0;
    std::wstring position = ReadCompatibleString(L"General.Position");
    if (position != L"after-tray" && position != L"before-icons" && position != L"before-start" &&
        position != L"after-icons" && position != L"center")
        position = L"before-tray";
    g_position = position;
    g_showPrimaryMonitor = ReadCompatibleInt(L"TaskbarPill.Monitors.ShowPrimaryMonitor") != 0;
    g_showSecondaryMonitors = ReadCompatibleInt(L"TaskbarPill.Monitors.ShowSecondaryMonitors") != 0;
    Wh_Log(L"[Mawaqit][Monitor] effective monitors primary=%d secondary=%d", g_showPrimaryMonitor ? 1 : 0,
           g_showSecondaryMonitors ? 1 : 0);
    Wh_Log(L"[Mawaqit][Monitor] Visibility: primary=%d secondary=%d unified-position=%s", g_showPrimaryMonitor ? 1 : 0,
           g_showSecondaryMonitors ? 1 : 0, g_position.c_str());
    auto theme = ReadSetting(L"General.Theme");
    if (theme != L"monochrome" && theme != L"traditional" && theme != L"catppuccin" && theme != L"emberveil" && theme != L"emberveil-midnight" &&
        theme != L"emberveil-daybreak" && theme != L"emerald-mosque" && theme != L"dracula" && theme != L"nord" &&
        theme != L"gruvbox" && theme != L"tokyo-night" && theme != L"solarized-dark" && theme != L"solarized-light" &&
        theme != L"catppuccin-latte" && theme != L"dracula-light" && theme != L"nord-light" &&
        theme != L"gruvbox-light" && theme != L"tokyo-night-day" && theme != L"rose-pine" && theme != L"everforest" &&
        theme != L"synthwave-84" && theme != L"flexoki" && theme != L"kanagawa" && theme != L"ayu" &&
        theme != L"one-dark" && theme != L"custom")
        theme = L"system";
    g_theme = theme;
    g_dynamicLightDarkMode = ReadCompatibleInt(L"General.AutomaticTheme") != 0;
    Wh_Log(L"[Mawaqit][Appearance] selected=%s effective=%s dynamic=%d", g_theme.c_str(), EffectiveTheme().c_str(),
           g_dynamicLightDarkMode ? 1 : 0);
    auto backgroundStyle = ReadCompatibleString(L"TaskbarPill.Appearance.BackgroundStyle");
    if (backgroundStyle != L"transparent" && backgroundStyle != L"blur" && backgroundStyle != L"acrylic")
        backgroundStyle = L"solid";
    g_backgroundStyle = backgroundStyle;
    g_pillFontWeight = ReadFontWeight(L"TaskbarPill.Typography.FontWeight");
    g_popupFontWeight = ReadFontWeight(L"Popup.Typography.FontWeight");
    g_popupFontSize = std::clamp(ReadCompatibleInt(L"Popup.Typography.FontSize", 15), 10, 24);
    g_pillTextShadow = ReadCompatibleInt(L"TaskbarPill.Typography.TextShadow") != 0;
    g_showPillBorder = ReadCompatibleInt(L"TaskbarPill.Appearance.ShowBorder") != 0;
    g_pillBorderOpacity = std::clamp(ReadCompatibleInt(L"TaskbarPill.Appearance.BorderOpacity", 20), 0, 100);
    g_backgroundOpacity = std::clamp(ReadCompatibleInt(L"TaskbarPill.Appearance.BackgroundOpacity"), 0, 100);
    auto popupStyle = ReadCompatibleString(L"Popup.Appearance.BackgroundStyle");
    if (popupStyle != L"solid" && popupStyle != L"transparent" && popupStyle != L"blur" && popupStyle != L"acrylic")
        popupStyle = L"follow-pill";
    g_popupBackgroundStyle = popupStyle;
    g_popupBackgroundOpacity = std::clamp(ReadCompatibleInt(L"Popup.Appearance.BackgroundOpacity"), -1, 100);
    g_popupScale = std::clamp(ReadCompatibleInt(L"Popup.Layout.ScalePercent", 100), 50, 200);
    g_popupLayout = ReadCompatibleString(L"Popup.Layout.Placement");
    if (g_popupLayout != L"attached")
        g_popupLayout = L"floating";
    g_popupAttachedCorners = ReadCompatibleString(L"Popup.Layout.AttachedCornerStyle");
    if (g_popupAttachedCorners != L"rounded" && g_popupAttachedCorners != L"square")
        g_popupAttachedCorners = L"adaptive";
    g_popupTextShadow = ReadCompatibleInt(L"Popup.Typography.TextShadow") != 0;
    g_popupShowBorder = ReadCompatibleInt(L"Popup.Appearance.ShowBorder") != 0;
    g_popupShowSeparators = ReadCompatibleInt(L"Popup.Appearance.ShowSeparators") != 0;
    g_popupShowNextPrayerAccent = ReadCompatibleInt(L"Popup.Content.HighlightNextPrayer", 1) != 0;
    g_popupShowNextPrayerBorder = ReadCompatibleInt(L"Popup.Content.ShowNextPrayerBorder", 1) != 0;
    g_popupBoldNextPrayer = ReadCompatibleInt(L"Popup.Content.BoldNextPrayer", 1) != 0;
    g_popupShowPrayerIcons = ReadCompatibleInt(L"Popup.Content.ShowPrayerIcons", 1) != 0;
    g_popupNextPrayerAccentOpacity = std::clamp(ReadCompatibleInt(L"Popup.Content.HighlightOpacity", 20), 0, 100);

    g_attachedPill = ReadCompatibleString(L"TaskbarPill.Layout.Style") == L"attached";
    int width = ReadCompatibleInt(L"TaskbarPill.Layout.Width");
    // Preserve older manual widths by lifting the former 180–189 range.
    g_width = width == 0 ? 0 : (width >= 180 && width <= 400) ? std::max(190, width) : 0;
    int height = ReadCompatibleInt(L"TaskbarPill.Layout.Height");
    g_pillHeight = (height >= 0 && height <= 100) ? height : 0;
    int radius = ReadCompatibleInt(L"TaskbarPill.Appearance.CornerRadius");
    g_cornerRadius = (radius >= 0 && radius <= 30) ? radius : 13;
    int font = ReadCompatibleInt(L"TaskbarPill.Typography.FontSize");
    g_fontSize = (font >= 10 && font <= 24) ? font : 15;
    int iqamahScale = ReadCompatibleInt(L"TaskbarPill.Typography.IqamahFontScale");
    g_iqamahFontScale = std::clamp(iqamahScale, 50, 100);
    auto marginSetting = ReadCompatibleString(L"TaskbarPill.Layout.Margins");
    if (!marginSetting.empty())
        g_margins = ParseInsets(marginSetting.c_str(), -100, 100);
    auto paddingSetting = ReadCompatibleString(L"TaskbarPill.Layout.Padding");
    if (!paddingSetting.empty())
        g_padding = ParseInsets(paddingSetting.c_str(), 0, 100);
    g_nextPrayerDisplay = ReadNextPrayerDisplay();
    g_showNext = g_nextPrayerDisplay != L"none";
    g_nextPrayerIconStyle = ReadCompatibleString(L"TaskbarPill.Content.NextPrayerIconStyle");
    if (g_nextPrayerIconStyle != L"mosque")
        g_nextPrayerIconStyle = L"prayer-period";
    g_showCountdown = ReadCompatibleInt(L"TaskbarPill.Content.ShowCountdown") != 0;
    std::wstring previousHijriMode = g_hijriMode;
    g_hijriMode = ReadCompatibleString(L"TaskbarPill.Content.HijriDate");
    if (g_hijriMode != L"full" && g_hijriMode != L"numeric")
        g_hijriMode = L"off";
    g_showIqamah = ReadCompatibleInt(L"TaskbarPill.Content.ShowIqamahTime") != 0;
    g_showFajrWindowCountdown = ReadCompatibleInt(L"TaskbarPill.Content.ShowFajrWindowCountdown") != 0;
    g_popupHijriMode = ReadCompatibleString(L"Popup.Content.HijriDate");
    if (g_popupHijriMode != L"off" && g_popupHijriMode != L"numeric")
        g_popupHijriMode = L"full";
    g_enablePopup = ReadCompatibleInt(L"Popup.Behavior.Enabled") != 0;
    g_openPopupOnHover = ReadCompatibleInt(L"Popup.Behavior.OpenOnHover") != 0;
    g_hoverOpenDelayMs = std::clamp(ReadCompatibleInt(L"Popup.Behavior.HoverOpenDelayMs", 500), 0, 5000);
    g_hoverCloseDelayMs = std::clamp(ReadCompatibleInt(L"Popup.Behavior.HoverCloseDelayMs"), 0, 5000);
    int autoClose = std::clamp(ReadCompatibleInt(L"Popup.Behavior.AutoCloseSeconds"), 0, 300);
    g_popupAutoCloseSeconds.store(autoClose);
    Wh_Log(L"[Mawaqit] Popup auto-close configured: %d seconds", autoClose);
    std::wstring mosque = ReadSetting(L"General.Mosque");
    if (mosque.empty())
        mosque = L"al-haram-makkah-saudi-arabia";
    // Accept the refresh interval as text or a stored integer while retaining
    // the existing setting key.
    constexpr PCWSTR refreshKey = L"Advanced.RefreshHours";
    std::wstring refreshValue = ReadCompatibleString(refreshKey);
    int hours = 0;
    if (!refreshValue.empty())
    {
        wchar_t *end = nullptr;
        long parsed = wcstol(refreshValue.c_str(), &end, 10);
        if (end != refreshValue.c_str() && *end == L'\0' && parsed >= 1 && parsed <= 720)
            hours = static_cast<int>(parsed);
    }
    if (hours == 0)
    {
        // Compatibility fallback for a previously stored numeric RefreshHours.
        int configuredHours = ReadCompatibleInt(refreshKey, 168);
        if (configuredHours >= 1 && configuredHours <= 720)
            hours = configuredHours;
    }
    if (hours == 0)
        hours = 168;
    AcquireSRWLockExclusive(&g_settingsLock);
    bool changed = g_mosque != mosque || g_refreshHours != hours;
    g_mosque = mosque;
    g_refreshHours = hours;
    if (changed)
        g_scheduleConfigGeneration.fetch_add(1, std::memory_order_release);
    ReleaseSRWLockExclusive(&g_settingsLock);
    Wh_Log(L"[Mawaqit] Settings: next=%d countdown=%d popup=%d theme=%s mosque=%s refresh=%d h "
           L"changed=%d",
           g_showNext ? 1 : 0, g_showCountdown ? 1 : 0, g_enablePopup ? 1 : 0, g_theme.c_str(), mosque.c_str(), hours,
           changed ? 1 : 0);
    if ((changed || previousHijriMode != g_hijriMode) && g_refreshEvent)
    {
        Wh_Log(L"[Mawaqit] Rechecking cached schedule after settings change");
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
    if (!taskxaml::HookTaskbarSymbols([]() {
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
    bool dispatched = OnTaskbarThread([](void *) { ApplyPendingSettingsOnUiThread(); });
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
                wnd, [](void *) { RemoveOnUiThread(); }, nullptr, L"Windhawk_Mawaqit_TaskbarDispatch", INFINITE))
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
