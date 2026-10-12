// ==WindhawkMod==
// @id              spotlight-launcher
// @name            Spotlight Launcher
// @description     Application launcher. Alt+Space opens a floating search bar that finds your programs instantly.
// @version         0.5.11
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

Spotlight Launcher indexes the same applications you see in the Start menu (classic programs and Microsoft Store / MSIX apps) and filters them as you type. It runs in its own `windhawk.exe` process, so it never gets injected into Explorer and cannot affect the shell.

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
* Multi-word queries (`visual code`) and acronyms (`vsc` finds *Visual Studio Code*).
* Matching letters are highlighted in the result.
* Results show a friendly name plus the real target (exe, `.lnk`, MSIX package).

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

**Alt + Space** is also the system shortcut for a window's title-bar menu (Move, Size, Close…). While the mod is enabled with the default shortcut, that menu is not reachable by keyboard, so pick another combination in the settings if you use it. Apps like PowerToys Run make the same trade-off.

### Details panel

Press **Ctrl + I** (or **Ctrl + K** / **Ctrl + Space** / **F4**) with an app selected to reveal a right-hand panel with actions and technical data. The actions also work with the panel closed. File actions are only offered for desktop apps; Store (MSIX) apps only expose *open* and *remove from recents*.

### Appearance

* Six background effects (**Acrylic**, **Mica**, **Glass**, **Frosted**, **Aurora**, **Solid**) and 12 colours.
* Effects capture what is behind the bar on open, blur it and composite it inside the rounded card (tint, grain, highlights, gradients) — so they look the same on Windows 10 and 11 without undocumented DWM APIs.
* Adjustable panel opacity, blur strength and layout.
* Adapts to the DPI of each monitor.
* The bar can sit anywhere from against the top edge to against the bottom edge of the screen.

### Fresh without fuss

* **Automatic** index updates watch the Start menu folders and re-read the list when an app is installed or removed, with a light periodic fallback for Store apps.
* Can also re-index on every open or on a fixed interval (1 / 3 / 5 / 10 minutes).
* Installers write an "Uninstall …" shortcut for everything they install; the mod inspects what each shortcut actually runs and hides the uninstallers and documentation, while keeping real tools whose names merely contain "uninstall".
* If reindexing finishes with the search bar already open, the list updates by itself.
* Only what Windows lists as an app appears: a portable program without a shortcut in the Start menu is not indexed. A freshly installed app may take a few seconds to appear, until Windows registers its shortcut.

### Safe by design

* Runs as a Windhawk **tool** mod in its own process, not inside Explorer.
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
  $description: Combination to open and close the search bar.
  $options:
    - alt+space: Alt + Space
    - ctrl+space: Ctrl + Space
    - ctrl+alt+space: Ctrl + Alt + Space
    - shift+alt+space: Shift + Alt + Space
    - ctrl+shift+space: Ctrl + Shift + Space
- VerticalPosition: 30
  $name: Vertical position (percentage of screen height)
  $description: Where the search bar sits. 0 puts it against the top edge of the screen, 50 in the exact middle and 100 against the bottom edge (above the taskbar). Please note that this only applies to the search box in Compact mode. If you want it centered even in Expanded mode, adjust it to your preference.
- MaxResults: 8
  $name: Maximum results
  $description: Maximum number of results the search returns. It does not change the height of the bar (which is fixed). It only caps how many results exist. The rest of the visible rows are reached by scrolling the list.
- Opacity: 90
  $name: Panel opacity (percentage)
  $description: Between 70 and 100. It is the opacity of the color tint over the blurred background. 70 lets a lot of the background through and 100 makes the tint nearly opaque (Glass stays see-through). Ignored with "Solid", which is always fully opaque.
- Effect: acrylic
  $name: Background effect
  $description: Look of the search bar background. The effects use a capture of what is behind it, taken on open. "Solid" applies no effect.
  $options:
    - acrylic: Acrylic
    - mica: Mica
    - glass: Glass
    - frosted: Frosted
    - aurora: Aurora
    - solid: Solid (no effect)
- BlurAmount: 50
  $name: Blur strength (0 to 100)
  $description: Multiplies each effect's own blur. 50 is normal, 0 reduces it to 40% (it is not turned off) and 100 raises it to 160%. The blur is only seen through the tint, so combine it with an Opacity below 100. Ignored with "Solid".
- Color: midnight
  $name: Color
  $description: Color of the search bar. Each color defines the background, the selection accent and the highlighted letters.
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
  $description: Hides the uninstaller shortcuts that installers create ("Uninstall X") and documentation shortcuts (Readme, help files, web links). It looks at what each shortcut really runs, so real programs that have "uninstall" in their name (for example, uninstaller tools) are kept.
- ReindexMode: auto
  $name: App index updates
  $description: When the Start menu app list is re-read. Automatic detects installs and uninstalls as they happen and does not waste resources when there are no changes (recommended).
  $options:
    - auto: Automatic
    - always: Every time it opens
    - min1: 1 minute
    - min3: 3 minutes
    - min5: 5 minutes
    - min10: 10 minutes
- ListMode: expanded
  $name: List mode
  $description: Compact shows only the search bar until you type. Expanded shows your recent apps and suggestions when the search bar is empty.
  $options:
    - compact: Compact
    - expanded: Expanded
- DetailsKey: ctrl+i
  $name: Details panel shortcut
  $description: Key to show or hide the actions and details panel for the selected app. If you pick Ctrl + Space, do not use that same combination as the global shortcut.
  $options:
    - ctrl+i: Ctrl + I
    - ctrl+k: Ctrl + K
    - ctrl+space: Ctrl + Space
    - f4: F4
- DisableInFullscreen: true
  $name: Disable in fullscreen
  $description: When on, the shortcut does nothing while the foreground window is in fullscreen (games, video players, fullscreen browsers). Also closes the search bar if the foreground window goes fullscreen while it is open.
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

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cwctype>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// ===== Design constants (in DIP; they are multiplied by the monitor scale) =====

static const float kMargin = 40.f;     // room for the shadow
static const float kWidth = 700.f;
static const float kBarH = 64.f;
static const float kRowH = 52.f;
static const float kListPad = 8.f;
static const float kEmptyRowH = 56.f;  // message row ("No results")
static const float kRadius = 20.f;
static const float kRowRadius = 12.f;
static const float kIconSize = 32.f;
static const float kFooterH = 40.f;    // footer with shortcuts (expanded only)
static const float kSectionH = 26.f;   // section header ("Recent" / "Suggestions")

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
static const int HOTKEY_ID = 1;

// ===== Types =====

enum class ReindexMode { Auto, Always, Interval };
enum class ListMode { Compact, Expanded };
enum class DetailsKey { CtrlI, CtrlK, CtrlSpace, F4 };
enum class BgEffect { Acrylic, Mica, Glass, Frosted, Aurora, Solid };

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
    int colorIdx = 0;     // index in kColors
    bool hideUtility = true;
    bool disableFullscreen = true;  // ignore the shortcut while a fullscreen window is focused
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
    bool canFile = false;    // there is a real program file (allows location, admin, ...)
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
    PCWSTR s = Wh_GetStringSetting(name);
    std::wstring r = s ? s : L"";
    if (s) {
        Wh_FreeStringSetting(s);
    }
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

    g_cfg = c;
}

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

    // Acronyms: "vsc" -> Visual Studio Code
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

static std::wstring UsageKey(const std::wstring& id) {
    uint64_t h = 1469598103934665603ULL;
    for (wchar_t c : id) {
        h ^= (uint64_t)c;
        h *= 1099511628211ULL;
    }
    wchar_t buf[80];
    swprintf_s(buf, L"u%s_%016llx", CurrentUserSid().c_str(), (unsigned long long)h);
    return buf;
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
        HMODULE m = LoadLibraryW(L"shcore.dll");
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
    HMODULE m = LoadLibraryW(L"dwmapi.dll");
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
        case BgEffect::Mica:    return {70.0, 95, 1.35f, 0.5f, 0.00f, false, 1.0f};
        case BgEffect::Glass:   return {9.0, 150, 0.45f, 0.0f, 0.00f, false, 2.2f};
        case BgEffect::Frosted: return {42.0, 100, 0.90f, 2.4f, 0.10f, false, 1.0f};
        case BgEffect::Aurora:  return {30.0, 135, 0.85f, 0.8f, 0.00f, true, 1.2f};
        default:                return {30.0, 125, 1.00f, 1.0f, 0.00f, false, 1.0f};
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
    if (g_cfg.effect == BgEffect::Solid) {
        ReleaseBackdrop();
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
    if (!g_noiseBrush) {
        // Grain: 128x128 tile of random gray with ~3.5% opacity (premultiplied).
        const int N = 128;
        std::vector<uint32_t> px((size_t)N * N);
        uint32_t s = 0x9E3779B9u;
        for (auto& p : px) {
            s ^= s << 13;
            s ^= s >> 17;
            s ^= s << 5;
            const uint32_t a = 9;
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
    // Available height for rows: expanded card minus the bar, the footer and both paddings.
    const double rowsArea = ExpandedH() - kBarH - kFooterH - 2 * kListPad;
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
    return ExpandedH() - kBarH - kFooterH - 2 * kListPad;
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
    RunSearch();
    g_sel = 0;
    g_scrollTarget = 0;
    UpdateTargets(false, /*followSelection=*/true);
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
            return d.canFile && d.isExe;
        case ACT_LOCATION:
        case ACT_COPYPATH:
        case ACT_PROPS:
            return d.canFile;
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

// Card: shadow, body (effect), top sheen, outer border and an inner edge that gives volume.
static void DrawCard(float x, float y, float w, float h, float alpha) {
    const float s = (float)g_scale;
    {
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
    if (!solid && EnsureBackdropBrushes()) {
        // Layers: blurred background (opaque) -> veil -> color tint -> gradient -> grain.
        g_backBrush->SetOpacity(alpha);
        g_rt->FillRoundedRectangle(&card, g_backBrush);
        if (ep.veil > 0.f) {
            FillRR(x, y, x + w, y + h, kRadius * s, D2D1::ColorF(1.f, 1.f, 1.f, 1.f), ep.veil * alpha);
        }
        D2D1_COLOR_F tint = g_pal.bg;
        tint.a = EffectTintAlpha();
        FillRR(x, y, x + w, y + h, kRadius * s, tint, alpha);
        if (ep.glow) {
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
        if (g_noiseBrush && ep.noiseMul > 0.f) {
            g_noiseBrush->SetOpacity(std::min(1.f, alpha * ep.noiseMul));
            g_rt->FillRoundedRectangle(&card, g_noiseBrush);
        }
    } else {
        // Solid (opaque), or no capture available (normal translucent color).
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

    DrawCard(ox, oy, pw, ph, a);

    D2D1_RECT_F clip = D2D1::RectF(ox, oy, ox + pw, oy + ph);
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

    // The footer takes the end of the expanded height; the list ends just above it.
    const float footerTop = cy + (float)(ExpandedH() - kFooterH) * s;
    const float listBottom = std::min(cy + ch, footerTop - kListPad * s);
    D2D1_RECT_F listClip = D2D1::RectF(cx, listTop + 1.f, cx + cw, listBottom);
    g_rt->PushAxisAlignedClip(&listClip, D2D1_ANTIALIAS_MODE_ALIASED);

    if (rows > 0) {
        const float rowsOrigin = listTop + kListPad * s;
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
    } else if (ch > kBarH * s + 8) {
        const wchar_t* msg = nullptr;
        if (!g_tokens.empty()) {
            msg = kTxtNoResults;
        } else if (g_apps.empty() && !g_indexedOnce) {
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
    ShowWindow(g_hwnd, SW_HIDE);
    g_vis = Vis::Hidden;
    g_lastHide = NowSec();
    ReleaseBackdrop();
    ReleaseResultLayouts();
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
    if (!EnsureTarget()) {
        return;
    }
    g_matchBrush->SetColor(&g_pal.match);

    g_text.clear();
    g_caret = g_anchor = 0;
    g_inputScroll = 0;
    g_caretOn = true;
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

    if (!file.empty()) {
        // A Windows-directory target needs WOW64 redirection off, or a 64-bit-only tool
        // (mdsched.exe, osk.exe, ...) is resolved to SysWOW64 and not found. The path must stay
        // the real System32 one: the 64-bit shell does not understand the Sysnative alias.
        PVOID old = nullptr;
        const BOOL off = IsUnderWindowsDir(file) ? Wow64DisableWow64FsRedirection(&old) : FALSE;
        // Start the app in its own folder, not in Windhawk's directory (portable apps and some
        // games read files relative to the working directory).
        std::wstring dir = file.substr(0, file.find_last_of(L'\\'));
        ShellExecuteW(nullptr, L"open", file.c_str(), args.empty() ? nullptr : args.c_str(),
                      dir.empty() ? nullptr : dir.c_str(), SW_SHOWNORMAL);
        if (off) Wow64RevertWow64FsRedirection(old);
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
    if (!s.empty()) ReplaceSelection(s);
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
            LaunchSelected();
            return true;
        case VK_DOWN:
            SelectDelta(1);
            return true;
        case VK_UP:
            SelectDelta(-1);
            return true;
        case VK_TAB:
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
            } else if (g_caret > 0) {
                g_anchor = ctrl ? PrevWord(g_caret) : g_caret - 1;
                ReplaceSelection(L"");
            }
            return true;
        case VK_DELETE:
            if (hasSel) {
                ReplaceSelection(L"");
            } else if (g_caret < len) {
                g_anchor = ctrl ? NextWord(g_caret) : g_caret + 1;
                ReplaceSelection(L"");
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

    const float rowsOrigin = cy + (kBarH + kListPad) * s;
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
                L"application is already using it (for example PowerToys Run with Alt+Space).\n\n"
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
    LoadPalette();

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
