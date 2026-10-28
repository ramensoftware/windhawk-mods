// ==WindhawkMod==
// @id              altbinz-dark
// @name            Alt.Binz Dark Mode
// @description     Dark colors for the skinned user interface of Alt.Binz, with a custom surface color, gradients and colored progress bars
// @version         1.0.1
// @author          MrSmith
// @github          https://github.com/MrSmith2017
// @include         altbinz.exe
// @architecture    x86
// @compilerOptions -lgdi32 -luser32 -luxtheme
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Alt.Binz Dark Mode

![Before and after: the light blue Alt.Binz versus the dark one](https://raw.githubusercontent.com/MrSmith2017/altbinz-dark-mode/main/altbinz-dark-mode.png)

Alt.Binz draws its whole user interface from a light blue skin. This mod
re-colors everything that is drawn with GDI: brushes, pens, text colors, system
colors and the opaque skin bitmaps. Common dialogs and shell menus keep
Windows' own dark mode.

## Compatibility

Tested with Alt.Binz 0.48.4 (32-bit) at 100% display scaling on Windows 11. Some
elements (the column header bars, the progress bar fills and the status bar button)
are recognized by their size and color, so another version may look different.

## Surface color

The blue skin surface (toolbar, tabs, status bar, dialogs, Setup) can get another
color: enter a hex code (for example `2F5A96`). Very bright colors are darkened
so that the light text stays readable.

## Surface gradient

Instead of one color the surface can run between two colors. Enter both as hex
codes, the angle in degrees (0 = left to right, 90 = top to bottom) and the
balance: 50 is the middle, a higher value gives the end color more room, a lower
value the start color. The gradient wins over the single surface color.

## Progress bars

The fill of the progress bars can get one color or, with a second color, a
gradient, either from top to bottom or from left to right. Leave both empty for the
default blue.

## Appearance

These values are applied when Alt.Binz is restarted:

* **Background lightness**: how dark the lists and dark areas are (percent).
* **Text lightness**: how bright text and lines are (percent). Ignored when a text color is set.
* **Text color**: an optional color for text and lines.
* **List background color**: an optional color for the large list areas.
* **Saturation**: how colorful the blue skin is (percent).

The **column header color** (the header bars of the lists) and everything else is
applied live. If you disable the mod, restart Alt.Binz to get the original look
back completely.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- surface:
  - customColor: ""
    $name: Surface color
    $description: Hex color, for example 2F5A96. Leave empty for the blue of the original skin.
  $name: Surface
- gradient:
  - enabled: false
    $name: Use a gradient for the surface
  - colorStart: "2F5A96"
    $name: Start color
    $description: Hex color, for example 2F5A96.
  - colorEnd: "152238"
    $name: End color
    $description: Hex color, for example 152238.
  - balance: 50
    $name: Balance
    $description: 0 to 100. 50 is the middle, higher gives the end color more room, lower the start color.
  - angle: 90
    $name: Angle
    $description: In degrees. 0 = left to right, 90 = top to bottom, 180 = right to left, 270 = bottom to top.
  $name: Surface gradient
- progress:
  - colorTop: ""
    $name: Progress bar color
    $description: Hex color of the progress bar fill, for example 2E8B57. Leave empty for the default blue.
  - colorBottom: ""
    $name: Progress bar gradient color
    $description: Optional second color. The fill then runs as a gradient from the first color to this one.
  - direction: topbottom
    $name: Gradient direction
    $options:
    - topbottom: Top to bottom
    - leftright: Left to right
  $name: Progress bars
- appearance:
  - backgroundLightness: 11
    $name: Background lightness
    $description: In percent. Restart Alt.Binz to apply. 5 is almost black, 20 is dark gray.
  - textLightness: 92
    $name: Text lightness
    $description: In percent. Ignored when a text color is set. Restart Alt.Binz to apply.
  - textColor: ""
    $name: Text color
    $description: Hex color for text and lines, for example E8EEF8. Leave empty for the neutral light gray. Restart Alt.Binz to apply.
  - listColor: ""
    $name: List background color
    $description: Hex color for the large list areas (download queue, log), for example 1E2430. Leave empty for dark gray. Restart Alt.Binz to apply.
  - headerColor: ""
    $name: Column header color
    $description: Hex color for the header bars of the lists (Subject, Size, Action, Description ...), for example 2F5A96. Leave empty for the default blue.
  - saturation: 85
    $name: Saturation
    $description: In percent. 1 is almost gray, 100 keeps the colors of the original skin, up to 200 is more colorful. Restart Alt.Binz to apply.
  - keepSetupTreeDark: false
    $name: Keep the tree in Setup dark gray
    $description: By default the tree on the left of the Setup window uses the color of the surface.
  $name: Appearance
*/
// ==/WindhawkModSettings==

#include <windhawk_api.h>
#include <windows.h>
#include <uxtheme.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <algorithm>
#include <atomic>
#include <unordered_map>
#include <vector>

// ---- settings -----------------------------------------------------------------

struct Settings {
    float darkOut;       // lightness of white
    float lightOut;      // lightness of black
    float satScale;
    bool gradient;
    COLORREF colorStart;
    COLORREF colorEnd;
    int angle;
    float balance;
    bool textTint;
    float textHue, textSat;
    bool listTint;
    float listHue, listSat;
    bool keepSetupTreeDark;
    bool progressOn;
    bool progressGradient;
    bool progressHorizontal;
    COLORREF progressC1, progressC2;
    float progressL1;
    float listL;
    bool headerOn;
    COLORREF headerColor;
};
static Settings g_cfg = {0.11f, 0.92f, 0.85f, false, RGB(0x2F, 0x5A, 0x96), RGB(0x15, 0x22, 0x38), 90, 0.5f, false, 0.0f, 0.0f, false, 0.0f, 0.0f,
                         false, false, false, false, 0, 0, 0.0f, 0.0f, false, 0};

// ---- color mapping ----------------------------------------------------------

static void RgbToHsl(float r, float g, float b, float* h, float* s, float* l) {
    float mx = fmaxf(r, fmaxf(g, b)), mn = fminf(r, fminf(g, b));
    float d = mx - mn;
    *l = (mx + mn) * 0.5f;
    *h = 0;
    *s = 0;
    if (d > 1e-6f) {
        *s = d / (1.0f - fabsf(2.0f * *l - 1.0f));
        if (mx == r) {
            *h = fmodf((g - b) / d, 6.0f);
        } else if (mx == g) {
            *h = (b - r) / d + 2.0f;
        } else {
            *h = (r - g) / d + 4.0f;
        }
        if (*h < 0) {
            *h += 6.0f;
        }
    }
}

// h in 0..6
static void HslToRgb8(float h, float s, float l, uint8_t* r8, uint8_t* g8, uint8_t* b8) {
    float c = (1.0f - fabsf(2.0f * l - 1.0f)) * s;
    float x = c * (1.0f - fabsf(fmodf(h, 2.0f) - 1.0f));
    float m = l - c * 0.5f;
    float rr, gg, bb;
    if (h < 1) { rr = c; gg = x; bb = 0; }
    else if (h < 2) { rr = x; gg = c; bb = 0; }
    else if (h < 3) { rr = 0; gg = c; bb = x; }
    else if (h < 4) { rr = 0; gg = x; bb = c; }
    else if (h < 5) { rr = x; gg = 0; bb = c; }
    else { rr = c; gg = 0; bb = x; }
    *r8 = (uint8_t)fminf(255.0f, fmaxf(0.0f, (rr + m) * 255.0f + 0.5f));
    *g8 = (uint8_t)fminf(255.0f, fmaxf(0.0f, (gg + m) * 255.0f + 0.5f));
    *b8 = (uint8_t)fminf(255.0f, fmaxf(0.0f, (bb + m) * 255.0f + 0.5f));
}

static void MapRGB(uint8_t& r8, uint8_t& g8, uint8_t& b8) {
    if (r8 == 255 && g8 == 0 && b8 == 255) {  // transparency key (clFuchsia)
        return;
    }
    float h, s, l;
    RgbToHsl(r8 / 255.0f, g8 / 255.0f, b8 / 255.0f, &h, &s, &l);
    float l2 = g_cfg.lightOut - (g_cfg.lightOut - g_cfg.darkOut) * l;
    if (g_cfg.listTint && s < 0.12f && l >= 0.9f) {  // white areas (lists, edits)
        HslToRgb8(g_cfg.listHue, g_cfg.listSat, fminf(0.5f, fmaxf(0.0f, l2 + g_cfg.listL - g_cfg.darkOut)), &r8, &g8, &b8);
        return;
    }
    if (g_cfg.textTint && s < 0.12f && l < 0.5f) {  // gray and black/white text takes the color of the text setting
        HslToRgb8(g_cfg.textHue, g_cfg.textSat, l2, &r8, &g8, &b8);
        return;
    }
    HslToRgb8(h, s * g_cfg.satScale, l2, &r8, &g8, &b8);
}

static COLORREF MapColor(COLORREF c) {
    if (c & 0xFF000000) {  // palette index / palette relative, leave alone
        return c;
    }
    uint8_t r = GetRValue(c), g = GetGValue(c), b = GetBValue(c);
    MapRGB(r, g, b);
    return RGB(r, g, b);
}

// ---- state --------------------------------------------------------------------

static CRITICAL_SECTION g_cs;
static bool g_csReady = false;

struct BmpEntry {
    int state;                    // 1 = transformed, 2 = skip, 3 = dirty (drawn into)
    void* bits;
    std::vector<uint8_t> backup;  // original pixels, to restore on unload
};
static std::unordered_map<HBITMAP, BmpEntry>* g_bmps;

// original text/background colors per DC (so that Get*/Set* return what the app expects)
static std::unordered_map<HDC, COLORREF>* g_textColors;
static std::unordered_map<HDC, COLORREF>* g_bkColors;

static HBRUSH g_sysBrushes[32];





// ---- who is calling? ---------------------------------------------------------------
// Alt.Binz' own code (and the standard GDI / user32 / comctl32 libraries drawing for it)
// is re-colored. Shell dialogs, shell menus and shell extensions come with Windows' own
// dark mode and must not be touched.

static HMODULE g_exeModule;
static std::unordered_map<HMODULE, int>* g_callerKind;  // 1 = the app, 2 = system drawing library, 0 = other

// The answer only depends on the call site, and the same few call sites draw all the time: a small
// table (address and kind in one word, so a read is never torn) avoids the module look-up
// (which takes the loader lock) for each call.
static std::atomic<uint32_t> g_callerCache[1024];

static int CallerKindUncached(void* ret);

static int CallerKind(void* ret) {
    const uint32_t a = (uint32_t)(uintptr_t)ret;
    const size_t slot = ((a >> 2) ^ (a >> 12)) & 1023;
    const uint32_t v = g_callerCache[slot].load(std::memory_order_relaxed);
    if (v != 0 && (v & ~3u) == (a & ~3u)) {
        return (int)(v & 3u);
    }
    const int kind = CallerKindUncached(ret);
    g_callerCache[slot].store((a & ~3u) | (uint32_t)kind, std::memory_order_relaxed);
    return kind;
}

static int CallerKindUncached(void* ret) {
    HMODULE m = nullptr;
    if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                            (LPCWSTR)ret, &m) || !m) {
        return 1;
    }
    if (m == g_exeModule) {
        return 1;
    }
    EnterCriticalSection(&g_cs);
    auto it = g_callerKind->find(m);
    if (it != g_callerKind->end()) {
        int kind = it->second;
        LeaveCriticalSection(&g_cs);
        return kind;
    }
    LeaveCriticalSection(&g_cs);

    wchar_t path[MAX_PATH] = L"";
    GetModuleFileNameW(m, path, MAX_PATH);
    const wchar_t* base = wcsrchr(path, L'\\');
    base = base ? base + 1 : path;
    static const wchar_t* const kSystem[] = {L"comctl32.dll", L"user32.dll", L"gdi32.dll", L"gdi32full.dll",
                                             L"win32u.dll", L"uxtheme.dll", L"gdiplus.dll", L"msimg32.dll",
                                             L"ntdll.dll", L"kernelbase.dll"};
    int kind = 0;
    for (const wchar_t* name : kSystem) {
        if (_wcsicmp(base, name) == 0) {
            kind = 2;
            break;
        }
    }
    EnterCriticalSection(&g_cs);
    (*g_callerKind)[m] = kind;
    LeaveCriticalSection(&g_cs);
    return kind;
}

// Windows of the app are Delphi forms (class names like TAltBinzMainForm). Everything else
// in the process (#32770 dialogs, #32768 menus, shell views) is foreign.
static bool IsAppRoot(HWND wnd) {
    HWND root = GetAncestor(wnd, GA_ROOT);
    wchar_t cls[64];
    return root && GetClassNameW(root, cls, ARRAYSIZE(cls)) && cls[0] == L'T' && iswalpha(cls[1]);
}

// true only when the DC belongs to a window of the app itself
static bool IsAppDC(HDC hdc) {
    if (!hdc) {
        return false;
    }
    HWND wnd = WindowFromDC(hdc);
    return wnd && IsAppRoot(wnd);
}

static bool ShouldMap(void* ret, HDC hdc = nullptr) {
    int kind = CallerKind(ret);
    if (kind == 1) {
        return true;
    }
    if (kind == 2) {
        return IsAppDC(hdc);  // system libraries: only when drawing straight into one of our windows
    }
    return false;
}

// selection / hot colors are paired with text that Windows draws itself: leave them alone
static bool IsSelectionColorIndex(int index) {
    return index == COLOR_HIGHLIGHT || index == COLOR_HIGHLIGHTTEXT || index == COLOR_HOTLIGHT ||
           index == COLOR_MENUHILIGHT;
}

// near black outlines and fills stay black (progress bar frames and similar)
static COLORREF MapShapeColor(COLORREF c, bool pen = false) {
    if (!(c & 0xFF000000) && GetRValue(c) < 24 && GetGValue(c) < 24 && GetBValue(c) < 24) {
        return c;
    }
    // neutral mid gray pen outlines (inactive progress bar frames) are drawn black as well
    if (pen && !(c & 0xFF000000)) {
        int r = GetRValue(c), g = GetGValue(c), b = GetBValue(c);
        int mx = std::max(r, std::max(g, b)), mn = std::min(r, std::min(g, b));
        if (mx - mn < 8 && mx >= 96 && mx <= 192) {
            return RGB(0, 0, 0);
        }
    }
    return MapColor(c);
}

// ---- bitmap helpers -------------------------------------------------------------

// Re-colors a bitmap in place. A bitmap has few distinct colors, so a small lookup table (colors
// seen before are looked up instead of converted again) makes this several times faster.
static void MapPixels(uint8_t* base, int width, int height, int stride, int bpp) {
    const int step = bpp / 8;
    const size_t pixels = (size_t)width * height;
    if (pixels < 4096) {
        for (int y = 0; y < height; y++) {
            uint8_t* row = base + (size_t)y * stride;
            for (int x = 0; x < width; x++) {
                uint8_t* p = row + x * step;
                MapRGB(p[2], p[1], p[0]);
            }
        }
        return;
    }
    const int kBits = 13;
    std::vector<uint32_t> cache((size_t)2 << kBits, 0xFFFFFFFFu);  // key, value pairs
    uint32_t lastIn = 0xFFFFFFFFu, lastOut = 0;
    for (int y = 0; y < height; y++) {
        uint8_t* row = base + (size_t)y * stride;
        for (int x = 0; x < width; x++) {
            uint8_t* p = row + x * step;
            const uint32_t in = ((uint32_t)p[2] << 16) | ((uint32_t)p[1] << 8) | p[0];
            uint32_t out;
            if (in == lastIn) {
                out = lastOut;
            } else {
                const size_t slot = (size_t)((in * 2654435761u) >> (32 - kBits)) * 2;
                if (cache[slot] == in) {
                    out = cache[slot + 1];
                } else {
                    uint8_t r = p[2], g = p[1], b = p[0];
                    MapRGB(r, g, b);
                    out = ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
                    cache[slot] = in;
                    cache[slot + 1] = out;
                }
                lastIn = in;
                lastOut = out;
            }
            p[2] = (uint8_t)(out >> 16);
            p[1] = (uint8_t)(out >> 8);
            p[0] = (uint8_t)out;
        }
    }
}

// true when a 32bpp bitmap carries real per pixel alpha (mixed alpha values)
static bool HasRealAlpha(const uint8_t* base, int width, int height, int stride) {
    bool seenZero = false, seenFull = false, seenMid = false;
    for (int y = 0; y < height; y++) {
        const uint8_t* row = base + (size_t)y * stride;
        for (int x = 0; x < width; x++) {
            uint8_t a = row[x * 4 + 3];
            if (a == 0) seenZero = true;
            else if (a == 255) seenFull = true;
            else seenMid = true;
            if ((seenZero && seenFull) || seenMid) {
                return true;
            }
        }
    }
    return false;
}

static void MarkDirty(HDC hdc) {
    if (!hdc || !g_csReady || GetObjectType(hdc) != OBJ_MEMDC) {
        return;
    }
    HBITMAP hbm = (HBITMAP)GetCurrentObject(hdc, OBJ_BITMAP);
    if (!hbm) {
        return;
    }
    EnterCriticalSection(&g_cs);
    auto it = g_bmps->find(hbm);
    if (it == g_bmps->end()) {
        BmpEntry e = {};
        e.state = 3;
        g_bmps->emplace(hbm, std::move(e));
    }
    LeaveCriticalSection(&g_cs);
}

// Re-colors a DIB section once, if it looks like static skin artwork.
static void PrepareHbm(HBITMAP hbm, bool hasAlphaBlend = false) {
    if (!hbm || !g_csReady) {
        return;
    }
    EnterCriticalSection(&g_cs);
    if (g_bmps->find(hbm) != g_bmps->end()) {
        LeaveCriticalSection(&g_cs);
        return;
    }
    BmpEntry e = {};
    e.state = 2;
    DIBSECTION ds;
    if (GetObjectW(hbm, sizeof(ds), &ds) == sizeof(ds) && ds.dsBm.bmBits &&
        (ds.dsBm.bmBitsPixel == 24 || ds.dsBm.bmBitsPixel == 32) && ds.dsBmih.biCompression == BI_RGB) {
        int w = ds.dsBm.bmWidth, h = abs(ds.dsBm.bmHeight), stride = ds.dsBm.bmWidthBytes;
        size_t size = (size_t)stride * h;
        uint8_t* bits = (uint8_t*)ds.dsBm.bmBits;
        bool alpha = ds.dsBm.bmBitsPixel == 32 && (hasAlphaBlend || HasRealAlpha(bits, w, h, stride));
        if (!alpha && size > 0 && size < (64u << 20)) {
            e.bits = bits;
            e.backup.assign(bits, bits + size);
            MapPixels(bits, w, h, stride, ds.dsBm.bmBitsPixel);
            e.state = 1;
        }
    }
    g_bmps->emplace(hbm, std::move(e));
    LeaveCriticalSection(&g_cs);
}

// Re-colors the bitmap selected into hdc once, if it looks like static skin artwork.
static void PrepareSource(HDC hdc, bool hasAlphaBlend = false) {
    if (!hdc || !g_csReady || GetObjectType(hdc) != OBJ_MEMDC) {
        return;
    }
    PrepareHbm((HBITMAP)GetCurrentObject(hdc, OBJ_BITMAP), hasAlphaBlend);
}

// ---- surface gradient ------------------------------------------------------------
// The skin surface is #283F5C after the dark mapping (lightness 26%, hue 213). Pixels of that
// color family are re-colored with a gradient when they are drawn to a window.

static const float kSkinLightness = 0.26f;
static struct {
    float h[256], s[256], l[256];
} g_gradient;

static void BuildGradient() {
    float bias = fminf(0.95f, fmaxf(0.05f, g_cfg.balance));
    for (int i = 0; i < 256; i++) {
        float t0 = i / 255.0f;
        float t = t0 / ((1.0f / bias - 2.0f) * (1.0f - t0) + 1.0f);  // moves the middle of the gradient
        // interpolate in RGB so that the hue does not take a detour
        float r = (GetRValue(g_cfg.colorStart) * (1 - t) + GetRValue(g_cfg.colorEnd) * t) / 255.0f;
        float g = (GetGValue(g_cfg.colorStart) * (1 - t) + GetGValue(g_cfg.colorEnd) * t) / 255.0f;
        float b = (GetBValue(g_cfg.colorStart) * (1 - t) + GetBValue(g_cfg.colorEnd) * t) / 255.0f;
        RgbToHsl(r, g, b, &g_gradient.h[i], &g_gradient.s[i], &g_gradient.l[i]);
        g_gradient.l[i] = fminf(0.45f, g_gradient.l[i]);  // the text stays light: the surface must stay dark
    }
}

static float Smooth(float edge0, float edge1, float x) {
    float t = fminf(1.0f, fmaxf(0.0f, (x - edge0) / (edge1 - edge0)));
    return t * t * (3.0f - 2.0f * t);
}

// base: BGRA or BGR pixels; t0 is the gradient position of the first pixel.
// Pixels of the skin color family are re-colored, anti-aliased text edges (lighter or less
// saturated mixes of skin and text color) are only tinted a little, so text stays sharp.
static void TintSkin(uint8_t* base, int width, int height, int stride, int bytesPerPixel, float t0, float dtx, float dty) {
    // The result for a pixel depends on its color and on the gradient step only. For the current skin
    // color there is a table with one entry per step; flat areas repeat the same few colors, so the
    // per-pixel work is a look-up.
    uint32_t tabKey = 0xFFFFFFFFu;
    float tabW = 0.0f, tabL0 = 0.0f;
    uint32_t tab[256];
    for (int y = 0; y < height; y++) {
        uint8_t* row = base + (size_t)y * stride;
        float t = t0 + y * dty;
        for (int x = 0; x < width; x++, t += dtx) {
            uint8_t* p = row + x * bytesPerPixel;
            int b = p[0], g = p[1], r = p[2];
            int mx = b > g ? (b > r ? b : r) : (g > r ? g : r);
            int mn = b < g ? (b < r ? b : r) : (g < r ? g : r);
            if (b != mx || mx - mn < 12 || mx > 200) {
                continue;  // not blue and dark enough
            }
            const uint32_t key = ((uint32_t)r << 16) | ((uint32_t)g << 8) | (uint32_t)b;
            if (key != tabKey) {
                float h0, s0, l0;
                RgbToHsl(r / 255.0f, g / 255.0f, b / 255.0f, &h0, &s0, &l0);
                float hueDeg = h0 * 60.0f;
                tabW = (1.0f - Smooth(0.34f, 0.55f, l0)) * Smooth(0.05f, 0.10f, l0) *
                       Smooth(0.10f, 0.28f, s0) * Smooth(200.0f, 208.0f, hueDeg) *
                       (1.0f - Smooth(224.0f, 236.0f, hueDeg));
                tabL0 = l0;
                tabKey = key;
                memset(tab, 0xFF, sizeof(tab));
            }
            if (tabW <= 0.01f) {
                continue;
            }
            int idx = (int)(fminf(1.0f, fmaxf(0.0f, t)) * 255.0f + 0.5f);
            uint32_t v = tab[idx];
            if (v == 0xFFFFFFFFu) {
                float l1 = fminf(0.6f, g_gradient.l[idx] * (tabL0 / kSkinLightness));
                uint8_t nr, ng, nb;
                HslToRgb8(g_gradient.h[idx], g_gradient.s[idx], l1, &nr, &ng, &nb);
                const uint32_t fr = (uint32_t)(r + (nr - r) * tabW + 0.5f);
                const uint32_t fg = (uint32_t)(g + (ng - g) * tabW + 0.5f);
                const uint32_t fb = (uint32_t)(b + (nb - b) * tabW + 0.5f);
                v = (fr << 16) | (fg << 8) | fb;
                tab[idx] = v;
            }
            p[2] = (uint8_t)(v >> 16);
            p[1] = (uint8_t)(v >> 8);
            p[0] = (uint8_t)v;
        }
    }
}




// ---- Setup tree: use the color of the surface ----------------------------------------------

static const COLORREF kOriginalSkin = RGB(187, 206, 230);  // #BBCEE6, the light blue of the original skin

static bool WindowClassIs(HWND wnd, const wchar_t* name) {
    wchar_t cls[64];
    return wnd && GetClassNameW(wnd, cls, ARRAYSIZE(cls)) && wcscmp(cls, name) == 0;
}

// buttons (their hover face) and the lists keep their own colors: the surface tint leaves them alone
static bool IsSkinButtonDC(HDC d) {
    HWND wnd = WindowFromDC(d);
    static HWND lastWnd = nullptr;
    static bool lastIsButton = false;
    if (wnd != lastWnd) {
        lastWnd = wnd;
        lastIsButton = wnd && (WindowClassIs(wnd, L"TbsSkinButton") || WindowClassIs(wnd, L"TVirtualStringTree"));
    }
    return lastIsButton;
}

static bool IsSetupTreeWindow(HDC d) {
    HWND wnd = WindowFromDC(d);
    return wnd && WindowClassIs(wnd, L"TVirtualStringTree") && WindowClassIs(GetAncestor(wnd, GA_ROOT), L"TSetupForm");
}

// The color the surface has at the center of the window: base skin color or gradient color.
static void SurfaceColorAt(HWND wnd, uint8_t* r, uint8_t* g, uint8_t* b) {
    *r = GetRValue(kOriginalSkin);
    *g = GetGValue(kOriginalSkin);
    *b = GetBValue(kOriginalSkin);
    MapRGB(*r, *g, *b);
    if (!g_cfg.gradient) {
        return;
    }
    HWND root = GetAncestor(wnd, GA_ROOT);
    RECT wr, rc;
    GetWindowRect(wnd, &wr);
    POINT c = {(wr.left + wr.right) / 2, (wr.top + wr.bottom) / 2};
    ScreenToClient(root, &c);
    GetClientRect(root, &rc);
    float W = (float)rc.right, H = (float)rc.bottom;
    float rad = (float)g_cfg.angle * 3.14159265f / 180.0f;
    float dx = cosf(rad), dy = sinf(rad);
    float extent = fabsf(W * dx) + fabsf(H * dy);
    if (extent < 1.0f) {
        extent = 1.0f;
    }
    float t = ((c.x - W * 0.5f) * dx + (c.y - H * 0.5f) * dy) / extent + 0.5f;
    int idx = (int)(fminf(1.0f, fmaxf(0.0f, t)) * 255.0f + 0.5f);
    HslToRgb8(g_gradient.h[idx], g_gradient.s[idx], g_gradient.l[idx], r, g, b);
}

// Neutral (gray) pixels are mixed toward the surface color: the background becomes the surface
// color, white text stays white, anti-aliased edges are mixed in between.
static void SurfaceTree(uint8_t* base, int width, int height, int stride, uint8_t sr, uint8_t sg, uint8_t sb) {
    uint8_t br = 255, bg = 255, bb = 255;
    MapRGB(br, bg, bb);  // the dark list background
    float bgMax = (float)br;
    for (int y = 0; y < height; y++) {
        uint8_t* row = base + (size_t)y * stride;
        for (int x = 0; x < width; x++) {
            uint8_t* p = row + x * 4;
            int b = p[0], g = p[1], r = p[2];
            int mx = b > g ? (b > r ? b : r) : (g > r ? g : r);
            int mn = b < g ? (b < r ? b : r) : (g < r ? g : r);
            if (mx - mn > 12) {
                continue;  // colored pixel (selection, icons): leave it
            }
            float a = fminf(1.0f, fmaxf(0.0f, (mx - bgMax) / (255.0f - bgMax)) * 2.5f);  // tree lines and text edges become bright
            p[2] = (uint8_t)(r * a + sr * (1.0f - a) + 0.5f);
            p[1] = (uint8_t)(g * a + sg * (1.0f - a) + 0.5f);
            p[0] = (uint8_t)(b * a + sb * (1.0f - a) + 0.5f);
        }
    }
}

// ---- hooks: colors ---------------------------------------------------------------

typedef DWORD(WINAPI* GetSysColor_t)(int);
static GetSysColor_t GetSysColor_orig;
static DWORD WINAPI GetSysColor_hook(int index) {
    DWORD c = GetSysColor_orig(index);
    if (IsSelectionColorIndex(index) || !ShouldMap(__builtin_return_address(0))) {
        return c;
    }
    return MapColor(c);
}

typedef HBRUSH(WINAPI* CreateSolidBrush_t)(COLORREF);
static CreateSolidBrush_t CreateSolidBrush_orig;

static HBRUSH MappedSysBrush(int index) {
    HBRUSH b = g_sysBrushes[index];
    if (!b) {
        b = CreateSolidBrush_orig(MapColor(GetSysColor_orig(index)));
        g_sysBrushes[index] = b;
    }
    return b;
}

typedef HBRUSH(WINAPI* GetSysColorBrush_t)(int);
static GetSysColorBrush_t GetSysColorBrush_orig;
static HBRUSH WINAPI GetSysColorBrush_hook(int index) {
    if (index < 0 || index >= 32 || IsSelectionColorIndex(index) || !ShouldMap(__builtin_return_address(0))) {
        return GetSysColorBrush_orig(index);
    }
    return MappedSysBrush(index);
}

// color of the progress fill at gradient position t (0..1) with the shading (lightness) l of the original
static COLORREF ProgressColor(float t, float l) {
    // blend the two colors in RGB (a hue blend would run through other colors, especially to white),
    // the lightness follows the original shading, shifted by the lightness of the blended color
    float r = GetRValue(g_cfg.progressC1) / 255.0f, g = GetGValue(g_cfg.progressC1) / 255.0f, b = GetBValue(g_cfg.progressC1) / 255.0f;
    if (g_cfg.progressGradient && t > 0.0f) {
        r += (GetRValue(g_cfg.progressC2) / 255.0f - r) * t;
        g += (GetGValue(g_cfg.progressC2) / 255.0f - g) * t;
        b += (GetBValue(g_cfg.progressC2) / 255.0f - b) * t;
    }
    float h, sa, lc;
    RgbToHsl(r, g, b, &h, &sa, &lc);
    uint8_t r8, g8, b8;
    HslToRgb8(h, sa, fminf(0.95f, fmaxf(0.02f, l + (lc - g_cfg.progressL1))), &r8, &g8, &b8);
    return RGB(r8, g8, b8);
}

// true for the brush that has the (re-colored) light blue of the skin
static bool IsSkinColorBrush(HBRUSH br) {
    LOGBRUSH lb;
    if (GetObjectW(br, sizeof(lb), &lb) != sizeof(lb) || lb.lbStyle != BS_SOLID) {
        return false;
    }
    uint8_t r = GetRValue(kOriginalSkin), g = GetGValue(kOriginalSkin), b = GetBValue(kOriginalSkin);
    MapRGB(r, g, b);
    return abs((int)GetRValue(lb.lbColor) - r) <= 12 && abs((int)GetGValue(lb.lbColor) - g) <= 12 &&
           abs((int)GetBValue(lb.lbColor) - b) <= 12;
}

typedef int(WINAPI* FillRect_t)(HDC, const RECT*, HBRUSH);
static FillRect_t FillRect_orig;
static int WINAPI FillRect_hook(HDC hdc, const RECT* rc, HBRUSH br) {
    MarkDirty(hdc);
    if ((ULONG_PTR)br > 0 && (ULONG_PTR)br <= 32) {
        int index = (int)(ULONG_PTR)br - 1;
        if (!IsSelectionColorIndex(index) && ShouldMap(__builtin_return_address(0), hdc)) {
            br = MappedSysBrush(index);
        }
    } else if (g_cfg.headerOn && rc && rc->right - rc->left >= 4 && rc->top == 0 && rc->bottom - rc->top >= 15 && rc->bottom - rc->top <= 22 &&
               ShouldMap(__builtin_return_address(0), hdc) && IsSkinColorBrush(br)) {
        // the header bars of the lists are filled with the color of the skin
        HBRUSH tmp = CreateSolidBrush_orig(g_cfg.headerColor);
        int res = FillRect_orig(hdc, rc, tmp);
        DeleteObject(tmp);
        return res;
    } else if (g_cfg.progressOn && rc && rc->right - rc->left >= 1 && rc->bottom - rc->top >= 8 && rc->bottom - rc->top <= 24 &&
               ShouldMap(__builtin_return_address(0), hdc)) {
        // the small bars of the connection list: a gray background piece, then the fill in narrow strips
        LOGBRUSH lb;
        if (GetObjectW(br, sizeof(lb), &lb) == sizeof(lb) && lb.lbStyle == BS_SOLID) {
            float h, sat, l;
            RgbToHsl(GetRValue(lb.lbColor) / 255.0f, GetGValue(lb.lbColor) / 255.0f, GetBValue(lb.lbColor) / 255.0f, &h, &sat, &l);
            static int barLeft = 0, barWidth = 221;
            const int w = rc->right - rc->left, hgt = rc->bottom - rc->top;
            if (sat < 0.1f && w >= 100) {
                barLeft = rc->left;
                barWidth = w;
            } else if (sat > 0.25f && w <= 12) {
                int res = 1;
                if (g_cfg.progressGradient && !g_cfg.progressHorizontal) {
                    for (int y = 0; y < hgt; y += 2) {
                        RECT band = {rc->left, rc->top + y, rc->right, rc->top + std::min(hgt, y + 2)};
                        HBRUSH tmp = CreateSolidBrush_orig(ProgressColor((y + 1) / (float)hgt, l));
                        res = FillRect_orig(hdc, &band, tmp);
                        DeleteObject(tmp);
                    }
                } else {
                    float t = fminf(1.0f, fmaxf(0.0f, (rc->left - barLeft + w * 0.5f) / (float)barWidth));
                    HBRUSH tmp = CreateSolidBrush_orig(ProgressColor(t, l));
                    res = FillRect_orig(hdc, rc, tmp);
                    DeleteObject(tmp);
                }
                return res;
            }
        }
    } else if (g_cfg.progressOn && rc && rc->bottom - rc->top == 1 && rc->right - rc->left >= 8 &&
               ShouldMap(__builtin_return_address(0), hdc)) {
        // the progress bars are filled line by line: take the hue of the chosen color, keep the shading
        LOGBRUSH lb;
        if (GetObjectW(br, sizeof(lb), &lb) == sizeof(lb) && lb.lbStyle == BS_SOLID) {
            float h, sat, l;
            RgbToHsl(GetRValue(lb.lbColor) / 255.0f, GetGValue(lb.lbColor) / 255.0f, GetBValue(lb.lbColor) / 255.0f, &h, &sat, &l);
            if (sat > 0.25f) {
                if (g_cfg.progressHorizontal && g_cfg.progressGradient) {
                    // left to right: the fill is drawn in short pieces, the gradient spans the filled part
                    const int step = 3, width = rc->right - rc->left;
                    int res = 1;
                    for (int x = 0; x < width; x += step) {
                        RECT piece = {rc->left + x, rc->top, rc->left + std::min(width, x + step), rc->bottom};
                        HBRUSH tmp = CreateSolidBrush_orig(ProgressColor((x + step / 2) / (float)width, l));
                        res = FillRect_orig(hdc, &piece, tmp);
                        DeleteObject(tmp);
                    }
                    return res;
                }
                // top to bottom: line index inside the bar (the bar is drawn from top to bottom, 15 lines)
                static int lastTop = -100, line = 0;
                line = rc->top == lastTop + 1 ? line + 1 : 0;
                lastTop = rc->top;
                HBRUSH tmp = CreateSolidBrush_orig(ProgressColor(fminf(1.0f, line / 14.0f), l));
                int res = FillRect_orig(hdc, rc, tmp);
                DeleteObject(tmp);
                return res;
            }
        }
    }
    return FillRect_orig(hdc, rc, br);
}

static HBRUSH WINAPI CreateSolidBrush_hook(COLORREF c) {
    return CreateSolidBrush_orig(ShouldMap(__builtin_return_address(0)) ? MapShapeColor(c) : c);
}

typedef HBRUSH(WINAPI* CreateBrushIndirect_t)(const LOGBRUSH*);
static CreateBrushIndirect_t CreateBrushIndirect_orig;
static HBRUSH WINAPI CreateBrushIndirect_hook(const LOGBRUSH* lb) {
    if (lb && (lb->lbStyle == BS_SOLID || lb->lbStyle == BS_HATCHED) && ShouldMap(__builtin_return_address(0))) {
        LOGBRUSH copy = *lb;
        copy.lbColor = MapShapeColor(copy.lbColor);
        return CreateBrushIndirect_orig(&copy);
    }
    return CreateBrushIndirect_orig(lb);
}

typedef HPEN(WINAPI* CreatePen_t)(int, int, COLORREF);
static CreatePen_t CreatePen_orig;
static HPEN WINAPI CreatePen_hook(int style, int width, COLORREF c) {
    return CreatePen_orig(style, width, ShouldMap(__builtin_return_address(0)) ? MapShapeColor(c, true) : c);
}

typedef HPEN(WINAPI* CreatePenIndirect_t)(const LOGPEN*);
static CreatePenIndirect_t CreatePenIndirect_orig;
static HPEN WINAPI CreatePenIndirect_hook(const LOGPEN* lp) {
    if (lp && ShouldMap(__builtin_return_address(0))) {
        LOGPEN copy = *lp;
        copy.lopnColor = MapShapeColor(copy.lopnColor, true);
        return CreatePenIndirect_orig(&copy);
    }
    return CreatePenIndirect_orig(lp);
}

typedef HPEN(WINAPI* ExtCreatePen_t)(DWORD, DWORD, const LOGBRUSH*, DWORD, const DWORD*);
static ExtCreatePen_t ExtCreatePen_orig;
static HPEN WINAPI ExtCreatePen_hook(DWORD style, DWORD width, const LOGBRUSH* lb, DWORD n, const DWORD* dash) {
    if (lb && lb->lbStyle == BS_SOLID && ShouldMap(__builtin_return_address(0))) {
        LOGBRUSH copy = *lb;
        copy.lbColor = MapShapeColor(copy.lbColor, true);
        return ExtCreatePen_orig(style, width, &copy, n, dash);
    }
    return ExtCreatePen_orig(style, width, lb, n, dash);
}

typedef COLORREF(WINAPI* SetColor_t)(HDC, COLORREF);
static SetColor_t SetTextColor_orig, SetBkColor_orig, SetDCBrushColor_orig, SetDCPenColor_orig;
typedef COLORREF(WINAPI* GetColor_t)(HDC);
static GetColor_t GetTextColor_orig, GetBkColor_orig;

// The app asks for the colors it has set: remember its originals per DC.
static bool IsMenuDC(HDC hdc) {
    HWND wnd = WindowFromDC(hdc);
    wchar_t cls[16];
    return wnd && GetClassNameW(wnd, cls, ARRAYSIZE(cls)) && wcscmp(cls, L"#32768") == 0;
}

static COLORREF SetMappedColor(std::unordered_map<HDC, COLORREF>* store, SetColor_t orig, HDC hdc, COLORREF c, bool map) {
    if (!map) {
        EnterCriticalSection(&g_cs);
        store->erase(hdc);
        LeaveCriticalSection(&g_cs);
        return orig(hdc, c);
    }
    // white text on the selection color (selected rows, selected text) stays white
    bool keep = orig == SetTextColor_orig && !(c & 0xFF000000) && c == GetSysColor_orig(COLOR_HIGHLIGHTTEXT) && !IsMenuDC(hdc);
    if (keep && GetObjectType(hdc) == OBJ_MEMDC) {
        // small bitmaps are icon masks (menu icons): map as usual
        BITMAP bm;
        HBITMAP cur = (HBITMAP)GetCurrentObject(hdc, OBJ_BITMAP);
        if (cur && GetObjectW(cur, sizeof(bm), &bm) == sizeof(bm) && bm.bmWidth <= 48 && bm.bmHeight <= 48) {
            keep = false;
        }
    }
    COLORREF prev = orig(hdc, keep ? c : MapColor(c));
    EnterCriticalSection(&g_cs);
    auto it = store->find(hdc);
    if (it != store->end()) {
        prev = it->second;
    }
    (*store)[hdc] = c;
    LeaveCriticalSection(&g_cs);
    return prev;
}

static COLORREF GetStoredColor(std::unordered_map<HDC, COLORREF>* store, GetColor_t orig, HDC hdc) {
    COLORREF cur = orig(hdc);
    EnterCriticalSection(&g_cs);
    auto it = store->find(hdc);
    if (it != store->end()) {
        cur = it->second;
    }
    LeaveCriticalSection(&g_cs);
    return cur;
}

static COLORREF WINAPI SetTextColor_hook(HDC hdc, COLORREF c) {
    return SetMappedColor(g_textColors, SetTextColor_orig, hdc, c, ShouldMap(__builtin_return_address(0), hdc));
}
static COLORREF WINAPI GetTextColor_hook(HDC hdc) {
    return GetStoredColor(g_textColors, GetTextColor_orig, hdc);
}
static COLORREF WINAPI SetBkColor_hook(HDC hdc, COLORREF c) {
    return SetMappedColor(g_bkColors, SetBkColor_orig, hdc, c, ShouldMap(__builtin_return_address(0), hdc));
}
static COLORREF WINAPI GetBkColor_hook(HDC hdc) {
    return GetStoredColor(g_bkColors, GetBkColor_orig, hdc);
}
static COLORREF WINAPI SetDCBrushColor_hook(HDC hdc, COLORREF c) {
    return SetDCBrushColor_orig(hdc, ShouldMap(__builtin_return_address(0), hdc) ? MapShapeColor(c) : c);
}
static COLORREF WINAPI SetDCPenColor_hook(HDC hdc, COLORREF c) {
    return SetDCPenColor_orig(hdc, ShouldMap(__builtin_return_address(0), hdc) ? MapShapeColor(c, true) : c);
}

typedef COLORREF(WINAPI* SetPixel_t)(HDC, int, int, COLORREF);
static SetPixel_t SetPixel_orig;
static COLORREF WINAPI SetPixel_hook(HDC hdc, int x, int y, COLORREF c) {
    return SetPixel_orig(hdc, x, y, ShouldMap(__builtin_return_address(0), hdc) ? MapColor(c) : c);
}
typedef BOOL(WINAPI* SetPixelV_t)(HDC, int, int, COLORREF);
static SetPixelV_t SetPixelV_orig;
static BOOL WINAPI SetPixelV_hook(HDC hdc, int x, int y, COLORREF c) {
    return SetPixelV_orig(hdc, x, y, ShouldMap(__builtin_return_address(0), hdc) ? MapColor(c) : c);
}

// ---- hooks: bitmaps ---------------------------------------------------------------

typedef BOOL(WINAPI* BitBlt_t)(HDC, int, int, int, int, HDC, int, int, DWORD);
static BitBlt_t BitBlt_orig;

// The "show/hide connections panel" glyph at the right end of the status bar is black on the skin:
// after the status bar painted it, everything darker than the surface there is brightened.
static void FixStatusGlyph(HDC d) {
    if (GetObjectType(d) != OBJ_DC) {
        return;
    }
    HWND wnd = WindowFromDC(d);
    static HWND lastWnd = nullptr;
    static bool lastIsBar = false;
    if (wnd != lastWnd) {
        lastWnd = wnd;
        lastIsBar = WindowClassIs(wnd, L"TbsSkinStatusBar");
    }
    if (!lastIsBar) {
        return;
    }
    RECT cr;
    GetClientRect(wnd, &cr);
    const int x0 = cr.right - 38, w = 24, h = cr.bottom;
    if (x0 < 0 || h <= 0) {
        return;
    }
    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = w;
    bi.bmiHeader.biHeight = -h;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    uint8_t* bits = nullptr;
    HBITMAP dib = CreateDIBSection(d, &bi, DIB_RGB_COLORS, (void**)&bits, nullptr, 0);
    if (!dib || !bits) {
        if (dib) {
            DeleteObject(dib);
        }
        return;
    }
    HDC mem = CreateCompatibleDC(d);
    HGDIOBJ old = SelectObject(mem, dib);
    BitBlt_orig(mem, 0, 0, w, h, d, x0, 0, SRCCOPY);
    bool changed = false;
    for (int y = 0; y < h; y++) {
        uint8_t* row = bits + (size_t)y * w * 4;
        // reference: the brightest background pixel of the row (surface or hover face), without the
        // gray glyph pixels this function has set before
        float ref = 0.0f;
        int refx = 1;
        for (int x = 1; x < w - 1; x++) {
            uint8_t* px = row + x * 4;
            float l = px[2] * 0.3f + px[1] * 0.59f + px[0] * 0.11f;
            if (l < 140.0f && l > ref) {
                ref = l;
                refx = x;
            }
        }
        if (ref < 12.0f) {
            continue;
        }
        uint8_t refc[3] = {row[refx * 4], row[refx * 4 + 1], row[refx * 4 + 2]};
        for (int x = 1; x < w - 1; x++) {
            uint8_t* px = row + x * 4;
            float l = px[2] * 0.3f + px[1] * 0.59f + px[0] * 0.11f;
            float a = (ref - l) / ref;
            if (a > 0.3f) {
                a = fminf(1.0f, a * 1.2f);
                for (int c = 0; c < 3; c++) {
                    px[c] = (uint8_t)(refc[c] + (150 - refc[c]) * a + 0.5f);  // soft gray
                }
                changed = true;
            }
        }
    }
    if (changed) {
        BitBlt_orig(d, x0, 0, w, h, mem, 0, 0, SRCCOPY);
    }
    SelectObject(mem, old);
    DeleteDC(mem);
    DeleteObject(dib);
}

// Graphics32 (TImage32) keeps rewriting the pixels of its buffer, so a one time in-place
// conversion does not work for it: convert a copy on every blit instead.
static bool IsImage32Window(HDC d) {
    HWND wnd = WindowFromDC(d);
    if (!wnd) {
        return false;
    }
    char cls[32];
    return GetClassNameA(wnd, cls, sizeof(cls)) && strcmp(cls, "TImage32") == 0;
}

typedef BOOL(WINAPI* StretchBlt_t)(HDC, int, int, int, int, HDC, int, int, int, int, DWORD);
static StretchBlt_t StretchBlt_orig;

// Copies the source into a temporary bitmap, re-colors the copy, and draws that:
//  * TImage32 (Graphics32) buffers: all pixels are re-colored on every blit
//  * with the gradient enabled: the skin colored pixels get the gradient color
// One temporary bitmap with its memory DC is reused for the blits that are processed (creating and
// deleting both for each of the thousands of blits of a repaint was a large part of the cost). If another
// thread holds it, the blit uses a bitmap of its own.
static CRITICAL_SECTION g_tempCs;
static HDC g_tempDc;
static HBITMAP g_tempBmp;
static HGDIOBJ g_tempOld;
static void* g_tempBits;
static int g_tempW, g_tempH;

static bool EnsureTempBitmap(HDC like, int w, int h) {
    if (g_tempDc && g_tempW >= w && g_tempH >= h) {
        return true;
    }
    const int nw = std::max(g_tempW, (w + 63) & ~63), nh = std::max(g_tempH, (h + 63) & ~63);
    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = nw;
    bi.bmiHeader.biHeight = -nh;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    HBITMAP bmp = CreateDIBSection(like, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!bmp || !bits) {
        if (bmp) {
            DeleteObject(bmp);
        }
        return false;
    }
    HDC dc = g_tempDc ? g_tempDc : CreateCompatibleDC(like);
    if (!dc) {
        DeleteObject(bmp);
        return false;
    }
    HGDIOBJ old = SelectObject(dc, bmp);
    if (g_tempDc) {
        DeleteObject(g_tempBmp);  // the previous bitmap was selected into the same DC: now free
    } else {
        g_tempOld = old;
    }
    g_tempDc = dc;
    g_tempBmp = bmp;
    g_tempBits = bits;
    g_tempW = nw;
    g_tempH = nh;
    return true;
}

static void FreeTempBitmap() {
    if (g_tempDc) {
        SelectObject(g_tempDc, g_tempOld);
        DeleteObject(g_tempBmp);
        DeleteDC(g_tempDc);
        g_tempDc = nullptr;
        g_tempBmp = nullptr;
        g_tempBits = nullptr;
        g_tempW = g_tempH = 0;
    }
}

static bool BlitProcessed(HDC d, int x, int y, int w, int h, HDC s, int sx, int sy, int sw, int sh, DWORD rop,
                          bool stretch) {
    if (!s || w <= 0 || h <= 0 || w > 4096 || h > 4096) {
        return false;
    }
    const bool image32 = IsImage32Window(d);
    const bool tint = g_cfg.gradient && !image32 && !IsSkinButtonDC(d) && rop == SRCCOPY && (long)w * h >= 600 &&
                      GetObjectType(s) == OBJ_MEMDC && IsAppDC(d);
    const bool surfaceTree = !g_cfg.keepSetupTreeDark && !image32 && rop == SRCCOPY && (long)w * h >= 600 &&
                             GetObjectType(s) == OBJ_MEMDC && IsSetupTreeWindow(d);
    if (!image32 && !tint && !surfaceTree) {
        return false;
    }

    // only small blits use the shared bitmap, the few large ones allocate their own (keeps the pool small)
    const bool pooled = w <= 1024 && h <= 1024 && TryEnterCriticalSection(&g_tempCs) != FALSE;
    HDC mem = nullptr;
    HBITMAP tmp = nullptr;
    HGDIOBJ old = nullptr;
    void* bits = nullptr;
    int bstride = w * 4;
    if (pooled && EnsureTempBitmap(d, w, h)) {
        mem = g_tempDc;
        bits = g_tempBits;
        bstride = g_tempW * 4;
    } else {
        if (pooled) {
            LeaveCriticalSection(&g_tempCs);
        }
        BITMAPINFO bi = {};
        bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bi.bmiHeader.biWidth = w;
        bi.bmiHeader.biHeight = -h;
        bi.bmiHeader.biPlanes = 1;
        bi.bmiHeader.biBitCount = 32;
        bi.bmiHeader.biCompression = BI_RGB;
        tmp = CreateDIBSection(d, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
        if (!tmp || !bits) {
            return false;
        }
        mem = CreateCompatibleDC(d);
        old = SelectObject(mem, tmp);
    }
    const bool usingPool = tmp == nullptr;
    if (stretch) {
        StretchBlt_orig(mem, 0, 0, w, h, s, sx, sy, sw, sh, SRCCOPY);
    } else {
        BitBlt_orig(mem, 0, 0, w, h, s, sx, sy, SRCCOPY);
    }

    if (surfaceTree) {
        uint8_t sr, sg, sb;
        SurfaceColorAt(WindowFromDC(d), &sr, &sg, &sb);
        SurfaceTree((uint8_t*)bits, w, h, bstride, sr, sg, sb);
    } else if (image32) {
        MapPixels((uint8_t*)bits, w, h, bstride, 32);
    } else {
        HWND wnd = WindowFromDC(d);
        HWND root = GetAncestor(wnd, GA_ROOT);
        POINT origin = {x, y};
        MapWindowPoints(wnd, root, &origin, 1);
        RECT rc;
        GetClientRect(root, &rc);
        float W = (float)rc.right, H = (float)rc.bottom;
        float rad = (float)g_cfg.angle * 3.14159265f / 180.0f;
        float dx = cosf(rad), dy = sinf(rad);
        float extent = fabsf(W * dx) + fabsf(H * dy);
        if (extent < 1.0f) {
            extent = 1.0f;
        }
        float t0 = ((origin.x - W * 0.5f) * dx + (origin.y - H * 0.5f) * dy) / extent + 0.5f;
        TintSkin((uint8_t*)bits, w, h, bstride, 4, t0, dx / extent, dy / extent);
    }

    BOOL ok = BitBlt_orig(d, x, y, w, h, mem, 0, 0, rop);
    if (usingPool) {
        LeaveCriticalSection(&g_tempCs);
    } else {
        SelectObject(mem, old);
        DeleteDC(mem);
        DeleteObject(tmp);
    }
    return ok != FALSE;
}

static BOOL WINAPI BitBlt_hook(HDC d, int x, int y, int w, int h, HDC s, int sx, int sy, DWORD rop) {
    if (ShouldMap(__builtin_return_address(0), d)) {
        if (!IsImage32Window(d)) {
            PrepareSource(s);
        }
        if (BlitProcessed(d, x, y, w, h, s, sx, sy, w, h, rop, false)) {
            return TRUE;
        }
    }
    MarkDirty(d);
    auto result = BitBlt_orig(d, x, y, w, h, s, sx, sy, rop);
    FixStatusGlyph(d);
    return result;
}

static BOOL WINAPI StretchBlt_hook(HDC d, int x, int y, int w, int h, HDC s, int sx, int sy, int sw, int sh, DWORD rop) {
    if (ShouldMap(__builtin_return_address(0), d)) {
        PrepareSource(s);
        if (BlitProcessed(d, x, y, w, h, s, sx, sy, sw, sh, rop, true)) {
            return TRUE;
        }
    }
    MarkDirty(d);
    auto result = StretchBlt_orig(d, x, y, w, h, s, sx, sy, sw, sh, rop);
    FixStatusGlyph(d);
    return result;
}

typedef BOOL(WINAPI* MaskBlt_t)(HDC, int, int, int, int, HDC, int, int, HBITMAP, int, int, DWORD);
static MaskBlt_t MaskBlt_orig;
static BOOL WINAPI MaskBlt_hook(HDC d, int x, int y, int w, int h, HDC s, int sx, int sy, HBITMAP m, int mx, int my, DWORD rop) {
    if (ShouldMap(__builtin_return_address(0), d)) {
        PrepareSource(s);
    }
    MarkDirty(d);
    auto result = MaskBlt_orig(d, x, y, w, h, s, sx, sy, m, mx, my, rop);
    FixStatusGlyph(d);
    return result;
}

typedef BOOL(WINAPI* GdiTransparentBlt_t)(HDC, int, int, int, int, HDC, int, int, int, int, UINT);
static GdiTransparentBlt_t GdiTransparentBlt_orig;
static BOOL WINAPI GdiTransparentBlt_hook(HDC d, int x, int y, int w, int h, HDC s, int sx, int sy, int sw, int sh, UINT key) {
    bool map = ShouldMap(__builtin_return_address(0), d);
    if (map) {
        PrepareSource(s);
    }
    MarkDirty(d);
    auto result = GdiTransparentBlt_orig(d, x, y, w, h, s, sx, sy, sw, sh, map ? MapColor(key) : key);
    FixStatusGlyph(d);
    return result;
}

typedef BOOL(WINAPI* GdiAlphaBlend_t)(HDC, int, int, int, int, HDC, int, int, int, int, BLENDFUNCTION);
static GdiAlphaBlend_t GdiAlphaBlend_orig;
static BOOL WINAPI GdiAlphaBlend_hook(HDC d, int x, int y, int w, int h, HDC s, int sx, int sy, int sw, int sh, BLENDFUNCTION bf) {
    if (ShouldMap(__builtin_return_address(0), d)) {
        PrepareSource(s, (bf.AlphaFormat & AC_SRC_ALPHA) != 0);
    }
    MarkDirty(d);
    return GdiAlphaBlend_orig(d, x, y, w, h, s, sx, sy, sw, sh, bf);
}

// Copies the pixels of a DIB that is passed to GDI as raw memory, re-colors the copy.
static bool MapDibCopy(const void* bits, const BITMAPINFO* bmi, UINT usage, std::vector<uint8_t>* out, UINT maxLines = 0) {
    // the header is the caller's input: only the full BITMAPINFOHEADER is understood, sizes are bounded
    if (!bits || !bmi || usage != DIB_RGB_COLORS || bmi->bmiHeader.biSize < sizeof(BITMAPINFOHEADER) ||
        bmi->bmiHeader.biCompression != BI_RGB ||
        (bmi->bmiHeader.biBitCount != 24 && bmi->bmiHeader.biBitCount != 32) || bmi->bmiHeader.biHeight == 0 ||
        bmi->bmiHeader.biWidth <= 0 || bmi->bmiHeader.biWidth > 16384 || bmi->bmiHeader.biHeight == INT32_MIN ||
        abs(bmi->bmiHeader.biHeight) > 16384) {
        return false;
    }
    int w = bmi->bmiHeader.biWidth, h = abs(bmi->bmiHeader.biHeight), bpp = bmi->bmiHeader.biBitCount;
    if (maxLines && (int)maxLines < h) {
        h = (int)maxLines;
    }
    int stride = ((w * bpp + 31) / 32) * 4;
    size_t size = (size_t)stride * h;
    if (size == 0 || size >= (64u << 20)) {
        return false;
    }
    out->assign((const uint8_t*)bits, (const uint8_t*)bits + size);
    if (bpp == 32 && HasRealAlpha(out->data(), w, h, stride)) {
        return false;
    }
    MapPixels(out->data(), w, h, stride, bpp);
    return true;
}

typedef int(WINAPI* StretchDIBits_t)(HDC, int, int, int, int, int, int, int, int, const void*, const BITMAPINFO*, UINT, DWORD);
static StretchDIBits_t StretchDIBits_orig;
static int WINAPI StretchDIBits_hook(HDC d, int xd, int yd, int wd, int hd, int xs, int ys, int ws, int hs,
                                     const void* bits, const BITMAPINFO* bmi, UINT usage, DWORD rop) {
    // Raw pixels the app hands over here are its own composed output (built from skin pieces that
    // were re-colored already): they are not re-colored again.
    MarkDirty(d);
    auto result = StretchDIBits_orig(d, xd, yd, wd, hd, xs, ys, ws, hs, bits, bmi, usage, rop);
    FixStatusGlyph(d);
    return result;
}

typedef int(WINAPI* SetDIBitsToDevice_t)(HDC, int, int, DWORD, DWORD, int, int, UINT, UINT, const void*, const BITMAPINFO*, UINT);
static SetDIBitsToDevice_t SetDIBitsToDevice_orig;
static int WINAPI SetDIBitsToDevice_hook(HDC d, int x, int y, DWORD w, DWORD h, int xs, int ys, UINT start, UINT lines,
                                         const void* bits, const BITMAPINFO* bmi, UINT usage) {
    MarkDirty(d);  // see StretchDIBits_hook: the pixels are not re-colored again
    auto result = SetDIBitsToDevice_orig(d, x, y, w, h, xs, ys, start, lines, bits, bmi, usage);
    FixStatusGlyph(d);
    return result;
}

// Raw pixel data that is copied into device bitmaps
typedef int(WINAPI* SetDIBits_t)(HDC, HBITMAP, UINT, UINT, const void*, const BITMAPINFO*, UINT);
static SetDIBits_t SetDIBits_orig;
static int WINAPI SetDIBits_hook(HDC hdc, HBITMAP hbm, UINT start, UINT lines, const void* bits, const BITMAPINFO* bmi, UINT usage) {
    std::vector<uint8_t> copy;
    bool mapped = ShouldMap(__builtin_return_address(0), nullptr) && MapDibCopy(bits, bmi, usage, &copy, lines);
    int res = SetDIBits_orig(hdc, hbm, start, lines, mapped ? copy.data() : bits, bmi, usage);
    if (mapped && hbm) {
        // the bitmap now holds re-colored pixels: it must not be re-colored again when it is used as a source
        EnterCriticalSection(&g_cs);
        BmpEntry e = {};
        e.state = 2;
        (*g_bmps)[hbm] = std::move(e);
        LeaveCriticalSection(&g_cs);
    }
    return res;
}

typedef HBITMAP(WINAPI* CreateDIBitmap_t)(HDC, const BITMAPINFOHEADER*, DWORD, const void*, const BITMAPINFO*, UINT);
static CreateDIBitmap_t CreateDIBitmap_orig;
static HBITMAP WINAPI CreateDIBitmap_hook(HDC hdc, const BITMAPINFOHEADER* hdr, DWORD init, const void* bits, const BITMAPINFO* bmi, UINT usage) {
    std::vector<uint8_t> copy;
    bool mapped = (init & CBM_INIT) && ShouldMap(__builtin_return_address(0), nullptr) && MapDibCopy(bits, bmi, usage, &copy);
    return CreateDIBitmap_orig(hdc, hdr, init, mapped ? copy.data() : bits, bmi, usage);
}

typedef BOOL(WINAPI* EndPaint_t)(HWND, const PAINTSTRUCT*);
static EndPaint_t EndPaint_orig;
static void ForgetDcColors(HDC dc) {
    EnterCriticalSection(&g_cs);
    g_textColors->erase(dc);
    g_bkColors->erase(dc);
    LeaveCriticalSection(&g_cs);
}

static BOOL WINAPI EndPaint_hook(HWND wnd, const PAINTSTRUCT* ps) {
    if (ps && ps->hdc && WindowClassIs(wnd, L"TbsSkinStatusBar")) {
        FixStatusGlyph(ps->hdc);
    }
    // window DCs are released, not deleted, and reused for other windows; a window with its own DC keeps
    // its attributes (and so its mapped colors), its entry must stay
    if (ps && ps->hdc && !(GetClassLongPtrW(wnd, GCL_STYLE) & (CS_OWNDC | CS_CLASSDC))) {
        ForgetDcColors(ps->hdc);
    }
    return EndPaint_orig(wnd, ps);
}

typedef int(WINAPI* ReleaseDC_t)(HWND, HDC);
static ReleaseDC_t ReleaseDC_orig;
static int WINAPI ReleaseDC_hook(HWND wnd, HDC dc) {
    if (dc && WindowClassIs(wnd, L"TbsSkinStatusBar")) {
        FixStatusGlyph(dc);
    }
    if (dc && !(GetClassLongPtrW(wnd, GCL_STYLE) & (CS_OWNDC | CS_CLASSDC))) {
        ForgetDcColors(dc);
    }
    return ReleaseDC_orig(wnd, dc);
}

typedef BOOL(WINAPI* GdiGradientFill_t)(HDC, PTRIVERTEX, ULONG, PVOID, ULONG, ULONG);
static GdiGradientFill_t GdiGradientFill_orig;
static BOOL WINAPI GdiGradientFill_hook(HDC hdc, PTRIVERTEX v, ULONG n, PVOID mesh, ULONG nm, ULONG mode) {
    MarkDirty(hdc);
    if (v && n > 0 && n < 4096 && ShouldMap(__builtin_return_address(0), hdc)) {
        std::vector<TRIVERTEX> copy(v, v + n);
        for (auto& t : copy) {
            uint8_t r = t.Red >> 8, g = t.Green >> 8, b = t.Blue >> 8;
            MapRGB(r, g, b);
            if (g_cfg.progressOn && mode != GRADIENT_FILL_TRIANGLE && n == 2 && abs(copy[1].y - copy[0].y) <= 24) {
                // the small bars of the connection list: take the chosen colors, keep the shading
                float h, sat, l;
                RgbToHsl(r / 255.0f, g / 255.0f, b / 255.0f, &h, &sat, &l);
                if (sat > 0.25f) {
                    COLORREF c = ProgressColor(&t == &copy[0] ? 0.0f : 1.0f, l);
                    r = GetRValue(c);
                    g = GetGValue(c);
                    b = GetBValue(c);
                }
            }
            t.Red = (COLOR16)(r << 8);
            t.Green = (COLOR16)(g << 8);
            t.Blue = (COLOR16)(b << 8);
        }
        return GdiGradientFill_orig(hdc, copy.data(), n, mesh, nm, mode);
    }
    return GdiGradientFill_orig(hdc, v, n, mesh, nm, mode);
}

// anything that draws into a bitmap makes it "dynamic"
typedef BOOL(WINAPI* ExtTextOutW_t)(HDC, int, int, UINT, const RECT*, LPCWSTR, UINT, const INT*);
static ExtTextOutW_t ExtTextOutW_orig;
static BOOL WINAPI ExtTextOutW_hook(HDC hdc, int x, int y, UINT o, const RECT* rc, LPCWSTR s, UINT n, const INT* dx) {
    MarkDirty(hdc);
    return ExtTextOutW_orig(hdc, x, y, o, rc, s, n, dx);
}
typedef BOOL(WINAPI* PatBlt_t)(HDC, int, int, int, int, DWORD);
static PatBlt_t PatBlt_orig;
static BOOL WINAPI PatBlt_hook(HDC hdc, int x, int y, int w, int h, DWORD rop) {
    MarkDirty(hdc);
    return PatBlt_orig(hdc, x, y, w, h, rop);
}
typedef BOOL(WINAPI* Rectangle_t)(HDC, int, int, int, int);
static Rectangle_t Rectangle_orig;
static BOOL WINAPI Rectangle_hook(HDC hdc, int a, int b, int c, int d) {
    MarkDirty(hdc);
    return Rectangle_orig(hdc, a, b, c, d);
}
typedef BOOL(WINAPI* LineTo_t)(HDC, int, int);
static LineTo_t LineTo_orig;
static BOOL WINAPI LineTo_hook(HDC hdc, int x, int y) {
    MarkDirty(hdc);
    return LineTo_orig(hdc, x, y);
}


typedef BOOL(WINAPI* DeleteObject_t)(HGDIOBJ);
static DeleteObject_t DeleteObject_orig;
static BOOL WINAPI DeleteObject_hook(HGDIOBJ obj) {
    if (g_csReady && obj) {
        EnterCriticalSection(&g_cs);
        g_bmps->erase((HBITMAP)obj);
        LeaveCriticalSection(&g_cs);
    }
    return DeleteObject_orig(obj);
}

typedef BOOL(WINAPI* DeleteDC_t)(HDC);
static DeleteDC_t DeleteDC_orig;
static BOOL WINAPI DeleteDC_hook(HDC hdc) {
    if (g_csReady && hdc) {
        EnterCriticalSection(&g_cs);
        g_textColors->erase(hdc);
        g_bkColors->erase(hdc);
        LeaveCriticalSection(&g_cs);
    }
    return DeleteDC_orig(hdc);
}



// ---- native controls: dark scroll bars, dark context menus --------------------------

static bool NeedsDarkTheme(const wchar_t* cls) {
    if (_wcsnicmp(cls, L"TVirtual", 8) == 0) {  // VirtualTreeView family
        return true;
    }
    static const wchar_t* const kClasses[] = {
        L"SysTreeView32", L"SysListView32", L"TVirtualStringTree", L"TVirtualDrawTree", L"ScrollBar",
        L"ListBox", L"ComboLBox", L"Edit", L"RichEdit20W", L"RichEdit20A", L"RICHEDIT50W", L"SysHeader32",
    };
    for (const wchar_t* c : kClasses) {
        if (_wcsicmp(cls, c) == 0) {
            return true;
        }
    }
    return false;
}

// true for windows that belong to a Delphi form of the app (not to a common dialog or the shell)
static bool IsAppWindow(HWND hWnd) {
    return IsAppRoot(hWnd);
}

static void ApplyDarkTheme(HWND hWnd) {
    wchar_t cls[64];
    if (hWnd && IsAppWindow(hWnd) && GetClassNameW(hWnd, cls, ARRAYSIZE(cls)) &&
        (NeedsDarkTheme(cls) || (GetWindowLongW(hWnd, GWL_STYLE) & (WS_HSCROLL | WS_VSCROLL)))) {
        SetWindowTheme(hWnd, L"DarkMode_Explorer", nullptr);
    }
}

typedef HWND(WINAPI* CreateWindowExW_t)(DWORD, LPCWSTR, LPCWSTR, DWORD, int, int, int, int, HWND, HMENU, HINSTANCE, LPVOID);
static CreateWindowExW_t CreateWindowExW_orig;
static HWND WINAPI CreateWindowExW_hook(DWORD ex, LPCWSTR cls, LPCWSTR name, DWORD style, int x, int y, int w, int h,
                                        HWND parent, HMENU menu, HINSTANCE inst, LPVOID param) {
    HWND wnd = CreateWindowExW_orig(ex, cls, name, style, x, y, w, h, parent, menu, inst, param);
    ApplyDarkTheme(wnd);
    return wnd;
}

typedef HWND(WINAPI* CreateWindowExA_t)(DWORD, LPCSTR, LPCSTR, DWORD, int, int, int, int, HWND, HMENU, HINSTANCE, LPVOID);
static CreateWindowExA_t CreateWindowExA_orig;
static HWND WINAPI CreateWindowExA_hook(DWORD ex, LPCSTR cls, LPCSTR name, DWORD style, int x, int y, int w, int h,
                                        HWND parent, HMENU menu, HINSTANCE inst, LPVOID param) {
    HWND wnd = CreateWindowExA_orig(ex, cls, name, style, x, y, w, h, parent, menu, inst, param);
    ApplyDarkTheme(wnd);
    return wnd;
}

static void ResetDarkTheme(HWND hWnd) {
    wchar_t cls[64];
    if (hWnd && IsAppWindow(hWnd) && GetClassNameW(hWnd, cls, ARRAYSIZE(cls)) &&
        (NeedsDarkTheme(cls) || (GetWindowLongW(hWnd, GWL_STYLE) & (WS_HSCROLL | WS_VSCROLL)))) {
        SetWindowTheme(hWnd, nullptr, nullptr);
    }
}

static BOOL CALLBACK ResetDarkThemeEnum(HWND hWnd, LPARAM) {
    ResetDarkTheme(hWnd);
    return TRUE;
}

static BOOL CALLBACK ResetDarkThemeTopLevel(HWND hWnd, LPARAM) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    if (pid == GetCurrentProcessId()) {
        ResetDarkTheme(hWnd);
        EnumChildWindows(hWnd, ResetDarkThemeEnum, 0);
    }
    return TRUE;
}

static BOOL CALLBACK ApplyDarkThemeEnum(HWND hWnd, LPARAM) {
    ApplyDarkTheme(hWnd);
    return TRUE;
}

static BOOL CALLBACK ApplyDarkThemeTopLevel(HWND hWnd, LPARAM) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    if (pid == GetCurrentProcessId()) {
        ApplyDarkTheme(hWnd);
        EnumChildWindows(hWnd, ApplyDarkThemeEnum, 0);
    }
    return TRUE;
}

// Undocumented uxtheme ordinals used by dark mode aware applications.
typedef int(WINAPI* SetPreferredAppMode_t)(int);
typedef void(WINAPI* FlushMenuThemes_t)();
static void SetAppMode(int mode) {
    HMODULE ux = GetModuleHandleW(L"uxtheme.dll");
    if (!ux) {
        return;
    }
    auto setMode = (SetPreferredAppMode_t)GetProcAddress(ux, MAKEINTRESOURCEA(135));
    auto flush = (FlushMenuThemes_t)GetProcAddress(ux, MAKEINTRESOURCEA(136));
    if (setMode) {
        setMode(mode);  // 0 = default, 2 = force dark
    }
    if (flush) {
        flush();
    }
}

// ---- settings loading ---------------------------------------------------------------

// Parses "282828", "#282828" or "#fff". Returns the fallback for anything else.
static COLORREF ParseColor(PCWSTR text, COLORREF fallback) {
    if (!text) {
        return fallback;
    }
    while (*text == L' ' || *text == L'#') {
        text++;
    }
    int digits[6];
    int count = 0;
    while (iswxdigit(*text) && count < 6) {
        wchar_t c = *text++;
        digits[count++] = (c >= L'a') ? c - L'a' + 10 : (c >= L'A') ? c - L'A' + 10 : c - L'0';
    }
    while (*text == L' ') {
        text++;
    }
    if (*text != 0) {
        return fallback;
    }
    if (count == 3) {
        return RGB(digits[0] * 17, digits[1] * 17, digits[2] * 17);
    }
    if (count == 6) {
        return RGB(digits[0] * 16 + digits[1], digits[2] * 16 + digits[3], digits[4] * 16 + digits[5]);
    }
    return fallback;
}

static COLORREF GetColorSetting(PCWSTR name, COLORREF fallback) {
    PCWSTR text = Wh_GetStringSetting(name);
    COLORREF color = ParseColor(text, fallback);
    Wh_FreeStringSetting(text);
    return color;
}

// A missing or zero value means "not set": use the default.
static int GetPercentSetting(PCWSTR name, int fallback, int min, int max) {
    int value = Wh_GetIntSetting(name);
    return (value < min || value > max) ? fallback : value;
}

static int GetAngleSetting() {
    int v = Wh_GetIntSetting(L"gradient.angle");
    return ((v % 360) + 360) % 360;
}

static void LoadSettings() {
    g_cfg.darkOut = GetPercentSetting(L"appearance.backgroundLightness", 11, 1, 40) / 100.0f;
    g_cfg.lightOut = GetPercentSetting(L"appearance.textLightness", 92, 50, 100) / 100.0f;
    PCWSTR tc = Wh_GetStringSetting(L"appearance.textColor");
    COLORREF tcc = tc && *tc ? ParseColor(tc, CLR_INVALID) : CLR_INVALID;
    Wh_FreeStringSetting(tc);
    g_cfg.textTint = tcc != CLR_INVALID;
    if (g_cfg.textTint) {
        float tl;
        RgbToHsl(GetRValue(tcc) / 255.0f, GetGValue(tcc) / 255.0f, GetBValue(tcc) / 255.0f, &g_cfg.textHue, &g_cfg.textSat, &tl);
        g_cfg.lightOut = fmaxf(0.5f, tl);
    }
    PCWSTR lc = Wh_GetStringSetting(L"appearance.listColor");
    COLORREF lcc = lc && *lc ? ParseColor(lc, CLR_INVALID) : CLR_INVALID;
    Wh_FreeStringSetting(lc);
    g_cfg.listTint = lcc != CLR_INVALID;
    if (g_cfg.listTint) {
        float ll;
        RgbToHsl(GetRValue(lcc) / 255.0f, GetGValue(lcc) / 255.0f, GetBValue(lcc) / 255.0f, &g_cfg.listHue, &g_cfg.listSat, &ll);
        g_cfg.listL = ll;
    }
    PCWSTR hc = Wh_GetStringSetting(L"appearance.headerColor");
    COLORREF hcc = hc && *hc ? ParseColor(hc, CLR_INVALID) : CLR_INVALID;
    Wh_FreeStringSetting(hc);
    g_cfg.headerOn = hcc != CLR_INVALID;
    g_cfg.headerColor = hcc;
    g_cfg.satScale = GetPercentSetting(L"appearance.saturation", 85, 1, 200) / 100.0f;
    const bool gradientOn = Wh_GetIntSetting(L"gradient.enabled") != 0;
    g_cfg.angle = GetAngleSetting();
    g_cfg.balance = GetPercentSetting(L"gradient.balance", 50, 0, 100) / 100.0f;

    COLORREF surface = CLR_INVALID;
    PCWSTR custom = Wh_GetStringSetting(L"surface.customColor");
    if (custom && *custom) {
        surface = ParseColor(custom, CLR_INVALID);
    }
    Wh_FreeStringSetting(custom);
    if (gradientOn) {
        g_cfg.gradient = true;
        g_cfg.colorStart = GetColorSetting(L"gradient.colorStart", RGB(0x2F, 0x5A, 0x96));
        g_cfg.colorEnd = GetColorSetting(L"gradient.colorEnd", RGB(0x15, 0x22, 0x38));
    } else {
        g_cfg.gradient = surface != CLR_INVALID;
        g_cfg.colorStart = g_cfg.colorEnd = surface == CLR_INVALID ? RGB(0x2F, 0x5A, 0x96) : surface;
    }

    g_cfg.keepSetupTreeDark = Wh_GetIntSetting(L"appearance.keepSetupTreeDark") != 0;
    PCWSTR prog = Wh_GetStringSetting(L"progress.colorTop");
    COLORREF pc = prog && *prog ? ParseColor(prog, CLR_INVALID) : CLR_INVALID;
    Wh_FreeStringSetting(prog);
    g_cfg.progressOn = pc != CLR_INVALID;
    PCWSTR prog2 = Wh_GetStringSetting(L"progress.colorBottom");
    COLORREF pc2 = prog2 && *prog2 ? ParseColor(prog2, CLR_INVALID) : CLR_INVALID;
    Wh_FreeStringSetting(prog2);
    g_cfg.progressGradient = g_cfg.progressOn && pc2 != CLR_INVALID;
    PCWSTR dir = Wh_GetStringSetting(L"progress.direction");
    g_cfg.progressHorizontal = dir && wcscmp(dir, L"leftright") == 0;
    Wh_FreeStringSetting(dir);
    if (g_cfg.progressOn) {
        float h, sat, l;
        RgbToHsl(GetRValue(pc) / 255.0f, GetGValue(pc) / 255.0f, GetBValue(pc) / 255.0f, &h, &sat, &l);
        g_cfg.progressC1 = pc;
        g_cfg.progressC2 = g_cfg.progressGradient ? pc2 : pc;
        g_cfg.progressL1 = l;
    }
    BuildGradient();
}

// Puts the original pixels of all re-colored skin bitmaps back.
static void RestoreBitmaps() {
    EnterCriticalSection(&g_cs);
    for (auto& kv : *g_bmps) {
        BmpEntry& e = kv.second;
        if (e.state == 1 && e.bits && !e.backup.empty()) {
            DIBSECTION ds;
            if (GetObjectW(kv.first, sizeof(ds), &ds) == sizeof(ds) && ds.dsBm.bmBits == e.bits) {
                memcpy(e.bits, e.backup.data(), e.backup.size());
            }
        }
    }
    // only the re-colored ones are forgotten (they are re-colored again on their next use); the
    // "drawn into" and "skip" entries stay, otherwise composed buffers would be re-colored
    for (auto it = g_bmps->begin(); it != g_bmps->end();) {
        it = it->second.state == 1 ? g_bmps->erase(it) : std::next(it);
    }
    LeaveCriticalSection(&g_cs);
}

// ---- lifecycle ---------------------------------------------------------------------

static BOOL CALLBACK RepaintWindow(HWND hWnd, LPARAM) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    if (pid == GetCurrentProcessId() && IsWindowVisible(hWnd)) {
        RedrawWindow(hWnd, nullptr, nullptr, RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN | RDW_FRAME);
    }
    return TRUE;
}

static HANDLE g_lateRepaint;
static DWORD WINAPI LateRepaint(LPVOID) {
    Sleep(250);
    EnumWindows(RepaintWindow, 0);
    return 0;
}

// Some controls repaint from cached buffers: paint once more a moment later.
static void StartLateRepaint() {
    if (g_lateRepaint) {
        if (WaitForSingleObject(g_lateRepaint, 0) != WAIT_OBJECT_0) {
            return;  // the previous one is still pending
        }
        CloseHandle(g_lateRepaint);
    }
    g_lateRepaint = CreateThread(nullptr, 0, LateRepaint, nullptr, 0, nullptr);
}

#define HOOK(module, name)                                                                              \
    do {                                                                                                \
        void* target = (void*)GetProcAddress(module, #name);                                            \
        if (target) {                                                                                   \
            Wh_SetFunctionHook(target, (void*)name##_hook, (void**)&name##_orig);                       \
        }                                                                                               \
    } while (0)

BOOL Wh_ModInit() {
    InitializeCriticalSection(&g_cs);
    InitializeCriticalSection(&g_tempCs);
    g_bmps = new std::unordered_map<HBITMAP, BmpEntry>();
    g_textColors = new std::unordered_map<HDC, COLORREF>();
    g_bkColors = new std::unordered_map<HDC, COLORREF>();
    g_csReady = true;
    g_exeModule = GetModuleHandleW(nullptr);
    g_callerKind = new std::unordered_map<HMODULE, int>();
    LoadSettings();

    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    HMODULE gdi32 = GetModuleHandleW(L"gdi32.dll");
    if (!user32 || !gdi32) {
        return FALSE;
    }

    HOOK(user32, GetSysColor);
    HOOK(user32, GetSysColorBrush);
    HOOK(user32, FillRect);
    HOOK(user32, EndPaint);
    HOOK(user32, ReleaseDC);

    HOOK(gdi32, CreateSolidBrush);
    HOOK(gdi32, CreateBrushIndirect);
    HOOK(gdi32, CreatePen);
    HOOK(gdi32, CreatePenIndirect);
    HOOK(gdi32, ExtCreatePen);
    HOOK(gdi32, SetTextColor);
    HOOK(gdi32, GetTextColor);
    HOOK(gdi32, SetBkColor);
    HOOK(gdi32, GetBkColor);
    HOOK(gdi32, SetDCBrushColor);
    HOOK(gdi32, SetDCPenColor);
    HOOK(gdi32, SetPixel);
    HOOK(gdi32, SetPixelV);
    HOOK(gdi32, BitBlt);
    HOOK(gdi32, StretchBlt);
    HOOK(gdi32, MaskBlt);
    HOOK(gdi32, GdiTransparentBlt);
    HOOK(gdi32, GdiAlphaBlend);
    HOOK(gdi32, GdiGradientFill);
    HOOK(gdi32, StretchDIBits);
    HOOK(gdi32, SetDIBits);
    HOOK(gdi32, CreateDIBitmap);
    HOOK(gdi32, SetDIBitsToDevice);
    HOOK(gdi32, ExtTextOutW);
    HOOK(gdi32, PatBlt);
    HOOK(gdi32, Rectangle);
    HOOK(gdi32, LineTo);
    HOOK(gdi32, DeleteObject);
    HOOK(gdi32, DeleteDC);
    HOOK(user32, CreateWindowExW);
    HOOK(user32, CreateWindowExA);

    SetAppMode(2);
    return TRUE;
}

void Wh_ModAfterInit() {
    EnumWindows(ApplyDarkThemeTopLevel, 0);
    EnumWindows(RepaintWindow, 0);
    StartLateRepaint();
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    RestoreBitmaps();  // the skin bitmaps are re-colored again with the new values on their next use
    for (int i = 0; i < 32; i++) {
        g_sysBrushes[i] = nullptr;  // rebuilt on demand (the old brushes stay alive: they may still be selected)
    }
    EnumWindows(RepaintWindow, 0);
    StartLateRepaint();
}

void Wh_ModUninit() {
    if (g_lateRepaint) {
        // the thread only sleeps and repaints; it must be gone before the DLL is unloaded
        WaitForSingleObject(g_lateRepaint, INFINITE);
        CloseHandle(g_lateRepaint);
        g_lateRepaint = nullptr;
    }
    SetAppMode(0);
    EnumWindows(ResetDarkThemeTopLevel, 0);
    RestoreBitmaps();
    EnumWindows(RepaintWindow, 0);
    EnterCriticalSection(&g_tempCs);
    FreeTempBitmap();
    LeaveCriticalSection(&g_tempCs);
}
