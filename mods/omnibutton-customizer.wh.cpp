// ==WindhawkMod==
// @id              omnibutton-customizer
// @name            OmniButton Customizer
// @description     Arrange and style each Windows 11 OmniButton item independently with custom grids, visibility, colors, fonts, opacity, and offsets
// @version         1.0
// @author          sb4ssman
// @github          https://github.com/sb4ssman
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lversion
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# OmniButton Customizer

Gives independent layout and appearance control over every native Windows 11
OmniButton item: wifi, volume/sound, battery, and battery percentage. Arrange
them into any grid, hide unneeded items, and tune each item's color, size, font,
opacity, and position. Designed for both standard and multi-row taskbars.

## Gallery

![Wifi, volume, battery, and percentage in a compact grid](https://raw.githubusercontent.com/sb4ssman/Windhawk-Mod-Lab/main/omnibutton-customizer/assets/in-a-grid.png)
*The default independent items arranged as a compact 2×2 grid on a standard-height taskbar.*

![Battery percentage, battery, volume, and wifi reordered into one row](https://raw.githubusercontent.com/sb4ssman/Windhawk-Mod-Lab/main/omnibutton-customizer/assets/re-odered-icons.png)
*The same native items reordered into a single row: percentage, battery, volume, then wifi.*

![OmniButton Customizer in a busy multi-row system tray](https://raw.githubusercontent.com/sb4ssman/Windhawk-Mod-Lab/main/omnibutton-customizer/assets/in-a-busy-tray.png)
*A compact custom OmniButton layout working alongside several other tray and taskbar mods.*

![Battery and percentage coupled in one cell](https://raw.githubusercontent.com/sb4ssman/Windhawk-Mod-Lab/main/omnibutton-customizer/assets/batt-percent-coupled.png)
*Coupled mode keeps the native battery and percentage together as one grid item.*

![Wifi, volume, battery, and percentage in a vertical layout](https://raw.githubusercontent.com/sb4ssman/Windhawk-Mod-Lab/main/omnibutton-customizer/assets/wifi-volume-batt-percent-vertical.png)
*All four independent items arranged vertically on a taller taskbar.*

![Wifi, volume, and battery in a vertical layout](https://raw.githubusercontent.com/sb4ssman/Windhawk-Mod-Lab/main/omnibutton-customizer/assets/wifi-vol-batt-vertical.png)
*A three-item vertical layout with the percentage omitted through `itemOrder`.*

## Battery / percent modes

**Coupled**: the selected battery and percentage elements stay in
their native inner panel and occupy one grid cell as a group. Each still has
independent appearance and nudge controls.

**Independent** (default): battery glyph and percentage text are separate grid
items and can be placed in any positions, including non-adjacent cells.

## Grid settings

- **Grid mode** — Smart automatic, single row/column, or fixed rows/columns/grid.
- **Smart layout** — balanced, vertical packing, or horizontal packing.
- **Slot width / height** — size of each grid cell. Height 0 = taskbar height ÷ rows.
- **Grid rows / columns** — dimensions used by the matching fixed mode.
- Smart automatic uses every row that can hold the 24px default pitch; four
  independent items therefore become a compact 2×2 on standard taskbar height.
- **Fill order** — row-first or column-first.
- **Short row or column** — when items don't divide evenly, whether the short
  row/column is first or last, and how it's aligned (start/center/end).

## Item order

`itemOrder` is a comma-separated list: `wifi`, `volume`, `battery`, `percent`.
Spaces are also accepted. Rearrange tokens to change grid order; omit a token
to hide that item. Items absent from Windows (for example, battery on a desktop)
are silently skipped.
In coupled mode, the first `battery` or `percent` token determines the shared
group's position.

## Per-item appearance

Each item has independent color, size, font family, opacity, and X/Y nudge
settings. Non-negative group padding controls reserved space around the grid;
group X/Y offsets move the complete OmniButton contents without changing tray
ordering. Padding values are clamped to 0–24px and offsets to -40–40px.
Colors accept `#RRGGBB`, `#AARRGGBB`, `accent`, `accentLight`, `accentDark`, or
`transparent`. Empty colors/fonts, size 0, and opacity -1 preserve native values.

## Placement and other taskbar mods

`groupPadding*` reserves non-negative space inside the native OmniButton and
`groupOffsetX`/`groupOffsetY` visually moves its grid. These controls preserve
the native `ControlCenterButton` in its original system-tray position, so other
mods' semantic anchors such as "before OmniButton" and "before clock" keep
their established meaning. The mod intentionally does not reorder the native
button across tray columns; doing so would require a shared placement lease so
multiple mods cannot claim contradictory anchor order.

## Presets

### Standard 2×2
`gridMode: fixedColumns` · `gridColumns: 2` · `fillOrder: rowFirst` · `batteryPercentMode: independent` · `itemOrder: "wifi, volume, battery, percent"`
(Smart automatic picks this shape on standard taskbar height)

### Single column — 3 icons (no percent)
`gridMode: singleColumn` · `itemOrder: "wifi, volume, battery"`

### Single column — all 4 icons
`gridMode: singleColumn` · `batteryPercentMode: independent` · `itemOrder: "wifi, volume, battery, percent"`

### Percent top, battery bottom (independent mode)
`gridMode: fixedColumns` · `gridColumns: 2` · `batteryPercentMode: independent` · `itemOrder: "wifi, volume, percent, battery"`

### Wide bar (original OmniButton style)
`gridMode: singleRow` · `itemOrder: "wifi, volume, battery"`

## Windows 11 Taskbar Styler compatibility

Does not use XAML Diagnostics. Compatible with Windows 11 Taskbar Styler.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- itemOrder: "wifi, volume, battery, percent"
  $name: Item order
  $description: >-
    Comma-separated tokens in grid fill order: wifi, volume, battery, percent.
    Spaces are also accepted. Omit a token to hide that item. Items unavailable
    in Windows are skipped.

- batteryPercentMode: independent
  $name: Battery / percent mode
  $description: >-
    Coupled: percent renders inside the battery slot (native behavior).
    Independent: battery glyph and percent are independent grid items and can be
    placed anywhere — even non-adjacent cells.
  $options:
  - coupled: Coupled
  - independent: Independent

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
  $description: Shape preference used by Smart automatic mode.
  $options:
  - balanced: Balanced
  - packVertical: Pack vertical
  - packHorizontal: Pack horizontal

- gridRows: 0
  $name: Rows (0 = auto)
  $description: >-
    Rows in the layout grid. 0 = automatic from the item count and taskbar
    height. Used by Fixed rows and Fixed grid modes.

- gridColumns: 0
  $name: Columns (0 = auto)
  $description: >-
    Columns in the layout grid. Used by Fixed columns and Fixed grid modes.

- fillOrder: rowFirst
  $name: Fill order
  $options:
  - rowFirst: Row first
  - columnFirst: Column first

- shortGroupPosition: last
  $name: Short row or column
  $description: When items don't divide evenly, where the short row/column goes.
  $options:
  - first: First
  - last: Last

- shortGroupAlign: center
  $name: Short row or column alignment
  $options:
  - start: Start
  - center: Center
  - end: End

- slotWidth: 32
  $name: Slot width (px)
  $description: >-
    Width of each grid column in pixels. Default 32 matches the standard icon size.

- slotHeight: 0
  $name: Slot height (px, 0 = auto)
  $description: >-
    Height of each grid row. 0 = taskbar height ÷ rows (minimum 16px, capped at 32px).

- wifiColor: ""
  $name: Wifi icon color
  $description: >-
    Hex ("#RRGGBB" or "#AARRGGBB"), "accent" / "accentLight" / "accentDark",
    or "transparent". Empty = theme default.

- volumeColor: ""
  $name: Volume icon color
  $description: >-
    Hex, "accent" / "accentLight" / "accentDark", or "transparent".
    Empty = theme default.

- batteryColor: ""
  $name: Battery icon color
  $description: >-
    Hex, "accent" / "accentLight" / "accentDark", or "transparent".
    Empty = theme default.

- percentColor: ""
  $name: Battery percent color
  $description: >-
    Hex, "accent" / "accentLight" / "accentDark", or "transparent".
    Empty = theme default.

- wifiSize: 0
  $name: Wifi glyph size (pt, 0 = native)
- volumeSize: 0
  $name: Volume glyph size (pt, 0 = native)
- batterySize: 0
  $name: Battery glyph size (pt, 0 = native)
- percentSize: 0
  $name: Battery percentage size (pt, 0 = native)

- wifiFontFamily: ""
  $name: Wifi font family
  $description: Empty preserves the native font.
- volumeFontFamily: ""
  $name: Volume font family
  $description: Empty preserves the native font.
- batteryFontFamily: ""
  $name: Battery font family
  $description: Empty preserves the native font.
- percentFontFamily: ""
  $name: Battery percentage font family
  $description: Empty preserves the native font.

- wifiOpacity: -1
  $name: Wifi opacity (-1 = native, 0-100%)
- volumeOpacity: -1
  $name: Volume opacity (-1 = native, 0-100%)
- batteryOpacity: -1
  $name: Battery opacity (-1 = native, 0-100%)
- percentOpacity: -1
  $name: Battery percentage opacity (-1 = native, 0-100%)

- groupPaddingLeft: 2
  $name: Group padding left (px)
  $description: Non-negative internal space, clamped to 0–24px.
- groupPaddingRight: 2
  $name: Group padding right (px)
  $description: Non-negative internal space, clamped to 0–24px.
- groupPaddingTop: 0
  $name: Group padding top (px)
  $description: Non-negative internal space, clamped to 0–24px.
- groupPaddingBottom: 0
  $name: Group padding bottom (px)
  $description: Non-negative internal space, clamped to 0–24px.
- groupOffsetX: 0
  $name: Group horizontal offset (px)
  $description: >-
    Moves the complete grid visually without changing tray ordering. Positive
    moves right; negative moves left. Clamped to -40–40px.
- groupOffsetY: 0
  $name: Group vertical offset (px)
  $description: >-
    Moves the complete grid visually without changing tray ordering. Positive
    moves down; negative moves up. Clamped to -40–40px.

- wifiOffsetX: 0
  $name: Wifi nudge X
  $description: Fine horizontal offset for the wifi glyph. Negative = left.

- wifiOffsetY: 0
  $name: Wifi nudge Y
  $description: Fine vertical offset for the wifi glyph. Negative = up.

- volumeOffsetX: 0
  $name: Volume nudge X
- volumeOffsetY: 0
  $name: Volume nudge Y

- batteryOffsetX: 0
  $name: Battery nudge X
- batteryOffsetY: 0
  $name: Battery nudge Y

- percentOffsetX: 0
  $name: Battery percent nudge X
- percentOffsetY: 0
  $name: Battery percent nudge Y
*/
// ==/WindhawkModSettings==

#include <algorithm>
#include <atomic>
#include <cmath>
#include <exception>
#include <functional>
#include <limits>
#include <list>
#include <vector>
#include <winrt/base.h>
#include <windhawk_api.h>
#include <windhawk_utils.h>

#undef GetCurrentTime

#include <windows.h>

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.h>
#include <winrt/Windows.UI.ViewManagement.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Media.h>

using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Controls;
using namespace winrt::Windows::UI::Xaml::Media;
using winrt::Windows::UI::Color;
using winrt::Windows::Foundation::IInspectable;

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

namespace grid = windhawk_mod_templates::smart_grid;

// ── Settings ───────────────────────────────────────────────────────────────

enum class BattPctMode { Coupled, Independent };

struct ModSettings {
    wchar_t    itemOrderStr[128];
    BattPctMode batteryPercentMode;
    grid::GridMode             gridMode;
    grid::SmartLayout          smartLayout;
    int        gridRows;
    int        gridColumns;
    grid::FillOrder          fillOrder;
    grid::ShortGroupPosition shortGroupPosition;
    grid::ShortGroupAlign    shortGroupAlign;
    int        slotWidth;
    int        slotHeight;
    wchar_t    wifiColor[32];
    wchar_t    volumeColor[32];
    wchar_t    batteryColor[32];
    wchar_t    percentColor[32];
    int        wifiSize, volumeSize, batterySize, percentSize;
    wchar_t    wifiFontFamily[64];
    wchar_t    volumeFontFamily[64];
    wchar_t    batteryFontFamily[64];
    wchar_t    percentFontFamily[64];
    int        wifiOpacity, volumeOpacity, batteryOpacity, percentOpacity;
    int        groupPaddingLeft, groupPaddingRight;
    int        groupPaddingTop, groupPaddingBottom;
    int        groupOffsetX, groupOffsetY;
    int        wifiX,    wifiY;
    int        volumeX,  volumeY;
    int        batteryX, batteryY;
    int        percentX, percentY;
};

[[clang::no_destroy]] static ModSettings g_settings{};

std::atomic<bool> g_unloading = false;

static void LoadColorSetting(PCWSTR key, wchar_t (&buf)[32]) {
    auto* s = Wh_GetStringSetting(key);
    if (s && *s) { wcsncpy(buf, s, 31); buf[31] = L'\0'; }
    else          buf[0] = L'\0';
    if (s) Wh_FreeStringSetting(s);
}

template <size_t N>
static void LoadStringSetting(PCWSTR key, wchar_t (&buf)[N]) {
    auto* value = Wh_GetStringSetting(key);
    if (value && *value) {
        wcsncpy(buf, value, N - 1);
        buf[N - 1] = L'\0';
    } else {
        buf[0] = L'\0';
    }
    if (value) Wh_FreeStringSetting(value);
}

static void LoadSettings() {
    auto clampSlot  = [](int v) { return v < 0 ? 0 : v > 80 ? 80 : v; };
    auto clampGrid  = [](int v) { return v < 0 ? 0 : v > 8  ? 8  : v; };
    auto clampPad   = [](int v) { return v < 0 ? 0 : v > 24 ? 24 : v; };
    auto clampNudge = [](int v) { return v < -40 ? -40 : v > 40 ? 40 : v; };
    auto clampSize  = [](int v) { return v < 0 ? 0 : v > 64 ? 64 : v; };
    auto clampOpacity = [](int v) { return v < -1 ? -1 : v > 100 ? 100 : v; };

    { auto* s = Wh_GetStringSetting(L"itemOrder");
      if (s && *s) {
          wcsncpy(g_settings.itemOrderStr, s, 127); g_settings.itemOrderStr[127] = L'\0';
          Wh_FreeStringSetting(s);
      } else {
          if (s) Wh_FreeStringSetting(s);
          wcscpy(g_settings.itemOrderStr, L"wifi, volume, battery, percent");
      } }

    { auto* s = Wh_GetStringSetting(L"batteryPercentMode");
      g_settings.batteryPercentMode = (s && wcscmp(s,L"coupled")==0)
                                    ? BattPctMode::Coupled : BattPctMode::Independent;
      if (s) Wh_FreeStringSetting(s); }

    { auto* s = Wh_GetStringSetting(L"gridMode");
      if      (s && wcscmp(s,L"singleRow")==0)    g_settings.gridMode = grid::GridMode::SingleRow;
      else if (s && wcscmp(s,L"singleColumn")==0) g_settings.gridMode = grid::GridMode::SingleColumn;
      else if (s && wcscmp(s,L"fixedRows")==0)    g_settings.gridMode = grid::GridMode::FixedRows;
      else if (s && wcscmp(s,L"fixedColumns")==0) g_settings.gridMode = grid::GridMode::FixedColumns;
      else if (s && wcscmp(s,L"fixedGrid")==0)    g_settings.gridMode = grid::GridMode::FixedGrid;
      else                                           g_settings.gridMode = grid::GridMode::AutoSmart;
      if (s) Wh_FreeStringSetting(s); }

    { auto* s = Wh_GetStringSetting(L"smartLayout");
      if      (s && wcscmp(s,L"packVertical")==0)   g_settings.smartLayout = grid::SmartLayout::PackVertical;
      else if (s && wcscmp(s,L"packHorizontal")==0) g_settings.smartLayout = grid::SmartLayout::PackHorizontal;
      else                                              g_settings.smartLayout = grid::SmartLayout::Balanced;
      if (s) Wh_FreeStringSetting(s); }

    g_settings.gridRows    = clampGrid(Wh_GetIntSetting(L"gridRows"));
    g_settings.gridColumns = clampGrid(Wh_GetIntSetting(L"gridColumns"));

    { auto* s = Wh_GetStringSetting(L"fillOrder");
      g_settings.fillOrder = (s && wcscmp(s,L"columnFirst")==0)
                           ? grid::FillOrder::ColumnFirst : grid::FillOrder::RowFirst;
      if (s) Wh_FreeStringSetting(s); }

    { auto* s = Wh_GetStringSetting(L"shortGroupPosition");
      g_settings.shortGroupPosition = (s && wcscmp(s,L"first")==0)
                                    ? grid::ShortGroupPosition::First
                                    : grid::ShortGroupPosition::Last;
      if (s) Wh_FreeStringSetting(s); }

    { auto* s = Wh_GetStringSetting(L"shortGroupAlign");
      if      (s && wcscmp(s,L"start")==0) g_settings.shortGroupAlign = grid::ShortGroupAlign::Start;
      else if (s && wcscmp(s,L"end")==0)   g_settings.shortGroupAlign = grid::ShortGroupAlign::End;
      else                                  g_settings.shortGroupAlign = grid::ShortGroupAlign::Center;
      if (s) Wh_FreeStringSetting(s); }

    int sw = clampSlot(Wh_GetIntSetting(L"slotWidth"));
    g_settings.slotWidth  = sw < 16 ? 32 : sw;
    g_settings.slotHeight = clampSlot(Wh_GetIntSetting(L"slotHeight"));

    LoadColorSetting(L"wifiColor",    g_settings.wifiColor);
    LoadColorSetting(L"volumeColor",  g_settings.volumeColor);
    LoadColorSetting(L"batteryColor", g_settings.batteryColor);
    LoadColorSetting(L"percentColor", g_settings.percentColor);

    g_settings.wifiSize    = clampSize(Wh_GetIntSetting(L"wifiSize"));
    g_settings.volumeSize  = clampSize(Wh_GetIntSetting(L"volumeSize"));
    g_settings.batterySize = clampSize(Wh_GetIntSetting(L"batterySize"));
    g_settings.percentSize = clampSize(Wh_GetIntSetting(L"percentSize"));
    LoadStringSetting(L"wifiFontFamily", g_settings.wifiFontFamily);
    LoadStringSetting(L"volumeFontFamily", g_settings.volumeFontFamily);
    LoadStringSetting(L"batteryFontFamily", g_settings.batteryFontFamily);
    LoadStringSetting(L"percentFontFamily", g_settings.percentFontFamily);
    g_settings.wifiOpacity    = clampOpacity(Wh_GetIntSetting(L"wifiOpacity"));
    g_settings.volumeOpacity  = clampOpacity(Wh_GetIntSetting(L"volumeOpacity"));
    g_settings.batteryOpacity = clampOpacity(Wh_GetIntSetting(L"batteryOpacity"));
    g_settings.percentOpacity = clampOpacity(Wh_GetIntSetting(L"percentOpacity"));

    g_settings.groupPaddingLeft = clampPad(Wh_GetIntSetting(L"groupPaddingLeft"));
    g_settings.groupPaddingRight = clampPad(Wh_GetIntSetting(L"groupPaddingRight"));
    g_settings.groupPaddingTop = clampPad(Wh_GetIntSetting(L"groupPaddingTop"));
    g_settings.groupPaddingBottom = clampPad(Wh_GetIntSetting(L"groupPaddingBottom"));
    g_settings.groupOffsetX = clampNudge(Wh_GetIntSetting(L"groupOffsetX"));
    g_settings.groupOffsetY = clampNudge(Wh_GetIntSetting(L"groupOffsetY"));

    g_settings.wifiX    = clampNudge(Wh_GetIntSetting(L"wifiOffsetX"));
    g_settings.wifiY    = clampNudge(Wh_GetIntSetting(L"wifiOffsetY"));
    g_settings.volumeX  = clampNudge(Wh_GetIntSetting(L"volumeOffsetX"));
    g_settings.volumeY  = clampNudge(Wh_GetIntSetting(L"volumeOffsetY"));
    g_settings.batteryX = clampNudge(Wh_GetIntSetting(L"batteryOffsetX"));
    g_settings.batteryY = clampNudge(Wh_GetIntSetting(L"batteryOffsetY"));
    g_settings.percentX = clampNudge(Wh_GetIntSetting(L"percentOffsetX"));
    g_settings.percentY = clampNudge(Wh_GetIntSetting(L"percentOffsetY"));
}

// ── Cached element references ─────────────────────────────────────────────

[[clang::no_destroy]] static StackPanel       g_omniStackPanel{ nullptr };
[[clang::no_destroy]] static FrameworkElement g_omniButton{ nullptr };
[[clang::no_destroy]] static FrameworkElement g_wifiPresenter{ nullptr };
[[clang::no_destroy]] static FrameworkElement g_volumePresenter{ nullptr };
[[clang::no_destroy]] static FrameworkElement g_batteryPresenter{ nullptr };
[[clang::no_destroy]] static StackPanel       g_batteryInnerPanel{ nullptr };
[[clang::no_destroy]] static FrameworkElement g_batteryGlyphFE{ nullptr };
[[clang::no_destroy]] static FrameworkElement g_batteryPercentFE{ nullptr };

[[clang::no_destroy]] static TextBlock g_wifiGlyphTB{ nullptr };
[[clang::no_destroy]] static TextBlock g_volumeGlyphTB{ nullptr };
[[clang::no_destroy]] static TextBlock g_batteryGlyphTB{ nullptr };
[[clang::no_destroy]] static TextBlock g_percentTB{ nullptr };

[[clang::no_destroy]] static StackPanel g_layoutUpdatedSP{ nullptr };
static winrt::event_token g_layoutUpdatedToken{};

static HWND g_taskbarWnd = nullptr;
static std::atomic<bool> g_applied{false};
static std::atomic<bool> g_reapplyPending{false};
static HANDLE g_retryThread = nullptr;
static HANDLE g_retryStopEvent = nullptr;

[[clang::no_destroy]] static std::list<FrameworkElement::Loaded_revoker>
    g_autoRevokerList;

struct PropertySnapshot {
    DependencyObject object{nullptr};
    DependencyProperty property{nullptr};
    IInspectable localValue{nullptr};
};

[[clang::no_destroy]] static std::vector<PropertySnapshot> g_propertySnapshots;

static void LogCurrentUiException(PCWSTR context) noexcept;

static void TrackProperty(DependencyObject const& object,
                          DependencyProperty const& property) {
    if (!object || !property) return;
    for (auto const& snapshot : g_propertySnapshots) {
        if (snapshot.object == object && snapshot.property == property) return;
    }
    g_propertySnapshots.push_back(
        {object, property, object.ReadLocalValue(property)});
}

static void RestorePropertySnapshots() {
    for (auto it = g_propertySnapshots.rbegin();
         it != g_propertySnapshots.rend(); ++it) {
        try {
            if (it->localValue == DependencyProperty::UnsetValue())
                it->object.ClearValue(it->property);
            else
                it->object.SetValue(it->property, it->localValue);
        } catch (...) {
            Wh_Log(L"[Cleanup] Failed to restore a XAML property");
        }
    }
    g_propertySnapshots.clear();
}

// ── Grid geometry ─────────────────────────────────────────────────────────
// All layout math comes from the smart-grid template; the mod-specific parts
// are the taskbar-height probe, mode derivation from the rows/columns
// settings, and the slot height.

struct GridGeom {
    grid::Config config;
    grid::Layout layout;
    int slotH;
};

static GridGeom ResolveGeometry(int itemCount, HWND hTaskbarWnd) {
    int taskbarH = 48;
    if (hTaskbarWnd) {
        RECT r{}; if (GetWindowRect(hTaskbarWnd, &r)) taskbarH = r.bottom - r.top;
    }
    if (itemCount < 1) itemCount = 1;

    GridGeom geom;
    geom.config.mode               = g_settings.gridMode;
    geom.config.smartLayout        = g_settings.smartLayout;
    geom.config.fillOrder          = g_settings.fillOrder;
    geom.config.shortGroupPosition = g_settings.shortGroupPosition;
    geom.config.shortGroupAlign    = g_settings.shortGroupAlign;
    geom.config.rows               = g_settings.gridRows;
    geom.config.columns            = g_settings.gridColumns;

    // Four independently controlled native visuals fit as a 2x2 at the 24px
    // default pitch on a standard taskbar. Explicit slotHeight remains an
    // intentional row-capacity override.
    int unit = g_settings.slotHeight > 0 ? g_settings.slotHeight : 24;
    geom.config.availableRows = std::max(1, taskbarH / std::max(1, unit));
    Wh_Log(L"[Layout] taskbarH=%d rowPitch=%d availableRows=%d gridMode=%d",
           taskbarH, unit, geom.config.availableRows,
           static_cast<int>(geom.config.mode));

    geom.layout = grid::ComputeLayout(itemCount, geom.config);

    geom.slotH = g_settings.slotHeight > 0 ? g_settings.slotHeight
                                           : (taskbarH / geom.layout.rows);
    if (geom.slotH > 32) geom.slotH = 32;
    if (geom.slotH < 16) geom.slotH = 16;
    return geom;
}

// ── XAML helpers ──────────────────────────────────────────────────────────

static void ApplyOffset(FrameworkElement const& fe, double x, double y) {
    if (!fe) return;
    TrackProperty(fe, UIElement::RenderTransformProperty());
    if (x != 0 || y != 0) {
        TranslateTransform tt; tt.X(double(x)); tt.Y(double(y));
        fe.RenderTransform(tt);
    } else {
        fe.ClearValue(UIElement::RenderTransformProperty());
    }
}

static bool HasBatteryDescendant(DependencyObject const& node, int depth = 0) {
    if (depth > 3) return false;
    int n = VisualTreeHelper::GetChildrenCount(node);
    for (int i = 0; i < n; i++) {
        auto child = VisualTreeHelper::GetChild(node, i);
        if (!child) continue;
        std::wstring cls(winrt::get_class_name(child).c_str());
        if (cls.find(L"Battery") != std::wstring::npos) return true;
        if (HasBatteryDescendant(child, depth + 1)) return true;
    }
    return false;
}

static bool WalkSetupBatteryInnerPanel(DependencyObject const& node, int slotW, int depth = 0) {
    if (depth > 5) return false;
    int n = VisualTreeHelper::GetChildrenCount(node);
    for (int i = 0; i < n; i++) {
        auto child = VisualTreeHelper::GetChild(node, i);
        if (!child) continue;
        auto sp = child.try_as<StackPanel>();
        if (sp && !sp.IsItemsHost()) {
            g_batteryInnerPanel = sp;
            g_batteryPercentFE = nullptr;
            int spN = VisualTreeHelper::GetChildrenCount(sp);
            Wh_Log(L"[Battery] inner panel name=%s orientation=%s children=%d size=%.1fx%.1f",
                   sp.Name().c_str(),
                   sp.Orientation() == Orientation::Horizontal ? L"horizontal"
                                                               : L"vertical",
                   spN, sp.ActualWidth(), sp.ActualHeight());
            if (spN >= 1) {
                auto glyph = VisualTreeHelper::GetChild(sp, 0).try_as<FrameworkElement>();
                if (glyph) {
                    g_batteryGlyphFE = glyph;
                    Wh_Log(L"[Battery] glyph class=%s name=%s size=%.1fx%.1f",
                           winrt::get_class_name(glyph).c_str(), glyph.Name().c_str(),
                           glyph.ActualWidth(), glyph.ActualHeight());
                }
            }
            if (spN >= 2) {
                auto percentElement = VisualTreeHelper::GetChild(sp, 1)
                                          .try_as<FrameworkElement>();
                if (percentElement) {
                    g_batteryPercentFE = percentElement;
                    auto textBlock = percentElement.try_as<TextBlock>();
                    Wh_Log(L"[Battery] percent class=%s name=%s text=%s size=%.1fx%.1f",
                           winrt::get_class_name(percentElement).c_str(),
                           percentElement.Name().c_str(),
                           textBlock ? textBlock.Text().c_str() : L"<non-TextBlock>",
                           percentElement.ActualWidth(),
                           percentElement.ActualHeight());
                }
            }
            if (!g_batteryPercentFE) {
                Wh_Log(L"[Battery] inner SP has %d children — no %% element", spN);
            }
            return true;
        }
        if (WalkSetupBatteryInnerPanel(child, slotW, depth + 1)) return true;
    }
    return false;
}

// ── Color / animation helpers ─────────────────────────────────────────────

// Color-returning variant of the canonical token parser (_templates/button-surface.h):
// "#RRGGBB" / "#AARRGGBB" hex (bare hex also accepted for back-compat), the
// generics "accent" / "accentLight" / "accentDark" / "transparent", and the
// numbered Windows shades "accentLight1"-"3" / "accentDark1"-"3" (accepted
// silently, undocumented). Empty/unparseable returns false = keep native.
static bool ParseHexColor(const wchar_t* s, Color& out) {
    using winrt::Windows::UI::ViewManagement::UIColorType;
    if (!s || !*s) return false;

    if (_wcsicmp(s, L"transparent") == 0) {
        out = {0, 0, 0, 0};
        return true;
    }

    static const struct { const wchar_t* token; UIColorType type; } kAccentTokens[] = {
        {L"accent",       UIColorType::Accent},
        {L"accentLight",  UIColorType::AccentLight2},
        {L"accentDark",   UIColorType::AccentDark1},
        {L"accentLight1", UIColorType::AccentLight1},
        {L"accentLight2", UIColorType::AccentLight2},
        {L"accentLight3", UIColorType::AccentLight3},
        {L"accentDark1",  UIColorType::AccentDark1},
        {L"accentDark2",  UIColorType::AccentDark2},
        {L"accentDark3",  UIColorType::AccentDark3},
    };
    for (auto const& entry : kAccentTokens) {
        if (_wcsicmp(s, entry.token) == 0) {
            try {
                winrt::Windows::UI::ViewManagement::UISettings settings;
                out = settings.GetColorValue(entry.type);
                return true;
            } catch (...) {
                Wh_Log(L"[Color] Failed to read the Windows accent color");
                return false;
            }
        }
    }

    const wchar_t* p = (*s == L'#') ? s + 1 : s;
    size_t len = wcslen(p);
    if (len != 6 && len != 8) return false;
    for (size_t i = 0; i < len; i++)
        if (!iswxdigit(p[i])) return false;
    wchar_t buf[9]{};
    wcsncpy(buf, p, 8);
    unsigned long v = wcstoul(buf, nullptr, 16);
    if (len == 6) { out = { 255, BYTE(v>>16), BYTE(v>>8), BYTE(v) }; }
    else          { out = { BYTE(v>>24), BYTE(v>>16), BYTE(v>>8), BYTE(v) }; }
    return true;
}

// Walk subtree looking for TextBlock named "InnerTextBlock" (the standard glyph element
// in Windows 11 system tray icon templates). Returns nullptr if not found.
static TextBlock FindInnerTextBlock(DependencyObject const& root, int depth = 0) {
    if (depth > 12) return nullptr;
    auto fe = root.try_as<FrameworkElement>();
    if (fe && fe.Name() == L"InnerTextBlock") {
        if (auto tb = fe.try_as<TextBlock>()) return tb;
    }
    int n = VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < n; i++) {
        auto child = VisualTreeHelper::GetChild(root, i);
        if (!child) continue;
        if (auto found = FindInnerTextBlock(child, depth + 1)) return found;
    }
    return nullptr;
}

static TextBlock FindFirstTextBlock(DependencyObject const& root, int depth = 0) {
    if (depth > 12) return nullptr;
    if (auto tb = root.try_as<TextBlock>()) return tb;
    int n = VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < n; i++) {
        auto child = VisualTreeHelper::GetChild(root, i);
        if (!child) continue;
        if (auto found = FindFirstTextBlock(child, depth + 1)) return found;
    }
    return nullptr;
}

static TextBlock AcquireGlyphTB(FrameworkElement const& root) {
    if (!root) return nullptr;
    if (auto tb = root.try_as<TextBlock>()) return tb;
    if (auto tb = FindInnerTextBlock(root)) return tb;
    return FindFirstTextBlock(root);  // fallback for battery/non-standard templates
}

static void ApplyGlyphColor(TextBlock const& tb, const wchar_t* colorStr) {
    if (!tb || !colorStr || !*colorStr) return;
    Color color{};
    if (!ParseHexColor(colorStr, color)) return;
    SolidColorBrush brush;
    brush.Color(color);
    TrackProperty(tb, TextBlock::ForegroundProperty());
    try { tb.Foreground(brush); } catch (...) {}
}

static void ApplyItemStyle(FrameworkElement const& element,
                           TextBlock const& textBlock,
                           const wchar_t* color, int size,
                           const wchar_t* fontFamily, int opacity) {
    if (textBlock) {
        ApplyGlyphColor(textBlock, color);
        if (size > 0) {
            TrackProperty(textBlock, TextBlock::FontSizeProperty());
            textBlock.FontSize(double(size));
        }
        if (fontFamily && *fontFamily) {
            TrackProperty(textBlock, TextBlock::FontFamilyProperty());
            textBlock.FontFamily(
                winrt::Windows::UI::Xaml::Media::FontFamily(fontFamily));
        }
    }
    if (element && opacity >= 0) {
        TrackProperty(element, UIElement::OpacityProperty());
        element.Opacity(double(opacity) / 100.0);
    }
}

static void ApplyAllItemStyles() {
    if (!g_wifiGlyphTB   && g_wifiPresenter)    g_wifiGlyphTB   = AcquireGlyphTB(g_wifiPresenter);
    if (!g_volumeGlyphTB && g_volumePresenter)   g_volumeGlyphTB = AcquireGlyphTB(g_volumePresenter);
    if (!g_batteryGlyphTB && g_batteryGlyphFE)   g_batteryGlyphTB = AcquireGlyphTB(g_batteryGlyphFE);
    if (!g_percentTB && g_batteryPercentFE)
        g_percentTB = AcquireGlyphTB(g_batteryPercentFE);

    ApplyItemStyle(g_wifiPresenter, g_wifiGlyphTB, g_settings.wifiColor,
                   g_settings.wifiSize, g_settings.wifiFontFamily,
                   g_settings.wifiOpacity);
    ApplyItemStyle(g_volumePresenter, g_volumeGlyphTB, g_settings.volumeColor,
                   g_settings.volumeSize, g_settings.volumeFontFamily,
                   g_settings.volumeOpacity);
    ApplyItemStyle(g_batteryGlyphFE, g_batteryGlyphTB,
                   g_settings.batteryColor, g_settings.batterySize,
                   g_settings.batteryFontFamily, g_settings.batteryOpacity);
    ApplyItemStyle(g_batteryPercentFE, g_percentTB, g_settings.percentColor,
                   g_settings.percentSize, g_settings.percentFontFamily,
                   g_settings.percentOpacity);
}

// ── Internal footprint ────────────────────────────────────────────────────
// The native ControlCenterButton owns taskbar placement and flyout semantics.
// Lease only its horizontal size so the complete internal grid isn't clipped;
// don't alter height or alignment.

static void ApplyItemsHostFootprint(StackPanel const& sp, const GridGeom& geom) {
    TrackProperty(sp, FrameworkElement::WidthProperty());
    TrackProperty(sp, FrameworkElement::HeightProperty());
    TrackProperty(sp, FrameworkElement::HorizontalAlignmentProperty());
    TrackProperty(sp, FrameworkElement::VerticalAlignmentProperty());
    double footprintWidth =
        double(geom.layout.columns * g_settings.slotWidth +
               g_settings.groupPaddingLeft + g_settings.groupPaddingRight);
    sp.Width(footprintWidth);
    sp.Height(double(geom.layout.rows * geom.slotH +
                     g_settings.groupPaddingTop +
                     g_settings.groupPaddingBottom));
    sp.HorizontalAlignment(HorizontalAlignment::Center);
    sp.VerticalAlignment(VerticalAlignment::Center);
    ApplyOffset(sp, g_settings.groupOffsetX, g_settings.groupOffsetY);
    sp.InvalidateMeasure();
    if (g_omniButton) {
        TrackProperty(g_omniButton, FrameworkElement::WidthProperty());
        TrackProperty(g_omniButton, FrameworkElement::MinWidthProperty());
        TrackProperty(g_omniButton, FrameworkElement::MaxWidthProperty());
        g_omniButton.Width(std::numeric_limits<double>::quiet_NaN());
        g_omniButton.MinWidth(footprintWidth);
        g_omniButton.MaxWidth(std::numeric_limits<double>::infinity());
        g_omniButton.InvalidateMeasure();
    }
}

// ── XAML cleanup ──────────────────────────────────────────────────────────
// Every mutated dependency property is leased through TrackProperty and
// restored to its exact prior local value. This is safer than guessing native
// defaults, which vary across Windows builds and taskbar templates.

static void ResetElementRefs() {
    g_omniStackPanel = nullptr; g_omniButton = nullptr;
    g_wifiPresenter = nullptr;  g_volumePresenter = nullptr;
    g_batteryPresenter = nullptr; g_batteryInnerPanel = nullptr;
    g_batteryGlyphFE = nullptr; g_batteryPercentFE = nullptr;
    g_wifiGlyphTB = nullptr;    g_volumeGlyphTB = nullptr;
    g_batteryGlyphTB = nullptr; g_percentTB = nullptr;
}

static void RevokeLayoutUpdated() {
    if (g_layoutUpdatedSP && g_layoutUpdatedToken.value) {
        g_layoutUpdatedSP.LayoutUpdated(g_layoutUpdatedToken);
        g_layoutUpdatedToken = {};
    }
    g_layoutUpdatedSP = nullptr;
}

static void CleanupAndResetCurrentElements() {
    RevokeLayoutUpdated();
    auto stackPanel = g_omniStackPanel;
    auto button = g_omniButton;
    RestorePropertySnapshots();
    try {
        if (stackPanel) {
            stackPanel.InvalidateMeasure();
            stackPanel.InvalidateArrange();
        }
        if (button) {
            button.InvalidateMeasure();
            button.InvalidateArrange();
            if (auto parent = VisualTreeHelper::GetParent(button)
                                  .try_as<UIElement>()) {
                parent.InvalidateMeasure();
                parent.InvalidateArrange();
            }
            button.UpdateLayout();
        }
    } catch (...) {
        LogCurrentUiException(L"native layout refresh");
    }
    ResetElementRefs();
}

// ── Layout application ────────────────────────────────────────────────────

struct ItemPositions {
    bool visible[4]{};
    int position[4]{-1, -1, -1, -1};
    int count = 0;
};

static bool ItemOrderContains(const wchar_t* orderStr, const wchar_t* name) {
    size_t nameLength = wcslen(name);
    const wchar_t* p = orderStr;
    while (*p) {
        while (*p == L' ' || *p == L'\t' || *p == L',') ++p;
        const wchar_t* start = p;
        while (*p && *p != L' ' && *p != L'\t' && *p != L',') ++p;
        if (size_t(p - start) == nameLength &&
            wcsncmp(start, name, nameLength) == 0)
            return true;
    }
    return false;
}

static ItemPositions ResolveItemPositions(const wchar_t* orderStr,
                                          bool hasWifi, bool hasVolume,
                                          bool hasBattery, bool hasPercent) {
    const bool present[4] = { hasWifi, hasVolume, hasBattery, hasPercent };
    const wchar_t* const names[4] = { L"wifi", L"volume", L"battery", L"percent" };
    ItemPositions result;
    bool batteryGroupAssigned = false;
    const wchar_t* p = orderStr;
    while (*p) {
        while (*p == L' ' || *p == L'\t' || *p == L',') p++;
        if (!*p) break;
        const wchar_t* start = p;
        while (*p && *p != L' ' && *p != L'\t' && *p != L',') p++;
        size_t len = size_t(p - start);
        for (int i = 0; i < 4; i++) {
            if (!result.visible[i] && present[i] &&
                wcslen(names[i]) == len && wcsncmp(start, names[i], len) == 0)
            {
                result.visible[i] = true;
                if (g_settings.batteryPercentMode == BattPctMode::Coupled &&
                    (i == 2 || i == 3)) {
                    if (!batteryGroupAssigned) {
                        result.position[2] = result.position[3] = result.count++;
                        batteryGroupAssigned = true;
                    }
                } else {
                    result.position[i] = result.count++;
                }
                break;
            }
        }
    }
    return result;
}

static void SetItemVisibility(FrameworkElement const& element, bool visible) {
    if (!element) return;
    TrackProperty(element, UIElement::VisibilityProperty());
    element.Visibility(visible ? Visibility::Visible : Visibility::Collapsed);
}

static void PrepareSlot(FrameworkElement const& element, int slotWidth,
                        int slotHeight) {
    if (!element) return;
    TrackProperty(element, FrameworkElement::WidthProperty());
    TrackProperty(element, FrameworkElement::HeightProperty());
    TrackProperty(element, FrameworkElement::HorizontalAlignmentProperty());
    TrackProperty(element, FrameworkElement::VerticalAlignmentProperty());
    element.Width(double(slotWidth));
    element.Height(double(slotHeight));
    element.HorizontalAlignment(HorizontalAlignment::Left);
    element.VerticalAlignment(VerticalAlignment::Top);
    if (auto presenter = element.try_as<ContentPresenter>()) {
        TrackProperty(presenter,
                      ContentPresenter::HorizontalContentAlignmentProperty());
        TrackProperty(presenter,
                      ContentPresenter::VerticalContentAlignmentProperty());
        presenter.HorizontalContentAlignment(HorizontalAlignment::Center);
        presenter.VerticalContentAlignment(VerticalAlignment::Center);
    }
}

struct CellContentMetrics {
    double centerX = 0;
    double centerY = 0;
    double naturalWidth = 0;
};

// Measure the native child at its desired size, then return the translation
// needed to center that complete visual in one slot. This works for direct
// TextBlocks and for Windows' custom battery-icon element without assuming an
// internal template or relying on manual nudges.
static CellContentMetrics PrepareIndependentItem(
    FrameworkElement const& element, int slotWidth, int slotHeight) {
    CellContentMetrics result;
    if (!element) return result;
    TrackProperty(element, FrameworkElement::WidthProperty());
    TrackProperty(element, FrameworkElement::HeightProperty());
    TrackProperty(element, FrameworkElement::HorizontalAlignmentProperty());
    TrackProperty(element, FrameworkElement::VerticalAlignmentProperty());
    element.Width(std::numeric_limits<double>::quiet_NaN());
    element.Height(std::numeric_limits<double>::quiet_NaN());
    element.HorizontalAlignment(HorizontalAlignment::Left);
    element.VerticalAlignment(VerticalAlignment::Top);
    element.Measure(winrt::Windows::Foundation::Size{
        std::numeric_limits<float>::infinity(),
        std::numeric_limits<float>::infinity()});
    auto desired = element.DesiredSize();
    result.naturalWidth = desired.Width;
    result.centerX = std::max(0.0,
        (double(slotWidth) - double(desired.Width)) / 2.0);
    result.centerY = std::max(0.0,
        (double(slotHeight) - double(desired.Height)) / 2.0);
    return result;
}

// Translate an item from its current natural StackPanel Y to an absolute pixel
// position within the grid footprint. Hidden siblings are collapsed and don't
// contribute to naturalY.
static void PositionSlot(FrameworkElement const& fe, int naturalY,
                         int x, int y, int nudgeX, int nudgeY)
{
    ApplyOffset(fe, x + nudgeX, -naturalY + y + nudgeY);
}

static void ApplyLayout(StackPanel const& sp, HWND hTaskbarWnd) {
    if (g_omniStackPanel) return;
    if (!sp.IsItemsHost()) return;

    g_omniStackPanel = sp;
    TrackProperty(sp, StackPanel::OrientationProperty());
    TrackProperty(sp, FrameworkElement::VerticalAlignmentProperty());
    TrackProperty(sp, StackPanel::SpacingProperty());
    sp.Orientation(Orientation::Vertical);
    sp.VerticalAlignment(VerticalAlignment::Top);
    sp.Spacing(0);

    int n = VisualTreeHelper::GetChildrenCount(sp);

    // Locate battery slot by class-name substring search
    int battIdx = -1;
    for (int i = 0; i < n; i++) {
        auto child = VisualTreeHelper::GetChild(sp, i).try_as<FrameworkElement>();
        if (!child) continue;
        bool battery = HasBatteryDescendant(child);
        Wh_Log(L"[Layout] native slot=%d class=%s name=%s battery=%d size=%.1fx%.1f",
               i, winrt::get_class_name(child).c_str(), child.Name().c_str(),
               battery, child.ActualWidth(), child.ActualHeight());
        if (battery && battIdx < 0) battIdx = i;
    }

    bool hasBattPres = battIdx >= 0;
    bool hasPercent  = false;
    if (hasBattPres) {
        g_batteryPresenter = VisualTreeHelper::GetChild(sp, battIdx).try_as<FrameworkElement>();
        if (WalkSetupBatteryInnerPanel(g_batteryPresenter, g_settings.slotWidth))
            hasPercent = (g_batteryPercentFE != nullptr);
    }

    // Log any unknown slots (future Windows builds may add more items)
    for (int i = 0; i < n; i++) {
        if (i == 0 || i == 1 || i == battIdx) continue;
        auto child = VisualTreeHelper::GetChild(sp, i);
        if (child) Wh_Log(L"[Layout] Unknown slot index %d: %s", i, winrt::get_class_name(child).c_str());
    }

    bool hasWifi = n >= 1;
    bool hasVolume = n >= 2;
    ItemPositions items = ResolveItemPositions(
        g_settings.itemOrderStr, hasWifi, hasVolume,
        hasBattPres && g_batteryGlyphFE, hasPercent);
    int itemCount = std::max(1, items.count);
    GridGeom geom = ResolveGeometry(itemCount, hTaskbarWnd);

    Wh_Log(L"[Layout] items=%d cols=%d rows=%d slotH=%d order=[%d,%d,%d,%d] visible=[%d,%d,%d,%d] mode=%s",
        items.count, geom.layout.columns, geom.layout.rows, geom.slotH,
        items.position[0], items.position[1], items.position[2],
        items.position[3], items.visible[0], items.visible[1],
        items.visible[2], items.visible[3],
        g_settings.batteryPercentMode == BattPctMode::Independent ? L"indep" : L"coupled");

    ApplyItemsHostFootprint(sp, geom);

    // Helper: absolute pixel position of a grid slot, including short-group
    // placement and alignment (template GetCell expresses those as fractional
    // offset units of one slot).
    auto resolvePos = [&](int gridPos, int& outX, int& outY) {
        grid::Cell c = grid::GetCell(gridPos, itemCount, geom.layout, geom.config);
        outX = g_settings.groupPaddingLeft +
               int((c.column + c.leftOffsetUnits) * g_settings.slotWidth);
        outY = g_settings.groupPaddingTop +
               int((c.row + c.topOffsetUnits) * geom.slotH);
    };

    // ── Wifi (slot 0) ──
    if (n >= 1) {
        auto wifi = VisualTreeHelper::GetChild(sp, 0).try_as<FrameworkElement>();
        if (wifi) {
            g_wifiPresenter = wifi;
            SetItemVisibility(wifi, items.visible[0]);
            if (items.visible[0]) {
                PrepareSlot(wifi, g_settings.slotWidth, geom.slotH);
                int x, y; resolvePos(items.position[0], x, y);
                PositionSlot(wifi, 0, x, y,
                             g_settings.wifiX, g_settings.wifiY);
            }
        }
    }
    // ── Volume (slot 1) ──
    if (n >= 2) {
        auto vol = VisualTreeHelper::GetChild(sp, 1).try_as<FrameworkElement>();
        if (vol) {
            g_volumePresenter = vol;
            SetItemVisibility(vol, items.visible[1]);
            if (items.visible[1]) {
                PrepareSlot(vol, g_settings.slotWidth, geom.slotH);
                int x, y; resolvePos(items.position[1], x, y);
                int naturalY = items.visible[0] ? geom.slotH : 0;
                PositionSlot(vol, naturalY, x, y,
                             g_settings.volumeX, g_settings.volumeY);
            }
        }
    }

    // Apply font/size before measuring direct TextBlocks for cell centering.
    ApplyAllItemStyles();

    // ── Battery + percent ──
    if (hasBattPres && g_batteryPresenter) {
        bool showBatteryGroup = items.visible[2] || items.visible[3];
        SetItemVisibility(g_batteryPresenter, showBatteryGroup);
        SetItemVisibility(g_batteryGlyphFE, items.visible[2]);
        SetItemVisibility(g_batteryPercentFE, items.visible[3]);
        if (!showBatteryGroup) {
            // Nothing else to do; Visibility participates in the tracked
            // property lease and is restored during settings changes/unload.
        } else if (g_settings.batteryPercentMode == BattPctMode::Independent) {
            // Independent mode: the battery presenter and inner StackPanel
            // span the full grid at its origin. The selected native children
            // receive one slot each and are translated to their template cells.

            int footprintWidth = geom.layout.columns * g_settings.slotWidth +
                                 g_settings.groupPaddingLeft +
                                 g_settings.groupPaddingRight;
            int footprintHeight = geom.layout.rows * geom.slotH +
                                  g_settings.groupPaddingTop +
                                  g_settings.groupPaddingBottom;
            TrackProperty(g_batteryPresenter, FrameworkElement::WidthProperty());
            TrackProperty(g_batteryPresenter, FrameworkElement::HeightProperty());
            TrackProperty(g_batteryPresenter, FrameworkElement::HorizontalAlignmentProperty());
            TrackProperty(g_batteryPresenter, FrameworkElement::VerticalAlignmentProperty());
            g_batteryPresenter.Width(double(footprintWidth));
            g_batteryPresenter.Height(double(footprintHeight));
            g_batteryPresenter.HorizontalAlignment(HorizontalAlignment::Left);
            g_batteryPresenter.VerticalAlignment(VerticalAlignment::Top);
            if (auto bcp = g_batteryPresenter.try_as<ContentPresenter>()) {
                TrackProperty(bcp, ContentPresenter::HorizontalContentAlignmentProperty());
                TrackProperty(bcp, ContentPresenter::VerticalContentAlignmentProperty());
                bcp.HorizontalContentAlignment(HorizontalAlignment::Left);
                bcp.VerticalContentAlignment(VerticalAlignment::Top);
            }
            // Move the presenter to the grid origin, accounting for collapsed
            // wifi/volume siblings that no longer contribute natural height.
            int batteryNaturalY =
                (items.visible[0] ? geom.slotH : 0) +
                (items.visible[1] ? geom.slotH : 0);
            ApplyOffset(g_batteryPresenter, 0, -batteryNaturalY);

            if (g_batteryInnerPanel) {
                TrackProperty(g_batteryInnerPanel, StackPanel::OrientationProperty());
                TrackProperty(g_batteryInnerPanel, FrameworkElement::WidthProperty());
                TrackProperty(g_batteryInnerPanel, FrameworkElement::HeightProperty());
                TrackProperty(g_batteryInnerPanel, FrameworkElement::HorizontalAlignmentProperty());
                TrackProperty(g_batteryInnerPanel, FrameworkElement::VerticalAlignmentProperty());
                g_batteryInnerPanel.Orientation(Orientation::Horizontal);
                g_batteryInnerPanel.Width(double(footprintWidth));
                g_batteryInnerPanel.Height(double(footprintHeight));
                g_batteryInnerPanel.HorizontalAlignment(HorizontalAlignment::Left);
                g_batteryInnerPanel.VerticalAlignment(VerticalAlignment::Top);
            }

            CellContentMetrics batteryMetrics = items.visible[2]
                ? PrepareIndependentItem(g_batteryGlyphFE,
                                         g_settings.slotWidth, geom.slotH)
                : CellContentMetrics{};
            CellContentMetrics percentMetrics = items.visible[3]
                ? PrepareIndependentItem(g_batteryPercentFE,
                                         g_settings.slotWidth, geom.slotH)
                : CellContentMetrics{};

            // Absolute position of battery glyph within battery CP
            int bx = 0, by = 0;
            if (items.visible[2]) {
                resolvePos(items.position[2], bx, by);
                ApplyOffset(g_batteryGlyphFE,
                    bx + batteryMetrics.centerX + g_settings.batteryX,
                    by + batteryMetrics.centerY + g_settings.batteryY);
            }

            // Absolute position of percent within the battery presenter. The
            // horizontal StackPanel naturally places it after the battery's
            // measured width, so subtract that natural origin first.
            int px = 0, py = 0;
            if (items.visible[3]) {
                resolvePos(items.position[3], px, py);
                double percentNaturalX = items.visible[2]
                    ? batteryMetrics.naturalWidth : 0;
                ApplyOffset(g_batteryPercentFE,
                    px - percentNaturalX + percentMetrics.centerX +
                        g_settings.percentX,
                    py + percentMetrics.centerY + g_settings.percentY);
            }

            Wh_Log(L"[Layout] Indep battCell=(%d,%d) center=(%.2f,%.2f) width=%.2f "
                   L"pctCell=(%d,%d) center=(%.2f,%.2f) width=%.2f",
                   bx, by, batteryMetrics.centerX, batteryMetrics.centerY,
                   batteryMetrics.naturalWidth, px, py,
                   percentMetrics.centerX, percentMetrics.centerY,
                   percentMetrics.naturalWidth);
        } else {
            // Coupled mode keeps the selected native battery/percent children
            // together as one grid item. Each child still has its own style
            // and fine offset.
            PrepareSlot(g_batteryPresenter, g_settings.slotWidth, geom.slotH);
            if (auto bcp = g_batteryPresenter.try_as<ContentPresenter>()) {
                TrackProperty(bcp, ContentPresenter::HorizontalContentAlignmentProperty());
                TrackProperty(bcp, ContentPresenter::VerticalContentAlignmentProperty());
                bcp.HorizontalContentAlignment(HorizontalAlignment::Center);
                bcp.VerticalContentAlignment(VerticalAlignment::Center);
            }
            if (g_batteryInnerPanel) {
                TrackProperty(g_batteryInnerPanel, StackPanel::OrientationProperty());
                g_batteryInnerPanel.Orientation(Orientation::Horizontal);
            }
            int bx, by;
            int groupPosition = items.visible[2] ? items.position[2]
                                                 : items.position[3];
            resolvePos(groupPosition, bx, by);
            int batteryNaturalY =
                (items.visible[0] ? geom.slotH : 0) +
                (items.visible[1] ? geom.slotH : 0);
            PositionSlot(g_batteryPresenter, batteryNaturalY, bx, by, 0, 0);
            if (items.visible[2])
                ApplyOffset(g_batteryGlyphFE, g_settings.batteryX,
                            g_settings.batteryY);
            if (items.visible[3])
                ApplyOffset(g_batteryPercentFE, g_settings.percentX,
                            g_settings.percentY);
            Wh_Log(L"[Layout] Coupled group=(%d,%d)px battery=%d percent=%d",
                   bx, by, items.visible[2], items.visible[3]);
        }
    }

    Wh_Log(L"[Layout] Applied grid (SP children=%d)", n);

}

// ── Deferred layout (LayoutUpdated) ──────────────────────────────────────

static void OnLayoutUpdatedImpl() {
    auto sp = g_layoutUpdatedSP;
    if (!sp) return;

    bool wantsPercent = ItemOrderContains(g_settings.itemOrderStr, L"percent");
    bool allReady = g_wifiPresenter && g_volumePresenter &&
                    (!g_batteryPresenter ||
                     (g_batteryInnerPanel &&
                      (!wantsPercent ||
                       g_batteryPercentFE)));
    if (allReady) {
        sp.LayoutUpdated(g_layoutUpdatedToken); g_layoutUpdatedToken = {};
        g_layoutUpdatedSP = nullptr;
        ApplyAllItemStyles();
        return;
    }

    bool batteryNowPresent = false;
    if (!g_batteryPresenter) {
        int nc = VisualTreeHelper::GetChildrenCount(sp);
        for (int i = 0; i < nc; i++) {
            auto child = VisualTreeHelper::GetChild(sp, i).try_as<FrameworkElement>();
            if (child && HasBatteryDescendant(child)) { batteryNowPresent = true; break; }
        }
    }
    bool percentNowPresent = g_batteryPresenter && !g_batteryInnerPanel &&
                             WalkSetupBatteryInnerPanel(g_batteryPresenter, g_settings.slotWidth);
    // Percent TextBlock materialized after the initial layout (e.g. the user
    // flipped the Windows battery-percent switch while the mod was applied).
    if (!percentNowPresent && g_batteryInnerPanel &&
        !g_batteryPercentFE &&
        VisualTreeHelper::GetChildrenCount(g_batteryInnerPanel) >= 2) {
        percentNowPresent = true;
    }

    if (!batteryNowPresent && !percentNowPresent) return;

    Wh_Log(L"[Layout] Deferred: battery=%d percent=%d — re-applying",
        (int)batteryNowPresent, (int)percentNowPresent);

    sp.LayoutUpdated(g_layoutUpdatedToken); g_layoutUpdatedToken = {};
    g_layoutUpdatedSP = nullptr;

    auto savedOmniButton = g_omniButton;
    CleanupAndResetCurrentElements();
    g_omniButton = savedOmniButton;
    ApplyLayout(sp, g_taskbarWnd);
}

static void OnLayoutUpdated(IInspectable const&, IInspectable const&) {
    try {
        OnLayoutUpdatedImpl();
    } catch (...) {
        LogCurrentUiException(L"LayoutUpdated");
        RevokeLayoutUpdated();
    }
}

// ── Taskbar and window thread helpers ─────────────────────────────────────

static HWND FindCurrentProcessTaskbarWnd() {
    HWND result = nullptr;
    EnumWindows([](HWND hWnd, LPARAM lParam) -> BOOL {
        DWORD pid; WCHAR cls[32];
        if (GetWindowThreadProcessId(hWnd, &pid) && pid == GetCurrentProcessId() &&
            GetClassName(hWnd, cls, ARRAYSIZE(cls)) && _wcsicmp(cls, L"Shell_TrayWnd") == 0) {
            *reinterpret_cast<HWND*>(lParam) = hWnd; return FALSE;
        }
        return TRUE;
    }, reinterpret_cast<LPARAM>(&result));
    return result;
}

using RunFromWindowThreadProc_t = void (*)(void*);
static void LogCurrentUiException(PCWSTR context) noexcept {
    try {
        throw;
    } catch (winrt::hresult_error const& error) {
        Wh_Log(L"[Lifecycle] %s failed hr=0x%08X: %s", context,
               static_cast<unsigned>(error.code().value),
               error.message().c_str());
    } catch (std::exception const&) {
        Wh_Log(L"[Lifecycle] %s failed with a C++ exception", context);
    } catch (...) {
        Wh_Log(L"[Lifecycle] %s failed with an unknown exception", context);
    }
}

static bool InvokeWindowThreadProc(RunFromWindowThreadProc_t proc,
                                   void* parameter) {
    try {
        proc(parameter);
        return true;
    } catch (...) {
        LogCurrentUiException(L"UI callback");
    }
    return false;
}

static bool RunFromWindowThread(HWND hWnd, RunFromWindowThreadProc_t proc,
                                void* procParam) {
    static UINT message = RegisterWindowMessageW(
        L"Windhawk_RunFromWindowThread_" WH_MOD_ID);
    struct Dispatch {
        RunFromWindowThreadProc_t proc;
        void* parameter;
        bool succeeded = false;
    };
    DWORD threadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (!threadId) return false;
    if (threadId == GetCurrentThreadId())
        return InvokeWindowThreadProc(proc, procParam);
    HHOOK hook = SetWindowsHookExW(
        WH_CALLWNDPROC,
        [](int code, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (code == HC_ACTION) {
                auto call = reinterpret_cast<CWPSTRUCT const*>(lParam);
                static UINT dispatchMessage = RegisterWindowMessageW(
                    L"Windhawk_RunFromWindowThread_" WH_MOD_ID);
                if (call->message == dispatchMessage) {
                    auto dispatch = reinterpret_cast<Dispatch*>(call->lParam);
                    dispatch->succeeded = InvokeWindowThreadProc(
                        dispatch->proc, dispatch->parameter);
                }
            }
            return CallNextHookEx(nullptr, code, wParam, lParam);
        },
        nullptr, threadId);
    if (!hook) return false;

    Dispatch dispatch{proc, procParam};
    SendMessageW(hWnd, message, 0, reinterpret_cast<LPARAM>(&dispatch));
    UnhookWindowsHookEx(hook);
    return dispatch.succeeded;
}

// ── GetTaskbarXamlRoot ────────────────────────────────────────────────────

using CTaskBand_GetTaskbarHost_t = void* (WINAPI*)(void*, void*);
CTaskBand_GetTaskbarHost_t CTaskBand_GetTaskbarHost_Original;
using TaskbarHost_FrameHeight_t  = int (WINAPI*)(void*);
TaskbarHost_FrameHeight_t  TaskbarHost_FrameHeight_Original;
using std__Ref_count_base__Decref_t = void (WINAPI*)(void*);
std__Ref_count_base__Decref_t std__Ref_count_base__Decref_Original;
static void* CTaskBand_ITaskListWndSite_vftable = nullptr;

static XamlRoot GetTaskbarXamlRoot(HWND hTaskbarWnd) {
    if (!CTaskBand_GetTaskbarHost_Original || !TaskbarHost_FrameHeight_Original ||
        !std__Ref_count_base__Decref_Original ||
        !CTaskBand_ITaskListWndSite_vftable) return nullptr;
    HWND hTaskSwWnd = (HWND)GetProp(hTaskbarWnd, L"TaskbandHWND");
    if (!hTaskSwWnd) return nullptr;
    void* taskBand = (void*)GetWindowLongPtr(hTaskSwWnd, 0);
    if (!taskBand) return nullptr;
    void* tbfs = taskBand;
    for (int i = 0; *(void**)tbfs != CTaskBand_ITaskListWndSite_vftable; i++) {
        if (i == 20) return nullptr; tbfs = (void**)tbfs + 1;
    }
    void* hsp[2]{};
    CTaskBand_GetTaskbarHost_Original(tbfs, hsp);
    if (!hsp[0] || !hsp[1]) {
        if (hsp[1]) std__Ref_count_base__Decref_Original(hsp[1]);
        return nullptr;
    }
    size_t offset = 0x10;
#if defined(_M_X64)
    {
        const BYTE* b = (const BYTE*)TaskbarHost_FrameHeight_Original;
        if (b[0]==0x48 && b[1]==0x83 && b[2]==0xEC && b[4]==0x48 &&
            b[5]==0x83 && b[6]==0xC1 && b[7]<=0x7F)
            offset = b[7];
        else Wh_Log(L"Unsupported TaskbarHost::FrameHeight");
    }
#elif defined(_M_ARM64)
    {
        const DWORD* p = (const DWORD*)TaskbarHost_FrameHeight_Original;
        if (p[0]==0xD503237F && (p[1]&0xFFC07FFF)==0xA9807BFD &&
            p[2]==0x910003FD && (p[3]&0xFFF00FE0)==0xF8400C00)
            offset = (p[3] >> 12) & 0xFF;
        else Wh_Log(L"Unsupported TaskbarHost::FrameHeight");
    }
#else
#error "Unsupported architecture"
#endif
    auto* iunk = *(IUnknown**)((BYTE*)hsp[0] + offset);
    if (!iunk) { std__Ref_count_base__Decref_Original(hsp[1]); return nullptr; }
    FrameworkElement taskbarElem = nullptr;
    iunk->QueryInterface(winrt::guid_of<FrameworkElement>(), winrt::put_abi(taskbarElem));
    auto result = taskbarElem ? taskbarElem.XamlRoot() : nullptr;
    std__Ref_count_base__Decref_Original(hsp[1]);
    return result;
}

// ── XAML tree helpers ─────────────────────────────────────────────────────

static FrameworkElement FindChildByClassName(FrameworkElement const& e, PCWSTR cls) {
    int n = VisualTreeHelper::GetChildrenCount(e);
    for (int i = 0; i < n; i++) {
        auto child = VisualTreeHelper::GetChild(e, i).try_as<FrameworkElement>();
        if (child && winrt::get_class_name(child) == cls) return child;
    }
    return nullptr;
}
static FrameworkElement FindChildByName(FrameworkElement const& e, PCWSTR name) {
    int n = VisualTreeHelper::GetChildrenCount(e);
    for (int i = 0; i < n; i++) {
        auto child = VisualTreeHelper::GetChild(e, i).try_as<FrameworkElement>();
        if (child && child.Name() == name) return child;
    }
    return nullptr;
}
static FrameworkElement FindChildRecursive(FrameworkElement const& e,
    std::function<bool(FrameworkElement)> const& cb, int maxDepth = 20)
{
    int n = VisualTreeHelper::GetChildrenCount(e);
    for (int i = 0; i < n && maxDepth > 0; i++) {
        auto child = VisualTreeHelper::GetChild(e, i).try_as<FrameworkElement>();
        if (!child) continue;
        if (cb(child)) return child;
        auto found = FindChildRecursive(child, cb, maxDepth - 1);
        if (found) return found;
    }
    return nullptr;
}

// ── Apply settings ────────────────────────────────────────────────────────

static bool ApplyAllSettings() {
    HWND hWnd = FindCurrentProcessTaskbarWnd();
    if (!hWnd) { Wh_Log(L"[Apply] No taskbar window"); return false; }
    g_taskbarWnd = hWnd;
    try {
        auto xamlRoot = GetTaskbarXamlRoot(hWnd);
        if (!xamlRoot) { Wh_Log(L"[Apply] GetTaskbarXamlRoot failed"); return false; }
        auto content = xamlRoot.Content().try_as<FrameworkElement>();
        if (!content) return false;
        if (!g_omniStackPanel) {
            auto omniBtn = FindChildRecursive(content, [](FrameworkElement fe) {
                return fe.Name() == L"ControlCenterButton"; });
            if (omniBtn) {
                g_omniButton = omniBtn;
                auto grid = FindChildByClassName(omniBtn, L"Windows.UI.Xaml.Controls.Grid");
                auto cp   = grid ? FindChildByName(grid, L"ContentPresenter") : nullptr;
                auto ip   = cp   ? FindChildByClassName(cp, L"Windows.UI.Xaml.Controls.ItemsPresenter") : nullptr;
                auto sp   = ip   ? FindChildByClassName(ip, L"Windows.UI.Xaml.Controls.StackPanel").try_as<StackPanel>() : nullptr;
                if (sp && sp.IsItemsHost()) {
                    ApplyLayout(sp, hWnd);
                    bool wantsPercent = ItemOrderContains(
                        g_settings.itemOrderStr, L"percent");
                    bool needsDeferred = !g_wifiPresenter || !g_volumePresenter ||
                                         (g_batteryPresenter &&
                                          (!g_batteryInnerPanel ||
                                           (wantsPercent &&
                                            !g_batteryPercentFE)));
                    if (needsDeferred) {
                        g_layoutUpdatedSP = sp;
                        g_layoutUpdatedToken = sp.LayoutUpdated(OnLayoutUpdated);
                        Wh_Log(L"[Apply] Registered LayoutUpdated for deferred elements");
                    }
                } else Wh_Log(L"[Apply] IsItemsHost StackPanel not found");
            } else Wh_Log(L"[Apply] ControlCenterButton not found");
        } else {
            // Already applied; late template materialization may expose glyph
            // TextBlocks after the initial pass.
            ApplyAllItemStyles();
        }
    } catch (...) {
        LogCurrentUiException(L"ApplyAllSettings");
        return false;
    }
    return g_omniStackPanel != nullptr;
}

static bool ApplyPendingSettings() {
    if (g_reapplyPending.exchange(false)) {
        CleanupAndResetCurrentElements();
        g_applied = false;
    }
    return ApplyAllSettings();
}

// ── IconView constructor hook ──────────────────────────────────────────────

using IconView_IconView_t = void*(WINAPI*)(void*);
IconView_IconView_t IconView_IconView_Original;

void* WINAPI IconView_IconView_Hook(void* pThis) {
    void* ret = IconView_IconView_Original(pThis);
    try {
        FrameworkElement iconView = nullptr;
        ((IUnknown**)pThis)[1]->QueryInterface(
            winrt::guid_of<FrameworkElement>(), winrt::put_abi(iconView));
        if (!iconView) return ret;
        g_autoRevokerList.emplace_back();
        auto it = std::prev(g_autoRevokerList.end());
        *it = iconView.Loaded(winrt::auto_revoke_t{},
            [it](IInspectable const&, RoutedEventArgs const&) {
                try {
                    g_autoRevokerList.erase(it);
                    if (!g_unloading && (!g_applied || g_reapplyPending))
                        g_applied = ApplyPendingSettings();
                } catch (...) {
                    LogCurrentUiException(L"IconView Loaded");
                }
            });
    } catch (...) {
        LogCurrentUiException(L"IconView constructor hook");
    }
    return ret;
}

// ── System tray module detection and hook setup ───────────────────────────

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;
static std::atomic<bool> g_systemTrayModuleHooked = false;

static VS_FIXEDFILEINFO* GetModuleVersionInfo(HMODULE hModule, UINT* puPtrLen) {
    void* pInfo = nullptr; UINT uLen = 0;
    HRSRC hRes = FindResource(hModule, MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION);
    if (hRes) {
        HGLOBAL hGlob = LoadResource(hModule, hRes);
        if (hGlob) {
            void* pData = LockResource(hGlob);
            if (pData && !VerQueryValue(pData, L"\\", &pInfo, &uLen)) { pInfo = nullptr; uLen = 0; }
        }
    }
    if (puPtrLen) *puPtrLen = uLen;
    return (VS_FIXEDFILEINFO*)pInfo;
}

static HMODULE GetSystemTrayModuleHandle() {
    HMODULE m = GetModuleHandleW(L"SystemTray.dll");
    if (!m) {
        m = GetModuleHandleW(L"Taskbar.View.dll");
        if (m) {
            auto* fi = GetModuleVersionInfo(m, nullptr);
            WORD major = fi ? HIWORD(fi->dwFileVersionMS) : 0;
            if (!major || major >= 2604) { Wh_Log(L"[Hooks] Skipping Taskbar.View.dll v%d", major); m = nullptr; }
        }
    }
    if (!m) m = GetModuleHandleW(L"ExplorerExtensions.dll");
    return m;
}

static bool HookSystemTraySymbols(HMODULE hModule) {
    WindhawkUtils::SYMBOL_HOOK systemTrayDllHooks[] = {{
        {LR"(public: __cdecl winrt::SystemTray::implementation::IconView::IconView(void))"},
        &IconView_IconView_Original, IconView_IconView_Hook,
    }};
    if (!WindhawkUtils::HookSymbols(hModule, systemTrayDllHooks,
                                    ARRAYSIZE(systemTrayDllHooks))) {
        Wh_Log(L"[Hooks] HookSymbols failed"); return false;
    }
    return true;
}

static void HandleLoadedModuleIfSystemTray(HMODULE hModule, LPCWSTR lpLibFileName) {
    if (!g_systemTrayModuleHooked && GetSystemTrayModuleHandle() == hModule) {
        bool expected = false;
        if (!g_systemTrayModuleHooked.compare_exchange_strong(expected, true))
            return;
        Wh_Log(L"[LoadLib] %s — hooking symbols", lpLibFileName);
        if (HookSystemTraySymbols(hModule)) {
            Wh_ApplyHookOperations();
        } else {
            g_systemTrayModuleHooked = false;
        }
    }
}

HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags) {
    HMODULE hModule = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    try {
        if (hModule && lpLibFileName)
            HandleLoadedModuleIfSystemTray(hModule, lpLibFileName);
    } catch (...) {
        LogCurrentUiException(L"LoadLibraryExW hook");
    }
    return hModule;
}

static void ApplyOnTaskbarWindowThread() {
    HWND window = g_taskbarWnd ? g_taskbarWnd
                               : FindCurrentProcessTaskbarWnd();
    if (!window) return;
    RunFromWindowThread(window, [](void*) {
        if (!g_unloading) g_applied = ApplyPendingSettings();
    }, nullptr);
}

static void StopRetryThread() {
    if (g_retryStopEvent) SetEvent(g_retryStopEvent);
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

static void StartRetryThread() {
    StopRetryThread();
    if (g_unloading) return;
    g_retryStopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_retryStopEvent) return;
    HANDLE stopEvent = g_retryStopEvent;
    g_retryThread = CreateThread(
        nullptr, 0,
        [](void* parameter) -> DWORD {
            HANDLE stopEvent = reinterpret_cast<HANDLE>(parameter);
            for (int attempt = 0; attempt < 5 && !g_unloading; ++attempt) {
                if (g_applied) break;
                if (attempt &&
                    WaitForSingleObject(stopEvent, 2000) != WAIT_TIMEOUT)
                    break;
                ApplyOnTaskbarWindowThread();
            }
            return 0;
        },
        stopEvent, 0, nullptr);
    if (!g_retryThread) {
        CloseHandle(g_retryStopEvent);
        g_retryStopEvent = nullptr;
    }
}

using TrayUI_StartTaskbar_t = void (WINAPI*)(void*);
static TrayUI_StartTaskbar_t TrayUI_StartTaskbar_Original;

static void WINAPI TrayUI_StartTaskbar_Hook(void* self) {
    TrayUI_StartTaskbar_Original(self);
    try {
        if (!g_unloading) {
            g_applied = false;
            g_reapplyPending = true;
            StartRetryThread();
        }
    } catch (...) {
        LogCurrentUiException(L"TrayUI::StartTaskbar hook");
    }
}

static bool HookTaskbarDllSymbols() {
    HMODULE hTaskbar = LoadLibraryExW(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!hTaskbar) { Wh_Log(L"[Hooks] Failed to load taskbar.dll"); return false; }
    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        { {LR"(const CTaskBand::`vftable'{for `ITaskListWndSite'})"}, &CTaskBand_ITaskListWndSite_vftable },
        { {LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const )"}, &CTaskBand_GetTaskbarHost_Original },
        { {LR"(public: int __cdecl TaskbarHost::FrameHeight(void)const )"}, &TaskbarHost_FrameHeight_Original },
        { {LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))"}, &std__Ref_count_base__Decref_Original },
        { {LR"(public: virtual void __cdecl TrayUI::StartTaskbar(void))"},
          &TrayUI_StartTaskbar_Original, TrayUI_StartTaskbar_Hook },
    };
    return WindhawkUtils::HookSymbols(hTaskbar, taskbarDllHooks,
                                      ARRAYSIZE(taskbarDllHooks));
}

// ── Windhawk lifecycle ─────────────────────────────────────────────────────

BOOL Wh_ModInit() {
    Wh_Log(L"[Init] OmniButton Customizer v1.0");
    LoadSettings();
    if (!HookTaskbarDllSymbols()) {
        Wh_Log(L"[Init] taskbar.dll symbol hooks failed");
        return FALSE;
    }
    if (HMODULE hSysTray = GetSystemTrayModuleHandle()) {
        if (HookSystemTraySymbols(hSysTray))
            g_systemTrayModuleHooked = true;
        else
            Wh_Log(L"[Init] system tray symbol hooks failed");
    } else {
        Wh_Log(L"[Init] System tray module not loaded yet");
        HMODULE kb = GetModuleHandleW(L"kernelbase.dll");
        auto pLoad = kb ? reinterpret_cast<LoadLibraryExW_t>(GetProcAddress(kb, "LoadLibraryExW")) : nullptr;
        if (pLoad) WindhawkUtils::SetFunctionHook(
            pLoad, LoadLibraryExW_Hook, &LoadLibraryExW_Original);
    }
    return TRUE;
}

void Wh_ModAfterInit() {
    if (!g_systemTrayModuleHooked) {
        if (HMODULE hSysTray = GetSystemTrayModuleHandle()) {
            if (!g_systemTrayModuleHooked.exchange(true)) {
                Wh_Log(L"[AfterInit] system tray module found — hooking");
                if (HookSystemTraySymbols(hSysTray)) {
                    Wh_ApplyHookOperations();
                } else {
                    g_systemTrayModuleHooked = false;
                }
            }
        }
    }
    StartRetryThread();
    Wh_Log(L"[AfterInit] systemTrayModuleHooked=%d", (int)g_systemTrayModuleHooked.load());
}

void Wh_ModUninit() {
    g_unloading = true;
    StopRetryThread();
    Wh_Log(L"[Uninit]");
    HWND hWnd = g_taskbarWnd ? g_taskbarWnd : FindCurrentProcessTaskbarWnd();
    if (hWnd) {
        if (!RunFromWindowThread(hWnd, [](void*) {
            g_autoRevokerList.clear();
            CleanupAndResetCurrentElements();
            g_applied = false;
        }, nullptr)) {
            Wh_Log(L"[Uninit] Taskbar dispatch failed; retaining XAML state");
        }
    } else {
        Wh_Log(L"[Uninit] No taskbar UI thread; retaining XAML state");
    }
}

void Wh_ModSettingsChanged() {
    StopRetryThread();
    LoadSettings();
    g_reapplyPending = true;
    g_applied = false;
    Wh_Log(L"[Settings] Updated");
    HWND hWnd = g_taskbarWnd ? g_taskbarWnd : FindCurrentProcessTaskbarWnd();
    if (!hWnd) {
        Wh_Log(L"[Settings] No taskbar window; scheduling retry");
        StartRetryThread();
        return;
    }
    if (!RunFromWindowThread(hWnd, [](void* parameter) {
        HWND window = static_cast<HWND>(parameter);
        if (!GetTaskbarXamlRoot(window)) return;
        g_applied = ApplyPendingSettings();
    }, hWnd))
        Wh_Log(L"[Settings] Taskbar dispatch failed; retaining XAML state");
    if (!g_applied && !g_unloading) StartRetryThread();
}
