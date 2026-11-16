// ==WindhawkMod==
// @id              copy-progress-styles
// @name            File Copy Progress Bar Styles
// @description     Restyles the progress bar in the Windows copy/move/delete dialog (12 styles)
// @version         1.1.2
// @author          Pixeliota
// @github          https://github.com/Pixeliota
// @include         explorer.exe
// @license         MIT
// @compilerOptions -lgdi32 -lmsimg32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# File Copy Progress Bar Styles

Restyles the progress bar shown in the Windows file copy / move / delete dialog.

**Classic styles** (use the accent color)
* **flat** - solid color
* **gradient** - dark-to-light gradient with a top highlight
* **rainbow** - multi-color gradient
* **striped** - candy stripes
* **neon** - dark track with a glowing core
* **segmented** - retro blocks

**Designed styles** (fixed colors and their own track)
* **forest** - dark green timeline with stepped bands and a diamond knob
* **tricolor** - pink / yellow / blue segments
* **dots** - blue gradient with dot markers
* **aurora** - cyan-to-lilac gradient with an end marker and tick track
* **ember** - orange diagonal stripes on a striped gray track
* **soft** - soft green bar with a subtle bottom shade

Paused and error states automatically switch to amber and red.

**Notes**
* The mod hooks the theme drawing of progress bars inside explorer.exe, so by default
  it also restyles the drive usage bars in This PC. Turn on "Only style the copy dialog"
  to leave those alone.
* Windows itself turns a drive bar red when the drive has under 10% free.
* Setting changes show up the next time the bar repaints.
* If nothing changes, enable "Log draw calls" and check the Windhawk log.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- style: gradient
  $name: Style
  $options:
  - flat: Flat solid color
  - gradient: Accent gradient
  - rainbow: Rainbow
  - striped: Candy stripes
  - neon: Neon glow
  - segmented: Retro blocks
  - forest: Forest timeline
  - tricolor: Tri-color segments
  - dots: Blue dots
  - aurora: Aurora with ticks
  - ember: Ember stripes
  - soft: Soft green
- accentColor: "#06B025"
  $name: Accent color (hex) - used by the classic styles
- trackColor: "#E6E6E6"
  $name: Track color (hex) - used by classic styles, tricolor and aurora
- cornerRadius: 3
  $name: Corner radius in pixels (0 = square)
- hideOverlay: true
  $name: Hide the default moving glow
- skipExplorerWindows: false
  $name: Only style the copy dialog (leave drive bars in This PC alone)
- debugLog: false
  $name: Log draw calls (for troubleshooting)
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <uxtheme.h>

#include <algorithm>
#include <cwctype>
#include <mutex>
#include <string>
#include <unordered_map>

// Progress bar theme parts / states (defined locally to avoid header dependency)
constexpr int kPP_BAR = 1;
constexpr int kPP_FILL = 5;
constexpr int kPP_PULSEOVERLAY = 7;
constexpr int kPP_MOVEOVERLAY = 8;
constexpr int kPBFS_ERROR = 2;
constexpr int kPBFS_PAUSED = 3;

// DTBGOPTS flags
constexpr DWORD kDTBG_CLIPRECT = 0x00000001;
constexpr DWORD kDTBG_COMPUTINGREGION = 0x00000010;

struct DtbgOptsLite {
    DWORD dwSize;
    DWORD dwFlags;
    RECT rcClip;
};

enum Style {
    S_FLAT,
    S_GRADIENT,
    S_RAINBOW,
    S_STRIPED,
    S_NEON,
    S_SEGMENTED,
    S_FOREST,
    S_TRICOLOR,
    S_DOTS,
    S_AURORA,
    S_EMBER,
    S_SOFT
};

struct Config {
    Style style = S_GRADIENT;
    COLORREF accent = RGB(6, 176, 37);
    COLORREF track = RGB(230, 230, 230);
    int radius = 3;
    bool hideOverlay = true;
    bool skipExplorer = false;
    bool debug = false;
} g;

std::mutex g_mutex;
std::unordered_map<HTHEME, HWND> g_progressThemes;

// ---------------------------------------------------------------- helpers

static int Imax(int a, int b) { return a > b ? a : b; }

static COLORREF ParseColor(PCWSTR s, COLORREF def) {
    if (!s || !*s) return def;
    if (*s == L'#') s++;
    wchar_t* end = nullptr;
    unsigned long v = wcstoul(s, &end, 16);
    if (end == s) return def;
    return RGB((v >> 16) & 0xFF, (v >> 8) & 0xFF, v & 0xFF);
}

static COLORREF Mix(COLORREF a, COLORREF b, int pct) {
    auto ch = [&](int x, int y) { return x + (y - x) * pct / 100; };
    return RGB(ch(GetRValue(a), GetRValue(b)), ch(GetGValue(a), GetGValue(b)),
               ch(GetBValue(a), GetBValue(b)));
}

static COLORREF Lighten(COLORREF c, int pct) { return Mix(c, RGB(255, 255, 255), pct); }
static COLORREF Darken(COLORREF c, int pct) { return Mix(c, RGB(0, 0, 0), pct); }

static void Fill(HDC hdc, const RECT& r, COLORREF c) {
    if (r.right <= r.left || r.bottom <= r.top) return;
    SetDCBrushColor(hdc, c);
    FillRect(hdc, &r, (HBRUSH)GetStockObject(DC_BRUSH));
}

static TRIVERTEX TV(LONG x, LONG y, COLORREF c) {
    TRIVERTEX v;
    v.x = x;
    v.y = y;
    v.Red = (COLOR16)(GetRValue(c) << 8);
    v.Green = (COLOR16)(GetGValue(c) << 8);
    v.Blue = (COLOR16)(GetBValue(c) << 8);
    v.Alpha = 0;
    return v;
}

static void Grad(HDC hdc, const RECT& r, COLORREF c1, COLORREF c2, bool vertical) {
    if (r.right <= r.left || r.bottom <= r.top) return;
    TRIVERTEX v[2] = {TV(r.left, r.top, c1), TV(r.right, r.bottom, c2)};
    GRADIENT_RECT gr = {0, 1};
    GradientFill(hdc, v, 2, &gr, 1, vertical ? GRADIENT_FILL_RECT_V : GRADIENT_FILL_RECT_H);
}

// Multi-stop horizontal gradient. pm = position in permille (0..1000).
struct Stop {
    int pm;
    COLORREF c;
};

static COLORREF ColorAt(const Stop* s, int n, int pm) {
    if (pm <= s[0].pm) return s[0].c;
    for (int i = 0; i < n - 1; i++) {
        if (pm <= s[i + 1].pm) {
            int span = s[i + 1].pm - s[i].pm;
            if (span <= 0) return s[i + 1].c;
            return Mix(s[i].c, s[i + 1].c, (pm - s[i].pm) * 100 / span);
        }
    }
    return s[n - 1].c;
}

static void GradStops(HDC hdc, const RECT& r, const Stop* s, int n) {
    const int w = r.right - r.left;
    if (w <= 0 || n < 2) return;
    for (int i = 0; i < n - 1; i++) {
        RECT seg = r;
        seg.left = r.left + (int)((long long)w * s[i].pm / 1000);
        seg.right = (i == n - 2) ? r.right : r.left + (int)((long long)w * s[i + 1].pm / 1000);
        Grad(hdc, seg, s[i].c, s[i + 1].c, false);
    }
}

// Diagonal stripes ("/" direction). Each stripe is a parallelogram.
// If stops is given, each stripe is the gradient color at its position mixed toward white.
static void Stripes(HDC hdc, const RECT& r, int sw, int period, int x0, int slant,
                    const Stop* stops, int n, COLORREF flat, int whitePct) {
    const int w = r.right - r.left;
    if (w <= 0 || period <= 0 || sw <= 0) return;
    SelectObject(hdc, GetStockObject(DC_BRUSH));
    SelectObject(hdc, GetStockObject(NULL_PEN));
    for (int x = x0 - period; x < r.right; x += period) {
        COLORREF base = flat;
        if (stops && n > 0) {
            int cx = x + (sw + slant) / 2;
            long long pm = (long long)(cx - r.left) * 1000 / w;
            if (pm < 0) pm = 0;
            if (pm > 1000) pm = 1000;
            base = ColorAt(stops, n, (int)pm);
        }
        SetDCBrushColor(hdc, Mix(base, RGB(255, 255, 255), whitePct));
        POINT pts[4] = {{x, r.bottom}, {x + sw, r.bottom}, {x + sw + slant, r.top},
                        {x + slant, r.top}};
        Polygon(hdc, pts, 4);
    }
}

static void Dots(HDC hdc, const RECT& r, COLORREF c) {
    const int h = r.bottom - r.top;
    const int rad = Imax(2, h * 21 / 100);
    const int spacing = Imax(8, h * 244 / 100);
    const int first = Imax(rad + 2, h * 134 / 100);
    const int cy = r.top + h / 2;
    SelectObject(hdc, GetStockObject(DC_BRUSH));
    SelectObject(hdc, GetStockObject(NULL_PEN));
    SetDCBrushColor(hdc, c);
    for (int x = r.left + first; x < r.right + rad; x += spacing) {
        Ellipse(hdc, x - rad, cy - rad, x + rad + 1, cy + rad + 1);
    }
}

static void ClipRounded(HDC hdc, const RECT& r, int radius) {
    if (radius <= 0) return;
    // Clip regions use device coordinates, but r is in the DC's logical coordinates.
    // Buffered/offset DCs (used when Explorer paints many items at once) have a viewport
    // origin, so convert first or the region lands in the wrong place and hides the bar.
    POINT pts[2] = {{r.left, r.top}, {r.right + 1, r.bottom + 1}};
    LPtoDP(hdc, pts, 2);
    HRGN rgn = CreateRoundRectRgn(pts[0].x, pts[0].y, pts[1].x, pts[1].y, radius * 2,
                                  radius * 2);
    if (rgn) {
        ExtSelectClipRgn(hdc, rgn, RGN_AND);
        DeleteObject(rgn);
    }
}

// Saves the DC and restricts drawing to the requested clip rectangle and rounded shape.
// Honoring the clip rectangle is what keeps partial repaints (for example when the dialog
// is resized) from overwriting parts of the bar that were not meant to be redrawn.
static int BeginClip(HDC hdc, const RECT& clip, const RECT& shape, int radius) {
    int saved = SaveDC(hdc);
    IntersectClipRect(hdc, clip.left, clip.top, clip.right, clip.bottom);
    ClipRounded(hdc, shape, radius);
    return saved;
}

static bool ContainsProgress(LPCWSTR s) {
    if (!s) return false;
    std::wstring t(s);
    std::transform(t.begin(), t.end(), t.begin(), [](wchar_t c) { return (wchar_t)towlower(c); });
    return t.find(L"progress") != std::wstring::npos;
}

static bool IsProgressTheme(HTHEME t) {
    if (!t) return false;
    std::lock_guard<std::mutex> lock(g_mutex);
    return g_progressThemes.count(t) != 0;
}

static BOOL CALLBACK EnumThreadWndProc(HWND h, LPARAM lp) {
    wchar_t cls[64] = {};
    GetClassNameW(h, cls, 64);
    if (wcscmp(cls, L"OperationStatusWindow") == 0) {
        *(bool*)lp = true;
        return FALSE;
    }
    return TRUE;
}

// True if the calling thread owns a copy/move progress dialog window.
static bool ThreadHostsCopyDialog() {
    bool found = false;
    EnumThreadWindows(GetCurrentThreadId(), EnumThreadWndProc, (LPARAM)&found);
    return found;
}

// Decides whether this progress bar should be restyled. With the "skipExplorerWindows"
// setting on, only bars that belong to the copy dialog are styled.
static bool ShouldStyle(HTHEME t, HDC hdc) {
    if (!g.skipExplorer) return true;

    HWND w = nullptr;
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        auto it = g_progressThemes.find(t);
        if (it != g_progressThemes.end()) w = it->second;
    }
    if (!w || !IsWindow(w)) w = WindowFromDC(hdc);

    if (w) {
        HWND root = GetAncestor(w, GA_ROOT);
        wchar_t cls[64] = {};
        GetClassNameW(root ? root : w, cls, 64);
        if (g.debug) Wh_Log(L"Progress bar window class: %s", cls);
        if (wcscmp(cls, L"CabinetWClass") == 0) return false;
        if (wcscmp(cls, L"OperationStatusWindow") == 0) return true;
    }

    // Window unknown (buffered painting): fall back to the thread that is painting.
    return ThreadHostsCopyDialog();
}

// ---------------------------------------------------------------- settings

static void LoadSettings() {
    PCWSTR st = Wh_GetStringSetting(L"style");
    if (st) {
        if (wcscmp(st, L"flat") == 0) g.style = S_FLAT;
        else if (wcscmp(st, L"rainbow") == 0) g.style = S_RAINBOW;
        else if (wcscmp(st, L"striped") == 0) g.style = S_STRIPED;
        else if (wcscmp(st, L"neon") == 0) g.style = S_NEON;
        else if (wcscmp(st, L"segmented") == 0) g.style = S_SEGMENTED;
        else if (wcscmp(st, L"forest") == 0) g.style = S_FOREST;
        else if (wcscmp(st, L"tricolor") == 0) g.style = S_TRICOLOR;
        else if (wcscmp(st, L"dots") == 0) g.style = S_DOTS;
        else if (wcscmp(st, L"aurora") == 0) g.style = S_AURORA;
        else if (wcscmp(st, L"ember") == 0) g.style = S_EMBER;
        else if (wcscmp(st, L"soft") == 0) g.style = S_SOFT;
        else g.style = S_GRADIENT;
        Wh_FreeStringSetting(st);
    }

    PCWSTR ac = Wh_GetStringSetting(L"accentColor");
    g.accent = ParseColor(ac, RGB(6, 176, 37));
    Wh_FreeStringSetting(ac);

    PCWSTR tr = Wh_GetStringSetting(L"trackColor");
    g.track = ParseColor(tr, RGB(230, 230, 230));
    Wh_FreeStringSetting(tr);

    g.radius = Wh_GetIntSetting(L"cornerRadius");
    if (g.radius < 0) g.radius = 0;
    if (g.radius > 12) g.radius = 12;
    g.hideOverlay = Wh_GetIntSetting(L"hideOverlay") != 0;
    g.skipExplorer = Wh_GetIntSetting(L"skipExplorerWindows") != 0;
    g.debug = Wh_GetIntSetting(L"debugLog") != 0;
}

// ---------------------------------------------------------------- palettes

static const Stop kRainbow[] = {{0, RGB(255, 59, 48)},   {200, RGB(255, 149, 0)},
                                {400, RGB(255, 214, 10)}, {600, RGB(52, 199, 89)},
                                {800, RGB(10, 132, 255)}, {1000, RGB(175, 82, 222)}};
static const Stop kDots[] = {{0, RGB(43, 167, 233)},
                             {190, RGB(43, 150, 233)},
                             {560, RGB(43, 107, 233)},
                             {1000, RGB(43, 49, 233)}};
static const Stop kAurora[] = {{0, RGB(79, 222, 233)},   {140, RGB(71, 205, 233)},
                               {410, RGB(51, 162, 233)}, {520, RGB(43, 144, 233)},
                               {690, RGB(107, 160, 239)}, {1000, RGB(234, 192, 251)}};
static const Stop kEmber[] = {{0, RGB(219, 46, 0)},
                              {250, RGB(226, 62, 10)},
                              {750, RGB(244, 105, 36)},
                              {1000, RGB(255, 129, 51)}};
static const COLORREF kForest[6] = {RGB(163, 220, 107), RGB(125, 180, 85), RGB(113, 166, 78),
                                    RGB(97, 149, 69),   RGB(78, 129, 58),  RGB(54, 103, 44)};
static const int kForestEnd[6] = {370, 520, 630, 750, 870, 1000};

// ---------------------------------------------------------------- painting

static void DrawTrack(HDC hdc, RECT r, const RECT& clip) {
    const int h = r.bottom - r.top;
    int saved = BeginClip(hdc, clip, r, g.radius);

    switch (g.style) {
        case S_NEON:
            Fill(hdc, r, RGB(28, 30, 38));
            break;

        case S_FOREST:
            Fill(hdc, r, RGB(25, 72, 27));
            break;

        case S_DOTS:
            Fill(hdc, r, RGB(204, 217, 245));
            Dots(hdc, r, RGB(240, 251, 255));
            break;

        case S_AURORA: {
            Fill(hdc, r, Lighten(g.track, 60));
            const int tw = Imax(1, h * 115 / 1000);
            const int period = Imax(3, h * 328 / 1000);
            for (int x = r.left; x < r.right; x += period) {
                RECT t = {x, r.top, x + tw, r.bottom};
                Fill(hdc, t, RGB(219, 219, 221));
            }
            break;
        }

        case S_EMBER:
            Fill(hdc, r, RGB(198, 198, 198));
            Stripes(hdc, r, h * 85 / 100, h * 193 / 100, r.left - h * 8 / 100, h, nullptr, 0,
                    RGB(198, 198, 198), 25);
            break;

        case S_SOFT: {
            const COLORREF base = RGB(198, 205, 211);
            Fill(hdc, r, base);
            RECT lower = r;
            lower.top = r.bottom - Imax(1, h * 47 / 100);
            Fill(hdc, lower, Darken(base, 7));
            break;
        }

        default:
            Fill(hdc, r, g.track);
            break;
    }

    RestoreDC(hdc, saved);
}

static void DrawFill(HDC hdc, RECT r, const RECT& clip, int state) {
    const bool normal = (state != kPBFS_ERROR && state != kPBFS_PAUSED);
    COLORREF c = g.accent;
    if (state == kPBFS_ERROR) c = RGB(218, 38, 38);
    else if (state == kPBFS_PAUSED) c = RGB(242, 174, 0);

    const int w = r.right - r.left;
    const int h = r.bottom - r.top;

    // Palette used by the gradient-based styles when paused / error.
    Stop st[2] = {{0, Darken(c, 25)}, {1000, Lighten(c, 25)}};
    auto gradient = [&](const RECT& rr, const Stop* s, int n) {
        if (normal) GradStops(hdc, rr, s, n);
        else GradStops(hdc, rr, st, 2);
    };

    int saved = BeginClip(hdc, clip, r, Imax(0, g.radius - 1));

    switch (g.style) {
        case S_FLAT:
            Fill(hdc, r, c);
            break;

        case S_GRADIENT: {
            Grad(hdc, r, Darken(c, 30), Lighten(c, 35), false);
            RECT top = r;
            top.bottom = top.top + 1;
            Fill(hdc, top, Lighten(c, 55));
            break;
        }

        case S_RAINBOW:
            gradient(r, kRainbow, 6);
            break;

        case S_STRIPED: {
            Fill(hdc, r, c);
            const int sw = Imax(6, h * 3 / 4);
            Stripes(hdc, r, sw, sw * 2, r.left - h, h, nullptr, 0, c, 30);
            break;
        }

        case S_NEON: {
            Fill(hdc, r, Darken(c, 45));  // outer halo
            RECT mid = r;
            InflateRect(&mid, 0, -Imax(1, h / 5));
            Fill(hdc, mid, c);
            RECT core = r;
            InflateRect(&core, 0, -Imax(2, h * 3 / 8));
            Fill(hdc, core, Lighten(c, 65));
            break;
        }

        case S_SEGMENTED: {
            const int bw = Imax(5, h * 3 / 4);
            const int gap = 2;
            for (int x = r.left; x < r.right; x += bw + gap) {
                RECT b = {x, r.top, (x + bw < r.right) ? x + bw : r.right, r.bottom};
                Fill(hdc, b, c);
            }
            break;
        }

        case S_FOREST: {
            Fill(hdc, r, RGB(25, 72, 27));
            const int ih = Imax(2, h * 31 / 100);
            const int top = r.top + (h - ih) / 2;
            int prev = r.left;
            for (int i = 0; i < 6; i++) {
                int end = (i == 5) ? r.right
                                   : r.left + (int)((long long)w * kForestEnd[i] / 1000);
                COLORREF col = normal ? kForest[i]
                                      : Mix(Lighten(c, 35), Darken(c, 35), i * 20);
                RECT seg = {prev, top, end, top + ih};
                Fill(hdc, seg, col);
                prev = end;
            }
            const int d = Imax(3, h * 38 / 100);
            if (w > 2 * d + 2) {
                const int cx = r.right - d - 1;
                const int cy = r.top + h / 2;
                SelectObject(hdc, GetStockObject(DC_BRUSH));
                SelectObject(hdc, GetStockObject(NULL_PEN));
                SetDCBrushColor(hdc, normal ? RGB(179, 237, 116) : Lighten(c, 50));
                POINT p[4] = {{cx, cy - d}, {cx + d, cy}, {cx, cy + d}, {cx - d, cy}};
                Polygon(hdc, p, 4);
            }
            break;
        }

        case S_TRICOLOR: {
            Fill(hdc, r, g.track);
            const COLORREF c1 = normal ? RGB(255, 164, 216) : Darken(c, 12);
            const COLORREF c2 = normal ? RGB(255, 209, 80) : c;
            const COLORREF c3 = normal ? RGB(139, 189, 239) : Lighten(c, 25);
            const int gap = Imax(2, h / 3);
            if (w < gap * 3 + 12) {
                Fill(hdc, r, c1);
                break;
            }
            const int usable = w - 2 * gap;
            const int a = usable * 55 / 100;
            const int b = usable * 26 / 100;
            RECT s1 = {r.left, r.top, r.left + a, r.bottom};
            RECT s2 = {s1.right + gap, r.top, s1.right + gap + b, r.bottom};
            RECT s3 = {s2.right + gap, r.top, r.right, r.bottom};
            Fill(hdc, s1, c1);
            Fill(hdc, s2, c2);
            Fill(hdc, s3, c3);
            break;
        }

        case S_DOTS:
            gradient(r, kDots, 4);
            Dots(hdc, r, normal ? RGB(0, 46, 147) : Darken(c, 55));
            break;

        case S_AURORA: {
            const int mw = Imax(2, h * 115 / 1000);
            const int gap = Imax(2, h * 20 / 100);
            Fill(hdc, r, Lighten(g.track, 60));
            RECT gr = r;
            gr.right = r.right - gap - mw;
            gradient(gr, kAurora, 6);
            RECT mk = {r.right - mw, r.top, r.right, r.bottom};
            Fill(hdc, mk, normal ? RGB(43, 111, 233) : Darken(c, 35));
            break;
        }

        case S_EMBER: {
            gradient(r, kEmber, 4);
            const Stop* ps = normal ? kEmber : st;
            const int pn = normal ? 4 : 2;
            Stripes(hdc, r, h * 85 / 100, h * 193 / 100, r.left - h * 8 / 100, h, ps, pn, 0,
                    25);
            break;
        }

        case S_SOFT: {
            const COLORREF base = normal ? RGB(104, 194, 107) : c;
            Fill(hdc, r, base);
            RECT lower = r;
            lower.top = r.bottom - Imax(1, h * 47 / 100);
            Fill(hdc, lower, Darken(base, 8));
            break;
        }
    }

    RestoreDC(hdc, saved);
}

// Returns true if we fully handled the draw call.
static bool PaintProgress(HDC hdc, int part, int state, const RECT* prc, const RECT* pclip) {
    if (!prc) return false;
    RECT r = *prc;
    if (r.right <= r.left || r.bottom <= r.top) return false;

    const bool overlay = (part == kPP_PULSEOVERLAY || part == kPP_MOVEOVERLAY);
    const bool handled = (part == kPP_BAR || part == kPP_FILL || (overlay && g.hideOverlay));
    if (!handled) return false;

    if (g.debug) {
        Wh_Log(L"Progress draw part=%d state=%d rc=%ld,%ld,%ld,%ld clip=%d", part, state,
               r.left, r.top, r.right, r.bottom, pclip ? 1 : 0);
    }

    // Respect the caller's clip rectangle, like the stock theme does.
    RECT clip = r;
    if (pclip && !IntersectRect(&clip, &r, pclip)) clip = r;  // odd clip rect: ignore it instead of drawing nothing

    if (overlay) return true;  // swallow the stock glow animation

    if (part == kPP_BAR) DrawTrack(hdc, r, clip);
    else DrawFill(hdc, r, clip, state);
    return true;
}

// ---------------------------------------------------------------- hooks

using OpenThemeData_t = HTHEME(WINAPI*)(HWND, LPCWSTR);
using OpenThemeDataForDpi_t = HTHEME(WINAPI*)(HWND, LPCWSTR, UINT);
using OpenThemeDataEx_t = HTHEME(WINAPI*)(HWND, LPCWSTR, DWORD);
using CloseThemeData_t = HRESULT(WINAPI*)(HTHEME);
using DrawThemeBackground_t = HRESULT(WINAPI*)(HTHEME, HDC, int, int, LPCRECT, LPCRECT);
using DrawThemeBackgroundEx_t = HRESULT(WINAPI*)(HTHEME, HDC, int, int, LPCRECT, const void*);

OpenThemeData_t OpenThemeData_Original;
OpenThemeDataForDpi_t OpenThemeDataForDpi_Original;
OpenThemeDataEx_t OpenThemeDataEx_Original;
CloseThemeData_t CloseThemeData_Original;
DrawThemeBackground_t DrawThemeBackground_Original;
DrawThemeBackgroundEx_t DrawThemeBackgroundEx_Original;

static void Track(HTHEME t, LPCWSTR cls, HWND hwnd) {
    if (t && ContainsProgress(cls)) {
        std::lock_guard<std::mutex> lock(g_mutex);
        g_progressThemes[t] = hwnd;
    }
}

HTHEME WINAPI OpenThemeData_Hook(HWND hwnd, LPCWSTR cls) {
    HTHEME t = OpenThemeData_Original(hwnd, cls);
    Track(t, cls, hwnd);
    return t;
}

HTHEME WINAPI OpenThemeDataForDpi_Hook(HWND hwnd, LPCWSTR cls, UINT dpi) {
    HTHEME t = OpenThemeDataForDpi_Original(hwnd, cls, dpi);
    Track(t, cls, hwnd);
    return t;
}

HTHEME WINAPI OpenThemeDataEx_Hook(HWND hwnd, LPCWSTR cls, DWORD flags) {
    HTHEME t = OpenThemeDataEx_Original(hwnd, cls, flags);
    Track(t, cls, hwnd);
    return t;
}

HRESULT WINAPI CloseThemeData_Hook(HTHEME t) {
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        g_progressThemes.erase(t);
    }
    return CloseThemeData_Original(t);
}

HRESULT WINAPI DrawThemeBackground_Hook(HTHEME t, HDC hdc, int part, int state, LPCRECT rc,
                                        LPCRECT clip) {
    if (hdc && IsProgressTheme(t) && ShouldStyle(t, hdc) &&
        PaintProgress(hdc, part, state, rc, clip)) {
        return S_OK;
    }
    return DrawThemeBackground_Original(t, hdc, part, state, rc, clip);
}

HRESULT WINAPI DrawThemeBackgroundEx_Hook(HTHEME t, HDC hdc, int part, int state, LPCRECT rc,
                                          const void* opts) {
    const DtbgOptsLite* o = (const DtbgOptsLite*)opts;
    const RECT* pclip = nullptr;
    bool computingRegion = false;
    if (o && o->dwSize >= sizeof(DtbgOptsLite)) {
        if (o->dwFlags & kDTBG_CLIPRECT) pclip = &o->rcClip;
        if (o->dwFlags & kDTBG_COMPUTINGREGION) computingRegion = true;
    }

    if (!computingRegion && hdc && IsProgressTheme(t) && ShouldStyle(t, hdc) &&
        PaintProgress(hdc, part, state, rc, pclip)) {
        return S_OK;
    }
    return DrawThemeBackgroundEx_Original(t, hdc, part, state, rc, opts);
}

// ---------------------------------------------------------------- entry points

BOOL Wh_ModInit() {
    LoadSettings();

    HMODULE ux = LoadLibraryW(L"uxtheme.dll");
    if (!ux) {
        Wh_Log(L"Failed to load uxtheme.dll");
        return FALSE;
    }

    auto hook = [&](LPCSTR name, void* hookFn, void** orig) {
        void* p = (void*)GetProcAddress(ux, name);
        if (p) {
            Wh_SetFunctionHook(p, hookFn, orig);
        } else {
            Wh_Log(L"uxtheme export not found, skipping a hook");
        }
    };

    hook("OpenThemeData", (void*)OpenThemeData_Hook, (void**)&OpenThemeData_Original);
    hook("OpenThemeDataForDpi", (void*)OpenThemeDataForDpi_Hook,
         (void**)&OpenThemeDataForDpi_Original);
    hook("OpenThemeDataEx", (void*)OpenThemeDataEx_Hook, (void**)&OpenThemeDataEx_Original);
    hook("CloseThemeData", (void*)CloseThemeData_Hook, (void**)&CloseThemeData_Original);
    hook("DrawThemeBackground", (void*)DrawThemeBackground_Hook,
         (void**)&DrawThemeBackground_Original);
    hook("DrawThemeBackgroundEx", (void*)DrawThemeBackgroundEx_Hook,
         (void**)&DrawThemeBackgroundEx_Original);

    return TRUE;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}

void Wh_ModUninit() {
    std::lock_guard<std::mutex> lock(g_mutex);
    g_progressThemes.clear();
}
