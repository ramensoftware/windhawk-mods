// ==WindhawkMod==
// @id              taskbar-vd-switcher
// @name            Taskbar Virtual Desktop Switcher
// @description     Injects clickable buttons into the taskbar — one per virtual desktop — with configurable grid arrangement for direct switching.
// @version         2.0
// @author          sb4ssman
// @github          https://github.com/sb4ssman
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lversion -luuid -ldwmapi -lgdi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Virtual Desktop Switcher

A [Windhawk](https://windhawk.net) mod for Windows 11 that injects clickable buttons into the system tray — one per virtual desktop — for instant switching without opening Task View.

![Three desktops with lower master button](https://raw.githubusercontent.com/sb4ssman/Windhawk-Mod-Lab/main/taskbar-vd-switcher/assets/simple3wlowmaster.png)
*Three desktops with the optional Task View button as a lower sliver.*

![Default tray placement — three numbered buttons, first active](https://raw.githubusercontent.com/sb4ssman/Windhawk-Mod-Lab/main/taskbar-vd-switcher/assets/simple3.png)
*Default tray placement: three desktops in a row, desktop 1 active.*

![Two desktop compact tray placement](https://raw.githubusercontent.com/sb4ssman/Windhawk-Mod-Lab/main/taskbar-vd-switcher/assets/simple2.png)
*Compact tray placement with two desktops.*

![Four desktops with master button](https://raw.githubusercontent.com/sb4ssman/Windhawk-Mod-Lab/main/taskbar-vd-switcher/assets/simple4wmaster.png)
*Four desktops with the optional Task View button.*

![Taller taskbar with right-side grid and lower master button](https://raw.githubusercontent.com/sb4ssman/Windhawk-Mod-Lab/main/taskbar-vd-switcher/assets/gridonrightwlowermaster.png)
*Taller taskbar: a dense grid with the Task View button as a lower sliver.*

![Eleven desktops in a grid on a busy double-height taskbar](https://raw.githubusercontent.com/sb4ssman/Windhawk-Mod-Lab/main/taskbar-vd-switcher/assets/busy-many-desktops-modified-taskview.png)
*Eleven desktops on a double-height taskbar, with a restyled Task View button
and several other taskbar mods alongside. `auto` fits the grid to the height it
is given rather than to a desktop count.*

![Left of Start button](https://raw.githubusercontent.com/sb4ssman/Windhawk-Mod-Lab/main/taskbar-vd-switcher/assets/left-of-start.png)
*Start placement: switcher reserved to the left of Start.*

![Left of Start with Start hidden](https://raw.githubusercontent.com/sb4ssman/Windhawk-Mod-Lab/main/taskbar-vd-switcher/assets/left-of-start-hidden-start.png)
*Start placement with the Start button hidden.*

![Over Start, nudged above](https://raw.githubusercontent.com/sb4ssman/Windhawk-Mod-Lab/main/taskbar-vd-switcher/assets/over-above-start.png)
*Overlay mode can be nudged up with the vertical offset setting.*

![Over Start, nudged below](https://raw.githubusercontent.com/sb4ssman/Windhawk-Mod-Lab/main/taskbar-vd-switcher/assets/over-below-start.png)
*Overlay mode can also be nudged down.*

![Right of Start with Start hidden](https://raw.githubusercontent.com/sb4ssman/Windhawk-Mod-Lab/main/taskbar-vd-switcher/assets/right-of-start-hidden-start.png)
*Right-of-Start placement when the Start button is hidden.*

## Features

- Numbered, roman-numeral, indicator-symbol, or custom-label buttons
- Automatic grid that fits the buttons to the taskbar's height, or an arrangement you write yourself
- Optional Task View button as a full column, a sliver, or one more button in the grid
- Highlights the active desktop immediately on switch
- Buttons appear and disappear as desktops are added or removed
- Five tray positions, plus experimental Start-adjacent and Start-overlay positions
- Configurable size, spacing, colors, opacity, and shine effect
- Per-state text color, font size and family, corner radius, bold, and border
- Native checked states that can be targeted by Windows 11 Taskbar Styler
- Tooltip on each button shows the desktop's display name
- Option to hide the bar entirely when only one desktop exists
- Experimental option to also show the switcher on secondary monitors' taskbars

## Upgrading from 1.x

Version 2.0 reorganizes every setting into groups — Placement, Content, Layout,
Size, Adjust, Surface, State, Behavior — so this mod matches the rest of the
family. Windhawk cannot carry values across renamed keys, so **your previous
customizations are not migrated; re-apply them once after updating.**

The layout settings collapsed into a single **Arrangement** field. Grid mode,
smart layout, rows, columns, primary axis, cross alignment, the four padding
sides, the vertical offset, and both nudge strings are gone; see below for what
replaced them.

## The Arrangement field

`Layout` → `Arrangement` decides how the buttons are placed, and it is the only
field that does. Its default value is the word `auto`:

- **`auto`** fits the buttons to the available taskbar height. `Fill order`
  chooses whether they fill across rows or down columns; `Short row or column`
  aligns a ragged last group. The shape itself is worked out for you: the mod
  takes the narrowest grid that fits the height, preferring the one that wastes
  the fewest slots — four desktops on a double-height taskbar become a 2×2
  block, not a lopsided 3+1.
- **Anything else** is an arrangement you write. Names sit side by side with
  `|` and stack with `,`, and parentheses group them:

  ```text
  1, 2 | 3, 4        a 2x2 block
  1 | 2 | 3 | 4      a single row
  1, 2, 3, 4         a single column
  master | (1, 2)    Task View button left of a stacked pair
  ```

  Buttons are named by desktop number; the Task View button is `master` or
  `taskview`. `desktop2` also works as a readable alias for `2`, and names are
  case-insensitive. A separator is always required — `1 (2 | 3)` is an error,
  not a shorthand for `1 | (2 | 3)`.

Every time the layout is applied, the arrangement `auto` produced is written to
the Windhawk log, along with which desktop each number refers to
(`tokens: 1=Home  2=Work`). Copy the arrangement line into the Arrangement
field and you have the automatic layout as a starting point to edit — the
automatic and manual paths are the same field and the same syntax. If what you
write doesn't parse, the log says what was expected and where, and the
automatic arrangement is used until you fix it.

**Nudging.** Append a pixel offset to any name to move just that button:

```text
1[+2,-1] | 2 | 3       desktop 1 moves 2px right and 1px up
(1, 2) | master[0,2]   Task View button drops 2px
```

A parenthesized group takes an offset too, moving everything inside it:

```text
(1, 2)[3,0] | 3        the stacked pair moves 3px right, 3 stays put
```

Offsets are cosmetic. Nothing else shifts, and the group's overall size does
not change. To move the whole group instead, use `Adjust` → horizontal and
vertical offset.

**Desktops you create later.** An arrangement you write names the desktops that
existed when you wrote it. Create another one and it is in no group, so by
default it is appended after your arrangement rather than vanishing — the log
says when that happened, so you can fold it in when you next edit. Set
`Layout` → `Newly created desktops` to *Leave them out* if you would rather
your arrangement be the whole truth. `auto` always includes every desktop.

## Desktop hover previews

Hover a desktop button for 400 ms to see an overview of that desktop's open
windows. The preview keeps their relative positions across your monitors and
shows the desktop name. It does not switch desktops or take keyboard focus;
click the desktop button to switch as usual. Moving away or clicking closes it.

Previews are enabled by default. In **Behavior**, turn **Desktop hover previews**
off to return to desktop-name tooltips, adjust **Preview delay** (100–2000 ms),
or set **Preview width** (200–800 px, scaled for the monitor).

The overview uses Windows' window thumbnails on a neutral background that
follows your Windows light/dark theme, with rounded corners to match the
shell; it is not a screenshot of the wallpaper or Task View. Minimized windows are counted
rather than shown. Windows may provide a blank or last-rendered image for
protected, suspended, or inactive-desktop applications. An empty desktop is
labelled explicitly. Window membership and positions are refreshed on each
hover; the thumbnails themselves are maintained by Windows while visible.
Windows pinned across desktops may only appear on their assigned desktop.

## The Task View button

`Content` → `Task View button placement` decides where it goes: a column
**before** or **after** the desktop buttons, a row **above** or **below** them,
or the **last button in the grid**. This applies whether the layout came from
`auto` or from an arrangement you wrote — write `master` in your arrangement
and you place it exactly, and the setting steps aside.

For the column and row placements, `Size` → `Task View button thickness` is how
thick it is: its **width** as a column, its **height** as a row. `Task View
button length` is how far it runs along the desktop buttons, and `0` — the
default — means match them exactly, so it is a full-height column or a
full-width sliver however many desktops you have. Give the length a value to
make it shorter; it is then centered by `Short row or column`.

`Task View button gap` puts extra distance between it and the desktop buttons,
on top of the normal spacing — positive pushes it further away whichever side
it is on, negative pulls it closer or over them. It moves the button **without
resizing the group**, which is the useful part: push a sliver below far enough
and it hangs past the bottom of the taskbar so only its leading edge shows,
rather than the whole group growing and re-centering. On a column, a few pixels
of gap simply sets it apart from the set.

**Last button in the grid** ignores thickness, length, and gap, and sizes it
like a desktop button so it flows with them as one more cell — `1, 4 | 2, 5 |
3, ⊞`. Use it when you want the Task View button to read as part of the set
rather than as a bar alongside it; it keeps its own label and font.

All of this applies when the arrangement does not name the button. Write
`master` yourself and you are placing it — add your own offset there if you
want the gap, like `(1 | 2 | 3), master[0,8]`.

## Settings

### Placement

| Setting | Default | Description |
|---------|---------|-------------|
| Position | After clock | Tray position, or left of / over / right of Start |
| Show on all taskbars | Off | Experimental; also injects into secondary monitors' taskbars |

### Content

| Setting | Default | Description |
|---------|---------|-------------|
| Label format | Numbers | Numbers · Roman numerals · Indicator symbols · Custom labels |
| Custom labels | *(empty)* | Comma-separated, e.g. `H,W,M` |
| Active indicator symbol | ● | Current desktop's symbol in Indicator symbols mode |
| Inactive indicator symbol | ○ | Other desktops' symbol; paste 🟢 above and 🔴 here for a stoplight |
| Task View button | Off | Adds a button that opens Task View for previewing, creating, or closing desktops |
| Task View button label | ⊞ | Text shown on that button |
| Task View button placement | After | Column before/after, row above/below, or last button in the grid |

### Layout

| Setting | Default | Description |
|---------|---------|-------------|
| Arrangement | `auto` | `auto`, or an arrangement you write — see above |
| Fill order | Fill rows first | Used by `auto` |
| Short row or column | Center | Used by `auto`; start, center, or end |
| Newly created desktops | Add them after | Or leave them out; only applies to a written arrangement |

### Size

| Setting | Default | Description |
|---------|---------|-------------|
| Button width | 20 px | |
| Button height | 22 px | |
| Button spacing | 2 px | Gap between buttons along each axis |
| Task View button thickness | 14 px | Width as a column, height as a sliver; unused in the grid placement |
| Task View button length | 0 px | 0 matches the desktop buttons exactly |
| Task View button gap | 0 px | Extra distance from the desktop buttons; moves it without resizing the group |

### Adjust

| Setting | Default | Description |
|---------|---------|-------------|
| Horizontal padding | 0 px | Reserved on both sides of the group |
| Vertical padding | 0 px | Reserved above and below the group |
| Horizontal offset | 0 px | Moves the group; reserves no space |
| Vertical offset | 0 px | Moves the group up (negative) or down (positive) |

### Surface

| Setting | Default | Description |
|---------|---------|-------------|
| Font size | 10 pt | |
| Font family | *(native)* | For desktop labels and indicator symbols |
| Hover background color | *(automatic)* | Empty brightens each button's own background |
| Click background color | *(automatic)* | Empty darkens each button's own background |
| Border color | *(native)* | |
| Border thickness | 0 px | |
| Corner radius | 4 px | 0 = square, 4 = Windows default |
| Opacity | 100 | Lower values let the taskbar show through |
| Shine effect | Off | Gradient highlight on buttons with custom colors |
| Task View font family | *(native)* | Independent font for the Task View label |

### State

| Setting | Default | Description |
|---------|---------|-------------|
| Active desktop text color | *(native)* | |
| Inactive button text color | *(native)* | |
| Active desktop color | `accent` | Empty keeps the plain native surface |
| Inactive button color | *(native)* | |
| Bold the active desktop label | Off | |

### Behavior

| Setting | Default | Description |
|---------|---------|-------------|
| Desktop hover previews | On | Overview of the hovered desktop without switching |
| Preview delay | 400 ms | Clamped to 100–2000 ms |
| Preview width | 320 px | Clamped to 200–800 px; monitor-scaled |
| Hide when only one desktop | Off | |

All color settings accept `#RRGGBB` or `#AARRGGBB` hex (the alpha byte is
honored), the generics `accent`, `accentLight`, and `accentDark` for the
Windows accent shades, or `transparent` for a fully transparent surface —
nothing drawn, element still present and clickable. Leaving a color empty
keeps the native behavior described for that setting — including the Active
desktop color, where empty means the current desktop's button keeps the plain
native surface with no highlight at all.

## Taskbar Styler

Desktop buttons are XAML `ToggleButton` controls named `VdBtn_0`, `VdBtn_1`,
and so on. The current desktop has `IsChecked=true`, exposing the native
`Checked`, `CheckedPointerOver`, and `CheckedPressed` states. Taskbar Styler
can target every indicator's template presenter with:

```text
Grid#VdSwitcherBar > ToggleButton > ContentPresenter#ContentPresenter@CommonStates
```

State-qualified styles such as `Background@Checked`,
`Background@CheckedPointerOver`, and `Background@CheckedPressed` then apply
without inferring the active desktop from its color.

## Known limitations

- Multi-monitor support is experimental and off by default: secondary taskbars use the tray positions only (Start positions stay on the primary taskbar), and they are discovered as their tray icons load — after enabling the option, an Explorer restart (or toggling the mod off and on) may be needed before the buttons appear on other monitors
- Buttons may not appear until the mod injects on the first tray icon load; retry loop runs up to 5 times at 2-second intervals

## Credits and inspirations

This mod builds directly on patterns established by several community mods:

**[taskbar-empty-space-clicks](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/taskbar-empty-space-clicks.wh.cpp)** — source of the `SwitchVirtualDesktop()` COM vtable pattern, build-specific IIDs for `IVirtualDesktopManagerInternal`, and the `IObjectArray` desktop enumeration approach.

**[taskbar-desktop-indicator](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/taskbar-desktop-indicator.wh.cpp)** — reference for reading the current virtual desktop from the registry (session-scoped `VirtualDesktopIDs` + `CurrentVirtualDesktop` keys) and the notification cookie / `IVirtualDesktopNotificationService` registration pattern.

**[Vertical OmniButton archive](../omnibutton-customizer/archive/vertical-omnibutton-v1.4.wh.cpp)** (this lab, by sb4ssman) — source of the `GetTaskbarXamlRoot` boilerplate, `RunFromWindowThread` dispatcher, `FindCurrentProcessTaskbarWnd`, and the `IconView::IconView` hook-and-retry injection pattern.

**[windows-11-taskbar-styler](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/windows-11-taskbar-styler.wh.cpp)** — reference for the `SystemTrayFrameGrid` XAML tree structure and element names (`ShowDesktopStack`, `NotificationCenterButton`, `ControlCenterButton`, `NotifyIconStack`).

**[Windhawk](https://windhawk.net)** by [m417z](https://github.com/m417z) — the modding platform that makes all of this possible.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- Placement:
  - Position: "afterClock"
    $name: Position
    $description: Where to place the switcher on the taskbar.
    $options:
    - "beforeIcons": "Before notification icons"
    - "beforeOmni": "Before network, volume, and battery"
    - "beforeClock": "Before clock"
    - "afterClock": "After clock"
    - "afterShowDesktop": "After Show Desktop"
    - "leftOfStart": "Left of Start (experimental)"
    - "overStart": "Over Start (experimental)"
    - "rightOfStart": "Right of Start (experimental)"
  - AllTaskbars: false
    $name: Show on all taskbars
    $description: >-
      Experimental. Also injects the switcher into secondary monitors'
      taskbars, in the same position. Tray positions only - the Start
      positions stay on the primary taskbar. Secondary taskbars are
      discovered as their tray icons load, so after enabling this you may
      need to restart Explorer before the buttons appear on other monitors.
  $name: Placement

- Content:
  - LabelFormat: "number"
    $name: Label format
    $options:
    - "number": "Numbers  1  2  3"
    - "roman": "Roman numerals  I  II  III"
    - "symbol": "Indicator symbols  ●  ○  ○"
    - "custom": "Custom labels"
  - CustomLabels: ""
    $name: Custom labels
    $description: >-
      Comma-separated, e.g. "H,W,M". Used when Label format is Custom. Falls
      back to numbers if labels run out.
  - ActiveSymbol: "●"
    $name: Active indicator symbol
    $description: >-
      Shown for the current desktop when Label format is Indicator symbols.
      For a stoplight look, paste 🟢 here and 🔴 below.
  - InactiveSymbol: "○"
    $name: Inactive indicator symbol
    $description: >-
      Shown for the other desktops when Label format is Indicator symbols.
      For a stoplight look, paste 🔴 here and 🟢 above.
  - TaskViewButton: false
    $name: Task View button
    $description: >-
      Adds a button that opens Task View (Win+Tab), where you can preview all
      desktops and create or close them.
  - TaskViewLabel: "⊞"
    $name: Task View button label
  - TaskViewPlacement: "after"
    $name: Task View button placement
    $description: >-
      Where the Task View button goes when your arrangement does not name it.
      Write "master" in the arrangement yourself to place it exactly, and this
      is ignored.
    $options:
    - "before": "Column before the desktop buttons"
    - "after": "Column after the desktop buttons"
    - "above": "Row above the desktop buttons"
    - "below": "Row below the desktop buttons"
    - "inGrid": "Last button in the grid, same size as the others"
  $name: Content

- Layout:
  - Arrangement: "auto"
    $name: Arrangement
    $description: >-
      "auto" fits the buttons to the available taskbar height. Anything else
      is an explicit arrangement: names side by side with "|", stacked with
      ",", grouped with parentheses - "1, 2 | 3, 4" is a 2x2 block. Buttons
      are named by desktop number ("desktop2" also works), plus "master" or
      "taskview" for the Task View button. Append a pixel offset to nudge one button,
      "1[+2,-1]", or a whole group, "(1, 2)[3,0]". Every time the layout is
      applied, the arrangement "auto" produced is written to the Windhawk log
      along with which desktop each number is, so you can paste it here and
      edit it. If what you type does not parse, the log says what was expected
      and where, and "auto" is used until you fix it.
  - FillOrder: "rows"
    $name: Fill order
    $description: Used by "auto". Whether buttons fill across rows or down columns first.
    $options:
    - "rows": "Fill rows first (left to right, then down)"
    - "columns": "Fill columns first (top to bottom, then right)"
  - Justify: "center"
    $name: Short row or column
    $description: Used by "auto". How a ragged last row or column is aligned.
    $options:
    - "start": "Start (top for columns, left for rows)"
    - "center": "Center"
    - "end": "End (bottom for columns, right for rows)"
  - NewItems: "append"
    $name: Newly created desktops
    $description: >-
      What happens when you create a desktop that your own arrangement does not
      name. Only applies when you have written an arrangement - "auto" always
      includes every desktop.
    $options:
    - "append": "Add them after the arrangement"
    - "ignore": "Leave them out until I add them"
  $name: Layout

- Size:
  - ItemWidth: 20
    $name: Button width (px)
  - ItemHeight: 22
    $name: Button height (px)
  - ItemSpacing: 2
    $name: Button spacing (px)
    $description: Gap between buttons along each axis.
  - TaskViewSize: 14
    $name: Task View button thickness (px)
    $description: >-
      How thick the Task View button is: its width when it sits beside the
      desktop buttons, its height when it sits above or below them. Not used
      when its placement is "Last button in the grid" - it is then sized like
      a desktop button.
  - TaskViewSpan: 0
    $name: Task View button length (px)
    $description: >-
      How far it runs along the desktop buttons. 0 matches them exactly - a
      full-height column beside them, or a full-width sliver above or below.
      Any other value is a fixed length, centered by Short row or column.
  - TaskViewGap: 0
    $name: Task View button gap (px)
    $description: >-
      Extra distance between the Task View button and the desktop buttons, on
      top of the normal button spacing. Positive pushes it further away,
      negative pulls it closer or over them. It moves without resizing the
      group, so a sliver can hang past the edge of the taskbar and show only
      its leading edge. Not used when its placement is "Last button in the
      grid", or when you name it in your own arrangement - write your own
      offset there, like "master[0,3]".
  $name: Size

- Adjust:
  - PadX: 0
    $name: Horizontal padding (px)
    $description: Space reserved on both sides of the button group.
  - PadY: 0
    $name: Vertical padding (px)
    $description: Space reserved above and below the button group.
  - OffsetX: 0
    $name: Horizontal offset (px)
    $description: Moves the whole group. Does not reserve space.
  - OffsetY: 0
    $name: Vertical offset (px)
    $description: Moves the whole group up (negative) or down (positive) from centered.
  $name: Adjust

- Surface:
  - FontSize: 10
    $name: Font size (pt)
  - FontFamily: ""
    $name: Font family
    $description: >-
      Font for the desktop labels and indicator symbols. Empty uses the
      native font. For example, Segoe UI Emoji.
  - HoverBackgroundColor: ""
    $name: Hover background color
    $description: >-
      Hex (#RRGGBB or #AARRGGBB), accent / accentLight / accentDark, or
      transparent, to force one shared hover color. Empty brightens each
      button's own background; buttons on the native surface keep the native
      hover.
  - PressedBackgroundColor: ""
    $name: Click background color
    $description: >-
      Hex, accent / accentLight / accentDark, or transparent, to force one
      shared pressed color. Empty darkens each button's own background;
      buttons on the native surface keep the native pressed state.
  - BorderColor: ""
    $name: Border color
    $description: Hex, accent / accentLight / accentDark, or transparent. Empty uses the native border.
  - BorderThickness: 0
    $name: Border thickness (px)
  - CornerRadius: 4
    $name: Corner radius (px)
    $description: 0 is square; 4 is the Windows default.
  - Opacity: 100
    $name: Opacity (%)
    $description: 100 is fully opaque; lower values let the taskbar show through.
  - ShineEffect: false
    $name: Shine effect
    $description: Adds a gradient highlight to buttons with a custom color.
  - TaskViewFontFamily: ""
    $name: Task View font family
    $description: Font for the Task View button label. Empty uses the native font.
  $name: Surface

- State:
  - ActiveTextColor: ""
    $name: Active desktop text color
    $description: Hex, accent / accentLight / accentDark, or transparent. Empty uses the native text color.
  - InactiveTextColor: ""
    $name: Inactive button text color
    $description: Hex, accent / accentLight / accentDark, or transparent. Empty uses the native text color.
  - ActiveBackgroundColor: "accent"
    $name: Active desktop color
    $description: >-
      Background for the current desktop's button. Hex, accent / accentLight
      / accentDark, or transparent. Empty keeps the native button surface,
      matching the other buttons.
  - InactiveBackgroundColor: ""
    $name: Inactive button color
    $description: >-
      Background for the other desktops' buttons. Hex, accent / accentLight /
      accentDark, or transparent. Empty keeps the native button surface.
  - ActiveBold: false
    $name: Bold the active desktop label
  $name: State

- Behavior:
  - HoverPreview: true
    $name: Desktop hover previews
    $description: Shows an overview of the hovered desktop's windows without switching desktops. Minimized windows are counted; protected or unavailable window images may be blank.
  - PreviewDelay: 400
    $name: Preview delay (ms)
    $description: How long to hover before showing the overview. Clamped to 100-2000 ms.
  - PreviewWidth: 320
    $name: Preview width (px)
    $description: Width of the overview, scaled for your monitor. Clamped to 200-800.
  - HideWhenSingle: false
    $name: Hide when only one desktop
    $description: Don't show the buttons when there is only one virtual desktop.
  $name: Behavior
*/
// ==/WindhawkModSettings==

#undef GetCurrentTime

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Text.h>
#include <winrt/Windows.UI.ViewManagement.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Media.h>

#include <atomic>
#include <mutex>
#include <list>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include <utility>
#include <sstream>
#include <thread>
#include <functional>
#include <algorithm>
#include <climits>
#include <cmath>
#include <cwchar>
#include <cstdlib>
#include <exception>

#include <windhawk_utils.h>
#include <combaseapi.h>
#include <winver.h>

using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Automation;
using namespace winrt::Windows::UI::Xaml::Controls;
using namespace winrt::Windows::UI::Xaml::Media;

// ============================================================
// Unified element placement -- nested-group-layout template v2.5
// Copy-source: _templates/nested-group-layout.h. One expression, one
// setting: Layout.Arrangement is either the word "auto" (this file picks
// the shape and emits the expression, which the mod logs) or an explicit
// expression. Per-item offsets ride in that same string as "1[+2,-1]".
// ============================================================

#include <algorithm>
#include <cwctype>
#include <cstdlib>
#include <functional>
#include <string>
#include <vector>

namespace windhawk_mod_templates::nested_group_layout {

enum class Axis { Horizontal, Vertical };  // node orientation, not a setting
enum class Justify { Start, Center, End };
enum class FillOrder { Rows, Columns };

// An item is sized either absolutely (width x height) or RELATIVE TO THE AXIS
// its group happens to lay out along. Axis-relative sizing exists because an
// item like a Task View button should be "as wide as it needs and as tall as
// the buttons beside it" when it is a column, and the mirror image when it is
// a row — and in a hand-written arrangement the mod cannot know which it will
// be. The parent group knows its own axis, so it resolves this at measure and
// arrange time:
//
//   thickness — extent ALONG the group's axis (its width as a column, its
//               height as a row)
//   cross     — extent ACROSS the group's axis; 0 means fill, i.e. match
//               whatever the rest of the group measures
struct Size {
    double width = 0.0;
    double height = 0.0;
    bool axisRelative = false;
    double thickness = 0.0;
    double cross = 0.0;

    bool Empty() const {
        return axisRelative ? thickness <= 0.0
                            : (width <= 0.0 || height <= 0.0);
    }
};

// Size an item against its group's axis. cross = 0 fills the group.
inline Size AlongAxis(double thickness, double cross = 0.0) {
    Size size;
    size.axisRelative = true;
    size.thickness = thickness;
    size.cross = cross;
    return size;
}

// CONTENT-SIZED ITEMS. A settings-driven item size describes a GLYPH: a box of
// a chosen width that a character is centered in. It does not describe TEXT.
// "9%", "80%", and "100%" are three different widths, a font or locale change
// moves them again, and a battery percentage grows while you watch it. Handing
// such an item the same fixed width as its neighbours reserves too little
// space, and the overflow is discovered at paint time — as a clipped edge.
//
// The SizeResolver is a callback precisely so a mod can answer with something
// it measured. Measure the live element (native_glyph_surface::MeasureNatural)
// and pass the result through here: the arrangement then RESERVES the real
// width, the group's total grows to match, and nothing clips.
//
// `minimum` keeps a short value from collapsing below the item size the user
// chose, so "9%" still lines up with the glyphs above it. Round `measured` up
// and add a pixel or two of slack, or the item will re-measure every time its
// text ticks over.
inline Size ContentAlong(double measured, double minimum, double cross) {
    return {std::max(measured, minimum), cross};
}

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
    // deepest arrangement in this family's own documentation uses three.
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
        return value;
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
// A token is an item's stable IDENTITY, never its displayed label. Labels are
// not unique, can contain the expression's own delimiters, can be empty or an
// emoji, and renaming one would silently break an arrangement the user wrote.
// Each mod declares its vocabulary and documents it:
//
//   fixed set     -> semantic names: wifi, volume, battery, percent, clock
//   dynamic set   -> 1, 2, 3, ... because the set changes at runtime
//   either        -> an extra named item such as "master"
//
// A dynamic mod may accept a readable alias for a number (desktop2 == 2). Log
// the token-to-label map next to the arrangement so a user can tell which
// number is which item without the arrangement depending on the labels.
//
// Matching is case-insensitive: someone typing "Wifi" means wifi.

inline bool TokenIs(std::wstring const& token, wchar_t const* name) {
    size_t i = 0;
    for (; i < token.size() && name[i]; ++i)
        if (towlower(token[i]) != towlower(name[i]))
            return false;
    return i == token.size() && !name[i];
}

// "desktop2" -> 2 with prefix L"desktop"; 0 when the token does not match.
inline int TokenIndexWithPrefix(std::wstring const& token,
                                wchar_t const* prefix) {
    size_t i = 0;
    for (; prefix[i]; ++i)
        if (i >= token.size() || towlower(token[i]) != towlower(prefix[i]))
            return 0;
    if (i >= token.size())
        return 0;
    int value = 0;
    for (; i < token.size(); ++i) {
        if (token[i] < L'0' || token[i] > L'9')
            return 0;
        value = value * 10 + (token[i] - L'0');
    }
    return value;
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

    // The grammar wraps every unit in a group, so most groups have a single
    // child. Such a group IS its child — pass the size through verbatim, or an
    // axis-relative child would be flattened into a concrete size by its own
    // wrapper before the real parent ever sees it.
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
    double fillFallback = 0.0;
    int placed = 0;
    for (auto const& child : node.children) {
        Size size = MeasureCached(child, config, resolve, cache);
        if (size.Empty())
            continue;
        double childMain, childCross;
        if (size.axisRelative) {
            childMain = size.thickness;
            // A filling item takes its cross extent FROM the group, so it must
            // not drive the group's cross size — otherwise it would size itself.
            childCross = size.cross;
            fillFallback = std::max(fillFallback, size.thickness);
        } else {
            childMain =
                node.axis == Axis::Horizontal ? size.width : size.height;
            childCross =
                node.axis == Axis::Horizontal ? size.height : size.width;
        }
        main += (placed ? config.spacing : 0.0) + childMain;
        cross = std::max(cross, childCross);
        ++placed;
    }
    if (!placed)
        return {};
    // Degenerate case: every child fills, so nothing established a cross size.
    // Fall back to the largest thickness rather than collapsing the group.
    if (cross <= 0.0)
        cross = fillFallback;
    return node.axis == Axis::Horizontal ? Size{main, cross}
                                         : Size{cross, main};
}

// Measure one tree on its own. Prefer Compute(), which shares a single cache
// across the measure and arrange passes; this overload exists for call sites
// that measure a tree by itself.
inline Size Measure(Node const& node, Config const& config,
                    SizeResolver const& resolve) {
    MeasureCache cache;
    return MeasureCached(node, config, resolve, cache);
}

// Resolve a child's size against its parent group's axis, so an axis-relative
// item becomes concrete width x height.
inline Size ConcreteSize(Size const& size, Axis axis, Size const& groupTotal) {
    if (!size.axisRelative)
        return size;
    double groupCross =
        axis == Axis::Horizontal ? groupTotal.height : groupTotal.width;
    double cross = size.cross > 0.0 ? size.cross : groupCross;
    return axis == Axis::Horizontal ? Size{size.thickness, cross}
                                    : Size{cross, size.thickness};
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

    // Single-child group: forward the size the real parent already resolved,
    // so axis-relative sizing survives the grammar's per-unit wrapper.
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
            ArrangeCached(*only, config, resolve, x, y, out, cache,
                          resolvedSize);
            return;
        }
    }

    double cursor = node.axis == Axis::Horizontal ? x : y;
    for (auto const& child : node.children) {
        Size measured = MeasureCached(child, config, resolve, cache);
        if (measured.Empty())
            continue;
        Size size = ConcreteSize(measured, node.axis, total);
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

// Arrange one tree on its own. Prefer Compute(); this overload exists for call
// sites that drive the arranger directly.
inline void Arrange(Node const& node, Config const& config,
                    SizeResolver const& resolve, double x, double y,
                    std::vector<Placement>& out,
                    Size const* resolvedSize = nullptr) {
    MeasureCache cache;
    ArrangeCached(node, config, resolve, x, y, out, cache, resolvedSize);
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
    if (inner.axisRelative) {
        // The whole arrangement is one axis-relative item, so there is no group
        // for it to fill against; square it off on its own thickness.
        double cross = inner.cross > 0.0 ? inner.cross : inner.thickness;
        inner = Size{inner.thickness, cross};
    }
    ArrangeCached(root, config, resolve, config.padX, config.padY, placements,
                  cache, &inner);
    totalSize = {inner.width + config.padX * 2.0,
                 inner.height + config.padY * 2.0};
    return true;
}

// ---- Taskbar metrics --------------------------------------------------------
//
// The taskbar rect comes from GetWindowRect in PHYSICAL pixels while every XAML
// size is a DIP. Dividing one by the other is the DPI bug flagged on PR #4855
// (blocking) and #4843. The mod supplies the raw numbers:
//
//   RECT r{}; GetWindowRect(hTaskbarWnd, &r);
//   int rows = AvailableRows(r.bottom - r.top, GetDpiForWindow(hTaskbarWnd),
//                            itemHeight, spacing);

inline double PixelsToDip(double physicalPixels, unsigned dpi) {
    return dpi ? physicalPixels * 96.0 / (double)dpi : physicalPixels;
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

// Convenience for the common case with nothing else reserved.
inline int AvailableRows(double taskbarHeightPx, unsigned dpi,
                         double itemHeight, double spacing) {
    return RowsInHeight(PixelsToDip(taskbarHeightPx, dpi), itemHeight, spacing);
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
// A hand-written arrangement names the items that existed when it was written.
// When the set is dynamic — a desktop is added, a folder appears — the new item
// is in no group, resolves to nothing, and silently vanishes from the taskbar.
// That is a trap, so a mod with a dynamic set offers a policy:
//
//   Append (default) — arrange the unlisted items automatically and put that
//                      block after everything the user wrote, so a new item is
//                      always reachable and the written block stays intact.
//   Ignore           — the arrangement is the whole truth; unlisted items stay
//                      off the taskbar until the user adds them.
//
// A mod that appends should log that it did, so the user knows to fold the new
// item into their arrangement when they next edit it.

// Whether a token the user wrote refers to the same item as one the mod
// expects. Defaults to a case-insensitive name match, which is WRONG for any
// mod that accepts aliases: "desktop1" and "1" are the same button, and
// comparing them as strings makes every aliased item look missing and get
// appended a second time. A mod with a vocabulary must supply this.
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

}  // namespace windhawk_mod_templates::nested_group_layout

namespace ngl = windhawk_mod_templates::nested_group_layout;

// ============================================================
// Color tokens
// Template block: _templates/color-tokens.h (verbatim copy —
// keep in sync with the template; Windhawk mods are single-file).
// ============================================================

namespace windhawk_mod_templates::color_tokens {

using winrt::Windows::UI::Color;
using winrt::Windows::UI::Xaml::Media::Brush;
using winrt::Windows::UI::Xaml::Media::SolidColorBrush;

// Reported when the Windows accent color cannot be read, so the mod can log.
using AccentErrorFn = void (*)();

// false means "no color here" — an empty setting, an unknown token, or bad
// hex. Callers must treat all three the same: leave the native value alone.
inline bool Parse(wchar_t const* value, Color& out,
                  AccentErrorFn onAccentError = nullptr) {
    using winrt::Windows::UI::ViewManagement::UIColorType;
    if (!value || !*value) return false;

    if (_wcsicmp(value, L"transparent") == 0) {
        out = {0, 0, 0, 0};
        return true;
    }

    static const struct {
        wchar_t const* token;
        UIColorType type;
    } kAccentTokens[] = {
        {L"accent", UIColorType::Accent},
        {L"accentLight", UIColorType::AccentLight2},
        {L"accentDark", UIColorType::AccentDark1},
        {L"accentLight1", UIColorType::AccentLight1},
        {L"accentLight2", UIColorType::AccentLight2},
        {L"accentLight3", UIColorType::AccentLight3},
        {L"accentDark1", UIColorType::AccentDark1},
        {L"accentDark2", UIColorType::AccentDark2},
        {L"accentDark3", UIColorType::AccentDark3},
    };
    for (auto const& entry : kAccentTokens) {
        if (_wcsicmp(value, entry.token) != 0) continue;
        try {
            winrt::Windows::UI::ViewManagement::UISettings settings;
            out = settings.GetColorValue(entry.type);
            return true;
        } catch (...) {
            if (onAccentError) onAccentError();
            return false;
        }
    }

    wchar_t const* digits = (*value == L'#') ? value + 1 : value;
    size_t length = wcslen(digits);
    if (length != 6 && length != 8) return false;
    for (size_t i = 0; i < length; ++i) {
        if (!iswxdigit(digits[i])) return false;
    }
    wchar_t buffer[9]{};
    wcsncpy(buffer, digits, 8);
    unsigned long packed = wcstoul(buffer, nullptr, 16);
    if (length == 6) {
        out = {255, BYTE(packed >> 16), BYTE(packed >> 8), BYTE(packed)};
    } else {
        out = {BYTE(packed >> 24), BYTE(packed >> 16), BYTE(packed >> 8),
               BYTE(packed)};
    }
    return true;
}

// nullptr means "no color here". Never a fallback brush — a caller that wrote
// a default color on parse failure would make an empty setting paint.
inline Brush ParseBrush(wchar_t const* value,
                        AccentErrorFn onAccentError = nullptr) {
    Color color{};
    if (!Parse(value, color, onAccentError)) return nullptr;
    SolidColorBrush brush;
    brush.Color(color);
    return brush;
}

}  // namespace windhawk_mod_templates::color_tokens

namespace clr = windhawk_mod_templates::color_tokens;

// ============================================================
// Visual tree walk
// Template block: _templates/visual-tree-walk.h (verbatim copy —
// keep in sync with the template; Windhawk mods are single-file).
// ============================================================

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

}  // namespace windhawk_mod_templates::visual_tree_walk

namespace vtw = windhawk_mod_templates::visual_tree_walk;

// ============================================================
// Settings IO
// Template block: _templates/settings-io.h (verbatim copy —
// keep in sync with the template; Windhawk mods are single-file).
// ============================================================

namespace windhawk_mod_templates::settings_io {

inline int Clamp(int value, int low, int high) {
    return std::max(low, std::min(high, value));
}

// Frees on every path, including the ones a hand-written loader forgets.
class StringSetting {
public:
    explicit StringSetting(PCWSTR key) : value_(Wh_GetStringSetting(key)) {}
    ~StringSetting() {
        if (value_) Wh_FreeStringSetting(value_);
    }
    StringSetting(StringSetting const&) = delete;
    StringSetting& operator=(StringSetting const&) = delete;

    // Never nullptr in practice, but do not rely on that at the call site.
    PCWSTR Get() const { return value_ ? value_ : L""; }
    bool Empty() const { return !value_ || !value_[0]; }

private:
    PCWSTR value_ = nullptr;
};

// Copy a string setting into a fixed buffer, always NUL-terminated. Fixed
// buffers rather than std::wstring because a namespace-scope settings struct
// must not own heap — see the exit-time destructor audit.
template <size_t N>
inline void LoadString(PCWSTR key, wchar_t (&buffer)[N]) {
    StringSetting setting(key);
    if (setting.Empty()) {
        buffer[0] = L'\0';
        return;
    }
    wcsncpy(buffer, setting.Get(), N - 1);
    buffer[N - 1] = L'\0';
}

// Same, but substitutes `fallback` when the setting is empty.
template <size_t N>
inline void LoadString(PCWSTR key, wchar_t (&buffer)[N], PCWSTR fallback) {
    LoadString(key, buffer);
    if (!buffer[0] && fallback) {
        wcsncpy(buffer, fallback, N - 1);
        buffer[N - 1] = L'\0';
    }
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
// Use this rather than a chain of _wcsicmp: after ANY option is renamed, a
// stale literal in a hand-written chain fails silently and the mod quietly
// falls back. That cost this lab a release (Indicator symbols reverted to
// numbers because `labelFormat == L"dot"` was never true again).
template <typename T>
struct Choice {
    wchar_t const* token;
    T value;
};

template <typename T, size_t N>
inline T LoadChoice(PCWSTR key, Choice<T> const (&choices)[N], T fallback) {
    StringSetting setting(key);
    if (setting.Empty()) return fallback;
    for (auto const& choice : choices) {
        if (_wcsicmp(setting.Get(), choice.token) == 0) return choice.value;
    }
    return fallback;
}

}  // namespace windhawk_mod_templates::settings_io

namespace sio = windhawk_mod_templates::settings_io;

// ============================================================
// Taskbar host
// Template block: _templates/taskbar-host.h (verbatim copy —
// keep in sync with the template; Windhawk mods are single-file).
// ============================================================

namespace windhawk_mod_templates::taskbar_host {

using winrt::Windows::UI::Xaml::FrameworkElement;
using winrt::Windows::UI::Xaml::XamlRoot;

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

// A CACHED TASKBAR HANDLE IS NOT PROOF THE WINDOW STILL EXISTS. Shell_TrayWnd
// can be recreated inside the same Explorer process, and every mod here cached
// it and then preferred the cache unconditionally:
//
//     HWND w = g_taskbarWnd ? g_taskbarWnd : FindCurrentProcessTaskbarWnd();
//
// After a recreate that hands back a dead handle forever, because the live
// window is only ever looked up when the cache is null. GetWindowThreadProcessId
// then returns 0, RunFromWindowThread fails, and the caller silently does
// nothing — which is survivable on a retry path but not on the unload path,
// where it means the mod's callbacks are never revoked before its image is
// freed. Flagged by the AI review on PR #4855. Validate, then fall back.
inline HWND ResolveTaskbarWnd(HWND cached) {
    if (cached && IsWindow(cached))
        return cached;
    return FindCurrentProcessTaskbarWnd();
}

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
// with it — which is exactly what an earlier revision of this template did.
// Atomic because the caller may be the retry thread while the hook proc runs
// on the taskbar's UI thread. RegisterWindowMessageW returns the same value
// for the same string for the lifetime of the session, so this settles on one
// value immediately and never changes again — the pre-template code got the
// same property from a function-local `static UINT` magic static, which a
// parameterised template cannot use.
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
                            reinterpret_cast<Dispatch*>(call->lParam)) {
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
        if (g_logException) g_logException(L"TrayUI::StartTaskbar hook");
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

// ---- Taskbar metrics and orientation ----------------------------------------
//
// WHERE THE TASKBAR IS, AND WHETHER THIS FAMILY CAN WORK THERE.
//
// Windows 11 itself only puts the taskbar at the bottom. Two mods by m417z
// move it, and both are first-class parts of the ecosystem these mods have to
// live in:
//
//   taskbar-on-top       — bottom -> top. FINE for this family. Everything
//                          here is positioned relative to the taskbar's own
//                          XAML tree, never to screen coordinates, so a top
//                          taskbar is the same tree at a different y.
//
//   taskbar-vertical     — bottom -> left/right. NOT COMPATIBLE, and not for
//                          a reason cooperation can fix. It walks the very
//                          same path this family walks
//                          (ControlCenterButton > Grid > ContentPresenter >
//                          ItemsPresenter > StackPanel) and applies a
//                          RotateTransform to `RenderTransform` on those
//                          children. Positioning here sets a
//                          TranslateTransform on the SAME property of the SAME
//                          elements. One dependency property, two owners, last
//                          writer wins — there is no version of this where
//                          both mods are correct. m417z documents the same
//                          class of conflict for taskbar-multirow.
//
// So: DETECT AND STAND DOWN, loudly, rather than fight and paint garbage. The
// detection is the taskbar's own rect aspect, not a check for a specific mod —
// it is the condition that matters, and it stays true however the taskbar got
// that way.
//
// The rect is in PHYSICAL pixels and every XAML size is a DIP, so the DIP
// conversion lives here too rather than being re-derived per mod. That is the
// bug that was blocking on PR #4855 and #4843.

enum class Orientation { Horizontal, Vertical };

struct Metrics {
    bool valid = false;
    RECT rect{};
    UINT dpi = 96;
    Orientation orientation = Orientation::Horizontal;
    // The extent this family's grid has to fit INTO: the taskbar's height when
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

// Whether this family's layout model applies at all. A mod must check this
// BEFORE touching anything and stand down cleanly if it is false — leaving the
// taskbar exactly as it found it — rather than arranging into a coordinate
// space someone else is rotating.
inline bool LayoutModelApplies(Metrics const& metrics) {
    return metrics.valid && metrics.orientation == Orientation::Horizontal;
}

inline wchar_t const* OrientationName(Orientation orientation) {
    return orientation == Orientation::Vertical ? L"vertical" : L"horizontal";
}

// ---- Bounded retry ----------------------------------------------------------
//
// Stoppable and WAITED during unload. A detached thread that outlives
// Wh_ModUninit runs mod code out of an unloaded DLL.

class RetryLoop {
public:
    // applied: has the work finished? unloading: stop immediately.
    using AppliedFn = bool (*)();
    using AttemptFn = void (*)();

    void Start(AttemptFn attempt, AppliedFn applied,
               std::atomic<bool> const& unloading, int attempts = 5,
               DWORD intervalMs = 2000, bool forceFirstAttempt = false) {
        Stop();
        if (unloading) return;
        attempt_ = attempt;
        applied_ = applied;
        unloading_ = &unloading;
        attempts_ = attempts;
        intervalMs_ = intervalMs;
        forceFirstAttempt_ = forceFirstAttempt;
        stopEvent_ = CreateEventW(nullptr, TRUE, FALSE, nullptr);
        if (!stopEvent_) return;
        thread_ = CreateThread(
            nullptr, 0,
            [](void* parameter) -> DWORD {
                auto* self = static_cast<RetryLoop*>(parameter);
                for (int i = 0; i < self->attempts_ && !*self->unloading_;
                     ++i) {
                    // A settings reload can need one restore/reapply pass even
                    // while `applied` truthfully says we still own live XAML.
                    // Do not overload that ownership flag merely to wake the
                    // retry loop; request a forced first attempt instead.
                    if (self->applied_ &&
                        !(self->forceFirstAttempt_ && i == 0) &&
                        self->applied_()) break;
                    if (i && WaitForSingleObject(self->stopEvent_,
                                                 self->intervalMs_) !=
                                 WAIT_TIMEOUT)
                        break;
                    if (self->attempt_) self->attempt_();
                }
                return 0;
            },
            this, 0, nullptr);
        if (!thread_) {
            CloseHandle(stopEvent_);
            stopEvent_ = nullptr;
        }
    }

    // Pumps sent messages while waiting: the retry thread marshals onto the UI
    // thread with SendMessage, so a plain wait from that same UI thread would
    // deadlock against the thread it is waiting for.
    void Stop() {
        if (stopEvent_) SetEvent(stopEvent_);
        if (thread_) {
            DWORD result;
            do {
                result = MsgWaitForMultipleObjects(1, &thread_, FALSE, INFINITE,
                                                   QS_SENDMESSAGE);
                if (result == WAIT_OBJECT_0 + 1) {
                    MSG message;
                    PeekMessageW(&message, nullptr, 0, 0, PM_NOREMOVE);
                }
            } while (result == WAIT_OBJECT_0 + 1);
            CloseHandle(thread_);
            thread_ = nullptr;
        }
        if (stopEvent_) {
            CloseHandle(stopEvent_);
            stopEvent_ = nullptr;
        }
    }

private:
    HANDLE thread_ = nullptr;
    HANDLE stopEvent_ = nullptr;
    AttemptFn attempt_ = nullptr;
    AppliedFn applied_ = nullptr;
    std::atomic<bool> const* unloading_ = nullptr;
    int attempts_ = 5;
    DWORD intervalMs_ = 2000;
    bool forceFirstAttempt_ = false;
};

}  // namespace windhawk_mod_templates::taskbar_host

namespace tbh = windhawk_mod_templates::taskbar_host;

// ============================================================
// Property lease
// Template block: _templates/property-lease.h (verbatim copy —
// keep in sync with the template; Windhawk mods are single-file).
// ============================================================

namespace windhawk_mod_templates::property_lease {

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

    // Drop the snapshots WITHOUT restoring. For the case where the elements
    // are already gone (an Explorer rebuild threw the tree away), so restoring
    // would only throw. Do not use it to "skip" a restore that could run.
    void Abandon() { snapshots_.clear(); }

    size_t Count() const { return snapshots_.size(); }
    bool Empty() const { return snapshots_.empty(); }

private:
    std::vector<Snapshot> snapshots_;
};

}  // namespace windhawk_mod_templates::property_lease

namespace ple = windhawk_mod_templates::property_lease;
using winrt::Windows::UI::Xaml::Controls::Primitives::ButtonBase;
using winrt::Windows::UI::Xaml::Controls::Primitives::ToggleButton;

// ============================================================
// Settings
// ============================================================

// Field names mirror the settings contract in _templates/settings-profiles.md:
// one nested group per concern, canonical keys in canonical order.
// $options values are parsed ONCE, at load, into these. Nothing downstream
// compares a setting against a string literal - that is the failure the
// settings-io template exists to prevent, and THIS mod is where it bit: after
// an option value was renamed, `labelFormat == L"dot"` kept compiling, stopped
// matching, and the Indicator symbols silently reverted to numbers.
enum class VdPosition {
    BeforeIcons, BeforeOmni, BeforeClock, AfterClock, AfterShowDesktop,
    LeftOfStart, OverStart, RightOfStart,
};
enum class VdLabelFormat { Number, Roman, Symbol, Custom };
enum class VdTaskViewPlacement { Before, After, Above, Below, InGrid };

static PCWSTR VdPositionName(VdPosition p) {
    switch (p) {
        case VdPosition::BeforeIcons:      return L"beforeIcons";
        case VdPosition::BeforeClock:      return L"beforeClock";
        case VdPosition::AfterClock:       return L"afterClock";
        case VdPosition::AfterShowDesktop: return L"afterShowDesktop";
        case VdPosition::LeftOfStart:      return L"leftOfStart";
        case VdPosition::OverStart:        return L"overStart";
        case VdPosition::RightOfStart:     return L"rightOfStart";
        default:                           return L"beforeOmni";
    }
}

struct ModSettings {
    // Placement
    VdPosition   position = VdPosition::BeforeOmni;
    bool         allTaskbars       = false;
    // Content
    VdLabelFormat labelFormat = VdLabelFormat::Number;
    std::wstring customLabels      = L"";
    std::wstring activeSymbol      = L"●";
    std::wstring inactiveSymbol    = L"○";
    bool         taskViewButton    = false;
    std::wstring taskViewLabel     = L"⊞";
    VdTaskViewPlacement taskViewPlacement = VdTaskViewPlacement::Before;
    // Layout
    std::wstring arrangement       = L"auto";
    ngl::FillOrder fillOrder = ngl::FillOrder::Rows;
    ngl::Justify justify = ngl::Justify::Center;
    bool         appendNewItems = true;
    // Size
    int          itemWidth         = 20;
    int          itemHeight        = 22;
    int          itemSpacing       = 2;
    int          taskViewSize      = 14;
    int          taskViewSpan      = 0;
    int          taskViewGap       = 0;
    // Adjust
    int          padX              = 0;
    int          padY              = 0;
    int          offsetX           = 0;
    int          offsetY           = 0;
    // Surface
    int          fontSize          = 10;
    std::wstring fontFamily        = L"";
    std::wstring hoverBackgroundColor;
    std::wstring pressedBackgroundColor;
    std::wstring borderColor       = L"";
    int          borderThickness   = 0;
    int          cornerRadius      = 4;
    int          opacity           = 100;
    bool         shineEffect       = false;
    std::wstring taskViewFontFamily = L"";
    // State
    std::wstring activeTextColor   = L"";
    std::wstring inactiveTextColor = L"";
    std::wstring activeBackgroundColor   = L"accent";
    std::wstring inactiveBackgroundColor = L"";
    bool         activeBold        = false;
    // Behavior
    bool         hideWhenSingle    = false;
    bool         hoverPreview = true;
    int          previewDelay = 400;
    int          previewWidth = 320;
};
// ModSettings holds only std::wstring/int/bool, so its destructor is safe to
// run at process shutdown; no [[clang::no_destroy]] needed, and adding it would
// leak the string buffers on every normal unload.
ModSettings g_settings;  // exit-time-safe: heap-only

static void LoadSettings() {
    // Free-form strings only: every $options value is parsed to an enum by
    // sio::LoadChoice below. StringSetting frees on every path, including the
    // ones a hand-written loader forgets.
    auto Str = [](const wchar_t* k) {
        sio::StringSetting setting(k);
        return std::wstring(setting.Get());
    };
    auto Int = [](const wchar_t* k) { return Wh_GetIntSetting(k); };
    auto Bool = [](const wchar_t* k) { return sio::LoadBool(k); };

    static constexpr sio::Choice<VdPosition> kPositions[] = {
        {L"beforeIcons", VdPosition::BeforeIcons},
        {L"beforeOmni", VdPosition::BeforeOmni},
        {L"beforeClock", VdPosition::BeforeClock},
        {L"afterClock", VdPosition::AfterClock},
        {L"afterShowDesktop", VdPosition::AfterShowDesktop},
        {L"leftOfStart", VdPosition::LeftOfStart},
        {L"overStart", VdPosition::OverStart},
        {L"rightOfStart", VdPosition::RightOfStart},
    };
    g_settings.position = sio::LoadChoice(L"Placement.Position", kPositions,
                                          VdPosition::BeforeOmni);
    g_settings.allTaskbars       = Bool(L"Placement.AllTaskbars");

    static constexpr sio::Choice<VdLabelFormat> kLabelFormats[] = {
        {L"number", VdLabelFormat::Number},
        {L"roman", VdLabelFormat::Roman},
        {L"symbol", VdLabelFormat::Symbol},
        {L"custom", VdLabelFormat::Custom},
    };
    g_settings.labelFormat = sio::LoadChoice(L"Content.LabelFormat",
                                             kLabelFormats,
                                             VdLabelFormat::Number);
    g_settings.customLabels      = Str(L"Content.CustomLabels");
    g_settings.activeSymbol      = Str(L"Content.ActiveSymbol");
    g_settings.inactiveSymbol    = Str(L"Content.InactiveSymbol");
    g_settings.taskViewButton    = Bool(L"Content.TaskViewButton");
    g_settings.taskViewLabel     = Str(L"Content.TaskViewLabel");
    static constexpr sio::Choice<VdTaskViewPlacement> kTaskViewPlacements[] = {
        {L"before", VdTaskViewPlacement::Before},
        {L"after", VdTaskViewPlacement::After},
        {L"above", VdTaskViewPlacement::Above},
        {L"below", VdTaskViewPlacement::Below},
        {L"inGrid", VdTaskViewPlacement::InGrid},
    };
    g_settings.taskViewPlacement = sio::LoadChoice(
        L"Content.TaskViewPlacement", kTaskViewPlacements,
        VdTaskViewPlacement::Before);

    g_settings.arrangement       = Str(L"Layout.Arrangement");
    static constexpr sio::Choice<ngl::FillOrder> kFillOrders[] = {
        {L"rows", ngl::FillOrder::Rows},
        {L"columns", ngl::FillOrder::Columns},
    };
    g_settings.fillOrder = sio::LoadChoice(L"Layout.FillOrder", kFillOrders,
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
        {L"ignore", false},
    };
    g_settings.appendNewItems = sio::LoadChoice(L"Layout.NewItems",
                                                kNewItemPolicies, true);

    g_settings.itemWidth         = std::max(1, Int(L"Size.ItemWidth"));
    g_settings.itemHeight        = std::max(1, Int(L"Size.ItemHeight"));
    g_settings.itemSpacing       = std::max(0, Int(L"Size.ItemSpacing"));
    g_settings.taskViewSize      = std::max(1, Int(L"Size.TaskViewSize"));
    g_settings.taskViewSpan      = std::max(0, Int(L"Size.TaskViewSpan"));
    g_settings.taskViewGap       = Int(L"Size.TaskViewGap");

    g_settings.padX              = std::max(0, Int(L"Adjust.PadX"));
    g_settings.padY              = std::max(0, Int(L"Adjust.PadY"));
    g_settings.offsetX           = Int(L"Adjust.OffsetX");
    g_settings.offsetY           = Int(L"Adjust.OffsetY");

    g_settings.fontSize          = Int(L"Surface.FontSize");
    g_settings.fontFamily        = Str(L"Surface.FontFamily");
    g_settings.hoverBackgroundColor   = Str(L"Surface.HoverBackgroundColor");
    g_settings.pressedBackgroundColor = Str(L"Surface.PressedBackgroundColor");
    g_settings.borderColor       = Str(L"Surface.BorderColor");
    g_settings.borderThickness   = Int(L"Surface.BorderThickness");
    g_settings.cornerRadius      = Int(L"Surface.CornerRadius");
    g_settings.opacity           = Int(L"Surface.Opacity");
    g_settings.shineEffect       = Bool(L"Surface.ShineEffect");
    g_settings.taskViewFontFamily = Str(L"Surface.TaskViewFontFamily");

    g_settings.activeTextColor   = Str(L"State.ActiveTextColor");
    g_settings.inactiveTextColor = Str(L"State.InactiveTextColor");
    g_settings.activeBackgroundColor   = Str(L"State.ActiveBackgroundColor");
    g_settings.inactiveBackgroundColor = Str(L"State.InactiveBackgroundColor");
    g_settings.activeBold        = Bool(L"State.ActiveBold");

    g_settings.hideWhenSingle    = Bool(L"Behavior.HideWhenSingle");
    g_settings.hoverPreview = Bool(L"Behavior.HoverPreview");
    g_settings.previewDelay = sio::LoadInt(L"Behavior.PreviewDelay", 100, 2000);
    g_settings.previewWidth = sio::LoadInt(L"Behavior.PreviewWidth", 200, 800);

    auto shownColor = [](std::wstring const& value) {
        return value.empty() ? L"<empty/automatic>" : value.c_str();
    };
    Wh_Log(L"[Settings] colors active=%ls inactive=%ls hover=%ls pressed=%ls border=%ls",
           shownColor(g_settings.activeBackgroundColor),
           shownColor(g_settings.inactiveBackgroundColor),
           shownColor(g_settings.hoverBackgroundColor),
           shownColor(g_settings.pressedBackgroundColor),
           shownColor(g_settings.borderColor));
}
// ============================================================
// Globals
// ============================================================

static std::atomic<bool> g_unloading{false};
static HWND              g_taskbarWnd      = nullptr;
[[clang::no_destroy]] static Grid g_buttonGrid = nullptr;
[[clang::no_destroy]] static FrameworkElement g_injectionParent = nullptr;
static int               g_injectedColumn  = -1;
static bool              g_startOverlayMode = false;
[[clang::no_destroy]] static FrameworkElement g_startOverlayRoot = nullptr;
[[clang::no_destroy]] static FrameworkElement g_startOverlayStart = nullptr;
static winrt::event_token g_startOverlayLayoutToken{};
[[clang::no_destroy]] static FrameworkElement g_taskItemsPanel = nullptr;
static Thickness         g_taskItemsPanelOriginalMargin{};
static double            g_startButtonOriginalX = -1.0;
static std::atomic<int>  g_currentDesktop{0};
static std::atomic<int>  g_desktopCount{1};

static HANDLE g_notificationThread    = nullptr;
static HANDLE g_notificationStopEvent = nullptr;
static DWORD  g_notificationCookie    = 0;

static HANDLE g_retryThread    = nullptr;
static HANDLE g_retryStopEvent = nullptr;

static std::atomic<bool> g_systemTrayModuleHooked{false};
// Desktop-switch workers must finish before Windhawk unloads this image.
// Handles, unlike a counter decremented inside a thread proc, establish that.
[[clang::no_destroy]] static std::mutex g_switchThreadsMutex;
[[clang::no_destroy]] static std::optional<std::vector<HANDLE>>
    g_switchThreads{std::in_place};
[[clang::no_destroy]] static std::optional<std::list<FrameworkElement::Loaded_revoker>>
    g_autoRevokerList{std::in_place};

struct ButtonEventState {
    Grid owner{nullptr};
    ButtonBase button{nullptr};
    winrt::event_token clickToken{};
    winrt::event_token previewEnter{}, previewExit{}, previewCancel{};
};
[[clang::no_destroy]] static std::optional<std::vector<ButtonEventState>>
    g_buttonEventStates{std::in_place};

// Forward declarations
static void ApplyAllSettings();
static void ApplyAllSettingsOnWindowThread();
static void RebuildButtonGrid();
static void RemoveButtonGrid();
static void StopNotificationThread();
static void StopRetryThread();
static void HandleLoadedModuleIfSystemTray(HMODULE hModule, LPCWSTR lpLibFileName);
static void WaitForSwitchThreads();

// ============================================================
// Explorer / twinui build detection
// ============================================================

static WORD g_explorerBuild    = 0;
static WORD g_explorerRevision = 0;
static WORD g_twinuiBuild      = 0;

static void DetectExplorerBuild() {
    wchar_t path[MAX_PATH];
    GetModuleFileNameW(nullptr, path, MAX_PATH);
    DWORD dummy;
    DWORD sz = GetFileVersionInfoSizeW(path, &dummy);
    if (!sz) return;
    std::vector<BYTE> buf(sz);
    if (!GetFileVersionInfoW(path, 0, sz, buf.data())) return;
    VS_FIXEDFILEINFO* fi = nullptr; UINT fs = 0;
    if (!VerQueryValueW(buf.data(), L"\\", (void**)&fi, &fs)) return;
    g_explorerBuild    = HIWORD(fi->dwFileVersionLS);
    g_explorerRevision = LOWORD(fi->dwFileVersionLS);
    Wh_Log(L"[Init] Explorer build %u rev %u", g_explorerBuild, g_explorerRevision);
}

static VS_FIXEDFILEINFO* GetModuleVersionInfo(HMODULE hModule, UINT* puPtrLen) {
    void* pFixedFileInfo = nullptr;
    UINT uPtrLen = 0;
    HRSRC hResource = FindResource(hModule, MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION);
    if (hResource) {
        HGLOBAL hGlobal = LoadResource(hModule, hResource);
        if (hGlobal) {
            void* pData = LockResource(hGlobal);
            if (pData) {
                if (!VerQueryValue(pData, L"\\", &pFixedFileInfo, &uPtrLen) || uPtrLen == 0) {
                    pFixedFileInfo = nullptr;
                    uPtrLen = 0;
                }
            }
        }
    }
    if (puPtrLen) *puPtrLen = uPtrLen;
    return (VS_FIXEDFILEINFO*)pFixedFileInfo;
}

static bool LoadTwinuiBuild() {
    if (g_twinuiBuild) return true;
    HMODULE h = GetModuleHandleW(L"twinui.pcshell.dll");
    if (!h) return false;
    VS_FIXEDFILEINFO* fi = GetModuleVersionInfo(h, nullptr);
    if (!fi) return false;
    g_twinuiBuild = HIWORD(fi->dwFileVersionLS);
    Wh_Log(L"[VD] twinui.pcshell.dll build %u", g_twinuiBuild);
    return true;
}

// Order matters: SystemTray.dll is the new home (Win11 Insider 26200+);
// older builds have the symbols in Taskbar.View.dll.
static HMODULE GetSystemTrayModuleHandle() {
    HMODULE module = GetModuleHandleW(L"SystemTray.dll");
    if (!module) {
        module = GetModuleHandleW(L"Taskbar.View.dll");
        if (module) {
            // Starting with Taskbar.View.dll 2604.x, the SystemTray types moved
            // out into SystemTray.dll — don't hook this version.
            VS_FIXEDFILEINFO* fi = GetModuleVersionInfo(module, nullptr);
            WORD moduleMajor = fi ? HIWORD(fi->dwFileVersionMS) : 0;
            if (!moduleMajor || moduleMajor >= 2604) {
                Wh_Log(L"[Hooks] Skipping Taskbar.View.dll version %d", moduleMajor);
                module = nullptr;
            }
        }
    }
    if (!module)
        module = GetModuleHandleW(L"ExplorerExtensions.dll");
    return module;
}

// ============================================================
// GetTaskbarXamlRoot boilerplate (from vertical-omnibutton)
// ============================================================

// All of this is now _templates/taskbar-host.h, embedded above. The mod keeps
// only its own exception logger and its rebuild callback.
//
// The CTaskBand walk, the runtime-disassembled FrameworkElement offset, the
// taskbar.dll symbol hooks and the CALLWNDPROC dispatch all moved out
// verbatim. The dispatch is the one worth naming: a WH_CALLWNDPROC hook sees
// every message sent to every window on the taskbar's UI thread, so the
// message must be compared BEFORE lParam is treated as a Dispatch*. This mod
// already got that right; the template now states the rule in capitals
// because reordering it once took Explorer down in OmniButton.

using RunFromWindowThreadProc_t = tbh::ThreadProc;

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

static bool RunFromWindowThread(HWND hWnd, RunFromWindowThreadProc_t proc,
                                void* procParam) {
    return tbh::RunFromWindowThread(
        hWnd, proc, procParam,
        L"Windhawk_RunFromWindowThread_" WH_MOD_ID);
}

static HWND FindCurrentProcessTaskbarWnd() {
    return tbh::FindCurrentProcessTaskbarWnd();
}

static XamlRoot GetTaskbarXamlRoot(HWND hTaskbarWnd) {
    return tbh::GetTaskbarXamlRoot(hTaskbarWnd);
}

// ============================================================
// XAML helpers
// ============================================================

// Kept as a thin wrapper rather than replacing every call site: the signature
// takes its predicate by value and many lambdas here rely on that. The walk
// itself is now the template's.
static FrameworkElement FindChildRecursive(FrameworkElement const& element,
    std::function<bool(FrameworkElement)> const& cb, int maxDepth = 20)
{
    return vtw::FindDescendant(
        element, maxDepth,
        [&cb](FrameworkElement const& child) { return cb(child); });
}

// ============================================================
// VD COM notification infrastructure
// ============================================================

const CLSID CLSID_ImmersiveShell = {
    0xc2f03a33,0x21f5,0x47fa,{0xb4,0xbb,0x15,0x63,0x62,0xa2,0xf2,0x39}
};
const GUID SID_VirtualDesktopNotificationService = {
    0xa501fdec,0x4a09,0x464c,{0xae,0x4e,0x1b,0x9c,0x21,0xb8,0x49,0x18}
};
const GUID IID_IVirtualDesktopNotificationService_G = {
    0x0cd45e71,0xd927,0x4f15,{0x8b,0x0a,0x8f,0xef,0x52,0x53,0x37,0xbf}
};

MIDL_INTERFACE("0CD45E71-D927-4F15-8B0A-8FEF525337BF")
IVirtualDesktopNotificationService_I : public IUnknown {
    virtual HRESULT STDMETHODCALLTYPE Register(IUnknown*, DWORD*) = 0;
    virtual HRESULT STDMETHODCALLTYPE Unregister(DWORD) = 0;
};

struct NotifConfig {
    int64_t iidPart1 = 0, iidPart2 = 0;
    int methodCount = 0, createdIdx = -1, destroyedIdx = -1, currentChangedIdx = -1;
    bool hasMonitors = false;
};

struct NotifObject {
    void** vtable  = nullptr;
    LONG refCount  = 1;
};

static NotifConfig GetNotifConfig() {
    if (g_explorerBuild < 22000) return {};
    if (g_explorerBuild < 22483 || (g_explorerBuild == 22621 && g_explorerRevision < 2215))
        return { 5481970284372180562ll, -1679294552252794956ll, 13, 7, 9, 11, true };
    if (g_explorerBuild < 22631 || (g_explorerBuild == 22631 && g_explorerRevision < 3085))
        return { 5123538856297626140ll,  8491238173783613346ll, 14, 6, 8, 10, false };
    return     { 5308375338100058445ll, -2401892766147978065ll, 14, 6, 8, 10, false };
}

static bool IsOurNotifIface(REFIID riid) {
    auto cfg = GetNotifConfig();
    if (!cfg.methodCount) return false;
    auto p = reinterpret_cast<const int64_t*>(&riid);
    return p[0] == cfg.iidPart1 && p[1] == cfg.iidPart2;
}

static HRESULT STDMETHODCALLTYPE Notif_QI(NotifObject* p, REFIID riid, void** ppv) {
    if (!ppv) return E_POINTER; *ppv = nullptr;
    static const GUID IID_IUnknown_ = {0,0,0,{0xc0,0,0,0,0,0,0,0x46}};
    if (InlineIsEqualGUID(riid, IID_IUnknown_) || IsOurNotifIface(riid)) {
        *ppv = p; InterlockedIncrement(&p->refCount); return S_OK;
    }
    return E_NOINTERFACE;
}
static ULONG STDMETHODCALLTYPE Notif_AddRef(NotifObject* p) {
    return (ULONG)InterlockedIncrement(&p->refCount);
}
static ULONG STDMETHODCALLTYPE Notif_Release(NotifObject* p) {
    LONG r = InterlockedDecrement(&p->refCount);
    if (r == 0) { delete[] p->vtable; delete p; }
    return (ULONG)std::max(r, 0L);
}
static HRESULT STDMETHODCALLTYPE Notif_HandleUpdate() {
    HWND hWnd = tbh::ResolveTaskbarWnd(g_taskbarWnd);
    if (g_unloading || !hWnd) return S_OK;
    RunFromWindowThread(hWnd, [](void*) {
        if (!g_unloading) RebuildButtonGrid();
    }, nullptr);
    return S_OK;
}
static HRESULT STDMETHODCALLTYPE Notif_NoOp() { return S_OK; }
static HRESULT STDMETHODCALLTYPE Notif_CountChanged(NotifObject*) { return Notif_HandleUpdate(); }
static HRESULT STDMETHODCALLTYPE Notif_CurrentChanged(NotifObject*) { return Notif_HandleUpdate(); }
static HRESULT STDMETHODCALLTYPE Notif_CurrentChangedWithMonitors(NotifObject*, void*, void*, void*) {
    return Notif_HandleUpdate();
}

static NotifObject* CreateNotifObject() {
    auto cfg = GetNotifConfig();
    if (cfg.methodCount == 0 || cfg.currentChangedIdx < 0) return nullptr;
    auto* obj = new (std::nothrow) NotifObject();
    if (!obj) return nullptr;
    obj->vtable = new (std::nothrow) void*[cfg.methodCount];
    if (!obj->vtable) { delete obj; return nullptr; }
    for (int i = 0; i < cfg.methodCount; i++) obj->vtable[i] = (void*)&Notif_NoOp;
    obj->vtable[0] = (void*)&Notif_QI;
    obj->vtable[1] = (void*)&Notif_AddRef;
    obj->vtable[2] = (void*)&Notif_Release;
    if (cfg.createdIdx >= 0)   obj->vtable[cfg.createdIdx]   = (void*)&Notif_CountChanged;
    if (cfg.destroyedIdx >= 0) obj->vtable[cfg.destroyedIdx] = (void*)&Notif_CountChanged;
    obj->vtable[cfg.currentChangedIdx] = cfg.hasMonitors
        ? (void*)&Notif_CurrentChangedWithMonitors
        : (void*)&Notif_CurrentChanged;
    return obj;
}

static NotifObject* g_notifObject = nullptr;

static DWORD WINAPI NotificationThreadProc(void*) {
    auto cfg = GetNotifConfig();
    if (cfg.methodCount == 0) {
        Wh_Log(L"[Notif] Unsupported build (explorer %u)", g_explorerBuild);
        return 0;
    }
    if (FAILED(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED))) return 0;

    IServiceProvider* svc = nullptr;
    if (FAILED(CoCreateInstance(CLSID_ImmersiveShell, nullptr, CLSCTX_LOCAL_SERVER,
                                IID_IServiceProvider, (void**)&svc)) || !svc) {
        CoUninitialize(); return 0;
    }
    IVirtualDesktopNotificationService_I* notifSvc = nullptr;
    svc->QueryService(SID_VirtualDesktopNotificationService,
                      IID_IVirtualDesktopNotificationService_G, (void**)&notifSvc);
    svc->Release();
    if (!notifSvc) { CoUninitialize(); return 0; }

    g_notifObject = CreateNotifObject();
    if (!g_notifObject) { notifSvc->Release(); CoUninitialize(); return 0; }

    HRESULT hr = notifSvc->Register(reinterpret_cast<IUnknown*>(g_notifObject), &g_notificationCookie);
    if (FAILED(hr)) {
        Wh_Log(L"[Notif] Register failed: 0x%08X", hr);
        Notif_Release(g_notifObject); g_notifObject = nullptr;
        notifSvc->Release(); CoUninitialize(); return 0;
    }
    Wh_Log(L"[Notif] Registered, cookie=%lu", g_notificationCookie);

    MSG msg;
    while (!g_unloading) {
        DWORD w = MsgWaitForMultipleObjects(1, &g_notificationStopEvent, FALSE, INFINITE, QS_ALLINPUT);
        if (w == WAIT_OBJECT_0) break;
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) { TranslateMessage(&msg); DispatchMessageW(&msg); }
    }

    if (g_notificationCookie) { notifSvc->Unregister(g_notificationCookie); g_notificationCookie = 0; }
    if (g_notifObject)        { Notif_Release(g_notifObject); g_notifObject = nullptr; }
    notifSvc->Release();
    CoUninitialize();
    return 0;
}

static void StartNotificationThread() {
    if (g_notificationThread) return;
    g_notificationStopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_notificationThread    = CreateThread(nullptr, 0, NotificationThreadProc, nullptr, 0, nullptr);
    if (!g_notificationThread) {
        CloseHandle(g_notificationStopEvent); g_notificationStopEvent = nullptr;
    }
}

static void StopNotificationThread() {
    if (g_notificationStopEvent) SetEvent(g_notificationStopEvent);
    if (g_notificationThread) {
        // Pump sent messages while waiting so that if Wh_ModUninit is called from
        // the UI thread and the notification thread is mid-SendMessage, the sent
        // message can be delivered and the notification thread can then exit.
        // PeekMessage(PM_NOREMOVE) processes incoming sent messages without
        // consuming posted messages from the queue.
        DWORD result;
        do {
            result = MsgWaitForMultipleObjects(1, &g_notificationThread, FALSE, INFINITE, QS_SENDMESSAGE);
            if (result == WAIT_OBJECT_0 + 1) {
                MSG msg;
                PeekMessage(&msg, nullptr, 0, 0, PM_NOREMOVE);
            }
        } while (result == WAIT_OBJECT_0 + 1);
        CloseHandle(g_notificationThread); g_notificationThread = nullptr;
    }
    if (g_notificationStopEvent) {
        CloseHandle(g_notificationStopEvent); g_notificationStopEvent = nullptr;
    }
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
        CloseHandle(g_retryThread); g_retryThread = nullptr;
    }
    if (g_retryStopEvent) {
        CloseHandle(g_retryStopEvent); g_retryStopEvent = nullptr;
    }
}

static void WaitForSwitchThreads() {
    std::vector<HANDLE> threads;
    {
        std::lock_guard lock(g_switchThreadsMutex);
        threads.swap(*g_switchThreads);
    }
    for (HANDLE thread : threads) {
        DWORD result;
        do {
            result = MsgWaitForMultipleObjects(
                1, &thread, FALSE, INFINITE, QS_SENDMESSAGE);
            if (result == WAIT_OBJECT_0 + 1) {
                MSG message;
                PeekMessageW(&message, nullptr, 0, 0, PM_NOREMOVE);
            }
        } while (result == WAIT_OBJECT_0 + 1);
        CloseHandle(thread);
    }
}

// ============================================================
// Desktop state — registry
// ============================================================

static std::vector<BYTE> ReadRegBinary(const wchar_t* path, const wchar_t* name) {
    DWORD type = 0, size = 0;
    if (RegGetValueW(HKEY_CURRENT_USER, path, name, RRF_RT_REG_BINARY, &type, nullptr, &size) != ERROR_SUCCESS || !size)
        return {};
    std::vector<BYTE> buf(size);
    if (RegGetValueW(HKEY_CURRENT_USER, path, name, RRF_RT_REG_BINARY, &type, buf.data(), &size) != ERROR_SUCCESS)
        return {};
    buf.resize(size);
    return buf;
}

static int ReadDesktopCount() {
    DWORD sessionId = 0;
    ProcessIdToSessionId(GetCurrentProcessId(), &sessionId);
    wchar_t sessionPath[256];
    swprintf_s(sessionPath, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\SessionInfo\\%lu\\VirtualDesktops", sessionId);
    for (auto* path : { (const wchar_t*)sessionPath, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\VirtualDesktops" }) {
        auto buf = ReadRegBinary(path, L"VirtualDesktopIDs");
        if (buf.size() >= 16) return (int)(buf.size() / 16);
    }
    return 1;
}

static int ReadCurrentDesktop() {
    DWORD sessionId = 0;
    ProcessIdToSessionId(GetCurrentProcessId(), &sessionId);
    wchar_t sessionPath[256];
    swprintf_s(sessionPath, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\SessionInfo\\%lu\\VirtualDesktops", sessionId);

    std::vector<BYTE> ids;
    for (auto* path : { (const wchar_t*)sessionPath, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\VirtualDesktops" }) {
        ids = ReadRegBinary(path, L"VirtualDesktopIDs");
        if (ids.size() >= 16) break;
    }
    if (ids.empty()) return 0;

    GUID currentGuid{};
    bool gotCurrent = false;
    for (auto* path : { (const wchar_t*)sessionPath, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\VirtualDesktops" }) {
        auto buf = ReadRegBinary(path, L"CurrentVirtualDesktop");
        if (buf.size() >= 16) { memcpy(&currentGuid, buf.data(), 16); gotCurrent = true; break; }
        // Try REG_SZ form
        wchar_t strBuf[64]; DWORD sz = sizeof(strBuf), type;
        if (RegGetValueW(HKEY_CURRENT_USER, path, L"CurrentVirtualDesktop",
                         RRF_RT_REG_SZ, &type, strBuf, &sz) == ERROR_SUCCESS &&
            SUCCEEDED(CLSIDFromString(strBuf, &currentGuid))) { gotCurrent = true; break; }
    }
    if (!gotCurrent) return 0;

    int count = (int)(ids.size() / 16);
    for (int i = 0; i < count; i++) {
        GUID g; memcpy(&g, ids.data() + i * 16, 16);
        if (memcmp(&g, &currentGuid, 16) == 0) return i;
    }
    return 0;
}

// Read Windows display names for all desktops (registry Desktops\{GUID}\Name).
// Falls back to "Desktop N" when a desktop has no custom name.
static std::vector<std::wstring> ReadDesktopNames(int count) {
    DWORD sessionId = 0;
    ProcessIdToSessionId(GetCurrentProcessId(), &sessionId);
    wchar_t sessionPath[256];
    swprintf_s(sessionPath, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\SessionInfo\\%lu\\VirtualDesktops", sessionId);

    std::vector<BYTE> ids;
    for (auto* path : { (const wchar_t*)sessionPath, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\VirtualDesktops" }) {
        ids = ReadRegBinary(path, L"VirtualDesktopIDs");
        if (ids.size() >= 16) break;
    }

    std::vector<std::wstring> names(count);
    for (int i = 0; i < count; i++) {
        names[i] = L"Desktop " + std::to_wstring(i + 1);
        if ((int)ids.size() >= (i + 1) * 16) {
            GUID g; memcpy(&g, ids.data() + i * 16, 16);
            wchar_t guidStr[64];
            StringFromGUID2(g, guidStr, ARRAYSIZE(guidStr));
            wchar_t regPath[300];
            swprintf_s(regPath, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\VirtualDesktops\\Desktops\\%ls", guidStr);
            wchar_t name[256]; DWORD sz = sizeof(name);
            if (RegGetValueW(HKEY_CURRENT_USER, regPath, L"Name", RRF_RT_REG_SZ, nullptr, name, &sz) == ERROR_SUCCESS && name[0])
                names[i] = name;
        }
    }
    return names;
}

// ============================================================
// Virtual desktop switching
// ============================================================

// Desktop hover overview. All state and HWND operations live on the taskbar
// UI thread. Native thumbnails avoid taking focus or switching desktops.
#include <dwmapi.h>
#include <shobjidl.h>

namespace desktop_preview {
constexpr PCWSTR kClass = L"WindhawkDesktopPreview_" WH_MOD_ID;

// The overview used to paint itself in COLOR_INFOBK / COLOR_INFOTEXT — the
// classic tooltip palette, which renders as pale yellow and looks nothing like
// the Windows 11 shell the preview floats over. The system colors are legacy
// Win32 and do NOT follow the light/dark setting, so the only way to match the
// OS is to read the theme and derive the palette from it.
struct Palette {
    COLORREF chrome, chromeText, canvas, thumb, thumbFrame, thumbText, border;
};

// The taskbar follows SystemUsesLightTheme, and this popup belongs to the
// taskbar, so that is the key to honor. AppsUseLightTheme is the documented
// fallback; a missing value means light, per the registry's own default.
static bool SystemUsesLightTheme() {
    static constexpr PCWSTR kPath =
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize";
    for (PCWSTR name : {L"SystemUsesLightTheme", L"AppsUseLightTheme"}) {
        DWORD value = 0, size = sizeof(value);
        if (RegGetValueW(HKEY_CURRENT_USER, kPath, name, RRF_RT_REG_DWORD,
                         nullptr, &value, &size) == ERROR_SUCCESS)
            return value != 0;
    }
    return true;
}

static Palette CurrentPalette() {
    if (SystemUsesLightTheme())
        return {RGB(0xF3, 0xF3, 0xF3), RGB(0x1A, 0x1A, 0x1A),
                RGB(0xE4, 0xE4, 0xE4), RGB(0xFF, 0xFF, 0xFF),
                RGB(0xC2, 0xC2, 0xC2), RGB(0x1A, 0x1A, 0x1A),
                RGB(0xD0, 0xD0, 0xD0)};
    return {RGB(0x20, 0x20, 0x20), RGB(0xF0, 0xF0, 0xF0),
            RGB(0x2B, 0x2B, 0x2B), RGB(0x38, 0x38, 0x38),
            RGB(0x4D, 0x4D, 0x4D), RGB(0xE6, 0xE6, 0xE6),
            RGB(0x3D, 0x3D, 0x3D)};
}

// FillRect/FrameRect want a brush, and the palette is resolved per paint.
struct ScopedBrush {
    HBRUSH handle;
    explicit ScopedBrush(COLORREF color) : handle(CreateSolidBrush(color)) {}
    ~ScopedBrush() { if (handle) DeleteObject(handle); }
    ScopedBrush(ScopedBrush const&) = delete;
    ScopedBrush& operator=(ScopedBrush const&) = delete;
    operator HBRUSH() const { return handle; }
};

// Present in the Windows 11 SDK, but the bundled headers may predate them.
#ifndef DWMWA_USE_IMMERSIVE_DARK_MODE
#define DWMWA_USE_IMMERSIVE_DARK_MODE 20
#endif
#ifndef DWMWA_WINDOW_CORNER_PREFERENCE
#define DWMWA_WINDOW_CORNER_PREFERENCE 33
#endif
#ifndef DWMWA_BORDER_COLOR
#define DWMWA_BORDER_COLOR 34
#endif

// Rounded corners and a themed border, so the popup reads as a shell flyout
// rather than a bare Win32 rectangle. Every call is best-effort: an older
// build simply rejects the attribute and the popup stays square.
static void ApplyWindowTheme(HWND window, Palette const& palette) {
    if (!window) return;
    BOOL dark = !SystemUsesLightTheme();
    DwmSetWindowAttribute(window, DWMWA_USE_IMMERSIVE_DARK_MODE, &dark,
                          sizeof(dark));
    DWORD corner = 2;  // DWMWCP_ROUND
    DwmSetWindowAttribute(window, DWMWA_WINDOW_CORNER_PREFERENCE, &corner,
                          sizeof(corner));
    COLORREF border = palette.border;
    DwmSetWindowAttribute(window, DWMWA_BORDER_COLOR, &border, sizeof(border));
}

struct Window {
    HWND source{};
    RECT screen{}, destination{};
    wchar_t title[256]{};
    HTHUMBNAIL thumbnail{};
};
struct State {
    HWND popup{};
    HINSTANCE module{};
    RECT anchor{}, work{}, desktop{};
    GUID desktopId{};
    wchar_t title[256]{};
    Window windows[48]{};
    int count{}, minimized{};
    int width{}, height{}, inset{}, heading{};
    bool enumerated{};
};
static State state;  // exit-time-safe: handles and fixed buffers, no XAML refs
static unsigned long long generation = 0;

static void Hide() {
    ++generation;
    if (state.popup) {
        KillTimer(state.popup, 1);
        KillTimer(state.popup, 2);
        ShowWindow(state.popup, SW_HIDE);
    }
    for (int i = 0; i < state.count; ++i) {
        if (state.windows[i].thumbnail)
            DwmUnregisterThumbnail(state.windows[i].thumbnail);
        state.windows[i] = {};
    }
    state.count = 0;
    state.minimized = 0;
}

struct EnumContext { IVirtualDesktopManager* manager; GUID desktop; unsigned long long generation; };
static BOOL CALLBACK Collect(HWND window, LPARAM parameter) {
    auto& context = *reinterpret_cast<EnumContext*>(parameter);
    if (context.generation != generation || g_unloading) return FALSE;
    if (window == state.popup || !IsWindowVisible(window) ||
        GetAncestor(window, GA_ROOT) != window) return TRUE;
    wchar_t className[64]{};
    GetClassNameW(window, className, ARRAYSIZE(className));
    if (wcscmp(className, L"Progman") == 0 || wcscmp(className, L"WorkerW") == 0 ||
        wcscmp(className, L"Shell_TrayWnd") == 0 || wcscmp(className, L"Shell_SecondaryTrayWnd") == 0) return TRUE;
    auto exStyle = GetWindowLongPtrW(window, GWL_EXSTYLE);
    if (exStyle & WS_EX_TOOLWINDOW) return TRUE;
    if (GetWindow(window, GW_OWNER) && !(exStyle & WS_EX_APPWINDOW)) return TRUE;
    GUID desktop{};
    if (FAILED(context.manager->GetWindowDesktopId(window, &desktop)) ||
        desktop != context.desktop) return TRUE;
    if (context.generation != generation || g_unloading) return FALSE;
    // Shell-cloaked windows belong to inactive desktops and MUST remain eligible.
    DWORD cloaked{};
    DwmGetWindowAttribute(window, DWMWA_CLOAKED, &cloaked, sizeof(cloaked));
    if (cloaked & DWM_CLOAKED_APP) return TRUE;
    if (IsIconic(window)) { ++state.minimized; return TRUE; }
    if (state.count == ARRAYSIZE(state.windows)) return TRUE;
    Window candidate{};
    if (!GetWindowTextW(window, candidate.title, ARRAYSIZE(candidate.title))) return TRUE;
    if (!GetWindowRect(window, &candidate.screen)) return TRUE;
    RECT intersection{};
    if (!IntersectRect(&intersection, &candidate.screen, &state.desktop)) return TRUE;
    candidate.source = window;
    state.windows[state.count++] = candidate;
    return TRUE;
}

static void Show() {
    auto request = generation;
    POINT cursor{};
    if (g_unloading || !GetCursorPos(&cursor) || !PtInRect(&state.anchor, cursor)) return;
    winrt::com_ptr<IVirtualDesktopManager> manager;
    HRESULT hr = CoCreateInstance(CLSID_VirtualDesktopManager, nullptr,
        CLSCTX_INPROC_SERVER, IID_PPV_ARGS(manager.put()));
    if (request != generation || g_unloading) return;
    state.enumerated = SUCCEEDED(hr);
    if (manager) {
        EnumContext context{manager.get(), state.desktopId, request};
        EnumWindows(Collect, reinterpret_cast<LPARAM>(&context));
    }
    if (request != generation || g_unloading || !IsWindow(state.popup)) return;
    // Composite the entire virtual screen, preserving each window's relative
    // location. EnumWindows yields topmost first: register bottom-to-top.
    double scale = std::min(
        double(state.width - 2 * state.inset) / (state.desktop.right - state.desktop.left),
        double(state.height - 2 * state.heading) / (state.desktop.bottom - state.desktop.top));
    for (int i = state.count - 1; i >= 0; --i) {
        auto& item = state.windows[i];
        RECT visible{};
        if (!IntersectRect(&visible, &item.screen, &state.desktop)) continue;
        item.destination = {
            state.inset + LONG((visible.left - state.desktop.left) * scale),
            state.heading + LONG((visible.top - state.desktop.top) * scale),
            state.inset + LONG((visible.right - state.desktop.left) * scale),
            state.heading + LONG((visible.bottom - state.desktop.top) * scale)};
        HTHUMBNAIL thumbnail{};
        if (SUCCEEDED(DwmRegisterThumbnail(state.popup, item.source, &thumbnail))) {
            DWM_THUMBNAIL_PROPERTIES properties{};
            properties.dwFlags = DWM_TNP_RECTDESTINATION | DWM_TNP_VISIBLE |
                DWM_TNP_OPACITY | DWM_TNP_SOURCECLIENTAREAONLY | DWM_TNP_RECTSOURCE;
            SIZE sourceSize{};
            if (FAILED(DwmQueryThumbnailSourceSize(thumbnail, &sourceSize))) {
                DwmUnregisterThumbnail(thumbnail);
                continue;
            }
            properties.rcSource = {
                MulDiv(visible.left - item.screen.left, sourceSize.cx, item.screen.right - item.screen.left),
                MulDiv(visible.top - item.screen.top, sourceSize.cy, item.screen.bottom - item.screen.top),
                MulDiv(visible.right - item.screen.left, sourceSize.cx, item.screen.right - item.screen.left),
                MulDiv(visible.bottom - item.screen.top, sourceSize.cy, item.screen.bottom - item.screen.top)};
            properties.rcDestination = item.destination;
            properties.fVisible = TRUE;
            properties.opacity = 255;
            properties.fSourceClientAreaOnly = FALSE;
            if (SUCCEEDED(DwmUpdateThumbnailProperties(thumbnail, &properties)))
                item.thumbnail = thumbnail;
            else DwmUnregisterThumbnail(thumbnail);
        }
    }
    int x = (state.anchor.left + state.anchor.right - state.width) / 2;
    int y = state.anchor.top - state.height - state.inset;
    if (y < state.work.top) y = state.anchor.bottom + state.inset;
    x = std::clamp(x, int(state.work.left), int(state.work.right - state.width));
    y = std::clamp(y, int(state.work.top), int(state.work.bottom - state.height));
    SetWindowPos(state.popup, HWND_TOPMOST, x, y, state.width, state.height,
                 SWP_NOACTIVATE | SWP_SHOWWINDOW);
    InvalidateRect(state.popup, nullptr, TRUE);
    SetTimer(state.popup, 2, 100, nullptr);
    Wh_Log(L"[Preview] %s: %d windows, %d minimized, enumeration hr=0x%08X",
           state.title, state.count, state.minimized, unsigned(hr));
}

static LRESULT CALLBACK WindowProc(HWND window, UINT message, WPARAM wp, LPARAM lp) {
    if (message == WM_NCDESTROY && window == state.popup) {
        state.popup = nullptr;
        Hide();
    }
    if (message == WM_MOUSEACTIVATE) return MA_NOACTIVATE;
    if (message == WM_NCHITTEST) return HTTRANSPARENT;
    if (message == WM_TIMER) {
        if (wp == 1) {
            KillTimer(window, 1);
            try { Show(); } catch (...) { Hide(); Wh_Log(L"[Preview] Could not build overview"); }
        } else {
            POINT cursor{};
            if (g_unloading || !GetCursorPos(&cursor) || !PtInRect(&state.anchor, cursor)) Hide();
        }
        return 0;
    }
    if (message == WM_PAINT) {
        PAINTSTRUCT paint{};
        HDC dc = BeginPaint(window, &paint);
        RECT client{};
        GetClientRect(window, &client);
        Palette palette = CurrentPalette();
        ScopedBrush chrome(palette.chrome), canvasBrush(palette.canvas),
            thumb(palette.thumb), thumbFrame(palette.thumbFrame);
        FillRect(dc, &client, chrome);
        SetTextColor(dc, palette.chromeText);
        SetBkMode(dc, TRANSPARENT);
        auto oldFont = SelectObject(dc, GetStockObject(DEFAULT_GUI_FONT));
        RECT title{state.inset, 0, state.width - state.inset, state.heading};
        DrawTextW(dc, state.title, -1, &title, DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS | DT_NOPREFIX);
        RECT canvas{state.inset, state.heading, state.width - state.inset, state.height - state.heading};
        FillRect(dc, &canvas, canvasBrush);
        int saved = SaveDC(dc);
        IntersectClipRect(dc, canvas.left, canvas.top, canvas.right, canvas.bottom);
        for (int i = state.count - 1; i >= 0; --i) {
            auto& item = state.windows[i];
            FillRect(dc, &item.destination, thumb);
            FrameRect(dc, &item.destination, thumbFrame);
            SetTextColor(dc, palette.thumbText);
            RECT label = item.destination;
            DrawTextW(dc, item.title, -1, &label, DT_SINGLELINE | DT_TOP | DT_END_ELLIPSIS | DT_NOPREFIX);
        }
        RestoreDC(dc, saved);
        SetTextColor(dc, palette.chromeText);
        RECT footer{state.inset, state.height - state.heading, state.width - state.inset, state.height};
        wchar_t text[128]{};
        if (!state.enumerated) wcscpy_s(text, L"Desktop preview unavailable");
        else if (!state.count && !state.minimized) wcscpy_s(text, L"No open windows on this desktop");
        else swprintf_s(text, L"%d windows · %d minimized", state.count, state.minimized);
        DrawTextW(dc, text, -1, &footer, DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS | DT_NOPREFIX);
        SelectObject(dc, oldFont);
        EndPaint(window, &paint);
        return 0;
    }
    return DefWindowProcW(window, message, wp, lp);
}

static void Schedule(FrameworkElement const& button, int desktopIndex, int widthDip, int delay) {
    Hide();
    if (g_unloading) return;
    DWORD session{};
    ProcessIdToSessionId(GetCurrentProcessId(), &session);
    wchar_t path[256];
    swprintf_s(path, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\SessionInfo\\%lu\\VirtualDesktops", session);
    auto ids = ReadRegBinary(path, L"VirtualDesktopIDs");
    if (ids.empty()) ids = ReadRegBinary(L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\VirtualDesktops", L"VirtualDesktopIDs");
    if (desktopIndex < 0 || (size_t(desktopIndex) + 1) * sizeof(GUID) > ids.size()) return;
    memcpy(&state.desktopId, ids.data() + desktopIndex * sizeof(GUID), sizeof(GUID));
    auto names = ReadDesktopNames(int(ids.size() / sizeof(GUID)));
    wcsncpy_s(state.title, names[desktopIndex].c_str(), _TRUNCATE);
    auto root = button.XamlRoot();
    HWND taskbar = FindCurrentProcessTaskbarWnd();
    // Match the XamlRoot for secondary taskbars as well.
    for (HWND candidate = FindWindowExW(nullptr, nullptr, L"Shell_SecondaryTrayWnd", nullptr);
         candidate; candidate = FindWindowExW(nullptr, candidate, L"Shell_SecondaryTrayWnd", nullptr)) {
        DWORD pid{};
        GetWindowThreadProcessId(candidate, &pid);
        if (pid == GetCurrentProcessId() && GetTaskbarXamlRoot(candidate) == root) { taskbar = candidate; break; }
    }
    if (!taskbar) return;
    POINT origin{};
    ClientToScreen(taskbar, &origin);
    auto point = button.TransformToVisual(root.Content().try_as<UIElement>()).TransformPoint({0, 0});
    double scale = root.RasterizationScale();
    state.anchor = {origin.x + LONG(point.X * scale), origin.y + LONG(point.Y * scale),
        origin.x + LONG((point.X + button.ActualWidth()) * scale),
        origin.y + LONG((point.Y + button.ActualHeight()) * scale)};
    MONITORINFO monitor{sizeof(monitor)};
    if (!GetMonitorInfoW(MonitorFromRect(&state.anchor, MONITOR_DEFAULTTONEAREST), &monitor)) return;
    state.work = monitor.rcWork;
    state.desktop = {GetSystemMetrics(SM_XVIRTUALSCREEN), GetSystemMetrics(SM_YVIRTUALSCREEN), 0, 0};
    state.desktop.right = state.desktop.left + std::max(1, GetSystemMetrics(SM_CXVIRTUALSCREEN));
    state.desktop.bottom = state.desktop.top + std::max(1, GetSystemMetrics(SM_CYVIRTUALSCREEN));
    state.inset = std::max(4, int(8 * scale));
    state.heading = std::max(20, int(28 * scale));
    state.width = std::min(int(widthDip * scale), int(state.work.right - state.work.left));
    state.height = std::min(int((state.width - 2 * state.inset) *
        double(state.desktop.bottom - state.desktop.top) / (state.desktop.right - state.desktop.left)) +
        2 * state.heading, int(state.work.bottom - state.work.top));
    if (state.width <= 2 * state.inset || state.height <= 2 * state.heading) return;
    if (!state.popup) {
        GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            reinterpret_cast<PCWSTR>(&WindowProc), &state.module);
        WNDCLASSW wc{};
        wc.hInstance = state.module;
        wc.lpfnWndProc = WindowProc;
        wc.lpszClassName = kClass;
        // This class belongs to this module. An existing copy can only be a
        // failed prior teardown, whose WndProc may be dangling; never reuse it.
        if (!RegisterClassW(&wc)) return;
        state.popup = CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_TOPMOST,
            kClass, L"Desktop preview", WS_POPUP, 0, 0, state.width, state.height,
            taskbar, nullptr, state.module, nullptr);
    }
    // Re-applied per hover, not once at creation: the user can switch the
    // Windows theme while Explorer keeps running, and the popup outlives that.
    ApplyWindowTheme(state.popup, CurrentPalette());
    if (state.popup) SetTimer(state.popup, 1, std::max(1, delay), nullptr);
}

static void Destroy() {
    Hide();
    if (state.popup) DestroyWindow(state.popup);
    if (state.module) UnregisterClassW(kClass, state.module);
    state = {};
}
}  // namespace desktop_preview

struct IVirtualDesktopManagerInternal_S : IUnknown {};
struct IVirtualDesktop_S : IUnknown {};

MIDL_INTERFACE("92CA9DCD-5622-4bba-A805-5E9F541BD8C9")
IObjectArray_Local : public IUnknown {
    virtual HRESULT STDMETHODCALLTYPE GetCount(UINT* pcObjects) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetAt(UINT i, REFIID riid, void** ppv) = 0;
};

const CLSID CLSID_VirtualDesktopManagerInternal = {
    0xC5E0CDCA,0x7B6E,0x41B2,{0x9F,0xC4,0xD9,0x39,0x75,0xCC,0x46,0x7B}
};

void SwitchToDesktop(int targetIndex) {
    if (!LoadTwinuiBuild()) { Wh_Log(L"[VD] twinui.pcshell.dll not loaded"); return; }

    IID IID_VDMI, IID_VD;
    bool usesHMonitor;
    if      (g_twinuiBuild >= 26100) {
        IID_VDMI = {0x53F5CA0B,0x158F,0x4124,{0x90,0x0C,0x05,0x71,0x58,0x06,0x0B,0x27}};
        IID_VD   = {0x3F07F4BE,0xB107,0x441A,{0xAF,0x0F,0x39,0xD8,0x25,0x29,0x07,0x2C}};
        usesHMonitor = false;
    } else if (g_twinuiBuild >= 22621) {
        IID_VDMI = {0xA3175F2D,0x239C,0x4BD2,{0x8A,0xA0,0xEE,0xBA,0x8B,0x0B,0x13,0x8E}};
        IID_VD   = {0x3F07F4BE,0xB107,0x441A,{0xAF,0x0F,0x39,0xD8,0x25,0x29,0x07,0x2C}};
        usesHMonitor = false;
    } else if (g_twinuiBuild >= 22000) {
        IID_VDMI = {0xB2F925B9,0x5A0F,0x4D2E,{0x9F,0x4D,0x2B,0x15,0x07,0x59,0x3C,0x10}};
        IID_VD   = {0x536D3495,0xB208,0x4CC9,{0xAE,0x26,0xDE,0x81,0x11,0x27,0x5B,0xF8}};
        usesHMonitor = true;
    } else if (g_twinuiBuild >= 20348) {
        IID_VDMI = {0x094AFE11,0x44F2,0x4BA0,{0x97,0x6F,0x29,0xA9,0x7E,0x26,0x3E,0xE0}};
        IID_VD   = {0x62FDF88B,0x11CA,0x4AFB,{0x8B,0xD8,0x22,0x96,0xDF,0xAE,0x49,0xE2}};
        usesHMonitor = true;
    } else {
        IID_VDMI = {0xF31574D6,0xB682,0x4CDC,{0xBD,0x56,0x18,0x27,0x86,0x0A,0xBE,0xC6}};
        IID_VD   = {0xFF72FFDD,0xBE7E,0x43FC,{0x9C,0x03,0xAD,0x81,0x68,0x1E,0x88,0xE4}};
        usesHMonitor = false;
    }

    IServiceProvider* svc = nullptr;
    if (FAILED(CoCreateInstance(CLSID_ImmersiveShell, nullptr, CLSCTX_LOCAL_SERVER,
                                IID_IServiceProvider, (void**)&svc)) || !svc)
        { Wh_Log(L"[VD] CoCreateInstance failed"); return; }

    IVirtualDesktopManagerInternal_S* mgr = nullptr;
    svc->QueryService(CLSID_VirtualDesktopManagerInternal, IID_VDMI, (void**)&mgr);
    svc->Release();
    if (!mgr) { Wh_Log(L"[VD] QueryService VDMI failed"); return; }

    IObjectArray_Local* arr = nullptr;
    if (usesHMonitor) {
        typedef HRESULT(STDMETHODCALLTYPE* FnM)(IVirtualDesktopManagerInternal_S*, HMONITOR, IObjectArray_Local**);
        ((FnM)(*(void***)mgr)[7])(mgr, nullptr, &arr);
    } else {
        typedef HRESULT(STDMETHODCALLTYPE* Fn)(IVirtualDesktopManagerInternal_S*, IObjectArray_Local**);
        ((Fn)(*(void***)mgr)[7])(mgr, &arr);
    }
    if (!arr) { mgr->Release(); Wh_Log(L"[VD] GetDesktops failed"); return; }

    UINT count = 0;
    arr->GetCount(&count);
    if (targetIndex < 0 || (UINT)targetIndex >= count) { arr->Release(); mgr->Release(); return; }

    IVirtualDesktop_S* target = nullptr;
    arr->GetAt((UINT)targetIndex, IID_VD, (void**)&target);
    arr->Release();
    if (!target) { mgr->Release(); Wh_Log(L"[VD] GetAt failed"); return; }

    if (usesHMonitor) {
        typedef HRESULT(STDMETHODCALLTYPE* FnM)(IVirtualDesktopManagerInternal_S*, HMONITOR, IVirtualDesktop_S*);
        ((FnM)(*(void***)mgr)[9])(mgr, nullptr, target);
    } else {
        typedef HRESULT(STDMETHODCALLTYPE* Fn)(IVirtualDesktopManagerInternal_S*, IVirtualDesktop_S*);
        ((Fn)(*(void***)mgr)[9])(mgr, target);
    }
    target->Release();
    mgr->Release();
    Wh_Log(L"[VD] Switched to desktop %d", targetIndex);
}


// ============================================================
// Button grid building
// ============================================================

static Brush GetWindowsAccentBrush(
    winrt::Windows::UI::ViewManagement::UIColorType colorType) {
    try {
        winrt::Windows::UI::ViewManagement::UISettings uiSettings;
        auto color = uiSettings.GetColorValue(colorType);
        SolidColorBrush brush;
        brush.Color(color);
        return brush;
    } catch (...) {
        Wh_Log(L"[Color] Failed to read the Windows accent color");
        return nullptr;
    }
}

// The one color-token parser now lives in _templates/color-tokens.h, embedded
// above. Three independent copies existed across this family and agreed only
// by luck; this delegation retires another. nullptr still means "no color
// here" for an empty setting, an unknown token, or bad hex alike — never a
// fallback brush, which would make an empty setting paint.
static void LogAccentReadFailure() {
    Wh_Log(L"[Color] Failed to read the Windows accent color");
}

static Brush ParseColorBrush(const std::wstring& value) {
    return clr::ParseBrush(value.c_str(), LogAccentReadFailure);
}

static std::wstring ToRoman(int n) {
    if (n <= 0 || n > 3999) return std::to_wstring(n);
    static const struct { int v; const wchar_t* s; } t[] = {
        {1000,L"M"},{900,L"CM"},{500,L"D"},{400,L"CD"},
        {100,L"C"},{90,L"XC"},{50,L"L"},{40,L"XL"},
        {10,L"X"},{9,L"IX"},{5,L"V"},{4,L"IV"},{1,L"I"}
    };
    std::wstring r;
    for (auto& [v, s] : t) { while (n >= v) { r += s; n -= v; } }
    return r;
}

static std::wstring GetButtonLabel(int idx, int current) {
    if (g_settings.labelFormat == VdLabelFormat::Symbol)
        return (idx == current) ? g_settings.activeSymbol
                                : g_settings.inactiveSymbol;
    if (g_settings.labelFormat == VdLabelFormat::Roman)
        return ToRoman(idx + 1);
    if (g_settings.labelFormat == VdLabelFormat::Custom && !g_settings.customLabels.empty()) {
        std::wistringstream ss(g_settings.customLabels);
        std::wstring token; int i = 0;
        while (std::getline(ss, token, L',')) { if (i++ == idx) return token; }
    }
    return std::to_wstring(idx + 1);
}

// Apply shine gradient to a base brush. Returns brush unchanged if no base color or shine off.
static Brush MakeShineBrush(Brush base) {
    if (!g_settings.shineEffect) return base;
    auto solid = base ? base.try_as<SolidColorBrush>() : nullptr;
    if (!solid) return base;
    auto c = solid.Color();

    LinearGradientBrush b;
    b.StartPoint({0.5, 0.0});
    b.EndPoint({0.5, 1.0});

    // Top: semi-transparent white highlight
    GradientStop g0; winrt::Windows::UI::Color shine{180,255,255,255};
    g0.Color(shine); g0.Offset(0.0); b.GradientStops().Append(g0);

    // Upper-mid: base color lightened slightly
    GradientStop g1;
    winrt::Windows::UI::Color light{c.A,
        (BYTE)std::min(255, (int)c.R + 35),
        (BYTE)std::min(255, (int)c.G + 35),
        (BYTE)std::min(255, (int)c.B + 35)};
    g1.Color(light); g1.Offset(0.42); b.GradientStops().Append(g1);

    // Lower: base color
    GradientStop g2; g2.Color(c); g2.Offset(0.52); b.GradientStops().Append(g2);

    // Bottom: slightly darker
    GradientStop g3;
    winrt::Windows::UI::Color dark{c.A,
        (BYTE)(c.R * 7 / 10), (BYTE)(c.G * 7 / 10), (BYTE)(c.B * 7 / 10)};
    g3.Color(dark); g3.Offset(1.0); b.GradientStops().Append(g3);

    return b;
}

// ---- Layout glue: settings -> the one arranger ------------------------------

// Rows that fit in this taskbar. The rect is physical pixels and every XAML
// size is a DIP, so the conversion happens before the division -- mixing them
// is the bug flagged on PR #4855 and #4843.
// "Last button in the grid": the Task View button is just another cell, sized
// and shaped like a desktop button, rather than a column or sliver alongside.
static bool TaskViewInGrid() {
    return g_settings.taskViewPlacement == VdTaskViewPlacement::InGrid;
}

static bool TaskViewIsRow() {
    return g_settings.taskViewButton && !TaskViewInGrid() &&
           (g_settings.taskViewPlacement == VdTaskViewPlacement::Above ||
            g_settings.taskViewPlacement == VdTaskViewPlacement::Below);
}

static int AvailableRows(bool quiet = false) {
    HWND hWnd = tbh::ResolveTaskbarWnd(g_taskbarWnd);
    auto metrics = tbh::GetMetrics(hWnd);
    if (!metrics.valid) {
        if (!quiet)
            Wh_Log(L"[Layout] No taskbar window - assuming a single row");
        return 1;
    }
    // constrainedDip is the taskbar's own thickness in DIPs whichever way it
    // runs, so the physical-px/DIP conversion lives in the template rather
    // than being re-derived in each mod.
    double heightDip = metrics.constrainedDip;

    // Reserve what the desktop grid does NOT get: the outer vertical padding,
    // and a Task View button placed above or below, which is a row of its own.
    // Without this the grid claims the whole taskbar and the assembled group
    // overflows it. The gap is deliberately not reserved - it is cosmetic, and
    // letting a sliver hang past the edge is the point of it.
    double reserved = 2.0 * (double)g_settings.padY;
    if (TaskViewIsRow())
        reserved += (double)g_settings.taskViewSize +
                    (double)g_settings.itemSpacing;

    int rows = ngl::RowsInHeight(heightDip - reserved,
                                 (double)g_settings.itemHeight,
                                 (double)g_settings.itemSpacing);
    if (!quiet) {
        Wh_Log(L"[Layout] %s taskbar, %.0f dip across at %udpi, %.0f reserved "
               L"-> %d row(s) for the desktop buttons",
               tbh::OrientationName(metrics.orientation), heightDip,
               metrics.dpi, reserved, rows);
    }
    return rows;
}

static ngl::Config MakeLayoutConfig() {
    ngl::Config config;
    config.spacing = (double)g_settings.itemSpacing;
    config.justify = g_settings.justify;
    config.padX = (double)g_settings.padX;
    config.padY = (double)g_settings.padY;
    return config;
}

static ngl::FillOrder LayoutFillOrder() {
    return g_settings.fillOrder;
}

// This mod's token vocabulary. Desktops are dynamic, so they are numbered;
// "desktop2" is accepted as a readable alias for "2", and the Task View button
// is "master". Matching is case-insensitive. Tokens are identity, never the
// button's label -- renaming a desktop never invalidates an arrangement.
// Returns a 0-based desktop index, or -1 when the token is not a desktop.
static int DesktopIndexFromToken(std::wstring const& token, int count) {
    int number = ngl::TokenIndexWithPrefix(token, L"desktop");
    if (!number) {
        wchar_t* end = nullptr;
        long parsed = std::wcstol(token.c_str(), &end, 10);
        if (!end || *end || end == token.c_str())
            return -1;
        number = (int)parsed;
    }
    if (number < 1 || number > count)
        return -1;
    return number - 1;
}

// Size of a layout token. "master" collapses to empty when the Task View
// button is off; anything unrecognized collapses out too, so a stray name in a
// hand-written arrangement costs nothing.
// The Task View button answers to "master" and to the friendlier "taskview".
static bool IsMasterToken(std::wstring const& token) {
    return ngl::TokenIs(token, L"master") || ngl::TokenIs(token, L"taskview");
}

static ngl::Size ResolveLayoutToken(std::wstring const& token, int count) {
    if (IsMasterToken(token)) {
        if (!g_settings.taskViewButton)
            return {};
        if (TaskViewInGrid())
            return {(double)g_settings.itemWidth, (double)g_settings.itemHeight};
        // Sized against whichever axis it lands on, so one pair of settings
        // covers a full-height column and a full-width sliver, wherever the
        // arrangement puts it. Span 0 means match the desktop buttons.
        return ngl::AlongAxis((double)g_settings.taskViewSize,
                              (double)g_settings.taskViewSpan);
    }
    if (DesktopIndexFromToken(token, count) >= 0)
        return {(double)g_settings.itemWidth, (double)g_settings.itemHeight};
    return {};
}

// Wrap the generated desktop grid with the Task View button in its configured
// place. Only the automatic arrangement consults this -- a hand-written
// arrangement positions "master" itself.
// The Task View token, carrying Size.TaskViewGap as a cosmetic offset away
// from the desktop buttons. Positive is always "further away", whichever side
// it sits on. Cosmetic on purpose: the group keeps its size, so a sliver can
// hang past the taskbar edge and show only its leading edge instead of the
// whole group growing and re-centering.
static std::wstring MasterToken() {
    int gap = g_settings.taskViewGap;
    if (!gap)
        return L"master";
    auto where = g_settings.taskViewPlacement;
    int dx = 0, dy = 0;
    if (where == VdTaskViewPlacement::Before)     dx = -gap;
    else if (where == VdTaskViewPlacement::Above) dy = -gap;
    else if (where == VdTaskViewPlacement::Below) dy = gap;
    else                        dx = gap;  // "after"
    return L"master[" + std::to_wstring(dx) + L"," + std::to_wstring(dy) + L"]";
}

static std::wstring AddTaskViewButton(std::wstring const& grid) {
    if (!g_settings.taskViewButton || TaskViewInGrid())
        return grid;
    auto where = g_settings.taskViewPlacement;
    std::wstring master = MasterToken();
    if (where == VdTaskViewPlacement::Before)
        return master + L" | (" + grid + L")";
    if (where == VdTaskViewPlacement::Above)
        return master + L", (" + grid + L")";
    if (where == VdTaskViewPlacement::Below)
        return L"(" + grid + L"), " + master;
    return L"(" + grid + L") | " + master;  // "after"
}

// Every token this mod expects to be on screen right now.
static std::vector<std::wstring> ExpectedTokens(int count) {
    std::vector<std::wstring> tokens;
    for (int i = 1; i <= count; i++)
        tokens.push_back(std::to_wstring(i));
    if (g_settings.taskViewButton)
        tokens.push_back(L"master");
    return tokens;
}

// Resolve Layout.Arrangement to placements. "auto" generates the expression
// and logs it so it can be pasted back into the same field and edited; an
// explicit arrangement is used as written, falling back to automatic if it
// does not parse. A desktop created after the arrangement was written is
// appended (or left out) per Layout.NewItems, so a new desktop is never
// silently unreachable. quiet suppresses the log for repeated measure-only
// calls from the Start-placement layout callback.
static bool ComputeButtonPlacements(int count,
                                    std::vector<ngl::Placement>& placements,
                                    ngl::Size& total, bool quiet = false) {
    ngl::Config config = MakeLayoutConfig();
    auto resolve = [count](std::wstring const& token) {
        return ResolveLayoutToken(token, count);
    };

    bool isAuto = ngl::IsAutoSetting(g_settings.arrangement);
    int maxRows = AvailableRows(quiet);
    auto makeAuto = [count, maxRows]() {
        // In-grid mode shapes count + 1 cells so the Task View button flows
        // with the desktops instead of hanging off the side.
        bool inGrid = g_settings.taskViewButton && TaskViewInGrid();
        auto namer = [count](int index) -> std::wstring {
            return index < count ? std::to_wstring(index + 1)
                                 : std::wstring(L"master");
        };
        std::wstring grid = ngl::BuildAutoExpression(
            inGrid ? count + 1 : count, maxRows, LayoutFillOrder(), namer);
        if (grid.empty())
            grid = L"1";
        return AddTaskViewButton(grid);
    };

    std::wstring expression = isAuto ? makeAuto() : g_settings.arrangement;
    ngl::ParseError error;
    bool ok = ngl::Compute(expression, config, resolve, placements, total,
                           &error);
    if (!ok) {
        Wh_Log(L"[Layout] Arrangement \"%ls\" - expected %ls at character %d; "
               L"using the automatic arrangement instead",
               expression.c_str(), error.expected.c_str(),
               (int)error.position + 1);
        isAuto = true;
        expression = makeAuto();
        ok = ngl::Compute(expression, config, resolve, placements, total,
                          nullptr);
    }

    // A hand-written arrangement names the desktops that existed when it was
    // written. Anything created since is in no group and would vanish.
    // Compare by item identity, not by the name typed: "desktop1" and "1" are
    // the same button, and a plain name comparison would append a duplicate of
    // every aliased desktop.
    if (ok && !isAuto && g_settings.appendNewItems) {
        auto sameItem = [count](std::wstring const& placed,
                                std::wstring const& wanted) {
            if (IsMasterToken(wanted))
                return IsMasterToken(placed);
            int a = DesktopIndexFromToken(placed, count);
            return a >= 0 && a == DesktopIndexFromToken(wanted, count);
        };
        auto missing = ngl::MissingTokens(ExpectedTokens(count), placements,
                                          sameItem);

        // Desktops and the Task View button are appended differently. Unlisted
        // desktops join a grid block after what the user wrote. The Task View
        // button instead honours its placement setting, so "before" and "above"
        // actually put it there — and so it never lands inside the desktop
        // block, where it would be squashed into a single cell instead of
        // staying a column or a sliver.
        std::vector<std::wstring> missingDesktops;
        bool missingMaster = false;
        for (auto const& token : missing) {
            if (IsMasterToken(token))
                missingMaster = true;
            else
                missingDesktops.push_back(token);
        }

        if (!missingDesktops.empty() || missingMaster) {
            if (TaskViewInGrid() && missingMaster)
                missingDesktops.push_back(L"master");
            if (!missingDesktops.empty()) {
                expression = ngl::AppendMissing(expression, missingDesktops,
                                                maxRows, LayoutFillOrder());
            }
            if (missingMaster && !TaskViewInGrid())
                expression = AddTaskViewButton(expression);
            ok = ngl::Compute(expression, config, resolve, placements, total,
                              nullptr);
            if (!quiet) {
                Wh_Log(L"[Layout] %d item(s) missing from your arrangement were "
                       L"added; name them in Arrangement to place them "
                       L"yourself", (int)missing.size());
            }
        }
    }

    if (!quiet) {
        Wh_Log(L"[Layout] %d desktop(s), arrangement = \"%ls\"%ls, size %.0fx%.0f",
               count, expression.c_str(),
               isAuto ? L" (auto - paste this into Arrangement to edit it)" : L"",
               total.width, total.height);
    }
    return ok;
}

// Measure only -- called from the Start-placement layout callback, so it must
// not log on every layout pass.
static ngl::Size EstimateButtonGridSize(int count) {
    std::vector<ngl::Placement> placements;
    ngl::Size total;
    ComputeButtonPlacements(count, placements, total, /*quiet=*/true);
    return total;
}

static void SetControlBrushResource(Control const& control,
                                    wchar_t const* key,
                                    Brush const& brush) {
    auto resources = control.Resources();
    auto boxedKey = winrt::box_value(key);
    if (brush)
        resources.Insert(boxedKey, brush);
    else
        resources.Remove(boxedKey);
}

static Brush AdjustSolidBrush(Brush const& base, double whiteBlend) {
    auto solid = base ? base.try_as<SolidColorBrush>() : nullptr;
    if (!solid)
        return nullptr;

    auto color = solid.Color();
    auto blend = [whiteBlend](BYTE channel) {
        double target = whiteBlend >= 0.0 ? 255.0 : 0.0;
        double amount = std::abs(whiteBlend);
        return (BYTE)std::clamp(
            (int)std::lround(channel + (target - channel) * amount), 0, 255);
    };

    winrt::Windows::UI::Color adjustedColor{
        color.A, blend(color.R), blend(color.G), blend(color.B)};
    SolidColorBrush adjusted;
    adjusted.Color(adjustedColor);
    return adjusted;
}

struct ButtonSurfaceBrushes {
    Brush normal;
    Brush hover;
    Brush pressed;
};

static ButtonSurfaceBrushes ResolveButtonSurface(
    bool isActive,
    Brush const& activeBrush,
    Brush const& inactiveBrush,
    Brush const& hoverOverride,
    Brush const& pressedOverride) {
    Brush normal = isActive ? activeBrush : inactiveBrush;

    // A native inactive surface keeps the native XAML hover/pressed states.
    // Colored surfaces derive their states from their own color unless the user
    // supplied an explicit shared override.
    Brush hover = hoverOverride;
    if (!hover && normal)
        hover = AdjustSolidBrush(normal, 0.18);

    Brush pressed = pressedOverride;
    if (!pressed && normal)
        pressed = AdjustSolidBrush(normal, -0.12);

    return {
        MakeShineBrush(normal),
        MakeShineBrush(hover),
        MakeShineBrush(pressed),
    };
}

static void ApplyMasterButtonState(Button const& btn,
    Brush inactiveBrush, Brush inactiveTextBrush,
    Brush hoverBrush, Brush pressedBrush, Brush borderBrush) {
    auto surface = ResolveButtonSurface(false, nullptr, inactiveBrush,
                                        hoverBrush, pressedBrush);
    if (surface.normal)
        btn.Background(surface.normal);
    else
        btn.ClearValue(Control::BackgroundProperty());
    SetControlBrushResource(btn, L"ButtonBackground", surface.normal);
    SetControlBrushResource(btn, L"ButtonBackgroundPointerOver", surface.hover);
    SetControlBrushResource(btn, L"ButtonBackgroundPressed", surface.pressed);

    if (inactiveTextBrush)
        btn.Foreground(inactiveTextBrush);
    else
        btn.ClearValue(Control::ForegroundProperty());
    SetControlBrushResource(btn, L"ButtonForeground", inactiveTextBrush);
    SetControlBrushResource(btn, L"ButtonForegroundPointerOver", inactiveTextBrush);
    SetControlBrushResource(btn, L"ButtonForegroundPressed", inactiveTextBrush);

    if (borderBrush)
        btn.BorderBrush(borderBrush);
    else
        btn.ClearValue(Control::BorderBrushProperty());
    SetControlBrushResource(btn, L"ButtonBorderBrush", borderBrush);
    SetControlBrushResource(btn, L"ButtonBorderBrushPointerOver", borderBrush);
    SetControlBrushResource(btn, L"ButtonBorderBrushPressed", borderBrush);
}

static void ApplyDesktopButtonState(ToggleButton const& btn, bool isActive,
    Brush activeBrush, Brush inactiveBrush,
    Brush activeTextBrush, Brush inactiveTextBrush,
    Brush hoverBrush, Brush pressedBrush, Brush borderBrush) {
    auto inactiveSurface = ResolveButtonSurface(false, activeBrush, inactiveBrush,
                                                hoverBrush, pressedBrush);
    auto activeSurface = ResolveButtonSurface(true, activeBrush, inactiveBrush,
                                              hoverBrush, pressedBrush);

    if (inactiveSurface.normal)
        btn.Background(inactiveSurface.normal);
    else
        btn.ClearValue(Control::BackgroundProperty());
    SetControlBrushResource(btn, L"ToggleButtonBackground", inactiveSurface.normal);
    SetControlBrushResource(btn, L"ToggleButtonBackgroundPointerOver", inactiveSurface.hover);
    SetControlBrushResource(btn, L"ToggleButtonBackgroundPressed", inactiveSurface.pressed);
    SetControlBrushResource(btn, L"ToggleButtonBackgroundChecked", activeSurface.normal);
    SetControlBrushResource(btn, L"ToggleButtonBackgroundCheckedPointerOver", activeSurface.hover);
    SetControlBrushResource(btn, L"ToggleButtonBackgroundCheckedPressed", activeSurface.pressed);

    if (inactiveTextBrush)
        btn.Foreground(inactiveTextBrush);
    else
        btn.ClearValue(Control::ForegroundProperty());
    SetControlBrushResource(btn, L"ToggleButtonForeground", inactiveTextBrush);
    SetControlBrushResource(btn, L"ToggleButtonForegroundPointerOver", inactiveTextBrush);
    SetControlBrushResource(btn, L"ToggleButtonForegroundPressed", inactiveTextBrush);
    SetControlBrushResource(btn, L"ToggleButtonForegroundChecked", activeTextBrush);
    SetControlBrushResource(btn, L"ToggleButtonForegroundCheckedPointerOver", activeTextBrush);
    SetControlBrushResource(btn, L"ToggleButtonForegroundCheckedPressed", activeTextBrush);

    if (borderBrush)
        btn.BorderBrush(borderBrush);
    else
        btn.ClearValue(Control::BorderBrushProperty());
    SetControlBrushResource(btn, L"ToggleButtonBorderBrush", borderBrush);
    SetControlBrushResource(btn, L"ToggleButtonBorderBrushPointerOver", borderBrush);
    SetControlBrushResource(btn, L"ToggleButtonBorderBrushPressed", borderBrush);
    SetControlBrushResource(btn, L"ToggleButtonBorderBrushChecked", borderBrush);
    SetControlBrushResource(btn, L"ToggleButtonBorderBrushCheckedPointerOver", borderBrush);
    SetControlBrushResource(btn, L"ToggleButtonBorderBrushCheckedPressed", borderBrush);

    if (g_settings.activeBold)
        btn.FontWeight(isActive
            ? winrt::Windows::UI::Text::FontWeights::Bold()
            : winrt::Windows::UI::Text::FontWeights::Normal());

    btn.IsThreeState(false);
    btn.IsChecked(isActive);
}

static void StyleButtonGeometry(Control const& btn,
                                std::wstring const& fontFamily) {
    btn.MinWidth(0.0);
    btn.MinHeight(0.0);
    btn.Padding({ 1.0, 0.0, 1.0, 0.0 });
    btn.FontSize((double)g_settings.fontSize);
    if (!fontFamily.empty())
        btn.FontFamily(winrt::Windows::UI::Xaml::Media::FontFamily(fontFamily));
    else
        btn.ClearValue(Control::FontFamilyProperty());
    btn.HorizontalAlignment(HorizontalAlignment::Stretch);
    btn.VerticalAlignment(VerticalAlignment::Stretch);

    double r = (double)g_settings.cornerRadius;
    btn.CornerRadius({ r, r, r, r });

    if (g_settings.borderThickness >= 0) {
        double t = (double)g_settings.borderThickness;
        btn.BorderThickness({ t, t, t, t });
    }
}

static void StyleDesktopButton(ToggleButton& btn, bool isActive,
    Brush activeBrush, Brush inactiveBrush,
    Brush activeTextBrush, Brush inactiveTextBrush,
    Brush hoverBrush, Brush pressedBrush, Brush borderBrush)
{
    StyleButtonGeometry(btn, g_settings.fontFamily);
    ApplyDesktopButtonState(btn, isActive, activeBrush, inactiveBrush,
                            activeTextBrush, inactiveTextBrush,
                            hoverBrush, pressedBrush, borderBrush);
}

static void StyleMasterButton(Button& btn,
    Brush inactiveBrush, Brush inactiveTextBrush,
    Brush hoverBrush, Brush pressedBrush, Brush borderBrush)
{
    StyleButtonGeometry(btn, g_settings.taskViewFontFamily);
    ApplyMasterButtonState(btn, inactiveBrush, inactiveTextBrush,
                           hoverBrush, pressedBrush, borderBrush);
}

static Grid BuildButtonGrid(int count, int current) {
    std::vector<ngl::Placement> placements;
    ngl::Size total;
    ComputeButtonPlacements(count, placements, total);

    Grid grid;
    std::vector<ButtonEventState> eventStates;
    grid.Name(L"VdSwitcherBar");
    // Absolute placement: one implicit cell, each button positioned by Margin
    // from the group's top-left. The group is sized to the arranger's padded
    // box and centered in the tray slot; spacing, shape, per-item offsets, and
    // short-group justification are already baked into the placements.
    grid.Width(total.width);
    grid.Height(total.height);
    grid.HorizontalAlignment(HorizontalAlignment::Center);
    grid.VerticalAlignment(VerticalAlignment::Center);
    // Adjust.OffsetX/Y translate the whole group without reserving space.
    if (g_settings.offsetX != 0 || g_settings.offsetY != 0)
        grid.Margin({(double)g_settings.offsetX, (double)g_settings.offsetY,
                     0.0, 0.0});
    if (g_settings.opacity < 100)
        grid.Opacity(std::max(0.0, std::min(1.0, g_settings.opacity / 100.0)));

    auto activeBrush       = ParseColorBrush(g_settings.activeBackgroundColor);
    auto inactiveBrush     = ParseColorBrush(g_settings.inactiveBackgroundColor);
    auto activeTextBrush   = ParseColorBrush(g_settings.activeTextColor);
    auto inactiveTextBrush = ParseColorBrush(g_settings.inactiveTextColor);
    auto hoverBrush        = ParseColorBrush(g_settings.hoverBackgroundColor);
    auto pressedBrush      = ParseColorBrush(g_settings.pressedBackgroundColor);
    auto borderBrush       = ParseColorBrush(g_settings.borderColor);
    auto desktopNames      = ReadDesktopNames(count);

    // Which token is which button, so a hand-written arrangement never has to
    // depend on the labels themselves.
    {
        std::wstring map;
        for (int i = 0; i < count; i++) {
            if (!map.empty())
                map += L"  ";
            map += std::to_wstring(i + 1) + L"=" + desktopNames[i];
        }
        if (g_settings.taskViewButton)
            map += L"  master=Task View";
        Wh_Log(L"[Layout] tokens: %ls", map.c_str());
    }

    for (auto const& p : placements) {
        if (IsMasterToken(p.token)) {
            Button masterBtn;
            masterBtn.Name(L"VdMasterBtn");
            TextBlock content;
            content.Text(winrt::hstring(g_settings.taskViewLabel));
            content.HorizontalAlignment(HorizontalAlignment::Center);
            content.VerticalAlignment(VerticalAlignment::Center);
            masterBtn.Content(content);
            StyleMasterButton(masterBtn, inactiveBrush, inactiveTextBrush,
                              hoverBrush, pressedBrush, borderBrush);
            masterBtn.Width(p.size.width);
            masterBtn.Height(p.size.height);
            masterBtn.HorizontalAlignment(HorizontalAlignment::Left);
            masterBtn.VerticalAlignment(VerticalAlignment::Top);
            masterBtn.Margin({p.x, p.y, 0.0, 0.0});
            ToolTipService::SetToolTip(masterBtn,
                winrt::box_value(winrt::hstring(L"Task View (Win+Tab)")));
            auto masterClickToken = masterBtn.Click([](auto const&, auto const&) {
                if (g_unloading) return;
                INPUT inputs[4]{};
                inputs[0].type = INPUT_KEYBOARD; inputs[0].ki.wVk = VK_LWIN;
                inputs[1].type = INPUT_KEYBOARD; inputs[1].ki.wVk = VK_TAB;
                inputs[2].type = INPUT_KEYBOARD; inputs[2].ki.wVk = VK_TAB;  inputs[2].ki.dwFlags = KEYEVENTF_KEYUP;
                inputs[3].type = INPUT_KEYBOARD; inputs[3].ki.wVk = VK_LWIN; inputs[3].ki.dwFlags = KEYEVENTF_KEYUP;
                SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));
            });
            eventStates.push_back({grid, masterBtn, masterClickToken});
            grid.Children().Append(masterBtn);
            continue;
        }

        int idx = DesktopIndexFromToken(p.token, count);
        if (idx < 0)
            continue;

        ToggleButton btn;
        btn.Name(L"VdBtn_" + std::to_wstring(idx));
        TextBlock content;
        content.Text(winrt::hstring(GetButtonLabel(idx, current)));
        content.HorizontalAlignment(HorizontalAlignment::Center);
        content.VerticalAlignment(VerticalAlignment::Center);
        btn.Content(content);
        StyleDesktopButton(btn, idx == current, activeBrush, inactiveBrush,
                           activeTextBrush, inactiveTextBrush,
                           hoverBrush, pressedBrush, borderBrush);
        btn.Width(p.size.width);
        btn.Height(p.size.height);
        btn.HorizontalAlignment(HorizontalAlignment::Left);
        btn.VerticalAlignment(VerticalAlignment::Top);
        btn.Margin({p.x, p.y, 0.0, 0.0});
        if (!g_settings.hoverPreview)
            ToolTipService::SetToolTip(btn, winrt::box_value(winrt::hstring(desktopNames[idx])));

        ButtonEventState previewEvents{};
        if (g_settings.hoverPreview) {
            previewEvents.previewEnter = btn.PointerEntered([idx](auto const& sender, auto const&) {
                try {
                    desktop_preview::Schedule(sender.template as<FrameworkElement>(), idx,
                        g_settings.previewWidth, g_settings.previewDelay);
                } catch (...) { desktop_preview::Hide(); LogCurrentUiException(L"desktop preview"); }
            });
            previewEvents.previewExit = btn.PointerExited([](auto const&, auto const&) { desktop_preview::Hide(); });
            previewEvents.previewCancel = btn.PointerCanceled([](auto const&, auto const&) { desktop_preview::Hide(); });
        }

        int capturedIdx = idx;
        auto clickToken = btn.Click([capturedIdx](auto const& sender, auto const&) {
            desktop_preview::Hide();
            // ToggleButton changes IsChecked before raising Click. Keep the
            // visual state tied to the actual current desktop while the COM
            // switch runs asynchronously (and if the switch fails).
            try {
                if (auto toggle = sender.template try_as<ToggleButton>())
                    toggle.IsChecked(capturedIdx == g_currentDesktop.load());
            } catch (...) {
                LogCurrentUiException(L"desktop button click");
            }
            if (g_unloading) return;
            // Dispatch to a background thread to avoid STA re-entrancy: when
            // SwitchToDesktop makes a LOCAL_SERVER COM call on the UI thread,
            // the STA message pump runs and can deliver the notification thread's
            // SendMessage re-entrantly, corrupting XAML state mid-click.
            // Serialize creation with Wh_ModUninit so every successful worker
            // handle is retained and waited before the mod image is freed.
            std::lock_guard lock(g_switchThreadsMutex);
            if (g_unloading) return;
            HANDLE h = CreateThread(nullptr, 0, [](LPVOID p2) -> DWORD {
                int i2 = (int)(INT_PTR)p2;
                CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
                if (!g_unloading) SwitchToDesktop(i2);
                CoUninitialize();
                return 0;
            }, (LPVOID)(INT_PTR)capturedIdx, 0, nullptr);
            if (h)
                g_switchThreads->push_back(h);
            else
                Wh_Log(L"[Switch] Failed to create desktop-switch thread");
        });
        previewEvents.owner = grid;
        previewEvents.button = btn;
        previewEvents.clickToken = clickToken;
        eventStates.push_back(std::move(previewEvents));
        grid.Children().Append(btn);
    }

    for (auto& state : eventStates)
        g_buttonEventStates->push_back(std::move(state));
    return grid;
}

// ============================================================
// Injection into XAML tree
// ============================================================

static bool PositionIsStartMode() {
    auto pos = g_settings.position;
    return pos == VdPosition::LeftOfStart || pos == VdPosition::OverStart ||
           pos == VdPosition::RightOfStart;
}

static FrameworkElement FindStartButton(FrameworkElement root) {
    return FindChildRecursive(root, [](FrameworkElement fe) {
        if (winrt::get_class_name(fe) != L"Taskbar.ExperienceToggleButton")
            return false;
        return AutomationProperties::GetAutomationId(fe) == L"StartButton";
    });
}

static FrameworkElement FindTaskbarRootGrid(FrameworkElement root) {
    auto taskbarFrame = FindChildRecursive(root, [](FrameworkElement fe) {
        return winrt::get_class_name(fe) == L"Taskbar.TaskbarFrame";
    });
    if (!taskbarFrame) return nullptr;

    int n = VisualTreeHelper::GetChildrenCount(taskbarFrame);
    for (int i = 0; i < n; i++) {
        auto child = VisualTreeHelper::GetChild(taskbarFrame, i).try_as<FrameworkElement>();
        if (child && child.Name() == L"RootGrid")
            return child;
    }
    return nullptr;
}

static FrameworkElement FindTaskbarFrameRepeater(FrameworkElement rootGrid) {
    int n = VisualTreeHelper::GetChildrenCount(rootGrid);
    for (int i = 0; i < n; i++) {
        auto child = VisualTreeHelper::GetChild(rootGrid, i).try_as<FrameworkElement>();
        if (child && child.Name() == L"TaskbarFrameRepeater")
            return child;
    }
    return nullptr;
}

static void SetTaskItemsLeftMargin(double left) {
    if (!g_taskItemsPanel)
        return;

    auto margin = g_taskItemsPanel.Margin();
    if (std::fabs(margin.Left - left) <= 0.5)
        return;

    margin.Left = left;
    g_taskItemsPanel.Margin(margin);
}

static void SetStartButtonVisualOffset(double x) {
    if (!g_startOverlayStart)
        return;

    if (std::fabs(x) <= 0.5) {
        if (!g_startOverlayStart.RenderTransform())
            return;
        g_startOverlayStart.ClearValue(UIElement::RenderTransformProperty());
        return;
    }

    auto existing = g_startOverlayStart.RenderTransform().try_as<TranslateTransform>();
    if (existing && std::fabs(existing.X() - x) <= 0.5 && existing.Y() == 0.0)
        return;

    TranslateTransform tt;
    tt.X(x);
    tt.Y(0.0);
    g_startOverlayStart.RenderTransform(tt);
}

static double GetElementActualWidth(FrameworkElement const& element) {
    return element ? element.ActualWidth() : 0.0;
}

static double GetElementActualHeight(FrameworkElement const& element) {
    return element ? element.ActualHeight() : 0.0;
}

static void PositionButtonGridNearStart() {
    if (!g_buttonGrid || !g_startOverlayRoot || !g_startOverlayStart)
        return;

    int count = g_desktopCount.load();
    // The arranger's total already includes Adjust.PadX/PadY on both sides.
    ngl::Size gridSize = EstimateButtonGridSize(count);
    double gridW = gridSize.width;
    double gridH = gridSize.height;
    auto pos = g_settings.position;

    bool startHidden = (g_startOverlayStart.Visibility() == Visibility::Collapsed);
    double startW = g_startOverlayStart.ActualWidth();
    double startH = g_startOverlayStart.ActualHeight();
    if (startW <= 0.0 && !startHidden) startW = 44.0;
    if (startH <= 0.0) startH = std::max((double)g_settings.itemHeight, gridH);

    // x changes when TaskbarFrameRepeater.Margin.Left is pushed (start button moves with it).
    // y is unaffected by horizontal margin changes and is always valid for vertical centering.
    double x = 0.0;
    double y = 0.0;
    try {
        auto transform = g_startOverlayStart.TransformToVisual(g_startOverlayRoot);
        winrt::Windows::Foundation::Point origin{ 0.0f, 0.0f };
        auto p = transform.TransformPoint(origin);
        x = p.X;
        y = p.Y;
    } catch (...) {
    }

    double left = 0.0;
    double top  = 0.0;

    if (pos == VdPosition::OverStart) {
        // Grid overlays the Start button. Adjust.OffsetY nudges it vertically.
        double anchorX = (g_startButtonOriginalX >= 0.0) ? g_startButtonOriginalX : x;
        left = anchorX;
        top  = y + (startH - gridH) / 2.0;
        if (left < 0.0) left = 0.0;
        SetStartButtonVisualOffset(0.0);
    } else if (pos == VdPosition::RightOfStart) {
        // Grid sits immediately right of the start button and reserves room for
        // itself before taskbar items. TaskbarFrameRepeater.Margin.Left moves
        // Start too, so counter-shift Start visually back to its stable anchor.
        double anchorX = (g_startButtonOriginalX >= 0.0) ? g_startButtonOriginalX : x;
        left = anchorX + startW;
        top  = y + (startH - gridH) / 2.0;
        if (left < 0.0) left = 0.0;

        if (g_taskItemsPanel) {
            double push = gridW + (double)g_settings.itemSpacing;
            SetTaskItemsLeftMargin(g_taskItemsPanelOriginalMargin.Left + push);
            if (!startHidden)
                SetStartButtonVisualOffset(-push);
        } else {
            SetStartButtonVisualOffset(0.0);
        }
    } else {
        // leftOfStart: anchor the grid at the left edge; push TaskbarFrameRepeater
        // rightward so Start button and task items don't overlap the grid.
        // y from TransformToVisual is unaffected by Margin.Left changes, so it
        // stays valid for vertical centering even after we push the panel right.
        left = 0.0;
        top  = y + (startH - gridH) / 2.0;
        SetStartButtonVisualOffset(0.0);

        if (g_taskItemsPanel) {
            double neededLeft = g_taskItemsPanelOriginalMargin.Left +
                                gridW + (double)g_settings.itemSpacing;
            SetTaskItemsLeftMargin(neededLeft);
        }
    }

    // Adjust.OffsetX/Y nudge the grid in every Start mode. In tray positions
    // BuildButtonGrid applies them as the group's margin instead.
    left += (double)g_settings.offsetX;
    top  += (double)g_settings.offsetY;

    double rootW = GetElementActualWidth(g_startOverlayRoot);
    double rootH = GetElementActualHeight(g_startOverlayRoot);
    if (rootW > 0.0 && left + gridW > rootW)
        left = std::max(0.0, rootW - gridW);
    if (rootH > 0.0 && top + gridH > rootH)
        top = std::max(0.0, rootH - gridH);
    if (top < 0.0) top = 0.0;

    g_buttonGrid.HorizontalAlignment(HorizontalAlignment::Left);
    g_buttonGrid.VerticalAlignment(VerticalAlignment::Top);
    auto current = g_buttonGrid.Margin();
    if (std::fabs(current.Left - left) > 0.5 ||
        std::fabs(current.Top - top) > 0.5 ||
        current.Right != 0.0 ||
        current.Bottom != 0.0) {
        g_buttonGrid.Margin({ left, top, 0.0, 0.0 });
    }
}

static bool InjectButtonGridNearStart(FrameworkElement root) {
    auto rootGrid = FindTaskbarRootGrid(root);
    if (!rootGrid) {
        Wh_Log(L"[Inject] Taskbar RootGrid not found");
        return false;
    }

    auto gridParent = rootGrid.try_as<Grid>();
    if (!gridParent) {
        Wh_Log(L"[Inject] Taskbar RootGrid is not a Grid");
        return false;
    }

    for (auto child : gridParent.Children()) {
        if (auto fe = child.try_as<FrameworkElement>(); fe && fe.Name() == L"VdSwitcherBar")
            return true;
    }

    auto startButton = FindStartButton(root);
    if (!startButton) {
        Wh_Log(L"[Inject] StartButton not found");
        return false;
    }

    int count   = ReadDesktopCount();
    int current = ReadCurrentDesktop();
    g_desktopCount.store(count);
    g_currentDesktop.store(current);

    if (g_settings.hideWhenSingle && count <= 1) {
        Wh_Log(L"[Inject] Skipping start overlay - hideWhenSingle, count=%d", count);
        return true;
    }

    auto grid = BuildButtonGrid(count, current);
    grid.IsHitTestVisible(true);
    Grid::SetColumn(grid, 0);
    Grid::SetColumnSpan(grid, std::max(1, (int)gridParent.ColumnDefinitions().Size()));
    Canvas::SetZIndex(grid, 1000);
    gridParent.Children().Append(grid);

    g_buttonGrid = grid;
    g_injectionParent = rootGrid;
    g_injectedColumn = -1;
    g_startOverlayMode = true;
    g_startOverlayRoot = rootGrid;
    g_startOverlayStart = startButton;

    // Capture TaskbarFrameRepeater for modes that reserve space near Start.
    if (auto repeater = FindTaskbarFrameRepeater(rootGrid)) {
        g_taskItemsPanel = repeater;
        g_taskItemsPanelOriginalMargin = repeater.Margin();
    }

    // Capture original start button x before any margin pushes — stable anchor.
    try {
        auto t = startButton.TransformToVisual(rootGrid);
        winrt::Windows::Foundation::Point o{ 0.0f, 0.0f };
        g_startButtonOriginalX = t.TransformPoint(o).X;
    } catch (...) {
        g_startButtonOriginalX = -1.0;
    }

    PositionButtonGridNearStart();
    g_startOverlayLayoutToken = rootGrid.LayoutUpdated(
        [](winrt::Windows::Foundation::IInspectable const&, auto const&) {
            try {
                if (!g_unloading)
                    PositionButtonGridNearStart();
            } catch (...) {
                LogCurrentUiException(L"Start placement layout");
            }
        });

    Wh_Log(L"[Inject] VdSwitcherBar near Start (%d desktops, current=%d)", count, current);
    return true;
}

// Windows 11 26200.9457 (KB5129195) kept the name SystemTrayFrameGrid but
// changed the element from a Grid to a StackPanel, where a child's index is its
// position and there are no columns at all. Both layouts are live in the wild —
// older builds still ship the Grid — so the kind is classified on every touch
// and never cached across a tray rebuild.
enum class TrayKind { Unsupported, Columns, Order };

static TrayKind ClassifyTray(FrameworkElement const& panel) {
    if (!panel) return TrayKind::Unsupported;
    if (panel.try_as<Grid>()) return TrayKind::Columns;
    if (panel.try_as<StackPanel>()) return TrayKind::Order;
    return TrayKind::Unsupported;  // never guess another panel's semantics
}

static std::wstring TrayClassName(FrameworkElement const& element) {
    if (!element) return L"(null)";
    try {
        return std::wstring(winrt::get_class_name(element));
    } catch (...) {
        return L"(unknown)";
    }
}

static int IndexOfTrayChild(Panel const& parent, FrameworkElement const& child) {
    if (!parent || !child) return -1;
    uint32_t index = 0;
    return parent.Children().IndexOf(child, index) ? (int)index : -1;
}

// Named direct children, for logging when an anchor cannot be resolved. A tray
// restructure shows up here as missing or renamed names, which is the one thing
// a user's debug log otherwise cannot tell us.
static std::wstring DescribeTrayChildren(Panel const& parent) {
    if (!parent) return L"(none)";
    std::wstring names;
    try {
        for (auto child : parent.Children()) {
            auto fe = child.try_as<FrameworkElement>();
            if (!fe || fe.Name().empty()) continue;
            if (!names.empty()) names += L", ";
            names += fe.Name();
        }
    } catch (...) {
        return L"(unreadable)";
    }
    return names.empty() ? L"(no named children)" : names;
}

// Map g_settings.position to a target slot in a SystemTrayFrameGrid. On a Grid
// that means inserting a new Auto-width column and shifting existing children
// to make room; on a StackPanel it means inserting at a child index. Returns the
// slot the grid was placed in, or -1 on an unsupported panel.
static int InsertGridIntoTrayColumns(Panel const& gridParent, Grid const& grid) {
    TrayKind kind = ClassifyTray(gridParent);
    if (kind == TrayKind::Unsupported) {
        Wh_Log(L"[Inject] Unsupported SystemTrayFrameGrid type: %s",
               TrayClassName(gridParent).c_str());
        return -1;
    }
    auto pos = g_settings.position;

    // Find a named direct child of the tray grid.
    auto findNamedDirect = [&](const wchar_t* name) -> FrameworkElement {
        for (auto child : gridParent.Children()) {
            if (auto fe = child.try_as<FrameworkElement>(); fe && fe.Name() == name)
                return fe;
        }
        return nullptr;
    };

    // Map position setting → reference element + whether to insert after it.
    // Secondary taskbars may lack some of these elements; a missing reference
    // falls through to column 0 (before icons).
    FrameworkElement refElem = nullptr;
    bool insertAfterRef = false;

    if      (pos == VdPosition::BeforeOmni)
        refElem = findNamedDirect(L"ControlCenterButton");
    else if (pos == VdPosition::BeforeClock)
        refElem = findNamedDirect(L"NotificationCenterButton");
    else if (pos == VdPosition::AfterClock)
        refElem = findNamedDirect(L"ShowDesktopStack");
    else if (pos == VdPosition::AfterShowDesktop) {
        refElem = findNamedDirect(L"ShowDesktopStack");
        insertAfterRef = true;
    }
    // beforeIcons → slot 0 (refElem stays nullptr)

    // A requested anchor that isn't there means the tray was restructured, and
    // the bar silently lands at the far left instead. Say so, with the names
    // actually present, so the fallback is never mistaken for the setting.
    if (pos != VdPosition::BeforeIcons && !refElem) {
        Wh_Log(L"[Inject] Anchor for the requested position is missing; "
               L"falling back to slot 0. Tray %s holds: %s",
               TrayClassName(gridParent).c_str(),
               DescribeTrayChildren(gridParent).c_str());
    }

    if (kind == TrayKind::Order) {
        // Order is layout: the slot is a child index, and inserting there needs
        // no column bookkeeping at all.
        int insertIdx = 0;
        if (refElem) {
            insertIdx = IndexOfTrayChild(gridParent, refElem);
            if (insertIdx < 0) insertIdx = 0;
            if (insertAfterRef) insertIdx++;
        }
        Canvas::SetZIndex(grid, 10000);
        gridParent.Children().InsertAt(
            std::min((uint32_t)insertIdx, gridParent.Children().Size()), grid);
        return insertIdx;
    }

    auto trayGrid = gridParent.as<Grid>();

    int insertCol;
    if (insertAfterRef && refElem)
        insertCol = Grid::GetColumn(refElem) + 1;
    else if (refElem)
        insertCol = Grid::GetColumn(refElem);
    else
        insertCol = 0;  // beforeIcons: leftmost column in tray

    // Insert a new Auto-width column at insertCol.
    ColumnDefinition cd;
    cd.Width({ 1.0, GridUnitType::Auto });
    if ((uint32_t)insertCol < trayGrid.ColumnDefinitions().Size())
        trayGrid.ColumnDefinitions().InsertAt((uint32_t)insertCol, cd);
    else
        trayGrid.ColumnDefinitions().Append(cd);

    // Shift every existing child whose column is >= insertCol to make room.
    // Elements that start before insertCol but span through it get their span
    // widened so they continue to cover the same original columns (plus the new one).
    for (auto child : trayGrid.Children()) {
        auto fe = child.try_as<FrameworkElement>();
        if (!fe) continue;
        int col  = Grid::GetColumn(fe);
        int span = Grid::GetColumnSpan(fe);
        if (col >= insertCol)
            Grid::SetColumn(fe, col + 1);
        else if (col + span > insertCol)
            Grid::SetColumnSpan(fe, span + 1);
    }

    Grid::SetColumn(grid, insertCol);
    Canvas::SetZIndex(grid, 10000);
    trayGrid.Children().Append(grid);
    return insertCol;
}

static bool InjectButtonGrid(FrameworkElement root) {
    if (PositionIsStartMode())
        return InjectButtonGridNearStart(root);

    FrameworkElement parent = FindChildRecursive(root, [](FrameworkElement fe) {
        return fe.Name() == L"SystemTrayFrameGrid";
    });
    if (!parent) {
        Wh_Log(L"[Inject] SystemTrayFrameGrid not found");
        return false;
    }

    // A Grid on older taskbars, a StackPanel since 26200.9457. InsertGridInto-
    // TrayColumns handles the difference; here we only need a Panel to walk.
    auto gridParent = parent.try_as<Panel>();
    TrayKind kind = ClassifyTray(parent);
    if (!gridParent || kind == TrayKind::Unsupported) {
        Wh_Log(L"[Inject] Unsupported SystemTrayFrameGrid type: %s",
               TrayClassName(parent).c_str());
        return false;
    }

    // Already injected?
    for (auto child : gridParent.Children()) {
        if (auto fe = child.try_as<FrameworkElement>(); fe && fe.Name() == L"VdSwitcherBar") {
            // Re-acquire state in case it was lost (e.g., transient null from GetTaskbarXamlRoot
            // during RebuildButtonGrid caused g_buttonGrid to be cleared while grid stayed in tree).
            if (!g_buttonGrid) {
                g_buttonGrid = fe.try_as<Grid>();
                g_injectionParent = parent;
                g_injectedColumn = kind == TrayKind::Columns
                                       ? Grid::GetColumn(fe)
                                       : IndexOfTrayChild(gridParent, fe);
                Wh_Log(L"[Inject] Re-acquired existing VdSwitcherBar at %s=%d",
                       kind == TrayKind::Columns ? L"col" : L"index",
                       g_injectedColumn);
            }
            return true;
        }
    }

    int count   = ReadDesktopCount();
    int current = ReadCurrentDesktop();
    g_desktopCount.store(count);
    g_currentDesktop.store(current);

    if (g_settings.hideWhenSingle && count <= 1) {
        Wh_Log(L"[Inject] Skipping — hideWhenSingle, count=%d", count);
        return true;  // notification thread will watch for desktop additions
    }

    auto grid = BuildButtonGrid(count, current);
    int insertCol = InsertGridIntoTrayColumns(gridParent, grid);
    if (insertCol < 0) return false;
    g_buttonGrid      = grid;
    g_injectionParent = parent;
    g_injectedColumn  = insertCol;

    Wh_Log(L"[Inject] VdSwitcherBar at %s=%d in %ls (%d desktops, current=%d)",
           kind == TrayKind::Columns ? L"column" : L"index",
           insertCol, parent.Name().c_str(), count, current);
    return true;
}

static Panel FindLiveSystemTrayFrameGrid() {
    HWND hWnd = tbh::ResolveTaskbarWnd(g_taskbarWnd);
    if (!hWnd) return nullptr;

    auto xamlRoot = GetTaskbarXamlRoot(hWnd);
    if (!xamlRoot) return nullptr;
    auto root = xamlRoot.Content().try_as<FrameworkElement>();
    if (!root) return nullptr;

    auto parent = FindChildRecursive(root, [](FrameworkElement fe) {
        return fe.Name() == L"SystemTrayFrameGrid";
    });
    // Grid on older taskbars, StackPanel since 26200.9457; Panel covers both.
    return parent ? parent.try_as<Panel>() : nullptr;
}

// XAML can defer teardown of a removed subtree until after the mod DLL unloads.
// Release every delegate and mod-created boxed value before detaching a grid.
static void ClearButtonEventState(Grid const& owner) {
    desktop_preview::Hide();
    if (!owner) return;
    for (auto it = g_buttonEventStates->begin(); it != g_buttonEventStates->end();) {
        if (it->owner != owner) {
            ++it;
            continue;
        }
        try {
            if (it->clickToken.value)
                it->button.Click(it->clickToken);
            if (it->previewEnter.value) it->button.PointerEntered(it->previewEnter);
            if (it->previewExit.value) it->button.PointerExited(it->previewExit);
            if (it->previewCancel.value) it->button.PointerCanceled(it->previewCancel);
            ToolTipService::SetToolTip(it->button, nullptr);
            if (auto contentControl = it->button.try_as<ContentControl>())
                contentControl.Content(nullptr);
        } catch (...) {
            Wh_Log(L"[Remove] Failed to clear one button's event state");
        }
        it = g_buttonEventStates->erase(it);
    }
}

static void ClearAllButtonEventState() {
    while (!g_buttonEventStates->empty()) {
        auto owner = g_buttonEventStates->front().owner;
        if (owner)
            ClearButtonEventState(owner);
        else
            g_buttonEventStates->erase(g_buttonEventStates->begin());
    }
}

static bool RemoveButtonGridFrom(Panel gridParent, int col) {
    if (!gridParent) return false;

    uint32_t removeIdx = (uint32_t)-1;
    int liveCol = col;
    for (uint32_t i = 0; i < gridParent.Children().Size(); i++) {
        auto fe = gridParent.Children().GetAt(i).try_as<FrameworkElement>();
        if (fe && fe.Name() == L"VdSwitcherBar") {
            removeIdx = i;
            liveCol = Grid::GetColumn(fe);
            break;
        }
    }
    if (removeIdx == (uint32_t)-1) return false;

    auto ownedGrid = gridParent.Children().GetAt(removeIdx).try_as<Grid>();
    ClearButtonEventState(ownedGrid);
    gridParent.Children().RemoveAt(removeIdx);

    // A StackPanel tray owns no column to give back; removing the child is
    // the whole teardown there.
    auto trayGrid = gridParent.try_as<Grid>();
    if (trayGrid && liveCol >= 0) {
        uint32_t colU = (uint32_t)liveCol;
        if (colU < trayGrid.ColumnDefinitions().Size())
            trayGrid.ColumnDefinitions().RemoveAt(colU);
        for (auto child : trayGrid.Children()) {
            auto fe = child.try_as<FrameworkElement>();
            if (!fe) continue;
            int c    = Grid::GetColumn(fe);
            int span = Grid::GetColumnSpan(fe);
            if (c > liveCol)
                Grid::SetColumn(fe, c - 1);
            else if (c < liveCol && c + span > liveCol)
                Grid::SetColumnSpan(fe, span - 1);
        }
    }
    return true;
}

static void RemoveButtonGrid() {
    if (g_startOverlayMode) {
        if (g_startOverlayRoot && g_startOverlayLayoutToken.value) {
            g_startOverlayRoot.LayoutUpdated(g_startOverlayLayoutToken);
            g_startOverlayLayoutToken = {};
        }

        auto gridParent = g_injectionParent ? g_injectionParent.try_as<Grid>() : nullptr;
        bool removed = false;
        if (gridParent) {
            for (uint32_t i = 0; i < gridParent.Children().Size(); i++) {
                auto fe = gridParent.Children().GetAt(i).try_as<FrameworkElement>();
                if (fe && fe.Name() == L"VdSwitcherBar") {
                    ClearButtonEventState(fe.try_as<Grid>());
                    gridParent.Children().RemoveAt(i);
                    removed = true;
                    break;
                }
            }
        }
        if (!removed)
            ClearButtonEventState(g_buttonGrid);

        if (g_taskItemsPanel) {
            g_taskItemsPanel.Margin(g_taskItemsPanelOriginalMargin);
            g_taskItemsPanel = nullptr;
        }
        SetStartButtonVisualOffset(0.0);
        g_startButtonOriginalX = -1.0;

        g_buttonGrid = nullptr;
        g_injectionParent = nullptr;
        g_injectedColumn = -1;
        g_startOverlayMode = false;
        g_startOverlayRoot = nullptr;
        g_startOverlayStart = nullptr;
        return;
    }

    auto gridParent = FindLiveSystemTrayFrameGrid();
    if (!gridParent) {
        // The tree may already be detaching, but the button handlers still
        // point into this DLL. Revoke them even when there is no live parent
        // left to perform the physical removal.
        Wh_Log(L"[Remove] No live tray grid; revoking VdSwitcherBar callbacks");
        ClearButtonEventState(g_buttonGrid);
        g_buttonGrid      = nullptr;
        g_injectionParent = nullptr;
        g_injectedColumn  = -1;
        return;
    }
    if (!RemoveButtonGridFrom(gridParent, g_injectedColumn)) {
        Wh_Log(L"[Remove] VdSwitcherBar not found");
        ClearButtonEventState(g_buttonGrid);
    }

    g_buttonGrid      = nullptr;
    g_injectionParent = nullptr;
    g_injectedColumn  = -1;
}

// ============================================================
// Secondary taskbars (experimental multi-monitor)
// ============================================================
// GetTaskbarXamlRoot goes through the primary CTaskBand only, so secondary
// taskbars (Shell_SecondaryTrayWnd) are discovered from IconView elements as
// they load: an element's XamlRoot belongs to whichever taskbar hosts it.
// Discovered tray grids stay registered while the toggle is off so enabling
// it can reinject without waiting for new icons. All access is UI-thread only.

struct SecondaryBar {
    Panel trayGrid{nullptr};   // SystemTrayFrameGrid of a secondary taskbar
    Grid buttonGrid{nullptr};  // injected VdSwitcherBar, null when not injected
};
[[clang::no_destroy]] static std::optional<std::vector<SecondaryBar>>
    g_secondaryBars{std::in_place};

static bool IsTrayGridAlive(Panel const& trayGrid) {
    try {
        return trayGrid && trayGrid.XamlRoot() != nullptr;
    } catch (...) {
        return false;
    }
}

static bool WantSecondaryBars() {
    return g_settings.allTaskbars && !PositionIsStartMode() &&
           !(g_settings.hideWhenSingle && g_desktopCount.load() <= 1);
}

// Remove injected grids from all secondary taskbars, keeping the tray-grid
// registrations so the bars can be reinjected without rediscovery.
static void RemoveSecondaryBars() {
    for (auto& bar : *g_secondaryBars) {
        if (bar.buttonGrid) {
            if (IsTrayGridAlive(bar.trayGrid))
                RemoveButtonGridFrom(bar.trayGrid, -1);
            else
                ClearButtonEventState(bar.buttonGrid);
        }
        bar.buttonGrid = nullptr;
    }
}

// Remove and reinject the grid on every known secondary taskbar (or just
// remove, when the toggle/settings no longer want them). Full rebuild for the
// same reason as the primary grid — see RebuildButtonGrid.
static void RefreshSecondaryBars() {
    if (g_secondaryBars->empty())
        return;
    int count    = g_desktopCount.load();
    int current  = g_currentDesktop.load();
    bool wantBars = WantSecondaryBars();
    for (auto it = g_secondaryBars->begin(); it != g_secondaryBars->end();) {
        auto& bar = *it;
        if (!IsTrayGridAlive(bar.trayGrid)) {
            // Taskbar went away (monitor disconnected or tray rebuilt); a new
            // IconView load on that taskbar will rediscover it.
            ClearButtonEventState(bar.buttonGrid);
            it = g_secondaryBars->erase(it);
            continue;
        }
        if (bar.buttonGrid) {
            RemoveButtonGridFrom(bar.trayGrid, -1);
            bar.buttonGrid = nullptr;
        }
        if (wantBars) {
            auto grid = BuildButtonGrid(count, current);
            InsertGridIntoTrayColumns(bar.trayGrid, grid);
            bar.buttonGrid = grid;
        }
        ++it;
    }
}

// Register the tray grid of the taskbar hosting this element and inject the
// switcher into it when the toggle is on. The primary taskbar is excluded —
// it is handled by the main injection path (including Start positions).
static void RegisterSecondaryTrayFromElement(FrameworkElement const& element) {
    if (g_unloading || !g_buttonGrid)  // wait until the primary bar exists
        return;

    auto xamlRoot = element.XamlRoot();
    if (!xamlRoot) return;
    auto root = xamlRoot.Content().try_as<FrameworkElement>();
    if (!root) return;

    // Never treat the primary taskbar as secondary.
    if (HWND hWnd = tbh::ResolveTaskbarWnd(g_taskbarWnd)) {
        try {
            if (auto primaryRoot = GetTaskbarXamlRoot(hWnd))
                if (primaryRoot.Content() == xamlRoot.Content())
                    return;
        } catch (...) {
        }
    }

    auto trayElement = FindChildRecursive(root, [](FrameworkElement fe) {
        return fe.Name() == L"SystemTrayFrameGrid";
    });
    auto trayGrid = trayElement.try_as<Panel>();
    if (!trayGrid || ClassifyTray(trayElement) == TrayKind::Unsupported) return;

    for (auto& bar : *g_secondaryBars)
        if (bar.trayGrid == trayGrid) return;  // already known

    // Defensive: skip trays that somehow already contain our bar.
    for (auto child : trayGrid.Children()) {
        if (auto fe = child.try_as<FrameworkElement>(); fe && fe.Name() == L"VdSwitcherBar")
            return;
    }

    SecondaryBar bar;
    bar.trayGrid = trayGrid;
    if (WantSecondaryBars()) {
        auto grid = BuildButtonGrid(g_desktopCount.load(), g_currentDesktop.load());
        InsertGridIntoTrayColumns(bar.trayGrid, grid);
        bar.buttonGrid = grid;
        Wh_Log(L"[Inject] VdSwitcherBar on secondary taskbar (%u registered)",
               (unsigned)(g_secondaryBars->size() + 1));
    }
    g_secondaryBars->push_back(std::move(bar));
}

// Rebuild the button grid on the UI thread. Always a full rebuild: button
// backgrounds use lightweight styling resources (ButtonBackground etc.), and
// {ThemeResource} references resolve once at template application — swapping
// the resources on a live button has no effect, so in-place highlight updates
// can't work. Freshly built buttons resolve correctly.
static void RebuildButtonGrid() {
    int count   = ReadDesktopCount();
    int current = ReadCurrentDesktop();
    g_desktopCount.store(count);
    g_currentDesktop.store(current);

    // Secondary bars first — RefreshSecondaryBars handles the hideWhenSingle
    // and toggle-off cases internally, so it must run before the early returns.
    RefreshSecondaryBars();

    if (g_settings.hideWhenSingle) {
        if (count <= 1) {
            if (g_buttonGrid) RemoveButtonGrid();
            return;
        }
        if (!g_buttonGrid) {
            ApplyAllSettings();
            return;
        }
    }

    if (!g_buttonGrid) { ApplyAllSettings(); return; }
    Panel gridParent{nullptr};
    uint32_t idx;
    if (g_startOverlayMode) {
        if (!g_injectionParent) { ApplyAllSettings(); return; }
        gridParent = g_injectionParent.try_as<Panel>();
        if (!gridParent || !gridParent.Children().IndexOf(g_buttonGrid, idx)) {
            ClearButtonEventState(g_buttonGrid);
            g_buttonGrid = nullptr;
            g_injectionParent = nullptr;
            g_injectedColumn = -1;
            ApplyAllSettings();
            return;
        }
    } else {
        // Use the live XAML tree — g_injectionParent may be stale if Windows
        // rebuilt the tray after a desktop add/remove.
        gridParent = FindLiveSystemTrayFrameGrid();
        if (!gridParent) {
            // XAML tree temporarily inaccessible (e.g., mid-rebuild by Windows).
            // Do NOT null g_buttonGrid — the grid is still in the tree; we just
            // can't reach it right now. The next notification will retry.
            Wh_Log(L"[Rebuild] XAML tree not accessible, deferring");
            return;
        }
        if (!gridParent.Children().IndexOf(g_buttonGrid, idx)) {
            // Our grid is genuinely gone (tray was rebuilt). Reinject from scratch.
            ClearButtonEventState(g_buttonGrid);
            g_buttonGrid = nullptr;
            g_injectionParent = nullptr;
            g_injectedColumn = -1;
            ApplyAllSettings();
            return;
        }
    }
    // Capture the live column BEFORE removing the old grid, so even if
    // g_injectedColumn is stale (columns were renumbered by Windows), we
    // reinsert at the correct position. On an order-based tray the child index
    // is the position, and removing then re-inserting at idx already preserves
    // it -- there is no column to carry over.
    bool ordered = !g_startOverlayMode &&
                   ClassifyTray(gridParent) == TrayKind::Order;
    int liveColumn =
        (g_startOverlayMode || ordered) ? 0 : Grid::GetColumn(g_buttonGrid);
    ClearButtonEventState(g_buttonGrid);
    gridParent.Children().RemoveAt(idx);
    g_buttonGrid = BuildButtonGrid(count, current);
    if (g_startOverlayMode) {
        auto overlayGrid = gridParent.try_as<Grid>();
        Grid::SetColumn(g_buttonGrid, 0);
        Grid::SetColumnSpan(
            g_buttonGrid,
            overlayGrid
                ? std::max(1, (int)overlayGrid.ColumnDefinitions().Size())
                : 1);
        Canvas::SetZIndex(g_buttonGrid, 1000);
        g_buttonGrid.IsHitTestVisible(true);
    } else if (ordered) {
        g_injectedColumn = (int)idx;
        Canvas::SetZIndex(g_buttonGrid, 10000);
    } else if (liveColumn >= 0) {
        Grid::SetColumn(g_buttonGrid, liveColumn);
        g_injectedColumn = liveColumn;
        Canvas::SetZIndex(g_buttonGrid, 10000);
    }
    gridParent.Children().InsertAt(idx, g_buttonGrid);
    if (g_startOverlayMode)
        PositionButtonGridNearStart();
}

// ============================================================
// Apply / cleanup
// ============================================================

// Logged once per orientation change rather than on every retry.
static bool g_verticalStandDownLogged = false;

static void ApplyAllSettings() {
    // A vertical taskbar ("Vertical Taskbar for Windows 11") walks the same
    // tray path this mod walks and owns RenderTransform on those children to
    // rotate them. This mod positions its button grid by writing that same
    // property on the same elements, so the two cannot both be right. Stand
    // down completely and leave the taskbar exactly as found; an Explorer
    // rebuild re-evaluates if the user turns that mod off.
    {
        HWND probe = tbh::ResolveTaskbarWnd(g_taskbarWnd);
        auto metrics = tbh::GetMetrics(probe);
        if (metrics.valid && !tbh::LayoutModelApplies(metrics)) {
            if (!g_verticalStandDownLogged) {
                g_verticalStandDownLogged = true;
                Wh_Log(L"[Apply] Taskbar is %s - standing down. This mod "
                       L"places its buttons with RenderTransform, which a "
                       L"vertical taskbar mod already owns on the same "
                       L"elements; leaving the native taskbar untouched.",
                       tbh::OrientationName(metrics.orientation));
            }
            return;
        }
        g_verticalStandDownLogged = false;
    }

    HWND hWnd = FindCurrentProcessTaskbarWnd();
    if (!hWnd) { Wh_Log(L"[Apply] No taskbar window"); return; }
    g_taskbarWnd = hWnd;

    try {
        auto xamlRoot = GetTaskbarXamlRoot(hWnd);
        if (!xamlRoot) { Wh_Log(L"[Apply] GetTaskbarXamlRoot failed"); return; }
        auto root = xamlRoot.Content().try_as<FrameworkElement>();
        if (!root) { Wh_Log(L"[Apply] No XAML root content"); return; }

        if (InjectButtonGrid(root))
            StartNotificationThread();
        else
            Wh_Log(L"[Apply] Injection failed");
    } catch (...) {
        Wh_Log(L"[Apply] Exception during injection (XAML not ready)");
    }
}

static void ApplyAllSettingsOnWindowThread() {
    HWND hWnd = tbh::ResolveTaskbarWnd(g_taskbarWnd);
    if (!hWnd) return;
    RunFromWindowThread(hWnd, [](void*) { ApplyAllSettings(); }, nullptr);
}

// ============================================================
// Hooks
// ============================================================

using IconView_IconView_t = void* (WINAPI*)(void* pThis);
IconView_IconView_t IconView_IconView_Original;

void* WINAPI IconView_IconView_Hook(void* pThis) {
    auto result = IconView_IconView_Original(pThis);
    try {
        if (g_unloading) return result;
        // Once the primary grid exists this hook is only needed to discover
        // secondary taskbars, which only matters with the multi-monitor toggle on.
        if (g_buttonGrid && !g_settings.allTaskbars) return result;

        // Defer until the element is live in the XAML tree. Calling ApplyAllSettings
        // immediately from the constructor fires before the XamlRoot is stable, causing
        // null dereferences and WinRT exceptions that propagate through WH_CALLWNDPROC
        // and crash the process on startup.
        FrameworkElement iconView = nullptr;
        ((IUnknown**)pThis)[1]->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                               winrt::put_abi(iconView));
        if (!iconView) {
            // Fallback: element isn't a FrameworkElement; try immediate path.
            ApplyAllSettingsOnWindowThread();
            return result;
        }

        g_autoRevokerList->emplace_back();
        auto autoRevokerIt = std::prev(g_autoRevokerList->end());
        *autoRevokerIt = iconView.Loaded(
            winrt::auto_revoke_t{},
            [autoRevokerIt](winrt::Windows::Foundation::IInspectable const& sender,
                            auto const&) {
                g_autoRevokerList->erase(autoRevokerIt);
                try {
                    if (g_unloading)
                        return;
                    if (!g_buttonGrid)
                        ApplyAllSettingsOnWindowThread();
                    // The sender's XamlRoot identifies the taskbar hosting this icon —
                    // register it if it is a secondary taskbar.
                    if (g_settings.allTaskbars) {
                        if (auto fe = sender.try_as<FrameworkElement>())
                            RegisterSecondaryTrayFromElement(fe);
                    }
                } catch (...) {
                    LogCurrentUiException(L"IconView Loaded");
                }
            });
    } catch (...) {
        LogCurrentUiException(L"IconView hook");
    }

    return result;
}

using LoadLibraryExW_t = HMODULE (WINAPI*)(LPCWSTR, HANDLE, DWORD);
LoadLibraryExW_t LoadLibraryExW_Original;

HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags) {
    HMODULE hModule = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (hModule && lpLibFileName)
        HandleLoadedModuleIfSystemTray(hModule, lpLibFileName);
    return hModule;
}

// ============================================================
// Symbol hook setup
// ============================================================

// Explorer rebuilt the taskbar: everything we were holding is gone. NEW in
// v2.1 — this mod previously relied on the IconView hook plus the bounded
// retry alone, so a rebuild that did not construct a fresh IconView could
// leave the grid missing until the next retry expired. TrayUI::StartTaskbar is
// the family's standard rebuild signal and the template hooks it for us.
static void OnTaskbarRebuilt() {
    if (g_unloading) return;
    g_taskbarWnd = nullptr;
    ApplyAllSettingsOnWindowThread();
}

static bool HookTaskbarDllSymbols() {
    return tbh::HookTaskbarSymbols(OnTaskbarRebuilt);
}

static bool HookSystemTraySymbols(HMODULE hModule) {
    // SystemTray.dll, Taskbar.View.dll, ExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK systemTrayHooks[] = {{
        {LR"(public: __cdecl winrt::SystemTray::implementation::IconView::IconView(void))"},
        &IconView_IconView_Original, IconView_IconView_Hook,
    }};
    if (!WindhawkUtils::HookSymbols(hModule, systemTrayHooks, ARRAYSIZE(systemTrayHooks))) {
        Wh_Log(L"[Hooks] HookSymbols failed");
        return false;
    }
    return true;
}

static void HandleLoadedModuleIfSystemTray(HMODULE hModule, LPCWSTR lpLibFileName) {
    if (!g_systemTrayModuleHooked && GetSystemTrayModuleHandle() == hModule &&
        !g_systemTrayModuleHooked.exchange(true)) {
        Wh_Log(L"[LoadLib] %s — hooking symbols", lpLibFileName);
        if (HookSystemTraySymbols(hModule))
            Wh_ApplyHookOperations();
    }
}

// ============================================================
// Windhawk lifecycle
// ============================================================

BOOL Wh_ModInit() {
    Wh_Log(L"[Init] VD Switcher v2.0");
    // Failures inside a template-marshalled UI callback report in this mod's
    // voice rather than vanishing.
    tbh::SetExceptionLogger(LogCurrentUiException);
    LoadSettings();
    DetectExplorerBuild();

    if (!HookTaskbarDllSymbols())
        Wh_Log(L"[Init] taskbar.dll hooks failed — GetTaskbarXamlRoot unavailable");

    if (HMODULE hSystemTray = GetSystemTrayModuleHandle()) {
        g_systemTrayModuleHooked = true;
        if (!HookSystemTraySymbols(hSystemTray))
            Wh_Log(L"[Init] System tray symbol hooks failed");
    } else {
        Wh_Log(L"[Init] System tray module not loaded yet");
        HMODULE kernelbase = GetModuleHandleW(L"kernelbase.dll");
        auto pLoadLibraryExW = kernelbase
            ? reinterpret_cast<LoadLibraryExW_t>(GetProcAddress(kernelbase, "LoadLibraryExW"))
            : nullptr;
        if (pLoadLibraryExW)
            WindhawkUtils::SetFunctionHook(pLoadLibraryExW,
                                           LoadLibraryExW_Hook,
                                           &LoadLibraryExW_Original);
    }
    return TRUE;
}

void Wh_ModAfterInit() {
    if (!g_systemTrayModuleHooked) {
        if (HMODULE hSystemTray = GetSystemTrayModuleHandle()) {
            if (!g_systemTrayModuleHooked.exchange(true)) {
                Wh_Log(L"[AfterInit] System tray module found — hooking symbols");
                if (HookSystemTraySymbols(hSystemTray))
                    Wh_ApplyHookOperations();
            }
        }
    }
    if (g_systemTrayModuleHooked)
        ApplyAllSettingsOnWindowThread();

    g_retryStopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_retryThread = CreateThread(nullptr, 0, [](void*) -> DWORD {
        for (int i = 0; i < 5 && !g_unloading; i++) {
            if (WaitForSingleObject(g_retryStopEvent, 2000) != WAIT_TIMEOUT) break;
            if (g_buttonGrid || g_unloading) break;
            Wh_Log(L"[AfterInit] Retry %d", i + 1);
            ApplyAllSettingsOnWindowThread();
        }
        return 0;
    }, nullptr, 0, nullptr);
}

void Wh_ModUninit() {
    g_unloading = true;
    Wh_Log(L"[Uninit]");

    // Waiting on each handle establishes that the worker has returned fully
    // out of mod code. A counter decremented inside its thread proc cannot.
    WaitForSwitchThreads();
    // Every handle was closed above; free the vector buffer on this controlled
    // unload path instead of retaining it behind no_destroy.
    {
        std::lock_guard lock(g_switchThreadsMutex);
        g_switchThreads.reset();
    }

    StopRetryThread();
    StopNotificationThread();

    // RunFromWindowThread is synchronous — blocks until the UI thread has removed the grid,
    // so all WinRT object lifetimes are safe and no FreeLibrary dance is needed.
    // Clear pending Loaded revokers on the UI thread so WinRT auto-revoke objects
    // are destroyed on the correct thread before the DLL is unloaded.
    HWND hWnd = tbh::ResolveTaskbarWnd(g_taskbarWnd);
    bool tornDown = false;
    if (hWnd) {
        tornDown = RunFromWindowThread(hWnd, [](void*) {
            desktop_preview::Destroy();
            // Controlled UI-thread unload: revoke/release on this thread, then
            // reset() the no_destroy optionals so their heap buffers are freed
            // (a bare no_destroy container would keep its capacity forever).
            g_autoRevokerList->clear();
            RemoveButtonGrid();
            RemoveSecondaryBars();
            ClearAllButtonEventState();
            g_autoRevokerList.reset();
            g_buttonEventStates.reset();
            g_secondaryBars.reset();  // release WinRT refs on the UI thread
        }, nullptr);
    }
    if (!tornDown) {
        // The popup has its own UI thread and WndProc. Tear it down through its
        // own window even if Shell_TrayWnd was recreated or is briefly absent.
        HWND popup = desktop_preview::state.popup;
        if (popup && !RunFromWindowThread(popup, [](void*) {
                desktop_preview::Destroy();
            }, nullptr)) {
            Wh_Log(L"[Uninit] Failed to dispatch desktop-preview cleanup");
        }
        // Explorer shutdown doesn't guarantee a usable XAML/UI thread. The
        // XAML owners deliberately retain state rather than releasing it from
        // Windhawk's arbitrary unload thread after framework teardown.
        Wh_Log(L"[Uninit] No taskbar UI dispatch; XAML tree was unavailable");
    }
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    Wh_Log(L"[Settings] Changed");

    StopRetryThread();
    // Stop desktop-change callbacks before rebuilding tray columns. A late
    // callback during settings save can otherwise rebuild the old bar while the
    // UI thread is removing/reinserting columns.
    StopNotificationThread();

    HWND hWnd = tbh::ResolveTaskbarWnd(g_taskbarWnd);
    if (!hWnd) return;

    RunFromWindowThread(hWnd, [](void* parameter) {
        HWND hWnd = static_cast<HWND>(parameter);
        if (!GetTaskbarXamlRoot(hWnd)) {
            Wh_Log(L"[Settings] No live XAML root; deferring reapply");
            return;
        }
        RemoveButtonGrid();
        ApplyAllSettings();
        // Apply the new settings (including the multi-monitor toggle) to any
        // secondary taskbars discovered earlier.
        RefreshSecondaryBars();
    }, hWnd);
}
