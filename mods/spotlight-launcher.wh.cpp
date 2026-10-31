// ==WindhawkMod==
// @id              spotlight-launcher
// @name            Spotlight Launcher
// @description     Application launcher. Alt+Space opens a floating search bar that finds your programs instantly.
// @version         1.1.0
// @author          David Ravelo
// @github          https://github.com/DavidRaveloU
// @include         windhawk.exe
// @compilerOptions -lole32 -lshell32 -luser32 -lgdi32 -ld2d1 -ldwrite -luuid -ladvapi32 -lwindowscodecs -lpropsys -lversion -lcrypt32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Spotlight Launcher

A fast, keyboard-driven launcher for Windows, packaged as a [Windhawk](https://windhawk.net) tool mod.

Press **Alt + Space**, type a few letters, press **Enter**. That is the whole idea.

Spotlight Launcher indexes the same applications you see in the Start menu — classic programs and
Microsoft Store apps — and filters them as you type. From there it reaches further: files and folders,
the clipboard, browser bookmarks, Windows' recent files, open windows, a calculator and a set of
built-in commands. It runs as its own Windhawk **tool** mod, so it never runs inside File Explorer and
cannot affect the way Windows works.

![Spotlight Launcher in action](https://raw.githubusercontent.com/DavidRaveloU/Spotlight-Launcher-Windhawk/main/assets/Desktop.png)

## Highlights

- **Instant, forgiving search.** Case- and accent-insensitive, multi-word and acronym matching, with the
  letters that matched highlighted. Results are ranked by match quality, then by how often and how
  recently you use them, and an app you pick after typing a few words comes first next time.
- **Files, without a second window.** Browse your drives and folders in the bar: locations and drive
  chips, sorting, previews, and rename, copy, terminal and Recycle Bin actions. Narrow the list with
  `*.pdf`, `type:image`, `dir:`, and by size and date too: `size:>10mb`, `modified:today`.
- **Global file search.** `find:` searches file and folder names under Home and any extra folders, with
  no index to build, using the same filters: `find: report size:>1mb modified:week`. Point it at
  **Everything** (optional) and the same queries are answered from its index in milliseconds, with the
  size and date of every file.
- **A window switcher you can act on.** `win:` lists your open windows and **Enter** brings one to the
  front, but it also **closes**, **minimizes**, **pins on top** or **moves to another monitor** the
  selected window without switching to it. A panel shows its process, state and path, and a pinned
  window wears a gold pin so a forgotten toggle is easy to spot.
- **Clipboard history, opt-in and encrypted.** Off by default. When it is on, it keeps the last 30
  things you copied, text and images, and `clip:` searches them. Everything is encrypted for your
  Windows account, copies an app marks as private are skipped, and turning the feature off erases what
  it saved.
- **Browser bookmarks, off by default.** Chrome, Edge, Brave, Vivaldi, Chromium, Opera, Firefox,
  Waterfox, LibreWolf, Zen and Floorp, read locally from the bookmark files — `bm:` searches only
  bookmarks.
- **Recent files.** The empty bar shows the files and folders from Windows' Recent list, and `recent:`
  searches them.
- **Custom entries.** Your own sites, folders and commands, with placeholders that ask for the values
  they need, plus an optional typed trigger or in-launcher shortcut per entry.
- **A calculator, commands and settings pages.** Type a calculation and the result appears as you go;
  `>` reaches lock, sleep, Task Manager, Settings pages and more, destructive ones asking for a second
  Enter.
- **Built to look right.** Six effects, twelve colours, live blur, adjustable opacity and position, and
  soft interface sounds generated inside the mod.

![The details panel](https://raw.githubusercontent.com/DavidRaveloU/Spotlight-Launcher-Windhawk/main/assets/Details.png)

**Before you install:** clipboard history is opt-in and encrypted for your Windows account; browser
bookmarks are off by default; Everything is optional, the built-in scan is the default; and `recent:`
needs Windows to be recording recent items, or its list stays empty.

## Install

1. Install [Windhawk](https://windhawk.net) (Windows 10 or 11).
2. In Windhawk, open **Mods**, search for **Spotlight Launcher** and click **Install**.
3. Press **Alt + Space** to open it.

From source: download `spotlight-launcher.wh.cpp`, choose **Create a new mod** in Windhawk, paste it in
place of the editor contents and save.

## The first keys

| Key | Action |
| --- | --- |
| **Alt + Space** | Open and close the bar (configurable) |
| **↑ / ↓**, **Enter** | Move the selection, open it |
| **Tab** | Switch between apps and Files |
| **Ctrl + I** | Actions and details panel |
| **F1** | The cheat sheet: every shortcut, prefix and filter |

## Documentation

The full manual lives in the repository: installation, every prefix and command, the file browser and
its filters, the clipboard, custom entries, the full calculator syntax, the window actions, every
setting and troubleshooting.

**[Read the documentation →](https://github.com/DavidRaveloU/Spotlight-Launcher-Windhawk/blob/main/docs/index.md)**

## Contributing

Bug reports, fixes, new features, translations and documentation corrections are welcome. Open an
[issue](https://github.com/DavidRaveloU/Spotlight-Launcher-Windhawk/issues) describing what happened,
your Windows build and the mod version.

## License

Released under the **MIT License**. See
[LICENSE](https://github.com/DavidRaveloU/Spotlight-Launcher-Windhawk/blob/main/LICENSE).

Copyright (c) 2026-present David Ravelo.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- Appearance:
  - ListMode: expanded
    $name: List mode
    $description: Compact shows only the search bar until you type. Expanded also shows your recent apps and suggestions while the search box is empty.
    $options:
      - compact: Compact
      - expanded: Expanded
  - VerticalPosition: 30
    $name: Vertical position (percentage of screen height)
    $description: Where the search bar sits, as a percentage of the screen height. 0 sticks it to the very top, 50 centres it and 100 sits it just above the taskbar. In Compact mode only the search box appears at this height; in Expanded mode the bar grows downward from here, so raise or lower this value if you want it to look centred.
  - Opacity: 90
    $name: Panel opacity (percentage)
    $description: How solid the colour of the bar is, from 70 to 100. A lower value lets more of the background show through and a higher value makes the colour stronger (Glass stays see-through in any case). Ignored with "Solid", which is always fully opaque.
  - Effect: acrylic
    $name: Background effect
    $description: The background style of the bar. Acrylic, Mica, Glass, Frosted and Aurora show a blurred view of whatever is behind the bar, each with its own look; "Solid" shows a plain colour with no blur. The blurred effects rely on Windows' "Transparency effects" (Settings > Personalization > Colors); with that turned off, Windows draws no blur and these effects fall back to a flat colour. Only "Solid" is unaffected.
    $options:
      - acrylic: Acrylic
      - mica: Mica
      - glass: Glass
      - frosted: Frosted
      - aurora: Aurora
      - solid: Solid (no effect)
  - LiveAcrylic: true
    $name: Live acrylic (real time)
    $description: Keeps the background blur live. Acrylic, Glass, Frosted and Aurora follow whatever is behind the bar in real time. Turn it off to use a single snapshot taken when the bar opens, which helps performance and saves battery on laptops and low-end machines. With it on, the blur strength of those effects is fixed by Windows and the Blur strength setting has no effect on them (Mica always follows it). Needs Windows 10 1803 or newer.
  - BlurAmount: 50
    $name: Blur strength (0 to 100)
    $description: How strong the blur is. 50 is the normal amount, lower values give a subtler blur and higher values a heavier one. The blur is seen through the colour, so it shows best with a Panel opacity below 100. Applies to Mica, and to the other effects only while Live acrylic is off (with it on, Windows sets their blur strength). Ignored with "Solid".
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
  $name: Appearance
- Behavior:
  - OpenOn: cursor
    $name: Open on monitor
    $description: Which monitor the search bar appears on. "Monitor with the mouse" follows the cursor, "Monitor of the active window" uses the monitor of the window you were using, and "Primary monitor" always uses the main monitor.
    $options:
      - cursor: Monitor with the mouse
      - foreground: Monitor of the active window
      - primary: Primary monitor
  - MaxResults: 8
    $name: Maximum results
    $description: How many apps the search shows at once. It does not change the height of the bar; when there are more matches, you reach the rest by scrolling the list.
  - DisableInFullscreen: true
    $name: Disable in fullscreen
    $description: When on, the shortcut is ignored while a fullscreen window is focused (a game, a video player, a fullscreen browser). It also closes the bar if a fullscreen window takes over while the bar is open.
  $name: Behavior
- Shortcuts:
  - Hotkey: alt+space
    $name: Keyboard shortcut
    $description: Key combination that opens and closes the search bar.
    $options:
      - alt+space: Alt + Space
      - ctrl+space: Ctrl + Space
      - ctrl+alt+space: Ctrl + Alt + Space
      - shift+alt+space: Shift + Alt + Space
      - ctrl+shift+space: Ctrl + Shift + Space
  - DetailsKey: ctrl+i
    $name: Details panel shortcut
    $description: Key that shows or hides the actions and details panel for the selected app. If you pick Ctrl + Space here, choose a different combination for the main shortcut so the two do not clash.
    $options:
      - ctrl+i: Ctrl + I
      - ctrl+k: Ctrl + K
      - ctrl+space: Ctrl + Space
      - f4: F4
  $name: Shortcuts
- Apps:
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
  $name: Apps
- Search:
  - WebSearch: true
    $name: Web search
    $description: Shows a result at the end of the list for what you typed. Press Enter to open it. A web address (like gmail.com) opens the site directly; prefix with ? to search instead. It is never offered while the text is being used as a calculation.
  - WebEngine: google
    $name: Web search engine
    $description: Which search engine the web result opens. Ignored while web search is off.
    $options:
      - google: Google
      - bing: Bing
      - duckduckgo: DuckDuckGo
      - youtube: YouTube
      - wikipedia: Wikipedia
  - FileSearchEngine: builtin
    $name: File search engine
    $description: How file searching works, for both the "find:" prefix and the deep search inside Files. Built-in scan walks your Home folder (and the extra folders, or the folder you are browsing) as you type, with no index and no extra software. Everything (voidtools) asks the Everything index instead, which is much faster and covers every drive and folder it has indexed, and understands Everything's own search syntax (wildcards, quotes, excluded words). Everything must be installed and running; when it is not, the built-in scan is used automatically and the footer says so.
    $options:
      - builtin: Built-in scan
      - everything: Everything (voidtools)
  - SearchRoots: ""
    $name: Extra search folders
    $description: Folders searched by the "find:" prefix, separated by semicolons (for example D:\Projects; C:\Tools). Environment variables such as %USERPROFILE% work. Your Home folder is always searched, so Documents, Downloads, Desktop, Pictures and the rest are included; extra folders already inside Home are skipped. Leave empty to search only Home. Not used while the file search engine is Everything.
  - LearnFromChoices: true
    $name: Learn from your choices
    $description: When you pick an app after typing a few words, the launcher remembers that choice for those words and offers the app first the next time you type them. It only reorders apps that already match equally well, the pairs are stored locally per user, and one you stop picking is forgotten after 90 days.
  - Bookmarks: false
    $name: Browser bookmarks
    $description: Searches your bookmarks from Chrome, Edge, Brave, Vivaldi, Chromium, Opera, Firefox (and its Waterfox, LibreWolf, Zen and Floorp forks) next to your apps, and adds a "bm:" prefix that searches only bookmarks. Only the browsers' local bookmark files and profile names are read; the mod never touches history, cookies, passwords or the network. Bookmark changes are picked up the next time the bar opens, and Firefox bookmarks follow Firefox's own automatic backups.
  - RecentFiles: true
    $name: Recent files
    $description: Shows files and folders from Windows' Recent list on the empty bar and adds a "recent:" prefix. Windows must be recording recent items. Turn on Settings > Personalization > Start > "Show recently opened items in Start, Jump Lists, and File Explorer" (Windows 11; the equivalent "Show recently used files" option on Windows 10). With that off, Windows keeps the Recent folder empty and this feature has nothing to show. Read locally from the Recent folder; no network and no disk scan. Turn it off to hide the section and the prefix.
  $name: Search
- Files:
  - FilesEnabled: true
    $name: File browser
    $description: Adds the Files mode, which browses drives and folders, previews images and manages files. Press Tab or the folder button in the bar to switch between apps and files. When off, the folder button is hidden and Tab moves through the app list. A chip is shown for each of your drives after the Home, Downloads, Documents, Pictures, Music and Videos locations; Ctrl + Tab (Ctrl + Shift + Tab) cycles through the drives and Ctrl + 1…9 jumps to a chip.
  - StartDrive: ""
    $name: Start drive
    $description: Which drive the file browser opens on the first time you use it in a session, as a single letter (for example D or E). Leave it empty to open on Home (your user folder), which is also where it falls back if the drive is not available.
  - RememberFileSort: false
    $name: Remember file sort
    $description: Keeps the Files order you pick (name, type, modified date or size) across closes and restarts. Off by default, so the order you pick only lasts while the bar is open and Files starts at Name (A-Z) again the next time; the sort menu is always available with Ctrl + S (or Sort in the actions menu).
  $name: Files
- Clipboard:
  - ClipboardHistory: false
    $name: Clipboard history
    $description: Keeps a history of what you copy and adds a "clip:" prefix that searches it. Text and images are both supported. Enter pastes an entry into the window you came from, Ctrl+Enter copies it, Ctrl+D pins or unpins it, Ctrl+Shift+R removes it, and the "Clear Clipboard History" command erases everything. Everything is encrypted for your Windows account (DPAPI) before it is written to the mod's storage, images included, and turning the feature off erases what was saved. Copies that an app marks as private (password managers do) are skipped. Off by default and never sent anywhere.
  - ClipRetentionDays: 3
    $name: Clipboard retention (days)
    $description: Clipboard entries older than this many days are deleted automatically, images and their saved files included. Pinned entries are kept. Use 0 to keep entries until the 30-entry limit removes them. Only used while Clipboard history is enabled.
  $name: Clipboard
- Calculator:
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
  $name: Calculator
- Sounds:
  - SoundEnabled: true
    $name: Interface sounds
    $description: Soft sounds when the bar opens, when the details panel opens or closes, and as you type. They are generated inside the mod, so they work with no internet connection.
  - SoundVolume: 20
    $name: Sound volume (0 to 100)
    $description: How loud the interface sounds are. Ignored while interface sounds are turned off.
  $name: Sounds
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <winver.h>
#include <wincrypt.h>
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
#include <atomic>
#include <cmath>
#include <cstdint>
#include <cwctype>
#include <deque>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <cwchar>
#include <cstring>

#ifndef LOAD_LIBRARY_SEARCH_SYSTEM32
#define LOAD_LIBRARY_SEARCH_SYSTEM32 0x00000800
#endif

#ifndef WM_CLIPBOARDUPDATE
#define WM_CLIPBOARDUPDATE 0x031D
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
static const float kChipH = 44.f;      // file-mode filter band (location chips), first row
static const float kDriveRowH = 34.f;  // second chip row (drive chips), only shown with 2+ drives
static const float kPathH = 30.f;      // file-mode path band (breadcrumb)

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

// Custom-entry form (replaces the results list while it is open)
static const float kFormRowH = 42.f;
static const int kFormRows = 6;  // fields visible at once; the rest scroll with Tab / arrows

// List with an empty search bar (Expanded mode)
static const size_t kMaxRecents = 4;
static const size_t kMaxRecentFiles = 4;  // files shown on the empty bar; "recent:" lists more

static const wchar_t kTxtPlaceholder[] = L"Search applications…";
static const wchar_t kTxtPlaceholderFiles[] = L"Search files…  (try *.pdf or dir:)";
static const wchar_t kTxtFilesMode[] = L"Files";
// Sentinel "folder" for the drives overview (reached by going up from a drive root). It is never a
// real path, so folder walking and editing must treat it specially (see InPcRoot).
static const wchar_t kPcRoot[] = L"::ThisPC";
static const wchar_t kTxtThisPc[] = L"This PC";
static const wchar_t kTxtFolder[] = L"Folder";
static const wchar_t kTxtOpen[] = L"Open";
static const wchar_t kTxtRun[] = L"Run";
static const wchar_t kTxtConfirm[] = L"Press Enter again to confirm";
static const wchar_t kTxtSearchLabel[] = L"Search";
static const wchar_t kTxtEnterGlyph[] = L"↵";
static const wchar_t kTxtNoResults[] = L"No results";
static const wchar_t kTxtIndexing[] = L"Indexing applications…";
static const wchar_t kTxtRecents[] = L"Recent";
static const wchar_t kTxtRecentFiles[] = L"Recent files";
static const wchar_t kTxtSuggestions[] = L"Suggestions";
static const wchar_t kTxtPinned[] = L"Pinned";
static const wchar_t kTxtActions[] = L"Actions";
static const wchar_t kTxtInfo[] = L"Information";
static const wchar_t kTxtDetails[] = L"Details";
static const wchar_t kTxtReindex[] = L"Reindex";
static const wchar_t* const kReindexKeys[2] = {L"Ctrl", L"R"};
static const wchar_t kTxtHelp[] = L"Help";
static const wchar_t* const kHelpKeys[1] = {L"F1"};
static const wchar_t kTxtTypeApp[] = L"Application";
static const wchar_t kTxtTypeStore[] = L"Store app";
static const wchar_t kTxtKindWeb[] = L"Web";
static const wchar_t kTxtKindUrl[] = L"Website";
static const wchar_t kTxtKindCommand[] = L"Command";
static const wchar_t kTxtKindFile[] = L"File";
static const wchar_t kTxtBrowse[] = L"Browse";
static const wchar_t kTxtSearching[] = L"Searching…";
static const wchar_t kTxtKindSetting[] = L"Setting";
static const wchar_t kTxtKindCustom[] = L"Custom entry";
static const wchar_t kTxtKindBookmark[] = L"Bookmark";
static const wchar_t kTxtKindClip[] = L"Clipboard";
static const wchar_t kTxtKindRecent[] = L"Recent file";
static const wchar_t kTxtKindWindow[] = L"Window";
static const wchar_t kTxtFormOptional[] = L"Optional";
static const wchar_t kTxtFormRequired[] = L"Required";
static const wchar_t kTxtFormNext[] = L"Next field";
static const wchar_t kTxtFormBack[] = L"Back";
static const wchar_t kTxtFormWillOpen[] = L"Will open";
static const wchar_t kTxtFormWillRun[] = L"Will run";
static const wchar_t kTxtFormFill[] = L"Fill in the required fields";
static const wchar_t kTxtFormEdit[] = L"Edit values";
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
static const UINT WM_APP_FILES = WM_APP + 6;         // deep file search finished
static const UINT WM_APP_THUMB = WM_APP + 7;         // a background image preview was decoded
static const UINT WM_APP_BOOKMARKS = WM_APP + 8;     // a background bookmark load finished
static const UINT WM_APP_RECENT = WM_APP + 9;        // a background recent-files load finished
static const UINT WM_APP_GLOBAL = WM_APP + 10;       // a partial/final global file search arrived
static const UINT WM_APP_EVERYTHING = WM_APP + 11;   // Everything answered a "find:" query

// Start menu reindexing
static const double kAutoSafetyRescanSec = 600.0;  // fallback in Automatic mode (MSIX apps)
static const DWORD kWatchDebounceMs = 1500;        // wait for the installer to finish

static const double kPi = 3.14159265358979323846;

static const UINT_PTR TIMER_FRAME = 1;
static const UINT_PTR TIMER_BLINK = 2;
static const UINT_PTR TIMER_COPY = 3;  // clears the calculator "Copied" flash
static const UINT_PTR TIMER_CALC = 4;  // debounce for calculator evaluation
static const UINT_PTR TIMER_CONFIRM = 5;  // cancels an armed destructive command after a while
static const UINT_PTR TIMER_FSEARCH = 6;  // debounce for the deep file search
static const UINT_PTR TIMER_HIDE = 7;     // fallback: completes a hide whose animation stalled
static const UINT_PTR TIMER_GSEARCH = 8;  // debounce for the global "find:" search
static const UINT_PTR TIMER_EVFALLBACK = 9;  // "find:" via Everything: fall back if it never answers
static const UINT_PTR TIMER_WINCLOSE = 10;   // re-read the window list while a closed window disappears
static const int HOTKEY_ID = 1;

// ===== Types =====

enum class ReindexMode { Auto, Always, Interval };
enum class ListMode { Compact, Expanded };
enum class DetailsKey { CtrlI, CtrlK, CtrlSpace, F4 };
enum class OpenMonitor { Cursor, Foreground, Primary };
enum class WebEngine { Google, Bing, DuckDuckGo, YouTube, Wikipedia };
enum class BgEffect { Acrylic, Mica, Glass, Frosted, Aurora, Solid };
enum class CalcStyle { Hero, Ghost, Both };

// A placeholder found in a custom entry's Target or Arguments ({folder}, {folder?}, {branch=main}).
struct CustomField {
    std::wstring key;    // as written in the settings; matched case-insensitively
    std::wstring label;  // friendly name shown in the form
    bool optional = false;
    std::wstring value;  // default text (may be empty)
};

// A user-defined entry (in-app manager, or the optional Commands list in the settings). Searched like
// an app and launched with the shell: URL opens in the browser, Path opens a file/folder, Run starts a
// program. Placeholders in Target/Arguments are filled by a small form; trigger/hotkey are optional
// shortcuts (a typed alias such as "yt cats" and a key combination used while the bar is open).
struct CustomEntry {
    std::wstring name;
    std::wstring folded;
    std::wstring type;  // "url", "path" or "run"
    std::wstring target;
    std::wstring args;
    std::wstring keywords;  // folded extra words that also match the entry
    std::wstring trigger;   // folded typed alias, matched against the first word of the query
    std::wstring hotkey;    // canonical global shortcut text ("ctrl+alt+t"), empty = none
    std::vector<CustomField> fields;  // placeholders, in the order they appear (Target, then Arguments)
    bool hasQuery = false;            // has a {query} placeholder: extra words typed after the name
    bool fromStore = false;           // created in the in-app manager (persisted store), not settings
    std::wstring id;                  // stable id for store entries (empty for settings entries)
};

// A bookmark from a Chromium-family browser. Only the name, the folder path and the URL are kept;
// nothing else is read from the browser profile.
struct BookmarkEntry {
    std::wstring name;
    std::wstring folded;   // folded name, for matching
    std::wstring url;
    std::wstring folder;   // "Bookmarks bar / Dev"
    std::wstring browser;  // "Chrome", "Edge", "Brave", ...
    std::wstring profile;  // profile display name, empty when unknown
    std::wstring tail;     // folded folder + host, matched with a lower score than the name
};

// Order of the Files listing. Folders always come first; this only decides the rest.
enum class FileSort {
    NameAsc,    // A-Z (the default)
    NameDesc,   // Z-A
    Type,       // by extension, then name
    ModNew,     // most recently modified first
    ModOld,     // oldest first
    SizeBig,    // largest first
    SizeSmall,  // smallest first
};
static const int kFileSortCount = 7;

enum class FileSearchEngine { Builtin, Everything };

struct LauncherSettings {
    ListMode listMode = ListMode::Expanded;
    DetailsKey detailsKey = DetailsKey::CtrlI;
    OpenMonitor openMonitor = OpenMonitor::Cursor;
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
    bool webSearch = true;   // offer a "search the web" result for the query
    WebEngine webEngine = WebEngine::Google;
    bool learnFromChoices = true;  // remember the app picked for a query and offer it first
    FileSearchEngine fileSearchEngine = FileSearchEngine::Builtin;  // "find:" backend
    bool filesEnabled = true;  // show the file browser mode (Tab and the folder button)
    FileSort fileSort = FileSort::NameAsc;  // Files order; restored at startup when remembered
    bool rememberFileSort = false;         // keep the chosen Files order across restarts
    bool bookmarks = false;    // search browser bookmarks and offer the "bm:" prefix
    bool clipboardHistory = false;  // keep a local clipboard history (text and images), "clip:"
    int clipRetentionDays = 3;      // drop clipboard entries older than this (0 = keep)
    bool recentFiles = true;        // show Windows' Recent files and offer "recent:"
    std::vector<std::wstring> searchRoots;  // extra folders for "find:" (Home is always searched)
    wchar_t startDrive = 0;    // drive letter the browser opens on first (0 = Home), see LoadSettings
    std::vector<CustomEntry> commands;  // user-defined entries (settings + the in-app manager)
};

struct AppEntry {
    std::wstring name;
    std::wstring folded;  // same length as name (folded 1:1)
    std::wstring id;      // AppsFolder parsing name, or the full path of a desktop shortcut
    // What the entry really starts. Empty for packaged apps and for shortcuts whose target the
    // shell cannot resolve. `launch` holds the .lnk/.exe itself for entries found on the Desktop,
    // which must be opened directly instead of through shell:AppsFolder.
    std::wstring target;
    std::wstring args;
    std::wstring launch;
    std::wstring usageKey;
    int uses = 0;
    int lastUsed = 0;  // seconds since 2020-01-01 (0 = never); see NowStamp()
    bool pinned = false;  // kept at the top of the home list and slightly boosted in search
    int iconW = 0;
    int iconH = 0;
    std::vector<uint32_t> iconPx;  // BGRA premultiplied
    ID2D1Bitmap* bmp = nullptr;
};

// Where a result comes from. Every kind is produced today: App for the app index, Web/Url for
// web search, Command for built-in commands, File for the file browser, Setting for Windows
// settings pages, Custom for the user's own entries, Bookmark for browser bookmarks and Clip for
// the clipboard history.
enum class ResultKind { App, Web, Url, Command, File, Setting, Custom, Bookmark, Clip, Recent, GlobalFile, Window };

struct SearchResult {
    ResultKind kind = ResultKind::App;
    int app = -1;     // ResultKind::App: index into g_apps
    int custom = -1;  // ResultKind::Custom: index into g_cfg.commands
    int score = 0;
    int section = 0;  // 0 = no section (search), 1 = Recent apps, 2 = Suggestions,
                      // 3 = Pinned apps, 4 = Recent files
    bool pinned = false;  // Clip: the entry is pinned (draws a small pin badge)
    bool image = false;   // Clip: the entry is an image, so the row shows its thumbnail
    bool isDir = false;   // Recent: the target is a folder
    std::wstring ext;     // Recent: uppercase extension for the file tile
    HWND hwnd = nullptr;  // Window: the top-level window to activate
    int iconApp = -1;     // Window: index into g_apps for the process icon, or -1
    bool onTop = false;   // Window: always on top (drawn as a badge on its icon)
    // Non-App results carry their own text and payload (URL, command, path, ...).
    std::wstring title;
    std::wstring subtitle;
    std::wstring payload;
    int icon = -1;  // provider-specific icon id (Web: the search engine, see WebEngine)
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
    std::wstring elevateTarget;  // packaged apps: the .exe "Run as administrator" would start
    std::vector<InfoRow> rows;
};

enum ActionId {
    ACT_OPEN,
    ACT_PIN,
    ACT_UNPIN,
    ACT_ADMIN,
    ACT_LOCATION,
    ACT_COPYPATH,
    ACT_PROPS,
    ACT_FORGET
};

struct ActionDef {
    int id;
    const wchar_t* label;
    const wchar_t* keys;
};

static const ActionDef kActions[] = {
    {ACT_OPEN, L"Open", L"↵"},
    {ACT_PIN, L"Pin to top", L"Ctrl+D"},
    {ACT_UNPIN, L"Unpin from top", L"Ctrl+D"},
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
    D2D1_COLOR_F accentInk;     // accent tuned for use as a foreground (text/icons) on the card
    D2D1_COLOR_F accentStrong;  // deeper accent for emphasized text (active chips)
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
    std::wstring target;  // program the entry really starts (empty when unknown)
    std::wstring args;
    std::wstring launch;  // open this file directly (desktop shortcuts); empty = AppsFolder
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
static bool g_opGuard = false;      // a shell file operation is running: ignore focus loss
static double g_focusGrace = 0;     // ignore focus loss until this time after such an operation

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
static ID2D1BitmapBrush* g_thumbBrush = nullptr;  // reused for image previews (device-dependent)
static ID2D1LinearGradientBrush* g_glow1 = nullptr;      // Aurora effect gradient (accent 1)
static ID2D1LinearGradientBrush* g_glow2 = nullptr;      // Aurora effect gradient (accent 2)
static IDWriteTextFormat* g_fmtInput = nullptr;
static IDWriteTextFormat* g_fmtTitle = nullptr;
static IDWriteTextFormat* g_fmtHint = nullptr;
static IDWriteTextFormat* g_fmtKey = nullptr;
static IDWriteTextFormat* g_fmtKeyBold = nullptr;  // emphasized chip labels (active filter/name)
static IDWriteTextFormat* g_fmtEmpty = nullptr;
static IDWriteTextFormat* g_fmtBadge = nullptr;       // engine initial in the web icon badge
static IDWriteTextFormat* g_fmtSection = nullptr;     // "Recent" / "Actions" headers
static IDWriteTextFormat* g_fmtMeta = nullptr;        // metadata line under a file row
static IDWriteTextFormat* g_fmtExt = nullptr;         // extension label inside a file-type tile
static IDWriteTextFormat* g_fmtPath = nullptr;        // last segment of the path band (semibold)
static IDWriteTextFormat* g_fmtPanelTitle = nullptr;  // app name in the panel
static IDWriteTextFormat* g_fmtPanelSub = nullptr;    // panel subtitle
static IDWriteTextFormat* g_fmtPanelRow = nullptr;    // panel rows
static IDWriteTextFormat* g_fmtForm = nullptr;        // value inside a custom-entry form field
static IDWriteTextFormat* g_fmtFormLabel = nullptr;   // label of a form field
static IDWriteTextFormat* g_fmtPreviewCode = nullptr; // monospace text of the Quick Look panel (no trimming)
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
// Browser bookmarks, loaded in the background and swapped in on the UI thread.
static std::vector<BookmarkEntry> g_bookmarks;
static UINT g_bookmarkGen = 0;             // drops the result of an older request
static ULONGLONG g_bookmarkStamp = 0;      // newest write time seen, to detect changes
static double g_bookmarkCheckedAt = -1e9;  // last change check (throttled)

// Opt-in clipboard history, newest first, stored per user in the mod storage. An image entry keeps
// only its file name and size in storage ("<hash>.png|1920x1080"); the PNG lives in the mod's own
// clips folder, so the history stays small and the file is deleted together with its entry.
struct ClipEntry {
    std::wstring text;    // the copied text, or "<file>|WxH" for an image
    bool pinned = false;  // pinned entries stay at the top and survive both the cap and retention
    bool image = false;
    int stamp = 0;  // NowStamp() at capture time, for the retention setting
    int w = 0;      // image width, for the row label
    int h = 0;      // image height
    std::wstring protectedText;  // the text as stored (DPAPI + base64), encrypted once per capture
};
static std::vector<ClipEntry> g_clips;

// Windows' Recent list, resolved to real paths in a worker thread.
struct RecentEntry {
    std::wstring name;
    std::wstring path;
    std::wstring dir;
    std::wstring ext;     // uppercase, "" when none
    std::wstring folded;  // name + folder, for filtering
    bool isDir = false;
    FILETIME used = {};   // the .lnk write time (newest first)
};
static std::vector<RecentEntry> g_recent;
static UINT g_recentGen = 0;
static ULONGLONG g_recentStamp = 0;
static double g_recentCheckedAt = -1e9;
static bool g_recentLoaded = false;  // a load has completed (even with no entries)

// Open top-level windows for "win:", cached until the bar opens again.
struct WindowEntry {
    HWND hwnd = nullptr;
    std::wstring title;
    std::wstring process;
    std::wstring folded;  // title + process, for filtering
    std::wstring path;    // the process image, used to draw the icon
    int appIcon = -1;     // index into g_apps, or -1
};
static std::vector<WindowEntry> g_windows;
static bool g_windowsDirty = true;
// The window that had the focus when the launcher opened, so "clip:" can paste back into it.
static HWND g_prevForeground = nullptr;
// "win:" panel: a snapshot of the selected window, because the row can disappear (a close, an app
// quitting) while the card is still sliding out.
struct WinPanelInfo {
    bool valid = false;
    HWND hwnd = nullptr;
    int iconApp = -1;
    std::wstring title;
    std::wstring process;
    std::wstring path;
    std::wstring state;
};
static WinPanelInfo g_winPanel;
static bool g_winCloseArmed = false;  // Ctrl+W waits for a second Ctrl+W
static HWND g_closingWin = nullptr;   // a window asked to close, still disappearing
static int g_closingTries = 0;
static double g_winNoticeAt = 0.0;             // last window notice, for the footer
static const wchar_t* g_winNoticeMsg = nullptr;
static D2D1_COLOR_F g_winNoticeTone = {};
static void FillWinPanel(const SearchResult& r);  // needs the window helpers further down
static void ReleaseWindowIcons();
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

// Details / preview panel
enum class PanelMode { Actions, Preview };
static bool g_detailsOpen = false;
static double g_detailPos = 0;           // 0..1, animated
static double g_detailTarget = 0;
static PanelMode g_panelMode = PanelMode::Actions;
// When non-None, the panel is animating closed and will reopen in this mode once it reaches ~0.
static PanelMode g_panelPendingMode = PanelMode::Actions;
static bool g_panelSwitching = false;    // true while waiting for close anim before switching mode
static int g_hoverAction = -1;     // id of the action under the mouse, or -1
static bool g_hoverReindex = false;  // the mouse is over the footer reindex button
static bool g_hoverHelp = false;     // the mouse is over the footer help button
static std::wstring g_panelAppId;  // last app shown in the panel (for the fade-out)
static std::unordered_map<std::wstring, AppDetails> g_detailsCache;

// Preview panel state
struct PreviewState {
    std::wstring path;              // file currently loaded (empty = nothing loaded)
    std::vector<std::wstring> lines; // text lines (text/JSON files)
    int scroll = 0;                  // first visible line
    int hscroll = 0;                 // first visible column (text is scrolled sideways, never trimmed)
    int maxCols = 0;                 // length of the longest line, in columns (tabs expanded)
    bool isImage = false;
    bool isText  = false;
    bool loaded  = false;
    std::wstring errorMsg;           // non-empty if load failed
};
static PreviewState g_preview;

// Custom-entry form (see the "Custom entry form" section). It replaces the results list while it
// is open, so the fields of an entry can be filled without leaving the launcher.
struct CustomFormState {
    bool active = false;
    int entry = -1;                  // index into g_cfg.commands
    int focus = 0;
    int scroll = 0;                  // first visible field (0 when they all fit)
    std::vector<std::wstring> values;  // one per field
    int caret = 0;
    int anchor = 0;
    float inputScroll = 0;           // horizontal scroll of the focused value
    std::wstring error;              // blocking problem (red)
    int errorField = -1;             // field to outline in red
    std::wstring warning;            // non-blocking problem (amber)
    bool warned = false;             // the warning was shown; the next Enter runs anyway
};
static CustomFormState g_form;

// Custom-entries manager: an in-app screen (opened from the ">" command "Custom entries") to
// create, edit and delete the user's own entries. It manages the persisted store only; entries
// defined in the Windhawk settings keep working but are not shown here. Two modes: a list of the
// stored entries (sel 0 is the "New entry" row, 1..N the entries) and an editor for one entry.
static const int kMgrFields = 7;  // Name, Type, Target, Arguments, Keywords, Trigger, Hotkey
struct ManagerState {
    bool active = false;
    bool editing = false;  // false: the list; true: the editor
    int sel = 0;           // list selection: 0 = "New entry", 1..N = stored entries
    int scroll = 0;        // first visible list row
    // Editor:
    bool creating = false;       // a new entry (vs editing an existing one)
    std::wstring editId;         // id of the entry being edited
    int field = 0;               // index into kMgrFieldLabels
    int typeIdx = 0;             // 0 url, 1 path, 2 run
    std::wstring vals[kMgrFields];  // text per field (index 1 / Type is unused: it uses typeIdx)
    int caret = 0;
    int anchor = 0;
    float inputScroll = 0;
    std::wstring error;
    bool deleteArmed = false;  // Delete pressed once; needs a second Delete to confirm
};
static ManagerState g_mgr;

// Help sheet (F1): replaces the list with a cheat sheet. Page 0 lists the shortcuts, page 1
// documents the calculator syntax (operators, functions, constants and conversions).
static bool g_helpOpen = false;
static int g_helpPage = 0;
static float g_helpScroll = 0;     // vertical offset of the sheet, for pages taller than the card
static float g_helpMaxScroll = 0;  // set while drawing, used by the wheel and the keys
static float g_helpViewH = 0;

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

// Ranking bonus from usage: frequent and recent apps rise, with a 30-day half-life. It stays below
// a word-start name match, and with the pin bonus (30) it never crosses a match tier.
static int FrecencyScore(const AppEntry& a) {
    if (a.uses <= 0) return 0;
    const int last = a.lastUsed > 0 ? a.lastUsed : NowStamp() - 90 * 86400;
    const double days = std::max(0, NowStamp() - last) / 86400.0;
    const double decayed = (double)a.uses * std::pow(0.5, days / 30.0);
    return std::min(60, (int)std::lround(decayed * 4.0));
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

// ----- Settings migration -----
// Old layouts are read while g_legacySettings (flat names) or g_movedSettings (the single "Features"
// section) are on, so an update never resets anything. Both switch off for good once the tab is
// saved in the current layout; a fresh install has nothing to carry over.
static const wchar_t kSettingsGroupedKey[] = L"settings.grouped_v1";
static const wchar_t kSettingsSectionsKey[] = L"settings.sections_v2";
static bool g_legacySettings = false;  // UI thread only: values still use the flat names
static bool g_movedSettings = false;   // UI thread only: values still live under the old sections

// Settings that moved into their own section in 1.0.0. Until the tab is saved once they are read
// from where they used to be stored, so an update never resets them.
struct MovedSetting {
    const wchar_t* now;
    const wchar_t* before;
};
static const MovedSetting kMovedSettings[] = {
    {L"Search.WebSearch", L"Features.WebSearch"},
    {L"Search.WebEngine", L"Features.WebEngine"},
    {L"Files.FilesEnabled", L"Features.FilesEnabled"},
    {L"Files.StartDrive", L"Features.StartDrive"},
    {L"Calculator.CalculatorEnabled", L"Features.CalculatorEnabled"},
    {L"Calculator.CalculatorStyle", L"Features.CalculatorStyle"},
};

static const wchar_t* MovedFrom(const wchar_t* name) {
    for (const MovedSetting& m : kMovedSettings) {
        if (wcscmp(m.now, name) == 0) return m.before;
    }
    return nullptr;
}

// "Shortcuts.Hotkey" -> "Hotkey" (the name the setting had before it was grouped).
static const wchar_t* LegacyLeaf(const wchar_t* name) {
    const wchar_t* dot = wcschr(name, L'.');
    return dot ? dot + 1 : name;
}

// Integer settings that did NOT exist in the last release that used flat names. An integer has
// no "not stored" state (a missing value reads as 0), so for these the old name must not be
// read, or a feature that never existed back then would come out switched off. Text settings
// do not need this: an empty text means "not stored".
static bool IntSettingExistedBeforeGrouping(const wchar_t* leaf) {
    static const wchar_t* const kNewer[] = {
        L"FilesEnabled", L"WebSearch", L"RecentFiles", L"LearnFromChoices", L"ClipRetentionDays",
    };
    for (const wchar_t* n : kNewer) {
        if (wcscmp(leaf, n) == 0) return false;
    }
    return true;
}

static std::wstring TrimBlanks(const std::wstring& s) {
    size_t a = 0, b = s.size();
    while (a < b && iswspace(s[a])) ++a;
    while (b > a && iswspace(s[b - 1])) --b;
    return s.substr(a, b - a);
}

// ----- Custom-entry placeholders -----
// `{folder}` is required, `{folder?}` is optional, `{branch=main}` starts pre-filled and
// `{query}` is the extra words typed after the entry's name (always optional). Names are matched
// case-insensitively (spaces, `_` and `-` are ignored), `{{` and `}}` stand for literal braces,
// and a field can be repeated.
// Placeholder names are compared without case, spaces, underscores or hyphens, so {my_folder},
// {My-Folder} and {my folder} are the same field.
static std::wstring CustomKeyId(const std::wstring& key) {
    std::wstring out;
    out.reserve(key.size());
    for (const wchar_t c : FoldString(key)) {
        if (c == L' ' || c == L'\t' || c == L'_' || c == L'-') continue;
        out.push_back(c);
    }
    return out;
}

static bool IsQueryFieldKey(const std::wstring& key) {
    return CustomKeyId(key) == L"query";
}

// True when the entry has a field the form would ask the user to fill — any placeholder other than
// {query}, which is filled from the words typed after the entry's name. This is what decides
// whether Enter opens the form (fields to fill) or launches straight away (search-only/fieldless).
static bool CustomHasFormFields(const CustomEntry& e) {
    for (const CustomField& f : e.fields) {
        if (!IsQueryFieldKey(f.key)) return true;
    }
    return false;
}

static std::wstring CustomFieldLabel(const std::wstring& key) {
    if (IsQueryFieldKey(key)) return L"Search";
    std::wstring out = key;
    for (wchar_t& c : out) {
        if (c == L'_' || c == L'-') c = L' ';
    }
    bool wordStart = true;
    for (wchar_t& c : out) {
        if (c == L' ') {
            wordStart = true;
        } else if (wordStart) {
            c = (wchar_t)towupper(c);
            wordStart = false;
        }
    }
    return TrimBlanks(out);
}

static void CollectCustomFields(const std::wstring& text, std::vector<CustomField>& fields) {
    for (size_t i = 0; i + 1 < text.size(); ++i) {
        if (text[i] != L'{') continue;
        if (text[i + 1] == L'{') {  // "{{" is a literal brace, not a placeholder
            ++i;
            continue;
        }
        const size_t close = text.find(L'}', i + 1);
        if (close == std::wstring::npos) break;
        std::wstring body = text.substr(i + 1, close - i - 1);
        i = close;
        if (body.empty()) continue;

        std::wstring def;
        const size_t eq = body.find(L'=');
        if (eq != std::wstring::npos) {
            def = TrimBlanks(body.substr(eq + 1));
            body.resize(eq);
        }
        body = TrimBlanks(body);
        bool optional = false;
        if (!body.empty() && body.back() == L'?') {
            optional = true;
            body.pop_back();
            body = TrimBlanks(body);
        }
        if (body.empty()) continue;
        if (fields.size() >= 16) break;  // a sane limit for the form

        const std::wstring folded = CustomKeyId(body);
        bool dup = false;
        for (const CustomField& f : fields) {
            if (CustomKeyId(f.key) == folded) {
                dup = true;
                break;
            }
        }
        if (dup) continue;

        CustomField f;
        f.key = body;
        f.label = CustomFieldLabel(body);
        f.optional = optional || IsQueryFieldKey(body);  // {query} may always be empty
        f.value = def;
        fields.push_back(std::move(f));
    }
}

// Builds a custom entry from its raw fields: normalizes the type, folds the name and keywords, and
// collects the placeholders. Shared by the settings loader and the in-app store (name and target
// should already be trimmed by the caller).
static CustomEntry MakeCustomEntry(const std::wstring& name, const std::wstring& type,
                                   const std::wstring& target, const std::wstring& args,
                                   const std::wstring& keywords,
                                   const std::wstring& trigger = L"",
                                   const std::wstring& hotkey = L"") {
    CustomEntry e;
    e.name = name;
    e.folded = FoldString(name);
    e.type = FoldString(type);
    if (e.type != L"path" && e.type != L"run") e.type = L"url";  // default and unknown values
    e.target = target;
    e.args = args;
    e.keywords = FoldString(keywords);
    e.trigger = FoldString(trigger);
    e.hotkey = hotkey;
    CollectCustomFields(e.target, e.fields);
    CollectCustomFields(e.args, e.fields);
    for (const CustomField& f : e.fields) {
        if (IsQueryFieldKey(f.key)) e.hasQuery = true;
    }
    return e;
}

// The in-app entry store (persisted by the mod, filled by its own manager) is loaded here; defined
// with the other persisted storage, below CurrentUserSid.
static void LoadCustomStore(std::vector<CustomEntry>& out);
static void LoadClipHistory();
static void SaveClipHistory();
static void UpdateClipboardListener();
static ID2D1Bitmap* ClipImageThumb(const std::wstring& stored);
static void DeleteClipImageFile(const std::wstring& stored);
static bool PurgeExpiredClips();
static void EraseClipHistory();

static std::wstring ReadTextSetting(const wchar_t* name) {
    PCWSTR s = Wh_GetStringSetting(name);  // never returns NULL
    std::wstring r = s;
    Wh_FreeStringSetting(s);
    return r;
}

// Decides once, when the mod starts, whether there are old flat values to carry over.
static void DetectLegacySettings() {
    g_legacySettings = false;
    if (Wh_GetIntValue(kSettingsGroupedKey, 0) != 0) return;  // already on the new names
    static const wchar_t* const kProbes[] = {
        L"Hotkey", L"Effect", L"Color", L"ListMode", L"ReindexMode", L"OpenOn", L"DetailsKey",
    };
    for (const wchar_t* p : kProbes) {
        if (!ReadTextSetting(p).empty()) {
            g_legacySettings = true;
            return;
        }
    }
    Wh_SetIntValue(kSettingsGroupedKey, 1);  // nothing to carry over: do not look again
}

// A fresh install has nothing stored anywhere, so the old section names are only read when the
// previous layout was actually saved: WebEngine is a string that has always had a value, which makes
// it the probe for "this user has settings to carry over".
static void DetectMovedSettings() {
    g_movedSettings = false;
    if (Wh_GetIntValue(kSettingsSectionsKey, 0) != 0) return;  // saved with the new sections
    if (!ReadTextSetting(L"Features.WebEngine").empty()) {
        g_movedSettings = true;
        return;
    }
    Wh_SetIntValue(kSettingsSectionsKey, 1);
}

static std::wstring GetStringSetting(const wchar_t* name) {
    if (g_legacySettings) {
        std::wstring old = ReadTextSetting(LegacyLeaf(name));
        if (!old.empty()) return old;
    }
    // While the previous layout was in use, its value wins: the new name would otherwise return the
    // schema default and hide what the user actually chose.
    if (g_movedSettings) {
        if (const wchar_t* before = MovedFrom(name)) {
            std::wstring old = ReadTextSetting(before);
            if (!old.empty()) return old;
        }
    }
    return ReadTextSetting(name);
}

static int GetIntSetting(const wchar_t* name) {
    if (g_legacySettings) {
        const wchar_t* leaf = LegacyLeaf(name);
        if (IntSettingExistedBeforeGrouping(leaf)) return Wh_GetIntSetting(leaf);
    }
    if (g_movedSettings) {
        if (const wchar_t* before = MovedFrom(name)) return Wh_GetIntSetting(before);
    }
    return Wh_GetIntSetting(name);
}

static void LoadSettings() {
    LauncherSettings c;
    std::wstring hk = GetStringSetting(L"Shortcuts.Hotkey");
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

    c.verticalPct = std::clamp(GetIntSetting(L"Appearance.VerticalPosition"), 0, 100);
    {
        std::wstring mo = GetStringSetting(L"Behavior.OpenOn");
        c.openMonitor = (mo == L"foreground") ? OpenMonitor::Foreground
                        : (mo == L"primary")  ? OpenMonitor::Primary
                                              : OpenMonitor::Cursor;
    }
    c.maxRows = std::clamp(GetIntSetting(L"Behavior.MaxResults"), 3, 12);
    c.opacityPct = std::clamp(GetIntSetting(L"Appearance.Opacity"), 70, 100);
    {
        std::wstring ef = GetStringSetting(L"Appearance.Effect");
        c.effect = (ef == L"mica")      ? BgEffect::Mica
                   : (ef == L"glass")   ? BgEffect::Glass
                   : (ef == L"frosted") ? BgEffect::Frosted
                   : (ef == L"aurora")  ? BgEffect::Aurora
                   : (ef == L"solid")   ? BgEffect::Solid
                                        : BgEffect::Acrylic;
    }
    c.blurPct = std::clamp(GetIntSetting(L"Appearance.BlurAmount"), 0, 100);
    c.liveAcrylic = GetIntSetting(L"Appearance.LiveAcrylic") != 0;

    {
        std::wstring col = GetStringSetting(L"Appearance.Color");
        c.colorIdx = 0;
        for (int i = 0; i < (int)(sizeof(kColors) / sizeof(kColors[0])); ++i) {
            if (col == kColors[i].id) {
                c.colorIdx = i;
                break;
            }
        }
    }
    c.hideUtility = GetIntSetting(L"Apps.HideUtilityEntries") != 0;
    c.disableFullscreen = GetIntSetting(L"Behavior.DisableInFullscreen") != 0;

    std::wstring rm = GetStringSetting(L"Apps.ReindexMode");
    if (rm == L"always") {
        c.reindexMode = ReindexMode::Always;
    } else if (rm == L"min1" || rm == L"min3" || rm == L"min5" || rm == L"min10") {
        c.reindexMode = ReindexMode::Interval;
        c.reindexIntervalSec = (double)_wtoi(rm.c_str() + 3) * 60.0;  // "min3" -> 3
    } else {
        c.reindexMode = ReindexMode::Auto;  // default (and if the value is not recognized)
    }

    std::wstring lm = GetStringSetting(L"Appearance.ListMode");
    c.listMode = (lm == L"compact") ? ListMode::Compact : ListMode::Expanded;

    std::wstring dk = GetStringSetting(L"Shortcuts.DetailsKey");
    c.detailsKey = (dk == L"ctrl+k")       ? DetailsKey::CtrlK
                   : (dk == L"ctrl+space") ? DetailsKey::CtrlSpace
                   : (dk == L"f4")         ? DetailsKey::F4
                                           : DetailsKey::CtrlI;
    // If the global shortcut is Ctrl+Space, that shortcut would never reach the window: use Ctrl+I.
    if (c.detailsKey == DetailsKey::CtrlSpace && c.hotkeyMods == MOD_CONTROL) {
        c.detailsKey = DetailsKey::CtrlI;
    }

    c.calculatorEnabled = GetIntSetting(L"Calculator.CalculatorEnabled") != 0;
    {
        std::wstring cs = GetStringSetting(L"Calculator.CalculatorStyle");
        c.calcStyle = (cs == L"hero")  ? CalcStyle::Hero
                      : (cs == L"ghost") ? CalcStyle::Ghost
                                         : CalcStyle::Both;
    }

    c.sounds = GetIntSetting(L"Sounds.SoundEnabled") != 0;
    c.soundVolume = std::clamp(GetIntSetting(L"Sounds.SoundVolume"), 0, 100);

    c.webSearch = GetIntSetting(L"Search.WebSearch") != 0;
    {
        std::wstring we = GetStringSetting(L"Search.WebEngine");
        c.webEngine = (we == L"bing")         ? WebEngine::Bing
                      : (we == L"duckduckgo") ? WebEngine::DuckDuckGo
                      : (we == L"youtube")    ? WebEngine::YouTube
                      : (we == L"wikipedia")  ? WebEngine::Wikipedia
                                              : WebEngine::Google;
    }

    c.filesEnabled = GetIntSetting(L"Files.FilesEnabled") != 0;
    c.fileSearchEngine = GetStringSetting(L"Search.FileSearchEngine") == L"everything"
                             ? FileSearchEngine::Everything
                             : FileSearchEngine::Builtin;
    c.rememberFileSort = GetIntSetting(L"Files.RememberFileSort") != 0;
    c.learnFromChoices = GetIntSetting(L"Search.LearnFromChoices") != 0;
    c.bookmarks = GetIntSetting(L"Search.Bookmarks") != 0;  // off unless enabled
    c.clipboardHistory = GetIntSetting(L"Clipboard.ClipboardHistory") != 0;  // off unless enabled
    c.clipRetentionDays =
        std::clamp(GetIntSetting(L"Clipboard.ClipRetentionDays"), 0, 365);
    c.recentFiles = GetIntSetting(L"Search.RecentFiles") != 0;
    c.searchRoots.clear();
    {
        std::wstring raw = GetStringSetting(L"Search.SearchRoots");
        std::wstring cur;
        for (const wchar_t ch : raw) {
            if (ch == L';' || ch == L'\n' || ch == L'\r') {
                std::wstring p = TrimBlanks(cur);
                if (!p.empty() && c.searchRoots.size() < 16) c.searchRoots.push_back(p);
                cur.clear();
            } else {
                cur += ch;
            }
        }
        std::wstring p = TrimBlanks(cur);
        if (!p.empty() && c.searchRoots.size() < 16) c.searchRoots.push_back(p);
    }
    {
        // Exactly one ASCII letter, optionally followed by a colon (for example "D" or "D:"). A word,
        // an accent, a non-ASCII letter or extra text means "open on Home" instead of guessing.
        std::wstring sd = GetStringSetting(L"Files.StartDrive");
        size_t a = 0;
        while (a < sd.size() && (sd[a] == L' ' || sd[a] == L'\t')) ++a;
        size_t b = sd.size();
        while (b > a && (sd[b - 1] == L' ' || sd[b - 1] == L'\t')) --b;
        if (b > a && sd[b - 1] == L':') --b;
        const wchar_t letter = a < b ? sd[a] : 0;
        const bool singleLetter = (b == a + 1) && ((letter >= L'A' && letter <= L'Z') ||
                                                   (letter >= L'a' && letter <= L'z'));
        c.startDrive = singleLetter ? (wchar_t)towupper(letter) : 0;
    }

    // Entries created in the in-app manager (persisted store).
    LoadCustomStore(c.commands);

    g_cfg = c;
    if (g_form.active) g_form = CustomFormState();  // the entry it pointed to may be gone
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

// XAudio2 reports a lost output device (unplugged headset, disabled device...) through
// OnCriticalError, on its own audio thread. StartEngine can still succeed afterwards with no
// audible output, so the flag is what tells Resume() to rebuild the engine.
struct EngineCallback : IXAudio2EngineCallback {
    std::atomic<bool> lost{false};
    void STDMETHODCALLTYPE OnProcessingPassStart() override {}
    void STDMETHODCALLTYPE OnProcessingPassEnd() override {}
    void STDMETHODCALLTYPE OnCriticalError(HRESULT) override { lost = true; }
};
static EngineCallback g_engineCallback;

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

// Deep, creamy bottom-out (a lubed linear switch): low body, quiet case resonances and a short
// low-passed contact. Rounded on purpose, never clicky. base = pitch in Hz, tauScale = ring time.
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
    g_engineCallback.lost = false;
    e.engine->RegisterForCallbacks(&g_engineCallback);
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
        e.engine->UnregisterForCallbacks(&g_engineCallback);
        e.engine->Release();
        e.engine = nullptr;
    }
    if (e.dll) { FreeLibrary(e.dll); e.dll = nullptr; }
    e.ready = false;
}

// The engine only runs while the bar is open and sounds are enabled.
// If there was no output device at startup, or the device was removed (the engine then reports
// a critical error, see EngineCallback), rebuild the engine here before starting it.
static void Resume() {
    if (!g_cfg.sounds) return;
    Engine& e = g_eng;
    if (e.ready && e.engine && !g_engineCallback.lost && SUCCEEDED(e.engine->StartEngine())) return;
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
        g_pal.textDim = Mix(ink, white, 0.25f);  // was 0.45: too light to read over Aurora
        // The accent as a brush is too light on a pale/Aurora card, so darken it for
        // anything that is drawn as ink (labels, icons, highlighted letters).
        g_pal.accentInk = Mix(A, ink, 0.50f);
        g_pal.accentStrong = Mix(A, ink, 0.68f);  // emphasized chip labels
        g_pal.selection = D2D1::ColorF(A.r, A.g, A.b, 0.15f);
        g_pal.separator = D2D1::ColorF(ink.r, ink.g, ink.b, 0.10f);
        g_pal.keycap = D2D1::ColorF(ink.r, ink.g, ink.b, 0.08f);
        g_pal.match = g_pal.accentInk;
        g_pal.caretSel = D2D1::ColorF(A.r, A.g, A.b, 0.25f);
        g_pal.shadowAlpha = 0.025f;
    } else {
        // Dark: near-white letters tinted with the accent, translucent layers of the same hue.
        g_pal.border = Mix(white, A, 0.30f, 0.19f);
        g_pal.text = Mix(white, A, 0.07f);
        g_pal.textDim = Mix(D2D1::ColorF(0.64f, 0.65f, 0.68f), A, 0.22f);
        // Slightly lift the accent so it stays clear over the Aurora sheen.
        g_pal.accentInk = Mix(A, white, 0.12f);
        g_pal.accentStrong = Mix(A, white, 0.30f);  // emphasized chip labels
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

// ===== Activators (typed prefixes that switch the launcher into a mode) =====
// A leading trigger selects a mode: "?" and "g:", "y:" ... search the web (optionally on a fixed
// engine) and ">" lists built-in commands.
enum class LauncherMode { Web, Commands, Bookmarks, Clipboard, Recent, GlobalFiles, WindowList };

struct ActivatorDef {
    const wchar_t* trigger;
    const wchar_t* name;
    LauncherMode mode;
    int engine;  // Web: WebEngine value, or -1 to use the engine from settings
};

static const ActivatorDef kActivators[] = {
    {L"?", L"Web", LauncherMode::Web, -1},
    {L">", L"Commands", LauncherMode::Commands, -1},
    {L"g:", L"Google", LauncherMode::Web, (int)WebEngine::Google},
    // "bm:" must come before "b:": triggers are matched in this order, so "b:" would swallow it.
    {L"bm:", L"Bookmarks", LauncherMode::Bookmarks, -1},
    {L"clip:", L"Clipboard", LauncherMode::Clipboard, -1},
    {L"recent:", L"Recent files", LauncherMode::Recent, -1},
    {L"find:", L"All files", LauncherMode::GlobalFiles, -1},
    {L"win:", L"Windows", LauncherMode::WindowList, -1},
    {L"b:", L"Bing", LauncherMode::Web, (int)WebEngine::Bing},
    {L"d:", L"DuckDuckGo", LauncherMode::Web, (int)WebEngine::DuckDuckGo},
    {L"y:", L"YouTube", LauncherMode::Web, (int)WebEngine::YouTube},
    {L"w:", L"Wikipedia", LauncherMode::Web, (int)WebEngine::Wikipedia},
};
static const int kActivatorCount = (int)(sizeof(kActivators) / sizeof(kActivators[0]));
static int g_activator = -1;  // index into kActivators, or -1 when no mode is active
static int g_confirmCmd = -1;  // index into kCommands armed for a second-Enter confirm, or -1

// Web modes have a single deterministic action, so the bar stays compact; command mode lists
// entries and expands like a normal search.
static bool ModeIsWeb() {
    return g_activator >= 0 && kActivators[g_activator].mode == LauncherMode::Web;
}

// Built-in commands offered by the ">" activator. id is the stable action key (see RunCommand).
// glyph/color give each command its own badge icon (see DrawCommandGlyph).
enum class CmdGlyph {
    Lock, Moon, Bars, Sliders, Chip, List, Key, Globe, Update, Grid, Monitor, Speaker, Bluetooth,
    Printer, Terminal, Folder, Gear, Power, Restart, SignOut, Hibernate, Recycle, Wifi, Clock, Bell,
    Camera, Mic, Doc, Home, Download, Image, Music, Video, Close, ExternalLink, ChevronRight, ChevronLeft,
    Drive, Link, Star, Clipboard, Pin
};

struct CommandDef {
    const wchar_t* id;
    const wchar_t* title;
    const wchar_t* keywords;
    const wchar_t* category;
    CmdGlyph glyph;
    D2D1_COLOR_F color;
    bool destructive;    // needs a second Enter to run (see LaunchSelected)
    int activator = -1;  // mode opener: index into kActivators, or -1 for a normal command
};

static const CommandDef kCommands[] = {
    {L"lock", L"Lock", L"lock workstation screen", L"Power", CmdGlyph::Lock,
     D2D1::ColorF(0.36f, 0.46f, 0.66f), false},
    {L"sleep", L"Sleep", L"sleep suspend standby", L"Power", CmdGlyph::Moon,
     D2D1::ColorF(0.44f, 0.42f, 0.72f), false},
    {L"hibernate", L"Hibernate", L"hibernate sleep to disk", L"Power", CmdGlyph::Hibernate,
     D2D1::ColorF(0.40f, 0.50f, 0.62f), true},
    {L"shutdown", L"Shut Down", L"shut down power off turn off", L"Power", CmdGlyph::Power,
     D2D1::ColorF(0.85f, 0.30f, 0.30f), true},
    {L"restart", L"Restart", L"restart reboot", L"Power", CmdGlyph::Restart,
     D2D1::ColorF(0.85f, 0.45f, 0.28f), true},
    {L"signout", L"Sign Out", L"sign out log off logoff", L"Power", CmdGlyph::SignOut,
     D2D1::ColorF(0.80f, 0.44f, 0.52f), true},
    {L"taskmgr", L"Task Manager", L"task manager processes", L"System", CmdGlyph::Bars,
     D2D1::ColorF(0.20f, 0.56f, 0.56f), false},
    {L"control", L"Control Panel", L"control panel", L"System", CmdGlyph::Sliders,
     D2D1::ColorF(0.42f, 0.50f, 0.60f), false},
    {L"devmgmt", L"Device Manager", L"device manager hardware", L"System", CmdGlyph::Chip,
     D2D1::ColorF(0.30f, 0.56f, 0.40f), false},
    {L"services", L"Services", L"services background", L"System", CmdGlyph::List,
     D2D1::ColorF(0.72f, 0.56f, 0.24f), false},
    {L"regedit", L"Registry Editor", L"registry editor regedit", L"System", CmdGlyph::Key,
     D2D1::ColorF(0.62f, 0.44f, 0.30f), false},
    {L"netconnections", L"Network Connections", L"network connections adapters", L"System",
     CmdGlyph::Globe, D2D1::ColorF(0.20f, 0.47f, 0.76f), false},
    {L"settings", L"Settings", L"settings windows", L"Settings", CmdGlyph::Gear,
     D2D1::ColorF(0.46f, 0.51f, 0.60f), false},
    {L"windowsupdate", L"Windows Update", L"windows update", L"Settings", CmdGlyph::Update,
     D2D1::ColorF(0.24f, 0.53f, 0.82f), false},
    {L"installedapps", L"Installed Apps", L"installed apps uninstall programs", L"Settings",
     CmdGlyph::Grid, D2D1::ColorF(0.50f, 0.42f, 0.70f), false},
    {L"display", L"Display Settings", L"display settings monitor brightness", L"Settings",
     CmdGlyph::Monitor, D2D1::ColorF(0.24f, 0.52f, 0.68f), false},
    {L"sound", L"Sound Settings", L"sound audio volume output input", L"Settings",
     CmdGlyph::Speaker, D2D1::ColorF(0.72f, 0.40f, 0.47f), false},
    {L"bluetooth", L"Bluetooth", L"bluetooth devices", L"Settings", CmdGlyph::Bluetooth,
     D2D1::ColorF(0.22f, 0.50f, 0.80f), false},
    {L"printers", L"Printers & Scanners", L"printers scanners print", L"Settings",
     CmdGlyph::Printer, D2D1::ColorF(0.46f, 0.49f, 0.55f), false},
    {L"terminal", L"Terminal", L"terminal powershell cmd console", L"Tools", CmdGlyph::Terminal,
     D2D1::ColorF(0.18f, 0.19f, 0.23f), false},
    {L"explorer", L"File Explorer", L"file explorer files folders", L"Tools", CmdGlyph::Folder,
     D2D1::ColorF(0.84f, 0.64f, 0.26f), false},
    {L"emptyrecyclebin", L"Empty Recycle Bin", L"empty recycle bin trash", L"Tools",
     CmdGlyph::Recycle, D2D1::ColorF(0.66f, 0.42f, 0.42f), true},
    {L"customentries", L"Custom Entries", L"custom entries commands manage add new edit", L"Tools",
     CmdGlyph::List, D2D1::ColorF(0.30f, 0.56f, 0.52f), false},
    {L"clearclipboard", L"Clear Clipboard History", L"clear clipboard history clipoard", L"Tools",
     CmdGlyph::Clipboard, D2D1::ColorF(0.30f, 0.62f, 0.55f), true},
    // Mode openers: listed in ">" and blended into normal search like an app, so the prefixes are
    // not the only way in. Enter switches into the mode exactly as typing its prefix does. The last
    // field is the index into kActivators ("?", ">", "g:", "bm:", "clip:", "recent:", "find:", "win:").
    {L"modecommands", L"Commands", L"commands actions all list", L"Modes", CmdGlyph::List,
     D2D1::ColorF(0.30f, 0.56f, 0.52f), false, 1 /* > */},
    {L"modebookmarks", L"Bookmarks", L"bookmarks favourites favorites browser bm", L"Modes",
     CmdGlyph::Star, D2D1::ColorF(0.84f, 0.72f, 0.30f), false, 3 /* bm: */},
    {L"modeclipboard", L"Clipboard History", L"clipboard history clip copies text", L"Modes",
     CmdGlyph::Clipboard, D2D1::ColorF(0.30f, 0.62f, 0.55f), false, 4 /* clip: */},
    {L"moderecent", L"Recent Files", L"recent recently files opened", L"Modes", CmdGlyph::Clock,
     D2D1::ColorF(0.50f, 0.56f, 0.72f), false, 5 /* recent: */},
    {L"modefiles", L"All Files", L"files find folders search disk", L"Modes", CmdGlyph::Folder,
     D2D1::ColorF(0.84f, 0.64f, 0.26f), false, 6 /* find: */},
    {L"modewindows", L"Open Windows", L"windows window manager switcher switch focus open list tasks",
     L"Modes", CmdGlyph::Monitor, D2D1::ColorF(0.24f, 0.52f, 0.68f), false, 7 /* win: */},
};
static const int kCommandCount = (int)(sizeof(kCommands) / sizeof(kCommands[0]));

// Windows Settings pages surfaced in normal search (no prefix): typing "bluetooth", "display" or
// "update" offers a result that opens the matching page in the Settings app. uri is the
// ms-settings: target; glyph/color give each page its own badge icon (see DrawCommandGlyph).
struct SettingDef {
    const wchar_t* title;
    const wchar_t* keywords;
    const wchar_t* uri;
    CmdGlyph glyph;
    D2D1_COLOR_F color;
};

static const SettingDef kSettings[] = {
    {L"Wi-Fi", L"wifi wireless network internet wlan", L"ms-settings:network-wifi",
     CmdGlyph::Wifi, D2D1::ColorF(0.24f, 0.56f, 0.82f)},
    {L"Bluetooth", L"bluetooth devices pairing", L"ms-settings:bluetooth",
     CmdGlyph::Bluetooth, D2D1::ColorF(0.22f, 0.50f, 0.80f)},
    {L"Airplane mode", L"airplane mode flight radio", L"ms-settings:network-airplanemode",
     CmdGlyph::Globe, D2D1::ColorF(0.20f, 0.47f, 0.76f)},
    {L"Ethernet", L"ethernet wired network lan cable", L"ms-settings:network-ethernet",
     CmdGlyph::Globe, D2D1::ColorF(0.20f, 0.47f, 0.76f)},
    {L"VPN", L"vpn virtual private network", L"ms-settings:network-vpn", CmdGlyph::Key,
     D2D1::ColorF(0.30f, 0.44f, 0.68f)},
    {L"Proxy", L"proxy server", L"ms-settings:network-proxy", CmdGlyph::Globe,
     D2D1::ColorF(0.20f, 0.47f, 0.76f)},
    {L"Mobile hotspot", L"mobile hotspot tethering share internet",
     L"ms-settings:network-mobilehotspot", CmdGlyph::Globe, D2D1::ColorF(0.26f, 0.52f, 0.78f)},
    {L"Display", L"display monitor screen resolution brightness scaling", L"ms-settings:display",
     CmdGlyph::Monitor, D2D1::ColorF(0.24f, 0.52f, 0.68f)},
    {L"Night light", L"night light blue light filter", L"ms-settings:nightlight",
     CmdGlyph::Moon, D2D1::ColorF(0.42f, 0.40f, 0.68f)},
    {L"Sound", L"sound audio volume output input speaker microphone", L"ms-settings:sound",
     CmdGlyph::Speaker, D2D1::ColorF(0.72f, 0.40f, 0.47f)},
    {L"Notifications", L"notifications alerts banners actions", L"ms-settings:notifications",
     CmdGlyph::Bell, D2D1::ColorF(0.86f, 0.55f, 0.25f)},
    {L"Focus assist", L"focus assist do not disturb quiet hours", L"ms-settings:quiethours",
     CmdGlyph::Moon, D2D1::ColorF(0.44f, 0.44f, 0.72f)},
    {L"Power & sleep", L"power sleep screen timeout", L"ms-settings:powersleep",
     CmdGlyph::Power, D2D1::ColorF(0.44f, 0.42f, 0.72f)},
    {L"Battery", L"battery saver power usage", L"ms-settings:batterysaver", CmdGlyph::Power,
     D2D1::ColorF(0.34f, 0.62f, 0.40f)},
    {L"Storage", L"storage disk space drives cleanup sense", L"ms-settings:storagesense",
     CmdGlyph::Folder, D2D1::ColorF(0.72f, 0.56f, 0.24f)},
    {L"Multitasking", L"multitasking snap windows virtual desktops", L"ms-settings:multitasking",
     CmdGlyph::Grid, D2D1::ColorF(0.38f, 0.52f, 0.66f)},
    {L"Apps & features", L"apps features uninstall programs installed software",
     L"ms-settings:appsfeatures", CmdGlyph::Grid, D2D1::ColorF(0.50f, 0.42f, 0.70f)},
    {L"Default apps", L"default apps file associations", L"ms-settings:defaultapps",
     CmdGlyph::Grid, D2D1::ColorF(0.50f, 0.42f, 0.70f)},
    {L"Startup apps", L"startup apps boot login startup", L"ms-settings:startupapps",
     CmdGlyph::List, D2D1::ColorF(0.46f, 0.49f, 0.58f)},
    {L"Windows Update", L"windows update upgrade patch", L"ms-settings:windowsupdate",
     CmdGlyph::Update, D2D1::ColorF(0.24f, 0.53f, 0.82f)},
    {L"Personalization", L"personalization desktop appearance", L"ms-settings:personalization",
     CmdGlyph::Gear, D2D1::ColorF(0.56f, 0.42f, 0.72f)},
    {L"Background", L"background wallpaper picture", L"ms-settings:personalization-background",
     CmdGlyph::Gear, D2D1::ColorF(0.56f, 0.42f, 0.72f)},
    {L"Colors", L"colors accent colour dark light mode", L"ms-settings:colors", CmdGlyph::Gear,
     D2D1::ColorF(0.56f, 0.42f, 0.72f)},
    {L"Themes", L"themes theme pack", L"ms-settings:themes", CmdGlyph::Gear,
     D2D1::ColorF(0.56f, 0.42f, 0.72f)},
    {L"Lock screen", L"lock screen", L"ms-settings:lockscreen", CmdGlyph::Lock,
     D2D1::ColorF(0.42f, 0.46f, 0.60f)},
    {L"Taskbar", L"taskbar system tray corner", L"ms-settings:taskbar", CmdGlyph::Bars,
     D2D1::ColorF(0.40f, 0.44f, 0.56f)},
    {L"Date & time", L"date time clock timezone", L"ms-settings:dateandtime", CmdGlyph::Clock,
     D2D1::ColorF(0.36f, 0.50f, 0.68f)},
    {L"Language", L"language keyboard input region", L"ms-settings:regionlanguage",
     CmdGlyph::Globe, D2D1::ColorF(0.30f, 0.54f, 0.72f)},
    {L"Region", L"region format locale country", L"ms-settings:regionformatting",
     CmdGlyph::Globe, D2D1::ColorF(0.30f, 0.54f, 0.72f)},
    {L"Accessibility", L"accessibility ease narrator magnifier contrast", L"ms-settings:easeofaccess",
     CmdGlyph::Sliders, D2D1::ColorF(0.30f, 0.56f, 0.62f)},
    {L"Typing", L"typing keyboard autocorrect", L"ms-settings:typing", CmdGlyph::Sliders,
     D2D1::ColorF(0.46f, 0.49f, 0.58f)},
    {L"Mouse", L"mouse cursor pointer speed", L"ms-settings:mousetouchpad", CmdGlyph::Sliders,
     D2D1::ColorF(0.46f, 0.49f, 0.58f)},
    {L"Touchpad", L"touchpad gestures", L"ms-settings:devices-touchpad", CmdGlyph::Sliders,
     D2D1::ColorF(0.46f, 0.49f, 0.58f)},
    {L"Cameras", L"camera webcam", L"ms-settings:camera", CmdGlyph::Camera,
     D2D1::ColorF(0.30f, 0.52f, 0.66f)},
    {L"Microphone", L"microphone mic privacy", L"ms-settings:privacy-microphone", CmdGlyph::Mic,
     D2D1::ColorF(0.62f, 0.40f, 0.56f)},
    {L"Location", L"location gps privacy", L"ms-settings:privacy-location", CmdGlyph::Globe,
     D2D1::ColorF(0.28f, 0.54f, 0.60f)},
    {L"Devices", L"devices connected bluetooth printers", L"ms-settings:connecteddevices",
     CmdGlyph::Chip, D2D1::ColorF(0.30f, 0.56f, 0.40f)},
    {L"Printers & scanners", L"printers scanners print", L"ms-settings:printers",
     CmdGlyph::Printer, D2D1::ColorF(0.46f, 0.49f, 0.55f)},
    {L"About", L"about system info specifications pc this", L"ms-settings:about",
     CmdGlyph::List, D2D1::ColorF(0.46f, 0.49f, 0.58f)},
    {L"Clipboard", L"clipboard history copy paste", L"ms-settings:clipboard", CmdGlyph::Bars,
     D2D1::ColorF(0.46f, 0.49f, 0.58f)},
    {L"Accounts", L"accounts your info microsoft profile", L"ms-settings:yourinfo",
     CmdGlyph::SignOut, D2D1::ColorF(0.40f, 0.50f, 0.70f)},
    {L"Sign-in options", L"sign in options password pin windows hello security",
     L"ms-settings:signinoptions", CmdGlyph::SignOut, D2D1::ColorF(0.40f, 0.50f, 0.70f)},
};
static const int kSettingCount = (int)(sizeof(kSettings) / sizeof(kSettings[0]));

// ===== File browsing (toggled by the folder button next to the search box) =====
// The chips at the top of file mode scope the listing to one of the user's known folders. `folder`
// is the real location, which follows the folder when Windows has moved it to another drive or
// redirected it (OneDrive-backed Documents, a relocated Downloads, …); `sub` is only a fallback
// when the known folder cannot be resolved.
struct FileLocationDef {
    const wchar_t* label;
    const wchar_t* sub;              // relative to %USERPROFILE% (empty = the profile itself)
    const KNOWNFOLDERID* folder;     // real location; nullptr to use `sub` only
    CmdGlyph glyph;
};

static const FileLocationDef kFileLocations[] = {
    {L"Home", L"", &FOLDERID_Profile, CmdGlyph::Home},
    {L"Downloads", L"Downloads", &FOLDERID_Downloads, CmdGlyph::Download},
    {L"Documents", L"Documents", &FOLDERID_Documents, CmdGlyph::Doc},
    {L"Pictures", L"Pictures", &FOLDERID_Pictures, CmdGlyph::Image},
    {L"Music", L"Music", &FOLDERID_Music, CmdGlyph::Music},
    {L"Videos", L"Videos", &FOLDERID_Videos, CmdGlyph::Video},
};
static const int kFileLocationCount = (int)(sizeof(kFileLocations) / sizeof(kFileLocations[0]));

struct FileEntry {
    std::wstring name;
    std::wstring path;    // full path
    std::wstring dir;     // containing folder
    std::wstring ext;     // uppercase extension without the dot ("" when none)
    std::wstring folded;  // name, lowercased and accent-free (for filtering and sorting)
    std::wstring meta;    // "Folder · 18/10/2026 16:02", built the first time it is shown
    uint64_t size = 0;
    FILETIME writeTime = {};
    DWORD attr = 0;  // WIN32_FIND_DATA attributes (cloud/offline checks)
    bool isDir = false;
    bool noInfo = false;  // size/date unknown (an index row whose reply did not carry them)
};

static bool g_fileMode = false;                 // file mode active (folder button)
static int g_fileLoc = 0;                       // chip index containing the current folder, or -1
static double g_chipSelPos = 0;                 // animated position of the location-chip highlight
static double g_chipSelTarget = 0;

// The chips shown in file mode: the six profile locations above, then one per ready drive. Built
// on entering file mode (RebuildFileChips), so the drive list is current without re-scanning every
// frame. g_fileLoc indexes into this.
struct FileChip {
    std::wstring label;  // "Home", "Downloads", … or "C:", "D:", …
    std::wstring path;   // resolved folder (profile location) or drive root ("C:\\")
    CmdGlyph glyph;
    bool isDrive;
};
static std::vector<FileChip> g_chips;
static bool g_fileStartUsed = false;  // the "start drive" setting is honoured once per mod session
static std::wstring g_fileCur;                  // folder being browsed (or kPcRoot: the drives view)
static std::wstring g_fileDir;                  // folder currently listed ("" = needs refresh)

static std::vector<std::wstring> ReadyDrives();  // defined with the drive helpers further down

// True while the drives overview ("This PC") is showing. It is a synthetic listing of drives, not
// a real folder, so folder actions (rename, delete, new, deep search, path editing) are skipped.
static bool InPcRoot() { return g_fileCur == kPcRoot; }
static std::vector<FileEntry> g_fileEntries;    // its children (capped, see kMaxListed*)
static bool g_fileListTrunc = false;            // the listing hit the cap
static size_t g_fileListTotal = 0;              // entries the folder has (listing cap included)
static std::vector<std::wstring> g_fileBack;    // navigation history (back)
static std::vector<std::wstring> g_fileFwd;     // navigation history (forward)
static std::wstring g_pendingSelect;            // path to highlight after navigating
static std::vector<FileEntry> g_fileDeep;       // deep search results under g_fileCur
static std::wstring g_fileDeepKey;              // folder + query the deep results belong to
static bool g_fileDeepTrunc = false;
static bool g_fileDeepDone = false;             // the deep search for g_fileDeepKey has finished
static bool g_fileSearching = false;

// Global "find:" results: a separate, multi-root search that does not touch the file browser list.
static std::vector<FileEntry> g_globalFiles;
static std::wstring g_globalKey;                // query the current global results belong to
static volatile LONG g_globalSearchGen = 0;     // cancels an older global search
static volatile LONG g_globalSearchRunning = 0; // threads still alive
static HANDLE g_globalSearchIdle = nullptr;     // manual-reset, signaled when none is alive
static bool g_globalSearching = false;
static bool g_globalDone = false;
static bool g_globalTrunc = false;
static std::vector<FileEntry>* g_fileSrc = &g_fileEntries;  // vector SearchResult::icon indexes into
static volatile LONG g_fileSearchGen = 0;
static volatile LONG g_fileSearchRunning = 0;  // deep-search threads still alive
static HANDLE g_fileSearchIdle = nullptr;      // manual-reset, signaled when none is alive

static void FileSearchStarted() {
    if (InterlockedIncrement(&g_fileSearchRunning) == 1 && g_fileSearchIdle) {
        ResetEvent(g_fileSearchIdle);
    }
}

static void FileSearchFinished() {
    if (InterlockedDecrement(&g_fileSearchRunning) == 0 && g_fileSearchIdle) {
        SetEvent(g_fileSearchIdle);
    }
}

// Decoded previews, keyed by full path and kept in a small LRU. File entries come and go with
// every deep-search snapshot, so the images live outside them.
struct ThumbCacheItem {
    ID2D1Bitmap* bmp = nullptr;
    uint64_t used = 0;
    bool failed = false;
};
static std::unordered_map<std::wstring, ThumbCacheItem> g_thumbCache;  // UI thread only
static std::unordered_set<std::wstring> g_thumbPending;                // queued, UI thread only
static uint64_t g_thumbClock = 0;
static const size_t kThumbCacheMax = 96;
static HANDLE g_thumbThread = nullptr;
static HANDLE g_thumbEvent = nullptr;
static HANDLE g_thumbStop = nullptr;  // manual-reset: asks the preview worker to exit
static CRITICAL_SECTION g_thumbLock;  // guards g_thumbQueue (UI enqueues, worker dequeues)
static std::deque<std::wstring> g_thumbQueue;

static std::vector<FileEntry>& FileSrc() { return *g_fileSrc; }

// ---- Query filters: *.pdf  ext:png,jpg  type:image  dir:  file:  (combinable with name text) ----
struct FileFilter {
    bool onlyDirs = false;
    bool onlyFiles = false;
    std::vector<std::wstring> exts;  // lowercase, without the dot
    std::vector<std::wstring> tags;  // what the path bar shows for the active filters
    bool hasSize = false;
    unsigned long long minSize = 0;
    unsigned long long maxSize = ~0ull;
    bool hasDate = false;
    bool hasAfter = false;
    bool hasBefore = false;
    unsigned long long afterTime = 0;  // 64-bit FILETIME value
    unsigned long long beforeTime = 0;
    bool Active() const {
        return onlyDirs || onlyFiles || !exts.empty() || hasSize || hasDate;
    }
};
static FileFilter g_fileFilter;

static void AddFilterExt(FileFilter& f, std::wstring e) {
    while (!e.empty() && e.front() == L'.') e.erase(e.begin());
    if (e.empty()) return;
    if (std::find(f.exts.begin(), f.exts.end(), e) == f.exts.end()) f.exts.push_back(e);
}

static void AddFilterExtList(FileFilter& f, const std::wstring& list, bool tag) {
    std::wstring cur;
    auto flush = [&] {
        if (!cur.empty()) {
            AddFilterExt(f, cur);
            if (tag) f.tags.push_back(L"." + cur);
        }
        cur.clear();
    };
    for (wchar_t c : list) {
        if (c == L',' || c == L';' || c == L'|') flush();
        else cur.push_back(c);
    }
    flush();
}

static bool CsvHas(const wchar_t* csv, const std::wstring& w) {
    const std::wstring all = std::wstring(L",") + csv + L",";
    return all.find(L"," + w + L",") != std::wstring::npos;
}

// "type:image" style categories; returns false when the word is not a known category.
static bool AddFilterType(FileFilter& f, const std::wstring& w) {
    static const struct {
        const wchar_t* names;
        const wchar_t* exts;
        const wchar_t* tag;
    } kTypes[] = {
        {L"image,images,img,photo,photos,picture,pictures",
         L"png,jpg,jpeg,gif,bmp,webp,tif,tiff,ico,svg,heic,avif", L"Images"},
        {L"doc,docs,document,documents,text",
         L"pdf,doc,docx,odt,rtf,txt,md,xls,xlsx,csv,ods,ppt,pptx,odp", L"Documents"},
        {L"audio,music,sound,sounds", L"mp3,wav,flac,ogg,m4a,aac,wma,opus", L"Audio"},
        {L"video,videos,movie,movies", L"mp4,mkv,avi,mov,wmv,webm,flv,m4v", L"Video"},
        {L"archive,archives,zip,compressed", L"zip,rar,7z,tar,gz,bz2,xz,tgz", L"Archives"},
        {L"code,source,dev",
         L"c,cpp,h,hpp,cs,js,ts,jsx,tsx,py,java,kt,dart,go,rs,rb,php,html,css,json,xml,yaml,yml,sh,ps1,bat,sql,lua,swift",
         L"Code"},
    };
    for (const auto& t : kTypes) {
        if (CsvHas(t.names, w)) {
            AddFilterExtList(f, t.exts, false);
            f.tags.push_back(t.tag);
            return true;
        }
    }
    return false;
}

// ---- Size and date filters ----
static std::wstring FormatFileSize(uint64_t bytes);  // defined with the row helpers, below
static const unsigned long long kFileTimeDay = 864000000000ull;  // 100 ns ticks in a day

static unsigned long long FileTimeU64(const FILETIME& ft) {
    return ((unsigned long long)ft.dwHighDateTime << 32) | ft.dwLowDateTime;
}

// Local wall-clock time to the UTC instant it names. A FILETIME built straight from a SYSTEMTIME
// treats it as UTC, which would shift every "today" by the timezone offset.
static unsigned long long LocalToUtcTicks(const SYSTEMTIME& local) {
    SYSTEMTIME utc = {};
    FILETIME ft = {};
    if (!TzSpecificLocalTimeToSystemTime(nullptr, &local, &utc)) return 0;
    if (!SystemTimeToFileTime(&utc, &ft)) return 0;
    return FileTimeU64(ft);
}

// Local midnight, `days` before today (negative = after today), as a 64-bit FILETIME value.
static unsigned long long LocalMidnightDaysAgo(int days) {
    SYSTEMTIME st;
    GetLocalTime(&st);
    st.wHour = st.wMinute = st.wSecond = st.wMilliseconds = 0;
    const unsigned long long base = LocalToUtcTicks(st);
    return days >= 0 ? base - (unsigned long long)days * kFileTimeDay
                     : base + (unsigned long long)(-days) * kFileTimeDay;
}

// The same value back as the local calendar date Everything compares against.
static std::wstring TicksToLocalDate(unsigned long long v) {
    FILETIME ft;
    ft.dwLowDateTime = (DWORD)(v & 0xFFFFFFFFull);
    ft.dwHighDateTime = (DWORD)(v >> 32);
    FILETIME local = {};
    SYSTEMTIME st = {};
    if (!FileTimeToLocalFileTime(&ft, &local) || !FileTimeToSystemTime(&local, &st)) {
        return std::wstring();
    }
    wchar_t buf[16] = L"";
    swprintf(buf, ARRAYSIZE(buf), L"%04d-%02d-%02d", st.wYear, st.wMonth, st.wDay);
    return buf;
}

// "500kb", "10mb", "1g" → bytes. Binary multiples, like the sizes the list shows.
static bool ParseSizeValue(const std::wstring& text, unsigned long long& out) {
    size_t i = 0;
    unsigned long long v = 0;
    while (i < text.size() && iswdigit(text[i])) {
        v = v * 10 + (unsigned long long)(text[i] - L'0');
        ++i;
    }
    if (i == 0) return false;
    std::wstring unit = text.substr(i);
    for (auto& c : unit) c = (wchar_t)towlower(c);
    if (unit.empty() || unit == L"b") {
        out = v;
    } else if (unit == L"k" || unit == L"kb") {
        out = v * 1024ull;
    } else if (unit == L"m" || unit == L"mb") {
        out = v * 1024ull * 1024;
    } else if (unit == L"g" || unit == L"gb") {
        out = v * 1024ull * 1024 * 1024;
    } else {
        return false;
    }
    return true;
}

// size:>10mb   size:<500kb   size:1mb..10mb   size:10mb (at least that size)
static bool ParseSizeFilter(FileFilter& f, const std::wstring& v) {
    if (v.empty()) return false;
    std::wstring s = v;
    const size_t range = s.find(L"..");
    if (range != std::wstring::npos) {
        unsigned long long lo = 0, hi = 0;
        if (!ParseSizeValue(s.substr(0, range), lo) || !ParseSizeValue(s.substr(range + 2), hi)) {
            return false;
        }
        f.hasSize = true;
        f.minSize = lo;
        f.maxSize = hi;
        f.tags.push_back(FormatFileSize(lo) + L"–" + FormatFileSize(hi));
        return true;
    }
    bool more = false, less = false;
    if (s[0] == L'>') {
        more = true;
        s.erase(0, 1);
        if (!s.empty() && s[0] == L'=') s.erase(0, 1);
    } else if (s[0] == L'<') {
        less = true;
        s.erase(0, 1);
        if (!s.empty() && s[0] == L'=') s.erase(0, 1);
    }
    unsigned long long val = 0;
    if (!ParseSizeValue(s, val)) return false;
    f.hasSize = true;
    if (less) {
        f.maxSize = std::min(f.maxSize, val > 0 ? val - 1 : 0);
        f.tags.push_back(L"<" + FormatFileSize(val));
    } else {
        f.minSize = std::max(f.minSize, val);  // bare value and ">" both mean "at least"
        f.tags.push_back((more ? L">" : L"≥") + FormatFileSize(val));
    }
    return true;
}

static bool ParseYmd(const std::wstring& s, int& y, int& m, int& d) {
    int parts[3] = {0, 0, 0};
    int sep = 0;
    for (const wchar_t c : s) {
        if (iswdigit(c)) {
            if (sep > 2) return false;
            parts[sep] = parts[sep] * 10 + (c - L'0');
        } else if (c == L'-' || c == L'/') {
            if (++sep > 2) return false;
        } else {
            return false;
        }
    }
    if (sep != 2 || parts[0] < 1000 || parts[1] < 1 || parts[1] > 12 || parts[2] < 1 ||
        parts[2] > 31) {
        return false;
    }
    y = parts[0];
    m = parts[1];
    d = parts[2];
    return true;
}

// modified:today|yesterday|week|month|year|2026-01-31|>2026-01-31|<2026-01-31
static bool ParseDateFilter(FileFilter& f, const std::wstring& v) {
    if (v.empty()) return false;
    std::wstring s = v;
    bool afterOnly = false, beforeOnly = false;
    if (s[0] == L'>') {
        afterOnly = true;
        s.erase(0, 1);
    } else if (s[0] == L'<') {
        beforeOnly = true;
        s.erase(0, 1);
    }
    std::wstring word;
    for (const wchar_t c : s) word.push_back((wchar_t)towlower(c));

    const unsigned long long today = LocalMidnightDaysAgo(0);
    if (word == L"today") {
        f.hasDate = true;
        f.hasAfter = true;
        f.afterTime = today;
        f.hasBefore = true;
        f.beforeTime = today + kFileTimeDay;
        f.tags.push_back(L"Today");
        return true;
    }
    if (word == L"yesterday") {
        f.hasDate = true;
        f.hasAfter = true;
        f.afterTime = today - kFileTimeDay;
        f.hasBefore = true;
        f.beforeTime = today;
        f.tags.push_back(L"Yesterday");
        return true;
    }
    if (word == L"week" || word == L"month" || word == L"year") {
        const int days = word == L"week" ? 7 : (word == L"month" ? 30 : 365);
        f.hasDate = true;
        f.hasAfter = true;
        f.afterTime = LocalMidnightDaysAgo(days);
        f.tags.push_back(days == 7 ? L"Last 7 days" : (days == 30 ? L"Last 30 days" : L"Last year"));
        return true;
    }
    int y = 0, m = 0, d = 0;
    if (!ParseYmd(s, y, m, d)) return false;
    SYSTEMTIME st = {};
    st.wYear = (WORD)y;
    st.wMonth = (WORD)m;
    st.wDay = (WORD)d;
    const unsigned long long t = LocalToUtcTicks(st);
    if (t == 0) return false;
    f.hasDate = true;
    if (afterOnly) {
        f.hasAfter = true;
        f.afterTime = t + kFileTimeDay;
    } else if (beforeOnly) {
        f.hasBefore = true;
        f.beforeTime = t;
    } else {  // that whole day
        f.hasAfter = true;
        f.afterTime = t;
        f.hasBefore = true;
        f.beforeTime = t + kFileTimeDay;
    }
    f.tags.push_back(s);
    return true;
}

// "size:> 1mb" and "modified: today" are what people actually type, but the tokenizer splits them
// into two words. The prefix and its value are glued back before tokenizing.
static std::wstring JoinFilterPrefixes(const std::wstring& q) {
    static const wchar_t* const kPre[] = {L"size:", L"modified:", L"date:"};
    std::wstring out;
    size_t i = 0;
    while (i < q.size()) {
        bool glued = false;
        if (i == 0 || q[i - 1] == L' ' || q[i - 1] == L'\t') {
            for (const wchar_t* p : kPre) {
                const size_t n = wcslen(p);
                if (q.compare(i, n, p) != 0) continue;
                out += p;
                i += n;
                while (i < q.size() && (q[i] == L'>' || q[i] == L'<' || q[i] == L'=')) {
                    out.push_back(q[i++]);
                }
                while (i < q.size() && (q[i] == L' ' || q[i] == L'\t')) ++i;
                glued = true;
                break;
            }
        }
        if (!glued) out.push_back(q[i++]);
    }
    return out;
}

// Splits the typed query into name tokens and filters.
static std::vector<std::wstring> ParseFileQuery(const std::wstring& query, FileFilter& f) {
    f = FileFilter();
    std::vector<std::wstring> out;
    for (const std::wstring& t : Tokenize(JoinFilterPrefixes(query))) {
        auto starts = [&](const wchar_t* pre) {
            const size_t n = wcslen(pre);
            return t.size() >= n && t.compare(0, n, pre) == 0;
        };
        if (starts(L"*.")) {
            AddFilterExtList(f, t.substr(2), true);
        } else if (starts(L"ext:")) {
            AddFilterExtList(f, t.substr(4), true);
        } else if (starts(L"type:")) {
            std::wstring cur;
            const std::wstring v = t.substr(5) + L",";
            for (wchar_t c : v) {
                if (c != L',') {
                    cur.push_back(c);
                    continue;
                }
                if (!cur.empty() && !AddFilterType(f, cur)) AddFilterExtList(f, cur, true);
                cur.clear();
            }
        } else if (starts(L"size:")) {
            // Not a valid size (e.g. "size:big"): keep it as text instead of dropping it.
            if (!ParseSizeFilter(f, t.substr(5))) out.push_back(t);
        } else if (starts(L"modified:") || starts(L"date:")) {
            if (!ParseDateFilter(f, t.substr(t.find(L':') + 1))) out.push_back(t);
        } else if (starts(L"dir:") || starts(L"folder:") || starts(L"folders:")) {
            f.onlyDirs = true;
            const std::wstring rest = t.substr(t.find(L':') + 1);
            if (!rest.empty()) out.push_back(rest);  // "dir:logs" = folders named logs
        } else if (starts(L"file:") || starts(L"files:")) {
            f.onlyFiles = true;
            const std::wstring rest = t.substr(t.find(L':') + 1);
            if (!rest.empty()) out.push_back(rest);
        } else if (t != L"*") {
            out.push_back(t);
        }
    }
    if (f.onlyDirs && f.onlyFiles) f.onlyDirs = f.onlyFiles = false;  // contradictory: ignore both
    if (!f.exts.empty()) f.onlyDirs = false;                           // extensions imply files
    if (f.onlyDirs) f.tags.insert(f.tags.begin(), L"Folders");
    if (f.onlyFiles) f.tags.insert(f.tags.begin(), L"Files");
    return out;
}

static bool NameEndsWithExt(const std::wstring& lower, const std::wstring& ext) {
    if (lower.size() <= ext.size() + 1) return false;
    const size_t off = lower.size() - ext.size();
    return lower[off - 1] == L'.' && lower.compare(off, ext.size(), ext) == 0;
}

// A size or date filter can only be proven against an entry that carries its metadata: a row from
// an index without it (or a folder, which has no size) is left out instead of guessed.
static bool FileFilterMatches(const FileFilter& f, const std::wstring& folded, bool isDir,
                              unsigned long long size, const FILETIME& writeTime, bool noInfo) {
    if (f.onlyDirs && !isDir) return false;
    if (f.onlyFiles && isDir) return false;
    if (!f.exts.empty()) {
        if (isDir) return false;
        bool hit = false;
        for (const auto& e : f.exts) {
            if (NameEndsWithExt(folded, e)) {
                hit = true;
                break;
            }
        }
        if (!hit) return false;
    }
    if (f.hasSize) {
        if (noInfo) return false;
        const unsigned long long sz = isDir ? 0 : size;
        if (sz < f.minSize || sz > f.maxSize) return false;
    }
    if (f.hasDate) {
        if (noInfo) return false;
        const unsigned long long t = FileTimeU64(writeTime);
        if (f.hasAfter && t < f.afterTime) return false;
        if (f.hasBefore && t >= f.beforeTime) return false;
    }
    return true;
}

static bool FileFilterMatches(const FileFilter& f, const FileEntry& e) {
    return FileFilterMatches(f, e.folded, e.isDir, e.size, e.writeTime, e.noInfo);
}

// ---- Band editor (path / rename) and the file actions menu ----
enum class BandEdit { None, Path, Rename };
static BandEdit g_bandEdit = BandEdit::None;
static std::wstring g_bandText;
static int g_bandCaret = 0, g_bandAnchor = 0;
static float g_bandScroll = 0;
static std::wstring g_bandTarget;  // Rename: full path being renamed
static std::wstring g_bandError;
static double g_bandErrorAt = 0;
static std::vector<std::wstring> g_bandComp;  // Tab-completion candidates (folder names)
static std::wstring g_bandCompDir, g_bandCompLast;
static int g_bandCompIdx = -1;

enum FileActId {
    FA_OPEN = 1, FA_GOTO, FA_REVEAL, FA_ADMIN, FA_TERMINAL, FA_RENAME,
    FA_COPYPATH, FA_COPYNAME, FA_COPYFILE, FA_NEWFOLDER, FA_NEWFILE, FA_PROPS, FA_DELETE,
    FA_SORTMENU,
    // Sort rows (the ids map to FileSort by offset, see FileMenuRunSel).
    FS_NAME_ASC = 100, FS_NAME_DESC, FS_TYPE, FS_MOD_NEW, FS_MOD_OLD, FS_SIZE_BIG, FS_SIZE_SMALL
};
struct FileMenuItem {
    int id;
    const wchar_t* label;
    const wchar_t* keys;
};
static std::vector<FileMenuItem> g_fileMenu;
static bool g_fileMenuOpen = false;
static int g_fileMenuSel = 0;
static bool g_fileMenuArmed = false;       // delete is waiting for its confirming Enter
static FileEntry g_fileMenuTarget;         // copy: results can change while the menu is open
static bool g_fileMenuHasTarget = false;
static double g_fileCopiedAt = -1e9;       // last "copy path/name/file", for the footer flash

static bool FileQueryActive() { return !g_tokens.empty() || g_fileFilter.Active(); }

static std::wstring FileFilterSummary() {
    std::wstring s;
    for (const auto& t : g_fileFilter.tags) {
        if (!s.empty()) s += L" · ";
        s += t;
    }
    return s;
}

static void ReleaseThumbCache() {
    for (auto& kv : g_thumbCache) SafeRelease(kv.second.bmp);
    g_thumbCache.clear();
    g_thumbPending.clear();
}

// Drops a cached preview (or a pending request) for one path, e.g. after a rename or a delete.
static void ForgetFileThumb(const std::wstring& path) {
    auto it = g_thumbCache.find(path);
    if (it != g_thumbCache.end()) {
        SafeRelease(it->second.bmp);
        g_thumbCache.erase(it);
    }
    g_thumbPending.erase(path);
}

// Drops previews still waiting to be decoded (e.g. after navigating away or closing the bar),
// so a folder the user left cannot delay the one now on screen.
static void CancelThumbQueue() {
    if (!g_thumbThread) return;
    EnterCriticalSection(&g_thumbLock);
    g_thumbQueue.clear();
    LeaveCriticalSection(&g_thumbLock);
    g_thumbPending.clear();
}

static void ClearFileDeep() {
    g_fileDeep.clear();
    g_fileDeepKey.clear();
    g_fileDeepTrunc = false;
    g_fileDeepDone = false;
}

static std::wstring KnownFolderPath(REFKNOWNFOLDERID id);  // defined further down

// Resolves the folder of a file location, or an empty string on failure. The known folder is
// preferred so a moved or redirected folder is followed to wherever it now lives (even another
// drive); %USERPROFILE%\sub is only the fallback.
static std::wstring ResolveFileLocation(int loc) {
    if (loc < 0 || loc >= kFileLocationCount) return L"";
    if (kFileLocations[loc].folder) {
        std::wstring kf = KnownFolderPath(*kFileLocations[loc].folder);
        if (!kf.empty()) return kf;
    }
    wchar_t base[MAX_PATH];
    DWORD n = GetEnvironmentVariableW(L"USERPROFILE", base, MAX_PATH);
    if (n == 0 || n >= MAX_PATH) return L"";
    std::wstring dir = base;
    const wchar_t* sub = kFileLocations[loc].sub;
    if (sub && *sub) {
        dir += L'\\';
        dir += sub;
    }
    return dir;
}

static std::wstring JoinPath(const std::wstring& dir, const std::wstring& name) {
    if (dir.empty()) return name;
    return (dir.back() == L'\\') ? dir + name : dir + L'\\' + name;
}

// Parent folder, or "" when there is none (drive root).
static std::wstring ParentDir(const std::wstring& p) {
    std::wstring t = p;
    while (t.size() > 3 && t.back() == L'\\') t.pop_back();
    if (t.size() <= 3) return L"";
    const size_t i = t.find_last_of(L'\\');
    if (i == std::wstring::npos) return L"";
    std::wstring r = t.substr(0, i);
    if (r.size() == 2 && r[1] == L':') r += L'\\';
    return r;
}

static bool SamePath(const std::wstring& a, const std::wstring& b) {
    auto trim = [](std::wstring t) {
        while (t.size() > 3 && t.back() == L'\\') t.pop_back();
        return t;
    };
    return _wcsicmp(trim(a).c_str(), trim(b).c_str()) == 0;
}

// Chip whose folder is the deepest ancestor of (or equal to) the path, or -1. Works for both the
// profile locations and the drive roots (a drive chip's path ends in "\\", so it is compared
// without the trailing separator; a folder inside the profile still wins over its drive chip).
static int LocationForPath(const std::wstring& path) {
    int best = -1;
    size_t bestLen = 0;
    for (int i = 0; i < (int)g_chips.size(); ++i) {
        std::wstring loc = g_chips[i].path;
        while (loc.size() > 2 && loc.back() == L'\\') loc.pop_back();  // "C:\" -> "C:"
        if (loc.empty() || path.size() < loc.size()) continue;
        if (_wcsnicmp(path.c_str(), loc.c_str(), loc.size()) != 0) continue;
        if (path.size() > loc.size() && path[loc.size()] != L'\\') continue;
        if (loc.size() > bestLen) {
            best = i;
            bestLen = loc.size();
        }
    }
    return best;
}

// Short human-readable size for files.
static std::wstring FormatFileSize(uint64_t bytes) {
    const wchar_t* units[] = {L"KB", L"MB", L"GB", L"TB"};
    if (bytes < 1024) {
        wchar_t buf[32];
        swprintf_s(buf, L"%llu B", (unsigned long long)bytes);
        return buf;
    }
    double v = (double)bytes / 1024.0;
    int u = 0;
    while (v >= 1024.0 && u < 3) {
        v /= 1024.0;
        ++u;
    }
    wchar_t buf[32];
    if (v < 10.0) {
        swprintf_s(buf, L"%.1f %s", v, units[u]);
    } else {
        swprintf_s(buf, L"%.0f %s", v, units[u]);
    }
    return buf;
}

// Subtitle for a drive row in the drives overview: volume label and free/total space when they can
// be read. The error mode is suppressed per thread so a slow or unready drive cannot pop a dialog.
static std::wstring DriveMetaLine(const std::wstring& root) {
    DWORD prevMode = 0;
    SetThreadErrorMode(SEM_FAILCRITICALERRORS, &prevMode);
    wchar_t label[MAX_PATH] = {};
    GetVolumeInformationW(root.c_str(), label, MAX_PATH, nullptr, nullptr, nullptr, nullptr, 0);
    ULARGE_INTEGER freeB = {}, totB = {};
    const BOOL okSpace = GetDiskFreeSpaceExW(root.c_str(), nullptr, &totB, &freeB);
    SetThreadErrorMode(prevMode, nullptr);
    std::wstring name = label[0] ? label : L"Drive";
    if (okSpace && totB.QuadPart) {
        return name + L" · " + FormatFileSize(freeB.QuadPart) + L" free of " +
               FormatFileSize(totB.QuadPart);
    }
    return name;
}

// Uppercase extension of a file name (without the dot); empty when it has none.
static std::wstring FileExtOf(const std::wstring& name) {
    const size_t d = name.find_last_of(L'.');
    if (d == std::wstring::npos || d == 0 || d + 1 >= name.size()) return L"";
    std::wstring e = name.substr(d + 1);
    if (e.size() > 4) return L"";  // too long to be a real extension
    for (auto& c : e) c = (wchar_t)towupper(c);
    return e;
}

static bool IsImageExt(const std::wstring& ext) {
    return ext == L"PNG" || ext == L"JPG" || ext == L"JPEG" || ext == L"JPE" ||
           ext == L"JFIF" || ext == L"GIF" || ext == L"BMP" || ext == L"WEBP" || ext == L"TIF" ||
           ext == L"TIFF" || ext == L"ICO" || ext == L"HEIC" || ext == L"HEIF" ||
           ext == L"AVIF" || ext == L"JXR" || ext == L"HDP" || ext == L"WDP";
}

static bool SkipFindEntry(const WIN32_FIND_DATAW& fd) {
    if (fd.cFileName[0] == L'.' &&
        (fd.cFileName[1] == 0 || (fd.cFileName[1] == L'.' && fd.cFileName[2] == 0))) {
        return true;
    }
    if (fd.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) return true;
    // Hidden entries stay out (AppData, ...), except dot-folders/files such as .claude or .vscode.
    return (fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) && fd.cFileName[0] != L'.';
}

static FileEntry MakeFileEntry(const WIN32_FIND_DATAW& fd, const std::wstring& dir) {
    FileEntry e;
    e.name = fd.cFileName;
    e.folded = FoldString(e.name);
    e.isDir = (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
    e.attr = fd.dwFileAttributes;
    e.dir = dir;
    e.path = JoinPath(dir, e.name);
    if (!e.isDir) e.ext = FileExtOf(e.name);
    ULARGE_INTEGER sz;
    sz.LowPart = fd.nFileSizeLow;
    sz.HighPart = fd.nFileSizeHigh;
    e.size = sz.QuadPart;
    e.writeTime = fd.ftLastWriteTime;
    return e;
}

// The "Folder · 18/10/2026 16:02" line, formatted the first time the entry is shown. Listing a
// large folder no longer pays for a date/size format per file.
// "File · 1.2 MB · 18/10/2026 16:02" (folders carry no size). Built without caching so callers that
// only hold a const entry can use it too.
static std::wstring FileMetaText(const FileEntry& e) {
    if (e.noInfo) {  // no size or date from the index: the type alone, never 0 B / 1601
        return e.isDir ? kTxtFolder : kTxtKindFile;
    }
    return std::wstring(e.isDir ? kTxtFolder : kTxtKindFile) + L" · " +
           (e.isDir ? std::wstring() : FormatFileSize(e.size) + L" · ") +
           FormatFileTime(e.writeTime);
}

static const std::wstring& FileEntryMeta(FileEntry& e) {
    if (e.meta.empty()) e.meta = FileMetaText(e);
    return e.meta;
}

// A folder with hundreds of thousands of entries would freeze the bar and eat memory just to
// show a list nobody scrolls to the end of. The listing stops at these caps (the deep search
// still looks past them) and the path band says so.
static const size_t kMaxListedDirs = 5000;
static const size_t kMaxListedFiles = 5000;

// Reads the children of g_fileCur into g_fileEntries (folders first).
// Ordinal comparison of the folded names: no per-comparison locale collation.
static int CompareFileNames(const FileEntry& a, const FileEntry& b) {
    const int c = CompareStringOrdinal(a.folded.c_str(), -1, b.folded.c_str(), -1, TRUE);
    if (c == CSTR_EQUAL) {
        const int n = wcscmp(a.name.c_str(), b.name.c_str());
        return n < 0 ? -1 : (n > 0 ? 1 : 0);
    }
    return c == CSTR_LESS_THAN ? -1 : 1;
}

// The Files order: folders always first, then the chosen order, ties by name so the list never
// jumps around between refreshes.
static bool FileEntryLess(const FileEntry& a, const FileEntry& b) {
    if (a.isDir != b.isDir) return a.isDir;
    switch (g_cfg.fileSort) {
        case FileSort::NameDesc:
            return CompareFileNames(a, b) > 0;
        case FileSort::Type: {
            const int c = CompareStringOrdinal(a.ext.c_str(), -1, b.ext.c_str(), -1, TRUE);
            if (c != CSTR_EQUAL) return c == CSTR_LESS_THAN;
            break;
        }
        case FileSort::ModNew:
        case FileSort::ModOld: {
            const int c = CompareFileTime(&a.writeTime, &b.writeTime);
            if (c != 0) return g_cfg.fileSort == FileSort::ModNew ? c > 0 : c < 0;
            break;
        }
        case FileSort::SizeBig:
        case FileSort::SizeSmall:
            if (a.size != b.size) {
                return g_cfg.fileSort == FileSort::SizeBig ? a.size > b.size : a.size < b.size;
            }
            break;
        case FileSort::NameAsc:
            return CompareFileNames(a, b) < 0;
    }
    return CompareFileNames(a, b) < 0;
}

static void RefreshFileListing() {
    CancelThumbQueue();
    g_fileEntries.clear();
    g_fileListTrunc = false;
    g_fileListTotal = 0;
    g_fileDir = g_fileCur;
    if (g_fileDir.empty()) return;
    if (g_fileDir == kPcRoot) {
        // The drives overview: one row per ready drive instead of a folder listing.
        for (const std::wstring& root : ReadyDrives()) {
            FileEntry e;
            e.name = root.substr(0, 2);  // "C:"
            e.folded = FoldString(e.name);
            e.path = root;               // "C:\"
            e.dir = kPcRoot;
            e.isDir = true;
            e.attr = FILE_ATTRIBUTE_DIRECTORY;
            e.meta = DriveMetaLine(root);
            g_fileEntries.push_back(std::move(e));
        }
        g_fileListTotal = g_fileEntries.size();
        return;
    }
    const std::wstring pattern = JoinPath(g_fileDir, L"*");
    WIN32_FIND_DATAW fd;
    HANDLE h = FindFirstFileExW(pattern.c_str(), FindExInfoBasic, &fd, FindExSearchNameMatch,
                               nullptr, FIND_FIRST_EX_LARGE_FETCH);
    if (h == INVALID_HANDLE_VALUE) return;
    size_t dirs = 0, files = 0, dropped = 0;
    do {
        if (SkipFindEntry(fd)) continue;
        const bool isDir = (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
        if (isDir && dirs >= kMaxListedDirs) {
            g_fileListTrunc = true;
            ++dropped;
            continue;
        }
        if (!isDir && files >= kMaxListedFiles) {
            g_fileListTrunc = true;
            ++dropped;
            continue;
        }
        if (isDir) ++dirs;
        else ++files;
        g_fileEntries.push_back(MakeFileEntry(fd, g_fileDir));
    } while (FindNextFileW(h, &fd));
    FindClose(h);
    g_fileListTotal = g_fileEntries.size() + dropped;
    std::sort(g_fileEntries.begin(), g_fileEntries.end(), FileEntryLess);
}

static std::wstring FileSearchKey() {
    std::wstring k = g_fileCur;
    k += L'\n';
    for (const auto& t : g_tokens) {
        k += t;
        k += L' ';
    }
    k += L'\n';
    k += g_fileFilter.onlyDirs ? L'd' : L'-';
    k += g_fileFilter.onlyFiles ? L'f' : L'-';
    for (const auto& e : g_fileFilter.exts) {
        k += L' ';
        k += e;
    }
    if (g_fileFilter.hasSize) {
        k += L'|';
        k += std::to_wstring(g_fileFilter.minSize);
        k += L'-';
        k += std::to_wstring(g_fileFilter.maxSize);
    }
    if (g_fileFilter.hasAfter) {
        k += L'|';
        k += std::to_wstring(g_fileFilter.afterTime);
    }
    if (g_fileFilter.hasBefore) {
        k += L'|';
        k += std::to_wstring(g_fileFilter.beforeTime);
    }
    return k;
}

// Builds the result list for file mode: the folder's children, or the deep matches when
// a query is typed (local matches are shown until the deep search finishes).
static void BuildFileResults() {
    g_results.clear();
    const bool deep = FileQueryActive() && g_fileDeepKey == FileSearchKey();
    g_fileSrc = deep ? &g_fileDeep : &g_fileEntries;
    std::vector<FileEntry>& src = *g_fileSrc;
    for (size_t i = 0; i < src.size(); ++i) {
        FileEntry& e = src[i];
        if (!deep) {
            if (!FileFilterMatches(g_fileFilter, e)) continue;
            bool ok = true;
            for (const auto& t : g_tokens) {
                if (e.folded.find(t) == std::wstring::npos) {
                    ok = false;
                    break;
                }
            }
            if (!ok) continue;
        }
        SearchResult r;
        r.kind = ResultKind::File;
        r.title = e.name;
        r.subtitle = FileEntryMeta(e);
        r.payload = e.path;
        r.icon = (int)i;  // index into FileSrc()
        g_results.push_back(std::move(r));
        if ((int)g_results.size() >= 300) break;
    }
}

// ---- Deep search (background, breadth-first so shallow matches come first) ----
struct FileSearchJob {
    HWND hwnd;
    LONG gen;
    std::wstring root;
    std::wstring key;
    std::vector<std::wstring> tokens;
    FileFilter filter;
};

struct FileSearchResult {
    LONG gen;
    std::wstring key;
    std::vector<FileEntry> entries;
    bool truncated;
    bool done;  // false for a partial snapshot sent while the search is still running
};

static DWORD WINAPI FileSearchThread(LPVOID param) {
    std::unique_ptr<FileSearchJob> job((FileSearchJob*)param);
    struct DoneGuard {
        ~DoneGuard() { FileSearchFinished(); }
    } doneGuard;
    const size_t kMaxHits = 300, kMaxScanned = 400000;  // matches the 300 rows BuildFileResults keeps
    const int kMaxDepth = 14;
    const DWORD kBudgetMs = 6000;
    struct Item {
        std::wstring dir;
        int depth;
    };
    struct Hit {
        FileEntry e;
        int score;
    };
    std::vector<Item> queue;
    std::vector<Hit> hits;
    queue.push_back({job->root, 0});
    size_t qi = 0, scanned = 0;
    bool truncated = false;
    const DWORD t0 = GetTickCount();
    DWORD lastPost = t0;
    auto cancelled = [&] { return job->gen != g_fileSearchGen; };
    auto sortHits = [&] {
        std::sort(hits.begin(), hits.end(), [](const Hit& a, const Hit& b) {
            if (a.score != b.score) return a.score > b.score;
            return CompareStringOrdinal(a.e.folded.c_str(), -1, b.e.folded.c_str(), -1, TRUE) ==
                   CSTR_LESS_THAN;
        });
    };
    // Sends the current hits so results appear while the search keeps running.
    auto post = [&](bool isFinal) {
        sortHits();
        auto* res = new FileSearchResult();
        res->gen = job->gen;
        res->key = job->key;
        res->truncated = truncated;
        res->done = isFinal;
        res->entries.reserve(hits.size());
        for (const auto& ht : hits) res->entries.push_back(ht.e);
        if (!PostMessageW(job->hwnd, WM_APP_FILES, 0, (LPARAM)res)) delete res;
    };

    while (qi < queue.size() && !truncated) {
        if (cancelled()) return 0;
        if (!hits.empty() && GetTickCount() - lastPost > 250) {
            post(false);
            lastPost = GetTickCount();
        }
        const Item it = queue[qi++];
        WIN32_FIND_DATAW fd;
        HANDLE h = FindFirstFileExW(JoinPath(it.dir, L"*").c_str(), FindExInfoBasic, &fd,
                                    FindExSearchNameMatch, nullptr, FIND_FIRST_EX_LARGE_FETCH);
        if (h == INVALID_HANDLE_VALUE) continue;
        do {
            if (SkipFindEntry(fd)) continue;
            ++scanned;
            if ((scanned & 255) == 0) {
                if (cancelled()) {
                    FindClose(h);
                    return 0;
                }
                if (GetTickCount() - t0 > kBudgetMs || scanned > kMaxScanned) {
                    truncated = true;
                    break;
                }
            }
            const bool isDir = (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
            const std::wstring folded = FoldString(fd.cFileName);
            ULARGE_INTEGER fsz;
            fsz.LowPart = fd.nFileSizeLow;
            fsz.HighPart = fd.nFileSizeHigh;
            bool ok = FileFilterMatches(job->filter, folded, isDir, fsz.QuadPart,
                                        fd.ftLastWriteTime, false);
            int score = 100;
            for (size_t k = 0; ok && k < job->tokens.size(); ++k) {
                const size_t pos = folded.find(job->tokens[k]);
                if (pos == std::wstring::npos) {
                    ok = false;
                    break;
                }
                if (k == 0) {
                    if (pos == 0) score = 300;
                    else if (!iswalnum(folded[pos - 1])) score = 200;
                }
            }
            if (ok) {
                Hit ht;
                ht.e = MakeFileEntry(fd, it.dir);
                FileEntryMeta(ht.e);  // formatted here, off the UI thread, and kept in snapshots
                ht.score = score + (isDir ? 20 : 0) - it.depth * 15;
                hits.push_back(std::move(ht));
                if (hits.size() >= kMaxHits) {
                    truncated = true;
                    break;
                }
            }
            if (isDir && it.depth < kMaxDepth) {
                const bool link = (fd.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) &&
                                  (fd.dwReserved0 == 0xA000000C || fd.dwReserved0 == 0xA0000003);
                // Offline/cloud folders may download on access: do not walk into them.
                const bool cloud =
                    (fd.dwFileAttributes &
                     (FILE_ATTRIBUTE_OFFLINE | FILE_ATTRIBUTE_RECALL_ON_DATA_ACCESS)) != 0;
                const std::wstring fn = folded;
                if (!link && !cloud && fn != L"node_modules" && fn != L".git") {
                    queue.push_back({JoinPath(it.dir, fd.cFileName), it.depth + 1});
                }
            }
        } while (FindNextFileW(h, &fd));
        FindClose(h);
    }
    if (cancelled()) return 0;

    post(true);
    return 0;
}

// Brand-ish colour for a file-type tile, by extension.
static D2D1_COLOR_F ExtColor(const std::wstring& ext) {
    auto c = [](float r, float g, float b) { return D2D1::ColorF(r, g, b); };
    if (ext == L"PDF") return c(0.86f, 0.29f, 0.24f);
    if (ext == L"DOC" || ext == L"DOCX" || ext == L"RTF" || ext == L"ODT") return c(0.24f, 0.46f, 0.85f);
    if (ext == L"XLS" || ext == L"XLSX" || ext == L"CSV" || ext == L"ODS") return c(0.20f, 0.62f, 0.33f);
    if (ext == L"PPT" || ext == L"PPTX" || ext == L"ODP") return c(0.90f, 0.47f, 0.20f);
    if (ext == L"JSON" || ext == L"XML" || ext == L"YAML" || ext == L"YML" || ext == L"INI")
        return c(0.80f, 0.60f, 0.20f);
    if (ext == L"TXT" || ext == L"MD" || ext == L"LOG") return c(0.44f, 0.47f, 0.55f);
    if (ext == L"ZIP" || ext == L"RAR" || ext == L"7Z" || ext == L"TAR" || ext == L"GZ")
        return c(0.72f, 0.52f, 0.28f);
    if (ext == L"EXE" || ext == L"MSI" || ext == L"BAT" || ext == L"CMD" || ext == L"PS1")
        return c(0.40f, 0.43f, 0.50f);
    if (ext == L"MP3" || ext == L"WAV" || ext == L"FLAC" || ext == L"OGG" || ext == L"M4A" ||
        ext == L"AAC" || ext == L"WMA")
        return c(0.60f, 0.36f, 0.80f);
    if (ext == L"MP4" || ext == L"MKV" || ext == L"AVI" || ext == L"MOV" || ext == L"WMV" ||
        ext == L"WEBM" || ext == L"FLV")
        return c(0.85f, 0.36f, 0.55f);
    if (ext == L"C" || ext == L"CPP" || ext == L"H" || ext == L"HPP" || ext == L"CS" || ext == L"JAVA" ||
        ext == L"PY" || ext == L"JS" || ext == L"TS" || ext == L"HTML" || ext == L"CSS" || ext == L"RS" ||
        ext == L"GO" || ext == L"SH" || ext == L"PHP" || ext == L"RB" || ext == L"SQL")
        return c(0.42f, 0.45f, 0.85f);
    if (IsImageExt(ext)) return c(0.25f, 0.62f, 0.62f);
    return c(0.44f, 0.47f, 0.54f);
}

static bool StartsWithTrigger(const std::wstring& text, const wchar_t* trigger) {
    const size_t n = wcslen(trigger);
    if (text.size() < n) return false;
    for (size_t i = 0; i < n; ++i) {
        if (towlower(text[i]) != towlower(trigger[i])) return false;
    }
    return true;
}

// A prefix only exists while its feature is enabled; otherwise the text stays a normal query.
static bool ActivatorEnabled(const ActivatorDef& a) {
    switch (a.mode) {
        case LauncherMode::Web: return g_cfg.webSearch;
        case LauncherMode::Bookmarks: return g_cfg.bookmarks;
        case LauncherMode::Clipboard: return g_cfg.clipboardHistory;
        case LauncherMode::Recent: return g_cfg.recentFiles;
        case LauncherMode::GlobalFiles: return true;
        case LauncherMode::WindowList: return true;
        case LauncherMode::Commands: return true;
    }
    return true;
}

// Detects a leading activator, stores it in g_activator and returns the remaining query (trimmed).
// With no activator, g_activator is -1 and the trimmed text is returned unchanged.
static std::wstring ActivationQuery(const std::wstring& text) {
    const int prev = g_activator;
    g_activator = -1;
    const size_t a = text.find_first_not_of(L" \t");
    if (a == std::wstring::npos) return std::wstring();
    std::wstring t = text.substr(a);
    const size_t b = t.find_last_not_of(L" \t");
    t = t.substr(0, b + 1);
    for (int i = 0; i < kActivatorCount; ++i) {
        if (!ActivatorEnabled(kActivators[i])) continue;
        if (StartsWithTrigger(t, kActivators[i].trigger)) {
            g_activator = i;
            // Entering "win:" refreshes the window list even if the bar stayed open.
            if (kActivators[i].mode == LauncherMode::WindowList &&
                (prev < 0 || kActivators[prev].mode != LauncherMode::WindowList)) {
                g_windowsDirty = true;
            }
            std::wstring q = t.substr(wcslen(kActivators[i].trigger));
            const size_t qa = q.find_first_not_of(L" \t");
            return qa == std::wstring::npos ? std::wstring() : q.substr(qa);
        }
    }
    return t;
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

// Expanded mode with an empty search bar: "Pinned" (apps the user kept), "Recent" (last opened
// apps), "Recent files" (Windows' Recent list) and "Suggestions" (by frecency and, so the list is
// never empty, common system apps). All entries are returned; the list scrolls inside the fixed
// panel height.
static SearchResult MakeRecentResult(const RecentEntry& r);  // defined with the search helpers

static void BuildHome() {
    std::unordered_set<int> taken;

    std::vector<int> pinned;
    for (size_t i = 0; i < g_apps.size(); ++i) {
        if (g_apps[i].pinned) pinned.push_back((int)i);
    }
    std::sort(pinned.begin(), pinned.end(), [](int a, int b) {
        return g_apps[a].folded < g_apps[b].folded;
    });
    for (int i : pinned) {
        SearchResult r;
        r.app = i;
        r.section = 3;
        g_results.push_back(r);
        taken.insert(i);
    }

    std::vector<SearchResult> rec;
    for (size_t i = 0; i < g_apps.size(); ++i) {
        if (g_apps[i].lastUsed > 0 && !taken.count((int)i)) {
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
    for (const auto& r : rec) taken.insert(r.app);

    // Recent files: after the apps, before the suggestions, so they are visible without scrolling.
    if (g_cfg.recentFiles) {
        for (size_t i = 0; i < g_recent.size() && i < kMaxRecentFiles; ++i) {
            SearchResult r = MakeRecentResult(g_recent[i]);
            r.section = 4;
            g_results.push_back(std::move(r));
        }
    }

    // Suggestions: first by frecency, then the common system apps.
    std::vector<int> pool;
    {
        std::vector<SearchResult> byRank;
        for (size_t i = 0; i < g_apps.size(); ++i) {
            if (g_apps[i].uses > 0 && !taken.count((int)i)) {
                SearchResult r;
                r.app = (int)i;
                r.score = FrecencyScore(g_apps[i]);
                byRank.push_back(r);
            }
        }
        std::sort(byRank.begin(), byRank.end(), [](const SearchResult& a, const SearchResult& b) {
            if (a.score != b.score) return a.score > b.score;
            return g_apps[a.app].folded < g_apps[b.app].folded;
        });
        for (const auto& r : byRank) {
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

// The drive chips sit on their own second row, kept apart from the location chips. The row only
// exists when there are drive chips (i.e. 2+ drives; see RebuildFileChips).
static bool HasDriveChips() {
    for (const auto& c : g_chips) {
        if (c.isDrive) return true;
    }
    return false;
}

// Height (DIP) of the chip row(s) above the path band: the location row, plus the drive row when
// it is shown.
static float FileChipRowsH() {
    return kChipH + (HasDriveChips() ? kDriveRowH : 0.f);
}

// Height (DIP) of the whole file-mode band (chip rows + path band) above the list.
static double FileChipsH() {
    return g_fileMode ? FileChipRowsH() + kPathH : 0.0;
}

// True when the calculation is actually visible (card or inline text).
static bool CalcShown() {
    return HeroShown() || GhostShown();
}

static void UpdateCalc() {
    g_calc = Calc::Result();
    g_calcDisplay.clear();
    g_calcActive = false;
    ActivationQuery(g_text);  // keep g_activator in sync with what is typed
    if (!g_cfg.calculatorEnabled || g_text.empty() || g_activator >= 0 || g_fileMode) {
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

// What Enter copies: the bare number, so it pastes cleanly into other apps. The display text
// adds thousands separators, the unit and (for data sizes) the other reading in brackets. Base
// conversions are the exception: their digits only make sense as shown (e.g. "0xFF").
static std::wstring CalcClipboardText() {
    if (!g_calc.display.empty() && g_calc.insertable == g_calc.display) {
        return g_calc.display;
    }
    Calc::Locale plain = Calc::SystemLocale();
    plain.grouping = false;
    return Calc::Format(g_calc.value, plain);
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

// ===== Protection at rest =====
// Windhawk storage is machine-wide: another account on the same PC can read what this one saved.
// Anything private (the clipboard history, in particular) is therefore encrypted with DPAPI for the
// current Windows account before it is written, and decrypted only to be shown or pasted.
static bool ProtectBytes(const std::vector<BYTE>& in, std::vector<BYTE>& out) {
    DATA_BLOB src = {(DWORD)in.size(), (BYTE*)in.data()};
    DATA_BLOB dst = {};
    if (!CryptProtectData(&src, L"Spotlight Launcher", nullptr, nullptr, nullptr,
                          CRYPTPROTECT_UI_FORBIDDEN, &dst)) {
        return false;
    }
    out.assign(dst.pbData, dst.pbData + dst.cbData);
    LocalFree(dst.pbData);
    return true;
}

static bool UnprotectBytes(const std::vector<BYTE>& in, std::vector<BYTE>& out) {
    DATA_BLOB src = {(DWORD)in.size(), (BYTE*)in.data()};
    DATA_BLOB dst = {};
    if (!CryptUnprotectData(&src, nullptr, nullptr, nullptr, nullptr,
                            CRYPTPROTECT_UI_FORBIDDEN, &dst)) {
        return false;
    }
    out.assign(dst.pbData, dst.pbData + dst.cbData);
    LocalFree(dst.pbData);
    return true;
}

// The storage API only takes strings, and ciphertext is binary: base64 keeps it there.
static std::string Base64Encode(const std::vector<BYTE>& in) {
    static const char* const kAlphabet =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::string out;
    out.reserve((in.size() + 2) / 3 * 4);
    for (size_t i = 0; i < in.size(); i += 3) {
        const uint32_t b0 = in[i];
        const uint32_t b1 = i + 1 < in.size() ? in[i + 1] : 0;
        const uint32_t b2 = i + 2 < in.size() ? in[i + 2] : 0;
        const uint32_t v = (b0 << 16) | (b1 << 8) | b2;
        out.push_back(kAlphabet[(v >> 18) & 63]);
        out.push_back(kAlphabet[(v >> 12) & 63]);
        out.push_back(i + 1 < in.size() ? kAlphabet[(v >> 6) & 63] : '=');
        out.push_back(i + 2 < in.size() ? kAlphabet[v & 63] : '=');
    }
    return out;
}

static bool Base64Decode(const std::string& in, std::vector<BYTE>& out) {
    auto value = [](char c) -> int {
        if (c >= 'A' && c <= 'Z') return c - 'A';
        if (c >= 'a' && c <= 'z') return c - 'a' + 26;
        if (c >= '0' && c <= '9') return c - '0' + 52;
        if (c == '+') return 62;
        if (c == '/') return 63;
        return -1;
    };
    out.clear();
    out.reserve(in.size() / 4 * 3);
    uint32_t acc = 0;
    int bits = 0;
    for (const char c : in) {
        if (c == '=' || c == '\0') break;
        const int v = value(c);
        if (v < 0) return false;
        acc = (acc << 6) | (uint32_t)v;
        bits += 6;
        if (bits >= 8) {
            bits -= 8;
            out.push_back((BYTE)((acc >> bits) & 0xFF));
        }
    }
    return true;
}

// "e" + base64(DPAPI(text as UTF-16)). An empty result means "do not store this".
static std::wstring ProtectText(const std::wstring& plain) {
    const BYTE* p = (const BYTE*)plain.data();
    std::vector<BYTE> enc;
    if (!ProtectBytes(std::vector<BYTE>(p, p + plain.size() * sizeof(wchar_t)), enc)) {
        return std::wstring();
    }
    const std::string b64 = Base64Encode(enc);
    return L"e" + std::wstring(b64.begin(), b64.end());
}

// Values written before encryption existed (or by a debug build) are plain text: read them as they
// are, then re-encrypt them when the history is loaded. They are never written back as they came.
static bool UnprotectText(const std::wstring& stored, std::wstring& plain) {
    plain.clear();
    if (stored.empty()) return false;
    if (stored[0] != L'e') {
        plain = stored;
        return true;
    }
    std::vector<BYTE> enc, bytes;
    if (!Base64Decode(std::string(stored.begin() + 1, stored.end()), enc)) return false;
    if (!UnprotectBytes(enc, bytes)) return false;
    if (bytes.size() % sizeof(wchar_t) != 0) return false;
    plain.assign((const wchar_t*)bytes.data(), bytes.size() / sizeof(wchar_t));
    return true;
}

// Where the clipboard images live: Windhawk's own storage, in a folder for this account only, so one
// account's sweep can never touch another's files (the entries themselves are per user too).
// Resolved once, thread-safely: the thumbnail worker reaches this through IsClipImagePath, and a
// lazy static written from two threads is not safe when the folder cannot be created.
// The callback is a plain CALLBACK function: PINIT_ONCE_FN is __stdcall and a lambda is not, which
// only the 32-bit build notices.
static std::wstring g_clipImageDir;

static BOOL CALLBACK ClipImageDirInit(PINIT_ONCE, PVOID, PVOID*) {
    wchar_t buf[MAX_PATH] = L"";
    if (Wh_GetModStoragePath(buf, ARRAYSIZE(buf)) == 0 || !buf[0]) return TRUE;
    const std::wstring dir = std::wstring(buf) + L"\\clips\\" + CurrentUserSid();
    SHCreateDirectoryExW(nullptr, dir.c_str(), nullptr);  // the mod folder may not exist yet
    if (GetFileAttributesW(dir.c_str()) != INVALID_FILE_ATTRIBUTES) g_clipImageDir = dir;
    return TRUE;
}

static std::wstring ClipImageDir() {
    static INIT_ONCE once = INIT_ONCE_STATIC_INIT;
    InitOnceExecuteOnce(&once, ClipImageDirInit, nullptr, nullptr);
    return g_clipImageDir;
}

// "<hash>.dat|1920x1080" -> "<clips>\<sid>\<hash>.dat" (empty when the entry is not an image).
static std::wstring ClipImagePath(const std::wstring& stored) {
    const size_t bar = stored.find(L'|');
    if (bar == std::wstring::npos || bar == 0) return std::wstring();
    const std::wstring dir = ClipImageDir();
    return dir.empty() ? std::wstring() : dir + L"\\" + stored.substr(0, bar);
}

static bool IsClipImagePath(const std::wstring& path) {
    const std::wstring dir = ClipImageDir();
    return !dir.empty() && path.size() > dir.size() &&
           CompareStringOrdinal(path.c_str(), (int)dir.size(), dir.c_str(), (int)dir.size(), TRUE) ==
               CSTR_EQUAL;
}

// ===== Custom entries store (managed in-app, persisted by the mod) =====
// Entries the user creates in the launcher's own manager live here, kept separate from the Windhawk
// settings. They are serialized one entry per line, eight tab-separated fields (id, name, type,
// target, arguments, keywords, trigger, hotkey), with \, newlines and tabs escaped so a field can
// hold any text. Records written by older versions have six fields: the missing ones read empty.
// Per-user name, because Windhawk storage is machine-wide (see CurrentUserSid).
static std::wstring CustomStoreKey() { return L"custom.entries_" + CurrentUserSid(); }

// A short, unique-enough id for a new store entry (time + a counter, hex). Only needs to be stable
// and distinct among this user's entries, not cryptographic.
static std::wstring GenerateEntryId() {
    static unsigned counter = 0;
    wchar_t buf[32];
    swprintf_s(buf, L"e%llx%x", (unsigned long long)GetTickCount64(), counter++);
    return buf;
}

static std::wstring EscapeStoreField(const std::wstring& s) {
    std::wstring o;
    o.reserve(s.size());
    for (wchar_t c : s) {
        if (c == L'\\') o += L"\\\\";
        else if (c == L'\n') o += L"\\n";
        else if (c == L'\r') o += L"\\r";
        else if (c == L'\t') o += L"\\t";
        else o += c;
    }
    return o;
}

static std::wstring UnescapeStoreField(const std::wstring& s) {
    std::wstring o;
    o.reserve(s.size());
    for (size_t i = 0; i < s.size(); ++i) {
        if (s[i] == L'\\' && i + 1 < s.size()) {
            const wchar_t n = s[++i];
            o += (n == L'n') ? L'\n' : (n == L'r') ? L'\r' : (n == L't') ? L'\t' : n;
        } else {
            o += s[i];
        }
    }
    return o;
}

// ===== Per-query learning =====
// Picking an app for some words teaches the launcher to offer it first next time; capped at 200
// pairs, forgotten after 90 days without a pick (the score lives in RunSearch).
struct LearnEntry {
    std::wstring app;    // AppEntry::id
    std::wstring query;  // folded words as typed ("vs code")
    int count = 1;
    int stamp = 0;  // NowStamp() of the last pick
};
static std::vector<LearnEntry> g_learn;
static const size_t kMaxLearn = 200;
static const int kLearnDays = 90;

// The folded query words joined with single spaces: the key a learned pick is stored under.
static std::wstring LearnedQueryKey() {
    std::wstring key;
    for (const auto& t : g_tokens) {
        if (!key.empty()) key += L' ';
        key += t;
    }
    return key;
}

static std::wstring LearnKey() { return L"learn.v1_" + CurrentUserSid(); }

// Forgets pairs nobody picked for kLearnDays and keeps the most-used ones.
static void PruneLearn() {
    const int cutoff = NowStamp() - kLearnDays * 86400;
    for (size_t i = g_learn.size(); i-- > 0;) {
        if (g_learn[i].stamp < cutoff) g_learn.erase(g_learn.begin() + (std::ptrdiff_t)i);
    }
    if (g_learn.size() <= kMaxLearn) return;
    std::sort(g_learn.begin(), g_learn.end(), [](const LearnEntry& a, const LearnEntry& b) {
        return a.count != b.count ? a.count > b.count : a.stamp > b.stamp;
    });
    g_learn.resize(kMaxLearn);
}

static void SaveLearn() {
    std::wstring all;
    for (const LearnEntry& e : g_learn) {
        if (all.size() > 24000) break;  // one registry value, not a database
        all += std::to_wstring(e.count) + L'\t' + std::to_wstring(e.stamp) + L'\t' +
               EscapeStoreField(e.app) + L'\t' + EscapeStoreField(e.query) + L'\n';
    }
    Wh_SetStringValue(LearnKey().c_str(), all.c_str());
}

static void LoadLearn() {
    g_learn.clear();
    std::vector<wchar_t> buf(1 << 15, 0);
    const size_t n = Wh_GetStringValue(LearnKey().c_str(), buf.data(), buf.size());
    if (n == 0) return;
    const std::wstring all(buf.data(), n);
    size_t start = 0;
    while (start <= all.size()) {
        const size_t nl = all.find(L'\n', start);
        const std::wstring line =
            all.substr(start, nl == std::wstring::npos ? std::wstring::npos : nl - start);
        start = (nl == std::wstring::npos) ? all.size() + 1 : nl + 1;
        if (line.empty()) continue;
        std::wstring f[4];  // count, stamp, app, query
        size_t fi = 0, p = 0;
        for (;;) {
            const size_t t = line.find(L'\t', p);
            const std::wstring part =
                line.substr(p, t == std::wstring::npos ? std::wstring::npos : t - p);
            if (fi < 4) f[fi] = UnescapeStoreField(part);
            ++fi;
            if (t == std::wstring::npos) break;
            p = t + 1;
        }
        LearnEntry e;
        e.count = _wtoi(f[0].c_str());
        e.stamp = _wtoi(f[1].c_str());
        e.app = f[2];
        e.query = f[3];
        if (e.app.empty() || e.query.empty() || e.count <= 0) continue;
        g_learn.push_back(std::move(e));
    }
    PruneLearn();
}

// A pick only teaches something when it fixed the order: true when another app sat above it in the
// list the user was looking at.
static bool PickWasNotFirst(int sel) {
    for (int i = 0; i < sel && i < (int)g_results.size(); ++i) {
        if (g_results[(size_t)i].kind == ResultKind::App) return true;
    }
    return false;
}

// Remembers that the user picked `appId` for `query`.
static void RecordLearn(const std::wstring& appId, const std::wstring& query) {
    if (!g_cfg.learnFromChoices || appId.empty() || query.size() < 2 || query.size() > 60) return;
    for (LearnEntry& e : g_learn) {
        if (e.app == appId && e.query == query) {
            e.count = std::min(e.count + 1, 99);
            e.stamp = NowStamp();
            SaveLearn();
            return;
        }
    }
    LearnEntry e;
    e.app = appId;
    e.query = query;
    e.stamp = NowStamp();
    g_learn.push_back(std::move(e));
    PruneLearn();
    SaveLearn();
}

// Appends the store entries to `out` (after any settings entries). Missing/short records are
// tolerated; an entry needs at least a name and a target.
static void LoadCustomStore(std::vector<CustomEntry>& out) {
    std::vector<wchar_t> buf(1 << 16, 0);  // 64K chars: plenty for the manager's entries
    size_t n = Wh_GetStringValue(CustomStoreKey().c_str(), buf.data(), buf.size());
    if (n == 0) return;
    std::wstring all(buf.data(), n);
    size_t start = 0;
    while (start <= all.size()) {
        size_t nl = all.find(L'\n', start);
        std::wstring line =
            all.substr(start, nl == std::wstring::npos ? std::wstring::npos : nl - start);
        start = (nl == std::wstring::npos) ? all.size() + 1 : nl + 1;
        if (line.empty()) continue;
        std::wstring f[8];  // id, name, type, target, args, keywords, trigger, hotkey
        size_t fi = 0, p = 0;
        for (;;) {
            size_t t = line.find(L'\t', p);
            std::wstring part =
                line.substr(p, t == std::wstring::npos ? std::wstring::npos : t - p);
            if (fi < 8) f[fi] = UnescapeStoreField(part);
            ++fi;
            if (t == std::wstring::npos) break;
            p = t + 1;
        }
        if (f[1].empty() || f[3].empty()) continue;  // need a name and a target
        CustomEntry e = MakeCustomEntry(f[1], f[2], f[3], f[4], f[5], f[6], f[7]);
        e.fromStore = true;
        e.id = f[0].empty() ? GenerateEntryId() : f[0];
        out.push_back(std::move(e));
    }
}

// Writes the given store entries back to the mod's storage. (Used by the in-app manager; the
// settings entries are never written here.)
[[maybe_unused]] static void SaveCustomStore(const std::vector<CustomEntry>& storeEntries) {
    std::wstring all;
    for (const CustomEntry& e : storeEntries) {
        if (!all.empty()) all += L'\n';
        all += EscapeStoreField(e.id);
        all += L'\t';
        all += EscapeStoreField(e.name);
        all += L'\t';
        all += EscapeStoreField(e.type);
        all += L'\t';
        all += EscapeStoreField(e.target);
        all += L'\t';
        all += EscapeStoreField(e.args);
        all += L'\t';
        all += EscapeStoreField(e.keywords);
        all += L'\t';
        all += EscapeStoreField(e.trigger);
        all += L'\t';
        all += EscapeStoreField(e.hotkey);
    }
    Wh_SetStringValue(CustomStoreKey().c_str(), all.c_str());
}

// ===== Browser bookmarks (Chromium family and Firefox) =====
// Chrome, Edge, Brave, Vivaldi, Chromium and Opera keep their bookmarks in a plain JSON file
// called "Bookmarks", one per profile; Firefox and its forks keep compressed mozLz4 backups. Only
// those files and the profile display names are read: never history, cookies, passwords, favicons
// or the network. Every profile is searched; a bookmark that appears in two profiles is listed
// once.
//
// The load runs in a worker thread (a big file would freeze the bar) and the UI thread swaps the
// list in when it arrives. When the bar opens, the files' write times are checked so a bookmark
// added while the mod runs shows up without a restart.

namespace Json {

// A JSON value. Objects keep their members in file order (they are small and only looked up by
// name), arrays their items.
struct Value {
    enum class Type { Null, Bool, Num, Str, Arr, Obj } type = Type::Null;
    std::wstring str;
    double num = 0;
    std::vector<Value> items;                          // Arr
    std::vector<std::pair<std::wstring, Value>> members;  // Obj

    const Value* Get(const wchar_t* key) const {
        if (type != Type::Obj) return nullptr;
        for (const auto& m : members) {
            if (m.first == key) return &m.second;
        }
        return nullptr;
    }
    std::wstring Str(const wchar_t* key) const {
        const Value* v = Get(key);
        return (v && v->type == Type::Str) ? v->str : std::wstring();
    }
};

// Dependency-free reader for the subset of JSON these files use. Refuses absurd input: nesting
// past 64 levels, strings past 64K characters, malformed UTF-8 or escapes.
class Parser {
   public:
    explicit Parser(const std::string& text) : p_(text.c_str()), n_(text.size()) {}

    bool Parse(Value& out) {
        SkipWs();
        if (!Value_(out, 0)) return false;
        SkipWs();
        return i_ >= n_;
    }

   private:
    void SkipWs() {
        while (i_ < n_ && (p_[i_] == ' ' || p_[i_] == '\t' || p_[i_] == '\r' || p_[i_] == '\n')) {
            ++i_;
        }
    }

    bool Value_(Value& v, int depth) {
        if (depth > 64 || i_ >= n_) return false;
        switch (p_[i_]) {
            case '{': return Object_(v, depth);
            case '[': return Array_(v, depth);
            case '"':
                v.type = Value::Type::Str;
                return String_(v.str);
            case 't': return Literal_(v, "true", Value::Type::Bool);
            case 'f': return Literal_(v, "false", Value::Type::Bool);
            case 'n': return Literal_(v, "null", Value::Type::Null);
            default: return Number_(v);
        }
    }

    bool Literal_(Value& v, const char* lit, Value::Type t) {
        const size_t n = strlen(lit);
        if (i_ + n > n_ || memcmp(p_ + i_, lit, n) != 0) return false;
        i_ += n;
        v.type = t;
        return true;
    }

    bool Number_(Value& v) {
        char buf[64];
        size_t k = 0;
        while (i_ < n_ && k + 1 < sizeof(buf)) {
            const char c = p_[i_];
            if (!((c >= '0' && c <= '9') || c == '-' || c == '+' || c == '.' || c == 'e' ||
                  c == 'E')) {
                break;
            }
            buf[k++] = c;
            ++i_;
        }
        if (k == 0) return false;
        buf[k] = 0;
        v.type = Value::Type::Num;
        v.num = strtod(buf, nullptr);
        return true;
    }

    bool Hex4(unsigned& out) {
        if (i_ + 4 > n_) return false;
        out = 0;
        for (int k = 0; k < 4; ++k) {
            const char c = p_[i_++];
            out <<= 4;
            if (c >= '0' && c <= '9') out |= (unsigned)(c - '0');
            else if (c >= 'a' && c <= 'f') out |= (unsigned)(c - 'a' + 10);
            else if (c >= 'A' && c <= 'F') out |= (unsigned)(c - 'A' + 10);
            else return false;
        }
        return true;
    }

    static void AppendCp(std::wstring& out, unsigned cp) {
        if (cp <= 0xFFFF) {
            out += (wchar_t)cp;
        } else if (cp <= 0x10FFFF) {
            cp -= 0x10000;
            out += (wchar_t)(0xD800 + (cp >> 10));
            out += (wchar_t)(0xDC00 + (cp & 0x3FF));
        }
    }

    bool String_(std::wstring& out) {
        if (i_ >= n_ || p_[i_] != '"') return false;
        ++i_;
        while (i_ < n_) {
            const unsigned char c = (unsigned char)p_[i_++];
            if (c == '"') return true;
            if (c == '\\') {
                if (i_ >= n_) return false;
                const char e = p_[i_++];
                switch (e) {
                    case '"': out += L'"'; break;
                    case '\\': out += L'\\'; break;
                    case '/': out += L'/'; break;
                    case 'b': out += L'\b'; break;
                    case 'f': out += L'\f'; break;
                    case 'n': out += L'\n'; break;
                    case 'r': out += L'\r'; break;
                    case 't': out += L'\t'; break;
                    case 'u': {
                        unsigned cp = 0;
                        if (!Hex4(cp)) return false;
                        // A high surrogate followed by a low one is a single character.
                        if (cp >= 0xD800 && cp <= 0xDBFF && i_ + 1 < n_ && p_[i_] == '\\' &&
                            p_[i_ + 1] == 'u') {
                            const size_t save = i_;
                            i_ += 2;
                            unsigned lo = 0;
                            if (Hex4(lo) && lo >= 0xDC00 && lo <= 0xDFFF) {
                                cp = 0x10000 + ((cp - 0xD800) << 10) + (lo - 0xDC00);
                            } else {
                                i_ = save;
                            }
                        }
                        AppendCp(out, cp);
                        break;
                    }
                    default: return false;
                }
            } else if (c < 0x80) {
                out += (wchar_t)c;
            } else {
                // UTF-8 sequence: 2, 3 or 4 bytes.
                const int extra = (c >= 0xF0) ? 3 : (c >= 0xE0) ? 2 : (c >= 0xC0) ? 1 : -1;
                if (extra < 0 || i_ + (size_t)extra > n_) return false;
                unsigned cp = c & (0x3Fu >> extra);
                for (int k = 0; k < extra; ++k) {
                    const unsigned char cc = (unsigned char)p_[i_++];
                    if ((cc & 0xC0) != 0x80) return false;
                    cp = (cp << 6) | (cc & 0x3F);
                }
                AppendCp(out, cp);
            }
            if (out.size() > 65536) return false;
        }
        return false;
    }

    bool Array_(Value& v, int depth) {
        ++i_;  // '['
        v.type = Value::Type::Arr;
        SkipWs();
        if (i_ < n_ && p_[i_] == ']') {
            ++i_;
            return true;
        }
        for (;;) {
            SkipWs();
            v.items.emplace_back();
            if (!Value_(v.items.back(), depth + 1)) return false;
            SkipWs();
            if (i_ < n_ && p_[i_] == ',') {
                ++i_;
                continue;
            }
            if (i_ < n_ && p_[i_] == ']') {
                ++i_;
                return true;
            }
            return false;
        }
    }

    bool Object_(Value& v, int depth) {
        ++i_;  // '{'
        v.type = Value::Type::Obj;
        SkipWs();
        if (i_ < n_ && p_[i_] == '}') {
            ++i_;
            return true;
        }
        for (;;) {
            SkipWs();
            std::wstring key;
            if (!String_(key)) return false;
            SkipWs();
            if (i_ >= n_ || p_[i_] != ':') return false;
            ++i_;
            SkipWs();
            v.members.emplace_back(std::move(key), Value());
            if (!Value_(v.members.back().second, depth + 1)) return false;
            SkipWs();
            if (i_ < n_ && p_[i_] == ',') {
                ++i_;
                continue;
            }
            if (i_ < n_ && p_[i_] == '}') {
                ++i_;
                return true;
            }
            return false;
        }
    }

    const char* p_;
    size_t n_;
    size_t i_ = 0;
};

}  // namespace Json

static const size_t kMaxBookmarks = 20000;       // a bigger set is not searchable anyway
static const size_t kMaxBookmarkFile = 32 << 20;  // and neither is a file this size

// One file the browser keeps its bookmarks in, plus the labels for its rows.
struct BookmarkFile {
    std::wstring path;
    std::wstring browser;
    std::wstring profile;
    bool firefox = false;  // a mozLz4 bookmark backup instead of a Chromium "Bookmarks" file
};

struct BookmarkBrowser {
    const wchar_t* name;
    const wchar_t* rel;   // relative to the known folder
    bool roaming;         // %APPDATA% instead of %LOCALAPPDATA%
};

// Firefox and its forks keep one compressed backup per profile; there is no plain JSON file.
struct FirefoxBrowser {
    const wchar_t* name;
    const wchar_t* rel;  // relative to %APPDATA%
};

// The Chromium family stores "<...>\User Data\<profile>\Bookmarks"; Opera keeps the file in the
// profile folder itself. Missing folders are skipped, so only installed browsers cost anything.
static const BookmarkBrowser kBookmarkBrowsers[] = {
    {L"Chrome", L"Google\\Chrome\\User Data", false},
    {L"Chrome Beta", L"Google\\Chrome Beta\\User Data", false},
    {L"Chrome Dev", L"Google\\Chrome Dev\\User Data", false},
    {L"Chrome Canary", L"Google\\Chrome SxS\\User Data", false},
    {L"Edge", L"Microsoft\\Edge\\User Data", false},
    {L"Edge Beta", L"Microsoft\\Edge Beta\\User Data", false},
    {L"Edge Dev", L"Microsoft\\Edge Dev\\User Data", false},
    {L"Edge Canary", L"Microsoft\\Edge SxS\\User Data", false},
    {L"Brave", L"BraveSoftware\\Brave-Browser\\User Data", false},
    {L"Brave Beta", L"BraveSoftware\\Brave-Browser-Beta\\User Data", false},
    {L"Brave Nightly", L"BraveSoftware\\Brave-Browser-Nightly\\User Data", false},
    {L"Vivaldi", L"Vivaldi\\User Data", false},
    {L"Chromium", L"Chromium\\User Data", false},
    {L"Opera", L"Opera Software\\Opera Stable", true},
    {L"Opera GX", L"Opera Software\\Opera GX Stable", true},
};

// The Firefox family is separate: backups live under %APPDATA%\Mozilla\Firefox\Profiles\... and
// the forks use their own folder next to Mozilla.
static const FirefoxBrowser kFirefoxBrowsers[] = {
    {L"Firefox", L"Mozilla\\Firefox"},
    {L"Waterfox", L"Waterfox"},
    {L"LibreWolf", L"libreWolf"},
    {L"Zen", L"zen"},
    {L"Floorp", L"Floorp"},
};

// Reads a whole file, refusing anything over cap. Retries once: a browser may be rewriting it.
static bool ReadFileBytes(const std::wstring& path, std::string& out, size_t cap) {
    for (int attempt = 0; attempt < 2; ++attempt) {
        HANDLE h = CreateFileW(path.c_str(), GENERIC_READ,
                               FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr,
                               OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (h == INVALID_HANDLE_VALUE) {
            if (attempt == 0) Sleep(20);
            continue;
        }
        LARGE_INTEGER size{};
        bool ok = GetFileSizeEx(h, &size) && size.QuadPart > 0 &&
                  (unsigned long long)size.QuadPart <= cap;
        if (ok) {
            out.resize((size_t)size.QuadPart);
            DWORD got = 0;
            ok = ReadFile(h, out.data(), (DWORD)out.size(), &got, nullptr) != FALSE;
            out.resize(ok ? got : 0);
        }
        CloseHandle(h);
        return ok && !out.empty();
    }
    return false;
}

static void ListSubdirs(const std::wstring& dir, std::vector<std::wstring>& out) {
    WIN32_FIND_DATAW fd;
    HANDLE h = FindFirstFileW((dir + L"\\*").c_str(), &fd);
    if (h == INVALID_HANDLE_VALUE) return;
    do {
        if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
            wcscmp(fd.cFileName, L".") != 0 && wcscmp(fd.cFileName, L"..") != 0) {
            out.push_back(dir + L"\\" + fd.cFileName);
        }
    } while (FindNextFileW(h, &fd));
    FindClose(h);
}

// Profile folder -> display name, from the browser's Local State (what its own profile menu shows).
static void ReadProfileNames(const std::wstring& root,
                             std::unordered_map<std::wstring, std::wstring>& names) {
    std::string bytes;
    if (!ReadFileBytes(root + L"\\Local State", bytes, 4 << 20)) return;
    Json::Value doc;
    Json::Parser parser(bytes);
    if (!parser.Parse(doc)) return;
    const Json::Value* profile = doc.Get(L"profile");
    const Json::Value* cache = profile ? profile->Get(L"info_cache") : nullptr;
    if (!cache) return;
    for (const auto& m : cache->members) {
        const std::wstring name = m.second.Str(L"name");
        if (!name.empty()) names[m.first] = name;
    }
}

// One value from a profile's profiles.ini (Firefox keeps the display names there).
static std::wstring IniString(const std::wstring& ini, const wchar_t* section, const wchar_t* key) {
    wchar_t buf[512] = L"";
    GetPrivateProfileStringW(section, key, L"", buf, ARRAYSIZE(buf), ini.c_str());
    return buf;
}

// Newest bookmark backup in "<profile>\bookmarkbackups", or empty when the profile has none.
static std::wstring NewestFirefoxBackup(const std::wstring& profileDir) {
    WIN32_FIND_DATAW fd;
    HANDLE h = FindFirstFileW(
        (profileDir + L"\\bookmarkbackups\\bookmarks-*.jsonlz4").c_str(), &fd);
    if (h == INVALID_HANDLE_VALUE) return L"";
    std::wstring best;
    FILETIME bestTime = {};
    do {
        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) continue;
        if (best.empty() || CompareFileTime(&fd.ftLastWriteTime, &bestTime) > 0) {
            best = profileDir + L"\\bookmarkbackups\\" + fd.cFileName;
            bestTime = fd.ftLastWriteTime;
        }
    } while (FindNextFileW(h, &fd));
    FindClose(h);
    return best;
}

static void AddFirefoxProfile(const std::wstring& dir, const std::wstring& browser,
                              const std::wstring& profile, std::vector<BookmarkFile>& files) {
    const std::wstring backup = NewestFirefoxBackup(dir);
    if (backup.empty()) return;
    BookmarkFile bf;
    bf.path = backup;
    bf.browser = browser;
    bf.profile = profile;
    bf.firefox = true;
    files.push_back(std::move(bf));
}

// Reads the profile list of one Firefox-family browser. profiles.ini is the normal case; without
// it, every folder under "Profiles" is tried and its folder name is used as the profile label.
static void AddFirefoxBrowser(const std::wstring& base, const FirefoxBrowser& b,
                              std::vector<BookmarkFile>& files) {
    const std::wstring root = base + L"\\" + b.rel;
    if (GetFileAttributesW(root.c_str()) == INVALID_FILE_ATTRIBUTES) return;
    const std::wstring ini = root + L"\\profiles.ini";
    wchar_t sections[8192] = L"";
    const DWORD n = GetPrivateProfileSectionNamesW(sections, ARRAYSIZE(sections), ini.c_str());
    bool any = false;
    for (const wchar_t* s = sections; n > 0 && *s; s += wcslen(s) + 1) {
        if (wcsncmp(s, L"Profile", 7) != 0) continue;  // Profile0, Profile1, ...
        std::wstring path = IniString(ini, s, L"Path");
        if (path.empty()) continue;
        for (auto& c : path) {
            if (c == L'/') c = L'\\';
        }
        const std::wstring dir =
            (IniString(ini, s, L"IsRelative") == L"0") ? path : root + L"\\" + path;
        std::wstring name = IniString(ini, s, L"Name");
        if (name.empty()) name = dir.substr(dir.find_last_of(L'\\') + 1);
        AddFirefoxProfile(dir, b.name, name, files);
        any = true;
    }
    if (any) return;
    std::vector<std::wstring> dirs;
    ListSubdirs(root + L"\\Profiles", dirs);
    for (const std::wstring& dir : dirs) {
        AddFirefoxProfile(dir, b.name, dir.substr(dir.find_last_of(L'\\') + 1), files);
    }
}

static std::vector<BookmarkFile> FindBookmarkFiles() {
    std::vector<BookmarkFile> files;
    for (const BookmarkBrowser& b : kBookmarkBrowsers) {
        const std::wstring base =
            KnownFolderPath(b.roaming ? FOLDERID_RoamingAppData : FOLDERID_LocalAppData);
        if (base.empty()) continue;
        const std::wstring root = base + L"\\" + b.rel;
        if (GetFileAttributesW(root.c_str()) == INVALID_FILE_ATTRIBUTES) continue;
        std::unordered_map<std::wstring, std::wstring> names;
        ReadProfileNames(root, names);
        std::vector<std::wstring> dirs;
        dirs.push_back(root);  // Opera keeps Bookmarks in the root itself
        ListSubdirs(root, dirs);
        for (const std::wstring& dir : dirs) {
            const std::wstring file = dir + L"\\Bookmarks";
            if (GetFileAttributesW(file.c_str()) == INVALID_FILE_ATTRIBUTES) continue;
            BookmarkFile bf;
            bf.path = file;
            bf.browser = b.name;
            if (dir != root) {
                const std::wstring dirName = dir.substr(root.size() + 1);
                const auto it = names.find(dirName);
                bf.profile = (it != names.end()) ? it->second : dirName;
            }
            files.push_back(std::move(bf));
        }
    }
    const std::wstring roaming = KnownFolderPath(FOLDERID_RoamingAppData);
    if (!roaming.empty()) {
        for (const FirefoxBrowser& b : kFirefoxBrowsers) AddFirefoxBrowser(roaming, b, files);
    }
    return files;
}

static bool BookmarkUrlAllowed(const std::wstring& url) {
    static const wchar_t* const kSchemes[] = {L"http://", L"https://", L"file://", L"mailto:"};
    for (const wchar_t* s : kSchemes) {
        const size_t n = wcslen(s);
        if (url.size() >= n && _wcsnicmp(url.c_str(), s, n) == 0) return true;
    }
    return false;  // javascript:, chrome://, data:, ... are not useful as launcher results
}

static std::wstring BookmarkHost(const std::wstring& url) {
    const size_t p = url.find(L"://");
    std::wstring host = (p == std::wstring::npos) ? std::wstring() : url.substr(p + 3);
    host = host.substr(0, host.find_first_of(L"/?#"));
    const size_t at = host.rfind(L'@');
    if (at != std::wstring::npos) host = host.substr(at + 1);
    const size_t colon = host.find(L':');
    if (colon != std::wstring::npos) host = host.substr(0, colon);
    if (host.rfind(L"www.", 0) == 0) host = host.substr(4);
    return host;
}

static void CollectBookmarkNode(const Json::Value& node, const std::wstring& folder,
                                const std::wstring& browser, const std::wstring& profile,
                                std::vector<BookmarkEntry>& out,
                                std::unordered_set<std::wstring>& seen, int depth) {
    if (depth > 32 || out.size() >= kMaxBookmarks) return;
    const std::wstring name = node.Str(L"name");
    if (node.Str(L"type") == L"url") {
        const std::wstring url = node.Str(L"url");
        if (url.empty() || !BookmarkUrlAllowed(url)) return;
        if (!seen.insert(url).second) return;  // same URL in two profiles/folders: one row
        BookmarkEntry e;
        e.name = name.empty() ? url : name;
        e.folded = FoldString(e.name);
        e.url = url;
        e.folder = folder;
        e.browser = browser;
        e.profile = profile;
        e.tail = FoldString(folder + L" " + BookmarkHost(url));
        out.push_back(std::move(e));
        return;
    }
    const Json::Value* children = node.Get(L"children");
    if (!children || children->type != Json::Value::Type::Arr) return;
    const std::wstring sub =
        name.empty() ? folder : (folder.empty() ? name : folder + L" / " + name);
    for (const Json::Value& child : children->items) {
        CollectBookmarkNode(child, sub, browser, profile, out, seen, depth + 1);
    }
}

// Firefox bookmark backups are "mozLz4": the 8-byte magic "mozLz40\0", a 4-byte little-endian
// uncompressed size, then an LZ4 block. This reads that block with no library and no writes.
static bool MozLz4Decompress(const std::string& in, std::string& out) {
    static const char kMagic[8] = {'m', 'o', 'z', 'L', 'z', '4', '0', '\0'};
    if (in.size() < 12 || memcmp(in.data(), kMagic, 8) != 0) return false;
    const size_t size = (size_t)(unsigned char)in[8] | ((size_t)(unsigned char)in[9] << 8) |
                        ((size_t)(unsigned char)in[10] << 16) |
                        ((size_t)(unsigned char)in[11] << 24);
    if (size == 0 || size > kMaxBookmarkFile) return false;
    out.clear();
    out.reserve(size);
    const unsigned char* src = (const unsigned char*)in.data() + 12;
    const unsigned char* const end = (const unsigned char*)in.data() + in.size();
    while (src < end) {
        const unsigned char token = *src++;
        size_t lit = token >> 4;
        if (lit == 15) {
            unsigned char b = 0;
            do {
                if (src >= end) return false;
                b = *src++;
                lit += b;
            } while (b == 255);
        }
        if ((size_t)(end - src) < lit || out.size() + lit > size) return false;
        out.append((const char*)src, lit);
        src += lit;
        if (src >= end) break;
        if ((size_t)(end - src) < 2) return false;
        const size_t offset = (size_t)src[0] | ((size_t)src[1] << 8);
        src += 2;
        if (offset == 0 || offset > out.size()) return false;
        size_t len = (size_t)(token & 0x0F) + 4;
        if ((token & 0x0F) == 15) {
            unsigned char b = 0;
            do {
                if (src >= end) return false;
                b = *src++;
                len += b;
            } while (b == 255);
        }
        if (out.size() + len > size) return false;
        const size_t dst = out.size();
        out.resize(dst + len);
        for (size_t k = 0; k < len; ++k) out[dst + k] = out[dst - offset + k];
    }
    return out.size() == size;
}

// Label of a Firefox root folder (the backup stores internal names such as "menu").
static std::wstring FirefoxRootLabel(const Json::Value& node) {
    const std::wstring root = node.Str(L"root");
    if (root == L"bookmarksMenuFolder") return L"Bookmarks Menu";
    if (root == L"toolbarFolder") return L"Bookmarks Toolbar";
    if (root == L"unfiledBookmarksFolder") return L"Other Bookmarks";
    if (root == L"mobileFolder") return L"Mobile Bookmarks";
    return node.Str(L"title");
}

// Walks one Firefox backup node: "uri" marks a bookmark, "children" a folder. At the top level,
// the four root folders get friendly names; below it, each folder's own title is used.
static void CollectFirefoxNode(const Json::Value& node, const std::wstring& folder,
                               const std::wstring& browser, const std::wstring& profile,
                               std::vector<BookmarkEntry>& out,
                               std::unordered_set<std::wstring>& seen, int depth,
                               bool rootLevel) {
    if (depth > 32 || out.size() >= kMaxBookmarks) return;
    const std::wstring url = node.Str(L"uri");
    if (!url.empty()) {
        if (!BookmarkUrlAllowed(url) || !seen.insert(url).second) return;
        const std::wstring title = node.Str(L"title");
        BookmarkEntry e;
        e.name = title.empty() ? url : title;
        e.folded = FoldString(e.name);
        e.url = url;
        e.folder = folder;
        e.browser = browser;
        e.profile = profile;
        e.tail = FoldString(folder + L" " + BookmarkHost(url));
        out.push_back(std::move(e));
        return;
    }
    const Json::Value* children = node.Get(L"children");
    if (!children || children->type != Json::Value::Type::Arr) return;
    std::wstring sub = folder;
    if (rootLevel) {
        sub = FirefoxRootLabel(node);
    } else {
        const std::wstring title = node.Str(L"title");
        if (!title.empty()) sub = folder.empty() ? title : folder + L" / " + title;
    }
    for (const Json::Value& child : children->items) {
        CollectFirefoxNode(child, sub, browser, profile, out, seen, depth + 1, false);
    }
}

static std::vector<BookmarkEntry> LoadBrowserBookmarks() {
    std::vector<BookmarkEntry> all;
    std::unordered_set<std::wstring> seen;
    for (const BookmarkFile& file : FindBookmarkFiles()) {
        std::string bytes;
        if (!ReadFileBytes(file.path, bytes, kMaxBookmarkFile)) continue;
        std::string json;
        if (file.firefox) {
            if (!MozLz4Decompress(bytes, json)) {
                Wh_Log(L"bookmarks: cannot decompress %s", file.path.c_str());
                continue;
            }
        } else {
            json = std::move(bytes);
        }
        Json::Value doc;
        Json::Parser parser(json);
        if (!parser.Parse(doc)) {
            Wh_Log(L"bookmarks: cannot parse %s", file.path.c_str());
            continue;
        }
        if (file.firefox) {
            const Json::Value* children = doc.Get(L"children");
            if (!children || children->type != Json::Value::Type::Arr) continue;
            for (const Json::Value& child : children->items) {
                CollectFirefoxNode(child, L"", file.browser, file.profile, all, seen, 0, true);
            }
            continue;
        }
        const Json::Value* roots = doc.Get(L"roots");
        if (!roots) continue;
        // The root nodes ("bookmark_bar", "other", "synced") are containers, not folders: their
        // children at the top level carry no folder name.
        for (const auto& root : roots->members) {
            const Json::Value* children = root.second.Get(L"children");
            if (!children || children->type != Json::Value::Type::Arr) continue;
            for (const Json::Value& child : children->items) {
                CollectBookmarkNode(child, L"", file.browser, file.profile, all, seen, 0);
            }
        }
    }
    return all;
}

static ULONGLONG BookmarkFilesStamp(const std::vector<BookmarkFile>& files) {
    ULONGLONG best = 0;
    for (const BookmarkFile& f : files) {
        WIN32_FILE_ATTRIBUTE_DATA d{};
        if (!GetFileAttributesExW(f.path.c_str(), GetFileExInfoStandard, &d)) continue;
        const ULONGLONG t =
            ((ULONGLONG)d.ftLastWriteTime.dwHighDateTime << 32) | d.ftLastWriteTime.dwLowDateTime;
        if (t > best) best = t;
    }
    return best;
}

struct BookmarkResult {
    UINT gen;
    std::vector<BookmarkEntry> entries;
};

static DWORD WINAPI BookmarkThreadProc(LPVOID param) {
    const UINT gen = (UINT)(UINT_PTR)param;
    auto* res = new BookmarkResult{gen, LoadBrowserBookmarks()};
    if (!g_hwnd || !PostMessageW(g_hwnd, WM_APP_BOOKMARKS, 0, (LPARAM)res)) delete res;
    return 0;
}

static void StartBookmarkLoad() {
    if (!g_cfg.bookmarks || !g_hwnd) return;  // no file is read while the feature is off
    const UINT gen = ++g_bookmarkGen;
    HANDLE th = CreateThread(nullptr, 0, BookmarkThreadProc, (LPVOID)(UINT_PTR)gen, 0, nullptr);
    if (th) CloseHandle(th);
}

// Called when the bar opens and when the setting is turned on: reloads only if a file changed.
static void RefreshBookmarksIfStale() {
    if (!g_cfg.bookmarks || !g_hwnd) return;
    const double now = NowSec();
    if (now - g_bookmarkCheckedAt < 2.0) return;
    g_bookmarkCheckedAt = now;
    const ULONGLONG stamp = BookmarkFilesStamp(FindBookmarkFiles());
    if (stamp == g_bookmarkStamp) return;
    g_bookmarkStamp = stamp;
    StartBookmarkLoad();
}

// Score of a bookmark for the current query; 0 when any token is missing. The name scores like an
// app name, the folder and host are a weaker fallback so a name match always wins.
static int ScoreBookmark(const BookmarkEntry& b) {
    if (g_tokens.empty()) return 1;
    int total = 0;
    for (const auto& t : g_tokens) {
        int s = ScoreToken(b.name, b.folded, t, nullptr);
        if (s <= 0) {
            if (b.tail.find(t) == std::wstring::npos) return 0;
            s = 180;
        }
        total += s;
    }
    return total / (int)g_tokens.size();
}

// The right-hand line of a bookmark row: folder, browser and profile, as far as they are known.
static std::wstring BookmarkMeta(const BookmarkEntry& b) {
    std::wstring meta = b.browser;
    if (!b.profile.empty()) meta += L" (" + b.profile + L")";
    return b.folder.empty() ? meta : b.folder + L" · " + meta;
}

static SearchResult MakeBookmarkResult(const BookmarkEntry& b, int score) {
    SearchResult r;
    r.kind = ResultKind::Bookmark;
    r.score = score;
    r.title = b.name;
    r.subtitle = BookmarkMeta(b);
    r.payload = b.url;
    return r;
}

// One-line preview of a clipboard entry; the full text stays in the result payload.
static std::wstring ClipPreview(const std::wstring& text) {
    std::wstring s;
    s.reserve(std::min<size_t>(text.size(), 140));
    for (const wchar_t c : text) {
        if (s.size() >= 140) {
            s += L"…";
            break;
        }
        s += (c == L'\r' || c == L'\n' || c == L'\t') ? L' ' : c;
    }
    return s;
}

static bool ClipMatches(const ClipEntry& c) {
    if (g_tokens.empty()) return true;
    // An image is searched by what it is and by its size ("image", "1920x1080").
    const std::wstring folded =
        c.image ? (L"image png " + std::to_wstring(c.w) + L"x" + std::to_wstring(c.h))
                : FoldString(c.text);
    for (const auto& t : g_tokens) {
        if (folded.find(t) == std::wstring::npos) return false;
    }
    return true;
}

static SearchResult MakeClipResult(const ClipEntry& c) {
    SearchResult r;
    r.kind = ResultKind::Clip;
    r.image = c.image;
    r.title = c.image ? (L"Image " + std::to_wstring(c.w) + L"×" + std::to_wstring(c.h))
                      : ClipPreview(c.text);
    r.subtitle = c.pinned ? std::wstring(L"Clipboard (pinned)") : std::wstring(kTxtKindClip);
    r.payload = c.text;
    r.pinned = c.pinned;
    return r;
}

static bool RecentMatches(const RecentEntry& r) {
    if (g_tokens.empty()) return true;
    for (const auto& t : g_tokens) {
        if (r.folded.find(t) == std::wstring::npos) return false;
    }
    return true;
}

static SearchResult MakeRecentResult(const RecentEntry& r) {
    SearchResult out;
    out.kind = ResultKind::Recent;
    out.title = r.name;
    out.subtitle = r.dir;
    out.payload = r.path;
    out.isDir = r.isDir;
    out.ext = r.ext;
    return out;
}

static std::wstring GlobalQueryKey() {
    std::wstring k;
    for (const auto& t : g_tokens) {
        k += t;
        k += L'\n';
    }
    // The filters are part of the query: typing one must start a new search.
    for (const auto& e : g_fileFilter.exts) {
        k += e;
        k += L',';
    }
    if (g_fileFilter.hasSize) {
        k += std::to_wstring(g_fileFilter.minSize);
        k += L'-';
        k += std::to_wstring(g_fileFilter.maxSize);
    }
    if (g_fileFilter.hasAfter) k += std::to_wstring(g_fileFilter.afterTime);
    if (g_fileFilter.hasBefore) k += std::to_wstring(g_fileFilter.beforeTime);
    return k;
}

static SearchResult MakeGlobalResult(const FileEntry& e) {
    SearchResult r;
    r.kind = ResultKind::GlobalFile;
    r.title = e.name;
    // "1.2 MB · 18/10/2026 16:02 · C:\Users\…": the metadata comes first so it survives when the
    // folder is too long and the row clips it.
    r.subtitle = FileMetaText(e) + L" · " + e.dir;
    r.payload = e.path;
    r.isDir = e.isDir;
    r.ext = e.ext;
    return r;
}

static void EnsureWindowList();  // defined with the window helpers, below

static bool WindowMatches(const WindowEntry& w) {
    if (g_tokens.empty()) return true;
    for (const auto& t : g_tokens) {
        if (w.folded.find(t) == std::wstring::npos) return false;
    }
    return true;
}

static SearchResult MakeWindowResult(const WindowEntry& w) {
    SearchResult r;
    r.kind = ResultKind::Window;
    r.title = w.title;
    r.subtitle = w.process;
    r.hwnd = w.hwnd;
    r.iconApp = w.appIcon;
    r.payload = w.path;  // the row draws the icon from the process itself when no app matches
    r.onTop = (GetWindowLongPtrW(w.hwnd, GWL_EXSTYLE) & WS_EX_TOPMOST) != 0;
    return r;
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

// ---- Web search -------------------------------------------------------------
static const wchar_t* WebEngineName(WebEngine e) {
    switch (e) {
        case WebEngine::Bing: return L"Bing";
        case WebEngine::DuckDuckGo: return L"DuckDuckGo";
        case WebEngine::YouTube: return L"YouTube";
        case WebEngine::Wikipedia: return L"Wikipedia";
        default: return L"Google";
    }
}

// Brand colour of the engine, used for the web result's icon badge.
static D2D1_COLOR_F WebEngineColor(WebEngine e) {
    switch (e) {
        case WebEngine::Bing: return D2D1::ColorF(0.00f, 0.51f, 0.45f);        // #008373
        case WebEngine::DuckDuckGo: return D2D1::ColorF(0.87f, 0.35f, 0.20f);  // #DE5833
        case WebEngine::YouTube: return D2D1::ColorF(0.90f, 0.13f, 0.09f);     // #E62117
        case WebEngine::Wikipedia: return D2D1::ColorF(0.33f, 0.33f, 0.33f);   // #545454
        default: return D2D1::ColorF(0.26f, 0.52f, 0.96f);                     // #4285F4 (Google)
    }
}

// Percent-encodes the query for a URL (UTF-8, unreserved characters kept as-is).
static std::wstring UrlEncode(const std::wstring& s) {
    if (s.empty()) return L"";
    int len = WideCharToMultiByte(CP_UTF8, 0, s.c_str(), (int)s.size(), nullptr, 0, nullptr, nullptr);
    if (len <= 0) return L"";
    std::string utf8((size_t)len, '\0');
    WideCharToMultiByte(CP_UTF8, 0, s.c_str(), (int)s.size(), &utf8[0], len, nullptr, nullptr);
    static const wchar_t* hex = L"0123456789ABCDEF";
    std::wstring out;
    out.reserve((size_t)len * 3);
    for (unsigned char c : utf8) {
        if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') ||
            c == '-' || c == '_' || c == '.' || c == '~') {
            out.push_back((wchar_t)c);
        } else {
            out.push_back(L'%');
            out.push_back(hex[c >> 4]);
            out.push_back(hex[c & 0x0F]);
        }
    }
    return out;
}

static std::wstring WebSearchUrl(WebEngine e, const std::wstring& query) {
    const std::wstring q = UrlEncode(query);
    switch (e) {
        case WebEngine::Bing: return L"https://www.bing.com/search?q=" + q;
        case WebEngine::DuckDuckGo: return L"https://duckduckgo.com/?q=" + q;
        case WebEngine::YouTube: return L"https://www.youtube.com/results?search_query=" + q;
        case WebEngine::Wikipedia: return L"https://en.wikipedia.org/w/index.php?search=" + q;
        default: return L"https://www.google.com/search?q=" + q;
    }
}

static bool IsIPv4(const std::wstring& s) {
    int parts = 0, val = 0, digits = 0;
    for (size_t i = 0; i <= s.size(); ++i) {
        if (i < s.size() && s[i] >= L'0' && s[i] <= L'9') {
            val = val * 10 + (s[i] - L'0');
            if (++digits > 3 || val > 255) return false;
        } else if (i < s.size() && s[i] == L'.') {
            if (digits == 0) return false;
            ++parts;
            val = 0;
            digits = 0;
        } else if (i == s.size()) {
            return parts == 3 && digits > 0;
        } else {
            return false;
        }
    }
    return false;
}

static bool IsCommonFileExtension(const std::wstring& ext) {
    static const wchar_t* kExts[] = {
        L"txt",  L"log",  L"ini",  L"cfg",  L"conf", L"pdf",  L"png",  L"jpg",  L"jpeg", L"gif",
        L"bmp",  L"svg",  L"ico",  L"mp3",  L"wav",  L"flac", L"mp4",  L"avi",  L"mkv",  L"mov",
        L"exe",  L"dll",  L"msi",  L"bat",  L"cmd",  L"ps1",  L"reg",  L"sys",  L"tmp",  L"bak",
        L"zip",  L"rar",  L"7z",   L"tar",  L"gz",   L"iso",  L"json", L"xml",  L"yaml", L"yml",
        L"csv",  L"tsv",  L"md",   L"doc",  L"docx", L"xls",  L"xlsx", L"ppt",  L"pptx", L"py",
        L"js",   L"ts",   L"c",    L"h",    L"cs",   L"cpp",  L"hpp",  L"java", L"rs",   L"go",
        L"rb",   L"php",  L"sh",   L"lnk",  L"url",  L"dat",  L"bin",  L"obj",  L"lib",  L"pdb",
    };
    for (const wchar_t* e : kExts) {
        if (ext == e) {
            return true;
        }
    }
    return false;
}

// Whether the text looks like something to open rather than to search for: a URL with a scheme,
// a "www." host, localhost, an IPv4 address, or a bare domain with a plausible TLD.
static bool LooksLikeUrl(const std::wstring& s) {
    if (s.empty() || s.find(L' ') != std::wstring::npos ||
        s.find(L'\t') != std::wstring::npos) {
        return false;
    }
    const std::wstring low = FoldString(s);
    const wchar_t* schemes[] = {L"http://", L"https://", L"ftp://", L"file://", L"mailto:"};
    for (const wchar_t* sc : schemes) {
        if (low.rfind(sc, 0) == 0) {
            return true;
        }
    }
    if (low.rfind(L"www.", 0) == 0) {
        return true;
    }
    // Keep only the host: drop an optional path, query, fragment and port.
    std::wstring host = low.substr(0, low.find_first_of(L"/?#"));
    const size_t colon = host.find(L':');
    if (colon != std::wstring::npos) {
        host = host.substr(0, colon);
    }
    if (host == L"localhost" || IsIPv4(host)) {
        return true;
    }
    // Bare domain: at least one dot, a letters-only TLD of 2+, and a valid host before it.
    const size_t dot = host.rfind(L'.');
    if (dot == std::wstring::npos || dot == 0 || dot + 2 >= host.size()) {
        return false;
    }
    for (size_t i = 0; i < host.size(); ++i) {
        const wchar_t c = host[i];
        if (!((c >= L'a' && c <= L'z') || (c >= L'0' && c <= L'9') || c == L'-' || c == L'.')) {
            return false;
        }
    }
    for (size_t i = dot + 1; i < host.size(); ++i) {
        if (host[i] < L'a' || host[i] > L'z') {
            return false;
        }
    }
    return !IsCommonFileExtension(host.substr(dot + 1));
}

// Adds a scheme when the user typed a bare host, so the browser opens it as a site.
static std::wstring NormalizeUrl(const std::wstring& s) {
    const std::wstring low = FoldString(s);
    const wchar_t* schemes[] = {L"http://", L"https://", L"ftp://", L"file://", L"mailto:"};
    for (const wchar_t* sc : schemes) {
        if (low.rfind(sc, 0) == 0) {
            return s;
        }
    }
    std::wstring host = low.substr(0, low.find_first_of(L"/?#"));
    const size_t colon = host.find(L':');
    if (colon != std::wstring::npos) {
        host = host.substr(0, colon);
    }
    if (host == L"localhost" || IsIPv4(host)) {
        return L"http://" + s;
    }
    return L"https://" + s;
}

// Right-hand label of a custom entry, from its type.
static const wchar_t* CustomTypeLabel(const std::wstring& type) {
    if (type == L"path") return L"File or folder";
    if (type == L"run") return L"Command";
    return L"Website";
}

static void RunSearch() {
    ReleaseResultLayouts();
    g_results.clear();

    const std::wstring query = ActivationQuery(g_text);
    g_tokens = Tokenize(query);

    // "find:" understands the same filters as Files. Its Everything query drops them (see
    // StripFileFilters) and they are applied to the results instead.
    if (g_activator >= 0 && kActivators[g_activator].mode == LauncherMode::GlobalFiles) {
        g_tokens = ParseFileQuery(query, g_fileFilter);
    }

    // File mode (toggled by the folder button) lists the selected location and filters it
    // locally; a typed activator still takes precedence (it is handled just below).
    if (g_activator < 0 && g_fileMode) {
        g_tokens = ParseFileQuery(query, g_fileFilter);
        g_calc = Calc::Result();
        g_calcDisplay.clear();
        g_calcActive = false;
        if (g_fileDir.empty() || !SamePath(g_fileDir, g_fileCur)) RefreshFileListing();
        BuildFileResults();
        BuildRowOffsets();
        return;
    }

    // An activator switches into a mode.
    if (g_activator >= 0) {
        g_calc = Calc::Result();
        g_calcDisplay.clear();
        g_calcActive = false;
        if (kActivators[g_activator].mode == LauncherMode::Commands) {
            for (int i = 0; i < kCommandCount; ++i) {
                const CommandDef& c = kCommands[i];
                // A mode opener only exists while its feature is on, like its prefix.
                if (c.activator >= 0 && !ActivatorEnabled(kActivators[c.activator])) continue;
                std::wstring hay = FoldString(c.title);
                hay += L' ';
                hay += FoldString(c.keywords);
                bool ok = true;
                for (const auto& t : g_tokens) {
                    if (hay.find(t) == std::wstring::npos) {
                        ok = false;
                        break;
                    }
                }
                if (!ok) continue;
                SearchResult r;
                r.kind = ResultKind::Command;
                r.title = c.title;
                r.subtitle = c.category;
                r.payload = c.id;
                r.icon = i;  // index into kCommands, for the badge glyph/colour
                g_results.push_back(std::move(r));
            }
        } else if (kActivators[g_activator].mode == LauncherMode::Bookmarks && g_cfg.bookmarks) {
            // Bookmark-only search: all matches, best first. An empty query lists them from the
            // start (the list still has to be scrolled).
            std::vector<SearchResult> found;
            const int kMaxBmRows = 200;
            for (const BookmarkEntry& b : g_bookmarks) {
                const int s = ScoreBookmark(b);
                if (s <= 0) continue;
                found.push_back(MakeBookmarkResult(b, s));
                // shortcut: only the first 4000 matches are ranked (typing narrows them anyway)
                if ((int)found.size() >= 4000) break;
            }
            std::sort(found.begin(), found.end(), [](const SearchResult& a, const SearchResult& b) {
                if (a.score != b.score) return a.score > b.score;
                return FoldString(a.title) < FoldString(b.title);
            });
            if ((int)found.size() > kMaxBmRows) found.resize(kMaxBmRows);
            for (auto& r : found) g_results.push_back(std::move(r));
        } else if (kActivators[g_activator].mode == LauncherMode::Clipboard) {
            // Clipboard-only search: pinned entries first, newest first inside each group.
            if (g_cfg.clipboardHistory) {
                for (int pass = 0; pass < 2; ++pass) {
                    for (const ClipEntry& c : g_clips) {
                        if (c.pinned != (pass == 0)) continue;
                        if (ClipMatches(c)) g_results.push_back(MakeClipResult(c));
                    }
                }
            }
        } else if (kActivators[g_activator].mode == LauncherMode::Recent) {
            // Windows' Recent list, newest first.
            if (g_cfg.recentFiles) {
                for (const RecentEntry& r : g_recent) {
                    if (RecentMatches(r)) g_results.push_back(MakeRecentResult(r));
                }
            }
        } else if (kActivators[g_activator].mode == LauncherMode::GlobalFiles) {
            // Global search: show the last completed/partial snapshot only when it matches this
            // exact query; the worker thread posts updates through WM_APP_GLOBAL.
            if (!query.empty() && g_globalKey == GlobalQueryKey()) {
                for (const FileEntry& e : g_globalFiles) {
                    g_results.push_back(MakeGlobalResult(e));
                }
            }
        } else if (kActivators[g_activator].mode == LauncherMode::WindowList) {
            // Open windows, topmost first; filtered by title and process name.
            EnsureWindowList();
            for (const WindowEntry& w : g_windows) {
                if (WindowMatches(w)) g_results.push_back(MakeWindowResult(w));
            }
        } else if (!query.empty()) {
            // Web mode: a single result, always a search (never the direct-open behaviour),
            // optionally pinned to one engine.
            const int ei = kActivators[g_activator].engine;
            const WebEngine eng = ei >= 0 ? (WebEngine)ei : g_cfg.webEngine;
            SearchResult w;
            w.kind = ResultKind::Web;
            w.title = L"Search the web for “" + query + L"”";
            w.subtitle = WebEngineName(eng);
            w.icon = (int)eng;
            w.payload = WebSearchUrl(eng, query);
            g_results.push_back(std::move(w));
        }
        BuildRowOffsets();
        return;
    }

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

    // The words as typed, folded: the key the learned picks are stored under.
    const std::wstring queryKey = LearnedQueryKey();
    // Learned picks for exactly these words (usually none or one), collected once instead of
    // scanning the whole learned list for every app. The value is how many times it was picked.
    std::vector<std::pair<std::wstring, int>> learned;
    if (g_cfg.learnFromChoices && !g_learn.empty()) {
        for (const LearnEntry& e : g_learn) {
            if (e.query == queryKey) learned.emplace_back(e.app, e.count);
        }
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
        // Usage and pins only break ties inside the same match tier: together they stay below the
        // 100-point gap between a name, a prefix, a word start and a substring.
        int extra = FrecencyScore(a) + (a.pinned ? 30 : 0);
        total += std::min(extra, 99);
        // An explicit pick for these words wins outright (2000 clears any match plus its extras), so
        // it also fixes a weak match; the count only orders learned apps among themselves.
        for (const auto& l : learned) {
            if (l.first == a.id) {
                total += 2000 + std::min(400, 20 * l.second);
                break;
            }
        }
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

    // Custom entries match like apps (name + keywords) and sit below them. Leftover words go to
    // {query} ("YouTube cats" searches "cats"); a trigger ("yt cats") runs it at once, on top.
    if (!g_cfg.commands.empty()) {
        // The query as typed, split the same way as g_tokens (which holds the folded words);
        // the original spelling is what the entry receives.
        std::vector<std::wstring> words;
        {
            std::wstring cur;
            for (const wchar_t c : query) {
                if (c == L' ' || c == L'\t') {
                    if (!cur.empty()) {
                        words.push_back(cur);
                        cur.clear();
                    }
                } else {
                    cur.push_back(c);
                }
            }
            if (!cur.empty()) words.push_back(cur);
        }

        bool triggerHit = false;
        std::vector<SearchResult> custom;
        for (size_t i = 0; i < g_cfg.commands.size(); ++i) {
            const CustomEntry& e = g_cfg.commands[i];
            // An entry accepts extra words if it has {query} OR any named placeholder.
            // {query} words are passed directly; named-placeholder words pre-fill the form.
            const bool hasAnyPlaceholder = e.hasQuery || !e.fields.empty();
            const bool triggered =
                !e.trigger.empty() && !words.empty() && FoldString(words[0]) == e.trigger;
            int total = 0;
            int matched = 0;
            std::wstring extra;  // words left over (for {query} or to pre-fill the first field)
            bool ok = true;
            if (triggered) {
                // The alias owns the whole query: everything after it is passed on.
                matched = 1;
                for (size_t k = 1; k < words.size(); ++k) {
                    if (!extra.empty()) extra += L' ';
                    extra += words[k];
                }
            } else {
                for (size_t k = 0; k < g_tokens.size(); ++k) {
                    const std::wstring& t = g_tokens[k];
                    int s = ScoreToken(e.name, e.folded, t, nullptr);
                    if (s <= 0) {
                        if (!e.keywords.empty() && e.keywords.find(t) != std::wstring::npos) {
                            s = 40;  // keyword hit: weaker than a match in the name
                        } else if (hasAnyPlaceholder) {
                            // This word is what the user wants to pass on. The two lists always
                            // have the same length, but guard against an empty tail anyway.
                            if (k < words.size()) {
                                if (!extra.empty()) extra += L' ';
                                extra += words[k];
                            }
                            continue;
                        } else {
                            ok = false;
                            break;
                        }
                    }
                    total += s;
                    ++matched;
                }
            }
            if (!ok || matched == 0) continue;  // the entry itself has to be named in the query
            SearchResult r;
            r.kind = ResultKind::Custom;
            r.custom = (int)i;
            r.score = triggered ? 1000 : total / matched;  // triggers sort above every name hit
            r.title = e.name;
            r.subtitle = CustomTypeLabel(e.type);
            r.payload = std::move(extra);
            custom.push_back(std::move(r));
            if (triggered) triggerHit = true;
        }
        std::sort(custom.begin(), custom.end(), [](const SearchResult& a, const SearchResult& b) {
            if (a.score != b.score) return a.score > b.score;
            return FoldString(a.title) < FoldString(b.title);
        });
        if (triggerHit && !custom.empty()) {
            // Above the apps and selected, so Enter runs it and the rest of the list stays usable.
            g_results.insert(g_results.begin(), std::move(custom.front()));
            custom.erase(custom.begin());
            g_sel = 0;
            g_scrollTarget = 0;
        }
        for (auto& r : custom) g_results.push_back(std::move(r));
    }

    // Cap the number of results. This only limits how many there are; the height of the
    // bar is fixed and rows that do not fit are reached by scrolling.
    if ((int)g_results.size() > g_cfg.maxRows) {
        g_results.resize((size_t)g_cfg.maxRows);
    }

    // Bookmarks: up to three, added after the cap so the app list does not hide them. They go
    // before the Windows Settings and web rows, and never mix with a calculation. Only solid
    // matches (name, prefix or acronym) are blended in: a fuzzy subsequence hit would otherwise
    // put a random bookmark under almost every query. "bm:" still lists everything.
    if (g_cfg.bookmarks && !g_bookmarks.empty() && !CalcOk()) {
        std::vector<SearchResult> bm;
        for (const BookmarkEntry& b : g_bookmarks) {
            const int s = ScoreBookmark(b);
            if (s < 350) continue;
            bm.push_back(MakeBookmarkResult(b, s));
        }
        std::sort(bm.begin(), bm.end(), [](const SearchResult& a, const SearchResult& b) {
            if (a.score != b.score) return a.score > b.score;
            return FoldString(a.title) < FoldString(b.title);
        });
        if (bm.size() > 3) bm.resize(3);
        for (auto& r : bm) g_results.push_back(std::move(r));
    }

    if (loneConst && !g_results.empty()) {
        g_calc = Calc::Result();  // apps matched: drop the constant's result card
        g_calcDisplay.clear();
        g_calcActive = false;
    }
    // Custom Entries manager: surface it in normal search so the user doesn't need ">".
    // Matches on the command's title + keywords, same fuzzy logic as the Settings block.
    if (!query.empty() && !CalcOk()) {
        for (int i = 0; i < kCommandCount; ++i) {
            const CommandDef& c = kCommands[i];
            if (wcscmp(c.id, L"customentries") != 0) continue;
            std::wstring hay = FoldString(c.title);
            hay += L' ';
            hay += FoldString(c.keywords);
            bool ok = true;
            for (const auto& t : g_tokens) {
                if (hay.find(t) == std::wstring::npos) { ok = false; break; }
            }
            if (ok) {
                SearchResult r;
                r.kind    = ResultKind::Command;
                r.title   = c.title;
                r.subtitle = c.category;
                r.payload = c.id;
                r.icon    = i;
                g_results.push_back(std::move(r));
            }
            break;
        }
    }

    // Mode openers ("Open Windows", "Bookmarks", "Clipboard History" ...): typing their name shows a
    // row that enters the mode on Enter, so nobody has to remember a prefix. It lists and filters
    // nothing by itself; the mode does that. Hidden while the feature behind it is off, and capped so
    // it can never flood the list (a calculation suppresses it, like the other blended rows).
    if (!query.empty() && !CalcOk()) {
        int shown = 0;
        for (int i = 0; i < kCommandCount && shown < 3; ++i) {
            const CommandDef& c = kCommands[i];
            if (c.activator < 0 || !ActivatorEnabled(kActivators[c.activator])) continue;
            std::wstring hay = FoldString(c.title);
            hay += L' ';
            hay += FoldString(c.keywords);
            bool ok = true;
            for (const auto& t : g_tokens) {
                if (hay.find(t) == std::wstring::npos) {
                    ok = false;
                    break;
                }
            }
            if (!ok) continue;
            SearchResult r;
            r.kind = ResultKind::Command;
            r.title = c.title;
            r.subtitle = c.category;
            r.payload = c.id;
            r.icon = i;  // index into kCommands, for the badge glyph/colour and the mode switch
            g_results.push_back(std::move(r));
            ++shown;
        }
    }

    // Windows Settings pages that match the query (no prefix needed). Kept after the apps and
    // capped so they never flood the list; a valid calculation suppresses them, like the web row.
    if (!CalcOk()) {
        int shown = 0;
        for (int i = 0; i < kSettingCount && shown < 3; ++i) {
            const SettingDef& sd = kSettings[i];
            std::wstring hay = FoldString(sd.title);
            hay += L' ';
            hay += FoldString(sd.keywords);
            bool ok = true;
            for (const auto& t : g_tokens) {
                if (hay.find(t) == std::wstring::npos) {
                    ok = false;
                    break;
                }
            }
            if (!ok) continue;
            SearchResult r;
            r.kind = ResultKind::Setting;
            r.title = sd.title;
            r.payload = sd.uri;
            r.icon = i;  // index into kSettings, for the badge glyph/colour
            g_results.push_back(std::move(r));
            ++shown;
        }
    }

    // Web: a single result at the end of the list (not offered while the text is a valid
    // calculation, so Enter still copies the result). If the text looks like a URL it opens the
    // site directly; use a "?" prefix to force a search instead.
    if (g_cfg.webSearch && !query.empty() && !CalcOk()) {
        SearchResult w;
        if (LooksLikeUrl(query)) {
            w.kind = ResultKind::Url;
            w.title = L"Open “" + query + L"”";
            w.payload = NormalizeUrl(query);
        } else {
            w.kind = ResultKind::Web;
            w.title = L"Search the web for “" + query + L"”";
            w.subtitle = WebEngineName(g_cfg.webEngine);
            w.icon = (int)g_cfg.webEngine;
            w.payload = WebSearchUrl(g_cfg.webEngine, query);
        }
        g_results.push_back(std::move(w));
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

// ---- Resolving what a shortcut really runs ------------------------------------------------
// Shell properties answer this for most shortcuts, but not for all of them (advertised MSI
// links, links written by older installers, links whose target moved). Reading the .lnk file
// itself is the fallback that lets an app be recognised no matter how its shortcut was made.

static bool StartsWithNoCase(const std::wstring& s, const std::wstring& prefix);  // defined below

static std::wstring ExpandedPath(const std::wstring& p) {
    if (p.empty()) return p;
    wchar_t buf[32768];
    const DWORD n = ExpandEnvironmentStringsW(p.c_str(), buf, (DWORD)ARRAYSIZE(buf));
    if (n > 0 && n <= ARRAYSIZE(buf)) return std::wstring(buf, n - 1);
    return p;
}

// The target of a shortcut, as the shell stores it: possibly quoted and with variables such as
// %ProgramFiles% left unexpanded.
static std::wstring CleanShortcutTarget(std::wstring s) {
    s = TrimBlanks(ExpandedPath(s));
    while (s.size() >= 2 && s.front() == L'"' && s.back() == L'"') {
        s = TrimBlanks(s.substr(1, s.size() - 2));
    }
    return s;
}

// Real, long form of a program path, so two shortcuts to the same file written differently
// ("PROGRA~1", different case, an 8.3 name) are recognised as the same app.
static std::wstring CanonicalAppPath(std::wstring p) {
    p = CleanShortcutTarget(p);
    if (p.empty()) return p;
    wchar_t buf[32768];
    const DWORD n = GetLongPathNameW(p.c_str(), buf, (DWORD)ARRAYSIZE(buf));
    if (n > 0 && n < ARRAYSIZE(buf)) p.assign(buf, n);
    return p;
}

static std::wstring FileNameOf(const std::wstring& p) {
    const size_t i = p.find_last_of(L"\\/");
    return i == std::wstring::npos ? p : p.substr(i + 1);
}

static std::wstring FolderNameOf(const std::wstring& p) {
    const std::wstring dir = ParentDir(p);
    return dir.empty() ? std::wstring() : FileNameOf(dir);
}

static bool ReadShellLink(const std::wstring& lnkPath, std::wstring& target, std::wstring& args) {
    IShellLinkW* link = nullptr;
    if (FAILED(CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&link))) ||
        !link) {
        return false;
    }
    bool ok = false;
    IPersistFile* pf = nullptr;
    if (SUCCEEDED(link->QueryInterface(IID_PPV_ARGS(&pf))) && pf) {
        if (SUCCEEDED(pf->Load(lnkPath.c_str(), STGM_READ))) {
            wchar_t buf[32768];
            WIN32_FIND_DATAW fd = {};
            if (SUCCEEDED(link->GetPath(buf, (int)ARRAYSIZE(buf), &fd, SLGP_RAWPATH)) && buf[0]) {
                target = CanonicalAppPath(buf);
                ok = !target.empty();
            }
            if (SUCCEEDED(link->GetArguments(buf, (int)ARRAYSIZE(buf))) && buf[0]) {
                args = buf;
            }
        }
        pf->Release();
    }
    link->Release();
    return ok;
}

// ---- Windows Recent list ---------------------------------------------------------------
// The Recent folder holds one .lnk per document Windows was asked to open. They are resolved in a
// worker thread (COM/shell calls) and only the shortcuts with a real file or folder target are kept.

static std::wstring RecentFolder() {
    // The shell knows the real (possibly redirected) Recent folder. Fall back to the classic path.
    std::wstring dir = KnownFolderPath(FOLDERID_Recent);
    if (dir.empty()) {
        const std::wstring appData = KnownFolderPath(FOLDERID_RoamingAppData);
        if (!appData.empty()) dir = appData + L"\\Microsoft\\Windows\\Recent";
    }
    return dir;
}

static ULONGLONG RecentFilesStamp(const std::wstring& dir = RecentFolder()) {
    if (dir.empty()) return 0;
    WIN32_FIND_DATAW fd;
    HANDLE h = FindFirstFileW((dir + L"\\*.lnk").c_str(), &fd);
    if (h == INVALID_HANDLE_VALUE) return 0;
    ULONGLONG best = 0;
    do {
        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) continue;
        const ULONGLONG t = ((ULONGLONG)fd.ftLastWriteTime.dwHighDateTime << 32) |
                            fd.ftLastWriteTime.dwLowDateTime;
        if (t > best) best = t;
    } while (FindNextFileW(h, &fd));
    FindClose(h);
    return best;
}

struct RecentCandidate {
    std::wstring path;
    FILETIME used{};
};

static std::vector<RecentEntry> LoadRecentFiles(const std::wstring& dir = RecentFolder()) {
    std::vector<RecentEntry> out;
    if (dir.empty()) return out;
    std::vector<RecentCandidate> candidates;
    WIN32_FIND_DATAW fd;
    HANDLE h = FindFirstFileW((dir + L"\\*.lnk").c_str(), &fd);
    if (h == INVALID_HANDLE_VALUE) return out;
    do {
        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) continue;
        candidates.push_back({dir + L"\\" + fd.cFileName, fd.ftLastWriteTime});
    } while (FindNextFileW(h, &fd));
    FindClose(h);
    std::sort(candidates.begin(), candidates.end(),
              [](const RecentCandidate& a, const RecentCandidate& b) {
                  return CompareFileTime(&a.used, &b.used) > 0;
              });
    if (candidates.size() > 40) candidates.resize(40);
    std::unordered_set<std::wstring> seen;
    for (const RecentCandidate& c : candidates) {
        std::wstring target, args;
        if (!ReadShellLink(c.path, target, args)) continue;
        if (!args.empty()) continue;  // an app launcher, not a document
        const DWORD attr = GetFileAttributesW(target.c_str());
        if (attr == INVALID_FILE_ATTRIBUTES) continue;
        const std::wstring name = FileNameOf(target);
        if (name.empty()) continue;
        if (!seen.insert(FoldString(target)).second) continue;
        RecentEntry e;
        e.name = name;
        e.path = target;
        e.dir = ParentDir(target);
        e.isDir = (attr & FILE_ATTRIBUTE_DIRECTORY) != 0;
        e.ext = e.isDir ? std::wstring() : FileExtOf(name);
        e.folded = FoldString(name + L" " + e.dir);
        e.used = c.used;
        out.push_back(std::move(e));
        if (out.size() >= 40) break;
    }
    return out;
}

struct RecentLoadResult {
    UINT gen;
    ULONGLONG stamp;
    std::vector<RecentEntry> entries;
};

static DWORD WINAPI RecentThreadProc(LPVOID param) {
    const UINT gen = (UINT)(UINT_PTR)param;
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    auto* res = new RecentLoadResult{gen, RecentFilesStamp(), LoadRecentFiles()};
    CoUninitialize();
    if (!g_hwnd || !PostMessageW(g_hwnd, WM_APP_RECENT, 0, (LPARAM)res)) delete res;
    return 0;
}

static void StartRecentLoad() {
    if (!g_cfg.recentFiles || !g_hwnd) return;
    const UINT gen = ++g_recentGen;
    HANDLE th = CreateThread(nullptr, 0, RecentThreadProc, (LPVOID)(UINT_PTR)gen, 0, nullptr);
    if (th) CloseHandle(th);
}

static void RefreshRecentIfStale() {
    if (!g_cfg.recentFiles || !g_hwnd) return;
    const double now = NowSec();
    if (now - g_recentCheckedAt < 5.0) return;
    g_recentCheckedAt = now;
    const ULONGLONG stamp = RecentFilesStamp();
    if (stamp == g_recentStamp && g_recentLoaded) return;
    g_recentStamp = stamp;
    StartRecentLoad();
}

// ---- Global "find:" search (Home plus extra roots) --------------------------------------
// BFS like the file browser's deep search, but with several roots and its own generation so it
// never touches the file mode list.

static bool PathUnder(const std::wstring& path, const std::wstring& root) {
    if (path.size() <= root.size()) return false;
    if (_wcsnicmp(path.c_str(), root.c_str(), root.size()) != 0) return false;
    if (!root.empty() && (root.back() == L'\\' || root.back() == L'/')) return true;
    return path[root.size()] == L'\\' || path[root.size()] == L'/';
}

static std::vector<std::wstring> GlobalRoots() {
    std::vector<std::wstring> roots;
    auto add = [&](std::wstring p) {
        p = TrimBlanks(ExpandedPath(p));
        // Keep a drive root ("C:\") intact; trim trailing separators elsewhere.
        while (p.size() > 3 && (p.back() == L'\\' || p.back() == L'/')) p.pop_back();
        if (p.empty()) return;
        const DWORD attr = GetFileAttributesW(p.c_str());
        if (attr == INVALID_FILE_ATTRIBUTES || !(attr & FILE_ATTRIBUTE_DIRECTORY)) return;
        for (const auto& r : roots) {
            if (SamePath(r, p)) return;
        }
        roots.push_back(std::move(p));
    };
    add(KnownFolderPath(FOLDERID_Profile));
    if (roots.empty()) {
        add(KnownFolderPath(FOLDERID_Documents));
        add(KnownFolderPath(FOLDERID_Downloads));
        add(KnownFolderPath(FOLDERID_Desktop));
    }
    for (const auto& r : g_cfg.searchRoots) add(r);
    // Home already covers Documents/Downloads/...; drop roots nested inside another root.
    std::vector<std::wstring> out;
    for (size_t i = 0; i < roots.size(); ++i) {
        bool nested = false;
        for (size_t j = 0; j < roots.size(); ++j) {
            if (i != j && PathUnder(roots[i], roots[j])) {
                nested = true;
                break;
            }
        }
        if (!nested) out.push_back(roots[i]);
    }
    return out;
}

struct GlobalSearchJob {
    HWND hwnd;
    LONG gen;
    std::wstring key;
    std::vector<std::wstring> roots;
    std::vector<std::wstring> tokens;
    FileFilter filter;  // size/date/extensions, applied as the scan walks
};

struct GlobalSearchResult {
    LONG gen;
    std::wstring key;
    std::vector<FileEntry> entries;
    bool truncated;
    bool done;
};

static void GlobalSearchStarted() {
    if (InterlockedIncrement(&g_globalSearchRunning) == 1 && g_globalSearchIdle) {
        ResetEvent(g_globalSearchIdle);
    }
}

static void GlobalSearchFinished() {
    if (InterlockedDecrement(&g_globalSearchRunning) == 0 && g_globalSearchIdle) {
        SetEvent(g_globalSearchIdle);
    }
}

static DWORD WINAPI GlobalSearchThread(LPVOID param) {
    std::unique_ptr<GlobalSearchJob> job((GlobalSearchJob*)param);
    struct DoneGuard {
        ~DoneGuard() { GlobalSearchFinished(); }
    } doneGuard;
    const size_t kMaxHits = 300, kMaxScanned = 400000;
    const int kMaxDepth = 14;
    const DWORD kBudgetMs = 8000;
    struct Item {
        std::wstring dir;
        int depth;
    };
    struct Hit {
        FileEntry e;
        int score;
    };
    std::vector<Item> queue;
    for (const auto& r : job->roots) queue.push_back({r, 0});
    std::vector<Hit> hits;
    size_t qi = 0, scanned = 0;
    bool truncated = false;
    const DWORD t0 = GetTickCount();
    DWORD lastPost = t0;
    auto cancelled = [&] { return job->gen != g_globalSearchGen; };
    auto post = [&](bool isFinal) {
        std::sort(hits.begin(), hits.end(), [](const Hit& a, const Hit& b) {
            if (a.score != b.score) return a.score > b.score;
            return CompareStringOrdinal(a.e.folded.c_str(), -1, b.e.folded.c_str(), -1, TRUE) ==
                   CSTR_LESS_THAN;
        });
        auto* res = new GlobalSearchResult();
        res->gen = job->gen;
        res->key = job->key;
        res->truncated = truncated;
        res->done = isFinal;
        res->entries.reserve(hits.size());
        for (const auto& ht : hits) res->entries.push_back(ht.e);
        if (!PostMessageW(job->hwnd, WM_APP_GLOBAL, 0, (LPARAM)res)) delete res;
    };

    while (qi < queue.size() && !truncated) {
        if (cancelled()) return 0;
        if (!hits.empty() && GetTickCount() - lastPost > 250) {
            post(false);
            lastPost = GetTickCount();
        }
        const Item it = queue[qi++];
        WIN32_FIND_DATAW fd;
        HANDLE h = FindFirstFileExW(JoinPath(it.dir, L"*").c_str(), FindExInfoBasic, &fd,
                                    FindExSearchNameMatch, nullptr, FIND_FIRST_EX_LARGE_FETCH);
        if (h == INVALID_HANDLE_VALUE) continue;
        do {
            if (SkipFindEntry(fd)) continue;
            ++scanned;
            if ((scanned & 255) == 0) {
                if (cancelled()) {
                    FindClose(h);
                    return 0;
                }
                if (GetTickCount() - t0 > kBudgetMs || scanned > kMaxScanned) {
                    truncated = true;
                    break;
                }
            }
            const bool isDir = (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
            const std::wstring folded = FoldString(fd.cFileName);
            ULARGE_INTEGER fsz;
            fsz.LowPart = fd.nFileSizeLow;
            fsz.HighPart = fd.nFileSizeHigh;
            int score = 100;
            bool ok = FileFilterMatches(job->filter, folded, isDir, fsz.QuadPart,
                                        fd.ftLastWriteTime, false);
            for (size_t k = 0; ok && k < job->tokens.size(); ++k) {
                const size_t pos = folded.find(job->tokens[k]);
                if (pos == std::wstring::npos) {
                    ok = false;
                    break;
                }
                if (k == 0) {
                    if (pos == 0) score = 300;
                    else if (!iswalnum(folded[pos - 1])) score = 200;
                }
            }
            if (ok) {
                Hit ht;
                ht.e = MakeFileEntry(fd, it.dir);
                FileEntryMeta(ht.e);
                ht.score = score + (isDir ? 20 : 0) - it.depth * 15;
                hits.push_back(std::move(ht));
                if (hits.size() >= kMaxHits) {
                    truncated = true;
                    break;
                }
            }
            if (isDir && it.depth < kMaxDepth) {
                const bool link = (fd.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) &&
                                  (fd.dwReserved0 == 0xA000000C || fd.dwReserved0 == 0xA0000003);
                const bool cloud =
                    (fd.dwFileAttributes &
                     (FILE_ATTRIBUTE_OFFLINE | FILE_ATTRIBUTE_RECALL_ON_DATA_ACCESS)) != 0;
                const std::wstring fn = folded;
                if (!link && !cloud && fn != L"node_modules" && fn != L".git") {
                    queue.push_back({JoinPath(it.dir, fd.cFileName), it.depth + 1});
                }
            }
        } while (FindNextFileW(h, &fd));
        FindClose(h);
    }
    if (cancelled()) return 0;
    post(true);
    return 0;
}

static bool GlobalModeActive() {
    // Filters alone are a query too: "find: size:>1mb modified:week" has no words, but it is
    // searching. Without this the bar collapsed and the list never ran.
    return g_activator >= 0 && kActivators[g_activator].mode == LauncherMode::GlobalFiles &&
           (!g_tokens.empty() || g_fileFilter.Active());
}

static void UpdateTargets(bool snap, bool followSelection);  // defined with the layout code
static void MarkDirty();
static void StartGlobalSearch();

// ===== Everything (voidtools) as an optional index =====
// The built-in scan stays the default. With "Everything" selected, the query is sent to the running
// Everything over its IPC: a WM_COPYDATA packet in the format of the official SDK (everything_ipc.h)
// and the answer comes back to this same window, so no DLL is shipped or loaded.
static const wchar_t kEverythingWndClass[] = L"EVERYTHING_TASKBAR_NOTIFICATION";
// The second-generation query: unlike the old one, it also returns the size and the modified date,
// which is what the file list needs to show them and to sort/filter on them.
static const DWORD kEverythingCopyDataQuery2W = 18;  // EVERYTHING_IPC_COPYDATA_QUERY2W
static const DWORD kEvReqName = 0x00000001;           // EVERYTHING_IPC_QUERY2_REQUEST_NAME
static const DWORD kEvReqPath = 0x00000002;           // …_PATH
static const DWORD kEvReqSize = 0x00000010;           // …_SIZE
static const DWORD kEvReqDateModified = 0x00000040;   // …_DATE_MODIFIED
static const DWORD kEverythingItemFolder = 0x00000001;
static const DWORD kEverythingItemDrive = 0x00000002;
static const DWORD kEverythingMaxHits = 1000;  // what we ask Everything for
static const size_t kEverythingMaxShown = 300; // what the list keeps after ranking

#pragma pack(push, 1)
struct EvQuery2W {
    DWORD replyHwnd;
    DWORD replyMessage;
    DWORD searchFlags;
    DWORD offset;
    DWORD maxResults;
    DWORD requestFlags;  // which fields we want back, per item
    DWORD sortType;      // 0 = Everything's default
    wchar_t search[1];
};
struct EvItem2W {
    DWORD flags;
    DWORD dataOffset;  // from the start of the reply, to this item's field block
};
struct EvList2W {
    DWORD totItems, numItems, offset, requestFlags, sortType;
};
#pragma pack(pop)

// Everything answers asynchronously: the reply arrives later as a WM_COPYDATA to this window and is
// finished in WM_APP_EVERYTHING (verified against a running 1.5). The reply is matched to its query by
// the sequence number carried in dwData (see g_evSeq), so no registered message is needed.
static std::vector<BYTE> g_evReply;  // raw reply of the last query
static HWND g_evWindow = nullptr;    // cached Everything notification window
static double g_evRetryAt = 0;       // wait before probing a missing Everything again
static bool g_evPending = false;     // a query is in flight, waiting for its answer
static DWORD g_evSeq = 0;            // id of the query in flight, echoed back by Everything
static DWORD g_evPendingSeq = 0;
static std::wstring g_evPendingKey;  // the search key the answer belongs to
static LONG g_evPendingGen = 0;      // the search generation it was sent for
static double g_evFailedAt = -1e9;   // last time Everything could not be used (footer notice)

// Where the answer in flight belongs: the "find:" list or the deep search inside Files.
enum class EvTarget { Global, Files };
static EvTarget g_evTarget = EvTarget::Global;

static HWND EverythingWindow() {
    if (g_evWindow && IsWindow(g_evWindow)) return g_evWindow;
    g_evWindow = FindWindowW(kEverythingWndClass, nullptr);
    return g_evWindow;
}

// Everything answers asynchronously: the reply arrives later as a WM_COPYDATA to this window and is
// finished in WM_APP_EVERYTHING (verified against a running 1.5).
static bool EverythingSendQuery(const std::wstring& query, DWORD seq) {
    if (!g_hwnd || query.empty()) return false;
    if (NowSec() < g_evRetryAt) {  // still cooling down after a failure
        g_evFailedAt = NowSec();
        return false;
    }
    if (!EverythingWindow()) {
        // Not running, or not installed at all. FindWindow is the cheap check (the handle is cached
        // while it lives), and the long cooldown keeps a typist from probing it on every keystroke.
        g_evRetryAt = NowSec() + 30.0;
        g_evFailedAt = NowSec();
        return false;
    }
    const size_t chars = query.size() + 1;
    std::vector<BYTE> packet(sizeof(EvQuery2W) - sizeof(wchar_t) + chars * sizeof(wchar_t), 0);
    auto* q = (EvQuery2W*)packet.data();
    q->replyHwnd = (DWORD)(DWORD_PTR)g_hwnd;
    q->replyMessage = seq;  // echoed back in dwData, so a stale answer is recognisable
    q->searchFlags = 0;     // case-insensitive name match: Everything's own default
    q->offset = 0;
    q->maxResults = kEverythingMaxHits;
    q->requestFlags = kEvReqName | kEvReqPath | kEvReqSize | kEvReqDateModified;
    q->sortType = 0;  // the list re-ranks the results itself
    memcpy(q->search, query.c_str(), chars * sizeof(wchar_t));

    COPYDATASTRUCT cds = {};
    cds.dwData = kEverythingCopyDataQuery2W;
    cds.cbData = (DWORD)packet.size();
    cds.lpData = packet.data();
    DWORD_PTR ignored = 0;
    const LRESULT sent = SendMessageTimeoutW(EverythingWindow(), WM_COPYDATA, (WPARAM)g_hwnd,
                                             (LPARAM)&cds, SMTO_ABORTIFHUNG, 500, &ignored);
    if (!sent) {
        g_evWindow = nullptr;  // it may have restarted: look it up again next time
        g_evRetryAt = NowSec() + 5.0;
        g_evFailedAt = NowSec();
    }
    return sent != 0;
}

// One item's fields: a length-prefixed string or a fixed-size value per requested flag, in bit
// order. `requestFlags` comes from the reply (Everything may fill in more than we asked for), so
// that is what drives the walk instead of our own request.
struct EvFields {
    std::wstring name;
    std::wstring path;
    long long size = 0;
    FILETIME modified = {};
    bool hasSize = false;
    bool hasModified = false;
};

static bool EvReadItem(const std::vector<BYTE>& buf, size_t pos, DWORD flags, EvFields& f) {
    auto text = [&](std::wstring* dst) {
        if (pos + 4 > buf.size()) return false;
        DWORD len = 0;
        memcpy(&len, buf.data() + pos, 4);
        pos += 4;
        if (len > 4096 || pos + ((size_t)len + 1) * sizeof(wchar_t) > buf.size()) return false;
        if (dst) {
            dst->resize(len);
            if (len) memcpy(&(*dst)[0], buf.data() + pos, (size_t)len * sizeof(wchar_t));
        }
        pos += ((size_t)len + 1) * sizeof(wchar_t);
        return true;
    };
    auto take = [&](size_t n, void* dst) {
        if (pos + n > buf.size()) return false;
        if (dst) memcpy(dst, buf.data() + pos, n);
        pos += n;
        return true;
    };
    if ((flags & 0x00000001) && !text(&f.name)) return false;  // NAME
    if ((flags & 0x00000002) && !text(&f.path)) return false;  // PATH
    if ((flags & 0x00000004) && !text(nullptr)) return false;  // FULL_PATH_AND_NAME
    if ((flags & 0x00000008) && !text(nullptr)) return false;  // EXTENSION
    if (flags & 0x00000010) {                                  // SIZE
        if (!take(8, &f.size)) return false;
        f.hasSize = true;
    }
    if ((flags & 0x00000020) && !take(8, nullptr)) return false;  // DATE_CREATED
    if (flags & 0x00000040) {                                     // DATE_MODIFIED
        if (!take(8, &f.modified)) return false;
        f.hasModified = true;
    }
    if ((flags & 0x00000080) && !take(8, nullptr)) return false;  // DATE_ACCESSED
    if ((flags & 0x00000100) && !take(4, nullptr)) return false;  // ATTRIBUTES
    if ((flags & 0x00000200) && !text(nullptr)) return false;     // FILE_LIST_FILE_NAME
    if ((flags & 0x00000400) && !take(4, nullptr)) return false;  // RUN_COUNT
    if ((flags & 0x00000800) && !take(8, nullptr)) return false;  // DATE_RUN
    if ((flags & 0x00001000) && !take(8, nullptr)) return false;  // DATE_RECENTLY_CHANGED
    if ((flags & 0x00002000) && !text(nullptr)) return false;     // HIGHLIGHTED_NAME
    if ((flags & 0x00004000) && !text(nullptr)) return false;     // HIGHLIGHTED_PATH
    if ((flags & 0x00008000) && !text(nullptr)) return false;     // HIGHLIGHTED_FULL_PATH_AND_NAME
    return true;
}

// Turns the last reply into entries: name, folder, size and modified date.
static void EverythingParseReply(std::vector<FileEntry>& out, bool& truncated) {
    out.clear();
    truncated = false;
    if (g_evReply.size() < sizeof(EvList2W)) return;
    EvList2W list;
    memcpy(&list, g_evReply.data(), sizeof(list));
    // The reply can come from any process at our integrity level that guesses the sequence number,
    // so the item array is sized against what actually arrived.
    const size_t fitting = (g_evReply.size() - sizeof(EvList2W)) / sizeof(EvItem2W);
    truncated = list.totItems > list.numItems;
    const DWORD n =
        (DWORD)std::min<size_t>(std::min<DWORD>(list.numItems, kEverythingMaxHits), fitting);
    out.reserve(n);
    for (DWORD i = 0; i < n; ++i) {
        EvItem2W it;
        memcpy(&it, g_evReply.data() + sizeof(EvList2W) + i * sizeof(EvItem2W), sizeof(it));
        if (it.flags & kEverythingItemDrive) continue;  // "C:" rows are noise here
        EvFields f;
        if (!EvReadItem(g_evReply, it.dataOffset, list.requestFlags, f)) continue;
        if (f.name.empty()) continue;
        FileEntry e;
        e.name = f.name;
        e.folded = FoldString(f.name);
        e.isDir = (it.flags & kEverythingItemFolder) != 0;
        e.dir = f.path;
        e.path = f.path.empty() ? f.name : JoinPath(f.path, f.name);
        e.size = (uint64_t)f.size;  // 0 for folders, which is what the list shows anyway
        e.writeTime = f.modified;
        e.noInfo = !(f.hasSize && f.hasModified);  // only claim what the reply actually carried
        if (!e.isDir) e.ext = FileExtOf(f.name);
        out.push_back(std::move(e));
    }
}

// Takes the answer that just landed and asks the UI to finish the search.
static void AcceptEverythingReply(const COPYDATASTRUCT* cds) {
    if (!g_evPending || !cds || !cds->lpData) return;
    if (cds->dwData != g_evPendingSeq) return;  // an answer to a query we already replaced
    const BYTE* p = (const BYTE*)cds->lpData;
    g_evReply.assign(p, p + cds->cbData);
    if (g_hwnd) PostMessageW(g_hwnd, WM_APP_EVERYTHING, 0, 0);
}

// The text typed after "find:", as typed: Everything understands its own search syntax, and the
// folded tokens would lose accents and quotes.
static std::wstring GlobalRawQuery() {
    if (g_activator < 0) return std::wstring();
    const std::wstring trigger = kActivators[g_activator].trigger;
    const size_t a = g_text.find_first_not_of(L" \t");
    if (a == std::wstring::npos) return std::wstring();
    const std::wstring t = g_text.substr(a);
    if (t.size() <= trigger.size()) return std::wstring();
    const size_t q = t.find_first_not_of(L" \t", trigger.size());
    return q == std::wstring::npos ? std::wstring() : t.substr(q);
}

// Everything indexes the Recycle Bin and the volume metadata; neither belongs in a file search.
static bool IsJunkPath(const std::wstring& path) {
    const std::wstring low = FoldString(path);
    return low.find(L"\\$recycle.bin") != std::wstring::npos ||
           low.find(L"\\system volume information") != std::wstring::npos;
}

// Our filters in Everything's own syntax, so the index does the work instead of us filtering a
// thousand arbitrary rows. Sizes go as bytes; dates go as the same local calendar date.
static std::wstring EverythingFilterTerms(const FileFilter& f) {
    std::wstring out;
    auto add = [&](const std::wstring& term) {
        if (!out.empty()) out += L' ';
        out += term;
    };
    if (f.hasSize) {
        if (f.minSize > 0) add(L"size:>=" + std::to_wstring(f.minSize));
        if (f.maxSize < ~0ull) add(L"size:<=" + std::to_wstring(f.maxSize));
    }
    if (f.hasAfter) {
        const std::wstring d = TicksToLocalDate(f.afterTime);
        if (!d.empty()) add(L"datemodified:>=" + d);
    }
    if (f.hasBefore) {
        const std::wstring d = TicksToLocalDate(f.beforeTime);
        if (!d.empty()) add(L"datemodified:<" + d);
    }
    return out;
}

// The typed text minus the tokens that became filters. Everything would read "modified:today" as a
// literal word and return nothing; its own syntax (quotes, wildcards) is left untouched.
static std::wstring StripFileFilters(const std::wstring& rawIn) {
    const std::wstring raw = JoinFilterPrefixes(rawIn);
    std::wstring out;
    size_t i = 0;
    while (i < raw.size()) {
        const size_t s = raw.find_first_not_of(L' ', i);
        if (s == std::wstring::npos) break;
        const size_t e = raw.find(L' ', s);
        const std::wstring tok = raw.substr(s, e == std::wstring::npos ? std::wstring::npos : e - s);
        const std::wstring low = FoldString(tok);
        const bool isFilter = low.compare(0, 5, L"size:") == 0 ||
                              low.compare(0, 9, L"modified:") == 0 ||
                              low.compare(0, 5, L"date:") == 0;
        if (!isFilter) {
            if (!out.empty()) out += L' ';
            out += tok;
        }
        i = (e == std::wstring::npos) ? raw.size() : e + 1;
    }
    return out;
}

// Sends the current query to Everything; the answer arrives later (WM_APP_EVERYTHING). False means
// "use the built-in scan instead".
static bool StartEverythingSearch() {
    if (!GlobalModeActive() || !g_hwnd) return false;
    const DWORD seq = ++g_evSeq;
    std::wstring q = StripFileFilters(GlobalRawQuery());
    const std::wstring terms = EverythingFilterTerms(g_fileFilter);
    if (!terms.empty()) q = q.empty() ? terms : q + L' ' + terms;
    if (q.empty()) q = L"*";  // only filters were typed and none translated: match everything
    if (!EverythingSendQuery(q, seq)) return false;
    g_evReply.clear();
    g_evPending = true;
    g_evPendingSeq = seq;
    g_evPendingKey = GlobalQueryKey();
    g_evPendingGen = g_globalSearchGen;
    g_evTarget = EvTarget::Global;
    g_globalSearching = true;
    SetTimer(g_hwnd, TIMER_EVFALLBACK, 900, nullptr);
    MarkDirty();  // the path band shows "Searching…" while the index answers
    return true;
}

// The Files deep search, answered by Everything restricted to the folder being browsed. Same wire
// format, different target: the answer lands in g_fileDeep through WM_APP_EVERYTHING.
static bool StartEverythingFileSearch() {
    if (!g_hwnd || g_fileCur.empty()) return false;
    std::wstring query = L"path:\"" + g_fileCur + L"\"";
    for (const auto& t : g_tokens) {
        query += L' ';
        query += t;
    }
    const DWORD seq = ++g_evSeq;
    if (!EverythingSendQuery(query, seq)) return false;
    g_evReply.clear();
    g_evPending = true;
    g_evPendingSeq = seq;
    g_evPendingKey = FileSearchKey();
    g_evPendingGen = g_fileSearchGen;
    g_evTarget = EvTarget::Files;
    g_fileSearching = true;
    SetTimer(g_hwnd, TIMER_EVFALLBACK, 900, nullptr);
    MarkDirty();
    return true;
}

// Closest match first, the rule the built-in deep search uses: the first word at the start of the
// name, then at a word start, then anywhere; folders get a small nudge and shallower results win, so
// a folder near the one you are browsing beats a deep temporary copy with the same name.
static int DeepFileScore(const FileEntry& e, const std::wstring& root) {
    int score = 100;
    if (!g_tokens.empty()) {
        const size_t pos = e.folded.find(g_tokens[0]);
        if (pos == 0) {
            score = 300;
        } else if (pos != std::wstring::npos && !iswalnum(e.folded[pos - 1])) {
            score = 200;
        }
    }
    int depth = 0;
    for (size_t i = root.size(); i < e.path.size(); ++i) {
        if (e.path[i] == L'\\') ++depth;
    }
    score -= std::min(depth, 8) * 15;
    return score + (e.isDir ? 20 : 0);
}

// Publishes the deep-search answer into the Files list.
static void FinishEverythingFileSearch(LONG gen, const std::wstring& key) {
    if (gen != g_fileSearchGen || !g_fileMode || g_fileCur.empty() || key != FileSearchKey()) {
        return;  // the folder or the words changed while Everything was answering
    }
    std::vector<FileEntry> hits;
    bool truncated = false;
    EverythingParseReply(hits, truncated);
    // "path:" is a text match, not a tree walk, and the local filters are not applied to deep
    // results: both are enforced here. A drive root already ends with a separator ("C:\"), every
    // other folder needs one, or nothing would ever match.
    const std::wstring prefix =
        (!g_fileCur.empty() && g_fileCur.back() == L'\\') ? g_fileCur : g_fileCur + L'\\';
    std::vector<FileEntry> kept;
    kept.reserve(hits.size());
    for (FileEntry& e : hits) {
        if (e.path.size() <= prefix.size()) continue;
        if (CompareStringOrdinal(e.path.c_str(), (int)prefix.size(), prefix.c_str(),
                                 (int)prefix.size(), TRUE) != CSTR_EQUAL) {
            continue;  // e.g. C:\Temp2 when browsing C:\Temp
        }
        if (!FileFilterMatches(g_fileFilter, e)) continue;
        FileEntryMeta(e);  // formatted once, kept in the snapshot
        kept.push_back(std::move(e));
    }
    std::stable_sort(kept.begin(), kept.end(), [&prefix](const FileEntry& a, const FileEntry& b) {
        return DeepFileScore(a, prefix) > DeepFileScore(b, prefix);
    });
    if (kept.size() > kEverythingMaxShown) {
        kept.resize(kEverythingMaxShown);
        truncated = true;
    }

    const int oldSel = g_sel;
    std::wstring keep;
    if (oldSel > 0 && oldSel < (int)g_results.size()) keep = g_results[oldSel].payload;
    ReleaseResultLayouts();
    g_results.clear();
    g_fileDeep = std::move(kept);
    g_fileDeepKey = g_evPendingKey;
    g_fileDeepTrunc = truncated;
    g_fileDeepDone = true;
    g_fileSearching = false;
    RunSearch();
    int restored = -1;
    for (size_t i = 0; i < g_results.size() && !keep.empty(); ++i) {
        if (g_results[i].payload == keep) {
            restored = (int)i;
            break;
        }
    }
    g_sel = restored >= 0 ? restored : std::min(oldSel, (int)g_results.size() - 1);
    if (g_sel < 0) g_sel = 0;
    if (restored < 0 && oldSel == 0) g_scrollTarget = 0;
    UpdateTargets(false, /*followSelection=*/restored >= 0);
    MarkDirty();
}

// Publishes the answer that just arrived, ranked like the built-in scan.
static void FinishEverythingSearch() {
    if (!g_evPending) return;
    const EvTarget target = g_evTarget;
    const LONG gen = g_evPendingGen;
    const std::wstring key = g_evPendingKey;
    g_evPending = false;
    if (g_hwnd) KillTimer(g_hwnd, TIMER_EVFALLBACK);
    if (target == EvTarget::Files) {
        FinishEverythingFileSearch(gen, key);
        return;
    }
    const bool stale = gen != g_globalSearchGen || key != GlobalQueryKey() || g_activator < 0 ||
                       kActivators[g_activator].mode != LauncherMode::GlobalFiles;
    if (stale) return;
    std::vector<FileEntry> hits;
    bool truncated = false;
    EverythingParseReply(hits, truncated);

    // The filter tokens were left out of the Everything query, so they are applied here, on the
    // size and date that the second-generation reply carries. Junk paths are dropped either way.
    {
        std::vector<FileEntry> kept;
        kept.reserve(hits.size());
        for (FileEntry& e : hits) {
            if (IsJunkPath(e.path)) continue;
            if (g_fileFilter.Active() && !FileFilterMatches(g_fileFilter, e)) continue;
            kept.push_back(std::move(e));
        }
        hits.swap(kept);
    }

    // Closest matches first, the same rule as the built-in scan: everything that matched the words
    // in the name scores, the rest keeps Everything's order at the bottom.
    std::vector<int> score(hits.size(), 0);
    for (size_t i = 0; i < hits.size(); ++i) {
        for (const auto& t : g_tokens) {
            const int s = ScoreToken(hits[i].name, hits[i].folded, t, nullptr);
            if (s > 0) score[i] += s;
        }
    }
    std::vector<size_t> order(hits.size());
    for (size_t i = 0; i < order.size(); ++i) order[i] = i;
    std::sort(order.begin(), order.end(), [&](size_t a, size_t b) {
        if (score[a] != score[b]) return score[a] > score[b];
        return CompareStringOrdinal(hits[a].folded.c_str(), -1, hits[b].folded.c_str(), -1, TRUE) ==
               CSTR_LESS_THAN;
    });
    std::vector<FileEntry> sorted;
    sorted.reserve(hits.size());
    for (const size_t i : order) sorted.push_back(std::move(hits[i]));
    if (sorted.size() > kEverythingMaxShown) {
        sorted.resize(kEverythingMaxShown);
        truncated = true;
    }

    const int oldSel = g_sel;
    std::wstring keep;
    if (oldSel >= 0 && oldSel < (int)g_results.size()) keep = g_results[oldSel].payload;
    g_globalFiles = std::move(sorted);
    g_globalKey = GlobalQueryKey();
    g_globalDone = true;
    g_globalSearching = false;
    g_globalTrunc = truncated;
    RunSearch();
    int restored = -1;
    for (size_t i = 0; i < g_results.size() && !keep.empty(); ++i) {
        if (g_results[i].payload == keep) {
            restored = (int)i;
            break;
        }
    }
    g_sel = restored >= 0 ? restored : std::min(oldSel, (int)g_results.size() - 1);
    if (g_sel < 0) g_sel = 0;
    UpdateTargets(false, /*followSelection=*/restored >= 0);
    MarkDirty();
}

// Everything never answered (it was closed or hung mid-query): drop it and scan instead.
static void AbandonEverythingSearch() {
    if (!g_evPending) return;
    g_evPending = false;
    if (g_hwnd) KillTimer(g_hwnd, TIMER_EVFALLBACK);
    g_evRetryAt = NowSec() + 5.0;
    g_evFailedAt = NowSec();
    StartGlobalSearch();  // Everything is now rate-limited, so this starts the built-in scan
}

static void ResetEverythingSearch() {
    g_evPending = false;
    if (g_hwnd) KillTimer(g_hwnd, TIMER_EVFALLBACK);
}

static void CancelGlobalSearch() {
    InterlockedIncrement(&g_globalSearchGen);
    g_globalSearching = false;
    ResetEverythingSearch();
    if (g_hwnd) KillTimer(g_hwnd, TIMER_GSEARCH);
}

static void ScheduleGlobalSearch() {
    if (!GlobalModeActive()) {
        CancelGlobalSearch();
        return;
    }
    if (g_globalDone && g_globalKey == GlobalQueryKey()) {
        CancelGlobalSearch();
        return;
    }
    InterlockedIncrement(&g_globalSearchGen);
    g_globalSearching = true;
    g_globalDone = false;
    if (g_hwnd) SetTimer(g_hwnd, TIMER_GSEARCH, 160, nullptr);
}

static void StartGlobalSearch() {
    if (!GlobalModeActive() || !g_hwnd) {
        g_globalSearching = false;
        return;
    }
    if (g_cfg.fileSearchEngine == FileSearchEngine::Everything && StartEverythingSearch()) {
        return;  // the index answered; nothing to scan
    }
    auto* job = new GlobalSearchJob();
    job->hwnd = g_hwnd;
    job->gen = InterlockedIncrement(&g_globalSearchGen);
    job->key = GlobalQueryKey();
    job->roots = GlobalRoots();
    job->tokens = g_tokens;
    job->filter = g_fileFilter;
    GlobalSearchStarted();
    HANDLE th = CreateThread(nullptr, 0, GlobalSearchThread, job, 0, nullptr);
    if (!th) {
        GlobalSearchFinished();
        delete job;
        g_globalSearching = false;
    }
}

// Two entries are the same app when they start the same program with the same arguments, or are
// the same packaged app. The display name is deliberately NOT part of this: two different apps
// can share a name, and the same app is often listed more than once (Start menu and Desktop).
static std::wstring AppIdentity(const RawApp& a) {
    if (IsPackagedId(a.id)) return L"package:" + FoldString(a.id);
    const std::wstring target = FoldString(CanonicalAppPath(a.target));
    if (!target.empty()) {
        return L"target:" + target + L"|" + FoldString(TrimBlanks(a.args));
    }
    return L"id:" + FoldString(a.id);
}

// A short suffix that tells two apps with the same name apart ("Outlook (Microsoft Office)",
// "Outlook (Store)"). Only used when a name really repeats.
static std::wstring AppQualifier(const RawApp& a) {
    if (IsPackagedId(a.id)) return L"Store";
    std::wstring path = CanonicalAppPath(a.target);
    if (path.empty()) path = a.launch;
    if (path.empty()) return L"App";

    static const wchar_t* const kGeneric[] = {
        L"windows",   L"system32",    L"syswow64", L"windowsapps", L"program files",
        L"program files (x86)", L"programs", L"appdata", L"local", L"roaming",
        L"desktop",   L"users",       L"public",   L"bin",        L"current",
        L"versions",
    };
    std::wstring folder = FolderNameOf(path);
    if (!folder.empty()) {
        const std::wstring f = FoldString(folder);
        bool generic = false;
        for (const wchar_t* g : kGeneric) {
            if (f == g) {
                generic = true;
                break;
            }
        }
        if (!generic) return folder;
    }
    std::wstring file = FileNameOf(path);
    const size_t dot = file.find_last_of(L'.');
    if (dot != std::wstring::npos && dot > 0) file = file.substr(0, dot);
    return file.empty() ? std::wstring(L"App") : file;
}

// True for a target that starts a program (not a document, folder or shell namespace id).
static bool IsProgramTarget(const std::wstring& target) {
    static const wchar_t* const kExt[] = {L".exe", L".com", L".bat", L".cmd", L".msc"};
    const std::wstring t = FoldString(target);
    for (const wchar_t* e : kExt) {
        if (EndsWith(t, e)) return true;
    }
    return false;
}

// %LOCALAPPDATA%\Microsoft\WindowsApps holds the execution aliases of packaged apps. A Desktop
// shortcut to one of them would duplicate the app that the Start menu already lists under its
// AUMID.
static bool IsStoreAliasPath(const std::wstring& path) {
    wchar_t local[MAX_PATH];
    const DWORD n = GetEnvironmentVariableW(L"LOCALAPPDATA", local, MAX_PATH);
    if (n == 0 || n >= MAX_PATH) return false;
    return StartsWithNoCase(path, std::wstring(local) + L"\\Microsoft\\WindowsApps\\");
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

// Scales a decoded frame down to `maxSide` (cover-cropped to a square when `centerSquare`) and copies
// it out as premultiplied BGRA. Shared by file previews and the clipboard image thumbnails.
static bool ScaleFrameToPixels(IWICImagingFactory* wic,
                               IWICBitmapSource* frame,
                               UINT fw,
                               UINT fh,
                               bool centerSquare,
                               UINT maxSide,
                               std::vector<uint32_t>& px,
                               int& w,
                               int& h) {
    IWICBitmapClipper* clip = nullptr;
    IWICBitmapScaler* scaler = nullptr;
    IWICFormatConverter* conv = nullptr;
    bool ok = false;
    IWICBitmapSource* src = frame;
    UINT sw = fw, sh = fh;
    if (centerSquare && fw != fh) {
        const UINT side = std::min(fw, fh);
        WICRect rc = {(INT)((fw - side) / 2), (INT)((fh - side) / 2), (INT)side, (INT)side};
        if (SUCCEEDED(wic->CreateBitmapClipper(&clip)) && clip &&
            SUCCEEDED(clip->Initialize(frame, &rc))) {
            src = clip;
            sw = sh = side;
        }
    }
    // Only refuse absurd images (e.g. a 30000x30000 scan) instead of decoding them.
    if ((uint64_t)sw * sh <= 64ull * 1000 * 1000) {
        UINT tw = sw, th = sh;
        if (sw > maxSide || sh > maxSide) {
            if (sw >= sh) {
                tw = maxSide;
                th = std::max<UINT>(1, (UINT)((uint64_t)sh * maxSide / sw));
            } else {
                th = maxSide;
                tw = std::max<UINT>(1, (UINT)((uint64_t)sw * maxSide / sh));
            }
        }
        const bool scaled = tw != sw || th != sh;
        if (!scaled || (SUCCEEDED(wic->CreateBitmapScaler(&scaler)) && scaler &&
                        SUCCEEDED(scaler->Initialize(src, tw, th, WICBitmapInterpolationModeFant)))) {
            if (scaled) src = scaler;
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
    SafeRelease(clip);
    return ok;
}

// Loads an image with WIC as premultiplied BGRA, at most 64 px on its longest side. With
// `centerSquare` the source is first cropped to its largest centered square: previews are drawn
// cover-fitted into a square, so huge panoramas and long screenshots now decode fine.
static bool LoadImageFilePixels(IWICImagingFactory* wic,
                                const std::wstring& path,
                                std::vector<uint32_t>& px,
                                int& w,
                                int& h,
                                bool centerSquare = false) {
    if (!wic) return false;
    IWICBitmapDecoder* dec = nullptr;
    IWICBitmapFrameDecode* frame = nullptr;
    bool ok = false;
    if (SUCCEEDED(wic->CreateDecoderFromFilename(path.c_str(), nullptr, GENERIC_READ,
                                                 WICDecodeMetadataCacheOnDemand, &dec)) &&
        dec && SUCCEEDED(dec->GetFrame(0, &frame)) && frame) {
        UINT fw = 0, fh = 0;
        frame->GetSize(&fw, &fh);
        ok = ScaleFrameToPixels(wic, frame, fw, fh, centerSquare, 64, px, w, h);
    }
    SafeRelease(frame);
    SafeRelease(dec);
    return ok;
}

static std::wstring KnownFolderPath(REFKNOWNFOLDERID id);  // defined with the folder watcher

// Reads a file and decrypts it; our clipboard images are DPAPI-protected on disk.
static bool ReadProtectedFile(const std::wstring& path, std::vector<BYTE>& out) {
    HANDLE f = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING,
                           FILE_ATTRIBUTE_NORMAL, nullptr);
    if (f == INVALID_HANDLE_VALUE) return false;
    LARGE_INTEGER size = {};
    bool ok = false;
    if (GetFileSizeEx(f, &size) && size.QuadPart > 0 && size.QuadPart < 64ll * 1024 * 1024) {
        std::vector<BYTE> enc((size_t)size.QuadPart);
        DWORD read = 0;
        ok = ReadFile(f, enc.data(), (DWORD)enc.size(), &read, nullptr) &&
             read == (DWORD)enc.size() && UnprotectBytes(enc, out);
    }
    CloseHandle(f);
    return ok;
}

static bool WriteFileBytes(const std::wstring& path, const std::vector<BYTE>& data) {
    HANDLE f = CreateFileW(path.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS,
                           FILE_ATTRIBUTE_NORMAL, nullptr);
    if (f == INVALID_HANDLE_VALUE) return false;
    DWORD written = 0;
    const bool ok = WriteFile(f, data.data(), (DWORD)data.size(), &written, nullptr) &&
                    written == (DWORD)data.size();
    CloseHandle(f);
    if (!ok) DeleteFileW(path.c_str());
    return ok;
}

// A WIC frame decoded from memory, with everything it needs kept alive.
struct DecodedImage {
    IStream* stream = nullptr;
    IWICBitmapDecoder* decoder = nullptr;
    IWICBitmapFrameDecode* frame = nullptr;
    ~DecodedImage() {
        SafeRelease(frame);
        SafeRelease(decoder);
        SafeRelease(stream);
    }
};

static bool DecodeImageBytes(IWICImagingFactory* wic, const std::vector<BYTE>& bytes, DecodedImage& out) {
    if (!wic || bytes.empty()) return false;
    HGLOBAL mem = GlobalAlloc(GMEM_MOVEABLE, bytes.size());
    if (!mem) return false;
    void* p = GlobalLock(mem);
    if (!p) {
        GlobalFree(mem);
        return false;
    }
    memcpy(p, bytes.data(), bytes.size());
    GlobalUnlock(mem);
    if (FAILED(CreateStreamOnHGlobal(mem, TRUE, &out.stream))) {
        GlobalFree(mem);
        return false;
    }
    UINT fw = 0, fh = 0;
    if (SUCCEEDED(wic->CreateDecoderFromStream(out.stream, nullptr, WICDecodeMetadataCacheOnDemand,
                                               &out.decoder)) &&
        out.decoder && SUCCEEDED(out.decoder->GetFrame(0, &out.frame)) && out.frame) {
        out.frame->GetSize(&fw, &fh);
    }
    return fw > 0 && fh > 0;
}

// Thumbnail for a clipboard image: decrypted in memory, never written to a temporary file.
static bool LoadClipThumbPixels(IWICImagingFactory* wic,
                                const std::wstring& path,
                                std::vector<uint32_t>& px,
                                int& w,
                                int& h) {
    std::vector<BYTE> png;
    if (!ReadProtectedFile(path, png)) return false;
    DecodedImage img;
    if (!DecodeImageBytes(wic, png, img)) return false;
    UINT fw = 0, fh = 0;
    img.frame->GetSize(&fw, &fh);
    return ScaleFrameToPixels(wic, img.frame, fw, fh, /*centerSquare=*/true, 64, px, w, h);
}

static DWORD WINAPI IndexThreadProc(LPVOID param) {
    std::unique_ptr<IndexJob> job((IndexJob*)param);
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    struct Tmp {
        std::wstring folded;
        int priority = 0;  // higher wins when two sources list the same app
        RawApp app;
        IShellItem* item = nullptr;
    };
    std::vector<Tmp> tmp;
    std::unordered_map<std::wstring, size_t> byIdentity;

    // Keeps the first entry for each app. A later entry from a preferred source (the Start menu)
    // replaces an earlier one (the Desktop) without changing the position, so the order stays
    // stable while the list is being built.
    auto addApp = [&](RawApp&& app, IShellItem* item, int priority) {
        std::wstring identity = AppIdentity(app);
        auto it = byIdentity.find(identity);
        if (it != byIdentity.end()) {
            Tmp& cur = tmp[it->second];
            if (priority > cur.priority) {
                SafeRelease(cur.item);
                cur.app = std::move(app);
                cur.item = item;
                cur.priority = priority;
                cur.folded = FoldString(cur.app.name);
            } else {
                item->Release();
            }
            return;
        }
        Tmp t;
        t.folded = FoldString(app.name);
        t.priority = priority;
        t.app = std::move(app);
        t.item = item;
        byIdentity[identity] = tmp.size();
        tmp.push_back(std::move(t));
    };

    // ---- Start menu (and packaged apps) through the shell's own app list ----
    IShellItem* folder = nullptr;
    HRESULT hr = SHCreateItemFromParsingName(L"shell:AppsFolder", nullptr, IID_PPV_ARGS(&folder));
    if (SUCCEEDED(hr) && folder) {
        IEnumShellItems* en = nullptr;
        hr = folder->BindToHandler(nullptr, BHID_EnumItems, IID_PPV_ARGS(&en));
        if (SUCCEEDED(hr) && en) {
            IShellItem* it = nullptr;
            ULONG got = 0;
            while (en->Next(1, &it, &got) == S_OK && got == 1) {
                PWSTR dn = nullptr;
                PWSTR pn = nullptr;
                it->GetDisplayName(SIGDN_NORMALDISPLAY, &dn);
                it->GetDisplayName(SIGDN_PARENTRELATIVEPARSING, &pn);
                bool keep = dn && pn && *dn && *pn;
                RawApp ra;
                if (dn) ra.name = dn;
                if (pn) ra.id = pn;
                if (dn) CoTaskMemFree(dn);
                if (pn) CoTaskMemFree(pn);

                if (keep && job->hideUtility && ClassifyUtility(it, ra.name, ra.id) != UtilNone) {
                    keep = false;
                }

                if (keep && !IsPackagedId(ra.id)) {
                    IShellItem2* it2 = nullptr;
                    if (SUCCEEDED(it->QueryInterface(IID_PPV_ARGS(&it2))) && it2) {
                        ra.target = CanonicalAppPath(ItemString(it2, L"System.Link.TargetParsingPath"));
                        ra.args = ItemString(it2, L"System.Link.Arguments");
                        it2->Release();
                    }
                    if (ra.target.empty()) {
                        // Advertised MSI links and a few older shortcuts do not expose their
                        // target as a property; the .lnk file always knows it.
                        std::wstring lnk = PathFromAppId(ra.id);
                        if (EndsWith(FoldString(lnk), L".lnk")) {
                            std::wstring t, a;
                            if (ReadShellLink(lnk, t, a)) {
                                ra.target = t;
                                if (ra.args.empty()) ra.args = a;
                            }
                        }
                    }
                }

                if (keep) {
                    addApp(std::move(ra), it, 2);
                } else {
                    it->Release();
                }
                it = nullptr;
            }
            en->Release();
        }
        folder->Release();
    }

    // ---- Desktop shortcuts ----
    // Programs are often launched from a Desktop shortcut that was never added to the Start menu
    // (portable apps, shortcuts created by installers). Only the two Desktop folders are read,
    // one level deep; anything that duplicates a Start menu entry is merged into it above.
    const std::wstring desktopDirs[] = {KnownFolderPath(FOLDERID_Desktop),
                                        KnownFolderPath(FOLDERID_PublicDesktop)};
    for (const std::wstring& dir : desktopDirs) {
        if (dir.empty()) continue;
        WIN32_FIND_DATAW fd = {};
        HANDLE h = FindFirstFileW((dir + L"\\*").c_str(), &fd);
        if (h == INVALID_HANDLE_VALUE) continue;
        do {
            if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) continue;
            const std::wstring leaf = fd.cFileName;
            const std::wstring foldLeaf = FoldString(leaf);
            const bool isLink = EndsWith(foldLeaf, L".lnk");
            if (!isLink && !EndsWith(foldLeaf, L".exe")) continue;
            if (leaf.empty() || leaf[0] == L'~') continue;  // stray 8.3 name

            const std::wstring path = dir + L"\\" + leaf;
            IShellItem* item = nullptr;
            if (FAILED(SHCreateItemFromParsingName(path.c_str(), nullptr, IID_PPV_ARGS(&item))) ||
                !item) {
                continue;
            }

            RawApp ra;
            ra.id = path;
            ra.launch = path;
            PWSTR dn = nullptr;
            item->GetDisplayName(SIGDN_NORMALDISPLAY, &dn);
            ra.name = (dn && *dn) ? dn : leaf;
            if (dn) CoTaskMemFree(dn);
            if (FoldString(ra.name) == foldLeaf) {
                ra.name = leaf.substr(0, leaf.find_last_of(L'.'));
            }

            if (isLink) {
                IShellItem2* it2 = nullptr;
                if (SUCCEEDED(item->QueryInterface(IID_PPV_ARGS(&it2))) && it2) {
                    ra.target = CanonicalAppPath(ItemString(it2, L"System.Link.TargetParsingPath"));
                    ra.args = ItemString(it2, L"System.Link.Arguments");
                    it2->Release();
                }
                if (ra.target.empty()) {
                    std::wstring t, a;
                    if (ReadShellLink(path, t, a)) {
                        ra.target = t;
                        if (ra.args.empty()) ra.args = a;
                    }
                }
            } else {
                ra.target = CanonicalAppPath(path);
            }

            bool keep = !ra.name.empty();
            if (keep && (ra.target.empty() || !IsProgramTarget(ra.target) ||
                         IsStoreAliasPath(ra.target))) {
                keep = false;
            }
            if (keep) {
                WIN32_FILE_ATTRIBUTE_DATA fad = {};
                if (!GetFileAttributesExW(ra.target.c_str(), GetFileExInfoStandard, &fad) ||
                    (fad.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                    keep = false;  // the program the shortcut points to is gone
                }
            }
            if (keep && job->hideUtility && ClassifyUtility(item, ra.name, path) != UtilNone) {
                keep = false;
            }

            if (keep) {
                addApp(std::move(ra), item, 1);
            } else {
                item->Release();
            }
        } while (FindNextFileW(h, &fd));
        FindClose(h);
    }

    // ---- Readable names ----
    // After the identity merge two different apps can still share a display name. Give each one
    // a suffix that tells them apart, so the list never shows two identical rows.
    {
        std::unordered_map<std::wstring, int> nameCount;
        for (const Tmp& t : tmp) ++nameCount[t.folded];
        std::unordered_set<std::wstring> used;
        for (Tmp& t : tmp) {
            std::wstring name = t.app.name;
            if (nameCount[t.folded] > 1) {
                name += L" (" + AppQualifier(t.app) + L")";
                if (used.count(FoldString(name))) {
                    const std::wstring file = FileNameOf(CanonicalAppPath(t.app.target));
                    if (!file.empty()) name = t.app.name + L" (" + file + L")";
                }
            }
            const std::wstring base = name;
            std::wstring folded = FoldString(name);
            int n = 2;
            while (used.count(folded)) {
                name = base + L" " + std::to_wstring(n++);
                folded = FoldString(name);
            }
            t.app.name = name;
            t.folded = folded;
            used.insert(folded);
        }
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
                std::wstring target = tmp[i].app.target;
                if (target.empty()) {
                    const std::wstring p = PathFromAppId(id);
                    // A .lnk has no icon of its own that we want; the shell item path below
                    // resolves the shortcut's icon correctly.
                    if (!EndsWith(FoldString(p), L".lnk")) target = p;
                }
                Pix f;
                if (GetClassicIconFallback(tmp[i].item, target, f)) {
                    take(f);
                } else if (a.ok) {
                    take(a);  // nothing better found: keep what the shell gave
                }
            }
        } else {
            Shot& s = shots[i];
            // Packaged apps: ICONONLY hands back the generic item icon (a blank page), while the
            // RESIZETOFIT render is the tile image the Start menu shows. So the order is reversed
            // here compared to desktop apps.
            if (s.b.ok && countB[s.b.hash] < 2) {
                take(s.b);
            } else if (s.a.ok && countA[s.a.hash] < 2) {
                take(s.a);
            } else {
                std::wstring logo = GetTileLogoPath(tmp[i].item);
                // The property is often relative to the package ("Assets\Logo.png"), which cannot be
                // opened from here: only an absolute path is worth trying.
                const bool absolute = logo.size() > 2 &&
                                      (logo[1] == L':' || (logo[0] == L'\\' && logo[1] == L'\\'));
                if (absolute && GetFileAttributesW(logo.c_str()) != INVALID_FILE_ATTRIBUTES) {
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
//    Store apps that do not touch the shortcut folders.
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

// ===== Shortcut folder watcher (change detection, Automatic mode) =====
// The two Start menu folders (all users and current user) and the Desktop ones are watched.
// Installers fire many events in a row and Windows needs a few seconds to register the shortcut,
// so the UI is only notified once the events stop arriving.

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
    const std::wstring dirs[] = {KnownFolderPath(FOLDERID_CommonPrograms),
                                 KnownFolderPath(FOLDERID_Programs),
                                 KnownFolderPath(FOLDERID_Desktop),
                                 KnownFolderPath(FOLDERID_PublicDesktop)};
    const DWORD filter =
        FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME | FILE_NOTIFY_CHANGE_LAST_WRITE;

    std::vector<HANDLE> handles;
    std::unordered_set<std::wstring> seenDirs;
    for (const auto& d : dirs) {
        if (d.empty()) continue;
        if (!seenDirs.insert(FoldString(d)).second) continue;  // a redirected Desktop can repeat
        HANDLE h = FindFirstChangeNotificationW(d.c_str(), TRUE, filter);
        if (h == INVALID_HANDLE_VALUE) {
            continue;
        }
        handles.push_back(h);
    }
    const DWORD stopIdx = (DWORD)handles.size();
    handles.push_back(g_watchStop);
    const DWORD n = (DWORD)handles.size();

    if (stopIdx == 0) {
        return 0;
    }

    for (;;) {
        DWORD w = WaitForMultipleObjects(n, handles.data(), FALSE, INFINITE);
        if (w >= WAIT_OBJECT_0 + stopIdx) break;  // stop, or error
        FindNextChangeNotification(handles[w - WAIT_OBJECT_0]);

        // Debounce: wait until it goes quiet.
        bool stop = false;
        for (;;) {
            DWORD w2 = WaitForMultipleObjects(n, handles.data(), FALSE, kWatchDebounceMs);
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
// Custom-entry form (defined in the "Custom entries" section, drawn from Render).
static void DrawCustomForm(float cx, float cy, float cw, float s);
static void DrawCustomFormFooter(float cx, float footerTop, float cw);
static void DrawManager(float cx, float cy, float cw, float s);
static void DrawManagerFooter(float cx, float footerTop, float cw);
static void OpenManager();  // defined with the manager, below
static bool ShiftDown();

// Two-step confirmation for destructive commands: the first Enter arms the command (and shows a
// warning), a second Enter within a few seconds runs it; anything else disarms it.
static void CancelConfirm() {
    if (g_winCloseArmed) {
        g_winCloseArmed = false;
        if (g_hwnd) KillTimer(g_hwnd, TIMER_CONFIRM);
        MarkDirty();
    }
    if (g_confirmCmd < 0) return;
    g_confirmCmd = -1;
    if (g_hwnd) KillTimer(g_hwnd, TIMER_CONFIRM);
    MarkDirty();
}

static void ArmConfirm(int cmdIndex) {
    g_confirmCmd = cmdIndex;
    if (g_hwnd) {
        KillTimer(g_hwnd, TIMER_CONFIRM);
        SetTimer(g_hwnd, TIMER_CONFIRM, 4000, nullptr);
    }
    MarkDirty();
}

static void ApplyIndex(IndexResult& r) {
    // Remember the selected app so a reindex while the bar is open does not reset the selection.
    std::wstring keepId;
    if (g_sel >= 0 && g_sel < (int)g_results.size() &&
        g_results[g_sel].kind == ResultKind::App) {
        keepId = g_apps[g_results[g_sel].app].id;
    }

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
        a.target = ra.target;
        a.args = ra.args;
        a.launch = ra.launch;
        a.folded = FoldString(a.name);
        a.usageKey = UsageKey(a.id);
        a.uses = Wh_GetIntValue(a.usageKey.c_str(), 0);
        a.lastUsed = Wh_GetIntValue((a.usageKey + L"t").c_str(), 0);
        a.pinned = Wh_GetIntValue((a.usageKey + L"p").c_str(), 0) != 0;
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
    g_windowsDirty = true;   // the process -> icon mapping may have changed

    RunSearch();
    g_sel = 0;
    for (size_t i = 0; i < g_results.size() && !keepId.empty(); ++i) {
        if (g_results[i].kind == ResultKind::App && g_apps[g_results[i].app].id == keepId) {
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
    IDWriteTextFormat** fmts[] = {&g_fmtInput, &g_fmtTitle,      &g_fmtHint,      &g_fmtKey,
                                  &g_fmtKeyBold, &g_fmtEmpty,    &g_fmtBadge,     &g_fmtSection,
                                  &g_fmtPanelTitle, &g_fmtPanelSub, &g_fmtPanelRow, &g_fmtMeta,
                                  &g_fmtExt, &g_fmtPath, &g_fmtForm, &g_fmtFormLabel,
                                  &g_fmtPreviewCode};
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
    g_fmtKeyBold = MakeFormat(fam, Sc(13.f), semi, DWRITE_TEXT_ALIGNMENT_CENTER);
    g_fmtEmpty = MakeFormat(fam, Sc(14.f), normal, DWRITE_TEXT_ALIGNMENT_CENTER);
    g_fmtBadge = MakeFormat(fam, Sc(19.f), semi, DWRITE_TEXT_ALIGNMENT_CENTER);
    g_fmtSection = MakeFormat(fam, Sc(11.5f), semi);
    g_fmtMeta = MakeFormat(fam, Sc(12.f), normal);
    g_fmtExt = MakeFormat(fam, Sc(10.5f), semi, DWRITE_TEXT_ALIGNMENT_CENTER);
    g_fmtPath = MakeFormat(fam, Sc(12.5f), semi);
    g_fmtPanelTitle = MakeFormat(fam, Sc(16.f), semi);
    g_fmtPanelSub = MakeFormat(fam, Sc(12.f), normal);
    g_fmtPanelRow = MakeFormat(fam, Sc(13.f), normal);
    g_fmtForm = MakeFormat(fam, Sc(17.f), normal);
    g_fmtFormLabel = MakeFormat(fam, Sc(13.f), semi);
    // Quick Look text: fixed pitch so columns map 1:1 to the horizontal scroll, and deliberately
    // without trimming: long lines are scrolled sideways instead of ending in an ellipsis.
    g_fmtPreviewCode = MakeFormat(L"Consolas", Sc(12.f), normal);

    // Long app names and paths are trimmed with an ellipsis instead of overflowing.
    const DWRITE_TRIMMING tr = {DWRITE_TRIMMING_GRANULARITY_CHARACTER, 0, 0};
    if (g_fmtTitle) {
        g_dw->CreateEllipsisTrimmingSign(g_fmtTitle, &g_ellipsis);
        if (g_ellipsis) g_fmtTitle->SetTrimming(&tr, g_ellipsis);
    }
    if (g_fmtPanelRow) {
        g_dw->CreateEllipsisTrimmingSign(g_fmtPanelRow, &g_ellipsisSm);
        if (g_ellipsisSm) {
            for (IDWriteTextFormat* f :
                 {g_fmtPanelRow, g_fmtPanelTitle, g_fmtPanelSub, g_fmtMeta, g_fmtFormLabel}) {
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
    LV_ACCENT_DISABLED = 0,
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
static bool g_liveAccentOn = false;                 // accent policy currently applied
static LvSetWindowCompositionAttribute_t g_pSetWCA = nullptr;
static HWND g_livePanel = nullptr;                  // acrylic of the details panel
static bool g_liveWin11 = false;                    // Windows 11: corners rounded by DWM
typedef HRESULT(WINAPI* LvDwmSetWindowAttribute_t)(HWND, DWORD, LPCVOID, DWORD);
static LvDwmSetWindowAttribute_t g_pDwmSetAttr = nullptr;
static int g_liveRgnSize[2][2] = {};                // Windows 10: size of the last region
static int g_livePos[2][4] = {};                    // last x, y, w, h given to each window

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
    g_liveAccentOn = true;
}

// Turns the acrylic off before hiding: DWM occasionally keeps the last blur frame on screen
// when a window with an accent policy is hidden (the effect rectangle that survived a close).
static void LiveDisableAccent() {
    if (!g_pSetWCA || !g_liveAccentOn) return;
    LvAccentPolicy p = {};
    p.AccentState = LV_ACCENT_DISABLED;
    LvWinCompAttrData d = {19 /*WCA_ACCENT_POLICY*/, &p, sizeof(p)};
    if (g_liveWnd) g_pSetWCA(g_liveWnd, &d);
    if (g_livePanel) g_pSetWCA(g_livePanel, &d);
    g_liveAccentOn = false;
}

static void LiveHide() {
    LiveDisableAccent();
    if (g_liveWnd) ShowWindow(g_liveWnd, SW_HIDE);
    if (g_livePanel) ShowWindow(g_livePanel, SW_HIDE);
    memset(g_liveRgnSize, 0, sizeof(g_liveRgnSize));
    memset(g_livePos, 0, sizeof(g_livePos));
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
    ReleaseThumbCache();  // preview bitmaps belong to the render target
    ReleaseWindowIcons();
    for (auto& a : g_apps) {
        SafeRelease(a.bmp);
    }
    SafeRelease(g_brush);
    SafeRelease(g_matchBrush);
    SafeRelease(g_gradBrush);
    SafeRelease(g_thumbBrush);
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
    return kExpandedMinH + (g_fileMode ? kPathH : 0.f);
}

// Height (DIP) of the details panel (actions or Quick Look); always matches the search bar, which
// is taller in file mode (path band), so the panel and the card end on the same line.
static double DetailsPanelH() {
    return ExpandedH();
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
    const double maxH = kExpandedMinH + kPathH;
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
    const double rowsArea = ExpandedH() - kBarH - FileChipsH() - kFooterH - 2 * kListPad - HeroH();
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
    return ExpandedH() - kBarH - FileChipsH() - kFooterH - 2 * kListPad - HeroH();
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
    // The custom-entry form replaces the list entirely, so it always uses the full height and
    // never shows the details panel.
    if (g_form.active) {
        g_targetH = ExpandedH();
        g_detailTarget = 0;
        if (snap) {
            g_curH = g_targetH;
            g_detailPos = 0;
        }
        return;
    }
    // There are only two heights: bar alone, or expanded (fixed). While typing it is always
    // expanded, even without results ("No results" fills the list area). Web modes are the
    // exception: they have a single, deterministic action, so the bar stays compact.
    double h = kBarH;
    if (g_helpOpen) {
        h = ExpandedH();
    } else if (ModeIsWeb()) {
        h = kBarH;
    } else if (g_fileMode || rows > 0 || !g_tokens.empty() || GlobalModeActive()) {
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

    // Details / preview panel: open if the mode matches the selected result kind.
    if (n > 0 && g_sel >= 0 && g_sel < n && g_results[g_sel].kind == ResultKind::App) {
        g_panelAppId = g_apps[g_results[g_sel].app].id;
    }
    {
        bool panelWanted = false;
        if (g_detailsOpen && n > 0 && g_sel >= 0 && g_sel < n && !g_panelSwitching) {
            const ResultKind rk = g_results[g_sel].kind;
            if (g_panelMode == PanelMode::Actions &&
                (rk == ResultKind::App || rk == ResultKind::Window))
                panelWanted = true;
            else if (g_panelMode == PanelMode::Preview && rk == ResultKind::File)
                panelWanted = true;
        }
        g_detailTarget = panelWanted ? 1.0 : 0.0;
    }

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
    CancelConfirm();  // moving the selection disarms a pending destructive command
    g_sel = std::clamp(idx, 0, n - 1);
    // When the preview panel is open, reset the preview if the file changed.
    if (g_panelMode == PanelMode::Preview && g_detailsOpen) {
        const auto& res = g_results[g_sel];
        if (res.kind != ResultKind::File || res.payload != g_preview.path) {
            g_preview = PreviewState{};
        }
    }
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

static void ScheduleFileSearch();
static void CancelFileSearch();

static void OnTextChanged() {
    CancelConfirm();  // typing disarms a pending destructive command
    g_form = CustomFormState();  // the form belongs to the text that opened it
    // The calculator is debounced: clear its state now so typing feels instant (app searches
    // update immediately) and only re-evaluate shortly after the user stops typing. That way
    // typing "pith" never flashes the value of "pi" on the way.
    g_calc = Calc::Result();
    g_calcDisplay.clear();
    g_calcActive = false;
    g_calcHistSel = -1;
    g_histScroll = 0;
    // File mode never shows the calculator, so it must not wake up later and re-run the search
    // (which would reset the selection while browsing a folder).
    g_calcPending = !g_fileMode;
    if (g_hwnd) {
        KillTimer(g_hwnd, TIMER_CALC);
        if (g_calcPending) SetTimer(g_hwnd, TIMER_CALC, 200, nullptr);
    }
    RunSearch();
    ScheduleFileSearch();
    ScheduleGlobalSearch();
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

// ===== Icons for "win:" rows =====
// The app index does not know about portables or packaged processes, so a window falls back to the
// icon of its own executable. Bitmaps are recreated from the pixels when the target is lost.
struct WinIcon {
    Pix pix;
    ID2D1Bitmap* bmp = nullptr;
};
static std::unordered_map<std::wstring, WinIcon> g_winIcons;

static ID2D1Bitmap* EnsureWindowIcon(const std::wstring& path) {
    if (path.empty()) return nullptr;
    WinIcon& e = g_winIcons[path];
    if (e.bmp) return e.bmp;
    if (e.pix.px.empty()) {
        if (!GetFileIconPix(path, e.pix)) {
            e.pix = Pix();
            GetSystemImageListIcon(path.c_str(), false, e.pix);  // handles packaged paths
        }
        if (e.pix.px.empty() || e.pix.w == 0 || e.pix.h == 0) return nullptr;
    }
    D2D1_BITMAP_PROPERTIES bp = D2D1::BitmapProperties(
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED), 96.f, 96.f);
    g_rt->CreateBitmap(D2D1::SizeU(e.pix.w, e.pix.h), e.pix.px.data(), e.pix.w * 4, &bp, &e.bmp);
    return e.bmp;
}

static void ReleaseWindowIcons() {
    for (auto& kv : g_winIcons) SafeRelease(kv.second.bmp);
}

// Short label shown on the right of a row, by result kind.
static const wchar_t* ResultTypeLabel(ResultKind k) {
    switch (k) {
        case ResultKind::Web: return kTxtKindWeb;
        case ResultKind::Url: return kTxtKindUrl;
        case ResultKind::Command: return kTxtKindCommand;
        case ResultKind::File: return kTxtKindFile;
        case ResultKind::Setting: return kTxtKindSetting;
        case ResultKind::Custom: return kTxtKindCustom;
        case ResultKind::Bookmark: return kTxtKindBookmark;
        case ResultKind::Clip: return kTxtKindClip;
        case ResultKind::Recent: return kTxtKindRecent;
        case ResultKind::GlobalFile: return kTxtKindFile;
        case ResultKind::Window: return kTxtKindWindow;
        default: return kTxtTypeApp;
    }
}

// Recent files and global "find:" results share the same row and actions: both carry a real path.
static bool IsPathResult(ResultKind k) {
    return k == ResultKind::Recent || k == ResultKind::GlobalFile;
}

// Title layout for any result text, with the matching letters set in the accent colour and
// semibold (used by the app rows and by results from other providers).
static IDWriteTextLayout* BuildMatchLayout(const std::wstring& name, const std::wstring& folded,
                                           float widthPx, float heightPx) {
    IDWriteTextLayout* l = nullptr;
    if (FAILED(g_dw->CreateTextLayout(name.c_str(), (UINT32)name.size(), g_fmtTitle, widthPx,
                                      heightPx, &l)) ||
        !l) {
        return nullptr;
    }
    std::vector<char> mask(folded.size(), 0);
    for (const auto& t : g_tokens) {
        ScoreToken(name, folded, t, &mask);
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

static IDWriteTextLayout* BuildTitleLayout(const AppEntry& a, float widthPx, float heightPx) {
    return BuildMatchLayout(a.name, a.folded, widthPx, heightPx);
}

static void DrawMagnifier(float cx, float cy) {
    SetBrush(g_pal.textDim);
    D2D1_ELLIPSE e = D2D1::Ellipse(D2D1::Point2F(cx - Sc(2.f), cy - Sc(2.f)), Sc(7.5f), Sc(7.5f));
    g_rt->DrawEllipse(&e, g_brush, Sc(2.f), g_roundStroke);
    g_rt->DrawLine(D2D1::Point2F(cx + Sc(3.4f), cy + Sc(3.4f)),
                   D2D1::Point2F(cx + Sc(9.f), cy + Sc(9.f)), g_brush, Sc(2.f), g_roundStroke);
}

// Fills a closed polygon (device pixels) with the current brush.
static void FillPolygon(const D2D1_POINT_2F* pts, int n) {
    if (!g_d2d || !g_rt || n < 3) return;
    ID2D1PathGeometry* geo = nullptr;
    ID2D1GeometrySink* sink = nullptr;
    if (FAILED(g_d2d->CreatePathGeometry(&geo)) || !geo) return;
    if (FAILED(geo->Open(&sink)) || !sink) {
        geo->Release();
        return;
    }
    sink->BeginFigure(pts[0], D2D1_FIGURE_BEGIN_FILLED);
    for (int i = 1; i < n; ++i) {
        sink->AddLine(pts[i]);
    }
    sink->EndFigure(D2D1_FIGURE_END_CLOSED);
    sink->Close();
    g_rt->FillGeometry(geo, g_brush);
    sink->Release();
    geo->Release();
}

// White line-art glyph for a command row, centred in a size x size tile placed at (x, y). The
// design grid is 24x24; `tile` is the badge colour and is used to punch the cut-outs.
static void DrawCommandGlyph(CmdGlyph g, float x, float y, float size, const D2D1_COLOR_F& tile,
                             const D2D1_COLOR_F& ink = D2D1::ColorF(1.f, 1.f, 1.f)) {
    if (!g_rt) return;
    const float u = size / 24.f;
    const float sw = 1.9f * u;
    const D2D1_COLOR_F white = ink;
    auto pt = [&](float gx, float gy) { return D2D1::Point2F(x + gx * u, y + gy * u); };
    auto ln = [&](float a, float b, float c, float d) {
        g_rt->DrawLine(pt(a, b), pt(c, d), g_brush, sw, g_roundStroke);
    };
    auto el = [&](float gx, float gy, float rx, float ry) {
        D2D1_ELLIPSE e = D2D1::Ellipse(pt(gx, gy), rx * u, ry * u);
        g_rt->DrawEllipse(&e, g_brush, sw, g_roundStroke);
    };
    auto fe = [&](float gx, float gy, float r) {
        D2D1_ELLIPSE e = D2D1::Ellipse(pt(gx, gy), r * u, r * u);
        g_rt->FillEllipse(&e, g_brush);
    };
    auto rr = [&](float a, float b, float c, float d, float r) {
        D2D1_ROUNDED_RECT R =
            D2D1::RoundedRect(D2D1::RectF(x + a * u, y + b * u, x + c * u, y + d * u), r * u, r * u);
        g_rt->DrawRoundedRectangle(&R, g_brush, sw, g_roundStroke);
    };
    auto fr = [&](float a, float b, float c, float d, float r) {
        FillRR(x + a * u, y + b * u, x + c * u, y + d * u, r * u, white);
    };

    SetBrush(white);
    switch (g) {
        case CmdGlyph::Lock:
            el(12, 11, 4, 4);
            fr(6.5f, 10.5f, 17.5f, 19.5f, 2.f);
            SetBrush(tile);
            fe(12, 15, 1.7f);
            break;
        case CmdGlyph::Moon:
            fe(11, 12, 7);
            SetBrush(tile);
            fe(14.5f, 9, 6.4f);
            break;
        case CmdGlyph::Bars:
            fr(6.f, 13.f, 9.5f, 18.f, 1.f);
            fr(10.5f, 8.5f, 14.f, 18.f, 1.f);
            fr(15.f, 11.f, 18.5f, 18.f, 1.f);
            break;
        case CmdGlyph::Sliders:
            ln(6, 9, 18, 9);
            fe(14.5f, 9, 2.4f);
            ln(6, 15, 18, 15);
            fe(9.5f, 15, 2.4f);
            break;
        case CmdGlyph::Chip:
            rr(8, 8, 16, 16, 2);
            ln(10, 6, 10, 8.2f);
            ln(12, 6, 12, 8.2f);
            ln(14, 6, 14, 8.2f);
            ln(10, 15.8f, 10, 18);
            ln(12, 15.8f, 12, 18);
            ln(14, 15.8f, 14, 18);
            ln(6, 10, 8.2f, 10);
            ln(6, 12, 8.2f, 12);
            ln(6, 14, 8.2f, 14);
            ln(15.8f, 10, 18, 10);
            ln(15.8f, 12, 18, 12);
            ln(15.8f, 14, 18, 14);
            rr(10.5f, 10.5f, 13.5f, 13.5f, 0.5f);
            break;
        case CmdGlyph::List:
            rr(7, 5.5f, 17, 9.5f, 1.4f);
            fe(9.6f, 7.5f, 0.9f);
            ln(12.2f, 7.5f, 15, 7.5f);
            rr(7, 10, 17, 14, 1.4f);
            fe(9.6f, 12, 0.9f);
            ln(12.2f, 12, 15, 12);
            rr(7, 14.5f, 17, 18.5f, 1.4f);
            fe(9.6f, 16.5f, 0.9f);
            ln(12.2f, 16.5f, 15, 16.5f);
            break;
        case CmdGlyph::Key:
            el(9, 12, 3.3f, 3.3f);
            ln(12.3f, 12, 18, 12);
            ln(15.4f, 12, 15.4f, 14.6f);
            ln(17.6f, 12, 17.6f, 14.2f);
            break;
        case CmdGlyph::Globe:
            el(12, 12, 7, 7);
            el(12, 12, 2.8f, 7);
            ln(5, 12, 19, 12);
            ln(6.3f, 8.5f, 17.7f, 8.5f);
            ln(6.3f, 15.5f, 17.7f, 15.5f);
            break;
        case CmdGlyph::Update: {
            ln(12, 5.5f, 12, 14.5f);
            D2D1_POINT_2F tri[3] = {pt(8.6f, 11.f), pt(12, 15.f), pt(15.4f, 11.f)};
            FillPolygon(tri, 3);
            ln(6.5f, 15.5f, 6.5f, 18);
            ln(6.5f, 18, 17.5f, 18);
            ln(17.5f, 18, 17.5f, 15.5f);
            break;
        }
        case CmdGlyph::Grid:
            fr(6, 6, 11, 11, 1.6f);
            fr(13, 6, 18, 11, 1.6f);
            fr(6, 13, 11, 18, 1.6f);
            fr(13, 13, 18, 18, 1.6f);
            break;
        case CmdGlyph::Monitor:
            rr(5, 6, 19, 15, 1.6f);
            ln(12, 15, 12, 18);
            ln(8.5f, 18, 15.5f, 18);
            break;
        case CmdGlyph::Speaker: {
            fr(6.5f, 9.5f, 10.f, 14.5f, 0.8f);
            D2D1_POINT_2F spk[4] = {pt(10.f, 8.5f), pt(15, 5.5f), pt(15, 18.5f), pt(10.f, 15.5f)};
            FillPolygon(spk, 4);
            break;
        }
        case CmdGlyph::Bluetooth: {
            const float bw = 1.6f * u;
            auto bl = [&](float a, float b, float c, float d) {
                g_rt->DrawLine(pt(a, b), pt(c, d), g_brush, bw, g_roundStroke);
            };
            bl(12, 4.5f, 12, 19.5f);
            bl(12, 4.5f, 16.5f, 8.f);
            bl(16.5f, 8.f, 7.5f, 15.5f);
            bl(7.5f, 8.5f, 16.5f, 16.f);
            bl(16.5f, 16.f, 12, 19.5f);
            break;
        }
        case CmdGlyph::Printer:
            rr(8, 5.5f, 16, 9, 0.8f);
            fr(4.5f, 9, 19.5f, 15.5f, 1.5f);
            SetBrush(tile);
            fe(7, 11.3f, 0.9f);
            SetBrush(white);
            rr(8, 15, 16, 19, 0.8f);
            break;
        case CmdGlyph::Terminal:
            ln(7, 8, 10.5f, 11.5f);
            ln(10.5f, 11.5f, 7, 15);
            ln(12.5f, 15, 17.5f, 15);
            break;
        case CmdGlyph::Folder: {
            D2D1_POINT_2F fol[6] = {pt(4, 8.5f), pt(9, 8.5f),  pt(10.6f, 5.5f),
                                    pt(20, 5.5f), pt(20, 19.f), pt(4, 19.f)};
            FillPolygon(fol, 6);
            break;
        }
        case CmdGlyph::Gear:
            for (int k = 0; k < 8; ++k) {
                const double a = k * kPi / 4.0;
                const float ca = (float)std::cos(a), sa = (float)std::sin(a);
                g_rt->DrawLine(
                    D2D1::Point2F(x + (12 + ca * 4.6f) * u, y + (12 + sa * 4.6f) * u),
                    D2D1::Point2F(x + (12 + ca * 7.4f) * u, y + (12 + sa * 7.4f) * u), g_brush,
                    2.3f * u, g_roundStroke);
            }
            el(12, 12, 4.6f, 4.6f);
            SetBrush(tile);
            fe(12, 12, 1.8f);
            break;
        case CmdGlyph::Hibernate:
            fe(9.5f, 13.5f, 6);
            SetBrush(tile);
            fe(12.5f, 10.5f, 5.4f);
            SetBrush(white);
            ln(14.5f, 5.5f, 19, 5.5f);
            ln(19, 5.5f, 14.5f, 9.5f);
            ln(14.5f, 9.5f, 19, 9.5f);
            break;
        case CmdGlyph::Power:
            el(12, 13, 5.8f, 5.8f);
            FillRR(x + 10.2f * u, y + 4.2f * u, x + 13.8f * u, y + 9.f * u, 0.f, tile);
            SetBrush(white);
            ln(12, 4.6f, 12, 11.4f);
            break;
        case CmdGlyph::Restart:
            el(12, 12, 6.2f, 6.2f);
            FillRR(x + 10.5f * u, y + 2.6f * u, x + 19.f * u, y + 8.6f * u, 0.f, tile);
            SetBrush(white);
            {
                D2D1_POINT_2F ah[3] = {pt(13.6f, 3.4f), pt(18.8f, 6.4f), pt(13.2f, 9.2f)};
                FillPolygon(ah, 3);
            }
            break;
        case CmdGlyph::SignOut:
            rr(6.5f, 5.f, 12.5f, 19.f, 1.4f);
            ln(11, 12, 18.5f, 12);
            {
                D2D1_POINT_2F ah[3] = {pt(15.6f, 8.6f), pt(19.6f, 12.f), pt(15.6f, 15.4f)};
                FillPolygon(ah, 3);
            }
            break;
        case CmdGlyph::Recycle: {
            ln(6.5f, 7.f, 17.5f, 7.f);
            ln(10.f, 4.6f, 14.f, 4.6f);
            ln(10.f, 4.6f, 10.f, 7.f);
            ln(14.f, 4.6f, 14.f, 7.f);
            D2D1_POINT_2F bin[4] = {pt(7.6f, 8.4f), pt(16.4f, 8.4f), pt(15.4f, 19.f), pt(8.6f, 19.f)};
            FillPolygon(bin, 4);
            break;
        }
        case CmdGlyph::Wifi:
            fe(12, 16.6f, 1.7f);
            ln(8.6f, 13.2f, 12, 10.2f);
            ln(12, 10.2f, 15.4f, 13.2f);
            ln(6.f, 12.8f, 12, 7.2f);
            ln(12, 7.2f, 18.f, 12.8f);
            break;
        case CmdGlyph::Clock:
            el(12, 12, 7.6f, 7.6f);
            ln(12, 12, 12, 7.4f);
            ln(12, 12, 15.6f, 13.6f);
            break;
        case CmdGlyph::Bell: {
            D2D1_POINT_2F body[5] = {pt(8.6f, 15.6f), pt(8.6f, 11.f), pt(12, 7.4f),
                                     pt(15.4f, 11.f), pt(15.4f, 15.6f)};
            FillPolygon(body, 5);
            fr(7.f, 15.f, 17.f, 16.6f, 0.6f);
            fe(12, 18.f, 1.3f);
            fe(12, 6.6f, 1.f);
            break;
        }
        case CmdGlyph::Camera:
            fr(3.5f, 7.5f, 20.5f, 17.5f, 2.5f);
            fr(8.5f, 5.f, 15.5f, 7.6f, 0.8f);
            SetBrush(tile);
            fe(12, 12.5f, 3.2f);
            break;
        case CmdGlyph::Mic:
            fr(9.8f, 4.2f, 14.2f, 12.4f, 2.2f);
            ln(12, 12.4f, 12, 16.4f);
            ln(8.2f, 16.4f, 15.8f, 16.4f);
            break;
        case CmdGlyph::Doc:
            rr(6, 3.5f, 18, 20.5f, 2);
            ln(9, 9, 15, 9);
            ln(9, 12.5f, 15, 12.5f);
            ln(9, 16, 13, 16);
            break;
        case CmdGlyph::Home:
            ln(4.5f, 11.5f, 12, 4.8f);
            ln(12, 4.8f, 19.5f, 11.5f);
            rr(7, 11, 17, 19.5f, 1.2f);
            ln(10.5f, 19.5f, 10.5f, 14.5f);
            ln(13.5f, 19.5f, 13.5f, 14.5f);
            ln(10.5f, 14.5f, 13.5f, 14.5f);
            break;
        case CmdGlyph::Download:
            ln(12, 4.5f, 12, 15.f);
            ln(8, 11, 12, 15.f);
            ln(16, 11, 12, 15.f);
            ln(5.5f, 19, 18.5f, 19);
            break;
        case CmdGlyph::Image:
            rr(4, 5.5f, 20, 18.5f, 1.8f);
            el(8.8f, 10, 1.7f, 1.7f);
            ln(5, 17, 10.2f, 11.8f);
            ln(10.2f, 11.8f, 14.f, 15.6f);
            ln(14.f, 15.6f, 16.6f, 13.f);
            ln(16.6f, 13.f, 20, 16.4f);
            break;
        case CmdGlyph::Music:
            ln(9.5f, 17.5f, 9.5f, 6.f);
            ln(9.5f, 6.f, 18.5f, 4.4f);
            ln(18.5f, 4.4f, 18.5f, 15.f);
            fe(6.8f, 17.5f, 2.7f);
            fe(15.8f, 15.f, 2.7f);
            break;
        case CmdGlyph::Video:
            rr(4, 6.f, 20, 18.f, 1.8f);
            fe(6.6f, 8.6f, 1.1f);
            fe(6.6f, 12.f, 1.1f);
            fe(6.6f, 15.4f, 1.1f);
            fe(17.4f, 8.6f, 1.1f);
            fe(17.4f, 12.f, 1.1f);
            fe(17.4f, 15.4f, 1.1f);
            break;
        case CmdGlyph::Drive:
            // A disk drive: a rounded body, a seam and a small activity light.
            rr(4.5f, 7.5f, 19.5f, 16.5f, 2.f);
            ln(6.5f, 12.f, 17.5f, 12.f);
            fe(15.5f, 14.4f, 0.9f);
            break;
        case CmdGlyph::Close:
            ln(8.f, 8.f, 16.f, 16.f);
            ln(16.f, 8.f, 8.f, 16.f);
            break;
        case CmdGlyph::ExternalLink: {
            // Arrow ↗ with baseline — clean "open externally" symbol used by
            // Raycast/Spotlight. Just the arrow head + shaft, no box.
            // Diagonal shaft bottom-left → top-right
            ln(7.f, 17.f, 17.f, 7.f);
            // Arrow head tip at (17, 7): right arm and top arm
            ln(17.f, 7.f, 17.f, 12.f);
            ln(17.f, 7.f, 12.f, 7.f);
            // Short baseline (ground indicator below the arrow)
            ln(6.f, 18.5f, 13.f, 18.5f);
            break;
        }
        case CmdGlyph::ChevronRight:
            ln(9.f, 6.f, 15.f, 12.f);
            ln(15.f, 12.f, 9.f, 18.f);
            break;
        case CmdGlyph::ChevronLeft:
            ln(15.f, 6.f, 9.f, 12.f);
            ln(9.f, 12.f, 15.f, 18.f);
            break;
        case CmdGlyph::Link: {
            // Chain-link icon: two pill-shaped ovals on a 45° axis.
            // Drawn as two thin ellipses offset diagonally, plus a gap connector.
            // Top-right pill centred at (14.5, 9.5), horizontal oval tilted via offset
            el(14.5f, 9.5f,  4.2f, 2.2f);   // right oval
            el(9.5f,  14.5f, 4.2f, 2.2f);   // left oval
            // Diagonal connector (the shared overlapping bar between the two links)
            ln(10.8f, 13.2f, 13.2f, 10.8f);
            break;
        }
        case CmdGlyph::Star: {
            // Five-point star: a bookmark row's badge.
            D2D1_POINT_2F prev = pt(12.f, 3.8f);
            for (int k = 1; k <= 10; ++k) {
                const float a = -kPi / 2 + (float)k * kPi / 5;
                const float r = (k % 2 == 0) ? 8.2f : 3.7f;
                const D2D1_POINT_2F p =
                    pt(12.f + r * (float)std::cos(a), 12.f + r * (float)std::sin(a));
                g_rt->DrawLine(prev, p, g_brush, sw, g_roundStroke);
                prev = p;
            }
            break;
        }
        case CmdGlyph::Clipboard:
            // A clipboard: outer board, top clip and three text lines.
            rr(6.5f, 5.5f, 17.5f, 19.5f, 2.f);
            rr(9.5f, 3.5f, 14.5f, 7.5f, 1.2f);
            ln(9.5f, 10.5f, 14.5f, 10.5f);
            ln(9.5f, 13.5f, 14.5f, 13.5f);
            ln(9.5f, 16.5f, 13.f, 16.5f);
            break;
        case CmdGlyph::Pin:
            // Pushpin: head, collar and needle.
            rr(9.f, 4.5f, 15.f, 10.5f, 2.f);
            ln(8.f, 11.f, 16.f, 11.f);
            ln(12.f, 11.f, 12.f, 19.f);
            break;
    }
}

// ===== File icons (outline folder, type tiles and image previews) =====

// Line-art folder outline (used instead of the filled badge for folders).
static void DrawFolderOutline(float x, float y, float size, const D2D1_COLOR_F& c) {
    if (!g_rt) return;
    const float u = size / 24.f;
    const float sw = 1.9f * u;
    auto pt = [&](float gx, float gy) { return D2D1::Point2F(x + gx * u, y + gy * u); };
    auto ln = [&](float a, float b, float cc, float d) {
        g_rt->DrawLine(pt(a, b), pt(cc, d), g_brush, sw, g_roundStroke);
    };
    SetBrush(c);
    ln(4.f, 9.f, 4.f, 19.f);
    ln(4.f, 19.f, 20.f, 19.f);
    ln(20.f, 19.f, 20.f, 6.5f);
    ln(20.f, 6.5f, 11.f, 6.5f);
    ln(11.f, 6.5f, 9.5f, 9.f);
    ln(9.5f, 9.f, 4.f, 9.f);
}

static float TextWidth(const wchar_t* txt, IDWriteTextFormat* fmt);  // defined further down

// Rounded tile with the extension text (falls back to a page glyph when there is no extension).
static void DrawFileTile(float x, float y, float size, const std::wstring& ext,
                         const D2D1_COLOR_F& col, float alpha = 1.f) {
    if (!g_rt) return;
    FillRR(x, y, x + size, y + size, size * 0.22f, col, alpha);
    if (!ext.empty() && g_fmtExt) {
        SetBrush(D2D1::ColorF(1.f, 1.f, 1.f), alpha);
        D2D1_RECT_F tr = D2D1::RectF(x, y, x + size, y + size);
        // Long extensions (e.g. "JSON", "HTML") on a small tile are shrunk around the tile
        // centre so the letters never stick out of it.
        const float tw = TextWidth(ext.c_str(), g_fmtExt);
        const float avail = size * 0.86f;
        if (tw > avail && tw > 0.f) {
            const float f = avail / tw;
            const float cx0 = x + size / 2, cy0 = y + size / 2;
            g_rt->SetTransform(D2D1::Matrix3x2F::Scale(f, f, D2D1::Point2F(cx0, cy0)));
            g_rt->DrawText(ext.c_str(), (UINT32)ext.size(), g_fmtExt, &tr, g_brush);
            g_rt->SetTransform(D2D1::Matrix3x2F::Identity());
        } else {
            g_rt->DrawText(ext.c_str(), (UINT32)ext.size(), g_fmtExt, &tr, g_brush);
        }
    } else {
        D2D1_COLOR_F gc = col;
        gc.a *= alpha;
        DrawCommandGlyph(CmdGlyph::Doc, x, y, size, gc);
    }
}

// Draws a cached preview bitmap "cover"-fitted into a rounded box.
static void DrawThumbImage(ID2D1Bitmap* bmp, float x, float y, float size, float radius) {
    if (!bmp || !g_rt) return;
    const D2D1_SIZE_F sz = bmp->GetSize();
    if (sz.width <= 0.f || sz.height <= 0.f) return;
    const float sc = std::max(size / sz.width, size / sz.height);
    const float ox = x + (size - sz.width * sc) / 2.f;
    const float oy = y + (size - sz.height * sc) / 2.f;
    if (!g_thumbBrush) {
        D2D1_BITMAP_BRUSH_PROPERTIES bp =
            D2D1::BitmapBrushProperties(D2D1_EXTEND_MODE_CLAMP, D2D1_EXTEND_MODE_CLAMP,
                                        D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
        D2D1_BRUSH_PROPERTIES bpp =
            D2D1::BrushProperties(1.f, D2D1::Matrix3x2F::Identity());
        if (FAILED(g_rt->CreateBitmapBrush(bmp, bp, bpp, &g_thumbBrush))) {
            g_thumbBrush = nullptr;
            return;
        }
    }
    // One brush for every preview: only the bitmap and the transform change per row.
    g_thumbBrush->SetBitmap(bmp);
    g_thumbBrush->SetTransform(D2D1::Matrix3x2F::Scale(sc, sc) *
                               D2D1::Matrix3x2F::Translation(ox, oy));
    D2D1_ROUNDED_RECT r =
        D2D1::RoundedRect(D2D1::RectF(x, y, x + size, y + size), radius, radius);
    g_rt->FillRoundedRectangle(&r, g_thumbBrush);
    SetBrush(D2D1::ColorF(1.f, 1.f, 1.f), 0.16f);
    D2D1_ROUNDED_RECT bd = D2D1::RoundedRect(D2D1::RectF(x, y, x + size, y + size), radius, radius);
    g_rt->DrawRoundedRectangle(&bd, g_brush, 1.f);
}

// Drive-type lookups can be slow for disconnected mapped drives, so they are cached per letter.
static bool IsRemoteDrive(wchar_t letter) {
    const int i = towupper(letter) - L'A';
    if (i < 0 || i >= 26) return false;
    static bool known[26] = {};
    static bool remote[26] = {};
    if (!known[i]) {
        wchar_t root[4] = {letter, L':', L'\\', 0};
        remote[i] = GetDriveTypeW(root) == DRIVE_REMOTE;
        known[i] = true;
    }
    return remote[i];
}

// True when the pixels of a file live elsewhere (network share, cloud placeholder). Decoding one
// of these blocks the UI thread for as long as the transfer takes, so previews are skipped.
static bool FileThumbRemote(const FileEntry& e) {
    if ((e.attr & (FILE_ATTRIBUTE_OFFLINE | FILE_ATTRIBUTE_RECALL_ON_DATA_ACCESS)) != 0) {
        return true;
    }
    if (e.path.size() >= 2 && e.path[1] == L':') return IsRemoteDrive(e.path[0]);
    return e.path.size() >= 2 && e.path[0] == L'\\' && e.path[1] == L'\\';
}

struct ThumbResult {
    std::wstring path;
    std::vector<uint32_t> px;
    int w = 0;
    int h = 0;
    bool ok = false;
};

// Decodes previews off the UI thread. The bitmap itself is still created on the UI thread (it
// belongs to the Direct2D render target); only the slow WIC/file work happens here.
static DWORD WINAPI ThumbThreadProc(LPVOID param) {
    const HWND hwnd = (HWND)param;
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_BELOW_NORMAL);
    IWICImagingFactory* wic = nullptr;
    CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&wic));
    HANDLE waits[2] = {g_thumbEvent, g_thumbStop};
    for (;;) {
        if (WaitForMultipleObjects(2, waits, FALSE, INFINITE) != WAIT_OBJECT_0) break;
        for (;;) {
            if (WaitForSingleObject(g_thumbStop, 0) == WAIT_OBJECT_0) break;
            std::wstring path;
            EnterCriticalSection(&g_thumbLock);
            if (!g_thumbQueue.empty()) {
                path = std::move(g_thumbQueue.front());
                g_thumbQueue.pop_front();
            }
            LeaveCriticalSection(&g_thumbLock);
            if (path.empty()) break;
            auto* tr = new ThumbResult();
            tr->path = std::move(path);
            tr->ok = IsClipImagePath(tr->path)
                         ? LoadClipThumbPixels(wic, tr->path, tr->px, tr->w, tr->h)
                         : LoadImageFilePixels(wic, tr->path, tr->px, tr->w, tr->h,
                                               /*centerSquare=*/true);
            tr->ok = tr->ok && tr->w > 0 && tr->h > 0;
            if (!tr->ok) {
                tr->px.clear();
                tr->w = tr->h = 0;
                Wh_Log(L"No preview for %s", tr->path.c_str());
            }
            if (!hwnd || !PostMessageW(hwnd, WM_APP_THUMB, 0, (LPARAM)tr)) delete tr;
        }
        if (WaitForSingleObject(g_thumbStop, 0) == WAIT_OBJECT_0) break;
    }
    SafeRelease(wic);
    CoUninitialize();
    return 0;
}

static bool EnsureThumbThread() {
    if (g_thumbThread) return true;
    InitializeCriticalSection(&g_thumbLock);
    g_thumbEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    g_thumbStop = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (g_thumbEvent && g_thumbStop) {
        g_thumbThread = CreateThread(nullptr, 0, ThumbThreadProc, g_hwnd, 0, nullptr);
    }
    if (!g_thumbThread) {
        if (g_thumbEvent) CloseHandle(g_thumbEvent);
        g_thumbEvent = nullptr;
        if (g_thumbStop) CloseHandle(g_thumbStop);
        g_thumbStop = nullptr;
        DeleteCriticalSection(&g_thumbLock);
        return false;
    }
    return true;
}

// Asks the preview worker to exit and waits for it. Must run before the window is destroyed, so
// the worker cannot post previews to a dead window. A blocked decode (slow or disconnected
// drive) makes the wait time out; the process exits right after, so the handles stay open.
static void StopThumbThread() {
    if (!g_thumbThread) return;
    if (g_thumbStop) SetEvent(g_thumbStop);
    const DWORD waited = WaitForSingleObject(g_thumbThread, 3000);
    if (waited != WAIT_OBJECT_0) return;
    CloseHandle(g_thumbThread);
    g_thumbThread = nullptr;
    if (g_thumbEvent) CloseHandle(g_thumbEvent);
    g_thumbEvent = nullptr;
    if (g_thumbStop) CloseHandle(g_thumbStop);
    g_thumbStop = nullptr;
    DeleteCriticalSection(&g_thumbLock);
}

static void TrimThumbCache() {
    while (g_thumbCache.size() > kThumbCacheMax) {
        auto victim = g_thumbCache.begin();
        for (auto it = g_thumbCache.begin(); it != g_thumbCache.end(); ++it) {
            if (it->second.used < victim->second.used) victim = it;
        }
        SafeRelease(victim->second.bmp);
        g_thumbCache.erase(victim);
    }
}

// Returns the cached preview for an entry, queueing a background decode when there is none yet.
// Rows show their type tile until WM_APP_THUMB arrives and the bar redraws.
static ID2D1Bitmap* EnsureFileThumb(FileEntry& e) {
    if (e.isDir || !IsImageExt(e.ext) || FileThumbRemote(e)) return nullptr;
    auto it = g_thumbCache.find(e.path);
    if (it != g_thumbCache.end()) {
        it->second.used = ++g_thumbClock;
        return it->second.bmp;  // null when the decode failed
    }
    if (!g_thumbPending.count(e.path) && EnsureThumbThread()) {
        g_thumbPending.insert(e.path);
        EnterCriticalSection(&g_thumbLock);
        if (g_thumbQueue.size() < 64) {
            g_thumbQueue.push_back(e.path);
        } else {
            g_thumbPending.erase(e.path);  // try again on a later frame
        }
        LeaveCriticalSection(&g_thumbLock);
        SetEvent(g_thumbEvent);
    }
    return nullptr;
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

// Reads a file into a byte string, up to 256 KB. Only package manifests go through here.
static std::string ReadSmallFile(const std::wstring& path) {
    std::string out;
    HANDLE h = CreateFileW(NativeSystemPath(path).c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr,
                           OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h == INVALID_HANDLE_VALUE) return out;
    char buf[8192];
    DWORD got = 0;
    while (out.size() < 256 * 1024 && ReadFile(h, buf, sizeof(buf), &got, nullptr) && got > 0) {
        out.append(buf, got);
    }
    CloseHandle(h);
    return out;
}

// A full-trust MSIX app (Notepad, Terminal, …) names a real .exe in its manifest: that is what
// Windows itself starts when the user picks "Run as administrator" in the Start menu.
static std::wstring PackagedExecutable(const std::wstring& installPath, const std::wstring& aumid) {
    if (installPath.empty()) return std::wstring();
    const std::string xml = ReadSmallFile(installPath + L"\\AppxManifest.xml");
    if (xml.empty()) return std::wstring();

    auto attr = [](const std::string& chunk, const char* name) {
        const std::string key = std::string(name) + "=\"";
        const size_t p = chunk.find(key);
        if (p == std::string::npos) return std::string();
        const size_t b = p + key.size();
        const size_t e = chunk.find('"', b);
        return e == std::string::npos ? std::string() : chunk.substr(b, e - b);
    };
    auto widen = [](const std::string& s) {
        std::wstring w;
        w.reserve(s.size());
        for (const char c : s) w.push_back((wchar_t)(unsigned char)c);
        return w;
    };
    auto existing = [](const std::wstring& p) {
        return GetFileAttributesW(NativeSystemPath(p).c_str()) == INVALID_FILE_ATTRIBUTES ? std::wstring()
                                                                                          : p;
    };

    // The AUMID ends with the application id ("…!App"), and a package may declare more than one.
    const size_t bang = aumid.find(L'!');
    std::string wantId;
    if (bang != std::wstring::npos) {
        for (const wchar_t c : aumid.substr(bang + 1)) wantId.push_back((char)c);
    }

    std::string fallback;
    size_t pos = 0;
    while ((pos = xml.find("<Application", pos)) != std::string::npos) {
        const size_t end = xml.find('>', pos);
        const std::string chunk =
            xml.substr(pos, end == std::string::npos ? std::string::npos : end - pos);
        const std::string exe = attr(chunk, "Executable");
        // A package could name a path outside its own folder: the executable must stay inside it.
        if (!exe.empty() && exe.find("..") == std::string::npos &&
            exe.find(':') == std::string::npos) {
            // Only a full-trust application is a real Win32 program. A UWP one (Calculator,
            // Settings, Store) ships an .exe too, but launched without its package identity it
            // starts and exits without a word, so offering "Run as administrator" would be a lie.
            const bool fullTrust = attr(chunk, "EntryPoint") == "Windows.FullTrustApplication";
            if (!wantId.empty() && attr(chunk, "Id") == wantId) {
                // The AUMID names this application: it is this one or nothing. Falling back to a
                // sibling Application would elevate a different component of the package.
                return fullTrust ? existing(installPath + L"\\" + widen(exe)) : std::wstring();
            }
            if (fullTrust && fallback.empty()) fallback = exe;
        }
        pos = end == std::string::npos ? xml.size() : end + 1;
    }
    return fallback.empty() ? std::wstring() : existing(installPath + L"\\" + widen(fallback));
}

static AppDetails ComputeDetails(const AppEntry& a) {
    AppDetails d;
    d.packaged = IsPackagedId(a.id);

    std::wstring fullName;
    std::wstring installPath;

    if (d.packaged) {
        IShellItem2* item = nullptr;
        std::wstring parse = L"shell:AppsFolder\\" + a.id;
        if (SUCCEEDED(SHCreateItemFromParsingName(parse.c_str(), nullptr, IID_PPV_ARGS(&item))) &&
            item) {
            fullName = ItemString(item, L"System.AppUserModel.PackageFullName");
            installPath = ItemString(item, L"System.AppUserModel.PackageInstallPath");
            item->Release();
        }
        d.target.clear();  // MSIX apps are not launched or elevated by path
        // …except by elevation: a full-trust package does have an executable, and Windows offers
        // "Run as administrator" for it. The panel's path stays the package folder.
        d.elevateTarget = PackagedExecutable(installPath, a.id);
    } else if (!a.target.empty()) {
        // The index resolved the program when the app was enumerated.
        d.target = a.target;
        d.args = a.args;
    } else {
        IShellItem2* item = nullptr;
        std::wstring parse = L"shell:AppsFolder\\" + a.id;
        if (SUCCEEDED(SHCreateItemFromParsingName(parse.c_str(), nullptr, IID_PPV_ARGS(&item))) &&
            item) {
            d.target = CanonicalAppPath(ItemString(item, L"System.Link.TargetParsingPath"));
            d.args = ItemString(item, L"System.Link.Arguments");
            item->Release();
        }
        if (d.target.empty()) {
            const std::wstring p = PathFromAppId(a.id);
            // A shortcut file is not the program: showing its own path and size would be wrong.
            if (!EndsWith(FoldString(p), L".lnk")) d.target = p;
        }
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
        case ACT_PIN:
            return !a.pinned;
        case ACT_UNPIN:
            return a.pinned;
        case ACT_ADMIN:
            return (d.canShell && d.isExe) || !d.elevateTarget.empty();
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
    // Acrylic windows are slow to move on some Windows 10 builds, so the window is only touched
    // when its rect changed, it is hidden, or something got between it and g_hwnd. Both acrylic
    // windows sit right below g_hwnd, in either order.
    const bool visible = IsWindowVisible(wnd) != FALSE;
    const HWND above = GetWindow(wnd, GW_HWNDPREV);
    const bool stacked = above == g_hwnd ||
                         ((above == g_liveWnd || above == g_livePanel) && above &&
                          GetWindow(above, GW_HWNDPREV) == g_hwnd);
    int* last = g_livePos[idx];
    if (visible && stacked && last[0] == x && last[1] == y && last[2] == w && last[3] == h) return;
    last[0] = x;
    last[1] = y;
    last[2] = w;
    last[3] = h;
    UINT flags = SWP_NOACTIVATE;
    if (!visible) flags |= SWP_SHOWWINDOW;
    if (stacked) flags |= SWP_NOZORDER;
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
        if (a > 0.02f && g_panelMode == PanelMode::Actions) {  // Quick Look has a flat background
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

// `plain` draws the same card (shadow, border, rim) over a flat opaque fill: no acrylic, blur or
// captured desktop behind it. Used by the Quick Look panel.
static void DrawCard(float x, float y, float w, float h, float alpha, bool plain = false) {
    const float s = (float)g_scale;
    const bool live = !plain && UseLive();

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
    const bool solid = plain || g_cfg.effect == BgEffect::Solid;
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

// Splits a path into a dim head and an emphasised last segment, dropping the leading folders
// (shown as "…\") until it fits in maxW.
static void FitPathTail(const std::wstring& path, IDWriteTextFormat* fh, IDWriteTextFormat* ft,
                        float maxW, std::wstring& head, std::wstring& tail) {
    std::vector<std::wstring> seg;
    std::wstring cur;
    for (wchar_t c : path) {
        if (c == L'\\') {
            if (!cur.empty()) seg.push_back(cur);
            cur.clear();
        } else {
            cur.push_back(c);
        }
    }
    if (!cur.empty()) seg.push_back(cur);
    head.clear();
    if (seg.empty()) {
        tail = path;
        return;
    }
    const size_t n = seg.size();
    tail = seg[n - 1];
    if (n == 1) tail += L'\\';
    const float tw = TextWidth(tail.c_str(), ft);
    for (size_t k = 0; k < n; ++k) {
        std::wstring h = k > 0 ? L"…\\" : L"";
        for (size_t j = k; j + 1 < n; ++j) {
            h += seg[j];
            h += L'\\';
        }
        head = h;
        if (TextWidth(h.c_str(), fh) + tw <= maxW) return;
    }
}

static D2D1_RECT_F FilePathBackRect(float cx, float cy) {
    const float s = (float)g_scale;
    const float mid = cy + (kBarH + FileChipRowsH() + kPathH / 2) * s;
    return D2D1::RectF(cx + 10 * s, mid - 12 * s, cx + 34 * s, mid + 12 * s);
}

static void DrawRefreshIcon(float cx, float cy, float r, double angle, const D2D1_COLOR_F& color);

static const wchar_t* BandPrefix() { return g_bandEdit == BandEdit::Rename ? L"Rename" : L""; }

// Rectangle of the editable field that replaces the breadcrumb while editing.
static void BandFieldRect(float cx, float cy, float cw, float& x0, float& y0, float& x1, float& y1) {
    const float s = (float)g_scale;
    const float top = cy + (kBarH + FileChipRowsH()) * s;
    x0 = cx + 38 * s;
    x1 = cx + cw - 14 * s;
    y0 = top + 4 * s;
    y1 = top + (kPathH - 4) * s;
}

// X where the text starts inside the field (after the optional "Rename" label).
static float BandTextX(float x0) {
    const float s = (float)g_scale;
    float tx = x0 + 10 * s;
    const wchar_t* pre = BandPrefix();
    if (*pre) tx += TextWidth(pre, g_fmtHint) + 14 * s;
    return tx;
}

static IDWriteTextLayout* BandLayout() {
    IDWriteTextLayout* tl = nullptr;
    g_dw->CreateTextLayout(g_bandText.c_str(), (UINT32)g_bandText.size(), g_fmtMeta, 10000.f,
                           kPathH * (float)g_scale, &tl);
    return tl;
}

// Character index under a client x position (for clicks inside the field).
static int BandIndexAt(float cx, float cy, float cw, int mx) {
    float x0, y0, x1, y1;
    BandFieldRect(cx, cy, cw, x0, y0, x1, y1);
    IDWriteTextLayout* tl = BandLayout();
    if (!tl) return (int)g_bandText.size();
    BOOL trailing = FALSE, inside = FALSE;
    DWRITE_HIT_TEST_METRICS hm = {};
    tl->HitTestPoint((float)mx - (BandTextX(x0) - g_bandScroll), kPathH * (float)g_scale / 2,
                     &trailing, &inside, &hm);
    tl->Release();
    return std::clamp((int)hm.textPosition + (trailing ? (int)hm.length : 0), 0,
                      (int)g_bandText.size());
}

static void DrawBandEditor(float cx, float cy, float cw) {
    const float s = (float)g_scale;
    float x0, y0, x1, y1;
    BandFieldRect(cx, cy, cw, x0, y0, x1, y1);
    const float mid = (y0 + y1) / 2;
    const bool err = !g_bandError.empty();
    const D2D1_COLOR_F red = D2D1::ColorF(0.93f, 0.35f, 0.35f);
    FillRR(x0, y0, x1, y1, 8 * s, g_pal.selection, 0.7f);
    SetBrush(err ? red : g_pal.accent, 0.95f);
    D2D1_ROUNDED_RECT rr =
        D2D1::RoundedRect(D2D1::RectF(x0 + 0.5f, y0 + 0.5f, x1 - 0.5f, y1 - 0.5f), 8 * s, 8 * s);
    g_rt->DrawRoundedRectangle(&rr, g_brush, 1.3f);

    const wchar_t* pre = BandPrefix();
    if (*pre) {
        DrawLabel(pre, g_fmtHint, x0 + 10 * s, y0, x0 + 10 * s + TextWidth(pre, g_fmtHint) + 4 * s, y1,
                  g_pal.accentInk, 1.f);
    }
    float tr = x1 - 10 * s;
    if (err) {
        const float ew = TextWidth(g_bandError.c_str(), g_fmtHint);
        DrawLabel(g_bandError, g_fmtHint, tr - ew - 2 * s, y0, tr, y1, red, 1.f);
        tr -= ew + 14 * s;
    }
    const float tx = BandTextX(x0);
    IDWriteTextLayout* tl = BandLayout();
    if (!tl) return;
    float caretX = 0, tmp = 0;
    DWRITE_HIT_TEST_METRICS hm = {};
    tl->HitTestTextPosition((UINT32)g_bandCaret, FALSE, &caretX, &tmp, &hm);
    const float fieldW = tr - tx;
    if (caretX - g_bandScroll > fieldW - 4 * s) g_bandScroll = caretX - (fieldW - 4 * s);
    if (caretX - g_bandScroll < 0) g_bandScroll = caretX;
    if (g_bandScroll < 0) g_bandScroll = 0;
    const float ox = tx - g_bandScroll;
    const float top = cy + (kBarH + FileChipRowsH()) * s;

    D2D1_RECT_F clip = D2D1::RectF(tx, y0, tr, y1);
    g_rt->PushAxisAlignedClip(&clip, D2D1_ANTIALIAS_MODE_ALIASED);
    if (g_bandAnchor != g_bandCaret) {
        float ax = 0;
        DWRITE_HIT_TEST_METRICS hm2 = {};
        tl->HitTestTextPosition((UINT32)g_bandAnchor, FALSE, &ax, &tmp, &hm2);
        SetBrush(g_pal.caretSel);
        D2D1_RECT_F sr = D2D1::RectF(ox + std::min(ax, caretX), mid - 9 * s, ox + std::max(ax, caretX),
                                     mid + 9 * s);
        g_rt->FillRectangle(&sr, g_brush);
    }
    SetBrush(g_pal.text);
    g_rt->DrawTextLayout(D2D1::Point2F(ox, top), tl, g_brush);
    if (g_caretOn && g_vis != Vis::Hiding) {
        SetBrush(g_pal.text);
        D2D1_RECT_F cr = D2D1::RectF(ox + caretX, mid - 8 * s, ox + caretX + 1.5f * s, mid + 8 * s);
        g_rt->FillRectangle(&cr, g_brush);
    }
    g_rt->PopAxisAlignedClip();
    tl->Release();
}

// Breadcrumb band: back button, folder icon, the path (tail kept when it is long) and a status.
static void DrawPathBand(float cx, float cy, float cw) {
    const float s = (float)g_scale;
    const float top = cy + (kBarH + FileChipRowsH()) * s;
    const float mid = top + kPathH * s / 2;
    SetBrush(g_pal.separator, 0.7f);
    D2D1_RECT_F sep = D2D1::RectF(cx + 12 * s, top, cx + cw - 12 * s, top + 1.f);
    g_rt->FillRectangle(&sep, g_brush);

    const D2D1_RECT_F br = FilePathBackRect(cx, cy);
    D2D1_COLOR_F bc = g_fileBack.empty() ? g_pal.textDim : g_pal.text;
    if (g_fileBack.empty()) bc.a *= 0.4f;
    DrawCommandGlyph(CmdGlyph::ChevronLeft, br.left + 2 * s, mid - 10 * s, 20 * s, bc, bc);

    if (g_bandEdit != BandEdit::None) {
        DrawBandEditor(cx, cy, cw);
        return;
    }
    DrawFolderOutline(cx + 40 * s, mid - 8 * s, 16 * s, g_pal.accentInk);

    std::wstring status;
    const bool keyOk = FileQueryActive() && g_fileDeepKey == FileSearchKey();
    if (g_fileSearching) {
        wchar_t b[64];
        if (keyOk) swprintf_s(b, L"Searching… %d found", (int)g_results.size());
        else swprintf_s(b, L"Searching…");
        status = b;
    } else if (keyOk) {
        wchar_t b[48];
        swprintf_s(b, L"%d results%s", (int)g_results.size(), g_fileDeepTrunc ? L"+" : L"");
        status = b;
    } else if (!FileQueryActive() && (int)g_results.size() < (int)g_fileListTotal) {
        // The row list is capped (see BuildFileResults) and/or the folder is a very large one.
        wchar_t b[64];
        swprintf_s(b, g_fileListTrunc ? L"Showing first %d of %d" : L"Showing %d of %d",
                   (int)g_results.size(), (int)g_fileListTotal);
        status = b;
    }
    const std::wstring filt = FileFilterSummary();
    if (!filt.empty()) status = status.empty() ? filt : filt + L" · " + status;
    const float spinW = g_fileSearching ? 22 * s : 0.f;
    const float statusW = status.empty() ? 0.f : TextWidth(status.c_str(), g_fmtHint) + 14 * s + spinW;
    const float x0 = cx + 64 * s;
    const float x1 = cx + cw - 16 * s - statusW;
    if (!status.empty()) {
        const float sx1 = cx + cw - 16 * s;
        DrawLabel(status, g_fmtHint, x1, top, sx1, top + kPathH * s,
                  g_fileSearching ? g_pal.accentInk : g_pal.textDim, 1.f);
        if (g_fileSearching) {
            DrawRefreshIcon(sx1 - TextWidth(status.c_str(), g_fmtHint) - 12 * s, mid, 6.f * s,
                            std::fmod(NowSec() * 6.0, 2 * kPi), g_pal.accentInk);
        }
    }
    if (g_fileSearching) {
        // Thin indeterminate bar along the bottom of the band.
        const float bx0 = cx + 12 * s, bw = cw - 24 * s;
        const double t = std::fmod(NowSec() * 0.9, 1.0) * 1.3;
        const float l = (float)std::clamp(t - 0.3, 0.0, 1.0), r = (float)std::clamp(t, 0.0, 1.0);
        if (r > l) {
            SetBrush(g_pal.accent, 0.8f);
            D2D1_RECT_F pb = D2D1::RectF(bx0 + l * bw, top + kPathH * s - 2 * s, bx0 + r * bw,
                                         top + kPathH * s);
            g_rt->FillRectangle(&pb, g_brush);
        }
    }
    std::wstring head, tail;
    const std::wstring shownPath = InPcRoot() ? std::wstring(kTxtThisPc) : g_fileCur;
    FitPathTail(shownPath, g_fmtMeta, g_fmtPath, x1 - x0, head, tail);
    D2D1_RECT_F clip = D2D1::RectF(x0, top + 1.f, x1 + 2 * s, top + kPathH * s);
    g_rt->PushAxisAlignedClip(&clip, D2D1_ANTIALIAS_MODE_ALIASED);
    const float hw = TextWidth(head.c_str(), g_fmtMeta);
    DrawLabel(head, g_fmtMeta, x0, top, x0 + hw + 4 * s, top + kPathH * s, g_pal.textDim, 1.f);
    DrawLabel(tail, g_fmtPath, x0 + hw, top, x1 + 40 * s, top + kPathH * s, g_pal.text, 1.f);
    g_rt->PopAxisAlignedClip();
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

// Rectangle (surface pixels) of the footer help button: "Help [F1]".
static D2D1_RECT_F HelpButtonRect(float cx, float footerTop) {
    const float s = (float)g_scale;
    const float x0 = ReindexButtonRect(cx, footerTop).right + 8 * s;
    const float w = HintWidth(kTxtHelp, kHelpKeys, 1) + 8 * s;
    return D2D1::RectF(x0, footerTop + 5 * s, x0 + w, footerTop + (kFooterH - 5) * s);
}

// Rectangle (surface pixels) of the Apps/Files toggle button at the right of the search bar.
static D2D1_RECT_F FileToggleRect(float cx, float cy) {
    const float s = (float)g_scale;
    const float cw = kWidth * s;
    const float rightX = cx + cw - 24 * s;
    const float w = 34 * s;
    const float mid = cy + kBarH * s / 2;
    return D2D1::RectF(rightX - w, mid - w / 2, rightX, mid + w / 2);
}

// Rectangle (surface pixels) of the i-th location chip in the file-mode filter band.
static float FileChipWidth(int i) {
    const float s = (float)g_scale;
    if (i < 0 || i >= (int)g_chips.size()) return 0.f;
    // Sized with the bold format so the active (emphasized) label still fits.
    return 30 * s + TextWidth(g_chips[i].label.c_str(), g_fmtKeyBold) + 9 * s;
}

static D2D1_RECT_F FileChipRect(float cx, float cy, int i) {
    const float s = (float)g_scale;
    const bool drv = (i >= 0 && i < (int)g_chips.size()) && g_chips[i].isDrive;
    // Location chips on the first row; drive chips on the second, each laid out independently.
    const float mid = drv ? cy + (kBarH + kChipH + kDriveRowH / 2) * s : cy + (kBarH + kChipH / 2) * s;
    float x = cx + 14 * s;
    for (int j = 0; j < i; ++j) {
        if (((bool)g_chips[j].isDrive) == drv) x += FileChipWidth(j) + 8 * s;
    }
    const float w = FileChipWidth(i);
    return D2D1::RectF(x, mid - 14 * s, x + w, mid + 14 * s);
}

// Rounded "Files" chip at the left of the search box: folder icon + label + close button.
static D2D1_RECT_F FileModeChipRect(float cx, float cy) {
    const float s = (float)g_scale;
    const float mid = cy + kBarH * s / 2;
    const float h = 26 * s;
    const float labelW = TextWidth(kTxtFilesMode, g_fmtKeyBold);
    const float w = 64 * s + labelW;  // 10 + 16 + 8 + label + 10 + 12 + 8
    const float x = cx + 16 * s;
    return D2D1::RectF(x, mid - h / 2, x + w, mid + h / 2);
}

static D2D1_RECT_F FileModeChipCloseRect(float cx, float cy) {
    const float s = (float)g_scale;
    const D2D1_RECT_F r = FileModeChipRect(cx, cy);
    const float cs = 16 * s;
    const float midY = (r.top + r.bottom) / 2;
    return D2D1::RectF(r.right - 8 * s - cs, midY - cs / 2, r.right - 8 * s, midY + cs / 2);
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
// Actions menu: a popup anchored above the footer, at the right.
static void FileMenuRect(float cx, float cy, float cw, float& x0, float& y0, float& x1, float& y1,
                         float& itemH) {
    const float s = (float)g_scale;
    const int n = std::max(1, (int)g_fileMenu.size());
    const float footerTop = cy + (float)(ExpandedH() - kFooterH) * s;
    const float avail = footerTop - (cy + (kBarH + (float)FileChipsH()) * s) - 14 * s;
    itemH = std::min(28 * s, (avail - 10 * s) / n);
    x1 = cx + cw - 14 * s;
    x0 = x1 - 300 * s;
    y1 = footerTop - 6 * s;
    y0 = y1 - (n * itemH + 10 * s);
}

static void DrawFileMenu(float cx, float cy, float cw) {
    const float s = (float)g_scale;
    float x0, y0, x1, y1, ih;
    FileMenuRect(cx, cy, cw, x0, y0, x1, y1, ih);
    FillRR(x0 - 1 * s, y0 + 3 * s, x1 + 1 * s, y1 + 6 * s, 14 * s, D2D1::ColorF(0, 0, 0), 0.28f);
    FillRR(x0, y0, x1, y1, 12 * s, g_pal.bg);
    SetBrush(g_pal.border);
    D2D1_ROUNDED_RECT rr =
        D2D1::RoundedRect(D2D1::RectF(x0 + 0.5f, y0 + 0.5f, x1 - 0.5f, y1 - 0.5f), 12 * s, 12 * s);
    g_rt->DrawRoundedRectangle(&rr, g_brush, 1.f);
    const D2D1_COLOR_F red = D2D1::ColorF(0.93f, 0.40f, 0.40f);
    for (int i = 0; i < (int)g_fileMenu.size(); ++i) {
        const FileMenuItem& it = g_fileMenu[i];
        const float y = y0 + 5 * s + i * ih;
        const bool sel = (i == g_fileMenuSel);
        const bool del = (it.id == FA_DELETE);
        if (sel) {
            if (del && g_fileMenuArmed) FillRR(x0 + 5 * s, y, x1 - 5 * s, y + ih, 8 * s, red, 0.22f);
            else FillRR(x0 + 5 * s, y, x1 - 5 * s, y + ih, 8 * s, g_pal.accent, 0.20f);
        }
        const wchar_t* label = (del && g_fileMenuArmed) ? kTxtConfirm : it.label;
        const D2D1_COLOR_F fg = del ? red : (sel ? g_pal.text : g_pal.textDim);
        DrawLabel(label, g_fmtPanelRow, x0 + 16 * s, y, x1 - 16 * s, y + ih, fg, 1.f);
        if (it.keys && *it.keys && !(del && g_fileMenuArmed)) {
            DrawLabel(it.keys, g_fmtHint, x0 + 16 * s, y, x1 - 16 * s, y + ih, g_pal.textDim, 0.9f);
        }
    }
}

static void FooterDivider(float& x, float yMid) {
    const float s = (float)g_scale;
    x -= 12 * s;
    SetBrush(g_pal.separator);
    D2D1_RECT_F vd = D2D1::RectF(x, yMid - 9 * s, x + 1.f, yMid + 9 * s);
    g_rt->FillRectangle(&vd, g_brush);
    x -= 12 * s;
}

// Footer while the actions menu or the path/rename field has the keyboard.
static void DrawModalFooter(float cx, float cw, float yMid) {
    const float s = (float)g_scale;
    float x = cx + cw - 20 * s;
    if (g_fileMenuOpen) {
        const wchar_t* ok[1] = {kTxtEnterGlyph};
        x = DrawHint(L"Run", ok, 1, x, yMid);
        FooterDivider(x, yMid);
        const wchar_t* nav[2] = {L"↑", L"↓"};
        x = DrawHint(L"Navigate", nav, 2, x, yMid);
        FooterDivider(x, yMid);
        const wchar_t* esc[1] = {L"Esc"};
        DrawHint(L"Close", esc, 1, x, yMid);
        return;
    }
    const wchar_t* ok[1] = {kTxtEnterGlyph};
    x = DrawHint(g_bandEdit == BandEdit::Rename ? L"Rename" : L"Go", ok, 1, x, yMid);
    if (g_bandEdit == BandEdit::Path) {
        FooterDivider(x, yMid);
        const wchar_t* tab[1] = {L"Tab"};
        x = DrawHint(L"Complete", tab, 1, x, yMid);
    }
    FooterDivider(x, yMid);
    const wchar_t* esc[1] = {L"Esc"};
    DrawHint(L"Cancel", esc, 1, x, yMid);
}

// Semantic feedback colours: the same three wherever a message appears, independent of the theme so
// the meaning never gets lost in a purple or orange palette.
static const D2D1_COLOR_F kToneOk = D2D1::ColorF(0.44f, 0.82f, 0.56f);     // done: copied, saved
static const D2D1_COLOR_F kToneWarn = D2D1::ColorF(0.98f, 0.73f, 0.32f);   // worked around a problem
static const D2D1_COLOR_F kToneError = D2D1::ColorF(0.94f, 0.42f, 0.42f);  // something failed

// A short message in the footer with a coloured dot, the way a launcher reports one-off news
// ("Copied", "Everything not running", "again to confirm").
static void DrawNotice(float right, float yMid, const wchar_t* text, const D2D1_COLOR_F& tone) {
    const float s = (float)g_scale;
    const float w = TextWidth(text, g_fmtHint);
    const float x = right - w;
    FillRR(x - 11 * s, yMid - 3 * s, x - 5 * s, yMid + 3 * s, 3 * s, tone);
    DrawLabel(text, g_fmtHint, x, yMid - 12 * s, right, yMid + 12 * s, tone, 1.f);
}

static void DrawFooter(float cx, float footerTop, float cw, int sel) {
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

    if (g_fileMenuOpen || g_bandEdit != BandEdit::None) {
        DrawModalFooter(cx, cw, yMid);
        return;
    }

    // Help is always one key (or click) away, even with an empty list.
    {
        const D2D1_RECT_F hb = HelpButtonRect(cx, footerTop);
        if (g_hoverHelp) FillRR(hb.left, hb.top, hb.right, hb.bottom, 8 * s, g_pal.selection);
        DrawHint(kTxtHelp, kHelpKeys, 1, hb.right - 6 * s, yMid);
    }

    // Copying keeps the bar open, so the footer confirms it for a moment.
    const bool copiedResult = sel >= 0 && sel < (int)g_results.size() &&
                              (g_results[sel].kind == ResultKind::Bookmark ||
                               g_results[sel].kind == ResultKind::Clip ||
                               g_results[sel].kind == ResultKind::Window ||
                               IsPathResult(g_results[sel].kind));
    if ((g_fileMode || copiedResult) && NowSec() - g_fileCopiedAt < 1.4) {
        DrawNotice(cx + cw - 20 * s, yMid, kTxtCalcCopied, kToneOk);
        return;
    }

    // "find:" or the Files deep search with the Everything engine: say so when Everything was not
    // available and the results came from the built-in scan instead.
    if (g_cfg.fileSearchEngine == FileSearchEngine::Everything && NowSec() - g_evFailedAt < 6.0 &&
        (GlobalModeActive() || (g_fileMode && FileQueryActive()))) {
        DrawNotice(cx + cw - 20 * s, yMid, L"Everything not running · built-in scan", kToneWarn);
        return;
    }

    // The calculation owns the footer while its history is on screen: it offers removing the
    // highlighted entry or clearing the whole history (same shortcut as "remove from recents").
    if (CalcHistoryVisible()) {
        const wchar_t* dk[3] = {L"Ctrl", L"Shift", L"R"};
        const float x = cx + cw - 20 * s;
        DrawHint(g_calcHistSel >= 0 ? kTxtCalcDelete : kTxtCalcClear, dk, 3, x, yMid);
        return;
    }
    if (sel < 0 || sel >= (int)g_results.size()) return;
    const ResultKind kind = g_results[sel].kind;

    // A destructive command is armed: show a red warning until the second Enter (or a timeout).
    if (kind == ResultKind::Command && g_confirmCmd >= 0 && g_results[sel].icon == g_confirmCmd) {
        DrawNotice(cx + cw - 20 * s, yMid, kTxtConfirm, kToneError);
        return;
    }
    // Ctrl+W is the window's own confirmation: the warning names the key that must be pressed again.
    if (kind == ResultKind::Window && g_winCloseArmed) {
        DrawNotice(cx + cw - 20 * s, yMid, L"Ctrl+W again to close", kToneError);
        return;
    }
    // The window refused the last action (elevated app, no other monitor): say why instead of
    // failing silently.
    if (kind == ResultKind::Window && NowSec() - g_winNoticeAt < 3.0) {
        DrawNotice(cx + cw - 20 * s, yMid,
                   g_winNoticeMsg ? g_winNoticeMsg : L"Windows refused that action",
                   g_winNoticeTone);
        return;
    }
    // "find:" has no path band like Files, so the footer is the only place that can say the scan is
    // still running once rows are already on screen.
    if (GlobalModeActive() && g_globalSearching) {
        DrawNotice(cx + cw - 20 * s, yMid, kTxtSearching, g_pal.accentInk);
        return;
    }

    float x = cx + cw - 20 * s;
    if (kind == ResultKind::App) {
        const wchar_t* dk[2] = {L"Ctrl", L"I"};
        int nk = 2;
        switch (g_cfg.detailsKey) {
            case DetailsKey::CtrlI:     dk[1] = L"I"; break;
            case DetailsKey::CtrlK:     dk[1] = L"K"; break;
            case DetailsKey::CtrlSpace: dk[1] = L"Space"; break;
            case DetailsKey::F4:        dk[0] = L"F4"; nk = 1; break;
        }
        x = DrawHint(kTxtDetails, dk, nk, x, yMid);

        x -= 12 * s;
        SetBrush(g_pal.separator);
        D2D1_RECT_F vdiv = D2D1::RectF(x, yMid - 9 * s, x + 1.f, yMid + 9 * s);
        g_rt->FillRectangle(&vdiv, g_brush);
        x -= 12 * s;
    }

    const wchar_t* ok[1] = {kTxtEnterGlyph};
    if (kind == ResultKind::File) {
        // The file footer only keeps the primary action: the rest (reveal, actions, back) is in
        // the F1 cheat sheet, so the hints never crowd the reindex and help buttons.
        const int fi = g_results[sel].icon;
        const bool isDir = fi >= 0 && fi < (int)FileSrc().size() && FileSrc()[fi].isDir;
        x = DrawHint(isDir ? kTxtBrowse : kTxtOpen, ok, 1, x, yMid);
        // Quick Look hint (only for previewable files, not folders)
        if (!isDir) {
            x -= 12 * s;
            SetBrush(g_pal.separator);
            D2D1_RECT_F vdiv = D2D1::RectF(x, yMid - 9 * s, x + 1.f, yMid + 9 * s);
            g_rt->FillRectangle(&vdiv, g_brush);
            x -= 12 * s;
            const wchar_t* qk[2] = {L"Ctrl", L"Q"};
            DrawHint(L"Preview", qk, 2, x, yMid);
        }
        return;
    }
    if (kind == ResultKind::Bookmark) {
        // Open is the primary action; the address copy stays discoverable next to it.
        x = DrawHint(kTxtOpen, ok, 1, x, yMid);
        FooterDivider(x, yMid);
        const wchar_t* ck[2] = {L"Ctrl", kTxtEnterGlyph};
        DrawHint(L"Copy address", ck, 2, x, yMid);
        return;
    }
    if (kind == ResultKind::Clip) {
        x = DrawHint(L"Paste", ok, 1, x, yMid);
        FooterDivider(x, yMid);
        const wchar_t* ck[2] = {L"Ctrl", kTxtEnterGlyph};
        x = DrawHint(L"Copy", ck, 2, x, yMid);
        FooterDivider(x, yMid);
        bool pinned = false;
        for (const ClipEntry& c : g_clips) {
            if (c.text == g_results[sel].payload) {
                pinned = c.pinned;
                break;
            }
        }
        const wchar_t* pk[2] = {L"Ctrl", L"D"};
        DrawHint(pinned ? L"Unpin" : L"Pin", pk, 2, x, yMid);
        return;
    }
    if (kind == ResultKind::Window) {
        // The panel lists the rest; these two are the ones worth a key even with it closed.
        const wchar_t* cw[2] = {L"Ctrl", L"W"};
        x = DrawHint(L"Close", cw, 2, x, yMid);
        FooterDivider(x, yMid);
        const wchar_t* cm[2] = {L"Ctrl", L"M"};
        x = DrawHint(L"Minimize", cm, 2, x, yMid);
        FooterDivider(x, yMid);
        x = DrawHint(L"Switch", ok, 1, x, yMid);
        return;
    }
    if (IsPathResult(kind)) {
        x = DrawHint(kTxtOpen, ok, 1, x, yMid);
        FooterDivider(x, yMid);
        const wchar_t* fk[2] = {L"Ctrl", kTxtEnterGlyph};
        DrawHint(g_results[sel].isDir ? L"Open folder" : L"Show in folder", fk, 2, x, yMid);
        return;
    }
    if (kind == ResultKind::Custom) {
        const int ci = g_results[sel].custom;
        if (ci >= 0 && ci < (int)g_cfg.commands.size() && CustomHasFormFields(g_cfg.commands[ci])) {
            // Enter opens the form to fill/review the fields; Shift+Enter skips it and launches now.
            const bool run = g_cfg.commands[ci].type == L"run";
            const wchar_t* sk[2] = {L"Shift", kTxtEnterGlyph};
            x = DrawHint(run ? kTxtRun : kTxtOpen, sk, 2, x, yMid);
            FooterDivider(x, yMid);
            DrawHint(kTxtFormEdit, ok, 1, x, yMid);
            return;
        }
    }
    DrawHint(kind == ResultKind::Command ? kTxtRun : kTxtOpen, ok, 1, x, yMid);
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

// True when a client point is over the (fully revealed) footer help button.
static bool HitHelp(int mx, int my) {
    if (g_vis == Vis::Hidden || !g_fmtHint || g_curH < ExpandedH() - 2) return false;
    const float s = (float)g_scale;
    float cx = 0, cy = 0;
    MainOrigin(cx, cy);
    const D2D1_RECT_F r = HelpButtonRect(cx, cy + (float)(ExpandedH() - kFooterH) * s);
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

// Keys for the window panel. They are shown, never clicked, so there is no hit test or hover state.
struct WinActionRow {
    const wchar_t* label;
    const wchar_t* keys;
};
static const WinActionRow kWinActionRows[] = {
    {L"Close window", L"Ctrl+W"},
    {L"Minimize", L"Ctrl+M"},
    {L"Maximize / restore", L"Ctrl+Shift+M"},
    {L"Always on top", L"Ctrl+Shift+T"},
    {L"Move to next monitor", L"Ctrl+Shift+→"},
    {L"Move to previous monitor", L"Ctrl+Shift+←"},
    {L"Open file location", L"Ctrl+↵"},
    {L"Copy path", L"Ctrl+Shift+C"},
};

// The blue initial tile used when neither the app index nor the exe gives a usable icon. Shared by
// the row and the panel so both show the same thing.
static void DrawWindowTile(float x, float y, float px, float radius, const std::wstring& label) {
    FillRR(x, y, x + px, y + px, radius, D2D1::ColorF(0.30f, 0.40f, 0.58f));
    wchar_t ch = L'?';
    for (const wchar_t c : label) {
        if (iswalnum(c)) {
            ch = (wchar_t)towupper(c);
            break;
        }
    }
    SetBrush(D2D1::ColorF(1.f, 1.f, 1.f));
    D2D1_RECT_F lr = D2D1::RectF(x, y, x + px, y + px);
    g_rt->DrawText(&ch, 1, g_fmtBadge, &lr, g_brush);
}

// Same card as the app panel, with the window's own keys. Nothing here activates the window, so the
// bar stays open and the list behind it keeps its selection.
static void DrawWindowPanel(const WinPanelInfo& w) {
    const float s = (float)g_scale;
    const float a = (float)std::clamp(g_detailPos, 0.0, 1.0);
    float mainX = 0, mainY = 0;
    MainOrigin(mainX, mainY);
    const float ox = mainX + (kWidth + kPanelGap + (1.f - a) * 14.f) * s;
    const float oy = mainY;
    const float pw = kPanelW * s;
    const float ph = (float)DetailsPanelH() * s;
    const float pad = kPanelPad * s;
    const float cardW = UseLive() ? std::max(1.f, pw * a) : pw;

    DrawCard(ox, oy, cardW, ph, a);

    D2D1_RECT_F clip = D2D1::RectF(ox, oy, ox + cardW, oy + ph);
    g_rt->PushAxisAlignedClip(&clip, D2D1_ANTIALIAS_MODE_ALIASED);

    const float icon = 44 * s;
    ID2D1Bitmap* bmp = (w.iconApp >= 0 && w.iconApp < (int)g_apps.size())
                           ? EnsureIcon(g_apps[w.iconApp])
                           : nullptr;
    if (!bmp) bmp = EnsureWindowIcon(w.path);  // the same fallback as the row
    if (bmp) {
        D2D1_RECT_F dst = D2D1::RectF(ox + pad, oy + pad, ox + pad + icon, oy + pad + icon);
        g_rt->DrawBitmap(bmp, &dst, a, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
    } else {
        DrawWindowTile(ox + pad, oy + pad, icon, 10 * s,
                       w.process.empty() ? w.title : w.process);
    }
    const float tx = ox + pad + icon + 12 * s;
    DrawLabel(w.title, g_fmtPanelTitle, tx, oy + pad, ox + pw - pad, oy + pad + 24 * s, g_pal.text, a);
    DrawLabel(w.process.empty() ? L"Window" : w.process, g_fmtPanelSub, tx, oy + pad + 24 * s,
              ox + pw - pad, oy + pad + 44 * s, g_pal.textDim, a);

    DrawLabel(kTxtActions, g_fmtSection, ox + pad, oy + 68 * s, ox + pw - pad, oy + 86 * s,
              g_pal.textDim, a);
    const int n = (int)ARRAYSIZE(kWinActionRows);
    const float actTop = oy + kPanelActionsTop * s;
    for (int k = 0; k < n; ++k) {
        const float y0 = actTop + k * kActionH * s;
        DrawLabel(kWinActionRows[k].label, g_fmtPanelRow, ox + pad, y0, ox + pw - pad,
                  y0 + kActionH * s, g_pal.text, a);
        DrawLabel(kWinActionRows[k].keys, g_fmtHint, ox + pad, y0, ox + pw - pad,
                  y0 + kActionH * s, g_pal.textDim, a);
    }

    const float sepY = actTop + n * kActionH * s + 8 * s;
    SetBrush(g_pal.separator, a);
    D2D1_RECT_F sep = D2D1::RectF(ox + pad, sepY, ox + pw - pad, sepY + 1.f);
    g_rt->FillRectangle(&sep, g_brush);

    const float infoTop = sepY + 10 * s;
    DrawLabel(kTxtInfo, g_fmtSection, ox + pad, infoTop, ox + pw - pad, infoTop + 16 * s,
              g_pal.textDim, a);

    InfoRow rows[3];
    rows[0].label = L"Process";
    rows[0].value = w.process.empty() ? L"—" : w.process;
    rows[1].label = L"State";
    rows[1].value = w.state;
    rows[2].label = L"Path";
    rows[2].value = w.path.empty() ? L"—" : w.path;
    rows[2].middle = true;

    const float labelW = 84 * s;
    const float valX = ox + pad + labelW + 8 * s;
    const float valW = (ox + pw - pad) - valX;
    float ry = infoTop + 22 * s;
    for (const InfoRow& r : rows) {
        if (ry + kInfoRowH * s > oy + ph - 10 * s) break;
        DrawLabel(r.label, g_fmtPanelRow, ox + pad, ry, ox + pad + labelW, ry + kInfoRowH * s,
                  g_pal.textDim, a);
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
    if (g_results[g_sel].kind != ResultKind::App) return -1;
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
            SetBrush(g_pal.accentInk);
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

// ---- Help sheet (F1) ----

struct HelpRow {
    const wchar_t* key;
    const wchar_t* desc;
};

struct HelpSection {
    const wchar_t* title;
    const HelpRow* rows;
    int count;
};

static const wchar_t* const kHelpTabs[2] = {L"Shortcuts", L"Calculator"};

// Rectangle (surface pixels) of a page tab at the top of the help sheet.
static D2D1_RECT_F HelpTabRect(float cx, float cy, int i) {
    const float s = (float)g_scale;
    const float top = cy + (kBarH + 12) * s;
    const float x0 = cx + 26 * s;
    const float w0 = TextWidth(kHelpTabs[0], g_fmtKeyBold) + 22 * s;
    if (i == 0) return D2D1::RectF(x0, top, x0 + w0, top + 22 * s);
    const float x1 = x0 + w0 + 6 * s;
    const float w1 = TextWidth(kHelpTabs[1], g_fmtKeyBold) + 22 * s;
    return D2D1::RectF(x1, top, x1 + w1, top + 22 * s);
}

// Labels for the F1 sheet, built from the configured shortcuts so the sheet never shows a
// combination the user has changed.
static std::wstring HotkeyHelpText() {
    switch (g_cfg.hotkeyMods) {
        case MOD_CONTROL: return L"Ctrl + Space";
        case MOD_CONTROL | MOD_ALT: return L"Ctrl + Alt + Space";
        case MOD_SHIFT | MOD_ALT: return L"Shift + Alt + Space";
        case MOD_CONTROL | MOD_SHIFT: return L"Ctrl + Shift + Space";
        default: return L"Alt + Space";
    }
}

static std::wstring DetailsKeyHelpText() {
    switch (g_cfg.detailsKey) {
        case DetailsKey::CtrlK: return L"Ctrl + K";
        case DetailsKey::CtrlSpace: return L"Ctrl + Space";
        case DetailsKey::F4: return L"F4";
        default: return L"Ctrl + I";
    }
}

static const HelpRow kHelpPrefixRows[] = {
    {L"? g: b: d: y: w:", L"Web search (any engine)"},
    {L"bm: clip:", L"Bookmarks / clipboard"},
    {L"recent: find:", L"Recent files / every folder"},
    {L"win: >", L"Open windows / commands"},
    {L"Open Windows, Bookmarks…", L"Type a mode name to enter it"},
};

static const HelpRow kHelpClipRows[] = {
    {L"Enter / Ctrl + Enter", L"Paste it / just copy it"},
    {L"Ctrl + D", L"Pin or unpin an entry"},
    {L"Ctrl + Shift + R", L"Remove an entry"},
};

// The keys only do something in "win:", on the selected window.
static const HelpRow kHelpWindowRows[] = {
    {L"Ctrl + W", L"Close it (press it twice)"},
    {L"Ctrl + M", L"Minimize"},
    {L"Ctrl + Shift + M", L"Maximize or restore"},
    {L"Ctrl + Shift + T", L"Keep it always on top"},
    {L"Ctrl + Shift + ← / →", L"Move between monitors"},
    {L"Ctrl + Enter", L"Open its program folder"},
    {L"Ctrl + Shift + C", L"Copy the program path"},
};

static const HelpRow kHelpCalcRows[] = {
    {L"12*8, 15%", L"Calculations as you type"},
    {L"Enter", L"Copy the result"},
};

static const HelpRow kHelpFileRows[] = {
    {L"Tab", L"Enter or leave the file browser"},
    {L"Enter / →", L"Open the file or folder"},
    {L"Backspace", L"Go up one folder"},
    {L"Alt + ← / →", L"Back and forward"},
    {L"Ctrl + 1…9", L"Jump to a location or drive"},
    {L"Ctrl + Tab", L"Switch drive (C:, D:, …)"},
    {L"Ctrl + L / Alt + D", L"Edit the path"},
    {L"Ctrl + Q", L"Preview the file"},
    {L"Ctrl + G", L"Show it in its folder"},
    {L"Ctrl + S", L"Sort the list"},
    {L"F2", L"Rename"},
    {L"Ctrl + Shift + N", L"New folder"},
    {L"Ctrl + T", L"Terminal in this folder"},
    {L"Ctrl + Shift + C", L"Copy the path"},
};

static const HelpRow kHelpFilterRows[] = {
    {L"*.pdf", L"Match by extension"},
    {L"type:image", L"Images, documents, audio…"},
    {L"dir: / file:", L"Only folders / only files"},
    {L"size:>10mb", L"By size (<, >, 1mb..10mb)"},
    {L"modified:today", L"By date (week, 2026-01-31)"},
};

static const HelpRow kCalcOpRows[] = {
    {L"+ - * /", L"Add, subtract, multiply, divide"},
    {L"^", L"Power: 2^10 = 1024"},
    {L"%", L"Percent: 15% = 0.15"},
    {L"100 + 15%", L"Add or subtract a percentage"},
    {L"! or fact(x)", L"Factorial: 5! = 120"},
    {L"mod", L"Remainder: 10 mod 3 = 1"},
    {L"of", L"Multiply: 20% of 150 = 30"},
    {L"( )", L"Group: (2+3)*4 = 20"},
};

static const HelpRow kCalcConstRows[] = {
    {L"pi, e, tau", L"3.14159, 2.71828, 6.28318"},
    {L"phi", L"Golden ratio: 1.61803"},
    {L"ans", L"Last result"},
};

static const HelpRow kCalcFnRows[] = {
    {L"sqrt cbrt", L"Square and cube root"},
    {L"abs sign", L"Absolute value, -1 / 0 / 1"},
    {L"round floor ceil trunc", L"Rounding"},
    {L"sin cos tan", L"Trigonometry (radians)"},
    {L"asin acos atan", L"Inverse trigonometry"},
    {L"sinh cosh tanh", L"Hyperbolic functions"},
    {L"log ln log2 exp", L"Logs and e raised to x"},
};

static const HelpRow kCalcUnitRows[] = {
    {L"10 km to mi", L"Convert compatible units"},
    {L"100 f to c", L"Temperature"},
    {L"255 to hex", L"Output hex, bin or oct"},
    {L"0xFF 0b1010 0o17", L"Hex, binary and octal input"},
};

static const HelpRow kCalcExampleRows[] = {
    {L"100 - 15%", L"= 85"},
};

static void DrawHelpColumn(const HelpSection* secs, int n, float x, float y, float w, float s) {
    const float headerH = 22 * s;
    const float rowH = 17 * s;
    const float gap = 6 * s;
    for (int i = 0; i < n; ++i) {
        const HelpSection& sec = secs[i];
        DrawLabel(sec.title, g_fmtSection, x, y, x + w, y + headerH, g_pal.accentStrong, 1.f);
        y += headerH;
        for (int r = 0; r < sec.count; ++r) {
            const HelpRow& row = sec.rows[r];
            const float kw = TextWidth(row.key, g_fmtMeta);
            DrawLabel(row.key, g_fmtMeta, x, y, x + kw + 0.5f, y + rowH, g_pal.text, 1.f);
            DrawLabel(row.desc, g_fmtMeta, x + kw + 12 * s, y, x + w, y + rowH, g_pal.textDim, 1.f);
            y += rowH;
        }
        y += gap;
    }
}

// Height one column needs, so the sheet can scroll instead of growing.
static float HelpColumnsHeight(const HelpSection* secs, int n, float s) {
    const float headerH = 22 * s;
    const float rowH = 17 * s;
    const float gap = 6 * s;
    float h = 0;
    for (int i = 0; i < n; ++i) h += headerH + secs[i].count * rowH + gap;
    return h;
}

// Keeps the sheet's scroll inside what the page actually needs (and remembers the limits for the
// wheel and the keys).
static void ClampHelpScroll(float contentH, float viewH) {
    g_helpMaxScroll = std::max(0.f, contentH - viewH);
    g_helpScroll = std::clamp(g_helpScroll, 0.f, g_helpMaxScroll);
    g_helpViewH = viewH;
}

// The sheet fills the expanded card: the bar stays on top, two tabs pick the page and the footer
// says how to switch pages and close. A page that does not fit scrolls with the wheel or the arrows.
static void DrawHelp(float cx, float cy, float cw, float ch, float s) {
    const float top = cy + (kBarH + 12) * s;
    const float contentTop = top + 28 * s;
    const float bottom = cy + ch - 30 * s;
    if (bottom <= contentTop) return;

    SetBrush(g_pal.separator);
    D2D1_RECT_F topSep =
        D2D1::RectF(cx + 12 * s, cy + kBarH * s, cx + cw - 12 * s, cy + kBarH * s + 1.f);
    g_rt->FillRectangle(&topSep, g_brush);

    for (int i = 0; i < 2; ++i) {
        const D2D1_RECT_F tr = HelpTabRect(cx, cy, i);
        const bool on = (g_helpPage == i);
        if (on) FillRR(tr.left, tr.top, tr.right, tr.bottom, 11 * s, g_pal.accent, 0.20f);
        DrawLabel(kHelpTabs[i], on ? g_fmtKeyBold : g_fmtKey, tr.left, tr.top, tr.right, tr.bottom,
                  on ? g_pal.accentInk : g_pal.textDim, 1.f);
    }

    const float pad = 26 * s;
    const float gap = 28 * s;
    const float colW = std::max(0.f, (cw - 2 * pad - gap) / 2);
    const float rx = cx + pad + colW + gap;
    D2D1_RECT_F clip = D2D1::RectF(cx, contentTop, cx + cw, bottom);
    g_rt->PushAxisAlignedClip(&clip, D2D1_ANTIALIAS_MODE_ALIASED);

    if (g_helpPage == 0) {
        const std::wstring hotkey = HotkeyHelpText();
        const std::wstring details = DetailsKeyHelpText();
        const HelpRow navRows[] = {
            {hotkey.c_str(), L"Open or close the bar"},
            {L"↑ / ↓", L"Move through the results"},
            {L"Enter", L"Open the selected item"},
            {L"Esc", L"Close the details panel, then the bar"},
        };
        const HelpRow actionRows[] = {
            {details.c_str(), L"Actions and details (configurable)"},
            {L"Ctrl + D", L"Pin or unpin the selected app"},
            {L"Ctrl + R", L"Rebuild the app index"},
        };
        const HelpSection left[] = {
            {L"GETTING AROUND", navRows, ARRAYSIZE(navRows)},
            {L"ACTIONS", actionRows, ARRAYSIZE(actionRows)},
            {L"PREFIXES", kHelpPrefixRows, ARRAYSIZE(kHelpPrefixRows)},
            {L"WINDOWS (win:)", kHelpWindowRows, ARRAYSIZE(kHelpWindowRows)},
            {L"CLIPBOARD", kHelpClipRows, ARRAYSIZE(kHelpClipRows)},
            {L"CALCULATOR", kHelpCalcRows, ARRAYSIZE(kHelpCalcRows)},
        };
        const HelpSection right[] = {
            {L"FILES", kHelpFileRows, ARRAYSIZE(kHelpFileRows)},
            {L"FILTERS", kHelpFilterRows, ARRAYSIZE(kHelpFilterRows)},
        };
        float contentH = HelpColumnsHeight(left, ARRAYSIZE(left), s);
        if (g_cfg.filesEnabled) {
            contentH = std::max(contentH, HelpColumnsHeight(right, ARRAYSIZE(right), s));
        }
        ClampHelpScroll(contentH, bottom - contentTop);
        const float y = contentTop - g_helpScroll;
        DrawHelpColumn(left, ARRAYSIZE(left), cx + pad, y, colW, s);

        if (g_cfg.filesEnabled) {
            DrawHelpColumn(right, ARRAYSIZE(right), rx, y, colW, s);
        } else {
            DrawLabel(L"FILES", g_fmtSection, rx, y, rx + colW, y + 22 * s,
                      g_pal.accentStrong, 1.f);
            DrawLabel(L"File browser is off in the mod's settings.", g_fmtMeta, rx, y + 22 * s,
                      rx + colW, y + 39 * s, g_pal.textDim, 1.f);
        }
    } else {
        const HelpSection left[] = {
            {L"OPERATORS", kCalcOpRows, ARRAYSIZE(kCalcOpRows)},
            {L"CONSTANTS", kCalcConstRows, ARRAYSIZE(kCalcConstRows)},
        };
        const HelpSection right[] = {
            {L"FUNCTIONS", kCalcFnRows, ARRAYSIZE(kCalcFnRows)},
            {L"NUMBERS & UNITS", kCalcUnitRows, ARRAYSIZE(kCalcUnitRows)},
            {L"EXAMPLE", kCalcExampleRows, ARRAYSIZE(kCalcExampleRows)},
        };
        const float contentH = std::max(HelpColumnsHeight(left, ARRAYSIZE(left), s),
                                        HelpColumnsHeight(right, ARRAYSIZE(right), s));
        ClampHelpScroll(contentH, bottom - contentTop);
        const float y = contentTop - g_helpScroll;
        DrawHelpColumn(left, ARRAYSIZE(left), cx + pad, y, colW, s);
        DrawHelpColumn(right, ARRAYSIZE(right), rx, y, colW, s);
    }
    g_rt->PopAxisAlignedClip();

    SetBrush(g_pal.separator);
    D2D1_RECT_F sep =
        D2D1::RectF(cx + 12 * s, cy + ch - 30 * s, cx + cw - 12 * s, cy + ch - 30 * s + 1.f);
    g_rt->FillRectangle(&sep, g_brush);
    const float hintMid = cy + ch - 15 * s;
    const wchar_t* closeKeys[2] = {L"Esc", L"F1"};
    float hx = DrawHint(L"Close", closeKeys, 2, cx + cw - 24 * s, hintMid);
    FooterDivider(hx, hintMid);
    const wchar_t* tabKeys[1] = {L"Tab"};
    hx = DrawHint(g_helpPage == 0 ? L"Calculator" : L"Shortcuts", tabKeys, 1, hx, hintMid);
    if (g_helpMaxScroll > 0) {
        FooterDivider(hx, hintMid);
        const wchar_t* scrollKeys[2] = {L"↑", L"↓"};
        DrawHint(L"Scroll", scrollKeys, 2, hx, hintMid);
    }
}

// ===== Preview helpers =====

static void LoadPreview(const std::wstring& path) {
    if (g_preview.path == path && g_preview.loaded) return;  // already loaded
    g_preview = PreviewState{};
    g_preview.path = path;

    // Determine type by extension (uppercase)
    auto dotPos = path.rfind(L'.');
    std::wstring ext;
    if (dotPos != std::wstring::npos) {
        ext = path.substr(dotPos + 1);
        for (auto& c : ext) c = (wchar_t)towupper(c);
    }

    if (IsImageExt(ext)) {
        g_preview.isImage = true;
        g_preview.loaded  = true;
        return;
    }

    // Text / JSON / code: read up to 2000 lines.
    static const std::wstring kTextExts[] = {
        L"TXT", L"MD", L"JSON", L"XML", L"YAML", L"YML", L"CSV",
        L"LOG", L"INI", L"CFG", L"CONF", L"TOML", L"JS", L"TS",
        L"JSX", L"TSX", L"PY", L"RB", L"CPP", L"C", L"H", L"HPP",
        L"CS", L"JAVA", L"KT", L"SWIFT", L"GO", L"RS", L"SH",
        L"BAT", L"CMD", L"PS1", L"HTML", L"CSS", L"SCSS", L"LESS",
        L"SQL", L"DART", L"LUA", L"PHP", L"R", L"M", L"VB",
        L"DIFF", L"PATCH", L"NFO", L"README",
    };
    bool isText = ext.empty();  // no extension → try as text
    for (auto& te : kTextExts) {
        if (ext == te) { isText = true; break; }
    }

    if (!isText) {
        g_preview.errorMsg = L"Preview not available for this file type.";
        g_preview.loaded = true;
        return;
    }

    HANDLE hf = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr,
                            OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hf == INVALID_HANDLE_VALUE) {
        g_preview.errorMsg = L"Cannot open file.";
        g_preview.loaded = true;
        return;
    }

    // Read up to 256 KB
    const DWORD kMaxBytes = 256 * 1024;
    std::string raw(kMaxBytes, '\0');
    DWORD read = 0;
    ReadFile(hf, raw.data(), kMaxBytes, &read, nullptr);
    CloseHandle(hf);
    raw.resize(read);

    // Convert to wstring (try UTF-8, fall back to system codepage)
    std::wstring text;
    int wlen = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                                   raw.c_str(), (int)raw.size(), nullptr, 0);
    if (wlen > 0) {
        text.resize(wlen);
        MultiByteToWideChar(CP_UTF8, 0, raw.c_str(), (int)raw.size(), text.data(), wlen);
    } else {
        wlen = MultiByteToWideChar(CP_ACP, 0, raw.c_str(), (int)raw.size(), nullptr, 0);
        if (wlen > 0) {
            text.resize(wlen);
            MultiByteToWideChar(CP_ACP, 0, raw.c_str(), (int)raw.size(), text.data(), wlen);
        }
    }

    if (!text.empty() && text[0] == 0xFEFF) text.erase(0, 1);  // UTF-8 BOM

    // Split into lines. Tabs are expanded to 4-column stops so that columns line up and the
    // horizontal scroll can work in whole columns.
    g_preview.lines.clear();
    std::wstring line;
    int col = 0;
    for (wchar_t ch : text) {
        if (ch == L'\n') {
            g_preview.lines.push_back(line);
            line.clear();
            col = 0;
            if (g_preview.lines.size() >= 2000) break;
        } else if (ch == L'\t') {
            const int n = 4 - (col % 4);
            line.append((size_t)n, L' ');
            col += n;
        } else if (ch != L'\r') {
            line += ch;
            ++col;
        }
    }
    if (!line.empty() && g_preview.lines.size() < 2000) g_preview.lines.push_back(line);
    for (const auto& l : g_preview.lines) g_preview.maxCols = std::max(g_preview.maxCols, (int)l.size());

    g_preview.isText  = true;
    g_preview.loaded  = true;
}

// ===== Preview Panel (Quick Look) =====
// Same card as the actions panel (same slot, same height as the search bar, same slide-in), but
// over a flat background instead of the acrylic / blur effect.

// Geometry (DIP) shared by the drawing code and the keyboard scrolling, so the keys clamp to
// exactly what is on screen.
static const float kPrevHeaderH = 44.f;  // file icon + name
static const float kPrevTopGap = 8.f;    // between the separator and the first line
static const float kPrevFootH = 36.f;    // hint row
static const float kPrevLineH = 15.f;
static const float kPrevHBarH = 8.f;     // reserved for the horizontal scroll bar
static const int kPrevHStep = 4;         // columns moved by Left / Right

struct PreviewTextGeom {
    float charW;     // px per column (fixed-pitch font)
    float numColW;   // px of the line-number column, including the gap to the text
    float textW;     // px available for the text
    int numDigits;
    int visCols;
    int visLines;
    bool hbar;       // a horizontal scroll bar is needed (and its height is reserved)
};

static PreviewTextGeom PreviewGeom() {
    const float s = (float)g_scale;
    PreviewTextGeom g = {};
    g.charW = TextWidth(L"0", g_fmtPreviewCode);
    if (g.charW < 1.f) g.charW = 7.f * s;
    g.numDigits = 2;
    for (int n = (int)g_preview.lines.size(); n >= 100; n /= 10) ++g.numDigits;
    g.numColW = g.numDigits * g.charW + 12.f * s;
    g.textW = (kPanelW - 2.f * kPanelPad) * s - g.numColW;
    g.visCols = std::max(1, (int)std::floor(g.textW / g.charW));
    g.hbar = g_preview.maxCols > g.visCols;
    const float contentH = ((float)DetailsPanelH() - kPrevHeaderH - kPrevTopGap - kPrevFootH) * s;
    g.visLines = std::max(
        1, (int)std::floor((contentH - (g.hbar ? kPrevHBarH * s : 0.f)) / (kPrevLineH * s)));
    return g;
}

static void DrawPreviewPanel(const SearchResult& res) {
    const float s   = (float)g_scale;
    const float a   = (float)std::clamp(g_detailPos, 0.0, 1.0);
    float mainX = 0, mainY = 0;
    MainOrigin(mainX, mainY);

    // Attached to the right edge of the main panel; it slides in a bit when opening.
    const float ox  = mainX + (kWidth + kPanelGap + (1.f - a) * 14.f) * s;
    const float oy  = mainY;
    const float pw  = kPanelW * s;
    const float ph  = (float)DetailsPanelH() * s;   // always the same as the search bar
    const float pad = kPanelPad * s;

    DrawCard(ox, oy, pw, ph, a, /*plain=*/true);

    // Nothing the panel draws can leave its card.
    D2D1_RECT_F panelClip = D2D1::RectF(ox, oy, ox + pw, oy + ph);
    g_rt->PushAxisAlignedClip(&panelClip, D2D1_ANTIALIAS_MODE_ALIASED);

    // Load preview data if needed
    if (!res.payload.empty()) {
        LoadPreview(res.payload);
    }

    // ---- Header: file name ----
    const float headerH = kPrevHeaderH * s;

    // Determine ext for colour / icon
    std::wstring ext;
    {
        auto d = res.payload.rfind(L'.');
        if (d != std::wstring::npos) {
            ext = res.payload.substr(d + 1);
            for (auto& c : ext) c = (wchar_t)towupper(c);
        }
    }
    const D2D1_COLOR_F tileCol = ExtColor(ext);

    // File name only (trimmed with an ellipsis if it does not fit)
    DrawLabel(res.title, g_fmtPanelTitle, ox + pad,
              oy, ox + pw - pad, oy + headerH, g_pal.text, a);

    // Separator
    SetBrush(g_pal.separator, a);
    D2D1_RECT_F sepR = D2D1::RectF(ox + pad, oy + headerH, ox + pw - pad, oy + headerH + 1.f);
    g_rt->FillRectangle(&sepR, g_brush);

    // ---- Content area ----
    const float contentTop = oy + headerH + kPrevTopGap * s;
    const float contentBot = oy + ph - kPrevFootH * s;   // leave room for the footer hints
    const float contentW   = pw - pad * 2.f;

    D2D1_RECT_F clip = D2D1::RectF(ox, contentTop, ox + pw, contentBot);
    g_rt->PushAxisAlignedClip(&clip, D2D1_ANTIALIAS_MODE_ALIASED);

    PreviewTextGeom tg = {};
    if (!g_preview.loaded || g_preview.path != res.payload) {
        // Still loading or path mismatch — show spinner placeholder
        SetBrush(g_pal.textDim, a);
        D2D1_RECT_F r = D2D1::RectF(ox + pad, contentTop + 12 * s, ox + pw - pad, contentBot);
        g_rt->DrawText(L"Loading…", 8, g_fmtPanelRow, &r, g_brush);

    } else if (!g_preview.errorMsg.empty()) {
        // A failed preview is an error, not just missing information: same red as the rest.
        const bool unsupported = g_preview.errorMsg.find(L"not available") != std::wstring::npos;
        SetBrush(unsupported ? g_pal.textDim : kToneError, a);
        D2D1_RECT_F r = D2D1::RectF(ox + pad, contentTop + 12 * s, ox + pw - pad, contentBot);
        g_rt->DrawText(g_preview.errorMsg.c_str(), (UINT32)g_preview.errorMsg.size(),
                       g_fmtPanelRow, &r, g_brush);

    } else if (g_preview.isImage) {
        // ---- Image preview ----
        // Use the thumbnail cache (async loaded by the thumb thread)
        // We need the FileEntry to call EnsureFileThumb — find it via the path
        ID2D1Bitmap* bmp = nullptr;
        for (auto& fe : FileSrc()) {
            if (fe.path == res.payload) {
                bmp = EnsureFileThumb(fe);
                break;
            }
        }
        if (bmp) {
            // Draw the image centered, aspect-fitted inside the content area
            const D2D1_SIZE_F bsz = bmp->GetSize();
            const float availW = contentW;
            const float availH = contentBot - contentTop - 8 * s;
            const float scale  = std::min(availW / bsz.width, availH / bsz.height);
            const float dw     = bsz.width  * scale;
            const float dh     = bsz.height * scale;
            const float dx     = ox + pad + (availW - dw) / 2.f;
            const float dy     = contentTop + 4 * s + (availH - dh) / 2.f;
            D2D1_RECT_F dst    = D2D1::RectF(dx, dy, dx + dw, dy + dh);
            g_rt->DrawBitmap(bmp, &dst, a, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
        } else {
            // "Loading" only while a decode is really queued. A remote file, a failed decode or an
            // unsupported format never arrives, and redrawing every frame to wait for it kept the
            // whole window at full frame rate; WM_APP_THUMB redraws when a decode does finish.
            const bool pending = g_thumbPending.count(res.payload) > 0;
            DrawFileTile(ox + pad + (contentW - 48 * s) / 2.f,
                         contentTop + 12 * s, 48 * s, ext, tileCol, a);
            SetBrush(g_pal.textDim, a);
            const wchar_t* lbl = pending ? L"Loading image…" : L"Preview not available";
            D2D1_RECT_F r = D2D1::RectF(ox + pad, contentTop + 70 * s, ox + pw - pad, contentBot);
            g_rt->DrawText(lbl, (UINT32)wcslen(lbl), g_fmtPanelRow, &r, g_brush);
        }

    } else if (g_preview.isText) {
        // ---- Text / code preview ----
        tg = PreviewGeom();
        const float lineH  = kPrevLineH * s;
        const int total    = (int)g_preview.lines.size();
        g_preview.scroll   = std::clamp(g_preview.scroll, 0, std::max(0, total - tg.visLines));
        const int maxH     = std::max(0, g_preview.maxCols - tg.visCols);
        g_preview.hscroll  = std::clamp(g_preview.hscroll, 0, maxH);

        const float numX  = ox + pad;
        const float textX = numX + tg.numColW;

        if (total == 0) {
            SetBrush(g_pal.textDim, a);
            D2D1_RECT_F r = D2D1::RectF(ox + pad, contentTop + 12 * s, ox + pw - pad, contentBot);
            g_rt->DrawText(L"Empty file.", 11, g_fmtPanelRow, &r, g_brush);
        }

        for (int i = 0; i < tg.visLines && (g_preview.scroll + i) < total; ++i) {
            const int lineIdx = g_preview.scroll + i;
            const float ly    = contentTop + i * lineH;

            // Line number, right-aligned in its column
            wchar_t numBuf[12];
            const int nLen = swprintf_s(numBuf, L"%d", lineIdx + 1);
            SetBrush(g_pal.textDim, a * 0.8f);
            const float nx = numX + (tg.numDigits - std::min(nLen, tg.numDigits)) * tg.charW;
            D2D1_RECT_F nr = D2D1::RectF(nx, ly, numX + tg.numColW, ly + lineH);
            g_rt->DrawText(numBuf, (UINT32)std::max(0, nLen), g_fmtPreviewCode, &nr, g_brush,
                           D2D1_DRAW_TEXT_OPTIONS_CLIP);

            // Line text: only the visible columns, no trimming (the clip cuts the last glyph)
            const std::wstring& ln = g_preview.lines[lineIdx];
            if ((int)ln.size() > g_preview.hscroll) {
                const std::wstring seg = ln.substr((size_t)g_preview.hscroll, (size_t)tg.visCols + 2);
                SetBrush(g_pal.text, a);
                D2D1_RECT_F tr = D2D1::RectF(textX, ly, textX + tg.textW, ly + lineH);
                g_rt->DrawText(seg.c_str(), (UINT32)seg.size(), g_fmtPreviewCode, &tr, g_brush,
                               D2D1_DRAW_TEXT_OPTIONS_CLIP);
            }
        }

        // Vertical scroll bar (if needed)
        if (total > tg.visLines) {
            const float sbX = ox + pw - pad * 0.6f - 3 * s;
            const float sbH = tg.visLines * lineH;
            const float tH  = std::max(12 * s, sbH * tg.visLines / (float)total);
            const float tY  = contentTop + (sbH - tH) * g_preview.scroll /
                                               (float)std::max(1, total - tg.visLines);
            FillRR(sbX, tY, sbX + 3 * s, tY + tH, 1.5f * s, g_pal.textDim, 0.35f * a);
        }
        // Horizontal scroll bar (if needed), under the last line
        if (tg.hbar) {
            const float hbY = contentTop + tg.visLines * lineH + 3 * s;
            const float tW  = std::max(16 * s, tg.textW * tg.visCols / (float)g_preview.maxCols);
            const float tX  = textX + (tg.textW - tW) * g_preview.hscroll / (float)std::max(1, maxH);
            FillRR(tX, hbY, tX + tW, hbY + 3 * s, 1.5f * s, g_pal.textDim, 0.35f * a);
        }
    }

    g_rt->PopAxisAlignedClip();

    // ---- Footer hints, laid out from the right edge inwards so they never leave the panel ----
    if (a > 0.98f) {
        const float hintMid = oy + ph - 18 * s;
        float hx = ox + pw - pad;
        const wchar_t* escKey[] = { L"Esc" };
        hx = DrawHint(L"Close", escKey, 1, hx, hintMid) - 14 * s;
        if (g_preview.isText && g_preview.loaded) {
            if (tg.hbar) {
                const wchar_t* hKeys[] = { L"←", L"→" };
                hx = DrawHint(L"Pan", hKeys, 2, hx, hintMid) - 14 * s;
            }
            if ((int)g_preview.lines.size() > tg.visLines) {
                const wchar_t* vKeys[] = { L"↑", L"↓" };
                DrawHint(L"Scroll", vKeys, 2, hx, hintMid);
            }
        }
    }

    g_rt->PopAxisAlignedClip();
}

static void Render() {
    if (!g_dib || !g_dw || !EnsureTarget()) {
        return;
    }
    const float s = (float)g_scale;

    // Panel mode switching: when g_panelSwitching is true the panel is animating closed;
    // once it reaches near-zero we flip the mode and reopen.
    if (g_panelSwitching && g_detailPos < 0.05) {
        g_panelSwitching = false;
        g_panelMode      = g_panelPendingMode;
        g_detailsOpen    = true;
        Sound::OnPanel(true);
        UpdateTargets(false, false);
    }

    // Details panel data (computed before drawing starts). If there is no longer a
    // selection (e.g. the text was deleted) the panel keeps showing the last app while it
    // fades out.
    AppEntry* panelApp = nullptr;
    const AppDetails* panelDetails = nullptr;
    const SearchResult* panelFileRes = nullptr;
    const SearchResult* panelWinRes = nullptr;
    if (g_detailPos > 0.01) {
        if (g_panelMode == PanelMode::Actions) {
            if (g_sel >= 0 && g_sel < (int)g_results.size() &&
                g_results[g_sel].kind == ResultKind::Window) {
                panelWinRes = &g_results[g_sel];
                if (g_winPanel.hwnd != panelWinRes->hwnd) FillWinPanel(*panelWinRes);
                g_winPanel.valid = true;
            } else if (g_sel >= 0 && g_sel < (int)g_results.size() &&
                       g_results[g_sel].kind == ResultKind::App) {
                g_winPanel.valid = false;  // a window card must not fade out over an app
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
        } else {
            // Preview mode: use currently selected file result
            if (g_sel >= 0 && g_sel < (int)g_results.size() &&
                g_results[g_sel].kind == ResultKind::File) {
                panelFileRes = &g_results[g_sel];
            }
        }
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
    const float rightX = cx + cw - 24 * s;
    // With a mode active the bar stays compact and no footer is shown, so keep a small
    // "Enter Search" hint inside the bar itself.
    const bool modeHint = ModeIsWeb() && !g_results.empty();
    const wchar_t* const enterKey[1] = {kTxtEnterGlyph};
    const float hintReserve = modeHint ? HintWidth(kTxtSearchLabel, enterKey, 1) + 8 * s : 0.f;
    // The Apps/Files toggle only makes sense outside a typed web/command mode. When the file
    // browser is disabled the button is hidden (but still drawn while leaving the mode).
    const bool showToggle = (g_activator < 0 && (g_cfg.filesEnabled || g_fileMode));
    const float toggleReserve = showToggle ? 44 * s : 0.f;
    float fieldX = cx + 58 * s;
    if (g_activator >= 0) {
        // Activator pill: rounded tag with the name of the active mode.
        const std::wstring name = kActivators[g_activator].name;
        const float pillH = 26 * s;
        const float pillW = TextWidth(name.c_str(), g_fmtKey) + 20 * s;
        const float pillX = cx + 16 * s;
        FillRR(pillX, barMid - pillH / 2, pillX + pillW, barMid + pillH / 2, pillH / 2,
               g_pal.accent, 0.15f);
        DrawLabel(name, g_fmtKey, pillX, barMid - pillH / 2, pillX + pillW, barMid + pillH / 2,
                  g_pal.accentInk, 1.f);
        fieldX = pillX + pillW + 12 * s;
    } else if (g_fileMode) {
        // Files chip: folder icon + label + a close button that leaves file mode.
        const D2D1_RECT_F cr = FileModeChipRect(cx, cy);
        FillRR(cr.left, cr.top, cr.right, cr.bottom, 13 * s, g_pal.accent, 0.15f);
        DrawFolderOutline(cr.left + 10 * s, (cr.top + cr.bottom) / 2 - 8 * s, 16 * s,
                          g_pal.accentInk);
        const float labelX = cr.left + 34 * s;  // 10 + 16 + 8
        const float labelW = TextWidth(kTxtFilesMode, g_fmtKeyBold);
        DrawLabel(kTxtFilesMode, g_fmtKeyBold, labelX, cr.top, labelX + labelW, cr.bottom,
                  g_pal.accentStrong, 1.f);
        const D2D1_RECT_F xr = FileModeChipCloseRect(cx, cy);
        DrawCommandGlyph(CmdGlyph::Close, xr.left, xr.top, xr.right - xr.left, g_pal.accentInk,
                         g_pal.accentInk);
        fieldX = cr.right + 12 * s;
    } else {
        DrawMagnifier(cx + 30 * s, barMid);
    }
    const float fieldW = rightX - toggleReserve - hintReserve - fieldX;
    {
        D2D1_RECT_F clip = D2D1::RectF(fieldX, cy, fieldX + fieldW, cy + kBarH * s);
        g_rt->PushAxisAlignedClip(&clip, D2D1_ANTIALIAS_MODE_ALIASED);

        if (g_text.empty()) {
            SetBrush(g_pal.textDim, 0.85f);
            D2D1_RECT_F r = D2D1::RectF(fieldX, cy, fieldX + fieldW, cy + kBarH * s);
            const wchar_t* ph =
                g_helpOpen ? (g_helpPage == 0 ? L"Keyboard shortcuts and tips"
                                              : L"Calculator syntax and examples")
                : g_fileMode ? kTxtPlaceholderFiles
                             : kTxtPlaceholder;
            g_rt->DrawText(ph, (UINT32)wcslen(ph), g_fmtInput, &r, g_brush);
            if (g_caretOn && !g_helpOpen && !g_form.active && g_bandEdit == BandEdit::None &&
                g_vis != Vis::Hiding) {
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

                if (g_caretOn && !g_helpOpen && !g_form.active && g_bandEdit == BandEdit::None &&
                    g_vis != Vis::Hiding) {
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

    // Apps/Files toggle: an outline folder at the right end of the bar.
    if (showToggle) {
        const D2D1_RECT_F tr = FileToggleRect(cx, cy);
        const float gs = (tr.right - tr.left) * 0.78f;
        const D2D1_COLOR_F fc = g_fileMode ? g_pal.accentInk : g_pal.textDim;
        DrawFolderOutline(tr.left + (tr.right - tr.left - gs) / 2,
                          (tr.top + tr.bottom) / 2 - gs / 2, gs, fc);
    }

    if (modeHint) {
        DrawHint(kTxtSearchLabel, enterKey, 1, rightX, barMid);
    }

    // ---- Help sheet: replaces the list, footer and details panel ----
    if (g_helpOpen) {
        DrawHelp(cx, cy, cw, ch, s);
        const HRESULT hrHelp = g_rt->EndDraw();
        if (hrHelp == D2DERR_RECREATE_TARGET) {
            ReleaseTarget();
            g_dirty = true;
        }
        return;
    }

    // ---- Custom entry form: replaces the list, footer and details panel ----
    if (g_form.active) {
        const D2D1_RECT_F cardClip = D2D1::RectF(cx, cy, cx + cw, cy + ch);
        g_rt->PushAxisAlignedClip(&cardClip, D2D1_ANTIALIAS_MODE_ALIASED);
        DrawCustomForm(cx, cy, cw, s);
        const float formFooterTop = cy + (float)(ExpandedH() - kFooterH) * s;
        if (cy + ch > formFooterTop + 2.f) {
            D2D1_RECT_F footClip = D2D1::RectF(cx, formFooterTop, cx + cw, cy + ch);
            g_rt->PushAxisAlignedClip(&footClip, D2D1_ANTIALIAS_MODE_ALIASED);
            DrawCustomFormFooter(cx, formFooterTop, cw);
            g_rt->PopAxisAlignedClip();
        }
        g_rt->PopAxisAlignedClip();
        const HRESULT hrForm = g_rt->EndDraw();
        if (hrForm == D2DERR_RECREATE_TARGET) {
            ReleaseTarget();
            g_dirty = true;
        }
        return;
    }

    // ---- Custom-entries manager: replaces the list, footer and details panel ----
    if (g_mgr.active) {
        const D2D1_RECT_F cardClip = D2D1::RectF(cx, cy, cx + cw, cy + ch);
        g_rt->PushAxisAlignedClip(&cardClip, D2D1_ANTIALIAS_MODE_ALIASED);
        DrawManager(cx, cy, cw, s);
        const float mgrFooterTop = cy + (float)(ExpandedH() - kFooterH) * s;
        if (cy + ch > mgrFooterTop + 2.f) {
            D2D1_RECT_F footClip = D2D1::RectF(cx, mgrFooterTop, cx + cw, cy + ch);
            g_rt->PushAxisAlignedClip(&footClip, D2D1_ANTIALIAS_MODE_ALIASED);
            DrawManagerFooter(cx, mgrFooterTop, cw);
            g_rt->PopAxisAlignedClip();
        }
        g_rt->PopAxisAlignedClip();
        const HRESULT hrMgr = g_rt->EndDraw();
        if (hrMgr == D2DERR_RECREATE_TARGET) {
            ReleaseTarget();
            g_dirty = true;
        }
        return;
    }

    // ---- List / messages ----
    const float listTop = cy + kBarH * s;
    const int rows = VisibleRows();

    if (ch > kBarH * s + 2) {
        SetBrush(g_pal.separator);
        D2D1_RECT_F sep = D2D1::RectF(cx + 12 * s, listTop, cx + cw - 12 * s, listTop + 1.f);
        g_rt->FillRectangle(&sep, g_brush);
    }

    // File-mode filter band: location chips on the first row, drive chips on a second row below
    // them (kept apart so the two groups do not read as one list). The highlight slides between
    // the active chip and its neighbour, across rows when needed (animated via g_chipSelPos).
    if (g_fileMode && !g_chips.empty()) {
        const int nc = std::min((int)g_chips.size(), 6 + 26);  // profile locations + A..Z
        D2D1_RECT_F rects[6 + 26];
        for (int i = 0; i < nc; ++i) rects[i] = FileChipRect(cx, cy, i);
        // Keep the chips inside the card: a long row is clipped on the right rather than drawn
        // over the rounded edge.
        const D2D1_RECT_F chipClip = D2D1::RectF(cx + 10 * s, listTop + 1.f, cx + cw - 10 * s,
                                                 listTop + FileChipRowsH() * s);
        g_rt->PushAxisAlignedClip(&chipClip, D2D1_ANTIALIAS_MODE_ALIASED);
        const double pos = std::clamp(g_chipSelPos, 0.0, (double)(nc - 1));
        const int a = (int)pos;
        const int b = std::min(a + 1, nc - 1);
        const float tl = (float)(pos - a);
        const float aw = rects[a].right - rects[a].left, bw = rects[b].right - rects[b].left;
        const float hx = rects[a].left + (rects[b].left - rects[a].left) * tl;
        const float hy = rects[a].top + (rects[b].top - rects[a].top) * tl;
        const float hw = aw + (bw - aw) * tl;
        if (g_fileLoc >= 0 && g_fileLoc < nc) {
            FillRR(hx, hy, hx + hw, hy + (rects[a].bottom - rects[a].top), 14 * s, g_pal.accent,
                   0.18f);
        }
        for (int i = 0; i < nc; ++i) {
            const bool on = (i == g_fileLoc);
            const D2D1_COLOR_F fg = on ? g_pal.accentInk : g_pal.textDim;
            const D2D1_COLOR_F lc = on ? g_pal.accentStrong : g_pal.textDim;
            IDWriteTextFormat* lfmt = on ? g_fmtKeyBold : g_fmtKey;
            const D2D1_RECT_F& r = rects[i];
            const float cmid = (r.top + r.bottom) / 2;
            DrawCommandGlyph(g_chips[i].glyph, r.left + 9 * s, cmid - 7 * s, 14 * s, fg, fg);
            DrawLabel(g_chips[i].label.c_str(), lfmt, r.left + 30 * s, r.top, r.right - 9 * s,
                      r.bottom, lc, 1.f);
        }
        g_rt->PopAxisAlignedClip();
        DrawPathBand(cx, cy, cw);
    }

    // Calculator result card, in the space reserved above the list.
    if (ch > kBarH * s + 2 && HeroShown()) {
        DrawCalcHero(cx, listTop, cw);
    }

    // The footer takes the end of the expanded height; the list ends just above it.
    const float footerTop = cy + (float)(ExpandedH() - kFooterH) * s;
    const float listBottom = std::min(cy + ch, footerTop - kListPad * s);
    D2D1_RECT_F listClip =
        D2D1::RectF(cx, listTop + (float)FileChipsH() * s + 1.f, cx + cw, listBottom);
    g_rt->PushAxisAlignedClip(&listClip, D2D1_ANTIALIAS_MODE_ALIASED);

    if (rows > 0) {
        const float rowsOrigin = listTop + (kListPad + (float)HeroH() + (float)FileChipsH()) * s;
        const float rowH = kRowH * s;
        const float rowX0 = cx + 8 * s;
        const float rowX1 = cx + cw - 8 * s;
        const double scrollOff = ScrollPx();

        const float hlY = rowsOrigin + (float)(RowOffset(g_selPos) - scrollOff) * s;
        const bool armedRow =
            g_confirmCmd >= 0 && g_sel >= 0 && g_sel < (int)g_results.size() &&
            g_results[g_sel].kind == ResultKind::Command && g_results[g_sel].icon == g_confirmCmd;
        const D2D1_COLOR_F hlCol =
            armedRow ? D2D1::ColorF(0.93f, 0.30f, 0.30f, 0.26f) : g_pal.selection;
        FillRR(rowX0, hlY, rowX1, hlY + rowH, kRowRadius * s, hlCol);
        {
            SetBrush(armedRow ? D2D1::ColorF(0.93f, 0.30f, 0.30f) : g_pal.text, armedRow ? 0.5f : 0.06f);
            D2D1_ROUNDED_RECT so = D2D1::RoundedRect(
                D2D1::RectF(rowX0 + 0.5f, hlY + 0.5f, rowX1 - 0.5f, hlY + rowH - 0.5f), kRowRadius * s,
                kRowRadius * s);
            g_rt->DrawRoundedRectangle(&so, g_brush, 1.f);
        }

        const float iconPx = kIconSize * s;
        const float textX = rowX0 + (10 + kIconSize + 14) * s;
        const float textW = (rowX1 - textX) - (g_fileMode ? 26 * s : 130 * s);

        int first = std::max(0, (int)std::floor(g_scrollPos) - (g_kbScroll ? 2 : 0));
        int last = std::min((int)g_results.size() - 1, (int)std::ceil(g_scrollPos) + rows);
        for (int i = first; i <= last; ++i) {
            SearchResult& res = g_results[i];
            AppEntry* app = (res.kind == ResultKind::App) ? &g_apps[res.app] : nullptr;
            float y = rowsOrigin + (float)(RowOffset((double)i) - scrollOff) * s;

            // Section header ("Pinned" / "Recent" / "Recent files" / "Suggestions") above the first
            // row of each group
            if (res.section != 0 && (i == 0 || g_results[i - 1].section != res.section)) {
                const wchar_t* secTxt = (res.section == 3)   ? kTxtPinned
                                        : (res.section == 1) ? kTxtRecents
                                        : (res.section == 4) ? kTxtRecentFiles
                                                             : kTxtSuggestions;
                SetBrush(g_pal.textDim);
                D2D1_RECT_F secRect = D2D1::RectF(rowX0 + 10 * s, y - kSectionH * s, rowX1, y);
                g_rt->DrawText(secTxt, (UINT32)wcslen(secTxt), g_fmtSection, &secRect, g_brush);
            }

            float ix = rowX0 + 10 * s;
            float iy = y + (rowH - iconPx) / 2;
            ID2D1Bitmap* bmp = app ? EnsureIcon(*app) : nullptr;
            if (!bmp && res.kind == ResultKind::Window && res.iconApp >= 0 &&
                res.iconApp < (int)g_apps.size()) {
                bmp = EnsureIcon(g_apps[res.iconApp]);
            }
            if (!bmp && res.kind == ResultKind::Window) {
                bmp = EnsureWindowIcon(res.payload);  // portables and packaged apps land here
            }
            if (bmp) {
                D2D1_RECT_F dst = D2D1::RectF(ix, iy, ix + iconPx, iy + iconPx);
                g_rt->DrawBitmap(bmp, &dst, 1.f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
            } else if (res.kind == ResultKind::Web) {
                // Engine badge: brand colour square with the engine's initial.
                FillRR(ix, iy, ix + iconPx, iy + iconPx, 9 * s,
                       WebEngineColor((WebEngine)res.icon));
                const wchar_t letter[2] = {res.subtitle.empty() ? L'?' : res.subtitle[0], 0};
                SetBrush(D2D1::ColorF(1.f, 1.f, 1.f));
                D2D1_RECT_F lr = D2D1::RectF(ix, iy, ix + iconPx, iy + iconPx);
                g_rt->DrawText(letter, 1, g_fmtBadge, &lr, g_brush);
            } else if (res.kind == ResultKind::Url) {
                // Direct link: a link-blue square with a small globe.
                FillRR(ix, iy, ix + iconPx, iy + iconPx, 9 * s,
                       D2D1::ColorF(0.20f, 0.55f, 0.90f));
                const float cx = ix + iconPx * 0.5f, cy = iy + iconPx * 0.5f;
                const float r = iconPx * 0.26f;
                SetBrush(D2D1::ColorF(1.f, 1.f, 1.f));
                g_rt->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(cx, cy), r, r), g_brush, 1.6f * s);
                g_rt->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(cx, cy), r * 0.42f, r), g_brush,
                                  1.6f * s);
                g_rt->DrawLine(D2D1::Point2F(cx - r, cy), D2D1::Point2F(cx + r, cy), g_brush,
                               1.6f * s);
            } else if (res.kind == ResultKind::Command) {
                // Command badge: a coloured tile with the command's own glyph.
                const int ci = (res.icon >= 0 && res.icon < kCommandCount) ? res.icon : -1;
                const CmdGlyph cg = ci >= 0 ? kCommands[ci].glyph : CmdGlyph::Terminal;
                const D2D1_COLOR_F tint =
                    ci >= 0 ? kCommands[ci].color : D2D1::ColorF(0.33f, 0.34f, 0.42f);
                FillRR(ix, iy, ix + iconPx, iy + iconPx, 9 * s, tint);
                DrawCommandGlyph(cg, ix, iy, iconPx, tint);
            } else if (res.kind == ResultKind::Setting) {
                // Settings badge: same tile style, with the settings page's own glyph.
                const int si = (res.icon >= 0 && res.icon < kSettingCount) ? res.icon : -1;
                const CmdGlyph sg = si >= 0 ? kSettings[si].glyph : CmdGlyph::Gear;
                const D2D1_COLOR_F st =
                    si >= 0 ? kSettings[si].color : D2D1::ColorF(0.33f, 0.34f, 0.42f);
                FillRR(ix, iy, ix + iconPx, iy + iconPx, 9 * s, st);
                DrawCommandGlyph(sg, ix, iy, iconPx, st);
            } else if (res.kind == ResultKind::Custom) {
                // User entry: a coloured tile with a glyph for its type.
                const CustomEntry* ce =
                    (res.custom >= 0 && res.custom < (int)g_cfg.commands.size())
                        ? &g_cfg.commands[res.custom]
                        : nullptr;
                const bool isUrl = ce && ce->type == L"url";
                const bool isRun = ce && ce->type == L"run";
                const D2D1_COLOR_F ct = isUrl ? D2D1::ColorF(0.20f, 0.55f, 0.90f)
                                        : isRun ? D2D1::ColorF(0.55f, 0.38f, 0.88f)
                                                : D2D1::ColorF(0.92f, 0.62f, 0.22f);
                FillRR(ix, iy, ix + iconPx, iy + iconPx, 9 * s, ct);
                DrawCommandGlyph(isUrl ? CmdGlyph::Link
                                       : isRun ? CmdGlyph::Terminal : CmdGlyph::Folder,
                                 ix, iy, iconPx, ct);
            } else if (res.kind == ResultKind::Bookmark) {
                // Bookmark badge: a star tile, so it reads as "saved link" at a glance.
                const D2D1_COLOR_F bt = D2D1::ColorF(0.92f, 0.66f, 0.20f);
                FillRR(ix, iy, ix + iconPx, iy + iconPx, 9 * s, bt);
                DrawCommandGlyph(CmdGlyph::Star, ix, iy, iconPx, bt);
            } else if (res.kind == ResultKind::Clip) {
                // An image shows its own thumbnail (decoded off the UI thread like a file preview);
                // text gets a green tile with a clipboard glyph. A pinned entry gets a small pin
                // badge over its top-right corner.
                ID2D1Bitmap* thumb = res.image ? ClipImageThumb(res.payload) : nullptr;
                if (thumb) {
                    DrawThumbImage(thumb, ix, iy, iconPx, 9 * s);
                } else {
                    const D2D1_COLOR_F ct = D2D1::ColorF(0.30f, 0.62f, 0.55f);
                    FillRR(ix, iy, ix + iconPx, iy + iconPx, 9 * s, ct);
                    DrawCommandGlyph(CmdGlyph::Clipboard, ix, iy, iconPx, ct);
                }
                if (res.pinned) {
                    const float bs = iconPx * 0.46f;
                    const float bx = ix + iconPx - bs * 0.55f;
                    const float by = iy - bs * 0.45f;
                    const D2D1_COLOR_F gold = D2D1::ColorF(0.98f, 0.78f, 0.30f);
                    FillRR(bx, by, bx + bs, by + bs, bs * 0.32f, gold);
                    DrawCommandGlyph(CmdGlyph::Pin, bx, by, bs, gold,
                                     D2D1::ColorF(0.12f, 0.12f, 0.14f));
                }
            } else if (IsPathResult(res.kind)) {
                // Recent/global file: folder outline or the same type tile as the file browser.
                if (res.isDir) {
                    DrawFolderOutline(ix + 1 * s, iy + 1 * s, iconPx - 2 * s,
                                      D2D1::ColorF(0.90f, 0.68f, 0.28f));
                } else {
                    DrawFileTile(ix, iy, iconPx, res.ext, ExtColor(res.ext));
                }
            } else if (res.kind == ResultKind::Window) {
                // Window without a usable icon: the same initial tile the panel shows.
                DrawWindowTile(ix, iy, iconPx, 9 * s,
                               res.subtitle.empty() ? res.title : res.subtitle);
            } else if (res.kind == ResultKind::File) {
                // Folders: outline folder. Files: image preview, else a type tile.
                const int fi = (res.icon >= 0 && res.icon < (int)FileSrc().size()) ? res.icon : -1;
                if (fi >= 0 && FileSrc()[fi].isDir) {
                    DrawFolderOutline(ix + 1 * s, iy + 1 * s, iconPx - 2 * s,
                                      D2D1::ColorF(0.90f, 0.68f, 0.28f));
                } else if (fi >= 0) {
                    FileEntry& fe = FileSrc()[fi];
                    ID2D1Bitmap* th = EnsureFileThumb(fe);
                    if (th) {
                        DrawThumbImage(th, ix, iy, iconPx, iconPx * 0.22f);
                    } else {
                        DrawFileTile(ix, iy, iconPx, fe.ext, ExtColor(fe.ext));
                    }
                } else {
                    DrawFolderOutline(ix + 1 * s, iy + 1 * s, iconPx - 2 * s,
                                      D2D1::ColorF(0.60f, 0.63f, 0.70f));
                }
            } else {
                FillRR(ix + 2 * s, iy + 2 * s, ix + iconPx - 2 * s, iy + iconPx - 2 * s, 8 * s,
                       g_pal.textDim, 0.18f);
            }

            // A window kept on top wears a pin badge, like a pinned entry: the state is visible
            // without opening the panel, so a forgotten toggle is easy to spot.
            if (res.kind == ResultKind::Window && res.onTop) {
                const float bs = iconPx * 0.46f;
                const float bx = ix + iconPx - bs * 0.55f;
                const float by = iy - bs * 0.45f;
                const D2D1_COLOR_F gold = D2D1::ColorF(0.98f, 0.78f, 0.30f);
                FillRR(bx, by, bx + bs, by + bs, bs * 0.32f, gold);
                DrawCommandGlyph(CmdGlyph::Pin, bx, by, bs, gold,
                                 D2D1::ColorF(0.12f, 0.12f, 0.14f));
            }

            if (res.kind == ResultKind::Bookmark) {
                // Two lines: the name on top, its folder and browser below.
                if (!res.layout) {
                    res.layout =
                        BuildMatchLayout(res.title, FoldString(res.title), textW, rowH * 0.5f);
                }
                if (res.layout) {
                    SetBrush(g_pal.text);
                    g_rt->DrawTextLayout(D2D1::Point2F(textX, y + 3 * s), res.layout, g_brush);
                }
                if (!res.subtitle.empty()) {
                    DrawLabel(res.subtitle, g_fmtMeta, textX, y + rowH * 0.5f, textX + textW,
                              y + rowH - 3 * s, g_pal.textDim, 0.9f);
                }
            } else if (res.kind == ResultKind::File) {
                // Two lines: the name on top, its metadata (type · size · date · folder) below.
                if (!res.layout) {
                    const int fi2 = res.icon;
                    const std::wstring& folded = (fi2 >= 0 && fi2 < (int)FileSrc().size())
                                                     ? FileSrc()[fi2].folded
                                                     : res.title;
                    res.layout = BuildMatchLayout(res.title, folded, textW, rowH * 0.5f);
                }
                if (res.layout) {
                    SetBrush(g_pal.text);
                    g_rt->DrawTextLayout(D2D1::Point2F(textX, y + 3 * s), res.layout, g_brush);
                }
                if (!res.subtitle.empty()) {
                    const float my0 = y + rowH * 0.5f, my1 = y + rowH - 3 * s;
                    const float mw = std::min(TextWidth(res.subtitle.c_str(), g_fmtMeta), textW);
                    DrawLabel(res.subtitle, g_fmtMeta, textX, my0, textX + mw, my1, g_pal.textDim, 0.9f);
                    const int mi = res.icon;
                    const float sepW = TextWidth(L" · ", g_fmtMeta);
                    if (mi >= 0 && mi < (int)FileSrc().size() && textW - mw - sepW > 40 * s) {
                        std::wstring mh, mt;
                        FitPathTail(FileSrc()[mi].dir, g_fmtMeta, g_fmtMeta, textW - mw - sepW, mh, mt);
                        const std::wstring rest = L" · " + mh + mt;
                        DrawLabel(rest, g_fmtMeta, textX + mw, my0, textX + textW, my1, g_pal.textDim, 0.9f);
                    }
                }
            } else if (IsPathResult(res.kind)) {
                // Two lines: the file name and the folder it lives in.
                if (!res.layout) {
                    res.layout = BuildMatchLayout(res.title, FoldString(res.title), textW, rowH * 0.5f);
                }
                if (res.layout) {
                    SetBrush(g_pal.text);
                    g_rt->DrawTextLayout(D2D1::Point2F(textX, y + 3 * s), res.layout, g_brush);
                }
                if (!res.subtitle.empty()) {
                    DrawLabel(res.subtitle, g_fmtMeta, textX, y + rowH * 0.5f, textX + textW,
                              y + rowH - 3 * s, g_pal.textDim, 0.9f);
                }
            } else if (res.kind == ResultKind::Window) {
                // Two lines: the window title and its process name.
                if (!res.layout) {
                    res.layout = BuildMatchLayout(res.title, FoldString(res.title), textW, rowH * 0.5f);
                }
                if (res.layout) {
                    SetBrush(g_pal.text);
                    g_rt->DrawTextLayout(D2D1::Point2F(textX, y + 3 * s), res.layout, g_brush);
                }
                if (!res.subtitle.empty()) {
                    DrawLabel(res.subtitle, g_fmtMeta, textX, y + rowH * 0.5f, textX + textW,
                              y + rowH - 3 * s, g_pal.textDim, 0.9f);
                }
            } else {
                if (!res.layout) {
                    if (app) {
                        res.layout = BuildTitleLayout(*app, textW, rowH);
                    } else {
                        res.layout = BuildMatchLayout(res.title, FoldString(res.title), textW, rowH);
                    }
                }
                if (res.layout) {
                    SetBrush(g_pal.text);
                    g_rt->DrawTextLayout(D2D1::Point2F(textX, y), res.layout, g_brush);
                }
            }

            // Right-hand label: an "open externally" icon for files, a type label otherwise.
            if (res.kind == ResultKind::File && !app) {
                const float es = 16 * s;
                const int gi = res.icon;
                const bool gDir = gi >= 0 && gi < (int)FileSrc().size() && FileSrc()[gi].isDir;
                DrawCommandGlyph(gDir ? CmdGlyph::ChevronRight : CmdGlyph::ExternalLink, rowX1 - 14 * s - es,
                                 y + (rowH - es) / 2, es, g_pal.textDim, g_pal.textDim);
            } else {
                const wchar_t* typeTxt;
                if (app) {
                    typeTxt = IsPackagedId(app->id) ? kTxtTypeStore : kTxtTypeApp;
                } else if (res.kind == ResultKind::Bookmark) {
                    typeTxt = kTxtKindBookmark;  // the subtitle already shows folder and browser
                } else if (IsPathResult(res.kind)) {
                    typeTxt = res.isDir ? kTxtFolder
                                        : (res.kind == ResultKind::Recent ? kTxtKindRecent
                                                                          : kTxtKindFile);
                } else if (res.kind == ResultKind::Window) {
                    // The badge on the icon says it too; the label makes it unmissable.
                    typeTxt = res.onTop ? L"On top" : kTxtKindWindow;
                } else {
                    typeTxt = res.subtitle.empty() ? ResultTypeLabel(res.kind) : res.subtitle.c_str();
                }
                SetBrush(g_pal.textDim);
                D2D1_RECT_F tr = D2D1::RectF(rowX1 - 150 * s, y, rowX1 - 14 * s, y + rowH);
                g_rt->DrawText(typeTxt, (UINT32)wcslen(typeTxt), g_fmtHint, &tr, g_brush);
            }
        }
    } else if (g_calcActive) {
        DrawCalcHistory(cx, listTop, cw);
    } else if (ch > kBarH * s + 8) {
        const wchar_t* msg = nullptr;
        if (g_activator >= 0 && kActivators[g_activator].mode == LauncherMode::WindowList) {
            msg = g_tokens.empty() ? L"No open windows" : kTxtNoResults;
        } else if (g_activator >= 0 && kActivators[g_activator].mode == LauncherMode::GlobalFiles) {
            if (!GlobalModeActive()) {
                msg = L"Type to search your folders…";
            } else if (g_globalSearching) {
                msg = kTxtSearching;
            } else if (g_results.empty()) {
                msg = kTxtNoResults;
            }
        } else if (g_fileMode && g_fileSearching && FileQueryActive()) {
            msg = kTxtSearching;
        } else if ((!g_tokens.empty() || (g_fileMode && g_fileFilter.Active())) && !CalcShown()) {
            msg = kTxtNoResults;
        } else if (g_apps.empty() && !g_indexedOnce && !CalcShown()) {
            msg = kTxtIndexing;
        }
        if (msg) {
            // Same three tones as the footer: the accent while something is still running, amber
            // when a problem explains why the list is empty, dim for plain information.
            D2D1_COLOR_F tone = g_pal.textDim;
            if (msg == kTxtSearching || msg == kTxtIndexing) {
                tone = g_pal.accentInk;
            } else if (msg == kTxtNoResults &&
                       g_cfg.fileSearchEngine == FileSearchEngine::Everything &&
                       NowSec() - g_evFailedAt < 6.0) {
                tone = kToneWarn;
            }
            SetBrush(tone);
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
        DrawFooter(cx, footerTop, cw,
                   (g_sel >= 0 && g_sel < (int)g_results.size()) ? g_sel : -1);
        g_rt->PopAxisAlignedClip();
    }

    if (g_fileMenuOpen && g_fileMode) DrawFileMenu(cx, cy, cw);

    // ---- Details / preview panel (on the right) ----
    if (panelApp && panelDetails) {
        DrawDetailsPanel(*panelApp, *panelDetails);
    } else if (panelWinRes && g_panelMode == PanelMode::Actions) {
        DrawWindowPanel(g_winPanel);
    } else if (panelFileRes) {
        DrawPreviewPanel(*panelFileRes);
    } else if (g_panelMode == PanelMode::Actions && g_winPanel.valid && g_detailPos > 0.01) {
        DrawWindowPanel(g_winPanel);  // the window went away: slide the card out with its data
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
    KillTimer(g_hwnd, TIMER_HIDE);
    CancelFileSearch();   // no point scanning the disk while the bar is hidden
    CancelGlobalSearch(); // nor while the "find:" list is not visible
    CancelThumbQueue();   // nor decoding previews nobody is looking at
    g_calcPending = false;
    g_helpOpen = false;  // next open starts on the list, not on the cheat sheet
    g_form = CustomFormState();  // and not inside a custom entry form
    g_mgr = ManagerState();      // nor inside the custom-entries manager
    ShowWindow(g_hwnd, SW_HIDE);
    LiveHide();
    g_vis = Vis::Hidden;
    g_lastHide = NowSec();
    ReleaseBackdrop();
    ReleaseResultLayouts();
    Sound::Suspend();
    MaybeReindexInBackground();  // changes detected while it was open
}

// A shell operation (delete, mostly) may steal the focus. While it runs, and for a short
// moment after it, losing the foreground must not close the bar: the user keeps their folder.
static bool IgnoreFocusLoss() {
    return g_opGuard || NowSec() < g_focusGrace;
}

static void HideLauncher() {
    if (g_vis == Vis::Hidden || g_vis == Vis::Hiding) {
        return;
    }
    g_vis = Vis::Hiding;
    g_visStart = NowSec();
    StartFrameTimer();
    // Without "Remember file sort" the chosen order only lives while the bar is open: closing it
    // puts Files back to Name A-Z. The listing is rebuilt on the next open, so nothing to re-sort.
    if (!g_cfg.rememberFileSort) g_cfg.fileSort = FileSort::NameAsc;
    // Safety net: if the animation is ever interrupted (e.g. the frame timer is lost while the
    // shell runs a modal loop), finish the hide so no acrylic layer is left on screen.
    if (g_hwnd) SetTimer(g_hwnd, TIMER_HIDE, 600, nullptr);
}

static void ShowLauncher() {
    if (!g_hwnd) {
        return;
    }
    HMONITOR mon = nullptr;
    if (g_cfg.openMonitor == OpenMonitor::Primary) {
        mon = MonitorFromPoint(POINT{0, 0}, MONITOR_DEFAULTTOPRIMARY);
    } else if (g_cfg.openMonitor == OpenMonitor::Foreground) {
        HWND fg = GetForegroundWindow();
        mon = (fg && fg != g_hwnd) ? MonitorFromWindow(fg, MONITOR_DEFAULTTOPRIMARY) : nullptr;
    }
    if (!mon) {
        POINT pt = {};
        GetCursorPos(&pt);
        mon = MonitorFromPoint(pt, MONITOR_DEFAULTTOPRIMARY);
    }
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
    g_fileCopiedAt = -1e9;
    g_activator = -1;
    g_confirmCmd = -1;
    g_fileDir.clear();  // refresh the file listing next time file mode is shown
    g_fileMode = false;  // file mode never persists across opens
    g_fileLoc = 0;       // and neither does the selected location
    g_fileCur.clear();
    g_fileBack.clear();
    g_fileFwd.clear();
    g_pendingSelect.clear();
    g_bandEdit = BandEdit::None;
    g_fileMenuOpen = false;
    ClearFileDeep();
    CancelFileSearch();
    CancelGlobalSearch();
    g_fileSrc = &g_fileEntries;
    g_chipSelPos = g_chipSelTarget = 0;
    g_calc = Calc::Result();
    g_calcDisplay.clear();
    g_calcActive = false;
    g_calcPending = false;
    g_calcHistSel = -1;
    g_histScroll = 0;
    if (g_hwnd) KillTimer(g_hwnd, TIMER_COPY);
    if (g_hwnd) KillTimer(g_hwnd, TIMER_CALC);
    if (g_hwnd) KillTimer(g_hwnd, TIMER_HIDE);  // a pending safety hide must not fire now
    g_lastMouse = {-1, -1};
    g_sel = 0;
    g_scrollTarget = 0;
    g_detailsOpen = false;  // the details panel starts closed on each open
    g_detailPos = g_detailTarget = 0;
    g_panelMode = PanelMode::Actions;
    g_panelSwitching = false;
    g_preview = PreviewState{};
    g_helpOpen = false;
    g_form = CustomFormState();  // no form is left open from the last time
    g_mgr = ManagerState();      // nor the custom-entries manager
    g_hoverAction = -1;
    g_hoverHelp = false;
    g_hoverReindex = false;
    RefreshBookmarksIfStale();  // a bookmark added since the last open shows up now
    RefreshRecentIfStale();     // same for Windows' Recent files
    g_windowsDirty = true;      // a window opened/closed since the last time
    if (g_cfg.clipboardHistory && PurgeExpiredClips()) {
        SaveClipHistory();  // entries past the retention setting go away with their files
    }
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
    const double expandedCardH = (kExpandedMinH + kPathH) * g_scale;
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
    // Remember who had the focus so "clip:" can paste back into it.
    {
        HWND prev = GetForegroundWindow();
        if (prev && prev != g_hwnd) g_prevForeground = prev;
    }
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
        if (Smooth(g_chipSelPos, g_chipSelTarget, 22.0, dt, 0.002)) { active = true; g_dirty = true; }
        if (Smooth(g_scrollPos, g_scrollTarget, 22.0, dt, 0.002)) { active = true; g_dirty = true; }
        if (Smooth(g_curH, g_targetH, 20.0, dt, 0.3)) { active = true; g_dirty = true; }
        if (Smooth(g_detailPos, g_detailTarget, 16.0, dt, 0.003)) { active = true; g_dirty = true; }
        if (g_indexing || g_fileSearching) { active = true; g_dirty = true; }  // keeps the footer icon spinning
        if (!g_bandError.empty()) {
            if (NowSec() - g_bandErrorAt > 2.2) g_bandError.clear();
            active = true;
            g_dirty = true;
        }
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

// ===== Open-window list and activation =====
static std::wstring WindowProcessPath(HWND hwnd) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (!pid) return std::wstring();
    HANDLE p = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!p) return std::wstring();
    wchar_t buf[32768];
    DWORD n = (DWORD)ARRAYSIZE(buf);
    std::wstring path;
    if (QueryFullProcessImageNameW(p, 0, buf, &n)) path.assign(buf, n);
    CloseHandle(p);
    return path;
}

static bool WindowCloaked(HWND hwnd) {
    typedef HRESULT(WINAPI * DwmGetWindowAttribute_t)(HWND, DWORD, PVOID, DWORD);
    static DwmGetWindowAttribute_t fn = [] {
        HMODULE m = LoadLibraryExW(L"dwmapi.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
        return m ? (DwmGetWindowAttribute_t)GetProcAddress(m, "DwmGetWindowAttribute") : nullptr;
    }();
    DWORD cloaked = 0;
    return fn && SUCCEEDED(fn(hwnd, 14 /* DWMWA_CLOAKED */, &cloaked, sizeof(cloaked))) && cloaked;
}

static bool SkipWindowClass(const wchar_t* cls) {
    static const wchar_t* const kSkip[] = {
        L"Progman", L"WorkerW", L"Shell_TrayWnd", L"Shell_SecondaryTrayWnd",
        L"Windows.UI.Core.CoreWindow", L"MultitaskingViewFrame",
    };
    for (const wchar_t* s : kSkip) {
        if (wcscmp(cls, s) == 0) return true;
    }
    return false;
}

static int AppIconForProcess(const std::wstring& path) {
    if (path.empty()) return -1;
    for (size_t i = 0; i < g_apps.size(); ++i) {
        const AppEntry& a = g_apps[i];
        if (!a.target.empty() && SamePath(a.target, path)) return (int)i;
        if (!a.launch.empty() && EndsWith(FoldString(a.launch), L".exe") &&
            SamePath(a.launch, path)) {
            return (int)i;
        }
    }
    return -1;
}

// A hung app must not freeze the launcher while the window list is built.
static std::wstring WindowTitle(HWND hwnd) {
    wchar_t title[512] = L"";
    DWORD_PTR result = 0;
    if (!SendMessageTimeoutW(hwnd, WM_GETTEXT, ARRAYSIZE(title), (LPARAM)title,
                             SMTO_ABORTIFHUNG | SMTO_BLOCK, 50, &result)) {
        return std::wstring();
    }
    return TrimBlanks(title);
}

static BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lp) {
    auto* out = (std::vector<WindowEntry>*)lp;
    if (hwnd == g_hwnd || !IsWindowVisible(hwnd)) return TRUE;
    const LONG_PTR ex = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    if (ex & (WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE)) return TRUE;
    const std::wstring title = WindowTitle(hwnd);
    if (title.empty()) return TRUE;
    if (WindowCloaked(hwnd)) return TRUE;
    wchar_t cls[128] = L"";
    GetClassNameW(hwnd, cls, ARRAYSIZE(cls));
    if (SkipWindowClass(cls)) return TRUE;
    const std::wstring path = WindowProcessPath(hwnd);
    WindowEntry w;
    w.hwnd = hwnd;
    w.title = title;
    w.process = path.empty() ? std::wstring() : FileNameOf(path);
    w.folded = FoldString(w.title + L" " + w.process);
    w.path = path;
    w.appIcon = AppIconForProcess(path);
    out->push_back(std::move(w));
    return out->size() < 200 ? TRUE : FALSE;
}

static void EnsureWindowList() {
    if (!g_windowsDirty) return;
    g_windowsDirty = false;
    g_windows.clear();
    EnumWindows(EnumWindowsProc, (LPARAM)&g_windows);
}

static bool ActivateWindow(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd)) return false;
    if (IsIconic(hwnd)) ShowWindow(hwnd, SW_RESTORE);
    AllowSetForegroundWindow(ASFW_ANY);
    INPUT wake{};
    wake.type = INPUT_MOUSE;
    SendInput(1, &wake, sizeof(INPUT));
    SetForegroundWindow(hwnd);
    BringWindowToTop(hwnd);
    HideLauncher();
    return true;
}

// ===== Window actions for "win:" =====
enum WinAction {
    WA_CLOSE,
    WA_MINIMIZE,
    WA_MAX,
    WA_TOPMOST,
    WA_MON_NEXT,
    WA_MON_PREV,
    WA_LOCATION,
    WA_COPY_PATH
};

// Each helper verifies the window really changed instead of trusting the call: Windows refuses
// cross-process work on elevated windows (UIPI), and that must not look like success.
static bool WinMinimize(HWND hwnd) {
    ShowWindow(hwnd, SW_MINIMIZE);
    return IsIconic(hwnd) != FALSE;
}

static bool WinToggleMax(HWND hwnd) {
    const bool zoomed = IsZoomed(hwnd) != FALSE;
    ShowWindow(hwnd, zoomed ? SW_RESTORE : SW_MAXIMIZE);
    return (IsZoomed(hwnd) != FALSE) != zoomed;
}

static bool WinToggleTopmost(HWND hwnd) {
    const bool top = (GetWindowLongPtrW(hwnd, GWL_EXSTYLE) & WS_EX_TOPMOST) != 0;
    // Only the z-order changes: no move, no resize, no activation.
    return SetWindowPos(hwnd, top ? HWND_NOTOPMOST : HWND_TOPMOST, 0, 0, 0, 0,
                        SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE) != FALSE;
}

static bool WinClose(HWND hwnd) {
    // WM_CLOSE is a request, not a kill: the app decides whether to ask about unsaved work.
    return PostMessageW(hwnd, WM_CLOSE, 0, 0) != FALSE;
}

struct MonitorArea {
    HMONITOR mon;
    RECT work;
};

static BOOL CALLBACK MonitorWorkProc(HMONITOR mon, HDC, LPRECT, LPARAM lp) {
    MONITORINFO mi = {sizeof(mi)};
    if (GetMonitorInfoW(mon, &mi)) ((std::vector<MonitorArea>*)lp)->push_back({mon, mi.rcWork});
    return TRUE;
}

// Walks the monitors left to right, then top to bottom, and keeps the window's size.
// 1 = moved, 0 = Windows refused it, -1 = there is no other monitor in that direction.
static int WinMoveMonitor(HWND hwnd, int dir) {
    std::vector<MonitorArea> mons;
    EnumDisplayMonitors(nullptr, nullptr, MonitorWorkProc, (LPARAM)&mons);
    if (mons.size() < 2) return -1;
    std::sort(mons.begin(), mons.end(), [](const MonitorArea& a, const MonitorArea& b) {
        return a.work.left != b.work.left ? a.work.left < b.work.left : a.work.top < b.work.top;
    });
    const HMONITOR cur = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    int idx = 0;
    for (int i = 0; i < (int)mons.size(); ++i) {
        if (mons[i].mon == cur) {
            idx = i;
            break;
        }
    }
    const int target = std::clamp(idx + dir, 0, (int)mons.size() - 1);
    if (target == idx) return -1;  // already on the last monitor in that direction
    const bool zoomed = IsZoomed(hwnd) != FALSE;
    if (zoomed) ShowWindow(hwnd, SW_RESTORE);
    RECT r = {};
    if (!GetWindowRect(hwnd, &r)) return 0;
    const RECT& w = mons[target].work;
    const int ww = r.right - r.left;
    const int wh = r.bottom - r.top;
    // Centre it in the target monitor, but never push an edge outside.
    const int x = std::clamp((w.left + w.right) / 2 - ww / 2, w.left,
                             std::max(w.left, w.right - ww));
    const int y = std::clamp((w.top + w.bottom) / 2 - wh / 2, w.top,
                             std::max(w.top, w.bottom - wh));
    const bool ok = SetWindowPos(hwnd, nullptr, x, y, 0, 0,
                                 SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE) != FALSE;
    if (zoomed) ShowWindow(hwnd, SW_MAXIMIZE);
    return ok ? 1 : 0;
}

// Snapshot for the panel. Called only when the selected window changes, because it opens the
// process to read its path.
static void FillWinPanel(const SearchResult& r) {
    g_winPanel.valid = true;
    g_winPanel.hwnd = r.hwnd;
    g_winPanel.iconApp = r.iconApp;
    g_winPanel.title = r.title;
    g_winPanel.process = r.subtitle;
    g_winPanel.path = WindowProcessPath(r.hwnd);
    if (IsIconic(r.hwnd)) {
        g_winPanel.state = L"Minimized";
    } else if (IsZoomed(r.hwnd)) {
        g_winPanel.state = L"Maximized";
    } else {
        g_winPanel.state = L"Normal";
    }
    if (GetWindowLongPtrW(r.hwnd, GWL_EXSTYLE) & WS_EX_TOPMOST) {
        g_winPanel.state += L" · always on top";
    }
}

struct FindWindowCtx {
    const std::wstring* path;
    HWND found;
};

static BOOL CALLBACK FindWindowProc(HWND hwnd, LPARAM lp) {
    auto* c = (FindWindowCtx*)lp;
    if (!IsWindowVisible(hwnd) || hwnd == g_hwnd) return TRUE;
    const LONG_PTR ex = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    if (ex & (WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE)) return TRUE;
    const std::wstring title = WindowTitle(hwnd);
    if (title.empty()) return TRUE;
    if (WindowCloaked(hwnd)) return TRUE;
    wchar_t cls[128] = L"";
    GetClassNameW(hwnd, cls, ARRAYSIZE(cls));
    if (SkipWindowClass(cls)) return TRUE;
    if (SamePath(WindowProcessPath(hwnd), *c->path)) {
        c->found = hwnd;
        return FALSE;
    }
    return TRUE;
}

static HWND FindWindowForProcessPath(const std::wstring& path) {
    if (path.empty()) return nullptr;
    FindWindowCtx c{&path, nullptr};
    EnumWindows(FindWindowProc, (LPARAM)&c);
    return c.found;
}

// Opens a desktop app through its real target. Resolving "shell:AppsFolder\{GUID}\..." from this
// 32-bit process redirects the FOLDERID_ProgramFilesX64 GUID to "C:\Program Files (x86)", so 64-bit
// apps installed there fail to launch; System.Link.TargetParsingPath holds the real, unredirected
// path. Store (MSIX) apps use their AUMID.
static void LaunchApp(int appIdx) {
    if (appIdx < 0 || appIdx >= (int)g_apps.size()) {
        return;
    }
    AppEntry& a = g_apps[appIdx];
    // If the app already has a visible window, focus it instead of starting a second instance.
    std::wstring runningExe = a.target;
    if (runningExe.empty() && !a.launch.empty() &&
        EndsWith(FoldString(a.launch), L".exe")) {
        runningExe = a.launch;
    }
    if (!runningExe.empty() && a.args.empty()) {
        HWND existing = FindWindowForProcessPath(runningExe);
        if (existing && ActivateWindow(existing)) {
            RecordUse(a);
            return;
        }
    }
    AllowSetForegroundWindow(ASFW_ANY);

    // Entries found on the Desktop are opened through the shortcut itself, so Windows applies its
    // arguments, working folder and elevation prompt exactly as when it is double-clicked.
    if (!a.launch.empty()) {
        const std::wstring dir = ParentDir(a.launch);
        ShellExecuteW(nullptr, L"open", a.launch.c_str(), nullptr,
                      dir.empty() ? nullptr : dir.c_str(), SW_SHOWNORMAL);
        RecordUse(a);
        HideLauncher();
        return;
    }

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
            // A System32 target must not be resolved by this 32-bit process (it would be redirected
            // to SysWOW64), and turning the redirection off is unsafe: a failed delay-load stays
            // failed. Let the 64-bit shell resolve the AppsFolder ID instead.
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

// Opens a URL in the default browser (used for web searches and direct links).
static void OpenWebResult(const std::wstring& url) {
    if (url.empty()) {
        return;
    }
    AllowSetForegroundWindow(ASFW_ANY);
    ShellExecuteW(nullptr, L"open", url.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
    HideLauncher();
}

// Closes the launcher and opens a shell target (executable, .msc, ms-settings:, ...).
static void RunShellOpen(const wchar_t* target) {
    HideLauncher();
    AllowSetForegroundWindow(ASFW_ANY);
    ShellExecuteW(nullptr, L"open", target, nullptr, nullptr, SW_SHOWNORMAL);
}

// ---- Custom entries -----------------------------------------------------------------------
// A custom entry can ask for values with placeholders in its Target or Arguments:
//   {folder}        a required field
//   {folder?}       an optional field
//   {branch=main}   a field that starts pre-filled (still required unless it also has `?`)
//   {query}         the extra words typed after the entry's name (always optional)
// Names are case-insensitive, `{{` and `}}` are literal braces, and a placeholder can repeat.
// Pressing Enter opens a small form to fill/review these fields before launching (Shift+Enter
// skips it); an entry with only {query}, or none, launches straight away.

// True when the text already carries a URI scheme ("https:", "ms-settings:", "steam:", ...).
// A "host:port" such as localhost:3000 is NOT a scheme: it is treated as a web address, so it
// gets https:// in front instead of being handed to the shell as some unknown protocol.
static bool LooksAbsoluteUri(const std::wstring& s) {
    const size_t colon = s.find(L':');
    if (colon == std::wstring::npos || colon == 0) return false;
    if (!iswalpha(s[0])) return false;  // a scheme starts with a letter (C:\... also passes)
    for (size_t i = 0; i < colon; ++i) {
        const wchar_t c = s[i];
        if (!(iswalnum(c) || c == L'+' || c == L'-' || c == L'.')) return false;
    }
    const size_t cut = s.find_first_of(L"/\\?#");
    if (cut != std::wstring::npos && colon > cut) return false;
    // Everything after the colon being digits means a port number, not a scheme.
    size_t i = colon + 1;
    size_t digits = 0;
    while (i < s.size() && iswdigit(s[i])) {
        ++i;
        ++digits;
    }
    if (digits > 0 && (i == s.size() || s[i] == L'/' || s[i] == L'?' || s[i] == L'#')) {
        return false;
    }
    return true;
}

static bool CustomPathExists(const std::wstring& p) {
    return !p.empty() && GetFileAttributesW(p.c_str()) != INVALID_FILE_ATTRIBUTES;
}

static bool CustomHasPathPart(const std::wstring& p) {
    return p.find_first_of(L"\\/") != std::wstring::npos ||
           (p.size() >= 2 && p[1] == L':');
}

// A program is considered launchable if it is a file, or if Windows finds it on PATH (with or
// without .exe). Installed apps registered through App Paths are left to the shell.
static bool CustomProgramResolves(const std::wstring& p) {
    if (p.empty()) return false;
    if (CustomPathExists(p)) return true;
    wchar_t found[MAX_PATH];
    return SearchPathW(nullptr, p.c_str(), L".exe", MAX_PATH, found, nullptr) > 0;
}

static std::wstring Unquoted(std::wstring s) {
    s = TrimBlanks(s);
    if (s.size() >= 2 && s.front() == L'"' && s.back() == L'"') {
        s = TrimBlanks(s.substr(1, s.size() - 2));
    }
    return s;
}

static int FindCustomField(const std::vector<CustomField>& fields, const std::wstring& key) {
    const std::wstring folded = CustomKeyId(key);
    for (size_t i = 0; i < fields.size(); ++i) {
        if (CustomKeyId(fields[i].key) == folded) return (int)i;
    }
    return -1;
}

// Replaces known placeholders with their values. In a Website entry `encode` percent-encodes
// {query} (UTF-8), so search spaces, `&` and `#` cannot break the address.
static std::wstring ExpandCustomText(const std::wstring& text, const CustomEntry& e,
                                     const std::vector<std::wstring>& values, bool encode) {
    std::wstring out;
    out.reserve(text.size() + 32);
    for (size_t i = 0; i < text.size();) {
        if (text[i] == L'{' && i + 1 < text.size() && text[i + 1] == L'{') {
            out += L'{';
            i += 2;
            continue;
        }
        if (text[i] == L'}' && i + 1 < text.size() && text[i + 1] == L'}') {
            out += L'}';
            i += 2;
            continue;
        }
        if (text[i] == L'{') {
            const size_t close = text.find(L'}', i + 1);
            if (close != std::wstring::npos) {
                std::wstring body = text.substr(i + 1, close - i - 1);
                const size_t eq = body.find(L'=');
                if (eq != std::wstring::npos) body.resize(eq);
                body = TrimBlanks(body);
                if (!body.empty() && body.back() == L'?') {
                    body.pop_back();
                    body = TrimBlanks(body);
                }
                const int fi = FindCustomField(e.fields, body);
                if (fi >= 0 && fi < (int)values.size()) {
                    // Only {query} is percent-encoded (it holds search words). Other values are
                    // inserted as typed, so a field can carry a path, a repo (user/repo) or a
                    // whole base address without its slashes and colons being escaped.
                    out += (encode && IsQueryFieldKey(e.fields[fi].key)) ? UrlEncode(values[fi])
                                                                         : values[fi];
                    i = close + 1;
                    continue;
                }
            }
        }
        out += text[i++];
    }
    return out;
}

static bool CustomValueEmpty(const std::wstring& v) {
    return TrimBlanks(v).empty();
}

static bool CustomRequiredMissing(const CustomEntry& e, const std::vector<std::wstring>& values) {
    for (size_t i = 0; i < e.fields.size() && i < values.size(); ++i) {
        if (!e.fields[i].optional && CustomValueEmpty(values[i])) return true;
    }
    return false;
}

// Values a freshly opened entry starts with: its defaults, plus the typed extra words in {query}.
static std::vector<std::wstring> CustomInitialValues(const CustomEntry& e,
                                                     const std::wstring& query) {
    std::vector<std::wstring> v;
    v.reserve(e.fields.size());
    bool queryFieldExists = false;
    for (const CustomField& f : e.fields) {
        if (IsQueryFieldKey(f.key)) queryFieldExists = true;
        if (IsQueryFieldKey(f.key) && !query.empty()) {
            v.push_back(query);
        } else {
            v.push_back(f.value);
        }
    }
    // If the entry has no {query} field but extra words were typed after the name,
    // pre-fill the first empty required field with those words so typing
    // "GitHub DavidRaveloU" pre-fills {user} without hiding the entry.
    if (!queryFieldExists && !query.empty()) {
        for (size_t i = 0; i < e.fields.size() && i < v.size(); ++i) {
            if (!e.fields[i].optional && CustomValueEmpty(v[i])) {
                v[i] = query;
                break;
            }
        }
    }
    return v;
}

struct CustomCheck {
    std::wstring error;    // blocks the launch until it is fixed
    std::wstring warning;  // can be ignored with a second Enter
};

static CustomCheck CheckCustomTarget(const CustomEntry& e, const std::wstring& target) {
    CustomCheck c;
    if (target.empty()) {
        c.error = L"The target is empty.";
        return c;
    }
    if (e.type == L"url") return c;

    // A path such as C:\Program Files\... is not a URI scheme, even though LooksAbsoluteUri
    // accepts it; only real schemes (shell:, ms-settings:, steam://...) skip the file checks.
    const bool drivePath = target.size() >= 2 && iswalpha(target[0]) && target[1] == L':';
    if (!drivePath && LooksAbsoluteUri(target)) return c;

    const std::wstring path = Unquoted(target);
    if (e.type == L"path") {
        if (!CustomPathExists(path)) {
            c.error = L"Not found: " + path;
            if (path.find(L'%') != std::wstring::npos) {
                c.error += L" (check the environment variable)";
            }
        }
        return c;
    }

    // Command entries.
    if (CustomPathExists(path)) return c;

    // "cmd.exe /k dir": the whole target is not a file, but its first word is a program. This is
    // the most common mistake (arguments typed into Target), so give a clear message.
    const size_t sp = path.find(L' ');
    if (sp != std::wstring::npos) {
        const std::wstring first = path.substr(0, sp);
        const std::wstring rest = TrimBlanks(path.substr(sp + 1));
        if (!first.empty() && !rest.empty() && CustomProgramResolves(first)) {
            c.error = L"Put the arguments in the Arguments field, not in Target.";
            return c;
        }
    }

    if (CustomHasPathPart(path)) {
        c.error = L"Program not found: " + path;
        return c;
    }

    // A bare name may still be resolved by the shell through an installed app, so only names
    // that look like a full executable are treated as a definite typo.
    if (!CustomProgramResolves(path)) {
        const std::wstring folded = FoldString(path);
        if (folded.size() > 4 && folded.compare(folded.size() - 4, 4, L".exe") == 0) {
            c.error = L"Program not found: " + path;
        } else {
            c.warning = L"Windows may not find “" + path + L"”. Use its full path if it fails.";
        }
    }
    return c;
}

static void BuildCustomLaunch(const CustomEntry& e, const std::vector<std::wstring>& values,
                              std::wstring& target, std::wstring& args) {
    const bool isUrl = e.type == L"url";
    target = ExpandedPath(TrimBlanks(ExpandCustomText(e.target, e, values, isUrl)));
    args = ExpandedPath(TrimBlanks(ExpandCustomText(e.args, e, values, false)));
    if (isUrl && !target.empty() && !LooksAbsoluteUri(target)) target = L"https://" + target;
}

static CustomCheck ResolveCustomEntry(const CustomEntry& e,
                                      const std::vector<std::wstring>& values,
                                      std::wstring& target, std::wstring& args) {
    BuildCustomLaunch(e, values, target, args);
    return CheckCustomTarget(e, target);
}

static void LaunchCustomEntry(const CustomEntry& e, const std::wstring& target,
                              const std::wstring& args) {
    if (target.empty()) return;
    AllowSetForegroundWindow(ASFW_ANY);
    if (e.type == L"url") {
        HideLauncher();
        ShellExecuteW(nullptr, L"open", target.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
        return;
    }
    // A program is started in its own folder (portable apps and games read files next to it);
    // for a file or folder, the shell decides the working directory.
    const std::wstring dir = (e.type == L"run") ? ParentDir(target) : L"";
    HideLauncher();
    ShellExecuteW(nullptr, L"open", target.c_str(), args.empty() ? nullptr : args.c_str(),
                  dir.empty() ? nullptr : dir.c_str(), SW_SHOWNORMAL);
}

// ---- Custom entry form ----

static void EnsureFormFocusVisible() {
    const int count = (int)g_form.values.size();
    if (count == 0) {
        g_form.scroll = 0;
        return;
    }
    if (g_form.focus < g_form.scroll) g_form.scroll = g_form.focus;
    if (g_form.focus >= g_form.scroll + kFormRows) g_form.scroll = g_form.focus - kFormRows + 1;
    g_form.scroll = std::clamp(g_form.scroll, 0, std::max(0, count - kFormRows));
}

static void OpenCustomForm(int idx, const std::wstring& query) {
    if (idx < 0 || idx >= (int)g_cfg.commands.size()) return;
    const CustomEntry& e = g_cfg.commands[idx];
    CustomFormState st;
    st.active = true;
    st.entry = idx;
    st.values = CustomInitialValues(e, query);
    st.focus = -1;
    // Focus the first empty required field; failing that, the first empty optional one.
    for (size_t i = 0; i < st.values.size(); ++i) {
        if (!e.fields[i].optional && CustomValueEmpty(st.values[i])) {
            st.focus = (int)i;
            break;
        }
    }
    if (st.focus < 0) {
        for (size_t i = 0; i < st.values.size(); ++i) {
            if (CustomValueEmpty(st.values[i])) {
                st.focus = (int)i;
                break;
            }
        }
    }
    if (st.focus < 0) st.focus = 0;
    st.caret = st.anchor = st.values.empty() ? 0 : (int)st.values[st.focus].size();
    g_form = std::move(st);
    EnsureFormFocusVisible();
    g_detailsOpen = false;
    g_detailTarget = 0;
    UpdateTargets(true, false);
    ResetBlink();
    MarkDirty();
}

static void CloseCustomForm() {
    if (!g_form.active) return;
    g_form = CustomFormState();
    UpdateTargets(false, false);
    MarkDirty();
}

static void MoveFormFocus(int d, bool wrap) {
    const int n = (int)g_form.values.size();
    if (n == 0) return;
    int f = g_form.focus + d;
    if (wrap) {
        f = (f % n + n) % n;
    } else {
        f = std::clamp(f, 0, n - 1);
    }
    if (f == g_form.focus) return;
    g_form.focus = f;
    g_form.caret = g_form.anchor = (int)g_form.values[f].size();
    g_form.inputScroll = 0;
    EnsureFormFocusVisible();
    ResetBlink();
    MarkDirty();
}

static std::wstring* FormValue() {
    if (!g_form.active || g_form.focus < 0 || g_form.focus >= (int)g_form.values.size()) {
        return nullptr;
    }
    return &g_form.values[g_form.focus];
}

static void FormReplaceSelection(const std::wstring& s) {
    std::wstring* v = FormValue();
    if (!v) return;
    int a = std::min(g_form.anchor, g_form.caret);
    int b = std::max(g_form.anchor, g_form.caret);
    a = std::clamp(a, 0, (int)v->size());
    b = std::clamp(b, 0, (int)v->size());
    if (v->size() - (size_t)(b - a) + s.size() > 2048) return;
    *v = v->substr(0, a) + s + v->substr(b);
    g_form.caret = g_form.anchor = a + (int)s.size();
    g_form.error.clear();
    g_form.errorField = -1;
    g_form.warning.clear();
    g_form.warned = false;
    ResetBlink();
    MarkDirty();
}

static void FormMoveCaret(int pos, bool extend) {
    std::wstring* v = FormValue();
    if (!v) return;
    pos = std::clamp(pos, 0, (int)v->size());
    g_form.caret = pos;
    if (!extend) g_form.anchor = pos;
    ResetBlink();
    MarkDirty();
}

static int FormPrevWord(int pos) {
    std::wstring* v = FormValue();
    if (!v) return 0;
    while (pos > 0 && (*v)[pos - 1] == L' ') --pos;
    while (pos > 0 && (*v)[pos - 1] != L' ') --pos;
    return pos;
}

static int FormNextWord(int pos) {
    std::wstring* v = FormValue();
    if (!v) return 0;
    const int n = (int)v->size();
    while (pos < n && (*v)[pos] != L' ') ++pos;
    while (pos < n && (*v)[pos] == L' ') ++pos;
    return pos;
}

static void CopyTextToClipboard(const std::wstring& s);  // defined further down

static std::wstring FormClipboardText() {
    if (!OpenClipboard(g_hwnd)) return L"";
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
    for (wchar_t& c : s) {
        if (c == L'\r' || c == L'\n' || c == L'\t') c = L' ';
    }
    return s;
}

static void FormCopy(bool cut) {
    std::wstring* v = FormValue();
    if (!v) return;
    const int a = std::min(g_form.anchor, g_form.caret);
    const int b = std::max(g_form.anchor, g_form.caret);
    if (a == b) return;
    CopyTextToClipboard(v->substr(a, b - a));
    if (cut) FormReplaceSelection(L"");
}

static void FormPaste() {
    const std::wstring s = FormClipboardText();
    if (!s.empty()) FormReplaceSelection(s);
}

static void SubmitCustomForm() {
    if (g_form.entry < 0 || g_form.entry >= (int)g_cfg.commands.size()) {
        CloseCustomForm();
        return;
    }
    const CustomEntry& e = g_cfg.commands[g_form.entry];

    // Required fields must not be empty; optional ones (including {query}) may be.
    for (size_t i = 0; i < e.fields.size() && i < g_form.values.size(); ++i) {
        if (e.fields[i].optional) continue;
        if (CustomValueEmpty(g_form.values[i])) {
            g_form.focus = (int)i;
            g_form.caret = g_form.anchor = 0;
            g_form.inputScroll = 0;
            g_form.error = L"Fill in “" + e.fields[i].label + L"”.";
            g_form.errorField = (int)i;
            EnsureFormFocusVisible();
            ResetBlink();
            MarkDirty();
            return;
        }
    }

    std::wstring target, args;
    const CustomCheck check = ResolveCustomEntry(e, g_form.values, target, args);
    if (!check.error.empty()) {
        g_form.error = check.error;
        g_form.errorField = -1;
        g_form.warning.clear();
        g_form.warned = false;
        MarkDirty();
        return;
    }
    if (!check.warning.empty() && !g_form.warned) {
        g_form.warning = check.warning;
        g_form.warned = true;
        g_form.error.clear();
        g_form.errorField = -1;
        MarkDirty();
        return;
    }
    g_form.error.clear();
    g_form.warning.clear();
    LaunchCustomEntry(e, target, args);
}

static bool HandleCustomFormKey(WPARAM vk, bool ctrl, bool shift) {
    std::wstring* v = FormValue();
    const int len = v ? (int)v->size() : 0;
    const bool sel = g_form.anchor != g_form.caret;
    switch (vk) {
        case VK_ESCAPE:
            CloseCustomForm();
            return true;
        case VK_RETURN:
            SubmitCustomForm();
            return true;
        case VK_TAB:
            MoveFormFocus(shift ? -1 : 1, true);
            return true;
        case VK_UP:
            MoveFormFocus(-1, false);
            return true;
        case VK_DOWN:
            MoveFormFocus(1, false);
            return true;
        case VK_LEFT:
            if (!v) return true;
            if (sel && !shift && !ctrl) {
                FormMoveCaret(std::min(g_form.anchor, g_form.caret), false);
            } else {
                FormMoveCaret(ctrl ? FormPrevWord(g_form.caret) : g_form.caret - 1, shift);
            }
            return true;
        case VK_RIGHT:
            if (!v) return true;
            if (sel && !shift && !ctrl) {
                FormMoveCaret(std::max(g_form.anchor, g_form.caret), false);
            } else {
                FormMoveCaret(ctrl ? FormNextWord(g_form.caret) : g_form.caret + 1, shift);
            }
            return true;
        case VK_HOME:
            if (v) FormMoveCaret(0, shift);
            return true;
        case VK_END:
            if (v) FormMoveCaret(len, shift);
            return true;
        case VK_BACK:
            if (!v) return true;
            if (sel) {
                FormReplaceSelection(L"");
                Sound::OnErase();
            } else if (g_form.caret > 0) {
                g_form.anchor = ctrl ? FormPrevWord(g_form.caret) : g_form.caret - 1;
                FormReplaceSelection(L"");
                Sound::OnErase();
            }
            return true;
        case VK_DELETE:
            if (!v) return true;
            if (sel) {
                FormReplaceSelection(L"");
                Sound::OnErase();
            } else if (g_form.caret < len) {
                g_form.anchor = ctrl ? FormNextWord(g_form.caret) : g_form.caret + 1;
                FormReplaceSelection(L"");
                Sound::OnErase();
            }
            return true;
        case 'A':
            if (ctrl && v) {
                g_form.anchor = 0;
                g_form.caret = len;
                ResetBlink();
                MarkDirty();
            }
            return true;
        case 'C':
            if (ctrl) FormCopy(false);
            return true;
        case 'X':
            if (ctrl) FormCopy(true);
            return true;
        case 'V':
            if (ctrl) FormPaste();
            return true;
    }
    return true;  // the form is modal: nothing reaches the search bar behind it
}

// Enter on a custom entry opens the form whenever the entry has fields to fill in — any
// placeholder other than {query} — so the values can be reviewed or edited before it launches. A
// pure search entry ({query} only) or a fieldless entry launches straight away. Shift+Enter skips
// the form and launches with the current values; if a required field is still empty (or the target
// is broken) the form opens regardless, so nothing ever launches with a missing value.
static void ActivateCustomEntry(int idx, const std::wstring& query, bool skipForm) {
    if (idx < 0 || idx >= (int)g_cfg.commands.size()) return;
    const CustomEntry& e = g_cfg.commands[idx];
    const std::vector<std::wstring> values = CustomInitialValues(e, query);
    const bool hasFormFields = CustomHasFormFields(e);  // any placeholder other than {query}
    if ((!skipForm && hasFormFields) || CustomRequiredMissing(e, values)) {
        OpenCustomForm(idx, query);
        return;
    }
    std::wstring target, args;
    const CustomCheck check = ResolveCustomEntry(e, values, target, args);
    if (!check.error.empty()) {
        OpenCustomForm(idx, query);
        g_form.error = check.error;
        MarkDirty();
        return;
    }
    LaunchCustomEntry(e, target, args);
}

// ---- Drawing the form ----

static std::wstring FitEnd(const std::wstring& t, IDWriteTextFormat* fmt, float widthPx) {
    if (!fmt || t.empty() || TextWidth(t.c_str(), fmt) <= widthPx) return t;
    const std::wstring ell = L"…";
    std::wstring s = t;
    while (!s.empty()) {
        s.pop_back();
        if (TextWidth((s + ell).c_str(), fmt) <= widthPx) break;
    }
    return s + ell;
}

struct FormGeom {
    float x0, x1, listTop, rowsTop, rowH, previewTop, footerTop;
    int first, count;
};

static FormGeom GetFormGeom(float cx, float cy, float cw, float s) {
    FormGeom g{};
    g.x0 = cx + 20 * s;
    g.x1 = cx + cw - 20 * s;
    g.listTop = cy + kBarH * s;
    g.rowsTop = g.listTop + 34 * s;  // 6 padding + 28 header
    g.rowH = kFormRowH * s;
    g.footerTop = cy + (float)(ExpandedH() - kFooterH) * s;
    g.previewTop = g.footerTop - 32 * s;
    g.first = g_form.scroll;
    g.count = 0;
    if (g_form.entry >= 0 && g_form.entry < (int)g_cfg.commands.size()) {
        g.count = (int)g_cfg.commands[g_form.entry].fields.size();
    }
    return g;
}

static D2D1_RECT_F FormValueRect(const FormGeom& g, int i, float s) {
    const float y = g.rowsTop + (float)(i - g.first) * g.rowH;
    return D2D1::RectF(g.x0 + 200 * s, y + 7 * s, g.x1 - 12 * s, y + g.rowH - 7 * s);
}

static void DrawCustomForm(float cx, float cy, float cw, float s) {
    if (!g_form.active) return;
    if (g_form.entry < 0 || g_form.entry >= (int)g_cfg.commands.size()) return;
    const CustomEntry& e = g_cfg.commands[g_form.entry];
    const FormGeom g = GetFormGeom(cx, cy, cw, s);

    SetBrush(g_pal.separator);
    D2D1_RECT_F sep = D2D1::RectF(cx + 12 * s, g.listTop, cx + cw - 12 * s, g.listTop + 1.f);
    g_rt->FillRectangle(&sep, g_brush);

    // Header: entry icon, name, type and (when needed) "1-6 of 8".
    const float hy = g.listTop + 6 * s;
    const float hh = 28 * s;
    const float isz = 20 * s;
    const bool isUrl = e.type == L"url";
    const bool isRun = e.type == L"run";
    const D2D1_COLOR_F ct = isUrl ? D2D1::ColorF(0.20f, 0.55f, 0.90f)
                            : isRun ? D2D1::ColorF(0.55f, 0.38f, 0.88f)
                                    : D2D1::ColorF(0.92f, 0.62f, 0.22f);
    FillRR(g.x0, hy + (hh - isz) / 2, g.x0 + isz, hy + (hh + isz) / 2, 6 * s, ct);
    DrawCommandGlyph(isUrl ? CmdGlyph::Link : isRun ? CmdGlyph::Terminal
                                                   : CmdGlyph::Folder,
                     g.x0, hy + (hh - isz) / 2, isz, ct);
    DrawLabel(e.name, g_fmtTitle, g.x0 + 30 * s, hy, g.x1 - 160 * s, hy + hh, g_pal.text, 1.f);
    const wchar_t* typeTxt = CustomTypeLabel(e.type);
    const float tw = TextWidth(typeTxt, g_fmtMeta);
    DrawLabel(typeTxt, g_fmtMeta, g.x1 - tw, hy, g.x1, hy + hh, g_pal.textDim, 1.f);
    if (g.count > kFormRows) {
        wchar_t counter[64];
        swprintf_s(counter, L"%d-%d of %d", g.first + 1,
                   std::min(g.first + kFormRows, g.count), g.count);
        const float cwTxt = TextWidth(counter, g_fmtMeta);
        DrawLabel(counter, g_fmtMeta, g.x1 - tw - 12 * s - cwTxt, hy, g.x1 - tw - 12 * s,
                  hy + hh, g_pal.textDim, 0.9f);
    }

    // Fields.
    const D2D1_RECT_F rowsClip =
        D2D1::RectF(cx + 10 * s, g.rowsTop, cx + cw - 10 * s, g.previewTop - 2 * s);
    g_rt->PushAxisAlignedClip(&rowsClip, D2D1_ANTIALIAS_MODE_ALIASED);
    const int last = std::min(g.count, g.first + kFormRows);
    for (int i = g.first; i < last; ++i) {
        const CustomField& f = e.fields[i];
        const float y = g.rowsTop + (float)(i - g.first) * g.rowH;
        const bool focused = (i == g_form.focus);
        const bool bad = (i == g_form.errorField);
        const D2D1_RECT_F vr = FormValueRect(g, i, s);
        FillRR(vr.left, vr.top, vr.right, vr.bottom, 7 * s,
               bad ? D2D1::ColorF(0.93f, 0.30f, 0.30f) : focused ? g_pal.accent : g_pal.keycap,
               bad || focused ? 0.12f : 0.45f);
        if (focused || bad) {
            SetBrush(bad ? D2D1::ColorF(0.93f, 0.30f, 0.30f) : g_pal.accent, 0.5f);
            D2D1_ROUNDED_RECT rr = D2D1::RoundedRect(
                D2D1::RectF(vr.left + 0.5f, vr.top + 0.5f, vr.right - 0.5f, vr.bottom - 0.5f),
                7 * s, 7 * s);
            g_rt->DrawRoundedRectangle(&rr, g_brush, 1.f);
        }

        const float labelW = 96 * s;
        DrawLabel(f.label, g_fmtFormLabel, g.x0 + 14 * s, y, g.x0 + 14 * s + labelW, y + g.rowH,
                  focused ? g_pal.text : g_pal.textDim, 1.f);
        if (f.optional) {
            const float tagX = g.x0 + 14 * s + labelW + 6 * s;
            const float tagW = TextWidth(kTxtFormOptional, g_fmtMeta) + 12 * s;
            const float tagH = 16 * s;
            const float tagY = y + (g.rowH - tagH) / 2;
            FillRR(tagX, tagY, tagX + tagW, tagY + tagH, tagH / 2, g_pal.textDim, 0.10f);
            DrawLabel(kTxtFormOptional, g_fmtMeta, tagX, tagY, tagX + tagW, tagY + tagH,
                      g_pal.textDim, 0.9f);
        }

        if (i >= (int)g_form.values.size()) continue;
        const std::wstring& val = g_form.values[i];
        const float vx0 = vr.left + 8 * s;
        const float vx1 = vr.right - 8 * s;
        if (val.empty()) {
            const wchar_t* ph = f.optional ? kTxtFormOptional : kTxtFormRequired;
            DrawLabel(ph, g_fmtForm, vx0, y, vx1, y + g.rowH,
                      bad ? D2D1::ColorF(0.93f, 0.30f, 0.30f) : g_pal.textDim, bad ? 0.9f : 0.5f);
            if (focused && g_caretOn) {
                SetBrush(g_pal.text);
                D2D1_RECT_F cr = D2D1::RectF(vx0, y + 10 * s, vx0 + 1.5f * s, y + g.rowH - 10 * s);
                g_rt->FillRectangle(&cr, g_brush);
            }
        } else if (focused) {
            IDWriteTextLayout* tl = nullptr;
            g_dw->CreateTextLayout(val.c_str(), (UINT32)val.size(), g_fmtForm, 10000.f,
                                   kFormRowH * s, &tl);
            if (tl) {
                float caretX = 0, tmpY = 0;
                DWRITE_HIT_TEST_METRICS hm = {};
                const int caret = std::clamp(g_form.caret, 0, (int)val.size());
                tl->HitTestTextPosition((UINT32)caret, FALSE, &caretX, &tmpY, &hm);
                const float avail = vx1 - vx0;
                if (caretX - g_form.inputScroll > avail - 4 * s) {
                    g_form.inputScroll = caretX - (avail - 4 * s);
                }
                if (caretX - g_form.inputScroll < 0) g_form.inputScroll = caretX;
                if (g_form.inputScroll < 0) g_form.inputScroll = 0;
                const float ox = vx0 - g_form.inputScroll;
                if (g_form.anchor != g_form.caret) {
                    float ax = 0, ay = 0;
                    DWRITE_HIT_TEST_METRICS hm2 = {};
                    const int anchor = std::clamp(g_form.anchor, 0, (int)val.size());
                    tl->HitTestTextPosition((UINT32)anchor, FALSE, &ax, &ay, &hm2);
                    SetBrush(g_pal.caretSel);
                    D2D1_RECT_F sr = D2D1::RectF(ox + std::min(ax, caretX), vr.top + 5 * s,
                                                ox + std::max(ax, caretX), vr.bottom - 5 * s);
                    g_rt->FillRectangle(&sr, g_brush);
                }
                SetBrush(g_pal.text);
                g_rt->DrawTextLayout(D2D1::Point2F(ox, y), tl, g_brush);
                if (g_caretOn) {
                    SetBrush(g_pal.text);
                    D2D1_RECT_F cr = D2D1::RectF(ox + caretX, y + 10 * s, ox + caretX + 1.5f * s,
                                                y + g.rowH - 10 * s);
                    g_rt->FillRectangle(&cr, g_brush);
                }
                tl->Release();
            }
        } else {
            const std::wstring shown = FitEnd(val, g_fmtForm, vx1 - vx0);
            DrawLabel(shown, g_fmtForm, vx0, y, vx1, y + g.rowH, g_pal.text, 1.f);
        }
    }
    g_rt->PopAxisAlignedClip();

    // An entry with no placeholders only opens this form to report a problem, so show what it
    // points at: that is what has to be corrected in the settings.
    if (g.count == 0) {
        DrawLabel(L"Target", g_fmtFormLabel, g.x0 + 14 * s, g.rowsTop, g.x0 + 110 * s,
                  g.rowsTop + g.rowH, g_pal.textDim, 1.f);
        std::wstring what = e.target;
        if (!e.args.empty()) what += L" " + e.args;
        const float tx = g.x0 + 124 * s;
        DrawLabel(FitEnd(what, g_fmtForm, g.x1 - tx), g_fmtForm, tx, g.rowsTop, g.x1,
                  g.rowsTop + g.rowH, g_pal.text, 0.9f);
    }

    // Preview of what will happen, or the problem that prevents it.
    const float py = g.previewTop;
    const float ph = 26 * s;
    const float px0 = g.x0;
    const float px1 = g.x1;
    const float maxW = px1 - px0;
    if (!g_form.error.empty()) {
        FillRR(px0, py, px1, py + ph, 7 * s, D2D1::ColorF(0.93f, 0.30f, 0.30f), 0.10f);
        const std::wstring msg = FitEnd(g_form.error, g_fmtMeta, maxW - 16 * s);
        DrawLabel(msg, g_fmtMeta, px0 + 8 * s, py, px1 - 8 * s, py + ph,
                  D2D1::ColorF(0.96f, 0.55f, 0.55f), 1.f);
    } else if (!g_form.warning.empty()) {
        FillRR(px0, py, px1, py + ph, 7 * s, D2D1::ColorF(0.93f, 0.62f, 0.22f), 0.10f);
        std::wstring msg = g_form.warning;
        if (g_form.warned) msg += L"  (press Enter again to run)";
        msg = FitEnd(msg, g_fmtMeta, maxW - 16 * s);
        DrawLabel(msg, g_fmtMeta, px0 + 8 * s, py, px1 - 8 * s, py + ph,
                  D2D1::ColorF(0.95f, 0.72f, 0.40f), 1.f);
    } else {
        if (CustomRequiredMissing(e, g_form.values)) {
            DrawLabel(kTxtFormFill, g_fmtMeta, px0 + 2 * s, py, px1, py + ph, g_pal.textDim, 0.85f);
        } else {
            std::wstring target, args;
            BuildCustomLaunch(e, g_form.values, target, args);
            std::wstring line = target;
            if (isRun && !args.empty()) line += L" " + args;
            const wchar_t* lead = isRun ? kTxtFormWillRun : kTxtFormWillOpen;
            const float lw = TextWidth(lead, g_fmtMeta);
            DrawLabel(lead, g_fmtMeta, px0 + 2 * s, py, px0 + 2 * s + lw, py + ph,
                      g_pal.textDim, 0.9f);
            const std::wstring shown = FitEnd(line, g_fmtMeta, maxW - lw - 16 * s);
            DrawLabel(shown, g_fmtMeta, px0 + 2 * s + lw + 8 * s, py, px1, py + ph, g_pal.text,
                      0.9f);
        }
    }
}

static void DrawCustomFormFooter(float cx, float footerTop, float cw) {
    const float s = (float)g_scale;
    SetBrush(g_pal.separator);
    D2D1_RECT_F sep = D2D1::RectF(cx + 12 * s, footerTop, cx + cw - 12 * s, footerTop + 1.f);
    g_rt->FillRectangle(&sep, g_brush);

    const float yMid = footerTop + kFooterH * s / 2 + 0.5f;
    const bool run = g_form.entry >= 0 && g_form.entry < (int)g_cfg.commands.size() &&
                     g_cfg.commands[g_form.entry].type == L"run";
    const bool hasFields = g_form.entry >= 0 && g_form.entry < (int)g_cfg.commands.size() &&
                           !g_cfg.commands[g_form.entry].fields.empty();
    const wchar_t* ok[1] = {kTxtEnterGlyph};
    const wchar_t* tab[1] = {L"Tab"};
    const wchar_t* esc[1] = {L"Esc"};
    float x = cx + cw - 20 * s;
    if (hasFields) {
        x = DrawHint(run ? kTxtRun : kTxtOpen, ok, 1, x, yMid);
        x -= 16 * s;
        x = DrawHint(kTxtFormNext, tab, 1, x, yMid);
        x -= 16 * s;
    }
    DrawHint(kTxtFormBack, esc, 1, x, yMid);
}

static int HitCustomFormField(int mx, int my) {
    if (!g_form.active) return -1;
    float cx = 0, cy = 0;
    MainOrigin(cx, cy);
    const float s = (float)g_scale;
    const FormGeom g = GetFormGeom(cx, cy, kWidth * s, s);
    if (mx < g.x0 || mx > g.x1) return -1;
    if (my < g.rowsTop || my >= g.previewTop) return -1;
    const int i = g.first + (int)std::floor((my - g.rowsTop) / g.rowH);
    if (i < g.first || i >= std::min(g.count, g.first + kFormRows)) return -1;
    return i;
}

static bool CustomFormClick(int mx, int my) {
    if (!g_form.active) return false;
    const int i = HitCustomFormField(mx, my);
    if (i < 0) return false;
    if (i != g_form.focus) {
        g_form.focus = i;
        g_form.inputScroll = 0;
        EnsureFormFocusVisible();
    }
    float cx = 0, cy = 0;
    MainOrigin(cx, cy);
    const float s = (float)g_scale;
    const FormGeom g = GetFormGeom(cx, cy, kWidth * s, s);
    const D2D1_RECT_F vr = FormValueRect(g, i, s);
    int pos = 0;
    if (i < (int)g_form.values.size()) {
        const std::wstring& val = g_form.values[i];
        IDWriteTextLayout* tl = nullptr;
        g_dw->CreateTextLayout(val.c_str(), (UINT32)val.size(), g_fmtForm, 10000.f,
                               kFormRowH * s, &tl);
        if (tl) {
            const float lx = (float)mx - (vr.left + 8 * s - g_form.inputScroll);
            const float ly = (float)my - (g.rowsTop + (float)(i - g.first) * g.rowH);
            BOOL trailing = FALSE, inside = FALSE;
            DWRITE_HIT_TEST_METRICS hm = {};
            tl->HitTestPoint(lx, ly, &trailing, &inside, &hm);
            pos = hm.textPosition + (trailing ? hm.length : 0);
            tl->Release();
        }
        pos = std::clamp(pos, 0, (int)val.size());
    }
    g_form.caret = g_form.anchor = pos;
    ResetBlink();
    MarkDirty();
    return true;
}

// ===== Shortcuts for custom entries =====
// Each entry can own a key combination (Ctrl+Alt+T ...). It is matched by the launcher window while
// the bar is open and is never registered with the system: it cannot fire while the launcher is
// closed, and it cannot take a combination away from Windows or another application.

// Parses "ctrl+alt+t" into modifier flags and a virtual key. Needs at least one modifier and
// exactly one key. Returns false for anything else (including an empty string).
static bool ParseHotkey(const std::wstring& text, UINT& mods, UINT& vk) {
    mods = 0;
    vk = 0;
    std::vector<std::wstring> parts;
    std::wstring cur;
    for (const wchar_t c : text) {
        if (c == L'+') {
            if (!cur.empty()) parts.push_back(cur);
            cur.clear();
        } else if (c != L' ' && c != L'\t') {
            cur += c;
        }
    }
    if (!cur.empty()) parts.push_back(cur);
    if (parts.size() < 2) return false;
    for (const std::wstring& raw : parts) {
        const std::wstring p = FoldString(raw);
        if (p == L"ctrl" || p == L"control") {
            mods |= MOD_CONTROL;
        } else if (p == L"alt") {
            mods |= MOD_ALT;
        } else if (p == L"shift") {
            mods |= MOD_SHIFT;
        } else if (p == L"win") {
            mods |= MOD_WIN;
        } else {
            if (vk != 0) return false;  // more than one non-modifier key
            if (p.size() == 1 && p[0] >= L'a' && p[0] <= L'z') {
                vk = (UINT)towupper(p[0]);
            } else if (p.size() == 1 && p[0] >= L'0' && p[0] <= L'9') {
                vk = (UINT)p[0];
            } else if (p.size() >= 2 && p[0] == L'f') {
                const int n = _wtoi(p.c_str() + 1);
                if (n < 1 || n > 24) return false;
                vk = VK_F1 + (UINT)(n - 1);
            } else if (p == L"space") {
                vk = VK_SPACE;
            } else if (p == L"tab") {
                vk = VK_TAB;
            } else if (p == L"enter") {
                vk = VK_RETURN;
            } else if (p == L"insert") {
                vk = VK_INSERT;
            } else if (p == L"delete") {
                vk = VK_DELETE;
            } else if (p == L"home") {
                vk = VK_HOME;
            } else if (p == L"end") {
                vk = VK_END;
            } else if (p == L"pageup") {
                vk = VK_PRIOR;
            } else if (p == L"pagedown") {
                vk = VK_NEXT;
            } else {
                return false;
            }
        }
    }
    // Shift on its own would swallow ordinary capital letters, so Ctrl, Alt or Win is required.
    return (mods & (MOD_CONTROL | MOD_ALT | MOD_WIN)) != 0 && vk != 0;
}

// The canonical text for a combination ("ctrl+alt+t"), or empty if the key has no name here.
static std::wstring FormatHotkey(UINT mods, UINT vk) {
    std::wstring s;
    if (mods & MOD_CONTROL) s += L"ctrl+";
    if (mods & MOD_ALT) s += L"alt+";
    if (mods & MOD_SHIFT) s += L"shift+";
    if (mods & MOD_WIN) s += L"win+";
    wchar_t key[16] = L"";
    if (vk >= 'A' && vk <= 'Z') {
        key[0] = (wchar_t)towlower((wint_t)vk);
    } else if (vk >= '0' && vk <= '9') {
        key[0] = (wchar_t)vk;
    } else if (vk >= VK_F1 && vk <= VK_F24) {
        swprintf_s(key, L"f%u", vk - VK_F1 + 1);
    } else if (vk == VK_SPACE) {
        wcscpy_s(key, L"space");
    } else if (vk == VK_TAB) {
        wcscpy_s(key, L"tab");
    } else if (vk == VK_RETURN) {
        wcscpy_s(key, L"enter");
    } else if (vk == VK_INSERT) {
        wcscpy_s(key, L"insert");
    } else if (vk == VK_DELETE) {
        wcscpy_s(key, L"delete");
    } else if (vk == VK_HOME) {
        wcscpy_s(key, L"home");
    } else if (vk == VK_END) {
        wcscpy_s(key, L"end");
    } else if (vk == VK_PRIOR) {
        wcscpy_s(key, L"pageup");
    } else if (vk == VK_NEXT) {
        wcscpy_s(key, L"pagedown");
    } else {
        return std::wstring();
    }
    return s + key;
}

// Runs the entry whose Shortcut matches the key being pressed, while the launcher is open. The
// window has the focus at that point, so no global registration (and no conflict) is involved.
static bool TryRunEntryHotkey(WPARAM vk) {
    if (g_vis == Vis::Hidden || vk == VK_CONTROL || vk == VK_MENU || vk == VK_SHIFT ||
        vk == VK_LWIN || vk == VK_RWIN) {
        return false;  // a modifier on its own never matches
    }
    UINT mods = 0;
    if (GetKeyState(VK_CONTROL) & 0x8000) mods |= MOD_CONTROL;
    if (GetKeyState(VK_MENU) & 0x8000) mods |= MOD_ALT;
    if (GetKeyState(VK_SHIFT) & 0x8000) mods |= MOD_SHIFT;
    if ((GetKeyState(VK_LWIN) & 0x8000) || (GetKeyState(VK_RWIN) & 0x8000)) mods |= MOD_WIN;
    if (mods == 0) return false;
    for (size_t i = 0; i < g_cfg.commands.size(); ++i) {
        const CustomEntry& e = g_cfg.commands[i];
        if (e.hotkey.empty()) continue;
        UINT eMods = 0, eKey = 0;
        if (!ParseHotkey(e.hotkey, eMods, eKey) || eMods != mods || eKey != (UINT)vk) continue;
        if (CustomHasFormFields(e)) {
            OpenCustomForm((int)i, L"");
            MarkDirty();
        } else {
            ActivateCustomEntry((int)i, L"", /*skipForm=*/true);
        }
        return true;
    }
    return false;
}

// Manager validation: a single lowercase word, 1..16 of [a-z0-9_-], that no other entry uses.
// The typed mode prefixes ("g:", "bm:") need a colon, so a plain word never shadows them.
static bool ValidTrigger(const std::wstring& trigger, const std::wstring& ignoreId,
                         std::wstring& error) {
    if (trigger.empty()) return true;
    if (trigger.size() > 16) {
        error = L"Trigger: at most 16 characters.";
        return false;
    }
    for (const wchar_t c : trigger) {
        if (!((c >= L'a' && c <= L'z') || (c >= L'0' && c <= L'9') || c == L'-' || c == L'_')) {
            error = L"Trigger: use lowercase letters, digits, - or _ only.";
            return false;
        }
    }
    for (const CustomEntry& e : g_cfg.commands) {
        if (e.id != ignoreId && e.trigger == trigger) {
            error = L"Trigger: another entry already uses it.";
            return false;
        }
    }
    return true;
}

// ===== Custom-entries manager (in-app create / edit / delete) =====

static const wchar_t* const kMgrTypeIds[3] = {L"url", L"path", L"run"};
static const wchar_t* const kMgrTypeLabels[3] = {L"Website", L"File or folder", L"Command"};
static const int kMgrFieldType = 1, kMgrFieldArgs = 3,
                 kMgrFieldKeywords = 4, kMgrFieldTrigger = 5, kMgrFieldHotkey = 6;
static const wchar_t* const kMgrFieldLabels[kMgrFields] = {
    L"Name", L"Type", L"Target", L"Arguments", L"Keywords", L"Trigger", L"Shortcut"};

static std::vector<CustomEntry> ManagerStoreList() {
    std::vector<CustomEntry> v;
    for (const CustomEntry& e : g_cfg.commands) {
        if (e.fromStore) v.push_back(e);
    }
    return v;
}

static int ManagerStoreCount() {
    int n = 0;
    for (const CustomEntry& e : g_cfg.commands) {
        if (e.fromStore) ++n;
    }
    return n;
}

static void CloseManager() {
    g_mgr = ManagerState();
    g_activator = -1;
    ResetBlink();
    UpdateTargets(false, /*followSelection=*/false);
    MarkDirty();
}

static void OpenManager() {
    g_mgr = ManagerState();
    g_mgr.active = true;
    g_activator = -1;
    g_text.clear();
    g_caret = g_anchor = 0;
    g_inputScroll = 0;
    CancelConfirm();
    ResetBlink();
    MarkDirty();
}

static void ManagerStartNew() {
    g_mgr.editing = true;
    g_mgr.creating = true;
    g_mgr.editId.clear();
    g_mgr.typeIdx = 0;
    for (auto& s : g_mgr.vals) s.clear();
    g_mgr.field = 0;
    g_mgr.caret = g_mgr.anchor = 0;
    g_mgr.inputScroll = 0;
    g_mgr.error.clear();
    ResetBlink();
    MarkDirty();
}

static void ManagerStartEdit(const CustomEntry& e) {
    g_mgr.editing = true;
    g_mgr.creating = false;
    g_mgr.editId = e.id;
    g_mgr.typeIdx = (e.type == L"run") ? 2 : (e.type == L"path") ? 1 : 0;
    g_mgr.vals[0] = e.name;
    g_mgr.vals[1].clear();
    g_mgr.vals[2] = e.target;
    g_mgr.vals[3] = e.args;
    g_mgr.vals[4] = e.keywords;
    g_mgr.vals[kMgrFieldTrigger] = e.trigger;
    g_mgr.vals[kMgrFieldHotkey] = e.hotkey;
    g_mgr.field = 0;
    g_mgr.caret = g_mgr.anchor = (int)g_mgr.vals[0].size();
    g_mgr.inputScroll = 0;
    g_mgr.error.clear();
    ResetBlink();
    MarkDirty();
}

// The focused text field's string, or nullptr when the Type selector (field 1) is focused.
static std::wstring* ManagerFieldText() {
    if (!g_mgr.active || !g_mgr.editing) return nullptr;
    if (g_mgr.field == kMgrFieldType || g_mgr.field < 0 || g_mgr.field >= kMgrFields) return nullptr;
    return &g_mgr.vals[g_mgr.field];
}

static void ManagerReplaceSelection(const std::wstring& s) {
    std::wstring* v = ManagerFieldText();
    if (!v) return;
    int a = std::min(g_mgr.anchor, g_mgr.caret);
    int b = std::max(g_mgr.anchor, g_mgr.caret);
    a = std::clamp(a, 0, (int)v->size());
    b = std::clamp(b, 0, (int)v->size());
    if (v->size() - (size_t)(b - a) + s.size() > 2048) return;
    *v = v->substr(0, a) + s + v->substr(b);
    g_mgr.caret = g_mgr.anchor = a + (int)s.size();
    g_mgr.error.clear();
    ResetBlink();
    MarkDirty();
}

static void ManagerMoveCaret(int pos, bool extend) {
    std::wstring* v = ManagerFieldText();
    if (!v) return;
    pos = std::clamp(pos, 0, (int)v->size());
    g_mgr.caret = pos;
    if (!extend) g_mgr.anchor = pos;
    ResetBlink();
    MarkDirty();
}

static void ManagerFocusField(int f) {
    f = std::clamp(f, 0, kMgrFields - 1);
    if (f == g_mgr.field) {
        return;
    }
    g_mgr.field = f;
    std::wstring* v = ManagerFieldText();
    g_mgr.caret = g_mgr.anchor = v ? (int)v->size() : 0;
    g_mgr.inputScroll = 0;
    ResetBlink();
    MarkDirty();
}

static void ManagerSave() {
    const std::wstring name = TrimBlanks(g_mgr.vals[0]);
    const std::wstring target = TrimBlanks(g_mgr.vals[2]);
    if (name.empty()) {
        g_mgr.error = L"Name is required.";
        ManagerFocusField(0);
        MarkDirty();
        return;
    }
    if (target.empty()) {
        g_mgr.error = L"Target is required.";
        ManagerFocusField(2);
        MarkDirty();
        return;
    }
    // Optional shortcuts: a typed alias and a key combination used while the bar is open.
    std::wstring trigger = TrimBlanks(g_mgr.vals[kMgrFieldTrigger]);
    for (auto& c : trigger) c = (wchar_t)towlower(c);
    std::wstring hotkeyText = TrimBlanks(g_mgr.vals[kMgrFieldHotkey]);
    const std::wstring selfId = g_mgr.creating ? std::wstring() : g_mgr.editId;
    if (!trigger.empty()) {
        std::wstring error;
        if (!ValidTrigger(trigger, selfId, error)) {
            g_mgr.error = error;
            ManagerFocusField(kMgrFieldTrigger);
            MarkDirty();
            return;
        }
    }
    if (!hotkeyText.empty()) {
        UINT hkMods = 0, hkVk = 0;
        if (!ParseHotkey(hotkeyText, hkMods, hkVk)) {
            g_mgr.error = L"Shortcut: press a combination like Ctrl+Alt+T.";
            ManagerFocusField(kMgrFieldHotkey);
            MarkDirty();
            return;
        }
        // The launcher's own shortcut still belongs to the system: it would close the bar before
        // the entry could see it.
        if (hkMods == g_cfg.hotkeyMods && hkVk == g_cfg.hotkeyVk) {
            g_mgr.error = L"Shortcut: the launcher's own key is already used.";
            ManagerFocusField(kMgrFieldHotkey);
            MarkDirty();
            return;
        }
        for (const CustomEntry& e : g_cfg.commands) {
            if (e.id == selfId || e.hotkey.empty()) continue;
            UINT m2 = 0, v2 = 0;
            if (ParseHotkey(e.hotkey, m2, v2) && m2 == hkMods && v2 == hkVk) {
                g_mgr.error = L"Shortcut: another entry already uses it.";
                ManagerFocusField(kMgrFieldHotkey);
                MarkDirty();
                return;
            }
        }
        hotkeyText = FormatHotkey(hkMods, hkVk);  // canonical spelling
    }
    std::vector<CustomEntry> store = ManagerStoreList();
    CustomEntry ne = MakeCustomEntry(name, kMgrTypeIds[g_mgr.typeIdx], target, g_mgr.vals[3],
                                     TrimBlanks(g_mgr.vals[4]), trigger, hotkeyText);
    ne.fromStore = true;
    std::wstring keepId;
    if (!g_mgr.creating && !g_mgr.editId.empty()) {
        ne.id = g_mgr.editId;
        keepId = ne.id;
        bool found = false;
        for (CustomEntry& e : store) {
            if (e.id == g_mgr.editId) {
                e = ne;
                found = true;
                break;
            }
        }
        if (!found) {
            ne.id = GenerateEntryId();
            keepId = ne.id;
            store.push_back(ne);
        }
    } else {
        ne.id = GenerateEntryId();
        keepId = ne.id;
        store.push_back(ne);
    }
    SaveCustomStore(store);
    LoadSettings();  // rebuild g_cfg.commands (settings entries + the updated store)
    g_mgr.editing = false;
    g_mgr.error.clear();
    // Leave the list selection on the entry that was just saved.
    g_mgr.sel = 1;
    int row = 1;
    for (const CustomEntry& e : g_cfg.commands) {
        if (!e.fromStore) continue;
        if (e.id == keepId) {
            g_mgr.sel = row;
            break;
        }
        ++row;
    }
    ResetBlink();
    MarkDirty();
}

static void ManagerDeleteSel() {
    if (g_mgr.sel <= 0) return;  // the "New entry" row
    std::vector<CustomEntry> store = ManagerStoreList();
    const int idx = g_mgr.sel - 1;
    if (idx < 0 || idx >= (int)store.size()) return;
    store.erase(store.begin() + idx);
    SaveCustomStore(store);
    LoadSettings();
    const int n = ManagerStoreCount();
    if (g_mgr.sel > n) g_mgr.sel = n;  // clamp to 0..n
    ResetBlink();
    MarkDirty();
}

static int ManagerVisibleRows() {
    const float s = (float)g_scale;
    const float avail = (float)(ExpandedH() - kFooterH) * s - (kBarH + 34.f) * s;
    const int rows = (int)(avail / (kRowH * s));
    return std::max(1, rows);
}

static void EnsureManagerSelVisible() {
    const int vis = ManagerVisibleRows();
    const int total = ManagerStoreCount() + 1;  // + the "New entry" row
    if (g_mgr.sel < g_mgr.scroll) g_mgr.scroll = g_mgr.sel;
    if (g_mgr.sel >= g_mgr.scroll + vis) g_mgr.scroll = g_mgr.sel - vis + 1;
    g_mgr.scroll = std::clamp(g_mgr.scroll, 0, std::max(0, total - vis));
}

static std::wstring ManagerClipboardText() {
    if (!OpenClipboard(g_hwnd)) return L"";
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
    return s;
}

// Captures a key combination for the Shortcut field. Returns false for the keys that keep their
// normal editor meaning (Esc, Enter, Tab, the arrows) so those are handled by the caller.
static bool ManagerCaptureHotkey(WPARAM vk, bool ctrl, bool shift) {
    switch (vk) {
        case VK_ESCAPE:
        case VK_TAB:
        case VK_UP:
        case VK_DOWN:
        case VK_RETURN:
            return false;
        case VK_BACK:
        case VK_DELETE:
            g_mgr.vals[kMgrFieldHotkey].clear();
            g_mgr.error.clear();
            MarkDirty();
            return true;
        case VK_CONTROL:
        case VK_MENU:
        case VK_SHIFT:
        case VK_LWIN:
        case VK_RWIN:
            return true;  // a modifier on its own: wait for the real key
        default: {
            UINT mods = 0;
            if (ctrl) mods |= MOD_CONTROL;
            if ((GetKeyState(VK_MENU) & 0x8000) != 0) mods |= MOD_ALT;
            if (shift) mods |= MOD_SHIFT;
            if ((GetKeyState(VK_LWIN) & 0x8000) != 0 || (GetKeyState(VK_RWIN) & 0x8000) != 0) {
                mods |= MOD_WIN;
            }
            if (mods == 0 || (mods & (MOD_CONTROL | MOD_ALT | MOD_WIN)) == 0) {
                g_mgr.error = L"Shortcut: hold Ctrl, Alt or Win as well.";
                MarkDirty();
                return true;
            }
            const std::wstring text = FormatHotkey(mods, (UINT)vk);
            if (text.empty()) return true;  // a key with no name here: keep waiting
            g_mgr.vals[kMgrFieldHotkey] = text;
            g_mgr.error.clear();
            MarkDirty();
            return true;
        }
    }
}

static bool HandleManagerKey(WPARAM vk, bool ctrl, bool shift) {
    if (g_mgr.editing) {
        if (g_mgr.field == kMgrFieldHotkey && ManagerCaptureHotkey(vk, ctrl, shift)) return true;
        std::wstring* v = ManagerFieldText();
        const int len = v ? (int)v->size() : 0;
        const bool sel = g_mgr.anchor != g_mgr.caret;
        switch (vk) {
            case VK_ESCAPE:
                g_mgr.editing = false;  // back to the list
                g_mgr.error.clear();
                ResetBlink();
                MarkDirty();
                return true;
            case VK_RETURN:
                ManagerSave();
                return true;
            case VK_TAB:
                ManagerFocusField((g_mgr.field + (shift ? -1 : 1) + kMgrFields) % kMgrFields);
                return true;
            case VK_UP:
                ManagerFocusField(g_mgr.field - 1);
                return true;
            case VK_DOWN:
                ManagerFocusField(g_mgr.field + 1);
                return true;
            case VK_LEFT:
                if (g_mgr.field == kMgrFieldType) {
                    g_mgr.typeIdx = (g_mgr.typeIdx + 2) % 3;
                    ResetBlink();
                    MarkDirty();
                    return true;
                }
                if (sel && !shift) ManagerMoveCaret(std::min(g_mgr.anchor, g_mgr.caret), false);
                else ManagerMoveCaret(g_mgr.caret - 1, shift);
                return true;
            case VK_RIGHT:
                if (g_mgr.field == kMgrFieldType) {
                    g_mgr.typeIdx = (g_mgr.typeIdx + 1) % 3;
                    ResetBlink();
                    MarkDirty();
                    return true;
                }
                if (sel && !shift) ManagerMoveCaret(std::max(g_mgr.anchor, g_mgr.caret), false);
                else ManagerMoveCaret(g_mgr.caret + 1, shift);
                return true;
            case VK_HOME:
                ManagerMoveCaret(0, shift);
                return true;
            case VK_END:
                ManagerMoveCaret(len, shift);
                return true;
            case VK_BACK:
                if (sel) {
                    ManagerReplaceSelection(L"");
                } else if (g_mgr.caret > 0) {
                    g_mgr.anchor = g_mgr.caret - 1;
                    ManagerReplaceSelection(L"");
                }
                return true;
            case VK_DELETE:
                if (sel) {
                    ManagerReplaceSelection(L"");
                } else if (g_mgr.caret < len) {
                    g_mgr.anchor = g_mgr.caret + 1;
                    ManagerReplaceSelection(L"");
                }
                return true;
            case 'A':
                if (ctrl && v) {
                    g_mgr.anchor = 0;
                    g_mgr.caret = len;
                    ResetBlink();
                    MarkDirty();
                }
                return true;
            case 'C':
            case 'X':
                if (ctrl && v && sel) {
                    const int a = std::min(g_mgr.anchor, g_mgr.caret);
                    const int b = std::max(g_mgr.anchor, g_mgr.caret);
                    CopyTextToClipboard(v->substr(a, b - a));
                    if (vk == 'X') ManagerReplaceSelection(L"");
                }
                return true;
            case 'V':
                if (ctrl) {
                    std::wstring t = ManagerClipboardText();
                    if (!t.empty()) ManagerReplaceSelection(t);
                }
                return true;
        }
        return false;  // unrecognised key: let WM_CHAR deliver the character
    }

    // List mode.
    const int n = ManagerStoreCount();
    switch (vk) {
        case VK_UP:
            g_mgr.deleteArmed = false;
            g_mgr.sel = std::clamp(g_mgr.sel - 1, 0, n);
            EnsureManagerSelVisible();
            ResetBlink();
            MarkDirty();
            return true;
        case VK_DOWN:
            g_mgr.deleteArmed = false;
            g_mgr.sel = std::clamp(g_mgr.sel + 1, 0, n);
            EnsureManagerSelVisible();
            ResetBlink();
            MarkDirty();
            return true;
        case VK_RETURN:
            g_mgr.deleteArmed = false;
            if (g_mgr.sel <= 0) {
                ManagerStartNew();
            } else {
                std::vector<CustomEntry> store = ManagerStoreList();
                const int idx = g_mgr.sel - 1;
                if (idx >= 0 && idx < (int)store.size()) ManagerStartEdit(store[idx]);
            }
            return true;
        case VK_DELETE:
            if (g_mgr.sel <= 0) return true;  // "New entry" row is not deletable
            if (!g_mgr.deleteArmed) {
                g_mgr.deleteArmed = true;  // first Delete: arm and show confirm hint
                MarkDirty();
            } else {
                g_mgr.deleteArmed = false;  // second Delete: execute
                ManagerDeleteSel();
            }
            return true;
        case VK_ESCAPE:
            if (g_mgr.deleteArmed) {
                g_mgr.deleteArmed = false;
                MarkDirty();
                return true;
            }
            CloseManager();
            return true;
    }
    return true;  // the manager is modal
}

// ---- Drawing the manager ----

static D2D1_COLOR_F ManagerTypeColor(int typeIdx) {
    return typeIdx == 0 ? D2D1::ColorF(0.20f, 0.55f, 0.90f)
           : typeIdx == 2 ? D2D1::ColorF(0.55f, 0.38f, 0.88f)
                          : D2D1::ColorF(0.92f, 0.62f, 0.22f);
}

static CmdGlyph ManagerTypeGlyph(int typeIdx) {
    return typeIdx == 0 ? CmdGlyph::Link : typeIdx == 2 ? CmdGlyph::Terminal
                                                                : CmdGlyph::Folder;
}

static void DrawManagerList(float cx, float cy, float cw, float s) {
    const float x0 = cx + 20 * s;
    const float x1 = cx + cw - 20 * s;
    const float listTop = cy + kBarH * s;
    const float rowsTop = listTop + 34 * s;
    const float rowH = kRowH * s;
    const float footerTop = cy + (float)(ExpandedH() - kFooterH) * s;

    SetBrush(g_pal.separator);
    D2D1_RECT_F sep = D2D1::RectF(cx + 12 * s, listTop, cx + cw - 12 * s, listTop + 1.f);
    g_rt->FillRectangle(&sep, g_brush);

    DrawLabel(L"Custom entries", g_fmtTitle, x0, listTop + 6 * s, x1 - 120 * s, listTop + 30 * s,
              g_pal.text, 1.f);
    const int n = ManagerStoreCount();
    {
        wchar_t c[48];
        swprintf_s(c, n == 1 ? L"%d entry" : L"%d entries", n);
        const float cwt = TextWidth(c, g_fmtMeta);
        DrawLabel(c, g_fmtMeta, x1 - cwt, listTop + 6 * s, x1, listTop + 30 * s, g_pal.textDim, 1.f);
    }

    const int vis = ManagerVisibleRows();
    const int total = n + 1;
    const D2D1_RECT_F clip = D2D1::RectF(cx + 8 * s, rowsTop, cx + cw - 8 * s, footerTop - 4 * s);
    g_rt->PushAxisAlignedClip(&clip, D2D1_ANTIALIAS_MODE_ALIASED);
    const std::vector<CustomEntry> store = ManagerStoreList();
    const int last = std::min(total, g_mgr.scroll + vis);
    for (int i = g_mgr.scroll; i < last; ++i) {
        const float y = rowsTop + (float)(i - g_mgr.scroll) * rowH;
        const bool on = (i == g_mgr.sel);
        const bool armedRow = on && g_mgr.deleteArmed;
        if (on) FillRR(x0 - 8 * s, y + 2 * s, x1 + 8 * s, y + rowH - 2 * s, kRowRadius * s,
                       armedRow ? D2D1::ColorF(0.88f, 0.22f, 0.22f) : g_pal.selection,
                       armedRow ? 0.22f : 1.f);
        const float iconSz = 20 * s;
        const float iconY = y + (rowH - iconSz) / 2;
        if (i == 0) {
            // "New entry" row.
            FillRR(x0, iconY, x0 + iconSz, iconY + iconSz, 6 * s, g_pal.accent, 0.16f);
            DrawCommandGlyph(CmdGlyph::ChevronRight, x0, iconY, iconSz, g_pal.accentStrong);
            DrawLabel(L"New entry", g_fmtTitle, x0 + 30 * s, y, x1, y + rowH, g_pal.accentStrong,
                      1.f);
        } else {
            const int si = i - 1;
            if (si < 0 || si >= (int)store.size()) continue;
            const CustomEntry& e = store[si];
            const int ti = (e.type == L"run") ? 2 : (e.type == L"path") ? 1 : 0;
            const D2D1_COLOR_F ct = ManagerTypeColor(ti);
            FillRR(x0, iconY, x0 + iconSz, iconY + iconSz, 6 * s, ct);
            DrawCommandGlyph(ManagerTypeGlyph(ti), x0, iconY, iconSz, ct);
            DrawLabel(e.name, g_fmtTitle, x0 + 30 * s, y + 4 * s, x1 - 8 * s, y + rowH / 2 + 4 * s,
                      g_pal.text, 1.f);
            std::wstring sub = kMgrTypeLabels[ti];
            if (!e.trigger.empty()) sub += L"  ·  /" + e.trigger;
            if (!e.hotkey.empty()) sub += L"  ·  " + e.hotkey;
            if (!e.target.empty()) sub += L"  ·  " + e.target;
            DrawLabel(FitEnd(sub, g_fmtMeta, x1 - (x0 + 30 * s) - 8 * s), g_fmtMeta, x0 + 30 * s,
                      y + rowH / 2 - 2 * s, x1 - 8 * s, y + rowH - 4 * s, g_pal.textDim, 1.f);
        }
    }
    g_rt->PopAxisAlignedClip();
}

static void DrawManagerEditor(float cx, float cy, float cw, float s) {
    const float x0 = cx + 20 * s;
    const float x1 = cx + cw - 20 * s;
    const float listTop = cy + kBarH * s;
    const float rowsTop = listTop + 34 * s;
    const float footerTop = cy + (float)(ExpandedH() - kFooterH) * s;
    // Seven rows do not fit at the form's row height, so they shrink into the space above the
    // hint line instead of scrolling: every field stays visible at once.
    const float rowSpace = (footerTop - 30 * s) - rowsTop;
    const float rowH = std::min(kFormRowH * s, rowSpace / (float)kMgrFields);

    SetBrush(g_pal.separator);
    D2D1_RECT_F sep = D2D1::RectF(cx + 12 * s, listTop, cx + cw - 12 * s, listTop + 1.f);
    g_rt->FillRectangle(&sep, g_brush);
    DrawLabel(g_mgr.creating ? L"New entry" : L"Edit entry", g_fmtTitle, x0, listTop + 6 * s,
              x1, listTop + 30 * s, g_pal.text, 1.f);

    const float labelW = 96 * s;
    const float boxL = x0 + 14 * s + labelW;
    for (int i = 0; i < kMgrFields; ++i) {
        const float y = rowsTop + (float)i * rowH;
        const bool on = (i == g_mgr.field);
        const D2D1_RECT_F vr = D2D1::RectF(boxL, y + 7 * s, x1 - 12 * s, y + rowH - 7 * s);
        FillRR(vr.left, vr.top, vr.right, vr.bottom, 7 * s, on ? g_pal.accent : g_pal.keycap,
               on ? 0.12f : 0.45f);
        if (on) {
            SetBrush(g_pal.accent, 0.5f);
            D2D1_ROUNDED_RECT rr = D2D1::RoundedRect(
                D2D1::RectF(vr.left + 0.5f, vr.top + 0.5f, vr.right - 0.5f, vr.bottom - 0.5f),
                7 * s, 7 * s);
            g_rt->DrawRoundedRectangle(&rr, g_brush, 1.f);
        }
        DrawLabel(kMgrFieldLabels[i], g_fmtFormLabel, x0 + 14 * s, y, x0 + 14 * s + labelW,
                  y + rowH, on ? g_pal.text : g_pal.textDim, 1.f);
        const float vx0 = vr.left + 8 * s;
        const float vx1 = vr.right - 8 * s;
        if (i == kMgrFieldType) {
            // Type selector: ‹ label ›
            std::wstring t = std::wstring(L"‹  ") + kMgrTypeLabels[g_mgr.typeIdx] + L"  ›";
            DrawLabel(t, g_fmtForm, vx0, y, vx1, y + rowH, g_pal.text, 1.f);
            continue;
        }
        const std::wstring& val = g_mgr.vals[i];
        const bool shortcutField = i == kMgrFieldTrigger || i == kMgrFieldHotkey;
        if (val.empty() && (!on || shortcutField)) {
            const wchar_t* hint =
                (i == kMgrFieldArgs || i == kMgrFieldKeywords || shortcutField) ? L"Optional"
                                                                                : L"Required";
            if (i == kMgrFieldTrigger && on) hint = L"Type a word, e.g. yt";
            if (i == kMgrFieldHotkey && on) hint = L"Press a combination";
            DrawLabel(hint, g_fmtForm, vx0, y, vx1, y + rowH, g_pal.textDim, 0.5f);
            continue;
        }
        // Caret and horizontal scroll via text measurement.
        float caretX = 0, anchorX = 0;
        if (on) {
            const int c = std::clamp(g_mgr.caret, 0, (int)val.size());
            const int a = std::clamp(g_mgr.anchor, 0, (int)val.size());
            caretX = TextWidth(val.substr(0, c).c_str(), g_fmtForm);
            anchorX = TextWidth(val.substr(0, a).c_str(), g_fmtForm);
            const float avail = vx1 - vx0;
            if (caretX - g_mgr.inputScroll > avail - 4 * s) g_mgr.inputScroll = caretX - (avail - 4 * s);
            if (caretX - g_mgr.inputScroll < 0) g_mgr.inputScroll = caretX;
            if (g_mgr.inputScroll < 0) g_mgr.inputScroll = 0;
        }
        const float ox = vx0 - (on ? g_mgr.inputScroll : 0.f);
        const D2D1_RECT_F vclip = D2D1::RectF(vx0, y, vx1 + 2 * s, y + rowH);
        g_rt->PushAxisAlignedClip(&vclip, D2D1_ANTIALIAS_MODE_ALIASED);
        if (on && g_mgr.anchor != g_mgr.caret) {
            SetBrush(g_pal.caretSel);
            D2D1_RECT_F selr = D2D1::RectF(ox + std::min(anchorX, caretX), vr.top + 4 * s,
                                           ox + std::max(anchorX, caretX), vr.bottom - 4 * s);
            g_rt->FillRectangle(&selr, g_brush);
        }
        DrawLabel(val, g_fmtForm, ox, y, ox + 10000 * s, y + rowH, g_pal.text, 1.f);
        if (on && g_caretOn) {
            SetBrush(g_pal.text);
            D2D1_RECT_F cr = D2D1::RectF(ox + caretX, y + 9 * s, ox + caretX + 1.5f * s,
                                         y + rowH - 9 * s);
            g_rt->FillRectangle(&cr, g_brush);
        }
        g_rt->PopAxisAlignedClip();
    }

    // Detected placeholders + error, just above the footer.
    const float py = footerTop - 30 * s;
    if (!g_mgr.error.empty()) {
        FillRR(x0, py, x1, py + 26 * s, 7 * s, D2D1::ColorF(0.93f, 0.30f, 0.30f), 0.10f);
        DrawLabel(FitEnd(g_mgr.error, g_fmtMeta, x1 - x0 - 16 * s), g_fmtMeta, x0 + 8 * s, py, x1,
                  py + 26 * s, D2D1::ColorF(0.96f, 0.55f, 0.55f), 1.f);
    } else {
        std::vector<CustomField> fs;
        CollectCustomFields(g_mgr.vals[2], fs);
        CollectCustomFields(g_mgr.vals[3], fs);
        std::wstring ph;
        for (const CustomField& f : fs) {
            if (!ph.empty()) ph += L", ";
            ph += L"{" + f.key + L"}";
        }
        const std::wstring line =
            ph.empty() ? std::wstring(L"No fields — launches directly.")
                       : std::wstring(L"Fields: ") + ph;
        DrawLabel(FitEnd(line, g_fmtMeta, x1 - x0), g_fmtMeta, x0 + 2 * s, py, x1, py + 26 * s,
                  g_pal.textDim, 0.85f);
    }
}

static void DrawManager(float cx, float cy, float cw, float s) {
    if (!g_mgr.active) return;
    if (g_mgr.editing) DrawManagerEditor(cx, cy, cw, s);
    else DrawManagerList(cx, cy, cw, s);
}

static void DrawManagerFooter(float cx, float footerTop, float cw) {
    const float s = (float)g_scale;
    SetBrush(g_pal.separator);
    D2D1_RECT_F sep = D2D1::RectF(cx + 12 * s, footerTop, cx + cw - 12 * s, footerTop + 1.f);
    g_rt->FillRectangle(&sep, g_brush);
    const float yMid = footerTop + kFooterH * s / 2 + 0.5f;
    const wchar_t* ok[1] = {kTxtEnterGlyph};
    const wchar_t* esc[1] = {L"Esc"};
    const wchar_t* tab[1] = {L"Tab"};
    const wchar_t* del[1] = {L"Del"};
    float x = cx + cw - 20 * s;
    if (g_mgr.editing) {
        x = DrawHint(L"Save", ok, 1, x, yMid);
        x -= 16 * s;
        x = DrawHint(L"Next", tab, 1, x, yMid);
        x -= 16 * s;
        DrawHint(L"Back", esc, 1, x, yMid);
    } else {
        if (g_mgr.deleteArmed) {
            x = DrawHint(kTxtConfirm, del, 1, x, yMid);
            x -= 16 * s;
            DrawHint(L"Cancel", esc, 1, x, yMid);
        } else {
            x = DrawHint(g_mgr.sel <= 0 ? L"New" : L"Edit", ok, 1, x, yMid);
            x -= 16 * s;
            if (g_mgr.sel > 0) {
                x = DrawHint(L"Delete", del, 1, x, yMid);
                x -= 16 * s;
            }
            DrawHint(L"Close", esc, 1, x, yMid);
        }
    }
}

// Full path of a System32 executable, so CreateProcess cannot pick up a same-named file from
// the working directory or the search path.
static std::wstring SystemExe(const wchar_t* name) {
    wchar_t dir[MAX_PATH];
    UINT n = GetSystemDirectoryW(dir, MAX_PATH);
    if (n == 0 || n >= MAX_PATH) return name;
    std::wstring path = dir;
    path += L'\\';
    path += name;
    return path;
}

// %LOCALAPPDATA%\Microsoft\WindowsApps\<name>, where Windows puts the app execution aliases. Used
// instead of a bare "wt.exe" so a same-named file in the folder being browsed cannot be picked.
static std::wstring LocalAppAlias(const wchar_t* name) {
    PWSTR base = nullptr;
    if (FAILED(SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, nullptr, &base)) || !base) {
        return std::wstring();
    }
    const std::wstring path = std::wstring(base) + L"\\Microsoft\\WindowsApps\\" + name;
    CoTaskMemFree(base);
    return GetFileAttributesW(path.c_str()) == INVALID_FILE_ATTRIBUTES ? std::wstring() : path;
}

// Full path of a file in the Windows folder (explorer.exe and friends live there, not in System32).
static std::wstring WindowsFile(const wchar_t* name) {
    wchar_t dir[MAX_PATH];
    const UINT n = GetWindowsDirectoryW(dir, MAX_PATH);
    if (n == 0 || n >= MAX_PATH) return name;
    return std::wstring(dir) + L'\\' + name;
}

// Runs an executable with arguments, without flashing a console window.
static void RunConsoleHidden(const wchar_t* exe, const wchar_t* args) {
    HideLauncher();
    wchar_t cmd[512];
    swprintf_s(cmd, L"\"%s\" %s", exe, args);
    STARTUPINFOW si = {};
    si.cb = sizeof(si);
    PROCESS_INFORMATION pi = {};
    if (CreateProcessW(nullptr, cmd, nullptr, nullptr, FALSE, CREATE_NO_WINDOW, nullptr, nullptr,
                       &si, &pi)) {
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
}

// Runs a built-in command by its stable id (see kCommands).
// Switches into a mode from the result list, with the same effect as typing its prefix: this is
// what the mode openers ("Open Windows", "Bookmarks" ...) do on Enter.
static void EnterActivator(int ai) {
    if (ai < 0 || ai >= kActivatorCount) return;
    g_text = kActivators[ai].trigger;
    g_text += L' ';
    g_caret = g_anchor = (int)g_text.size();
    OnTextChanged();
}

static void RunCommand(const std::wstring& id) {
    if (id == L"lock") {
        HideLauncher();
        LockWorkStation();
    } else if (id == L"sleep") {
        HideLauncher();
        // Loaded dynamically so the mod does not need to link against PowrProf.lib.
        typedef BOOLEAN(WINAPI * SuspendFn)(BOOLEAN, BOOLEAN, BOOLEAN);
        HMODULE h = LoadLibraryExW(L"powrprof.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
        if (h) {
            auto fn = (SuspendFn)GetProcAddress(h, "SetSuspendState");
            if (fn) fn(FALSE, FALSE, FALSE);
            FreeLibrary(h);
        }
    } else if (id == L"hibernate") {
        RunConsoleHidden(SystemExe(L"shutdown.exe").c_str(), L"/h");
    } else if (id == L"shutdown") {
        RunConsoleHidden(SystemExe(L"shutdown.exe").c_str(), L"/s /t 0");
    } else if (id == L"restart") {
        RunConsoleHidden(SystemExe(L"shutdown.exe").c_str(), L"/r /t 0");
    } else if (id == L"signout") {
        RunConsoleHidden(SystemExe(L"shutdown.exe").c_str(), L"/l");
    } else if (id == L"emptyrecyclebin") {
        HideLauncher();
        SHEmptyRecycleBinW(nullptr, nullptr,
                           SHERB_NOCONFIRMATION | SHERB_NOPROGRESSUI | SHERB_NOSOUND);
    } else if (id == L"taskmgr") {
        RunShellOpen(SystemExe(L"taskmgr.exe").c_str());
    } else if (id == L"control") {
        RunShellOpen(SystemExe(L"control.exe").c_str());
    } else if (id == L"devmgmt") {
        RunShellOpen(SystemExe(L"devmgmt.msc").c_str());
    } else if (id == L"services") {
        RunShellOpen(SystemExe(L"services.msc").c_str());
    } else if (id == L"regedit") {
        RunShellOpen(SystemExe(L"regedit.exe").c_str());
    } else if (id == L"netconnections") {
        RunShellOpen(SystemExe(L"ncpa.cpl").c_str());
    } else if (id == L"settings") {
        RunShellOpen(L"ms-settings:");
    } else if (id == L"windowsupdate") {
        RunShellOpen(L"ms-settings:windowsupdate");
    } else if (id == L"installedapps") {
        RunShellOpen(L"ms-settings:appsfeatures");
    } else if (id == L"display") {
        RunShellOpen(L"ms-settings:display");
    } else if (id == L"sound") {
        RunShellOpen(L"ms-settings:sound");
    } else if (id == L"bluetooth") {
        RunShellOpen(L"ms-settings:bluetooth");
    } else if (id == L"printers") {
        RunShellOpen(L"ms-settings:printers");
    } else if (id == L"terminal") {
        // Windows Terminal through its app alias, falling back to PowerShell from System32.
        HideLauncher();
        AllowSetForegroundWindow(ASFW_ANY);
        const std::wstring wt = LocalAppAlias(L"wt.exe");
        if (wt.empty() || (INT_PTR)ShellExecuteW(nullptr, L"open", wt.c_str(), nullptr, nullptr,
                                                SW_SHOWNORMAL) <= 32) {
            const std::wstring ps = SystemExe(L"powershell.exe");
            ShellExecuteW(nullptr, L"open", ps.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
        }
    } else if (id == L"explorer") {
        RunShellOpen(WindowsFile(L"explorer.exe").c_str());
    } else if (id == L"customentries") {
        OpenManager();  // in-app manager; does not hide the launcher
    } else if (id == L"clearclipboard") {
        const int keep = g_sel;
        EraseClipHistory();  // entries and their image files
        RunSearch();
        g_sel = std::clamp(keep, 0, std::max(0, (int)g_results.size() - 1));
        UpdateTargets(false, /*followSelection=*/true);
        MarkDirty();
    }
}

// ===== File navigation (folders, history, deep search) =====

static void CancelFileSearch() {
    InterlockedIncrement(&g_fileSearchGen);
    g_fileSearching = false;
    if (g_hwnd) KillTimer(g_hwnd, TIMER_FSEARCH);
}

// Called after the query changes: shows local matches now and starts the deep search shortly.
static void ScheduleFileSearch() {
    if (!g_fileMode || g_activator >= 0 || !FileQueryActive() || InPcRoot()) {
        CancelFileSearch();
        return;
    }
    if (g_fileDeepDone && FileSearchKey() == g_fileDeepKey) {
        CancelFileSearch();
        return;
    }
    InterlockedIncrement(&g_fileSearchGen);
    g_fileSearching = true;
    if (g_hwnd) SetTimer(g_hwnd, TIMER_FSEARCH, 140, nullptr);
}

static void StartFileSearch() {
    if (!g_fileMode || !FileQueryActive() || g_fileCur.empty() || InPcRoot()) {
        g_fileSearching = false;
        return;
    }
    if (g_cfg.fileSearchEngine == FileSearchEngine::Everything && StartEverythingFileSearch()) {
        return;  // the index answers for the whole subtree
    }
    auto* job = new FileSearchJob();
    job->hwnd = g_hwnd;
    job->gen = InterlockedIncrement(&g_fileSearchGen);
    job->root = g_fileCur;
    job->key = FileSearchKey();
    job->tokens = g_tokens;
    job->filter = g_fileFilter;
    FileSearchStarted();
    HANDLE th = CreateThread(nullptr, 0, FileSearchThread, job, 0, nullptr);
    if (!th) {
        FileSearchFinished();
        delete job;
        g_fileSearching = false;
        return;
    }
    // The search must never make typing or scrolling feel slower.
    SetThreadPriority(th, THREAD_PRIORITY_BELOW_NORMAL);
    CloseHandle(th);
}

static void FileGoTo(const std::wstring dir, bool record, const std::wstring select) {
    if (dir.empty()) return;
    if (dir != kPcRoot) {  // the drives view is synthetic; every real folder must exist
        const DWORD at = GetFileAttributesW(dir.c_str());
        if (at == INVALID_FILE_ATTRIBUTES || !(at & FILE_ATTRIBUTE_DIRECTORY)) return;
    }
    if (record && !g_fileCur.empty() && !SamePath(dir, g_fileCur)) {
        g_fileBack.push_back(g_fileCur);
        if (g_fileBack.size() > 64) g_fileBack.erase(g_fileBack.begin());
        g_fileFwd.clear();
    }
    g_bandEdit = BandEdit::None;
    g_fileMenuOpen = false;
    CancelFileSearch();
    ClearFileDeep();
    g_fileCur = dir;
    g_fileLoc = LocationForPath(g_fileCur);
    if (g_fileLoc >= 0) g_chipSelTarget = g_fileLoc;
    g_fileDir.clear();
    g_pendingSelect = select;
    g_text.clear();
    g_caret = g_anchor = 0;
    g_inputScroll = 0;
    g_calc = Calc::Result();
    g_calcDisplay.clear();
    g_calcActive = false;
    g_calcPending = false;
    if (g_hwnd) KillTimer(g_hwnd, TIMER_CALC);
    CancelConfirm();
    g_sel = 0;
    g_scrollTarget = g_scrollPos = 0;
    g_kbScroll = false;
    RunSearch();
    if (!g_pendingSelect.empty()) {
        for (size_t i = 0; i < g_results.size(); ++i) {
            if (SamePath(g_results[i].payload, g_pendingSelect)) {
                g_sel = (int)i;
                break;
            }
        }
        g_pendingSelect.clear();
    }
    UpdateTargets(false, /*followSelection=*/true);
    ResetBlink();
    MarkDirty();
}

static void FileBack() {
    if (g_fileBack.empty()) return;
    const std::wstring prev = g_fileBack.back();
    g_fileBack.pop_back();
    const std::wstring from = g_fileCur;
    g_fileFwd.push_back(from);
    FileGoTo(prev, false, from);
}

static void FileForward() {
    if (g_fileFwd.empty()) return;
    const std::wstring next = g_fileFwd.back();
    g_fileFwd.pop_back();
    const std::wstring from = g_fileCur;
    g_fileBack.push_back(from);
    FileGoTo(next, false, L"");
}

static void FileUp() {
    if (InPcRoot()) return;  // already at the top
    const std::wstring parent = ParentDir(g_fileCur);
    if (parent.empty()) {
        // At a drive root: step up to the drives overview instead of going nowhere.
        FileGoTo(kPcRoot, true, g_fileCur);
        return;
    }
    FileGoTo(parent, true, g_fileCur);
}

// A chip jump (Ctrl+1..9, Ctrl+Left/Right, click).
static void SetFileLocation(int loc) {
    if (loc < 0 || loc >= (int)g_chips.size()) return;
    FileGoTo(g_chips[loc].path, true, L"");
}

// ---- Drives ----
// Roots of the fixed and removable drives worth showing, in A..Z order. A removable or optical
// drive is listed only when a volume is actually present, so touching it never raises a "there is
// no disk" dialog; network and unknown drives are left out. The error mode is set per thread (not
// SetErrorMode, which would change it for the whole host process).
// GetDriveTypeW (and the media check below) can block on a disconnected mapped drive or a spun-down
// device, and this runs when the file browser opens, so the answer is kept for a few seconds.
static std::vector<std::wstring> ReadyDrives() {
    static std::vector<std::wstring> cached;
    static double cachedAt = -1e9;
    if (!cached.empty() && NowSec() - cachedAt < 5.0) return cached;
    cached.clear();
    const DWORD mask = GetLogicalDrives();
    DWORD prevMode = 0;
    SetThreadErrorMode(SEM_FAILCRITICALERRORS, &prevMode);
    for (int i = 0; i < 26; ++i) {
        if (!(mask & (1u << i))) continue;
        wchar_t root[4] = {(wchar_t)(L'A' + i), L':', L'\\', 0};
        const UINT type = GetDriveTypeW(root);
        if (type != DRIVE_FIXED && type != DRIVE_REMOVABLE && type != DRIVE_CDROM) continue;
        if (type == DRIVE_REMOVABLE || type == DRIVE_CDROM) {
            // Only include these when media is inserted; the call fails quietly otherwise.
            if (!GetVolumeInformationW(root, nullptr, 0, nullptr, nullptr, nullptr, nullptr, 0)) {
                continue;
            }
        }
        cached.push_back(root);
    }
    SetThreadErrorMode(prevMode, nullptr);
    cachedAt = NowSec();
    return cached;
}

// Rebuilds the chip row: the six profile locations, then one chip per ready drive. Called when
// entering file mode, so the drive list reflects what is plugged in without scanning every frame.
static void RebuildFileChips() {
    g_chips.clear();
    for (int i = 0; i < kFileLocationCount; ++i) {
        g_chips.push_back({kFileLocations[i].label, ResolveFileLocation(i), kFileLocations[i].glyph,
                           false});
    }
    // Drive chips go on their own second row. With a single drive they add nothing (it is the one
    // you are already on), so they appear only when there are two or more.
    const std::vector<std::wstring> drives = ReadyDrives();
    if (drives.size() >= 2) {
        for (const std::wstring& root : drives) {
            g_chips.push_back({root.substr(0, 2), root, CmdGlyph::Drive, true});  // "C:" -> "C:\"
        }
    }
}

// Drive root ("X:\\") the browser should open on the first time it is used in a session, or "" to
// use Home. The letter comes from settings; an empty or missing letter, or a drive that is not
// there (an unplugged USB stick), falls back to Home with no prompt.
static std::wstring StartDrivePath() {
    if (!g_cfg.startDrive) return L"";
    wchar_t root[4] = {g_cfg.startDrive, L':', L'\\', 0};
    DWORD prevMode = 0;
    SetThreadErrorMode(SEM_FAILCRITICALERRORS, &prevMode);
    const DWORD at = GetFileAttributesW(root);
    SetThreadErrorMode(prevMode, nullptr);
    if (at == INVALID_FILE_ATTRIBUTES || !(at & FILE_ATTRIBUTE_DIRECTORY)) return L"";
    return root;
}

// Ctrl+Tab hops between the drive chips only (the profile locations are a separate idea); dir is
// +1 for the next drive, -1 for the previous. Nothing happens when there are no drive chips.
static void CycleDrive(int dir) {
    std::vector<int> drives;  // indices of the drive chips, in order
    for (int i = 0; i < (int)g_chips.size(); ++i) {
        if (g_chips[i].isDrive) drives.push_back(i);
    }
    if (drives.empty()) return;
    // Which drive chip the current folder sits on (-1 when browsing a profile location or a UNC).
    const int here = LocationForPath(g_fileCur);
    int at = -1;
    for (int k = 0; k < (int)drives.size(); ++k) {
        if (drives[k] == here) {
            at = k;
            break;
        }
    }
    const int m = (int)drives.size();
    const int next = (at < 0) ? (dir > 0 ? 0 : m - 1) : (((at + dir) % m) + m) % m;
    if (at >= 0 && next == at) return;  // a single drive: nothing to switch to
    SetFileLocation(drives[next]);
}

static bool SelectedFileIsDir() {
    if (g_sel < 0 || g_sel >= (int)g_results.size()) return false;
    const SearchResult& r = g_results[g_sel];
    if (r.kind != ResultKind::File || r.icon < 0 || r.icon >= (int)FileSrc().size()) return false;
    return FileSrc()[r.icon].isDir;
}

// Shows the selected file or folder in Windows Explorer.
static void FileReveal() {
    if (g_sel < 0 || g_sel >= (int)g_results.size()) return;
    const SearchResult& r = g_results[g_sel];
    if (r.kind != ResultKind::File) return;
    const bool dir = SelectedFileIsDir();
    const std::wstring path = r.payload;
    HideLauncher();
    AllowSetForegroundWindow(ASFW_ANY);
    // Opened through the shell so the default file manager handles it, not explorer.exe.
    const std::wstring folder = dir ? path : ParentDir(path);
    if (!folder.empty()) {
        ShellExecuteW(nullptr, L"open", folder.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
    }
}

// ===== Path field, rename and file actions =====

static void CopyTextToClipboard(const std::wstring& s);  // defined further down

static void BandFail(const wchar_t* msg) {
    g_bandError = msg;
    g_bandErrorAt = NowSec();
    ResetBlink();
    MarkDirty();
}

static const FileEntry* SelectedFileEntry() {
    if (g_sel < 0 || g_sel >= (int)g_results.size()) return nullptr;
    const SearchResult& r = g_results[g_sel];
    if (r.kind != ResultKind::File || r.icon < 0 || r.icon >= (int)FileSrc().size()) return nullptr;
    return &FileSrc()[r.icon];
}

// Remembers the highlighted entry as the target of the next action.
static bool CaptureFileTarget() {
    const FileEntry* e = SelectedFileEntry();
    g_fileMenuHasTarget = e != nullptr;
    if (e) g_fileMenuTarget = *e;
    return e != nullptr;
}

static bool IsExeLike(const std::wstring& name) {
    const std::wstring n = FoldString(name);
    return NameEndsWithExt(n, L"exe") || NameEndsWithExt(n, L"bat") || NameEndsWithExt(n, L"cmd") ||
           NameEndsWithExt(n, L"msi") || NameEndsWithExt(n, L"com") || NameEndsWithExt(n, L"lnk");
}

// Resolves what was typed in the path field: ~, %VARS%, "quotes", / slashes, ".." and paths
// relative to the folder being browsed.
static std::wstring AbsFromInput(std::wstring in) {
    auto ws = [](wchar_t c) { return c == L' ' || c == L'\t'; };
    while (!in.empty() && ws(in.back())) in.pop_back();
    size_t a = 0;
    while (a < in.size() && ws(in[a])) ++a;
    in.erase(0, a);
    in.erase(std::remove(in.begin(), in.end(), L'"'), in.end());
    for (auto& c : in) {
        if (c == L'/') c = L'\\';
    }
    if (in.empty()) return g_fileCur;
    if (in[0] == L'~' && (in.size() == 1 || in[1] == L'\\')) {
        const std::wstring home = ResolveFileLocation(0);
        in = home + in.substr(1);
    }
    if (in.find(L'%') != std::wstring::npos) {
        wchar_t buf[2048];
        const DWORD n = ExpandEnvironmentStringsW(in.c_str(), buf, 2048);
        if (n > 0 && n <= 2048) in = buf;
    }
    if (in.size() == 2 && in[1] == L':') in += L'\\';
    const bool absolute = (in.size() >= 3 && in[1] == L':' && in[2] == L'\\') || in[0] == L'\\';
    if (!absolute) in = JoinPath(g_fileCur, in);
    std::wstring out(1024, L'\0');
    DWORD n = GetFullPathNameW(in.c_str(), (DWORD)out.size(), &out[0], nullptr);
    if (n >= out.size()) {
        out.assign(n + 1, L'\0');
        n = GetFullPathNameW(in.c_str(), (DWORD)out.size(), &out[0], nullptr);
    }
    if (n == 0) return in;
    out.resize(n);
    while (out.size() > 3 && out.back() == L'\\') out.pop_back();
    return out;
}

static void BeginPathEdit() {
    if (!g_fileMode || g_fileCur.empty() || InPcRoot()) return;  // nothing to type in the drives view
    g_fileMenuOpen = false;
    g_bandEdit = BandEdit::Path;
    g_bandText = g_fileCur;
    g_bandAnchor = 0;
    g_bandCaret = (int)g_bandText.size();
    g_bandScroll = 0;
    g_bandError.clear();
    g_bandComp.clear();
    g_bandCompIdx = -1;
    ResetBlink();
    MarkDirty();
}

static void BeginRename(const FileEntry& t) {
    g_fileMenuOpen = false;
    g_bandEdit = BandEdit::Rename;
    g_bandTarget = t.path;
    g_bandText = t.name;
    const size_t dot = t.isDir ? std::wstring::npos : t.name.find_last_of(L'.');
    g_bandAnchor = 0;  // the name without its extension starts selected, like in Explorer
    g_bandCaret = (dot != std::wstring::npos && dot > 0) ? (int)dot : (int)t.name.size();
    g_bandScroll = 0;
    g_bandError.clear();
    ResetBlink();
    MarkDirty();
}

static void BandReplace(const std::wstring& ins) {
    const int a = std::min(g_bandAnchor, g_bandCaret);
    const int b = std::max(g_bandAnchor, g_bandCaret);
    const std::wstring t = g_bandText.substr(0, a) + ins + g_bandText.substr(b);
    if (t.size() > 1024) return;
    g_bandText = t;
    g_bandCaret = g_bandAnchor = a + (int)ins.size();
    g_bandError.clear();
    g_bandComp.clear();
    g_bandCompIdx = -1;
    ResetBlink();
    MarkDirty();
}

static void BandMove(int pos, bool extend) {
    pos = std::clamp(pos, 0, (int)g_bandText.size());
    g_bandCaret = pos;
    if (!extend) g_bandAnchor = pos;
    ResetBlink();
    MarkDirty();
}

static bool BandSep(wchar_t c) {
    return c == L'\\' || c == L'/' || c == L' ' || c == L'.' || c == L'_' || c == L'-';
}

static int BandPrevWord(int pos) {
    while (pos > 0 && BandSep(g_bandText[pos - 1])) --pos;
    while (pos > 0 && !BandSep(g_bandText[pos - 1])) --pos;
    return pos;
}

static int BandNextWord(int pos) {
    const int n = (int)g_bandText.size();
    while (pos < n && !BandSep(g_bandText[pos])) ++pos;
    while (pos < n && BandSep(g_bandText[pos])) ++pos;
    return pos;
}

static std::wstring ClipboardText() {
    std::wstring r;
    if (!OpenClipboard(g_hwnd)) return r;
    HANDLE h = GetClipboardData(CF_UNICODETEXT);
    if (h) {
        const wchar_t* p = (const wchar_t*)GlobalLock(h);
        if (p) {
            r = p;
            GlobalUnlock(h);
        }
    }
    CloseClipboard();
    return r;
}

// ===== Clipboard history (opt-in, text and images) =====

static const size_t kMaxClipItems = 30;      // plenty for a launcher; older entries fall off
static const size_t kMaxClipChars = 8192;    // larger copies stay out of the history
static const int kMaxClipImageSide = 2560;  // longer images are scaled down before saving
static std::wstring ClipKey(const std::wstring& suffix) {
    return L"clip." + CurrentUserSid() + L"." + suffix;
}

// True when a previous run left clipboard data behind: the count value, or the per-account image
// folder. The folder is checked without creating it, because creating it is exactly what a disabled
// feature must not do.
static bool ClipHistoryOnDisk() {
    if (Wh_GetIntValue(ClipKey(L"count").c_str(), 0) > 0) return true;
    wchar_t buf[MAX_PATH] = L"";
    if (Wh_GetModStoragePath(buf, ARRAYSIZE(buf)) == 0 || !buf[0]) return false;
    const std::wstring dir = std::wstring(buf) + L"\\clips\\" + CurrentUserSid();
    return GetFileAttributesW(dir.c_str()) != INVALID_FILE_ATTRIBUTES;
}

// True when the app that put the content on the clipboard asked monitors to keep it out of history.
// Password managers set these, and Windows' own clipboard history honours them. The clipboard can be
// busy right after a copy, so this retries and, when the flag is there but cannot be read, skips the
// copy instead of risking a password in the history.
static bool ClipboardOptedOut() {
    static const UINT kExclude = RegisterClipboardFormatW(L"ExcludeClipboardContentFromMonitorProcessing");
    static const UINT kIgnore = RegisterClipboardFormatW(L"Clipboard Viewer Ignore");
    static const UINT kHistory = RegisterClipboardFormatW(L"CanIncludeInClipboardHistory");
    if ((kExclude && IsClipboardFormatAvailable(kExclude)) ||
        (kIgnore && IsClipboardFormatAvailable(kIgnore))) {
        return true;
    }
    if (!kHistory || !IsClipboardFormatAvailable(kHistory)) return false;
    for (int attempt = 0; attempt < 5; ++attempt) {
        if (!OpenClipboard(g_hwnd)) {
            Sleep(10);
            continue;
        }
        bool optedOut = true;  // unreadable: fail safe
        if (HANDLE h = GetClipboardData(kHistory)) {
            if (const DWORD* v = (const DWORD*)GlobalLock(h)) {
                optedOut = *v == 0;
                GlobalUnlock(h);
            }
        }
        CloseClipboard();
        return optedOut;
    }
    return true;  // could not check it: do not store the copy
}

// Deletes every saved entry and its image, used by "Clear Clipboard History" and when the feature is
// switched off: turning it off must not leave copies behind.
static void EraseClipHistory() {
    for (const ClipEntry& c : g_clips) {
        if (c.image) DeleteClipImageFile(c.text);
    }
    g_clips.clear();
    SaveClipHistory();
}

static void DeleteClipImageFile(const std::wstring& stored) {
    const std::wstring path = ClipImagePath(stored);
    if (!path.empty()) DeleteFileW(path.c_str());
}

// Drops entries older than the retention setting and deletes their image files. Pinned entries stay
// until they are unpinned, and 0 days means "keep until the 30-entry limit removes them".
static bool PurgeExpiredClips() {
    if (g_cfg.clipRetentionDays <= 0 || g_clips.empty()) return false;
    const int cutoff = NowStamp() - g_cfg.clipRetentionDays * 86400;
    bool removed = false;
    for (size_t i = g_clips.size(); i-- > 0;) {
        const ClipEntry& c = g_clips[i];
        if (c.pinned || c.stamp <= 0 || c.stamp > cutoff) continue;
        if (c.image) DeleteClipImageFile(c.text);
        g_clips.erase(g_clips.begin() + (std::ptrdiff_t)i);
        removed = true;
    }
    return removed;
}

// Deletes image files no entry points to, left behind by a crash or by an older run.
static void SweepClipImages() {
    const std::wstring dir = ClipImageDir();
    if (dir.empty()) return;
    std::vector<std::wstring> keep;
    for (const ClipEntry& c : g_clips) {
        if (!c.image) continue;
        const size_t bar = c.text.find(L'|');
        keep.push_back(bar == std::wstring::npos ? c.text : c.text.substr(0, bar));
    }
    WIN32_FIND_DATAW fd = {};
    HANDLE h = FindFirstFileW((dir + L"\\*.dat").c_str(), &fd);
    if (h == INVALID_HANDLE_VALUE) return;
    do {
        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) continue;
        const std::wstring name = fd.cFileName;
        bool used = false;
        for (const std::wstring& k : keep) {
            if (k == name) {
                used = true;
                break;
            }
        }
        if (!used) DeleteFileW((dir + L"\\" + name).c_str());
    } while (FindNextFileW(h, &fd));
    FindClose(h);
}

static void SaveClipHistory() {
    // Clear the tail as well: items that fell off must not stay in storage. Text is encrypted first;
    // if that ever fails the entry is skipped rather than written in the clear.
    for (int i = 0; i < (int)kMaxClipItems; ++i) {
        const bool has = i < (int)g_clips.size();
        const ClipEntry* c = has ? &g_clips[(size_t)i] : nullptr;
        const std::wstring idx = std::to_wstring(i);
        std::wstring slot;
        if (c) slot = c->image ? c->text : c->protectedText;
        Wh_SetStringValue(ClipKey(L"i" + idx).c_str(), slot.c_str());
        Wh_SetIntValue(ClipKey(L"p" + idx).c_str(), c && c->pinned ? 1 : 0);
        Wh_SetIntValue(ClipKey(L"k" + idx).c_str(), c && c->image ? 1 : 0);
        Wh_SetIntValue(ClipKey(L"t" + idx).c_str(), c && !slot.empty() ? c->stamp : 0);
    }
    Wh_SetIntValue(ClipKey(L"count").c_str(), (int)g_clips.size());
}

static void LoadClipHistory() {
    g_clips.clear();
    const int n =
        std::clamp(Wh_GetIntValue(ClipKey(L"count").c_str(), 0), 0, (int)kMaxClipItems);
    // Base64 of the ciphertext is about 2.7 times the plain text, so the read buffer is bigger.
    std::vector<wchar_t> buf(kMaxClipChars * 3 + 256, 0);
    for (int i = 0; i < n; ++i) {
        const std::wstring idx = std::to_wstring(i);
        const size_t len = Wh_GetStringValue(ClipKey(L"i" + idx).c_str(), buf.data(), buf.size());
        if (len == 0) continue;
        ClipEntry e;
        const std::wstring slot(buf.data(), len);
        e.image = Wh_GetIntValue(ClipKey(L"k" + idx).c_str(), 0) != 0;
        e.pinned = Wh_GetIntValue(ClipKey(L"p" + idx).c_str(), 0) != 0;
        e.stamp = Wh_GetIntValue(ClipKey(L"t" + idx).c_str(), 0);
        if (e.image) {
            e.text = slot;
            const size_t bar = e.text.find(L'|');
            if (bar != std::wstring::npos) {
                const std::wstring dims = e.text.substr(bar + 1);
                const size_t x = dims.find(L'x');
                if (x != std::wstring::npos) {
                    e.w = _wtoi(dims.c_str());
                    e.h = _wtoi(dims.c_str() + x + 1);
                }
            }
        } else if (!UnprotectText(slot, e.text)) {
            continue;  // copied under another account or a broken blob: drop it
        } else {
            // A slot written before encryption existed is plain text: keep the entry, but never
            // write it back unencrypted. Only the first build of 1.0.0 could produce one.
            e.protectedText = (slot.empty() || slot[0] == L'e') ? slot : ProtectText(e.text);
        }
        g_clips.push_back(std::move(e));
    }
    SweepClipImages();  // files without an entry are gone before anything shows them
    if (PurgeExpiredClips()) SaveClipHistory();
}

static void UpdateClipboardListener() {
    if (!g_hwnd) return;
    if (g_cfg.clipboardHistory) {
        AddClipboardFormatListener(g_hwnd);
    } else {
        RemoveClipboardFormatListener(g_hwnd);
    }
}

// ===== Clipboard images =====
// PNG, not raw pixels: a 4K screenshot is ~33 MB raw and a few MB as PNG, and PNG keeps text edges
// sharp where JPEG would ring. Anything over kMaxClipImageSide is scaled down while reading.
// shortcut: encoded on the UI thread (~20-40 ms for a big screenshot); move it to a worker if needed.

// Thumbnails reuse the file-preview pipeline (64 px, decoded off the UI thread).
static ID2D1Bitmap* ClipImageThumb(const std::wstring& stored) {
    const std::wstring path = ClipImagePath(stored);
    if (path.empty()) return nullptr;
    FileEntry e;
    e.path = path;
    e.ext = L"PNG";
    return EnsureFileThumb(e);
}

static IWICImagingFactory* NewWicFactory() {
    IWICImagingFactory* wic = nullptr;
    CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&wic));
    return wic;
}

// Encodes the pixels as PNG in memory, encrypts them and writes the file. Nothing readable is left on
// disk: Windhawk storage is shared by every account on the PC.
static bool SaveClipPng(const std::wstring& path, const uint32_t* px, int w, int h) {
    IWICImagingFactory* wic = NewWicFactory();
    if (!wic) return false;
    IWICBitmap* bmp = nullptr;
    IStream* stream = nullptr;
    IWICBitmapEncoder* enc = nullptr;
    IWICBitmapFrameEncode* frame = nullptr;
    IPropertyBag2* props = nullptr;
    WICPixelFormatGUID fmt = GUID_WICPixelFormat32bppBGRA;
    bool encoded = false;
    const UINT stride = (UINT)w * 4;
    const UINT bytes = (UINT)((size_t)w * h * 4);
    if (SUCCEEDED(CreateStreamOnHGlobal(nullptr, TRUE, &stream)) &&
        SUCCEEDED(wic->CreateBitmapFromMemory((UINT)w, (UINT)h, GUID_WICPixelFormat32bppBGRA,
                                              stride, bytes, (BYTE*)px, &bmp)) &&
        SUCCEEDED(wic->CreateEncoder(GUID_ContainerFormatPng, nullptr, &enc)) &&
        SUCCEEDED(enc->Initialize(stream, WICBitmapEncoderNoCache)) &&
        SUCCEEDED(enc->CreateNewFrame(&frame, &props)) &&
        SUCCEEDED(frame->Initialize(props)) && SUCCEEDED(frame->SetSize((UINT)w, (UINT)h)) &&
        SUCCEEDED(frame->SetPixelFormat(&fmt)) &&
        SUCCEEDED(frame->WritePixels((UINT)h, stride, bytes, (BYTE*)px))) {
        encoded = SUCCEEDED(frame->Commit()) && SUCCEEDED(enc->Commit());
    }
    SafeRelease(props);
    SafeRelease(frame);
    SafeRelease(enc);
    SafeRelease(bmp);
    bool ok = false;
    if (encoded) {
        STATSTG stat = {};
        HGLOBAL hglobal = nullptr;
        if (SUCCEEDED(stream->Stat(&stat, STATFLAG_NONAME)) && stat.cbSize.QuadPart > 0 &&
            SUCCEEDED(GetHGlobalFromStream(stream, &hglobal))) {
            const void* p = GlobalLock(hglobal);
            if (p) {
                const std::vector<BYTE> png((const BYTE*)p,
                                            (const BYTE*)p + (size_t)stat.cbSize.QuadPart);
                GlobalUnlock(hglobal);
                std::vector<BYTE> enc2;
                ok = ProtectBytes(png, enc2) && WriteFileBytes(path, enc2);
            }
        }
    }
    SafeRelease(stream);
    wic->Release();
    return ok;
}

// Reads one of our encrypted entries back as top-down 32bpp BGRA pixels (for pasting and copying it).
static bool LoadClipPng(const std::wstring& path, std::vector<uint32_t>& px, int& w, int& h) {
    std::vector<BYTE> png;
    if (!ReadProtectedFile(path, png)) return false;
    IWICImagingFactory* wic = NewWicFactory();
    if (!wic) return false;
    DecodedImage img;
    bool ok = false;
    if (DecodeImageBytes(wic, png, img)) {
        UINT fw = 0, fh = 0;
        img.frame->GetSize(&fw, &fh);
        IWICFormatConverter* conv = nullptr;
        if (fw > 0 && fh > 0 && (uint64_t)fw * fh <= 64ull * 1000 * 1000 &&
            SUCCEEDED(wic->CreateFormatConverter(&conv)) && conv &&
            SUCCEEDED(conv->Initialize(img.frame, GUID_WICPixelFormat32bppBGRA,
                                       WICBitmapDitherTypeNone, nullptr, 0.0,
                                       WICBitmapPaletteTypeCustom))) {
            std::vector<uint32_t> out((size_t)fw * fh, 0);
            if (SUCCEEDED(conv->CopyPixels(nullptr, fw * 4, (UINT)(out.size() * 4),
                                           (BYTE*)out.data()))) {
                px = std::move(out);
                w = (int)fw;
                h = (int)fh;
                ok = true;
            }
        }
        SafeRelease(conv);
    }
    wic->Release();
    return ok;
}

// Bytes between a packed DIB's header and its pixel data (colour table or channel masks).
static size_t DibBitsOffset(const BITMAPINFOHEADER* bih) {
    if (bih->biBitCount <= 8) {
        return (size_t)bih->biSize + ((size_t)1 << bih->biBitCount) * sizeof(RGBQUAD);
    }
    // BI_BITFIELDS (3) and BI_ALPHABITFIELDS (6) keep their masks right after a 40-byte header.
    if (bih->biSize == sizeof(BITMAPINFOHEADER) && (bih->biCompression == 3 || bih->biCompression == 6)) {
        return (size_t)bih->biSize + 3 * sizeof(DWORD);
    }
    return (size_t)bih->biSize;
}

// Converts a clipboard DIB (CF_DIB or CF_DIBV5) to top-down 32bpp BGRA, scaled down to the storage
// limit in the same blit. The alpha channel is forced opaque: Windows leaves it at 0 for screenshots
// and browser copies, which would otherwise be saved as a fully transparent image.
// Converts a clipboard DIB (CF_DIB or CF_DIBV5) to top-down 32bpp BGRA, scaled down to the storage
// limit in the same blit. `avail` is the size of the clipboard block: the header's dimensions are
// checked against it, because another app can publish a header that claims more than it sent. The
// alpha channel is forced opaque: Windows leaves it at 0 for screenshots and browser copies, which
// would otherwise be saved as a fully transparent image.
static bool DibToPixels(const BYTE* base,
                        size_t avail,
                        std::vector<uint32_t>& px,
                        int& w,
                        int& h) {
    if (!base) return false;
    const BITMAPINFOHEADER* bih = (const BITMAPINFOHEADER*)base;
    if (bih->biSize < sizeof(BITMAPINFOHEADER) || bih->biSize > avail) return false;
    const int sw = std::abs((int)bih->biWidth);
    const int sh = std::abs((int)bih->biHeight);
    if (sw <= 0 || sh <= 0) return false;
    // Bound the work and the read: a header can claim any size, the block cannot.
    if ((uint64_t)sw * sh > 64ull * 1000 * 1000) return false;
    const size_t off = DibBitsOffset(bih);
    const size_t stride = (((size_t)sw * bih->biBitCount + 31) / 32) * 4;  // DWORD-aligned rows
    if (off > avail || stride > (avail - off) / (size_t)sh) return false;
    int tw = sw, th = sh;
    if (sw > kMaxClipImageSide || sh > kMaxClipImageSide) {
        if (sw >= sh) {
            tw = kMaxClipImageSide;
            th = std::max(1, (int)((int64_t)sh * kMaxClipImageSide / sw));
        } else {
            th = kMaxClipImageSide;
            tw = std::max(1, (int)((int64_t)sw * kMaxClipImageSide / sh));
        }
    }
    BITMAPINFO dst = {};
    dst.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    dst.bmiHeader.biWidth = tw;
    dst.bmiHeader.biHeight = -th;  // top-down
    dst.bmiHeader.biPlanes = 1;
    dst.bmiHeader.biBitCount = 32;
    dst.bmiHeader.biCompression = BI_RGB;
    HDC screen = GetDC(nullptr);
    if (!screen) return false;
    void* dstBits = nullptr;
    HBITMAP bmp = CreateDIBSection(screen, &dst, DIB_RGB_COLORS, &dstBits, nullptr, 0);
    HDC mem = bmp ? CreateCompatibleDC(screen) : nullptr;
    bool ok = false;
    if (mem && dstBits) {
        HGDIOBJ old = SelectObject(mem, bmp);
        SetStretchBltMode(mem, HALFTONE);
        SetBrushOrgEx(mem, 0, 0, nullptr);
        // StretchDIBits returns the number of scan lines copied, or GDI_ERROR (0xFFFFFFFF) on
        // failure, which is negative once it is an int: "> 0" is the portable way to test it.
        const int got = StretchDIBits(mem, 0, 0, tw, th, 0, 0, sw, sh,
                                      base + DibBitsOffset(bih), (const BITMAPINFO*)bih,
                                      DIB_RGB_COLORS, SRCCOPY);
        SelectObject(mem, old);
        if (got > 0) {
            px.assign((size_t)tw * th, 0);
            memcpy(px.data(), dstBits, px.size() * 4);
            for (uint32_t& p : px) p |= 0xFF000000u;  // opaque, see above
            w = tw;
            h = th;
            ok = true;
        }
    }
    if (mem) DeleteDC(mem);
    if (bmp) DeleteObject(bmp);
    ReleaseDC(nullptr, screen);
    return ok;
}

// FNV-1a over every pixel. Sampling (it used to take 1 in ~500) let two different screenshots of the
// same window share a name, and the second copy then reused the first file and pasted the old image.
static uint64_t ClipImageHash(const std::vector<uint32_t>& px, int w, int h) {
    uint64_t hash = 14695981039346656037ull;
    for (const uint32_t p : px) {
        hash = (hash ^ p) * 1099511628211ull;
    }
    hash = (hash ^ (uint64_t)w) * 1099511628211ull;
    return (hash ^ (uint64_t)h) * 1099511628211ull;
}

// Shows a new entry right away when "clip:" is the mode on screen.
static void RefreshClipList() {
    if (g_vis != Vis::Shown || g_activator < 0 ||
        kActivators[g_activator].mode != LauncherMode::Clipboard) {
        return;
    }
    const int keep = g_sel;
    RunSearch();
    g_sel = std::clamp(keep, 0, std::max(0, (int)g_results.size() - 1));
    UpdateTargets(false, /*followSelection=*/true);
    MarkDirty();
}

// Moves an entry to the front (or adds it), trims the list, drops expired entries and saves.
static void AddClipEntry(std::wstring payload, bool image, int w, int h) {
    bool wasPinned = false;
    for (size_t i = 0; i < g_clips.size(); ++i) {
        if (g_clips[i].text == payload && g_clips[i].image == image) {
            wasPinned = g_clips[i].pinned;
            g_clips.erase(g_clips.begin() + (std::ptrdiff_t)i);
            break;
        }
    }
    ClipEntry e;
    e.text = std::move(payload);
    e.image = image;
    e.w = w;
    e.h = h;
    e.pinned = wasPinned;
    e.stamp = NowStamp();
    if (!image) e.protectedText = ProtectText(e.text);  // encrypt once, not on every save
    g_clips.insert(g_clips.begin(), std::move(e));
    // Drop the oldest unpinned entry, but keep the copy that just arrived. If everything else is
    // pinned, the oldest pin makes room instead of silently losing the new copy.
    while (g_clips.size() > kMaxClipItems) {
        size_t victim = g_clips.size() - 1;
        for (size_t i = g_clips.size(); i-- > 1;) {
            if (!g_clips[i].pinned) {
                victim = i;
                break;
            }
        }
        if (g_clips[victim].image) DeleteClipImageFile(g_clips[victim].text);
        g_clips.erase(g_clips.begin() + (std::ptrdiff_t)victim);
    }
    PurgeExpiredClips();
    SaveClipHistory();
    RefreshClipList();
}

// Saves the image on the clipboard, if there is one, as a PNG entry. Returns true when it was added.
static bool CaptureClipboardImage() {
    std::vector<uint32_t> px;
    int w = 0, h = 0;
    if (!OpenClipboard(g_hwnd)) return false;
    HANDLE dib = GetClipboardData(CF_DIBV5);
    if (!dib) dib = GetClipboardData(CF_DIB);
    if (dib) {
        const BYTE* base = (const BYTE*)GlobalLock(dib);
        if (base) {
            DibToPixels(base, GlobalSize(dib), px, w, h);
            GlobalUnlock(dib);
        }
    }
    CloseClipboard();
    if (px.empty() || w <= 0 || h <= 0) return false;
    const std::wstring dir = ClipImageDir();
    if (dir.empty()) return false;
    wchar_t name[64] = L"";
    swprintf_s(name, L"%016llx.dat", (unsigned long long)ClipImageHash(px, w, h));
    const std::wstring file = dir + L"\\" + name;
    // The name is the content hash, so the same image copied twice reuses its file instead of
    // encoding it again.
    if (GetFileAttributesW(file.c_str()) == INVALID_FILE_ATTRIBUTES &&
        !SaveClipPng(file, px.data(), w, h)) {
        DeleteFileW(file.c_str());  // never leave a half-written file behind
        return false;
    }
    AddClipEntry(std::wstring(name) + L"|" + std::to_wstring(w) + L"x" + std::to_wstring(h), true,
                 w, h);
    return true;
}

// Called from WM_CLIPBOARDUPDATE: text wins when the copy has it (a link copied from a browser
// carries both), otherwise the image is saved. Copies that ask to stay out of clipboard history
// (password managers and Windows itself) are ignored.
static void CaptureClipboard() {
    if (!g_cfg.clipboardHistory) return;
    if (ClipboardOptedOut()) return;
    std::wstring text = ClipboardText();
    if (!text.empty() && text.size() <= kMaxClipChars && !TrimBlanks(text).empty()) {
        AddClipEntry(std::move(text), false, 0, 0);
        return;
    }
    CaptureClipboardImage();
}

// Removes the selected clipboard entry (Ctrl+Shift+R while "clip:" is active).
static void RemoveSelectedClip() {
    if (g_sel < 0 || g_sel >= (int)g_results.size() ||
        g_results[g_sel].kind != ResultKind::Clip) {
        return;
    }
    const std::wstring text = g_results[g_sel].payload;
    for (auto it = g_clips.begin(); it != g_clips.end(); ++it) {
        if (it->text == text) {
            if (it->image) DeleteClipImageFile(it->text);
            g_clips.erase(it);
            break;
        }
    }
    SaveClipHistory();
    const int keep = g_sel;
    RunSearch();
    g_sel = std::clamp(keep, 0, std::max(0, (int)g_results.size() - 1));
    UpdateTargets(false, /*followSelection=*/true);
    MarkDirty();
}

// Toggles the pin on the selected clipboard entry (Ctrl+D while "clip:" is active).
static void ToggleSelectedClipPin() {
    if (g_sel < 0 || g_sel >= (int)g_results.size() ||
        g_results[g_sel].kind != ResultKind::Clip) {
        return;
    }
    const std::wstring text = g_results[g_sel].payload;
    for (ClipEntry& c : g_clips) {
        if (c.text == text) {
            c.pinned = !c.pinned;
            break;
        }
    }
    SaveClipHistory();
    RunSearch();
    g_sel = 0;
    for (int i = 0; i < (int)g_results.size(); ++i) {
        if (g_results[i].kind == ResultKind::Clip && g_results[i].payload == text) {
            g_sel = i;
            break;
        }
    }
    UpdateTargets(false, /*followSelection=*/true);
    MarkDirty();
}

// Puts an image entry back on the clipboard as a DIB (what every app understands). The pixels are
// stored top-down, so the header is negative-height to match.
static bool SetClipboardImage(const std::wstring& stored) {
    std::vector<uint32_t> px;
    int w = 0, h = 0;
    const std::wstring path = ClipImagePath(stored);
    if (path.empty() || !LoadClipPng(path, px, w, h)) return false;
    const size_t bytes = (size_t)w * h * 4;
    HGLOBAL mem = GlobalAlloc(GMEM_MOVEABLE, sizeof(BITMAPINFOHEADER) + bytes);
    if (!mem) return false;
    BITMAPINFOHEADER* bih = (BITMAPINFOHEADER*)GlobalLock(mem);
    if (!bih) {
        GlobalFree(mem);
        return false;
    }
    ZeroMemory(bih, sizeof(BITMAPINFOHEADER));
    bih->biSize = sizeof(BITMAPINFOHEADER);
    bih->biWidth = w;
    bih->biHeight = -h;
    bih->biPlanes = 1;
    bih->biBitCount = 32;
    bih->biCompression = BI_RGB;
    bih->biSizeImage = (DWORD)bytes;
    memcpy(bih + 1, px.data(), bytes);
    GlobalUnlock(mem);
    if (!OpenClipboard(g_hwnd)) {
        GlobalFree(mem);
        return false;
    }
    EmptyClipboard();
    if (!SetClipboardData(CF_DIB, mem)) GlobalFree(mem);  // the clipboard owns it on success
    CloseClipboard();
    return true;
}

// Hides the bar and sends Ctrl+V to the window that was focused before it opened. If the focus
// cannot be restored (an elevated window, for example), the entry is still on the clipboard for a
// manual paste.
static void PasteIntoPreviousWindow() {
    HideLauncher();
    HWND target = g_prevForeground;
    if (target && target != g_hwnd && IsWindow(target)) {
        INPUT wake{};
        wake.type = INPUT_MOUSE;
        SendInput(1, &wake, sizeof(INPUT));
        SetForegroundWindow(target);
        BringWindowToTop(target);
        Sleep(60);
        INPUT keys[4] = {};
        keys[0].type = INPUT_KEYBOARD;
        keys[0].ki.wVk = VK_CONTROL;
        keys[1].type = INPUT_KEYBOARD;
        keys[1].ki.wVk = 'V';
        keys[2].type = INPUT_KEYBOARD;
        keys[2].ki.wVk = 'V';
        keys[2].ki.dwFlags = KEYEVENTF_KEYUP;
        keys[3].type = INPUT_KEYBOARD;
        keys[3].ki.wVk = VK_CONTROL;
        keys[3].ki.dwFlags = KEYEVENTF_KEYUP;
        SendInput(4, keys, sizeof(INPUT));
    }
}

// Puts the text on the clipboard and pastes it into the window that was focused before.
static void PasteClipboardText(const std::wstring& text) {
    if (text.empty()) return;
    CopyTextToClipboard(text);
    PasteIntoPreviousWindow();
}

// Same for an image entry.
static void PasteClipboardImage(const std::wstring& stored) {
    if (SetClipboardImage(stored)) PasteIntoPreviousWindow();
}

// Tab in the path field: completes the last segment with the matching sub-folders, cycling.
static void BandComplete(bool back) {
    if (g_bandEdit != BandEdit::Path) return;
    if (g_bandComp.empty() || g_bandText != g_bandCompLast) {
        const size_t sl = g_bandText.find_last_of(L"\\/");
        const std::wstring dirPart = sl == std::wstring::npos ? L"" : g_bandText.substr(0, sl + 1);
        const std::wstring namePart = sl == std::wstring::npos ? g_bandText : g_bandText.substr(sl + 1);
        const std::wstring base = AbsFromInput(dirPart);
        g_bandComp.clear();
        g_bandCompIdx = -1;
        g_bandCompDir = dirPart;
        WIN32_FIND_DATAW fd;
        HANDLE h = FindFirstFileExW(JoinPath(base, L"*").c_str(), FindExInfoBasic, &fd,
                                    FindExSearchNameMatch, nullptr, FIND_FIRST_EX_LARGE_FETCH);
        if (h != INVALID_HANDLE_VALUE) {
            do {
                if (SkipFindEntry(fd) || !(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) continue;
                if (_wcsnicmp(fd.cFileName, namePart.c_str(), namePart.size()) != 0) continue;
                g_bandComp.push_back(fd.cFileName);
            } while (FindNextFileW(h, &fd));
            FindClose(h);
        }
        std::sort(g_bandComp.begin(), g_bandComp.end(),
                  [](const std::wstring& a, const std::wstring& b) { return _wcsicmp(a.c_str(), b.c_str()) < 0; });
    }
    const int n = (int)g_bandComp.size();
    if (n == 0) return;
    g_bandCompIdx = back ? (g_bandCompIdx <= 0 ? n - 1 : g_bandCompIdx - 1) : (g_bandCompIdx + 1) % n;
    g_bandText = g_bandCompDir + g_bandComp[g_bandCompIdx] + L"\\";
    g_bandCompLast = g_bandText;
    g_bandCaret = g_bandAnchor = (int)g_bandText.size();
    g_bandError.clear();
    ResetBlink();
    MarkDirty();
}

static void CommitPath() {
    const std::wstring p = AbsFromInput(g_bandText);
    const DWORD at = GetFileAttributesW(p.c_str());
    if (at == INVALID_FILE_ATTRIBUTES) {
        BandFail(L"Path not found");
        return;
    }
    g_bandEdit = BandEdit::None;
    if (at & FILE_ATTRIBUTE_DIRECTORY) {
        FileGoTo(p, true, L"");
    } else {
        FileGoTo(ParentDir(p), true, p);  // a file path: open its folder with the file highlighted
    }
}

static void CommitRename() {
    std::wstring name = g_bandText;
    while (!name.empty() && name.back() == L' ') name.pop_back();
    size_t a = 0;
    while (a < name.size() && name[a] == L' ') ++a;
    name.erase(0, a);
    const std::wstring old = g_bandTarget;
    const std::wstring dir = ParentDir(old);
    const size_t sl = old.find_last_of(L'\\');
    const std::wstring oldName = sl == std::wstring::npos ? old : old.substr(sl + 1);
    if (name == oldName) {
        g_bandEdit = BandEdit::None;
        MarkDirty();
        return;
    }
    if (name.empty() || name == L"." || name == L".." ||
        name.find_first_of(L"\\/:*?\"<>|") != std::wstring::npos || name.back() == L'.') {
        BandFail(L"Invalid name");
        return;
    }
    const std::wstring np = JoinPath(dir, name);
    if (_wcsicmp(np.c_str(), old.c_str()) != 0 && GetFileAttributesW(np.c_str()) != INVALID_FILE_ATTRIBUTES) {
        BandFail(L"Name already exists");
        return;
    }
    if (!MoveFileW(old.c_str(), np.c_str())) {
        BandFail(L"Could not rename");
        return;
    }
    g_bandEdit = BandEdit::None;
    for (auto& e : g_fileDeep) {  // keep deep results in sync
        if (SamePath(e.path, old)) {
            e.name = name;
            e.folded = FoldString(name);
            e.path = np;
            e.meta.clear();  // the type/size line may have changed
            if (!e.isDir) e.ext = FileExtOf(name);
        }
    }
    ForgetFileThumb(old);
    ForgetFileThumb(np);
    g_fileDir.clear();  // relist the folder
    RunSearch();
    for (size_t i = 0; i < g_results.size(); ++i) {
        if (SamePath(g_results[i].payload, np)) {
            g_sel = (int)i;
            break;
        }
    }
    UpdateTargets(false, /*followSelection=*/true);
    ResetBlink();
    MarkDirty();
}

// Creates "New folder" in the folder being browsed and starts renaming it.
static void FileNewFolder() {
    if (!g_fileMode || g_fileCur.empty() || InPcRoot()) return;
    std::wstring path;
    for (int i = 1; i < 1000; ++i) {
        const std::wstring name = i == 1 ? L"New folder" : L"New folder (" + std::to_wstring(i) + L")";
        const std::wstring cand = JoinPath(g_fileCur, name);
        if (CreateDirectoryW(cand.c_str(), nullptr)) {
            path = cand;
            break;
        }
        if (GetLastError() != ERROR_ALREADY_EXISTS) return;
    }
    if (path.empty()) return;
    FileGoTo(g_fileCur, false, path);  // relists with the new folder highlighted
    if (const FileEntry* e = SelectedFileEntry()) {
        if (SamePath(e->path, path)) BeginRename(*e);
    }
}

// Creates "New file.txt" in the folder being browsed and starts renaming it. The name without
// the extension is selected, so typing replaces it and the extension stays unless the user
// edits it.
static void FileNewFile() {
    if (!g_fileMode || g_fileCur.empty() || InPcRoot()) return;
    std::wstring path;
    for (int i = 1; i < 1000; ++i) {
        const std::wstring base =
            i == 1 ? L"New file" : L"New file (" + std::to_wstring(i) + L")";
        const std::wstring cand = JoinPath(g_fileCur, base + L".txt");
        HANDLE h = CreateFileW(cand.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_NEW,
                               FILE_ATTRIBUTE_NORMAL, nullptr);
        if (h != INVALID_HANDLE_VALUE) {
            CloseHandle(h);
            path = cand;
            break;
        }
        const DWORD err = GetLastError();
        if (err != ERROR_FILE_EXISTS && err != ERROR_ALREADY_EXISTS) return;
    }
    if (path.empty()) return;
    FileGoTo(g_fileCur, false, path);  // relists with the new file highlighted
    if (const FileEntry* e = SelectedFileEntry()) {
        if (SamePath(e->path, path)) BeginRename(*e);
    }
}

static void CopyFileToClipboard(const std::wstring& path) {
    const size_t bytes = sizeof(DROPFILES) + (path.size() + 2) * sizeof(wchar_t);
    HGLOBAL h = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, bytes);
    if (!h) return;
    auto* df = (DROPFILES*)GlobalLock(h);
    if (!df) {
        GlobalFree(h);
        return;
    }
    df->pFiles = sizeof(DROPFILES);
    df->fWide = TRUE;
    memcpy((BYTE*)df + sizeof(DROPFILES), path.c_str(), path.size() * sizeof(wchar_t));
    GlobalUnlock(h);
    if (!OpenClipboard(g_hwnd)) {
        GlobalFree(h);
        return;
    }
    EmptyClipboard();
    if (!SetClipboardData(CF_HDROP, h)) GlobalFree(h);
    CloseClipboard();
}

// Sends the entry to the Recycle Bin and refreshes the list.
static void FileDelete(const FileEntry& t) {
    std::wstring from = t.path;
    from.push_back(L'\0');  // pFrom is double-null terminated
    SHFILEOPSTRUCTW op = {};
    op.hwnd = g_hwnd;
    op.wFunc = FO_DELETE;
    op.pFrom = from.c_str();
    // WANTNUKEWARNING re-enables the shell prompt when the item cannot go to the Recycle Bin
    // (network share, item larger than the bin, no bin on the volume): without it the shell
    // would silently turn "Move to Recycle Bin" into a permanent delete.
    op.fFlags = FOF_ALLOWUNDO | FOF_NOCONFIRMATION | FOF_WANTNUKEWARNING;
    // The shell briefly takes the foreground while deleting. That must not close the bar (the
    // user would lose the folder they are working in), so focus loss is ignored during the call
    // and for a moment after it; then the bar takes the focus back.
    g_opGuard = true;
    const int delResult = SHFileOperationW(&op);
    const bool failed = delResult != 0 || op.fAnyOperationsAborted;
    g_opGuard = false;
    g_focusGrace = NowSec() + 0.6;
    if (g_vis == Vis::Shown) ForceForeground(g_hwnd);
    if (failed) return;
    for (size_t i = 0; i < g_fileDeep.size(); ++i) {
        if (SamePath(g_fileDeep[i].path, t.path)) {
            g_fileDeep.erase(g_fileDeep.begin() + i);
            break;
        }
    }
    ForgetFileThumb(t.path);
    const int keep = g_sel;
    g_fileDir.clear();
    RunSearch();
    g_sel = std::clamp(keep, 0, std::max(0, (int)g_results.size() - 1));
    UpdateTargets(false, /*followSelection=*/true);
    MarkDirty();
}

// File copies keep the bar open, so the footer confirms them for a moment.
static void FileCopied() {
    g_fileCopiedAt = NowSec();
    SetTimer(g_hwnd, TIMER_COPY, 1400, nullptr);
}

static void RunFileAction(int id) {
    const FileEntry t = g_fileMenuTarget;
    const bool has = g_fileMenuHasTarget;
    g_fileMenuOpen = false;
    g_fileMenuArmed = false;
    if (!has && id != FA_NEWFOLDER && id != FA_NEWFILE) {
        MarkDirty();
        return;
    }
    switch (id) {
        case FA_OPEN:
            if (t.isDir) FileGoTo(t.path, true, L"");
            else RunShellOpen(t.path.c_str());
            break;
        case FA_GOTO:
            FileGoTo(t.dir, true, t.path);
            break;
        case FA_REVEAL: {
            HideLauncher();
            AllowSetForegroundWindow(ASFW_ANY);
            const std::wstring folder = t.isDir ? t.path : t.dir;
            ShellExecuteW(nullptr, L"open", folder.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
            break;
        }
        case FA_ADMIN:
            HideLauncher();
            AllowSetForegroundWindow(ASFW_ANY);
            ShellExecuteW(nullptr, L"runas", t.path.c_str(), nullptr, t.dir.c_str(), SW_SHOWNORMAL);
            break;
        case FA_TERMINAL: {
            const std::wstring folder = t.isDir ? t.path : t.dir;
            std::wstring arg = folder;
            if (!arg.empty() && arg.back() == L'\\') arg += L'.';  // a trailing \ would escape the quote
            arg = L"-d \"" + arg + L"\"";
            HideLauncher();
            AllowSetForegroundWindow(ASFW_ANY);
            const std::wstring wt = LocalAppAlias(L"wt.exe");
            HINSTANCE r = wt.empty() ? (HINSTANCE)0
                                     : ShellExecuteW(nullptr, L"open", wt.c_str(), arg.c_str(),
                                                     folder.c_str(), SW_SHOWNORMAL);
            if ((INT_PTR)r <= 32) {  // no Windows Terminal: plain console in that folder
                const std::wstring cmd = SystemExe(L"cmd.exe");
                ShellExecuteW(nullptr, L"open", cmd.c_str(), nullptr, folder.c_str(), SW_SHOWNORMAL);
            }
            break;
        }
        case FA_RENAME:
            BeginRename(t);
            break;
        case FA_COPYPATH:
            CopyTextToClipboard(t.path);
            FileCopied();
            break;
        case FA_COPYNAME:
            CopyTextToClipboard(t.name);
            FileCopied();
            break;
        case FA_COPYFILE:
            CopyFileToClipboard(t.path);
            FileCopied();
            break;
        case FA_NEWFOLDER:
            FileNewFolder();
            break;
        case FA_NEWFILE:
            FileNewFile();
            break;
        case FA_PROPS: {
            SHELLEXECUTEINFOW sei = {};
            sei.cbSize = sizeof(sei);
            sei.fMask = SEE_MASK_INVOKEIDLIST;
            sei.lpVerb = L"properties";
            sei.lpFile = t.path.c_str();
            sei.nShow = SW_SHOWNORMAL;
            ShellExecuteExW(&sei);
            HideLauncher();
            break;
        }
        case FA_DELETE:
            FileDelete(t);
            break;
    }
    MarkDirty();
}

static void BuildFileMenu() {
    g_fileMenu.clear();
    if (g_fileMenuHasTarget) {
        const FileEntry& t = g_fileMenuTarget;
        g_fileMenu.push_back({FA_OPEN, t.isDir ? L"Open folder" : L"Open", L"↵"});
        if (!SamePath(t.dir, g_fileCur)) g_fileMenu.push_back({FA_GOTO, L"Show in its folder", L"Ctrl+G"});
        g_fileMenu.push_back({FA_REVEAL, L"Reveal in file manager", L"Ctrl+↵"});
        if (!t.isDir && IsExeLike(t.name)) {
            g_fileMenu.push_back({FA_ADMIN, L"Run as administrator", L"Ctrl+Shift+↵"});
        }
        g_fileMenu.push_back({FA_TERMINAL, L"Open terminal here", L"Ctrl+T"});
        g_fileMenu.push_back({FA_RENAME, L"Rename", L"F2"});
        g_fileMenu.push_back({FA_COPYPATH, L"Copy path", L"Ctrl+Shift+C"});
        g_fileMenu.push_back({FA_COPYNAME, L"Copy name", L""});
        g_fileMenu.push_back({FA_COPYFILE, L"Copy file", L""});
    }
    g_fileMenu.push_back({FA_NEWFOLDER, L"New folder", L"Ctrl+Shift+N"});
    g_fileMenu.push_back({FA_NEWFILE, L"New file", L""});
    if (g_fileMenuHasTarget) {
        g_fileMenu.push_back({FA_PROPS, L"Properties", L"Ctrl+P"});
        g_fileMenu.push_back({FA_DELETE, L"Move to Recycle Bin", L""});
    }
    g_fileMenu.push_back({FA_SORTMENU, L"Sort…", L"Ctrl+S"});
}

// Which key the chosen Files order is remembered under (per user).
static std::wstring FileSortKey() { return L"filesort." + CurrentUserSid(); }

// The order saved by the user, or Name A-Z when the setting is off or nothing was saved.
static void RestoreFileSort() {
    g_cfg.fileSort = FileSort::NameAsc;
    if (!g_cfg.rememberFileSort) return;
    const int v = std::clamp(Wh_GetIntValue(FileSortKey().c_str(), 0), 0, kFileSortCount - 1);
    g_cfg.fileSort = (FileSort)v;
}

// Switches the Files order, keeping the highlighted row when it is still listed.
static void ApplyFileSort(FileSort s) {
    g_cfg.fileSort = s;
    if (g_cfg.rememberFileSort) Wh_SetIntValue(FileSortKey().c_str(), (int)s);
    if (g_fileMode) {
        if (g_sel >= 0 && g_sel < (int)g_results.size()) g_pendingSelect = g_results[g_sel].payload;
        std::sort(g_fileEntries.begin(), g_fileEntries.end(), FileEntryLess);
        RunSearch();
        if (!g_pendingSelect.empty()) {
            for (size_t i = 0; i < g_results.size(); ++i) {
                if (SamePath(g_results[i].payload, g_pendingSelect)) {
                    g_sel = (int)i;
                    break;
                }
            }
            g_pendingSelect.clear();
        }
        UpdateTargets(false, /*followSelection=*/true);
    }
    MarkDirty();
}

// The sort menu: one row per order, with the one in use marked, so the menu itself shows the order.
static void BuildSortMenu() {
    static const struct {
        int id;
        const wchar_t* label;
        FileSort sort;
    } kSorts[kFileSortCount] = {
        {FS_NAME_ASC, L"Name (A-Z)", FileSort::NameAsc},
        {FS_NAME_DESC, L"Name (Z-A)", FileSort::NameDesc},
        {FS_TYPE, L"Type", FileSort::Type},
        {FS_MOD_NEW, L"Modified (newest first)", FileSort::ModNew},
        {FS_MOD_OLD, L"Modified (oldest first)", FileSort::ModOld},
        {FS_SIZE_BIG, L"Size (largest first)", FileSort::SizeBig},
        {FS_SIZE_SMALL, L"Size (smallest first)", FileSort::SizeSmall},
    };
    g_fileMenu.clear();
    g_fileMenuSel = 0;
    for (int i = 0; i < kFileSortCount; ++i) {
        const bool active = kSorts[i].sort == g_cfg.fileSort;
        if (active) g_fileMenuSel = i;
        g_fileMenu.push_back({kSorts[i].id, kSorts[i].label, active ? L"Active" : L""});
    }
}

static void OpenSortMenu() {
    BuildSortMenu();
    g_fileMenuArmed = false;
    g_fileMenuOpen = true;
    MarkDirty();
}

static void OpenFileMenu() {
    if (InPcRoot()) return;  // drive rows have no file actions
    CaptureFileTarget();
    BuildFileMenu();
    g_fileMenuSel = 0;
    g_fileMenuArmed = false;
    g_fileMenuOpen = true;
    MarkDirty();
}

static void FileMenuRunSel() {
    if (g_fileMenuSel < 0 || g_fileMenuSel >= (int)g_fileMenu.size()) return;
    const int id = g_fileMenu[g_fileMenuSel].id;
    if (id == FA_SORTMENU) {  // swap the menu for the sort list, no target needed
        OpenSortMenu();
        return;
    }
    if (id >= FS_NAME_ASC && id <= FS_SIZE_SMALL) {
        ApplyFileSort((FileSort)(id - FS_NAME_ASC));
        // Picking an order is a complete action: close the list (it reopens on the new order).
        g_fileMenuOpen = false;
        g_fileMenuArmed = false;
        MarkDirty();
        return;
    }
    if (id == FA_DELETE && !g_fileMenuArmed) {  // destructive: needs a second Enter
        g_fileMenuArmed = true;
        MarkDirty();
        return;
    }
    RunFileAction(id);
}

static void LaunchSelected() {
    if (g_results.empty() || g_sel < 0 || g_sel >= (int)g_results.size()) {
        return;
    }
    SearchResult& r = g_results[g_sel];
    if (r.kind == ResultKind::Command) {
        const int ci = (r.icon >= 0 && r.icon < kCommandCount) ? r.icon : -1;
        if (ci >= 0 && kCommands[ci].activator >= 0) {
            EnterActivator(kCommands[ci].activator);  // mode opener: switch into it, bar stays open
            return;
        }
        if (ci >= 0 && kCommands[ci].destructive && g_confirmCmd != ci) {
            ArmConfirm(ci);  // first Enter: arm and warn; a second Enter actually runs it
            return;
        }
        CancelConfirm();
        RunCommand(r.payload);
        return;
    }
    CancelConfirm();
    if (r.kind == ResultKind::Custom) {
        // Enter opens the form to fill/review the fields; Shift+Enter skips it and launches with
        // the current values. Entries with nothing to fill (search-only or fieldless) run directly.
        ActivateCustomEntry(r.custom, r.payload, /*skipForm=*/ShiftDown());
        return;
    }
    if (r.kind == ResultKind::Window) {
        ActivateWindow(r.hwnd);
        return;
    }
    if (IsPathResult(r.kind)) {
        RunShellOpen(r.payload.c_str());
        return;
    }
    if (r.kind == ResultKind::Clip) {
        if (r.image) {
            PasteClipboardImage(r.payload);
        } else {
            PasteClipboardText(r.payload);
        }
        return;
    }
    if (r.kind == ResultKind::Web || r.kind == ResultKind::Url ||
        r.kind == ResultKind::Bookmark) {
        OpenWebResult(r.payload);
        return;
    }
    if (r.kind == ResultKind::Setting) {
        RunShellOpen(r.payload.c_str());  // opens the ms-settings: page
        return;
    }
    if (r.kind == ResultKind::File) {
        if (SelectedFileIsDir()) {
            const std::wstring path = r.payload;
            FileGoTo(path, true, L"");  // browse inside the launcher
        } else {
            RunShellOpen(r.payload.c_str());
        }
        return;
    }
    if (r.kind != ResultKind::App) {
        return;  // other non-app results are activated by their own provider
    }
    // Learn from a real search pick (not from the empty bar or from a mode list), and only when the
    // pick fixed the order: if this app was already the best app match, there is nothing to learn.
    if (g_activator < 0 && !g_tokens.empty() && PickWasNotFirst(g_sel)) {
        RecordLearn(g_apps[r.app].id, LearnedQueryKey());
    }
    LaunchApp(r.app);
}

// Switches between the app list and the file browser (folder button in the search bar).
static void ToggleFileMode() {
    if (!g_fileMode && !g_cfg.filesEnabled) return;  // the file browser is turned off
    g_fileMode = !g_fileMode;
    g_bandEdit = BandEdit::None;
    g_fileMenuOpen = false;
    g_form = CustomFormState();  // a custom entry form does not survive a mode switch
    CancelFileSearch();
    CancelGlobalSearch();
    ClearFileDeep();
    g_fileBack.clear();
    g_fileFwd.clear();
    g_pendingSelect.clear();
    g_fileSrc = &g_fileEntries;
    if (g_fileMode) {
        RebuildFileChips();
        // Home, except the first open in a session when a valid start drive is configured.
        std::wstring dest = ResolveFileLocation(0);
        if (!g_fileStartUsed) {
            g_fileStartUsed = true;
            const std::wstring sd = StartDrivePath();
            if (!sd.empty()) dest = sd;
        }
        g_fileCur = dest;
        g_fileLoc = LocationForPath(g_fileCur);
    } else {
        g_fileCur.clear();
        g_fileLoc = 0;
    }
    g_chipSelPos = g_chipSelTarget = (g_fileLoc >= 0) ? g_fileLoc : 0;  // no slide on enter/leave
    g_fileDir.clear();  // force a fresh listing next RunSearch
    // Switching context starts from a clean slate: the query is not carried over.
    g_text.clear();
    g_caret = 0;
    g_anchor = 0;
    g_inputScroll = 0;
    g_calc = Calc::Result();
    g_calcDisplay.clear();
    g_calcActive = false;
    g_calcPending = false;
    g_calcHistSel = -1;
    g_histScroll = 0;
    if (g_hwnd) KillTimer(g_hwnd, TIMER_CALC);
    CancelConfirm();
    g_sel = 0;
    g_scrollTarget = 0;
    g_scrollPos = 0;
    RunSearch();
    ResetBlink();
    UpdateTargets(false, /*followSelection=*/true);
    MarkDirty();
}

// ===== Help sheet =====

static void CloseHelp() {
    if (!g_helpOpen) return;
    g_helpOpen = false;  // the list behind is untouched, so it comes back as it was
    ResetBlink();
    UpdateTargets(false, /*followSelection=*/false);
    MarkDirty();
}

static void OpenHelp() {
    if (g_helpOpen) return;
    // The sheet is only an overlay: the view underneath (file browser, calculator, the current
    // folder, query and selection) is left untouched so closing it returns exactly where the user
    // was. Only transient popups are dismissed so they do not linger behind the sheet.
    g_helpOpen = true;
    g_helpPage = 0;
    g_helpScroll = 0;
    g_bandEdit = BandEdit::None;
    g_fileMenuOpen = false;
    g_detailsOpen = false;
    g_detailTarget = 0;
    CancelConfirm();
    ResetBlink();
    UpdateTargets(false, /*followSelection=*/false);
    MarkDirty();
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
    const std::wstring elevate = d.elevateTarget;

    switch (actionId) {
        case ACT_OPEN:
            LaunchApp(appIdx);
            return;

        case ACT_PIN:
        case ACT_UNPIN: {
            a.pinned = (actionId == ACT_PIN);
            Wh_SetIntValue((a.usageKey + L"p").c_str(), a.pinned ? 1 : 0);
            const std::wstring keepId = a.id;
            RunSearch();
            g_sel = 0;
            for (int i = 0; i < (int)g_results.size(); ++i) {
                if (g_results[i].kind == ResultKind::App &&
                    g_apps[g_results[i].app].id == keepId) {
                    g_sel = i;
                    break;
                }
            }
            UpdateTargets(false, /*followSelection=*/true);
            MarkDirty();
            return;
        }

        case ACT_ADMIN: {
            AllowSetForegroundWindow(ASFW_ANY);
            const std::wstring exe = elevate.empty() ? target : elevate;
            std::wstring dir = exe.substr(0, exe.find_last_of(L'\\'));
            // Keep the shortcut's arguments, or a profile shortcut would open the default one.
            HINSTANCE r = ShellExecuteW(nullptr, L"runas", exe.c_str(),
                                        d.args.empty() ? nullptr : d.args.c_str(),
                                        dir.empty() ? nullptr : dir.c_str(), SW_SHOWNORMAL);
            if ((INT_PTR)r > 32) {
                RecordUse(a);
            }
            HideLauncher();
            return;
        }

        case ACT_LOCATION: {
            const std::wstring folder = ParentDir(target);
            if (!folder.empty()) {
                ShellExecuteW(nullptr, L"open", folder.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
            }
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
    g_panelMode = PanelMode::Actions;
    g_panelSwitching = false;
    g_hoverAction = -1;
    Sound::OnPanel(open);
    UpdateTargets(false, /*followSelection=*/false);
    MarkDirty();
}

// "win:" keys. -1 when the combination is not a window action.
static int WindowActionForKey(WPARAM vk, bool ctrl, bool shift) {
    if (!ctrl) return -1;
    if (!shift) {
        if (vk == 'W') return WA_CLOSE;
        if (vk == 'M') return WA_MINIMIZE;
        if (vk == VK_RETURN) return WA_LOCATION;
        return -1;
    }
    if (vk == 'M') return WA_MAX;
    if (vk == 'T') return WA_TOPMOST;
    if (vk == 'C') return WA_COPY_PATH;
    if (vk == VK_RIGHT) return WA_MON_NEXT;
    if (vk == VK_LEFT) return WA_MON_PREV;
    return -1;
}

// One line of feedback for a window action: refusals in amber, state changes in green.
static void WinNotice(const wchar_t* msg, const D2D1_COLOR_F& tone) {
    g_winNoticeMsg = msg;
    g_winNoticeTone = tone;
    g_winNoticeAt = NowSec();
    if (g_hwnd) SetTimer(g_hwnd, TIMER_COPY, 3000, nullptr);  // redraw once the notice expires
    MarkDirty();
}

static void WindowRefused(const wchar_t* msg = L"Needs administrator · can't control that window") {
    WinNotice(msg, kToneWarn);
}

// Re-reads the window list (a window was closed, by us or by its app) and keeps the selection near
// where it was instead of jumping to the top.
static void RefreshWindowResults() {
    const int prev = g_sel;
    g_windowsDirty = true;
    RunSearch();
    g_sel = g_results.empty() ? 0 : std::clamp(prev, 0, (int)g_results.size() - 1);
    UpdateTargets(false, /*followSelection=*/true);
    MarkDirty();
}

static void DoWindowAction(int act) {
    if (g_sel < 0 || g_sel >= (int)g_results.size()) return;
    const SearchResult r = g_results[g_sel];  // copy: RunSearch() rebuilds the vector below
    if (r.kind != ResultKind::Window) return;
    if (!IsWindow(r.hwnd)) {  // the window closed while the list was open
        RefreshWindowResults();
        return;
    }
    // Minimizing or closing the active window can hand the foreground to another one: without this
    // the bar would treat it as the user clicking away and close itself.
    g_focusGrace = NowSec() + 0.8;

    if (act == WA_CLOSE) {
        if (!g_winCloseArmed) {  // first Ctrl+W: warn, the second one closes
            g_winCloseArmed = true;
            if (g_hwnd) {
                KillTimer(g_hwnd, TIMER_CONFIRM);
                SetTimer(g_hwnd, TIMER_CONFIRM, 4000, nullptr);
            }
            MarkDirty();
            return;
        }
        g_winCloseArmed = false;
        if (g_hwnd) KillTimer(g_hwnd, TIMER_CONFIRM);
        if (!WinClose(r.hwnd)) {
            WindowRefused();
            return;
        }
        // The window may be gone, or still up asking about unsaved work: re-read the list either way,
        // and keep re-reading for a moment. Chromium-based apps (Helium, Chrome) destroy their window
        // asynchronously, so an immediate re-read still finds it and the row would never leave.
        g_closingWin = r.hwnd;
        g_closingTries = 0;
        if (g_hwnd) SetTimer(g_hwnd, TIMER_WINCLOSE, 250, nullptr);
        RefreshWindowResults();
        if (g_vis != Vis::Hidden) SetForegroundWindow(g_hwnd);
        return;
    }

    bool ok = true;
    bool noTarget = false;
    g_winCloseArmed = false;  // another action means the user moved on: no pending close
    switch (act) {
        case WA_MINIMIZE: ok = WinMinimize(r.hwnd); break;
        case WA_MAX:      ok = WinToggleMax(r.hwnd); break;
        case WA_TOPMOST:  ok = WinToggleTopmost(r.hwnd); break;
        case WA_MON_NEXT: {
            const int m = WinMoveMonitor(r.hwnd, +1);
            ok = m > 0;
            noTarget = m < 0;
            break;
        }
        case WA_MON_PREV: {
            const int m = WinMoveMonitor(r.hwnd, -1);
            ok = m > 0;
            noTarget = m < 0;
            break;
        }
        case WA_LOCATION: {
            const std::wstring path = WindowProcessPath(r.hwnd);
            const size_t cut = path.find_last_of(L'\\');
            ok = cut != std::wstring::npos;
            if (ok) RunShellOpen(path.substr(0, cut).c_str());
            break;
        }
        case WA_COPY_PATH: {
            const std::wstring path = WindowProcessPath(r.hwnd);
            ok = !path.empty();
            if (ok) {
                CopyTextToClipboard(path);
                FileCopied();
            }
            break;
        }
        default:
            return;  // WA_CLOSE is handled above
    }
    if (!ok) {
        if (noTarget) {
            WindowRefused(L"No other monitor in that direction");
        } else {
            WindowRefused();
        }
        return;
    }
    // The panel and the row badge show whether the window is minimized, maximized or on top.
    if (g_winPanel.hwnd == r.hwnd) FillWinPanel(r);
    if (act == WA_TOPMOST) {
        const bool top = (GetWindowLongPtrW(r.hwnd, GWL_EXSTYLE) & WS_EX_TOPMOST) != 0;
        if (g_sel >= 0 && g_sel < (int)g_results.size()) g_results[g_sel].onTop = top;
        WinNotice(top ? L"Always on top · Ctrl+Shift+T to undo" : L"No longer on top", kToneOk);
    }
    if (g_vis != Vis::Hidden) SetForegroundWindow(g_hwnd);
    MarkDirty();
}

// Helper: is the selected result previewable?
static bool SelIsPreviewable() {
    if (g_results.empty() || g_sel < 0 || g_sel >= (int)g_results.size()) return false;
    return g_results[g_sel].kind == ResultKind::File;
}

// Open or close the preview panel.  Handles the animated mode-switch when the
// actions panel is currently open: closes it first, then reopens in Preview.
static void SetPreviewOpen(bool open) {
    if (!open) {
        g_detailsOpen   = false;
        g_panelSwitching = false;
        Sound::OnPanel(false);
        UpdateTargets(false, false);
        MarkDirty();
        return;
    }
    if (!SelIsPreviewable()) return;

    // Path is in res.payload for File results; LoadPreview will be called from Render.

    if (!g_detailsOpen) {
        // Panel is closed — open directly in Preview mode
        g_panelMode    = PanelMode::Preview;
        g_panelSwitching = false;
        g_detailsOpen  = true;
        Sound::OnPanel(true);
        UpdateTargets(false, false);
        MarkDirty();
    } else if (g_panelMode == PanelMode::Actions) {
        // Panel is open in Actions mode — animate close, then reopen as Preview
        g_panelSwitching   = true;
        g_panelPendingMode = PanelMode::Preview;
        g_detailsOpen      = false;   // starts closing animation
        // No sound yet; will play when it reopens
        UpdateTargets(false, false);
        MarkDirty();
    } else {
        // Already in Preview mode — just keep it open (or toggle off if same file)
        // No-op for now
    }
}

// Toggle the preview panel (called from keyboard shortcut).
static void TogglePreview() {
    if (g_panelMode == PanelMode::Preview && g_detailsOpen) {
        SetPreviewOpen(false);
    } else {
        SetPreviewOpen(true);
    }
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

// AltGr (right Alt) generates a synthetic Ctrl+Alt combination on Windows; treat it as
// plain Alt so that AltGr characters (e.g. backslash on many European keyboards) are not
// blocked by the !CtrlDown() guard in WM_CHAR handlers.
static bool CtrlDown() {
    return (GetKeyState(VK_CONTROL) & 0x8000) != 0 &&
           (GetKeyState(VK_RMENU)   & 0x8000) == 0;   // AltGr is not Ctrl
}
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

// Keys while the path / rename field is being edited: it swallows everything.
static bool HandleBandKey(WPARAM vk, bool ctrl, bool shift) {
    const int len = (int)g_bandText.size();
    const bool sel = g_bandAnchor != g_bandCaret;
    switch (vk) {
        case VK_ESCAPE:
            g_bandEdit = BandEdit::None;
            g_bandError.clear();
            MarkDirty();
            return true;
        case VK_RETURN:
            if (g_bandEdit == BandEdit::Rename) CommitRename();
            else CommitPath();
            return true;
        case VK_TAB:
            BandComplete(shift);
            return true;
        case VK_LEFT:
            if (sel && !shift && !ctrl) BandMove(std::min(g_bandAnchor, g_bandCaret), false);
            else BandMove(ctrl ? BandPrevWord(g_bandCaret) : g_bandCaret - 1, shift);
            return true;
        case VK_RIGHT:
            if (sel && !shift && !ctrl) BandMove(std::max(g_bandAnchor, g_bandCaret), false);
            else BandMove(ctrl ? BandNextWord(g_bandCaret) : g_bandCaret + 1, shift);
            return true;
        case VK_HOME:
            BandMove(0, shift);
            return true;
        case VK_END:
            BandMove(len, shift);
            return true;
        case VK_BACK:
            if (sel) {
                BandReplace(L"");
            } else if (g_bandCaret > 0) {
                g_bandAnchor = ctrl ? BandPrevWord(g_bandCaret) : g_bandCaret - 1;
                BandReplace(L"");
            }
            return true;
        case VK_DELETE:
            if (sel) {
                BandReplace(L"");
            } else if (g_bandCaret < len) {
                g_bandAnchor = ctrl ? BandNextWord(g_bandCaret) : g_bandCaret + 1;
                BandReplace(L"");
            }
            return true;
        case 'A':
            if (ctrl) {
                g_bandAnchor = 0;
                g_bandCaret = len;
                MarkDirty();
            }
            return true;
        case 'C':
        case 'X':
            if (ctrl && sel) {
                const int a = std::min(g_bandAnchor, g_bandCaret);
                const int b = std::max(g_bandAnchor, g_bandCaret);
                CopyTextToClipboard(g_bandText.substr(a, b - a));
                if (vk == 'X') BandReplace(L"");
            }
            return true;
        case 'V':
            if (ctrl) {
                std::wstring t = ClipboardText();
                const size_t nl = t.find_first_of(L"\r\n");
                if (nl != std::wstring::npos) t.erase(nl);
                if (g_bandEdit == BandEdit::Path) t.erase(std::remove(t.begin(), t.end(), L'"'), t.end());
                if (!t.empty()) BandReplace(t);
            }
            return true;
    }
    return true;
}

// Keys while the actions menu is open.
static bool HandleFileMenuKey(WPARAM vk, bool ctrl, bool shift) {
    const int n = (int)g_fileMenu.size();
    if (IsDetailsKey(vk, ctrl, shift)) {
        g_fileMenuOpen = false;
        MarkDirty();
        return true;
    }
    auto move = [&](int to) {
        if (n == 0) return;
        g_fileMenuSel = (to % n + n) % n;
        g_fileMenuArmed = false;
        MarkDirty();
    };
    switch (vk) {
        case VK_ESCAPE:
            g_fileMenuOpen = false;
            g_fileMenuArmed = false;
            MarkDirty();
            return true;
        case VK_DOWN:
            move(g_fileMenuSel + 1);
            return true;
        case VK_UP:
            move(g_fileMenuSel - 1);
            return true;
        case VK_TAB:
            move(g_fileMenuSel + (shift ? -1 : 1));
            return true;
        case VK_HOME:
            move(0);
            return true;
        case VK_END:
            move(n - 1);
            return true;
        case VK_RETURN:
            FileMenuRunSel();
            return true;
    }
    return true;
}

static bool HandleKeyDown(WPARAM vk, bool repeat) {
    const bool ctrl = CtrlDown();
    const bool shift = ShiftDown();
    const int len = (int)g_text.size();
    const bool hasSel = g_anchor != g_caret;

    // Holding a key must not confirm a destructive action: the first press arms it and the repeat
    // would run it (Enter on "Shut Down", Delete in the entries manager, and so on).
    if (repeat && (g_confirmCmd >= 0 || g_fileMenuArmed || g_mgr.deleteArmed || g_winCloseArmed)) return true;

    if (g_bandEdit != BandEdit::None) return HandleBandKey(vk, ctrl, shift);
    if (g_fileMenuOpen) return HandleFileMenuKey(vk, ctrl, shift);
    // The custom-entry form and the manager are modal: they take every key while open.
    if (g_form.active) return HandleCustomFormKey(vk, ctrl, shift);
    if (g_mgr.active) return HandleManagerKey(vk, ctrl, shift);

    // The help sheet is modal: Esc or F1 closes it, Tab and the arrows switch pages and every
    // other key is swallowed, so a stray Enter cannot launch whatever was selected behind it.
    if (g_helpOpen) {
        if (vk == VK_ESCAPE || vk == VK_F1) {
            CloseHelp();
        } else if (vk == VK_TAB || vk == VK_LEFT || vk == VK_RIGHT) {
            const int page = (vk == VK_LEFT) ? 0 : (vk == VK_RIGHT) ? 1 : 1 - g_helpPage;
            if (page != g_helpPage) {
                g_helpPage = page;
                g_helpScroll = 0;
                MarkDirty();
            }
        } else if (vk == VK_UP || vk == VK_DOWN || vk == VK_PRIOR || vk == VK_NEXT ||
                   vk == VK_HOME || vk == VK_END) {
            const float rowH = 17 * (float)g_scale;
            const float pageH = std::max(rowH, g_helpViewH - rowH);
            float to = g_helpScroll;
            if (vk == VK_UP) to -= rowH;
            else if (vk == VK_DOWN) to += rowH;
            else if (vk == VK_PRIOR) to -= pageH;
            else if (vk == VK_NEXT) to += pageH;
            else if (vk == VK_HOME) to = 0;
            else to = g_helpMaxScroll;
            to = std::clamp(to, 0.f, std::max(0.f, g_helpMaxScroll));
            if (to != g_helpScroll) {
                g_helpScroll = to;
                MarkDirty();
            }
        }
        return true;
    }
    if (vk == VK_F1 && !ctrl && !shift) {
        OpenHelp();
        return true;
    }

    // Entry shortcuts are matched by the launcher window itself: they only work while the bar is
    // open, and they never take a combination away from Windows or another application.
    if (TryRunEntryHotkey(vk)) return true;

    // "win:" acts on the selected window. This comes before the generic Ctrl+Enter / Ctrl+Shift+C
    // handlers (which need an app index) and before CancelConfirm(), because the second Ctrl+W is
    // what confirms a close.
    if (g_activator >= 0 && kActivators[g_activator].mode == LauncherMode::WindowList) {
        const int act = WindowActionForKey(vk, ctrl, shift);
        if (act >= 0) {
            DoWindowAction(act);
            return true;
        }
    }

    // Any key other than Enter disarms a pending destructive confirmation.
    if (vk != VK_RETURN) CancelConfirm();

    // File mode: actions menu, path field, rename and the other file shortcuts.
    if (g_fileMode && g_activator < 0) {
        if (IsDetailsKey(vk, ctrl, shift)) {
            OpenFileMenu();
            return true;
        }
        if (ctrl && !shift && vk == 'L') {
            BeginPathEdit();
            return true;
        }
        if (ctrl && !shift && vk == 'S') {
            OpenSortMenu();  // the sort list, with the current order marked
            return true;
        }
        if (!ctrl && !shift && vk == VK_F2) {
            if (!InPcRoot() && CaptureFileTarget()) BeginRename(g_fileMenuTarget);
            return true;
        }
        if (ctrl && shift && vk == 'N') {
            FileNewFolder();
            return true;
        }
        if (ctrl && !shift && vk == 'T') {
            if (!CaptureFileTarget()) {  // empty folder: open the terminal in the folder itself
                g_fileMenuTarget = FileEntry();
                g_fileMenuTarget.path = g_fileMenuTarget.dir = g_fileCur;
                g_fileMenuTarget.isDir = true;
                g_fileMenuHasTarget = !g_fileCur.empty();
            }
            RunFileAction(FA_TERMINAL);
            return true;
        }
        if (ctrl && !shift && vk == 'G') {
            if (CaptureFileTarget()) RunFileAction(FA_GOTO);
            return true;
        }
        if (ctrl && !shift && vk == 'P') {
            if (CaptureFileTarget()) RunFileAction(FA_PROPS);
            return true;
        }
        if (ctrl && shift && vk == VK_RETURN) {
            if (CaptureFileTarget() && !g_fileMenuTarget.isDir && IsExeLike(g_fileMenuTarget.name)) {
                RunFileAction(FA_ADMIN);
            }
            return true;
        }
    }

    // Details panel: shortcut to show/hide it, and direct keyboard actions
    if (IsDetailsKey(vk, ctrl, shift)) {
        SetDetailsOpen(!g_detailsOpen);
        return true;
    }
    // Preview panel: Ctrl+Q (Quick Look) toggles the file preview panel.
    if (ctrl && vk == 'Q' && !shift) {
        if (SelIsPreviewable()) {
            TogglePreview();
        }
        return true;
    }
    if (ctrl && vk == VK_RETURN && !shift && g_sel >= 0 && g_sel < (int)g_results.size() &&
        IsPathResult(g_results[g_sel].kind)) {
        const std::wstring& path = g_results[g_sel].payload;
        const std::wstring dir = g_results[g_sel].isDir ? path : ParentDir(path);
        if (!dir.empty()) RunShellOpen(dir.c_str());
        return true;
    }
    if (ctrl && vk == VK_RETURN && !shift && g_sel >= 0 && g_sel < (int)g_results.size() &&
        (g_results[g_sel].kind == ResultKind::Bookmark ||
         g_results[g_sel].kind == ResultKind::Clip)) {
        // A bookmark has no file location; copy its address instead, leaving the bar open with
        // the footer confirming it, like the file browser's copies. A clipboard entry is copied
        // the same way (Enter pastes it), keeping the image when it is one.
        if (g_results[g_sel].kind == ResultKind::Clip && g_results[g_sel].image) {
            SetClipboardImage(g_results[g_sel].payload);
        } else {
            CopyTextToClipboard(g_results[g_sel].payload);
        }
        FileCopied();
        return true;
    }
    if (ctrl && vk == VK_RETURN && g_fileMode && !shift) {
        FileReveal();
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
    if (ctrl && !shift && vk == 'D') {
        if (g_activator >= 0 && kActivators[g_activator].mode == LauncherMode::Clipboard &&
            g_sel >= 0 && g_sel < (int)g_results.size() &&
            g_results[g_sel].kind == ResultKind::Clip) {
            ToggleSelectedClipPin();
            return true;
        }
        if (g_sel >= 0 && g_sel < (int)g_results.size() &&
            g_results[g_sel].kind == ResultKind::App) {
            const AppEntry& a = g_apps[g_results[g_sel].app];
            RunActionOnSelection(a.pinned ? ACT_UNPIN : ACT_PIN);
        }
        return true;
    }
    if (ctrl && shift && vk == 'C' && g_fileMode && g_sel >= 0 && g_sel < (int)g_results.size() &&
        g_results[g_sel].kind == ResultKind::File) {
        CopyTextToClipboard(g_results[g_sel].payload);
        FileCopied();
        return true;
    }
    if (ctrl && shift && vk == 'C' && g_sel >= 0 && g_sel < (int)g_results.size() &&
        IsPathResult(g_results[g_sel].kind)) {
        CopyTextToClipboard(g_results[g_sel].payload);
        FileCopied();
        return true;
    }
    if (ctrl && shift && vk == 'C') {
        RunActionOnSelection(ACT_COPYPATH);
        return true;
    }
    if (ctrl && shift && vk == 'R') {
        // "clip:" removes the selected clipboard entry with the same shortcut.
        if (g_activator >= 0 && kActivators[g_activator].mode == LauncherMode::Clipboard &&
            g_sel >= 0 && g_sel < (int)g_results.size() &&
            g_results[g_sel].kind == ResultKind::Clip) {
            RemoveSelectedClip();
            return true;
        }
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

    if (g_fileMode && g_activator < 0 && ctrl && !shift) {
        // Ctrl+1..9 jumps to the Nth chip (locations first, then drives); more than nine chips are
        // reached with Ctrl+Left/Right or Ctrl+Tab.
        if (vk >= '1' && vk <= '9' && (int)(vk - '1') < (int)g_chips.size()) {
            SetFileLocation((int)(vk - '1'));
            return true;
        }
        if (g_text.empty() && (vk == VK_LEFT || vk == VK_RIGHT)) {
            // Outside every chip (the folder does not belong to one) both keys land on the first.
            if (g_fileLoc < 0) {
                SetFileLocation(0);
            } else {
                SetFileLocation(std::clamp(g_fileLoc + (vk == VK_LEFT ? -1 : 1), 0,
                                           (int)g_chips.size() - 1));
            }
            return true;
        }
    }

    // Preview panel (text files): Up/Down/PgUp/PgDn/Home/End scroll the lines and Left/Right scroll
    // sideways, instead of navigating results or moving the caret / changing folder.
    // Left/Right are only taken when the text really is wider than the panel.
    if (g_panelMode == PanelMode::Preview && g_detailsOpen && g_preview.isText && !ctrl &&
        (vk == VK_DOWN || vk == VK_UP || vk == VK_PRIOR || vk == VK_NEXT ||
         vk == VK_HOME || vk == VK_END ||
         ((vk == VK_LEFT || vk == VK_RIGHT) && !shift && PreviewGeom().hbar))) {
        const PreviewTextGeom tg = PreviewGeom();
        const int total    = (int)g_preview.lines.size();
        const int maxV     = std::max(0, total - tg.visLines);
        const int maxH     = std::max(0, g_preview.maxCols - tg.visCols);
        const int pageSize = std::max(1, tg.visLines - 1);
        if      (vk == VK_DOWN)  g_preview.scroll = std::min(g_preview.scroll + 1, maxV);
        else if (vk == VK_UP)    g_preview.scroll = std::max(g_preview.scroll - 1, 0);
        else if (vk == VK_NEXT)  g_preview.scroll = std::min(g_preview.scroll + pageSize, maxV);
        else if (vk == VK_PRIOR) g_preview.scroll = std::max(g_preview.scroll - pageSize, 0);
        else if (vk == VK_HOME)  g_preview.scroll = 0;
        else if (vk == VK_END)   g_preview.scroll = maxV;
        else if (vk == VK_RIGHT) g_preview.hscroll = std::min(g_preview.hscroll + kPrevHStep, maxH);
        else if (vk == VK_LEFT)  g_preview.hscroll = std::max(g_preview.hscroll - kPrevHStep, 0);
        MarkDirty();
        return true;
    }

    switch (vk) {
        case VK_ESCAPE:
            if (g_panelMode == PanelMode::Preview && g_detailsOpen) {
                SetPreviewOpen(false);  // close preview first; another Esc closes the bar
            } else if (g_detailsOpen) {
                SetDetailsOpen(false);
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
                CopyTextToClipboard(CalcClipboardText());
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
            // Ctrl+Tab hops to the next ready drive in the file browser (Ctrl+Shift+Tab the
            // previous one); the profile's drive lands on Home, the others on their root.
            if (g_fileMode && g_activator < 0 && ctrl) {
                CycleDrive(shift ? -1 : 1);
                return true;
            }
            // In a calculator context Tab keeps its meaning: pull a history row in for editing,
            // otherwise move the selection.
            if (CalcHistoryVisible() || CalcShown()) {
                if (CalcHistoryVisible() && g_calcHistSel >= 0 &&
                    g_calcHistSel < (int)g_calcHist.size()) {
                    EditCalcHistoryOp(g_calcHistSel);
                } else {
                    SelectDelta(shift ? -1 : 1);
                }
                return true;
            }
            // Otherwise Tab alternates between searching apps and files (outside a prefix mode),
            // or just moves the selection when the file browser is disabled.
            if (g_activator < 0 && g_cfg.filesEnabled) {
                ToggleFileMode();
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
            if (g_fileMode && g_activator < 0 && !hasSel && !shift && !ctrl && g_text.empty()) {
                FileUp();  // empty field: Left goes to the parent folder
                return true;
            }
            if (hasSel && !shift) {
                MoveCaret(std::min(g_anchor, g_caret), false);
            } else {
                MoveCaret(ctrl ? PrevWord(g_caret) : g_caret - 1, shift);
            }
            return true;
        case VK_RIGHT:
            if (g_fileMode && g_activator < 0 && !hasSel && !shift && !ctrl && g_caret == len &&
                SelectedFileIsDir()) {
                LaunchSelected();  // Right enters the highlighted folder
                return true;
            }
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
            } else if (g_fileMode && g_activator < 0 && g_text.empty()) {
                // Empty field: the drives view and a location chip root leave file mode; a drive
                // root steps up to the drives view (FileUp); anything else goes up a folder.
                bool leave = InPcRoot();
                for (int i = 0; i < kFileLocationCount && !leave; ++i) {
                    if (SamePath(ResolveFileLocation(i), g_fileCur)) leave = true;
                }
                if (leave) {
                    ToggleFileMode();
                } else {
                    FileUp();
                }
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
// Row of the actions menu under a client point (-1 outside any row); `inside` tells if the point
// is over the popup at all.
static int HitFileMenu(int mx, int my, bool& inside) {
    const float s = (float)g_scale;
    float cx = 0, cy = 0;
    MainOrigin(cx, cy);
    float x0, y0, x1, y1, ih;
    FileMenuRect(cx, cy, kWidth * s, x0, y0, x1, y1, ih);
    inside = mx >= x0 && mx < x1 && my >= y0 && my < y1;
    if (!inside) return -1;
    const int i = (int)std::floor(((float)my - (y0 + 5 * s)) / ih);
    return (i >= 0 && i < (int)g_fileMenu.size()) ? i : -1;
}

static int HitRow(int mx, int my) {
    const float s = (float)g_scale;
    float cx = 0, cy = 0;
    MainOrigin(cx, cy);
    const float cw = kWidth * s;
    const int rows = VisibleRows();
    if (rows <= 0) return -1;
    if (mx < cx || mx > cx + cw) return -1;

    const float rowsOrigin = cy + (kBarH + kListPad + (float)HeroH() + (float)FileChipsH()) * s;
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
            } else if (wp == TIMER_FSEARCH) {
                KillTimer(g_hwnd, TIMER_FSEARCH);
                StartFileSearch();
            } else if (wp == TIMER_GSEARCH) {
                KillTimer(g_hwnd, TIMER_GSEARCH);
                StartGlobalSearch();
            } else if (wp == TIMER_EVFALLBACK) {
                AbandonEverythingSearch();
            } else if (wp == TIMER_WINCLOSE) {
                KillTimer(g_hwnd, TIMER_WINCLOSE);
                const bool stillThere = g_closingWin && IsWindow(g_closingWin);
                if (g_vis != Vis::Hidden && g_activator >= 0 &&
                    kActivators[g_activator].mode == LauncherMode::WindowList) {
                    RefreshWindowResults();
                } else {
                    g_windowsDirty = true;  // the next "win:" reads the list fresh
                }
                // A Chromium window can take a second or two to go; keep checking for a bounded time.
                if (stillThere && ++g_closingTries < 8 && g_hwnd) {
                    SetTimer(g_hwnd, TIMER_WINCLOSE, 250, nullptr);
                } else {
                    g_closingWin = nullptr;
                }
            } else if (wp == TIMER_HIDE) {
                KillTimer(g_hwnd, TIMER_HIDE);
                if (g_vis == Vis::Hiding) FinishHide();
            } else if (wp == TIMER_CONFIRM) {
                KillTimer(g_hwnd, TIMER_CONFIRM);
                if (g_winCloseArmed) {
                    g_winCloseArmed = false;
                    MarkDirty();
                }
                if (g_confirmCmd >= 0) {
                    g_confirmCmd = -1;
                    MarkDirty();
                }
            } else if (wp == TIMER_BLINK) {
                if (g_vis == Vis::Shown && !IgnoreFocusLoss() && GetForegroundWindow() != g_hwnd &&
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
            if (LOWORD(wp) == WA_INACTIVE && g_vis != Vis::Hidden && !IgnoreFocusLoss() &&
                NowSec() - g_shownAt > 0.15) {
                HideLauncher();
            } else if (LOWORD(wp) == WA_ACTIVE && g_cfg.disableFullscreen &&
                       g_vis != Vis::Hidden && IsForegroundFullscreen()) {
                HideLauncher();
            }
            return 0;

        case WM_KEYDOWN:
            if (HandleKeyDown(wp, (lp & 0x40000000) != 0)) return 0;  // bit 30: key auto-repeat
            break;

        case WM_CHAR: {
            wchar_t c = (wchar_t)wp;
            if (g_helpOpen) return 0;
            if (g_fileMenuOpen) return 0;
            if (g_form.active) {
                // Typing goes into the focused form field, never into the search bar behind it.
                if (c >= 32 && c != 127 && !CtrlDown()) {
                    FormReplaceSelection(std::wstring(1, c));
                    Sound::OnKey(c);
                }
                return 0;
            }
            if (g_mgr.active) {
                // Typing goes into the focused editor field (the Type selector takes no text, and
                // the Shortcut field is filled by capturing a key combination, never by typing).
                if (g_mgr.editing && g_mgr.field != kMgrFieldHotkey && c >= 32 && c != 127 &&
                    !CtrlDown()) {
                    ManagerReplaceSelection(std::wstring(1, c));
                    Sound::OnKey(c);
                }
                return 0;
            }
            if (g_bandEdit != BandEdit::None) {
                if (c >= 32 && c != 127) {
                    if (g_bandEdit == BandEdit::Rename && wcschr(L"\\/:*?\"<>|", c)) {
                        BandFail(L"Invalid character");
                    } else {
                        BandReplace(std::wstring(1, c));
                        Sound::OnKey(c);
                    }
                }
                return 0;
            }
            if (c == L' ' && CtrlDown()) return 0;  // Ctrl+Space (panel shortcut) does not type
            if (c >= 32 && c != 127) {
                ReplaceSelection(std::wstring(1, c));
                Sound::OnKey(c);
            }
            return 0;
        }

        // Alt held after the shortcut: do not open system menus or beep
        case WM_SYSKEYDOWN:
            // Alt+key arrives here, not as WM_KEYDOWN: the manager's Shortcut field still needs it.
            if (g_mgr.active && g_mgr.editing && g_mgr.field == kMgrFieldHotkey) {
                if (HandleKeyDown(wp, (lp & 0x40000000) != 0)) return 0;
            }
            // Entry shortcuts that use Alt are matched here, only while the bar is open.
            if (!g_mgr.active && !g_form.active && !g_helpOpen && TryRunEntryHotkey(wp)) {
                return 0;
            }
            if (g_fileMode && g_activator < 0 && g_bandEdit == BandEdit::None && !g_fileMenuOpen) {
                if (wp == 'D') BeginPathEdit();
                else if (wp == VK_LEFT) FileBack();
                else if (wp == VK_RIGHT) FileForward();
                else if (wp == VK_UP) FileUp();
                else if (wp == VK_HOME) SetFileLocation(0);
            }
            return 0;
        case WM_XBUTTONDOWN:
            if (g_fileMode && g_activator < 0 && g_bandEdit == BandEdit::None && !g_fileMenuOpen) {
                if (GET_XBUTTON_WPARAM(wp) == XBUTTON1) FileBack();
                else if (GET_XBUTTON_WPARAM(wp) == XBUTTON2) FileForward();
            }
            return TRUE;
        case WM_SYSKEYUP:
        case WM_SYSCHAR:
            return 0;
        case WM_SYSCOMMAND:
            if ((wp & 0xFFF0) == SC_KEYMENU) return 0;
            break;

        case WM_MOUSEMOVE: {
            POINT p = {GET_X_LPARAM(lp), GET_Y_LPARAM(lp)};
            if (g_fileMenuOpen) {
                bool inside = false;
                const int h = HitFileMenu(p.x, p.y, inside);
                if (h >= 0 && h != g_fileMenuSel) {
                    g_fileMenuSel = h;
                    g_fileMenuArmed = false;
                    MarkDirty();
                }
                return 0;
            }
            if (g_helpOpen) return 0;  // no hover on the hidden list
            if (g_form.active) return 0;  // the form has no hover highlight
            if (g_mgr.active)  return 0;  // neither does the manager
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
            const bool overHelp = HitHelp(p.x, p.y);
            if (act != g_hoverAction || overReindex != g_hoverReindex || overHelp != g_hoverHelp) {
                g_hoverAction = act;
                g_hoverReindex = overReindex;
                g_hoverHelp = overHelp;
                MarkDirty();
            }
            return 0;
        }

        case WM_MOUSEWHEEL: {
            // The wheel scrolls the list; it does not change the selected app. The
            // selection stays where it is (or follows the mouse hover, which is handled in
            // WM_MOUSEMOVE).
            const int delta = GET_WHEEL_DELTA_WPARAM(wp);
            if (g_helpOpen) {
                const float step = (delta > 0 ? -40.f : 40.f) * (float)g_scale;
                const float to = std::clamp(g_helpScroll + step, 0.f, std::max(0.f, g_helpMaxScroll));
                if (to != g_helpScroll) {
                    g_helpScroll = to;
                    MarkDirty();
                }
                return 0;
            }
            if (g_form.active) return 0;  // neither does the form
            if (g_mgr.active)  return 0;  // neither does the manager
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
            if (g_helpOpen) {
                float hx = 0, hy = 0;
                MainOrigin(hx, hy);
                const int mx = GET_X_LPARAM(lp), my = GET_Y_LPARAM(lp);
                for (int i = 0; i < 2; ++i) {
                    const D2D1_RECT_F tr = HelpTabRect(hx, hy, i);
                    if (mx >= tr.left && mx < tr.right && my >= tr.top && my < tr.bottom) {
                        g_helpPage = i;
                        MarkDirty();
                        return 0;
                    }
                }
                CloseHelp();
                return 0;
            }
            if (g_fileMenuOpen) {
                bool inside = false;
                const int h = HitFileMenu(GET_X_LPARAM(lp), GET_Y_LPARAM(lp), inside);
                if (h >= 0) {
                    g_fileMenuSel = h;
                    FileMenuRunSel();
                } else if (!inside) {
                    g_fileMenuOpen = false;
                    g_fileMenuArmed = false;
                    MarkDirty();
                }
                return 0;
            }
            // The custom-entry form is modal: clicking a field focuses it and places the caret;
            // clicking the search bar goes back to the list so the query can be edited.
            if (g_form.active) {
                const int mx = GET_X_LPARAM(lp), my = GET_Y_LPARAM(lp);
                if (CustomFormClick(mx, my)) return 0;
                float bx = 0, by = 0;
                MainOrigin(bx, by);
                const bool inBar = mx >= bx && mx < bx + kWidth * (float)g_scale && my >= by &&
                                   my < by + kBarH * (float)g_scale;
                if (!inBar) return 0;  // clicks outside the fields do nothing
                CloseCustomForm();
            }
            if (g_mgr.active) return 0;  // manager is keyboard-only; ignore clicks on the list
            if (g_bandEdit != BandEdit::None) {
                float bx = 0, by = 0;
                MainOrigin(bx, by);
                float fx0, fy0, fx1, fy1;
                BandFieldRect(bx, by, kWidth * (float)g_scale, fx0, fy0, fx1, fy1);
                const int mx = GET_X_LPARAM(lp), my = GET_Y_LPARAM(lp);
                if (mx >= fx0 && mx < fx1 && my >= fy0 && my < fy1) {
                    BandMove(BandIndexAt(bx, by, kWidth * (float)g_scale, mx), ShiftDown());
                    return 0;
                }
                g_bandEdit = BandEdit::None;  // a click elsewhere cancels, then acts normally
                MarkDirty();
            }
            // Apps/Files toggle button and the file-mode location chips (inside the bar).
            if (g_activator < 0) {
                float ocx = 0, ocy = 0;
                MainOrigin(ocx, ocy);
                const int mx = GET_X_LPARAM(lp), my = GET_Y_LPARAM(lp);
                const D2D1_RECT_F tr = FileToggleRect(ocx, ocy);
                if (mx >= tr.left && mx < tr.right && my >= tr.top && my < tr.bottom) {
                    ToggleFileMode();
                    return 0;
                }
                if (g_fileMode) {
                    const D2D1_RECT_F xr = FileModeChipCloseRect(ocx, ocy);
                    if (mx >= xr.left && mx < xr.right && my >= xr.top && my < xr.bottom) {
                        ToggleFileMode();  // the chip's close button leaves file mode
                        return 0;
                    }
                    for (int i = 0; i < (int)g_chips.size(); ++i) {
                        const D2D1_RECT_F cr = FileChipRect(ocx, ocy, i);
                        if (mx >= cr.left && mx < cr.right && my >= cr.top && my < cr.bottom) {
                            SetFileLocation(i);
                            return 0;
                        }
                    }
                }
            }
            if (g_fileMode && g_activator < 0) {
                float bcx = 0, bcy = 0;
                MainOrigin(bcx, bcy);
                const D2D1_RECT_F pb = FilePathBackRect(bcx, bcy);
                const int px = GET_X_LPARAM(lp), py = GET_Y_LPARAM(lp);
                if (px >= pb.left && px < pb.right && py >= pb.top && py < pb.bottom) {
                    FileBack();
                    return 0;
                }
                const float bandTop = bcy + (kBarH + FileChipRowsH()) * (float)g_scale;
                if (px >= bcx + 38 * (float)g_scale && px < bcx + kWidth * (float)g_scale &&
                    py >= bandTop && py < bandTop + kPathH * (float)g_scale) {
                    BeginPathEdit();  // click on the path: edit it
                    return 0;
                }
            }
            if (HitHelp(GET_X_LPARAM(lp), GET_Y_LPARAM(lp))) {
                OpenHelp();
                return 0;
            }
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

        case WM_APP_FILES: {
            auto* r = (FileSearchResult*)lp;
            if (r->gen == g_fileSearchGen && g_fileMode && r->key == FileSearchKey()) {
                const int oldSel = g_sel;
                std::wstring keep;
                if (oldSel > 0 && oldSel < (int)g_results.size()) keep = g_results[oldSel].payload;
                ReleaseResultLayouts();
                g_results.clear();
                g_fileDeep = std::move(r->entries);
                g_fileDeepKey = r->key;
                g_fileDeepTrunc = r->truncated;
                g_fileDeepDone = r->done;
                g_fileSearching = !r->done;
                RunSearch();
                int restored = -1;
                for (size_t i = 0; i < g_results.size() && !keep.empty(); ++i) {
                    if (g_results[i].payload == keep) {
                        restored = (int)i;
                        break;
                    }
                }
                g_sel = restored >= 0 ? restored : std::min(oldSel, (int)g_results.size() - 1);
                if (g_sel < 0) g_sel = 0;
                // Partial snapshots must not yank the view: keep the scroll unless the user had
                // actually moved the highlight (then keep that row in sight).
                if (r->done && restored < 0 && oldSel == 0) g_scrollTarget = 0;
                UpdateTargets(false, /*followSelection=*/restored >= 0);
                MarkDirty();
            }
            delete r;
            return 0;
        }

        case WM_APP_THUMB: {
            auto* tr = (ThumbResult*)lp;
            g_thumbPending.erase(tr->path);
            if (g_rt) {  // hidden or between render targets: drop it, it will be requested again
                ThumbCacheItem item;
                if (tr->ok) {
                    D2D1_BITMAP_PROPERTIES bp = D2D1::BitmapProperties(
                        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
                                          D2D1_ALPHA_MODE_PREMULTIPLIED), 96.f, 96.f);
                    g_rt->CreateBitmap(D2D1::SizeU((UINT32)tr->w, (UINT32)tr->h), tr->px.data(),
                                       (UINT32)tr->w * 4, &bp, &item.bmp);
                }
                item.failed = item.bmp == nullptr;
                item.used = ++g_thumbClock;
                ThumbCacheItem& slot = g_thumbCache[tr->path];
                SafeRelease(slot.bmp);
                slot = item;
                TrimThumbCache();
                MarkDirty();
            }
            delete tr;
            return 0;
        }

        case WM_APP_BOOKMARKS: {
            auto* r = (BookmarkResult*)lp;
            if (r->gen == g_bookmarkGen) {
                g_bookmarks = std::move(r->entries);
                if (g_vis != Vis::Hidden && !g_form.active && !g_mgr.active && !g_helpOpen) {
                    const int keep = g_sel;
                    RunSearch();
                    g_sel = std::clamp(keep, 0, std::max(0, (int)g_results.size() - 1));
                    UpdateTargets(false, /*followSelection=*/false);
                    MarkDirty();
                }
            }
            delete r;
            return 0;
        }

        case WM_APP_RECENT: {
            auto* r = (RecentLoadResult*)lp;
            if (r->gen == g_recentGen) {
                g_recent = std::move(r->entries);
                g_recentStamp = r->stamp;
                g_recentLoaded = true;
                if (g_vis != Vis::Hidden && !g_form.active && !g_mgr.active && !g_helpOpen) {
                    const int keep = g_sel;
                    RunSearch();
                    g_sel = std::clamp(keep, 0, std::max(0, (int)g_results.size() - 1));
                    UpdateTargets(false, /*followSelection=*/false);
                    MarkDirty();
                }
            }
            delete r;
            return 0;
        }

        case WM_APP_GLOBAL: {
            auto* r = (GlobalSearchResult*)lp;
            if (r->gen == g_globalSearchGen && g_activator >= 0 &&
                kActivators[g_activator].mode == LauncherMode::GlobalFiles) {
                const int oldSel = g_sel;
                std::wstring keep;
                if (oldSel >= 0 && oldSel < (int)g_results.size()) keep = g_results[oldSel].payload;
                g_globalFiles = std::move(r->entries);
                g_globalKey = r->key;
                g_globalDone = r->done;
                g_globalSearching = !r->done;
                g_globalTrunc = r->truncated;
                RunSearch();
                int restored = -1;
                for (size_t i = 0; i < g_results.size() && !keep.empty(); ++i) {
                    if (g_results[i].payload == keep) {
                        restored = (int)i;
                        break;
                    }
                }
                g_sel = restored >= 0 ? restored : std::min(oldSel, (int)g_results.size() - 1);
                if (g_sel < 0) g_sel = 0;
                UpdateTargets(false, /*followSelection=*/restored >= 0);
                MarkDirty();
            }
            delete r;
            return 0;
        }

        case WM_APP_MENU_CHANGED:
            g_menuDirty = true;
            MaybeReindexInBackground();
            return 0;

        case WM_CLIPBOARDUPDATE:
            CaptureClipboard();
            return 0;

        case WM_COPYDATA: {
            // Everything answers a "find:" query here (see EverythingSendQuery).
            const COPYDATASTRUCT* cds = (const COPYDATASTRUCT*)lp;
            if (cds && g_evPending && cds->dwData == g_evPendingSeq) {
                AcceptEverythingReply(cds);
                return TRUE;
            }
            break;
        }

        case WM_APP_EVERYTHING:
            FinishEverythingSearch();
            return 0;

        case WM_APP_SETTINGS: {
            const bool oldHideUtility = g_cfg.hideUtility;
            const bool hadRememberSort = g_cfg.rememberFileSort;
            const bool hadClipboard = g_cfg.clipboardHistory;
            const FileSort liveSort = g_cfg.fileSort;
            g_legacySettings = false;  // the user saved the new layout: its values are the real ones
            g_movedSettings = false;   // and they now live under the new sections
            Wh_SetIntValue(kSettingsSectionsKey, 1);
            LoadSettings();
            if (!hadRememberSort && g_cfg.rememberFileSort) {
                // The setting was just turned on: keep the order that is on screen and save it.
                g_cfg.fileSort = liveSort;
                Wh_SetIntValue(FileSortKey().c_str(), (int)liveSort);
            } else {
                // Turning it off (or leaving it off) falls back to Name A-Z.
                RestoreFileSort();
            }
            g_fileStartUsed = false;  // re-arm the start drive so a freshly set value takes effect
            Sound::Configure();
            Sound::ApplyVolume();
            if (g_cfg.hideUtility != oldHideUtility) {
                g_menuDirty = true;  // the index must be rebuilt to apply this
            }
            RegisterHotkeyOrWarn();
            UpdateClipboardListener();
            if (!g_cfg.clipboardHistory && hadClipboard) {
                EraseClipHistory();  // switching the feature off must not leave copies behind
            }
            if (g_cfg.clipboardHistory && g_clips.empty()) LoadClipHistory();
            // The global roots or the query context may have changed: start "find:" from scratch.
            CancelGlobalSearch();
            g_globalFiles.clear();
            g_globalKey.clear();
            g_globalDone = false;
            if (g_cfg.bookmarks) {
                g_bookmarkCheckedAt = -1e9;  // a settings change is not throttled
                RefreshBookmarksIfStale();
            } else {
                ++g_bookmarkGen;  // drop any load still in flight
                g_bookmarks.clear();
                g_bookmarkStamp = 0;  // re-enabling loads from scratch
            }
            if (g_cfg.recentFiles) {
                g_recentCheckedAt = -1e9;  // a settings change is not throttled
                RefreshRecentIfStale();
            } else {
                ++g_recentGen;  // drop any load still in flight
                g_recent.clear();
                g_recentStamp = 0;
                g_recentLoaded = false;
            }
            if (g_vis != Vis::Hidden) FinishHide();
            MaybeReindexInBackground();  // e.g. if switched to "Automatic" with pending changes
            return 0;
        }

        case WM_DESTROY:
            RemoveClipboardFormatListener(hwnd);
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
    g_fileSearchIdle = CreateEventW(nullptr, TRUE, TRUE, nullptr);
    g_globalSearchIdle = CreateEventW(nullptr, TRUE, TRUE, nullptr);
    DetectLegacySettings();
    DetectMovedSettings();
    LoadSettings();
    RestoreFileSort();  // the remembered Files order, when "Remember file sort" is on
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
    if (g_cfg.clipboardHistory) {
        LoadClipHistory();
    } else if (ClipHistoryOnDisk()) {
        // Switched off while the mod was not running: nothing it saved may stay on disk. With
        // nothing stored, touching the storage here would create the folder and write empty values
        // for every account that never enabled the feature.
        LoadClipHistory();
        EraseClipHistory();
    }
    LoadLearn();  // learned picks are tiny: always read them, used only while enabled
    UpdateClipboardListener();
    StartBookmarkLoad();  // async: the first open may use an empty list, the next one has it
    RefreshRecentIfStale();  // async: Windows' Recent files, same pattern
    StartIndex();
    StartWatcher();  // always runs (it is almost free); only used in Automatic mode

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    StopWatcher();
    CancelFileSearch();
    CancelGlobalSearch();
    if (g_fileSearchIdle) {
        if (WaitForSingleObject(g_fileSearchIdle, 3000) == WAIT_OBJECT_0) {
            CloseHandle(g_fileSearchIdle);
            g_fileSearchIdle = nullptr;
        }
    }
    if (g_globalSearchIdle) {
        if (WaitForSingleObject(g_globalSearchIdle, 3000) == WAIT_OBJECT_0) {
            CloseHandle(g_globalSearchIdle);
            g_globalSearchIdle = nullptr;
        }
    }
    StopThumbThread();
    ++g_bookmarkGen;  // a load still in flight must not touch a dying window
    ++g_recentGen;    // same for Recent files
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
    Wh_SetIntValue(kSettingsGroupedKey, 1);  // from now on only the grouped names are read
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
