// ==WindhawkMod==
// @id              after-effects-theme
// @name            After Effects Theme
// @description     Recolors the Adobe After Effects interface — panels, Timeline, viewers, window frame and menu bar — with a choice of very dark palettes.
// @version         1.0.0
// @author          Threshold Editor
// @license         MIT
// @include         AfterFX.exe
// @include         AfterFX (Beta).exe
// @github          https://github.com/CakeDev4k
// @architecture    x86-64
// @compilerOptions -ldwmapi -lgdi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# After Effects Theme

Recolors After Effects far beyond what the Appearance brightness slider
reaches, and does it consistently across the parts of the app that are painted
in different ways.

It is the sister of **Premiere Pro Theme**: the two apps share Adobe's UI
toolkit, `dvaui.dll`, so the same palettes land on After Effects the same way.

## Screenshots

The same project in After Effects 2026, palette by palette.

**Onyx**: the default. Near black, and neutral.

![Onyx palette](https://raw.githubusercontent.com/CakeDev4k/after-effects-theme/main/screenshots/onyx.png)

**Abyss**: absolute black, for an OLED panel.

![Abyss palette](https://raw.githubusercontent.com/CakeDev4k/after-effects-theme/main/screenshots/abyss.png)

**Graphite**: dark, a little more legible.

![Graphite palette](https://raw.githubusercontent.com/CakeDev4k/after-effects-theme/main/screenshots/graphite.png)

**After Effects**: the violet sampled from the app icon.

![After Effects palette](https://raw.githubusercontent.com/CakeDev4k/after-effects-theme/main/screenshots/aftereffects.png)

**Comfy**: warm brown, low contrast for long sessions.

![Comfy palette](https://raw.githubusercontent.com/CakeDev4k/after-effects-theme/main/screenshots/comfy.png)

**Neon**: near black with magenta.

![Neon palette](https://raw.githubusercontent.com/CakeDev4k/after-effects-theme/main/screenshots/neon.png)

**Glitch**: acid green with a magenta accent.

![Glitch palette](https://raw.githubusercontent.com/CakeDev4k/after-effects-theme/main/screenshots/glitch.png)

**Contrast**: black panels, light dividers, pure white text.

![Contrast palette](https://raw.githubusercontent.com/CakeDev4k/after-effects-theme/main/screenshots/contrast.png)

**Violet**: a purple interface, not just a purple accent.

![Violet palette](https://raw.githubusercontent.com/CakeDev4k/after-effects-theme/main/screenshots/violet.png)

**Blossom**: dark rose, pastel in its text and border.

![Blossom palette](https://raw.githubusercontent.com/CakeDev4k/after-effects-theme/main/screenshots/blossom.png)

**Ember**: near black under a strong orange.

![Ember palette](https://raw.githubusercontent.com/CakeDev4k/after-effects-theme/main/screenshots/ember.png)

**Amethyst**: neutral panels, the purple only on the edges.

![Amethyst palette](https://raw.githubusercontent.com/CakeDev4k/after-effects-theme/main/screenshots/amethyst.png)

**Crimson**: near black under a strong red.

![Crimson palette](https://raw.githubusercontent.com/CakeDev4k/after-effects-theme/main/screenshots/crimson.png)

**Threshold**: `#050505`, `#FFFFFF` and `#DC2626`.

![Threshold palette](https://raw.githubusercontent.com/CakeDev4k/after-effects-theme/main/screenshots/threshold.png)

**Miku**: deep teal with a cyan accent.

![Miku palette](https://raw.githubusercontent.com/CakeDev4k/after-effects-theme/main/screenshots/miku.png)

## Palettes

Neutral:

| Level   | Onyx      | Abyss     | Graphite  | Contrast  |
|---------|-----------|-----------|-----------|-----------|
| Base    | `#050505` | `#000000` | `#0D0D0D` | `#000000` |
| Panel   | `#090909` | `#040404` | `#141414` | `#060606` |
| Surface | `#0E0E0E` | `#080808` | `#1A1A1A` | `#101010` |
| Raised  | `#161616` | `#101010` | `#232323` | `#202020` |
| Border  | `#242424` | `#1C1C1C` | `#303030` | `#4A4A4A` |

Tinted:

| Level   | After Effects | Comfy     | Neon      | Glitch    |
|---------|---------------|-----------|-----------|-----------|
| Base    | `#06060F`     | `#100C09` | `#080508` | `#060A02` |
| Panel   | `#0C0C20`     | `#181310` | `#0E0A0D` | `#0C1305` |
| Surface | `#131333`     | `#211A15` | `#1A0F18` | `#131D08` |
| Raised  | `#1E1E4D`     | `#2E241C` | `#2A1526` | `#1E2C0C` |
| Border  | `#33337A`     | `#453528` | `#4D213F` | `#3C5410` |
| Text    | `#D2D2F7`     | `#EDE0D0` | `#FAF5EF` | `#E9F7C0` |
| Accent  | `#3A3A99`     | `#5A4028` | `#5C1F47` | `#B8157A` |

Vivid — the hue is in the ramp, so the panels themselves carry it:

| Level     | Violet    | Blossom   | Ember     | Crimson   | Miku      |
|-----------|-----------|-----------|-----------|-----------|-----------|
| Base      | `#0A0414` | `#120A0E` | `#0C0703` | `#0D0405` | `#061014` |
| Panel     | `#120827` | `#180D13` | `#140B04` | `#150609` | `#0A1920` |
| Surface   | `#1C0C3B` | `#241320` | `#1F1206` | `#200A0E` | `#10262D` |
| Raised    | `#2A1257` | `#341B2E` | `#2E1A08` | `#300F15` | `#173740` |
| Border    | `#4B2088` | `#5A2C4A` | `#55300C` | `#591A24` | `#28515C` |
| Text      | `#E2D4FA` | `#FBDCE8` | `#FBE3C8` | `#F7D9DD` | `#DDFBFF` |
| Accent    | `#6D28D9` | `#864160` | `#944108` | `#B91C1C` | `#0E7478` |
| Highlight | `#7737DC` | `#934869` | `#9F4608` | `#BB2222` | `#2B6A68` |

Strong accent on near black — the panels stay neutral, and the color is only the
edge, the accent and the highlight:

| Level     | Amethyst  | Threshold |
|-----------|-----------|-----------|
| Base      | `#050507` | `#050505` |
| Panel     | `#09090D` | `#0A0A0A` |
| Surface   | `#0F0F16` | `#121212` |
| Raised    | `#181823` | `#1C1C1C` |
| Border    | `#33254F` | `#2B2B2B` |
| Text      | `#E4DCF2` | `#FFFFFF` |
| Accent    | `#6730C6` | `#DC2626` |
| Highlight | `#7537DF` | `#BC2020` |

**After Effects** takes its violet from the app's icon; **Neon** and
**Glitch** come from reference artwork, and **Comfy** is the lightest, for
long sessions. **Contrast** keeps a `#4A4A4A` border so edges stay legible on
black panels. **Blossom** is pastel in its text and border only: a pastel
background would leave the app's own light text unreadable. **Threshold** is
threshold-editor.com.br's `#050505`, `#FFFFFF` and `#DC2626`.

The accent stays out of the ramp, which is interpolated, so it only shows on
hovered menu items and system highlights; each one is at least 4.5:1 against
its palette's text.

Contrast and the palettes with a **Highlight** row give the interface blue —
the focused panel's border, the active tool, selections, links — their own
hue, each blue at its own brightness, so white text on a blue button keeps its
contrast. The row is the shade of Spectrum's `#005CC8`, the blue most of the
interface uses; Contrast's is the gray `#606060`. **Palette highlight** in the
settings keeps the blue on any palette.

## Custom themes

**Custom** takes its colors from the **Custom theme** group in the settings,
one field each:

| Field             |          | What it colors                                                |
|-------------------|----------|---------------------------------------------------------------|
| **Base**          | required | the darkest step: the deepest background                      |
| **Panel**         | required | the second step, and the title bar and menu bar               |
| **Surface**       | required | the middle step                                               |
| **Raised**        | required | the fourth step                                               |
| **Border**        | required | the lightest step: dividers and edges                         |
| **Text**          | required | menu and title bar text                                       |
| **Accent**        | optional | hovered menu items; the border when left empty                |
| **Disabled text** | optional | disabled menu items; halfway from text to panel when empty    |
| **Highlight**     | optional | the hue the interface blue takes; the blue stays when empty   |

Colors are `#RRGGBB`, or `#RRGGBBAA` from a picker that writes an alpha,
which is ignored. The group starts as Onyx, with **Highlight** empty. A
required field left empty or misspelled falls back to Onyx's color, named in
the log, so the interface never ends up half themed.

**Highlight** in the palette tables above is the shade the hue produces, not
the hue: Violet's row reads `#7737DC`, which is what its `#6D28D9` becomes at
the brightness of the interface's own blue — so a custom theme wanting
Violet's highlight takes `#6D28D9`. Every other row is the value the field
takes directly.

### Sharing one

The **Settings** tab has a text mode next to the visual one, and what it holds
is the settings as YAML — the palette, then `customTheme` as a block under it:

```yaml
palette: custom
customTheme:
  base: '#050505'
  panel: '#0A0A0A'
  surface: '#121212'
  raised: '#1C1C1C'
  border: '#2B2B2B'
  text: '#FFFFFF'
  accent: '#DC2626'
  disabledText: '#8A8A8A'
  highlight: '#DC2626'
```

**To use a theme someone sent you**, paste it into that box over what is there
and save. The colors need their quotes: a bare `#` starts a comment in YAML.

**To copy your own out**, put the palette on **Custom** and read the mod's log.
It writes the theme as one line, in YAML's flow form, which that box reads the
same as the block above. Saving that box replaces every setting it is given, so
the log writes only the palette and the colors, never the layer switches.

Keep the five steps dark and in order: After Effects' own text is light and is
not recolored.

## What it changes, and what it leaves alone

Each layer has its own switch in the settings:

- **After Effects interface** — the theme colors served by `dvaui.dll`,
  Adobe's UI toolkit: panels, the Timeline, the viewers, Effect Controls.
- **Direct fills** — surfaces painted without asking the theme, through the
  Direct2D brush factory most solid fills pass through.
- **Window and system dialogs** — dark title bar, border and native dialogs.
- **Menu bar and menus** — the File / Edit / Composition bar and its dropdowns.
- **GDI surfaces** — brushes, pens and text backgrounds created by Adobe's
  own modules.
- **UXP panels** — the Home screen, bar across its top included, drawn from
  stylesheets, scripts and design tokens of its own. Off by default, because
  it only follows a change across a restart: turn it on, and restart, for the
  Home screen to take the palette.
- **Palette highlight** — the interface blue, on the palettes that carry a
  highlight.

Saturated colors — layer labels, keyframes, warnings, and the interface blue
unless the palette carries a highlight — pass through, and so does anything
above the **brightness ceiling**: text, icons, and the `#4B4B4B` of disabled
text. Content is left alone whatever its color: the color picker, color
chips, the solid-color button and the gradient editor's ramp.

## If something becomes unreadable

Lower the ceiling. Raising it darkens more, starting with disabled text — which
is exactly what disappears first.

Settings apply while After Effects is running. Some surfaces keep the previous
palette until After Effects restarts, because it copies those colors into
caches of its own.

If an After Effects update ever makes a panel misbehave with the mod on,
switch off **After Effects interface**, **Direct fills** and **GDI surfaces**
together and restart After Effects. With all three off when the mod loads, it
does not hook Adobe's modules at all.

## Other mods that darken menus

If your menu bar ignores the palette, another mod is painting it first: any mod
that darkens Win32 menus globally answers the same `DefWindowProc` messages
before this one, with its own fixed color. Add `AfterFX.exe` to that mod's
process **exclusion** list and this one takes over.

## Known limitations

**The Home screen changes on restart**, which is why **UXP panels** is off by
default. After Effects reads its files once, when it loads, and the mod
recolors that read — a temporary copy, deleted as soon as it is closed;
nothing on disk changes. So turning the switch on, a palette switch, and
turning it back off or disabling the mod all show there after a restart.

**The dropdown menus are dark, not palette-colored.** The menu bar itself is
painted here, item by item; the menus that drop out of it, and the right-click
menus, are drawn by Windows with the dark menu theme this mod switches on.
Measured on Windows 11 build 26200, the theme those menus draw with never
passes through the entry points the mod watches, so they are dark rather than
the palette. A menu taller than the screen grows a small scroll button at each
end, which Windows paints outside the theme system altogether, so those keep
the light system color.

## Compatibility

The mod looks up each color function by name at startup, installs the ones the
running build exports, and logs the rest, so a version that moved or dropped a
function loses that surface, not the mod. Measured against the installed
builds:

| After Effects | Color functions found | Drawing primitives |
|---------------|-----------------------|--------------------|
| 2026          | 26 of 26              | 5 of 5             |
| 2023          | 7 of 26               | 4 of 5             |

The 2023 toolkit is older and has no skins utilities or `dna` family; what it
does export covers the classic theme and the Spectrum gray ramp, and the brush
factory catches much of the rest. The window frame, the menu bar and the
native dialogs do not depend on the After Effects version at all. Native dark
mode needs Windows 10 build 17763 or newer; below that the mod still themes
the interface and paints the menus itself.

Turn on **Mod logs** in Windhawk's Advanced tab to see what a session got: it
logs `dvaui: N hooks active`, or says the interface layer is not installed
when a build exports none of the functions.

## Questions, bugs and palettes

Bug reports and palette suggestions are welcome on
[Discord](https://discord.gg/m5kVMR8Vuu), where an After Effects build and a
screenshot are usually all it takes to work one out.

## Credits

The menu bar technique — the undocumented `WM_UAHDRAWMENU` messages and the
`DarkMode::Menu` theme class — comes from
[win32-darkmode](https://github.com/adzm/win32-darkmode) by adzm, MIT licensed.
This mod is MIT as well.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- palette: onyx
  $name: Palette
  $description: The set of tones used across the whole interface.
  $options:
  - onyx: Onyx — near black (#050505)
  - abyss: Abyss — absolute black (#000000)
  - graphite: Graphite — dark, a little more legible (#0D0D0D)
  - aftereffects: After Effects — the violet sampled from the app icon
  - comfy: Comfy — warm brown, low contrast for long sessions
  - neon: Neon — near black with magenta
  - glitch: Glitch — acid green with a magenta accent
  - contrast: Contrast — black panels, light dividers, pure white text
  - violet: Violet — a purple interface, not just a purple accent
  - blossom: Blossom — dark rose with pastel pink text and accent
  - ember: Ember — near black with a strong orange
  - amethyst: Amethyst — near black with a strong purple
  - crimson: Crimson — near black with a strong red
  - threshold: Threshold — the threshold-editor.com.br palette
  - miku: Miku — deep teal with a cyan accent
  - custom: Custom — the colors in the group below
- customTheme:
  - base: "#050505"
    $name: Base
    $description: The darkest step — the deepest background.
  - panel: "#090909"
    $name: Panel
    $description: The second step, and the title bar and the menu bar.
  - surface: "#0E0E0E"
    $name: Surface
    $description: The middle step.
  - raised: "#161616"
    $name: Raised
    $description: The fourth step.
  - border: "#242424"
    $name: Border
    $description: The lightest step — dividers and edges.
  - text: "#E6E6E6"
    $name: Text
    $description: Menu and title bar text.
  - accent: "#2E2E2E"
    $name: Accent
    $description: Hovered menu items. Left empty, the border is used.
  - disabledText: "#777777"
    $name: Disabled text
    $description: >-
      Disabled menu items. Left empty, halfway from the text to the panel is
      used.
  - highlight: ""
    $name: Highlight
    $description: >-
      The hue the interface blue — the focused panel, the active tool,
      selections — takes. Left empty, the blue stays.
  $name: Custom theme
  $description: >-
    Used when the palette is Custom. It starts as Onyx. Colors are #RRGGBB, or
    #RRGGBBAA with the alpha ignored; the six required fields fall back to
    Onyx's one at a time when left empty or unreadable, which the log names.
    Themes are shared through this tab's text mode: the log writes the one in
    force as a line to paste into it.
- strength: 100
  $name: Strength
  $description: How much of the palette is applied over the original color, in percent. 100 = palette only.
- ceiling: 28
  $name: Brightness ceiling
  $description: >-
    The highest brightness, in percent, still treated as background and
    darkened. Anything above passes through untouched. The default 28 sits just
    below the #4B4B4B that Spectrum uses for disabled text and dividers —
    raising it starts erasing that text. In the Home screen, borders and fills
    get a little more room than this, since a stylesheet says which is which.
- dvauiHook: true
  $name: After Effects interface
  $description: Intercepts the theme color functions in dvaui.dll. This is the layer that recolors panels, the Timeline and the viewers.
- brushHook: true
  $name: Direct fills
  $description: >-
    Also intercepts the Direct2D brush factory — surfaces painted without
    consulting the theme. Turn this off if a panel paints wrong.
- nativeDarkMode: true
  $name: Window and system dialogs
  $description: Immersive dark mode, title bar, border and native dialogs.
- menuHook: true
  $name: Menu bar and menus
  $description: >-
    Paints the File/Edit/Composition bar in the palette, and turns the menus
    that drop out of it dark instead of white. On Windows 11 those menus take
    Windows' own dark menu style rather than the palette; the readme says why.
- gdiHook: true
  $name: GDI surfaces
  $description: Darkens GDI brushes, pens and text backgrounds created by Adobe's own modules.
- uxpPanels: false
  $name: UXP panels
  $description: >-
    The Home screen, bar across its top included, which After Effects draws
    from stylesheets, scripts and design tokens of its own. Off by default
    because those are read once, when it loads: this switch, a palette change
    and disabling the mod all show there only after After Effects restarts.
- highlight: true
  $name: Palette highlight
  $description: >-
    Gives the interface blue — the focused panel's border, the active tool,
    selections and links — the palette's own hue, each blue at its own
    brightness. Contrast, Violet, Blossom, Ember, Amethyst, Crimson,
    Threshold and Miku carry one, and a custom theme can; the others keep the
    blue.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <dwmapi.h>
#include <uxtheme.h>

#include <windhawk_utils.h>

#include <stdint.h>
#include <algorithm>
#include <atomic>
#include <bit>
#include <cmath>
#include <cstring>
#include <cwchar>
#include <new>
#include <optional>
#include <unordered_map>
#include <utility>
#include <vector>

// ============================================================================
// PALETTE AND SETTINGS
// ============================================================================

struct Palette {
    /*
        The five tones, deepest to lightest. The title bar and the menu bar use
        ramp[1], the panel tone: in a tinted palette ramp[0] reads as black, and
        a black frame around a purple interface looks like it missed the theme.
    */
    COLORREF ramp[5];
    COLORREF text;
    COLORREF dimText;

    /*
        Outside the ramp on purpose: the ramp is interpolated, so a strong color
        in it would bleed into its neighbors and tint half the interface. The
        accent only shows on hovered menu items and system highlights.
    */
    COLORREF accent;

    /*
        The hue the interface blue takes: the focused panel's border, the
        active tool, selections. Each blue keeps its own luminance and takes
        only the hue, so white text on a blue button keeps its contrast.
        CLR_INVALID keeps the blue, as Onyx through Glitch do.
    */
    COLORREF highlight = CLR_INVALID;
};

/*
    The interface blues, as dvaui's color tables and the bundled UXP
    stylesheets hold them: the Spectrum 2 ramp dvaui draws the interface with,
    then the Spectrum 1 values the UXP panels still carry. Matched exactly,
    never by hue: dvaui holds hundreds of blues, and a layer label or a picked
    color that merely looks blue is content, not interface.
*/
constexpr COLORREF kInterfaceBlues[] = {
    // Spectrum 2, dvaui's blue ramp
    RGB(0x00, 0x26, 0x51), RGB(0x00, 0x32, 0x6A), RGB(0x00, 0x40, 0x87),
    RGB(0x00, 0x4E, 0xA6), RGB(0x00, 0x5C, 0xC8), RGB(0x06, 0x6C, 0xE7),
    RGB(0x1D, 0x80, 0xF5), RGB(0x40, 0x96, 0xF3), RGB(0x5E, 0xAA, 0xF7),
    RGB(0x7C, 0xBD, 0xFA), RGB(0x98, 0xCE, 0xFD), RGB(0xB3, 0xDE, 0xFE),
    RGB(0xCE, 0xEA, 0xFF), RGB(0xE3, 0xF3, 0xFF),

    // Spectrum 1, in the UXP stylesheets
    RGB(0x00, 0x41, 0x8A), RGB(0x00, 0x44, 0x91), RGB(0x00, 0x54, 0xB6),
    RGB(0x02, 0x65, 0xDC), RGB(0x09, 0x5A, 0xBA), RGB(0x0D, 0x66, 0xD0),
    RGB(0x14, 0x73, 0xE6), RGB(0x14, 0x7A, 0xF3), RGB(0x26, 0x80, 0xEB),
    RGB(0x34, 0x8F, 0xF4), RGB(0x37, 0x8E, 0xF0), RGB(0x4B, 0x9C, 0xF5),
    RGB(0x54, 0xA3, 0xF6), RGB(0x5A, 0xA9, 0xFA), RGB(0x72, 0xB7, 0xF9),
    RGB(0x8F, 0xCA, 0xFC),

    /*
        After Effects' own, and in no table: the frame count under the
        Timeline's current time, which After Effects derives from that
        timecode's #4096F3 — the same hue and saturation, at a lightness of
        36% instead of 60%. Read off a running After Effects 2026.
    */
    RGB(0x0B, 0x59, 0xAD),
};

constexpr size_t kInterfaceBlueCount = ARRAYSIZE(kInterfaceBlues);

struct Settings {
    Palette palette;
    float strength;  // 0.0 .. 1.0
    float ceiling;   // 0.0 .. 1.0
    bool dvauiHook;
    bool brushHook;
    bool nativeDarkMode;
    bool menuHook;
    bool gdiHook;
    bool uxpPanels;
    bool highlight;  // the palette carries one, and the setting is on

    // What each of kInterfaceBlues becomes, computed with the palette.
    COLORREF highlightShades[kInterfaceBlueCount];
};

/*
    Published whole. LoadSettings fills the next of these copies and swaps the
    pointer, so a hook reads one complete set, never half of two. A hook holds
    its set for one call; its copy is only reused after kSettingsCopies more
    settings changes, each of which also repaints the process.
*/
constexpr size_t kSettingsCopies = 16;

Settings g_settingsCopies[kSettingsCopies];
std::atomic<const Settings*> g_currentSettings{&g_settingsCopies[0]};
size_t g_nextSettingsCopy = 1;  // LoadSettings only

static const Settings& CurrentSettings() {
    return *g_currentSettings.load(std::memory_order_acquire);
}

// ============================================================================
// HELPERS
// ============================================================================

static float ClampFloat(float v, float lo, float hi) {
    return v < lo ? lo : (v > hi ? hi : v);
}

static int ClampInt(int v, int lo, int hi) {
    return v < lo ? lo : (v > hi ? hi : v);
}

static float BlendWith(float strength, float original, float target) {
    return original * (1.0f - strength) + target * strength;
}

// WCAG relative luminance: the measure a contrast ratio is built on.
static float LinearChannel(float c) {
    return c <= 0.04045f ? c / 12.92f : std::pow((c + 0.055f) / 1.055f, 2.4f);
}

static float LuminanceOf(float r, float g, float b) {
    return 0.2126f * LinearChannel(r) + 0.7152f * LinearChannel(g) +
           0.0722f * LinearChannel(b);
}

static float Luminance(COLORREF c) {
    return LuminanceOf(GetRValue(c) / 255.0f, GetGValue(c) / 255.0f,
                       GetBValue(c) / 255.0f);
}

/*
    The tone of `hue` with the given luminance, along black → hue → white: a
    dark target gives a shade of the hue, a light one a tint. Luminance only
    grows along that path, so bisection finds the point.
*/
static COLORREF ShadeWithLuminance(COLORREF hue, float target) {
    const float h[3] = {GetRValue(hue) / 255.0f, GetGValue(hue) / 255.0f,
                        GetBValue(hue) / 255.0f};
    float c[3];

    auto at = [&](float t) {
        for (int i = 0; i < 3; i++) {
            c[i] = t <= 0.5f ? h[i] * t * 2.0f
                             : h[i] + (1.0f - h[i]) * (t * 2.0f - 1.0f);
        }
    };

    float lo = 0.0f;
    float hi = 1.0f;

    for (int step = 0; step < 32; step++) {
        float mid = (lo + hi) / 2.0f;
        at(mid);

        if (LuminanceOf(c[0], c[1], c[2]) < target) {
            lo = mid;
        } else {
            hi = mid;
        }
    }

    at((lo + hi) / 2.0f);

    auto channel = [](float v) {
        return ClampInt(static_cast<int>(v * 255.0f + 0.5f), 0, 255);
    };

    return RGB(channel(c[0]), channel(c[1]), channel(c[2]));
}

/*
    The top log2(Slots) bits of a Fibonacci hash, which are the best mixed.
    Derived from the table size, so that resizing a table cannot leave its
    shift behind.
*/
template <size_t Slots>
static size_t FibonacciIndex(uint64_t value) {
    static_assert(std::has_single_bit(Slots), "table sizes are powers of two");

    return static_cast<size_t>((value * 0x9E3779B97F4A7C15ull) >>
                               (64 - std::countr_zero(Slots)));
}

// True for the first caller only, whichever thread it is on.
static bool Claim(volatile LONG* flag) {
    return InterlockedCompareExchange(flag, TRUE, FALSE) == FALSE;
}

// ============================================================================
// WHERE THE CALL CAME FROM
// ============================================================================

/*
    Which modules count as Adobe UI, kept as address ranges rather than asked
    per call.

    The GDI hooks ask this on every CreateSolidBrush, CreatePen and SetBkColor.
    GetModuleHandleExW would take the loader lock each time, and ThemeSysBrush
    calls CreateSolidBrush while holding g_brushLock, which a thread inside the
    loader could be waiting for the other way round. The set only changes when
    a module is mapped: it is snapshot once at init, then appended to from a
    loader notification.
*/
struct ModuleRange {
    /*
        begin is written once, before the entry is published, and never
        changes again; end is the one field that moves, and zero means the
        module is gone. A reader therefore never sees one module's begin
        paired with another's end, without taking a lock for it.
    */
    uintptr_t begin;
    std::atomic<uintptr_t> end;
};

/*
    After Effects 2026 ships 49 dva* modules, plus AfterFXLib and the
    executable;
    the rest is headroom for versions that add more, and for the plugin and
    codec modules that come and go during a session. A module that unloads
    leaves its entry behind with end cleared, and reloading it at the same
    base — which is what the loader normally does — revives that entry
    instead of taking another.
*/
constexpr size_t kMaxModuleRanges = 512;

ModuleRange g_moduleRanges[kMaxModuleRanges];
volatile LONG g_moduleRangeCount = 0;
volatile LONG g_moduleRangesFullLogged = FALSE;
SRWLOCK g_moduleRangeLock = SRWLOCK_INIT;

/*
    Bumped whenever a range is added or dropped. IsAdobeUICaller caches its
    last hit per thread, and that cache would otherwise outlive the module it
    came from; comparing the epoch throws it away the moment the table moves.
    It starts at 1, so the zero every thread starts with never matches.
*/
volatile LONG g_moduleRangeEpoch = 1;

/*
    Adobe's whole UI toolkit is prefixed dva (dvaui, dvacore, ...).
    AfterFXLib is After Effects' own application layer — the panels, the
    Timeline, the Composition viewer — and draws through that toolkit, so it
    belongs in the same set. The length is passed in because the loader hands
    names over without a terminator.
*/
static bool IsAdobeUIName(const wchar_t* name, size_t length) {
    constexpr wchar_t kLib[] = L"AfterFXLib.dll";
    constexpr size_t kLibLength = ARRAYSIZE(kLib) - 1;

    return (length >= 3 && _wcsnicmp(name, L"dva", 3) == 0) ||
           (length == kLibLength && _wcsnicmp(name, kLib, kLibLength) == 0);
}

/*
    Writers are serialized by g_moduleRangeLock; readers take no lock at all.
    An entry is written in full before the count that exposes it is
    published, so IsAdobeUICaller never sees a half-written range.

    Nothing that can take the loader lock is called while this lock is held.
    OnDllNotification runs with the loader lock held and then takes this one,
    so taking the two in the other order anywhere would deadlock them.
*/
static void AddModuleRange(uintptr_t begin, uintptr_t end) {
    AcquireSRWLockExclusive(&g_moduleRangeLock);

    LONG count = g_moduleRangeCount;
    bool stored = false;

    for (LONG i = 0; i < count && !stored; i++) {
        if (g_moduleRanges[i].begin != begin) {
            continue;
        }

        /*
            Either already known, or an entry whose module unloaded and has
            been mapped over. Writing end is the whole update: begin is
            already this base, and end is what publishes the entry again.
        */
        g_moduleRanges[i].end.store(end, std::memory_order_release);
        InterlockedIncrement(&g_moduleRangeEpoch);
        stored = true;
    }

    if (!stored && count < static_cast<LONG>(kMaxModuleRanges)) {
        g_moduleRanges[count].begin = begin;
        g_moduleRanges[count].end.store(end, std::memory_order_release);
        InterlockedExchange(&g_moduleRangeCount, count + 1);
        InterlockedIncrement(&g_moduleRangeEpoch);
        stored = true;
    }

    ReleaseSRWLockExclusive(&g_moduleRangeLock);

    if (!stored && Claim(&g_moduleRangesFullLogged)) {
        Wh_Log(L"module range table full; GDI calls from Adobe modules loaded "
               L"from now on will not be recognized");
    }
}

/*
    A module that unloads stops counting as Adobe UI: its address range is
    about to be handed to whatever the loader maps there next, and a
    third-party plugin painting from a dead dva module's base must not be
    recolored. Clearing end is a single store, so a reader either sees the
    module or does not.
*/
static void DropModuleRange(uintptr_t begin) {
    AcquireSRWLockExclusive(&g_moduleRangeLock);

    LONG count = g_moduleRangeCount;

    for (LONG i = 0; i < count; i++) {
        if (g_moduleRanges[i].begin != begin ||
            g_moduleRanges[i].end.load(std::memory_order_relaxed) == 0) {
            continue;
        }

        g_moduleRanges[i].end.store(0, std::memory_order_release);
        InterlockedIncrement(&g_moduleRangeEpoch);
        break;
    }

    ReleaseSRWLockExclusive(&g_moduleRangeLock);
}

/*
    Every module mapped or unmapped after init, including the dependencies
    LoadLibraryExW never returns, which is how most dva* modules arrive in
    After Effects. The notification carries the name, base and size, so nothing
    here asks the loader anything. It runs with the loader lock held, and it
    is unregistered in Wh_ModUninit, before the image that contains it goes
    away.
*/
struct LdrUnicodeString {
    USHORT length;  // in bytes, and not terminated
    USHORT maximumLength;
    PWSTR buffer;
};

struct LdrDllLoadedData {
    ULONG flags;
    const LdrUnicodeString* fullDllName;
    const LdrUnicodeString* baseDllName;
    PVOID dllBase;
    ULONG sizeOfImage;
};

constexpr ULONG kLdrDllLoaded = 1;
constexpr ULONG kLdrDllUnloaded = 2;

using LdrDllNotification_t = VOID(CALLBACK*)(ULONG, const LdrDllLoadedData*,
                                             PVOID);
using LdrRegisterDllNotification_t = LONG(NTAPI*)(ULONG, LdrDllNotification_t,
                                                  PVOID, PVOID*);
using LdrUnregisterDllNotification_t = LONG(NTAPI*)(PVOID);

PVOID g_dllNotificationCookie = nullptr;

static VOID CALLBACK OnDllNotification(ULONG reason,
                                       const LdrDllLoadedData* data, PVOID) {
    if ((reason != kLdrDllLoaded && reason != kLdrDllUnloaded) || !data ||
        !data->baseDllName || !data->baseDllName->buffer) {
        return;
    }

    const LdrUnicodeString& name = *data->baseDllName;
    size_t length = name.length / sizeof(wchar_t);

    auto base = reinterpret_cast<uintptr_t>(data->dllBase);
    bool loaded = reason == kLdrDllLoaded;

    if (!IsAdobeUIName(name.buffer, length)) {
        return;
    }

    // Both reasons carry the same fields; unloaded only needs the base.
    if (!loaded) {
        DropModuleRange(base);
        return;
    }

    AddModuleRange(base, base + data->sizeOfImage);
}

static void WatchModuleLoads() {
    HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");

    auto registerNotification =
        ntdll ? reinterpret_cast<LdrRegisterDllNotification_t>(
                    GetProcAddress(ntdll, "LdrRegisterDllNotification"))
              : nullptr;

    if (!registerNotification ||
        registerNotification(0, OnDllNotification, nullptr,
                             &g_dllNotificationCookie) != 0) {
        g_dllNotificationCookie = nullptr;
        Wh_Log(L"could not register for DLL notifications; GDI calls from "
               L"dva modules loaded later will not be recognized");
    }
}

static void StopWatchingModuleLoads() {
    if (!g_dllNotificationCookie) {
        return;
    }

    HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");

    auto unregisterNotification =
        ntdll ? reinterpret_cast<LdrUnregisterDllNotification_t>(
                    GetProcAddress(ntdll, "LdrUnregisterDllNotification"))
              : nullptr;

    /*
        If this ever failed, the callback would stay registered into an image
        that is about to be unmapped, so the status is worth a line even
        though there is nothing left to try.
    */
    if (!unregisterNotification) {
        Wh_Log(L"LdrUnregisterDllNotification is missing; the DLL "
               L"notification could not be unregistered");
    } else {
        LONG status = unregisterNotification(g_dllNotificationCookie);

        if (status != 0) {
            Wh_Log(L"LdrUnregisterDllNotification failed (0x%08X)",
                   static_cast<unsigned>(status));
        }
    }

    g_dllNotificationCookie = nullptr;
}

/*
    The modules already mapped when the mod starts; OnDllNotification covers
    the ones after. It is registered first, so a module mapped in between is
    seen twice and deduplicated rather than missed.

    The list is sized from what the process reports: an Adobe video app runs
    with several hundred modules. Sizes come from K32GetModuleInformation
    rather than from the module's headers, so a module unloaded in between
    costs a failed call, not an access violation.
*/
struct ModuleInformation {  // MODULEINFO
    LPVOID base;
    DWORD size;
    LPVOID entryPoint;
};

using EnumProcessModules_t = BOOL(WINAPI*)(HANDLE, HMODULE*, DWORD, LPDWORD);
using GetModuleInformation_t = BOOL(WINAPI*)(HANDLE, HMODULE, ModuleInformation*,
                                             DWORD);

/*
    The executable and the modules the mod hooks by name, sized from their own
    PE headers. Those stay mapped for the life of the process, so reading their
    headers is safe here, unlike for an arbitrary module out of an enumeration.
*/
static bool ModuleImageRange(HMODULE module, uintptr_t* begin, uintptr_t* end) {
    if (!module) {
        return false;
    }

    auto base = reinterpret_cast<uintptr_t>(module);
    auto dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(base);

    if (dos->e_magic != IMAGE_DOS_SIGNATURE) {
        return false;
    }

    auto nt = reinterpret_cast<const IMAGE_NT_HEADERS*>(base + dos->e_lfanew);

    if (nt->Signature != IMAGE_NT_SIGNATURE) {
        return false;
    }

    *begin = base;
    *end = base + nt->OptionalHeader.SizeOfImage;

    return true;
}

static void NoteModuleFromHeaders(HMODULE module) {
    uintptr_t begin = 0;
    uintptr_t end = 0;

    if (ModuleImageRange(module, &begin, &end)) {
        AddModuleRange(begin, end);
    }
}

static void NoteKnownModules() {
    NoteModuleFromHeaders(GetModuleHandleW(nullptr));
    NoteModuleFromHeaders(GetModuleHandleW(L"dvaui.dll"));
    NoteModuleFromHeaders(GetModuleHandleW(L"dvacore.dll"));
    NoteModuleFromHeaders(GetModuleHandleW(L"AfterFXLib.dll"));
}

/*
    The modules mapped right now. False when the process cannot be enumerated,
    with `error` the reason: zero when the API itself is missing,
    ERROR_OUTOFMEMORY when the list cannot be held, and otherwise whatever the
    call set.

    This is the one allocation on the mod's startup path, and the one place it
    could throw back into Windhawk, so bad_alloc is answered here.
*/
static bool EnumerateProcessModules(std::vector<HMODULE>* modules, DWORD* error) {
    *error = 0;
    modules->clear();

    HMODULE kernel32 = GetModuleHandleW(L"kernel32.dll");

    auto enumModules =
        kernel32 ? reinterpret_cast<EnumProcessModules_t>(
                       GetProcAddress(kernel32, "K32EnumProcessModules"))
                 : nullptr;

    if (!enumModules) {
        return false;
    }

    HANDLE process = GetCurrentProcess();
    DWORD needed = 0;
    DWORD bytes = 0;

    try {
        modules->resize(1024);

        // Modules can load between two calls, so a retry may still come up short.
        for (int attempt = 0; attempt < 3; attempt++) {
            bytes = static_cast<DWORD>(modules->size() * sizeof(HMODULE));

            if (!enumModules(process, modules->data(), bytes, &needed)) {
                *error = GetLastError();
                modules->clear();
                return false;
            }

            if (needed <= bytes) {
                break;
            }

            if (attempt < 2) {
                modules->resize(needed / sizeof(HMODULE) + 64);
            }
        }

        modules->resize(std::min(needed, bytes) / sizeof(HMODULE));
    } catch (const std::bad_alloc&) {
        *error = ERROR_OUTOFMEMORY;
        modules->clear();
        return false;
    }

    return true;
}

static void SnapshotAdobeModules() {
    /*
        These are recorded first, whatever the enumeration below manages. It
        can fail or come up short while After Effects is still mapping modules from
        several threads, and the executable was mapped before the loader
        notification could see it. A module recorded twice is deduplicated.
    */
    NoteKnownModules();

    HMODULE kernel32 = GetModuleHandleW(L"kernel32.dll");

    auto moduleInformation =
        kernel32 ? reinterpret_cast<GetModuleInformation_t>(
                       GetProcAddress(kernel32, "K32GetModuleInformation"))
                 : nullptr;

    std::vector<HMODULE> modules;
    DWORD error = 0;

    /*
        Short of memory, or with no way to ask, the modules noted above are all
        the mod knows about — the same degradation either way.
    */
    if (!moduleInformation || !EnumerateProcessModules(&modules, &error)) {
        Wh_Log(L"cannot enumerate modules (%u); besides the executable, dvaui, "
               L"dvacore and AfterFXLib, only dva modules loaded from now on "
               L"will be recognized as Adobe UI",
               error);
        return;
    }

    HANDLE process = GetCurrentProcess();
    HMODULE executable = GetModuleHandleW(nullptr);

    for (HMODULE module : modules) {
        bool adobe = module == executable;

        if (!adobe) {
            wchar_t path[MAX_PATH]{};

            if (!GetModuleFileNameW(module, path, ARRAYSIZE(path))) {
                continue;  // unloaded since the enumeration
            }

            const wchar_t* name = wcsrchr(path, L'\\');
            name = name ? name + 1 : path;

            adobe = IsAdobeUIName(name, wcslen(name));
        }

        ModuleInformation info{};

        if (adobe && moduleInformation(process, module, &info, sizeof(info))) {
            auto base = reinterpret_cast<uintptr_t>(info.base);
            AddModuleRange(base, base + info.size);
        }
    }
}

/*
    The range of this thread's last match: consecutive calls mostly come from
    one module. The epoch it was taken under comes with it, so the cache is
    dropped the moment any module is added or unloaded rather than outliving
    the module it names.
*/
thread_local uintptr_t g_lastAdobeBegin = 0;
thread_local uintptr_t g_lastAdobeEnd = 0;
thread_local LONG g_lastAdobeEpoch = 0;

static bool IsAdobeUICaller(void* caller) {
    auto p = reinterpret_cast<uintptr_t>(caller);
    LONG epoch = g_moduleRangeEpoch;

    // Read before the scan, so a change during it only costs the next call.
    if (epoch == g_lastAdobeEpoch && p >= g_lastAdobeBegin && p < g_lastAdobeEnd) {
        return true;
    }

    LONG count = g_moduleRangeCount;

    for (LONG i = 0; i < count; i++) {
        uintptr_t end = g_moduleRanges[i].end.load(std::memory_order_acquire);

        if (end == 0) {
            continue;  // unloaded
        }

        uintptr_t begin = g_moduleRanges[i].begin;

        if (p >= begin && p < end) {
            g_lastAdobeBegin = begin;
            g_lastAdobeEnd = end;
            g_lastAdobeEpoch = epoch;
            return true;
        }
    }

    return false;
}

// ============================================================================
// COLOR DECISION
// ============================================================================

/*
    Interpolated, not stepped: each step of Spectrum's gray scale is how a
    panel is told from the panel behind it, and fixed bands would collapse
    neighboring steps into one tone. Whatever was one step lighter stays one
    step lighter, inside a much darker range.
*/
static COLORREF PickTarget(const Settings& s, float brightness) {
    const COLORREF* ramp = s.palette.ramp;

    float t = ClampFloat(brightness / s.ceiling, 0.0f, 1.0f);

    float pos = t * 4.0f;  // five stops -> four intervals

    int i = static_cast<int>(pos);
    if (i > 3) {
        i = 3;
    }

    float f = pos - static_cast<float>(i);

    auto mix = [&](int channelShift) {
        float a = static_cast<float>((ramp[i] >> channelShift) & 0xFF);
        float b = static_cast<float>((ramp[i + 1] >> channelShift) & 0xFF);
        return a + (b - a) * f;
    };

    int r = ClampInt(static_cast<int>(mix(0) + 0.5f), 0, 255);
    int g = ClampInt(static_cast<int>(mix(8) + 0.5f), 0, 255);
    int b = ClampInt(static_cast<int>(mix(16) + 0.5f), 0, 255);

    return RGB(r, g, b);
}

/*
    Saturated colors pass through untouched: selection, timeline clips, labels,
    warnings, color swatches. Darkening those would not make the interface darker —
    it would make the interface wrong.
*/
static bool IsNeutral(float r, float g, float b, float tolerance) {
    float hi = r > g ? (r > b ? r : b) : (g > b ? g : b);
    float lo = r < g ? (r < b ? r : b) : (g < b ? g : b);

    return (hi - lo) <= tolerance;
}

// ============================================================================
// DVAUI COLORS
// ============================================================================

struct DvaColorRGBA {
    float r;
    float g;
    float b;
    float a;
};

static bool IsSaneChannel(float v) {
    return std::isfinite(v) && v >= -0.05f && v <= 1.5f;
}

/*
    Which of kInterfaceBlues `in` is, or -1, matched on 8-bit channels — the
    precision dvaui's tables hold them in. Blue has to be the largest channel
    first, which turns every other color away before the list is read.
*/
static int InterfaceBlueIndex(const DvaColorRGBA& in) {
    if (!(in.b > in.r + 0.1f && in.b > in.g)) {
        return -1;
    }

    auto channel = [](float v) {
        return ClampInt(static_cast<int>(v * 255.0f + 0.5f), 0, 255);
    };

    COLORREF c = RGB(channel(in.r), channel(in.g), channel(in.b));

    for (size_t i = 0; i < kInterfaceBlueCount; i++) {
        if (kInterfaceBlues[i] == c) {
            return static_cast<int>(i);
        }
    }

    return -1;
}

static int HighlightIndex(const Settings& s, const DvaColorRGBA& in) {
    return s.highlight ? InterfaceBlueIndex(in) : -1;
}

/*
    dvaui::drawbot::ColorRGBA is four floats in RGBA order, which is how Adobe's
    own ASLColorToDVAColorRGBA writes it. If a future After Effects changes the
    struct, the values read here stop looking like a color, and the original is
    returned rather than garbage painted.

    Two kinds of color are converted: a dark neutral gray and, on a palette
    with a highlight, one of the interface's own blues. Every other saturated color
    passes.
*/
static bool ShouldConvertWith(const Settings& s, const DvaColorRGBA& in) {
    if (!IsSaneChannel(in.r) || !IsSaneChannel(in.g) || !IsSaneChannel(in.b) ||
        !IsSaneChannel(in.a)) {
        return false;
    }

    if (IsNeutral(in.r, in.g, in.b, 0.035f)) {
        return (in.r + in.g + in.b) / 3.0f <= s.ceiling;
    }

    return HighlightIndex(s, in) >= 0;
}

/*
    The tone `in` becomes: a ramp stop for a gray, the highlight shade for a
    blue. The caller passes one settings snapshot for the whole conversion, so
    a palette change cannot pair one palette's test with another's shades or
    strength. No side effects, so the stylesheet rewrite shares it.
*/
static bool PaletteTarget(const Settings& s, const DvaColorRGBA& in, COLORREF* target,
                          int* blue) {
    if (!ShouldConvertWith(s, in)) {
        return false;
    }

    *blue = HighlightIndex(s, in);
    *target = *blue >= 0 ? s.highlightShades[*blue]
                         : PickTarget(s, (in.r + in.g + in.b) / 3.0f);

    return true;
}

// One bit per kInterfaceBlues entry a hook recolored, logged at unload.
volatile LONG64 g_bluesRecolored = 0;

static_assert(kInterfaceBlueCount <= 64,
              "one bit of g_bluesRecolored per interface blue");

/*
    The caller passes the snapshot it already took, for the same reason
    PaletteTarget does: one decision is made with one set of settings.
*/
static bool ConvertDvaColorWith(const Settings& s, const DvaColorRGBA& in,
                                DvaColorRGBA* out) {
    COLORREF target = 0;
    int blue = -1;

    if (!PaletteTarget(s, in, &target, &blue)) {
        return false;
    }

    if (blue >= 0) {
        LONG64 bit = LONG64{1} << blue;

        if (!(g_bluesRecolored & bit)) {
            InterlockedOr64(&g_bluesRecolored, bit);
        }
    }

    out->r = BlendWith(s.strength, in.r, GetRValue(target) / 255.0f);
    out->g = BlendWith(s.strength, in.g, GetGValue(target) / 255.0f);
    out->b = BlendWith(s.strength, in.b, GetBValue(target) / 255.0f);
    out->a = in.a;

    return true;
}

static bool ConvertDvaColor(const DvaColorRGBA& in, DvaColorRGBA* out) {
    return ConvertDvaColorWith(CurrentSettings(), in, out);
}

// ============================================================================
// CONVERTED COLOR TABLE
// ============================================================================

/*
    The theme functions return `const ColorRGBA&`, a reference to a color the
    theme owns and Adobe's code may keep, so the conversion returns a stable
    reference too. A single reused buffer would make
    `Draw(GetColor(A), GetColor(B))` paint both in the same color.

    The key is the address of the theme's color. That covers functions with
    different signatures — `GetGrayColor(enum)`, `GetColor(ImmutableString)`,
    `GrayBackgroundColor()` — without understanding their parameters, and the
    theme's own storage is what keeps an address meaning one color. If the
    theme changes the color behind an address, comparing against `src` notices.
*/

/*
    A Premiere 2026 session with a project open used 12 slots, logged at
    unload, and After Effects asks the same theme: the key is an address the
    theme hands out, and the theme holds few color objects. 128 is a tenfold
    margin, and the count is logged at unload here too. If the table ever fills,
    StoreColor logs it once and the caller keeps the original color.
*/
constexpr size_t kSlotCount = 128;  // power of two, so the mask is a single &

struct ColorSlot {
    volatile LONG state;       // 0 free, 1 being filled, 2 ready
    volatile LONG generation;  // the settings generation dst was computed under
    uintptr_t key;
    DvaColorRGBA src;
    DvaColorRGBA dst;
};

/*
    Deliberately heap-allocated and deliberately leaked.

    After Effects keeps the pointers this table hands out. A static array would
    live in the mod image, which Windhawk unmaps on unload, and those
    references would then point at unmapped memory. So one table of about
    6 KB is left behind each time the mod is disabled or updated; a settings
    change does not unload it. The GetSysColorBrush brushes make the same
    trade.
*/
ColorSlot* g_slots = nullptr;
constexpr size_t kSlotBytes = kSlotCount * sizeof(ColorSlot);

static bool AllocateSlots() {
    if (g_slots) {
        return true;
    }

    g_slots = static_cast<ColorSlot*>(
        HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, kSlotBytes));

    return g_slots != nullptr;
}

static bool SameColor(const DvaColorRGBA& a, const DvaColorRGBA& b) {
    return a.r == b.r && a.g == b.g && a.b == b.b && a.a == b.a;
}

/*
    The address returned by this table is what tells "already converted" from
    "original" when one hook calls another — see ConvertColorRef.
*/
static bool IsOurSlot(const void* address) {
    if (!g_slots) {
        return false;
    }

    const char* p = reinterpret_cast<const char*>(address);
    const char* begin = reinterpret_cast<const char*>(g_slots);

    return p >= begin && p < begin + kSlotBytes;
}

/*
    Whether an address is a color this table actually converted.

    A slot whose setting is off holds its original color again (see
    RefreshSlot), and that color is raw: the brush layer has to treat it like
    any other. Skipping every slot address instead would leave whatever
    After Effects still holds from the table stock gray once "After Effects
    interface" is
    switched off, even with "Direct fills" on.
*/
static bool IsConvertedSlot(const void* address) {
    if (!IsOurSlot(address)) {
        return false;
    }

    size_t offset = static_cast<size_t>(reinterpret_cast<const char*>(address) -
                                        reinterpret_cast<const char*>(g_slots));
    const ColorSlot& slot = g_slots[offset / sizeof(ColorSlot)];

    // Only &slot.dst is ever handed out; anything else in here is left alone.
    if (address != &slot.dst) {
        return true;
    }

    return !SameColor(slot.dst, slot.src);
}

/*
    Bumped on every settings change, after the new settings are in place.

    A color another thread was converting at the moment of the change was
    computed under the old settings, and After Effects may keep the slot it lands
    in without ever asking again. So a write computed under an older
    generation never replaces a slot the change already recomputed, and a
    slot that was still being filled when the change ran is recomputed by the
    thread filling it (see StoreColor). Neither can leave a color After
    Effects holds on the previous palette.
*/
volatile LONG g_generation = 0;

/*
    Recomputes one slot's dst from its src under the settings in force now.
    With restoreOriginals, or with "After Effects interface" off, dst goes back to
    src.
*/
static void RefreshSlot(ColorSlot& slot, LONG generation, bool restoreOriginals) {
    const Settings& s = CurrentSettings();

    // ConvertDvaColorWith leaves dst untouched when it declines.
    DvaColorRGBA dst = slot.src;

    if (!restoreOriginals && s.dvauiHook) {
        ConvertDvaColorWith(s, slot.src, &dst);
    }

    slot.dst = dst;
    slot.generation = generation;
}

volatile LONG g_slotsFullLogged = FALSE;

static const DvaColorRGBA* StoreColor(uintptr_t key, const DvaColorRGBA& src,
                                      const DvaColorRGBA& dst, LONG generation) {
    if (!g_slots) {
        return nullptr;
    }

    size_t index = FibonacciIndex<kSlotCount>(key >> 4);

    for (size_t probe = 0; probe < 64; probe++) {
        ColorSlot& slot = g_slots[(index + probe) & (kSlotCount - 1)];

        LONG state = InterlockedCompareExchange(&slot.state, 0, 0);

        if (state == 0) {
            if (InterlockedCompareExchange(&slot.state, 1, 0) == 0) {
                slot.key = key;
                slot.src = src;
                slot.dst = dst;
                slot.generation = generation;

                InterlockedExchange(&slot.state, 2);

                /*
                    RecomputeColorTable skips a slot while it is being filled.
                    If the settings changed in the meantime, dst was computed
                    under the old ones and nothing else would revisit it.
                */
                LONG now = g_generation;

                if (now != generation) {
                    RefreshSlot(slot, now, false);
                }

                return &slot.dst;
            }

            // Another thread claimed the slot between the read and the exchange.
            state = InterlockedCompareExchange(&slot.state, 0, 0);
        }

        /*
            state == 1 means another thread is still filling this slot. We do not wait:
            holding up a drawing thread over one color costs more than the color. Probing
            continues and, at worst, the same key gets a second slot — a harmless duplicate,
            because both slots converge on the same value and both addresses are stable.
        */
        if (state != 2 || slot.key != key) {
            continue;
        }

        LONG current = slot.generation;

        // Computed under older settings than the slot already holds.
        if (generation < current) {
            return &slot.dst;
        }

        /*
            The address was reused, the theme changed the color behind it, or
            the settings changed since dst was computed. These are plain
            writes, and a drawing thread may be reading dst at the same moment;
            the worst case is one frame painted in a color that is half the
            old one and half the new.
        */
        if (current != generation || !SameColor(slot.src, src)) {
            slot.src = src;
            slot.dst = dst;
            slot.generation = generation;
        }

        return &slot.dst;
    }

    // Saturated: the original color beats a wrong one.
    if (Claim(&g_slotsFullLogged)) {
        Wh_Log(L"color table full; further interface colors keep After "
               L"Effects' own value");
    }

    return nullptr;
}

/*
    Second line of defense against converting a color twice, by value.

    A converted color comes back when Adobe copies what a theme function
    returned — into a temporary, or into a COLORREF — and builds a brush from
    the copy, and when one hooked call makes another with the color it was
    given. Both happen on the same thread within the same paint, so each
    thread remembers only the last few colors it produced, forgets them at the
    top of every dvaui paint, and lets each go after a quarter of a second
    anyway — the GDI and menu paths run outside any dvaui paint.

    Remembering them for good would be wrong: a value the mod produces is also
    a real interface gray — Onyx turns #3F3F3F into #1D1D1D, Spectrum's
    stock panel gray — and every later fill of it would pass unconverted. The
    key is 8-bit RGB, so it survives a round trip through a COLORREF.
*/
constexpr int kRecentProduced = 16;
constexpr ULONGLONG kRecentProducedMs = 250;

thread_local LONG g_recentProduced[kRecentProduced];
thread_local ULONGLONG g_recentProducedAt[kRecentProduced];
thread_local int g_recentNext = 0;

static LONG PackColorKey(const DvaColorRGBA& c) {
    int r = ClampInt(static_cast<int>(c.r * 255.0f + 0.5f), 0, 255);
    int g = ClampInt(static_cast<int>(c.g * 255.0f + 0.5f), 0, 255);
    int b = ClampInt(static_cast<int>(c.b * 255.0f + 0.5f), 0, 255);

    // +1 because 0 means an empty entry.
    return static_cast<LONG>((r << 16) | (g << 8) | b) + 1;
}

static void RememberProduced(const DvaColorRGBA& c) {
    g_recentProduced[g_recentNext] = PackColorKey(c);
    g_recentProducedAt[g_recentNext] = GetTickCount64();
    g_recentNext = (g_recentNext + 1) % kRecentProduced;
}

static bool IsProducedColor(const DvaColorRGBA& c) {
    LONG key = PackColorKey(c);
    ULONGLONG now = GetTickCount64();

    for (int i = 0; i < kRecentProduced; i++) {
        if (g_recentProduced[i] == key &&
            now - g_recentProducedAt[i] < kRecentProducedMs) {
            return true;
        }
    }

    return false;
}

static void ForgetRecentProduced() {
    for (LONG& recent : g_recentProduced) {
        recent = 0;
    }
}

static const DvaColorRGBA* ConvertColorRef(const DvaColorRGBA* original) {
    // Read before the settings are, so a change in between is caught as stale.
    LONG generation = g_generation;

    /*
        A color is a pointer into After Effects' own data, so anything in the
        first page or misaligned for a float is not one and goes back
        untouched. That covers null as well, and costs two instructions: every
        function hooked here was found by name, where the value is always a
        color, and reading sixteen bytes from anything else would fault inside
        After Effects — the one failure this design is careful never to
        produce.
    */
    auto address = reinterpret_cast<uintptr_t>(original);

    if (address < 0x10000 || (address & (alignof(DvaColorRGBA) - 1)) != 0) {
        return original;
    }

    if (!CurrentSettings().dvauiHook || IsOurSlot(original)) {
        return original;
    }

    DvaColorRGBA converted{};

    if (!ConvertDvaColor(*original, &converted)) {
        return original;
    }

    const DvaColorRGBA* stored = StoreColor(reinterpret_cast<uintptr_t>(original),
                                            *original, converted, generation);

    if (!stored) {
        return original;
    }

    // What After Effects now holds, in case it copies it into a brush this paint.
    RememberProduced(*stored);

    return stored;
}

/*
    Brings every color already handed out in line with the current settings,
    in place, so the references After Effects holds follow a settings change. With
    restoreOriginals — on unload — every slot goes back to the color it
    replaced, since the table outlives the mod.

    A slot another thread is filling (state 1) is skipped; that thread brings
    it up to date when it publishes it — see StoreColor. Returns how many
    slots are in use.
*/
static size_t RecomputeColorTable(bool restoreOriginals) {
    if (!g_slots) {
        return 0;
    }

    LONG generation = g_generation;
    size_t used = 0;

    for (size_t i = 0; i < kSlotCount; i++) {
        ColorSlot& slot = g_slots[i];

        if (InterlockedCompareExchange(&slot.state, 0, 0) != 2) {
            continue;
        }

        used++;

        RefreshSlot(slot, generation, restoreOriginals);
    }

    return used;
}

// ============================================================================
// DVAUI HOOKS
// ============================================================================

/*
    One hook template covers every color function.

    On Windows x86-64 there is a single calling convention: the first four integer
    or pointer arguments go in RCX, RDX, R8 and R9, and a method's `this` is just
    the first of them. Since the template only forwards the arguments without
    looking at them, the same function covers `GetGrayColor(enum)`,
    `Theme::GetColor(this, id)` and `GrayBackgroundColor(this)` without knowing any
    of the types.

    The limit is four: the list below only contains functions with at most four
    arguments, precisely so no argument has to come off the stack.
*/
using GenericColorFn = const DvaColorRGBA* (*)(uintptr_t, uintptr_t, uintptr_t,
                                               uintptr_t);

template <size_t I>
struct ColorHook {
    static inline GenericColorFn original = nullptr;

    static const DvaColorRGBA* Hook(uintptr_t a, uintptr_t b, uintptr_t c,
                                    uintptr_t d) {
        return ConvertColorRef(original(a, b, c, d));
    }
};

struct ColorSymbol {
    const char* mangled;
    const wchar_t* label;
    const char* before2026 = nullptr;  // the same function's earlier name
};

/*
    Not every After Effects version exports everything: 2026 exports all of
    them, while 2023's dvaui is older and has only the classic theme and
    ui::GetGrayColor — no `dna` family, no skins utilities. Whatever is missing
    is logged and the rest carries on.

    Fourteen of them were renamed in the 2026 toolkit, when Adobe swapped
    boost::intrusive_ptr for dvacore's own IntrusivePtr in their signatures.
    That changes the mangled name and nothing else, so those entries carry the
    earlier name too, and each version exports one of the two.
*/
static const ColorSymbol kColorSymbols[] = {
    // --- classic theme ---
    {"?GetColor@Theme@ui@dvaui@@UEBAAEBVColorRGBA@drawbot@3@_K@Z",
     L"Theme::GetColor"},
    {"?GetColor@ThemeClient@ui@dvaui@@UEBAAEBVColorRGBA@drawbot@3@_K@Z",
     L"ThemeClient::GetColor"},
    {"?GetThemeColor@ThemeClient@ui@dvaui@@UEBAAEBVColorRGBA@drawbot@3@_K@Z",
     L"ThemeClient::GetThemeColor"},
    {"?GetColor@ui@dvaui@@YAAEBVColorRGBA@drawbot@2@_K@Z", L"ui::GetColor"},

    // --- Spectrum gray ramp: the bulk of the interface ---
    {"?GetGrayColor@ui@dvaui@@YAAEBVColorRGBA@drawbot@2@W4SpectrumGrayColor@12@@Z",
     L"ui::GetGrayColor"},
    {"?GetGrayColor@Theme@ui@dvaui@@QEBAAEBVColorRGBA@drawbot@3@"
     "W4SpectrumGrayColor@23@@Z",
     L"Theme::GetGrayColor"},
    {"?GetSpectrumGrayColor@utilities@skins@dvaui@@YAAEBVColorRGBA@drawbot@3@"
     "PEBVThemeProvider@ui@3@W4SpectrumGrayColor@73@@Z",
     L"skins::GetSpectrumGrayColor"},
    {"?GetSpectrumColor@dna@dvaui@@YAAEBVColorRGBA@drawbot@2@W4SpectrumColor@12@@Z",
     L"dna::GetSpectrumColor"},
    {"?GetSpectrumColor@design@skins@dvaui@@YAAEBVColorRGBA@drawbot@3@"
     "W4SpectrumColor@23@@Z",
     L"design::GetSpectrumColor"},

    // --- backgrounds ---
    {"?GetApplicationBackgroundColor@utilities@skins@dvaui@@YAAEBVColorRGBA@"
     "drawbot@3@PEBVThemeProvider@ui@3@V?$IntrusivePtr@VSkinSet@core@skins@dvaui@"
     "@@RefCountedInterface@utility@dvacore@@@Z",
     L"GetApplicationBackgroundColor",
     "?GetApplicationBackgroundColor@utilities@skins@dvaui@@YAAEBVColorRGBA@"
     "drawbot@3@PEBVThemeProvider@ui@3@V?$intrusive_ptr@VSkinSet@core@skins@"
     "dvaui@@@boost@@@Z"},
    {"?GetContentBackgroundColor@utilities@skins@dvaui@@YAAEBVColorRGBA@drawbot@"
     "3@PEBVThemeProvider@ui@3@V?$IntrusivePtr@VSkinSet@core@skins@dvaui@@"
     "@RefCountedInterface@utility@dvacore@@@Z",
     L"GetContentBackgroundColor",
     "?GetContentBackgroundColor@utilities@skins@dvaui@@YAAEBVColorRGBA@draw"
     "bot@3@PEBVThemeProvider@ui@3@V?$intrusive_ptr@VSkinSet@core@skins@dvau"
     "i@@@boost@@@Z"},
    {"?GetListBoxBackgroundColor@utilities@skins@dvaui@@YAAEBVColorRGBA@drawbot@"
     "3@PEBVThemeProvider@ui@3@V?$IntrusivePtr@VSkinSet@core@skins@dvaui@@"
     "@RefCountedInterface@utility@dvacore@@@Z",
     L"GetListBoxBackgroundColor",
     "?GetListBoxBackgroundColor@utilities@skins@dvaui@@YAAEBVColorRGBA@draw"
     "bot@3@PEBVThemeProvider@ui@3@V?$intrusive_ptr@VSkinSet@core@skins@dvau"
     "i@@@boost@@@Z"},
    {"?GetHoverBackgroundColor@utilities@skins@dvaui@@YAAEBVColorRGBA@drawbot@3@"
     "PEBVThemeProvider@ui@3@V?$IntrusivePtr@VSkinSet@core@skins@dvaui@@"
     "@RefCountedInterface@utility@dvacore@@@Z",
     L"GetHoverBackgroundColor",
     "?GetHoverBackgroundColor@utilities@skins@dvaui@@YAAEBVColorRGBA@drawbo"
     "t@3@PEBVThemeProvider@ui@3@V?$intrusive_ptr@VSkinSet@core@skins@dvaui@"
     "@@boost@@@Z"},
    {"?GetTabBackgroundColor@utilities@skins@dvaui@@YAAEBVColorRGBA@drawbot@3@"
     "PEBVThemeProvider@ui@3@V?$IntrusivePtr@VSkinSet@core@skins@dvaui@@"
     "@RefCountedInterface@utility@dvacore@@@Z",
     L"GetTabBackgroundColor",
     "?GetTabBackgroundColor@utilities@skins@dvaui@@YAAEBVColorRGBA@drawbot@"
     "3@PEBVThemeProvider@ui@3@V?$intrusive_ptr@VSkinSet@core@skins@dvaui@@@"
     "boost@@@Z"},
    {"?GetThumbnailBackgroundColor@utilities@skins@dvaui@@YAAEBVColorRGBA@"
     "drawbot@3@PEBVThemeProvider@ui@3@V?$IntrusivePtr@VSkinSet@core@skins@dvaui@"
     "@@RefCountedInterface@utility@dvacore@@@Z",
     L"GetThumbnailBackgroundColor",
     "?GetThumbnailBackgroundColor@utilities@skins@dvaui@@YAAEBVColorRGBA@dr"
     "awbot@3@PEBVThemeProvider@ui@3@V?$intrusive_ptr@VSkinSet@core@skins@dv"
     "aui@@@boost@@@Z"},

    // --- DNA ---
    {"?GrayBackgroundColor@GrayBackgroundColorDNA@dna@dvaui@@QEBAAEBVColorRGBA@"
     "drawbot@3@XZ",
     L"dna::GrayBackgroundColor"},
    {"?GrayBorderColor@GrayBorderColorDNA@dna@dvaui@@QEBAAEBVColorRGBA@drawbot@3@"
     "XZ",
     L"dna::GrayBorderColor"},
    {"?GetColor@ApplicationDNAModel@ui@dvaui@@UEBAAEBVColorRGBA@drawbot@3@"
     "AEBVImmutableString@utility@dvacore@@@Z",
     L"ApplicationDNAModel::GetColor"},

    // --- controls, dividers and scrollbars ---
    {"?GetDefaultControlColor@utilities@skins@dvaui@@YAAEBVColorRGBA@drawbot@3@"
     "PEBVThemeProvider@ui@3@V?$IntrusivePtr@VSkinSet@core@skins@dvaui@@"
     "@RefCountedInterface@utility@dvacore@@_N@Z",
     L"GetDefaultControlColor",
     "?GetDefaultControlColor@utilities@skins@dvaui@@YAAEBVColorRGBA@drawbot"
     "@3@PEBVThemeProvider@ui@3@V?$intrusive_ptr@VSkinSet@core@skins@dvaui@@"
     "@boost@@_N@Z"},
    {"?GetInteractiveControlColor@utilities@skins@dvaui@@YAAEBVColorRGBA@drawbot@"
     "3@PEBVThemeProvider@ui@3@V?$IntrusivePtr@VSkinSet@core@skins@dvaui@@"
     "@RefCountedInterface@utility@dvacore@@@Z",
     L"GetInteractiveControlColor",
     "?GetInteractiveControlColor@utilities@skins@dvaui@@YAAEBVColorRGBA@dra"
     "wbot@3@PEBVThemeProvider@ui@3@V?$intrusive_ptr@VSkinSet@core@skins@dva"
     "ui@@@boost@@@Z"},
    {"?GetDividerColor@utilities@skins@dvaui@@YAAEBVColorRGBA@drawbot@3@"
     "PEBVThemeProvider@ui@3@V?$IntrusivePtr@VSkinSet@core@skins@dvaui@@"
     "@RefCountedInterface@utility@dvacore@@@Z",
     L"GetDividerColor",
     "?GetDividerColor@utilities@skins@dvaui@@YAAEBVColorRGBA@drawbot@3@PEBV"
     "ThemeProvider@ui@3@V?$intrusive_ptr@VSkinSet@core@skins@dvaui@@@boost@"
     "@@Z"},
    {"?GetListBoxBorderColor@utilities@skins@dvaui@@YAAEBVColorRGBA@drawbot@3@"
     "PEBVThemeProvider@ui@3@V?$IntrusivePtr@VSkinSet@core@skins@dvaui@@"
     "@RefCountedInterface@utility@dvacore@@@Z",
     L"GetListBoxBorderColor",
     "?GetListBoxBorderColor@utilities@skins@dvaui@@YAAEBVColorRGBA@drawbot@"
     "3@PEBVThemeProvider@ui@3@V?$intrusive_ptr@VSkinSet@core@skins@dvaui@@@"
     "boost@@@Z"},
    {"?GetFieldBorderColor@utilities@skins@dvaui@@YAAEBVColorRGBA@drawbot@3@"
     "PEBVThemeProvider@ui@3@V?$IntrusivePtr@VSkinSet@core@skins@dvaui@@"
     "@RefCountedInterface@utility@dvacore@@@Z",
     L"GetFieldBorderColor",
     "?GetFieldBorderColor@utilities@skins@dvaui@@YAAEBVColorRGBA@drawbot@3@"
     "PEBVThemeProvider@ui@3@V?$intrusive_ptr@VSkinSet@core@skins@dvaui@@@bo"
     "ost@@@Z"},
    {"?GetScrollBarThumbColor@utilities@skins@dvaui@@YAAEBVColorRGBA@drawbot@3@"
     "PEBVThemeProvider@ui@3@V?$IntrusivePtr@VSkinSet@core@skins@dvaui@@"
     "@RefCountedInterface@utility@dvacore@@_N@Z",
     L"GetScrollBarThumbColor",
     "?GetScrollBarThumbColor@utilities@skins@dvaui@@YAAEBVColorRGBA@drawbot"
     "@3@PEBVThemeProvider@ui@3@V?$intrusive_ptr@VSkinSet@core@skins@dvaui@@"
     "@boost@@_N@Z"},
    {"?GetScrollBarTrackColor@utilities@skins@dvaui@@YAAEBVColorRGBA@drawbot@3@"
     "PEBVThemeProvider@ui@3@V?$IntrusivePtr@VSkinSet@core@skins@dvaui@@"
     "@RefCountedInterface@utility@dvacore@@_N@Z",
     L"GetScrollBarTrackColor",
     "?GetScrollBarTrackColor@utilities@skins@dvaui@@YAAEBVColorRGBA@drawbot"
     "@3@PEBVThemeProvider@ui@3@V?$intrusive_ptr@VSkinSet@core@skins@dvaui@@"
     "@boost@@_N@Z"},
    {"?GetWidgetColor@utilities@skins@dvaui@@YAAEBVColorRGBA@drawbot@3@"
     "PEBVThemeProvider@ui@3@V?$IntrusivePtr@VSkinSet@core@skins@dvaui@@"
     "@RefCountedInterface@utility@dvacore@@_N@Z",
     L"GetWidgetColor",
     "?GetWidgetColor@utilities@skins@dvaui@@YAAEBVColorRGBA@drawbot@3@PEBVT"
     "hemeProvider@ui@3@V?$intrusive_ptr@VSkinSet@core@skins@dvaui@@@boost@@"
     "_N@Z"},
};

constexpr size_t kColorSymbolCount = ARRAYSIZE(kColorSymbols);

/*
    Every hook into Adobe's modules goes in through here: looked up by its
    mangled name, registered, and logged when the running version lacks it.
*/
struct HookSpec {
    const char* mangled;
    void* hook;
    void** original;
    const wchar_t* label;
};

// What the startup log reports for one module. Each module fills its own.
struct HookCount {
    int installed = 0;
    int missing = 0;
};

static void InstallHookAt(void* proc, const HookSpec& spec, HookCount* count) {
    bool installed = false;

    if (!proc) {
        Wh_Log(L"absent in this version: %s", spec.label);
    } else if (!Wh_SetFunctionHook(proc, spec.hook, spec.original)) {
        Wh_Log(L"failed to hook %s", spec.label);
    } else {
        installed = true;
    }

    if (count) {
        (installed ? count->installed : count->missing)++;
    }
}

static void InstallHook(HMODULE module, const HookSpec& spec, HookCount* count) {
    InstallHookAt(reinterpret_cast<void*>(GetProcAddress(module, spec.mangled)),
                  spec, count);
}

template <size_t N>
static void InstallHooks(HMODULE module, const HookSpec (&specs)[N],
                         HookCount* count) {
    for (const HookSpec& spec : specs) {
        InstallHook(module, spec, count);
    }
}

static void InstallOneColorHook(HMODULE dvaui, size_t index, void* hook,
                                void** original, HookCount& count) {
    const ColorSymbol& sym = kColorSymbols[index];

    // A version exports one of the two names, so a function is only counted
    // and logged as absent when it has neither.
    const char* mangled = sym.mangled;

    if (sym.before2026 && !GetProcAddress(dvaui, mangled)) {
        mangled = sym.before2026;
    }

    InstallHook(dvaui, {mangled, hook, original, sym.label}, &count);
}

template <size_t... I>
static void InstallColorHooksImpl(HMODULE dvaui, HookCount& count,
                                  std::index_sequence<I...>) {
    (InstallOneColorHook(dvaui, I, reinterpret_cast<void*>(&ColorHook<I>::Hook),
                         reinterpret_cast<void**>(&ColorHook<I>::original), count),
     ...);
}

// ============================================================================
// DRAWBOT BRUSHES: THE POINT EVERY FILL PASSES THROUGH
// ============================================================================

/*
    Much of the viewer and Timeline chrome is painted without asking the
    theme, so the Direct2D brush factory that most solid fills pass through is
    hooked as well:

        dvaui::drawbot::d2d::OSSupplier::NewBrush(const ColorRGBA&)

    The color is read while the brush is built, so a stack temporary is safe.
    The SVG sibling is left out: it paints icons, and a darkened icon
    disappears.
*/

using NewBrush_t = void*(*)(void*, const DvaColorRGBA*);
using NewPen_t = void*(*)(void*, const DvaColorRGBA*, float);
using DrawFillRect_t = void (*)(void*, const void*, const DvaColorRGBA*);

NewBrush_t NewBrush_Original = nullptr;
NewPen_t NewPen_Original = nullptr;
DrawFillRect_t DrawFillRect_Original = nullptr;

/*
    Not every dark gray on screen is interface: #373737 in the color picker is a
    value being edited. The brush hooks decide by color alone, so the origin
    decides instead. While a swatch or a parameter color is drawn, the thread
    doing it paints what it is given. Interface colors drawn inside still arrive
    themed, because the theme functions converted them first.

    The outermost scope also records the frame it was opened in. An exception
    from Adobe's code can unwind past the hook without running ~ContentScope,
    since this mod's unwinder is not guaranteed to run destructors for a foreign
    exception; a check made from higher up the stack can only mean the scope is
    gone, and clears it. The hook passes its own frame in, so two scopes in one
    frame compare equal whether or not the constructor was inlined.
*/
thread_local int g_contentDepth = 0;
thread_local uintptr_t g_contentFrame = 0;  // frame of the outermost live scope

struct ContentScope {
    explicit ContentScope(void* frame) {
        auto here = reinterpret_cast<uintptr_t>(frame);

        if (g_contentDepth && here > g_contentFrame) {
            g_contentDepth = 0;  // left behind by an unwind that skipped it
        }

        if (g_contentDepth++ == 0) {
            g_contentFrame = here;
        }
    }

    ~ContentScope() {
        if (g_contentDepth > 0 && --g_contentDepth == 0) {
            g_contentFrame = 0;
        }
    }

    ContentScope(const ContentScope&) = delete;
    ContentScope& operator=(const ContentScope&) = delete;
};

static bool InContentScope() {
    if (!g_contentDepth) {
        return false;
    }

    if (reinterpret_cast<uintptr_t>(__builtin_frame_address(0)) > g_contentFrame) {
        g_contentDepth = 0;
        g_contentFrame = 0;
        return false;
    }

    return true;
}

// A content scope only changes what the brush and GDI layers do.
static bool ContentScopesMatter() {
    const Settings& s = CurrentSettings();
    return s.brushHook || s.gdiHook;
}

/*
    Cheapest test first: most colors arriving here are saturated or too light,
    and float comparisons turn them away before the recent colors are read.
*/
static bool ConvertForPaint(const DvaColorRGBA* in, DvaColorRGBA* out) {
    const Settings& s = CurrentSettings();

    if (!s.brushHook || !in || InContentScope()) {
        return false;
    }

    // Two pointer comparisons for anything outside the table.
    if (IsConvertedSlot(in)) {
        return false;
    }

    // Cheap float test, rejects the majority.
    if (!ShouldConvertWith(s, *in)) {
        return false;
    }

    // Only now the recent colors, and only for what is left.
    if (IsProducedColor(*in)) {
        return false;
    }

    if (!ConvertDvaColorWith(s, *in, out)) {
        return false;
    }

    RememberProduced(*out);

    return true;
}

void* NewBrush_Hook(void* self, const DvaColorRGBA* color) {
    DvaColorRGBA converted{};

    if (ConvertForPaint(color, &converted)) {
        return NewBrush_Original(self, &converted);
    }

    return NewBrush_Original(self, color);
}

void* NewPen_Hook(void* self, const DvaColorRGBA* color, float width) {
    DvaColorRGBA converted{};

    if (ConvertForPaint(color, &converted)) {
        return NewPen_Original(self, &converted, width);
    }

    return NewPen_Original(self, color, width);
}

void DrawFillRect_Hook(void* drawbot, const void* rect,
                       const DvaColorRGBA* color) {
    DvaColorRGBA converted{};

    if (ConvertForPaint(color, &converted)) {
        DrawFillRect_Original(drawbot, rect, &converted);
        return;
    }

    DrawFillRect_Original(drawbot, rect, color);
}

/*
    Some panels erase their own background before their children draw, with a
    color handed over by reference:

        OS_NodeManager::ScEnableEraseBackgroundDrawing(OS_NodeManager*, const ColorRGBA&)
        UI_Node::UI_DispatchDrawFromRoot(const ColorRGBA&, Drawbot*, bool)

    That color has not necessarily been through a theme function. Neither
    keeps the reference, so a stack temporary is safe here as it is for the
    brushes. In dvaui 2023, 2025 and 2026 the constructor turns the color into
    a GDI brush that the node manager keeps, and the dispatch only passes it
    down to UI_DrawAndCache, which draws with it and returns.
*/
using EraseBackgroundCtor_t = void* (*)(void*, void*, const DvaColorRGBA*);
using DispatchDrawFromRoot_t = void (*)(void*, const DvaColorRGBA*, void*, bool);

EraseBackgroundCtor_t EraseBackgroundCtor_Original = nullptr;
DispatchDrawFromRoot_t DispatchDrawFromRoot_Original = nullptr;

void* EraseBackgroundCtor_Hook(void* self, void* nodeManager,
                               const DvaColorRGBA* color) {
    DvaColorRGBA converted{};

    if (ConvertForPaint(color, &converted)) {
        return EraseBackgroundCtor_Original(self, nodeManager, &converted);
    }

    return EraseBackgroundCtor_Original(self, nodeManager, color);
}

void DispatchDrawFromRoot_Hook(void* self, const DvaColorRGBA* color,
                               void* drawbot, bool flag) {
    // A new paint: nothing this thread produced before it is coming back.
    ForgetRecentProduced();

    DvaColorRGBA converted{};

    if (ConvertForPaint(color, &converted)) {
        DispatchDrawFromRoot_Original(self, &converted, drawbot, flag);
        return;
    }

    DispatchDrawFromRoot_Original(self, color, drawbot, flag);
}

static void InstallBrushHooks(HMODULE dvaui, HookCount& count) {
    const HookSpec specs[] = {
        {"?NewBrush@OSSupplier@d2d@drawbot@dvaui@@UEBAPEAUBrushInterface@34@"
         "AEBVColorRGBA@34@@Z",
         reinterpret_cast<void*>(NewBrush_Hook),
         reinterpret_cast<void**>(&NewBrush_Original), L"d2d::NewBrush"},

        {"?NewPen@OSSupplier@d2d@drawbot@dvaui@@UEBAPEAUPenInterface@34@"
         "AEBVColorRGBA@34@M@Z",
         reinterpret_cast<void*>(NewPen_Hook),
         reinterpret_cast<void**>(&NewPen_Original), L"d2d::NewPen"},

        {"?DrawFillRect@utils@dvaui@@YAXPEAVDrawbot@drawbot@2@"
         "AEBV?$RectT@M@geom@dvacore@@AEBVColorRGBA@42@@Z",
         reinterpret_cast<void*>(DrawFillRect_Hook),
         reinterpret_cast<void**>(&DrawFillRect_Original), L"utils::DrawFillRect"},

        {"??0ScEnableEraseBackgroundDrawing@OS_NodeManager@ui@dvaui@@QEAA@"
         "PEAV123@AEBVColorRGBA@drawbot@3@@Z",
         reinterpret_cast<void*>(EraseBackgroundCtor_Hook),
         reinterpret_cast<void**>(&EraseBackgroundCtor_Original),
         L"OS_NodeManager::ScEnableEraseBackgroundDrawing"},

        {"?UI_DispatchDrawFromRoot@UI_Node@ui@dvaui@@QEBAXAEBVColorRGBA@drawbot@"
         "3@PEAVDrawbot@53@_N@Z",
         reinterpret_cast<void*>(DispatchDrawFromRoot_Hook),
         reinterpret_cast<void**>(&DispatchDrawFromRoot_Original),
         L"UI_Node::UI_DispatchDrawFromRoot"},
    };

    InstallHooks(dvaui, specs, &count);
}

/*
    The swatch controls, drawn inside a ContentScope. UI_Swatch::UI_Draw is what
    the color picker, marker colors and Essential Graphics draw their swatches
    through; the skin-level draws cover a swatch painted through its skin by
    something else, and popup menus draw label colors through their own
    function.

    All take pointers only, five at most with the fifth on the stack, so one
    thunk forwarding five integer arguments carries each through unchanged.
*/
using ContentDraw_t = void (*)(uintptr_t, uintptr_t, uintptr_t, uintptr_t,
                               uintptr_t);

template <size_t I>
struct ContentDrawHook {
    static inline ContentDraw_t original = nullptr;

    static void Hook(uintptr_t a, uintptr_t b, uintptr_t c, uintptr_t d,
                     uintptr_t e) {
        ContentScope scope(__builtin_frame_address(0));
        original(a, b, c, d, e);
    }
};

static const ColorSymbol kContentDraws[] = {
    {"?UI_Draw@UI_Swatch@controls@dvaui@@MEBAXPEAVDrawbot@drawbot@3@@Z",
     L"UI_Swatch::UI_Draw"},
    {"?DrawSwatch@V7SwatchSkin@v7@skins@dvaui@@UEBAXPEAVDrawbot@drawbot@4@"
     "AEBVDrawSwatchParameters@controls@4@@Z",
     L"V7SwatchSkin::DrawSwatch"},
    {"?DrawSwatch@BaseSwatchSkin@csnext@skins@dvaui@@MEBAXPEAVDrawbot@drawbot@"
     "4@AEBVDrawSwatchParameters@controls@4@@Z",
     L"BaseSwatchSkin::DrawSwatch"},
    {"?DrawColorSwatch@V7PopupSkin@v7@skins@dvaui@@UEBAXPEAVDrawbot@drawbot@4@"
     "PEBVThemeProvider@ui@4@AEBV?$RectT@M@geom@dvacore@@AEBVColorRGBA@64@@Z",
     L"V7PopupSkin::DrawColorSwatch"},
    {"?DrawColorSwatch@V6PopupSkin@v6@skins@dvaui@@UEBAXPEAVDrawbot@drawbot@4@"
     "PEBVThemeProvider@ui@4@AEBV?$RectT@M@geom@dvacore@@AEBVColorRGBA@64@@Z",
     L"V6PopupSkin::DrawColorSwatch"},
};

constexpr size_t kContentDrawCount = ARRAYSIZE(kContentDraws);

static void InstallOneContentHook(HMODULE dvaui, size_t index, void* hook,
                                  void** original) {
    const ColorSymbol& sym = kContentDraws[index];

    InstallHook(dvaui, {sym.mangled, hook, original, sym.label}, nullptr);
}

template <size_t... I>
static void InstallContentHooksImpl(HMODULE dvaui, std::index_sequence<I...>) {
    (InstallOneContentHook(dvaui, I,
                           reinterpret_cast<void*>(&ContentDrawHook<I>::Hook),
                           reinterpret_cast<void**>(&ContentDrawHook<I>::original)),
     ...);
}

/*
    Content views that export no draw function of their own, such as the color
    picker's chip. Every node is drawn through UI_Node::UI_DrawSelf or
    UI_DrawAndCache, so those are hooked and the node is recognized by its
    most-derived class, from MSVC's RTTI: the slot before a vtable points at the
    complete object locator, which points at the type descriptor holding the
    decorated name. Every step is checked to lie in the right image, and the
    answer is cached per vtable.
*/
static const char* const kContentClasses[] = {
    ".?AVColorPickerChipView@sharedui@dvaui@@",  // the shared dvaui picker

    /*
        After Effects' own, all in AfterFXLib.dll and all dvaui nodes by their
        RTTI: the picker's color field, the chip a color is shown in, the
        solid-color button, and the gradient editor's ramp.
    */
    ".?AVDialogColorPickerControl@@",
    ".?AVColorChip@@",
    ".?AVSolidColorButton@egg@ae@@",
    ".?AVGradientColorsControl@@",
};

struct RttiLocator {  // _RTTICompleteObjectLocator on x64
    DWORD signature;  // 1
    DWORD offset;
    DWORD constructorOffset;
    DWORD typeDescriptor;   // RVA
    DWORD classDescriptor;  // RVA
    DWORD self;             // RVA of this locator
};

static bool IsImageMemory(const void* address, size_t size, uintptr_t* image) {
    MEMORY_BASIC_INFORMATION info{};

    if (!address || !VirtualQuery(address, &info, sizeof(info)) ||
        info.State != MEM_COMMIT || info.Type != MEM_IMAGE ||
        (info.Protect & (PAGE_NOACCESS | PAGE_GUARD))) {
        return false;
    }

    auto end = static_cast<const char*>(info.BaseAddress) + info.RegionSize;

    if (static_cast<const char*>(address) + size > end) {
        return false;
    }

    if (image) {
        *image = reinterpret_cast<uintptr_t>(info.AllocationBase);
    }

    return true;
}

static bool ResolveContentClass(uintptr_t vtable) {
    auto slots = reinterpret_cast<void* const*>(vtable);
    uintptr_t vtableImage = 0;

    if (!IsImageMemory(slots - 1, sizeof(void*), &vtableImage)) {
        return false;
    }

    auto locator = static_cast<const RttiLocator*>(slots[-1]);
    uintptr_t image = 0;

    if (!IsImageMemory(locator, sizeof(RttiLocator), &image) || image != vtableImage ||
        locator->signature != 1 ||
        image + locator->self != reinterpret_cast<uintptr_t>(locator)) {
        return false;
    }

    // The type descriptor: a vtable pointer, a spare pointer, then the name.
    auto name = reinterpret_cast<const char*>(image + locator->typeDescriptor +
                                              2 * sizeof(void*));

    for (const char* candidate : kContentClasses) {
        size_t length = strlen(candidate) + 1;  // with the terminator

        if (IsImageMemory(name, length, nullptr) &&
            memcmp(name, candidate, length) == 0) {
            return true;
        }
    }

    return false;
}

/*
    vtable | 2 once the class is known, | 1 if it is content; 0 is an empty slot.
    Vtables are 8-byte aligned, so the low bits are free.
*/
constexpr size_t kNodeClassSlots = 2048;

volatile LONG64 g_nodeClasses[kNodeClassSlots];

// The module generation the entries above were resolved under; see below.
volatile LONG g_nodeClassEpoch = 1;

static bool IsContentNode(const void* node) {
    uintptr_t vtable = *static_cast<const uintptr_t*>(node);

    if (!vtable || (vtable & 7)) {
        return false;
    }

    /*
        A vtable address belongs to the module it came from, so the whole
        table goes whenever a module is mapped or unmapped — the same reason
        IsAdobeUICaller throws away its cached range. Without this, a module
        unloading and another mapping over it would leave a class decision
        behind that no longer describes anything.

        Whoever notices first clears it. A call already reading an entry at
        that moment can still get the stale answer, which costs one node one
        paint; every entry written from here on is resolved against memory as
        it is now.
    */
    LONG epoch = g_moduleRangeEpoch;
    LONG seen = g_nodeClassEpoch;

    if (epoch != seen &&
        InterlockedCompareExchange(&g_nodeClassEpoch, epoch, seen) == seen) {
        for (volatile LONG64& entry : g_nodeClasses) {
            InterlockedExchange64(&entry, 0);
        }
    }

    size_t start = FibonacciIndex<kNodeClassSlots>(vtable >> 3);

    for (size_t probe = 0; probe < 32; probe++) {
        size_t i = (start + probe) & (kNodeClassSlots - 1);
        LONG64 entry = g_nodeClasses[i];

        if (!entry) {
            bool content = ResolveContentClass(vtable);

            // Losing this race to another thread only means resolving twice.
            InterlockedCompareExchange64(
                &g_nodeClasses[i],
                static_cast<LONG64>(vtable | 2 | (content ? 1 : 0)), 0);

            return content;
        }

        if (static_cast<uintptr_t>(entry & ~7LL) == vtable) {
            return entry & 1;
        }
    }

    return ResolveContentClass(vtable);  // table full: still right, just slower
}

using NodeDraw_t = void (*)(const void*, void*, bool, const void*);

NodeDraw_t UiDrawSelf_Original = nullptr;
NodeDraw_t UiDrawAndCache_Original = nullptr;

/*
    One body for both entry points. The class lookup is skipped outright while
    no setting a content scope affects is on.
*/
template <NodeDraw_t* Original>
void NodeDraw_Hook(const void* node, void* drawbot, bool flag, const void* color) {
    std::optional<ContentScope> scope;

    if (node && ContentScopesMatter() && IsContentNode(node)) {
        scope.emplace(__builtin_frame_address(0));
    }

    (*Original)(node, drawbot, flag, color);
}

static void InstallNodeDrawHooks(HMODULE dvaui) {
    const HookSpec specs[] = {
        {"?UI_DrawSelf@UI_Node@ui@dvaui@@AEBAXPEAVDrawbot@drawbot@3@_NPEBV"
         "ColorRGBA@53@@Z",
         reinterpret_cast<void*>(&NodeDraw_Hook<&UiDrawSelf_Original>),
         reinterpret_cast<void**>(&UiDrawSelf_Original), L"UI_Node::UI_DrawSelf"},

        {"?UI_DrawAndCache@UI_Node@ui@dvaui@@AEBAXPEAVDrawbot@drawbot@3@_NPEBV"
         "ColorRGBA@53@@Z",
         reinterpret_cast<void*>(&NodeDraw_Hook<&UiDrawAndCache_Original>),
         reinterpret_cast<void**>(&UiDrawAndCache_Original),
         L"UI_Node::UI_DrawAndCache"},
    };

    InstallHooks(dvaui, specs, nullptr);
}

/*
    dvaui is hooked when it arrives — once, by whichever thread gets there
    first. After Effects loads libraries from several threads at startup, and a
    plain flag read and then set would let two of them register the same hooks.
*/
volatile LONG g_dvauiHooked = FALSE;

/*
    Registers the hooks for dvaui if it is loaded now and not done yet, and
    returns whether it registered any. It does not apply them: Windhawk does
    that when Wh_ModInit returns, and the loader hook and the settings callback
    apply what this registers.

    Once in, a hook stays in and checks its own setting each time it runs,
    which is what lets settings change without a reload. They only go in while
    a setting that works through that module is on: with "After Effects
    interface", "Direct fills" and "GDI surfaces" all off when the mod loads,
    dvaui is not touched at all — the way back to a working After Effects if
    an update ever breaks one of these hooks.
*/
static bool WantsAdobeHooks() {
    const Settings& s = CurrentSettings();
    return s.dvauiHook || s.brushHook || s.gdiHook;
}

/*
    After Effects ships Adobe's UI toolkit as dvaui.dll, the same module
    Premiere does, and every hook below is looked up in it by name. A build
    that renames the module, or folds the toolkit into another one, still
    carries Adobe's own mangled names — they spell the dvaui namespaces out —
    so when the name is gone the module is looked for by symbol instead.

    Nothing here runs while GetModuleHandleW(L"dvaui.dll") answers: on every
    build that ships the DLL no module is enumerated and no anchor is asked
    for.
*/
static const char* const kColorAnchors[] = {
    // The Spectrum ramp, which most of the interface is painted from.
    "?GetGrayColor@ui@dvaui@@YAAEBVColorRGBA@drawbot@2@W4SpectrumGrayColor@12@@Z",

    // The classic theme, which the older builds paint from.
    "?GetColor@Theme@ui@dvaui@@UEBAAEBVColorRGBA@drawbot@3@_K@Z",

    // The fill path, which is hooked even with the color layer off.
    ("?NewBrush@OSSupplier@d2d@drawbot@dvaui@@UEBAPEAUBrushInterface@34@"
     "AEBVColorRGBA@34@@Z"),
};

static bool ExportsColorAnchor(HMODULE module) {
    for (const char* anchor : kColorAnchors) {
        if (GetProcAddress(module, anchor)) {
            return true;
        }
    }

    return false;
}

// How much of the color surface one module carries; either spelling counts once.
static int CountColorExports(HMODULE module) {
    int found = 0;

    for (const ColorSymbol& symbol : kColorSymbols) {
        if (GetProcAddress(module, symbol.mangled) ||
            (symbol.before2026 && GetProcAddress(module, symbol.before2026))) {
            found++;
        }
    }

    return found;
}

/*
    How much of the color surface a module has to carry to stand in for
    dvaui.dll. More than one module can answer an anchor — in Premiere,
    dvaworkspace.dll and Frontend.dll carry copies of part of the toolkit that
    paint none of the interface — so hooking one in place of the real module
    would cost the session. Half the surface keeps those out and still accepts
    a build that dropped a few functions. Measured on After Effects 2026,
    dvaui.dll exports all 26.
*/
constexpr int kMinColorExports = static_cast<int>(kColorSymbolCount) / 2;

static bool QualifiesAsColorModule(HMODULE module) {
    return module && ExportsColorAnchor(module) &&
           CountColorExports(module) >= kMinColorExports;
}

/*
    The richest module in `modules` that qualifies, or null when none does.

    Split from the enumeration so the harness can hand it a set of modules.
*/
static HMODULE FindColorModuleIn(const HMODULE* modules, size_t count) {
    HMODULE best = nullptr;
    int bestExports = 0;

    for (size_t i = 0; i < count; i++) {
        if (!modules[i] || !ExportsColorAnchor(modules[i])) {
            continue;
        }

        int exports = CountColorExports(modules[i]);

        if (exports >= kMinColorExports && exports > bestExports) {
            best = modules[i];
            bestExports = exports;
        }
    }

    return best;
}

volatile LONG g_colorSearchClosed = FALSE;
volatile LONG g_noColorModuleLogged = FALSE;

/*
    Said once, from wherever first knows for certain. Not at startup: dvaui is
    a static dependency of AfterFXLib.dll and the mod can initialize before
    the loader maps it, so "not there yet" and "not in this build" look alike
    until the interface is up.
*/
static void ReportNoColorModule() {
    if (!Claim(&g_noColorModuleLogged)) {
        return;
    }

    Wh_Log(L"the interface layer is not installed: no module in this process "
           L"exports Adobe's color functions, so this build cannot be "
           L"themed that way. The window frame, the menu bar and the native "
           L"dialogs still follow the palette; the panels keep After Effects' "
           L"own colors.");
}

/*
    The module named here is hooked for the rest of the process, the way
    dvaui.dll is. A module carrying most of the color surface is part of
    the interface and stays mapped while that interface is up; the hooks would
    point into unmapped memory if one ever did not.

    It says dvaui is not loaded, not that this build has none. One of the
    two callers reaches here from the loader hook, where a null
    GetModuleHandleW may only mean dvaui has not been mapped yet — see
    SearchForColorModule, which is careful about the same distinction.
*/
static void ReportColorModule(HMODULE module) {
    wchar_t path[MAX_PATH]{};
    const wchar_t* name = L"an unnamed module";

    if (GetModuleFileNameW(module, path, ARRAYSIZE(path))) {
        const wchar_t* slash = wcsrchr(path, L'\\');
        name = slash ? slash + 1 : path;
    }

    Wh_Log(L"dvaui.dll is not loaded; the color functions were found in "
           L"%s, and the interface layer goes in there",
           name);
}

// Defined with the window layer, far below; see SearchForColorModule.
static bool HasFramedWindow();

/*
    Every module in the process, asked for the color functions. This is the
    expensive half of the search, so it runs only from the two callers that
    can afford it — after init, and after a settings change — and never from
    the loader hook, which After Effects goes through hundreds of times while
    it starts.

    Failing does not close the search: until the interface is up, "nothing
    carries them" only means the toolkit is not mapped yet. Once a framed
    window is there, every module the UI needs is in, and a build that still
    answers nothing is one this layer cannot reach — which is said once and
    not asked again.
*/
static HMODULE SearchForColorModule() {
    if (g_colorSearchClosed) {
        return nullptr;
    }

    std::vector<HMODULE> modules;
    DWORD error = 0;

    if (!EnumerateProcessModules(&modules, &error)) {
        Wh_Log(L"dvaui.dll is not loaded and the modules could not be "
               L"enumerated (%u), so the interface layer has nothing to look "
               L"through",
               error);
        return nullptr;
    }

    HMODULE found = FindColorModuleIn(modules.data(), modules.size());

    if (found) {
        ReportColorModule(found);
        return found;
    }

    if (HasFramedWindow()) {
        InterlockedExchange(&g_colorSearchClosed, TRUE);
        ReportNoColorModule();
    }

    return nullptr;
}

/*
    `justLoaded` is the module the loader hook has just mapped, which costs
    three lookups to ask and covers a renamed toolkit that arrives late.
    `maySearchProcess` is for the callers that can afford to walk every module
    in the process; see SearchForColorModule.
*/
static bool HookLoadedModules(HMODULE justLoaded = nullptr,
                              bool maySearchProcess = false) {
    if (!WantsAdobeHooks()) {
        return false;
    }

    bool registered = false;

    HMODULE dvaui = GetModuleHandleW(L"dvaui.dll");

    if (!dvaui && !g_dvauiHooked && QualifiesAsColorModule(justLoaded)) {
        dvaui = justLoaded;
        ReportColorModule(dvaui);
    }

    if (!dvaui && !g_dvauiHooked && maySearchProcess) {
        dvaui = SearchForColorModule();
    }

    if (dvaui && Claim(&g_dvauiHooked)) {
        HookCount count;

        InstallColorHooksImpl(dvaui, count,
                              std::make_index_sequence<kColorSymbolCount>{});
        InstallBrushHooks(dvaui, count);
        InstallContentHooksImpl(dvaui, std::make_index_sequence<kContentDrawCount>{});
        InstallNodeDrawHooks(dvaui);

        registered = true;

        if (count.installed) {
            Wh_Log(L"dvaui: %d hooks active, %d absent in this version",
                   count.installed, count.missing);
        } else {
            Wh_Log(L"no dvaui color entry point matched — this After Effects "
                   L"version is not supported by the interface layer. The window frame "
                   L"and menus still apply.");
        }
    }

    return registered;
}

/*
    dvaui is not necessarily loaded when the mod initializes — enable the mod
    before After Effects starts and it is not — so the loader is hooked, and
    the module is hooked when it arrives. The hook goes in
    kernelbase, not kernel32, whose export is only a forwarder.
*/
using LoadLibraryExW_t = HMODULE(WINAPI*)(LPCWSTR, HANDLE, DWORD);

LoadLibraryExW_t LoadLibraryExW_Original = nullptr;

HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR fileName, HANDLE file, DWORD flags) {
    HMODULE module = LoadLibraryExW_Original(fileName, file, flags);

    constexpr DWORD kDataOnly = LOAD_LIBRARY_AS_DATAFILE |
                                LOAD_LIBRARY_AS_DATAFILE_EXCLUSIVE |
                                LOAD_LIBRARY_AS_IMAGE_RESOURCE;

    // A data-only mapping has no code in it, so there is nothing to hook.
    if (!module || (flags & kDataOnly)) {
        return module;
    }

    /*
        Whether dvaui is loaded now, not whether it is the module just
        returned: it usually arrives as a dependency of something else. Once
        it is hooked, this is a plain read.
    */
    bool registered = !g_dvauiHooked && HookLoadedModules(module);

    if (registered && !Wh_ApplyHookOperations()) {
        Wh_Log(L"failed to apply hooks for a late-loaded module");
    }

    return module;
}

// ============================================================================
// UXP PANELS: THE STYLESHEETS THEY READ
// ============================================================================

/*
    The Home screen is a UXP plugin that paints without dvaui, from
    stylesheets and scripts with the Spectrum grays written into them, and a
    webview page for the bar across its top. What reaches them is the file: a
    read of one of those is answered with a recolored copy, a temporary file
    deleted on close. Every color keeps its length, because the
    UXP runtime also asks for a file's size by path.
*/

wchar_t g_uxpPluginsDir[MAX_PATH + 16] = {};  // folded, see FoldPathChar
size_t g_uxpPluginsDirLength = 0;

volatile LONG g_bundledSerial = 0;
volatile LONG g_bundledFailureLogged = FALSE;
volatile LONG g_bundledRecolored = 0;  // logged at unload

// ASCII case and both separators; the rest of a path has to match exactly.
static wchar_t FoldPathChar(wchar_t c) {
    if (c == L'/') {
        return L'\\';
    }

    if (c >= L'A' && c <= L'Z') {
        return static_cast<wchar_t>(c - L'A' + L'a');
    }

    return c;
}

// <folder of the executable>\UXP\plugins\, where only Adobe's own plugins live.
static void SetUxpPluginsDirFrom(const wchar_t* executablePath) {
    g_uxpPluginsDirLength = 0;

    const wchar_t* slash = wcsrchr(executablePath, L'\\');

    if (!slash) {
        return;
    }

    constexpr wchar_t kTail[] = L"UXP\\plugins\\";
    size_t folder = static_cast<size_t>(slash - executablePath) + 1;
    size_t tail = ARRAYSIZE(kTail) - 1;

    if (folder + tail >= ARRAYSIZE(g_uxpPluginsDir)) {
        return;
    }

    for (size_t i = 0; i < folder; i++) {
        g_uxpPluginsDir[i] = FoldPathChar(executablePath[i]);
    }

    for (size_t i = 0; i < tail; i++) {
        g_uxpPluginsDir[folder + i] = FoldPathChar(kTail[i]);
    }

    g_uxpPluginsDir[folder + tail] = L'\0';
    g_uxpPluginsDirLength = folder + tail;
}

static void FindUxpPluginsDir() {
    wchar_t path[MAX_PATH]{};
    DWORD length = GetModuleFileNameW(nullptr, path, ARRAYSIZE(path));

    if (length && length < ARRAYSIZE(path)) {
        SetUxpPluginsDirFrom(path);
    }
}

/*
    The parts of a UXP plugin that carry its colors.

    The stylesheet is the obvious one. The script is not, and it is where the
    panels keep the design tokens their own components read — see
    RecolorTokenTable — and the stylesheet text they inject. A page is read
    the way a script is: a webview's page carries both in its inline script
    and style, and After Effects itself serves it to the webview, so the read
    passes through here like any other.
*/
enum class BundledFile {
    None,
    Stylesheet,
    Script,
    Page,
};

// A .css, .js or .html file under After Effects' own UXP\plugins folder.
// `relative` receives the part after that folder, for the log.
static BundledFile BundledFileKind(LPCWSTR path, LPCWSTR* relative) {
    if (!path || !g_uxpPluginsDirLength) {
        return BundledFile::None;
    }

    size_t length = wcslen(path);
    BundledFile kind = BundledFile::None;

    if (length >= 4 && _wcsicmp(path + length - 4, L".css") == 0) {
        kind = BundledFile::Stylesheet;
    } else if (length >= 3 && _wcsicmp(path + length - 3, L".js") == 0) {
        kind = BundledFile::Script;
    } else if (length >= 5 && _wcsicmp(path + length - 5, L".html") == 0) {
        kind = BundledFile::Page;
    } else {
        return BundledFile::None;
    }

    if (wcsncmp(path, L"\\\\?\\", 4) == 0) {
        path += 4;
        length -= 4;
    }

    if (length <= g_uxpPluginsDirLength) {
        return BundledFile::None;
    }

    for (size_t i = 0; i < g_uxpPluginsDirLength; i++) {
        if (FoldPathChar(path[i]) != g_uxpPluginsDir[i]) {
            return BundledFile::None;
        }
    }

    // A path that climbs back out of the folder is not one of its files.
    if (wcsstr(path + g_uxpPluginsDirLength, L"..")) {
        return BundledFile::None;
    }

    if (relative) {
        *relative = path + g_uxpPluginsDirLength;
    }

    return kind;
}

static int CssHexDigit(char c) {
    if (c >= '0' && c <= '9') {
        return c - '0';
    }

    if (c >= 'a' && c <= 'f') {
        return c - 'a' + 10;
    }

    if (c >= 'A' && c <= 'F') {
        return c - 'A' + 10;
    }

    return -1;
}

static bool IsCssWordChar(char c) {
    return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z') || c == '-' || c == '_';
}

// What an 8-bit stylesheet color becomes, in place; false leaves it as written.
/*
    How far above the brightness ceiling a border or a fill is still chrome.

    The ceiling is there to protect text: raise it and Spectrum's #4B4B4B
    disabled text starts disappearing. But it turns away borders as well, and
    Spectrum paints 106 of those #494949 in the panels this rewrites — 28.6%,
    six tenths of a point above the default 28. Everything below was themed
    and those were not, which on a search field is the whole of the control:
    a stock gray box on a themed panel.

    Brightness cannot tell the two apart — #494949 and #4B4B4B are eight
    tenths of a point from each other. The property can, and a stylesheet is
    the one place in the mod that knows it. The slack stops well short of the
    next neutral Spectrum uses for chrome, #696969 at 41.2%, so a light
    divider or a focus ring is still left alone.
*/
constexpr float kCssChromeSlack = 0.07f;

// A neutral just above the ceiling, which only a border or a fill can be.
static bool IsCssChromeTone(const Settings& s, const DvaColorRGBA& in) {
    if (!IsSaneChannel(in.r) || !IsSaneChannel(in.g) || !IsSaneChannel(in.b) ||
        !IsNeutral(in.r, in.g, in.b, 0.035f)) {
        return false;
    }

    float gray = (in.r + in.g + in.b) / 3.0f;

    return gray > s.ceiling && gray <= s.ceiling + kCssChromeSlack;
}

static bool RecolorCssChannels(int rgb[3], bool chrome) {
    DvaColorRGBA in{rgb[0] / 255.0f, rgb[1] / 255.0f, rgb[2] / 255.0f, 1.0f};
    const Settings& s = CurrentSettings();
    COLORREF target = 0;
    int blue = -1;

    if (!PaletteTarget(s, in, &target, &blue)) {
        if (!chrome || !IsCssChromeTone(s, in)) {
            return false;
        }

        // Past the ceiling, PickTarget clamps to the lightest step — which is
        // the tone the palette keeps for dividers and edges, and is what this
        // is.
        target = PickTarget(s, (in.r + in.g + in.b) / 3.0f);
    }

    auto channel = [&](float original, BYTE wanted) {
        float v = BlendWith(s.strength, original, wanted / 255.0f) * 255.0f + 0.5f;
        return ClampInt(static_cast<int>(v), 0, 255);
    };

    int out[3] = {channel(in.r, GetRValue(target)), channel(in.g, GetGValue(target)),
                  channel(in.b, GetBValue(target))};

    if (out[0] == rgb[0] && out[1] == rgb[1] && out[2] == rgb[2]) {
        return false;
    }

    for (int k = 0; k < 3; k++) {
        rgb[k] = out[k];
    }

    return true;
}

// #rrggbb and #rrggbbaa, alpha kept. #rgb has no room for most results.
static bool RecolorCssHex(char* text, size_t size, size_t hash, bool chrome) {
    size_t start = hash + 1;
    size_t end = start;

    while (end < size && CssHexDigit(text[end]) >= 0) {
        end++;
    }

    size_t digits = end - start;

    if ((digits != 6 && digits != 8) || (end < size && IsCssWordChar(text[end]))) {
        return false;
    }

    /*
        The case follows the letters already there, the alpha pair included:
        the first letter across the whole run decides, so #00ff00AA stays
        lowercase and #001100AA is written uppercase.
    */
    bool upper = false;

    for (size_t i = start; i < end; i++) {
        if (text[i] >= 'A' && text[i] <= 'F') {
            upper = true;
            break;
        }

        if (text[i] >= 'a' && text[i] <= 'f') {
            break;
        }
    }

    int rgb[3];

    for (int k = 0; k < 3; k++) {
        rgb[k] = CssHexDigit(text[start + 2 * k]) * 16 +
                 CssHexDigit(text[start + 2 * k + 1]);
    }

    if (!RecolorCssChannels(rgb, chrome)) {
        return false;
    }

    const char* hex = upper ? "0123456789ABCDEF" : "0123456789abcdef";

    for (int k = 0; k < 3; k++) {
        text[start + 2 * k] = hex[rgb[k] >> 4];
        text[start + 2 * k + 1] = hex[rgb[k] & 0xF];
    }

    return true;
}

static size_t WriteCssDecimal(char* out, int value) {
    char reversed[3];
    size_t count = 0;

    do {
        reversed[count++] = static_cast<char>('0' + value % 10);
        value /= 10;
    } while (value && count < 3);

    for (size_t k = 0; k < count; k++) {
        out[k] = reversed[count - 1 - k];
    }

    return count;
}

/*
    "R,G,B" as a -rgb custom property or an rgb()/rgba() holds it. The new
    digits are padded with spaces to the old length, and a color whose digits
    would not fit keeps its own.
*/
static bool RecolorCssTriplet(char* text, size_t size, size_t from, bool chrome) {
    size_t i = from;
    size_t first = 0;
    int rgb[3];

    for (int k = 0; k < 3; k++) {
        while (i < size && text[i] == ' ') {
            i++;
        }

        if (k > 0) {
            if (i >= size || text[i] != ',') {
                return false;
            }

            i++;

            while (i < size && text[i] == ' ') {
                i++;
            }
        }

        size_t start = i;
        int value = 0;

        while (i < size && text[i] >= '0' && text[i] <= '9' && i - start < 3) {
            value = value * 10 + (text[i] - '0');
            i++;
        }

        // Nothing there, too many digits, out of range, a percentage or a fraction.
        if (i == start || value > 255 ||
            (i < size && ((text[i] >= '0' && text[i] <= '9') || text[i] == '%' ||
                          text[i] == '.'))) {
            return false;
        }

        if (k == 0) {
            first = start;
        }

        rgb[k] = value;
    }

    if (!RecolorCssChannels(rgb, chrome)) {
        return false;
    }

    char written[12];
    size_t length = 0;

    for (int k = 0; k < 3; k++) {
        if (k > 0) {
            written[length++] = ',';
        }

        length += WriteCssDecimal(written + length, rgb[k]);
    }

    size_t span = i - first;

    if (length > span) {
        return false;
    }

    memcpy(text + first, written, length);
    memset(text + first + length, ' ', span - length);

    return true;
}

/*
    Whether the value at `at` belongs to a property that paints chrome rather
    than text: a border, a fill, an outline.

    The declaration is walked backwards to its colon and the property read off
    in front of it, so `border-top-color` and `background-image` count and
    `color`, `-webkit-text-fill-color` and `fill` — which is an icon, and a
    darkened icon disappears — do not.
*/
/*
    Where a color sits, which is two questions answered by one walk back.

    NotAValue is the one that decides whether to rewrite at all: a run of six
    hex digits is only a color when a property introduces it. `#1d1d1d { }` is
    a selector, and rewriting it would leave a rule that matches nothing.
    Chrome and Text then decide only how much slack the color gets above the
    ceiling.
*/
enum class CssValueRole { NotAValue, Text, Chrome };

static CssValueRole CssValueRoleAt(const char* text, size_t at) {
    size_t colon = at;

    while (colon > 0 && text[colon - 1] != ':' && text[colon - 1] != ';' &&
           text[colon - 1] != '{' && text[colon - 1] != '}') {
        colon--;
    }

    if (colon == 0 || text[colon - 1] != ':') {
        return CssValueRole::NotAValue;
    }

    size_t end = colon - 1;

    /*
        A key in quotes is the same property: UXP ships its stylesheets as
        serialized rules, `"background":"#1e1e1e"`, and a script's inline
        styles are keyed the same way.
    */
    if (end > 0 && (text[end - 1] == '"' || text[end - 1] == '\'')) {
        end--;
    }

    size_t begin = end;

    while (begin > 0 && IsCssWordChar(text[begin - 1])) {
        begin--;
    }

    for (const char* prefix : {"background", "border", "outline"}) {
        size_t length = strlen(prefix);

        if (end - begin >= length &&
            _strnicmp(text + begin, prefix, length) == 0) {
            return CssValueRole::Chrome;
        }
    }

    return CssValueRole::Text;
}

// The slack question on its own, for the two forms that can only be values.
static bool CssValueIsChrome(const char* text, size_t at) {
    return CssValueRoleAt(text, at) == CssValueRole::Chrome;
}

static bool CssPrecededBy(const char* text, size_t at, const char* word) {
    size_t length = strlen(word);

    return at >= length && _strnicmp(text + at - length, word, length) == 0;
}

/*
    Rewrites every color a stylesheet spells out, in place and at its own
    length, and returns how many changed. A value taken from a variable, like
    rgb(var(--x)), changes where the variable is defined.
*/
/*
    Whether a design token's key paints text rather than chrome.

    The keys are the same words a stylesheet uses — background-color,
    border-color, track-color, tip-color, text-color — so the same rule
    applies: everything but text is chrome, and chrome gets the slack above
    the ceiling that an input's border needs.
*/
static bool CssTokenKeyIsText(const char* text, size_t at) {
    size_t begin = at;

    while (begin > 0 && IsCssWordChar(text[begin - 1])) {
        begin--;
    }

    return at - begin >= 4 && _strnicmp(text + at - 4, "text", 4) == 0;
}

/*
    The design tokens in a panel's own script.

    A UXP panel keeps its colors twice. The stylesheet has them, and so does a
    table in the script beside it, which the panel's components read and apply
    as inline styles — and an inline style beats every rule a stylesheet can
    state. Premiere's Text panel sets its search field from
    `"background-color":"rgb(37, 37, 37)"` there, so recoloring its main.css
    alone never reached it; After Effects' Home screen carries 528 such tokens
    in js\4.js.

    This pass touches only that one shape: a `<name>-color` key whose value
    is an rgb() string literal. Measured on that Text panel, all 264 of them
    are design tokens and nothing else in 1.7 MB of script has the shape.
    RecolorScriptCss, below, takes the stylesheet text a script carries as
    well. The digits are rewritten inside the quotes at their own length, so
    the script parses exactly as it did.
*/
static size_t RecolorTokenTable(char* text, size_t size) {
    constexpr char kKey[] = "-color\":\"rgb(";
    constexpr size_t kKeyLength = sizeof(kKey) - 1;

    size_t changed = 0;

    for (size_t i = 0; i + kKeyLength < size; i++) {
        // One byte turns away almost every position of a 1.7 MB script.
        if (text[i] != kKey[0] || memcmp(text + i, kKey, kKeyLength) != 0) {
            continue;
        }

        if (RecolorCssTriplet(text, size, i + kKeyLength,
                              !CssTokenKeyIsText(text, i))) {
            changed++;
        }
    }

    return changed;
}

static bool IsQuote(char c) {
    return c == '"' || c == '\'' || c == '`';
}

/*
    The stylesheet text a script or a page carries, rather than the file
    beside it: rules a component injects as a string, `.spectrum--dark {\n
    background-color: rgb(50, 50, 50);\n}`, an inline <style>, and a theme's
    colors held in a table, `{[THEME_DARKEST]:"rgb(30, 30, 30)"}`.

    Measured on After Effects 26.3's Home screen: the bar across its top is a
    UXP webview whose page and scripts paint it that way, rgb(50, 50, 50) in
    uab\3.js on a background the panel's own js\5.js sets from that table.
    The stylesheet pass alone reaches neither, and the token pass is keyed to
    another shape.

    The one thing this does not take is the one a script is full of: a hex
    color that is itself a JavaScript string. `fill:"#2c2c2c"` and
    `stroke:"#231f20"` are icons, and a darkened icon disappears, so a quoted
    hex value is only taken under a key that paints chrome — a background, a
    border, an outline — which an icon never is. rgb() in a script is almost
    only ever stylesheet text or a theme's table, and is taken as it comes,
    except in the token shape, which RecolorTokenTable has already done.
*/
static size_t RecolorScriptCss(char* text, size_t size) {
    size_t changed = 0;

    for (size_t i = 0; i < size; i++) {
        bool recolored = false;

        if (text[i] == '#') {
            CssValueRole role = CssValueRoleAt(text, i);
            bool quoted = i > 0 && IsQuote(text[i - 1]);

            if (role == CssValueRole::Chrome ||
                (role == CssValueRole::Text && !quoted)) {
                recolored = RecolorCssHex(text, size, i, role == CssValueRole::Chrome);
            }
        } else if (text[i] == ':' && CssPrecededBy(text, i, "-rgb")) {
            recolored =
                RecolorCssTriplet(text, size, i + 1, CssValueIsChrome(text, i));
        } else if (text[i] == '(' && !CssPrecededBy(text, i, "-color\":\"rgb") &&
                   (CssPrecededBy(text, i, "rgb") || CssPrecededBy(text, i, "rgba"))) {
            recolored =
                RecolorCssTriplet(text, size, i + 1, CssValueIsChrome(text, i));
        }

        changed += recolored ? 1 : 0;
    }

    return changed;
}

// Both passes over a script or a page, the token table first.
static size_t RecolorScript(char* text, size_t size) {
    size_t changed = RecolorTokenTable(text, size);
    return changed + RecolorScriptCss(text, size);
}

static size_t RecolorStylesheet(char* text, size_t size) {
    size_t changed = 0;

    for (size_t i = 0; i < size; i++) {
        bool recolored = false;

        if (text[i] == '#') {
            /*
                Only where a property introduced it. Six hex digits are also
                what an id selector looks like, and `#1d1d1d { }` rewritten is
                a rule that stops matching — the one form here that can be
                something other than a color.

                The two below cannot: `rgb(` is only ever a value, and the
                `-rgb` one is a custom property's own name, whose colon is the
                declaration's rather than a position inside a value.
            */
            CssValueRole role = CssValueRoleAt(text, i);

            if (role != CssValueRole::NotAValue) {
                recolored = RecolorCssHex(text, size, i,
                                          role == CssValueRole::Chrome);
            }
        } else if (text[i] == ':' && CssPrecededBy(text, i, "-rgb")) {
            recolored =
                RecolorCssTriplet(text, size, i + 1, CssValueIsChrome(text, i));
        } else if (text[i] == '(' &&
                   (CssPrecededBy(text, i, "rgb") || CssPrecededBy(text, i, "rgba"))) {
            recolored =
                RecolorCssTriplet(text, size, i + 1, CssValueIsChrome(text, i));
        }

        changed += recolored ? 1 : 0;
    }

    return changed;
}

using CreateFileW_t = HANDLE(WINAPI*)(LPCWSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES,
                                      DWORD, DWORD, HANDLE);
using CreateFile2_t = HANDLE(WINAPI*)(LPCWSTR, DWORD, DWORD, DWORD,
                                      LPCREATEFILE2_EXTENDED_PARAMETERS);

CreateFileW_t CreateFileW_Original = nullptr;
CreateFile2_t CreateFile2_Original = nullptr;

// Far above any stylesheet Adobe ships with its UXP panels.
constexpr LONGLONG kMaxBundledBytes = 16LL << 20;

// A read of a file that already exists: what a copy can stand in for.
static bool IsPlainRead(DWORD access, DWORD disposition, DWORD flags) {
    constexpr DWORD kWrite = GENERIC_WRITE | GENERIC_ALL | FILE_WRITE_DATA |
                             FILE_APPEND_DATA | FILE_WRITE_EA |
                             FILE_WRITE_ATTRIBUTES | DELETE | WRITE_DAC |
                             WRITE_OWNER;
    constexpr DWORD kUnusual = FILE_FLAG_NO_BUFFERING | FILE_FLAG_DELETE_ON_CLOSE |
                               FILE_FLAG_OPEN_REPARSE_POINT;

    return (access & (GENERIC_READ | FILE_READ_DATA)) && !(access & kWrite) &&
           disposition == OPEN_EXISTING && !(flags & kUnusual);
}

static bool ReadWholeFile(LPCWSTR path, std::vector<char>* bytes) {
    HANDLE file = CreateFileW_Original(
        path, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

    if (file == INVALID_HANDLE_VALUE) {
        return false;
    }

    LARGE_INTEGER size{};
    bool ok = GetFileSizeEx(file, &size) && size.QuadPart > 0 &&
              size.QuadPart <= kMaxBundledBytes;

    if (ok) {
        // A hook must not throw into After Effects: short of memory, the panel
        // simply gets its own file.
        try {
            bytes->resize(static_cast<size_t>(size.QuadPart));
        } catch (const std::bad_alloc&) {
            ok = false;
        }
    }

    if (ok) {
        DWORD read = 0;
        ok = ReadFile(file, bytes->data(), static_cast<DWORD>(bytes->size()), &read,
                      nullptr) &&
             read == bytes->size();
    }

    CloseHandle(file);

    return ok;
}

// Writes `value` in decimal at `out` and returns the end.
static wchar_t* AppendDecimal(wchar_t* out, unsigned long value) {
    wchar_t digits[12];
    int count = 0;

    do {
        digits[count++] = static_cast<wchar_t>(L'0' + value % 10);
        value /= 10;
    } while (value);

    while (count) {
        *out++ = digits[--count];
    }

    return out;
}

/*
    A temporary file holding `bytes`, handed back open with the caller's own
    access, security and flags. The mod writes it through a handle of its own
    that marks it delete-on-close, so Windows removes it once the caller's
    handle closes too, and also if After Effects exits without closing it.
*/
static HANDLE WriteTemporaryCopy(const std::vector<char>& bytes, BundledFile kind,
                                 DWORD access, DWORD share,
                                 LPSECURITY_ATTRIBUTES security, DWORD flags) {
    wchar_t folder[MAX_PATH + 1]{};
    DWORD length = GetTempPathW(ARRAYSIZE(folder), folder);

    if (!length || length >= ARRAYSIZE(folder)) {
        return INVALID_HANDLE_VALUE;
    }

    wchar_t name[MAX_PATH + 64]{};
    HANDLE writer = INVALID_HANDLE_VALUE;

    /*
        CREATE_NEW, never CREATE_ALWAYS: the name is predictable, so whatever
        already sits at it — a leftover, or a link planted to redirect the
        write — is stepped around rather than opened or followed.
    */
    for (int attempt = 0; attempt < 8 && writer == INVALID_HANDLE_VALUE; attempt++) {
        constexpr wchar_t kPrefix[] = L"after-effects-theme-";
        wchar_t* end = name;

        wmemcpy(end, folder, length);
        end += length;
        wmemcpy(end, kPrefix, ARRAYSIZE(kPrefix) - 1);
        end += ARRAYSIZE(kPrefix) - 1;
        end = AppendDecimal(end, GetCurrentProcessId());
        *end++ = L'-';
        end = AppendDecimal(
            end, static_cast<unsigned long>(InterlockedIncrement(&g_bundledSerial)));
        /*
            The copy stands in for the original, so it carries the same kind.
            A script substituted under a .css name is asking a runtime that
            looks at extensions to be surprised.
        */
        if (kind == BundledFile::Script) {
            wmemcpy(end, L".js", 4);  // and its terminator
        } else if (kind == BundledFile::Page) {
            wmemcpy(end, L".html", 6);
        } else {
            wmemcpy(end, L".css", 5);
        }

        writer = CreateFileW_Original(name, GENERIC_WRITE | DELETE,
                                      FILE_SHARE_READ | FILE_SHARE_DELETE, nullptr,
                                      CREATE_NEW,
                                      FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_DELETE_ON_CLOSE,
                                      nullptr);

        DWORD error = writer == INVALID_HANDLE_VALUE ? GetLastError() : ERROR_SUCCESS;

        if (error != ERROR_SUCCESS && error != ERROR_FILE_EXISTS &&
            error != ERROR_ALREADY_EXISTS) {
            break;
        }
    }

    if (writer == INVALID_HANDLE_VALUE) {
        return writer;
    }

    auto size = static_cast<DWORD>(bytes.size());
    DWORD written = 0;
    HANDLE copy = INVALID_HANDLE_VALUE;

    /*
        The caller's own share mode first, so the handle it gets back behaves
        like the one it asked for.

        It can legitimately fail. This handle is opened while the writer's is
        still open — it has to be, since the file is delete-on-close and would
        go the moment the writer let it go — so a caller asking for a share
        mode that excludes the writer's GENERIC_WRITE | DELETE gets a sharing
        violation. A mode wide enough to admit the writer is the fallback:
        only until CloseHandle below, after which the file waits for the
        caller's handle alone.
    */
    if (WriteFile(writer, bytes.data(), size, &written, nullptr) && written == size) {
        constexpr DWORD kAdmitsWriter =
            FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE;

        copy = CreateFileW_Original(name, access, share, security, OPEN_EXISTING,
                                    flags, nullptr);

        if (copy == INVALID_HANDLE_VALUE && share != kAdmitsWriter) {
            copy = CreateFileW_Original(name, access, kAdmitsWriter, security,
                                        OPEN_EXISTING, flags, nullptr);
        }
    }

    CloseHandle(writer);

    return copy;
}

/*
    The recolored copy of a bundled file, or INVALID_HANDLE_VALUE for
    the caller to open the file itself: when there is nothing to recolor, and
    when anything fails.
*/
static HANDLE OpenThemedBundledFile(LPCWSTR path, LPCWSTR relative,
                                    BundledFile kind, DWORD access, DWORD share,
                                    LPSECURITY_ATTRIBUTES security, DWORD flags) {
    /*
        Both the read and the copy go through CreateFileW's trampoline, so the
        CreateFile2 path needs it too — the two are hooked separately, and a
        kernelbase without one of them would otherwise be a null call.
    */
    if (!CreateFileW_Original) {
        return INVALID_HANDLE_VALUE;
    }

    std::vector<char> bytes;

    if (!ReadWholeFile(path, &bytes)) {
        return INVALID_HANDLE_VALUE;
    }

    size_t colors = kind == BundledFile::Stylesheet
                        ? RecolorStylesheet(bytes.data(), bytes.size())
                        : RecolorScript(bytes.data(), bytes.size());

    if (!colors) {
        return INVALID_HANDLE_VALUE;
    }

    HANDLE copy = WriteTemporaryCopy(bytes, kind, access, share, security, flags);

    if (copy == INVALID_HANDLE_VALUE) {
        DWORD error = GetLastError();

        if (Claim(&g_bundledFailureLogged)) {
            Wh_Log(L"could not write a recolored UXP file (%u); those panels "
                   L"keep their own colors",
                   error);
        }

        return INVALID_HANDLE_VALUE;
    }

    InterlockedIncrement(&g_bundledRecolored);
    const wchar_t* what = kind == BundledFile::Script ? L"script"
                          : kind == BundledFile::Page ? L"page"
                                                      : L"stylesheet";

    Wh_Log(L"UXP %s recolored: %s, %u colors", what, relative,
           static_cast<unsigned>(colors));

    // A successful open of an existing file reports no error.
    SetLastError(ERROR_SUCCESS);

    return copy;
}

HANDLE WINAPI CreateFileW_Hook(LPCWSTR path, DWORD access, DWORD share,
                               LPSECURITY_ATTRIBUTES security, DWORD disposition,
                               DWORD flags, HANDLE templateFile) {
    LPCWSTR relative = nullptr;

    BundledFile kind = CurrentSettings().uxpPanels &&
                               IsPlainRead(access, disposition, flags)
                           ? BundledFileKind(path, &relative)
                           : BundledFile::None;

    if (kind != BundledFile::None) {
        HANDLE copy = OpenThemedBundledFile(path, relative, kind, access, share,
                                            security, flags);

        if (copy != INVALID_HANDLE_VALUE) {
            return copy;
        }
    }

    return CreateFileW_Original(path, access, share, security, disposition, flags,
                                templateFile);
}

HANDLE WINAPI CreateFile2_Hook(LPCWSTR path, DWORD access, DWORD share,
                               DWORD disposition,
                               LPCREATEFILE2_EXTENDED_PARAMETERS parameters) {
    /*
        CreateFile2 splits what CreateFileW takes as one word, so both halves
        are put back together for the substitute — the attributes are the
        caller's to keep as much as the flags are.
    */
    DWORD flags = parameters ? (parameters->dwFileFlags | parameters->dwFileAttributes)
                             : 0;
    LPCWSTR relative = nullptr;

    BundledFile kind = CurrentSettings().uxpPanels &&
                               IsPlainRead(access, disposition, flags)
                           ? BundledFileKind(path, &relative)
                           : BundledFile::None;

    if (kind != BundledFile::None) {
        HANDLE copy = OpenThemedBundledFile(
            path, relative, kind, access, share,
            parameters ? parameters->lpSecurityAttributes : nullptr, flags);

        if (copy != INVALID_HANDLE_VALUE) {
            return copy;
        }
    }

    return CreateFile2_Original(path, access, share, disposition, parameters);
}

// ============================================================================
// NATIVE WINDOWS DARK MODE
// ============================================================================

enum class PreferredAppMode { Default, AllowDark, ForceDark, ForceLight, Max };

using AllowDarkModeForWindow_t = bool(WINAPI*)(HWND, bool);
using SetPreferredAppMode_t = PreferredAppMode(WINAPI*)(PreferredAppMode);
using AllowDarkModeForApp_t = bool(WINAPI*)(bool);
using FlushMenuThemes_t = void(WINAPI*)();
using RefreshImmersiveColorPolicyState_t = void(WINAPI*)();
using SetWindowTheme_t = HRESULT(WINAPI*)(HWND, LPCWSTR, LPCWSTR);

AllowDarkModeForWindow_t g_AllowDarkModeForWindow = nullptr;
SetPreferredAppMode_t g_SetPreferredAppMode = nullptr;
AllowDarkModeForApp_t g_AllowDarkModeForApp = nullptr;
FlushMenuThemes_t g_FlushMenuThemes = nullptr;
RefreshImmersiveColorPolicyState_t g_RefreshImmersiveColorPolicyState = nullptr;
SetWindowTheme_t g_SetWindowTheme = nullptr;

HMODULE g_uxtheme = nullptr;
DWORD g_buildNumber = 0;

using GetDpiForWindow_t = UINT(WINAPI*)(HWND);
GetDpiForWindow_t g_GetDpiForWindow = nullptr;  // Windows 10 1607 and later

template <typename Function>
static Function UxThemeProc(const char* name) {
    return g_uxtheme ? reinterpret_cast<Function>(GetProcAddress(g_uxtheme, name))
                     : nullptr;
}

template <typename Function>
static Function UxThemeOrdinal(WORD ordinal) {
    return g_uxtheme ? reinterpret_cast<Function>(
                           GetProcAddress(g_uxtheme, MAKEINTRESOURCEA(ordinal)))
                     : nullptr;
}

static DWORD GetWindowsBuild() {
    using RtlGetNtVersionNumbers_t = void(WINAPI*)(DWORD*, DWORD*, DWORD*);

    HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");

    if (!ntdll) {
        return 0;
    }

    auto fn = reinterpret_cast<RtlGetNtVersionNumbers_t>(
        GetProcAddress(ntdll, "RtlGetNtVersionNumbers"));

    if (!fn) {
        return 0;
    }

    DWORD major = 0;
    DWORD minor = 0;
    DWORD build = 0;

    fn(&major, &minor, &build);

    return build & 0x0FFFFFFF;
}

/*
    Turns the process-wide app mode on or off; a settings change calls it too.
    Only a mode this mod set is ever undone: with "Window and system dialogs"
    off from the start nothing is called, not even to set Default, which would
    overwrite a mode something else in the process chose.
*/
bool g_appModeForced = false;

static void ApplyAppMode(bool dark) {
    if (dark == g_appModeForced) {
        return;
    }

    if (g_buildNumber < 18362) {
        if (g_AllowDarkModeForApp) {
            g_AllowDarkModeForApp(dark);
        }
    } else if (g_SetPreferredAppMode) {
        g_SetPreferredAppMode(dark ? PreferredAppMode::ForceDark
                                   : PreferredAppMode::Default);
    }

    if (g_RefreshImmersiveColorPolicyState) {
        g_RefreshImmersiveColorPolicyState();
    }

    if (g_FlushMenuThemes) {
        g_FlushMenuThemes();
    }

    g_appModeForced = dark;
}

/*
    The uxtheme dark mode functions have no names — only ordinals — and ordinal 135
    changed signature between Windows 10 1809 and 1903. Calling the wrong signature
    passes garbage on the stack, so the Windows version decides which of the two to
    resolve.
*/
static void InitNativeDarkMode() {
    g_buildNumber = GetWindowsBuild();

    HMODULE user32 = GetModuleHandleW(L"user32.dll");

    g_GetDpiForWindow = user32 ? reinterpret_cast<GetDpiForWindow_t>(
                                     GetProcAddress(user32, "GetDpiForWindow"))
                               : nullptr;

    g_uxtheme =
        LoadLibraryExW(L"uxtheme.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);

    if (!g_uxtheme) {
        return;
    }

    g_SetWindowTheme = reinterpret_cast<SetWindowTheme_t>(
        GetProcAddress(g_uxtheme, "SetWindowTheme"));

    if (g_buildNumber < 17763) {
        Wh_Log(L"build %u has no native dark mode", g_buildNumber);
        return;
    }

    g_RefreshImmersiveColorPolicyState =
        reinterpret_cast<RefreshImmersiveColorPolicyState_t>(
            GetProcAddress(g_uxtheme, MAKEINTRESOURCEA(104)));

    g_AllowDarkModeForWindow = reinterpret_cast<AllowDarkModeForWindow_t>(
        GetProcAddress(g_uxtheme, MAKEINTRESOURCEA(133)));

    g_FlushMenuThemes = reinterpret_cast<FlushMenuThemes_t>(
        GetProcAddress(g_uxtheme, MAKEINTRESOURCEA(136)));

    FARPROC ordinal135 = GetProcAddress(g_uxtheme, MAKEINTRESOURCEA(135));

    if (g_buildNumber < 18362) {
        g_AllowDarkModeForApp = reinterpret_cast<AllowDarkModeForApp_t>(ordinal135);
    } else {
        g_SetPreferredAppMode =
            reinterpret_cast<SetPreferredAppMode_t>(ordinal135);
    }

    /*
        Everything above is a lookup and changes nothing; the menu hooks need
        those entry points whatever the settings say. The app mode itself —
        scrollbars, common dialogs, control themes — belongs to "Window and
        system dialogs" and answers to it.
    */
    ApplyAppMode(CurrentSettings().nativeDarkMode);
}

/*
    DWMWA_USE_IMMERSIVE_DARK_MODE is 20 from build 18985 (Windows 10 2004) on.
    Before that the same attribute was 19, and DWM rejects 20 there — on 1809,
    1903 and 1909 the frame would never go dark.
*/
static DWORD ImmersiveDarkModeAttribute() {
    return g_buildNumber >= 18985 ? 20 : 19;
}

constexpr DWORD kBorderColor = 34;
constexpr DWORD kCaptionColor = 35;
constexpr DWORD kTextColor = 36;
constexpr COLORREF kDwmColorDefault = 0xFFFFFFFF;  // DWMWA_COLOR_DEFAULT

/*
    The windows this mod changed, and what it changed on each, so the revert
    undoes exactly that and never touches a window the mod left alone.
    Destroyed windows are not removed as they go: the map drops dead handles
    whenever it doubles, and the revert re-checks each handle.

    Which leaves one narrow window: a handle reused between two prunes carries
    the old window's bits into the new one, and the revert then undoes on it
    something the mod set on its predecessor. The cost is bounded and it is
    paid once, at unload — immersive dark mode back to FALSE and the caption
    colors back to the DWM default, which is where an After Effects window that the
    mod did not touch already sits, or a theme class cleared on a window that
    had one. Telling the two apart would need the destroy notification this
    mod deliberately does not hook, for a window whose frame the revert is
    about to set to what it already is.
*/
constexpr BYTE kThemedClass = 1;  // DarkMode_Explorer, on the classes WantsExplorerTheme names
constexpr BYTE kThemedFrame = 2;  // immersive dark mode, and caption colors on 22000+

SRWLOCK g_themedLock = SRWLOCK_INIT;
std::unordered_map<HWND, BYTE> g_themedWindows;
size_t g_themedPruneAt = 256;

static void RememberThemedWindow(HWND hwnd, BYTE applied) {
    if (!applied) {
        return;
    }

    AcquireSRWLockExclusive(&g_themedLock);

    /*
        Growing the map allocates, and this runs inside CreateWindowEx. Short
        of memory the window simply goes unrecorded, and the revert then
        leaves it alone — the same as for a window that existed before the mod
        loaded. Letting the throw out would unwind into user32 and leave this
        lock held for the rest of the session.
    */
    try {
        g_themedWindows[hwnd] |= applied;

        if (g_themedWindows.size() >= g_themedPruneAt) {
            std::erase_if(g_themedWindows,
                          [](const auto& entry) { return !IsWindow(entry.first); });
            g_themedPruneAt = std::max<size_t>(256, g_themedWindows.size() * 2);
        }
    } catch (const std::bad_alloc&) {
    }

    ReleaseSRWLockExclusive(&g_themedLock);
}

/*
    Whoever calls SetWindowTheme on a window after the mod did owns its theme
    class from then on, and the revert has to leave it alone: clearing it
    would drop the class that caller chose, not the one the mod set. The mod's
    own call in ApplyDarkModeToWindow comes through here as well, and records
    the window again right after.
*/
static void ForgetThemedClass(HWND hwnd) {
    /*
        This runs on every SetWindowTheme in the process, almost none of them
        for a window the mod recorded. So the lookup is shared — several
        callers at once, and no writer shut out — and the exclusive lock is
        only taken for a window that is actually in the map. ForgetMenuTheme
        does the same for the same reason.
    */
    AcquireSRWLockShared(&g_themedLock);
    bool known = g_themedWindows.find(hwnd) != g_themedWindows.end();
    ReleaseSRWLockShared(&g_themedLock);

    if (!known) {
        return;
    }

    AcquireSRWLockExclusive(&g_themedLock);

    // Looked up again: the shared lock was let go, so the entry may be gone.
    auto entry = g_themedWindows.find(hwnd);

    if (entry != g_themedWindows.end()) {
        entry->second &= ~kThemedClass;

        if (!entry->second) {
            g_themedWindows.erase(entry);
        }
    }

    ReleaseSRWLockExclusive(&g_themedLock);
}

/*
    DWMWA_CAPTION_COLOR exists from Windows 11 (build 22000) on. On an earlier build
    the call returns an error and the bar keeps the default dark mode gray —
    acceptable degradation, not a failure — so it is not even attempted.
*/
static void SetFrameColors(HWND hwnd, COLORREF border, COLORREF caption,
                           COLORREF text) {
    if (g_buildNumber < 22000) {
        return;
    }

    DwmSetWindowAttribute(hwnd, static_cast<DWMWINDOWATTRIBUTE>(kBorderColor),
                          &border, sizeof(border));
    DwmSetWindowAttribute(hwnd, static_cast<DWMWINDOWATTRIBUTE>(kCaptionColor),
                          &caption, sizeof(caption));
    DwmSetWindowAttribute(hwnd, static_cast<DWMWINDOWATTRIBUTE>(kTextColor), &text,
                          sizeof(text));
}

/*
    The classes DarkMode_Explorer changes anything for. SetWindowTheme sends
    WM_THEMECHANGED — from the hook, inside CreateWindowEx, before the creator
    has its handle — and MFC and ProfUIS windows are known to deadlock on it;
    After Effects hosts third-party plugin dialogs. Every other window only gets
    AllowDarkModeForWindow, which sends nothing.
*/
static bool WantsExplorerTheme(HWND hwnd) {
    wchar_t name[64];

    if (!GetClassNameW(hwnd, name, ARRAYSIZE(name))) {
        return false;
    }

    return _wcsicmp(name, L"SysListView32") == 0 ||
           _wcsicmp(name, L"SysTreeView32") == 0 ||
           _wcsicmp(name, L"SysHeader32") == 0 || _wcsicmp(name, L"ScrollBar") == 0;
}

static void ApplyDarkModeToWindow(HWND hwnd) {
    if (!hwnd || !CurrentSettings().nativeDarkMode) {
        return;
    }

    BYTE applied = 0;

    if (g_AllowDarkModeForWindow) {
        g_AllowDarkModeForWindow(hwnd, true);
    }

    if (g_SetWindowTheme && WantsExplorerTheme(hwnd) &&
        SUCCEEDED(g_SetWindowTheme(hwnd, L"DarkMode_Explorer", nullptr))) {
        applied |= kThemedClass;
    }

    /*
        Frame attributes only apply to a top-level window that has a frame.

        The style bit and not GetParent, because for an owned popup such as a
        dialog GetParent returns the owner. And a caption or a sizing border,
        because everything else here is a window with no frame to color:
        After Effects opens menu popups, tooltips and combo dropdowns constantly,
        and each one was costing four round trips to DWM plus an insert under
        the themed-window lock, inside CreateWindowEx, to set colors on a
        frame that does not exist.

        A window that gains a caption after it is created is picked up the
        next time ApplyThemeToExistingWindows runs, which a settings change
        does.
    */
    constexpr LONG_PTR kFramed = WS_CAPTION | WS_THICKFRAME;
    LONG_PTR style = GetWindowLongPtrW(hwnd, GWL_STYLE);

    if (!(style & WS_CHILD) && (style & kFramed)) {
        BOOL dark = TRUE;

        DwmSetWindowAttribute(hwnd,
                              static_cast<DWMWINDOWATTRIBUTE>(ImmersiveDarkModeAttribute()),
                              &dark, sizeof(dark));

        const Palette& p = CurrentSettings().palette;
        SetFrameColors(hwnd, p.ramp[4], p.ramp[1], p.text);

        applied |= kThemedFrame;
    }

    RememberThemedWindow(hwnd, applied);
}

/*
    Undoes what ApplyDarkModeToWindow recorded for one window.

    Immersive dark mode goes back to FALSE, which is the value these windows
    had: no module After Effects 2026 ships imports DwmSetWindowAttribute.
    The caption colors go back to DWMWA_COLOR_DEFAULT.

    SetWindowTheme(nullptr, nullptr) clears the theme class, since uxtheme
    cannot read the earlier one back; that is why only windows the apply
    changed, and nobody re-themed since, are cleared. AllowDarkModeForWindow
    stays: it is a permission that only acts while the app mode allows dark.
*/
static void RevertWindow(HWND hwnd, BYTE applied) {
    if ((applied & kThemedClass) && g_SetWindowTheme) {
        g_SetWindowTheme(hwnd, nullptr, nullptr);
    }

    if (applied & kThemedFrame) {
        BOOL dark = FALSE;

        DwmSetWindowAttribute(hwnd,
                              static_cast<DWMWINDOWATTRIBUTE>(ImmersiveDarkModeAttribute()),
                              &dark, sizeof(dark));

        SetFrameColors(hwnd, kDwmColorDefault, kDwmColorDefault, kDwmColorDefault);
    }
}

static bool IsOwnWindow(HWND hwnd) {
    DWORD pid = 0;

    // GetWindowThreadProcessId returns zero for a window that no longer exists.
    return GetWindowThreadProcessId(hwnd, &pid) && pid == GetCurrentProcessId();
}

/*
    Runs on unload, and when "Window and system dialogs" is switched off.

    The map is swapped out before any window is touched: SetWindowTheme waits
    for the window's thread, which may at that moment be creating a window and
    waiting for g_themedLock in RememberThemedWindow.
*/
static void RevertThemedWindows() {
    std::unordered_map<HWND, BYTE> windows;

    AcquireSRWLockExclusive(&g_themedLock);
    windows.swap(g_themedWindows);
    g_themedPruneAt = 256;
    ReleaseSRWLockExclusive(&g_themedLock);

    for (const auto& [hwnd, applied] : windows) {
        // Destroyed since, or the handle value now belongs to another process.
        if (IsOwnWindow(hwnd)) {
            RevertWindow(hwnd, applied);
        }
    }
}

/*
    A palette change with the frame on. Only the caption colors depend on the
    palette; the theme class and immersive dark mode stay as they are, so no
    window gets a WM_THEMECHANGED for this.
*/
static void RecolorThemedFrames() {
    if (g_buildNumber < 22000) {
        return;
    }

    std::unordered_map<HWND, BYTE> windows;

    // The copy allocates; the lock is released whatever it does, and short of
    // memory the captions simply keep the color they have.
    AcquireSRWLockShared(&g_themedLock);

    try {
        windows = g_themedWindows;
    } catch (const std::bad_alloc&) {
        windows.clear();
    }

    ReleaseSRWLockShared(&g_themedLock);

    const Palette& p = CurrentSettings().palette;

    for (const auto& [hwnd, applied] : windows) {
        if ((applied & kThemedFrame) && IsOwnWindow(hwnd)) {
            SetFrameColors(hwnd, p.ramp[4], p.ramp[1], p.text);
        }
    }
}

/*
    Asks every window of the process to repaint, frame included, after a
    settings change and on unload. RedrawWindow only invalidates, so this waits
    on no UI thread. RDW_ERASE because native dialogs paint their background in
    WM_ERASEBKGND.
*/
static BOOL CALLBACK RedrawTopLevel(HWND hwnd, LPARAM) {
    if (IsOwnWindow(hwnd)) {
        RedrawWindow(hwnd, nullptr, nullptr,
                     RDW_INVALIDATE | RDW_ERASE | RDW_FRAME | RDW_ALLCHILDREN);
    }

    return TRUE;
}

static void RedrawProcessWindows() {
    EnumWindows(RedrawTopLevel, 0);
}

static BOOL CALLBACK ApplyToChild(HWND hwnd, LPARAM) {
    ApplyDarkModeToWindow(hwnd);
    return TRUE;
}

static BOOL CALLBACK ApplyToTopLevel(HWND hwnd, LPARAM) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);

    if (pid != GetCurrentProcessId()) {
        return TRUE;
    }

    ApplyDarkModeToWindow(hwnd);
    EnumChildWindows(hwnd, ApplyToChild, 0);

    /*
        The menu bar is non-client area: it does not repaint on WM_PAINT or on an
        ordinary window invalidation. Without this nudge the strip only darkens once
        something else forces it to redraw.
    */
    if (GetMenu(hwnd)) {
        DrawMenuBar(hwnd);
    }

    return TRUE;
}

static void ApplyThemeToExistingWindows() {
    EnumWindows(ApplyToTopLevel, 0);
}

static BOOL CALLBACK NoteFramedWindow(HWND hwnd, LPARAM found) {
    if (!IsOwnWindow(hwnd) ||
        !(GetWindowLongPtrW(hwnd, GWL_STYLE) & WS_CAPTION)) {
        return TRUE;
    }

    *reinterpret_cast<bool*>(found) = true;
    return FALSE;  // one is the whole answer
}

/*
    Whether this process already has a window with a frame, which After
    Effects' main window and its dialogs have and its tooltips and menu popups
    do not. It is how the mod tells "the UI is up" from "After Effects is still
    starting".
*/
static bool HasFramedWindow() {
    bool found = false;
    EnumWindows(NoteFramedWindow, reinterpret_cast<LPARAM>(&found));
    return found;
}

// ============================================================================
// CreateWindowExW HOOK
// ============================================================================

using CreateWindowExW_t = HWND(WINAPI*)(DWORD, LPCWSTR, LPCWSTR, DWORD, int, int,
                                        int, int, HWND, HMENU, HINSTANCE, LPVOID);

CreateWindowExW_t CreateWindowExW_Original = nullptr;

HWND WINAPI CreateWindowExW_Hook(DWORD exStyle, LPCWSTR className,
                                 LPCWSTR windowName, DWORD style, int x, int y,
                                 int width, int height, HWND parent, HMENU menu,
                                 HINSTANCE instance, LPVOID param) {
    HWND hwnd = CreateWindowExW_Original(exStyle, className, windowName, style, x,
                                         y, width, height, parent, menu, instance,
                                         param);

    // A message-only window never shows, so there is nothing to theme.
    if (hwnd && parent != HWND_MESSAGE) {
        ApplyDarkModeToWindow(hwnd);
    }

    return hwnd;
}

/*
    CreateWindowExA does not route through CreateWindowExW — each goes to
    user32's internal create path on its own — so without this an ANSI caller
    would create windows the frame never reaches.
*/
using CreateWindowExA_t = HWND(WINAPI*)(DWORD, LPCSTR, LPCSTR, DWORD, int, int,
                                        int, int, HWND, HMENU, HINSTANCE, LPVOID);

CreateWindowExA_t CreateWindowExA_Original = nullptr;

HWND WINAPI CreateWindowExA_Hook(DWORD exStyle, LPCSTR className,
                                 LPCSTR windowName, DWORD style, int x, int y,
                                 int width, int height, HWND parent, HMENU menu,
                                 HINSTANCE instance, LPVOID param) {
    HWND hwnd = CreateWindowExA_Original(exStyle, className, windowName, style, x,
                                         y, width, height, parent, menu, instance,
                                         param);

    // A message-only window never shows, so there is nothing to theme.
    if (hwnd && parent != HWND_MESSAGE) {
        ApplyDarkModeToWindow(hwnd);
    }

    return hwnd;
}

SetWindowTheme_t SetWindowTheme_Original = nullptr;

// See ForgetThemedClass.
HRESULT WINAPI SetWindowTheme_Hook(HWND hwnd, LPCWSTR subAppName,
                                   LPCWSTR subIdList) {
    ForgetThemedClass(hwnd);

    return SetWindowTheme_Original(hwnd, subAppName, subIdList);
}

// ============================================================================
// SYSTEM COLORS
// ============================================================================

/*
    GetSysColorBrush promises a brush valid for the life of the process, so a
    brush this mod hands out is never deleted. Keyed by color, so switching
    back to a palette already seen adds nothing; past the cap, the system brush
    is returned instead.
*/
struct SysBrush {
    COLORREF color;
    HBRUSH brush;
};

constexpr size_t kMaxSysBrushes = 256;

SysBrush g_sysBrushes[kMaxSysBrushes]{};
size_t g_sysBrushCount = 0;
volatile LONG g_sysBrushesFullLogged = FALSE;
SRWLOCK g_brushLock = SRWLOCK_INIT;

static bool MapSysColor(int index, COLORREF* out) {
    const Palette& p = CurrentSettings().palette;

    switch (index) {
        case COLOR_WINDOW:
        case COLOR_MENU:
        case COLOR_MENUBAR:
        case COLOR_BTNFACE:  // == COLOR_3DFACE
        case COLOR_SCROLLBAR:
        case COLOR_INACTIVECAPTION:
        case COLOR_ACTIVECAPTION:
        case COLOR_APPWORKSPACE:
        case COLOR_INFOBK:
            *out = p.ramp[1];
            return true;

        case COLOR_WINDOWFRAME:
        case COLOR_BTNSHADOW:
        case COLOR_3DDKSHADOW:
        case COLOR_BTNHIGHLIGHT:
        case COLOR_3DLIGHT:
        case COLOR_ACTIVEBORDER:
        case COLOR_INACTIVEBORDER:
            *out = p.ramp[4];
            return true;

        case COLOR_WINDOWTEXT:
        case COLOR_BTNTEXT:
        case COLOR_MENUTEXT:
        case COLOR_CAPTIONTEXT:
        case COLOR_INFOTEXT:
            *out = p.text;
            return true;

        case COLOR_GRAYTEXT:
        case COLOR_INACTIVECAPTIONTEXT:
            *out = p.dimText;
            return true;

        case COLOR_MENUHILIGHT:
        case COLOR_HOTLIGHT:
            *out = p.accent;
            return true;

        default:
            return false;  // selection and highlight stay as Windows defined them
    }
}

using GetSysColor_t = DWORD(WINAPI*)(int);
using GetSysColorBrush_t = HBRUSH(WINAPI*)(int);
using FillRect_t = int(WINAPI*)(HDC, const RECT*, HBRUSH);

GetSysColor_t GetSysColor_Original = nullptr;
GetSysColorBrush_t GetSysColorBrush_Original = nullptr;
FillRect_t FillRect_Original = nullptr;

DWORD WINAPI GetSysColor_Hook(int index) {
    COLORREF mapped;

    if (CurrentSettings().nativeDarkMode && MapSysColor(index, &mapped)) {
        return mapped;
    }

    return GetSysColor_Original(index);
}

static HBRUSH FindSysBrush(COLORREF color) {
    for (size_t i = 0; i < g_sysBrushCount; i++) {
        if (g_sysBrushes[i].color == color) {
            return g_sysBrushes[i].brush;
        }
    }

    return nullptr;
}

/*
    The shared lock covers the common case — a color already cached — so
    concurrent FillRect calls with a system color do not queue behind one
    another. Only a color seen for the first time takes the exclusive lock.
*/
static HBRUSH ThemeSysBrush(int index) {
    COLORREF mapped;

    if (!MapSysColor(index, &mapped)) {
        return nullptr;
    }

    AcquireSRWLockShared(&g_brushLock);
    HBRUSH brush = FindSysBrush(mapped);
    ReleaseSRWLockShared(&g_brushLock);

    if (brush) {
        return brush;
    }

    AcquireSRWLockExclusive(&g_brushLock);

    brush = FindSysBrush(mapped);
    bool full = !brush && g_sysBrushCount >= kMaxSysBrushes;

    if (!brush && !full) {
        brush = CreateSolidBrush(mapped);

        if (brush) {
            g_sysBrushes[g_sysBrushCount] = {mapped, brush};
            g_sysBrushCount++;
        }
    }

    ReleaseSRWLockExclusive(&g_brushLock);

    if (full && Claim(&g_sysBrushesFullLogged)) {
        Wh_Log(L"system brush cache full; new palette colors use the Windows "
               L"brushes");
    }

    return brush;
}

HBRUSH WINAPI GetSysColorBrush_Hook(int index) {
    if (CurrentSettings().nativeDarkMode) {
        HBRUSH brush = ThemeSysBrush(index);

        if (brush) {
            return brush;
        }
    }

    return GetSysColorBrush_Original(index);
}

/*
    FillRect accepts (HBRUSH)(COLOR_X + 1) in place of a real brush, and on that
    path it resolves the color internally without going through GetSysColorBrush.
    Without this hook half the native surfaces would stay light while the other half
    darkened.
*/
int WINAPI FillRect_Hook(HDC hdc, const RECT* rect, HBRUSH brush) {
    ULONG_PTR raw = reinterpret_cast<ULONG_PTR>(brush);

    if (CurrentSettings().nativeDarkMode && raw >= 1 &&
        raw <= static_cast<ULONG_PTR>(COLOR_MENUBAR) + 1) {
        HBRUSH replacement = ThemeSysBrush(static_cast<int>(raw) - 1);

        if (replacement) {
            brush = replacement;
        }
    }

    return FillRect_Original(hdc, rect, brush);
}

// ============================================================================
// MENU BAR AND MENUS
// ============================================================================

/*
    Menus take their theme from OpenThemeData and, for non-client parts, from
    OpenNcThemeData, which exists only as uxtheme ordinal 49; both are hooked.
    DrawThemeBackground receives an opaque HTHEME with no API to read its class
    back, so menu themes are noted when they are opened.
*/

constexpr int kMenuBarBackground = 7;
constexpr int kMenuBarItem = 8;
constexpr int kMenuPopupBackground = 9;
constexpr int kMenuPopupBorders = 10;
constexpr int kMenuPopupGutter = 13;
constexpr int kMenuPopupItem = 14;
constexpr int kMenuPopupSeparator = 15;

constexpr size_t kThemeSlots = 256;

HTHEME g_menuThemes[kThemeSlots]{};
SRWLOCK g_themeLock = SRWLOCK_INIT;

// How many slots have ever been used, which bounds every scan.
size_t g_menuThemeHighWater = 0;

volatile LONG g_menuThemesFullLogged = FALSE;

static void RememberMenuTheme(HTHEME theme) {
    AcquireSRWLockExclusive(&g_themeLock);

    bool stored = false;

    for (size_t i = 0; i < g_menuThemeHighWater; i++) {
        if (g_menuThemes[i] == theme) {
            stored = true;
            break;
        }
    }

    for (size_t i = 0; i < kThemeSlots && !stored; i++) {
        if (!g_menuThemes[i]) {
            g_menuThemes[i] = theme;
            g_menuThemeHighWater = std::max(g_menuThemeHighWater, i + 1);
            stored = true;
        }
    }

    ReleaseSRWLockExclusive(&g_themeLock);

    if (!stored && Claim(&g_menuThemesFullLogged)) {
        Wh_Log(L"menu theme list full; menus opened from now on are not "
               L"repainted in the palette");
    }
}

static bool IsMenuTheme(HTHEME theme) {
    if (!g_menuThemeHighWater) {
        return false;
    }

    bool found = false;

    AcquireSRWLockShared(&g_themeLock);

    for (size_t i = 0; i < g_menuThemeHighWater; i++) {
        if (g_menuThemes[i] == theme) {
            found = true;
            break;
        }
    }

    ReleaseSRWLockShared(&g_themeLock);

    return found;
}

/*
    Runs on every CloseThemeData in the process, almost none of them for a
    menu theme. So it asks IsMenuTheme first — nothing at all until a menu
    theme has been seen, a shared scan after that — and only takes the lock
    exclusively for a handle that is actually in the list.
*/
static void ForgetMenuTheme(HTHEME theme) {
    if (!IsMenuTheme(theme)) {
        return;
    }

    AcquireSRWLockExclusive(&g_themeLock);

    for (size_t i = 0; i < g_menuThemeHighWater; i++) {
        if (g_menuThemes[i] == theme) {
            g_menuThemes[i] = nullptr;
            break;
        }
    }

    ReleaseSRWLockExclusive(&g_themeLock);
}

using OpenThemeData_t = HTHEME(WINAPI*)(HWND, LPCWSTR);
using OpenThemeDataForDpi_t = HTHEME(WINAPI*)(HWND, LPCWSTR, UINT);
using OpenNcThemeData_t = HTHEME(WINAPI*)(HWND, LPCWSTR);
using OpenThemeDataEx_t = HTHEME(WINAPI*)(HWND, LPCWSTR, DWORD);
using CloseThemeData_t = HRESULT(WINAPI*)(HTHEME);
using DrawThemeBackground_t = HRESULT(WINAPI*)(HTHEME, HDC, int, int, const RECT*,
                                               const RECT*);
using DrawThemeBackgroundEx_t = HRESULT(WINAPI*)(HTHEME, HDC, int, int,
                                                 const RECT*, const void*);
using DrawThemeText_t = HRESULT(WINAPI*)(HTHEME, HDC, int, int, LPCWSTR, int,
                                         DWORD, DWORD, const RECT*);
using DrawThemeTextEx_t = HRESULT(WINAPI*)(HTHEME, HDC, int, int, LPCWSTR, int,
                                           DWORD, RECT*, const void*);

OpenThemeData_t OpenThemeData_Original = nullptr;
OpenThemeDataForDpi_t OpenThemeDataForDpi_Original = nullptr;
OpenNcThemeData_t OpenNcThemeData_Original = nullptr;
OpenThemeDataEx_t OpenThemeDataEx_Original = nullptr;
CloseThemeData_t CloseThemeData_Original = nullptr;
DrawThemeBackground_t DrawThemeBackground_Original = nullptr;
DrawThemeBackgroundEx_t DrawThemeBackgroundEx_Original = nullptr;
DrawThemeText_t DrawThemeText_Original = nullptr;
DrawThemeTextEx_t DrawThemeTextEx_Original = nullptr;

/*
    The class asked for is swapped, not just painted over. With the system in
    light mode:

        OpenNcThemeData(nullptr, L"Menu")            -> text #000000  (light)
        OpenNcThemeData(nullptr, L"DarkMode::Menu")  -> text #FFFFFF  (dark)

    "Menu" only turns dark after SetPreferredAppMode(ForceDark) and
    FlushMenuThemes, and user32 caches the menu theme on a window's first paint,
    which can come before the mod acts; the text, metrics and glyphs would then
    stay light. "DarkMode::Menu" is dark regardless. The result is checked,
    because the class can be missing.
*/
template <typename Open>
static HTHEME OpenDarkMenuTheme(HWND hwnd, LPCWSTR classList, Open open) {
    if (!CurrentSettings().menuHook || !classList) {
        return nullptr;
    }

    if (_wcsicmp(classList, L"Menu") != 0) {
        return nullptr;
    }

    // Window first: it carries the DPI, and menu metrics depend on it. The
    // retry without one covers a window uxtheme will not open a theme for,
    // and all four openers take it, since any of them can be the one that
    // fails.
    HTHEME theme = open(hwnd, L"DarkMode::Menu");

    if (!theme && hwnd) {
        theme = open(nullptr, L"DarkMode::Menu");
    }

    return theme;
}

static HTHEME TrackMenuTheme(HTHEME theme, LPCWSTR classList) {
    if (!theme) {
        return theme;
    }

    /*
        Exactly "Menu", the same test OpenDarkMenuTheme uses to decide what to
        swap. A substring match would also register any class whose name merely
        contains "menu", and PaintMenuPart would then repaint parts 7-15 of it
        with menu colors — part numbers mean different things in different
        classes, so that shows up as a corrupted control.
    */
    if (classList && _wcsicmp(classList, L"Menu") == 0) {
        RememberMenuTheme(theme);
    } else {
        /*
            A menu theme's handle is not freed only through the CloseThemeData
            hooked below. uxtheme releases non-client themes itself when the
            owning window is destroyed or the theme or DPI changes, none of
            which comes through that export, and then hands the same value back
            for another class. Dropping it the moment it reappears as a
            non-menu class keeps a stale handle from making PaintMenuPart
            repaint that control in menu colors. All four public openers are
            hooked — OpenThemeData, OpenThemeDataForDpi, OpenThemeDataEx and
            OpenNcThemeData — so whichever one hands the value back, it is seen.
        */
        ForgetMenuTheme(theme);
    }

    return theme;
}

HTHEME WINAPI OpenThemeData_Hook(HWND hwnd, LPCWSTR classList) {
    auto open = [](HWND w, LPCWSTR c) { return OpenThemeData_Original(w, c); };
    HTHEME dark = OpenDarkMenuTheme(hwnd, classList, open);

    if (dark) {
        return TrackMenuTheme(dark, classList);
    }

    return TrackMenuTheme(OpenThemeData_Original(hwnd, classList), classList);
}

HTHEME WINAPI OpenThemeDataForDpi_Hook(HWND hwnd, LPCWSTR classList, UINT dpi) {
    auto open = [dpi](HWND w, LPCWSTR c) {
        return OpenThemeDataForDpi_Original(w, c, dpi);
    };
    HTHEME dark = OpenDarkMenuTheme(hwnd, classList, open);

    if (dark) {
        return TrackMenuTheme(dark, classList);
    }

    return TrackMenuTheme(OpenThemeDataForDpi_Original(hwnd, classList, dpi),
                          classList);
}

// comctl32's opener when it wants OTD_NONCLIENT or OTD_FORCE_RECT_SIZING.
HTHEME WINAPI OpenThemeDataEx_Hook(HWND hwnd, LPCWSTR classList, DWORD flags) {
    auto open = [flags](HWND w, LPCWSTR c) {
        return OpenThemeDataEx_Original(w, c, flags);
    };
    HTHEME dark = OpenDarkMenuTheme(hwnd, classList, open);

    if (dark) {
        return TrackMenuTheme(dark, classList);
    }

    return TrackMenuTheme(OpenThemeDataEx_Original(hwnd, classList, flags), classList);
}

// Non-client menu themes, the menu bar's among them, come through here.
HTHEME WINAPI OpenNcThemeData_Hook(HWND hwnd, LPCWSTR classList) {
    auto open = [](HWND w, LPCWSTR c) { return OpenNcThemeData_Original(w, c); };
    HTHEME dark = OpenDarkMenuTheme(hwnd, classList, open);

    if (dark) {
        return TrackMenuTheme(dark, classList);
    }

    return TrackMenuTheme(OpenNcThemeData_Original(hwnd, classList), classList);
}

HRESULT WINAPI CloseThemeData_Hook(HTHEME theme) {
    ForgetMenuTheme(theme);
    return CloseThemeData_Original(theme);
}

static void FillWith(HDC hdc, const RECT* rect, COLORREF color) {
    HBRUSH brush = CreateSolidBrush(color);

    if (!brush) {
        return;
    }

    if (FillRect_Original) {
        FillRect_Original(hdc, rect, brush);
    } else {
        FillRect(hdc, rect, brush);
    }

    DeleteObject(brush);
}

static bool PaintMenuPart(HDC hdc, int part, int state, const RECT* rect) {
    const Palette& p = CurrentSettings().palette;

    switch (part) {
        case kMenuBarBackground:
            FillWith(hdc, rect, p.ramp[1]);
            return true;

        case kMenuBarItem:
            // 2 = hot, 3 = pushed
            FillWith(hdc, rect, (state == 2 || state == 3) ? p.accent : p.ramp[1]);
            return true;

        case kMenuPopupBackground:
        case kMenuPopupGutter:
            FillWith(hdc, rect, p.ramp[1]);
            return true;

        case kMenuPopupBorders: {
            HBRUSH brush = CreateSolidBrush(p.ramp[4]);

            if (brush) {
                FrameRect(hdc, rect, brush);
                DeleteObject(brush);
            }

            return true;
        }

        case kMenuPopupItem:
            // 2 = hot, 4 = disabled hot
            FillWith(hdc, rect, (state == 2 || state == 4) ? p.accent : p.ramp[1]);
            return true;

        case kMenuPopupSeparator: {
            FillWith(hdc, rect, p.ramp[1]);

            RECT line = *rect;
            line.top = (rect->top + rect->bottom) / 2;
            line.bottom = line.top + 1;

            FillWith(hdc, &line, p.ramp[4]);

            return true;
        }

        default:
            return false;  // check and submenu glyphs are left to the theme
    }
}

HRESULT WINAPI DrawThemeBackground_Hook(HTHEME theme, HDC hdc, int part, int state,
                                        const RECT* rect, const RECT* clip) {
    if (CurrentSettings().menuHook && rect && IsMenuTheme(theme) &&
        PaintMenuPart(hdc, part, state, rect)) {
        return S_OK;
    }

    return DrawThemeBackground_Original(theme, hdc, part, state, rect, clip);
}

HRESULT WINAPI DrawThemeBackgroundEx_Hook(HTHEME theme, HDC hdc, int part,
                                          int state, const RECT* rect,
                                          const void* options) {
    if (CurrentSettings().menuHook && rect && IsMenuTheme(theme) &&
        PaintMenuPart(hdc, part, state, rect)) {
        return S_OK;
    }

    return DrawThemeBackgroundEx_Original(theme, hdc, part, state, rect, options);
}

/*
    The text color is forced alongside the background: light-variant text on a
    dark background would leave an invisible menu. DTTOPTS is written out here
    because uxtheme.h varies between toolchains, and one field more or less
    would misalign it silently.
*/
struct ThemeDttOpts {
    DWORD dwSize;
    DWORD dwFlags;
    COLORREF crText;
    COLORREF crBorder;
    COLORREF crShadow;
    int iTextShadowType;
    POINT ptShadowOffset;
    int iBorderSize;
    int iFontPropId;
    int iColorPropId;
    int iStateId;
    BOOL fApplyOverlay;
    int iGlowSize;
    void* pfnDrawTextCallback;
    LPARAM lParam;
};

constexpr DWORD kDttTextColor = 0x00000001;

static COLORREF MenuTextColor(int part, int state) {
    const Palette& p = CurrentSettings().palette;

    if (part == kMenuPopupItem) {
        // 3 = disabled, 4 = disabled hot
        return (state == 3 || state == 4) ? p.dimText : p.text;
    }

    if (part == kMenuBarItem) {
        // 4, 5, 6 = the disabled variations
        return (state >= 4) ? p.dimText : p.text;
    }

    return p.text;
}

HRESULT WINAPI DrawThemeText_Hook(HTHEME theme, HDC hdc, int part, int state,
                                  LPCWSTR text, int length, DWORD flags,
                                  DWORD flags2, const RECT* rect) {
    if (CurrentSettings().menuHook && rect && DrawThemeTextEx_Original &&
        IsMenuTheme(theme)) {
        ThemeDttOpts opts{};
        opts.dwSize = sizeof(opts);
        opts.dwFlags = kDttTextColor;
        opts.crText = MenuTextColor(part, state);

        RECT copy = *rect;

        return DrawThemeTextEx_Original(theme, hdc, part, state, text, length,
                                        flags, &copy, &opts);
    }

    return DrawThemeText_Original(theme, hdc, part, state, text, length, flags,
                                  flags2, rect);
}

HRESULT WINAPI DrawThemeTextEx_Hook(HTHEME theme, HDC hdc, int part, int state,
                                    LPCWSTR text, int length, DWORD flags,
                                    RECT* rect, const void* options) {
    if (CurrentSettings().menuHook && IsMenuTheme(theme)) {
        ThemeDttOpts opts{};

        if (options) {
            DWORD callerSize = *reinterpret_cast<const DWORD*>(options);

            /*
                A DTTOPTS larger than this one is forwarded as it came:
                copying only part of it would drop flags the caller set.
            */
            if (callerSize > sizeof(opts)) {
                return DrawThemeTextEx_Original(theme, hdc, part, state, text,
                                                length, flags, rect, options);
            }

            // Keep what the caller asked for and only swap the color.
            memcpy(&opts, options, callerSize);
        }

        opts.dwSize = sizeof(opts);
        opts.dwFlags |= kDttTextColor;
        opts.crText = MenuTextColor(part, state);

        return DrawThemeTextEx_Original(theme, hdc, part, state, text, length,
                                        flags, rect, &opts);
    }

    return DrawThemeTextEx_Original(theme, hdc, part, state, text, length, flags,
                                    rect, options);
}

// ============================================================================
// THE MENU BAR: UAH MESSAGES
// ============================================================================

/*
    The menu bar is non-client area, which user32 draws by sending the window
    two undocumented messages:

        WM_UAHDRAWMENU      (0x91)  the background of the whole strip
        WM_UAHDRAWMENUITEM  (0x92)  each item, one at a time

    They reach DefWindowProc / DefFrameProc, where the default drawing happens,
    so the mod paints into the same DCs and rectangles there. Technique from
    https://github.com/adzm/win32-darkmode (darkmenubar branch), MIT licensed.
*/

#define WM_UAHDRAWMENU 0x0091
#define WM_UAHDRAWMENUITEM 0x0092

union UahMenuItemMetrics {
    struct {
        DWORD cx;
        DWORD cy;
    } rgsizeBar[2];
    struct {
        DWORD cx;
        DWORD cy;
    } rgsizePopup[4];
};

struct UahMenuPopupMetrics {
    DWORD rgcx[4];
    DWORD fUpdateMaxWidths : 2;
};

struct UahMenu {
    HMENU hMenu;
    HDC hdc;
    DWORD dwFlags;
};

struct UahMenuItem {
    int iPosition;
    UahMenuItemMetrics umim;
    UahMenuPopupMetrics umpm;
};

struct UahDrawMenuItem {
    DRAWITEMSTRUCT dis;
    UahMenu um;
    UahMenuItem umi;
};

/*
    The theme for one menu bar item, opened when the item is drawn and closed
    right after.

    A handle kept per thread would be left open on every UI thread when the
    mod unloads, since only that thread's own messages could close it, and it
    would keep the DPI it was opened at after its window moves to another
    monitor. Menu bar items only repaint on hover and non-client paints, so
    opening one per item costs nothing visible.

    The originals are called, so the hooks above do not swap the class again.
*/
static HTHEME OpenMenuBarTheme(HWND hwnd) {
    // The dark variant comes back dark even with Windows in light mode.
    static constexpr LPCWSTR kClasses[] = {L"DarkMode::Menu", L"Menu"};

    auto openForDpi = OpenThemeDataForDpi_Original
                          ? OpenThemeDataForDpi_Original
                          : UxThemeProc<OpenThemeDataForDpi_t>("OpenThemeDataForDpi");
    auto open = OpenThemeData_Original
                    ? OpenThemeData_Original
                    : UxThemeProc<OpenThemeData_t>("OpenThemeData");

    UINT dpi = g_GetDpiForWindow ? g_GetDpiForWindow(hwnd) : 0;

    for (LPCWSTR themeClass : kClasses) {
        HTHEME theme = nullptr;

        if (openForDpi && dpi) {
            theme = openForDpi(hwnd, themeClass, dpi);
        } else if (open) {
            theme = open(hwnd, themeClass);
        }

        if (theme) {
            return theme;
        }
    }

    return nullptr;
}

class MenuBarTheme {
   public:
    explicit MenuBarTheme(HWND hwnd) : m_theme(OpenMenuBarTheme(hwnd)) {}

    ~MenuBarTheme() {
        auto close = CloseThemeData_Original
                         ? CloseThemeData_Original
                         : UxThemeProc<CloseThemeData_t>("CloseThemeData");

        if (m_theme && close) {
            // As CloseThemeData_Hook does: uxtheme hands the same handle value
            // back for another class, and a stale entry would have
            // PaintMenuPart repaint that one in menu colors.
            ForgetMenuTheme(m_theme);
            close(m_theme);
        } else if (m_theme) {
            Wh_Log(L"CloseThemeData could not be resolved; a menu bar theme leaks");
        }
    }

    MenuBarTheme(const MenuBarTheme&) = delete;
    MenuBarTheme& operator=(const MenuBarTheme&) = delete;

    HTHEME get() const { return m_theme; }

   private:
    HTHEME m_theme;
};

static DrawThemeTextEx_t ResolveDrawThemeTextEx() {
    if (DrawThemeTextEx_Original) {
        return DrawThemeTextEx_Original;
    }

    return g_uxtheme ? reinterpret_cast<DrawThemeTextEx_t>(
                           GetProcAddress(g_uxtheme, "DrawThemeTextEx"))
                     : nullptr;
}

/*
    Windows leaves a 1px line just below the bar, in non-client area, that comes
    through none of the messages above — and it stays light, scoring a line across
    the top of the window. The only way to cover it is drawing into the whole-window
    DC.
*/
static void PaintMenuBarBottomLine(HWND hwnd) {
    MENUBARINFO barInfo{};
    barInfo.cbSize = sizeof(barInfo);

    if (!GetMenuBarInfo(hwnd, OBJID_MENU, 0, &barInfo)) {
        return;
    }

    RECT client;
    GetClientRect(hwnd, &client);
    MapWindowPoints(hwnd, nullptr, reinterpret_cast<LPPOINT>(&client), 2);

    RECT window;
    GetWindowRect(hwnd, &window);

    OffsetRect(&client, -window.left, -window.top);

    RECT line = client;
    line.bottom = line.top;
    line.top--;

    /*
        The edge the system leaves between the menu bar and the client area is
        one pixel at 100% and follows SM_CYBORDER upward, so a fixed row leaves
        a light hairline on a scaled display. GetMenuBarInfo already gave the
        bar's own rectangle, so the gap is measured rather than computed from
        a DPI — right at every scale, and no second system metric to agree
        with.

        Only when it is a plausible edge. A bar rectangle that overlaps the
        client area, or is further off than any border would be, leaves the
        single row this always painted.
    */
    RECT bar = barInfo.rcBar;
    OffsetRect(&bar, -window.left, -window.top);

    constexpr LONG kMaxEdge = 8;

    if (bar.bottom < client.top && client.top - bar.bottom <= kMaxEdge) {
        line.top = bar.bottom;
    }

    HDC hdc = GetWindowDC(hwnd);

    if (hdc) {
        FillWith(hdc, &line, CurrentSettings().palette.ramp[4]);
        ReleaseDC(hwnd, hdc);
    }
}

static bool PaintMenuBarBackground(HWND hwnd, LPARAM lParam) {
    const auto* info = reinterpret_cast<const UahMenu*>(lParam);

    if (!info || !info->hdc) {
        return false;
    }

    MENUBARINFO barInfo{};
    barInfo.cbSize = sizeof(barInfo);

    if (!GetMenuBarInfo(hwnd, OBJID_MENU, 0, &barInfo)) {
        return false;
    }

    RECT window;

    if (!GetWindowRect(hwnd, &window)) {
        return false;
    }

    // rcBar is in screen coordinates; the HDC is window-relative.
    RECT bar = barInfo.rcBar;

    if (!OffsetRect(&bar, -window.left, -window.top)) {
        return false;
    }

    FillWith(info->hdc, &bar, CurrentSettings().palette.ramp[1]);

    return true;
}

volatile LONG g_menuBarTextFailedLogged = FALSE;

static bool PaintMenuBarItem(HWND hwnd, LPARAM lParam) {
    auto* draw = reinterpret_cast<UahDrawMenuItem*>(lParam);

    if (!draw || !draw->um.hdc) {
        return false;
    }

    wchar_t label[256]{};

    MENUITEMINFOW itemInfo{};
    itemInfo.cbSize = sizeof(itemInfo);
    itemInfo.fMask = MIIM_STRING;
    itemInfo.dwTypeData = label;
    itemInfo.cch = ARRAYSIZE(label) - 1;

    if (!GetMenuItemInfoW(draw->um.hMenu, draw->umi.iPosition, TRUE, &itemInfo)) {
        return false;
    }

    label[ARRAYSIZE(label) - 1] = L'\0';

    bool hasText = label[0] != L'\0';

    /*
        Everything that can fail is resolved before anything is painted.
        Filling first and failing after would hand the item back to
        DefWindowProc, which then paints it again on top of the fill — which
        is also why, once the fill is down, this returns true whatever
        DrawThemeTextEx says.
    */
    DrawThemeTextEx_t drawText = nullptr;
    std::optional<MenuBarTheme> theme;

    if (hasText) {
        drawText = ResolveDrawThemeTextEx();
        theme.emplace(hwnd);

        if (!drawText || !theme->get()) {
            return false;
        }
    }

    const Palette& p = CurrentSettings().palette;

    UINT state = draw->dis.itemState;

    COLORREF background = p.ramp[1];

    if (state & ODS_HOTLIGHT) {
        background = p.accent;
    } else if (state & ODS_SELECTED) {
        background = p.ramp[4];
    }

    FillWith(draw->um.hdc, &draw->dis.rcItem, background);

    /*
        An item with no text is an MDI window caption button, drawn with an icon
        font glyph. Returning true here skips DefWindowProc, so that glyph is not
        drawn at all — only the background is. After Effects has no MDI child
        windows, so there is no such button to lose; a host that had one would
        need the glyph drawn here.
    */
    if (!hasText) {
        return true;
    }

    // Only a disabled item is dimmed; Windows does not dim an inactive bar.
    bool disabled = (state & (ODS_GRAYED | ODS_DISABLED)) != 0;

    DWORD flags = DT_CENTER | DT_SINGLELINE | DT_VCENTER;

    if (state & ODS_NOACCEL) {
        flags |= DT_HIDEPREFIX;
    }

    ThemeDttOpts opts{};
    opts.dwSize = sizeof(opts);
    opts.dwFlags = kDttTextColor;
    opts.crText = disabled ? p.dimText : p.text;

    // A copy: the API takes an LPRECT, and this one belongs to user32's message.
    RECT rect = draw->dis.rcItem;

    HRESULT hr = drawText(theme->get(), draw->um.hdc, kMenuBarItem, 1, label,
                          static_cast<int>(itemInfo.cch), flags, &rect, &opts);

    /*
        The background is painted; only the label is missing. Handing the item
        back to DefWindowProc now would redraw it in the Windows gray, so this
        keeps the item and logs once — the bar repaints constantly.
    */
    if (FAILED(hr) && Claim(&g_menuBarTextFailedLogged)) {
        Wh_Log(L"a menu bar item's text could not be drawn (0x%08X); its "
               L"background is still painted",
               static_cast<unsigned>(hr));
    }

    return true;
}

/*
    Returns true when the message was handled completely and the original
    DefWindowProc must not run.
*/
static bool HandleMenuBarMessage(HWND hwnd, UINT msg, LPARAM lParam,
                                 LRESULT* result) {
    switch (msg) {
        case WM_UAHDRAWMENU:
            if (PaintMenuBarBackground(hwnd, lParam)) {
                *result = 1;
                return true;
            }
            return false;

        case WM_UAHDRAWMENUITEM:
            if (PaintMenuBarItem(hwnd, lParam)) {
                *result = 1;
                return true;
            }
            return false;

        default:
            return false;
    }
}

/*
    DefWindowProc runs for almost every message in the process, so the message
    id, a jump table, is tested before anything reaches into the window.
*/
static bool IsMenuBarMessage(UINT msg) {
    switch (msg) {
        case WM_UAHDRAWMENU:
        case WM_UAHDRAWMENUITEM:
        case WM_NCPAINT:
        case WM_ACTIVATE:
            return true;
        default:
            return false;
    }
}

/*
    WS_CHILD is tested before GetMenu, whose answer is undefined for a child
    window: in practice it is the control ID, so a child with a nonzero ID
    would pass.
*/
static bool NeedsMenuBarWork(HWND hwnd, UINT msg) {
    return CurrentSettings().menuHook && IsMenuBarMessage(msg) && hwnd &&
           !(GetWindowLongPtrW(hwnd, GWL_STYLE) & WS_CHILD) &&
           GetMenu(hwnd) != nullptr;
}

using DefWindowProcW_t = LRESULT(WINAPI*)(HWND, UINT, WPARAM, LPARAM);
using DefFrameProcW_t = LRESULT(WINAPI*)(HWND, HWND, UINT, WPARAM, LPARAM);

DefWindowProcW_t DefWindowProcW_Original = nullptr;
DefWindowProcW_t DefWindowProcA_Original = nullptr;
DefFrameProcW_t DefFrameProcW_Original = nullptr;
DefFrameProcW_t DefFrameProcA_Original = nullptr;

LRESULT WINAPI DefWindowProcW_Hook(HWND hwnd, UINT msg, WPARAM wParam,
                                   LPARAM lParam) {
    if (!NeedsMenuBarWork(hwnd, msg)) {
        return DefWindowProcW_Original(hwnd, msg, wParam, lParam);
    }

    LRESULT result = 0;

    if (HandleMenuBarMessage(hwnd, msg, lParam, &result)) {
        return result;
    }

    result = DefWindowProcW_Original(hwnd, msg, wParam, lParam);

    if (msg == WM_NCPAINT || msg == WM_ACTIVATE) {
        PaintMenuBarBottomLine(hwnd);
    }

    return result;
}

LRESULT WINAPI DefWindowProcA_Hook(HWND hwnd, UINT msg, WPARAM wParam,
                                   LPARAM lParam) {
    if (!NeedsMenuBarWork(hwnd, msg)) {
        return DefWindowProcA_Original(hwnd, msg, wParam, lParam);
    }

    LRESULT result = 0;

    if (HandleMenuBarMessage(hwnd, msg, lParam, &result)) {
        return result;
    }

    result = DefWindowProcA_Original(hwnd, msg, wParam, lParam);

    if (msg == WM_NCPAINT || msg == WM_ACTIVATE) {
        PaintMenuBarBottomLine(hwnd);
    }

    return result;
}

LRESULT WINAPI DefFrameProcW_Hook(HWND hwnd, HWND mdiClient, UINT msg,
                                  WPARAM wParam, LPARAM lParam) {
    if (!NeedsMenuBarWork(hwnd, msg)) {
        return DefFrameProcW_Original(hwnd, mdiClient, msg, wParam, lParam);
    }

    LRESULT result = 0;

    if (HandleMenuBarMessage(hwnd, msg, lParam, &result)) {
        return result;
    }

    result = DefFrameProcW_Original(hwnd, mdiClient, msg, wParam, lParam);

    if (msg == WM_NCPAINT || msg == WM_ACTIVATE) {
        PaintMenuBarBottomLine(hwnd);
    }

    return result;
}

LRESULT WINAPI DefFrameProcA_Hook(HWND hwnd, HWND mdiClient, UINT msg,
                                  WPARAM wParam, LPARAM lParam) {
    if (!NeedsMenuBarWork(hwnd, msg)) {
        return DefFrameProcA_Original(hwnd, mdiClient, msg, wParam, lParam);
    }

    LRESULT result = 0;

    if (HandleMenuBarMessage(hwnd, msg, lParam, &result)) {
        return result;
    }

    result = DefFrameProcA_Original(hwnd, mdiClient, msg, wParam, lParam);

    if (msg == WM_NCPAINT || msg == WM_ACTIVATE) {
        PaintMenuBarBottomLine(hwnd);
    }

    return result;
}

// ============================================================================
// GDI SURFACES
// ============================================================================

static DvaColorRGBA GdiToDva(COLORREF color) {
    return {GetRValue(color) / 255.0f, GetGValue(color) / 255.0f,
            GetBValue(color) / 255.0f, 1.0f};
}

static COLORREF DvaToGdi(const DvaColorRGBA& color) {
    auto channel = [](float v) {
        return ClampInt(static_cast<int>(v * 255.0f + 0.5f), 0, 255);
    };

    return RGB(channel(color.r), channel(color.g), channel(color.b));
}

/*
    The same conversion and the same guard as for dvaui's colors. Adobe code
    often turns a color that already came out of a theme function into a
    COLORREF before it builds a brush or pen from it, and converting it again
    would push it toward the darkest stop. The recent produced colors are keyed
    on 8-bit channels, so a COLORREF compares equal to the float color it was
    made from.
*/
static COLORREF ConvertGdiColor(const Settings& s, COLORREF color) {
    DvaColorRGBA in = GdiToDva(color);
    DvaColorRGBA out{};

    if (IsProducedColor(in) || !ConvertDvaColorWith(s, in, &out)) {
        return color;
    }

    RememberProduced(out);

    return DvaToGdi(out);
}

/*
    Cheapest test first: the color test turns most calls away on its own, so
    the module ranges are only walked for the dark grays that may be
    converted. The caller's address is read in the hook and passed in; read in
    here, it would be the hook's own address unless this was inlined. The
    settings snapshot is the hook's, and the same one the conversion gets.
*/
static bool ShouldConvertGdi(const Settings& s, COLORREF color, void* caller) {
    return s.gdiHook && ShouldConvertWith(s, GdiToDva(color)) &&
           !InContentScope() && IsAdobeUICaller(caller);
}

using CreateSolidBrush_t = HBRUSH(WINAPI*)(COLORREF);
using CreatePen_t = HPEN(WINAPI*)(int, int, COLORREF);
using SetBkColor_t = COLORREF(WINAPI*)(HDC, COLORREF);

CreateSolidBrush_t CreateSolidBrush_Original = nullptr;
CreatePen_t CreatePen_Original = nullptr;
SetBkColor_t SetBkColor_Original = nullptr;

HBRUSH WINAPI CreateSolidBrush_Hook(COLORREF color) {
    const Settings& s = CurrentSettings();

    if (ShouldConvertGdi(s, color, __builtin_return_address(0))) {
        color = ConvertGdiColor(s, color);
    }

    return CreateSolidBrush_Original(color);
}

HPEN WINAPI CreatePen_Hook(int style, int width, COLORREF color) {
    const Settings& s = CurrentSettings();

    if (ShouldConvertGdi(s, color, __builtin_return_address(0))) {
        color = ConvertGdiColor(s, color);
    }

    return CreatePen_Original(style, width, color);
}

COLORREF WINAPI SetBkColor_Hook(HDC hdc, COLORREF color) {
    const Settings& s = CurrentSettings();

    if (ShouldConvertGdi(s, color, __builtin_return_address(0))) {
        color = ConvertGdiColor(s, color);
    }

    return SetBkColor_Original(hdc, color);
}

// ============================================================================
// SETTINGS
// ============================================================================

/*
    "#RRGGBB", and "#RRGGBBAA" for the color pickers that write one.

    Eight digits are read as a color with an alpha channel and the alpha is
    dropped. Every surface a palette describes is opaque, so there is nothing
    for it to mean here — and refusing it is worse than ignoring it: the field
    falls back to Onyx's color with only a line in the log to say why, which
    is what a color picked rather than typed used to do.

    The leading # and any spaces are skipped, and the case is free.
*/
static bool ParseHexColor(PCWSTR text, COLORREF* out) {
    while (*text == L'#' || *text == L' ') {
        text++;
    }

    unsigned value = 0;
    int digits = 0;

    for (; *text; text++) {
        wchar_t c = *text;
        unsigned nibble;

        if (c >= L'0' && c <= L'9') {
            nibble = c - L'0';
        } else if (c >= L'a' && c <= L'f') {
            nibble = c - L'a' + 10;
        } else if (c >= L'A' && c <= L'F') {
            nibble = c - L'A' + 10;
        } else if (c == L' ') {
            continue;
        } else {
            return false;
        }

        value = (value << 4) | nibble;
        digits++;
    }

    if (digits != 6 && digits != 8) {
        return false;
    }

    // RRGGBBAA: the alpha is the low byte, and it is not this palette's to keep.
    if (digits == 8) {
        value >>= 8;
    }

    // The text is RRGGBB; COLORREF is 0x00BBGGRR.
    *out = RGB((value >> 16) & 0xFF, (value >> 8) & 0xFF, value & 0xFF);

    return true;
}

struct NamedPalette {
    const wchar_t* id;
    Palette colors;
};

/*
    After Effects is sampled from the app's icon, which lives on hue 240; Neon and
    Glitch from reference artwork, with Neon's magenta accent lowered so it
    highlights a menu item instead of drowning its text. Glitch's acid green
    would be too bright as background, so it tints the black and tops the ramp.
    Comfy is designed for long sessions and stays away from black.

    Violet and Blossom put the hue in the ramp; Amethyst and Crimson hold it
    out, on near-black panels. Blossom is pastel only in its text and border:
    the interface's own text passes above the ceiling, so a pastel background would
    be light on light. Every accent is at least 4.5:1 against its own text,
    which is what darkened Blossom's, Ember's, Amethyst's and Miku's: read at
    full strength they were a hovered menu item nobody could read.

    Contrast through Miku also carry a highlight, the hue the interface blue
    takes; Threshold is threshold-editor.com.br's #050505 / #FFFFFF /
    #DC2626.
*/
static const NamedPalette kPalettes[] = {
    {L"onyx",
     {{RGB(0x05, 0x05, 0x05), RGB(0x09, 0x09, 0x09), RGB(0x0E, 0x0E, 0x0E),
       RGB(0x16, 0x16, 0x16), RGB(0x24, 0x24, 0x24)},
      RGB(0xE6, 0xE6, 0xE6),
      RGB(0x77, 0x77, 0x77),
      RGB(0x2E, 0x2E, 0x2E)}},

    {L"abyss",
     {{RGB(0x00, 0x00, 0x00), RGB(0x04, 0x04, 0x04), RGB(0x08, 0x08, 0x08),
       RGB(0x10, 0x10, 0x10), RGB(0x1C, 0x1C, 0x1C)},
      RGB(0xE6, 0xE6, 0xE6),
      RGB(0x6E, 0x6E, 0x6E),
      RGB(0x24, 0x24, 0x24)}},

    {L"graphite",
     {{RGB(0x0D, 0x0D, 0x0D), RGB(0x14, 0x14, 0x14), RGB(0x1A, 0x1A, 0x1A),
       RGB(0x23, 0x23, 0x23), RGB(0x30, 0x30, 0x30)},
      RGB(0xE8, 0xE8, 0xE8),
      RGB(0x80, 0x80, 0x80),
      RGB(0x3A, 0x3A, 0x3A)}},

    {L"aftereffects",
     {{RGB(0x06, 0x06, 0x0F), RGB(0x0C, 0x0C, 0x20), RGB(0x13, 0x13, 0x33),
       RGB(0x1E, 0x1E, 0x4D), RGB(0x33, 0x33, 0x7A)},
      RGB(0xD2, 0xD2, 0xF7),
      RGB(0x71, 0x71, 0xA8),
      RGB(0x3A, 0x3A, 0x99)}},

    {L"comfy",
     {{RGB(0x10, 0x0C, 0x09), RGB(0x18, 0x13, 0x10), RGB(0x21, 0x1A, 0x15),
       RGB(0x2E, 0x24, 0x1C), RGB(0x45, 0x35, 0x28)},
      RGB(0xED, 0xE0, 0xD0),
      RGB(0x8C, 0x7A, 0x66),
      RGB(0x5A, 0x40, 0x28)}},

    {L"neon",
     {{RGB(0x08, 0x05, 0x08), RGB(0x0E, 0x0A, 0x0D), RGB(0x1A, 0x0F, 0x18),
       RGB(0x2A, 0x15, 0x26), RGB(0x4D, 0x21, 0x3F)},
      RGB(0xFA, 0xF5, 0xEF),
      RGB(0x8A, 0x60, 0x79),
      RGB(0x5C, 0x1F, 0x47)}},

    {L"glitch",
     {{RGB(0x06, 0x0A, 0x02), RGB(0x0C, 0x13, 0x05), RGB(0x13, 0x1D, 0x08),
       RGB(0x1E, 0x2C, 0x0C), RGB(0x3C, 0x54, 0x10)},
      RGB(0xE9, 0xF7, 0xC0),
      RGB(0x8F, 0xA4, 0x63),
      RGB(0xB8, 0x15, 0x7A)}},

    {L"contrast",
     {{RGB(0x00, 0x00, 0x00), RGB(0x06, 0x06, 0x06), RGB(0x10, 0x10, 0x10),
       RGB(0x20, 0x20, 0x20), RGB(0x4A, 0x4A, 0x4A)},
      RGB(0xFF, 0xFF, 0xFF),
      RGB(0x9A, 0x9A, 0x9A),
      RGB(0x5E, 0x5E, 0x5E),
      RGB(0x80, 0x80, 0x80)}},

    {L"violet",
     {{RGB(0x0A, 0x04, 0x14), RGB(0x12, 0x08, 0x27), RGB(0x1C, 0x0C, 0x3B),
       RGB(0x2A, 0x12, 0x57), RGB(0x4B, 0x20, 0x88)},
      RGB(0xE2, 0xD4, 0xFA),
      RGB(0x8E, 0x7A, 0xB4),
      RGB(0x6D, 0x28, 0xD9),
      RGB(0x6D, 0x28, 0xD9)}},

    {L"blossom",
     {{RGB(0x12, 0x0A, 0x0E), RGB(0x18, 0x0D, 0x13), RGB(0x24, 0x13, 0x20),
       RGB(0x34, 0x1B, 0x2E), RGB(0x5A, 0x2C, 0x4A)},
      RGB(0xFB, 0xDC, 0xE8),
      RGB(0xA7, 0x80, 0x8F),
      RGB(0x86, 0x41, 0x60),
      RGB(0xD9, 0x6A, 0x9B)}},

    {L"ember",
     {{RGB(0x0C, 0x07, 0x03), RGB(0x14, 0x0B, 0x04), RGB(0x1F, 0x12, 0x06),
       RGB(0x2E, 0x1A, 0x08), RGB(0x55, 0x30, 0x0C)},
      RGB(0xFB, 0xE3, 0xC8),
      RGB(0x94, 0x76, 0x5A),
      RGB(0x94, 0x41, 0x08),
      RGB(0xC2, 0x56, 0x0A)}},

    {L"amethyst",
     {{RGB(0x05, 0x05, 0x07), RGB(0x09, 0x09, 0x0D), RGB(0x0F, 0x0F, 0x16),
       RGB(0x18, 0x18, 0x23), RGB(0x33, 0x25, 0x4F)},
      RGB(0xE4, 0xDC, 0xF2),
      RGB(0x7E, 0x76, 0x91),
      RGB(0x67, 0x30, 0xC6),
      RGB(0x7C, 0x3A, 0xED)}},

    {L"crimson",
     {{RGB(0x0D, 0x04, 0x05), RGB(0x15, 0x06, 0x09), RGB(0x20, 0x0A, 0x0E),
       RGB(0x30, 0x0F, 0x15), RGB(0x59, 0x1A, 0x24)},
      RGB(0xF7, 0xD9, 0xDD),
      RGB(0x96, 0x67, 0x6D),
      RGB(0xB9, 0x1C, 0x1C),
      RGB(0xB9, 0x1C, 0x1C)}},

    {L"threshold",
     {{RGB(0x05, 0x05, 0x05), RGB(0x0A, 0x0A, 0x0A), RGB(0x12, 0x12, 0x12),
       RGB(0x1C, 0x1C, 0x1C), RGB(0x2B, 0x2B, 0x2B)},
      RGB(0xFF, 0xFF, 0xFF),
      RGB(0x80, 0x80, 0x80),
      RGB(0xDC, 0x26, 0x26),
      RGB(0xDC, 0x26, 0x26)}},

    {L"miku",
     {{RGB(0x06, 0x10, 0x14), RGB(0x0A, 0x19, 0x20), RGB(0x10, 0x26, 0x2D),
       RGB(0x17, 0x37, 0x40), RGB(0x28, 0x51, 0x5C)},
      RGB(0xDD, 0xFB, 0xFF),
      RGB(0x71, 0x8B, 0x91),
      RGB(0x0E, 0x74, 0x78),
      RGB(0x63, 0xF3, 0xEE)}},
};

/*
    A custom theme is nine settings in one group, so each color gets its own
    labelled field in Windhawk's settings and only ParseHexColor is needed to
    validate it. Sharing a theme is Windhawk's own job: the Advanced tab
    exports and imports a mod's whole settings as text.

    present says the field had something in it, which is what tells an empty
    field — the default for the three optional ones — from a typo.
*/
static bool ReadThemeColor(PCWSTR setting, COLORREF* color, bool* present) {
    auto value = WindhawkUtils::StringSetting::make(setting);
    PCWSTR text = value;

    *present = text && *text;

    return *present && ParseHexColor(text, color);
}

/*
    "#RRGGBB" into out, or the empty string for a color the theme leaves to its
    default. Written by hand rather than with a formatter, for the same reason
    the CSS rewrite writes its own digits: this runs where a locale-aware
    printf has no business being.
*/
static void FormatHexColor(COLORREF color, wchar_t out[8]) {
    if (color == CLR_INVALID) {
        out[0] = L'\0';
        return;
    }

    constexpr wchar_t kDigits[] = L"0123456789ABCDEF";
    const BYTE channels[3] = {GetRValue(color), GetGValue(color),
                              GetBValue(color)};

    out[0] = L'#';

    for (int i = 0; i < 3; i++) {
        out[1 + i * 2] = kDigits[channels[i] >> 4];
        out[2 + i * 2] = kDigits[channels[i] & 0x0F];
    }

    out[7] = L'\0';
}

/*
    The theme in force, written out as the settings text Windhawk itself reads,
    so it can be handed to someone else.

    The mod's Settings tab has a text mode beside the visual one, and what it
    holds is YAML: the palette, then customTheme as a block under it. This
    writes the same thing in YAML's flow form, which is one line and parses
    identically — so a theme is a line to copy out of the log and paste into
    that box.

    Only the theme goes out. The document in that box also carries the layer
    switches, and saving it replaces every setting, so a whole document shared
    as-is would hand the sharer's switches to whoever pasted it.
*/
static void LogThemeForSharing(const Palette& p) {
    wchar_t base[8], panel[8], surface[8], raised[8], border[8], text[8];
    wchar_t accent[8], disabledText[8], highlight[8];

    FormatHexColor(p.ramp[0], base);
    FormatHexColor(p.ramp[1], panel);
    FormatHexColor(p.ramp[2], surface);
    FormatHexColor(p.ramp[3], raised);
    FormatHexColor(p.ramp[4], border);
    FormatHexColor(p.text, text);
    FormatHexColor(p.accent, accent);
    FormatHexColor(p.dimText, disabledText);
    FormatHexColor(p.highlight, highlight);

    Wh_Log(L"custom theme, to share — paste this into the text mode of the "
           L"Settings tab: {palette: custom, customTheme: {base: '%s', "
           L"panel: '%s', surface: '%s', raised: '%s', border: '%s', "
           L"text: '%s', accent: '%s', disabledText: '%s', "
           L"highlight: '%s'}}",
           base, panel, surface, raised, border, text, accent, disabledText,
           highlight);
}

/*
    The palette a custom theme describes. The six colors every surface needs
    fall back to Onyx's one at a time, each logged; the other three have
    defaults of their own.
*/
static Palette LoadCustomTheme(const Palette& onyx) {
    Palette p = onyx;

    const struct {
        PCWSTR setting;
        PCWSTR name;
        COLORREF* color;
    } kRequired[] = {
        {L"customTheme.base", L"Base", &p.ramp[0]},
        {L"customTheme.panel", L"Panel", &p.ramp[1]},
        {L"customTheme.surface", L"Surface", &p.ramp[2]},
        {L"customTheme.raised", L"Raised", &p.ramp[3]},
        {L"customTheme.border", L"Border", &p.ramp[4]},
        {L"customTheme.text", L"Text", &p.text},
    };

    for (const auto& field : kRequired) {
        bool present = false;

        if (!ReadThemeColor(field.setting, field.color, &present)) {
            Wh_Log(L"custom theme: %s is %s; Onyx's is used", field.name,
                   present ? L"not a hex color" : L"empty");
        }
    }

    /*
        Left empty takes the default without a word — highlight ships that way
        — while a value that is there but unreadable is worth a line.
    */
    auto optional = [](PCWSTR setting, PCWSTR name, COLORREF* color) {
        bool present = false;

        if (ReadThemeColor(setting, color, &present)) {
            return true;
        }

        if (present) {
            Wh_Log(L"custom theme: %s is not a hex color; left out", name);
        }

        return false;
    };

    if (!optional(L"customTheme.accent", L"Accent", &p.accent)) {
        p.accent = p.ramp[4];
    }

    /*
        Halfway from the text to the panel is close to where the built-in
        palettes put disabled text, and it always lies between the two, which
        Onyx's #777777 would not once a theme's text is darker than that.
    */
    if (!optional(L"customTheme.disabledText", L"Disabled text", &p.dimText)) {
        p.dimText = RGB((GetRValue(p.text) + GetRValue(p.ramp[1])) / 2,
                        (GetGValue(p.text) + GetGValue(p.ramp[1])) / 2,
                        (GetBValue(p.text) + GetBValue(p.ramp[1])) / 2);
    }

    if (!optional(L"customTheme.highlight", L"Highlight", &p.highlight)) {
        p.highlight = CLR_INVALID;
    }

    LogThemeForSharing(p);

    return p;
}

static void LoadSettings() {
    auto name = WindhawkUtils::StringSetting::make(L"palette");

    // Onyx is the default: first in the table and also the fallback.
    Palette p = kPalettes[0].colors;

    if (wcscmp(name, L"custom") == 0) {
        p = LoadCustomTheme(p);
    } else {
        for (const NamedPalette& candidate : kPalettes) {
            if (wcscmp(name, candidate.id) == 0) {
                p = candidate.colors;
                break;
            }
        }
    }

    // Built in a local, then published whole; see g_currentSettings.
    Settings next{};

    next.palette = p;

    next.strength = ClampFloat(Wh_GetIntSetting(L"strength") / 100.0f, 0.0f, 1.0f);

    next.ceiling = ClampFloat(Wh_GetIntSetting(L"ceiling") / 100.0f, 0.05f, 0.95f);

    next.dvauiHook = Wh_GetIntSetting(L"dvauiHook") != 0;
    next.brushHook = Wh_GetIntSetting(L"brushHook") != 0;
    next.nativeDarkMode = Wh_GetIntSetting(L"nativeDarkMode") != 0;
    next.menuHook = Wh_GetIntSetting(L"menuHook") != 0;
    next.gdiHook = Wh_GetIntSetting(L"gdiHook") != 0;
    next.uxpPanels = Wh_GetIntSetting(L"uxpPanels") != 0;
    next.highlight =
        p.highlight != CLR_INVALID && Wh_GetIntSetting(L"highlight") != 0;

    if (next.highlight) {
        for (size_t i = 0; i < kInterfaceBlueCount; i++) {
            next.highlightShades[i] =
                ShadeWithLuminance(p.highlight, Luminance(kInterfaceBlues[i]));
        }
    }

    Settings& copy = g_settingsCopies[g_nextSettingsCopy];
    copy = next;
    g_currentSettings.store(&copy, std::memory_order_release);
    g_nextSettingsCopy = (g_nextSettingsCopy + 1) % kSettingsCopies;
}

// ============================================================================
// LIFECYCLE
// ============================================================================

/*
    Typed through WindhawkUtils::SetFunctionHook, so a hook whose signature
    drifts from its target is a compile error rather than a crash. The dvaui
    tables stay on the raw form: they are type-erased on
    purpose, one template or one loop covering many signatures.
*/
template <typename Prototype>
static void HookOrLog(Prototype* target, Prototype* hook, Prototype** original,
                      const wchar_t* label) {
    if (!target) {
        Wh_Log(L"target absent: %s", label);
        return;
    }

    if (!WindhawkUtils::SetFunctionHook(target, hook, original)) {
        Wh_Log(L"failed to hook %s", label);
    }
}

BOOL Wh_ModInit() {
    LoadSettings();

    if (!AllocateSlots()) {
        Wh_Log(L"could not allocate the color table; the interface layer will "
               L"pass colors through unchanged");
    }

    WatchModuleLoads();
    SnapshotAdobeModules();

    InitNativeDarkMode();

    /*
        The Windows hooks below are installed whatever the settings say, and
        each one checks its own setting every time it runs — so one whose
        setting is off only forwards the call it received. Installing them all
        is what lets Wh_ModSettingsChanged apply a change without reloading the
        mod.
        dvaui is the exception; see HookLoadedModules.
    */
    HookOrLog(CreateWindowExW, CreateWindowExW_Hook, &CreateWindowExW_Original,
              L"CreateWindowExW");
    HookOrLog(CreateWindowExA, CreateWindowExA_Hook, &CreateWindowExA_Original,
              L"CreateWindowExA");

    HookOrLog(GetSysColor, GetSysColor_Hook, &GetSysColor_Original, L"GetSysColor");
    HookOrLog(GetSysColorBrush, GetSysColorBrush_Hook, &GetSysColorBrush_Original,
              L"GetSysColorBrush");
    HookOrLog(FillRect, FillRect_Hook, &FillRect_Original, L"FillRect");

    HookOrLog(UxThemeProc<OpenThemeData_t>("OpenThemeData"), OpenThemeData_Hook,
              &OpenThemeData_Original, L"OpenThemeData");
    HookOrLog(UxThemeProc<OpenThemeDataForDpi_t>("OpenThemeDataForDpi"),
              OpenThemeDataForDpi_Hook, &OpenThemeDataForDpi_Original,
              L"OpenThemeDataForDpi");
    HookOrLog(UxThemeProc<OpenThemeDataEx_t>("OpenThemeDataEx"), OpenThemeDataEx_Hook,
              &OpenThemeDataEx_Original, L"OpenThemeDataEx");

    // Ordinal 49: this is where the menu bar picks up its theme.
    HookOrLog(UxThemeOrdinal<OpenNcThemeData_t>(49), OpenNcThemeData_Hook,
              &OpenNcThemeData_Original, L"OpenNcThemeData");

    HookOrLog(UxThemeProc<CloseThemeData_t>("CloseThemeData"), CloseThemeData_Hook,
              &CloseThemeData_Original, L"CloseThemeData");
    HookOrLog(UxThemeProc<DrawThemeBackground_t>("DrawThemeBackground"),
              DrawThemeBackground_Hook, &DrawThemeBackground_Original,
              L"DrawThemeBackground");
    HookOrLog(UxThemeProc<DrawThemeBackgroundEx_t>("DrawThemeBackgroundEx"),
              DrawThemeBackgroundEx_Hook, &DrawThemeBackgroundEx_Original,
              L"DrawThemeBackgroundEx");
    HookOrLog(UxThemeProc<DrawThemeText_t>("DrawThemeText"), DrawThemeText_Hook,
              &DrawThemeText_Original, L"DrawThemeText");
    HookOrLog(UxThemeProc<DrawThemeTextEx_t>("DrawThemeTextEx"),
              DrawThemeTextEx_Hook, &DrawThemeTextEx_Original, L"DrawThemeTextEx");

    // Only to notice windows someone else re-themes; see ForgetThemedClass.
    HookOrLog(UxThemeProc<SetWindowTheme_t>("SetWindowTheme"), SetWindowTheme_Hook,
              &SetWindowTheme_Original, L"SetWindowTheme");

    /*
        The menu bar is drawn by DefWindowProc / DefFrameProc in response to
        WM_UAHDRAWMENU and WM_UAHDRAWMENUITEM. All four variants are hooked because
        there is no way to know which one After Effects uses — and hooking the one it does
        not use costs nothing.
    */
    HookOrLog(DefWindowProcW, DefWindowProcW_Hook, &DefWindowProcW_Original,
              L"DefWindowProcW");
    HookOrLog(DefWindowProcA, DefWindowProcA_Hook, &DefWindowProcA_Original,
              L"DefWindowProcA");
    HookOrLog(DefFrameProcW, DefFrameProcW_Hook, &DefFrameProcW_Original,
              L"DefFrameProcW");
    HookOrLog(DefFrameProcA, DefFrameProcA_Hook, &DefFrameProcA_Original,
              L"DefFrameProcA");

    HookOrLog(CreateSolidBrush, CreateSolidBrush_Hook, &CreateSolidBrush_Original,
              L"CreateSolidBrush");
    HookOrLog(CreatePen, CreatePen_Hook, &CreatePen_Original, L"CreatePen");
    HookOrLog(SetBkColor, SetBkColor_Hook, &SetBkColor_Original, L"SetBkColor");

    // kernelbase, not kernel32; see LoadLibraryExW_Hook.
    HMODULE kernelBase = GetModuleHandleW(L"kernelbase.dll");

    auto loadLibraryExW =
        kernelBase ? reinterpret_cast<LoadLibraryExW_t>(
                         GetProcAddress(kernelBase, "LoadLibraryExW"))
                   : nullptr;

    if (loadLibraryExW) {
        HookOrLog(loadLibraryExW, LoadLibraryExW_Hook, &LoadLibraryExW_Original,
                  L"kernelbase!LoadLibraryExW");
    } else {
        Wh_Log(L"could not resolve kernelbase!LoadLibraryExW; an After Effects "
               L"that loads dvaui after this point will not be themed");
    }

    /*
        The UXP runtime reads the panels' stylesheets and scripts through
        these two, and
        they are the only hooks the mod puts on a path every file open in the
        process takes. So unlike the rest, they go in only when "UXP panels"
        is on as the mod loads — turning it on later needs a restart,
        which that layer needs anyway, since a panel reads its stylesheet once.
    */
    if (kernelBase && CurrentSettings().uxpPanels) {
        FindUxpPluginsDir();

        HookOrLog(reinterpret_cast<CreateFileW_t>(
                      GetProcAddress(kernelBase, "CreateFileW")),
                  CreateFileW_Hook, &CreateFileW_Original, L"kernelbase!CreateFileW");
        HookOrLog(reinterpret_cast<CreateFile2_t>(
                      GetProcAddress(kernelBase, "CreateFile2")),
                  CreateFile2_Hook, &CreateFile2_Original, L"kernelbase!CreateFile2");
    }

    // Windhawk applies every operation registered here once this returns.
    HookLoadedModules();

    return TRUE;
}

/*
    Pushing the theme to windows that already exist belongs here, not at the end
    of Wh_ModInit.

    Windhawk installs the mod's hooks when Wh_ModInit returns. ApplyToTopLevel
    calls DrawMenuBar, which schedules a WM_NCPAINT that the UI thread is
    free to process before that happens — the bar would paint light, and nothing
    would ask it to paint again.
*/
void Wh_ModAfterInit() {
    /*
        Once more for a module that arrived between Wh_ModInit returning and
        the LoadLibraryExW hook going live.

        A module mapped as a static dependency never passes through
        LoadLibraryExW at all — the loader goes straight to its own path — so
        a dvaui that arrives that way in the gap would leave the interface
        layer off for the session. After Effects calls LoadLibraryExW hundreds
        of times while it starts, so in practice the next one catches it, but
        the race closes here for nothing.
    */
    bool registered = HookLoadedModules(nullptr, true);

    if (registered && !Wh_ApplyHookOperations()) {
        Wh_Log(L"failed to apply hooks for a module loaded during init");
    }

    ApplyThemeToExistingWindows();
}

void Wh_ModUninit() {
    /*
        Hooks are already removed by the time this runs, so the calls below go
        straight to the system and the windows come back with their own colors.
    */

    // The notification callback lives in this image, which is about to go.
    StopWatchingModuleLoads();

    // After Effects' references into the table get their original colors back.
    // The counts are logged to judge the table size against real sessions,
    // and to tell a highlight that reached the screen from one never asked for.
    size_t used = RecomputeColorTable(true);

    Wh_Log(L"color table: %u of %u slots in use; %d of %u interface blues "
           L"recolored",
           static_cast<unsigned>(used), static_cast<unsigned>(kSlotCount),
           std::popcount(static_cast<uint64_t>(g_bluesRecolored)),
           static_cast<unsigned>(kInterfaceBlueCount));

    /*
        A whole session with nothing hooked, and a UI that did come up: every
        module loaded by then was asked in Wh_ModAfterInit, and every one that
        arrived through the loader hook after it. Without a window of its own
        the process never got far enough to tell, so nothing is claimed.
    */
    if (WantsAdobeHooks() && !g_dvauiHooked && HasFramedWindow()) {
        ReportNoColorModule();
    }

    // Nothing to hand back there: After Effects parsed those files already.
    if (g_bundledRecolored) {
        Wh_Log(L"%ld UXP files were recolored this session; those panels keep "
               L"the palette until After Effects restarts",
               g_bundledRecolored);
    }

    RevertThemedWindows();
    ApplyAppMode(false);

    /*
        Menus opened while the hooks were swapping in DarkMode::Menu keep that
        theme cached in user32 until it is flushed. If the setting was switched
        off earlier, that change already flushed it.
    */
    if (CurrentSettings().menuHook && g_FlushMenuThemes) {
        g_FlushMenuThemes();
    }

    RedrawProcessWindows();

    /*
        Left behind on purpose, each for the reason given where it is declared:
        the color table and the g_sysBrushes brushes. Both are reachable by
        After Effects after this image is gone — the table because After
        Effects holds pointers into it, which is why it lives in the process
        heap and not here, and the brushes because a window may still be
        painting with one.
    */

    if (g_uxtheme) {
        FreeLibrary(g_uxtheme);
        g_uxtheme = nullptr;
    }
}

/*
    Settings are applied in place; the mod is not reloaded.

    A reload would leave the color table and the system brushes behind once
    per change, and trying out palettes changes settings many times in a row.
    It would also do less: the colors After Effects took before the change would
    stay on the palette they were converted under.

    Every hook already checks its own setting each time it runs. What is left
    is the state that lives outside the hooks: the color table, the process
    app mode, the window frames, the menu themes user32 caches, and a repaint
    so the new colors show.
*/
void Wh_ModSettingsChanged() {
    Settings previous = CurrentSettings();

    LoadSettings();
    InterlockedIncrement(&g_generation);

    // A setting that works through dvaui may just have come on.
    bool registered = HookLoadedModules(nullptr, true);

    if (registered && !Wh_ApplyHookOperations()) {
        Wh_Log(L"failed to apply hooks after a settings change");
    }

    RecomputeColorTable(false);

    const Settings& now = CurrentSettings();

    ApplyAppMode(now.nativeDarkMode);

    if (previous.nativeDarkMode && !now.nativeDarkMode) {
        RevertThemedWindows();
    } else if (!previous.nativeDarkMode && now.nativeDarkMode) {
        ApplyThemeToExistingWindows();
    } else if (now.nativeDarkMode &&
               memcmp(&previous.palette, &now.palette, sizeof(Palette)) != 0) {
        RecolorThemedFrames();
    }

    // ApplyAppMode flushes these itself when the mode changes.
    if (previous.menuHook != now.menuHook && g_FlushMenuThemes) {
        g_FlushMenuThemes();
    }

    RedrawProcessWindows();
}
