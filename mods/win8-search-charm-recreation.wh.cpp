// ==WindhawkMod==
// @id              win8-search-charm-recreation
// @name            Windows 8/8.1 Search Charm Recreation
// @description     This mod recreates the Windows 8/8.1 Search Charm on Windows 10 and Windows 11
// @version         1.0.0
// @author          babamohammed
// @github          https://github.com/babamohammed2022
// @license         GPL-3.0
// @include         explorer.exe
// @compilerOptions -lgdi32 -luser32 -lshell32 -lole32 -lshlwapi -ldwmapi -luuid -loleaut32 -lcomctl32 -lshcore -ladvapi32 -fexceptions
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Windows 8/8.1 Search Charm

This mod recreates the Windows 8/8.1 search panel with local-only results, inspired by
the Metro UI design language.

## Features

- **Search panel** on the right side of the screen, with Windows 8.1 green color scheme.
- **Local search** across:
  - **Classic Win32 apps** from Start menu `.lnk` shortcuts
  - **Modern UWP/Store apps** via `FOLDERID_AppsFolder` (the virtual shell namespace
    that lists every installed app, including those without a physical `.lnk` file)
  - **User files** from Desktop, Documents, Downloads, Pictures, Music, Videos
  - **System entries** (Control Panel, Settings, Command Prompt, Task Manager, etc.)
- **All/Apps/Settings/Files** filter, mimicking the Windows 8.1 search scope selector.
- **Keyboard shortcuts**: `Win+S` or `Win+Q` to open, `Ctrl+A` to select all,
  `Ctrl+V` to paste, arrow keys to navigate.
- **Click outside** to close, `Esc` to dismiss.
- **Per-monitor DPI** aware, with IME support for CJK input.
- **Context menu** (right-click) with Open file location, Run as administrator, Copy path.
  The context menu stays open until the search panel is closed or an entry is launched;
  it is not dismissed on a timer.
- **Native search suppression (virtualized)**: the native Windows Search service
  (`WSearch`) is suppressed by *virtualizing* the registry. The value
  `HKLM\SYSTEM\CurrentControlSet\Services\WSearch\Start` is reported as `4`
  (disabled) to any process that queries it, **without ever writing to the real
  registry**. This makes the suppression fully reversible and crash-safe: if the
  mod or Windhawk is terminated, nothing has to be undone.
- **Native search UI blocked**: `SearchHost.exe` / `SearchApp.exe` /
  `SearchUI.exe` launches are also blocked as a second line of defense.
- **Z-band overlay**: uses `CreateWindowInBand` (when available) so the panel stays
  above the taskbar, Start menu, and fullscreen UWP apps.
- **UI translations**: English, Italian, Spanish, French, Portuguese, German,
  Russian, Chinese (Simplified), Japanese, Korean.

## Native search suppression (virtualized registry)

Instead of writing `Start=4` into the real registry, this mod hooks the registry
APIs (`RegOpenKeyExW/A`, `RegQueryValueExW/A`, `RegSetValueExW/A`, `RegEnumValueW/A`,
`RegCreateKeyExW/A`, `RegCloseKey`) and, whenever a caller opens or reads the
`WSearch` service key, returns a **virtual handle** that reports `Start=4`.

The real registry is never modified. When the mod is unloaded, the virtual
handles are closed and the real keys are released. Nothing has to be restored.

- If the service is already running when the mod loads, the mod asks the SCM to
  stop it (this is a runtime action, not a persistent registry change; the
  service can be started again normally).
- Writing `Start` through a virtual handle is a no-op (silently succeeds), so
  other components cannot "fix" the value while the mod is active.

## Credits

- **m417z** (https://github.com/m417z) — for the inspiration and the low-level
  hooking techniques (`CreateProcessInternalW`) used in the "Search Menu Inspect
  Helper" mod, which made reliable native search suppression possible.
- **AdministratoX** — for the Metro UI interface template that inspired the
  visual layout and design language of this search panel.
- The **z-band overlay** technique (`CreateWindowInBand`, `ZBID_UIACCESS`) and the
  z-order diagnostic helpers are adapted from the "Windows 8 (8102) Charms" mod.

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- useWinS: true
  $name: Open with Win+S
  $description: This setting allows the mod to intercept Win+S and open this panel instead of Windows native search.
- useWinQ: true
  $name: Open with Win+Q
  $description: This setting allows the mod to ntercept Win+Q and open this panel.
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
  $description: This setting allows to close the search panel when clicking outside of it.
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
- blockNativeSearch: true
  $name: Suppress native search
  $description: This setting allows to block SearchHost.exe / SearchApp.exe launches when Windows search is requested, and virtualize the WSearch registry key so it reports Start=4 (disabled) without touching the real registry.
- stopWSearchService: true
  $name: Stop WSearch service if running
  $description: When the mod loads, this setting makes the mod ask the Service Control Manager to stop the WSearch service if it is currently running. This is a runtime action, not a persistent change.
- monitor: 0
  $name: Monitor
  $description: 0 = monitor under the cursor, 1 = first monitor, 2 = second monitor, and so on.
- debugLog: false
  $name: Debug log
  $description: This setting allows to write z-order diagnostics and registry-virtualization diagnostics to the Windhawk log (does not change how anything looks).
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

static const UINT WM_APP_TOGGLE = WM_APP + 1;
static const UINT WM_APP_INDEX  = WM_APP + 2;
static const wchar_t* kClass = L"Win81SearchCharmMod";
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
static WinHandle g_thread, g_scanThread;
static DWORD g_tid = 0;
static HHOOK g_hook = NULL, g_mouseHook = NULL;
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
static bool g_blockNativeSearch = true;
static bool g_stopWSearchService = true;
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

static int S(int v) { return (int)(v * g_scale + 0.5); }

// ---------------------------------------------------------------- forward declarations
// WndProc uses these helpers; declare them before WndProc so the compiler
// can see them regardless of the order in which the definitions appear.
static void EnforceTopmost();
static void OpenPane();
static void ClosePane();
static void DismissContextMenu();
static bool InEditBox(int x, int y);
static void Changed();
static void LaunchResult(int r);
static void EnsureVisible();
static std::wstring ClipText(HWND h);
static int RowAt(int y);
static int DropdownItemAt(int x, int y, int* outIndex);
static bool InDropdownArea(int x, int y);
static void ShowContextMenu(HWND h, int x, int y, int r);

// ---------------------------------------------------------------- z-band overlay (from 8102 Charms mod)
//
// CreateWindowInBand is an undocumented user32 export that lets a process create
// a window in a specific Z-band. ZBID_UIACCESS (=2) is the band used by Task
// Manager and the On-Screen Keyboard: it sits above the taskbar, the Start menu
// and fullscreen UWP apps. This is what makes the search panel reliably visible
// on top of everything, unlike a plain WS_EX_TOPMOST window.
//
// The band is selected at creation time; if the system refuses (older Windows
// builds, sandboxed processes, etc.) we fall back to an ordinary topmost window.
typedef HWND (WINAPI *CreateWindowInBand_t)(
    DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle,
    int X, int Y, int nWidth, int nHeight,
    HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam, DWORD dwBand);

static CreateWindowInBand_t g_createInBand = NULL;
static int g_band = -1;            // -1 = not tried yet, 0 = normal band, else the band that worked
static const DWORD kBandUIAccess = 2;     // ZBID_UIACCESS
static const DWORD kBandSystemTools = 16; // ZBID_SYSTEM_TOOLS

static HWND CreateOverlayWindow(const wchar_t* cls, int w, int h) {
    const DWORD ex = WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_LAYERED;
    HINSTANCE hi = (HINSTANCE)&__ImageBase;

    HWND hw = NULL;
    if (g_createInBand && g_useZBand && g_band != 0) {
        const DWORD bands[2] = { kBandUIAccess, kBandSystemTools };
        for (DWORD b : bands) {
            if (g_band > 0 && (DWORD)g_band != b) continue; // reuse the band that already worked
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
    g_blockNativeSearch = Wh_GetIntSetting(L"blockNativeSearch") != 0;
    g_stopWSearchService = Wh_GetIntSetting(L"stopWSearchService") != 0;
    g_monitorChoice = std::max(0, (int)Wh_GetIntSetting(L"monitor"));
    g_debug = Wh_GetIntSetting(L"debugLog") != 0;
    Wh_Log(L"[Settings] lang=%d monitor=%d colorMode=%d blockNative=%d stopSvc=%d zBand=%d debug=%d",
           g_lang, g_monitorChoice, g_colorMode, g_blockNativeSearch ? 1 : 0,
           g_stopWSearchService ? 1 : 0, g_useZBand ? 1 : 0, g_debug ? 1 : 0);
}

// Strings: 0=it,1=en,2=es,3=fr,4=pt,5=de,6=ru,7=zh,8=ja,9=ko
static const wchar_t* T(int id) {
    static const wchar_t* S[10][22] = {
        { L"Cerca", L"Ovunque", L"Nessun risultato", L"Applicazione", L"Indicizzazione...",
          L"Pannello di controllo", L"Impostazioni", L"Esplora file", L"Prompt dei comandi",
          L"Task Manager", L"Programmi e funzionalità", L"File", L"Impostazioni", L"Applicazioni",
          L"Cronologia", L"Cerca \"%s\" sul Web", L"Apri percorso file", L"Esegui come amministratore",
          L"Copia percorso", L"Ovunque", L"Applicazioni", L"Impostazioni" },
        { L"Search", L"Everywhere", L"No results", L"App", L"Indexing...",
          L"Control Panel", L"Settings", L"File Explorer", L"Command Prompt",
          L"Task Manager", L"Programs and Features", L"Files", L"Settings", L"Apps",
          L"History", L"Search \"%s\" on the Web", L"Open file location", L"Run as administrator",
          L"Copy path", L"Everywhere", L"Apps", L"Settings" },
        { L"Buscar", L"En todas partes", L"Sin resultados", L"Aplicación", L"Indexando...",
          L"Panel de control", L"Configuración", L"Explorador de archivos", L"Símbolo del sistema",
          L"Administrador de tareas", L"Programas y características", L"Archivos", L"Configuración",
          L"Aplicaciones", L"Historial", L"Buscar \"%s\" en la Web", L"Abrir ubicación del archivo",
          L"Ejecutar como administrador", L"Copiar ruta", L"En todas partes", L"Aplicaciones", L"Configuración" },
        { L"Rechercher", L"Partout", L"Aucun résultat", L"Application", L"Indexation...",
          L"Panneau de configuration", L"Paramètres", L"Explorateur de fichiers", L"Invite de commandes",
          L"Gestionnaire des tâches", L"Programmes et fonctionnalités", L"Fichiers", L"Paramètres",
          L"Applications", L"Historique", L"Rechercher \"%s\" sur le Web", L"Ouvrir l'emplacement du fichier",
          L"Exécuter en tant qu'administrateur", L"Copier le chemin", L"Partout", L"Applications", L"Paramètres" },
        { L"Pesquisar", L"Em todo o lado", L"Sem resultados", L"Aplicativo", L"Indexando...",
          L"Painel de Controle", L"Configurações", L"Explorador de Arquivos", L"Prompt de Comando",
          L"Gerenciador de Tarefas", L"Programas e Recursos", L"Arquivos", L"Configurações",
          L"Aplicativos", L"Histórico", L"Pesquisar \"%s\" na Web", L"Abrir local do arquivo",
          L"Executar como administrador", L"Copiar caminho", L"Em todo o lado", L"Aplicativos", L"Configurações" },
        { L"Suchen", L"Überall", L"Keine Ergebnisse", L"App", L"Indizierung...",
          L"Systemsteuerung", L"Einstellungen", L"Datei-Explorer", L"Eingabeaufforderung",
          L"Task-Manager", L"Programme und Features", L"Dateien", L"Einstellungen", L"Apps",
          L"Verlauf", L"\"%s\" im Web suchen", L"Dateispeicherort öffnen", L"Als Administrator ausführen",
          L"Pfad kopieren", L"Überall", L"Apps", L"Einstellungen" },
        { L"Поиск", L"Везде", L"Нет результатов", L"Приложение", L"Индексация...",
          L"Панель управления", L"Параметры", L"Проводник", L"Командная строка",
          L"Диспетчер задач", L"Программы и компоненты", L"Файлы", L"Параметры", L"Приложения",
          L"История", L"Искать \"%s\" в Интернете", L"Открыть расположение файла", L"Запуск от имени администратора",
          L"Копировать путь", L"Везде", L"Приложения", L"Параметры" },
        { L"搜索", L"随处", L"无结果", L"应用", L"正在索引...",
          L"控制面板", L"设置", L"文件资源管理器", L"命令提示符",
          L"任务管理器", L"程序和功能", L"文件", L"设置", L"应用",
          L"历史记录", L"在 Web 上搜索 \"%s\"", L"打开文件位置", L"以管理员身份运行",
          L"复制路径", L"随处", L"应用", L"设置" },
        { L"検索", L"すべての場所", L"結果なし", L"アプリ", L"インデックス作成中...",
          L"コントロール パネル", L"設定", L"ファイル エクスプローラー", L"コマンド プロンプト",
          L"タスク マネージャー", L"プログラムと機能", L"ファイル", L"設定", L"アプリ",
          L"履歴", L"Web で \"%s\" を検索", L"ファイルの場所を開く", L"管理者として実行",
          L"パスをコピー", L"すべての場所", L"アプリ", L"設定" },
        { L"검색", L"모든 위치", L"결과 없음", L"앱", L"인덱싱 중...",
          L"제어판", L"설정", L"파일 탐색기", L"명령 프롬프트",
          L"작업 관리자", L"프로그램 및 기능", L"파일", L"설정", L"앱",
          L"기록", L"웹에서 \"%s\" 검색", L"파일 위치 열기", L"관리자 권한으로 실행",
          L"경로 복사", L"모든 위치", L"앱", L"설정" }
    };
    int l = g_lang; if (l < 0 || l > 9) l = 1;
    if (id < 0 || id > 21) return L"";
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

// Enumerates FOLDERID_AppsFolder to add modern (UWP/Store) apps.
// Modern apps have no physical .lnk file: they only exist in the virtual
// shell namespace exposed by FOLDERID_AppsFolder. The documented way to
// enumerate them is to bind an IShellFolder to the folder and call
// IShellFolder::EnumObjects, then use SHGDN_FORPARSING to obtain the
// AppUserModelID (AUMID) used to launch the app.
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
        std::sort(hits.begin(), hits.end(), [&](const H& a, const H& b) {
            if (a.rank != b.rank) return a.rank < b.rank;
            return g_items[a.i].lower < g_items[b.i].lower;
        });
    }
    for (size_t i = 0; i < hits.size() && i < 300; i++) g_res.push_back(hits[i].i);
}

// ---------------------------------------------------------------- layout
struct Lay { int mx, titleY, lblY, editY, editH, editR, btnW, listY, rowH, vis; };
static Lay Layout() {
    Lay l;
    l.mx = S(41); l.titleY = S(35); l.lblY = S(80);
    l.editY = S(110); l.editH = S(32);
    l.editR = g_w - S(40); l.btnW = S(32);
    l.listY = S(168); l.rowH = S(52);
    l.vis = std::max(1, (g_h - l.listY - S(20)) / l.rowH);
    return l;
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

        struct HeaderInfo { const wchar_t* text; int y; };
        HeaderInfo headers[8];
        int headerCount = 0;

        int y = L.listY;
        Cat lastCat = (Cat)-1;
        for (int r = g_scroll; r < (int)g_res.size() && r < g_scroll + L.vis; r++, y += L.rowH) {
            int idx = g_res[r];
            if (idx < 0 || idx >= (int)g_items.size()) continue;
            const Item& it = g_items[idx];

            if (it.cat != lastCat) {
                lastCat = it.cat;
                if (headerCount < 8) {
                    headers[headerCount].text = it.cat == CAT_APP ? T(20) :
                                                it.cat == CAT_SETTING ? T(21) : T(11);
                    // Section titles (Apps / Settings / Files) are drawn 3% higher
                    // than before. Only the header text position is shifted; every
                    // other element (icons, names, paths) keeps its original Y.
                    headers[headerCount].y = y - S(14) - S(14) * 3 / 100;
                    headerCount++;
                }
            }

            if (r == g_sel || r == g_hover) {
                RECT band = { px, y, W, y + L.rowH };
                GdiObj<HBRUSH> bb(CreateSolidBrush(Blend(pane, r == g_sel ? 24 : 14)));
                FillRect(mem.get(), &band, bb.get());
            }
            HICON ic = GetIcon(it.path);
            if (ic)
                DrawIconEx(mem.get(), L.mx + ox, y + (L.rowH - S(32)) / 2, ic, S(32), S(32), 0, NULL, DI_NORMAL);
            int tx = L.mx + ox + S(44);
            RECT nr = { tx, y + S(6), W - S(16) + ox, y + S(28) };
            RECT sr = { tx, y + S(28), W - S(16) + ox, y + S(48) };

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

        for (int i = 0; i < headerCount; i++) {
            ScopedSelectedObject so(mem.get(), g_fHeader);
            ScopedTextColor tc(mem.get(), RGB(240, 248, 240));
            RECT hr = { L.mx + ox, headers[i].y, W - S(16) + ox, headers[i].y + S(20) };
            DrawTextW(mem.get(), headers[i].text, -1, &hr, DT_SINGLELINE | DT_NOPREFIX);
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

// ---------------------------------------------------------------- context menu
//
// The context menu is deliberately kept open until either the search panel
// is closed or an entry is launched. There is no fixed timeout: the menu
// blocks the UI thread inside TrackPopupMenu until the user makes a choice
// or dismisses it by other means (Esc, click outside the menu itself).
// Additionally, the menu is explicitly dismissed when the search panel is
// closed or when a result is launched, so it cannot outlive the panel.
static HMENU g_contextMenu = NULL;
static bool  g_contextMenuActive = false;

static void DismissContextMenu() {
    if (g_contextMenuActive && g_contextMenu) {
        // Ask the menu to dismiss itself. The UI thread is currently blocked
        // inside TrackPopupMenu for that menu; posting WM_CANCELMODE causes
        // TrackPopupMenu to return, which lets us clean up.
        if (g_wnd) PostMessageW(g_wnd, WM_CANCELMODE, 0, 0);
        g_contextMenuActive = false;
    }
}

static void ClosePane() {
    if (!g_wnd || !g_open) return;
    DismissContextMenu();
    StartAnim(false);
}

// ---------------------------------------------------------------- topmost enforcement
// Re-raise the panel when needed. The 8102 Charms mod re-raises every so often
// to survive other topmost windows popping on top; the search panel does the
// same but stops early and falls back to plain topmost if it keeps getting
// covered (which normally means a UIAccess window is deliberately on top).
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

    // Sample the center of the panel to check whether something covers it.
    RECT r; GetWindowRect(g_wnd, &r);
    POINT c = { (r.left + r.right) / 2, (r.top + r.bottom) / 2 };
    bool covered = (WindowFromPoint(c) != g_wnd);

    if (!covered) {
        // Restore the band when we are not covered any more.
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

        SetWindowPos(g_wnd, HWND_TOPMOST, r.right - g_w, r.top, g_w, g_h,
                     SWP_NOACTIVATE | SWP_NOREDRAW);

        g_query.clear(); g_caret = true; g_selectAll = false; g_inMenu = false;
        g_dropdownOpen = false; g_scope = SCOPE_ALL;
        g_raiseCount = 0; g_raiseSince = 0; g_gaveUp = false;
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

static void ShowContextMenu(HWND h, int x, int y, int r) {
    if (r < 0 || r >= (int)g_res.size()) return;
    std::wstring path;
    { SrwGuard g(&g_lock, false); int idx = g_res[r]; if (idx >= 0 && idx < (int)g_items.size()) path = g_items[idx].path; }
    if (path.empty()) return;

    g_inMenu = true;
    SetForegroundWindow(h);

    HMENU m = CreatePopupMenu();
    if (!m) { g_inMenu = false; return; }
    AppendMenuW(m, MF_STRING, 1, T(16));
    AppendMenuW(m, MF_STRING, 2, T(17));
    AppendMenuW(m, MF_STRING, 3, T(18));

    g_contextMenu = m;
    g_contextMenuActive = true;

    int cmd = TrackPopupMenu(m, TPM_RETURNCMD | TPM_RIGHTBUTTON, x, y, 0, h, NULL);

    g_contextMenuActive = false;
    g_contextMenu = NULL;
    DestroyMenu(m);

    g_inMenu = false;
    PostMessageW(h, WM_NULL, 0, 0);

    if (cmd == 1) {
        wchar_t args[MAX_PATH + 16];
        StringCchPrintfW(args, MAX_PATH + 16, L"/select,\"%s\"", path.c_str());
        ShellExecuteW(NULL, L"open", L"explorer.exe", args, NULL, SW_SHOWNORMAL);
    } else if (cmd == 2) {
        SHELLEXECUTEINFOW sei = { sizeof(sei) };
        sei.fMask = SEE_MASK_NOCLOSEPROCESS;
        sei.lpVerb = L"runas";
        sei.lpFile = path.c_str();
        sei.nShow = SW_SHOWNORMAL;
        ShellExecuteExW(&sei);
    } else if (cmd == 3) {
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
    }
}

// ---------------------------------------------------------------- registry virtualization (WSearch)
//
// We never write to the real registry. Instead we hook the registry
// APIs and, whenever a caller opens or reads the WSearch service key,
// we hand back a *virtual* handle that reports Start=4 (disabled).
//
// The real key is still opened underneath (so that other values
// remain accessible) but the caller never sees the real handle: it
// sees a virtual one. When the caller closes the virtual handle we
// also close the real one and forget the mapping.
//
// Writing Start through a virtual handle is a silent no-op, so nothing
// can "repair" the value while the mod is loaded.
//
// On unload, every still-open virtual handle is closed along with its
// real counterpart. Nothing has to be restored because nothing was
// ever changed.

static const wchar_t* kWSearchKeyPath   = L"SYSTEM\\CurrentControlSet\\Services\\WSearch";
static const wchar_t* kWSearchKeySuffix = L"\\Services\\WSearch";
static const wchar_t* kWSearchValueName = L"Start";
static const DWORD    kWSearchDisabled  = 4; // SERVICE_DISABLED

static CRITICAL_SECTION g_vkeyLock;
static bool g_vkeyLockInit = false;
static std::unordered_map<HKEY, HKEY> g_virtualToReal;
static std::unordered_map<HKEY, HKEY> g_realToVirtual;
static HKEY g_nextVirtualKey = (HKEY)0xDEAD0001;

static bool IsWSearchKeyPath(const wchar_t* path) {
    if (!path) return false;
    size_t len = wcslen(path);
    size_t suf = wcslen(kWSearchKeySuffix);
    if (len >= suf) {
        if (_wcsicmp(path + len - suf, kWSearchKeySuffix) == 0) return true;
    }
    if (_wcsicmp(path, kWSearchKeyPath) == 0) return true;
    return false;
}

static bool IsWSearchValueName(const wchar_t* name) {
    return name && _wcsicmp(name, kWSearchValueName) == 0;
}

static HKEY AllocateVirtualKey(HKEY realKey) {
    EnterCriticalSection(&g_vkeyLock);
    HKEY vk = g_nextVirtualKey++;
    g_virtualToReal[vk] = realKey;
    g_realToVirtual[realKey] = vk;
    LeaveCriticalSection(&g_vkeyLock);
    return vk;
}

static HKEY GetRealFromVirtual(HKEY vk) {
    EnterCriticalSection(&g_vkeyLock);
    auto it = g_virtualToReal.find(vk);
    HKEY result = (it != g_virtualToReal.end()) ? it->second : NULL;
    LeaveCriticalSection(&g_vkeyLock);
    return result;
}

static bool IsVirtualKey(HKEY k) {
    EnterCriticalSection(&g_vkeyLock);
    bool found = g_virtualToReal.find(k) != g_virtualToReal.end();
    LeaveCriticalSection(&g_vkeyLock);
    return found;
}

// ---------------------------------------------------------------- registry hook typedefs
using RegOpenKeyExW_t    = LSTATUS (WINAPI*)(HKEY, LPCWSTR, DWORD, REGSAM, PHKEY);
using RegOpenKeyExA_t    = LSTATUS (WINAPI*)(HKEY, LPCSTR,  DWORD, REGSAM, PHKEY);
using RegQueryValueExW_t = LSTATUS (WINAPI*)(HKEY, LPCWSTR, LPDWORD, LPDWORD, LPBYTE, LPDWORD);
using RegQueryValueExA_t = LSTATUS (WINAPI*)(HKEY, LPCSTR,  LPDWORD, LPDWORD, LPBYTE, LPDWORD);
using RegSetValueExW_t   = LSTATUS (WINAPI*)(HKEY, LPCWSTR, DWORD, DWORD, const BYTE*, DWORD);
using RegSetValueExA_t   = LSTATUS (WINAPI*)(HKEY, LPCSTR,  DWORD, DWORD, const BYTE*, DWORD);
using RegCloseKey_t      = LSTATUS (WINAPI*)(HKEY);
using RegEnumValueW_t    = LSTATUS (WINAPI*)(HKEY, DWORD, LPWSTR, LPDWORD, LPDWORD, LPDWORD, LPBYTE, LPDWORD);
using RegEnumValueA_t    = LSTATUS (WINAPI*)(HKEY, DWORD, LPSTR,  LPDWORD, LPDWORD, LPDWORD, LPBYTE, LPDWORD);
using RegCreateKeyExW_t  = LSTATUS (WINAPI*)(HKEY, LPCWSTR, DWORD, LPWSTR, DWORD, REGSAM, LPSECURITY_ATTRIBUTES, PHKEY, LPDWORD);
using RegCreateKeyExA_t  = LSTATUS (WINAPI*)(HKEY, LPCSTR,  DWORD, LPSTR,  DWORD, REGSAM, LPSECURITY_ATTRIBUTES, PHKEY, LPDWORD);

static RegOpenKeyExW_t    RegOpenKeyExW_Original    = nullptr;
static RegOpenKeyExA_t    RegOpenKeyExA_Original    = nullptr;
static RegQueryValueExW_t RegQueryValueExW_Original = nullptr;
static RegQueryValueExA_t RegQueryValueExA_Original = nullptr;
static RegSetValueExW_t   RegSetValueExW_Original   = nullptr;
static RegSetValueExA_t   RegSetValueExA_Original   = nullptr;
static RegCloseKey_t      RegCloseKey_Original      = nullptr;
static RegEnumValueW_t    RegEnumValueW_Original    = nullptr;
static RegEnumValueA_t    RegEnumValueA_Original    = nullptr;
static RegCreateKeyExW_t  RegCreateKeyExW_Original  = nullptr;
static RegCreateKeyExA_t  RegCreateKeyExA_Original  = nullptr;

// ---------------------------------------------------------------- registry hook implementations
static LSTATUS WINAPI RegOpenKeyExW_Hook(HKEY hKey, LPCWSTR lpSubKey, DWORD ulOptions,
                                          REGSAM samDesired, PHKEY phkResult) {
    LSTATUS result = RegOpenKeyExW_Original(hKey, lpSubKey, ulOptions, samDesired, phkResult);
    if (result == ERROR_SUCCESS && phkResult && *phkResult && g_blockNativeSearch &&
        IsWSearchKeyPath(lpSubKey)) {
        HKEY realKey = *phkResult;
        HKEY vk = AllocateVirtualKey(realKey);
        *phkResult = vk;
        if (g_debug) Wh_Log(L"[RegVirt] RegOpenKeyExW WSearch -> virtual %p (real %p)", vk, realKey);
    }
    return result;
}

static LSTATUS WINAPI RegOpenKeyExA_Hook(HKEY hKey, LPCSTR lpSubKey, DWORD ulOptions,
                                          REGSAM samDesired, PHKEY phkResult) {
    LSTATUS result = RegOpenKeyExA_Original(hKey, lpSubKey, ulOptions, samDesired, phkResult);
    if (result == ERROR_SUCCESS && phkResult && *phkResult && lpSubKey && g_blockNativeSearch) {
        wchar_t wide[512] = {};
        MultiByteToWideChar(CP_ACP, 0, lpSubKey, -1, wide, ARRAYSIZE(wide));
        if (IsWSearchKeyPath(wide)) {
            HKEY realKey = *phkResult;
            HKEY vk = AllocateVirtualKey(realKey);
            *phkResult = vk;
            if (g_debug) Wh_Log(L"[RegVirt] RegOpenKeyExA WSearch -> virtual %p (real %p)", vk, realKey);
        }
    }
    return result;
}

static LSTATUS WINAPI RegQueryValueExW_Hook(HKEY hKey, LPCWSTR lpValueName, LPDWORD lpReserved,
                                             LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData) {
    if (g_blockNativeSearch && IsVirtualKey(hKey) && IsWSearchValueName(lpValueName)) {
        if (lpType) *lpType = REG_DWORD;
        if (lpData && lpcbData) {
            if (*lpcbData >= sizeof(DWORD)) {
                *(DWORD*)lpData = kWSearchDisabled;
                *lpcbData = sizeof(DWORD);
                if (g_debug) Wh_Log(L"[RegVirt] RegQueryValueExW Start -> virtualized to 4");
                return ERROR_SUCCESS;
            } else {
                *lpcbData = sizeof(DWORD);
                return ERROR_MORE_DATA;
            }
        }
        return ERROR_SUCCESS;
    }
    return RegQueryValueExW_Original(hKey, lpValueName, lpReserved, lpType, lpData, lpcbData);
}

static LSTATUS WINAPI RegQueryValueExA_Hook(HKEY hKey, LPCSTR lpValueName, LPDWORD lpReserved,
                                             LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData) {
    if (g_blockNativeSearch && IsVirtualKey(hKey) && lpValueName &&
        _stricmp(lpValueName, "Start") == 0) {
        if (lpType) *lpType = REG_DWORD;
        if (lpData && lpcbData) {
            if (*lpcbData >= sizeof(DWORD)) {
                *(DWORD*)lpData = kWSearchDisabled;
                *lpcbData = sizeof(DWORD);
                if (g_debug) Wh_Log(L"[RegVirt] RegQueryValueExA Start -> virtualized to 4");
                return ERROR_SUCCESS;
            } else {
                *lpcbData = sizeof(DWORD);
                return ERROR_MORE_DATA;
            }
        }
        return ERROR_SUCCESS;
    }
    return RegQueryValueExA_Original(hKey, lpValueName, lpReserved, lpType, lpData, lpcbData);
}

static LSTATUS WINAPI RegSetValueExW_Hook(HKEY hKey, LPCWSTR lpValueName, DWORD Reserved,
                                           DWORD dwType, const BYTE* lpData, DWORD cbData) {
    if (g_blockNativeSearch && IsVirtualKey(hKey) && IsWSearchValueName(lpValueName)) {
        if (g_debug) Wh_Log(L"[RegVirt] RegSetValueExW Start suppressed on virtual handle");
        return ERROR_SUCCESS;
    }
    if (IsVirtualKey(hKey)) {
        HKEY real = GetRealFromVirtual(hKey);
        if (real) return RegSetValueExW_Original(real, lpValueName, Reserved, dwType, lpData, cbData);
    }
    return RegSetValueExW_Original(hKey, lpValueName, Reserved, dwType, lpData, cbData);
}

static LSTATUS WINAPI RegSetValueExA_Hook(HKEY hKey, LPCSTR lpValueName, DWORD Reserved,
                                           DWORD dwType, const BYTE* lpData, DWORD cbData) {
    if (g_blockNativeSearch && IsVirtualKey(hKey) && lpValueName &&
        _stricmp(lpValueName, "Start") == 0) {
        if (g_debug) Wh_Log(L"[RegVirt] RegSetValueExA Start suppressed on virtual handle");
        return ERROR_SUCCESS;
    }
    if (IsVirtualKey(hKey)) {
        HKEY real = GetRealFromVirtual(hKey);
        if (real) return RegSetValueExA_Original(real, lpValueName, Reserved, dwType, lpData, cbData);
    }
    return RegSetValueExA_Original(hKey, lpValueName, Reserved, dwType, lpData, cbData);
}

static LSTATUS WINAPI RegCloseKey_Hook(HKEY hKey) {
    if (IsVirtualKey(hKey)) {
        HKEY real = GetRealFromVirtual(hKey);
        EnterCriticalSection(&g_vkeyLock);
        g_virtualToReal.erase(hKey);
        if (real) g_realToVirtual.erase(real);
        LeaveCriticalSection(&g_vkeyLock);
        if (real) {
            if (g_debug) Wh_Log(L"[RegVirt] RegCloseKey virtual %p -> real %p", hKey, real);
            return RegCloseKey_Original(real);
        }
        return ERROR_SUCCESS;
    }
    return RegCloseKey_Original(hKey);
}

static LSTATUS WINAPI RegEnumValueW_Hook(HKEY hKey, DWORD dwIndex, LPWSTR lpValueName,
                                          LPDWORD lpcchValueName, LPDWORD lpReserved,
                                          LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData) {
    if (g_blockNativeSearch && IsVirtualKey(hKey)) {
        HKEY real = GetRealFromVirtual(hKey);
        if (real) {
            LSTATUS result = RegEnumValueW_Original(real, dwIndex, lpValueName,
                                                     lpcchValueName, lpReserved,
                                                     lpType, lpData, lpcbData);
            if (result == ERROR_SUCCESS && lpValueName && IsWSearchValueName(lpValueName)) {
                if (lpType) *lpType = REG_DWORD;
                if (lpData && lpcbData && *lpcbData >= sizeof(DWORD)) {
                    *(DWORD*)lpData = kWSearchDisabled;
                    *lpcbData = sizeof(DWORD);
                }
                if (g_debug) Wh_Log(L"[RegVirt] RegEnumValueW Start -> virtualized to 4");
            }
            return result;
        }
    }
    return RegEnumValueW_Original(hKey, dwIndex, lpValueName, lpcchValueName,
                                   lpReserved, lpType, lpData, lpcbData);
}

static LSTATUS WINAPI RegEnumValueA_Hook(HKEY hKey, DWORD dwIndex, LPSTR lpValueName,
                                          LPDWORD lpcchValueName, LPDWORD lpReserved,
                                          LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData) {
    if (g_blockNativeSearch && IsVirtualKey(hKey)) {
        HKEY real = GetRealFromVirtual(hKey);
        if (real) {
            LSTATUS result = RegEnumValueA_Original(real, dwIndex, lpValueName,
                                                     lpcchValueName, lpReserved,
                                                     lpType, lpData, lpcbData);
            if (result == ERROR_SUCCESS && lpValueName &&
                _stricmp(lpValueName, "Start") == 0) {
                if (lpType) *lpType = REG_DWORD;
                if (lpData && lpcbData && *lpcbData >= sizeof(DWORD)) {
                    *(DWORD*)lpData = kWSearchDisabled;
                    *lpcbData = sizeof(DWORD);
                }
            }
            return result;
        }
    }
    return RegEnumValueA_Original(hKey, dwIndex, lpValueName, lpcchValueName,
                                   lpReserved, lpType, lpData, lpcbData);
}

static LSTATUS WINAPI RegCreateKeyExW_Hook(HKEY hKey, LPCWSTR lpSubKey, DWORD Reserved,
                                            LPWSTR lpClass, DWORD dwOptions, REGSAM samDesired,
                                            LPSECURITY_ATTRIBUTES lpSecurityAttributes,
                                            PHKEY phkResult, LPDWORD lpdwDisposition) {
    LSTATUS result = RegCreateKeyExW_Original(hKey, lpSubKey, Reserved, lpClass, dwOptions,
                                               samDesired, lpSecurityAttributes,
                                               phkResult, lpdwDisposition);
    if (result == ERROR_SUCCESS && phkResult && *phkResult && g_blockNativeSearch &&
        IsWSearchKeyPath(lpSubKey)) {
        HKEY realKey = *phkResult;
        HKEY vk = AllocateVirtualKey(realKey);
        *phkResult = vk;
        if (g_debug) Wh_Log(L"[RegVirt] RegCreateKeyExW WSearch -> virtual %p (real %p)", vk, realKey);
    }
    return result;
}

static LSTATUS WINAPI RegCreateKeyExA_Hook(HKEY hKey, LPCSTR lpSubKey, DWORD Reserved,
                                            LPSTR lpClass, DWORD dwOptions, REGSAM samDesired,
                                            LPSECURITY_ATTRIBUTES lpSecurityAttributes,
                                            PHKEY phkResult, LPDWORD lpdwDisposition) {
    LSTATUS result = RegCreateKeyExA_Original(hKey, lpSubKey, Reserved, lpClass, dwOptions,
                                               samDesired, lpSecurityAttributes,
                                               phkResult, lpdwDisposition);
    if (result == ERROR_SUCCESS && phkResult && *phkResult && lpSubKey && g_blockNativeSearch) {
        wchar_t wide[512] = {};
        MultiByteToWideChar(CP_ACP, 0, lpSubKey, -1, wide, ARRAYSIZE(wide));
        if (IsWSearchKeyPath(wide)) {
            HKEY realKey = *phkResult;
            HKEY vk = AllocateVirtualKey(realKey);
            *phkResult = vk;
            if (g_debug) Wh_Log(L"[RegVirt] RegCreateKeyExA WSearch -> virtual %p (real %p)", vk, realKey);
        }
    }
    return result;
}

// ---------------------------------------------------------------- WSearch service stop
static void StopWSearchServiceIfRunning() {
    if (!g_stopWSearchService) return;

    SC_HANDLE hSCM = OpenSCManagerW(NULL, NULL, SC_MANAGER_CONNECT);
    if (!hSCM) {
        Wh_Log(L"[Svc] OpenSCManager failed (err=%lu)", GetLastError());
        return;
    }
    SC_HANDLE hService = OpenServiceW(hSCM, L"WSearch", SERVICE_STOP | SERVICE_QUERY_STATUS);
    if (!hService) {
        Wh_Log(L"[Svc] OpenService(WSearch) failed (err=%lu)", GetLastError());
        CloseServiceHandle(hSCM);
        return;
    }
    SERVICE_STATUS status = {};
    BOOL ok = ControlService(hService, SERVICE_CONTROL_STOP, &status);
    DWORD err = GetLastError();
    CloseServiceHandle(hService);
    CloseServiceHandle(hSCM);

    if (ok) {
        Wh_Log(L"[Svc] WSearch stop requested");
    } else if (err == ERROR_SERVICE_NOT_ACTIVE) {
        Wh_Log(L"[Svc] WSearch was not running");
    } else {
        Wh_Log(L"[Svc] ControlService(STOP) failed (err=%lu)", err);
    }
}

// ---------------------------------------------------------------- ShellExecuteExW hook
using ShellExecuteExW_t = BOOL (WINAPI*)(SHELLEXECUTEINFOW*);
static ShellExecuteExW_t ShellExecuteExW_Original = nullptr;

static BOOL WINAPI ShellExecuteExW_Hook(SHELLEXECUTEINFOW* sei) {
    if (sei && sei->lpFile && g_blockNativeSearch) {
        const wchar_t* f = sei->lpFile;
        if (_wcsnicmp(f, L"search-ms:", 10) == 0 ||
            _wcsnicmp(f, L"ms-search:", 10) == 0) {
            Wh_Log(L"[Block/Shell] native search URI: %s", f);
            return FALSE;
        }
        const wchar_t* base = wcsrchr(f, L'\\');
        base = base ? base + 1 : f;
        if (_wcsicmp(base, L"SearchHost.exe") == 0 ||
            _wcsicmp(base, L"SearchUI.exe") == 0 ||
            _wcsicmp(base, L"SearchApp.exe") == 0) {
            Wh_Log(L"[Block/Shell] native search executable: %s", f);
            return FALSE;
        }
    }
    if (!ShellExecuteExW_Original) return FALSE;
    return ShellExecuteExW_Original(sei);
}

// ---------------------------------------------------------------- CreateProcessInternalW hook
using CreateProcessInternalW_t = BOOL (WINAPI*)(
    HANDLE hUserToken, LPCWSTR lpApplicationName, LPWSTR lpCommandLine,
    LPSECURITY_ATTRIBUTES lpProcessAttributes, LPSECURITY_ATTRIBUTES lpThreadAttributes,
    BOOL bInheritHandles, DWORD dwCreationFlags, LPVOID lpEnvironment,
    LPCWSTR lpCurrentDirectory, LPSTARTUPINFOW lpStartupInfo,
    LPPROCESS_INFORMATION lpProcessInformation, PHANDLE hRestrictedUserToken);

static CreateProcessInternalW_t CreateProcessInternalW_Original = nullptr;

static bool LooksLikeNativeSearchLaunch(LPCWSTR appName, LPCWSTR cmdLine) {
    if (appName) {
        const wchar_t* base = wcsrchr(appName, L'\\');
        base = base ? base + 1 : appName;
        if (_wcsicmp(base, L"SearchHost.exe") == 0) return true;
        if (_wcsicmp(base, L"SearchApp.exe") == 0) return true;
        if (_wcsicmp(base, L"SearchUI.exe") == 0) return true;
    }
    if (cmdLine) {
        if (StrStrIW(cmdLine, L"SearchHost.exe")) return true;
        if (StrStrIW(cmdLine, L"SearchApp.exe")) return true;
        if (StrStrIW(cmdLine, L"SearchUI.exe")) return true;
        if (StrStrIW(cmdLine, L"--webview-exe-name=SearchHost.exe")) return true;
        if (StrStrIW(cmdLine, L"--webview-exe-name=SearchApp.exe")) return true;
    }
    return false;
}

static BOOL WINAPI CreateProcessInternalW_Hook(
    HANDLE hUserToken, LPCWSTR lpApplicationName, LPWSTR lpCommandLine,
    LPSECURITY_ATTRIBUTES lpProcessAttributes, LPSECURITY_ATTRIBUTES lpThreadAttributes,
    BOOL bInheritHandles, DWORD dwCreationFlags, LPVOID lpEnvironment,
    LPCWSTR lpCurrentDirectory, LPSTARTUPINFOW lpStartupInfo,
    LPPROCESS_INFORMATION lpProcessInformation, PHANDLE hRestrictedUserToken)
{
    try {
        if (g_blockNativeSearch && LooksLikeNativeSearchLaunch(lpApplicationName, lpCommandLine)) {
            static RateLimitedLog log{L"native search process blocked", 10};
            if (log.ShouldLog()) {
                Wh_Log(L"[Block/Proc] blocked native search launch: app=%s cmd=%s",
                       lpApplicationName ? lpApplicationName : L"(null)",
                       lpCommandLine ? lpCommandLine : L"(null)");
            }
            SetLastError(ERROR_ACCESS_DENIED);
            return FALSE;
        }
    } catch (...) {}
    if (!CreateProcessInternalW_Original) {
        SetLastError(ERROR_PROC_NOT_FOUND);
        return FALSE;
    }
    return CreateProcessInternalW_Original(
        hUserToken, lpApplicationName, lpCommandLine, lpProcessAttributes,
        lpThreadAttributes, bInheritHandles, dwCreationFlags, lpEnvironment,
        lpCurrentDirectory, lpStartupInfo, lpProcessInformation, hRestrictedUserToken);
}

// ---------------------------------------------------------------- window procedure
static LRESULT CALLBACK WndProc(HWND h, UINT msg, WPARAM wp, LPARAM lp) {
    try {
        switch (msg) {
        case WM_PAINT: DoPaint(h); return 0;
        case WM_ERASEBKGND: return 1;
        case WM_CANCELMODE:
            // Forwarded to us by DismissContextMenu(): TrackPopupMenu is
            // currently blocking on the UI thread, and WM_CANCELMODE makes
            // it return. Nothing else to do here.
            return 0;
        case WM_TIMER:
            if (wp == 1) Tick();
            else if (wp == 2) { g_caret = !g_caret; if (IsWindowVisible(h)) InvalidateRect(h, NULL, FALSE); }
            else if (wp == 3) { if (g_open) EnforceTopmost(); }
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
                if (g_contextMenuActive) { DismissContextMenu(); break; }
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
            if (g_state != ST_IDLE) return 0;
            TRACKMOUSEEVENT tme = { sizeof(tme), TME_LEAVE, h, 0 };
            TrackMouseEvent(&tme);
            int x = (short)LOWORD(lp), y = (short)HIWORD(lp);
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
        case WM_LBUTTONDOWN: {
            if (g_state != ST_IDLE || !g_open) return 0;
            DWORD now = GetTickCount();
            if (now - g_lastClick < (DWORD)g_clickCooldownMs) return 0;
            g_lastClick = now;
            int x = (short)LOWORD(lp), y = (short)HIWORD(lp);
            Lay L = Layout();

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
            int r = RowAt((short)HIWORD(lp));
            if (r >= 0) { POINT pt; GetCursorPos(&pt); ShowContextMenu(h, pt.x, pt.y, r); }
            return 0;
        }
        case WM_DPICHANGED: {
            g_scale = HIWORD(wp) / 96.0;
            g_fontScale = 0;
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
    SendInput(2, in, sizeof(INPUT));
}

static LRESULT CALLBACK KbProc(int code, WPARAM wp, LPARAM lp) {
    try {
        if (code == HC_ACTION) {
            const KBDLLHOOKSTRUCT* k = (const KBDLLHOOKSTRUCT*)lp;
            if (k->dwExtraInfo != kMagic) {
                bool down = (wp == WM_KEYDOWN || wp == WM_SYSKEYDOWN);
                bool up = (wp == WM_KEYUP || wp == WM_SYSKEYUP);
                if (up && g_swallowVk && k->vkCode == g_swallowVk) { g_swallowVk = 0; return 1; }
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
                    if (g_wnd) PostMessageW(g_wnd, WM_APP_TOGGLE, 0, 0);
                    return 1;
                }
            }
        }
    } catch (...) {
        Wh_Log(L"[Hook] exception in KbProc");
    }
    return CallNextHookEx(g_hook, code, wp, lp);
}

// ---------------------------------------------------------------- mouse hook
static LRESULT CALLBACK MouseProc(int code, WPARAM wp, LPARAM lp) {
    try {
        if (code == HC_ACTION && g_closeOnClickOutside && g_open && g_wnd && !g_inMenu &&
            (wp == WM_LBUTTONDOWN || wp == WM_RBUTTONDOWN || wp == WM_MBUTTONDOWN)) {
            const MSLLHOOKSTRUCT* m = (const MSLLHOOKSTRUCT*)lp;
            if (m->dwExtraInfo != kMagic) {
                DWORD now = GetTickCount();
                if (now - g_lastClick >= (DWORD)g_clickCooldownMs) {
                    RECT r;
                    if (GetWindowRect(g_wnd, &r) && !PtInRect(&r, m->pt)) {
                        g_lastClick = now;
                        if (g_state == ST_IDLE) ClosePane();
                    }
                }
            }
        }
    } catch (...) {}
    return CallNextHookEx(g_mouseHook, code, wp, lp);
}

// ---------------------------------------------------------------- UI thread
static DWORD WINAPI UiThread(LPVOID) {
    RunGuarded(L"the UI thread", [] {
        SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
        ScopedComApartment com(SUCCEEDED(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED)));

        // Resolve CreateWindowInBand early so the panel is born in the right band.
        HMODULE user32 = GetModuleHandleW(L"user32.dll");
        if (user32)
            g_createInBand = (CreateWindowInBand_t)(void*)GetProcAddress(user32, "CreateWindowInBand");
        if (!g_createInBand)
            Wh_Log(L"[ZBand] CreateWindowInBand not available, using ordinary topmost windows");

        g_hook = SetWindowsHookExW(WH_KEYBOARD_LL, KbProc, GetModuleHandleW(NULL), 0);
        g_mouseHook = SetWindowsHookExW(WH_MOUSE_LL, MouseProc, GetModuleHandleW(NULL), 0);
        Wh_Log(L"[UI] hooks: kb=%p mouse=%p", g_hook, g_mouseHook);

        HINSTANCE hi = (HINSTANCE)&__ImageBase;
        WNDCLASSW wc = {};
        wc.lpfnWndProc = WndProc;
        wc.hInstance = hi;
        wc.hCursor = LoadCursorW(NULL, IDC_ARROW);
        wc.lpszClassName = kClass;
        RegisterClassW(&wc);
        g_wnd = CreateOverlayWindow(kClass, 346, 600);
        if (g_wnd) SetLayeredWindowAttributes(g_wnd, KEY, 255, LWA_COLORKEY);
        if (!g_wnd) {
            Wh_Log(L"[UI] could not create panel window, err=%u", (unsigned)GetLastError());
            return;
        }

        SetTimer(g_wnd, 3, 2000, NULL);
        StartScan();

        MSG m;
        while (GetMessageW(&m, NULL, 0, 0)) {
            TranslateMessage(&m);
            DispatchMessageW(&m);
        }

        if (g_hook) { UnhookWindowsHookEx(g_hook); g_hook = NULL; }
        if (g_mouseHook) { UnhookWindowsHookEx(g_mouseHook); g_mouseHook = NULL; }
        if (g_wnd) { DestroyWindow(g_wnd); g_wnd = NULL; }
        for (auto& kv : g_icons) if (kv.second) DestroyIcon(kv.second);
        g_icons.clear();
        HFONT* f[] = { &g_fTitle, &g_fLabel, &g_fText, &g_fName, &g_fSub, &g_fGlyph, &g_fHeader };
        for (auto p : f) if (*p) { DeleteObject(*p); *p = NULL; }
        UnregisterClassW(kClass, hi);
    });
    return 0;
}

void Wh_ModSettingsChanged() { RunGuarded(L"applying settings", [] { LoadSettings(); }); }

BOOL Wh_ModInit() {
    return RunGuarded(L"initializing the mod", [] {
        LARGE_INTEGER f; QueryPerformanceFrequency(&f);
        g_freq = f.QuadPart ? f.QuadPart : 1;

        InitializeCriticalSection(&g_vkeyLock);
        g_vkeyLockInit = true;

        LoadSettings();

        // ------------------------------------------------------------
        // Registry virtualization for WSearch (so the native search
        // service appears disabled without ever touching the real
        // registry). The real value is preserved automatically because
        // we only ever hand out virtual handles.
        // ------------------------------------------------------------
        if (g_blockNativeSearch) {
            HMODULE hAdvapi = GetModuleHandleW(L"advapi32.dll");
            if (!hAdvapi) hAdvapi = LoadLibraryW(L"advapi32.dll");
            if (hAdvapi) {
                #define HOOK_REG(name) \
                    { void* p = (void*)GetProcAddress(hAdvapi, #name); \
                      if (p) { \
                          Wh_SetFunctionHook(p, (void*)name##_Hook, (void**)&name##_Original); \
                          Wh_Log(L"[Init] hooked " L## #name); \
                      } else { \
                          Wh_Log(L"[Init] could not resolve " L## #name); \
                      } }

                HOOK_REG(RegOpenKeyExW);
                HOOK_REG(RegOpenKeyExA);
                HOOK_REG(RegQueryValueExW);
                HOOK_REG(RegQueryValueExA);
                HOOK_REG(RegSetValueExW);
                HOOK_REG(RegSetValueExA);
                HOOK_REG(RegCloseKey);
                HOOK_REG(RegEnumValueW);
                HOOK_REG(RegEnumValueA);
                HOOK_REG(RegCreateKeyExW);
                HOOK_REG(RegCreateKeyExA);
                #undef HOOK_REG
            } else {
                Wh_Log(L"[Init] advapi32.dll not available, registry virtualization disabled");
            }

            StopWSearchServiceIfRunning();
        }

        HMODULE hShell = GetModuleHandleW(L"shell32.dll");
        if (hShell) {
            void* p = (void*)GetProcAddress(hShell, "ShellExecuteExW");
            if (p) {
                Wh_SetFunctionHook(p, (void*)ShellExecuteExW_Hook, (void**)&ShellExecuteExW_Original);
                Wh_Log(L"[Init] ShellExecuteExW hooked, original=%p", ShellExecuteExW_Original);
            }
        }

        HMODULE hKernelBase = GetModuleHandleW(L"kernelbase.dll");
        if (hKernelBase) {
            void* p = (void*)GetProcAddress(hKernelBase, "CreateProcessInternalW");
            if (p) {
                Wh_SetFunctionHook(p, (void*)CreateProcessInternalW_Hook,
                                   (void**)&CreateProcessInternalW_Original);
                Wh_Log(L"[Init] CreateProcessInternalW hooked");
            }
        }

        DWORD tid = 0;
        HANDLE raw = CreateThread(NULL, 0, UiThread, NULL, 0, &tid);
        g_tid = tid;
        g_thread.reset(raw);
        return g_thread.get() != NULL;
    }) ? TRUE : FALSE;
}

void Wh_ModUninit() {
    Wh_Log(L"[Uninit] shutting down, exceptions=%u", g_exceptionCount.load());
    InterlockedExchange(&g_stop, 1);
    if (g_tid) PostThreadMessageW(g_tid, WM_QUIT, 0, 0);
    if (g_thread.get()) { WaitForSingleObject(g_thread.get(), INFINITE); g_thread.reset(); }
    if (g_scanThread.get()) { WaitForSingleObject(g_scanThread.get(), INFINITE); g_scanThread.reset(); }

    // Release any virtual handles still open so we do not leak real
    // registry handles. Nothing has to be "restored" because nothing
    // was ever written to the real registry.
    if (g_vkeyLockInit) {
        EnterCriticalSection(&g_vkeyLock);
        for (auto& kv : g_virtualToReal) {
            if (kv.second && RegCloseKey_Original) RegCloseKey_Original(kv.second);
        }
        g_virtualToReal.clear();
        g_realToVirtual.clear();
        LeaveCriticalSection(&g_vkeyLock);
        DeleteCriticalSection(&g_vkeyLock);
        g_vkeyLockInit = false;
    }
    Wh_Log(L"[Uninit] all virtual registry handles released");
}
