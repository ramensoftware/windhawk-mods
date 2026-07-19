// ==WindhawkMod==
// @id              tray-utility-customizer
// @name            Tray Utility Customizer
// @description     Arranges Windows tray utility controls such as Show hidden icons, Emoji, touch keyboard, pen menu, and virtual touchpad into a configurable row, column, or smart grid.
// @version         1.0
// @author          sb4ssman
// @github          https://github.com/sb4ssman
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lversion
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Tray Utility Customizer

Arranges low-frequency Windows 11 system-tray utility controls into one compact
row, column, or smart grid:

- **Show hidden icons** (the overflow chevron)
- **Emoji and more** (emoji, GIF, kaomoji, symbols, and clipboard history)
- **Touch keyboard**
- **Pen menu**
- **Virtual touchpad**
- **Input/language indicator**

Only controls detected on the current Windows build are included. The mod keeps
Windows-owned controls intact and moves their native tray hosts instead of
drawing replacement buttons or forwarding clicks.

![Native tray before the mod](https://raw.githubusercontent.com/sb4ssman/Windhawk-Mod-Lab/main/tray-utility-customizer/assets/disabled.png)
*Mod disabled: the chevron and the Emoji button sit in their native positions.*

![Row layout at the hidden-icons position](https://raw.githubusercontent.com/sb4ssman/Windhawk-Mod-Lab/main/tray-utility-customizer/assets/with-extra-icons-carot.png)
*Enabled: the Emoji button is gathered in next to the chevron.*

![Stacked column on a taller taskbar](https://raw.githubusercontent.com/sb4ssman/Windhawk-Mod-Lab/main/tray-utility-customizer/assets/with-carot-stacked.png)
*Smart automatic stacks the pair into a column once the taskbar is tall enough.*

## Layout

- **Smart automatic** picks the densest sensible grid for the item count and
  the current taskbar height, tuned by the Smart layout preference (balanced,
  pack vertical, or pack horizontal). It never exceeds the tray height.
- **Single row** and **Single column** are exactly that.
- **Fixed rows / Fixed columns / Fixed grid** honor the requested shape even if
  it is taller than the tray; use the minimum tray height guard to keep short
  taskbars native.

When the last grid row or column is not full, the short group can be placed
first or last and aligned to the start, center, or end.

## Position

The group can borrow the hidden-icons or Emoji column, or lease a dedicated
tray column before the notification icons, before Wi-Fi/volume/battery, before
or after the clock, or after the Show Desktop strip. The lease is
marker-tracked and fully reversible on unload.

Two **experimental** positions relocate the group out of the tray entirely:
**Left of Start** and **Right of Start** move the native hosts into a small
overlay beside the Start button and push the task list right to reserve room.
The group follows Start as the taskbar re-centers and everything returns to
the tray on unload. Primary taskbar only.

## Detection

- **Automatic** uses Windows accessibility metadata and a guarded Emoji
  fallback.
- **Force MainStack** allows the complete native `MainStack` to participate
  when Windows doesn't expose useful metadata. It can include unrelated
  indicators.

Use `itemOrder` to select and order utilities. If multiple selected utilities
belong to one indivisible Windows host, they stay bundled together and the log
identifies the shared host.

Utilities that Windows currently hides — a control toggled off in taskbar
settings, or a transient one like the touch keyboard — don't occupy a cell.
The grid re-adapts and re-centers automatically as they appear and disappear.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- enabled: true
  $name: Enable utility layout

- position: overflow
  $name: Position
  $description: >-
    Borrow a native utility column, lease a dedicated column elsewhere in the
    system tray, or experimentally relocate the group beside Start.
  $options:
  - overflow: Hidden-icons column
  - emoji: Emoji column
  - beforeIcons: Before notification icons
  - beforeOmni: Before network, volume, and battery
  - beforeClock: Before clock
  - afterClock: After clock
  - afterShowDesktop: After Show Desktop
  - leftOfStart: Left of Start (experimental)
  - rightOfStart: Right of Start (experimental)

- itemOrder: "overflow,emoji"
  $name: Utility items and order
  $description: >-
    Comma-separated utility tokens. Remove a token to exclude it. Only controls
    detected on this Windows build are included. Valid tokens: overflow, emoji,
    touchKeyboard, penMenu, virtualTouchpad, inputIndicator.

- gridMode: autoSmart
  $name: Grid mode
  $options:
  - autoSmart: Smart automatic
  - singleRow: Single row
  - singleColumn: Single column
  - fixedRows: Fixed rows
  - fixedColumns: Fixed columns
  - fixedGrid: Fixed rows and columns

- smartLayout: balanced
  $name: Smart layout
  $options:
  - balanced: Balanced
  - packVertical: Pack vertical
  - packHorizontal: Pack horizontal

- gridRows: 0
  $name: Rows (0 = auto)

- gridColumns: 0
  $name: Columns (0 = auto)

- fillOrder: rowFirst
  $name: Fill order
  $options:
  - rowFirst: Row first
  - columnFirst: Column first

- shortGroupPosition: last
  $name: Short row or column
  $options:
  - first: First
  - last: Last

- shortGroupAlign: center
  $name: Short row or column alignment
  $options:
  - start: Start
  - center: Center
  - end: End

- buttonWidth: 24
  $name: Button width (px)

- buttonHeight: 24
  $name: Button height (px)

- buttonSpacing: 0
  $name: Button spacing (px)
  $description: Horizontal and vertical gap between utility cells.

- groupOffsetX: 0
  $name: Group horizontal offset (px)

- groupOffsetY: 0
  $name: Group vertical offset (px)

- overflowOffsetX: 0
  $name: Hidden icons X offset (px)

- overflowOffsetY: 0
  $name: Hidden icons Y offset (px)

- emojiOffsetX: 0
  $name: Emoji X offset (px)

- emojiOffsetY: 0
  $name: Emoji Y offset (px)

- touchKeyboardOffsetX: 0
  $name: Touch keyboard X offset (px)

- touchKeyboardOffsetY: 0
  $name: Touch keyboard Y offset (px)

- penMenuOffsetX: 0
  $name: Pen menu X offset (px)

- penMenuOffsetY: 0
  $name: Pen menu Y offset (px)

- virtualTouchpadOffsetX: 0
  $name: Virtual touchpad X offset (px)

- virtualTouchpadOffsetY: 0
  $name: Virtual touchpad Y offset (px)

- inputIndicatorOffsetX: 0
  $name: Input indicator X offset (px)

- inputIndicatorOffsetY: 0
  $name: Input indicator Y offset (px)

- minimumTrayHeight: 44
  $name: Minimum tray height (px)
  $description: >-
    Below this height the mod leaves the native layout unchanged. Use 0 to
    allow stacking on any taskbar height.

- mergeMode: auto
  $name: Detection mode
  $description: >-
    Automatic is guarded and recommended. Force allows the complete native
    MainStack to be moved when Windows doesn't identify its controls.
  $options:
  - auto: Automatic
  - forceMainStack: Force MainStack (experimental)

- detailedLogging: false
  $name: Detailed discovery logging
  $description: >-
    Logs tray host names, classes, columns, visible IconView counts, and the
    accessibility metadata used to locate Emoji and more.
*/
// ==/WindhawkModSettings==

#include <atomic>
#include <algorithm>
#include <cmath>
#include <cwctype>
#include <functional>
#include <initializer_list>
#include <list>
#include <string>
#include <vector>

#include <windows.h>
#include <windhawk_api.h>
#include <windhawk_utils.h>

#undef GetCurrentTime

#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Media.h>

using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Automation;
using namespace winrt::Windows::UI::Xaml::Controls;
using namespace winrt::Windows::UI::Xaml::Media;

// ── Smart grid layout ─────────────────────────────────────────────────────
// Template block: _templates/smart-grid-layout.h v1.0 (verbatim copy — keep
// in sync with the template; Windhawk mods are single-file).

#include <climits>

namespace windhawk_mod_templates::smart_grid {

enum class GridMode {
    AutoSmart,
    SingleRow,
    SingleColumn,
    FixedRows,
    FixedColumns,
    FixedGrid,
};

enum class SmartLayout { Balanced, PackVertical, PackHorizontal };
enum class FillOrder { RowFirst, ColumnFirst };
enum class ShortGroupPosition { First, Last };
enum class ShortGroupAlign { Start, Center, End };

struct Config {
    GridMode mode = GridMode::AutoSmart;
    SmartLayout smartLayout = SmartLayout::Balanced;
    FillOrder fillOrder = FillOrder::RowFirst;
    ShortGroupPosition shortGroupPosition = ShortGroupPosition::Last;
    ShortGroupAlign shortGroupAlign = ShortGroupAlign::Center;
    int rows = 0;          // exact in fixed modes; maximum in AutoSmart
    int columns = 0;       // exact in fixed modes; maximum in AutoSmart
    int availableRows = 1; // derive from host height / item pitch
};

struct Layout {
    int rows = 1;
    int columns = 1;
};

// A short group may need to span its complete axis so a half-cell offset can
// be expressed with Margin. Multiply offsetUnits by item-size-plus-spacing.
struct Cell {
    int row = 0;
    int column = 0;
    int rowSpan = 1;
    int columnSpan = 1;
    double topOffsetUnits = 0.0;
    double leftOffsetUnits = 0.0;
};

inline int ScoreCandidate(int rows, int columns, int count,
                          SmartLayout preference) {
    int waste = rows * columns - count;
    int widePenalty = columns > rows ? (columns - rows) * 2 : 0;
    int score = waste * 10 + widePenalty;

    if (preference == SmartLayout::PackVertical)
        score -= rows * 20;
    else if (preference == SmartLayout::PackHorizontal)
        score += rows * 20;
    else
        score -= rows * 3;

    return score;
}

inline Layout ComputeLayout(int count, Config const& config) {
    count = std::max(1, count);
    Layout result;
    int availableRows = std::clamp(config.availableRows, 1, count);
    if (config.rows > 0 && config.mode == GridMode::AutoSmart)
        availableRows = std::min(availableRows, config.rows);

    switch (config.mode) {
        case GridMode::SingleRow:
            result = {1, count};
            break;
        case GridMode::SingleColumn:
            result = {count, 1};
            break;
        case GridMode::FixedRows:
            result.rows = std::clamp(config.rows, 1, count);
            result.columns = (count + result.rows - 1) / result.rows;
            break;
        case GridMode::FixedColumns:
            result.columns = std::clamp(config.columns, 1, count);
            result.rows = (count + result.columns - 1) / result.columns;
            break;
        case GridMode::FixedGrid:
            result.rows = std::clamp(config.rows, 1, count);
            result.columns = config.columns > 0
                ? std::clamp(config.columns, 1, count)
                : (count + result.rows - 1) / result.rows;
            if (result.rows * result.columns < count)
                result.rows = (count + result.columns - 1) / result.columns;
            break;
        case GridMode::AutoSmart: {
            int bestScore = INT_MAX;
            int firstRows = availableRows > 1 && count > 1 &&
                            config.smartLayout != SmartLayout::PackHorizontal
                ? 2 : 1;
            for (int rows = firstRows; rows <= availableRows; ++rows) {
                int columns = (count + rows - 1) / rows;
                if (config.columns > 0 && columns > config.columns)
                    continue;
                int score = ScoreCandidate(rows, columns, count,
                                           config.smartLayout);
                if (score < bestScore) {
                    bestScore = score;
                    result = {rows, columns};
                }
            }
            if (bestScore == INT_MAX) {
                result.columns = std::clamp(config.columns, 1, count);
                result.rows = (count + result.columns - 1) / result.columns;
            }
            break;
        }
    }

    result.rows = std::clamp(result.rows, 1, count);
    result.columns = std::max(1, result.columns);
    while (result.rows * result.columns < count) {
        if (config.mode == GridMode::FixedColumns)
            ++result.rows;
        else
            ++result.columns;
    }
    return result;
}

inline double AlignOffset(int capacity, int itemCount,
                          ShortGroupAlign alignment) {
    int unused = std::max(0, capacity - itemCount);
    if (alignment == ShortGroupAlign::Center)
        return unused / 2.0;
    if (alignment == ShortGroupAlign::End)
        return static_cast<double>(unused);
    return 0.0;
}

inline Cell GetCell(int index, int count, Layout const& layout,
                    Config const& config) {
    Cell cell;
    index = std::clamp(index, 0, std::max(0, count - 1));

    if (config.fillOrder == FillOrder::RowFirst) {
        int groupCount = (count + layout.columns - 1) / layout.columns;
        int shortCount = count % layout.columns;
        if (!shortCount) shortCount = layout.columns;
        int group;
        int itemInGroup;
        if (shortCount < layout.columns &&
            config.shortGroupPosition == ShortGroupPosition::First) {
            if (index < shortCount) {
                group = 0;
                itemInGroup = index;
            } else {
                int adjusted = index - shortCount;
                group = 1 + adjusted / layout.columns;
                itemInGroup = adjusted % layout.columns;
            }
        } else {
            group = index / layout.columns;
            itemInGroup = index % layout.columns;
        }
        int shortGroup = config.shortGroupPosition == ShortGroupPosition::First
            ? 0 : groupCount - 1;
        bool isShort = shortCount < layout.columns && group == shortGroup;

        cell.row = group;
        cell.column = itemInGroup;
        if (isShort && config.shortGroupAlign != ShortGroupAlign::Start) {
            cell.column = 0;
            cell.columnSpan = layout.columns;
            cell.leftOffsetUnits = AlignOffset(layout.columns, shortCount,
                                               config.shortGroupAlign) +
                                   itemInGroup;
        }
    } else {
        int groupCount = (count + layout.rows - 1) / layout.rows;
        int shortCount = count % layout.rows;
        if (!shortCount) shortCount = layout.rows;
        int group;
        int itemInGroup;
        if (shortCount < layout.rows &&
            config.shortGroupPosition == ShortGroupPosition::First) {
            if (index < shortCount) {
                group = 0;
                itemInGroup = index;
            } else {
                int adjusted = index - shortCount;
                group = 1 + adjusted / layout.rows;
                itemInGroup = adjusted % layout.rows;
            }
        } else {
            group = index / layout.rows;
            itemInGroup = index % layout.rows;
        }
        int shortGroup = config.shortGroupPosition == ShortGroupPosition::First
            ? 0 : groupCount - 1;
        bool isShort = shortCount < layout.rows && group == shortGroup;

        cell.row = itemInGroup;
        cell.column = group;
        if (isShort && config.shortGroupAlign != ShortGroupAlign::Start) {
            cell.row = 0;
            cell.rowSpan = layout.rows;
            cell.topOffsetUnits = AlignOffset(layout.rows, shortCount,
                                              config.shortGroupAlign) +
                                  itemInGroup;
        }
    }
    return cell;
}

} // namespace windhawk_mod_templates::smart_grid

// ── Injected grid column ──────────────────────────────────────────────────
// Template block: _templates/injected-grid-column.h v1.2 (verbatim copy —
// keep in sync with the template; Windhawk mods are single-file).

namespace windhawk_mod_templates::injected_grid_column {

using winrt::Windows::UI::Xaml::FrameworkElement;
using winrt::Windows::UI::Xaml::GridUnitType;
using winrt::Windows::UI::Xaml::Controls::ColumnDefinition;
using winrt::Windows::UI::Xaml::Controls::Grid;

enum class Anchor {
    BeforeIcons,
    BeforeOmni,
    BeforeClock,
    AfterClock,
    AfterShowDesktop,
};

struct Lease {
    std::wstring markerName;
    int column = -1;
};

inline FrameworkElement FindDirectChild(Grid const& parent,
                                        wchar_t const* name) {
    for (auto const& child : parent.Children()) {
        auto element = child.try_as<FrameworkElement>();
        if (element && element.Name() == name)
            return element;
    }
    return nullptr;
}

inline bool ResolveColumn(Grid const& parent, Anchor anchor, int& column) {
    if (anchor == Anchor::BeforeIcons) {
        column = 0;
        return true;
    }

    wchar_t const* referenceName = nullptr;
    bool after = false;
    switch (anchor) {
        case Anchor::BeforeOmni:
            referenceName = L"ControlCenterButton";
            break;
        case Anchor::BeforeClock:
            referenceName = L"NotificationCenterButton";
            break;
        case Anchor::AfterClock:
            referenceName = L"ShowDesktopStack";
            break;
        case Anchor::AfterShowDesktop:
            referenceName = L"ShowDesktopStack";
            after = true;
            break;
        case Anchor::BeforeIcons:
            break;
    }

    auto reference = FindDirectChild(parent, referenceName);
    if (!reference)
        return false; // Never silently turn an unavailable anchor into column 0.
    column = Grid::GetColumn(reference) +
             (after ? std::max(1, Grid::GetColumnSpan(reference)) : 0);
    return true;
}

inline bool AcquireAt(Grid const& parent, int column,
                      std::wstring const& markerName, Lease& lease) {
    if (!parent || column < 0 || markerName.empty() ||
        FindDirectChild(parent, markerName.c_str()))
        return false;

    ColumnDefinition definition;
    definition.Width({1.0, GridUnitType::Auto});
    if (static_cast<uint32_t>(column) < parent.ColumnDefinitions().Size())
        parent.ColumnDefinitions().InsertAt(column, definition);
    else
        parent.ColumnDefinitions().Append(definition);

    for (auto const& child : parent.Children()) {
        auto element = child.try_as<FrameworkElement>();
        if (!element) continue;
        int start = Grid::GetColumn(element);
        int span = Grid::GetColumnSpan(element);
        if (start >= column)
            Grid::SetColumn(element, start + 1);
        else if (start + span > column)
            Grid::SetColumnSpan(element, span + 1);
    }

    Grid marker;
    marker.Name(markerName);
    marker.Width(0.0);
    marker.Height(0.0);
    marker.IsHitTestVisible(false);
    Grid::SetColumn(marker, column);
    parent.Children().Append(marker);

    lease = {markerName, column};
    return true;
}

inline bool Acquire(Grid const& parent, Anchor anchor,
                    std::wstring const& markerName, Lease& lease) {
    int column = -1;
    if (!parent || !ResolveColumn(parent, anchor, column))
        return false;
    return AcquireAt(parent, column, markerName, lease);
}

inline bool Release(Grid const& parent, Lease& lease) {
    if (!parent || lease.markerName.empty())
        return false;

    uint32_t markerIndex = 0;
    bool found = false;
    int liveColumn = lease.column;
    for (uint32_t i = 0; i < parent.Children().Size(); ++i) {
        auto element = parent.Children().GetAt(i).try_as<FrameworkElement>();
        if (element && element.Name() == lease.markerName) {
            markerIndex = i;
            liveColumn = Grid::GetColumn(element);
            found = true;
            break;
        }
    }
    if (!found || liveColumn < 0)
        return false;

    parent.Children().RemoveAt(markerIndex);
    if (static_cast<uint32_t>(liveColumn) < parent.ColumnDefinitions().Size())
        parent.ColumnDefinitions().RemoveAt(liveColumn);

    for (auto const& child : parent.Children()) {
        auto element = child.try_as<FrameworkElement>();
        if (!element) continue;
        int start = Grid::GetColumn(element);
        int span = Grid::GetColumnSpan(element);
        if (start > liveColumn)
            Grid::SetColumn(element, start - 1);
        else if (start < liveColumn && start + span > liveColumn)
            Grid::SetColumnSpan(element, std::max(1, span - 1));
    }

    lease = {};
    return true;
}

} // namespace windhawk_mod_templates::injected_grid_column

namespace grid = windhawk_mod_templates::smart_grid;
namespace lease_column = windhawk_mod_templates::injected_grid_column;

// ── Settings ──────────────────────────────────────────────────────────────

enum class MergeMode {
    Auto,
    ForceMainStack,
};

enum class Position {
    Overflow,
    Emoji,
    BeforeIcons,
    BeforeOmni,
    BeforeClock,
    AfterClock,
    AfterShowDesktop,
    LeftOfStart,
    RightOfStart,
};

struct Settings {
    bool enabled;
    Position position;
    std::wstring itemOrder;
    grid::GridMode gridMode;
    grid::SmartLayout smartLayout;
    int gridRows;
    int gridColumns;
    grid::FillOrder fillOrder;
    grid::ShortGroupPosition shortGroupPosition;
    grid::ShortGroupAlign shortGroupAlign;
    int buttonWidth;
    int buttonHeight;
    int buttonSpacing;
    int groupOffsetX;
    int groupOffsetY;
    int overflowOffsetX;
    int overflowOffsetY;
    int emojiOffsetX;
    int emojiOffsetY;
    int touchKeyboardOffsetX;
    int touchKeyboardOffsetY;
    int penMenuOffsetX;
    int penMenuOffsetY;
    int virtualTouchpadOffsetX;
    int virtualTouchpadOffsetY;
    int inputIndicatorOffsetX;
    int inputIndicatorOffsetY;
    int minimumTrayHeight;
    MergeMode mergeMode;
    bool detailedLogging;
};

static Settings g_settings{};
static std::atomic<bool> g_unloading = false;
static std::atomic<bool> g_systemTrayModuleHooked = false;
static HWND g_taskbarWnd = nullptr;
static HANDLE g_retryStopEvent = nullptr;
static HANDLE g_retryThread = nullptr;
static std::list<FrameworkElement::Loaded_revoker> g_loadedRevokers;

struct HostSnapshot {
    FrameworkElement element{nullptr};
    FrameworkElement columnMarker{nullptr};
    int column = 0;
    int columnSpan = 1;
    int row = 0;
    int rowSpan = 1;
    double width = NAN;
    double height = NAN;
    double minWidth = 0;
    double minHeight = 0;
    double maxWidth = INFINITY;
    double maxHeight = INFINITY;
    Thickness margin{};
    HorizontalAlignment horizontalAlignment = HorizontalAlignment::Stretch;
    VerticalAlignment verticalAlignment = VerticalAlignment::Stretch;
    Transform renderTransform{nullptr};
    bool hadVisibleIconView = false;
};

struct UtilityHost {
    std::wstring token;
    FrameworkElement element{nullptr};
    int offsetX = 0;
    int offsetY = 0;
};

static std::vector<HostSnapshot> g_hostSnapshots;
static bool g_layoutApplied = false;
static Grid g_layoutGrid{nullptr};
static lease_column::Lease g_columnLease;

// Start-adjacent overlay state (experimental positions).
static Grid g_startGroupGrid{nullptr};
static Grid g_startRootGrid{nullptr};
static FrameworkElement g_startButton{nullptr};
static FrameworkElement g_taskItemsPanel{nullptr};
static Thickness g_taskItemsPanelOriginalMargin{};
static double g_startButtonOriginalX = -1.0;
static winrt::event_token g_startLayoutToken{};

static constexpr PCWSTR kLayoutColumnMarkerName =
    L"TrayUtilityCustomizerColumnMarker";

// ── Transient-host reapply plumbing ───────────────────────────────────────
// Windows hides/shows utility hosts live (taskbar settings toggles, the
// transient touch keyboard). Watch every candidate's visibility and re-run
// the whole layout when the visible set changes.

static bool ApplyLayout();

struct HostWatcher {
    FrameworkElement element{nullptr};
    int64_t token = 0;
};
static std::vector<HostWatcher> g_hostWatchers;
static winrt::event_token g_trayLayoutToken{};
static DispatcherTimer g_reapplyTimer{nullptr};

// Coalesces bursts (a settings toggle can flip several properties) and gets
// the reapply out of the property-changed/layout callback that noticed it.
static void ScheduleReapply() {
    if (g_unloading) {
        return;
    }
    try {
        if (!g_reapplyTimer) {
            g_reapplyTimer = DispatcherTimer();
            g_reapplyTimer.Interval(
                std::chrono::milliseconds{150});
            g_reapplyTimer.Tick(
                [](auto const&, auto const&) {
                    if (g_reapplyTimer) {
                        g_reapplyTimer.Stop();
                    }
                    if (!g_unloading) {
                        try {
                            ApplyLayout();
                        } catch (...) {
                            Wh_Log(
                                L"[Apply] Exception in "
                                L"scheduled reapply");
                        }
                    }
                });
        }
        g_reapplyTimer.Stop();
        g_reapplyTimer.Start();
    } catch (...) {
    }
}

static void ClearHostWatchers() {
    for (auto& watcher : g_hostWatchers) {
        try {
            watcher.element.UnregisterPropertyChangedCallback(
                UIElement::VisibilityProperty(), watcher.token);
        } catch (...) {
        }
    }
    g_hostWatchers.clear();
}

static void WatchHostVisibility(FrameworkElement const& element) {
    if (!element) {
        return;
    }
    for (auto const& watcher : g_hostWatchers) {
        if (watcher.element == element) {
            return;
        }
    }
    try {
        int64_t token = element.RegisterPropertyChangedCallback(
            UIElement::VisibilityProperty(),
            [](DependencyObject const&,
               DependencyProperty const&) {
                if (!g_unloading) {
                    ScheduleReapply();
                }
            });
        g_hostWatchers.push_back({element, token});
    } catch (...) {
    }
}

static int ClampSetting(int value, int low, int high) {
    return value < low ? low : value > high ? high : value;
}

static std::wstring GetStringSetting(PCWSTR key) {
    PCWSTR value = Wh_GetStringSetting(key);
    std::wstring result = value ? value : L"";
    if (value) {
        Wh_FreeStringSetting(value);
    }
    return result;
}

static int GetOffsetSetting(PCWSTR key) {
    return ClampSetting(Wh_GetIntSetting(key), -100, 100);
}

static void LoadSettings() {
    g_settings.enabled = Wh_GetIntSetting(L"enabled") != 0;

    auto position = GetStringSetting(L"position");
    if (position == L"emoji") {
        g_settings.position = Position::Emoji;
    } else if (position == L"beforeIcons") {
        g_settings.position = Position::BeforeIcons;
    } else if (position == L"beforeOmni") {
        g_settings.position = Position::BeforeOmni;
    } else if (position == L"beforeClock") {
        g_settings.position = Position::BeforeClock;
    } else if (position == L"afterClock") {
        g_settings.position = Position::AfterClock;
    } else if (position == L"afterShowDesktop") {
        g_settings.position = Position::AfterShowDesktop;
    } else if (position == L"leftOfStart") {
        g_settings.position = Position::LeftOfStart;
    } else if (position == L"rightOfStart") {
        g_settings.position = Position::RightOfStart;
    } else {
        g_settings.position = Position::Overflow;
    }

    g_settings.itemOrder = GetStringSetting(L"itemOrder");
    if (g_settings.itemOrder.empty()) {
        g_settings.itemOrder = L"overflow,emoji";
    }

    auto gridMode = GetStringSetting(L"gridMode");
    if (gridMode == L"singleRow") {
        g_settings.gridMode = grid::GridMode::SingleRow;
    } else if (gridMode == L"singleColumn") {
        g_settings.gridMode = grid::GridMode::SingleColumn;
    } else if (gridMode == L"fixedRows") {
        g_settings.gridMode = grid::GridMode::FixedRows;
    } else if (gridMode == L"fixedColumns") {
        g_settings.gridMode = grid::GridMode::FixedColumns;
    } else if (gridMode == L"fixedGrid") {
        g_settings.gridMode = grid::GridMode::FixedGrid;
    } else {
        g_settings.gridMode = grid::GridMode::AutoSmart;
    }

    auto smartLayout = GetStringSetting(L"smartLayout");
    if (smartLayout == L"packVertical") {
        g_settings.smartLayout = grid::SmartLayout::PackVertical;
    } else if (smartLayout == L"packHorizontal") {
        g_settings.smartLayout = grid::SmartLayout::PackHorizontal;
    } else {
        g_settings.smartLayout = grid::SmartLayout::Balanced;
    }

    g_settings.gridRows =
        ClampSetting(Wh_GetIntSetting(L"gridRows"), 0, 8);
    g_settings.gridColumns =
        ClampSetting(Wh_GetIntSetting(L"gridColumns"), 0, 8);
    g_settings.fillOrder =
        GetStringSetting(L"fillOrder") == L"columnFirst"
            ? grid::FillOrder::ColumnFirst
            : grid::FillOrder::RowFirst;
    g_settings.shortGroupPosition =
        GetStringSetting(L"shortGroupPosition") == L"first"
            ? grid::ShortGroupPosition::First
            : grid::ShortGroupPosition::Last;
    auto shortAlign = GetStringSetting(L"shortGroupAlign");
    if (shortAlign == L"start") {
        g_settings.shortGroupAlign = grid::ShortGroupAlign::Start;
    } else if (shortAlign == L"end") {
        g_settings.shortGroupAlign = grid::ShortGroupAlign::End;
    } else {
        g_settings.shortGroupAlign = grid::ShortGroupAlign::Center;
    }

    g_settings.buttonWidth =
        ClampSetting(Wh_GetIntSetting(L"buttonWidth"), 16, 96);
    g_settings.buttonHeight =
        ClampSetting(Wh_GetIntSetting(L"buttonHeight"), 12, 64);
    g_settings.buttonSpacing =
        ClampSetting(Wh_GetIntSetting(L"buttonSpacing"), -16, 32);

    g_settings.groupOffsetX = GetOffsetSetting(L"groupOffsetX");
    g_settings.groupOffsetY = GetOffsetSetting(L"groupOffsetY");
    g_settings.overflowOffsetX = GetOffsetSetting(L"overflowOffsetX");
    g_settings.overflowOffsetY = GetOffsetSetting(L"overflowOffsetY");
    g_settings.emojiOffsetX = GetOffsetSetting(L"emojiOffsetX");
    g_settings.emojiOffsetY = GetOffsetSetting(L"emojiOffsetY");
    g_settings.touchKeyboardOffsetX =
        GetOffsetSetting(L"touchKeyboardOffsetX");
    g_settings.touchKeyboardOffsetY =
        GetOffsetSetting(L"touchKeyboardOffsetY");
    g_settings.penMenuOffsetX = GetOffsetSetting(L"penMenuOffsetX");
    g_settings.penMenuOffsetY = GetOffsetSetting(L"penMenuOffsetY");
    g_settings.virtualTouchpadOffsetX =
        GetOffsetSetting(L"virtualTouchpadOffsetX");
    g_settings.virtualTouchpadOffsetY =
        GetOffsetSetting(L"virtualTouchpadOffsetY");
    g_settings.inputIndicatorOffsetX =
        GetOffsetSetting(L"inputIndicatorOffsetX");
    g_settings.inputIndicatorOffsetY =
        GetOffsetSetting(L"inputIndicatorOffsetY");

    g_settings.minimumTrayHeight =
        ClampSetting(Wh_GetIntSetting(L"minimumTrayHeight"), 0, 160);
    g_settings.mergeMode =
        GetStringSetting(L"mergeMode") == L"forceMainStack"
            ? MergeMode::ForceMainStack
            : MergeMode::Auto;
    g_settings.detailedLogging =
        Wh_GetIntSetting(L"detailedLogging") != 0;
}

// ── Taskbar plumbing ──────────────────────────────────────────────────────

using RunFromWindowThreadProc_t = void (*)(void*);

static bool RunFromWindowThread(HWND hWnd,
                                RunFromWindowThreadProc_t proc,
                                void* procParam) {
    static const UINT message =
        RegisterWindowMessage(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);
    struct Param {
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
        [](int code, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (code == HC_ACTION) {
                const CWPSTRUCT* cwp =
                    reinterpret_cast<const CWPSTRUCT*>(lParam);
                if (cwp->message == RegisterWindowMessageW(
                                        L"Windhawk_RunFromWindowThread_" WH_MOD_ID)) {
                    auto* param = reinterpret_cast<Param*>(cwp->lParam);
                    param->proc(param->procParam);
                }
            }
            return CallNextHookEx(nullptr, code, wParam, lParam);
        },
        nullptr,
        threadId);
    if (!hook) {
        return false;
    }

    Param param{proc, procParam};
    DWORD_PTR messageResult = 0;
    bool sent =
        SendMessageTimeoutW(
            hWnd,
            message,
            0,
            reinterpret_cast<LPARAM>(&param),
            SMTO_ABORTIFHUNG | SMTO_BLOCK,
            3000,
            &messageResult) != 0;
    UnhookWindowsHookEx(hook);
    return sent;
}

static HWND FindCurrentProcessTaskbarWnd() {
    HWND result = nullptr;
    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            DWORD processId = 0;
            WCHAR className[32]{};
            if (GetWindowThreadProcessId(hWnd, &processId) &&
                processId == GetCurrentProcessId() &&
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

using CTaskBand_GetTaskbarHost_t =
    void* (WINAPI*)(void* pThis, void* taskbarHostSharedPtr);
static CTaskBand_GetTaskbarHost_t CTaskBand_GetTaskbarHost_Original;

using TaskbarHost_FrameHeight_t = int (WINAPI*)(void* pThis);
static TaskbarHost_FrameHeight_t TaskbarHost_FrameHeight_Original;

using std__Ref_count_base__Decref_t = void (WINAPI*)(void* pThis);
static std__Ref_count_base__Decref_t std__Ref_count_base__Decref_Original;

static void* CTaskBand_ITaskListWndSite_vftable = nullptr;

static XamlRoot GetTaskbarXamlRoot(HWND hTaskbarWnd) {
    if (!CTaskBand_GetTaskbarHost_Original ||
        !TaskbarHost_FrameHeight_Original ||
        !std__Ref_count_base__Decref_Original ||
        !CTaskBand_ITaskListWndSite_vftable) {
        return nullptr;
    }

    HWND taskSwWnd =
        reinterpret_cast<HWND>(GetProp(hTaskbarWnd, L"TaskbandHWND"));
    if (!taskSwWnd) {
        return nullptr;
    }

    void* taskBand =
        reinterpret_cast<void*>(GetWindowLongPtr(taskSwWnd, 0));
    if (!taskBand) {
        return nullptr;
    }

    void* taskBandForSite = taskBand;
    for (int i = 0;
         *reinterpret_cast<void**>(taskBandForSite) !=
             CTaskBand_ITaskListWndSite_vftable;
         i++) {
        if (i == 20) {
            return nullptr;
        }
        taskBandForSite =
            reinterpret_cast<void**>(taskBandForSite) + 1;
    }

    void* taskbarHostSharedPtr[2]{};
    CTaskBand_GetTaskbarHost_Original(taskBandForSite,
                                     taskbarHostSharedPtr);
    if (!taskbarHostSharedPtr[0] || !taskbarHostSharedPtr[1]) {
        if (taskbarHostSharedPtr[1]) {
            std__Ref_count_base__Decref_Original(
                taskbarHostSharedPtr[1]);
        }
        return nullptr;
    }

    size_t offset = 0x10;
    const BYTE* bytes =
        reinterpret_cast<const BYTE*>(TaskbarHost_FrameHeight_Original);
    if (bytes[0] == 0x48 && bytes[1] == 0x83 &&
        bytes[2] == 0xEC && bytes[4] == 0x48 &&
        bytes[5] == 0x83 && bytes[6] == 0xC1 &&
        bytes[7] <= 0x7F) {
        offset = bytes[7];
    } else {
        Wh_Log(L"[XamlRoot] Unsupported TaskbarHost::FrameHeight");
    }

    auto* unknown = *reinterpret_cast<IUnknown**>(
        reinterpret_cast<BYTE*>(taskbarHostSharedPtr[0]) + offset);
    if (!unknown) {
        std__Ref_count_base__Decref_Original(taskbarHostSharedPtr[1]);
        return nullptr;
    }

    FrameworkElement taskbarElement = nullptr;
    unknown->QueryInterface(winrt::guid_of<FrameworkElement>(),
                            winrt::put_abi(taskbarElement));
    auto result =
        taskbarElement ? taskbarElement.XamlRoot() : nullptr;
    std__Ref_count_base__Decref_Original(taskbarHostSharedPtr[1]);
    return result;
}

static FrameworkElement FindChildRecursive(
    FrameworkElement const& element,
    std::function<bool(FrameworkElement)> const& predicate,
    int maxDepth = 20) {
    if (!element || maxDepth <= 0) {
        return nullptr;
    }

    int count = VisualTreeHelper::GetChildrenCount(element);
    for (int i = 0; i < count; i++) {
        auto child = VisualTreeHelper::GetChild(element, i)
                         .try_as<FrameworkElement>();
        if (!child) {
            continue;
        }
        if (predicate(child)) {
            return child;
        }
        auto found =
            FindChildRecursive(child, predicate, maxDepth - 1);
        if (found) {
            return found;
        }
    }
    return nullptr;
}

// ── Utility discovery ─────────────────────────────────────────────────────

static std::wstring ToLower(std::wstring value) {
    for (auto& ch : value) {
        ch = static_cast<wchar_t>(towlower(ch));
    }
    return value;
}

static std::wstring GetElementMetadata(FrameworkElement const& element) {
    try {
        return ToLower(
            std::wstring(element.Name()) + L" " +
            std::wstring(AutomationProperties::GetAutomationId(element)) +
            L" " +
            std::wstring(AutomationProperties::GetName(element)) +
            L" " +
            std::wstring(AutomationProperties::GetHelpText(element)));
    } catch (...) {
        return L"";
    }
}

static bool MetadataContainsAny(
    FrameworkElement const& element,
    std::initializer_list<PCWSTR> terms) {
    auto metadata = GetElementMetadata(element);
    for (auto term : terms) {
        if (metadata.find(term) != std::wstring::npos) {
            return true;
        }
    }
    return false;
}

static FrameworkElement FindDirectTrayHost(
    Grid const& trayGrid,
    FrameworkElement element) {
    DependencyObject current = element;
    while (current) {
        auto parent = VisualTreeHelper::GetParent(current);
        if (!parent) {
            return nullptr;
        }
        if (parent == trayGrid) {
            return current.try_as<FrameworkElement>();
        }
        current = parent;
    }
    return nullptr;
}

static std::vector<std::wstring> ParseItemOrder(
    std::wstring value) {
    for (auto& ch : value) {
        if (ch == L',' || ch == L';' || iswspace(ch)) {
            ch = L' ';
        }
    }

    std::vector<std::wstring> result;
    size_t position = 0;
    while (position < value.size()) {
        while (position < value.size() &&
               iswspace(value[position])) {
            position++;
        }
        size_t end = position;
        while (end < value.size() && !iswspace(value[end])) {
            end++;
        }
        if (end > position) {
            auto token = value.substr(position, end - position);
            if (std::find(result.begin(), result.end(), token) ==
                result.end()) {
                result.push_back(std::move(token));
            }
        }
        position = end;
    }
    return result;
}

static int CountVisibleIconViews(FrameworkElement const& host) {
    int result = 0;
    std::function<void(FrameworkElement const&, int)> visit =
        [&](FrameworkElement const& element, int depth) {
            if (!element || depth > 12) {
                return;
            }
            int count = VisualTreeHelper::GetChildrenCount(element);
            for (int i = 0; i < count; i++) {
                auto child = VisualTreeHelper::GetChild(element, i)
                                 .try_as<FrameworkElement>();
                if (!child) {
                    continue;
                }
                if (winrt::get_class_name(child) ==
                        L"SystemTray.IconView" &&
                    child.Visibility() == Visibility::Visible) {
                    result++;
                }
                visit(child, depth + 1);
            }
        };
    visit(host, 0);
    return result;
}

// Windows disables a utility by gutting the inside of its host — removing
// or collapsing the IconView, sometimes behind a collapsed mid-level
// element — while the host itself stays parented and Visible. Effective
// visibility (self AND every ancestor) is the signal that survives all of
// those variants.
static void ScanIconViews(FrameworkElement const& element,
                          bool ancestorsVisible,
                          bool& hasAny,
                          bool& hasVisible,
                          int depth = 0) {
    if (!element || depth > 12) {
        return;
    }
    int count = VisualTreeHelper::GetChildrenCount(element);
    for (int i = 0; i < count; i++) {
        auto child = VisualTreeHelper::GetChild(element, i)
                         .try_as<FrameworkElement>();
        if (!child) {
            continue;
        }
        bool childVisible =
            ancestorsVisible &&
            child.Visibility() == Visibility::Visible;
        if (winrt::get_class_name(child) ==
            L"SystemTray.IconView") {
            hasAny = true;
            if (childVisible) {
                hasVisible = true;
            }
        }
        ScanIconViews(child, childVisible, hasAny, hasVisible,
                      depth + 1);
    }
}

static FrameworkElement FindUtilityElement(
    Grid const& trayGrid,
    std::wstring const& token) {
    return FindChildRecursive(
        trayGrid,
        [&](FrameworkElement element) {
            if (token == L"emoji") {
                return MetadataContainsAny(
                    element, {L"emoji", L"kaomoji"});
            }
            if (token == L"touchKeyboard") {
                return MetadataContainsAny(
                    element, {L"touch keyboard"});
            }
            if (token == L"penMenu") {
                return MetadataContainsAny(
                    element, {L"pen menu"});
            }
            if (token == L"virtualTouchpad") {
                return MetadataContainsAny(
                    element, {L"virtual touchpad"});
            }
            if (token == L"inputIndicator") {
                return MetadataContainsAny(
                    element,
                    {L"input indicator", L"language bar",
                     L"input language"});
            }
            return false;
        });
}

static std::vector<UtilityHost> DiscoverUtilityHosts(
    Grid const& trayGrid,
    FrameworkElement overflowHost,
    FrameworkElement mainStack) {
    std::vector<UtilityHost> result;
    auto tokens = ParseItemOrder(g_settings.itemOrder);

    for (auto const& token : tokens) {
        FrameworkElement host = nullptr;
        FrameworkElement inner = nullptr;
        if (token == L"overflow") {
            host = overflowHost;
        } else {
            inner = FindUtilityElement(trayGrid, token);
            if (inner) {
                host = FindDirectTrayHost(trayGrid, inner);
            }

            if (!host && token == L"emoji" && mainStack) {
                int visibleIcons = CountVisibleIconViews(mainStack);
                if (g_settings.mergeMode == MergeMode::ForceMainStack ||
                    visibleIcons == 1) {
                    host = mainStack;
                    Wh_Log(
                        L"[Discover] Emoji using MainStack fallback "
                        L"(visibleIcons=%d force=%d)",
                        visibleIcons,
                        g_settings.mergeMode ==
                            MergeMode::ForceMainStack);
                }
            }
        }

        if (!host) {
            Wh_Log(L"[Discover] Utility not found: %s",
                   token.c_str());
            continue;
        }

        // Watch even the hosts we skip, so a utility toggled on in
        // Windows settings (or a transient control appearing) re-runs
        // the layout and gets gathered in.
        WatchHostVisibility(host);
        if (inner) {
            WatchHostVisibility(inner);
        }
        if (host.Visibility() != Visibility::Visible ||
            (inner &&
             inner.Visibility() != Visibility::Visible)) {
            Wh_Log(
                L"[Discover] %s is hidden; leaving it native",
                token.c_str());
            continue;
        }
        bool hasAnyIconView = false;
        bool hasVisibleIconView = false;
        ScanIconViews(host, true, hasAnyIconView,
                      hasVisibleIconView);
        if (hasAnyIconView && !hasVisibleIconView) {
            Wh_Log(
                L"[Discover] %s content is hidden; "
                L"leaving it native",
                token.c_str());
            continue;
        }

        auto existing = std::find_if(
            result.begin(),
            result.end(),
            [&](UtilityHost const& utility) {
                return utility.element == host;
            });
        if (existing != result.end()) {
            Wh_Log(
                L"[Discover] %s shares native host with %s; "
                L"keeping the host bundled",
                token.c_str(),
                existing->token.c_str());
            continue;
        }

        UtilityHost utility;
        utility.token = token;
        utility.element = host;
        if (token == L"overflow") {
            utility.offsetX = g_settings.overflowOffsetX;
            utility.offsetY = g_settings.overflowOffsetY;
        } else if (token == L"emoji") {
            utility.offsetX = g_settings.emojiOffsetX;
            utility.offsetY = g_settings.emojiOffsetY;
        } else if (token == L"touchKeyboard") {
            utility.offsetX = g_settings.touchKeyboardOffsetX;
            utility.offsetY = g_settings.touchKeyboardOffsetY;
        } else if (token == L"penMenu") {
            utility.offsetX = g_settings.penMenuOffsetX;
            utility.offsetY = g_settings.penMenuOffsetY;
        } else if (token == L"virtualTouchpad") {
            utility.offsetX = g_settings.virtualTouchpadOffsetX;
            utility.offsetY = g_settings.virtualTouchpadOffsetY;
        } else if (token == L"inputIndicator") {
            utility.offsetX = g_settings.inputIndicatorOffsetX;
            utility.offsetY = g_settings.inputIndicatorOffsetY;
        }
        result.push_back(std::move(utility));
    }

    return result;
}

static void LogElement(FrameworkElement const& element,
                       PCWSTR prefix) {
    if (!g_settings.detailedLogging || !element) {
        return;
    }

    try {
        auto className = winrt::get_class_name(element);
        auto name = element.Name();
        auto automationId =
            AutomationProperties::GetAutomationId(element);
        auto automationName =
            AutomationProperties::GetName(element);
        Wh_Log(
            L"[Discover] %s class=%s name=%s automationId=%s "
            L"automationName=%s col=%d size=%.1fx%.1f visibleIcons=%d",
            prefix,
            className.c_str(),
            name.c_str(),
            automationId.c_str(),
            automationName.c_str(),
            Grid::GetColumn(element),
            element.ActualWidth(),
            element.ActualHeight(),
            CountVisibleIconViews(element));
    } catch (...) {
        Wh_Log(L"[Discover] Failed to log %s", prefix);
    }
}

// ── Snapshot / restore ────────────────────────────────────────────────────

static HostSnapshot CaptureHost(FrameworkElement const& element,
                                Grid const& trayGrid,
                                int markerIndex) {
    HostSnapshot snapshot;
    snapshot.element = element;
    snapshot.column = Grid::GetColumn(element);
    snapshot.columnSpan = Grid::GetColumnSpan(element);
    snapshot.row = Grid::GetRow(element);
    snapshot.rowSpan = Grid::GetRowSpan(element);
    snapshot.width = element.Width();
    snapshot.height = element.Height();
    snapshot.minWidth = element.MinWidth();
    snapshot.minHeight = element.MinHeight();
    snapshot.maxWidth = element.MaxWidth();
    snapshot.maxHeight = element.MaxHeight();
    snapshot.margin = element.Margin();
    snapshot.horizontalAlignment = element.HorizontalAlignment();
    snapshot.verticalAlignment = element.VerticalAlignment();
    snapshot.renderTransform = element.RenderTransform();

    bool hasAnyIconView = false;
    bool hasVisibleIconView = false;
    ScanIconViews(element, true, hasAnyIconView, hasVisibleIconView);
    snapshot.hadVisibleIconView = hasVisibleIconView;

    Grid marker;
    marker.Name(
        L"TrayUtilityCustomizerHostMarker_" +
        std::to_wstring(markerIndex));
    marker.Width(0);
    marker.Height(0);
    marker.MinWidth(0);
    marker.MinHeight(0);
    marker.MaxWidth(0);
    marker.MaxHeight(0);
    marker.IsHitTestVisible(false);
    Grid::SetColumn(marker, snapshot.column);
    Grid::SetColumnSpan(marker, 1);
    Grid::SetRow(marker, snapshot.row);
    Grid::SetRowSpan(marker, snapshot.rowSpan);
    trayGrid.Children().Append(marker);
    snapshot.columnMarker = marker;
    return snapshot;
}

static void RestoreHost(HostSnapshot& snapshot) {
    try {
        if (snapshot.element) {
            int restoreColumn = snapshot.column;
            if (snapshot.columnMarker) {
                restoreColumn = Grid::GetColumn(snapshot.columnMarker);
            }

            Grid::SetColumn(snapshot.element, restoreColumn);
            Grid::SetColumnSpan(snapshot.element, snapshot.columnSpan);
            Grid::SetRow(snapshot.element, snapshot.row);
            Grid::SetRowSpan(snapshot.element, snapshot.rowSpan);
            snapshot.element.Width(snapshot.width);
            snapshot.element.Height(snapshot.height);
            snapshot.element.MinWidth(snapshot.minWidth);
            snapshot.element.MinHeight(snapshot.minHeight);
            snapshot.element.MaxWidth(snapshot.maxWidth);
            snapshot.element.MaxHeight(snapshot.maxHeight);
            snapshot.element.Margin(snapshot.margin);
            snapshot.element.HorizontalAlignment(
                snapshot.horizontalAlignment);
            snapshot.element.VerticalAlignment(snapshot.verticalAlignment);
            snapshot.element.RenderTransform(snapshot.renderTransform);
        }

        if (snapshot.columnMarker && g_layoutGrid) {
            uint32_t markerIndex = 0;
            if (g_layoutGrid.Children().IndexOf(
                    snapshot.columnMarker, markerIndex)) {
                g_layoutGrid.Children().RemoveAt(markerIndex);
            }
        }
    } catch (...) {
        Wh_Log(L"[Restore] Native host restore failed");
    }
    snapshot = {};
}

// ── Start-adjacent overlay (experimental) ─────────────────────────────────
// Ported from taskbar-vd-switcher's Start placement, trimmed to two modes.
// The native hosts are reparented into a small owned Grid appended to the
// taskbar RootGrid, positioned beside Start, and the TaskbarFrameRepeater is
// pushed right to reserve room.

static FrameworkElement FindTaskbarRootGrid(FrameworkElement const& root) {
    auto taskbarFrame = FindChildRecursive(
        root,
        [](FrameworkElement element) {
            return winrt::get_class_name(element) ==
                   L"Taskbar.TaskbarFrame";
        });
    if (!taskbarFrame) {
        return nullptr;
    }

    int count = VisualTreeHelper::GetChildrenCount(taskbarFrame);
    for (int i = 0; i < count; i++) {
        auto child = VisualTreeHelper::GetChild(taskbarFrame, i)
                         .try_as<FrameworkElement>();
        if (child && child.Name() == L"RootGrid") {
            return child;
        }
    }
    return nullptr;
}

static FrameworkElement FindStartButton(FrameworkElement const& root) {
    return FindChildRecursive(
        root,
        [](FrameworkElement element) {
            return winrt::get_class_name(element) ==
                       L"Taskbar.ExperienceToggleButton" &&
                   AutomationProperties::GetAutomationId(element) ==
                       L"StartButton";
        });
}

static void PositionStartGroup() {
    if (!g_startGroupGrid || !g_startRootGrid || !g_startButton) {
        return;
    }

    try {
        double groupWidth = g_startGroupGrid.Width();
        double groupHeight = g_startGroupGrid.Height();
        bool startHidden =
            g_startButton.Visibility() == Visibility::Collapsed;
        double startWidth = g_startButton.ActualWidth();
        double startHeight = g_startButton.ActualHeight();
        if (startWidth <= 0.0 && !startHidden) {
            startWidth = 44.0;
        }
        if (startHeight <= 0.0) {
            startHeight = groupHeight;
        }

        // The reported x includes any counter-shift we applied, so back it
        // out to get the raw layout position. Whether Start rides the
        // repeater-margin push varies by build; instead of assuming, shift
        // Start by exactly the error between where it is and where this
        // mode wants it. y stays valid for vertical centering throughout.
        auto transform =
            g_startButton.TransformToVisual(g_startRootGrid);
        auto point = transform.TransformPoint({0.0f, 0.0f});
        auto existingShift =
            g_startButton.RenderTransform()
                .try_as<TranslateTransform>();
        double currentShift =
            existingShift ? existingShift.X() : 0.0;
        double rawX = point.X - currentShift;
        double anchorX = g_startButtonOriginalX >= 0.0
                             ? g_startButtonOriginalX
                             : rawX;

        double push =
            groupWidth + std::max(0, g_settings.buttonSpacing);
        if (g_taskItemsPanel) {
            auto margin = g_taskItemsPanel.Margin();
            double needed =
                g_taskItemsPanelOriginalMargin.Left + push;
            if (std::fabs(margin.Left - needed) > 0.5) {
                margin.Left = needed;
                g_taskItemsPanel.Margin(margin);
            }
        }

        // Left of Start wants Start pushed right of the group; Right of
        // Start wants it held at its original anchor with the group beside.
        double left;
        double desiredStartX;
        if (g_settings.position == Position::LeftOfStart) {
            left = 0.0;
            desiredStartX = anchorX + push;
        } else {
            left = anchorX + startWidth;
            desiredStartX = anchorX;
        }
        if (!startHidden) {
            double neededShift = desiredStartX - rawX;
            if (std::fabs(neededShift) <= 0.5) {
                if (existingShift || g_startButton.RenderTransform()) {
                    g_startButton.ClearValue(
                        UIElement::RenderTransformProperty());
                }
            } else if (std::fabs(currentShift - neededShift) > 0.5) {
                TranslateTransform startShift;
                startShift.X(neededShift);
                g_startButton.RenderTransform(startShift);
            }
        }

        double top =
            point.Y + (startHeight - groupHeight) / 2.0;
        if (top < 0.0) {
            top = 0.0;
        }
        double rootWidth = g_startRootGrid.ActualWidth();
        if (rootWidth > 0.0 && left + groupWidth > rootWidth) {
            left = std::max(0.0, rootWidth - groupWidth);
        }

        auto current = g_startGroupGrid.Margin();
        if (std::fabs(current.Left - left) > 0.5 ||
            std::fabs(current.Top - top) > 0.5) {
            g_startGroupGrid.Margin({left, top, 0.0, 0.0});
        }
    } catch (...) {
    }
}

static void RestoreStartOverlay() {
    if (!g_startGroupGrid) {
        return;
    }

    try {
        if (g_startRootGrid && g_startLayoutToken) {
            g_startRootGrid.LayoutUpdated(g_startLayoutToken);
        }

        // Send the native hosts home before tearing down the group;
        // RestoreHost then puts their properties and columns back.
        if (g_layoutGrid) {
            while (g_startGroupGrid.Children().Size() > 0) {
                auto child = g_startGroupGrid.Children()
                                 .GetAt(0)
                                 .try_as<FrameworkElement>();
                g_startGroupGrid.Children().RemoveAt(0);
                if (child) {
                    g_layoutGrid.Children().Append(child);
                }
            }
        }

        if (g_startRootGrid) {
            uint32_t index = 0;
            if (g_startRootGrid.Children().IndexOf(
                    g_startGroupGrid, index)) {
                g_startRootGrid.Children().RemoveAt(index);
            }
        }
        if (g_taskItemsPanel) {
            g_taskItemsPanel.Margin(g_taskItemsPanelOriginalMargin);
        }
        if (g_startButton) {
            g_startButton.ClearValue(
                UIElement::RenderTransformProperty());
        }
    } catch (...) {
        Wh_Log(L"[Restore] Start overlay restore failed");
    }
    g_startLayoutToken = {};
    g_startGroupGrid = nullptr;
    g_startRootGrid = nullptr;
    g_startButton = nullptr;
    g_taskItemsPanel = nullptr;
    g_taskItemsPanelOriginalMargin = {};
    g_startButtonOriginalX = -1.0;
}

static void RestoreLayout() {
    if (!g_layoutApplied) {
        return;
    }

    if (g_trayLayoutToken && g_layoutGrid) {
        try {
            g_layoutGrid.LayoutUpdated(g_trayLayoutToken);
        } catch (...) {
        }
    }
    g_trayLayoutToken = {};

    RestoreStartOverlay();

    if (!g_columnLease.markerName.empty() && g_layoutGrid) {
        if (!lease_column::Release(g_layoutGrid, g_columnLease)) {
            Wh_Log(
                L"[Restore] Dedicated-column marker missing; "
                L"leaving columns untouched");
            g_columnLease = {};
        }
    }
    for (auto& snapshot : g_hostSnapshots) {
        RestoreHost(snapshot);
    }
    g_hostSnapshots.clear();
    g_layoutGrid = nullptr;
    g_layoutApplied = false;
    Wh_Log(L"[Restore] Native utility layout restored");
}

// ── Layout application ────────────────────────────────────────────────────

// Margin-based placement: unlike a render transform, a margin participates
// in layout, so Windows anchors the utility's flyout at the host's real
// on-screen position.
static void ApplyHostLayout(FrameworkElement const& host,
                            int column,
                            double marginLeft,
                            double marginTop) {
    Grid::SetColumn(host, column);
    Grid::SetColumnSpan(host, 1);
    host.Width(static_cast<double>(g_settings.buttonWidth));
    host.Height(static_cast<double>(g_settings.buttonHeight));
    host.MinWidth(0);
    host.MinHeight(0);
    host.MaxWidth(static_cast<double>(g_settings.buttonWidth));
    host.MaxHeight(static_cast<double>(g_settings.buttonHeight));
    host.HorizontalAlignment(HorizontalAlignment::Left);
    host.VerticalAlignment(VerticalAlignment::Top);
    host.Margin(Thickness{marginLeft, marginTop, 0.0, 0.0});
    host.RenderTransform(nullptr);
}

static bool ApplyLayout() {
    ClearHostWatchers();
    RestoreLayout();

    HWND hWnd =
        g_taskbarWnd ? g_taskbarWnd : FindCurrentProcessTaskbarWnd();
    if (!hWnd) {
        return false;
    }
    g_taskbarWnd = hWnd;

    auto xamlRoot = GetTaskbarXamlRoot(hWnd);
    if (!xamlRoot) {
        Wh_Log(L"[Apply] Taskbar XAML root unavailable");
        return false;
    }

    auto root = xamlRoot.Content().try_as<FrameworkElement>();
    if (!root) {
        return false;
    }

    auto trayGridElement = FindChildRecursive(
        root,
        [](FrameworkElement element) {
            return element.Name() == L"SystemTrayFrameGrid";
        });
    auto trayGrid = trayGridElement.try_as<Grid>();
    if (!trayGrid) {
        Wh_Log(L"[Apply] SystemTrayFrameGrid not found");
        return false;
    }

    if (g_settings.detailedLogging) {
        auto children = trayGrid.Children();
        for (uint32_t i = 0; i < children.Size(); i++) {
            LogElement(children.GetAt(i).try_as<FrameworkElement>(),
                       L"tray child");
        }
    }

    if (!g_settings.enabled) {
        Wh_Log(L"[Apply] Disabled by setting");
        return true;
    }

    auto overflowHost =
        lease_column::FindDirectChild(trayGrid, L"NotifyIconStack");
    if (!overflowHost) {
        Wh_Log(L"[Apply] NotifyIconStack not found");
        return false;
    }

    auto mainStack =
        lease_column::FindDirectChild(trayGrid, L"MainStack");

    if (g_settings.minimumTrayHeight > 0 &&
        trayGrid.ActualHeight() <
            static_cast<double>(
                g_settings.minimumTrayHeight)) {
        Wh_Log(
            L"[Apply] Tray height %.1f is below minimum %d",
            trayGrid.ActualHeight(),
            g_settings.minimumTrayHeight);
        return true;
    }

    LogElement(overflowHost, L"overflow host");
    LogElement(mainStack, L"MainStack fallback");

    auto utilities = DiscoverUtilityHosts(
        trayGrid, overflowHost, mainStack);
    if (utilities.empty()) {
        Wh_Log(L"[Apply] No selected utility hosts found");
        return true;
    }

    g_layoutGrid = trayGrid;
    g_layoutApplied = true;
    for (int index = 0;
         index < static_cast<int>(utilities.size());
         index++) {
        auto const& utility = utilities[index];
        LogElement(utility.element, utility.token.c_str());
        g_hostSnapshots.push_back(
            CaptureHost(utility.element, trayGrid, index));
    }

    int count = static_cast<int>(utilities.size());
    double pitchX =
        g_settings.buttonWidth + g_settings.buttonSpacing;
    double pitchY =
        g_settings.buttonHeight + g_settings.buttonSpacing;

    grid::Config config;
    config.mode = g_settings.gridMode;
    config.smartLayout = g_settings.smartLayout;
    config.fillOrder = g_settings.fillOrder;
    config.shortGroupPosition = g_settings.shortGroupPosition;
    config.shortGroupAlign = g_settings.shortGroupAlign;
    config.rows = g_settings.gridRows;
    config.columns = g_settings.gridColumns;
    config.availableRows =
        pitchY > 0
            ? std::max(
                  1,
                  static_cast<int>(
                      (trayGrid.ActualHeight() +
                       g_settings.buttonSpacing) /
                      pitchY))
            : 1;

    auto layout = grid::ComputeLayout(count, config);
    double groupWidth =
        layout.columns * g_settings.buttonWidth +
        std::max(0, layout.columns - 1) *
            g_settings.buttonSpacing;
    double groupHeight =
        layout.rows * g_settings.buttonHeight +
        std::max(0, layout.rows - 1) *
            g_settings.buttonSpacing;
    if (groupHeight > trayGrid.ActualHeight()) {
        Wh_Log(
            L"[Layout] Requested layout needs %.1fpx but tray "
            L"height is %.1fpx; honoring the requested layout",
            groupHeight,
            trayGrid.ActualHeight());
    }

    FrameworkElement emojiHost = nullptr;
    for (auto const& utility : utilities) {
        if (utility.token == L"emoji") {
            emojiHost = utility.element;
            break;
        }
    }
    if (!emojiHost && g_settings.position == Position::Emoji) {
        auto emojiElement =
            FindUtilityElement(trayGrid, L"emoji");
        if (emojiElement) {
            emojiHost =
                FindDirectTrayHost(trayGrid, emojiElement);
        }
    }

    bool borrowPosition =
        g_settings.position == Position::Overflow ||
        g_settings.position == Position::Emoji;
    bool startPosition =
        g_settings.position == Position::LeftOfStart ||
        g_settings.position == Position::RightOfStart;
    int sharedColumn = -1;

    if (startPosition) {
        auto rootGrid =
            FindTaskbarRootGrid(root).try_as<Grid>();
        auto startButton = FindStartButton(root);
        if (!rootGrid || !startButton) {
            Wh_Log(
                L"[Apply] Start anchor unavailable; "
                L"leaving the native layout unchanged");
            RestoreLayout();
            return false;
        }

        Grid group;
        group.Width(groupWidth);
        group.Height(groupHeight);
        group.HorizontalAlignment(HorizontalAlignment::Left);
        group.VerticalAlignment(VerticalAlignment::Top);
        Grid::SetColumn(group, 0);
        Grid::SetColumnSpan(
            group,
            std::max(
                1,
                static_cast<int>(
                    rootGrid.ColumnDefinitions().Size())));
        Canvas::SetZIndex(group, 1000);
        rootGrid.Children().Append(group);

        g_startGroupGrid = group;
        g_startRootGrid = rootGrid;
        g_startButton = startButton;
        if (auto repeater = FindChildRecursive(
                rootGrid,
                [](FrameworkElement element) {
                    return element.Name() ==
                           L"TaskbarFrameRepeater";
                },
                1)) {
            g_taskItemsPanel = repeater;
            g_taskItemsPanelOriginalMargin = repeater.Margin();
        }
        try {
            auto transform =
                startButton.TransformToVisual(rootGrid);
            g_startButtonOriginalX =
                transform.TransformPoint({0.0f, 0.0f}).X;
        } catch (...) {
            g_startButtonOriginalX = -1.0;
        }

        for (auto const& utility : utilities) {
            uint32_t index = 0;
            if (trayGrid.Children().IndexOf(
                    utility.element, index)) {
                trayGrid.Children().RemoveAt(index);
            }
            group.Children().Append(utility.element);
        }
        sharedColumn = 0;
    } else if (borrowPosition && layout.columns == 1) {
        if (g_settings.position == Position::Emoji &&
            emojiHost) {
            sharedColumn = Grid::GetColumn(emojiHost);
        } else {
            if (g_settings.position == Position::Emoji) {
                Wh_Log(
                    L"[Apply] Emoji anchor unavailable; "
                    L"using hidden-icons column");
            }
            sharedColumn = Grid::GetColumn(overflowHost);
        }
    } else {
        bool acquired = false;
        if (borrowPosition) {
            int borrowColumn =
                g_settings.position == Position::Emoji &&
                        emojiHost
                    ? Grid::GetColumn(emojiHost)
                    : Grid::GetColumn(overflowHost);
            acquired = lease_column::AcquireAt(
                trayGrid, borrowColumn,
                kLayoutColumnMarkerName, g_columnLease);
        } else {
            lease_column::Anchor anchor =
                lease_column::Anchor::BeforeIcons;
            switch (g_settings.position) {
                case Position::BeforeOmni:
                    anchor = lease_column::Anchor::BeforeOmni;
                    break;
                case Position::BeforeClock:
                    anchor = lease_column::Anchor::BeforeClock;
                    break;
                case Position::AfterClock:
                    anchor = lease_column::Anchor::AfterClock;
                    break;
                case Position::AfterShowDesktop:
                    anchor =
                        lease_column::Anchor::AfterShowDesktop;
                    break;
                default:
                    break;
            }
            acquired = lease_column::Acquire(
                trayGrid, anchor,
                kLayoutColumnMarkerName, g_columnLease);
        }

        if (!acquired) {
            Wh_Log(
                L"[Apply] Requested position anchor unavailable; "
                L"leaving the native layout unchanged");
            RestoreLayout();
            return false;
        }

        // The hosts fan out with render transforms, which do not
        // participate in Auto column sizing, so the leased column
        // needs an explicit pixel width.
        trayGrid.ColumnDefinitions()
            .GetAt(static_cast<uint32_t>(g_columnLease.column))
            .Width(GridLength{groupWidth, GridUnitType::Pixel});
        sharedColumn = g_columnLease.column;
    }

    // The start-overlay group is exactly group-sized; a tray column is the
    // full tray height, so the grid gets centered in it vertically.
    double baseTop =
        startPosition
            ? 0.0
            : std::max(
                  0.0,
                  (trayGrid.ActualHeight() - groupHeight) / 2.0);
    for (int index = 0; index < count; index++) {
        auto cell = grid::GetCell(index, count, layout, config);
        double columnUnits = cell.column + cell.leftOffsetUnits;
        double rowUnits = cell.row + cell.topOffsetUnits;
        ApplyHostLayout(
            utilities[index].element,
            sharedColumn,
            columnUnits * pitchX +
                g_settings.groupOffsetX +
                utilities[index].offsetX,
            baseTop + rowUnits * pitchY +
                g_settings.groupOffsetY +
                utilities[index].offsetY);
    }

    if (startPosition) {
        PositionStartGroup();
        g_startLayoutToken = g_startRootGrid.LayoutUpdated(
            [](auto const&, auto const&) {
                if (!g_unloading) {
                    PositionStartGroup();
                }
            });
    }

    // Visibility watchers miss a host being removed from the tree
    // outright, so also verify on tray layout passes that every managed
    // host is still parented and visible.
    g_trayLayoutToken = trayGrid.LayoutUpdated(
        [](auto const&, auto const&) {
            if (g_unloading || !g_layoutApplied) {
                return;
            }
            // Throttle: layout passes come in bursts (animations, clock
            // ticks); one intactness check per 250 ms is plenty.
            static ULONGLONG lastCheckTick = 0;
            ULONGLONG nowTick = GetTickCount64();
            if (nowTick - lastCheckTick < 250) {
                return;
            }
            lastCheckTick = nowTick;
            for (auto const& snapshot : g_hostSnapshots) {
                if (!snapshot.element) {
                    continue;
                }
                bool gone = false;
                try {
                    gone =
                        !VisualTreeHelper::GetParent(
                            snapshot.element) ||
                        snapshot.element.Visibility() !=
                            Visibility::Visible;
                    if (!gone && snapshot.hadVisibleIconView) {
                        bool hasAnyIconView = false;
                        bool hasVisibleIconView = false;
                        ScanIconViews(snapshot.element, true,
                                      hasAnyIconView,
                                      hasVisibleIconView);
                        gone = !hasVisibleIconView;
                    }
                } catch (...) {
                    gone = true;
                }
                if (gone) {
                    ScheduleReapply();
                    return;
                }
            }
        });

    Wh_Log(
        L"[Apply] Utility layout applied: items=%d "
        L"columns=%d rows=%d trayColumn=%d dedicated=%d",
        count,
        layout.columns,
        layout.rows,
        sharedColumn,
        !g_columnLease.markerName.empty());
    return true;
}

static void ApplyLayoutOnWindowThread() {
    HWND hWnd =
        g_taskbarWnd ? g_taskbarWnd : FindCurrentProcessTaskbarWnd();
    if (!hWnd || g_unloading) {
        return;
    }
    RunFromWindowThread(
        hWnd,
        [](void*) {
            try {
                ApplyLayout();
            } catch (...) {
                Wh_Log(L"[Apply] Exception while applying layout");
            }
        },
        nullptr);
}

// ── Hooks and lifecycle ───────────────────────────────────────────────────

static VS_FIXEDFILEINFO* GetModuleVersionInfo(HMODULE module) {
    void* info = nullptr;
    UINT length = 0;
    HRSRC resource =
        FindResource(module, MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION);
    if (resource) {
        HGLOBAL loaded = LoadResource(module, resource);
        if (loaded) {
            void* data = LockResource(loaded);
            if (data &&
                (!VerQueryValue(data, L"\\", &info, &length) ||
                 length == 0)) {
                info = nullptr;
            }
        }
    }
    return static_cast<VS_FIXEDFILEINFO*>(info);
}

static HMODULE GetSystemTrayModuleHandle() {
    HMODULE module = GetModuleHandleW(L"SystemTray.dll");
    if (!module) {
        module = GetModuleHandleW(L"Taskbar.View.dll");
        if (module) {
            auto* version = GetModuleVersionInfo(module);
            WORD major =
                version ? HIWORD(version->dwFileVersionMS) : 0;
            if (!major || major >= 2604) {
                module = nullptr;
            }
        }
    }
    if (!module) {
        module = GetModuleHandleW(L"ExplorerExtensions.dll");
    }
    return module;
}

using IconView_IconView_t = void* (WINAPI*)(void*);
static IconView_IconView_t IconView_IconView_Original;

static void* WINAPI IconView_IconView_Hook(void* pThis) {
    void* result = IconView_IconView_Original(pThis);
    if (g_unloading) {
        return result;
    }

    FrameworkElement iconView = nullptr;
    reinterpret_cast<IUnknown**>(pThis)[1]->QueryInterface(
        winrt::guid_of<FrameworkElement>(),
        winrt::put_abi(iconView));
    if (!iconView) {
        return result;
    }

    g_loadedRevokers.emplace_back();
    auto revoker = std::prev(g_loadedRevokers.end());
    *revoker = iconView.Loaded(
        winrt::auto_revoke_t{},
        [revoker](auto const&, auto const&) {
            g_loadedRevokers.erase(revoker);
            if (!g_unloading) {
                ApplyLayoutOnWindowThread();
            }
        });
    return result;
}

using LoadLibraryExW_t =
    HMODULE (WINAPI*)(LPCWSTR, HANDLE, DWORD);
static LoadLibraryExW_t LoadLibraryExW_Original;

static bool HookSystemTraySymbols(HMODULE module) {
    // SystemTray.dll, Taskbar.View.dll, ExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK systemTrayHooks[] = {{
        {LR"(public: __cdecl winrt::SystemTray::implementation::IconView::IconView(void))"},
        &IconView_IconView_Original,
        IconView_IconView_Hook,
    }};
    return WindhawkUtils::HookSymbols(
        module, systemTrayHooks, ARRAYSIZE(systemTrayHooks));
}

static HMODULE WINAPI LoadLibraryExW_Hook(
    LPCWSTR fileName,
    HANDLE file,
    DWORD flags) {
    HMODULE module =
        LoadLibraryExW_Original(fileName, file, flags);
    if (module && fileName &&
        !g_systemTrayModuleHooked &&
        GetSystemTrayModuleHandle() == module &&
        !g_systemTrayModuleHooked.exchange(true)) {
        Wh_Log(L"[Hooks] System tray module loaded: %s", fileName);
        if (HookSystemTraySymbols(module)) {
            Wh_ApplyHookOperations();
        } else {
            g_systemTrayModuleHooked = false;
            Wh_Log(L"[Hooks] System tray symbol hooks failed");
        }
    }
    return module;
}

static bool HookTaskbarDllSymbols() {
    HMODULE module = LoadLibraryExW(
        L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) {
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {{LR"(const CTaskBand::`vftable'{for `ITaskListWndSite'})"},
         &CTaskBand_ITaskListWndSite_vftable},
        {{LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const )"},
         &CTaskBand_GetTaskbarHost_Original},
        {{LR"(public: int __cdecl TaskbarHost::FrameHeight(void)const )"},
         &TaskbarHost_FrameHeight_Original},
        {{LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))"},
         &std__Ref_count_base__Decref_Original},
    };
    return WindhawkUtils::HookSymbols(
        module, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks));
}

static void StopRetryThread() {
    if (g_retryStopEvent) {
        SetEvent(g_retryStopEvent);
    }
    if (g_retryThread) {
        DWORD result;
        do {
            result = MsgWaitForMultipleObjects(
                1, &g_retryThread, FALSE, INFINITE, QS_SENDMESSAGE);
            if (result == WAIT_OBJECT_0 + 1) {
                MSG message;
                PeekMessageW(&message, nullptr, 0, 0, PM_NOREMOVE);
            }
        } while (result == WAIT_OBJECT_0 + 1);
        CloseHandle(g_retryThread);
        g_retryThread = nullptr;
    }
    if (g_retryStopEvent) {
        CloseHandle(g_retryStopEvent);
        g_retryStopEvent = nullptr;
    }
}

BOOL Wh_ModInit() {
    Wh_Log(L"[Init] Tray Utility Customizer v1.0");
    LoadSettings();

    if (!HookTaskbarDllSymbols()) {
        Wh_Log(L"[Init] taskbar.dll symbol hooks failed");
        return FALSE;
    }

    if (HMODULE module = GetSystemTrayModuleHandle()) {
        if (!HookSystemTraySymbols(module)) {
            Wh_Log(L"[Init] system tray symbol hooks failed");
            return FALSE;
        }
        g_systemTrayModuleHooked = true;
    } else {
        HMODULE kernelbase = GetModuleHandleW(L"kernelbase.dll");
        auto loadLibraryExW = kernelbase
            ? reinterpret_cast<LoadLibraryExW_t>(
                  GetProcAddress(kernelbase, "LoadLibraryExW"))
            : nullptr;
        if (loadLibraryExW) {
            WindhawkUtils::SetFunctionHook(
                loadLibraryExW,
                LoadLibraryExW_Hook,
                &LoadLibraryExW_Original);
        } else {
            Wh_Log(L"[Init] LoadLibraryExW hook unavailable");
            return FALSE;
        }
    }
    return TRUE;
}

void Wh_ModAfterInit() {
    ApplyLayoutOnWindowThread();

    g_retryStopEvent =
        CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_retryThread = CreateThread(
        nullptr,
        0,
        [](void*) -> DWORD {
            for (int attempt = 1;
                 attempt <= 6 && !g_unloading;
                 attempt++) {
                if (WaitForSingleObject(
                        g_retryStopEvent, 1500) != WAIT_TIMEOUT) {
                    break;
                }
                if (g_layoutApplied) {
                    break;
                }
                Wh_Log(L"[Retry] Layout attempt %d", attempt);
                ApplyLayoutOnWindowThread();
            }
            return 0;
        },
        nullptr,
        0,
        nullptr);
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    Wh_Log(L"[Settings] Reapplying");
    ApplyLayoutOnWindowThread();
}

void Wh_ModUninit() {
    g_unloading = true;
    Wh_Log(L"[Uninit]");
    StopRetryThread();

    HWND hWnd =
        g_taskbarWnd ? g_taskbarWnd : FindCurrentProcessTaskbarWnd();
    if (hWnd) {
        RunFromWindowThread(
            hWnd,
            [](void*) {
                g_loadedRevokers.clear();
                ClearHostWatchers();
                if (g_reapplyTimer) {
                    try {
                        g_reapplyTimer.Stop();
                    } catch (...) {
                    }
                    g_reapplyTimer = nullptr;
                }
                RestoreLayout();
            },
            nullptr);
    } else {
        g_loadedRevokers.clear();
        ClearHostWatchers();
        if (g_reapplyTimer) {
            try {
                g_reapplyTimer.Stop();
            } catch (...) {
            }
            g_reapplyTimer = nullptr;
        }
        RestoreLayout();
    }
}
