// ==WindhawkMod==
// @id              taskbar-folder-menus
// @name            Taskbar Folder Menus
// @description     Adds compact Windows 11 taskbar buttons that open configured Shell targets as popup menus, similar to classic taskbar toolbars.
// @version         0.7
// @author          sb4ssman
// @github          https://github.com/sb4ssman
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lshell32 -luuid -lgdi32 -lcomctl32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Folder Menus

A Windows 11-only mod that adds compact taskbar buttons which open Shell targets
as native popup menus, recreating the most useful part of the classic taskbar
toolbar workflow. Windows 10 is not supported.

![Two folder buttons in the system tray](https://raw.githubusercontent.com/sb4ssman/Windhawk-Mod-Lab/main/taskbar-folder-menus/assets/desktop-controlpanel.png)
*A minimal two-button setup for Desktop and Control Panel.*

![Folder button destination tooltip](https://raw.githubusercontent.com/sb4ssman/Windhawk-Mod-Lab/main/taskbar-folder-menus/assets/tooltip-shows-destination.png)
*Hovering a compact button shows its configured Shell target.*

![Four folder buttons on a standard taskbar](https://raw.githubusercontent.com/sb4ssman/Windhawk-Mod-Lab/main/taskbar-folder-menus/assets/c-github-desktop-controlpanel.png)
*Drive, GitHub, Desktop, and Control Panel shortcuts arranged in a grid.*

![Four folder buttons on a taller taskbar](https://raw.githubusercontent.com/sb4ssman/Windhawk-Mod-Lab/main/taskbar-folder-menus/assets/c-github-desktop-controlpanel-v.png)
*The same four shortcuts arranged vertically by the grid layout on a taller taskbar.*

![Control Panel opened as a native Shell menu](https://raw.githubusercontent.com/sb4ssman/Windhawk-Mod-Lab/main/taskbar-folder-menus/assets/controlpanel-menu-open.png)
*The Control Panel namespace opens directly as a native Shell menu with full icons.*

![Whole drive opened from a taskbar folder button](https://raw.githubusercontent.com/sb4ssman/Windhawk-Mod-Lab/main/taskbar-folder-menus/assets/whole-drive-on-taskbar.png)
*A drive-root button opens the whole drive as a native cascading Shell menu.*

Click a small button to browse a folder, drive, Desktop, or Control Panel
directly from the taskbar — no minimizing required. Subfolders expand on
hover. Right-click any item for the full classic Windows Shell context menu.
Every folder popup includes an "Open in Explorer" shortcut at the top,
including the configured root folder.

## Example folder entries

```text
Label: 🖥    Target: shell:Desktop
Label: ⚙     Target: shell:ControlPanelFolder
Label: 📥    Target: %USERPROFILE%\Downloads
Label: C:    Target: C:\
```

Add one record per button in the Folders setting. Each record has a short button
label and a target. Targets can be normal paths or Shell namespace roots like
`shell:Desktop` and `shell:ControlPanelFolder`. Environment variables such as
`%USERPROFILE%` are expanded automatically. The full label and target appear in
the tooltip.

Emoji labels are a natural fit for narrow buttons. Label ideas: 📁 folder,
🖥 desktop, 💻 laptop, 🪟 windows, 📥 downloads, 🌐 network, 🗄 drive,
📄 documents, 🔧 tools, ⚙ settings, ⭐ favorites.

## Reordering folder buttons

For several existing records, open the mod's Settings page and switch to
**Textual mode**, then move each complete `label` + `target` record as a block.
The regular form currently provides add/remove controls but no direct
drag-to-reorder control.

## Settings

| Setting | Default | Description |
|---------|---------|-------------|
| Position | Before notification icons | Where to inject the button group in the tray |
| Folders | Desktop and Control Panel | Add/remove records in the form; reorder complete records in Textual mode |
| Grid mode | Smart automatic | Smart automatic, single row/column, fixed rows/columns, or fixed grid |
| Smart layout | Balanced | Balanced, pack vertical, or pack horizontal in smart mode |
| Grid columns | 0 (auto) | Exact columns in fixed-column modes; maximum columns in smart mode (0 = automatic) |
| Grid rows | 0 (auto) | Exact rows in fixed-row modes; maximum rows in smart mode (0 = tray height) |
| Fill order | Row-first | Row-first or column-first |
| Short row/column position | Last | Put an incomplete row or column first or last |
| Short row/column alignment | Start | Align an incomplete final row or column |
| Button width | 24 px | Width of each folder button |
| Button height | 22 px | Height of each folder button |
| Button spacing | 4 px | Gap between buttons |
| Default button text | 📁 | Fallback icon for entries with long labels |
| Text/icon size | 10 pt | Button label font size |
| Text color | *(system)* | Button label color; empty keeps the system default |
| Background color | *(system)* | Button background; empty keeps the system default |
| Hover background color | `accent` | Hover background; empty keeps the native hover color |
| Click background color | *(system)* | Pressed background; empty keeps the native pressed color |
| Border color | *(system)* | Button border; empty keeps the system default |
| Border thickness | -1 (system) | -1 = system default, 0 = no border |
| Corner rounding | -1 (system) | -1 = system default, 0 = square |
| Opacity | 100% | Button transparency |
| Shine effect | Off | Gradient highlight; activates when a custom background color is set |
| Group padding left | 0 px | Extra space to the left of the button group |
| Group padding right | 0 px | Extra space to the right of the button group |
| Group X offset | 0 px | Shift the entire group left or right |
| Group Y offset | 0 px | Shift the entire group up or down |
| Max menu items | 0 (unlimited) | Limit items shown per folder |
| Subfolder depth | 0 (unlimited) | How many subfolder levels to include |
| Show hidden/system items | Off | Include hidden and system Shell items |

All color settings accept `#RRGGBB` or `#AARRGGBB` hex (the alpha byte is
honored), the generics `accent`, `accentLight`, and `accentDark` for the
Windows accent shades, or `transparent` for a fully transparent surface —
nothing drawn, button still present and clickable, useful for borderless
background-free buttons. Leaving a color empty keeps the system default for
that state.

## Note on shell:Desktop

`shell:Desktop` shows the full Desktop Shell namespace — user shortcuts, public
shortcuts, and virtual items like Recycle Bin — not just the physical Desktop
folder. Duplicates from the user+public Desktop merge are suppressed automatically.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- position: beforeIcons
  $name: Position
  $description: Where to place the folder menu buttons in the tray.
  $options:
  - "beforeIcons": "Before notification icons"
  - "beforeOmni": "Before OmniButton (wifi/vol/bat)"
  - "beforeClock": "Before clock"
  - "afterClock": "After clock"
  - "afterShowDesktop": "After Show Desktop strip"

- folders:
  - - label: "🖥"
      $name: Button label
      $description: Short text or emoji shown on the taskbar button.
    - target: "shell:Desktop"
      $name: Folder or Shell target
      $description: A folder path, drive root, Shell namespace root, or path containing environment variables.
  - - label: "⚙"
    - target: "shell:ControlPanelFolder"
  $name: Folders
  $description: >-
    Folder buttons to show. Add one record per button. Targets can be normal
    paths or Shell namespace roots such as shell:Desktop and
    shell:ControlPanelFolder. Environment variables such as %USERPROFILE% are
    expanded. Emoji labels work well on narrow buttons.

- gridMode: autoSmart
  $name: Grid mode
  $description: How to arrange multiple folder buttons.
  $options:
  - "autoSmart": "Smart automatic"
  - "singleRow": "Single row"
  - "singleColumn": "Single column"
  - "fixedRows": "Fixed rows"
  - "fixedColumns": "Fixed columns"
  - "fixedGrid": "Fixed rows and columns"

- smartLayout: balanced
  $name: Smart layout
  $description: Used when Layout mode is Smart automatic.
  $options:
  - balanced: Balanced
  - packVertical: Pack vertical
  - packHorizontal: Pack horizontal

- gridColumns: 0
  $name: Grid columns (0 = auto)
  $description: >-
    Exact column count in Fixed columns and Fixed rows and columns modes. In
    Smart automatic mode, this is the maximum column count; 0 chooses automatically.

- gridRows: 0
  $name: Grid rows (0 = auto)
  $description: >-
    Exact row count in Fixed rows and Fixed rows and columns modes. In Smart
    automatic mode, this is the maximum row count; 0 uses the tray height.

- fillOrder: rowFirst
  $name: Fill order
  $description: Whether folder buttons fill across rows first or down columns first.
  $options:
  - rowFirst: Row first
  - columnFirst: Column first

- shortGroupPosition: last
  $name: Short row/column position
  $description: Whether an incomplete row or column appears first or last.
  $options:
  - first: First
  - last: Last

- shortGroupAlign: start
  $name: Short row/column alignment
  $description: How to align an incomplete final row or column.
  $options:
  - start: Start
  - center: Center
  - end: End

- buttonWidth: 24
  $name: Button width (px)

- buttonHeight: 22
  $name: Button height (px)

- buttonSpacing: 4
  $name: Button spacing (px)

- buttonText: "📁"
  $name: Default button text
  $description: >-
    Icon shown for folder entries with long labels. One- or two-character
    labels are shown directly. Supports Unicode and emoji.

- fontSize: 10
  $name: Text/icon size (pt)
  $description: Size of the button label, including emoji labels.

- textColor: ""
  $name: Text color
  $description: "Hex (#RRGGBB or #AARRGGBB), accent / accentLight / accentDark, transparent, or empty for the system default."

- backgroundColor: ""
  $name: Background color
  $description: "Hex (#RRGGBB or #AARRGGBB), accent / accentLight / accentDark, transparent, or empty for the system default."

- hoverBackgroundColor: "accent"
  $name: Hover background color
  $description: "Hex (#RRGGBB or #AARRGGBB), accent / accentLight / accentDark, transparent, or empty for the system default."

- pressedBackgroundColor: ""
  $name: Click background color
  $description: "Hex (#RRGGBB or #AARRGGBB), accent / accentLight / accentDark, transparent, or empty for the system default."

- borderColor: ""
  $name: Border color
  $description: "Hex (#RRGGBB or #AARRGGBB), accent / accentLight / accentDark, transparent, or empty for the system default."

- borderThickness: -1
  $name: Border thickness (px)
  $description: "-1 = system default. 0 = no border. Positive values set a custom border thickness."

- cornerRadius: -1
  $name: Corner rounding (px)
  $description: "-1 = system default. 0 = square corners. Positive values round the button corners."

- opacity: 100
  $name: Opacity (%)
  $description: "Button opacity. 100 = fully opaque, 0 = invisible."

- shineEffect: false
  $name: Shine effect
  $description: Adds a subtle gradient highlight when a custom background color is set.

- groupPaddingLeft: 0
  $name: Group padding left (px)
  $description: Extra space to the left of the button group.

- groupPaddingRight: 0
  $name: Group padding right (px)
  $description: Extra space to the right of the button group.

- groupOffsetX: 0
  $name: Group X offset (px)
  $description: Move the entire button group left (negative) or right (positive).

- groupOffsetY: 0
  $name: Group Y offset (px)
  $description: Move the entire button group up (negative) or down (positive).

- maxMenuItems: 0
  $name: Max menu items per folder
  $description: Limit menu size for very large folders. 0 = unlimited.

- maxDepth: 0
  $name: Subfolder depth
  $description: How many subfolder levels to include as nested menus. 0 = unlimited.

- showHidden: false
  $name: Show hidden/system items

*/
// ==/WindhawkModSettings==

#undef GetCurrentTime

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.h>
#include <winrt/Windows.UI.Text.h>
#include <winrt/Windows.UI.ViewManagement.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Media.h>

#include <algorithm>
#include <atomic>
#include <climits>
#include <cwctype>
#include <functional>
#include <optional>
#include <string>
#include <vector>

#include <commctrl.h>
#include <shellapi.h>
#include <shlobj.h>
#include <windhawk_utils.h>

using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Controls;
using namespace winrt::Windows::UI::Xaml::Input;
using namespace winrt::Windows::UI::Xaml::Media;

// Smart-grid template: _templates/smart-grid-layout.h v1.0 (verbatim copy;
// Windhawk mods are single-file, so keep this block in sync with the template).
// Copy-source template v1.0: pure layout math for repeated taskbar items.
// This file intentionally has no WinRT dependency.

#include <algorithm>
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

// ============================================================
// Settings
// ============================================================

struct FolderEntry {
    std::wstring label;
    std::wstring target;
};

struct ModSettings {
    std::wstring position = L"beforeIcons";
    grid::GridMode gridMode = grid::GridMode::AutoSmart;
    grid::SmartLayout smartLayout = grid::SmartLayout::Balanced;
    std::wstring buttonText = L"📁";
    std::vector<FolderEntry> folders;
    int gridColumns = 0;
    int gridRows = 0;
    grid::FillOrder fillOrder = grid::FillOrder::RowFirst;
    grid::ShortGroupPosition shortGroupPosition =
        grid::ShortGroupPosition::Last;
    grid::ShortGroupAlign shortGroupAlign = grid::ShortGroupAlign::Start;
    int buttonWidth = 24;
    int buttonHeight = 22;
    int buttonSpacing = 4;
    int maxMenuItems = 0;
    int maxDepth = 0;
    bool showHidden = false;
    int fontSize = 10;
    std::wstring textColor;
    std::wstring backgroundColor;
    std::wstring hoverBackgroundColor;
    std::wstring pressedBackgroundColor;
    std::wstring borderColor;
    int borderThickness = -1;
    int cornerRadius = -1;
    int opacityPct = 100;
    bool shineEffect = false;
    int groupPaddingLeft = 0;
    int groupPaddingRight = 0;
    int groupOffsetX = 0;
    int groupOffsetY = 0;
};
static ModSettings g_settings;  // exit-time-safe: heap-only

static std::wstring Trim(std::wstring s) {
    auto isWs = [](wchar_t c) { return std::iswspace(c) != 0; };
    while (!s.empty() && isWs(s.front())) s.erase(s.begin());
    while (!s.empty() && isWs(s.back())) s.pop_back();
    return s;
}

// Expand a KNOWNFOLDERID token (e.g. %DESKTOP%) in text.
static std::wstring ExpandKnownFolder(std::wstring text,
                                       const wchar_t* token,
                                       REFKNOWNFOLDERID id) {
    size_t pos = 0;
    while ((pos = text.find(token, pos)) != std::wstring::npos) {
        PWSTR path = nullptr;
        if (SUCCEEDED(SHGetKnownFolderPath(id, 0, nullptr, &path)) && path) {
            text.replace(pos, wcslen(token), path);
            CoTaskMemFree(path);
        } else {
            pos += wcslen(token);
        }
    }
    return text;
}

static std::wstring ExpandEnv(std::wstring const& s) {
    // Expand known-folder tokens before the standard ExpandEnvironmentStrings call.
    std::wstring result = ExpandKnownFolder(s, L"%DESKTOP%",   FOLDERID_Desktop);
    result = ExpandKnownFolder(result, L"%DOWNLOADS%", FOLDERID_Downloads);
    result = ExpandKnownFolder(result, L"%DOCUMENTS%", FOLDERID_Documents);

    DWORD needed = ExpandEnvironmentStringsW(result.c_str(), nullptr, 0);
    if (!needed) return result;
    std::wstring out(needed, L'\0');
    DWORD written = ExpandEnvironmentStringsW(result.c_str(), out.data(), needed);
    if (!written || written > needed) return result;
    if (!out.empty() && out.back() == L'\0') out.pop_back();
    return out;
}

static std::wstring FileNameFromPath(std::wstring path) {
    while (!path.empty() && (path.back() == L'\\' || path.back() == L'/'))
        path.pop_back();
    size_t pos = path.find_last_of(L"\\/");
    return pos == std::wstring::npos ? path : path.substr(pos + 1);
}

static std::vector<FolderEntry> LoadFolders() {
    std::vector<FolderEntry> folders;
    for (int i = 0;; i++) {
        PCWSTR rawTarget = Wh_GetStringSetting(L"folders[%d].target", i);
        std::wstring target = rawTarget ? rawTarget : L"";
        Wh_FreeStringSetting(rawTarget);
        target = ExpandEnv(Trim(std::move(target)));
        if (target.empty())
            break;

        PCWSTR rawLabel = Wh_GetStringSetting(L"folders[%d].label", i);
        std::wstring label = rawLabel ? rawLabel : L"";
        Wh_FreeStringSetting(rawLabel);
        label = Trim(std::move(label));
        if (label.empty())
            label = FileNameFromPath(target);

        folders.push_back({std::move(label), std::move(target)});
    }

    if (folders.empty()) {
        folders.push_back({ L"🖥", L"shell:Desktop" });
        folders.push_back({ L"⚙", L"shell:ControlPanelFolder" });
    }
    return folders;
}

static std::wstring GetStringSetting(PCWSTR name) {
    PCWSTR raw = Wh_GetStringSetting(name);
    std::wstring value = raw;
    Wh_FreeStringSetting(raw);
    return value;
}

static void LoadSettings() {
    auto clamp = [](int value, int lo, int hi) {
        return std::max(lo, std::min(hi, value));
    };

    g_settings.position = GetStringSetting(L"position");
    std::wstring gridMode = GetStringSetting(L"gridMode");
    if (gridMode == L"singleRow")
        g_settings.gridMode = grid::GridMode::SingleRow;
    else if (gridMode == L"singleColumn")
        g_settings.gridMode = grid::GridMode::SingleColumn;
    else if (gridMode == L"fixedRows")
        g_settings.gridMode = grid::GridMode::FixedRows;
    else if (gridMode == L"fixedColumns")
        g_settings.gridMode = grid::GridMode::FixedColumns;
    else if (gridMode == L"fixedGrid")
        g_settings.gridMode = grid::GridMode::FixedGrid;
    else
        g_settings.gridMode = grid::GridMode::AutoSmart;

    std::wstring smartLayout = GetStringSetting(L"smartLayout");
    if (smartLayout == L"packVertical")
        g_settings.smartLayout = grid::SmartLayout::PackVertical;
    else if (smartLayout == L"packHorizontal")
        g_settings.smartLayout = grid::SmartLayout::PackHorizontal;
    else
        g_settings.smartLayout = grid::SmartLayout::Balanced;
    g_settings.buttonText = GetStringSetting(L"buttonText");
    g_settings.folders = LoadFolders();
    g_settings.gridColumns = std::max(0, Wh_GetIntSetting(L"gridColumns"));
    g_settings.gridRows = std::max(0, Wh_GetIntSetting(L"gridRows"));
    g_settings.fillOrder = GetStringSetting(L"fillOrder") == L"columnFirst"
        ? grid::FillOrder::ColumnFirst : grid::FillOrder::RowFirst;
    g_settings.shortGroupPosition =
        GetStringSetting(L"shortGroupPosition") == L"first"
            ? grid::ShortGroupPosition::First
            : grid::ShortGroupPosition::Last;
    std::wstring shortGroupAlign = GetStringSetting(L"shortGroupAlign");
    if (shortGroupAlign == L"center")
        g_settings.shortGroupAlign = grid::ShortGroupAlign::Center;
    else if (shortGroupAlign == L"end")
        g_settings.shortGroupAlign = grid::ShortGroupAlign::End;
    else
        g_settings.shortGroupAlign = grid::ShortGroupAlign::Start;
    g_settings.buttonWidth = std::max(10, Wh_GetIntSetting(L"buttonWidth"));
    g_settings.buttonHeight = std::max(10, Wh_GetIntSetting(L"buttonHeight"));
    g_settings.buttonSpacing = std::max(0, Wh_GetIntSetting(L"buttonSpacing"));
    g_settings.maxMenuItems = std::max(0, Wh_GetIntSetting(L"maxMenuItems"));
    g_settings.maxDepth = std::max(0, Wh_GetIntSetting(L"maxDepth"));
    g_settings.showHidden = Wh_GetIntSetting(L"showHidden") != 0;
    g_settings.fontSize = std::max(1, Wh_GetIntSetting(L"fontSize"));
    g_settings.textColor = GetStringSetting(L"textColor");
    g_settings.backgroundColor = GetStringSetting(L"backgroundColor");
    g_settings.hoverBackgroundColor = GetStringSetting(L"hoverBackgroundColor");
    g_settings.pressedBackgroundColor = GetStringSetting(L"pressedBackgroundColor");
    g_settings.borderColor = GetStringSetting(L"borderColor");
    g_settings.borderThickness = std::max(-1, Wh_GetIntSetting(L"borderThickness"));
    g_settings.cornerRadius = std::max(-1, Wh_GetIntSetting(L"cornerRadius"));
    g_settings.opacityPct = std::clamp(Wh_GetIntSetting(L"opacity"), 0, 100);
    g_settings.shineEffect = Wh_GetIntSetting(L"shineEffect") != 0;
    g_settings.groupPaddingLeft = clamp(Wh_GetIntSetting(L"groupPaddingLeft"), -80, 80);
    g_settings.groupPaddingRight = clamp(Wh_GetIntSetting(L"groupPaddingRight"), -80, 80);
    g_settings.groupOffsetX = clamp(Wh_GetIntSetting(L"groupOffsetX"), -80, 80);
    g_settings.groupOffsetY = clamp(Wh_GetIntSetting(L"groupOffsetY"), -80, 80);
}

// ============================================================
// Globals
// ============================================================

static std::atomic<bool> g_unloading{false};
static HWND              g_taskbarWnd = nullptr;
[[clang::no_destroy]] static Grid g_buttonGrid = nullptr;
static int               g_injectedColumn = -1;
static std::atomic<bool>  g_injectionLive{false};

struct ButtonEventState {
    Button button{nullptr};
    winrt::event_token clickToken{};
};
[[clang::no_destroy]] static std::optional<std::vector<ButtonEventState>>
    g_buttonEventStates{std::in_place};

static HANDLE g_retryThread = nullptr;
static HANDLE g_retryStopEvent = nullptr;
static SRWLOCK g_retryLock = SRWLOCK_INIT;

// Lazy Shell menu loading state (per-ShowFolderMenu call, single-threaded UI).
static UINT g_menuNextId = 1000;
static std::vector<PIDLIST_ABSOLUTE> g_menuIdToPidl;  // exit-time-safe: heap-only
static std::vector<HBITMAP> g_menuBitmaps;  // exit-time-safe: heap-only

struct PendingSubmenu {
    HMENU hmenu;
    PIDLIST_ABSOLUTE pidl;
    int depth;
};
static std::vector<PendingSubmenu> g_pendingSubmenus;  // exit-time-safe: heap-only

// Non-owning pointers valid only while a nested Shell context menu is being
// tracked. MenuOwnerSubclassProc forwards owner-draw and submenu messages to
// the strongest interface exposed by the selected Shell item.
static IContextMenu2* g_activeContextMenu2 = nullptr;
static IContextMenu3* g_activeContextMenu3 = nullptr;
static HMENU g_activeFolderMenu = nullptr;
static HMENU g_suspendedFolderMenu = nullptr;
static HMENU g_activeShellContextMenu = nullptr;
static bool g_shellContextDismissedByOutsideClick = false;

struct PendingShellCommand {
    IContextMenu* contextMenu = nullptr;
    int commandOffset = -1;
    POINT invokePoint{};
};
static PendingShellCommand g_pendingShellCommand;

static UINT GetInvokeShellCommandMessage() {
    static const UINT message = RegisterWindowMessageW(
        L"Windhawk_InvokeFolderMenuShellCommand_" WH_MOD_ID);
    return message;
}

// Forward declarations
static void ApplyAllSettings();
static void ApplyAllSettingsOnWindowThread();
static void RemoveButtonGrid();
static void StartRetryThread();
static void StopRetryThread();

// ============================================================
// GetTaskbarXamlRoot
// ============================================================

using CTaskBand_GetTaskbarHost_t = void* (WINAPI*)(void* pThis, void* result);
CTaskBand_GetTaskbarHost_t CTaskBand_GetTaskbarHost_Original;

using TaskbarHost_FrameHeight_t = int (WINAPI*)(void* pThis);
TaskbarHost_FrameHeight_t TaskbarHost_FrameHeight_Original;

using std__Ref_count_base__Decref_t = void (WINAPI*)(void* pThis);
std__Ref_count_base__Decref_t std__Ref_count_base__Decref_Original;

static void* CTaskBand_ITaskListWndSite_vftable = nullptr;

static XamlRoot GetTaskbarXamlRoot(HWND hTaskbarWnd) {
    if (!CTaskBand_GetTaskbarHost_Original || !TaskbarHost_FrameHeight_Original ||
        !std__Ref_count_base__Decref_Original || !CTaskBand_ITaskListWndSite_vftable)
        return nullptr;

    HWND hTaskSwWnd = (HWND)GetProp(hTaskbarWnd, L"TaskbandHWND");
    if (!hTaskSwWnd) return nullptr;
    void* taskBand = (void*)GetWindowLongPtr(hTaskSwWnd, 0);
    if (!taskBand) return nullptr;
    void* taskBandForSite = taskBand;
    for (int i = 0; *(void**)taskBandForSite != CTaskBand_ITaskListWndSite_vftable; i++) {
        if (i == 20) return nullptr;
        taskBandForSite = (void**)taskBandForSite + 1;
    }
    void* taskbarHostSharedPtr[2]{};
    CTaskBand_GetTaskbarHost_Original(taskBandForSite, taskbarHostSharedPtr);
    if (!taskbarHostSharedPtr[0] || !taskbarHostSharedPtr[1]) {
        if (taskbarHostSharedPtr[1])
            std__Ref_count_base__Decref_Original(taskbarHostSharedPtr[1]);
        return nullptr;
    }
    size_t offset = 0x10;
#if defined(_M_X64)
    {
        // 48:83EC 28 | sub rsp,28
        // 48:83C1 48 | add rcx,48
        const BYTE* b = (const BYTE*)TaskbarHost_FrameHeight_Original;
        if (b[0] == 0x48 && b[1] == 0x83 && b[2] == 0xEC && b[4] == 0x48 &&
            b[5] == 0x83 && b[6] == 0xC1 && b[7] <= 0x7F)
            offset = b[7];
        else
            Wh_Log(L"Unsupported TaskbarHost::FrameHeight");
    }
#elif defined(_M_ARM64)
    {
        // 7f2303d5 pacibsp
        // fd7bbfa9 stp     fp, lr, [sp, #-0x10]!
        // fd030091 mov     fp, sp
        // 080c41f8 ldr     x8, [x0, #0x10]!
        const DWORD* p = (const DWORD*)TaskbarHost_FrameHeight_Original;
        if (p[0] == 0xD503237F && (p[1] & 0xFFC07FFF) == 0xA9807BFD &&
            p[2] == 0x910003FD && (p[3] & 0xFFF00FE0) == 0xF8400C00)
            offset = (p[3] >> 12) & 0xFF;
        else
            Wh_Log(L"Unsupported TaskbarHost::FrameHeight");
    }
#else
#error "Unsupported architecture"
#endif
    auto* iunk = *(IUnknown**)((BYTE*)taskbarHostSharedPtr[0] + offset);
    if (!iunk) {
        std__Ref_count_base__Decref_Original(taskbarHostSharedPtr[1]);
        return nullptr;
    }
    FrameworkElement taskbarElem = nullptr;
    iunk->QueryInterface(winrt::guid_of<FrameworkElement>(), winrt::put_abi(taskbarElem));
    auto result = taskbarElem ? taskbarElem.XamlRoot() : nullptr;
    std__Ref_count_base__Decref_Original(taskbarHostSharedPtr[1]);
    return result;
}

// ============================================================
// Window thread marshalling / taskbar discovery
// ============================================================

using RunFromWindowThreadProc_t = void (*)(void*);

static bool RunFromWindowThread(HWND hWnd, RunFromWindowThreadProc_t proc, void* procParam) {
    static const UINT kMsg = RegisterWindowMessage(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);
    struct Param { RunFromWindowThreadProc_t proc; void* procParam; };
    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (!dwThreadId) return false;
    if (dwThreadId == GetCurrentThreadId()) { proc(procParam); return true; }
    HHOOK hook = SetWindowsHookEx(WH_CALLWNDPROC, [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT {
        if (nCode == HC_ACTION) {
            const CWPSTRUCT* cwp = (const CWPSTRUCT*)lParam;
            if (cwp->message == RegisterWindowMessageW(L"Windhawk_RunFromWindowThread_" WH_MOD_ID)) {
                auto* p = (Param*)cwp->lParam;
                p->proc(p->procParam);
            }
        }
        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }, nullptr, dwThreadId);
    if (!hook) return false;
    Param param{ proc, procParam };
    SendMessage(hWnd, kMsg, 0, (LPARAM)&param);
    UnhookWindowsHookEx(hook);
    return true;
}

static HWND FindCurrentProcessTaskbarWnd() {
    HWND result = nullptr;
    EnumWindows([](HWND hWnd, LPARAM lParam) -> BOOL {
        DWORD pid = 0;
        WCHAR cls[32];
        if (GetWindowThreadProcessId(hWnd, &pid) && pid == GetCurrentProcessId() &&
            GetClassNameW(hWnd, cls, ARRAYSIZE(cls)) &&
            _wcsicmp(cls, L"Shell_TrayWnd") == 0) {
            *reinterpret_cast<HWND*>(lParam) = hWnd;
            return FALSE;
        }
        return TRUE;
    }, reinterpret_cast<LPARAM>(&result));
    return result;
}

// ============================================================
// XAML helpers
// ============================================================

static FrameworkElement FindChildRecursive(FrameworkElement const& element,
    std::function<bool(FrameworkElement)> const& cb, int maxDepth = 20) {
    int n = VisualTreeHelper::GetChildrenCount(element);
    for (int i = 0; i < n && maxDepth > 0; i++) {
        auto child = VisualTreeHelper::GetChild(element, i).try_as<FrameworkElement>();
        if (!child) continue;
        if (cb(child)) return child;
        auto found = FindChildRecursive(child, cb, maxDepth - 1);
        if (found) return found;
    }
    return nullptr;
}

static FrameworkElement FindLiveSystemTrayFrameGrid() {
    HWND hWnd = FindCurrentProcessTaskbarWnd();
    if (!hWnd) hWnd = g_taskbarWnd;
    if (!hWnd) return nullptr;
    auto xamlRoot = GetTaskbarXamlRoot(hWnd);
    if (!xamlRoot) return nullptr;
    auto root = xamlRoot.Content().try_as<FrameworkElement>();
    if (!root) return nullptr;
    return FindChildRecursive(root, [](FrameworkElement fe) {
        return fe.Name() == L"SystemTrayFrameGrid";
    });
}

// ============================================================
// Folder menu
// ============================================================

struct ShellMenuItem {
    std::wstring displayName;
    PIDLIST_ABSOLUTE pidl = nullptr;
    bool canExpand = false;
};

static void FreeMenuState() {
    for (auto& ps : g_pendingSubmenus)
        if (ps.pidl) CoTaskMemFree(ps.pidl);
    g_pendingSubmenus.clear();

    for (auto pidl : g_menuIdToPidl)
        if (pidl) CoTaskMemFree(pidl);
    g_menuIdToPidl.clear();

    for (auto bmp : g_menuBitmaps)
        if (bmp) DeleteObject(bmp);
    g_menuBitmaps.clear();
}

static bool IsTarget(std::wstring const& value, const wchar_t* token) {
    return _wcsicmp(value.c_str(), token) == 0;
}

static PIDLIST_ABSOLUTE ParseDisplayNamePidl(const wchar_t* name) {
    PIDLIST_ABSOLUTE pidl = nullptr;
    SFGAOF attrs = 0;
    if (SUCCEEDED(SHParseDisplayName(name, nullptr, &pidl, 0, &attrs)))
        return pidl;
    return nullptr;
}

static PIDLIST_ABSOLUTE ParseShellTarget(std::wstring const& target) {
    PIDLIST_ABSOLUTE pidl = nullptr;

    if (IsTarget(target, L"shell:Desktop") || IsTarget(target, L"desktop:")) {
        if (SUCCEEDED(SHGetSpecialFolderLocation(nullptr, CSIDL_DESKTOP, &pidl)))
            return pidl;
        return nullptr;
    }

    if (IsTarget(target, L"shell:ControlPanelFolder") ||
        IsTarget(target, L"control:") ||
        IsTarget(target, L"control")) {
        if ((pidl = ParseDisplayNamePidl(target.c_str())) != nullptr)
            return pidl;
        if ((pidl = ParseDisplayNamePidl(L"shell:::{26EE0668-A00A-44D7-9371-BEB064C98683}")) != nullptr)
            return pidl;
        if ((pidl = ParseDisplayNamePidl(L"shell:::{21EC2020-3AEA-1069-A2DD-08002B30309D}")) != nullptr)
            return pidl;
        return nullptr;
    }

    return ParseDisplayNamePidl(target.c_str());
}

static bool BindFolderFromPidl(PCIDLIST_ABSOLUTE pidl, IShellFolder** folder) {
    *folder = nullptr;
    if (!pidl)
        return false;

    if (ILIsEmpty(pidl))
        return SUCCEEDED(SHGetDesktopFolder(folder));

    IShellFolder* parent = nullptr;
    PCUITEMID_CHILD child = nullptr;
    HRESULT hr = SHBindToParent(pidl, IID_IShellFolder, (void**)&parent, &child);
    if (FAILED(hr) || !parent)
        return false;

    hr = parent->BindToObject(child, nullptr, IID_IShellFolder, (void**)folder);
    parent->Release();
    return SUCCEEDED(hr) && *folder;
}

static std::wstring StrRetToString(STRRET const& str, PCUITEMID_CHILD pidl) {
    if (str.uType == STRRET_WSTR) {
        std::wstring result = str.pOleStr ? str.pOleStr : L"";
        CoTaskMemFree(str.pOleStr);
        return result;
    }

    if (str.uType == STRRET_OFFSET)
        return (LPCWSTR)(((const BYTE*)pidl) + str.uOffset);

    if (str.uType == STRRET_CSTR) {
        int needed = MultiByteToWideChar(CP_ACP, 0, str.cStr, -1, nullptr, 0);
        if (needed > 0) {
            std::wstring result(needed, L'\0');
            MultiByteToWideChar(CP_ACP, 0, str.cStr, -1, result.data(), needed);
            if (!result.empty() && result.back() == L'\0')
                result.pop_back();
            return result;
        }
    }

    return L"";
}

static HBITMAP BitmapFromIcon(HICON icon) {
    if (!icon)
        return nullptr;

    int iconW = GetSystemMetrics(SM_CXSMICON);
    int iconH = GetSystemMetrics(SM_CYSMICON);
    int bmpW = std::max(16, iconW);
    int bmpH = std::max(16, iconH);

    BITMAPINFO bmi{};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = bmpW;
    bmi.bmiHeader.biHeight = -bmpH;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void* bits = nullptr;
    HDC screenDc = GetDC(nullptr);
    HBITMAP bmp = CreateDIBSection(screenDc, &bmi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!bmp) {
        if (screenDc)
            ReleaseDC(nullptr, screenDc);
        return nullptr;
    }
    HDC memDc = CreateCompatibleDC(screenDc);
    if (!memDc) {
        DeleteObject(bmp);
        if (screenDc)
            ReleaseDC(nullptr, screenDc);
        return nullptr;
    }
    HGDIOBJ oldBmp = SelectObject(memDc, bmp);
    if (bits)
        ZeroMemory(bits, bmpW * bmpH * 4);
    DrawIconEx(memDc, (bmpW - iconW) / 2, (bmpH - iconH) / 2,
               icon, iconW, iconH, 0, nullptr, DI_NORMAL);
    SelectObject(memDc, oldBmp);
    DeleteDC(memDc);
    if (screenDc)
        ReleaseDC(nullptr, screenDc);
    return bmp;
}

static HBITMAP CreateMenuBitmapForPidl(PCIDLIST_ABSOLUTE pidl) {
    SHFILEINFOW sfi{};
    if (!SHGetFileInfoW((LPCWSTR)pidl, 0, &sfi, sizeof(sfi),
                        SHGFI_PIDL | SHGFI_ICON | SHGFI_SMALLICON))
        return nullptr;

    HBITMAP bmp = BitmapFromIcon(sfi.hIcon);
    DestroyIcon(sfi.hIcon);
    if (bmp)
        g_menuBitmaps.push_back(bmp);
    return bmp;
}

static std::vector<ShellMenuItem> EnumerateShellFolder(PCIDLIST_ABSOLUTE folderPidl) {
    std::vector<ShellMenuItem> items;

    IShellFolder* folder = nullptr;
    if (!BindFolderFromPidl(folderPidl, &folder))
        return items;

    DWORD flags = SHCONTF_FOLDERS | SHCONTF_NONFOLDERS;
    if (g_settings.showHidden)
        flags |= SHCONTF_INCLUDEHIDDEN;

    IEnumIDList* enumList = nullptr;
    if (SUCCEEDED(folder->EnumObjects(g_taskbarWnd, flags, &enumList)) && enumList) {
        PITEMID_CHILD child = nullptr;
        ULONG fetched = 0;
        while (enumList->Next(1, &child, &fetched) == S_OK && child) {
            STRRET str{};
            std::wstring displayName;
            if (SUCCEEDED(folder->GetDisplayNameOf(child, SHGDN_NORMAL, &str)))
                displayName = StrRetToString(str, child);

            SFGAOF attrs = SFGAO_FOLDER | SFGAO_HASSUBFOLDER | SFGAO_FILESYSTEM;
            PCUITEMID_CHILD childConst = child;
            folder->GetAttributesOf(1, &childConst, &attrs);

            PIDLIST_ABSOLUTE abs = ILCombine(folderPidl, child);
            CoTaskMemFree(child);
            child = nullptr;

            if (!abs || displayName.empty()) {
                if (abs) CoTaskMemFree(abs);
                continue;
            }

            ShellMenuItem item;
            item.displayName = std::move(displayName);
            item.pidl = abs;
            item.canExpand = (attrs & SFGAO_FOLDER) && (attrs & SFGAO_FILESYSTEM);
            items.push_back(std::move(item));
        }
        enumList->Release();
    }

    folder->Release();

    std::sort(items.begin(), items.end(), [](auto const& a, auto const& b) {
        if (a.canExpand != b.canExpand) return a.canExpand > b.canExpand;
        return _wcsicmp(a.displayName.c_str(), b.displayName.c_str()) < 0;
    });

    // The Desktop namespace merges user+public Desktop folders and virtual items,
    // causing the same shortcut to appear multiple times. Remove consecutive
    // duplicates by display name after sorting.
    for (auto it = items.begin(); it != items.end(); ) {
        auto next = it + 1;
        if (next != items.end() &&
                _wcsicmp(it->displayName.c_str(), next->displayName.c_str()) == 0) {
            if (next->pidl) CoTaskMemFree(next->pidl);
            items.erase(next);
        } else {
            ++it;
        }
    }

    if (g_settings.maxMenuItems > 0 && (int)items.size() > g_settings.maxMenuItems) {
        for (size_t i = g_settings.maxMenuItems; i < items.size(); i++)
            if (items[i].pidl) CoTaskMemFree(items[i].pidl);
        items.resize(g_settings.maxMenuItems);
    }

    return items;
}

static void AddShellFolderItemsToMenu(HMENU menu, PCIDLIST_ABSOLUTE folderPidl, int depth, UINT startPosition = 0);

static void InsertShellMenuItem(HMENU menu, UINT position, ShellMenuItem& item, int depth) {
    bool canExpand = item.canExpand && (g_settings.maxDepth == 0 || depth < g_settings.maxDepth);

    MENUITEMINFOW mii{};
    mii.cbSize = sizeof(mii);
    mii.fMask = MIIM_STRING | MIIM_BITMAP;
    mii.dwTypeData = const_cast<LPWSTR>(item.displayName.c_str());
    mii.cch = (UINT)item.displayName.size();
    mii.hbmpItem = CreateMenuBitmapForPidl(item.pidl);

    if (canExpand) {
        HMENU sub = CreatePopupMenu();
        AppendMenuW(sub, MF_STRING | MF_GRAYED, 0, L"(Loading…)");
        UINT id = g_menuNextId++;
        g_menuIdToPidl.push_back(ILCloneFull(item.pidl)); // clone for right-click context menu
        g_pendingSubmenus.push_back({sub, item.pidl, depth + 1});
        item.pidl = nullptr;
        mii.fMask |= MIIM_SUBMENU | MIIM_ID;
        mii.hSubMenu = sub;
        mii.wID = id;
    } else {
        UINT id = g_menuNextId++;
        g_menuIdToPidl.push_back(item.pidl);
        item.pidl = nullptr;
        mii.fMask |= MIIM_ID;
        mii.wID = id;
    }

    InsertMenuItemW(menu, position, TRUE, &mii);
}

static void AddShellFolderItemsToMenu(HMENU menu, PCIDLIST_ABSOLUTE folderPidl, int depth, UINT startPosition) {
    auto items = EnumerateShellFolder(folderPidl);
    if (items.empty()) {
        AppendMenuW(menu, MF_STRING | MF_GRAYED, 0, L"(empty)");
        return;
    }

    UINT position = startPosition;
    for (auto& item : items)
        InsertShellMenuItem(menu, position++, item, depth);

    for (auto& item : items)
        if (item.pidl) CoTaskMemFree(item.pidl);
}

static void InsertOpenInExplorerHeader(HMENU menu,
                                       PCIDLIST_ABSOLUTE folderPidl,
                                       UINT position = 0) {
    UINT openId = g_menuNextId++;
    g_menuIdToPidl.push_back(ILCloneFull(folderPidl));

    MENUITEMINFOW openMii{};
    openMii.cbSize = sizeof(openMii);
    openMii.fMask = MIIM_STRING | MIIM_ID;
    openMii.wID = openId;
    wchar_t openLabel[] = L"Open in Explorer";
    openMii.dwTypeData = openLabel;
    openMii.cch = (UINT)wcslen(openLabel);
    InsertMenuItemW(menu, position, TRUE, &openMii);

    MENUITEMINFOW separator{};
    separator.cbSize = sizeof(separator);
    separator.fMask = MIIM_FTYPE;
    separator.fType = MFT_SEPARATOR;
    InsertMenuItemW(menu, position + 1, TRUE, &separator);
}

static void InvokePidl(HWND owner, PCIDLIST_ABSOLUTE pidl) {
    SHELLEXECUTEINFOW sei{};
    sei.cbSize = sizeof(sei);
    sei.fMask = SEE_MASK_IDLIST | SEE_MASK_INVOKEIDLIST | SEE_MASK_ASYNCOK;
    sei.hwnd = owner;
    sei.lpVerb = L"open";
    sei.lpIDList = (void*)pidl;
    sei.nShow = SW_SHOWNORMAL;
    ShellExecuteExW(&sei);
}

static void ClearPendingShellCommand() {
    if (g_pendingShellCommand.contextMenu)
        g_pendingShellCommand.contextMenu->Release();
    g_pendingShellCommand = {};
}

static void InvokePendingShellCommand(HWND owner) {
    IContextMenu* contextMenu = g_pendingShellCommand.contextMenu;
    int commandOffset = g_pendingShellCommand.commandOffset;
    POINT invokePoint = g_pendingShellCommand.invokePoint;
    g_pendingShellCommand = {};
    if (!contextMenu || commandOffset < 0) {
        if (contextMenu) contextMenu->Release();
        return;
    }

    CMINVOKECOMMANDINFOEX ici{};
    ici.cbSize = sizeof(ici);
    // Let the selected Shell handler choose its normal execution model.
    ici.fMask = CMIC_MASK_UNICODE | CMIC_MASK_PTINVOKE;
    ici.hwnd = owner;
    ici.lpVerb = MAKEINTRESOURCEA(commandOffset);
    ici.lpVerbW = MAKEINTRESOURCEW(commandOffset);
    ici.nShow = SW_SHOWNORMAL;
    ici.ptInvoke = invokePoint;

    Wh_Log(L"[ContextMenu] Invoking queued Shell command offset=%d",
           commandOffset);
    HRESULT hr = contextMenu->InvokeCommand(
        reinterpret_cast<LPCMINVOKECOMMANDINFO>(&ici));
    if (FAILED(hr))
        Wh_Log(L"[ContextMenu] InvokeCommand failed hr=0x%08X", hr);
    else
        Wh_Log(L"[ContextMenu] InvokeCommand returned hr=0x%08X", hr);
    contextMenu->Release();
}

static bool ShowShellContextMenu(HWND owner, PCIDLIST_ABSOLUTE pidl) {
    IShellFolder* parent = nullptr;
    PCUITEMID_CHILD child = nullptr;
    HRESULT hr = SHBindToParent(
        pidl, IID_IShellFolder, (void**)&parent, &child);
    if (FAILED(hr) || !parent) {
        Wh_Log(L"[ContextMenu] SHBindToParent failed hr=0x%08X", hr);
        return false;
    }

    IContextMenu* ctxMenu = nullptr;
    hr = parent->GetUIObjectOf(
        owner, 1, &child, IID_IContextMenu, nullptr, (void**)&ctxMenu);
    parent->Release();
    if (FAILED(hr) || !ctxMenu) {
        Wh_Log(L"[ContextMenu] GetUIObjectOf failed hr=0x%08X", hr);
        return false;
    }

    HMENU hPopup = CreatePopupMenu();
    if (!hPopup) {
        ctxMenu->Release();
        return false;
    }

    UINT queryFlags = CMF_NORMAL;
    if (GetKeyState(VK_SHIFT) < 0)
        queryFlags |= CMF_EXTENDEDVERBS;
    hr = ctxMenu->QueryContextMenu(hPopup, 0, 1, 0x7FFF, queryFlags);
    if (FAILED(hr)) {
        Wh_Log(L"[ContextMenu] QueryContextMenu failed hr=0x%08X", hr);
        DestroyMenu(hPopup);
        ctxMenu->Release();
        return false;
    }

    IContextMenu3* ctxMenu3 = nullptr;
    IContextMenu2* ctxMenu2 = nullptr;
    if (FAILED(ctxMenu->QueryInterface(IID_PPV_ARGS(&ctxMenu3))))
        ctxMenu->QueryInterface(IID_PPV_ARGS(&ctxMenu2));

    POINT pt;
    GetCursorPos(&pt);

    g_activeContextMenu3 = ctxMenu3;
    g_activeContextMenu2 = ctxMenu2;
    // Suspend hit-testing of the underlying folder menu while its nested Shell
    // context menu is active. Otherwise a right-click inside an overlapping
    // Shell submenu can be mistaken for another click on the original item.
    HMENU suspendedFolderMenu = g_activeFolderMenu;
    g_suspendedFolderMenu = suspendedFolderMenu;
    g_activeShellContextMenu = hPopup;
    g_shellContextDismissedByOutsideClick = false;
    g_activeFolderMenu = nullptr;
    int cmd = (int)TrackPopupMenuEx(
        hPopup,
        TPM_RETURNCMD | TPM_RIGHTBUTTON | TPM_RECURSE,
        pt.x, pt.y, owner, nullptr);
    PostMessageW(owner, WM_NULL, 0, 0);
    bool dismissedByOutsideClick =
        g_shellContextDismissedByOutsideClick;
    g_shellContextDismissedByOutsideClick = false;
    g_activeShellContextMenu = nullptr;
    g_suspendedFolderMenu = nullptr;
    g_activeFolderMenu = suspendedFolderMenu;
    g_activeContextMenu3 = nullptr;
    g_activeContextMenu2 = nullptr;

    if (ctxMenu3) ctxMenu3->Release();
    if (ctxMenu2) ctxMenu2->Release();
    DestroyMenu(hPopup);

    if (cmd <= 0) {
        if (dismissedByOutsideClick) {
            Wh_Log(L"[ContextMenu] Shell menu dismissed by outside click; closing folder popup");
            if (!EndMenu()) {
                Wh_Log(L"[ContextMenu] EndMenu failed error=%u; sending WM_CANCELMODE",
                       GetLastError());
                SendMessageW(owner, WM_CANCELMODE, 0, 0);
            }
        } else {
            Wh_Log(L"[ContextMenu] Shell menu dismissed; folder popup remains");
        }
        ctxMenu->Release();
        return true;
    }

    // A positive selection closes the original folder popup, but invocation is
    // deferred until that outer TrackPopupMenu call has completely unwound.
    ClearPendingShellCommand();
    g_pendingShellCommand.contextMenu = ctxMenu;
    g_pendingShellCommand.commandOffset = cmd - 1;
    g_pendingShellCommand.invokePoint = pt;
    Wh_Log(L"[ContextMenu] Queued Shell command offset=%d; closing folder popup",
           cmd - 1);
    if (!EndMenu()) {
        Wh_Log(L"[ContextMenu] EndMenu failed error=%u; sending WM_CANCELMODE",
               GetLastError());
        SendMessageW(owner, WM_CANCELMODE, 0, 0);
    }
    return true;
}

static bool PopulatePendingSubmenu(HMENU hMenu) {
    for (size_t i = 0; i < g_pendingSubmenus.size(); ++i) {
        if (g_pendingSubmenus[i].hmenu != hMenu)
            continue;

        int depth = g_pendingSubmenus[i].depth;
        PIDLIST_ABSOLUTE pidl = g_pendingSubmenus[i].pidl;
        g_pendingSubmenus[i].pidl = nullptr;
        g_pendingSubmenus.erase(g_pendingSubmenus.begin() + i);
        while (GetMenuItemCount(hMenu) > 0)
            RemoveMenu(hMenu, 0, MF_BYPOSITION);

        InsertOpenInExplorerHeader(hMenu, pidl);
        AddShellFolderItemsToMenu(hMenu, pidl, depth, 2);
        CoTaskMemFree(pidl);
        return true;
    }
    return false;
}

static bool FindMenuItemAtPoint(HMENU menu, POINT point,
                                HMENU* itemMenu, UINT* itemPosition) {
    if (!menu) return false;

    int position = MenuItemFromPoint(nullptr, menu, point);
    if (position >= 0) {
        if (itemMenu) *itemMenu = menu;
        if (itemPosition) *itemPosition = static_cast<UINT>(position);
        return true;
    }

    int count = GetMenuItemCount(menu);
    for (int i = 0; i < count; ++i) {
        HMENU submenu = GetSubMenu(menu, i);
        if (submenu && FindMenuItemAtPoint(
                submenu, point, itemMenu, itemPosition))
            return true;
    }
    return false;
}

static bool ShowMenuItemContextMenu(HWND owner, HMENU menu, UINT position) {
    MENUITEMINFOW mii{};
    mii.cbSize = sizeof(mii);
    mii.fMask = MIIM_ID | MIIM_FTYPE | MIIM_STATE;
    if (!GetMenuItemInfoW(menu, position, TRUE, &mii) ||
        (mii.fType & MFT_SEPARATOR) ||
        (mii.fState & (MFS_DISABLED | MFS_GRAYED)) ||
        mii.wID < 1000) {
        return false;
    }

    size_t index = mii.wID - 1000;
    if (index >= g_menuIdToPidl.size() || !g_menuIdToPidl[index]) {
        Wh_Log(L"[ContextMenu] No PIDL for menu id=%u", mii.wID);
        return false;
    }

    Wh_Log(L"[ContextMenu] Opening for menu id=%u", mii.wID);
    return ShowShellContextMenu(owner, g_menuIdToPidl[index]);
}

static LRESULT CALLBACK FolderMenuMsgFilterProc(
    int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == MSGF_MENU && lParam) {
        auto message = reinterpret_cast<MSG const*>(lParam);
        if (g_activeShellContextMenu &&
            (message->message == WM_LBUTTONDOWN ||
             message->message == WM_RBUTTONDOWN ||
             message->message == WM_MBUTTONDOWN ||
             message->message == WM_NCLBUTTONDOWN ||
             message->message == WM_NCRBUTTONDOWN ||
             message->message == WM_NCMBUTTONDOWN)) {
            HMENU hitMenu = nullptr;
            UINT hitPosition = 0;
            bool inShellMenu = FindMenuItemAtPoint(
                g_activeShellContextMenu, message->pt,
                &hitMenu, &hitPosition);
            bool inFolderMenu = g_suspendedFolderMenu &&
                FindMenuItemAtPoint(
                    g_suspendedFolderMenu, message->pt,
                    &hitMenu, &hitPosition);
            if (!inShellMenu && !inFolderMenu) {
                g_shellContextDismissedByOutsideClick = true;
                Wh_Log(L"[ContextMenu] Outside click will dismiss both menus");
            }
        }
        if (message->message == WM_INITMENUPOPUP) {
            PopulatePendingSubmenu(reinterpret_cast<HMENU>(message->wParam));
        } else if (message->message == WM_RBUTTONUP && g_activeFolderMenu) {
            Wh_Log(L"[ContextMenu] Right-button release in folder menu loop");
            POINT point{};
            GetCursorPos(&point);
            HMENU itemMenu = nullptr;
            UINT itemPosition = 0;
            if (FindMenuItemAtPoint(g_activeFolderMenu, point,
                                    &itemMenu, &itemPosition)) {
                // Don't enter another TrackPopupMenuEx while still inside this
                // raw menu-input filter callback. Defer to the documented
                // WM_MENURBUTTONUP owner-message path; TPM_RECURSE can then
                // suspend and resume the outer popup correctly.
                if (PostMessageW(g_taskbarWnd, WM_MENURBUTTONUP,
                                 itemPosition,
                                 reinterpret_cast<LPARAM>(itemMenu))) {
                    Wh_Log(L"[ContextMenu] Deferred menu id position=%u to owner",
                           itemPosition);
                    return 1;
                }
                Wh_Log(L"[ContextMenu] Failed to defer right-click error=%u",
                       GetLastError());
            }
            Wh_Log(L"[ContextMenu] Menu item hit test produced no actionable item");
        }
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

static bool ForwardActiveContextMenuMessage(UINT msg, WPARAM wParam,
                                            LPARAM lParam, LRESULT* result) {
    if (msg != WM_INITMENUPOPUP && msg != WM_DRAWITEM &&
        msg != WM_MEASUREITEM && msg != WM_MENUCHAR)
        return false;

    if (g_activeContextMenu3) {
        LRESULT menuResult = 0;
        if (SUCCEEDED(g_activeContextMenu3->HandleMenuMsg2(
                msg, wParam, lParam, &menuResult))) {
            if (result) *result = menuResult;
            return true;
        }
    } else if (g_activeContextMenu2 &&
               SUCCEEDED(g_activeContextMenu2->HandleMenuMsg(
                   msg, wParam, lParam))) {
        if (result) *result = 0;
        return true;
    }

    return false;
}

static LRESULT CALLBACK MenuOwnerSubclassProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam,
    UINT_PTR /*subclassId*/, DWORD_PTR /*data*/) {
    if (msg == GetInvokeShellCommandMessage()) {
        // The posted message runs only after the XAML Button.Click callback
        // has returned, allowing the pressed visual to clear before a modal
        // Shell verb such as Properties blocks this UI thread.
        RemoveWindowSubclass(hwnd, MenuOwnerSubclassProc, 1);
        if (g_unloading)
            ClearPendingShellCommand();
        else
            InvokePendingShellCommand(hwnd);
        return 0;
    }

    if (msg == WM_INITMENUPOPUP) {
        PopulatePendingSubmenu(reinterpret_cast<HMENU>(wParam));
    }

    if (msg == WM_MENURBUTTONUP) {
        Wh_Log(L"[ContextMenu] Owner received WM_MENURBUTTONUP position=%u",
               (UINT)wParam);
        if (!ShowMenuItemContextMenu(
                hwnd, reinterpret_cast<HMENU>(lParam), (UINT)wParam)) {
            Wh_Log(L"[ContextMenu] Owner message had no actionable item");
        }
        return 0;
    }

    LRESULT contextMenuResult = 0;
    if (ForwardActiveContextMenuMessage(
            msg, wParam, lParam, &contextMenuResult))
        return contextMenuResult;

    return DefSubclassProc(hwnd, msg, wParam, lParam);
}

static void ShowFolderMenu(FolderEntry folder) {
    HWND owner = g_taskbarWnd ? g_taskbarWnd : FindCurrentProcessTaskbarWnd();
    if (!owner || GetWindowThreadProcessId(owner, nullptr) != GetCurrentThreadId()) {
        Wh_Log(L"[Menu] Taskbar owner is unavailable on the current thread");
        return;
    }

    g_menuNextId = 1000;
    FreeMenuState();
    ClearPendingShellCommand();

    HMENU menu = CreatePopupMenu();
    PIDLIST_ABSOLUTE rootPidl = ParseShellTarget(folder.target);
    if (!rootPidl) {
        AppendMenuW(menu, MF_STRING | MF_GRAYED, 0, L"(target not found)");
        Wh_Log(L"[Menu] Failed to parse target: %s", folder.target.c_str());
    } else {
        InsertOpenInExplorerHeader(menu, rootPidl);
        AddShellFolderItemsToMenu(menu, rootPidl, 0, 2);
    }

    POINT pt;
    GetCursorPos(&pt);
    SetForegroundWindow(owner);

    // Align the menu so it opens away from the taskbar edge.
    UINT tpmAlign = TPM_RIGHTBUTTON | TPM_RETURNCMD | TPM_LEFTALIGN | TPM_BOTTOMALIGN;
    if (owner) {
        APPBARDATA abd{};
        abd.cbSize = sizeof(abd);
        abd.hWnd = owner;
        SHAppBarMessage(ABM_GETTASKBARPOS, &abd);
        switch (abd.uEdge) {
            case ABE_TOP:   tpmAlign = TPM_RIGHTBUTTON | TPM_RETURNCMD | TPM_LEFTALIGN | TPM_TOPALIGN;   break;
            case ABE_LEFT:  tpmAlign = TPM_RIGHTBUTTON | TPM_RETURNCMD | TPM_LEFTALIGN | TPM_TOPALIGN;   break;
            case ABE_RIGHT: tpmAlign = TPM_RIGHTBUTTON | TPM_RETURNCMD | TPM_RIGHTALIGN | TPM_TOPALIGN;  break;
            default:        break; // ABE_BOTTOM: default flags are correct
        }
    }

    if (!SetWindowSubclass(owner, MenuOwnerSubclassProc, 1, 0)) {
        Wh_Log(L"[Menu] Failed to subclass the taskbar menu owner");
        DestroyMenu(menu);
        if (rootPidl)
            CoTaskMemFree(rootPidl);
        FreeMenuState();
        return;
    }
    g_activeFolderMenu = menu;
    HHOOK menuHook = SetWindowsHookExW(
        WH_MSGFILTER, FolderMenuMsgFilterProc,
        nullptr, GetCurrentThreadId());
    if (!menuHook) {
        Wh_Log(L"[Menu] WH_MSGFILTER hook failed error=%u", GetLastError());
    }
    UINT cmd = TrackPopupMenu(menu, tpmAlign, pt.x, pt.y, 0, owner, nullptr);
    PostMessageW(owner, WM_NULL, 0, 0);
    if (menuHook)
        UnhookWindowsHookEx(menuHook);
    g_activeFolderMenu = nullptr;

    bool hasPendingShellCommand =
        g_pendingShellCommand.contextMenu &&
        g_pendingShellCommand.commandOffset >= 0;

    if (!hasPendingShellCommand)
        RemoveWindowSubclass(owner, MenuOwnerSubclassProc, 1);

    if (!hasPendingShellCommand && cmd >= 1000) {
        size_t idx = cmd - 1000;
        if (idx < g_menuIdToPidl.size() && g_menuIdToPidl[idx])
            InvokePidl(owner, g_menuIdToPidl[idx]);
    }

    DestroyMenu(menu);
    if (rootPidl)
        CoTaskMemFree(rootPidl);
    FreeMenuState();

    if (hasPendingShellCommand) {
        Wh_Log(L"[ContextMenu] Folder popup closed; posting queued Shell command");
        if (!PostMessageW(owner, GetInvokeShellCommandMessage(), 0, 0)) {
            Wh_Log(L"[ContextMenu] Failed to post Shell command error=%u; invoking inline",
                   GetLastError());
            RemoveWindowSubclass(owner, MenuOwnerSubclassProc, 1);
            InvokePendingShellCommand(owner);
        }
    } else {
        ClearPendingShellCommand();
    }

}

// ============================================================
// Button grid
// ============================================================

static Brush ParseColorBrush(std::wstring const& hex) {
    using winrt::Windows::UI::ViewManagement::UIColorType;

    if (_wcsicmp(hex.c_str(), L"transparent") == 0) {
        SolidColorBrush brush;
        brush.Color(winrt::Windows::UI::Color{0, 0, 0, 0});
        return brush;
    }

    // Numbered Windows shades are accepted silently and stay undocumented.
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
        if (_wcsicmp(hex.c_str(), entry.token) == 0) {
            try {
                winrt::Windows::UI::ViewManagement::UISettings settings;
                auto color = settings.GetColorValue(entry.type);
                SolidColorBrush brush;
                brush.Color(color);
                return brush;
            } catch (...) {
                Wh_Log(L"[Color] Failed to read the Windows accent color");
                return nullptr;
            }
        }
    }

    if (hex.empty() || hex[0] != L'#')
        return nullptr;

    std::wstring h = hex.substr(1);
    if (h.size() == 6)
        h = L"FF" + h;
    if (h.size() != 8)
        return nullptr;

    UINT32 val = 0;
    for (wchar_t c : h) {
        val <<= 4;
        if (c >= L'0' && c <= L'9')
            val |= (UINT32)(c - L'0');
        else if (c >= L'A' && c <= L'F')
            val |= (UINT32)(10 + c - L'A');
        else if (c >= L'a' && c <= L'f')
            val |= (UINT32)(10 + c - L'a');
        else
            return nullptr;
    }

    winrt::Windows::UI::Color color;
    color.A = (BYTE)(val >> 24);
    color.R = (BYTE)(val >> 16);
    color.G = (BYTE)(val >> 8);
    color.B = (BYTE)val;
    SolidColorBrush brush;
    brush.Color(color);
    return brush;
}

static Brush MakeShineBrush(Brush const& base) {
    if (!g_settings.shineEffect)
        return base;

    auto solid = base ? base.try_as<SolidColorBrush>() : nullptr;
    if (!solid)
        return base;

    auto c = solid.Color();
    auto adjust = [](BYTE value, int delta) {
        return (BYTE)std::clamp((int)value + delta, 0, 255);
    };

    LinearGradientBrush brush;
    brush.StartPoint({0.0, 0.0});
    brush.EndPoint({0.0, 1.0});

    GradientStop shine;
    winrt::Windows::UI::Color shineColor{180, 255, 255, 255};
    shine.Color(shineColor);
    shine.Offset(0.0);
    brush.GradientStops().Append(shine);

    GradientStop light;
    winrt::Windows::UI::Color lightColor{
        c.A, adjust(c.R, 34), adjust(c.G, 34), adjust(c.B, 34)};
    light.Color(lightColor);
    light.Offset(0.42);
    brush.GradientStops().Append(light);

    GradientStop baseStop;
    baseStop.Color(c);
    baseStop.Offset(0.52);
    brush.GradientStops().Append(baseStop);

    GradientStop dark;
    winrt::Windows::UI::Color darkColor{
        c.A, adjust(c.R, -28), adjust(c.G, -28), adjust(c.B, -28)};
    dark.Color(darkColor);
    dark.Offset(1.0);
    brush.GradientStops().Append(dark);

    return brush;
}

static void SetButtonBrushResource(Button const& button,
                                   wchar_t const* key,
                                   Brush const& brush) {
    auto resources = button.Resources();
    auto boxedKey = winrt::box_value(key);
    if (brush)
        resources.Insert(boxedKey, brush);
    else
        resources.Remove(boxedKey);
}

static void ApplyButtonStyle(Button btn,
                             Brush const& textBrush,
                             Brush const& bgBrush,
                             Brush const& hoverBgBrush,
                             Brush const& pressedBgBrush,
                             Brush const& borderBrush) {
    if (textBrush) {
        btn.Foreground(textBrush);
    } else {
        btn.ClearValue(Control::ForegroundProperty());
    }
    SetButtonBrushResource(btn, L"ButtonForeground", textBrush);
    SetButtonBrushResource(btn, L"ButtonForegroundPointerOver", textBrush);
    SetButtonBrushResource(btn, L"ButtonForegroundPressed", textBrush);

    auto normalBrush = MakeShineBrush(bgBrush);
    auto hoverBrush = MakeShineBrush(hoverBgBrush);
    auto pressBrush = MakeShineBrush(pressedBgBrush);
    if (normalBrush)
        btn.Background(normalBrush);
    else
        btn.ClearValue(Control::BackgroundProperty());
    SetButtonBrushResource(btn, L"ButtonBackground", normalBrush);
    SetButtonBrushResource(btn, L"ButtonBackgroundPointerOver", hoverBrush);
    SetButtonBrushResource(btn, L"ButtonBackgroundPressed", pressBrush);

    if (borderBrush) {
        btn.BorderBrush(borderBrush);
    } else {
        btn.ClearValue(Control::BorderBrushProperty());
    }
    SetButtonBrushResource(btn, L"ButtonBorderBrush", borderBrush);
    SetButtonBrushResource(btn, L"ButtonBorderBrushPointerOver", borderBrush);
    SetButtonBrushResource(btn, L"ButtonBorderBrushPressed", borderBrush);

    if (g_settings.borderThickness >= 0) {
        double t = (double)g_settings.borderThickness;
        btn.BorderThickness({ t, t, t, t });
    }

    if (g_settings.cornerRadius >= 0) {
        double r = (double)g_settings.cornerRadius;
        btn.CornerRadius({ r, r, r, r });
    }
}

static grid::Config MakeFolderGridConfig(double trayHeight) {
    grid::Config config;
    config.mode = g_settings.gridMode;
    config.smartLayout = g_settings.smartLayout;
    config.fillOrder = g_settings.fillOrder;
    config.shortGroupPosition = g_settings.shortGroupPosition;
    config.shortGroupAlign = g_settings.shortGroupAlign;
    config.rows = g_settings.gridRows;
    config.columns = g_settings.gridColumns;

    int spacing = std::max(0, g_settings.buttonSpacing);
    int pitch = std::max(1, g_settings.buttonHeight + spacing);
    config.availableRows = std::max(
        1, static_cast<int>((trayHeight + spacing) / pitch));
    return config;
}

static Grid BuildFolderButtonGrid(double trayHeight) {
    int count = (int)g_settings.folders.size();
    auto config = MakeFolderGridConfig(trayHeight);
    auto layout = grid::ComputeLayout(count, config);
    Wh_Log(L"[Layout] trayHeight=%.1f availableRows=%d result=%dx%d",
           trayHeight, config.availableRows, layout.rows, layout.columns);

    Grid grid;
    grid.Name(L"TaskbarFolderMenuBar");
    grid.VerticalAlignment(VerticalAlignment::Center);
    grid.Margin({(double)g_settings.groupPaddingLeft, 0.0,
                 (double)g_settings.groupPaddingRight, 0.0});
    if (g_settings.groupOffsetX || g_settings.groupOffsetY) {
        TranslateTransform transform;
        transform.X((double)g_settings.groupOffsetX);
        transform.Y((double)g_settings.groupOffsetY);
        grid.RenderTransform(transform);
    }
    if (g_settings.buttonSpacing > 0) {
        grid.ColumnSpacing((double)g_settings.buttonSpacing);
        grid.RowSpacing((double)g_settings.buttonSpacing);
    }

    for (int r = 0; r < layout.rows; r++) {
        RowDefinition rd;
        rd.Height({ (double)g_settings.buttonHeight, GridUnitType::Pixel });
        grid.RowDefinitions().Append(rd);
    }
    for (int c = 0; c < layout.columns; c++) {
        ColumnDefinition cd;
        cd.Width({ (double)g_settings.buttonWidth, GridUnitType::Pixel });
        grid.ColumnDefinitions().Append(cd);
    }

    auto textBrush = ParseColorBrush(g_settings.textColor);
    auto bgBrush = ParseColorBrush(g_settings.backgroundColor);
    auto hoverBgBrush = ParseColorBrush(g_settings.hoverBackgroundColor);
    auto pressedBgBrush = ParseColorBrush(g_settings.pressedBackgroundColor);
    auto borderBrush = ParseColorBrush(g_settings.borderColor);

    for (int i = 0; i < count; i++) {
        auto entry = g_settings.folders[i];
        std::wstring caption = entry.label;
        if (caption.empty())
            caption = g_settings.buttonText.empty() ? L"📁" : g_settings.buttonText;

        Button btn;
        btn.Name(L"FolderMenuButton_" + std::to_wstring(i));
        btn.Content(winrt::box_value(winrt::hstring(caption)));
        btn.Width((double)g_settings.buttonWidth);
        btn.Height((double)g_settings.buttonHeight);
        btn.Padding({ 0.0, 0.0, 0.0, 1.0 });
        btn.FontSize((double)g_settings.fontSize);
        btn.FontWeight(winrt::Windows::UI::Text::FontWeights::SemiBold());
        btn.HorizontalAlignment(HorizontalAlignment::Stretch);
        btn.VerticalAlignment(VerticalAlignment::Stretch);
        ApplyButtonStyle(btn, textBrush, bgBrush, hoverBgBrush, pressedBgBrush, borderBrush);
        if (g_settings.opacityPct != 100)
            btn.Opacity((double)g_settings.opacityPct / 100.0);
        ToolTipService::SetToolTip(btn,
            winrt::box_value(winrt::hstring(entry.label + L"\n" + entry.target)));

        auto weakButton = winrt::make_weak(btn);
        auto clickToken = btn.Click([entry, weakButton](auto const&, auto const&) {
            if (!g_unloading)
                ShowFolderMenu(entry);

            // TrackPopupMenu runs a native modal input loop, so XAML can miss
            // the pointer-exit transition when the user dismisses the menu by
            // clicking elsewhere. Merely selecting the Normal visual state is
            // temporary: Button can immediately restore PointerOver from its
            // stale internal input state. Briefly remove the Button from hit
            // testing and layout to invalidate that state, then restore it.
            if (auto button = weakButton.get()) {
                button.ReleasePointerCaptures();
                button.IsHitTestVisible(false);
                button.Visibility(Visibility::Collapsed);
                button.UpdateLayout();
                button.Visibility(Visibility::Visible);
                button.UpdateLayout();
                button.IsHitTestVisible(true);
                bool reset = VisualStateManager::GoToState(
                    button, L"Normal", false);
                Wh_Log(L"[Button] Reset input/visual state after menu, success=%d",
                       reset ? 1 : 0);
            }
        });
        g_buttonEventStates->push_back({btn, clickToken});

        auto cell = grid::GetCell(i, count, layout, config);
        Grid::SetRow(btn, cell.row);
        Grid::SetColumn(btn, cell.column);
        if (cell.rowSpan > 1) {
            Grid::SetRowSpan(btn, cell.rowSpan);
            btn.VerticalAlignment(VerticalAlignment::Top);
        }
        if (cell.columnSpan > 1) {
            Grid::SetColumnSpan(btn, cell.columnSpan);
            btn.HorizontalAlignment(HorizontalAlignment::Left);
        }
        if (cell.topOffsetUnits || cell.leftOffsetUnits) {
            double pitchX = g_settings.buttonWidth + g_settings.buttonSpacing;
            double pitchY = g_settings.buttonHeight + g_settings.buttonSpacing;
            btn.Margin({cell.leftOffsetUnits * pitchX,
                        cell.topOffsetUnits * pitchY, 0.0, 0.0});
        }
        grid.Children().Append(btn);
    }

    return grid;
}

// ============================================================
// Injection / cleanup
// ============================================================

static bool RemoveButtonGridFrom(Grid gridParent, int col) {
    if (!gridParent) return false;

    bool removed = false;
    int liveCol = col;
    for (uint32_t i = 0; i < gridParent.Children().Size(); i++) {
        auto fe = gridParent.Children().GetAt(i).try_as<FrameworkElement>();
        if (fe && fe.Name() == L"TaskbarFolderMenuBar") {
            liveCol = Grid::GetColumn(fe);
            gridParent.Children().RemoveAt(i);
            removed = true;
            break;
        }
    }

    if (removed && liveCol >= 0) {
        uint32_t colU = (uint32_t)liveCol;
        if (colU < gridParent.ColumnDefinitions().Size())
            gridParent.ColumnDefinitions().RemoveAt(colU);
        for (auto child : gridParent.Children()) {
            auto fe = child.try_as<FrameworkElement>();
            if (!fe) continue;
            int c = Grid::GetColumn(fe);
            int span = Grid::GetColumnSpan(fe);
            if (c > liveCol)
                Grid::SetColumn(fe, c - 1);
            else if (c < liveCol && c + span > liveCol)
                Grid::SetColumnSpan(fe, span - 1);
        }
    }

    return removed;
}

static Grid FindButtonGridInParent(Grid gridParent) {
    if (!gridParent) return nullptr;
    for (auto child : gridParent.Children()) {
        auto fe = child.try_as<FrameworkElement>();
        if (fe && fe.Name() == L"TaskbarFolderMenuBar")
            return fe.try_as<Grid>();
    }
    return nullptr;
}

static bool HasButtonGridColumnCollision(Grid gridParent, Grid buttonGrid) {
    if (!gridParent || !buttonGrid) return false;
    int buttonCol = Grid::GetColumn(buttonGrid);
    for (auto child : gridParent.Children()) {
        auto fe = child.try_as<FrameworkElement>();
        if (fe && fe != buttonGrid && Grid::GetColumn(fe) == buttonCol)
            return true;
    }
    return false;
}

static int RepairButtonGridColumnCollision(Grid gridParent, Grid buttonGrid) {
    if (!gridParent || !buttonGrid) return 0;
    int buttonCol = Grid::GetColumn(buttonGrid);
    int moved = 0;
    for (auto child : gridParent.Children()) {
        auto fe = child.try_as<FrameworkElement>();
        if (!fe || fe == buttonGrid)
            continue;

        // A native tray child recreated after our initial injection gets its
        // original XAML Grid.Column again. Move only those children back across
        // the still-reserved synthetic column; already-shifted siblings stay put.
        if (Grid::GetColumn(fe) == buttonCol) {
            Grid::SetColumn(fe, buttonCol + 1);
            moved++;
        }
    }
    return moved;
}

// Routed event delegates, tooltips, and boxed Content point into this DLL, so
// release them while the DLL is still loaded. XAML tears down removed subtrees
// on a later UI tick, which can land after the mod has been unloaded.
static void ClearButtonEventState() {
    for (auto& state : *g_buttonEventStates) {
        if (!state.button) continue;
        try { state.button.Click(state.clickToken); } catch (...) {}
        try {
            ToolTipService::SetToolTip(
                state.button, winrt::Windows::Foundation::IInspectable{nullptr});
        } catch (...) {}
        try { state.button.Content(nullptr); } catch (...) {}
    }
    g_buttonEventStates->clear();
}

static void RemoveButtonGrid() {
    g_injectionLive.store(false);
    ClearButtonEventState();
    auto gridParent = FindLiveSystemTrayFrameGrid().try_as<Grid>();
    if (!RemoveButtonGridFrom(gridParent, g_injectedColumn))
        Wh_Log(L"[Remove] TaskbarFolderMenuBar not found");

    g_buttonGrid = nullptr;
    g_injectedColumn = -1;
}

static bool InjectButtonGrid(FrameworkElement root) {
    auto parent = FindChildRecursive(root, [](FrameworkElement fe) {
        return fe.Name() == L"SystemTrayFrameGrid";
    });
    if (!parent) {
        Wh_Log(L"[Inject] SystemTrayFrameGrid not found");
        return false;
    }

    auto gridParent = parent.try_as<Grid>();
    if (!gridParent) {
        Wh_Log(L"[Inject] SystemTrayFrameGrid is not a Grid");
        return false;
    }

    double trayHeight = gridParent.ActualHeight();
    if (trayHeight <= 0.0) {
        Wh_Log(L"[Inject] SystemTrayFrameGrid layout is not ready");
        return false;
    }

    for (auto child : gridParent.Children()) {
        if (auto fe = child.try_as<FrameworkElement>();
            fe && fe.Name() == L"TaskbarFolderMenuBar") {
            g_buttonGrid = fe.try_as<Grid>();
            g_injectedColumn = Grid::GetColumn(fe);
            return true;
        }
    }

    auto findNamedDirect = [&](PCWSTR name) -> FrameworkElement {
        for (auto child : gridParent.Children()) {
            if (auto fe = child.try_as<FrameworkElement>(); fe && fe.Name() == name)
                return fe;
        }
        return nullptr;
    };

    const auto& pos = g_settings.position;
    FrameworkElement refElem = nullptr;
    bool insertAfterRef = false;

    if (pos == L"beforeIcons") {
        // No named reference is needed: the notification icon stack starts at
        // the first native column.
    } else if (pos == L"beforeOmni")
        refElem = findNamedDirect(L"ControlCenterButton");
    else if (pos == L"beforeClock")
        refElem = findNamedDirect(L"NotificationCenterButton");
    else if (pos == L"afterClock")
        refElem = findNamedDirect(L"ShowDesktopStack");
    else if (pos == L"afterShowDesktop") {
        refElem = findNamedDirect(L"ShowDesktopStack");
        insertAfterRef = true;
    } else {
        Wh_Log(L"[Inject] Unknown position: %s", pos.c_str());
        return false;
    }

    if (pos != L"beforeIcons" && !refElem) {
        Wh_Log(L"[Inject] Anchor for position '%s' is not ready", pos.c_str());
        return false;
    }

    int insertCol = 0;
    if (insertAfterRef)
        insertCol = Grid::GetColumn(refElem) + 1;
    else if (refElem)
        insertCol = Grid::GetColumn(refElem);

    ColumnDefinition cd;
    cd.Width({ 1.0, GridUnitType::Auto });
    if ((uint32_t)insertCol < gridParent.ColumnDefinitions().Size())
        gridParent.ColumnDefinitions().InsertAt((uint32_t)insertCol, cd);
    else
        gridParent.ColumnDefinitions().Append(cd);

    for (auto child : gridParent.Children()) {
        auto fe = child.try_as<FrameworkElement>();
        if (!fe) continue;
        int col = Grid::GetColumn(fe);
        int span = Grid::GetColumnSpan(fe);
        if (col >= insertCol)
            Grid::SetColumn(fe, col + 1);
        else if (col + span > insertCol)
            Grid::SetColumnSpan(fe, span + 1);
    }

    auto grid = BuildFolderButtonGrid(trayHeight);
    Grid::SetColumn(grid, insertCol);
    gridParent.Children().Append(grid);

    g_buttonGrid = grid;
    g_injectedColumn = insertCol;

    Wh_Log(L"[Inject] TaskbarFolderMenuBar at column=%d, folders=%d",
           insertCol, (int)g_settings.folders.size());
    return true;
}

static void ApplyAllSettings() {
    g_injectionLive.store(false);
    HWND hWnd = FindCurrentProcessTaskbarWnd();
    if (!hWnd) {
        Wh_Log(L"[Apply] No taskbar window");
        return;
    }
    g_taskbarWnd = hWnd;

    try {
        auto xamlRoot = GetTaskbarXamlRoot(hWnd);
        if (!xamlRoot) {
            Wh_Log(L"[Apply] GetTaskbarXamlRoot failed");
            return;
        }
        auto root = xamlRoot.Content().try_as<FrameworkElement>();
        if (!root) {
            Wh_Log(L"[Apply] No XAML root content");
            return;
        }

        auto gridParent = FindChildRecursive(root, [](FrameworkElement fe) {
            return fe.Name() == L"SystemTrayFrameGrid";
        }).try_as<Grid>();
        if (!gridParent) {
            Wh_Log(L"[Apply] SystemTrayFrameGrid unavailable");
            return;
        }

        auto liveButtonGrid = FindButtonGridInParent(gridParent);
        bool rebuild = false;
        if (liveButtonGrid) {
            if (!g_buttonGrid || g_buttonGrid != liveButtonGrid) {
                Wh_Log(L"[Apply] Rebuilding orphaned folder grid");
                rebuild = true;
            } else if (HasButtonGridColumnCollision(gridParent, liveButtonGrid)) {
                int moved = RepairButtonGridColumnCollision(
                    gridParent, liveButtonGrid);
                Wh_Log(L"[Apply] Repaired %d tray child column collision(s)", moved);
                rebuild = HasButtonGridColumnCollision(gridParent, liveButtonGrid);
                if (rebuild)
                    Wh_Log(L"[Apply] Targeted tray column repair failed; rebuilding");
            }
        } else if (g_buttonGrid) {
            Wh_Log(L"[Apply] Releasing stale folder grid after tray recreation");
            ClearButtonEventState();
            g_buttonGrid = nullptr;
            g_injectedColumn = -1;
        }

        if (rebuild) {
            int liveColumn = Grid::GetColumn(liveButtonGrid);
            ClearButtonEventState();
            RemoveButtonGridFrom(gridParent, liveColumn);
            g_buttonGrid = nullptr;
            g_injectedColumn = -1;
        }

        if (!InjectButtonGrid(root)) {
            Wh_Log(L"[Apply] Injection failed");
            return;
        }
        g_injectionLive.store(true);
    } catch (...) {
        Wh_Log(L"[Apply] XAML tree is not ready");
    }
}

static void ApplyAllSettingsOnWindowThread() {
    HWND hWnd = FindCurrentProcessTaskbarWnd();
    if (!hWnd) return;
    g_taskbarWnd = hWnd;
    RunFromWindowThread(hWnd, [](void*) { ApplyAllSettings(); }, nullptr);
}

// ============================================================
// Hooks
// ============================================================

using TrayUI_StartTaskbar_t = void (WINAPI*)(void* pThis);
TrayUI_StartTaskbar_t TrayUI_StartTaskbar_Original;

void WINAPI TrayUI_StartTaskbar_Hook(void* pThis) {
    TrayUI_StartTaskbar_Original(pThis);
    if (!g_unloading)
        StartRetryThread();
}

static bool HookTaskbarDllSymbols() {
    HMODULE h = LoadLibraryExW(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!h) return false;
    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        { {LR"(const CTaskBand::`vftable'{for `ITaskListWndSite'})"},
          &CTaskBand_ITaskListWndSite_vftable },
        { {LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const )"},
          &CTaskBand_GetTaskbarHost_Original },
        { {LR"(public: int __cdecl TaskbarHost::FrameHeight(void)const )"},
          &TaskbarHost_FrameHeight_Original },
        { {LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))"},
          &std__Ref_count_base__Decref_Original },
        { {LR"(public: virtual void __cdecl TrayUI::StartTaskbar(void))"},
          &TrayUI_StartTaskbar_Original, TrayUI_StartTaskbar_Hook },
    };
    return WindhawkUtils::HookSymbols(h, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks));
}

static void StopRetryThread() {
    AcquireSRWLockExclusive(&g_retryLock);
    HANDLE retryThread = g_retryThread;
    HANDLE retryStopEvent = g_retryStopEvent;
    g_retryThread = nullptr;
    g_retryStopEvent = nullptr;
    if (retryStopEvent)
        SetEvent(retryStopEvent);
    ReleaseSRWLockExclusive(&g_retryLock);

    if (retryThread) {
        DWORD result;
        do {
            result = MsgWaitForMultipleObjects(
                1, &retryThread, FALSE, INFINITE, QS_SENDMESSAGE);
            if (result == WAIT_OBJECT_0 + 1) {
                MSG msg;
                PeekMessageW(&msg, nullptr, 0, 0, PM_NOREMOVE);
            }
        } while (result == WAIT_OBJECT_0 + 1);

        CloseHandle(retryThread);
    }

    if (retryStopEvent) {
        CloseHandle(retryStopEvent);
    }
}

static void StartRetryThread() {
    StopRetryThread();
    AcquireSRWLockExclusive(&g_retryLock);
    if (g_unloading || g_retryThread || g_retryStopEvent) {
        ReleaseSRWLockExclusive(&g_retryLock);
        return;
    }

    // A non-null cached Grid isn't proof that it still belongs to the current
    // tray. StartTaskbar can recreate/reindex the XAML tree after resume.
    // Sign-in can also take much longer than the old eight-second retry window.
    g_injectionLive.store(false);

    g_retryStopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_retryStopEvent) {
        ReleaseSRWLockExclusive(&g_retryLock);
        return;
    }

    HANDLE stopEvent = g_retryStopEvent;
    g_retryThread = CreateThread(nullptr, 0, [](void* param) -> DWORD {
        HANDLE stopEvent = static_cast<HANDLE>(param);
        static constexpr DWORD kRetryDelaysMs[] = {
            0, 500, 1000, 2000, 4000, 8000, 15000, 30000,
        };
        for (int i = 0;
             i < static_cast<int>(ARRAYSIZE(kRetryDelaysMs)) && !g_unloading;
             i++) {
            if (kRetryDelaysMs[i] &&
                WaitForSingleObject(stopEvent, kRetryDelaysMs[i]) != WAIT_TIMEOUT)
                break;
            Wh_Log(L"[Inject] Reconcile attempt %d", i + 1);
            ApplyAllSettingsOnWindowThread();
            if (g_injectionLive.load() || g_unloading)
                break;
        }
        return 0;
    }, stopEvent, 0, nullptr);

    if (!g_retryThread) {
        CloseHandle(g_retryStopEvent);
        g_retryStopEvent = nullptr;
    }
    ReleaseSRWLockExclusive(&g_retryLock);
}

// ============================================================
// Windhawk lifecycle
// ============================================================

BOOL Wh_ModInit() {
    Wh_Log(L"[Init] Taskbar Folder Menus v0.7");
    LoadSettings();

    if (!HookTaskbarDllSymbols()) {
        Wh_Log(L"[Init] taskbar.dll hooks failed - XamlRoot unavailable");
        return FALSE;
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    // One attempt covers loading the mod into an already-running taskbar.
    // Taskbar startup and rebuilds use the bounded StartTaskbar retry path.
    ApplyAllSettingsOnWindowThread();
}

void Wh_ModUninit() {
    g_unloading = true;
    Wh_Log(L"[Uninit]");

    StopRetryThread();

    HWND hWnd = FindCurrentProcessTaskbarWnd();
    if (hWnd) {
        RunFromWindowThread(hWnd, [](void*) {
            RemoveButtonGrid();
            g_buttonEventStates.reset();
        }, nullptr);
    } else {
        // No known taskbar UI thread: retain no_destroy XAML state rather than
        // releasing it from Windhawk's callback thread after framework teardown.
        Wh_Log(L"[Uninit] No taskbar UI thread; retaining XAML state");
    }
}

void Wh_ModSettingsChanged() {
    StopRetryThread();
    LoadSettings();
    Wh_Log(L"[Settings] Changed");

    HWND hWnd = FindCurrentProcessTaskbarWnd();
    if (!hWnd) {
        return;
    }
    g_taskbarWnd = hWnd;

    RunFromWindowThread(hWnd, [](void*) {
        RemoveButtonGrid();
        ApplyAllSettings();
    }, nullptr);
}
