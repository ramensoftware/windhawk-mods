// ==WindhawkMod==
// @id              win8-search-charm-recreation
// @name            Windows 8/8.1 Search Charm Recreation
// @description     This mod recreates the Windows 8/8.1 Search Charm on Windows 10 and Windows 11
// @version         1.0.0
// @author          babamohammed
// @github          https://github.com/babamohammed2022
// @license         MIT
// @include         explorer.exe
// @compilerOptions -lgdi32 -luser32 -lshell32 -lole32 -lshlwapi -ldwmapi -luuid -loleaut32 -lcomctl32 -lshcore
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Windows 8/8.1 Search Charm Recreation

This mod recreates the Windows 8/8.1 search panel with local-only results, inspired by
the Metro UI design language. This mod is a best-effort recreation of the Windows 8/8.1 Search Charm on
Windows 10 and Windows 11. It does **not** replace system files, modify the
registry, alter Windows Search, or modify native search binaries.

## Screenshot 

![Windows 8/8.1 Search Charm screenshot](https://raw.githubusercontent.com/babamohammed2022/babamohammed2022/main/win8search.png)

## Features

- **Search panel** on the right side of the screen, with Windows 8.1 green color scheme.
- **Local search** across:
  - **Classic Win32 apps** from Start menu `.lnk` shortcuts
  - **Modern UWP/Store apps** via `FOLDERID_AppsFolder` (the virtual shell namespace
    that lists every installed app, including those without a physical `.lnk` file)
  - **User files** from Desktop, Documents, Downloads, Pictures, Music, Videos
  - **System entries** (Control Panel, Settings, Command Prompt, Task Manager, etc.)
- **All/Apps/Settings/Files** filter, mimicking the Windows 8.1 search scope selector.
- **Vertical scrollbar** in the results list (Windows 8 style): draggable thumb and
  track paging, shown only when the results overflow the visible area.
- **Keyboard shortcuts**: `Win+S` or `Win+Q` to open, `Ctrl+A` to select all,
  `Ctrl+V` to paste, arrow keys to navigate.
- **Click outside** to close, `Esc` to dismiss.
- **Per-monitor DPI** aware, with IME support for CJK input.
- **Metro-style context menu** (right-click) with **Open**, Open file location,
  Run as administrator, Copy path, and sort options (by name, by date installed,
  by most used, by category). The menu is drawn with the Metro design language:
  white background, no 3D borders, Segoe UI text, and the hover highlight uses
  the system accent color, so it adapts to the user's personalization settings.
- **Dedicated keyboard-hook thread**: the low-level `Win+S` / `Win+Q` hook has its
  own message loop, so slow icon extraction, clipboard work, Shell execution, or
  a UAC prompt on the panel thread cannot make Windows remove the keyboard hook.
- **Main-shell only**: the mod loads only in the Explorer process that owns the
  shell window, avoiding duplicate panels, hooks, and index scans when folder
  windows run in separate Explorer processes.
- **Z-band overlay**: uses `CreateWindowInBand` (when available) so the panel stays
  above the taskbar, Start menu, and fullscreen UWP apps.
- **UI translations**: English, Italian, Spanish, French, Portuguese, German,
  Russian, Chinese (Simplified), Japanese, Korean.

## Native-search behavior

When enabled in the settings, the dedicated keyboard hook consumes `Win+S` and/or
`Win+Q` before Windows receives those shortcuts and posts a request to open this
panel. This is the only interception required for those keys.

## Credits

- **AdministratoX** — for the Metro UI interface template that inspired the
  visual layout and design language of this search panel.
- **Meteoni** — for the `IsMainExplorerProcess` pattern used to avoid loading the
  mod in separate folder-window Explorer processes.

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- useWinS: true
  $name: Open with Win+S
  $description: This setting allows the mod to intercept Win+S and open this panel instead of Windows native search.
- useWinQ: true
  $name: Open with Win+Q
  $description: This setting allows the mod to intercept Win+Q and open this panel.
- language: auto
  $name: Language
  $description: This setting modifies the UI language for the search panel. Automatic follows the Windows display language.
  $options:
    - auto: Automatic (Windows display language)
    - en: English
    - it: Italiano
    - es: Español
    - fr: Français
    - pt: Português
    - de: Deutsch
    - ru: Русский
    - zh: 简体中文
    - ja: 日本語
    - ko: 한국어
- colorMode: green
  $name: Panel color
  $description: This setting modifies the color scheme for the search panel background.
  $options:
    - green: Green (Windows 8.1)
    - system: System color (DWM)
    - custom: Custom color
- customColor: "#087900"
  $name: Custom color (hexadecimal)
  $description: This setting is used only when "Custom color" is selected. Example - #0078D7.
- openMs: 300
  $name: Opening duration (ms)
  $description: Duration of the panel opening animation in milliseconds.
- closeMs: 250
  $name: Closing duration (ms)
  $description: This setting modifies the duration of the panel closing animation in milliseconds.
- closeOnClickOutside: true
  $name: Close when clicking outside
  $description: This setting allows to close the panel when it loses activation, such as after clicking another window.
- aboveTaskbar: true
  $name: Keep above the taskbar
  $description: This setting allows to maintain the panel above the taskbar in Z-order.
- useZBand: true
  $name: Use higher z-band (experimental)
  $description: This setting allows to place the panel in the UIAccess z-band (like Task Manager) so it stays above the taskbar, Start menu and fullscreen UWP apps. Requires CreateWindowInBand; falls back to ordinary topmost if unavailable.
- clickCooldownMs: 500
  $name: Click cooldown (ms)
  $description: This setting changes the minimum time between two valid clicks to prevent double launches.
- hotkeyCooldownMs: 400
  $name: Hotkey cooldown (ms)
  $description: This setting changes the minimum time between two Win+S/Q presses to prevent multiple openings.
- monitor: 0
  $name: Monitor
  $description: 0 = monitor under the cursor, 1 = first monitor, 2 = second monitor, and so on.
- debugLog: false
  $name: Debug log
  $description: This setting allows the mod to write z-order diagnostics to the Windhawk log (does not change how anything looks).
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shlobj.h>
#include <shellapi.h>
#include <shlwapi.h>
#include <dwmapi.h>
#include <objbase.h>
#include <shobjidl.h>
#include <commctrl.h>
#include <shellscalingapi.h>
#include <math.h>
#include <string>
#include <vector>
#include <set>
#include <unordered_map>
#include <algorithm>
#include <strsafe.h>
#include <atomic>
#include <cwctype>
#include <imm.h>

extern "C" IMAGE_DOS_HEADER __ImageBase;

// ---------------------------------------------------------------- RAII / utility
class SrwGuard {
    SRWLOCK* l_; bool excl_;
public:
    SrwGuard(SRWLOCK* l, bool exclusive = false) : l_(l), excl_(exclusive) {
        if (excl_) AcquireSRWLockExclusive(l_); else AcquireSRWLockShared(l_);
    }
    ~SrwGuard() { if (excl_) ReleaseSRWLockExclusive(l_); else ReleaseSRWLockShared(l_); }
    SrwGuard(const SrwGuard&) = delete; SrwGuard& operator=(const SrwGuard&) = delete;
};

template <typename T> class GdiObj {
    T h_;
public:
    explicit GdiObj(T h = NULL) : h_(h) {}
    ~GdiObj() { if (h_) DeleteObject(h_); }
    GdiObj(const GdiObj&) = delete; GdiObj& operator=(const GdiObj&) = delete;
    GdiObj(GdiObj&& o) noexcept : h_(o.h_) { o.h_ = NULL; }
    GdiObj& operator=(GdiObj&& o) noexcept { if (this != &o) { if (h_) DeleteObject(h_); h_ = o.h_; o.h_ = NULL; } return *this; }
    T get() const { return h_; } operator T() const { return h_; }
    T release() { T t = h_; h_ = NULL; return t; }
};

class WinHandle {
    HANDLE h_;
public:
    explicit WinHandle(HANDLE h = NULL) : h_(h) {}
    ~WinHandle() { if (h_) CloseHandle(h_); }
    WinHandle(const WinHandle&) = delete; WinHandle& operator=(const WinHandle&) = delete;
    WinHandle(WinHandle&& o) noexcept : h_(o.h_) { o.h_ = NULL; }
    WinHandle& operator=(WinHandle&& o) noexcept { if (this != &o) { if (h_) CloseHandle(h_); h_ = o.h_; o.h_ = NULL; } return *this; }
    HANDLE get() const { return h_; } HANDLE release() { HANDLE t = h_; h_ = NULL; return t; }
    void reset(HANDLE h = NULL) { if (h_) CloseHandle(h_); h_ = h; }
};

class DcObj {
    HDC h_;
public:
    explicit DcObj(HDC h = NULL) : h_(h) {}
    ~DcObj() { if (h_) DeleteDC(h_); }
    DcObj(const DcObj&) = delete; DcObj& operator=(const DcObj&) = delete;
    DcObj(DcObj&& o) noexcept : h_(o.h_) { o.h_ = NULL; }
    DcObj& operator=(DcObj&& o) noexcept { if (this != &o) { if (h_) DeleteDC(h_); h_ = o.h_; o.h_ = NULL; } return *this; }
    HDC get() const { return h_; } operator HDC() const { return h_; }
    HDC release() { HDC t = h_; h_ = NULL; return t; }
};

struct ScopedSelectedObject {
    HDC dc = nullptr;
    HGDIOBJ previous = nullptr;
    ScopedSelectedObject(HDC target, HGDIOBJ object) : dc(target) {
        if (dc && object) previous = SelectObject(dc, object);
    }
    ~ScopedSelectedObject() {
        if (dc && previous && previous != HGDI_ERROR) SelectObject(dc, previous);
    }
    ScopedSelectedObject(const ScopedSelectedObject&) = delete;
    ScopedSelectedObject& operator=(const ScopedSelectedObject&) = delete;
};

struct ScopedTextColor {
    HDC dc = nullptr;
    COLORREF previous = 0;
    ScopedTextColor(HDC target, COLORREF color) : dc(target) {
        if (dc) previous = SetTextColor(dc, color);
    }
    ~ScopedTextColor() { if (dc) SetTextColor(dc, previous); }
    ScopedTextColor(const ScopedTextColor&) = delete;
    ScopedTextColor& operator=(const ScopedTextColor&) = delete;
};

struct ScopedBkMode {
    HDC dc = nullptr;
    int previous = 0;
    ScopedBkMode(HDC target, int mode) : dc(target) {
        if (dc) previous = SetBkMode(dc, mode);
    }
    ~ScopedBkMode() { if (dc) SetBkMode(dc, previous); }
    ScopedBkMode(const ScopedBkMode&) = delete;
    ScopedBkMode& operator=(const ScopedBkMode&) = delete;
};

struct ScopedComApartment {
    bool initialized = false;
    explicit ScopedComApartment(bool ok) : initialized(ok) {}
    ~ScopedComApartment() { if (initialized) CoUninitialize(); }
    ScopedComApartment(const ScopedComApartment&) = delete;
    ScopedComApartment& operator=(const ScopedComApartment&) = delete;
};

template <typename T>
class ScopedComPtr {
    T* p_;
public:
    explicit ScopedComPtr(T* p = nullptr) : p_(p) {}
    ~ScopedComPtr() { if (p_) p_->Release(); }
    ScopedComPtr(const ScopedComPtr&) = delete;
    ScopedComPtr& operator=(const ScopedComPtr&) = delete;
    ScopedComPtr(ScopedComPtr&& o) noexcept : p_(o.p_) { o.p_ = nullptr; }
    ScopedComPtr& operator=(ScopedComPtr&& o) noexcept {
        if (this != &o) { if (p_) p_->Release(); p_ = o.p_; o.p_ = nullptr; }
        return *this;
    }
    T* get() const { return p_; }
    T* operator->() const { return p_; }
    T** operator&() { return &p_; }
    explicit operator bool() const { return p_ != nullptr; }
    void reset(T* p = nullptr) { if (p_) p_->Release(); p_ = p; }
};

// ---------------------------------------------------------------- exception guard
static std::atomic<unsigned> g_exceptionCount{0};

template <typename Function>
static bool RunGuarded(const wchar_t* step, Function&& function) {
    try {
        function();
        return true;
    } catch (...) {
        const unsigned count = g_exceptionCount.fetch_add(1) + 1;
        if (count <= 5) {
            Wh_Log(L"[Guard] exception while %s", step);
        }
        return false;
    }
}

struct RateLimitedLog {
    const wchar_t* message;
    int limit;
    int count = 0;
    bool ShouldLog() { ++count; return count <= limit; }
};

// ---------------------------------------------------------------- state
enum Cat { CAT_APP = 0, CAT_SETTING = 1, CAT_FILE = 2 };
struct Item { std::wstring name, lower, path; Cat cat; bool dir; };

// Layout struct: needs to be fully defined before the g_cachedLay global.
struct Lay { int mx, titleY, lblY, editY, editH, editR, btnW, listY, rowH, vis; };

static const UINT WM_APP_TOGGLE = WM_APP + 1;
static const UINT WM_APP_INDEX  = WM_APP + 2;
static const wchar_t* kClass = L"Win81SearchCharmMod";
static const wchar_t* kMenuClass = L"Win81SearchCharmMenu";
static const ULONG_PTR kMagic = 0x53524348;
static const COLORREF KEY = RGB(255, 0, 255);

static std::vector<Item> g_items;
static SRWLOCK g_lock = SRWLOCK_INIT;
static std::vector<int> g_res;
static std::wstring g_query;
static int g_sel = 0, g_scroll = 0, g_hover = -1;
static bool g_caret = true;
static bool g_selectAll = false;
static bool g_inMenu = false;
static wchar_t g_pendingHigh = 0;

static HWND g_wnd = NULL;
static WinHandle g_uiThread, g_keyboardThread, g_scanThread;
static WinHandle g_uiReady, g_keyboardReady;
static DWORD g_uiTid = 0, g_keyboardTid = 0;
static HHOOK g_hook = NULL;
static volatile LONG g_uiStarted = 0, g_keyboardStarted = 0;
static volatile LONG g_stop = 0, g_scanning = 0;
static DWORD g_lastScan = 0, g_lastClick = 0, g_lastHotkey = 0;

static bool g_useWinS = true, g_useWinQ = true;
static int g_colorMode = 0, g_openMs = 300, g_closeMs = 250;
static COLORREF g_custom = RGB(8, 121, 0);
static int g_lang = 0;
static UINT g_swallowVk = 0;
static bool g_closeOnClickOutside = true, g_aboveTaskbar = true;
static bool g_useZBand = true;
static int g_clickCooldownMs = 500, g_hotkeyCooldownMs = 400;
static int g_monitorChoice = 0;
static bool g_debug = false;

enum { ST_IDLE, ST_IN, ST_OUT };
static int g_state = ST_IDLE;
static bool g_open = false;
static int g_w = 0, g_h = 0;
static double g_scale = 1.0;
static int g_paneOff = 0, g_contOff = 0;
static int g_fromPane, g_toPane, g_fromCont, g_dur, g_contDur;
static LONGLONG g_t0 = 0, g_freq = 1;

static HFONT g_fTitle, g_fLabel, g_fText, g_fName, g_fSub, g_fGlyph, g_fHeader;
static double g_fontScale = 0;
static std::unordered_map<std::wstring, HICON> g_icons;

enum { SCOPE_ALL, SCOPE_APPS, SCOPE_SETTINGS, SCOPE_FILES };
static int g_scope = SCOPE_ALL;
static std::vector<std::wstring> g_recent;

static bool g_dropdownOpen = false;
static int g_dropdownHover = -1;

// Scrollbar state
static bool g_scrollDragging = false;
static int  g_scrollDragY = 0;
static int  g_scrollDragScroll = 0;

// Metro context menu state
static HWND g_menuWnd = NULL;
static bool g_menuClassRegistered = false;
static std::vector<std::wstring> g_menuItems;
static std::vector<bool> g_menuIsSeparator;
static int g_menuHover = -1;
static int g_menuResult = 0;   // 0 = no selection, 1-based otherwise

// Sort mode (context menu)
enum { SORT_RELEVANCE = 0, SORT_NAME, SORT_DATE, SORT_MOST_USED, SORT_CATEGORY };
static int g_sortMode = SORT_RELEVANCE;

// Layout cache (Lay is fully defined above, so this global is valid)
static Lay g_cachedLay = {};
static int g_cachedLayW = -1;
static int g_cachedLayH = -1;
static double g_cachedLayScale = -1.0;

static int S(int v) { return (int)(v * g_scale + 0.5); }

// ---------------------------------------------------------------- forward declarations
static void EnforceTopmost();
static void OpenPane();
static void ClosePane();
static void DismissContextMenu();
static bool InEditBox(int x, int y);
static void Changed();
static bool LaunchPath(const std::wstring& path);
static void LaunchResult(int r);
static void EnsureVisible();
static std::wstring ClipText(HWND h);
static int RowAt(int y);
static int DropdownItemAt(int x, int y, int* outIndex);
static bool InDropdownArea(int x, int y);
static void ShowContextMenu(HWND h, int x, int y, int r);

// ---------------------------------------------------------------- z-band overlay (from 8102 Charms mod)
typedef HWND (WINAPI *CreateWindowInBand_t)(
    DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle,
    int X, int Y, int nWidth, int nHeight,
    HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam, DWORD dwBand);

static CreateWindowInBand_t g_createInBand = NULL;
static int g_band = -1;
static const DWORD kBandUIAccess = 2;
static const DWORD kBandSystemTools = 16;

static HWND CreateOverlayWindow(const wchar_t* cls, int w, int h) {
    const DWORD ex = WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_LAYERED;
    HINSTANCE hi = (HINSTANCE)&__ImageBase;

    HWND hw = NULL;
    if (g_createInBand && g_useZBand && g_band != 0) {
        const DWORD bands[2] = { kBandUIAccess, kBandSystemTools };
        for (DWORD b : bands) {
            if (g_band > 0 && (DWORD)g_band != b) continue;
            hw = g_createInBand(ex, cls, L"", WS_POPUP, 0, 0, w, h,
                                NULL, NULL, hi, NULL, b);
            if (hw) {
                if (g_band != (int)b)
                    Wh_Log(L"[ZBand] window created in band %u (above taskbar/Start/fullscreen)",
                           (unsigned)b);
                g_band = (int)b;
                break;
            }
            Wh_Log(L"[ZBand] CreateWindowInBand(%u) failed, err=%u",
                   (unsigned)b, (unsigned)GetLastError());
        }
        if (!hw && g_band < 0) {
            g_band = 0;
            Wh_Log(L"[ZBand] no higher band available, falling back to ordinary topmost");
        }
    }
    if (!hw)
        hw = CreateWindowExW(ex, cls, L"", WS_POPUP, 0, 0, w, h, NULL, NULL, hi, NULL);
    return hw;
}

struct EnumCtx { RECT r; int idx; };
static BOOL CALLBACK EnumTop(HWND w, LPARAM lp) {
    EnumCtx* c = (EnumCtx*)lp;
    c->idx++;
    if (!IsWindowVisible(w)) return TRUE;
    RECT wr, x;
    GetWindowRect(w, &wr);
    if (!IntersectRect(&x, &wr, &c->r)) return TRUE;
    wchar_t cls[96] = L"";
    GetClassNameW(w, cls, 96);
    DWORD pid = 0;
    GetWindowThreadProcessId(w, &pid);
    Wh_Log(L"[dump] z=%d hwnd=%p class=%s pid=%u ex=%08X rect=%d,%d,%d,%d%s",
           c->idx, w, cls, (unsigned)pid,
           (unsigned)GetWindowLongW(w, GWL_EXSTYLE),
           (int)wr.left, (int)wr.top, (int)wr.right, (int)wr.bottom,
           (w == g_wnd) ? L"  <== OUR PANEL" : L"");
    return TRUE;
}

static void DebugDump(const wchar_t* tag) {
    if (!g_debug || !g_wnd) return;
    HWND tray = FindWindowW(L"Shell_TrayWnd", NULL);
    Wh_Log(L"[dump] ---- %s ---- panel=%p band=%d pid=%u tray=%p",
           tag, g_wnd, g_band, (unsigned)GetCurrentProcessId(), tray);
    EnumCtx c;
    RECT r; GetWindowRect(g_wnd, &r);
    c.r = r;
    c.idx = 0;
    Wh_Log(L"[dump] top-level windows intersecting the panel, from the top of the z-order:");
    EnumWindows(EnumTop, (LPARAM)&c);
}

// ---------------------------------------------------------------- utility
static std::wstring Lower(std::wstring s) { for (auto& c : s) c = (wchar_t)towlower(c); return s; }

static COLORREF Blend(COLORREF bg, int pct) {
    int r = GetRValue(bg), g = GetGValue(bg), b = GetBValue(bg);
    r += (255 - r) * pct / 100; g += (255 - g) * pct / 100; b += (255 - b) * pct / 100;
    return RGB(r, g, b);
}

static COLORREF ParseHex(const wchar_t* t, COLORREF def) {
    if (!t) return def;
    while (*t == L' ') t++;
    if (*t == L'#') t++;
    wchar_t* e = NULL; unsigned long v = wcstoul(t, &e, 16);
    if (e == t) return def;
    return RGB((v >> 16) & 255, (v >> 8) & 255, v & 255);
}

static COLORREF PaneColor() {
    if (g_colorMode == 1) {
        DWORD c = 0; BOOL o = FALSE;
        if (SUCCEEDED(DwmGetColorizationColor(&c, &o)))
            return RGB((c >> 16) & 255, (c >> 8) & 255, c & 255);
    } else if (g_colorMode == 2) return g_custom;
    return RGB(8, 121, 0);
}

// Returns the system accent color (the same used by the Start screen and
// Charms bar on Windows 8). Falls back to the Metro blue if DWM does not
// provide a color.
static COLORREF SystemAccentColor() {
    DWORD c = 0; BOOL o = FALSE;
    if (SUCCEEDED(DwmGetColorizationColor(&c, &o)))
        return RGB((c >> 16) & 255, (c >> 8) & 255, c & 255);
    return RGB(0, 120, 215);
}

static void LoadSettings() {
    g_useWinS = Wh_GetIntSetting(L"useWinS") != 0;
    g_useWinQ = Wh_GetIntSetting(L"useWinQ") != 0;
    g_openMs = std::min(3000, std::max(80, (int)Wh_GetIntSetting(L"openMs")));
    g_closeMs = std::min(3000, std::max(80, (int)Wh_GetIntSetting(L"closeMs")));
    PCWSTR cm = Wh_GetStringSetting(L"colorMode");
    g_colorMode = (cm && !wcscmp(cm, L"system")) ? 1 : (cm && !wcscmp(cm, L"custom")) ? 2 : 0;
    Wh_FreeStringSetting(cm);
    PCWSTR cc = Wh_GetStringSetting(L"customColor");
    g_custom = ParseHex(cc, RGB(8, 121, 0));
    Wh_FreeStringSetting(cc);
    PCWSTR lg = Wh_GetStringSetting(L"language");
    if (lg && !wcscmp(lg, L"it")) g_lang = 0;
    else if (lg && !wcscmp(lg, L"en")) g_lang = 1;
    else if (lg && !wcscmp(lg, L"es")) g_lang = 2;
    else if (lg && !wcscmp(lg, L"fr")) g_lang = 3;
    else if (lg && !wcscmp(lg, L"pt")) g_lang = 4;
    else if (lg && !wcscmp(lg, L"de")) g_lang = 5;
    else if (lg && !wcscmp(lg, L"ru")) g_lang = 6;
    else if (lg && !wcscmp(lg, L"zh")) g_lang = 7;
    else if (lg && !wcscmp(lg, L"ja")) g_lang = 8;
    else if (lg && !wcscmp(lg, L"ko")) g_lang = 9;
    else {
        LANGID lid = GetUserDefaultUILanguage();
        switch (PRIMARYLANGID(lid)) {
            case LANG_ITALIAN: g_lang = 0; break;
            case LANG_SPANISH: g_lang = 2; break;
            case LANG_FRENCH: g_lang = 3; break;
            case LANG_PORTUGUESE: g_lang = 4; break;
            case LANG_GERMAN: g_lang = 5; break;
            case LANG_RUSSIAN: g_lang = 6; break;
            case LANG_CHINESE: g_lang = 7; break;
            case LANG_JAPANESE: g_lang = 8; break;
            case LANG_KOREAN: g_lang = 9; break;
            default: g_lang = 1; break;
        }
    }
    Wh_FreeStringSetting(lg);
    g_closeOnClickOutside = Wh_GetIntSetting(L"closeOnClickOutside") != 0;
    g_aboveTaskbar = Wh_GetIntSetting(L"aboveTaskbar") != 0;
    g_useZBand = Wh_GetIntSetting(L"useZBand") != 0;
    g_clickCooldownMs = std::min(2000, std::max(0, (int)Wh_GetIntSetting(L"clickCooldownMs")));
    g_hotkeyCooldownMs = std::min(2000, std::max(0, (int)Wh_GetIntSetting(L"hotkeyCooldownMs")));
    g_monitorChoice = std::max(0, (int)Wh_GetIntSetting(L"monitor"));
    g_debug = Wh_GetIntSetting(L"debugLog") != 0;
    Wh_Log(L"[Settings] lang=%d monitor=%d colorMode=%d zBand=%d debug=%d",
           g_lang, g_monitorChoice, g_colorMode, g_useZBand ? 1 : 0,
           g_debug ? 1 : 0);
}

// Strings: 0=it,1=en,2=es,3=fr,4=pt,5=de,6=ru,7=zh,8=ja,9=ko
static const wchar_t* T(int id) {
    static const wchar_t* S[10][27] = {
        { L"Cerca", L"Ovunque", L"Nessun risultato", L"Applicazione", L"Indicizzazione...",
          L"Pannello di controllo", L"Impostazioni", L"Esplora file", L"Prompt dei comandi",
          L"Task Manager", L"Programmi e funzionalità", L"File", L"Impostazioni", L"Applicazioni",
          L"Cronologia", L"Cerca \"%s\" sul Web", L"Apri percorso file", L"Esegui come amministratore",
          L"Copia percorso", L"Ovunque", L"Applicazioni", L"Impostazioni",
          L"per nome", L"per data installazione", L"per uso frequente", L"per categoria", L"Apri" },
        { L"Search", L"Everywhere", L"No results", L"App", L"Indexing...",
          L"Control Panel", L"Settings", L"File Explorer", L"Command Prompt",
          L"Task Manager", L"Programs and Features", L"Files", L"Settings", L"Apps",
          L"History", L"Search \"%s\" on the Web", L"Open file location", L"Run as administrator",
          L"Copy path", L"Everywhere", L"Apps", L"Settings",
          L"by name", L"by date installed", L"by most used", L"by category", L"Open" },
        { L"Buscar", L"En todas partes", L"Sin resultados", L"Aplicación", L"Indexando...",
          L"Panel de control", L"Configuración", L"Explorador de archivos", L"Símbolo del sistema",
          L"Administrador de tareas", L"Programas y características", L"Archivos", L"Configuración",
          L"Aplicaciones", L"Historial", L"Buscar \"%s\" en la Web", L"Abrir ubicación del archivo",
          L"Ejecutar como administrador", L"Copiar ruta", L"En todas partes", L"Aplicaciones", L"Configuración",
          L"por nombre", L"por fecha de instalación", L"por uso frecuente", L"por categoría", L"Abrir" },
        { L"Rechercher", L"Partout", L"Aucun résultat", L"Application", L"Indexation...",
          L"Panneau de configuration", L"Paramètres", L"Explorateur de fichiers", L"Invite de commandes",
          L"Gestionnaire des tâches", L"Programmes et fonctionnalités", L"Fichiers", L"Paramètres",
          L"Applications", L"Historique", L"Rechercher \"%s\" sur le Web", L"Ouvrir l'emplacement du fichier",
          L"Exécuter en tant qu'administrateur", L"Copier le chemin", L"Partout", L"Applications", L"Paramètres",
          L"par nom", L"par date d'installation", L"par utilisation fréquente", L"par catégorie", L"Ouvrir" },
        { L"Pesquisar", L"Em todo o lado", L"Sem resultados", L"Aplicativo", L"Indexando...",
          L"Painel de Controle", L"Configurações", L"Explorador de Arquivos", L"Prompt de Comando",
          L"Gerenciador de Tarefas", L"Programas e Recursos", L"Arquivos", L"Configurações",
          L"Aplicativos", L"Histórico", L"Pesquisar \"%s\" na Web", L"Abrir local do arquivo",
          L"Executar como administrador", L"Copiar caminho", L"Em todo o lado", L"Aplicativos", L"Configurações",
          L"por nome", L"por data de instalação", L"por uso frequente", L"por categoria", L"Abrir" },
        { L"Suchen", L"Überall", L"Keine Ergebnisse", L"App", L"Indizierung...",
          L"Systemsteuerung", L"Einstellungen", L"Datei-Explorer", L"Eingabeaufforderung",
          L"Task-Manager", L"Programme und Features", L"Dateien", L"Einstellungen", L"Apps",
          L"Verlauf", L"\"%s\" im Web suchen", L"Dateispeicherort öffnen", L"Als Administrator ausführen",
          L"Pfad kopieren", L"Überall", L"Apps", L"Einstellungen",
          L"nach Name", L"nach Installationsdatum", L"nach häufigster Nutzung", L"nach Kategorie", L"Öffnen" },
        { L"Поиск", L"Везде", L"Нет результатов", L"Приложение", L"Индексация...",
          L"Панель управления", L"Параметры", L"Проводник", L"Командная строка",
          L"Диспетчер задач", L"Программы и компоненты", L"Файлы", L"Параметры", L"Приложения",
          L"История", L"Искать \"%s\" в Интернете", L"Открыть расположение файла", L"Запуск от имени администратора",
          L"Копировать путь", L"Везде", L"Приложения", L"Параметры",
          L"по имени", L"по дате установки", L"по частоте использования", L"по категории", L"Открыть" },
        { L"搜索", L"随处", L"无结果", L"应用", L"正在索引...",
          L"控制面板", L"设置", L"文件资源管理器", L"命令提示符",
          L"任务管理器", L"程序和功能", L"文件", L"设置", L"应用",
          L"历史记录", L"在 Web 上搜索 \"%s\"", L"打开文件位置", L"以管理员身份运行",
          L"复制路径", L"随处", L"应用", L"设置",
          L"按名称", L"按安装日期", L"按使用频率", L"按类别", L"打开" },
        { L"検索", L"すべての場所", L"結果なし", L"アプリ", L"インデックス作成中...",
          L"コントロール パネル", L"設定", L"ファイル エクスプローラー", L"コマンド プロンプト",
          L"タスク マネージャー", L"プログラムと機能", L"ファイル", L"設定", L"アプリ",
          L"履歴", L"Web で \"%s\" を検索", L"ファイルの場所を開く", L"管理者として実行",
          L"パスをコピー", L"すべての場所", L"アプリ", L"設定",
          L"名前順", L"インストール日順", L"使用頻度順", L"カテゴリ順", L"開く" },
        { L"검색", L"모든 위치", L"결과 없음", L"앱", L"인덱싱 중...",
          L"제어판", L"설정", L"파일 탐색기", L"명령 프롬프트",
          L"작업 관리자", L"프로그램 및 기능", L"파일", L"설정", L"앱",
          L"기록", L"웹에서 \"%s\" 검색", L"파일 위치 열기", L"관리자 권한으로 실행",
          L"경로 복사", L"모든 위치", L"앱", L"설정",
          L"이름순", L"설치 날짜순", L"사용 빈도순", L"범주순", L"열기" }
    };
    int l = g_lang; if (l < 0 || l > 9) l = 1;
    if (id < 0 || id > 26) return L"";
    return S[l][id];
}

static HFONT MkFont(int px, const wchar_t* face, int weight = FW_NORMAL) {
    return CreateFontW(-px, 0, 0, 0, weight, 0, 0, 0, DEFAULT_CHARSET, 0, 0,
                       CLEARTYPE_QUALITY, 0, face);
}

static void EnsureFonts() {
    if (g_fontScale == g_scale && g_fTitle) return;
    HFONT* f[] = { &g_fTitle, &g_fLabel, &g_fText, &g_fName, &g_fSub, &g_fGlyph, &g_fHeader };
    for (auto p : f) if (*p) { DeleteObject(*p); *p = NULL; }
    g_fTitle = MkFont(S(29), L"Segoe UI Light");
    g_fLabel = MkFont(S(15), L"Segoe UI");
    g_fText  = MkFont(S(16), L"Segoe UI");
    g_fName  = MkFont(S(16), L"Segoe UI");
    g_fSub   = MkFont(S(12), L"Segoe UI");
    g_fGlyph = MkFont(S(16), L"Segoe MDL2 Assets");
    g_fHeader = MkFont(S(13), L"Segoe UI Semibold", FW_SEMIBOLD);
    g_fontScale = g_scale;
}

// ---------------------------------------------------------------- icons
static HICON GetIcon(const std::wstring& path);

static HICON GetSettingsIcon() {
    static HICON cached = nullptr;
    static bool tried = false;
    if (tried) return cached;
    tried = true;

    wchar_t sys[MAX_PATH] = {};
    GetWindowsDirectoryW(sys, MAX_PATH);
    std::wstring exe = std::wstring(sys) + L"\\ImmersiveControlPanel\\SystemSettings.exe";

    HICON large = nullptr, small = nullptr;
    if (ExtractIconExW(exe.c_str(), 0, &large, &small, 1) > 0) {
        cached = large;
        if (small) DestroyIcon(small);
        Wh_Log(L"[Icon] Settings icon loaded from %s", exe.c_str());
        return cached;
    }
    SHFILEINFOW fi = {};
    if (SHGetFileInfoW(exe.c_str(), 0, &fi, sizeof(fi),
                       SHGFI_ICON | SHGFI_LARGEICON)) {
        cached = fi.hIcon;
        return cached;
    }
    Wh_Log(L"[Icon] Settings icon unavailable for %s", exe.c_str());
    return nullptr;
}

static HICON GetShellIcon(const std::wstring& shellPath) {
    PIDLIST_ABSOLUTE pidl = nullptr;
    SFGAOF attr = 0;
    HRESULT hr = SHParseDisplayName(shellPath.c_str(), NULL, &pidl, 0, &attr);
    if (FAILED(hr) || !pidl) {
        static RateLimitedLog log{L"SHParseDisplayName failed", 5};
        if (log.ShouldLog())
            Wh_Log(L"[Icon] SHParseDisplayName failed for %s (hr=0x%08X)",
                   shellPath.c_str(), hr);
        return nullptr;
    }

    HICON result = nullptr;
    ScopedComPtr<IShellFolder> psf;
    PCUITEMID_CHILD child = nullptr;
    hr = SHBindToParent(pidl, IID_IShellFolder, (void**)&psf, &child);
    if (SUCCEEDED(hr) && psf && child) {
        ScopedComPtr<IExtractIconW> pei;
        hr = psf->GetUIObjectOf(NULL, 1, &child, IID_IExtractIconW, NULL, (void**)&pei);
        if (SUCCEEDED(hr) && pei) {
            wchar_t iconFile[MAX_PATH] = {}; int iconIndex = 0; UINT flags = 0;
            hr = pei->GetIconLocation(GIL_FORSHELL, iconFile, MAX_PATH, &iconIndex, &flags);
            if (SUCCEEDED(hr)) {
                if (flags & GIL_NOTFILENAME) {
                    pei->Extract(iconFile, iconIndex, &result, nullptr, S(32));
                } else if (iconFile[0]) {
                    HICON large = nullptr, small = nullptr;
                    if (ExtractIconExW(iconFile, iconIndex, &large, &small, 1) > 0) {
                        result = large; if (small) DestroyIcon(small);
                    }
                }
            }
        }
    }

    if (!result) {
        SHFILEINFOW fi = {};
        if (SHGetFileInfoW((LPCWSTR)pidl, 0, &fi, sizeof(fi),
                           SHGFI_PIDL | SHGFI_ICON | SHGFI_LARGEICON)) {
            result = fi.hIcon;
        }
    }
    CoTaskMemFree(pidl);
    return result;
}

static HICON GetIconNoOverlay(const std::wstring& path) {
    SHFILEINFOW sfi = {};
    DWORD_PTR rawImageList = SHGetFileInfoW(
        path.c_str(), 0, &sfi, sizeof(sfi),
        SHGFI_SYSICONINDEX | SHGFI_LARGEICON);

    if (!rawImageList) return nullptr;

    HIMAGELIST imageList = (HIMAGELIST)rawImageList;
    return ImageList_GetIcon(imageList, sfi.iIcon, ILD_TRANSPARENT);
}

static HICON GetIcon(const std::wstring& path) {
    auto it = g_icons.find(path);
    if (it != g_icons.end()) return it->second;
    if (g_icons.size() > 500) {
        for (auto& kv : g_icons) if (kv.second) DestroyIcon(kv.second);
        g_icons.clear();
    }

    HICON ic = nullptr;

    if (path.rfind(L"ms-settings:", 0) == 0 || path.rfind(L"ms-", 0) == 0) {
        ic = GetSettingsIcon();
    }
    if (!ic && (path.rfind(L"shell:::", 0) == 0 || path.rfind(L"::{", 0) == 0 ||
                path.rfind(L"shell:", 0) == 0)) {
        ic = GetShellIcon(path);
    }
    if (!ic) {
        ic = GetIconNoOverlay(path);
        if (!ic) {
            SHFILEINFOW fi = {};
            if (SHGetFileInfoW(path.c_str(), 0, &fi, sizeof(fi),
                               SHGFI_ICON | SHGFI_LARGEICON)) {
                ic = fi.hIcon;
            }
        }
    }
    if (!ic) {
        static RateLimitedLog log{L"icon missing", 5};
        if (log.ShouldLog()) Wh_Log(L"[Icon] No icon for: %s", path.c_str());
    }
    g_icons[path] = ic;
    return ic;
}

// ---------------------------------------------------------------- index
static void ScanDir(std::vector<Item>& out, const std::wstring& dir, int depth,
                    bool appsMode, std::set<std::wstring>& seen, size_t cap) {
    if (g_stop || depth > 8 || out.size() >= cap) return;
    WIN32_FIND_DATAW fd;
    HANDLE raw = FindFirstFileW((dir + L"\\*").c_str(), &fd);
    if (raw == INVALID_HANDLE_VALUE) return;
    WinHandle h(raw);
    do {
        if (g_stop || out.size() >= cap) break;
        if (fd.cFileName[0] == L'.') continue;
        if (!appsMode && (fd.dwFileAttributes & (FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM |
                                                  FILE_ATTRIBUTE_REPARSE_POINT)))
            continue;
        std::wstring full = dir + L"\\" + fd.cFileName;
        bool isDir = (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
        if (isDir) {
            if (!appsMode) {
                if (!_wcsicmp(fd.cFileName, L"node_modules")) continue;
                Item it{ fd.cFileName, Lower(fd.cFileName), full, CAT_FILE, true };
                out.push_back(std::move(it));
            }
            ScanDir(out, full, depth + 1, appsMode, seen, cap);
        } else if (appsMode) {
            size_t n = wcslen(fd.cFileName);
            if (n < 5 || _wcsicmp(fd.cFileName + n - 4, L".lnk")) continue;
            std::wstring name(fd.cFileName, n - 4);
            std::wstring lw = Lower(name);
            if (!seen.insert(lw).second) continue;
            Item it{ name, lw, full, CAT_APP, false };
            out.push_back(std::move(it));
        } else {
            Item it{ fd.cFileName, Lower(fd.cFileName), full, CAT_FILE, false };
            out.push_back(std::move(it));
        }
    } while (FindNextFileW(h.get(), &fd));
}

static void ScanAppsFolder(std::vector<Item>& out, std::set<std::wstring>& seen) {
    PWSTR appsFolderPath = nullptr;
    HRESULT hr = SHGetKnownFolderPath(FOLDERID_AppsFolder, 0, NULL, &appsFolderPath);
    if (FAILED(hr) || !appsFolderPath) {
        Wh_Log(L"[Scan] FOLDERID_AppsFolder unavailable (hr=0x%08X)", hr);
        return;
    }

    PIDLIST_ABSOLUTE pidl = nullptr;
    SFGAOF attrs = 0;
    hr = SHParseDisplayName(appsFolderPath, NULL, &pidl, 0, &attrs);
    CoTaskMemFree(appsFolderPath);

    if (FAILED(hr) || !pidl) {
        Wh_Log(L"[Scan] SHParseDisplayName(AppsFolder) failed (hr=0x%08X)", hr);
        return;
    }

    ScopedComPtr<IShellFolder> psfDesktop;
    hr = SHGetDesktopFolder(&psfDesktop);
    if (FAILED(hr) || !psfDesktop) {
        Wh_Log(L"[Scan] SHGetDesktopFolder failed (hr=0x%08X)", hr);
        CoTaskMemFree(pidl);
        return;
    }

    ScopedComPtr<IShellFolder> psfApps;
    hr = psfDesktop->BindToObject(pidl, NULL, IID_IShellFolder, (void**)&psfApps);
    CoTaskMemFree(pidl);

    if (FAILED(hr) || !psfApps) {
        Wh_Log(L"[Scan] BindToObject(AppsFolder) failed (hr=0x%08X)", hr);
        return;
    }

    ScopedComPtr<IEnumIDList> enumIds;
    hr = psfApps->EnumObjects(NULL, SHCONTF_FOLDERS | SHCONTF_NONFOLDERS, &enumIds);
    if (FAILED(hr) || !enumIds) {
        Wh_Log(L"[Scan] EnumObjects(AppsFolder) failed (hr=0x%08X)", hr);
        return;
    }

    LPITEMIDLIST childPidl = nullptr;
    ULONG fetched = 0;
    int count = 0;
    while (enumIds->Next(1, &childPidl, &fetched) == S_OK && fetched == 1) {
        if (g_stop) { CoTaskMemFree(childPidl); break; }

        wchar_t nameBuf[MAX_PATH] = {};
        STRRET strret = {};
        if (SUCCEEDED(psfApps->GetDisplayNameOf(childPidl, SHGDN_NORMAL, &strret))) {
            StrRetToBufW(&strret, childPidl, nameBuf, ARRAYSIZE(nameBuf));
        }

        wchar_t aumid[512] = {};
        strret = {};
        if (SUCCEEDED(psfApps->GetDisplayNameOf(childPidl, SHGDN_FORPARSING, &strret))) {
            StrRetToBufW(&strret, childPidl, aumid, ARRAYSIZE(aumid));
        }

        CoTaskMemFree(childPidl);
        if (!nameBuf[0] || !aumid[0]) continue;

        std::wstring name = nameBuf;
        std::wstring lw = Lower(name);
        if (!seen.insert(lw).second) continue;

        Item it;
        it.name = name;
        it.lower = lw;
        it.path = std::wstring(L"shell:AppsFolder\\") + aumid;
        it.cat = CAT_APP;
        it.dir = false;
        out.push_back(std::move(it));
        count++;
    }

    Wh_Log(L"[Scan] AppsFolder: added %d modern apps", count);
}

static void AddSystemEntries(std::vector<Item>& v, std::set<std::wstring>& seen) {
    struct Ent { int labelId; const wchar_t* target; Cat cat; };
    const Ent ents[] = {
        { 5,  L"shell:::{26EE0668-A00A-44D7-9371-BEB064C98683}", CAT_SETTING },
        { 6,  L"ms-settings:",                                    CAT_SETTING },
        { 7,  L"shell:MyComputerFolder",                          CAT_APP     },
        { 8,  L"cmd.exe",                                         CAT_APP     },
        { 9,  L"taskmgr.exe",                                     CAT_APP     },
        {10,  L"ms-settings:appsfeatures",                        CAT_SETTING },
    };
    for (auto& e : ents) {
        std::wstring name = T(e.labelId);
        std::wstring lw = Lower(name);
        if (!seen.insert(lw).second) continue;
        Item it;
        it.name = name; it.lower = lw; it.path = e.target;
        it.cat = e.cat; it.dir = false;
        v.push_back(std::move(it));
    }
}

static DWORD WINAPI ScanThread(LPVOID) {
    RunGuarded(L"scanning the index", [] {
        ScopedComApartment com(SUCCEEDED(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED)));
        std::vector<Item> v;
        std::set<std::wstring> seen;

        AddSystemEntries(v, seen);
        ScanAppsFolder(v, seen);

        const KNOWNFOLDERID* apps[] = { &FOLDERID_Programs, &FOLDERID_CommonPrograms };
        for (auto id : apps) {
            PWSTR p = NULL;
            if (SUCCEEDED(SHGetKnownFolderPath(*id, 0, NULL, &p)) && p) {
                ScanDir(v, p, 0, true, seen, 100000);
                CoTaskMemFree(p);
            }
        }
        const KNOWNFOLDERID* files[] = { &FOLDERID_Desktop, &FOLDERID_Documents, &FOLDERID_Downloads,
                                         &FOLDERID_Pictures, &FOLDERID_Music, &FOLDERID_Videos };
        for (auto id : files) {
            PWSTR p = NULL;
            if (SUCCEEDED(SHGetKnownFolderPath(*id, 0, NULL, &p)) && p) {
                ScanDir(v, p, 0, false, seen, 80000);
                CoTaskMemFree(p);
            }
        }

        Wh_Log(L"[Scan] indexed %zu items", v.size());

        if (!g_stop) {
            { SrwGuard g(&g_lock, true); g_items.swap(v); }
            if (g_wnd) PostMessageW(g_wnd, WM_APP_INDEX, 0, 0);
        }
    });
    InterlockedExchange(&g_scanning, 0);
    return 0;
}

static void StartScan() {
    if (InterlockedCompareExchange(&g_scanning, 1, 0) != 0) return;
    g_scanThread.reset();
    g_lastScan = GetTickCount();
    HANDLE raw = CreateThread(NULL, 0, ScanThread, NULL, 0, NULL);
    g_scanThread.reset(raw);
    if (!g_scanThread.get()) InterlockedExchange(&g_scanning, 0);
}

// ---------------------------------------------------------------- search
static void Refilter() {
    g_res.clear(); g_sel = 0; g_scroll = 0; g_hover = -1;
    if (g_query.empty()) return;
    std::wstring q = Lower(g_query);
    std::vector<std::wstring> words;
    size_t st = 0;
    for (size_t i = 0; i <= q.size(); i++) {
        if (i == q.size() || q[i] == L' ') {
            if (i > st) words.push_back(q.substr(st, i - st));
            st = i + 1;
        }
    }
    if (words.empty()) return;
    struct H { int i; int rank; };
    std::vector<H> hits;
    {
        SrwGuard g(&g_lock, false);
        for (size_t i = 0; i < g_items.size(); i++) {
            const Item& it = g_items[i];
            if (g_scope == SCOPE_APPS     && it.cat != CAT_APP)     continue;
            if (g_scope == SCOPE_SETTINGS && it.cat != CAT_SETTING) continue;
            if (g_scope == SCOPE_FILES    && it.cat != CAT_FILE)    continue;
            bool all = true;
            for (auto& w : words)
                if (it.lower.find(w) == std::wstring::npos) { all = false; break; }
            if (!all) continue;
            int rank = (it.cat == CAT_APP ? 0 : it.cat == CAT_SETTING ? 2 : 4)
                     + (it.lower.rfind(q, 0) == 0 ? 0 : 2)
                     + (it.dir ? 1 : 0);
            hits.push_back({ (int)i, rank });
        }
        if (g_sortMode == SORT_NAME) {
            std::sort(hits.begin(), hits.end(), [&](const H& a, const H& b) {
                return g_items[a.i].lower < g_items[b.i].lower;
            });
        } else if (g_sortMode == SORT_DATE) {
            std::sort(hits.begin(), hits.end(), [&](const H& a, const H& b) {
                WIN32_FILE_ATTRIBUTE_DATA fa, fb;
                GetFileAttributesExW(g_items[a.i].path.c_str(), GetFileExInfoStandard, &fa);
                GetFileAttributesExW(g_items[b.i].path.c_str(), GetFileExInfoStandard, &fb);
                return CompareFileTime(&fa.ftLastWriteTime, &fb.ftLastWriteTime) > 0;
            });
        } else if (g_sortMode == SORT_MOST_USED) {
            std::sort(hits.begin(), hits.end(), [&](const H& a, const H& b) {
                WIN32_FILE_ATTRIBUTE_DATA fa, fb;
                GetFileAttributesExW(g_items[a.i].path.c_str(), GetFileExInfoStandard, &fa);
                GetFileAttributesExW(g_items[b.i].path.c_str(), GetFileExInfoStandard, &fb);
                return CompareFileTime(&fa.ftLastAccessTime, &fb.ftLastAccessTime) > 0;
            });
        } else if (g_sortMode == SORT_CATEGORY) {
            std::sort(hits.begin(), hits.end(), [&](const H& a, const H& b) {
                return g_items[a.i].cat < g_items[b.i].cat;
            });
        } else {
            std::sort(hits.begin(), hits.end(), [&](const H& a, const H& b) {
                if (a.rank != b.rank) return a.rank < b.rank;
                return g_items[a.i].lower < g_items[b.i].lower;
            });
        }
    }
    for (size_t i = 0; i < hits.size() && i < 300; i++) g_res.push_back(hits[i].i);
}

// ---------------------------------------------------------------- layout (cached)
static Lay Layout() {
    if (g_cachedLayW == g_w && g_cachedLayH == g_h && g_cachedLayScale == g_scale) {
        return g_cachedLay;
    }
    Lay l;
    l.mx = S(41); l.titleY = S(35); l.lblY = S(80);
    l.editY = S(110); l.editH = S(32);
    l.editR = g_w - S(40); l.btnW = S(32);
    l.listY = S(168); l.rowH = S(52);
    l.vis = std::max(1, (g_h - l.listY - S(20)) / l.rowH);
    g_cachedLay = l;
    g_cachedLayW = g_w;
    g_cachedLayH = g_h;
    g_cachedLayScale = g_scale;
    return l;
}

// ---------------------------------------------------------------- scrollbar
static RECT ScrollTrackRect() {
    RECT r;
    r.left   = g_w - S(8);
    r.right  = g_w - S(3);
    r.top    = Layout().listY;
    r.bottom = g_h - S(20);
    if (r.bottom <= r.top) r.bottom = r.top + 1;
    return r;
}

static RECT ScrollThumbRect() {
    RECT track = ScrollTrackRect();
    int total = (int)g_res.size();
    int vis = Layout().vis;
    RECT empty = { 0, 0, 0, 0 };
    if (total <= vis || total <= 0) return empty;
    int trackH = track.bottom - track.top;
    if (trackH <= 0) return empty;
    int thumbH = std::max(S(24), trackH * vis / total);
    if (thumbH > trackH) thumbH = trackH;
    int maxScroll = total - vis;
    int scroll = std::min(std::max(0, g_scroll), maxScroll);
    int thumbY = track.top + (trackH - thumbH) * scroll / std::max(1, maxScroll);
    RECT r;
    r.left = track.left;
    r.right = track.right;
    r.top = thumbY;
    r.bottom = thumbY + thumbH;
    return r;
}

static bool InScrollbarRegion(int x, int y) {
    int total = (int)g_res.size();
    if (total <= Layout().vis) return false;
    RECT track = ScrollTrackRect();
    int hitLeft  = track.left - S(7);
    int hitRight = g_w;
    return x >= hitLeft && x < hitRight && y >= track.top - S(4) && y < track.bottom + S(4);
}

// ---------------------------------------------------------------- painting
static void DoPaint(HWND h) {
    PAINTSTRUCT ps;
    HDC dc = BeginPaint(h, &ps);
    RECT rc; GetClientRect(h, &rc);
    int W = rc.right, H = rc.bottom;
    if (W <= 0 || H <= 0) { EndPaint(h, &ps); return; }

    DcObj mem(CreateCompatibleDC(dc));
    GdiObj<HBITMAP> bmp(CreateCompatibleBitmap(dc, W, H));
    ScopedSelectedObject selBmp(mem.get(), bmp.get());

    { GdiObj<HBRUSH> kb(CreateSolidBrush(KEY)); FillRect(mem.get(), &rc, kb.get()); }

    COLORREF pane = PaneColor();
    int px = std::max(0, g_paneOff);
    if (px < W) {
        RECT pr = { px, 0, W, H };
        GdiObj<HBRUSH> b(CreateSolidBrush(pane));
        FillRect(mem.get(), &pr, b.get());
    }
    IntersectClipRect(mem.get(), px, 0, W, H);
    ScopedBkMode bkMode(mem.get(), TRANSPARENT);
    EnsureFonts();
    Lay L = Layout();
    int ox = g_paneOff + g_contOff;
    int oxT = g_paneOff + g_contOff * 55 / 100;

    {
        ScopedTextColor tc(mem.get(), RGB(255, 255, 255));
        ScopedSelectedObject so(mem.get(), g_fTitle);
        TextOutW(mem.get(), L.mx + oxT, L.titleY, T(0), lstrlenW(T(0)));
        SelectObject(mem.get(), g_fLabel);
        TextOutW(mem.get(), L.mx + ox, L.lblY, T(1), lstrlenW(T(1)));
    }

    RECT box = { L.mx + ox, L.editY, L.editR - L.btnW + ox, L.editY + L.editH };
    {
        GdiObj<HBRUSH> wb(CreateSolidBrush(RGB(255, 255, 255)));
        FillRect(mem.get(), &box, wb.get());
        RECT btn = { L.editR - L.btnW + ox, L.editY, L.editR + ox, L.editY + L.editH };
        GdiObj<HBRUSH> hb(CreateSolidBrush(pane));
        FillRect(mem.get(), &btn, hb.get());
        FrameRect(mem.get(), &btn, wb.get());

        ScopedSelectedObject so(mem.get(), g_fGlyph);
        ScopedTextColor tc(mem.get(), RGB(255, 255, 255));
        const wchar_t mag[2] = { 0xE721, 0 };
        DrawTextW(mem.get(), mag, 1, &btn, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);

        if (!g_query.empty()) {
            RECT clearBtn = { L.editR - L.btnW - S(24) + ox, L.editY, L.editR - L.btnW + ox, L.editY + L.editH };
            const wchar_t clearChar[2] = { 0xE10A, 0 };
            DrawTextW(mem.get(), clearChar, 1, &clearBtn, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
        }
    }

    {
        ScopedSelectedObject so(mem.get(), g_fText);
        RECT tr = { box.left + S(6), box.top, box.right - S(6), box.bottom };
        SIZE sz = {};
        GetTextExtentPoint32W(mem.get(), g_query.c_str(), (int)g_query.size(), &sz);
        bool tail = sz.cx > (tr.right - tr.left - S(4));

        if (g_selectAll && !g_query.empty()) {
            RECT sel = { tr.left, box.top + S(2), tr.left + sz.cx, box.bottom - S(2) };
            if (sel.right > tr.right) sel.right = tr.right;
            GdiObj<HBRUSH> sb(CreateSolidBrush(RGB(0, 120, 215)));
            FillRect(mem.get(), &sel, sb.get());
            SetTextColor(mem.get(), RGB(255, 255, 255));
        } else {
            SetTextColor(mem.get(), RGB(0, 0, 0));
        }

        DrawTextW(mem.get(), g_query.c_str(), (int)g_query.size(), &tr,
                  DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX | (tail ? DT_RIGHT : DT_LEFT));

        if (g_caret && !g_selectAll) {
            int cx = tail ? tr.right : tr.left + sz.cx;
            GdiObj<HPEN> pen(CreatePen(PS_SOLID, 1, RGB(0, 0, 0)));
            HGDIOBJ op = SelectObject(mem.get(), pen.get());
            MoveToEx(mem.get(), cx, box.top + S(6), NULL);
            LineTo(mem.get(), cx, box.bottom - S(6));
            SelectObject(mem.get(), op);
        }
    }

    {
        SrwGuard g(&g_lock, false);
        if (!g_query.empty() && g_res.empty()) {
            ScopedSelectedObject so(mem.get(), g_fText);
            ScopedTextColor tc(mem.get(), RGB(255, 255, 255));
            const wchar_t* msg = (g_scanning && g_items.empty()) ? T(4) : T(2);
            TextOutW(mem.get(), L.mx + ox, L.listY, msg, lstrlenW(msg));
        }

        int y = L.listY;
        for (int r = g_scroll; r < (int)g_res.size() && r < g_scroll + L.vis; r++, y += L.rowH) {
            int idx = g_res[r];
            if (idx < 0 || idx >= (int)g_items.size()) continue;
            const Item& it = g_items[idx];

            if (r == g_sel || r == g_hover) {
                RECT band = { px, y, W, y + L.rowH };
                GdiObj<HBRUSH> bb(CreateSolidBrush(Blend(pane, r == g_sel ? 24 : 14)));
                FillRect(mem.get(), &band, bb.get());
            }
            HICON ic = GetIcon(it.path);
            if (ic)
                DrawIconEx(mem.get(), L.mx + ox, y + (L.rowH - S(32)) / 2, ic, S(32), S(32), 0, NULL, DI_NORMAL);
            int tx = L.mx + ox + S(44);
            RECT nr = { tx, y + S(6), W - S(20) + ox, y + S(28) };
            RECT sr = { tx, y + S(28), W - S(20) + ox, y + S(48) };

            {
                ScopedSelectedObject so(mem.get(), g_fName);
                ScopedTextColor tc(mem.get(), RGB(255, 255, 255));
                DrawTextW(mem.get(), it.name.c_str(), -1, &nr, DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX);
            }
            {
                ScopedSelectedObject so(mem.get(), g_fSub);
                ScopedTextColor tc(mem.get(), Blend(pane, 70));
                if (it.cat == CAT_APP) {
                    DrawTextW(mem.get(), T(3), -1, &sr, DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX);
                } else if (it.cat == CAT_SETTING) {
                    DrawTextW(mem.get(), T(12), -1, &sr, DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX);
                } else {
                    wchar_t parent[MAX_PATH];
                    StringCchCopyW(parent, MAX_PATH, it.path.c_str());
                    PathRemoveFileSpecW(parent);
                    DrawTextW(mem.get(), parent, -1, &sr,
                              DT_SINGLELINE | DT_PATH_ELLIPSIS | DT_NOPREFIX);
                }
            }
        }

        // Vertical scrollbar
        {
            RECT track = ScrollTrackRect();
            RECT thumb = ScrollThumbRect();
            if (thumb.bottom > thumb.top) {
                GdiObj<HBRUSH> tb(CreateSolidBrush(Blend(pane, 12)));
                FillRect(mem.get(), &track, tb.get());
                int thumbShade = g_scrollDragging ? 65 : (g_hover == -2 ? 50 : 38);
                GdiObj<HBRUSH> thb(CreateSolidBrush(Blend(pane, thumbShade)));
                FillRect(mem.get(), &thumb, thb.get());
            }
        }
    }

    SelectClipRgn(mem.get(), NULL);
    BitBlt(dc, 0, 0, W, H, mem.get(), 0, 0, SRCCOPY);
    EndPaint(h, &ps);
}

// ---------------------------------------------------------------- animation
static LONGLONG Qpc() { LARGE_INTEGER c; QueryPerformanceCounter(&c); return c.QuadPart; }

static double Ease(double t) {
    if (t <= 0) return 0; if (t >= 1) return 1;
    double u = 1 - t;
    return 1 - u*u*u;
}

static void StartAnim(bool in) {
    if (in && g_state == ST_IN) return;
    if (!in && g_state == ST_OUT) return;
    g_open = in;
    g_fromPane = g_paneOff;
    g_toPane = in ? 0 : g_w;
    g_fromCont = g_contOff;
    double frac = g_w ? fabs((double)(g_fromPane - g_toPane)) / g_w : 1.0;
    int full = in ? g_openMs : g_closeMs;
    g_dur = std::max(80, (int)(full * frac));
    g_contDur = in ? (int)(g_dur * 1.5) : g_dur;
    g_state = in ? ST_IN : ST_OUT;
    g_t0 = Qpc();
    SetTimer(g_wnd, 1, 8, NULL);
}

static void Tick() {
    double ms = (double)(Qpc() - g_t0) * 1000.0 / (double)g_freq;
    double t = std::min(1.0, ms / g_dur);
    double tc = std::min(1.0, ms / g_contDur);
    double dp = (g_toPane - g_fromPane) * Ease(t);
    if (fabs((g_toPane - g_fromPane) - dp) < 0.75) dp = g_toPane - g_fromPane;
    g_paneOff = g_fromPane + (int)(dp + (dp >= 0 ? 0.5 : -0.5));
    double dc2 = (0 - g_fromCont) * Ease(tc);
    if (fabs(g_fromCont + dc2) < 0.75) dc2 = -g_fromCont;
    g_contOff = g_fromCont + (int)(dc2 + (dc2 >= 0 ? 0.5 : -0.5));
    if (t >= 1.0 && tc >= 1.0) {
        g_paneOff = g_toPane; g_contOff = 0;
        KillTimer(g_wnd, 1);
        if (g_state == ST_OUT) ShowWindow(g_wnd, SW_HIDE);
        g_state = ST_IDLE;
    }
    RedrawWindow(g_wnd, NULL, NULL, RDW_INVALIDATE | RDW_NOERASE | RDW_UPDATENOW);
}

// ---------------------------------------------------------------- Metro context menu
static int MenuItemHeight() { return S(34); }
static int MenuPadY()       { return S(6);  }
static int MenuPadX()       { return S(14); }
static int MenuWidth()      { return S(220); }

static void MenuPaint(HWND h) {
    PAINTSTRUCT ps;
    HDC dc = BeginPaint(h, &ps);
    RECT rc; GetClientRect(h, &rc);

    GdiObj<HBRUSH> bg(CreateSolidBrush(RGB(255, 255, 255)));
    FillRect(dc, &rc, bg.get());

    GdiObj<HPEN> border(CreatePen(PS_SOLID, 1, RGB(200, 200, 200)));
    HGDIOBJ oldPen = SelectObject(dc, border.get());
    HGDIOBJ oldBrush = SelectObject(dc, GetStockObject(NULL_BRUSH));
    Rectangle(dc, rc.left, rc.top, rc.right, rc.bottom);
    SelectObject(dc, oldBrush);
    SelectObject(dc, oldPen);

    HFONT font = CreateFontW(-S(15), 0, 0, 0, FW_NORMAL, 0, 0, 0,
                             DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, L"Segoe UI");
    HGDIOBJ oldFont = SelectObject(dc, font);
    SetBkMode(dc, TRANSPARENT);

    int itemH = MenuItemHeight();
    int padY = MenuPadY();
    int padX = MenuPadX();

    COLORREF accent = SystemAccentColor();

    for (size_t i = 0; i < g_menuItems.size(); ++i) {
        RECT r = { 1, padY + (int)i * itemH, rc.right - 1, padY + (int)(i + 1) * itemH };
        bool isSep = g_menuIsSeparator[i];
        bool hover = ((int)i == g_menuHover) && !isSep;

        if (isSep) {
            GdiObj<HBRUSH> sep(CreateSolidBrush(RGB(224, 224, 224)));
            RECT sr = { padX, r.top + itemH / 2 - 1, rc.right - padX, r.top + itemH / 2 };
            FillRect(dc, &sr, sep.get());
            continue;
        }

        if (hover) {
            GdiObj<HBRUSH> hb(CreateSolidBrush(accent));
            FillRect(dc, &r, hb.get());
            SetTextColor(dc, RGB(255, 255, 255));
        } else {
            SetTextColor(dc, RGB(32, 32, 32));
        }

        RECT tr = r;
        tr.left += padX;
        DrawTextW(dc, g_menuItems[i].c_str(), -1, &tr,
                  DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX);
    }

    SelectObject(dc, oldFont);
    DeleteObject(font);
    EndPaint(h, &ps);
}

static LRESULT CALLBACK MenuWndProc(HWND h, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
    case WM_PAINT:
        MenuPaint(h);
        return 0;
    case WM_ERASEBKGND:
        return 1;
    case WM_MOUSEMOVE: {
        int y = (short)HIWORD(lp);
        int itemH = MenuItemHeight();
        int padY = MenuPadY();
        int idx = (y - padY) / itemH;
        if (idx < 0 || idx >= (int)g_menuItems.size() || g_menuIsSeparator[idx])
            idx = -1;
        if (idx != g_menuHover) {
            g_menuHover = idx;
            InvalidateRect(h, NULL, FALSE);
        }
        return 0;
    }
    case WM_LBUTTONDOWN: {
        int y = (short)HIWORD(lp);
        int itemH = MenuItemHeight();
        int padY = MenuPadY();
        int idx = (y - padY) / itemH;
        if (idx >= 0 && idx < (int)g_menuItems.size() && !g_menuIsSeparator[idx]) {
            g_menuResult = idx + 1;
        }
        PostMessageW(h, WM_CLOSE, 0, 0);
        return 0;
    }
    case WM_RBUTTONDOWN:
        PostMessageW(h, WM_CLOSE, 0, 0);
        return 0;
    case WM_KEYDOWN:
        if (wp == VK_ESCAPE) {
            g_menuResult = 0;
            PostMessageW(h, WM_CLOSE, 0, 0);
        }
        return 0;
    case WM_KILLFOCUS:
        PostMessageW(h, WM_CLOSE, 0, 0);
        return 0;
    case WM_CLOSE:
        DestroyWindow(h);
        return 0;
    case WM_DESTROY:
        if (g_menuWnd == h) g_menuWnd = NULL;
        return 0;
    }
    return DefWindowProcW(h, msg, wp, lp);
}

static bool EnsureMenuClass() {
    if (g_menuClassRegistered) return true;

    WNDCLASSW wc = {};
    wc.style = CS_DROPSHADOW;
    wc.lpfnWndProc = MenuWndProc;
    wc.hInstance = (HINSTANCE)&__ImageBase;
    wc.hCursor = LoadCursorW(NULL, IDC_ARROW);
    wc.lpszClassName = kMenuClass;
    if (!RegisterClassW(&wc)) {
        Wh_Log(L"[Menu] RegisterClassW failed, err=%u", (unsigned)GetLastError());
        return false;
    }

    g_menuClassRegistered = true;
    return true;
}

static void UnregisterMenuClass() {
    if (g_menuClassRegistered) {
        UnregisterClassW(kMenuClass, (HINSTANCE)&__ImageBase);
        g_menuClassRegistered = false;
    }
}

static void ShowContextMenu(HWND h, int x, int y, int r) {
    if (r < 0 || r >= (int)g_res.size()) return;
    std::wstring path;
    { SrwGuard g(&g_lock, false); int idx = g_res[r]; if (idx >= 0 && idx < (int)g_items.size()) path = g_items[idx].path; }
    if (path.empty()) return;

    if (g_menuWnd) { DestroyWindow(g_menuWnd); g_menuWnd = NULL; }

    g_menuItems.clear();
    g_menuIsSeparator.clear();

    // "open" is the documented Shell verb for executable and document files.
    // ShellExecuteExW resolves the user's registered file association, so the
    // same command opens .exe files and associated documents such as .txt.
    g_menuItems.push_back(T(26)); g_menuIsSeparator.push_back(false);
    g_menuItems.push_back(T(16)); g_menuIsSeparator.push_back(false);
    g_menuItems.push_back(T(17)); g_menuIsSeparator.push_back(false);
    g_menuItems.push_back(T(18)); g_menuIsSeparator.push_back(false);
    g_menuItems.push_back(L"");   g_menuIsSeparator.push_back(true);
    g_menuItems.push_back(T(22)); g_menuIsSeparator.push_back(false);
    g_menuItems.push_back(T(23)); g_menuIsSeparator.push_back(false);
    g_menuItems.push_back(T(24)); g_menuIsSeparator.push_back(false);
    g_menuItems.push_back(T(25)); g_menuIsSeparator.push_back(false);

    if (!EnsureMenuClass()) return;

    int itemH = MenuItemHeight();
    int padY = MenuPadY();
    int w = MenuWidth();
    int hgt = padY * 2 + (int)g_menuItems.size() * itemH;

    int sw = GetSystemMetrics(SM_CXSCREEN);
    int sh = GetSystemMetrics(SM_CYSCREEN);
    if (x + w > sw) x = sw - w;
    if (y + hgt > sh) y = sh - hgt;
    if (x < 0) x = 0;
    if (y < 0) y = 0;

    g_inMenu = true;
    g_menuHover = -1;
    g_menuResult = 0;

    HWND menu = CreateWindowExW(
        WS_EX_TOOLWINDOW | WS_EX_TOPMOST,
        kMenuClass, L"", WS_POPUP,
        x, y, w, hgt,
        h, NULL, (HINSTANCE)&__ImageBase, NULL);

    if (!menu) {
        Wh_Log(L"[Menu] CreateWindowExW failed, err=%u", (unsigned)GetLastError());
        g_inMenu = false;
        return;
    }

    g_menuWnd = menu;
    ShowWindow(menu, SW_SHOW);
    SetForegroundWindow(menu);
    SetFocus(menu);

    MSG msg;
    while (g_menuWnd && GetMessageW(&msg, NULL, 0, 0)) {
        if (msg.message == WM_QUIT) {
            PostQuitMessage((int)msg.wParam);
            break;
        }
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    g_inMenu = false;
    int cmd = g_menuResult;
    g_menuResult = 0;

    if (cmd >= 1 && cmd <= (int)g_menuItems.size()) {
        // g_menuResult is the selected vector index plus one; keep that index
        // intact so separators cannot shift the command selected by the user.
        int vecIdx = cmd - 1;
        if (g_menuIsSeparator[vecIdx]) vecIdx = -1;

        if (vecIdx == 0) {
            // Use the same documented Shell "open" verb as keyboard/left-click
            // activation. This lets the Shell select the user's associated app.
            LaunchPath(path);
            ClosePane();
        } else if (vecIdx == 1) {
            wchar_t args[MAX_PATH + 16];
            StringCchPrintfW(args, MAX_PATH + 16, L"/select,\"%s\"", path.c_str());
            ShellExecuteW(NULL, L"open", L"explorer.exe", args, NULL, SW_SHOWNORMAL);
        } else if (vecIdx == 2) {
            SHELLEXECUTEINFOW sei = { sizeof(sei) };
            sei.fMask = SEE_MASK_NOCLOSEPROCESS;
            sei.lpVerb = L"runas";
            sei.lpFile = path.c_str();
            sei.nShow = SW_SHOWNORMAL;
            ShellExecuteExW(&sei);
        } else if (vecIdx == 3) {
            if (OpenClipboard(h)) {
                EmptyClipboard();
                size_t len = (path.size() + 1) * sizeof(wchar_t);
                HGLOBAL g = GlobalAlloc(GMEM_MOVEABLE, len);
                if (g) {
                    void* locked = GlobalLock(g);
                    if (locked) {
                        memcpy(locked, path.c_str(), len);
                        GlobalUnlock(g);
                        if (!SetClipboardData(CF_UNICODETEXT, g)) GlobalFree(g);
                    } else {
                        GlobalFree(g);
                    }
                }
                CloseClipboard();
            }
        } else if (vecIdx == 5) {
            g_sortMode = SORT_NAME; Refilter(); InvalidateRect(h, NULL, FALSE);
        } else if (vecIdx == 6) {
            g_sortMode = SORT_DATE; Refilter(); InvalidateRect(h, NULL, FALSE);
        } else if (vecIdx == 7) {
            g_sortMode = SORT_MOST_USED; Refilter(); InvalidateRect(h, NULL, FALSE);
        } else if (vecIdx == 8) {
            g_sortMode = SORT_CATEGORY; Refilter(); InvalidateRect(h, NULL, FALSE);
        }
    }

    if (g_wnd) {
        SetForegroundWindow(g_wnd);
        SetFocus(g_wnd);
    }
}

// ---------------------------------------------------------------- dismiss/close
static void DismissContextMenu() {
    if (g_menuWnd) {
        PostMessageW(g_menuWnd, WM_CLOSE, 0, 0);
    }
}

static void ClosePane() {
    if (!g_wnd || !g_open) return;
    DismissContextMenu();
    StartAnim(false);
}

// ---------------------------------------------------------------- topmost enforcement
static int  g_raiseCount = 0;
static DWORD g_raiseSince = 0;
static bool g_gaveUp = false;

static void EnforceTopmost() {
    if (!g_wnd || !g_aboveTaskbar) return;

    if (g_gaveUp) {
        SetWindowPos(g_wnd, HWND_TOPMOST, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOREDRAW);
        return;
    }

    RECT r; GetWindowRect(g_wnd, &r);
    POINT c = { (r.left + r.right) / 2, (r.top + r.bottom) / 2 };
    bool covered = (WindowFromPoint(c) != g_wnd);

    if (!covered) {
        SetWindowPos(g_wnd, HWND_TOPMOST, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOREDRAW);
        g_raiseCount = 0;
        g_raiseSince = 0;
        return;
    }

    DWORD now = GetTickCount();
    if (now - g_raiseSince > 1000) { g_raiseSince = now; g_raiseCount = 0; }
    if (++g_raiseCount > 5) {
        g_gaveUp = true;
        Wh_Log(L"[ZBand] panel keeps getting covered, giving up re-raising (plain topmost only)");
        DebugDump(L"gave up");
        return;
    }
    SetWindowPos(g_wnd, HWND_TOPMOST, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOREDRAW);
}

struct MonCtx { int target; int cur; HMONITOR result; };
static BOOL CALLBACK MonEnumProc(HMONITOR hm, HDC, LPRECT, LPARAM lp) {
    auto* c = (MonCtx*)lp;
    if (c->cur == c->target) { c->result = hm; return FALSE; }
    c->cur++;
    return TRUE;
}

static HMONITOR PickMonitor() {
    if (g_monitorChoice <= 0) {
        POINT pt; GetCursorPos(&pt);
        return MonitorFromPoint(pt, MONITOR_DEFAULTTOPRIMARY);
    }
    MonCtx c{ g_monitorChoice - 1, 0, nullptr };
    EnumDisplayMonitors(nullptr, nullptr, MonEnumProc, (LPARAM)&c);
    if (c.result) return c.result;
    POINT pt = {0, 0};
    return MonitorFromPoint(pt, MONITOR_DEFAULTTOPRIMARY);
}

static void OpenPane() {
    if (!g_wnd) return;
    if (g_state == ST_IDLE && !IsWindowVisible(g_wnd)) {
        HMONITOR target = PickMonitor();
        MONITORINFO mi = { sizeof(mi) };
        GetMonitorInfoW(target, &mi);
        RECT r = mi.rcMonitor;

        UINT dpiX = 96, dpiY = 96;
        if (FAILED(GetDpiForMonitor(target, MDT_EFFECTIVE_DPI, &dpiX, &dpiY))) {
            dpiX = dpiY = 96;
        }
        g_scale = dpiX / 96.0;

        g_w = S(346);
        g_h = r.bottom - r.top;

        // Invalidate the Layout cache since the panel dimensions changed.
        g_cachedLayW = -1;

        SetWindowPos(g_wnd, HWND_TOPMOST, r.right - g_w, r.top, g_w, g_h,
                     SWP_NOACTIVATE | SWP_NOREDRAW);

        g_query.clear(); g_caret = true; g_selectAll = false; g_inMenu = false;
        g_dropdownOpen = false; g_scope = SCOPE_ALL;
        g_raiseCount = 0; g_raiseSince = 0; g_gaveUp = false;
        g_scrollDragging = false;
        Refilter();
        if (!g_scanning && GetTickCount() - g_lastScan > 5 * 60 * 1000) StartScan();
        g_paneOff = g_w; g_contOff = S(160);
        SetWindowPos(g_wnd, HWND_TOPMOST, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW | SWP_NOREDRAW);
        SetTimer(g_wnd, 2, 530, NULL);
        EnforceTopmost();
        if (g_debug) DebugDump(L"after OpenPane");
    }
    StartAnim(true);
    HWND fg = GetForegroundWindow();
    DWORD ft = fg ? GetWindowThreadProcessId(fg, NULL) : 0, me = GetCurrentThreadId();
    if (ft && ft != me) AttachThreadInput(me, ft, TRUE);
    AllowSetForegroundWindow(ASFW_ANY);
    SetForegroundWindow(g_wnd);
    SetFocus(g_wnd);
    if (ft && ft != me) AttachThreadInput(me, ft, FALSE);
}

// ---------------------------------------------------------------- path launch
static bool LaunchPath(const std::wstring& path) {
    Wh_Log(L"[Launch] attempting: %s", path.c_str());

    SHELLEXECUTEINFOW sei = {};
    sei.cbSize = sizeof(sei);
    sei.fMask = SEE_MASK_FLAG_NO_UI | SEE_MASK_NOASYNC | SEE_MASK_NOCLOSEPROCESS;
    sei.lpVerb = L"open";
    sei.lpFile = path.c_str();
    sei.nShow = SW_SHOWNORMAL;

    if (ShellExecuteExW(&sei)) {
        if (sei.hProcess) CloseHandle(sei.hProcess);
        Wh_Log(L"[Launch] ShellExecuteExW succeeded");
        return true;
    }

    DWORD err = GetLastError();
    Wh_Log(L"[Launch] ShellExecuteExW failed (err=%lu); trying ShellExecuteW", err);

    HINSTANCE result = ShellExecuteW(NULL, L"open", path.c_str(), NULL, NULL, SW_SHOWNORMAL);
    INT_PTR code = (INT_PTR)result;
    if (code > 32) {
        Wh_Log(L"[Launch] ShellExecuteW succeeded");
        return true;
    }

    Wh_Log(L"[Launch] ShellExecuteW failed (code=%lld)", (long long)code);
    return false;
}

static void LaunchResult(int r) {
    if (r < 0 || r >= (int)g_res.size()) return;
    std::wstring path;
    {
        SrwGuard g(&g_lock, false);
        int idx = g_res[r];
        if (idx >= 0 && idx < (int)g_items.size()) path = g_items[idx].path;
    }
    if (path.empty()) return;
    LaunchPath(path);
    if (!g_query.empty()) {
        g_recent.push_back(g_query);
        if (g_recent.size() > 8) g_recent.erase(g_recent.begin());
    }
    ClosePane();
}

// ---------------------------------------------------------------- UI helpers
static void EnsureVisible() {
    Lay L = Layout();
    if (g_sel < g_scroll) g_scroll = g_sel;
    if (g_sel >= g_scroll + L.vis) g_scroll = g_sel - L.vis + 1;
    if (g_scroll < 0) g_scroll = 0;
}

static int RowAt(int y) {
    Lay L = Layout();
    if (y < L.listY) return -1;
    int r = g_scroll + (y - L.listY) / L.rowH;
    return (r >= 0 && r < (int)g_res.size() && r < g_scroll + L.vis) ? r : -1;
}

static bool InEditBox(int x, int y) {
    Lay L = Layout();
    return (y >= L.editY && y < L.editY + L.editH && x >= L.mx && x < L.editR);
}

static bool InDropdownArea(int x, int y) {
    Lay L = Layout();
    return (y >= L.lblY - S(4) && y < L.editY - S(2) && x >= L.mx && x < L.editR);
}

static int DropdownItemAt(int x, int y, int* outIndex) {
    Lay L = Layout();
    if (!g_dropdownOpen) return -1;
    int ddY = L.editY + L.editH + S(2);
    int itemH = S(28);
    int items = 4;
    for (int i = 0; i < items; i++) {
        RECT r = { L.mx, ddY + i * itemH, L.editR, ddY + (i + 1) * itemH };
        if (x >= r.left && x < r.right && y >= r.top && y < r.bottom) {
            *outIndex = i;
            return i;
        }
    }
    return -1;
}

static void Changed() { g_selectAll = false; Refilter(); InvalidateRect(g_wnd, NULL, FALSE); }

static std::wstring ClipText(HWND h) {
    std::wstring out;
    if (!OpenClipboard(h)) return out;
    HANDLE d = GetClipboardData(CF_UNICODETEXT);
    if (d) {
        const wchar_t* s = (const wchar_t*)GlobalLock(d);
        if (s) { for (; *s; ++s) if (*s != L'\r' && *s != L'\n' && *s != L'\t') out += *s; }
        GlobalUnlock(d);
    }
    CloseClipboard();
    return out;
}

// ---------------------------------------------------------------- window procedure
static LRESULT CALLBACK WndProc(HWND h, UINT msg, WPARAM wp, LPARAM lp) {
    try {
        switch (msg) {
        case WM_PAINT: DoPaint(h); return 0;
        case WM_ERASEBKGND: return 1;
        case WM_CANCELMODE:
            return 0;
        case WM_TIMER:
            if (wp == 1) Tick();
            else if (wp == 2) { g_caret = !g_caret; if (IsWindowVisible(h)) InvalidateRect(h, NULL, FALSE); }
            else if (wp == 3) {
                if (g_open && !g_inMenu) EnforceTopmost();
            }
            return 0;
        case WM_APP_TOGGLE:
            if (g_open) { DismissContextMenu(); ClosePane(); }
            else OpenPane();
            return 0;
        case WM_APP_INDEX:
            Refilter(); InvalidateRect(h, NULL, FALSE);
            return 0;
        case WM_ACTIVATE:
            if (LOWORD(wp) == WA_INACTIVE && g_open && g_closeOnClickOutside && !g_inMenu)
                ClosePane();
            return 0;
        case WM_KILLFOCUS:
            if (g_open && g_closeOnClickOutside && g_state == ST_IDLE && !g_inMenu)
                ClosePane();
            return 0;
        case WM_MOUSEACTIVATE: return MA_ACTIVATE;
        case WM_SETCURSOR: {
            if (LOWORD(lp) == HTCLIENT) {
                POINT pt; GetCursorPos(&pt); ScreenToClient(h, &pt);
                SetCursor(LoadCursorW(NULL, InEditBox(pt.x, pt.y) ? IDC_IBEAM : IDC_ARROW));
                return TRUE;
            }
            break;
        }
        case WM_CHAR:
            if (wp >= 32) {
                if (g_selectAll) { g_query.clear(); g_selectAll = false; }
                if (wp >= 0xD800 && wp <= 0xDBFF) { g_pendingHigh = (wchar_t)wp; return 0; }
                if (wp >= 0xDC00 && wp <= 0xDFFF && g_pendingHigh) {
                    g_query += g_pendingHigh; g_query += (wchar_t)wp; g_pendingHigh = 0;
                    Changed(); return 0;
                }
                g_query += (wchar_t)wp; Changed();
            }
            return 0;
        case WM_KEYDOWN: {
            bool ctrl = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
            switch (wp) {
            case VK_ESCAPE:
                if (g_inMenu) { DismissContextMenu(); break; }
                ClosePane(); break;
            case VK_BACK:
                if (g_selectAll) { g_query.clear(); g_selectAll = false; Changed(); }
                else if (ctrl) {
                    while (!g_query.empty() && iswspace(g_query.back())) g_query.pop_back();
                    while (!g_query.empty() && !iswspace(g_query.back())) g_query.pop_back();
                    Changed();
                } else if (!g_query.empty()) { g_query.pop_back(); Changed(); }
                break;
            case 'A':
                if (ctrl && !g_query.empty()) { g_selectAll = true; InvalidateRect(h, NULL, FALSE); }
                break;
            case VK_RETURN: LaunchResult(g_sel); break;
            case VK_UP: if (g_sel > 0) { g_sel--; EnsureVisible(); InvalidateRect(h, NULL, FALSE); } break;
            case VK_DOWN: if (g_sel + 1 < (int)g_res.size()) { g_sel++; EnsureVisible(); InvalidateRect(h, NULL, FALSE); } break;
            case VK_HOME: g_sel = 0; EnsureVisible(); InvalidateRect(h, NULL, FALSE); break;
            case VK_END: g_sel = (int)g_res.size() - 1; EnsureVisible(); InvalidateRect(h, NULL, FALSE); break;
            case VK_PRIOR: {
                Lay L = Layout();
                g_sel = std::max(0, g_sel - L.vis);
                EnsureVisible(); InvalidateRect(h, NULL, FALSE);
                break;
            }
            case VK_NEXT: {
                Lay L = Layout();
                g_sel = std::min((int)g_res.size() - 1, g_sel + L.vis);
                EnsureVisible(); InvalidateRect(h, NULL, FALSE);
                break;
            }
            case 'V': if (ctrl) { g_query += ClipText(h); Changed(); } break;
            }
            return 0;
        }
        case WM_MOUSEWHEEL: {
            POINT pt = { (short)LOWORD(lp), (short)HIWORD(lp) };
            ScreenToClient(h, &pt);
            Lay L = Layout();
            if (pt.y >= L.listY) {
                int d = GET_WHEEL_DELTA_WPARAM(wp) > 0 ? -1 : 1;
                int mx = std::max(0, (int)g_res.size() - L.vis);
                g_scroll = std::min(mx, std::max(0, g_scroll + d));
                InvalidateRect(h, NULL, FALSE);
            }
            return 0;
        }
        case WM_MOUSEMOVE: {
            if (g_scrollDragging) {
                RECT track = ScrollTrackRect();
                RECT thumb = ScrollThumbRect();
                int trackH = track.bottom - track.top;
                int thumbH = thumb.bottom - thumb.top;
                int total = (int)g_res.size();
                int vis = Layout().vis;
                int maxScroll = std::max(0, total - vis);
                int travel = std::max(1, trackH - thumbH);
                int y = (short)HIWORD(lp);
                int delta = y - g_scrollDragY;
                int newScroll = g_scrollDragScroll + delta * maxScroll / travel;
                if (newScroll < 0) newScroll = 0;
                if (newScroll > maxScroll) newScroll = maxScroll;
                if (newScroll != g_scroll) { g_scroll = newScroll; InvalidateRect(h, NULL, FALSE); }
                return 0;
            }

            if (g_state != ST_IDLE) return 0;
            TRACKMOUSEEVENT tme = { sizeof(TME_LEAVE), TME_LEAVE, h, 0 };
            TrackMouseEvent(&tme);
            int x = (short)LOWORD(lp), y = (short)HIWORD(lp);
            if (InScrollbarRegion(x, y)) {
                if (g_hover != -2) { g_hover = -2; InvalidateRect(h, NULL, FALSE); }
                return 0;
            }
            if (g_dropdownOpen) {
                int idx = -1;
                DropdownItemAt(x, y, &idx);
                if (idx != g_dropdownHover) { g_dropdownHover = idx; InvalidateRect(h, NULL, FALSE); }
            } else {
                int r = RowAt(y);
                if (r != g_hover) { g_hover = r; InvalidateRect(h, NULL, FALSE); }
            }
            return 0;
        }
        case WM_MOUSELEAVE:
            if (g_hover != -1 || g_dropdownHover != -1) {
                g_hover = -1; g_dropdownHover = -1;
                InvalidateRect(h, NULL, FALSE);
            }
            return 0;
        case WM_LBUTTONUP: {
            if (g_scrollDragging) {
                g_scrollDragging = false;
                ReleaseCapture();
                InvalidateRect(h, NULL, FALSE);
                return 0;
            }
            return 0;
        }
        case WM_LBUTTONDOWN: {
            if (g_state != ST_IDLE || !g_open) return 0;
            DWORD now = GetTickCount();
            if (now - g_lastClick < (DWORD)g_clickCooldownMs) return 0;
            g_lastClick = now;
            int x = (short)LOWORD(lp), y = (short)HIWORD(lp);
            Lay L = Layout();

            if (InScrollbarRegion(x, y)) {
                RECT thumb = ScrollThumbRect();
                int total = (int)g_res.size();
                int vis = L.vis;
                int maxScroll = std::max(0, total - vis);
                if (thumb.bottom > thumb.top &&
                    x >= thumb.left - S(7) && x < thumb.right &&
                    y >= thumb.top && y < thumb.bottom) {
                    g_scrollDragging = true;
                    g_scrollDragY = y;
                    g_scrollDragScroll = g_scroll;
                    SetCapture(h);
                } else if (y < thumb.top) {
                    g_scroll = std::max(0, g_scroll - vis);
                    InvalidateRect(h, NULL, FALSE);
                } else if (y >= thumb.bottom) {
                    g_scroll = std::min(maxScroll, g_scroll + vis);
                    InvalidateRect(h, NULL, FALSE);
                }
                return 0;
            }

            if (g_dropdownOpen) {
                int idx = -1;
                if (DropdownItemAt(x, y, &idx) >= 0) {
                    g_scope = idx;
                    g_dropdownOpen = false;
                    g_dropdownHover = -1;
                    Refilter();
                    InvalidateRect(h, NULL, FALSE);
                    return 0;
                }
                g_dropdownOpen = false;
                g_dropdownHover = -1;
                InvalidateRect(h, NULL, FALSE);
                return 0;
            }

            if (InDropdownArea(x, y)) {
                g_dropdownOpen = true;
                g_dropdownHover = -1;
                InvalidateRect(h, NULL, FALSE);
                return 0;
            }

            if (!g_query.empty() && x >= L.editR - L.btnW - S(24) && x < L.editR - L.btnW &&
                y >= L.editY && y < L.editY + L.editH) {
                g_query.clear();
                g_selectAll = false;
                Changed();
                return 0;
            }

            if (InEditBox(x, y)) {
                g_selectAll = false;
                InvalidateRect(h, NULL, FALSE);
                if (x >= L.editR - L.btnW) LaunchResult(g_sel);
            }
            else if (RowAt(y) >= 0) LaunchResult(RowAt(y));
            return 0;
        }
        case WM_RBUTTONUP: {
            if (g_state != ST_IDLE || !g_open) return 0;
            int x = (short)LOWORD(lp);
            int y = (short)HIWORD(lp);
            if (InScrollbarRegion(x, y)) return 0;
            int r = RowAt(y);
            if (r >= 0) { POINT pt; GetCursorPos(&pt); ShowContextMenu(h, pt.x, pt.y, r); }
            return 0;
        }
        case WM_DPICHANGED: {
            g_scale = HIWORD(wp) / 96.0;
            g_fontScale = 0;
            g_cachedLayW = -1;   // invalidate Layout cache
            RECT* r = (RECT*)lp;
            SetWindowPos(h, NULL, r->left, r->top, r->right - r->left, r->bottom - r->top,
                         SWP_NOZORDER | SWP_NOACTIVATE);
            InvalidateRect(h, NULL, FALSE);
            return 0;
        }
        }
    } catch (...) {
        Wh_Log(L"[Wnd] exception caught in WndProc");
    }
    return DefWindowProcW(h, msg, wp, lp);
}

// ---------------------------------------------------------------- keyboard hook
static void MaskWinKey() {
    INPUT in[2] = {};
    in[0].type = INPUT_KEYBOARD; in[0].ki.wVk = 0xE8; in[0].ki.dwExtraInfo = kMagic;
    in[1].type = INPUT_KEYBOARD; in[1].ki.wVk = 0xE8; in[1].ki.dwExtraInfo = kMagic;
    in[1].ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(ARRAYSIZE(in), in, sizeof(INPUT));
}

static LRESULT CALLBACK KbProc(int code, WPARAM wp, LPARAM lp) {
    try {
        if (code == HC_ACTION) {
            const KBDLLHOOKSTRUCT* k = (const KBDLLHOOKSTRUCT*)lp;
            if (k->dwExtraInfo != kMagic) {
                bool down = (wp == WM_KEYDOWN || wp == WM_SYSKEYDOWN);
                bool up = (wp == WM_KEYUP || wp == WM_SYSKEYUP);
                if (up && g_swallowVk && k->vkCode == g_swallowVk) {
                    g_swallowVk = 0;
                    return 1;
                }
                if (down && g_swallowVk && k->vkCode == g_swallowVk) return 1;

                bool win = ((GetAsyncKeyState(VK_LWIN) | GetAsyncKeyState(VK_RWIN)) & 0x8000) != 0;
                bool other = (GetAsyncKeyState(VK_SHIFT) | GetAsyncKeyState(VK_CONTROL) |
                              GetAsyncKeyState(VK_MENU)) & 0x8000;
                if (down && win && !other &&
                    ((k->vkCode == 'S' && g_useWinS) || (k->vkCode == 'Q' && g_useWinQ))) {
                    DWORD now = GetTickCount();
                    if (now - g_lastHotkey < (DWORD)g_hotkeyCooldownMs) return 1;

                    g_lastHotkey = now;
                    g_swallowVk = k->vkCode;
                    MaskWinKey();

                    // The panel belongs to the UI thread. Posting is deliberately the
                    // only work performed here after recognizing the shortcut.
                    HWND panel = g_wnd;
                    if (panel) PostMessageW(panel, WM_APP_TOGGLE, 0, 0);
                    return 1;
                }
            }
        }
    } catch (...) {
        Wh_Log(L"[Hook] exception in KbProc");
    }
    return CallNextHookEx(g_hook, code, wp, lp);
}

static void SignalUiReady(bool started) {
    InterlockedExchange(&g_uiStarted, started ? 1 : 0);
    if (g_uiReady.get()) SetEvent(g_uiReady.get());
}

static void SignalKeyboardReady(bool started) {
    InterlockedExchange(&g_keyboardStarted, started ? 1 : 0);
    if (g_keyboardReady.get()) SetEvent(g_keyboardReady.get());
}

// This thread owns only the low-level keyboard hook and an otherwise empty
// GetMessage loop. It never performs painting, scanning, shell execution, or
// clipboard work, so Windows can keep delivering low-level callbacks promptly.
static DWORD WINAPI KeyboardHookThread(LPVOID) {
    MSG queued = {};
    PeekMessageW(&queued, NULL, WM_USER, WM_USER, PM_NOREMOVE); // create the queue first

    g_hook = SetWindowsHookExW(WH_KEYBOARD_LL, KbProc, (HINSTANCE)&__ImageBase, 0);
    if (!g_hook) {
        Wh_Log(L"[Hook] SetWindowsHookExW(WH_KEYBOARD_LL) failed, err=%u",
               (unsigned)GetLastError());
        SignalKeyboardReady(false);
        return 0;
    }

    Wh_Log(L"[Hook] keyboard hook installed: %p", g_hook);
    SignalKeyboardReady(true);

    MSG msg;
    int result = 0;
    while ((result = GetMessageW(&msg, NULL, 0, 0)) > 0) {
        // No window dispatch is needed on this thread. Retrieving messages is
        // enough to service WH_KEYBOARD_LL callbacks and keeps this loop minimal.
    }
    if (result == -1)
        Wh_Log(L"[Hook] GetMessageW failed, err=%u", (unsigned)GetLastError());

    if (g_hook) {
        UnhookWindowsHookEx(g_hook);
        g_hook = NULL;
    }
    return 0;
}

class ScopedWindowClass {
    HINSTANCE instance_;
    const wchar_t* name_;
    bool registered_ = false;
public:
    ScopedWindowClass(HINSTANCE instance, const wchar_t* name)
        : instance_(instance), name_(name) {}
    ~ScopedWindowClass() {
        if (registered_) UnregisterClassW(name_, instance_);
    }
    bool Register(const WNDCLASSW& wc) {
        registered_ = RegisterClassW(&wc) != 0;
        return registered_;
    }
    ScopedWindowClass(const ScopedWindowClass&) = delete;
    ScopedWindowClass& operator=(const ScopedWindowClass&) = delete;
};

// ---------------------------------------------------------------- UI thread
static DWORD WINAPI UiThread(LPVOID) {
    // A queue exists before initialization can fail. This makes WM_QUIT reliable
    // during an early unload and lets Wh_ModInit wait for the panel startup state.
    MSG queued = {};
    PeekMessageW(&queued, NULL, WM_USER, WM_USER, PM_NOREMOVE);

    HINSTANCE hi = (HINSTANCE)&__ImageBase;
    ScopedWindowClass panelClass(hi, kClass);
    bool readySignaled = false;

    try {
        SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
        ScopedComApartment com(SUCCEEDED(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED)));

        HMODULE user32 = GetModuleHandleW(L"user32.dll");
        if (user32)
            g_createInBand = (CreateWindowInBand_t)(void*)GetProcAddress(user32, "CreateWindowInBand");
        if (!g_createInBand)
            Wh_Log(L"[ZBand] CreateWindowInBand not available, using ordinary topmost windows");

        WNDCLASSW wc = {};
        wc.lpfnWndProc = WndProc;
        wc.hInstance = hi;
        wc.hCursor = LoadCursorW(NULL, IDC_ARROW);
        wc.lpszClassName = kClass;
        if (!panelClass.Register(wc)) {
            Wh_Log(L"[UI] RegisterClassW failed, err=%u", (unsigned)GetLastError());
            SignalUiReady(false);
            readySignaled = true;
            return 0;
        }

        g_wnd = CreateOverlayWindow(kClass, 346, 600);
        if (!g_wnd) {
            Wh_Log(L"[UI] could not create panel window, err=%u", (unsigned)GetLastError());
            SignalUiReady(false);
            readySignaled = true;
            return 0;
        }
        SetLayeredWindowAttributes(g_wnd, KEY, 255, LWA_COLORKEY);

        // Wh_ModInit waits for this signal before it enables the keyboard hook
        // or returns. The window and message queue are both ready at this point.
        SignalUiReady(true);
        readySignaled = true;

        SetTimer(g_wnd, 3, 2000, NULL);
        StartScan();

        MSG msg;
        int result = 0;
        while ((result = GetMessageW(&msg, NULL, 0, 0)) > 0) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
        if (result == -1)
            Wh_Log(L"[UI] GetMessageW failed, err=%u", (unsigned)GetLastError());
    } catch (...) {
        Wh_Log(L"[UI] exception caught in UI thread");
    }

    if (!readySignaled) SignalUiReady(false);

    if (g_menuWnd) {
        DestroyWindow(g_menuWnd);
        g_menuWnd = NULL;
    }
    g_inMenu = false;
    if (g_wnd) {
        KillTimer(g_wnd, 3);
        HWND panel = g_wnd;
        g_wnd = NULL;
        DestroyWindow(panel);
    }
    for (auto& kv : g_icons) if (kv.second) DestroyIcon(kv.second);
    g_icons.clear();
    HFONT* f[] = { &g_fTitle, &g_fLabel, &g_fText, &g_fName, &g_fSub, &g_fGlyph, &g_fHeader };
    for (auto p : f) if (*p) { DeleteObject(*p); *p = NULL; }
    UnregisterMenuClass();
    return 0;
}

// The shell process owns GetShellWindow. If it is in the middle of restarting,
// exclude the known folder-window command lines while waiting for its taskbar.
static bool IsMainExplorerProcess() {
    HWND shellWindow = GetShellWindow();
    if (shellWindow) {
        DWORD shellProcessId = 0;
        GetWindowThreadProcessId(shellWindow, &shellProcessId);
        if (shellProcessId == GetCurrentProcessId()) return true;
    }

    std::wstring commandLine = GetCommandLineW();
    std::transform(commandLine.begin(), commandLine.end(), commandLine.begin(),
                   [](wchar_t ch) { return (wchar_t)towlower(ch); });
    return commandLine.find(L" /factory") == std::wstring::npos &&
           commandLine.find(L" /separate") == std::wstring::npos &&
           commandLine.find(L" -embedding") == std::wstring::npos;
}

static bool WaitForStartup(HANDLE readyEvent, const wchar_t* component) {
    const DWORD wait = WaitForSingleObject(readyEvent, 10000);
    if (wait == WAIT_OBJECT_0) return true;
    Wh_Log(L"[Init] timed out waiting for %s (result=%u)", component, (unsigned)wait);
    return false;
}

// Retry WM_QUIT while a thread is alive. A normal startup creates the message
// queue before signaling readiness; retrying also covers an unload racing an
// unsuccessful startup instead of waiting forever after one failed post.
static void QuitAndJoinThread(WinHandle& thread, DWORD& threadId, const wchar_t* name) {
    HANDLE handle = thread.get();
    if (!handle) return;

    while (WaitForSingleObject(handle, 0) == WAIT_TIMEOUT) {
        if (threadId && PostThreadMessageW(threadId, WM_QUIT, 0, 0)) break;
        Sleep(10);
    }

    if (WaitForSingleObject(handle, 0) == WAIT_TIMEOUT) {
        Wh_Log(L"[Uninit] waiting for %s", name);
        WaitForSingleObject(handle, INFINITE);
    }
    thread.reset();
    threadId = 0;
}

static void StopInfrastructure() {
    InterlockedExchange(&g_stop, 1);

    // Stop the producer first so it cannot post work to a panel that is going away.
    QuitAndJoinThread(g_keyboardThread, g_keyboardTid, L"keyboard-hook thread");
    QuitAndJoinThread(g_uiThread, g_uiTid, L"UI thread");

    if (g_scanThread.get()) {
        WaitForSingleObject(g_scanThread.get(), INFINITE);
        g_scanThread.reset();
    }

    g_uiReady.reset();
    g_keyboardReady.reset();
    InterlockedExchange(&g_uiStarted, 0);
    InterlockedExchange(&g_keyboardStarted, 0);
}

void Wh_ModSettingsChanged() {
    RunGuarded(L"applying settings", [] { LoadSettings(); });
}

BOOL Wh_ModInit() {
    if (!IsMainExplorerProcess()) {
        Wh_Log(L"[Init] not the main Explorer shell process; skipping");
        return FALSE;
    }

    try {
        LARGE_INTEGER f;
        QueryPerformanceFrequency(&f);
        g_freq = f.QuadPart ? f.QuadPart : 1;
        InterlockedExchange(&g_stop, 0);

        LoadSettings();

        g_uiReady.reset(CreateEventW(NULL, TRUE, FALSE, NULL));
        g_keyboardReady.reset(CreateEventW(NULL, TRUE, FALSE, NULL));
        if (!g_uiReady.get() || !g_keyboardReady.get()) {
            Wh_Log(L"[Init] CreateEventW failed, err=%u", (unsigned)GetLastError());
            StopInfrastructure();
            return FALSE;
        }

        HANDLE rawUi = CreateThread(NULL, 0, UiThread, NULL, 0, &g_uiTid);
        g_uiThread.reset(rawUi);
        if (!g_uiThread.get() || !WaitForStartup(g_uiReady.get(), L"the UI thread") ||
            InterlockedCompareExchange(&g_uiStarted, 0, 0) == 0) {
            Wh_Log(L"[Init] UI thread did not create the panel");
            StopInfrastructure();
            return FALSE;
        }

        HANDLE rawKeyboard = CreateThread(NULL, 0, KeyboardHookThread, NULL, 0, &g_keyboardTid);
        g_keyboardThread.reset(rawKeyboard);
        if (!g_keyboardThread.get() ||
            !WaitForStartup(g_keyboardReady.get(), L"the keyboard-hook thread") ||
            InterlockedCompareExchange(&g_keyboardStarted, 0, 0) == 0) {
            Wh_Log(L"[Init] keyboard hook was not installed");
            StopInfrastructure();
            return FALSE;
        }

        Wh_Log(L"[Init] initialized in the main Explorer shell process");
        return TRUE;
    } catch (...) {
        g_exceptionCount.fetch_add(1);
        Wh_Log(L"[Init] exception while initializing the mod");
        StopInfrastructure();
        return FALSE;
    }
}

void Wh_ModUninit() {
    Wh_Log(L"[Uninit] shutting down, exceptions=%u", g_exceptionCount.load());
    StopInfrastructure();
}
