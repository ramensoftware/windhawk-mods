// ==WindhawkMod==
// @id              custom-start-menu-apps
// @name            Custom Start Menu Apps
// @description     Adds a customizable app panel beside the Windows 11 Start menu, with freely positioned shortcuts, automatic sizing and full support for Windows 11 Start Menu Styler
// @version         0.7.1
// @author          Nightmarehelltime & RTantrumR
// @github          Nightmarehelltime
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -lshell32 -lgdi32 -luser32 -ldwmapi -lmsimg32 -ladvapi32 -luuid -loleaut32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Custom Start Menu Apps 0.7.1

A customizable app panel that appears beside the Windows 11 Start menu and
works as an extension of the Start experience without modifying the Start menu
XAML. The panel runs from explorer.exe, follows the Start menu position and
size, and provides a convenient place for frequently used applications and
shortcuts.

The mod is designed to work seamlessly with **Windows 11 Start Menu Styler**.
It automatically follows the Start menu's visual style, including sampled
colours, transparency and the current Start panel appearance, so the side
panel can visually match Start instead of looking like a separate window.

## Smooth animation
The fade and slide run on their own thread, in step with the screen refresh
(DwmFlush), and only change the window's position and opacity. They do not
depend on how busy Explorer is. The card also reacts at once when Start opens
or loses focus, instead of waiting for the next timer tick.

## Free icon placement
Drag a tile anywhere inside the card and drop it: it stays exactly there.
Tiles never snap to a grid. The card scrolls vertically if tiles are placed
below the visible area. New programs are put in the first free spot.
Right click an empty part of the card, or a tile, and choose
"Вирівняти за сіткою" to arrange everything into a tidy grid again.

## Automatic size ("Start Menu Size" compatible)
Windows gives the Start window a window region that matches the visible Start
panel. The card is sized and placed from that region, so a panel resized by
"Start Menu Size" is followed. The last region is remembered between openings
and restarts, so the card appears in the right place at once. On the very first
opening the card waits up to 0.9 s for the region instead of jumping.
If no region is available, the sizes from the settings are used.

## Automatic style ("Windows 11 Start Menu Styler" compatible)
The colour of the Start panel is sampled from the screen along its edge, row by
row, and the card is painted with the same vertical colour gradient, so the
card continues Start without a seam. Anything Styler does to Start's colour or
transparency is included. The last profile is remembered too.

## Compatibility with Start Menu Styler
The panel fully supports **Windows 11 Start Menu Styler**. When Styler changes
the Start menu's colour, transparency or visual appearance, this mod reads the
resulting Start surface and adapts the side panel to match it. The panel keeps
its own layout and app shortcuts while following the visual changes made to
Start.

## Credits
Created by **Nightmarehelltime**, with help from **RTantrumR** in completing
and polishing the mod.

## Which settings do what
* Always applied: side, width, gap, shrink top/bottom, icon size, columns (used
  when arranging), label size, title, corner radius, theme, animation.
* **Opacity** is used only when the Start colour is not matched automatically
  (or cannot be read). While matching works, the card is as opaque as Start's
  own sampled colour; use "Extra transparency" to make it more see-through.
* **Theme** Dark/Light forces the text colour.
* Start panel position / width / height / gap to taskbar are only used when the
  Start region cannot be read.

## Adding programs
* Click "+" in the card header: the card shows the list of all installed programs
  (the same list as "All apps" in Start). Click a program to add it, click it
  again to remove it. The check mark means "already on the card". Press the check
  mark in the header to go back. Start stays open all the time.
* Drag a program from the Start menu (All apps or Pinned) onto the card.
* The folder button chooses files (.lnk, .exe, .url). The file dialog takes
  focus, so Start closes; the programs are added anyway.
* Files can also be dragged from Explorer.

## Using the card
* Click a tile: launches it. Mouse wheel: scroll.
* Drag a tile to move it anywhere.
* Hover a tile and click the x in its corner to remove it. The real program is
  never deleted.
* Right click a tile: open, open location, run as administrator, arrange, remove.

## If something is off
Turn on "Diagnostic log" and open Start. Lines beginning with [region], [start],
[win], [card], [style], [drop], [icon] show what the mod found.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- enabled: true
  $name: Enable the card
- language: auto
  $name: Language
  $description: Language of the card's own texts, menus and file dialog.
  $options:
  - auto: Same as Windows
  - en: English
  - uk: Українська
- tiles:
  - layout: "4"
    $name: Tile placement
    $description: "On a grid, tiles sit in cells: drop one on a taken cell and that tile moves aside. Free: a tile stays wherever you drop it. The card widens if the columns need more room (icon size + 28 px each, at least 72)."
    $options:
    - free: Free placement
    - "1": Grid, 1 column
    - "2": Grid, 2 columns
    - "3": Grid, 3 columns
    - "4": Grid, 4 columns
    - "5": Grid, 5 columns
    - "6": Grid, 6 columns
    - "7": Grid, 7 columns
    - "8": Grid, 8 columns
  - iconSize: 36
    $name: Icon size (pixels)
    $description: 16 to 64.
  - labelSize: 12
    $name: Label font size
    $description: 10 to 18.
  $name: Tiles
- card:
  - side: left
    $name: Side of the Start menu
    $options:
    - left: Left
    - right: Right
  - width: 320
    $name: Width (pixels at 100% scale)
    $description: 200 to 600. On a grid, the card is at least as wide as its columns need.
  - gap: 8
    $name: Gap to the Start panel (pixels)
    $description: 0 to 64.
  - insetTop: 0
    $name: Shrink from the top (pixels)
    $description: 0 to 300.
  - insetBottom: 0
    $name: Shrink from the bottom (pixels)
    $description: 0 to 300.
  $name: Card position and size
- look:
  - background: match
    $name: Background
    $description: The text colour always follows the background.
    $options:
    - match: Match the Start panel (works with Windows 11 Start Menu Styler)
    - system: Follow the Windows theme
    - dark: Dark
    - light: Light
  - transparency: 0
    $name: Transparency (percent)
    $description: 0 to 80. 0 = as solid as the Start panel.
  - cornerRadius: 8
    $name: Corner radius (pixels)
    $description: 0 to 24.
  - title: ""
    $name: Title
    $description: Leave empty for the default title in the card's language, or type - to show no title.
  - animate: true
    $name: Rise and sink with Start
  $name: Appearance
- start:
  - autoDetect: true
    $name: Detect the Start panel automatically
    $description: Measures the visible Start panel each time Start opens (works with Start Menu Size). Off, or when it fails, the sizes in "When the panel is not detected" are used.
  - tune:
    - left: 0
      $name: Left edge
    - top: 0
      $name: Top edge
    - right: 0
      $name: Right edge
    - bottom: 0
      $name: Bottom edge
    $name: Fine-tune the detected panel
    $description: Pixels, -100 to 100. Moves the detected edges (the card follows). Only used while detection works.
  - fallback:
    - align: auto
      $name: Panel position
      $options:
      - auto: Follow taskbar alignment
      - center: Center
      - left: Left
    - width: 660
      $name: Panel width
      $description: Pixels at 100% scale, 300 to 1400.
    - height: 726
      $name: Panel height
      $description: Pixels at 100% scale, 300 to 1200.
    - bottomGap: 12
      $name: Gap between panel and taskbar
      $description: 0 to 48.
    $name: When the panel is not detected
  $name: Start panel
- advanced:
  - startClass: Windows.UI.Core.CoreWindow
    $name: Start window class
    $description: Class name of the Start menu window to dock to.
  - diagnostics: false
    $name: Diagnostic log
    $description: Logs the Start window, the detected region, drag and drop data and the card state.
  $name: Advanced
*/
// ==/WindhawkModSettings==

#ifndef NOMINMAX
#define NOMINMAX
#endif
#undef GetCurrentTime

#include <windows.h>
#include <windowsx.h>
#include <shellapi.h>
#include <shlobj.h>
#include <shobjidl.h>
#include <dwmapi.h>

#include <algorithm>
#include <atomic>
#include <climits>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <cwchar>
#include <cwctype>
#include <map>
#include <memory>
#include <mutex>
#include <set>
#include <string>
#include <utility>
#include <vector>

#ifndef EVENT_OBJECT_CLOAKED
#define EVENT_OBJECT_CLOAKED 0x8017
#endif
#ifndef EVENT_OBJECT_UNCLOAKED
#define EVENT_OBJECT_UNCLOAKED 0x8018
#endif

////////////////////////////////////////////////////////////////////////////////
// Texts. Each language is one block in kLocales, keyed by its ISO 639-1 code;
// "auto" picks the block that matches the Windows display language. To add a
// language: add a block here and an option to the "language" setting. A missing
// or empty string falls back to English (the first block).
////////////////////////////////////////////////////////////////////////////////
enum TextId {
    T_DefaultTitle,
    T_PickerTitle,
    T_AppsEmpty,
    T_AppsLoading,
    T_CardEmpty,
    T_DialogTitle,
    T_FilterPrograms,
    T_FilterAll,
    T_MenuOpen,
    T_MenuLocate,
    T_MenuRunAs,
    T_MenuArrange,
    T_MenuRemove,
    T_MenuAdd,
    T_SearchHint,
    T_NoMatches,
    T_Count
};

struct Locale {
    PCWSTR code;
    PCWSTR text[T_Count];
};

static const Locale kLocales[] = {
    {L"en",
     {
         L"Custom apps",
         L"Add programs",
         L"The program list is empty.",
         L"Loading the program list…",
         L"Press + to pick programs from the list, or drag them here from the Start menu.",
         L"Add programs to the card",
         L"Programs and shortcuts",
         L"All files",
         L"Open",
         L"Open file location",
         L"Run as administrator",
         L"Arrange in grid",
         L"Remove from card",
         L"Add programs…",
         L"Type to search",
         L"Nothing found.",
     }},
    {L"uk",
     {
         L"Власні програми",
         L"Додати програми",
         L"Список програм порожній.",
         L"Завантажую список програм…",
         L"Натисніть +, щоб вибрати програми зі списку, або перетягніть їх сюди з меню Пуск.",
         L"Додати програми до картки",
         L"Програми та ярлики",
         L"Усі файли",
         L"Відкрити",
         L"Показати розташування файлу",
         L"Запустити від імені адміністратора",
         L"Вирівняти за сіткою",
         L"Прибрати з картки",
         L"Додати програми…",
         L"Пишіть, щоб шукати",
         L"Нічого не знайдено.",
     }},
};

static Locale const* g_loc = &kLocales[0];  // set once in LoadConfig, read-only afterwards (also on worker threads)

static PCWSTR Tr(TextId id) {
    PCWSTR t = g_loc->text[id];
    return t && *t ? t : kLocales[0].text[id];
}

// "auto": the language of the Windows display ("uk-UA" -> "uk"); unknown languages get English.
static void SelectLocale(std::wstring code) {
    if (code.empty() || code == L"auto") {
        WCHAR name[LOCALE_NAME_MAX_LENGTH] = L"";
        if (LCIDToLocaleName(MAKELCID(GetUserDefaultUILanguage(), SORT_DEFAULT), name, ARRAYSIZE(name), 0)) code = name;
        code = code.substr(0, code.find(L'-'));
    }
    g_loc = &kLocales[0];
    for (auto const& l : kLocales)
        if (_wcsicmp(l.code, code.c_str()) == 0) g_loc = &l;
}

////////////////////////////////////////////////////////////////////////////////
// Settings
////////////////////////////////////////////////////////////////////////////////
struct Cfg {
    bool enabled = true;
    bool left = true;
    int width = 320, gap = 8, insetTop = 0, insetBottom = 0;
    bool autoDetect = true, autoStyle = true, animate = true;
    int styleTransp = 0;
    int tuneL = 0, tuneT = 0, tuneR = 0, tuneB = 0;
    int align = 0;  // 0 follow taskbar, 1 center, 2 left
    int startW = 660, startH = 726, startBottom = 12;
    int theme = 0;  // 0 auto, 1 dark, 2 light
    int opacity = 95, radius = 8, iconSize = 36, columns = 4, labelSize = 12;
    bool grid = true;  // tiles sit in cells, in list order; off = free placement
    bool showTitle = true;
    std::wstring title;  // the setting, or the default title in the card's language
    std::wstring startClass = L"Windows.UI.Core.CoreWindow";
    bool diag = false;
};
static Cfg g_cfg;

#define LOGE(...) Wh_Log(__VA_ARGS__)
#define LOGI(...)                            \
    do {                                     \
        if (g_cfg.diag) Wh_Log(__VA_ARGS__); \
    } while (0)

static std::wstring ReadString(PCWSTR name) {
    PCWSTR v = Wh_GetStringSetting(name);
    std::wstring r = v ? v : L"";
    Wh_FreeStringSetting(v);
    return r;
}

// A value outside the allowed range is moved to the nearest limit (width 700 becomes 600), which
// is closer to what was asked for than falling back to the default.
static int ReadRanged(PCWSTR name, int lo, int hi) {
    int v = Wh_GetIntSetting(name);
    int c = std::max(lo, std::min(v, hi));
    if (c != v) LOGE(L"[config] %s=%d is outside %d..%d, using %d", name, v, lo, hi, c);
    return c;
}

static void LoadConfig() {
    Cfg c;
    c.enabled = Wh_GetIntSetting(L"enabled") != 0;
    SelectLocale(ReadString(L"language"));
    c.left = ReadString(L"card.side") != L"right";
    c.width = ReadRanged(L"card.width", 200, 600);
    c.gap = ReadRanged(L"card.gap", 0, 64);
    c.insetTop = ReadRanged(L"card.insetTop", 0, 300);
    c.insetBottom = ReadRanged(L"card.insetBottom", 0, 300);
    std::wstring layout = ReadString(L"tiles.layout");  // "free" or a number of grid columns
    int cols = _wtoi(layout.c_str());
    c.grid = layout != L"free";
    c.columns = cols >= 1 && cols <= 8 ? cols : 4;
    c.iconSize = ReadRanged(L"tiles.iconSize", 16, 64);
    c.labelSize = ReadRanged(L"tiles.labelSize", 10, 18);
    // One choice decides both the background and the text colour, so they cannot contradict.
    std::wstring bg = ReadString(L"look.background");
    c.autoStyle = bg.empty() || bg == L"match";
    c.theme = bg == L"dark" ? 1 : bg == L"light" ? 2 : 0;
    c.styleTransp = ReadRanged(L"look.transparency", 0, 80);
    c.opacity = 100 - c.styleTransp;
    c.radius = ReadRanged(L"look.cornerRadius", 0, 24);
    c.title = ReadString(L"look.title");
    c.showTitle = c.title != L"-";
    if (c.title.empty() || !c.showTitle) c.title = Tr(T_DefaultTitle);
    if (c.title.size() > 64) c.title.resize(64);
    c.animate = Wh_GetIntSetting(L"look.animate") != 0;
    c.autoDetect = Wh_GetIntSetting(L"start.autoDetect") != 0;
    c.tuneL = ReadRanged(L"start.tune.left", -100, 100);
    c.tuneT = ReadRanged(L"start.tune.top", -100, 100);
    c.tuneR = ReadRanged(L"start.tune.right", -100, 100);
    c.tuneB = ReadRanged(L"start.tune.bottom", -100, 100);
    std::wstring al = ReadString(L"start.fallback.align");
    c.align = al == L"center" ? 1 : al == L"left" ? 2 : 0;
    c.startW = ReadRanged(L"start.fallback.width", 300, 1400);
    c.startH = ReadRanged(L"start.fallback.height", 300, 1200);
    c.startBottom = ReadRanged(L"start.fallback.bottomGap", 0, 48);
    c.startClass = ReadString(L"advanced.startClass");
    if (c.startClass.empty()) c.startClass = L"Windows.UI.Core.CoreWindow";
    c.diag = Wh_GetIntSetting(L"advanced.diagnostics") != 0;
    g_cfg = c;
}

////////////////////////////////////////////////////////////////////////////////
// Global state
////////////////////////////////////////////////////////////////////////////////
constexpr UINT WM_APP_ICON = WM_APP + 1;
constexpr UINT WM_APP_QUIT = WM_APP + 2;
constexpr UINT WM_APP_ADD = WM_APP + 3;
constexpr UINT WM_APP_APPS = WM_APP + 4;
constexpr UINT WM_APP_ANIMDONE = WM_APP + 5;
constexpr UINT WM_APP_KEY = WM_APP + 6;  // a key typed while the picker is open (wParam: character)
constexpr UINT WM_APP_DESKCLICK = WM_APP + 7;  // a plain click on the desktop while the picker is open
constexpr UINT_PTR kTimerTick = 1;
constexpr UINT_PTR kTimerScroll = 2;
constexpr UINT_PTR kTimerIcon = 4;
constexpr UINT_PTR kTimerFrame = 5;  // at most one repaint per frame while dragging or while tiles slide
constexpr size_t kMaxItems = 48;
// Start's own motion, measured on Windows 11 26200 at 60 Hz: the panel rises from behind the
// taskbar in ~260 ms (quintic ease-out) from the moment its window is uncloaked, and sinks back
// in ~186 ms (exponential ease-in, 2^(6(u-1))) starting ~6 ms before the focus leaves it. The
// card uses the same curves over the same distance, so the two move as one piece. The rise is
// timed from Start's uncloak (over 6 openings the card ran 0..8 ms off Start, which itself starts
// up to a frame late); the sink starts when the focus leaves Start, so it needs a lead.
constexpr double kShowMs = 260.0;
constexpr double kHideMs = 186.0;
constexpr double kShowPow = 5.0;
constexpr double kHideExpo = 6.0;
constexpr double kHideLeadMs = 10.0;  // Start's 6 ms head start + ~3 ms presentation + ~1 ms focus event

static double HideEase(double t) {  // 0..1 -> 0..1, exponential ease-in
    const double floor = std::pow(2.0, -kHideExpo);
    return (std::pow(2.0, kHideExpo * (t - 1.0)) - floor) / (1.0 - floor);
}

static double HideEaseInverse(double e) {
    const double floor = std::pow(2.0, -kHideExpo);
    return 1.0 + std::log2(e * (1.0 - floor) + floor) / kHideExpo;
}
constexpr ULONGLONG kFirstWaitMs = 900;
constexpr UINT kTickFastMs = 16;   // while Start is open or the card is on screen
constexpr UINT kTickIdleMs = 250;  // otherwise: the WinEvent hooks wake Tick, this is only a safety net
constexpr int kProf = 16;  // vertical colour profile of the Start edge
constexpr size_t kIconCacheMax = 200;  // above this, icons that are neither tiles nor on screen are freed
// Start's window region is larger than the visible panel by a transparent margin on every side:
// 12 px at 100 % scale on Windows 11 26200 (region (0,142,858,1032), panel border at x 12..845,
// y 154..1019, measured on screen). Sampling or docking to the region itself hits the wallpaper.
constexpr int kRegionMargin = 12;

struct Item {
    std::wstring name, path;
    int lx = -1, ly = -1;  // free position inside the card content, in 96-dpi pixels (-1 = not placed yet)
    int gc = -1, gr = -1;  // grid cell: column, row (-1 = none yet)
};

static std::atomic<bool> g_quit{true};
static std::atomic<bool> g_started{false};
static std::atomic<int> g_jobs{0};
static std::atomic<bool> g_pickerBusy{false};
static std::atomic<bool> g_appsLoading{false};
static HWND g_hwnd = nullptr;
static HANDLE g_thread = nullptr;
static HANDLE g_animThread = nullptr;
static HANDLE g_animEvt = nullptr;

// window-thread-only state
static std::vector<Item> g_items;
static int g_dpi = 96;
static int g_x = 0, g_y = 0, g_w = 0, g_h = 0;
static bool g_shown = false;
static int g_hover = -1;  // -1 none, -2 plus, -3 file button, -4 "done" (picker), >=0 tile or row
static bool g_hoverRemove = false;
static int g_down = -1;  // what the left button went down on (same codes as g_hover)
static bool g_pressRemove = false;
static POINT g_downPt{};
static POINT g_grab{};  // where inside the tile the drag started
static bool g_dragging = false;
static int g_dragFrom = -1;
static POINT g_dragPt{};
static POINT g_ghost{};  // where the dragged tile would land (content coordinates), found once per Repaint
static int g_scroll = 0;
static bool g_menuOpen = false;
static bool g_trackingLeave = false;
static bool g_iconTimer = false;
static UINT g_tickMs = 0;  // current period of kTimerTick
static int g_showLogs = 0;
static int g_dumpLogs = 0;

// picker (list of installed programs shown inside the card)
static int g_mode = 0;  // 0 tiles, 1 picker
static std::vector<Item> g_apps;
static bool g_appsLoaded = false;
static int g_pscroll = 0;
static std::wstring g_query;    // typed search text
static std::vector<int> g_view;  // the picker's rows: indices into g_apps, filtered and ranked by g_query
static HHOOK g_keyHook = nullptr;

// open / close sequence
static bool g_hiding = false;
static bool g_suppress = false;  // closed early (Start lost focus); wait for Start to really go away
static bool g_fgStart = false;   // Start has been the foreground window during this opening
static ULONGLONG g_openT0 = 0;   // when this Start opening was first seen (0 = Start closed)
static LONGLONG g_openQpc = 0;   // the same moment as a QueryPerformanceCounter value
static int g_slide = 0;          // distance from the card's place down to behind the taskbar
static int g_stylePass = 0;
static bool g_stateLogged = false;
static bool g_regionLogged = false;

// animation, shared with the animation thread
static std::atomic<int> g_animKind{0};  // 0 none, 1 show, 2 hide, 3 hold at alpha 0 until the first frame is ready
static std::atomic<LONGLONG> g_animQpc0{0};
static std::atomic<int> g_animGen{0};
static std::atomic<int> g_apx{0}, g_apy{0}, g_aslide{0};
static LONGLONG g_qpcFreq = 1;

// remembered Start region (relative to the Start window) and colour profile
struct Mem {
    bool valid = false;
    RECT rel{};
    LONG winW = 0, winH = 0;
};
static Mem g_mem;
struct Prof {
    bool valid = false;
    uint32_t c[kProf] = {};  // 0x00RRGGBB, top to bottom
};
static Prof g_prof;

static HWINEVENTHOOK g_hookFg = nullptr, g_hookCloak = nullptr;

static int S(int v) { return MulDiv(v, g_dpi, 96); }

static void PinModule(PCWSTR why) {
    HMODULE m = nullptr;
    if (GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_PIN,
                           reinterpret_cast<PCWSTR>(&PinModule), &m)) {
        LOGE(L"[uninit] module pinned in memory because: %s", why);
    }
}

static bool IsShellPath(std::wstring const& p) { return _wcsnicmp(p.c_str(), L"shell:", 6) == 0; }

static bool LooksLikeFsPath(std::wstring const& p) {
    if (p.size() >= 3 && p[1] == L':' && (p[2] == L'\\' || p[2] == L'/')) return true;
    return p.size() >= 2 && p[0] == L'\\' && p[1] == L'\\';
}

static std::wstring FileStem(std::wstring const& path) {
    size_t slash = path.find_last_of(L"\\/");
    std::wstring f = slash == std::wstring::npos ? path : path.substr(slash + 1);
    size_t dot = f.find_last_of(L'.');
    return dot == std::wstring::npos || dot == 0 ? f : f.substr(0, dot);
}

static std::wstring Clean(std::wstring s) {
    for (auto& c : s)
        if (c == L'\t' || c == L'\n' || c == L'\r') c = L' ';
    return s;
}

static std::wstring StripLnk(std::wstring s) {
    if (s.size() > 4 && _wcsicmp(s.c_str() + s.size() - 4, L".lnk") == 0) s.resize(s.size() - 4);
    return s;
}

// "::{4234d49b-...}\Pkg!App" (virtual Apps folder) -> "shell:AppsFolder\Pkg!App"
static std::wstring NormalizeShellPath(std::wstring const& p) {
    static const wchar_t kGuid[] = L"{4234d49b-0245-4df3-b780-3893943456e1}\\";
    std::wstring low = p;
    for (auto& c : low) c = (wchar_t)std::towlower(c);
    size_t pos = low.find(kGuid);
    if (pos == std::wstring::npos) return p;
    return L"shell:AppsFolder\\" + p.substr(pos + wcslen(kGuid));
}

static LONGLONG NowQpc() {
    LARGE_INTEGER q;
    QueryPerformanceCounter(&q);
    return q.QuadPart;
}

////////////////////////////////////////////////////////////////////////////////
// Persistence: items ("name<TAB>path<TAB>x<TAB>y<TAB>column<TAB>row" per line) and the remembered region/colours
////////////////////////////////////////////////////////////////////////////////
static void SaveItems() {
    std::wstring s;
    for (auto const& it : g_items) {
        s += it.name;
        s += L'\t';
        s += it.path;
        s += L'\t';
        s += std::to_wstring(it.lx);
        s += L'\t';
        s += std::to_wstring(it.ly);
        s += L'\t';
        s += std::to_wstring(it.gc);
        s += L'\t';
        s += std::to_wstring(it.gr);
        s += L'\n';
    }
    if (!Wh_SetStringValue(L"items2", s.c_str())) LOGE(L"[store] Wh_SetStringValue failed");
}

static void LoadItems() {
    static WCHAR buf[65536];
    buf[0] = 0;
    Wh_GetStringValue(L"items2", buf, ARRAYSIZE(buf));
    buf[ARRAYSIZE(buf) - 1] = 0;
    g_items.clear();
    std::wstring all = buf;
    size_t pos = 0;
    while (pos < all.size() && g_items.size() < kMaxItems) {
        size_t nl = all.find(L'\n', pos);
        std::wstring line = all.substr(pos, nl == std::wstring::npos ? std::wstring::npos : nl - pos);
        pos = nl == std::wstring::npos ? all.size() : nl + 1;
        size_t t1 = line.find(L'\t');
        if (t1 == std::wstring::npos) continue;
        size_t t2 = line.find(L'\t', t1 + 1);
        Item it;
        it.name = line.substr(0, t1);
        if (t2 == std::wstring::npos) {
            it.path = line.substr(t1 + 1);
        } else {
            it.path = line.substr(t1 + 1, t2 - t1 - 1);
            size_t t3 = line.find(L'\t', t2 + 1);
            if (t3 != std::wstring::npos) {
                it.lx = _wtoi(line.substr(t2 + 1, t3 - t2 - 1).c_str());
                it.ly = _wtoi(line.substr(t3 + 1).c_str());
                if (it.lx < -1 || it.lx > 4000 || it.ly < -1 || it.ly > 20000) it.lx = it.ly = -1;
                size_t t4 = line.find(L'\t', t3 + 1);
                size_t t5 = t4 == std::wstring::npos ? t4 : line.find(L'\t', t4 + 1);
                if (t5 != std::wstring::npos) {
                    it.gc = _wtoi(line.substr(t4 + 1, t5 - t4 - 1).c_str());
                    it.gr = _wtoi(line.substr(t5 + 1).c_str());
                    if (it.gc < 0 || it.gc > 7 || it.gr < 0 || it.gr > 999) it.gc = it.gr = -1;
                }
            }
        }
        if (it.path.empty()) continue;
        if (it.name.empty()) it.name = FileStem(it.path);
        g_items.push_back(std::move(it));
    }
    LOGI(L"[store] loaded %zu item(s)", g_items.size());
}

static void SaveGeom() {
    WCHAR b[512];
    WCHAR* p = b;
    p += wsprintfW(p, L"2 %ld %ld %ld %ld %ld %ld %d %d", g_mem.winW, g_mem.winH, g_mem.rel.left, g_mem.rel.top,
                   g_mem.rel.right, g_mem.rel.bottom, g_mem.valid ? 1 : 0, g_prof.valid ? 1 : 0);
    for (int i = 0; i < kProf; i++) p += wsprintfW(p, L" %lu", (unsigned long)g_prof.c[i]);
    Wh_SetStringValue(L"geom", b);
}

static void LoadGeom() {
    WCHAR b[512];
    b[0] = 0;
    Wh_GetStringValue(L"geom", b, ARRAYSIZE(b));
    b[ARRAYSIZE(b) - 1] = 0;
    PWSTR s = b, e = nullptr;
    long v[9];
    for (int i = 0; i < 9; i++) {
        v[i] = wcstol(s, &e, 10);
        if (e == s) return;
        s = e;
    }
    if (v[0] != 2) return;
    Mem m;  // fields: ver winW winH l t r b memFlag profFlag
    m.winW = v[1];
    m.winH = v[2];
    m.rel = RECT{v[3], v[4], v[5], v[6]};
    m.valid = v[7] != 0 && m.rel.right - m.rel.left >= 300 && m.rel.bottom - m.rel.top >= 300;
    g_mem = m;
    if (v[8]) {
        Prof pr;
        bool ok = true;
        for (int i = 0; i < kProf; i++) {
            unsigned long c = wcstoul(s, &e, 10);
            if (e == s) {
                ok = false;
                break;
            }
            s = e;
            pr.c[i] = (uint32_t)(c & 0x00FFFFFFu);
        }
        if (ok) {
            pr.valid = true;
            g_prof = pr;
        }
    }
    LOGI(L"[store] remembered region=%d, colour profile=%d", g_mem.valid ? 1 : 0, g_prof.valid ? 1 : 0);
}

////////////////////////////////////////////////////////////////////////////////
// Icons: extracted on worker threads into premultiplied DIB sections
////////////////////////////////////////////////////////////////////////////////
struct IconEntry {
    HBITMAP bmp = nullptr;
    int w = 0, h = 0;
    bool done = false;
    int tries = 0;
    ULONGLONG retryAt = 0;
};
static std::mutex g_iconMutex;
static std::map<std::wstring, IconEntry> g_icons;

static void FillBitmapInfo(BITMAPINFO& bi, int w, int h) {
    ZeroMemory(&bi, sizeof(bi));
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = w;
    bi.bmiHeader.biHeight = -h;  // top-down
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
}

static HBITMAP PremultCopy(HBITMAP hb, int& w, int& h) {
    BITMAP bm{};
    if (!GetObjectW(hb, sizeof(bm), &bm) || bm.bmWidth <= 0 || bm.bmHeight <= 0 || bm.bmWidth > 512 ||
        bm.bmHeight > 512)
        return nullptr;
    w = bm.bmWidth;
    h = bm.bmHeight;
    BITMAPINFO bi;
    FillBitmapInfo(bi, w, h);
    std::vector<uint32_t> buf((size_t)w * h);
    HDC dc = GetDC(nullptr);
    int lines = GetDIBits(dc, hb, 0, h, buf.data(), &bi, DIB_RGB_COLORS);
    ReleaseDC(nullptr, dc);
    if (lines != h) return nullptr;
    bool anyA = false, straight = false;
    for (uint32_t v : buf) {
        uint32_t a = v >> 24, r = (v >> 16) & 255, g = (v >> 8) & 255, b = v & 255;
        if (a) anyA = true;
        if (r > a || g > a || b > a) straight = true;
    }
    if (!anyA) {
        for (auto& v : buf) v |= 0xFF000000u;  // no alpha channel: opaque
    } else if (straight) {
        for (auto& v : buf) {  // straight alpha -> premultiplied
            uint32_t a = v >> 24, r = (v >> 16) & 255, g = (v >> 8) & 255, b = v & 255;
            r = r * a / 255;
            g = g * a / 255;
            b = b * a / 255;
            v = (a << 24) | (r << 16) | (g << 8) | b;
        }
    }
    void* bits = nullptr;
    BITMAPINFO bi2;
    FillBitmapInfo(bi2, w, h);
    HBITMAP dib = CreateDIBSection(nullptr, &bi2, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!dib || !bits) return nullptr;
    memcpy(bits, buf.data(), buf.size() * 4);
    return dib;
}

struct IconJob {
    std::wstring key, path;
    int px;
};

// Several attempts, because items dragged from Start are virtual "shell:AppsFolder\..."
// entries and do not always answer to the same request as ordinary files.
static HBITMAP LoadShellImage(std::wstring const& path, int px, int& ow, int& oh, HRESULT& hrOut) {
    HBITMAP out = nullptr;
    IShellItem* si = nullptr;
    HRESULT hr = SHCreateItemFromParsingName(path.c_str(), nullptr, IID_PPV_ARGS(&si));
    if (FAILED(hr) || !si) {
        si = nullptr;
        PIDLIST_ABSOLUTE pidl = nullptr;
        if (SUCCEEDED(SHParseDisplayName(path.c_str(), nullptr, &pidl, 0, nullptr)) && pidl) {
            hr = SHCreateItemFromIDList(pidl, IID_PPV_ARGS(&si));
            CoTaskMemFree(pidl);
        }
    }
    if (FAILED(hr) || !si) {
        hrOut = FAILED(hr) ? hr : E_FAIL;
        return nullptr;
    }
    IShellItemImageFactory* f = nullptr;
    hr = si->QueryInterface(IID_PPV_ARGS(&f));
    if (SUCCEEDED(hr) && f) {
        const bool shell = IsShellPath(path);
        const DWORD kFs[3] = {SIIGBF_BIGGERSIZEOK | SIIGBF_ICONONLY, SIIGBF_BIGGERSIZEOK, 0};
        const DWORD kShell[3] = {SIIGBF_BIGGERSIZEOK, SIIGBF_BIGGERSIZEOK | SIIGBF_ICONONLY, 0};
        for (int k = 0; k < 3 && !out; k++) {
            SIZE sz{px, px};
            HBITMAP hb = nullptr;
            hr = f->GetImage(sz, shell ? kShell[k] : kFs[k], &hb);
            if (SUCCEEDED(hr) && hb) {
                out = PremultCopy(hb, ow, oh);
                DeleteObject(hb);
            }
        }
        f->Release();
    }
    si->Release();
    hrOut = hr;
    return out;
}

static DWORD WINAPI IconProc(LPVOID p) {
    std::unique_ptr<IconJob> j(static_cast<IconJob*>(p));
    HRESULT co = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    HBITMAP out = nullptr;
    int ow = 0, oh = 0;
    if (!g_quit) {
        HRESULT hr = S_OK;
        out = LoadShellImage(j->path, j->px, ow, oh, hr);
        if (!out) LOGI(L"[icon] no image for '%s' (hr=0x%08X)", j->path.c_str(), (unsigned)hr);
    }
    {
        std::lock_guard<std::mutex> l(g_iconMutex);
        IconEntry& e = g_icons[j->key];
        e.bmp = out;
        e.w = ow;
        e.h = oh;
        e.done = true;
        if (!out) {
            e.tries++;
            e.retryAt = GetTickCount64() + 2500;  // ask again a little later
        }
    }
    if (SUCCEEDED(co)) CoUninitialize();
    HWND h = g_hwnd;
    if (h && !g_quit) PostMessageW(h, WM_APP_ICON, 0, 0);
    j.reset();
    g_jobs.fetch_sub(1);
    return 0;
}

static std::wstring IconKey(std::wstring const& path, int px) { return path + L"|" + std::to_wstring(px); }

static IconEntry IconLookup(std::wstring const& path, int px) {
    std::wstring key = IconKey(path, px);
    {
        std::lock_guard<std::mutex> l(g_iconMutex);
        auto it = g_icons.find(key);
        if (it != g_icons.end()) {
            IconEntry& e = it->second;
            bool retry = e.done && !e.bmp && e.tries < 4 && GetTickCount64() >= e.retryAt;
            if (!retry) return e;
            e.done = false;  // failed earlier: try again
        } else {
            g_icons[key] = IconEntry{};  // pending
        }
    }
    if (!g_quit && g_jobs.load() < 32) {
        g_jobs.fetch_add(1);
        auto* j = new IconJob{key, path, px * 2};
        if (!QueueUserWorkItem(IconProc, j, WT_EXECUTELONGFUNCTION)) {
            delete j;
            g_jobs.fetch_sub(1);
            std::lock_guard<std::mutex> l(g_iconMutex);
            g_icons.erase(key);
        }
    } else {
        std::lock_guard<std::mutex> l(g_iconMutex);  // too busy right now: forget it, ask again at the next paint
        g_icons.erase(key);
    }
    return IconEntry{};
}

// Every cached icon is a DIB section, i.e. one of explorer.exe's limited GDI handles.
// Frees the finished entries that are not in `keep`; pending ones are left to their worker.
static void PruneIcons(std::set<std::wstring> const& keep) {
    std::lock_guard<std::mutex> l(g_iconMutex);
    for (auto it = g_icons.begin(); it != g_icons.end();) {
        if (!it->second.done || keep.count(it->first)) {
            ++it;
            continue;
        }
        if (it->second.bmp) DeleteObject(it->second.bmp);
        it = g_icons.erase(it);
    }
}

static size_t IconCount() {
    std::lock_guard<std::mutex> l(g_iconMutex);
    return g_icons.size();
}

static void FreeIcons() {
    std::lock_guard<std::mutex> l(g_iconMutex);
    for (auto& e : g_icons)
        if (e.second.bmp) DeleteObject(e.second.bmp);
    g_icons.clear();
}

////////////////////////////////////////////////////////////////////////////////
// Offscreen surfaces (32-bit DIB sections) and the cached background
////////////////////////////////////////////////////////////////////////////////
struct Surf {
    HDC dc = nullptr;
    HBITMAP bmp = nullptr;
    HGDIOBJ old = nullptr;
    uint32_t* px = nullptr;
    int w = 0, h = 0;
};
static Surf g_sb, g_sw, g_so;  // layer on black, layer on white, final output

static std::vector<uint32_t> g_bg;  // premultiplied rounded background with border
static int g_bgW = 0, g_bgH = 0, g_bgDark = -1, g_bgOpacity = -1, g_bgRadius = -1, g_bgTransp = -1;
static uint32_t g_bgKey = 0xFFFFFFFFu;

static bool SurfInit(Surf& s, int w, int h) {
    BITMAPINFO bi;
    FillBitmapInfo(bi, w, h);
    HDC scr = GetDC(nullptr);
    s.dc = CreateCompatibleDC(scr);
    ReleaseDC(nullptr, scr);
    void* bits = nullptr;
    s.bmp = CreateDIBSection(s.dc, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!s.dc || !s.bmp || !bits) return false;
    s.px = static_cast<uint32_t*>(bits);
    s.old = SelectObject(s.dc, s.bmp);
    s.w = w;
    s.h = h;
    return true;
}

static void SurfFree(Surf& s) {
    if (s.dc) {
        if (s.old) SelectObject(s.dc, s.old);
        if (s.bmp) DeleteObject(s.bmp);
        DeleteDC(s.dc);
    }
    s = Surf{};
}

static bool EnsureSurfaces() {
    if (g_sb.w == g_w && g_sb.h == g_h && g_sb.dc) return true;
    SurfFree(g_sb);
    SurfFree(g_sw);
    SurfFree(g_so);
    if (!SurfInit(g_sb, g_w, g_h) || !SurfInit(g_sw, g_w, g_h) || !SurfInit(g_so, g_w, g_h)) {
        LOGE(L"[paint] cannot create surfaces %dx%d", g_w, g_h);
        SurfFree(g_sb);
        SurfFree(g_sw);
        SurfFree(g_so);
        return false;
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////
// Theme, fonts, layout
////////////////////////////////////////////////////////////////////////////////
static bool IsDarkTheme() {
    static ULONGLONG last = 0;
    static bool dark = true;
    ULONGLONG now = GetTickCount64();
    if (last == 0 || now - last > 1000) {
        DWORD v = 1, sz = sizeof(v);
        if (RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                         L"AppsUseLightTheme", RRF_RT_REG_DWORD, nullptr, &v, &sz) == ERROR_SUCCESS)
            dark = v == 0;
        else
            dark = true;
        last = now;
    }
    return dark;
}

static bool StyleActive() { return g_cfg.autoStyle && g_prof.valid; }

static inline int ProfR(uint32_t c) { return (c >> 16) & 255; }
static inline int ProfG(uint32_t c) { return (c >> 8) & 255; }
static inline int ProfB(uint32_t c) { return c & 255; }

// Dark or light content. An explicit Dark/Light setting always wins; otherwise
// the sampled Start colour decides, and without it the Windows app theme.
static bool EffectiveDark() {
    if (g_cfg.theme == 1) return true;
    if (g_cfg.theme == 2) return false;
    if (StyleActive()) {
        long sum = 0;
        for (int i = 0; i < kProf; i++)
            sum += (ProfR(g_prof.c[i]) * 299 + ProfG(g_prof.c[i]) * 587 + ProfB(g_prof.c[i]) * 114) / 1000;
        return sum / kProf < 128;
    }
    return IsDarkTheme();
}

static uint32_t ProfKey() {
    uint32_t h = 2166136261u;
    for (int i = 0; i < kProf; i++) {
        h ^= g_prof.c[i];
        h *= 16777619u;
    }
    return h | 1u;
}

static HFONT g_fTitle = nullptr, g_fLabel = nullptr, g_fSym = nullptr, g_fGlyph = nullptr, g_fX = nullptr;
static int g_fontDpi = 0, g_fontIcon = 0, g_fontLabel = 0;

static HFONT MakeFont(int px, int weight, PCWSTR face) {
    return CreateFontW(-px, 0, 0, 0, weight, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                       CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, face);
}

static void FreeFonts() {
    for (HFONT* f : {&g_fTitle, &g_fLabel, &g_fSym, &g_fGlyph, &g_fX}) {
        if (*f) DeleteObject(*f);
        *f = nullptr;
    }
}

static void EnsureFonts() {
    if (g_fTitle && g_fontDpi == g_dpi && g_fontIcon == g_cfg.iconSize && g_fontLabel == g_cfg.labelSize) return;
    FreeFonts();
    g_fTitle = MakeFont(S(g_cfg.labelSize + 3), FW_SEMIBOLD, L"Segoe UI");
    g_fLabel = MakeFont(S(g_cfg.labelSize), FW_NORMAL, L"Segoe UI");
    g_fSym = MakeFont(S(16), FW_NORMAL, L"Segoe MDL2 Assets");
    g_fGlyph = MakeFont(std::max(S(12), S(g_cfg.iconSize) * 7 / 10), FW_NORMAL, L"Segoe MDL2 Assets");
    g_fX = MakeFont(S(10), FW_NORMAL, L"Segoe MDL2 Assets");
    g_fontDpi = g_dpi;
    g_fontIcon = g_cfg.iconSize;
    g_fontLabel = g_cfg.labelSize;
}

////////////////////////////////////////////////////////////////////////////////
// Picker search. Under 4 typed characters a name must contain the text; from 4 on,
// length/3 typos are forgiven (an insert, a delete, a wrong or two swapped neighbour
// characters each count as one), anywhere inside the name.
////////////////////////////////////////////////////////////////////////////////
constexpr size_t kFuzzyFrom = 4;

static std::wstring Lower(std::wstring s) {
    if (!s.empty()) CharLowerBuffW(s.data(), (DWORD)s.size());  // also folds Cyrillic
    return s;
}

// Fewest edits that turn q into some part of t (optimal string alignment, free start and end in t);
// anything above `limit` is reported as limit + 1.
static int FuzzyDistance(std::wstring const& q, std::wstring const& t, int limit) {
    const int m = (int)q.size(), n = (int)t.size();
    std::vector<int> pp(n + 1, 0), p(n + 1, 0), c(n + 1, 0);  // rows i-2, i-1, i; row 0 is all zeros
    for (int i = 1; i <= m; i++) {
        c[0] = i;
        int rowMin = i;
        for (int j = 1; j <= n; j++) {
            int v = std::min({p[j] + 1, c[j - 1] + 1, p[j - 1] + (q[i - 1] != t[j - 1] ? 1 : 0)});
            if (i > 1 && j > 1 && q[i - 1] == t[j - 2] && q[i - 2] == t[j - 1]) v = std::min(v, pp[j - 2] + 1);
            c[j] = v;
            rowMin = std::min(rowMin, v);
        }
        if (rowMin > limit) return limit + 1;
        std::swap(pp, p);
        std::swap(p, c);
    }
    int best = limit + 1;
    for (int j = 0; j <= n; j++) best = std::min(best, p[j]);
    return best;
}

// Rows for the current query, best first: name starts with it, a word starts with it, contains it,
// then fuzzy matches by number of typos. Ties keep the alphabetical order of g_apps.
static void RebuildView() {
    g_view.clear();
    const std::wstring q = Lower(g_query);
    if (q.empty()) {
        for (int i = 0; i < (int)g_apps.size(); i++) g_view.push_back(i);
        return;
    }
    const int allowed = q.size() >= kFuzzyFrom ? (int)q.size() / 3 : 0;
    std::vector<std::pair<int, int>> hits;  // (rank, app index)
    for (int i = 0; i < (int)g_apps.size(); i++) {
        const std::wstring name = Lower(g_apps[i].name);
        size_t pos = name.find(q);
        int rank = -1;
        if (pos == 0) {
            rank = 0;
        } else if (pos != std::wstring::npos) {
            rank = 2;
            for (; pos != std::wstring::npos; pos = name.find(q, pos + 1))
                if (!IsCharAlphaNumericW(name[pos - 1])) {
                    rank = 1;
                    break;
                }
        } else if (allowed) {
            int d = FuzzyDistance(q, name, allowed);
            if (d <= allowed) rank = 2 + d;
        }
        if (rank >= 0) hits.push_back({rank, i});
    }
    std::stable_sort(hits.begin(), hits.end(), [](auto const& a, auto const& b) { return a.first < b.first; });
    for (auto const& h : hits) g_view.push_back(h.second);
}

static Item const& PickerItem(int row) { return g_apps[g_view[row]]; }

struct Lay {
    int pad, headerH, cols, rows, cellW, tileW, tileH, icon, label, totalH, rowH, pic;
    RECT plus, file, content, search;
};


static void FindFreeTilePos(Lay const& L, int moving, int wantedX, int wantedY, int& outX, int& outY);


static int CardPad() { return S(12); }

// A tile: the icon plus side margins, never narrower than two lines of label need.
static int TileWidth() { return std::max(S(g_cfg.iconSize) + S(28), S(72)); }

// The width setting, widened when the grid's columns need more room, so tiles never crowd;
// without the grid the columns follow the width instead.
static int CardWidth() {
    int w = S(g_cfg.width);
    if (g_cfg.grid) w = std::max(w, g_cfg.columns * TileWidth() + 2 * CardPad());
    return w;
}

static Lay MakeLay() {
    Lay L{};
    L.pad = CardPad();
    L.headerH = S(32);
    L.icon = S(g_cfg.iconSize);
    L.label = S(g_cfg.labelSize);
    L.rowH = S(40);
    L.pic = S(24);
    int inner = std::max(1, g_w - 2 * L.pad);
    int minTile = TileWidth();
    L.cols = g_cfg.grid ? g_cfg.columns : std::max(1, inner / minTile);
    L.cols = std::max(1, std::min(L.cols, 8));
    L.cellW = std::max(1, inner / L.cols);
    L.tileW = g_cfg.grid ? L.cellW : std::max(1, std::min(L.cellW, minTile));  // grid: a tile is its whole cell
    L.tileH = L.icon + (int)(L.label * 2 * 1.35) + S(18);
    L.plus = RECT{g_w - L.pad - S(32), L.pad, g_w - L.pad, L.pad + S(32)};
    L.file = RECT{L.plus.left - S(34), L.pad, L.plus.left - S(2), L.pad + S(32)};
    L.content = RECT{L.pad, L.pad + L.headerH, g_w - L.pad, g_h - L.pad};
    if (g_mode == 1) {
        L.search = RECT{L.pad, L.pad + L.headerH, g_w - L.pad, L.pad + L.headerH + S(30)};
        L.content.top = L.search.bottom + S(6);
        L.totalH = (int)g_view.size() * L.rowH;
    } else if (g_cfg.grid) {
        // the whole card is split into cells: as many rows as fit, more only if tiles sit lower
        L.rows = std::max(1, (int)(L.content.bottom - L.content.top) / std::max(1, L.tileH));
        for (auto const& it : g_items) L.rows = std::max(L.rows, it.gr + 1);
        L.totalH = L.rows * L.tileH;
    } else {
        int bottom = 0;
        for (auto const& it : g_items) bottom = std::max(bottom, S(std::max(0, it.ly)) + L.tileH);
        L.totalH = bottom;
    }
    return L;
}

////////////////////////////////////////////////////////////////////////////////
// Grid mode: the whole card is split into cells and every tile has one of its own (Item::gc, gr),
// anywhere, with any gaps. A dragged tile lands in the cell under it; tiles in the way shift one
// cell along, like icons on a phone, and every tile slides to its cell instead of jumping.
////////////////////////////////////////////////////////////////////////////////
static std::vector<POINT> g_tilePos;  // where each tile is drawn now (content coordinates)
static std::vector<POINT> g_preview;  // while dragging: every tile's cell (x column, y row) if dropped now
static POINT g_dropCell{-1, -1};      // the cell the dragged tile would land in
static bool g_frameTimer = false;     // re-arming a running timer would restart it on every mouse move
static bool g_frameDirty = false;     // something changed that the next frame has to show

// Asks for a repaint on the next frame instead of now: a mouse reports up to 1000 moves a
// second, and painting the whole card for each kept Explorer's thread busy all the time.
static void RequestFrame() {
    g_frameDirty = true;
    if (!g_frameTimer && g_hwnd) g_frameTimer = SetTimer(g_hwnd, kTimerFrame, 15, nullptr) != 0;
}

static POINT CellPos(Lay const& L, POINT c) { return POINT{c.x * L.cellW, c.y * L.tileH}; }

static POINT CellOf(int i) {
    if (g_dragging && i < (int)g_preview.size()) return g_preview[i];
    return POINT{std::max(0, g_items[i].gc), std::max(0, g_items[i].gr)};
}

// The free cell nearest to (c, r); moving along the row costs less than changing rows, ties go
// to the upper, then the left cell.
static POINT NearestFreeCell(std::set<int> const& used, int cols, int c, int r) {
    int maxRow = r;
    for (int k : used) maxRow = std::max(maxRow, k / cols);
    POINT best{0, maxRow + 1};
    int bestCost = INT_MAX;
    for (int y = 0; y <= maxRow + 1; y++)
        for (int x = 0; x < cols; x++) {
            if (used.count(y * cols + x)) continue;
            int cost = std::abs(x - c) + 2 * std::abs(y - r);
            if (cost < bestCost) {
                bestCost = cost;
                best = POINT{x, y};
            }
        }
    return best;
}

// Gives every tile a cell of its own. Tiles keep their cell when it still exists and is not
// taken; the others (fewer columns now, or two on one cell) go to the nearest free cell, and
// tiles without a cell yet (new ones) to the first free cell in reading order.
static bool PlaceGridCells(int cols) {
    std::set<int> used;
    std::vector<int> displaced, fresh;
    for (int i = 0; i < (int)g_items.size(); i++) {
        Item const& it = g_items[i];
        if (it.gc < 0 || it.gr < 0) fresh.push_back(i);
        else if (it.gc >= cols || !used.insert(it.gr * cols + it.gc).second) displaced.push_back(i);
    }
    for (int i : displaced) {
        Item& it = g_items[i];
        POINT p = NearestFreeCell(used, cols, std::min(it.gc, cols - 1), it.gr);
        it.gc = p.x;
        it.gr = p.y;
        used.insert(p.y * cols + p.x);
    }
    for (int i : fresh) {
        int k = 0;
        while (used.count(k)) k++;
        g_items[i].gc = k % cols;
        g_items[i].gr = k / cols;
        used.insert(k);
    }
    return !displaced.empty() || !fresh.empty();
}

// The cell under the middle of the dragged tile when the cursor is at (x, y) in the card.
static POINT DropCell(Lay const& L, int x, int y) {
    int cx = x - (int)g_grab.x + L.tileW / 2 - (int)L.content.left;
    int cy = y - (int)g_grab.y + L.tileH / 2 - (int)L.content.top + g_scroll;
    int col = cx < 0 ? 0 : std::min(cx / std::max(1, L.cellW), L.cols - 1);
    int row = cy < 0 ? 0 : std::min(cy / std::max(1, L.tileH), L.rows - 1);
    return POINT{col, row};
}

// Where every tile goes if the dragged one is dropped on `drop`. Cells are counted in reading
// order (a row's end continues at the next row's start). If `drop` is taken, that tile and the
// ones right after it shift one cell toward the gap the dragged tile left, up to the first empty
// cell: | 1 | 2 | 3 | 4 | with 4 dropped on 2 becomes | 1 | 4 | 2 | 3 |.
static void BuildPreview(Lay const& L, POINT drop) {
    const int cols = L.cols;
    g_preview.assign(g_items.size(), POINT{});
    std::map<int, int> at;  // cell number -> tile, without the dragged one
    for (int i = 0; i < (int)g_items.size(); i++) {
        g_preview[i] = POINT{std::max(0, g_items[i].gc), std::max(0, g_items[i].gr)};
        if (i != g_dragFrom) at[g_preview[i].y * cols + g_preview[i].x] = i;
    }
    if (g_dragFrom < 0 || g_dragFrom >= (int)g_preview.size()) return;
    const int from = g_preview[g_dragFrom].y * cols + g_preview[g_dragFrom].x;
    const int to = drop.y * cols + drop.x;
    g_preview[g_dragFrom] = drop;
    const int dir = from > to ? 1 : -1;  // the dragged tile's own cell is empty, so the run ends there at the latest
    for (int k = to; at.count(k); k += dir) {
        int n = k + dir;
        g_preview[at[k]] = POINT{n % cols, n / cols};
    }
}

// Keeps g_tilePos one entry per tile (new tiles appear in their cell at once) and says
// whether any tile still has to slide.
static bool TilesMoving(Lay const& L) {
    if (g_tilePos.size() > g_items.size()) g_tilePos.clear();
    while (g_tilePos.size() < g_items.size()) g_tilePos.push_back(CellPos(L, CellOf((int)g_tilePos.size())));
    for (int i = 0; i < (int)g_items.size(); i++) {
        if (g_dragging && i == g_dragFrom) continue;
        POINT t = CellPos(L, CellOf(i));
        if (g_tilePos[i].x != t.x || g_tilePos[i].y != t.y) return true;
    }
    return false;
}

// One animation step: every tile covers a third of what is left to its cell (~120 ms in all).
static void StepTiles(Lay const& L) {
    auto step = [](LONG& v, LONG t) {
        LONG d = t - v;
        LONG m = d / 3;
        if (m == 0) m = d > 0 ? 1 : d < 0 ? -1 : 0;
        v += std::abs(d) <= 2 ? d : m;
    };
    for (int i = 0; i < (int)g_items.size() && i < (int)g_tilePos.size(); i++) {
        if (g_dragging && i == g_dragFrom) continue;
        POINT t = CellPos(L, CellOf(i));
        step(g_tilePos[i].x, t.x);
        step(g_tilePos[i].y, t.y);
    }
}

static void ClampScroll(Lay const& L) {
    int view = std::max(0, (int)(L.content.bottom - L.content.top));
    int maxS = std::max(0, L.totalH - view);
    int& s = g_mode == 1 ? g_pscroll : g_scroll;
    s = std::max(0, std::min(s, maxS));
}

// Free position of a tile, clamped so that it always stays inside the card.
static RECT TileRect(Lay const& L, int i) {
    if (g_cfg.grid) {
        POINT p = i < (int)g_tilePos.size() ? g_tilePos[i] : CellPos(L, CellOf(i));
        int left = (int)L.content.left + p.x, top = (int)L.content.top + p.y - g_scroll;
        return RECT{left, top, left + L.tileW, top + L.tileH};
    }
    int cw = (int)(L.content.right - L.content.left);
    int x = std::max(0, std::min(S(std::max(0, g_items[i].lx)), cw - L.tileW));
    int y = S(std::max(0, g_items[i].ly));
    int left = (int)L.content.left + x, top = (int)L.content.top + y - g_scroll;
    return RECT{left, top, left + L.tileW, top + L.tileH};
}

static RECT RowRect(Lay const& L, int i) {
    int y = L.content.top + i * L.rowH - g_pscroll;
    return RECT{L.content.left, y, L.content.right, y + L.rowH};
}

static RECT RemoveRect(Lay const& L, int i) {
    RECT t = TileRect(L, i);
    int s = S(18), m = S(4);
    return RECT{t.right - s - m, t.top + m, t.right - m, t.top + m + s};
}

static int HitTest(int x, int y) {
    Lay L = MakeLay();
    POINT p{x, y};
    if (PtInRect(&L.plus, p)) return g_mode == 1 ? -4 : -2;
    if (g_mode == 0 && PtInRect(&L.file, p)) return -3;
    if (!PtInRect(&L.content, p)) return -1;
    if (g_mode == 1) {
        int i = (y - (int)L.content.top + g_pscroll) / std::max(1, L.rowH);
        return i >= 0 && i < (int)g_view.size() ? i : -1;
    }
    for (int i = (int)g_items.size() - 1; i >= 0; i--) {  // topmost first
        RECT r = TileRect(L, i);
        if (PtInRect(&r, p)) return i;
    }
    return -1;
}

static bool InRemoveBtn(int i, int x, int y) {
    if (g_mode != 0 || i < 0 || i >= (int)g_items.size()) return false;
    Lay L = MakeLay();
    RECT r = RemoveRect(L, i);
    POINT p{x, y};
    return PtInRect(&r, p) && PtInRect(&L.content, p);
}

// Puts every tile that has no position yet into the first free grid cell.
static void PlaceUnplaced() {
    if (g_cfg.grid) return;
    bool any = false;
    for (auto const& it : g_items)
        if (it.lx < 0 || it.ly < 0) any = true;
    if (!any || g_w < 100) return;
    Lay L = MakeLay();
    auto L96 = [&](int px) { return std::max(1, MulDiv(px, 96, g_dpi)); };
    int cw = L96(L.cellW), tw = L96(L.tileW), th = L96(L.tileH);
    int ox = std::max(0, (cw - tw) / 2);
    for (auto& it : g_items) {
        if (it.lx >= 0 && it.ly >= 0) continue;
        bool done = false;
        for (int r = 0; r < 400 && !done; r++) {
            for (int c = 0; c < L.cols && !done; c++) {
                int x = c * cw + ox, y = r * th;
                bool hit = false;
                for (auto const& o : g_items) {
                    if (&o == &it || o.lx < 0 || o.ly < 0) continue;
                    if (x < o.lx + tw && o.lx < x + tw && y < o.ly + th && o.ly < y + th) {
                        hit = true;
                        break;
                    }
                }
                if (!hit) {
                    it.lx = x;
                    it.ly = y;
                    done = true;
                }
            }
        }
        if (!done) it.lx = it.ly = 0;
    }
    SaveItems();
}

// Free placement: two tiles collide only when their boxes overlap by more than a few pixels.
// Touching is fine (grid-arranged tiles touch), and the slack absorbs 96-dpi rounding.
static bool TilesCollide(Lay const& L, int ax, int ay, int bx, int by) {
    const int slack = S(4);
    return ax < bx + L.tileW - slack && bx < ax + L.tileW - slack && ay < by + L.tileH - slack &&
           by < ay + L.tileH - slack;
}

// After the card got narrower or the tiles bigger, a saved position can stick out past the right
// edge (drawn clamped to it, on top of its neighbour) or overlap another tile. Going in reading
// order, tiles that still fit and do not overlap an earlier one stay put; the others move to the
// nearest free spot.
static void FixOverlaps() {
    if (g_cfg.grid || g_w < 100 || g_dragging || g_items.empty()) return;
    Lay L = MakeLay();
    const int cw = (int)(L.content.right - L.content.left);
    const int maxX = std::max(0, cw - L.tileW);
    const int tol = S(2);  // positions are stored at 96 dpi: rounding alone must never move a tile
    std::vector<int> order(g_items.size());
    for (int i = 0; i < (int)order.size(); i++) order[i] = i;
    std::stable_sort(order.begin(), order.end(), [](int a, int b) {
        return g_items[a].ly != g_items[b].ly ? g_items[a].ly < g_items[b].ly : g_items[a].lx < g_items[b].lx;
    });
    std::vector<int> kept;
    bool moved = false;
    for (int i : order) {
        int x = S(std::max(0, g_items[i].lx)), y = S(std::max(0, g_items[i].ly));
        bool ok = x <= maxX + tol;
        for (int k = 0; ok && k < (int)kept.size(); k++) {
            int ox = S(std::max(0, g_items[kept[k]].lx)), oy = S(std::max(0, g_items[kept[k]].ly));
            if (TilesCollide(L, x, y, ox, oy)) ok = false;
        }
        if (!ok) {
            int fx = 0, fy = 0;
            FindFreeTilePos(L, i, std::min(x, maxX), y, fx, fy);
            g_items[i].lx = MulDiv(fx, 96, g_dpi);
            g_items[i].ly = MulDiv(fy, 96, g_dpi);
            moved = true;
        }
        kept.push_back(i);
    }
    if (moved) {
        LOGI(L"[tiles] moved tiles that no longer fit the card");
        SaveItems();
    }
}

////////////////////////////////////////////////////////////////////////////////
// Painting. Content is drawn with GDI twice (on black and on white); the pair
// gives exact per-pixel alpha, so text and icons stay anti-aliased on a
// rounded background. Icon bitmaps are snapshotted once per repaint so both
// layers always see the same state.
////////////////////////////////////////////////////////////////////////////////
static std::vector<IconEntry> g_snap;   // tiles, parallel to g_items
static std::vector<IconEntry> g_psnap;  // picker rows g_psnapFirst .. g_psnapFirst + size - 1
static int g_psnapFirst = 0;

static void VisibleRows(Lay const& L, int& first, int& last) {
    int n = (int)g_view.size();
    int view = std::max(0, (int)(L.content.bottom - L.content.top));
    first = std::max(0, g_pscroll / std::max(1, L.rowH));
    last = std::min(n - 1, (g_pscroll + view) / std::max(1, L.rowH));
}

// Frees every cached icon except the tiles' (and, in the picker, the rows on screen).
// The snapshots hold copies of the bitmap handles, so the picker one is dropped with them.
static void KeepOnlyNeededIcons(Lay const& L, bool visibleRows) {
    std::set<std::wstring> keep;
    for (auto const& it : g_items) keep.insert(IconKey(it.path, L.icon));
    if (visibleRows) {
        int first = 0, last = -1;
        VisibleRows(L, first, last);
        for (int i = first; i <= last; i++) keep.insert(IconKey(PickerItem(i).path, L.pic));
    } else {
        g_psnap.clear();
    }
    PruneIcons(keep);
}

static void SnapshotIcons() {
    Lay L = MakeLay();
    ClampScroll(L);
    if (g_mode == 1) {
        int first = 0, last = -1;
        VisibleRows(L, first, last);
        g_psnapFirst = first;
        g_psnap.assign(last >= first ? (size_t)(last - first + 1) : 0, IconEntry{});
        for (int i = first; i <= last; i++) g_psnap[i - first] = IconLookup(PickerItem(i).path, L.pic);
    } else {
        g_snap.assign(g_items.size(), IconEntry{});
        for (size_t i = 0; i < g_items.size(); i++) g_snap[i] = IconLookup(g_items[i].path, L.icon);
    }
    if (IconCount() > kIconCacheMax) KeepOnlyNeededIcons(L, g_mode == 1);  // scrolling a long program list
}

static void DrawIconAt(HDC dc, HDC tmp, IconEntry const& ie, int x, int y, int size, BYTE alpha, COLORREF fallback) {
    if (ie.done && ie.bmp) {
        HGDIOBJ old = SelectObject(tmp, ie.bmp);
        BLENDFUNCTION bf{AC_SRC_OVER, 0, alpha, AC_SRC_ALPHA};
        AlphaBlend(dc, x, y, size, size, tmp, 0, 0, ie.w, ie.h, bf);
        SelectObject(tmp, old);
    } else {
        SelectObject(dc, g_fGlyph);
        SetTextColor(dc, fallback);
        RECT gr{x, y, x + size, y + size};
        DrawTextW(dc, L"\uECAA", -1, &gr, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
    }
}

static int FindByPath(std::wstring const& path) {
    for (size_t i = 0; i < g_items.size(); i++)
        if (_wcsicmp(g_items[i].path.c_str(), path.c_str()) == 0) return (int)i;
    return -1;
}

static void DrawLayer(HDC dc, bool dark) {
    Lay L = MakeLay();
    ClampScroll(L);
    const bool pick = g_mode == 1;
    COLORREF text = dark ? RGB(245, 245, 245) : RGB(24, 24, 24);
    COLORREF sub = dark ? RGB(170, 170, 170) : RGB(90, 90, 90);
    COLORREF frame = dark ? RGB(120, 120, 120) : RGB(130, 130, 130);
    COLORREF accent = dark ? RGB(96, 165, 250) : RGB(0, 95, 184);
    SetBkMode(dc, TRANSPARENT);
    auto frameRect = [&](RECT const& r) {
        HBRUSH b = CreateSolidBrush(frame);
        FrameRect(dc, &r, b);
        DeleteObject(b);
    };

    if (pick || g_cfg.showTitle) {
        SelectObject(dc, g_fTitle);
        SetTextColor(dc, text);
        RECT tr{L.pad, L.pad, (pick ? L.plus.left : L.file.left) - S(4), L.pad + L.headerH};
        DrawTextW(dc, pick ? Tr(T_PickerTitle) : g_cfg.title.c_str(), -1, &tr,
                  DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX);
    }
    SelectObject(dc, g_fSym);
    SetTextColor(dc, text);
    RECT pr = L.plus;
    DrawTextW(dc, pick ? L"\uE73E" : L"\uE710", -1, &pr, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
    if ((g_hover == -2 || g_hover == -4) && !g_dragging) frameRect(L.plus);
    if (!pick) {
        RECT fr = L.file;
        DrawTextW(dc, L"\uE838", -1, &fr, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
        if (g_hover == -3 && !g_dragging) frameRect(L.file);
    }
    if (pick) {  // search line: what has been typed, with a caret, or the hint
        frameRect(L.search);
        RECT gr{L.search.left + S(6), L.search.top, L.search.left + S(28), L.search.bottom};
        SelectObject(dc, g_fX);
        SetTextColor(dc, sub);
        DrawTextW(dc, L"", -1, &gr, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
        SelectObject(dc, g_fLabel);
        RECT qr{gr.right + S(4), L.search.top, L.search.right - S(8), L.search.bottom};
        if (g_query.empty()) {
            DrawTextW(dc, Tr(T_SearchHint), -1, &qr, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX);
        } else {
            SetTextColor(dc, text);
            DrawTextW(dc, g_query.c_str(), -1, &qr, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_PATH_ELLIPSIS | DT_NOPREFIX);
            SIZE sz{};
            GetTextExtentPoint32W(dc, g_query.c_str(), (int)g_query.size(), &sz);
            int cx = std::min((int)qr.right, (int)(qr.left + sz.cx) + S(1));
            RECT caret{cx, L.search.top + S(7), cx + std::max(1, S(1)), L.search.bottom - S(7)};
            HBRUSH b = CreateSolidBrush(text);
            FillRect(dc, &caret, b);
            DeleteObject(b);
        }
    }

    int saved = SaveDC(dc);
    IntersectClipRect(dc, L.content.left, L.content.top, L.content.right, L.content.bottom);
    if (pick) {
        int n = (int)g_view.size();
        if (n == 0) {
            SelectObject(dc, g_fLabel);
            SetTextColor(dc, sub);
            RECT hr = L.content;
            hr.top += S(8);
            PCWSTR msg = !g_appsLoaded ? Tr(T_AppsLoading) : g_apps.empty() ? Tr(T_AppsEmpty) : Tr(T_NoMatches);
            DrawTextW(dc, msg, -1, &hr, DT_LEFT | DT_WORDBREAK | DT_NOPREFIX);
        } else {
            int first = 0, last = -1;
            VisibleRows(L, first, last);
            HDC tmp = CreateCompatibleDC(dc);
            for (int i = first; i <= last; i++) {
                RECT r = RowRect(L, i);
                if (i == g_hover && !g_dragging) {
                    RECT fr = r;
                    InflateRect(&fr, 0, -S(1));
                    frameRect(fr);
                }
                size_t si = (size_t)(i - g_psnapFirst);
                DrawIconAt(dc, tmp, i >= g_psnapFirst && si < g_psnap.size() ? g_psnap[si] : IconEntry{}, r.left + S(8),
                           r.top + (L.rowH - L.pic) / 2, L.pic, 255, sub);
                SelectObject(dc, g_fLabel);
                SetTextColor(dc, text);
                RECT nr{r.left + S(8) + L.pic + S(10), r.top, r.right - S(34), r.bottom};
                DrawTextW(dc, PickerItem(i).name.c_str(), -1, &nr, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX);
                if (FindByPath(PickerItem(i).path) >= 0) {
                    SelectObject(dc, g_fSym);
                    SetTextColor(dc, accent);
                    RECT cr{r.right - S(32), r.top, r.right - S(4), r.bottom};
                    DrawTextW(dc, L"\uE73E", -1, &cr, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
                }
            }
            DeleteDC(tmp);
        }
    } else if (g_items.empty()) {
        SelectObject(dc, g_fLabel);
        SetTextColor(dc, sub);
        RECT hr = L.content;
        hr.top += S(8);
        DrawTextW(dc, Tr(T_CardEmpty), -1, &hr,
                  DT_LEFT | DT_WORDBREAK | DT_NOPREFIX);
    } else {
        HDC tmp = CreateCompatibleDC(dc);
        auto drawTile = [&](int i, RECT const& tr, bool ghost) {
            bool hot = !ghost && i == g_hover && !g_dragging;
            if (hot) {
                RECT fr = tr;
                InflateRect(&fr, -S(2), -S(2));
                frameRect(fr);
            }
            int ix = tr.left + (L.tileW - L.icon) / 2, iy = tr.top + S(8);
            DrawIconAt(dc, tmp, i < (int)g_snap.size() ? g_snap[i] : IconEntry{}, ix, iy, L.icon, ghost ? 215 : 255, sub);
            SelectObject(dc, g_fLabel);
            SetTextColor(dc, text);
            RECT lr{tr.left + S(2), iy + L.icon + S(4), tr.right - S(2), iy + L.icon + S(4) + (int)(L.label * 2 * 1.35)};
            DrawTextW(dc, g_items[i].name.c_str(), -1, &lr,
                      DT_CENTER | DT_WORDBREAK | DT_END_ELLIPSIS | DT_NOPREFIX | DT_EDITCONTROL);
            if (hot) {  // small "x" in the corner removes the tile
                RECT rr = RemoveRect(L, i);
                if (g_hoverRemove) frameRect(rr);
                SelectObject(dc, g_fX);
                SetTextColor(dc, text);
                DrawTextW(dc, L"\uE711", -1, &rr, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
            }
        };
        for (int i = 0; i < (int)g_items.size(); i++) {
            if (g_dragging && i == g_dragFrom) continue;  // drawn last, under the cursor
            RECT tr = TileRect(L, i);
            if (tr.bottom < L.content.top || tr.top > L.content.bottom) continue;
            drawTile(i, tr, false);
        }
        if (g_dragging && g_cfg.grid && g_dropCell.x >= 0) {  // the cell it would land in
            POINT p = CellPos(L, g_dropCell);
            RECT cr{(int)L.content.left + p.x, (int)L.content.top + p.y - g_scroll, 0, 0};
            cr.right = cr.left + L.tileW;
            cr.bottom = cr.top + L.tileH;
            InflateRect(&cr, -S(2), -S(2));
            frameRect(cr);
        }
        if (g_dragging && g_dragFrom >= 0 && g_dragFrom < (int)g_items.size()) {
            int gx = (int)L.content.left + g_ghost.x;
            int gy = (int)L.content.top + g_ghost.y - g_scroll;
            RECT tr{gx, gy, gx + L.tileW, gy + L.tileH};
            drawTile(g_dragFrom, tr, true);
        }
        DeleteDC(tmp);
    }
    RestoreDC(dc, saved);
}

static inline float Clamp01(float v) { return v < 0.f ? 0.f : v > 1.f ? 1.f : v; }

// Rounded background with a thin border, premultiplied. With the automatic style
// every row takes the colour of the Start edge at the same height (smooth gradient),
// so the card continues Start without a seam. Cached: it only changes with size,
// theme, colours, opacity or corner radius.
static void BuildBackground(bool dark) {
    const bool useStyle = StyleActive();
    const uint32_t key = useStyle ? ProfKey() : 0u;
    int rpx = S(g_cfg.radius);
    if (g_bgW == g_w && g_bgH == g_h && g_bgDark == (dark ? 1 : 0) && g_bgOpacity == g_cfg.opacity &&
        g_bgRadius == rpx && g_bgTransp == g_cfg.styleTransp && g_bgKey == key && g_bg.size() == (size_t)g_w * g_h)
        return;
    g_bg.assign((size_t)g_w * g_h, 0u);
    // With the sampled colour the card is as opaque as Start looks: Start's acrylic blurs what is
    // behind it, while a see-through card would show the desktop sharp and darken the colour.
    const float bgA = useStyle ? 1.f - g_cfg.styleTransp / 100.f : g_cfg.opacity / 100.f;
    const float brC = dark ? 255.f : 0.f;
    const float brA = 0.12f;
    const float hw = g_w / 2.f, hh = g_h / 2.f;
    const float r = std::min((float)rpx, std::min(hw, hh));
    const int edge = (int)std::ceil(r) + 3;
    auto B = [](float v) { return (uint32_t)(Clamp01(v) * 255.f + 0.5f); };

    std::vector<float> rowR((size_t)g_h), rowG((size_t)g_h), rowB((size_t)g_h);
    for (int y = 0; y < g_h; y++) {
        if (useStyle) {
            float f = (y + 0.5f) / (float)g_h * kProf - 0.5f;
            f = f < 0.f ? 0.f : f > kProf - 1 ? (float)(kProf - 1) : f;
            int i0 = (int)f, i1 = std::min(i0 + 1, kProf - 1);
            float t = f - i0;
            uint32_t c0 = g_prof.c[i0], c1 = g_prof.c[i1];
            rowR[y] = ProfR(c0) + (ProfR(c1) - ProfR(c0)) * t;
            rowG[y] = ProfG(c0) + (ProfG(c1) - ProfG(c0)) * t;
            rowB[y] = ProfB(c0) + (ProfB(c1) - ProfB(c0)) * t;
        } else {
            rowR[y] = rowG[y] = rowB[y] = dark ? 32.f : 243.f;
        }
    }
    for (int y = 0; y < g_h; y++) {
        const bool rowInterior = y >= edge && y < g_h - edge;
        uint32_t inner = (B(bgA) << 24) | (B(rowR[y] / 255.f * bgA) << 16) | (B(rowG[y] / 255.f * bgA) << 8) |
                         B(rowB[y] / 255.f * bgA);
        float py = y + 0.5f - hh;
        float qy = std::fabs(py) - (hh - r);
        for (int x = 0; x < g_w; x++) {
            if (rowInterior && x >= edge && x < g_w - edge) {  // fast path, no edge maths needed
                g_bg[(size_t)y * g_w + x] = inner;
                continue;
            }
            float px = x + 0.5f - hw;
            float qx = std::fabs(px) - (hw - r);
            float ox = qx > 0.f ? qx : 0.f, oy = qy > 0.f ? qy : 0.f;
            float sd = std::sqrt(ox * ox + oy * oy) + std::min(std::max(qx, qy), 0.f) - r;
            float cov = Clamp01(0.5f - sd);
            float bcov = Clamp01(0.5f - std::fabs(sd + 0.5f));
            float a = bgA * cov;
            float cr = rowR[y] / 255.f * a, cg = rowG[y] / 255.f * a, cb = rowB[y] / 255.f * a;
            float ba = brA * bcov;
            float bc = brC / 255.f * ba;
            cr = bc + cr * (1.f - ba);
            cg = bc + cg * (1.f - ba);
            cb = bc + cb * (1.f - ba);
            a = ba + a * (1.f - ba);
            g_bg[(size_t)y * g_w + x] = (B(a) << 24) | (B(cr) << 16) | (B(cg) << 8) | B(cb);
        }
    }
    g_bgW = g_w;
    g_bgH = g_h;
    g_bgDark = dark ? 1 : 0;
    g_bgOpacity = g_cfg.opacity;
    g_bgRadius = rpx;
    g_bgTransp = g_cfg.styleTransp;
    g_bgKey = key;
}

static void Composite() {
    const size_t n = (size_t)g_w * g_h;
    for (size_t i = 0; i < n; i++) {
        uint32_t b = g_sb.px[i] & 0x00FFFFFFu, w = g_sw.px[i] & 0x00FFFFFFu;
        uint32_t bgv = g_bg[i];
        if (b == 0u && w == 0x00FFFFFFu) {  // nothing drawn here
            g_so.px[i] = bgv;
            continue;
        }
        int br = (b >> 16) & 255, bgc = (b >> 8) & 255, bb = b & 255;
        int wr = (w >> 16) & 255, wg = (w >> 8) & 255, wb = w & 255;
        int diff = ((wr - br) + (wg - bgc) + (wb - bb)) / 3;
        int la = std::max(0, std::min(255, 255 - diff));
        int inv = 255 - la;
        int oa = std::min(255, la + (int)(bgv >> 24) * inv / 255);
        int orr = std::min(255, std::min(br, la) + (int)((bgv >> 16) & 255) * inv / 255);
        int og = std::min(255, std::min(bgc, la) + (int)((bgv >> 8) & 255) * inv / 255);
        int ob = std::min(255, std::min(bb, la) + (int)(bgv & 255) * inv / 255);
        g_so.px[i] = ((uint32_t)oa << 24) | ((uint32_t)orr << 16) | ((uint32_t)og << 8) | (uint32_t)ob;
    }
}

////////////////////////////////////////////////////////////////////////////////
// Animation. Frames are produced by a separate thread, once per screen refresh
// (DwmFlush), and only change the window's position, so the window thread being
// busy cannot make them stutter. The card sits right under the taskbar in the
// z-order, so like Start it slides out from behind the taskbar; dy is how far
// below its final place it is (g_aslide = fully hidden behind the taskbar).
////////////////////////////////////////////////////////////////////////////////
static void AnimCompute(int kind, LONGLONG t0, BYTE& alpha, int& dy, bool& done) {
    alpha = 255;
    dy = 0;
    done = true;
    if (kind == 3) {
        alpha = 0;
        dy = g_aslide.load();
        done = false;
        return;
    }
    if (kind != 1 && kind != 2) return;
    double ms = (double)(NowQpc() - t0) * 1000.0 / (double)g_qpcFreq;
    double t = ms / (kind == 2 ? kHideMs : kShowMs);
    t = t < 0.0 ? 0.0 : t > 1.0 ? 1.0 : t;
    done = t >= 1.0;
    double e = kind == 1 ? std::pow(1.0 - t, kShowPow) : HideEase(t);
    dy = (int)((double)g_aslide.load() * e + 0.5);
}

static DWORD WINAPI AnimThread(LPVOID) {
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL);
    while (!g_quit) {
        WaitForSingleObject(g_animEvt, INFINITE);  // StartAnim and Wh_ModUninit signal it
        if (g_quit) break;
        int kind = g_animKind.load();
        if (kind != 1 && kind != 2) continue;
        int gen = g_animGen.load();
        for (;;) {
            HWND h = g_hwnd;
            if (g_quit || !h || gen != g_animGen.load()) break;
            BYTE a = 255;
            int dy = 0;
            bool done = true;
            AnimCompute(kind, g_animQpc0.load(), a, dy, done);
            POINT pt{g_apx.load(), g_apy.load() + dy};
            BLENDFUNCTION bf{AC_SRC_OVER, 0, a, AC_SRC_ALPHA};
            // No new bitmap: only position and opacity change.
            UpdateLayeredWindow(h, nullptr, &pt, nullptr, nullptr, nullptr, 0, &bf, ULW_ALPHA);
            if (done) {
                if (kind == 2) PostMessageW(h, WM_APP_ANIMDONE, (WPARAM)gen, 0);
                else if (gen == g_animGen.load()) g_animKind = 0;
                break;
            }
            if (FAILED(DwmFlush())) Sleep(8);
        }
    }
    return 0;
}

static void SyncAnimPos() {
    g_apx = g_x;
    g_apy = g_y;
}

// Keeps the card invisible until its first frame is painted, then StartAnim(1) runs it.
static void HoldAnim() {
    g_aslide = g_slide;
    SyncAnimPos();
    g_animKind = g_cfg.animate ? 3 : 0;
}

// 1 show, 2 hide. The rise is timed from when this Start opening was first seen, so a card that
// appears late (the first opening waits for Start's region) joins Start where it is in its motion.
// A sink that interrupts a rise starts from the card's current height instead of jumping up.
static void StartAnim(int kind) {
    g_hiding = kind == 2;
    if (!g_cfg.animate) {
        g_animKind = 0;
        return;
    }
    auto ms = [](double v) { return (LONGLONG)(v / 1000.0 * (double)g_qpcFreq); };
    LONGLONG now = NowQpc(), t0 = now;
    if (kind == 1 && g_openQpc) t0 = std::min(now, g_openQpc);
    if (kind == 2) {
        t0 = now - ms(kHideLeadMs);
        if (g_animKind.load() == 1 && g_slide > 0) {
            BYTE a;
            int dy;
            bool done;
            AnimCompute(1, g_animQpc0.load(), a, dy, done);
            t0 = now - ms(HideEaseInverse(std::min(1.0, (double)dy / g_slide)) * kHideMs);
        }
    }
    g_aslide = g_slide;
    SyncAnimPos();
    g_animQpc0 = t0;
    g_animGen++;
    g_animKind = kind;
    if (g_animEvt) SetEvent(g_animEvt);
}

// Puts the already composed frame on screen with the current animation state.
static void Present() {
    if (!g_hwnd || !g_shown || !g_so.dc || g_so.w != g_w || g_so.h != g_h) return;
    BYTE a = 255;
    int dy = 0;
    bool done = true;
    AnimCompute(g_animKind.load(), g_animQpc0.load(), a, dy, done);
    POINT pt{g_x, g_y + dy};
    SIZE sz{g_w, g_h};
    POINT src{0, 0};
    BLENDFUNCTION bf{AC_SRC_OVER, 0, a, AC_SRC_ALPHA};
    if (!UpdateLayeredWindow(g_hwnd, nullptr, &pt, &sz, g_so.dc, &src, 0, &bf, ULW_ALPHA))
        LOGE(L"[paint] UpdateLayeredWindow failed: %u", GetLastError());
}

static void Repaint() {
    if (!g_hwnd || !g_shown || g_w < 50 || g_h < 50 || g_quit) return;
    if (!EnsureSurfaces()) return;
    EnsureFonts();
    if (g_mode == 0) {
        PlaceUnplaced();
        FixOverlaps();
    }
    bool dark = EffectiveDark();
    BuildBackground(dark);
    SnapshotIcons();
    if (g_mode == 0 && g_cfg.grid) {
        Lay L = MakeLay();
        bool changed = !g_dragging && PlaceGridCells(L.cols);
        if (g_dragging && g_dragFrom >= 0 && g_dragFrom < (int)g_items.size()) {
            // the dragged tile follows the cursor; the tile on the cell under it moves aside
            g_dropCell = DropCell(L, g_dragPt.x, g_dragPt.y);
            BuildPreview(L, g_dropCell);
            g_ghost = POINT{g_dragPt.x - (int)g_grab.x - (int)L.content.left,
                            g_dragPt.y - (int)g_grab.y - (int)L.content.top + g_scroll};
        }
        if (TilesMoving(L)) {
            if (!g_frameTimer) g_frameTimer = SetTimer(g_hwnd, kTimerFrame, 15, nullptr) != 0;
        } else if (!g_dragging) {
            // the cells become the free positions too, so turning the grid off keeps this layout
            for (int i = 0; i < (int)g_items.size(); i++) {
                POINT p = CellPos(L, CellOf(i));
                int lx = MulDiv(p.x + std::max(0, L.cellW - TileWidth()) / 2, 96, g_dpi);
                int ly = MulDiv(p.y, 96, g_dpi);
                if (g_items[i].lx != lx || g_items[i].ly != ly) {
                    g_items[i].lx = lx;
                    g_items[i].ly = ly;
                    changed = true;
                }
            }
        }
        if (changed) SaveItems();
    } else if (g_mode == 0 && g_dragging && g_dragFrom >= 0 && g_dragFrom < (int)g_items.size()) {
        // where the dragged tile would land; searched once here, both layers draw it
        Lay L = MakeLay();
        int wantedX = g_dragPt.x - (int)g_grab.x - (int)L.content.left;
        int wantedY = g_dragPt.y - (int)g_grab.y - (int)L.content.top + g_scroll;
        int fx = wantedX, fy = wantedY;
        FindFreeTilePos(L, g_dragFrom, wantedX, wantedY, fx, fy);
        g_ghost = POINT{fx, fy};
    }
    memset(g_sb.px, 0x00, (size_t)g_w * g_h * 4);
    memset(g_sw.px, 0xFF, (size_t)g_w * g_h * 4);
    DrawLayer(g_sb.dc, dark);
    DrawLayer(g_sw.dc, dark);
    GdiFlush();
    Composite();
    Present();
}

////////////////////////////////////////////////////////////////////////////////
// Launching, file dialog and the list of installed programs (worker threads)
////////////////////////////////////////////////////////////////////////////////
struct LaunchJob {
    std::wstring name, path, mode;
};

static DWORD WINAPI LaunchProc(LPVOID p) {
    std::unique_ptr<LaunchJob> j(static_cast<LaunchJob*>(p));
    HRESULT co = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    std::wstring file = j->path, params;
    PCWSTR verb = nullptr;
    if (j->mode == L"locate") {
        params = L"/select,\"" + j->path + L"\"";
        file = L"explorer.exe";
    } else if (j->mode == L"runas") {
        verb = L"runas";
    }
    SHELLEXECUTEINFOW sei{};
    sei.cbSize = sizeof(sei);
    sei.fMask = SEE_MASK_FLAG_NO_UI | SEE_MASK_NOASYNC;
    sei.lpVerb = verb;
    sei.lpFile = file.c_str();
    sei.lpParameters = params.empty() ? nullptr : params.c_str();
    sei.nShow = SW_SHOWNORMAL;
    if (ShellExecuteExW(&sei))
        LOGI(L"[launch] started '%s' (%s)", j->name.c_str(), j->mode.c_str());
    else
        LOGE(L"[launch] ShellExecuteEx failed for '%s': %u", file.c_str(), GetLastError());
    if (SUCCEEDED(co)) CoUninitialize();
    j.reset();
    g_jobs.fetch_sub(1);
    return 0;
}

static void Launch(Item const& it, PCWSTR mode) {
    if (g_quit || g_jobs.load() >= 32) return;
    g_jobs.fetch_add(1);
    auto* j = new LaunchJob{it.name, it.path, mode};
    if (!QueueUserWorkItem(LaunchProc, j, WT_EXECUTELONGFUNCTION)) {
        delete j;
        g_jobs.fetch_sub(1);
    }
}

static DWORD WINAPI PickProc(LPVOID) {
    HRESULT co = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    auto* out = new std::vector<Item>();
    IFileOpenDialog* dlg = nullptr;
    HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&dlg));
    if (SUCCEEDED(hr) && dlg) {
        DWORD opts = 0;
        dlg->GetOptions(&opts);
        opts |= FOS_ALLOWMULTISELECT | FOS_DONTADDTORECENT | FOS_NODEREFERENCELINKS | FOS_FORCEFILESYSTEM |
                FOS_FILEMUSTEXIST;  // keep .lnk files as they are
        dlg->SetOptions(opts);
        COMDLG_FILTERSPEC fs[] = {{Tr(T_FilterPrograms), L"*.lnk;*.exe;*.url;*.appref-ms"}, {Tr(T_FilterAll), L"*.*"}};
        dlg->SetFileTypes(2, fs);
        dlg->SetTitle(Tr(T_DialogTitle));
        HRESULT sh = dlg->Show(nullptr);
        if (SUCCEEDED(sh)) {
            IShellItemArray* arr = nullptr;
            if (SUCCEEDED(dlg->GetResults(&arr)) && arr) {
                DWORD n = 0;
                arr->GetCount(&n);
                for (DWORD i = 0; i < n && out->size() < kMaxItems; i++) {
                    IShellItem* si = nullptr;
                    if (FAILED(arr->GetItemAt(i, &si)) || !si) continue;
                    PWSTR pth = nullptr;
                    if (SUCCEEDED(si->GetDisplayName(SIGDN_FILESYSPATH, &pth)) && pth) {
                        Item it;
                        it.name = FileStem(pth);
                        it.path = pth;
                        out->push_back(std::move(it));
                        CoTaskMemFree(pth);
                    }
                    si->Release();
                }
                arr->Release();
            }
        } else if (sh != HRESULT_FROM_WIN32(ERROR_CANCELLED)) {
            LOGE(L"[picker] dialog failed: 0x%08X", (unsigned)sh);
        }
        dlg->Release();
    } else {
        LOGE(L"[picker] cannot create the file dialog: 0x%08X", (unsigned)hr);
    }
    if (SUCCEEDED(co)) CoUninitialize();
    HWND h = g_hwnd;
    if (out->empty() || g_quit || !h || !PostMessageW(h, WM_APP_ADD, 0, reinterpret_cast<LPARAM>(out))) delete out;
    g_pickerBusy = false;
    g_jobs.fetch_sub(1);
    return 0;
}

static void StartFilePicker() {
    if (g_pickerBusy.exchange(true)) return;
    g_jobs.fetch_add(1);
    if (g_quit || !QueueUserWorkItem(PickProc, nullptr, WT_EXECUTELONGFUNCTION)) {
        g_jobs.fetch_sub(1);
        g_pickerBusy = false;
    }
}

// Lists the same programs as "All apps" in Start (the virtual shell:AppsFolder).
static DWORD WINAPI AppsProc(LPVOID) {
    HRESULT co = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    auto* out = new std::vector<Item>();
    IShellItem* folder = nullptr;
    HRESULT hr = SHCreateItemFromParsingName(L"shell:AppsFolder", nullptr, IID_PPV_ARGS(&folder));
    if (SUCCEEDED(hr) && folder) {
        IEnumShellItems* en = nullptr;
        if (SUCCEEDED(folder->BindToHandler(nullptr, BHID_EnumItems, IID_PPV_ARGS(&en))) && en) {
            IShellItem* si = nullptr;
            ULONG got = 0;
            while (!g_quit && out->size() < 3000 && en->Next(1, &si, &got) == S_OK && si) {
                PWSTR rel = nullptr, disp = nullptr;
                if (SUCCEEDED(si->GetDisplayName(SIGDN_PARENTRELATIVEPARSING, &rel)) && rel &&
                    SUCCEEDED(si->GetDisplayName(SIGDN_NORMALDISPLAY, &disp)) && disp && *disp && *rel) {
                    Item it;
                    it.name = Clean(StripLnk(disp));
                    it.path = std::wstring(L"shell:AppsFolder\\") + rel;
                    out->push_back(std::move(it));
                }
                if (rel) CoTaskMemFree(rel);
                if (disp) CoTaskMemFree(disp);
                si->Release();
                si = nullptr;
            }
            en->Release();
        } else {
            LOGE(L"[apps] cannot enumerate shell:AppsFolder");
        }
        folder->Release();
    } else {
        LOGE(L"[apps] cannot open shell:AppsFolder: 0x%08X", (unsigned)hr);
    }
    std::sort(out->begin(), out->end(), [](Item const& a, Item const& b) {
        return CompareStringOrdinal(a.name.c_str(), -1, b.name.c_str(), -1, TRUE) == CSTR_LESS_THAN;
    });
    out->erase(std::unique(out->begin(), out->end(),
                           [](Item const& a, Item const& b) { return _wcsicmp(a.path.c_str(), b.path.c_str()) == 0; }),
               out->end());
    LOGI(L"[apps] %zu program(s) found", out->size());
    if (SUCCEEDED(co)) CoUninitialize();
    HWND h = g_hwnd;
    if (g_quit || !h || !PostMessageW(h, WM_APP_APPS, 0, reinterpret_cast<LPARAM>(out))) delete out;
    g_appsLoading = false;
    g_jobs.fetch_sub(1);
    return 0;
}

static void StartAppsLoad() {
    if (g_appsLoading.exchange(true)) return;
    g_jobs.fetch_add(1);
    if (g_quit || !QueueUserWorkItem(AppsProc, nullptr, WT_EXECUTELONGFUNCTION)) {
        g_jobs.fetch_sub(1);
        g_appsLoading = false;
    }
}

////////////////////////////////////////////////////////////////////////////////
// Item operations (window thread)
////////////////////////////////////////////////////////////////////////////////
static void AddItems(std::vector<Item> const& items) {
    bool changed = false;
    for (auto const& raw : items) {
        if (g_items.size() >= kMaxItems) break;
        Item it;
        it.name = Clean(StripLnk(raw.name));
        it.path = Clean(raw.path);
        if (it.path.empty()) continue;
        if (IsShellPath(it.path)) {
            if (it.path.size() <= 6) continue;
        } else if (LooksLikeFsPath(it.path)) {
            DWORD at = GetFileAttributesW(it.path.c_str());
            if (at == INVALID_FILE_ATTRIBUTES || (at & FILE_ATTRIBUTE_DIRECTORY)) {
                LOGE(L"[add] skipped (not a file): %s", it.path.c_str());
                continue;
            }
        }
        if (it.name.empty()) it.name = Clean(FileStem(it.path));
        if (FindByPath(it.path) >= 0) continue;
        LOGI(L"[add] '%s' -> %s", it.name.c_str(), it.path.c_str());
        g_items.push_back(std::move(it));
        changed = true;
    }
    if (changed) {
        SaveItems();
        Repaint();
    }
}

static void RemoveAt(int idx) {
    if (idx < 0 || idx >= (int)g_items.size()) return;
    g_items.erase(g_items.begin() + idx);
    if (idx < (int)g_tilePos.size()) g_tilePos.erase(g_tilePos.begin() + idx);
    g_hover = -1;
    g_hoverRemove = false;
    SaveItems();
    Repaint();
}

static void ArrangeInGrid() {
    for (auto& it : g_items) it.lx = it.ly = -1;
    g_scroll = 0;
    PlaceUnplaced();
    Repaint();
}

static void TogglePickerRow(int row) {
    if (row < 0 || row >= (int)g_view.size()) return;
    Item const it = PickerItem(row);
    int idx = FindByPath(it.path);
    if (idx >= 0) {
        RemoveAt(idx);
    } else {
        AddItems(std::vector<Item>{it});
        Repaint();
    }
}

// While the picker is open the keyboard focus stays in Start's search box (taking it would close
// Start), so typed keys are caught by a low-level hook and kept away from Start. Only text,
// Backspace, Enter and Esc (while there is text to clear) are taken; Win, Alt and Ctrl shortcuts,
// Tab, arrows and the rest pass through. The hook exists only while the picker is shown.
static bool g_keyTaken[256];  // keys whose key-down was taken: their key-up is taken too

static LRESULT CALLBACK PickerKeyProc(int code, WPARAM wp, LPARAM lp) {
    if (code != HC_ACTION || !g_hwnd) return CallNextHookEx(nullptr, code, wp, lp);
    auto const* k = reinterpret_cast<KBDLLHOOKSTRUCT const*>(lp);
    const DWORD vk = k->vkCode & 0xFF;
    if (wp == WM_KEYUP || wp == WM_SYSKEYUP) {
        if (!g_keyTaken[vk]) return CallNextHookEx(nullptr, code, wp, lp);
        g_keyTaken[vk] = false;
        return 1;
    }
    if (g_mode != 1 || !g_shown || g_hiding || g_menuOpen) return CallNextHookEx(nullptr, code, wp, lp);
    auto held = [](int v) { return (GetAsyncKeyState(v) & 0x8000) != 0; };
    const bool ctrl = held(VK_CONTROL), alt = (k->flags & LLKHF_ALTDOWN) != 0;
    if (held(VK_LWIN) || held(VK_RWIN) || ctrl != alt)  // a shortcut; Ctrl+Alt is AltGr, which types text
        return CallNextHookEx(nullptr, code, wp, lp);
    WCHAR ch = 0;
    if (vk == VK_BACK) {
        ch = L'\b';
    } else if (vk == VK_RETURN) {
        ch = L'\r';
    } else if (vk == VK_ESCAPE) {
        ch = g_query.empty() ? 0 : 0x1B;  // with nothing typed, Esc closes Start as usual
    } else if (vk == VK_PACKET) {
        ch = (WCHAR)k->scanCode;  // text sent as Unicode (on-screen keyboards, input tools)
    } else {
        BYTE ks[256] = {};
        if (held(VK_SHIFT)) ks[VK_SHIFT] = 0x80;
        if (ctrl) ks[VK_CONTROL] = ks[VK_MENU] = 0x80;
        if (GetKeyState(VK_CAPITAL) & 1) ks[VK_CAPITAL] = 1;
        HWND fg = GetForegroundWindow();  // the layout the user is typing in (uk, en, ru...)
        HKL hkl = GetKeyboardLayout(fg ? GetWindowThreadProcessId(fg, nullptr) : 0);
        WCHAR buf[4];
        if (ToUnicodeEx(vk, k->scanCode, ks, buf, 4, 0x4 /* keep dead-key state */, hkl) == 1 && buf[0] >= 0x20 &&
            buf[0] != 0x7F)
            ch = buf[0];
    }
    if (!ch) return CallNextHookEx(nullptr, code, wp, lp);
    g_keyTaken[vk] = true;
    PostMessageW(g_hwnd, WM_APP_KEY, ch, 0);
    return 1;
}

static void StartPickerKeys() {
    if (g_keyHook) return;
    HMODULE self = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       reinterpret_cast<PCWSTR>(&PickerKeyProc), &self);
    g_keyHook = SetWindowsHookExW(WH_KEYBOARD_LL, PickerKeyProc, self, 0);
    if (!g_keyHook) LOGE(L"[search] keyboard hook failed: %u", GetLastError());
}

static void StopPickerKeys() {
    if (g_keyHook) UnhookWindowsHookEx(g_keyHook);
    g_keyHook = nullptr;
    memset(g_keyTaken, 0, sizeof(g_keyTaken));
    g_query.clear();
}

static void PickerKey(WCHAR ch) {
    if (g_mode != 1) return;
    if (ch == L'\r') {  // Enter adds (or removes) the best match
        TogglePickerRow(0);
        return;
    }
    if (ch == L'\b') {
        if (g_query.empty()) return;
        g_query.pop_back();
    } else if (ch == 0x1B) {
        g_query.clear();
    } else if (g_query.size() < 64) {
        g_query += ch;
    } else {
        return;
    }
    g_pscroll = 0;
    g_hover = -1;
    RebuildView();
    Repaint();
}

// Dragging a desktop icon onto the card while the picker is open. A press on the desktop would
// activate it and Start would close (Start closes when it loses the focus), taking the card along.
// While the picker is open the desktop's icon list view is subclassed: a left press does not
// activate the desktop, so Start stays open and the drag can reach the card. A press that ends
// without moving (a plain click) activates the desktop afterwards, so clicking off still closes
// Start. The subclass procedure runs on the desktop's thread, inside this module.
static HWND g_deskLv = nullptr;            // window thread
static WNDPROC g_deskOrig = nullptr;
static std::atomic<bool> g_deskHold{false};
static std::atomic<int> g_deskInProc{0};   // DeskProc frames on the desktop thread's stack
static POINT g_deskDownPt{};               // desktop thread only
static bool g_deskPressed = false;         // desktop thread only

static BOOL CALLBACK FindDeskDefView(HWND top, LPARAM lp) {
    HWND dv = FindWindowExW(top, nullptr, L"SHELLDLL_DefView", nullptr);
    if (!dv) return TRUE;
    *reinterpret_cast<HWND*>(lp) = FindWindowExW(dv, nullptr, L"SysListView32", nullptr);
    return FALSE;
}

static void DeskPressEnded() {  // desktop thread: decide between a click and a drag
    POINT p;
    GetCursorPos(&p);
    bool moved = std::abs(p.x - g_deskDownPt.x) > GetSystemMetrics(SM_CXDRAG) ||
                 std::abs(p.y - g_deskDownPt.y) > GetSystemMetrics(SM_CYDRAG);
    HWND h = g_hwnd;
    if (!moved && h) PostMessageW(h, WM_APP_DESKCLICK, 0, 0);
}

static LRESULT CALLBACK DeskProc(HWND h, UINT m, WPARAM w, LPARAM l) {
    g_deskInProc++;
    WNDPROC orig = g_deskOrig;
    LRESULT r;
    if (g_deskHold && m == WM_MOUSEACTIVATE && HIWORD(l) == WM_LBUTTONDOWN) {
        r = MA_NOACTIVATE;  // the click still reaches the list view
    } else if (g_deskHold && m == WM_LBUTTONDOWN) {
        GetCursorPos(&g_deskDownPt);
        g_deskPressed = true;
        r = CallWindowProcW(orig, h, m, w, l);  // may run the whole drag and drop before returning
        if (g_deskPressed && !(GetAsyncKeyState(VK_LBUTTON) & 0x8000)) {
            g_deskPressed = false;  // the button-up was consumed inside (drag detection or the drag itself)
            DeskPressEnded();
        }
    } else if (m == WM_LBUTTONUP && g_deskPressed) {
        g_deskPressed = false;
        r = CallWindowProcW(orig, h, m, w, l);
        if (g_deskHold) DeskPressEnded();
    } else {
        r = CallWindowProcW(orig, h, m, w, l);
    }
    g_deskInProc--;
    return r;
}

static void StartDeskHold() {
    if (g_deskLv) {
        g_deskHold = true;
        return;
    }
    HWND lv = nullptr;
    EnumWindows(FindDeskDefView, reinterpret_cast<LPARAM>(&lv));
    DWORD pid = 0;
    if (!lv || !GetWindowThreadProcessId(lv, &pid) || pid != GetCurrentProcessId()) return;
    g_deskOrig = reinterpret_cast<WNDPROC>(GetWindowLongPtrW(lv, GWLP_WNDPROC));
    if (!g_deskOrig) return;
    g_deskHold = true;
    if (!SetWindowLongPtrW(lv, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&DeskProc))) {
        g_deskHold = false;
        return;
    }
    g_deskLv = lv;
}

static void StopDeskHold() {
    g_deskHold = false;
    if (!g_deskLv) return;
    if (IsWindow(g_deskLv)) {
        if (GetWindowLongPtrW(g_deskLv, GWLP_WNDPROC) == reinterpret_cast<LONG_PTR>(&DeskProc)) {
            SetWindowLongPtrW(g_deskLv, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(g_deskOrig));
        } else {
            // someone subclassed the list view after us: our procedure stays in their chain (as a
            // pass-through now), so this module must stay loaded
            PinModule(L"the desktop list view was subclassed after this mod");
        }
    }
    if (g_deskInProc.load() > 0) PinModule(L"a desktop drag was still running");
    g_deskLv = nullptr;
}

static void DeskClicked() {  // a plain click on the desktop while the picker was open: close Start
    if (g_mode != 1 || !g_shown || !g_deskLv) return;
    SetForegroundWindow(GetAncestor(g_deskLv, GA_ROOT));
}

static void OpenPicker() {
    g_mode = 1;
    g_pscroll = 0;
    g_hover = -1;
    g_hoverRemove = false;
    g_query.clear();
    RebuildView();
    StartPickerKeys();
    StartDeskHold();
    StartAppsLoad();
    Repaint();
}

static void ClosePicker() {
    g_mode = 0;
    g_hover = -1;
    g_hoverRemove = false;
    StopPickerKeys();
    StopDeskHold();
    KeepOnlyNeededIcons(MakeLay(), false);  // the program list's icons are not needed any more
    Repaint();
}

static void EndDrag() {
    g_dragging = false;
    g_dragFrom = -1;
    g_dropCell = POINT{-1, -1};
    g_preview.clear();
    g_down = -1;
    g_pressRemove = false;
    if (g_hwnd) KillTimer(g_hwnd, kTimerScroll);
    if (g_hwnd && GetCapture() == g_hwnd) ReleaseCapture();
}

// Where a dragged tile lands with free placement: where it is dropped, except that each direction
// snaps to a value a few pixels away that lines it up with another tile (same row or column, or
// right beside it), so a tile goes back into a line without pixel-exact aiming. If that spot is
// taken, the nearest free lined-up spot is used; tiles never cover each other (see TilesCollide).
static void FindFreeTilePos(Lay const& L, int moving, int wantedX, int wantedY, int& outX, int& outY) {
    const int cw = (int)(L.content.right - L.content.left);
    const int maxX = std::max(0, cw - L.tileW);
    const int maxY = std::max(wantedY, L.totalH + L.tileH);
    const int snap = S(10);

    wantedX = std::max(0, std::min(wantedX, maxX));
    wantedY = std::max(0, wantedY);

    std::vector<POINT> others;
    for (int i = 0; i < (int)g_items.size(); ++i)
        if (i != moving)
            others.push_back(POINT{std::min(S(std::max(0, g_items[i].lx)), maxX), S(std::max(0, g_items[i].ly))});
    auto freeAt = [&](int x, int y) {
        for (POINT const& o : others)
            if (TilesCollide(L, x, y, o.x, o.y)) return false;
        return true;
    };

    // every x and y that lines the tile up with another one (or with the card's edges)
    std::vector<int> xs{0, maxX}, ys{0};
    for (POINT const& o : others) {
        for (int x : {o.x - L.tileW, o.x, o.x + L.tileW})
            if (x >= 0 && x <= maxX) xs.push_back(x);
        for (int y : {o.y - L.tileH, o.y, o.y + L.tileH})
            if (y >= 0 && y <= maxY) ys.push_back(y);
    }
    // each direction snaps on its own to the closest lined-up value a few pixels away
    auto snapTo = [&](std::vector<int> const& vs, int v) {
        int best = v, bestD = snap + 1;
        for (int c : vs)
            if (std::abs(c - v) < bestD) {
                best = c;
                bestD = std::abs(c - v);
            }
        return best;
    };
    wantedX = snapTo(xs, wantedX);
    wantedY = snapTo(ys, wantedY);
    if (freeAt(wantedX, wantedY)) {
        outX = wantedX;
        outY = wantedY;
        return;
    }
    xs.push_back(wantedX);
    ys.push_back(wantedY);
    std::sort(xs.begin(), xs.end());
    xs.erase(std::unique(xs.begin(), xs.end()), xs.end());
    std::sort(ys.begin(), ys.end());
    ys.erase(std::unique(ys.begin(), ys.end()), ys.end());

    struct Cand {
        long long d;
        int x, y;
    };
    std::vector<Cand> cands;
    for (int x : xs)
        for (int y : ys) {
            long long dx = x - wantedX, dy = y - wantedY;
            cands.push_back(Cand{dx * dx + dy * dy, x, y});
        }
    std::sort(cands.begin(), cands.end(), [](Cand const& a, Cand const& b) { return a.d < b.d; });
    for (Cand const& c : cands) {  // nearest free lined-up spot; below the lowest tile always is one
        if (freeAt(c.x, c.y)) {
            outX = c.x;
            outY = c.y;
            return;
        }
    }
    outX = wantedX;
    outY = std::max(maxY, wantedY);
}

static void CommitDrag(int x, int y) {
    int from = g_dragFrom;
    if (from < 0 || from >= (int)g_items.size()) return;

    Lay L = MakeLay();
    int px = x - (int)g_grab.x - (int)L.content.left;
    int py = y - (int)g_grab.y - (int)L.content.top + g_scroll;
    if (g_cfg.grid) {  // every tile takes its previewed cell; the dragged one slides in from the cursor
        BuildPreview(L, DropCell(L, x, y));
        for (int i = 0; i < (int)g_items.size(); i++) {
            g_items[i].gc = g_preview[i].x;
            g_items[i].gr = g_preview[i].y;
        }
        if (from < (int)g_tilePos.size()) g_tilePos[from] = POINT{px, py};
        g_hover = -1;
        g_hoverRemove = false;
        SaveItems();
        return;
    }

    int freeX = px, freeY = py;
    FindFreeTilePos(L, from, px, py, freeX, freeY);

    Item it = std::move(g_items[from]);
    it.lx = MulDiv(freeX, 96, g_dpi);
    it.ly = MulDiv(freeY, 96, g_dpi);
    g_items.erase(g_items.begin() + from);
    g_items.push_back(std::move(it));
    g_hover = -1;
    g_hoverRemove = false;
    SaveItems();
}

static void ShowItemMenu(int idx) {
    if (idx < 0 || idx >= (int)g_items.size()) return;
    Item it = g_items[idx];
    bool shell = IsShellPath(it.path);
    HMENU m = CreatePopupMenu();
    AppendMenuW(m, MF_STRING, 1, Tr(T_MenuOpen));
    if (!shell) {
        AppendMenuW(m, MF_STRING, 2, Tr(T_MenuLocate));
        AppendMenuW(m, MF_STRING, 3, Tr(T_MenuRunAs));
    }
    if (!g_cfg.grid) {
        AppendMenuW(m, MF_SEPARATOR, 0, nullptr);
        AppendMenuW(m, MF_STRING, 4, Tr(T_MenuArrange));
    }
    AppendMenuW(m, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(m, MF_STRING, 6, Tr(T_MenuRemove));
    POINT pt;
    GetCursorPos(&pt);
    g_menuOpen = true;
    int cmd = (int)TrackPopupMenuEx(m, TPM_RETURNCMD | TPM_NONOTIFY | TPM_LEFTALIGN | TPM_TOPALIGN, pt.x, pt.y,
                                    g_hwnd, nullptr);
    g_menuOpen = false;
    DestroyMenu(m);
    int cur = FindByPath(it.path);  // the list may have changed while the menu was open
    if (cmd == 1) Launch(it, L"open");
    else if (cmd == 2) Launch(it, L"locate");
    else if (cmd == 3) Launch(it, L"runas");
    else if (cmd == 4) ArrangeInGrid();
    else if (cur >= 0 && cmd == 6) RemoveAt(cur);
    else Repaint();
}

static void ShowCardMenu() {
    HMENU m = CreatePopupMenu();
    AppendMenuW(m, MF_STRING, 1, Tr(T_MenuAdd));
    if (!g_cfg.grid) AppendMenuW(m, MF_STRING, 2, Tr(T_MenuArrange));
    POINT pt;
    GetCursorPos(&pt);
    g_menuOpen = true;
    int cmd = (int)TrackPopupMenuEx(m, TPM_RETURNCMD | TPM_NONOTIFY | TPM_LEFTALIGN | TPM_TOPALIGN, pt.x, pt.y,
                                    g_hwnd, nullptr);
    g_menuOpen = false;
    DestroyMenu(m);
    if (cmd == 1) OpenPicker();
    else if (cmd == 2) ArrangeInGrid();
}

////////////////////////////////////////////////////////////////////////////////
// Drop target: files from Explorer and programs dragged from the Start menu
////////////////////////////////////////////////////////////////////////////////
static void LogFormats(IDataObject* d, DWORD allowed) {
    LOGE(L"[drop] allowed effects=0x%lX; formats offered by the source:", (unsigned long)allowed);
    IEnumFORMATETC* en = nullptr;
    if (FAILED(d->EnumFormatEtc(DATADIR_GET, &en)) || !en) {
        LOGE(L"[drop]   (cannot list the formats)");
        return;
    }
    FORMATETC fe{};
    ULONG got = 0;
    for (int n = 0; n < 40 && en->Next(1, &fe, &got) == S_OK; n++) {
        WCHAR nm[128] = L"";
        if (fe.cfFormat >= 0xC000) GetClipboardFormatNameW(fe.cfFormat, nm, ARRAYSIZE(nm));
        LOGE(L"[drop]   format %u '%s' tymed=0x%lX", (unsigned)fe.cfFormat, nm, (unsigned long)fe.tymed);
    }
    en->Release();
}

static bool ItemsFromDataObject(IDataObject* d, std::vector<Item>& out) {
    bool any = false;
    IShellItemArray* arr = nullptr;
    if (SUCCEEDED(SHCreateShellItemArrayFromDataObject(d, IID_PPV_ARGS(&arr))) && arr) {
        DWORD n = 0;
        arr->GetCount(&n);
        for (DWORD i = 0; i < n && out.size() < kMaxItems; i++) {
            IShellItem* si = nullptr;
            if (FAILED(arr->GetItemAt(i, &si)) || !si) continue;
            PWSTR fs = nullptr, parse = nullptr, disp = nullptr;
            std::wstring path, name;
            if (SUCCEEDED(si->GetDisplayName(SIGDN_FILESYSPATH, &fs)) && fs) path = fs;
            if (path.empty() && SUCCEEDED(si->GetDisplayName(SIGDN_DESKTOPABSOLUTEPARSING, &parse)) && parse)
                path = NormalizeShellPath(parse);
            if (SUCCEEDED(si->GetDisplayName(SIGDN_NORMALDISPLAY, &disp)) && disp) name = StripLnk(disp);
            if (fs) CoTaskMemFree(fs);
            if (parse) CoTaskMemFree(parse);
            if (disp) CoTaskMemFree(disp);
            si->Release();
            if (path.empty()) continue;
            if (name.empty()) name = FileStem(path);
            Item it;
            it.name = name;
            it.path = path;
            out.push_back(std::move(it));
            any = true;
        }
        arr->Release();
    }
    if (!any) {  // plain file list
        FORMATETC fe{CF_HDROP, nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};
        STGMEDIUM sm{};
        if (d->QueryGetData(&fe) == S_OK && SUCCEEDED(d->GetData(&fe, &sm))) {
            HDROP h = reinterpret_cast<HDROP>(sm.hGlobal);
            UINT n = DragQueryFileW(h, 0xFFFFFFFF, nullptr, 0);
            for (UINT i = 0; i < n && out.size() < kMaxItems; i++) {
                WCHAR buf[MAX_PATH * 2];
                if (DragQueryFileW(h, i, buf, ARRAYSIZE(buf))) {
                    Item it;
                    it.name = FileStem(buf);
                    it.path = buf;
                    out.push_back(std::move(it));
                    any = true;
                }
            }
            ReleaseStgMedium(&sm);
        }
    }
    return any;
}

class DropTarget final : public IDropTarget {
    LONG m_ref = 1;

    // Never answer MOVE: the source must not delete or unpin anything.
    static DWORD Pick(DWORD allowed) {
        if (allowed & DROPEFFECT_COPY) return DROPEFFECT_COPY;
        if (allowed & DROPEFFECT_LINK) return DROPEFFECT_LINK;
        return DROPEFFECT_NONE;
    }

   public:
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv) override {
        if (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IDropTarget)) {
            *ppv = static_cast<IDropTarget*>(this);
            AddRef();
            return S_OK;
        }
        *ppv = nullptr;
        return E_NOINTERFACE;
    }
    ULONG STDMETHODCALLTYPE AddRef() override { return InterlockedIncrement(&m_ref); }
    ULONG STDMETHODCALLTYPE Release() override {
        ULONG r = InterlockedDecrement(&m_ref);
        if (!r) delete this;
        return r;
    }
    HRESULT STDMETHODCALLTYPE DragEnter(IDataObject* d, DWORD, POINTL, DWORD* eff) override {
        if (g_cfg.diag && d) LogFormats(d, *eff);
        *eff = Pick(*eff);
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE DragOver(DWORD, POINTL, DWORD* eff) override {
        *eff = Pick(*eff);
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE DragLeave() override { return S_OK; }
    HRESULT STDMETHODCALLTYPE Drop(IDataObject* d, DWORD, POINTL, DWORD* eff) override {
        DWORD chosen = Pick(*eff);
        *eff = DROPEFFECT_NONE;
        std::vector<Item> items;
        if (d && ItemsFromDataObject(d, items) && !items.empty()) {
            LOGI(L"[drop] %zu item(s) accepted", items.size());
            AddItems(items);
            *eff = chosen;
        } else if (d) {
            LOGE(L"[drop] nothing usable in the dropped data (turn on the diagnostic log and send me the lines below)");
            LogFormats(d, chosen);
        }
        return S_OK;
    }
};

////////////////////////////////////////////////////////////////////////////////
// Finding the Start window from outside (we live in explorer.exe)
////////////////////////////////////////////////////////////////////////////////
static DWORD CloakValue(HWND h) {
    DWORD cloaked = 0;
    if (SUCCEEDED(DwmGetWindowAttribute(h, DWMWA_CLOAKED, &cloaked, sizeof(cloaked)))) return cloaked;
    return 0;
}

// Only the process that owns the taskbar hosts the card (not every Explorer window process).
static bool IsShellProcess() {
    static bool ok = false;
    if (ok) return true;
    HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (!tray) return false;
    DWORD pid = 0;
    GetWindowThreadProcessId(tray, &pid);
    ok = pid == GetCurrentProcessId();
    return ok;
}

// Is `pid` a running copy of `exe`? The answer per pid is cached (window thread only).
struct ExeMatcher {
    PCWSTR exe;
    DWORD goodPid = 0;
    std::set<DWORD> bad;

    bool Is(DWORD pid) {
        if (pid == goodPid) return true;
        if (bad.count(pid)) return false;
        bool ok = false;
        HANDLE p = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
        if (p) {
            WCHAR path[MAX_PATH];
            DWORD n = ARRAYSIZE(path);
            if (QueryFullProcessImageNameW(p, 0, path, &n)) {
                PCWSTR base = wcsrchr(path, L'\\');
                base = base ? base + 1 : path;
                ok = _wcsicmp(base, exe) == 0;
            }
            CloseHandle(p);
        }
        if (ok) {
            goodPid = pid;
        } else {
            if (bad.size() > 256) bad.clear();
            bad.insert(pid);
        }
        return ok;
    }
};
static ExeMatcher g_startExe{L"StartMenuExperienceHost.exe"};
static ExeMatcher g_searchExe{L"SearchHost.exe"};

// Several system windows share the CoreWindow class (Search, Widgets...), so the
// owner process must be StartMenuExperienceHost.exe.
static bool IsStartProcess(DWORD pid) { return g_startExe.Is(pid); }

// Who has the keyboard focus while Start is open. On current Windows 11 builds Start's
// search box, hosted by SearchHost.exe, takes the focus, not Start's own process.
static bool IsStartFocusProcess(DWORD pid) { return g_startExe.Is(pid) || g_searchExe.Is(pid); }

static HWND FindStartWindow() {
    HWND best = nullptr;
    long long bestArea = 0;
    HWND h = nullptr;
    while ((h = FindWindowExW(nullptr, h, g_cfg.startClass.c_str(), nullptr)) != nullptr) {
        DWORD pid = 0;
        GetWindowThreadProcessId(h, &pid);
        if (!IsStartProcess(pid)) continue;
        if (!IsWindowVisible(h) || CloakValue(h) != 0) continue;
        RECT r;
        if (!GetWindowRect(h, &r)) continue;
        if (!MonitorFromRect(&r, MONITOR_DEFAULTTONULL)) continue;
        long long area = (long long)(r.right - r.left) * (r.bottom - r.top);
        if (area > bestArea) {
            bestArea = area;
            best = h;
        }
    }
    return best;
}

////////////////////////////////////////////////////////////////////////////////
// Where is the visible Start panel?
// The Start window covers the whole work area, so its rectangle is useless. About
// 0.3 s after Start opens, Windows gives the window a region that matches the
// visible panel (this follows "Start Menu Size"). It is remembered, so the card
// is right from the first frame next time. Without a region: settings.
////////////////////////////////////////////////////////////////////////////////
static bool RegionBox(HWND h, RECT const& wr, RECT& out) {
    HRGN rgn = CreateRectRgn(0, 0, 0, 0);
    if (!rgn) return false;
    bool ok = false;
    int t = GetWindowRgn(h, rgn);
    if (t != ERROR && t != NULLREGION) {
        RECT b{};
        if (GetRgnBox(rgn, &b) != ERROR) {
            int bw = b.right - b.left, bh = b.bottom - b.top;
            if (bw >= 200 && bh >= 200 && (bw < (wr.right - wr.left) - 16 || bh < (wr.bottom - wr.top) - 16)) {
                out = RECT{wr.left + b.left, wr.top + b.top, wr.left + b.right, wr.top + b.bottom};
                ok = true;
            }
        }
    }
    DeleteObject(rgn);
    return ok;
}

static bool TaskbarCentered() {
    static ULONGLONG last = 0;
    static bool centered = true;
    ULONGLONG now = GetTickCount64();
    if (last == 0 || now - last > 2000) {
        DWORD v = 1, sz = sizeof(v);
        if (RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced",
                         L"TaskbarAl", RRF_RT_REG_DWORD, nullptr, &v, &sz) != ERROR_SUCCESS)
            v = 1;
        centered = v != 0;
        last = now;
    }
    return centered;
}

// Start slides out from behind the taskbar. Placing the card right under the taskbar of Start's
// monitor in the z-order (still topmost) makes the taskbar cover it the same way while it slides.
static HWND BelowTaskbar(RECT const& wr) {
    HMONITOR mon = MonitorFromRect(&wr, MONITOR_DEFAULTTONEAREST);
    for (PCWSTR cls : {L"Shell_TrayWnd", L"Shell_SecondaryTrayWnd"}) {
        HWND t = nullptr;
        while ((t = FindWindowExW(nullptr, t, cls, nullptr)) != nullptr)
            if (MonitorFromWindow(t, MONITOR_DEFAULTTONULL) == mon) return t;
    }
    return HWND_TOPMOST;
}

static RECT ManualFlyout(RECT const& wr) {
    int ww = wr.right - wr.left, wh = wr.bottom - wr.top;
    int W = std::min(S(g_cfg.startW), ww), H = std::min(S(g_cfg.startH), wh);
    int gapB = S(g_cfg.startBottom);
    bool centered = g_cfg.align == 1 || (g_cfg.align == 0 && TaskbarCentered());
    int left = centered ? wr.left + (ww - W) / 2 : wr.left + gapB;
    int bottom = wr.bottom - gapB;
    return RECT{left, bottom - H, left + W, bottom};
}

// Colour of the Start edge, row by row. A thin vertical strip just inside the Start
// edge next to the card is read from the screen (this includes whatever Windows 11
// Start Menu Styler did to Start's colour and transparency). The rows are reduced
// to kProf buckets that follow the card's height; stray pixels are rejected.
static void SampleStyle(RECT const& fly, int cardTop, int cardBottom) {
    const int H = cardBottom - cardTop;
    if (H < 60) return;
    int y0 = std::max(cardTop, (int)fly.top + S(12));
    int y1 = std::min(cardBottom, (int)fly.bottom - S(12));
    int n = y1 - y0;
    if (n < 40) return;
    int sw = std::max(3, S(8));
    // The card's actual side: it moves to the other one when the configured side has no room.
    const bool cardLeft = g_x + g_w / 2 < (fly.left + fly.right) / 2;
    int x0 = cardLeft ? (int)fly.left + S(5) : (int)fly.right - S(5) - sw;
    BITMAPINFO bi;
    FillBitmapInfo(bi, sw, n);
    HDC scr = GetDC(nullptr);
    HDC mem = scr ? CreateCompatibleDC(scr) : nullptr;
    void* bits = nullptr;
    HBITMAP dib = mem ? CreateDIBSection(mem, &bi, DIB_RGB_COLORS, &bits, nullptr, 0) : nullptr;
    bool ok = false;
    std::vector<uint32_t> rowCol((size_t)n);
    std::vector<int> rowLum((size_t)n);
    if (mem && dib && bits) {
        HGDIOBJ old = SelectObject(mem, dib);
        if (BitBlt(mem, 0, 0, sw, n, scr, x0, y0, SRCCOPY)) {
            GdiFlush();
            auto* px = static_cast<uint32_t*>(bits);
            std::vector<std::pair<int, uint32_t>> v((size_t)sw);
            for (int y = 0; y < n; y++) {
                for (int x = 0; x < sw; x++) {
                    uint32_t c = px[(size_t)y * sw + x] & 0x00FFFFFFu;
                    v[x] = {ProfR(c) * 299 + ProfG(c) * 587 + ProfB(c) * 114, c};
                }
                std::sort(v.begin(), v.end());
                rowCol[y] = v[sw / 2].second;
                rowLum[y] = v[sw / 2].first;
            }
            ok = true;
        }
        SelectObject(mem, old);
    }
    if (dib) DeleteObject(dib);
    if (mem) DeleteDC(mem);
    if (scr) ReleaseDC(nullptr, scr);
    if (!ok) return;

    std::vector<int> sorted = rowLum;
    std::nth_element(sorted.begin(), sorted.begin() + sorted.size() / 2, sorted.end());
    const int gLum = sorted[sorted.size() / 2];
    uint32_t gCol = rowCol[0];
    for (int y = 0; y < n; y++)
        if (rowLum[y] == gLum) {
            gCol = rowCol[y];
            break;
        }

    // buckets over the card's rows [cardTop, cardBottom)
    long sr[kProf] = {}, sg[kProf] = {}, sb[kProf] = {}, cnt[kProf] = {};
    for (int y = 0; y < n; y++) {
        if (std::abs(rowLum[y] - gLum) > 40000) continue;  // text, icons, anything that is not the panel
        int cardRow = (y0 + y) - cardTop;
        int k = std::min(kProf - 1, std::max(0, cardRow * kProf / H));
        sr[k] += ProfR(rowCol[y]);
        sg[k] += ProfG(rowCol[y]);
        sb[k] += ProfB(rowCol[y]);
        cnt[k]++;
    }
    Prof np;
    bool have[kProf];
    for (int k = 0; k < kProf; k++) {
        have[k] = cnt[k] > 0;
        np.c[k] = have[k] ? (uint32_t)(((sr[k] / cnt[k]) << 16) | ((sg[k] / cnt[k]) << 8) | (sb[k] / cnt[k])) : 0;
    }
    for (int k = 0; k < kProf; k++) {  // empty buckets take the nearest filled one
        if (have[k]) continue;
        int best = -1;
        for (int d = 1; d < kProf && best < 0; d++) {
            if (k - d >= 0 && have[k - d]) best = k - d;
            else if (k + d < kProf && have[k + d]) best = k + d;
        }
        np.c[k] = best >= 0 ? np.c[best] : gCol;
    }
    np.valid = true;

    bool differs = !g_prof.valid;
    for (int k = 0; k < kProf && !differs; k++) {
        if (std::abs(ProfR(np.c[k]) - ProfR(g_prof.c[k])) > 4 || std::abs(ProfG(np.c[k]) - ProfG(g_prof.c[k])) > 4 ||
            std::abs(ProfB(np.c[k]) - ProfB(g_prof.c[k])) > 4)
            differs = true;
    }
    LOGI(L"[style] edge x=%d, rows %d, top #%06X middle #%06X bottom #%06X%s", x0, n, np.c[0], np.c[kProf / 2],
         np.c[kProf - 1], differs ? L" (applied)" : L" (unchanged)");
    if (!differs) return;
    g_prof = np;
    SaveGeom();
    Repaint();
}

////////////////////////////////////////////////////////////////////////////////
// Diagnostics
////////////////////////////////////////////////////////////////////////////////
static BOOL CALLBACK EnumChildDump(HWND h, LPARAM) {
    if (g_dumpLogs >= 60) return FALSE;
    WCHAR cls[128] = L"";
    GetClassNameW(h, cls, ARRAYSIZE(cls));
    RECT r{};
    GetWindowRect(h, &r);
    g_dumpLogs++;
    LOGE(L"[start-child] hwnd=%p class=%s visible=%d rect=(%ld,%ld,%ld,%ld) %ldx%ld", h, cls, IsWindowVisible(h) ? 1 : 0,
         r.left, r.top, r.right, r.bottom, r.right - r.left, r.bottom - r.top);
    return TRUE;
}

static void DumpStart(HWND start) {
    if (!g_cfg.diag) return;
    RECT wr{};
    GetWindowRect(start, &wr);
    HRGN rgn = CreateRectRgn(0, 0, 0, 0);
    int t = rgn ? GetWindowRgn(start, rgn) : ERROR;
    RECT b{};
    if (rgn && t != ERROR && t != NULLREGION) GetRgnBox(rgn, &b);
    LOGE(L"[start] hwnd=%p rect=(%ld,%ld,%ld,%ld) windowRgnType=%d regionBox=(%ld,%ld,%ld,%ld) cloakedValue=%lu", start,
         wr.left, wr.top, wr.right, wr.bottom, t, b.left, b.top, b.right, b.bottom, CloakValue(start));
    if (rgn) DeleteObject(rgn);
    g_dumpLogs = 0;
    EnumChildWindows(start, EnumChildDump, 0);
}

static void LogCardState() {
    if (!g_hwnd) return;
    POINT c{g_x + g_w / 2, g_y + g_h / 2};
    HWND at = WindowFromPoint(c);
    WCHAR cls[128] = L"";
    DWORD pid = 0;
    if (at) {
        GetClassNameW(at, cls, ARRAYSIZE(cls));
        GetWindowThreadProcessId(at, &pid);
    }
    LOGE(L"[card] visible=%d cloakedValue=%lu rect=(%d,%d %dx%d) windowAtCardCenter=%p%s class=%s pid=%lu (ours=%lu)",
         IsWindowVisible(g_hwnd) ? 1 : 0, CloakValue(g_hwnd), g_x, g_y, g_w, g_h, at, at == g_hwnd ? L" (our card)" : L"",
         cls, pid, GetCurrentProcessId());
}

////////////////////////////////////////////////////////////////////////////////
// Following the Start window
////////////////////////////////////////////////////////////////////////////////
static void ResetUiState() {
    g_hover = -1;
    g_hoverRemove = false;
    g_mode = 0;  // the next time Start opens the card shows the tiles again
    g_pscroll = 0;
    StopPickerKeys();
    StopDeskHold();
}

static void FinishHide() {
    g_animKind = 0;
    g_animGen++;
    g_hiding = false;
    g_shown = false;
    g_openT0 = 0;
    if (g_hwnd) ShowWindow(g_hwnd, SW_HIDE);
    ResetUiState();
    KeepOnlyNeededIcons(MakeLay(), false);
    LOGI(L"[win] card hidden");
}

static void HideCard() {
    if (g_dragging || g_down != -1) EndDrag();
    if (!g_shown) {
        g_openT0 = 0;  // a pending first-time wait is cancelled
        return;
    }
    if (g_hiding) return;
    if (g_menuOpen) EndMenu();
    g_hover = -1;
    g_hoverRemove = false;
    LOGI(L"[win] Start closed");
    if (g_cfg.animate) StartAnim(2);
    else FinishHide();
}

// Start lost the keyboard focus (Win key, click elsewhere): it is closing, so the card
// starts fading out now instead of waiting until Start's window is finally cloaked.
static void EarlyClose() {
    if (!g_shown || g_hiding || g_menuOpen) return;
    g_suppress = true;
    g_fgStart = false;
    HideCard();
}

// Polling every frame is only needed while Start is open or the card is still visible.
static void SetTickRate(bool fast) {
    UINT ms = fast ? kTickFastMs : kTickIdleMs;
    if (!g_hwnd || ms == g_tickMs) return;
    SetTimer(g_hwnd, kTimerTick, ms, nullptr);  // replaces the running timer
    g_tickMs = ms;
}

// timer = false when called from an event (does not advance the style schedule)
static void Tick(bool timer = true) {
    if (g_quit) {
        if (g_menuOpen) EndMenu();
        return;
    }
    if (!g_hwnd || !IsShellProcess()) return;

    HWND start = FindStartWindow();
    if (!start) {
        g_suppress = false;
        g_fgStart = false;
        HideCard();
        // Only the timer slows itself down (fast until the fade-out has finished); an event may
        // arrive just before Start becomes findable and must not cancel the fast polling.
        if (timer) SetTickRate(g_shown);
        return;
    }
    SetTickRate(true);
    if (g_suppress) return;  // closing: ignore the still visible Start window
    RECT wr{};
    if (!GetWindowRect(start, &wr) || wr.right - wr.left < 200 || wr.bottom - wr.top < 200) {
        HideCard();
        return;
    }
    UINT dpi = GetDpiForWindow(start);
    g_dpi = dpi ? (int)dpi : 96;

    if (g_hiding) {
        // Let a running fade-out finish; restarting in the middle of it is what looks like a flicker.
        double fadeMs = (double)(NowQpc() - g_animQpc0.load()) * 1000.0 / (double)g_qpcFreq;
        if (g_cfg.animate && fadeMs < kHideMs) return;
        // Start came back (or the fade never finished): start over
        g_hiding = false;
        g_animKind = 0;
        g_animGen++;
        ResetUiState();
        g_shown = false;
        g_openT0 = 0;
    }
    ULONGLONG now = GetTickCount64();
    if (!g_shown && !g_openT0) {  // a new opening of Start
        g_openT0 = now;
        g_openQpc = NowQpc();
        g_stylePass = 0;
        g_stateLogged = false;
        g_regionLogged = false;
    }
    const ULONGLONG elapsed = now - g_openT0;

    if (!g_fgStart) {
        HWND fg = GetForegroundWindow();
        DWORD fpid = 0;
        if (fg) GetWindowThreadProcessId(fg, &fpid);
        if (fpid && IsStartFocusProcess(fpid)) g_fgStart = true;
    }

    RECT fly{};
    int src = 0;
    if (g_cfg.autoDetect) {
        RECT box{};
        if (RegionBox(start, wr, box)) {
            fly = box;
            src = 2;
            RECT rel{box.left - wr.left, box.top - wr.top, box.right - wr.left, box.bottom - wr.top};
            bool differs = !g_mem.valid || memcmp(&g_mem.rel, &rel, sizeof(RECT)) != 0 ||
                           g_mem.winW != wr.right - wr.left || g_mem.winH != wr.bottom - wr.top;
            if (!g_regionLogged) {
                g_regionLogged = true;
                LOGI(L"[region] panel (%ld,%ld,%ld,%ld) %ldx%ld, available %llu ms after Start opened%s", box.left,
                     box.top, box.right, box.bottom, box.right - box.left, box.bottom - box.top, elapsed,
                     differs ? L" (differs from the remembered one)" : L"");
            }
            if (differs) {
                g_mem.valid = true;
                g_mem.rel = rel;
                g_mem.winW = wr.right - wr.left;
                g_mem.winH = wr.bottom - wr.top;
                SaveGeom();
            }
        } else if (g_mem.valid && g_mem.winW == wr.right - wr.left && g_mem.winH == wr.bottom - wr.top) {
            fly = RECT{wr.left + g_mem.rel.left, wr.top + g_mem.rel.top, wr.left + g_mem.rel.right,
                       wr.top + g_mem.rel.bottom};
            src = 1;
        }
    }
    if (src) {
        // The region is larger than the visible panel by a transparent margin, then the fine-tune
        // settings move each edge (positive = right / down).
        RECT p{fly.left + S(kRegionMargin + g_cfg.tuneL), fly.top + S(kRegionMargin + g_cfg.tuneT),
               fly.right + S(g_cfg.tuneR - kRegionMargin), fly.bottom + S(g_cfg.tuneB - kRegionMargin)};
        if (p.right - p.left >= S(100) && p.bottom - p.top >= S(100)) fly = p;
    }
    if (!src) fly = ManualFlyout(wr);

    // Nothing remembered yet: wait briefly for the region instead of showing the card in the wrong place.
    if (!g_shown && g_cfg.autoDetect && src == 0 && elapsed < kFirstWaitMs) return;

    int W = CardWidth();
    int top = fly.top + S(g_cfg.insetTop);
    int bottom = fly.bottom - S(g_cfg.insetBottom);
    if (bottom - top < 100) {
        top = fly.top;
        bottom = fly.bottom;
    }
    int H = bottom - top;
    int gap = S(g_cfg.gap);
    int x = g_cfg.left ? fly.left - gap - W : fly.right + gap;
    int workBottom = wr.bottom;
    MONITORINFO mi{sizeof(mi)};
    if (GetMonitorInfoW(MonitorFromRect(&wr, MONITOR_DEFAULTTONEAREST), &mi)) {
        RECT const& wa = mi.rcWork;
        workBottom = wa.bottom;
        if (x < wa.left || x + W > wa.right) {
            int alt = g_cfg.left ? fly.right + gap : fly.left - gap - W;  // no room: try the other side
            if (alt >= wa.left && alt + W <= wa.right)
                x = alt;
            else
                x = std::max((int)wa.left, std::min(x, (int)wa.right - W));
        }
    }

    // Start starts and ends its motion with its top edge at the taskbar; moving the card by the
    // same distance keeps it level with Start the whole way.
    g_slide = std::max(S(20), workBottom - (int)fly.top);

    bool first = !g_shown;
    bool changed = first || x != g_x || top != g_y || W != g_w || H != g_h;
    if (changed) {
        g_x = x;
        g_y = top;
        g_w = W;
        g_h = H;
        g_shown = true;
        if (first) {
            HoldAnim();  // invisible until the first frame is painted
            Repaint();
            SetWindowPos(g_hwnd, BelowTaskbar(wr), g_x, g_y, g_w, g_h, SWP_NOACTIVATE | SWP_SHOWWINDOW);
            StartAnim(1);
            if (g_cfg.diag && g_showLogs < 5) {
                g_showLogs++;
                LOGE(L"[win] docking beside Start window %p rect=(%ld,%ld,%ld,%ld), panel from %s (%ld,%ld,%ld,%ld); card at (%d,%d) %dx%d, dpi=%d",
                     start, wr.left, wr.top, wr.right, wr.bottom,
                     src == 2 ? L"window region" : src == 1 ? L"remembered region" : L"settings", fly.left, fly.top,
                     fly.right, fly.bottom, g_x, g_y, g_w, g_h, g_dpi);
                DumpStart(start);
            }
        } else {
            SyncAnimPos();
            g_aslide = g_slide;
            // Repaint() moves and resizes the layered window itself, including the animation offset.
            // A SetWindowPos to the final position here would make the card jump for one frame mid-animation.
            if (!IsWindowVisible(g_hwnd))
                SetWindowPos(g_hwnd, BelowTaskbar(wr), g_x, g_y, g_w, g_h, SWP_NOACTIVATE | SWP_SHOWWINDOW);
            Repaint();
        }
    }

    // Colour of the Start edge: after Start's own fade-in has finished, then twice more.
    if (timer && g_shown && g_cfg.autoStyle && src == 2) {
        static const ULONGLONG kSt[] = {350, 900, 1800};
        if (g_stylePass < 3 && elapsed >= kSt[g_stylePass]) {
            g_stylePass++;
            SampleStyle(fly, g_y, g_y + g_h);
        }
    }
    if (timer && g_shown && g_cfg.diag && !g_stateLogged && elapsed >= 600) {
        g_stateLogged = true;
        LogCardState();
    }
}

static void CALLBACK WinEventProc(HWINEVENTHOOK, DWORD ev, HWND hwnd, LONG idObject, LONG, DWORD, DWORD) {
    if (g_quit || !g_hwnd) return;
    if (ev == EVENT_SYSTEM_FOREGROUND) {
        if (!hwnd) return;
        DWORD pid = 0;
        GetWindowThreadProcessId(hwnd, &pid);
        if (IsStartFocusProcess(pid)) {
            g_fgStart = true;
            g_suppress = false;  // focus came back to Start: it is not closing after all
            SetTickRate(true);   // Start is opening: poll every frame
            Tick(false);
        } else if (g_fgStart && hwnd != g_hwnd) {
            EarlyClose();
        }
    } else if ((ev == EVENT_OBJECT_CLOAKED || ev == EVENT_OBJECT_UNCLOAKED) && idObject == OBJID_WINDOW) {
        WCHAR cls[96] = L"";
        if (hwnd && GetClassNameW(hwnd, cls, ARRAYSIZE(cls)) && g_cfg.startClass == cls) {
            SetTickRate(true);
            Tick(false);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// Window procedure and UI thread
////////////////////////////////////////////////////////////////////////////////
static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;  // never take focus: Start must stay open
        case WM_TIMER:
            if (wp == kTimerTick) {
                Tick(true);
            } else if (wp == kTimerIcon) {
                KillTimer(hwnd, kTimerIcon);
                g_iconTimer = false;
                Repaint();
            } else if (wp == kTimerFrame) {
                Lay L = MakeLay();
                bool sliding = g_mode == 0 && g_cfg.grid && TilesMoving(L);
                if (sliding) StepTiles(L);
                if (sliding || g_frameDirty) {
                    g_frameDirty = false;
                    Repaint();
                } else {
                    KillTimer(hwnd, kTimerFrame);
                    g_frameTimer = false;
                }
            } else if (wp == kTimerScroll) {
                if (!g_dragging) {
                    KillTimer(hwnd, kTimerScroll);
                } else {
                    Lay L = MakeLay();
                    int old = g_scroll, edge = S(24);
                    if (g_dragPt.y < L.content.top + edge) g_scroll -= S(12);
                    else if (g_dragPt.y > L.content.bottom - edge) g_scroll += S(12);
                    ClampScroll(L);
                    if (g_scroll != old) Repaint();
                }
            }
            return 0;
        case WM_APP_ICON:
            if (!g_iconTimer) {  // several icons often arrive together: paint once
                g_iconTimer = true;
                SetTimer(hwnd, kTimerIcon, 30, nullptr);
            }
            return 0;
        case WM_APP_KEY:
            PickerKey((WCHAR)wp);
            return 0;
        case WM_APP_DESKCLICK:
            DeskClicked();
            return 0;
        case WM_APP_ANIMDONE:
            if ((int)wp == g_animGen.load() && g_hiding) FinishHide();
            return 0;
        case WM_APP_ADD: {
            std::unique_ptr<std::vector<Item>> v(reinterpret_cast<std::vector<Item>*>(lp));
            AddItems(*v);
            return 0;
        }
        case WM_APP_APPS: {
            std::unique_ptr<std::vector<Item>> v(reinterpret_cast<std::vector<Item>*>(lp));
            g_apps = std::move(*v);
            g_appsLoaded = true;
            RebuildView();
            Repaint();
            return 0;
        }
        case WM_APP_QUIT:
            DestroyWindow(hwnd);
            return 0;
        case WM_MOUSEMOVE: {
            int x = GET_X_LPARAM(lp), y = GET_Y_LPARAM(lp);
            if (!g_trackingLeave) {
                TRACKMOUSEEVENT t{sizeof(t), TME_LEAVE, hwnd, 0};
                if (TrackMouseEvent(&t)) g_trackingLeave = true;
            }
            if (g_mode == 0 && g_down >= 0 && !g_pressRemove) {  // a tile is pressed: maybe start dragging
                int dx = x - (int)g_downPt.x, dy = y - (int)g_downPt.y;
                if (dx < 0) dx = -dx;
                if (dy < 0) dy = -dy;
                if (!g_dragging && (dx > GetSystemMetrics(SM_CXDRAG) || dy > GetSystemMetrics(SM_CYDRAG))) {
                    g_dragging = true;
                    g_dragFrom = g_down;
                    SetTimer(hwnd, kTimerScroll, 40, nullptr);
                }
                if (g_dragging) {
                    g_dragPt = POINT{x, y};
                    RequestFrame();
                    return 0;
                }
            }
            int h = HitTest(x, y);
            bool rem = h >= 0 && InRemoveBtn(h, x, y);
            if (h != g_hover || rem != g_hoverRemove) {
                g_hover = h;
                g_hoverRemove = rem;
                Repaint();
            }
            return 0;
        }
        case WM_MOUSELEAVE:
            g_trackingLeave = false;
            if (!g_dragging && (g_hover != -1 || g_hoverRemove)) {
                g_hover = -1;
                g_hoverRemove = false;
                Repaint();
            }
            return 0;
        case WM_LBUTTONDOWN: {
            int x = GET_X_LPARAM(lp), y = GET_Y_LPARAM(lp);
            g_down = HitTest(x, y);
            g_pressRemove = g_down >= 0 && InRemoveBtn(g_down, x, y);
            g_downPt = POINT{x, y};
            if (g_mode == 0 && g_down >= 0 && g_down < (int)g_items.size()) {
                Lay L = MakeLay();
                RECT r = TileRect(L, g_down);
                g_grab = POINT{x - (int)r.left, y - (int)r.top};
            }
            if (g_down != -1) SetCapture(hwnd);
            return 0;
        }
        case WM_LBUTTONUP: {
            int x = GET_X_LPARAM(lp), y = GET_Y_LPARAM(lp);
            if (g_dragging) {
                CommitDrag(x, y);
                EndDrag();
                Repaint();
                return 0;
            }
            int pressed = g_down;
            bool pressRemove = g_pressRemove;
            if (GetCapture() == hwnd) ReleaseCapture();
            g_down = -1;
            g_pressRemove = false;
            int h = HitTest(x, y);
            if (pressed != -1 && h == pressed) {
                if (h == -2) {
                    OpenPicker();
                } else if (h == -3) {
                    StartFilePicker();
                } else if (h == -4) {
                    ClosePicker();
                } else if (g_mode == 1) {
                    if (h >= 0) TogglePickerRow(h);
                } else {
                    bool onRemove = h >= 0 && InRemoveBtn(h, x, y);
                    if (pressRemove && onRemove) RemoveAt(h);
                    else if (!pressRemove && !onRemove && h >= 0 && h < (int)g_items.size()) Launch(g_items[h], L"open");
                }
            }
            return 0;
        }
        case WM_CAPTURECHANGED:
            if (lp != 0 && reinterpret_cast<HWND>(lp) != hwnd && (g_dragging || g_down != -1)) {
                EndDrag();  // another window took the mouse: cancel the drag
                Repaint();
            }
            return 0;
        case WM_RBUTTONUP: {
            if (g_mode == 1) return 0;
            int h = HitTest(GET_X_LPARAM(lp), GET_Y_LPARAM(lp));
            if (h >= 0) ShowItemMenu(h);
            else if (h == -1) ShowCardMenu();
            return 0;
        }
        case WM_MOUSEWHEEL: {
            Lay L = MakeLay();
            int& s = g_mode == 1 ? g_pscroll : g_scroll;
            int step = g_mode == 1 ? L.rowH * 2 : L.tileH / 2;
            s -= GET_WHEEL_DELTA_WPARAM(wp) * step / WHEEL_DELTA;
            ClampScroll(L);
            Repaint();
            return 0;
        }
        case WM_DESTROY:
            RevokeDragDrop(hwnd);
            KillTimer(hwnd, kTimerTick);
            KillTimer(hwnd, kTimerScroll);
            KillTimer(hwnd, kTimerIcon);
            KillTimer(hwnd, kTimerFrame);
            g_frameTimer = false;
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

static DWORD WINAPI UiThread(LPVOID) {
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    HRESULT oleHr = OleInitialize(nullptr);
    LoadItems();
    LoadGeom();
    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = WndProc;
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.lpszClassName = L"CustomStartMenuAppsSideCard";
    RegisterClassExW(&wc);
    HWND h = CreateWindowExW(WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_NOACTIVATE, wc.lpszClassName,
                             L"Custom Start Menu Apps", WS_POPUP, 0, 0, 10, 10, nullptr, nullptr, wc.hInstance, nullptr);
    if (!h) {
        LOGE(L"[init] CreateWindowEx failed: %u", GetLastError());
        UnregisterClassW(wc.lpszClassName, wc.hInstance);
        if (SUCCEEDED(oleHr)) OleUninitialize();
        return 0;
    }
    g_hwnd = h;
    DropTarget* dt = new DropTarget();
    HRESULT dh = RegisterDragDrop(h, dt);
    dt->Release();
    LOGI(L"[init] card window created, RegisterDragDrop: 0x%08X", (unsigned)dh);
    g_tickMs = 0;
    SetTickRate(false);
    // Immediate reactions: Start gets or loses focus, Start's window is cloaked or uncloaked.
    g_hookFg = SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, nullptr, WinEventProc, 0, 0,
                               WINEVENT_OUTOFCONTEXT);
    g_hookCloak = SetWinEventHook(EVENT_OBJECT_CLOAKED, EVENT_OBJECT_UNCLOAKED, nullptr, WinEventProc, 0, 0,
                                  WINEVENT_OUTOFCONTEXT);

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    if (g_hookFg) UnhookWinEvent(g_hookFg);
    if (g_hookCloak) UnhookWinEvent(g_hookCloak);
    StopPickerKeys();
    StopDeskHold();
    g_hookFg = g_hookCloak = nullptr;
    g_hwnd = nullptr;
    SurfFree(g_sb);
    SurfFree(g_sw);
    SurfFree(g_so);
    FreeFonts();
    UnregisterClassW(wc.lpszClassName, wc.hInstance);
    if (SUCCEEDED(oleHr)) OleUninitialize();
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
// Windhawk entry points
////////////////////////////////////////////////////////////////////////////////
BOOL Wh_ModInit() {
    LoadConfig();
    LOGE(L"[init] Custom Start Menu Apps 0.7.0 (side card hosted in explorer.exe)");
    if (!g_cfg.enabled) {
        LOGE(L"[init] disabled in settings; the mod does nothing");
        return TRUE;
    }
    // Explorer can run several processes; only the one that owns the taskbar needs the card.
    HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (tray) {
        DWORD pid = 0;
        GetWindowThreadProcessId(tray, &pid);
        if (pid != GetCurrentProcessId()) {
            LOGI(L"[init] not the taskbar process, nothing to do here");
            return TRUE;
        }
    }
    LARGE_INTEGER f;
    QueryPerformanceFrequency(&f);
    g_qpcFreq = f.QuadPart > 0 ? f.QuadPart : 1;
    g_quit = false;
    g_jobs = 0;
    g_pickerBusy = false;
    g_appsLoading = false;
    g_shown = false;
    g_hiding = false;
    g_suppress = false;
    g_fgStart = false;
    g_animKind = 0;
    g_openT0 = 0;
    g_mode = 0;
    g_showLogs = 0;
    g_animEvt = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (!g_animEvt) {
        LOGE(L"[init] CreateEvent failed: %u", GetLastError());
        g_quit = true;
        return FALSE;
    }
    // Started before the window thread, so g_cfg.animate is final before anything reads it.
    g_animThread = CreateThread(nullptr, 0, AnimThread, nullptr, 0, nullptr);
    if (!g_animThread) {
        // Without this thread the fade-out never reports that it finished and the card would stay on screen.
        LOGE(L"[init] animation thread failed: %u (the card will appear without animation)", GetLastError());
        g_cfg.animate = false;
    }
    g_thread = CreateThread(nullptr, 0, UiThread, nullptr, 0, nullptr);
    if (!g_thread) {
        LOGE(L"[init] CreateThread failed: %u", GetLastError());
        g_quit = true;
        SetEvent(g_animEvt);
        if (g_animThread && WaitForSingleObject(g_animThread, 2000) != WAIT_OBJECT_0) {
            PinModule(L"the animation thread did not finish within 2 s");
            return FALSE;  // the thread may still wait on the event: leave both handles open
        }
        if (g_animThread) CloseHandle(g_animThread);
        g_animThread = nullptr;
        CloseHandle(g_animEvt);
        g_animEvt = nullptr;
        return FALSE;
    }
    g_started = true;
    return TRUE;
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    *bReload = TRUE;  // every setting is applied by reloading the mod: simple and safe
    return TRUE;
}

void Wh_ModUninit() {
    if (!g_started) return;
    g_quit = true;
    if (g_animEvt) SetEvent(g_animEvt);
    HWND h = g_hwnd;
    if (h) PostMessageW(h, WM_APP_QUIT, 0, 0);
    bool threadDone = true;
    if (g_thread) {
        if (WaitForSingleObject(g_thread, 3000) == WAIT_OBJECT_0) {
            CloseHandle(g_thread);
        } else {
            threadDone = false;
            PinModule(L"the window thread did not finish within 3 s");
        }
        g_thread = nullptr;
    }
    bool animDone = true;
    if (g_animThread) {
        if (WaitForSingleObject(g_animThread, 2000) == WAIT_OBJECT_0) {
            CloseHandle(g_animThread);
        } else {
            animDone = false;
            PinModule(L"the animation thread did not finish within 2 s");
        }
        g_animThread = nullptr;
    }
    if (g_animEvt && animDone) {
        CloseHandle(g_animEvt);
        g_animEvt = nullptr;
    }
    for (int i = 0; i < 40 && g_jobs.load() > 0; i++) Sleep(50);
    bool jobsDone = g_jobs.load() == 0;
    if (!jobsDone) PinModule(L"background jobs are still running");
    if (threadDone && jobsDone) FreeIcons();
    g_started = false;
    LOGI(L"[uninit] done");
}