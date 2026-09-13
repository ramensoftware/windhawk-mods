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

Some surfaces are painted directly, without consulting the theme at all — most
of the monitor and timeline chrome. There is no color function to intercept for
them. What there is, is the places most solid fills pass through no matter who
picked the color:

```
dvaui::drawbot::d2d::OSSupplier::NewBrush(const ColorRGBA&)
UIF::DC::FillRect / UIF::DC::FrameRect      [UIFramework.dll]
```

The Direct2D brush factory, and Premiere's own drawing layer. Hooking them
catches most of what the theme functions never hand out — not all of it: the
band around the video in the monitors reaches neither, see *Known limitations*.
The SVG brush is deliberately left alone — it paints icons, and a darkened icon
disappears.

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
active. On the near-black palettes it matches the panels exactly; with a
tinted palette it shows as a neutral band. It is a documented limitation rather
than a bug, and what was measured is under *Known limitations* below.

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

Colors that are content rather than interface are left alone whatever they
are: the swatches in the color picker, marker colors and Essential Graphics,
and the parameter colors that Effect Controls, Lumetri and the monitors draw. A
dark gray you pick shows as that gray, not as the palette.

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

Zoomed out, the monitors paint the area around the picture a gray made from a
single channel of the panel color — the red one. Without the mod it matches the
panels, `#1D1D1D` on both. On the near-black palettes it still matches, because
the red channel of a gray is the gray: Onyx measures `#0C0C0C` for panels and
surround alike. With a hue the step shows. Glitch panels `#101907` give a
surround of `#101010`, and Neon panels `#150D14` = (21, 13, 20) give `#151515`.
The average of Neon's channels would be 18, so this is the red channel and not
a desaturation.

The monitor also keeps that gray from when it last fetched it. Switching palettes
with Premiere open repaints the panels but leaves the band on the previous
palette's value until Premiere restarts.

Where it is painted has not been found. A diagnostic build recorded every
distinct color arriving at nine separate entry points — the theme functions, the
Direct2D brush and pen factories, `UIF::DC::FillRect` and `FrameRect` in both
their color overloads, the background-erase path, and the GPU fill in
`GPUFoundation.dll` — 181 distinct (entry point, color) pairs, and the gray
appeared at **none** of them. `UIF::DC` has a third `FillRect` that takes a
named decal and no color at all, which would explain the absence; confirming
that means reading an undocumented skin table, and guessing at one inside an
editor holding unsaved work is not a trade worth making for a band.

So it stays as Premiere paints it, and this readme says so instead of the mod
patching blind. If you use a tinted palette and the band bothers you, the
near-black palettes do not have it.

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

Settings apply while Premiere is running: the colours it already holds are
rewritten in place and the window repaints. Some surfaces keep the previous
palette until Premiere restarts, because Premiere copies those colours into
caches of its own.

If a Premiere update ever makes a panel misbehave with the mod on, switch off
**Premiere interface**, **Direct fills** and **GDI surfaces** together and
restart Premiere. With all three off when the mod loads, it does not hook
Premiere's own modules at all.
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
  $description: Menu and title bar text. Disabled items use the tone halfway between it and the panel.
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

#include <windhawk_utils.h>

#include <stdint.h>
#include <algorithm>
#include <bit>
#include <cmath>
#include <cstring>
#include <cwchar>
#include <optional>
#include <unordered_map>
#include <utility>
#include <vector>

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

    The set only changes when a module is mapped. It is snapshot once at init,
    then appended to from a loader notification, and the question becomes a
    pointer comparison against a short list of ranges.
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
    Writers are serialised by g_moduleRangeLock; readers take no lock at all.
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

    if (!known && count < static_cast<LONG>(kMaxModuleRanges)) {
        g_moduleRanges[count] = {begin, end};
        InterlockedExchange(&g_moduleRangeCount, count + 1);
    }

    ReleaseSRWLockExclusive(&g_moduleRangeLock);
}

/*
    Every module mapped after init, including the ones LoadLibraryExW never
    returns.

    LoadLibraryExW hands back only the module that was asked for. A DLL brings
    its dependencies in with it, and in Premiere most dva* modules arrive that
    way, as imports of something else, so noting what LoadLibraryExW returned
    missed them. The loader notification sees every mapping and carries the
    name, base and size with it, so nothing here asks the loader anything.

    It runs with the loader lock held, which is also what serialises it
    against itself. It is unregistered in Wh_ModUninit, before the image that
    contains it goes away.
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
               L"dva modules loaded later will not be recognised");
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

    The list grows to whatever the process reports. Premiere 2026 runs with
    more than 560 modules, past the 512 a fixed array used to hold, and the
    modules beyond that point were never noted. Sizes come from
    K32GetModuleInformation rather than from reading the module's headers, so
    a module unloaded in between costs a failed call, not an access violation.
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
               L"will be recognised as Adobe UI");
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
                   L"from now on will be recognised as Adobe UI",
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

/*
    A Premiere 2026 session measured 12 slots in use, logged at unload. The key
    is an address the theme hands out, and the theme holds few color objects.
    1024 leaves room for many more panels and versions, and if the table ever
    fills, StoreColor returns nothing and the caller keeps the original color.
*/
constexpr size_t kSlotCount = 1024;  // power of two, so the mask is a single &

/*
    Which hook handed a slot's address out, so that a settings change knows
    which setting the slot answers to. The same address can arrive through
    both, which is why this is a mask rather than a single value.
*/
constexpr LONG kFromThemeFunction = 1;  // ConvertColorRef — "Premiere interface"
constexpr LONG kFromEraseColor = 2;     // StableConvert — "Direct fills"

struct ColorSlot {
    volatile LONG state;       // 0 free, 1 being filled, 2 ready
    volatile LONG sources;     // kFromThemeFunction | kFromEraseColor
    volatile LONG generation;  // the settings generation dst was computed under
    uintptr_t key;
    DvaColorRGBA src;
    DvaColorRGBA dst;
};

/*
    Deliberately heap-allocated and deliberately leaked.

    The pointer this table returns is handed to Premiere and kept by it — the
    comments on ConvertColorRef and StableConvert spell out that contract. A
    static array would live in the mod image, and Windhawk unmaps that image on
    unload. The references Premiere still holds would then point at unmapped
    memory.

    One table of about 56 KB leaks each time the mod is unloaded — disabled,
    or updated. A settings change does not unload it (see
    Wh_ModSettingsChanged), so trying out palettes costs nothing here. That is
    the price of handing out a pointer whose lifetime the mod does not
    control, and it is the same trade already made for the GetSysColorBrush
    brushes.
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
    Whether an address is a colour this table actually converted.

    A slot whose setting is off holds its original colour again (see
    RefreshSlot), and that colour is raw: the brush layer has to treat it like
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

    A colour another thread was converting at the moment of the change was
    computed under the old settings, and Premiere may keep the slot it lands
    in without ever asking again. So a write computed under an older
    generation never replaces a slot the change already recomputed, and a
    slot that was still being filled when the change ran is recomputed by the
    thread filling it (see StoreColor). Neither can leave a colour Premiere
    holds on the previous palette.
*/
volatile LONG g_generation = 0;

static void RememberProduced(const DvaColorRGBA& c);

/*
    Recomputes one slot's dst from its src under the settings in force now.
    With restoreOriginals, or with neither setting the slot answers to on,
    dst goes back to src.
*/
static void RefreshSlot(ColorSlot& slot, LONG generation, bool restoreOriginals) {
    LONG sources = slot.sources;

    bool active = !restoreOriginals &&
                  (((sources & kFromThemeFunction) && g_settings.dvauiHook) ||
                   ((sources & kFromEraseColor) && g_settings.brushHook));

    // ConvertDvaColor leaves dst untouched when it declines.
    DvaColorRGBA dst = slot.src;

    if (active && ConvertDvaColor(slot.src, &dst)) {
        RememberProduced(dst);
    }

    slot.dst = dst;
    slot.generation = generation;
}

static const DvaColorRGBA* StoreColor(uintptr_t key, const DvaColorRGBA& src,
                                      const DvaColorRGBA& dst, LONG generation,
                                      LONG source) {
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
                slot.sources = source;
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

        if (!(slot.sources & source)) {
            InterlockedOr(&slot.sources, source);
        }

        LONG current = slot.generation;

        // Computed under older settings than the slot already holds.
        if (generation < current) {
            return &slot.dst;
        }

        /*
            The address was reused, the theme changed the colour behind it, or
            the settings changed since dst was computed. These are plain
            writes, and a drawing thread may be reading dst at the same moment;
            the worst case is one frame painted in a colour that is half the
            old one and half the new.
        */
        if (current != generation || !SameColor(slot.src, src)) {
            slot.src = src;
            slot.dst = dst;
            slot.generation = generation;
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
    return FibonacciIndex<kProducedSlots>(static_cast<uint32_t>(key));
}

/*
    Called for every colour a theme function hands out, and almost every one of
    them is already in the set — so each slot is read plainly first, and the
    lock cmpxchg only happens to claim an empty one. The reasoning is the same
    as in IsProducedColor below.
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
    // Read before the settings are, so a change in between is caught as stale.
    LONG generation = g_generation;

    if (!original || !g_settings.dvauiHook || IsOurSlot(original)) {
        return original;
    }

    DvaColorRGBA converted{};

    if (!ConvertDvaColor(*original, &converted)) {
        return original;
    }

    RememberProduced(converted);

    const DvaColorRGBA* stored =
        StoreColor(reinterpret_cast<uintptr_t>(original), *original, converted,
                   generation, kFromThemeFunction);

    return stored ? stored : original;
}

/*
    Brings every colour already handed out in line with the current settings.

    Premiere keeps the references this table returns, so a settings change
    that only reached new requests would leave everything Premiere already
    holds on the old palette. Rewriting dst in place reaches those too: the
    address Premiere holds stays the same, the colour behind it changes.

    With restoreOriginals — on unload — every slot goes back to the colour it
    replaced. The table outlives the mod, and without this the surfaces still
    pointing into it would keep the last palette frozen after the mod is gone.

    A slot another thread is filling at this moment (state 1) is skipped; that
    thread brings it up to date itself when it publishes it — see StoreColor.
    Returns how many slots are in use.
*/
static size_t RecomputeColorTable(bool restoreOriginals) {
    /*
        The produced set describes what the current settings produce. Left as
        it was, it would make a colour from the previous palette look already
        converted and let it through untouched, so it is emptied here and
        refilled from the table below.
    */
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
    bool alternate = false;  // an older spelling of a name listed above it
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
     L"GetApplicationBackgroundColor (pre-2026)", true},
    {"?GetContentBackgroundColor@utilities@skins@dvaui@@YAAEBVColorRGBA@draw"
     "bot@3@PEBVThemeProvider@ui@3@V?$intrusive_ptr@VSkinSet@core@skins@dvau"
     "i@@@boost@@@Z",
     L"GetContentBackgroundColor (pre-2026)", true},
    {"?GetListBoxBackgroundColor@utilities@skins@dvaui@@YAAEBVColorRGBA@draw"
     "bot@3@PEBVThemeProvider@ui@3@V?$intrusive_ptr@VSkinSet@core@skins@dvau"
     "i@@@boost@@@Z",
     L"GetListBoxBackgroundColor (pre-2026)", true},
    {"?GetHoverBackgroundColor@utilities@skins@dvaui@@YAAEBVColorRGBA@drawbo"
     "t@3@PEBVThemeProvider@ui@3@V?$intrusive_ptr@VSkinSet@core@skins@dvaui@"
     "@@boost@@@Z",
     L"GetHoverBackgroundColor (pre-2026)", true},
    {"?GetTabBackgroundColor@utilities@skins@dvaui@@YAAEBVColorRGBA@drawbot@"
     "3@PEBVThemeProvider@ui@3@V?$intrusive_ptr@VSkinSet@core@skins@dvaui@@@"
     "boost@@@Z",
     L"GetTabBackgroundColor (pre-2026)", true},
    {"?GetThumbnailBackgroundColor@utilities@skins@dvaui@@YAAEBVColorRGBA@dr"
     "awbot@3@PEBVThemeProvider@ui@3@V?$intrusive_ptr@VSkinSet@core@skins@dv"
     "aui@@@boost@@@Z",
     L"GetThumbnailBackgroundColor (pre-2026)", true},
    {"?GetDefaultControlColor@utilities@skins@dvaui@@YAAEBVColorRGBA@drawbot"
     "@3@PEBVThemeProvider@ui@3@V?$intrusive_ptr@VSkinSet@core@skins@dvaui@@"
     "@boost@@_N@Z",
     L"GetDefaultControlColor (pre-2026)", true},
    {"?GetInteractiveControlColor@utilities@skins@dvaui@@YAAEBVColorRGBA@dra"
     "wbot@3@PEBVThemeProvider@ui@3@V?$intrusive_ptr@VSkinSet@core@skins@dva"
     "ui@@@boost@@@Z",
     L"GetInteractiveControlColor (pre-2026)", true},
    {"?GetDividerColor@utilities@skins@dvaui@@YAAEBVColorRGBA@drawbot@3@PEBV"
     "ThemeProvider@ui@3@V?$intrusive_ptr@VSkinSet@core@skins@dvaui@@@boost@"
     "@@Z",
     L"GetDividerColor (pre-2026)", true},
    {"?GetListBoxBorderColor@utilities@skins@dvaui@@YAAEBVColorRGBA@drawbot@"
     "3@PEBVThemeProvider@ui@3@V?$intrusive_ptr@VSkinSet@core@skins@dvaui@@@"
     "boost@@@Z",
     L"GetListBoxBorderColor (pre-2026)", true},
    {"?GetFieldBorderColor@utilities@skins@dvaui@@YAAEBVColorRGBA@drawbot@3@"
     "PEBVThemeProvider@ui@3@V?$intrusive_ptr@VSkinSet@core@skins@dvaui@@@bo"
     "ost@@@Z",
     L"GetFieldBorderColor (pre-2026)", true},
    {"?GetScrollBarThumbColor@utilities@skins@dvaui@@YAAEBVColorRGBA@drawbot"
     "@3@PEBVThemeProvider@ui@3@V?$intrusive_ptr@VSkinSet@core@skins@dvaui@@"
     "@boost@@_N@Z",
     L"GetScrollBarThumbColor (pre-2026)", true},
    {"?GetScrollBarTrackColor@utilities@skins@dvaui@@YAAEBVColorRGBA@drawbot"
     "@3@PEBVThemeProvider@ui@3@V?$intrusive_ptr@VSkinSet@core@skins@dvaui@@"
     "@boost@@_N@Z",
     L"GetScrollBarTrackColor (pre-2026)", true},
    {"?GetWidgetColor@utilities@skins@dvaui@@YAAEBVColorRGBA@drawbot@3@PEBVT"
     "hemeProvider@ui@3@V?$intrusive_ptr@VSkinSet@core@skins@dvaui@@@boost@@"
     "_N@Z",
     L"GetWidgetColor (pre-2026)", true},

};

constexpr size_t kColorSymbolCount = ARRAYSIZE(kColorSymbols);

static int g_colorHooksInstalled = 0;
static int g_colorHooksMissing = 0;

/*
    Every hook into Premiere's modules goes in through here: looked up by its
    mangled name, registered, and logged when the running version lacks it.
    The counted ones are the color entry points the startup log reports on.
*/
struct HookSpec {
    const char* mangled;
    void* hook;
    void** original;
    const wchar_t* label;
};

static bool InstallHook(HMODULE module, const HookSpec& spec, bool counted) {
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

    if (counted) {
        if (installed) {
            g_colorHooksInstalled++;
        } else {
            g_colorHooksMissing++;
        }
    }

    return installed;
}

template <size_t N>
static void InstallHooks(HMODULE module, const HookSpec (&specs)[N], bool counted) {
    for (const HookSpec& spec : specs) {
        InstallHook(module, spec, counted);
    }
}

static void InstallOneColorHook(HMODULE dvaui, size_t index, void* hook,
                                void** original) {
    const ColorSymbol& sym = kColorSymbols[index];

    /*
        An older spelling only exists where the newer name listed above it is
        missing, and that name was already counted absent. So a missing
        alternate is not counted again, and one that resolves takes the
        absence back: the log reports functions, not spellings.
    */
    bool installed = InstallHook(dvaui, {sym.mangled, hook, original, sym.label},
                                 !sym.alternate);

    if (sym.alternate && installed) {
        g_colorHooksInstalled++;
        g_colorHooksMissing--;
    }
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

    Much of the monitor and timeline chrome is painted directly, without
    consulting any theme, and there is no color function to intercept for it in
    MediaCoreUI.dll or dvaplayer.dll. What does exist is the place most solid
    fills pass through, no matter who picked the color:

        dvaui::drawbot::d2d::OSSupplier::NewBrush(const ColorRGBA&)

    Most, not all: the band around the video in the monitors never arrives
    here — see Known limitations in the readme.

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
    Not every dark gray on screen is interface.

    A color swatch shows a color the user chose: #373737 in the color picker is
    a value being edited, not a panel. The brush hooks decide by color alone, so
    they painted it in the palette — the value itself was never changed, but the
    swatch showed a color that was not there, which is the one thing a color
    picker must not do.

    Color cannot tell the two apart, so the origin does. While a swatch or a
    parameter color is being drawn, the thread doing it paints what it is given.
    Interface colors drawn inside still arrive themed, because the theme
    functions converted them before they got here; only raw colors are left
    alone.
*/
/*
    The depth alone cannot be trusted to come back down. An exception thrown by
    Adobe's code under a content hook and caught above it unwinds through the
    hook's frame, and the unwinder this mod is built with is not guaranteed to
    run destructors for an exception that is not its own. A skipped
    ~ContentScope would leave the thread passing every colour through for the
    rest of the session.

    So the outermost scope also records the frame it was opened in. Anything
    running inside it is deeper on the same stack; a check made from higher up
    can only mean the scope is gone, and clears it. The hook passes its own
    frame in, rather than the scope using its address or reading the frame
    itself: two scopes in one frame then compare equal, and the answer does not
    depend on whether the constructor was inlined.
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
    return g_settings.brushHook || g_settings.gdiHook;
}

/*
    The order of these checks is the difference between a light mod and a heavy one.

    The overwhelming majority of colors arriving here will not be converted: they
    are saturated, or too light. Rejecting those first, with float comparisons,
    keeps the hash lookup off the common path — it only runs for the few colors that
    actually become background.
*/
static bool ConvertForPaint(const DvaColorRGBA* in, DvaColorRGBA* out) {
    if (!g_settings.brushHook || !in || InContentScope()) {
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
    Some entry points are handed a color reference to KEEP, not to read and drop.

    A brush reads the color while it is being built, so a stack temporary is
    safe there. A node manager that is told "erase your background with this
    color" may hold the reference for as long as the panel lives. Handing it a
    temporary would leave it pointing at reclaimed stack.

    So these go through the slot table, the same one the theme functions use:
    the returned pointer is stable for the life of the process.
*/
static const DvaColorRGBA* StableConvert(const DvaColorRGBA* in) {
    // Read before the settings are, as in ConvertColorRef.
    LONG generation = g_generation;

    if (!g_settings.brushHook || !in || IsConvertedSlot(in) || InContentScope()) {
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
        StoreColor(reinterpret_cast<uintptr_t>(in), *in, converted, generation,
                   kFromEraseColor);

    return stored ? stored : in;
}

/*
    Some panels erase their own background before their children draw, with a
    color handed over by reference:

        OS_NodeManager::ScEnableEraseBackgroundDrawing(OS_NodeManager*, const ColorRGBA&)
        UI_Node::UI_DispatchDrawFromRoot(const ColorRGBA&, Drawbot*, bool)

    The first is a scope guard that switches background erasing on with a color;
    the second draws a node subtree over one. The color they are handed has not
    necessarily been through a theme color function, and neither builds a brush,
    so both are hooked here — and both get a stable pointer, because either may
    keep the reference.
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

    Fills that go through here never ask the theme for their color, which is why
    the theme hooks alone left them stock. The band around the video in the
    monitors is not one of them: a diagnostic build logged every color arriving
    at these entry points, and that gray was never among them — see Known
    limitations in the readme.

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
    The other FillRect, and the fills the mod deliberately leaves alone.

    UIF::DC has two of each drawing primitive: one taking dvaui's ColorRGBA, and
    one taking ASL::ParamColor<unsigned char> — a parameter color, the 8-bit
    color an effect or a clip carries. Every module that calls the ParamColor
    overload is a panel showing user content: Effect Controls, the Timeline,
    both monitors, Lumetri, Essential Graphics, the Project panel, the timecode
    display. A diagnostic build that logged every color arriving here in four
    palettes saw only content colors and colors already derived from the
    palette, never an interface gray.

    Converting here could therefore only ever repaint content: a dark gray
    parameter shown in the palette instead of as itself. So these fills pass
    their color through untouched, and run inside a ContentScope, so whatever
    they call further in does not convert it either.

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

void UifFillRectParam_Hook(void* self, const void* rect, const void* color,
                           unsigned char flags) {
    ContentScope scope(__builtin_frame_address(0));
    UifFillRectParam_Original(self, rect, color, flags);
}

void UifFrameRectParam_Hook(void* self, const void* rect, const void* color) {
    ContentScope scope(__builtin_frame_address(0));
    UifFrameRectParam_Original(self, rect, color);
}

static void InstallUifHooks(HMODULE uif) {
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

    InstallHooks(uif, specs, true);
}

static void InstallBrushHooks(HMODULE dvaui) {
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

    InstallHooks(dvaui, specs, true);
}

/*
    The swatch controls, drawn inside a ContentScope — see the comment above it.

    UI_Swatch::UI_Draw is the one every swatch goes through, whichever skin
    version paints it: the color picker's web-safe swatch, marker colors and
    Essential Graphics all build theirs from UI_Swatch. The picker's larger chip
    is a class of its own, handled below. The skin-level draws are hooked as
    well, so a swatch painted through its skin by something other than a
    UI_Swatch — the swatch pair control is one candidate — is covered too, and
    popup menus draw label colors through their own swatch function.

    All of them take pointers only, five at most with the fifth on the stack, so
    one thunk forwarding five integer arguments carries each of them through
    unchanged; a function that takes fewer ignores the extra ones.
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

    InstallHook(dvaui, {sym.mangled, hook, original, sym.label}, false);
}

template <size_t... I>
static void InstallContentHooksImpl(HMODULE dvaui, std::index_sequence<I...>) {
    (InstallOneContentHook(dvaui, I,
                           reinterpret_cast<void*>(&ContentDrawHook<I>::Hook),
                           reinterpret_cast<void**>(&ContentDrawHook<I>::original)),
     ...);
}

/*
    Content views that export no draw function of their own.

    The color picker's chip — the new color over the original one — is a
    ColorPickerChipView, and neither its draw function nor its vtable is
    exported. What is exported is the function every node is drawn through,
    UI_Node::UI_DrawSelf, and UI_DrawAndCache for nodes drawn into a cache. Those
    are hooked, and the node being drawn is recognised by its class.

    The class comes from MSVC's RTTI, which Adobe's modules carry. The slot before
    a vtable points at the complete object locator, and the locator at the type
    descriptor that holds the decorated name of the most-derived class. Every step
    is checked against the image it must lie in — the locator's own RVA has to
    lead back to the module it sits in — so anything that does not look like that
    is simply not content. The answer is cached per vtable, so each class is
    looked up once; every other node pays a hash and a compare.
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

    InstallHooks(dvaui, specs, false);
}

/*
    The two modules do not necessarily load together, so each is tracked on its
    own and hooked when it arrives — once, by whichever thread gets there
    first. Premiere loads libraries from several threads at startup, and a
    plain flag read and then set would let two of them register the same hooks.
*/
volatile LONG g_dvauiHooked = FALSE;
volatile LONG g_uifHooked = FALSE;

static bool Claim(volatile LONG* flag) {
    return InterlockedCompareExchange(flag, TRUE, FALSE) == FALSE;
}

/*
    Registers the hooks for whichever of the two modules is loaded now and not
    done yet.

    Returns whether anything was registered, so the caller knows whether an
    apply is needed. Nothing here applies the operations itself: at init
    Windhawk applies them when Wh_ModInit returns, and from the loader hook the
    caller applies them once for the module that just arrived.

    Once in, a hook stays in and checks its own setting every time it runs
    (ConvertColorRef tests dvauiHook, ConvertForPaint and StableConvert test
    brushHook), so one whose setting is off only forwards the call it received.
    That is what lets a setting be switched off and on again without the mod
    being reloaded — see Wh_ModSettingsChanged.

    They only go in while a setting that works through these modules is on.
    With "Premiere interface", "Direct fills" and "GDI surfaces" all off when
    the mod loads, dvaui and UIFramework are not touched at all: the way back
    to a working Premiere if an update ever breaks one of these hooks.
    Switching one of them on later hooks the modules then.
*/
static bool WantsPremiereHooks() {
    return g_settings.dvauiHook || g_settings.brushHook || g_settings.gdiHook;
}

static bool HookLoadedModules() {
    if (!WantsPremiereHooks()) {
        return false;
    }

    bool registered = false;

    HMODULE dvaui = GetModuleHandleW(L"dvaui.dll");

    if (dvaui && Claim(&g_dvauiHooked)) {
        g_colorHooksInstalled = 0;
        g_colorHooksMissing = 0;

        InstallColorHooksImpl(dvaui, std::make_index_sequence<kColorSymbolCount>{});
        InstallBrushHooks(dvaui);
        InstallContentHooksImpl(dvaui, std::make_index_sequence<kContentDrawCount>{});
        InstallNodeDrawHooks(dvaui);

        registered = true;

        if (g_colorHooksInstalled) {
            Wh_Log(L"dvaui: %d hooks active, %d absent in this version",
                   g_colorHooksInstalled, g_colorHooksMissing);
        } else {
            Wh_Log(L"no dvaui color entry point matched — this Premiere version "
                   L"is not supported by the interface layer. The window frame "
                   L"and menus still apply.");
        }
    }

    HMODULE uif = GetModuleHandleW(L"UIFramework.dll");

    if (uif && Claim(&g_uifHooked)) {
        InstallUifHooks(uif);
        registered = true;
    }

    return registered;
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

    /*
        The question is not whether the module just returned is dvaui, but
        whether dvaui is loaded now. A DLL brings its dependencies in with it,
        and those never come back from LoadLibraryExW on their own: when dvaui
        arrives as an import of whatever Premiere asked for, comparing the
        returned handle would miss it for good. Once both are hooked, or while
        no setting needs them, this is a couple of plain reads.
    */
    if ((!g_dvauiHooked || !g_uifHooked) && HookLoadedModules() &&
        !Wh_ApplyHookOperations()) {
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
    Turns the process-wide app mode on or off.

    Split out from the lookups below so that a settings change can flip it
    without the mod being torn down and rebuilt. This is the one part of the
    native dark mode that is not re-checked at call time — it is a state Windows
    holds, not a branch in a hook.

    Only a mode this mod set is ever undone. With "Window and system dialogs"
    off from the start, nothing is called at all — not even to set Default,
    which would still overwrite a mode something else in the process chose.
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
    ApplyAppMode(g_settings.nativeDarkMode);
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
    The windows this mod actually changed, and what it changed on each.

    The revert has to undo what the apply did and nothing more. Walking every
    window in the process instead would clear the theme class of windows the
    mod never touched — a common dialog, a CEF panel, a control Premiere
    themed itself — and force immersive dark mode off on frames that may have
    turned it on for themselves. So the apply records each window here, and
    the revert walks this and nothing else.

    Entries are not removed when a window is destroyed: nothing on that path
    is hooked, and hooking it for this would cost every window in the process.
    Instead the map drops dead handles whenever it doubles, and the revert
    re-checks each handle before touching it.
*/
constexpr BYTE kThemedClass = 1;  // SetWindowTheme(DarkMode_Explorer)
constexpr BYTE kThemedFrame = 2;  // immersive dark mode, and caption colours on 22000+

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
    if (!hwnd || !g_settings.nativeDarkMode) {
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
        Title bar and border only exist on a top-level window, and GetParent is
        the wrong way to ask: for a WS_POPUP it returns the OWNER, so Premiere's
        owned dialogs and floating panels — which do have captions — were being
        treated as children and kept the default colours. The style bit answers
        the question that was actually being asked.

        Immersive dark mode is a frame attribute as well, so it waits for the
        same test: a child window has no frame for DWM to apply it to.
    */
    if (!(GetWindowLongPtrW(hwnd, GWL_STYLE) & WS_CHILD)) {
        BOOL dark = TRUE;

        DwmSetWindowAttribute(hwnd,
                              static_cast<DWMWINDOWATTRIBUTE>(ImmersiveDarkModeAttribute()),
                              &dark, sizeof(dark));

        const Palette& p = g_settings.palette;
        SetFrameColors(hwnd, p.ramp[4], p.ramp[1], p.text);

        applied |= kThemedFrame;
    }

    RememberThemedWindow(hwnd, applied);
}

/*
    Undoes exactly what ApplyDarkModeToWindow recorded for one window.

    Immersive dark mode goes back to FALSE, and for these windows that is the
    value they had rather than an override: it is only recorded on windows the
    mod set it on, and nothing in Premiere sets it on its own. In Premiere 2026
    the executable, dvaui.dll, dvacore.dll, UIFramework.dll and PlugPlug.dll do
    not reference DwmSetWindowAttribute at all, and none of the process's
    top-level windows is a Chromium frame — so the frames the mod turned dark
    were light before it did. The caption colours go back to
    DWMWA_COLOR_DEFAULT, which hands them to the system.

    SetWindowTheme(nullptr, nullptr) clears the theme class rather than
    restoring an earlier one, because there is no earlier one to restore: the
    apply replaced it, and uxtheme has no call that reads it back. That is why
    only windows the apply changed, and nobody has re-themed since (see
    ForgetThemedClass), are cleared.

    AllowDarkModeForWindow is left as it is. It is a permission, not a colour —
    it only has an effect while the app mode allows dark — and the app mode is
    handed back alongside this.
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

    With the setting on, the apply reaches every window Premiere creates, so
    this is still a couple of hundred windows — about 230 in a running
    Premiere 2026 with a project open — and SetWindowTheme sends each one
    WM_THEMECHANGED on its own thread. That cost is paid once per disable; a
    settings change that leaves the frame on never comes here.

    The map is emptied before any window is touched. SetWindowTheme waits for
    the window's thread to answer, and if that thread were at that moment
    creating a window and waiting for g_themedLock in RememberThemedWindow,
    holding the lock across the send would deadlock the two.
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
    A palette change with the frame on. Only the caption colours depend on the
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

    const Palette& p = g_settings.palette;

    for (const auto& [hwnd, applied] : windows) {
        if ((applied & kThemedFrame) && IsOwnWindow(hwnd)) {
            SetFrameColors(hwnd, p.ramp[4], p.ramp[1], p.text);
        }
    }
}

/*
    Asks every window of the process to repaint, frame included.

    Needed after a settings change and on unload: the colour table has just
    been rewritten under Premiere, and the menu bar and the 1px line under it
    are non-client area that nothing else would repaint. RedrawWindow only
    invalidates here — each window paints on its own thread when it next gets
    to it — so this waits on no UI thread.

    RDW_ERASE too: native dialogs paint their background in WM_ERASEBKGND, and
    without it their controls would repaint over the previous palette.
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
    GetSysColorBrush promises that the brush it returns stays valid for the
    life of the process, and Premiere keeps running after the mod goes away —
    so a brush this mod hands out is never deleted, on unload or on a palette
    change.

    Keyed by colour rather than by index: a palette change then adds only the
    handful of colours the new palette uses, switching back to a palette
    already seen adds nothing, and the cap keeps endless editing of a custom
    palette from running the process out of GDI handles. Past the cap the
    system brush is returned instead.
*/
struct SysBrush {
    COLORREF color;
    HBRUSH brush;
};

constexpr size_t kMaxSysBrushes = 256;

SysBrush g_sysBrushes[kMaxSysBrushes]{};
size_t g_sysBrushCount = 0;
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

static HBRUSH FindSysBrush(COLORREF color) {
    for (size_t i = 0; i < g_sysBrushCount; i++) {
        if (g_sysBrushes[i].color == color) {
            return g_sysBrushes[i].brush;
        }
    }

    return nullptr;
}

/*
    The shared lock covers the common case — a colour already cached — so
    concurrent FillRect calls with a system colour do not queue behind one
    another. Only a colour seen for the first time takes the exclusive lock.
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

    if (!brush && g_sysBrushCount < kMaxSysBrushes) {
        brush = CreateSolidBrush(mapped);

        if (brush) {
            g_sysBrushes[g_sysBrushCount] = {mapped, brush};
            g_sysBrushCount++;
        }
    }

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

    bool hasText = label[0] != L'\0';

    /*
        Everything that can fail is resolved before anything is painted.
        Filling first and failing after would hand the item back to
        DefWindowProc, which then paints it again on top of the fill.
    */
    DrawThemeTextEx_t drawText = nullptr;
    HTHEME theme = nullptr;

    if (hasText) {
        drawText = ResolveDrawThemeTextEx();
        theme = MenuBarTheme(hwnd);

        if (!drawText || !theme) {
            return false;
        }
    }

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
        An item with no text is an MDI window caption button, drawn with an icon
        font glyph. Returning true here skips DefWindowProc, so that glyph is not
        drawn at all — only the background is. Premiere has no MDI child
        windows, so there is no such button to lose; a host that had one would
        need the glyph drawn here.
    */
    if (!hasText) {
        return true;
    }

    /*
        Only a genuinely disabled item is dimmed. An unfocused window used to
        count as disabled here, which meant alt-tabbing away greyed the whole
        File/Edit/Clip bar — Windows does not do that, and on a second monitor
        the bar would sit there looking permanently disabled.
    */
    bool disabled = (state & (ODS_GRAYED | ODS_DISABLED)) != 0;

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

/*
    WS_CHILD is tested before GetMenu, whose answer is undefined for a child
    window: in practice it is the control ID, so a child with a nonzero ID
    would pass, and its WM_DESTROY would close the thread's cached menu bar
    theme.
*/
static bool NeedsMenuBarWork(HWND hwnd, UINT msg) {
    return g_settings.menuHook && IsMenuBarMessage(msg) && hwnd &&
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

/*
    An app can install its own background brush on a menu. If that brush is light,
    the menu goes light again underneath everything we did. Replacing the brush here
    is cheaper than working out later why one single menu came back white.
*/
using SetMenuInfo_t = BOOL(WINAPI*)(HMENU, LPCMENUINFO);

SetMenuInfo_t SetMenuInfo_Original = nullptr;

BOOL WINAPI SetMenuInfo_Hook(HMENU menu, LPCMENUINFO info) {
    /*
        Only a MENUINFO of exactly the documented size is copied. Trusting a
        larger cbSize would read past what the caller actually passed; anything
        else is forwarded untouched for SetMenuInfo to judge.
    */
    if (!g_settings.menuHook || !info || info->cbSize != sizeof(MENUINFO) ||
        !(info->fMask & MIM_BACKGROUND)) {
        return SetMenuInfo_Original(menu, info);
    }

    HBRUSH brush = ThemeSysBrush(COLOR_MENU);

    if (!brush) {
        return SetMenuInfo_Original(menu, info);
    }

    MENUINFO copy = *info;
    copy.hbrBack = brush;

    return SetMenuInfo_Original(menu, &copy);
}

// ============================================================================
// GDI SURFACES
// ============================================================================

static DvaColorRGBA GdiToDva(COLORREF color) {
    return {GetRValue(color) / 255.0f, GetGValue(color) / 255.0f,
            GetBValue(color) / 255.0f, 1.0f};
}

/*
    Adobe code often turns a colour that already came out of a theme function
    into a COLORREF before it builds a brush or pen from it. So the produced
    set is consulted here as on every other path: converting that colour again
    would push it toward the darkest stop, and Onyx's border would land on the
    surface tone. PackColorKey quantises to 8 bits, so a COLORREF compares
    equal to the float colour it was made from.
*/
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

    // Asked only after the float tests, as in ConvertForPaint.
    if (IsProducedColor(GdiToDva(color))) {
        return color;  // already a palette colour
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

    COLORREF result = RGB(nr, ng, nb);

    RememberProduced(GdiToDva(result));

    return result;
}

using CreateSolidBrush_t = HBRUSH(WINAPI*)(COLORREF);
using CreatePen_t = HPEN(WINAPI*)(int, int, COLORREF);
using SetBkColor_t = COLORREF(WINAPI*)(HDC, COLORREF);

CreateSolidBrush_t CreateSolidBrush_Original = nullptr;
CreatePen_t CreatePen_Original = nullptr;
SetBkColor_t SetBkColor_Original = nullptr;

HBRUSH WINAPI CreateSolidBrush_Hook(COLORREF color) {
    if (g_settings.gdiHook && !InContentScope() &&
        IsAdobeUICaller(__builtin_return_address(0))) {
        color = ConvertGdiColor(color);
    }

    return CreateSolidBrush_Original(color);
}

HPEN WINAPI CreatePen_Hook(int style, int width, COLORREF color) {
    if (g_settings.gdiHook && !InContentScope() &&
        IsAdobeUICaller(__builtin_return_address(0))) {
        color = ConvertGdiColor(color);
    }

    return CreatePen_Original(style, width, color);
}

COLORREF WINAPI SetBkColor_Hook(HDC hdc, COLORREF color) {
    if (g_settings.gdiHook && !InContentScope() &&
        IsAdobeUICaller(__builtin_return_address(0))) {
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

        /*
            Disabled text has no setting of its own. Halfway from the text to
            the panel is close to where the built-in palettes put it, and it
            always lies between the two, which Onyx's #777777 would not once
            the custom text is darker than that.
        */
        p.dimText = RGB((GetRValue(p.text) + GetRValue(p.ramp[1])) / 2,
                        (GetGValue(p.text) + GetGValue(p.ramp[1])) / 2,
                        (GetBValue(p.text) + GetBValue(p.ramp[1])) / 2);
    } else {
        for (const NamedPalette& candidate : kPalettes) {
            if (wcscmp(name, candidate.id) == 0) {
                p = candidate.colors;
                break;
            }
        }
    }

    Wh_FreeStringSetting(name);

    /*
        Built in a local and published with a single assignment. Hooks on other
        threads read g_settings without a lock while a settings change runs, and
        filling it field by field would widen the window in which one of them
        sees half the old settings and half the new.
    */
    Settings next{};

    next.palette = p;

    next.strength = ClampFloat(Wh_GetIntSetting(L"strength") / 100.0f, 0.0f, 1.0f);

    next.ceiling = ClampFloat(Wh_GetIntSetting(L"ceiling") / 100.0f, 0.05f, 0.95f);

    next.dvauiHook = Wh_GetIntSetting(L"dvauiHook") != 0;
    next.brushHook = Wh_GetIntSetting(L"brushHook") != 0;
    next.nativeDarkMode = Wh_GetIntSetting(L"nativeDarkMode") != 0;
    next.menuHook = Wh_GetIntSetting(L"menuHook") != 0;
    next.gdiHook = Wh_GetIntSetting(L"gdiHook") != 0;

    g_settings = next;
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

BOOL Wh_ModInit() {
    LoadSettings();

    if (!AllocateSlots()) {
        Wh_Log(L"could not allocate the colour table; the interface layer will "
               L"pass colours through unchanged");
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
    HookOrLog(SetMenuInfo, SetMenuInfo_Hook, &SetMenuInfo_Original, L"SetMenuInfo");

    HookOrLog(CreateSolidBrush, CreateSolidBrush_Hook, &CreateSolidBrush_Original,
              L"CreateSolidBrush");
    HookOrLog(CreatePen, CreatePen_Hook, &CreatePen_Original, L"CreatePen");
    HookOrLog(SetBkColor, SetBkColor_Hook, &SetBkColor_Original, L"SetBkColor");

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
        HookOrLog(loadLibraryExW, LoadLibraryExW_Hook, &LoadLibraryExW_Original,
                  L"kernelbase!LoadLibraryExW");
    } else {
        Wh_Log(L"could not resolve kernelbase!LoadLibraryExW; a Premiere that "
               L"loads its UI modules after this point will not be themed");
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
        straight to the system and the windows come back with their own colours.
    */

    // The notification callback lives in this image, which is about to go.
    StopWatchingModuleLoads();

    /*
        The colour table outlives the mod, so every reference Premiere still
        holds into it gets its original colour back rather than the last
        palette. The count is logged so the table size can be judged against
        what a real session actually uses.
    */
    size_t used = RecomputeColorTable(true);

    Wh_Log(L"colour table: %u of %u slots in use", static_cast<unsigned>(used),
           static_cast<unsigned>(kSlotCount));

    RevertThemedWindows();
    ApplyAppMode(false);

    /*
        Menus opened while the hooks were swapping in DarkMode::Menu keep that
        theme cached in user32 until it is flushed. If the setting was switched
        off earlier, that change already flushed it.
    */
    if (g_settings.menuHook && g_FlushMenuThemes) {
        g_FlushMenuThemes();
    }

    RedrawProcessWindows();

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

/*
    Settings are applied in place; the mod is not reloaded.

    A reload would unload the image and leave behind what Premiere still holds
    from it — the colour table, the system brushes, a menu theme per UI thread —
    once per change, and trying out palettes is exactly the pattern that
    changes settings many times in a row. It would also do less: the colours
    Premiere took before the change would stay on the palette they were
    converted under.

    Every hook already checks its own setting each time it runs. What is left
    is the state that lives outside the hooks: the colour table, the process
    app mode, the window frames, the menu themes user32 caches, and a repaint
    so the new colours show.
*/
void Wh_ModSettingsChanged() {
    Settings previous = g_settings;

    LoadSettings();
    InterlockedIncrement(&g_generation);

    // A setting that works through Premiere's modules may just have come on.
    if (HookLoadedModules() && !Wh_ApplyHookOperations()) {
        Wh_Log(L"failed to apply hooks after a settings change");
    }

    RecomputeColorTable(false);

    ApplyAppMode(g_settings.nativeDarkMode);

    if (previous.nativeDarkMode && !g_settings.nativeDarkMode) {
        RevertThemedWindows();
    } else if (!previous.nativeDarkMode && g_settings.nativeDarkMode) {
        ApplyThemeToExistingWindows();
    } else if (g_settings.nativeDarkMode &&
               memcmp(&previous.palette, &g_settings.palette, sizeof(Palette)) !=
                   0) {
        RecolorThemedFrames();
    }

    // ApplyAppMode flushes these itself when the mode changes.
    if (previous.menuHook != g_settings.menuHook && g_FlushMenuThemes) {
        g_FlushMenuThemes();
    }

    RedrawProcessWindows();
}
