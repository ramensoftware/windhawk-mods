// ==WindhawkMod==
// @id            photoshop-dark-menus
// @name          Photoshop Dark Menus
// @description   Enables dark mode and custom colors for all menus in Adobe Photoshop.
// @version       2.0.0
// @author        Saber Naeemi
// @github        https://github.com/sabergraphics
// @twitter       https://x.com/SaberNaeemi
// @homepage      https://www.sabernaeemi.com
// @include       Photoshop.exe
// @compilerOptions -lUser32 -lGdi32 -lComctl32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Photoshop Dark Menus

This Windhawk mod enables dark menus (top-bar dropdowns and context menus) in
Adobe Photoshop on Windows, with fully customizable colors.

Everything happens inside the Photoshop process. No system colors are changed,
no other application is affected, and nothing has to be restored on exit.

## Screenshots

![Top Bar Menu Dropdown](https://raw.githubusercontent.com/sabergraphics/photoshop-dark-menus/main/images/photoshop-dark-menu-screenshot-1.png)

![Context Menu](https://raw.githubusercontent.com/sabergraphics/photoshop-dark-menus/main/images/photoshop-dark-menu-screenshot-2.png)

## How it works

Photoshop's menus look like classic unthemed Win32 menus, but they are not:
Photoshop **owner-draws every menu item itself** through `AdobeOwl.dll`, using
plain GDI, inside its own process. Instrumenting a live Photoshop shows the
menu popups going up via `TrackPopupMenuEx` and every item being painted with:

- `FillRect(hdc, &rc, (HBRUSH)(COLOR_MENU + 1))` for the item background, and
  `(HBRUSH)(COLOR_HIGHLIGHT + 1)` for the hovered item. These **system color
  pseudo-handles** are resolved inside `user32` from shared memory, so they
  bypass `GetSysColor` and `GetSysColorBrush` hooks entirely - which is why
  earlier per-process attempts appeared to do nothing.
- `GetSysColor(COLOR_MENUTEXT / COLOR_GRAYTEXT / COLOR_HIGHLIGHTTEXT)` for text.
- Its own 1px `FillRect` for separator lines.

So every pixel of a menu item is reachable from inside the process:

- `FillRect` / `PatBlt` are hooked and, while a menu is up, sys-color
  pseudo-handles are re-resolved through this mod's own color table, and 1px
  fills inside a menu DC are recolored to the separator color.
- `GetSysColor` / `GetSysColorBrush` are hooked for the menu color indices, but
  **only while a menu is open**, so Photoshop's dialogs, panels and lists keep
  the system colors.
- `MENUINFO::hbrBack` is stamped on each popup (and its submenus) at
  `WM_INITMENUPOPUP`, covering the popup background that the system paints
  around the owner-drawn items.
- The popup's non-client frame is the one part drawn kernel-side, from the 3D
  colors, where no user-mode hook can reach it - so the popup is subclassed at
  creation and the frame is repainted with the border color after it. Set
  **Border Color** empty to leave the system frame alone.

## Options

- **Menu Background Color**: Background color for all menu popups (Default: `#282828`).
- **Menu Text Color**: Text color for active items (Default: `#DCDCDC`).
- **Highlight Background Color**: Color when hovering over an item (Default: `#505050`).
- **Highlight Text Color**: Text color when hovering over an item (Default: `#FFFFFF`).
- **Separator Line Color**: Color for separator lines. Set to match the background color to hide them completely (Default: `#383838`).
- **Disabled Text Color**: Text color for disabled menu items (Default: `#808080`).
- **Border Color**: Color of the popup frame. Leave empty to keep the system frame (Default: `#282828`).
- **Debug Logging**: Diagnostic logging, off by default.

## Scope

The menu bar itself (File, Edit, Image, ...) is drawn by Photoshop's own UI
framework and already follows Photoshop's interface theme; this mod covers the
dropdown popups and context menus.

## Changelog

### 2.0.0
- Replaced the global `SetSysColors` engine with a fully process-local one.
  Nothing outside Photoshop changes any more, and with it went the registry
  backup, the `RtlExitUserProcess` teardown, crash recovery, the multi-instance
  semaphore and the two-engine setting.
- `FillRect` now re-resolves system color pseudo-handles, which is what actually
  reaches Photoshop's item backgrounds and hover highlight.
- System color overrides are scoped to open menus, so Photoshop's dialogs and
  panels keep the system colors.
- The popup frame is repainted from a subclass on the popup, with a new border
  color setting.
- Menus stamped with the background brush are restored when the mod is
  unloaded, so disabling the mod brings the normal menus back without
  restarting Photoshop.
*/
// ==/WindhawkModReadme==


// ==WindhawkModSettings==
/*
- MenuBgColor: "#282828"
  $name: "Menu Background Color"
- MenuTextColor: "#DCDCDC"
  $name: "Menu Text Color"
- HighlightBgColor: "#505050"
  $name: "Highlight Background Color"
- HighlightTextColor: "#FFFFFF"
  $name: "Highlight Text Color"
- SeparatorColor: "#383838"
  $name: "Separator Line Color"
  $description: Set this to the menu background color to hide separators.
- GrayTextColor: "#808080"
  $name: "Disabled Text Color"
- BorderColor: "#282828"
  $name: "Border Color"
  $description: >-
    Color of the popup frame, which the system draws from the 3D colors. Leave
    empty to keep the system frame.
- DebugLogging: false
  $name: "Debug Logging"
  $description: >-
    Logs menu drawing diagnostics. Leave off for normal use.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <commctrl.h>
#include <windhawk_api.h>
#include <windhawk_utils.h>
#include <wchar.h>
#include <wctype.h>
#include <atomic>
#include <mutex>
#include <unordered_set>
#include <vector>

// ---------------------------------------------------------------------------
// State
// ---------------------------------------------------------------------------

// Colors read by the in-process hooks.
std::atomic<COLORREF> g_colMenu{RGB(40, 40, 40)};
std::atomic<COLORREF> g_colText{RGB(220, 220, 220)};
std::atomic<COLORREF> g_colHighlight{RGB(80, 80, 80)};
std::atomic<COLORREF> g_colHiText{RGB(255, 255, 255)};
std::atomic<COLORREF> g_colGray{RGB(128, 128, 128)};

// Brushes are published with an atomic exchange and never deleted - see the
// note in Wh_ModUninit.
std::atomic<HBRUSH> g_hMenuBrush{nullptr};
std::atomic<HBRUSH> g_hTextBrush{nullptr};
std::atomic<HBRUSH> g_hHighlightBrush{nullptr};
std::atomic<HBRUSH> g_hHiTextBrush{nullptr};
std::atomic<HBRUSH> g_hGrayBrush{nullptr};
std::atomic<HBRUSH> g_hSeparatorBrush{nullptr};
std::atomic<HBRUSH> g_hBorderBrush{nullptr};
std::vector<HBRUSH> g_retiredBrushes;
std::mutex g_brushMutex;

std::atomic<bool> g_debugLog{false};

// Number of menu tracking calls currently on the stack. The hot GDI hooks bail
// out on a single relaxed load when no menu is up.
std::atomic<int> g_menuDepth{0};

// Set while this thread is painting on the mod's behalf, so our own drawing
// does not re-enter the hooks.
thread_local bool tl_inOurPaint = false;

// Hooks installed for the duration of a menu, on the menu's own thread.
thread_local HHOOK tl_msgHook = nullptr;
thread_local HHOOK tl_cbtHook = nullptr;
thread_local int tl_menuHookDepth = 0;

#define DBG(...) do { if (g_debugLog.load(std::memory_order_relaxed)) Wh_Log(__VA_ARGS__); } while (0)

// ---------------------------------------------------------------------------
// Settings
// ---------------------------------------------------------------------------

// Returns false for an empty string, so a setting can mean "leave it alone".
bool ParseHexColor(PCWSTR hexStr, COLORREF* pOut) {
    if (!hexStr) return false;

    const wchar_t* p = hexStr;
    while (*p == L' ') p++;
    if (*p == L'#') p++;

    size_t len = wcslen(p);
    while (len > 0 && p[len - 1] == L' ') len--;
    if (len == 0) return false;

    bool allHex = true;
    for (size_t i = 0; i < len && allHex; i++) {
        if (!iswxdigit(p[i])) allHex = false;
    }

    unsigned int r, g, b;
    if (allHex && len == 6 && swscanf_s(p, L"%02x%02x%02x", &r, &g, &b) == 3) {
        *pOut = RGB(r, g, b);
        return true;
    }
    if (allHex && len == 3 && swscanf_s(p, L"%1x%1x%1x", &r, &g, &b) == 3) {
        *pOut = RGB(r * 17, g * 17, b * 17);  // expand short hex
        return true;
    }

    Wh_Log(L"ParseHexColor: invalid format '%s', using default.", hexStr);
    return false;
}

COLORREF ParseHexColorOr(PCWSTR hexStr, COLORREF defaultColor) {
    COLORREF c;
    return ParseHexColor(hexStr, &c) ? c : defaultColor;
}

void PublishBrush(std::atomic<HBRUSH>* pSlot, HBRUSH hNew) {
    HBRUSH hOld = pSlot->exchange(hNew, std::memory_order_release);
    if (hOld) {
        std::lock_guard<std::mutex> lock(g_brushMutex);
        g_retiredBrushes.push_back(hOld);
    }
}

void PublishSolidBrush(std::atomic<HBRUSH>* pSlot, COLORREF color) {
    HBRUSH hNew = CreateSolidBrush(color);
    if (hNew) PublishBrush(pSlot, hNew);
}

void LoadSettings() {
    g_debugLog.store(Wh_GetIntSetting(L"DebugLogging") != 0, std::memory_order_relaxed);

    COLORREF colMenu  = ParseHexColorOr(WindhawkUtils::StringSetting::make(L"MenuBgColor").get(), RGB(40, 40, 40));
    COLORREF colText  = ParseHexColorOr(WindhawkUtils::StringSetting::make(L"MenuTextColor").get(), RGB(220, 220, 220));
    COLORREF colHigh  = ParseHexColorOr(WindhawkUtils::StringSetting::make(L"HighlightBgColor").get(), RGB(80, 80, 80));
    COLORREF colHiTxt = ParseHexColorOr(WindhawkUtils::StringSetting::make(L"HighlightTextColor").get(), RGB(255, 255, 255));
    COLORREF colGray  = ParseHexColorOr(WindhawkUtils::StringSetting::make(L"GrayTextColor").get(), RGB(128, 128, 128));
    COLORREF colSep   = ParseHexColorOr(WindhawkUtils::StringSetting::make(L"SeparatorColor").get(), RGB(56, 56, 56));

    g_colMenu.store(colMenu, std::memory_order_relaxed);
    g_colText.store(colText, std::memory_order_relaxed);
    g_colHighlight.store(colHigh, std::memory_order_relaxed);
    g_colHiText.store(colHiTxt, std::memory_order_relaxed);
    g_colGray.store(colGray, std::memory_order_relaxed);

    PublishSolidBrush(&g_hMenuBrush, colMenu);
    PublishSolidBrush(&g_hTextBrush, colText);
    PublishSolidBrush(&g_hHighlightBrush, colHigh);
    PublishSolidBrush(&g_hHiTextBrush, colHiTxt);
    PublishSolidBrush(&g_hGrayBrush, colGray);
    PublishSolidBrush(&g_hSeparatorBrush, colSep);

    // Empty means "keep the system frame".
    COLORREF colBorder;
    if (ParseHexColor(WindhawkUtils::StringSetting::make(L"BorderColor").get(), &colBorder)) {
        PublishSolidBrush(&g_hBorderBrush, colBorder);
    } else {
        PublishBrush(&g_hBorderBrush, nullptr);
    }
}

// ---------------------------------------------------------------------------
// Menu scoping
// ---------------------------------------------------------------------------

inline bool MenuIsOpen() {
    return g_menuDepth.load(std::memory_order_relaxed) > 0;
}

// Photoshop paints menu items into per-item memory DCs while the popup is being
// laid out, and directly onto the popup's own window DC when an item is
// re-drawn on hover. Both are only reached while a menu is up, which the caller
// has already established with MenuIsOpen().
bool IsMenuContext(HDC hdc) {
    HWND hWnd = WindowFromDC(hdc);
    if (hWnd) {
        WCHAR cls[16];
        return GetClassNameW(hWnd, cls, ARRAYSIZE(cls)) && wcscmp(cls, L"#32768") == 0;
    }
    return GetObjectType(hdc) == OBJ_MEMDC;
}

// Our color for a system color index, or nullptr / false if we don't override it.
//
// The frame's 3D colors (COLOR_BTNSHADOW and friends) are deliberately not
// listed: the popup frame is drawn kernel-side and never reaches a user-mode
// GDI call, so overriding them here would change nothing - it is repainted in
// the popup's subclass instead.
HBRUSH BrushForSysColor(int nIndex) {
    switch (nIndex) {
        case COLOR_MENU:
        case COLOR_MENUBAR:       return g_hMenuBrush.load(std::memory_order_acquire);
        case COLOR_MENUTEXT:      return g_hTextBrush.load(std::memory_order_acquire);
        case COLOR_HIGHLIGHT:     return g_hHighlightBrush.load(std::memory_order_acquire);
        case COLOR_HIGHLIGHTTEXT: return g_hHiTextBrush.load(std::memory_order_acquire);
        case COLOR_GRAYTEXT:      return g_hGrayBrush.load(std::memory_order_acquire);
    }
    return nullptr;
}

bool ColorForSysColor(int nIndex, COLORREF* pOut) {
    switch (nIndex) {
        case COLOR_MENU:
        case COLOR_MENUBAR:       *pOut = g_colMenu.load(std::memory_order_relaxed); return true;
        case COLOR_MENUTEXT:      *pOut = g_colText.load(std::memory_order_relaxed); return true;
        case COLOR_HIGHLIGHT:     *pOut = g_colHighlight.load(std::memory_order_relaxed); return true;
        case COLOR_HIGHLIGHTTEXT: *pOut = g_colHiText.load(std::memory_order_relaxed); return true;
        case COLOR_GRAYTEXT:      *pOut = g_colGray.load(std::memory_order_relaxed); return true;
    }
    return false;
}

// Separator geometry scales with DPI; 1px at 100% is often 2-3px at 175%.
bool IsSeparatorGeometry(HDC hdc, int w, int h) {
    int dpi = GetDeviceCaps(hdc, LOGPIXELSY);
    if (dpi <= 0) dpi = 96;
    return h >= 1 && h <= MulDiv(3, dpi, 96) && w > MulDiv(20, dpi, 96);
}

// A background brush set on a menu is state on the menu object, not on a hook,
// so it outlives the mod: without clearing it, disabling the mod leaves every
// area the system fills from hbrBack - gaps, margins, separator bands - dark
// until Photoshop restarts. Remember what we stamped so it can be undone.
//
// Photoshop keeps a bounded set of menus, but they can be rebuilt over a long
// session, so the set is capped rather than allowed to grow without limit.
constexpr size_t kMaxTrackedMenus = 1024;
std::unordered_set<HMENU> g_stampedMenus;
std::mutex g_stampedMenusMutex;

void ApplyMenuBackground(HMENU hMenu) {
    HBRUSH hBrush = g_hMenuBrush.load(std::memory_order_acquire);
    if (!hMenu || !hBrush) return;

    MENUINFO mi = { sizeof(mi) };
    mi.fMask = MIM_BACKGROUND | MIM_APPLYTOSUBMENUS;
    mi.hbrBack = hBrush;
    if (!SetMenuInfo(hMenu, &mi)) return;

    std::lock_guard<std::mutex> lock(g_stampedMenusMutex);
    if (g_stampedMenus.size() < kMaxTrackedMenus) {
        g_stampedMenus.insert(hMenu);
    }
}

// MIM_APPLYTOSUBMENUS propagates the cleared brush down each menu tree, so
// submenus that were stamped but never opened are covered too.
void RestoreStampedMenus() {
    std::vector<HMENU> menus;
    {
        std::lock_guard<std::mutex> lock(g_stampedMenusMutex);
        menus.assign(g_stampedMenus.begin(), g_stampedMenus.end());
        g_stampedMenus.clear();
    }

    // SetMenuInfo can reach a menu owned by another thread, so it is called
    // with the lock released.
    for (HMENU hMenu : menus) {
        MENUINFO mi = { sizeof(mi) };
        mi.fMask = MIM_BACKGROUND | MIM_APPLYTOSUBMENUS;
        mi.hbrBack = nullptr;
        SetMenuInfo(hMenu, &mi);
    }
}

// Width of the non-client frame, i.e. how much of the window rect the system
// paints from the 3D colors.
int GetPopupFrameWidth(HWND hWnd, const RECT& windowRect) {
    RECT client;
    POINT origin = { 0, 0 };
    if (!GetClientRect(hWnd, &client) || !ClientToScreen(hWnd, &origin)) return 1;

    int left = origin.x - windowRect.left;
    int top = origin.y - windowRect.top;
    int right = windowRect.right - (origin.x + (client.right - client.left));
    int bottom = windowRect.bottom - (origin.y + (client.bottom - client.top));

    int frame = left;
    if (top > frame) frame = top;
    if (right > frame) frame = right;
    if (bottom > frame) frame = bottom;

    if (frame < 1) frame = 1;
    if (frame > 4) frame = 4;
    return frame;
}

// The popup's non-client frame is drawn by the system from COLOR_3DDKSHADOW /
// COLOR_BTNHIGHLIGHT, which no process-local hook can change - so repaint it
// once the system is done with it.
//
// hdcTarget is the DC the system rendered the popup into. Menus are drawn
// offscreen and blitted (WM_PRINT / WM_NCUAHDRAWFRAME carry that DC in wParam),
// so painting on the window DC instead would just be overwritten by the blit.
void PaintPopupBorder(HWND hWnd, HDC hdcTarget) {
    HBRUSH hBorder = g_hBorderBrush.load(std::memory_order_acquire);
    if (!hBorder) return;

    RECT windowRect;
    if (!GetWindowRect(hWnd, &windowRect)) return;

    int frame = GetPopupFrameWidth(hWnd, windowRect);

    RECT rc = windowRect;
    OffsetRect(&rc, -rc.left, -rc.top);
    if (rc.right <= 0 || rc.bottom <= 0) return;

    HDC hdc = hdcTarget ? hdcTarget : GetWindowDC(hWnd);
    if (!hdc) return;

    tl_inOurPaint = true;
    for (int i = 0; i < frame && rc.right > rc.left && rc.bottom > rc.top; i++) {
        FrameRect(hdc, &rc, hBorder);
        InflateRect(&rc, -1, -1);
    }
    tl_inOurPaint = false;

    if (!hdcTarget) ReleaseDC(hWnd, hdc);
}

// Undocumented: sent to a window to paint its frame into the DC in wParam.
constexpr UINT WM_NCUAHDRAWFRAME = 0x00AF;

bool IsMenuPopupWindow(HWND hWnd) {
    WCHAR cls[16];
    return GetClassNameW(hWnd, cls, ARRAYSIZE(cls)) && wcscmp(cls, L"#32768") == 0;
}

// The system reuses menu popup windows, so a subclassed popup can still be
// alive when the mod unloads - and its window procedure would then point into
// an unloaded DLL. Track them and remove each subclass at uninit.
std::unordered_set<HWND> g_subclassedPopups;
std::mutex g_subclassedPopupsMutex;

// Subclassing the popup rather than watching messages from a hook: WM_PAINT is
// posted, not sent, so a WH_CALLWNDPROCRET hook never sees it, and a popup that
// repaints that way would keep the system frame.
LRESULT CALLBACK MenuPopupSubclassProc(HWND hWnd,
                                       UINT uMsg,
                                       WPARAM wParam,
                                       LPARAM lParam,
                                       DWORD_PTR dwRefData) {
    if (uMsg == WM_NCDESTROY) {
        std::lock_guard<std::mutex> lock(g_subclassedPopupsMutex);
        g_subclassedPopups.erase(hWnd);
    }

    LRESULT result = DefSubclassProc(hWnd, uMsg, wParam, lParam);

    switch (uMsg) {
        // wParam carries the DC the popup was rendered into.
        case WM_PRINT:
        case WM_NCUAHDRAWFRAME:
            PaintPopupBorder(hWnd, (HDC)wParam);
            break;

        // Painted on the window's own DC. WM_ERASEBKGND's DC is client
        // relative, so the frame goes on the window DC in every one of these.
        case WM_PAINT:
        case WM_NCPAINT:
        case WM_ERASEBKGND:
            PaintPopupBorder(hWnd, nullptr);
            break;
    }

    return result;
}

// Catches the popup at creation, which is the only reliable moment: it exists
// on this thread and has not painted yet.
LRESULT CALLBACK CbtProcHook(int code, WPARAM wParam, LPARAM lParam) {
    if (code == HCBT_CREATEWND) {
        HWND hWnd = (HWND)wParam;
        if (IsMenuPopupWindow(hWnd) && g_hBorderBrush.load(std::memory_order_acquire)) {
            if (WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, MenuPopupSubclassProc, 0)) {
                std::lock_guard<std::mutex> lock(g_subclassedPopupsMutex);
                g_subclassedPopups.insert(hWnd);
            } else {
                DBG(L"Failed to subclass menu popup %p; it keeps the system frame.", hWnd);
            }
        }
    }
    return CallNextHookEx(nullptr, code, wParam, lParam);
}

// Runs on the menu's own thread, after the window has handled the message.
LRESULT CALLBACK CallWndRetProcHook(int code, WPARAM wParam, LPARAM lParam) {
    if (code == HC_ACTION && lParam) {
        auto* ret = (CWPRETSTRUCT*)lParam;
        if (ret->message == WM_INITMENUPOPUP) {
            ApplyMenuBackground((HMENU)ret->wParam);
        }
    }
    return CallNextHookEx(nullptr, code, wParam, lParam);
}

// Menus are modal, so the hook only exists while one is up. Nested tracking
// calls (submenus opened via TrackPopupMenu) share the outermost hook.
void EnterMenu(HMENU hMenu) {
    g_menuDepth.fetch_add(1, std::memory_order_relaxed);
    ApplyMenuBackground(hMenu);

    if (tl_menuHookDepth++ == 0) {
        DWORD tid = GetCurrentThreadId();
        tl_msgHook = SetWindowsHookExW(WH_CALLWNDPROCRET, CallWndRetProcHook, nullptr, tid);
        tl_cbtHook = SetWindowsHookExW(WH_CBT, CbtProcHook, nullptr, tid);
        if (!tl_msgHook || !tl_cbtHook) {
            Wh_Log(L"SetWindowsHookEx failed (%u); popup border and submenu "
                   L"backgrounds fall back to the system's.", GetLastError());
        }
    }
}

void LeaveMenu() {
    if (--tl_menuHookDepth <= 0) {
        tl_menuHookDepth = 0;
        if (tl_msgHook) {
            UnhookWindowsHookEx(tl_msgHook);
            tl_msgHook = nullptr;
        }
        if (tl_cbtHook) {
            UnhookWindowsHookEx(tl_cbtHook);
            tl_cbtHook = nullptr;
        }
    }

    if (g_menuDepth.fetch_sub(1, std::memory_order_relaxed) <= 0) {
        g_menuDepth.store(0, std::memory_order_relaxed);
    }
}

// ---------------------------------------------------------------------------
// Hooks
// ---------------------------------------------------------------------------

decltype(&TrackPopupMenu) TrackPopupMenu_Original;
BOOL WINAPI TrackPopupMenu_Hook(HMENU hMenu, UINT uFlags, int x, int y,
                                int nReserved, HWND hWnd, const RECT* prcRect) {
    EnterMenu(hMenu);
    BOOL bRes = TrackPopupMenu_Original(hMenu, uFlags, x, y, nReserved, hWnd, prcRect);
    LeaveMenu();
    return bRes;
}

decltype(&TrackPopupMenuEx) TrackPopupMenuEx_Original;
BOOL WINAPI TrackPopupMenuEx_Hook(HMENU hMenu, UINT uFlags, int x, int y,
                                  HWND hWnd, LPTPMPARAMS lptpm) {
    EnterMenu(hMenu);
    BOOL bRes = TrackPopupMenuEx_Original(hMenu, uFlags, x, y, hWnd, lptpm);
    LeaveMenu();
    return bRes;
}

decltype(&GetSysColor) GetSysColor_Original;
DWORD WINAPI GetSysColor_Hook(int nIndex) {
    COLORREF color;
    if (MenuIsOpen() && !tl_inOurPaint && ColorForSysColor(nIndex, &color)) {
        return color;
    }
    return GetSysColor_Original(nIndex);
}

decltype(&GetSysColorBrush) GetSysColorBrush_Original;
HBRUSH WINAPI GetSysColorBrush_Hook(int nIndex) {
    if (MenuIsOpen() && !tl_inOurPaint) {
        HBRUSH hBrush = BrushForSysColor(nIndex);
        // Callers may cache this handle for the process lifetime, so the
        // brushes it hands out can never be deleted. See Wh_ModUninit.
        if (hBrush) return hBrush;
    }
    return GetSysColorBrush_Original(nIndex);
}

decltype(&FillRect) FillRect_Original;
int WINAPI FillRect_Hook(HDC hdc, const RECT* lprc, HBRUSH hbr) {
    if (!MenuIsOpen() || tl_inOurPaint || !lprc) {
        return FillRect_Original(hdc, lprc, hbr);
    }

    // Photoshop fills item backgrounds with a system color pseudo-handle -
    // (HBRUSH)(COLOR_MENU + 1) and (HBRUSH)(COLOR_HIGHLIGHT + 1). user32
    // resolves those from shared memory, so the GetSysColorBrush hook never
    // sees them; re-resolve them here instead.
    int w = lprc->right - lprc->left;
    int h = lprc->bottom - lprc->top;

    ULONG_PTR sysIndex = (ULONG_PTR)hbr - 1;
    if (sysIndex <= (ULONG_PTR)COLOR_MENUBAR) {
        HBRUSH ours = BrushForSysColor((int)sysIndex);
        if (ours && IsMenuContext(hdc)) {
            DBG(L"FillRect: system color %d -> mod color, %dx%d", (int)sysIndex, w, h);
            return FillRect_Original(hdc, lprc, ours);
        }
        return FillRect_Original(hdc, lprc, hbr);
    }

    HBRUSH hSep = g_hSeparatorBrush.load(std::memory_order_acquire);
    if (hSep && IsSeparatorGeometry(hdc, w, h) && IsMenuContext(hdc)) {
        DBG(L"FillRect: separator %dx%d recolored", w, h);
        return FillRect_Original(hdc, lprc, hSep);
    }

    return FillRect_Original(hdc, lprc, hbr);
}

decltype(&PatBlt) PatBlt_Original;
BOOL WINAPI PatBlt_Hook(HDC hdc, int x, int y, int w, int h, DWORD rop) {
    if (!MenuIsOpen() || tl_inOurPaint || rop != PATCOPY) {
        return PatBlt_Original(hdc, x, y, w, h, rop);
    }

    HBRUSH hSep = g_hSeparatorBrush.load(std::memory_order_acquire);
    if (hSep && IsSeparatorGeometry(hdc, w, h) && IsMenuContext(hdc)) {
        HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hSep);
        BOOL bRes = PatBlt_Original(hdc, x, y, w, h, rop);
        SelectObject(hdc, hOldBrush);
        return bRes;
    }

    return PatBlt_Original(hdc, x, y, w, h, rop);
}

// ---------------------------------------------------------------------------
// Windhawk events
// ---------------------------------------------------------------------------

void Wh_ModSettingsChanged() {
    LoadSettings();
}

BOOL Wh_ModInit() {
    LoadSettings();

    if (!WindhawkUtils::SetFunctionHook(TrackPopupMenu, TrackPopupMenu_Hook, &TrackPopupMenu_Original)) Wh_Log(L"Failed to hook TrackPopupMenu");
    if (!WindhawkUtils::SetFunctionHook(TrackPopupMenuEx, TrackPopupMenuEx_Hook, &TrackPopupMenuEx_Original)) Wh_Log(L"Failed to hook TrackPopupMenuEx");
    if (!WindhawkUtils::SetFunctionHook(GetSysColor, GetSysColor_Hook, &GetSysColor_Original)) Wh_Log(L"Failed to hook GetSysColor");
    if (!WindhawkUtils::SetFunctionHook(GetSysColorBrush, GetSysColorBrush_Hook, &GetSysColorBrush_Original)) Wh_Log(L"Failed to hook GetSysColorBrush");
    if (!WindhawkUtils::SetFunctionHook(FillRect, FillRect_Hook, &FillRect_Original)) Wh_Log(L"Failed to hook FillRect");
    if (!WindhawkUtils::SetFunctionHook(PatBlt, PatBlt_Hook, &PatBlt_Original)) Wh_Log(L"Failed to hook PatBlt");

    return TRUE;
}

void Wh_ModUninit() {
    if (tl_msgHook) {
        UnhookWindowsHookEx(tl_msgHook);
        tl_msgHook = nullptr;
    }
    if (tl_cbtHook) {
        UnhookWindowsHookEx(tl_cbtHook);
        tl_cbtHook = nullptr;
    }
    // Not WindhawkUtils::RemoveAllWindowSubclasses(), which needs Windhawk 1.8.
    {
        std::vector<HWND> popups;
        {
            std::lock_guard<std::mutex> lock(g_subclassedPopupsMutex);
            popups.assign(g_subclassedPopups.begin(), g_subclassedPopups.end());
            g_subclassedPopups.clear();
        }
        for (HWND hWnd : popups) {
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(hWnd, MenuPopupSubclassProc);
        }
    }

    // Undo the background brush stamped on Photoshop's menus, so disabling the
    // mod restores the normal menus without restarting the application.
    RestoreStampedMenus();

    // The brushes are deliberately NOT deleted.
    //
    // Two paths hand these handles to Photoshop and neither can be reclaimed:
    // MENUINFO::hbrBack on menus that outlive the mod, and GetSysColorBrush
    // return values, which callers are entitled to cache for the process
    // lifetime. Deleting them means Photoshop eventually paints with a freed -
    // possibly recycled - GDI handle. A handful of brushes for the remaining
    // lifetime of the process is a trivial cost next to a crash in the host.
    {
        std::lock_guard<std::mutex> lock(g_brushMutex);
        g_retiredBrushes.clear();
    }
}
