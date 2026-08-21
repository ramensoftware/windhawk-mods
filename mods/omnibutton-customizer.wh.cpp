// ==WindhawkMod==
// @id              omnibutton-customizer
// @name            OmniButton Customizer
// @description     Arrange the Windows 11 OmniButton's network, volume, battery and percentage into any layout you write, hide any of them, and recolor them independently
// @version         2.0
// @author          sb4ssman
// @github          https://github.com/sb4ssman
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lversion
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# OmniButton Customizer

A [Windhawk](https://windhawk.net) mod for Windows 11 that takes the native
OmniButton — the network / volume / battery cluster that opens Quick Settings —
and lets you arrange its items into any shape you like, hide the ones you don't
want, and restyle each one independently.

These native icons are compound and a little weird — each is several glyphs
layered on top of one another, each with its own built-in visual origin — so a
mathematically correct grid does not necessarily look like one. `auto` gets the
shape right, but it does not make pleasing arrangements; expect to adjust.
Every example below is a real arrangement string, nudges and all.

![All four items as a 2×2 block on a single-height taskbar](https://raw.githubusercontent.com/sb4ssman/Windhawk-Mod-Lab/main/omnibutton-customizer/assets/single-height-auto.png)
*Straight out of the box: `Arrangement` left at `auto`, which fits the four
native items to the taskbar height and settles on a 2×2 block.*

## Showcase

![The same 2×2 block, written by hand](https://raw.githubusercontent.com/sb4ssman/Windhawk-Mod-Lab/main/omnibutton-customizer/assets/single-height-arranged-2x2.png)
*The same shape written out and optically corrected:*

```text
(network[-6,2] | volume[-2,4]), (battery[1,0] | percent[4,-2])
```

*Two rows joined into a column — the parentheses matter, because `,` binds
tighter than `|`. Every time `auto` runs it logs the arrangement it generated,
so you can paste that line into the field and start nudging from there; the
automatic and hand-written paths are the same field and the same syntax.*

![Network and volume above a centered battery](https://raw.githubusercontent.com/sb4ssman/Windhawk-Mod-Lab/main/omnibutton-customizer/assets/single-height-arranged-2-over-1.png)
*Three items with the percentage left out: two across the top, the battery
centered below. `Short row or column` decides how a ragged last group lines up.*

![The items in a single row](https://raw.githubusercontent.com/sb4ssman/Windhawk-Mod-Lab/main/omnibutton-customizer/assets/single-height-arranged-reverse.png)
*A single row, in an order you choose rather than the native one.*

![A tight two-high stack](https://raw.githubusercontent.com/sb4ssman/Windhawk-Mod-Lab/main/omnibutton-customizer/assets/single-height-compact-stack.png)
*Pulled in close with a negative `Size` → `Item spacing`. That is the setting
that tightens a cluster — horizontal padding only reserves space at the two
outside edges and can never change the distance between items.*

![The four items arranged as a diamond, with the native tooltip showing](https://raw.githubusercontent.com/sb4ssman/Windhawk-Mod-Lab/main/omnibutton-customizer/assets/single-height-diamond-adjusted-with-hover.png)
*A diamond — volume on top, battery below, network and the percentage on the
sides:*

```text
network[4,-2] | volume[2,-4], battery[0,4] | percent[0,-2]
```

*Nesting one stacked pair between two single items is all it takes; no
parentheses are needed here because `,` already binds tighter than `|`. It is
still the native button, so the hover tooltip and the click through to Quick
Settings behave exactly as they always did.*

![A tight cluster with an enlarged battery percentage](https://raw.githubusercontent.com/sb4ssman/Windhawk-Mod-Lab/main/omnibutton-customizer/assets/single-height-stacked-with-percent-emphasis.png)
*The percentage enlarged with `Surface` → `Battery percentage size`, the one
size control the mod offers — because the percentage is the one item that is
really a single piece of text.*

![A recolored battery percentage in a busy tray](https://raw.githubusercontent.com/sb4ssman/Windhawk-Mod-Lab/main/omnibutton-customizer/assets/with-colors.png)
*Per-item color — the battery percentage recolored here. Network, volume,
battery, and the percentage each have their own color and opacity setting, and
an empty color leaves that item exactly as Windows drew it.*

![All four items arranged vertically on a double-height taskbar](https://raw.githubusercontent.com/sb4ssman/Windhawk-Mod-Lab/main/omnibutton-customizer/assets/double-height-arranged-vertical.png)
*All four in a single column on a double-height taskbar, working alongside
several other tray and taskbar mods in a dense two-row tray:*

```text
network[-2,6], volume[0,2], battery[0,0], percent[2,-6]
```

## Features

- Arrange network, volume, battery, and the battery percentage into any grid —
  automatically fitted to your taskbar height, or written out by hand
- Turn any of the four items off individually
- Writes no Windows settings and no registry values — it arranges the taskbar
  and nothing else
- Independent color and opacity per item, plus size and font family on the
  battery percentage, the one item that is really a single piece of text
- Per-item and per-group pixel nudges inside the arrangement expression
- Group padding and offset for positioning the cluster inside the button
- Keeps the native button in its native tray position, so other mods' "before
  OmniButton" anchors still mean what they always did
- No XAML Diagnostics, so it coexists with Windows 11 Taskbar Styler

## Why this starts at 2.0

Version 1.0 was never published — it existed only as a pull request. The 2.0 in
the version field marks the settings contract, not a history of releases: every
mod in this family moved to the same grouped layout — Placement, Content,
Layout, Size, Adjust, Surface — and to the shared **Arrangement** expression
that replaced each mod's homegrown grid settings. This mod arrived at that
contract second, so its first published version is the one that has it.

**If you installed 1.x by hand from the pull request**, Windhawk cannot carry
values across renamed keys, so your previous customizations are not migrated —
re-apply them once.

`itemOrder` and the whole grid-mode family are gone, replaced by a single
**Arrangement** field. Grid mode, smart layout, fixed rows and columns, slot
width and height, the coupled/independent battery mode, and all eight per-item
nudge settings no longer exist; what replaced each of them is below.

Battery and percentage are now always two independent arrangement items. The
old coupled mode is not a mode any more — write them next to each other in the
arrangement and you have it, with the freedom to put them anywhere instead.

## The Arrangement field

`Layout` → `Arrangement` decides how the items are placed, and it is the only
field that does. Its default value is the word `auto`:

- **`auto`** fits the available items to the taskbar height. `Fill order`
  chooses whether they fill across rows or down columns; `Short row or column`
  aligns a ragged last group. The shape is worked out for you: the mod takes
  the narrowest grid that fits the height, preferring the one that wastes the
  fewest slots — four items on a standard taskbar become a 2×2 block, not a
  lopsided 3+1.
- **Anything else** is an arrangement you write. Names sit side by side with
  `|` and stack with `,`, and parentheses group them:

  ```text
  network, battery | volume, percent     a 2x2 block
  network | volume | battery | percent   a single row
  network, volume, battery, percent      a single column
  network | volume | (battery, percent)  battery stacked over its percentage
  network | (volume, battery) | percent  a diamond
  ```

  The tokens are `network`, `volume`, `battery`, and `percent`, and they are
  case-insensitive. `network` is the one native slot whose glyph changes
  between Wi-Fi, Ethernet, disconnected, airplane-mode, and VPN states. A
  separator is always required — `network (volume | battery)` is an error,
  not a shorthand for `network | (volume | battery)`.

**Omitting a token hides that item**, exactly like turning it off in `Content`.
Items Windows isn't showing at all — the battery on a desktop PC, for one — are
skipped silently whether you name them or not.

Every time the layout is applied, the arrangement `auto` produced is written to
the Windhawk log. Copy that line into the Arrangement field and you have the
automatic layout as a starting point to edit — the automatic and manual paths
are the same field and the same syntax. If what you write doesn't parse, the
log says what was expected and where, and the automatic arrangement is used
until you fix it.

**Nudging.** Append a pixel offset to any name to move just that item:

```text
network[+2,-1] | volume | battery   network moves 2px right and 1px up
(battery, percent)[3,0] | network   the stacked pair moves 3px right
```

Offsets are cosmetic. Nothing else shifts, and the group's overall size does
not change. To move the whole cluster instead, use `Adjust` → horizontal and
vertical offset. These replace the eight per-item nudge settings that 1.x had.

**The percentage arriving late.** An arrangement you write names the items that
existed when you wrote it. Turn the battery percentage on afterwards and it is
in no group, so by default it is appended after your arrangement rather than
vanishing — the log says when that happened, so you can fold it in when you
next edit. Set `Layout` → `Items your arrangement does not name` to *Leave it
out* if you would rather your arrangement be the whole truth. `auto` always
includes every enabled item.

## The battery percentage

**Whether the percentage exists is Windows' decision, not this mod's.** Turn it
on or off in **Settings → System → Power & battery → Battery percentage**. This
mod does not write that setting, or any other Windows setting.

`Content` → `Battery percentage` hides the percentage from the arrangement,
exactly like the three toggles above it hide their own items. All four mean the
same thing, and none of them reaches outside the taskbar. If Windows isn't
showing the percentage, there is nothing here to arrange or hide and the toggle
does nothing.

*An earlier version did drive the Windows setting. It was removed: even with
the correct registry value and a change broadcast, Explorer only sometimes
re-read it and the Settings page never refreshed, so the control worked once
and then appeared dead. A switch that behaves that way is worse than no switch.*

Because the percentage genuinely appears and disappears in the native tree,
the mod watches for it and re-applies the arrangement when it shows up or goes
away, rather than leaving a briefly unstyled percentage sitting in the cluster.

**It is text, so it does not use `Item width`.** "9%", "80%", and "100%" are
three different widths, and a font change moves them again. The percentage's
cell is measured from the text it actually contains — never narrower than
`Item width`, wider when it needs to be — so it cannot be clipped at the edge
of the group. If the value grows past the cell that was reserved for it, the
layout is re-applied at the new width. Every other item is a glyph and does
use `Item width`.

## Settings

### Placement

| Setting | Default | Description |
|---------|---------|-------------|
| Placement: not available in this mod | — | A note, not a control. The OmniButton stays in its native tray position; editing the box does nothing |

### Content

| Setting | Default | Description |
|---------|---------|-------------|
| Network | On | Wi-Fi, Ethernet, disconnected, airplane-mode, and VPN states share this one native slot |
| Volume | On | |
| Battery | On | Absent on machines without a battery |
| Battery percentage | On | Hides it from the arrangement. Windows decides whether it exists — Settings → System → Power & battery |

### Layout

| Setting | Default | Description |
|---------|---------|-------------|
| Arrangement | `auto` | `auto`, or an arrangement you write — see above |
| Fill order | Fill rows first | Used by `auto` |
| Short row or column | Center | Used by `auto`; start, center, or end |
| Items your arrangement does not name | Add it after | Or leave it out; only applies to a written arrangement |

### Size

| Setting | Default | Description |
|---------|---------|-------------|
| Item width | 0 (fit) | 0 reserves exactly the width each item needs; a number puts every item in a fixed box of that width. Clamped to 0–80 |
| Item height | 24 px | Clamped to 16–80 |
| Item spacing | 0 px | Gap between items along each axis; negative pulls them together. Clamped to −16–40 |

**How to make the cluster tight.** Two settings change the space *between* the
icons, and horizontal padding is not one of them:

- **`Item width`** is the box each glyph is centered in. A native tray glyph is
  about 16px wide, so a 32px box is 16px of dead space per item — barely
  noticeable in a 2×2 block, and half the button in a single row. `0` sizes
  every item to its own content, which is why it is the default.
- **`Item spacing`** is the gap between those boxes, and it goes negative if you
  want them closer than touching.
- **`Horizontal padding`** is *outside* the whole group. It cannot change the
  distance between two items, and no value of it ever will.

If the button looks far bigger than the icons in it, `Item width` is almost
always the reason.

### Adjust

| Setting | Default | Description |
|---------|---------|-------------|
| Horizontal padding | 4 px | Reserved on both sides of the group; clamped to 0–24. **Not cosmetic — see below** |
| Vertical padding | 0 px | Reserved above and below the group; clamped to 0–24 |
| Horizontal offset | 0 px | Moves the group; reserves no space; clamped to ±40 |
| Vertical offset | 0 px | Moves the group up (negative) or down (positive); clamped to ±40 |

**Why horizontal padding defaults to 4 and not 0.** The mod zeroes the
OmniButton's own padding so the arrangement owns the entire content area — and
the button has rounded corners. An item arranged flush against that edge has
its last pixels shaved by the curve, which is exactly what used to clip the
"%" off the battery percentage. Those few pixels are the group's breathing
room, not decoration. Setting it to 0 is supported, but expect edge items to
touch the button's rounded border.

It is *outer* padding: it reserves space at the two ends of the group and can
never change the gap between two items. `Item width` and `Item spacing` do
that.

### Surface

| Setting | Default | Description |
|---------|---------|-------------|
| Network / Volume / Battery / Battery percentage color | *(native)* | Empty preserves the native color |
| Network / Volume / Battery / Battery percentage opacity | -1 | -1 is the native opacity; otherwise 0–100% |
| Battery percentage size | 0 pt | 0 is the native size; clamped to 0–64 |
| Battery percentage font family | *(native)* | Empty preserves the native font |

**Why only the percentage has a size and a font.** Because only the percentage
is a single piece of text. Each of the other three is a **stack of glyphs
layered exactly on top of one another** — network and volume are three deep
(Windows calls them Underlay, Base, and AccentOverlay), the battery is two, an
outline and a fill. That stacking is how one icon shows signal strength, a mute
slash, or a charge level.

Resize or re-font one layer of a stack and it stops coinciding with the others:
you get a larger glyph ghosting over the original rather than a bigger icon. So
those two controls are not offered for items that are stacked, and the mod
works out which is which by counting the glyphs rather than assuming.

Color and opacity work on all four. Color is applied where every layer inherits
it, so a stack recolors as one — except the battery, whose layers have no
shared parent to write to; there the outline recolors reliably and the fill only
on some Windows builds. The log says what it found. Opacity applies to the whole
item rather than to any glyph inside it, so it is always safe.

All color settings accept `#RRGGBB` or `#AARRGGBB` hex (the alpha byte is
honored), the generics `accent`, `accentLight`, and `accentDark` for the
Windows accent shades, or `transparent` for a fully transparent glyph — nothing
drawn, the item still present and clickable. Leaving a color empty keeps the
native color.

## Other taskbar mods

The mod deliberately does not move the native `ControlCenterButton` across tray
columns. Keeping it where Windows put it is what lets other mods' semantic
anchors — "before OmniButton", "before clock" — keep their established meaning.
Moving it would need a shared placement lease so two mods couldn't claim
contradictory anchor order, which is why the Placement group is a note rather
than a control.

## Other taskbar positions

**[Taskbar on top](https://windhawk.net/mods/taskbar-on-top) — supported.**
Everything here is positioned relative to the taskbar's own layout, never to
screen coordinates, so a taskbar at the top is the same arrangement in a
different place.

**[Vertical Taskbar](https://windhawk.net/mods/taskbar-vertical) — not
compatible, and this mod stands down when it detects one.** Both mods reach the
same OmniButton elements and both position them by writing `RenderTransform`:
that mod rotates them, this one moves them into a grid. One property, two
owners — there is no arrangement in which both are correct. Rather than fight
over it and paint something broken, this mod detects a taskbar that runs down a
side, leaves the native OmniButton completely untouched, and says so in the
Windhawk log. Turn the vertical mod off and this one resumes on the next
Explorer restart.

## Taskbar Styler

Does not use XAML Diagnostics, so it is compatible with Windows 11 Taskbar
Styler. The mod leases the native elements' dependency properties and restores
each one's exact prior local value when it unloads.

## Known limitations

- The items may not appear arranged until the mod injects on the first tray
  icon load; the retry loop runs up to 5 times at 2-second intervals
- A glyph's color, size, and font wait for its XAML template to expand. That
  normally happens within a few layout passes; if an item's template never
  produces a text glyph, the log says so and the item keeps its native
  appearance
- Turning the battery percentage on or off in Windows Settings sometimes needs
  the next Explorer start before the taskbar reflects it. That is Windows, not
  this mod — the arrangement follows whatever ends up on screen
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- Placement:
  - Status: "Fixed - native position"
    $name: "Placement: not available in this mod"
    $description: >-
      A note, not a control - this box does nothing and any edit to it is
      ignored. The OmniButton stays in its native system-tray position, which
      is what lets other taskbar mods keep using it as their "before
      OmniButton" anchor. Placement controls may arrive later with the shared
      taskbar-arrangement system.
  $name: Placement

- Content:
  - Network: true
    $name: Network
    $description: >-
      The one native network slot. Windows changes its glyph between Wi-Fi,
      Ethernet, disconnected, airplane-mode and VPN states; those are states
      of this item, not separate arrangement items.
  - Volume: true
    $name: Volume
  - Battery: true
    $name: Battery
  - Percent: true
    $name: Battery percentage
    $description: >-
      Hides the percentage from the arrangement, exactly like the three
      toggles above hide their items. It does NOT change any Windows setting.
      Whether the percentage exists at all is decided in Settings > System >
      Power & battery > Battery percentage - if Windows is not showing it,
      there is nothing here to arrange or hide, and this toggle does nothing.
      The two switches are independent: turn it on in Windows, then use this
      one to decide whether the arrangement includes it.
  $name: Content

- Layout:
  - Arrangement: "auto"
    $name: Arrangement
    $description: >-
      "auto" fits the available native items to the taskbar height. Anything
      else is an explicit layout: names side by side with "|", stacked with
      ",", and grouped with parentheses - "network, battery | volume, percent"
      is a 2x2 block. Tokens are network, volume, battery, and percent. Omit a
      token to hide that item. Append a pixel offset to nudge one item,
      "network[+2,-1]", or a whole group, "(network, volume)[3,0]". Every time
      "auto" is applied, its generated arrangement is written to the Windhawk
      log so you can paste it here and edit it. A parse error is logged and
      falls back to automatic.
  - FillOrder: "rows"
    $name: Fill order
    $description: Used by "auto". Whether items fill across rows or down columns first.
    $options:
    - "rows": "Fill rows first (left to right, then down)"
    - "columns": "Fill columns first (top to bottom, then right)"
  - Justify: "center"
    $name: Short row or column
    $description: Used by "auto". How a ragged last row or column is aligned.
    $options:
    - "start": "Start"
    - "center": "Center"
    - "end": "End"
  - NewItems: "append"
    $name: Items your arrangement does not name
    $description: >-
      In practice this means the battery percentage: turning it on above adds
      an item that an arrangement you wrote earlier cannot have named. Only
      applies to a written arrangement - "auto" always includes every enabled
      item Windows is currently showing.
    $options:
    - "append": "Add it after the arrangement"
    - "ignore": "Leave it out until I add it"
  $name: Layout

- Size:
  - ItemWidth: 0
    $name: Item width (px, 0 = fit each item)
    $description: >-
      0 measures every item and reserves exactly the width its own content
      needs, which is what keeps the button tight around the icons. A number
      puts every item in a fixed box of that width instead - handy for lining
      up columns, but a 32px box around a 16px glyph is 16px of dead space per
      item, and in a single row that is most of the button. The battery
      percentage is always measured and treats this as a floor.
  - ItemHeight: 24
    $name: Item height (px)
  - ItemSpacing: 0
    $name: Item spacing (px)
    $description: >-
      Gap between items along each axis. Negative pulls them together and may
      overlap. THIS is what tightens the cluster - horizontal padding only
      reserves space at the two outside edges and can never change the
      distance between items.
  $name: Size

- Adjust:
  - PadX: 4
    $name: Horizontal padding (px)
    $description: >-
      Space reserved on both sides of the native item group. This is not
      cosmetic: the mod zeroes the OmniButton's own padding so the arrangement
      owns the whole content area, and the button has rounded corners, so an
      item sitting flush against the edge gets its last pixels shaved by the
      curve. A few pixels here is what keeps the battery percentage whole.
  - PadY: 0
    $name: Vertical padding (px)
    $description: Space reserved above and below the native item group.
  - OffsetX: 0
    $name: Horizontal offset (px)
    $description: Moves the whole group. Does not reserve space.
  - OffsetY: 0
    $name: Vertical offset (px)
    $description: Moves the whole group up (negative) or down (positive).
  $name: Adjust

- Surface:
  - NetworkColor: ""
    $name: Network icon color
    $description: >-
      Hex (#RRGGBB or #AARRGGBB), accent / accentLight / accentDark, or
      transparent. Empty preserves the native color.
  - VolumeColor: ""
    $name: Volume icon color
    $description: Hex, accent / accentLight / accentDark, or transparent. Empty preserves the native color.
  - BatteryColor: ""
    $name: Battery icon color
    $description: >-
      Hex, accent / accentLight / accentDark, or transparent. Empty preserves
      the native color. The battery is two glyphs layered on top of each other,
      an outline and a fill, so it has no size or font-family setting -
      resizing one of the pair would pull them apart. Recoloring reaches the
      outline reliably and the fill only on some Windows builds; the log says
      what it found.
  - PercentColor: ""
    $name: Battery percentage color
    $description: Hex, accent / accentLight / accentDark, or transparent. Empty preserves the native color.
  - PercentSize: 0
    $name: Battery percentage size (pt, 0 = native)
    $description: >-
      Only the percentage has a size and a font, because only the percentage
      is a single piece of text. Network, volume and the battery are each drawn
      by a STACK of glyphs layered exactly on top of one another - that is how
      they show signal strength, a mute slash or a charge level - and resizing
      one layer of a stack pulls it away from the others.
  - PercentFontFamily: ""
    $name: Battery percentage font family
    $description: Empty preserves the native font.
  - NetworkOpacity: -1
    $name: Network opacity (-1 = native, 0-100%)
  - VolumeOpacity: -1
    $name: Volume opacity (-1 = native, 0-100%)
  - BatteryOpacity: -1
    $name: Battery opacity (-1 = native, 0-100%)
  - PercentOpacity: -1
    $name: Battery percentage opacity (-1 = native, 0-100%)
  $name: Surface
*/
// ==/WindhawkModSettings==

#include <algorithm>
#include <atomic>
#include <cmath>
#include <exception>
#include <functional>
#include <limits>
#include <list>
#include <optional>
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
#include <winrt/Windows.UI.Xaml.Shapes.h>

using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Controls;
using namespace winrt::Windows::UI::Xaml::Media;
using winrt::Windows::UI::Color;
using winrt::Windows::Foundation::IInspectable;

// ============================================================
// Nested group layout
// Template block: _templates/nested-group-layout.h v2.5 (verbatim copy —
// keep in sync with the template; Windhawk mods are single-file).
// ============================================================

// Copy-source template v2.0: nested group layout — pixel-space placement of
// named items described by ONE layout expression. No Windows or WinRT
// dependency; the caller supplies pixel sizes and the taskbar metrics.
//
// This is the only element arranger in the mod family. It backs a single
// user-facing setting, `Layout.Arrangement`, whose default value is the word
// `auto`:
//
//   auto            -> ChooseShape() picks rows x columns from the available
//                      taskbar height, BuildGridExpression() emits the
//                      equivalent expression, and the mod logs it.
//   anything else   -> that string IS the layout.
//
// Because the Windhawk settings API is read-only (windhawk_api.h has no
// setter), a mod can never fill the field in for the user. Logging the
// expansion is the supported path: the user pastes the logged expression back
// into the same field to take manual control. There is one field and one
// string, so nothing can drift out of sync with anything else.
//
// Grammar:
//   expr  := stack ('|' stack)*      '|' places groups side by side
//   stack := unit (',' unit)*        ',' stacks units top to bottom
//   unit  := leaf | '(' expr ')'     parens nest, orientation never flips
//   leaf  := token ('[' dx ',' dy ']')?
//
// "1, 2 | 3, 4" is a 2x2 block. "a | b, c | d" is three columns with b over c
// (the diamond). Nesting is arbitrary: "a | (b, (c | d)), e | f". '|' always
// means horizontal and ',' always means vertical, at every depth — there is no
// primary-axis setting to reason about.
//
// OFFSETS ride in the expression: "1[+2,-1] | 2 | 3" shifts item 1 two pixels
// right and one up. A parenthesized group takes one too — "(1, 2)[3,0] | 3"
// moves that whole column. Offsets are cosmetic: they move their own leaf or
// their own group's contents, and change neither the measured size nor any
// neighbor's position. This replaces every keyed per-item offset setting;
// there is no second string to maintain.
//
// A separator is always required: "1 (2 | 3)" is a parse error, not an
// implicit "1 | (2 | 3)". Silently reinterpreting a missing separator would
// turn a typo into a different layout instead of a logged, recoverable error.
//
// Tokens are caller-defined names resolved to pixel sizes by a callback. A
// token that resolves to an empty size (width or height <= 0) is skipped and
// consumes no space, so absent items collapse out of the arrangement.
//
// PADDING vs OFFSET. Config.padX / padY are symmetric outer padding: they
// participate in layout and are included in the returned totalSize. Group
// offset (Adjust.OffsetX / OffsetY) is a visual translation that must NOT
// reserve space, so it is deliberately not handled here — the mod applies it
// to the container it places, after this arranger has sized the group.
//
// Every group is centered on its cross axis by Config.justify.

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

    Node ParseExpr() {
        Node node;
        node.axis = Axis::Horizontal;
        node.children.push_back(ParseStack());
        while (Peek() == L'|') {
            ++position_;
            node.children.push_back(ParseStack());
        }
        return node;
    }

    Node ParseStack() {
        Node node;
        node.axis = Axis::Vertical;
        node.children.push_back(ParseUnit());
        while (Peek() == L',') {
            ++position_;
            node.children.push_back(ParseUnit());
        }
        return node;
    }

    Node ParseUnit() {
        SkipSpace();
        if (position_ < text_.size() && text_[position_] == L'(') {
            ++position_;
            Node inner = ParseExpr();
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

inline Size Measure(Node const& node, Config const& config,
                    SizeResolver const& resolve) {
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
            if (Measure(child, config, resolve).Empty())
                continue;
            only = &child;
            if (++visible > 1)
                break;
        }
        if (visible == 1)
            return Measure(*only, config, resolve);
    }

    double main = 0.0;
    double cross = 0.0;
    double fillFallback = 0.0;
    int placed = 0;
    for (auto const& child : node.children) {
        Size size = Measure(child, config, resolve);
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

inline void Arrange(Node const& node, Config const& config,
                    SizeResolver const& resolve, double x, double y,
                    std::vector<Placement>& out,
                    Size const* resolvedSize = nullptr) {
    if (!node.token.empty()) {
        Size size = resolvedSize ? *resolvedSize : resolve(node.token);
        if (!size.Empty())
            out.push_back(
                {node.token, x + node.offset.x, y + node.offset.y, size});
        return;
    }

    Size total = Measure(node, config, resolve);
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
            if (Measure(child, config, resolve).Empty())
                continue;
            only = &child;
            if (++visible > 1)
                break;
        }
        if (visible == 1) {
            Arrange(*only, config, resolve, x, y, out, resolvedSize);
            return;
        }
    }

    double cursor = node.axis == Axis::Horizontal ? x : y;
    for (auto const& child : node.children) {
        Size measured = Measure(child, config, resolve);
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
            Arrange(child, config, resolve, cursor, y + crossOffset, out,
                    &size);
            cursor += size.width + config.spacing;
        } else {
            Arrange(child, config, resolve, x + crossOffset, cursor, out,
                    &size);
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
    Size inner = Measure(root, config, resolve);
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
    Arrange(root, config, resolve, config.padX, config.padY, placements,
            &inner);
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
// Native glyph surface
// Template block: _templates/native-glyph-surface.h v1.1 (verbatim copy —
// keep in sync with the template; Windhawk mods are single-file).
// ============================================================
namespace windhawk_mod_templates::native_glyph_surface {

using winrt::Windows::UI::Xaml::DependencyObject;
using winrt::Windows::UI::Xaml::DependencyProperty;
using winrt::Windows::UI::Xaml::FrameworkElement;
using winrt::Windows::UI::Xaml::UIElement;
using winrt::Windows::UI::Xaml::Controls::Control;
using winrt::Windows::UI::Xaml::Controls::TextBlock;
using winrt::Windows::UI::Xaml::Media::Brush;
using winrt::Windows::UI::Xaml::Media::VisualTreeHelper;
using winrt::Windows::UI::Xaml::Shapes::Shape;

// How the native item draws itself, and therefore what can be changed.
enum class Kind {
    None,       // nothing stylable was found under the host
    TextGlyph,  // one or more TextBlocks. Color always applies; size and font
                // only when there is a SINGLE glyph — see Capabilities
    Shapes,     // Path/Rectangle/Ellipse drawing: only color applies, via
                // Fill and Stroke — there is no font to size or replace
    Opaque,     // something we can position and fade, but not recolor
};

inline wchar_t const* KindName(Kind kind) {
    switch (kind) {
        case Kind::TextGlyph: return L"text glyph";
        case Kind::Shapes:    return L"shapes";
        case Kind::Opaque:    return L"opaque";
        default:              return L"nothing";
    }
}

// What a probed surface will actually honor. Drive the settings UI from this
// rather than offering every control for every item.
struct Capabilities {
    bool color = false;
    bool fontSize = false;
    bool fontFamily = false;
    bool opacity = false;  // true whenever there is a host element at all
};

struct Surface {
    Kind kind = Kind::None;
    FrameworkElement host{nullptr};  // owns layout, position, and opacity
    TextBlock text{nullptr};         // set iff kind == TextGlyph
    Control anchor{nullptr};         // templated parent that owns text props
    std::vector<Shape> shapes;       // set iff kind == Shapes
    int glyphLayers = 0;             // stacked TextBlocks drawing ONE icon
    bool textWasUnnamed = false;     // matched by fallback, not by identity
    std::wstring detail;             // one line, for the mod's log

    explicit operator bool() const { return kind != Kind::None; }

    Capabilities Supports() const {
        Capabilities capabilities;
        capabilities.opacity = host != nullptr;
        switch (kind) {
            case Kind::TextGlyph:
                // Colour is safe on a stack: it goes to the ANCHOR, which
                // every layer inherits from, so they all move together.
                capabilities.color = true;
                // SIZE AND FONT ARE NOT.
                //
                // A native tray icon is frequently drawn by SEVERAL TextBlocks
                // stacked on top of each other, each holding one glyph of a
                // composite. Windows 11's wifi and volume are three deep —
                // AdaptiveTextBlocks named Underlay, Base and AccentOverlay —
                // and the battery is two, an outline and a fill. That is how
                // an icon shows signal strength, a mute slash, or a charge
                // level at all.
                //
                // Resizing or re-fonting such a stack pulls the layers apart:
                // they are only one icon because they are exactly registered
                // on top of each other. Verified 2026-07-26 — setting a glyph
                // size on wifi produced a visible GHOST, a large glyph over
                // the original, because the layers stopped coinciding.
                //
                // Probe() finds the FIRST matching TextBlock, so without this
                // count a three-layer icon reports itself as a single glyph
                // and the mod offers two controls that can only damage it.
                capabilities.fontSize = glyphLayers <= 1;
                capabilities.fontFamily = glyphLayers <= 1;
                break;
            case Kind::Shapes:
                capabilities.color = true;
                break;
            default:
                break;
        }
        return capabilities;
    }
};

// The standard Windows 11 tray glyph element. Named, so this is an identity
// match rather than a guess.
inline constexpr wchar_t const* kInnerTextBlock = L"InnerTextBlock";

// The element that OWNS a glyph's text properties, when that is not the glyph.
//
// SystemTray.IconView's `InnerTextBlock` is TEMPLATE-BOUND: its Foreground and
// FontSize come from its templated parent, not from itself. Writing a local
// value onto the TextBlock is one level too deep — the template re-asserts the
// binding and the write disappears, silently and permanently. The designed way
// to restyle such a glyph is to set the property on the templated parent and
// let it flow down.
//
// VERIFIED 2026-07-26 on the Windows 11 tray: with identical code, the battery
// and its percentage accepted colour and font size while wifi and volume
// accepted neither. Opacity worked on all four, because opacity is applied to
// the outer host, which no template owns. The dividing line was exactly
// "inside a SystemTray.IconView or not".
//
// Found by walking UP from the leaf: the parent chain is unambiguous, whereas
// a downward search would have to guess which of several Controls is the
// templated parent. Returns the OUTERMOST Control strictly between leaf and
// host, which is the IconView rather than some inner presenter.
inline Control FindStyleAnchor(FrameworkElement const& host,
                               DependencyObject const& leaf) {
    if (!host || !leaf) return nullptr;
    Control anchor = nullptr;
    DependencyObject node = leaf;
    for (int depth = 0; depth < 32; ++depth) {
        auto parent = VisualTreeHelper::GetParent(node);
        if (!parent) break;
        if (parent.try_as<FrameworkElement>() == host) break;
        if (auto control = parent.try_as<Control>()) anchor = control;
        node = parent;
    }
    return anchor;
}

inline TextBlock FindNamedTextBlock(DependencyObject const& root,
                                    wchar_t const* name, int maxDepth,
                                    int depth = 0) {
    if (!root || depth > maxDepth) return nullptr;
    if (auto element = root.try_as<FrameworkElement>())
        if (element.Name() == name)
            if (auto text = element.try_as<TextBlock>())
                return text;
    int count = VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < count; ++i) {
        auto child = VisualTreeHelper::GetChild(root, i);
        if (!child) continue;
        if (auto found = FindNamedTextBlock(child, name, maxDepth, depth + 1))
            return found;
    }
    return nullptr;
}

inline TextBlock FindAnyTextBlock(DependencyObject const& root, int maxDepth,
                                  int depth = 0) {
    if (!root || depth > maxDepth) return nullptr;
    if (auto text = root.try_as<TextBlock>()) return text;
    int count = VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < count; ++i) {
        auto child = VisualTreeHelper::GetChild(root, i);
        if (!child) continue;
        if (auto found = FindAnyTextBlock(child, maxDepth, depth + 1))
            return found;
    }
    return nullptr;
}

// How many TextBlocks draw this item. One is a plain glyph; more than one is a
// STACK that must be resized together or not at all — see Capabilities.
inline int CountTextBlocks(DependencyObject const& root, int maxDepth,
                           int depth = 0) {
    if (!root || depth > maxDepth) return 0;
    if (root.try_as<TextBlock>()) return 1;  // leaves; no TextBlock nests one
    int total = 0;
    int count = VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < count; ++i) {
        auto child = VisualTreeHelper::GetChild(root, i);
        if (!child) continue;
        total += CountTextBlocks(child, maxDepth, depth + 1);
    }
    return total;
}

inline void CollectShapes(DependencyObject const& root, int maxDepth,
                          std::vector<Shape>& out, int depth = 0) {
    if (!root || depth > maxDepth) return;
    if (auto shape = root.try_as<Shape>()) out.push_back(shape);
    int count = VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < count; ++i) {
        auto child = VisualTreeHelper::GetChild(root, i);
        if (!child) continue;
        CollectShapes(child, maxDepth, out, depth + 1);
    }
}

// Work out what `host` is made of.
//
// PRECEDENCE MATTERS, and it is not the obvious order:
//
//   1. host is itself a TextBlock            — nothing to search for
//   2. a descendant named InnerTextBlock     — identity, the documented glyph
//   3. any Shape descendants                 — a drawn icon
//   4. any TextBlock at all                  — last resort, flagged as such
//
// Steps 3 and 4 are in this order on purpose. Searching for a bare TextBlock
// before checking for shapes is exactly the bug this template replaces: a
// drawn icon can still have some incidental TextBlock buried under it, and
// binding to that produces settings that appear to work and never do.
//
// A host whose template has not expanded yet legitimately probes to None —
// XAML materializes a ContentPresenter's content after the presenter itself
// appears. Callers must re-probe rather than treat the first None as final.
inline Surface Probe(FrameworkElement const& host, int maxDepth = 12) {
    Surface surface;
    surface.host = host;
    if (!host) {
        surface.detail = L"no host element";
        return surface;
    }

    if (auto text = host.try_as<TextBlock>()) {
        surface.kind = Kind::TextGlyph;
        surface.text = text;
        surface.glyphLayers = 1;
        surface.detail = L"host is itself a TextBlock";
        return surface;
    }

    if (auto text = FindNamedTextBlock(host, kInnerTextBlock, maxDepth)) {
        surface.kind = Kind::TextGlyph;
        surface.text = text;
        surface.anchor = FindStyleAnchor(host, text);
        surface.glyphLayers = CountTextBlocks(host, maxDepth);
        surface.detail =
            surface.glyphLayers > 1
                ? L"a STACK of " + std::to_wstring(surface.glyphLayers) +
                      L" layered glyphs - colour only, no size or font"
                : (surface.anchor ? L"TextBlock named InnerTextBlock, styled "
                                    L"through its templated parent"
                                  : L"TextBlock named InnerTextBlock");
        return surface;
    }

    CollectShapes(host, maxDepth, surface.shapes);
    if (!surface.shapes.empty()) {
        surface.kind = Kind::Shapes;
        surface.detail = L"drawn from " +
                         std::to_wstring(surface.shapes.size()) +
                         L" shape(s) - no font to size or replace";
        return surface;
    }

    if (auto text = FindAnyTextBlock(host, maxDepth)) {
        surface.kind = Kind::TextGlyph;
        surface.text = text;
        surface.anchor = FindStyleAnchor(host, text);
        surface.glyphLayers = CountTextBlocks(host, maxDepth);
        surface.textWasUnnamed = true;
        surface.detail =
            surface.glyphLayers > 1
                ? L"a STACK of " + std::to_wstring(surface.glyphLayers) +
                      L" unnamed TextBlocks - colour only, no size or font"
                : L"unnamed TextBlock (fallback - verify it is really what "
                  L"draws this item)";
        return surface;
    }

    surface.kind = host ? Kind::Opaque : Kind::None;
    surface.detail = L"no text or shapes found; opacity and position only";
    return surface;
}

// The mod's property-lease hook. Every mutation below is announced through
// this before it happens, so the mod can snapshot the prior local value.
using TrackFn =
    std::function<void(DependencyObject const&, DependencyProperty const&)>;

// WRITE THE ANCHOR FIRST, THEN THE LEAF. When the glyph is template-bound the
// anchor is the only write that survives; when it is not, the leaf's local
// value wins and the anchor's is harmlessly inherited past. Both are leased,
// so the restore is unaffected either way, and one code path covers both
// shapes of tray item instead of a per-item special case.
//
// A null brush means "leave the native color alone" — never a fallback color.
inline bool ApplyColor(Surface const& surface, Brush const& brush,
                       TrackFn const& track) {
    if (!brush || !surface.Supports().color) return false;
    if (surface.kind == Kind::TextGlyph) {
        if (surface.anchor) {
            if (track) track(surface.anchor, Control::ForegroundProperty());
            try {
                surface.anchor.Foreground(brush);
            } catch (...) {
            }
        }
        if (track) track(surface.text, TextBlock::ForegroundProperty());
        try {
            surface.text.Foreground(brush);
        } catch (...) {
            return false;
        }
        return true;
    }

    bool applied = false;
    for (auto const& shape : surface.shapes) {
        // Only repaint what the shape already paints. A shape with no fill is
        // an outline and a shape with no stroke is a solid; overriding the
        // absent one would add a border or a blob that was never there.
        if (shape.Fill()) {
            if (track) track(shape, Shape::FillProperty());
            try {
                shape.Fill(brush);
                applied = true;
            } catch (...) {
            }
        }
        if (shape.Stroke()) {
            if (track) track(shape, Shape::StrokeProperty());
            try {
                shape.Stroke(brush);
                applied = true;
            } catch (...) {
            }
        }
    }
    return applied;
}

// Sizes in points; 0 or less means "keep the native size".
inline bool ApplyFontSize(Surface const& surface, double points,
                          TrackFn const& track) {
    if (points <= 0.0 || !surface.Supports().fontSize) return false;
    if (surface.anchor) {
        if (track) track(surface.anchor, Control::FontSizeProperty());
        try {
            surface.anchor.FontSize(points);
        } catch (...) {
        }
    }
    if (track) track(surface.text, TextBlock::FontSizeProperty());
    try {
        surface.text.FontSize(points);
    } catch (...) {
        return false;
    }
    return true;
}

// An empty family means "keep the native font".
inline bool ApplyFontFamily(Surface const& surface, std::wstring const& family,
                            TrackFn const& track) {
    if (family.empty() || !surface.Supports().fontFamily) return false;
    if (surface.anchor) {
        if (track) track(surface.anchor, Control::FontFamilyProperty());
        try {
            surface.anchor.FontFamily(
                winrt::Windows::UI::Xaml::Media::FontFamily(family));
        } catch (...) {
        }
    }
    if (track) track(surface.text, TextBlock::FontFamilyProperty());
    try {
        surface.text.FontFamily(
            winrt::Windows::UI::Xaml::Media::FontFamily(family));
    } catch (...) {
        return false;
    }
    return true;
}

// Percent, or a negative value to keep the native opacity. Applied to the HOST
// rather than the glyph, so it fades the whole item including any chrome.
inline bool ApplyOpacity(Surface const& surface, int percent,
                         TrackFn const& track) {
    if (percent < 0 || !surface.Supports().opacity) return false;
    if (track) track(surface.host, UIElement::OpacityProperty());
    try {
        surface.host.Opacity((double)percent / 100.0);
    } catch (...) {
        return false;
    }
    return true;
}

// Natural size of a native element with nothing the mod imposed on it.
//
// A TEXT item must be measured, never assumed. "9%", "80%", and "100%" are
// three different widths and a font or locale change moves them again, so one
// Size.ItemWidth cannot describe both a 20x16 drawn icon and a string. Feed
// the result into the layout's SizeResolver so the arrangement RESERVES the
// width, rather than discovering the overflow at paint time and clipping it.
inline winrt::Windows::Foundation::Size MeasureNatural(
    FrameworkElement const& element, TrackFn const& track) {
    if (!element) return {};
    try {
        if (track) {
            track(element, FrameworkElement::WidthProperty());
            track(element, FrameworkElement::HeightProperty());
        }
        element.Width(std::numeric_limits<double>::quiet_NaN());
        element.Height(std::numeric_limits<double>::quiet_NaN());
        element.Measure({std::numeric_limits<float>::infinity(),
                         std::numeric_limits<float>::infinity()});
        return element.DesiredSize();
    } catch (...) {
        return {};
    }
}

}  // namespace windhawk_mod_templates::native_glyph_surface

namespace ngs = windhawk_mod_templates::native_glyph_surface;

// ============================================================
// Property lease
// Template block: _templates/property-lease.h v1.0 (verbatim copy —
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

// ============================================================
// Settings IO
// Template block: _templates/settings-io.h v1.0 (verbatim copy —
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
// Color tokens
// Template block: _templates/color-tokens.h v1.0 (verbatim copy —
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
// Taskbar host
// Template block: _templates/taskbar-host.h v1.0 (verbatim copy —
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
               DWORD intervalMs = 2000) {
        Stop();
        if (unloading) return;
        attempt_ = attempt;
        applied_ = applied;
        unloading_ = &unloading;
        attempts_ = attempts;
        intervalMs_ = intervalMs;
        stopEvent_ = CreateEventW(nullptr, TRUE, FALSE, nullptr);
        if (!stopEvent_) return;
        thread_ = CreateThread(
            nullptr, 0,
            [](void* parameter) -> DWORD {
                auto* self = static_cast<RetryLoop*>(parameter);
                for (int i = 0; i < self->attempts_ && !*self->unloading_;
                     ++i) {
                    if (self->applied_ && self->applied_()) break;
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
};

}  // namespace windhawk_mod_templates::taskbar_host

namespace tbh = windhawk_mod_templates::taskbar_host;

// ============================================================
// Visual tree walk
// Template block: _templates/visual-tree-walk.h v1.0 (verbatim copy —
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

} // namespace windhawk_mod_templates::visual_tree_walk

namespace vtw = windhawk_mod_templates::visual_tree_walk;

// ── Settings ───────────────────────────────────────────────────────────────

struct ModSettings {
    // Content
    bool network;
    bool volume;
    bool battery;
    bool percent;
    // Layout
    wchar_t arrangement[512];
    ngl::FillOrder fillOrder;
    ngl::Justify justify;
    bool appendNewItems;
    // Size
    int itemWidth;
    int itemHeight;
    int itemSpacing;
    // Adjust
    int padX;
    int padY;
    int offsetX;
    int offsetY;
    // Surface
    wchar_t networkColor[32];
    wchar_t volumeColor[32];
    wchar_t batteryColor[32];
    wchar_t percentColor[32];
    // Only the percentage has these. Network, volume and the battery are each
    // drawn by a STACK of layered glyphs, which cannot be resized or
    // re-fonted without pulling the layers apart — see the Surface group.
    int percentSize;
    wchar_t percentFontFamily[64];
    int networkOpacity;
    int volumeOpacity;
    int batteryOpacity;
    int percentOpacity;
};

static ModSettings g_settings{};  // exit-time-safe: heap-only

std::atomic<bool> g_unloading = false;

static void LoadSettings() {
    // Content
    g_settings.network = sio::LoadBool(L"Content.Network");
    g_settings.volume = sio::LoadBool(L"Content.Volume");
    g_settings.battery = sio::LoadBool(L"Content.Battery");
    g_settings.percent = sio::LoadBool(L"Content.Percent");

    // Layout. Table-driven rather than a chain of _wcsicmp: after any option
    // is renamed a stale literal fails silently and the mod quietly falls back.
    sio::LoadString(L"Layout.Arrangement", g_settings.arrangement, L"auto");

    static constexpr sio::Choice<ngl::FillOrder> kFillOrders[] = {
        {L"rows", ngl::FillOrder::Rows},
        {L"columns", ngl::FillOrder::Columns},
    };
    g_settings.fillOrder =
        sio::LoadChoice(L"Layout.FillOrder", kFillOrders, ngl::FillOrder::Rows);

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
    g_settings.appendNewItems =
        sio::LoadChoice(L"Layout.NewItems", kNewItemPolicies, true);

    // Size
    // 0 is meaningful, not a floor to clamp away: it means "fit each item to
    // its own content". Spacing goes negative so a row can be pulled tighter
    // than its cells; the arranger handles a negative gap natively.
    g_settings.itemWidth = sio::LoadInt(L"Size.ItemWidth", 0, 80);
    g_settings.itemHeight = sio::LoadInt(L"Size.ItemHeight", 16, 80);
    g_settings.itemSpacing = sio::LoadInt(L"Size.ItemSpacing", -16, 40);

    // Adjust
    g_settings.padX = sio::LoadInt(L"Adjust.PadX", 0, 24);
    g_settings.padY = sio::LoadInt(L"Adjust.PadY", 0, 24);
    g_settings.offsetX = sio::LoadInt(L"Adjust.OffsetX", -40, 40);
    g_settings.offsetY = sio::LoadInt(L"Adjust.OffsetY", -40, 40);

    // Surface
    sio::LoadString(L"Surface.NetworkColor", g_settings.networkColor);
    sio::LoadString(L"Surface.VolumeColor", g_settings.volumeColor);
    sio::LoadString(L"Surface.BatteryColor", g_settings.batteryColor);
    sio::LoadString(L"Surface.PercentColor", g_settings.percentColor);

    g_settings.percentSize = sio::LoadInt(L"Surface.PercentSize", 0, 64);
    sio::LoadString(L"Surface.PercentFontFamily", g_settings.percentFontFamily);

    g_settings.networkOpacity =
        sio::LoadInt(L"Surface.NetworkOpacity", -1, 100);
    g_settings.volumeOpacity = sio::LoadInt(L"Surface.VolumeOpacity", -1, 100);
    g_settings.batteryOpacity =
        sio::LoadInt(L"Surface.BatteryOpacity", -1, 100);
    g_settings.percentOpacity =
        sio::LoadInt(L"Surface.PercentOpacity", -1, 100);
}

// THIS MOD DOES NOT WRITE ANY WINDOWS SETTING.
//
// `Content.Percent` used to drive the OS battery-percentage setting through
// os-setting-bridge.h. That is gone, and the whole embedded bridge with it.
//
// The feature was correct on paper and unreliable in practice. Even with the
// right registry value and a WM_SETTINGCHANGE broadcast, Explorer only
// sometimes re-read it, the Settings page never live-refreshed, and the result
// was a toggle that appeared to do nothing most of the time. A control that
// works once and then does not is worse than no control.
//
// So Content.Percent now means exactly what Content.Network, Content.Volume and
// Content.Battery mean: "include this native item in the arrangement". All
// four are uniform, none of them reaches outside the taskbar, and there is
// nothing to restore on unload because nothing was ever changed.
//
// Whether the percentage EXISTS at all is Windows' business, decided in
// Settings > System > Power & battery. The mod arranges what Windows shows.

// ── Cached element references ─────────────────────────────────────────────

[[clang::no_destroy]] static StackPanel       g_omniStackPanel{ nullptr };
[[clang::no_destroy]] static FrameworkElement g_omniButton{ nullptr };
[[clang::no_destroy]] static FrameworkElement g_networkPresenter{ nullptr };
[[clang::no_destroy]] static FrameworkElement g_volumePresenter{ nullptr };
[[clang::no_destroy]] static FrameworkElement g_batteryPresenter{ nullptr };
[[clang::no_destroy]] static StackPanel       g_batteryInnerPanel{ nullptr };
[[clang::no_destroy]] static FrameworkElement g_batteryGlyphFE{ nullptr };
[[clang::no_destroy]] static FrameworkElement g_batteryPercentFE{ nullptr };

// Probed once each, then reused. A Surface knows not just WHERE the stylable
// leaf is but WHAT it is, so a setting that cannot apply to a given item is
// dropped rather than silently written to the wrong element.
[[clang::no_destroy]] static ngs::Surface g_networkSurface{};
[[clang::no_destroy]] static ngs::Surface g_volumeSurface{};
[[clang::no_destroy]] static ngs::Surface g_batterySurface{};
[[clang::no_destroy]] static ngs::Surface g_percentSurface{};

[[clang::no_destroy]] static StackPanel g_layoutUpdatedSP{ nullptr };
static winrt::event_token g_layoutUpdatedToken{};

static HWND g_taskbarWnd = nullptr;
static std::atomic<bool> g_applied{false};
static std::atomic<bool> g_reapplyPending{false};
// Stoppable and WAITED during unload: a detached retry thread that outlives
// Wh_ModUninit would run mod code out of an unloaded DLL.
static tbh::RetryLoop g_retryLoop;

[[clang::no_destroy]] static std::optional<std::list<FrameworkElement::Loaded_revoker>>
    g_autoRevokerList{std::in_place};

// Optional-backed so Wh_ModUninit can reset() it on the UI thread and free the
// vector, instead of leaving an exit-time destructor to touch XAML at process
// teardown. See _templates/property-lease.h.
[[clang::no_destroy]] static std::optional<ple::Lease> g_lease{std::in_place};

static void LogCurrentUiException(PCWSTR context) noexcept;

static void TrackProperty(DependencyObject const& object,
                          DependencyProperty const& property) {
    if (g_lease) g_lease->Track(object, property);
}

static void RestorePropertySnapshots() {
    if (!g_lease) return;
    g_lease->RestoreAll(
        [] { Wh_Log(L"[Cleanup] Failed to restore a XAML property"); });
}

// ── Nested layout geometry ────────────────────────────────────────────────
// The native presenters and their lifecycle stay unchanged. This layer only
// translates the one Layout.Arrangement expression into their absolute
// positions inside the existing OmniButton items host.

struct OmniLayout {
    bool visible[4]{};
    bool hasPlacement[4]{};
    ngl::Placement placement[4];
    ngl::Size total;
    std::wstring expression;
    bool wasAuto = false;
};

static int OmniItemIndex(std::wstring const& token) {
    if (ngl::TokenIs(token, L"network")) return 0;
    if (ngl::TokenIs(token, L"volume")) return 1;
    if (ngl::TokenIs(token, L"battery")) return 2;
    if (ngl::TokenIs(token, L"percent")) return 3;
    return -1;
}

static bool SameOmniItem(std::wstring const& placed,
                         std::wstring const& expected) {
    int placedItem = OmniItemIndex(placed);
    return placedItem >= 0 && placedItem == OmniItemIndex(expected);
}

// The battery percentage is TEXT, not a glyph. "9%", "80%", and "100%" are
// three different widths, and a font or locale change moves them again, so the
// one Size.ItemWidth that suits a 20x16 battery icon cannot also describe it.
// Reserving a glyph-sized cell for it is what clipped the percentage at the
// group's right edge on default settings. This holds the width measured from
// the live element; 0 means "not measured, use ItemWidth".
static double g_percentCellWidth = 0.0;

// The widest the percentage has ever been measured at, sticky across the
// element resets that a re-apply performs. Without this the corrected width
// found during arrangement is thrown away by the very re-apply it triggers.
// Cleared in LoadSettings, because a font-size change invalidates it.
static double g_percentWidestDesired = 0.0;

// Set when the arranged cell turned out narrower than the percentage really
// needs; consumed once by the LayoutUpdated monitor.
static bool g_percentNeedsWiderCell = false;

// The text the cell was measured for. A TextBlock arranged into a slot smaller
// than its content reports ActualWidth == the slot, so comparing ActualWidth
// against the reserved cell can NEVER detect "needs more room" — it is clamped
// by the very number it is being compared to. Watching the text is the honest
// signal that a re-measure is due.
// Plain buffer, not a std::wstring: a namespace-scope string owns heap and
// would need exit-time teardown for a value that is never longer than "100%".
static wchar_t g_percentMeasuredText[32]{};

static void SetPercentMeasuredText(wchar_t const* text) {
    if (!text) { g_percentMeasuredText[0] = L'\0'; return; }
    wcsncpy(g_percentMeasuredText, text,
            ARRAYSIZE(g_percentMeasuredText) - 1);
    g_percentMeasuredText[ARRAYSIZE(g_percentMeasuredText) - 1] = L'\0';
}

// FIT TO CONTENT, for every item and not just the percentage.
//
// A settings-driven Size.ItemWidth describes a BOX to centre a glyph in, and
// the native tray glyphs are about 16px wide. At the old default of 32 that is
// 16px of dead space per item, contributed by the arrangement itself and not
// by any padding. Stacked two-by-two it reads as generous spacing; strung out
// in a single row it is half the button, which is exactly the "why is the
// OmniButton's area so enormous around its own icons" complaint. No amount of
// Adjust.PadX can touch it — padding is outside the group, this is inside it.
//
// Sticky widest-seen, because the first measure can land before an item's XAML
// template has expanded and honestly report 0. A glyph's natural width does
// not drift once it is real, so this settles on the first true measurement.
// Cleared in Wh_ModSettingsChanged, since a glyph-size or font change moves it.
static double g_itemContentWidth[4]{};

// Used while fit-to-content is on and nothing has measured yet, so a group is
// never arranged at zero width and then re-applied from an invisible state.
static constexpr double kUnmeasuredCellWidth = 24.0;

// Set when fit-to-content had to fall back for at least one item; consumed
// once by the LayoutUpdated monitor to re-arrange at the real widths.
static bool g_cellsNeedRemeasure = false;

// A template that has not produced a measurable item after this many rebuilds
// is not going to. Bounded so an item that never measures cannot re-apply the
// layout on every layout pass forever.
static constexpr int kMaxRemeasures = 3;
static int g_remeasures = 0;

// Every cell size in one place, so the arranger, the missing-token pass, and
// the per-item centering cannot disagree about how wide an item is.
static ngl::Size OmniItemSize(int item) {
    double width = (double)g_settings.itemWidth;  // 0 = fit to content
    double height = (double)g_settings.itemHeight;

    // The percentage is ALWAYS content-sized: "9%" and "100%" are different
    // widths and no single number describes both. ItemWidth is only its floor,
    // which at 0 means it is sized purely by its text.
    if (item == 3 && g_percentCellWidth > 0.0)
        return ngl::ContentAlong(g_percentCellWidth, width, height);

    if (width <= 0.0) {
        double measured =
            (item >= 0 && item < 4) ? g_itemContentWidth[item] : 0.0;
        return {measured > 0.0 ? measured : kUnmeasuredCellWidth, height};
    }
    return {width, height};
}

static ngl::Config OmniLayoutConfig() {
    ngl::Config config;
    config.spacing = (double)g_settings.itemSpacing;
    config.justify = g_settings.justify;
    config.padX = (double)g_settings.padX;
    config.padY = (double)g_settings.padY;
    return config;
}

static int AvailableOmniRows(HWND hTaskbarWnd) {
    auto metrics = tbh::GetMetrics(hTaskbarWnd);
    if (!metrics.valid) {
        Wh_Log(L"[Layout] No taskbar window - assuming a single row");
        return 1;
    }

    // constrainedDip is already the taskbar's own thickness in DIPs, whichever
    // way it runs — the physical-px/DIP conversion that PR #4855 flagged as
    // blocking now lives in the template rather than here.
    double reserved = 2.0 * (double)g_settings.padY;
    int rows = ngl::RowsInHeight(
        metrics.constrainedDip - reserved, (double)g_settings.itemHeight,
        (double)g_settings.itemSpacing);
    Wh_Log(L"[Layout] %s taskbar, %.0f dip across at %udpi, %.0f reserved "
           L"-> %d row(s) for OmniButton items",
           tbh::OrientationName(metrics.orientation), metrics.constrainedDip,
           metrics.dpi, reserved, rows);
    return rows;
}

static void NormalizeOmniNode(ngl::Node& node, bool const present[4],
                              bool visible[4], bool seen[4]) {
    if (!node.token.empty()) {
        int item = OmniItemIndex(node.token);
        if (item < 0 || !present[item]) {
            node.token = L"__omni_hidden";
            return;
        }

        visible[item] = true;
        bool duplicate = seen[item];
        seen[item] = true;
        if (duplicate)
            node.token = L"__omni_hidden";
        return;
    }

    for (auto& child : node.children)
        NormalizeOmniNode(child, present, visible, seen);
}

static bool ComputeOmniNode(ngl::Node const& root,
                            std::vector<ngl::Placement>& placements,
                            ngl::Size& total) {
    auto resolve = [](std::wstring const& token) -> ngl::Size {
        int item = OmniItemIndex(token);
        return item >= 0 ? OmniItemSize(item) : ngl::Size{};
    };
    ngl::Config config = OmniLayoutConfig();
    ngl::Size inner = ngl::Measure(root, config, resolve);
    placements.clear();
    if (inner.Empty()) {
        total = {};
        return true;
    }
    if (inner.axisRelative) {
        double cross = inner.cross > 0.0 ? inner.cross : inner.thickness;
        inner = {inner.thickness, cross};
    }
    ngl::Arrange(root, config, resolve, config.padX, config.padY,
                 placements, &inner);
    total = {inner.width + config.padX * 2.0,
             inner.height + config.padY * 2.0};
    return true;
}

static OmniLayout ResolveOmniLayout(HWND hTaskbarWnd, bool hasNetwork,
                                    bool hasVolume, bool hasBattery,
                                    bool hasPercent) {
    bool const present[4] = {
        hasNetwork && g_settings.network,
        hasVolume && g_settings.volume,
        hasBattery && g_settings.battery,
        hasPercent && g_settings.percent,
    };

    std::vector<std::wstring> autoTokens;
    std::vector<std::wstring> expectedTokens;
    if (present[0]) {
        autoTokens.push_back(L"network");
        expectedTokens.push_back(L"network");
    }
    if (present[1]) {
        autoTokens.push_back(L"volume");
        expectedTokens.push_back(L"volume");
    }
    if (present[2]) {
        autoTokens.push_back(L"battery");
        expectedTokens.push_back(L"battery");
    }
    if (present[3]) {
        autoTokens.push_back(L"percent");
        expectedTokens.push_back(L"percent");
    }

    int maxRows = AvailableOmniRows(hTaskbarWnd);
    auto namer = [&autoTokens](int index) {
        return autoTokens[index];
    };
    auto arrangement = ngl::ResolveArrangement(
        g_settings.arrangement, (int)autoTokens.size(), maxRows,
        g_settings.fillOrder, namer);

    OmniLayout result;
    result.expression = arrangement.expression;
    result.wasAuto = arrangement.wasAuto;

    ngl::Node root;
    ngl::ParseError error;
    if (!ngl::Parse(result.expression, root, &error)) {
        Wh_Log(L"[Layout] Arrangement \"%ls\" - expected %ls at character "
               L"%d; using the automatic arrangement instead",
               result.expression.c_str(), error.expected.c_str(),
               (int)error.position + 1);
        arrangement = ngl::ResolveArrangement(
            L"auto", (int)autoTokens.size(), maxRows,
            g_settings.fillOrder, namer);
        result.expression = arrangement.expression;
        result.wasAuto = true;
        ngl::Parse(result.expression, root, nullptr);
    }

    if (!result.wasAuto && g_settings.appendNewItems) {
        auto resolveAvailable = [&present](std::wstring const& token)
            -> ngl::Size {
            int item = OmniItemIndex(token);
            return item >= 0 && present[item] ? OmniItemSize(item)
                                              : ngl::Size{};
        };
        std::vector<ngl::Placement> writtenPlacements;
        ngl::Size writtenTotal;
        ngl::Compute(result.expression, OmniLayoutConfig(),
                     resolveAvailable, writtenPlacements, writtenTotal,
                     nullptr);
        auto missing = ngl::MissingTokens(
            expectedTokens, writtenPlacements, SameOmniItem);
        if (!missing.empty()) {
            result.expression = ngl::AppendMissing(
                result.expression, missing, maxRows,
                g_settings.fillOrder);
            ngl::Parse(result.expression, root, nullptr);
            Wh_Log(L"[Layout] %d enabled item(s) missing from your "
                   L"arrangement were added",
                   (int)missing.size());
        }
    }

    bool seen[4]{};
    NormalizeOmniNode(root, present, result.visible, seen);

    std::vector<ngl::Placement> placements;
    ComputeOmniNode(root, placements, result.total);
    for (auto const& placement : placements) {
        int item = OmniItemIndex(placement.token);
        if (item < 0) continue;
        result.placement[item] = placement;
        result.hasPlacement[item] = true;
    }

    Wh_Log(L"[Layout] tokens: network=Network  volume=Volume  battery=Battery  "
           L"percent=Battery percentage");
    Wh_Log(L"[Layout] arrangement = \"%ls\"%ls, items=%d, size %.0fx%.0f, "
           L"visible=[%d,%d,%d,%d]",
           result.expression.c_str(),
           result.wasAuto
               ? L" (auto - paste this into Arrangement to edit it)"
               : L"",
           (int)placements.size(), result.total.width, result.total.height,
           result.visible[0], result.visible[1], result.visible[2],
           result.visible[3]);
    return result;
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

static bool WalkSetupBatteryInnerPanel(DependencyObject const& node, int depth = 0) {
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
        if (WalkSetupBatteryInnerPanel(child, depth + 1)) return true;
    }
    return false;
}

// ── Color ─────────────────────────────────────────────────────────────────

static void LogAccentReadFailure() {
    Wh_Log(L"[Color] Failed to read the Windows accent color");
}

// nullptr means "keep the native color" — for an empty setting, an unknown
// token, and bad hex alike. Never substitute a default color here.
static Brush ParseColorBrush(const wchar_t* value) {
    return clr::ParseBrush(value, LogAccentReadFailure);
}

// The mod owns the property lease; the template announces every mutation
// through this so the exact prior local value is snapshotted first.
// Returned by value: a function-local static std::function would need an
// exit-time destructor, and the lambda is captureless so building one is free.
static ngs::TrackFn GlyphTrack() {
    return [](DependencyObject const& object,
              DependencyProperty const& property) {
        TrackProperty(object, property);
    };
}

// A slot's ContentPresenter appears in the items host before its
// SystemTray.IconView template expands, so the first pass can legitimately
// find a presenter with nothing stylable underneath it yet. Probe lazily and
// report the outcome once per item, so an item that never resolves is visible
// in the log instead of silently going unstyled.
static bool g_glyphMissLogged[4]{};

// Bounds the in-place retop from LayoutUpdated (see OnLayoutUpdatedImpl).
static constexpr int kMaxGlyphTopUps = 60;
static int g_glyphTopUps = 0;

static void ResolveSurface(PCWSTR item, int index,
                           FrameworkElement const& host,
                           ngs::Surface& surface) {
    if (surface || !host) return;
    surface = ngs::Probe(host);
    if (surface) {
        g_glyphMissLogged[index] = false;
        auto capabilities = surface.Supports();
        Wh_Log(L"[Style] %s is %s - %s | color=%d fontSize=%d fontFamily=%d "
               L"opacity=%d", item, ngs::KindName(surface.kind),
               surface.detail.c_str(), (int)capabilities.color,
               (int)capabilities.fontSize, (int)capabilities.fontFamily,
               (int)capabilities.opacity);
        if (surface.anchor)
            Wh_Log(L"[Style] %s style anchor = %s (colour and size are set "
                   L"here as well as on the glyph)", item,
                   winrt::get_class_name(surface.anchor).c_str());
        if (surface.textWasUnnamed)
            Wh_Log(L"[Style] %s matched an UNNAMED TextBlock; if styling it "
                   L"has no visible effect, this item is drawn some other way",
                   item);
        return;
    }
    if (g_glyphMissLogged[index]) return;
    g_glyphMissLogged[index] = true;
    Wh_Log(L"[Style] %s has nothing stylable yet - host class=%s with %d "
           L"child(ren); waiting for the template to expand",
           item, winrt::get_class_name(host).c_str(),
           VisualTreeHelper::GetChildrenCount(host));
}

// True once every native item we actually found has been probed. Treating
// "items host discovered" as success is what left network and volume permanently
// unstyled: g_applied went true on the first pass, and the retry thread, the
// IconView Loaded handler, and LayoutUpdated all stand down on g_applied, so
// nothing ever looked again.
static bool AllGlyphsResolved() {
    if (g_networkPresenter && !g_networkSurface) return false;
    if (g_volumePresenter && !g_volumeSurface) return false;
    if (g_batteryGlyphFE && !g_batterySurface) return false;
    if (g_batteryPercentFE && !g_percentSurface) return false;
    return true;
}

// Size and family are asked for only where they mean something, and the
// TEMPLATE is what decides: a Surface whose icon is a stack of layered glyphs
// reports fontSize/fontFamily false, and these calls become no-ops. Three of
// the four items are such a stack, which is why only the percentage has those
// two settings at all.
static void ApplyItemStyle(ngs::Surface const& surface, const wchar_t* color,
                           int size, const wchar_t* fontFamily, int opacity) {
    auto track = GlyphTrack();
    ngs::ApplyColor(surface, ParseColorBrush(color), track);
    ngs::ApplyFontSize(surface, (double)size, track);
    ngs::ApplyFontFamily(surface, fontFamily ? fontFamily : L"", track);
    ngs::ApplyOpacity(surface, opacity, track);
}

static void ApplyAllItemStyles() {
    ResolveSurface(L"Network", 0, g_networkPresenter, g_networkSurface);
    ResolveSurface(L"Volume", 1, g_volumePresenter, g_volumeSurface);
    ResolveSurface(L"Battery", 2, g_batteryGlyphFE, g_batterySurface);
    ResolveSurface(L"Battery percentage", 3, g_batteryPercentFE,
                   g_percentSurface);

    ApplyItemStyle(g_networkSurface, g_settings.networkColor, 0, nullptr,
                   g_settings.networkOpacity);
    ApplyItemStyle(g_volumeSurface, g_settings.volumeColor, 0, nullptr,
                   g_settings.volumeOpacity);
    ApplyItemStyle(g_batterySurface, g_settings.batteryColor, 0, nullptr,
                   g_settings.batteryOpacity);
    ApplyItemStyle(g_percentSurface, g_settings.percentColor,
                   g_settings.percentSize, g_settings.percentFontFamily,
                   g_settings.percentOpacity);
}

// ── Internal footprint ────────────────────────────────────────────────────
// The native ControlCenterButton owns taskbar placement and flyout semantics.
// Lease only its horizontal size so the complete internal grid isn't clipped;
// don't alter height or alignment.

static void ApplyItemsHostFootprint(StackPanel const& sp,
                                    OmniLayout const& layout) {
    TrackProperty(sp, FrameworkElement::WidthProperty());
    TrackProperty(sp, FrameworkElement::HeightProperty());
    TrackProperty(sp, FrameworkElement::HorizontalAlignmentProperty());
    TrackProperty(sp, FrameworkElement::VerticalAlignmentProperty());
    double footprintWidth = layout.total.width;
    sp.Width(footprintWidth);
    sp.Height(layout.total.height);
    sp.HorizontalAlignment(HorizontalAlignment::Center);
    sp.VerticalAlignment(VerticalAlignment::Center);
    ApplyOffset(sp, g_settings.offsetX, g_settings.offsetY);
    sp.InvalidateMeasure();
    if (g_omniButton) {
        if (auto control = g_omniButton.try_as<Control>()) {
            TrackProperty(control, Control::PaddingProperty());
            TrackProperty(
                control,
                Control::HorizontalContentAlignmentProperty());
            TrackProperty(
                control,
                Control::VerticalContentAlignmentProperty());
            // Arrangement padding belongs exclusively to Adjust.PadX. The
            // native button padding otherwise shrinks the usable content
            // area inside the exact arranged footprint and clips edge items.
            control.Padding(Thickness{0, 0, 0, 0});
            control.HorizontalContentAlignment(
                HorizontalAlignment::Center);
            control.VerticalContentAlignment(
                VerticalAlignment::Center);
        }
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
    g_networkPresenter = nullptr;  g_volumePresenter = nullptr;
    g_batteryPresenter = nullptr; g_batteryInnerPanel = nullptr;
    g_batteryGlyphFE = nullptr; g_batteryPercentFE = nullptr;
    g_networkSurface = {}; g_volumeSurface = {};
    g_batterySurface = {}; g_percentSurface = {};
    for (bool& logged : g_glyphMissLogged) logged = false;
    g_glyphTopUps = 0;
    g_percentCellWidth = 0.0;
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

static void SetItemVisibility(FrameworkElement const& element, bool visible) {
    if (!element) return;
    TrackProperty(element, UIElement::VisibilityProperty());
    element.Visibility(visible ? Visibility::Visible : Visibility::Collapsed);
}

// The battery ContentPresenter is expanded across the whole arranged
// footprint so its battery glyph and percentage can occupy independent cells.
// It is also the last native sibling, which otherwise puts that transparent
// footprint above Network and Volume for pointer hit-testing. Keep the two
// independent native presenters above it without changing any placement.
static void RaiseAboveBatteryHitSurface(FrameworkElement const& element) {
    if (!element) return;
    TrackProperty(element, Canvas::ZIndexProperty());
    Canvas::SetZIndex(element, 1);
}

static void PrepareSlot(FrameworkElement const& element, double slotWidth,
                        double slotHeight) {
    if (!element) return;
    TrackProperty(element, FrameworkElement::WidthProperty());
    TrackProperty(element, FrameworkElement::HeightProperty());
    TrackProperty(element, FrameworkElement::HorizontalAlignmentProperty());
    TrackProperty(element, FrameworkElement::VerticalAlignmentProperty());
    element.Width(slotWidth);
    element.Height(slotHeight);
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
    double naturalX = 0;
    double naturalY = 0;
    double desiredWidth = 0;
    double desiredHeight = 0;
};

// Measure the native child at its desired size, then return the translation
// needed to center that complete visual in one slot. This works for direct
// TextBlocks and for Windows' custom battery-icon element without assuming an
// internal template or relying on manual nudges.
static CellContentMetrics PrepareIndependentItem(
    FrameworkElement const& element, double slotWidth, double slotHeight) {
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
    result.desiredWidth = double(desired.Width);
    result.desiredHeight = double(desired.Height);
    result.centerX = std::max(0.0, (slotWidth - result.desiredWidth) / 2.0);
    result.centerY = std::max(0.0, (slotHeight - result.desiredHeight) / 2.0);
    return result;
}

// One-shot structure dump of an item's subtree. The battery "glyph" is a Grid,
// not a font glyph, so what is actually stylable under it has to be observed
// rather than assumed. Logged once per apply.
// maxDepth is a PARAMETER because the trees differ wildly. The battery's
// glyphs sit one level down; a SystemTray.IconView nests
// IconView > ContainerGrid > ContentGrid > TextIconContent > ContainerGrid >
// ... before reaching its InnerTextBlock. A limit tuned for the shallow case
// silently truncates the deep one and the dump answers nothing.
static void LogItemSubtree(PCWSTR label, DependencyObject const& root,
                           int maxDepth = 4, int depth = 0) {
    if (depth > maxDepth) return;
    int n = VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < n; i++) {
        auto child = VisualTreeHelper::GetChild(root, i);
        if (!child) continue;
        auto fe = child.try_as<FrameworkElement>();
        Wh_Log(L"[Probe] %s d%d[%d] class=%s name=%s size=%.1fx%.1f", label,
               depth, i, winrt::get_class_name(child).c_str(),
               fe ? fe.Name().c_str() : L"", fe ? fe.ActualWidth() : 0.0,
               fe ? fe.ActualHeight() : 0.0);
        LogItemSubtree(label, child, maxDepth, depth + 1);
    }
}

static void ReadNaturalOrigin(
    FrameworkElement const& element, FrameworkElement const& relativeTo,
    CellContentMetrics& metrics) {
    if (!element || !relativeTo) return;
    try {
        auto point = element.TransformToVisual(relativeTo).TransformPoint({});
        metrics.naturalX = point.X;
        metrics.naturalY = point.Y;
    } catch (...) {
        Wh_Log(L"[Layout] Failed to read a battery child's native origin");
    }
}

// Translate an item from its current natural StackPanel Y to an absolute pixel
// position within the grid footprint. Hidden siblings are collapsed and don't
// contribute to naturalY.
static void PositionSlot(FrameworkElement const& fe, double naturalY,
                         double x, double y) {
    ApplyOffset(fe, x, -naturalY + y);
}

// Measure one item's natural width for fit-to-content sizing. Returns whether
// there is a usable width now — a false answer is not a failure, just an item
// whose template has not expanded yet, and the caller asks for one more pass.
//
// No slack is added, unlike the percentage. Slack exists there to absorb text
// growing from "9%" to "100%"; a glyph does not grow, and padding it would put
// back the dead space this is here to remove.
static bool MeasureItemContentWidth(int item, FrameworkElement const& element) {
    if (item < 0 || item > 3) return true;
    if (g_itemContentWidth[item] > 0.0) return true;  // already settled
    if (!element) return true;  // item is not present; nothing to wait for
    double measured = ngs::MeasureNatural(element, GlyphTrack()).Width;
    if (measured <= 0.0) return false;
    g_itemContentWidth[item] =
        std::max(g_itemContentWidth[item], std::ceil(measured));
    return true;
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

    // Locate battery slot by class-name substring search.
    int battIdx = -1;
    for (int i = 0; i < n; i++) {
        auto child =
            VisualTreeHelper::GetChild(sp, i).try_as<FrameworkElement>();
        if (!child) continue;
        bool battery = HasBatteryDescendant(child);
        Wh_Log(L"[Layout] native slot=%d class=%s name=%s battery=%d "
               L"size=%.1fx%.1f",
               i, winrt::get_class_name(child).c_str(),
               child.Name().c_str(), battery, child.ActualWidth(),
               child.ActualHeight());
        if (battery && battIdx < 0) battIdx = i;
    }

    bool hasBattPres = battIdx >= 0;
    bool hasPercent = false;
    if (hasBattPres) {
        g_batteryPresenter =
            VisualTreeHelper::GetChild(sp, battIdx)
                .try_as<FrameworkElement>();
        if (WalkSetupBatteryInnerPanel(g_batteryPresenter))
            hasPercent = g_batteryPercentFE != nullptr;
    }

    // Log any unknown slots (future Windows builds may add more items).
    for (int i = 0; i < n; i++) {
        if (i == 0 || i == 1 || i == battIdx) continue;
        auto child = VisualTreeHelper::GetChild(sp, i);
        if (child)
            Wh_Log(L"[Layout] Unknown slot index %d: %s", i,
                   winrt::get_class_name(child).c_str());
    }

    // Style BEFORE measuring anything. A glyph size or font family the user
    // chose changes an item's natural width, so measuring first would reserve
    // cells for the native size and then paint a different size into them.
    if (n >= 1)
        g_networkPresenter =
            VisualTreeHelper::GetChild(sp, 0).try_as<FrameworkElement>();
    if (n >= 2)
        g_volumePresenter =
            VisualTreeHelper::GetChild(sp, 1).try_as<FrameworkElement>();
    ApplyAllItemStyles();

    // FIT TO CONTENT. Size.ItemWidth = 0 asks each item for the width its own
    // content needs. The ARRANGEMENT is what reserves that space, so the widths
    // have to be known before the arrangement is resolved, not after.
    g_cellsNeedRemeasure = false;
    if (g_settings.itemWidth <= 0) {
        bool settled = true;
        settled &= MeasureItemContentWidth(0, g_networkPresenter);
        settled &= MeasureItemContentWidth(1, g_volumePresenter);
        settled &= MeasureItemContentWidth(2, g_batteryGlyphFE);
        if (!settled && g_remeasures < kMaxRemeasures) {
            g_remeasures++;
            g_cellsNeedRemeasure = true;
        }
        Wh_Log(L"[Layout] fit to content: network=%.0f volume=%.0f battery=%.0f "
               L"(0 = not measured yet, falling back to %.0f)%s",
               g_itemContentWidth[0], g_itemContentWidth[1],
               g_itemContentWidth[2], kUnmeasuredCellWidth,
               g_cellsNeedRemeasure ? L" - will re-measure" : L"");
    }

    // Measure the percentage BEFORE the arrangement is resolved, so its cell
    // is reserved at the width its text actually needs. Slack absorbs the
    // small growth from a battery tick (80% -> 100%) without a re-apply.
    g_percentCellWidth = 0.0;
    g_percentNeedsWiderCell = false;
    if (g_batteryPercentFE && g_settings.percent) {
        double measured =
            ngs::MeasureNatural(g_batteryPercentFE, GlyphTrack()).Width;
        if (measured > 0.0)
            g_percentWidestDesired =
                std::max(g_percentWidestDesired, std::ceil(measured) + 2.0);
        g_percentCellWidth = g_percentWidestDesired;
        if (auto text = g_batteryPercentFE.try_as<TextBlock>())
            SetPercentMeasuredText(text.Text().c_str());
        Wh_Log(L"[Layout] percentage \"%s\" measures %.1f, widest seen %.0f "
               L"-> cell %.0f (ItemWidth %d)",
               g_percentMeasuredText, measured, g_percentWidestDesired,
               std::max((double)g_settings.itemWidth, g_percentCellWidth),
               g_settings.itemWidth);
    }

    if (g_batteryGlyphFE)
        LogItemSubtree(L"battery", g_batteryGlyphFE);
    // Network and volume are NOT dumped. They were, at depth 12, to find out why
    // a glyph size produced a ghost — the answer was three stacked
    // AdaptiveTextBlocks (Underlay / Base / AccentOverlay), and the Surface
    // now reports that itself as "a STACK of N layered glyphs". Twenty lines
    // per apply to re-establish a settled fact is noise that buries the lines
    // that still matter. Raise the depth here again if a Windows build ever
    // makes the [Style] line look wrong.

    bool hasNetwork = n >= 1;
    bool hasVolume = n >= 2;
    OmniLayout layout = ResolveOmniLayout(
        hTaskbarWnd, hasNetwork, hasVolume,
        hasBattPres && g_batteryGlyphFE, hasPercent);

    ApplyItemsHostFootprint(sp, layout);

    // Network (native slot 0). Windows changes this one presenter's glyph
    // between Wi-Fi, Ethernet, disconnected, airplane-mode and VPN states.
    if (n >= 1) {
        auto network =
            VisualTreeHelper::GetChild(sp, 0).try_as<FrameworkElement>();
        if (network) {
            g_networkPresenter = network;
            RaiseAboveBatteryHitSurface(network);
            SetItemVisibility(network, layout.visible[0]);
            if (layout.visible[0] && layout.hasPlacement[0]) {
                auto const& p = layout.placement[0];
                PrepareSlot(network, p.size.width, p.size.height);
                PositionSlot(network, 0, p.x, p.y);
            }
        }
    }

    // Volume (native slot 1).
    if (n >= 2) {
        auto volume =
            VisualTreeHelper::GetChild(sp, 1).try_as<FrameworkElement>();
        if (volume) {
            g_volumePresenter = volume;
            RaiseAboveBatteryHitSurface(volume);
            SetItemVisibility(volume, layout.visible[1]);
            if (layout.visible[1] && layout.hasPlacement[1]) {
                auto const& p = layout.placement[1];
                PrepareSlot(volume, p.size.width, p.size.height);
                double naturalY =
                    layout.visible[0] ? g_settings.itemHeight : 0;
                PositionSlot(volume, naturalY, p.x, p.y);
            }
        }
    }

    // Apply font/size before measuring direct TextBlocks for cell centering.
    ApplyAllItemStyles();

    // Battery + percent.
    if (hasBattPres && g_batteryPresenter) {
        bool showBatteryGroup =
            layout.visible[2] || layout.visible[3];
        SetItemVisibility(g_batteryPresenter, showBatteryGroup);
        SetItemVisibility(g_batteryGlyphFE, layout.visible[2]);
        SetItemVisibility(g_batteryPercentFE, layout.visible[3]);
        if (!showBatteryGroup) {
            // Visibility participates in the tracked property lease and is
            // restored during settings changes and unload.
        } else {
            // The battery presenter and inner StackPanel span the complete
            // arranged footprint. Battery and percentage are always separate
            // arrangement items translated to their named cells.
            double footprintWidth = layout.total.width;
            double footprintHeight = layout.total.height;
            TrackProperty(g_batteryPresenter,
                          FrameworkElement::WidthProperty());
            TrackProperty(g_batteryPresenter,
                          FrameworkElement::HeightProperty());
            TrackProperty(g_batteryPresenter,
                          FrameworkElement::HorizontalAlignmentProperty());
            TrackProperty(g_batteryPresenter,
                          FrameworkElement::VerticalAlignmentProperty());
            g_batteryPresenter.Width(footprintWidth);
            g_batteryPresenter.Height(footprintHeight);
            g_batteryPresenter.HorizontalAlignment(
                HorizontalAlignment::Left);
            g_batteryPresenter.VerticalAlignment(
                VerticalAlignment::Top);
            if (auto presenter =
                    g_batteryPresenter.try_as<ContentPresenter>()) {
                TrackProperty(
                    presenter,
                    ContentPresenter::HorizontalContentAlignmentProperty());
                TrackProperty(
                    presenter,
                    ContentPresenter::VerticalContentAlignmentProperty());
                presenter.HorizontalContentAlignment(
                    HorizontalAlignment::Left);
                presenter.VerticalContentAlignment(
                    VerticalAlignment::Top);
            }

            // Move the presenter to the arrangement origin, accounting for
            // collapsed network/volume siblings that no longer contribute
            // natural height.
            double batteryNaturalY =
                (layout.visible[0] ? g_settings.itemHeight : 0) +
                (layout.visible[1] ? g_settings.itemHeight : 0);
            ApplyOffset(g_batteryPresenter, 0, -batteryNaturalY);

            if (g_batteryInnerPanel) {
                TrackProperty(
                    g_batteryInnerPanel,
                    StackPanel::OrientationProperty());
                TrackProperty(
                    g_batteryInnerPanel,
                    FrameworkElement::WidthProperty());
                TrackProperty(
                    g_batteryInnerPanel,
                    FrameworkElement::HeightProperty());
                TrackProperty(
                    g_batteryInnerPanel,
                    FrameworkElement::HorizontalAlignmentProperty());
                TrackProperty(
                    g_batteryInnerPanel,
                    FrameworkElement::VerticalAlignmentProperty());
                g_batteryInnerPanel.Orientation(
                    Orientation::Horizontal);
                g_batteryInnerPanel.Width(footprintWidth);
                g_batteryInnerPanel.Height(footprintHeight);
                g_batteryInnerPanel.HorizontalAlignment(
                    HorizontalAlignment::Left);
                g_batteryInnerPanel.VerticalAlignment(
                    VerticalAlignment::Top);
            }

            CellContentMetrics batteryMetrics =
                layout.visible[2] && layout.hasPlacement[2]
                    ? PrepareIndependentItem(
                          g_batteryGlyphFE, layout.placement[2].size.width,
                          layout.placement[2].size.height)
                    : CellContentMetrics{};
            CellContentMetrics percentMetrics =
                layout.visible[3] && layout.hasPlacement[3]
                    ? PrepareIndependentItem(
                          g_batteryPercentFE, layout.placement[3].size.width,
                          layout.placement[3].size.height)
                    : CellContentMetrics{};

            // The measurement taken before the arrangement is the only one
            // available at that point, but this one is taken with the element
            // in its final parent. If it wants more room than the cell that
            // was reserved, record the true width and ask for exactly one
            // re-apply; the sticky widest-seen value survives the reset so the
            // second pass arranges at the correct width instead of measuring
            // small all over again.
            if (layout.visible[3] && layout.hasPlacement[3] &&
                percentMetrics.desiredWidth >
                    layout.placement[3].size.width + 0.5) {
                double corrected = std::ceil(percentMetrics.desiredWidth) + 2.0;
                Wh_Log(L"[Layout] percentage really needs %.1f but was given "
                       L"%.0f - widening its cell to %.0f and re-applying",
                       percentMetrics.desiredWidth,
                       layout.placement[3].size.width, corrected);
                if (corrected > g_percentWidestDesired) {
                    g_percentWidestDesired = corrected;
                    g_percentNeedsWiderCell = true;
                }
            }

            // Belt and braces, and the thing that actually stops the visible
            // clip: whatever the cell math decided, no item may be arranged
            // past the group's own right edge. Measure where the percentage
            // really ends and widen the group NOW, rather than relying on a
            // re-apply that may not arrive. Every property touched here was
            // already leased above, so the restore is unaffected.
            if (layout.visible[3] && layout.hasPlacement[3]) {
                double requiredWidth =
                    layout.placement[3].x + percentMetrics.desiredWidth +
                    (double)g_settings.padX;
                if (requiredWidth > footprintWidth + 0.5) {
                    Wh_Log(L"[Layout] percentage ends at %.1f but the group is "
                           L"only %.0f wide - widening to %.0f so it cannot "
                           L"clip",
                           requiredWidth - (double)g_settings.padX,
                           footprintWidth, requiredWidth);
                    sp.Width(requiredWidth);
                    g_batteryPresenter.Width(requiredWidth);
                    if (g_batteryInnerPanel)
                        g_batteryInnerPanel.Width(requiredWidth);
                    if (g_omniButton) {
                        g_omniButton.MinWidth(requiredWidth);
                        g_omniButton.InvalidateMeasure();
                    }
                    sp.InvalidateMeasure();
                }
            }

            if (g_batteryInnerPanel) {
                try {
                    g_batteryInnerPanel.UpdateLayout();
                } catch (...) {
                    Wh_Log(L"[Layout] Failed to update the battery panel "
                           L"before measuring native origins");
                }
                // MEASURE IN THE SAME SPACE THE ARRANGEMENT USES. The cell
                // positions below are in the ITEMS HOST's coordinates, so the
                // natural origin has to be too. Measuring it against the inner
                // panel instead silently adds that panel's own inset to every
                // item in the battery group — a constant 4px on this build,
                // which is exactly how the percentage kept ending up hard
                // against the button's edge no matter what the cell math said.
                //
                // Measuring against `sp` also folds in the presenter's own
                // vertical offset, applied above: TransformToVisual reports
                // where the child actually is right now, and the offset below
                // is then simply (target - current). Nothing is double-counted.
                ReadNaturalOrigin(g_batteryGlyphFE, sp, batteryMetrics);
                ReadNaturalOrigin(g_batteryPercentFE, sp, percentMetrics);
            }

            double batteryX = 0;
            double batteryY = 0;
            if (layout.visible[2] && layout.hasPlacement[2]) {
                auto const& p = layout.placement[2];
                batteryX = p.x;
                batteryY = p.y;
                ApplyOffset(g_batteryGlyphFE,
                            batteryX + batteryMetrics.centerX -
                                batteryMetrics.naturalX,
                            batteryY + batteryMetrics.centerY -
                                batteryMetrics.naturalY);
            }

            double percentX = 0;
            double percentY = 0;
            if (layout.visible[3] && layout.hasPlacement[3]) {
                auto const& p = layout.placement[3];
                percentX = p.x;
                percentY = p.y;
                ApplyOffset(
                    g_batteryPercentFE,
                    percentX + percentMetrics.centerX -
                        percentMetrics.naturalX,
                    percentY + percentMetrics.centerY -
                        percentMetrics.naturalY);
            }

            Wh_Log(L"[Layout] Indep battCell=(%.1f,%.1f) "
                   L"center=(%.2f,%.2f) origin=(%.2f,%.2f) "
                   L"pctCell=(%.1f,%.1f) center=(%.2f,%.2f) "
                   L"origin=(%.2f,%.2f)",
                   batteryX, batteryY, batteryMetrics.centerX,
                   batteryMetrics.centerY, batteryMetrics.naturalX,
                   batteryMetrics.naturalY, percentX, percentY,
                   percentMetrics.centerX, percentMetrics.centerY,
                   percentMetrics.naturalX, percentMetrics.naturalY);
        }
    }

    // The arrangement arithmetic can be provably correct and the result can
    // still clip, because the number that matters is where the content ends
    // in the BUTTON's own space versus how wide the button actually rendered.
    // Everything above is what the mod intends; this is what the framework
    // did. Log both edges of every container so a mismatch names itself
    // instead of costing another round of theorising.
    try {
        if (g_omniButton) g_omniButton.UpdateLayout();
        auto edgeInButton = [](FrameworkElement const& element) -> double {
            if (!element || !g_omniButton) return -1.0;
            try {
                auto origin = element.TransformToVisual(g_omniButton)
                                  .TransformPoint({});
                return origin.X + element.ActualWidth();
            } catch (...) {
                return -1.0;
            }
        };
        Wh_Log(L"[Geometry] button=%.1f sp=%.1f battPresenter=%.1f "
               L"innerPanel=%.1f percent=%.1f | right edge in button space: "
               L"sp=%.1f battery=%.1f percent=%.1f",
               g_omniButton ? g_omniButton.ActualWidth() : -1.0,
               sp.ActualWidth(),
               g_batteryPresenter ? g_batteryPresenter.ActualWidth() : -1.0,
               g_batteryInnerPanel ? g_batteryInnerPanel.ActualWidth() : -1.0,
               g_batteryPercentFE ? g_batteryPercentFE.ActualWidth() : -1.0,
               edgeInButton(sp), edgeInButton(g_batteryGlyphFE),
               edgeInButton(g_batteryPercentFE));
    } catch (...) {
        Wh_Log(L"[Geometry] Failed to read the final rendered geometry");
    }

    Wh_Log(L"[Layout] Applied arrangement (SP children=%d)", n);
}

// ── Native item availability monitor (LayoutUpdated) ──────────────────────
// Windows can add or remove the percentage child without notifying this mod.
// Keep the existing LayoutUpdated path attached and rebuild only when the
// native child set actually changes.

static void OnLayoutUpdated(IInspectable const&, IInspectable const&);

static void RegisterLayoutUpdatedMonitor(StackPanel const& sp) {
    RevokeLayoutUpdated();
    if (!sp) return;
    g_layoutUpdatedSP = sp;
    g_layoutUpdatedToken = sp.LayoutUpdated(OnLayoutUpdated);
}

static void OnLayoutUpdatedImpl() {
    auto sp = g_layoutUpdatedSP;
    if (!sp) return;

    bool changed = false;
    int childCount = VisualTreeHelper::GetChildrenCount(sp);
    if (!g_networkPresenter && childCount >= 1)
        changed = true;
    if (!g_volumePresenter && childCount >= 2)
        changed = true;

    if (!g_batteryPresenter) {
        for (int i = 0; i < childCount; ++i) {
            auto child = VisualTreeHelper::GetChild(sp, i)
                             .try_as<FrameworkElement>();
            if (child && HasBatteryDescendant(child)) {
                changed = true;
                break;
            }
        }
    } else if (!g_batteryInnerPanel) {
        if (WalkSetupBatteryInnerPanel(g_batteryPresenter))
            changed = true;
    } else {
        FrameworkElement nativePercent = nullptr;
        int batteryChildren =
            VisualTreeHelper::GetChildrenCount(g_batteryInnerPanel);
        if (batteryChildren >= 2) {
            nativePercent =
                VisualTreeHelper::GetChild(g_batteryInnerPanel, 1)
                    .try_as<FrameworkElement>();
        }
        if (nativePercent != g_batteryPercentFE)
            changed = true;
    }

    // The arrangement pass found the reserved cell too narrow.
    if (!changed && g_percentNeedsWiderCell) {
        g_percentNeedsWiderCell = false;
        changed = true;
    }

    // Fit-to-content had to guess for at least one item because its template
    // had not expanded when the arrangement was resolved. Now that a layout
    // pass has run it probably has, so arrange again at the real widths.
    if (!changed && g_cellsNeedRemeasure) {
        g_cellsNeedRemeasure = false;
        changed = true;
    }

    // The percentage's text grows as the battery charges ("9%" -> "100%") and
    // its cell was measured for an earlier value. Reading Text is a free
    // property read, unlike a Measure, so this is safe on a path that fires
    // every layout pass. Re-applying re-measures and re-records the text, so
    // it settles rather than looping.
    if (!changed && g_percentSurface.text && g_percentCellWidth > 0.0) {
        auto current = g_percentSurface.text.Text();
        if (wcscmp(current.c_str(), g_percentMeasuredText) != 0) {
            Wh_Log(L"[Layout] percentage text \"%s\" -> \"%s\"; re-measuring",
                   g_percentMeasuredText, current.c_str());
            changed = true;
        }
    }

    if (!changed) {
        // A glyph TextBlock can arrive later than the presenter that hosts it.
        // That is not a structural change, so top the styling up in place
        // rather than tearing the whole layout down and rebuilding it.
        // LayoutUpdated fires on every layout pass, and each attempt walks the
        // slot subtree, so bound it: a template that has not produced a glyph
        // after this many passes is not going to.
        if (!AllGlyphsResolved() &&
            g_glyphTopUps < kMaxGlyphTopUps) {
            g_glyphTopUps++;
            ApplyAllItemStyles();
            if (g_glyphTopUps == kMaxGlyphTopUps && !AllGlyphsResolved()) {
                Wh_Log(L"[Style] Giving up on the unresolved glyph(s) after "
                       L"%d layout passes; the item keeps its native "
                       L"appearance", kMaxGlyphTopUps);
            }
        }
        return;
    }

    Wh_Log(L"[Layout] Native OmniButton items changed - re-applying");

    auto savedOmniButton = g_omniButton;
    CleanupAndResetCurrentElements();
    g_omniButton = savedOmniButton;
    ApplyLayout(sp, g_taskbarWnd);
    RegisterLayoutUpdatedMonitor(sp);
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
    return tbh::FindCurrentProcessTaskbarWnd();
}

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

// ── GetTaskbarXamlRoot ────────────────────────────────────────────────────
// The CTaskBand walk, the runtime-disassembled FrameworkElement offset, and
// the taskbar.dll symbol hooks all live in _templates/taskbar-host.h now.

static XamlRoot GetTaskbarXamlRoot(HWND hTaskbarWnd) {
    return tbh::GetTaskbarXamlRoot(hTaskbarWnd);
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
static FrameworkElement FindChildRecursive(
    FrameworkElement const& e,
    std::function<bool(FrameworkElement const&)> const& cb,
    int maxDepth = 20) {
    return vtw::FindDescendant(e, maxDepth, cb);
}

// ── Apply settings ────────────────────────────────────────────────────────

// Logged once per orientation change rather than on every retry.
static bool g_verticalStandDownLogged = false;

static bool ApplyAllSettings() {
    HWND hWnd = FindCurrentProcessTaskbarWnd();
    if (!hWnd) { Wh_Log(L"[Apply] No taskbar window"); return false; }
    g_taskbarWnd = hWnd;

    // A vertical taskbar (Windhawk's "Vertical Taskbar for Windows 11") walks
    // the identical ControlCenterButton path this mod walks and owns
    // RenderTransform on the same children to rotate them. This mod positions
    // by writing RenderTransform on those same elements, so the two cannot
    // both be right. Stand down completely and leave the taskbar untouched;
    // returning true retires the retry loop, and an Explorer rebuild
    // re-evaluates if the user turns that mod off.
    auto metrics = tbh::GetMetrics(hWnd);
    if (metrics.valid && !tbh::LayoutModelApplies(metrics)) {
        if (!g_verticalStandDownLogged) {
            g_verticalStandDownLogged = true;
            Wh_Log(L"[Apply] Taskbar is %s - standing down. This mod arranges "
                   L"items with RenderTransform, which a vertical taskbar mod "
                   L"already owns on the same elements; leaving the native "
                   L"OmniButton untouched.",
                   tbh::OrientationName(metrics.orientation));
        }
        return true;
    }
    g_verticalStandDownLogged = false;

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
                    RegisterLayoutUpdatedMonitor(sp);
                    Wh_Log(L"[Apply] Monitoring native OmniButton items");
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
    // Success means styled, not merely found. Reporting success while a glyph
    // TextBlock is still missing retires the retry thread and the IconView
    // Loaded handler, and the item keeps its native color forever.
    return g_omniStackPanel != nullptr && AllGlyphsResolved();
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
        g_autoRevokerList->emplace_back();
        auto it = std::prev(g_autoRevokerList->end());
        *it = iconView.Loaded(winrt::auto_revoke_t{},
            [it](IInspectable const&, RoutedEventArgs const&) {
                try {
                    g_autoRevokerList->erase(it);
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
    // SystemTray.dll, Taskbar.View.dll, ExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK systemTrayModuleHooks[] = {{
        {LR"(public: __cdecl winrt::SystemTray::implementation::IconView::IconView(void))"},
        &IconView_IconView_Original, IconView_IconView_Hook,
    }};
    if (!WindhawkUtils::HookSymbols(hModule, systemTrayModuleHooks,
                                    ARRAYSIZE(systemTrayModuleHooks))) {
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

static void StopRetryThread() { g_retryLoop.Stop(); }

static void StartRetryThread() {
    if (g_unloading) return;
    g_retryLoop.Start(ApplyOnTaskbarWindowThread, [] { return g_applied.load(); },
                      g_unloading);
}

// Explorer rebuilt the taskbar: the whole XAML tree we were holding is gone.
static void OnTaskbarRebuilt() {
    if (g_unloading) return;
    g_applied = false;
    g_reapplyPending = true;
    // The elements those widths were measured from no longer exist, and the
    // new tree gets its own bounded budget of re-measure passes.
    for (double& width : g_itemContentWidth) width = 0.0;
    g_remeasures = 0;
    StartRetryThread();
}

static bool HookTaskbarDllSymbols() {
    if (!tbh::HookTaskbarSymbols(OnTaskbarRebuilt)) {
        Wh_Log(L"[Hooks] taskbar.dll symbol hooks failed");
        return false;
    }
    return true;
}

// ── Windhawk lifecycle ─────────────────────────────────────────────────────

BOOL Wh_ModInit() {
    Wh_Log(L"[Init] OmniButton Customizer v2.0");
    // Failures inside a template-marshalled UI callback report in this mod's
    // voice rather than vanishing.
    tbh::SetExceptionLogger(LogCurrentUiException);
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
            // Controlled UI-thread unload: revoke/restore on this thread, then
            // reset() the no_destroy optionals to free their heap buffers.
            g_autoRevokerList->clear();
            CleanupAndResetCurrentElements();
            g_autoRevokerList.reset();
            g_lease.reset();
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
    // A glyph-size or font-family change invalidates every width measured so
    // far — the percentage's text width and every fit-to-content cell alike.
    g_percentWidestDesired = 0.0;
    SetPercentMeasuredText(nullptr);
    for (double& width : g_itemContentWidth) width = 0.0;
    g_remeasures = 0;
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
