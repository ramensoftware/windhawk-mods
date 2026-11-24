// ==WindhawkMod==
// @id              network-speed-on-taskbar
// @name            Network Speed On Taskbar
// @description     A modern, good-looking network speed widget (download and upload) on the taskbar, with hover usage details (today, month, year), 4 layouts and 8 color palettes
// @version         1.0.0
// @author          Pixeliota
// @github          https://github.com/Pixeliota
// @twitter         https://twitter.com/Pixeliota
// @homepage        https://pixeliota.gumroad.com/
// @include         explorer.exe
// @license         MIT
// @compilerOptions -lgdi32 -luser32 -ladvapi32 -lole32 -loleaut32 -luuid
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Network Speed On Taskbar

By **Pixeliota** - Instagram: [@pixeliota](https://instagram.com/pixeliota) -
X: [@Pixeliota](https://x.com/Pixeliota) - [Gumroad](https://pixeliota.gumroad.com/) -
[GitHub](https://github.com/Pixeliota)

![Network Speed On Taskbar](https://i.imgur.com/yIRzls2.png)

**Light taskbar**

![Light taskbar](https://i.imgur.com/b0CxHVm.png)

**Dark taskbar**

![Dark taskbar](https://i.imgur.com/rlVL4rD.png)

**Aurora colors (teal and violet)**

![Aurora colors](https://i.imgur.com/lswU6tP.png)

**Pulse layout - a live graph plus two lines**

![Pulse layout](https://i.imgur.com/2p9j2zB.png)

**Stack layout - two lines**

![Stack layout](https://i.imgur.com/rqILOuR.png)

**Capsule layout - one line**

![Capsule layout](https://i.imgur.com/1pMJfiT.png)

A small, modern network speed widget that sits on the taskbar next to the notification
area. It shows the current download and upload speed with colored arrow chips, and some
layouts add a live graph or meters.

**Layouts**
* **Capsule** - one line: download and upload side by side
* **Stack** - two lines
* **Pulse** - a live graph plus two lines
* **Meter** - two animated bars plus two lines

**Color palettes**
* **Aurora** (teal and violet), **Sunset** (orange and rose), **Ocean** (sky blue and indigo),
  **Neon** (green and magenta), **Candy** (pink and sky blue), **Lime** (lime and cyan),
  **Mono** (matches the text color), **Accent** (your Windows accent color)

**Arrow styles**
* **Solid** - filled circle with a contrasting arrow (easiest to see, the default)
* **Soft** - tinted circle with a colored arrow
* **Bold** - a big bold arrow without a circle
* **Triangle** - a filled triangle

**Font**
The widget looks best with the free **Inter** font (https://rsms.me/inter/ or
https://fonts.google.com/specimen/Inter). Install it once in Windows, then restart Explorer
or toggle this mod. If Inter is not installed, Segoe UI is used instead. You can type the
name of any other installed font in the "Font" setting.

**Highlights**
* Follows the light or dark taskbar theme automatically, and the display scaling (DPI).
* Frosted-glass background with a soft border. You can change its opacity, turn off the
  color tint, or make the corners more or less round.
* Shows speeds in bytes (KB/s, MB/s) or bits (Kbps, Mbps).
* The numbers dim when the connection is idle.
* **Hover for usage details.** Point at the widget to see a card with your data usage for
  today, this month and this year, a 7-day chart, the total for this session and your
  fastest speed today.
* The widget never blocks clicks: the taskbar underneath works as usual.
* It hides together with the taskbar (auto-hide, full screen).

**Which network is measured?**
By default the adapter that carries your internet traffic (the one with the default
route). This avoids counting the same traffic twice on PCs with virtual adapters.
Alternatively sum all physical adapters, or type part of an adapter name (for example
"Wi-Fi" or "Ethernet") to measure only that adapter.

**About the usage card**
* Usage is counted while Explorer is running (not while the PC is off) on the network
  you chose in the settings, and it is kept on this PC between restarts.
* It keeps up to about 2 years of daily totals. To clear the history, change the number in
  "Change this number to reset the saved usage history".
* You can turn the card off, or change how long you must hover before it appears.

**Never covers your icons**
The widget asks Windows (through UI Automation) where the last taskbar button is. If there
is not enough room next to the notification area, it shrinks (your chosen layout stays),
and hides itself if it still does not fit. It comes back when space frees up. With the
"Shrink, then switch to the narrower Stack layout" option it may also change to Stack.
This works for the default "Next to the notification area" position, and can be turned off
in the settings.

**Notes**
* Works on horizontal taskbars (bottom or top) and only on the main taskbar. The widget is
  hidden on a vertical taskbar.
* The position is next to the notification area by default. Use "Position" and the
  offsets if you want it somewhere else.
* Taskbar Clock Customization can also show the network speed, as text inside the clock.
  This mod is a separate widget with its own design.

**About the author**

Made by [Pixeliota](https://github.com/Pixeliota) -
[Instagram](https://instagram.com/pixeliota) -
[X (Twitter)](https://x.com/Pixeliota) -
[Gumroad store](https://pixeliota.gumroad.com/)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- skin: capsule
  $name: Layout
  $options:
  - capsule: Capsule (one line)
  - stack: Stack (two lines)
  - pulse: Pulse (live graph and two lines)
  - meter: Meter (two bars and two lines)
- palette: aurora
  $name: Colors
  $options:
  - aurora: Aurora (teal and violet)
  - sunset: Sunset (orange and rose)
  - ocean: Ocean (sky blue and indigo)
  - neon: Neon (green and magenta)
  - candy: Candy (pink and sky blue)
  - lime: Lime (lime and cyan)
  - mono: Mono (matches the text)
  - accent: Accent (Windows accent color)
- fontName: Inter
  $name: Font (Segoe UI is used if this font is not installed)
- arrowStyle: solid
  $name: Arrow style
  $options:
  - solid: Solid (filled circle, easiest to see)
  - soft: Soft (tinted circle)
  - bold: Bold (big arrow, no circle)
  - triangle: Triangle (filled triangle)
- themeMode: auto
  $name: Light or dark
  $options:
  - auto: Follow the taskbar
  - light: Light
  - dark: Dark
- unitMode: bytes
  $name: Units
  $options:
  - bytes: Bytes per second (KB/s, MB/s)
  - bits: Bits per second (Kbps, Mbps)
- updateMs: 1000
  $name: Update interval in milliseconds (250 to 5000)
- placement: tray
  $name: Position
  $options:
  - tray: Next to the notification area
  - left: Left edge of the taskbar
  - center: Center of the taskbar
  - right: Right edge of the taskbar
- offsetX: 0
  $name: Horizontal offset in pixels (positive moves right)
- offsetY: 0
  $name: Vertical offset in pixels (positive moves down)
- sizePercent: 100
  $name: Size in percent (60 to 200)
- cornerRadius: 10
  $name: Corner radius in pixels at 100% scaling (0 = square)
- backgroundOpacity: 100
  $name: Background opacity in percent (0 = no background)
- tintBackground: true
  $name: Tint the background with the palette colors
- dimWhenIdle: true
  $name: Dim the numbers when the connection is idle
- autoFit: shrink
  $name: When there is not enough room next to the taskbar icons
  $options:
  - shrink: Shrink the widget (hide it if it still does not fit)
  - smart: Shrink, then switch to the narrower Stack layout
  - off: Do nothing (the widget may cover icons)
- showTooltip: true
  $name: Show the usage card when hovering the widget
- tooltipDelayMs: 300
  $name: Hover delay in milliseconds (0 to 2000)
- resetUsageToken: 0
  $name: Change this number to reset the saved usage history
- adapterMode: auto
  $name: Which network to measure
  $options:
  - auto: The adapter that carries internet traffic
  - all: All physical adapters added together
- adapterFilter: ""
  $name: Only measure adapters whose name contains this text (optional, overrides the choice above)
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <uiautomation.h>

#include <math.h>
#include <stdint.h>
#include <string.h>
#include <wchar.h>

#include <mutex>
#include <unordered_map>
#include <utility>
#include <vector>

// ---- RENDERER BEGIN ----------------------------------------------------------------
// Pure C++ software renderer: draws the widget into a premultiplied BGRA pixel buffer.
// It has no Windows dependencies, except for the text functions declared below.

struct RGBA {
    float r, g, b, a;  // straight (non-premultiplied) color, 0..1
};

struct Canvas {
    int w = 0, h = 0;
    std::vector<uint32_t> px;  // premultiplied 0xAARRGGBB
    void Init(int W, int H) {
        w = W;
        h = H;
        px.assign((size_t)W * (size_t)H, 0);
    }
};

enum { FN_NUM = 0, FN_UNIT = 1 };

// Provided by the host (GDI text in the mod, a preview font in the test harness).
static float TextW(int fn, const wchar_t* s);
static float TextH(int fn);
static float TextAscent(int fn);
static void TextDraw(Canvas& c, int fn, float x, float y, const wchar_t* s, RGBA col);

static inline float Clamp01(float v) { return v < 0.f ? 0.f : (v > 1.f ? 1.f : v); }
static inline float FMin(float a, float b) { return a < b ? a : b; }
static inline float FMax(float a, float b) { return a > b ? a : b; }
static inline int IMax(int a, int b) { return a > b ? a : b; }
static inline int IMin(int a, int b) { return a < b ? a : b; }
static inline int ToByte(float v) {
    int i = (int)(v * 255.f + 0.5f);
    return i < 0 ? 0 : (i > 255 ? 255 : i);
}

static RGBA Rgb(int r, int g, int b, float a = 1.f) {
    return RGBA{r / 255.f, g / 255.f, b / 255.f, a};
}
static RGBA WithAlpha(RGBA c, float a) {
    c.a = a;
    return c;
}
static RGBA MixC(RGBA a, RGBA b, float t) {
    return RGBA{a.r + (b.r - a.r) * t, a.g + (b.g - a.g) * t, a.b + (b.b - a.b) * t,
                a.a + (b.a - a.a) * t};
}
static RGBA Lighten(RGBA c, float t) { return MixC(c, RGBA{1.f, 1.f, 1.f, c.a}, t); }
static RGBA Darken(RGBA c, float t) { return MixC(c, RGBA{0.f, 0.f, 0.f, c.a}, t); }

static inline void Blend(Canvas& c, int x, int y, const RGBA& col, float cov) {
    if (x < 0 || y < 0 || x >= c.w || y >= c.h) return;
    const float a = col.a * cov;
    if (a <= 0.f) return;
    uint32_t& d = c.px[(size_t)y * c.w + x];
    const float da = ((d >> 24) & 255) / 255.f;
    const float dr = ((d >> 16) & 255) / 255.f;
    const float dg = ((d >> 8) & 255) / 255.f;
    const float db = (d & 255) / 255.f;
    const float inv = 1.f - a;
    const float oa = a + da * inv;
    const float orr = col.r * a + dr * inv;
    const float og = col.g * a + dg * inv;
    const float ob = col.b * a + db * inv;
    d = ((uint32_t)ToByte(oa) << 24) | ((uint32_t)ToByte(orr) << 16) |
        ((uint32_t)ToByte(og) << 8) | (uint32_t)ToByte(ob);
}

// Signed distance to a rounded box (negative inside).
static float SdRoundRect(float px, float py, float cx, float cy, float hw, float hh, float r) {
    const float qx = fabsf(px - cx) - (hw - r);
    const float qy = fabsf(py - cy) - (hh - r);
    const float ox = FMax(qx, 0.f), oy = FMax(qy, 0.f);
    return sqrtf(ox * ox + oy * oy) + FMin(FMax(qx, qy), 0.f) - r;
}

static float RoundRectCov(float x, float y, float w, float h, float r, float px, float py) {
    r = FMin(r, FMin(w, h) * 0.5f);
    return Clamp01(0.5f - SdRoundRect(px, py, x + w * 0.5f, y + h * 0.5f, w * 0.5f, h * 0.5f, r));
}

static void FillRoundRectGrad(Canvas& c, float x, float y, float w, float h, float r, RGBA c0,
                              RGBA c1, bool vertical) {
    if (w <= 0.f || h <= 0.f) return;
    r = FMin(r, FMin(w, h) * 0.5f);
    const int x0 = (int)floorf(x) - 1, x1 = (int)ceilf(x + w) + 1;
    const int y0 = (int)floorf(y) - 1, y1 = (int)ceilf(y + h) + 1;
    for (int py = y0; py < y1; py++) {
        for (int px = x0; px < x1; px++) {
            const float cov = RoundRectCov(x, y, w, h, r, px + 0.5f, py + 0.5f);
            if (cov <= 0.f) continue;
            const float t = vertical ? Clamp01((py + 0.5f - y) / h) : Clamp01((px + 0.5f - x) / w);
            Blend(c, px, py, MixC(c0, c1, t), cov);
        }
    }
}

static void FillRoundRect(Canvas& c, float x, float y, float w, float h, float r, RGBA col) {
    FillRoundRectGrad(c, x, y, w, h, r, col, col, false);
}

// Border of the given thickness, drawn inside the rounded rectangle.
static void StrokeRoundRect(Canvas& c, float x, float y, float w, float h, float r, float t,
                            RGBA col) {
    if (w <= 0.f || h <= 0.f) return;
    r = FMin(r, FMin(w, h) * 0.5f);
    const int x0 = (int)floorf(x) - 1, x1 = (int)ceilf(x + w) + 1;
    const int y0 = (int)floorf(y) - 1, y1 = (int)ceilf(y + h) + 1;
    for (int py = y0; py < y1; py++) {
        for (int px = x0; px < x1; px++) {
            const float d = SdRoundRect(px + 0.5f, py + 0.5f, x + w * 0.5f, y + h * 0.5f,
                                        w * 0.5f, h * 0.5f, r);
            const float cov = Clamp01(0.5f - d) * Clamp01(0.5f + d + t);
            if (cov > 0.f) Blend(c, px, py, col, cov);
        }
    }
}

static void FillCircle(Canvas& c, float cx, float cy, float rad, RGBA col) {
    FillRoundRect(c, cx - rad, cy - rad, rad * 2.f, rad * 2.f, rad, col);
}

// Line segment with round caps.
static void StrokeSeg(Canvas& c, float ax, float ay, float bx, float by, float width, RGBA col) {
    const float pad = width * 0.5f + 1.5f;
    const int x0 = (int)floorf(FMin(ax, bx) - pad), x1 = (int)ceilf(FMax(ax, bx) + pad);
    const int y0 = (int)floorf(FMin(ay, by) - pad), y1 = (int)ceilf(FMax(ay, by) + pad);
    const float dx = bx - ax, dy = by - ay;
    const float len2 = dx * dx + dy * dy;
    for (int py = y0; py < y1; py++) {
        for (int px = x0; px < x1; px++) {
            const float qx = px + 0.5f - ax, qy = py + 0.5f - ay;
            float t = len2 > 0.f ? (qx * dx + qy * dy) / len2 : 0.f;
            t = Clamp01(t);
            const float ex = qx - dx * t, ey = qy - dy * t;
            const float dist = sqrtf(ex * ex + ey * ey);
            const float cov = Clamp01(width * 0.5f + 0.5f - dist);
            if (cov > 0.f) Blend(c, px, py, col, cov);
        }
    }
}

// Smooth line graph with an optional gradient fill below the line, clipped to a rounded rect.
static void DrawSeries(Canvas& c, float x, float y, float w, float h, float rad, const float* vals,
                       int n, float vmax, RGBA col, float lineW, float fillA) {
    if (n < 2 || w < 2.f || h < 2.f || vmax <= 0.f) return;
    const int x0 = (int)floorf(x), x1 = (int)ceilf(x + w);
    const int cols = x1 - x0;
    if (cols <= 0) return;
    const float top = y + lineW * 0.5f + 1.f;
    const float bottom = y + h - lineW * 0.5f - 1.f;
    std::vector<float> ys((size_t)cols + 2);
    for (int i = -1; i <= cols; i++) {
        float fx = (x0 + i + 0.5f - x) / w * (float)(n - 1);
        if (fx < 0.f) fx = 0.f;
        if (fx > (float)(n - 1)) fx = (float)(n - 1);
        int k = (int)floorf(fx);
        if (k > n - 2) k = n - 2;
        const float t = fx - (float)k;
        const float p0 = vals[IMax(k - 1, 0)], p1 = vals[k], p2 = vals[k + 1],
                    p3 = vals[IMin(k + 2, n - 1)];
        const float t2 = t * t, t3 = t2 * t;
        float v = 0.5f * ((2.f * p1) + (-p0 + p2) * t + (2.f * p0 - 5.f * p1 + 4.f * p2 - p3) * t2 +
                          (-p0 + 3.f * p1 - 3.f * p2 + p3) * t3);
        v = FMax(0.f, FMin(v, vmax));
        ys[(size_t)(i + 1)] = bottom - (v / vmax) * (bottom - top);
    }
    const int y0 = (int)floorf(y) - 1, y1 = (int)ceilf(y + h) + 1;
    for (int i = 0; i < cols; i++) {
        const int px = x0 + i;
        const float yc = ys[(size_t)(i + 1)];
        const float slope = (ys[(size_t)(i + 2)] - ys[(size_t)i]) * 0.5f;
        const float norm = 1.f / sqrtf(1.f + slope * slope);
        for (int py = y0; py < y1; py++) {
            const float cy = py + 0.5f;
            const float clip = RoundRectCov(x, y, w, h, rad, px + 0.5f, cy);
            if (clip <= 0.f) continue;
            if (fillA > 0.f) {
                const float cov = Clamp01(cy - yc + 0.5f);
                if (cov > 0.f) {
                    const float fade = Clamp01(1.f - (cy - yc) / (h * 0.85f));
                    Blend(c, px, py, WithAlpha(col, fillA * fade), cov * clip);
                }
            }
            const float dist = fabsf(cy - yc) * norm;
            const float lc = Clamp01(lineW * 0.5f + 0.5f - dist);
            if (lc > 0.f) Blend(c, px, py, col, lc * clip);
        }
    }
}

// ---- Widget model --------------------------------------------------------------------

enum Skin { SK_CAPSULE = 0, SK_STACK, SK_PULSE, SK_METER };

struct Palette {
    RGBA down, up;
};

struct View {
    int skin = SK_CAPSULE;
    int arrowStyle = 1;  // ArrowStyle
    bool dark = true;
    bool bits = false;
    bool dim = true;     // dim the numbers while the link is idle
    bool tint = true;    // tint the background with the palette colors
    float scale = 1.f;   // device pixels per design pixel
    float maxH = 48.f;   // available height in device pixels
    float radius = 10.f; // corner radius, design pixels
    float bgOpacity = 1.f;
    Palette pal;
    RGBA text, textDim;
    double down = 0, up = 0;  // bytes per second
    const float* histDown = nullptr;
    const float* histUp = nullptr;
    int histN = 0;
    float scaleMax = 1.f;  // bytes per second that fills the graph / meter
};

struct Layout {
    float w = 0, h = 0;
    float pad = 0, chip = 0, gap = 0, numW = 0, unitW = 0, graphW = 0, barW = 0, mid = 0;
};

static void FontSizes(const View& v, float* num, float* unit) {
    float n = 14.f, u = 10.5f;
    if (v.skin == SK_CAPSULE) {
        n = 15.f;
        u = 11.f;
    } else if (v.skin == SK_METER) {
        n = 13.5f;
        u = 10.f;
    }
    // Shrink the text on small taskbars so two rows still fit.
    const float need = (v.skin == SK_CAPSULE ? 29.f : 39.f) * v.scale;
    const float k = v.maxH < need ? FMax(0.6f, v.maxH / need) : 1.f;
    *num = n * v.scale * k;
    *unit = u * v.scale * k;
}

static void FmtFixed(wchar_t* out, int cap, double v, int dec) {
    long long mul = 1;
    for (int i = 0; i < dec; i++) mul *= 10;
    if (v < 0) v = 0;
    long long scaled = (long long)(v * (double)mul + 0.5);
    long long ip = scaled / mul, fp = scaled % mul;
    wchar_t tmp[48];
    int n = 0;
    if (ip == 0) tmp[n++] = L'0';
    while (ip > 0 && n < 20) {
        tmp[n++] = (wchar_t)(L'0' + (int)(ip % 10));
        ip /= 10;
    }
    int o = 0;
    while (n > 0 && o < cap - 1) out[o++] = tmp[--n];
    if (dec > 0 && o < cap - 2 - dec) {
        out[o++] = L'.';
        for (int i = dec - 1; i >= 0; i--) {
            long long p = 1;
            for (int j = 0; j < i; j++) p *= 10;
            out[o++] = (wchar_t)(L'0' + (int)((fp / p) % 10));
        }
    }
    out[o] = 0;
}

static void FormatSpeed(double bytesPerSec, bool bits, wchar_t* num, int numCap, const wchar_t** unit) {
    static const wchar_t* kBytes[] = {L"B/s", L"KB/s", L"MB/s", L"GB/s", L"TB/s"};
    static const wchar_t* kBits[] = {L"bps", L"Kbps", L"Mbps", L"Gbps", L"Tbps"};
    double v = bits ? bytesPerSec * 8.0 : bytesPerSec;
    const double base = bits ? 1000.0 : 1024.0;
    int i = 0;
    while (v >= 1000.0 && i < 4) {  // at most 3 digits, so the number column can stay narrow
        v /= base;
        i++;
    }
    int dec = 0;
    if (i > 0) dec = v < 10.0 ? 2 : (v < 100.0 ? 1 : 0);
    FmtFixed(num, numCap, v, dec);
    *unit = bits ? kBits[i] : kBytes[i];
}

static void ComputeLayout(const View& v, Layout& L) {
    const float s = v.scale;
    L.numW = FMax(TextW(FN_NUM, L"888"), FMax(TextW(FN_NUM, L"88.8"), TextW(FN_NUM, L"8.88")));
    const wchar_t* us[3] = {v.bits ? L"Kbps" : L"KB/s", v.bits ? L"Mbps" : L"MB/s",
                            v.bits ? L"Gbps" : L"GB/s"};
    L.unitW = FMax(TextW(FN_UNIT, us[0]), FMax(TextW(FN_UNIT, us[1]), TextW(FN_UNIT, us[2])));
    L.gap = 6.f * s;
    L.mid = 3.f * s;  // between number and unit
    switch (v.skin) {
        case SK_STACK:
            L.h = FMin(40.f * s, v.maxH);
            L.chip = L.h * 0.37f;
            L.pad = 4.f * s;
            L.w = L.pad + L.chip + L.gap + L.numW + L.mid + L.unitW + L.pad;
            break;
        case SK_PULSE:
            L.h = FMin(42.f * s, v.maxH);
            L.chip = L.h * 0.34f;
            L.pad = FMax(4.f * s, L.h * 0.12f);
            L.graphW = 64.f * s;
            L.w = L.pad + L.graphW + 9.f * s + L.chip + L.gap + L.numW + L.mid + L.unitW + L.pad + 4.f * s;
            break;
        case SK_METER:
            L.h = FMin(40.f * s, v.maxH);
            L.chip = L.h * 0.34f;
            L.pad = 8.f * s;
            L.barW = 54.f * s;
            L.w = L.pad + L.chip + L.gap + L.barW + 8.f * s + L.numW + L.mid + L.unitW + L.pad;
            break;
        default:  // capsule
            L.h = FMin(30.f * s, v.maxH);
            L.chip = L.h * 0.54f;
            L.pad = L.h * 0.28f;
            L.gap = 5.f * s;
            L.mid = 2.5f * s;
            L.w = L.pad + 2.f * (L.chip + L.gap + L.numW + L.mid + L.unitW) + 10.f * s + L.pad;
            break;
    }
    L.w = ceilf(L.w);
    L.h = ceilf(L.h);
}

static void DrawBackground(Canvas& c, const View& v, const Layout& L) {
    const float op = v.bgOpacity;
    if (op <= 0.f) return;
    const float r = FMin(v.radius * v.scale, L.h * 0.5f);
    RGBA top, bot, border;
    if (v.dark) {
        top = Rgb(255, 255, 255, 0.115f);
        bot = Rgb(255, 255, 255, 0.055f);
        border = Rgb(255, 255, 255, 0.11f);
    } else {
        top = Rgb(255, 255, 255, 0.70f);
        bot = Rgb(255, 255, 255, 0.42f);
        border = Rgb(0, 0, 0, 0.09f);
    }
    top.a *= op;
    bot.a *= op;
    border.a *= op;
    FillRoundRectGrad(c, 0, 0, L.w, L.h, r, top, bot, true);
    if (v.tint) {
        const float ta = (v.dark ? 0.16f : 0.14f) * op;
        FillRoundRectGrad(c, 0, 0, L.w, L.h, r, WithAlpha(v.pal.down, ta), WithAlpha(v.pal.up, ta),
                          false);
    }
    StrokeRoundRect(c, 0, 0, L.w, L.h, r, FMax(1.f, floorf(v.scale + 0.25f)), border);
}

enum ArrowStyle { ARW_SOFT = 0, ARW_SOLID, ARW_BOLD, ARW_TRIANGLE };
static int g_arrowStyle = ARW_SOLID;

static float Luma(RGBA c) { return 0.299f * c.r + 0.587f * c.g + 0.114f * c.b; }

static void FillTriangleAA(Canvas& c, float ax, float ay, float bx, float by, float cx, float cy,
                           RGBA col) {
    const int x0 = (int)floorf(FMin(ax, FMin(bx, cx))) - 1, x1 = (int)ceilf(FMax(ax, FMax(bx, cx))) + 1;
    const int y0 = (int)floorf(FMin(ay, FMin(by, cy))) - 1, y1 = (int)ceilf(FMax(ay, FMax(by, cy))) + 1;
    auto edge = [](float px, float py, float x1_, float y1_, float x2_, float y2_) {
        return (px - x2_) * (y1_ - y2_) - (x1_ - x2_) * (py - y2_);
    };
    for (int py = y0; py < y1; py++) {
        for (int px = x0; px < x1; px++) {
            int hit = 0;
            for (int sy = 0; sy < 4; sy++) {
                for (int sx = 0; sx < 4; sx++) {
                    const float fx = px + (sx + 0.5f) / 4.f, fy = py + (sy + 0.5f) / 4.f;
                    const float d1 = edge(fx, fy, ax, ay, bx, by);
                    const float d2 = edge(fx, fy, bx, by, cx, cy);
                    const float d3 = edge(fx, fy, cx, cy, ax, ay);
                    const bool neg = d1 < 0 || d2 < 0 || d3 < 0;
                    const bool pos = d1 > 0 || d2 > 0 || d3 > 0;
                    if (!(neg && pos)) hit++;
                }
            }
            if (hit > 0) Blend(c, px, py, col, (float)hit / 16.f);
        }
    }
}

static void DrawArrowStrokes(Canvas& c, float cx, float cy, float u, float dir, float sw, RGBA a) {
    StrokeSeg(c, cx, cy - dir * 3.6f * u, cx, cy + dir * 3.3f * u, sw, a);
    StrokeSeg(c, cx - 3.0f * u, cy + dir * 0.4f * u, cx, cy + dir * 3.5f * u, sw, a);
    StrokeSeg(c, cx + 3.0f * u, cy + dir * 0.4f * u, cx, cy + dir * 3.5f * u, sw, a);
}

static void DrawChip(Canvas& c, float cx, float cy, float d, bool up, RGBA col, float alpha) {
    const float u = d / 13.f;
    const float dir = up ? -1.f : 1.f;
    switch (g_arrowStyle) {
        case ARW_SOLID: {
            FillCircle(c, cx, cy, d * 0.5f, WithAlpha(col, alpha));
            RGBA ink = Luma(col) > 0.62f ? Rgb(20, 20, 26) : Rgb(255, 255, 255);
            DrawArrowStrokes(c, cx, cy, u, dir, FMax(1.3f, 1.8f * u), WithAlpha(ink, alpha));
            break;
        }
        case ARW_BOLD:
            DrawArrowStrokes(c, cx, cy, u * 1.3f, dir, FMax(1.6f, 2.5f * u), WithAlpha(col, alpha));
            break;
        case ARW_TRIANGLE: {
            const float hw = 3.9f * u, hh = 3.3f * u;
            const RGBA a = WithAlpha(col, alpha);
            if (up) FillTriangleAA(c, cx - hw, cy + hh * 0.75f, cx + hw, cy + hh * 0.75f, cx, cy - hh * 1.1f, a);
            else FillTriangleAA(c, cx - hw, cy - hh * 0.75f, cx + hw, cy - hh * 0.75f, cx, cy + hh * 1.1f, a);
            break;
        }
        default:  // soft
            FillCircle(c, cx, cy, d * 0.5f, WithAlpha(col, 0.30f * alpha));
            DrawArrowStrokes(c, cx, cy, u, dir, FMax(1.3f, 1.9f * u), WithAlpha(col, alpha));
            break;
    }
}

// Number (right-aligned in its column) followed by the unit; returns nothing, x is the column start.
static void DrawSpeed(Canvas& c, const View& v, const Layout& L, float x, float cy, double bps,
                      bool idle) {
    wchar_t num[48];
    const wchar_t* unit = L"";
    FormatSpeed(bps, v.bits, num, 48, &unit);
    const float nh = TextH(FN_NUM);
    const float ty = cy - nh * 0.5f + 0.5f * v.scale;
    const float nx = x + L.numW - TextW(FN_NUM, num);
    RGBA tc = (idle && v.dim) ? v.textDim : v.text;
    TextDraw(c, FN_NUM, nx, ty, num, tc);
    const float uy = ty + (TextAscent(FN_NUM) - TextAscent(FN_UNIT));
    RGBA uc = v.textDim;
    if (idle && v.dim) uc.a *= 0.8f;
    TextDraw(c, FN_UNIT, x + L.numW + L.mid, uy, unit, uc);
}

static bool IsIdle(double bps) { return bps < 1024.0; }

static void RenderWidget(Canvas& c, const View& v, const Layout& L) {
    g_arrowStyle = v.arrowStyle;
    c.Init((int)L.w, (int)L.h);
    DrawBackground(c, v, L);
    const float s = v.scale;
    const bool idleD = IsIdle(v.down), idleU = IsIdle(v.up);
    const float aD = (idleD && v.dim) ? 0.8f : 1.f;
    const float aU = (idleU && v.dim) ? 0.8f : 1.f;

    if (v.skin == SK_CAPSULE) {
        const float cy = L.h * 0.5f;
        float x = L.pad;
        DrawChip(c, x + L.chip * 0.5f, cy, L.chip, false, v.pal.down, aD);
        x += L.chip + L.gap;
        DrawSpeed(c, v, L, x, cy, v.down, idleD);
        x += L.numW + L.mid + L.unitW + 10.f * s;
        DrawChip(c, x + L.chip * 0.5f, cy, L.chip, true, v.pal.up, aU);
        x += L.chip + L.gap;
        DrawSpeed(c, v, L, x, cy, v.up, idleU);
    } else if (v.skin == SK_STACK) {
        const float r1 = L.h * 0.27f, r2 = L.h * 0.73f;
        float x = L.pad;
        DrawChip(c, x + L.chip * 0.5f, r1, L.chip, false, v.pal.down, aD);
        DrawChip(c, x + L.chip * 0.5f, r2, L.chip, true, v.pal.up, aU);
        x += L.chip + L.gap;
        DrawSpeed(c, v, L, x, r1, v.down, idleD);
        DrawSpeed(c, v, L, x, r2, v.up, idleU);
    } else if (v.skin == SK_PULSE) {
        const float gx = L.pad, gy = L.pad, gw = L.graphW, gh = L.h - 2.f * L.pad;
        const float gr = FMin(6.f * s, gh * 0.5f);
        FillRoundRectGrad(c, gx, gy, gw, gh, gr, WithAlpha(v.pal.down, 0.20f),
                          WithAlpha(v.pal.down, 0.06f), true);
        if (v.histN > 1) {
            DrawSeries(c, gx, gy, gw, gh, gr, v.histUp, v.histN, v.scaleMax, v.pal.up,
                       FMax(1.f, 1.15f * s), 0.f);
            DrawSeries(c, gx, gy, gw, gh, gr, v.histDown, v.histN, v.scaleMax, v.pal.down,
                       FMax(1.2f, 1.5f * s), 0.42f);
        }
        const float r1 = L.h * 0.27f, r2 = L.h * 0.73f;
        float x = gx + gw + 9.f * s;
        DrawChip(c, x + L.chip * 0.5f, r1, L.chip, false, v.pal.down, aD);
        DrawChip(c, x + L.chip * 0.5f, r2, L.chip, true, v.pal.up, aU);
        x += L.chip + L.gap;
        DrawSpeed(c, v, L, x, r1, v.down, idleD);
        DrawSpeed(c, v, L, x, r2, v.up, idleU);
    } else {  // meter
        const float r1 = L.h * 0.27f, r2 = L.h * 0.73f;
        float x = L.pad;
        const float bh = FMax(3.f, 4.f * s);
        const float bx = x + L.chip + L.gap;
        const float tx = bx + L.barW + 8.f * s;
        const double sm = (double)(v.scaleMax > 1.f ? v.scaleMax : 1.f);
        for (int row = 0; row < 2; row++) {
            const bool up = row == 1;
            const double bps = up ? v.up : v.down;
            const float cy = up ? r2 : r1;
            const RGBA col = up ? v.pal.up : v.pal.down;
            DrawChip(c, x + L.chip * 0.5f, cy, L.chip, up, col, up ? aU : aD);
            FillRoundRect(c, bx, cy - bh * 0.5f, L.barW, bh, bh * 0.5f, WithAlpha(v.text, 0.12f));
            float ratio = (float)(bps / sm);
            ratio = sqrtf(Clamp01(ratio));
            const float fw = L.barW * ratio;
            if (fw > 0.5f) {
                FillRoundRectGrad(c, bx, cy - bh * 0.5f, FMax(fw, bh), bh, bh * 0.5f, col,
                                  Lighten(col, 0.35f), false);
            }
            DrawSpeed(c, v, L, tx, cy, bps, IsIdle(bps));
        }
    }
}

// ---- Usage tooltip -------------------------------------------------------------------

enum { FN_TNUM = 2, FN_TLBL = 3 };  // tooltip fonts: semibold numbers, regular labels

struct TipModel {
    bool dark = true;
    bool bits = false;
    int arrowStyle = 1;
    float scale = 1.f;
    float radius = 12.f;
    Palette pal;
    RGBA text, textDim;
    uint64_t todayD = 0, todayU = 0, monthD = 0, monthU = 0, yearD = 0, yearU = 0;
    uint64_t sessD = 0, sessU = 0;
    uint64_t day[7] = {};              // total bytes per day, oldest first, last = today
    const wchar_t* dayLbl[7] = {};     // two-letter weekday names
    double peakDown = 0, peakUp = 0;   // fastest speeds today, bytes per second
};

static void TipFontSizes(float scale, float* num, float* lbl) {
    *num = 14.f * scale;
    *lbl = 12.5f * scale;
}

static void FormatBytes(uint64_t bytes, wchar_t* num, int cap, const wchar_t** unit) {
    static const wchar_t* kUnits[] = {L"B", L"KB", L"MB", L"GB", L"TB"};
    double v = (double)bytes;
    int i = 0;
    while (v >= 1000.0 && i < 4) {
        v /= 1024.0;
        i++;
    }
    int dec = 0;
    if (i > 0) dec = v < 10.0 ? 2 : (v < 100.0 ? 1 : 0);
    FmtFixed(num, cap, v, dec);
    *unit = kUnits[i];
}

static void RenderTip(Canvas& c, const TipModel& t) {
    g_arrowStyle = t.arrowStyle;
    const float s = t.scale;
    const float m = 12.f * s;  // room for the shadow
    const float padX = 16.f * s;

    // Column widths follow the font that is actually in use (Inter, Segoe UI, ...).
    const float numW = FMax(TextW(FN_TNUM, L"888"), FMax(TextW(FN_TNUM, L"88.8"), TextW(FN_TNUM, L"8.88")));
    const float unitW = FMax(FMax(TextW(FN_TLBL, L"MB"), TextW(FN_TLBL, L"GB")),
                             FMax(TextW(FN_TLBL, L"KB"), TextW(FN_TLBL, L"TB")));
    const float colD = numW + 3.f * s + unitW + 10.f * s, colU = colD, colT = colD + 6.f * s;
    const wchar_t* labels[5] = {L"Today", L"This month", L"This year", L"This session", L"Fastest today"};
    float labelW = 0.f;
    for (int i = 0; i < 5; i++) labelW = FMax(labelW, TextW(FN_TLBL, labels[i]));
    labelW += 10.f * s;
    const float tableW = labelW + colD + colU + colT;
    const float speedW = numW + 3.f * s + FMax(TextW(FN_TLBL, L"MB/s"), TextW(FN_TLBL, L"GB/s")) + 4.f * s;
    const float footW = labelW + 14.f * s + 2.f * (18.f * s + speedW) + 12.f * s;
    const float contentW = FMax(tableW, footW);
    labelW += contentW - tableW;
    const float cardW = contentW + 2.f * padX;
    const float cardH = 292.f * s;
    const float rad = FMin(t.radius * s, 16.f * s);
    c.Init((int)ceilf(cardW + 2.f * m), (int)ceilf(cardH + 2.f * m));

    // Soft shadow, then the card.
    for (int i = 0; i < 8; i++) {
        const float e = (float)(8 - i) * 1.4f * s;
        FillRoundRect(c, m - e, m - e + 3.f * s, cardW + 2.f * e, cardH + 2.f * e, rad + e,
                      Rgb(0, 0, 0, t.dark ? 0.030f : 0.016f));
    }
    const RGBA card = t.dark ? Rgb(37, 37, 42, 0.975f) : Rgb(252, 252, 254, 0.975f);
    const RGBA edge = t.dark ? Rgb(255, 255, 255, 0.11f) : Rgb(0, 0, 0, 0.09f);
    FillRoundRect(c, m, m, cardW, cardH, rad, card);
    StrokeRoundRect(c, m, m, cardW, cardH, rad, FMax(1.f, floorf(s + 0.25f)), edge);

    const RGBA line = WithAlpha(t.text, 0.08f);
    const float lineH = FMax(1.f, floorf(s));
    const float x0 = m + padX;
    const float rD = x0 + labelW + colD, rU = rD + colU, rT = rU + colT;

    auto textTop = [&](int fn, float cy) { return cy - TextH(fn) * 0.5f + 0.5f * s; };
    auto drawValR = [&](float rx, float cy, const wchar_t* num, const wchar_t* unit, RGBA numCol) {
        const float uw = TextW(FN_TLBL, unit), nw = TextW(FN_TNUM, num);
        const float ty = textTop(FN_TNUM, cy);
        TextDraw(c, FN_TLBL, rx - uw, ty + (TextAscent(FN_TNUM) - TextAscent(FN_TLBL)), unit, t.textDim);
        TextDraw(c, FN_TNUM, rx - uw - 3.f * s - nw, ty, num, numCol);
    };
    auto drawValL = [&](float x, float cy, const wchar_t* num, const wchar_t* unit, RGBA numCol) {
        const float nw = TextW(FN_TNUM, num);
        const float ty = textTop(FN_TNUM, cy);
        TextDraw(c, FN_TNUM, x, ty, num, numCol);
        TextDraw(c, FN_TLBL, x + nw + 3.f * s, ty + (TextAscent(FN_TNUM) - TextAscent(FN_TLBL)), unit, t.textDim);
        return x + nw + 3.f * s + TextW(FN_TLBL, unit);
    };
    auto bytesR = [&](float rx, float cy, uint64_t b, RGBA col) {
        wchar_t num[48];
        const wchar_t* unit = L"";
        FormatBytes(b, num, 48, &unit);
        drawValR(rx, cy, num, unit, col);
    };
    auto bytesL = [&](float x, float cy, uint64_t b, RGBA col) {
        wchar_t num[48];
        const wchar_t* unit = L"";
        FormatBytes(b, num, 48, &unit);
        return drawValL(x, cy, num, unit, col);
    };
    auto speedL = [&](float x, float cy, double bps, RGBA col) {
        wchar_t num[48];
        const wchar_t* unit = L"";
        FormatSpeed(bps, t.bits, num, 48, &unit);
        return drawValL(x, cy, num, unit, col);
    };

    // Title and column headers.
    TextDraw(c, FN_TNUM, x0, textTop(FN_TNUM, m + 23.f * s), L"Data usage", t.text);
    const float hy = m + 48.f * s;
    DrawChip(c, rD - 9.f * s, hy, 14.f * s, false, t.pal.down, 1.f);
    DrawChip(c, rU - 9.f * s, hy, 14.f * s, true, t.pal.up, 1.f);
    TextDraw(c, FN_TLBL, rT - TextW(FN_TLBL, L"Total"), textTop(FN_TLBL, hy), L"Total", t.textDim);
    FillRoundRect(c, x0, m + 61.f * s, contentW, lineH, 0.f, line);

    // Today / month / year.
    const wchar_t* names[3] = {L"Today", L"This month", L"This year"};
    const uint64_t vd[3] = {t.todayD, t.monthD, t.yearD};
    const uint64_t vu[3] = {t.todayU, t.monthU, t.yearU};
    for (int i = 0; i < 3; i++) {
        const float cy = m + 77.f * s + (float)i * 26.f * s;
        TextDraw(c, FN_TLBL, x0, textTop(FN_TLBL, cy), names[i], WithAlpha(t.text, 0.85f));
        bytesR(rD, cy, vd[i], WithAlpha(t.text, 0.9f));
        bytesR(rU, cy, vu[i], WithAlpha(t.text, 0.9f));
        bytesR(rT, cy, vd[i] + vu[i], t.text);
    }
    FillRoundRect(c, x0, m + 145.f * s, contentW, lineH, 0.f, line);

    // Last 7 days.
    uint64_t mx = 1;
    for (int i = 0; i < 7; i++) mx = t.day[i] > mx ? t.day[i] : mx;
    const float cty = m + 160.f * s;
    TextDraw(c, FN_TLBL, x0, textTop(FN_TLBL, cty), L"Last 7 days", t.textDim);
    {
        wchar_t num[48];
        const wchar_t* unit = L"";
        FormatBytes(mx > 1 ? mx : 0, num, 48, &unit);
        const float uw = TextW(FN_TLBL, unit), nw = TextW(FN_TLBL, num);
        const float rx = x0 + contentW;
        TextDraw(c, FN_TLBL, rx - uw, textTop(FN_TLBL, cty), unit, t.textDim);
        TextDraw(c, FN_TLBL, rx - uw - 3.f * s - nw, textTop(FN_TLBL, cty), num, t.textDim);
        const wchar_t* pk = L"Busiest";
        TextDraw(c, FN_TLBL, rx - uw - 3.f * s - nw - 6.f * s - TextW(FN_TLBL, pk), textTop(FN_TLBL, cty), pk, WithAlpha(t.textDim, 0.7f));
    }
    const float slot = contentW / 7.f, barW = slot * 0.5f;
    const float barTop = m + 173.f * s, barH = 40.f * s;
    for (int i = 0; i < 7; i++) {
        const float bx = x0 + slot * (float)i + (slot - barW) * 0.5f;
        const bool today = (i == 6);
        FillRoundRect(c, bx, barTop, barW, barH, 4.f * s, WithAlpha(t.text, 0.07f));
        float h = barH * (float)((double)t.day[i] / (double)mx);
        if (t.day[i] > 0 && h < 3.f * s) h = 3.f * s;
        if (h > 0.f) {
            const float a = today ? 1.f : 0.72f;
            FillRoundRectGrad(c, bx, barTop + barH - h, barW, h, FMin(4.f * s, h * 0.5f),
                              WithAlpha(t.pal.down, a), WithAlpha(t.pal.up, a), true);
        }
        if (t.dayLbl[i]) {
            const float lw = TextW(FN_TLBL, t.dayLbl[i]);
            TextDraw(c, FN_TLBL, bx + barW * 0.5f - lw * 0.5f, textTop(FN_TLBL, m + 224.f * s),
                     t.dayLbl[i], today ? t.text : t.textDim);
        }
    }
    FillRoundRect(c, x0, m + 236.f * s, contentW, lineH, 0.f, line);

    // Session and fastest speeds.
    const float f1 = m + 252.f * s, f2 = m + 276.f * s;
    TextDraw(c, FN_TLBL, x0, textTop(FN_TLBL, f1), L"This session", t.textDim);
    TextDraw(c, FN_TLBL, x0, textTop(FN_TLBL, f2), L"Fastest today", t.textDim);
    const float vx = x0 + labelW + 14.f * s;
    for (int row = 0; row < 2; row++) {
        const float cy = row == 0 ? f1 : f2;
        float x = vx;
        DrawChip(c, x + 7.f * s, cy, 14.f * s, false, t.pal.down, 1.f);
        x += 20.f * s;
        x = row == 0 ? bytesL(x, cy, t.sessD, WithAlpha(t.text, 0.9f)) : speedL(x, cy, t.peakDown, WithAlpha(t.text, 0.9f));
        x += 12.f * s;
        DrawChip(c, x + 7.f * s, cy, 14.f * s, true, t.pal.up, 1.f);
        x += 20.f * s;
        if (row == 0) bytesL(x, cy, t.sessU, WithAlpha(t.text, 0.9f));
        else speedL(x, cy, t.peakUp, WithAlpha(t.text, 0.9f));
    }
}

// ---- Palettes ------------------------------------------------------------------------

enum PaletteId { PAL_AURORA = 0, PAL_SUNSET, PAL_OCEAN, PAL_NEON, PAL_MONO, PAL_ACCENT, PAL_CANDY, PAL_LIME };

static Palette MakePalette(int id, bool dark, RGBA accent, RGBA text) {
    Palette p;
    switch (id) {
        case PAL_SUNSET:
            p.down = Rgb(251, 146, 60);
            p.up = Rgb(244, 63, 94);
            break;
        case PAL_CANDY:
            p.down = Rgb(244, 114, 182);
            p.up = Rgb(56, 189, 248);
            break;
        case PAL_LIME:
            p.down = Rgb(163, 230, 53);
            p.up = Rgb(34, 211, 238);
            break;
        case PAL_OCEAN:
            p.down = Rgb(56, 189, 248);
            p.up = Rgb(129, 140, 248);
            break;
        case PAL_NEON:
            p.down = Rgb(74, 222, 128);
            p.up = Rgb(232, 121, 249);
            break;
        case PAL_MONO:
            p.down = WithAlpha(text, 1.f);
            p.up = WithAlpha(MixC(text, dark ? Rgb(0, 0, 0) : Rgb(255, 255, 255), 0.45f), 1.f);
            break;
        case PAL_ACCENT:
            if (dark) {
                p.down = Lighten(accent, 0.35f);
                p.up = Lighten(accent, 0.62f);
            } else {
                p.down = accent;
                p.up = Darken(accent, 0.35f);
            }
            break;
        default:  // aurora
            p.down = Rgb(45, 212, 191);
            p.up = Rgb(167, 139, 250);
            break;
    }
    if (!dark && id != PAL_MONO) {
        p.down = Darken(p.down, 0.24f);
        p.up = Darken(p.up, 0.24f);
    }
    return p;
}

static void MakeTextColors(bool dark, RGBA* text, RGBA* dim) {
    if (dark) {
        *text = Rgb(255, 255, 255, 0.93f);
        *dim = Rgb(255, 255, 255, 0.58f);
    } else {
        *text = Rgb(20, 20, 26, 0.90f);
        *dim = Rgb(20, 20, 26, 0.56f);
    }
}
// ---- RENDERER END ------------------------------------------------------------------

// ---------------------------------------------------------------- settings

struct Settings {
    int skin = SK_CAPSULE;
    int palette = PAL_AURORA;
    int arrowStyle = ARW_SOLID;
    int themeMode = 0;  // 0 auto, 1 light, 2 dark
    bool bits = false;
    int updateMs = 1000;
    int placement = 0;  // 0 left of the tray, 1 left edge, 2 center, 3 right edge
    int offsetX = 0;
    int offsetY = 0;
    int sizePercent = 100;
    int radius = 10;
    int bgOpacity = 100;
    bool tint = true;
    bool dim = true;
    int adapterMode = 0;  // 0 auto (default route), 1 all physical adapters
    int autoFit = 1;  // 0 off, 1 shrink only, 2 shrink then switch to the Stack layout
    wchar_t fontName[48] = L"Inter";
    bool showTip = true;
    int tipDelay = 300;
    int resetToken = 0;
    wchar_t filter[64] = L"";
};

static Settings g_settings;
static std::mutex g_settingsMutex;

static bool StrEq(PCWSTR a, PCWSTR b) { return a && wcscmp(a, b) == 0; }
static int ClampInt(int v, int lo, int hi) { return v < lo ? lo : (v > hi ? hi : v); }

static void LoadSettings() {
    Settings s;

    PCWSTR v = Wh_GetStringSetting(L"skin");
    if (StrEq(v, L"stack")) s.skin = SK_STACK;
    else if (StrEq(v, L"pulse")) s.skin = SK_PULSE;
    else if (StrEq(v, L"meter")) s.skin = SK_METER;
    else s.skin = SK_CAPSULE;
    Wh_FreeStringSetting(v);

    v = Wh_GetStringSetting(L"palette");
    if (StrEq(v, L"sunset")) s.palette = PAL_SUNSET;
    else if (StrEq(v, L"ocean")) s.palette = PAL_OCEAN;
    else if (StrEq(v, L"neon")) s.palette = PAL_NEON;
    else if (StrEq(v, L"mono")) s.palette = PAL_MONO;
    else if (StrEq(v, L"accent")) s.palette = PAL_ACCENT;
    else if (StrEq(v, L"candy")) s.palette = PAL_CANDY;
    else if (StrEq(v, L"lime")) s.palette = PAL_LIME;
    else s.palette = PAL_AURORA;
    Wh_FreeStringSetting(v);

    v = Wh_GetStringSetting(L"arrowStyle");
    if (StrEq(v, L"soft")) s.arrowStyle = ARW_SOFT;
    else if (StrEq(v, L"bold")) s.arrowStyle = ARW_BOLD;
    else if (StrEq(v, L"triangle")) s.arrowStyle = ARW_TRIANGLE;
    else s.arrowStyle = ARW_SOLID;
    Wh_FreeStringSetting(v);

    v = Wh_GetStringSetting(L"themeMode");
    s.themeMode = StrEq(v, L"light") ? 1 : (StrEq(v, L"dark") ? 2 : 0);
    Wh_FreeStringSetting(v);

    v = Wh_GetStringSetting(L"unitMode");
    s.bits = StrEq(v, L"bits");
    Wh_FreeStringSetting(v);

    v = Wh_GetStringSetting(L"placement");
    if (StrEq(v, L"left")) s.placement = 1;
    else if (StrEq(v, L"center")) s.placement = 2;
    else if (StrEq(v, L"right")) s.placement = 3;
    else s.placement = 0;
    Wh_FreeStringSetting(v);

    v = Wh_GetStringSetting(L"adapterMode");
    s.adapterMode = StrEq(v, L"all") ? 1 : 0;
    Wh_FreeStringSetting(v);

    v = Wh_GetStringSetting(L"fontName");
    lstrcpynW(s.fontName, (v && v[0]) ? v : L"Inter", 48);
    Wh_FreeStringSetting(v);

    v = Wh_GetStringSetting(L"adapterFilter");
    lstrcpynW(s.filter, v ? v : L"", 64);
    Wh_FreeStringSetting(v);

    s.updateMs = ClampInt(Wh_GetIntSetting(L"updateMs"), 250, 5000);
    s.offsetX = ClampInt(Wh_GetIntSetting(L"offsetX"), -2000, 2000);
    s.offsetY = ClampInt(Wh_GetIntSetting(L"offsetY"), -200, 200);
    s.sizePercent = ClampInt(Wh_GetIntSetting(L"sizePercent"), 60, 200);
    s.radius = ClampInt(Wh_GetIntSetting(L"cornerRadius"), 0, 24);
    s.bgOpacity = ClampInt(Wh_GetIntSetting(L"backgroundOpacity"), 0, 100);
    s.tint = Wh_GetIntSetting(L"tintBackground") != 0;
    s.dim = Wh_GetIntSetting(L"dimWhenIdle") != 0;
    s.showTip = Wh_GetIntSetting(L"showTooltip") != 0;
    v = Wh_GetStringSetting(L"autoFit");
    s.autoFit = StrEq(v, L"off") ? 0 : (StrEq(v, L"smart") ? 2 : 1);
    Wh_FreeStringSetting(v);
    s.tipDelay = ClampInt(Wh_GetIntSetting(L"tooltipDelayMs"), 0, 2000);
    s.resetToken = Wh_GetIntSetting(L"resetUsageToken");

    Wh_Log(L"Settings loaded: layout=%d palette=%d autoFit=%d placement=%d", s.skin, s.palette,
           s.autoFit, s.placement);
    std::lock_guard<std::mutex> lock(g_settingsMutex);
    g_settings = s;
}

static Settings GetSettings() {
    std::lock_guard<std::mutex> lock(g_settingsMutex);
    return g_settings;
}

// ---------------------------------------------------------------- GDI text for the renderer

static HDC g_textDC = nullptr;
static HFONT g_font[4] = {nullptr, nullptr, nullptr, nullptr};
static int g_fontPx[4] = {0, 0, 0, 0};
static wchar_t g_wantedFont[48] = L"";
static wchar_t g_faceReg[48] = L"Segoe UI";
static wchar_t g_faceSemi[48] = L"Segoe UI";
static bool g_semiByWeight = true;

static int CALLBACK FaceEnumProc(const LOGFONTW*, const TEXTMETRICW*, DWORD, LPARAM lp) {
    *(bool*)lp = true;
    return 0;
}

static bool FaceInstalled(const wchar_t* face) {
    LOGFONTW lf = {};
    lf.lfCharSet = DEFAULT_CHARSET;
    lstrcpynW(lf.lfFaceName, face, 32);
    bool found = false;
    EnumFontFamiliesExW(g_textDC, &lf, FaceEnumProc, (LPARAM)&found, 0);
    return found;
}

// Picks the fonts for the "fontName" setting. If that font is not installed, Segoe UI is used.
static void ApplyFontSetting(const wchar_t* wanted) {
    if (wcscmp(g_wantedFont, wanted) == 0) return;
    lstrcpynW(g_wantedFont, wanted, 48);
    if (FaceInstalled(wanted)) lstrcpynW(g_faceReg, wanted, 48);
    else lstrcpynW(g_faceReg, L"Segoe UI", 48);

    wchar_t semi[48];
    lstrcpynW(semi, g_faceReg, 32);
    lstrcatW(semi, L" SemiBold");
    if (FaceInstalled(semi)) {
        lstrcpynW(g_faceSemi, semi, 48);
        g_semiByWeight = false;
    } else {
        lstrcpynW(g_faceSemi, g_faceReg, 48);
        g_semiByWeight = true;  // ask for the semibold weight of the regular family
    }
    for (int i = 0; i < 4; i++) {
        if (g_font[i]) DeleteObject(g_font[i]);
        g_font[i] = nullptr;
        g_fontPx[i] = 0;
    }
}

constexpr int kTextSS = 4;  // text is drawn 4x larger and averaged down: natural letter spacing

static void EnsureFont(int fn, float pxf) {
    const int px4 = IMax(4 * kTextSS, (int)(pxf * (float)kTextSS + 0.5f));
    if (g_font[fn] && g_fontPx[fn] == px4) return;
    if (g_font[fn]) DeleteObject(g_font[fn]);
    const bool semi = (fn == FN_NUM || fn == FN_TNUM);
    g_font[fn] = CreateFontW(-px4, 0, 0, 0, (semi && g_semiByWeight) ? FW_SEMIBOLD : FW_NORMAL,
                             FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                             CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
                             semi ? g_faceSemi : g_faceReg);
    g_fontPx[fn] = px4;
}

static float TextW(int fn, const wchar_t* s) {
    if (!g_font[fn]) return 0.f;
    HGDIOBJ old = SelectObject(g_textDC, g_font[fn]);
    SIZE sz = {};
    GetTextExtentPoint32W(g_textDC, s, lstrlenW(s), &sz);
    SelectObject(g_textDC, old);
    return (float)sz.cx / (float)kTextSS;
}

static float TextH(int fn) {
    if (!g_font[fn]) return 0.f;
    HGDIOBJ old = SelectObject(g_textDC, g_font[fn]);
    TEXTMETRICW tm = {};
    GetTextMetricsW(g_textDC, &tm);
    SelectObject(g_textDC, old);
    return (float)tm.tmHeight / (float)kTextSS;
}

static float TextAscent(int fn) {
    if (!g_font[fn]) return 0.f;
    HGDIOBJ old = SelectObject(g_textDC, g_font[fn]);
    TEXTMETRICW tm = {};
    GetTextMetricsW(g_textDC, &tm);
    SelectObject(g_textDC, old);
    return (float)tm.tmAscent / (float)kTextSS;
}

// Draws white text on a black bitmap at 4x size, averages every 4x4 block into one coverage
// value, and blends that into the per-pixel-alpha canvas. Drawing large and shrinking avoids
// GDI's hinting and whole-pixel letter spacing, so the text keeps its natural proportions.
static void TextDraw(Canvas& c, int fn, float x, float y, const wchar_t* s, RGBA col) {
    const int len = lstrlenW(s);
    if (len == 0 || !g_font[fn]) return;
    HGDIOBJ oldFont = SelectObject(g_textDC, g_font[fn]);
    SIZE sz = {};
    GetTextExtentPoint32W(g_textDC, s, len, &sz);

    const int ix = (int)floorf(x), iy = (int)floorf(y);
    const int padX = 2 * kTextSS + (int)((x - (float)ix) * (float)kTextSS + 0.5f);
    const int padY = 1 * kTextSS + (int)((y - (float)iy) * (float)kTextSS + 0.5f);
    const int outW = (sz.cx + padX + 2 * kTextSS + kTextSS - 1) / kTextSS;
    const int outH = (sz.cy + padY + 1 * kTextSS + kTextSS - 1) / kTextSS;
    const int tw = outW * kTextSS, th = outH * kTextSS;

    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = tw;
    bi.bmiHeader.biHeight = -th;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    HBITMAP bmp = CreateDIBSection(g_textDC, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!bmp || !bits) {
        if (bmp) DeleteObject(bmp);
        SelectObject(g_textDC, oldFont);
        return;
    }
    HGDIOBJ oldBmp = SelectObject(g_textDC, bmp);
    memset(bits, 0, (size_t)tw * (size_t)th * 4);
    SetBkMode(g_textDC, TRANSPARENT);
    SetTextColor(g_textDC, RGB(255, 255, 255));
    TextOutW(g_textDC, padX, padY, s, len);
    GdiFlush();

    const uint32_t* p = (const uint32_t*)bits;
    const float norm = 1.f / (float)(kTextSS * kTextSS * 255);
    for (int j = 0; j < outH; j++) {
        for (int i = 0; i < outW; i++) {
            int sum = 0;
            for (int dy = 0; dy < kTextSS; dy++) {
                const uint32_t* row = p + (size_t)(j * kTextSS + dy) * (size_t)tw + (size_t)(i * kTextSS);
                for (int dx = 0; dx < kTextSS; dx++) sum += (int)((row[dx] >> 8) & 255);
            }
            if (sum > 0) Blend(c, ix - 2 + i, iy - 1 + j, col, (float)sum * norm);
        }
    }
    SelectObject(g_textDC, oldBmp);
    DeleteObject(bmp);
    SelectObject(g_textDC, oldFont);
}

// ---------------------------------------------------------------- network sampling

// Layout of MIB_IFROW / MIB_IFTABLE (declared here so no networking headers are needed).
struct WhIfRow {
    WCHAR wszName[256];
    DWORD dwIndex, dwType, dwMtu, dwSpeed, dwPhysAddrLen;
    UCHAR bPhysAddr[8];
    DWORD dwAdminStatus, dwOperStatus, dwLastChange;
    DWORD dwInOctets, dwInUcastPkts, dwInNUcastPkts, dwInDiscards, dwInErrors, dwInUnknownProtos;
    DWORD dwOutOctets, dwOutUcastPkts, dwOutNUcastPkts, dwOutDiscards, dwOutErrors, dwOutQLen;
    DWORD dwDescrLen;
    UCHAR bDescr[256];
};
struct WhIfTable {
    DWORD dwNumEntries;
    WhIfRow table[1];
};

typedef DWORD(WINAPI* GetIfTable_t)(void*, PULONG, BOOL);
typedef DWORD(WINAPI* GetBestInterface_t)(ULONG, PDWORD);

static HMODULE g_iphlpapi = nullptr;
static GetIfTable_t g_getIfTable = nullptr;
static GetBestInterface_t g_getBestInterface = nullptr;

constexpr int kHist = 48;
static float g_histDown[kHist];
static float g_histUp[kHist];
static double g_down = 0, g_up = 0;
static float g_scaleMax = 256.f * 1024.f;
static ULONGLONG g_lastTick = 0;
static std::vector<BYTE> g_ifBuf;
static std::unordered_map<DWORD, std::pair<DWORD, DWORD>> g_prev;

static bool ContainsNoCase(const char* hay, const wchar_t* needleW) {
    char needle[64];
    int n = 0;
    for (; needleW[n] && n < 63; n++) {
        wchar_t ch = needleW[n];
        if (ch >= L'A' && ch <= L'Z') ch = (wchar_t)(ch - L'A' + L'a');
        needle[n] = ch < 128 ? (char)ch : '?';
    }
    needle[n] = 0;
    if (n == 0) return false;
    for (const char* h = hay; *h; h++) {
        int i = 0;
        while (needle[i]) {
            char hc = h[i];
            if (!hc) break;
            if (hc >= 'A' && hc <= 'Z') hc = (char)(hc - 'A' + 'a');
            if (hc != needle[i]) break;
            i++;
        }
        if (!needle[i]) return true;
    }
    return false;
}

static bool LooksVirtual(const char* descr) {
    static const wchar_t* kWords[] = {L"virtual", L"vmware", L"hyper-v", L"vpn", L"tap-",
                                      L"wintun", L"wireguard", L"loopback", L"pseudo",
                                      L"miniport", L"tailscale", L"npcap", L"bluetooth"};
    for (const wchar_t* w : kWords) {
        if (ContainsNoCase(descr, w)) return true;
    }
    return false;
}

static void AddUsage(unsigned long long dIn, unsigned long long dOut, double down, double up);

static void SampleNetwork(const Settings& s) {
    if (!g_getIfTable) return;

    ULONG size = (ULONG)g_ifBuf.size();
    DWORD rc = g_getIfTable(g_ifBuf.empty() ? nullptr : g_ifBuf.data(), &size, FALSE);
    if (rc == ERROR_INSUFFICIENT_BUFFER) {
        g_ifBuf.resize((size_t)size + 4096);
        size = (ULONG)g_ifBuf.size();
        rc = g_getIfTable(g_ifBuf.data(), &size, FALSE);
    }
    if (rc != NO_ERROR) return;

    DWORD bestIndex = 0;
    bool haveBest = false;
    if (s.adapterMode == 0 && g_getBestInterface) {
        haveBest = g_getBestInterface(0x08080808u, &bestIndex) == NO_ERROR;
    }

    const WhIfTable* table = (const WhIfTable*)g_ifBuf.data();
    unsigned long long dIn = 0, dOut = 0;
    const bool first = (g_lastTick == 0);
    for (DWORD i = 0; i < table->dwNumEntries; i++) {
        const WhIfRow& row = table->table[i];
        DWORD dIn32 = 0, dOut32 = 0;
        auto it = g_prev.find(row.dwIndex);
        if (it != g_prev.end() && !first) {
            dIn32 = row.dwInOctets - it->second.first;    // unsigned wrap-around is intended
            dOut32 = row.dwOutOctets - it->second.second;
        }
        g_prev[row.dwIndex] = std::make_pair(row.dwInOctets, row.dwOutOctets);

        bool selected;
        if (s.filter[0]) {
            selected = ContainsNoCase((const char*)row.bDescr, s.filter);
        } else if (s.adapterMode == 0) {
            selected = haveBest && row.dwIndex == bestIndex;
        } else {
            selected = (row.dwOperStatus == 4 || row.dwOperStatus == 5) && row.dwType != 24 &&
                       !LooksVirtual((const char*)row.bDescr);
        }
        if (selected) {
            dIn += dIn32;
            dOut += dOut32;
        }
    }

    const ULONGLONG now = GetTickCount64();
    double down = 0, up = 0;
    if (!first) {
        const double dt = (double)(now - g_lastTick) / 1000.0;
        if (dt > 0.05) {
            down = (double)dIn / dt;
            up = (double)dOut / dt;
        }
    }
    g_lastTick = now;
    g_down = down;
    g_up = up;
    AddUsage(dIn, dOut, down, up);

    for (int i = 0; i < kHist - 1; i++) {
        g_histDown[i] = g_histDown[i + 1];
        g_histUp[i] = g_histUp[i + 1];
    }
    g_histDown[kHist - 1] = (float)down;
    g_histUp[kHist - 1] = (float)up;

    float peak = 0.f;
    for (int i = 0; i < kHist; i++) peak = FMax(peak, FMax(g_histDown[i], g_histUp[i]));
    const float target = FMax(128.f * 1024.f, peak * 1.15f);
    if (target > g_scaleMax) g_scaleMax = target;
    else g_scaleMax += (target - g_scaleMax) * 0.08f;
}

// ---------------------------------------------------------------- usage history

static RGBA SystemAccent();

struct UsageRec {
    uint32_t date, peakDown, peakUp, reserved;  // date is year*10000 + month*100 + day
    uint64_t down, up;                           // bytes
};
static_assert(sizeof(UsageRec) == 32, "UsageRec layout");
struct UsageHeader {
    uint32_t magic, version, count, reserved;
};
constexpr uint32_t kUsageMagic = 0x474E5357;
constexpr size_t kMaxUsageDays = 800;

static std::vector<UsageRec> g_usage;  // sorted by date, oldest first
static uint64_t g_sessDown = 0, g_sessUp = 0;
static bool g_usageDirty = false;
static ULONGLONG g_lastSave = 0;
static int g_lastResetToken = 0;

static uint32_t DateKey(const SYSTEMTIME& st) {
    return (uint32_t)st.wYear * 10000u + (uint32_t)st.wMonth * 100u + (uint32_t)st.wDay;
}

static void LoadUsage() {
    g_usage.clear();
    std::vector<BYTE> buf(sizeof(UsageHeader) + kMaxUsageDays * sizeof(UsageRec));
    const size_t n = Wh_GetBinaryValue(L"usage", buf.data(), buf.size());
    if (n < sizeof(UsageHeader)) return;
    UsageHeader h;
    memcpy(&h, buf.data(), sizeof(h));
    if (h.magic != kUsageMagic || h.version != 1 || h.count > kMaxUsageDays) return;
    if (n < sizeof(UsageHeader) + (size_t)h.count * sizeof(UsageRec)) return;
    g_usage.resize(h.count);
    if (h.count) memcpy(g_usage.data(), buf.data() + sizeof(UsageHeader), (size_t)h.count * sizeof(UsageRec));
}

static void SaveUsage() {
    UsageHeader h = {kUsageMagic, 1, (uint32_t)g_usage.size(), 0};
    std::vector<BYTE> buf(sizeof(h) + g_usage.size() * sizeof(UsageRec));
    memcpy(buf.data(), &h, sizeof(h));
    if (!g_usage.empty()) memcpy(buf.data() + sizeof(h), g_usage.data(), g_usage.size() * sizeof(UsageRec));
    Wh_SetBinaryValue(L"usage", buf.data(), buf.size());
    g_usageDirty = false;
    g_lastSave = GetTickCount64();
}

static int FindDay(uint32_t key) {
    for (size_t i = g_usage.size(); i-- > 0;) {
        if (g_usage[i].date == key) return (int)i;
        if (g_usage[i].date < key) break;
    }
    return -1;
}

static UsageRec* TodayRecord() {
    SYSTEMTIME st;
    GetLocalTime(&st);
    const uint32_t key = DateKey(st);
    int idx = FindDay(key);
    if (idx < 0) {
        UsageRec r = {};
        r.date = key;
        size_t pos = g_usage.size();
        while (pos > 0 && g_usage[pos - 1].date > key) pos--;
        g_usage.insert(g_usage.begin() + (ptrdiff_t)pos, r);
        if (g_usage.size() > kMaxUsageDays) {
            g_usage.erase(g_usage.begin(), g_usage.begin() + (ptrdiff_t)(g_usage.size() - kMaxUsageDays));
        }
        idx = FindDay(key);
    }
    return idx >= 0 ? &g_usage[(size_t)idx] : nullptr;
}

static void AddUsage(unsigned long long dIn, unsigned long long dOut, double down, double up) {
    g_sessDown += dIn;
    g_sessUp += dOut;
    UsageRec* r = TodayRecord();
    if (!r) return;
    bool changed = (dIn != 0 || dOut != 0);
    r->down += dIn;
    r->up += dOut;
    const uint32_t pd = (uint32_t)(down > 4e9 ? 4e9 : down);
    const uint32_t pu = (uint32_t)(up > 4e9 ? 4e9 : up);
    if (pd > r->peakDown) { r->peakDown = pd; changed = true; }
    if (pu > r->peakUp) { r->peakUp = pu; changed = true; }
    if (changed) g_usageDirty = true;
    if (g_usageDirty && GetTickCount64() - g_lastSave > 60000) SaveUsage();
}

static void ResetUsage() {
    g_usage.clear();
    g_sessDown = g_sessUp = 0;
    SaveUsage();
}

static void BuildTipModel(const Settings& s, float scale, bool dark, TipModel& t) {
    static const wchar_t* kDow[7] = {L"Su", L"Mo", L"Tu", L"We", L"Th", L"Fr", L"Sa"};
    SYSTEMTIME now;
    GetLocalTime(&now);
    const uint32_t today = DateKey(now);
    const uint32_t ym = today / 100, yy = today / 10000;

    t.dark = dark;
    t.bits = s.bits;
    t.arrowStyle = s.arrowStyle;
    t.scale = scale;
    t.radius = (float)s.radius + 2.f;
    MakeTextColors(dark, &t.text, &t.textDim);
    t.pal = MakePalette(s.palette, dark, SystemAccent(), t.text);

    for (const UsageRec& r : g_usage) {
        if (r.date / 10000 != yy) continue;
        t.yearD += r.down;
        t.yearU += r.up;
        if (r.date / 100 == ym) {
            t.monthD += r.down;
            t.monthU += r.up;
        }
        if (r.date == today) {
            t.todayD += r.down;
            t.todayU += r.up;
            t.peakDown = (double)r.peakDown;
            t.peakUp = (double)r.peakUp;
        }
    }
    t.sessD = g_sessDown;
    t.sessU = g_sessUp;

    FILETIME ft;
    SystemTimeToFileTime(&now, &ft);
    ULARGE_INTEGER base;
    base.LowPart = ft.dwLowDateTime;
    base.HighPart = ft.dwHighDateTime;
    for (int i = 0; i < 7; i++) {
        const int back = 6 - i;
        ULARGE_INTEGER u = base;
        u.QuadPart -= (ULONGLONG)back * 864000000000ULL;
        FILETIME f2;
        f2.dwLowDateTime = u.LowPart;
        f2.dwHighDateTime = u.HighPart;
        SYSTEMTIME d;
        FileTimeToSystemTime(&f2, &d);
        const int idx = FindDay(DateKey(d));
        t.day[i] = idx >= 0 ? g_usage[(size_t)idx].down + g_usage[(size_t)idx].up : 0;
        t.dayLbl[i] = kDow[d.wDayOfWeek % 7];
    }
}

// ---------------------------------------------------------------- where do the taskbar icons end?

// A helper thread asks Windows (UI Automation) for the position of the taskbar buttons every
// couple of seconds. It runs on its own thread so that a slow answer can never freeze the
// widget window or the taskbar.
static volatile LONG g_appsRightScreen = -1;  // right edge of the last taskbar button; -1 = unknown
static HANDLE g_uiaThread = nullptr;
static HANDLE g_uiaStop = nullptr;

static LONG QueryAppsRight(IUIAutomation* uia, HWND tray) {
    RECT trc = {};
    if (!GetWindowRect(tray, &trc)) return -1;
    LONG limit = trc.right;  // buttons at or beyond the notification area are not apps
    HWND notify = FindWindowExW(tray, nullptr, L"TrayNotifyWnd", nullptr);
    RECT nr = {};
    if (notify && GetWindowRect(notify, &nr) && nr.left > trc.left && nr.left < trc.right) limit = nr.left;

    IUIAutomationElement* root = nullptr;
    if (FAILED(uia->ElementFromHandle((void*)tray, &root)) || !root) return -1;

    VARIANT var;
    VariantInit(&var);
    var.vt = VT_I4;
    var.lVal = UIA_ButtonControlTypeId;
    IUIAutomationCondition* cond = nullptr;
    uia->CreatePropertyCondition(UIA_ControlTypePropertyId, var, &cond);

    LONG bestTaskbar = -1, bestOther = -1;
    IUIAutomationElementArray* arr = nullptr;
    if (cond && SUCCEEDED(root->FindAll(TreeScope_Descendants, cond, &arr)) && arr) {
        int n = 0;
        arr->get_Length(&n);
        for (int i = 0; i < n; i++) {
            IUIAutomationElement* el = nullptr;
            if (FAILED(arr->GetElement(i, &el)) || !el) continue;
            RECT r = {};
            BSTR cls = nullptr;
            el->get_CurrentClassName(&cls);
            const bool trayItem = cls && wcsncmp(cls, L"SystemTray", 10) == 0;
            const bool taskbarItem = cls && wcsncmp(cls, L"Taskbar.", 8) == 0;
            if (cls) SysFreeString(cls);
            if (!trayItem && SUCCEEDED(el->get_CurrentBoundingRectangle(&r)) && r.right > r.left &&
                r.left >= trc.left && r.right <= limit + 1) {
                if (taskbarItem && r.right > bestTaskbar) bestTaskbar = r.right;
                if (r.right > bestOther) bestOther = r.right;
            }
            el->Release();
        }
        arr->Release();
    }
    if (cond) cond->Release();
    root->Release();
    // Windows 11 names its taskbar buttons "Taskbar.*"; older versions need the broader match.
    return bestTaskbar >= 0 ? bestTaskbar : bestOther;
}

static DWORD WINAPI UiaThread(LPVOID) {
    const HRESULT hrInit = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    IUIAutomation* uia = nullptr;
    if (SUCCEEDED(hrInit)) {
        CoCreateInstance(CLSID_CUIAutomation, nullptr, CLSCTX_INPROC_SERVER, IID_IUIAutomation,
                         (void**)&uia);
    }
    while (uia && WaitForSingleObject(g_uiaStop, 2000) == WAIT_TIMEOUT) {
        LONG v = -1;
        if (GetSettings().autoFit != 0) {
            HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr);
            DWORD pid = 0;
            if (tray) GetWindowThreadProcessId(tray, &pid);
            if (tray && pid == GetCurrentProcessId()) v = QueryAppsRight(uia, tray);
        }
        InterlockedExchange(&g_appsRightScreen, v);
    }
    if (uia) uia->Release();
    if (SUCCEEDED(hrInit)) CoUninitialize();
    return 0;
}

// ---------------------------------------------------------------- widget window

typedef HANDLE (WINAPI* DpiCtxFn)(HANDLE);
typedef HANDLE (WINAPI* GetWindowDpiCtx_t)(HWND);
typedef UINT(WINAPI* GetDpiForWindow_t)(HWND);

static GetWindowDpiCtx_t g_getWindowDpiCtx = nullptr;
static DpiCtxFn g_setThreadDpiCtx = nullptr;
static GetDpiForWindow_t g_getDpiForWindow = nullptr;

static const wchar_t kWidgetClass[] = L"WhNetworkSpeedOnTaskbarWidget";
static HWND g_hwnd = nullptr;
static HWND g_hwndParent = nullptr;
static HWND g_tipHwnd = nullptr;
static bool g_tipVisible = false;
static float g_tipAlpha = 0.f;  // 0..255, moves toward g_tipTarget
static int g_tipTarget = 0;     // 255 while the cursor is on the widget, 0 otherwise
static int g_lastX = -1, g_lastY = -1, g_lastW = -1, g_lastH = -1;
static bool g_classRegistered = false;

static LRESULT CALLBACK WidgetProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_NCHITTEST) return HTTRANSPARENT;  // never steal clicks from the taskbar
    if (msg == WM_ERASEBKGND) return 1;
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

static void DestroyWidget() {
    if (g_tipVisible && g_tipHwnd) ShowWindow(g_tipHwnd, SW_HIDE);
    g_tipVisible = false;
    g_tipTarget = 0;
    g_tipAlpha = 0.f;
    g_lastX = g_lastY = g_lastW = g_lastH = -1;
    if (g_hwnd) {
        if (IsWindow(g_hwnd)) DestroyWindow(g_hwnd);
        g_hwnd = nullptr;
        g_hwndParent = nullptr;
    }
}

static bool CreateWidget(HWND tray) {
    if (g_getWindowDpiCtx && g_setThreadDpiCtx) {
        // A child window must share the DPI awareness of its parent.
        g_setThreadDpiCtx(g_getWindowDpiCtx(tray));
    }
    g_hwnd = CreateWindowExW(WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE, kWidgetClass,
                             L"", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS, 0, 0, 1, 1, tray,
                             nullptr, GetModuleHandleW(nullptr), nullptr);
    g_hwndParent = g_hwnd ? tray : nullptr;
    return g_hwnd != nullptr;
}

static bool ReadDword(HKEY root, const wchar_t* path, const wchar_t* name, DWORD* out) {
    DWORD size = sizeof(DWORD);
    return RegGetValueW(root, path, name, RRF_RT_REG_DWORD, nullptr, out, &size) == ERROR_SUCCESS;
}

static bool TaskbarIsDark(int mode) {
    if (mode == 1) return false;
    if (mode == 2) return true;
    DWORD light = 0;
    if (ReadDword(HKEY_CURRENT_USER,
                  L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                  L"SystemUsesLightTheme", &light)) {
        return light == 0;
    }
    return true;
}

static RGBA SystemAccent() {
    DWORD v = 0;
    if (ReadDword(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\DWM", L"AccentColor", &v)) {
        return Rgb((int)(v & 0xFF), (int)((v >> 8) & 0xFF), (int)((v >> 16) & 0xFF));
    }
    return Rgb(0, 120, 212);
}

static void PushCanvas(HWND hwnd, const Canvas& canvas, BYTE alpha) {
    if (!hwnd || canvas.w <= 0 || canvas.h <= 0) return;
    HDC screen = GetDC(nullptr);
    HDC mem = CreateCompatibleDC(screen);
    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = canvas.w;
    bi.bmiHeader.biHeight = -canvas.h;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    HBITMAP dib = CreateDIBSection(screen, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (dib && bits) {
        // Premultiplied alpha: scale all channels for the fade, UpdateLayeredWindow applies it too.
        memcpy(bits, canvas.px.data(), (size_t)canvas.w * (size_t)canvas.h * 4);
        HGDIOBJ old = SelectObject(mem, dib);
        POINT src = {0, 0};
        SIZE size = {canvas.w, canvas.h};
        BLENDFUNCTION bf = {AC_SRC_OVER, 0, alpha, AC_SRC_ALPHA};
        UpdateLayeredWindow(hwnd, screen, nullptr, &size, mem, &src, 0, &bf, ULW_ALPHA);
        SelectObject(mem, old);
    }
    if (dib) DeleteObject(dib);
    DeleteDC(mem);
    ReleaseDC(nullptr, screen);
}

// ---------------------------------------------------------------- hover tooltip

static bool g_hovering = false;
static ULONGLONG g_hoverSince = 0, g_outSince = 0, g_tipLastStep = 0;
static Canvas g_tipCanvas;
static int g_tipX = 0, g_tipY = 0;
static bool g_tipAbove = true;
static float g_tipScale = 1.f;
static UINT_PTR g_hoverTimer = 0;
static UINT g_hoverPeriod = 0;

static void SetHoverPeriod(UINT ms) {
    if (g_hoverPeriod == ms && g_hoverTimer) return;
    if (g_hoverTimer) KillTimer(nullptr, g_hoverTimer);
    g_hoverTimer = SetTimer(nullptr, 0, ms, nullptr);
    g_hoverPeriod = ms;
}

static void DestroyTip() {
    if (g_tipHwnd) {
        if (IsWindow(g_tipHwnd)) DestroyWindow(g_tipHwnd);
        g_tipHwnd = nullptr;
    }
    g_tipVisible = false;
    g_tipAlpha = 0.f;
    g_tipTarget = 0;
}

static void UpdateTip(const Settings& s, bool place) {
    if (!g_hwnd) return;
    const UINT dpi = g_getDpiForWindow ? g_getDpiForWindow(g_hwnd) : 96;
    const float scale = ((float)dpi / 96.f) * (float)s.sizePercent / 100.f;
    TipModel t;
    BuildTipModel(s, scale, TaskbarIsDark(s.themeMode), t);
    float numPx = 0, lblPx = 0;
    TipFontSizes(scale, &numPx, &lblPx);
    ApplyFontSetting(s.fontName);
    EnsureFont(FN_TNUM, numPx);
    EnsureFont(FN_TLBL, lblPx);
    RenderTip(g_tipCanvas, t);
    if (!place) return;

    // The card floats above the taskbar (below it for a top taskbar), with a small gap, and is
    // centered on the widget.
    RECT wr = {}, tr = {};
    GetWindowRect(g_hwnd, &wr);
    GetWindowRect(g_hwndParent ? g_hwndParent : g_hwnd, &tr);
    MONITORINFO mi = {};
    mi.cbSize = sizeof(mi);
    HMONITOR mon = MonitorFromWindow(g_hwnd, MONITOR_DEFAULTTONEAREST);
    GetMonitorInfoW(mon, &mi);
    const int shadow = (int)(12.f * scale), gap = (int)(10.f * scale);
    g_tipScale = scale;
    g_tipAbove = (tr.top + tr.bottom) / 2 > (mi.rcMonitor.top + mi.rcMonitor.bottom) / 2;
    int x = (wr.left + wr.right) / 2 - g_tipCanvas.w / 2;
    const int minX = mi.rcMonitor.left - shadow + 6;
    const int maxX = mi.rcMonitor.right - g_tipCanvas.w + shadow - 6;
    if (x > maxX) x = maxX;
    if (x < minX) x = minX;
    g_tipX = x;
    g_tipY = g_tipAbove ? tr.top - g_tipCanvas.h + shadow - gap : tr.bottom - shadow + gap;
}

static float TipEased() {
    const float p = g_tipAlpha / 255.f;
    return p * p * (3.f - 2.f * p);
}

// Fade plus a small slide (8 px) toward the final position.
static void ApplyTipFrame() {
    if (!g_tipHwnd) return;
    const float e = TipEased();
    const int off = (int)((1.f - e) * 8.f * g_tipScale);
    SetWindowPos(g_tipHwnd, nullptr, g_tipX, g_tipY + (g_tipAbove ? off : -off), 0, 0,
                 SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
    PushCanvas(g_tipHwnd, g_tipCanvas, (BYTE)(e * 255.f + 0.5f));
}

static void BeginShowTip(const Settings& s) {
    if (!g_tipHwnd) {
        g_tipHwnd = CreateWindowExW(WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW |
                                        WS_EX_NOACTIVATE | WS_EX_TRANSPARENT,
                                    kWidgetClass, L"", WS_POPUP, 0, 0, 1, 1, nullptr, nullptr,
                                    GetModuleHandleW(nullptr), nullptr);
        if (!g_tipHwnd) return;
    }
    UpdateTip(s, true);
    g_tipAlpha = 0.f;
    g_tipTarget = 255;
    g_tipLastStep = GetTickCount64();
    SetWindowPos(g_tipHwnd, HWND_TOPMOST, g_tipX, g_tipY, g_tipCanvas.w, g_tipCanvas.h,
                 SWP_NOACTIVATE);
    ApplyTipFrame();  // fully transparent first, then show: no flash of an empty window
    SetWindowPos(g_tipHwnd, HWND_TOPMOST, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW);
    g_tipVisible = true;
    SetHoverPeriod(30);
}

static void StepTip(ULONGLONG dtMs) {
    const float target = (float)g_tipTarget;
    if (g_tipAlpha < target) g_tipAlpha = FMin(target, g_tipAlpha + 255.f * (float)dtMs / 160.f);
    else if (g_tipAlpha > target) g_tipAlpha = FMax(target, g_tipAlpha - 255.f * (float)dtMs / 120.f);

    if (g_tipAlpha <= 0.f && g_tipTarget == 0) {
        ShowWindow(g_tipHwnd, SW_HIDE);
        g_tipVisible = false;
        SetHoverPeriod(120);
        return;
    }
    ApplyTipFrame();
    SetHoverPeriod(g_tipAlpha == target ? 120 : 30);
}

// The widget ignores the mouse (clicks pass through to the taskbar), so hovering is detected
// by polling the cursor position. The card only starts to fade out after the cursor has been
// away for 150 ms, so a brief jitter does not make it blink.
static void HoverTick() {
    const Settings s = GetSettings();
    bool inside = false;
    if (s.showTip && g_hwnd && IsWindowVisible(g_hwnd)) {
        POINT pt;
        RECT r;
        if (GetCursorPos(&pt) && GetWindowRect(g_hwnd, &r)) inside = PtInRect(&r, pt) != 0;
    }
    const ULONGLONG now = GetTickCount64();
    ULONGLONG dt = g_tipLastStep ? now - g_tipLastStep : 0;
    if (dt > 100) dt = 100;
    g_tipLastStep = now;

    if (inside) {
        g_outSince = 0;
        if (!g_hovering) {
            g_hovering = true;
            g_hoverSince = now;
        }
        if (g_tipTarget == 0 && now - g_hoverSince >= (ULONGLONG)s.tipDelay) {
            g_tipTarget = 255;
            if (!g_tipVisible) BeginShowTip(s);
        }
    } else {
        g_hovering = false;
        if (g_tipTarget != 0) {
            if (g_outSince == 0) g_outSince = now;
            if (now - g_outSince >= 150) g_tipTarget = 0;
        }
    }
    if (g_tipVisible) StepTip(dt);
}

static int g_fitIndex = 0;

static void MakeView(View& v, const Settings& s, int skin, float factor, float dpiScale, int TH,
                     bool dark) {
    v.skin = skin;
    v.arrowStyle = s.arrowStyle;
    v.dark = dark;
    v.bits = s.bits;
    v.dim = s.dim;
    v.tint = s.tint;
    v.scale = dpiScale * (float)s.sizePercent / 100.f * factor;
    v.maxH = (float)TH - 6.f * dpiScale;
    v.radius = (float)s.radius;
    v.bgOpacity = (float)s.bgOpacity / 100.f;
    MakeTextColors(v.dark, &v.text, &v.textDim);
    v.pal = MakePalette(s.palette, v.dark, SystemAccent(), v.text);
    v.down = g_down;
    v.up = g_up;
    v.histDown = g_histDown;
    v.histUp = g_histUp;
    v.histN = kHist;
    v.scaleMax = g_scaleMax;
}

// Sets the fonts for this view and computes its layout. Returns the widget width.
static int BuildLayout(const View& v, Layout& L) {
    float numPx = 0, unitPx = 0;
    FontSizes(v, &numPx, &unitPx);
    EnsureFont(FN_NUM, numPx);
    EnsureFont(FN_UNIT, unitPx);
    ComputeLayout(v, L);
    return (int)L.w;
}

// Picks the first (preferred) option that fits into freeW. Options that are better than the
// one used last time need hyst extra room, so the widget does not flip back and forth.
static int ChooseFit(const int* need, int n, int prevIndex, int freeW, int hyst) {
    for (int i = 0; i < n; i++) {
        const int extra = (i < prevIndex) ? hyst : 0;
        if (need[i] + extra <= freeW) return i;
    }
    return -1;
}

static void RenderAndPlace(HWND tray, const Settings& s) {
    RECT trc = {};
    GetClientRect(tray, &trc);
    const int TW = trc.right, TH = trc.bottom;
    if (TW <= 0 || TH <= 0 || TH > TW) {  // vertical taskbars are not supported
        ShowWindow(g_hwnd, SW_HIDE);
        return;
    }

    const UINT dpi = g_getDpiForWindow ? g_getDpiForWindow(tray) : 96;
    const float dpiScale = (float)dpi / 96.f;
    const bool dark = TaskbarIsDark(s.themeMode);
    ApplyFontSetting(s.fontName);

    // Where the notification area starts.
    int trayLeft = TW - (int)(300.f * dpiScale);
    HWND notify = FindWindowExW(tray, nullptr, L"TrayNotifyWnd", nullptr);
    if (notify) {
        RECT nr = {};
        if (GetWindowRect(notify, &nr)) {
            MapWindowPoints(nullptr, tray, (POINT*)&nr, 2);
            if (nr.left > 0 && nr.left < TW) trayLeft = nr.left;
        }
    }
    const int margin = (int)(10.f * dpiScale);

    // Options, from the preferred look to the most compact one.
    struct Option {
        int skin;
        float factor;
    };
    Option opts[8];
    int nOpts = 0;
    opts[nOpts++] = {s.skin, 1.f};
    const bool fit = s.autoFit != 0 && s.placement == 0;
    if (fit) {
        // Keep the layout the user picked and shrink it first.
        opts[nOpts++] = {s.skin, 0.9f};
        opts[nOpts++] = {s.skin, 0.8f};
        opts[nOpts++] = {s.skin, 0.7f};
        opts[nOpts++] = {s.skin, 0.6f};
        if (s.autoFit == 2 && s.skin != SK_STACK) {  // "smart": allow the narrower layout too
            opts[nOpts++] = {SK_STACK, 1.f};
            opts[nOpts++] = {SK_STACK, 0.85f};
            opts[nOpts++] = {SK_STACK, 0.7f};
        }
    }

    // Room between the last taskbar button and the notification area.
    int chosen = 0;
    const LONG appsRight = InterlockedCompareExchange(&g_appsRightScreen, 0, 0);
    if (fit && appsRight >= 0) {
        POINT p = {appsRight, 0};
        ScreenToClient(tray, &p);
        const int freeW = (trayLeft - margin) - (p.x + (int)(6.f * dpiScale));
        int need[8];
        for (int i = 0; i < nOpts; i++) {
            View tv;
            MakeView(tv, s, opts[i].skin, opts[i].factor, dpiScale, TH, dark);
            Layout tl;
            need[i] = BuildLayout(tv, tl);
        }
        chosen = ChooseFit(need, nOpts, g_fitIndex, freeW, (int)(16.f * dpiScale));
        g_fitIndex = chosen < 0 ? nOpts : chosen;
        static int s_lastLogged = -2;
        if (chosen != s_lastLogged) {  // only log when the choice changes
            s_lastLogged = chosen;
            Wh_Log(L"Auto-fit: free=%d px, apps end at %d, tray starts at %d, chosen option %d of %d "
                   L"(layout %d, scale %d%%)",
                   freeW, (int)p.x, trayLeft, chosen, nOpts, chosen >= 0 ? opts[chosen].skin : -1,
                   chosen >= 0 ? (int)(opts[chosen].factor * 100.f) : 0);
        }
        if (chosen < 0) {  // no room at all: stay out of the way until there is
            ShowWindow(g_hwnd, SW_HIDE);
            g_lastX = -1;
            return;
        }
    } else {
        g_fitIndex = 0;
    }

    View v;
    MakeView(v, s, opts[chosen].skin, opts[chosen].factor, dpiScale, TH, dark);
    Layout L;
    BuildLayout(v, L);
    Canvas canvas;
    RenderWidget(canvas, v, L);
    if (canvas.w <= 0 || canvas.h <= 0) return;

    int x;
    switch (s.placement) {
        case 1: x = margin; break;
        case 2: x = (TW - canvas.w) / 2; break;
        case 3: x = TW - canvas.w - margin; break;
        default: x = trayLeft - canvas.w - margin; break;
    }
    x += (int)((float)s.offsetX * dpiScale);
    int y = (TH - canvas.h) / 2 + (int)((float)s.offsetY * dpiScale);
    if (x > TW - canvas.w) x = TW - canvas.w;
    if (x < 0) x = 0;

    // Only touch the window when something changed, to avoid needless repaints of the taskbar.
    const bool changed = x != g_lastX || y != g_lastY || canvas.w != g_lastW || canvas.h != g_lastH ||
                         !IsWindowVisible(g_hwnd);
    const bool onTop = GetWindow(tray, GW_CHILD) == g_hwnd;
    if (changed || !onTop) {
        SetWindowPos(g_hwnd, HWND_TOP, x, y, canvas.w, canvas.h, SWP_NOACTIVATE | SWP_SHOWWINDOW);
        g_lastX = x;
        g_lastY = y;
        g_lastW = canvas.w;
        g_lastH = canvas.h;
    }

    PushCanvas(g_hwnd, canvas, 255);
}

static void DoTick(bool sample) {
    const Settings s = GetSettings();
    HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr);
    DWORD pid = 0;
    if (tray) GetWindowThreadProcessId(tray, &pid);
    if (!tray || pid != GetCurrentProcessId()) {  // this explorer.exe has no taskbar
        DestroyWidget();
        return;
    }
    if (g_hwnd && (!IsWindow(g_hwnd) || g_hwndParent != tray)) DestroyWidget();
    if (!g_hwnd && !CreateWidget(tray)) return;
    if (s.resetToken != g_lastResetToken) {  // the user asked to clear the saved history
        g_lastResetToken = s.resetToken;
        Wh_SetIntValue(L"resetToken", s.resetToken);
        ResetUsage();
    }
    if (!sample) g_fitIndex = 0;  // settings changed: start the layout choice from scratch
    if (sample) SampleNetwork(s);
    RenderAndPlace(tray, s);
    if (g_tipVisible && g_tipTarget != 0) {  // keep the numbers in the open tooltip live
        UpdateTip(s, false);
        ApplyTipFrame();
    }
}

// ---------------------------------------------------------------- UI thread and entry points

static HANDLE g_thread = nullptr;
static DWORD g_threadId = 0;
static HANDLE g_ready = nullptr;
constexpr UINT kMsgSettings = WM_APP + 1;

static DWORD WINAPI UiThread(LPVOID) {
    MSG msg;
    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);  // create the message queue

    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = WidgetProc;
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.lpszClassName = kWidgetClass;
    g_classRegistered = RegisterClassExW(&wc) != 0;

    g_textDC = CreateCompatibleDC(nullptr);

    LoadUsage();
    g_lastResetToken = Wh_GetIntValue(L"resetToken", GetSettings().resetToken);

    UINT_PTR timer = SetTimer(nullptr, 0, (UINT)GetSettings().updateMs, nullptr);
    SetHoverPeriod(120);
    SetEvent(g_ready);
    DoTick(true);

    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        if (msg.message == WM_TIMER && msg.hwnd == nullptr && msg.wParam == timer) {
            DoTick(true);
            continue;
        }
        if (msg.message == WM_TIMER && msg.hwnd == nullptr && msg.wParam == g_hoverTimer) {
            HoverTick();
            continue;
        }
        if (msg.message == kMsgSettings) {
            if (timer) KillTimer(nullptr, timer);
            timer = SetTimer(nullptr, 0, (UINT)GetSettings().updateMs, nullptr);
            DoTick(false);
            continue;
        }
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    if (timer) KillTimer(nullptr, timer);
    if (g_hoverTimer) KillTimer(nullptr, g_hoverTimer);
    if (g_usageDirty) SaveUsage();
    DestroyTip();
    DestroyWidget();
    for (int i = 0; i < 4; i++) {
        if (g_font[i]) DeleteObject(g_font[i]);
        g_font[i] = nullptr;
    }
    if (g_textDC) DeleteDC(g_textDC);
    g_textDC = nullptr;
    if (g_classRegistered) UnregisterClassW(kWidgetClass, GetModuleHandleW(nullptr));
    return 0;
}

BOOL Wh_ModInit() {
    LoadSettings();

    g_iphlpapi = LoadLibraryExW(L"iphlpapi.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (g_iphlpapi) {
        g_getIfTable = (GetIfTable_t)(void*)GetProcAddress(g_iphlpapi, "GetIfTable");
        g_getBestInterface =
            (GetBestInterface_t)(void*)GetProcAddress(g_iphlpapi, "GetBestInterface");
    }
    if (!g_getIfTable) {
        Wh_Log(L"GetIfTable not found");
        return FALSE;
    }

    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    if (user32) {
        g_getWindowDpiCtx =
            (GetWindowDpiCtx_t)(void*)GetProcAddress(user32, "GetWindowDpiAwarenessContext");
        g_setThreadDpiCtx =
            (DpiCtxFn)(void*)GetProcAddress(user32, "SetThreadDpiAwarenessContext");
        g_getDpiForWindow = (GetDpiForWindow_t)(void*)GetProcAddress(user32, "GetDpiForWindow");
    }

    g_ready = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_thread = CreateThread(nullptr, 0, UiThread, nullptr, 0, &g_threadId);
    if (!g_thread) {
        Wh_Log(L"Failed to create the UI thread");
        return FALSE;
    }
    WaitForSingleObject(g_ready, 3000);

    g_uiaStop = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_uiaThread = CreateThread(nullptr, 0, UiaThread, nullptr, 0, nullptr);
    return TRUE;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    if (g_thread) PostThreadMessageW(g_threadId, kMsgSettings, 0, 0);
}

void Wh_ModUninit() {
    if (g_uiaStop) SetEvent(g_uiaStop);
    if (g_uiaThread) {
        WaitForSingleObject(g_uiaThread, 8000);
        CloseHandle(g_uiaThread);
        g_uiaThread = nullptr;
    }
    if (g_uiaStop) {
        CloseHandle(g_uiaStop);
        g_uiaStop = nullptr;
    }
    if (g_thread) {
        PostThreadMessageW(g_threadId, WM_QUIT, 0, 0);
        WaitForSingleObject(g_thread, 5000);
        CloseHandle(g_thread);
        g_thread = nullptr;
    }
    if (g_ready) {
        CloseHandle(g_ready);
        g_ready = nullptr;
    }
}
