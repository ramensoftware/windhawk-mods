// ==WindhawkMod==
// @id              copy-progress-styles
// @name            File Copy Progress Bar Styles
// @description     Restyles the progress bar in the Windows copy/move/delete dialog and in 7-Zip / WinRAR extraction (16 styles)
// @version         1.0.0
// @author          Pixeliota
// @github          https://github.com/Pixeliota
// @include         explorer.exe
// @include         7zG.exe
// @include         7zFM.exe
// @include         WinRAR.exe
// @license         MIT
// @compilerOptions -lgdi32 -lmsimg32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# File Copy Progress Bar Styles

Restyles the progress bar shown in the Windows file copy / move / delete dialog, and the
extraction progress bar of 7-Zip and WinRAR.

Screenshots of the styles are shown below.

**Accent styles** (use the accent color)
* **Solid** - flat color

  ![Solid style](https://i.imgur.com/vdd9Uv5.png)

* **Satin** - gradient with a top highlight
* **Candy Cane** - diagonal stripes

  ![Candy Cane style](https://i.imgur.com/FzzmGxh.png)

* **Neon Tube** - dark track with a glowing core

  ![Neon Tube style](https://i.imgur.com/fLQqUQt.png)

* **Arcade** - retro blocks

  ![Arcade style](https://i.imgur.com/kTFi3jM.png)


**Designer styles** (fixed colors, some with their own track)
* **Evergreen** - dark green timeline with stepped bands and a diamond knob

  ![Evergreen style](https://i.imgur.com/G2zPbsh.png)

* **Sorbet** - pink, yellow and blue segments

  ![Sorbet style](https://i.imgur.com/nx5igz7.png)

* **Polka** - blue gradient with dot markers

  ![Polka style](https://i.imgur.com/VmmfywN.png)

* **Aurora** - cyan-to-lilac gradient with an end marker and tick track

  ![Aurora style](https://i.imgur.com/ZOjlvY1.png)

* **Ember** - orange diagonal stripes on a striped gray track

  ![Ember style](https://i.imgur.com/wf7WwLz.png)

* **Mint** - soft green bar with a subtle bottom shade

  ![Mint style](https://i.imgur.com/fvcFM5f.png)

* **Fire & Ice** - red-to-icy-blue gradient

  ![Fire & Ice style](https://i.imgur.com/URI28xU.png)

* **Prism** - full-spectrum gradient

  ![Prism style](https://i.imgur.com/OZzUoOu.png)

* **Confetti** - colorful mosaic

  ![Confetti style](https://i.imgur.com/J7TYCcC.png)

* **Carbon** - dark mosaic on a dark track

  ![Carbon style](https://i.imgur.com/RQL1wxd.png)

* **Dusk** - navy-to-orange-to-lilac gradient

  ![Dusk style](https://i.imgur.com/o1m91bm.png)


Paused and error states automatically switch to amber and red.

**Notes**
* Only the copy dialog is restyled by default. The drive usage bars in This PC keep the
  standard Windows look. Turn on "Also restyle other progress bars in Explorer" to
  restyle those too.
* The mod also loads into 7-Zip (7zG.exe, 7zFM.exe) and WinRAR (WinRAR.exe) and restyles
  their extraction progress bars. To turn that off, switch off "Also style 7-Zip and
  WinRAR" in the settings (or add those processes to the mod's custom process exclusion
  list in the Advanced tab). Extracting a ZIP with Explorer's built-in "Extract All" uses
  the normal copy dialog.
* Tested on Windows 11. The theme lookup uses an undocumented uxtheme function (ordinal
  74), so other Windows versions have not been verified.
* In a dark copy dialog the tracks of the styles are darkened to match.
* Windows itself turns a drive bar red when the drive has under 10% free.
* Copy dialogs shown by other processes (for example inside an app's Open/Save dialog)
  are not styled. Add those processes in the mod's custom process inclusion list.
* Setting changes show up the next time the bar repaints.
* To troubleshoot, enable logging in the mod's Advanced tab and watch the log during a copy.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- style: satin
  $name: Style
  $options:
  - solid: Solid (flat accent color)
  - satin: Satin (accent gradient with highlight)
  - candycane: Candy Cane (diagonal stripes)
  - neon: Neon Tube (dark track, glowing core)
  - arcade: Arcade (retro blocks)
  - evergreen: Evergreen (dark green timeline with knob)
  - sorbet: Sorbet (pink, yellow and blue segments)
  - polka: Polka (blue gradient with dots)
  - aurora: Aurora (cyan to lilac with end marker)
  - ember: Ember (orange diagonal stripes)
  - mint: Mint (soft green)
  - fireice: Fire & Ice (red to icy blue)
  - prism: Prism (full spectrum)
  - confetti: Confetti (colorful mosaic)
  - carbon: Carbon (dark mosaic)
  - dusk: Dusk (navy to orange to lilac)
- accentColor: "#06B025"
  $name: Accent color (hex) - used by Solid, Satin, Candy Cane, Neon Tube and Arcade
- trackColor: "#E6E6E6"
  $name: Track color (hex) - used by styles that have no track of their own
- cornerRadius: 3
  $name: Fill corner radius in pixels at 100% scaling (0 = square)
- hideOverlay: true
  $name: Hide the default moving glow
- styleArchivers: true
  $name: Also style 7-Zip and WinRAR
- styleOtherBars: false
  $name: Also restyle other progress bars in Explorer
  $description: For example, the drive usage bars in This PC
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <uxtheme.h>


// Progress bar theme parts / states (defined locally to avoid header dependency)
constexpr int kPP_BAR = 1;
constexpr int kPP_FILL = 5;
constexpr int kPP_PULSEOVERLAY = 7;
constexpr int kPP_MOVEOVERLAY = 8;
constexpr int kPP_TRANSPARENTBAR = 11;
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
    S_SOLID,
    S_SATIN,
    S_CANDY,
    S_NEON,
    S_ARCADE,
    S_EVERGREEN,
    S_SORBET,
    S_POLKA,
    S_AURORA,
    S_EMBER,
    S_MINT,
    S_FIREICE,
    S_PRISM,
    S_CONFETTI,
    S_CARBON,
    S_DUSK
};

struct Config {
    Style style = S_SATIN;
    COLORREF accent = RGB(6, 176, 37);
    COLORREF track = RGB(230, 230, 230);
    int radius = 3;
    bool hideOverlay = true;
    bool styleArchivers = true;
    bool styleOtherBars = false;
} g;

// Private uxtheme export (ordinal 74): returns the class name a theme handle was opened
// with. Works for handles that were opened before the mod was loaded.
using GetThemeClass_t = HRESULT(WINAPI*)(HTHEME, LPWSTR, int);
GetThemeClass_t g_getThemeClass;

// True when the mod runs inside 7-Zip or WinRAR. Their progress bars are all styled, which
// the "Also style 7-Zip and WinRAR" setting controls. In every other process (Explorer
// included) only the copy dialog is styled unless "styleOtherBars" is on.
bool g_isArchiver = false;

// ---------------------------------------------------------------- helpers

static int Imax(int a, int b) { return a > b ? a : b; }

// Corner radius is set in pixels at 100% scaling; the bar height is used as the scale.
static int ScaledRadius(int barHeight) { return g.radius * barHeight / 14; }

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

// Asks uxtheme which class the handle was opened with, and reports whether it is a
// progress bar theme.
static bool GetProgressClass(HTHEME t, WCHAR* cls, int count) {
    if (!t || !g_getThemeClass) return false;
    cls[0] = 0;
    if (FAILED(g_getThemeClass(t, cls, count))) return false;
    CharLowerW(cls);  // lowercased in place, so later checks need no extra copy
    return wcsstr(cls, L"progress") != nullptr;
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

// Decides whether this progress bar should be restyled. By default only the copy dialog is.
static bool ShouldStyle(LPCWSTR themeClass, HDC hdc) {
    if (g_isArchiver) return g.styleArchivers;
    if (g.styleOtherBars) return true;

    // Copy engine theme class (for example "CopyEngine::Progress"), when Windows uses one.
    if (wcsstr(themeClass, L"copyengine") != nullptr) return true;  // already lowercase

    HWND w = WindowFromDC(hdc);
    if (w) {
        HWND root = GetAncestor(w, GA_ROOT);
        wchar_t cls[64] = {};
        GetClassNameW(root ? root : w, cls, 64);
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
        if (wcscmp(st, L"solid") == 0) g.style = S_SOLID;
        else if (wcscmp(st, L"candycane") == 0) g.style = S_CANDY;
        else if (wcscmp(st, L"neon") == 0) g.style = S_NEON;
        else if (wcscmp(st, L"arcade") == 0) g.style = S_ARCADE;
        else if (wcscmp(st, L"evergreen") == 0) g.style = S_EVERGREEN;
        else if (wcscmp(st, L"sorbet") == 0) g.style = S_SORBET;
        else if (wcscmp(st, L"polka") == 0) g.style = S_POLKA;
        else if (wcscmp(st, L"aurora") == 0) g.style = S_AURORA;
        else if (wcscmp(st, L"ember") == 0) g.style = S_EMBER;
        else if (wcscmp(st, L"mint") == 0) g.style = S_MINT;
        else if (wcscmp(st, L"fireice") == 0) g.style = S_FIREICE;
        else if (wcscmp(st, L"prism") == 0) g.style = S_PRISM;
        else if (wcscmp(st, L"confetti") == 0) g.style = S_CONFETTI;
        else if (wcscmp(st, L"carbon") == 0) g.style = S_CARBON;
        else if (wcscmp(st, L"dusk") == 0) g.style = S_DUSK;
        else g.style = S_SATIN;
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
    g.styleOtherBars = Wh_GetIntSetting(L"styleOtherBars") != 0;
    g.styleArchivers = Wh_GetIntSetting(L"styleArchivers") != 0;
}

// ---------------------------------------------------------------- palettes

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

static const Stop kFireIce[] = {{0, RGB(255, 39, 42)},    {80, RGB(255, 55, 41)},
                                {240, RGB(255, 98, 39)},  {470, RGB(255, 167, 36)},
                                {520, RGB(255, 183, 36)}, {610, RGB(241, 204, 100)},
                                {800, RGB(209, 255, 255)}, {1000, RGB(132, 231, 246)}};
static const Stop kPrism[] = {{0, RGB(239, 216, 147)},   {220, RGB(255, 39, 42)},
                              {250, RGB(240, 44, 58)},   {300, RGB(202, 60, 101)},
                              {380, RGB(142, 85, 171)},  {470, RGB(69, 116, 255)},
                              {630, RGB(117, 180, 255)}, {800, RGB(174, 255, 255)},
                              {1000, RGB(111, 181, 118)}};
// Dusk colors are estimated from a preview image.
static const Stop kDusk[] = {{0, RGB(10, 31, 61)},     {250, RGB(110, 150, 185)},
                             {400, RGB(190, 172, 150)}, {500, RGB(226, 140, 60)},
                             {650, RGB(238, 105, 20)}, {780, RGB(212, 130, 150)},
                             {880, RGB(190, 150, 238)}, {1000, RGB(168, 180, 255)}};

// Mosaic tiles, in design units: the design is 440 wide and 15 tall and is scaled by the
// bar height. Tile colors are blended with the base color using a percentage (a).
struct Tile {
    int x, w, y, h;
    COLORREF c;
    int a;
};

static const Tile kConfetti[] = {
    {0, 15, 0, 9, RGB(63, 55, 201), 68},
    {5, 32, 4, 11, RGB(184, 192, 255), 74},
    {21, 32, 0, 9, RGB(248, 249, 250), 96},
    {40, 32, 0, 15, RGB(158, 240, 26), 79},
    {60, 32, 0, 9, RGB(202, 255, 191), 81},
    {82, 32, 4, 11, RGB(255, 173, 173), 74},
    {98, 32, 0, 9, RGB(24, 28, 52), 82},
    {117, 32, 0, 15, RGB(33, 37, 41), 80},
    {137, 32, 0, 9, RGB(164, 19, 60), 93},
    {159, 32, 4, 11, RGB(231, 240, 255), 88},
    {175, 32, 0, 9, RGB(58, 95, 184), 34},
    {194, 32, 0, 15, RGB(247, 37, 133), 35},
    {214, 32, 0, 9, RGB(255, 158, 0), 69},
    {236, 32, 4, 11, RGB(0, 128, 0), 79},
    {252, 32, 0, 9, RGB(255, 77, 109), 29},
    {271, 32, 0, 15, RGB(46, 196, 182), 94},
    {291, 32, 0, 9, RGB(199, 193, 185), 71},
    {313, 32, 4, 11, RGB(15, 143, 141), 82},
    {329, 32, 0, 9, RGB(181, 43, 10), 77},
    {348, 32, 0, 15, RGB(209, 227, 255), 57},
    {368, 32, 0, 9, RGB(255, 196, 61), 41},
    {390, 32, 4, 11, RGB(232, 199, 184), 98},
    {406, 32, 0, 9, RGB(128, 237, 153), 46},
    {425, 15, 0, 15, RGB(107, 128, 110), 79},
};

static const Tile kCarbon[] = {
    {425, 15, 6, 9, RGB(99, 99, 99), 73},
    {403, 32, 0, 11, RGB(99, 99, 99), 75},
    {387, 32, 6, 9, RGB(99, 99, 99), 91},
    {368, 32, 0, 15, RGB(99, 99, 99), 71},
    {348, 32, 6, 9, RGB(99, 99, 99), 24},
    {326, 32, 0, 11, RGB(99, 99, 99), 59},
    {310, 32, 6, 9, RGB(99, 99, 99), 89},
    {291, 32, 0, 15, RGB(99, 99, 99), 31},
    {271, 32, 6, 9, RGB(99, 99, 99), 26},
    {249, 32, 0, 11, RGB(99, 99, 99), 37},
    {233, 32, 6, 9, RGB(99, 99, 99), 15},
    {214, 32, 0, 15, RGB(99, 99, 99), 65},
    {194, 32, 6, 9, RGB(99, 99, 99), 63},
    {172, 32, 0, 11, RGB(99, 99, 99), 37},
    {156, 32, 6, 9, RGB(99, 99, 99), 57},
    {137, 32, 0, 15, RGB(99, 99, 99), 41},
    {117, 32, 6, 9, RGB(99, 99, 99), 60},
    {95, 32, 0, 11, RGB(99, 99, 99), 41},
    {79, 32, 6, 9, RGB(99, 99, 99), 33},
    {40, 32, 6, 9, RGB(99, 99, 99), 28},
    {18, 32, 0, 11, RGB(99, 99, 99), 78},
    {2, 32, 6, 9, RGB(99, 99, 99), 54},
    {0, 15, 0, 15, RGB(99, 99, 99), 32},
};

static void Mosaic(HDC hdc, const RECT& r, COLORREF base, const Tile* tiles, int n,
                   bool tinted, COLORREF tint) {
    const int h = r.bottom - r.top;
    const int width = r.right - r.left;
    if (h <= 0 || width <= 0) return;
    const COLORREF bg = tinted ? tint : base;
    Fill(hdc, r, bg);
    const int cycle = Imax(1, 440 * h / 15);  // the pattern repeats if the bar is longer
    for (int off = 0; off < width; off += cycle) {
        for (int i = 0; i < n; i++) {
            const Tile& t = tiles[i];
            RECT tr;
            tr.left = r.left + off + t.x * h / 15;
            tr.right = r.left + off + (t.x + t.w) * h / 15;
            tr.top = r.top + t.y * h / 15;
            tr.bottom = r.top + (t.y + t.h) * h / 15;
            if (tr.left >= r.right) continue;
            Fill(hdc, tr, Mix(bg, t.c, tinted ? t.a / 3 : t.a));
        }
    }
}

// ---------------------------------------------------------------- painting

// In a dark dialog light track colors are darkened; colors the user already set dark stay.
static COLORREF ShadeForDark(COLORREF c, bool dark) {
    if (!dark) return c;
    const int luma = (GetRValue(c) * 299 + GetGValue(c) * 587 + GetBValue(c) * 114) / 1000;
    return luma > 110 ? Darken(c, 70) : c;
}

static void DrawTrack(HDC hdc, RECT r, const RECT& clip, bool dark) {
    const int h = r.bottom - r.top;
    // The track always covers its whole rectangle (square corners). Rounding it would leave
    // the stock border corners visible as faint marks; only the fill is rounded.
    int saved = BeginClip(hdc, clip, r, 0);
    auto shade = [&](COLORREF c) { return ShadeForDark(c, dark); };

    switch (g.style) {
        case S_NEON:
            Fill(hdc, r, RGB(28, 30, 38));
            break;

        case S_EVERGREEN:
            Fill(hdc, r, RGB(25, 72, 27));
            break;

        case S_POLKA:
            Fill(hdc, r, shade(RGB(204, 217, 245)));
            Dots(hdc, r, shade(RGB(240, 251, 255)));
            break;

        case S_AURORA: {
            Fill(hdc, r, shade(Lighten(g.track, 60)));
            const int tw = Imax(1, h * 115 / 1000);
            const int period = Imax(3, h * 328 / 1000);
            for (int x = r.left; x < r.right; x += period) {
                RECT t = {x, r.top, x + tw, r.bottom};
                Fill(hdc, t, shade(RGB(219, 219, 221)));
            }
            break;
        }

        case S_EMBER:
            Fill(hdc, r, shade(RGB(198, 198, 198)));
            Stripes(hdc, r, h * 85 / 100, h * 193 / 100, r.left - h * 8 / 100, h, nullptr, 0,
                    shade(RGB(198, 198, 198)), 25);
            break;

        case S_MINT: {
            const COLORREF base = shade(RGB(198, 205, 211));
            Fill(hdc, r, base);
            RECT lower = r;
            lower.top = r.bottom - Imax(1, h * 47 / 100);
            Fill(hdc, lower, Darken(base, 7));
            break;
        }

        case S_CARBON:
            Fill(hdc, r, RGB(30, 30, 30));
            break;

        default:
            Fill(hdc, r, shade(g.track));
            break;
    }

    RestoreDC(hdc, saved);
}

static void DrawFill(HDC hdc, RECT r, const RECT& clip, int state, bool dark) {
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

    int saved = BeginClip(hdc, clip, r, Imax(0, ScaledRadius(h) - 1));

    switch (g.style) {
        case S_SOLID:
            Fill(hdc, r, c);
            break;

        case S_SATIN: {
            Grad(hdc, r, Darken(c, 30), Lighten(c, 35), false);
            RECT top = r;
            top.bottom = top.top + 1;
            Fill(hdc, top, Lighten(c, 55));
            break;
        }

        case S_CANDY: {
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

        case S_ARCADE: {
            const int bw = Imax(5, h * 3 / 4);
            const int gap = 2;
            for (int x = r.left; x < r.right; x += bw + gap) {
                RECT b = {x, r.top, (x + bw < r.right) ? x + bw : r.right, r.bottom};
                Fill(hdc, b, c);
            }
            break;
        }

        case S_EVERGREEN: {
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

        case S_SORBET: {
            Fill(hdc, r, ShadeForDark(g.track, dark));
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

        case S_POLKA:
            gradient(r, kDots, 4);
            Dots(hdc, r, normal ? RGB(0, 46, 147) : Darken(c, 55));
            break;

        case S_AURORA: {
            const int mw = Imax(2, h * 115 / 1000);
            const int gap = Imax(2, h * 20 / 100);
            Fill(hdc, r, ShadeForDark(Lighten(g.track, 60), dark));
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

        case S_FIREICE:
            gradient(r, kFireIce, 8);
            break;

        case S_PRISM:
            gradient(r, kPrism, 9);
            break;

        case S_DUSK:
            gradient(r, kDusk, 8);
            break;

        case S_CONFETTI:
            Mosaic(hdc, r, RGB(208, 160, 141), kConfetti, (int)ARRAYSIZE(kConfetti), !normal, c);
            break;

        case S_CARBON:
            Mosaic(hdc, r, RGB(15, 15, 15), kCarbon, (int)ARRAYSIZE(kCarbon), !normal, c);
            break;

        case S_MINT: {
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
static bool PaintProgress(HDC hdc, int part, int state, const RECT* prc, const RECT* pclip,
                          bool dark) {
    if (!prc) return false;
    RECT r = *prc;
    if (r.right <= r.left || r.bottom <= r.top) return false;

    const bool overlay = (part == kPP_PULSEOVERLAY || part == kPP_MOVEOVERLAY);
    const bool handled = (part == kPP_BAR || part == kPP_TRANSPARENTBAR || part == kPP_FILL ||
                          (overlay && g.hideOverlay));
    if (!handled) return false;

    // Respect the caller's clip rectangle, like the stock theme does: if it does not
    // overlap the bar, the stock call would draw nothing, so draw nothing here too.
    RECT clip = r;
    if (pclip && !IntersectRect(&clip, &r, pclip)) return true;

    if (overlay) return true;  // swallow the stock glow animation

    if (part == kPP_BAR || part == kPP_TRANSPARENTBAR) DrawTrack(hdc, r, clip, dark);
    else DrawFill(hdc, r, clip, state, dark);
    return true;
}

// ---------------------------------------------------------------- hooks

using DrawThemeBackground_t = HRESULT(WINAPI*)(HTHEME, HDC, int, int, LPCRECT, LPCRECT);
using DrawThemeBackgroundEx_t = HRESULT(WINAPI*)(HTHEME, HDC, int, int, LPCRECT, const void*);

DrawThemeBackground_t DrawThemeBackground_Original;
DrawThemeBackgroundEx_t DrawThemeBackgroundEx_Original;

// Only these parts are ever replaced, so every other draw call skips the class lookup.
static bool IsProgressPart(int part) {
    return part == kPP_BAR || part == kPP_TRANSPARENTBAR || part == kPP_FILL ||
           part == kPP_PULSEOVERLAY || part == kPP_MOVEOVERLAY;
}

HRESULT WINAPI DrawThemeBackground_Hook(HTHEME t, HDC hdc, int part, int state, LPCRECT rc,
                                        LPCRECT clip) {
    if (hdc && IsProgressPart(part)) {
        WCHAR cls[64];
        if (GetProgressClass(t, cls, ARRAYSIZE(cls))) {
            Wh_Log(L"Progress draw: class=%s part=%d state=%d", cls, part, state);
            if (ShouldStyle(cls, hdc)) {
                const bool dark = wcsstr(cls, L"darkmode") != nullptr;
                if (part == kPP_TRANSPARENTBAR) {
                    // Let Windows paint the background behind the bar first, so the
                    // rounded corners of our track are not left black.
                    HRESULT hr = DrawThemeBackground_Original(t, hdc, part, state, rc, clip);
                    PaintProgress(hdc, part, state, rc, clip, dark);
                    return hr;
                }
                if (PaintProgress(hdc, part, state, rc, clip, dark)) return S_OK;
            }
        }
    }
    return DrawThemeBackground_Original(t, hdc, part, state, rc, clip);
}

HRESULT WINAPI DrawThemeBackgroundEx_Hook(HTHEME t, HDC hdc, int part, int state, LPCRECT rc,
                                          const void* opts) {
    if (hdc && IsProgressPart(part)) {
        const DtbgOptsLite* o = (const DtbgOptsLite*)opts;
        const RECT* pclip = nullptr;
        bool computingRegion = false;
        if (o && o->dwSize >= sizeof(DtbgOptsLite)) {
            if (o->dwFlags & kDTBG_CLIPRECT) pclip = &o->rcClip;
            if (o->dwFlags & kDTBG_COMPUTINGREGION) computingRegion = true;
        }

        WCHAR cls[64];
        if (!computingRegion && GetProgressClass(t, cls, ARRAYSIZE(cls))) {
            Wh_Log(L"Progress draw (Ex): class=%s part=%d state=%d", cls, part, state);
            if (ShouldStyle(cls, hdc)) {
                const bool dark = wcsstr(cls, L"darkmode") != nullptr;
                if (part == kPP_TRANSPARENTBAR) {
                    // Let Windows paint the background behind the bar first, so the
                    // rounded corners of our track are not left black.
                    HRESULT hr =
                        DrawThemeBackgroundEx_Original(t, hdc, part, state, rc, opts);
                    PaintProgress(hdc, part, state, rc, pclip, dark);
                    return hr;
                }
                if (PaintProgress(hdc, part, state, rc, pclip, dark)) return S_OK;
            }
        }
    }
    return DrawThemeBackgroundEx_Original(t, hdc, part, state, rc, opts);
}

// ---------------------------------------------------------------- entry points

BOOL Wh_ModInit() {
    LoadSettings();

    wchar_t exePath[MAX_PATH] = {};
    GetModuleFileNameW(nullptr, exePath, MAX_PATH);
    const wchar_t* exeName = wcsrchr(exePath, L'\\');
    exeName = exeName ? exeName + 1 : exePath;
    g_isArchiver = (_wcsicmp(exeName, L"7zG.exe") == 0 || _wcsicmp(exeName, L"7zFM.exe") == 0 ||
                    _wcsicmp(exeName, L"WinRAR.exe") == 0);
    if (g_isArchiver && !g.styleArchivers) return FALSE;  // nothing to do; the mod reloads
                                                          // when the setting changes

    HMODULE ux = LoadLibraryExW(L"uxtheme.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!ux) {
        Wh_Log(L"Failed to load uxtheme.dll");
        return FALSE;
    }

    g_getThemeClass = (GetThemeClass_t)(void*)GetProcAddress(ux, MAKEINTRESOURCEA(74));
    if (!g_getThemeClass) {
        Wh_Log(L"uxtheme GetThemeClass (ordinal 74) not found");
        return FALSE;
    }

    void* drawBg = (void*)GetProcAddress(ux, "DrawThemeBackground");
    if (drawBg) {
        Wh_SetFunctionHook(drawBg, (void*)DrawThemeBackground_Hook,
                           (void**)&DrawThemeBackground_Original);
    } else {
        Wh_Log(L"DrawThemeBackground not found");
        return FALSE;
    }

    void* drawBgEx = (void*)GetProcAddress(ux, "DrawThemeBackgroundEx");
    if (drawBgEx) {
        Wh_SetFunctionHook(drawBgEx, (void*)DrawThemeBackgroundEx_Hook,
                           (void**)&DrawThemeBackgroundEx_Original);
    }

    return TRUE;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}
