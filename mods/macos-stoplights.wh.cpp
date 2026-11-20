// ==WindhawkMod==
// @id              macos-stoplights
// @name            macOS Stoplights
// @description     Replaces the Windows caption buttons with macOS traffic lights for Windows 11.
// @version         1.0.0
// @author          Iaroslav
// @license         MIT
// @include         *
// @exclude         dwm.exe
// @exclude         winlogon.exe
// @exclude         csrss.exe
// @exclude         lsass.exe
// @exclude         services.exe
// @exclude         smss.exe
// @exclude         wininit.exe
// @exclude         fontdrvhost.exe
// @exclude         svchost.exe
// @exclude         sihost.exe
// @exclude         ctfmon.exe
// @exclude         SearchIndexer.exe
// @exclude         MsMpEng.exe
// @exclude         NisSrv.exe
// @exclude         SecurityHealthService.exe
// @exclude         SecurityHealthSystray.exe
// @exclude         ShellExperienceHost.exe
// @exclude         StartMenuExperienceHost.exe
// @exclude         SearchHost.exe
// @exclude         TextInputHost.exe
// @exclude         LockApp.exe
// @exclude         WidgetService.exe
// @exclude         Widgets.exe
// @exclude         RuntimeBroker.exe
// @exclude         backgroundTaskHost.exe
// @exclude         smartscreen.exe
// @exclude         SystemSettingsBroker.exe
// @architecture    x86
// @architecture    x86-64
// @architecture    arm64
// @compilerOptions -ldwmapi -lgdi32 -luser32
// @github          https://github.com/beliiiaroslav04-tech
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# macOS Stoplights

Replaces Minimize, Maximize and Close with macOS traffic lights.
Yellow minimizes, green zooms, red closes.

An unactivated Windows 11 is fine. The desktop "Activate Windows"
watermark is not a caption button, so this mod leaves it alone.

## Not covered

Chrome, Edge, Steam, Discord, Spotify, VS Code and plenty of other apps draw
their own title bar. Those stay as they are. Snap layouts will not pop out of
the green light, because the light takes the mouse. Alt+F4 and Alt+Space
still work.

## Layouts

**In place** (the default) paints one light in the middle of each Windows
button, large enough to cover the original glyph.

**Packed** pulls the three lights together at the right end of that strip and
paints over the old glyphs. On blurry Mica title bars that paint can show as
a flat patch. Order applies to Packed only.

Packed order `mac` is red, yellow, green from left to right, so the corner
light is green. Switch the order to `windows` if you want the corner to close.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- layout: inplace
  $name: Layout
  $description: '"In place" puts a light on each Windows button. "Packed" groups them like macOS.'
  $options:
    - inplace: In place
    - packed: Packed group
- order: mac
  $name: Packed order
  $description: 'Left to right, packed layout only. "windows" keeps Close at the outer corner.'
  $options:
    - mac: Red, yellow, green
    - windows: Yellow, green, red
- diameter: 0
  $name: Diameter
  $description: Light size in pixels at 100% scale. 0 picks a size that covers the Windows glyph.
- gap: 8
  $name: Gap
  $description: Space between packed lights, in pixels at 100% scale.
- margin: 10
  $name: Margin
  $description: Space between a packed group and the right edge, in pixels at 100% scale.
- symbols: true
  $name: Symbols on hover
  $description: Draw the x, minus and zoom mark when the pointer is over a light.
- grayInactive: true
  $name: Gray when inactive
  $description: Use gray lights on windows that are not in front, like macOS.
- closeColor: '#FF5F57'
  $name: Close
  $description: 'Hex color, #RRGGBB.'
- minColor: '#FEBC2E'
  $name: Minimize
  $description: 'Hex color, #RRGGBB.'
- maxColor: '#28C840'
  $name: Zoom
  $description: 'Hex color, #RRGGBB.'
*/
// ==/WindhawkModSettings==

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <dwmapi.h>

#include <atomic>
#include <cmath>
#include <mutex>
#include <memory>
#include <unordered_map>
#include <vector>

#ifndef DWMWA_USE_IMMERSIVE_DARK_MODE
#define DWMWA_USE_IMMERSIVE_DARK_MODE ((DWMWINDOWATTRIBUTE)20)
#endif
#ifndef DWMWA_CAPTION_COLOR
#define DWMWA_CAPTION_COLOR ((DWMWINDOWATTRIBUTE)35)
#endif
#ifndef DWMWA_COLOR_DEFAULT
#define DWMWA_COLOR_DEFAULT 0xFFFFFFFFu
#endif
#ifndef DWMWA_COLOR_NONE
#define DWMWA_COLOR_NONE 0xFFFFFFFEu
#endif
#ifndef STATE_SYSTEM_OFFSCREEN
#define STATE_SYSTEM_OFFSCREEN 0x00010000
#endif

enum Role { RoleMin = 0, RoleMax = 1, RoleClose = 2, RoleNone = -1 };

struct Config {
    int packed;
    int macOrder;
    int diameter;
    int gap;
    int margin;
    int symbols;
    int grayInactive;
    COLORREF closeC;
    COLORREF minC;
    COLORREF maxC;
};

struct Light {
    int role;
    RECT native;
    float cx;
    float cy;
    float r;
};

struct Pix {
    uint8_t b, g, r, a;
};

struct State {
    HWND target = nullptr;
    HWND overlay = nullptr;
    WNDPROC oldProc = nullptr;
    Light lights[3]{};
    int lightCount = 0;
    RECT plate{0, 0, 0, 0};
    COLORREF plateColor = RGB(243, 243, 243);
    bool plateValid = false;
    DWORD sampleTick = 0;
    int hover = RoleNone;
    int pressed = RoleNone;
    int paintedHover = RoleNone;
    int paintedPressed = RoleNone;
    bool pointerInside = false;
    int appliedGen = -1;
    bool lastActive = false;
    bool lastZoomed = false;
    int lastW = 0;
    int lastH = 0;
};

static Config g_cfg{};
static std::atomic<int> g_cfgGen{0};
static std::atomic<bool> g_unloading{false};
static std::mutex g_mu;
static std::unordered_map<HWND, std::shared_ptr<State>> g_byTarget;
static std::unordered_map<HWND, std::shared_ptr<State>> g_byOverlay;
static std::unordered_map<DWORD, std::vector<HWND>> g_pending;
static UINT g_msgSync = 0;
static decltype(&CreateWindowExW) CreateWindowExW_Original = nullptr;
static HANDLE g_timerThread = nullptr;
static HANDLE g_timerReady = nullptr;
static HWND g_timerHwnd = nullptr;
static std::atomic<DWORD> g_timerTid{0};
static thread_local int g_syncDepth = 0;

static LRESULT CALLBACK TargetProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
static LRESULT CALLBACK OverlayProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
static LRESULT CALLBACK TimerHostProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

static const wchar_t kOverlayClass[] = L"WhMacStoplightsOverlay";
static const wchar_t kTimerClass[] = L"WhMacStoplightsTimer";
static const wchar_t kProp[] = L"WhMacStoplights";

static void FreeSetting(PCWSTR value) {
    if (value) Wh_FreeStringSetting(value);
}

static int Clampi(int v, int lo, int hi) {
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

static float Clampf(float v, float lo, float hi) {
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

static int HexNibble(wchar_t c) {
    if (c >= L'0' && c <= L'9') return c - L'0';
    if (c >= L'a' && c <= L'f') return c - L'a' + 10;
    if (c >= L'A' && c <= L'F') return c - L'A' + 10;
    return -1;
}

static COLORREF ParseColor(PCWSTR text, COLORREF fallback) {
    if (!text || !text[0]) return fallback;
    if (text[0] == L'#') text++;
    int n = 0;
    int v[8];
    for (; text[n] && n < 8; n++) {
        int d = HexNibble(text[n]);
        if (d < 0) return fallback;
        v[n] = d;
    }
    if (n == 6) return RGB((v[0] << 4) | v[1], (v[2] << 4) | v[3], (v[4] << 4) | v[5]);
    if (n == 3) return RGB((v[0] << 4) | v[0], (v[1] << 4) | v[1], (v[2] << 4) | v[2]);
    return fallback;
}

static bool NameIs(PCWSTR value, const wchar_t* name) {
    return value && _wcsicmp(value, name) == 0;
}

static bool ClassIsOurs(LPCWSTR className) {
    if (!className || IS_INTRESOURCE(className)) return false;
    return wcscmp(className, kOverlayClass) == 0 || wcscmp(className, kTimerClass) == 0;
}

static void LoadSettings() {
    Config c{};
    PCWSTR layout = Wh_GetStringSetting(L"layout");
    PCWSTR order = Wh_GetStringSetting(L"order");
    c.packed = NameIs(layout, L"packed") ? 1 : 0;
    c.macOrder = NameIs(order, L"windows") ? 0 : 1;
    FreeSetting(layout);
    FreeSetting(order);

    c.diameter = Clampi(Wh_GetIntSetting(L"diameter"), 0, 64);
    c.gap = Clampi(Wh_GetIntSetting(L"gap"), 0, 48);
    c.margin = Clampi(Wh_GetIntSetting(L"margin"), 0, 64);
    c.symbols = Wh_GetIntSetting(L"symbols") ? 1 : 0;
    c.grayInactive = Wh_GetIntSetting(L"grayInactive") ? 1 : 0;

    PCWSTR closeC = Wh_GetStringSetting(L"closeColor");
    PCWSTR minC = Wh_GetStringSetting(L"minColor");
    PCWSTR maxC = Wh_GetStringSetting(L"maxColor");
    c.closeC = ParseColor(closeC, RGB(255, 95, 87));
    c.minC = ParseColor(minC, RGB(254, 188, 46));
    c.maxC = ParseColor(maxC, RGB(40, 200, 64));
    FreeSetting(closeC);
    FreeSetting(minC);
    FreeSetting(maxC);

    std::lock_guard<std::mutex> lock(g_mu);
    g_cfg = c;
    g_cfgGen.fetch_add(1, std::memory_order_relaxed);
}

static Config GetConfig() {
    std::lock_guard<std::mutex> lock(g_mu);
    return g_cfg;
}

static int DpiOf(HWND hwnd) {
    UINT dpi = GetDpiForWindow(hwnd);
    if (dpi < 96) dpi = 96;
    return (int)dpi;
}

static bool RectLive(const RECT& r) {
    int w = r.right - r.left;
    int h = r.bottom - r.top;
    return w > 4 && h > 4 && w < 800 && h < 400;
}

static bool Cloaked(HWND hwnd) {
    BOOL cloaked = FALSE;
    return SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &cloaked, sizeof(cloaked))) && cloaked;
}

static bool DarkCaption(HWND hwnd) {
    BOOL dark = FALSE;
    return SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &dark, sizeof(dark))) && dark;
}

static COLORREF FallbackPlate(HWND hwnd) {
    return DarkCaption(hwnd) ? RGB(32, 32, 32) : RGB(243, 243, 243);
}

static COLORREF SamplePlate(HWND hwnd, const RECT& buttons) {
    COLORREF cap = DWMWA_COLOR_DEFAULT;
    if (SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_CAPTION_COLOR, &cap, sizeof(cap)))) {
        if (cap != DWMWA_COLOR_DEFAULT && cap != DWMWA_COLOR_NONE && (cap >> 24) == 0) return cap;
    }

    RECT wr{};
    GetWindowRect(hwnd, &wr);
    int y = buttons.top + 3;
    if (y < wr.top + 2) y = wr.top + 2;
    int x = buttons.left - 10;
    if (x < wr.left + 6) x = wr.left + (buttons.left - wr.left) / 2;
    POINT pts[3] = {{x, y}, {x - 8, y}, {(wr.left + buttons.left) / 2, wr.top + 4}};

    HDC dc = GetDC(nullptr);
    if (!dc) return FallbackPlate(hwnd);
    unsigned br = 0, bg = 0, bb = 0, n = 0;
    for (POINT p : pts) {
        HWND top = WindowFromPoint(p);
        if (!top || (top != hwnd && GetAncestor(top, GA_ROOT) != hwnd)) continue;
        COLORREF px = GetPixel(dc, p.x, p.y);
        if (px == CLR_INVALID) continue;
        br += GetRValue(px);
        bg += GetGValue(px);
        bb += GetBValue(px);
        n++;
    }
    ReleaseDC(nullptr, dc);
    if (!n) return FallbackPlate(hwnd);
    return RGB(br / n, bg / n, bb / n);
}

static void Blend(Pix& dst, int r, int g, int b, int a) {
    if (a <= 0) return;
    if (a > 255) a = 255;
    unsigned inv = 255u - (unsigned)a;
    dst.r = (uint8_t)((unsigned)r * (unsigned)a / 255u + dst.r * inv / 255u);
    dst.g = (uint8_t)((unsigned)g * (unsigned)a / 255u + dst.g * inv / 255u);
    dst.b = (uint8_t)((unsigned)b * (unsigned)a / 255u + dst.b * inv / 255u);
    dst.a = (uint8_t)((unsigned)a + dst.a * inv / 255u);
}

static void Shade(COLORREF c, float mul, int& r, int& g, int& b) {
    r = Clampi((int)(GetRValue(c) * mul + 0.5f), 0, 255);
    g = Clampi((int)(GetGValue(c) * mul + 0.5f), 0, 255);
    b = Clampi((int)(GetBValue(c) * mul + 0.5f), 0, 255);
}

static void DrawDisc(Pix* px, int w, int h, float cx, float cy, float radius, int r, int g, int b, int alpha) {
    if (radius <= 0.5f || alpha <= 0) return;
    int x0 = Clampi((int)std::floor(cx - radius - 1.5f), 0, w - 1);
    int y0 = Clampi((int)std::floor(cy - radius - 1.5f), 0, h - 1);
    int x1 = Clampi((int)std::ceil(cx + radius + 1.5f), 0, w - 1);
    int y1 = Clampi((int)std::ceil(cy + radius + 1.5f), 0, h - 1);
    for (int y = y0; y <= y1; y++) {
        for (int x = x0; x <= x1; x++) {
            float dx = (x + 0.5f) - cx;
            float dy = (y + 0.5f) - cy;
            float dist = std::sqrt(dx * dx + dy * dy);
            float cov = radius + 0.45f - dist;
            if (cov <= 0.f) continue;
            if (cov > 1.f) cov = 1.f;
            float t = Clampf(dy / radius, -1.f, 1.f);
            float mul = 1.10f - 0.20f * (t * 0.5f + 0.5f);
            Blend(px[(size_t)y * w + x], Clampi((int)(r * mul), 0, 255), Clampi((int)(g * mul), 0, 255),
                  Clampi((int)(b * mul), 0, 255), (int)(cov * alpha));
        }
    }
}

static float DistToSeg(float px, float py, float x0, float y0, float x1, float y1) {
    float vx = x1 - x0, vy = y1 - y0;
    float c2 = vx * vx + vy * vy;
    float t = c2 > 0.f ? (vx * (px - x0) + vy * (py - y0)) / c2 : 0.f;
    t = Clampf(t, 0.f, 1.f);
    float dx = px - (x0 + t * vx);
    float dy = py - (y0 + t * vy);
    return std::sqrt(dx * dx + dy * dy);
}

static void DrawSegment(Pix* px, int w, int h, float x0, float y0, float x1, float y1, float thickness,
                        float clipCx, float clipCy, float clipR) {
    int xa = Clampi((int)std::floor(std::fmin(x0, x1) - thickness - 1.f), 0, w - 1);
    int ya = Clampi((int)std::floor(std::fmin(y0, y1) - thickness - 1.f), 0, h - 1);
    int xb = Clampi((int)std::ceil(std::fmax(x0, x1) + thickness + 1.f), 0, w - 1);
    int yb = Clampi((int)std::ceil(std::fmax(y0, y1) + thickness + 1.f), 0, h - 1);
    float half = thickness * 0.5f;
    float limit = clipR - 0.6f;
    for (int y = ya; y <= yb; y++) {
        for (int x = xa; x <= xb; x++) {
            float fx = x + 0.5f, fy = y + 0.5f;
            float dx = fx - clipCx, dy = fy - clipCy;
            if (dx * dx + dy * dy > limit * limit) continue;
            float cov = half + 0.45f - DistToSeg(fx, fy, x0, y0, x1, y1);
            if (cov <= 0.f) continue;
            if (cov > 1.f) cov = 1.f;
            Blend(px[(size_t)y * w + x], 20, 20, 20, (int)(cov * 210));
        }
    }
}

static void DrawSymbol(Pix* px, int w, int h, const Light& light, int role, bool zoomed) {
    float s = light.r * 0.34f;
    float t = std::fmax(1.15f, light.r * 0.155f);
    float cx = light.cx, cy = light.cy;
    if (role == RoleClose) {
        DrawSegment(px, w, h, cx - s, cy - s, cx + s, cy + s, t, cx, cy, light.r);
        DrawSegment(px, w, h, cx + s, cy - s, cx - s, cy + s, t, cx, cy, light.r);
    } else if (role == RoleMin) {
        DrawSegment(px, w, h, cx - s, cy, cx + s, cy, t, cx, cy, light.r);
    } else if (zoomed) {
        float a = s * 0.78f;
        float boxes[2][2] = {{cx - a * 0.28f, cy + a * 0.22f}, {cx + a * 0.28f, cy - a * 0.22f}};
        for (auto& box : boxes) {
            float ox = box[0], oy = box[1];
            DrawSegment(px, w, h, ox - a, oy - a, ox + a, oy - a, t * 0.85f, cx, cy, light.r);
            DrawSegment(px, w, h, ox + a, oy - a, ox + a, oy + a, t * 0.85f, cx, cy, light.r);
            DrawSegment(px, w, h, ox + a, oy + a, ox - a, oy + a, t * 0.85f, cx, cy, light.r);
            DrawSegment(px, w, h, ox - a, oy + a, ox - a, oy - a, t * 0.85f, cx, cy, light.r);
        }
    } else {
        DrawSegment(px, w, h, cx - s, cy, cx + s, cy, t, cx, cy, light.r);
        DrawSegment(px, w, h, cx, cy - s, cx, cy + s, t, cx, cy, light.r);
    }
}

static bool InsideRoundCorner(const RECT& window, int dpi, int sx, int sy) {
    float rad = (float)MulDiv(8, dpi, 96);
    if (rad < 2.f) return true;
    float fx = sx + 0.5f, fy = sy + 0.5f;
    if (fx < (float)window.left + rad && fy < (float)window.top + rad) {
        float dx = fx - ((float)window.left + rad);
        float dy = fy - ((float)window.top + rad);
        if (dx * dx + dy * dy > rad * rad) return false;
    }
    if (fx > (float)window.right - rad && fy < (float)window.top + rad) {
        float dx = fx - ((float)window.right - rad);
        float dy = fy - ((float)window.top + rad);
        if (dx * dx + dy * dy > rad * rad) return false;
    }
    return true;
}

static COLORREF RoleColor(const Config& cfg, int role) {
    if (role == RoleClose) return cfg.closeC;
    if (role == RoleMin) return cfg.minC;
    return cfg.maxC;
}

static void PaintOverlay(State* st, const Config& cfg, bool active, bool zoomed, int dpi) {
    int w = st->plate.right - st->plate.left;
    int h = st->plate.bottom - st->plate.top;
    if (w <= 0 || h <= 0 || w > 2000 || h > 500 || !st->overlay) return;

    std::vector<Pix> bits((size_t)w * (size_t)h);
    RECT window{};
    GetWindowRect(st->target, &window);
    if (cfg.packed) {
        for (int y = 0; y < h; y++) {
            for (int x = 0; x < w; x++) {
                if (!InsideRoundCorner(window, dpi, st->plate.left + x, st->plate.top + y)) continue;
                Pix& p = bits[(size_t)y * w + x];
                p.r = GetRValue(st->plateColor);
                p.g = GetGValue(st->plateColor);
                p.b = GetBValue(st->plateColor);
                p.a = 255;
            }
        }
    }

    bool showColor = active || !cfg.grayInactive || st->hover != RoleNone;
    COLORREF gray = DarkCaption(st->target) ? RGB(92, 92, 92) : RGB(214, 214, 214);
    for (int i = 0; i < st->lightCount; i++) {
        const Light& light = st->lights[i];
        bool hot = st->hover == light.role;
        COLORREF base = (showColor || hot) ? RoleColor(cfg, light.role) : gray;
        float mul = st->pressed == light.role ? 0.78f : (hot ? 1.06f : 1.f);
        int r, g, b, sr, sg, sb;
        Shade(base, mul, r, g, b);
        Shade(base, mul * 0.72f, sr, sg, sb);
        DrawDisc(bits.data(), w, h, light.cx, light.cy + 0.8f, light.r + 0.3f, 0, 0, 0, 36);
        DrawDisc(bits.data(), w, h, light.cx, light.cy, light.r, sr, sg, sb, 255);
        DrawDisc(bits.data(), w, h, light.cx, light.cy, light.r - 1.05f, r, g, b, 255);
        if (cfg.symbols && hot) DrawSymbol(bits.data(), w, h, light, light.role, zoomed);
    }

    BITMAPINFO bmi{};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = w;
    bmi.bmiHeader.biHeight = -h;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    HDC screen = GetDC(nullptr);
    HDC mem = screen ? CreateCompatibleDC(screen) : nullptr;
    void* view = nullptr;
    HBITMAP dib = mem ? CreateDIBSection(mem, &bmi, DIB_RGB_COLORS, &view, nullptr, 0) : nullptr;
    if (dib && view) {
        memcpy(view, bits.data(), bits.size() * sizeof(Pix));
        HGDIOBJ old = SelectObject(mem, dib);
        POINT dst{st->plate.left, st->plate.top};
        SIZE size{w, h};
        POINT src{0, 0};
        BLENDFUNCTION blend{AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
        UpdateLayeredWindow(st->overlay, screen, &dst, &size, mem, &src, 0, &blend, ULW_ALPHA);
        SelectObject(mem, old);
        st->lastW = w;
        st->lastH = h;
        st->lastActive = active;
        st->lastZoomed = zoomed;
        st->paintedHover = st->hover;
        st->paintedPressed = st->pressed;
        st->appliedGen = g_cfgGen.load(std::memory_order_relaxed);
    }
    if (dib) DeleteObject(dib);
    if (mem) DeleteDC(mem);
    if (screen) ReleaseDC(nullptr, screen);
}

static int HitRole(const State* st, int x, int y) {
    float fx = (float)x, fy = (float)y;
    for (int i = 0; i < st->lightCount; i++) {
        float dx = fx - st->lights[i].cx;
        float dy = fy - st->lights[i].cy;
        float reach = st->lights[i].r + 1.5f;
        if (dx * dx + dy * dy <= reach * reach) return st->lights[i].role;
    }
    POINT screen{st->plate.left + x, st->plate.top + y};
    for (int i = 0; i < st->lightCount; i++) {
        if (PtInRect(&st->lights[i].native, screen)) return st->lights[i].role;
    }
    return RoleNone;
}

static void DoRole(HWND target, int role) {
    UINT cmd = SC_CLOSE;
    if (role == RoleMin) cmd = SC_MINIMIZE;
    else if (role == RoleMax) cmd = IsZoomed(target) ? SC_RESTORE : SC_MAXIMIZE;
    SetForegroundWindow(target);
    PostMessageW(target, WM_SYSCOMMAND, cmd, 0);
}

static std::shared_ptr<State> FindTarget(HWND hwnd) {
    std::lock_guard<std::mutex> lock(g_mu);
    auto it = g_byTarget.find(hwnd);
    return it == g_byTarget.end() ? nullptr : it->second;
}

static std::shared_ptr<State> FindOverlay(HWND hwnd) {
    std::lock_guard<std::mutex> lock(g_mu);
    auto it = g_byOverlay.find(hwnd);
    return it == g_byOverlay.end() ? nullptr : it->second;
}

static void HideOverlay(State* st) {
    if (st->overlay) ShowWindow(st->overlay, SW_HIDE);
}

static bool CollectButtons(HWND hwnd, RECT out[3], bool present[3]) {
    if (GetWindowThreadProcessId(hwnd, nullptr) != GetCurrentThreadId()) return false;
    TITLEBARINFOEX info{};
    info.cbSize = sizeof(info);
    SendMessageW(hwnd, WM_GETTITLEBARINFOEX, 0, (LPARAM)&info);

    LONG style = GetWindowLongW(hwnd, GWL_STYLE);
    const int index[3] = {2, 3, 5};
    const LONG bits[3] = {WS_MINIMIZEBOX, WS_MAXIMIZEBOX, WS_SYSMENU};
    RECT window{};
    GetWindowRect(hwnd, &window);
    bool any = false;
    for (int i = 0; i < 3; i++) {
        present[i] = false;
        out[i] = info.rgrect[index[i]];
        if (!(style & bits[i])) continue;
        if (info.rgstate[index[i]] & STATE_SYSTEM_OFFSCREEN) continue;
        RECT cut{};
        if (!IntersectRect(&cut, &out[i], &window) || !RectLive(cut)) continue;
        out[i] = cut;
        present[i] = true;
        any = true;
    }
    return any;
}

static void LayoutLights(State* st, const Config& cfg, const RECT native[3], const bool present[3], int dpi) {
    st->lightCount = 0;
    int order[3] = {RoleMin, RoleMax, RoleClose};
    if (cfg.packed && cfg.macOrder) {
        order[0] = RoleClose;
        order[1] = RoleMin;
        order[2] = RoleMax;
    }

    int alive[3];
    int aliveCount = 0;
    RECT uni{LONG_MAX, LONG_MAX, LONG_MIN, LONG_MIN};
    for (int i = 0; i < 3; i++) {
        int role = cfg.packed ? order[i] : i;
        if (!present[role]) continue;
        alive[aliveCount++] = role;
        const RECT& r = native[role];
        if (r.left < uni.left) uni.left = r.left;
        if (r.top < uni.top) uni.top = r.top;
        if (r.right > uni.right) uni.right = r.right;
        if (r.bottom > uni.bottom) uni.bottom = r.bottom;
    }
    if (!aliveCount) {
        st->plate = RECT{0, 0, 0, 0};
        return;
    }
    InflateRect(&uni, 1, 1);
    st->plate = uni;

    if (!cfg.packed) {
        for (int i = 0; i < aliveCount; i++) {
            int role = alive[i];
            RECT r = native[role];
            int side = r.right - r.left;
            if (r.bottom - r.top < side) side = r.bottom - r.top;
            int d = cfg.diameter > 0 ? MulDiv(cfg.diameter, dpi, 96) : side * 68 / 100;
            if (d < 12) d = 12;
            if (d > side - 2) d = side - 2;
            Light& light = st->lights[st->lightCount++];
            light.role = role;
            light.native = r;
            light.cx = ((r.left + r.right) * 0.5f) - (float)uni.left;
            light.cy = ((r.top + r.bottom) * 0.5f) - (float)uni.top;
            light.r = d * 0.5f;
        }
        return;
    }

    int d = cfg.diameter > 0 ? MulDiv(cfg.diameter, dpi, 96) : MulDiv(14, dpi, 96);
    int gap = MulDiv(cfg.gap, dpi, 96);
    int margin = MulDiv(cfg.margin, dpi, 96);
    int room = (uni.right - uni.left) - margin;
    int cluster = aliveCount * d + (aliveCount - 1) * gap;
    if (cluster > room) {
        d = (room - (aliveCount - 1) * gap) / aliveCount;
        if (d < 8) d = 8;
    }
    float x = (float)(uni.right - uni.left) - (float)margin - d * 0.5f;
    float cy = (uni.bottom - uni.top) * 0.5f;
    for (int i = aliveCount - 1; i >= 0; i--) {
        Light& light = st->lights[st->lightCount++];
        light.role = alive[i];
        light.native = native[alive[i]];
        light.cx = x;
        light.cy = cy;
        light.r = d * 0.5f;
        x -= (float)(d + gap);
    }
}

static void EnsureOverlay(const std::shared_ptr<State>& st) {
    if (st->overlay && IsWindow(st->overlay)) return;
    HWND created = CreateWindowExW(WS_EX_LAYERED | WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW, kOverlayClass, L"",
                                   WS_POPUP, 0, 0, 0, 0, st->target, nullptr, GetModuleHandleW(nullptr), nullptr);
    if (!created) return;
    st->overlay = created;
    SetWindowLongPtrW(created, GWLP_USERDATA, (LONG_PTR)st->target);
    std::lock_guard<std::mutex> lock(g_mu);
    g_byOverlay[created] = st;
}

static void Sync(HWND hwnd, bool force) {
    if (g_unloading.load(std::memory_order_relaxed) || g_syncDepth) return;
    ++g_syncDepth;

    auto st = FindTarget(hwnd);
    if (!st || !st->oldProc) {
        --g_syncDepth;
        return;
    }

    bool show = IsWindowVisible(hwnd) && !IsIconic(hwnd) && !Cloaked(hwnd);
    LONG style = GetWindowLongW(hwnd, GWL_STYLE);
    if ((style & WS_CHILD) || !(style & WS_CAPTION)) show = false;

    RECT native[3]{};
    bool present[3]{};
    if (show) show = CollectButtons(hwnd, native, present);
    if (!show) {
        HideOverlay(st.get());
        st->lightCount = 0;
        --g_syncDepth;
        return;
    }

    Config cfg = GetConfig();
    int dpi = DpiOf(hwnd);
    LayoutLights(st.get(), cfg, native, present, dpi);
    if (!st->lightCount) {
        HideOverlay(st.get());
        --g_syncDepth;
        return;
    }

    bool active = GetForegroundWindow() == hwnd;
    bool zoomed = IsZoomed(hwnd) != FALSE;
    if (cfg.packed) {
        DWORD now = GetTickCount();
        if (!st->plateValid || active != st->lastActive || (DWORD)(now - st->sampleTick) > 800) {
            COLORREF sampled = SamplePlate(hwnd, st->plate);
            if (sampled != st->plateColor) force = true;
            st->plateColor = sampled;
            st->plateValid = true;
            st->sampleTick = now;
        }
    }

    EnsureOverlay(st);
    if (!st->overlay) {
        --g_syncDepth;
        return;
    }

    int w = st->plate.right - st->plate.left;
    int h = st->plate.bottom - st->plate.top;
    int gen = g_cfgGen.load(std::memory_order_relaxed);
    bool visualSame = !force && gen == st->appliedGen && w == st->lastW && h == st->lastH &&
                      active == st->lastActive && zoomed == st->lastZoomed && st->hover == st->paintedHover &&
                      st->pressed == st->paintedPressed;
    if (!visualSame) PaintOverlay(st.get(), cfg, active, zoomed, dpi);
    SetWindowPos(st->overlay, nullptr, st->plate.left, st->plate.top, w, h,
                 SWP_NOACTIVATE | SWP_NOZORDER | SWP_SHOWWINDOW);
    --g_syncDepth;
}

static void ForgetOverlay(HWND overlay) {
    std::lock_guard<std::mutex> lock(g_mu);
    g_byOverlay.erase(overlay);
}

static void Detach(HWND hwnd) {
    std::shared_ptr<State> st;
    {
        std::lock_guard<std::mutex> lock(g_mu);
        auto it = g_byTarget.find(hwnd);
        if (it == g_byTarget.end()) return;
        st = it->second;
        g_byTarget.erase(it);
        if (st->overlay) g_byOverlay.erase(st->overlay);
    }
    RemovePropW(hwnd, kProp);
    if (st->oldProc && IsWindow(hwnd) && (WNDPROC)GetWindowLongPtrW(hwnd, GWLP_WNDPROC) == TargetProc) {
        SetWindowLongPtrW(hwnd, GWLP_WNDPROC, (LONG_PTR)st->oldProc);
    }
    st->oldProc = nullptr;
    if (g_msgSync) {
        MSG junk;
        while (PeekMessageW(&junk, hwnd, g_msgSync, g_msgSync, PM_REMOVE)) {
        }
    }
    HWND overlay = st->overlay;
    st->overlay = nullptr;
    if (!overlay || !IsWindow(overlay)) return;
    if (GetWindowThreadProcessId(overlay, nullptr) == GetCurrentThreadId()) DestroyWindow(overlay);
    else SendMessageTimeoutW(overlay, WM_CLOSE, 0, 0, SMTO_ABORTIFHUNG | SMTO_BLOCK, 150, nullptr);
}

static LRESULT CALLBACK TargetProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    auto st = FindTarget(hwnd);
    WNDPROC old = st ? st->oldProc : nullptr;
    if (g_unloading.load(std::memory_order_relaxed) && msg != WM_DESTROY) {
        return old ? CallWindowProcW(old, hwnd, msg, wp, lp) : DefWindowProcW(hwnd, msg, wp, lp);
    }
    if (msg == WM_DESTROY) {
        if (st) Detach(hwnd);
        if (old) return CallWindowProcW(old, hwnd, msg, wp, lp);
        return DefWindowProcW(hwnd, msg, wp, lp);
    }
    if (g_msgSync && msg == g_msgSync) {
        Sync(hwnd, wp != 0);
        return 0;
    }

    LRESULT result = old ? CallWindowProcW(old, hwnd, msg, wp, lp) : DefWindowProcW(hwnd, msg, wp, lp);
    if (!g_unloading.load(std::memory_order_relaxed)) {
        switch (msg) {
        case WM_WINDOWPOSCHANGED:
        case WM_SHOWWINDOW:
        case WM_ACTIVATE:
        case WM_NCACTIVATE:
        case WM_STYLECHANGED:
        case WM_DPICHANGED:
        case WM_EXITSIZEMOVE:
            Sync(hwnd, false);
            break;
        default:
            break;
        }
    }
    return result;
}

static LRESULT CALLBACK OverlayProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    if (msg == WM_MOUSEACTIVATE) return MA_NOACTIVATE;
    if (msg == WM_CLOSE) {
        DestroyWindow(hwnd);
        return 0;
    }
    if (msg == WM_DESTROY) {
        ForgetOverlay(hwnd);
        return 0;
    }
    auto st = FindOverlay(hwnd);
    if (!st || g_unloading.load(std::memory_order_relaxed)) return DefWindowProcW(hwnd, msg, wp, lp);

    switch (msg) {
    case WM_MOUSEMOVE: {
        int role = HitRole(st.get(), (int)(short)LOWORD(lp), (int)(short)HIWORD(lp));
        if (!st->pointerInside) {
            TRACKMOUSEEVENT tme{sizeof(tme), TME_LEAVE, hwnd, 0};
            TrackMouseEvent(&tme);
            st->pointerInside = true;
        }
        if (role != st->hover) {
            st->hover = role;
            Sync(st->target, true);
        }
        return 0;
    }
    case WM_MOUSELEAVE:
        st->pointerInside = false;
        st->hover = RoleNone;
        st->pressed = RoleNone;
        Sync(st->target, true);
        return 0;
    case WM_LBUTTONDOWN: {
        int role = HitRole(st.get(), (int)(short)LOWORD(lp), (int)(short)HIWORD(lp));
        st->pressed = role;
        st->hover = role;
        SetCapture(hwnd);
        Sync(st->target, true);
        return 0;
    }
    case WM_LBUTTONUP: {
        int role = HitRole(st.get(), (int)(short)LOWORD(lp), (int)(short)HIWORD(lp));
        int pressed = st->pressed;
        st->pressed = RoleNone;
        if (GetCapture() == hwnd) ReleaseCapture();
        Sync(st->target, true);
        if (pressed != RoleNone && pressed == role) DoRole(st->target, role);
        return 0;
    }
    case WM_CAPTURECHANGED:
        if ((HWND)lp != hwnd && st->pressed != RoleNone) {
            st->pressed = RoleNone;
            Sync(st->target, true);
        }
        return 0;
    default:
        break;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

static bool WindowClassIsOurs(HWND hwnd) {
    wchar_t cls[96];
    return GetClassNameW(hwnd, cls, 96) && ClassIsOurs(cls);
}

static void AttachNow(HWND hwnd) {
    if (g_unloading.load(std::memory_order_relaxed) || !IsWindow(hwnd) || GetPropW(hwnd, kProp)) return;
    if (WindowClassIsOurs(hwnd)) return;
    LONG style = GetWindowLongW(hwnd, GWL_STYLE);
    if ((style & WS_CHILD) || !(style & WS_CAPTION) || !(style & WS_SYSMENU)) return;

    auto st = std::make_shared<State>();
    st->target = hwnd;
    {
        std::lock_guard<std::mutex> lock(g_mu);
        if (g_byTarget.count(hwnd)) return;
        g_byTarget.emplace(hwnd, st);
    }
    SetLastError(0);
    WNDPROC old = (WNDPROC)SetWindowLongPtrW(hwnd, GWLP_WNDPROC, (LONG_PTR)TargetProc);
    if (!old && GetLastError()) {
        std::lock_guard<std::mutex> lock(g_mu);
        g_byTarget.erase(hwnd);
        return;
    }
    st->oldProc = old;
    SetPropW(hwnd, kProp, (HANDLE)1);
    if (g_msgSync) PostMessageW(hwnd, g_msgSync, 0, 0);
}

static LRESULT CALLBACK CallWndHook(int code, WPARAM wp, LPARAM lp) {
    if (code == HC_ACTION && !g_unloading.load(std::memory_order_relaxed)) {
        std::vector<HWND> batch;
        DWORD tid = GetCurrentThreadId();
        {
            std::lock_guard<std::mutex> lock(g_mu);
            auto it = g_pending.find(tid);
            if (it != g_pending.end()) {
                batch.swap(it->second);
                g_pending.erase(it);
            }
        }
        for (HWND hwnd : batch) AttachNow(hwnd);
    }
    return CallNextHookEx(nullptr, code, wp, lp);
}

static BOOL CALLBACK EnumExisting(HWND hwnd, LPARAM lp) {
    auto* poke = reinterpret_cast<std::unordered_map<DWORD, HWND>*>(lp);
    DWORD pid = 0;
    DWORD tid = GetWindowThreadProcessId(hwnd, &pid);
    if (pid != GetCurrentProcessId() || !tid || IsHungAppWindow(hwnd)) return TRUE;
    {
        std::lock_guard<std::mutex> lock(g_mu);
        g_pending[tid].push_back(hwnd);
    }
    poke->emplace(tid, hwnd);
    return TRUE;
}

static void AttachExisting() {
    std::unordered_map<DWORD, HWND> poke;
    EnumWindows(EnumExisting, (LPARAM)&poke);
    for (auto& item : poke) {
        if (item.first == GetCurrentThreadId()) {
            std::vector<HWND> batch;
            {
                std::lock_guard<std::mutex> lock(g_mu);
                batch.swap(g_pending[item.first]);
                g_pending.erase(item.first);
            }
            for (HWND hwnd : batch) AttachNow(hwnd);
            continue;
        }
        HHOOK hook = SetWindowsHookExW(WH_CALLWNDPROC, CallWndHook, nullptr, item.first);
        if (!hook) continue;
        SendMessageTimeoutW(item.second, WM_NULL, 0, 0, SMTO_ABORTIFHUNG | SMTO_BLOCK, 250, nullptr);
        UnhookWindowsHookEx(hook);
    }
}

static void DetachThreadWindows() {
    std::vector<HWND> mine;
    DWORD tid = GetCurrentThreadId();
    {
        std::lock_guard<std::mutex> lock(g_mu);
        for (auto& kv : g_byTarget) {
            DWORD windowTid = IsWindow(kv.first) ? GetWindowThreadProcessId(kv.first, nullptr) : tid;
            if (windowTid == tid) mine.push_back(kv.first);
        }
    }
    for (HWND hwnd : mine) Detach(hwnd);
}

static LRESULT CALLBACK UnloadHook(int code, WPARAM wp, LPARAM lp) {
    if (code == HC_ACTION) DetachThreadWindows();
    return CallNextHookEx(nullptr, code, wp, lp);
}

static void DetachAll() {
    g_unloading.store(true, std::memory_order_relaxed);
    std::vector<HWND> targets;
    {
        std::lock_guard<std::mutex> lock(g_mu);
        for (auto& item : g_byTarget) targets.push_back(item.first);
        g_pending.clear();
    }
    std::unordered_map<DWORD, HWND> poke;
    for (HWND hwnd : targets) {
        if (!IsWindow(hwnd)) {
            Detach(hwnd);
            continue;
        }
        poke.emplace(GetWindowThreadProcessId(hwnd, nullptr), hwnd);
    }
    for (auto& item : poke) {
        if (item.first == GetCurrentThreadId()) continue;
        HHOOK hook = SetWindowsHookExW(WH_CALLWNDPROC, UnloadHook, nullptr, item.first);
        if (!hook) continue;
        SendMessageTimeoutW(item.second, WM_NULL, 0, 0, SMTO_ABORTIFHUNG | SMTO_BLOCK, 250, nullptr);
        UnhookWindowsHookEx(hook);
    }
    targets.clear();
    {
        std::lock_guard<std::mutex> lock(g_mu);
        for (auto& item : g_byTarget) targets.push_back(item.first);
    }
    for (HWND hwnd : targets) Detach(hwnd);
}

static void TickAll() {
    if (g_unloading.load(std::memory_order_relaxed) || !g_msgSync) return;
    std::vector<HWND> targets;
    {
        std::lock_guard<std::mutex> lock(g_mu);
        for (auto& item : g_byTarget) targets.push_back(item.first);
    }
    for (HWND hwnd : targets) {
        if (IsWindow(hwnd)) PostMessageW(hwnd, g_msgSync, 0, 0);
    }
}

static LRESULT CALLBACK TimerHostProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    if (msg == WM_TIMER) {
        TickAll();
        return 0;
    }
    if (msg == WM_CLOSE) {
        KillTimer(hwnd, 1);
        DestroyWindow(hwnd);
        return 0;
    }
    if (msg == WM_DESTROY) {
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

static DWORD WINAPI TimerThread(LPVOID) {
    MSG kick{};
    PeekMessageW(&kick, nullptr, WM_USER, WM_USER, PM_NOREMOVE);
    g_timerTid.store(GetCurrentThreadId(), std::memory_order_relaxed);
    g_timerHwnd = CreateWindowExW(0, kTimerClass, L"", 0, 0, 0, 0, 0, HWND_MESSAGE, nullptr,
                                  GetModuleHandleW(nullptr), nullptr);
    if (g_timerHwnd) SetTimer(g_timerHwnd, 1, 400, nullptr);
    if (g_timerReady) SetEvent(g_timerReady);
    if (!g_timerHwnd) return 0;
    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    return 0;
}

static void StartTimer() {
    if (g_timerThread) return;
    g_timerReady = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_timerThread = CreateThread(nullptr, 0, TimerThread, nullptr, 0, nullptr);
}

static void StopTimer() {
    if (g_timerReady) WaitForSingleObject(g_timerReady, 2000);
    if (g_timerHwnd && IsWindow(g_timerHwnd)) PostMessageW(g_timerHwnd, WM_CLOSE, 0, 0);
    else if (g_timerTid.load()) PostThreadMessageW(g_timerTid.load(), WM_QUIT, 0, 0);
    if (g_timerThread) {
        WaitForSingleObject(g_timerThread, 3000);
        CloseHandle(g_timerThread);
        g_timerThread = nullptr;
    }
    if (g_timerReady) {
        CloseHandle(g_timerReady);
        g_timerReady = nullptr;
    }
    g_timerHwnd = nullptr;
}

static HWND WINAPI CreateWindowExW_Hook(DWORD exStyle, LPCWSTR className, LPCWSTR windowName, DWORD style, int x,
                                        int y, int w, int h, HWND parent, HMENU menu, HINSTANCE instance, LPVOID param) {
    HWND hwnd = CreateWindowExW_Original(exStyle, className, windowName, style, x, y, w, h, parent, menu, instance, param);
    if (!hwnd || g_unloading.load(std::memory_order_relaxed) || ClassIsOurs(className)) return hwnd;
    AttachNow(hwnd);
    return hwnd;
}

static bool RegisterClasses() {
    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(wc);
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.lpfnWndProc = OverlayProc;
    wc.lpszClassName = kOverlayClass;
    SetLastError(0);
    ATOM overlay = RegisterClassExW(&wc);
    DWORD overlayErr = GetLastError();

    wc.lpfnWndProc = TimerHostProc;
    wc.lpszClassName = kTimerClass;
    wc.hCursor = nullptr;
    SetLastError(0);
    ATOM timer = RegisterClassExW(&wc);
    DWORD timerErr = GetLastError();
    return (overlay || overlayErr == ERROR_CLASS_ALREADY_EXISTS) && (timer || timerErr == ERROR_CLASS_ALREADY_EXISTS);
}

BOOL Wh_ModInit() {
    LoadSettings();
    g_msgSync = RegisterWindowMessageW(L"WhMacStoplights.Sync");
    if (!g_msgSync || !RegisterClasses()) return FALSE;
    if (!Wh_SetFunctionHook((void*)CreateWindowExW, (void*)CreateWindowExW_Hook, (void**)&CreateWindowExW_Original)) {
        Wh_Log(L"Failed to hook CreateWindowExW");
        return FALSE;
    }
    return TRUE;
}

void Wh_ModAfterInit() {
    AttachExisting();
    StartTimer();
}

void Wh_ModBeforeUninit() {
    g_unloading.store(true, std::memory_order_relaxed);
    StopTimer();
    DetachAll();
    HINSTANCE instance = GetModuleHandleW(nullptr);
    UnregisterClassW(kOverlayClass, instance);
    UnregisterClassW(kTimerClass, instance);
}

void Wh_ModUninit() {}

void Wh_ModSettingsChanged() {
    LoadSettings();
    std::vector<HWND> targets;
    {
        std::lock_guard<std::mutex> lock(g_mu);
        for (auto& item : g_byTarget) targets.push_back(item.first);
    }
    for (HWND hwnd : targets) {
        if (IsWindow(hwnd)) PostMessageW(hwnd, g_msgSync, 1, 0);
    }
}
