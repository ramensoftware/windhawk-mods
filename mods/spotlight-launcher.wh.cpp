// ==WindhawkMod==
// @id              spotlight-launcher
// @name            Spotlight Launcher
// @description     Application launcher. Alt+Space opens a floating search bar that finds your programs instantly.
// @version         0.6.0
// @author          David Ravelo
// @github          https://github.com/DavidRaveloU
// @include         windhawk.exe
// @compilerOptions -lole32 -lshell32 -luser32 -lgdi32 -ld2d1 -ldwrite -luuid -ladvapi32 -lwindowscodecs -lpropsys -lversion
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Spotlight Launcher

A fast, keyboard-driven application launcher for Windows, packaged as a [Windhawk](https://windhawk.net) tool mod.

Press **Alt + Space**, type a few letters, press **Enter**. That is the whole idea.

Spotlight Launcher indexes the same applications you see in the Start menu (both classic programs and Microsoft Store apps) and filters them as you type. It runs on its own, as a Windhawk mod, so it never runs inside File Explorer and cannot affect the way Windows works.

---

## Screenshots

### Main

![Spotlight Launcher in action](https://raw.githubusercontent.com/DavidRaveloU/Spotlight-Launcher-Windhawk/main/assets/Desktop.png)

The search bar over the desktop, with the default **Midnight** colour and **Acrylic** effect. A short query already filtered a handful of results, with the matching letters highlighted. This is the launcher's first impression.

### Details panel

![Details panel](https://raw.githubusercontent.com/DavidRaveloU/Spotlight-Launcher-Windhawk/main/assets/Details.png)

An app selected with the details panel open on the right (**Ctrl + I**): the actions list and the technical info table.

### Look and feel

These are some of the built-in looks (themes): the **Aurora** effect in three colours, plus **Frosted**, **Glass** and **Mica**.

| Theme | Preview |
| --- | --- |
| **Aurora** &middot; Amber | ![Aurora, amber](https://raw.githubusercontent.com/DavidRaveloU/Spotlight-Launcher-Windhawk/main/assets/Aurora%20Amber.png) |
| **Aurora** &middot; Emerald | ![Aurora, emerald](https://raw.githubusercontent.com/DavidRaveloU/Spotlight-Launcher-Windhawk/main/assets/Aurora%20Emerald.png) |
| **Aurora** &middot; Violet | ![Aurora, violet](https://raw.githubusercontent.com/DavidRaveloU/Spotlight-Launcher-Windhawk/main/assets/Aurora%20Violet.png) |
| **Frosted** | ![Frosted](https://raw.githubusercontent.com/DavidRaveloU/Spotlight-Launcher-Windhawk/main/assets/Frosted.png) |
| **Glass** | ![Glass](https://raw.githubusercontent.com/DavidRaveloU/Spotlight-Launcher-Windhawk/main/assets/Glass.png) |
| **Mica** | ![Mica](https://raw.githubusercontent.com/DavidRaveloU/Spotlight-Launcher-Windhawk/main/assets/Mica.png) |

The rest (the **Acrylic** and **Solid** effects and the remaining colours — Midnight blue, Graphite, Black, Teal, Rose, Crimson, Sky, Lavender, White) are covered below in the Features section.

---

## Features

### Instant, forgiving search

* Filters classic programs and Microsoft Store apps as you type.
* Case- and accent-insensitive.
* Multi-word queries (`visual code`) and acronyms (typing the initials of each word, like `vsc`).
* Matching letters are highlighted in the result.
* Results show a friendly name plus the file or package they open.

### Keyboard-first

Everything is reachable without touching the mouse.

| Shortcut | Action |
| --- | --- |
| **Alt + Space** | Open / close the search bar (configurable) |
| **↑ / ↓ / Tab** | Move the selection |
| **Enter** | Open the selected app |
| **Esc** | Close the details panel, then the search bar |
| **Ctrl + Enter** | Open the file location |
| **Ctrl + Shift + Enter** | Run as administrator |
| **Ctrl + Shift + C** | Copy the path |
| **Ctrl + P** | Properties |
| **Ctrl + Shift + R** | Remove from recents |
| **Ctrl + I** | Toggle the details panel (configurable) |
| **Ctrl + R** | Reindex the app list |
| Mouse wheel | Scroll the list without changing the selection |
| Mouse hover | Highlight the row under the cursor |

**Alt + Space** is also the system shortcut for a window's title-bar menu (Move, Size, Close…). While the mod is enabled with the default shortcut, that menu is not reachable by keyboard, so pick another combination in the settings if you use it.

### Details panel

Press **Ctrl + I** (or **Ctrl + K** / **Ctrl + Space** / **F4**) with an app selected to reveal a right-hand panel with actions and extra details. The actions also work with the panel closed. File actions are only offered for classic programs; Microsoft Store apps only offer *open* and *remove from recents*.

### Built-in calculator

Type a calculation and the result appears as you go: a highlighted card at the top of the list shows it, and it is also shown faintly next to what you typed. Press **Enter** to copy the result; recent results are kept so you can reuse them. It only reacts when the whole text looks like a calculation, so ordinary searches are left alone.

### Soft interface sounds

Gentle sounds play as you use the launcher: a soft switch-like click when the bar opens (like a flashlight button), a tiny tick when the details panel opens and closes, and a soft, deep key sound as you type (each key sounds slightly different). Everything is generated inside the mod, so it needs no extra files and no internet connection. You can turn the sounds off or change their volume in the settings.

### Appearance

* Six background effects (**Acrylic**, **Mica**, **Glass**, **Frosted**, **Aurora**, **Solid**) and 12 colours.
* Each effect is built from a blurred view of whatever is behind the bar, finished with a colour tint, subtle grain and light highlights, so the bar looks the same on Windows 10 and 11.
* **Live acrylic**: Acrylic, Glass, Frosted and Aurora follow what is behind the bar in real time. Turning it off uses a single snapshot taken when the bar opens, which helps performance and saves battery. It is on by default and can be changed in the settings.
* Adjustable panel opacity, blur strength and layout.
* Looks sharp on every monitor, even high-resolution ones or a mix of several.
* The bar can sit anywhere from against the top edge to against the bottom edge of the screen.

### Fresh without fuss

* **Automatic** index updates watch the Start menu folders and re-read the list when an app is installed or removed, with a light periodic fallback for Store apps.
* Can also re-index on every open or on a fixed interval (1 / 3 / 5 / 10 minutes).
* Installers write an "Uninstall …" shortcut for everything they install; the mod inspects what each shortcut actually runs and hides the uninstallers and documentation, while keeping real tools whose names merely contain "uninstall".
* If reindexing finishes with the search bar already open, the list updates by itself.
* Only what Windows lists as an app appears: a portable program without a shortcut in the Start menu is not indexed. A freshly installed app may take a few seconds to appear, until Windows registers its shortcut.

### Safe by design

* Runs as a Windhawk **tool** mod on its own, separate from File Explorer, so it cannot affect Windows itself.
* **Disable in fullscreen**: the hotkey stays quiet while a fullscreen window (a game, a video player, a fullscreen browser) is focused.
* If another app already owns the shortcut, the mod warns you instead of fighting over it.

---

## Installation

1. Install [Windhawk](https://windhawk.net) (Windows 10 or 11).
2. In Windhawk, open **Mods** and search for **Spotlight Launcher**, then click **Install**.
3. Press **Alt + Space** to open it. Change the shortcut, position, effect and colour from the mod's **Settings** tab.

### From source

1. Download [`spotlight-launcher.wh.cpp`](https://github.com/DavidRaveloU/Spotlight-Launcher-Windhawk/blob/main/spotlight-launcher.wh.cpp).
2. In Windhawk, choose **Create a new mod**, replace the editor contents with the file and **Save**.
3. Install it from your local mods list.

---

## Contributing

Contributions are welcome — bug reports, fixes, new features and translations alike. See the [repository](https://github.com/DavidRaveloU/Spotlight-Launcher-Windhawk) for details.

### Reporting a bug

Open an [issue](https://github.com/DavidRaveloU/Spotlight-Launcher-Windhawk/issues) and include:

* Windows build (`winver`) and Windhawk version.
* The mod version (shown in the README header / settings).
* Steps to reproduce it and what you expected instead.
* A screenshot or short clip when the problem is visual.

### Sending a pull request

1. Fork the repository and create a branch from `main`, e.g. `fix/acrylic-on-win10`.
2. Make your change in `spotlight-launcher.wh.cpp` (C++23, Windhawk tool mod).
3. Bump `@version` in the mod header when behaviour changes.
4. Confirm it compiles and test it on the Windows versions you can reach.
5. Open a pull request against `main` describing **what** changed and **why**.

Keep pull requests focused: one bug or feature per PR is much easier to review and merge.

---

## License

Released under the **MIT License**. See [LICENSE](https://github.com/DavidRaveloU/Spotlight-Launcher-Windhawk/blob/main/LICENSE).

Copyright (c) 2026-present David Ravelo.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- Hotkey: alt+space
  $name: Keyboard shortcut
  $description: Key combination that opens and closes the search bar.
  $options:
    - alt+space: Alt + Space
    - ctrl+space: Ctrl + Space
    - ctrl+alt+space: Ctrl + Alt + Space
    - shift+alt+space: Shift + Alt + Space
    - ctrl+shift+space: Ctrl + Shift + Space
- VerticalPosition: 30
  $name: Vertical position (percentage of screen height)
  $description: Where the search bar sits, as a percentage of the screen height. 0 sticks it to the very top, 50 centres it and 100 sits it just above the taskbar. In Compact mode only the search box appears at this height; in Expanded mode the bar grows downward from here, so raise or lower this value if you want it to look centred.
- MaxResults: 8
  $name: Maximum results
  $description: How many apps the search shows at once. It does not change the height of the bar; when there are more matches, you reach the rest by scrolling the list.
- Opacity: 90
  $name: Panel opacity (percentage)
  $description: How solid the colour of the bar is, from 70 to 100. A lower value lets more of the background show through and a higher value makes the colour stronger (Glass stays see-through in any case). Ignored with "Solid", which is always fully opaque.
- Effect: acrylic
  $name: Background effect
  $description: The background style of the bar. Acrylic, Mica, Glass, Frosted and Aurora show a blurred view of whatever is behind the bar, each with its own look; "Solid" shows a plain colour with no blur.
  $options:
    - acrylic: Acrylic
    - mica: Mica
    - glass: Glass
    - frosted: Frosted
    - aurora: Aurora
    - solid: Solid (no effect)
- LiveAcrylic: true
  $name: Live acrylic (real time)
  $description: Keeps the background blur live. Acrylic, Glass, Frosted and Aurora follow whatever is behind the bar in real time. Turn it off to use a single snapshot taken when the bar opens, which helps performance and saves battery on laptops and low-end machines. Needs Windows 10 1803 or newer.
- BlurAmount: 50
  $name: Blur strength (0 to 100)
  $description: How strong the blur is. 50 is the normal amount, lower values give a subtler blur and higher values a heavier one. The blur is seen through the colour, so it shows best with a Panel opacity below 100. Ignored with "Solid".
- Color: midnight
  $name: Color
  $description: Colour scheme of the bar. Each colour sets the background colour, the selection highlight and the colour of the matching letters.
  $options:
    - midnight: Midnight blue
    - graphite: Graphite
    - black: Black
    - violet: Violet
    - emerald: Emerald
    - teal: Teal
    - rose: Rose
    - crimson: Crimson
    - amber: Amber
    - white: White
    - sky: Sky
    - lavender: Lavender
- HideUtilityEntries: true
  $name: Hide uninstallers and documentation
  $description: Hides the "Uninstall …" shortcuts and the readme, help and web-link shortcuts that installers add to the Start menu. It checks what each shortcut really opens, so genuine tools whose names merely contain "uninstall" are kept.
- ReindexMode: auto
  $name: App index updates
  $description: How often the list of installed apps is refreshed. Automatic notices apps as they are installed or removed and stays idle when nothing changes, so it uses almost no resources (recommended).
  $options:
    - auto: Automatic
    - always: Every time it opens
    - min1: 1 minute
    - min3: 3 minutes
    - min5: 5 minutes
    - min10: 10 minutes
- ListMode: expanded
  $name: List mode
  $description: Compact shows only the search bar until you type. Expanded also shows your recent apps and suggestions while the search box is empty.
  $options:
    - compact: Compact
    - expanded: Expanded
- DetailsKey: ctrl+i
  $name: Details panel shortcut
  $description: Key that shows or hides the actions and details panel for the selected app. If you pick Ctrl + Space here, choose a different combination for the main shortcut so the two do not clash.
  $options:
    - ctrl+i: Ctrl + I
    - ctrl+k: Ctrl + K
    - ctrl+space: Ctrl + Space
    - f4: F4
- DisableInFullscreen: true
  $name: Disable in fullscreen
  $description: When on, the shortcut is ignored while a fullscreen window is focused (a game, a video player, a fullscreen browser). It also closes the bar if a fullscreen window takes over while the bar is open.
- CalculatorEnabled: true
  $name: Calculator
  $description: While you type, the text is also tried as a calculation (for example "12*8", "100+15%" or "sqrt(2)+pi") and the result is shown. It only reacts when the whole text looks like a maths expression, so ordinary searches are left alone. Press Enter to copy the result, which is also saved to a small history you can reuse.
- CalculatorStyle: both
  $name: Calculator display
  $description: How the result is shown. The card is a highlighted result at the top of the list, the inline text appears faintly right after what you typed, and "Both" shows the card and the inline text together.
  $options:
    - hero: Result card
    - ghost: Inline text
    - both: Card and inline text
- SoundEnabled: true
  $name: Interface sounds
  $description: Soft sounds when the bar opens, when the details panel opens or closes, and as you type. They are generated inside the mod, so they work with no internet connection.
- SoundVolume: 20
  $name: Sound volume (0 to 100)
  $description: How loud the interface sounds are. Ignored while interface sounds are turned off.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <winver.h>
#include <windowsx.h>
#include <shellapi.h>
#include <shobjidl.h>
#include <shlobj.h>
#include <propsys.h>
#include <wincodec.h>
#include <commoncontrols.h>
#include <sddl.h>

// windows.h defines DrawText as a macro (DrawTextW); it must be removed BEFORE d2d1.h
// so that ID2D1RenderTarget::DrawText keeps its name.
#ifdef DrawText
#undef DrawText
#endif

#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <xaudio2.h>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cwctype>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <cstdio>
#include <cwchar>
#include <cstring>

#ifndef LOAD_LIBRARY_SEARCH_SYSTEM32
#define LOAD_LIBRARY_SEARCH_SYSTEM32 0x00000800
#endif

// ===== Inline calculator engine =====
// A small self-contained arithmetic evaluator (tokenizer + recursive-descent parser). It is
// never fed to another interpreter: it only recognises numbers, the four operations, powers,
// percentages, factorials, a fixed set of functions and a few constants, so it is safe to run
// on whatever the user types. Detection is strict (the whole string must parse and look like
// maths) to avoid matching ordinary app names that merely contain numbers or symbols.
namespace Calc {

struct Locale {
    wchar_t decimal = L'.';
    wchar_t group = L',';
    bool grouping = true;
};

inline Locale SystemLocale() {
    Locale l;
    wchar_t buf[8] = {};
    if (GetLocaleInfoW(LOCALE_USER_DEFAULT, LOCALE_SDECIMAL, buf, ARRAYSIZE(buf)) > 1) {
        l.decimal = buf[0];
    }
    int n = GetLocaleInfoW(LOCALE_USER_DEFAULT, LOCALE_STHOUSAND, buf, ARRAYSIZE(buf));
    if (n > 1) {
        l.group = buf[0];
    } else {
        l.grouping = false;
    }
    if (l.group == l.decimal) {
        l.grouping = false;
    }
    return l;
}

enum class Status { NotMath, Ok, Error };

struct Result {
    Status status = Status::NotMath;
    double value = 0.0;
    std::wstring error;
    // When set, the caller shows this text instead of formatting `value` (unit results, base
    // conversions). It is not parseable by the evaluator, so `insertable` holds the plain form.
    std::wstring display;
    std::wstring insertable;
    // The input was just a constant such as "pi" or "ans". App matches take priority over it.
    bool loneConstant = false;
};

// Values supplied by the host: the previous result, so "ans" can be reused.
struct Context {
    bool hasAns = false;
    double ans = 0.0;
};

// A unit: canonical symbol for display, the dimension it belongs to, and the affine transform to
// and from the dimension's base unit (base = value * factor + offset).
struct UnitDef {
    const wchar_t* name;
    const wchar_t* sym;
    int dim;
    double factor;
    double offset;
};

// Dimensions (must match the `dim` field of the table below).
enum {
    DIM_LENGTH = 0,
    DIM_MASS,
    DIM_TIME,
    DIM_DATA,
    DIM_VOLUME,
    DIM_SPEED,
    DIM_AREA,
    DIM_ANGLE,
    DIM_PRESSURE,
    DIM_ENERGY,
    DIM_POWER,
    DIM_TEMP,
};

inline const std::vector<UnitDef>& UnitTable() {
    static const std::vector<UnitDef> t = {
        // ---- length (base: metre) ----
        {L"nm", L"nm", DIM_LENGTH, 1e-9, 0}, {L"um", L"µm", DIM_LENGTH, 1e-6, 0},
        {L"mm", L"mm", DIM_LENGTH, 1e-3, 0}, {L"cm", L"cm", DIM_LENGTH, 1e-2, 0},
        {L"dm", L"dm", DIM_LENGTH, 0.1, 0},  {L"m", L"m", DIM_LENGTH, 1, 0},
        {L"km", L"km", DIM_LENGTH, 1000, 0}, {L"in", L"in", DIM_LENGTH, 0.0254, 0},
        {L"inch", L"in", DIM_LENGTH, 0.0254, 0}, {L"ft", L"ft", DIM_LENGTH, 0.3048, 0},
        {L"foot", L"ft", DIM_LENGTH, 0.3048, 0}, {L"yd", L"yd", DIM_LENGTH, 0.9144, 0},
        {L"yard", L"yd", DIM_LENGTH, 0.9144, 0}, {L"mi", L"mi", DIM_LENGTH, 1609.344, 0},
        {L"mile", L"mi", DIM_LENGTH, 1609.344, 0}, {L"nmi", L"nmi", DIM_LENGTH, 1852, 0},
        // ---- mass (base: kilogram) ----
        {L"mg", L"mg", DIM_MASS, 1e-6, 0}, {L"g", L"g", DIM_MASS, 1e-3, 0},
        {L"kg", L"kg", DIM_MASS, 1, 0},    {L"t", L"t", DIM_MASS, 1000, 0},
        {L"tonne", L"t", DIM_MASS, 1000, 0}, {L"lb", L"lb", DIM_MASS, 0.45359237, 0},
        {L"lbs", L"lb", DIM_MASS, 0.45359237, 0}, {L"pound", L"lb", DIM_MASS, 0.45359237, 0},
        {L"oz", L"oz", DIM_MASS, 0.028349523125, 0}, {L"ounce", L"oz", DIM_MASS, 0.028349523125, 0},
        {L"st", L"st", DIM_MASS, 6.35029318, 0}, {L"stone", L"st", DIM_MASS, 6.35029318, 0},
        // ---- time (base: second) ----
        {L"ns", L"ns", DIM_TIME, 1e-9, 0}, {L"us", L"µs", DIM_TIME, 1e-6, 0},
        {L"ms", L"ms", DIM_TIME, 1e-3, 0}, {L"s", L"s", DIM_TIME, 1, 0},
        {L"sec", L"s", DIM_TIME, 1, 0},    {L"second", L"s", DIM_TIME, 1, 0},
        {L"min", L"min", DIM_TIME, 60, 0}, {L"minute", L"min", DIM_TIME, 60, 0},
        {L"h", L"h", DIM_TIME, 3600, 0},   {L"hr", L"h", DIM_TIME, 3600, 0},
        {L"hour", L"h", DIM_TIME, 3600, 0}, {L"day", L"d", DIM_TIME, 86400, 0},
        {L"week", L"wk", DIM_TIME, 604800, 0}, {L"year", L"yr", DIM_TIME, 31557600, 0},
        // ---- data (base: byte) ----
        {L"bit", L"bit", DIM_DATA, 0.125, 0}, {L"byte", L"B", DIM_DATA, 1, 0},
        {L"kb", L"kB", DIM_DATA, 1000, 0},  {L"mb", L"MB", DIM_DATA, 1e6, 0},
        {L"gb", L"GB", DIM_DATA, 1e9, 0},   {L"tb", L"TB", DIM_DATA, 1e12, 0},
        {L"pb", L"PB", DIM_DATA, 1e15, 0},  {L"kib", L"KiB", DIM_DATA, 1024, 0},
        {L"mib", L"MiB", DIM_DATA, 1048576, 0}, {L"gib", L"GiB", DIM_DATA, 1073741824.0, 0},
        {L"tib", L"TiB", DIM_DATA, 1099511627776.0, 0},
        // ---- volume (base: litre) ----
        {L"ml", L"mL", DIM_VOLUME, 1e-3, 0}, {L"cl", L"cL", DIM_VOLUME, 1e-2, 0},
        {L"dl", L"dL", DIM_VOLUME, 0.1, 0},  {L"l", L"L", DIM_VOLUME, 1, 0},
        {L"liter", L"L", DIM_VOLUME, 1, 0},  {L"litre", L"L", DIM_VOLUME, 1, 0},
        {L"m3", L"m³", DIM_VOLUME, 1000, 0}, {L"gal", L"gal", DIM_VOLUME, 3.785411784, 0},
        {L"gallon", L"gal", DIM_VOLUME, 3.785411784, 0}, {L"qt", L"qt", DIM_VOLUME, 0.946352946, 0},
        {L"pt", L"pt", DIM_VOLUME, 0.473176473, 0}, {L"cup", L"cup", DIM_VOLUME, 0.2365882365, 0},
        {L"floz", L"fl oz", DIM_VOLUME, 0.0295735295625, 0},
        // ---- speed (base: metre/second) ----
        {L"mps", L"m/s", DIM_SPEED, 1, 0}, {L"kph", L"km/h", DIM_SPEED, 1000.0 / 3600, 0},
        {L"kmh", L"km/h", DIM_SPEED, 1000.0 / 3600, 0}, {L"mph", L"mph", DIM_SPEED, 0.44704, 0},
        {L"knot", L"kn", DIM_SPEED, 1000.0 / 3600 * 1.852, 0}, {L"fps", L"ft/s", DIM_SPEED, 0.3048, 0},
        // ---- area (base: square metre) ----
        {L"m2", L"m²", DIM_AREA, 1, 0}, {L"km2", L"km²", DIM_AREA, 1e6, 0},
        {L"cm2", L"cm²", DIM_AREA, 1e-4, 0}, {L"ha", L"ha", DIM_AREA, 10000, 0},
        {L"acre", L"ac", DIM_AREA, 4046.8564224, 0}, {L"sqm", L"m²", DIM_AREA, 1, 0},
        {L"sqft", L"ft²", DIM_AREA, 0.09290304, 0},
        // ---- angle (base: radian) ----
        {L"rad", L"rad", DIM_ANGLE, 1, 0}, {L"deg", L"°", DIM_ANGLE, 3.14159265358979323846 / 180, 0},
        {L"degree", L"°", DIM_ANGLE, 3.14159265358979323846 / 180, 0},
        {L"grad", L"grad", DIM_ANGLE, 3.14159265358979323846 / 200, 0},
        {L"turn", L"turn", DIM_ANGLE, 6.28318530717958647692, 0},
        // ---- pressure (base: pascal) ----
        {L"pa", L"Pa", DIM_PRESSURE, 1, 0}, {L"kpa", L"kPa", DIM_PRESSURE, 1000, 0},
        {L"mpa", L"MPa", DIM_PRESSURE, 1e6, 0}, {L"bar", L"bar", DIM_PRESSURE, 1e5, 0},
        {L"atm", L"atm", DIM_PRESSURE, 101325, 0}, {L"psi", L"psi", DIM_PRESSURE, 6894.757293168, 0},
        {L"mmhg", L"mmHg", DIM_PRESSURE, 133.322387415, 0}, {L"torr", L"Torr", DIM_PRESSURE, 133.322368421, 0},
        // ---- energy (base: joule) ----
        {L"j", L"J", DIM_ENERGY, 1, 0}, {L"kj", L"kJ", DIM_ENERGY, 1000, 0},
        {L"cal", L"cal", DIM_ENERGY, 4.184, 0}, {L"kcal", L"kcal", DIM_ENERGY, 4184, 0},
        {L"wh", L"Wh", DIM_ENERGY, 3600, 0}, {L"kwh", L"kWh", DIM_ENERGY, 3.6e6, 0},
        {L"ev", L"eV", DIM_ENERGY, 1.602176634e-19, 0}, {L"btu", L"BTU", DIM_ENERGY, 1055.05585262, 0},
        // ---- power (base: watt) ----
        {L"w", L"W", DIM_POWER, 1, 0}, {L"kw", L"kW", DIM_POWER, 1000, 0},
        {L"hp", L"hp", DIM_POWER, 745.6998715823, 0},
        // ---- temperature (base: kelvin) ----
        {L"c", L"°C", DIM_TEMP, 1, 273.15}, {L"celsius", L"°C", DIM_TEMP, 1, 273.15},
        {L"f", L"°F", DIM_TEMP, 5.0 / 9, 459.67 * 5.0 / 9}, {L"fahrenheit", L"°F", DIM_TEMP, 5.0 / 9, 459.67 * 5.0 / 9},
        {L"k", L"K", DIM_TEMP, 1, 0}, {L"kelvin", L"K", DIM_TEMP, 1, 0},
    };
    return t;
}

namespace detail {

enum class Tk { Num, Ident, Op, LP, RP, End };

struct Token {
    Tk kind = Tk::End;
    double num = 0.0;
    bool exponent = false;
    std::wstring id;
    wchar_t op = 0;
};

struct Value {
    double v = 0.0;
    bool percent = false;
};

inline bool IsDigit(wchar_t c) { return c >= L'0' && c <= L'9'; }
// Value of a hex/base digit, or -1.
inline int DigitVal(wchar_t c) {
    if (c >= L'0' && c <= L'9') return c - L'0';
    if (c >= L'a' && c <= L'f') return c - L'a' + 10;
    if (c >= L'A' && c <= L'F') return c - L'A' + 10;
    return -1;
}
inline bool IsAlpha(wchar_t c) {
    return (c >= L'a' && c <= L'z') || (c >= L'A' && c <= L'Z');
}
inline bool IsAlnum(wchar_t c) { return IsAlpha(c) || IsDigit(c); }

inline bool IsConstant(const std::wstring& id, double& out) {
    if (id == L"pi") { out = 3.14159265358979323846; return true; }
    if (id == L"e") { out = 2.71828182845904523536; return true; }
    if (id == L"tau") { out = 6.28318530717958647692; return true; }
    if (id == L"phi") { out = 1.61803398874989484820; return true; }
    if (id == L"\u03c0") { out = 3.14159265358979323846; return true; }
    return false;
}

inline bool IsKeyword(const std::wstring& id) {
    return id == L"mod" || id == L"of";
}

inline bool IsFunction(const std::wstring& id) {
    static const wchar_t* const kFuncs[] = {
        L"sqrt", L"cbrt", L"abs", L"round", L"floor", L"ceil", L"trunc",
        L"sign", L"sin",  L"cos",  L"tan", L"asin",  L"acos", L"atan",
        L"sinh", L"cosh", L"tanh", L"log",   L"ln",   L"log2", L"exp",
        L"fact",
    };
    for (const wchar_t* f : kFuncs) {
        if (id == f) return true;
    }
    return false;
}

inline bool ParseCleanDecimal(const std::wstring& s, double& out) {
    size_t i = 0;
    bool neg = false;
    if (i < s.size() && (s[i] == L'-' || s[i] == L'+')) {
        neg = s[i] == L'-';
        i++;
    }
    double v = 0.0;
    bool any = false;
    while (i < s.size() && IsDigit(s[i])) {
        v = v * 10.0 + (s[i] - L'0');
        any = true;
        i++;
    }
    if (i < s.size() && s[i] == L'.') {
        i++;
        double scale = 0.1;
        while (i < s.size() && IsDigit(s[i])) {
            v += (s[i] - L'0') * scale;
            scale *= 0.1;
            any = true;
            i++;
        }
    }
    if (!any || i != s.size()) return false;
    out = neg ? -v : v;
    return true;
}

inline int CountChar(const std::wstring& s, wchar_t c) {
    int n = 0;
    for (wchar_t x : s) {
        if (x == c) n++;
    }
    return n;
}

// A run of digits/separators is interpreted with the user's decimal and grouping characters.
// A single separator that is not the decimal character is treated as the decimal only when it
// is not a plain thousands group (i.e. not exactly three digits after it).
inline bool ParseNumberLiteral(const std::wstring& run, const Locale& loc, double& out) {
    wchar_t dec = loc.decimal;
    wchar_t alt = (dec == L'.') ? L',' : L'.';

    int decCount = CountChar(run, dec);
    int altCount = CountChar(run, alt);

    bool altIsDecimal = false;
    if (decCount == 0 && altCount > 0) {
        if (altCount >= 2) {
            altIsDecimal = false;
        } else {
            size_t p = run.find(alt);
            size_t before = p;
            size_t after = run.size() - p - 1;
            altIsDecimal = !(after == 3 && before >= 1 && before <= 3);
        }
    }

    std::wstring cleaned;
    bool hasDecimal = false;
    for (wchar_t c : run) {
        if (IsDigit(c)) {
            cleaned += c;
        } else if (c == dec) {
            if (hasDecimal) return false;
            cleaned += L'.';
            hasDecimal = true;
        } else if (c == alt) {
            if (altIsDecimal) {
                if (hasDecimal) return false;
                cleaned += L'.';
                hasDecimal = true;
            }
        } else {
            return false;
        }
    }
    if (cleaned.empty() || cleaned == L".") return false;
    return ParseCleanDecimal(cleaned, out);
}

// Maps typographic/Unicode look-alikes to the ASCII characters the lexer understands, so that
// expressions pasted from documents, messages or web pages (e.g. "5−3" with U+2212, "3×4",
// "6÷2", non-breaking spaces or full-width punctuation) still evaluate.
inline wchar_t NormalizeMathChar(wchar_t c) {
    switch (c) {
        case 0x00A0:  // no-break space
        case 0x2007:  // figure space
        case 0x2008:  // punctuation space
        case 0x2009:  // thin space
        case 0x202F:  // narrow no-break space
        case 0x205F:  // medium mathematical space
        case 0x3000:  // ideographic space
            return L' ';
        case 0x2212:  // minus sign
        case 0x2013:  // en dash
        case 0x2014:  // em dash
        case 0x2011:  // non-breaking hyphen
        case 0xFE63:  // small hyphen-minus
        case 0xFF0D:  // full-width hyphen-minus
        case 0x2216:  // set minus
            return L'-';
        case 0x00D7:  // multiplication sign
        case 0x2217:  // asterisk operator
        case 0x22C5:  // dot operator
        case 0x00B7:  // middle dot
        case 0xFF0A:  // full-width asterisk
            return L'*';
        case 0x00F7:  // division sign
        case 0x2215:  // division slash
        case 0x2044:  // fraction slash
        case 0xFF0F:  // full-width solidus
            return L'/';
        case 0xFF0B:  // full-width plus
            return L'+';
        case 0xFF05:  // full-width percent
            return L'%';
        case 0xFF01:  // full-width exclamation
            return L'!';
        case 0xFF08:  // full-width left parenthesis
            return L'(';
        case 0xFF09:  // full-width right parenthesis
            return L')';
        case 0xFF3E:  // full-width circumflex
            return L'^';
        default:
            return c;
    }
}

struct Lexed {
    std::vector<Token> toks;
    bool ok = true;
    bool mathy = false;       // the input contains something that only makes sense as maths
    bool knownIdents = true;  // every word is a known function, keyword or constant
};

inline Lexed Lex(const std::wstring& in, const Locale& loc, bool hasAns = false) {
    Lexed out;
    bool sawKeyword = false;
    size_t i = 0;
    const size_t n = in.size();
    while (i < n) {
        wchar_t c = NormalizeMathChar(in[i]);
        if (c == L' ' || c == L'\t' || c == L'\r' || c == L'\n') {
            i++;
            continue;
        }
        // Radix-prefixed integers: 0xFF, 0b1010, 0o17 (case-insensitive prefixes/digits).
        if (c == L'0' && i + 1 < n) {
            const wchar_t p = in[i + 1];
            const int base = (p == L'x' || p == L'X')   ? 16
                             : (p == L'b' || p == L'B') ? 2
                             : (p == L'o' || p == L'O') ? 8
                                                        : 0;
            if (base) {
                size_t j = i + 2;
                double val = 0.0;
                bool any = false;
                for (; j < n; ++j) {
                    const int d = DigitVal(in[j]);
                    if (d < 0 || d >= base) break;
                    val = val * base + d;
                    any = true;
                }
                if (any) {
                    Token t;
                    t.kind = Tk::Num;
                    t.num = val;
                    out.toks.push_back(t);
                    out.mathy = true;
                    i = j;
                    continue;
                }
            }
        }
        if (IsDigit(c) || ((c == loc.decimal || c == (loc.decimal == L'.' ? L',' : L'.')) &&
                           i + 1 < n && IsDigit(in[i + 1]))) {
            size_t start = i;
            while (i < n && (IsDigit(in[i]) || in[i] == L'.' || in[i] == L',')) i++;
            std::wstring run = in.substr(start, i - start);
            double val = 0.0;
            if (!ParseNumberLiteral(run, loc, val)) {
                out.ok = false;
                return out;
            }
            bool usedExp = false;
            if (i < n && (in[i] == L'e' || in[i] == L'E')) {
                size_t save = i;
                size_t j = i + 1;
                if (j < n && (in[j] == L'+' || in[j] == L'-')) j++;
                if (j < n && IsDigit(in[j])) {
                    while (j < n && IsDigit(in[j])) j++;
                    std::wstring es = in.substr(save + 1, j - save - 1);
                    double ev = 0.0;
                    if (ParseCleanDecimal(es, ev)) {
                        val *= std::pow(10.0, ev);
                        usedExp = true;
                        i = j;
                    }
                }
            }
            if (usedExp) out.mathy = true;
            Token t;
            t.kind = Tk::Num;
            t.num = val;
            t.exponent = usedExp;
            out.toks.push_back(t);
            continue;
        }
        if (IsAlpha(c)) {
            size_t start = i;
            while (i < n && IsAlnum(in[i])) i++;
            std::wstring id = in.substr(start, i - start);
            for (wchar_t& ch : id) {
                if (ch >= L'A' && ch <= L'Z') ch = (wchar_t)(ch - L'A' + L'a');
            }
            double cval = 0.0;
            bool isKw = IsKeyword(id);
            if (!IsFunction(id) && !isKw && !IsConstant(id, cval) && !(hasAns && id == L"ans")) {
                out.knownIdents = false;
            }
            if (isKw) sawKeyword = true;
            Token t;
            t.kind = Tk::Ident;
            t.id = id;
            out.toks.push_back(t);
            continue;
        }
        if (c == L'+' || c == L'-' || c == L'*' || c == L'/' || c == L'^' || c == L'%' ||
            c == L'!') {
            Token t;
            t.kind = Tk::Op;
            t.op = c;
            out.toks.push_back(t);
            out.mathy = true;
            i++;
            continue;
        }
        if (c == L'(') {
            Token t;
            t.kind = Tk::LP;
            out.toks.push_back(t);
            out.mathy = true;
            i++;
            continue;
        }
        if (c == L')') {
            Token t;
            t.kind = Tk::RP;
            out.toks.push_back(t);
            out.mathy = true;
            i++;
            continue;
        }
        out.ok = false;
        return out;
    }
    // "mod" / "of" only count as maths next to a number ("10 mod 3", "20 of 150"). Alone they are
    // just the start of a name ("Office", "Mod Organizer") and must not show an error card.
    if (sawKeyword) {
        for (const Token& tk : out.toks) {
            if (tk.kind == Tk::Num) { out.mathy = true; break; }
        }
    }
    Token end;
    end.kind = Tk::End;
    out.toks.push_back(end);
    return out;
}

struct Parser {
    const std::vector<Token>& t;
    size_t i = 0;
    bool ok = true;
    std::wstring err;
    Context ctx;

    explicit Parser(const std::vector<Token>& toks, const Context& c = Context{}) : t(toks), ctx(c) {}

    const Token& Cur() const { return t[i]; }
    void Adv() {
        if (i + 1 < t.size()) i++;
    }
    void Fail(const std::wstring& m) {
        if (ok) {
            ok = false;
            err = m;
        }
    }

    double Factorial(double x) {
        if (x < 0.0 || x != std::floor(x) || x > 170.0) {
            Fail(L"factorial is only defined for whole numbers 0-170");
            return 0.0;
        }
        double r = 1.0;
        for (int k = 2; k <= (int)x; k++) r *= k;
        return r;
    }

    double CallFunc(const std::wstring& id, double x) {
        if (id == L"sqrt") { if (x < 0) { Fail(L"square root of a negative number"); return 0; } return std::sqrt(x); }
        if (id == L"cbrt") return std::cbrt(x);
        if (id == L"abs") return std::fabs(x);
        if (id == L"round") return std::round(x);
        if (id == L"floor") return std::floor(x);
        if (id == L"ceil") return std::ceil(x);
        if (id == L"trunc") return std::trunc(x);
        if (id == L"sign") return (double)((x > 0) - (x < 0));
        if (id == L"sin") return std::sin(x);
        if (id == L"cos") return std::cos(x);
        if (id == L"tan") return std::tan(x);
        if (id == L"asin") { if (x < -1 || x > 1) { Fail(L"asin needs a value between -1 and 1"); return 0; } return std::asin(x); }
        if (id == L"acos") { if (x < -1 || x > 1) { Fail(L"acos needs a value between -1 and 1"); return 0; } return std::acos(x); }
        if (id == L"atan") return std::atan(x);
        if (id == L"sinh") return std::sinh(x);
        if (id == L"cosh") return std::cosh(x);
        if (id == L"tanh") return std::tanh(x);
        if (id == L"log") { if (x <= 0) { Fail(L"logarithm needs a positive value"); return 0; } return std::log10(x); }
        if (id == L"ln") { if (x <= 0) { Fail(L"logarithm needs a positive value"); return 0; } return std::log(x); }
        if (id == L"log2") { if (x <= 0) { Fail(L"logarithm needs a positive value"); return 0; } return std::log2(x); }
        if (id == L"exp") return std::exp(x);
        if (id == L"fact") return Factorial(x);
        Fail(L"unknown function");
        return 0.0;
    }

    Value ParsePrimary() {
        Value r;
        const Token& c = Cur();
        if (c.kind == Tk::Num) {
            r.v = c.num;
            Adv();
            return r;
        }
        if (c.kind == Tk::LP) {
            Adv();
            r = ParseExpr();
            if (Cur().kind != Tk::RP) {
                Fail(L"missing closing parenthesis");
                return r;
            }
            Adv();
            return r;
        }
        if (c.kind == Tk::Ident) {
            std::wstring id = c.id;
            Adv();
            if (Cur().kind == Tk::LP) {
                Adv();
                Value a = ParseExpr();
                if (Cur().kind != Tk::RP) {
                    Fail(L"missing closing parenthesis");
                    return r;
                }
                Adv();
                r.v = CallFunc(id, a.v);
                return r;
            }
            double cv = 0.0;
            if (IsConstant(id, cv)) {
                r.v = cv;
                return r;
            }
            if (id == L"ans" && ctx.hasAns) {
                r.v = ctx.ans;
                return r;
            }
            Fail(L"unexpected name");
            return r;
        }
        Fail(L"unexpected symbol");
        return r;
    }

    Value ParsePostfix() {
        Value r = ParsePrimary();
        while (Cur().kind == Tk::Op && (Cur().op == L'%' || Cur().op == L'!')) {
            wchar_t op = Cur().op;
            Adv();
            if (op == L'%') {
                r.v = r.v / 100.0;
                r.percent = true;
            } else {
                r.v = Factorial(r.v);
                r.percent = false;
            }
        }
        return r;
    }

    Value ParsePower() {
        Value base = ParsePostfix();
        if (Cur().kind == Tk::Op && Cur().op == L'^') {
            Adv();
            Value ex = ParseUnary();
            Value r;
            r.v = std::pow(base.v, ex.v);
            r.percent = false;
            return r;
        }
        return base;
    }

    Value ParseUnary() {
        if (Cur().kind == Tk::Op && (Cur().op == L'+' || Cur().op == L'-')) {
            wchar_t op = Cur().op;
            Adv();
            Value u = ParseUnary();
            u.v = (op == L'-') ? -u.v : u.v;
            u.percent = false;
            return u;
        }
        return ParsePower();
    }

    Value ParseTerm() {
        Value left = ParseUnary();
        while (true) {
            const Token& c = Cur();
            bool isMul = c.kind == Tk::Op && (c.op == L'*' || c.op == L'/');
            bool isWord = c.kind == Tk::Ident && IsKeyword(c.id);
            if (!isMul && !isWord) break;
            wchar_t op = isMul ? c.op : 0;
            std::wstring word = isMul ? L"" : c.id;
            Adv();
            Value right = ParseUnary();
            if (isMul) {
                if (op == L'*') {
                    left.v = left.v * right.v;
                } else {
                    if (right.v == 0.0) Fail(L"division by zero");
                    left.v = left.v / right.v;
                }
            } else if (word == L"mod") {
                if (right.v == 0.0) Fail(L"division by zero");
                left.v = std::fmod(left.v, right.v);
            } else {
                left.v = left.v * right.v;  // "20% of 150"
            }
            left.percent = false;
        }
        return left;
    }

    Value ParseExpr() {
        Value left = ParseTerm();
        while (Cur().kind == Tk::Op && (Cur().op == L'+' || Cur().op == L'-')) {
            wchar_t op = Cur().op;
            Adv();
            Value right = ParseTerm();
            // "100 + 15%" adds 15% of the left side; otherwise a percentage is already a fraction.
            double rhs = right.percent ? left.v * right.v : right.v;
            left.v = (op == L'+') ? left.v + rhs : left.v - rhs;
            left.percent = false;
        }
        return left;
    }
};

}  // namespace detail

// Core expression evaluation. `requireMathy` is false for the numeric part of a conversion, where
// a bare number (e.g. "10" in "10 km to miles") must still evaluate.
inline Result EvalCore(const std::wstring& input, const Locale& loc, const Context& ctx,
                       bool requireMathy) {
    Result res;
    detail::Lexed lx = detail::Lex(input, loc, ctx.hasAns);
    if (!lx.ok) return res;
    if (!lx.knownIdents) return res;

    // Count tokens and detect a lone constant. A single well-known constant with two or more
    // letters ("pi", "tau", "phi", "ans") is a calculation by itself, so typing it shows its
    // value. Single letters are excluded so that "e" still searches normally.
    const detail::Token* only = nullptr;
    size_t count = 0;
    for (const auto& t : lx.toks) {
        if (t.kind == detail::Tk::End) continue;
        only = &t;
        ++count;
    }
    if (count == 0) return res;
    bool loneConstant = false;
    if (count == 1 && only->kind == detail::Tk::Ident) {
        double cv = 0.0;
        loneConstant = only->id.size() >= 2 &&
                       (detail::IsConstant(only->id, cv) || (ctx.hasAns && only->id == L"ans"));
    }
    if (requireMathy && !lx.mathy && !loneConstant) return res;

    detail::Parser p(lx.toks, ctx);
    detail::Value v = p.ParseExpr();
    if (!p.ok) {
        res.status = Status::Error;
        res.error = p.err;
        return res;
    }
    if (p.Cur().kind != detail::Tk::End) {
        res.status = Status::Error;
        res.error = L"could not read the whole expression";
        return res;
    }
    if (!std::isfinite(v.v)) {
        res.status = Status::Error;
        res.error = L"result is not a finite number";
        return res;
    }
    res.status = Status::Ok;
    res.value = v.v;
    res.loneConstant = loneConstant;
    return res;
}

inline std::wstring Format(double v, const Locale& loc);  // defined below

// ===== Unit / numeric-base conversions ("<value> to <target>") =====

inline std::vector<std::wstring> SplitWords(const std::wstring& s) {
    std::vector<std::wstring> out;
    std::wstring cur;
    for (wchar_t ch : s) {
        wchar_t c = detail::NormalizeMathChar(ch);
        if (c == L' ' || c == L'\t' || c == L'\r' || c == L'\n') {
            if (!cur.empty()) { out.push_back(cur); cur.clear(); }
        } else {
            if (c >= L'A' && c <= L'Z') c = (wchar_t)(c - L'A' + L'a');
            cur.push_back(c);
        }
    }
    if (!cur.empty()) out.push_back(cur);
    return out;
}

inline std::wstring JoinWords(const std::vector<std::wstring>& w, int a, int b) {
    std::wstring s;
    for (int i = a; i < b; ++i) {
        if (!s.empty()) s += L' ';
        s += w[i];
    }
    return s;
}

inline const UnitDef* FindUnit(const std::wstring& word) {
    const auto& t = UnitTable();
    for (const auto& u : t) {
        if (word == u.name) return &u;
    }
    if (word.size() > 1 && word.back() == L's') {  // plural
        const std::wstring sing = word.substr(0, word.size() - 1);
        for (const auto& u : t) {
            if (sing == u.name) return &u;
        }
    }
    return nullptr;
}

inline int BaseTargetOf(const std::wstring& w) {
    if (w == L"hex" || w == L"hexadecimal") return 16;
    if (w == L"bin" || w == L"binary") return 2;
    if (w == L"oct" || w == L"octal") return 8;
    if (w == L"dec" || w == L"decimal") return 10;
    return 0;
}

// Rank of a byte-based data unit (0 = byte, 1 = kilo, ... 5 = peta). `isBinNamed` is true for the
// explicit binary names (KiB, MiB, ...). "bit" is deliberately excluded: the decimal/binary
// ambiguity is about byte multiples, so bit conversions stay as a single value.
inline bool DataRank(const std::wstring& name, int& rank, bool& isBinNamed) {
    isBinNamed = false;
    if (name == L"byte") { rank = 0; return true; }
    if (name == L"kb") { rank = 1; return true; }
    if (name == L"mb") { rank = 2; return true; }
    if (name == L"gb") { rank = 3; return true; }
    if (name == L"tb") { rank = 4; return true; }
    if (name == L"pb") { rank = 5; return true; }
    if (name == L"kib") { rank = 1; isBinNamed = true; return true; }
    if (name == L"mib") { rank = 2; isBinNamed = true; return true; }
    if (name == L"gib") { rank = 3; isBinNamed = true; return true; }
    if (name == L"tib") { rank = 4; isBinNamed = true; return true; }
    return false;
}

// Values at or beyond 2^64 do not fit the unsigned conversion below (undefined behaviour).
inline bool FitsInBase(double v) { return std::isfinite(v) && std::fabs(v) < 18446744073709551616.0; }

inline std::wstring FormatInBase(double v, int base) {
    if (!FitsInBase(v)) return L"";
    const bool neg = v < 0;
    unsigned long long u = (unsigned long long)(neg ? -v : v);
    static const wchar_t* const kDigits = L"0123456789ABCDEF";
    std::wstring s;
    do {
        s.push_back(kDigits[u % base]);
        u /= base;
    } while (u);
    std::reverse(s.begin(), s.end());
    const wchar_t* prefix = (base == 16) ? L"0x" : (base == 2) ? L"0b" : (base == 8) ? L"0o" : L"";
    return std::wstring(neg ? L"-" : L"") + prefix + s;
}

// Handles "<value> [unit] (to|as) <unit-or-base>". Returns true when it consumed the input (even
// to report an error), false to let normal expression evaluation try instead.
inline bool TryConversion(const std::wstring& input, const Locale& loc, const Context& ctx,
                          Result& out) {
    const std::vector<std::wstring> w = SplitWords(input);
    if (w.size() < 3) return false;
    int k = -1;
    for (size_t idx = 1; idx + 1 < w.size(); ++idx) {
        if (w[idx] == L"to" || w[idx] == L"as") { k = (int)idx; break; }
    }
    if (k < 0 || k + 2 != (int)w.size()) return false;  // right side must be a single word
    const std::wstring target = w[k + 1];

    // Numeric base target.
    if (const int base = BaseTargetOf(target)) {
        const Result l = EvalCore(JoinWords(w, 0, k), loc, ctx, /*requireMathy=*/false);
        if (l.status != Status::Ok) return false;
        if (l.value != std::floor(l.value) || !std::isfinite(l.value)) {
            out.status = Status::Error;
            out.error = L"a base conversion needs a whole number";
            return true;
        }
        if (!FitsInBase(l.value)) {
            out.status = Status::Error;
            out.error = L"the number is too large for a base conversion";
            return true;
        }
        out.status = Status::Ok;
        out.value = l.value;
        out.display = FormatInBase(l.value, base);
        out.insertable = out.display;
        return true;
    }

    // Unit target.
    const UnitDef* dst = FindUnit(target);
    if (!dst) return false;

    const int last = k - 1;
    const UnitDef* src = FindUnit(w[last]);
    std::wstring glueNum;
    if (!src) {
        // "10km" was kept as one word: split it into the number and the unit.
        size_t cut = std::wstring::npos;
        for (size_t p = 0; p < w[last].size(); ++p) {
            if (w[last][p] >= L'a' && w[last][p] <= L'z') { cut = p; break; }
        }
        if (cut != std::wstring::npos && cut > 0) {
            const UnitDef* s2 = FindUnit(w[last].substr(cut));
            if (s2) { src = s2; glueNum = w[last].substr(0, cut); }
        }
    }
    if (!src) return false;

    std::wstring left = JoinWords(w, 0, last);
    if (!glueNum.empty()) {
        if (!left.empty()) left += L' ';
        left += glueNum;
    }
    const Result l = EvalCore(left, loc, ctx, /*requireMathy=*/false);
    if (l.status != Status::Ok) return false;
    if (src->dim != dst->dim) {
        out.status = Status::Error;
        out.error = std::wstring(L"cannot convert ") + src->sym + L" to " + dst->sym;
        return true;
    }

    // Data sizes are ambiguous: "GB" can mean 10^9 (decimal) or 2^30 (binary). Rather than ask the
    // user to choose, show both interpretations of the same target unit. The primary value follows
    // the naming convention the user typed (GiB/MiB -> binary, GB/MB -> decimal).
    if (src->dim == DIM_DATA && dst->dim == DIM_DATA) {
        int rs = 0, rd = 0;
        bool binS = false, binD = false;
        if (DataRank(src->name, rs, binS) && DataRank(dst->name, rd, binD)) {
            const double dres = l.value * std::pow(1000.0, rs - rd);
            const double bres = l.value * std::pow(1024.0, rs - rd);
            const bool primaryBin = binS && binD;
            const double prim = primaryBin ? bres : dres;
            const double other = primaryBin ? dres : bres;
            const std::wstring ps = Format(prim, loc);
            out.status = Status::Ok;
            out.value = prim;
            out.insertable = ps;
            out.display = ps + L" " + dst->sym;
            if (other != prim) {
                out.display += (primaryBin ? L"  (decimal " : L"  (binary ") + Format(other, loc) +
                                L" " + dst->sym + L")";
            }
            return true;
        }
    }

    const double baseVal = l.value * src->factor + src->offset;
    const double conv = (baseVal - dst->offset) / dst->factor;
    out.status = Status::Ok;
    out.value = conv;
    out.display = Format(conv, loc) + L" " + dst->sym;
    out.insertable = Format(conv, loc);
    return true;
}

inline Result Evaluate(const std::wstring& input, const Locale& loc = SystemLocale(),
                       const Context& ctx = Context{}) {
    Result res;
    if (TryConversion(input, loc, ctx, res)) return res;
    return EvalCore(input, loc, ctx, /*requireMathy=*/true);
}

namespace detail {

inline std::wstring GroupInt(const std::wstring& digits, const Locale& loc) {
    bool neg = !digits.empty() && digits[0] == L'-';
    std::wstring d = neg ? digits.substr(1) : digits;
    if (!loc.grouping || d.size() <= 3) return digits;
    std::wstring out;
    int count = 0;
    for (int i = (int)d.size() - 1; i >= 0; --i) {
        out.push_back(d[i]);
        if (++count % 3 == 0 && i > 0) out.push_back(loc.group);
    }
    std::reverse(out.begin(), out.end());
    return neg ? L"-" + out : out;
}

}  // namespace detail

// Formats a value for display: no trailing zeros, thousands grouped and the decimal separator
// taken from the user's Windows locale.
inline std::wstring Format(double v, const Locale& loc) {
    if (!std::isfinite(v)) return L"—";

    bool integral =
        std::fabs(v) < 1e15 && std::fabs(v - std::round(v)) < 1e-9 * std::max(1.0, std::fabs(v));
    if (integral) {
        long long iv = (long long)std::llround(v);
        wchar_t buf[32] = {};
        swprintf(buf, ARRAYSIZE(buf), L"%lld", iv);
        return detail::GroupInt(buf, loc);
    }

    wchar_t buf[64] = {};
    swprintf(buf, ARRAYSIZE(buf), L"%.12g", v);
    std::wstring s = buf;

    std::wstring mant = s;
    std::wstring exp;
    size_t epos = s.find_first_of(L"eE");
    if (epos != std::wstring::npos) {
        mant = s.substr(0, epos);
        exp = s.substr(epos);
    }

    std::wstring ip = mant;
    std::wstring fp;
    size_t dot = mant.find(L'.');
    if (dot != std::wstring::npos) {
        ip = mant.substr(0, dot);
        fp = mant.substr(dot + 1);
    }
    std::wstring out = detail::GroupInt(ip, loc);
    if (!fp.empty()) {
        out += loc.decimal;
        out += fp;
    }
    out += exp;
    return out;
}

}  // namespace Calc

// ===== Design constants (in DIP; they are multiplied by the monitor scale) =====

static const float kMargin = 40.f;     // room for the shadow
static const float kWidth = 700.f;
static const float kBarH = 64.f;
static const float kRowH = 52.f;
static const float kListPad = 8.f;
static const float kEmptyRowH = 56.f;  // message row ("No results")
static float kRadius = 20.f;
static const float kRowRadius = 12.f;
static const float kIconSize = 32.f;
static const float kFooterH = 40.f;    // footer with shortcuts (expanded only)
static const float kSectionH = 26.f;   // section header ("Recent" / "Suggestions")
static const float kHeroH = 84.f;      // calculator result card, reserved above the list
static const float kHistRowH = 34.f;   // compact row for the calculator history

// Details panel (card to the right of the search bar)
static const float kPanelW = 320.f;
// Fixed height shared by the expanded search bar and the details panel; the row count is
// derived from it.
static const float kExpandedMinH = 440.f;
static const float kPanelGap = 12.f;
static const float kPanelPad = 16.f;
static const float kPanelActionsTop = 90.f;  // y of the first action (from the top of the panel)
static const float kActionH = 28.f;
static const float kInfoRowH = 22.f;

// List with an empty search bar (Expanded mode)
static const size_t kMaxRecents = 4;

static const wchar_t kTxtPlaceholder[] = L"Search applications…";
static const wchar_t kTxtOpen[] = L"Open";
static const wchar_t kTxtEnterGlyph[] = L"↵";
static const wchar_t kTxtNoResults[] = L"No results";
static const wchar_t kTxtIndexing[] = L"Indexing applications…";
static const wchar_t kTxtRecents[] = L"Recent";
static const wchar_t kTxtSuggestions[] = L"Suggestions";
static const wchar_t kTxtActions[] = L"Actions";
static const wchar_t kTxtInfo[] = L"Information";
static const wchar_t kTxtDetails[] = L"Details";
static const wchar_t kTxtReindex[] = L"Reindex";
static const wchar_t* const kReindexKeys[2] = {L"Ctrl", L"R"};
static const wchar_t kTxtTypeApp[] = L"Application";
static const wchar_t kTxtTypeStore[] = L"Store app";
static const wchar_t kTxtCalcCopy[] = L"Copy result";
static const wchar_t kTxtCalcCopied[] = L"Copied";
static const wchar_t kTxtCalcAdd[] = L"Add to operation";
static const wchar_t kTxtCalcEdit[] = L"Edit";
static const wchar_t kTxtCalcDelete[] = L"Delete";
static const wchar_t kTxtCalcClear[] = L"Clear history";
static const wchar_t kTxtCalcHistory[] = L"History";

static const UINT WM_APP_INDEX = WM_APP + 1;
static const UINT WM_APP_ICON = WM_APP + 2;
static const UINT WM_APP_INDEX_DONE = WM_APP + 3;
static const UINT WM_APP_SETTINGS = WM_APP + 4;
static const UINT WM_APP_MENU_CHANGED = WM_APP + 5;  // the watcher detected changes in the Start menu

// Start menu reindexing
static const double kAutoSafetyRescanSec = 600.0;  // fallback in Automatic mode (MSIX apps)
static const DWORD kWatchDebounceMs = 1500;        // wait for the installer to finish

static const double kPi = 3.14159265358979323846;

static const UINT_PTR TIMER_FRAME = 1;
static const UINT_PTR TIMER_BLINK = 2;
static const UINT_PTR TIMER_COPY = 3;  // clears the calculator "Copied" flash
static const UINT_PTR TIMER_CALC = 4;  // debounce for calculator evaluation
static const int HOTKEY_ID = 1;

// ===== Types =====

enum class ReindexMode { Auto, Always, Interval };
enum class ListMode { Compact, Expanded };
enum class DetailsKey { CtrlI, CtrlK, CtrlSpace, F4 };
enum class BgEffect { Acrylic, Mica, Glass, Frosted, Aurora, Solid };
enum class CalcStyle { Hero, Ghost, Both };

struct LauncherSettings {
    ListMode listMode = ListMode::Expanded;
    DetailsKey detailsKey = DetailsKey::CtrlI;
    ReindexMode reindexMode = ReindexMode::Auto;
    double reindexIntervalSec = 0;  // only with ReindexMode::Interval
    UINT hotkeyMods = MOD_ALT;
    UINT hotkeyVk = VK_SPACE;
    int verticalPct = 30;
    int maxRows = 8;                 // cap on how many results the search returns
    int opacityPct = 90;
    BgEffect effect = BgEffect::Acrylic;
    int blurPct = 50;     // 0..100 -> multiplies the effect's base blur (50 = x1)
    bool liveAcrylic = true;  // DWM acrylic in real time (Acrylic / Frosted / Aurora)
    int colorIdx = 0;     // index in kColors
    bool hideUtility = true;
    bool disableFullscreen = true;  // ignore the shortcut while a fullscreen window is focused
    bool calculatorEnabled = true;
    CalcStyle calcStyle = CalcStyle::Both;
    bool sounds = true;      // soft interface sounds (open, details panel, typing)
    int soundVolume = 70;    // 0..100
};

struct AppEntry {
    std::wstring name;
    std::wstring folded;  // same length as name (folded 1:1)
    std::wstring id;      // parsing name relative to AppsFolder
    std::wstring usageKey;
    int uses = 0;
    int lastUsed = 0;  // seconds since 2020-01-01 (0 = never); see NowStamp()
    int iconW = 0;
    int iconH = 0;
    std::vector<uint32_t> iconPx;  // BGRA premultiplied
    ID2D1Bitmap* bmp = nullptr;
};

struct SearchResult {
    int app = 0;
    int score = 0;
    int section = 0;  // 0 = no section (search), 1 = Recent, 2 = Suggestions
    IDWriteTextLayout* layout = nullptr;
};

// Technical data about an app for the details panel (computed on demand and cached)
struct InfoRow {
    std::wstring label;
    std::wstring value;
    bool middle = false;  // truncate in the middle (paths, identifiers)
};

struct AppDetails {
    bool packaged = false;   // Store app (MSIX)
    bool canFile = false;    // a real program file exists (open location, copy path)
    bool canShell = false;   // this 32-bit process can reach the path (elevate, properties)
    bool isExe = false;
    std::wstring target;     // program path (desktop apps)
    std::wstring args;       // shortcut arguments (kept for elevation)
    std::vector<InfoRow> rows;
};

enum ActionId { ACT_OPEN, ACT_ADMIN, ACT_LOCATION, ACT_COPYPATH, ACT_PROPS, ACT_FORGET };

struct ActionDef {
    int id;
    const wchar_t* label;
    const wchar_t* keys;
};

static const ActionDef kActions[] = {
    {ACT_OPEN, L"Open", L"↵"},
    {ACT_ADMIN, L"Run as administrator", L"Ctrl+Shift+↵"},
    {ACT_LOCATION, L"Open file location", L"Ctrl+↵"},
    {ACT_COPYPATH, L"Copy path", L"Ctrl+Shift+C"},
    {ACT_PROPS, L"Properties", L"Ctrl+P"},
    {ACT_FORGET, L"Remove from recents", L"Ctrl+Shift+R"},
};

struct ThemePalette {
    D2D1_COLOR_F bg;
    D2D1_COLOR_F border;
    D2D1_COLOR_F text;
    D2D1_COLOR_F textDim;
    D2D1_COLOR_F selection;
    D2D1_COLOR_F separator;
    D2D1_COLOR_F keycap;
    D2D1_COLOR_F match;
    D2D1_COLOR_F caretSel;
    D2D1_COLOR_F accent;   // main accent (selection, highlighted letters, Aurora gradient)
    D2D1_COLOR_F accent2;  // second color of the Aurora gradient
    bool light;
    float shadowAlpha;
};

// Available colors: base background, accent and second accent (RGB 0..1).
struct ColorDef {
    const wchar_t* id;
    bool light;
    float bg[3];
    float acc[3];
    float acc2[3];
};

static const ColorDef kColors[] = {
    {L"midnight", false, {0.075f, 0.090f, 0.125f}, {0.30f, 0.55f, 1.00f}, {0.55f, 0.35f, 1.00f}},
    {L"graphite", false, {0.105f, 0.105f, 0.120f}, {0.35f, 0.62f, 1.00f}, {0.65f, 0.45f, 1.00f}},
    {L"black",    false, {0.020f, 0.020f, 0.025f}, {0.45f, 0.70f, 1.00f}, {0.80f, 0.45f, 1.00f}},
    {L"violet",   false, {0.105f, 0.075f, 0.170f}, {0.66f, 0.45f, 1.00f}, {1.00f, 0.45f, 0.75f}},
    {L"emerald",  false, {0.050f, 0.120f, 0.095f}, {0.20f, 0.82f, 0.55f}, {0.25f, 0.75f, 0.95f}},
    {L"teal",     false, {0.045f, 0.110f, 0.130f}, {0.15f, 0.78f, 0.85f}, {0.35f, 0.55f, 1.00f}},
    {L"rose",     false, {0.160f, 0.075f, 0.110f}, {1.00f, 0.40f, 0.62f}, {1.00f, 0.62f, 0.35f}},
    {L"crimson",  false, {0.155f, 0.060f, 0.070f}, {1.00f, 0.35f, 0.38f}, {1.00f, 0.60f, 0.30f}},
    {L"amber",    false, {0.150f, 0.105f, 0.050f}, {1.00f, 0.66f, 0.20f}, {1.00f, 0.40f, 0.35f}},
    {L"white",    true,  {0.975f, 0.975f, 0.980f}, {0.16f, 0.43f, 0.96f}, {0.55f, 0.35f, 0.95f}},
    {L"sky",      true,  {0.915f, 0.955f, 0.995f}, {0.10f, 0.50f, 0.95f}, {0.20f, 0.75f, 0.90f}},
    {L"lavender", true,  {0.945f, 0.930f, 0.985f}, {0.50f, 0.32f, 0.90f}, {0.90f, 0.40f, 0.70f}},
};

struct RawApp {
    std::wstring name;
    std::wstring id;
};

struct IndexResult {
    UINT gen;
    std::vector<RawApp> apps;
};

struct IconResult {
    UINT gen;
    size_t index;
    int w;
    int h;
    std::vector<uint32_t> px;
};

struct IndexJob {
    HWND hwnd;
    UINT gen;
    bool hideUtility;
    std::unordered_set<std::wstring> haveIcons;
};

enum class Vis { Hidden, Showing, Shown, Hiding };

// ===== Global state (only touched from the UI thread, except where noted) =====

static LauncherSettings g_cfg;
static HWND g_hwnd = nullptr;
static HANDLE g_uiThread = nullptr;
static DWORD g_uiThreadId = 0;

static Vis g_vis = Vis::Hidden;
static double g_visStart = 0;
static double g_shownAt = 0;
static double g_alpha = 0;
static double g_offY = 0;
static double g_lastFrame = 0;
static bool g_frameTimerOn = false;
static bool g_dirty = false;

static double g_scale = 1.0;
static int g_bmpW = 0;
static int g_bmpH = 0;
static HDC g_memDC = nullptr;
static HBITMAP g_dib = nullptr;
static HGDIOBJ g_oldBmp = nullptr;
static int g_winX = 0;
static int g_winY = 0;
// Vertical placement (screen px). g_anchorTop is where the top of the search bar would sit
// without any limit; the card is kept inside the work area [g_workTop, g_workBottom].
static double g_anchorTop = 0;
static double g_workTop = 0;
static double g_workBottom = 0;

static ID2D1Factory* g_d2d = nullptr;
static IDWriteFactory* g_dw = nullptr;
static ID2D1DCRenderTarget* g_rt = nullptr;
static ID2D1SolidColorBrush* g_brush = nullptr;
static ID2D1SolidColorBrush* g_matchBrush = nullptr;
static ID2D1StrokeStyle* g_roundStroke = nullptr;
static ID2D1LinearGradientBrush* g_gradBrush = nullptr;  // subtle sheen at the top of the cards
static ID2D1LinearGradientBrush* g_glow1 = nullptr;      // Aurora effect gradient (accent 1)
static ID2D1LinearGradientBrush* g_glow2 = nullptr;      // Aurora effect gradient (accent 2)
static IDWriteTextFormat* g_fmtInput = nullptr;
static IDWriteTextFormat* g_fmtTitle = nullptr;
static IDWriteTextFormat* g_fmtHint = nullptr;
static IDWriteTextFormat* g_fmtKey = nullptr;
static IDWriteTextFormat* g_fmtEmpty = nullptr;
static IDWriteTextFormat* g_fmtSection = nullptr;     // "Recent" / "Actions" headers
static IDWriteTextFormat* g_fmtPanelTitle = nullptr;  // app name in the panel
static IDWriteTextFormat* g_fmtPanelSub = nullptr;    // panel subtitle
static IDWriteTextFormat* g_fmtPanelRow = nullptr;    // panel rows
static IDWriteInlineObject* g_ellipsis = nullptr;
static IDWriteInlineObject* g_ellipsisSm = nullptr;
static ThemePalette g_pal;

// Software acrylic: downscaled and blurred desktop capture (BGRA premultiplied, opaque)
static std::vector<uint32_t> g_backPx;
static int g_backW = 0;
static int g_backH = 0;
static ID2D1Bitmap* g_backBmp = nullptr;          // g_backPx as a D2D bitmap (created when drawing)
static ID2D1BitmapBrush* g_backBrush = nullptr;   // brush in surface coordinates
static ID2D1Bitmap* g_noiseBmp = nullptr;         // grain tile
static ID2D1BitmapBrush* g_noiseBrush = nullptr;
static double g_lastHide = -1e9;                  // last time the window was hidden

static std::vector<AppEntry> g_apps;
static std::vector<SearchResult> g_results;
static std::vector<float> g_rowOff;  // y (DIP) of each row relative to the top of the list, n+1 entries
static int g_headerCount = 0;        // section headers present (only with the search bar empty)
static std::vector<std::wstring> g_tokens;
static std::wstring g_text;
static int g_caret = 0;
static int g_anchor = 0;
static float g_inputScroll = 0;
static bool g_caretOn = true;

static int g_sel = 0;
static double g_selPos = 0;
static double g_selTarget = 0;
static double g_scrollPos = 0;
static double g_scrollTarget = 0;
static bool g_kbScroll = false;  // the scroll came from the keyboard (pixel-exact, partial rows)
static double g_curH = kBarH;
static double g_targetH = kBarH;
static POINT g_lastMouse = {-1, -1};

// Details panel
static bool g_detailsOpen = false;
static double g_detailPos = 0;     // 0..1, animated
static double g_detailTarget = 0;
static int g_hoverAction = -1;     // id of the action under the mouse, or -1
static bool g_hoverReindex = false;  // the mouse is over the footer reindex button
static std::wstring g_panelAppId;  // last app shown in the panel (for the fade-out)
static std::unordered_map<std::wstring, AppDetails> g_detailsCache;

static UINT g_gen = 0;
static bool g_indexing = false;
static bool g_indexedOnce = false;
static double g_lastIndexAt = -1e9;
static bool g_menuDirty = true;  // the Start menu changed since the last reindex (UI thread)

// Inline calculator (see namespace Calc): the result for the current text, its formatted form
// (for the card, the inline text and the clipboard) and when it was last copied (for the flash).
static Calc::Result g_calc;
static std::wstring g_calcDisplay;
static bool g_calcActive = false;
static double g_copiedAt = -1e9;

// Last committed result, exposed to expressions as "ans".
static double g_calcAns = 0.0;
static bool g_calcHasAns = false;

// Calculator history: the last few calculations the user chose to keep (by pressing Enter).
// Newest first; when it is full the oldest entry is dropped.
struct CalcHistoryItem {
    std::wstring expr;    // what the user typed
    std::wstring resFmt;  // the formatted result (shown in the row)
    std::wstring insVal;  // what "Add to operation" inserts (plain number for unit results)
};
static std::vector<CalcHistoryItem> g_calcHist;
static int g_calcHistSel = -1;  // -1 = current result, otherwise the highlighted history row
static double g_histScroll = 0;  // scroll offset (DIP) of the history list
static bool g_calcPending = false;

// Start menu watcher
static HANDLE g_watchThread = nullptr;
static HANDLE g_watchStop = nullptr;

// ===== General utilities =====

static double NowSec() {
    static LARGE_INTEGER freq = [] {
        LARGE_INTEGER f;
        QueryPerformanceFrequency(&f);
        return f;
    }();
    LARGE_INTEGER c;
    QueryPerformanceCounter(&c);
    return (double)c.QuadPart / (double)freq.QuadPart;
}

static float Sc(float v) {
    return v * (float)g_scale;
}

// Compact timestamp (fits in a Wh_SetIntValue int): seconds since 2020-01-01 UTC.
static const ULONGLONG kEpoch2020 = 132223104000000000ULL;  // 2020-01-01 as FILETIME

static int NowStamp() {
    FILETIME ft;
    GetSystemTimeAsFileTime(&ft);
    ULARGE_INTEGER u;
    u.LowPart = ft.dwLowDateTime;
    u.HighPart = ft.dwHighDateTime;
    if (u.QuadPart <= kEpoch2020) return 1;
    return (int)((u.QuadPart - kEpoch2020) / 10000000ULL);
}

static std::wstring FormatFileTime(const FILETIME& utc) {
    FILETIME local;
    SYSTEMTIME st;
    if (!FileTimeToLocalFileTime(&utc, &local) || !FileTimeToSystemTime(&local, &st)) return L"";
    wchar_t d[64] = {};
    wchar_t t[64] = {};
    GetDateFormatW(LOCALE_USER_DEFAULT, DATE_SHORTDATE, &st, nullptr, d, 64);
    GetTimeFormatW(LOCALE_USER_DEFAULT, TIME_NOSECONDS, &st, nullptr, t, 64);
    return std::wstring(d) + L" " + t;
}

static std::wstring FormatStamp(int stamp) {
    ULARGE_INTEGER u;
    u.QuadPart = kEpoch2020 + (ULONGLONG)stamp * 10000000ULL;
    FILETIME ft;
    ft.dwLowDateTime = u.LowPart;
    ft.dwHighDateTime = u.HighPart;
    return FormatFileTime(ft);
}

static double EaseOutCubic(double p) {
    double q = 1.0 - p;
    return 1.0 - q * q * q;
}

// Exponential smoothing towards the target. Returns true while it is still moving.
static bool Smooth(double& cur, double target, double rate, double dt, double eps) {
    if (cur == target) {
        return false;
    }
    cur += (target - cur) * (1.0 - std::exp(-rate * dt));
    if (std::fabs(target - cur) < eps) {
        cur = target;
        return false;
    }
    return true;
}

template <class T>
static void SafeRelease(T*& p) {
    if (p) {
        p->Release();
        p = nullptr;
    }
}

// Fold a character: lowercase and accent-free (á -> a, ñ -> n). Always 1 char -> 1 char.
static wchar_t FoldChar(wchar_t c) {
    if (c < 0x80) {
        return (c >= L'A' && c <= L'Z') ? (wchar_t)(c + 32) : c;
    }
    wchar_t out[4] = {};
    int n = FoldStringW(MAP_COMPOSITE, &c, 1, out, 4);
    wchar_t base = (n > 0 && out[0]) ? out[0] : c;
    return (wchar_t)(UINT_PTR)CharLowerW((LPWSTR)(UINT_PTR)base);
}

static std::wstring FoldString(const std::wstring& s) {
    std::wstring r(s.size(), L' ');
    for (size_t i = 0; i < s.size(); ++i) {
        r[i] = FoldChar(s[i]);
    }
    return r;
}

static std::wstring GetStringSetting(const wchar_t* name) {
    PCWSTR s = Wh_GetStringSetting(name);  // never returns NULL
    std::wstring r = s;
    Wh_FreeStringSetting(s);
    return r;
}

static void LoadSettings() {
    LauncherSettings c;
    std::wstring hk = GetStringSetting(L"Hotkey");
    UINT mods = 0;
    if (hk == L"alt+space") {
        mods = MOD_ALT;
    } else if (hk == L"ctrl+space") {
        mods = MOD_CONTROL;
    } else if (hk == L"ctrl+alt+space") {
        mods = MOD_CONTROL | MOD_ALT;
    } else if (hk == L"shift+alt+space") {
        mods = MOD_SHIFT | MOD_ALT;
    } else if (hk == L"ctrl+shift+space") {
        mods = MOD_CONTROL | MOD_SHIFT;
    } else {
        mods = MOD_ALT;
    }
    c.hotkeyMods = mods;
    c.hotkeyVk = VK_SPACE;

    c.verticalPct = std::clamp(Wh_GetIntSetting(L"VerticalPosition"), 0, 100);
    c.maxRows = std::clamp(Wh_GetIntSetting(L"MaxResults"), 3, 12);
    c.opacityPct = std::clamp(Wh_GetIntSetting(L"Opacity"), 70, 100);
    {
        std::wstring ef = GetStringSetting(L"Effect");
        c.effect = (ef == L"mica")      ? BgEffect::Mica
                   : (ef == L"glass")   ? BgEffect::Glass
                   : (ef == L"frosted") ? BgEffect::Frosted
                   : (ef == L"aurora")  ? BgEffect::Aurora
                   : (ef == L"solid")   ? BgEffect::Solid
                                        : BgEffect::Acrylic;
    }
    c.blurPct = std::clamp(Wh_GetIntSetting(L"BlurAmount"), 0, 100);
    c.liveAcrylic = Wh_GetIntSetting(L"LiveAcrylic") != 0;

    {
        std::wstring col = GetStringSetting(L"Color");
        c.colorIdx = 0;
        for (int i = 0; i < (int)(sizeof(kColors) / sizeof(kColors[0])); ++i) {
            if (col == kColors[i].id) {
                c.colorIdx = i;
                break;
            }
        }
    }
    c.hideUtility = Wh_GetIntSetting(L"HideUtilityEntries") != 0;
    c.disableFullscreen = Wh_GetIntSetting(L"DisableInFullscreen") != 0;

    std::wstring rm = GetStringSetting(L"ReindexMode");
    if (rm == L"always") {
        c.reindexMode = ReindexMode::Always;
    } else if (rm == L"min1" || rm == L"min3" || rm == L"min5" || rm == L"min10") {
        c.reindexMode = ReindexMode::Interval;
        c.reindexIntervalSec = (double)_wtoi(rm.c_str() + 3) * 60.0;  // "min3" -> 3
    } else {
        c.reindexMode = ReindexMode::Auto;  // default (and if the value is not recognized)
    }

    std::wstring lm = GetStringSetting(L"ListMode");
    c.listMode = (lm == L"compact") ? ListMode::Compact : ListMode::Expanded;

    std::wstring dk = GetStringSetting(L"DetailsKey");
    c.detailsKey = (dk == L"ctrl+k")       ? DetailsKey::CtrlK
                   : (dk == L"ctrl+space") ? DetailsKey::CtrlSpace
                   : (dk == L"f4")         ? DetailsKey::F4
                                           : DetailsKey::CtrlI;
    // If the global shortcut is Ctrl+Space, that shortcut would never reach the window: use Ctrl+I.
    if (c.detailsKey == DetailsKey::CtrlSpace && c.hotkeyMods == MOD_CONTROL) {
        c.detailsKey = DetailsKey::CtrlI;
    }

    c.calculatorEnabled = Wh_GetIntSetting(L"CalculatorEnabled") != 0;
    {
        std::wstring cs = GetStringSetting(L"CalculatorStyle");
        c.calcStyle = (cs == L"hero")  ? CalcStyle::Hero
                      : (cs == L"ghost") ? CalcStyle::Ghost
                                         : CalcStyle::Both;
    }

    c.sounds = Wh_GetIntSetting(L"SoundEnabled") != 0;
    c.soundVolume = std::clamp(Wh_GetIntSetting(L"SoundVolume"), 0, 100);

    g_cfg = c;
}

// ===== Interface sounds =====
// Every sound is generated in code, so they need no audio files and no internet. Playback goes
// through XAudio2 when it is available (it lets the short typing sounds overlap naturally); if it
// is not present, the sounds are simply skipped. The sounds are deliberately quiet and short:
// rounded switch-like clicks with a low-pass, no reverb and no bright tones. Typing is a deep,
// creamy "thock" in the style of a lubed linear switch (Cherry MX Black), with a small difference
// from key to key.
namespace Sound {

static const int kRate = 44100;
static const int kVoices = 12;  // enough to overlap fast typing

typedef HRESULT(WINAPI* XAudio2Create_t)(IXAudio2**, UINT32, XAUDIO2_PROCESSOR);

struct Engine {
    HMODULE dll = nullptr;
    IXAudio2* engine = nullptr;
    IXAudio2MasteringVoice* master = nullptr;
    IXAudio2SourceVoice* voices[kVoices] = {};
    int cursor = 0;
    bool ready = false;
    float volume = 0.7f;
    WAVEFORMATEX fmt = {};
    std::vector<std::vector<int16_t>> samples;
};

static Engine g_eng;
static unsigned g_rng = 0x9E3779B9u;

static double Rand01() {
    g_rng = g_rng * 1664525u + 1013904223u;
    return (double)((g_rng >> 8) & 0xFFFFFF) / 16777216.0;
}

// ---- Sound design -------------------------------------------------------------------------
// The goal is "quiet and soft", like the click of a flashlight button: very short, rounded sounds
// with no reverb, no bright partials and a low-pass on top so nothing ever feels sharp.
//   AddTone  - a soft rounded body (a short sine whose pitch settles a little, like a plastic key
//              being pressed) with an optional faint second partial.
//   AddNoise - a tiny band-passed contact "tk", mixed in very low.
// Finish() rounds the top with a low-pass, sets a deliberately low peak level and fades the tail.
static const double kPi = 3.14159265358979323846;

static std::vector<float> NewBuf(double sec) {
    return std::vector<float>((size_t)(sec * kRate), 0.0f);
}

static void AddTone(std::vector<float>& buf, double startSec, double f0, double f1, double glideSec,
                    double attackSec, double tau, double gain, double bright, double detuneCents) {
    const int start = (int)(startSec * kRate);
    if (start >= (int)buf.size()) return;
    const double dt = 1.0 / kRate;
    const double ratio[3] = {1.0, 2.0, 3.0};
    const double amp[3] = {1.0, 0.42 * bright, 0.16 * bright};
    const double up = std::pow(2.0, detuneCents / 1200.0), down = 1.0 / up;
    double ph[3][2] = {};
    int n = (int)((attackSec + tau * 7.0) * kRate);
    for (int i = 0; i < n; ++i) {
        const int idx = start + i;
        if (idx >= (int)buf.size()) break;
        const double t = i * dt;
        const double g = glideSec > 0.0 ? 1.0 - std::exp(-t / glideSec) : 1.0;
        const double f = f0 + (f1 - f0) * g;
        double att = 1.0;
        double dec = 0.0;
        if (t < attackSec) {
            const double a = t / attackSec;
            att = a * a * (3.0 - 2.0 * a);
        } else {
            dec = t - attackSec;
        }
        double s = 0.0;
        for (int k = 0; k < 3; ++k) {
            if (amp[k] <= 0.0) continue;
            const double fk = f * ratio[k];
            ph[k][0] += 2.0 * kPi * fk * up * dt;
            ph[k][1] += 2.0 * kPi * fk * down * dt;
            const double env = att * std::exp(-dec * (1.0 + 0.9 * k) / tau);  // upper partials die first
            s += amp[k] * env * 0.5 * (std::sin(ph[k][0]) + std::sin(ph[k][1]));
        }
        buf[idx] += (float)(s * gain);
    }
}

static void AddNoise(std::vector<float>& buf, double startSec, double durSec, double fc0, double fc1,
                     double Q, double attackSec, double tau, double gain) {
    const int start = (int)(startSec * kRate);
    const int n = (int)(durSec * kRate);
    double low = 0.0, band = 0.0;
    const double q = 1.0 / Q;
    for (int i = 0; i < n; ++i) {
        const int idx = start + i;
        if (idx < 0 || idx >= (int)buf.size()) break;
        const double t = (double)i / kRate;
        const double x = (double)i / n;
        const double fc = fc0 * std::pow(fc1 / fc0, x);
        const double f = 2.0 * std::sin(kPi * fc / kRate);
        g_rng = g_rng * 1664525u + 1013904223u;
        const double in = (double)((g_rng >> 9) & 0x7FFF) / 16384.0 - 1.0;
        low += f * band;
        const double high = in - low - q * band;
        band += f * high;
        double env;
        if (t < attackSec) {
            const double a = t / attackSec;
            env = a * a * (3.0 - 2.0 * a);
        } else {
            env = std::exp(-(t - attackSec) / tau);
        }
        buf[idx] += (float)(band * env * gain);
    }
}

static void Finish(std::vector<float>& x, std::vector<int16_t>& out, double lpHz, double peak,
                   double fadeSec) {
    const double dt = 1.0 / kRate;
    const double alpha = dt / (1.0 / (2.0 * kPi * lpHz) + dt);
    double lp = 0.0, mx = 1e-9;
    for (size_t i = 0; i < x.size(); ++i) {
        lp += alpha * (x[i] - lp);
        x[i] = (float)lp;
        mx = std::max(mx, (double)std::fabs(x[i]));
    }
    const double scale = peak / mx;
    int fade = std::min((int)(fadeSec * kRate), (int)x.size());
    out.assign(x.size(), 0);
    for (size_t i = 0; i < x.size(); ++i) {
        double v = x[i] * scale;
        const int fromEnd = (int)x.size() - 1 - (int)i;
        if (fromEnd < fade) {
            const double a = (double)fromEnd / fade;
            v *= a * a * (3.0 - 2.0 * a);
        }
        out[i] = (int16_t)std::lround(std::clamp(v, -1.0, 1.0) * 32000.0);
    }
}

// Typing has one sample per key: a-z (0-25), 0-9 (26-35), space (36) and four punctuation groups.
enum { S_OPEN = 0, S_PANEL_ON = 1, S_PANEL_OFF = 2, S_TYPE = 3, S_TYPE_N = 41,
       S_ERASE = S_TYPE + S_TYPE_N, S_ERASE_N = 2, S_COUNT = S_ERASE + S_ERASE_N };

// Opening the bar: a soft two-stage switch click, press then a lighter release (flashlight button).
static void RenderOpen(std::vector<int16_t>& out) {
    std::vector<float> b = NewBuf(0.15);
    AddTone(b, 0.000, 520.0, 330.0, 0.010, 0.0008, 0.014, 1.00, 0.12, 0.0);   // press: round body
    AddNoise(b, 0.000, 0.012, 2400.0, 2400.0, 1.0, 0.0003, 0.0025, 0.30);     // soft contact
    AddTone(b, 0.048, 760.0, 560.0, 0.008, 0.0006, 0.008, 0.32, 0.05, 0.0);   // release: lighter
    AddNoise(b, 0.048, 0.008, 2800.0, 2800.0, 1.0, 0.0002, 0.0018, 0.12);
    Finish(b, out, 3000.0, 0.24, 0.040);
}

// Details panel: one tiny tick, a little higher when it opens and a little lower when it closes.
static void RenderPanel(std::vector<int16_t>& out, bool opening) {
    std::vector<float> b = NewBuf(0.10);
    if (opening) AddTone(b, 0.0, 420.0, 560.0, 0.014, 0.0008, 0.016, 1.00, 0.08, 0.0);
    else         AddTone(b, 0.0, 520.0, 380.0, 0.014, 0.0008, 0.016, 1.00, 0.08, 0.0);
    AddNoise(b, 0.0, 0.010, 2200.0, 2200.0, 1.0, 0.0003, 0.0020, 0.14);
    Finish(b, out, 2800.0, opening ? 0.17 : 0.15, 0.030);
}

// Evenly spread, repeatable "random" numbers: every key always gets the same character.
static double Hash01(int slot, int salt) {
    const double k[8] = {0.6180339887, 0.7548776662, 0.5698402910, 0.8191725134,
                         0.4142135624, 0.7320508076, 0.3247179572, 0.2360679775};
    const double v = (slot + 1) * k[salt & 7] + 0.137 * salt;
    return v - std::floor(v);
}

// Deep, creamy bottom-out: the soft "thock" of a well-lubed linear switch (Cherry MX Black style).
// A low body that settles down on impact, a couple of quiet inharmonic case resonances for the
// boxy colour, and a short, low-passed contact "clack" on top. Nothing bright: it is rounded on
// purpose so it stays creamy instead of clicky.
//   base     - resting pitch in Hz (lower = deeper)
//   tauScale - scales how long it rings
static void RenderTick(std::vector<int16_t>& out, double base, double tauScale, double peak) {
    std::vector<float> b = NewBuf(0.10);
    AddTone(b, 0.0, base * 1.20, base, 0.007, 0.0007, 0.021 * tauScale, 1.00, 0.12, 9.0);
    AddTone(b, 0.0, base * 2.32, base * 2.12, 0.006, 0.0007, 0.011 * tauScale, 0.24, 0.0, 6.0);
    AddTone(b, 0.0, base * 3.45, base * 3.18, 0.005, 0.0007, 0.007 * tauScale, 0.09, 0.0, 0.0);
    AddNoise(b, 0.0, 0.011, 1600.0, 900.0, 0.9, 0.0003, 0.0020, 0.16);
    Finish(b, out, 2400.0, peak, 0.020);
}

// Every key is the same deep thock with a slightly different pitch and ring, so typing has a
// natural, creamy variety. The space bar is a touch lower and rounder.
static void RenderType(std::vector<int16_t>& out, int slot) {
    if (slot == 36) {  // space bar: lower and rounder
        std::vector<float> b = NewBuf(0.13);
        AddTone(b, 0.0, 155.0 * 1.20, 155.0, 0.008, 0.0008, 0.030, 1.00, 0.12, 9.0);
        AddTone(b, 0.0, 155.0 * 2.32, 155.0 * 2.12, 0.007, 0.0008, 0.016, 0.22, 0.0, 6.0);
        AddNoise(b, 0.0, 0.012, 1400.0, 800.0, 0.9, 0.0003, 0.0022, 0.16);
        Finish(b, out, 2200.0, 0.090, 0.025);
        return;
    }
    const double base = 190.0 * (0.90 + 0.20 * ((slot * 17) % 41) / 40.0);  // +-10% pitch, all different
    const double tauScale = 0.90 + 0.20 * Hash01(slot, 3);
    RenderTick(out, base, tauScale, 0.070 + 0.012 * Hash01(slot, 7));
}

// Which "key" a typed character uses.
static int KeySlot(wchar_t c) {
    if (c >= L'A' && c <= L'Z') c = (wchar_t)(c - L'A' + L'a');
    if (c >= L'a' && c <= L'z') return (int)(c - L'a');
    if (c >= L'0' && c <= L'9') return 26 + (int)(c - L'0');
    if (c == L' ') return 36;
    if (c > 127) return (int)(c % 26);  // accents, ñ, etc. share the letter keys
    return 37 + (int)(c % 4);
}

// Backspace: the same thock, a bit lower and softer.
static void RenderErase(std::vector<int16_t>& out, int v) {
    RenderTick(out, 150.0 + 10.0 * v, 1.15, 0.065);
}

static void RenderAll() {
    g_eng.samples.assign(S_COUNT, {});
    RenderOpen(g_eng.samples[S_OPEN]);
    RenderPanel(g_eng.samples[S_PANEL_ON], true);
    RenderPanel(g_eng.samples[S_PANEL_OFF], false);
    for (int i = 0; i < S_TYPE_N; ++i) RenderType(g_eng.samples[S_TYPE + i], i);
    for (int i = 0; i < S_ERASE_N; ++i) RenderErase(g_eng.samples[S_ERASE + i], i);
}

static bool Init() {
    if (g_eng.samples.empty()) RenderAll();
    Engine& e = g_eng;
    e.dll = LoadLibraryExW(L"xaudio2_9.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!e.dll) e.dll = LoadLibraryExW(L"xaudio2_8.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!e.dll) return false;
    XAudio2Create_t create = (XAudio2Create_t)GetProcAddress(e.dll, "XAudio2Create");
    if (!create) return false;
    if (FAILED(create(&e.engine, 0, XAUDIO2_DEFAULT_PROCESSOR)) || !e.engine) return false;
    if (FAILED(e.engine->CreateMasteringVoice(&e.master)) || !e.master) return false;
    e.fmt.wFormatTag = WAVE_FORMAT_PCM;
    e.fmt.nChannels = 1;
    e.fmt.nSamplesPerSec = kRate;
    e.fmt.wBitsPerSample = 16;
    e.fmt.nBlockAlign = 2;
    e.fmt.nAvgBytesPerSec = kRate * 2;
    e.fmt.cbSize = 0;
    for (int i = 0; i < kVoices; ++i) {
        if (FAILED(e.engine->CreateSourceVoice(&e.voices[i], &e.fmt))) e.voices[i] = nullptr;
    }
    // XAudio2 starts its engine on creation: keep it stopped (no audio thread / render stream)
    // until the bar opens with sounds enabled.
    e.engine->StopEngine();
    e.ready = true;
    return true;
}

static void Shutdown() {
    Engine& e = g_eng;
    if (e.engine) {
        for (int i = 0; i < kVoices; ++i) {
            if (e.voices[i]) { e.voices[i]->DestroyVoice(); e.voices[i] = nullptr; }
        }
        if (e.master) { e.master->DestroyVoice(); e.master = nullptr; }
        e.engine->Release();
        e.engine = nullptr;
    }
    if (e.dll) { FreeLibrary(e.dll); e.dll = nullptr; }
    e.ready = false;
}

// The engine only runs while the bar is open and sounds are enabled.
// If there was no output device at startup, or the device was removed/changed (the engine then
// reports a critical error), rebuild the engine here before starting it.
static void Resume() {
    if (!g_cfg.sounds) return;
    Engine& e = g_eng;
    if (e.ready && e.engine && SUCCEEDED(e.engine->StartEngine())) return;
    Shutdown();
    if (Init() && e.engine) e.engine->StartEngine();
}
static void Suspend() { if (g_eng.engine) g_eng.engine->StopEngine(); }

// Creates the engine only when sounds are on; releases it completely when they are off.
static void Configure() {
    if (g_cfg.sounds) {
        if (!g_eng.ready) {
            Shutdown();  // clean up any partial init before retrying
            Init();
        }
    } else {
        Shutdown();
    }
}

static void ApplyVolume() {
    g_eng.volume = (float)std::clamp(g_cfg.soundVolume, 0, 100) / 100.0f;
}

static void Play(int idx, double pitch, double gain) {
    Engine& e = g_eng;
    if (!e.ready || idx < 0 || idx >= (int)e.samples.size()) return;
    std::vector<int16_t>& pcm = e.samples[idx];
    if (pcm.empty()) return;
    IXAudio2SourceVoice* v = e.voices[e.cursor];
    e.cursor = (e.cursor + 1) % kVoices;
    if (!v) return;
    v->Stop(0);
    v->FlushSourceBuffers();
    v->SetFrequencyRatio((float)pitch);
    v->SetVolume((float)(gain * e.volume));
    XAUDIO2_BUFFER buf = {};
    buf.AudioBytes = (UINT32)(pcm.size() * sizeof(int16_t));
    buf.pAudioData = (const BYTE*)pcm.data();
    buf.Flags = XAUDIO2_END_OF_STREAM;
    if (FAILED(v->SubmitSourceBuffer(&buf))) return;
    v->Start(0);
}

static bool On() { return g_cfg.sounds; }

static int PickVariant(int n, int& last) {
    int v = (int)(Rand01() * n) % n;
    if (v == last) v = (v + 1) % n;  // never the same variant twice in a row
    last = v;
    return v;
}

static int g_lastErase = -1;

static void OnOpen() { if (On()) Play(S_OPEN, 1.0, 1.0); }
static void OnPanel(bool opening) { if (On()) Play(opening ? S_PANEL_ON : S_PANEL_OFF, 1.0, 1.0); }
// c is the typed character; each key always sounds like itself (with a tiny human wobble).
// With no character (paste) a random letter key is used.
static void OnKey(wchar_t c = 0) {
    if (!On()) return;
    const int slot = c ? KeySlot(c) : (int)(Rand01() * 26.0) % 26;
    Play(S_TYPE + slot, 0.985 + Rand01() * 0.03, 0.88 + Rand01() * 0.12);
}
static void OnErase() {
    if (On()) Play(S_ERASE + PickVariant(S_ERASE_N, g_lastErase), 0.95 + Rand01() * 0.07, 0.85 + Rand01() * 0.15);
}

}  // namespace Sound

static D2D1_COLOR_F Mix(const D2D1_COLOR_F& a, const D2D1_COLOR_F& b, float t, float alpha = 1.f) {
    return D2D1::ColorF(a.r + (b.r - a.r) * t, a.g + (b.g - a.g) * t, a.b + (b.b - a.b) * t, alpha);
}

// Builds the whole palette from the chosen color (background + accents).
static void LoadPalette() {
    SafeRelease(g_gradBrush);  // the gradients depend on the current palette
    SafeRelease(g_glow1);
    SafeRelease(g_glow2);
    const int nColors = (int)(sizeof(kColors) / sizeof(kColors[0]));
    const ColorDef& cd = kColors[std::clamp(g_cfg.colorIdx, 0, nColors - 1)];
    const float op = g_cfg.opacityPct / 100.f;
    const D2D1_COLOR_F white = D2D1::ColorF(1.f, 1.f, 1.f, 1.f);
    const D2D1_COLOR_F A = D2D1::ColorF(cd.acc[0], cd.acc[1], cd.acc[2], 1.f);
    const D2D1_COLOR_F A2 = D2D1::ColorF(cd.acc2[0], cd.acc2[1], cd.acc2[2], 1.f);

    g_pal.light = cd.light;
    g_pal.accent = A;
    g_pal.accent2 = A2;
    g_pal.bg = D2D1::ColorF(cd.bg[0], cd.bg[1], cd.bg[2], op);

    if (cd.light) {
        // Light: dark ink with a hint of the accent; borders and keys are subtle.
        const D2D1_COLOR_F ink = Mix(D2D1::ColorF(0.09f, 0.11f, 0.16f), A, 0.10f);
        g_pal.border = D2D1::ColorF(ink.r, ink.g, ink.b, 0.18f);
        g_pal.text = ink;
        g_pal.textDim = Mix(ink, white, 0.45f);
        g_pal.selection = D2D1::ColorF(A.r, A.g, A.b, 0.13f);
        g_pal.separator = D2D1::ColorF(ink.r, ink.g, ink.b, 0.10f);
        g_pal.keycap = D2D1::ColorF(ink.r, ink.g, ink.b, 0.08f);
        g_pal.match = Mix(A, D2D1::ColorF(0.f, 0.f, 0.f), 0.10f);
        g_pal.caretSel = D2D1::ColorF(A.r, A.g, A.b, 0.25f);
        g_pal.shadowAlpha = 0.025f;
    } else {
        // Dark: near-white letters tinted with the accent, translucent layers of the same hue.
        g_pal.border = Mix(white, A, 0.30f, 0.19f);
        g_pal.text = Mix(white, A, 0.07f);
        g_pal.textDim = Mix(D2D1::ColorF(0.64f, 0.65f, 0.68f), A, 0.22f);
        g_pal.selection = D2D1::ColorF(A.r * 0.8f, A.g * 0.8f, A.b * 0.8f, 0.30f);
        g_pal.separator = Mix(white, A, 0.30f, 0.11f);
        g_pal.keycap = Mix(white, A, 0.30f, 0.13f);
        g_pal.match = Mix(A, white, 0.20f);
        g_pal.caretSel = D2D1::ColorF(A.r, A.g, A.b, 0.40f);
        g_pal.shadowAlpha = 0.040f;
    }

    if (g_matchBrush) {
        g_matchBrush->SetColor(&g_pal.match);
    }
}
// ===== Search =====

static bool IsWordStart(const std::wstring& name, size_t p) {
    if (p == 0) {
        return iswalnum(name[0]) != 0;
    }
    wchar_t prev = name[p - 1];
    wchar_t cur = name[p];
    if (!iswalnum(prev)) {
        return iswalnum(cur) != 0;
    }
    return iswlower(prev) && iswupper(cur);  // camelCase: WhatsApp -> W, A
}

// Scores a token against a name. 0 = no match. If mask != nullptr it marks the
// matching positions (to highlight them).
static int ScoreToken(const std::wstring& name,
                      const std::wstring& f,
                      const std::wstring& tok,
                      std::vector<char>* mask) {
    const size_t n = f.size();
    const size_t m = tok.size();
    if (m == 0 || n == 0) {
        return 0;
    }

    auto markRange = [&](size_t a, size_t len) {
        if (!mask) return;
        for (size_t i = a; i < a + len && i < n; ++i) (*mask)[i] = 1;
    };

    if (m <= n) {
        if (f == tok) {
            markRange(0, m);
            return 1000;
        }
        if (f.compare(0, m, tok) == 0) {
            markRange(0, m);
            return 900 - (int)std::min<size_t>(n - m, 150);
        }
        for (size_t p = 1; p + m <= n; ++p) {
            if (IsWordStart(name, p) && f.compare(p, m, tok) == 0) {
                markRange(p, m);
                return 700 - (int)std::min<size_t>(p, 100);
            }
        }
        size_t pos = f.find(tok);
        if (pos != std::wstring::npos) {
            markRange(pos, m);
            return 500 - (int)std::min<size_t>(pos, 100);
        }
    }

    // Acronyms: the initials of each word in the name
    {
        std::vector<size_t> ini;
        for (size_t p = 0; p < n; ++p) {
            if (IsWordStart(name, p)) ini.push_back(p);
        }
        if (m >= 2 && m <= ini.size()) {
            size_t k = 0;
            std::vector<size_t> hit;
            for (size_t i = 0; i < ini.size() && k < m; ++i) {
                if (f[ini[i]] == tok[k]) {
                    hit.push_back(ini[i]);
                    ++k;
                }
            }
            if (k == m) {
                if (mask) {
                    for (size_t p : hit) (*mask)[p] = 1;
                }
                return 420 - (int)std::min<size_t>(ini.size() - m, 50);
            }
        }
    }

    // Fuzzy subsequence
    if (m >= 2 && m <= n) {
        std::vector<size_t> hit;
        size_t from = 0;
        for (size_t k = 0; k < m; ++k) {
            size_t q = f.find(tok[k], from);
            if (q == std::wstring::npos) {
                return 0;
            }
            // Prefer a nearby word start
            for (size_t r = q; r < n && r <= q + 6; ++r) {
                if (f[r] == tok[k] && IsWordStart(name, r)) {
                    q = r;
                    break;
                }
            }
            hit.push_back(q);
            from = q + 1;
        }
        size_t span = hit.back() - hit.front() + 1;
        if (span > m * 3 + 6) {
            return 0;
        }
        int score = 100;
        for (size_t i = 0; i < hit.size(); ++i) {
            if (IsWordStart(name, hit[i])) score += 8;
            if (i > 0) {
                size_t gap = hit[i] - hit[i - 1] - 1;
                if (gap == 0) {
                    score += 18;
                } else {
                    score -= (int)std::min<size_t>(gap * 2, 20);
                }
            }
        }
        if (mask) {
            for (size_t p : hit) (*mask)[p] = 1;
        }
        return std::clamp(score, 20, 380);
    }

    return 0;
}

static std::vector<std::wstring> Tokenize(const std::wstring& text) {
    std::vector<std::wstring> out;
    std::wstring cur;
    for (wchar_t c : text) {
        if (c == L' ' || c == L'\t') {
            if (!cur.empty()) {
                out.push_back(FoldString(cur));
                cur.clear();
            }
        } else {
            cur.push_back(c);
        }
    }
    if (!cur.empty()) {
        out.push_back(FoldString(cur));
    }
    return out;
}

static void ReleaseResultLayouts() {
    for (auto& r : g_results) {
        SafeRelease(r.layout);
    }
}

// Vertical position (DIP, from the top of the list) of each row, counting section
// headers. Without sections it is simply i * kRowH.
static void BuildRowOffsets() {
    g_rowOff.clear();
    g_headerCount = 0;
    int prev = 0;
    float y = 0;
    for (size_t i = 0; i < g_results.size(); ++i) {
        int sec = g_results[i].section;
        if (sec != 0 && sec != prev) {
            y += kSectionH;
            ++g_headerCount;
        }
        prev = sec;
        g_rowOff.push_back(y);
        y += kRowH;
    }
    g_rowOff.push_back(y);
}

// Same as g_rowOff, but for a fractional position (highlight and scroll animations).
static double RowOffset(double pos) {
    const int n = (int)g_results.size();
    if (n == 0 || g_rowOff.size() != (size_t)n + 1) return pos * kRowH;
    if (pos <= 0) return g_rowOff[0] + pos * kRowH;
    int i = (int)std::floor(pos);
    if (i >= n) return g_rowOff[n] + (pos - n) * kRowH;
    double f = pos - i;
    double a = g_rowOff[i];
    double b = (i + 1 < n) ? g_rowOff[i + 1] : g_rowOff[i] + kRowH;
    return a + (b - a) * f;
}

// Scroll offset in DIP, measured from the first row (with scroll 0 it is 0, so the
// header of the first section keeps its slot).
static double ScrollPx() {
    return RowOffset(g_scrollPos) - RowOffset(0.0);
}

// Common Windows apps to fill "Suggestions" when there is still no usage history. They are
// looked up by fragments of the identifier (they do not change with the Windows language);
// the ones that do not exist on the machine are simply skipped.
static const wchar_t* const kDefaultSuggestions[] = {
    L"microsoft.windows.explorer", L"windowsterminal", L"windowscalculator", L"windowsnotepad",
    L"immersivecontrolpanel",     L"msedge",           L"taskmgr",           L"taskmanager",
    L"screensketch",              L"microsoft.paint",  L"windows.photos",    L"windowsstore",
    L"windowsalarms",             L"cmd.exe",          L"powershell",
};

static std::wstring LowerAscii(std::wstring s) {
    for (auto& c : s) {
        if (c >= L'A' && c <= L'Z') c = (wchar_t)(c + 32);
    }
    return s;
}

// Expanded mode with an empty search bar: "Recent" (last opened apps) and "Suggestions"
// (the most used ones and, so the list is never empty, common system apps).
// All entries are returned; the list scrolls inside the fixed panel height.
static void BuildHome() {
    std::vector<SearchResult> rec;
    for (size_t i = 0; i < g_apps.size(); ++i) {
        if (g_apps[i].lastUsed > 0) {
            SearchResult r;
            r.app = (int)i;
            r.score = g_apps[i].lastUsed;
            rec.push_back(r);
        }
    }
    std::sort(rec.begin(), rec.end(), [](const SearchResult& a, const SearchResult& b) {
        if (a.score != b.score) return a.score > b.score;
        return g_apps[a.app].folded < g_apps[b.app].folded;
    });
    if (rec.size() > kMaxRecents) rec.resize(kMaxRecents);

    std::unordered_set<int> taken;
    for (const auto& r : rec) taken.insert(r.app);

    // Suggestions: first the most used, then the common system apps
    std::vector<int> pool;
    {
        std::vector<SearchResult> byUse;
        for (size_t i = 0; i < g_apps.size(); ++i) {
            if (g_apps[i].uses > 0 && !taken.count((int)i)) {
                SearchResult r;
                r.app = (int)i;
                r.score = g_apps[i].uses;
                byUse.push_back(r);
            }
        }
        std::sort(byUse.begin(), byUse.end(), [](const SearchResult& a, const SearchResult& b) {
            if (a.score != b.score) return a.score > b.score;
            return g_apps[a.app].folded < g_apps[b.app].folded;
        });
        for (const auto& r : byUse) {
            pool.push_back(r.app);
            taken.insert(r.app);
        }

        std::vector<std::wstring> lowIds(g_apps.size());
        for (size_t i = 0; i < g_apps.size(); ++i) lowIds[i] = LowerAscii(g_apps[i].id);
        for (const wchar_t* frag : kDefaultSuggestions) {
            for (size_t i = 0; i < g_apps.size(); ++i) {
                if (!taken.count((int)i) && lowIds[i].find(frag) != std::wstring::npos) {
                    pool.push_back((int)i);
                    taken.insert((int)i);
                    break;
                }
            }
        }
    }

    const size_t nr = rec.size();
    const size_t ns = pool.size();

    for (size_t i = 0; i < nr; ++i) {
        rec[i].section = 1;
        g_results.push_back(rec[i]);
    }
    for (size_t i = 0; i < ns; ++i) {
        SearchResult r;
        r.app = pool[i];
        r.score = (int)(ns - i);
        r.section = 2;
        g_results.push_back(r);
    }
}

// ===== Calculator state =====

// True when there is a valid result to offer (copy, inline text).
static bool CalcOk() {
    return g_calcActive && g_calc.status == Calc::Status::Ok;
}

// The result card is shown for a result or an error, unless the user asked for inline only.
static bool HeroShown() {
    return g_calcActive && g_cfg.calcStyle != CalcStyle::Ghost;
}

// The faint inline text is only shown for a real result, unless the user asked for the card only.
static bool GhostShown() {
    return CalcOk() && g_cfg.calcStyle != CalcStyle::Hero;
}

// Height (DIP) reserved above the list for the result card.
static double HeroH() {
    return HeroShown() ? kHeroH : 0.0;
}

// True when the calculation is actually visible (card or inline text).
static bool CalcShown() {
    return HeroShown() || GhostShown();
}

static void UpdateCalc() {
    g_calc = Calc::Result();
    g_calcDisplay.clear();
    g_calcActive = false;
    if (!g_cfg.calculatorEnabled || g_text.empty()) {
        return;
    }
    Calc::Context ctx;
    ctx.hasAns = g_calcHasAns;
    ctx.ans = g_calcAns;
    Calc::Result r = Calc::Evaluate(g_text, Calc::SystemLocale(), ctx);
    if (r.status == Calc::Status::Ok) {
        g_calc = r;
        g_calcDisplay = r.display.empty() ? Calc::Format(r.value, Calc::SystemLocale()) : r.display;
        g_calcActive = true;
    } else if (r.status == Calc::Status::Error) {
        g_calc = r;
        g_calcActive = true;
    }
}

// SID of the current user. Windhawk's Wh_Get/SetIntValue storage is machine-wide in a regular
// install, and each signed-in user runs their own tool process, so put the SID in the value
// names; otherwise "Recent" and "Suggestions" would be shared by every account on the PC.
static std::wstring CurrentUserSid() {
    static std::wstring sid;
    if (!sid.empty()) return sid;
    HANDLE token = nullptr;
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token) && token) {
        DWORD size = 0;
        GetTokenInformation(token, TokenUser, nullptr, 0, &size);
        if (size > 0) {
            std::vector<BYTE> buf(size);
            TOKEN_USER* user = reinterpret_cast<TOKEN_USER*>(buf.data());
            if (GetTokenInformation(token, TokenUser, user, size, &size) && user->User.Sid) {
                LPWSTR str = nullptr;
                if (ConvertSidToStringSidW(user->User.Sid, &str) && str) {
                    sid = str;
                    LocalFree(str);
                }
            }
        }
        CloseHandle(token);
    }
    if (sid.empty()) sid = L"shared";  // fallback: degrade to machine-wide if the SID fails
    return sid;
}

// ===== Calculator history (persisted in the mod's local storage) =====

// Per-user names: Windhawk storage is machine-wide, so the SID keeps accounts separate.
static std::wstring CalcHistKey() { return L"calc.history_" + CurrentUserSid(); }
static const size_t kCalcHistMax = 10;

static void TrimHistField(std::wstring& s) {
    for (auto& c : s) {
        if (c == L'\n' || c == L'\r' || c == L'\t') c = L' ';
    }
}

static void LoadCalcHistory() {
    g_calcHist.clear();
    wchar_t buf[8192] = {};
    size_t n = Wh_GetStringValue(CalcHistKey().c_str(), buf, ARRAYSIZE(buf));
    if (n == 0) return;
    std::wstring all(buf, n);
    size_t start = 0;
    while (start <= all.size() && g_calcHist.size() < kCalcHistMax) {
        size_t nl = all.find(L'\n', start);
        std::wstring line =
            all.substr(start, nl == std::wstring::npos ? std::wstring::npos : nl - start);
        start = (nl == std::wstring::npos) ? all.size() + 1 : nl + 1;
        if (line.empty()) continue;
        size_t tab1 = line.find(L'\t');
        CalcHistoryItem it;
        if (tab1 == std::wstring::npos) {
            it.expr = line;
        } else {
            it.expr = line.substr(0, tab1);
            size_t tab2 = line.find(L'\t', tab1 + 1);
            if (tab2 == std::wstring::npos) {
                it.resFmt = line.substr(tab1 + 1);
            } else {
                it.resFmt = line.substr(tab1 + 1, tab2 - tab1 - 1);
                it.insVal = line.substr(tab2 + 1);
            }
        }
        if (it.insVal.empty()) it.insVal = it.resFmt;
        if (!it.expr.empty()) g_calcHist.push_back(it);
    }
}

static void SaveCalcHistory() {
    std::wstring all;
    for (size_t i = 0; i < g_calcHist.size(); ++i) {
        if (i) all += L'\n';
        all += g_calcHist[i].expr;
        all += L'\t';
        all += g_calcHist[i].resFmt;
        all += L'\t';
        all += g_calcHist[i].insVal.empty() ? g_calcHist[i].resFmt : g_calcHist[i].insVal;
    }
    Wh_SetStringValue(CalcHistKey().c_str(), all.c_str());
}

// "ans": the last committed result, persisted so it survives a restart.
static std::wstring CalcAnsKey() { return L"calc.ans_" + CurrentUserSid(); }

static void LoadCalcAns() {
    g_calcHasAns = false;
    wchar_t buf[64] = {};
    size_t n = Wh_GetStringValue(CalcAnsKey().c_str(), buf, ARRAYSIZE(buf));
    if (n == 0) return;
    double v = 0.0;
    if (Calc::detail::ParseCleanDecimal(buf, v)) {
        g_calcAns = v;
        g_calcHasAns = true;
    }
}

static void SetCalcAns(double v) {
    g_calcAns = v;
    g_calcHasAns = true;
    wchar_t buf[64] = {};
    swprintf(buf, ARRAYSIZE(buf), L"%.17g", v);
    Wh_SetStringValue(CalcAnsKey().c_str(), buf);
}

// Records a calculation the user committed (pressed Enter on). Repeating an expression moves it
// back to the top instead of duplicating it. When the list is full the oldest entry is dropped.
static void AddCalcHistory(const std::wstring& expr, const std::wstring& resFmt,
                           const std::wstring& insVal) {
    CalcHistoryItem it;
    it.expr = expr;
    it.resFmt = resFmt;
    it.insVal = insVal.empty() ? resFmt : insVal;
    TrimHistField(it.expr);
    TrimHistField(it.resFmt);
    TrimHistField(it.insVal);
    if (it.expr.empty()) return;
    for (size_t i = 0; i < g_calcHist.size();) {
        if (g_calcHist[i].expr == it.expr) {
            g_calcHist.erase(g_calcHist.begin() + i);
        } else {
            ++i;
        }
    }
    g_calcHist.insert(g_calcHist.begin(), it);
    if (g_calcHist.size() > kCalcHistMax) {
        g_calcHist.resize(kCalcHistMax);
    }
    SaveCalcHistory();
}

static void RunSearch() {
    ReleaseResultLayouts();
    g_results.clear();
    g_tokens = Tokenize(g_text);

    if (g_tokens.empty()) {
        // No query: Compact = nothing; Expanded = recents + suggestions.
        if (g_cfg.listMode == ListMode::Expanded) {
            BuildHome();
        }
        BuildRowOffsets();
        return;
    }

    // A valid calculation is the only result: it is what Enter copies, so we do not mix it with
    // app matches (a query like "2+2" should never launch an app that happened to match).
    // Exception: a lone constant ("pi", "tau", "phi", "ans") only wins when no app matches,
    // so typing the start of "Pinta" or "PicPick" keeps showing the apps.
    const bool loneConst = CalcOk() && g_calc.loneConstant;
    if (CalcOk() && !loneConst) {
        BuildRowOffsets();
        return;
    }

    for (size_t i = 0; i < g_apps.size(); ++i) {
        const AppEntry& a = g_apps[i];
        int total = 0;
        bool ok = true;
        for (const auto& t : g_tokens) {
            int s = ScoreToken(a.name, a.folded, t, nullptr);
            if (s <= 0) {
                ok = false;
                break;
            }
            total += s;
        }
        if (!ok) {
            continue;
        }
        total /= (int)g_tokens.size();
        total += std::min(a.uses, 30) * 4;
        SearchResult r;
        r.app = (int)i;
        r.score = total;
        g_results.push_back(r);
    }

    std::sort(g_results.begin(), g_results.end(),
              [](const SearchResult& a, const SearchResult& b) {
                  if (a.score != b.score) return a.score > b.score;
                  return g_apps[a.app].folded < g_apps[b.app].folded;
              });
    // Cap the number of results. This only limits how many there are; the height of the
    // bar is fixed and rows that do not fit are reached by scrolling.
    if ((int)g_results.size() > g_cfg.maxRows) {
        g_results.resize((size_t)g_cfg.maxRows);
    }
    if (loneConst && !g_results.empty()) {
        g_calc = Calc::Result();  // apps matched: drop the constant's result card
        g_calcDisplay.clear();
        g_calcActive = false;
    }
    BuildRowOffsets();
}

// ===== App indexing (background thread) =====

static bool EndsWith(const std::wstring& s, const wchar_t* suffix) {
    size_t n = wcslen(suffix);
    return s.size() >= n && s.compare(s.size() - n, n, suffix) == 0;
}

static bool HBitmapToPixels(HBITMAP hbm, std::vector<uint32_t>& px, int& w, int& h) {
    BITMAP bm = {};
    if (!GetObjectW(hbm, sizeof(bm), &bm)) {
        return false;
    }
    w = bm.bmWidth;
    h = bm.bmHeight;
    if (w <= 0 || h <= 0 || w > 512 || h > 512) {
        return false;
    }

    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = w;
    bi.bmiHeader.biHeight = -h;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;

    px.assign((size_t)w * h, 0);
    HDC hdc = GetDC(nullptr);
    int lines = GetDIBits(hdc, hbm, 0, h, px.data(), &bi, DIB_RGB_COLORS);
    ReleaseDC(nullptr, hdc);
    if (lines != h) {
        return false;
    }

    bool anyAlpha = false;
    bool needPremul = false;
    for (uint32_t p : px) {
        uint32_t a = p >> 24;
        uint32_t r = (p >> 16) & 255;
        uint32_t g = (p >> 8) & 255;
        uint32_t b = p & 255;
        if (a) anyAlpha = true;
        if (r > a || g > a || b > a) needPremul = true;
    }
    if (!anyAlpha) {
        for (auto& p : px) p |= 0xFF000000u;  // icon without alpha channel: opaque
    } else if (needPremul) {
        for (auto& p : px) {
            uint32_t a = p >> 24;
            uint32_t r = ((p >> 16) & 255) * a / 255;
            uint32_t g = ((p >> 8) & 255) * a / 255;
            uint32_t b = (p & 255) * a / 255;
            p = (a << 24) | (r << 16) | (g << 8) | b;
        }
    }
    return true;
}

// ---- Icons for packaged apps (Microsoft Store / MSIX) -----------------------
// For these apps, SIIGBF_ICONONLY usually returns the generic document icon.

// Packaged apps are identified by an AUMID like "Package_hash!App"
static bool IsPackagedId(const std::wstring& id) {
    return id.find(L'!') != std::wstring::npos && id.find(L'\\') == std::wstring::npos &&
           id.find(L'/') == std::wstring::npos && id.find(L':') == std::wstring::npos;
}

static std::wstring ItemString(IShellItem2* item, const wchar_t* canonicalName) {
    PROPERTYKEY key = {};
    if (FAILED(PSGetPropertyKeyFromName(canonicalName, &key))) {
        return L"";
    }
    std::wstring r;
    PWSTR str = nullptr;
    if (SUCCEEDED(item->GetString(key, &str)) && str) {
        r = str;
        CoTaskMemFree(str);
    }
    return r;
}

// Desktop app ids are usually "{GUID of known folder}\path\app.exe" or a full path. Serves as
// a fallback if the shell does not give the target property.
static std::wstring PathFromAppId(const std::wstring& id) {
    std::wstring path;
    if (!id.empty() && id[0] == L'{') {
        size_t end = id.find(L'}');
        if (end != std::wstring::npos) {
            GUID g;
            if (SUCCEEDED(CLSIDFromString(id.substr(0, end + 1).c_str(), &g))) {
                PWSTR base = nullptr;
                if (SUCCEEDED(SHGetKnownFolderPath(g, 0, nullptr, &base)) && base) {
                    path = base;
                    CoTaskMemFree(base);
                    if (end + 1 < id.size()) path += id.substr(end + 1);
                }
            }
        }
    } else if (id.size() > 3 && id[1] == L':' && id[2] == L'\\') {
        path = id;
    }
    return path;
}

// ---- Filter: uninstallers and documentation ----------------------------------------------
// Judged by what the shortcut REALLY runs (target and arguments), not by its name: a program
// such as "BCUninstaller" or "Revo Uninstaller" has "uninstall" in its name but is a real app.

enum UtilityKind { UtilNone = 0, UtilDoc = 1, UtilUninstaller = 2 };

static std::wstring LowerBaseName(const std::wstring& foldedPath) {
    size_t p = foldedPath.find_last_of(L"\\/");
    return p == std::wstring::npos ? foldedPath : foldedPath.substr(p + 1);
}

// `s` starts with `word` and the word ends there (end of text or a non-alphanumeric char).
static bool StartsWithWord(const std::wstring& s, const wchar_t* word) {
    const size_t n = wcslen(word);
    if (s.size() < n || s.compare(0, n, word) != 0) return false;
    return s.size() == n || !iswalnum(s[n]);
}

static bool EndsWithDocExt(std::wstring s) {
    while (!s.empty() && (s.back() == L'"' || s.back() == L' ')) s.pop_back();
    static const wchar_t* kDocExt[] = {L".url", L".chm", L".htm", L".html", L".mht",
                                       L".txt", L".rtf", L".pdf"};
    for (const wchar_t* e : kDocExt) {
        if (EndsWith(s, e)) return true;
    }
    return false;
}

// Executables that only uninstall: Inno Setup (unins000.exe), NSIS and others (uninstall.exe...).
static bool IsUninstallerExeName(const std::wstring& exe) {
    if (!EndsWith(exe, L".exe")) return false;
    const std::wstring stem = exe.substr(0, exe.size() - 4);
    static const wchar_t* kExact[] = {L"uninstall", L"uninst",   L"uninstal", L"uninstaller",
                                      L"unwise",    L"unwise32", L"un_a"};
    for (const wchar_t* e : kExact) {
        if (stem == e) return true;
    }
    // unins000, uninst64, uninstall32...: base word followed only by digits
    static const wchar_t* kBase[] = {L"unins", L"uninst", L"uninstall"};
    for (const wchar_t* b : kBase) {
        const size_t n = wcslen(b);
        if (stem.size() > n && stem.compare(0, n, b) == 0) {
            bool digits = true;
            for (size_t i = n; i < stem.size(); ++i) {
                if (!iswdigit(stem[i])) {
                    digits = false;
                    break;
                }
            }
            if (digits) return true;
        }
    }
    return false;
}

// "Uninstall X" / "Desinstalar X" style names (used only when the target is unknown).
static bool NameIsUninstallCommand(const std::wstring& fn) {
    static const wchar_t* kVerbs[] = {L"uninstall",      L"un-install",    L"desinstalar",
                                      L"desinstalacion", L"desinstalador", L"deinstallieren",
                                      L"desinstaller",   L"disinstalla"};
    for (const wchar_t* v : kVerbs) {
        if (StartsWithWord(fn, v)) return true;
    }
    return EndsWith(fn, L" uninstall") || EndsWith(fn, L" desinstalar");
}

static bool ArgsLookLikeUninstall(const std::wstring& foldedArgs) {
    return foldedArgs.find(L"uninstall") != std::wstring::npos ||
           foldedArgs.find(L"removeonly") != std::wstring::npos;
}

// Returns UtilNone, UtilDoc or UtilUninstaller.
static int ClassifyUtility(IShellItem* item, const std::wstring& name, const std::wstring& id) {
    if (IsPackagedId(id)) return UtilNone;  // Store apps are neither uninstallers nor documents

    const std::wstring fn = FoldString(name);
    const std::wstring fi = FoldString(id);

    std::wstring target, args;
    IShellItem2* item2 = nullptr;
    if (SUCCEEDED(item->QueryInterface(IID_PPV_ARGS(&item2))) && item2) {
        target = ItemString(item2, L"System.Link.TargetParsingPath");
        args = ItemString(item2, L"System.Link.Arguments");
        item2->Release();
    }
    if (target.empty()) target = PathFromAppId(id);

    const std::wstring ft = FoldString(target);
    const std::wstring fa = FoldString(args);
    const std::wstring exe = LowerBaseName(ft);

    // Documentation: a shortcut to a document or web page, or a "Readme"-style name
    if (EndsWithDocExt(fi) || EndsWithDocExt(ft) || EndsWithDocExt(fa)) return UtilDoc;
    static const wchar_t* kDocNames[] = {L"readme", L"read me", L"leeme", L"release notes",
                                         L"notas de la version"};
    for (const wchar_t* n : kDocNames) {
        if (StartsWithWord(fn, n)) return UtilDoc;
    }

    if (!ft.empty()) {
        // The target is known: it decides. Only things that really uninstall are hidden;
        // anything else (even with "uninstall" in the name) is a normal program.
        if (exe == L"msiexec.exe") return UtilUninstaller;
        if (IsUninstallerExeName(exe)) return UtilUninstaller;
        if (ArgsLookLikeUninstall(fa)) return UtilUninstaller;
        return UtilNone;
    }
    // Unknown target (for example advertised MSI shortcuts): only the imperative name counts.
    return NameIsUninstallCommand(fn) ? UtilUninstaller : UtilNone;
}


struct Pix {
    std::vector<uint32_t> px;
    int w = 0;
    int h = 0;
    bool ok = false;
    uint64_t hash = 0;
};

static uint64_t HashPixels(const std::vector<uint32_t>& px, int w, int h) {
    uint64_t hsh = 1469598103934665603ULL ^ ((uint64_t)(uint32_t)w << 32) ^ (uint64_t)(uint32_t)h;
    for (uint32_t p : px) {
        hsh ^= p;
        hsh *= 1099511628211ULL;
    }
    return hsh;
}

// Shell icon of an IShellItem. WOW64 redirection stays enabled: icon resolution can load shell
// handlers and system DLLs, and switching redirection off around that could leave a failed DLL
// load behind for the whole session. 64-bit-only System32 tools are covered by
// GetClassicIconFallback (below), which reads their icons from Sysnative paths with plain file
// APIs. Packaged (Store) apps take their logo from the package and are left untouched.
static bool GetShellItemPixels(IShellItem* item, SIIGBF flags, int size, Pix& out) {
    IShellItemImageFactory* fac = nullptr;
    if (FAILED(item->QueryInterface(IID_PPV_ARGS(&fac))) || !fac) {
        return false;
    }
    SIZE sz = {size, size};
    HBITMAP hbm = nullptr;
    bool ok = false;
    const HRESULT hr = fac->GetImage(sz, flags, &hbm);
    if (SUCCEEDED(hr) && hbm) {
        ok = HBitmapToPixels(hbm, out.px, out.w, out.h);
        DeleteObject(hbm);
    }
    fac->Release();
    out.ok = ok;
    if (ok) {
        out.hash = HashPixels(out.px, out.w, out.h);
    }
    return ok;
}

// Case-insensitive prefix test.
static bool StartsWithNoCase(const std::wstring& s, const std::wstring& prefix) {
    if (s.size() < prefix.size()) return false;
    for (size_t i = 0; i < prefix.size(); ++i) {
        if (towlower(s[i]) != towlower(prefix[i])) return false;
    }
    return true;
}

// The mod runs inside the 32-bit windhawk.exe tool process, where the WOW64 redirector maps
// "C:\Windows\System32" to "C:\Windows\SysWOW64". 64-bit-only tools in the real System32
// (mdsched.exe, voiceaccess.exe, ...) are therefore invisible by path. "Sysnative" is the
// WOW64 alias for the real System32 (only valid from a 32-bit process), so paths under
// "<Windows>\System32\" are rewritten to it.
static std::wstring NativeSystemPath(const std::wstring& path) {
    static const bool wow64 = [] {
        BOOL w = FALSE;
        return IsWow64Process(GetCurrentProcess(), &w) && w;
    }();
    if (!wow64 || path.empty()) return path;
    wchar_t win[MAX_PATH];
    UINT n = GetWindowsDirectoryW(win, MAX_PATH);
    if (n == 0 || n >= MAX_PATH) return path;
    const std::wstring prefix = std::wstring(win) + L"\\System32\\";
    if (StartsWithNoCase(path, prefix)) {
        return std::wstring(win) + L"\\Sysnative\\" + path.substr(prefix.size());
    }
    return path;
}

// True when the path is inside the Windows directory (the only place affected by the WOW64
// redirection, and where a path must be resolved with the redirection disabled instead).
static bool IsUnderWindowsDir(const std::wstring& path) {
    wchar_t win[MAX_PATH];
    UINT n = GetWindowsDirectoryW(win, MAX_PATH);
    if (n == 0 || n >= MAX_PATH) return false;
    return StartsWithNoCase(path, std::wstring(win) + L"\\");
}

// ---- Fallback icon sources (system image list) --------------------------------------------
// Used when IShellItemImageFactory fails (system tools, MSI-advertised shortcuts such as
// Node.js, shortcuts whose icon lives in an installer cache, etc.).

static const GUID kIidImageList = {0x46EB5926, 0x582E, 0x4017, {0x9F, 0xDF, 0xE8, 0x99, 0x8D, 0xAA, 0x09, 0x50}};

// HICON -> premultiplied BGRA. Handles icons with an alpha channel and legacy mask icons.
static bool HIconToPix(HICON hi, Pix& out) {
    ICONINFO ii = {};
    if (!hi || !GetIconInfo(hi, &ii)) return false;
    bool ok = false;
    if (ii.hbmColor) {
        BITMAP bm = {};
        if (GetObjectW(ii.hbmColor, sizeof(bm), &bm) && bm.bmWidth > 0 && bm.bmHeight > 0 &&
            bm.bmWidth <= 512 && bm.bmHeight <= 512) {
            const int w = bm.bmWidth, h = bm.bmHeight;
            BITMAPINFO bi = {};
            bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
            bi.bmiHeader.biWidth = w;
            bi.bmiHeader.biHeight = -h;
            bi.bmiHeader.biPlanes = 1;
            bi.bmiHeader.biBitCount = 32;
            bi.bmiHeader.biCompression = BI_RGB;
            std::vector<uint32_t> px((size_t)w * h, 0);
            HDC hdc = GetDC(nullptr);
            bool got = GetDIBits(hdc, ii.hbmColor, 0, h, px.data(), &bi, DIB_RGB_COLORS) == h;
            if (got) {
                bool anyAlpha = false;
                for (uint32_t p : px) {
                    if (p >> 24) { anyAlpha = true; break; }
                }
                if (!anyAlpha) {
                    // Legacy icon: transparency comes from the AND mask.
                    std::vector<uint32_t> mk((size_t)w * h, 0);
                    bool haveMask = ii.hbmMask &&
                                    GetDIBits(hdc, ii.hbmMask, 0, h, mk.data(), &bi, DIB_RGB_COLORS) == h;
                    for (size_t i = 0; i < px.size(); ++i) {
                        const bool transparent = haveMask && (mk[i] & 0xFF) != 0;
                        px[i] = transparent ? 0u : (px[i] | 0xFF000000u);
                    }
                } else {
                    for (auto& p : px) {
                        uint32_t a = p >> 24;
                        if (a == 255) continue;
                        uint32_t r = ((p >> 16) & 255) * a / 255;
                        uint32_t g = ((p >> 8) & 255) * a / 255;
                        uint32_t b = (p & 255) * a / 255;
                        p = (a << 24) | (r << 16) | (g << 8) | b;
                    }
                }
                out.px = std::move(px);
                out.w = w;
                out.h = h;
                ok = true;
            }
            ReleaseDC(nullptr, hdc);
        }
    }
    if (ii.hbmColor) DeleteObject(ii.hbmColor);
    if (ii.hbmMask) DeleteObject(ii.hbmMask);
    return ok;
}

// Box-downscale by an integer factor so 256 px jumbo icons end up at 64 px.
static void DownscalePix(Pix& p, int factor) {
    if (factor <= 1) return;
    const int nw = p.w / factor, nh = p.h / factor;
    if (nw <= 0 || nh <= 0) return;
    std::vector<uint32_t> o((size_t)nw * nh);
    for (int y = 0; y < nh; ++y) {
        for (int x = 0; x < nw; ++x) {
            uint32_t a = 0, r = 0, g = 0, b = 0;
            for (int yy = 0; yy < factor; ++yy) {
                for (int xx = 0; xx < factor; ++xx) {
                    uint32_t v = p.px[(size_t)(y * factor + yy) * p.w + (x * factor + xx)];
                    a += v >> 24;
                    r += (v >> 16) & 255;
                    g += (v >> 8) & 255;
                    b += v & 255;
                }
            }
            const uint32_t n = (uint32_t)(factor * factor);
            o[(size_t)y * nw + x] = ((a / n) << 24) | ((r / n) << 16) | ((g / n) << 8) | (b / n);
        }
    }
    p.px = std::move(o);
    p.w = nw;
    p.h = nh;
}

// Icon from the system image list. `pidlOrPath` is a PIDL when isPidl, else a file path.
static bool GetSystemImageListIcon(const void* pidlOrPath, bool isPidl, Pix& out) {
    SHFILEINFOW sfi = {};
    UINT flags = SHGFI_SYSICONINDEX | (isPidl ? SHGFI_PIDL : 0);
    if (!SHGetFileInfoW((LPCWSTR)pidlOrPath, 0, &sfi, sizeof(sfi), flags)) return false;

    // JUMBO (256) first, then EXTRALARGE (48), then LARGE (32).
    static const int kLists[] = {SHIL_JUMBO, SHIL_EXTRALARGE, SHIL_LARGE};
    for (int shil : kLists) {
        IImageList* il = nullptr;
        if (FAILED(SHGetImageList(shil, kIidImageList, (void**)&il)) || !il) continue;
        HICON hi = nullptr;
        bool ok = SUCCEEDED(il->GetIcon(sfi.iIcon, ILD_TRANSPARENT, &hi)) && hi && HIconToPix(hi, out);
        if (hi) DestroyIcon(hi);
        il->Release();
        if (ok) {
            if (out.w > 64 && out.w % 64 == 0) DownscalePix(out, out.w / 64);
            out.ok = true;
            out.hash = HashPixels(out.px, out.w, out.h);
            return true;
        }
    }
    return false;
}

// True if the image has a meaningful amount of visible (non-transparent) pixels.
static bool HasVisiblePixels(const Pix& p) {
    if (!p.ok || p.px.empty()) return false;
    size_t vis = 0;
    for (uint32_t v : p.px) {
        if ((v >> 24) > 24) ++vis;
    }
    return vis * 50 >= p.px.size();  // at least 2% of the pixels
}

// Icon straight from the resource of an exe/dll/ico (hi-res, bypasses the shell cache).
static bool GetFileIconPix(const std::wstring& path, Pix& out) {
    if (path.empty()) return false;
    const std::wstring real = NativeSystemPath(path);
    HICON hi = nullptr;
    UINT id = 0;
    UINT n = PrivateExtractIconsW(real.c_str(), 0, 64, 64, &hi, &id, 1, 0);
    bool ok = n >= 1 && n != 0xFFFFFFFFu && hi && HIconToPix(hi, out);
    if (hi) DestroyIcon(hi);
    if (ok) {
        out.ok = true;
        out.hash = HashPixels(out.px, out.w, out.h);
    }
    return ok;
}

// Tries every alternative source for a desktop app. Each result must have visible pixels.
static bool GetClassicIconFallback(IShellItem* item, const std::wstring& target, Pix& out) {
    auto good = [&](bool got) {
        if (got && HasVisiblePixels(out)) return true;
        out = Pix();
        return false;
    };
    if (!target.empty() && good(GetFileIconPix(target, out))) return true;

    PIDLIST_ABSOLUTE pidl = nullptr;
    if (SUCCEEDED(SHGetIDListFromObject(item, &pidl)) && pidl) {
        bool ok = good(GetSystemImageListIcon(pidl, true, out));
        CoTaskMemFree(pidl);
        if (ok) return true;
    }
    const std::wstring realTarget = NativeSystemPath(target);
    if (!realTarget.empty() && good(GetSystemImageListIcon(realTarget.c_str(), false, out))) return true;
    if (good(GetShellItemPixels(item, (SIIGBF)(SIIGBF_ICONONLY | SIIGBF_BIGGERSIZEOK), 64, out))) return true;
    if (good(GetShellItemPixels(item, SIIGBF_RESIZETOFIT, 64, out))) return true;
    return false;
}

// Path of the small logo PNG for a packaged app (property System.Tile.SmallLogoPath)
static std::wstring GetTileLogoPath(IShellItem* item) {
    PROPERTYKEY key = {};
    if (FAILED(PSGetPropertyKeyFromName(L"System.Tile.SmallLogoPath", &key))) {
        return L"";
    }
    IShellItem2* item2 = nullptr;
    if (FAILED(item->QueryInterface(IID_PPV_ARGS(&item2))) || !item2) {
        return L"";
    }
    std::wstring r;
    PWSTR str = nullptr;
    if (SUCCEEDED(item2->GetString(key, &str)) && str) {
        r = str;
        CoTaskMemFree(str);
    }
    item2->Release();
    return r;
}

// Loads a PNG (or other image) with WIC as premultiplied BGRA, scaled to at most 64 px
static bool LoadImageFilePixels(IWICImagingFactory* wic,
                                const std::wstring& path,
                                std::vector<uint32_t>& px,
                                int& w,
                                int& h) {
    if (!wic) {
        return false;
    }
    IWICBitmapDecoder* dec = nullptr;
    IWICBitmapFrameDecode* frame = nullptr;
    IWICBitmapScaler* scaler = nullptr;
    IWICFormatConverter* conv = nullptr;
    bool ok = false;

    if (SUCCEEDED(wic->CreateDecoderFromFilename(path.c_str(), nullptr, GENERIC_READ,
                                                 WICDecodeMetadataCacheOnDemand, &dec)) &&
        dec && SUCCEEDED(dec->GetFrame(0, &frame)) && frame) {
        UINT fw = 0, fh = 0;
        frame->GetSize(&fw, &fh);
        if (fw > 0 && fh > 0 && fw <= 4096 && fh <= 4096) {
            IWICBitmapSource* src = frame;
            UINT tw = fw, th = fh;
            const UINT kMax = 64;
            if (fw > kMax || fh > kMax) {
                UINT nw = fw, nh = fh;
                if (fw >= fh) {
                    nw = kMax;
                    nh = std::max<UINT>(1, fh * kMax / fw);
                } else {
                    nh = kMax;
                    nw = std::max<UINT>(1, fw * kMax / fh);
                }
                if (SUCCEEDED(wic->CreateBitmapScaler(&scaler)) && scaler &&
                    SUCCEEDED(scaler->Initialize(frame, nw, nh, WICBitmapInterpolationModeFant))) {
                    src = scaler;
                    tw = nw;
                    th = nh;
                }
            }
            if (SUCCEEDED(wic->CreateFormatConverter(&conv)) && conv &&
                SUCCEEDED(conv->Initialize(src, GUID_WICPixelFormat32bppPBGRA,
                                           WICBitmapDitherTypeNone, nullptr, 0.0,
                                           WICBitmapPaletteTypeCustom))) {
                px.assign((size_t)tw * th, 0);
                if (SUCCEEDED(conv->CopyPixels(nullptr, tw * 4, (UINT)(px.size() * 4),
                                               (BYTE*)px.data()))) {
                    w = (int)tw;
                    h = (int)th;
                    ok = true;
                }
            }
        }
    }
    SafeRelease(conv);
    SafeRelease(scaler);
    SafeRelease(frame);
    SafeRelease(dec);
    return ok;
}

static DWORD WINAPI IndexThreadProc(LPVOID param) {
    std::unique_ptr<IndexJob> job((IndexJob*)param);
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    struct Tmp {
        std::wstring folded;
        RawApp app;
        IShellItem* item;
    };
    std::vector<Tmp> tmp;

    IShellItem* folder = nullptr;
    HRESULT hr = SHCreateItemFromParsingName(L"shell:AppsFolder", nullptr, IID_PPV_ARGS(&folder));
    if (SUCCEEDED(hr) && folder) {
        IEnumShellItems* en = nullptr;
        hr = folder->BindToHandler(nullptr, BHID_EnumItems, IID_PPV_ARGS(&en));
        if (SUCCEEDED(hr) && en) {
            std::unordered_set<std::wstring> seenNames;
            IShellItem* it = nullptr;
            ULONG got = 0;
            while (en->Next(1, &it, &got) == S_OK && got == 1) {
                PWSTR dn = nullptr;
                PWSTR pn = nullptr;
                it->GetDisplayName(SIGDN_NORMALDISPLAY, &dn);
                it->GetDisplayName(SIGDN_PARENTRELATIVEPARSING, &pn);
                bool keep = dn && pn && *dn && *pn;
                std::wstring name = dn ? dn : L"";
                std::wstring id = pn ? pn : L"";
                if (dn) CoTaskMemFree(dn);
                if (pn) CoTaskMemFree(pn);

                if (keep && job->hideUtility && ClassifyUtility(it, name, id) != UtilNone) {
                    keep = false;
                }
                if (keep) {
                    std::wstring folded = FoldString(name);
                    if (!seenNames.insert(folded).second) {
                        keep = false;  // duplicate name
                    } else {
                        Tmp t;
                        t.folded = folded;
                        t.app.name = name;
                        t.app.id = id;
                        t.item = it;
                        tmp.push_back(std::move(t));
                    }
                }
                if (!keep) {
                    it->Release();
                }
                it = nullptr;
            }
            en->Release();
        }
        folder->Release();
    }

    std::sort(tmp.begin(), tmp.end(),
              [](const Tmp& a, const Tmp& b) { return a.folded < b.folded; });

    // 1) Send the app list (without icons) to the UI
    {
        auto* res = new IndexResult();
        res->gen = job->gen;
        res->apps.reserve(tmp.size());
        for (auto& t : tmp) res->apps.push_back(t.app);
        if (!PostMessageW(job->hwnd, WM_APP_INDEX, 0, (LPARAM)res)) {
            delete res;
        }
    }

    // 2) Load icons. Classic apps use the shell icon (ICONONLY). Packaged apps try, in order,
    //    ICONONLY, the normal shell tile and the app logo PNG; an image repeated across 2+
    //    packaged apps is the generic icon and is discarded.
    struct Shot {
        Pix a;  // ICONONLY
        Pix b;  // normal shell image
    };
    std::vector<Shot> shots(tmp.size());
    std::unordered_map<uint64_t, int> countA;
    std::unordered_map<uint64_t, int> countB;
    std::unordered_map<uint64_t, int> countC;  // classic apps: repeated image = generic icon
    for (size_t i = 0; i < tmp.size(); ++i) {
        if (job->haveIcons.count(tmp[i].app.id)) {
            continue;
        }
        if (!IsPackagedId(tmp[i].app.id)) {
            if (GetShellItemPixels(tmp[i].item, SIIGBF_ICONONLY, 64, shots[i].a)) {
                ++countC[shots[i].a.hash];
            }
            continue;
        }
        if (GetShellItemPixels(tmp[i].item, SIIGBF_ICONONLY, 64, shots[i].a)) {
            ++countA[shots[i].a.hash];
        }
        if (GetShellItemPixels(tmp[i].item, SIIGBF_RESIZETOFIT, 64, shots[i].b)) {
            ++countB[shots[i].b.hash];
        }
    }

    IWICImagingFactory* wic = nullptr;
    for (size_t i = 0; i < tmp.size(); ++i) {
        const std::wstring& id = tmp[i].app.id;
        if (job->haveIcons.count(id)) {
            continue;
        }
        auto* ir = new IconResult();
        ir->gen = job->gen;
        ir->index = i;
        ir->w = 0;
        ir->h = 0;
        bool have = false;

        auto take = [&](Pix& p) {
            ir->px = std::move(p.px);
            ir->w = p.w;
            ir->h = p.h;
            have = true;
        };

        if (!IsPackagedId(id)) {
            Pix& a = shots[i].a;
            // The shell may return a blank or generic icon, so only trust the result
            // when it is visible and not repeated across 3+ apps.
            const bool suspicious = a.ok && (!HasVisiblePixels(a) || countC[a.hash] >= 3);
            if (a.ok && !suspicious) {
                take(a);
            } else {
                std::wstring target;
                IShellItem2* it2 = nullptr;
                if (SUCCEEDED(tmp[i].item->QueryInterface(IID_PPV_ARGS(&it2))) && it2) {
                    target = ItemString(it2, L"System.Link.TargetParsingPath");
                    it2->Release();
                }
                if (target.empty()) target = PathFromAppId(id);
                Pix f;
                if (GetClassicIconFallback(tmp[i].item, target, f)) {
                    take(f);
                } else if (a.ok) {
                    take(a);  // nothing better found: keep what the shell gave
                }
            }
        } else {
            Shot& s = shots[i];
            if (s.a.ok && countA[s.a.hash] < 2) {
                take(s.a);
            } else if (s.b.ok && countB[s.b.hash] < 2) {
                take(s.b);
            } else {
                std::wstring logo = GetTileLogoPath(tmp[i].item);
                if (!logo.empty() && GetFileAttributesW(logo.c_str()) != INVALID_FILE_ATTRIBUTES) {
                    if (!wic) {
                        CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER,
                                         IID_PPV_ARGS(&wic));
                    }
                    if (LoadImageFilePixels(wic, logo, ir->px, ir->w, ir->h)) {
                        have = true;
                    }
                }
                if (!have) {
                    Pix f;
                    PIDLIST_ABSOLUTE pidl = nullptr;
                    if (SUCCEEDED(SHGetIDListFromObject(tmp[i].item, &pidl)) && pidl) {
                        if (GetSystemImageListIcon(pidl, true, f)) {
                            take(f);
                        }
                        CoTaskMemFree(pidl);
                    }
                }
            }
        }

        if (!have || !PostMessageW(job->hwnd, WM_APP_ICON, 0, (LPARAM)ir)) {
            delete ir;
        }
    }
    SafeRelease(wic);

    for (auto& t : tmp) {
        t.item->Release();
    }

    PostMessageW(job->hwnd, WM_APP_INDEX_DONE, job->gen, 0);
    CoUninitialize();
    return 0;
}

// `refreshIcons` also reloads icons that are already cached (manual reindex).
static void StartIndex(bool refreshIcons = false) {
    if (g_indexing || !g_hwnd) {
        return;
    }
    g_indexing = true;
    g_lastIndexAt = NowSec();
    g_menuDirty = false;  // whatever changes from now on will set it again
    ++g_gen;

    auto* job = new IndexJob();
    job->hwnd = g_hwnd;
    job->gen = g_gen;
    job->hideUtility = g_cfg.hideUtility;
    for (const auto& a : g_apps) {
        if (!refreshIcons && !a.iconPx.empty()) job->haveIcons.insert(a.id);
    }

    HANDLE th = CreateThread(nullptr, 0, IndexThreadProc, job, 0, nullptr);
    if (!th) {
        delete job;
        g_indexing = false;
        g_menuDirty = true;  // could not index: retry next time
        return;
    }
    CloseHandle(th);
}

// Whether to reindex on opening, according to the "App index updates" setting:
//  - Automatic: only if the watcher saw changes, or after 10 minutes as a fallback for
//    Store apps that do not touch the Start menu folders.
//  - Every time it opens: always.
//  - Interval: once the chosen time has passed since the last reindex.
static bool ShouldReindexOnOpen() {
    double elapsed = NowSec() - g_lastIndexAt;
    switch (g_cfg.reindexMode) {
        case ReindexMode::Always:
            return true;
        case ReindexMode::Interval:
            return elapsed >= g_cfg.reindexIntervalSec;
        case ReindexMode::Auto:
        default:
            return g_menuDirty || elapsed >= kAutoSafetyRescanSec;
    }
}

// ===== Start menu watcher (change detection, Automatic mode) =====
// The two Start menu folders (all users and current user) are watched. Installers fire many
// events in a row and Windows needs a few seconds to register the shortcut, so the UI is only
// notified once the events stop arriving.

static std::wstring KnownFolderPath(REFKNOWNFOLDERID id) {
    std::wstring r;
    PWSTR p = nullptr;
    if (SUCCEEDED(SHGetKnownFolderPath(id, 0, nullptr, &p)) && p) {
        r = p;
        CoTaskMemFree(p);
    }
    return r;
}

static DWORD WINAPI WatchThreadProc(LPVOID param) {
    HWND hwnd = (HWND)param;
    const std::wstring dirs[2] = {KnownFolderPath(FOLDERID_CommonPrograms),
                                  KnownFolderPath(FOLDERID_Programs)};
    const DWORD filter =
        FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME | FILE_NOTIFY_CHANGE_LAST_WRITE;

    HANDLE handles[3];
    DWORD n = 0;
    for (const auto& d : dirs) {
        if (d.empty()) continue;
        HANDLE h = FindFirstChangeNotificationW(d.c_str(), TRUE, filter);
        if (h == INVALID_HANDLE_VALUE) {
            continue;
        }
        handles[n++] = h;
    }
    const DWORD stopIdx = n;
    handles[n++] = g_watchStop;

    if (stopIdx == 0) {
        return 0;
    }

    for (;;) {
        DWORD w = WaitForMultipleObjects(n, handles, FALSE, INFINITE);
        if (w >= WAIT_OBJECT_0 + stopIdx) break;  // stop, or error
        FindNextChangeNotification(handles[w - WAIT_OBJECT_0]);

        // Debounce: wait until it goes quiet.
        bool stop = false;
        for (;;) {
            DWORD w2 = WaitForMultipleObjects(n, handles, FALSE, kWatchDebounceMs);
            if (w2 == WAIT_TIMEOUT) break;
            if (w2 >= WAIT_OBJECT_0 + stopIdx) {
                stop = true;
                break;
            }
            FindNextChangeNotification(handles[w2 - WAIT_OBJECT_0]);
        }
        if (stop) break;

        PostMessageW(hwnd, WM_APP_MENU_CHANGED, 0, 0);
    }

    for (DWORD i = 0; i < stopIdx; ++i) {
        FindCloseChangeNotification(handles[i]);
    }
    return 0;
}

static void StartWatcher() {
    if (g_watchThread || !g_hwnd) {
        return;
    }
    g_watchStop = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_watchStop) {
        return;
    }
    g_watchThread = CreateThread(nullptr, 0, WatchThreadProc, g_hwnd, 0, nullptr);
    if (!g_watchThread) {
        CloseHandle(g_watchStop);
        g_watchStop = nullptr;
    }
}

static void StopWatcher() {
    if (g_watchStop) {
        SetEvent(g_watchStop);
    }
    if (g_watchThread) {
        WaitForSingleObject(g_watchThread, 3000);
        CloseHandle(g_watchThread);
        g_watchThread = nullptr;
    }
    if (g_watchStop) {
        CloseHandle(g_watchStop);
        g_watchStop = nullptr;
    }
}

// Automatic mode: with pending changes, reindex in the background instead of waiting for the
// bar to open, only while it is hidden (applying an index resets the selection).
static void MaybeReindexInBackground() {
    if (g_cfg.reindexMode == ReindexMode::Auto && g_menuDirty && !g_indexing &&
        g_vis == Vis::Hidden) {
        StartIndex();
    }
}

static std::wstring UsageKey(const std::wstring& id) {
    uint64_t h = 1469598103934665603ULL;
    for (wchar_t c : id) {
        h ^= (uint64_t)c;
        h *= 1099511628211ULL;
    }
    wchar_t hex[17];
    swprintf_s(hex, L"%016llx", (unsigned long long)h);
    return L"u" + CurrentUserSid() + L"_" + hex;
}

static void UpdateTargets(bool snap, bool followSelection);
static void MarkDirty();

static void ApplyIndex(IndexResult& r) {
    // Remember the selected app so a reindex while the bar is open does not reset the selection.
    std::wstring keepId;
    if (g_sel >= 0 && g_sel < (int)g_results.size()) keepId = g_apps[g_results[g_sel].app].id;

    std::unordered_map<std::wstring, size_t> old;
    for (size_t i = 0; i < g_apps.size(); ++i) {
        old[g_apps[i].id] = i;
    }
    std::vector<char> taken(g_apps.size(), 0);

    std::vector<AppEntry> next;
    next.reserve(r.apps.size());
    for (auto& ra : r.apps) {
        AppEntry a;
        a.name = ra.name;
        a.id = ra.id;
        a.folded = FoldString(a.name);
        a.usageKey = UsageKey(a.id);
        a.uses = Wh_GetIntValue(a.usageKey.c_str(), 0);
        a.lastUsed = Wh_GetIntValue((a.usageKey + L"t").c_str(), 0);
        auto it = old.find(a.id);
        if (it != old.end()) {
            AppEntry& o = g_apps[it->second];
            a.iconW = o.iconW;
            a.iconH = o.iconH;
            a.iconPx = std::move(o.iconPx);
            a.bmp = o.bmp;
            o.bmp = nullptr;
            taken[it->second] = 1;
        }
        next.push_back(std::move(a));
    }
    for (size_t i = 0; i < g_apps.size(); ++i) {
        if (!taken[i]) SafeRelease(g_apps[i].bmp);
    }

    ReleaseResultLayouts();
    g_results.clear();
    g_apps.swap(next);
    g_detailsCache.clear();  // paths/versions may have changed

    RunSearch();
    g_sel = 0;
    for (size_t i = 0; i < g_results.size() && !keepId.empty(); ++i) {
        if (g_apps[g_results[i].app].id == keepId) {
            g_sel = (int)i;
            break;
        }
    }
    if (g_sel == 0) g_scrollTarget = 0;
    UpdateTargets(false, /*followSelection=*/true);
    MarkDirty();
}

// Manual reindex (footer button or Ctrl+R): re-reads the app list and reloads every icon.
static void ForceReindex() {
    if (g_indexing) return;
    StartIndex(true);
    MarkDirty();
}

// ===== Surface (DIB) and Direct2D / DirectWrite resources =====

typedef HRESULT(WINAPI* GetDpiForMonitor_t)(HMONITOR, int, UINT*, UINT*);

static double MonitorScale(HMONITOR mon) {
    static GetDpiForMonitor_t fn = [] {
        HMODULE m = LoadLibraryExW(L"shcore.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
        return m ? (GetDpiForMonitor_t)GetProcAddress(m, "GetDpiForMonitor") : nullptr;
    }();
    UINT dx = 96, dy = 96;
    if (fn && SUCCEEDED(fn(mon, 0 /*MDT_EFFECTIVE_DPI*/, &dx, &dy)) && dx > 0) {
        return dx / 96.0;
    }
    return 1.0;
}

static void DestroySurface() {
    if (g_memDC) {
        if (g_oldBmp) {
            SelectObject(g_memDC, g_oldBmp);
            g_oldBmp = nullptr;
        }
    }
    if (g_dib) {
        DeleteObject(g_dib);
        g_dib = nullptr;
    }
}

static const wchar_t* PickFontFamily() {
    bool ok = false;
    IDWriteFontCollection* col = nullptr;
    if (SUCCEEDED(g_dw->GetSystemFontCollection(&col, FALSE)) && col) {
        UINT32 idx = 0;
        BOOL exists = FALSE;
        if (SUCCEEDED(col->FindFamilyName(L"Segoe UI Variable Text", &idx, &exists)) && exists) {
            ok = true;
        }
        col->Release();
    }
    return ok ? L"Segoe UI Variable Text" : L"Segoe UI";
}

static IDWriteTextFormat* MakeFormat(const wchar_t* family, float size, DWRITE_FONT_WEIGHT weight,
                                     DWRITE_TEXT_ALIGNMENT align = DWRITE_TEXT_ALIGNMENT_LEADING) {
    IDWriteTextFormat* f = nullptr;
    g_dw->CreateTextFormat(family, nullptr, weight, DWRITE_FONT_STYLE_NORMAL,
                           DWRITE_FONT_STRETCH_NORMAL, size, L"en-us", &f);
    if (f) {
        f->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
        f->SetTextAlignment(align);
        f->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    }
    return f;
}

static void ReleaseFormats() {
    IDWriteTextFormat** fmts[] = {&g_fmtInput, &g_fmtTitle,      &g_fmtHint,     &g_fmtKey,     &g_fmtEmpty,
                                  &g_fmtSection, &g_fmtPanelTitle, &g_fmtPanelSub, &g_fmtPanelRow};
    for (auto** f : fmts) SafeRelease(*f);
    SafeRelease(g_ellipsis);
    SafeRelease(g_ellipsisSm);
}

static void RebuildFormats() {
    ReleaseFormats();
    if (!g_dw) {
        return;
    }
    const wchar_t* fam = PickFontFamily();
    const DWRITE_FONT_WEIGHT normal = DWRITE_FONT_WEIGHT_NORMAL;
    const DWRITE_FONT_WEIGHT semi = DWRITE_FONT_WEIGHT_SEMI_BOLD;

    g_fmtInput = MakeFormat(fam, Sc(22.f), normal);
    g_fmtTitle = MakeFormat(fam, Sc(15.f), normal);
    g_fmtHint = MakeFormat(fam, Sc(12.5f), normal, DWRITE_TEXT_ALIGNMENT_TRAILING);
    g_fmtKey = MakeFormat(fam, Sc(13.f), normal, DWRITE_TEXT_ALIGNMENT_CENTER);
    g_fmtEmpty = MakeFormat(fam, Sc(14.f), normal, DWRITE_TEXT_ALIGNMENT_CENTER);
    g_fmtSection = MakeFormat(fam, Sc(11.5f), semi);
    g_fmtPanelTitle = MakeFormat(fam, Sc(16.f), semi);
    g_fmtPanelSub = MakeFormat(fam, Sc(12.f), normal);
    g_fmtPanelRow = MakeFormat(fam, Sc(13.f), normal);

    // Long app names and paths are trimmed with an ellipsis instead of overflowing.
    const DWRITE_TRIMMING tr = {DWRITE_TRIMMING_GRANULARITY_CHARACTER, 0, 0};
    if (g_fmtTitle) {
        g_dw->CreateEllipsisTrimmingSign(g_fmtTitle, &g_ellipsis);
        if (g_ellipsis) g_fmtTitle->SetTrimming(&tr, g_ellipsis);
    }
    if (g_fmtPanelRow) {
        g_dw->CreateEllipsisTrimmingSign(g_fmtPanelRow, &g_ellipsisSm);
        if (g_ellipsisSm) {
            for (IDWriteTextFormat* f : {g_fmtPanelRow, g_fmtPanelTitle, g_fmtPanelSub}) {
                if (f) f->SetTrimming(&tr, g_ellipsisSm);
            }
        }
    }
}

// ===== Software acrylic =====
// The system acrylic (DWMWA_SYSTEMBACKDROP_TYPE) fills the whole window rectangle, which cannot
// be combined with a layered window whose rounded card and shadow are drawn per pixel by
// Direct2D. Instead the desktop is captured before showing, downscaled, blurred and tinted as
// the card's fill brush, with grain on top.

// Box sizes (odd) whose 3-pass composition approximates a Gaussian of standard deviation sigma.
static void BoxesForGauss(double sigma, int n, int* sizes) {
    double wIdeal = std::sqrt(12.0 * sigma * sigma / n + 1.0);
    int wl = (int)std::floor(wIdeal);
    if (wl % 2 == 0) wl--;
    if (wl < 1) wl = 1;
    int wu = wl + 2;
    double mIdeal = (12.0 * sigma * sigma - n * wl * wl - 4.0 * n * wl - 3.0 * n) / (-4.0 * wl - 4.0);
    int m = (int)std::lround(mIdeal);
    for (int i = 0; i < n; ++i) sizes[i] = (i < m) ? wl : wu;
}

// One box blur pass (radius r) over 8-bit BGRA, with replicated edges (so the edges do
// not darken). horiz=true scans rows; false scans columns.
static void BoxBlurPass(const uint8_t* src, uint8_t* dst, int w, int h, int r, bool horiz) {
    const int len = horiz ? w : h;
    const int lines = horiz ? h : w;
    const size_t stepPx = horiz ? 4 : (size_t)w * 4;
    const size_t stepLine = horiz ? (size_t)w * 4 : 4;
    const int div = 2 * r + 1;
    for (int l = 0; l < lines; ++l) {
        const uint8_t* s = src + (size_t)l * stepLine;
        uint8_t* d = dst + (size_t)l * stepLine;
        for (int c = 0; c < 3; ++c) {
            int sum = s[c] * r;  // positions -r..-1 (replicate the first pixel)
            for (int k = 0; k <= r; ++k) sum += s[(size_t)std::min(k, len - 1) * stepPx + c];
            for (int i = 0; i < len; ++i) {
                d[(size_t)i * stepPx + c] = (uint8_t)((sum + div / 2) / div);
                int add = std::min(i + r + 1, len - 1);
                int sub = std::max(i - r, 0);
                sum += s[(size_t)add * stepPx + c] - s[(size_t)sub * stepPx + c];
            }
        }
        for (int i = 0; i < len; ++i) d[(size_t)i * stepPx + 3] = 255;
    }
}

static void GaussianBlurBGRA(std::vector<uint32_t>& px, int w, int h, double sigma) {
    if (w <= 0 || h <= 0 || sigma < 0.45) return;
    int sizes[3];
    BoxesForGauss(sigma, 3, sizes);
    std::vector<uint32_t> tmp(px.size());
    for (int i = 0; i < 3; ++i) {
        int r = (sizes[i] - 1) / 2;
        if (r < 1) continue;
        BoxBlurPass((const uint8_t*)px.data(), (uint8_t*)tmp.data(), w, h, r, true);
        BoxBlurPass((const uint8_t*)tmp.data(), (uint8_t*)px.data(), w, h, r, false);
    }
}

// BGRA capture (w x h, alpha ignored) -> image reduced `factor` times, blurred and with
// saturation increased (satPct 100 = no change; the Windows acrylic uses 125). Opaque.
static void BuildBackdrop(const uint32_t* src, int w, int h, int factor, double sigmaPx, int satPct,
                          std::vector<uint32_t>& out, int& ow, int& oh) {
    factor = std::max(1, factor);
    ow = (w + factor - 1) / factor;
    oh = (h + factor - 1) / factor;
    out.assign((size_t)ow * oh, 0xFF000000u);
    for (int y = 0; y < oh; ++y) {
        for (int x = 0; x < ow; ++x) {
            unsigned b = 0, g = 0, r = 0, n = 0;
            const int y1 = std::min(h, (y + 1) * factor);
            const int x1 = std::min(w, (x + 1) * factor);
            for (int yy = y * factor; yy < y1; ++yy) {
                const uint32_t* row = src + (size_t)yy * w;
                for (int xx = x * factor; xx < x1; ++xx) {
                    uint32_t p = row[xx];
                    b += p & 0xFF;
                    g += (p >> 8) & 0xFF;
                    r += (p >> 16) & 0xFF;
                    ++n;
                }
            }
            if (!n) n = 1;
            out[(size_t)y * ow + x] =
                0xFF000000u | ((r / n) << 16) | ((g / n) << 8) | (b / n);
        }
    }
    GaussianBlurBGRA(out, ow, oh, sigmaPx / factor);
    if (satPct != 100) {
        const int sat = satPct * 256 / 100;
        for (auto& p : out) {
            int b = p & 0xFF, g = (p >> 8) & 0xFF, r = (p >> 16) & 0xFF;
            int lum = (54 * r + 183 * g + 19 * b) >> 8;
            r = std::clamp(lum + (((r - lum) * sat) >> 8), 0, 255);
            g = std::clamp(lum + (((g - lum) * sat) >> 8), 0, 255);
            b = std::clamp(lum + (((b - lum) * sat) >> 8), 0, 255);
            p = 0xFF000000u | ((uint32_t)r << 16) | ((uint32_t)g << 8) | (uint32_t)b;
        }
    }
}

static void WaitForCompositor() {
    typedef HRESULT(WINAPI * DwmFlush_t)();
    HMODULE m = LoadLibraryExW(L"dwmapi.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (m) {
        auto fn = (DwmFlush_t)GetProcAddress(m, "DwmFlush");
        if (fn) fn();
        FreeLibrary(m);
    }
}

static void ReleaseBackdropResources() {
    SafeRelease(g_backBrush);
    SafeRelease(g_backBmp);
}

static void ReleaseBackdrop() {
    ReleaseBackdropResources();
    std::vector<uint32_t>().swap(g_backPx);
    g_backW = g_backH = 0;
}

// Recipe for each effect: blur (DIP), saturation (%), tint and grain multiplier,
// white veil, Aurora gradient and border sheen strength.
struct EffectParams {
    double sigmaDip;
    int satPct;
    float tintMul;
    float noiseMul;
    float veil;
    bool glow;
    float rim;
};

static EffectParams GetEffectParams() {
    switch (g_cfg.effect) {
        case BgEffect::Mica:    return {70.0, 95, 1.35f, 0.2f, 0.00f, false, 1.0f};
        case BgEffect::Glass:   return {9.0, 150, 0.45f, 0.0f, 0.00f, false, 2.2f};
        case BgEffect::Frosted: return {42.0, 100, 0.90f, 0.7f, 0.10f, false, 1.0f};
        case BgEffect::Aurora:  return {30.0, 135, 0.85f, 0.32f, 0.00f, true, 1.2f};
        default:                return {30.0, 125, 1.00f, 0.4f, 0.00f, false, 1.0f};
    }
}

static double BlurFactor() {
    return 0.4 + g_cfg.blurPct * 0.012;  // 0 -> 0.4, 50 -> 1.0, 100 -> 1.6
}

// Base tint opacity: Opacity 70 -> 0.15 (a lot of background shows), 90 -> ~0.56, 100 -> 1.
static float AcrylicTintAlpha() {
    const float t = std::clamp((g_cfg.opacityPct - 70) / 30.f, 0.f, 1.f);
    return 0.15f + 0.85f * std::pow(t, 1.8f);
}

static float EffectTintAlpha() {
    return std::clamp(AcrylicTintAlpha() * GetEffectParams().tintMul, 0.f, 1.f);
}

// ---- Undocumented API (SetWindowCompositionAttribute + WCA_ACCENT_POLICY) ----
enum LvAccentState {
    LV_ACCENT_ACRYLICBLURBEHIND = 4,
};

struct LvAccentPolicy {
    int AccentState;
    int AccentFlags;
    DWORD GradientColor;  // ABGR: alpha = tint opacity
    DWORD AnimationId;
};

struct LvWinCompAttrData {
    int Attrib;  // 19 = WCA_ACCENT_POLICY
    PVOID pvData;
    SIZE_T cbData;
};

typedef BOOL(WINAPI* LvSetWindowCompositionAttribute_t)(HWND, LvWinCompAttrData*);

static HWND g_liveWnd = nullptr;                    // acrylic window (below g_hwnd)
static bool g_liveSupported = false;                // API present and Windows >= 1803
static LvSetWindowCompositionAttribute_t g_pSetWCA = nullptr;
static HWND g_livePanel = nullptr;                  // acrylic of the details panel
static bool g_liveWin11 = false;                    // Windows 11: corners rounded by DWM
typedef HRESULT(WINAPI* LvDwmSetWindowAttribute_t)(HWND, DWORD, LPCVOID, DWORD);
static LvDwmSetWindowAttribute_t g_pDwmSetAttr = nullptr;
static int g_liveRgnSize[2][2] = {};                // Windows 10: size of the last region

static bool UseLive() {
    if (!g_liveSupported || !g_cfg.liveAcrylic) return false;
    return g_cfg.effect == BgEffect::Acrylic || g_cfg.effect == BgEffect::Glass ||
           g_cfg.effect == BgEffect::Frosted || g_cfg.effect == BgEffect::Aurora;
}

static bool WindowsBuildAtLeast(DWORD build) {
    typedef LONG(WINAPI * RtlGetVersion_t)(OSVERSIONINFOW*);
    HMODULE nt = GetModuleHandleW(L"ntdll.dll");
    auto fn = nt ? (RtlGetVersion_t)GetProcAddress(nt, "RtlGetVersion") : nullptr;
    if (!fn) return false;
    OSVERSIONINFOW vi = {};
    vi.dwOSVersionInfoSize = sizeof(vi);
    return fn(&vi) == 0 && vi.dwBuildNumber >= build;
}

static LRESULT CALLBACK LiveWndProc(HWND h, UINT m, WPARAM w, LPARAM l) {
    switch (m) {
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;
        case WM_NCHITTEST:
            return HTTRANSPARENT;  // the top window (g_hwnd) receives the input
    }
    return DefWindowProcW(h, m, w, l);
}

static HWND LiveCreateOne(HINSTANCE hInst) {
    // No WS_EX_LAYERED: DWM ignores regions and acrylic on layered windows.
    HWND h = CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_NOACTIVATE,
                             L"SpotlightLauncherBackdrop", L"", WS_POPUP, 0, 0, 100, 100, nullptr,
                             nullptr, hInst, nullptr);
    if (h && g_pDwmSetAttr && g_liveWin11) {
        const DWORD pref = 2;  // DWMWCP_ROUND
        g_pDwmSetAttr(h, 33 /*DWMWA_WINDOW_CORNER_PREFERENCE*/, &pref, sizeof(pref));
        const DWORD none = 0xFFFFFFFE;  // DWMWA_COLOR_NONE: no system border
        g_pDwmSetAttr(h, 34 /*DWMWA_BORDER_COLOR*/, &none, sizeof(none));
    }
    return h;
}

static bool LiveCreate(HINSTANCE hInst) {
    HMODULE u = GetModuleHandleW(L"user32.dll");
    g_pSetWCA = u ? (LvSetWindowCompositionAttribute_t)GetProcAddress(u, "SetWindowCompositionAttribute")
                  : nullptr;
    g_liveSupported = g_pSetWCA && WindowsBuildAtLeast(17134);  // acrylic: 1803+
    if (!g_liveSupported) return false;

    g_liveWin11 = WindowsBuildAtLeast(22000);
    HMODULE dm = LoadLibraryExW(L"dwmapi.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    g_pDwmSetAttr = dm ? (LvDwmSetWindowAttribute_t)GetProcAddress(dm, "DwmSetWindowAttribute") : nullptr;

    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = LiveWndProc;
    wc.hInstance = hInst;
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);  // black = transparent for DWM
    wc.lpszClassName = L"SpotlightLauncherBackdrop";
    RegisterClassExW(&wc);

    g_liveWnd = LiveCreateOne(hInst);
    g_livePanel = LiveCreateOne(hInst);
    if (!g_liveWnd) {
        g_liveSupported = false;
        return false;
    }
    return true;
}

static void LiveDestroy() {
    if (g_livePanel) {
        DestroyWindow(g_livePanel);
        g_livePanel = nullptr;
    }
    if (g_liveWnd) {
        DestroyWindow(g_liveWnd);
        g_liveWnd = nullptr;
    }
}

// Tint + real-time acrylic.
static void LiveApplyAccent() {
    if (!g_pSetWCA) return;
    auto to8 = [](float v) { return (DWORD)std::clamp((int)std::lround(v * 255.f), 0, 255); };
    DWORD a = to8(EffectTintAlpha());
    if (a == 0) a = 1;  // acrylic does not support zero opacity
    LvAccentPolicy p = {};
    p.AccentState = LV_ACCENT_ACRYLICBLURBEHIND;
    p.AccentFlags = 0;
    p.GradientColor = (a << 24) | (to8(g_pal.bg.b) << 16) | (to8(g_pal.bg.g) << 8) | to8(g_pal.bg.r);
    LvWinCompAttrData d = {19 /*WCA_ACCENT_POLICY*/, &p, sizeof(p)};
    if (g_liveWnd) g_pSetWCA(g_liveWnd, &d);
    if (g_livePanel) g_pSetWCA(g_livePanel, &d);
}

static void LiveHide() {
    if (g_liveWnd && IsWindowVisible(g_liveWnd)) ShowWindow(g_liveWnd, SW_HIDE);
    if (g_livePanel && IsWindowVisible(g_livePanel)) ShowWindow(g_livePanel, SW_HIDE);
    memset(g_liveRgnSize, 0, sizeof(g_liveRgnSize));
}

// Grain: kept separate from EnsureBackdropBrushes.
static void EnsureNoiseBrush() {
    if (g_noiseBrush || !g_rt) return;
    const int N = 128;
    std::vector<uint32_t> px((size_t)N * N);
    uint32_t s = 0x9E3779B9u;
    for (auto& p : px) {
        s ^= s << 13;
        s ^= s >> 17;
        s ^= s << 5;
        const uint32_t a = 22;  // base grain ~8.6%; each effect scales it with noiseMul
        const uint32_t c = (s & 0xFF) * a / 255;
        p = (a << 24) | (c << 16) | (c << 8) | c;
    }
    D2D1_BITMAP_PROPERTIES bp = D2D1::BitmapProperties(
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED), 96.f, 96.f);
    if (SUCCEEDED(g_rt->CreateBitmap(D2D1::SizeU(N, N), px.data(), N * 4, &bp, &g_noiseBmp)) && g_noiseBmp) {
        D2D1_BITMAP_BRUSH_PROPERTIES nbp = D2D1::BitmapBrushProperties(
            D2D1_EXTEND_MODE_WRAP, D2D1_EXTEND_MODE_WRAP, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR);
        g_rt->CreateBitmapBrush(g_noiseBmp, &nbp, nullptr, &g_noiseBrush);
    }
    if (!g_noiseBrush) SafeRelease(g_noiseBmp);
}



// Captures the desktop at (x, y, w, h) - screen pixels - and leaves it blurred in g_backPx.
// The window must be HIDDEN, or it would show up in the capture itself.
static bool CaptureBackdrop(int x, int y, int w, int h) {
    ReleaseBackdrop();
    if (w <= 0 || h <= 0) return false;
    HDC scr = GetDC(nullptr);
    if (!scr) {
        return false;
    }
    HDC mem = CreateCompatibleDC(scr);
    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = w;
    bi.bmiHeader.biHeight = -h;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    HBITMAP dib = mem ? CreateDIBSection(scr, &bi, DIB_RGB_COLORS, &bits, nullptr, 0) : nullptr;
    bool ok = false;
    if (mem && dib && bits) {
        HGDIOBJ old = SelectObject(mem, dib);
        // CAPTUREBLT also includes layered windows from other applications.
        ok = BitBlt(mem, 0, 0, w, h, scr, x, y, SRCCOPY | CAPTUREBLT) != 0;
        SelectObject(mem, old);
        if (ok) {
            GdiFlush();
            const EffectParams ep = GetEffectParams();
            const double sigmaPx = ep.sigmaDip * BlurFactor() * g_scale;
            const int factor = std::clamp((int)(sigmaPx / 3.0), 1, 4);
            BuildBackdrop((const uint32_t*)bits, w, h, factor, sigmaPx, ep.satPct, g_backPx, g_backW, g_backH);
        }
    }
    if (dib) DeleteObject(dib);
    if (mem) DeleteDC(mem);
    ReleaseDC(nullptr, scr);
    if (!ok) {
        ReleaseBackdrop();
        return false;
    }
    return true;
}

// Prepares the acrylic background for the next opening.
static void PrepareBackdrop() {
    if (g_cfg.effect == BgEffect::Solid || UseLive()) {
        ReleaseBackdrop();  // live acrylic: DWM blurs in real time, no capture needed
        return;
    }
    // DWM must stop compositing the window before capturing, or it shows up in the capture.
    if (IsWindowVisible(g_hwnd)) {
        ShowWindow(g_hwnd, SW_HIDE);
        WaitForCompositor();
    } else if (NowSec() - g_lastHide < 0.12) {
        WaitForCompositor();
    }
    CaptureBackdrop(g_winX, g_winY, g_bmpW, g_bmpH);
}

// Creates (if needed) the acrylic brushes in the current render target.
static bool EnsureBackdropBrushes() {
    if (!g_rt) return false;
    if (!g_backBrush && !g_backPx.empty() && g_backW > 0 && g_backH > 0 && g_bmpW > 0 && g_bmpH > 0) {
        D2D1_BITMAP_PROPERTIES bp = D2D1::BitmapProperties(
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED), 96.f, 96.f);
        HRESULT hr = g_rt->CreateBitmap(D2D1::SizeU((UINT32)g_backW, (UINT32)g_backH), g_backPx.data(),
                                        (UINT32)g_backW * 4, &bp, &g_backBmp);
        if (SUCCEEDED(hr) && g_backBmp) {
            D2D1_BITMAP_BRUSH_PROPERTIES bbp = D2D1::BitmapBrushProperties(
                D2D1_EXTEND_MODE_CLAMP, D2D1_EXTEND_MODE_CLAMP, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
            g_rt->CreateBitmapBrush(g_backBmp, &bbp, nullptr, &g_backBrush);
            if (g_backBrush) {
                // The downscaled image is stretched to cover the whole surface (screen px).
                D2D1_MATRIX_3X2_F m = D2D1::Matrix3x2F::Scale((float)g_bmpW / (float)g_backW,
                                                               (float)g_bmpH / (float)g_backH);
                g_backBrush->SetTransform(&m);
            }
        }
        if (!g_backBrush) SafeRelease(g_backBmp);
    }
    EnsureNoiseBrush();
    return g_backBrush != nullptr;
}

// Releases everything that depends on the render target (in case it has to be recreated)
static void ReleaseTarget() {
    ReleaseResultLayouts();
    for (auto& a : g_apps) {
        SafeRelease(a.bmp);
    }
    SafeRelease(g_brush);
    SafeRelease(g_matchBrush);
    SafeRelease(g_gradBrush);
    SafeRelease(g_glow1);
    SafeRelease(g_glow2);
    ReleaseBackdropResources();  // the pixels (g_backPx) are kept: the bitmap is recreated on draw
    SafeRelease(g_noiseBrush);
    SafeRelease(g_noiseBmp);
    SafeRelease(g_rt);
}

static bool EnsureTarget() {
    if (g_rt) {
        return true;
    }
    if (!g_d2d) {
        return false;
    }
    D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
        D2D1_RENDER_TARGET_TYPE_DEFAULT,
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED), 96.f, 96.f,
        D2D1_RENDER_TARGET_USAGE_NONE, D2D1_FEATURE_LEVEL_DEFAULT);
    HRESULT hrRt = g_d2d->CreateDCRenderTarget(&props, &g_rt);
    if (FAILED(hrRt) || !g_rt) {
        return false;
    }
    g_rt->SetDpi(96.f, 96.f);
    g_rt->CreateSolidColorBrush(D2D1::ColorF(0, 0, 0, 1), &g_brush);
    g_rt->CreateSolidColorBrush(g_pal.match, &g_matchBrush);
    return g_brush && g_matchBrush;
}

// Height (DIP) of the expanded search bar; fixed, and equal to the details panel.
static double ExpandedH() {
    return kExpandedMinH;
}

// Height (DIP) of the details panel; always matches the search bar.
static double DetailsPanelH() {
    return kExpandedMinH;
}

// Animated height (DIP) of the whole visible block.
static double BlockHeight() {
    return g_curH;
}

static bool EnsureSurface(double scale) {
    // The surface includes the details panel slot on the right (transparent while it is
    // closed), and is at least as tall as the panel.
    int newW = (int)std::ceil((kWidth + kPanelGap + kPanelW + 2 * kMargin) * scale);
    // The surface must be tall enough for the bar / details panel.
    const double maxH = std::max<double>(ExpandedH(), kExpandedMinH);
    int newH = (int)std::ceil((maxH + 2 * kMargin) * scale);
    bool scaleChanged = std::fabs(scale - g_scale) > 0.001;
    if (g_dib && !scaleChanged && newW == g_bmpW && newH == g_bmpH) {
        return true;
    }

    if (scaleChanged) {
        ReleaseResultLayouts();
    }
    g_scale = scale;
    g_bmpW = newW;
    g_bmpH = newH;

    if (!g_memDC) {
        g_memDC = CreateCompatibleDC(nullptr);
    }
    DestroySurface();

    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = g_bmpW;
    bi.bmiHeader.biHeight = -g_bmpH;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    g_dib = CreateDIBSection(nullptr, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!g_dib) {
        return false;
    }
    g_oldBmp = SelectObject(g_memDC, g_dib);

    RebuildFormats();
    return true;
}

// ===== Layout / state animation =====

// Number of rows that fit in the visible rows area (the list scrolls for the rest).
static int VisibleRows() {
    const int n = (int)g_results.size();
    if (n == 0) return 0;
    // Available height for rows: expanded card minus the bar, the footer, the calculator card
    // and both paddings.
    const double rowsArea = ExpandedH() - kBarH - kFooterH - 2 * kListPad - HeroH();
    int cap = (int)std::floor(rowsArea / kRowH);
    if (cap < 1) cap = 1;
    return std::min(n, cap);
}

// Height (DIP) of the rows area, including the section headers if any.
static double ListRowsHeight() {
    return VisibleRows() * kRowH + g_headerCount * kSectionH;
}

// Height (DIP) of the visible rows area: below the bar, above the footer, minus the padding.
static double ListViewportH() {
    return ExpandedH() - kBarH - kFooterH - 2 * kListPad - HeroH();
}

// Inverse of RowOffset(): given a vertical position `v` (DIP, in the same coordinates as
// g_rowOff) returns the fractional row position whose RowOffset equals `v`. It lets the
// keyboard scroll be computed in exact pixels and then be expressed in the row units used by
// the scroll animation (g_scrollPos / g_scrollTarget).
static double PosFromContentY(double v) {
    const int n = (int)g_results.size();
    if (n == 0 || g_rowOff.size() != (size_t)n + 1) return 0.0;
    if (v <= g_rowOff[0]) return 0.0;
    int i = (int)(std::upper_bound(g_rowOff.begin(), g_rowOff.begin() + n, (float)v) -
                  g_rowOff.begin()) - 1;
    i = std::clamp(i, 0, n - 1);
    const double a = g_rowOff[i];
    const double b = (i + 1 < n) ? g_rowOff[i + 1] : g_rowOff[i] + kRowH;
    const double f = (b > a) ? std::clamp((v - a) / (b - a), 0.0, 1.0) : 0.0;
    return i + f;
}

// Maximum scroll (DIP) that still shows the last row completely above the footer.
static double MaxScrollPx() {
    const int n = (int)g_results.size();
    if (n == 0 || g_rowOff.size() != (size_t)n + 1) return 0.0;
    return std::max(0.0, (double)g_rowOff[n] - ListViewportH());
}

// `followSelection` is true only for keyboard selection changes (arrows, Tab, PageUp/Down);
// the mouse wheel and hover leave the scroll untouched.
static void UpdateTargets(bool snap, bool followSelection = true) {
    int rows = VisibleRows();
    // There are only two heights: bar alone, or expanded (fixed). While typing it is always
    // expanded, even without results ("No results" fills the list area).
    double h = kBarH;
    if (rows > 0 || !g_tokens.empty()) {
        h = ExpandedH();
    } else if (g_apps.empty() && !g_indexedOnce) {
        h += kEmptyRowH;
    }
    g_targetH = h;

    int n = (int)g_results.size();
    if (g_sel >= n) g_sel = std::max(0, n - 1);
    double maxScroll = std::max(0, n - rows);
    if (n > 0) {
        // Section headers consume space, so the last row may need a larger scroll than n - rows.
        maxScroll = std::max(maxScroll, PosFromContentY(MaxScrollPx() + g_rowOff[0]));
    }

    if (followSelection && rows > 0 && n > 0 && g_rowOff.size() == (size_t)n + 1) {
        // Scroll in exact pixels (rows have different heights due to headers): move just enough
        // to bring the selected row fully above the footer, without moving an already visible row.
        const double vp = ListViewportH();
        const double cur = RowOffset(g_scrollTarget) - g_rowOff[0];
        const int sec = g_results[g_sel].section;
        const int prevSec = g_sel > 0 ? g_results[g_sel - 1].section : 0;
        const bool hasHeader = sec != 0 && sec != prevSec;
        const double selTop = g_rowOff[g_sel] - (hasHeader ? kSectionH : 0.f);
        const double selBottom = g_rowOff[g_sel] + kRowH;
        double want = cur;
        if (selTop < cur) {
            want = selTop;            // above the view: align to the top (with its header)
        } else if (selBottom > cur + vp) {
            want = selBottom - vp;    // below the view: align bottom, exactly above the footer
        }
        want = std::clamp(want, 0.0, MaxScrollPx());
        if (std::fabs(want - cur) > 0.01) {
            g_scrollTarget = PosFromContentY(want + g_rowOff[0]);
            g_kbScroll = true;
        }
    }

    g_scrollTarget = std::clamp(g_scrollTarget, 0.0, maxScroll);
    g_selTarget = g_sel;

    // Details panel: there is only a panel if it is enabled and there is a selected app.
    if (n > 0 && g_sel >= 0 && g_sel < n) {
        g_panelAppId = g_apps[g_results[g_sel].app].id;
    }
    g_detailTarget = (g_detailsOpen && n > 0) ? 1.0 : 0.0;

    if (snap) {
        g_curH = g_targetH;
        g_selPos = g_selTarget;
        g_scrollPos = g_scrollTarget;
        g_detailPos = g_detailTarget;
    }
}

static void StartFrameTimer() {
    if (!g_frameTimerOn && g_hwnd) {
        SetTimer(g_hwnd, TIMER_FRAME, 15, nullptr);
        g_frameTimerOn = true;
        g_lastFrame = NowSec();
    }
}

static void MarkDirty() {
    g_dirty = true;
    if (g_vis != Vis::Hidden) {
        StartFrameTimer();
    }
}

static void ResetBlink() {
    g_caretOn = true;
    if (g_hwnd && g_vis != Vis::Hidden) {
        SetTimer(g_hwnd, TIMER_BLINK, 530, nullptr);
    }
}

static void SetSelection(int idx) {
    int n = (int)g_results.size();
    if (n == 0) return;
    g_sel = std::clamp(idx, 0, n - 1);
    UpdateTargets(false, /*followSelection=*/true);
    MarkDirty();
}

static void SelectDelta(int d) {
    int n = (int)g_results.size();
    if (n == 0) return;
    int idx;
    if (d == 1 || d == -1) {
        idx = (g_sel + d + n) % n;  // wraps around
    } else {
        idx = std::clamp(g_sel + d, 0, n - 1);
    }
    SetSelection(idx);
}

static void OnTextChanged() {
    // The calculator is debounced: clear its state now so typing feels instant (app searches
    // update immediately) and only re-evaluate shortly after the user stops typing. That way
    // typing "pith" never flashes the value of "pi" on the way.
    g_calc = Calc::Result();
    g_calcDisplay.clear();
    g_calcActive = false;
    g_calcHistSel = -1;
    g_histScroll = 0;
    g_calcPending = true;
    if (g_hwnd) {
        KillTimer(g_hwnd, TIMER_CALC);
        SetTimer(g_hwnd, TIMER_CALC, 200, nullptr);
    }
    RunSearch();
    g_sel = 0;
    g_scrollTarget = 0;
    UpdateTargets(false, /*followSelection=*/true);  // layout/selection follow the new results
    g_copiedAt = -1e9;  // typing again clears the calculator "Copied" flash
    if (g_hwnd) KillTimer(g_hwnd, TIMER_COPY);
    ResetBlink();
    MarkDirty();
}

// ===== Drawing =====

static void SetBrush(const D2D1_COLOR_F& c, float alphaMul = 1.f) {
    D2D1_COLOR_F k = c;
    k.a *= alphaMul;
    g_brush->SetColor(&k);
}

static void FillRR(float x0, float y0, float x1, float y1, float radius, const D2D1_COLOR_F& c,
                   float alphaMul = 1.f) {
    SetBrush(c, alphaMul);
    D2D1_ROUNDED_RECT rr = D2D1::RoundedRect(D2D1::RectF(x0, y0, x1, y1), radius, radius);
    g_rt->FillRoundedRectangle(&rr, g_brush);
}

static ID2D1Bitmap* EnsureIcon(AppEntry& a) {
    if (a.bmp) return a.bmp;
    if (a.iconPx.empty() || a.iconW <= 0 || a.iconH <= 0) return nullptr;
    D2D1_BITMAP_PROPERTIES bp = D2D1::BitmapProperties(
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED), 96.f, 96.f);
    g_rt->CreateBitmap(D2D1::SizeU((UINT32)a.iconW, (UINT32)a.iconH), a.iconPx.data(),
                       (UINT32)a.iconW * 4, &bp, &a.bmp);
    return a.bmp;
}

static IDWriteTextLayout* BuildTitleLayout(const AppEntry& a, float widthPx, float heightPx) {
    IDWriteTextLayout* l = nullptr;
    if (FAILED(g_dw->CreateTextLayout(a.name.c_str(), (UINT32)a.name.size(), g_fmtTitle, widthPx,
                                      heightPx, &l)) ||
        !l) {
        return nullptr;
    }
    std::vector<char> mask(a.folded.size(), 0);
    for (const auto& t : g_tokens) {
        ScoreToken(a.name, a.folded, t, &mask);
    }
    size_t i = 0;
    while (i < mask.size()) {
        if (mask[i]) {
            size_t j = i;
            while (j < mask.size() && mask[j]) ++j;
            DWRITE_TEXT_RANGE r = {(UINT32)i, (UINT32)(j - i)};
            l->SetFontWeight(DWRITE_FONT_WEIGHT_SEMI_BOLD, r);
            l->SetDrawingEffect(g_matchBrush, r);
            i = j;
        } else {
            ++i;
        }
    }
    return l;
}

static void DrawMagnifier(float cx, float cy) {
    SetBrush(g_pal.textDim);
    D2D1_ELLIPSE e = D2D1::Ellipse(D2D1::Point2F(cx - Sc(2.f), cy - Sc(2.f)), Sc(7.5f), Sc(7.5f));
    g_rt->DrawEllipse(&e, g_brush, Sc(2.f), g_roundStroke);
    g_rt->DrawLine(D2D1::Point2F(cx + Sc(3.4f), cy + Sc(3.4f)),
                   D2D1::Point2F(cx + Sc(9.f), cy + Sc(9.f)), g_brush, Sc(2.f), g_roundStroke);
}

// ===== Details panel: technical data, available actions and drawing =====
// Top-left corner (px) of the main panel inside the surface. The visible block is centered
// horizontally using the animated g_detailPos; vertically the bar stays at g_anchorTop unless
// the expanded card would not fit in the work area.
static void MainOrigin(float& ox, float& oy) {
    const double a = std::clamp(g_detailPos, 0.0, 1.0);
    const double innerW = kWidth + kPanelGap + kPanelW;
    const double compW = kWidth + a * (kPanelGap + kPanelW);
    ox = (float)((kMargin + (innerW - compW) / 2) * g_scale);

    const double blockPx = BlockHeight() * g_scale;
    const double cardTop =
        std::clamp(g_anchorTop, g_workTop, std::max(g_workTop, g_workBottom - blockPx));
    oy = (float)(cardTop - g_winY);
}

static std::wstring FormatSize(ULONGLONG b) {
    wchar_t buf[48];
    if (b < 1024) {
        swprintf_s(buf, L"%llu B", (unsigned long long)b);
    } else if (b < 1024ULL * 1024) {
        swprintf_s(buf, L"%.1f KB", b / 1024.0);
    } else {
        swprintf_s(buf, L"%.1f MB", b / (1024.0 * 1024.0));
    }
    return buf;
}

// Version and company from the .exe version resource
static void ReadVersionInfo(const std::wstring& path, std::wstring& version, std::wstring& company) {
    DWORD handle = 0;
    DWORD size = GetFileVersionInfoSizeW(path.c_str(), &handle);
    if (size == 0) {
        return;
    }
    std::vector<BYTE> buf(size);
    if (!GetFileVersionInfoW(path.c_str(), 0, size, buf.data())) {
        return;
    }

    VS_FIXEDFILEINFO* ffi = nullptr;
    UINT len = 0;
    if (VerQueryValueW(buf.data(), L"\\", (LPVOID*)&ffi, &len) && ffi && len >= sizeof(VS_FIXEDFILEINFO)) {
        wchar_t v[64];
        swprintf_s(v, L"%u.%u.%u.%u", (unsigned)HIWORD(ffi->dwFileVersionMS),
                   (unsigned)LOWORD(ffi->dwFileVersionMS), (unsigned)HIWORD(ffi->dwFileVersionLS),
                   (unsigned)LOWORD(ffi->dwFileVersionLS));
        version = v;
    }

    struct LangCp {
        WORD lang;
        WORD cp;
    };
    LangCp* tr = nullptr;
    UINT trLen = 0;
    if (VerQueryValueW(buf.data(), L"\\VarFileInfo\\Translation", (LPVOID*)&tr, &trLen) && tr &&
        trLen >= sizeof(LangCp)) {
        wchar_t sub[96];
        swprintf_s(sub, L"\\StringFileInfo\\%04x%04x\\CompanyName", (unsigned)tr[0].lang,
                   (unsigned)tr[0].cp);
        LPVOID val = nullptr;
        UINT vl = 0;
        if (VerQueryValueW(buf.data(), sub, &val, &vl) && val && vl > 1) {
            company = (const wchar_t*)val;
        }
    }
}

static AppDetails ComputeDetails(const AppEntry& a) {
    AppDetails d;
    d.packaged = IsPackagedId(a.id);

    std::wstring fullName;
    std::wstring installPath;

    IShellItem2* item = nullptr;
    std::wstring parse = L"shell:AppsFolder\\" + a.id;
    if (SUCCEEDED(SHCreateItemFromParsingName(parse.c_str(), nullptr, IID_PPV_ARGS(&item))) && item) {
        d.target = ItemString(item, L"System.Link.TargetParsingPath");
        d.args = ItemString(item, L"System.Link.Arguments");
        if (d.packaged) {
            fullName = ItemString(item, L"System.AppUserModel.PackageFullName");
            installPath = ItemString(item, L"System.AppUserModel.PackageInstallPath");
        }
        item->Release();
    }
    if (d.packaged) {
        d.target.clear();  // MSIX apps are not launched or elevated by path
    } else if (d.target.empty()) {
        d.target = PathFromAppId(a.id);
    }
    // Shell namespace targets (e.g. File Explorer -> "::{...}This PC") are not files: hide the
    // path and the file-based actions instead of showing the raw GUID.
    if (d.target.size() >= 2 && d.target[0] == L':' && d.target[1] == L':') {
        d.target.clear();
    }

    std::wstring version, company, arch, sizeText, modified;

    if (!d.target.empty()) {
        // This 32-bit process has System32 redirected to SysWOW64: a 64-bit-only tool would
        // look missing and a dual-arch one would report the 32-bit copy's size and version.
        const std::wstring real = NativeSystemPath(d.target);
        WIN32_FILE_ATTRIBUTE_DATA fad = {};
        if (GetFileAttributesExW(real.c_str(), GetFileExInfoStandard, &fad) &&
            !(fad.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            d.canFile = true;
            d.isExe = EndsWith(FoldString(d.target), L".exe");
            // Elevation and the Properties dialog are handled by this 32-bit process, so they
            // see the path with WOW64 redirection. Offer them only when that path exists, or a
            // 64-bit-only tool (mdsched.exe, ...) would fail with "file not found".
            WIN32_FILE_ATTRIBUTE_DATA sad = {};
            d.canShell = GetFileAttributesExW(d.target.c_str(), GetFileExInfoStandard, &sad) &&
                         !(sad.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
            ReadVersionInfo(real, version, company);
            sizeText = FormatSize(((ULONGLONG)fad.nFileSizeHigh << 32) | fad.nFileSizeLow);
            modified = FormatFileTime(fad.ftLastWriteTime);
        } else {
            d.target.clear();  // path does not exist or is not a file
        }
    }

    if (d.packaged && !fullName.empty()) {
        // Name_Version_Architecture_ResourceId_PublisherId
        std::vector<std::wstring> parts;
        size_t start = 0;
        for (;;) {
            size_t p = fullName.find(L'_', start);
            if (p == std::wstring::npos) {
                parts.push_back(fullName.substr(start));
                break;
            }
            parts.push_back(fullName.substr(start, p - start));
            start = p + 1;
        }
        if (parts.size() >= 3) {
            version = parts[1];
            arch = (parts[2] == L"neutral") ? L"Neutral" : parts[2];
        }
    }

    auto add = [&](const wchar_t* label, const std::wstring& value, bool middle) {
        if (value.empty()) return;
        InfoRow r;
        r.label = label;
        r.value = value;
        r.middle = middle;
        d.rows.push_back(std::move(r));
    };
    add(L"Version", version, false);
    add(L"Company", company, false);
    add(L"Architecture", arch, false);
    add(L"Path", d.packaged ? installPath : d.target, true);
    if (d.packaged) add(L"ID", a.id, true);
    add(L"Size", sizeText, false);
    add(L"Modified", modified, false);
    add(L"Uses", std::to_wstring(a.uses), false);
    add(L"Last used", a.lastUsed > 0 ? FormatStamp(a.lastUsed) : (a.uses > 0 ? L"—" : L"Never"), false);
    return d;
}

static const AppDetails& GetDetails(const AppEntry& a) {
    auto it = g_detailsCache.find(a.id);
    if (it != g_detailsCache.end()) {
        return it->second;
    }
    return g_detailsCache.emplace(a.id, ComputeDetails(a)).first->second;
}

static bool ActionAvailable(int id, const AppEntry& a, const AppDetails& d) {
    switch (id) {
        case ACT_OPEN:
            return true;
        case ACT_ADMIN:
            return d.canShell && d.isExe;
        case ACT_LOCATION:
        case ACT_COPYPATH:
            return d.canFile;
        case ACT_PROPS:
            return d.canShell;
        case ACT_FORGET:
            return a.uses > 0;
    }
    return false;
}

static const ActionDef* FindAction(int id) {
    for (const ActionDef& ad : kActions) {
        if (ad.id == id) return &ad;
    }
    return nullptr;
}

// Actions available for an app, in the order they are shown. Returns how many.
static int BuildActions(const AppEntry& a, const AppDetails& d, int out[8]) {
    int n = 0;
    for (const ActionDef& ad : kActions) {
        if (n < 8 && ActionAvailable(ad.id, a, d)) out[n++] = ad.id;
    }
    return n;
}

// Shortens a text in the middle ("C:\Prog…\app.exe") so it fits in `widthPx`.
static std::wstring FitMiddle(const std::wstring& t, IDWriteTextFormat* fmt, float widthPx) {
    auto textW = [&](const std::wstring& str) -> float {
        IDWriteTextLayout* l = nullptr;
        if (FAILED(g_dw->CreateTextLayout(str.c_str(), (UINT32)str.size(), fmt, 10000.f, 100.f, &l)) ||
            !l) {
            return 0.f;
        }
        DWRITE_TEXT_METRICS m = {};
        l->GetMetrics(&m);
        l->Release();
        return m.widthIncludingTrailingWhitespace;
    };
    if (t.size() < 8 || textW(t) <= widthPx) {
        return t;
    }
    auto make = [&](size_t keep) {
        size_t head = keep / 3;  // more is kept at the end (the file name)
        size_t tail = keep - head;
        return t.substr(0, head) + L"…" + t.substr(t.size() - tail);
    };
    size_t lo = 2, hi = t.size() - 1, best = 2;
    while (lo <= hi) {
        size_t mid = (lo + hi) / 2;
        if (textW(make(mid)) <= widthPx) {
            best = mid;
            lo = mid + 1;
        } else {
            hi = mid - 1;
        }
    }
    return make(best);
}

static void DrawLabel(const std::wstring& str, IDWriteTextFormat* fmt, float x0, float y0, float x1,
                      float y1, const D2D1_COLOR_F& c, float alpha) {
    if (!fmt || str.empty()) return;
    SetBrush(c, alpha);
    D2D1_RECT_F r = D2D1::RectF(x0, y0, x1, y1);
    g_rt->DrawText(str.c_str(), (UINT32)str.size(), fmt, &r, g_brush);
}

// ---- Card, keycap and footer finish ----

// Linear gradient that fades from `c` (alpha `alpha`) to fully transparent, in unit coordinates
// from (0,0) to `end`; the caller stretches it over the target rectangle.
static void MakeFadeBrush(const D2D1_COLOR_F& c, float alpha, D2D1_POINT_2F end,
                          ID2D1LinearGradientBrush*& out) {
    if (out || !g_rt) return;
    D2D1_GRADIENT_STOP st[2] = {
        {0.f, D2D1::ColorF(c.r, c.g, c.b, alpha)},
        {1.f, D2D1::ColorF(c.r, c.g, c.b, 0.f)},
    };
    ID2D1GradientStopCollection* coll = nullptr;
    if (FAILED(g_rt->CreateGradientStopCollection(st, 2, &coll)) || !coll) return;
    g_rt->CreateLinearGradientBrush(D2D1::LinearGradientBrushProperties(D2D1::Point2F(0, 0), end), coll,
                                    &out);
    coll->Release();
}

// Subtle sheen at the top of the card (dark themes only).
static void EnsureGradient() {
    if (!g_pal.light) MakeFadeBrush(g_pal.text, 0.095f, D2D1::Point2F(0, 1), g_gradBrush);
}

// Aurora effect: one accent gradient from each corner, fading towards the opposite one.
static void EnsureGlow() {
    MakeFadeBrush(g_pal.accent, g_pal.light ? 0.34f : 0.40f, D2D1::Point2F(1, 1), g_glow1);
    MakeFadeBrush(g_pal.accent2, g_pal.light ? 0.26f : 0.30f, D2D1::Point2F(1, 1), g_glow2);
}

// Each acrylic window is exactly the size of its card (no margins), so it does not depend on
// clipping with SetWindowRgn (DWM ignores it with acrylic). The rounded corners come from DWM on
// Windows 11 (DWMWCP_ROUND) and from a region as a fallback on Windows 10.
static void LivePlaceWindow(HWND wnd, int idx, int x, int y, int w, int h) {
    if (!wnd) return;
    if (w < 4 || h < 4) {
        if (IsWindowVisible(wnd)) ShowWindow(wnd, SW_HIDE);
        return;
    }
    if (!g_liveWin11 && (g_liveRgnSize[idx][0] != w || g_liveRgnSize[idx][1] != h)) {
        g_liveRgnSize[idx][0] = w;
        g_liveRgnSize[idx][1] = h;
        const int d = (int)std::lround(kRadius * (float)g_scale * 2.f);
        HRGN rgn = CreateRoundRectRgn(0, 0, w + 1, h + 1, d, d);
        SetWindowRgn(wnd, rgn, TRUE);  // the system takes ownership of rgn
    }
    UINT flags = SWP_NOACTIVATE;
    if (!IsWindowVisible(wnd)) flags |= SWP_SHOWWINDOW;
    // insertAfter = g_hwnd  ->  ends up right BELOW g_hwnd.
    SetWindowPos(wnd, g_hwnd, x, y, w, h, flags);
}

// Call from Present(): keeps the acrylic window aligned with g_hwnd.
static void LiveSync() {
    if (!g_liveWnd) return;

    // Without WS_EX_LAYERED there is no global fade. The acrylic appears once the card is already
    // ~40% visible and disappears as soon as it starts closing (the top layer does fade fully).
    if (g_alpha < 0.4) {
        if (IsWindowVisible(g_liveWnd)) ShowWindow(g_liveWnd, SW_HIDE);
        if (g_livePanel && IsWindowVisible(g_livePanel)) ShowWindow(g_livePanel, SW_HIDE);
        return;
    }

    const float s = (float)g_scale;
    float cx = 0, cy = 0;
    MainOrigin(cx, cy);
    const int baseX = g_winX;
    const int baseY = g_winY + (int)std::lround(g_offY);

    LivePlaceWindow(g_liveWnd, 0, baseX + (int)std::lround(cx), baseY + (int)std::lround(cy),
                    (int)std::lround(kWidth * s), (int)std::lround(g_curH * s));

    if (g_livePanel) {
        const float a = (float)std::clamp(g_detailPos, 0.0, 1.0);
        if (a > 0.02f) {
            const float ox = cx + (kWidth + kPanelGap + (1.f - a) * 14.f) * s;
            LivePlaceWindow(g_livePanel, 1, baseX + (int)std::lround(ox), baseY + (int)std::lround(cy),
                            (int)std::lround(kPanelW * s * a), (int)std::lround(DetailsPanelH() * s));
        } else if (IsWindowVisible(g_livePanel)) {
            ShowWindow(g_livePanel, SW_HIDE);
        }
    }
}

// Shadow that does NOT paint inside the card (otherwise it would cover the acrylic window below).
static void DrawShadowRing(float x, float y, float w, float h, float alpha) {
    if (!g_d2d || !g_rt) return;
    const float s = (float)g_scale;
    ID2D1RoundedRectangleGeometry* cardG = nullptr;
    if (FAILED(g_d2d->CreateRoundedRectangleGeometry(
            D2D1::RoundedRect(D2D1::RectF(x, y, x + w, y + h), kRadius * s, kRadius * s), &cardG)) ||
        !cardG) {
        return;
    }
    const int kLayers = 6;
    for (int k = kLayers; k >= 1; --k) {
        const float e = k * 2.f * s;
        const float a = g_pal.shadowAlpha * alpha * (1.f - (float)(k - 1) / kLayers);
        ID2D1RoundedRectangleGeometry* outerG = nullptr;
        ID2D1PathGeometry* ring = nullptr;
        ID2D1GeometrySink* sink = nullptr;
        if (SUCCEEDED(g_d2d->CreateRoundedRectangleGeometry(
                D2D1::RoundedRect(D2D1::RectF(x - e, y - e + 4 * s, x + w + e, y + h + e + 4 * s),
                                  kRadius * s + e, kRadius * s + e),
                &outerG)) && outerG &&
            SUCCEEDED(g_d2d->CreatePathGeometry(&ring)) && ring && SUCCEEDED(ring->Open(&sink)) && sink) {
            outerG->CombineWithGeometry(cardG, D2D1_COMBINE_MODE_EXCLUDE, nullptr, sink);
            sink->Close();
            SetBrush(D2D1::ColorF(0, 0, 0, 1), a);
            g_rt->FillGeometry(ring, g_brush);
        }
        SafeRelease(sink);
        SafeRelease(ring);
        SafeRelease(outerG);
    }
    cardG->Release();
}

// Card: shadow, body (live DWM acrylic or captured effect), sheen, border, inner edge.
static void DrawGlow(const D2D1_ROUNDED_RECT& card, float x, float y, float w, float h, float alpha) {
    EnsureGlow();
    if (g_glow1) {
        g_glow1->SetStartPoint(D2D1::Point2F(x, y));
        g_glow1->SetEndPoint(D2D1::Point2F(x + w, y + h));
        g_glow1->SetOpacity(alpha);
        g_rt->FillRoundedRectangle(&card, g_glow1);
    }
    if (g_glow2) {
        g_glow2->SetStartPoint(D2D1::Point2F(x + w, y + h));
        g_glow2->SetEndPoint(D2D1::Point2F(x, y));
        g_glow2->SetOpacity(alpha);
        g_rt->FillRoundedRectangle(&card, g_glow2);
    }
}

static void DrawCard(float x, float y, float w, float h, float alpha) {
    const float s = (float)g_scale;
    const bool live = UseLive();

    if (live) {
        DrawShadowRing(x, y, w, h, alpha);
    } else {
        D2D1_COLOR_F black = D2D1::ColorF(0, 0, 0, 1);
        static const int kShadowLayers = 6;
        for (int k = kShadowLayers; k >= 1; --k) {
            float e = k * 2.0f * s;
            float a = g_pal.shadowAlpha * alpha * (1.0f - (float)(k - 1) / kShadowLayers);
            FillRR(x - e, y - e + 4 * s, x + w + e, y + h + e + 4 * s, kRadius * s + e, black, a);
        }
    }

    const EffectParams ep = GetEffectParams();
    const bool solid = g_cfg.effect == BgEffect::Solid;
    D2D1_ROUNDED_RECT card = D2D1::RoundedRect(D2D1::RectF(x, y, x + w, y + h), kRadius * s, kRadius * s);

    if (live) {
        // DWM provides the background and tint in g_liveWnd. Only a nearly invisible layer goes
        // here (alpha ~3/255) so that this window keeps receiving the mouse over the card.
        FillRR(x, y, x + w, y + h, kRadius * s, D2D1::ColorF(0, 0, 0, 1), 0.012f);
        if (ep.veil > 0.f) {
            FillRR(x, y, x + w, y + h, kRadius * s, D2D1::ColorF(1.f, 1.f, 1.f, 1.f), ep.veil * alpha);
        }
        if (ep.glow) DrawGlow(card, x, y, w, h, alpha);
        if (g_cfg.effect != BgEffect::Acrylic) {  // DWM's acrylic already has its own grain
            EnsureNoiseBrush();
            if (g_noiseBrush && ep.noiseMul > 0.f) {
                g_noiseBrush->SetOpacity(std::min(1.f, alpha * ep.noiseMul));
                g_rt->FillRoundedRectangle(&card, g_noiseBrush);
            }
        }
    } else if (!solid && EnsureBackdropBrushes()) {
        g_backBrush->SetOpacity(alpha);
        g_rt->FillRoundedRectangle(&card, g_backBrush);
        if (ep.veil > 0.f) {
            FillRR(x, y, x + w, y + h, kRadius * s, D2D1::ColorF(1.f, 1.f, 1.f, 1.f), ep.veil * alpha);
        }
        D2D1_COLOR_F tint = g_pal.bg;
        tint.a = EffectTintAlpha();
        FillRR(x, y, x + w, y + h, kRadius * s, tint, alpha);
        if (ep.glow) DrawGlow(card, x, y, w, h, alpha);
        if (g_noiseBrush && ep.noiseMul > 0.f) {
            g_noiseBrush->SetOpacity(std::min(1.f, alpha * ep.noiseMul));
            g_rt->FillRoundedRectangle(&card, g_noiseBrush);
        }
    } else {
        D2D1_COLOR_F c = g_pal.bg;
        if (solid) c.a = 1.f;
        FillRR(x, y, x + w, y + h, kRadius * s, c, alpha);
    }

    EnsureGradient();
    if (g_gradBrush) {
        g_gradBrush->SetStartPoint(D2D1::Point2F(x, y));
        g_gradBrush->SetEndPoint(D2D1::Point2F(x, y + std::max(1.f, h * 0.6f)));
        g_gradBrush->SetOpacity(std::min(1.f, alpha * ep.rim));
        D2D1_ROUNDED_RECT body = D2D1::RoundedRect(D2D1::RectF(x, y, x + w, y + h), kRadius * s, kRadius * s);
        g_rt->FillRoundedRectangle(&body, g_gradBrush);
    }

    // The antialiased border hides the "jagged" edge that the DWM region leaves.
    SetBrush(g_pal.border, alpha);
    D2D1_ROUNDED_RECT outer = D2D1::RoundedRect(D2D1::RectF(x + 0.5f, y + 0.5f, x + w - 0.5f, y + h - 0.5f),
                                                kRadius * s, kRadius * s);
    g_rt->DrawRoundedRectangle(&outer, g_brush, 1.f);

    const bool dark = !g_pal.light;
    SetBrush(D2D1::ColorF(1.f, 1.f, 1.f, 1.f), std::min(1.f, (dark ? 0.06f : 0.55f) * ep.rim) * alpha);
    D2D1_ROUNDED_RECT inner = D2D1::RoundedRect(D2D1::RectF(x + 1.5f, y + 1.5f, x + w - 1.5f, y + h - 1.5f),
                                                kRadius * s - 1.f, kRadius * s - 1.f);
    g_rt->DrawRoundedRectangle(&inner, g_brush, 1.f);
}



static float TextWidth(const wchar_t* txt, IDWriteTextFormat* fmt) {
    if (!fmt || !txt || !*txt) return 0.f;
    IDWriteTextLayout* l = nullptr;
    if (FAILED(g_dw->CreateTextLayout(txt, (UINT32)wcslen(txt), fmt, 10000.f, 100.f, &l)) || !l) {
        return 0.f;
    }
    DWRITE_TEXT_METRICS m = {};
    l->GetMetrics(&m);
    l->Release();
    return m.widthIncludingTrailingWhitespace;
}

static float KeycapWidth(const wchar_t* txt) {
    const float s = (float)g_scale;
    return std::max(24 * s, TextWidth(txt, g_fmtKey) + 14 * s);
}

// Keycap ending at xRight; returns its left x.
static float DrawKeycap(const wchar_t* txt, float xRight, float yMid) {
    const float s = (float)g_scale;
    const float capH = 22 * s;
    const float x0 = xRight - KeycapWidth(txt);
    FillRR(x0, yMid - capH / 2, xRight, yMid + capH / 2, 6 * s, g_pal.keycap);
    DrawLabel(txt, g_fmtKey, x0, yMid - capH / 2, xRight, yMid + capH / 2, g_pal.textDim, 1.f);
    return x0;
}

// Footer hint ("Label [key][key]") ending at xRight; returns its left x.
static float DrawHint(const wchar_t* label, const wchar_t* const* keys, int nKeys, float xRight,
                      float yMid) {
    const float s = (float)g_scale;
    float x = xRight;
    for (int i = nKeys - 1; i >= 0; --i) {
        x = DrawKeycap(keys[i], x, yMid) - 4 * s;
    }
    const float labelRight = x - 4 * s;
    const float lw = TextWidth(label, g_fmtHint);
    DrawLabel(label, g_fmtHint, labelRight - lw - 4 * s, yMid - 12 * s, labelRight, yMid + 12 * s,
              g_pal.textDim, 1.f);
    return labelRight - lw;
}

// Width of a hint as drawn by DrawHint, used to size the clickable footer button.
static float HintWidth(const wchar_t* label, const wchar_t* const* keys, int nKeys) {
    const float s = (float)g_scale;
    float w = 4 * s + TextWidth(label, g_fmtHint);
    for (int i = 0; i < nKeys; ++i) w += KeycapWidth(keys[i]) + 4 * s;
    return w;
}

// Rectangle (surface pixels) of the footer reindex button: refresh icon + label + shortcut.
static D2D1_RECT_F ReindexButtonRect(float cx, float footerTop) {
    const float s = (float)g_scale;
    const float x0 = cx + 12 * s;
    const float w = 32 * s + HintWidth(kTxtReindex, kReindexKeys, 2) + 6 * s;
    return D2D1::RectF(x0, footerTop + 5 * s, x0 + w, footerTop + (kFooterH - 5) * s);
}

// Circular arrow made of short segments. `angle` rotates it (it spins while indexing).
static void DrawRefreshIcon(float cx, float cy, float r, double angle, const D2D1_COLOR_F& color) {
    const float s = (float)g_scale;
    const int kSegments = 16;
    const double span = 1.5 * kPi;  // 270 degrees
    SetBrush(color);
    D2D1_POINT_2F prev = D2D1::Point2F(cx + r * (float)std::cos(angle), cy + r * (float)std::sin(angle));
    for (int i = 1; i <= kSegments; ++i) {
        const double a = angle + span * i / kSegments;
        const D2D1_POINT_2F p = D2D1::Point2F(cx + r * (float)std::cos(a), cy + r * (float)std::sin(a));
        g_rt->DrawLine(prev, p, g_brush, 1.6f * s, g_roundStroke);
        prev = p;
    }
    // Arrowhead at the end of the arc: two short strokes swept back from the direction of travel.
    const double end = angle + span;
    const double bx = std::sin(end), by = -std::cos(end);  // opposite of the tangent
    for (int k = -1; k <= 1; k += 2) {
        const double w = 0.6 * k;
        const float dx = (float)(bx * std::cos(w) - by * std::sin(w));
        const float dy = (float)(bx * std::sin(w) + by * std::cos(w));
        g_rt->DrawLine(prev, D2D1::Point2F(prev.x + dx * 4.5f * s, prev.y + dy * 4.5f * s), g_brush,
                       1.6f * s, g_roundStroke);
    }
}

// Card footer: separator, reindex button on the left and shortcut hints on the right.
static bool CalcHistoryVisible();  // defined further down
static void DrawFooter(float cx, float footerTop, float cw, bool hasSel) {
    const float s = (float)g_scale;
    SetBrush(g_pal.separator);
    D2D1_RECT_F sep = D2D1::RectF(cx + 12 * s, footerTop, cx + cw - 12 * s, footerTop + 1.f);
    g_rt->FillRectangle(&sep, g_brush);

    const float yMid = footerTop + kFooterH * s / 2 + 0.5f;

    const D2D1_RECT_F btn = ReindexButtonRect(cx, footerTop);
    if (g_hoverReindex) FillRR(btn.left, btn.top, btn.right, btn.bottom, 8 * s, g_pal.selection);
    const double angle = g_indexing ? std::fmod(NowSec() * 6.0, 2 * kPi) : 0.6;
    DrawRefreshIcon(btn.left + 18 * s, yMid, 6.5f * s, angle, g_hoverReindex ? g_pal.text : g_pal.textDim);
    DrawHint(kTxtReindex, kReindexKeys, 2, btn.right - 6 * s, yMid);

    // The calculation owns the footer while its history is on screen: it offers removing the
    // highlighted entry or clearing the whole history (same shortcut as "remove from recents").
    if (CalcHistoryVisible()) {
        const wchar_t* dk[3] = {L"Ctrl", L"Shift", L"R"};
        const float x = cx + cw - 20 * s;
        DrawHint(g_calcHistSel >= 0 ? kTxtCalcDelete : kTxtCalcClear, dk, 3, x, yMid);
        return;
    }
    if (!hasSel) return;

    const wchar_t* dk[2] = {L"Ctrl", L"I"};
    int nk = 2;
    switch (g_cfg.detailsKey) {
        case DetailsKey::CtrlI:     dk[1] = L"I"; break;
        case DetailsKey::CtrlK:     dk[1] = L"K"; break;
        case DetailsKey::CtrlSpace: dk[1] = L"Space"; break;
        case DetailsKey::F4:        dk[0] = L"F4"; nk = 1; break;
    }
    float x = cx + cw - 20 * s;
    x = DrawHint(kTxtDetails, dk, nk, x, yMid);

    x -= 12 * s;
    SetBrush(g_pal.separator);
    D2D1_RECT_F vdiv = D2D1::RectF(x, yMid - 9 * s, x + 1.f, yMid + 9 * s);
    g_rt->FillRectangle(&vdiv, g_brush);
    x -= 12 * s;

    const wchar_t* ok[1] = {kTxtEnterGlyph};
    DrawHint(kTxtOpen, ok, 1, x, yMid);
}

// True when a client point is over the (fully revealed) footer reindex button.
static bool HitReindex(int mx, int my) {
    if (g_vis == Vis::Hidden || !g_fmtHint || g_curH < ExpandedH() - 2) return false;
    const float s = (float)g_scale;
    float cx = 0, cy = 0;
    MainOrigin(cx, cy);
    const D2D1_RECT_F r = ReindexButtonRect(cx, cy + (float)(ExpandedH() - kFooterH) * s);
    return mx >= r.left && mx < r.right && my >= r.top && my < r.bottom;
}

static void DrawDetailsPanel(AppEntry& app, const AppDetails& d) {
    const float s = (float)g_scale;
    const float a = (float)std::clamp(g_detailPos, 0.0, 1.0);
    float mainX = 0, mainY = 0;
    MainOrigin(mainX, mainY);
    // Attached to the right edge of the main panel; it slides in a bit when opening.
    const float ox = mainX + (kWidth + kPanelGap + (1.f - a) * 14.f) * s;
    const float oy = mainY;
    const float pw = kPanelW * s;
    const float ph = (float)DetailsPanelH() * s;   // always the same as the search bar
    const float pad = kPanelPad * s;

    // With live acrylic the DWM window behind the panel is only pw * a wide while it opens or
    // closes (see LiveSync), so it sweeps. The card's own layers (shadow, veil, glow, grain, rim,
    // border) and the content clip use that same width; at full width they would only fade and
    // would not follow the window.
    const float cardW = UseLive() ? std::max(1.f, pw * a) : pw;

    DrawCard(ox, oy, cardW, ph, a);

    D2D1_RECT_F clip = D2D1::RectF(ox, oy, ox + cardW, oy + ph);
    g_rt->PushAxisAlignedClip(&clip, D2D1_ANTIALIAS_MODE_ALIASED);

    const float icon = 44 * s;
    if (ID2D1Bitmap* bmp = EnsureIcon(app)) {
        D2D1_RECT_F dst = D2D1::RectF(ox + pad, oy + pad, ox + pad + icon, oy + pad + icon);
        g_rt->DrawBitmap(bmp, &dst, a, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
    } else {
        FillRR(ox + pad + 3 * s, oy + pad + 3 * s, ox + pad + icon - 3 * s, oy + pad + icon - 3 * s, 10 * s,
               g_pal.textDim, 0.18f * a);
    }
    const float tx = ox + pad + icon + 12 * s;
    DrawLabel(app.name, g_fmtPanelTitle, tx, oy + pad, ox + pw - pad, oy + pad + 24 * s, g_pal.text, a);
    DrawLabel(d.packaged ? L"Store application (MSIX)" : L"Desktop application", g_fmtPanelSub,
              tx, oy + pad + 24 * s, ox + pw - pad, oy + pad + 44 * s, g_pal.textDim, a);

    DrawLabel(kTxtActions, g_fmtSection, ox + pad, oy + 68 * s, ox + pw - pad, oy + 86 * s, g_pal.textDim, a);
    int acts[8];
    const int n = BuildActions(app, d, acts);
    const float actTop = oy + kPanelActionsTop * s;
    for (int k = 0; k < n; ++k) {
        const ActionDef* ad = FindAction(acts[k]);
        if (!ad) continue;
        const float y0 = actTop + k * kActionH * s;
        const float y1 = y0 + kActionH * s;
        if (acts[k] == g_hoverAction) {
            FillRR(ox + 8 * s, y0, ox + pw - 8 * s, y1, 8 * s, g_pal.selection, a);
        }
        DrawLabel(ad->label, g_fmtPanelRow, ox + pad, y0, ox + pw - pad, y1, g_pal.text, a);
        DrawLabel(ad->keys, g_fmtHint, ox + pad, y0, ox + pw - pad, y1, g_pal.textDim, a);
    }

    const float sepY = actTop + n * kActionH * s + 8 * s;
    SetBrush(g_pal.separator, a);
    D2D1_RECT_F sep = D2D1::RectF(ox + pad, sepY, ox + pw - pad, sepY + 1.f);
    g_rt->FillRectangle(&sep, g_brush);

    const float infoTop = sepY + 10 * s;
    DrawLabel(kTxtInfo, g_fmtSection, ox + pad, infoTop, ox + pw - pad, infoTop + 16 * s, g_pal.textDim, a);

    const float labelW = 84 * s;
    const float valX = ox + pad + labelW + 8 * s;
    const float valW = (ox + pw - pad) - valX;
    float ry = infoTop + 22 * s;
    for (const InfoRow& r : d.rows) {
        if (ry + kInfoRowH * s > oy + ph - 10 * s) break;
        DrawLabel(r.label, g_fmtPanelRow, ox + pad, ry, ox + pad + labelW, ry + kInfoRowH * s, g_pal.textDim, a);
        const std::wstring v = r.middle ? FitMiddle(r.value, g_fmtPanelRow, valW) : r.value;
        DrawLabel(v, g_fmtPanelRow, valX, ry, ox + pw - pad, ry + kInfoRowH * s, g_pal.text, a);
        ry += kInfoRowH * s;
    }

    g_rt->PopAxisAlignedClip();
}

// Id of the panel action under a client point, or -1
static int HitAction(int mx, int my) {
    if (!g_detailsOpen || g_detailPos < 0.6 || g_results.empty()) return -1;
    if (g_sel < 0 || g_sel >= (int)g_results.size()) return -1;
    const float s = (float)g_scale;
    float mainX = 0, mainY = 0;
    MainOrigin(mainX, mainY);
    const float lx = (mx - mainX) / s - (kWidth + kPanelGap);
    const float ly = (my - mainY) / s;
    if (lx < 8 || lx > kPanelW - 8) return -1;
    AppEntry& a = g_apps[g_results[g_sel].app];
    const AppDetails& d = GetDetails(a);
    int acts[8];
    const int n = BuildActions(a, d, acts);
    for (int k = 0; k < n; ++k) {
        const float y0 = kPanelActionsTop + k * kActionH;
        if (ly >= y0 && ly < y0 + kActionH) return acts[k];
    }
    return -1;
}

// Canonical form used ONLY inside the cards (never in the search field, which always shows exactly
// what the user typed): typographic maths symbols instead of the raw ones, and no spaces, so every
// entry reads the same no matter how it was written or pasted.
static bool IsCalcSymbol(wchar_t c);  // defined below

// Renders an expression for the cards: operator glyphs become math symbols, spaces around
// operators are dropped, but spaces between words (e.g. "10 km to miles") are kept.
static std::wstring CalcDisplayExpr(const std::wstring& in) {
    auto isSpace = [](wchar_t c) {
        return c == L' ' || c == L'\t' || c == L'\r' || c == L'\n' || c == 0x00A0 || c == 0x2009;
    };
    auto isWord = [](wchar_t c) {
        if (IsCalcSymbol(c)) return false;
        return (c >= L'0' && c <= L'9') || (c >= L'a' && c <= L'z') ||
               (c >= L'A' && c <= L'Z') || c >= 0x80;
    };
    std::wstring out;
    out.reserve(in.size());
    for (size_t idx = 0; idx < in.size(); ++idx) {
        const wchar_t c = Calc::detail::NormalizeMathChar(in[idx]);
        if (isSpace(c)) {
            const wchar_t prev = out.empty() ? 0 : out.back();
            wchar_t next = 0;
            for (size_t k = idx + 1; k < in.size(); ++k) {
                const wchar_t nc = Calc::detail::NormalizeMathChar(in[k]);
                if (!isSpace(nc)) { next = nc; break; }
            }
            if (isWord(prev) && isWord(next) && (out.empty() || out.back() != L' ')) {
                out.push_back(L' ');
            }
            continue;
        }
        switch (c) {
            case L'*': out.push_back(L'\u00D7'); break;  // ×
            case L'/': out.push_back(L'\u00F7'); break;  // ÷
            case L'-': out.push_back(L'\u2212'); break;  // −
            default: out.push_back(c);
        }
    }
    return out;
}

// The operator glyphs drawn in their own (lighter) colour inside the cards.
static bool IsCalcSymbol(wchar_t c) {
    return c == L'\u00D7' || c == L'\u00F7' || c == L'\u2212' || c == L'+' || c == L'^' ||
           c == L'%' || c == L'!';
}

// Draws an expression with the operator glyphs in a lighter colour. Runs are measured and placed
// manually so the two colours butt together; drawing is clipped to `maxX`.
static void DrawCalcExpr(const std::wstring& expr, IDWriteTextFormat* fmt, const D2D1_COLOR_F& c,
                         float alpha, float symAlpha, float x, float y0, float y1, float maxX) {
    if (!fmt || expr.empty()) return;
    size_t i = 0;
    while (i < expr.size() && x < maxX) {
        const bool sym = IsCalcSymbol(expr[i]);
        size_t j = i;
        while (j < expr.size() && IsCalcSymbol(expr[j]) == sym) ++j;
        const std::wstring run = expr.substr(i, j - i);
        const float w = TextWidth(run.c_str(), fmt);
        DrawLabel(run, fmt, x, y0, std::min(x + w + 2, maxX), y1, c, sym ? symAlpha : alpha);
        x += w;
        i = j;
    }
}

// Calculator result card, drawn in the space reserved above the list. It shows the typed
// expression, the result (or a short error) and, on the right, how to copy it. After a copy it
// briefly flashes "Copied".
static void DrawCalcHero(float cx, float top, float cw) {
    const float s = (float)g_scale;
    const float pad = 14.f * s;
    const float x0 = cx + 8 * s;
    const float x1 = cx + cw - 8 * s;
    const float y0 = top + kListPad * s;
    const float y1 = y0 + (kHeroH - 2 * kListPad) * s;

    FillRR(x0, y0, x1, y1, kRowRadius * s, g_pal.selection);
    SetBrush(g_pal.border);
    D2D1_ROUNDED_RECT b = D2D1::RoundedRect(
        D2D1::RectF(x0 + 0.5f, y0 + 0.5f, x1 - 0.5f, y1 - 0.5f), kRowRadius * s, kRowRadius * s);
    g_rt->DrawRoundedRectangle(&b, g_brush, 1.f);

    const float midY = (y0 + y1) / 2;
    const float textW = (x1 - 150 * s) - (x0 + pad);

    D2D1_RECT_F clip = D2D1::RectF(x0, y0, x1 - 132 * s, y1);
    g_rt->PushAxisAlignedClip(&clip, D2D1_ANTIALIAS_MODE_ALIASED);

    // Small expression on top, in the canonical card format with lighter operator glyphs.
    DrawCalcExpr(CalcDisplayExpr(g_text), g_fmtTitle, g_pal.textDim, 0.9f, 0.5f, x0 + pad,
                 y0 + 6 * s, midY + 2 * s, x0 + pad + textW);

    if (CalcOk()) {
        std::wstring res = L"= " + g_calcDisplay;
        SetBrush(g_pal.text);
        D2D1_RECT_F rr = D2D1::RectF(x0 + pad, midY - 2 * s, x0 + pad + textW, y1 - 4 * s);
        g_rt->DrawText(res.c_str(), (UINT32)res.size(), g_fmtInput, &rr, g_brush);
    } else {
        SetBrush(g_pal.textDim);
        D2D1_RECT_F rr = D2D1::RectF(x0 + pad, midY - 2 * s, x0 + pad + textW, y1 - 4 * s);
        g_rt->DrawText(g_calc.error.c_str(), (UINT32)g_calc.error.size(), g_fmtTitle, &rr, g_brush);
    }
    g_rt->PopAxisAlignedClip();

    // The action hint belongs to whatever is selected: the result card only shows it while the
    // selection is on the result itself (not while a history row is selected).
    if (CalcOk() && g_calcHistSel < 0) {
        const bool copied = (NowSec() - g_copiedAt) < 1.4;
        if (copied) {
            SetBrush(g_pal.accent);
            D2D1_RECT_F cr = D2D1::RectF(x1 - 120 * s, y0, x1 - pad, y1);
            g_rt->DrawText(kTxtCalcCopied, (UINT32)wcslen(kTxtCalcCopied), g_fmtHint, &cr, g_brush);
        } else {
            const wchar_t* const keys[1] = {kTxtEnterGlyph};
            DrawHint(kTxtCalcCopy, keys, 1, x1 - pad, midY);
        }
    }
}

// Compact, muted rows below the result card with recent calculations. They are deliberately
// plainer than the result card (no border, dimmer text) so the two are not confused. Clicking a
// row (or picking it with the arrows and Enter) inserts its result at the caret.
static void ReplaceSelection(const std::wstring& s);  // defined further down

static void DrawCalcHistory(float cx, float listTop, float cw) {
    if (g_calcHist.empty()) return;
    const float s = (float)g_scale;
    const float origin = listTop + (kListPad + (float)HeroH()) * s;
    const float cy = listTop - kBarH * s;
    const float footerTop = cy + (float)(ExpandedH() - kFooterH) * s;
    const float listBottom = std::min(cy + (float)ExpandedH() * s, footerTop - kListPad * s);

    const float rowX0 = cx + 8 * s;
    const float rowX1 = cx + cw - 8 * s;
    const float headerH = kSectionH * s;
    const float rowH = kHistRowH * s;
    const float firstY = origin + headerH - (float)g_histScroll * s;

    // Clip the whole history to the area below the result card, so rows (and the header) slide
    // under it while scrolling instead of drawing over it.
    D2D1_RECT_F hclip = D2D1::RectF(cx, origin, cx + cw, listBottom);
    g_rt->PushAxisAlignedClip(&hclip, D2D1_ANTIALIAS_MODE_ALIASED);

    // Section label, scrolls with the rows (like the "Recent" / "Suggestions" headers).
    SetBrush(g_pal.textDim);
    const float headerY = origin - (float)g_histScroll * s;
    D2D1_RECT_F hr = D2D1::RectF(rowX0 + 10 * s, headerY, rowX1, headerY + headerH);
    g_rt->DrawText(kTxtCalcHistory, (UINT32)wcslen(kTxtCalcHistory), g_fmtSection, &hr, g_brush);

    const int n = (int)g_calcHist.size();
    for (int i = 0; i < n; ++i) {
        const float y = firstY + (float)i * rowH;
        if (y + rowH <= origin || y >= listBottom) continue;

        const bool sel = (i == g_calcHistSel);
        if (sel) {
            FillRR(rowX0, y + 1, rowX1, y + rowH - 1, 8 * s, g_pal.text, 0.07f);
        }

        // The selected row shows its own action hints ("Edit [Tab]" and "Add to operation [Enter]");
        // the others leave that space to the result text.
        const wchar_t* const addKeys[1] = {kTxtEnterGlyph};
        const wchar_t* const editKeys[1] = {L"Tab"};
        const float hintW = sel ? HintWidth(kTxtCalcAdd, addKeys, 1) + 14 * s +
                                      HintWidth(kTxtCalcEdit, editKeys, 1) + 10 * s
                                : 0.f;
        const float resRight = rowX1 - 14 * s - hintW;

        // The result takes the width it needs (up to ~62% of the row) so longer values such as
        // "1,000 MB  (binary 1,024 MB)" stay readable; the expression gets what is left.
        const std::wstring res = L"= " + g_calcHist[i].resFmt;
        const float maxResW = (rowX1 - rowX0) * 0.62f;
        const float resW = std::min(TextWidth(res.c_str(), g_fmtHint) + 6 * s, maxResW);
        const float resLeft = resRight - resW;

        const float tx = rowX0 + 16 * s;
        const float tw = std::max(0.f, (resLeft - 10 * s) - tx);
        DrawCalcExpr(CalcDisplayExpr(g_calcHist[i].expr), g_fmtTitle, g_pal.text, 0.6f, 0.33f, tx, y,
                     y + rowH, tx + tw);

        SetBrush(g_pal.text, 0.5f);
        D2D1_RECT_F rr = D2D1::RectF(resLeft, y, resRight, y + rowH);
        g_rt->DrawText(res.c_str(), (UINT32)res.size(), g_fmtHint, &rr, g_brush);

        if (sel) {
            float hx = DrawHint(kTxtCalcAdd, addKeys, 1, rowX1 - 8 * s, y + rowH / 2);
            DrawHint(kTxtCalcEdit, editKeys, 1, hx - 14 * s, y + rowH / 2);
        }
    }
    g_rt->PopAxisAlignedClip();
}

// Height (DIP) available for the history rows (shared with the app list viewport).
static double CalcHistViewportH() {
    return ListViewportH();
}

static double CalcHistContentH() {
    return kSectionH + (double)g_calcHist.size() * kHistRowH;
}

static void ClampHistScroll() {
    const double maxScroll = std::max(0.0, CalcHistContentH() - CalcHistViewportH());
    g_histScroll = std::clamp(g_histScroll, 0.0, maxScroll);
}

static void EnsureHistSelVisible() {
    if (g_calcHistSel < 0) return;
    const double top = kSectionH + g_calcHistSel * (double)kHistRowH;
    const double bot = top + kHistRowH;
    const double view = CalcHistViewportH();
    if (top < g_histScroll) {
        g_histScroll = top;
    } else if (bot > g_histScroll + view) {
        g_histScroll = bot - view;
    }
    ClampHistScroll();
}

// Index of the history row under a client point, or -1.
static int HitCalcHistory(int mx, int my) {
    if (!g_calcActive || g_calcHist.empty()) return -1;
    const float s = (float)g_scale;
    float cx = 0, cy = 0;
    MainOrigin(cx, cy);
    const float cw = kWidth * s;
    if (mx < cx || mx > cx + cw) return -1;
    const float origin = cy + (kBarH + kListPad + (float)HeroH()) * s;
    const float footerTop = cy + (float)(ExpandedH() - kFooterH) * s;
    const float listBottom = std::min(cy + (float)ExpandedH() * s, footerTop - kListPad * s);
    if (my < origin || my >= listBottom) return -1;
    const float firstY = origin + kSectionH * s - (float)g_histScroll * s;
    const int idx = (int)std::floor((my - firstY) / (kHistRowH * s));
    if (idx < 0 || idx >= (int)g_calcHist.size()) return -1;
    return idx;
}

static void InsertCalcHistory(int i) {
    if (i < 0 || i >= (int)g_calcHist.size()) return;
    const std::wstring& v = g_calcHist[i].insVal.empty() ? g_calcHist[i].resFmt
                                                         : g_calcHist[i].insVal;
    ReplaceSelection(v);
}

// Moves a history entry's original expression into the search field (replacing whatever is there)
// so it can be edited. Bound to Tab while a history row is selected.
static void EditCalcHistoryOp(int i) {
    if (i < 0 || i >= (int)g_calcHist.size()) return;
    const std::wstring e = g_calcHist[i].expr;
    if (e.empty() || e.size() > 200) return;
    g_text = e;
    g_caret = g_anchor = (int)g_text.size();
    OnTextChanged();
}

// The history is drawn (and interactive) whenever the text is a recognised maths expression and
// no app results are shown: it stays visible while the user is still composing an expression
// (a partial/errored one), so a previous result can be dropped into the current operation.
static bool CalcHistoryVisible() {
    return g_calcActive && g_results.empty() && !g_calcHist.empty();
}

// Removes one history entry (its position in the list is its index).
static void DeleteCalcHistory(int i) {
    if (i < 0 || i >= (int)g_calcHist.size()) return;
    g_calcHist.erase(g_calcHist.begin() + i);
    if (g_calcHistSel > i) {
        --g_calcHistSel;
    } else if (g_calcHistSel == i) {
        g_calcHistSel = (i < (int)g_calcHist.size()) ? i : (int)g_calcHist.size() - 1;
    }
    if (g_calcHist.empty()) {
        g_calcHistSel = -1;
        g_histScroll = 0;
    } else {
        ClampHistScroll();
        EnsureHistSelVisible();
    }
    SaveCalcHistory();
    MarkDirty();
}

static void ClearCalcHistory() {
    if (g_calcHist.empty()) return;
    g_calcHist.clear();
    g_calcHistSel = -1;
    g_histScroll = 0;
    SaveCalcHistory();
    MarkDirty();
}

static void Render() {
    if (!g_dib || !g_dw || !EnsureTarget()) {
        return;
    }
    const float s = (float)g_scale;

    // Details panel data (computed before drawing starts). If there is no longer a
    // selection (e.g. the text was deleted) the panel keeps showing the last app while it
    // fades out.
    AppEntry* panelApp = nullptr;
    const AppDetails* panelDetails = nullptr;
    if (g_detailPos > 0.01) {
        if (g_sel >= 0 && g_sel < (int)g_results.size()) {
            panelApp = &g_apps[g_results[g_sel].app];
        } else if (!g_panelAppId.empty()) {
            for (auto& a : g_apps) {
                if (a.id == g_panelAppId) {
                    panelApp = &a;
                    break;
                }
            }
        }
        if (panelApp) panelDetails = &GetDetails(*panelApp);
    }

    RECT rc = {0, 0, g_bmpW, g_bmpH};
    HRESULT hrBind = g_rt->BindDC(g_memDC, &rc);
    if (FAILED(hrBind)) {
        return;
    }
    g_rt->BeginDraw();
    g_rt->SetTransform(D2D1::Matrix3x2F::Identity());
    g_rt->Clear(D2D1::ColorF(0, 0, 0, 0));
    g_rt->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
    g_rt->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);

    float cx = 0, cy = 0;
    MainOrigin(cx, cy);
    const float cw = kWidth * s;
    const float ch = (float)g_curH * s;

    DrawCard(cx, cy, cw, ch, 1.f);

    // ---- Search bar ----
    const float barMid = cy + kBarH * s / 2;
    DrawMagnifier(cx + 30 * s, barMid);

    const float fieldX = cx + 58 * s;
    const float fieldW = (kWidth - 58 - 24) * s;
    {
        D2D1_RECT_F clip = D2D1::RectF(fieldX, cy, fieldX + fieldW, cy + kBarH * s);
        g_rt->PushAxisAlignedClip(&clip, D2D1_ANTIALIAS_MODE_ALIASED);

        if (g_text.empty()) {
            SetBrush(g_pal.textDim, 0.85f);
            D2D1_RECT_F r = D2D1::RectF(fieldX, cy, fieldX + fieldW, cy + kBarH * s);
            g_rt->DrawText(kTxtPlaceholder, (UINT32)wcslen(kTxtPlaceholder), g_fmtInput, &r, g_brush);
            if (g_caretOn && g_vis != Vis::Hiding) {
                SetBrush(g_pal.text);
                float ch2 = 26 * s;
                D2D1_RECT_F cr = D2D1::RectF(fieldX, barMid - ch2 / 2, fieldX + 1.5f * s, barMid + ch2 / 2);
                g_rt->FillRectangle(&cr, g_brush);
            }
        } else {
            IDWriteTextLayout* tl = nullptr;
            g_dw->CreateTextLayout(g_text.c_str(), (UINT32)g_text.size(), g_fmtInput, 10000.f,
                                   kBarH * s, &tl);
            if (tl) {
                float cxPos = 0, cyTmp = 0;
                DWRITE_HIT_TEST_METRICS hm = {};
                tl->HitTestTextPosition((UINT32)g_caret, FALSE, &cxPos, &cyTmp, &hm);

                if (cxPos - g_inputScroll > fieldW - 4 * s) g_inputScroll = cxPos - (fieldW - 4 * s);
                if (cxPos - g_inputScroll < 0) g_inputScroll = cxPos;
                if (g_inputScroll < 0) g_inputScroll = 0;

                const float ox = fieldX - g_inputScroll;
                float chh = 26 * s;

                if (g_anchor != g_caret) {
                    float ax = 0, ay = 0;
                    DWRITE_HIT_TEST_METRICS hm2 = {};
                    tl->HitTestTextPosition((UINT32)g_anchor, FALSE, &ax, &ay, &hm2);
                    float x0 = std::min(ax, cxPos);
                    float x1 = std::max(ax, cxPos);
                    SetBrush(g_pal.caretSel);
                    D2D1_RECT_F sr = D2D1::RectF(ox + x0, barMid - chh / 2, ox + x1, barMid + chh / 2);
                    g_rt->FillRectangle(&sr, g_brush);
                }

                SetBrush(g_pal.text);
                g_rt->DrawTextLayout(D2D1::Point2F(ox, cy), tl, g_brush);

                // Inline calculator result, faintly after the typed text.
                if (GhostShown()) {
                    float endX = 0, endY = 0;
                    DWRITE_HIT_TEST_METRICS hmEnd = {};
                    tl->HitTestTextPosition((UINT32)g_text.size(), FALSE, &endX, &endY, &hmEnd);
                    std::wstring ghost = L"   = " + g_calcDisplay;
                    SetBrush(g_pal.textDim, 0.75f);
                    D2D1_RECT_F gr = D2D1::RectF(ox + endX, cy, fieldX + fieldW, cy + kBarH * s);
                    g_rt->DrawText(ghost.c_str(), (UINT32)ghost.size(), g_fmtInput, &gr, g_brush);
                }

                if (g_caretOn && g_vis != Vis::Hiding) {
                    SetBrush(g_pal.text);
                    D2D1_RECT_F cr =
                        D2D1::RectF(ox + cxPos, barMid - chh / 2, ox + cxPos + 1.5f * s, barMid + chh / 2);
                    g_rt->FillRectangle(&cr, g_brush);
                }
                tl->Release();
            }
        }
        g_rt->PopAxisAlignedClip();
    }

    // ---- List / messages ----
    const float listTop = cy + kBarH * s;
    const int rows = VisibleRows();

    if (ch > kBarH * s + 2) {
        SetBrush(g_pal.separator);
        D2D1_RECT_F sep = D2D1::RectF(cx + 12 * s, listTop, cx + cw - 12 * s, listTop + 1.f);
        g_rt->FillRectangle(&sep, g_brush);
    }

    // Calculator result card, in the space reserved above the list.
    if (ch > kBarH * s + 2 && HeroShown()) {
        DrawCalcHero(cx, listTop, cw);
    }

    // The footer takes the end of the expanded height; the list ends just above it.
    const float footerTop = cy + (float)(ExpandedH() - kFooterH) * s;
    const float listBottom = std::min(cy + ch, footerTop - kListPad * s);
    D2D1_RECT_F listClip = D2D1::RectF(cx, listTop + 1.f, cx + cw, listBottom);
    g_rt->PushAxisAlignedClip(&listClip, D2D1_ANTIALIAS_MODE_ALIASED);

    if (rows > 0) {
        const float rowsOrigin = listTop + (kListPad + (float)HeroH()) * s;
        const float rowH = kRowH * s;
        const float rowX0 = cx + 8 * s;
        const float rowX1 = cx + cw - 8 * s;
        const double scrollOff = ScrollPx();

        const float hlY = rowsOrigin + (float)(RowOffset(g_selPos) - scrollOff) * s;
        FillRR(rowX0, hlY, rowX1, hlY + rowH, kRowRadius * s, g_pal.selection);
        {
            SetBrush(g_pal.text, 0.06f);
            D2D1_ROUNDED_RECT so = D2D1::RoundedRect(
                D2D1::RectF(rowX0 + 0.5f, hlY + 0.5f, rowX1 - 0.5f, hlY + rowH - 0.5f), kRowRadius * s,
                kRowRadius * s);
            g_rt->DrawRoundedRectangle(&so, g_brush, 1.f);
        }

        const float iconPx = kIconSize * s;
        const float textX = rowX0 + (10 + kIconSize + 14) * s;
        const float textW = (rowX1 - textX) - 130 * s;

        int first = std::max(0, (int)std::floor(g_scrollPos) - (g_kbScroll ? 2 : 0));
        int last = std::min((int)g_results.size() - 1, (int)std::ceil(g_scrollPos) + rows);
        for (int i = first; i <= last; ++i) {
            SearchResult& res = g_results[i];
            AppEntry& app = g_apps[res.app];
            float y = rowsOrigin + (float)(RowOffset((double)i) - scrollOff) * s;

            // Section header ("Recent" / "Suggestions") above the first row of each group
            if (res.section != 0 && (i == 0 || g_results[i - 1].section != res.section)) {
                const wchar_t* secTxt = (res.section == 1) ? kTxtRecents : kTxtSuggestions;
                SetBrush(g_pal.textDim);
                D2D1_RECT_F secRect = D2D1::RectF(rowX0 + 10 * s, y - kSectionH * s, rowX1, y);
                g_rt->DrawText(secTxt, (UINT32)wcslen(secTxt), g_fmtSection, &secRect, g_brush);
            }

            float ix = rowX0 + 10 * s;
            float iy = y + (rowH - iconPx) / 2;
            ID2D1Bitmap* bmp = EnsureIcon(app);
            if (bmp) {
                D2D1_RECT_F dst = D2D1::RectF(ix, iy, ix + iconPx, iy + iconPx);
                g_rt->DrawBitmap(bmp, &dst, 1.f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
            } else {
                FillRR(ix + 2 * s, iy + 2 * s, ix + iconPx - 2 * s, iy + iconPx - 2 * s, 8 * s,
                       g_pal.textDim, 0.18f);
            }

            if (!res.layout) {
                res.layout = BuildTitleLayout(app, textW, rowH);
            }
            if (res.layout) {
                SetBrush(g_pal.text);
                g_rt->DrawTextLayout(D2D1::Point2F(textX, y), res.layout, g_brush);
            }

            // Type label, on the right (like "Application" / "Command" in other launchers)
            {
                const wchar_t* typeTxt = IsPackagedId(app.id) ? kTxtTypeStore : kTxtTypeApp;
                SetBrush(g_pal.textDim);
                D2D1_RECT_F tr = D2D1::RectF(rowX1 - 150 * s, y, rowX1 - 14 * s, y + rowH);
                g_rt->DrawText(typeTxt, (UINT32)wcslen(typeTxt), g_fmtHint, &tr, g_brush);
            }
        }
    } else if (g_calcActive) {
        DrawCalcHistory(cx, listTop, cw);
    } else if (ch > kBarH * s + 8) {
        const wchar_t* msg = nullptr;
        if (!g_tokens.empty() && !CalcShown()) {
            msg = kTxtNoResults;
        } else if (g_apps.empty() && !g_indexedOnce && !CalcShown()) {
            msg = kTxtIndexing;
        }
        if (msg) {
            SetBrush(g_pal.textDim);
            // "No results" is centered in the whole list area; "Indexing…" is a short row
            const float msgBottom = g_tokens.empty() ? listTop + kEmptyRowH * s : listBottom;
            D2D1_RECT_F r = D2D1::RectF(cx, listTop + 1.f, cx + cw, msgBottom);
            g_rt->DrawText(msg, (UINT32)wcslen(msg), g_fmtEmpty, &r, g_brush);
        }
    }
    g_rt->PopAxisAlignedClip();

    // ---- Footer with shortcuts (revealed as the card expands) ----
    if (cy + ch > footerTop + 2.f) {
        D2D1_RECT_F footClip = D2D1::RectF(cx, footerTop, cx + cw, cy + ch);
        g_rt->PushAxisAlignedClip(&footClip, D2D1_ANTIALIAS_MODE_ALIASED);
        DrawFooter(cx, footerTop, cw, !g_results.empty() && g_sel >= 0 && g_sel < (int)g_results.size());
        g_rt->PopAxisAlignedClip();
    }

    // ---- Details panel (on the right) ----
    if (panelApp && panelDetails) {
        DrawDetailsPanel(*panelApp, *panelDetails);
    }

    HRESULT hr = g_rt->EndDraw();
    if (hr == D2DERR_RECREATE_TARGET) {
        ReleaseTarget();
        g_dirty = true;
    }
}

static void Present() {
    if (!g_hwnd || !g_dib) {
        return;
    }
    POINT ptSrc = {0, 0};
    POINT ptDst = {g_winX, g_winY + (int)std::lround(g_offY)};
    SIZE size = {g_bmpW, g_bmpH};
    BLENDFUNCTION bf = {};
    bf.BlendOp = AC_SRC_OVER;
    bf.SourceConstantAlpha = (BYTE)std::clamp((int)std::lround(g_alpha * 255.0), 0, 255);
    bf.AlphaFormat = AC_SRC_ALPHA;
    UpdateLayeredWindow(g_hwnd, nullptr, &ptDst, &size, g_memDC, &ptSrc, 0, &bf, ULW_ALPHA);
    if (UseLive()) {
        LiveSync();
    } else {
        LiveHide();
    }
}

// ===== Show / hide / per-frame animation =====

static void ForceForeground(HWND h) {
    // The hotkey (a global input event) already grants foreground rights; the empty SendInput
    // covers the case where SetForegroundWindow would still be denied. No AttachThreadInput:
    // while sharing an input queue with the foreground app, a hung app would stall the launcher.
    INPUT input{};
    input.type = INPUT_MOUSE;
    SendInput(1, &input, sizeof(INPUT));
    SetForegroundWindow(h);
    BringWindowToTop(h);
    SetFocus(h);
}

// True when the foreground window covers its monitor and is not the desktop or taskbar (the
// usual heuristic for a focused fullscreen app: games, video players, F11 browsers). A few
// pixels of tolerance keep a maximized window from counting when it does not fully cover it.
static bool IsForegroundFullscreen() {
    HWND fg = GetForegroundWindow();
    if (!fg || fg == g_hwnd) {
        return false;
    }
    // A maximized window's rect includes the invisible resize border, so on a monitor whose
    // taskbar auto-hides it would pass the "covers the monitor" test below. A maximized window
    // is not fullscreen (F11 browsers and fullscreen games are not zoomed), so it is ruled out.
    if (IsZoomed(fg)) {
        return false;
    }
    // The shell windows (desktop, taskbar, Start menu) are never treated as fullscreen.
    wchar_t cls[64] = {};
    if (GetClassNameW(fg, cls, 64)) {
        if (lstrcmpW(cls, L"Progman") == 0 || lstrcmpW(cls, L"WorkerW") == 0 ||
            lstrcmpW(cls, L"Shell_TrayWnd") == 0 || lstrcmpW(cls, L"Shell_SecondaryTrayWnd") == 0 ||
            lstrcmpW(cls, L"Windows.UI.Core.CoreWindow") == 0 ||
            lstrcmpW(cls, L"XamlExplorerHostIslandWindow") == 0) {
            return false;
        }
    }

    RECT wr = {};
    if (!GetWindowRect(fg, &wr)) {
        return false;
    }
    HMONITOR mon = MonitorFromWindow(fg, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = {};
    mi.cbSize = sizeof(mi);
    if (!GetMonitorInfoW(mon, &mi)) {
        return false;
    }
    // The window rect includes the invisible resize border; a few pixels of tolerance keep a
    // maximized window that does not hide the taskbar from counting as fullscreen.
    const int tol = 8;
    const RECT& m = mi.rcMonitor;
    return wr.left <= m.left + tol && wr.top <= m.top + tol &&
           wr.right >= m.right - tol && wr.bottom >= m.bottom - tol;
}

static void FinishHide() {
    if (g_frameTimerOn) {
        KillTimer(g_hwnd, TIMER_FRAME);
        g_frameTimerOn = false;
    }
    KillTimer(g_hwnd, TIMER_BLINK);
    KillTimer(g_hwnd, TIMER_COPY);
    KillTimer(g_hwnd, TIMER_CALC);
    g_calcPending = false;
    ShowWindow(g_hwnd, SW_HIDE);
    LiveHide();
    g_vis = Vis::Hidden;
    g_lastHide = NowSec();
    ReleaseBackdrop();
    ReleaseResultLayouts();
    Sound::Suspend();
    MaybeReindexInBackground();  // changes detected while it was open
}

static void HideLauncher() {
    if (g_vis == Vis::Hidden || g_vis == Vis::Hiding) {
        return;
    }
    g_vis = Vis::Hiding;
    g_visStart = NowSec();
    StartFrameTimer();
}

static void ShowLauncher() {
    if (!g_hwnd) {
        return;
    }
    POINT pt = {};
    GetCursorPos(&pt);
    HMONITOR mon = MonitorFromPoint(pt, MONITOR_DEFAULTTOPRIMARY);
    MONITORINFO mi = {};
    mi.cbSize = sizeof(mi);
    GetMonitorInfoW(mon, &mi);

    double monScale = MonitorScale(mon);
    if (!EnsureSurface(monScale)) {
        return;
    }
    LoadPalette();
    // With live acrylic on Windows 11, DWM rounds the corners (fixed radius ~8 px).
    kRadius = (UseLive() && g_liveWin11) ? 8.f : 20.f;
    if (!EnsureTarget()) {
        return;
    }
    g_matchBrush->SetColor(&g_pal.match);

    g_text.clear();
    g_caret = g_anchor = 0;
    g_inputScroll = 0;
    g_caretOn = true;
    g_copiedAt = -1e9;
    g_calc = Calc::Result();
    g_calcDisplay.clear();
    g_calcActive = false;
    g_calcPending = false;
    g_calcHistSel = -1;
    g_histScroll = 0;
    if (g_hwnd) KillTimer(g_hwnd, TIMER_COPY);
    if (g_hwnd) KillTimer(g_hwnd, TIMER_CALC);
    g_lastMouse = {-1, -1};
    g_sel = 0;
    g_scrollTarget = 0;
    g_detailsOpen = false;  // the details panel starts closed on each open
    g_detailPos = g_detailTarget = 0;
    g_hoverAction = -1;
    g_hoverReindex = false;
    RunSearch();
    UpdateTargets(true, /*followSelection=*/true);

    // g_anchorTop is the top of the COLLAPSED bar: 0% = top of the work area, 100% = bottom
    // (accounting for the bar height). The bar keeps that position while growing, moving up
    // only if the expanded card would not fit (see MainOrigin).
    const int monW = mi.rcMonitor.right - mi.rcMonitor.left;
    g_winX = mi.rcMonitor.left + (monW - g_bmpW) / 2;
    g_workTop = (double)mi.rcWork.top;
    g_workBottom = (double)mi.rcWork.bottom;
    const double barH = kBarH * g_scale;
    g_anchorTop = mi.rcWork.top + (mi.rcWork.bottom - mi.rcWork.top - barH) *
                                       g_cfg.verticalPct / 100.0;

    // Place the window so the collapsed card sits at g_anchorTop and the surface stays on screen.
    const double expandedCardH = kExpandedMinH * g_scale;
    g_winY = (int)std::lround(g_anchorTop - kMargin * g_scale);
    if (g_winY + expandedCardH + 2 * kMargin * g_scale > mi.rcMonitor.bottom) {
        g_winY = (int)std::lround(mi.rcMonitor.bottom - expandedCardH - 2 * kMargin * g_scale);
    }
    if (g_winY < mi.rcMonitor.top) {
        g_winY = mi.rcMonitor.top;
    }
    if (g_winX + g_bmpW > mi.rcMonitor.right) g_winX = mi.rcMonitor.right - g_bmpW;
    if (g_winX < mi.rcMonitor.left) g_winX = mi.rcMonitor.left;

    // Acrylic: capture the desktop now, with the window still hidden.
    PrepareBackdrop();
    if (UseLive()) {
        LiveApplyAccent();
    }

    g_vis = Vis::Showing;
    g_visStart = NowSec();
    g_shownAt = g_visStart;
    g_alpha = 0;
    g_offY = 14.0 * g_scale;

    Render();
    g_dirty = false;
    Present();
    ShowWindow(g_hwnd, SW_SHOW);
    ForceForeground(g_hwnd);
    Sound::Resume();
    Sound::OnOpen();

    SetTimer(g_hwnd, TIMER_BLINK, 530, nullptr);
    StartFrameTimer();

    if (!g_indexing && ShouldReindexOnOpen()) {
        StartIndex();
    }
}

static void ToggleLauncher() {
    if (g_vis == Vis::Showing || g_vis == Vis::Shown) {
        HideLauncher();
    } else {
        ShowLauncher();
    }
}

static void OnFrame() {
    double t = NowSec();
    double dt = std::min(0.05, t - g_lastFrame);
    g_lastFrame = t;
    bool active = false;
    bool present = false;

    if (g_vis == Vis::Showing) {
        double p = (t - g_visStart) / 0.20;
        if (p >= 1.0) {
            p = 1.0;
            g_vis = Vis::Shown;
        } else {
            active = true;
        }
        double e = EaseOutCubic(p);
        g_alpha = e;
        g_offY = (1.0 - e) * 14.0 * g_scale;
        present = true;
    } else if (g_vis == Vis::Hiding) {
        double p = (t - g_visStart) / 0.12;
        if (p >= 1.0) {
            FinishHide();
            return;
        }
        g_alpha = 1.0 - p * p;
        g_offY = p * 6.0 * g_scale;
        active = true;
        present = true;
    }

    if (g_vis != Vis::Hidden) {
        if (Smooth(g_selPos, g_selTarget, 28.0, dt, 0.002)) { active = true; g_dirty = true; }
        if (Smooth(g_scrollPos, g_scrollTarget, 22.0, dt, 0.002)) { active = true; g_dirty = true; }
        if (Smooth(g_curH, g_targetH, 20.0, dt, 0.3)) { active = true; g_dirty = true; }
        if (Smooth(g_detailPos, g_detailTarget, 16.0, dt, 0.003)) { active = true; g_dirty = true; }
        if (g_indexing) { active = true; g_dirty = true; }  // keeps the footer icon spinning
    }

    if (g_dirty) {
        g_dirty = false;
        Render();
        present = true;
    }
    if (present) {
        Present();
    }
    if (!active && !g_dirty && g_frameTimerOn) {
        KillTimer(g_hwnd, TIMER_FRAME);
        g_frameTimerOn = false;
    }
}

// ===== Launching =====
// Records a use: counter (for "Suggestions") and timestamp (for "Recent").
static void RecordUse(AppEntry& a) {
    a.uses++;
    a.lastUsed = NowStamp();
    Wh_SetIntValue(a.usageKey.c_str(), a.uses);
    Wh_SetIntValue((a.usageKey + L"t").c_str(), a.lastUsed);
    g_detailsCache.erase(a.id);  // "Uses" / "Last used" change
}

// Opens a desktop app through its real target. Resolving "shell:AppsFolder\{GUID}\..." from
// this 32-bit process redirects the FOLDERID_ProgramFilesX64 GUID to "C:\Program Files (x86)",
// so 64-bit apps installed in "C:\Program Files" fail to launch. System.Link.TargetParsingPath
// holds the real, unredirected path and is used instead. Store (MSIX) apps use their AUMID.
static void LaunchApp(int appIdx) {
    if (appIdx < 0 || appIdx >= (int)g_apps.size()) {
        return;
    }
    AppEntry& a = g_apps[appIdx];
    AllowSetForegroundWindow(ASFW_ANY);

    std::wstring file;
    std::wstring args;
    if (!IsPackagedId(a.id)) {
        IShellItem2* item = nullptr;
        std::wstring parse = L"shell:AppsFolder\\" + a.id;
        if (SUCCEEDED(SHCreateItemFromParsingName(parse.c_str(), nullptr, IID_PPV_ARGS(&item))) && item) {
            file = ItemString(item, L"System.Link.TargetParsingPath");
            args = ItemString(item, L"System.Link.Arguments");
            item->Release();
        }
        if (file.empty()) {
            file = PathFromAppId(a.id);
        }
    }

    // Some AppsFolder entries resolve to a shell namespace GUID rather than a file (File Explorer's
    // target is "::{52205FD8-...}", the "This PC" folder), which cannot be started as a file. Drop
    // it so the AUMID route below is used instead.
    if (file.size() >= 2 && file[0] == L':' && file[1] == L':') {
        file.clear();
    }

    if (!file.empty()) {
        if (IsUnderWindowsDir(file)) {
            // A System32 target must not be resolved by this 32-bit process: it would be
            // redirected to SysWOW64 and the tool (or a delay-loaded DLL) would not be found.
            // Disabling the redirection around the call is not safe, because a failed
            // delay-load stays failed for the rest of the process. Let the 64-bit shell resolve
            // the AppsFolder ID instead (explorer.exe is not redirected); it also keeps the
            // shortcut's arguments, working folder and elevation.
            wchar_t win[MAX_PATH];
            UINT n = GetWindowsDirectoryW(win, MAX_PATH);
            if (n && n < MAX_PATH) {
                const std::wstring explorer = std::wstring(win) + L"\\explorer.exe";
                const std::wstring arg = L"shell:AppsFolder\\" + a.id;
                ShellExecuteW(nullptr, L"open", explorer.c_str(), arg.c_str(), nullptr, SW_SHOWNORMAL);
            }
        } else {
            // Start the app in its own folder, not in Windhawk's directory (portable apps and
            // some games read files relative to the working directory).
            std::wstring dir = file.substr(0, file.find_last_of(L'\\'));
            ShellExecuteW(nullptr, L"open", file.c_str(), args.empty() ? nullptr : args.c_str(),
                          dir.empty() ? nullptr : dir.c_str(), SW_SHOWNORMAL);
        }
    } else {
        std::wstring url = L"shell:AppsFolder\\" + a.id;
        ShellExecuteW(nullptr, L"open", url.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
    }
    RecordUse(a);
    HideLauncher();
}

static void LaunchSelected() {
    if (g_results.empty() || g_sel < 0 || g_sel >= (int)g_results.size()) {
        return;
    }
    LaunchApp(g_results[g_sel].app);
}

// ===== Details panel actions =====

static void CopyTextToClipboard(const std::wstring& s) {
    if (!OpenClipboard(g_hwnd)) return;
    EmptyClipboard();
    HGLOBAL h = GlobalAlloc(GMEM_MOVEABLE, (s.size() + 1) * sizeof(wchar_t));
    wchar_t* p = h ? (wchar_t*)GlobalLock(h) : nullptr;
    if (p) {
        memcpy(p, s.c_str(), (s.size() + 1) * sizeof(wchar_t));
        GlobalUnlock(h);
        if (SetClipboardData(CF_UNICODETEXT, h)) h = nullptr;  // the clipboard owns it now
    }
    if (h) GlobalFree(h);
    CloseClipboard();
}

static void RunAction(int actionId, int appIdx) {
    if (appIdx < 0 || appIdx >= (int)g_apps.size()) {
        return;
    }
    AppEntry& a = g_apps[appIdx];
    const AppDetails& d = GetDetails(a);
    if (!ActionAvailable(actionId, a, d)) {
        return;  // e.g. "Run as administrator" on a Store app
    }
    const std::wstring target = d.target;  // copy: RecordUse invalidates the cache

    switch (actionId) {
        case ACT_OPEN:
            LaunchApp(appIdx);
            return;

        case ACT_ADMIN: {
            AllowSetForegroundWindow(ASFW_ANY);
            std::wstring dir = target.substr(0, target.find_last_of(L'\\'));
            // Keep the shortcut's arguments, or a profile shortcut would open the default one.
            HINSTANCE r = ShellExecuteW(nullptr, L"runas", target.c_str(),
                                        d.args.empty() ? nullptr : d.args.c_str(),
                                        dir.empty() ? nullptr : dir.c_str(), SW_SHOWNORMAL);
            if ((INT_PTR)r > 32) {
                RecordUse(a);
            }
            HideLauncher();
            return;
        }

        case ACT_LOCATION: {
            std::wstring args = L"/select,\"" + target + L"\"";
            ShellExecuteW(nullptr, L"open", L"explorer.exe", args.c_str(), nullptr, SW_SHOWNORMAL);
            HideLauncher();
            return;
        }

        case ACT_COPYPATH:
            CopyTextToClipboard(target);
            HideLauncher();
            return;

        case ACT_PROPS: {
            SHELLEXECUTEINFOW sei = {};
            sei.cbSize = sizeof(sei);
            sei.fMask = SEE_MASK_INVOKEIDLIST;
            sei.lpVerb = L"properties";
            sei.lpFile = target.c_str();
            sei.nShow = SW_SHOWNORMAL;
            ShellExecuteExW(&sei);
            HideLauncher();
            return;
        }

        case ACT_FORGET:
            a.uses = 0;
            a.lastUsed = 0;
            Wh_SetIntValue(a.usageKey.c_str(), 0);
            Wh_SetIntValue((a.usageKey + L"t").c_str(), 0);
            g_detailsCache.erase(a.id);
            RunSearch();
            g_sel = std::min(g_sel, std::max(0, (int)g_results.size() - 1));
            UpdateTargets(false, /*followSelection=*/true);
            MarkDirty();
            return;
    }
}

static void RunActionOnSelection(int actionId) {
    if (g_results.empty() || g_sel < 0 || g_sel >= (int)g_results.size()) {
        return;
    }
    RunAction(actionId, g_results[g_sel].app);
}

static void SetDetailsOpen(bool open) {
    if (open && g_results.empty()) {
        return;  // no selected app, nothing to detail
    }
    g_detailsOpen = open;
    g_hoverAction = -1;
    Sound::OnPanel(open);
    UpdateTargets(false, /*followSelection=*/false);
    MarkDirty();
}

static bool IsDetailsKey(WPARAM vk, bool ctrl, bool shift) {
    if (shift) return false;
    switch (g_cfg.detailsKey) {
        case DetailsKey::CtrlI:     return ctrl && vk == 'I';
        case DetailsKey::CtrlK:     return ctrl && vk == 'K';
        case DetailsKey::CtrlSpace: return ctrl && vk == VK_SPACE;
        case DetailsKey::F4:        return !ctrl && vk == VK_F4;
    }
    return false;
}

// ===== Text editing =====

static bool CtrlDown() { return (GetKeyState(VK_CONTROL) & 0x8000) != 0; }
static bool ShiftDown() { return (GetKeyState(VK_SHIFT) & 0x8000) != 0; }

static int PrevWord(int pos) {
    while (pos > 0 && g_text[pos - 1] == L' ') --pos;
    while (pos > 0 && g_text[pos - 1] != L' ') --pos;
    return pos;
}

static int NextWord(int pos) {
    int n = (int)g_text.size();
    while (pos < n && g_text[pos] != L' ') ++pos;
    while (pos < n && g_text[pos] == L' ') ++pos;
    return pos;
}

static void MoveCaret(int pos, bool extend) {
    pos = std::clamp(pos, 0, (int)g_text.size());
    g_caret = pos;
    if (!extend) g_anchor = pos;
    ResetBlink();
    MarkDirty();
}

static void ReplaceSelection(const std::wstring& s) {
    int a = std::min(g_anchor, g_caret);
    int b = std::max(g_anchor, g_caret);
    std::wstring t = g_text.substr(0, a) + s + g_text.substr(b);
    if (t.size() > 200) {
        return;
    }
    g_text = t;
    g_caret = g_anchor = a + (int)s.size();
    OnTextChanged();
}

static void CopySelection(bool cut) {
    int a = std::min(g_anchor, g_caret);
    int b = std::max(g_anchor, g_caret);
    if (a == b) return;
    CopyTextToClipboard(g_text.substr(a, b - a));
    if (cut) ReplaceSelection(L"");
}

static void PasteClipboard() {
    if (!OpenClipboard(g_hwnd)) return;
    std::wstring s;
    HANDLE h = GetClipboardData(CF_UNICODETEXT);
    if (h) {
        const wchar_t* p = (const wchar_t*)GlobalLock(h);
        if (p) {
            s = p;
            GlobalUnlock(h);
        }
    }
    CloseClipboard();
    for (auto& c : s) {
        if (c == L'\r' || c == L'\n' || c == L'\t') c = L' ';
    }
    if (!s.empty()) {
        ReplaceSelection(s);
        Sound::OnKey();
    }
}

static bool HandleKeyDown(WPARAM vk) {
    const bool ctrl = CtrlDown();
    const bool shift = ShiftDown();
    const int len = (int)g_text.size();
    const bool hasSel = g_anchor != g_caret;

    // Details panel: shortcut to show/hide it, and direct keyboard actions
    if (IsDetailsKey(vk, ctrl, shift)) {
        SetDetailsOpen(!g_detailsOpen);
        return true;
    }
    if (ctrl && vk == VK_RETURN) {
        RunActionOnSelection(shift ? ACT_ADMIN : ACT_LOCATION);
        return true;
    }
    if (ctrl && vk == 'P' && !shift) {
        RunActionOnSelection(ACT_PROPS);
        return true;
    }
    if (ctrl && shift && vk == 'C') {
        RunActionOnSelection(ACT_COPYPATH);
        return true;
    }
    if (ctrl && shift && vk == 'R') {
        // With the calculation history on screen this removes an entry (the highlighted one) or,
        // when nothing is highlighted, clears the whole history. Otherwise it forgets the app.
        if (CalcHistoryVisible()) {
            if (g_calcHistSel >= 0) {
                DeleteCalcHistory(g_calcHistSel);
            } else {
                ClearCalcHistory();
            }
            return true;
        }
        RunActionOnSelection(ACT_FORGET);
        return true;
    }
    if (ctrl && !shift && vk == 'R') {
        ForceReindex();
        return true;
    }

    switch (vk) {
        case VK_ESCAPE:
            if (g_detailsOpen) {
                SetDetailsOpen(false);  // the panel is closed first; another Esc closes the bar
            } else {
                HideLauncher();
            }
            return true;
        case VK_RETURN:
            // If the debounced calculation has not run yet, run it now so Enter is immediate.
            if (g_calcPending) {
                if (g_hwnd) KillTimer(g_hwnd, TIMER_CALC);
                g_calcPending = false;
                UpdateCalc();
                RunSearch();
                UpdateTargets(false, /*followSelection=*/true);
            }
            // A highlighted history row inserts its result into the expression. This works for any
            // recognised maths text, valid or still being composed (e.g. "2+2+").
            if (CalcHistoryVisible() && g_calcHistSel >= 0 && g_calcHistSel < (int)g_calcHist.size()) {
                InsertCalcHistory(g_calcHistSel);
                return true;
            }
            // A calculation owns Enter while there is no app result to open: copy the result and
            // keep the bar open so it can be refined or opened again.
            if (CalcOk() && g_results.empty()) {
                AddCalcHistory(g_text, g_calcDisplay, g_calc.insertable);
                SetCalcAns(g_calc.value);
                CopyTextToClipboard(g_calcDisplay);
                g_copiedAt = NowSec();
                g_histScroll = 0;
                g_calcHistSel = -1;
                SetTimer(g_hwnd, TIMER_COPY, 1400, nullptr);
                MarkDirty();
                return true;
            }
            LaunchSelected();
            return true;
        case VK_DOWN:
            if (CalcHistoryVisible()) {
                if (g_calcHistSel < (int)g_calcHist.size() - 1) {
                    ++g_calcHistSel;
                    EnsureHistSelVisible();
                    MarkDirty();
                }
                return true;
            }
            SelectDelta(1);
            return true;
        case VK_UP:
            if (CalcHistoryVisible() && g_calcHistSel >= 0) {
                --g_calcHistSel;
                if (g_calcHistSel < 0) {
                    g_histScroll = 0;
                } else {
                    EnsureHistSelVisible();
                }
                MarkDirty();
                return true;
            }
            SelectDelta(-1);
            return true;
        case VK_TAB:
            // With a history row selected, Tab pulls its original expression into the field so it
            // can be edited (replacing whatever was typed). Otherwise it cycles the selection.
            if (CalcHistoryVisible() && g_calcHistSel >= 0 &&
                g_calcHistSel < (int)g_calcHist.size()) {
                EditCalcHistoryOp(g_calcHistSel);
                return true;
            }
            SelectDelta(shift ? -1 : 1);
            return true;
        case VK_NEXT:
            SelectDelta(std::max(1, VisibleRows()));
            return true;
        case VK_PRIOR:
            SelectDelta(-std::max(1, VisibleRows()));
            return true;
        case VK_LEFT:
            if (hasSel && !shift) {
                MoveCaret(std::min(g_anchor, g_caret), false);
            } else {
                MoveCaret(ctrl ? PrevWord(g_caret) : g_caret - 1, shift);
            }
            return true;
        case VK_RIGHT:
            if (hasSel && !shift) {
                MoveCaret(std::max(g_anchor, g_caret), false);
            } else {
                MoveCaret(ctrl ? NextWord(g_caret) : g_caret + 1, shift);
            }
            return true;
        case VK_HOME:
            MoveCaret(0, shift);
            return true;
        case VK_END:
            MoveCaret(len, shift);
            return true;
        case VK_BACK:
            if (hasSel) {
                ReplaceSelection(L"");
                Sound::OnErase();
            } else if (g_caret > 0) {
                g_anchor = ctrl ? PrevWord(g_caret) : g_caret - 1;
                ReplaceSelection(L"");
                Sound::OnErase();
            }
            return true;
        case VK_DELETE:
            if (hasSel) {
                ReplaceSelection(L"");
                Sound::OnErase();
            } else if (g_caret < len) {
                g_anchor = ctrl ? NextWord(g_caret) : g_caret + 1;
                ReplaceSelection(L"");
                Sound::OnErase();
            }
            return true;
        case 'A':
            if (ctrl) {
                g_anchor = 0;
                g_caret = len;
                ResetBlink();
                MarkDirty();
                return true;
            }
            break;
        case 'C':
            if (ctrl) {
                CopySelection(false);
                return true;
            }
            break;
        case 'X':
            if (ctrl) {
                CopySelection(true);
                return true;
            }
            break;
        case 'V':
            if (ctrl) {
                PasteClipboard();
                return true;
            }
            break;
    }
    return false;
}

// Index of the result under a client point, or -1
static int HitRow(int mx, int my) {
    const float s = (float)g_scale;
    float cx = 0, cy = 0;
    MainOrigin(cx, cy);
    const float cw = kWidth * s;
    const int rows = VisibleRows();
    if (rows <= 0) return -1;
    if (mx < cx || mx > cx + cw) return -1;

    const float rowsOrigin = cy + (kBarH + kListPad + (float)HeroH()) * s;
    // The hit area is limited to what is really visible (rows area above the footer).
    const double hitH = std::min(ListRowsHeight(), ListViewportH());
    if (my < rowsOrigin || my >= rowsOrigin + (float)hitH * s) return -1;

    // With section headers the rows are not evenly spaced, so the row whose vertical range
    // contains the cursor is searched (headers are not selectable).
    const double scrollOff = ScrollPx();
    const int n = (int)g_results.size();
    const int first = std::max(0, (int)std::floor(g_scrollPos) - (g_kbScroll ? 2 : 0));
    const int last = std::min(n - 1, (int)std::ceil(g_scrollPos) + rows);
    for (int i = first; i <= last; ++i) {
        const float y0 = rowsOrigin + (float)(RowOffset((double)i) - scrollOff) * s;
        if (my >= y0 && my < y0 + kRowH * s) return i;
    }
    return -1;
}

// ===== Window =====

static bool RegisterLauncherHotkey() {
    UnregisterHotKey(g_hwnd, HOTKEY_ID);
    if (!RegisterHotKey(g_hwnd, HOTKEY_ID, g_cfg.hotkeyMods | MOD_NOREPEAT, g_cfg.hotkeyVk)) {
        return false;
    }
    return true;
}

static volatile LONG g_hotkeyWarnUp = 0;

static DWORD WINAPI HotkeyWarningThread(LPVOID) {
    MessageBoxW(nullptr,
                L"Spotlight Launcher could not register the keyboard shortcut because another "
                L"application is already using it (for example, Alt + Space is also the system "
                L"shortcut for a window's menu).\n\n"
                L"Change the shortcut in the mod settings or free that shortcut in the other "
                L"application.",
                L"Spotlight Launcher", MB_OK | MB_ICONWARNING | MB_TOPMOST | MB_SETFOREGROUND);
    InterlockedExchange(&g_hotkeyWarnUp, 0);
    return 0;
}

static void RegisterHotkeyOrWarn() {
    if (!RegisterLauncherHotkey()) {
        Sleep(300);
        if (RegisterLauncherHotkey()) return;
        // User-triggered settings changes can call this repeatedly: keep a single warning visible
        // instead of stacking one box per change.
        if (InterlockedCompareExchange(&g_hotkeyWarnUp, 1, 0) == 0) {
            HANDLE th = CreateThread(nullptr, 0, HotkeyWarningThread, nullptr, 0, nullptr);
            if (th) CloseHandle(th);
        }
    }
}

static LRESULT CALLBACK LauncherWndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_HOTKEY:
            if (wp == HOTKEY_ID) {
                // Respect the "Disable in fullscreen" toggle: while a fullscreen window is
                // focused, the shortcut does not open the search bar. Closing is still
                // allowed (it is already open, so the toggle only makes sense for opening).
                if (g_cfg.disableFullscreen && g_vis == Vis::Hidden && IsForegroundFullscreen()) {
                    return 0;
                }
                ToggleLauncher();
            }
            return 0;

        case WM_TIMER:
            if (wp == TIMER_FRAME) {
                OnFrame();
            } else if (wp == TIMER_COPY) {
                KillTimer(g_hwnd, TIMER_COPY);
                MarkDirty();  // redraw without the "Copied" flash
            } else if (wp == TIMER_CALC) {
                KillTimer(g_hwnd, TIMER_CALC);
                g_calcPending = false;
                g_calcHistSel = -1;
                g_histScroll = 0;
                UpdateCalc();
                RunSearch();
                UpdateTargets(false, /*followSelection=*/true);
                MarkDirty();
            } else if (wp == TIMER_BLINK) {
                if (g_vis == Vis::Shown && GetForegroundWindow() != g_hwnd &&
                    NowSec() - g_shownAt > 0.5) {
                    HideLauncher();  // focus was lost without WM_ACTIVATE arriving
                } else if (g_vis != Vis::Hidden) {
                    if (g_cfg.disableFullscreen && g_vis == Vis::Shown && IsForegroundFullscreen()) {
                        HideLauncher();
                    } else {
                        g_caretOn = !g_caretOn;
                        MarkDirty();
                    }
                }
            }
            return 0;

        case WM_ACTIVATE:
            if (LOWORD(wp) == WA_INACTIVE && g_vis != Vis::Hidden &&
                NowSec() - g_shownAt > 0.15) {
                HideLauncher();
            } else if (LOWORD(wp) == WA_ACTIVE && g_cfg.disableFullscreen &&
                       g_vis != Vis::Hidden && IsForegroundFullscreen()) {
                HideLauncher();
            }
            return 0;

        case WM_KEYDOWN:
            if (HandleKeyDown(wp)) return 0;
            break;

        case WM_CHAR: {
            wchar_t c = (wchar_t)wp;
            if (c == L' ' && CtrlDown()) return 0;  // Ctrl+Space (panel shortcut) does not type
            if (c >= 32 && c != 127) {
                ReplaceSelection(std::wstring(1, c));
                Sound::OnKey(c);
            }
            return 0;
        }

        // Alt held after the shortcut: do not open system menus or beep
        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        case WM_SYSCHAR:
            return 0;
        case WM_SYSCOMMAND:
            if ((wp & 0xFFF0) == SC_KEYMENU) return 0;
            break;

        case WM_MOUSEMOVE: {
            POINT p = {GET_X_LPARAM(lp), GET_Y_LPARAM(lp)};
            if (p.x != g_lastMouse.x || p.y != g_lastMouse.y) {
                bool first = g_lastMouse.x == -1 && g_lastMouse.y == -1;
                g_lastMouse = p;
                if (!first) {
                    if (CalcHistoryVisible()) {
                        const int h = HitCalcHistory(p.x, p.y);
                        if (h != g_calcHistSel) {
                            g_calcHistSel = h;
                            MarkDirty();
                        }
                    } else {
                        int row = HitRow(p.x, p.y);
                        if (row >= 0 && row != g_sel) {
                            // Hover selects, and the view scrolls to keep the selected row
                            // visible with one row of breathing room.
                            g_sel = row;
                            UpdateTargets(false, /*followSelection=*/true);
                            MarkDirty();
                        }
                    }
                }
            }
            const int act = HitAction(p.x, p.y);
            const bool overReindex = HitReindex(p.x, p.y);
            if (act != g_hoverAction || overReindex != g_hoverReindex) {
                g_hoverAction = act;
                g_hoverReindex = overReindex;
                MarkDirty();
            }
            return 0;
        }

        case WM_MOUSEWHEEL: {
            // The wheel scrolls the list; it does not change the selected app. The
            // selection stays where it is (or follows the mouse hover, which is handled in
            // WM_MOUSEMOVE).
            const int delta = GET_WHEEL_DELTA_WPARAM(wp);
            if (CalcHistoryVisible()) {
                const double step = (delta > 0 ? -3.0 : 3.0) * (double)kHistRowH;
                g_histScroll = std::max(0.0, g_histScroll + step);
                ClampHistScroll();
                MarkDirty();
                return 0;
            }
            const int rows = VisibleRows();
            const int n = (int)g_results.size();
            if (rows > 0 && n > 0) {
                const double step = (delta > 0 ? -3.0 : 3.0);
                double maxScroll = std::max(0, n - rows);
                g_kbScroll = false;  // wheel: back to row-based scroll
                g_scrollTarget = std::clamp(std::floor(g_scrollTarget + 0.5) + step, 0.0, maxScroll);
                MarkDirty();
            }
            return 0;
        }

        case WM_LBUTTONDOWN: {
            if (HitReindex(GET_X_LPARAM(lp), GET_Y_LPARAM(lp))) {
                ForceReindex();
                return 0;
            }
            int act = HitAction(GET_X_LPARAM(lp), GET_Y_LPARAM(lp));
            if (act >= 0) {
                RunActionOnSelection(act);
                return 0;
            }
            if (CalcHistoryVisible()) {
                const int h = HitCalcHistory(GET_X_LPARAM(lp), GET_Y_LPARAM(lp));
                if (h >= 0) {
                    InsertCalcHistory(h);
                }
                return 0;
            }
            int row = HitRow(GET_X_LPARAM(lp), GET_Y_LPARAM(lp));
            if (row >= 0) {
                g_sel = row;
                UpdateTargets(false, /*followSelection=*/true);
                LaunchSelected();
            }
            return 0;
        }

        case WM_MOUSEACTIVATE:
            return MA_ACTIVATE;

        case WM_ERASEBKGND:
            return 1;

        case WM_APP_INDEX: {
            auto* r = (IndexResult*)lp;
            if (r->gen == g_gen) ApplyIndex(*r);
            delete r;
            return 0;
        }
        case WM_APP_ICON: {
            auto* r = (IconResult*)lp;
            if (r->gen == g_gen && r->index < g_apps.size()) {
                AppEntry& a = g_apps[r->index];
                SafeRelease(a.bmp);
                a.iconW = r->w;
                a.iconH = r->h;
                a.iconPx = std::move(r->px);
                if (g_vis != Vis::Hidden) MarkDirty();
            }
            delete r;
            return 0;
        }
        case WM_APP_INDEX_DONE:
            if ((UINT)wp == g_gen) {
                g_indexing = false;
                g_indexedOnce = true;
                if (g_vis != Vis::Hidden) {
                    UpdateTargets(false, /*followSelection=*/true);
                    MarkDirty();
                }
                MaybeReindexInBackground();  // changes that arrived while indexing
            }
            return 0;

        case WM_APP_MENU_CHANGED:
            g_menuDirty = true;
            MaybeReindexInBackground();
            return 0;

        case WM_APP_SETTINGS: {
            const bool oldHideUtility = g_cfg.hideUtility;
            LoadSettings();
            Sound::Configure();
            Sound::ApplyVolume();
            if (g_cfg.hideUtility != oldHideUtility) {
                g_menuDirty = true;  // the index must be rebuilt to apply this
            }
            RegisterHotkeyOrWarn();
            if (g_vis != Vis::Hidden) FinishHide();
            MaybeReindexInBackground();  // e.g. if switched to "Automatic" with pending changes
            return 0;
        }

        case WM_DESTROY:
            UnregisterHotKey(hwnd, HOTKEY_ID);
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

static DWORD WINAPI UiThreadProc(LPVOID) {
    // Per-monitor DPI awareness for this thread (before creating the window)
    {
        typedef HANDLE(WINAPI * SetThreadDpiAwarenessContext_t)(HANDLE);
        HMODULE u = GetModuleHandleW(L"user32.dll");
        auto fn = u ? (SetThreadDpiAwarenessContext_t)GetProcAddress(u, "SetThreadDpiAwarenessContext")
                    : nullptr;
        if (fn) {
            fn((HANDLE)(INT_PTR)-4);  // PER_MONITOR_AWARE_V2
        }
    }

    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    LoadSettings();
    LoadCalcHistory();
    LoadCalcAns();
    LoadPalette();
    if (g_cfg.sounds) Sound::Init();
    Sound::ApplyVolume();

    HINSTANCE hInst = GetModuleHandleW(nullptr);
    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = LauncherWndProc;
    wc.hInstance = hInst;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.lpszClassName = L"SpotlightLauncherWindow";
    RegisterClassExW(&wc);

    g_hwnd = CreateWindowExW(WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST,
                             wc.lpszClassName, L"Spotlight Launcher", WS_POPUP, 0, 0, 100, 100,
                             nullptr, nullptr, hInst, nullptr);
    if (!g_hwnd) {
        return 1;
    }
    LiveCreate(hInst);

    D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory), nullptr,
                      (void**)&g_d2d);
    DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), (IUnknown**)&g_dw);
    if (g_d2d) {
        g_d2d->CreateStrokeStyle(D2D1::StrokeStyleProperties(D2D1_CAP_STYLE_ROUND,
                                                             D2D1_CAP_STYLE_ROUND,
                                                             D2D1_CAP_STYLE_ROUND,
                                                             D2D1_LINE_JOIN_ROUND),
                                 nullptr, 0, &g_roundStroke);
    }

    RegisterHotkeyOrWarn();
    StartIndex();
    StartWatcher();  // always runs (it is almost free); only used in Automatic mode

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    StopWatcher();
    UnregisterHotKey(g_hwnd, HOTKEY_ID);
    Sound::Shutdown();
    LiveDestroy();
    DestroyWindow(g_hwnd);
    g_hwnd = nullptr;
    ReleaseTarget();
    SafeRelease(g_roundStroke);
    ReleaseFormats();
    SafeRelease(g_dw);
    SafeRelease(g_d2d);
    DestroySurface();
    if (g_memDC) {
        DeleteDC(g_memDC);
        g_memDC = nullptr;
    }
    CoUninitialize();
    return 0;
}

// ===== Tool mod callbacks =====

BOOL WhTool_ModInit() {
    g_uiThread = CreateThread(nullptr, 0, UiThreadProc, nullptr, 0, &g_uiThreadId);
    return g_uiThread != nullptr;
}

void WhTool_ModSettingsChanged() {
    if (g_hwnd) {
        PostMessageW(g_hwnd, WM_APP_SETTINGS, 0, 0);
    }
}

void WhTool_ModUninit() {
    if (g_uiThread) {
        PostThreadMessageW(g_uiThreadId, WM_QUIT, 0, 0);
        WaitForSingleObject(g_uiThread, 3000);
        CloseHandle(g_uiThread);
        g_uiThread = nullptr;
    }
}

////////////////////////////////////////////////////////////////////////////////
// Windhawk tool mod implementation for mods which don't need to inject to other
// processes or hook other functions. Context:
// https://github.com/ramensoftware/windhawk/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process
//
// The mod will load and run in a dedicated windhawk.exe process.
//
// Paste the code below as part of the mod code, and use these callbacks:
// * WhTool_ModInit
// * WhTool_ModSettingsChanged
// * WhTool_ModUninit
//
// Currently, other callbacks are not supported.

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

BOOL Wh_ModInit() {
    DWORD sessionId;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) &&
        sessionId == 0) {
        return FALSE;
    }

    bool isExcluded = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
    if (!argv) {
        Wh_Log(L"CommandLineToArgvW failed");
        return FALSE;
    }

    for (int i = 1; i < argc; i++) {
        if (wcscmp(argv[i], L"-service") == 0 ||
            wcscmp(argv[i], L"-service-start") == 0 ||
            wcscmp(argv[i], L"-service-stop") == 0) {
            isExcluded = true;
            break;
        }
    }

    for (int i = 1; i < argc - 1; i++) {
        if (wcscmp(argv[i], L"-tool-mod") == 0) {
            isToolModProcess = true;
            if (wcscmp(argv[i + 1], WH_MOD_ID) == 0) {
                isCurrentToolModProcess = true;
            }
            break;
        }
    }

    LocalFree(argv);

    if (isExcluded) {
        return FALSE;
    }

    if (isCurrentToolModProcess) {
        g_toolModProcessMutex =
            CreateMutex(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
        if (!g_toolModProcessMutex) {
            Wh_Log(L"CreateMutex failed");
            ExitProcess(1);
        }

        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            Wh_Log(L"Tool mod already running (%s)", WH_MOD_ID);
            ExitProcess(1);
        }

        if (!WhTool_ModInit()) {
            ExitProcess(1);
        }

        IMAGE_DOS_HEADER* dosHeader =
            (IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);
        IMAGE_NT_HEADERS* ntHeaders =
            (IMAGE_NT_HEADERS*)((BYTE*)dosHeader + dosHeader->e_lfanew);

        DWORD entryPointRVA = ntHeaders->OptionalHeader.AddressOfEntryPoint;
        void* entryPoint = (BYTE*)dosHeader + entryPointRVA;

        Wh_SetFunctionHook(entryPoint, (void*)EntryPoint_Hook, nullptr);
        return TRUE;
    }

    if (isToolModProcess) {
        return FALSE;
    }

    g_isToolModProcessLauncher = true;
    return TRUE;
}

void Wh_ModAfterInit() {
    if (!g_isToolModProcessLauncher) {
        return;
    }

    WCHAR currentProcessPath[MAX_PATH];
    switch (GetModuleFileName(nullptr, currentProcessPath,
                              ARRAYSIZE(currentProcessPath))) {
        case 0:
        case ARRAYSIZE(currentProcessPath):
            Wh_Log(L"GetModuleFileName failed");
            return;
    }

    WCHAR
    commandLine[MAX_PATH + 2 +
                (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) - 1];
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", currentProcessPath,
               WH_MOD_ID);

    HMODULE kernelModule = GetModuleHandle(L"kernelbase.dll");
    if (!kernelModule) {
        kernelModule = GetModuleHandle(L"kernel32.dll");
        if (!kernelModule) {
            Wh_Log(L"No kernelbase.dll/kernel32.dll");
            return;
        }
    }

    using CreateProcessInternalW_t = BOOL(WINAPI*)(
        HANDLE hUserToken, LPCWSTR lpApplicationName, LPWSTR lpCommandLine,
        LPSECURITY_ATTRIBUTES lpProcessAttributes,
        LPSECURITY_ATTRIBUTES lpThreadAttributes, WINBOOL bInheritHandles,
        DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory,
        LPSTARTUPINFOW lpStartupInfo,
        LPPROCESS_INFORMATION lpProcessInformation,
        PHANDLE hRestrictedUserToken);
    CreateProcessInternalW_t pCreateProcessInternalW =
        (CreateProcessInternalW_t)GetProcAddress(kernelModule,
                                                 "CreateProcessInternalW");
    if (!pCreateProcessInternalW) {
        Wh_Log(L"No CreateProcessInternalW");
        return;
    }

    STARTUPINFO si{
        .cb = sizeof(STARTUPINFO),
        .dwFlags = STARTF_FORCEOFFFEEDBACK,
    };
    PROCESS_INFORMATION pi;
    if (!pCreateProcessInternalW(nullptr, currentProcessPath, commandLine,
                                 nullptr, nullptr, FALSE, NORMAL_PRIORITY_CLASS,
                                 nullptr, nullptr, &si, &pi, nullptr)) {
        Wh_Log(L"CreateProcess failed");
        return;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

void Wh_ModSettingsChanged() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModSettingsChanged();
}

void Wh_ModUninit() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModUninit();
    ExitProcess(0);
}
