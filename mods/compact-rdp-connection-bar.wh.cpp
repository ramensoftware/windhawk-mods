// ==WindhawkMod==
// @id              compact-rdp-connection-bar
// @name            Compact RDP connection bar
// @description     Shrinks the top connection bar, optionally hides pin/quality buttons, keeps drag working
// @version         2.9.3.2
// @author          hsb5
// @github          https://github.com/hsb5
// @include         mstsc.exe
// @compilerOptions -luser32 -lgdi32 -lcomctl32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Compact RDP connection bar

Shrinks the top connection bar of Remote Desktop Connection (Ctrl+Alt+Home).

The bar normally spans a large fixed width: a small button group on the left,
the hostname in the middle, the window buttons on the right. This mod keeps the
hostname zone, keeps the button groups, and drops the empty space between them.

Works with any number of toolbars. A toolbar whose buttons are all hidden needs
no width and is simply not given any, which is what happens once the pin and
quality buttons are hidden.

The right toolbar carries the window buttons - minimize, restore and close.
Each can be hidden separately; hide all three and the bar shrinks to the
hostname alone.

## Screenshots

| Before | After | After2 | After3 | After4 |
|--------|-------|--------|-------|-------|
| ![Original wide connection bar](https://raw.githubusercontent.com/hsb5/screenshots/main/Before.png) | ![Compact connection bar with centered hostname](https://raw.githubusercontent.com/hsb5/screenshots/main/After.png) | ![Compact connection bar 2 with centered hostname](https://raw.githubusercontent.com/hsb5/screenshots/main/After2.png) | ![Compact connection bar 3 with centered hostname](https://raw.githubusercontent.com/hsb5/screenshots/main/After3.png) | ![Compact connection bar 4 with centered hostname](https://raw.githubusercontent.com/hsb5/screenshots/main/After4.png) |

## Settings

- **Text area width** - target width of the hostname zone, scaled for DPI.
  Automatically raised to fit the hostname, so it is never clipped.
- **Hide pin / quality button** - hides the pin and quality indicator
  buttons from the left toolbar.
- **Hide minimize / restore / close** - hide the window buttons on the
  right. Hiding the close button removes the only visible way to disconnect;
  the session can still be closed from the taskbar icon.
- **Debug layout** - dump the bar, its children and every toolbar button once.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- gapWidth: 100
  $name: Text area width (pixels)
  $description: Target width of the hostname zone, scaled for DPI
- hidePinButton: false
  $name: Hide pin button
  $description: Hides the pin (keep on top) button
- hideQualityButton: false
  $name: Hide quality button
  $description: Hides the connection quality indicator
- hideMinimizeButton: false
  $name: Hide minimize button
  $description: Hides the minimize button on the right (bmp 0)
- hideRestoreButton: false
  $name: Hide restore button
  $description: Hides the restore-to-window button (bmp 1)
- hideCloseButton: false
  $name: Hide close button
  $description: Hides the close button (bmp 2), the only visible way to disconnect
- debugLayout: false
  $name: Debug layout
  $description: Write the bar/toolbar/button structure to the Windhawk log
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <commctrl.h>
#include <windhawk_utils.h>

#define MAX_TB      8
#define MAX_HIDDEN 16
#define EDGE_PAD    8   // margin mstsc leaves at both ends of the bar, in 96-DPI px
#define PIN_ID      4   // command id of the pin button in mstsc
#define QUALITY_ID  6   // command id of the quality indicator in mstsc

// ---------------------------------------------------------------- state

struct TbInfo {
    HWND hwnd;
    bool haveOrig;
    int  ox;     // original x, bar client coords
    int  ow;     // original width
    int  x;      // target x
    int  w;      // target width
};

static HWND   g_bbar = NULL;
static TbInfo g_tb[MAX_TB];
static int    g_tbCount = 0;

static int  g_gapWidth    = 100;
static bool g_hidePin     = false;
static bool g_hideQuality = false;
static bool g_hideMin     = false;
static bool g_hideRestore = false;
static bool g_hideClose   = false;
static bool g_debug       = false;

static RECT g_origBar;
static bool g_haveOrigBar = false;

static int  g_targetW = 0;
static int  g_targetX = 0;
static bool g_active  = false;

static RECT g_textRect;          // hostname zone, bar client coords
static int  g_textLimitX = 0;    // text may never go past this x (right group)
static int  g_textNeed   = 0;    // widest hostname measured while painting
static int  g_dpi        = 96;   // DPI of the bar's monitor, set by ComputeTargets
static bool g_stale      = false;

// Painting happens on the bar's own thread, but both DrawText hooks are called
// from every thread in the process. Without thread_local a DrawText on another
// thread during a paint would be moved into the hostname zone too.
static thread_local bool t_painting = false;
static bool g_computing = false;  // re-entrancy guard
static bool g_dumped    = false;  // one-shot structure dump

// Buttons we hid, remembered by command id so they can be restored.
struct HiddenButton { HWND tb; int id; };
static HiddenButton g_hidden[MAX_HIDDEN];
static int g_hiddenCount = 0;

typedef int  (WINAPI* DrawTextW_t)(HDC, LPCWSTR, int, LPRECT, UINT);
typedef int  (WINAPI* DrawTextExW_t)(HDC, LPWSTR, int, LPRECT, UINT, LPDRAWTEXTPARAMS);
typedef HWND (WINAPI* CreateWindowExW_t)(DWORD, LPCWSTR, LPCWSTR, DWORD, int, int, int, int,
                                         HWND, HMENU, HINSTANCE, LPVOID);

static DrawTextW_t       DrawTextW_Original;
static DrawTextExW_t     DrawTextExW_Original;
static CreateWindowExW_t CreateWindowExW_Original;

static LRESULT CALLBACK BarProc(HWND, UINT, WPARAM, LPARAM, DWORD_PTR);
static LRESULT CALLBACK TbProc(HWND, UINT, WPARAM, LPARAM, DWORD_PTR);

// ---------------------------------------------------------------- bar enumeration

// The bar can be either a top-level window or a child of the full-screen
// session window depending on the mstsc mode. EnumWindows finds it either way.
using BarFn = void (*)(HWND);

static BOOL CALLBACK VisitIfBar(HWND hwnd, LPARAM lParam) {
    WCHAR cls[32];
    if (GetClassNameW(hwnd, cls, ARRAYSIZE(cls)) &&
        lstrcmpW(cls, L"BBarWindowClass") == 0) {
        ((BarFn)lParam)(hwnd);
    }
    return TRUE;
}

static BOOL CALLBACK VisitTopLevel(HWND hwnd, LPARAM lParam) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid == GetCurrentProcessId()) {
        VisitIfBar(hwnd, lParam);                    // a top-level bar
        EnumChildWindows(hwnd, VisitIfBar, lParam);  // a bar at any depth below it
    }
    return TRUE;
}

static void ForEachBar(BarFn fn) {
    EnumWindows(VisitTopLevel, (LPARAM)fn);
}

// Window rect in the coordinates SetWindowPos expects: the parent's client
// area for a child window, the screen for a top-level one. Mapping changes
// nothing for top-level windows.
static RECT GetRectInParent(HWND hwnd) {
    RECT rc;
    GetWindowRect(hwnd, &rc);
    MapWindowPoints(HWND_DESKTOP, GetAncestor(hwnd, GA_PARENT), (POINT*)&rc, 2);
    return rc;
}

// ---------------------------------------------------------------- helpers

// GetDpiForWindow gives the DPI of the monitor the window is on, which is what
// the bar needs on a mixed-DPI setup. It only exists on Windows 10 1607 and
// later, so fall back to the system DPI when it cannot be resolved.
typedef UINT (WINAPI* GetDpiForWindow_t)(HWND);

static UINT QueryWindowDpi(HWND h) {
    static GetDpiForWindow_t pGetDpiForWindow = NULL;
    static bool resolved = false;
    if (!resolved) {
        resolved = true;
        HMODULE user32 = GetModuleHandleW(L"user32.dll");
        if (user32)
            pGetDpiForWindow =
                (GetDpiForWindow_t)GetProcAddress(user32, "GetDpiForWindow");
    }

    if (pGetDpiForWindow) {
        UINT dpi = pGetDpiForWindow(h);
        if (dpi) return dpi;
    }

    HDC dc = GetDC(h);
    UINT dpi = dc ? (UINT)GetDeviceCaps(dc, LOGPIXELSX) : 0;
    if (dc) ReleaseDC(h, dc);
    return dpi ? dpi : 96;
}

static int DpiOf(HWND h) {
    UINT dpi = QueryWindowDpi(h);
    return dpi > 0 ? (int)dpi : 96;
}

static int MeasureText(HDC dc, LPCWSTR text, int cch) {
    if (!dc || !text) return 0;
    int len = cch < 0 ? lstrlenW(text) : cch;
    if (len <= 0) return 0;
    SIZE sz;
    sz.cx = 0;
    sz.cy = 0;
    if (!GetTextExtentPoint32W(dc, text, len, &sz)) return 0;
    return (int)sz.cx;
}

// Width of the window text, as a first guess for the hostname zone.
static int HostTextWidth(HWND hwnd) {
    wchar_t text[512];
    text[0] = L'\0';
    int len = GetWindowTextW(hwnd, text, 512);
    if (len <= 0) return 0;

    HDC dc = GetDC(hwnd);
    if (!dc) return 0;

    HGDIOBJ old = NULL;
    HFONT f = (HFONT)SendMessageW(hwnd, WM_GETFONT, 0, 0);
    if (f) old = SelectObject(dc, f);

    int w = MeasureText(dc, text, len);

    if (old) SelectObject(dc, old);
    ReleaseDC(hwnd, dc);
    return w;
}

static bool ShouldHideId(int id) {
    return (g_hidePin && id == PIN_ID) || (g_hideQuality && id == QUALITY_ID);
}

// Window buttons on the right toolbar are matched by bitmap index (0 minimize,
// 1 restore, 2 close), because mstsc gives them plain command ids that repeat
// the ids used by the left toolbar. Their ids happen to equal their bitmap
// index, which is what tells them apart from the pin/quality group; there the
// two never coincide.
static bool ShouldHideWindowButton(int id, int bmp) {
    if (id != bmp) return false;
    return (g_hideMin && bmp == 0) || (g_hideRestore && bmp == 1) ||
           (g_hideClose && bmp == 2);
}

// Hidden state as reported by the toolbar itself.
static bool IsButtonHidden(HWND tb, int id) {
    if (!tb || !IsWindow(tb)) return false;
    return SendMessageW(tb, TB_ISBUTTONHIDDEN, (WPARAM)id, 0) != FALSE;
}

// ---------------------------------------------------------------- diagnostics

static void DumpTb(HWND tb) {
    int count = (int)SendMessageW(tb, TB_BUTTONCOUNT, 0, 0);
    RECT wr;
    GetWindowRect(tb, &wr);
    DWORD style = (DWORD)GetWindowLongW(tb, GWL_STYLE);

    Wh_Log(L"  ToolbarWindow32 %p rect=%d,%d %dx%d style=0x%08X flat=%d buttons=%d",
           tb, (int)wr.left, (int)wr.top, (int)(wr.right - wr.left),
           (int)(wr.bottom - wr.top), (unsigned)style,
           (int)((style & TBSTYLE_FLAT) != 0), count);

    for (int i = 0; i < count; i++) {
        TBBUTTON b;
        ZeroMemory(&b, sizeof(b));
        if (!SendMessageW(tb, TB_GETBUTTON, i, (LPARAM)&b)) {
            Wh_Log(L"    btn[%d] TB_GETBUTTON failed", i);
            continue;
        }
        RECT ir;
        ZeroMemory(&ir, sizeof(ir));
        SendMessageW(tb, TB_GETITEMRECT, i, (LPARAM)&ir);

        wchar_t txt[128];
        txt[0] = L'\0';
        SendMessageW(tb, TB_GETBUTTONTEXTW, b.idCommand, (LPARAM)txt);

        Wh_Log(L"    btn[%d] id=%d bmp=%d state=0x%02X style=0x%02X rect=%d..%d w=%d hidden=%d txt='%s'",
               i, b.idCommand, b.iBitmap, b.fsState, b.fsStyle,
               (int)ir.left, (int)ir.right, (int)(ir.right - ir.left),
               (int)IsButtonHidden(tb, b.idCommand), txt);
    }
}

static void DumpAll(HWND bbar) {
    wchar_t barText[256];
    barText[0] = L'\0';
    GetWindowTextW(bbar, barText, 256);

    RECT r;
    GetWindowRect(bbar, &r);
    Wh_Log(L"== dump BBar %p rect=%d,%d %dx%d text='%s'",
           bbar, (int)r.left, (int)r.top, (int)(r.right - r.left),
           (int)(r.bottom - r.top), barText);

    HWND child = NULL;
    wchar_t cls[128];
    int idx = 0;
    while ((child = FindWindowExW(bbar, child, NULL, NULL)) != NULL) {
        GetClassNameW(child, cls, 128);
        GetWindowRect(child, &r);
        Wh_Log(L"  [%d] %s %p rect=%d,%d %dx%d vis=%d",
               idx++, cls, child, (int)r.left, (int)r.top,
               (int)(r.right - r.left), (int)(r.bottom - r.top),
               (int)(IsWindowVisible(child) != FALSE));
        if (lstrcmpW(cls, L"ToolbarWindow32") == 0) DumpTb(child);
    }
    Wh_Log(L"== dump end children=%d", idx);
}

// ---------------------------------------------------------------- toolbars

// Collects ToolbarWindow32 children, sorted by original left edge.
static int CollectToolbars(HWND bbar) {
    HWND found[MAX_TB];
    int n = 0;

    HWND child = NULL;
    wchar_t cls[64];
    while ((child = FindWindowExW(bbar, child, NULL, NULL)) != NULL) {
        if (!GetClassNameW(child, cls, 64)) continue;
        if (lstrcmpW(cls, L"ToolbarWindow32") != 0) continue;
        if (n < MAX_TB) found[n++] = child;
    }

    for (int i = 1; i < n; i++) {
        HWND key = found[i];
        RECT kr;
        GetWindowRect(key, &kr);
        int j = i - 1;
        while (j >= 0) {
            RECT r;
            GetWindowRect(found[j], &r);
            if (r.left <= kr.left) break;
            found[j + 1] = found[j];
            j--;
        }
        found[j + 1] = key;
    }

    POINT origin;
    origin.x = 0;
    origin.y = 0;
    ClientToScreen(bbar, &origin);

    g_tbCount = 0;
    for (int i = 0; i < n; i++) {
        TbInfo* t = &g_tb[g_tbCount++];
        t->hwnd = found[i];
        t->x = 0;
        t->w = 0;
        if (!t->haveOrig) {
            RECT r;
            GetWindowRect(found[i], &r);
            t->ox = (int)r.left - origin.x;
            t->ow = (int)(r.right - r.left);
            t->haveOrig = true;
        }
    }
    return g_tbCount;
}

// Width the toolbar needs for the buttons that are not meant to be hidden.
// Summing the item rects instead of taking the right edge keeps this correct
// while hidden buttons still leave gaps in the original layout.
static int NeededWidth(HWND tb) {
    if (!tb || !IsWindow(tb)) return 0;

    int count = (int)SendMessageW(tb, TB_BUTTONCOUNT, 0, 0);
    int width = 0;

    for (int i = 0; i < count; i++) {
        TBBUTTON b;
        ZeroMemory(&b, sizeof(b));
        if (!SendMessageW(tb, TB_GETBUTTON, i, (LPARAM)&b)) continue;

        // A button we asked to hide, and the toolbar really hid, needs no room.
        if ((ShouldHideId(b.idCommand) ||
             ShouldHideWindowButton(b.idCommand, b.iBitmap)) &&
            IsButtonHidden(tb, b.idCommand)) continue;

        RECT ir;
        ZeroMemory(&ir, sizeof(ir));
        if (!SendMessageW(tb, TB_GETITEMRECT, i, (LPARAM)&ir)) continue;
        width += (int)(ir.right - ir.left);
    }
    return width;
}

// ---------------------------------------------------------------- buttons

// TB_HIDEBUTTON takes the button's command id, not its index. Passing the
// index silently matches nothing, which is why hiding used to do nothing.
static void HideMatching(HWND tb) {
    if (!tb || !IsWindow(tb)) return;

    int count = (int)SendMessageW(tb, TB_BUTTONCOUNT, 0, 0);
    bool changed = false;

    for (int i = 0; i < count; i++) {
        TBBUTTON b;
        ZeroMemory(&b, sizeof(b));
        if (!SendMessageW(tb, TB_GETBUTTON, i, (LPARAM)&b)) continue;
        if (!ShouldHideId(b.idCommand) &&
            !ShouldHideWindowButton(b.idCommand, b.iBitmap)) continue;

        if (!IsButtonHidden(tb, b.idCommand)) {
            SendMessageW(tb, TB_HIDEBUTTON, (WPARAM)b.idCommand, MAKELPARAM(TRUE, 0));
            changed = true;
            if (g_debug)
                Wh_Log(L"hide btn[%d] id=%d bmp=%d on %p -> now hidden=%d",
                       i, b.idCommand, b.iBitmap, tb,
                       (int)IsButtonHidden(tb, b.idCommand));
        }

        bool known = false;
        for (int k = 0; k < g_hiddenCount; k++)
            if (g_hidden[k].tb == tb && g_hidden[k].id == b.idCommand) known = true;

        if (!known && g_hiddenCount < MAX_HIDDEN) {
            g_hidden[g_hiddenCount].tb = tb;
            g_hidden[g_hiddenCount].id = b.idCommand;
            g_hiddenCount++;
        }
    }

    if (changed) SendMessageW(tb, TB_AUTOSIZE, 0, 0);
}

static void RestoreButtons() {
    for (int i = 0; i < g_hiddenCount; i++) {
        HWND tb = g_hidden[i].tb;
        if (!tb || !IsWindow(tb)) continue;
        SendMessageW(tb, TB_HIDEBUTTON, (WPARAM)g_hidden[i].id, MAKELPARAM(FALSE, 0));
        SendMessageW(tb, TB_AUTOSIZE, 0, 0);
    }
    g_hiddenCount = 0;
}

// ---------------------------------------------------------------- layout

static void SubclassToolbar(HWND tb) {
    WindhawkUtils::SetWindowSubclassFromAnyThread(tb, TbProc, 0);
}

// True when the current geometry no longer matches what the buttons need.
static bool LayoutStale(HWND hwnd) {
    if (g_stale) return true;

    RECT bar;
    GetWindowRect(hwnd, &bar);
    if (g_targetW > 0 && (int)(bar.right - bar.left) != g_targetW) return true;

    for (int i = 0; i < g_tbCount; i++) {
        if (!IsWindow(g_tb[i].hwnd)) continue;
        if (NeededWidth(g_tb[i].hwnd) != g_tb[i].w) return true;
    }
    return false;
}

// Returns false when the structure is unusable; nothing is changed then.
static bool ComputeTargets(HWND hwnd) {
    if (g_computing) return false;
    g_computing = true;

    bool ok = false;
    do {
        int n = CollectToolbars(hwnd);
        if (g_debug) Wh_Log(L"ComputeTargets: %d toolbar(s)", n);
        if (n <= 0) break;

        for (int i = 0; i < n; i++) SubclassToolbar(g_tb[i].hwnd);
        for (int i = 0; i < n; i++) HideMatching(g_tb[i].hwnd);

        // Dump once, once the toolbars really exist and hiding has been tried.
        if (g_debug && !g_dumped) {
            g_dumped = true;
            DumpAll(hwnd);
        }

        RECT client;
        GetClientRect(hwnd, &client);
        int height = (int)client.bottom;
        if (height <= 0) {
            RECT r;
            GetWindowRect(hwnd, &r);
            height = (int)(r.bottom - r.top);
        }

        // First non-empty toolbar anchors left, last one anchors right.
        // Everything in between is not laid out; it has nothing to show.
        int needed[MAX_TB];
        int firstIdx = -1;
        int lastIdx  = -1;
        for (int i = 0; i < n; i++) {
            needed[i] = NeededWidth(g_tb[i].hwnd);
            g_tb[i].w = 0;
            g_tb[i].x = 0;
            if (needed[i] <= 0) continue;
            if (firstIdx < 0) firstIdx = i;
            lastIdx = i;
        }

        if (lastIdx < 0) {
            Wh_Log(L"ComputeTargets: every toolbar is empty, abort");
            break;
        }

        int leftIdx = (firstIdx != lastIdx) ? firstIdx : -1;
        int leftW   = (leftIdx >= 0) ? needed[leftIdx] : 0;
        int rightW  = needed[lastIdx];

        int dpi = DpiOf(hwnd);
        g_dpi = dpi;
        int pad = MulDiv(EDGE_PAD, dpi, 96);
        int gap = MulDiv(g_gapWidth, dpi, 96);

        int minText = HostTextWidth(hwnd) + MulDiv(20, dpi, 96);
        if (g_textNeed > minText) minText = g_textNeed;
        if (gap < minText) {
            if (g_debug)
                Wh_Log(L"ComputeTargets: gap %d raised to %d for the hostname", gap, minText);
            gap = minText;
        }

        int width = pad + leftW + gap + rightW + pad;
        if (width <= 0) break;

        RECT bar = GetRectInParent(hwnd);   // SetWindowPos coordinate space
        if (!g_haveOrigBar) {
            g_origBar = bar;
            g_haveOrigBar = true;
        }
        int origW = (int)(g_origBar.right - g_origBar.left);

        // Only ever shrink.
        if (origW > 0 && width >= origW) {
            if (g_debug)
                Wh_Log(L"ComputeTargets: target %d >= original %d, nothing to do", width, origW);
            break;
        }

        int cursor = pad;
        if (leftIdx >= 0) {
            g_tb[leftIdx].x = cursor;
            g_tb[leftIdx].w = leftW;
            cursor += leftW;
        }

        g_textRect.left   = cursor;
        g_textRect.right  = cursor + gap;
        g_textRect.top    = 0;
        g_textRect.bottom = height;
        g_textLimitX      = cursor + gap;

        g_tb[lastIdx].x = cursor + gap;
        g_tb[lastIdx].w = rightW;

        g_targetW = width;
        // Anchor to the original left edge, not the current one: a recompute
        // after Commit would otherwise drift by half the shrink each time.
        g_targetX = (int)g_origBar.left + (origW > 0 ? (origW - width) / 2 : 0);

        if (g_debug)
            Wh_Log(L"ComputeTargets: leftW=%d gap=%d rightW=%d pad=%d -> width=%d (orig %d) text=%d..%d",
                   leftW, gap, rightW, pad, width, origW,
                   (int)g_textRect.left, (int)g_textRect.right);
        ok = true;
    } while (0);

    g_computing = false;
    return ok;
}

static void Commit(HWND hwnd) {
    RECT bar = GetRectInParent(hwnd);   // parent/client space for SetWindowPos
    RECT client;
    GetClientRect(hwnd, &client);
    int height = client.bottom > 0 ? (int)client.bottom : (int)(bar.bottom - bar.top);

    g_active = false;
    g_stale = false;
    SetWindowPos(hwnd, NULL, g_targetX, (int)bar.top, g_targetW,
                 (int)(bar.bottom - bar.top), SWP_NOZORDER | SWP_NOACTIVATE);

    for (int i = 0; i < g_tbCount; i++) {
        if (!IsWindow(g_tb[i].hwnd)) continue;
        SetWindowPos(g_tb[i].hwnd, NULL, g_tb[i].x, 0, g_tb[i].w, height,
                     SWP_NOZORDER | SWP_NOACTIVATE);
    }

    g_active = true;
    InvalidateRect(hwnd, NULL, FALSE);
    Wh_Log(L"Commit: bar x=%d w=%d, text zone %d..%d",
           g_targetX, g_targetW, (int)g_textRect.left, (int)g_textRect.right);
}

// ---------------------------------------------------------------- subclass procs

static LRESULT CALLBACK BarProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp, DWORD_PTR) {
    switch (msg) {
    case WM_PAINT: {
        if (!g_computing) {
            if (!g_active) {
                if (ComputeTargets(hwnd)) Commit(hwnd);
            } else if (LayoutStale(hwnd)) {
                if (ComputeTargets(hwnd)) Commit(hwnd);
            }
        }

        // Redirect the hostname only when a sane zone is known.
        if (g_active && g_textRect.right > g_textRect.left) {
            t_painting = true;
            LRESULT r = DefSubclassProc(hwnd, msg, wp, lp);
            t_painting = false;
            return r;
        }
        return DefSubclassProc(hwnd, msg, wp, lp);
    }

    case WM_WINDOWPOSCHANGING: {
        WINDOWPOS* p = (WINDOWPOS*)lp;
        if (g_active && p && !(p->flags & SWP_NOSIZE) && p->cx > g_targetW) {
            int oldCx = p->cx;
            p->cx = g_targetW;
            if (!(p->flags & SWP_NOMOVE))
                p->x += (oldCx - g_targetW) / 2;
        }
        break;
    }

    case WM_SHOWWINDOW:
        if (wp && !g_computing) {
            if (!g_active) {
                if (ComputeTargets(hwnd)) Commit(hwnd);
            } else if (LayoutStale(hwnd)) {
                if (ComputeTargets(hwnd)) Commit(hwnd);
            } else {
                InvalidateRect(hwnd, NULL, FALSE);
            }
        }
        break;

    case WM_DESTROY:
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hwnd, BarProc);
        if (hwnd == g_bbar) {
            g_bbar = NULL;
            g_tbCount = 0;
            g_haveOrigBar = false;
            g_active = false;
            g_hiddenCount = 0;
            g_textNeed = 0;
            g_stale = false;
            t_painting = false;
            g_dumped = false;
        }
        break;
    }
    return DefSubclassProc(hwnd, msg, wp, lp);
}

static LRESULT CALLBACK TbProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp, DWORD_PTR) {
    switch (msg) {
    case WM_WINDOWPOSCHANGING: {
        WINDOWPOS* p = (WINDOWPOS*)lp;
        if (g_active && p) {
            for (int i = 0; i < g_tbCount; i++) {
                if (g_tb[i].hwnd != hwnd) continue;
                if (!(p->flags & SWP_NOSIZE)) p->cx = g_tb[i].w;
                if (!(p->flags & SWP_NOMOVE)) p->x  = g_tb[i].x;
                break;
            }
        }
        break;
    }

    case WM_DESTROY:
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hwnd, TbProc);
        break;
    }
    return DefSubclassProc(hwnd, msg, wp, lp);
}

// ---------------------------------------------------------------- text painting

static UINT CenterFormat(UINT format) {
    return (format & ~(UINT)(DT_LEFT | DT_RIGHT)) | DT_CENTER | DT_VCENTER | DT_SINGLELINE;
}

// Centers the hostname inside its zone. A hostname wider than the zone gets
// the room it needs, up to the right button group, and the next layout pass
// widens the zone so it never stays clipped.
static LPRECT RedirectRect(HDC hdc, LPCWSTR text, int cch, LPRECT out) {
    *out = g_textRect;
    int tw = MeasureText(hdc, text, cch);
    if (tw > 0) {
        int pad = MulDiv(24, g_dpi, 96);
        int need = (int)g_textRect.left + tw + pad;
        if (need > (int)out->right) {
            out->right = (need < g_textLimitX) ? need : g_textLimitX;
            int learned = tw + pad;
            if (learned > g_textNeed + 4) {
                g_textNeed = learned;
                g_stale = true;
            }
        }
    }
    return out;
}

static int WINAPI DrawTextW_Hook(HDC hdc, LPCWSTR text, int cch, LPRECT lprc, UINT format) {
    if (t_painting && g_active && lprc && !(format & DT_CALCRECT) &&
        g_textRect.right > g_textRect.left && g_textLimitX > (int)g_textRect.left) {
        RECT rc;
        return DrawTextW_Original(hdc, text, cch, RedirectRect(hdc, text, cch, &rc),
                                  CenterFormat(format));
    }
    return DrawTextW_Original(hdc, text, cch, lprc, format);
}

static int WINAPI DrawTextExW_Hook(HDC hdc, LPWSTR text, int cch, LPRECT lprc,
                                   UINT format, LPDRAWTEXTPARAMS dtp) {
    if (t_painting && g_active && lprc && !(format & DT_CALCRECT) &&
        g_textRect.right > g_textRect.left && g_textLimitX > (int)g_textRect.left) {
        RECT rc;
        return DrawTextExW_Original(hdc, text, cch, RedirectRect(hdc, text, cch, &rc),
                                    CenterFormat(format), dtp);
    }
    return DrawTextExW_Original(hdc, text, cch, lprc, format, dtp);
}

// ---------------------------------------------------------------- hooks

static void AttachBar(HWND hwnd) {
    // Avoid double-attaching when CreateWindowExW hook and EnumWindows both see it
    if (hwnd == g_bbar) return;

    if (WindhawkUtils::SetWindowSubclassFromAnyThread(hwnd, BarProc, 0)) {
        g_bbar = hwnd;
        g_dumped = false;
        DWORD style = (DWORD)GetWindowLongW(hwnd, GWL_STYLE);
        Wh_Log(L"attached to BBar %p style=0x%08X %s", hwnd, style,
               (style & WS_CHILD) ? L"(child)" : L"(top-level)");
    } else {
        Wh_Log(L"SetWindowSubclass failed for %p", hwnd);
    }
}

static HWND WINAPI CreateWindowExW_Hook(DWORD exStyle, LPCWSTR cls, LPCWSTR name, DWORD style,
                                        int x, int y, int w, int h, HWND parent,
                                        HMENU menu, HINSTANCE inst, LPVOID param) {
    HWND hwnd = CreateWindowExW_Original(exStyle, cls, name, style, x, y, w, h,
                                         parent, menu, inst, param);
    if (hwnd) {
        wchar_t real[64];
        if (GetClassNameW(hwnd, real, 64) && lstrcmpW(real, L"BBarWindowClass") == 0) {
            Wh_Log(L"BBar created %p", hwnd);
            AttachBar(hwnd);
        }
    }
    return hwnd;
}

static void FindExisting() {
    ForEachBar(AttachBar);
}

static void DetachBar(HWND hwnd) {
    WindhawkUtils::RemoveWindowSubclassFromAnyThread(hwnd, BarProc);

    HWND tb = NULL;
    while ((tb = FindWindowExW(hwnd, tb, L"ToolbarWindow32", NULL)) != NULL)
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(tb, TbProc);
}

// ---------------------------------------------------------------- settings

static void LoadSettings() {
    g_gapWidth = Wh_GetIntSetting(L"gapWidth");
    if (g_gapWidth < 0) g_gapWidth = 0;
    if (g_gapWidth > 600) g_gapWidth = 600;

    g_hidePin     = Wh_GetIntSetting(L"hidePinButton") != 0;
    g_hideQuality = Wh_GetIntSetting(L"hideQualityButton") != 0;
    g_hideMin     = Wh_GetIntSetting(L"hideMinimizeButton") != 0;
    g_hideRestore = Wh_GetIntSetting(L"hideRestoreButton") != 0;
    g_hideClose   = Wh_GetIntSetting(L"hideCloseButton") != 0;
    g_debug       = Wh_GetIntSetting(L"debugLayout") != 0;
}

// ---------------------------------------------------------------- entry points

BOOL Wh_ModInit() {
    LoadSettings();

    WindhawkUtils::SetFunctionHook(DrawTextW, DrawTextW_Hook, &DrawTextW_Original);
    WindhawkUtils::SetFunctionHook(DrawTextExW, DrawTextExW_Hook, &DrawTextExW_Original);
    WindhawkUtils::SetFunctionHook(CreateWindowExW, CreateWindowExW_Hook, &CreateWindowExW_Original);

    Wh_Log(L"init 2.9.3.2: gap=%d pin=%d quality=%d min=%d restore=%d close=%d debug=%d",
           g_gapWidth, (int)g_hidePin, (int)g_hideQuality,
           (int)g_hideMin, (int)g_hideRestore, (int)g_hideClose,
           (int)g_debug);
    return TRUE;
}

void Wh_ModAfterInit() {
    FindExisting();
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    if (g_bbar && IsWindow(g_bbar)) {
        RestoreButtons();
        g_active = false;
        g_dumped = false;
        g_textNeed = 0;
        g_stale = false;
        if (!ComputeTargets(g_bbar)) InvalidateRect(g_bbar, NULL, FALSE);
    }
}

void Wh_ModUninit() {
    // Remove every subclass first. Both procs live in this mod's image, so a
    // bar or toolbar that keeps one crashes mstsc on its next message once the
    // mod is unloaded. Doing this before RestoreButtons also stops a WM_PAINT
    // from landing in the gap and recomputing the layout we are undoing.
    ForEachBar(DetachBar);

    RestoreButtons();
    g_active = false;

    if (g_bbar && IsWindow(g_bbar) && g_haveOrigBar) {
        // Restore in the same coordinate space we captured
        SetWindowPos(g_bbar, NULL, (int)g_origBar.left, (int)g_origBar.top,
                     (int)(g_origBar.right - g_origBar.left),
                     (int)(g_origBar.bottom - g_origBar.top),
                     SWP_NOZORDER | SWP_NOACTIVATE);

        for (int i = 0; i < g_tbCount; i++) {
            if (!IsWindow(g_tb[i].hwnd) || !g_tb[i].haveOrig) continue;
            SendMessageW(g_tb[i].hwnd, TB_AUTOSIZE, 0, 0);
            SetWindowPos(g_tb[i].hwnd, NULL, g_tb[i].ox, 0, g_tb[i].ow,
                         (int)(g_origBar.bottom - g_origBar.top),
                         SWP_NOZORDER | SWP_NOACTIVATE);
        }
        InvalidateRect(g_bbar, NULL, FALSE);
    }

    g_bbar = NULL;
    g_tbCount = 0;
    g_haveOrigBar = false;
    g_textNeed = 0;
    Wh_Log(L"unloaded");
}
