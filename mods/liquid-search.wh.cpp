// ==WindhawkMod==
// @id              liquid-search
// @name            Liquid Search
// @description     Liquid-glass search bar with spring animations, hover zoom, a glass context menu, recents, file browser, clipboard history and all-drive file search            
// @author          itzderix
// @include         windhawk.exe
// @github          https://github.com/itzderix
// @compilerOptions -lgdi32 -luser32 -lshell32 -luxtheme -lshlwapi -ladvapi32 -lole32 -lshcore -lcrypt32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Liquid Search

![Spotlight for Windows](https://raw.githubusercontent.com/itzderix/spotlight-screenshots/main/Screenshot%202026-11-24%20122542%20%282%29.png)

A liquid-glass Spotlight-style search bar: a frosted, blurred-desktop glass panel with themes,
smooth open / close and selection animations, hover zoom and a draggable layout. The right-click
menu is drawn in the same glass.

## Demos

**Open animation (Jelly)**

![Jelly](https://raw.githubusercontent.com/itzderix/spotlight-screenshots/main/jelly.gif)

**Open animation (Fade)**

![Fade-in](https://raw.githubusercontent.com/itzderix/spotlight-screenshots/main/fade-in.gif)

**Switching selection / Tab**

![Jelly tab](https://raw.githubusercontent.com/itzderix/spotlight-screenshots/main/jelly-tab.gif)

**Right-click menu**

![Right-click menu](https://raw.githubusercontent.com/itzderix/spotlight-screenshots/main/right-click.gif)

## Usage

* **Hotkey** (default Ctrl+Space) opens / closes it. Ctrl+Space is also used by some editors and IDEs
  for code completion and by IME layouts; pick another combination in the settings if it clashes.
  If the combination is already taken by another program, the bar will not open.
* Type to search Start Menu apps, system tools, **files and folders on all drives** (fixed and removable), and more.
* **Tab** autofills the selected suggestion (grey ghost text). **Right arrow** at the end of the text also accepts it.
  When there is nothing to autofill, Tab switches to the file browser.
  **Shift+Tab / Up / Down** move the selection. **Enter** launches, **Esc** closes.
* Calculator: `12*(3+4)/2`, `100+15%`, `sqrt(2)+pi` + Enter copies the result. `>command args` runs a command.
* If nothing matches, the web search uses the browser you pick in settings.

## File browser
* Press Tab (with nothing to autofill) to switch between apps and Files.
* Enter opens a file or enters a folder. Backspace on an empty bar goes up one level.
* Type to filter the current folder. Ctrl+S changes the sort (name, type, modified, size).
* Ctrl+Tab / Ctrl+Shift+Tab cycles through the drives. Ctrl+1...9 jumps to Home, Downloads, Documents,
  Pictures, Music, Videos and then your drives.
* Right-click a row for Open, Open file location, Cut, Copy, Paste, Copy path and Delete (Recycle Bin).
* The list scrolls with the mouse wheel and the arrow keys.

## Clipboard history
* Off by default (setting "Clipboard history"). Type `clip:` to search what you copied.
* Enter pastes into the window you came from, Ctrl+Enter only copies, Ctrl+D pins / unpins,
  Ctrl+Shift+R removes the entry.
* Text only. Entries are encrypted for your Windows account (DPAPI) before they are stored, copies that an
  app marks as private (password managers) are skipped, and turning the feature off erases what was saved.

## Text box
* Click and drag inside the text field to select text. Double-click selects everything.
* Left / Right / Home / End move the caret (hold Shift to select). Ctrl+A, Ctrl+C, Ctrl+X, Ctrl+V work.
* Right-click the text field for Cut / Copy / Paste / Select all.

## Right-click on a result
* Open, Open file location, Cut, Copy, Paste (into that folder) and Copy path.
  Cut and Copy put the real file on the clipboard, so you can paste it in Explorer.

## Recents
* With an empty bar the most recently launched items are listed.
  They disappear the moment you start typing and are replaced by search results.
* Recents are stored per Windows user.
* Hover a recent and click the X (or press Delete) to remove it.

## Mouse
* Drag the bar with the left mouse button by the search icon or by the right end of the search row.
  The text field and the result rows never drag. Double-click the drag area to snap back to the
  original position. Dragging can be turned off in settings.
* Hover zoom: the bar grows smoothly (100%-200%) while the pointer is over it.

## Look
* Frosted glass: a blurred copy of the desktop behind the bar plus a white veil.
  (The blur is a snapshot taken when the bar opens.)
* Size, position (upper / center / top / custom), corner radius, themes, shadow, rim.
* App icon of the selected result is shown at the end of the search field.

## Animation
* Open/close: Fade, Jelly, Pop, Slide, Zoom, Bounce, Elastic, Rubber.
* Selection/Tab switching: Jelly, Pop, Shake, Bounce.
* Auto fade-out after N seconds without typing (configurable, 0 = off).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- hotkey: ctrl+space
  $name: Hotkey
  $options:
  - ctrl+space: Ctrl + Space
  - alt+space: Alt + Space
  - ctrl+alt+space: Ctrl + Alt + Space
  - ctrl+shift+space: Ctrl + Shift + Space
  - alt+shift+space: Alt + Shift + Space
- theme: frosted
  $name: Theme
  $options:
  - frosted: White frosted
  - system: Follow system (dark / light)
  - dark: Dark
  - light: Light
  - midnight: Midnight (deep blue)
  - ocean: Ocean (blue / teal)
  - aurora: Aurora (teal / purple)
  - sunset: Sunset (orange / magenta)
  - rose: Rose (pink)
  - forest: Forest (green)
  - lavender: Lavender (light purple)
- frostedGlass: true
  $name: Frosted glass (blurred desktop behind the bar)
- frostBlur: 24
  $name: Frost blur amount (0-40)
- frostWhite: 30
  $name: Frost white veil % (0-100)
- tintOpacity: 150
  $name: Theme colour strength (0-255)
  $description: With frosted glass on, this is how strongly the theme colour covers the blurred backdrop. With frosted glass off, it is the plain background opacity (0 = see-through).
- cornerRadius: 22
  $name: Corner radius in pixels (0-48)
- showBorder: true
  $name: Thin rim border
- showShadow: true
  $name: Soft drop shadow
- showIcons: true
  $name: Show app icons in the list
- showEndIcon: true
  $name: Show selected app icon at the end of the search field
- barSize: 100
  $name: Bar size % (60-200)
- hoverScale: 110
  $name: Hover zoom % (100 = off, 120 = 1.2x ... 200 = 2x)
- maxResults: 7
  $name: Maximum results (3-10)
- position: upper
  $name: Position
  $options:
  - upper: Upper centre (default)
  - center: Middle of the screen
  - top: Top of the screen
  - custom: Custom (use X / Y below)
- customX: 50
  $name: Custom X - % of screen width (centre of bar)
- customY: 30
  $name: Custom Y - % of screen height (centre of search row)
- allowDrag: true
  $name: Drag with mouse (search icon / right end of the bar; double-click to reset)
- alwaysOnTop: false
  $name: Always on top (stay open when focus is lost)
- idleSeconds: 5
  $name: Fade out after N seconds without typing (0 = never)
- idleFadeMs: 700
  $name: Idle fade-out duration in ms (0-3000)
- animation: jelly
  $name: Open / close animation
  $options:
  - none: None
  - fade: Fade
  - jelly: Jelly
  - pop: Pop (overshoot)
  - slide: Slide down
  - zoom: Zoom
  - bounce: Drop and bounce
  - elastic: Elastic
  - rubber: Rubber band
- animationMs: 450
  $name: Animation duration in ms (0-1500)
- jiggleStrength: 60
  $name: Animation strength (0-100)
- switchAnimation: jelly
  $name: Animation when switching selection / Tab
  $options:
  - none: None
  - jelly: Jelly
  - pop: Pop
  - shake: Shake
  - bounce: Bounce
- switchAnimMs: 350
  $name: Switch animation duration in ms (0-1000)
- showRecents: true
  $name: Show recents when the bar is empty
- recentsMax: 5
  $name: Number of recents (1-10)
- showApps: true
  $name: Suggest apps (Start Menu / Desktop)
- showTools: true
  $name: Suggest system tools (Settings, Task Manager ...)
- showFiles: true
  $name: Suggest files from all drives
- showFolders: true
  $name: Suggest folders from all drives
- fileResults: 3
  $name: Maximum file / folder results (1-6)
- showCalculator: true
  $name: Calculator results
- showCommands: true
  $name: Run commands with  >
- showWebsites: true
  $name: Open typed URLs
- showWebSearch: true
  $name: Web search fallback
- hiddenApps: ""
  $name: Hidden apps (names separated by ;)
- excludeDirs: "Windows;$Recycle.Bin;System Volume Information;node_modules;.git;Program Files;Program Files (x86);ProgramData;AppData"
  $name: Folders excluded from the drive index (names separated by ;)
  $description: Hidden and system files are always skipped. The index is built in the background and refreshed every 15 minutes.
- browser: default
  $name: Browser for web search / URLs
  $options:
  - default: System default browser
  - chrome: Google Chrome
  - edge: Microsoft Edge
  - firefox: Mozilla Firefox
  - brave: Brave
  - opera: Opera
  - vivaldi: Vivaldi
  - custom: Custom path (below)
- customBrowser: ""
  $name: Custom browser exe path
  $description: Only used when Browser is set to Custom. If the chosen browser is not installed, the default browser is used.
- webSearchUrl: https://www.google.com/search?q=
  $name: Web search URL prefix
- openMonitor: mouse
  $name: Open on monitor
  $options:
  - mouse: Monitor with the mouse
  - active: Monitor of the active window
  - primary: Primary monitor
- disableFullscreen: false
  $name: Disable in fullscreen
  $description: Ignore the hotkey while a fullscreen window (a game, a video player) is focused.
- calcDisplay: both
  $name: Calculator display
  $options:
  - card: Result card
  - inline: Inline text
  - both: Card and inline text
- fileBrowser: true
  $name: File browser
  $description: Press Tab (when no app suggestion is pending) to switch between apps and Files. Enter opens a file or enters a folder, Backspace on an empty bar goes up, Ctrl+Tab cycles drives, Ctrl+1..9 jumps to Home / Downloads / Documents / Pictures / Music / Videos / drives, Ctrl+S changes the sort.
- startDrive: ""
  $name: Start drive
  $description: Drive letter the file browser opens on the first time (for example D). Empty = Home.
- rememberSort: false
  $name: Remember file sort
- clipHistory: false
  $name: Clipboard history
  $description: Keeps text you copy and adds a "clip:" prefix to search it. Enter pastes into the window you came from, Ctrl+Enter only copies, Ctrl+D pins, Ctrl+Shift+R removes. Entries are encrypted for your Windows account (DPAPI) before they are stored; copies marked private by password managers are skipped. Turning it off erases what was saved.
- clipDays: 3
  $name: Clipboard retention (days)
  $description: Older unpinned entries are deleted (0 = keep until the 30-entry limit removes them).
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windowsx.h>
#include <uxtheme.h>
#include <shellapi.h>
#include <shlwapi.h>
#include <shellscalingapi.h>
#include <sddl.h>
#include <wincrypt.h>
#include <windhawk_utils.h>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <cwctype>
#include <map>
#include <set>
#include <string>
#include <vector>

extern "C" IMAGE_DOS_HEADER __ImageBase;

// ---------------------------------------------------------------- types ---

enum { C_APP, C_FILE, C_FOLDER, C_CALC, C_CMD, C_SITE, C_SEARCH, C_RECENT };

struct Item {
    std::wstring name, sub, target;
    int kind;  // 0 = open/launch, 1 = copy text, 3 = run command, 4 = open URL in chosen browser, 5 = paste clipboard entry, 6 = enter folder
    int cat;
};

struct Entry {
    std::wstring name, path;
    bool tool;
};

struct Recent {
    std::wstring name, target;
    int cat;
};

struct IconPx {
    int s = 0;
    std::vector<DWORD> px;  // premultiplied ARGB
};

struct Pal {
    COLORREF top, bot, text, dim, ghost, hi, edge;
    bool dark;
};

struct FRec {
    uint32_t off;
    uint32_t parent;
    uint16_t len;
    uint16_t flags;  // 1 = directory
};

struct DropFilesHdr {  // same layout as DROPFILES
    DWORD pFiles;
    POINT pt;
    BOOL fNC;
    BOOL fWide;
};

struct MenuEnt {
    int id;
    std::wstring text;
    bool enabled;
    bool sep;
};

typedef HRESULT(WINAPI* pSHGetImageList)(int, REFIID, void**);

// -------------------------------------------------------------- globals ---

enum { WM_APP_SETTINGS = WM_APP + 1 };
static constexpr int LAYOUT_W = 680;
static constexpr int SEARCH_H = 64;
static constexpr int ROW_H = 52;
static constexpr int PAD = 8;
static constexpr int ICON_SZ = 34;
static constexpr int MARGIN = 36;
static constexpr int BD_DIV = 4;
static constexpr int DRAG_ZONE = 110;  // right end of the search row: drags the bar instead of selecting text
enum { ANIM_NONE = 0, ANIM_OPEN = 1, ANIM_CLOSE = 2, ANIM_PULSE = 3 };
enum { TIMER_BLINK = 1, TIMER_ANIM = 2, TIMER_HOVER = 3, TIMER_IDLE = 4 };
enum { ST_NONE, ST_FADE, ST_JELLY, ST_POP, ST_SLIDE, ST_ZOOM, ST_BOUNCE, ST_ELASTIC, ST_RUBBER };
enum { PU_NONE, PU_JELLY, PU_POP, PU_SHAKE, PU_BOUNCE };
enum {
    M_OPEN = 1, M_LOC, M_CUT, M_COPY, M_PASTE, M_COPYPATH,
    M_TCUT, M_TCOPY, M_TPASTE, M_TSELALL, M_DELETE
};

static const wchar_t* kClass = L"WindhawkSpotlightWnd";
static const wchar_t* kPlaceholder = L"Liquid Search";
static const wchar_t* kMenuClass = L"WindhawkSpotlightMenu";

static HANDLE g_thread = nullptr;
static DWORD g_threadId = 0;
static HWND g_hwnd = nullptr;
static HTHEME g_theme = nullptr;

static struct {
    std::wstring hotkey, theme, webUrl, animation, switchAnim, position, browser, customBrowser,
        hiddenApps, excludeDirs;
    int opacity = 150, maxResults = 7, radius = 22, animMs = 450, jiggle = 60, switchMs = 350,
        frostBlur = 24, frostWhite = 30, barSize = 100, hoverPct = 110, customX = 50, customY = 30,
        idleSec = 5, idleFadeMs = 700, recentsMax = 5, fileResults = 3;
    bool border = true, shadow = true, icons = true, endIcon = true, frost = true, drag = true,
         onTop = false, recents = true, apps = true, tools = true, files = true, folders = true,
         calc = true, cmds = true, sites = true, websearch = true;
    std::wstring openMonitor, calcDisplay, startDrive;
    int clipDays = 3;
    bool noFull = false, fileBrowser = true, rememberSort = false, clipOn = false;
} S;

static std::vector<std::wstring> g_hidden;
static std::wstring g_browserPath, g_browserName;

static std::wstring g_query, g_ghost;
static size_t g_caret = 0, g_anchor = 0;  // text caret and selection anchor (indices into g_query)
static bool g_selecting = false;          // mouse-selecting text in the search field
static bool g_menuOpen = false;           // the glass context menu is showing
static HWND g_menuWnd = nullptr;
static std::vector<MenuEnt> g_mItems;
static std::vector<RECT> g_mRects;        // item rects, relative to the menu panel
static int g_mHot = -1, g_mW = 0, g_mH = 0, g_mM = 0, g_mX = 0, g_mY = 0, g_mRad = 0;
static HDC g_mDc = nullptr;
static HBITMAP g_mBmp = nullptr;
static HGDIOBJ g_mOld = nullptr;
static DWORD* g_mPx = nullptr;
static int g_ctxRow = -1;                 // what the menu was opened on
static Item g_ctxItem{};
static std::wstring g_ctxPath;
static bool g_ctxDir = false;
static void CloseMenu();
static std::vector<Item> g_items;
static std::vector<Entry> g_index;
static std::vector<Recent> g_recents;
static std::wstring g_sid;
static std::map<std::wstring, IconPx> g_icons;
static int g_sel = 0;
static ULONGLONG g_indexTick = 0;
static ULONGLONG g_shownTick = 0;
static ULONGLONG g_lastInput = 0;
static bool g_caretOn = true;
static int g_rad = 0;
static double g_scale = 1.0, g_dpi = 1.0, g_scaleBuilt = -1.0, g_hov = 1.0;
static HFONT g_fQuery, g_fName, g_fSub, g_fIcon, g_fSearch;
static Pal g_pal;

// panel geometry (screen pixels)
static int g_pw = 0, g_ph = 0, g_panelX = 0, g_panelY = 0;
static int g_off = 0;
static int g_baseCx = 0, g_baseTop = 0;       // current anchor (centre-x, top)
static int g_origCx = 0, g_origTop = 0;       // anchor from settings
static RECT g_work{}, g_homeWork{};
static bool g_userMoved = false;
static int g_dragCx = 0, g_dragTop = 0;
static bool g_dragging = false, g_dragMoved = false;
static POINT g_dragStart{}, g_dragBase0{}, g_lastPt{};

// off-screen buffers
static HDC g_srcDc = nullptr, g_dstDc = nullptr;
static HBITMAP g_srcBmp = nullptr, g_dstBmp = nullptr;
static HGDIOBJ g_oldSrc = nullptr, g_oldDst = nullptr;
static DWORD* g_srcPx = nullptr;
static DWORD* g_dstPx = nullptr;
static int g_ww = 0, g_wh = 0;
static std::vector<DWORD> g_shadow;
static int g_shKey[5] = {0, 0, 0, 0, 0};

// blurred desktop snapshot (1/BD_DIV resolution)
static std::vector<DWORD> g_bd;
static int g_bdW = 0, g_bdH = 0, g_bdX = 0, g_bdY = 0;

// animation
static int g_anim = ANIM_NONE;
static ULONGLONG g_animStart = 0;
static int g_animDur = 0;
static int g_animStyle = ST_NONE;
static bool g_idleFade = false;
static double g_curSx = 1.0, g_curSy = 1.0, g_curA = 1.0, g_curTx = 0.0, g_curTy = 0.0;

// drive index (built by a background thread)
static CRITICAL_SECTION g_fcs;
static HANDLE g_stopEvt = nullptr, g_rescanEvt = nullptr, g_idxThread = nullptr;
static bool g_cfgOn = false;
static std::wstring g_cfgExcl;
static std::vector<FRec> g_frec;
static std::wstring g_fpool, g_fpoolL;

// -------------------------------------------------------------- helpers ---

static int Sc(int v) { return (int)(v * g_scale + 0.5); }
static int Mg() { return Sc(MARGIN); }
static int Clampi(int v, int lo, int hi) { return std::max(lo, std::min(hi, v)); }

static std::wstring Lower(std::wstring s) {
    for (auto& c : s) c = (wchar_t)towlower(c);
    return s;
}

static std::wstring Trim(const std::wstring& s) {
    size_t a = s.find_first_not_of(L" \t");
    if (a == std::wstring::npos) return L"";
    size_t b = s.find_last_not_of(L" \t");
    return s.substr(a, b - a + 1);
}

static std::vector<std::wstring> SplitList(const std::wstring& s) {
    std::vector<std::wstring> out;
    size_t pos = 0;
    while (pos <= s.size()) {
        size_t e = s.find(L';', pos);
        if (e == std::wstring::npos) e = s.size();
        std::wstring t = Lower(Trim(s.substr(pos, e - pos)));
        if (!t.empty()) out.push_back(t);
        pos = e + 1;
    }
    return out;
}

static std::wstring GetStr(const wchar_t* name) {
    return std::wstring(WindhawkUtils::StringSetting::make(name).get());
}

static std::wstring Env(const wchar_t* s) {
    wchar_t b[MAX_PATH * 2];
    DWORD n = ExpandEnvironmentStringsW(s, b, ARRAYSIZE(b));
    return (n && n <= ARRAYSIZE(b)) ? std::wstring(b) : std::wstring();
}

static std::wstring UrlEnc(const std::wstring& s) {
    int n = WideCharToMultiByte(CP_UTF8, 0, s.c_str(), (int)s.size(), nullptr, 0, nullptr, nullptr);
    std::string u(n, 0);
    WideCharToMultiByte(CP_UTF8, 0, s.c_str(), (int)s.size(), &u[0], n, nullptr, nullptr);
    static const wchar_t* hex = L"0123456789ABCDEF";
    std::wstring o;
    for (unsigned char c : u) {
        if ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
            c == '-' || c == '_' || c == '.' || c == '~') {
            o += (wchar_t)c;
        } else {
            o += L'%';
            o += hex[c >> 4];
            o += hex[c & 15];
        }
    }
    return o;
}

static bool IsWow() {
    static int w = -1;
    if (w < 0) {
        BOOL b = FALSE;
        IsWow64Process(GetCurrentProcess(), &b);
        w = b ? 1 : 0;
    }
    return w == 1;
}

static std::wstring WinDir() {
    wchar_t win[MAX_PATH];
    UINT n = GetWindowsDirectoryW(win, MAX_PATH);
    if (!n || n >= MAX_PATH) return L"";
    return win;
}

// Resolves a System32 tool and returns the canonical ...\System32\... path (this is what is
// stored in the index and in recents). Existence is checked through Sysnative when this process
// is 32-bit (Windhawk 1.x), because plain System32 would be redirected to SysWOW64.
// Falls back to the plain name if the file is not found.
static std::wstring Sys32(const std::wstring& exe) {
    if (exe.empty() || exe.find(L':') != std::wstring::npos || exe[0] == L'\\' || exe[0] == L'/')
        return exe;
    std::wstring win = WinDir();
    if (win.empty()) return exe;
    std::wstring chk = win + (IsWow() ? L"\\Sysnative\\" : L"\\System32\\") + exe;
    std::wstring canon = win + L"\\System32\\" + exe;
    if (PathFileExistsW(chk.c_str())) return canon;
    if (exe.find(L'.') == std::wstring::npos) {  // name given without extension
        if (PathFileExistsW((chk + L".exe").c_str())) return canon + L".exe";
    }
    return exe;
}

// Maps a canonical System32 path to Sysnative when running as a 32-bit process, so that the
// 64-bit tool is started / its icon is read. Other paths are returned unchanged.
static std::wstring LaunchPath(const std::wstring& p) {
    if (!IsWow()) return p;
    std::wstring win = WinDir();
    if (win.empty()) return p;
    std::wstring pre = win + L"\\System32\\";
    if (p.size() > pre.size() && _wcsnicmp(p.c_str(), pre.c_str(), pre.size()) == 0)
        return win + L"\\Sysnative\\" + p.substr(pre.size());
    return p;
}

// Turns ...\Sysnative\... (old recents) back into the canonical ...\System32\... form.
static std::wstring NormSys(const std::wstring& p) {
    std::wstring win = WinDir();
    if (win.empty()) return p;
    std::wstring pre = win + L"\\Sysnative\\";
    if (p.size() > pre.size() && _wcsnicmp(p.c_str(), pre.c_str(), pre.size()) == 0)
        return win + L"\\System32\\" + p.substr(pre.size());
    return p;
}

// DPI scale (1.0 = 96 dpi) of a monitor.
static double MonDpi(HMONITOR hm) {
    UINT dx = 96, dy = 96;
    if (hm && SUCCEEDED(GetDpiForMonitor(hm, MDT_EFFECTIVE_DPI, &dx, &dy)) && dx) return dx / 96.0;
    HDC sdc = GetDC(nullptr);
    double d = GetDeviceCaps(sdc, LOGPIXELSX) / 96.0;
    ReleaseDC(nullptr, sdc);
    return d;
}

// String SID of the user this process runs as ("" on failure).
static std::wstring CurrentUserSid() {
    std::wstring out;
    HANDLE tok = nullptr;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &tok)) return out;
    DWORD sz = 0;
    GetTokenInformation(tok, TokenUser, nullptr, 0, &sz);
    if (sz) {
        std::vector<BYTE> buf(sz);
        if (GetTokenInformation(tok, TokenUser, buf.data(), sz, &sz)) {
            LPWSTR s = nullptr;
            if (ConvertSidToStringSidW(((TOKEN_USER*)buf.data())->User.Sid, &s)) {
                out = s;
                LocalFree(s);
            }
        }
    }
    CloseHandle(tok);
    return out;
}

// -------------------------------------------------------------- browser ---

static std::wstring RegAppPath(const wchar_t* exe) {
    std::wstring key = std::wstring(L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\") + exe;
    HKEY roots[2] = {HKEY_LOCAL_MACHINE, HKEY_CURRENT_USER};
    for (HKEY root : roots) {
        wchar_t b[MAX_PATH * 2];
        DWORD sz = sizeof(b);
        if (RegGetValueW(root, key.c_str(), nullptr,
                         RRF_RT_REG_SZ | RRF_RT_REG_EXPAND_SZ | RRF_SUBKEY_WOW6464KEY,
                         nullptr, b, &sz) == ERROR_SUCCESS) {
            std::wstring p = b;
            if (!p.empty() && p.front() == L'"') {
                size_t e = p.find(L'"', 1);
                p = p.substr(1, e == std::wstring::npos ? std::wstring::npos : e - 1);
            }
            if (!p.empty() && PathFileExistsW(p.c_str())) return p;
        }
    }
    return L"";
}

static void ResolveBrowser() {
    g_browserPath.clear();
    g_browserName.clear();
    const std::wstring& b = S.browser;
    if (b.empty() || b == L"default") return;
    const wchar_t* exe = nullptr;
    std::vector<std::wstring> fb;
    if (b == L"chrome") {
        exe = L"chrome.exe"; g_browserName = L"Chrome";
        fb = {Env(L"%ProgramW6432%\\Google\\Chrome\\Application\\chrome.exe"),
              Env(L"%ProgramFiles%\\Google\\Chrome\\Application\\chrome.exe"),
              Env(L"%ProgramFiles(x86)%\\Google\\Chrome\\Application\\chrome.exe"),
              Env(L"%LOCALAPPDATA%\\Google\\Chrome\\Application\\chrome.exe")};
    } else if (b == L"edge") {
        exe = L"msedge.exe"; g_browserName = L"Edge";
        fb = {Env(L"%ProgramFiles(x86)%\\Microsoft\\Edge\\Application\\msedge.exe"),
              Env(L"%ProgramW6432%\\Microsoft\\Edge\\Application\\msedge.exe"),
              Env(L"%ProgramFiles%\\Microsoft\\Edge\\Application\\msedge.exe")};
    } else if (b == L"firefox") {
        exe = L"firefox.exe"; g_browserName = L"Firefox";
        fb = {Env(L"%ProgramW6432%\\Mozilla Firefox\\firefox.exe"),
              Env(L"%ProgramFiles%\\Mozilla Firefox\\firefox.exe"),
              Env(L"%ProgramFiles(x86)%\\Mozilla Firefox\\firefox.exe")};
    } else if (b == L"brave") {
        exe = L"brave.exe"; g_browserName = L"Brave";
        fb = {Env(L"%ProgramW6432%\\BraveSoftware\\Brave-Browser\\Application\\brave.exe"),
              Env(L"%ProgramFiles%\\BraveSoftware\\Brave-Browser\\Application\\brave.exe"),
              Env(L"%LOCALAPPDATA%\\BraveSoftware\\Brave-Browser\\Application\\brave.exe")};
    } else if (b == L"opera") {
        exe = L"opera.exe"; g_browserName = L"Opera";
        fb = {Env(L"%LOCALAPPDATA%\\Programs\\Opera\\opera.exe"),
              Env(L"%ProgramW6432%\\Opera\\opera.exe"),
              Env(L"%ProgramFiles%\\Opera\\opera.exe")};
    } else if (b == L"vivaldi") {
        exe = L"vivaldi.exe"; g_browserName = L"Vivaldi";
        fb = {Env(L"%LOCALAPPDATA%\\Vivaldi\\Application\\vivaldi.exe"),
              Env(L"%ProgramW6432%\\Vivaldi\\Application\\vivaldi.exe"),
              Env(L"%ProgramFiles%\\Vivaldi\\Application\\vivaldi.exe")};
    } else if (b == L"custom") {
        std::wstring p = Env(S.customBrowser.c_str());
        if (!p.empty() && PathFileExistsW(p.c_str())) {
            g_browserPath = p;
            const wchar_t* f = PathFindFileNameW(p.c_str());
            g_browserName = f;
            size_t dot = g_browserName.rfind(L'.');
            if (dot != std::wstring::npos) g_browserName.resize(dot);
        }
        return;
    }
    if (!exe) return;
    std::wstring p = RegAppPath(exe);
    if (p.empty()) {
        for (auto& c : fb) {
            if (!c.empty() && PathFileExistsW(c.c_str())) { p = c; break; }
        }
    }
    if (p.empty()) g_browserName.clear();  // not installed: fall back to default browser
    g_browserPath = p;
}

static void OpenUrl(const std::wstring& url) {
    // A double quote would break out of the quoted browser argument and allow extra arguments.
    if (url.find(L'"') != std::wstring::npos) return;
    if (!g_browserPath.empty()) {
        std::wstring a = L"\"" + url + L"\"";
        ShellExecuteW(nullptr, L"open", g_browserPath.c_str(), a.c_str(), nullptr, SW_SHOWNORMAL);
    } else {
        ShellExecuteW(nullptr, L"open", url.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
    }
}

// ------------------------------------------------------------- settings ---

static void LoadSettings() {
    S.hotkey = GetStr(L"hotkey");
    S.theme = GetStr(L"theme");
    S.webUrl = GetStr(L"webSearchUrl");
    S.animation = GetStr(L"animation");
    S.switchAnim = GetStr(L"switchAnimation");
    S.position = GetStr(L"position");
    S.browser = GetStr(L"browser");
    S.customBrowser = GetStr(L"customBrowser");
    S.hiddenApps = GetStr(L"hiddenApps");
    S.excludeDirs = GetStr(L"excludeDirs");
    S.opacity = Clampi(Wh_GetIntSetting(L"tintOpacity"), 0, 255);
    S.maxResults = Clampi(Wh_GetIntSetting(L"maxResults"), 3, 10);
    S.radius = Clampi(Wh_GetIntSetting(L"cornerRadius"), 0, 48);
    S.animMs = Clampi(Wh_GetIntSetting(L"animationMs"), 0, 1500);
    S.jiggle = Clampi(Wh_GetIntSetting(L"jiggleStrength"), 0, 100);
    S.switchMs = Clampi(Wh_GetIntSetting(L"switchAnimMs"), 0, 1000);
    S.frostBlur = Clampi(Wh_GetIntSetting(L"frostBlur"), 0, 40);
    S.frostWhite = Clampi(Wh_GetIntSetting(L"frostWhite"), 0, 100);
    S.barSize = Clampi(Wh_GetIntSetting(L"barSize"), 60, 200);
    S.hoverPct = Clampi(Wh_GetIntSetting(L"hoverScale"), 100, 200);
    S.customX = Clampi(Wh_GetIntSetting(L"customX"), 0, 100);
    S.customY = Clampi(Wh_GetIntSetting(L"customY"), 0, 100);
    S.idleSec = Clampi(Wh_GetIntSetting(L"idleSeconds"), 0, 3600);
    S.idleFadeMs = Clampi(Wh_GetIntSetting(L"idleFadeMs"), 0, 3000);
    S.recentsMax = Clampi(Wh_GetIntSetting(L"recentsMax"), 1, 10);
    S.fileResults = Clampi(Wh_GetIntSetting(L"fileResults"), 1, 6);
    S.border = Wh_GetIntSetting(L"showBorder") != 0;
    S.shadow = Wh_GetIntSetting(L"showShadow") != 0;
    S.icons = Wh_GetIntSetting(L"showIcons") != 0;
    S.endIcon = Wh_GetIntSetting(L"showEndIcon") != 0;
    S.frost = Wh_GetIntSetting(L"frostedGlass") != 0;
    S.drag = Wh_GetIntSetting(L"allowDrag") != 0;
    S.onTop = Wh_GetIntSetting(L"alwaysOnTop") != 0;
    S.recents = Wh_GetIntSetting(L"showRecents") != 0;
    S.apps = Wh_GetIntSetting(L"showApps") != 0;
    S.tools = Wh_GetIntSetting(L"showTools") != 0;
    S.files = Wh_GetIntSetting(L"showFiles") != 0;
    S.folders = Wh_GetIntSetting(L"showFolders") != 0;
    S.calc = Wh_GetIntSetting(L"showCalculator") != 0;
    S.cmds = Wh_GetIntSetting(L"showCommands") != 0;
    S.sites = Wh_GetIntSetting(L"showWebsites") != 0;
    S.websearch = Wh_GetIntSetting(L"showWebSearch") != 0;
    S.openMonitor = GetStr(L"openMonitor");
    S.calcDisplay = GetStr(L"calcDisplay");
    S.startDrive = GetStr(L"startDrive");
    S.noFull = Wh_GetIntSetting(L"disableFullscreen") != 0;
    S.fileBrowser = Wh_GetIntSetting(L"fileBrowser") != 0;
    S.rememberSort = Wh_GetIntSetting(L"rememberSort") != 0;
    S.clipOn = Wh_GetIntSetting(L"clipHistory") != 0;
    S.clipDays = Clampi(Wh_GetIntSetting(L"clipDays"), 0, 365);
    if (S.webUrl.empty()) S.webUrl = L"https://www.google.com/search?q=";
    g_hidden = SplitList(S.hiddenApps);
    ResolveBrowser();

    bool on = S.files || S.folders;
    EnterCriticalSection(&g_fcs);
    bool chg = (g_cfgOn != on) || (g_cfgExcl != S.excludeDirs);
    g_cfgOn = on;
    g_cfgExcl = S.excludeDirs;
    LeaveCriticalSection(&g_fcs);
    if (chg && g_rescanEvt) SetEvent(g_rescanEvt);
}

static bool SystemDark() {
    DWORD v = 1, sz = sizeof(v);
    RegGetValueW(HKEY_CURRENT_USER,
                 L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                 L"AppsUseLightTheme", RRF_RT_REG_DWORD, nullptr, &v, &sz);
    return v == 0;
}

static Pal PalFor(std::wstring t) {
    if (t.empty() || t == L"system") t = SystemDark() ? L"dark" : L"light";
    //            top               bottom            text                dim                 ghost               hi              edge             dark
    if (t == L"frosted")
        return {RGB(255, 255, 255), RGB(238, 242, 250), RGB(28, 30, 36), RGB(96, 100, 110), RGB(158, 162, 172), RGB(0, 0, 0), RGB(255, 255, 255), false};
    if (t == L"light")
        return {RGB(247, 247, 249), RGB(234, 234, 239), RGB(25, 25, 25), RGB(95, 95, 100), RGB(150, 150, 156), RGB(0, 0, 0), RGB(0, 0, 0), false};
    if (t == L"midnight")
        return {RGB(24, 32, 78), RGB(10, 12, 34), RGB(235, 240, 255), RGB(150, 165, 210), RGB(105, 118, 165), RGB(255, 255, 255), RGB(140, 160, 255), true};
    if (t == L"ocean")
        return {RGB(10, 78, 112), RGB(6, 30, 62), RGB(230, 248, 255), RGB(140, 200, 220), RGB(95, 150, 170), RGB(255, 255, 255), RGB(120, 220, 255), true};
    if (t == L"aurora")
        return {RGB(20, 110, 120), RGB(78, 40, 130), RGB(240, 255, 250), RGB(170, 220, 215), RGB(120, 165, 165), RGB(255, 255, 255), RGB(150, 255, 220), true};
    if (t == L"sunset")
        return {RGB(240, 120, 60), RGB(160, 36, 110), RGB(255, 248, 242), RGB(255, 215, 195), RGB(225, 170, 150), RGB(255, 255, 255), RGB(255, 210, 160), true};
    if (t == L"rose")
        return {RGB(236, 110, 158), RGB(184, 64, 120), RGB(255, 247, 250), RGB(255, 222, 236), RGB(240, 178, 202), RGB(255, 255, 255), RGB(255, 205, 228), true};
    if (t == L"forest")
        return {RGB(24, 84, 58), RGB(8, 36, 30), RGB(232, 250, 238), RGB(150, 205, 170), RGB(105, 155, 125), RGB(255, 255, 255), RGB(130, 230, 170), true};
    if (t == L"lavender")
        return {RGB(236, 229, 255), RGB(210, 198, 246), RGB(40, 30, 70), RGB(100, 90, 140), RGB(150, 140, 185), RGB(60, 40, 120), RGB(140, 110, 230), false};
    return {RGB(32, 32, 36), RGB(24, 24, 28), RGB(255, 255, 255), RGB(175, 175, 180), RGB(130, 130, 136), RGB(255, 255, 255), RGB(255, 255, 255), true};
}

static void UpdateTheme() { g_pal = PalFor(S.theme); }

// ----------------------------------------------------------- calculator ---

struct Calc {
    const wchar_t* p;
    bool ok = true;
    void ws() { while (*p == L' ') p++; }
    bool pct() { ws(); if (*p == L'%') { p++; return true; } return false; }
    double expr() {
        double v = term();
        if (pct()) v /= 100.0;
        for (;;) {
            ws();
            if (*p == L'+') { p++; double t = term(); if (pct()) t = v * t / 100.0; v += t; }
            else if (*p == L'-') { p++; double t = term(); if (pct()) t = v * t / 100.0; v -= t; }
            else return v;
        }
    }
    double term() {
        double v = pw();
        for (;;) {
            ws();
            if (*p == L'*') { p++; v *= pw(); }
            else if (*p == L'/') { p++; v /= pw(); }
            else return v;
        }
    }
    double pw() {
        double b = unary();
        ws();
        if (*p == L'^') { p++; return std::pow(b, pw()); }
        return b;
    }
    double unary() {
        ws();
        if (*p == L'-') { p++; return -unary(); }
        if (*p == L'+') { p++; return unary(); }
        return prim();
    }
    double prim() {
        ws();
        if (*p == L'(') {
            p++;
            double v = expr();
            ws();
            if (*p == L')') p++; else ok = false;
            return v;
        }
        if (iswalpha(*p)) {
            wchar_t name[8] = {0};
            int k = 0;
            while (iswalpha(*p)) { if (k < 7) name[k++] = (wchar_t)towlower(*p); p++; }
            if (!wcscmp(name, L"pi")) return 3.14159265358979323846;
            if (!wcscmp(name, L"e")) return 2.71828182845904523536;
            ws();
            if (*p != L'(') { ok = false; return 0; }
            p++;
            double a = expr();
            ws();
            if (*p == L')') p++; else ok = false;
            if (!wcscmp(name, L"sqrt")) { if (a < 0) ok = false; return std::sqrt(a); }
            if (!wcscmp(name, L"sin")) return std::sin(a);
            if (!wcscmp(name, L"cos")) return std::cos(a);
            if (!wcscmp(name, L"tan")) return std::tan(a);
            if (!wcscmp(name, L"ln")) { if (a <= 0) ok = false; return std::log(a); }
            if (!wcscmp(name, L"log")) { if (a <= 0) ok = false; return std::log10(a); }
            if (!wcscmp(name, L"abs")) return std::fabs(a);
            if (!wcscmp(name, L"round")) return std::round(a);
            if (!wcscmp(name, L"floor")) return std::floor(a);
            if (!wcscmp(name, L"ceil")) return std::ceil(a);
            ok = false;
            return 0;
        }
        wchar_t* e;
        double v = wcstod(p, &e);
        if (e == p) { ok = false; return 0; }
        p = e;
        return v;
    }
};

static bool TryCalc(const std::wstring& q0, double& out) {
    std::wstring q = Lower(q0);
    bool hasOp = false, hasDigit = false;
    for (size_t i = 0; i < q.size(); i++) {
        wchar_t c = q[i];
        if (c >= L'0' && c <= L'9') hasDigit = true;
        else if (c == L'+' || c == L'-' || c == L'*' || c == L'/' || c == L'^' || c == L'(' ||
                 c == L'%') {
            if (i > 0 || c == L'(') hasOp = true;
        }
        else if (!((c >= L'a' && c <= L'z') || c == L'.' || c == L' ' || c == L')')) return false;
    }
    if (!hasOp || !hasDigit) return false;
    Calc c;
    c.p = q.c_str();
    double v = c.expr();
    c.ws();
    if (!c.ok || *c.p != 0 || !std::isfinite(v)) return false;
    out = v;
    return true;
}

// ---------------------------------------------------------------- recents ---
// Windhawk mod storage is machine-wide, so every value name carries the user's SID
// (<SID>_recentCount, <SID>_recent<i>Name/Target/Cat). Stored as one value per field
// because portable Windhawk keeps storage in an INI file, which can't hold line breaks.

static std::wstring CountKey() { return g_sid + L"_recentCount"; }

static std::wstring RKey(const wchar_t* field, int i) {
    return g_sid + L"_recent" + std::to_wstring(i) + field;
}

// Removes the old formats: the single "recents" value and the shared unprefixed values.
static void DeleteLegacyRecents() {
    Wh_DeleteValue(L"recents");
    int n = Clampi(Wh_GetIntValue(L"recentCount", 0), 0, 20);
    for (int i = 0; i < n; i++) {
        std::wstring b = L"recent" + std::to_wstring(i);
        Wh_DeleteValue((b + L"Name").c_str());
        Wh_DeleteValue((b + L"Target").c_str());
        Wh_DeleteValue((b + L"Cat").c_str());
    }
    Wh_DeleteValue(L"recentCount");
}

static void SaveRecents() {
    int old = Wh_GetIntValue(CountKey().c_str(), 0);
    int n = (int)g_recents.size();
    Wh_SetIntValue(CountKey().c_str(), n);
    for (int i = 0; i < n; i++) {
        Wh_SetStringValue(RKey(L"Name", i).c_str(), g_recents[i].name.c_str());
        Wh_SetStringValue(RKey(L"Target", i).c_str(), g_recents[i].target.c_str());
        Wh_SetIntValue(RKey(L"Cat", i).c_str(), g_recents[i].cat);
    }
    for (int i = n; i < old; i++) {
        Wh_DeleteValue(RKey(L"Name", i).c_str());
        Wh_DeleteValue(RKey(L"Target", i).c_str());
        Wh_DeleteValue(RKey(L"Cat", i).c_str());
    }
}

static void LoadRecents() {
    g_recents.clear();
    g_sid = CurrentUserSid();
    if (g_sid.empty()) g_sid = L"user";
    DeleteLegacyRecents();
    int n = Clampi(Wh_GetIntValue(CountKey().c_str(), 0), 0, 20);
    for (int i = 0; i < n; i++) {
        std::vector<wchar_t> name(1024, 0), target(4096, 0);
        Wh_GetStringValue(RKey(L"Name", i).c_str(), name.data(), (size_t)name.size() - 1);
        Wh_GetStringValue(RKey(L"Target", i).c_str(), target.data(), (size_t)target.size() - 1);
        Recent r;
        r.name = name.data();
        r.target = NormSys(target.data());
        r.cat = Wh_GetIntValue(RKey(L"Cat", i).c_str(), C_APP);
        if (!r.name.empty() && !r.target.empty()) g_recents.push_back(r);
    }
}

static void AddRecent(const Item& it) {
    if (it.cat != C_APP && it.cat != C_FILE && it.cat != C_FOLDER && it.cat != C_RECENT) return;
    std::wstring lt = Lower(it.target);
    int cat = it.cat == C_RECENT ? C_APP : it.cat;
    for (size_t i = 0; i < g_recents.size(); i++) {
        if (Lower(g_recents[i].target) == lt) {
            cat = g_recents[i].cat;
            g_recents.erase(g_recents.begin() + i);
            break;
        }
    }
    if (it.cat != C_RECENT) cat = it.cat;
    g_recents.insert(g_recents.begin(), Recent{it.name, it.target, cat});
    if (g_recents.size() > 20) g_recents.resize(20);
    SaveRecents();
}

// ======================= v3.4: clipboard history, file browser, helpers =======================

static constexpr int C_CLIP = C_RECENT + 1;  // clipboard history entry
static constexpr int K_PASTE = 5;            // Item::kind: paste a clipboard entry
static constexpr int K_NAV = 6;              // Item::kind: enter a folder in the file browser

static void NavTo(const std::wstring& path);        // defined further down
static void PasteClip(const std::wstring& text);    // defined further down

// Scrollable result list: g_full is everything, g_items is the visible slice.
static std::vector<Item> g_full;
static size_t g_top = 0;
static bool g_scrollable = false;
static std::wstring g_calcInline, g_calcCopy;
static HWND g_prevWnd = nullptr;  // window that had focus before the bar opened

static void ApplySlice() {
    size_t cap = (size_t)S.maxResults;
    if (g_top + cap > g_full.size()) g_top = g_full.size() > cap ? g_full.size() - cap : 0;
    size_t end = std::min(g_full.size(), g_top + cap);
    g_items.assign(g_full.begin() + g_top, g_full.begin() + end);
}

static HMONITOR PickMonitor() {
    if (S.openMonitor == L"primary")
        return MonitorFromPoint(POINT{0, 0}, MONITOR_DEFAULTTOPRIMARY);
    if (S.openMonitor == L"active" && g_prevWnd)
        return MonitorFromWindow(g_prevWnd, MONITOR_DEFAULTTOPRIMARY);
    POINT pt;
    GetCursorPos(&pt);
    return MonitorFromPoint(pt, MONITOR_DEFAULTTOPRIMARY);
}

static bool FullscreenActive() {
    HWND fg = GetForegroundWindow();
    if (!fg || fg == g_hwnd || fg == GetDesktopWindow() || fg == GetShellWindow()) return false;
    wchar_t cls[64] = {0};
    GetClassNameW(fg, cls, 64);
    if (!wcscmp(cls, L"WorkerW") || !wcscmp(cls, L"Progman")) return false;
    RECT r;
    if (!GetWindowRect(fg, &r)) return false;
    MONITORINFO mi{sizeof(mi)};
    if (!GetMonitorInfoW(MonitorFromWindow(fg, MONITOR_DEFAULTTONEAREST), &mi)) return false;
    return r.left <= mi.rcMonitor.left && r.top <= mi.rcMonitor.top &&
           r.right >= mi.rcMonitor.right && r.bottom >= mi.rcMonitor.bottom;
}

// ---- clipboard history (text, encrypted with DPAPI before it is stored) ----

struct Clip {
    std::wstring text;
    int minutes;  // minutes since 1970
    bool pinned;
};
static std::vector<Clip> g_clips;  // newest first
static bool g_clipListen = false;

static int NowMin() {
    FILETIME ft;
    GetSystemTimeAsFileTime(&ft);
    ULARGE_INTEGER u;
    u.LowPart = ft.dwLowDateTime;
    u.HighPart = ft.dwHighDateTime;
    return (int)((u.QuadPart / 10000000ULL - 11644473600ULL) / 60ULL);
}

static std::wstring ClipKey(const wchar_t* f, int i) {
    return g_sid + L"_clip" + f + std::to_wstring(i);
}

static bool ClipEncode(const std::wstring& text, std::wstring& hex) {
    DATA_BLOB in{(DWORD)(text.size() * sizeof(wchar_t)), (BYTE*)text.data()}, out{};
    if (!CryptProtectData(&in, L"ls", nullptr, nullptr, nullptr, CRYPTPROTECT_UI_FORBIDDEN, &out))
        return false;
    static const wchar_t* h = L"0123456789ABCDEF";
    hex.clear();
    hex.reserve((size_t)out.cbData * 2);
    for (DWORD i = 0; i < out.cbData; i++) {
        hex += h[out.pbData[i] >> 4];
        hex += h[out.pbData[i] & 15];
    }
    LocalFree(out.pbData);
    return true;
}

static bool ClipDecode(const std::wstring& hex, std::wstring& text) {
    auto hv = [](wchar_t c) -> int {
        if (c >= L'0' && c <= L'9') return c - L'0';
        if (c >= L'A' && c <= L'F') return c - L'A' + 10;
        return -1;
    };
    std::vector<BYTE> b;
    for (size_t i = 0; i + 1 < hex.size(); i += 2) {
        int a = hv(hex[i]), c = hv(hex[i + 1]);
        if (a < 0 || c < 0) return false;
        b.push_back((BYTE)(a * 16 + c));
    }
    if (b.empty()) return false;
    DATA_BLOB in{(DWORD)b.size(), b.data()}, out{};
    if (!CryptUnprotectData(&in, nullptr, nullptr, nullptr, nullptr, CRYPTPROTECT_UI_FORBIDDEN, &out))
        return false;
    text.assign((const wchar_t*)out.pbData, out.cbData / sizeof(wchar_t));
    LocalFree(out.pbData);
    return true;
}

static void SaveClips() {
    std::wstring cnt = g_sid + L"_clipCount";
    int old = Wh_GetIntValue(cnt.c_str(), 0);
    int n = (int)g_clips.size();
    Wh_SetIntValue(cnt.c_str(), n);
    for (int i = 0; i < n; i++) {
        std::wstring hex;
        if (!ClipEncode(g_clips[i].text, hex)) hex.clear();
        Wh_SetStringValue(ClipKey(L"Text", i).c_str(), hex.c_str());
        Wh_SetIntValue(ClipKey(L"Time", i).c_str(), g_clips[i].minutes);
        Wh_SetIntValue(ClipKey(L"Pin", i).c_str(), g_clips[i].pinned ? 1 : 0);
    }
    for (int i = n; i < old; i++) {
        Wh_DeleteValue(ClipKey(L"Text", i).c_str());
        Wh_DeleteValue(ClipKey(L"Time", i).c_str());
        Wh_DeleteValue(ClipKey(L"Pin", i).c_str());
    }
}

static void LoadClips() {
    g_clips.clear();
    int n = Clampi(Wh_GetIntValue((g_sid + L"_clipCount").c_str(), 0), 0, 60);
    for (int i = 0; i < n; i++) {
        std::vector<wchar_t> buf(32768, 0);
        Wh_GetStringValue(ClipKey(L"Text", i).c_str(), buf.data(), buf.size() - 1);
        Clip c;
        if (!ClipDecode(buf.data(), c.text) || c.text.empty()) continue;
        c.minutes = Wh_GetIntValue(ClipKey(L"Time", i).c_str(), 0);
        c.pinned = Wh_GetIntValue(ClipKey(L"Pin", i).c_str(), 0) != 0;
        g_clips.push_back(c);
    }
}

static void PruneClips() {
    int now = NowMin();
    for (size_t i = g_clips.size(); i-- > 0;) {
        if (!g_clips[i].pinned && S.clipDays > 0 && now - g_clips[i].minutes > S.clipDays * 1440)
            g_clips.erase(g_clips.begin() + i);
    }
    while (g_clips.size() > 30) {
        bool done = false;
        for (size_t i = g_clips.size(); i-- > 0;) {
            if (!g_clips[i].pinned) { g_clips.erase(g_clips.begin() + i); done = true; break; }
        }
        if (!done) break;
    }
}

static void ClipAdd(const std::wstring& raw) {
    std::wstring t = raw.substr(0, 2000);
    if (Trim(t).empty()) return;
    bool pin = false;
    for (size_t i = 0; i < g_clips.size(); i++) {
        if (g_clips[i].text == t) {
            pin = g_clips[i].pinned;
            g_clips.erase(g_clips.begin() + i);
            break;
        }
    }
    g_clips.insert(g_clips.begin(), Clip{t, NowMin(), pin});
    PruneClips();
    SaveClips();
}

static bool ReadClipText(std::wstring& out) {
    for (int k = 0; k < 5; k++) {
        if (OpenClipboard(g_hwnd)) {
            HANDLE h = GetClipboardData(CF_UNICODETEXT);
            if (h) {
                const wchar_t* p = (const wchar_t*)GlobalLock(h);
                if (p) {
                    size_t sz = GlobalSize(h) / sizeof(wchar_t), n = 0;
                    while (n < sz && p[n] && n < 2000) n++;
                    out.assign(p, n);
                    GlobalUnlock(h);
                }
            }
            CloseClipboard();
            return !out.empty();
        }
        Sleep(15);
    }
    return false;
}

static void OnClipUpdate() {
    if (!S.clipOn || g_sid.empty()) return;
    if (GetClipboardOwner() == g_hwnd) return;  // our own copies
    static UINT excl = RegisterClipboardFormatW(L"ExcludeClipboardContentFromMonitorProcessing");
    if (IsClipboardFormatAvailable(excl)) return;  // password managers
    if (!IsClipboardFormatAvailable(CF_UNICODETEXT)) return;
    std::wstring t;
    if (ReadClipText(t)) ClipAdd(t);
}

static void ApplyClipSetting() {
    if (!g_hwnd || g_sid.empty()) return;
    if (S.clipOn) {
        if (!g_clipListen && AddClipboardFormatListener(g_hwnd)) g_clipListen = true;
        LoadClips();
        PruneClips();
    } else {
        if (g_clipListen) { RemoveClipboardFormatListener(g_hwnd); g_clipListen = false; }
        if (Wh_GetIntValue((g_sid + L"_clipCount").c_str(), 0) > 0) {
            g_clips.clear();
            SaveClips();  // erases everything that was saved
        }
    }
}

static std::wstring AgeStr(int minutes) {
    int d = NowMin() - minutes;
    if (d < 1) return L"just now";
    if (d < 60) return std::to_wstring(d) + L" min ago";
    if (d < 1440) return std::to_wstring(d / 60) + L" h ago";
    return std::to_wstring(d / 1440) + L" d ago";
}

static void UpdateClipResults(const std::wstring& filter) {
    PruneClips();
    std::wstring lf = Lower(filter);
    for (int pass = 0; pass < 2; pass++) {  // pinned first
        for (auto& c : g_clips) {
            if ((pass == 0) != c.pinned) continue;
            if (!lf.empty() && Lower(c.text).find(lf) == std::wstring::npos) continue;
            std::wstring line;
            for (wchar_t ch : c.text) {
                if (line.size() >= 120) break;
                line += (ch == L'\r' || ch == L'\n' || ch == L'\t') ? L' ' : ch;
            }
            std::wstring sub = L"Clipboard \u00B7 " + AgeStr(c.minutes) +
                               (c.pinned ? L" \u00B7 pinned" : L"");
            g_full.push_back({line, sub, c.text, K_PASTE, C_CLIP});
        }
    }
}

// ---- file browser ----

struct DirEnt {
    std::wstring name, lname;
    bool dir;
    ULONGLONG size;
    FILETIME mt;
};
struct Chip {
    std::wstring name, path;
    bool drive;
};
static std::vector<DirEnt> g_dir;
static std::wstring g_path;  // "" = list of drives
static bool g_files = false, g_pathInit = false;
static int g_sort = 0;  // 0 name, 1 type, 2 modified, 3 size

static std::wstring JoinPath(const std::wstring& base, const std::wstring& name) {
    if (base.empty()) return name + L"\\";
    if (base.back() == L'\\') return base + name;
    return base + L"\\" + name;
}

static std::wstring ParentOf(std::wstring p) {
    while (p.size() > 3 && p.back() == L'\\') p.pop_back();
    if (p.size() <= 3) return L"";  // drive root -> list of drives
    size_t k = p.find_last_of(L'\\');
    if (k == std::wstring::npos) return L"";
    p.resize(k);
    if (p.size() == 2) p += L'\\';
    return p;
}

static void LoadDir(const std::wstring& path) {
    g_dir.clear();
    if (path.empty()) {
        DWORD m = GetLogicalDrives();
        for (int i = 2; i < 26; i++) {
            if (!(m & (1u << i))) continue;
            wchar_t r[4] = {(wchar_t)(L'A' + i), L':', L'\\', 0};
            UINT t = GetDriveTypeW(r);
            if (t != DRIVE_FIXED && t != DRIVE_REMOVABLE) continue;
            DirEnt e{};
            e.name = std::wstring(r, 2);
            e.lname = Lower(e.name);
            e.dir = true;
            g_dir.push_back(e);
        }
        return;
    }
    WIN32_FIND_DATAW fd;
    std::wstring pat = JoinPath(path, L"*");
    HANDLE h = FindFirstFileExW(pat.c_str(), FindExInfoBasic, &fd, FindExSearchNameMatch, nullptr,
                                FIND_FIRST_EX_LARGE_FETCH);
    if (h == INVALID_HANDLE_VALUE) return;
    do {
        if (fd.cFileName[0] == L'.' &&
            (!fd.cFileName[1] || (fd.cFileName[1] == L'.' && !fd.cFileName[2])))
            continue;
        if (fd.dwFileAttributes & (FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM)) continue;
        DirEnt e{};
        e.name = fd.cFileName;
        e.lname = Lower(e.name);
        e.dir = (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
        e.size = ((ULONGLONG)fd.nFileSizeHigh << 32) | fd.nFileSizeLow;
        e.mt = fd.ftLastWriteTime;
        g_dir.push_back(e);
        if (g_dir.size() > 20000) break;
    } while (FindNextFileW(h, &fd));
    FindClose(h);
}

static std::vector<Chip> Chips() {
    std::vector<Chip> c;
    std::wstring h = Env(L"%USERPROFILE%");
    if (!h.empty()) {
        c.push_back({L"Home", h, false});
        const wchar_t* sub[] = {L"Downloads", L"Documents", L"Pictures", L"Music", L"Videos"};
        for (const wchar_t* s : sub) {
            std::wstring p = h + L"\\" + s;
            if (PathIsDirectoryW(p.c_str())) c.push_back({s, p, false});
        }
    }
    DWORD m = GetLogicalDrives();
    for (int i = 2; i < 26; i++) {
        if (!(m & (1u << i))) continue;
        wchar_t r[4] = {(wchar_t)(L'A' + i), L':', L'\\', 0};
        UINT t = GetDriveTypeW(r);
        if (t == DRIVE_FIXED || t == DRIVE_REMOVABLE) c.push_back({std::wstring(r, 2), r, true});
    }
    return c;
}

static std::wstring FmtSize(ULONGLONG s) {
    wchar_t b[32];
    if (s < 1024ULL) return std::to_wstring(s) + L" B";
    if (s < 1024ULL * 1024) _snwprintf(b, 31, L"%.1f KB", s / 1024.0);
    else if (s < 1024ULL * 1024 * 1024) _snwprintf(b, 31, L"%.1f MB", s / (1024.0 * 1024));
    else _snwprintf(b, 31, L"%.1f GB", s / (1024.0 * 1024 * 1024));
    b[31] = 0;
    return b;
}

static std::wstring FmtTime(const FILETIME& ft) {
    FILETIME lf;
    SYSTEMTIME st;
    if (!FileTimeToLocalFileTime(&ft, &lf) || !FileTimeToSystemTime(&lf, &st)) return L"";
    wchar_t d[64] = {0}, t[64] = {0};
    GetDateFormatW(LOCALE_USER_DEFAULT, DATE_SHORTDATE, &st, nullptr, d, 64);
    GetTimeFormatW(LOCALE_USER_DEFAULT, TIME_NOSECONDS, &st, nullptr, t, 64);
    return std::wstring(d) + L" " + t;
}

static const wchar_t* SortName() {
    static const wchar_t* n[] = {L"name", L"type", L"modified", L"size"};
    return n[g_sort & 3];
}

static std::wstring Placeholder() {
    if (!g_files) return kPlaceholder;
    return L"Files \u00B7 " + (g_path.empty() ? std::wstring(L"This PC") : g_path) +
           L" \u00B7 sorted by " + SortName();
}

static void BuildFileItems() {
    std::wstring q = Lower(Trim(g_query));
    std::vector<size_t> idx;
    for (size_t i = 0; i < g_dir.size(); i++)
        if (q.empty() || g_dir[i].lname.find(q) != std::wstring::npos) idx.push_back(i);
    auto ext = [](const DirEnt& e) {
        size_t k = e.lname.rfind(L'.');
        return k == std::wstring::npos ? std::wstring() : e.lname.substr(k);
    };
    std::stable_sort(idx.begin(), idx.end(), [&](size_t a, size_t b) {
        const DirEnt& x = g_dir[a];
        const DirEnt& y = g_dir[b];
        if (x.dir != y.dir) return x.dir;  // folders first
        switch (g_sort) {
            case 1: { int c = ext(x).compare(ext(y)); if (c) return c < 0; break; }
            case 2: { int c = CompareFileTime(&x.mt, &y.mt); if (c) return c > 0; break; }
            case 3: if (x.size != y.size) return x.size > y.size; break;
        }
        return x.lname < y.lname;
    });
    for (size_t i : idx) {
        const DirEnt& e = g_dir[i];
        std::wstring full = JoinPath(g_path, e.name);
        std::wstring disp = g_path.empty() ? e.name + L"\\" : e.name;
        std::wstring sub;
        if (g_path.empty()) sub = L"Drive";
        else if (e.dir) sub = L"Folder \u00B7 " + FmtTime(e.mt) + L" \u00B7 " + g_path;
        else sub = FmtSize(e.size) + L" \u00B7 " + FmtTime(e.mt) + L" \u00B7 " + g_path;
        g_full.push_back({disp, sub, full, e.dir ? K_NAV : 0, e.dir ? C_FOLDER : C_FILE});
    }
}

// ======================= end of v3.4 data-side block =======================

// ---------------------------------------------------------------- index ---

static bool StopReq() { return WaitForSingleObject(g_stopEvt, 0) == WAIT_OBJECT_0; }

static void ScanDir(const std::wstring& dir, std::vector<Entry>& out, int depth) {
    if (depth > 6 || dir.empty()) return;
    WIN32_FIND_DATAW fd;
    HANDLE h = FindFirstFileW((dir + L"\\*").c_str(), &fd);
    if (h == INVALID_HANDLE_VALUE) return;
    do {
        if (StopReq()) break;
        if (fd.cFileName[0] == L'.') {
            if (!fd.cFileName[1] || (fd.cFileName[1] == L'.' && !fd.cFileName[2])) continue;
        }
        std::wstring full = dir + L"\\" + fd.cFileName;
        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            ScanDir(full, out, depth + 1);
            continue;
        }
        const wchar_t* ext = PathFindExtensionW(fd.cFileName);
        if (_wcsicmp(ext, L".lnk") && _wcsicmp(ext, L".url") && _wcsicmp(ext, L".exe")) continue;
        std::wstring name(fd.cFileName, ext - fd.cFileName);
        if (Lower(name).find(L"uninstall") != std::wstring::npos) continue;
        out.push_back({name, full, false});
    } while (FindNextFileW(h, &fd));
    FindClose(h);
}

static void BuildIndex() {
    std::vector<Entry> all;

    // Built-in tools go first: the first entry for a name wins, so a Start Menu shortcut with the
    // same name can't replace them with a 32-bit-redirected launch.
    all.push_back({L"Settings", L"ms-settings:", true});
    all.push_back({L"Calculator", Sys32(L"calc.exe"), true});
    all.push_back({L"Microsoft Store", L"ms-windows-store:", true});
    all.push_back({L"Task Manager", Sys32(L"taskmgr.exe"), true});
    all.push_back({L"File Explorer", L"explorer.exe", true});
    all.push_back({L"Command Prompt", Sys32(L"cmd.exe"), true});
    all.push_back({L"PowerShell", Sys32(L"WindowsPowerShell\\v1.0\\powershell.exe"), true});
    all.push_back({L"Windows Terminal", L"wt.exe", true});
    all.push_back({L"Control Panel", Sys32(L"control.exe"), true});
    all.push_back({L"Notepad", Sys32(L"notepad.exe"), true});
    all.push_back({L"Registry Editor", Sys32(L"regedit.exe"), true});
    all.push_back({L"Device Manager", Sys32(L"devmgmt.msc"), true});

    ScanDir(Env(L"%APPDATA%\\Microsoft\\Windows\\Start Menu\\Programs"), all, 0);
    ScanDir(Env(L"%ProgramData%\\Microsoft\\Windows\\Start Menu\\Programs"), all, 0);
    ScanDir(Env(L"%USERPROFILE%\\Desktop"), all, 0);
    ScanDir(Env(L"%PUBLIC%\\Desktop"), all, 0);
    std::wstring od = Env(L"%OneDrive%");
    if (!od.empty()) ScanDir(od + L"\\Desktop", all, 0);

    std::set<std::wstring> seen;
    g_index.clear();
    for (auto& e : all) {
        if (seen.insert(Lower(e.name)).second) g_index.push_back(e);
    }
}

// ---- all-drive file index (background thread) ----

static const size_t kMaxRecs = 600000;

struct ScanCtx {
    std::vector<FRec> recs;
    std::wstring pool, poolL;
    std::vector<std::wstring> excl;
};

static uint32_t AddRec(ScanCtx& c, const wchar_t* name, uint32_t parent, uint16_t flags) {
    FRec r;
    size_t len = std::min<size_t>(wcslen(name), 65535);
    r.off = (uint32_t)c.pool.size();
    r.len = (uint16_t)len;
    r.parent = parent;
    r.flags = flags;
    c.pool.append(name, len);
    c.pool.push_back(0);
    size_t st = c.poolL.size();
    c.poolL.append(name, len);
    for (size_t i = st; i < c.poolL.size(); i++) c.poolL[i] = (wchar_t)towlower(c.poolL[i]);
    c.poolL.push_back(0);
    c.recs.push_back(r);
    return (uint32_t)(c.recs.size() - 1);
}

static void ScanFiles(ScanCtx& cx, const std::wstring& dir, uint32_t parentIdx, int depth) {
    if (depth > 24 || cx.recs.size() >= kMaxRecs || StopReq()) return;
    WIN32_FIND_DATAW fd;
    HANDLE h = FindFirstFileExW((dir + L"\\*").c_str(), FindExInfoBasic, &fd,
                                FindExSearchNameMatch, nullptr, FIND_FIRST_EX_LARGE_FETCH);
    if (h == INVALID_HANDLE_VALUE) return;
    std::vector<std::pair<uint32_t, std::wstring>> subs;
    do {
        if (StopReq()) break;
        if (fd.cFileName[0] == L'.') {
            if (!fd.cFileName[1] || (fd.cFileName[1] == L'.' && !fd.cFileName[2])) continue;
        }
        if (fd.dwFileAttributes & (FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM)) continue;
        bool isDir = (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
        if (isDir) {
            if (fd.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) continue;
            std::wstring ln = Lower(fd.cFileName);
            bool skip = false;
            for (auto& e : cx.excl) if (e == ln) { skip = true; break; }
            if (skip) continue;
        }
        uint32_t idx = AddRec(cx, fd.cFileName, parentIdx, isDir ? 1 : 0);
        if (isDir) subs.push_back({idx, fd.cFileName});
        if (cx.recs.size() >= kMaxRecs) break;
    } while (FindNextFileW(h, &fd));
    FindClose(h);
    for (auto& s : subs) {
        if (StopReq() || cx.recs.size() >= kMaxRecs) return;
        ScanFiles(cx, dir + L"\\" + s.second, s.first, depth + 1);
    }
}

static void RunScan(const std::wstring& exclRaw) {
    ScanCtx cx;
    cx.excl = SplitList(exclRaw);
    wchar_t drives[512];
    DWORD n = GetLogicalDriveStringsW(ARRAYSIZE(drives), drives);
    for (wchar_t* d = drives; n && *d; d += wcslen(d) + 1) {
        UINT t = GetDriveTypeW(d);
        if (t != DRIVE_FIXED && t != DRIVE_REMOVABLE) continue;
        wchar_t l = (wchar_t)towupper(d[0]);
        if (l == L'A' || l == L'B') continue;
        if (StopReq()) return;
        std::wstring root(d, 2);
        uint32_t ri = AddRec(cx, root.c_str(), 0xFFFFFFFFu, 1);
        ScanFiles(cx, root, ri, 0);
    }
    if (StopReq()) return;
    EnterCriticalSection(&g_fcs);
    g_frec.swap(cx.recs);
    g_fpool.swap(cx.pool);
    g_fpoolL.swap(cx.poolL);
    LeaveCriticalSection(&g_fcs);
}

static DWORD WINAPI IndexThread(LPVOID) {
    SetThreadPriority(GetCurrentThread(), THREAD_MODE_BACKGROUND_BEGIN);
    HANDLE h[2] = {g_stopEvt, g_rescanEvt};
    DWORD wait = INFINITE;
    for (;;) {
        DWORD r = WaitForMultipleObjects(2, h, FALSE, wait);
        if (r == WAIT_OBJECT_0) break;
        bool on;
        std::wstring excl;
        EnterCriticalSection(&g_fcs);
        on = g_cfgOn;
        excl = g_cfgExcl;
        LeaveCriticalSection(&g_fcs);
        if (!on) {
            EnterCriticalSection(&g_fcs);
            std::vector<FRec>().swap(g_frec);
            std::wstring().swap(g_fpool);
            std::wstring().swap(g_fpoolL);
            LeaveCriticalSection(&g_fcs);
            wait = INFINITE;
            continue;
        }
        RunScan(excl);
        wait = 15 * 60 * 1000;
    }
    return 0;
}

// lock must be held
static std::wstring RecPath(uint32_t idx) {
    std::vector<uint32_t> chain;
    for (uint32_t c = idx; c != 0xFFFFFFFFu && c < g_frec.size() && chain.size() < 64;
         c = g_frec[c].parent)
        chain.push_back(c);
    std::wstring p;
    for (size_t i = chain.size(); i-- > 0;) {
        const FRec& r = g_frec[chain[i]];
        if (!p.empty()) p += L'\\';
        p.append(g_fpool.c_str() + r.off, r.len);
    }
    if (p.size() == 2 && p[1] == L':') p += L'\\';
    return p;
}

struct FHit {
    int score;
    uint32_t idx;
};

static void SearchFiles(const std::wstring& lq, std::vector<Item>& out, size_t want,
                        bool wantFiles, bool wantFolders) {
    EnterCriticalSection(&g_fcs);
    size_t n = g_frec.size();
    const wchar_t* pl = g_fpoolL.c_str();
    std::vector<FHit> top;
    for (size_t i = 0; i < n; i++) {
        const FRec& r = g_frec[i];
        bool dir = (r.flags & 1) != 0;
        if (r.len < lq.size() || r.parent == 0xFFFFFFFFu) continue;
        if (dir ? !wantFolders : !wantFiles) continue;
        const wchar_t* nm = pl + r.off;
        const wchar_t* f = wcsstr(nm, lq.c_str());
        if (!f) continue;
        size_t p = (size_t)(f - nm);
        int s;
        if (r.len == lq.size()) s = 1000;
        else if (p == 0) s = 900 - (int)r.len;
        else {
            wchar_t pv = nm[p - 1];
            s = (pv == L' ' || pv == L'-' || pv == L'_' || pv == L'.') ? 700 - (int)p : 500 - (int)p;
        }
        if (top.size() >= want && s <= top.back().score) continue;
        FHit h{s, (uint32_t)i};
        auto it = top.begin();
        while (it != top.end() && it->score >= s) ++it;
        top.insert(it, h);
        if (top.size() > want) top.pop_back();
    }
    for (auto& h : top) {
        const FRec& r = g_frec[h.idx];
        std::wstring name(g_fpool.c_str() + r.off, r.len);
        std::wstring full = RecPath(h.idx);
        std::wstring parent = RecPath(r.parent);
        bool dir = (r.flags & 1) != 0;
        out.push_back({name, parent, full, 0, dir ? C_FOLDER : C_FILE});
    }
    LeaveCriticalSection(&g_fcs);
}

// ---------------------------------------------------------------- results ---

static int Score(const std::wstring& name, const std::wstring& q) {
    if (name == q) return 1000;
    size_t p = name.find(q);
    if (p == 0) return 900 - (int)name.size();
    if (p != std::wstring::npos) {
        wchar_t prev = name[p - 1];
        if (prev == L' ' || prev == L'-' || prev == L'_' || prev == L'.') return 700 - (int)p;
        return 500 - (int)p;
    }
    size_t qi = 0;
    for (size_t i = 0; i < name.size() && qi < q.size(); i++)
        if (name[i] == q[qi]) qi++;
    return qi == q.size() ? 200 : -1;
}

static void ComputeGhost() {
    g_ghost.clear();
    if (g_query.empty() || g_items.empty()) return;
    const Item& it = g_items[g_sel];
    if (it.cat != C_APP) return;
    if (it.name.size() > g_query.size() &&
        _wcsnicmp(it.name.c_str(), g_query.c_str(), g_query.size()) == 0)
        g_ghost = it.name.substr(g_query.size());
}

static bool IsHidden(const std::wstring& lname) {
    for (auto& h : g_hidden) if (h == lname) return true;
    return false;
}

static void UpdateResults() {
    g_items.clear();
    g_sel = 0;
    g_ghost.clear();
    g_full.clear();
    g_scrollable = false;
    g_top = 0;
    g_calcInline.clear();
    if (g_files) {
        BuildFileItems();
        g_scrollable = true;
        ApplySlice();
        return;
    }
    std::wstring q = Trim(g_query);
    if (q.empty()) {
        if (S.recents) {
            size_t n = std::min<size_t>(g_recents.size(), (size_t)std::min(S.recentsMax, S.maxResults));
            for (size_t i = 0; i < n; i++)
                g_items.push_back({g_recents[i].name, L"Recent", g_recents[i].target, 0, C_RECENT});
        }
        return;
    }
    if (S.clipOn && _wcsnicmp(q.c_str(), L"clip:", 5) == 0) {
        UpdateClipResults(Trim(q.substr(5)));
        g_scrollable = true;
        ApplySlice();
        return;
    }
    size_t cap = (size_t)S.maxResults;

    double v;
    if (S.calc && TryCalc(q, v)) {
        wchar_t b[64];
        _snwprintf(b, 63, L"%.10g", v);
        b[63] = 0;
        g_calcCopy = b;
        if (S.calcDisplay != L"inline")
            g_items.push_back({std::wstring(L"= ") + b, L"Press Enter to copy result", b, 1, C_CALC});
        if (S.calcDisplay != L"card") g_calcInline = std::wstring(L"= ") + b;
    }
    if (S.cmds && q[0] == L'>') {
        std::wstring cmd = Trim(q.substr(1));
        if (!cmd.empty()) g_items.push_back({L"Run: " + cmd, L"Run command", cmd, 3, C_CMD});
    }
    std::wstring lq = Lower(q);
    if (S.sites &&
        (lq.rfind(L"http://", 0) == 0 || lq.rfind(L"https://", 0) == 0 || lq.rfind(L"www.", 0) == 0)) {
        std::wstring url = lq.rfind(L"www.", 0) == 0 ? L"https://" + q : q;
        g_items.push_back({L"Open " + q, L"Website", url, 4, C_SITE});
    }
    bool webItem = S.websearch && q[0] != L'>';
    size_t reserve = webItem ? 1 : 0;

    std::vector<std::pair<int, size_t>> sc;
    if (S.apps || S.tools) {
        for (size_t i = 0; i < g_index.size(); i++) {
            const Entry& e = g_index[i];
            if (e.tool ? !S.tools : !S.apps) continue;
            std::wstring ln = Lower(e.name);
            if (IsHidden(ln)) continue;
            int s = Score(ln, lq);
            if (s >= 0) sc.push_back({s, i});
        }
        std::sort(sc.begin(), sc.end(), [](auto& a, auto& b) {
            if (a.first != b.first) return a.first > b.first;
            return a.second < b.second;
        });
    }

    std::vector<Item> files;
    if (q[0] != L'>' && lq.size() >= 2 && (S.files || S.folders))
        SearchFiles(lq, files, (size_t)S.fileResults, S.files, S.folders);

    size_t used = g_items.size();
    size_t budget = cap > used + reserve ? cap - used - reserve : 0;
    size_t nf = std::min(files.size(), budget);
    size_t na = std::min(sc.size(), budget - nf);
    for (size_t i = 0; i < na; i++) {
        const Entry& e = g_index[sc[i].second];
        g_items.push_back({e.name, L"Application", e.path, 0, C_APP});
    }
    for (size_t i = 0; i < nf; i++) g_items.push_back(files[i]);

    if (webItem && g_items.size() < cap) {
        std::wstring sub = g_browserName.empty() ? L"Web search" : L"Web search \u00B7 " + g_browserName;
        g_items.push_back({L"Search the web for \"" + q + L"\"", sub, S.webUrl + UrlEnc(q), 4, C_SEARCH});
    }
    ComputeGhost();
}

// ---------------------------------------------------------------- icons ---

static bool IconToPx(HICON hi, IconPx& o) {
    ICONINFO ii{};
    if (!GetIconInfo(hi, &ii)) return false;
    bool ok = false;
    if (ii.hbmColor) {
        BITMAP bm{};
        GetObject(ii.hbmColor, sizeof(bm), &bm);
        int w = bm.bmWidth, h = bm.bmHeight;
        if (w > 0 && w == h) {
            BITMAPINFO bi{};
            bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
            bi.bmiHeader.biWidth = w;
            bi.bmiHeader.biHeight = -h;
            bi.bmiHeader.biPlanes = 1;
            bi.bmiHeader.biBitCount = 32;
            bi.bmiHeader.biCompression = BI_RGB;
            std::vector<DWORD> c((size_t)w * h), m((size_t)w * h);
            HDC dc = GetDC(nullptr);
            GetDIBits(dc, ii.hbmColor, 0, h, c.data(), &bi, DIB_RGB_COLORS);
            bool hasA = false;
            for (DWORD v : c) if (v >> 24) { hasA = true; break; }
            if (!hasA && ii.hbmMask) {
                GetDIBits(dc, ii.hbmMask, 0, h, m.data(), &bi, DIB_RGB_COLORS);
                for (size_t i = 0; i < c.size(); i++)
                    c[i] = (m[i] & 0xFFFFFF) ? 0 : (c[i] | 0xFF000000);
            }
            ReleaseDC(nullptr, dc);
            for (auto& v : c) {
                DWORD a = v >> 24;
                DWORD r = ((v >> 16) & 255) * a / 255;
                DWORD g = ((v >> 8) & 255) * a / 255;
                DWORD b = (v & 255) * a / 255;
                v = (a << 24) | (r << 16) | (g << 8) | b;
            }
            o.s = w;
            o.px = std::move(c);
            ok = true;
        }
    }
    if (ii.hbmColor) DeleteObject(ii.hbmColor);
    if (ii.hbmMask) DeleteObject(ii.hbmMask);
    return ok;
}

static bool LoadIconPx(const std::wstring& path, IconPx& out) {
    std::wstring p = path;
    if (p.find(L'\\') == std::wstring::npos) {
        if (p.empty() || p.back() == L':' || p.find(L"://") != std::wstring::npos) return false;
        wchar_t b[MAX_PATH];
        if (!SearchPathW(nullptr, p.c_str(), nullptr, MAX_PATH, b, nullptr)) {
            if (!SearchPathW(nullptr, p.c_str(), L".exe", MAX_PATH, b, nullptr)) return false;
        }
        p = b;
    }
    p = LaunchPath(p);  // 64-bit System32 file when this process is 32-bit
    SHFILEINFOW sfi{};
    if (!SHGetFileInfoW(p.c_str(), 0, &sfi, sizeof(sfi), SHGFI_SYSICONINDEX)) return false;

    static pSHGetImageList fn = (pSHGetImageList)GetProcAddress(
        GetModuleHandleW(L"shell32.dll"), MAKEINTRESOURCEA(727));
    if (!fn) return false;
    static const GUID kIID_IImageList = {0x46EB5926, 0x582E, 0x4017,
                                         {0x9F, 0xDF, 0xE8, 0x99, 0x8D, 0xAA, 0x09, 0x50}};
    void* il = nullptr;
    if (FAILED(fn(2 /*SHIL_EXTRALARGE*/, kIID_IImageList, &il)) || !il) return false;
    void** vt = *(void***)il;
    typedef HRESULT(STDMETHODCALLTYPE * GetIconFn)(void*, int, UINT, HICON*);
    typedef ULONG(STDMETHODCALLTYPE * ReleaseFn)(void*);
    HICON hi = nullptr;
    ((GetIconFn)vt[10])(il, sfi.iIcon, 1 /*ILD_TRANSPARENT*/, &hi);
    ((ReleaseFn)vt[2])(il);
    if (!hi) return false;
    bool ok = IconToPx(hi, out);
    DestroyIcon(hi);
    return ok;
}

static const IconPx* GetIconPx(const std::wstring& path) {
    std::wstring key = Lower(path);
    auto it = g_icons.find(key);
    if (it != g_icons.end()) return it->second.s ? &it->second : nullptr;
    if (g_icons.size() > 300) g_icons.clear();
    IconPx ic;
    LoadIconPx(path, ic);
    IconPx& ref = (g_icons[key] = std::move(ic));
    return ref.s ? &ref : nullptr;
}

static const IconPx* ItemIcon(const Item& it) {
    switch (it.cat) {
        case C_APP:
        case C_FILE:
        case C_FOLDER:
        case C_RECENT:
            return GetIconPx(it.target);
        case C_SITE:
        case C_SEARCH:
            return g_browserPath.empty() ? nullptr : GetIconPx(g_browserPath);
    }
    return nullptr;
}

static void BlitIcon(DWORD* px, int stride, int W, int H, int dx, int dy, int ds, const IconPx& ic) {
    double ratio = (double)ic.s / ds;
    for (int y = 0; y < ds; y++) {
        int py = dy + y;
        if (py < 0 || py >= H) continue;
        for (int x = 0; x < ds; x++) {
            int pxx = dx + x;
            if (pxx < 0 || pxx >= W) continue;
            double sx = (x + 0.5) * ratio - 0.5, sy = (y + 0.5) * ratio - 0.5;
            int x0 = (int)std::floor(sx), y0 = (int)std::floor(sy);
            double fx = sx - x0, fy = sy - y0;
            double ch[4] = {0, 0, 0, 0};
            for (int j = 0; j < 2; j++) {
                for (int i = 0; i < 2; i++) {
                    int xx = Clampi(x0 + i, 0, ic.s - 1), yy = Clampi(y0 + j, 0, ic.s - 1);
                    double wgt = (i ? fx : 1 - fx) * (j ? fy : 1 - fy);
                    DWORD v = ic.px[(size_t)yy * ic.s + xx];
                    ch[0] += wgt * (v >> 24);
                    ch[1] += wgt * ((v >> 16) & 255);
                    ch[2] += wgt * ((v >> 8) & 255);
                    ch[3] += wgt * (v & 255);
                }
            }
            int sa = (int)ch[0];
            if (sa <= 0) continue;
            DWORD dst = px[(size_t)py * stride + pxx];
            int ia = 255 - sa;
            int oa = Clampi(sa + (int)(dst >> 24) * ia / 255, 0, 255);
            int orr = Clampi((int)ch[1] + (int)((dst >> 16) & 255) * ia / 255, 0, 255);
            int og = Clampi((int)ch[2] + (int)((dst >> 8) & 255) * ia / 255, 0, 255);
            int ob = Clampi((int)ch[3] + (int)(dst & 255) * ia / 255, 0, 255);
            px[(size_t)py * stride + pxx] =
                ((DWORD)oa << 24) | ((DWORD)orr << 16) | ((DWORD)og << 8) | (DWORD)ob;
        }
    }
}

// ------------------------------------------------------- frosted backdrop ---

static void BlurPass(const std::vector<DWORD>& s, std::vector<DWORD>& d, int w, int h, int r,
                     bool horiz) {
    int len = horiz ? w : h, lines = horiz ? h : w;
    int win = 2 * r + 1;
    for (int l = 0; l < lines; l++) {
        auto at = [&](int k) -> DWORD {
            k = Clampi(k, 0, len - 1);
            return horiz ? s[(size_t)l * w + k] : s[(size_t)k * w + l];
        };
        int sr = 0, sg = 0, sb = 0;
        for (int k = -r; k <= r; k++) {
            DWORD p = at(k);
            sr += (p >> 16) & 255; sg += (p >> 8) & 255; sb += p & 255;
        }
        for (int i = 0; i < len; i++) {
            DWORD o = 0xFF000000u | ((DWORD)(sr / win) << 16) | ((DWORD)(sg / win) << 8) |
                      (DWORD)(sb / win);
            if (horiz) d[(size_t)l * w + i] = o; else d[(size_t)i * w + l] = o;
            DWORD a = at(i + r + 1), b = at(i - r);
            sr += (int)((a >> 16) & 255) - (int)((b >> 16) & 255);
            sg += (int)((a >> 8) & 255) - (int)((b >> 8) & 255);
            sb += (int)(a & 255) - (int)(b & 255);
        }
    }
}

// Grabs the monitor (while our window is hidden), shrinks it and blurs it.
static void CaptureBackdrop(const RECT& mon) {
    g_bd.clear();
    int mw = mon.right - mon.left, mh = mon.bottom - mon.top;
    if (mw <= 0 || mh <= 0) return;
    HDC sdc = GetDC(nullptr);
    HDC mdc = CreateCompatibleDC(sdc);
    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = mw;
    bi.bmiHeader.biHeight = -mh;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    DWORD* px = nullptr;
    HBITMAP bmp = CreateDIBSection(sdc, &bi, DIB_RGB_COLORS, (void**)&px, nullptr, 0);
    if (!bmp || !px) {
        if (bmp) DeleteObject(bmp);
        DeleteDC(mdc);
        ReleaseDC(nullptr, sdc);
        return;
    }
    HGDIOBJ old = SelectObject(mdc, bmp);
    BitBlt(mdc, 0, 0, mw, mh, sdc, mon.left, mon.top, SRCCOPY | CAPTUREBLT);
    GdiFlush();
    int bw = (mw + BD_DIV - 1) / BD_DIV, bh = (mh + BD_DIV - 1) / BD_DIV;
    std::vector<DWORD> a((size_t)bw * bh, 0);
    for (int by = 0; by < bh; by++) {
        for (int bx = 0; bx < bw; bx++) {
            int sr = 0, sg = 0, sb = 0, cnt = 0;
            for (int j = 0; j < BD_DIV; j++) {
                int yy = by * BD_DIV + j;
                if (yy >= mh) break;
                for (int i = 0; i < BD_DIV; i++) {
                    int xx = bx * BD_DIV + i;
                    if (xx >= mw) break;
                    DWORD p = px[(size_t)yy * mw + xx];
                    sr += (p >> 16) & 255; sg += (p >> 8) & 255; sb += p & 255;
                    cnt++;
                }
            }
            if (!cnt) cnt = 1;
            a[(size_t)by * bw + bx] =
                0xFF000000u | ((DWORD)(sr / cnt) << 16) | ((DWORD)(sg / cnt) << 8) | (DWORD)(sb / cnt);
        }
    }
    SelectObject(mdc, old);
    DeleteObject(bmp);
    DeleteDC(mdc);
    ReleaseDC(nullptr, sdc);

    int r = S.frostBlur / BD_DIV;
    if (S.frostBlur > 0 && r < 1) r = 1;
    if (r > 0) {
        std::vector<DWORD> b((size_t)bw * bh, 0);
        for (int it = 0; it < 2; it++) {
            BlurPass(a, b, bw, bh, r, true);
            BlurPass(b, a, bw, bh, r, false);
        }
    }
    g_bd.swap(a);
    g_bdW = bw;
    g_bdH = bh;
    g_bdX = mon.left;
    g_bdY = mon.top;
}

// -------------------------------------------------------------- buffers ---

static void FreeBuffers() {
    if (g_srcDc) {
        SelectObject(g_srcDc, g_oldSrc);
        DeleteDC(g_srcDc);
    }
    if (g_dstDc) {
        SelectObject(g_dstDc, g_oldDst);
        DeleteDC(g_dstDc);
    }
    if (g_srcBmp) DeleteObject(g_srcBmp);
    if (g_dstBmp) DeleteObject(g_dstBmp);
    g_srcDc = g_dstDc = nullptr;
    g_srcBmp = g_dstBmp = nullptr;
    g_srcPx = g_dstPx = nullptr;
    g_ww = g_wh = 0;
}

static void EnsureBuffers(int ww, int wh) {
    if (g_srcBmp && g_dstBmp && ww == g_ww && wh == g_wh) return;
    FreeBuffers();
    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = ww;
    bi.bmiHeader.biHeight = -wh;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    g_srcDc = CreateCompatibleDC(nullptr);
    g_dstDc = CreateCompatibleDC(nullptr);
    g_srcBmp = CreateDIBSection(g_srcDc, &bi, DIB_RGB_COLORS, (void**)&g_srcPx, nullptr, 0);
    g_dstBmp = CreateDIBSection(g_dstDc, &bi, DIB_RGB_COLORS, (void**)&g_dstPx, nullptr, 0);
    if (!g_srcBmp || !g_dstBmp) {
        FreeBuffers();
        return;
    }
    g_oldSrc = SelectObject(g_srcDc, g_srcBmp);
    g_oldDst = SelectObject(g_dstDc, g_dstBmp);
    g_ww = ww;
    g_wh = wh;
}

// --------------------------------------------------------------- window ---

// Applies DPI * bar size * hover zoom; rebuilds fonts when the scale changed.
static void ApplyScale() {
    double s = g_dpi * S.barSize / 100.0 * g_hov;
    g_scale = s;
    if (std::fabs(s - g_scaleBuilt) < 1e-6 && g_fQuery) return;
    g_scaleBuilt = s;
    if (g_fQuery) DeleteObject(g_fQuery);
    if (g_fName) DeleteObject(g_fName);
    if (g_fSub) DeleteObject(g_fSub);
    if (g_fIcon) DeleteObject(g_fIcon);
    if (g_fSearch) DeleteObject(g_fSearch);
    auto mk = [](int h, int w, const wchar_t* face) {
        return CreateFontW(-h, 0, 0, 0, w, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                           CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH, face);
    };
    g_fQuery = mk(Sc(24), FW_LIGHT, L"Segoe UI");
    g_fName = mk(Sc(15), FW_NORMAL, L"Segoe UI");
    g_fSub = mk(Sc(12), FW_NORMAL, L"Segoe UI");
    g_fIcon = mk(Sc(18), FW_NORMAL, L"Segoe MDL2 Assets");
    // Bold weight on a font without a bold face makes GDI thicken the strokes (thicker magnifier).
    g_fSearch = mk(Sc(19), FW_BOLD, L"Segoe MDL2 Assets");
}

static int WinH() {
    int h = Sc(SEARCH_H);
    if (!g_items.empty()) h += Sc(1) + Sc(PAD) + (int)g_items.size() * Sc(ROW_H) + Sc(PAD);
    return h;
}

// ---------------------------------------------------------- text editing ---

static bool HasSel() { return g_caret != g_anchor; }
static size_t SelLo() { return std::min(g_caret, g_anchor); }
static size_t SelHi() { return std::max(g_caret, g_anchor); }

static void SetQuery(const std::wstring& q) {
    g_query = q;
    g_caret = g_anchor = g_query.size();
}

static bool DeleteSel() {
    if (!HasSel()) return false;
    size_t lo = SelLo(), hi = SelHi();
    g_query.erase(lo, hi - lo);
    g_caret = g_anchor = lo;
    return true;
}

static void InsertText(const std::wstring& t) {
    DeleteSel();
    size_t room = g_query.size() < 80 ? 80 - g_query.size() : 0;
    std::wstring s = t.substr(0, room);
    g_query.insert(g_caret, s);
    g_caret += s.size();
    g_anchor = g_caret;
}

static std::wstring SelText() {
    return HasSel() ? g_query.substr(SelLo(), SelHi() - SelLo()) : std::wstring();
}

// Width in pixels of the first n characters of the query in the query font.
static int TextW(size_t n) {
    if (!n || !g_srcDc || !g_fQuery) return 0;
    n = std::min(n, g_query.size());
    HGDIOBJ old = SelectObject(g_srcDc, g_fQuery);
    SIZE sz{};
    GetTextExtentPoint32W(g_srcDc, g_query.c_str(), (int)n, &sz);
    SelectObject(g_srcDc, old);
    return sz.cx;
}

// Caret index nearest to a panel-relative x coordinate.
static size_t CaretFromX(int px) {
    int rel = px - Sc(62);
    size_t n = g_query.size();
    int prev = 0;
    for (size_t i = 0; i < n; i++) {
        int next = TextW(i + 1);
        if (rel < (prev + next) / 2) return i;
        prev = next;
    }
    return n;
}

static bool HasEndIcon() {
    return S.endIcon && !g_query.empty() && !g_items.empty() &&
           ItemIcon(g_items[g_sel]) != nullptr;
}

// The clickable text field (panel-relative). Clicks here select text; drag-to-move uses the rest.
static RECT TextBox() {
    return {Sc(58), Sc(8), g_pw - Sc(DRAG_ZONE), Sc(SEARCH_H) - Sc(8)};
}

static bool InTextBox(int px, int py) {
    RECT r = TextBox();
    POINT p = {px, py};
    return PtInRect(&r, p) != FALSE;
}

// ------------------------------------------------------------- drawing ---

static inline double Sdf(double px, double py, double hx, double hy, double r) {
    double qx = std::fabs(px - hx) - (hx - r), qy = std::fabs(py - hy) - (hy - r);
    if (qx <= 0 && qy <= 0) return std::max(qx, qy) - r;
    return std::hypot(std::max(qx, 0.0), std::max(qy, 0.0)) - r;
}

static void BuildShadow(std::vector<DWORD>& out, int ww, int wh, int M, int W, int H, double r) {
    out.assign((size_t)ww * wh, 0);
    double hx = W / 2.0, hy = H / 2.0;
    double oy = Sc(8), range = M * 0.95;
    for (int y = 0; y < wh; y++) {
        for (int x = 0; x < ww; x++) {
            double px = x + 0.5 - M, py = y + 0.5 - M;
            if (Sdf(px, py, hx, hy, r) < -0.5) continue;
            double d1 = Sdf(px, py - oy, hx, hy, r);
            double t = std::max(d1, 0.0) / range;
            if (t >= 1.0) continue;
            double u = 1.0 - t;
            DWORD a = (DWORD)(85.0 * u * u * u);
            out[(size_t)y * ww + x] = a << 24;
        }
    }
}

// Background: frosted (blurred desktop + white veil) or plain tinted glass.
static void PaintBg(DWORD* P, int stride, int W, int H, int ox, int oy, int rad) {
    const Pal& pl = g_pal;
    double r = (double)rad, hx = W / 2.0, hy = H / 2.0, bx = hx - r, by = hy - r;
    double bw = std::max(1.0, (double)Sc(1));
    int tr = GetRValue(pl.top), tg = GetGValue(pl.top), tb = GetBValue(pl.top);
    int br = GetRValue(pl.bot), bg = GetGValue(pl.bot), bb = GetBValue(pl.bot);
    int er = GetRValue(pl.edge), eg = GetGValue(pl.edge), eb = GetBValue(pl.edge);
    int baseA = S.opacity;
    bool frost = S.frost && !g_bd.empty();
    int wv = Clampi(S.frostWhite, 0, 100) * 255 / 100;
    for (int y = 0; y < H; y++) {
        int f = H > 1 ? y * 256 / (H - 1) : 0;
        int cr = (tr * (256 - f) + br * f) >> 8;
        int cg = (tg * (256 - f) + bg * f) >> 8;
        int cb = (tb * (256 - f) + bb * f) >> 8;
        double qy = std::fabs(y + 0.5 - hy) - by;
        int ya = 0, yb = 0, fy = 0;
        int sheen = 0;
        if (frost) {
            double v = (oy + y + 0.5 - g_bdY) / BD_DIV - 0.5;
            int y0 = (int)std::floor(v);
            fy = (int)((v - y0) * 256);
            ya = Clampi(y0, 0, g_bdH - 1);
            yb = Clampi(y0 + 1, 0, g_bdH - 1);
            double ft = 1.0 - (double)y / std::max(1, H);
            sheen = (int)(28.0 * ft * ft);
        }
        for (int x = 0; x < W; x++) {
            double qx = std::fabs(x + 0.5 - hx) - bx;
            double d;
            if (qx <= 0 && qy <= 0) d = std::max(qx, qy) - r;
            else d = std::hypot(std::max(qx, 0.0), std::max(qy, 0.0)) - r;
            if (d >= 0.5) continue;
            double cov = d <= -0.5 ? 1.0 : 0.5 - d;
            int fr = cr, fg = cg, fb = cb, a;
            if (frost) {
                double u = (ox + x + 0.5 - g_bdX) / BD_DIV - 0.5;
                int x0 = (int)std::floor(u);
                int fx = (int)((u - x0) * 256);
                int xa = Clampi(x0, 0, g_bdW - 1), xb = Clampi(x0 + 1, 0, g_bdW - 1);
                DWORD p00 = g_bd[(size_t)ya * g_bdW + xa], p10 = g_bd[(size_t)ya * g_bdW + xb];
                DWORD p01 = g_bd[(size_t)yb * g_bdW + xa], p11 = g_bd[(size_t)yb * g_bdW + xb];
                int w00 = (256 - fx) * (256 - fy), w10 = fx * (256 - fy);
                int w01 = (256 - fx) * fy, w11 = fx * fy;
                int c3[3];
                for (int k = 0; k < 3; k++) {
                    int sh = 16 - 8 * k;
                    c3[k] = (int)(((p00 >> sh) & 255) * w00 + ((p10 >> sh) & 255) * w10 +
                                  ((p01 >> sh) & 255) * w01 + ((p11 >> sh) & 255) * w11) >> 16;
                }
                int t1 = baseA;
                int rr = c3[0] + (cr - c3[0]) * t1 / 255;
                int gg = c3[1] + (cg - c3[1]) * t1 / 255;
                int bl = c3[2] + (cb - c3[2]) * t1 / 255;
                rr += (255 - rr) * wv / 255;
                gg += (255 - gg) * wv / 255;
                bl += (255 - bl) * wv / 255;
                rr += (255 - rr) * sheen / 255;
                gg += (255 - gg) * sheen / 255;
                bl += (255 - bl) * sheen / 255;
                unsigned hxn = (unsigned)x * 374761393u + (unsigned)y * 668265263u;
                hxn = (hxn ^ (hxn >> 13)) * 1274126177u;
                int nz = (int)(hxn >> 29) - 3;
                fr = Clampi(rr + nz, 0, 255);
                fg = Clampi(gg + nz, 0, 255);
                fb = Clampi(bl + nz, 0, 255);
                a = (int)(255.0 * cov + 0.5);
            } else {
                a = (int)(baseA * cov + 0.5);
            }
            int R = fr * a / 255, G = fg * a / 255, B = fb * a / 255;
            if (S.border && d > -bw - 0.5) {
                double inner = std::min(1.0, d + bw + 0.5);
                int ba = (int)(110.0 * cov * inner);
                int ia = 255 - ba;
                R = er * ba / 255 + R * ia / 255;
                G = eg * ba / 255 + G * ia / 255;
                B = eb * ba / 255 + B * ia / 255;
                a = ba + a * ia / 255;
            }
            DWORD& dstp = P[(size_t)y * stride + x];
            DWORD dst = dstp;
            int ida = 255 - a;
            int oa = std::min(255, a + (int)(dst >> 24) * ida / 255);
            int orr = std::min(255, R + (int)((dst >> 16) & 255) * ida / 255);
            int og = std::min(255, G + (int)((dst >> 8) & 255) * ida / 255);
            int ob = std::min(255, B + (int)(dst & 255) * ida / 255);
            dstp = ((DWORD)oa << 24) | ((DWORD)orr << 16) | ((DWORD)og << 8) | (DWORD)ob;
        }
    }
}

static void FillRR(DWORD* px, int stride, int W, int H, int x0, int y0, int w, int h, int r,
                   COLORREF c, int a) {
    int cr = GetRValue(c), cg = GetGValue(c), cb = GetBValue(c);
    double hx = x0 + w / 2.0, hy = y0 + h / 2.0;
    double rr = std::min((double)r, std::min(w, h) / 2.0);
    for (int y = std::max(0, y0); y < std::min(H, y0 + h); y++) {
        for (int x = std::max(0, x0); x < std::min(W, x0 + w); x++) {
            double qx = std::fabs(x + 0.5 - hx) - (w / 2.0 - rr);
            double qy = std::fabs(y + 0.5 - hy) - (h / 2.0 - rr);
            double d = std::hypot(std::max(qx, 0.0), std::max(qy, 0.0)) +
                       std::min(std::max(qx, qy), 0.0) - rr;
            double cov = std::min(1.0, std::max(0.0, 0.5 - d));
            if (cov <= 0) continue;
            int sa = (int)(a * cov);
            DWORD dst = px[(size_t)y * stride + x];
            int da = dst >> 24, dr = (dst >> 16) & 255, dg = (dst >> 8) & 255, db = dst & 255;
            int ia = 255 - sa;
            int oa = sa + da * ia / 255;
            int orr = cr * sa / 255 + dr * ia / 255;
            int og = cg * sa / 255 + dg * ia / 255;
            int ob = cb * sa / 255 + db * ia / 255;
            px[(size_t)y * stride + x] =
                ((DWORD)oa << 24) | ((DWORD)orr << 16) | ((DWORD)og << 8) | (DWORD)ob;
        }
    }
}

static void Txt(HDC dc, HFONT f, const std::wstring& s, RECT r, COLORREF col, UINT flags) {
    if (!g_theme || s.empty()) return;
    OffsetRect(&r, g_off, g_off);
    SelectObject(dc, f);
    DTTOPTS o{};
    o.dwSize = sizeof(o);
    o.dwFlags = DTT_COMPOSITED | DTT_TEXTCOLOR;
    o.crText = col;
    DrawThemeTextEx(g_theme, dc, 0, 0, s.c_str(), (int)s.size(), flags, &r, &o);
}

static const wchar_t* GlyphFor(const Item& it) {
    switch (it.cat) {
        case C_CALC: return L"\uE8EF";
        case C_CMD: return L"\uE756";
        case C_SITE: return L"\uE71B";
        case C_SEARCH: return L"\uE774";
        case C_FOLDER: return L"\uE8B7";
        case C_CLIP: return L"\uE77F";
    }
    return nullptr;
}

static void RenderPanel() {
    int W = g_pw, H = g_ph, M = Mg();
    int ww = W + 2 * M, wh = H + 2 * M;
    EnsureBuffers(ww, wh);
    if (!g_srcPx) return;

    size_t total = (size_t)ww * wh;
    if (S.shadow) {
        int key[5] = {W, H, g_rad, M, ww};
        bool same = g_shadow.size() == total && memcmp(key, g_shKey, sizeof(key)) == 0;
        if (!same) {
            BuildShadow(g_shadow, ww, wh, M, W, H, (double)g_rad);
            memcpy(g_shKey, key, sizeof(key));
        }
        memcpy(g_srcPx, g_shadow.data(), total * 4);
    } else {
        memset(g_srcPx, 0, total * 4);
    }

    g_off = M;
    DWORD* P = g_srcPx + (size_t)M * ww + M;
    PaintBg(P, ww, W, H, g_panelX, g_panelY, g_rad);

    HDC mdc = g_srcDc;
    SetBkMode(mdc, TRANSPARENT);
    const Pal& pl = g_pal;
    COLORREF txt = pl.text, dim = pl.dim, ghostC = pl.ghost, line = pl.hi;

    // search glyph (bold = thicker strokes)
    RECT ri = {Sc(20), 0, Sc(56), Sc(SEARCH_H)};
    Txt(mdc, g_fSearch, L"\uE721", ri, dim, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);

    // app icon at the end of the search field
    int esz = Sc(30);
    bool hasEnd = HasEndIcon();

    int tx = Sc(62), maxR = W - Sc(DRAG_ZONE);
    RECT rq = {tx, 0, maxR, Sc(SEARCH_H)};
    UINT tf = DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX | DT_END_ELLIPSIS;
    int tw = 0;
    if (g_query.empty()) {
        Txt(mdc, g_fQuery, Placeholder(), rq, dim, tf);
    } else {
        if (HasSel()) {
            int x0 = tx + TextW(SelLo()), x1 = std::min(tx + TextW(SelHi()), maxR);
            if (x1 > x0)
                FillRR(P, ww, W, H, x0, Sc(16), x1 - x0, Sc(32), Sc(4), RGB(0, 120, 215), 120);
        }
        Txt(mdc, g_fQuery, g_query, rq, txt, tf);
        tw = TextW(g_query.size());
        if (!g_ghost.empty() && !HasSel() && g_caret == g_query.size() && tx + tw < maxR) {
            RECT rg = {tx + tw, 0, maxR, Sc(SEARCH_H)};
            Txt(mdc, g_fQuery, g_ghost, rg, ghostC, tf);
        }
    }
    if (!g_calcInline.empty() && !g_query.empty() && tx + tw < maxR) {
        RECT rc = {tx + tw + Sc(10), 0, maxR, Sc(SEARCH_H)};
        Txt(mdc, g_fQuery, g_calcInline, rc, ghostC, tf);
    }
    if (g_caretOn && !HasSel()) {
        int cx = std::min(tx + TextW(g_caret) + Sc(1), maxR);
        FillRR(P, ww, W, H, cx, Sc(17), std::max(2, Sc(2)), Sc(30), 0, txt, 230);
    }
    if (hasEnd) {
        const IconPx* eic = ItemIcon(g_items[g_sel]);
        if (eic) BlitIcon(P, ww, W, H, W - Sc(22) - esz, (Sc(SEARCH_H) - esz) / 2, esz, *eic);
    }

    if (!g_items.empty()) {
        FillRR(P, ww, W, H, Sc(16), Sc(SEARCH_H), W - Sc(32), std::max(1, Sc(1)), 0, line, 36);
        int top = Sc(SEARCH_H) + Sc(1) + Sc(PAD);
        int isz = Sc(ICON_SZ);
        int textX = S.icons ? Sc(20) + isz + Sc(14) : Sc(24);
        for (size_t i = 0; i < g_items.size(); i++) {
            const Item& it = g_items[i];
            int y = top + (int)i * Sc(ROW_H);
            bool isSel = (int)i == g_sel;
            if (isSel)
                FillRR(P, ww, W, H, Sc(8), y, W - Sc(16), Sc(ROW_H), Sc(12), line,
                       pl.dark ? 42 : 30);

            if (S.icons) {
                int ix = Sc(20), iy = y + (Sc(ROW_H) - isz) / 2;
                const IconPx* ic = ItemIcon(it);
                if (ic) {
                    BlitIcon(P, ww, W, H, ix, iy, isz, *ic);
                } else {
                    FillRR(P, ww, W, H, ix, iy, isz, isz, Sc(9), line, pl.dark ? 38 : 24);
                    RECT rt = {ix, iy, ix + isz, iy + isz};
                    const wchar_t* gl = GlyphFor(it);
                    if (gl) {
                        Txt(mdc, g_fIcon, gl, rt, txt, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
                    } else if (!it.name.empty()) {
                        Txt(mdc, g_fName, std::wstring(1, (wchar_t)towupper(it.name[0])), rt,
                            txt, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
                    }
                }
            }
            bool showTab = isSel && it.cat == C_APP && !g_ghost.empty();
            bool showX = isSel && it.cat == C_RECENT;
            int rightPad = (showTab || showX) ? Sc(76) : Sc(24);
            UINT f = DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX | DT_END_ELLIPSIS;
            RECT rn = {textX, y + Sc(8), W - rightPad, y + Sc(30)};
            RECT rs = {textX, y + Sc(29), W - rightPad, y + Sc(46)};
            Txt(mdc, g_fName, it.name, rn, txt, f);
            Txt(mdc, g_fSub, it.sub, rs, dim, f);
            if (showTab) {
                int pw = Sc(36), ph = Sc(22);
                int px0 = W - Sc(20) - pw, py0 = y + (Sc(ROW_H) - ph) / 2;
                FillRR(P, ww, W, H, px0, py0, pw, ph, Sc(7), line, pl.dark ? 60 : 36);
                RECT rt = {px0, py0, px0 + pw, py0 + ph};
                Txt(mdc, g_fSub, L"tab", rt, dim, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
            }
            if (showX) {
                RECT rx = {W - Sc(56), y, W - Sc(18), y + Sc(ROW_H)};
                Txt(mdc, g_fIcon, L"\uE711", rx, dim, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
            }
        }
    }
    GdiFlush();
}

// Scales the source picture about its centre, fades it, offsets it and hands it to the window.
static void Present(double sx, double sy, double alpha, double tx, double ty) {
    if (!g_hwnd || !g_srcPx || !g_dstPx) return;
    int ww = g_ww, wh = g_wh;
    size_t n = (size_t)ww * wh;
    int ka = Clampi((int)(alpha * 256 + 0.5), 0, 256);

    if (ka <= 0) {
        memset(g_dstPx, 0, n * 4);
    } else if (std::fabs(sx - 1) < 0.002 && std::fabs(sy - 1) < 0.002) {
        if (ka >= 256) {
            memcpy(g_dstPx, g_srcPx, n * 4);
        } else {
            for (size_t i = 0; i < n; i++) {
                DWORD v = g_srcPx[i];
                if (!v) { g_dstPx[i] = 0; continue; }
                DWORD a = ((v >> 24) * ka) >> 8, r = (((v >> 16) & 255) * ka) >> 8;
                DWORD g = (((v >> 8) & 255) * ka) >> 8, b = ((v & 255) * ka) >> 8;
                g_dstPx[i] = (a << 24) | (r << 16) | (g << 8) | b;
            }
        }
    } else {
        double cx = ww / 2.0, cy = wh / 2.0;
        static std::vector<int> xi, xf;
        xi.resize(ww);
        xf.resize(ww);
        for (int x = 0; x < ww; x++) {
            double s = (x + 0.5 - cx) / sx + cx - 0.5;
            double fl = std::floor(s);
            xi[x] = (int)fl;
            xf[x] = (int)((s - fl) * 256);
        }
        auto G = [&](int xx, int yy) -> DWORD {
            return (xx < 0 || yy < 0 || xx >= ww || yy >= wh) ? 0 : g_srcPx[(size_t)yy * ww + xx];
        };
        for (int y = 0; y < wh; y++) {
            DWORD* o = g_dstPx + (size_t)y * ww;
            double s = (y + 0.5 - cy) / sy + cy - 0.5;
            double fl = std::floor(s);
            int yi = (int)fl, fy = (int)((s - fl) * 256);
            if (yi < -1 || yi >= wh) { memset(o, 0, (size_t)ww * 4); continue; }
            for (int x = 0; x < ww; x++) {
                int x0 = xi[x], fx = xf[x];
                if (x0 < -1 || x0 >= ww) { o[x] = 0; continue; }
                DWORD p00 = G(x0, yi), p10 = G(x0 + 1, yi), p01 = G(x0, yi + 1), p11 = G(x0 + 1, yi + 1);
                if (!(p00 | p10 | p01 | p11)) { o[x] = 0; continue; }
                int w00 = (256 - fx) * (256 - fy), w10 = fx * (256 - fy);
                int w01 = (256 - fx) * fy, w11 = fx * fy;
                int ch[4];
                for (int k = 0; k < 4; k++) {
                    int sh = 24 - 8 * k;
                    int v = (int)((p00 >> sh) & 255) * w00 + (int)((p10 >> sh) & 255) * w10 +
                            (int)((p01 >> sh) & 255) * w01 + (int)((p11 >> sh) & 255) * w11;
                    v >>= 16;
                    ch[k] = (v * ka) >> 8;
                }
                o[x] = ((DWORD)ch[0] << 24) | ((DWORD)ch[1] << 16) | ((DWORD)ch[2] << 8) | (DWORD)ch[3];
            }
        }
    }

    POINT pos = {g_panelX - Mg() + (int)std::lround(tx), g_panelY - Mg() + (int)std::lround(ty)};
    SIZE size = {ww, wh};
    POINT src = {0, 0};
    BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    HDC sdc = GetDC(nullptr);
    UpdateLayeredWindow(g_hwnd, sdc, &pos, &size, g_dstDc, &src, 0, &bf, ULW_ALPHA);
    ReleaseDC(nullptr, sdc);
}

static void PresentCur() { Present(g_curSx, g_curSy, g_curA, g_curTx, g_curTy); }

static void ResetCur() {
    g_curSx = g_curSy = g_curA = 1.0;
    g_curTx = g_curTy = 0.0;
}

static void Redraw() {
    RenderPanel();
    PresentCur();
}

// Places the panel from the current anchor, kept inside the work area.
static void PlacePanel() {
    int x = g_baseCx - g_pw / 2;
    int y = g_baseTop;
    int maxX = std::max((int)g_work.left, (int)g_work.right - g_pw);
    x = Clampi(x, (int)g_work.left, maxX);
    y = std::max(y, (int)g_work.top);
    g_panelX = x;
    g_panelY = y;
}

static void Layout() {
    ApplyScale();
    g_pw = Sc(LAYOUT_W);
    g_ph = WinH();
    g_rad = std::min(Sc(S.radius), g_ph / 2);
    PlacePanel();
    int M = Mg();
    SetWindowPos(g_hwnd, nullptr, g_panelX - M, g_panelY - M, g_pw + 2 * M, g_ph + 2 * M,
                 SWP_NOZORDER | SWP_NOACTIVATE);
    Redraw();
}

// Anchor (centre-x, top) from the position setting, relative to a work area.
static void ComputeBase(const RECT& wk) {
    int ww = wk.right - wk.left, wh = wk.bottom - wk.top;
    int barH = (int)(SEARCH_H * g_dpi * S.barSize / 100.0 + 0.5);
    double fx = 0.5;
    int top;
    if (S.position == L"center") {
        top = wk.top + wh / 2 - barH / 2;
    } else if (S.position == L"top") {
        top = wk.top + wh * 6 / 100;
    } else if (S.position == L"custom") {
        fx = S.customX / 100.0;
        top = wk.top + (int)(wh * S.customY / 100.0) - barH / 2;
    } else {
        top = wk.top + wh * 22 / 100;
    }
    g_origCx = wk.left + (int)(ww * fx);
    g_origTop = top;
}

// ------------------------------------------------------------ animation ---

static int StyleOf(const std::wstring& m) {
    if (m == L"fade") return ST_FADE;
    if (m == L"jelly") return ST_JELLY;
    if (m == L"pop") return ST_POP;
    if (m == L"slide") return ST_SLIDE;
    if (m == L"zoom") return ST_ZOOM;
    if (m == L"bounce") return ST_BOUNCE;
    if (m == L"elastic") return ST_ELASTIC;
    if (m == L"rubber") return ST_RUBBER;
    return ST_NONE;
}

static int PulseOf(const std::wstring& m) {
    if (m == L"jelly") return PU_JELLY;
    if (m == L"pop") return PU_POP;
    if (m == L"shake") return PU_SHAKE;
    if (m == L"bounce") return PU_BOUNCE;
    return PU_NONE;
}

static bool DecideAnim(bool opening, int& style, int& dur) {
    style = StyleOf(S.animation);
    if (style == ST_NONE || S.animMs <= 0) return false;
    dur = S.animMs;
    if (!opening) dur = dur * 65 / 100;
    return dur > 0;
}

static double EaseOutCubic(double t) { double u = 1.0 - t; return 1.0 - u * u * u; }
static double EaseOutBack(double t, double s) {
    double c3 = s + 1.0, u = t - 1.0;
    return 1.0 + c3 * u * u * u + s * u * u;
}
static double EaseOutBounce(double t) {
    const double n1 = 7.5625, d1 = 2.75;
    if (t < 1.0 / d1) return n1 * t * t;
    if (t < 2.0 / d1) { t -= 1.5 / d1; return n1 * t * t + 0.75; }
    if (t < 2.5 / d1) { t -= 2.25 / d1; return n1 * t * t + 0.9375; }
    t -= 2.625 / d1;
    return n1 * t * t + 0.984375;
}
static double EaseOutElastic(double t) {
    const double PI = 3.14159265358979323846;
    if (t <= 0) return 0;
    if (t >= 1) return 1;
    return std::pow(2.0, -10.0 * t) * std::sin((t * 10.0 - 0.75) * (2.0 * PI / 3.0)) + 1.0;
}

static void ComputeFrame(bool opening, double t, int style, double& sx, double& sy, double& a,
                         double& tx, double& ty) {
    const double PI = 3.14159265358979323846;
    double k = S.jiggle / 100.0;
    double amp = 0.4 + k;
    sx = sy = 1.0;
    tx = ty = 0.0;
    a = 1.0;
    if (opening) {
        double e = EaseOutCubic(t);
        switch (style) {
            case ST_FADE:
                a = e;
                break;
            case ST_JELLY: {
                double g = 1.0 - std::exp(-7.0 * t);
                double base = 1.0 - 0.25 * k * (1.0 - g);
                double w = 0.14 * k * std::exp(-4.5 * t) * std::sin(2.0 * PI * 2.2 * t);
                sx = base + w;
                sy = base - w;
                double u = 1.0 - std::min(1.0, t * 2.5);
                a = 1.0 - u * u;
                break;
            }
            case ST_POP: {
                double st = 1.0 - 0.3 * k;
                sx = sy = st + (1.0 - st) * EaseOutBack(t, 1.2 + 2.0 * k);
                a = std::min(1.0, t * 3.0);
                break;
            }
            case ST_SLIDE:
                ty = -40.0 * amp * (1.0 - e) * g_scale;
                a = e;
                break;
            case ST_ZOOM:
                sx = sy = 1.0 - 0.18 * (0.4 + k) * (1.0 - e);
                a = e;
                break;
            case ST_BOUNCE:
                ty = -110.0 * amp * (1.0 - EaseOutBounce(t)) * g_scale;
                a = std::min(1.0, t * 4.0);
                break;
            case ST_ELASTIC: {
                double el = EaseOutElastic(t);
                sx = 1.0 - (0.35 + 0.4 * k) * (1.0 - el);
                sy = 1.0 - (0.12 + 0.2 * k) * (1.0 - el);
                a = std::min(1.0, t * 4.0);
                break;
            }
            case ST_RUBBER: {
                double c = std::exp(-4.5 * t) * std::cos(2.0 * PI * 2.4 * t);
                sx = 1.0 - 0.45 * k * c;
                sy = 1.0 + 0.30 * k * c;
                a = std::min(1.0, t * 3.5);
                break;
            }
        }
    } else {
        double e = t * t;
        double fa = 1.0 - std::pow(t, 1.5);
        switch (style) {
            case ST_FADE:
                a = fa;
                break;
            case ST_JELLY: {
                double base = 1.0 + 0.05 * k * std::sin(PI * std::min(1.0, t / 0.35)) - 0.25 * k * e;
                double w = 0.06 * k * std::sin(4.0 * PI * t) * (1.0 - t);
                sx = base + w;
                sy = base - w;
                a = fa;
                break;
            }
            case ST_POP:
                sx = sy = 1.0 + 0.05 * k * std::sin(PI * std::min(1.0, t / 0.3)) - 0.3 * k * e;
                a = fa;
                break;
            case ST_SLIDE:
                ty = -30.0 * amp * e * g_scale;
                a = 1.0 - t;
                break;
            case ST_ZOOM:
                sx = sy = 1.0 - 0.15 * (0.4 + k) * e;
                a = 1.0 - t;
                break;
            case ST_BOUNCE:
                ty = -60.0 * amp * e * t * g_scale;
                a = 1.0 - t;
                break;
            case ST_ELASTIC: {
                double w = 0.06 * k * std::sin(3.0 * PI * t) * (1.0 - t);
                sx = 1.0 - 0.3 * k * e + w;
                sy = 1.0 - 0.3 * k * e - w;
                a = fa;
                break;
            }
            case ST_RUBBER:
                sx = 1.0 - 0.4 * k * e;
                sy = 1.0 + 0.12 * k * std::sin(PI * t);
                a = 1.0 - t;
                break;
        }
    }
}

static void ComputePulse(double t, int style, double& sx, double& sy, double& tx, double& ty) {
    const double PI = 3.14159265358979323846;
    double k = S.jiggle / 100.0;
    double dec = std::exp(-5.0 * t);
    sx = sy = 1.0;
    tx = ty = 0.0;
    switch (style) {
        case PU_JELLY: {
            double w = 0.10 * k * dec * std::sin(2.0 * PI * 2.5 * t);
            sx = 1.0 + w;
            sy = 1.0 - w;
            break;
        }
        case PU_POP:
            sx = sy = 1.0 + 0.09 * k * std::sin(PI * t);
            break;
        case PU_SHAKE:
            tx = 10.0 * k * dec * std::sin(2.0 * PI * 3.0 * t) * g_scale;
            break;
        case PU_BOUNCE:
            ty = -14.0 * k * dec * std::fabs(std::sin(2.0 * PI * 1.5 * t)) * g_scale;
            break;
    }
}

static void DoHide() {
    if (!g_hwnd) return;
    CloseMenu();
    KillTimer(g_hwnd, TIMER_ANIM);
    KillTimer(g_hwnd, TIMER_HOVER);
    KillTimer(g_hwnd, TIMER_IDLE);
    g_anim = ANIM_NONE;
    g_idleFade = false;
    g_dragging = false;
    g_selecting = false;
    if (GetCapture() == g_hwnd) ReleaseCapture();
    ResetCur();
    ShowWindow(g_hwnd, SW_HIDE);
    std::vector<DWORD>().swap(g_bd);  // drop the desktop snapshot while the bar is hidden
}

static void FinishAnim() {
    KillTimer(g_hwnd, TIMER_ANIM);
    bool closing = (g_anim == ANIM_CLOSE);
    g_anim = ANIM_NONE;
    g_idleFade = false;
    ResetCur();
    if (closing) DoHide();
    else PresentCur();
}

static void TickAnim() {
    if (g_anim == ANIM_NONE) return;
    double t = std::min(1.0, (double)(GetTickCount64() - g_animStart) / std::max(1, g_animDur));
    if (t >= 1.0) { FinishAnim(); return; }
    if (g_anim == ANIM_PULSE) {
        g_curA = 1.0;
        ComputePulse(t, PulseOf(S.switchAnim), g_curSx, g_curSy, g_curTx, g_curTy);
    } else {
        ComputeFrame(g_anim == ANIM_OPEN, t, g_animStyle, g_curSx, g_curSy, g_curA, g_curTx, g_curTy);
    }
    PresentCur();
}

static void StartPulse() {
    if (g_anim != ANIM_NONE && g_anim != ANIM_PULSE) return;
    if (PulseOf(S.switchAnim) == PU_NONE || S.switchMs <= 0) return;
    g_anim = ANIM_PULSE;
    g_animDur = S.switchMs;
    g_animStart = GetTickCount64();
    SetTimer(g_hwnd, TIMER_ANIM, 10, nullptr);
}

static void HideWnd(bool idle = false) {
    if (!IsWindowVisible(g_hwnd) || g_anim == ANIM_CLOSE) return;
    CloseMenu();
    int style = ST_FADE, dur = 0;
    bool animate;
    if (idle) {
        dur = S.idleFadeMs;
        animate = dur > 0;
    } else {
        animate = DecideAnim(false, style, dur);
    }
    g_dragging = false;
    g_selecting = false;
    if (GetCapture() == g_hwnd) ReleaseCapture();
    if (!animate) { DoHide(); return; }
    g_idleFade = idle;
    g_animStyle = style;
    g_animDur = dur;
    g_anim = ANIM_CLOSE;
    g_animStart = GetTickCount64();
    SetTimer(g_hwnd, TIMER_ANIM, 10, nullptr);
}

static void CancelIdleFade() {
    if (g_anim == ANIM_CLOSE && g_idleFade) {
        KillTimer(g_hwnd, TIMER_ANIM);
        g_anim = ANIM_NONE;
        g_idleFade = false;
        ResetCur();
        PresentCur();
    }
}

static void NoteInput() {
    g_lastInput = GetTickCount64();
    CancelIdleFade();
}

static void HoverTick() {
    if (!IsWindowVisible(g_hwnd)) return;
    POINT pt;
    GetCursorPos(&pt);
    bool inside = pt.x >= g_panelX && pt.x < g_panelX + g_pw && pt.y >= g_panelY &&
                  pt.y < g_panelY + g_ph;
    bool moved = pt.x != g_lastPt.x || pt.y != g_lastPt.y;
    g_lastPt = pt;
    if (inside && moved) NoteInput();
    if (g_anim == ANIM_OPEN || g_anim == ANIM_CLOSE) return;

    double target = 1.0;
    if ((inside || g_dragging || g_selecting) && S.hoverPct > 100) {
        target = S.hoverPct / 100.0;
        double unit = LAYOUT_W * g_dpi * S.barSize / 100.0;
        double maxH = (double)(g_work.right - g_work.left) * 0.96 / std::max(1.0, unit);
        target = std::min(target, std::max(1.0, maxH));
    }
    if (std::fabs(g_hov - target) < 0.0005) return;
    g_hov += (target - g_hov) * 0.28;
    if (std::fabs(g_hov - target) < 0.004) g_hov = target;
    Layout();
}

static void IdleTick() {
    if (!IsWindowVisible(g_hwnd)) return;
    if (S.onTop && !g_menuOpen && g_anim != ANIM_CLOSE)
        SetWindowPos(g_hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    if (g_menuOpen) { g_lastInput = GetTickCount64(); return; }
    if (S.idleSec > 0 && (g_anim == ANIM_NONE || g_anim == ANIM_PULSE) && !g_dragging &&
        !g_selecting && GetTickCount64() - g_lastInput > (ULONGLONG)S.idleSec * 1000ULL)
        HideWnd(true);
}

static void ShowWnd() {
    if (IsWindowVisible(g_hwnd)) {  // still fading out: remove it before grabbing the backdrop
        DoHide();
        Sleep(35);
    }
    ULONGLONG now = GetTickCount64();
    if (g_index.empty() || now - g_indexTick > 30000) {
        BuildIndex();
        g_indexTick = now;
    }
    KillTimer(g_hwnd, TIMER_ANIM);
    g_anim = ANIM_NONE;
    g_idleFade = false;
    g_dragging = false;
    g_selecting = false;
    g_query.clear();
    g_caret = g_anchor = 0;
    g_hov = 1.0;
    g_files = false;
    g_prevWnd = GetForegroundWindow();
    g_sort = (S.rememberSort && !g_sid.empty())
                 ? (Wh_GetIntValue((g_sid + L"_fileSort").c_str(), 0) & 3)
                 : 0;

    // DPI of the monitor the bar opens on (not the system DPI)
    HMONITOR hmHome = PickMonitor();
    MONITORINFO mi{sizeof(mi)};
    GetMonitorInfoW(hmHome, &mi);
    g_dpi = MonDpi(hmHome);
    ApplyScale();
    UpdateTheme();
    g_homeWork = mi.rcWork;
    ComputeBase(g_homeWork);

    RECT monRect = mi.rcMonitor;
    g_work = g_homeWork;
    if (g_userMoved) {
        POINT mp = {g_dragCx, g_dragTop};
        HMONITOR hm2 = MonitorFromPoint(mp, MONITOR_DEFAULTTOPRIMARY);
        MONITORINFO m2{sizeof(m2)};
        GetMonitorInfoW(hm2, &m2);
        g_work = m2.rcWork;
        monRect = m2.rcMonitor;
        g_baseCx = g_dragCx;
        g_baseTop = g_dragTop;
        g_dpi = MonDpi(hm2);
        ApplyScale();
    } else {
        g_baseCx = g_origCx;
        g_baseTop = g_origTop;
    }
    if (S.frost) CaptureBackdrop(monRect);
    else g_bd.clear();

    UpdateResults();

    int style = ST_NONE, dur = 0;
    bool animate = DecideAnim(true, style, dur);
    if (animate) {
        g_anim = ANIM_OPEN;
        g_animStyle = style;
        g_animDur = dur;
        ComputeFrame(true, 0.0, style, g_curSx, g_curSy, g_curA, g_curTx, g_curTy);
    } else {
        ResetCur();
    }
    g_caretOn = true;
    Layout();

    SetWindowPos(g_hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);

    // Allows to take focus without attaching to another thread's input queue.
    INPUT input{};
    SendInput(1, &input, sizeof(INPUT));
    SetForegroundWindow(g_hwnd);
    SetFocus(g_hwnd);

    g_shownTick = GetTickCount64();
    g_lastInput = g_shownTick;
    GetCursorPos(&g_lastPt);
    SetTimer(g_hwnd, TIMER_HOVER, 16, nullptr);
    SetTimer(g_hwnd, TIMER_IDLE, 250, nullptr);
    if (animate) {
        g_animStart = GetTickCount64();
        SetTimer(g_hwnd, TIMER_ANIM, 10, nullptr);
    }
}

// ------------------------------------------------------------- actions ---

static void CopyText(const std::wstring& s) {
    if (!OpenClipboard(g_hwnd)) return;
    EmptyClipboard();
    HGLOBAL m = GlobalAlloc(GMEM_MOVEABLE, (s.size() + 1) * sizeof(wchar_t));
    if (m) {
        void* p = GlobalLock(m);
        if (p) {
            memcpy(p, s.c_str(), (s.size() + 1) * sizeof(wchar_t));
            GlobalUnlock(m);
            if (!SetClipboardData(CF_UNICODETEXT, m)) GlobalFree(m);
        } else {
            GlobalFree(m);
        }
    }
    CloseClipboard();
}

// First line of the clipboard text.
static std::wstring ClipText() {
    std::wstring out;
    if (!OpenClipboard(g_hwnd)) return out;
    HANDLE h = GetClipboardData(CF_UNICODETEXT);
    if (h) {
        const wchar_t* p = (const wchar_t*)GlobalLock(h);
        if (p) {
            for (; *p && *p != L'\r' && *p != L'\n' && out.size() < 80; p++) out += *p;
            GlobalUnlock(h);
        }
    }
    CloseClipboard();
    return out;
}

// Puts a real file on the clipboard (CF_HDROP + Preferred DropEffect) so Explorer can paste it.
static void SetClipFiles(const std::wstring& path, bool cut) {
    size_t bytes = sizeof(DropFilesHdr) + (path.size() + 2) * sizeof(wchar_t);
    HGLOBAL h = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, bytes);
    HGLOBAL he = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, sizeof(DWORD));
    if (!h || !he) {
        if (h) GlobalFree(h);
        if (he) GlobalFree(he);
        return;
    }
    DropFilesHdr* df = (DropFilesHdr*)GlobalLock(h);
    if (!df) { GlobalFree(h); GlobalFree(he); return; }
    df->pFiles = sizeof(DropFilesHdr);
    df->fWide = TRUE;
    memcpy((BYTE*)df + sizeof(DropFilesHdr), path.c_str(), (path.size() + 1) * sizeof(wchar_t));
    GlobalUnlock(h);
    DWORD* eff = (DWORD*)GlobalLock(he);
    if (!eff) { GlobalFree(h); GlobalFree(he); return; }
    *eff = cut ? 2 : 1;  // DROPEFFECT_MOVE / DROPEFFECT_COPY
    GlobalUnlock(he);

    if (!OpenClipboard(g_hwnd)) { GlobalFree(h); GlobalFree(he); return; }
    EmptyClipboard();
    if (!SetClipboardData(CF_HDROP, h)) GlobalFree(h);
    UINT fmt = RegisterClipboardFormatW(L"Preferred DropEffect");
    if (!SetClipboardData(fmt, he)) GlobalFree(he);
    CloseClipboard();
}

// Pastes the files on the clipboard (copy or move, as Explorer would) into a folder.
static void PasteFilesInto(const std::wstring& destDir) {
    if (destDir.empty()) return;
    std::wstring from;
    bool move = false;
    if (!OpenClipboard(g_hwnd)) return;
    HANDLE h = GetClipboardData(CF_HDROP);
    if (h) {
        HDROP dr = (HDROP)h;
        UINT n = DragQueryFileW(dr, 0xFFFFFFFFu, nullptr, 0);
        for (UINT i = 0; i < n; i++) {
            wchar_t b[MAX_PATH * 2];
            if (DragQueryFileW(dr, i, b, ARRAYSIZE(b))) {
                from.append(b);
                from.push_back(0);
            }
        }
        UINT fmt = RegisterClipboardFormatW(L"Preferred DropEffect");
        HANDLE he = GetClipboardData(fmt);
        if (he) {
            DWORD* e = (DWORD*)GlobalLock(he);
            if (e) {
                move = (*e & 2) && !(*e & 1);
                GlobalUnlock(he);
            }
        }
    }
    CloseClipboard();
    if (from.empty()) return;
    from.push_back(0);  // double-null terminated list
    std::wstring to = destDir;
    to.push_back(0);
    SHFILEOPSTRUCTW op{};
    op.hwnd = g_hwnd;
    op.wFunc = move ? FO_MOVE : FO_COPY;
    op.pFrom = from.c_str();
    op.pTo = to.c_str();
    op.fFlags = FOF_ALLOWUNDO;
    SHFileOperationW(&op);
    if (move && OpenClipboard(g_hwnd)) {
        EmptyClipboard();
        CloseClipboard();
    }
}

static void Activate(int i) {
    if (i < 0 || i >= (int)g_items.size()) return;
    Item it = g_items[i];
    if (it.kind == K_NAV) { NavTo(it.target); return; }
    if (it.kind == K_PASTE) { PasteClip(it.target); return; }
    HideWnd();
    AddRecent(it);
    if (it.kind == 1) {
        CopyText(it.target);
    } else if (it.kind == 3) {
        std::wstring file = it.target, args;
        size_t sp = file.find(L' ');
        if (sp != std::wstring::npos) {
            args = file.substr(sp + 1);
            file = file.substr(0, sp);
        }
        file = LaunchPath(Sys32(file));  // 64-bit System32 tools even when running as 32-bit
        ShellExecuteW(nullptr, L"open", file.c_str(), args.empty() ? nullptr : args.c_str(),
                      nullptr, SW_SHOWNORMAL);
    } else if (it.kind == 4) {
        OpenUrl(it.target);
    } else {
        std::wstring file = LaunchPath(it.target);
        ShellExecuteW(nullptr, L"open", file.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
    }
}

static void RemoveRecentAt(int i) {
    if (i < 0 || i >= (int)g_items.size() || g_items[i].cat != C_RECENT) return;
    std::wstring lt = Lower(g_items[i].target);
    for (size_t k = 0; k < g_recents.size(); k++) {
        if (Lower(g_recents[k].target) == lt) {
            g_recents.erase(g_recents.begin() + k);
            break;
        }
    }
    SaveRecents();
    int keep = g_sel;
    UpdateResults();
    g_sel = g_items.empty() ? 0 : std::min(keep, (int)g_items.size() - 1);
    Layout();
}

static void RegisterHot() {
    UnregisterHotKey(g_hwnd, 1);
    std::wstring s = Lower(S.hotkey);
    UINT mods = MOD_NOREPEAT, vk = VK_SPACE;
    if (s.find(L"ctrl") != std::wstring::npos) mods |= MOD_CONTROL;
    if (s.find(L"alt") != std::wstring::npos) mods |= MOD_ALT;
    if (s.find(L"shift") != std::wstring::npos) mods |= MOD_SHIFT;
    if (s.find(L"win") != std::wstring::npos) mods |= MOD_WIN;
    size_t p = s.rfind(L'+');
    std::wstring key = p == std::wstring::npos ? s : s.substr(p + 1);
    if (key.size() == 1) vk = (UINT)towupper(key[0]);
    if (!RegisterHotKey(g_hwnd, 1, mods, vk)) Wh_Log(L"Failed to register hotkey");
}

// Result row under a window-relative point; the shadow margin around the panel is not a row.
static int HitRow(int x, int y) {
    int M = Mg();
    if (x < M || x >= M + g_pw) return -1;
    y -= M;
    int top = Sc(SEARCH_H) + Sc(1) + Sc(PAD);
    if (y < top) return -1;
    int i = (y - top) / Sc(ROW_H);
    return i < (int)g_items.size() ? i : -1;
}

static void SetSel(int i, bool pulse) {
    if (i == g_sel) return;
    g_sel = i;
    ComputeGhost();
    Redraw();
    if (pulse) StartPulse();
}

static void QueryChanged() {
    UpdateResults();
    g_caretOn = true;
    Layout();
}

// ======================= v3.4: UI-side actions =======================

static void NavTo(const std::wstring& path) {
    g_path = path;
    LoadDir(path);
    g_query.clear();
    g_caret = g_anchor = 0;
    UpdateResults();
    g_caretOn = true;
    Layout();
}

static void ToggleFiles() {
    if (!S.fileBrowser) return;
    g_files = !g_files;
    if (g_files) {
        if (!g_pathInit) {
            g_pathInit = true;
            g_path = Env(L"%USERPROFILE%");
            std::wstring sd = Trim(S.startDrive);
            if (!sd.empty() && iswalpha(sd[0])) {
                std::wstring root;
                root += (wchar_t)towupper(sd[0]);
                root += L":\\";
                if (PathIsDirectoryW(root.c_str())) g_path = root;
            }
        }
        LoadDir(g_path);
    }
    g_query.clear();
    g_caret = g_anchor = 0;
    UpdateResults();
    g_caretOn = true;
    Layout();
    StartPulse();
}

static void GoUp() {
    if (!g_files || g_path.empty()) return;
    NavTo(ParentOf(g_path));
}

static void CycleSort() {
    g_sort = (g_sort + 1) & 3;
    if (S.rememberSort && !g_sid.empty())
        Wh_SetIntValue((g_sid + L"_fileSort").c_str(), g_sort);
    UpdateResults();
    Layout();
}

static void JumpChip(int i) {
    std::vector<Chip> c = Chips();
    if (i >= 0 && i < (int)c.size()) NavTo(c[i].path);
}

static void CycleDrive(int dir) {
    if (!g_files) return;
    std::vector<std::wstring> d;
    for (auto& c : Chips()) if (c.drive) d.push_back(c.path);
    if (d.empty()) return;
    int n = (int)d.size(), cur = -1;
    if (g_path.size() >= 2 && g_path[1] == L':') {
        for (int i = 0; i < n; i++)
            if (towupper(d[i][0]) == towupper(g_path[0])) cur = i;
    }
    int nx = cur < 0 ? (dir > 0 ? 0 : n - 1) : (cur + dir + n) % n;
    NavTo(d[nx]);
}

static void SliceKeep(int sel) {
    ApplySlice();
    g_sel = g_items.empty() ? 0 : Clampi(sel, 0, (int)g_items.size() - 1);
    ComputeGhost();
    Redraw();
}

static void MoveSel(int dir) {
    int n = (int)g_items.size();
    if (!n) return;
    if (!g_scrollable) { SetSel((g_sel + dir + n) % n, true); return; }
    size_t total = g_full.size(), cap = (size_t)S.maxResults;
    if (dir > 0) {
        if (g_sel < n - 1) SetSel(g_sel + 1, true);
        else if (g_top + (size_t)n < total) { g_top++; SliceKeep(n - 1); }
        else { g_top = 0; SliceKeep(0); }
    } else {
        if (g_sel > 0) SetSel(g_sel - 1, true);
        else if (g_top > 0) { g_top--; SliceKeep(0); }
        else { g_top = total > cap ? total - cap : 0; SliceKeep((int)cap); }
    }
}

static void PasteClip(const std::wstring& text) {
    HWND prev = g_prevWnd;
    DoHide();  // no animation: focus must go back right away
    CopyText(text);
    if (prev && IsWindow(prev)) SetForegroundWindow(prev);
    Sleep(120);
    INPUT in[4] = {};
    in[0].type = INPUT_KEYBOARD;
    in[0].ki.wVk = VK_CONTROL;
    in[1].type = INPUT_KEYBOARD;
    in[1].ki.wVk = 'V';
    in[2] = in[1];
    in[2].ki.dwFlags = KEYEVENTF_KEYUP;
    in[3] = in[0];
    in[3].ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(4, in, sizeof(INPUT));
}

static void ClipToggleSel(bool remove) {
    if (g_items.empty() || g_items[g_sel].cat != C_CLIP) return;
    std::wstring t = g_items[g_sel].target;
    for (size_t i = 0; i < g_clips.size(); i++) {
        if (g_clips[i].text != t) continue;
        if (remove) g_clips.erase(g_clips.begin() + i);
        else g_clips[i].pinned = !g_clips[i].pinned;
        break;
    }
    SaveClips();
    int keep = g_sel;
    UpdateResults();
    g_sel = g_items.empty() ? 0 : std::min(keep, (int)g_items.size() - 1);
    Layout();
}

static void DeleteToBin(const std::wstring& path) {
    std::wstring from = path;
    from.push_back(0);  // double-null terminated
    SHFILEOPSTRUCTW op{};
    op.hwnd = g_hwnd;
    op.wFunc = FO_DELETE;
    op.pFrom = from.c_str();
    op.fFlags = FOF_ALLOWUNDO;
    SHFileOperationW(&op);
    if (g_files) {
        LoadDir(g_path);
        UpdateResults();
        Layout();
    }
}

// ======================= end of v3.4 UI-side block =======================

static void ResetPosition() {
    g_userMoved = false;
    g_dragging = false;
    if (GetCapture() == g_hwnd) ReleaseCapture();
    g_work = g_homeWork;
    g_dpi = MonDpi(MonitorFromRect(&g_homeWork, MONITOR_DEFAULTTONEAREST));
    ApplyScale();
    ComputeBase(g_homeWork);
    g_baseCx = g_origCx;
    g_baseTop = g_origTop;
    Layout();
    StartPulse();
}

// Real file-system path of a result (empty for calculator / commands / URLs / non-file items).
static std::wstring FsPath(const Item& it) {
    if (it.cat != C_APP && it.cat != C_FILE && it.cat != C_FOLDER && it.cat != C_RECENT)
        return L"";
    std::wstring p = NormSys(it.target);
    bool drive = p.size() > 2 && p[1] == L':' && p[2] == L'\\';
    bool unc = p.size() > 2 && p[0] == L'\\' && p[1] == L'\\';
    if (!drive && !unc) return L"";
    // Existence is checked on the launch path (Sysnative when 32-bit); the canonical path is returned.
    if (!PathFileExistsW(LaunchPath(p).c_str())) return L"";
    return p;
}

static void TextMenuAction(int id) {
    switch (id) {
        case M_TCUT:
            if (HasSel()) { CopyText(SelText()); DeleteSel(); QueryChanged(); }
            break;
        case M_TCOPY:
            if (HasSel()) CopyText(SelText());
            break;
        case M_TPASTE: {
            std::wstring t = ClipText();
            if (!t.empty()) { InsertText(t); QueryChanged(); }
            break;
        }
        case M_TSELALL:
            g_anchor = 0;
            g_caret = g_query.size();
            g_caretOn = true;
            Redraw();
            break;
    }
}

// ---- liquid-glass context menu: its own layered window, same glass as the bar ----

static void FreeMenuBuf() {
    if (g_mDc) {
        SelectObject(g_mDc, g_mOld);
        DeleteDC(g_mDc);
    }
    if (g_mBmp) DeleteObject(g_mBmp);
    g_mDc = nullptr;
    g_mBmp = nullptr;
    g_mOld = nullptr;
    g_mPx = nullptr;
}

static void CloseMenu() {
    if (!g_menuOpen) return;
    g_menuOpen = false;  // first, so WM_CAPTURECHANGED does not recurse
    if (g_menuWnd) {
        if (GetCapture() == g_menuWnd) ReleaseCapture();
        ShowWindow(g_menuWnd, SW_HIDE);
    }
    FreeMenuBuf();
    NoteInput();
}

static void RenderMenu() {
    if (!g_mPx || !g_menuWnd) return;
    int M = g_mM, W = g_mW, H = g_mH, ww = W + 2 * M, wh = H + 2 * M;
    size_t total = (size_t)ww * wh;
    if (S.shadow) {
        std::vector<DWORD> sh;
        BuildShadow(sh, ww, wh, M, W, H, (double)g_mRad);
        memcpy(g_mPx, sh.data(), total * 4);
    } else {
        memset(g_mPx, 0, total * 4);
    }
    DWORD* P = g_mPx + (size_t)M * ww + M;
    PaintBg(P, ww, W, H, g_mX, g_mY, g_mRad);

    const Pal& pl = g_pal;
    HDC dc = g_mDc;
    SetBkMode(dc, TRANSPARENT);
    int saveOff = g_off;
    g_off = M;
    for (size_t i = 0; i < g_mItems.size(); i++) {
        const MenuEnt& e = g_mItems[i];
        const RECT& r = g_mRects[i];
        if (e.sep) {
            FillRR(P, ww, W, H, r.left + Sc(6), (r.top + r.bottom) / 2, r.right - r.left - Sc(12),
                   std::max(1, Sc(1)), 0, pl.hi, 40);
            continue;
        }
        if ((int)i == g_mHot && e.enabled)
            FillRR(P, ww, W, H, r.left, r.top, r.right - r.left, r.bottom - r.top, Sc(9), pl.hi,
                   pl.dark ? 46 : 32);
        RECT rt = {r.left + Sc(14), r.top, r.right - Sc(10), r.bottom};
        Txt(dc, g_fName, e.text, rt, e.enabled ? pl.text : pl.ghost,
            DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
    }
    g_off = saveOff;
    GdiFlush();

    POINT pos = {g_mX - M, g_mY - M};
    SIZE size = {ww, wh};
    POINT src = {0, 0};
    BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    HDC sdc = GetDC(nullptr);
    UpdateLayeredWindow(g_menuWnd, sdc, &pos, &size, g_mDc, &src, 0, &bf, ULW_ALPHA);
    ReleaseDC(nullptr, sdc);
}

static void OpenMenu(const std::vector<MenuEnt>& items, int sx, int sy) {
    if (!g_menuWnd) return;
    g_mItems = items;
    int itemH = Sc(34), sepH = Sc(11), padV = Sc(7), padH = Sc(7);

    HDC sdc = GetDC(nullptr);
    HGDIOBJ old = SelectObject(sdc, g_fName);
    int maxT = 0;
    for (auto& e : g_mItems) {
        if (e.sep) continue;
        SIZE sz{};
        GetTextExtentPoint32W(sdc, e.text.c_str(), (int)e.text.size(), &sz);
        maxT = std::max(maxT, (int)sz.cx);
    }
    SelectObject(sdc, old);
    ReleaseDC(nullptr, sdc);

    g_mW = std::max(Sc(200), maxT + Sc(44));
    g_mRects.clear();
    int y = padV;
    for (auto& e : g_mItems) {
        int h = e.sep ? sepH : itemH;
        g_mRects.push_back(RECT{padH, y, g_mW - padH, y + h});
        y += h;
    }
    g_mH = y + padV;
    g_mM = Sc(18);
    g_mRad = std::min(Sc(16), g_mH / 2);

    POINT pt = {sx, sy};
    HMONITOR hm = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi{sizeof(mi)};
    GetMonitorInfoW(hm, &mi);
    g_mX = std::max((int)mi.rcWork.left, std::min(sx, (int)mi.rcWork.right - g_mW));
    g_mY = sy;
    if (g_mY + g_mH > mi.rcWork.bottom) g_mY = sy - g_mH;
    if (g_mY < mi.rcWork.top) g_mY = mi.rcWork.top;

    FreeMenuBuf();
    int ww = g_mW + 2 * g_mM, wh = g_mH + 2 * g_mM;
    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = ww;
    bi.bmiHeader.biHeight = -wh;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    g_mDc = CreateCompatibleDC(nullptr);
    g_mBmp = CreateDIBSection(g_mDc, &bi, DIB_RGB_COLORS, (void**)&g_mPx, nullptr, 0);
    if (!g_mBmp || !g_mPx) {
        FreeMenuBuf();
        return;
    }
    g_mOld = SelectObject(g_mDc, g_mBmp);

    g_mHot = -1;
    g_menuOpen = true;
    RenderMenu();
    SetWindowPos(g_menuWnd, HWND_TOPMOST, g_mX - g_mM, g_mY - g_mM, ww, wh,
                 SWP_NOACTIVATE | SWP_SHOWWINDOW);
    SetCapture(g_menuWnd);  // clicks anywhere else close the menu
}

static void MenuMove(int dir) {
    int n = (int)g_mItems.size();
    if (!n || !g_menuOpen) return;
    int i = g_mHot;
    if (i < 0) i = dir > 0 ? -1 : 0;
    for (int k = 0; k < n; k++) {
        i = (i + dir + n) % n;
        if (!g_mItems[i].sep && g_mItems[i].enabled) {
            g_mHot = i;
            RenderMenu();
            return;
        }
    }
}

static void RunMenuAction(int id) {
    switch (id) {
        case M_OPEN:
            Activate(g_ctxRow);
            break;
        case M_LOC:
            if (!g_ctxPath.empty()) {
                std::wstring args = L"/select,\"" + g_ctxPath + L"\"";
                HideWnd();
                ShellExecuteW(nullptr, L"open", L"explorer.exe", args.c_str(), nullptr, SW_SHOWNORMAL);
            }
            break;
        case M_CUT:
            if (!g_ctxPath.empty()) SetClipFiles(g_ctxPath, true);
            break;
        case M_COPY:
            if (!g_ctxPath.empty()) SetClipFiles(g_ctxPath, false);
            else CopyText(g_ctxItem.target);
            break;
        case M_PASTE:
            if (!g_ctxPath.empty()) {
                std::wstring dest = g_ctxPath;
                if (!g_ctxDir) {
                    PathRemoveFileSpecW(&dest[0]);
                    dest.resize(wcslen(dest.c_str()));
                }
                PasteFilesInto(dest);
            }
            break;
        case M_COPYPATH:
            if (!g_ctxPath.empty()) CopyText(g_ctxPath);
            break;
        case M_DELETE:
            if (!g_ctxPath.empty()) DeleteToBin(g_ctxPath);
            break;
        case M_TCUT:
        case M_TCOPY:
        case M_TPASTE:
        case M_TSELALL:
            TextMenuAction(id);
            break;
    }
}

static void ChooseMenu(int idx) {
    if (idx < 0 || idx >= (int)g_mItems.size()) return;
    int id = g_mItems[idx].id;
    CloseMenu();
    RunMenuAction(id);
}

static int MenuHit(LPARAM lp) {
    POINT p = {GET_X_LPARAM(lp) - g_mM, GET_Y_LPARAM(lp) - g_mM};
    for (size_t i = 0; i < g_mRects.size(); i++) {
        if (PtInRect(&g_mRects[i], p) && !g_mItems[i].sep && g_mItems[i].enabled) return (int)i;
    }
    return -1;
}

static LRESULT CALLBACK MenuProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_ERASEBKGND:
            return 1;
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;
        case WM_MOUSEMOVE: {
            if (!g_menuOpen) return 0;
            int hot = MenuHit(lp);
            if (hot != g_mHot) {
                g_mHot = hot;
                RenderMenu();
            }
            NoteInput();
            return 0;
        }
        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN: {
            if (!g_menuOpen) return 0;
            int hit = MenuHit(lp);
            int x = GET_X_LPARAM(lp) - g_mM, y = GET_Y_LPARAM(lp) - g_mM;
            bool inPanel = x >= 0 && x < g_mW && y >= 0 && y < g_mH;
            if (hit >= 0 && msg == WM_LBUTTONDOWN) ChooseMenu(hit);
            else if (!inPanel) CloseMenu();
            return 0;
        }
        case WM_CAPTURECHANGED:
            if (g_menuOpen) CloseMenu();
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

static void ShowContextMenu(LPARAM lp) {
    int cx = GET_X_LPARAM(lp), cy = GET_Y_LPARAM(lp);
    int M = Mg();
    int px = cx - M, py = cy - M;
    int row = HitRow(cx, cy);
    bool inSearch = px >= 0 && px < g_pw && py >= 0 && py < Sc(SEARCH_H);
    if (row < 0 && !inSearch) return;

    NoteInput();
    g_ctxRow = row;
    g_ctxPath.clear();
    g_ctxDir = false;
    g_ctxItem = Item{};
    std::vector<MenuEnt> m;

    if (row >= 0) {
        if (g_sel != row) SetSel(row, false);
        g_ctxItem = g_items[row];
        g_ctxPath = FsPath(g_ctxItem);
        bool hasPath = !g_ctxPath.empty();
        g_ctxDir = hasPath && PathIsDirectoryW(LaunchPath(g_ctxPath).c_str());
        bool canPaste = hasPath && IsClipboardFormatAvailable(CF_HDROP);
        m.push_back(MenuEnt{M_OPEN, L"Open", true, false});
        m.push_back(MenuEnt{M_LOC, L"Open file location", hasPath, false});
        m.push_back(MenuEnt{0, L"", false, true});
        m.push_back(MenuEnt{M_CUT, L"Cut", hasPath, false});
        m.push_back(MenuEnt{M_COPY, L"Copy", true, false});
        m.push_back(MenuEnt{M_PASTE, g_ctxDir ? L"Paste into folder" : L"Paste into this folder",
                            canPaste, false});
        m.push_back(MenuEnt{0, L"", false, true});
        m.push_back(MenuEnt{M_COPYPATH, L"Copy path", hasPath, false});
        if (g_files && hasPath && g_ctxPath.size() > 3) {
            m.push_back(MenuEnt{0, L"", false, true});
            m.push_back(MenuEnt{M_DELETE, L"Delete (Recycle Bin)", true, false});
        }
    } else {
        bool sel = HasSel();
        m.push_back(MenuEnt{M_TCUT, L"Cut", sel, false});
        m.push_back(MenuEnt{M_TCOPY, L"Copy", sel, false});
        m.push_back(MenuEnt{M_TPASTE, L"Paste", IsClipboardFormatAvailable(CF_UNICODETEXT) != FALSE,
                            false});
        m.push_back(MenuEnt{0, L"", false, true});
        m.push_back(MenuEnt{M_TSELALL, L"Select all", !g_query.empty(), false});
    }

    POINT pt;
    GetCursorPos(&pt);
    OpenMenu(m, pt.x, pt.y);
}

static void OnLButton(LPARAM lp, bool dbl) {
    if (g_anim == ANIM_CLOSE) return;
    NoteInput();
    int cx = GET_X_LPARAM(lp), cy = GET_Y_LPARAM(lp);
    int i = HitRow(cx, cy);
    if (i >= 0) {
        if (g_items[i].cat == C_RECENT && cx - Mg() > g_pw - Sc(60)) {
            RemoveRecentAt(i);
            return;
        }
        Activate(i);
        return;
    }
    int M = Mg();
    int px = cx - M, py = cy - M;
    bool inPanel = px >= 0 && px < g_pw && py >= 0 && py < g_ph;
    if (!inPanel) return;

    // Text field: select text, never drag the bar.
    if (InTextBox(px, py)) {
        if (dbl) {
            g_anchor = 0;
            g_caret = g_query.size();
            g_caretOn = true;
            Redraw();
            return;
        }
        g_caret = CaretFromX(px);
        if (GetKeyState(VK_SHIFT) >= 0) g_anchor = g_caret;
        g_selecting = true;
        g_caretOn = true;
        SetCapture(g_hwnd);
        Redraw();
        return;
    }

    if (!S.drag) return;
    if (dbl) {
        ResetPosition();
        return;
    }
    GetCursorPos(&g_dragStart);
    g_dragBase0.x = g_baseCx;
    g_dragBase0.y = g_baseTop;
    g_dragging = true;
    g_dragMoved = false;
    SetCapture(g_hwnd);
}

static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_ERASEBKGND:
            return 1;
        case WM_PAINT: {
            PAINTSTRUCT ps;
            BeginPaint(hwnd, &ps);
            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_CLIPBOARDUPDATE:
            OnClipUpdate();
            return 0;
        case WM_MOUSEWHEEL:
            if (g_scrollable && g_anim != ANIM_CLOSE && !g_menuOpen) {
                size_t cap = (size_t)S.maxResults;
                if (GET_WHEEL_DELTA_WPARAM(wp) > 0) { if (g_top > 0) g_top--; }
                else if (g_top + cap < g_full.size()) g_top++;
                SliceKeep(g_sel);
                NoteInput();
            }
            return 0;
        case WM_HOTKEY:
            if (IsWindowVisible(hwnd) && g_anim != ANIM_CLOSE) HideWnd();
            else if (!(S.noFull && FullscreenActive())) ShowWnd();
            return 0;
        case WM_APP_SETTINGS:
            LoadSettings();
            RegisterHot();
            UpdateTheme();
            ApplyClipSetting();
            if (IsWindowVisible(hwnd) && g_anim != ANIM_CLOSE) {
                ComputeBase(g_homeWork);
                if (!g_userMoved) {
                    g_baseCx = g_origCx;
                    g_baseTop = g_origTop;
                }
                UpdateResults();
                Layout();
            }
            return 0;
        case WM_ACTIVATE:
            if (LOWORD(wp) == WA_INACTIVE && g_menuOpen) CloseMenu();
            if (LOWORD(wp) == WA_INACTIVE && IsWindowVisible(hwnd) && !S.onTop && !g_menuOpen &&
                GetTickCount64() - g_shownTick > 250)
                HideWnd();
            return 0;
        case WM_SETCURSOR:
            if (LOWORD(lp) == HTCLIENT) {
                POINT pt;
                GetCursorPos(&pt);
                ScreenToClient(hwnd, &pt);
                if (g_selecting || InTextBox(pt.x - Mg(), pt.y - Mg())) {
                    SetCursor(LoadCursorW(nullptr, IDC_IBEAM));
                    return TRUE;
                }
            }
            break;
        case WM_TIMER:
            switch (wp) {
                case TIMER_ANIM: TickAnim(); break;
                case TIMER_HOVER: HoverTick(); break;
                case TIMER_IDLE: IdleTick(); break;
                case TIMER_BLINK:
                    if (IsWindowVisible(hwnd) && g_anim == ANIM_NONE) {
                        if (!S.onTop && !g_menuOpen && GetForegroundWindow() != hwnd &&
                            GetTickCount64() - g_shownTick > 600) {
                            HideWnd();
                        } else {
                            g_caretOn = !g_caretOn;
                            Redraw();
                        }
                    }
                    break;
            }
            return 0;
        case WM_CHAR:
            if (g_menuOpen) return 0;
            if (g_anim == ANIM_CLOSE) CancelIdleFade();
            if (g_anim == ANIM_CLOSE) return 0;
            NoteInput();
            if (wp >= 32 && wp != 127) {
                InsertText(std::wstring(1, (wchar_t)wp));
                QueryChanged();
            }
            return 0;
        case WM_KEYDOWN: {
            if (g_menuOpen) {
                if (wp == VK_ESCAPE) CloseMenu();
                else if (wp == VK_UP) MenuMove(-1);
                else if (wp == VK_DOWN) MenuMove(1);
                else if (wp == VK_RETURN && g_mHot >= 0) ChooseMenu(g_mHot);
                return 0;
            }
            if (g_anim == ANIM_CLOSE) CancelIdleFade();
            if (g_anim == ANIM_CLOSE) return 0;
            NoteInput();
            bool ctrl = GetKeyState(VK_CONTROL) < 0;
            bool shift = GetKeyState(VK_SHIFT) < 0;
            int n = (int)g_items.size();
            if (ctrl && g_files && wp >= '1' && wp <= '9') { JumpChip((int)(wp - '1')); return 0; }
            switch (wp) {
                case VK_ESCAPE: HideWnd(); break;
                case VK_RETURN:
                    if (ctrl && n && g_items[g_sel].cat == C_CLIP) {
                        CopyText(g_items[g_sel].target);
                        HideWnd();
                    } else if (!g_calcInline.empty() && S.calcDisplay == L"inline") {
                        CopyText(g_calcCopy);
                        HideWnd();
                    } else {
                        Activate(g_sel);
                    }
                    break;
                case VK_DOWN:
                    MoveSel(1);
                    break;
                case VK_UP:
                    MoveSel(-1);
                    break;
                case VK_LEFT: {
                    size_t pos = (!shift && HasSel()) ? SelLo() : (g_caret > 0 ? g_caret - 1 : 0);
                    g_caret = pos;
                    if (!shift) g_anchor = pos;
                    g_caretOn = true;
                    Redraw();
                    break;
                }
                case VK_RIGHT:
                    if (!g_ghost.empty() && !HasSel() && g_caret == g_query.size()) {
                        SetQuery(g_items[g_sel].name);  // accept ghost text
                        QueryChanged();
                        StartPulse();
                    } else {
                        size_t pos = (!shift && HasSel()) ? SelHi()
                                                          : std::min(g_caret + 1, g_query.size());
                        g_caret = pos;
                        if (!shift) g_anchor = pos;
                        g_caretOn = true;
                        Redraw();
                    }
                    break;
                case VK_HOME:
                    g_caret = 0;
                    if (!shift) g_anchor = 0;
                    g_caretOn = true;
                    Redraw();
                    break;
                case VK_END:
                    g_caret = g_query.size();
                    if (!shift) g_anchor = g_caret;
                    g_caretOn = true;
                    Redraw();
                    break;
                case VK_DELETE:
                    if (g_query.empty()) {
                        if (n && g_items[g_sel].cat == C_RECENT) RemoveRecentAt(g_sel);
                    } else if (HasSel()) {
                        DeleteSel();
                        QueryChanged();
                    } else if (g_caret < g_query.size()) {
                        g_query.erase(g_caret, 1);
                        QueryChanged();
                    }
                    break;
                case VK_TAB:
                    if (ctrl) { CycleDrive(shift ? -1 : 1); break; }
                    if (shift) { MoveSel(-1); break; }
                    if (!g_files && n && g_items[g_sel].cat == C_APP &&
                        _wcsicmp(g_items[g_sel].name.c_str(), g_query.c_str()) != 0) {
                        SetQuery(g_items[g_sel].name);  // autofill
                        QueryChanged();
                        StartPulse();
                    } else if (S.fileBrowser) {
                        ToggleFiles();
                    } else {
                        MoveSel(1);
                    }
                    break;
                case VK_BACK:
                    if (HasSel()) {
                        DeleteSel();
                    } else if (g_caret > 0) {
                        if (ctrl) {
                            size_t e = g_caret;
                            while (e > 0 && g_query[e - 1] == L' ') e--;
                            size_t s = e;
                            while (s > 0 && g_query[s - 1] != L' ') s--;
                            g_query.erase(s, g_caret - s);
                            g_caret = g_anchor = s;
                        } else {
                            g_query.erase(g_caret - 1, 1);
                            g_caret--;
                            g_anchor = g_caret;
                        }
                    } else {
                        if (g_files && g_query.empty()) GoUp();
                        break;
                    }
                    QueryChanged();
                    break;
                case 'A':
                    if (ctrl) TextMenuAction(M_TSELALL);
                    break;
                case 'C':
                    if (ctrl) TextMenuAction(M_TCOPY);
                    break;
                case 'X':
                    if (ctrl) TextMenuAction(M_TCUT);
                    break;
                case 'V':
                    if (ctrl) TextMenuAction(M_TPASTE);
                    break;
                case 'S':
                    if (ctrl && g_files) CycleSort();
                    break;
                case 'D':
                    if (ctrl) ClipToggleSel(false);
                    break;
                case 'R':
                    if (ctrl && shift) ClipToggleSel(true);
                    break;
            }
            return 0;
        }
        case WM_MOUSEMOVE: {
            if (g_anim == ANIM_CLOSE) return 0;
            if (g_selecting) {
                g_caret = CaretFromX(GET_X_LPARAM(lp) - Mg());
                g_caretOn = true;
                g_lastInput = GetTickCount64();
                Redraw();
                return 0;
            }
            if (g_dragging) {
                POINT pt;
                GetCursorPos(&pt);
                int dx = pt.x - g_dragStart.x, dy = pt.y - g_dragStart.y;
                if (!g_dragMoved && std::abs(dx) + std::abs(dy) < 4) return 0;
                g_dragMoved = true;
                g_userMoved = true;
                g_baseCx = g_dragBase0.x + dx;
                g_baseTop = g_dragBase0.y + dy;
                g_dragCx = g_baseCx;
                g_dragTop = g_baseTop;
                HMONITOR hm = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
                MONITORINFO mi{sizeof(mi)};
                if (GetMonitorInfoW(hm, &mi)) g_work = mi.rcWork;
                double nd = MonDpi(hm);
                g_lastInput = GetTickCount64();
                if (std::fabs(nd - g_dpi) > 0.001) {  // moved onto a monitor with another DPI
                    g_dpi = nd;
                    Layout();
                } else {
                    PlacePanel();
                    Redraw();
                }
                return 0;
            }
            int i = HitRow(GET_X_LPARAM(lp), GET_Y_LPARAM(lp));
            if (i >= 0 && i != g_sel) SetSel(i, false);
            return 0;
        }
        case WM_LBUTTONDOWN:
            OnLButton(lp, false);
            return 0;
        case WM_LBUTTONDBLCLK:
            OnLButton(lp, true);
            return 0;
        case WM_LBUTTONUP:
            if (g_dragging || g_selecting) {
                g_dragging = false;
                g_selecting = false;
                if (GetCapture() == hwnd) ReleaseCapture();
            }
            return 0;
        case WM_RBUTTONDOWN:
            NoteInput();
            return 0;
        case WM_RBUTTONUP:
            if (g_anim != ANIM_CLOSE) ShowContextMenu(lp);
            return 0;
        case WM_CAPTURECHANGED:
            g_dragging = false;
            g_selecting = false;
            return 0;
        case WM_DESTROY:
            KillTimer(hwnd, TIMER_BLINK);
            KillTimer(hwnd, TIMER_ANIM);
            KillTimer(hwnd, TIMER_HOVER);
            KillTimer(hwnd, TIMER_IDLE);
            UnregisterHotKey(hwnd, 1);
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

// --------------------------------------------------------------- thread ---

static DWORD WINAPI UiThread(LPVOID) {
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    HINSTANCE hi = (HINSTANCE)&__ImageBase;  // this mod's module, not the host exe
    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(wc);
    wc.style = CS_DBLCLKS;  // double-click selects all / resets the dragged position
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hi;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.lpszClassName = kClass;
    if (!RegisterClassExW(&wc)) {
        Wh_Log(L"RegisterClassExW failed");
        CoUninitialize();
        return 0;
    }
    WNDCLASSEXW mc{};
    mc.cbSize = sizeof(mc);
    mc.lpfnWndProc = MenuProc;
    mc.hInstance = hi;
    mc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    mc.lpszClassName = kMenuClass;
    RegisterClassExW(&mc);

    g_hwnd = CreateWindowExW(WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW, kClass,
                             L"Spotlight", WS_POPUP, 0, 0, 1, 1, nullptr, nullptr, hi, nullptr);
    if (g_hwnd) {
        g_theme = OpenThemeData(g_hwnd, L"CompositedWindow::Window");
        g_menuWnd = CreateWindowExW(WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
                                    kMenuClass, L"", WS_POPUP, 0, 0, 1, 1, nullptr, nullptr, hi,
                                    nullptr);
        LoadSettings();
        LoadRecents();
        ApplyClipSetting();
        UpdateTheme();
        g_dpi = MonDpi(MonitorFromPoint(POINT{0, 0}, MONITOR_DEFAULTTOPRIMARY));
        ApplyScale();
        RegisterHot();
        SetTimer(g_hwnd, TIMER_BLINK, 530, nullptr);

        MSG m;
        while (GetMessageW(&m, nullptr, 0, 0) > 0) {
            TranslateMessage(&m);
            DispatchMessageW(&m);
        }
        CloseMenu();
        if (g_menuWnd) {
            DestroyWindow(g_menuWnd);
            g_menuWnd = nullptr;
        }
        DestroyWindow(g_hwnd);
        if (g_theme) CloseThemeData(g_theme);
        g_theme = nullptr;
        g_hwnd = nullptr;
    }
    FreeBuffers();
    g_shadow.clear();
    g_bd.clear();
    if (g_fQuery) DeleteObject(g_fQuery);
    if (g_fName) DeleteObject(g_fName);
    if (g_fSub) DeleteObject(g_fSub);
    if (g_fIcon) DeleteObject(g_fIcon);
    if (g_fSearch) DeleteObject(g_fSearch);
    g_fQuery = g_fName = g_fSub = g_fIcon = g_fSearch = nullptr;
    g_icons.clear();
    UnregisterClassW(kMenuClass, hi);
    UnregisterClassW(kClass, hi);
    CoUninitialize();
    return 0;
}

// ------------------------------------------------------- tool mod hooks ---

BOOL WhTool_ModInit() {
    InitializeCriticalSection(&g_fcs);
    g_stopEvt = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_rescanEvt = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    g_idxThread = CreateThread(nullptr, 0, IndexThread, nullptr, 0, nullptr);
    g_thread = CreateThread(nullptr, 0, UiThread, nullptr, 0, &g_threadId);
    return TRUE;
}

void WhTool_ModSettingsChanged() {
    if (g_hwnd) PostMessageW(g_hwnd, WM_APP_SETTINGS, 0, 0);
}

void WhTool_ModUninit() {
    if (g_stopEvt) SetEvent(g_stopEvt);
    if (g_thread) {
        // Wait until the UI thread has really exited (retry until its queue exists).
        while (WaitForSingleObject(g_thread, 100) == WAIT_TIMEOUT)
            PostThreadMessageW(g_threadId, WM_QUIT, 0, 0);
        CloseHandle(g_thread);
        g_thread = nullptr;
    }
    if (g_idxThread) {
        WaitForSingleObject(g_idxThread, INFINITE);
        CloseHandle(g_idxThread);
        g_idxThread = nullptr;
    }
    if (g_stopEvt) { CloseHandle(g_stopEvt); g_stopEvt = nullptr; }
    if (g_rescanEvt) { CloseHandle(g_rescanEvt); g_rescanEvt = nullptr; }
    DeleteCriticalSection(&g_fcs);
}

////////////////////////////////////////////////////////////////////////////////
// Windhawk tool mod launcher code (from the Windhawk wiki, "Mods as tools").
// Runs the mod in a dedicated windhawk.exe process.
// Keep this block identical to the wiki snippet so it can be maintained in one place.

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
