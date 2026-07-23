// ==WindhawkMod==
// @id              tray-utility-customizer
// @name            Tray Utility Customizer
// @description     Granular per-icon control over the Windows tray utility icons — Show hidden icons, Emoji, touch keyboard, pen menu, virtual touchpad, and input/language indicator — arranged by one nestable layout expression.
// @version         1.1
// @author          sb4ssman
// @github          https://github.com/sb4ssman
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Tray Utility Customizer

Granular, predictable control over the low-frequency Windows 11 system-tray
utility icons:

- **Show hidden icons** (the overflow chevron) — token `overflow`
- **Emoji and more** — token `emoji`
- **Touch keyboard** — token `touchKeyboard`
- **Pen menu** — token `penMenu`
- **Virtual touchpad** — token `virtualTouchpad`
- **Input/language indicator** — token `inputIndicator`

The native controls stay alive and Windows-owned — clicks, flyouts, and
tooltips are untouched. The mod gathers their hosts into one owned group and
positions each icon individually, at its native size by default.

![Native tray before the mod](https://raw.githubusercontent.com/sb4ssman/Windhawk-Mod-Lab/main/tray-utility-customizer/assets/disabled.png)
*Mod disabled: the chevron, Emoji, and touch keyboard sit in their native positions.*

![One row at the hidden-icons position](https://raw.githubusercontent.com/sb4ssman/Windhawk-Mod-Lab/main/tray-utility-customizer/assets/inline-overflow-emoji-touchkeyboard.png)
*The default `overflow | emoji | touchKeyboard`: one row, native sizes.*

![Chevron centered above a row of utilities](https://raw.githubusercontent.com/sb4ssman/Windhawk-Mod-Lab/main/tray-utility-customizer/assets/overflow-over-utility-row.png)
*`overflow, (emoji | touchKeyboard)`: the chevron centered on its own row above the pair.*

![Chevron above the utility stack](https://raw.githubusercontent.com/sb4ssman/Windhawk-Mod-Lab/main/tray-utility-customizer/assets/overflow-utility-stack.png)
*The chevron leading a stacked pair.*

![Utility stack above the chevron](https://raw.githubusercontent.com/sb4ssman/Windhawk-Mod-Lab/main/tray-utility-customizer/assets/utility-stack-overflow.png)
*The same stack with the chevron last instead.*

![A neat column](https://raw.githubusercontent.com/sb4ssman/Windhawk-Mod-Lab/main/tray-utility-customizer/assets/neat-stack.png)
*A full single column of utilities on a single-height taskbar.*

![A dedicated column elsewhere in the tray](https://raw.githubusercontent.com/sb4ssman/Windhawk-Mod-Lab/main/tray-utility-customizer/assets/dedicated-tray-column.png)
*The group leased into its own tray column at the right end of the taskbar.*

![On a busy double-height taskbar](https://raw.githubusercontent.com/sb4ssman/Windhawk-Mod-Lab/main/tray-utility-customizer/assets/busy-tray.png)
*Coexisting with a heavily modded double-height tray.*

![Beside Start](https://raw.githubusercontent.com/sb4ssman/Windhawk-Mod-Lab/main/tray-utility-customizer/assets/unnecessary-but-possible.png)
*The experimental Right of Start position — unnecessary, but possible.*

![Right of Start, stacked on a double-height taskbar](https://raw.githubusercontent.com/sb4ssman/Windhawk-Mod-Lab/main/tray-utility-customizer/assets/right-of-start-2x-taskmanager-height.png)
*Right of Start on a double-height taskbar, stacked as a column beside Start.*

## Layout expression

One string describes the whole arrangement:

- `|` places groups side by side along the **primary axis**
- `,` stacks items along the crossed axis
- parentheses nest, alternating axes
- every group is centered (or start/end-aligned) against its siblings

Examples with the primary axis set to Row:

- `overflow | emoji | touchKeyboard` — one row of three icons
- `overflow | emoji, touchKeyboard` — chevron centered beside a stacked pair
- `overflow | emoji, touchKeyboard | penMenu` — the diamond: two centered
  icons flanking a stacked middle column

Set the primary axis to Column and the same strings arrange top-to-bottom
instead. Icons omitted from the expression, and icons Windows currently
hides, simply don't participate; anything visible that you didn't place is
appended after the group so nothing is ever lost. The group re-adapts
automatically as icons appear and disappear (for example the transient touch
keyboard).

Tokens accept forgiving aliases: `chevron`/`hidden` for `overflow`,
`keyboard` for `touchKeyboard`, `pen` for `penMenu`, `touchpad` for
`virtualTouchpad`, and `input`/`language` for `inputIndicator`.

Icons render at their native size unless you set explicit button sizes, and
each icon has fine X/Y nudge settings. A tall column of native-size icons can
overhang a single-height taskbar; set the icon width/height to about 16 px if
you want it to fit.

## Position

The group can sit in the hidden-icons or Emoji column, or lease a dedicated
tray column before the notification icons, before Wi-Fi/volume/battery,
before or after the clock, or after the Show Desktop strip. The lease is
marker-tracked and fully reversible on unload.

Two **experimental** positions relocate the group out of the tray entirely:
**Left of Start** and **Right of Start** place it beside the Start button and
push the task list right to reserve room. The group follows Start as the
taskbar re-centers. Primary taskbar only.

## Detection

Icons are identified by Windows' language-neutral runtime data-model classes,
XAML names and content types, Automation IDs, and stable Segoe Fluent glyphs.
Detection doesn't depend on translated accessibility labels. **Force
MainStack** allows the complete native `MainStack` to participate as the
`emoji` item when Windows doesn't expose a distinct identity.

## Known limitations

- The utility flyouts (the Emoji panel, the hidden-icons overflow) are
  positioned by Windows itself from the icon's location; at extreme
  screen-edge positions they can open partially off-screen. Prefer the
  tray positions if this bothers you.
- Left/Right of Start are experimental. The centered taskbar re-flows with
  an animation, and the group can briefly sit at a stale position until
  the taskbar's next layout pass settles it.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- position: overflow
  $name: Position
  $description: >-
    Where the utility group lives: a native utility column, a dedicated
    leased tray column, or experimentally beside Start.
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

- layout: "overflow | emoji | touchKeyboard"
  $name: Layout expression
  $description: >-
    "|" places groups along the primary axis, "," stacks across it, and
    parentheses nest (axes alternate). Tokens: overflow, emoji,
    touchKeyboard, penMenu, virtualTouchpad, inputIndicator (aliases:
    chevron, keyboard, pen, touchpad, input). Example diamond:
    "overflow | emoji, touchKeyboard | penMenu". A single column:
    "overflow, emoji, touchKeyboard" with the primary axis on Row. Icons
    left out don't participate; visible unplaced icons are appended after
    the group.

- primaryAxis: row
  $name: Primary axis
  $description: The direction "|" runs; "," stacks the other way.
  $options:
  - row: Row
  - column: Column

- crossAlign: center
  $name: Group alignment
  $description: How shorter groups align against their siblings.
  $options:
  - start: Start
  - center: Center
  - end: End

- buttonWidth: 0
  $name: Icon width (px, 0 = native)

- buttonHeight: 0
  $name: Icon height (px, 0 = native)

- buttonSpacing: 0
  $name: Icon spacing (px)
  $description: Gap between placed items on both axes.

- groupOffsetX: 0
  $name: Group horizontal offset (px)

- groupOffsetY: 0
  $name: Group vertical offset (px)

- overflowOffsetX: 0
  $name: Hidden icons X nudge (px)

- overflowOffsetY: 0
  $name: Hidden icons Y nudge (px)

- emojiOffsetX: 0
  $name: Emoji X nudge (px)

- emojiOffsetY: 0
  $name: Emoji Y nudge (px)

- touchKeyboardOffsetX: 0
  $name: Touch keyboard X nudge (px)

- touchKeyboardOffsetY: 0
  $name: Touch keyboard Y nudge (px)

- penMenuOffsetX: 0
  $name: Pen menu X nudge (px)

- penMenuOffsetY: 0
  $name: Pen menu Y nudge (px)

- virtualTouchpadOffsetX: 0
  $name: Virtual touchpad X nudge (px)

- virtualTouchpadOffsetY: 0
  $name: Virtual touchpad Y nudge (px)

- inputIndicatorOffsetX: 0
  $name: Input indicator X nudge (px)

- inputIndicatorOffsetY: 0
  $name: Input indicator Y nudge (px)

- minimumTrayHeight: 44
  $name: Minimum tray height (px)
  $description: >-
    Below this height the mod leaves the native layout unchanged. Use 0 to
    allow rearranging on any taskbar height.

- mergeMode: auto
  $name: Detection mode
  $description: >-
    Automatic is guarded and recommended. Force allows the complete native
    MainStack to participate as the emoji item when Windows doesn't
    identify its controls.
  $options:
  - auto: Automatic
  - forceMainStack: Force MainStack (experimental)

- detailedLogging: false
  $name: Detailed discovery logging
  $description: >-
    Logs tray host names, classes, per-icon glyph codepoints, and the
    computed placements.
*/
// ==/WindhawkModSettings==

#include <atomic>
#include <algorithm>
#include <cmath>
#include <cwctype>
#include <exception>
#include <functional>
#include <optional>
#include <string>
#include <utility>
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

// ── Nested group layout ───────────────────────────────────────────────────
// Template block: _templates/nested-group-layout.h v1.1 (verbatim copy —
// keep in sync with the template; Windhawk mods are single-file).

namespace windhawk_mod_templates::nested_group_layout {

enum class Axis { Horizontal, Vertical };
enum class CrossAlign { Start, Center, End };

struct Size {
    double width = 0.0;
    double height = 0.0;
    bool Empty() const { return width <= 0.0 || height <= 0.0; }
};

struct Config {
    Axis primaryAxis = Axis::Horizontal;
    double spacing = 0.0;
    CrossAlign crossAlign = CrossAlign::Center;
};

struct Placement {
    std::wstring token;
    double x = 0.0;
    double y = 0.0;
    Size size;
};

struct Node {
    std::wstring token;            // non-empty = leaf
    std::vector<Node> children;    // group children, laid along axis
    Axis axis = Axis::Horizontal;  // group axis (unused for leaves)
};

class Parser {
public:
    Parser(std::wstring const& text, Axis axis)
        : text_(text), axis_(axis) {}

    bool Run(Node& root) {
        position_ = 0;
        valid_ = true;
        root = ParseExpr(axis_);
        SkipSpace();
        return valid_ && position_ >= text_.size();
    }

private:
    Node ParseExpr(Axis axis) {
        Node node;
        node.axis = axis;
        node.children.push_back(ParseStack(axis));
        while (Peek() == L'|') {
            ++position_;
            node.children.push_back(ParseStack(axis));
        }
        return node;
    }

    Node ParseStack(Axis axis) {
        Node node;
        node.axis = axis == Axis::Horizontal ? Axis::Vertical
                                             : Axis::Horizontal;
        node.children.push_back(ParseUnit(axis));
        while (Peek() == L',') {
            ++position_;
            node.children.push_back(ParseUnit(axis));
        }
        return node;
    }

    Node ParseUnit(Axis axis) {
        SkipSpace();
        if (position_ < text_.size() && text_[position_] == L'(') {
            ++position_;
            Node inner = ParseExpr(axis);
            SkipSpace();
            if (position_ < text_.size() && text_[position_] == L')') {
                ++position_;
            } else {
                valid_ = false;
            }
            return inner;
        }
        Node leaf;
        size_t start = position_;
        while (position_ < text_.size() &&
               text_[position_] != L'|' && text_[position_] != L',' &&
               text_[position_] != L'(' && text_[position_] != L')' &&
               !iswspace(text_[position_]))
            ++position_;
        leaf.token = text_.substr(start, position_ - start);
        if (leaf.token.empty())
            valid_ = false;
        return leaf;
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
    Axis axis_;
    size_t position_ = 0;
    bool valid_ = true;
};

inline bool Parse(std::wstring const& text, Axis primaryAxis, Node& root) {
    return Parser(text, primaryAxis).Run(root);
}

using SizeResolver = std::function<Size(std::wstring const&)>;

inline Size Measure(Node const& node, Config const& config,
                    SizeResolver const& resolve) {
    if (!node.token.empty())
        return resolve(node.token);

    double main = 0.0;
    double cross = 0.0;
    int placed = 0;
    for (auto const& child : node.children) {
        Size size = Measure(child, config, resolve);
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

inline void Arrange(Node const& node, Config const& config,
                    SizeResolver const& resolve, double x, double y,
                    std::vector<Placement>& out) {
    if (!node.token.empty()) {
        Size size = resolve(node.token);
        if (!size.Empty())
            out.push_back({node.token, x, y, size});
        return;
    }

    Size total = Measure(node, config, resolve);
    if (total.Empty())
        return;
    double cursor = node.axis == Axis::Horizontal ? x : y;
    for (auto const& child : node.children) {
        Size size = Measure(child, config, resolve);
        if (size.Empty())
            continue;
        double unused = node.axis == Axis::Horizontal
                            ? total.height - size.height
                            : total.width - size.width;
        double crossOffset =
            config.crossAlign == CrossAlign::Center ? unused / 2.0
            : config.crossAlign == CrossAlign::End  ? unused
                                                    : 0.0;
        if (node.axis == Axis::Horizontal) {
            Arrange(child, config, resolve, cursor, y + crossOffset, out);
            cursor += size.width + config.spacing;
        } else {
            Arrange(child, config, resolve, x + crossOffset, cursor, out);
            cursor += size.height + config.spacing;
        }
    }
}

// Parse + measure + arrange in one call. Returns false only on a parse
// error (unbalanced parentheses / trailing garbage). placements come back
// in expression order; totalSize is the tight bounding size of the group.
inline bool Compute(std::wstring const& text, Config const& config,
                    SizeResolver const& resolve,
                    std::vector<Placement>& placements, Size& totalSize) {
    Node root;
    if (!Parse(text, config.primaryAxis, root))
        return false;
    totalSize = Measure(root, config, resolve);
    placements.clear();
    Arrange(root, config, resolve, 0.0, 0.0, placements);
    return true;
}

} // namespace windhawk_mod_templates::nested_group_layout

// ── Visual tree walk ──────────────────────────────────────────────────────
// Template block: _templates/visual-tree-walk.h v1.0 (verbatim copy — keep
// in sync with the template; Windhawk mods are single-file).

namespace windhawk_mod_templates::visual_tree_walk {

using winrt::Windows::UI::Xaml::FrameworkElement;
using winrt::Windows::UI::Xaml::Controls::StackPanel;
using winrt::Windows::UI::Xaml::Media::VisualTreeHelper;

// Depth-first visit of every FrameworkElement descendant (root excluded).
// The visitor returns true to stop the walk early.
inline bool ForEachDescendant(
    FrameworkElement const& root, int maxDepth,
    std::function<bool(FrameworkElement const&, int)> const& visit,
    int depth = 0) {
    if (!root || depth >= maxDepth)
        return false;
    int count = VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < count; ++i) {
        auto child =
            VisualTreeHelper::GetChild(root, i).try_as<FrameworkElement>();
        if (!child)
            continue;
        if (visit(child, depth + 1))
            return true;
        if (ForEachDescendant(child, maxDepth, visit, depth + 1))
            return true;
    }
    return false;
}

// First descendant matching the predicate, depth-first document order.
inline FrameworkElement FindDescendant(
    FrameworkElement const& root, int maxDepth,
    std::function<bool(FrameworkElement const&)> const& predicate) {
    FrameworkElement found = nullptr;
    ForEachDescendant(root, maxDepth,
                      [&](FrameworkElement const& element, int) {
                          if (predicate(element)) {
                              found = element;
                              return true;
                          }
                          return false;
                      });
    return found;
}

// Every descendant matching the predicate, in depth-first document order —
// which is also visual order for the tray's horizontal stacks.
inline void CollectDescendants(
    FrameworkElement const& root, int maxDepth,
    std::function<bool(FrameworkElement const&)> const& predicate,
    std::vector<FrameworkElement>& out) {
    ForEachDescendant(root, maxDepth,
                      [&](FrameworkElement const& element, int) {
                          if (predicate(element))
                              out.push_back(element);
                          return false;
                      });
}

// The OmniButton battery walk: the first non-items-host StackPanel
// descendant — the inner panel whose children are the individually
// addressable native elements (glyph, percent, per-icon views).
inline StackPanel FindInnerStackPanel(FrameworkElement const& root,
                                      int maxDepth) {
    StackPanel found = nullptr;
    ForEachDescendant(root, maxDepth,
                      [&](FrameworkElement const& element, int) {
                          auto panel = element.try_as<StackPanel>();
                          if (panel && !panel.IsItemsHost()) {
                              found = panel;
                              return true;
                          }
                          return false;
                      });
    return found;
}

} // namespace windhawk_mod_templates::visual_tree_walk

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

// ── Start-adjacent placement ──────────────────────────────────────────────
// Template block: _templates/start-placement.h v1.2 (verbatim copy — keep
// in sync with the template; Windhawk mods are single-file).

namespace windhawk_mod_templates::start_placement {

using winrt::Windows::UI::Xaml::DependencyObject;
using winrt::Windows::UI::Xaml::FrameworkElement;
using winrt::Windows::UI::Xaml::HorizontalAlignment;
using winrt::Windows::UI::Xaml::Thickness;
using winrt::Windows::UI::Xaml::UIElement;
using winrt::Windows::UI::Xaml::VerticalAlignment;
using winrt::Windows::UI::Xaml::Visibility;
using winrt::Windows::UI::Xaml::Automation::AutomationProperties;
using winrt::Windows::UI::Xaml::Controls::Canvas;
using winrt::Windows::UI::Xaml::Controls::Grid;
using winrt::Windows::UI::Xaml::Media::TranslateTransform;
using winrt::Windows::UI::Xaml::Media::VisualTreeHelper;

enum class Side {
    Left,
    Right,
};

struct Lease {
    Grid group{nullptr};
    Grid rootGrid{nullptr};
    FrameworkElement startButton{nullptr};
    FrameworkElement taskItemsPanel{nullptr};
    Thickness groupOriginalMargin{};
    Thickness taskItemsPanelOriginalMargin{};
    bool startInTaskItemsPanel = false;
    winrt::event_token layoutToken{};
    Side side = Side::Left;
    double spacing = 0.0;
};

template<typename Predicate>
inline FrameworkElement FindDescendant(FrameworkElement const& root,
                                       Predicate&& predicate,
                                       int depth = 0) {
    if (!root || depth > 64)
        return nullptr;
    if (predicate(root))
        return root;
    int count = VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < count; ++i) {
        auto child = VisualTreeHelper::GetChild(root, i)
                         .try_as<FrameworkElement>();
        auto match = FindDescendant(
            child, std::forward<Predicate>(predicate), depth + 1);
        if (match)
            return match;
    }
    return nullptr;
}

inline Grid FindTaskbarRootGrid(FrameworkElement const& root) {
    auto taskbarFrame = FindDescendant(
        root, [](FrameworkElement const& element) {
            return winrt::get_class_name(element) ==
                   L"Taskbar.TaskbarFrame";
        });
    if (!taskbarFrame)
        return nullptr;

    int count = VisualTreeHelper::GetChildrenCount(taskbarFrame);
    for (int i = 0; i < count; ++i) {
        auto child = VisualTreeHelper::GetChild(taskbarFrame, i)
                         .try_as<Grid>();
        if (child && child.Name() == L"RootGrid")
            return child;
    }
    return nullptr;
}

inline FrameworkElement FindStartButton(FrameworkElement const& root) {
    return FindDescendant(
        root, [](FrameworkElement const& element) {
            return winrt::get_class_name(element) ==
                       L"Taskbar.ExperienceToggleButton" &&
                   AutomationProperties::GetAutomationId(element) ==
                       L"StartButton";
        });
}

inline bool Position(Lease& lease) noexcept {
    if (!lease.group || !lease.rootGrid || !lease.startButton)
        return false;

    try {
        double groupWidth = lease.group.Width() +
                            lease.groupOriginalMargin.Left +
                            lease.groupOriginalMargin.Right;
        double groupHeight = lease.group.Height() +
                             lease.groupOriginalMargin.Top +
                             lease.groupOriginalMargin.Bottom;
        bool startHidden =
            lease.startButton.Visibility() == Visibility::Collapsed;
        double startWidth = lease.startButton.ActualWidth();
        double startHeight = lease.startButton.ActualHeight();
        if (startWidth <= 0.0 && !startHidden)
            startWidth = 44.0;
        if (startHeight <= 0.0)
            startHeight = groupHeight;

        // rawX is Start's live layout position with our own counter-shift
        // backed out. It is re-read on every layout pass, so task-list churn
        // on a center-aligned taskbar re-centers the group naturally.
        auto transform = lease.startButton.TransformToVisual(lease.rootGrid);
        auto point = transform.TransformPoint({0.0f, 0.0f});
        auto existingShift =
            lease.startButton.RenderTransform().try_as<TranslateTransform>();
        double currentShift = existingShift ? existingShift.X() : 0.0;
        double rawX = point.X - currentShift;

        double spacing = std::max(0.0, lease.spacing);
        double push = groupWidth + spacing;
        if (lease.taskItemsPanel) {
            auto margin = lease.taskItemsPanel.Margin();
            double needed =
                lease.taskItemsPanelOriginalMargin.Left + push;
            if (std::fabs(margin.Left - needed) > 0.5) {
                margin.Left = needed;
                lease.taskItemsPanel.Margin(margin);
            }
        }

        // The Start counter-shift is a constant per mode, not an absolute-
        // anchor correction. When Start rides the repeater-margin push, room
        // for a Left group already opens at the block's left edge (no shift),
        // and a Right group needs Start pulled back so the gap opens between
        // Start and the task items. When Start sits outside the repeater the
        // roles invert: the pushed items leave the Right gap by themselves,
        // and a Left group needs Start pushed out of the way instead.
        double neededShift;
        if (lease.side == Side::Left)
            neededShift = lease.startInTaskItemsPanel ? 0.0 : push;
        else
            neededShift = lease.startInTaskItemsPanel ? -push : 0.0;
        if (startHidden)
            neededShift = 0.0;

        if (std::fabs(neededShift) <= 0.5) {
            if (existingShift || lease.startButton.RenderTransform())
                lease.startButton.ClearValue(
                    UIElement::RenderTransformProperty());
        } else if (std::fabs(currentShift - neededShift) > 0.5) {
            TranslateTransform startShift;
            startShift.X(neededShift);
            lease.startButton.RenderTransform(startShift);
        }

        // Place the group relative to where Start actually ends up.
        double startFinalX = rawX + neededShift;
        double left = lease.side == Side::Left
                          ? startFinalX - groupWidth - spacing
                          : startFinalX + startWidth + spacing;
        if (left < 0.0)
            left = 0.0;

        // v1.2: center against the taskbar root; Start's own box is not a
        // reliable vertical reference.
        double rootHeight = lease.rootGrid.ActualHeight();
        double top = rootHeight > 0.0
                         ? (rootHeight - groupHeight) / 2.0
                         : point.Y + (startHeight - groupHeight) / 2.0;
        if (top < 0.0)
            top = 0.0;
        double rootWidth = lease.rootGrid.ActualWidth();
        if (rootWidth > 0.0 && left + groupWidth > rootWidth)
            left = std::max(0.0, rootWidth - groupWidth);

        auto target = lease.groupOriginalMargin;
        target.Left += left;
        target.Top += top;
        auto current = lease.group.Margin();
        if (std::fabs(current.Left - target.Left) > 0.5 ||
            std::fabs(current.Top - target.Top) > 0.5) {
            lease.group.Margin(target);
        }
        return true;
    } catch (...) {
        return false;
    }
}

inline bool Release(Lease& lease) noexcept {
    if (!lease.group)
        return false;

    try {
        if (lease.rootGrid && lease.layoutToken)
            lease.rootGrid.LayoutUpdated(lease.layoutToken);
        if (lease.taskItemsPanel)
            lease.taskItemsPanel.Margin(
                lease.taskItemsPanelOriginalMargin);
        if (lease.startButton)
            lease.startButton.ClearValue(
                UIElement::RenderTransformProperty());
        lease.group.Margin(lease.groupOriginalMargin);
        if (lease.rootGrid) {
            uint32_t index = 0;
            if (lease.rootGrid.Children().IndexOf(lease.group, index))
                lease.rootGrid.Children().RemoveAt(index);
        }
    } catch (...) {
        lease = {};
        return false;
    }
    lease = {};
    return true;
}

inline bool Acquire(FrameworkElement const& root, Grid const& group,
                    Side side, double spacing, Lease& lease) {
    if (!root || !group || lease.group || group.Width() <= 0.0 ||
        group.Height() <= 0.0)
        return false;

    auto rootGrid = FindTaskbarRootGrid(root);
    auto startButton = FindStartButton(root);
    if (!rootGrid || !startButton)
        return false;

    lease.group = group;
    lease.rootGrid = rootGrid;
    lease.startButton = startButton;
    lease.groupOriginalMargin = group.Margin();
    lease.side = side;
    lease.spacing = spacing;

    group.HorizontalAlignment(HorizontalAlignment::Left);
    group.VerticalAlignment(VerticalAlignment::Top);
    Grid::SetColumn(group, 0);
    Grid::SetColumnSpan(
        group,
        std::max(1, static_cast<int>(
                        rootGrid.ColumnDefinitions().Size())));
    Canvas::SetZIndex(group, 1000);
    rootGrid.Children().Append(group);

    lease.taskItemsPanel = FindDescendant(
        rootGrid, [](FrameworkElement const& element) {
            return element.Name() == L"TaskbarFrameRepeater";
        });
    if (lease.taskItemsPanel) {
        lease.taskItemsPanelOriginalMargin =
            lease.taskItemsPanel.Margin();
        // Whether Start rides the repeater-margin push is build-dependent.
        // Resolve it from the visual tree instead of inferring from motion.
        try {
            auto panel = lease.taskItemsPanel.as<DependencyObject>();
            for (auto node = startButton.as<DependencyObject>(); node;
                 node = VisualTreeHelper::GetParent(node)) {
                if (node == panel) {
                    lease.startInTaskItemsPanel = true;
                    break;
                }
            }
        } catch (...) {
            lease.startInTaskItemsPanel = false;
        }
    }

    if (!Position(lease)) {
        Release(lease);
        return false;
    }
    lease.layoutToken = rootGrid.LayoutUpdated(
        [&lease](auto const&, auto const&) {
            Position(lease);
        });
    return true;
}

} // namespace windhawk_mod_templates::start_placement

namespace ngl = windhawk_mod_templates::nested_group_layout;
namespace tree_walk = windhawk_mod_templates::visual_tree_walk;
namespace lease_column = windhawk_mod_templates::injected_grid_column;
namespace start_placement = windhawk_mod_templates::start_placement;

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
    Position position;
    std::wstring layout;
    ngl::Axis primaryAxis;
    ngl::CrossAlign crossAlign;
    int buttonWidth;   // 0 = native
    int buttonHeight;  // 0 = native
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

// Lifecycle v1.3: heap-only state destructs normally; direct XAML handles use
// no_destroy; XAML-owning containers use no_destroy optional<container> and
// reset their backing storage after controlled UI-thread cleanup.
static Settings g_settings{};  // exit-time-safe: heap-only
static std::atomic<bool> g_unloading = false;
static HWND g_taskbarWnd = nullptr;
static HANDLE g_retryStopEvent = nullptr;
static HANDLE g_retryThread = nullptr;
static SRWLOCK g_retryLock = SRWLOCK_INIT;
static std::atomic<bool> g_retryForceApply = false;

// Property snapshot for every element the mod touches (tray hosts and the
// individual IconViews inside them). Hosts additionally get a zero-size
// marker child in the tray grid so their column survives live re-indexing.
struct ElementSnapshot {
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
    int visibleIconViews = 0;
};

// One placeable thing: a native IconView (per-icon control), or a whole
// tray host for the chevron / MainStack fallback.
struct LayoutItem {
    std::wstring token;
    FrameworkElement element{nullptr};  // IconView, or host when hostLeaf
    FrameworkElement host{nullptr};     // direct tray-grid child
    bool hostLeaf = false;
    double naturalW = 0.0;
    double naturalH = 0.0;
    int offsetX = 0;
    int offsetY = 0;
};

[[clang::no_destroy]] static std::optional<std::vector<ElementSnapshot>>
    g_hostSnapshots{std::in_place};
[[clang::no_destroy]] static std::optional<std::vector<ElementSnapshot>>
    g_iconSnapshots{std::in_place};
static std::atomic<bool> g_layoutApplied = false;
[[clang::no_destroy]] static Grid g_layoutGrid{nullptr};
[[clang::no_destroy]] static Grid g_group{nullptr};
static lease_column::Lease g_columnLease;  // exit-time-safe: heap-only
[[clang::no_destroy]] static start_placement::Lease g_startLease;

static constexpr PCWSTR kLayoutColumnMarkerName =
    L"TrayUtilityCustomizerColumnMarker";
static constexpr PCWSTR kGroupName = L"TrayUtilityCustomizerGroup";

// Stable Segoe Fluent glyphs captured live on build 26200. Runtime class,
// XAML name, and AutomationId matching below use Windows' language-neutral
// identities; no localized accessibility text participates in detection.
static constexpr wchar_t kGlyphEmoji = 0xF353;
static constexpr wchar_t kGlyphTouchKeyboard = 0xE765;

// ── Transient reapply plumbing ────────────────────────────────────────────
// Windows hides/shows utility icons live (taskbar settings toggles, the
// transient touch keyboard). Watch every candidate's visibility and re-run
// the whole layout when the visible set changes.

static bool ApplyLayout();

struct HostWatcher {
    FrameworkElement element{nullptr};
    int64_t token = 0;
};
[[clang::no_destroy]] static std::optional<std::vector<HostWatcher>>
    g_hostWatchers{std::in_place};
static winrt::event_token g_trayLayoutToken{};
[[clang::no_destroy]] static DispatcherTimer g_reapplyTimer{nullptr};
[[clang::no_destroy]] static DispatcherTimer g_startSettleTimer{nullptr};

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
    for (auto& watcher : *g_hostWatchers) {
        try {
            watcher.element.UnregisterPropertyChangedCallback(
                UIElement::VisibilityProperty(), watcher.token);
        } catch (...) {
        }
    }
    g_hostWatchers->clear();
}

static void WatchHostVisibility(FrameworkElement const& element) {
    if (!element) {
        return;
    }
    for (auto const& watcher : *g_hostWatchers) {
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
        g_hostWatchers->push_back({element, token});
    } catch (...) {
    }
}

static int ClampSetting(int value, int low, int high) {
    return value < low ? low : value > high ? high : value;
}

static std::wstring GetStringSetting(PCWSTR key) {
    PCWSTR value = Wh_GetStringSetting(key);
    std::wstring result = value;
    Wh_FreeStringSetting(value);
    return result;
}

static int GetOffsetSetting(PCWSTR key) {
    return ClampSetting(Wh_GetIntSetting(key), -100, 100);
}

static void LoadSettings() {
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

    g_settings.layout = GetStringSetting(L"layout");
    if (g_settings.layout.empty()) {
        g_settings.layout = L"overflow | emoji | touchKeyboard";
    }

    g_settings.primaryAxis =
        GetStringSetting(L"primaryAxis") == L"column"
            ? ngl::Axis::Vertical
            : ngl::Axis::Horizontal;
    auto crossAlign = GetStringSetting(L"crossAlign");
    if (crossAlign == L"start") {
        g_settings.crossAlign = ngl::CrossAlign::Start;
    } else if (crossAlign == L"end") {
        g_settings.crossAlign = ngl::CrossAlign::End;
    } else {
        g_settings.crossAlign = ngl::CrossAlign::Center;
    }

    g_settings.buttonWidth =
        ClampSetting(Wh_GetIntSetting(L"buttonWidth"), 0, 96);
    g_settings.buttonHeight =
        ClampSetting(Wh_GetIntSetting(L"buttonHeight"), 0, 96);
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

// Lifecycle v1.2 dispatcher (taskbar-xaml-lifecycle.template.cpp): contain
// every UI-dispatch exception at the WH_CALLWNDPROC boundary.

using WindowThreadProc = void (*)(void*);

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

static bool InvokeWindowThreadProc(WindowThreadProc proc, void* parameter) {
    try {
        proc(parameter);
        return true;
    } catch (...) {
        LogCurrentUiException(L"UI callback");
    }
    return false;
}

static bool RunFromWindowThread(HWND window, WindowThreadProc proc,
                                void* parameter) {
    static UINT message = RegisterWindowMessageW(
        L"Windhawk_RunFromWindowThread_" WH_MOD_ID);
    struct Dispatch {
        WindowThreadProc proc;
        void* parameter;
        bool succeeded = false;
    };
    DWORD threadId = GetWindowThreadProcessId(window, nullptr);
    if (!threadId) return false;
    if (threadId == GetCurrentThreadId()) {
        return InvokeWindowThreadProc(proc, parameter);
    }

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

    Dispatch dispatch{proc, parameter};
    SendMessageW(window, message, 0, reinterpret_cast<LPARAM>(&dispatch));
    UnhookWindowsHookEx(hook);
    return dispatch.succeeded;
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
#if defined(_M_X64)
    {
        // 48:83EC 28 | sub rsp,28
        // 48:83C1 48 | add rcx,48
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
    }
#elif defined(_M_ARM64)
    {
        // 7f2303d5 pacibsp
        // fd7bbfa9 stp     fp, lr, [sp, #-0x10]!
        // fd030091 mov     fp, sp
        // 080c41f8 ldr     x8, [x0, #0x10]!
        const DWORD* words =
            reinterpret_cast<const DWORD*>(TaskbarHost_FrameHeight_Original);
        if (words[0] == 0xD503237F &&
            (words[1] & 0xFFC07FFF) == 0xA9807BFD &&
            words[2] == 0x910003FD &&
            (words[3] & 0xFFF00FE0) == 0xF8400C00) {
            offset = (words[3] >> 12) & 0xFF;
        } else {
            Wh_Log(L"[XamlRoot] Unsupported TaskbarHost::FrameHeight");
        }
    }
#else
#error "Unsupported architecture"
#endif

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

// ── Utility discovery ─────────────────────────────────────────────────────

static std::wstring ToLower(std::wstring value) {
    for (auto& ch : value) {
        ch = static_cast<wchar_t>(towlower(ch));
    }
    return value;
}

static bool ElementMatchesStableIdentity(
    FrameworkElement const& element,
    PCWSTR identity) {
    try {
        if (element.Name() == identity ||
            AutomationProperties::GetAutomationId(element) == identity ||
            winrt::get_class_name(element) == identity) {
            return true;
        }

        auto dataContext = element.DataContext();
        return dataContext && winrt::get_class_name(dataContext) == identity;
    } catch (...) {
        return false;
    }
}

static bool TreeContainsStableIdentity(
    FrameworkElement const& element,
    PCWSTR identity) {
    if (ElementMatchesStableIdentity(element, identity)) {
        return true;
    }

    return tree_walk::ForEachDescendant(
        element, 12,
        [identity](FrameworkElement const& child, int) {
            return ElementMatchesStableIdentity(child, identity);
        });
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

static bool IsIconView(FrameworkElement const& element) {
    return winrt::get_class_name(element) == L"SystemTray.IconView";
}

// Effective visibility: the element and every ancestor up to (and
// including) stopAt must be Visible. Windows hides utilities by collapsing
// mid-level wrappers while the IconView itself stays Visible.
static bool IsEffectivelyVisible(FrameworkElement const& element,
                                 FrameworkElement const& stopAt) {
    DependencyObject current = element;
    while (current) {
        auto fe = current.try_as<FrameworkElement>();
        if (fe && fe.Visibility() != Visibility::Visible) {
            return false;
        }
        if (fe == stopAt) {
            return true;
        }
        current = VisualTreeHelper::GetParent(current);
    }
    return true;
}

static void CollectVisibleIconViews(FrameworkElement const& host,
                                    std::vector<FrameworkElement>& out) {
    std::vector<FrameworkElement> all;
    tree_walk::CollectDescendants(
        host, 12,
        [](FrameworkElement const& element) {
            return IsIconView(element);
        },
        all);
    for (auto const& icon : all) {
        if (IsEffectivelyVisible(icon, host)) {
            out.push_back(icon);
        }
    }
}

static int CountVisibleIconViews(FrameworkElement const& host) {
    std::vector<FrameworkElement> icons;
    CollectVisibleIconViews(host, icons);
    return static_cast<int>(icons.size());
}

// First glyph codepoint of the first non-empty TextBlock under (or at) the
// element — the language-neutral identity of a tray icon.
static wchar_t FirstGlyphChar(FrameworkElement const& element) {
    try {
        if (auto text = element.try_as<TextBlock>()) {
            auto value = text.Text();
            if (!value.empty()) {
                return value[0];
            }
        }
        wchar_t found = 0;
        tree_walk::ForEachDescendant(
            element, 12,
            [&](FrameworkElement const& child, int) {
                if (auto text = child.try_as<TextBlock>()) {
                    auto value = text.Text();
                    if (!value.empty()) {
                        found = value[0];
                        return true;
                    }
                }
                return false;
            });
        return found;
    } catch (...) {
        return 0;
    }
}

static std::wstring DescribeElementGlyphs(FrameworkElement const& element) {
    wchar_t glyph = FirstGlyphChar(element);
    if (!glyph) {
        return L"<none>";
    }
    wchar_t buffer[16];
    swprintf_s(buffer, L"U+%04X", static_cast<unsigned>(glyph));
    return buffer;
}

static bool IconViewMatchesToken(FrameworkElement const& iconView,
                                 std::wstring const& token) {
    wchar_t glyph = FirstGlyphChar(iconView);
    if (token == L"emoji") {
        return glyph == kGlyphEmoji ||
               TreeContainsStableIdentity(
                   iconView,
                   L"SystemTray.EmojiAndMoreSystemTrayIconDataModel");
    }
    if (token == L"touchKeyboard") {
        return glyph == kGlyphTouchKeyboard ||
               TreeContainsStableIdentity(
                   iconView,
                   L"SystemTray.TouchKeyboardSystemTrayIconDataModel") ||
               TreeContainsStableIdentity(iconView,
                                          L"TouchKeyboardModeButton");
    }
    if (token == L"penMenu") {
        return TreeContainsStableIdentity(
                   iconView,
                   L"SystemTray.InkWorkspaceSystemTrayIconDataModel") ||
               TreeContainsStableIdentity(iconView,
                                          L"SystemTray.InkWorkspaceButton") ||
               TreeContainsStableIdentity(iconView,
                                          L"PenWorkspaceButton");
    }
    if (token == L"virtualTouchpad") {
        return TreeContainsStableIdentity(
            iconView,
            L"SystemTray.VirtualTouchpadSystemTrayIconDataModel");
    }
    if (token == L"inputIndicator") {
        return TreeContainsStableIdentity(
                   iconView,
                   L"SystemTray.LanguageSystemTrayIconDataModel") ||
               TreeContainsStableIdentity(
                   iconView,
                   L"SystemTray.ImeSystemTrayIconDataModel") ||
               TreeContainsStableIdentity(
                   iconView,
                   L"SystemTray.LanguageTextIconContent") ||
               TreeContainsStableIdentity(
                   iconView,
                   L"SystemTray.LanguageImageIconContent") ||
               TreeContainsStableIdentity(iconView,
                                          L"SystemTray.IMEButton") ||
               TreeContainsStableIdentity(iconView,
                                          L"SystemTray.InputIndicatorButton");
    }
    return false;
}

// Direct tray children that can carry utility IconViews. Excludes the
// well-known non-utility hosts so battery/volume/clock icons never match.
static bool IsUtilityCandidateHost(FrameworkElement const& element) {
    auto name = std::wstring(element.Name());
    return name != L"ControlCenterButton" &&
           name != L"NotificationCenterButton" &&
           name != L"ShowDesktopStack" &&
           name != L"NotificationAreaIcons" &&
           name != kGroupName &&
           name.find(L"TrayUtilityCustomizer") == std::wstring::npos;
}

static double NaturalWidth(FrameworkElement const& element) {
    double width = element.ActualWidth();
    return width > 0.0 ? width : 24.0;
}

static double NaturalHeight(FrameworkElement const& element) {
    // Tray icons stretch to the full tray height, so ActualHeight is not a
    // content size. The native content box is square-ish: use the width,
    // capped by whatever height the element actually has.
    double width = NaturalWidth(element);
    double height = element.ActualHeight();
    return height > 0.0 ? std::min(width, height) : width;
}

// Forgiving token spelling: canonicalize what the user typed; empty result
// means the token is unknown (worth a log line, not silent loss).
static std::wstring CanonicalToken(std::wstring const& raw) {
    auto lower = ToLower(raw);
    static constexpr struct {
        PCWSTR alias;
        PCWSTR token;
    } kAliases[] = {
        {L"overflow", L"overflow"},
        {L"chevron", L"overflow"},
        {L"hidden", L"overflow"},
        {L"hiddenicons", L"overflow"},
        {L"emoji", L"emoji"},
        {L"touchkeyboard", L"touchKeyboard"},
        {L"keyboard", L"touchKeyboard"},
        {L"penmenu", L"penMenu"},
        {L"pen", L"penMenu"},
        {L"virtualtouchpad", L"virtualTouchpad"},
        {L"touchpad", L"virtualTouchpad"},
        {L"inputindicator", L"inputIndicator"},
        {L"input", L"inputIndicator"},
        {L"language", L"inputIndicator"},
    };
    for (auto const& alias : kAliases) {
        if (lower == alias.alias) {
            return alias.token;
        }
    }
    return L"";
}

static void CollectLeafTokens(ngl::Node const& node,
                              std::vector<std::wstring>& out) {
    if (!node.token.empty()) {
        out.push_back(node.token);
        return;
    }
    for (auto const& child : node.children) {
        CollectLeafTokens(child, out);
    }
}

static std::vector<LayoutItem> ResolveLayoutItems(
    Grid const& trayGrid,
    FrameworkElement const& overflowHost,
    FrameworkElement const& mainStack,
    std::vector<std::wstring> const& wantedTokens) {
    std::vector<LayoutItem> items;

    // Utility candidate hosts and their visible icons, collected once.
    std::vector<FrameworkElement> candidateHosts;
    for (auto const& child : trayGrid.Children()) {
        auto element = child.try_as<FrameworkElement>();
        if (element && IsUtilityCandidateHost(element)) {
            candidateHosts.push_back(element);
        }
    }

    for (auto const& token : wantedTokens) {
        LayoutItem item;
        item.token = token;

        if (item.token == L"overflow") {
            item.element = overflowHost;
            item.host = overflowHost;
            item.hostLeaf = true;
            item.offsetX = g_settings.overflowOffsetX;
            item.offsetY = g_settings.overflowOffsetY;
        } else {
            for (auto const& host : candidateHosts) {
                std::vector<FrameworkElement> icons;
                CollectVisibleIconViews(host, icons);
                for (auto const& icon : icons) {
                    if (IconViewMatchesToken(icon, item.token)) {
                        item.element = icon;
                        item.host = host;
                        break;
                    }
                }
                if (item.element) {
                    break;
                }
            }

            if (!item.element && item.token == L"emoji" && mainStack) {
                int visibleIcons = CountVisibleIconViews(mainStack);
                if (g_settings.mergeMode == MergeMode::ForceMainStack ||
                    visibleIcons == 1) {
                    item.element = mainStack;
                    item.host = mainStack;
                    item.hostLeaf = true;
                    Wh_Log(
                        L"[Discover] emoji using MainStack fallback "
                        L"(visibleIcons=%d force=%d)",
                        visibleIcons,
                        g_settings.mergeMode == MergeMode::ForceMainStack);
                }
            }

            if (item.token == L"emoji") {
                item.offsetX = g_settings.emojiOffsetX;
                item.offsetY = g_settings.emojiOffsetY;
            } else if (item.token == L"touchKeyboard") {
                item.offsetX = g_settings.touchKeyboardOffsetX;
                item.offsetY = g_settings.touchKeyboardOffsetY;
            } else if (item.token == L"penMenu") {
                item.offsetX = g_settings.penMenuOffsetX;
                item.offsetY = g_settings.penMenuOffsetY;
            } else if (item.token == L"virtualTouchpad") {
                item.offsetX = g_settings.virtualTouchpadOffsetX;
                item.offsetY = g_settings.virtualTouchpadOffsetY;
            } else if (item.token == L"inputIndicator") {
                item.offsetX = g_settings.inputIndicatorOffsetX;
                item.offsetY = g_settings.inputIndicatorOffsetY;
            }
        }

        if (!item.element || !item.host) {
            Wh_Log(L"[Discover] %s not found; it won't participate",
                   item.token.c_str());
            continue;
        }

        // Watch even skipped/hidden candidates so toggles re-run layout.
        WatchHostVisibility(item.host);
        if (!item.hostLeaf) {
            WatchHostVisibility(item.element);
        }
        if (item.host.Visibility() != Visibility::Visible ||
            (item.hostLeaf &&
             CountVisibleIconViews(item.host) == 0 &&
             item.token != L"overflow")) {
            Wh_Log(L"[Discover] %s is hidden; leaving it native",
                   item.token.c_str());
            continue;
        }

        item.naturalW = NaturalWidth(item.element);
        item.naturalH = NaturalHeight(item.element);

        if (g_settings.detailedLogging) {
            Wh_Log(
                L"[Discover] %s host=%s hostLeaf=%d glyph=%s "
                L"natural=%.1fx%.1f",
                item.token.c_str(),
                item.host.Name().c_str(),
                item.hostLeaf,
                DescribeElementGlyphs(item.element).c_str(),
                item.naturalW,
                item.naturalH);
        }
        items.push_back(std::move(item));
    }

    return items;
}

static void LogElement(FrameworkElement const& element, PCWSTR prefix) {
    if (!g_settings.detailedLogging || !element) {
        return;
    }
    try {
        Wh_Log(
            L"[Discover] %s class=%s name=%s glyph=%s col=%d "
            L"size=%.1fx%.1f visibleIcons=%d",
            prefix,
            winrt::get_class_name(element).c_str(),
            element.Name().c_str(),
            DescribeElementGlyphs(element).c_str(),
            Grid::GetColumn(element),
            element.ActualWidth(),
            element.ActualHeight(),
            CountVisibleIconViews(element));
    } catch (...) {
        Wh_Log(L"[Discover] Failed to log %s", prefix);
    }
}

static void LogIconViews(FrameworkElement const& host, PCWSTR prefix) {
    if (!g_settings.detailedLogging || !host) {
        return;
    }
    std::vector<FrameworkElement> icons;
    CollectVisibleIconViews(host, icons);
    for (auto const& icon : icons) {
        try {
            Wh_Log(
                L"[Discover]   %s IconView automationName=%s glyph=%s "
                L"size=%.1fx%.1f",
                prefix,
                AutomationProperties::GetName(icon).c_str(),
                DescribeElementGlyphs(icon).c_str(),
                icon.ActualWidth(),
                icon.ActualHeight());
        } catch (...) {
        }
    }
}

// ── Snapshot / restore ────────────────────────────────────────────────────

static ElementSnapshot CaptureElementState(FrameworkElement const& element) {
    ElementSnapshot snapshot;
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
    return snapshot;
}

static ElementSnapshot CaptureHost(FrameworkElement const& element,
                                   Grid const& trayGrid,
                                   int markerIndex) {
    ElementSnapshot snapshot = CaptureElementState(element);
    snapshot.visibleIconViews = CountVisibleIconViews(element);

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

static void RestoreElement(ElementSnapshot& snapshot) {
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
        Wh_Log(L"[Restore] Element restore failed");
    }
    snapshot = {};
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

    if (g_startSettleTimer) {
        try {
            g_startSettleTimer.Stop();
        } catch (...) {
        }
    }

    // Icon properties first (icons live inside the hosts).
    for (auto& snapshot : *g_iconSnapshots) {
        RestoreElement(snapshot);
    }
    g_iconSnapshots->clear();

    // Send the native hosts home, then take the owned group down.
    try {
        if (g_group) {
            while (g_group.Children().Size() > 0) {
                auto child =
                    g_group.Children().GetAt(0).try_as<FrameworkElement>();
                g_group.Children().RemoveAt(0);
                if (child && g_layoutGrid) {
                    g_layoutGrid.Children().Append(child);
                }
            }
            if (g_startLease.group) {
                if (!start_placement::Release(g_startLease)) {
                    Wh_Log(L"[Restore] Start placement lease was not live");
                }
            } else if (g_layoutGrid) {
                uint32_t index = 0;
                if (g_layoutGrid.Children().IndexOf(g_group, index)) {
                    g_layoutGrid.Children().RemoveAt(index);
                }
            }
        }
    } catch (...) {
        Wh_Log(L"[Restore] Group teardown failed");
    }
    g_group = nullptr;
    g_startLease = {};

    if (!g_columnLease.markerName.empty() && g_layoutGrid) {
        if (!lease_column::Release(g_layoutGrid, g_columnLease)) {
            Wh_Log(
                L"[Restore] Dedicated-column marker missing; "
                L"leaving columns untouched");
            g_columnLease = {};
        }
    }
    for (auto& snapshot : *g_hostSnapshots) {
        RestoreElement(snapshot);
    }
    g_hostSnapshots->clear();
    g_layoutGrid = nullptr;
    g_layoutApplied = false;
    Wh_Log(L"[Restore] Native utility layout restored");
}

// ── Layout application ────────────────────────────────────────────────────

// Margin-based placement participates in layout, so Windows anchors each
// utility's flyout at the element's real on-screen position.
static void ApplyLeafPlacement(FrameworkElement const& element,
                               double x, double y,
                               double width, double height) {
    element.Width(width);
    element.Height(height);
    element.MinWidth(0);
    element.MinHeight(0);
    element.MaxWidth(width);
    element.MaxHeight(height);
    element.HorizontalAlignment(HorizontalAlignment::Left);
    element.VerticalAlignment(VerticalAlignment::Top);
    element.Margin(Thickness{x, y, 0.0, 0.0});
    element.RenderTransform(nullptr);
}

struct IconTarget {
    FrameworkElement element{nullptr};
    double x = 0.0;
    double y = 0.0;
    double width = 0.0;
    double height = 0.0;
};

static bool ApplyLayout() {
    ClearHostWatchers();

    // After an in-place taskbar rebuild (TrayUI::StartTaskbar) the old XAML
    // tree is gone; drop stale references instead of restoring into it.
    if (!g_layoutApplied &&
        (!g_hostSnapshots->empty() || !g_iconSnapshots->empty())) {
        // We still own strong references to the old tree here, so revoke its
        // callbacks before releasing those references. Don't attempt full
        // placement restoration into a detached taskbar tree.
        if (g_trayLayoutToken && g_layoutGrid) {
            try {
                g_layoutGrid.LayoutUpdated(g_trayLayoutToken);
            } catch (...) {
            }
        }
        if (g_startLease.layoutToken && g_startLease.rootGrid) {
            try {
                g_startLease.rootGrid.LayoutUpdated(
                    g_startLease.layoutToken);
            } catch (...) {
            }
        }
        g_hostSnapshots->clear();
        g_iconSnapshots->clear();
        g_columnLease = {};
        g_startLease = {};
        g_group = nullptr;
        g_trayLayoutToken = {};
        g_layoutGrid = nullptr;
    }
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

    auto trayGridElement = tree_walk::FindDescendant(
        root, 20,
        [](FrameworkElement const& element) {
            return element.Name() == L"SystemTrayFrameGrid";
        });
    auto trayGrid = trayGridElement.try_as<Grid>();
    if (!trayGrid) {
        Wh_Log(L"[Apply] SystemTrayFrameGrid not found");
        return false;
    }

    if (g_settings.detailedLogging) {
        for (auto const& child : trayGrid.Children()) {
            auto element = child.try_as<FrameworkElement>();
            LogElement(element, L"tray child");
            LogIconViews(element, L"tray child");
        }
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
            static_cast<double>(g_settings.minimumTrayHeight)) {
        Wh_Log(
            L"[Apply] Tray height %.1f is below minimum %d",
            trayGrid.ActualHeight(),
            g_settings.minimumTrayHeight);
        return true;
    }

    // Parse the expression first so token spellings can be canonicalized
    // (aliases like "keyboard") and unknown tokens reported instead of
    // silently dropped.
    ngl::Config config;
    config.primaryAxis = g_settings.primaryAxis;
    config.spacing = static_cast<double>(g_settings.buttonSpacing);
    config.crossAlign = g_settings.crossAlign;

    ngl::Node exprRoot;
    if (!ngl::Parse(g_settings.layout, config.primaryAxis, exprRoot)) {
        Wh_Log(L"[Apply] Layout expression parse error; leaving native: %s",
               g_settings.layout.c_str());
        return true;
    }

    std::vector<std::wstring> leafTokens;
    CollectLeafTokens(exprRoot, leafTokens);
    std::vector<std::wstring> wantedTokens;
    for (auto const& raw : leafTokens) {
        auto canonical = CanonicalToken(raw);
        if (canonical.empty()) {
            Wh_Log(
                L"[Apply] Unknown layout token '%s' — valid: overflow, "
                L"emoji, touchKeyboard, penMenu, virtualTouchpad, "
                L"inputIndicator (aliases: chevron, keyboard, pen, "
                L"touchpad, input)",
                raw.c_str());
            continue;
        }
        if (std::find(wantedTokens.begin(), wantedTokens.end(),
                      canonical) == wantedTokens.end()) {
            wantedTokens.push_back(std::move(canonical));
        }
    }
    if (wantedTokens.empty()) {
        Wh_Log(L"[Apply] Layout expression names no known tokens");
        return true;
    }

    auto items = ResolveLayoutItems(trayGrid, overflowHost, mainStack,
                                    wantedTokens);
    if (items.empty()) {
        Wh_Log(L"[Apply] No layout items found");
        return true;
    }

    // Pixel-space placement from the parsed expression, sized natively
    // unless the user set explicit icon sizes.
    auto resolve = [&](std::wstring const& raw) -> ngl::Size {
        auto token = CanonicalToken(raw);
        for (auto const& item : items) {
            if (item.token == token) {
                double width = g_settings.buttonWidth > 0
                                   ? g_settings.buttonWidth
                                   : item.naturalW;
                double height = g_settings.buttonHeight > 0
                                    ? g_settings.buttonHeight
                                    : item.naturalH;
                return {width, height};
            }
        }
        return {};
    };

    std::vector<ngl::Placement> placements;
    ngl::Size total = ngl::Measure(exprRoot, config, resolve);
    ngl::Arrange(exprRoot, config, resolve, 0.0, 0.0, placements);
    if (placements.empty() || total.Empty()) {
        Wh_Log(L"[Apply] Layout produced no placements");
        return true;
    }

    // Per-element targets from the placements, plus stragglers: visible
    // icons in managed hosts that the expression didn't place get appended
    // after the group so nothing is ever lost or clipped.
    std::vector<IconTarget> targets;
    std::vector<FrameworkElement> managedHosts;
    for (auto const& placement : placements) {
        auto placementToken = CanonicalToken(placement.token);
        for (auto const& item : items) {
            if (item.token != placementToken) {
                continue;
            }
            targets.push_back({item.element,
                               placement.x + item.offsetX,
                               placement.y + item.offsetY,
                               placement.size.width,
                               placement.size.height});
            bool known = false;
            for (auto const& host : managedHosts) {
                if (host == item.host) {
                    known = true;
                    break;
                }
            }
            if (!known) {
                managedHosts.push_back(item.host);
            }
            break;
        }
    }

    auto isTargeted = [&](FrameworkElement const& element) {
        for (auto const& target : targets) {
            if (target.element == element) {
                return true;
            }
        }
        return false;
    };

    bool horizontal = config.primaryAxis == ngl::Axis::Horizontal;
    double extraCursor = horizontal ? total.width : total.height;
    for (auto const& host : managedHosts) {
        bool leafHost = false;
        for (auto const& item : items) {
            if (item.hostLeaf && item.host == host) {
                leafHost = true;
                break;
            }
        }
        if (leafHost) {
            continue;  // whole-host item; carries no separate targets
        }
        std::vector<FrameworkElement> icons;
        CollectVisibleIconViews(host, icons);
        for (auto const& icon : icons) {
            if (isTargeted(icon)) {
                continue;
            }
            double width = NaturalWidth(icon);
            double height = NaturalHeight(icon);
            IconTarget straggler;
            straggler.element = icon;
            straggler.width = width;
            straggler.height = height;
            if (horizontal) {
                straggler.x = extraCursor + config.spacing;
                straggler.y = std::max(0.0, (total.height - height) / 2.0);
                extraCursor = straggler.x + width;
            } else {
                straggler.y = extraCursor + config.spacing;
                straggler.x = std::max(0.0, (total.width - width) / 2.0);
                extraCursor = straggler.y + height;
            }
            targets.push_back(straggler);
        }
    }
    if (horizontal) {
        total.width = std::max(total.width, extraCursor);
    } else {
        total.height = std::max(total.height, extraCursor);
    }

    // From here on we mutate the tree: snapshot everything first.
    g_layoutGrid = trayGrid;
    g_layoutApplied = true;
    for (int i = 0; i < static_cast<int>(managedHosts.size()); i++) {
        g_hostSnapshots->push_back(
            CaptureHost(managedHosts[i], trayGrid, i));
    }

    // The owned group: one Grid the position options place; the native
    // hosts are reparented into it and each icon is margin-placed.
    Grid group;
    group.Name(kGroupName);
    group.Width(total.width);
    group.Height(total.height);

    bool startPosition =
        g_settings.position == Position::LeftOfStart ||
        g_settings.position == Position::RightOfStart;
    int sharedColumn = -1;

    if (startPosition) {
        auto side = g_settings.position == Position::LeftOfStart
                        ? start_placement::Side::Left
                        : start_placement::Side::Right;
        if (!start_placement::Acquire(
                root, group, side,
                std::max(0, g_settings.buttonSpacing), g_startLease)) {
            Wh_Log(
                L"[Apply] Start anchor unavailable; "
                L"leaving the native layout unchanged");
            RestoreLayout();
            return false;
        }
        // The Start counter-shift depends on whether Start rides the
        // task-repeater push; log the resolved geometry so a wrong gap
        // (before Start instead of beside it) is diagnosable from one run.
        try {
            auto transform = g_startLease.startButton.TransformToVisual(
                g_startLease.rootGrid);
            auto point = transform.TransformPoint({0.0f, 0.0f});
            Wh_Log(
                L"[Start] side=%s inRepeater=%d start=(%.1f,%.1f "
                L"%.1fx%.1f) groupMargin=(%.1f,%.1f) root=%.1fx%.1f",
                side == start_placement::Side::Left ? L"left" : L"right",
                g_startLease.startInTaskItemsPanel,
                point.X,
                point.Y,
                g_startLease.startButton.ActualWidth(),
                g_startLease.startButton.ActualHeight(),
                g_startLease.group.Margin().Left,
                g_startLease.group.Margin().Top,
                g_startLease.rootGrid.ActualWidth(),
                g_startLease.rootGrid.ActualHeight());
        } catch (...) {
        }
        sharedColumn = 0;
    } else {
        int column = -1;
        if (g_settings.position == Position::Overflow ||
            g_settings.position == Position::Emoji) {
            FrameworkElement anchorHost = overflowHost;
            if (g_settings.position == Position::Emoji) {
                for (auto const& item : items) {
                    if (item.token == L"emoji") {
                        anchorHost = item.host;
                        break;
                    }
                }
            }
            column = Grid::GetColumn(anchorHost);
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
                    anchor = lease_column::Anchor::AfterShowDesktop;
                    break;
                default:
                    break;
            }
            if (!lease_column::Acquire(trayGrid, anchor,
                                       kLayoutColumnMarkerName,
                                       g_columnLease)) {
                Wh_Log(
                    L"[Apply] Requested position anchor unavailable; "
                    L"leaving the native layout unchanged");
                RestoreLayout();
                return false;
            }
            column = g_columnLease.column;
        }

        Grid::SetColumn(group, column);
        group.HorizontalAlignment(HorizontalAlignment::Center);
        group.VerticalAlignment(VerticalAlignment::Center);
        trayGrid.Children().Append(group);
        sharedColumn = column;
    }
    g_group = group;

    // Reparent the involved hosts into the group. The group is a plain
    // Grid, so the hosts overlap; blank host regions have no background
    // and stay hit-test transparent, so icons of one host remain clickable
    // through another host's empty area.
    for (auto const& host : managedHosts) {
        uint32_t index = 0;
        if (trayGrid.Children().IndexOf(host, index)) {
            trayGrid.Children().RemoveAt(index);
        }
        group.Children().Append(host);
    }

    double groupOffsetX = static_cast<double>(g_settings.groupOffsetX);
    double groupOffsetY = static_cast<double>(g_settings.groupOffsetY);

    for (auto const& host : managedHosts) {
        // Host-leaf items (the chevron, MainStack fallback) are placed as
        // a whole; icon hosts span the group and their icons are placed
        // individually with flow-compensating margins.
        LayoutItem const* leafItem = nullptr;
        for (auto const& item : items) {
            if (item.hostLeaf && item.host == host) {
                leafItem = &item;
                break;
            }
        }

        if (leafItem) {
            for (auto const& target : targets) {
                if (target.element == leafItem->element) {
                    ApplyLeafPlacement(host,
                                       target.x + groupOffsetX,
                                       target.y + groupOffsetY,
                                       target.width, target.height);
                    break;
                }
            }
            continue;
        }

        ApplyLeafPlacement(host, 0.0, 0.0, total.width, total.height);
        host.Margin(Thickness{groupOffsetX, groupOffsetY, 0.0, 0.0});

        // Windows lays the host's icons out in one horizontal flow. Each
        // icon's Left margin steers it from where the flow would put it to
        // its target, and the flow position advances by the arranged
        // extent (never negative — XAML floors desired size at zero).
        std::vector<FrameworkElement> icons;
        CollectVisibleIconViews(host, icons);
        double flow = 0.0;
        for (auto const& icon : icons) {
            IconTarget const* target = nullptr;
            for (auto const& candidate : targets) {
                if (candidate.element == icon) {
                    target = &candidate;
                    break;
                }
            }
            if (!target) {
                continue;
            }
            g_iconSnapshots->push_back(CaptureElementState(icon));
            icon.Width(target->width);
            icon.Height(target->height);
            icon.MinWidth(0);
            icon.MinHeight(0);
            icon.MaxWidth(target->width);
            icon.MaxHeight(target->height);
            icon.VerticalAlignment(VerticalAlignment::Top);
            double marginLeft = target->x - flow;
            icon.Margin(Thickness{marginLeft, target->y, 0.0, 0.0});
            icon.RenderTransform(nullptr);
            flow += std::max(0.0, marginLeft + target->width);
        }
    }

    // Visibility watchers miss icons appearing or vanishing inside a host,
    // so verify on tray layout passes that every managed host is intact
    // and its visible icon count is unchanged; any drift re-runs layout.
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
            for (auto const& snapshot : *g_hostSnapshots) {
                if (!snapshot.element) {
                    continue;
                }
                bool changed = false;
                try {
                    changed =
                        !VisualTreeHelper::GetParent(snapshot.element) ||
                        snapshot.element.Visibility() !=
                            Visibility::Visible;
                    if (!changed) {
                        changed =
                            CountVisibleIconViews(snapshot.element) !=
                            snapshot.visibleIconViews;
                    }
                } catch (...) {
                    changed = true;
                }
                if (changed) {
                    ScheduleReapply();
                    return;
                }
            }
        });

    if (startPosition && g_startLease.group) {
        // Removing the hosts shrinks the tray and the centered taskbar
        // re-flows — partly through an ANIMATION, so both an immediate
        // forced layout pass and a deferred re-position are needed for
        // the group to land where Start actually settles.
        try {
            g_startLease.rootGrid.UpdateLayout();
        } catch (...) {
        }
        start_placement::Position(g_startLease);
        try {
            if (!g_startSettleTimer) {
                g_startSettleTimer = DispatcherTimer();
                g_startSettleTimer.Interval(
                    std::chrono::milliseconds{600});
                g_startSettleTimer.Tick(
                    [](auto const&, auto const&) {
                        if (g_startSettleTimer) {
                            g_startSettleTimer.Stop();
                        }
                        if (!g_unloading && g_startLease.group) {
                            start_placement::Position(g_startLease);
                        }
                    });
            }
            g_startSettleTimer.Stop();
            g_startSettleTimer.Start();
        } catch (...) {
        }
    }

    Wh_Log(
        L"[Apply] Layout applied: items=%d targets=%d hosts=%d "
        L"trayColumn=%d dedicated=%d groupSize=%.0fx%.0f trayHeight=%.1f "
        L"expr=%s",
        static_cast<int>(items.size()),
        static_cast<int>(targets.size()),
        static_cast<int>(managedHosts.size()),
        sharedColumn,
        !g_columnLease.markerName.empty(),
        total.width,
        total.height,
        trayGrid.ActualHeight(),
        g_settings.layout.c_str());
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

static void StartRetryThread(bool forceApply = false);

// Explorer can rebuild the taskbar in place (TrayUI::StartTaskbar); the old
// XAML tree and our snapshots are gone, so restart the bounded retry
// (lifecycle v1.3 contract).
using TrayUI_StartTaskbar_t = void (WINAPI*)(void*);
static TrayUI_StartTaskbar_t TrayUI_StartTaskbar_Original;

static void WINAPI TrayUI_StartTaskbar_Hook(void* pThis) {
    TrayUI_StartTaskbar_Original(pThis);
    if (!g_unloading) {
        Wh_Log(L"[Hooks] TrayUI::StartTaskbar; rescheduling layout");
        g_taskbarWnd = nullptr;
        g_layoutApplied = false;
        StartRetryThread();
    }
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
        {{LR"(public: virtual void __cdecl TrayUI::StartTaskbar(void))"},
         &TrayUI_StartTaskbar_Original, TrayUI_StartTaskbar_Hook},
    };
    return WindhawkUtils::HookSymbols(
        module, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks));
}

static void StopRetryThread() {
    AcquireSRWLockExclusive(&g_retryLock);
    HANDLE retryThread = g_retryThread;
    HANDLE retryStopEvent = g_retryStopEvent;
    g_retryThread = nullptr;
    g_retryStopEvent = nullptr;
    if (retryStopEvent) {
        SetEvent(retryStopEvent);
    }
    ReleaseSRWLockExclusive(&g_retryLock);

    if (retryThread) {
        DWORD result;
        do {
            result = MsgWaitForMultipleObjects(
                1, &retryThread, FALSE, INFINITE, QS_SENDMESSAGE);
            if (result == WAIT_OBJECT_0 + 1) {
                MSG message;
                PeekMessageW(&message, nullptr, 0, 0, PM_NOREMOVE);
            }
        } while (result == WAIT_OBJECT_0 + 1);
        CloseHandle(retryThread);
    }
    if (retryStopEvent) {
        CloseHandle(retryStopEvent);
    }
}

static void StartRetryThread(bool forceApply) {
    StopRetryThread();
    AcquireSRWLockExclusive(&g_retryLock);
    if (g_unloading || g_retryThread || g_retryStopEvent) {
        if (forceApply) {
            g_retryForceApply = true;
        }
        ReleaseSRWLockExclusive(&g_retryLock);
        return;
    }
    g_retryForceApply = forceApply;
    g_retryStopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_retryStopEvent) {
        ReleaseSRWLockExclusive(&g_retryLock);
        return;
    }
    HANDLE stopEvent = g_retryStopEvent;
    g_retryThread = CreateThread(
        nullptr,
        0,
        [](void* parameter) -> DWORD {
            HANDLE stopEvent = reinterpret_cast<HANDLE>(parameter);
            for (int attempt = 0;
                 attempt < 6 && !g_unloading;
                 attempt++) {
                bool forceApply = g_retryForceApply.exchange(false);
                if (g_layoutApplied && !forceApply) {
                    break;
                }
                if (attempt &&
                    WaitForSingleObject(stopEvent, 1500) !=
                        WAIT_TIMEOUT) {
                    break;
                }
                if (attempt) {
                    Wh_Log(L"[Retry] Layout attempt %d", attempt);
                }
                ApplyLayoutOnWindowThread();
            }
            return 0;
        },
        stopEvent,
        0,
        nullptr);
    if (!g_retryThread) {
        CloseHandle(g_retryStopEvent);
        g_retryStopEvent = nullptr;
    }
    ReleaseSRWLockExclusive(&g_retryLock);
}

BOOL Wh_ModInit() {
    Wh_Log(L"[Init] Tray Utility Customizer v1.1");
    LoadSettings();

    if (!HookTaskbarDllSymbols()) {
        Wh_Log(L"[Init] taskbar.dll symbol hooks failed");
        return FALSE;
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    // One attempt covers loading into an already-running taskbar. Startup and
    // rebuilds use the bounded TrayUI::StartTaskbar retry path.
    ApplyLayoutOnWindowThread();
}

void Wh_ModSettingsChanged() {
    StopRetryThread();
    LoadSettings();
    Wh_Log(L"[Settings] Reapplying");
    // Settings can land during a transient taskbar rebuild. The retry path is
    // now handle-race-safe, so use it instead of silently losing the reapply.
    StartRetryThread(true);
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
                ClearHostWatchers();
                if (g_reapplyTimer) {
                    try {
                        g_reapplyTimer.Stop();
                    } catch (...) {
                    }
                    g_reapplyTimer = nullptr;
                }
                if (g_startSettleTimer) {
                    try {
                        g_startSettleTimer.Stop();
                    } catch (...) {
                    }
                    g_startSettleTimer = nullptr;
                }
                RestoreLayout();
                g_hostWatchers.reset();
                g_iconSnapshots.reset();
                g_hostSnapshots.reset();
            },
            nullptr);
    } else {
        // Intentionally retain all no_destroy XAML/WinRT holders. There is
        // no known UI thread on which releasing them would be safe
        // (lifecycle v1.3 process-shutdown contract).
        Wh_Log(L"[Uninit] No taskbar UI thread; retaining XAML state");
    }
}
