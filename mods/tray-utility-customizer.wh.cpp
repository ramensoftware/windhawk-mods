// ==WindhawkMod==
// @id              tray-utility-customizer
// @name            Tray Utility Customizer
// @description     Granular per-icon control over the Windows tray utility icons — Show hidden icons, Emoji, touch keyboard, pen menu, virtual touchpad, and input/language indicator — arranged by one nestable layout expression.
// @version         2.0
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
*One row of three at native size — what `auto` produces on a single-height taskbar, and what `overflow | emoji | touchKeyboard` produces anywhere.*

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

## Upgrading from 1.x

Version 2.0 groups the settings under `Placement`, `Content`, `Layout`, `Size`,
`Adjust`, and `Behavior`. Windhawk cannot carry a value across a renamed
setting, so **after updating, the mod starts from the 2.0 defaults until you
re-apply your settings once.** Before updating, copy your settings from the
mod's Settings page in **Textual mode**; afterwards, re-enter them in their new
groups.

Two things are worth knowing before you retype your layout:

- **The primary-axis setting is gone.** `|` is now ALWAYS horizontal and `,` is
  ALWAYS vertical, at every depth. If you had Primary axis set to **Column**,
  your old expression means its transpose under the new grammar — swap `|` and
  `,` when you re-enter it. If you were on Row, the string means exactly what
  it did before.
- **The twelve per-icon nudge settings are gone.** A nudge now rides in the
  arrangement itself: `emoji[+2,-1]`. One string, nothing to keep in sync.

## Arrangement

One string describes the whole layout, under `Layout` → `Arrangement`:

- `|` places items **side by side**, always
- `,` stacks them **on top of each other**, always
- parentheses nest, to any depth
- `name[dx,dy]` nudges one item; `(a, b)[dx,dy]` nudges a whole group
- every group is centered against its siblings (see `Layout.Justify`)

Examples:

- `overflow | emoji | touchKeyboard` — one row of three icons
- `overflow, emoji, touchKeyboard` — a single column
- `overflow | emoji, touchKeyboard` — chevron beside a stacked pair
- `overflow | emoji, touchKeyboard | penMenu` — the diamond: two icons
  flanking a stacked middle column
- `overflow[0,-2] | emoji` — the same row with the chevron nudged up 2px

The default is the word **`auto`**, which fits the utilities you enabled to
the taskbar's height: it takes the fewest columns that fit in the rows
available, and `Layout.FillOrder` decides whether items fill across or down.
Every time `auto` runs it writes the expression it generated to the Windhawk
log, so you can paste that into the field and edit it.

A separator is always required — `overflow (emoji | touchKeyboard)` is a parse
error rather than an implied `|`, so a typo shows up in the log instead of
silently becoming a different layout. A parse error falls back to `auto`.

Tokens accept forgiving aliases: `chevron`/`hidden` for `overflow`,
`keyboard` for `touchKeyboard`, `pen` for `penMenu`, `touchpad` for
`virtualTouchpad`, and `input`/`language` for `inputIndicator`. An unknown
token is named in the log rather than silently dropped.

**Items your arrangement does not name.** Windows shows and hides these
utilities live — the touch keyboard comes and goes, and the taskbar settings
toggle the rest — so an arrangement you wrote earlier can be missing one.
`Layout.NewItems` decides what happens then: **append** (the default) arranges
them after what you wrote and logs that it did, and **ignore** leaves them out
until you add them yourself.

## Which utilities participate

The `Content` group has one switch per utility. All six are on by default:
a utility Windows is not currently showing contributes nothing either way, and
on most machines the pen menu, virtual touchpad and input indicator are simply
absent. The overflow chevron has its own host, so turning it off leaves it
native. The other utilities share Windows' `MainStack`: if any sibling there is
enabled, a disabled sibling travels with that host and is parked after the
arrangement rather than staying at its original tray position.

## Size and adjustment

Icons render at their native size unless you set `Size.ItemWidth` /
`Size.ItemHeight`. A tall column of native-size icons can overhang a
single-height taskbar; about 16 px makes it fit.

`Size.ItemSpacing` is the gap between items and may be negative to pull them
together. `Adjust.PadX` / `PadY` reserve space at the outside edges of the
group and participate in layout — raising `PadY` gives `auto` fewer rows to
work with. `Adjust.OffsetX` / `OffsetY` move the whole group visually and
reserve nothing.

## Position

`Placement.Position` puts the group in the hidden-icons column (the default —
that is where these utilities already are), the Emoji column, or a dedicated
leased tray column before the notification icons, before Wi-Fi/volume/battery,
before or after the clock, or after the Show Desktop strip. The lease is
marker-tracked and fully reversible on unload.

Two **experimental** positions relocate the group out of the tray entirely:
**Left of Start** and **Right of Start** place it beside the Start button and
push the task list right to reserve room. The group follows Start as the
taskbar re-centers. Primary taskbar only.

## Detection

Icons are identified by Windows' language-neutral runtime data-model classes,
XAML names and content types, Automation IDs, and stable Segoe Fluent glyphs.
Detection doesn't depend on translated accessibility labels.
`Behavior.Detection` → **Force MainStack** allows the complete native
`MainStack` to participate as the `emoji` item when Windows doesn't expose a
distinct identity.

## Settings

| Setting | Default | What it does |
|---|---|---|
| `Placement.Position` | `overflow` | Which tray column (or Start-adjacent spot) the group occupies |
| `Content.*` | all on | One switch per utility; MainStack siblings travel with an enabled sibling |
| `Layout.Arrangement` | `auto` | The layout expression, or `auto` |
| `Layout.FillOrder` | `rows` | Used by `auto`: fill across rows or down columns |
| `Layout.Justify` | `center` | How a ragged row or column aligns against its siblings |
| `Layout.NewItems` | `append` | What happens to a utility a written arrangement doesn't name |
| `Size.ItemWidth` | `0` | 0 = the size Windows drew it at |
| `Size.ItemHeight` | `0` | 0 = native; ~16 fits a column on a single-height taskbar |
| `Size.ItemSpacing` | `0` | Gap between items; negative pulls them together |
| `Adjust.PadX` / `PadY` | `0` | Space reserved at the group's edges; participates in layout |
| `Adjust.OffsetX` / `OffsetY` | `0` | Moves the group visually; reserves nothing |
| `Behavior.MinimumTrayHeight` | `44` | Below this tray height the mod leaves everything native |
| `Behavior.Detection` | `auto` | Guarded detection, or Force MainStack |

## Taskbar position

Windows 11 only puts the taskbar at the bottom, but two mods move it:

- **[taskbar-on-top](https://windhawk.net/mods/taskbar-on-top) — supported.**
  Nothing here positions against screen coordinates; everything is relative to
  the taskbar's own XAML tree.
- **[taskbar-vertical](https://windhawk.net/mods/taskbar-vertical) — not
  compatible.** It rotates the same tray elements this mod positions, through
  the same `RenderTransform` property. One property, two owners. This mod
  detects a vertical taskbar, **leaves it completely untouched**, and says so
  in the log rather than painting a rotated mess.

## Known limitations

- The utility flyouts (the Emoji panel, the hidden-icons overflow) are
  positioned by Windows itself from the icon's location; at extreme
  screen-edge positions they can open partially off-screen. Prefer the
  tray positions if this bothers you.
- Left/Right of Start are experimental. The centered taskbar re-flows with
  an animation, and the group can briefly sit at a stale position until
  the taskbar's next layout pass settles it.

## Changelog

### 2.0

- Adopted the grouped `Placement` / `Content` / `Layout` / `Size` / `Adjust` /
  `Behavior` settings contract. 1.x settings are not read; re-apply them once
  after updating (see "Upgrading from 1.x").
- One `Layout.Arrangement` field replaces the layout expression, the primary
  axis, the group alignment, and all twelve per-icon nudge settings. `|` is
  always horizontal and `,` always vertical; nudges ride in the expression.
- Added `auto`, which fits the enabled utilities to the taskbar height and
  logs the expression it generated so it can be pasted back and edited.
- Added `Content` switches per utility and `Layout.NewItems`, so a utility
  that appears after you wrote your arrangement is not silently lost.
- Added `Adjust.PadX` / `PadY` / `OffsetX` / `OffsetY`.
- Row capacity is now computed in DIPs from the taskbar's real DPI instead of
  raw pixels, so the automatic shape is correct at 125% and 150% scaling.
- A vertical taskbar is now detected and the mod stands down completely
  instead of arranging into a rotated coordinate space.
- Restoring a borrowed element now puts back its exact previous local value,
  or clears the property when it had none, instead of writing back a value
  read from the live element. A tray element whose size or alignment came from
  its template keeps that binding when the mod unloads.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- Placement:
  - Position: "overflow"
    $name: Position
    $description: >-
      Where the utility group lives: a native utility column, a dedicated
      leased tray column, or experimentally beside Start. The default is the
      hidden-icons column because that is where these utilities already are -
      the group takes the space it is already using instead of asking the
      tray for more.
    $options:
    - "overflow": "Hidden-icons column (the chevron's own column)"
    - "emoji": "Emoji column"
    - "beforeIcons": "Before notification icons"
    - "beforeOmni": "Before network, volume, and battery"
    - "beforeClock": "Before clock"
    - "afterClock": "After clock"
    - "afterShowDesktop": "After Show Desktop"
    - "leftOfStart": "Left of Start (experimental)"
    - "rightOfStart": "Right of Start (experimental)"
  $name: Placement

- Content:
  - Overflow: true
    $name: Show hidden icons (the chevron)
    $description: >-
      Include this utility in the arrangement. A utility Windows is not
      currently showing contributes nothing either way - pen menu, virtual
      touchpad and the input indicator are absent on most machines.
  - Emoji: true
    $name: Emoji and more
  - TouchKeyboard: true
    $name: Touch keyboard
  - PenMenu: true
    $name: Pen menu
  - VirtualTouchpad: true
    $name: Virtual touchpad
  - InputIndicator: true
    $name: Input/language indicator
  $name: Content

- Layout:
  - Arrangement: "auto"
    $name: Arrangement
    $description: >-
      "auto" fits the utilities you enabled above to the taskbar height.
      Anything else is an explicit layout: names side by side with "|",
      stacked with ",", and grouped with parentheses - "overflow, emoji |
      touchKeyboard" is the chevron over Emoji beside the touch keyboard.
      "|" is ALWAYS horizontal and "," is ALWAYS vertical, at every depth.
      Tokens are overflow, emoji, touchKeyboard, penMenu, virtualTouchpad and
      inputIndicator (aliases: chevron, hidden, keyboard, pen, touchpad,
      input, language). Append a pixel offset to nudge one item,
      "emoji[+2,-1]", or a whole group, "(overflow, emoji)[3,0]". Every time
      "auto" is applied its generated arrangement is written to the Windhawk
      log, so you can paste it here and edit it. A parse error is logged and
      falls back to automatic.
  - FillOrder: "rows"
    $name: Fill order
    $description: Used by "auto". Whether items fill across rows or down columns first.
    $options:
    - "rows": "Fill rows first (left to right, then down)"
    - "columns": "Fill columns first (top to bottom, then right)"
  - Justify: "center"
    $name: Short row or column
    $description: How a ragged row or column is aligned against its siblings.
    $options:
    - "start": "Start"
    - "center": "Center"
    - "end": "End"
  - NewItems: "append"
    $name: Items your arrangement does not name
    $description: >-
      Windows shows and hides these utilities live - the touch keyboard comes
      and goes, and the taskbar settings toggle the rest - so an arrangement
      you wrote earlier can be missing one. Only applies to a written
      arrangement; "auto" always includes every enabled utility Windows is
      currently showing.
    $options:
    - "append": "Add them after the arrangement"
    - "ignore": "Leave them out until I add them"
  $name: Layout

- Size:
  - ItemWidth: 0
    $name: Item width (px, 0 = native size)
    $description: >-
      0 gives each utility the width Windows drew it at. A number puts every
      item in a fixed box of that width instead, which lines columns up but
      adds dead space around a narrower glyph.
  - ItemHeight: 0
    $name: Item height (px, 0 = native size)
    $description: >-
      0 uses the native size. A tall column of native-size icons can overhang
      a single-height taskbar; about 16 here makes it fit.
  - ItemSpacing: 0
    $name: Item spacing (px)
    $description: >-
      Gap between items along each axis. Negative pulls them together and may
      overlap. THIS is what tightens the cluster - horizontal padding only
      reserves space at the two outside edges and can never change the
      distance between items.
  $name: Size

- Adjust:
  - PadX: 0
    $name: Horizontal padding (px)
    $description: Space reserved on both sides of the group. Participates in layout.
  - PadY: 0
    $name: Vertical padding (px)
    $description: >-
      Space reserved above and below the group. Reserved before the
      arrangement divides the taskbar height, so raising it gives "auto"
      fewer rows to work with.
  - OffsetX: 0
    $name: Horizontal offset (px)
    $description: Moves the whole group. Does not reserve space.
  - OffsetY: 0
    $name: Vertical offset (px)
    $description: Moves the whole group up (negative) or down (positive).
  $name: Adjust

- Behavior:
  - MinimumTrayHeight: 44
    $name: Minimum tray height (px)
    $description: >-
      Below this height the mod leaves the native layout unchanged. Use 0 to
      allow rearranging on any taskbar height.
  - Detection: "auto"
    $name: Detection mode
    $description: >-
      Automatic is guarded and recommended. Force allows the complete native
      MainStack to participate as the emoji item when Windows doesn't
      identify its controls.
    $options:
    - "auto": "Automatic"
    - "forceMainStack": "Force MainStack (experimental)"
  $name: Behavior
*/
// ==/WindhawkModSettings==

#include <atomic>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cwchar>
#include <cwctype>
#include <exception>
#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>
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

// ==ModComponents==
// Self-contained building blocks this mod is built from. Each
// section below is one contract in its own namespace; the mod's own
// code begins after them.

// -- Settings values --------------------------------------------------------
// Clamped int/bool setting reads, fixed-buffer string reads, and the
// $options choice table - so a renamed option fails loudly instead of
// silently falling back.
namespace tray_utility_settings {

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

}  // namespace tray_utility_settings

// -- Arrangement expression -------------------------------------------------
// One user-typed string - names joined by '|' (side by side) and ','
// (stacked), nested with parentheses, nudged with [dx,dy] - parsed,
// measured and arranged into concrete placements. Includes the automatic
// grid shape and the policy for items a written arrangement does not name.
namespace tray_utility_layout {

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

}  // namespace tray_utility_layout

// -- Visual-tree walk -------------------------------------------------------
// Depth-bounded descendant visit, find-first and collect-all over a XAML
// subtree.
namespace tray_utility_tree_walk {

using winrt::Windows::UI::Xaml::FrameworkElement;
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

}  // namespace tray_utility_tree_walk

// -- Property lease ---------------------------------------------------------
// Snapshot a dependency property's exact prior local value before writing
// it, and put that value back - or clear the property when there was none -
// on unload.
namespace tray_utility_property_lease {

using winrt::Windows::Foundation::IInspectable;
using winrt::Windows::UI::Xaml::DependencyObject;
using winrt::Windows::UI::Xaml::DependencyProperty;

struct Snapshot {
    DependencyObject object{nullptr};
    DependencyProperty property{nullptr};
    IInspectable localValue{nullptr};
};

// Reported per failed restore so the mod can log in its own voice.
using RestoreErrorFn = std::function<void()>;

class Lease {
public:
    // Announce a mutation BEFORE making it. Safe to call repeatedly; only the
    // first call for a given (object, property) records anything.
    void Track(DependencyObject const& object,
               DependencyProperty const& property) {
        if (!object || !property) return;
        for (auto const& snapshot : snapshots_) {
            if (snapshot.object == object && snapshot.property == property)
                return;
        }
        snapshots_.push_back(
            {object, property, object.ReadLocalValue(property)});
    }

    // Put everything back, newest first, and forget it. Call on the UI thread.
    void RestoreAll(RestoreErrorFn const& onError = {}) {
        for (auto it = snapshots_.rbegin(); it != snapshots_.rend(); ++it) {
            try {
                if (it->localValue == DependencyProperty::UnsetValue())
                    it->object.ClearValue(it->property);
                else
                    it->object.SetValue(it->property, it->localValue);
            } catch (...) {
                if (onError) onError();
            }
        }
        snapshots_.clear();
    }

    // Put ONE object's properties back and forget them, leaving every other
    // object's snapshots alone. For the case where a mod discovers that an
    // element it began borrowing was never actually its business — handing
    // that element back has to be possible without ending the whole lease.
    void RestoreObject(DependencyObject const& object,
                       RestoreErrorFn const& onError = {}) {
        if (!object) return;
        for (auto it = snapshots_.rbegin(); it != snapshots_.rend();) {
            if (it->object != object) {
                ++it;
                continue;
            }
            try {
                if (it->localValue == DependencyProperty::UnsetValue())
                    it->object.ClearValue(it->property);
                else
                    it->object.SetValue(it->property, it->localValue);
            } catch (...) {
                if (onError) onError();
            }
            // Erase through the reverse iterator without invalidating the
            // traversal: base() points one past the element being erased.
            it = std::make_reverse_iterator(snapshots_.erase(
                std::next(it).base()));
        }
    }

    // Drop the snapshots WITHOUT restoring. For the case where the elements
    // are already gone (an Explorer rebuild threw the tree away), so restoring
    // would only throw. Do not use it to "skip" a restore that could run.
    void Abandon() { snapshots_.clear(); }

    size_t Count() const { return snapshots_.size(); }
    bool Empty() const { return snapshots_.empty(); }

private:
    std::vector<Snapshot> snapshots_;
};

}  // namespace tray_utility_property_lease

// -- Taskbar window discovery -----------------------------------------------
// Find this process's Shell_TrayWnd, and validate a cached handle before
// preferring it.
namespace tray_utility_taskbar_window {

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

}  // namespace tray_utility_taskbar_window

// -- UI-thread dispatch -----------------------------------------------------
// Marshal a callback onto the taskbar's UI thread with a CALLWNDPROC hook
// and a private registered message, reporting whether it actually ran.
namespace tray_utility_dispatch {

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

}  // namespace tray_utility_dispatch

// -- Taskbar XamlRoot -------------------------------------------------------
// Hook the taskbar.dll symbols, reach the taskbar's XamlRoot, and call back
// when Explorer rebuilds the taskbar in place.
namespace dispatch = tray_utility_dispatch;
namespace tray_utility_taskbar_xaml {

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

}  // namespace tray_utility_taskbar_xaml

// -- Taskbar metrics and orientation ----------------------------------------
// The taskbar's rect in DIPs, and whether a horizontal-layout model applies
// at all - so a mod stands down on a vertical taskbar instead of arranging
// into a rotated coordinate space.
namespace tray_utility_taskbar_metrics {

// ---- Taskbar metrics and orientation ----------------------------------------
//
// WHERE THE TASKBAR IS, AND WHETHER THIS MOD CAN WORK THERE.
//
// Windows 11 itself only puts the taskbar at the bottom. Two mods by m417z
// move it, and both are first-class parts of the ecosystem this mod has to
// live in:
//
//   taskbar-on-top       — bottom -> top. FINE here. Everything
//                          here is positioned relative to the taskbar's own
//                          XAML tree, never to screen coordinates, so a top
//                          taskbar is the same tree at a different y.
//
//   taskbar-vertical     — bottom -> left/right. NOT COMPATIBLE, and not for
//                          a reason cooperation can fix. It walks the very
//                          same path this mod walks
//                          (ControlCenterButton > Grid > ContentPresenter >
//                          ItemsPresenter > StackPanel) and applies a
//                          RotateTransform to `RenderTransform` on those
//                          children. Positioning here sets a
//                          TranslateTransform on the SAME property of the SAME
//                          elements. One dependency property, two owners, last
//                          writer wins — the two layouts cannot coexist.
//
// So: DETECT AND STAND DOWN, loudly, rather than fight and paint garbage. The
// detection is the taskbar's own rect aspect, not a check for a specific mod —
// it is the condition that matters, and it stays true however the taskbar got
// that way.
//
// The rect is in PHYSICAL pixels and every XAML size is a DIP, so conversion
// belongs here instead of being re-derived at each call site.

enum class Orientation { Horizontal, Vertical };

struct Metrics {
    bool valid = false;
    RECT rect{};
    UINT dpi = 96;
    Orientation orientation = Orientation::Horizontal;
    // The extent the arranged group has to fit INTO: the taskbar's height when
    // it runs across the screen, its width when it runs down the side.
    double constrainedDip = 0.0;
    // The extent it can run ALONG.
    double alongDip = 0.0;
};

inline Metrics GetMetrics(HWND taskbarWnd) {
    Metrics metrics;
    if (!taskbarWnd || !GetWindowRect(taskbarWnd, &metrics.rect))
        return metrics;

    metrics.valid = true;
    metrics.dpi = GetDpiForWindow(taskbarWnd);
    if (!metrics.dpi) metrics.dpi = 96;

    double width = (double)(metrics.rect.right - metrics.rect.left);
    double height = (double)(metrics.rect.bottom - metrics.rect.top);
    double scale = 96.0 / (double)metrics.dpi;

    // Taller than wide means it runs down a side. Nothing else can produce
    // that shape, so this needs no cooperation from whatever moved it.
    metrics.orientation =
        height > width ? Orientation::Vertical : Orientation::Horizontal;
    if (metrics.orientation == Orientation::Horizontal) {
        metrics.constrainedDip = height * scale;
        metrics.alongDip = width * scale;
    } else {
        metrics.constrainedDip = width * scale;
        metrics.alongDip = height * scale;
    }
    return metrics;
}

// Whether this mod's layout model applies at all. Checked BEFORE touching
// anything, so a taskbar it does not describe is left exactly as it was found
// rather than arranged into a coordinate space someone else is rotating.
inline bool LayoutModelApplies(Metrics const& metrics) {
    return metrics.valid && metrics.orientation == Orientation::Horizontal;
}

inline wchar_t const* OrientationName(Orientation orientation) {
    return orientation == Orientation::Vertical ? L"vertical" : L"horizontal";
}

}  // namespace tray_utility_taskbar_metrics

// -- Bounded retry loop -----------------------------------------------------
// A stoppable, waited worker that retries an apply a bounded number of
// times. Safe against a Stop from one thread racing a Start from another.
namespace tray_utility_retry {

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

}  // namespace tray_utility_retry

// -- Tray slot lease --------------------------------------------------------
// Lease a position in the tray panel - a column on a Grid, a child index on
// a StackPanel - tracked by a named zero-size marker so the release stays
// exact after other mods inject siblings around it.
namespace tray_utility_slot_lease {

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

}  // namespace tray_utility_slot_lease

// -- Start-adjacent lane placement ------------------------------------------
// Place owned content beside the Start button and reserve room for it by
// pushing the task list, following Start as a centered taskbar re-flows.
// Reserves a lane; it does not overlay Start.
namespace tray_utility_start_placement {

using winrt::Windows::UI::Xaml::DependencyObject;
using winrt::Windows::UI::Xaml::DependencyProperty;
using winrt::Windows::UI::Xaml::FrameworkElement;
using winrt::Windows::Foundation::IInspectable;
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
    IInspectable taskItemsPanelMarginLocal{nullptr};
    IInspectable startRenderTransformLocal{nullptr};
    bool startInTaskItemsPanel = false;
    winrt::event_token layoutToken{};
    Side side = Side::Left;
    double spacing = 0.0;
};

inline void RestoreLocalValue(DependencyObject const& object,
                              DependencyProperty const& property,
                              IInspectable const& value) {
    if (value == DependencyProperty::UnsetValue()) {
        object.ClearValue(property);
    } else {
        object.SetValue(property, value);
    }
}

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
            RestoreLocalValue(lease.startButton,
                              UIElement::RenderTransformProperty(),
                              lease.startRenderTransformLocal);
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

        // Center against the taskbar root; Start's own box is not a reliable
        // vertical reference.
        double rootHeight = lease.rootGrid.ActualHeight();
        double startCenteredTop = point.Y + (startHeight - groupHeight) / 2.0;
        double top = rootHeight > 0.0 ? (rootHeight - groupHeight) / 2.0
                                      : startCenteredTop;
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
            RestoreLocalValue(lease.taskItemsPanel,
                              FrameworkElement::MarginProperty(),
                              lease.taskItemsPanelMarginLocal);
        if (lease.startButton)
            RestoreLocalValue(lease.startButton,
                              UIElement::RenderTransformProperty(),
                              lease.startRenderTransformLocal);
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
    lease.startRenderTransformLocal = startButton.ReadLocalValue(
        UIElement::RenderTransformProperty());
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
        lease.taskItemsPanelMarginLocal = lease.taskItemsPanel.ReadLocalValue(
            FrameworkElement::MarginProperty());
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

}  // namespace tray_utility_start_placement

// ==/ModComponents==

// Short names for the assembled components above.
namespace sio = tray_utility_settings;
namespace ngl = tray_utility_layout;
namespace tree_walk = tray_utility_tree_walk;
namespace please = tray_utility_property_lease;
namespace taskbar_window = tray_utility_taskbar_window;
namespace dispatch = tray_utility_dispatch;
namespace taskbar_xaml = tray_utility_taskbar_xaml;
namespace taskbar_metrics = tray_utility_taskbar_metrics;
namespace retry_loop = tray_utility_retry;
namespace lease_column = tray_utility_slot_lease;
namespace start_placement = tray_utility_start_placement;

// ── Settings ──────────────────────────────────────────────────────────────

enum class MergeMode {
    Auto,
    ForceMainStack,
};

enum class NewItems {
    Append,
    Ignore,
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

// The six utilities, in the order they are offered in Content. This is also
// the token vocabulary: a stable identity per utility, never a displayed
// label.
enum class Utility {
    Overflow,
    Emoji,
    TouchKeyboard,
    PenMenu,
    VirtualTouchpad,
    InputIndicator,
    Count,
};

static constexpr int kUtilityCount = static_cast<int>(Utility::Count);

static constexpr PCWSTR kUtilityTokens[kUtilityCount] = {
    L"overflow", L"emoji",           L"touchKeyboard",
    L"penMenu",  L"virtualTouchpad", L"inputIndicator",
};

// Fixed buffers rather than std::wstring: a namespace-scope settings struct
// must not own heap (exit-time destructor audit).
struct Settings {
    Position position;
    bool content[kUtilityCount];
    wchar_t arrangement[1024];
    ngl::FillOrder fillOrder;
    ngl::Justify justify;
    NewItems newItems;
    int itemWidth;   // 0 = native
    int itemHeight;  // 0 = native
    int itemSpacing;
    int padX;
    int padY;
    int offsetX;
    int offsetY;
    int minimumTrayHeight;
    MergeMode mergeMode;
};

// Lifecycle: heap-only state destructs normally; direct XAML handles use
// no_destroy; XAML-owning containers use no_destroy optional<container> and
// reset their backing storage after controlled UI-thread cleanup.
static Settings g_settings{};  // exit-time-safe: heap-only
static std::atomic<bool> g_unloading = false;
static std::atomic<HWND> g_taskbarWnd = nullptr;

// What the mod remembers about each tray host it borrowed. The hosts get a
// zero-size marker child in the tray grid so their column survives live
// re-indexing.
// What a host needs REMEMBERED rather than merely restored. Its dependency
// properties go to the lease, which puts back the exact prior local value (or
// clears it, if there was none). Two things cannot come from a lease: the
// column, because the marker tracks live re-indexing and is therefore a
// better answer than the value captured at apply time, and the visible icon
// count, which is drift detection rather than state.
struct HostRecord {
    FrameworkElement element{nullptr};
    FrameworkElement columnMarker{nullptr};
    int column = 0;
    int columnSpan = 1;
    int row = 0;
    int rowSpan = 1;
    int visibleIconViews = 0;
    // True when the tray panel lays out by child order (StackPanel), not columns.
    bool ordered = false;
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
};

[[clang::no_destroy]] static std::optional<std::vector<HostRecord>>
    g_hostRecords{std::in_place};
// Every dependency property this mod writes on a borrowed element, restored
// exactly. Held as optional + no_destroy and reset on the UI thread
// (see the property lease above).
[[clang::no_destroy]] static std::optional<please::Lease> g_lease{
    std::in_place};
static std::atomic<bool> g_layoutApplied = false;
// Only TrayUI::StartTaskbar makes the old XAML tree stale. A settings save
// still owns live state and must restore it before rebuilding.
static std::atomic<bool> g_treeStale = false;
// A settled decision NOT to lay anything out — a vertical taskbar, every
// utility switched off, or a measured tray below the minimum height. Distinct
// from "not applied yet" (tray not laid out, nothing populated): the retry
// must retire on this, and must keep going on that. Cleared on every apply
// and on an Explorer rebuild, so the decision is re-made rather than cached.
static std::atomic<bool> g_stoodDown = false;
// Grid on older taskbars, StackPanel since 26200.9457; Panel covers both.
[[clang::no_destroy]] static Panel g_layoutGrid{nullptr};
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
// Visible-icon counts of the candidate hosts this apply did NOT manage. A host
// that fills in later (MainStack populating after a chevron-only apply) has no
// visibility change to watch, so the LayoutUpdated check compares these too.
struct CandidateCount {
    winrt::weak_ref<FrameworkElement> host;
    int visibleIconViews = 0;
};
// Weak refs and ints only; weak_ref release is a plain refcount decrement.
static std::vector<CandidateCount> g_candidateCounts;  // exit-time-safe: heap-only
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
                            if (!ApplyLayout()) {
                                Wh_Log(
                                    L"[Apply] Scheduled reapply found "
                                    L"the tray unusable");
                            }
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

static void RevokeLayoutCallbacks() {
    if (g_trayLayoutToken && g_layoutGrid) {
        try { g_layoutGrid.LayoutUpdated(g_trayLayoutToken); } catch (...) {}
    }
    g_trayLayoutToken = {};
    if (g_startLease.layoutToken && g_startLease.rootGrid) {
        try { g_startLease.rootGrid.LayoutUpdated(g_startLease.layoutToken); } catch (...) {}
    }
    g_startLease.layoutToken = {};
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

static std::wstring ReadStringSetting(PCWSTR key) {
    auto value = WindhawkUtils::StringSetting::make(key);
    return value.get() ? std::wstring(value.get()) : std::wstring{};
}

template <size_t N>
static void CopyStringSetting(std::wstring const& value,
                              wchar_t (&buffer)[N], PCWSTR key) {
    if (value.size() >= N) {
        Wh_Log(L"[Settings] %s was truncated to %u characters", key,
               static_cast<unsigned>(N - 1));
    }
    wcsncpy_s(buffer, N, value.c_str(), _TRUNCATE);
}

// Settings were reorganised into groups in 2.0. Windhawk cannot write a
// setting, so any reader of the old 1.x keys would leave the settings page
// showing one value while the mod used another; the README instead asks 1.x
// users to re-apply their settings once.
static void LoadSettings() {
    static constexpr sio::Choice<Position> kPositions[] = {
        {L"overflow", Position::Overflow},
        {L"emoji", Position::Emoji},
        {L"beforeIcons", Position::BeforeIcons},
        {L"beforeOmni", Position::BeforeOmni},
        {L"beforeClock", Position::BeforeClock},
        {L"afterClock", Position::AfterClock},
        {L"afterShowDesktop", Position::AfterShowDesktop},
        {L"leftOfStart", Position::LeftOfStart},
        {L"rightOfStart", Position::RightOfStart},
    };
    static constexpr sio::Choice<ngl::FillOrder> kFillOrders[] = {
        {L"rows", ngl::FillOrder::Rows},
        {L"columns", ngl::FillOrder::Columns},
    };
    static constexpr sio::Choice<ngl::Justify> kJustifies[] = {
        {L"start", ngl::Justify::Start},
        {L"center", ngl::Justify::Center},
        {L"end", ngl::Justify::End},
    };
    static constexpr sio::Choice<NewItems> kNewItems[] = {
        {L"append", NewItems::Append},
        {L"ignore", NewItems::Ignore},
    };
    static constexpr sio::Choice<MergeMode> kMergeModes[] = {
        {L"auto", MergeMode::Auto},
        {L"forceMainStack", MergeMode::ForceMainStack},
    };

    g_settings.position = sio::LoadChoice(L"Placement.Position", kPositions,
                                          Position::Overflow);

    static constexpr PCWSTR kContentKeys[kUtilityCount] = {
        L"Content.Overflow",        L"Content.Emoji",
        L"Content.TouchKeyboard",   L"Content.PenMenu",
        L"Content.VirtualTouchpad", L"Content.InputIndicator",
    };
    for (int i = 0; i < kUtilityCount; i++) {
        g_settings.content[i] = sio::LoadBool(kContentKeys[i]);
    }

    CopyStringSetting(ReadStringSetting(L"Layout.Arrangement"),
                      g_settings.arrangement, L"Layout.Arrangement");
    g_settings.fillOrder = sio::LoadChoice(L"Layout.FillOrder", kFillOrders,
                                           ngl::FillOrder::Rows);
    g_settings.justify = sio::LoadChoice(L"Layout.Justify", kJustifies,
                                         ngl::Justify::Center);
    g_settings.newItems =
        sio::LoadChoice(L"Layout.NewItems", kNewItems, NewItems::Append);

    g_settings.itemWidth = sio::LoadInt(L"Size.ItemWidth", 0, 96);
    g_settings.itemHeight = sio::LoadInt(L"Size.ItemHeight", 0, 96);
    g_settings.itemSpacing = sio::LoadInt(L"Size.ItemSpacing", -16, 32);

    g_settings.padX = sio::LoadInt(L"Adjust.PadX", 0, 100);
    g_settings.padY = sio::LoadInt(L"Adjust.PadY", 0, 100);
    g_settings.offsetX = sio::LoadInt(L"Adjust.OffsetX", -100, 100);
    g_settings.offsetY = sio::LoadInt(L"Adjust.OffsetY", -100, 100);

    g_settings.minimumTrayHeight =
        sio::LoadInt(L"Behavior.MinimumTrayHeight", 0, 160);
    g_settings.mergeMode = sio::LoadChoice(L"Behavior.Detection", kMergeModes,
                                           MergeMode::Auto);
}

// ── Taskbar plumbing ──────────────────────────────────────────────────────
//
// Window discovery, UI-thread dispatch, the XamlRoot walk, the taskbar.dll
// symbol hooks and the taskbar metrics all live in tray_utility_taskbar above.
// What remains here is the mod's own voice for failures and call-site names.

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

// taskbar_host::Invoke calls this from inside its own catch, so rethrowing
// there is what lets the logger above name the exception.
static void LogUiCallbackFailure(PCWSTR context) {
    LogCurrentUiException(context);
}

static bool RunFromWindowThread(HWND window, dispatch::ThreadProc proc,
                                void* parameter) {
    return dispatch::RunFromWindowThread(
        window, proc, parameter,
        L"Windhawk_RunFromWindowThread_" WH_MOD_ID);
}

static XamlRoot GetTaskbarXamlRoot(HWND hTaskbarWnd) {
    return taskbar_xaml::GetTaskbarXamlRoot(hTaskbarWnd);
}

// ── Utility discovery ─────────────────────────────────────────────────────

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

// Forgiving token spelling: canonicalize what the user typed. Returns
// Utility::Count when the token is unknown, which is worth a log line rather
// than silent loss.
static Utility CanonicalUtility(std::wstring const& raw) {
    static constexpr struct {
        PCWSTR alias;
        Utility utility;
    } kAliases[] = {
        {L"overflow", Utility::Overflow},
        {L"chevron", Utility::Overflow},
        {L"hidden", Utility::Overflow},
        {L"hiddenIcons", Utility::Overflow},
        {L"emoji", Utility::Emoji},
        {L"touchKeyboard", Utility::TouchKeyboard},
        {L"keyboard", Utility::TouchKeyboard},
        {L"penMenu", Utility::PenMenu},
        {L"pen", Utility::PenMenu},
        {L"virtualTouchpad", Utility::VirtualTouchpad},
        {L"touchpad", Utility::VirtualTouchpad},
        {L"inputIndicator", Utility::InputIndicator},
        {L"input", Utility::InputIndicator},
        {L"language", Utility::InputIndicator},
    };
    for (auto const& alias : kAliases) {
        if (_wcsicmp(raw.c_str(), alias.alias) == 0) {
            return alias.utility;
        }
    }
    return Utility::Count;
}

static std::wstring CanonicalToken(std::wstring const& raw) {
    Utility utility = CanonicalUtility(raw);
    return utility == Utility::Count
               ? std::wstring()
               : std::wstring(kUtilityTokens[static_cast<int>(utility)]);
}

// Two tokens name the same item when they canonicalize to the same utility.
// Comparing them as STRINGS is the alias trap: "chevron" and "overflow" are
// one button, and a string comparison makes an aliased item look missing and
// appends it a second time.
static bool SameUtility(std::wstring const& placed,
                        std::wstring const& expected) {
    Utility a = CanonicalUtility(placed);
    return a != Utility::Count && a == CanonicalUtility(expected);
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

// A token nobody recognizes resolves to an empty size and is silently skipped,
// which looks exactly like a utility Windows is not showing. Say which it is:
// a typo in an arrangement is worth one log line, not a mystery.
static void WarnUnknownTokens(std::wstring const& expression) {
    ngl::Node root;
    if (!ngl::Parse(expression, root)) {
        return;  // the caller reports the parse error itself
    }
    std::vector<std::wstring> leaves;
    CollectLeafTokens(root, leaves);
    for (auto const& leaf : leaves) {
        if (CanonicalUtility(leaf) != Utility::Count) {
            continue;
        }
        Wh_Log(
            L"[Layout] Unknown token '%s' in your arrangement — valid tokens "
            L"are overflow, emoji, touchKeyboard, penMenu, virtualTouchpad "
            L"and inputIndicator (aliases: chevron, hidden, keyboard, pen, "
            L"touchpad, input, language)",
            leaf.c_str());
    }
}

static std::vector<LayoutItem> ResolveLayoutItems(
    Panel const& trayGrid,
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

        Wh_Log(
            L"[Discover] %s host=%s hostLeaf=%d glyph=%s "
            L"natural=%.1fx%.1f",
            item.token.c_str(),
            item.host.Name().c_str(),
            item.hostLeaf,
            DescribeElementGlyphs(item.element).c_str(),
            item.naturalW,
            item.naturalH);
        items.push_back(std::move(item));
    }

    return items;
}

// Discovery diagnostics. Windhawk's own per-mod logging switch already gates
// Wh_Log — and the macro evaluates its arguments only when logging is on — so
// there is no verbosity setting here and no need for one. These run once per
// apply, not per frame, so the tree walk they cost is not worth a switch.
static void LogElement(FrameworkElement const& element, PCWSTR prefix) {
    if (!element) {
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
    if (!host) {
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
//
// Every property this mod writes on an element Windows owns goes through the
// lease, which puts back the exact prior LOCAL value — or clears it, when
// there was none. Clearing matters: a tray element's size and alignment are
// usually driven by its template, and writing a "restored" concrete value
// where no local value existed overrides that binding permanently.

// Announce the placement writes BEFORE making them. First write wins inside
// the lease, so calling this on a re-apply cannot capture the mod's own
// values.
static void TrackPlacement(FrameworkElement const& element) {
    if (!element) {
        return;
    }
    g_lease->Track(element, FrameworkElement::WidthProperty());
    g_lease->Track(element, FrameworkElement::HeightProperty());
    g_lease->Track(element, FrameworkElement::MinWidthProperty());
    g_lease->Track(element, FrameworkElement::MinHeightProperty());
    g_lease->Track(element, FrameworkElement::MaxWidthProperty());
    g_lease->Track(element, FrameworkElement::MaxHeightProperty());
    g_lease->Track(element, FrameworkElement::MarginProperty());
    g_lease->Track(element, FrameworkElement::HorizontalAlignmentProperty());
    g_lease->Track(element, FrameworkElement::VerticalAlignmentProperty());
    g_lease->Track(element, UIElement::RenderTransformProperty());
}

// The host's slot is deliberately NOT leased. A zero-size marker child left
// where the host was follows the tray's live re-indexing, so the marker's
// position at restore time is a better answer than the index captured when the
// layout was applied. On a Grid the marker carries the column; on the 26200.9457
// StackPanel it carries the child index, which is what order-based layout uses.
static HostRecord CaptureHost(FrameworkElement const& element,
                              Panel const& trayGrid,
                              int markerIndex) {
    HostRecord record;
    record.element = element;
    record.column = Grid::GetColumn(element);
    record.columnSpan = Grid::GetColumnSpan(element);
    record.row = Grid::GetRow(element);
    record.rowSpan = Grid::GetRowSpan(element);
    record.visibleIconViews = CountVisibleIconViews(element);
    record.ordered =
        lease_column::Classify(trayGrid) == lease_column::Kind::Order;

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
    if (!record.ordered) {
        Grid::SetColumn(marker, record.column);
        Grid::SetColumnSpan(marker, 1);
        Grid::SetRow(marker, record.row);
        Grid::SetRowSpan(marker, record.rowSpan);
    }
    // Insert where the host stands on BOTH panel kinds, so the marker holds
    // its place in the child order. On a StackPanel that order is the layout;
    // on a Grid it is not, but keyboard and UIA traversal still follow it, and
    // appending would leave the host after the clock once it is restored.
    int index = lease_column::IndexOfChild(trayGrid, element);
    trayGrid.Children().InsertAt(
        index < 0 ? trayGrid.Children().Size() : static_cast<uint32_t>(index),
        marker);
    record.columnMarker = marker;
    return record;
}

// Put a host back among the tray's children at its marker's index, so the
// child order is exactly what it was. On a StackPanel the index IS the
// position; on a Grid the column is written back afterwards and the index
// keeps keyboard/UIA order intact.
static void ReturnHostToTray(FrameworkElement const& child) {
    if (!child || !g_layoutGrid) {
        return;
    }
    for (auto const& record : *g_hostRecords) {
        if (record.element != child || !record.columnMarker) {
            continue;
        }
        int index = lease_column::IndexOfChild(g_layoutGrid,
                                               record.columnMarker);
        if (index >= 0) {
            g_layoutGrid.Children().InsertAt(static_cast<uint32_t>(index),
                                             child);
            return;
        }
        break;
    }
    g_layoutGrid.Children().Append(child);
}

static void RestoreHostPosition(HostRecord& record) {
    try {
        // On the ordered tray the host was already re-inserted at the marker's
        // index by ReturnHostToTray; there is no column to write back.
        if (record.element && !record.ordered) {
            int restoreColumn = record.column;
            if (record.columnMarker) {
                restoreColumn = Grid::GetColumn(record.columnMarker);
            }
            Grid::SetColumn(record.element, restoreColumn);
            Grid::SetColumnSpan(record.element, record.columnSpan);
            Grid::SetRow(record.element, record.row);
            Grid::SetRowSpan(record.element, record.rowSpan);
        }

        if (record.columnMarker && g_layoutGrid) {
            uint32_t markerIndex = 0;
            if (g_layoutGrid.Children().IndexOf(
                    record.columnMarker, markerIndex)) {
                g_layoutGrid.Children().RemoveAt(markerIndex);
            }
        }
    } catch (...) {
        Wh_Log(L"[Restore] Host position restore failed");
    }
    record = {};
}

static void RestoreLayout() {
    // Tokens can be live while an apply is pending. Revoke them before the
    // ownership check so controlled unload cannot leave mod callbacks in XAML.
    RevokeLayoutCallbacks();
    if (!g_layoutApplied) {
        return;
    }

    if (g_startSettleTimer) {
        try {
            g_startSettleTimer.Stop();
        } catch (...) {
        }
    }

    // Newest first, so the icons (written last, inside the hosts) come back
    // before the hosts that carry them.
    g_lease->RestoreAll(
        []() { Wh_Log(L"[Restore] A leased property failed to restore"); });

    // Send the native hosts home, then take the owned group down.
    try {
        if (g_group) {
            while (g_group.Children().Size() > 0) {
                auto child =
                    g_group.Children().GetAt(0).try_as<FrameworkElement>();
                g_group.Children().RemoveAt(0);
                ReturnHostToTray(child);
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

    // Release the leased column BEFORE reading the markers: releasing shifts
    // every later column index down by one, and the markers shift with it, so
    // reading them afterwards gives the corrected index.
    if (!g_columnLease.markerName.empty() && g_layoutGrid) {
        if (!lease_column::Release(g_layoutGrid, g_columnLease)) {
            Wh_Log(
                L"[Restore] Dedicated-column marker missing; "
                L"leaving columns untouched");
            g_columnLease = {};
        }
    }
    for (auto& record : *g_hostRecords) {
        RestoreHostPosition(record);
    }
    g_hostRecords->clear();
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
    TrackPlacement(element);
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
    g_candidateCounts.clear();
    g_stoodDown = false;

    // After an in-place taskbar rebuild (TrayUI::StartTaskbar) the old XAML
    // tree is gone; drop stale references instead of restoring into it.
    if (g_treeStale.exchange(false) &&
        (!g_hostRecords->empty() || !g_lease->Empty())) {
        // We still own strong references to the old tree here, so revoke its
        // callbacks before releasing those references. Don't attempt full
        // placement restoration into a detached taskbar tree.
        RevokeLayoutCallbacks();
        g_hostRecords->clear();
        // The elements these snapshots describe no longer exist, so restoring
        // would only throw, so the snapshots are abandoned instead.
        g_lease->Abandon();
        g_columnLease = {};
        g_startLease = {};
        g_group = nullptr;
        g_layoutGrid = nullptr;
    }
    RestoreLayout();

    HWND hWnd =
        taskbar_window::ResolveTaskbarWnd(g_taskbarWnd.load());
    if (!hWnd) {
        return false;
    }
    g_taskbarWnd.store(hWnd);

    // Stand down completely on a taskbar this mod's layout model does not
    // describe, BEFORE touching anything. A vertical taskbar is rotated by
    // whichever mod moved it, and arranging into a coordinate space someone
    // else is rotating produces garbage the user cannot diagnose.
    auto metrics = taskbar_metrics::GetMetrics(hWnd);
    if (!taskbar_metrics::LayoutModelApplies(metrics)) {
        Wh_Log(
            L"[Apply] Taskbar is %s (%.0fx%.0f DIP); leaving the native "
            L"layout untouched",
            taskbar_metrics::OrientationName(metrics.orientation),
            metrics.alongDip,
            metrics.constrainedDip);
        // Nothing left to wait for, so the retry loop retires. An Explorer
        // rebuild re-evaluates if the user disables the vertical mod.
        g_stoodDown = true;
        return true;
    }

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
    if (!trayGridElement) {
        Wh_Log(L"[Apply] SystemTrayFrameGrid not found");
        return false;
    }
    // Windows 11 26200.9457 (KB5129195) kept the name SystemTrayFrameGrid but
    // changed the element from a Grid to a StackPanel. Accept either; refuse to
    // guess the layout semantics of anything else.
    auto trayGrid = trayGridElement.try_as<Panel>();
    lease_column::Kind trayKind = lease_column::Classify(trayGridElement);
    if (!trayGrid || trayKind == lease_column::Kind::Unsupported) {
        Wh_Log(L"[Apply] Unsupported SystemTrayFrameGrid type: %s",
               lease_column::ClassName(trayGridElement).c_str());
        return false;
    }

    for (auto const& child : trayGrid.Children()) {
        auto element = child.try_as<FrameworkElement>();
        LogElement(element, L"tray child");
        LogIconViews(element, L"tray child");
    }

    auto overflowHost =
        lease_column::FindDirectChild(trayGrid, L"NotifyIconStack");
    if (!overflowHost) {
        Wh_Log(L"[Apply] NotifyIconStack not found");
        return false;
    }
    auto mainStack =
        lease_column::FindDirectChild(trayGrid, L"MainStack");

    // SETTLED vs NOT-YET. g_stoodDown retires the bounded retry, so it is set
    // only for decisions another attempt cannot change. The tray frame is in
    // the tree before its first arrange pass, so a zero height is "not laid
    // out yet", never "too short" — returning false keeps the retry alive.
    double trayHeight = trayGrid.ActualHeight();
    if (trayHeight <= 0.0) {
        Wh_Log(L"[Apply] Tray not laid out yet");
        return false;
    }
    if (g_settings.minimumTrayHeight > 0 &&
        trayHeight < static_cast<double>(g_settings.minimumTrayHeight)) {
        Wh_Log(
            L"[Apply] Tray height %.1f is below minimum %d",
            trayHeight,
            g_settings.minimumTrayHeight);
        g_stoodDown = true;  // settled: a measured tray that is too short
        return true;
    }

    // Which utilities may participate at all. Content is the enable switch;
    // the arrangement decides where the enabled ones go.
    std::vector<std::wstring> enabledTokens;
    for (int i = 0; i < kUtilityCount; i++) {
        if (g_settings.content[i]) {
            enabledTokens.push_back(kUtilityTokens[i]);
        }
    }
    if (enabledTokens.empty()) {
        Wh_Log(L"[Apply] Every utility is switched off in Content");
        g_stoodDown = true;
        return true;
    }

    // Resolve every enabled utility up front: "auto" needs to know what
    // actually exists, and a written arrangement needs the same list to tell
    // which items it forgot to name.
    auto items = ResolveLayoutItems(trayGrid, overflowHost, mainStack,
                                    enabledTokens);
    if (items.empty()) {
        // MainStack fills in from the tray view model after the frame exists,
        // and with no hidden icons the chevron is collapsed too. Not settled.
        Wh_Log(L"[Apply] No layout items found yet");
        return false;
    }

    std::vector<std::wstring> presentTokens;
    for (auto const& item : items) {
        presentTokens.push_back(item.token);
    }

    ngl::Config config;
    config.spacing = static_cast<double>(g_settings.itemSpacing);
    config.justify = g_settings.justify;
    config.padX = static_cast<double>(g_settings.padX);
    config.padY = static_cast<double>(g_settings.padY);

    // Pixel sizes, native unless the user set explicit item sizes. A token
    // that resolves to nothing is skipped and consumes no space, which is how
    // a utility Windows is not showing collapses out of the arrangement.
    auto resolve = [&](std::wstring const& raw) -> ngl::Size {
        auto token = CanonicalToken(raw);
        if (token.empty()) {
            return {};
        }
        for (auto const& item : items) {
            if (item.token == token) {
                double width = g_settings.itemWidth > 0
                                   ? g_settings.itemWidth
                                   : item.naturalW;
                double height = g_settings.itemHeight > 0
                                    ? g_settings.itemHeight
                                    : item.naturalH;
                return {width, height};
            }
        }
        return {};
    };

    // Rows available to the ITEM GRID. RESERVE BEFORE YOU DIVIDE: the outer
    // vertical padding is spoken for on both sides before the height is
    // divided, or the assembled group overflows the taskbar. The height is in
    // DIPs (taskbar_host::GetMetrics), never raw GetWindowRect pixels.
    double rowHeight = static_cast<double>(g_settings.itemHeight);
    if (rowHeight <= 0.0) {
        for (auto const& item : items) {
            rowHeight = std::max(rowHeight, item.naturalH);
        }
    }
    // The group lives in SystemTrayFrameGrid, not in the whole taskbar. Its
    // live XAML height is already DIPs and remains correct when the tray does
    // not span the taskbar's full height.
    double gridHeightDip =
        std::max(0.0, trayGrid.ActualHeight() - config.padY * 2.0);
    int maxRows = ngl::RowsInHeight(gridHeightDip, rowHeight, config.spacing);

    auto namer = [&presentTokens](int index) {
        return presentTokens[index];
    };
    auto arrangement = ngl::ResolveArrangement(
        g_settings.arrangement, static_cast<int>(presentTokens.size()),
        maxRows, g_settings.fillOrder, namer);
    std::wstring expression = arrangement.expression;
    if (!arrangement.wasAuto) {
        WarnUnknownTokens(expression);
    }

    std::vector<ngl::Placement> placements;
    ngl::Size total;
    ngl::ParseError parseError;
    if (!ngl::Compute(expression, config, resolve, placements, total,
                      &parseError)) {
        Wh_Log(
            L"[Apply] Arrangement parse error at position %d: expected %s. "
            L"Falling back to automatic. Arrangement was: %s",
            static_cast<int>(parseError.position),
            parseError.expected.c_str(),
            expression.c_str());
        expression = ngl::BuildAutoExpression(
            static_cast<int>(presentTokens.size()), maxRows,
            g_settings.fillOrder, namer);
        arrangement.wasAuto = true;
        if (!ngl::Compute(expression, config, resolve, placements, total)) {
            Wh_Log(L"[Apply] Automatic arrangement failed to build");
            return true;
        }
    }

    // Windows shows and hides these utilities live, so an arrangement written
    // earlier can be missing one entirely. Matching is by IDENTITY, not
    // spelling: "chevron" and "overflow" are the same button, and comparing
    // them as strings would append it a second time.
    if (!arrangement.wasAuto && g_settings.newItems == NewItems::Append) {
        auto missing =
            ngl::MissingTokens(presentTokens, placements, SameUtility);
        if (!missing.empty()) {
            std::wstring appended = L"";
            for (auto const& token : missing) {
                if (!appended.empty()) {
                    appended += L", ";
                }
                appended += token;
            }
            expression = ngl::AppendMissing(expression, missing, maxRows,
                                            g_settings.fillOrder);
            Wh_Log(
                L"[Layout] Your arrangement does not name %s; appended after "
                L"it. Fold it in when you next edit the arrangement, or set "
                L"Layout.NewItems to \"ignore\".",
                appended.c_str());
            if (!ngl::Compute(expression, config, resolve, placements,
                              total)) {
                Wh_Log(L"[Apply] Appending the missing items broke the "
                       L"arrangement; keeping what you wrote");
                ngl::Compute(arrangement.expression, config, resolve,
                             placements, total);
                expression = arrangement.expression;
            }
        }
    }

    if (placements.empty() || total.Empty()) {
        // Every present item resolved to an empty size — still measuring.
        Wh_Log(L"[Apply] Layout produced no placements yet");
        return false;
    }

    if (arrangement.wasAuto) {
        // The settings API is read-only, so this is the only way to hand the
        // generated expression back: the user pastes it into Arrangement to
        // take manual control.
        Wh_Log(
            L"[Layout] auto -> \"%s\"  (%d rows available, item pitch %.0f)",
            expression.c_str(), maxRows, rowHeight);
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
            targets.push_back({item.element, placement.x, placement.y,
                               placement.size.width, placement.size.height});
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

    // Reparenting a host brings ALL of its icons along, named or not, so an
    // icon the arrangement never mentioned would otherwise pile up at the
    // group's origin underneath another one. This is a physical consequence
    // of reparenting, not the Layout.NewItems policy: these are icons with no
    // token at all, so no arrangement could have named them.
    double extraCursor = total.width;
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
        std::wstring carried;
        for (auto const& icon : icons) {
            if (isTargeted(icon)) {
                continue;
            }
            std::wstring name = L"unidentified utility";
            for (int i = 0; i < kUtilityCount; ++i) {
                if (IconViewMatchesToken(icon, kUtilityTokens[i])) {
                    name = kUtilityTokens[i];
                    break;
                }
            }
            if (!carried.empty()) {
                carried += L", ";
            }
            carried += name;
            double width = NaturalWidth(icon);
            double height = NaturalHeight(icon);
            IconTarget straggler;
            straggler.element = icon;
            straggler.width = width;
            straggler.height = height;
            straggler.x = extraCursor + config.spacing;
            straggler.y = std::max(0.0, (total.height - height) / 2.0);
            extraCursor = straggler.x + width;
            targets.push_back(straggler);
        }
        if (!carried.empty()) {
            Wh_Log(L"[Layout] Shared host carried %s after the arrangement",
                   carried.c_str());
        }
    }
    total.width = std::max(total.width, extraCursor);

    // From here on we mutate the tree: snapshot everything first.
    g_layoutGrid = trayGrid;
    g_layoutApplied = true;
    for (int i = 0; i < static_cast<int>(managedHosts.size()); i++) {
        g_hostRecords->push_back(CaptureHost(managedHosts[i], trayGrid, i));
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
                std::max(0, g_settings.itemSpacing), g_startLease)) {
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
            bool anchorIsManaged = std::find(
                managedHosts.begin(), managedHosts.end(), anchorHost) !=
                managedHosts.end();
            column = trayKind == lease_column::Kind::Order
                         ? lease_column::IndexOfChild(trayGrid, anchorHost)
                         : Grid::GetColumn(anchorHost);
            if (column < 0) {
                Wh_Log(L"[Apply] Anchor host is no longer a tray child");
                RestoreLayout();
                return false;
            }
            // A borrowed anchor only frees its slot when it will be moved into
            // our group. If it remains native (for example the chevron is off
            // while another MainStack utility is on), lease a neighboring slot
            // rather than put two Grid children in one column.
            if (!anchorIsManaged &&
                !lease_column::AcquireAt(trayGrid, column,
                                         kLayoutColumnMarkerName,
                                         g_columnLease)) {
                Wh_Log(L"[Apply] Could not lease a slot beside the anchor host");
                RestoreLayout();
                return false;
            }
            if (!g_columnLease.markerName.empty()) {
                column = g_columnLease.slot;
            }
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
                    L"leaving the native layout unchanged (tray %s holds: %s)",
                    lease_column::ClassName(trayGridElement).c_str(),
                    lease_column::DescribeChildren(trayGrid).c_str());
                RestoreLayout();
                return false;
            }
            column = g_columnLease.slot;
        }

        group.HorizontalAlignment(HorizontalAlignment::Center);
        group.VerticalAlignment(VerticalAlignment::Center);
        if (!g_columnLease.markerName.empty()) {
            // The lease already holds the slot; drop the group into it.
            if (!lease_column::PlaceChild(trayGrid, g_columnLease, group)) {
                Wh_Log(L"[Apply] Could not place the group in the lease");
                RestoreLayout();
                return false;
            }
        } else if (trayKind == lease_column::Kind::Order) {
            // Borrowed-anchor path, order-based tray: take the anchor host's
            // index. Reparenting the managed hosts below removes siblings
            // around the group, which keeps its place relative to the rest.
            trayGrid.Children().InsertAt(
                std::min(static_cast<uint32_t>(column),
                         trayGrid.Children().Size()),
                group);
        } else {
            Grid::SetColumn(group, column);
            trayGrid.Children().Append(group);
        }
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

    // Adjust.OffsetX/Y move the group VISUALLY and reserve nothing, so they
    // ride on the hosts rather than on the group itself — the group's own
    // margin belongs to the Start lease, which repositions it every layout
    // pass.
    double groupOffsetX = static_cast<double>(g_settings.offsetX);
    double groupOffsetY = static_cast<double>(g_settings.offsetY);

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
            TrackPlacement(icon);
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

    // Remember the unmanaged candidates too (see g_candidateCounts).
    for (auto const& child : trayGrid.Children()) {
        auto element = child.try_as<FrameworkElement>();
        if (!element || !IsUtilityCandidateHost(element)) {
            continue;
        }
        bool managed = false;
        for (auto const& host : managedHosts) {
            if (host == element) {
                managed = true;
                break;
            }
        }
        if (!managed) {
            g_candidateCounts.push_back(
                {winrt::make_weak(element), CountVisibleIconViews(element)});
        }
    }

    // Visibility watchers miss icons appearing or vanishing inside a host,
    // so verify on tray layout passes that every managed host is intact
    // and its visible icon count is unchanged, and that no unmanaged
    // candidate has gained or lost icons; any drift re-runs layout.
    g_trayLayoutToken = trayGrid.LayoutUpdated(
        [](auto const&, auto const&) {
            if (g_unloading || !g_layoutApplied) {
                return;
            }
            // Throttle: layout passes come in bursts (animations, clock
            // ticks), and each check walks every candidate host's subtree.
            // Utility icons change on a human timescale, so twice a second
            // keeps this off the hot path without making a change feel late.
            static ULONGLONG lastCheckTick = 0;
            ULONGLONG nowTick = GetTickCount64();
            if (nowTick - lastCheckTick < 500) {
                return;
            }
            lastCheckTick = nowTick;
            for (auto const& record : *g_hostRecords) {
                if (!record.element) {
                    continue;
                }
                bool changed = false;
                try {
                    changed =
                        !VisualTreeHelper::GetParent(record.element) ||
                        record.element.Visibility() !=
                            Visibility::Visible;
                    if (!changed) {
                        changed =
                            CountVisibleIconViews(record.element) !=
                            record.visibleIconViews;
                    }
                } catch (...) {
                    changed = true;
                }
                if (changed) {
                    ScheduleReapply();
                    return;
                }
            }
            for (auto const& candidate : g_candidateCounts) {
                try {
                    auto host = candidate.host.get();
                    if (host && CountVisibleIconViews(host) !=
                                    candidate.visibleIconViews) {
                        ScheduleReapply();
                        return;
                    }
                } catch (...) {
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
        L"tray%s=%d dedicated=%d groupSize=%.0fx%.0f trayHeight=%.1f "
        L"leased=%d expr=%s",
        static_cast<int>(items.size()),
        static_cast<int>(targets.size()),
        static_cast<int>(managedHosts.size()),
        trayKind == lease_column::Kind::Order ? L"Index" : L"Column",
        sharedColumn,
        !g_columnLease.markerName.empty(),
        total.width,
        total.height,
        trayGrid.ActualHeight(),
        static_cast<int>(g_lease->Count()),
        expression.c_str());
    return true;
}

static void ApplyLayoutOnWindowThread() {
    HWND hWnd =
        taskbar_window::ResolveTaskbarWnd(g_taskbarWnd.load());
    if (!hWnd || g_unloading) {
        return;
    }
    RunFromWindowThread(
        hWnd,
        [](void*) {
            try {
                // false means the tray was not in a usable state this pass —
                // not a settled decision. The bounded retry comes back.
                if (!ApplyLayout()) {
                    Wh_Log(L"[Apply] Tray not ready; will retry");
                }
            } catch (...) {
                Wh_Log(L"[Apply] Exception while applying layout");
            }
        },
        nullptr);
}

// ── Hooks and lifecycle ───────────────────────────────────────────────────

// The bounded retry, the taskbar.dll symbol hooks and the TrayUI::StartTaskbar
// rebuild trigger all live in tray_utility_taskbar. Three things kick an apply —
// this retry, an Explorer taskbar rebuild, and the visibility watchers — and
// all three converge on the one idempotent ApplyLayout.
// no_destroy is not about waiting — RetryLoop's destructor waits for nothing;
// it would only release the last Run and close two handles. It is kept so the
// lab's exit-time-destructor audit stays exact (every namespace-scope object
// with a non-trivial destructor is either marked or proven heap-only), and
// Wh_ModUninit stops the loop explicitly.
[[clang::no_destroy]] static retry_loop::RetryLoop g_retry;

// Attempts x 1.5 s. Transient "not ready yet" states return false and keep
// the loop alive, so the budget has to cover a tray that populates slowly at
// sign-in, not just one that is briefly missing.
static constexpr int kRetryAttempts = 20;
static constexpr DWORD kRetryIntervalMs = 1500;

// "Applied" must mean the work is DONE, not that the tray was found: the
// retry stops on the first true, so a premature one retires the retry while
// the layout is still unresolved.
static bool LayoutIsApplied() {
    return g_layoutApplied || g_stoodDown;
}

static void RetryAttempt() {
    ApplyLayoutOnWindowThread();
}

// Explorer can rebuild the taskbar in place; the old XAML tree and our
// records are gone with it, so restart the bounded retry.
static void OnTaskbarRebuilt() {
    if (g_unloading) {
        return;
    }
    Wh_Log(L"[Hooks] TrayUI::StartTaskbar; rescheduling layout");
    g_taskbarWnd.store(nullptr);
    g_treeStale = true;
    g_stoodDown = false;
    g_retry.Start(RetryAttempt, LayoutIsApplied, g_unloading, kRetryAttempts,
                  kRetryIntervalMs, true);
}

BOOL Wh_ModInit() {
    Wh_Log(L"[Init] Tray Utility Customizer v2.0");
    LoadSettings();
    dispatch::SetExceptionLogger(LogUiCallbackFailure);

    if (!taskbar_xaml::HookTaskbarSymbols(OnTaskbarRebuilt)) {
        Wh_Log(L"[Init] taskbar.dll symbol hooks failed");
        return FALSE;
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    // The same bounded retry as a rebuild, not a single attempt: Windhawk can
    // attach to an Explorer whose taskbar window exists while its tray XAML
    // is still being built, and one failed attempt would never come back.
    g_retry.Start(RetryAttempt, LayoutIsApplied, g_unloading, kRetryAttempts,
                  kRetryIntervalMs, false);
}

void Wh_ModSettingsChanged() {
    g_retry.Stop();
    HWND hWnd = taskbar_window::ResolveTaskbarWnd(g_taskbarWnd.load());
    if (!hWnd) {
        // No taskbar thread means no layout pass can be reading g_settings,
        // so load here; the next TrayUI::StartTaskbar applies the new values.
        LoadSettings();
        Wh_Log(L"[Settings] No taskbar UI thread; loaded for the next rebuild");
        return;
    }
    if (!RunFromWindowThread(
            hWnd,
            [](void*) {
                // Settings and every consumer of g_settings now share the
                // taskbar UI thread, so a layout pass cannot observe a torn
                // fixed-buffer arrangement during a save.
                LoadSettings();
                Wh_Log(L"[Settings] Reapplying");
                // Request a retry without lying about ownership of the live
                // layout; the forced first pass restores it before applying
                // the new settings.
                g_stoodDown = false;
                g_retry.Start(RetryAttempt, LayoutIsApplied, g_unloading,
                              kRetryAttempts, kRetryIntervalMs, true);
            },
            nullptr)) {
        Wh_Log(L"[Settings] Could not dispatch the reapply to the taskbar UI thread");
    }
}

// Everything that must stop pointing into this image before Windhawk frees it:
// the visibility watchers, both timers, the LayoutUpdated tokens (revoked by
// RestoreLayout before its ownership check), and the borrowed properties.
// Runs on the taskbar's UI thread, and is safe to run twice.
static void TearDownOnWindowThread(void*) {
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
    g_lease.reset();
    g_hostRecords.reset();
}

void Wh_ModUninit() {
    g_unloading = true;
    Wh_Log(L"[Uninit]");
    g_retry.Stop();

    HWND hWnd = taskbar_window::ResolveTaskbarWnd(g_taskbarWnd.load());
    if (!hWnd) {
        // Intentionally retain all no_destroy XAML/WinRT holders: there is no
        // safe thread on which to release them at shutdown.
        Wh_Log(L"[Uninit] No taskbar UI thread; retaining XAML state");
        return;
    }

    // The dispatch reports whether the callback actually RAN. If it did not,
    // every watcher, timer and LayoutUpdated token is still registered while
    // the image is about to be freed, so rediscover the taskbar and try once
    // more rather than unloading over live callbacks.
    if (RunFromWindowThread(hWnd, TearDownOnWindowThread, nullptr)) {
        return;
    }
    Wh_Log(L"[Uninit] Taskbar cleanup dispatch failed; retrying");
    HWND retryWnd = taskbar_window::ResolveTaskbarWnd(nullptr);
    if (!retryWnd ||
        !RunFromWindowThread(retryWnd, TearDownOnWindowThread, nullptr)) {
        Wh_Log(
            L"[Uninit] Could not reach the taskbar UI thread; mod callbacks "
            L"may still be registered");
    }
}
