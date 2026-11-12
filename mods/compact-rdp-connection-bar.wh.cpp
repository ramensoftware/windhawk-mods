// ==WindhawkMod==
// @id              compact-rdp-connection-bar
// @name            Compact RDP connection bar
// @description     Shrinks the top connection bar, optionally hides pin/quality buttons, keeps drag working
// @version         2.9.0
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

## Settings

- **Text area width** - target width of the hostname zone, scaled for DPI.
  Automatically raised to fit the hostname, so it is never clipped.
- **Hide pin / quality button** - hide those buttons by command id. Turn on
  **Debug layout**, open the bar, and read the `btn[i] id=... bmp=...` lines in
  the log to confirm the ids.
- **Hide minimize / restore / close** - hide the window buttons on the
  right. They are matched by bitmap index (bmp 0/1/2), because mstsc gives
  those buttons plain command ids that are shared with the left toolbar.
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
- pinButtonId: 4
  $name: Pin button id
  $description: Command id of the pin button (see debug log)
- qualityButtonId: 6
  $name: Quality button id
  $description: Command id of the quality indicator (see debug log)
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
static int  g_pinId       = 4;
static int  g_qualityId   = 6;
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
static bool g_stale      = false;

static bool t_painting  = false;  // only touched on the bar's own thread
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

// ---------------------------------------------------------------- helpers

static int DpiOf(HWND h) {
    HDC dc = GetDC(h);
    int dpi = dc ? GetDeviceCaps(dc, LOGPIXELSX) : 96;
    if (dc) ReleaseDC(h, dc);
    return dpi > 0 ? dpi : 96;
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
    return (g_hidePin && id == g_pinId) || (g_hideQuality && id == g_qualityId);
}

// Window buttons on the right toolbar are matched by their bitmap index,
// which is unique per toolbar (0 minimize, 1 restore, 2 close on mstsc),
// while their command ids repeat the ids of the left toolbar.
static bool ShouldHideBmp(int bmp) {
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
        if ((ShouldHideId(b.idCommand) || ShouldHideBmp(b.iBitmap)) &&
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
        if (!ShouldHideId(b.idCommand) && !ShouldHideBmp(b.iBitmap)) continue;

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

        RECT bar;
        GetWindowRect(hwnd, &bar);
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
    RECT bar;
    GetWindowRect(hwnd, &bar);
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
        int pad = MulDiv(24, 96, 96);
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
    if (WindhawkUtils::SetWindowSubclassFromAnyThread(hwnd, BarProc, 0)) {
        g_bbar = hwnd;
        g_dumped = false;
        Wh_Log(L"attached to BBar %p", hwnd);
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
    HWND desktop = GetDesktopWindow();
    HWND hwnd = NULL;
    DWORD myPid = GetCurrentProcessId();
    while ((hwnd = FindWindowExW(desktop, hwnd, L"BBarWindowClass", NULL)) != NULL) {
        DWORD pid = 0;
        GetWindowThreadProcessId(hwnd, &pid);
        if (pid == myPid) AttachBar(hwnd);
    }
}

// ---------------------------------------------------------------- settings

static void LoadSettings() {
    g_gapWidth = Wh_GetIntSetting(L"gapWidth");
    if (g_gapWidth < 0) g_gapWidth = 0;
    if (g_gapWidth > 600) g_gapWidth = 600;

    g_hidePin     = Wh_GetIntSetting(L"hidePinButton") != 0;
    g_hideQuality = Wh_GetIntSetting(L"hideQualityButton") != 0;
    g_pinId       = Wh_GetIntSetting(L"pinButtonId");
    g_qualityId   = Wh_GetIntSetting(L"qualityButtonId");
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

    Wh_Log(L"init 2.9.0: gap=%d pin=%d(id %d) quality=%d(id %d) min=%d restore=%d close=%d debug=%d",
           g_gapWidth, (int)g_hidePin, g_pinId,
           (int)g_hideQuality, g_qualityId,
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
    RestoreButtons();
    g_active = false;
    t_painting = false;

    HWND desktop = GetDesktopWindow();
    HWND hwnd = NULL;
    DWORD myPid = GetCurrentProcessId();
    while ((hwnd = FindWindowExW(desktop, hwnd, L"BBarWindowClass", NULL)) != NULL) {
        DWORD pid = 0;
        GetWindowThreadProcessId(hwnd, &pid);
        if (pid == myPid)
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(hwnd, BarProc);
    }

    if (g_bbar && IsWindow(g_bbar) && g_haveOrigBar) {
        SetWindowPos(g_bbar, NULL, (int)g_origBar.left, (int)g_origBar.top,
                     (int)(g_origBar.right - g_origBar.left),
                     (int)(g_origBar.bottom - g_origBar.top),
                     SWP_NOZORDER | SWP_NOACTIVATE);

        for (int i = 0; i < g_tbCount; i++) {
            if (!IsWindow(g_tb[i].hwnd) || !g_tb[i].haveOrig) continue;
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(g_tb[i].hwnd, TbProc);
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
