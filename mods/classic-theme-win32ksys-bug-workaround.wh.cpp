// ==WindhawkMod==
// @id              classic-theme-win32ksys-bug-workaround
// @name            Classic theme caption/scrollbar button fix for 24H2+ builds 9444+
// @description     Fixes caption buttons and scrollbar arrows on newer Windows builds
// @version         0.28.0
// @author          Anixx
// @github          https://github.com/Anixx
// @include         *
// @compilerOptions -luser32 -lgdi32 -luxtheme
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
On the newest builds of Windows Microsoft broke the very core of its operating system, the essential part of the kernel,
the module win32k.sys.

This led to the titlebar buttons and scrollbars in unthemed applications being broken. Unfortunately, Windhawk cannot fix or patch a part of the kernel.
But it is possible to make a partial workaround.

You will encounter the bug if you are using the Classic theme on Windows versions 24H2, 25H2, 26H2 with build number 9444 or above or version 26H1.
If you are on build 8875 or below, you do not need this mod.

Microsoft removed the calculation of visual size of titlebar and scrollbar buttons, hardcoding them to 22px and 17px respectively,
so if you are using any other sizes, you will face broken visuals and garbage. This mod partially fixes the fallout.

Before mod:

![before](https://i.imgur.com/MSXt5Wt.png)

After mod:

![after](https://i.imgur.com/Y8GreLt.png)

This fix has multiple known issues. For instance, expect garbled buttons in conhost windows and when using DWM Ghost Mods mod.

*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <windowsx.h>
#include <algorithm>
#include <uxtheme.h>

// ======================= all tuning lives here =======================
// gapRight, iconicGapRight, kernelPad, kernelExtraLeft, kernelOffY and kernelScrollPad are given at 96 DPI
// and scaled to the window's DPI at run time. kernelBtnSize and kernelScrollSize are hardcoded in the
// kernel and are NOT scaled.
namespace cfg {
    constexpr int  gapRight        = 3;   // gap between close and the right edge of its cell
    constexpr int  iconicGapRight  = 2;   // minimized windows: gap between close and caption's right edge

    constexpr int  kernelBtnSize   = 22;  // hardcoded size of the kernel's buttons (0 = no cleanup)
    constexpr int  kernelPad       = 7;   // extra margin around the cleaned strip
    constexpr int  kernelExtraLeft = 0;   // extra margin to the left of the kernel buttons
    constexpr int  kernelOffY      = 0;   // vertical shift of the cleaned strip

    // Scrollbar: hardcoded size of the kernel's arrow buttons (0 = no cleanup)
    constexpr int  kernelScrollSize   = 17;
    // Extra length (along the bar) of the repaired frame strip at each end of the bar.
    // Also the extra margin of the area cleaned beyond the ends of the bars.
    constexpr int  kernelScrollPad    = 2;
    // The kernel's squares start at the near (left/top) edge of the bar and stick out towards
    // the far side (the window frame). Set to true if they stick out the other way
    // (then the garbage is in the client area and is repaired by invalidation).
    constexpr bool kernelCrossFromEnd = false;

    // "type" argument of NtUserMessageCall for calls coming from DefWindowProc
    // (only the low 16 bits are compared; the kernel may set extra high bits).
    constexpr ULONG_PTR fnidDefWindowProc = 0x029E;

    // Repaint the part of the client area covered by the kernel's buttons.
    constexpr bool cleanClientOverlap = true;
}
// =====================================================================

static HWND g_pressHwnd;
static int  g_pressId = -1;      // caption: 2..5; scroll arrows: 10..13
static bool g_tracking;
static thread_local bool t_busy;
static thread_local HWND t_hotHwnd;   // window whose NC area is currently hot
static thread_local int  t_hotHt;     // which hit-test area (HTCAPTION, HTCLOSE, etc.)

enum { DRAW_CAPTION = 1, DRAW_SCROLL = 2, DRAW_ALL = 3 };

typedef ULONG_PTR (NTAPI *MessageCall_t)(ULONG_PTR, ULONG_PTR, ULONG_PTR, ULONG_PTR,
                                         ULONG_PTR, ULONG_PTR, ULONG_PTR);
typedef ULONG_PTR (NTAPI *Fn5_t)(ULONG_PTR, ULONG_PTR, ULONG_PTR, ULONG_PTR, ULONG_PTR);
typedef ULONG_PTR (NTAPI *Fn4_t)(ULONG_PTR, ULONG_PTR, ULONG_PTR, ULONG_PTR);
typedef ULONG_PTR (NTAPI *Fn3_t)(ULONG_PTR, ULONG_PTR, ULONG_PTR);

static MessageCall_t g_origMsg;
static Fn4_t         g_origSetScrollInfo;
static Fn3_t         g_origEnableScrollBar;
static Fn4_t         g_origGetMessage;
static Fn5_t         g_origPeekMessage;
static bool          g_flushHooked;

static const wchar_t kActiveProp[] = L"WhClassicCapActive";   // 1 = active, 2 = inactive

struct BusyScope {
    BusyScope()  { t_busy = true; }
    ~BusyScope() { t_busy = false; }
};

// ---------- DPI helpers ----------

// DPI of the window (what the kernel uses for its non-client area), not the system DPI.
static UINT WinDpi(HWND hwnd)
{
    UINT d = GetDpiForWindow(hwnd);
    return d ? d : 96;
}

static int Sm(int index, UINT dpi)
{
    return GetSystemMetricsForDpi(index, dpi);
}

// Scale a 96-DPI pixel constant to the given DPI.
static int Px(int v, UINT dpi)
{
    return MulDiv(v, (int)dpi, 96);
}

// ---------- drawing primitives (pure GDI) ----------

static void Fill(HDC dc, int l, int t, int r, int b, COLORREF c)
{
    if (r <= l || b <= t) return;
    RECT rc = {l, t, r, b};
    SetBkColor(dc, c);
    ExtTextOutW(dc, 0, 0, ETO_OPAQUE, &rc, nullptr, 0, nullptr);
}

static COLORREF Lerp(COLORREF a, COLORREF b, int t, int d)
{
    if (d <= 0) return a;
    if (t < 0) t = 0;
    if (t > d) t = d;
    int r = GetRValue(a) + (GetRValue(b) - GetRValue(a)) * t / d;
    int g = GetGValue(a) + (GetGValue(b) - GetGValue(a)) * t / d;
    int bl = GetBValue(a) + (GetBValue(b) - GetBValue(a)) * t / d;
    return RGB(r, g, bl);
}

static void DrawButtonRaw(HDC dc, RECT r, UINT type, UINT state, bool pressed, bool disabled)
{
    if (pressed)  state |= DFCS_PUSHED | (type == DFC_SCROLL ? DFCS_FLAT : 0);
    if (disabled) state |= DFCS_INACTIVE;
    DrawFrameControl(dc, &r, type, state);
}

static void DrawButton(HDC dc, RECT r, UINT type, UINT state, bool pressed, bool disabled)
{
    int w = r.right - r.left, h = r.bottom - r.top;
    if (w <= 0 || h <= 0) return;
    HDC mdc = CreateCompatibleDC(dc);
    HBITMAP bm = mdc ? CreateCompatibleBitmap(dc, w, h) : nullptr;
    if (!mdc || !bm) {
        if (bm) DeleteObject(bm);
        if (mdc) DeleteDC(mdc);
        DrawButtonRaw(dc, r, type, state, pressed, disabled);
        return;
    }
    HGDIOBJ old = SelectObject(mdc, bm);
    DrawButtonRaw(mdc, {0, 0, w, h}, type, state, pressed, disabled);
    BitBlt(dc, r.left, r.top, w, h, mdc, 0, 0, SRCCOPY);
    SelectObject(mdc, old);
    DeleteObject(bm);
    DeleteDC(mdc);
}

// ---------- geometry (screen coordinates) ----------

static void GetTitleInfo(HWND hwnd, TITLEBARINFOEX& tb)
{
    ZeroMemory(&tb, sizeof(tb));
    tb.cbSize = sizeof(tb);
    SendMessageW(hwnd, WM_GETTITLEBARINFOEX, 0, (LPARAM)&tb);
}

static bool SameThread(HWND hwnd)
{
    DWORD pid;
    return GetWindowThreadProcessId(hwnd, &pid) == GetCurrentThreadId();
}

// Minimized windows hidden because of the taskbar live at about (-32000,-32000)
static bool IsParkedIconic(const RECT& wr)
{
    return wr.left <= -30000 || wr.top <= -30000;
}

// Frame of a minimized window is the fixed frame
static RECT IconicCaptionRect(const RECT& wr, UINT dpi)
{
    int fx = Sm(SM_CXFIXEDFRAME, dpi);
    int fy = Sm(SM_CYFIXEDFRAME, dpi);
    return {wr.left + fx, wr.top + fy, wr.right - fx, wr.bottom - fy};
}

struct CapBtns {
    TITLEBARINFOEX tb;
    RECT cell[6];     // kernel-reported (or synthesized for minimized windows) cells
    RECT btn[6];      // buttons we draw
    bool vis[6];
    bool dis[6];
};

// Which buttons the window is supposed to have (by style) and which of them are grayed
static void StyleButtons(HWND hwnd, LONG style, LONG ex, bool want[6], bool dis[6])
{
    for (int i = 0; i < 6; i++) { want[i] = false; dis[i] = false; }
    if (!(style & WS_SYSMENU)) return;

    const bool tool = (ex & WS_EX_TOOLWINDOW) != 0;
    const bool minb = (style & WS_MINIMIZEBOX) != 0;
    const bool maxb = (style & WS_MAXIMIZEBOX) != 0;

    want[5] = true;
    if (!tool) {
        if (minb || maxb) {
            want[2] = want[3] = true;
            dis[2] = !minb;
            dis[3] = !maxb;
        } else if (ex & WS_EX_CONTEXTHELP) {
            want[4] = true;
        }
    }

    if (GetClassLongW(hwnd, GCL_STYLE) & CS_NOCLOSE) {
        dis[5] = true;
    } else {
        HMENU sm = GetSystemMenu(hwnd, FALSE);
        if (sm) {
            UINT s = GetMenuState(sm, SC_CLOSE, MF_BYCOMMAND);
            if (s != (UINT)-1 && (s & (MF_GRAYED | MF_DISABLED))) dis[5] = true;
        }
    }
}

static int FrameSize(HWND hwnd, bool vertical);   // defined below

static bool LayoutCaption(HWND hwnd, CapBtns& o)
{
    ZeroMemory(&o, sizeof(o));
    const LONG style = GetWindowLongW(hwnd, GWL_STYLE);
    const LONG ex    = GetWindowLongW(hwnd, GWL_EXSTYLE);
    const bool tool   = (ex & WS_EX_TOOLWINDOW) != 0;
    const bool iconic = (style & WS_MINIMIZE) != 0;
    const UINT dpi    = WinDpi(hwnd);

    bool want[6], sdis[6];
    StyleButtons(hwnd, style, ex, want, sdis);

    int right = -0x7fffffff, top = 0, h = 0;
    int inset = Px(cfg::gapRight, dpi);
    bool any = false;

    if (iconic) {
        // No cells from the kernel for minimized windows: synthesize them
        RECT wr;
        if (!GetWindowRect(hwnd, &wr)) return false;
        RECT cap = IconicCaptionRect(wr, dpi);
        if (cap.right <= cap.left || cap.bottom <= cap.top) return false;
        const int cw = Sm(tool ? SM_CXSMSIZE : SM_CXSIZE, dpi);
        int cur = cap.right;
        const int ord[4] = {5, 4, 3, 2};
        for (int k = 0; k < 4; k++) {
            int i = ord[k];
            if (!want[i]) continue;
            o.vis[i] = true;
            o.cell[i] = {cur - cw, cap.top, cur, cap.bottom};
            cur -= cw;
            any = true;
        }
        right = cap.right;
        top   = cap.top;
        h     = cap.bottom - cap.top;
        inset = Px(cfg::iconicGapRight, dpi);
    } else {
        GetTitleInfo(hwnd, o.tb);
        for (int i = 2; i <= 5; i++) {
            if (!want[i]) continue;
            const RECT& c = o.tb.rgrect[i];
            if (o.tb.rgstate[i] & (STATE_SYSTEM_INVISIBLE | STATE_SYSTEM_OFFSCREEN)) continue;
            if (c.right <= c.left || c.bottom <= c.top) continue;
            o.vis[i] = true;
            o.cell[i] = c;
            if (!any) { top = c.top; h = c.bottom - c.top; }
            any = true;
            if (c.right > right) right = c.right;
            Wh_Log(L"cell[%d]=(%d,%d,%d,%d) tool=%d dpi=%u", i, c.left, c.top, c.right, c.bottom,
                   (int)tool, dpi);
        }
    }
    if (!any) return false;

    // Size: system metrics at the window's DPI (SmCaption* for palette windows).
    // Position: centered in the cell.
    const int cxEdge = Sm(SM_CXEDGE, dpi);
    const int cyEdge = Sm(SM_CYEDGE, dpi);
    int bw = Sm(tool ? SM_CXSMSIZE : SM_CXSIZE, dpi) - cxEdge;
    int bh = Sm(tool ? SM_CYSMSIZE : SM_CYSIZE, dpi) - 2 * cyEdge;
    if (bw < 6) bw = 6;
    if (bh < 6) bh = 6;
    if (bh > h) bh = h;
    const int by = top + (h - bh) / 2;

    int cur = right - inset;
    const int order[4] = {5, 4, 3, 2};
    for (int k = 0; k < 4; k++) {
        int i = order[k];
        if (!o.vis[i]) continue;
        o.btn[i] = {cur - bw, by, cur, by + bh};
        cur = o.btn[i].left - (i == 3 ? 0 : cxEdge);
    }

    for (int i = 2; i <= 5; i++)
        o.dis[i] = sdis[i] || (o.tb.rgstate[i] & STATE_SYSTEM_UNAVAILABLE) != 0;

    return true;
}

static bool ScrollArrows(HWND hwnd, int obj, RECT a[2], bool dis[2])
{
    SCROLLBARINFO sb = {};
    sb.cbSize = sizeof(sb);
    if (!GetScrollBarInfo(hwnd, obj, &sb)) return false;
    if (sb.rgstate[0] & (STATE_SYSTEM_INVISIBLE | STATE_SYSTEM_OFFSCREEN)) return false;
    RECT r = sb.rcScrollBar;
    int d = sb.dxyLineButton;
    bool vert = (obj == OBJID_VSCROLL);
    int len = vert ? (r.bottom - r.top) : (r.right - r.left);
    if (d * 2 > len) d = len / 2;
    if (d <= 0) return false;
    if (vert) {
        a[0] = {r.left, r.top, r.right, r.top + d};
        a[1] = {r.left, r.bottom - d, r.right, r.bottom};
    } else {
        a[0] = {r.left, r.top, r.left + d, r.bottom};
        a[1] = {r.right - d, r.top, r.right, r.bottom};
    }
    dis[0] = (sb.rgstate[1] & STATE_SYSTEM_UNAVAILABLE) != 0;
    dis[1] = (sb.rgstate[5] & STATE_SYSTEM_UNAVAILABLE) != 0;
    return true;
}

static bool IsCaptionActive(HWND hwnd)
{
    HANDLE p = GetPropW(hwnd, kActiveProp);
    if (p) return p == (HANDLE)1;
    return GetActiveWindow() == hwnd;
}

static int FrameSize(HWND hwnd, bool vertical)
{
    const UINT dpi = WinDpi(hwnd);
    LONG style = GetWindowLongW(hwnd, GWL_STYLE);
    if (style & WS_MINIMIZE)
        return Sm(vertical ? SM_CYFIXEDFRAME : SM_CXFIXEDFRAME, dpi);
    if (style & WS_THICKFRAME)
        return Sm(vertical ? SM_CYSIZEFRAME : SM_CXSIZEFRAME, dpi) +
               Sm(SM_CXPADDEDBORDER, dpi);
    if (style & (WS_DLGFRAME | WS_CAPTION))
        return Sm(vertical ? SM_CYFIXEDFRAME : SM_CXFIXEDFRAME, dpi);
    return Sm(vertical ? SM_CYBORDER : SM_CXBORDER, dpi);
}

static bool ClientScreenRect(HWND hwnd, RECT& r)
{
    if (!GetClientRect(hwnd, &r)) return false;
    POINT p = {0, 0};
    if (!ClientToScreen(hwnd, &p)) return false;
    OffsetRect(&r, p.x, p.y);
    return true;
}

static RECT ComputeCaptionRect(HWND hwnd, const TITLEBARINFOEX& tb, const RECT& wr)
{
    const UINT dpi = WinDpi(hwnd);
    if (IsIconic(hwnd)) return IconicCaptionRect(wr, dpi);

    LONG ex = GetWindowLongW(hwnd, GWL_EXSTYLE);
    int capH = Sm((ex & WS_EX_TOOLWINDOW) ? SM_CYSMCAPTION : SM_CYCAPTION, dpi);

    int fx = FrameSize(hwnd, false), fy = FrameSize(hwnd, true);
    RECT m = {wr.left + fx, wr.top + fy, wr.right - fx, wr.top + fy + capH - 1};
    RECT t = tb.rcTitleBar, r;
    if (t.right > t.left && t.bottom > t.top && IntersectRect(&r, &t, &m))
        return r;
    return m;
}

struct CapGeom {
    RECT wr;
    CapBtns b;
    RECT cap;
    bool hasStrip;
    RECT strip;
    UINT dpi;
};

static void ComputeStrip(CapGeom& g)
{
    g.hasStrip = false;
    const LONG S = cfg::kernelBtnSize;   // hardcoded in the kernel, not DPI-scaled
    if (S <= 0) return;

    const LONG pad       = Px(cfg::kernelPad, g.dpi);
    const LONG extraLeft = Px(cfg::kernelExtraLeft, g.dpi);
    const LONG offY      = Px(cfg::kernelOffY, g.dpi);

    LONG left = 0x7fffffff, right = -0x7fffffff;
    LONG top = 0x7fffffff, bottom = -0x7fffffff;
    LONG cellTop = 0;
    int n = 0;
    for (int i = 2; i <= 5; i++) {
        if (!g.b.vis[i]) continue;
        const RECT& c = g.b.cell[i];
        const RECT& b = g.b.btn[i];
        if (n == 0) cellTop = c.top;
        left   = std::min({left,   c.left,   b.left});
        right  = std::max({right,  c.right,  b.right, c.left + S});
        top    = std::min({top,    c.top,    b.top});
        bottom = std::max({bottom, c.bottom, b.bottom});
        n++;
    }
    if (!n) return;

    const LONG kernelLeft   = right - (S * n + extraLeft);
    const LONG kernelTop    = cellTop + offY;
    const LONG kernelBottom = kernelTop + S;

    RECT strip;
    strip.left   = std::min(kernelLeft, left) - pad;
    strip.right  = right + pad;
    strip.top    = std::min(top, kernelTop) - pad;
    strip.bottom = std::max({bottom, kernelBottom, g.cap.bottom}) + pad;

    if (!IntersectRect(&g.strip, &strip, &g.wr)) return;
    g.hasStrip = true;
}

static bool GetCapGeom(HWND hwnd, CapGeom& g)
{
    ZeroMemory(&g, sizeof(g));
    if (!IsWindow(hwnd) || !IsWindowVisible(hwnd) || !SameThread(hwnd))
        return false;
    LONG style = GetWindowLongW(hwnd, GWL_STYLE);
    if ((style & WS_CAPTION) != WS_CAPTION) return false;
    if (!GetWindowRect(hwnd, &g.wr)) return false;
    if ((style & WS_MINIMIZE) && IsParkedIconic(g.wr)) return false;
    g.dpi = WinDpi(hwnd);
    if (!LayoutCaption(hwnd, g.b)) return false;
    g.cap = ComputeCaptionRect(hwnd, g.b.tb, g.wr);
    ComputeStrip(g);
    return true;
}

static RECT CaptionSpan(HWND hwnd, const CapGeom& g)
{
    int f = FrameSize(hwnd, false);
    return {g.wr.left + f, g.cap.top, g.wr.right - f, g.cap.bottom};
}

static void DrawCaptionButtons(HWND hwnd, HDC target, const CapGeom& g, int originX, int originY,
                               bool raw)
{
    const bool iconic = IsIconic(hwnd);
    for (int i = 2; i <= 5; i++) {
        if (!g.b.vis[i]) continue;
        DWORD st = g.b.tb.rgstate[i];
        RECT r = g.b.btn[i];
        OffsetRect(&r, -originX, -originY);
        UINT style;
        if (i == 2)      style = iconic ? DFCS_CAPTIONRESTORE : DFCS_CAPTIONMIN;
        else if (i == 3) style = IsZoomed(hwnd) ? DFCS_CAPTIONRESTORE : DFCS_CAPTIONMAX;
        else if (i == 4) style = DFCS_CAPTIONHELP;
        else             style = DFCS_CAPTIONCLOSE;
        bool dis = g.b.dis[i];
        bool pressed = !dis && ((st & STATE_SYSTEM_PRESSED) ||
                                (g_pressHwnd == hwnd && g_pressId == i));
        if (raw) DrawButtonRaw(target, r, DFC_CAPTION, style, pressed, dis);
        else     DrawButton(target, r, DFC_CAPTION, style, pressed, dis);
    }
}

static bool PaintCaptionStrip(HWND hwnd, HDC dc, const CapGeom& g)
{
    const RECT& wr = g.wr;
    const RECT& cap = g.cap;
    if (cap.bottom <= cap.top) return false;
    RECT span = CaptionSpan(hwnd, g);
    if (span.right <= span.left + 1) return false;
    RECT area;
    if (!IntersectRect(&area, &g.strip, &span)) return false;

    RECT box = area;
    for (int i = 2; i <= 5; i++)
        if (g.b.vis[i]) UnionRect(&box, &box, &g.b.btn[i]);
    RECT inWin;
    if (!IntersectRect(&inWin, &box, &wr)) return false;
    box = inWin;
    int bw = box.right - box.left, bh = box.bottom - box.top;
    if (bw <= 0 || bh <= 0) return false;

    bool active = IsCaptionActive(hwnd);
    COLORREF c1 = GetSysColor(active ? COLOR_ACTIVECAPTION : COLOR_INACTIVECAPTION);
    COLORREF c2 = GetSysColor(active ? COLOR_GRADIENTACTIVECAPTION : COLOR_GRADIENTINACTIVECAPTION);

    int btnLeft = area.right;
    for (int i = 2; i <= 5; i++)
        if (g.b.vis[i] && g.b.btn[i].left < btnLeft) btnLeft = g.b.btn[i].left;
    if (btnLeft < area.left) btnLeft = area.left;

    int sy = (cap.top + 1 < cap.bottom) ? cap.top + 1 : cap.top;
    COLORREF L = CLR_INVALID;
    if (area.left - 1 >= span.left)
        L = GetPixel(dc, area.left - 1 - wr.left, sy - wr.top);
    if (L == CLR_INVALID)
        L = Lerp(c1, c2, area.left - span.left, span.right - span.left - 1);

    HDC mdc = CreateCompatibleDC(dc);
    HBITMAP bm = mdc ? CreateCompatibleBitmap(dc, bw, bh) : nullptr;
    if (!mdc || !bm) {
        if (bm) DeleteObject(bm);
        if (mdc) DeleteDC(mdc);
        return false;
    }
    HGDIOBJ old = SelectObject(mdc, bm);
    BitBlt(mdc, 0, 0, bw, bh, dc, box.left - wr.left, box.top - wr.top, SRCCOPY);

    auto colorAt = [&](int x) -> COLORREF {
        if (x >= btnLeft) return c2;
        return Lerp(L, c2, x - area.left + 1, btnLeft - area.left);
    };

    int runStart = area.left;
    COLORREF runC = colorAt(area.left);
    for (int x = area.left + 1; x <= area.right; x++) {
        COLORREF c = (x < area.right) ? colorAt(x) : CLR_INVALID;
        if (x == area.right || c != runC) {
            Fill(mdc, runStart - box.left, area.top - box.top, x - box.left,
                 area.bottom - box.top, runC);
            runStart = x;
            runC = c;
        }
    }

    DrawCaptionButtons(hwnd, mdc, g, box.left, box.top, true);

    BitBlt(dc, box.left - wr.left, box.top - wr.top, bw, bh, mdc, 0, 0, SRCCOPY);
    SelectObject(mdc, old);
    DeleteObject(bm);
    DeleteDC(mdc);
    return true;
}

static void RepairBand(HWND hwnd, const CapGeom& g)
{
    RECT span = CaptionSpan(hwnd, g);
    HRGN rgn = CreateRectRgnIndirect(&g.strip);
    if (!rgn) return;
    HRGN tmp = CreateRectRgn(0, 0, 0, 0);
    if (tmp) {
        SetRectRgn(tmp, span.left, span.top, span.right, span.bottom);
        CombineRgn(rgn, rgn, tmp, RGN_DIFF);
        RECT cl;
        if (ClientScreenRect(hwnd, cl)) {
            SetRectRgn(tmp, cl.left, cl.top, cl.right, cl.bottom);
            CombineRgn(rgn, rgn, tmp, RGN_DIFF);
        }
        DeleteObject(tmp);
    }

    RECT box;
    if (GetRgnBox(rgn, &box) > NULLREGION) {
        LONG_PTR st = GetWindowLongPtrW(hwnd, GWL_STYLE);
        SetWindowLongPtrW(hwnd, GWL_STYLE,
                          st & ~(LONG_PTR)(WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX));
        DefWindowProcW(hwnd, WM_NCPAINT, (WPARAM)rgn, 0);
        SetWindowLongPtrW(hwnd, GWL_STYLE, st);
    }
    if (GetObjectType(rgn) == OBJ_REGION) DeleteObject(rgn);
}

static void InvalidateClientOverlap(HWND hwnd, const RECT& strip)
{
    RECT cl, ov;
    if (!ClientScreenRect(hwnd, cl) || !IntersectRect(&ov, &cl, &strip)) return;
    MapWindowPoints(nullptr, hwnd, (POINT*)&ov, 2);
    RedrawWindow(hwnd, &ov, nullptr, RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN);
}

static void CleanKernelScrollButtons(HWND hwnd, HDC dc, const RECT& wr, int obj, bool live)
{
    const UINT dpi = WinDpi(hwnd);
    const int S = cfg::kernelScrollSize;   // hardcoded in the kernel, not DPI-scaled
    if (S <= 0) return;

    SCROLLBARINFO sb = {};
    sb.cbSize = sizeof(sb);
    if (!GetScrollBarInfo(hwnd, obj, &sb)) return;
    if (sb.rgstate[0] & (STATE_SYSTEM_INVISIBLE | STATE_SYSTEM_OFFSCREEN)) return;

    const RECT bar = sb.rcScrollBar;
    const bool vert = (obj == OBJID_VSCROLL);
    const int thick = vert ? (bar.right - bar.left) : (bar.bottom - bar.top);
    const int len   = vert ? (bar.bottom - bar.top) : (bar.right - bar.left);
    const int reach = S + Px(cfg::kernelScrollPad, dpi);

    RECT cl = {};
    bool haveCl = ClientScreenRect(hwnd, cl);

    Wh_Log(L"scroll obj=%d bar=(%d,%d,%d,%d) thick=%d len=%d dxyLine=%d win=(%d,%d,%d,%d) client=(%d,%d,%d,%d) haveCl=%d dpi=%u",
           obj, bar.left, bar.top, bar.right, bar.bottom, thick, len, (int)sb.dxyLineButton,
           wr.left, wr.top, wr.right, wr.bottom, cl.left, cl.top, cl.right, cl.bottom,
           (int)haveCl, dpi);

    if (thick >= S) return;
    if (len < 2 * reach + 2) return;
    if (!haveCl) return;

    const bool frameAfter = vert ? (bar.left + bar.right > cl.left + cl.right)
                                 : (bar.top + bar.bottom > cl.top + cl.bottom);
    const bool overAfter = !cfg::kernelCrossFromEnd;

    if (overAfter != frameAfter) {
        if (live && cfg::cleanClientOverlap) {
            RECT r[2];
            int c0 = overAfter ? (vert ? bar.right : bar.bottom) : (vert ? bar.right - S : bar.bottom - S);
            int c1 = overAfter ? (vert ? bar.left + S : bar.top + S) : (vert ? bar.left : bar.top);
            if (c1 > c0) {
                if (vert) {
                    r[0] = {c0, bar.top, c1, bar.top + reach};
                    r[1] = {c0, bar.bottom - reach, c1, bar.bottom};
                } else {
                    r[0] = {bar.left, c0, bar.left + reach, c1};
                    r[1] = {bar.right - reach, c0, bar.right, c1};
                }
                for (int i = 0; i < 2; i++) {
                    RECT ov;
                    if (!IntersectRect(&ov, &r[i], &cl)) continue;
                    MapWindowPoints(nullptr, hwnd, (POINT*)&ov, 2);
                    RedrawWindow(hwnd, &ov, nullptr, RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN);
                }
            }
        }
        return;
    }

    RECT b = bar;
    OffsetRect(&b, -wr.left, -wr.top);
    const int winW = wr.right - wr.left, winH = wr.bottom - wr.top;

    if (vert) {
        int x0 = frameAfter ? b.right : 0;
        int x1 = frameAfter ? winW : b.left;
        if (x1 > winW) x1 = winW;
        if (x0 < 0) x0 = 0;
        int w = x1 - x0;
        if (w <= 0) return;
        int yr = b.top + len / 2;
        StretchBlt(dc, x0, b.top, w, reach, dc, x0, yr, w, 1, SRCCOPY);
        StretchBlt(dc, x0, b.bottom - reach, w, reach, dc, x0, yr, w, 1, SRCCOPY);
    } else {
        int y0 = frameAfter ? b.bottom : 0;
        int y1 = frameAfter ? winH : b.top;
        if (y1 > winH) y1 = winH;
        if (y0 < 0) y0 = 0;
        int h = y1 - y0;
        if (h <= 0) return;
        int xr = b.left + len / 2;
        StretchBlt(dc, b.left, y0, reach, h, dc, xr, y0, 1, h, SRCCOPY);
        StretchBlt(dc, b.right - reach, y0, reach, h, dc, xr, y0, 1, h, SRCCOPY);
    }
}

static bool GetBarInfo(HWND hwnd, int obj, RECT& bar, int& d)
{
    SCROLLBARINFO sb = {};
    sb.cbSize = sizeof(sb);
    if (!GetScrollBarInfo(hwnd, obj, &sb)) return false;
    if (sb.rgstate[0] & (STATE_SYSTEM_INVISIBLE | STATE_SYSTEM_OFFSCREEN)) return false;
    bar = sb.rcScrollBar;
    d = sb.dxyLineButton;
    int thick = (obj == OBJID_VSCROLL) ? bar.right - bar.left : bar.bottom - bar.top;
    if (d <= 0) d = thick;
    return true;
}

static inline bool InRc(const RECT& r, int x, int y)
{
    return x >= r.left && x < r.right && y >= r.top && y < r.bottom;
}

static void CleanKernelOverflow(HWND hwnd, HDC dc, const RECT& wr, LONG style)
{
    const UINT dpi = WinDpi(hwnd);
    const int S = cfg::kernelScrollSize;   // hardcoded in the kernel, not DPI-scaled
    if (S <= 0 || cfg::kernelCrossFromEnd) return;

    RECT vb = {}, hb = {}, cl = {};
    int vd = 0, hd = 0;
    const bool hasV = (style & WS_VSCROLL) && GetBarInfo(hwnd, OBJID_VSCROLL, vb, vd);
    const bool hasH = (style & WS_HSCROLL) && GetBarInfo(hwnd, OBJID_HSCROLL, hb, hd);
    if (!hasV && !hasH) return;
    if (!ClientScreenRect(hwnd, cl)) return;

    OffsetRect(&vb, -wr.left, -wr.top);
    OffsetRect(&hb, -wr.left, -wr.top);
    OffsetRect(&cl, -wr.left, -wr.top);
    const int winW = wr.right - wr.left, winH = wr.bottom - wr.top;
    const RECT win = {0, 0, winW, winH};
    const int pad = Px(cfg::kernelScrollPad, dpi);

    RECT g[2];
    int ng = 0;
    if (hasV && vd < S) {
        RECT t = {vb.left, vb.bottom, vb.left + S + pad, vb.bottom + (S - vd) + pad};
        if (IntersectRect(&g[ng], &t, &win)) ng++;
    }
    if (hasH && hd < S) {
        RECT t = {hb.right, hb.top, hb.right + (S - hd) + pad, hb.top + S + pad};
        if (IntersectRect(&g[ng], &t, &win)) ng++;
    }
    if (!ng) return;

    COLORREF ring[32];
    int ringN = 0;
    if (hasV) {
        int len = vb.bottom - vb.top;
        if (len < 2 * S + 2) return;
        int ys = vb.top + len / 2;
        ringN = std::min(winW - (int)vb.right, 32);
        for (int k = 0; k < ringN; k++) ring[k] = GetPixel(dc, winW - 1 - k, ys);
    } else {
        int len = hb.right - hb.left;
        if (len < 2 * S + 2) return;
        int xs = hb.left + len / 2;
        ringN = std::min(winH - (int)hb.bottom, 32);
        for (int k = 0; k < ringN; k++) ring[k] = GetPixel(dc, xs, winH - 1 - k);
    }

    const bool hasC = hasV && hasH && hb.bottom > hb.top && vb.right > vb.left;
    const RECT C = {vb.left, hb.top, vb.right, hb.bottom};

    if (ringN > 0) {
        for (int i = 0; i < ng; i++) {
            for (int y = g[i].top; y < g[i].bottom; y++) {
                for (int x = g[i].left; x < g[i].right; x++) {
                    if (InRc(cl, x, y) || InRc(vb, x, y) || InRc(hb, x, y)) continue;
                    if (hasC && InRc(C, x, y)) continue;
                    int k = std::min(winW - 1 - x, winH - 1 - y);
                    if (k < 0) continue;
                    if (k >= ringN) k = ringN - 1;
                    if (ring[k] != CLR_INVALID) SetPixelV(dc, x, y, ring[k]);
                }
            }
        }
    }

    if (hasC) {
        RECT gc[2];
        int n = 0;
        for (int i = 0; i < ng; i++)
            if (IntersectRect(&gc[n], &g[i], &C)) n++;
        if (n) {
            const COLORREF face = GetSysColor(COLOR_3DFACE);
            bool grip = false;
            for (int y = C.top; y < C.bottom && !grip; y++) {
                for (int x = C.left; x < C.right; x++) {
                    bool dirty = false;
                    for (int j = 0; j < n; j++) if (InRc(gc[j], x, y)) dirty = true;
                    if (dirty) continue;
                    COLORREF p = GetPixel(dc, x, y);
                    if (p != CLR_INVALID && p != face) { grip = true; break; }
                }
            }
            for (int j = 0; j < n; j++)
                Fill(dc, gc[j].left, gc[j].top, gc[j].right, gc[j].bottom, face);
            if (grip) {
                int saved = SaveDC(dc);
                HRGN rgn = CreateRectRgn(0, 0, 0, 0);
                if (rgn) {
                    for (int j = 0; j < n; j++) {
                        HRGN t = CreateRectRgnIndirect(&gc[j]);
                        if (t) { CombineRgn(rgn, rgn, t, RGN_OR); DeleteObject(t); }
                    }
                    SelectClipRgn(dc, rgn);
                    RECT rc = C;
                    DrawFrameControl(dc, &rc, DFC_SCROLL, DFCS_SCROLLSIZEGRIP);
                    DeleteObject(rgn);
                }
                RestoreDC(dc, saved);
            }
        }
    }
}

static void DrawScrollBars(HWND hwnd, HDC dc, const RECT& wr, LONG style, bool live)
{
    const struct { LONG bit; int obj; int baseId; UINT s0, s1; } bars[] = {
        {WS_VSCROLL, OBJID_VSCROLL, 10, DFCS_SCROLLUP,   DFCS_SCROLLDOWN},
        {WS_HSCROLL, OBJID_HSCROLL, 12, DFCS_SCROLLLEFT, DFCS_SCROLLRIGHT},
    };
    for (auto& b : bars) {
        if (!(style & b.bit)) continue;
        RECT a[2];
        bool dis[2];
        if (!ScrollArrows(hwnd, b.obj, a, dis)) continue;
        for (int k = 0; k < 2; k++) {
            RECT r = a[k];
            OffsetRect(&r, -wr.left, -wr.top);
            bool pressed = !dis[k] && g_pressHwnd == hwnd && g_pressId == b.baseId + k;
            DrawButton(dc, r, DFC_SCROLL, k == 0 ? b.s0 : b.s1, pressed, dis[k]);
        }
        CleanKernelScrollButtons(hwnd, dc, wr, b.obj, live);
    }
    CleanKernelOverflow(hwnd, dc, wr, style);
}

// ---------- redraw of the buttons ----------

static void DrawAll(HWND hwnd, int what, bool repair, bool clientDirty = false)
{
    if (t_busy) return;
    if (!IsWindow(hwnd) || !IsWindowVisible(hwnd) || !SameThread(hwnd)) return;

    BusyScope busy;
    RECT wr;
    if (!GetWindowRect(hwnd, &wr)) return;
    LONG style = GetWindowLongW(hwnd, GWL_STYLE);
    const bool iconic = (style & WS_MINIMIZE) != 0;
    if (iconic && IsParkedIconic(wr)) return;

    HDC dc = GetWindowDC(hwnd);
    if (!dc) return;

    if ((what & DRAW_CAPTION) && (style & WS_CAPTION) == WS_CAPTION) {
        CapGeom g;
        if (GetCapGeom(hwnd, g)) {
            bool composed = false;
            if (repair && g.hasStrip) {
                RepairBand(hwnd, g);
                if (clientDirty && cfg::cleanClientOverlap)
                    InvalidateClientOverlap(hwnd, g.strip);
                composed = PaintCaptionStrip(hwnd, dc, g);
            }
            if (!composed)
                DrawCaptionButtons(hwnd, dc, g, wr.left, wr.top, false);
        }
    }

    if ((what & DRAW_SCROLL) && !iconic)
        DrawScrollBars(hwnd, dc, wr, style, true);

    ReleaseDC(hwnd, dc);
}

// ---------- deferred repair ----------

static thread_local HWND t_dirtyWnd[8];
static thread_local bool t_dirtyClient[8];
static thread_local int  t_dirtyN;

static void MarkDirty(HWND h, bool client)
{
    if (t_busy || !IsWindow(h) || !SameThread(h)) return;
    if (!g_flushHooked) {
        if (!g_tracking) DrawAll(h, DRAW_CAPTION, true, client);
        return;
    }
    for (int i = 0; i < t_dirtyN; i++) {
        if (t_dirtyWnd[i] == h) {
            if (client) t_dirtyClient[i] = true;
            return;
        }
    }
    if (t_dirtyN < 8) {
        t_dirtyWnd[t_dirtyN] = h;
        t_dirtyClient[t_dirtyN] = client;
        t_dirtyN++;
    }
}

static void FlushDirty()
{
    if (t_dirtyN == 0 || t_busy || g_tracking) return;
    while (t_dirtyN > 0) {
        if (HIWORD(GetQueueStatus(QS_PAINT)) & QS_PAINT) return;
        HWND h = t_dirtyWnd[0];
        bool client = t_dirtyClient[0];
        for (int i = 1; i < t_dirtyN; i++) {
            t_dirtyWnd[i - 1] = t_dirtyWnd[i];
            t_dirtyClient[i - 1] = t_dirtyClient[i];
        }
        t_dirtyN--;
        if (IsWindow(h)) DrawAll(h, DRAW_CAPTION, true, client);
    }
}

// ---------- own tracking loop ----------

struct Target {
    int kind;
    int idx;
    int obj;
};

static bool GetTargetRect(HWND hwnd, const Target& t, RECT* r, bool* disabled)
{
    if (t.kind == 0) {
        CapBtns b;
        if (!LayoutCaption(hwnd, b) || !b.vis[t.idx]) return false;
        *r = b.btn[t.idx];
        *disabled = b.dis[t.idx];
        return true;
    }
    RECT a[2];
    bool d[2];
    if (!ScrollArrows(hwnd, t.obj, a, d)) return false;
    *r = a[t.idx];
    *disabled = d[t.idx];
    return true;
}

static void Track(HWND hwnd, const Target& t, int pressId)
{
    g_tracking = true;
    SetCapture(hwnd);
    g_pressHwnd = hwnd;
    g_pressId = pressId;

    const int what = (t.kind == 0) ? DRAW_CAPTION : DRAW_SCROLL;
    const UINT scrollMsg = (t.obj == OBJID_HSCROLL) ? WM_HSCROLL : WM_VSCROLL;
    const WPARAM scrollCode = (t.idx == 0) ? SB_LINEUP : SB_LINEDOWN;

    UINT_PTR timer = 0;
    if (t.kind == 1) {
        SendMessageW(hwnd, scrollMsg, scrollCode, 0);
        timer = SetTimer(nullptr, 0, 400, nullptr);
    }
    DrawAll(hwnd, what, false);

    bool inside = true, cancelled = false;
    MSG m;
    while (GetCapture() == hwnd) {
        if (!GetMessageW(&m, nullptr, 0, 0)) {
            PostQuitMessage((int)m.wParam);
            cancelled = true;
            break;
        }
        if (m.message == WM_LBUTTONUP || m.message == WM_NCLBUTTONUP) break;
        if (m.message == WM_KEYDOWN && m.wParam == VK_ESCAPE) { cancelled = true; break; }
        if (m.message == WM_MOUSEMOVE && m.hwnd == hwnd) {
            POINT pt;
            GetCursorPos(&pt);
            RECT cur;
            bool dis;
            bool in = GetTargetRect(hwnd, t, &cur, &dis) && PtInRect(&cur, pt);
            if (in != inside) {
                inside = in;
                g_pressId = in ? pressId : -1;
                DrawAll(hwnd, what, false);
            }
            continue;
        }
        if (timer && m.message == WM_TIMER && m.hwnd == nullptr && m.wParam == timer) {
            if (inside) SendMessageW(hwnd, scrollMsg, scrollCode, 0);
            timer = SetTimer(nullptr, timer, 50, nullptr);
            continue;
        }
        TranslateMessage(&m);
        DispatchMessageW(&m);
    }

    if (timer) KillTimer(nullptr, timer);
    if (GetCapture() == hwnd) ReleaseCapture();

    POINT pt;
    GetCursorPos(&pt);
    g_pressId = -1;
    g_pressHwnd = nullptr;
    g_tracking = false;
    t_hotHwnd = nullptr;
    t_hotHt = 0;

    if (t.kind == 1 && IsWindow(hwnd))
        SendMessageW(hwnd, scrollMsg, SB_ENDSCROLL, 0);
    DrawAll(hwnd, what, false);

    if (t.kind == 0 && inside && !cancelled && IsWindow(hwnd)) {
        WPARAM sc = SC_CLOSE;
        if (t.idx == 2) sc = IsIconic(hwnd) ? SC_RESTORE : SC_MINIMIZE;
        else if (t.idx == 3) sc = IsZoomed(hwnd) ? SC_RESTORE : SC_MAXIMIZE;
        else if (t.idx == 4) sc = SC_CONTEXTHELP;
        SendMessageW(hwnd, WM_SYSCOMMAND, sc, MAKELPARAM(pt.x, pt.y));
    }
}

static bool HandleNcButtonDown(HWND hwnd, WPARAM ht, LPARAM lParam)
{
    if (!IsWindow(hwnd) || !SameThread(hwnd)) return false;
    POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
    Target t = {};
    int pressId = -1;

    switch (ht) {
    case HTMINBUTTON: t = {0, 2, 0}; pressId = 2; break;
    case HTMAXBUTTON: t = {0, 3, 0}; pressId = 3; break;
    case HTHELP:      t = {0, 4, 0}; pressId = 4; break;
    case HTCLOSE:     t = {0, 5, 0}; pressId = 5; break;
    case HTVSCROLL:
    case HTHSCROLL: {
        int obj = (ht == HTVSCROLL) ? OBJID_VSCROLL : OBJID_HSCROLL;
        RECT a[2];
        bool d[2];
        if (!ScrollArrows(hwnd, obj, a, d)) return false;
        int i = PtInRect(&a[0], pt) ? 0 : (PtInRect(&a[1], pt) ? 1 : -1);
        if (i < 0 || d[i]) return false;
        t = {1, i, obj};
        pressId = ((ht == HTVSCROLL) ? 10 : 12) + i;
        break;
    }
    default:
        return false;
    }

    if (t.kind == 0) {
        RECT r;
        bool dis;
        if (!GetTargetRect(hwnd, t, &r, &dis)) return false;
        if (dis) return true;   // grayed button: swallow the click, the kernel would only draw garbage
    }
    Track(hwnd, t, pressId);
    return true;
}

// Minimized windows: the kernel's hit-test knows nothing about our button layout,
// so we test the cursor against our own rectangles.
static bool HandleIconicNcDown(HWND hwnd, ULONG_PTR& wParam, LPARAM lParam)
{
    if (!IsWindow(hwnd) || !SameThread(hwnd)) return false;
    if ((GetWindowLongW(hwnd, GWL_STYLE) & WS_CAPTION) != WS_CAPTION) return false;

    CapBtns b;
    if (!LayoutCaption(hwnd, b)) return false;

    POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
    int hit = -1;
    for (int i = 2; i <= 5; i++)
        if (b.vis[i] && PtInRect(&b.btn[i], pt)) hit = i;

    if (hit < 0) {
        // Kernel thinks it is a button, but our buttons are elsewhere: treat as caption.
        if (wParam == HTMINBUTTON || wParam == HTMAXBUTTON || wParam == HTCLOSE || wParam == HTHELP)
            wParam = HTCAPTION;
        return false;
    }
    if (b.dis[hit]) return true;

    Target t = {0, hit, 0};
    Track(hwnd, t, hit);
    return true;
}

// ---------- non-client painting without kernel buttons ----------

static HRGN BaseRegion(const RECT& wr, ULONG_PTR wParam)
{
    HRGN base = CreateRectRgn(0, 0, 0, 0);
    if (!base) return nullptr;
    bool ok = false;
    if (wParam > 1 && wParam != (ULONG_PTR)-1 && GetObjectType((HGDIOBJ)wParam) == OBJ_REGION)
        ok = CombineRgn(base, (HRGN)wParam, nullptr, RGN_COPY) != ERROR;
    if (!ok) SetRectRgn(base, wr.left, wr.top, wr.right, wr.bottom);
    return base;
}

static HRGN FrameRegion(HWND hwnd, const CapGeom& g)
{
    const RECT& wr = g.wr;
    int f = FrameSize(hwnd, false);
    int capB = g.cap.bottom + 1;
    if (capB > wr.bottom) capB = wr.bottom;
    HRGN rgn = CreateRectRgn(wr.left, wr.top, wr.right, capB);
    HRGN part = CreateRectRgn(0, 0, 0, 0);
    if (rgn && part) {
        SetRectRgn(part, wr.left, wr.top, wr.left + f, wr.bottom);
        CombineRgn(rgn, rgn, part, RGN_OR);
        SetRectRgn(part, wr.right - f, wr.top, wr.right, wr.bottom);
        CombineRgn(rgn, rgn, part, RGN_OR);
        SetRectRgn(part, wr.left, wr.bottom - f, wr.right, wr.bottom);
        CombineRgn(rgn, rgn, part, RGN_OR);
    }
    if (part) DeleteObject(part);
    return rgn;
}

static ULONG_PTR NcPaintClipped(const CapGeom& g, HRGN base, ULONG_PTR hwnd, ULONG_PTR lParam,
                                ULONG_PTR resultInfo, ULONG_PTR type, ULONG_PTR ansi)
{
    HRGN s = CreateRectRgnIndirect(&g.strip);
    if (s) {
        CombineRgn(base, base, s, RGN_DIFF);
        DeleteObject(s);
    }
    ULONG_PTR r = g_origMsg(hwnd, WM_NCPAINT, (ULONG_PTR)base, lParam, resultInfo, type, ansi);
    if (GetObjectType(base) == OBJ_REGION) DeleteObject(base);
    return r;
}

// ---------- hover tracking ----------

static void ResetHot(HWND h)
{
    if (t_hotHwnd == h) {
        t_hotHwnd = nullptr;
        t_hotHt = 0;
    }
}

// Отслеживает ВСЕ события наведения на рамку и тайтлбар, а не только на сами кнопки.
// Это решает баг, когда ядро Windows после разворачивания/запуска обновляет
// своё состояние наведения и рисует мусор.
static void UpdateHot(HWND h, ULONG_PTR ht)
{
    int newHt = (int)ht;
    // Любой HitTest больше HTCLIENT (например, HTCAPTION, HTCLOSE, HTTOP) считается рамкой
    bool isNC = (newHt > HTCLIENT);

    if (!isNC) {
        if (t_hotHwnd == h) {
            ResetHot(h);
            MarkDirty(h, true);
        }
        return;
    }

    if (t_hotHwnd == h && t_hotHt == newHt) return;

    if (t_hotHwnd && t_hotHwnd != h) MarkDirty(t_hotHwnd, true);

    t_hotHwnd = h;
    t_hotHt = newHt;
    MarkDirty(h, true);
}

// ---------- hooks ----------

static ULONG_PTR NTAPI Hook_MessageCall(ULONG_PTR hwnd, ULONG_PTR msg, ULONG_PTR wParam,
                                        ULONG_PTR lParam, ULONG_PTR resultInfo,
                                        ULONG_PTR type, ULONG_PTR ansi)
{
    UINT m = (UINT)msg;
    HWND h = (HWND)hwnd;

    if ((m == WM_NCLBUTTONDOWN || m == WM_NCLBUTTONDBLCLK) && !g_tracking && !t_busy) {
        bool handled = IsIconic(h) ? HandleIconicNcDown(h, wParam, (LPARAM)lParam)
                                   : HandleNcButtonDown(h, (WPARAM)wParam, (LPARAM)lParam);
        if (handled) {
            ResetHot(h);
            MarkDirty(h, true);
            return 0;
        }
    }

    if (m == WM_NCDESTROY) {
        RemovePropW(h, kActiveProp);
        ResetHot(h);
    }

    if ((type & 0xFFFF) == cfg::fnidDefWindowProc && !t_busy &&
        (m == WM_NCPAINT || m == WM_NCACTIVATE)) {
        CapGeom g;
        if (GetCapGeom(h, g) && g.hasStrip) {
            if (m == WM_NCPAINT) {
                HRGN base = BaseRegion(g.wr, wParam);
                if (base) {
                    ULONG_PTR ret = NcPaintClipped(g, base, hwnd, lParam, resultInfo, type, ansi);
                    DrawAll(h, DRAW_ALL, true);
                    return ret;
                }
            } else {
                ULONG_PTR ret = g_origMsg(hwnd, msg, wParam, (ULONG_PTR)-1, resultInfo, type, ansi);
                SetPropW(h, kActiveProp, (HANDLE)(ULONG_PTR)(wParam ? 1 : 2));
                HRGN base = FrameRegion(h, g);
                if (base)
                    NcPaintClipped(g, base, hwnd, 0, resultInfo, type, ansi);
                else
                    g_origMsg(hwnd, WM_NCPAINT, 1, 0, resultInfo, type, ansi);
                DrawAll(h, DRAW_ALL, true);
                return ret;
            }
        }
    }

    ULONG_PTR ret = g_origMsg(hwnd, msg, wParam, lParam, resultInfo, type, ansi);

    switch (m) {
    case WM_NCACTIVATE:
        SetPropW(h, kActiveProp, (HANDLE)(ULONG_PTR)(wParam ? 1 : 2));
        DrawAll(h, DRAW_ALL, true, true);
        break;

    case WM_WINDOWPOSCHANGED:
    case WM_DPICHANGED:
        ResetHot(h);
        DrawAll(h, DRAW_ALL, true);
        break;

    case WM_SIZE:
        ResetHot(h);
        break;

    case WM_NCPAINT:
    case WM_SETTEXT:
    case WM_ENABLE:
        DrawAll(h, DRAW_ALL, true);
        break;

    case WM_PRINT:
        if (!t_busy && wParam && (lParam & PRF_NONCLIENT) && IsWindow(h) && SameThread(h) &&
            !IsIconic(h)) {
            LONG style = GetWindowLongW(h, GWL_STYLE);
            RECT wr;
            if ((style & (WS_VSCROLL | WS_HSCROLL)) && GetWindowRect(h, &wr)) {
                BusyScope busy;
                DrawScrollBars(h, (HDC)wParam, wr, style, false);
            }
        }
        break;

    case WM_NCLBUTTONDOWN:
    case WM_NCLBUTTONUP:
    case WM_NCLBUTTONDBLCLK:
    case WM_NCRBUTTONUP:
    case WM_SYSCOMMAND:
    case WM_EXITSIZEMOVE:
    case WM_CAPTURECHANGED:
        MarkDirty(h, true);
        break;

    case WM_NCHITTEST:
        UpdateHot(h, ret);
        break;

    case WM_NCMOUSEMOVE:
        UpdateHot(h, wParam);
        break;

    case WM_NCMOUSELEAVE:
        if (t_hotHwnd == h) {
            ResetHot(h);
            MarkDirty(h, true);
        }
        break;
    }
    return ret;
}

static ULONG_PTR NTAPI Hook_GetMessage(ULONG_PTR a, ULONG_PTR b, ULONG_PTR c, ULONG_PTR d)
{
    FlushDirty();
    return g_origGetMessage(a, b, c, d);
}

static ULONG_PTR NTAPI Hook_PeekMessage(ULONG_PTR a, ULONG_PTR b, ULONG_PTR c, ULONG_PTR d,
                                        ULONG_PTR e)
{
    FlushDirty();
    return g_origPeekMessage(a, b, c, d, e);
}

static ULONG_PTR NTAPI Hook_SetScrollInfo(ULONG_PTR hwnd, ULONG_PTR bar, ULONG_PTR psi, ULONG_PTR redraw)
{
    ULONG_PTR ret = g_origSetScrollInfo(hwnd, bar, psi, redraw);
    if ((int)bar != SB_CTL) DrawAll((HWND)hwnd, DRAW_SCROLL, false);
    return ret;
}

static ULONG_PTR NTAPI Hook_EnableScrollBar(ULONG_PTR hwnd, ULONG_PTR flags, ULONG_PTR arrows)
{
    ULONG_PTR ret = g_origEnableScrollBar(hwnd, flags, arrows);
    DrawAll((HWND)hwnd, DRAW_SCROLL, false);
    return ret;
}

static bool HookExport(HMODULE m, const char* name, void* hook, void** orig)
{
    void* p = (void*)GetProcAddress(m, name);
    if (!p) { Wh_Log(L"missing: %hs", name); return false; }
    if (!Wh_SetFunctionHook(p, hook, orig)) { Wh_Log(L"hook failed: %hs", name); return false; }
    return true;
}

BOOL Wh_ModInit()
{
    if (IsThemeActive()) return FALSE;
    HMODULE w = GetModuleHandleW(L"win32u.dll");
    if (!w) return FALSE;
    if (!HookExport(w, "NtUserMessageCall", (void*)Hook_MessageCall, (void**)&g_origMsg))
        return FALSE;
    HookExport(w, "NtUserSetScrollInfo", (void*)Hook_SetScrollInfo, (void**)&g_origSetScrollInfo);
    HookExport(w, "NtUserEnableScrollBar", (void*)Hook_EnableScrollBar, (void**)&g_origEnableScrollBar);

    bool gm = HookExport(w, "NtUserGetMessage", (void*)Hook_GetMessage, (void**)&g_origGetMessage);
    bool pm = HookExport(w, "NtUserPeekMessage", (void*)Hook_PeekMessage, (void**)&g_origPeekMessage);
    g_flushHooked = gm || pm;
    return TRUE;
}
