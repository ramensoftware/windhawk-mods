// ==WindhawkMod==
// @id              premiere-pro-theme
// @name            Premiere Pro Theme
// @description     Recolors the entire Adobe Premiere Pro interface — panels, timeline, video monitor, window frame and menu bar — with a choice of very dark palettes.
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
and does it consistently across parts of the app that are painted by four
different mechanisms.

## How it finds the colors

Premiere's interface is painted with the **Adobe Spectrum** gray ramp —
`#1D1D1D`, `#262626`, `#303030`, `#4B4B4B` — served by `dvaui.dll`, Adobe's UI
toolkit. The mod intercepts the functions that hand out those colors and returns
a darkened version instead.

Three families, all in `dvaui.dll`:

- **Theme**: `Theme::GetColor`, `ThemeClient::GetColor`, `ui::GetColor`
- **Spectrum**: `ui::GetGrayColor`, `Theme::GetGrayColor`,
  `GetSpectrumGrayColor`, `GetSpectrumColor` — this family paints most panels
- **DNA / skins**: application, content and list backgrounds, dividers,
  scrollbars

Not every Premiere version exports all of them. The mod installs what it finds
and logs the rest.

## Parts that never ask the theme

Some surfaces are painted directly, without consulting the theme at all — the
video monitor backdrop is the clearest case. With a tinted palette active and
57% of the window already recolored, it stayed a neutral `#0D0D0D` across 13% of
the screen.

There is no color function to intercept for them. What there is, is the places
every solid fill passes through no matter who picked the color:

```
dvaui::drawbot::d2d::OSSupplier::NewBrush(const ColorRGBA&)
UIF::DC::FillRect / UIF::DC::FrameRect      [UIFramework.dll]
```

The Direct2D brush factory, and Premiere's own drawing layer. Hooking them
catches whatever the theme functions never hand out. The SVG brush is
deliberately left alone — it paints icons, and a darkened icon disappears.

## The menu bar

The `File / Edit / Clip` bar and the menus that drop from it look like one
thing, but Windows paints them through two different paths.

**Dropdown menus** get their theme from `OpenNcThemeData`, uxtheme ordinal 49,
used for non-client area and absent from every header. Asking it for the `Menu`
class only returns the dark theme *after* `SetPreferredAppMode` and
`FlushMenuThemes` have run — an ordering dependency that fails when the app
already opened and cached the light one. Asking for `DarkMode::Menu` depends on
no ordering at all:

```
Windows app mode = light, no SetPreferredAppMode call:
  OpenNcThemeData(nullptr, L"Menu")           -> text #000000  (light theme)
  OpenNcThemeData(nullptr, L"DarkMode::Menu") -> text #FFFFFF  (dark theme)
```

**The bar itself** is not themed content and no theme will recolor it. It is
non-client area, drawn by `DefWindowProc` / `DefFrameProc` in response to two
undocumented messages — `WM_UAHDRAWMENU` (0x91) and `WM_UAHDRAWMENUITEM` (0x92).
The mod intercepts those four functions and paints the strip and each item into
the same DCs and rectangles Windows would have used, plus the 1px line below it
that arrives through no message at all.

Technique from [win32-darkmode](https://github.com/adzm/win32-darkmode), branch
`darkmenubar`.

> **If your menu bar ignores the palette**, another mod is painting it first.
> Any mod that darkens Win32 menus globally hooks the same `DefWindowProc` and
> answers before this one, with its own fixed color. Add
> `Adobe Premiere Pro.exe` to that mod's process **exclusion** list and this one
> takes over.

## Palettes

Neutral:

| Level   | Onyx      | Abyss     | Graphite  |
|---------|-----------|-----------|-----------|
| Base    | `#050505` | `#000000` | `#0D0D0D` |
| Panel   | `#090909` | `#040404` | `#141414` |
| Surface | `#0E0E0E` | `#080808` | `#1A1A1A` |
| Raised  | `#161616` | `#101010` | `#232323` |
| Border  | `#242424` | `#1C1C1C` | `#303030` |

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

**Premiere** was sampled from the app's own executable icon: of 728 opaque
pixels, 72% are `#00005B` and 16% are `#9999FF`, all on hue 240.

**Neon** and **Glitch** were sampled from reference artwork — the first
near-black with magenta, the second acid green with magenta. In Glitch the green
never becomes background: 70% of the screen in `#C3E600` would be far too bright
to judge an image on, which is the job. It becomes the bias of the black and the
top of the ramp instead, and the magenta becomes the accent.

**Comfy** is the only one designed rather than sampled. Warm brown, lower
contrast, and deliberately the lightest of the set — the point is to sit in it
for hours.

## The program monitor surround

Around the picture in the Program and Source monitors, visible when the monitor
is zoomed out, there is a band that stays neutral gray whichever palette is
active. **With the near-black palettes you will not see it** — a neutral band
beside a near-black panel is the same color. With a tinted palette it is
visible, and it is a documented limitation rather than a bug; the reason, and
what was measured, is under *Known limitations* below.

The black *inside* the sequence frame is a different thing: that is the rendered
picture, black wherever the sequence has no media, and it stays black in every
palette. It is the image you are judging, not chrome.

## The accent

The accent is not part of the ramp. The ramp is interpolated to paint
background, so a strong color placed in it bleeds into neighbouring tones and
tints half the interface. The accent appears only on hovered menu items and
system highlights — which is what makes a purple theme look purple without
painting the timeline purple.

## What is never touched

Saturated colors — selection blue, timeline clips, labels, warnings — pass
through untouched. So does anything above the **brightness ceiling**: text,
icons, and the `#4B4B4B` Spectrum uses for disabled text. The default ceiling
(28) sits just below that gray on purpose.

## Known limitations

Two surfaces are not themed. Both are written down here rather than worked
around, because the workaround available for each is worse than the limitation.

### The Home screen

The startup screen is rendered by **UXP**, Adobe's newer runtime
(`dvauxphost.dll` / `dvauxpui.dll`), which styles itself with its own CSS rather
than through the `dvaui` color functions everything else goes through.
`dvauxphost` exports exactly two theme entry points —
`SetUIThemeInfoAPIObject(const UIThemeInfo&)` and `UpdateTheme(int)` — and
neither hands out a color. Reaching it would mean guessing the layout of an
undocumented struct, in a process holding unsaved work.

Premiere's own preferences can open the most recent project directly instead of
the Home screen, which takes it out of the way.

### The band around the video, with a tinted palette

Premiere strips the color out of that surround on purpose, and the arithmetic
gives it away. With the Glitch palette the panels measured `#101907` —
(16, 25, 7) — and the surround measured `#101010`; the average of 16, 25 and 7
is 16. An earlier measurement of the same pair: panels `#0D1405` = (13, 20, 5),
surround `#0D0D0D`, and (13 + 20 + 5) / 3 = 13. Twice, exactly. The surround is
the channel average of the panel color.

That is Adobe being careful, not Adobe being wrong: a tinted surface next to the
picture biases how you read the picture, so it is forced neutral. It also
explains why the near-black palettes look right — averaging a gray returns the
same gray, so the step is invisible until the palette has a hue.

Where it is painted has not been found. A diagnostic build recorded every
distinct color arriving at nine separate entry points — the theme functions, the
Direct2D brush and pen factories, `UIF::DC::FillRect` and `FrameRect` in both
their color overloads, the background-erase path, and the GPU fill in
`GPUFoundation.dll` — 181 distinct (entry point, color) pairs, and the gray
appeared at **none** of them. `UIF::DC` has a third `FillRect` that takes a
named decal and no color at all, which would explain the absence; confirming
that means reading an undocumented skin table, and guessing at one inside an
editor holding unsaved work is not a trade worth making for a band.

So it stays neutral, and this readme says so instead of the mod patching blind.
If you use a tinted palette and the band bothers you, the near-black palettes do
not have it.

## Compatibility

The mod does not assume a Premiere version. It looks up each color function by
name at startup, installs the ones the running build exports, and logs the rest
— so a version that moved or dropped a function loses that surface, not the mod.

Measured against the export tables of the installed builds:

| Premiere | Color functions found | Drawing primitives |
|----------|-----------------------|--------------------|
| 2026     | 31 of 31              | 4 of 4             |
| 2023     | 27 of 31              | 4 of 4             |

Between those two Adobe swapped the smart pointer that half of the color
functions take — `boost::intrusive_ptr` for its own `IntrusivePtr` — which
changes the mangled name and nothing else. Both spellings are in the table, so
each version finds its own and skips the other.

The four missing on 2023 (`dna::GetSpectrumColor`, `GrayBackgroundColor`,
`GrayBorderColor`, `utils::DrawFillRect`) do not exist there under any
signature. Their surfaces fall back to the Spectrum family, which is present in
both.

The window frame, the menu bar and the native dialogs do not depend on Premiere
at all — they are Windows, and they work on any version. Native dark mode needs
Windows 10 build 17763 or newer; below that the mod still themes the interface
and paints the menus itself.

## Credits

The menu bar technique — the two undocumented `WM_UAHDRAWMENU` messages and the
`DarkMode::Menu` theme class — comes from
[win32-darkmode](https://github.com/adzm/win32-darkmode) by adzm, MIT licensed.
This mod is MIT as well.

## If something becomes unreadable

Lower the ceiling. Raising it darkens more, starting with disabled text — which
is exactly what disappears first.

Changing a setting reloads the mod, but Premiere only repaints everything on
restart.
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
  - custom: Custom (uses the fields below)
- customBase: "050505"
  $name: Custom — base
  $description: Hex RGB without "#". Deepest background. Only used by the Custom palette.
- customPanel: "090909"
  $name: Custom — panel
- customSurface: "0E0E0E"
  $name: Custom — surface
- customElevated: "161616"
  $name: Custom — raised
- customBorder: "242424"
  $name: Custom — border
- customText: "E6E6E6"
  $name: Custom — text
  $description: Menu and title bar text.
- customAccent: "2E2E2E"
  $name: Custom — accent
  $description: Hovered menu items and system highlights. The one place a strong color fits without tinting everything else.
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
  $description: Darkens brushes and pens created by Premiere and by dvaui.dll.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <dwmapi.h>
#include <uxtheme.h>

#include <stdint.h>
#include <cmath>
#include <cstring>
#include <cwchar>
#include <utility>

// ============================================================================
// PALETTE AND SETTINGS
// ============================================================================

struct Palette {
    /*
        The five tones, deepest to lightest, in ramp order.

        ramp[0] is the darkest stop of the INTERPOLATION, not the window frame color.
        The title bar and the menu bar use ramp[1], the same tone as the panels — in
        any tinted palette the darkest tone reads as black, and a black frame around a
        purple interface does not look like a theme, it looks like a frame that missed
        the theme.
    */
    COLORREF ramp[5];
    COLORREF text;
    COLORREF dimText;

    /*
        The accent sits OUTSIDE the ramp on purpose.

        The ramp is interpolated to paint background, so any color placed in it bleeds
        into the neighbouring tones and would tint half the interface. Here it appears
        only where it is genuinely a highlight: hovered menu items and system
        highlights. That is what makes a purple theme look purple without painting the
        timeline purple.
    */
    COLORREF accent;
};

struct Settings {
    Palette palette;
    float strength;  // 0.0 .. 1.0
    float ceiling;   // 0.0 .. 1.0
    bool dvauiHook;
    bool brushHook;
    bool nativeDarkMode;
    bool menuHook;
    bool gdiHook;
};

Settings g_settings;

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
    return original * (1.0f - g_settings.strength) + target * g_settings.strength;
}

// ============================================================================
// WHERE THE CALL CAME FROM
// ============================================================================

/*
    The return address is passed in as a parameter, never read inside this
    function.

    Reading __builtin_return_address(0) in here would only give the real caller if
    the compiler had inlined the function — otherwise it gives an address inside
    the mod itself, and the filter lets everything through. Depending on an
    optimization decision for correctness is a bug that shows up when the clang
    version changes.
*/
/*
    Which modules count as Adobe UI, kept as address ranges rather than asked
    per call.

    The GDI hooks ask this on every CreateSolidBrush, CreatePen and SetBkColor.
    Asking it with GetModuleHandleExW took the loader lock each time, which is
    more expensive than the colour work it guards and, worse, a lock-order
    hazard: ThemeSysBrush holds g_brushLock while calling CreateSolidBrush, so
    that path takes g_brushLock and then the loader lock, while a thread already
    inside the loader calling GetSysColorBrush takes the two the other way
    round.

    The set only changes when a module loads, and LoadLibraryExW_Hook already
    sees that happen. So it is snapshot once and appended to, and the question
    becomes a pointer comparison against a handful of ranges.
*/
struct ModuleRange {
    uintptr_t begin;
    uintptr_t end;
};

constexpr size_t kMaxModuleRanges = 64;

ModuleRange g_moduleRanges[kMaxModuleRanges];
volatile LONG g_moduleRangeCount = 0;

static bool IsAdobeUIModule(HMODULE module) {
    if (module == GetModuleHandleW(nullptr)) {
        return true;
    }

    wchar_t path[MAX_PATH]{};

    if (!GetModuleFileNameW(module, path, ARRAYSIZE(path))) {
        return false;
    }

    const wchar_t* name = wcsrchr(path, L'\\');
    name = name ? name + 1 : path;

    /*
        Adobe's whole UI toolkit is prefixed dva (dvaui, dvacore, ...).
        UIFramework is Premiere's own drawing layer, hooked elsewhere as a
        first-class paint layer, so it belongs in the same set.
    */
    return _wcsnicmp(name, L"dva", 3) == 0 ||
           _wcsicmp(name, L"UIFramework.dll") == 0;
}

/*
    Only ever called from Wh_ModInit, before any hook is live, and from
    LoadLibraryExW_Hook, which runs under the loader lock and is therefore
    serialised against itself. That is why the append needs no lock of its own;
    the count is published last so a concurrent reader never sees a half-written
    entry.
*/
static void NoteAdobeModule(HMODULE module) {
    if (!module || !IsAdobeUIModule(module)) {
        return;
    }

    auto base = reinterpret_cast<uintptr_t>(module);
    auto dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(module);

    if (dos->e_magic != IMAGE_DOS_SIGNATURE) {
        return;
    }

    auto nt = reinterpret_cast<const IMAGE_NT_HEADERS*>(base + dos->e_lfanew);

    if (nt->Signature != IMAGE_NT_SIGNATURE) {
        return;
    }

    LONG count = g_moduleRangeCount;

    for (LONG i = 0; i < count; i++) {
        if (g_moduleRanges[i].begin == base) {
            return;
        }
    }

    if (count >= static_cast<LONG>(kMaxModuleRanges)) {
        return;
    }

    g_moduleRanges[count].begin = base;
    g_moduleRanges[count].end = base + nt->OptionalHeader.SizeOfImage;

    InterlockedExchange(&g_moduleRangeCount, count + 1);
}

using EnumProcessModules_t = BOOL(WINAPI*)(HANDLE, HMODULE*, DWORD, LPDWORD);

static void SnapshotAdobeModules() {
    NoteAdobeModule(GetModuleHandleW(nullptr));

    HMODULE kernel32 = GetModuleHandleW(L"kernel32.dll");

    auto enumModules =
        kernel32 ? reinterpret_cast<EnumProcessModules_t>(
                       GetProcAddress(kernel32, "K32EnumProcessModules"))
                 : nullptr;

    if (!enumModules) {
        // Named fallback, so the common case still works without psapi.
        NoteAdobeModule(GetModuleHandleW(L"dvaui.dll"));
        NoteAdobeModule(GetModuleHandleW(L"dvacore.dll"));
        NoteAdobeModule(GetModuleHandleW(L"UIFramework.dll"));
        return;
    }

    HMODULE modules[512];
    DWORD needed = 0;

    if (!enumModules(GetCurrentProcess(), modules, sizeof(modules), &needed)) {
        return;
    }

    DWORD count = needed / sizeof(HMODULE);

    if (count > ARRAYSIZE(modules)) {
        count = ARRAYSIZE(modules);
    }

    for (DWORD i = 0; i < count; i++) {
        NoteAdobeModule(modules[i]);
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
    The ramp is interpolated, not stepped.

    Premiere's interface is painted with the Adobe Spectrum gray scale (#1D1D1D,
    #262626, #303030, #4B4B4B ...), and every step of that scale carries
    information: it is how you tell a panel from the panel behind it, a header
    strip from the body. Mapping brightness bands onto fixed tones would collapse
    neighbouring steps into the same value and erase that depth — the interface
    would end up dark and flat.

    Interpolating preserves the order: whatever was one step lighter stays one step
    lighter, just inside a much darker range.
*/
static COLORREF PickTarget(float brightness) {
    const COLORREF* ramp = g_settings.palette.ramp;

    float t = ClampFloat(brightness / g_settings.ceiling, 0.0f, 1.0f);

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
    This validation exists because the layout of dvaui::drawbot::ColorRGBA is an
    assumption — four floats in RGBA order. If a future Premiere changes the
    struct, the values read here stop looking like a color, and the mod would
    rather return the original than paint garbage on screen.
*/
static bool ShouldConvert(const DvaColorRGBA& in) {
    if (!IsSaneChannel(in.r) || !IsSaneChannel(in.g) || !IsSaneChannel(in.b) ||
        !IsSaneChannel(in.a)) {
        return false;
    }

    if (!IsNeutral(in.r, in.g, in.b, 0.035f)) {
        return false;
    }

    return (in.r + in.g + in.b) / 3.0f <= g_settings.ceiling;
}

static bool ConvertDvaColor(const DvaColorRGBA& in, DvaColorRGBA* out) {
    if (!ShouldConvert(in)) {
        return false;
    }

    float brightness = (in.r + in.g + in.b) / 3.0f;

    COLORREF target = PickTarget(brightness);

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
    All of these functions return `const ColorRGBA&` — a reference to a color that
    belongs to the theme and lives as long as the theme does, and Adobe's code may
    hold on to it. So the conversion has to return a stable reference too, not one
    into a reused buffer: with a single buffer, `Draw(GetColor(A), GetColor(B))`
    would make both references point at the same place and both colors would come
    out identical.

    The slot key is the ADDRESS of the original color, not an id.

    That is what allows dozens of functions with different signatures to be covered
    — `GetGrayColor(enum)`, `GetColor(ImmutableString)`, `GrayBackgroundColor()` —
    without understanding any parameter type. The address identifies the color
    better than any key could, because it is the color.

    And if an address is ever reused for a different color, comparing against `src`
    notices and recomputes. The table corrects itself.
*/

constexpr size_t kSlotCount = 8192;  // power of two, so the mask is a single &

struct ColorSlot {
    volatile LONG state;  // 0 free, 1 being filled, 2 ready
    uintptr_t key;
    DvaColorRGBA src;
    DvaColorRGBA dst;
};

/*
    Deliberately heap-allocated and deliberately leaked.

    The pointer this table returns is handed to Premiere and kept by it — the
    comments on ConvertColorRef and StableConvert spell out that contract. A
    static array would live in the mod image, and Windhawk unmaps that image on
    unload, which happens on every settings change and not only on disable. The
    references Premiere still holds would then point at unmapped memory.

    One table of about 400 KB leaks per load cycle. That is the price of handing
    out a pointer whose lifetime the mod does not control, and it is the same
    trade already made for the GetSysColorBrush brushes.
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

static const DvaColorRGBA* StoreColor(uintptr_t key, const DvaColorRGBA& src,
                                      const DvaColorRGBA& dst) {
    if (!g_slots) {
        return nullptr;
    }

    size_t index = static_cast<size_t>(((key >> 4) * 0x9E3779B97F4A7C15ull) >> 51) &
                   (kSlotCount - 1);

    for (size_t probe = 0; probe < 64; probe++) {
        ColorSlot& slot = g_slots[(index + probe) & (kSlotCount - 1)];

        LONG state = InterlockedCompareExchange(&slot.state, 0, 0);

        if (state == 0) {
            if (InterlockedCompareExchange(&slot.state, 1, 0) == 0) {
                slot.key = key;
                slot.src = src;
                slot.dst = dst;

                InterlockedExchange(&slot.state, 2);

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

        if (!SameColor(slot.src, src)) {
            // The address was reused, or the theme changed.
            slot.src = src;
            slot.dst = dst;
        }

        return &slot.dst;
    }

    return nullptr;  // table saturated: the original color beats a wrong one
}

/*
    Second line of defense against double conversion, BY VALUE.

    A color that came out of a theme function has already been converted — and
    arrives again when it is time to build a brush. Converting twice pushes
    everything toward the darkest stop of the ramp and flattens the interface.

    On the reference path this is settled by address (IsOurSlot), but the caller may
    have copied the color into a temporary before asking for the brush, and then the
    address says nothing. So every color we produce also goes into this set, and a
    color found in it passes through untouched.

    The key is the RGB quantized to 8 bits rather than the raw floats: the
    comparison has to survive a round trip through a format conversion.
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
    return ((static_cast<uint32_t>(key) * 2654435761u) >> 19) &
           (kProducedSlots - 1);
}

static void RememberProduced(const DvaColorRGBA& c) {
    LONG key = PackColorKey(c);
    size_t start = ProducedIndex(key);

    for (size_t probe = 0; probe < 32; probe++) {
        size_t i = (start + probe) & (kProducedSlots - 1);
        LONG cur = InterlockedCompareExchange(&g_produced[i], key, 0);

        if (cur == 0 || cur == key) {
            return;
        }
    }
}

/*
    Plain read, no interlocked operation.

    Using InterlockedCompareExchange just to READ costs a `lock cmpxchg`: tens of
    cycles, plus the cache line marked dirty for every other core. In a function
    called once per screen fill that shows up. An aligned LONG is already read
    atomically on x86-64; the interlocked operation is only needed to WRITE, which
    happens once per new color.
*/
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
    if (!original || !g_settings.dvauiHook || IsOurSlot(original)) {
        return original;
    }

    DvaColorRGBA converted{};

    if (!ConvertDvaColor(*original, &converted)) {
        return original;
    }

    RememberProduced(converted);

    const DvaColorRGBA* stored =
        StoreColor(reinterpret_cast<uintptr_t>(original), *original, converted);

    return stored ? stored : original;
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
};

/*
    Not every Premiere version exports everything: 2023 has no `dna` family, which
    only appears in the versions with the newer Spectrum. Whatever is missing is
    logged and the rest carries on.
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
     L"GetApplicationBackgroundColor"},
    {"?GetContentBackgroundColor@utilities@skins@dvaui@@YAAEBVColorRGBA@drawbot@"
     "3@PEBVThemeProvider@ui@3@V?$IntrusivePtr@VSkinSet@core@skins@dvaui@@"
     "@RefCountedInterface@utility@dvacore@@@Z",
     L"GetContentBackgroundColor"},
    {"?GetListBoxBackgroundColor@utilities@skins@dvaui@@YAAEBVColorRGBA@drawbot@"
     "3@PEBVThemeProvider@ui@3@V?$IntrusivePtr@VSkinSet@core@skins@dvaui@@"
     "@RefCountedInterface@utility@dvacore@@@Z",
     L"GetListBoxBackgroundColor"},
    {"?GetHoverBackgroundColor@utilities@skins@dvaui@@YAAEBVColorRGBA@drawbot@3@"
     "PEBVThemeProvider@ui@3@V?$IntrusivePtr@VSkinSet@core@skins@dvaui@@"
     "@RefCountedInterface@utility@dvacore@@@Z",
     L"GetHoverBackgroundColor"},
    {"?GetTabBackgroundColor@utilities@skins@dvaui@@YAAEBVColorRGBA@drawbot@3@"
     "PEBVThemeProvider@ui@3@V?$IntrusivePtr@VSkinSet@core@skins@dvaui@@"
     "@RefCountedInterface@utility@dvacore@@@Z",
     L"GetTabBackgroundColor"},
    {"?GetThumbnailBackgroundColor@utilities@skins@dvaui@@YAAEBVColorRGBA@"
     "drawbot@3@PEBVThemeProvider@ui@3@V?$IntrusivePtr@VSkinSet@core@skins@dvaui@"
     "@@RefCountedInterface@utility@dvacore@@@Z",
     L"GetThumbnailBackgroundColor"},

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
     L"GetDefaultControlColor"},
    {"?GetInteractiveControlColor@utilities@skins@dvaui@@YAAEBVColorRGBA@drawbot@"
     "3@PEBVThemeProvider@ui@3@V?$IntrusivePtr@VSkinSet@core@skins@dvaui@@"
     "@RefCountedInterface@utility@dvacore@@@Z",
     L"GetInteractiveControlColor"},
    {"?GetDividerColor@utilities@skins@dvaui@@YAAEBVColorRGBA@drawbot@3@"
     "PEBVThemeProvider@ui@3@V?$IntrusivePtr@VSkinSet@core@skins@dvaui@@"
     "@RefCountedInterface@utility@dvacore@@@Z",
     L"GetDividerColor"},
    {"?GetListBoxBorderColor@utilities@skins@dvaui@@YAAEBVColorRGBA@drawbot@3@"
     "PEBVThemeProvider@ui@3@V?$IntrusivePtr@VSkinSet@core@skins@dvaui@@"
     "@RefCountedInterface@utility@dvacore@@@Z",
     L"GetListBoxBorderColor"},
    {"?GetFieldBorderColor@utilities@skins@dvaui@@YAAEBVColorRGBA@drawbot@3@"
     "PEBVThemeProvider@ui@3@V?$IntrusivePtr@VSkinSet@core@skins@dvaui@@"
     "@RefCountedInterface@utility@dvacore@@@Z",
     L"GetFieldBorderColor"},
    {"?GetScrollBarThumbColor@utilities@skins@dvaui@@YAAEBVColorRGBA@drawbot@3@"
     "PEBVThemeProvider@ui@3@V?$IntrusivePtr@VSkinSet@core@skins@dvaui@@"
     "@RefCountedInterface@utility@dvacore@@_N@Z",
     L"GetScrollBarThumbColor"},
    {"?GetScrollBarTrackColor@utilities@skins@dvaui@@YAAEBVColorRGBA@drawbot@3@"
     "PEBVThemeProvider@ui@3@V?$IntrusivePtr@VSkinSet@core@skins@dvaui@@"
     "@RefCountedInterface@utility@dvacore@@_N@Z",
     L"GetScrollBarTrackColor"},
    {"?GetWidgetColor@utilities@skins@dvaui@@YAAEBVColorRGBA@drawbot@3@"
     "PEBVThemeProvider@ui@3@V?$IntrusivePtr@VSkinSet@core@skins@dvaui@@"
     "@RefCountedInterface@utility@dvacore@@_N@Z",
     L"GetWidgetColor"},

    /*
        The same functions as above, under the name they carry before
        Premiere 2026.

        Adobe swapped the smart pointer these take — boost::intrusive_ptr for
        dvacore's own IntrusivePtr — which changes the mangled name and nothing
        else: same arguments, same count, same convention. Carrying only the
        newer spelling meant sixteen of them silently missed on Premiere 2023.

        Whichever spelling the running version does not have is simply absent,
        logged, and skipped, so both can sit in the table at once.
    */
    {"?GetApplicationBackgroundColor@utilities@skins@dvaui@@YAAEBVColorRGBA@"
     "drawbot@3@PEBVThemeProvider@ui@3@V?$intrusive_ptr@VSkinSet@core@skins@"
     "dvaui@@@boost@@@Z",
     L"GetApplicationBackgroundColor (pre-2026)"},
    {"?GetContentBackgroundColor@utilities@skins@dvaui@@YAAEBVColorRGBA@draw"
     "bot@3@PEBVThemeProvider@ui@3@V?$intrusive_ptr@VSkinSet@core@skins@dvau"
     "i@@@boost@@@Z",
     L"GetContentBackgroundColor (pre-2026)"},
    {"?GetListBoxBackgroundColor@utilities@skins@dvaui@@YAAEBVColorRGBA@draw"
     "bot@3@PEBVThemeProvider@ui@3@V?$intrusive_ptr@VSkinSet@core@skins@dvau"
     "i@@@boost@@@Z",
     L"GetListBoxBackgroundColor (pre-2026)"},
    {"?GetHoverBackgroundColor@utilities@skins@dvaui@@YAAEBVColorRGBA@drawbo"
     "t@3@PEBVThemeProvider@ui@3@V?$intrusive_ptr@VSkinSet@core@skins@dvaui@"
     "@@boost@@@Z",
     L"GetHoverBackgroundColor (pre-2026)"},
    {"?GetTabBackgroundColor@utilities@skins@dvaui@@YAAEBVColorRGBA@drawbot@"
     "3@PEBVThemeProvider@ui@3@V?$intrusive_ptr@VSkinSet@core@skins@dvaui@@@"
     "boost@@@Z",
     L"GetTabBackgroundColor (pre-2026)"},
    {"?GetThumbnailBackgroundColor@utilities@skins@dvaui@@YAAEBVColorRGBA@dr"
     "awbot@3@PEBVThemeProvider@ui@3@V?$intrusive_ptr@VSkinSet@core@skins@dv"
     "aui@@@boost@@@Z",
     L"GetThumbnailBackgroundColor (pre-2026)"},
    {"?GetDefaultControlColor@utilities@skins@dvaui@@YAAEBVColorRGBA@drawbot"
     "@3@PEBVThemeProvider@ui@3@V?$intrusive_ptr@VSkinSet@core@skins@dvaui@@"
     "@boost@@_N@Z",
     L"GetDefaultControlColor (pre-2026)"},
    {"?GetInteractiveControlColor@utilities@skins@dvaui@@YAAEBVColorRGBA@dra"
     "wbot@3@PEBVThemeProvider@ui@3@V?$intrusive_ptr@VSkinSet@core@skins@dva"
     "ui@@@boost@@@Z",
     L"GetInteractiveControlColor (pre-2026)"},
    {"?GetDividerColor@utilities@skins@dvaui@@YAAEBVColorRGBA@drawbot@3@PEBV"
     "ThemeProvider@ui@3@V?$intrusive_ptr@VSkinSet@core@skins@dvaui@@@boost@"
     "@@Z",
     L"GetDividerColor (pre-2026)"},
    {"?GetListBoxBorderColor@utilities@skins@dvaui@@YAAEBVColorRGBA@drawbot@"
     "3@PEBVThemeProvider@ui@3@V?$intrusive_ptr@VSkinSet@core@skins@dvaui@@@"
     "boost@@@Z",
     L"GetListBoxBorderColor (pre-2026)"},
    {"?GetFieldBorderColor@utilities@skins@dvaui@@YAAEBVColorRGBA@drawbot@3@"
     "PEBVThemeProvider@ui@3@V?$intrusive_ptr@VSkinSet@core@skins@dvaui@@@bo"
     "ost@@@Z",
     L"GetFieldBorderColor (pre-2026)"},
    {"?GetScrollBarThumbColor@utilities@skins@dvaui@@YAAEBVColorRGBA@drawbot"
     "@3@PEBVThemeProvider@ui@3@V?$intrusive_ptr@VSkinSet@core@skins@dvaui@@"
     "@boost@@_N@Z",
     L"GetScrollBarThumbColor (pre-2026)"},
    {"?GetScrollBarTrackColor@utilities@skins@dvaui@@YAAEBVColorRGBA@drawbot"
     "@3@PEBVThemeProvider@ui@3@V?$intrusive_ptr@VSkinSet@core@skins@dvaui@@"
     "@boost@@_N@Z",
     L"GetScrollBarTrackColor (pre-2026)"},
    {"?GetWidgetColor@utilities@skins@dvaui@@YAAEBVColorRGBA@drawbot@3@PEBVT"
     "hemeProvider@ui@3@V?$intrusive_ptr@VSkinSet@core@skins@dvaui@@@boost@@"
     "_N@Z",
     L"GetWidgetColor (pre-2026)"},

};

constexpr size_t kColorSymbolCount = ARRAYSIZE(kColorSymbols);

static int g_colorHooksInstalled = 0;
static int g_colorHooksMissing = 0;

static void InstallOneColorHook(HMODULE dvaui, size_t index, void* hook,
                                void** original) {
    const ColorSymbol& sym = kColorSymbols[index];

    FARPROC proc = GetProcAddress(dvaui, sym.mangled);

    if (!proc) {
        Wh_Log(L"absent in this version: %s", sym.label);
        g_colorHooksMissing++;
        return;
    }

    if (!Wh_SetFunctionHook(reinterpret_cast<void*>(proc), hook, original)) {
        Wh_Log(L"failed to hook %s", sym.label);
        g_colorHooksMissing++;
        return;
    }

    g_colorHooksInstalled++;
}

template <size_t... I>
static void InstallColorHooksImpl(HMODULE dvaui, std::index_sequence<I...>) {
    (InstallOneColorHook(dvaui, I, reinterpret_cast<void*>(&ColorHook<I>::Hook),
                         reinterpret_cast<void**>(&ColorHook<I>::original)),
     ...);
}

// ============================================================================
// DRAWBOT BRUSHES: THE POINT EVERY FILL PASSES THROUGH
// ============================================================================

/*
    Not every interface color comes from a theme function.

    Measuring the window with a tinted palette active, 57% of it was already
    recolored — and the program monitor backdrop was still #0D0D0D, neutral, across
    13% of the screen. It is painted directly, without consulting any theme. There
    is no color function to intercept in MediaCoreUI.dll or dvaplayer.dll, and
    nothing in dvaui is named after the monitor.

    What does exist is the one place every solid fill passes through, no matter who
    picked the color:

        dvaui::drawbot::d2d::OSSupplier::NewBrush(const ColorRGBA&)

    The Direct2D brush factory. The color is read while building the brush, so a
    stack temporary is safe here — unlike the theme function path, which returns a
    reference.

    The SVG sibling (SVGSupplier::NewBrush) is deliberately left out: it paints
    icons, and a darkened icon disappears.
*/

using NewBrush_t = void*(*)(void*, const DvaColorRGBA*);
using NewPen_t = void*(*)(void*, const DvaColorRGBA*, float);
using DrawFillRect_t = void (*)(void*, const void*, const DvaColorRGBA*);

NewBrush_t NewBrush_Original = nullptr;
NewPen_t NewPen_Original = nullptr;
DrawFillRect_t DrawFillRect_Original = nullptr;

/*
    The order of these checks is the difference between a light mod and a heavy one.

    The overwhelming majority of colors arriving here will not be converted: they
    are saturated, or too light. Rejecting those first, with float comparisons,
    keeps the hash lookup off the common path — it only runs for the few colors that
    actually become background.
*/
static bool ConvertForPaint(const DvaColorRGBA* in, DvaColorRGBA* out) {
    if (!g_settings.brushHook || !in) {
        return false;
    }

    // Two pointer comparisons: cheaper than anything else here.
    if (IsOurSlot(in)) {
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
    Some entry points are handed a color reference to KEEP, not to read and drop.

    A brush reads the color while it is being built, so a stack temporary is
    safe there. A node manager that is told "erase your background with this
    color" may hold the reference for as long as the panel lives. Handing it a
    temporary would leave it pointing at reclaimed stack.

    So these go through the slot table, the same one the theme functions use:
    the returned pointer is stable for the life of the process.
*/
static const DvaColorRGBA* StableConvert(const DvaColorRGBA* in) {
    if (!g_settings.brushHook || !in || IsOurSlot(in)) {
        return in;
    }

    if (!ShouldConvert(*in)) {
        return in;
    }

    if (IsProducedColor(*in)) {
        return in;
    }

    DvaColorRGBA converted{};

    if (!ConvertDvaColor(*in, &converted)) {
        return in;
    }

    RememberProduced(converted);

    const DvaColorRGBA* stored =
        StoreColor(reinterpret_cast<uintptr_t>(in), *in, converted);

    return stored ? stored : in;
}

/*
    The surround of the program monitor is painted by neither of the paths above.

    Measured with a tinted palette active: 57% of the window had been recolored
    and that surround was still #0D0D0D, perfectly neutral, across 13% of the
    screen. It does not come from a theme color function, and it does not come
    from the Direct2D brush factory either.

    What it does look like is a panel erasing its own background before its
    children draw. Two entry points in dvaui take a background color by
    reference for exactly that:

        OS_NodeManager::ScEnableEraseBackgroundDrawing(OS_NodeManager*, const ColorRGBA&)
        UI_Node::UI_DispatchDrawFromRoot(const ColorRGBA&, Drawbot*, bool)

    The first is a scope guard that switches background erasing on with a color;
    the second draws a node subtree over one. Both are hooked, and both get a
    stable pointer because either may keep the reference.
*/
using EraseBackgroundCtor_t = void* (*)(void*, void*, const DvaColorRGBA*);
using DispatchDrawFromRoot_t = void (*)(void*, const DvaColorRGBA*, void*, bool);

EraseBackgroundCtor_t EraseBackgroundCtor_Original = nullptr;
DispatchDrawFromRoot_t DispatchDrawFromRoot_Original = nullptr;

void* EraseBackgroundCtor_Hook(void* self, void* nodeManager,
                               const DvaColorRGBA* color) {
    return EraseBackgroundCtor_Original(self, nodeManager, StableConvert(color));
}

void DispatchDrawFromRoot_Hook(void* self, const DvaColorRGBA* color,
                               void* drawbot, bool flag) {
    DispatchDrawFromRoot_Original(self, StableConvert(color), drawbot, flag);
}

/*
    Premiere does not paint everything through dvaui.

    UIFramework.dll is Adobe's own UI layer on top of the toolkit, and it has its
    own drawing context with primitives that take a color directly:

        UIF::DC::FillRect(const RectT<int>&, const ColorRGBA&)
        UIF::DC::FrameRect(const RectT<int>&, const ColorRGBA&)

    This is the layer the program monitor paints its surround with. Measured with
    the Glitch palette active: the panels around it were #101907, green, and the
    band above the sequence frame was #101010 — neutral, untouched, because no
    dvaui entry point ever sees that color.

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
    The other FillRect.

    UIF::DC has two of each drawing primitive: one taking dvaui's ColorRGBA, and
    one taking ASL::ParamColor<unsigned char>, an 8-bit-per-channel colour from a
    different Adobe library. Hooking only the first left a whole class of fills
    untouched.

    The byte layout was decoded from Adobe's own converter,
    UIF::DVAConversionUtilities::ASLColorToDVAColorRGBA:

        movzx eax, byte [rdx]        ->  out.r = b0 / 255
        movzx eax, byte [rdx+1]      ->  out.g = b1 / 255
        movzx eax, byte [rdx+2]      ->  out.b = b2 / 255
        mov dword [rcx+0Ch], 1.0f    ->  alpha is a constant, never read

    Bytes 0, 1, 2 are R, G, B. The same disassembly settles something that had
    been an assumption all along: ColorRGBA really is four floats in r, g, b, a
    order.
*/
using UifFillRectParam_t = void (*)(void*, const void*, const void*,
                                    unsigned char);
using UifFrameRectParam_t = void (*)(void*, const void*, const void*);

UifFillRectParam_t UifFillRectParam_Original = nullptr;
UifFrameRectParam_t UifFrameRectParam_Original = nullptr;

static bool RecolorParamColor(const void* color, unsigned char* copy) {
    if (!color) {
        return false;
    }

    const unsigned char* src = static_cast<const unsigned char*>(color);

    DvaColorRGBA in{};
    in.r = src[0] / 255.0f;
    in.g = src[1] / 255.0f;
    in.b = src[2] / 255.0f;
    in.a = 1.0f;

    DvaColorRGBA out{};

    if (!ConvertForPaint(&in, &out)) {
        return false;
    }

    /*
        Sixteen bytes, not four. The disassembly of Adobe's converter proves
        bytes 0, 1 and 2 are the channels; it proves nothing about the size of
        the struct, and this buffer is what the original function reads. Four
        bytes are copied because four are what is known; the rest stays zeroed
        so a larger struct still reads defined memory.
    */
    memcpy(copy, color, 4);
    copy[0] = static_cast<unsigned char>(
        ClampInt(static_cast<int>(out.r * 255.0f + 0.5f), 0, 255));
    copy[1] = static_cast<unsigned char>(
        ClampInt(static_cast<int>(out.g * 255.0f + 0.5f), 0, 255));
    copy[2] = static_cast<unsigned char>(
        ClampInt(static_cast<int>(out.b * 255.0f + 0.5f), 0, 255));

    return true;
}

void UifFillRectParam_Hook(void* self, const void* rect, const void* color,
                           unsigned char flags) {
    unsigned char copy[16]{};

    if (RecolorParamColor(color, copy)) {
        UifFillRectParam_Original(self, rect, copy, flags);
        return;
    }

    UifFillRectParam_Original(self, rect, color, flags);
}

void UifFrameRectParam_Hook(void* self, const void* rect, const void* color) {
    unsigned char copy[16]{};

    if (RecolorParamColor(color, copy)) {
        UifFrameRectParam_Original(self, rect, copy);
        return;
    }

    UifFrameRectParam_Original(self, rect, color);
}

static void InstallUifHooks(HMODULE uif) {
    struct {
        const char* mangled;
        void* hook;
        void** original;
        const wchar_t* label;
    } specs[] = {
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

    for (const auto& spec : specs) {
        FARPROC proc = GetProcAddress(uif, spec.mangled);

        if (!proc) {
            Wh_Log(L"absent in this version: %s", spec.label);
            g_colorHooksMissing++;
            continue;
        }

        if (!Wh_SetFunctionHook(reinterpret_cast<void*>(proc), spec.hook,
                                spec.original)) {
            Wh_Log(L"failed to hook %s", spec.label);
            g_colorHooksMissing++;
            continue;
        }

        g_colorHooksInstalled++;
    }
}

static void InstallBrushHooks(HMODULE dvaui) {
    struct {
        const char* mangled;
        void* hook;
        void** original;
        const wchar_t* label;
    } specs[] = {
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

    for (const auto& spec : specs) {
        FARPROC proc = GetProcAddress(dvaui, spec.mangled);

        if (!proc) {
            Wh_Log(L"absent in this version: %s", spec.label);
            g_colorHooksMissing++;
            continue;
        }

        if (!Wh_SetFunctionHook(reinterpret_cast<void*>(proc), spec.hook,
                                spec.original)) {
            Wh_Log(L"failed to hook %s", spec.label);
            g_colorHooksMissing++;
            continue;
        }

        g_colorHooksInstalled++;
    }
}

/*
    The two modules do not necessarily load together, so each is tracked on its
    own. The waiting thread keeps going until both are in, instead of stopping
    at the first one that happened to be ready.
*/
static bool g_dvauiHooked = false;
static bool g_uifHooked = false;

/*
    Registers the hooks for one module, if it is one of ours and not done yet.

    Returns whether anything was registered, so the caller knows whether an
    apply is needed. Nothing here applies the operations itself: at init
    Windhawk applies them when Wh_ModInit returns, and from the loader hook the
    caller applies them once for the module that just arrived.
*/
static bool HandleLoadedModule(HMODULE module) {
    if (!module) {
        return false;
    }

    bool registered = false;

    if (!g_dvauiHooked && module == GetModuleHandleW(L"dvaui.dll")) {
        g_colorHooksInstalled = 0;
        g_colorHooksMissing = 0;

        if (g_settings.dvauiHook) {
            InstallColorHooksImpl(module,
                                  std::make_index_sequence<kColorSymbolCount>{});
        }

        if (g_settings.brushHook) {
            InstallBrushHooks(module);
        }

        g_dvauiHooked = true;
        registered = true;

        if (g_colorHooksInstalled) {
            Wh_Log(L"dvaui: %d hooks active, %d absent in this version",
                   g_colorHooksInstalled, g_colorHooksMissing);
        } else if (g_settings.dvauiHook) {
            Wh_Log(L"no dvaui color entry point matched — this Premiere version "
                   L"is not supported by the interface layer. The window frame "
                   L"and menus still apply.");
        }
    }

    if (!g_uifHooked && g_settings.brushHook &&
        module == GetModuleHandleW(L"UIFramework.dll")) {
        InstallUifHooks(module);
        g_uifHooked = true;
        registered = true;
    }

    return registered;
}

static bool HandleAlreadyLoadedModules() {
    bool registered = HandleLoadedModule(GetModuleHandleW(L"dvaui.dll"));

    return HandleLoadedModule(GetModuleHandleW(L"UIFramework.dll")) || registered;
}

/*
    Why the loader instead of a thread.

    dvaui and UIFramework are not necessarily loaded when the mod initialises —
    enable the mod before Premiere starts and neither is. The previous answer
    was a thread that polled GetModuleHandleW every 100 ms and gave up after two
    minutes, which had two problems beyond the polling itself: a mod that starts
    late enough missed its window silently, and the thread could still be inside
    Wh_SetFunctionHook when Windhawk tore the mod down.

    Hooking the loader removes all of it. The hook goes in kernelbase, not
    kernel32, because kernel32's export is only a forwarder and Premiere's own
    calls go straight to kernelbase.
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

    NoteAdobeModule(module);

    if (HandleLoadedModule(module) && !Wh_ApplyHookOperations()) {
        Wh_Log(L"failed to apply hooks for a late-loaded module");
    }

    return module;
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
    The uxtheme dark mode functions have no names — only ordinals — and ordinal 135
    changed signature between Windows 10 1809 and 1903. Calling the wrong signature
    passes garbage on the stack, so the Windows version decides which of the two to
    resolve.
*/
static void InitNativeDarkMode() {
    g_buildNumber = GetWindowsBuild();

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

    /*
        Everything above is a lookup and changes nothing; the menu hooks need
        those entry points whatever this setting says. What follows changes the
        process-wide app mode — scrollbars, common dialogs, control themes — so
        it belongs to "Window and system dialogs" and has to answer to it.
    */
    if (!g_settings.nativeDarkMode) {
        return;
    }

    if (g_buildNumber < 18362) {
        g_AllowDarkModeForApp = reinterpret_cast<AllowDarkModeForApp_t>(ordinal135);

        if (g_AllowDarkModeForApp) {
            g_AllowDarkModeForApp(true);
        }
    } else {
        g_SetPreferredAppMode =
            reinterpret_cast<SetPreferredAppMode_t>(ordinal135);

        if (g_SetPreferredAppMode) {
            g_SetPreferredAppMode(PreferredAppMode::ForceDark);
        }
    }

    if (g_RefreshImmersiveColorPolicyState) {
        g_RefreshImmersiveColorPolicyState();
    }

    if (g_FlushMenuThemes) {
        g_FlushMenuThemes();
    }
}

static void ApplyDarkModeToWindow(HWND hwnd) {
    if (!hwnd || !g_settings.nativeDarkMode) {
        return;
    }

    if (g_AllowDarkModeForWindow) {
        g_AllowDarkModeForWindow(hwnd, true);
    }

    if (g_SetWindowTheme) {
        g_SetWindowTheme(hwnd, L"DarkMode_Explorer", nullptr);
    }

    constexpr DWORD kUseImmersiveDarkMode = 20;

    BOOL dark = TRUE;

    DwmSetWindowAttribute(hwnd,
                          static_cast<DWMWINDOWATTRIBUTE>(kUseImmersiveDarkMode),
                          &dark, sizeof(dark));

    /*
        Title bar and border only exist on a top-level window, and GetParent is
        the wrong way to ask: for a WS_POPUP it returns the OWNER, so Premiere's
        owned dialogs and floating panels — which do have captions — were taking
        this early return and keeping the default colours. The style bit answers
        the question that was actually being asked.
    */
    if (GetWindowLongPtrW(hwnd, GWL_STYLE) & WS_CHILD) {
        return;
    }

    /*
        DWMWA_CAPTION_COLOR exists from Windows 11 (build 22000) on. On an earlier build
        the call returns an error and the bar keeps the default dark mode gray —
        acceptable degradation, not a failure.
    */
    if (g_buildNumber < 22000) {
        return;
    }

    constexpr DWORD kBorderColor = 34;
    constexpr DWORD kCaptionColor = 35;
    constexpr DWORD kTextColor = 36;

    COLORREF border = g_settings.palette.ramp[4];
    COLORREF caption = g_settings.palette.ramp[1];
    COLORREF text = g_settings.palette.text;

    DwmSetWindowAttribute(hwnd, static_cast<DWMWINDOWATTRIBUTE>(kBorderColor),
                          &border, sizeof(border));
    DwmSetWindowAttribute(hwnd, static_cast<DWMWINDOWATTRIBUTE>(kCaptionColor),
                          &caption, sizeof(caption));
    DwmSetWindowAttribute(hwnd, static_cast<DWMWINDOWATTRIBUTE>(kTextColor), &text,
                          sizeof(text));
}

/*
    Undoing the frame.

    DwmSetWindowAttribute and SetWindowTheme stick to a window for its lifetime,
    so without this the caption, border and text stay themed after the mod is
    disabled, until Premiere restarts. Windhawk's principle is that a mod's
    effects go away when it does, and since the reload path runs on every
    settings change, this also stops an old palette from surviving a change.
*/
static void RevertWindowFrame(HWND hwnd) {
    if (!hwnd) {
        return;
    }

    if (g_SetWindowTheme) {
        g_SetWindowTheme(hwnd, nullptr, nullptr);
    }

    constexpr DWORD kUseImmersiveDarkMode = 20;
    constexpr DWORD kBorderColor = 34;
    constexpr DWORD kCaptionColor = 35;
    constexpr DWORD kTextColor = 36;

    BOOL dark = FALSE;

    DwmSetWindowAttribute(hwnd,
                          static_cast<DWMWINDOWATTRIBUTE>(kUseImmersiveDarkMode),
                          &dark, sizeof(dark));

    // DWMWA_COLOR_DEFAULT: hand the colour back to the system.
    COLORREF automatic = 0xFFFFFFFF;

    const DWORD colorAttributes[] = {kBorderColor, kCaptionColor, kTextColor};

    for (DWORD attribute : colorAttributes) {
        DwmSetWindowAttribute(hwnd, static_cast<DWMWINDOWATTRIBUTE>(attribute),
                              &automatic, sizeof(automatic));
    }
}

static BOOL CALLBACK RevertChild(HWND hwnd, LPARAM) {
    RevertWindowFrame(hwnd);
    return TRUE;
}

static BOOL CALLBACK RevertTopLevel(HWND hwnd, LPARAM) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);

    if (pid != GetCurrentProcessId()) {
        return TRUE;
    }

    RevertWindowFrame(hwnd);
    EnumChildWindows(hwnd, RevertChild, 0);

    if (GetMenu(hwnd)) {
        DrawMenuBar(hwnd);
    }

    return TRUE;
}

static void RevertThemeFromExistingWindows() {
    EnumWindows(RevertTopLevel, 0);
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

// ============================================================================
// SYSTEM COLORS
// ============================================================================

HBRUSH g_sysBrushes[COLOR_MENUBAR + 1]{};
SRWLOCK g_brushLock = SRWLOCK_INIT;

static bool MapSysColor(int index, COLORREF* out) {
    const Palette& p = g_settings.palette;

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

    if (g_settings.nativeDarkMode && MapSysColor(index, &mapped)) {
        return mapped;
    }

    return GetSysColor_Original(index);
}

/*
    The brush returned by GetSysColorBrush must not be deleted by the caller, so it
    has to live as long as the process does. That is why these brushes are cached
    and never destroyed when the mod unloads — see Wh_ModUninit.
*/
static HBRUSH ThemeSysBrush(int index) {
    COLORREF mapped;

    if (index < 0 || index > COLOR_MENUBAR || !MapSysColor(index, &mapped)) {
        return nullptr;
    }

    AcquireSRWLockExclusive(&g_brushLock);

    if (!g_sysBrushes[index]) {
        g_sysBrushes[index] = CreateSolidBrush(mapped);
    }

    HBRUSH brush = g_sysBrushes[index];

    ReleaseSRWLockExclusive(&g_brushLock);

    return brush;
}

HBRUSH WINAPI GetSysColorBrush_Hook(int index) {
    if (g_settings.nativeDarkMode) {
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

    if (g_settings.nativeDarkMode && raw >= 1 &&
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
    Premiere's File / Edit / Clip bar is a native Win32 menu, and its theme does NOT
    come from the public OpenThemeData: Windows opens non-client themes — menu,
    scrollbar, border — through OpenNcThemeData, which exists only as uxtheme
    ordinal 49 and appears in no header.

    Intercepting only OpenThemeData leaves the dropdown menus dark and the top bar
    white, which was exactly the symptom. Both have to be recorded.

    DrawThemeBackground receives an opaque HTHEME and there is no API that returns
    its class, so the class is noted at open time.
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

/*
    High-water mark: how many slots have ever been used.

    Without it, every DrawThemeBackground call scans all 256 slots even in a process
    that registered three themes. With it, it scans three.
*/
size_t g_menuThemeHighWater = 0;

static void RememberMenuTheme(HTHEME theme) {
    AcquireSRWLockExclusive(&g_themeLock);

    bool known = false;

    for (size_t i = 0; i < kThemeSlots; i++) {
        if (g_menuThemes[i] == theme) {
            known = true;
            break;
        }
    }

    if (!known) {
        for (size_t i = 0; i < kThemeSlots; i++) {
            if (!g_menuThemes[i]) {
                g_menuThemes[i] = theme;
                if (i + 1 > g_menuThemeHighWater) {
                    g_menuThemeHighWater = i + 1;
                }
                break;
            }
        }
    }

    ReleaseSRWLockExclusive(&g_themeLock);
}

static void ForgetMenuTheme(HTHEME theme) {
    AcquireSRWLockExclusive(&g_themeLock);

    for (size_t i = 0; i < kThemeSlots; i++) {
        if (g_menuThemes[i] == theme) {
            g_menuThemes[i] = nullptr;
            break;
        }
    }

    ReleaseSRWLockExclusive(&g_themeLock);
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
    Observing the theme being opened and repainting over it is not enough: the class
    being asked for has to be swapped.

    Measured on this Windows, with the system app mode set to light:

        OpenNcThemeData(nullptr, L"Menu")            -> text #000000  (light)
        OpenNcThemeData(nullptr, L"DarkMode::Menu")  -> text #FFFFFF  (dark)

    The "Menu" class does return the dark theme too, but only after
    SetPreferredAppMode(ForceDark) and FlushMenuThemes have run. That is an ORDERING
    dependency, and it is what leaves the bar white: user32 opens and caches the
    menu bar theme on the window's first paint, which can happen before the mod
    acts — and then the light theme is cached and no amount of repainting fixes the
    half the theme decides (text, metrics, check glyphs, submenu arrows).

    Asking for "DarkMode::Menu" explicitly depends on no state at all: the dark
    variant comes back dark even with the whole of Windows in light mode.

    The classes "DarkMode_Menu" and "DarkMode::Explorer::Menu" do not exist — they
    return NULL. That is why the result is always checked before being used.
*/
static HTHEME OpenDarkMenuTheme(HWND hwnd, LPCWSTR classList,
                                OpenNcThemeData_t opener) {
    if (!g_settings.menuHook || !classList || !opener) {
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
    /*
        Exactly "Menu", the same test OpenDarkMenuTheme uses to decide what to
        swap. A substring match would also register any class whose name merely
        contains "menu", and PaintMenuPart would then repaint parts 7-15 of it
        with menu colours — part numbers mean different things in different
        classes, so that shows up as a corrupted control.
    */
    if (theme && classList && _wcsicmp(classList, L"Menu") == 0) {
        RememberMenuTheme(theme);
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
    if (g_settings.menuHook && classList && _wcsicmp(classList, L"Menu") == 0) {
        HTHEME dark =
            OpenThemeDataForDpi_Original(hwnd, L"DarkMode::Menu", dpi);

        if (dark) {
            return TrackMenuTheme(dark, classList);
        }
    }

    return TrackMenuTheme(OpenThemeDataForDpi_Original(hwnd, classList, dpi),
                          classList);
}

/*
    The File / Edit / Clip bar comes through here, not through the public
    OpenThemeData: Windows opens non-client themes through OpenNcThemeData, which
    exists only as uxtheme ordinal 49 and appears in no header.
*/
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
    const Palette& p = g_settings.palette;

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
    if (g_settings.menuHook && rect && IsMenuTheme(theme) &&
        PaintMenuPart(hdc, part, state, rect)) {
        return S_OK;
    }

    return DrawThemeBackground_Original(theme, hdc, part, state, rect, clip);
}

HRESULT WINAPI DrawThemeBackgroundEx_Hook(HTHEME theme, HDC hdc, int part,
                                          int state, const RECT* rect,
                                          const void* options) {
    if (g_settings.menuHook && rect && IsMenuTheme(theme) &&
        PaintMenuPart(hdc, part, state, rect)) {
        return S_OK;
    }

    return DrawThemeBackgroundEx_Original(theme, hdc, part, state, rect, options);
}

/*
    Repainting only the background is not enough.

    If Windows opened the light variant of the menu theme, the text comes back
    black — and black on the background we just painted black is an invisible menu,
    which is worse than a white one. So the text color is forced alongside it.

    The DTTOPTS layout is written out here instead of coming from uxtheme.h because
    that header varies between toolchains, and one field more or less misaligns the
    whole struct with no compile error.
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
    const Palette& p = g_settings.palette;

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
    if (g_settings.menuHook && rect && DrawThemeTextEx_Original &&
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
    if (g_settings.menuHook && IsMenuTheme(theme)) {
        ThemeDttOpts opts{};

        if (options) {
            // Keep what the caller asked for and only swap the color.
            DWORD callerSize = *reinterpret_cast<const DWORD*>(options);

            if (callerSize <= sizeof(opts)) {
                memcpy(&opts, options, callerSize);
            }
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
    The File / Edit / Clip bar is not themed content that can be repainted through
    DrawThemeBackground. It is non-client area, and user32 draws it by sending the
    window two undocumented messages:

        WM_UAHDRAWMENU      (0x91)  the background of the whole strip
        WM_UAHDRAWMENUITEM  (0x92)  each item, one at a time

    They arrive at DefWindowProc / DefFrameProc, which is where the default — light
    — drawing happens. Intercepting there and painting into the same HDCs and
    rectangles Windows was going to use makes the strip ours.

    Swapping the theme class to DarkMode::Menu does not fix this strip on its own:
    that governs the dropdown menus. Two different mechanisms for two parts that
    look like the same thing.

    Technique from https://github.com/adzm/win32-darkmode (darkmenubar branch),
    MIT licensed. The implementation below is this mod's own.
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

thread_local HTHEME g_menuBarTheme = nullptr;

static HTHEME MenuBarTheme(HWND hwnd) {
    if (g_menuBarTheme) {
        return g_menuBarTheme;
    }

    OpenThemeData_t open = OpenThemeData_Original;

    if (!open && g_uxtheme) {
        open = reinterpret_cast<OpenThemeData_t>(
            GetProcAddress(g_uxtheme, "OpenThemeData"));
    }

    if (!open) {
        return nullptr;
    }

    // The dark variant comes back dark even with Windows in light mode.
    g_menuBarTheme = open(hwnd, L"DarkMode::Menu");

    if (!g_menuBarTheme) {
        g_menuBarTheme = open(hwnd, L"Menu");
    }

    return g_menuBarTheme;
}

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
        FillWith(hdc, &line, g_settings.palette.ramp[4]);
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

    FillWith(info->hdc, &bar, g_settings.palette.ramp[1]);

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

    const Palette& p = g_settings.palette;

    UINT state = draw->dis.itemState;

    COLORREF background = p.ramp[1];

    if (state & ODS_HOTLIGHT) {
        background = p.accent;
    } else if (state & ODS_SELECTED) {
        background = p.ramp[4];
    }

    FillWith(draw->um.hdc, &draw->dis.rcItem, background);

    /*
        An item with no text is an MDI window caption button, drawn with an icon font
        glyph. Premiere has no MDI, so the already-darkened background is enough and the
        glyph is left to Windows.
    */
    if (label[0] == L'\0') {
        return true;
    }

    /*
        Only a genuinely disabled item is dimmed. An unfocused window used to
        count as disabled here, which meant alt-tabbing away greyed the whole
        File/Edit/Clip bar — Windows does not do that, and on a second monitor
        the bar would sit there looking permanently disabled.
    */
    bool disabled = (state & (ODS_GRAYED | ODS_DISABLED)) != 0;

    DrawThemeTextEx_t drawText = ResolveDrawThemeTextEx();
    HTHEME theme = MenuBarTheme(hwnd);

    if (!drawText || !theme) {
        return false;
    }

    DWORD flags = DT_CENTER | DT_SINGLELINE | DT_VCENTER;

    if (state & ODS_NOACCEL) {
        flags |= DT_HIDEPREFIX;
    }

    ThemeDttOpts opts{};
    opts.dwSize = sizeof(opts);
    opts.dwFlags = kDttTextColor;
    opts.crText = disabled ? p.dimText : p.text;

    HRESULT hr = drawText(theme, draw->um.hdc, kMenuBarItem, 1, label,
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

        case WM_THEMECHANGED:
        case WM_DESTROY:
            if (g_menuBarTheme) {
                CloseThemeData_t close = CloseThemeData_Original;

                if (!close && g_uxtheme) {
                    close = reinterpret_cast<CloseThemeData_t>(
                        GetProcAddress(g_uxtheme, "CloseThemeData"));
                }

                if (close) {
                    close(g_menuBarTheme);
                }

                g_menuBarTheme = nullptr;
            }
            return false;

        default:
            return false;
    }
}

/*
    DefWindowProc runs for practically every message of every window in the process.
    The cheap test has to come first.

    Comparing the message id is a chain of integer comparisons the compiler turns
    into a jump table; GetMenu reaches into the window structure. With the right
    order, the mod's cost on the common path — which is almost every path — stays
    close to zero.
*/
static bool IsMenuBarMessage(UINT msg) {
    switch (msg) {
        case WM_UAHDRAWMENU:
        case WM_UAHDRAWMENUITEM:
        case WM_THEMECHANGED:
        case WM_DESTROY:
        case WM_NCPAINT:
        case WM_ACTIVATE:
            return true;
        default:
            return false;
    }
}

static bool NeedsMenuBarWork(HWND hwnd, UINT msg) {
    return g_settings.menuHook && IsMenuBarMessage(msg) && hwnd &&
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

/*
    An app can install its own background brush on a menu. If that brush is light,
    the menu goes light again underneath everything we did. Replacing the brush here
    is cheaper than working out later why one single menu came back white.
*/
using SetMenuInfo_t = BOOL(WINAPI*)(HMENU, LPCMENUINFO);

SetMenuInfo_t SetMenuInfo_Original = nullptr;

BOOL WINAPI SetMenuInfo_Hook(HMENU menu, LPCMENUINFO info) {
    alignas(MENUINFO) BYTE buffer[256];

    if (!g_settings.menuHook || !info || !(info->fMask & MIM_BACKGROUND) ||
        info->cbSize > sizeof(buffer)) {
        return SetMenuInfo_Original(menu, info);
    }

    memcpy(buffer, info, info->cbSize);

    auto* copy = reinterpret_cast<LPMENUINFO>(buffer);
    copy->hbrBack = ThemeSysBrush(COLOR_MENU);

    return SetMenuInfo_Original(menu, copy);
}

// ============================================================================
// GDI SURFACES
// ============================================================================

static COLORREF ConvertGdiColor(COLORREF color) {
    float r = GetRValue(color) / 255.0f;
    float g = GetGValue(color) / 255.0f;
    float b = GetBValue(color) / 255.0f;

    if (!IsNeutral(r, g, b, 0.035f)) {
        return color;
    }

    float brightness = (r + g + b) / 3.0f;

    if (brightness > g_settings.ceiling) {
        return color;
    }

    COLORREF target = PickTarget(brightness);

    int nr = ClampInt(
        static_cast<int>(Blend(r, GetRValue(target) / 255.0f) * 255.0f + 0.5f), 0,
        255);
    int ng = ClampInt(
        static_cast<int>(Blend(g, GetGValue(target) / 255.0f) * 255.0f + 0.5f), 0,
        255);
    int nb = ClampInt(
        static_cast<int>(Blend(b, GetBValue(target) / 255.0f) * 255.0f + 0.5f), 0,
        255);

    return RGB(nr, ng, nb);
}

using CreateSolidBrush_t = HBRUSH(WINAPI*)(COLORREF);
using CreatePen_t = HPEN(WINAPI*)(int, int, COLORREF);
using SetBkColor_t = COLORREF(WINAPI*)(HDC, COLORREF);

CreateSolidBrush_t CreateSolidBrush_Original = nullptr;
CreatePen_t CreatePen_Original = nullptr;
SetBkColor_t SetBkColor_Original = nullptr;

HBRUSH WINAPI CreateSolidBrush_Hook(COLORREF color) {
    if (g_settings.gdiHook && IsAdobeUICaller(__builtin_return_address(0))) {
        color = ConvertGdiColor(color);
    }

    return CreateSolidBrush_Original(color);
}

HPEN WINAPI CreatePen_Hook(int style, int width, COLORREF color) {
    if (g_settings.gdiHook && IsAdobeUICaller(__builtin_return_address(0))) {
        color = ConvertGdiColor(color);
    }

    return CreatePen_Original(style, width, color);
}

COLORREF WINAPI SetBkColor_Hook(HDC hdc, COLORREF color) {
    if (g_settings.gdiHook && IsAdobeUICaller(__builtin_return_address(0))) {
        color = ConvertGdiColor(color);
    }

    return SetBkColor_Original(hdc, color);
}

// ============================================================================
// SETTINGS
// ============================================================================

static bool ParseHexColor(PCWSTR text, COLORREF* out) {
    if (!text) {
        return false;
    }

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

static COLORREF ReadColorSetting(PCWSTR name, COLORREF fallback) {
    PCWSTR text = Wh_GetStringSetting(name);
    COLORREF parsed = fallback;

    if (!ParseHexColor(text, &parsed)) {
        Wh_Log(L"invalid value in %s, using the default", name);
        parsed = fallback;
    }

    Wh_FreeStringSetting(text);

    return parsed;
}

struct NamedPalette {
    const wchar_t* id;
    Palette colors;
};

/*
    Two of these palettes were not invented, they were measured.

    PREMIERE: sampled from the icon of Adobe Premiere Pro.exe itself. Of the 728
    opaque pixels, 72% are #00005B and 16% are #9999FF, with #3A3A99 and #7373D6 in
    the mid-tones. The whole logo lives on hue 240 — which is why the ramp below
    climbs from near-black to an indigo, with the text pulled toward the periwinkle
    of the "Pr".

    NEON and GLITCH: sampled from reference artwork. The first is near-black with
    magenta; the original magenta accent enters lowered, because at full strength,
    behind a menu item, it drowns the text instead of highlighting it.

    In GLITCH the acid green cannot be background: 70% of the screen in it would be
    far too bright to judge an image on, which is the job. So the green becomes the
    BIAS of the black and the top of the ramp, and the magenta becomes the accent.
    Strong color where it is a highlight, not where it is area.

    COMFY: this one is designed rather than measured. Warm brown and lower contrast
    than the others — the point is to sit in it for hours, so it deliberately does
    NOT go near black.
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
};

static void LoadSettings() {
    PCWSTR name = Wh_GetStringSetting(L"palette");

    // Onyx is the default: first in the table and also the fallback.
    Palette p = kPalettes[0].colors;

    if (wcscmp(name, L"custom") == 0) {
        p.ramp[0] = ReadColorSetting(L"customBase", p.ramp[0]);
        p.ramp[1] = ReadColorSetting(L"customPanel", p.ramp[1]);
        p.ramp[2] = ReadColorSetting(L"customSurface", p.ramp[2]);
        p.ramp[3] = ReadColorSetting(L"customElevated", p.ramp[3]);
        p.ramp[4] = ReadColorSetting(L"customBorder", p.ramp[4]);
        p.text = ReadColorSetting(L"customText", p.text);
        p.accent = ReadColorSetting(L"customAccent", p.accent);
    } else {
        for (const NamedPalette& candidate : kPalettes) {
            if (wcscmp(name, candidate.id) == 0) {
                p = candidate.colors;
                break;
            }
        }
    }

    Wh_FreeStringSetting(name);

    g_settings.palette = p;

    g_settings.strength =
        ClampFloat(Wh_GetIntSetting(L"strength") / 100.0f, 0.0f, 1.0f);

    g_settings.ceiling =
        ClampFloat(Wh_GetIntSetting(L"ceiling") / 100.0f, 0.05f, 0.95f);

    g_settings.dvauiHook = Wh_GetIntSetting(L"dvauiHook") != 0;
    g_settings.brushHook = Wh_GetIntSetting(L"brushHook") != 0;
    g_settings.nativeDarkMode = Wh_GetIntSetting(L"nativeDarkMode") != 0;
    g_settings.menuHook = Wh_GetIntSetting(L"menuHook") != 0;
    g_settings.gdiHook = Wh_GetIntSetting(L"gdiHook") != 0;
}

// ============================================================================
// LIFECYCLE
// ============================================================================

static void HookOrLog(void* target, void* hook, void** original,
                      const wchar_t* label) {
    if (!target) {
        Wh_Log(L"target absent: %s", label);
        return;
    }

    if (!Wh_SetFunctionHook(target, hook, original)) {
        Wh_Log(L"failed to hook %s", label);
    }
}

static void* UxThemeProc(const char* name) {
    return g_uxtheme ? reinterpret_cast<void*>(GetProcAddress(g_uxtheme, name))
                     : nullptr;
}

static void* UxThemeOrdinal(WORD ordinal) {
    return g_uxtheme ? reinterpret_cast<void*>(
                           GetProcAddress(g_uxtheme, MAKEINTRESOURCEA(ordinal)))
                     : nullptr;
}

BOOL Wh_ModInit() {
    LoadSettings();

    if (!AllocateSlots()) {
        Wh_Log(L"could not allocate the colour table; the interface layer will "
               L"pass colours through unchanged");
    }

    SnapshotAdobeModules();
    InitNativeDarkMode();

    HookOrLog(reinterpret_cast<void*>(CreateWindowExW),
              reinterpret_cast<void*>(CreateWindowExW_Hook),
              reinterpret_cast<void**>(&CreateWindowExW_Original),
              L"CreateWindowExW");

    if (g_settings.nativeDarkMode) {
        HookOrLog(reinterpret_cast<void*>(GetSysColor),
                  reinterpret_cast<void*>(GetSysColor_Hook),
                  reinterpret_cast<void**>(&GetSysColor_Original), L"GetSysColor");

        HookOrLog(reinterpret_cast<void*>(GetSysColorBrush),
                  reinterpret_cast<void*>(GetSysColorBrush_Hook),
                  reinterpret_cast<void**>(&GetSysColorBrush_Original),
                  L"GetSysColorBrush");

        HookOrLog(reinterpret_cast<void*>(FillRect),
                  reinterpret_cast<void*>(FillRect_Hook),
                  reinterpret_cast<void**>(&FillRect_Original), L"FillRect");
    }

    if (g_settings.menuHook) {
        HookOrLog(UxThemeProc("OpenThemeData"),
                  reinterpret_cast<void*>(OpenThemeData_Hook),
                  reinterpret_cast<void**>(&OpenThemeData_Original),
                  L"OpenThemeData");

        HookOrLog(UxThemeProc("OpenThemeDataForDpi"),
                  reinterpret_cast<void*>(OpenThemeDataForDpi_Hook),
                  reinterpret_cast<void**>(&OpenThemeDataForDpi_Original),
                  L"OpenThemeDataForDpi");

        // Ordinal 49: this is where the menu bar picks up its theme.
        HookOrLog(UxThemeOrdinal(49),
                  reinterpret_cast<void*>(OpenNcThemeData_Hook),
                  reinterpret_cast<void**>(&OpenNcThemeData_Original),
                  L"OpenNcThemeData");

        HookOrLog(UxThemeProc("CloseThemeData"),
                  reinterpret_cast<void*>(CloseThemeData_Hook),
                  reinterpret_cast<void**>(&CloseThemeData_Original),
                  L"CloseThemeData");

        HookOrLog(UxThemeProc("DrawThemeBackground"),
                  reinterpret_cast<void*>(DrawThemeBackground_Hook),
                  reinterpret_cast<void**>(&DrawThemeBackground_Original),
                  L"DrawThemeBackground");

        HookOrLog(UxThemeProc("DrawThemeBackgroundEx"),
                  reinterpret_cast<void*>(DrawThemeBackgroundEx_Hook),
                  reinterpret_cast<void**>(&DrawThemeBackgroundEx_Original),
                  L"DrawThemeBackgroundEx");

        HookOrLog(UxThemeProc("DrawThemeText"),
                  reinterpret_cast<void*>(DrawThemeText_Hook),
                  reinterpret_cast<void**>(&DrawThemeText_Original),
                  L"DrawThemeText");

        HookOrLog(UxThemeProc("DrawThemeTextEx"),
                  reinterpret_cast<void*>(DrawThemeTextEx_Hook),
                  reinterpret_cast<void**>(&DrawThemeTextEx_Original),
                  L"DrawThemeTextEx");

        /*
            The menu bar is drawn by DefWindowProc / DefFrameProc in response to
            WM_UAHDRAWMENU and WM_UAHDRAWMENUITEM. All four variants are hooked because
            there is no way to know which one Premiere uses — and hooking the one it does
            not use costs nothing.
        */
        HookOrLog(reinterpret_cast<void*>(DefWindowProcW),
                  reinterpret_cast<void*>(DefWindowProcW_Hook),
                  reinterpret_cast<void**>(&DefWindowProcW_Original),
                  L"DefWindowProcW");

        HookOrLog(reinterpret_cast<void*>(DefWindowProcA),
                  reinterpret_cast<void*>(DefWindowProcA_Hook),
                  reinterpret_cast<void**>(&DefWindowProcA_Original),
                  L"DefWindowProcA");

        HookOrLog(reinterpret_cast<void*>(DefFrameProcW),
                  reinterpret_cast<void*>(DefFrameProcW_Hook),
                  reinterpret_cast<void**>(&DefFrameProcW_Original),
                  L"DefFrameProcW");

        HookOrLog(reinterpret_cast<void*>(DefFrameProcA),
                  reinterpret_cast<void*>(DefFrameProcA_Hook),
                  reinterpret_cast<void**>(&DefFrameProcA_Original),
                  L"DefFrameProcA");

        HookOrLog(reinterpret_cast<void*>(SetMenuInfo),
                  reinterpret_cast<void*>(SetMenuInfo_Hook),
                  reinterpret_cast<void**>(&SetMenuInfo_Original), L"SetMenuInfo");
    }

    if (g_settings.gdiHook) {
        HookOrLog(reinterpret_cast<void*>(CreateSolidBrush),
                  reinterpret_cast<void*>(CreateSolidBrush_Hook),
                  reinterpret_cast<void**>(&CreateSolidBrush_Original),
                  L"CreateSolidBrush");

        HookOrLog(reinterpret_cast<void*>(CreatePen),
                  reinterpret_cast<void*>(CreatePen_Hook),
                  reinterpret_cast<void**>(&CreatePen_Original), L"CreatePen");

        HookOrLog(reinterpret_cast<void*>(SetBkColor),
                  reinterpret_cast<void*>(SetBkColor_Hook),
                  reinterpret_cast<void**>(&SetBkColor_Original), L"SetBkColor");
    }

    /*
        kernelbase, not kernel32: kernel32's LoadLibraryExW is only a forwarder,
        and callers inside the process go straight to the real one.
    */
    HMODULE kernelBase = GetModuleHandleW(L"kernelbase.dll");

    auto loadLibraryExW =
        kernelBase ? reinterpret_cast<LoadLibraryExW_t>(
                         GetProcAddress(kernelBase, "LoadLibraryExW"))
                   : nullptr;

    if (loadLibraryExW) {
        HookOrLog(reinterpret_cast<void*>(loadLibraryExW),
                  reinterpret_cast<void*>(LoadLibraryExW_Hook),
                  reinterpret_cast<void**>(&LoadLibraryExW_Original),
                  L"kernelbase!LoadLibraryExW");
    } else {
        Wh_Log(L"could not resolve kernelbase!LoadLibraryExW; a Premiere that "
               L"loads its UI modules after this point will not be themed");
    }

    // Windhawk applies every operation registered here once this returns.
    HandleAlreadyLoadedModules();

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
        straight to the system and the windows come back with their own colours.
    */
    RevertThemeFromExistingWindows();

    if (g_SetPreferredAppMode) {
        g_SetPreferredAppMode(PreferredAppMode::Default);
    }

    if (g_AllowDarkModeForApp) {
        g_AllowDarkModeForApp(false);
    }

    if (g_FlushMenuThemes) {
        g_FlushMenuThemes();
    }

    /*
        The g_sysBrushes brushes are deliberately leaked.

        They were handed to Premiere by GetSysColorBrush, which promises a valid brush
        for the rest of the process lifetime. Deleting them here would trade a few bytes
        of leak for an invalid handle still in use — Premiere keeps running after the
        mod goes away.
    */

    if (g_uxtheme) {
        FreeLibrary(g_uxtheme);
        g_uxtheme = nullptr;
    }
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    /*
        Colors are cached on both sides: in this mod's slots and inside Premiere itself.
        Reloading clears our side; the other side only clears on a Premiere restart.
    */
    *bReload = TRUE;
    return TRUE;
}
