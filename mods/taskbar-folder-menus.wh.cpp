// ==WindhawkMod==
// @id              taskbar-folder-menus
// @name            Taskbar Folder Menus
// @description     Adds compact Windows 11 taskbar buttons that open configured Shell targets as popup menus, similar to classic taskbar toolbars.
// @version         2.0
// @author          sb4ssman
// @github          https://github.com/sb4ssman
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lshell32 -lshlwapi -luuid -lgdi32 -lcomctl32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Folder Menus

A Windows 11-only mod that adds compact taskbar buttons which open Shell targets
as native popup menus, recreating the most useful part of the classic taskbar
toolbar workflow. Windows 10 is not supported.

Folder buttons form one grouped toolbar at a shared taskbar position.
Independent per-folder placement around the taskbar is outside this mod's scope.

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
**Textual mode**, then move each complete `Label` + `Target` + `UseDefaultIcon` record as a block.
The regular form currently provides add/remove controls but no direct
drag-to-reorder control.

## Native folder icons

Enable **Content → Folders → Use native Shell icon** for any entry to show
its actual Shell icon. Labels remain the default and provide the fallback if
the icon cannot be obtained. Icons are cached as pixels at the requested
display size; changing settings refreshes the requested DPI size. Text color
and font size affect labels; button dimensions determine native icon size.

## Placement after app icons

**Placement → Position → After pinned/running app icons** places the whole
toolbar after the rendered app buttons and follows them as apps open and close.
It reserves space and stops at the tray's left edge. If the taskbar cannot fit
the toolbar, it hides until enough room is available. Other positions place the
group before notification icons, before the OmniButton, before/after the clock,
or after Show Desktop. This mod targets the primary horizontal Windows 11 taskbar.

## Layout

**Layout → Arrangement** defaults to `auto`, which fits the available taskbar
height and logs its equivalent expression. Use folder numbers from the list:

| Expression | Result |
|---|---|
| `1 \| 2 \| 3` | One row |
| `1, 2, 3` | One column |
| `1, 2 \| 3, 4` | Two columns of two |
| `(1 \| 2), 3` | Two buttons above a third |
| `1 \| pad \| 2` | An empty button-sized space between entries |
| `1[2,-1] \| 2` | Nudge the first button right 2 DIP and up 1 DIP |

`folder1` is an alias for `1`. Unknown or unavailable items occupy no space;
duplicate folder tokens show one button. Invalid syntax logs its position and
falls back to `auto`. Unlisted folders are appended automatically unless
**Layout → Unlisted folders** is set to Hide.

## Settings

Dimensions use device-independent pixels (DIP) and scale with Windows display scaling.

| Group / setting | Default | Description |
|---|---|---|
| Placement.Position | `beforeIcons` | Position of the single toolbar |
| Content.Folders | Desktop, Control Panel | Each entry has Label, Target and UseDefaultIcon (off) |
| Content.DefaultLabel | 📁 | Used for empty labels |
| Layout.Arrangement | `auto` | Expression described above |
| Layout.FillOrder | `rows` | Automatic rows-first or columns-first filling |
| Layout.Justify | `center` | Align items along the cross axis: start, center, end |
| Layout.NewItems | `append` | Append unlisted entries or hide them |
| Size.ItemWidth / ItemHeight | 24 / 22 | Button dimensions, 10–256 DIP |
| Size.ItemSpacing | 4 | Gap, 0–80 DIP |
| Adjust.PadX / PadY | 0 / 0 | Symmetric reserved padding, 0–80 DIP |
| Adjust.OffsetX / OffsetY | 0 / 0 | Cosmetic group translation, −80–80 DIP |
| Surface.FontSize | 10 | Label size |
| Surface.TextColor / BackgroundColor | empty | System foreground/background |
| Surface.HoverBackgroundColor | `accent` | Hover background |
| Surface.PressedBackgroundColor / BorderColor | empty | System pressed background/border |
| Surface.BorderThickness / CornerRadius | −1 / −1 | System default; 0 removes border or makes corners square |
| Surface.Opacity | 100 | Button opacity, 0–100% |
| Surface.ShineEffect | off | Gradient highlight on custom background colors |
| Behavior.MaxMenuItems / MaxDepth | 150 / 0 | Limits each menu to 150 items by default; 0 remains unlimited. Submenu depth is unlimited by default. |
| Behavior.ShowHidden | off | Include hidden items; protected operating system files follow Explorer's own setting |

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

## Upgrading from 0.7

2.0 reorganised every setting into the groups above, and Windhawk cannot
carry a value across a renamed setting. **After updating, your buttons return
to the defaults (Desktop and Control Panel) until you re-apply your settings
once.** Before updating, copy your settings from the mod's Settings page in
**Textual mode**; afterwards, re-enter your folders under Content → Folders,
preserving their order and targets, and copy appearance and menu preferences
into their corresponding groups. Defaults remain 24×22 buttons, 4 DIP spacing
and 10 DIP labels; menus are now limited to 150 items per folder by default
(Behavior → Max menu items, 0 for unlimited).

Replace single-row/single-column settings with `1 | 2` / `1, 2` (extend for your
folder count), or use `auto`. The former separate left/right padding controls
become symmetric PadX; OffsetX provides a cosmetic horizontal adjustment.

Native-icon and after-app-icon feature ideas were contributed by
[diegoalejo15](https://github.com/diegoalejo15). This implementation adds icon
caching, a tray-edge limit, and shared layout, settings, surface and tray-column components.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- Placement:
  - Position: beforeIcons
    $name: Position
    $description: Where to place the folder menu buttons in the tray.
    $options:
    - "beforeIcons": "Before notification icons"
    - "beforeOmni": "Before OmniButton (wifi/vol/bat)"
    - "beforeClock": "Before clock"
    - "afterClock": "After clock"
    - "afterShowDesktop": "After Show Desktop strip"
    - "afterTaskbarIcons": "After pinned/running app icons"
  $name: Placement

- Content:
  - Folders:
    - - Label: "🖥"
        $name: Button label
        $description: Short text or emoji shown on the taskbar button.
      - Target: "shell:Desktop"
        $name: Folder or Shell target
        $description: A folder path, drive root, Shell namespace root, or path containing environment variables.
      - UseDefaultIcon: false
        $name: Use native Shell icon
        $description: Show the target icon instead of the label; fall back to the label if unavailable.
    - - Label: "⚙"
      - Target: "shell:ControlPanelFolder"
      - UseDefaultIcon: false
    $name: Folders
    $description: >-
      Folder buttons to show. Add one record per button. Targets can be normal
      paths or Shell namespace roots such as shell:Desktop and
      shell:ControlPanelFolder. Environment variables such as %USERPROFILE% are
      expanded. Emoji labels work well on narrow buttons.
  - DefaultLabel: "📁"
    $name: Default button label
    $description: Used when a folder label is empty.
  $name: Content

- Layout:
  - Arrangement: auto
    $name: Arrangement
    $description: "auto fits the taskbar height. Use 1 | 2 for a row, 1, 2 for a column; parentheses nest groups. Numbers follow the Folders list."
  - FillOrder: rows
    $name: Fill order
    $options:
    - rows: Rows first
    - columns: Columns first
  - Justify: center
    $name: Cross-axis alignment
    $options:
    - start: Start
    - center: Center
    - end: End
  - NewItems: append
    $name: Unlisted folders
    $options:
    - append: Append automatically
    - hide: Hide
  $name: Layout

- Size:
  - ItemWidth: 24
    $name: Button width (DIP)
  - ItemHeight: 22
    $name: Button height (DIP)
  - ItemSpacing: 4
    $name: Button spacing (DIP)
  $name: Size

- Adjust:
  - PadX: 0
    $name: Horizontal padding (DIP)
  - PadY: 0
    $name: Vertical padding (DIP)
  - OffsetX: 0
    $name: Group X offset (DIP)
    $description: Move the entire button group left (negative) or right (positive).
  - OffsetY: 0
    $name: Group Y offset (DIP)
    $description: Move the entire button group up (negative) or down (positive).
  $name: Adjust

- Surface:
  - FontSize: 10
    $name: Text/icon size (DIP)
    $description: Size of the button label, including emoji labels.
  - TextColor: ""
    $name: Text color
    $description: "Hex (#RRGGBB or #AARRGGBB), accent / accentLight / accentDark, transparent, or empty for the system default."
  - BackgroundColor: ""
    $name: Background color
    $description: "Hex (#RRGGBB or #AARRGGBB), accent / accentLight / accentDark, transparent, or empty for the system default."
  - HoverBackgroundColor: "accent"
    $name: Hover background color
    $description: "Hex (#RRGGBB or #AARRGGBB), accent / accentLight / accentDark, transparent, or empty for the system default."
  - PressedBackgroundColor: ""
    $name: Click background color
    $description: "Hex (#RRGGBB or #AARRGGBB), accent / accentLight / accentDark, transparent, or empty for the system default."
  - BorderColor: ""
    $name: Border color
    $description: "Hex (#RRGGBB or #AARRGGBB), accent / accentLight / accentDark, transparent, or empty for the system default."
  - BorderThickness: -1
    $name: Border thickness (DIP)
    $description: "-1 = system default. 0 = no border. Positive values set a custom border thickness."
  - CornerRadius: -1
    $name: Corner rounding (DIP)
    $description: "-1 = system default. 0 = square corners. Positive values round the button corners."
  - Opacity: 100
    $name: Opacity (%)
    $description: "Button opacity. 100 = fully opaque, 0 = invisible."
  - ShineEffect: false
    $name: Shine effect
    $description: Adds a subtle gradient highlight when a custom background color is set.
  $name: Surface

- Behavior:
  - MaxMenuItems: 150
    $name: Max menu items per folder
    $description: >-
      Limit menu size for very large folders. 0 = unlimited; 150 is the safe
      default. Values above 2000 are treated as 2000.
  - MaxDepth: 0
    $name: Subfolder depth
    $description: How many subfolder levels to include as nested menus. 0 = unlimited.
  - ShowHidden: false
    $name: Show hidden items
    $description: >-
      Include items Windows marks hidden. Protected operating system files
      stay hidden either way - those follow Explorer's own separate "Hide
      protected operating system files" setting.
  $name: Behavior

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
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Media.Imaging.h>
#include <winrt/Windows.Storage.Streams.h>
#include <robuffer.h>

#include <algorithm>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <cwctype>
#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include <commctrl.h>
#include <shellapi.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <windhawk_utils.h>

using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Controls;
using namespace winrt::Windows::UI::Xaml::Input;
using namespace winrt::Windows::UI::Xaml::Media;

// ==ModComponents==
// Self-contained building blocks this mod is built from. Each
// section below is one contract in its own namespace; the mod's own
// code begins after them.

// -- Settings values --------------------------------------------------------
// Clamped int/bool setting reads, fixed-buffer string reads, and the
// $options choice table - so a renamed option fails loudly instead of
// silently falling back.
namespace folder_menus_settings {

inline int Clamp(int value, int low, int high) {
    return std::max(low, std::min(high, value));
}

inline int LoadInt(PCWSTR key, int low, int high) {
    return Clamp(Wh_GetIntSetting(key), low, high);
}

inline bool LoadBool(PCWSTR key) {
    return Wh_GetIntSetting(key) != 0;
}

// A $options choice, matched case-insensitively against a table of tokens.
// Returns the matching entry's value, or `fallback` when nothing matches —
// which also covers the unset case, since an unset string is empty.
//
// Use a table rather than a chain of comparisons, so the accepted literals and
// their enum mapping stay adjacent when this mod's settings evolve.
template <typename T>
struct Choice {
    wchar_t const* token;
    T value;
};

template <typename T, size_t N>
inline T LoadChoice(PCWSTR key, Choice<T> const (&choices)[N], T fallback) {
    auto setting = WindhawkUtils::StringSetting::make(key);
    PCWSTR value = setting.get() ? setting.get() : L"";
    if (!*value) return fallback;
    for (auto const& choice : choices) {
        if (_wcsicmp(value, choice.token) == 0) return choice.value;
    }
    return fallback;
}

// Copy a string setting into a fixed buffer, always NUL-terminated, using
// `fallback` when the setting is empty. Fixed buffers rather than std::wstring
// because a namespace-scope settings struct must not own heap - see the
// exit-time destructor audit.
//
// Reading goes through WindhawkUtils::StringSetting rather than a local RAII
// wrapper: it is the same contract, it already ships with Windhawk, and a
// second copy of it is one more thing for a reader to check.
template <size_t N>
inline void LoadString(PCWSTR key, wchar_t (&buffer)[N],
                       PCWSTR fallback = nullptr) {
    auto setting = WindhawkUtils::StringSetting::make(key);
    PCWSTR value = setting.get() ? setting.get() : L"";
    if (!*value && fallback) value = fallback;
    wcsncpy_s(buffer, N, value, _TRUNCATE);
}

}  // namespace folder_menus_settings

// -- Button surface ---------------------------------------------------------
// Paint a button the mod owns: hex or accent colors, hover and pressed
// states, border, corner radius, opacity and an optional gradient shine -
// with an empty color meaning 'leave the system default alone'.
namespace folder_menus_button_surface {

using winrt::Windows::UI::Xaml::Controls::Button;
using winrt::Windows::UI::Xaml::Controls::Control;
using winrt::Windows::UI::Xaml::Media::Brush;
using winrt::Windows::UI::Xaml::Media::GradientStop;
using winrt::Windows::UI::Xaml::Media::LinearGradientBrush;
using winrt::Windows::UI::Xaml::Media::SolidColorBrush;

struct Colors {
    std::wstring foreground;
    std::wstring background;
    std::wstring hoverBackground;
    std::wstring pressedBackground;
    std::wstring border;
};

struct Options {
    int opacityPercent = 100;
    int borderThickness = -1; // -1 preserves native value
    int cornerRadius = -1;    // -1 preserves native value
    bool shine = false;
};

// Accepted tokens: "#RRGGBB" or "#AARRGGBB" hex (alpha honored), the generics
// "accent" / "accentLight" / "accentDark" / "transparent", the numbered
// Windows shades "accentLight1"-"3" and "accentDark1"-"3" (accepted silently,
// undocumented), or empty for nullptr — callers must treat nullptr as "leave
// the native surface alone" (ClearValue, not a fallback color).
inline Brush ParseColor(std::wstring const& value) {
    using winrt::Windows::UI::ViewManagement::UIColorType;

    if (_wcsicmp(value.c_str(), L"transparent") == 0) {
        SolidColorBrush brush;
        brush.Color(winrt::Windows::UI::Color{0, 0, 0, 0});
        return brush;
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
        if (_wcsicmp(value.c_str(), entry.token) == 0) {
            try {
                winrt::Windows::UI::ViewManagement::UISettings settings;
                auto color = settings.GetColorValue(entry.type);
                SolidColorBrush brush;
                brush.Color(color);
                return brush;
            } catch (...) {
                return nullptr;
            }
        }
    }

    if (value.empty() || value.front() != L'#')
        return nullptr;
    std::wstring hex = value.substr(1);
    if (hex.size() == 6) hex = L"FF" + hex;
    if (hex.size() != 8) return nullptr;

    uint32_t packed = 0;
    for (wchar_t c : hex) {
        packed <<= 4;
        if (c >= L'0' && c <= L'9') packed |= c - L'0';
        else if (c >= L'A' && c <= L'F') packed |= 10 + c - L'A';
        else if (c >= L'a' && c <= L'f') packed |= 10 + c - L'a';
        else return nullptr;
    }

    winrt::Windows::UI::Color color{
        static_cast<uint8_t>(packed >> 24), static_cast<uint8_t>(packed >> 16),
        static_cast<uint8_t>(packed >> 8), static_cast<uint8_t>(packed)};
    SolidColorBrush brush;
    brush.Color(color);
    return brush;
}

inline Brush MakeShine(Brush const& base, bool enabled) {
    auto solid = enabled && base ? base.try_as<SolidColorBrush>() : nullptr;
    if (!solid) return base;
    auto color = solid.Color();
    auto adjust = [](uint8_t value, int delta) {
        return static_cast<uint8_t>(std::clamp(static_cast<int>(value) + delta,
                                               0, 255));
    };

    LinearGradientBrush brush;
    brush.StartPoint({0.0, 0.0});
    brush.EndPoint({0.0, 1.0});

    GradientStop top;
    top.Color({180, 255, 255, 255});
    top.Offset(0.0);
    brush.GradientStops().Append(top);

    GradientStop light;
    light.Color({color.A, adjust(color.R, 34), adjust(color.G, 34),
                 adjust(color.B, 34)});
    light.Offset(0.42);
    brush.GradientStops().Append(light);

    GradientStop middle;
    middle.Color(color);
    middle.Offset(0.52);
    brush.GradientStops().Append(middle);

    GradientStop bottom;
    bottom.Color({color.A, adjust(color.R, -28), adjust(color.G, -28),
                  adjust(color.B, -28)});
    bottom.Offset(1.0);
    brush.GradientStops().Append(bottom);
    return brush;
}

inline void PutResource(Button const& button, wchar_t const* key,
                        Brush const& brush) {
    if (brush)
        button.Resources().Insert(winrt::box_value(key), brush);
    else
        button.Resources().Remove(winrt::box_value(key));
}

inline void Apply(Button const& button, Colors const& colors,
                  Options const& options) {
    auto foreground = ParseColor(colors.foreground);
    auto background = MakeShine(ParseColor(colors.background), options.shine);
    auto hover = MakeShine(ParseColor(colors.hoverBackground), options.shine);
    auto pressed = MakeShine(ParseColor(colors.pressedBackground), options.shine);
    auto border = ParseColor(colors.border);

    if (foreground) button.Foreground(foreground);
    else button.ClearValue(Control::ForegroundProperty());
    PutResource(button, L"ButtonForeground", foreground);
    PutResource(button, L"ButtonForegroundPointerOver", foreground);
    PutResource(button, L"ButtonForegroundPressed", foreground);

    if (background) button.Background(background);
    else button.ClearValue(Control::BackgroundProperty());
    PutResource(button, L"ButtonBackground", background);

    // Missing state colors deliberately preserve the native theme state.
    PutResource(button, L"ButtonBackgroundPointerOver", hover);
    PutResource(button, L"ButtonBackgroundPressed", pressed);

    if (border) button.BorderBrush(border);
    else button.ClearValue(Control::BorderBrushProperty());
    PutResource(button, L"ButtonBorderBrush", border);
    PutResource(button, L"ButtonBorderBrushPointerOver", border);
    PutResource(button, L"ButtonBorderBrushPressed", border);

    if (options.borderThickness >= 0) {
        double value = static_cast<double>(options.borderThickness);
        button.BorderThickness({value, value, value, value});
    } else {
        button.ClearValue(Control::BorderThicknessProperty());
    }

    if (options.cornerRadius >= 0) {
        double value = static_cast<double>(options.cornerRadius);
        button.CornerRadius({value, value, value, value});
    } else {
        button.ClearValue(Control::CornerRadiusProperty());
    }
    button.Opacity(std::clamp(options.opacityPercent, 0, 100) / 100.0);
}

}  // namespace folder_menus_button_surface

// -- Arrangement expression -------------------------------------------------
// One user-typed string - names joined by '|' (side by side) and ','
// (stacked), nested with parentheses, nudged with [dx,dy] - parsed,
// measured and arranged into concrete placements. Includes the automatic
// grid shape and the policy for items a written arrangement does not name.
namespace folder_menus_layout {

enum class Axis { Horizontal, Vertical };  // node orientation, not a setting
enum class Justify { Start, Center, End };
enum class FillOrder { Rows, Columns };

struct Size {
    double width = 0.0;
    double height = 0.0;
    bool Empty() const { return width <= 0.0 || height <= 0.0; }
};

// Cosmetic per-leaf nudge parsed from the expression's "[dx,dy]" suffix.
struct Offset {
    double x = 0.0;
    double y = 0.0;
};

struct Config {
    double spacing = 0.0;
    Justify justify = Justify::Center;
    double padX = 0.0;  // reserved on BOTH left and right
    double padY = 0.0;  // reserved on BOTH top and bottom
};

struct Placement {
    std::wstring token;
    double x = 0.0;
    double y = 0.0;
    Size size;
};

struct Node {
    std::wstring token;            // non-empty = leaf
    Offset offset;                 // from the "[dx,dy]" suffix; leaf or group
    std::vector<Node> children;    // group children, laid along axis
    Axis axis = Axis::Horizontal;  // group axis (unused for leaves)
};

// Where an arrangement stopped making sense, and what was expected there.
// Report both: a hand-edited expression is much easier to fix with a column
// number than with "did not parse".
struct ParseError {
    size_t position = 0;
    std::wstring expected;
};

class Parser {
public:
    explicit Parser(std::wstring const& text) : text_(text) {}

    bool Run(Node& root) {
        position_ = 0;
        valid_ = true;
        root = ParseExpr();
        SkipSpace();
        if (valid_ && position_ < text_.size())
            Fail(position_, L"a separator ('|' or ',') or end of arrangement");
        return valid_;
    }

    ParseError const& Error() const { return error_; }

private:
    void Fail(size_t position, wchar_t const* expected) {
        if (valid_) {  // keep the first failure; later ones are fallout
            valid_ = false;
            error_ = {position, expected};
        }
    }

    // Parsing, measuring and arranging all recurse once per nesting level, so
    // the depth a user can type is the depth three separate recursions reach.
    // Measure is memoized, so this is no longer a running-time limit — it is a
    // STACK limit, and it is refused at parse time so the user gets a real
    // error instead of a crash. Nothing legible needs this many levels; the
    // deepest arrangement in this mod's own documentation uses three.
    static constexpr int kMaxNestingDepth = 24;

    Node ParseExpr(int depth = 0) {
        Node node;
        node.axis = Axis::Horizontal;
        node.children.push_back(ParseStack(depth));
        while (Peek() == L'|') {
            ++position_;
            node.children.push_back(ParseStack(depth));
        }
        return node;
    }

    Node ParseStack(int depth) {
        Node node;
        node.axis = Axis::Vertical;
        node.children.push_back(ParseUnit(depth));
        while (Peek() == L',') {
            ++position_;
            node.children.push_back(ParseUnit(depth));
        }
        return node;
    }

    Node ParseUnit(int depth) {
        SkipSpace();
        if (position_ < text_.size() && text_[position_] == L'(') {
            if (depth >= kMaxNestingDepth) {
                Fail(position_, L"fewer levels of nested parentheses");
                return {};
            }
            ++position_;
            Node inner = ParseExpr(depth + 1);
            SkipSpace();
            if (position_ < text_.size() && text_[position_] == L')')
                ++position_;
            else
                Fail(position_, L"a closing ')'");
            // A group takes an offset too, moving everything inside it.
            if (position_ < text_.size() && text_[position_] == L'[')
                inner.offset = ParseOffset();
            return inner;
        }

        Node leaf;
        size_t start = position_;
        while (position_ < text_.size() && !IsDelimiter(text_[position_]))
            ++position_;
        leaf.token = text_.substr(start, position_ - start);
        if (leaf.token.empty()) {
            Fail(position_, L"a name");
            return leaf;
        }
        if (position_ < text_.size() && text_[position_] == L'[')
            leaf.offset = ParseOffset();
        return leaf;
    }

    // "[dx,dy]" — signs optional, spaces allowed, both components required.
    Offset ParseOffset() {
        ++position_;  // consume '['
        Offset offset;
        offset.x = ParseNumber();
        SkipSpace();
        if (position_ < text_.size() && text_[position_] == L',')
            ++position_;
        else
            Fail(position_, L"a ',' between the x and y offsets");
        offset.y = ParseNumber();
        SkipSpace();
        if (position_ < text_.size() && text_[position_] == L']')
            ++position_;
        else
            Fail(position_, L"a closing ']'");
        return offset;
    }

    double ParseNumber() {
        SkipSpace();
        wchar_t* end = nullptr;
        double value = std::wcstod(text_.c_str() + position_, &end);
        size_t consumed = end ? (size_t)(end - (text_.c_str() + position_)) : 0;
        if (!consumed) {
            Fail(position_, L"a number");
            return 0.0;
        }
        position_ += consumed;
        if (!std::isfinite(value)) {
            Fail(position_ - consumed, L"a finite number");
            return 0.0;
        }
        // Offsets are cosmetic. Keep expression nudges within the same
        // user-facing range as Adjust.OffsetX/Y so a typo cannot move an icon
        // outside its owned group or hand XAML NaN/infinity.
        return std::clamp(value, -100.0, 100.0);
    }

    static bool IsDelimiter(wchar_t c) {
        return c == L'|' || c == L',' || c == L'(' || c == L')' ||
               c == L'[' || c == L']' || iswspace(c);
    }

    wchar_t Peek() {
        SkipSpace();
        return position_ < text_.size() ? text_[position_] : L'\0';
    }

    void SkipSpace() {
        while (position_ < text_.size() && iswspace(text_[position_]))
            ++position_;
    }

    std::wstring const& text_;
    size_t position_ = 0;
    bool valid_ = true;
    ParseError error_;
};

inline bool Parse(std::wstring const& text, Node& root,
                  ParseError* error = nullptr) {
    Parser parser(text);
    bool ok = parser.Run(root);
    if (!ok && error)
        *error = parser.Error();
    return ok;
}

// ---- Token vocabulary -------------------------------------------------------
//
// Tokens are stable utility identities, compared case-insensitively so an
// arrangement remains readable without depending on localized labels.

inline bool TokenIs(std::wstring const& token, wchar_t const* name) {
    size_t i = 0;
    for (; i < token.size() && name[i]; ++i)
        if (towlower(token[i]) != towlower(name[i]))
            return false;
    return i == token.size() && !name[i];
}

using SizeResolver = std::function<Size(std::wstring const&)>;

// MEASURE IS MEMOIZED, AND HAS TO BE. Each group measures every child twice —
// once in the single-visible-child scan, once in the accumulation loop — and
// Arrange measures the same nodes again at every level. Uncached, that doubles
// per tree level, and since the grammar wraps each unit in its own group, every
// "(" in the expression adds two levels. A hand-typed expression with ~16
// nested parentheses reached roughly 4^16 node visits on the Explorer UI
// thread: a hang with no way out but killing Explorer. Memoizing collapses the
// whole pass to one visit per node.
//
// The cache is keyed on the node's ADDRESS, which is only valid because a Node
// tree is built once by Parse and never mutated or moved while it is being
// measured. Do not hold a cache across a re-parse, and do not mutate a tree
// that a live cache refers to.
using MeasureCache = std::unordered_map<Node const*, Size>;

inline Size MeasureNode(Node const& node, Config const& config,
                        SizeResolver const& resolve, MeasureCache& cache);

inline Size MeasureCached(Node const& node, Config const& config,
                          SizeResolver const& resolve, MeasureCache& cache) {
    auto found = cache.find(&node);
    if (found != cache.end())
        return found->second;
    Size size = MeasureNode(node, config, resolve, cache);
    cache.emplace(&node, size);
    return size;
}

inline Size MeasureNode(Node const& node, Config const& config,
                        SizeResolver const& resolve, MeasureCache& cache) {
    if (!node.token.empty())
        return resolve(node.token);

    // The grammar wraps every unit in a group, so a single-child group is its
    // child and introduces no geometry of its own.
    {
        Node const* only = nullptr;
        int visible = 0;
        for (auto const& child : node.children) {
            if (MeasureCached(child, config, resolve, cache).Empty())
                continue;
            only = &child;
            if (++visible > 1)
                break;
        }
        if (visible == 1)
            return MeasureCached(*only, config, resolve, cache);
    }

    double main = 0.0;
    double cross = 0.0;
    int placed = 0;
    for (auto const& child : node.children) {
        Size size = MeasureCached(child, config, resolve, cache);
        if (size.Empty())
            continue;
        double childMain =
            node.axis == Axis::Horizontal ? size.width : size.height;
        double childCross =
            node.axis == Axis::Horizontal ? size.height : size.width;
        main += (placed ? config.spacing : 0.0) + childMain;
        cross = std::max(cross, childCross);
        ++placed;
    }
    if (!placed)
        return {};
    return node.axis == Axis::Horizontal ? Size{main, cross}
                                         : Size{cross, main};
}

inline void ArrangeCached(Node const& node, Config const& config,
                          SizeResolver const& resolve, double x, double y,
                          std::vector<Placement>& out, MeasureCache& cache,
                          Size const* resolvedSize = nullptr) {
    if (!node.token.empty()) {
        Size size = resolvedSize ? *resolvedSize : resolve(node.token);
        if (!size.Empty())
            out.push_back(
                {node.token, x + node.offset.x, y + node.offset.y, size});
        return;
    }

    Size total = MeasureCached(node, config, resolve, cache);
    if (total.Empty())
        return;
    // A group's own offset moves everything inside it and nothing outside.
    x += node.offset.x;
    y += node.offset.y;

    // A single-child group only carries an optional offset.
    {
        Node const* only = nullptr;
        int visible = 0;
        for (auto const& child : node.children) {
            if (MeasureCached(child, config, resolve, cache).Empty())
                continue;
            only = &child;
            if (++visible > 1)
                break;
        }
        if (visible == 1) {
            ArrangeCached(*only, config, resolve, x, y, out, cache);
            return;
        }
    }

    double cursor = node.axis == Axis::Horizontal ? x : y;
    for (auto const& child : node.children) {
        Size measured = MeasureCached(child, config, resolve, cache);
        if (measured.Empty())
            continue;
        Size size = measured;
        double unused = node.axis == Axis::Horizontal
                            ? total.height - size.height
                            : total.width - size.width;
        double crossOffset = config.justify == Justify::Center ? unused / 2.0
                             : config.justify == Justify::End  ? unused
                                                               : 0.0;
        if (node.axis == Axis::Horizontal) {
            ArrangeCached(child, config, resolve, cursor, y + crossOffset, out,
                          cache, &size);
            cursor += size.width + config.spacing;
        } else {
            ArrangeCached(child, config, resolve, x + crossOffset, cursor, out,
                          cache, &size);
            cursor += size.height + config.spacing;
        }
    }
}

// Parse + measure + arrange in one call. Returns false only on a parse error
// (unbalanced parentheses, malformed offset, trailing garbage) — the caller
// should then fall back to the auto expression and log that it did.
// placements come back in expression order; totalSize is the group's bounding
// box INCLUDING outer padding. A per-item offset shifts its leaf without
// changing totalSize or any neighbor.
inline bool Compute(std::wstring const& text, Config const& config,
                    SizeResolver const& resolve,
                    std::vector<Placement>& placements, Size& totalSize,
                    ParseError* error = nullptr) {
    Node root;
    if (!Parse(text, root, error))
        return false;
    // One cache for both passes: Arrange re-measures the same nodes at every
    // level, so sharing it is what keeps the whole call linear in node count.
    MeasureCache cache;
    Size inner = MeasureCached(root, config, resolve, cache);
    placements.clear();
    if (inner.Empty()) {
        // No visible items: an empty group has no padded box either.
        totalSize = {};
        return true;
    }
    ArrangeCached(root, config, resolve, config.padX, config.padY, placements,
                  cache, &inner);
    totalSize = {inner.width + config.padX * 2.0,
                 inner.height + config.padY * 2.0};
    return true;
}

// How many item rows fit in a height already expressed in DIPs. Pitch is one
// item plus one gap; the trailing gap of the last row is not required, hence
// the + spacing.
//
// RESERVE FIRST. This is the height available to the ITEM GRID, not the whole
// taskbar. Anything else that occupies vertical space — outer padY, an extra
// item shaped as a row (a sliver above or below) — must be subtracted before
// calling, or the grid claims height that is already spoken for and the
// assembled group overflows its host.
inline int RowsInHeight(double heightDip, double itemHeight, double spacing) {
    double pitch = itemHeight + std::max(0.0, spacing);
    if (pitch <= 0.0 || heightDip <= 0.0)
        return 1;
    return std::max(1, (int)((heightDip + std::max(0.0, spacing)) / pitch));
}

// ---- The auto shape ---------------------------------------------------------
//
// Deterministic, not scored. Take the smallest column count reachable within
// the available rows — that is what "use the taskbar's height" means — and
// among the row counts that produce it, the one with the fewest empty slots.
// So 4 items with 3 rows available gives 2x2 rather than a ragged 3+1, and 5
// items with 4 rows available gives 3x2 rather than 4+1.

struct Shape {
    int rows = 1;
    int columns = 1;
};

inline Shape ChooseShape(int count, int maxRows) {
    if (count <= 0)
        return {0, 0};
    int limit = std::max(1, std::min(maxRows, count));
    Shape best{1, count};
    int bestWaste = 0;
    bool first = true;
    for (int rows = 1; rows <= limit; ++rows) {
        int columns = (count + rows - 1) / rows;
        int waste = rows * columns - count;
        if (first || columns < best.columns ||
            (columns == best.columns && waste < bestWaste)) {
            first = false;
            best = {rows, columns};
            bestWaste = waste;
        }
    }
    return best;
}

// ---- Expression generation --------------------------------------------------
//
// Turn a rows x columns shape into an expression so the auto path and the
// manual path are the same code below this point. Positions fill row-major
// (left to right, then down) for FillOrder::Rows or column-major (top to
// bottom, then right) for FillOrder::Columns. Grid positions past `count` are
// simply absent, so a ragged final row or column yields fewer tokens and the
// result is always a valid expression. Justify aligns that ragged group.
//
// Tokens come from namer(index); the default names items by 1-based number,
// matching what a user reads on screen. The caller's SizeResolver must map
// those same names back to pixel sizes.

using TokenNamer = std::function<std::wstring(int index)>;

inline std::wstring BuildGridExpression(int count, int rows, int columns,
                                        FillOrder fill,
                                        TokenNamer const& namer = {}) {
    if (count <= 0 || rows <= 0 || columns <= 0)
        return {};

    auto name = [&](int index) -> std::wstring {
        return namer ? namer(index) : std::to_wstring(index + 1);
    };

    // '|' groups are columns, ',' units are rows, always.
    std::wstring expr;
    for (int column = 0; column < columns; ++column) {
        std::wstring stack;
        for (int row = 0; row < rows; ++row) {
            int index = fill == FillOrder::Rows ? row * columns + column
                                                : column * rows + row;
            if (index < 0 || index >= count)
                continue;
            if (!stack.empty())
                stack += L", ";
            stack += name(index);
        }
        if (stack.empty())
            continue;
        if (!expr.empty())
            expr += L" | ";
        expr += stack;
    }
    return expr;
}

inline std::wstring BuildAutoExpression(int count, int maxRows, FillOrder fill,
                                        TokenNamer const& namer = {}) {
    Shape shape = ChooseShape(count, maxRows);
    return BuildGridExpression(count, shape.rows, shape.columns, fill, namer);
}

// ---- Items the arrangement forgot -------------------------------------------
//
// A hand-written arrangement names the utilities that existed when it was
// written. Windows shows and hides these live — the touch keyboard comes and
// goes, the taskbar settings toggle the rest — so a utility that appears later
// is in no group, resolves to nothing, and silently vanishes from the taskbar.
// That is a trap, hence Layout.NewItems:
//
//   Append (default) — arrange the unlisted items automatically and put that
//                      block after everything the user wrote, so a new item is
//                      always reachable and the written block stays intact.
//   Ignore           — the arrangement is the whole truth; unlisted items stay
//                      off the taskbar until the user adds them.
//
// Appending is logged, so the user knows to fold the new item into their
// arrangement when they next edit it.

// Whether a token the user wrote refers to the same item as the one expected.
// A plain case-insensitive name match is WRONG here, because the vocabulary
// accepts aliases: "chevron" and "overflow" are one button, and comparing them
// as strings makes an aliased item look missing and get appended a second
// time. SameUtility below supplies the identity comparison.
using TokenMatcher =
    std::function<bool(std::wstring const& placed, std::wstring const& expected)>;

inline std::vector<std::wstring> MissingTokens(
    std::vector<std::wstring> const& expected,
    std::vector<Placement> const& placements,
    TokenMatcher const& same = {}) {
    std::vector<std::wstring> missing;
    for (auto const& token : expected) {
        bool found = false;
        for (auto const& placement : placements) {
            bool match = same ? same(placement.token, token)
                              : TokenIs(placement.token, token.c_str());
            if (match) {
                found = true;
                break;
            }
        }
        if (!found)
            missing.push_back(token);
    }
    return missing;
}

inline std::wstring AppendMissing(std::wstring const& expression,
                                  std::vector<std::wstring> const& missing,
                                  int maxRows, FillOrder fill) {
    if (missing.empty())
        return expression;
    auto namer = [&missing](int index) { return missing[index]; };
    std::wstring block = BuildAutoExpression((int)missing.size(), maxRows, fill,
                                             namer);
    if (block.empty())
        return expression;
    if (expression.empty())
        return block;
    return L"(" + expression + L") | (" + block + L")";
}

// ---- The one setting --------------------------------------------------------
//
// Resolve `Layout.Arrangement` to the expression to arrange. Empty or the word
// "auto" (any case, surrounding space ignored) means generate one. The caller
// logs the result when wasAuto is true so the user can paste it back into the
// same field and edit it.

struct Arrangement {
    std::wstring expression;
    bool wasAuto = false;
};

inline bool IsAutoSetting(std::wstring const& setting) {
    size_t first = setting.find_first_not_of(L" \t\r\n");
    if (first == std::wstring::npos)
        return true;
    size_t last = setting.find_last_not_of(L" \t\r\n");
    std::wstring trimmed = setting.substr(first, last - first + 1);
    if (trimmed.size() != 4)
        return false;
    for (size_t i = 0; i < 4; ++i)
        if (towlower(trimmed[i]) != L"auto"[i])
            return false;
    return true;
}

inline Arrangement ResolveArrangement(std::wstring const& setting, int count,
                                      int maxRows, FillOrder fill,
                                      TokenNamer const& namer = {}) {
    if (IsAutoSetting(setting))
        return {BuildAutoExpression(count, maxRows, fill, namer), true};
    return {setting, false};
}

}  // namespace folder_menus_layout

// -- Tray slot lease --------------------------------------------------------
// Lease a position in the tray panel - a column on a Grid, a child index on
// a StackPanel - tracked by a named zero-size marker so the release stays
// exact after other mods inject siblings around it.
namespace folder_menus_slot_lease {

using winrt::Windows::UI::Xaml::FrameworkElement;
using winrt::Windows::UI::Xaml::GridUnitType;
using winrt::Windows::UI::Xaml::Controls::ColumnDefinition;
using winrt::Windows::UI::Xaml::Controls::Grid;
using winrt::Windows::UI::Xaml::Controls::Panel;
using winrt::Windows::UI::Xaml::Controls::StackPanel;

enum class Anchor {
    BeforeIcons,
    BeforeOmni,
    BeforeClock,
    AfterClock,
    AfterShowDesktop,
};

// Which layout contract the live tray panel follows.
enum class Kind {
    Unsupported,  // some other Panel: do not guess its layout semantics.
    Columns,      // Grid. A slot is a column index.
    Order,        // StackPanel. A slot is a child index.
};

struct Lease {
    std::wstring markerName;
    int slot = -1;
    Kind kind = Kind::Unsupported;
};

inline Kind Classify(FrameworkElement const& parent) {
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
inline std::wstring ClassName(FrameworkElement const& element) {
    if (!element)
        return L"(null)";
    try {
        return std::wstring(winrt::get_class_name(element));
    } catch (...) {
        return L"(unknown)";
    }
}

// Named direct children, for logging when an anchor cannot be resolved. A tray
// restructure shows up here as missing or renamed names, which is the one thing
// a user's debug log otherwise cannot tell us.
inline std::wstring DescribeChildren(Panel const& parent) {
    if (!parent)
        return L"(none)";
    std::wstring names;
    try {
        for (auto const& child : parent.Children()) {
            auto element = child.try_as<FrameworkElement>();
            if (!element || element.Name().empty())
                continue;
            if (!names.empty())
                names += L", ";
            names += element.Name();
        }
    } catch (...) {
        return L"(unreadable)";
    }
    return names.empty() ? L"(no named children)" : names;
}

inline FrameworkElement FindDirectChild(Panel const& parent,
                                        wchar_t const* name) {
    if (!parent || !name)
        return nullptr;
    for (auto const& child : parent.Children()) {
        auto element = child.try_as<FrameworkElement>();
        if (element && element.Name() == name)
            return element;
    }
    return nullptr;
}

inline int IndexOfChild(Panel const& parent, FrameworkElement const& child) {
    if (!parent || !child)
        return -1;
    for (uint32_t i = 0; i < parent.Children().Size(); ++i) {
        if (parent.Children().GetAt(i).try_as<FrameworkElement>() == child)
            return static_cast<int>(i);
    }
    return -1;
}

inline bool ResolveSlot(Panel const& parent, Anchor anchor, int& slot) {
    Kind kind = Classify(parent);
    if (kind == Kind::Unsupported)
        return false;

    if (anchor == Anchor::BeforeIcons) {
        slot = 0;
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
        return false; // Never silently turn an unavailable anchor into slot 0.

    if (kind == Kind::Columns) {
        slot = Grid::GetColumn(reference) +
               (after ? std::max(1, Grid::GetColumnSpan(reference)) : 0);
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

inline bool AcquireAt(Panel const& parent, int slot,
                      std::wstring const& markerName, Lease& lease) {
    Kind kind = Classify(parent);
    if (kind == Kind::Unsupported || slot < 0 || markerName.empty())
        return false;

    // A marker with this name already in the tray is either this caller's own
    // live lease — refuse, acquiring twice would strand the first slot — or a
    // leftover from an instance whose teardown never reached the UI thread.
    // Refusing the leftover would block every later apply until Explorer
    // restarts, so release it and take the slot fresh, correcting the
    // requested slot if the released one sat before it.
    if (auto stale = FindDirectChild(parent, markerName.c_str())) {
        if (lease.markerName == markerName)
            return false;
        int staleSlot = kind == Kind::Columns
                            ? Grid::GetColumn(stale)
                            : IndexOfChild(parent, stale);
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

    if (kind == Kind::Columns) {
        auto grid = parent.try_as<Grid>();
        if (!grid)
            return false;
        ColumnDefinition definition;
        definition.Width({1.0, GridUnitType::Auto});
        if (static_cast<uint32_t>(slot) < grid.ColumnDefinitions().Size())
            grid.ColumnDefinitions().InsertAt(slot, definition);
        else
            grid.ColumnDefinitions().Append(definition);

        for (auto const& child : grid.Children()) {
            auto element = child.try_as<FrameworkElement>();
            if (!element) continue;
            int start = Grid::GetColumn(element);
            int span = Grid::GetColumnSpan(element);
            if (start >= slot)
                Grid::SetColumn(element, start + 1);
            else if (start + span > slot)
                Grid::SetColumnSpan(element, span + 1);
        }

        Grid::SetColumn(marker, slot);
        grid.Children().Append(marker);
    } else {
        // Order: no column is created or owned. The marker simply holds the
        // position, and PlaceChild drops the content next to it.
        uint32_t index = std::min(static_cast<uint32_t>(slot),
                                  parent.Children().Size());
        parent.Children().InsertAt(index, marker);
        slot = static_cast<int>(index);
    }

    lease = {markerName, slot, kind};
    return true;
}

inline bool Acquire(Panel const& parent, Anchor anchor,
                    std::wstring const& markerName, Lease& lease) {
    int slot = -1;
    if (!parent || !ResolveSlot(parent, anchor, slot))
        return false;
    return AcquireAt(parent, slot, markerName, lease);
}

// Live index of the lease marker. Other mods inject and remove siblings around
// us, so the acquire-time index is a hint, never the truth at removal time.
inline bool FindMarker(Panel const& parent, Lease const& lease,
                       uint32_t& index) {
    if (!parent || lease.markerName.empty())
        return false;
    for (uint32_t i = 0; i < parent.Children().Size(); ++i) {
        auto element = parent.Children().GetAt(i).try_as<FrameworkElement>();
        if (element && element.Name() == lease.markerName) {
            index = i;
            return true;
        }
    }
    return false;
}

// Put mod content into the leased slot. On a Grid the content joins the leased
// column; on a StackPanel it is inserted directly after the marker, so the
// marker's position is the content's position.
inline bool PlaceChild(Panel const& parent, Lease const& lease,
                       FrameworkElement const& content) {
    if (!parent || !content || lease.kind == Kind::Unsupported)
        return false;

    uint32_t markerIndex = 0;
    if (!FindMarker(parent, lease, markerIndex))
        return false;

    if (lease.kind == Kind::Columns) {
        auto marker = parent.Children().GetAt(markerIndex)
                          .try_as<FrameworkElement>();
        Grid::SetColumn(content, marker ? Grid::GetColumn(marker) : lease.slot);
        parent.Children().Append(content);
        return true;
    }

    parent.Children().InsertAt(markerIndex + 1, content);
    return true;
}

inline bool Release(Panel const& parent, Lease& lease) {
    if (!parent || lease.markerName.empty())
        return false;

    uint32_t markerIndex = 0;
    if (!FindMarker(parent, lease, markerIndex))
        return false;

    if (lease.kind == Kind::Order) {
        parent.Children().RemoveAt(markerIndex);
        lease = {};
        return true;
    }

    auto grid = parent.try_as<Grid>();
    auto marker =
        parent.Children().GetAt(markerIndex).try_as<FrameworkElement>();
    int liveColumn = marker ? Grid::GetColumn(marker) : lease.slot;
    if (!grid || liveColumn < 0)
        return false;

    grid.Children().RemoveAt(markerIndex);
    if (static_cast<uint32_t>(liveColumn) < grid.ColumnDefinitions().Size())
        grid.ColumnDefinitions().RemoveAt(liveColumn);

    for (auto const& child : grid.Children()) {
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

}  // namespace folder_menus_slot_lease

// -- Taskbar window discovery -----------------------------------------------
// Find this process's Shell_TrayWnd, and validate a cached handle before
// preferring it.
namespace folder_menus_taskbar_window {

// ---- Window discovery -------------------------------------------------------


inline HWND FindCurrentProcessTaskbarWnd() {
    HWND result = nullptr;
    EnumWindows(
        [](HWND window, LPARAM parameter) -> BOOL {
            DWORD processId = 0;
            WCHAR className[32];
            if (GetWindowThreadProcessId(window, &processId) &&
                processId == GetCurrentProcessId() &&
                GetClassName(window, className, ARRAYSIZE(className)) &&
                _wcsicmp(className, L"Shell_TrayWnd") == 0) {
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
inline HWND ResolveTaskbarWnd(HWND cached) {
    if (cached && IsWindow(cached))
        return cached;
    return FindCurrentProcessTaskbarWnd();
}

}  // namespace folder_menus_taskbar_window

// -- UI-thread dispatch -----------------------------------------------------
// Marshal a callback onto the taskbar's UI thread with a CALLWNDPROC hook
// and a private registered message, reporting whether it actually ran.
namespace folder_menus_dispatch {

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
inline void SetExceptionLogger(ExceptionLogFn logger) {
    g_logException = logger;
}

inline bool Invoke(ThreadProc proc, void* parameter) {
    try {
        proc(parameter);
        return true;
    } catch (...) {
        if (g_logException) g_logException(L"UI callback");
    }
    return false;
}

struct Dispatch {
    ThreadProc proc;
    void* parameter;
    bool succeeded = false;
    // Every concurrent caller installs its own hook with this same proc, and
    // each hook instance sees every message equal to g_dispatchMessage. With
    // two dispatches in flight, both hooks are in the chain when either
    // message arrives, so without this each callback would run twice. The
    // hooks run one after another on the UI thread, so a plain flag suffices.
    bool ran = false;
};

// The private message this mod dispatches on. Set before the hook is
// installed, and read by the hook proc to recognise its own message.
//
// A CALLWNDPROC HOOK SEES EVERY MESSAGE SENT TO EVERY WINDOW ON THE TASKBAR'S
// UI THREAD. `lParam` for all of those is arbitrary — an integer, a flag, a
// pointer to something else entirely. So the message MUST be checked first,
// against a value that does not come from lParam, and only then may lParam be
// treated as a Dispatch*. Reading anything out of lParam before that check
// dereferences whatever happened to be in the message and takes Explorer down
// with it.
//
// Atomic because the caller may be the retry thread while the hook
// proc runs on the taskbar's UI thread. RegisterWindowMessageW returns the
// same value for the same string for the lifetime of the session, so this
// settles on one value immediately and never changes again.
inline std::atomic<UINT> g_dispatchMessage{0};

// messageName must embed WH_MOD_ID, so two mods cannot collide on the message.
inline bool RunFromWindowThread(HWND window, ThreadProc proc, void* parameter,
                                PCWSTR messageName) {
    UINT message = RegisterWindowMessageW(messageName);
    if (!message) return false;

    DWORD threadId = GetWindowThreadProcessId(window, nullptr);
    if (!threadId) return false;
    if (threadId == GetCurrentThreadId()) return Invoke(proc, parameter);

    g_dispatchMessage.store(message, std::memory_order_release);

    HHOOK hook = SetWindowsHookExW(
        WH_CALLWNDPROC,
        [](int code, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (code == HC_ACTION) {
                auto const* call = reinterpret_cast<CWPSTRUCT const*>(lParam);
                // Message first. Only our own private message carries a
                // Dispatch* in lParam; everything else carries something we
                // must not touch.
                UINT expected =
                    g_dispatchMessage.load(std::memory_order_acquire);
                if (expected && call->message == expected) {
                    if (auto* dispatch =
                            reinterpret_cast<Dispatch*>(call->lParam);
                        dispatch && !dispatch->ran) {
                        dispatch->ran = true;
                        dispatch->succeeded =
                            Invoke(dispatch->proc, dispatch->parameter);
                    }
                }
            }
            return CallNextHookEx(nullptr, code, wParam, lParam);
        },
        nullptr, threadId);
    if (!hook) return false;

    Dispatch dispatch{proc, parameter};
    SendMessageW(window, message, 0, reinterpret_cast<LPARAM>(&dispatch));
    UnhookWindowsHookEx(hook);
    return dispatch.succeeded;
}

}  // namespace folder_menus_dispatch

// -- Taskbar XamlRoot -------------------------------------------------------
// Hook the taskbar.dll symbols, reach the taskbar's XamlRoot, and call back
// when Explorer rebuilds the taskbar in place.
namespace dispatch = folder_menus_dispatch;
namespace folder_menus_taskbar_xaml {

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

// The mod's rebuild callback, invoked after Explorer rebuilds the taskbar.
inline void (*g_onTaskbarRebuilt)() = nullptr;

inline void WINAPI TrayUI_StartTaskbar_Hook(void* self) {
    TrayUI_StartTaskbar_Original(self);
    try {
        if (g_onTaskbarRebuilt) g_onTaskbarRebuilt();
    } catch (...) {
        if (dispatch::g_logException)
            dispatch::g_logException(L"TrayUI::StartTaskbar hook");
    }
}

inline bool HookTaskbarSymbols(void (*onTaskbarRebuilt)()) {
    g_onTaskbarRebuilt = onTaskbarRebuilt;
    HMODULE taskbar = LoadLibraryExW(L"taskbar.dll", nullptr,
                                     LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!taskbar) return false;
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
         &TrayUI_StartTaskbar_Original, TrayUI_StartTaskbar_Hook},
    };
    return WindhawkUtils::HookSymbols(taskbar, taskbarDllHooks,
                                      ARRAYSIZE(taskbarDllHooks));
}

// The FrameworkElement lives at an offset inside TaskbarHost that MOVES
// between Windows builds, so it is read out of TaskbarHost::FrameHeight's
// prologue at runtime rather than hardcoded.
inline size_t FrameworkElementOffset() {
    size_t offset = 0x10;
#if defined(_M_X64)
    BYTE const* code =
        reinterpret_cast<BYTE const*>(TaskbarHost_FrameHeight_Original);
    if (code[0] == 0x48 && code[1] == 0x83 && code[2] == 0xEC &&
        code[4] == 0x48 && code[5] == 0x83 && code[6] == 0xC1 &&
        code[7] <= 0x7F) {
        offset = code[7];
    }
#elif defined(_M_ARM64)
    DWORD const* code =
        reinterpret_cast<DWORD const*>(TaskbarHost_FrameHeight_Original);
    if (code[0] == 0xD503237F && (code[1] & 0xFFC07FFF) == 0xA9807BFD &&
        code[2] == 0x910003FD && (code[3] & 0xFFF00FE0) == 0xF8400C00) {
        offset = (code[3] >> 12) & 0xFF;
    }
#else
#error "Unsupported architecture"
#endif
    return offset;
}

inline XamlRoot GetTaskbarXamlRoot(HWND taskbarWnd) {
    if (!CTaskBand_GetTaskbarHost_Original ||
        !TaskbarHost_FrameHeight_Original || !Ref_count_base_Decref_Original ||
        !CTaskBand_ITaskListWndSite_vftable)
        return nullptr;

    HWND taskSwWnd = (HWND)GetProp(taskbarWnd, L"TaskbandHWND");
    if (!taskSwWnd) return nullptr;
    void* taskBand = (void*)GetWindowLongPtr(taskSwWnd, 0);
    if (!taskBand) return nullptr;

    void* site = taskBand;
    for (int i = 0; *(void**)site != CTaskBand_ITaskListWndSite_vftable; ++i) {
        if (i == 20) return nullptr;
        site = (void**)site + 1;
    }

    void* host[2]{};
    CTaskBand_GetTaskbarHost_Original(site, host);
    if (!host[0] || !host[1]) {
        if (host[1]) Ref_count_base_Decref_Original(host[1]);
        return nullptr;
    }

    auto* unknown =
        *(IUnknown**)((BYTE*)host[0] + FrameworkElementOffset());
    if (!unknown) {
        Ref_count_base_Decref_Original(host[1]);
        return nullptr;
    }
    FrameworkElement element = nullptr;
    unknown->QueryInterface(winrt::guid_of<FrameworkElement>(),
                            winrt::put_abi(element));
    auto result = element ? element.XamlRoot() : nullptr;
    Ref_count_base_Decref_Original(host[1]);
    return result;
}

}  // namespace folder_menus_taskbar_xaml

// -- Bounded retry loop -----------------------------------------------------
// A stoppable, waited worker that retries an apply a bounded number of
// times. Safe against a Stop from one thread racing a Start from another.
namespace folder_menus_retry {

// ---- Bounded retry ----------------------------------------------------------
//
// Stoppable and WAITED during unload. A detached thread that outlives
// Wh_ModUninit runs mod code out of an unloaded DLL.
//
// STOP IS CALLED FROM MORE THAN ONE THREAD. Wh_ModUninit stops the loop from
// Windhawk's thread while an Explorer taskbar rebuild can be starting it from
// the taskbar's UI thread, and Start() stops the previous run before it begins
// a new one. So the handles cannot live in bare members that each caller
// closes: two callers would read the same handle and close it twice, and in
// explorer.exe a double CloseHandle later closes whatever unrelated handle the
// value was recycled into.
//
// One attempt therefore owns its handles through a shared Run, and EVERY
// caller that observes a live Run waits for it. The mutex is held only across
// the handoff, never across the wait: the retry thread marshals onto the UI
// thread with SendMessage, so a UI-thread caller blocked on the mutex while
// another thread waited under it could never service that message.
//
// Start() itself is not serialized against a concurrent Start(), because both
// of this mod's callers run on the taskbar's UI thread.


class RetryLoop {
public:
    // applied: has the work finished? unloading: stop immediately.
    using AppliedFn = bool (*)();
    using AttemptFn = void (*)();

    // No destructor on purpose. A namespace-scope loop's destructor would run
    // at DLL detach, inside the loader lock, and Stop() waits on a thread —
    // the owner stops it explicitly from Wh_ModUninit instead.

    void Start(AttemptFn attempt, AppliedFn applied,
               std::atomic<bool> const& unloading, int attempts = 5,
               DWORD intervalMs = 2000, bool forceFirstAttempt = false) {
        Stop();
        if (unloading) return;

        auto run = std::make_shared<Run>();
        run->attempt = attempt;
        run->applied = applied;
        run->unloading = &unloading;
        run->attempts = attempts;
        run->intervalMs = intervalMs;
        run->forceFirstAttempt = forceFirstAttempt;
        run->stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
        if (!run->stopEvent) return;  // ~Run closes nothing it did not create

        // The thread carries a reference of its own, so the Run survives until
        // both the loop and the thread are done with it, whichever ends first.
        auto* parameter = new std::shared_ptr<Run>(run);
        run->thread =
            CreateThread(nullptr, 0, ThreadMain, parameter, 0, nullptr);
        if (!run->thread) {
            delete parameter;
            return;
        }

        // PUBLISH BY EXCHANGE, AND WAIT FOR WHATEVER THIS DISPLACES. The Stop()
        // above runs OUTSIDE the mutex and pumps sent messages while it waits,
        // so a second Start() can slip in behind it: two callers both get past
        // Stop(), and an unconditional store would drop the first run's last
        // tracked reference. Its thread keeps going on its own reference with
        // nothing able to stop it, and an unload inside that window frees the
        // image under a thread still dereferencing `unloading`.
        std::shared_ptr<Run> displaced;
        {
            std::lock_guard<std::mutex> guard(mutex_);
            if (!unloading)
                displaced = std::exchange(run_, std::move(run));
        }
        // `run` is only non-null here when unload began while this attempt was
        // being created, so the Stop that would have waited for it saw nothing.
        if (run) StopRun(run);
        // A concurrent Start() installed its run after ours got past Stop().
        if (displaced) StopRun(displaced);
    }

    void Stop() {
        std::shared_ptr<Run> run;
        {
            std::lock_guard<std::mutex> guard(mutex_);
            run = run_;  // shared, not moved: a concurrent Stop must wait too
        }
        if (!run) return;
        StopRun(run);
        std::lock_guard<std::mutex> guard(mutex_);
        if (run_ == run) run_.reset();
    }

private:
    struct Run {
        HANDLE thread = nullptr;
        HANDLE stopEvent = nullptr;
        AttemptFn attempt = nullptr;
        AppliedFn applied = nullptr;
        std::atomic<bool> const* unloading = nullptr;
        int attempts = 5;
        DWORD intervalMs = 2000;
        bool forceFirstAttempt = false;

        // Closed exactly once, when the last of the loop and the thread lets
        // go. Both have already stopped using them by then.
        ~Run() {
            if (thread) CloseHandle(thread);
            if (stopEvent) CloseHandle(stopEvent);
        }
    };

    static DWORD WINAPI ThreadMain(void* parameter) {
        auto* owned = static_cast<std::shared_ptr<Run>*>(parameter);
        std::shared_ptr<Run> run = *owned;
        delete owned;
        for (int i = 0; i < run->attempts && !*run->unloading; ++i) {
            // Opt-in, via forceFirstAttempt. A caller that clears its own
            // "applied" flag before starting does not need it. It exists for
            // the caller that must run one restore/reapply pass while `applied`
            // still truthfully reports that it owns live XAML — so that flag
            // does not have to be falsified just to wake this loop.
            if (run->applied && !(run->forceFirstAttempt && i == 0) &&
                run->applied())
                break;
            if (i && WaitForSingleObject(run->stopEvent, run->intervalMs) !=
                         WAIT_TIMEOUT)
                break;
            if (run->attempt) run->attempt();
        }
        return 0;
    }

    // Signal and wait, pumping sent messages: a caller on the taskbar's UI
    // thread would otherwise deadlock against the SendMessage the retry thread
    // is making back to it. Idempotent — the stop event is manual-reset, and
    // waiting on an already-exited thread returns at once.
    static void StopRun(std::shared_ptr<Run> const& run) {
        if (run->stopEvent) SetEvent(run->stopEvent);
        if (!run->thread) return;
        DWORD result;
        do {
            HANDLE thread = run->thread;
            result = MsgWaitForMultipleObjects(1, &thread, FALSE, INFINITE,
                                               QS_SENDMESSAGE);
            if (result == WAIT_OBJECT_0 + 1) {
                MSG message;
                PeekMessageW(&message, nullptr, 0, 0, PM_NOREMOVE);
            }
        } while (result == WAIT_OBJECT_0 + 1);
    }

    std::mutex mutex_;
    std::shared_ptr<Run> run_;
};

}  // namespace folder_menus_retry

// ==/ModComponents==

namespace sio = folder_menus_settings;
namespace bs = folder_menus_button_surface;
namespace ngl = folder_menus_layout;
namespace igc = folder_menus_slot_lease;
namespace taskbar_window = folder_menus_taskbar_window;
namespace dispatch = folder_menus_dispatch;
namespace taskbar_xaml = folder_menus_taskbar_xaml;
namespace retry_loop = folder_menus_retry;

// ============================================================
// Settings
// ============================================================

struct FolderEntry {
    std::wstring label;
    std::wstring target;
    bool useDefaultIcon = false;
};

// Placement.Position, parsed once at load. Every other value is a tray anchor;
// AfterTaskbarIcons places the group in the taskbar's app lane instead.
enum class Position {
    BeforeIcons,
    BeforeOmni,
    BeforeClock,
    AfterClock,
    AfterShowDesktop,
    AfterTaskbarIcons,
};

static PCWSTR PositionName(Position position) {
    switch (position) {
        case Position::BeforeIcons:       return L"beforeIcons";
        case Position::BeforeOmni:        return L"beforeOmni";
        case Position::BeforeClock:       return L"beforeClock";
        case Position::AfterClock:        return L"afterClock";
        case Position::AfterShowDesktop:  return L"afterShowDesktop";
        case Position::AfterTaskbarIcons: return L"afterTaskbarIcons";
    }
    return L"unknown";
}

struct ModSettings {
    Position position = Position::BeforeIcons;
    std::wstring arrangement = L"auto";
    ngl::FillOrder layoutFill = ngl::FillOrder::Rows;
    ngl::Justify justify = ngl::Justify::Center;
    bool appendNewItems = true;
    int padX = 0;
    int padY = 0;
    std::wstring buttonText = L"📁";
    std::vector<FolderEntry> folders;
    int buttonWidth = 24;
    int buttonHeight = 22;
    int buttonSpacing = 4;
    int maxMenuItems = 150;
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

// WindhawkUtils::StringSetting already does the RAII free, and its `make`
// takes the same format arguments Wh_GetStringSetting does, so the indexed
// folder keys go through one reader like everything else.
template <typename... Args>
static std::wstring GetStringSetting(PCWSTR name, Args... args) {
    auto value = WindhawkUtils::StringSetting::make(name, args...);
    return value.get() ? std::wstring(value.get()) : std::wstring{};
}

static std::vector<FolderEntry> LoadFolders() {
    std::vector<FolderEntry> folders;
    for (int i = 0;; i++) {
        std::wstring target = ExpandEnv(
            Trim(GetStringSetting(L"Content.Folders[%d].Target", i)));
        if (target.empty())
            break;
        std::wstring label =
            Trim(GetStringSetting(L"Content.Folders[%d].Label", i));
        bool useDefaultIcon = Wh_GetIntSetting(
            L"Content.Folders[%d].UseDefaultIcon", i) != 0;
        folders.push_back({std::move(label), std::move(target), useDefaultIcon});
    }

    if (folders.empty()) {
        folders.push_back({ L"🖥", L"shell:Desktop" });
        folders.push_back({ L"⚙", L"shell:ControlPanelFolder" });
    }
    return folders;
}

// Settings were reorganised into groups in 2.0. Windhawk cannot write a
// setting, so any reader of the old flat keys would leave the settings page
// showing one value while the mod used another; the README instead asks 0.7
// users to re-apply their settings once.
static void LoadSettings() {
    static constexpr sio::Choice<Position> kPositions[] = {
        {L"beforeIcons", Position::BeforeIcons},
        {L"beforeOmni", Position::BeforeOmni},
        {L"beforeClock", Position::BeforeClock},
        {L"afterClock", Position::AfterClock},
        {L"afterShowDesktop", Position::AfterShowDesktop},
        {L"afterTaskbarIcons", Position::AfterTaskbarIcons},
    };
    g_settings.position = sio::LoadChoice(L"Placement.Position", kPositions,
                                          Position::BeforeIcons);

    g_settings.arrangement = GetStringSetting(L"Layout.Arrangement");
    static constexpr sio::Choice<ngl::FillOrder> kFillOrders[] = {
        {L"rows", ngl::FillOrder::Rows},
        {L"columns", ngl::FillOrder::Columns},
    };
    g_settings.layoutFill = sio::LoadChoice(L"Layout.FillOrder", kFillOrders,
                                            ngl::FillOrder::Rows);
    static constexpr sio::Choice<ngl::Justify> kJustifications[] = {
        {L"start", ngl::Justify::Start},
        {L"center", ngl::Justify::Center},
        {L"end", ngl::Justify::End},
    };
    g_settings.justify = sio::LoadChoice(L"Layout.Justify", kJustifications,
                                         ngl::Justify::Center);
    static constexpr sio::Choice<bool> kNewItemPolicies[] = {
        {L"append", true},
        {L"hide", false},
    };
    g_settings.appendNewItems =
        sio::LoadChoice(L"Layout.NewItems", kNewItemPolicies, true);

    g_settings.padX = sio::LoadInt(L"Adjust.PadX", 0, 80);
    g_settings.padY = sio::LoadInt(L"Adjust.PadY", 0, 80);
    g_settings.groupOffsetX = sio::LoadInt(L"Adjust.OffsetX", -80, 80);
    g_settings.groupOffsetY = sio::LoadInt(L"Adjust.OffsetY", -80, 80);

    g_settings.buttonText = GetStringSetting(L"Content.DefaultLabel");
    g_settings.folders = LoadFolders();

    g_settings.buttonWidth = sio::LoadInt(L"Size.ItemWidth", 10, 256);
    g_settings.buttonHeight = sio::LoadInt(L"Size.ItemHeight", 10, 256);
    g_settings.buttonSpacing = sio::LoadInt(L"Size.ItemSpacing", 0, 80);

    g_settings.maxMenuItems = sio::LoadInt(L"Behavior.MaxMenuItems", 0, 2000);
    g_settings.maxDepth = sio::LoadInt(L"Behavior.MaxDepth", 0, 64);
    g_settings.showHidden = sio::LoadBool(L"Behavior.ShowHidden");

    g_settings.fontSize = sio::LoadInt(L"Surface.FontSize", 1, 96);
    g_settings.textColor = GetStringSetting(L"Surface.TextColor");
    g_settings.backgroundColor = GetStringSetting(L"Surface.BackgroundColor");
    g_settings.hoverBackgroundColor =
        GetStringSetting(L"Surface.HoverBackgroundColor");
    g_settings.pressedBackgroundColor =
        GetStringSetting(L"Surface.PressedBackgroundColor");
    g_settings.borderColor = GetStringSetting(L"Surface.BorderColor");
    g_settings.borderThickness =
        sio::LoadInt(L"Surface.BorderThickness", -1, 64);
    g_settings.cornerRadius = sio::LoadInt(L"Surface.CornerRadius", -1, 64);
    g_settings.opacityPct = sio::LoadInt(L"Surface.Opacity", 0, 100);
    g_settings.shineEffect = sio::LoadBool(L"Surface.ShineEffect");
}

// ============================================================
// Globals
// ============================================================

static std::atomic<bool> g_unloading{false};
static std::atomic<bool> g_updatingSettings{false};
static HWND              g_taskbarWnd = nullptr;
[[clang::no_destroy]] static Grid g_buttonGrid = nullptr;
// Grid column on older taskbars, child index on the 26200.9457 StackPanel.
static int               g_injectedSlot = -1;
static igc::Lease g_columnLease; // exit-time-safe: heap-only
static std::atomic<bool>  g_injectionLive{false};

struct ButtonEventState {
    Button button{nullptr};
    winrt::event_token clickToken{};
};
[[clang::no_destroy]] static std::optional<std::vector<ButtonEventState>>
    g_buttonEventStates{std::in_place};

// TrackPopupMenu owns a nested UI loop with two mod callbacks installed into
// it. Unload ends that loop on its owning thread and waits for this event
// before Windhawk can unload the code those callbacks execute.
//
// THE HOOK AND THE OWNER ARE GLOBAL ON PURPOSE. While the menu is up, the
// taskbar thread is inside this mod's ShowFolderMenu frame with this mod's
// WH_MSGFILTER hook installed. If unload gave up and returned while either was
// live, Windhawk would FreeLibrary the image under a running call and take
// Explorer with it. Keeping them reachable from a global is what lets
// Wh_ModUninit undo them itself instead of hoping the menu unwinds.
static std::atomic<int> g_menuLoopDepth{0};
static HANDLE g_menuIdleEvent = nullptr;
static HHOOK g_menuMsgFilterHook = nullptr;
static HWND g_menuOwner = nullptr;

// Marks a span in which the taskbar thread is executing this mod's menu path:
// the folder popup's modal loop, or a queued Shell verb, which may pump its own
// modal loop (a Delete confirmation, Open With) with this image on the stack.
// Wh_ModUninit waits until no such span is open. One RAII type keeps the
// increment/reset and decrement/signal pairing in one place.
struct MenuPathScope {
    MenuPathScope() {
        g_menuLoopDepth.fetch_add(1);
        if (g_menuIdleEvent) ResetEvent(g_menuIdleEvent);
    }
    // SIGNAL LAST: the idle event promises "no mod code is running in the
    // menu path", so it is set only as the scope's frame is leaving.
    ~MenuPathScope() {
        if (g_menuLoopDepth.fetch_sub(1) == 1 && g_menuIdleEvent)
            SetEvent(g_menuIdleEvent);
    }
    MenuPathScope(MenuPathScope const&) = delete;
    MenuPathScope& operator=(MenuPathScope const&) = delete;
};

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
static void ClearButtonEventState();
static void StartRetryThread();
static void StopRetryThread();

// ============================================================
// GetTaskbarXamlRoot
// ============================================================


static XamlRoot GetTaskbarXamlRoot(HWND window) {
    return taskbar_xaml::GetTaskbarXamlRoot(window);
}

static bool RunFromWindowThread(HWND window, dispatch::ThreadProc callback, void* parameter) {
    return dispatch::RunFromWindowThread(window, callback, parameter,
        L"Windhawk_RunFromWindowThread_" WH_MOD_ID);
}

static HWND FindCurrentProcessTaskbarWnd() {
    return taskbar_window::FindCurrentProcessTaskbarWnd();
}

static FrameworkElement FindChildRecursive(FrameworkElement const& root,
    std::function<bool(FrameworkElement)> const& predicate, int maxDepth = 20) {
    // First descendant (root excluded) matching the predicate, depth-first.
    if (!root || maxDepth <= 0) return nullptr;
    int count = VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < count; i++) {
        auto child = VisualTreeHelper::GetChild(root, i).try_as<FrameworkElement>();
        if (!child) continue;
        if (predicate(child)) return child;
        if (auto found = FindChildRecursive(child, predicate, maxDepth - 1))
            return found;
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
    // File name from the item's parsing name, read only at the merged Desktop
    // root where duplicate detection needs to tell "the same file in two
    // Desktop directories" from "two different files that display the same".
    std::wstring fileName;
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

// The Shell's own converter, rather than a hand-rolled one. It matters for
// STRRET_OFFSET in particular: that form points at a string embedded in the
// item ID which is classically ANSI, not UTF-16, so reading it as LPCWSTR
// yields garbage and can run off the end of the PIDL looking for a
// terminator. StrRetToBufW handles all three forms and the free.
static std::wstring StrRetToString(STRRET& str, PCUITEMID_CHILD pidl) {
    WCHAR buffer[MAX_PATH * 2];
    buffer[0] = L'\0';
    if (FAILED(StrRetToBufW(&str, pidl, buffer, ARRAYSIZE(buffer))))
        return L"";
    return buffer;
}

// Small-icon metric at the TASKBAR's DPI, not the system one.
// GetSystemMetrics reports the primary monitor's value, but the menu is laid
// out on whichever monitor the taskbar is on — so on a mixed-DPI setup the
// plain metric makes every menu bitmap visibly too small or too large beside
// its text. GetSystemMetricsForDpi is Windows 10 1607+; resolve it
// dynamically so an older build simply keeps the old behaviour.
static int SmallIconMetricForTaskbar(int metric) {
    // Windows 11 only, so GetSystemMetricsForDpi is always present.
    HWND taskbar = taskbar_window::ResolveTaskbarWnd(g_taskbarWnd);
    UINT dpi = taskbar ? GetDpiForWindow(taskbar) : 0;
    return dpi ? GetSystemMetricsForDpi(metric, dpi) : GetSystemMetrics(metric);
}

static HBITMAP BitmapFromIcon(HICON icon) {
    if (!icon)
        return nullptr;

    int iconW = SmallIconMetricForTaskbar(SM_CXSMICON);
    int iconH = SmallIconMetricForTaskbar(SM_CYSMICON);
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

// True only for the Desktop namespace root, which is the one folder whose
// enumeration merges two real directories.
static bool IsDesktopNamespaceRoot(PCIDLIST_ABSOLUTE folderPidl) {
    if (!folderPidl)
        return false;
    PIDLIST_ABSOLUTE desktop = nullptr;
    if (FAILED(SHGetSpecialFolderLocation(nullptr, CSIDL_DESKTOP, &desktop)) ||
        !desktop)
        return false;
    bool same = ILIsEqual(desktop, folderPidl);
    CoTaskMemFree(desktop);
    return same;
}

static std::vector<ShellMenuItem> EnumerateShellFolder(PCIDLIST_ABSOLUTE folderPidl) {
    std::vector<ShellMenuItem> items;

    IShellFolder* folder = nullptr;
    if (!BindFolderFromPidl(folderPidl, &folder))
        return items;

    bool isDesktopRoot = IsDesktopNamespaceRoot(folderPidl);

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

            // Only the merged Desktop root needs the parsing name, and it
            // costs a second Shell round trip per item, so do not pay for it
            // in every folder.
            std::wstring fileName;
            if (isDesktopRoot) {
                STRRET parse{};
                if (SUCCEEDED(folder->GetDisplayNameOf(child, SHGDN_FORPARSING,
                                                       &parse))) {
                    fileName = FileNameFromPath(StrRetToString(parse, child));
                }
            }

            SFGAOF attrs = SFGAO_FOLDER | SFGAO_FILESYSTEM;
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
            item.fileName = std::move(fileName);
            item.pidl = abs;
            item.canExpand = (attrs & SFGAO_FOLDER) && (attrs & SFGAO_FILESYSTEM);
            items.push_back(std::move(item));
        }
        enumList->Release();
    }

    folder->Release();

    // fileName is the tie-break so the dedupe below is exact: items sharing a
    // display name are then ordered by file name, which puts the merged
    // duplicates next to each other even among several same-named entries.
    std::sort(items.begin(), items.end(), [](auto const& a, auto const& b) {
        if (a.canExpand != b.canExpand) return a.canExpand > b.canExpand;
        int byName = _wcsicmp(a.displayName.c_str(), b.displayName.c_str());
        if (byName != 0) return byName < 0;
        return _wcsicmp(a.fileName.c_str(), b.fileName.c_str()) < 0;
    });

    // The Desktop namespace ROOT merges the user's and the public Desktop
    // folders, so one shortcut is enumerated twice there. Nowhere else.
    //
    // Deduplicating by DISPLAY name would be wrong even there: with "hide
    // extensions for known file types" on — the Windows default — report.docx
    // and report.pdf both display as "report", and one would silently vanish
    // from the menu. The file name from the parsing name separates the two
    // cases exactly: the merged duplicates share a file name (in different
    // Desktop directories), while two different files do not.
    if (isDesktopRoot) {
        for (auto it = items.begin(); it != items.end();) {
            auto next = it + 1;
            bool duplicate =
                next != items.end() && !it->fileName.empty() &&
                _wcsicmp(it->fileName.c_str(), next->fileName.c_str()) == 0;
            if (duplicate) {
                if (next->pidl) CoTaskMemFree(next->pidl);
                items.erase(next);
            } else {
                ++it;
            }
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
                // The menu's actual owner, not the cached taskbar handle:
                // ShowFolderMenu resolved and validated that window, and if
                // Shell_TrayWnd was recreated since the last apply the cached
                // one is dead and the deferral would simply fail.
                if (PostMessageW(g_menuOwner ? g_menuOwner : g_taskbarWnd,
                                 WM_MENURBUTTONUP, itemPosition,
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
        //
        // The verb runs AFTER the folder popup's own scope has closed, and may
        // pump a modal loop of its own, so it opens a scope of its own: unload
        // then waits for the dialog to be answered rather than freeing the
        // image under InvokeCommand's return address.
        //
        // Open the scope BEFORE reading g_unloading. Wh_ModUninit stores the
        // flag and then reads the depth; this reads them in the opposite
        // order, so one side always sees the other.
        RemoveWindowSubclass(hwnd, MenuOwnerSubclassProc, 1);
        MenuPathScope scope;
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
    // Menu capture makes re-entry hard to reach, but the FreeMenuState() below
    // would free PIDLs an already-open menu is still using. The depth counter
    // is right here, so make it impossible rather than unlikely.
    if (g_menuLoopDepth.load() > 0) {
        Wh_Log(L"[Menu] A folder menu is already open; ignoring");
        return;
    }
    if (g_unloading) {
        return;
    }

    HWND owner = taskbar_window::ResolveTaskbarWnd(g_taskbarWnd);
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
    g_menuOwner = owner;
    g_menuMsgFilterHook = SetWindowsHookExW(
        WH_MSGFILTER, FolderMenuMsgFilterProc,
        nullptr, GetCurrentThreadId());
    if (!g_menuMsgFilterHook) {
        Wh_Log(L"[Menu] WH_MSGFILTER hook failed error=%u", GetLastError());
    }
    // Held to the end of this function, not just across TrackPopupMenu: the
    // tail below (InvokePidl, cleanup) is mod code on this stack too.
    MenuPathScope menuPath;
    // Re-check AFTER the scope is open. The check at the top can pass just
    // before Wh_ModUninit sets the flag and reads a depth of zero; the menu
    // loop below would then run the uninit cleanup inside itself and return
    // into a freed image. Uninit stores the flag before reading the depth,
    // so from here on one side always sees the other.
    UINT cmd = 0;
    if (!g_unloading)
        cmd = TrackPopupMenu(menu, tpmAlign, pt.x, pt.y, 0, owner, nullptr);
    PostMessageW(owner, WM_NULL, 0, 0);
    if (g_menuMsgFilterHook) {
        UnhookWindowsHookEx(g_menuMsgFilterHook);
        g_menuMsgFilterHook = nullptr;
    }
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

    g_menuOwner = nullptr;
    // menuPath closes here, signalling idle only once this tail is done.
}

// ============================================================
// Button grid
// ============================================================

// Cache only pixels, never apartment-bound XAML objects. Shell extraction runs
// on Windhawk's settings thread; UI rebuilds only read already-cached icons.
struct FolderIconPixels {
    std::wstring target;
    int requested = 0;
    int width = 0;
    int height = 0;
    std::vector<BYTE> pixels;
};
static std::vector<FolderIconPixels> g_folderIcons; // exit-time-safe: heap-only
// No no_destroy: std::mutex has a trivial destructor here, so the attribute
// would suppress nothing and only invite copying it onto types where it does
// matter.
static std::mutex g_folderIconsMutex;  // exit-time-safe: heap-only

static int FolderIconSize() {
    HWND taskbar = taskbar_window::ResolveTaskbarWnd(g_taskbarWnd);
    UINT dpi = taskbar ? GetDpiForWindow(taskbar) : 96;
    return std::clamp(MulDiv(std::max(1, std::min(g_settings.buttonWidth,
        g_settings.buttonHeight) - 4), dpi ? dpi : 96, 96), 8, 256);
}

static FolderIconPixels const* CacheFolderIcon(std::wstring const& target, int size) {
    for (auto const& cached : g_folderIcons)
        if (cached.target == target && cached.requested == size) return &cached;
    auto pidl = ParseShellTarget(target);
    if (!pidl) return nullptr;
    winrt::com_ptr<IShellItemImageFactory> factory;
    HRESULT hr = SHCreateItemFromIDList(pidl, IID_PPV_ARGS(factory.put()));
    CoTaskMemFree(pidl);
    if (FAILED(hr)) return nullptr;
    HBITMAP bitmap = nullptr;
    hr = factory->GetImage({size, size}, SIIGBF_ICONONLY, &bitmap);
    if (FAILED(hr) || !bitmap) return nullptr;
    BITMAP details{};
    if (!GetObjectW(bitmap, sizeof(details), &details) || details.bmWidth <= 0 ||
        details.bmHeight <= 0 || details.bmWidth > 256 || details.bmHeight > 256) {
        DeleteObject(bitmap);
        return nullptr;
    }
    FolderIconPixels cached{target, size, details.bmWidth, details.bmHeight, {}};
    try { cached.pixels.resize(cached.width * cached.height * 4); }
    catch (...) { DeleteObject(bitmap); throw; }
    BITMAPINFO info{};
    info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    info.bmiHeader.biWidth = cached.width;
    info.bmiHeader.biHeight = -cached.height;
    info.bmiHeader.biPlanes = 1;
    info.bmiHeader.biBitCount = 32;
    info.bmiHeader.biCompression = BI_RGB;
    HDC dc = GetDC(nullptr);
    int rows = dc ? GetDIBits(dc, bitmap, 0, cached.height, cached.pixels.data(),
                              &info, DIB_RGB_COLORS) : 0;
    if (dc) ReleaseDC(nullptr, dc);
    DeleteObject(bitmap);
    if (rows != cached.height) return nullptr;
    if (g_folderIcons.size() >= 128) g_folderIcons.erase(g_folderIcons.begin());
    g_folderIcons.push_back(std::move(cached));
    return &g_folderIcons.back();
}

static void PrepareFolderIcons() {
    std::lock_guard lock(g_folderIconsMutex);
    HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    try {
        int size = FolderIconSize();
        for (auto const& entry : g_settings.folders) {
            // Each of these is a Shell round trip that can block for as long
            // as an unreachable network target takes. Unload waits on this
            // worker, so check between entries instead of making the user
            // wait out the whole list.
            if (g_unloading)
                break;
            if (entry.useDefaultIcon) CacheFolderIcon(entry.target, size);
        }
    } catch (...) { Wh_Log(L"[Icons] Shell icon extraction failed; using labels"); }
    if (SUCCEEDED(hr)) CoUninitialize();
}

static Image NativeFolderIcon(FolderEntry const& entry) {
    std::lock_guard lock(g_folderIconsMutex);
    FolderIconPixels const* cached = nullptr;
    int size = FolderIconSize();
    for (auto const& item : g_folderIcons) {
        if (item.target != entry.target) continue;
        if (!cached || abs(item.requested - size) < abs(cached->requested - size)) cached = &item;
    }
    if (!cached) return nullptr;
    winrt::Windows::UI::Xaml::Media::Imaging::WriteableBitmap bitmap(
        cached->width, cached->height);
    auto access = bitmap.PixelBuffer().as<::Windows::Storage::Streams::IBufferByteAccess>();
    BYTE* bytes = nullptr;
    winrt::check_hresult(access->Buffer(&bytes));
    memcpy(bytes, cached->pixels.data(), cached->pixels.size());
    bitmap.Invalidate();
    Image image;
    image.Source(bitmap);
    image.Width(std::max(1, std::min(g_settings.buttonWidth, g_settings.buttonHeight) - 4));
    image.Height(image.Width());
    image.Stretch(Stretch::Uniform);
    return image;
}

static Grid BuildFolderButtonGrid(double trayHeight) {
    int count = (int)g_settings.folders.size();
    int maxRows = std::max(1, static_cast<int>(
        (trayHeight - 2 * g_settings.padY + g_settings.buttonSpacing) /
        (g_settings.buttonHeight + g_settings.buttonSpacing)));
    auto arrangement = ngl::ResolveArrangement(g_settings.arrangement, count,
        maxRows, g_settings.layoutFill);
    ngl::Config config{static_cast<double>(g_settings.buttonSpacing),
        g_settings.justify, static_cast<double>(g_settings.padX),
        static_cast<double>(g_settings.padY)};
    auto indexOf = [count](std::wstring const& token) {
        size_t first = 0;
        if (token.size() >= 6 && _wcsnicmp(token.c_str(), L"folder", 6) == 0)
            first = 6;
        if (first == token.size()) return -1;
        int number = 0;
        for (; first < token.size(); ++first) {
            if (token[first] < L'0' || token[first] > L'9') return -1;
            if (number > count / 10) return -1;
            number = number * 10 + token[first] - L'0';
            if (number > count) return -1;
        }
        return number > 0 ? number - 1 : -1;
    };
    auto resolve = [&](std::wstring const& token) -> ngl::Size {
        if (indexOf(token) < 0 && !ngl::TokenIs(token, L"pad")) return {};
        return {static_cast<double>(g_settings.buttonWidth),
                static_cast<double>(g_settings.buttonHeight)};
    };
    std::vector<ngl::Placement> placements;
    ngl::Size total;
    ngl::ParseError error;
    if (!ngl::Compute(arrangement.expression, config, resolve, placements, total, &error)) {
        Wh_Log(L"[Layout] Invalid arrangement at %zu: %s; using auto",
               error.position, error.expected.c_str());
        arrangement = ngl::ResolveArrangement(L"auto", count, maxRows, g_settings.layoutFill);
        ngl::Compute(arrangement.expression, config, resolve, placements, total);
    }
    if (g_settings.appendNewItems) {
        std::vector<std::wstring> expected;
        for (int i = 0; i < count; ++i) expected.push_back(std::to_wstring(i + 1));
        auto missing = ngl::MissingTokens(expected, placements,
            [&](auto const& a, auto const& b) { return indexOf(a) == indexOf(b); });
        arrangement.expression = ngl::AppendMissing(arrangement.expression,
            missing, maxRows, g_settings.layoutFill);
        ngl::Compute(arrangement.expression, config, resolve, placements, total);
    }
    Wh_Log(L"[Layout] %s => %.1fx%.1f DIP", arrangement.expression.c_str(), total.width, total.height);

    Grid grid;
    grid.Name(L"TaskbarFolderMenuBar");
    grid.VerticalAlignment(VerticalAlignment::Center);
    grid.Width(total.width);
    grid.Height(total.height);
    if (g_settings.groupOffsetX || g_settings.groupOffsetY) {
        TranslateTransform transform;
        transform.X((double)g_settings.groupOffsetX);
        transform.Y((double)g_settings.groupOffsetY);
        grid.RenderTransform(transform);
    }
    bs::Colors colors{g_settings.textColor, g_settings.backgroundColor,
        g_settings.hoverBackgroundColor, g_settings.pressedBackgroundColor,
        g_settings.borderColor};
    bs::Options surface{g_settings.opacityPct, g_settings.borderThickness,
        g_settings.cornerRadius, g_settings.shineEffect};
    std::vector<bool> shown(count, false);
    for (auto const& placement : placements) {
        int i = indexOf(placement.token);
        if (i < 0 || shown[i]) continue;
        shown[i] = true;
        auto entry = g_settings.folders[i];
        std::wstring caption = entry.label;
        if (caption.empty())
            caption = g_settings.buttonText.empty() ? L"📁" : g_settings.buttonText;

        Button btn;
        btn.Name(L"FolderMenuButton_" + std::to_wstring(i));
        winrt::Windows::UI::Xaml::Automation::AutomationProperties::SetName(btn, caption);
        winrt::Windows::UI::Xaml::Automation::AutomationProperties::SetHelpText(btn, entry.target);
        btn.Content(winrt::box_value(winrt::hstring(caption)));
        if (entry.useDefaultIcon) {
            try { if (auto icon = NativeFolderIcon(entry)) btn.Content(icon); }
            catch (...) { Wh_Log(L"[Icons] Using label for %s", entry.target.c_str()); }
        }
        btn.Width((double)g_settings.buttonWidth);
        btn.Height((double)g_settings.buttonHeight);
        btn.Padding({ 0.0, 0.0, 0.0, 1.0 });
        btn.FontSize((double)g_settings.fontSize);
        btn.FontWeight(winrt::Windows::UI::Text::FontWeights::SemiBold());
        btn.HorizontalAlignment(HorizontalAlignment::Left);
        btn.VerticalAlignment(VerticalAlignment::Top);
        btn.Margin({placement.x, placement.y, 0.0, 0.0});
        bs::Apply(btn, colors, surface);
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

        grid.Children().Append(btn);
    }

    return grid;
}

// ============================================================
// Injection / cleanup
// ============================================================

struct AppIconPlacement {
    Grid root{nullptr};
    Grid group{nullptr};
    FrameworkElement repeater{nullptr};
    FrameworkElement tray{nullptr};
    Thickness originalMargin{};
    Thickness appliedMargin{};
    winrt::event_token layoutToken{};
};
// no_destroy optional rather than a bare no_destroy aggregate: the members are
// strong XAML references, so the release has to be an explicit reset() on the
// UI thread rather than whatever the CRT would do at detach.
[[clang::no_destroy]] static std::optional<AppIconPlacement> g_appPlacement{
    std::in_place};

static bool PositionAfterAppIcons() noexcept {
    if (!g_appPlacement) return false;
    auto& p = *g_appPlacement;
    if (!p.root || !p.group || !p.repeater || !p.tray) return false;
    try {
        double right = 0;
        bool found = false;
        int count = VisualTreeHelper::GetChildrenCount(p.repeater);
        for (int i = 0; i < count; ++i) {
            auto child = VisualTreeHelper::GetChild(p.repeater, i).try_as<FrameworkElement>();
            // Virtualized/recycled elements can exist with no rendered box.
            if (!child || child.Visibility() != Visibility::Visible ||
                child.ActualWidth() <= 0 || child.ActualHeight() <= 0) continue;
            auto bounds = child.TransformToVisual(p.root).TransformBounds(
                {0, 0, static_cast<float>(child.ActualWidth()), static_cast<float>(child.ActualHeight())});
            right = std::max(right, static_cast<double>(bounds.X + bounds.Width));
            found = true;
        }
        auto trayPoint = p.tray.TransformToVisual(p.root).TransformPoint({0, 0});
        double limit = std::min(p.root.ActualWidth(), static_cast<double>(trayPoint.X));
        double width = p.group.Width();
        double x = right + g_settings.buttonSpacing;
        // Reserve space through the repeater margin, and fail closed if the
        // host cannot make enough room (e.g. an exceptionally narrow taskbar).
        bool fits = found && width > 0 && x + width <= limit && x >= 0;
        auto visible = fits ? Visibility::Visible : Visibility::Collapsed;
        if (p.group.Visibility() != visible) p.group.Visibility(visible);
        if (!fits) return true;
        x = std::clamp(x + g_settings.groupOffsetX, right, std::max(right, limit - width));
        double y = std::max(0.0, (p.root.ActualHeight() - p.group.Height()) / 2);
        Thickness margin{x, y + g_settings.groupOffsetY, 0, 0};
        auto old = p.group.Margin();
        if (old.Left != margin.Left || old.Top != margin.Top) p.group.Margin(margin);
        return true;
    } catch (...) { return false; }
}

static void ReleaseAppIconPlacement() {
    if (!g_appPlacement) return;
    auto& p = *g_appPlacement;
    if (!p.root) return;
    try { p.root.LayoutUpdated(p.layoutToken); } catch (...) {}
    try {
        if (p.repeater) {
            auto current = p.repeater.Margin();
            // Restore only our contribution if another mod changed this margin.
            current.Right -= p.appliedMargin.Right - p.originalMargin.Right;
            p.repeater.Margin(current);
        }
        uint32_t index;
        if (p.group && p.root.Children().IndexOf(p.group, index))
            p.root.Children().RemoveAt(index);
    } catch (...) {}
    p = {};
}

static bool InjectAfterAppIcons(FrameworkElement root, double trayHeight) {
    auto frame = FindChildRecursive(root, [](FrameworkElement const& item) {
        return winrt::get_class_name(item) == L"Taskbar.TaskbarFrame";
    });
    if (!frame) return false;
    auto rootGrid = FindChildRecursive(frame, [](FrameworkElement const& item) {
        return item.Name() == L"RootGrid";
    }).try_as<Grid>();
    if (!rootGrid) return false;
    if (g_appPlacement && g_appPlacement->root == rootGrid &&
        g_appPlacement->group)
        return PositionAfterAppIcons();
    ReleaseAppIconPlacement();
    ClearButtonEventState();
    auto repeater = FindChildRecursive(rootGrid, [](FrameworkElement const& item) {
        return item.Name() == L"TaskbarFrameRepeater";
    });
    auto tray = FindChildRecursive(root, [](FrameworkElement const& item) {
        return item.Name() == L"SystemTrayFrameGrid";
    });
    if (!repeater || !tray) return false;
    auto group = BuildFolderButtonGrid(trayHeight);
    group.ClearValue(UIElement::RenderTransformProperty());
    group.HorizontalAlignment(HorizontalAlignment::Left);
    group.VerticalAlignment(VerticalAlignment::Top);
    Grid::SetColumnSpan(group, std::max(1, static_cast<int>(rootGrid.ColumnDefinitions().Size())));
    auto original = repeater.Margin();
    auto applied = original;
    applied.Right += group.Width() + g_settings.buttonSpacing;
    g_appPlacement.emplace(AppIconPlacement{rootGrid, group, repeater, tray,
                                            original, applied, {}});
    try {
        repeater.Margin(applied);
        rootGrid.Children().Append(group);
        g_appPlacement->layoutToken = rootGrid.LayoutUpdated([](auto const&, auto const&) {
            if (!g_unloading && !g_updatingSettings) PositionAfterAppIcons();
        });
        if (!PositionAfterAppIcons()) { ReleaseAppIconPlacement(); return false; }
    } catch (...) { ReleaseAppIconPlacement(); throw; }
    g_buttonGrid = group;
    return true;
}

static bool RemoveButtonGridFrom(Panel gridParent) {
    if (!gridParent) return false;

    bool removed = false;
    for (uint32_t i = 0; i < gridParent.Children().Size(); i++) {
        auto fe = gridParent.Children().GetAt(i).try_as<FrameworkElement>();
        if (fe && fe.Name() == L"TaskbarFolderMenuBar") {
            gridParent.Children().RemoveAt(i);
            removed = true;
            break;
        }
    }

    igc::Release(gridParent, g_columnLease);

    return removed;
}

static Grid FindButtonGridInParent(Panel gridParent) {
    if (!gridParent) return nullptr;
    for (auto child : gridParent.Children()) {
        auto fe = child.try_as<FrameworkElement>();
        if (fe && fe.Name() == L"TaskbarFolderMenuBar")
            return fe.try_as<Grid>();
    }
    return nullptr;
}

// Column collisions are a Grid-only failure mode: two children assigned the
// same Grid.Column. A StackPanel has no columns, so there is nothing to collide
// and nothing to repair -- both checks stand down on the newer tray panel.
static bool HasButtonGridColumnCollision(Panel gridParent, Grid buttonGrid) {
    if (!gridParent || !buttonGrid ||
        igc::Classify(gridParent) != igc::Kind::Columns)
        return false;
    int buttonCol = Grid::GetColumn(buttonGrid);
    for (auto child : gridParent.Children()) {
        auto fe = child.try_as<FrameworkElement>();
        if (fe && fe != buttonGrid && fe.Name() != L"FolderMenuColumnLease" &&
            Grid::GetColumn(fe) == buttonCol)
            return true;
    }
    return false;
}

static int RepairButtonGridColumnCollision(Panel gridParent, Grid buttonGrid) {
    if (!gridParent || !buttonGrid ||
        igc::Classify(gridParent) != igc::Kind::Columns)
        return 0;
    int buttonCol = Grid::GetColumn(buttonGrid);
    int moved = 0;
    for (auto child : gridParent.Children()) {
        auto fe = child.try_as<FrameworkElement>();
        if (!fe || fe == buttonGrid || fe.Name() == L"FolderMenuColumnLease")
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
    ReleaseAppIconPlacement();
    auto gridParent = FindLiveSystemTrayFrameGrid().try_as<Panel>();
    if (!RemoveButtonGridFrom(gridParent))
        Wh_Log(L"[Remove] TaskbarFolderMenuBar not found");

    g_buttonGrid = nullptr;
    g_injectedSlot = -1;
}

static bool InjectButtonGrid(FrameworkElement root) {
    auto parent = FindChildRecursive(root, [](FrameworkElement fe) {
        return fe.Name() == L"SystemTrayFrameGrid";
    });
    if (!parent) {
        Wh_Log(L"[Inject] SystemTrayFrameGrid not found");
        return false;
    }

    // Windows 11 26200.9457 (KB5129195) kept the name SystemTrayFrameGrid but
    // changed the element from a Grid to a StackPanel. Accept either; refuse to
    // guess the layout semantics of anything else.
    auto gridParent = parent.try_as<Panel>();
    igc::Kind kind = igc::Classify(parent);
    if (!gridParent || kind == igc::Kind::Unsupported) {
        Wh_Log(L"[Inject] Unsupported SystemTrayFrameGrid type: %s",
               igc::ClassName(parent).c_str());
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
            g_injectedSlot = kind == igc::Kind::Columns
                                 ? Grid::GetColumn(fe)
                                 : igc::IndexOfChild(gridParent, fe);
            return true;
        }
    }

    igc::Anchor anchor = igc::Anchor::BeforeIcons;
    switch (g_settings.position) {
        case Position::BeforeIcons:
            // No named reference is needed: the notification icon stack
            // starts at the first native slot.
            break;
        case Position::BeforeOmni:       anchor = igc::Anchor::BeforeOmni; break;
        case Position::BeforeClock:      anchor = igc::Anchor::BeforeClock; break;
        case Position::AfterClock:       anchor = igc::Anchor::AfterClock; break;
        case Position::AfterShowDesktop: anchor = igc::Anchor::AfterShowDesktop; break;
        case Position::AfterTaskbarIcons:
            // Placed by InjectAfterAppIcons, never through the tray lease.
            return false;
    }

    // ResolveSlot yields a column on a Grid and a child index on a StackPanel.
    int insertSlot = 0;
    if (!igc::ResolveSlot(gridParent, anchor, insertSlot)) {
        Wh_Log(L"[Inject] Anchor for position '%s' is not ready; tray %s holds: %s",
               PositionName(g_settings.position), igc::ClassName(parent).c_str(),
               igc::DescribeChildren(gridParent).c_str());
        return false;
    }

    // Build before reserving the slot, so a content failure cannot leave an
    // empty synthetic column behind.
    auto grid = BuildFolderButtonGrid(trayHeight);
    if (!igc::AcquireAt(gridParent, insertSlot, L"FolderMenuColumnLease", g_columnLease))
        return false;
    bool placed = false;
    try { placed = igc::PlaceChild(gridParent, g_columnLease, grid); }
    catch (...) { igc::Release(gridParent, g_columnLease); throw; }
    if (!placed) {
        igc::Release(gridParent, g_columnLease);
        Wh_Log(L"[Inject] Could not place TaskbarFolderMenuBar in the lease");
        return false;
    }

    g_buttonGrid = grid;
    g_injectedSlot = g_columnLease.slot;

    Wh_Log(L"[Inject] TaskbarFolderMenuBar at %s=%d, folders=%d",
           kind == igc::Kind::Columns ? L"column" : L"index",
           g_columnLease.slot, (int)g_settings.folders.size());
    return true;
}

static void ApplyAllSettings() {
    if (g_updatingSettings || g_unloading) return;
    g_injectionLive.store(false);
    HWND hWnd = FindCurrentProcessTaskbarWnd();
    if (!hWnd) {
        Wh_Log(L"[Apply] No taskbar window");
        return;
    }
    g_taskbarWnd = hWnd;

    try {
        RECT taskbarRect{};
        if (GetWindowRect(hWnd, &taskbarRect) &&
            taskbarRect.bottom - taskbarRect.top > taskbarRect.right - taskbarRect.left) {
            RemoveButtonGrid();
            Wh_Log(L"[Apply] Vertical taskbar unsupported; standing down");
            // Settled, not failed: retrying cannot change the orientation. A
            // taskbar rebuild or settings change re-evaluates.
            g_injectionLive.store(true);
            return;
        }
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

        auto trayFrame = FindChildRecursive(root, [](FrameworkElement fe) {
            return fe.Name() == L"SystemTrayFrameGrid";
        });
        if (!trayFrame) {
            Wh_Log(L"[Apply] SystemTrayFrameGrid unavailable");
            return;
        }
        // Grid on older builds, StackPanel since 26200.9457 (KB5129195).
        auto gridParent = trayFrame.try_as<Panel>();
        if (!gridParent || igc::Classify(trayFrame) == igc::Kind::Unsupported) {
            Wh_Log(L"[Apply] Unsupported SystemTrayFrameGrid type: %s",
                   igc::ClassName(trayFrame).c_str());
            return;
        }

        if (g_settings.position == Position::AfterTaskbarIcons) {
            g_injectionLive.store(InjectAfterAppIcons(root, gridParent.ActualHeight()));
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
            g_injectedSlot = -1;
        }

        if (rebuild) {
            ClearButtonEventState();
            RemoveButtonGridFrom(gridParent);
            g_buttonGrid = nullptr;
            g_injectedSlot = -1;
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


static bool HookTaskbarDllSymbols() {
    return taskbar_xaml::HookTaskbarSymbols([] {
        if (!g_unloading && !g_updatingSettings) StartRetryThread();
    });
}


// Stopped from Wh_ModUninit and Wh_ModSettingsChanged on Windhawk's thread
// while a taskbar rebuild can start it from the taskbar thread; RetryLoop makes
// every caller that observes a live run wait for it, so neither can return
// while the worker is still running mod code.
[[clang::no_destroy]] static retry_loop::RetryLoop g_retry;

static void StopRetryThread() {
    g_retry.Stop();
}

// Every build goes through this worker, so the first attempt (no delay)
// prepares Shell icons BEFORE the first build: extraction runs here, never in
// Wh_ModInit or a XAML callback, and at the live taskbar DPI. 40 attempts
// 1.5 s apart cover a slow sign-in for about a minute.
static void StartRetryThread() {
    if (g_unloading || g_updatingSettings) return;
    // A non-null cached Grid isn't proof that it still belongs to the current
    // tray. StartTaskbar can recreate/reindex the XAML tree after resume.
    g_injectionLive.store(false);
    g_retry.Start(
        [] {
            PrepareFolderIcons();
            ApplyAllSettingsOnWindowThread();
        },
        [] { return g_injectionLive.load(); }, g_unloading, 40, 1500);
}

// ============================================================
// Windhawk lifecycle
// ============================================================

// Exceptions thrown inside a UI callback are caught where they happen, which
// is the only place they can be caught — but without this they are swallowed
// with no trace at all.
static void LogUiCallbackFailure(PCWSTR context) {
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

BOOL Wh_ModInit() {
    Wh_Log(L"[Init] Taskbar Folder Menus v2.0");
    LoadSettings();
    dispatch::SetExceptionLogger(LogUiCallbackFailure);
    g_menuIdleEvent = CreateEventW(nullptr, TRUE, TRUE, nullptr);
    if (!g_menuIdleEvent) {
        Wh_Log(L"[Init] Failed to create menu-idle event");
        return FALSE;
    }

    if (!HookTaskbarDllSymbols()) {
        Wh_Log(L"[Init] taskbar.dll hooks failed - XamlRoot unavailable");
        return FALSE;
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    // No direct apply here. A build before PrepareFolderIcons would inject
    // label-only buttons that no later pass replaces (an existing grid is
    // kept as is), so native icons would never appear until Explorer
    // restarted. The worker's first attempt prepares, then builds.
    StartRetryThread();
}

void Wh_ModUninit() {
    g_unloading = true;
    Wh_Log(L"[Uninit]");

    StopRetryThread();

    HWND hWnd = taskbar_window::ResolveTaskbarWnd(g_taskbarWnd);
    if (hWnd) {
        // EndMenu only REQUESTS that the active menu unwind, and with a nested
        // Shell context menu up it ends that one rather than the folder popup
        // underneath. So ask repeatedly, and add WM_CANCELMODE, until the menu
        // path reports itself idle.
        //
        // NO CAP. While the menu path is open the taskbar thread has this
        // image on its stack — inside TrackPopupMenu, a ShellExecuteEx to a
        // slow network target, or a queued Shell verb's modal dialog — and
        // returning from here would free the code it will return into. Each
        // RunFromWindowThread is a SendMessage that completes only when that
        // thread pumps, so this loop follows the thread's real progress and
        // ends when the user dismisses whatever is open.
        bool loggedWait = false;
        while (g_menuLoopDepth.load() > 0) {
            if (!loggedWait) {
                loggedWait = true;
                Wh_Log(L"[Uninit] Waiting for the open folder menu or Shell "
                       L"command to finish");
            }
            RunFromWindowThread(hWnd, [](void* parameter) {
                HWND owner = static_cast<HWND>(parameter);
                EndMenu();
                if (owner)
                    SendMessageW(owner, WM_CANCELMODE, 0, 0);
            }, hWnd);
            WaitForSingleObject(g_menuIdleEvent, 100);
        }

        // The menu path is idle, so nothing still points into this image
        // through the hook or the subclass; remove both on their thread.
        RunFromWindowThread(hWnd, [](void* parameter) {
            HWND owner = static_cast<HWND>(parameter);
            if (g_menuMsgFilterHook) {
                UnhookWindowsHookEx(g_menuMsgFilterHook);
                g_menuMsgFilterHook = nullptr;
            }
            RemoveWindowSubclass(owner, MenuOwnerSubclassProc, 1);
            if (g_menuOwner && g_menuOwner != owner)
                RemoveWindowSubclass(g_menuOwner, MenuOwnerSubclassProc, 1);
            ClearPendingShellCommand();
            RemoveButtonGrid();
            g_buttonEventStates.reset();
            g_appPlacement.reset();
        }, hWnd);

        // Safe to close: the menu path is idle and g_unloading stops a new
        // one from opening, so nothing can SetEvent this handle again.
        CloseHandle(g_menuIdleEvent);
        g_menuIdleEvent = nullptr;
    } else {
        // No known taskbar UI thread: retain no_destroy XAML state rather than
        // releasing it from Windhawk's callback thread after framework teardown.
        Wh_Log(L"[Uninit] No taskbar UI thread; retaining XAML state");
        if (g_menuIdleEvent && g_menuLoopDepth.load() == 0) {
            CloseHandle(g_menuIdleEvent);
            g_menuIdleEvent = nullptr;
        }
    }
}

void Wh_ModSettingsChanged() {
    g_updatingSettings = true;
    StopRetryThread();
    HWND hWnd = FindCurrentProcessTaskbarWnd();
    if (hWnd && !RunFromWindowThread(hWnd, [](void*) { RemoveButtonGrid(); }, nullptr)) {
        g_updatingSettings = false;
        Wh_Log(L"[Settings] Could not detach old UI; reload to apply settings");
        return;
    }
    LoadSettings();
    g_updatingSettings = false;
    Wh_Log(L"[Settings] Changed");
    if (!hWnd) {
        return;
    }
    g_taskbarWnd = hWnd;
    // Rebuilt by the worker, which prepares icons first (see Wh_ModAfterInit).
    StartRetryThread();
}
