// ==WindhawkMod==
// @id              premiere-pro-theme
// @name            Premiere Pro Theme
// @description     Recolors the Adobe Premiere Pro interface — panels, timeline, monitors, window frame and menu bar — with a choice of very dark palettes.
// @version         1.0.0
// @author          Threshold Editor
// @license         MIT
// @include         Adobe Premiere Pro.exe
// @include         Adobe Premiere Pro (Beta).exe
// @github          https://github.com/CakeDev4k
// @architecture    x86-64
// @compilerOptions -ldwmapi -lgdi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Premiere Pro Theme

Recolors Premiere Pro far beyond what the Appearance brightness slider reaches,
and does it consistently across the parts of the app that are painted in
different ways.

## Screenshots

The same project in each palette. First, Premiere without the mod: the Spectrum
gray `#1D1D1D`, under the light title bar and menu bar that Windows gives it.

![Premiere Pro without the mod](https://raw.githubusercontent.com/CakeDev4k/premiere-pro-theme/main/images/stock.png)

**Onyx** — the default: near black, and neutral:

![Onyx palette](https://raw.githubusercontent.com/CakeDev4k/premiere-pro-theme/main/images/onyx.png)

**Premiere** — the violet sampled from the app icon:

![Premiere palette](https://raw.githubusercontent.com/CakeDev4k/premiere-pro-theme/main/images/premiere.png)

**Comfy** — warm brown, low contrast for long sessions:

![Comfy palette](https://raw.githubusercontent.com/CakeDev4k/premiere-pro-theme/main/images/comfy.png)

**Neon** — near black with magenta:

![Neon palette](https://raw.githubusercontent.com/CakeDev4k/premiere-pro-theme/main/images/neon.png)

**Glitch** — acid green with a magenta accent:

![Glitch palette](https://raw.githubusercontent.com/CakeDev4k/premiere-pro-theme/main/images/glitch.png)

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

| Level   | Premiere  | Comfy     | Neon      | Glitch    |
|---------|-----------|-----------|-----------|-----------|
| Base    | `#06060F` | `#100C09` | `#080508` | `#060A02` |
| Panel   | `#0C0C20` | `#181310` | `#0E0A0D` | `#0C1305` |
| Surface | `#131333` | `#211A15` | `#1A0F18` | `#131D08` |
| Raised  | `#1E1E4D` | `#2E241C` | `#2A1526` | `#1E2C0C` |
| Border  | `#33337A` | `#453528` | `#4D213F` | `#3C5410` |
| Text    | `#D2D2F7` | `#EDE0D0` | `#FAF5EF` | `#E9F7C0` |
| Accent  | `#3A3A99` | `#5A4028` | `#5C1F47` | `#B8157A` |

Vivid — the hue is in the ramp, so the panels themselves carry it:

| Level     | Violet    | Blossom   | Ember     | Crimson   |
|-----------|-----------|-----------|-----------|-----------|
| Base      | `#0A0414` | `#120A0E` | `#0C0703` | `#0D0405` |
| Panel     | `#120827` | `#180D13` | `#140B04` | `#150609` |
| Surface   | `#1C0C3B` | `#241320` | `#1F1206` | `#200A0E` |
| Raised    | `#2A1257` | `#341B2E` | `#2E1A08` | `#300F15` |
| Border    | `#4B2088` | `#5A2C4A` | `#55300C` | `#591A24` |
| Text      | `#E2D4FA` | `#FBDCE8` | `#FBE3C8` | `#F7D9DD` |
| Accent    | `#6D28D9` | `#864160` | `#944108` | `#B91C1C` |
| Highlight | `#7737DC` | `#934869` | `#9F4608` | `#BB2222` |

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

**Premiere** takes its violet from the app's icon; **Neon** and **Glitch** come
from reference artwork, and **Comfy** is the lightest, for long sessions.
**Contrast** keeps a `#4A4A4A` border so edges stay legible on black panels.
**Blossom** is pastel in its text and border only: a pastel background would
leave Premiere's own light text unreadable. **Threshold** is
threshold-editor.com.br's `#050505`, `#FFFFFF` and `#DC2626`.

The accent stays out of the ramp, which is interpolated, so it only shows on
hovered menu items and system highlights; each one is at least 4.5:1 against
its palette's text.

Contrast and the palettes with a **Highlight** row give Premiere's blue — track
targeting, the focused panel's border, the active tool, selections, links —
their own hue, each blue at its own brightness, so white text on a blue button
keeps its contrast. The row is the track-targeting shade; Contrast's is the gray
`#606060`. **Palette highlight** in the settings keeps the blue on any palette.

## Custom themes

**Custom** reads a whole theme from one setting, as JSON, so a theme can be
shared as text and imported by pasting it into **Custom theme (JSON)**:

```json
{
  "name": "Threshold",
  "author": "Threshold Editor",
  "base": "#050505",
  "panel": "#0A0A0A",
  "surface": "#121212",
  "raised": "#1C1C1C",
  "border": "#2B2B2B",
  "text": "#FFFFFF",
  "accent": "#DC2626",
  "highlight": "#DC2626"
}
```

| Key              |          | What it colors                                                  |
|------------------|----------|-----------------------------------------------------------------|
| `base`           | required | the darkest step: the deepest background                        |
| `panel`          | required | the second step, and the title bar and menu bar                 |
| `surface`        | required | the middle step                                                 |
| `raised`         | required | the fourth step                                                 |
| `border`         | required | the lightest step: dividers and edges                           |
| `text`           | required | menu and title bar text                                         |
| `accent`         | optional | hovered menu items; the border when left out                    |
| `disabledText`   | optional | disabled menu items; halfway from text to panel when left out   |
| `highlight`      | optional | the hue Premiere's blue takes; the blue stays when left out     |
| `name`, `author` | optional | written to the log                                              |

Colors are `"#RRGGBB"`; an optional key left empty (`""`) takes its default, so
`"highlight": ""` keeps Premiere's blue. The setting starts with every key and
Onyx's colors. Unknown keys, and anything copied around the braces, are
ignored. Invalid JSON is not applied — Onyx stays and the log says where it
broke — and a missing required color falls back to Onyx's, logged by name.

Keep the five steps dark and in order: Premiere's own text is light and is not
recolored.

## What it changes, and what it leaves alone

Each layer has its own switch in the settings:

- **Premiere interface** — the theme colors served by `dvaui.dll`, Adobe's UI
  toolkit: panels, timeline, monitors.
- **Direct fills** — surfaces Premiere paints without asking the theme, most of
  the monitor and timeline chrome.
- **Window and system dialogs** — dark title bar, border and native dialogs.
- **Menu bar and menus** — the File / Edit / Clip bar and its dropdowns.
- **GDI surfaces** — brushes, pens and text backgrounds created by Premiere's
  own modules.
- **UXP panels** — the Text panel, Import, Export, Quick Export, Progress,
  Preset Manager and the Home screen, which Premiere draws from stylesheets of
  their own.
- **Palette highlight** — Premiere's blue, on the palettes that carry a
  highlight.

Saturated colors — clips, labels, warnings, and Premiere's blue unless the
palette carries a highlight — pass through, and so does anything above the
**brightness ceiling**: text, icons, and the `#4B4B4B` of disabled text.
Content is left alone whatever its color: the color picker's swatches, markers,
Essential Graphics, and the parameter colors of Effect Controls, Lumetri and the
monitors.

## If something becomes unreadable

Lower the ceiling. Raising it darkens more, starting with disabled text — which
is exactly what disappears first.

Settings apply while Premiere is running. Some surfaces keep the previous
palette until Premiere restarts, because Premiere copies those colors into
caches of its own.

If a Premiere update ever makes a panel misbehave with the mod on, switch off
**Premiere interface**, **Direct fills** and **GDI surfaces** together and
restart Premiere. With all three off when the mod loads, it does not hook
Premiere's own modules at all.

## Other mods that darken menus

If your menu bar ignores the palette, another mod is painting it first: any mod
that darkens Win32 menus globally answers the same `DefWindowProc` messages
before this one, with its own fixed color. Add `Adobe Premiere Pro.exe` to that
mod's process **exclusion** list and this one takes over.

## Known limitations

**UXP panels change on restart.** Premiere reads their stylesheets once, when a
panel loads, and the mod recolors that read — a temporary copy, deleted as soon
as it is closed; nothing on disk changes. So a palette switch, or disabling the
mod, shows on those panels after Premiere restarts. Frame.io and Adobe Stock
carry no Spectrum grays and keep their own look.

**The band around the video, with a tinted palette.** Zoomed out, the monitors
paint the area around the picture a gray made from the red channel of the panel
color, cached until Premiere restarts. On the neutral palettes it matches the
panels; with a hue it shows as a neutral band (Glitch's `#101907` gives
`#101010`), which is why the warm palettes keep their panels' red low. Its
painter has not been found, so the mod leaves it alone rather than patch blind.

The black *inside* the sequence frame is the rendered picture, not chrome, and
stays black in every palette.

## Compatibility

The mod looks up each color function by name at startup, installs the ones the
running build exports, and logs the rest, so a version that moved or dropped a
function loses that surface, not the mod. Measured against the installed
builds:

| Premiere | Color functions found | Drawing primitives |
|----------|-----------------------|--------------------|
| 2026     | 31 of 31              | 4 of 4             |
| 2023     | 27 of 31              | 4 of 4             |

The four missing on 2023 do not exist there under any name, and their surfaces
fall back to the Spectrum family, which both have. The window frame, the menu
bar and the native dialogs do not depend on the Premiere version. Native dark
mode needs Windows 10 build 17763 or newer; below that the mod still themes the
interface and paints the menus itself.

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
  - premiere: Premiere — the violet sampled from the app icon
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
  - custom: Custom — the theme JSON below
- customTheme: '{"name": "My theme", "author": "", "base": "#050505", "panel": "#090909", "surface": "#0E0E0E", "raised": "#161616", "border": "#242424", "text": "#E6E6E6", "disabledText": "#777777", "accent": "#2E2E2E", "highlight": ""}'
  $name: Custom theme (JSON)
  $description: >-
    Used when the palette is Custom. Paste a theme someone shared, or edit this
    one, which starts with every key and Onyx's colors. Colors are "#RRGGBB".
    base, panel, surface, raised, border and text are required; accent,
    disabledText and highlight can be left empty for their defaults, and an
    empty highlight keeps Premiere's blue. The readme has the full format.
- strength: 100
  $name: Strength
  $description: How much of the palette is applied over the original color, in percent. 100 = palette only.
- ceiling: 28
  $name: Brightness ceiling
  $description: >-
    The highest brightness, in percent, still treated as background and
    darkened. Anything above passes through untouched. The default 28 sits just
    below the #4B4B4B that Spectrum uses for disabled text and dividers —
    raising it starts erasing that text.
- dvauiHook: true
  $name: Premiere interface
  $description: Intercepts the theme color functions in dvaui.dll. This is the layer that recolors panels, timeline and monitors.
- brushHook: true
  $name: Direct fills
  $description: >-
    Also intercepts the Direct2D brush factory and the UIFramework drawing
    primitives — surfaces painted without consulting the theme, which is most of
    the monitor and timeline chrome. Turn this off if a panel paints wrong.
- nativeDarkMode: true
  $name: Window and system dialogs
  $description: Immersive dark mode, title bar, border and native dialogs.
- menuHook: true
  $name: Menu bar and menus
  $description: Paints the File/Edit/Clip bar and the dropdown menus in the palette, instead of white or the Windows default gray.
- gdiHook: true
  $name: GDI surfaces
  $description: Darkens GDI brushes, pens and text backgrounds created by Premiere's own modules.
- uxpPanels: true
  $name: UXP panels
  $description: >-
    The Text panel, Import, Export, Quick Export, Progress, Preset Manager and
    the Home screen, which Premiere draws from stylesheets of their own. Those
    are read once, when a panel loads, so this switch, a palette change and
    disabling the mod all show there after Premiere restarts.
- highlight: true
  $name: Palette highlight
  $description: >-
    Gives Premiere's blue — track targeting, the focused panel's border, the
    active tool, selections and links — the palette's own hue, each blue at its
    own brightness. Contrast, Violet, Blossom, Ember, Amethyst, Crimson and
    Threshold carry one, and a custom theme can; the others keep the blue.
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
#include <string>
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
        The hue Premiere's blue takes: track targeting, the focused panel's
        border, the active tool. Each blue keeps its own luminance and takes
        only the hue, so white text on a blue button keeps its contrast.
        CLR_INVALID keeps the blue, as Onyx through Glitch do.
    */
    COLORREF highlight = CLR_INVALID;
};

/*
    Premiere's interface blues, as dvaui's color tables and the bundled UXP
    stylesheets hold them: the Spectrum 2 ramp dvaui draws the interface with,
    then the Spectrum 1 values the UXP panels still carry. Matched exactly,
    never by hue: dvaui holds hundreds of blues, and a clip label or a picked
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

static float Blend(float original, float target) {
    float strength = CurrentSettings().strength;
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
    uintptr_t begin;
    uintptr_t end;
};

/*
    Premiere 2026 maps 59 dva* modules plus UIFramework and the executable;
    the rest is headroom for versions that add more.
*/
constexpr size_t kMaxModuleRanges = 256;

ModuleRange g_moduleRanges[kMaxModuleRanges];
volatile LONG g_moduleRangeCount = 0;
volatile LONG g_moduleRangesFullLogged = FALSE;
SRWLOCK g_moduleRangeLock = SRWLOCK_INIT;

/*
    Adobe's whole UI toolkit is prefixed dva (dvaui, dvacore, ...).
    UIFramework is Premiere's own drawing layer, hooked elsewhere as a
    first-class paint layer, so it belongs in the same set. The length is
    passed in because the loader hands names over without a terminator.
*/
static bool IsAdobeUIName(const wchar_t* name, size_t length) {
    constexpr wchar_t kUif[] = L"UIFramework.dll";
    constexpr size_t kUifLength = ARRAYSIZE(kUif) - 1;

    return (length >= 3 && _wcsnicmp(name, L"dva", 3) == 0) ||
           (length == kUifLength && _wcsnicmp(name, kUif, kUifLength) == 0);
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
    bool known = false;

    for (LONG i = 0; i < count; i++) {
        if (g_moduleRanges[i].begin == begin) {
            known = true;
            break;
        }
    }

    bool full = !known && count >= static_cast<LONG>(kMaxModuleRanges);

    if (!known && !full) {
        g_moduleRanges[count] = {begin, end};
        InterlockedExchange(&g_moduleRangeCount, count + 1);
    }

    ReleaseSRWLockExclusive(&g_moduleRangeLock);

    if (full && Claim(&g_moduleRangesFullLogged)) {
        Wh_Log(L"module range table full; GDI calls from Adobe modules loaded "
               L"from now on will not be recognized");
    }
}

/*
    Every module mapped after init, including the dependencies LoadLibraryExW
    never returns, which is how most dva* modules arrive in Premiere. The
    notification carries the name, base and size, so nothing here asks the
    loader anything. It runs with the loader lock held, and it is unregistered
    in Wh_ModUninit, before the image that contains it goes away.
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

using LdrDllNotification_t = VOID(CALLBACK*)(ULONG, const LdrDllLoadedData*,
                                             PVOID);
using LdrRegisterDllNotification_t = LONG(NTAPI*)(ULONG, LdrDllNotification_t,
                                                  PVOID, PVOID*);
using LdrUnregisterDllNotification_t = LONG(NTAPI*)(PVOID);

PVOID g_dllNotificationCookie = nullptr;

static VOID CALLBACK OnDllNotification(ULONG reason,
                                       const LdrDllLoadedData* data, PVOID) {
    if (reason != kLdrDllLoaded || !data || !data->baseDllName ||
        !data->baseDllName->buffer) {
        return;
    }

    const LdrUnicodeString& name = *data->baseDllName;

    if (!IsAdobeUIName(name.buffer, name.length / sizeof(wchar_t))) {
        return;
    }

    auto base = reinterpret_cast<uintptr_t>(data->dllBase);
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

    if (unregisterNotification) {
        unregisterNotification(g_dllNotificationCookie);
    }

    g_dllNotificationCookie = nullptr;
}

/*
    The modules already mapped when the mod starts; OnDllNotification covers
    the ones after. It is registered first, so a module mapped in between is
    seen twice and deduplicated rather than missed.

    The list is sized from what the process reports (Premiere 2026 runs with
    more than 560 modules). Sizes come from K32GetModuleInformation rather
    than from the module's headers, so a module unloaded in between costs a
    failed call, not an access violation.
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
static void NoteModuleFromHeaders(HMODULE module) {
    if (!module) {
        return;
    }

    auto base = reinterpret_cast<uintptr_t>(module);
    auto dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(base);

    if (dos->e_magic != IMAGE_DOS_SIGNATURE) {
        return;
    }

    auto nt = reinterpret_cast<const IMAGE_NT_HEADERS*>(base + dos->e_lfanew);

    if (nt->Signature != IMAGE_NT_SIGNATURE) {
        return;
    }

    AddModuleRange(base, base + nt->OptionalHeader.SizeOfImage);
}

static void NoteKnownModules() {
    NoteModuleFromHeaders(GetModuleHandleW(nullptr));
    NoteModuleFromHeaders(GetModuleHandleW(L"dvaui.dll"));
    NoteModuleFromHeaders(GetModuleHandleW(L"dvacore.dll"));
    NoteModuleFromHeaders(GetModuleHandleW(L"UIFramework.dll"));
}

static void SnapshotAdobeModules() {
    /*
        These are recorded first, whatever the enumeration below manages. It
        can fail or come up short while Premiere is still mapping modules from
        several threads, and the executable was mapped before the loader
        notification could see it. A module recorded twice is deduplicated.
    */
    NoteKnownModules();

    HMODULE kernel32 = GetModuleHandleW(L"kernel32.dll");

    auto enumModules =
        kernel32 ? reinterpret_cast<EnumProcessModules_t>(
                       GetProcAddress(kernel32, "K32EnumProcessModules"))
                 : nullptr;

    auto moduleInformation =
        kernel32 ? reinterpret_cast<GetModuleInformation_t>(
                       GetProcAddress(kernel32, "K32GetModuleInformation"))
                 : nullptr;

    if (!enumModules || !moduleInformation) {
        Wh_Log(L"cannot enumerate modules; besides the executable, dvaui, "
               L"dvacore and UIFramework, only dva modules loaded from now on "
               L"will be recognized as Adobe UI");
        return;
    }

    HANDLE process = GetCurrentProcess();
    HMODULE executable = GetModuleHandleW(nullptr);

    std::vector<HMODULE> modules(1024);
    DWORD needed = 0;
    DWORD bytes = 0;

    // Modules can load between two calls, so a retry may still come up short.
    for (int attempt = 0; attempt < 3; attempt++) {
        bytes = static_cast<DWORD>(modules.size() * sizeof(HMODULE));

        if (!enumModules(process, modules.data(), bytes, &needed)) {
            Wh_Log(L"module enumeration failed (%u); besides the executable, "
                   L"dvaui, dvacore and UIFramework, only dva modules loaded "
                   L"from now on will be recognized as Adobe UI",
                   GetLastError());
            return;
        }

        if (needed <= bytes) {
            break;
        }

        if (attempt < 2) {
            modules.resize(needed / sizeof(HMODULE) + 64);
        }
    }

    size_t count = std::min(needed, bytes) / sizeof(HMODULE);

    for (size_t i = 0; i < count; i++) {
        HMODULE module = modules[i];
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

static bool IsAdobeUICaller(void* caller) {
    auto p = reinterpret_cast<uintptr_t>(caller);
    LONG count = g_moduleRangeCount;

    for (LONG i = 0; i < count; i++) {
        if (p >= g_moduleRanges[i].begin && p < g_moduleRanges[i].end) {
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
    own ASLColorToDVAColorRGBA writes it. If a future Premiere changes the
    struct, the values read here stop looking like a color, and the original is
    returned rather than garbage painted.

    Two kinds of color are converted: a dark neutral gray and, on a palette
    with a highlight, one of Premiere's own blues. Every other saturated color
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

static bool ShouldConvert(const DvaColorRGBA& in) {
    return ShouldConvertWith(CurrentSettings(), in);
}

/*
    The tone `in` becomes: a ramp stop for a gray, the highlight shade for a
    blue. One settings snapshot for the whole decision, so a palette change
    cannot pair one palette's test with another's shades. No side effects, so
    the stylesheet rewrite shares it.
*/
static bool PaletteTarget(const DvaColorRGBA& in, COLORREF* target, int* blue) {
    const Settings& s = CurrentSettings();

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

static bool ConvertDvaColor(const DvaColorRGBA& in, DvaColorRGBA* out) {
    COLORREF target = 0;
    int blue = -1;

    if (!PaletteTarget(in, &target, &blue)) {
        return false;
    }

    if (blue >= 0) {
        LONG64 bit = LONG64{1} << blue;

        if (!(g_bluesRecolored & bit)) {
            InterlockedOr64(&g_bluesRecolored, bit);
        }
    }

    out->r = Blend(in.r, GetRValue(target) / 255.0f);
    out->g = Blend(in.g, GetGValue(target) / 255.0f);
    out->b = Blend(in.b, GetBValue(target) / 255.0f);
    out->a = in.a;

    return true;
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
    unload: the key is an address the theme hands out, and the theme holds
    few color objects. 128 is a tenfold margin. If the table ever fills,
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

    Premiere keeps the pointers this table hands out. A static array would
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
    Premiere still holds from the table stock gray once "Premiere interface" is
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
    computed under the old settings, and Premiere may keep the slot it lands
    in without ever asking again. So a write computed under an older
    generation never replaces a slot the change already recomputed, and a
    slot that was still being filled when the change ran is recomputed by the
    thread filling it (see StoreColor). Neither can leave a color Premiere
    holds on the previous palette.
*/
volatile LONG g_generation = 0;

static void RememberProduced(const DvaColorRGBA& c);

/*
    Recomputes one slot's dst from its src under the settings in force now.
    With restoreOriginals, or with "Premiere interface" off, dst goes back to
    src.
*/
static void RefreshSlot(ColorSlot& slot, LONG generation, bool restoreOriginals) {
    // ConvertDvaColor leaves dst untouched when it declines.
    DvaColorRGBA dst = slot.src;

    if (!restoreOriginals && CurrentSettings().dvauiHook &&
        ConvertDvaColor(slot.src, &dst)) {
        RememberProduced(dst);
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
        Wh_Log(L"color table full; further interface colors keep Premiere's "
               L"own value");
    }

    return nullptr;
}

/*
    Second line of defense against converting a color twice, by value.

    A converted color arrives again when a brush is built from it, and
    converting twice pushes everything toward the darkest stop. The address
    settles that when the brush gets the table's own pointer (IsConvertedSlot),
    but not once the caller has copied the color. So every color produced goes
    into this set, and one found in it passes through. The key is 8-bit RGB, so
    it survives a round trip through another format.
*/
constexpr size_t kProducedSlots = 8192;

volatile LONG g_produced[kProducedSlots];

static LONG PackColorKey(const DvaColorRGBA& c) {
    int r = ClampInt(static_cast<int>(c.r * 255.0f + 0.5f), 0, 255);
    int g = ClampInt(static_cast<int>(c.g * 255.0f + 0.5f), 0, 255);
    int b = ClampInt(static_cast<int>(c.b * 255.0f + 0.5f), 0, 255);

    // +1 because 0 means an empty slot.
    return static_cast<LONG>((r << 16) | (g << 8) | b) + 1;
}

static size_t ProducedIndex(LONG key) {
    return FibonacciIndex<kProducedSlots>(static_cast<uint32_t>(key));
}

/*
    Almost every color is already in the set, so slots are read plainly and only
    an empty one is claimed with an interlocked exchange.
*/
static void RememberProduced(const DvaColorRGBA& c) {
    LONG key = PackColorKey(c);
    size_t start = ProducedIndex(key);

    for (size_t probe = 0; probe < 32; probe++) {
        size_t i = (start + probe) & (kProducedSlots - 1);
        LONG cur = g_produced[i];

        if (cur == key) {
            return;
        }

        if (cur == 0) {
            cur = InterlockedCompareExchange(&g_produced[i], key, 0);

            if (cur == 0 || cur == key) {
                return;
            }
        }
    }
}

// Plain reads: an aligned LONG is read atomically on x86-64.
static bool IsProducedColor(const DvaColorRGBA& c) {
    LONG key = PackColorKey(c);
    size_t start = ProducedIndex(key);

    for (size_t probe = 0; probe < 32; probe++) {
        LONG cur = g_produced[(start + probe) & (kProducedSlots - 1)];

        if (cur == 0) {
            return false;
        }
        if (cur == key) {
            return true;
        }
    }

    return false;
}

static const DvaColorRGBA* ConvertColorRef(const DvaColorRGBA* original) {
    // Read before the settings are, so a change in between is caught as stale.
    LONG generation = g_generation;

    if (!original || !CurrentSettings().dvauiHook || IsOurSlot(original)) {
        return original;
    }

    DvaColorRGBA converted{};

    if (!ConvertDvaColor(*original, &converted)) {
        return original;
    }

    RememberProduced(converted);

    const DvaColorRGBA* stored = StoreColor(reinterpret_cast<uintptr_t>(original),
                                            *original, converted, generation);

    return stored ? stored : original;
}

/*
    Brings every color already handed out in line with the current settings,
    in place, so the references Premiere holds follow a settings change. With
    restoreOriginals — on unload — every slot goes back to the color it
    replaced, since the table outlives the mod.

    A slot another thread is filling (state 1) is skipped; that thread brings
    it up to date when it publishes it — see StoreColor. Returns how many
    slots are in use.
*/
static size_t RecomputeColorTable(bool restoreOriginals) {
    // Emptied and refilled below, or a color from the previous palette would
    // look already converted.
    for (size_t i = 0; i < kProducedSlots; i++) {
        g_produced[i] = 0;
    }

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
    Not every Premiere version exports everything: 2023 has no `dna` family, which
    only appears in the versions with the newer Spectrum. Whatever is missing is
    logged and the rest carries on.

    Fourteen of them were renamed in Premiere 2026, when Adobe swapped
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
    Every hook into Premiere's modules goes in through here: looked up by its
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

static void InstallHook(HMODULE module, const HookSpec& spec, HookCount* count) {
    FARPROC proc = GetProcAddress(module, spec.mangled);
    bool installed = false;

    if (!proc) {
        Wh_Log(L"absent in this version: %s", spec.label);
    } else if (!Wh_SetFunctionHook(reinterpret_cast<void*>(proc), spec.hook,
                                   spec.original)) {
        Wh_Log(L"failed to hook %s", spec.label);
    } else {
        installed = true;
    }

    if (count) {
        (installed ? count->installed : count->missing)++;
    }
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
    Much of the monitor and timeline chrome is painted without asking the
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
    return CurrentSettings().brushHook || CurrentSettings().gdiHook;
}

/*
    Cheapest test first: most colors arriving here are saturated or too light,
    and float comparisons turn them away before the hash lookup.
*/
static bool ConvertForPaint(const DvaColorRGBA* in, DvaColorRGBA* out) {
    if (!CurrentSettings().brushHook || !in || InContentScope()) {
        return false;
    }

    // Two pointer comparisons for anything outside the table.
    if (IsConvertedSlot(in)) {
        return false;
    }

    // Cheap float test, rejects the majority.
    if (!ShouldConvert(*in)) {
        return false;
    }

    // Only now the table, and only for what is left.
    if (IsProducedColor(*in)) {
        return false;
    }

    if (!ConvertDvaColor(*in, out)) {
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
    DvaColorRGBA converted{};

    if (ConvertForPaint(color, &converted)) {
        DispatchDrawFromRoot_Original(self, &converted, drawbot, flag);
        return;
    }

    DispatchDrawFromRoot_Original(self, color, drawbot, flag);
}

/*
    UIFramework.dll, Premiere's UI layer on top of the toolkit, has drawing
    primitives of its own that take a color directly and never ask the theme:

        UIF::DC::FillRect(const RectT<int>&, const ColorRGBA&)
        UIF::DC::FrameRect(const RectT<int>&, const ColorRGBA&)

    Both read the color while drawing, so a stack temporary is safe here.
*/
using UifFillRect_t = void (*)(void*, const void*, const DvaColorRGBA*);

UifFillRect_t UifFillRect_Original = nullptr;
UifFillRect_t UifFrameRect_Original = nullptr;

void UifFillRect_Hook(void* self, const void* rect, const DvaColorRGBA* color) {
    DvaColorRGBA converted{};

    if (ConvertForPaint(color, &converted)) {
        UifFillRect_Original(self, rect, &converted);
        return;
    }

    UifFillRect_Original(self, rect, color);
}

void UifFrameRect_Hook(void* self, const void* rect, const DvaColorRGBA* color) {
    DvaColorRGBA converted{};

    if (ConvertForPaint(color, &converted)) {
        UifFrameRect_Original(self, rect, &converted);
        return;
    }

    UifFrameRect_Original(self, rect, color);
}

/*
    The ParamColor overloads, which the mod deliberately leaves alone.

    ASL::ParamColor<unsigned char> is a parameter color, the 8-bit color an
    effect or a clip carries. Every module that calls these is a panel showing
    user content — Effect Controls, the Timeline, the monitors, Lumetri,
    Essential Graphics — and a diagnostic build saw only content colors arrive
    here, never an interface gray. So they pass their color through, inside a
    ContentScope, so that nothing they call further in converts it either.
*/
using UifFillRectParam_t = void (*)(void*, const void*, const void*,
                                    unsigned char);
using UifFrameRectParam_t = void (*)(void*, const void*, const void*);

UifFillRectParam_t UifFillRectParam_Original = nullptr;
UifFrameRectParam_t UifFrameRectParam_Original = nullptr;

void UifFillRectParam_Hook(void* self, const void* rect, const void* color,
                           unsigned char flags) {
    ContentScope scope(__builtin_frame_address(0));
    UifFillRectParam_Original(self, rect, color, flags);
}

void UifFrameRectParam_Hook(void* self, const void* rect, const void* color) {
    ContentScope scope(__builtin_frame_address(0));
    UifFrameRectParam_Original(self, rect, color);
}

static void InstallUifHooks(HMODULE uif, HookCount& count) {
    const HookSpec specs[] = {
        {"?FillRect@DC@UIF@@QEAAXAEBV?$RectT@H@geom@dvacore@@"
         "AEBVColorRGBA@drawbot@dvaui@@@Z",
         reinterpret_cast<void*>(UifFillRect_Hook),
         reinterpret_cast<void**>(&UifFillRect_Original), L"UIF::DC::FillRect"},

        {"?FrameRect@DC@UIF@@QEAAXAEBV?$RectT@H@geom@dvacore@@"
         "AEBVColorRGBA@drawbot@dvaui@@@Z",
         reinterpret_cast<void*>(UifFrameRect_Hook),
         reinterpret_cast<void**>(&UifFrameRect_Original), L"UIF::DC::FrameRect"},

        {"?FillRect@DC@UIF@@QEAAXAEBV?$RectT@H@geom@dvacore@@"
         "AEBV?$ParamColor@E@ASL@@E@Z",
         reinterpret_cast<void*>(UifFillRectParam_Hook),
         reinterpret_cast<void**>(&UifFillRectParam_Original),
         L"UIF::DC::FillRect (ParamColor)"},

        {"?FrameRect@DC@UIF@@QEAAXAEBV?$RectT@H@geom@dvacore@@"
         "AEBV?$ParamColor@E@ASL@@@Z",
         reinterpret_cast<void*>(UifFrameRectParam_Hook),
         reinterpret_cast<void**>(&UifFrameRectParam_Original),
         L"UIF::DC::FrameRect (ParamColor)"},
    };

    InstallHooks(uif, specs, &count);
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
    ".?AVColorPickerChipView@ColorPicker@DLG@@",  // Premiere's color picker
    ".?AVColorPickerChipView@sharedui@dvaui@@",   // the shared dvaui one
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

static bool IsContentNode(const void* node) {
    uintptr_t vtable = *static_cast<const uintptr_t*>(node);

    if (!vtable || (vtable & 7)) {
        return false;
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
    The two modules do not necessarily load together, so each is tracked on its
    own and hooked when it arrives — once, by whichever thread gets there
    first. Premiere loads libraries from several threads at startup, and a
    plain flag read and then set would let two of them register the same hooks.
*/
volatile LONG g_dvauiHooked = FALSE;
volatile LONG g_uifHooked = FALSE;

/*
    Registers the hooks for whichever of the two modules is loaded now and not
    done yet, and returns whether it registered any. It does not apply them:
    Windhawk does that when Wh_ModInit returns, and the loader hook and the
    settings callback apply what this registers.

    Once in, a hook stays in and checks its own setting each time it runs,
    which is what lets settings change without a reload. They only go in while
    a setting that works through these modules is on: with "Premiere
    interface", "Direct fills" and "GDI surfaces" all off when the mod loads,
    dvaui and UIFramework are not touched at all — the way back to a working
    Premiere if an update ever breaks one of these hooks.
*/
static bool WantsPremiereHooks() {
    return CurrentSettings().dvauiHook || CurrentSettings().brushHook || CurrentSettings().gdiHook;
}

static bool HookLoadedModules() {
    if (!WantsPremiereHooks()) {
        return false;
    }

    bool registered = false;

    HMODULE dvaui = GetModuleHandleW(L"dvaui.dll");

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
            Wh_Log(L"no dvaui color entry point matched — this Premiere version "
                   L"is not supported by the interface layer. The window frame "
                   L"and menus still apply.");
        }
    }

    HMODULE uif = GetModuleHandleW(L"UIFramework.dll");

    if (uif && Claim(&g_uifHooked)) {
        HookCount count;

        InstallUifHooks(uif, count);
        registered = true;

        Wh_Log(L"UIFramework: %d hooks active, %d absent in this version",
               count.installed, count.missing);
    }

    return registered;
}

/*
    dvaui and UIFramework are not necessarily loaded when the mod initializes —
    enable the mod before Premiere starts and neither is — so the loader is
    hooked, and each module is hooked when it arrives. The hook goes in
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
        both are hooked, this is a couple of plain reads.
    */
    if ((!g_dvauiHooked || !g_uifHooked) && HookLoadedModules() &&
        !Wh_ApplyHookOperations()) {
        Wh_Log(L"failed to apply hooks for a late-loaded module");
    }

    return module;
}

// ============================================================================
// UXP PANELS: THE STYLESHEETS THEY READ
// ============================================================================

/*
    The Text panel, Import, Export, Quick Export, Progress, Preset Manager and
    the Home screen are UXP plugins that paint without dvaui, from stylesheets
    with the Spectrum grays written into them. What reaches them is the file:
    a read of one of those stylesheets is answered with a recolored copy, a
    temporary file deleted on close. Every color keeps its length, because the
    UXP runtime also asks for a file's size by path.
*/

wchar_t g_uxpPluginsDir[MAX_PATH + 16] = {};  // folded, see FoldPathChar
size_t g_uxpPluginsDirLength = 0;

volatile LONG g_stylesheetSerial = 0;
volatile LONG g_stylesheetFailureLogged = FALSE;
volatile LONG g_stylesheetsRecolored = 0;  // logged at unload

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
    A .css file under Premiere's own UXP\plugins folder. `relative` receives
    the part after that folder, for the log.
*/
static bool IsBundledStylesheet(LPCWSTR path, LPCWSTR* relative) {
    if (!path || !g_uxpPluginsDirLength) {
        return false;
    }

    size_t length = wcslen(path);

    if (length < 4 || _wcsicmp(path + length - 4, L".css") != 0) {
        return false;
    }

    if (wcsncmp(path, L"\\\\?\\", 4) == 0) {
        path += 4;
        length -= 4;
    }

    if (length <= g_uxpPluginsDirLength) {
        return false;
    }

    for (size_t i = 0; i < g_uxpPluginsDirLength; i++) {
        if (FoldPathChar(path[i]) != g_uxpPluginsDir[i]) {
            return false;
        }
    }

    // A path that climbs back out of the folder is not one of its stylesheets.
    if (wcsstr(path + g_uxpPluginsDirLength, L"..")) {
        return false;
    }

    if (relative) {
        *relative = path + g_uxpPluginsDirLength;
    }

    return true;
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
static bool RecolorCssChannels(int rgb[3]) {
    DvaColorRGBA in{rgb[0] / 255.0f, rgb[1] / 255.0f, rgb[2] / 255.0f, 1.0f};
    COLORREF target = 0;
    int blue = -1;

    if (!PaletteTarget(in, &target, &blue)) {
        return false;
    }

    auto channel = [](float original, BYTE wanted) {
        float v = Blend(original, wanted / 255.0f) * 255.0f + 0.5f;
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
static bool RecolorCssHex(char* text, size_t size, size_t hash) {
    size_t start = hash + 1;
    size_t end = start;

    while (end < size && CssHexDigit(text[end]) >= 0) {
        end++;
    }

    size_t digits = end - start;

    if ((digits != 6 && digits != 8) || (end < size && IsCssWordChar(text[end]))) {
        return false;
    }

    int rgb[3];
    bool upper = false;

    for (int k = 0; k < 3; k++) {
        char high = text[start + 2 * k];
        char low = text[start + 2 * k + 1];

        rgb[k] = CssHexDigit(high) * 16 + CssHexDigit(low);
        upper = upper || (high >= 'A' && high <= 'F') || (low >= 'A' && low <= 'F');
    }

    if (!RecolorCssChannels(rgb)) {
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
static bool RecolorCssTriplet(char* text, size_t size, size_t from) {
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

    if (!RecolorCssChannels(rgb)) {
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

static bool CssPrecededBy(const char* text, size_t at, const char* word) {
    size_t length = strlen(word);

    return at >= length && _strnicmp(text + at - length, word, length) == 0;
}

/*
    Rewrites every color a stylesheet spells out, in place and at its own
    length, and returns how many changed. A value taken from a variable, like
    rgb(var(--x)), changes where the variable is defined.
*/
static size_t RecolorStylesheet(char* text, size_t size) {
    size_t changed = 0;

    for (size_t i = 0; i < size; i++) {
        bool recolored = false;

        if (text[i] == '#') {
            recolored = RecolorCssHex(text, size, i);
        } else if (text[i] == ':' && CssPrecededBy(text, i, "-rgb")) {
            recolored = RecolorCssTriplet(text, size, i + 1);
        } else if (text[i] == '(' &&
                   (CssPrecededBy(text, i, "rgb") || CssPrecededBy(text, i, "rgba"))) {
            recolored = RecolorCssTriplet(text, size, i + 1);
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

// Far above any stylesheet Premiere ships.
constexpr LONGLONG kMaxStylesheetBytes = 16LL << 20;

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
              size.QuadPart <= kMaxStylesheetBytes;

    if (ok) {
        // A hook must not throw into Premiere: short of memory, the panel
        // simply gets its own stylesheet.
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
    A temporary file holding `bytes`, opened with the caller's
    FILE_FLAG_OVERLAPPED and positioned at the start. Windows deletes it when
    the handle closes, and also if Premiere exits without closing it.
*/
static HANDLE WriteTemporaryCopy(const std::vector<char>& bytes, DWORD callerFlags) {
    wchar_t folder[MAX_PATH + 1]{};
    DWORD length = GetTempPathW(ARRAYSIZE(folder), folder);

    if (!length || length >= ARRAYSIZE(folder)) {
        return INVALID_HANDLE_VALUE;
    }

    bool overlapped = (callerFlags & FILE_FLAG_OVERLAPPED) != 0;
    HANDLE copy = INVALID_HANDLE_VALUE;

    /*
        CREATE_NEW, never CREATE_ALWAYS: the name is predictable, so whatever
        already sits at it — a leftover, or a link planted to redirect the
        write — is stepped around rather than opened or followed.
    */
    for (int attempt = 0; attempt < 8 && copy == INVALID_HANDLE_VALUE; attempt++) {
        constexpr wchar_t kPrefix[] = L"premiere-pro-theme-";
        wchar_t name[MAX_PATH + 64]{};
        wchar_t* end = name;

        wmemcpy(end, folder, length);
        end += length;
        wmemcpy(end, kPrefix, ARRAYSIZE(kPrefix) - 1);
        end += ARRAYSIZE(kPrefix) - 1;
        end = AppendDecimal(end, GetCurrentProcessId());
        *end++ = L'-';
        end = AppendDecimal(
            end, static_cast<unsigned long>(InterlockedIncrement(&g_stylesheetSerial)));
        wmemcpy(end, L".css", 5);  // and its terminator

        copy = CreateFileW_Original(
            name, GENERIC_READ | GENERIC_WRITE | DELETE,
            FILE_SHARE_READ | FILE_SHARE_DELETE, nullptr, CREATE_NEW,
            FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_DELETE_ON_CLOSE |
                (overlapped ? FILE_FLAG_OVERLAPPED : 0),
            nullptr);

        DWORD error = copy == INVALID_HANDLE_VALUE ? GetLastError() : ERROR_SUCCESS;

        if (error != ERROR_SUCCESS && error != ERROR_FILE_EXISTS &&
            error != ERROR_ALREADY_EXISTS) {
            break;
        }
    }

    if (copy == INVALID_HANDLE_VALUE) {
        return copy;
    }

    auto size = static_cast<DWORD>(bytes.size());
    DWORD written = 0;
    bool ok = false;

    if (overlapped) {
        OVERLAPPED io{};
        io.hEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);

        ok = io.hEvent &&
             (WriteFile(copy, bytes.data(), size, nullptr, &io) ||
              GetLastError() == ERROR_IO_PENDING) &&
             GetOverlappedResult(copy, &io, &written, TRUE);

        if (io.hEvent) {
            CloseHandle(io.hEvent);
        }
    } else {
        LARGE_INTEGER start{};
        ok = WriteFile(copy, bytes.data(), size, &written, nullptr) &&
             SetFilePointerEx(copy, start, nullptr, FILE_BEGIN);
    }

    if (!ok || written != size) {
        CloseHandle(copy);
        return INVALID_HANDLE_VALUE;
    }

    return copy;
}

/*
    The recolored copy of a bundled stylesheet, or INVALID_HANDLE_VALUE for
    the caller to open the file itself: when there is nothing to recolor, and
    when anything fails.
*/
static HANDLE OpenThemedStylesheet(LPCWSTR path, LPCWSTR relative, DWORD flags) {
    std::vector<char> bytes;

    if (!ReadWholeFile(path, &bytes)) {
        return INVALID_HANDLE_VALUE;
    }

    size_t colors = RecolorStylesheet(bytes.data(), bytes.size());

    if (!colors) {
        return INVALID_HANDLE_VALUE;
    }

    HANDLE copy = WriteTemporaryCopy(bytes, flags);

    if (copy == INVALID_HANDLE_VALUE) {
        DWORD error = GetLastError();

        if (Claim(&g_stylesheetFailureLogged)) {
            Wh_Log(L"could not write a recolored stylesheet (%u); UXP panels "
                   L"keep their own colors",
                   error);
        }

        return INVALID_HANDLE_VALUE;
    }

    InterlockedIncrement(&g_stylesheetsRecolored);
    Wh_Log(L"UXP stylesheet recolored: %s, %u colors", relative,
           static_cast<unsigned>(colors));

    // A successful open of an existing file reports no error.
    SetLastError(ERROR_SUCCESS);

    return copy;
}

HANDLE WINAPI CreateFileW_Hook(LPCWSTR path, DWORD access, DWORD share,
                               LPSECURITY_ATTRIBUTES security, DWORD disposition,
                               DWORD flags, HANDLE templateFile) {
    LPCWSTR relative = nullptr;

    if (CurrentSettings().uxpPanels && IsPlainRead(access, disposition, flags) &&
        IsBundledStylesheet(path, &relative)) {
        HANDLE copy = OpenThemedStylesheet(path, relative, flags);

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
    DWORD flags = parameters ? parameters->dwFileFlags : 0;
    LPCWSTR relative = nullptr;

    if (CurrentSettings().uxpPanels && IsPlainRead(access, disposition, flags) &&
        IsBundledStylesheet(path, &relative)) {
        HANDLE copy = OpenThemedStylesheet(path, relative, flags);

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
*/
constexpr BYTE kThemedClass = 1;  // SetWindowTheme(DarkMode_Explorer)
constexpr BYTE kThemedFrame = 2;  // immersive dark mode, and caption colors on 22000+

SRWLOCK g_themedLock = SRWLOCK_INIT;
std::unordered_map<HWND, BYTE> g_themedWindows;
size_t g_themedPruneAt = 256;

static void RememberThemedWindow(HWND hwnd, BYTE applied) {
    if (!applied) {
        return;
    }

    AcquireSRWLockExclusive(&g_themedLock);

    g_themedWindows[hwnd] |= applied;

    if (g_themedWindows.size() >= g_themedPruneAt) {
        std::erase_if(g_themedWindows,
                      [](const auto& entry) { return !IsWindow(entry.first); });
        g_themedPruneAt = std::max<size_t>(256, g_themedWindows.size() * 2);
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
    AcquireSRWLockExclusive(&g_themedLock);

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

static void ApplyDarkModeToWindow(HWND hwnd) {
    if (!hwnd || !CurrentSettings().nativeDarkMode) {
        return;
    }

    BYTE applied = 0;

    if (g_AllowDarkModeForWindow) {
        g_AllowDarkModeForWindow(hwnd, true);
    }

    if (g_SetWindowTheme &&
        SUCCEEDED(g_SetWindowTheme(hwnd, L"DarkMode_Explorer", nullptr))) {
        applied |= kThemedClass;
    }

    /*
        Frame attributes only apply to a top-level window. The style bit, not
        GetParent: for an owned popup such as a dialog, GetParent returns the
        owner.
    */
    if (!(GetWindowLongPtrW(hwnd, GWL_STYLE) & WS_CHILD)) {
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
    had: Premiere's executable, dvaui, dvacore, UIFramework and PlugPlug do not
    call DwmSetWindowAttribute, and none of its top-level windows is a Chromium
    frame. The caption colors go back to DWMWA_COLOR_DEFAULT.

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

    AcquireSRWLockShared(&g_themedLock);
    std::unordered_map<HWND, BYTE> windows = g_themedWindows;
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

    if (hwnd) {
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

    if (hwnd) {
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
static HTHEME OpenDarkMenuTheme(HWND hwnd, LPCWSTR classList,
                                OpenNcThemeData_t opener) {
    if (!CurrentSettings().menuHook || !classList || !opener) {
        return nullptr;
    }

    if (_wcsicmp(classList, L"Menu") != 0) {
        return nullptr;
    }

    // Window first: it carries the DPI, and menu metrics depend on it.
    HTHEME theme = opener(hwnd, L"DarkMode::Menu");

    if (!theme && hwnd) {
        theme = opener(nullptr, L"DarkMode::Menu");
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
            repaint that control in menu colors.
        */
        ForgetMenuTheme(theme);
    }

    return theme;
}

HTHEME WINAPI OpenThemeData_Hook(HWND hwnd, LPCWSTR classList) {
    HTHEME dark = OpenDarkMenuTheme(hwnd, classList, OpenThemeData_Original);

    if (dark) {
        return TrackMenuTheme(dark, classList);
    }

    return TrackMenuTheme(OpenThemeData_Original(hwnd, classList), classList);
}

HTHEME WINAPI OpenThemeDataForDpi_Hook(HWND hwnd, LPCWSTR classList, UINT dpi) {
    if (CurrentSettings().menuHook && classList && _wcsicmp(classList, L"Menu") == 0) {
        HTHEME dark =
            OpenThemeDataForDpi_Original(hwnd, L"DarkMode::Menu", dpi);

        if (dark) {
            return TrackMenuTheme(dark, classList);
        }
    }

    return TrackMenuTheme(OpenThemeDataForDpi_Original(hwnd, classList, dpi),
                          classList);
}

// Non-client menu themes, the menu bar's among them, come through here.
HTHEME WINAPI OpenNcThemeData_Hook(HWND hwnd, LPCWSTR classList) {
    HTHEME dark = OpenDarkMenuTheme(hwnd, classList, OpenNcThemeData_Original);

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
            close(m_theme);
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
        DefWindowProc, which then paints it again on top of the fill.
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
        drawn at all — only the background is. Premiere has no MDI child
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

    HRESULT hr = drawText(theme->get(), draw->um.hdc, kMenuBarItem, 1, label,
                          static_cast<int>(itemInfo.cch), flags,
                          &draw->dis.rcItem, &opts);

    return SUCCEEDED(hr);
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
    would push it toward the darkest stop. The produced set is keyed on 8-bit
    channels, so a COLORREF compares equal to the float color it was made
    from.
*/
static COLORREF ConvertGdiColor(COLORREF color) {
    DvaColorRGBA in = GdiToDva(color);
    DvaColorRGBA out{};

    if (IsProducedColor(in) || !ConvertDvaColor(in, &out)) {
        return color;
    }

    RememberProduced(out);

    return DvaToGdi(out);
}

/*
    Cheapest test first: the color test turns most calls away on its own, so
    the module ranges are only walked for the dark grays that may be
    converted. The caller's address is read in the hook and passed in; read in
    here, it would be the hook's own address unless this was inlined.
*/
static bool ShouldConvertGdi(COLORREF color, void* caller) {
    return CurrentSettings().gdiHook && ShouldConvert(GdiToDva(color)) &&
           !InContentScope() && IsAdobeUICaller(caller);
}

using CreateSolidBrush_t = HBRUSH(WINAPI*)(COLORREF);
using CreatePen_t = HPEN(WINAPI*)(int, int, COLORREF);
using SetBkColor_t = COLORREF(WINAPI*)(HDC, COLORREF);

CreateSolidBrush_t CreateSolidBrush_Original = nullptr;
CreatePen_t CreatePen_Original = nullptr;
SetBkColor_t SetBkColor_Original = nullptr;

HBRUSH WINAPI CreateSolidBrush_Hook(COLORREF color) {
    if (ShouldConvertGdi(color, __builtin_return_address(0))) {
        color = ConvertGdiColor(color);
    }

    return CreateSolidBrush_Original(color);
}

HPEN WINAPI CreatePen_Hook(int style, int width, COLORREF color) {
    if (ShouldConvertGdi(color, __builtin_return_address(0))) {
        color = ConvertGdiColor(color);
    }

    return CreatePen_Original(style, width, color);
}

COLORREF WINAPI SetBkColor_Hook(HDC hdc, COLORREF color) {
    if (ShouldConvertGdi(color, __builtin_return_address(0))) {
        color = ConvertGdiColor(color);
    }

    return SetBkColor_Original(hdc, color);
}

// ============================================================================
// SETTINGS
// ============================================================================

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

    if (digits != 6) {
        return false;
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
    Premiere is sampled from the app's icon, which lives on hue 240; Neon and
    Glitch from reference artwork, with Neon's magenta accent lowered so it
    highlights a menu item instead of drowning its text. Glitch's acid green
    would be too bright as background, so it tints the black and tops the ramp.
    Comfy is designed for long sessions and stays away from black.

    Violet and Blossom put the hue in the ramp; Amethyst and Crimson hold it
    out, on near-black panels. Blossom is pastel only in its text and border:
    Premiere's own text passes above the ceiling, so a pastel background would
    be light on light. The warm palettes keep the panel's red channel low for
    the monitor surround, and every accent is at least 4.5:1 against its text.
    Contrast through Threshold also carry a highlight, the hue Premiere's blue
    takes; Threshold is threshold-editor.com.br's #050505 / #FFFFFF / #DC2626.
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

    {L"premiere",
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
};

/*
    A custom theme is one JSON object in a single setting, so it can be shared
    as text; the readme lists the keys. The text comes from other people, so it
    is read strictly and within fixed bounds: JSON that does not parse is
    rejected whole. Anything before the first brace or after the object, like a
    code fence, is ignored, and unknown members are skipped however nested.
*/
constexpr size_t kMaxThemeLength = 16384;
constexpr size_t kMaxThemeMembers = 64;
constexpr int kMaxThemeDepth = 16;

struct JsonCursor {
    const wchar_t* p;
    const wchar_t* end;
};

struct ThemeMember {
    std::wstring key;
    std::wstring value;
    bool isString = false;
    bool isNull = false;
};

static void SkipJsonSpace(JsonCursor& c) {
    while (c.p < c.end &&
           (*c.p == L' ' || *c.p == L'\t' || *c.p == L'\n' || *c.p == L'\r')) {
        c.p++;
    }
}

static int JsonHexDigit(wchar_t c) {
    if (c >= L'0' && c <= L'9') {
        return c - L'0';
    }

    if (c >= L'a' && c <= L'f') {
        return c - L'a' + 10;
    }

    if (c >= L'A' && c <= L'F') {
        return c - L'A' + 10;
    }

    return -1;
}

// A string at the cursor, unescaped into `out`, or only skipped when it is null.
static bool ReadJsonString(JsonCursor& c, std::wstring* out) {
    if (c.p >= c.end || *c.p != L'"') {
        return false;
    }

    c.p++;

    while (c.p < c.end) {
        wchar_t ch = *c.p++;

        if (ch == L'"') {
            return true;
        }

        if (ch < 0x20) {
            return false;  // a raw control character, which JSON does not allow
        }

        if (ch == L'\\') {
            if (c.p >= c.end) {
                return false;
            }

            wchar_t escape = *c.p++;

            switch (escape) {
                case L'"':
                case L'\\':
                case L'/':
                    ch = escape;
                    break;
                case L'b':
                    ch = L'\b';
                    break;
                case L'f':
                    ch = L'\f';
                    break;
                case L'n':
                    ch = L'\n';
                    break;
                case L'r':
                    ch = L'\r';
                    break;
                case L't':
                    ch = L'\t';
                    break;
                case L'u': {
                    if (c.end - c.p < 4) {
                        return false;
                    }

                    int code = 0;

                    for (int k = 0; k < 4; k++) {
                        int digit = JsonHexDigit(c.p[k]);

                        if (digit < 0) {
                            return false;
                        }

                        code = code * 16 + digit;
                    }

                    c.p += 4;
                    ch = static_cast<wchar_t>(code);  // UTF-16, as the setting is
                    break;
                }
                default:
                    return false;
            }
        }

        if (out) {
            out->push_back(ch);
        }
    }

    return false;  // unterminated
}

static bool SkipJsonValue(JsonCursor& c, int depth);

static bool SkipJsonWord(JsonCursor& c, const wchar_t* word) {
    size_t length = wcslen(word);

    if (static_cast<size_t>(c.end - c.p) < length || wcsncmp(c.p, word, length) != 0) {
        return false;
    }

    c.p += length;
    return true;
}

static bool SkipJsonDigits(JsonCursor& c) {
    const wchar_t* start = c.p;

    while (c.p < c.end && *c.p >= L'0' && *c.p <= L'9') {
        c.p++;
    }

    return c.p > start;
}

static bool SkipJsonNumber(JsonCursor& c) {
    if (c.p < c.end && *c.p == L'-') {
        c.p++;
    }

    if (!SkipJsonDigits(c)) {
        return false;
    }

    if (c.p < c.end && *c.p == L'.') {
        c.p++;

        if (!SkipJsonDigits(c)) {
            return false;
        }
    }

    if (c.p < c.end && (*c.p == L'e' || *c.p == L'E')) {
        c.p++;

        if (c.p < c.end && (*c.p == L'+' || *c.p == L'-')) {
            c.p++;
        }

        if (!SkipJsonDigits(c)) {
            return false;
        }
    }

    return true;
}

// An object or an array, skipped whole; `depth` bounds how far it nests.
static bool SkipJsonContainer(JsonCursor& c, int depth) {
    if (depth > kMaxThemeDepth) {
        return false;
    }

    bool object = *c.p == L'{';
    wchar_t close = object ? L'}' : L']';

    c.p++;
    SkipJsonSpace(c);

    if (c.p < c.end && *c.p == close) {
        c.p++;
        return true;
    }

    for (;;) {
        if (object) {
            if (!ReadJsonString(c, nullptr)) {
                return false;
            }

            SkipJsonSpace(c);

            if (c.p >= c.end || *c.p != L':') {
                return false;
            }

            c.p++;
        }

        if (!SkipJsonValue(c, depth + 1)) {
            return false;
        }

        SkipJsonSpace(c);

        if (c.p >= c.end) {
            return false;
        }

        if (*c.p == close) {
            c.p++;
            return true;
        }

        if (*c.p != L',') {
            return false;
        }

        c.p++;
        SkipJsonSpace(c);
    }
}

static bool SkipJsonValue(JsonCursor& c, int depth) {
    SkipJsonSpace(c);

    if (c.p >= c.end) {
        return false;
    }

    switch (*c.p) {
        case L'"':
            return ReadJsonString(c, nullptr);
        case L'{':
        case L'[':
            return SkipJsonContainer(c, depth);
        case L't':
            return SkipJsonWord(c, L"true");
        case L'f':
            return SkipJsonWord(c, L"false");
        case L'n':
            return SkipJsonWord(c, L"null");
        default:
            return SkipJsonNumber(c);
    }
}

/*
    The members of the first object in `text`. False when it does not parse,
    with `errorAt` the offset where reading stopped.
*/
static bool ReadThemeMembers(PCWSTR text, std::vector<ThemeMember>* members,
                             size_t* errorAt) {
    size_t length = wcsnlen(text, kMaxThemeLength + 1);
    *errorAt = 0;

    if (length > kMaxThemeLength) {
        *errorAt = kMaxThemeLength;
        return false;
    }

    const wchar_t* open = wcschr(text, L'{');

    if (!open) {
        return false;
    }

    JsonCursor c{open + 1, text + length};
    SkipJsonSpace(c);

    if (c.p < c.end && *c.p == L'}') {
        return true;
    }

    for (;;) {
        ThemeMember member;

        bool ok = ReadJsonString(c, &member.key);

        if (ok) {
            SkipJsonSpace(c);
            ok = c.p < c.end && *c.p == L':';
        }

        if (ok) {
            c.p++;
            SkipJsonSpace(c);

            member.isString = c.p < c.end && *c.p == L'"';
            member.isNull = c.p < c.end && *c.p == L'n';  // SkipJsonValue checks the rest
            ok = member.isString ? ReadJsonString(c, &member.value)
                                 : SkipJsonValue(c, 1);
        }

        if (ok && members->size() < kMaxThemeMembers) {
            members->push_back(std::move(member));
        }

        if (ok) {
            SkipJsonSpace(c);
            ok = c.p < c.end && (*c.p == L'}' || *c.p == L',');
        }

        if (!ok) {
            *errorAt = static_cast<size_t>(c.p - text);
            return false;
        }

        if (*c.p++ == L'}') {
            return true;
        }

        SkipJsonSpace(c);
    }
}

// The last member named `key`, as JSON readers usually take it, case aside.
static const ThemeMember* FindThemeMember(const std::vector<ThemeMember>& members,
                                          PCWSTR key) {
    const ThemeMember* found = nullptr;

    for (const ThemeMember& member : members) {
        if (_wcsicmp(member.key.c_str(), key) == 0) {
            found = &member;
        }
    }

    return found;
}

static bool ReadThemeColor(const ThemeMember* member, COLORREF* out) {
    return member && member->isString && ParseHexColor(member->value.c_str(), out);
}

/*
    The palette a custom theme describes. The six colors every surface needs
    fall back to Onyx's one at a time, each logged; the optional ones have
    defaults of their own.
*/
static Palette LoadCustomTheme(const Palette& onyx) {
    auto json = WindhawkUtils::StringSetting::make(L"customTheme");
    PCWSTR text = json;

    std::vector<ThemeMember> members;
    size_t errorAt = 0;

    if (!*text) {
        Wh_Log(L"custom theme is empty; Onyx is used instead");
        return onyx;
    }

    if (!ReadThemeMembers(text, &members, &errorAt)) {
        bool curly = wcschr(text, L'“') || wcschr(text, L'”');

        Wh_Log(L"custom theme is not valid JSON (it stops making sense at "
               L"character %u)%s; Onyx is used instead",
               static_cast<unsigned>(errorAt + 1),
               curly ? L", and it has curly quotes where straight ones belong" : L"");
        return onyx;
    }

    Palette p = onyx;

    const struct {
        PCWSTR key;
        COLORREF* color;
    } kRequired[] = {
        {L"base", &p.ramp[0]},   {L"panel", &p.ramp[1]},  {L"surface", &p.ramp[2]},
        {L"raised", &p.ramp[3]}, {L"border", &p.ramp[4]}, {L"text", &p.text},
    };

    for (const auto& field : kRequired) {
        if (!ReadThemeColor(FindThemeMember(members, field.key), field.color)) {
            Wh_Log(L"custom theme: \"%s\" is missing or not a #RRGGBB color; "
                   L"Onyx's is used",
                   field.key);
        }
    }

    /*
        Absent, empty or null takes the default without a word — the shipped
        template lists every key, with "highlight": "" — while a value that is
        there but unreadable is worth a line.
    */
    auto optional = [&](PCWSTR key, COLORREF* color) {
        const ThemeMember* member = FindThemeMember(members, key);

        if (!member || member->isNull || (member->isString && member->value.empty())) {
            return false;
        }

        if (ReadThemeColor(member, color)) {
            return true;
        }

        Wh_Log(L"custom theme: \"%s\" is not a #RRGGBB color; left out", key);
        return false;
    };

    if (!optional(L"accent", &p.accent)) {
        p.accent = p.ramp[4];
    }

    /*
        Halfway from the text to the panel is close to where the built-in
        palettes put disabled text, and it always lies between the two, which
        Onyx's #777777 would not once a theme's text is darker than that.
    */
    if (!optional(L"disabledText", &p.dimText)) {
        p.dimText = RGB((GetRValue(p.text) + GetRValue(p.ramp[1])) / 2,
                        (GetGValue(p.text) + GetGValue(p.ramp[1])) / 2,
                        (GetBValue(p.text) + GetBValue(p.ramp[1])) / 2);
    }

    if (!optional(L"highlight", &p.highlight)) {
        p.highlight = CLR_INVALID;
    }

    const ThemeMember* name = FindThemeMember(members, L"name");
    const ThemeMember* author = FindThemeMember(members, L"author");

    if (name && name->isString && !name->value.empty()) {
        bool by = author && author->isString && !author->value.empty();

        Wh_Log(L"custom theme: %.64s%s%.64s", name->value.c_str(),
               by ? L", by " : L"", by ? author->value.c_str() : L"");
    }

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
    and UIFramework tables stay on the raw form: they are type-erased on
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
        Every Windows hook is installed whatever the settings say, and each one
        checks its own setting every time it runs — so one whose setting is off
        only forwards the call it received. Installing them all is what lets
        Wh_ModSettingsChanged apply a change without reloading the mod.
        Premiere's own modules are the exception; see HookLoadedModules.
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
        there is no way to know which one Premiere uses — and hooking the one it does
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
        Wh_Log(L"could not resolve kernelbase!LoadLibraryExW; a Premiere that "
               L"loads its UI modules after this point will not be themed");
    }

    // The UXP runtime reads the panels' stylesheets through these two.
    FindUxpPluginsDir();

    if (kernelBase) {
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
    calls DrawMenuBar, which schedules a WM_NCPAINT that Premiere's UI thread is
    free to process before that happens — the bar would paint light, and nothing
    would ask it to paint again.
*/
void Wh_ModAfterInit() {
    ApplyThemeToExistingWindows();
}

void Wh_ModUninit() {
    /*
        Hooks are already removed by the time this runs, so the calls below go
        straight to the system and the windows come back with their own colors.
    */

    // The notification callback lives in this image, which is about to go.
    StopWatchingModuleLoads();

    // Premiere's references into the table get their original colors back.
    // The counts are logged to judge the table size against real sessions,
    // and to tell a highlight that reached the screen from one never asked for.
    size_t used = RecomputeColorTable(true);

    Wh_Log(L"color table: %u of %u slots in use; %d of %u interface blues "
           L"recolored",
           static_cast<unsigned>(used), static_cast<unsigned>(kSlotCount),
           std::popcount(static_cast<uint64_t>(g_bluesRecolored)),
           static_cast<unsigned>(kInterfaceBlueCount));

    // Nothing to hand back there: Premiere parsed those stylesheets already.
    if (g_stylesheetsRecolored) {
        Wh_Log(L"%ld UXP stylesheets were recolored this session; those panels "
               L"keep the palette until Premiere restarts",
               g_stylesheetsRecolored);
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

    // The g_sysBrushes brushes are never deleted; see the comment above them.

    if (g_uxtheme) {
        FreeLibrary(g_uxtheme);
        g_uxtheme = nullptr;
    }
}

/*
    Settings are applied in place; the mod is not reloaded.

    A reload would leave the color table and the system brushes behind once
    per change, and trying out palettes changes settings many times in a row.
    It would also do less: the colors Premiere took before the change would
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

    // A setting that works through Premiere's modules may just have come on.
    if (HookLoadedModules() && !Wh_ApplyHookOperations()) {
        Wh_Log(L"failed to apply hooks after a settings change");
    }

    RecomputeColorTable(false);

    ApplyAppMode(CurrentSettings().nativeDarkMode);

    if (previous.nativeDarkMode && !CurrentSettings().nativeDarkMode) {
        RevertThemedWindows();
    } else if (!previous.nativeDarkMode && CurrentSettings().nativeDarkMode) {
        ApplyThemeToExistingWindows();
    } else if (CurrentSettings().nativeDarkMode &&
               memcmp(&previous.palette, &CurrentSettings().palette, sizeof(Palette)) !=
                   0) {
        RecolorThemedFrames();
    }

    // ApplyAppMode flushes these itself when the mode changes.
    if (previous.menuHook != CurrentSettings().menuHook && g_FlushMenuThemes) {
        g_FlushMenuThemes();
    }

    RedrawProcessWindows();
}
