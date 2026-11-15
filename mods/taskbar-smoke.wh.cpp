// ==WindhawkMod==
// @id              taskbar-smoke
// @name            Taskbar Smoke
// @description     Soft, living smoke around the Windows 11 taskbar pills. Real fluid simulation: it rises, swirls, drifts over the edge and reacts to the mouse. Click-through overlay, follows auto-hide, hides in fullscreen.
// @version         1.2.0
// @author          Rubicus
// @github          https://github.com/xxsoulxx
// @include         windhawk.exe
// @compilerOptions -ld2d1 -lole32 -loleaut32 -luuid -lshell32 -luser32 -lgdi32 -ldwmapi
// ==/WindhawkMod==

// clang-format off
// ==WindhawkModReadme==
/*
# Taskbar Smoke
Draws a transparent, click-through overlay with animated dark smoke around the
Windows 11 taskbar.

![Taskbar Smoke](https://raw.githubusercontent.com/xxsoulxx/taskbar-smoke/main/taskbar-smoke.gif)

* **Five smoke styles:** a real-time fluid simulation (the smoke rises, swirls
and drifts over the edge), a classic layered texture smoke, and three tuned
variants of the fluid smoke.
* **Reacts to the mouse:** moving the cursor across the taskbar pushes the smoke
aside; it flows back on its own.
* **Eight colors** (violet, pink, light blue, ice blue, green, gold, orange,
red), plus the Windows accent color or a slow rainbow cycle. An optional
**second color** blends along each pill, from left to right.
* **Opacity** in seven steps (17% to 150%), **density**, **speed**, **tone**
(dark smoke or light glow), **halo thickness** and **mouse reach**.
* Optional small dark clouds slowly rising from the taskbar.
* Follows auto-hide, hides in fullscreen apps, settings apply instantly.

Works best with a dark, pill-shaped taskbar theme (e.g. Windows 11 Taskbar
Styler); the smoke follows the taskbar's button groups. Runs in its own process
(tool mod), Explorer is not modified.

Limitations: primary taskbar only, designed for a bottom-docked taskbar.
*/
// ==/WindhawkModReadme==
// clang-format on

// clang-format off
// ==WindhawkModSettings==
/*
- color: violet
  $name: Smoke color
  $options:
  - violet: Violet
  - pink: Pink
  - lightblue: Light blue
  - iceblue: Ice blue
  - green: Green
  - gold: Gold
  - orange: Orange
  - red: Red
  - accent: Windows accent color
  - rainbow: Slow rainbow cycle
- color2: none
  $name: Second color
  $description: Blends the smoke from the first color (left end of each pill) to this one (right end).
  $options:
  - none: None
  - violet: Violet
  - pink: Pink
  - lightblue: Light blue
  - iceblue: Ice blue
  - green: Green
  - gold: Gold
  - orange: Orange
  - red: Red
- opacity: normal
  $name: Opacity
  $description: Normal (default).
  $options:
  - faint: Faint (17%)
  - whisper: Whisper (30%)
  - subtle: Subtle (50%)
  - light: Light (70%)
  - normal: Normal (100%)
  - strong: Strong (125%)
  - full: Full (150%)
- density: normal
  $name: Smoke density
  $options:
  - sparse: Sparse
  - normal: Normal
  - dense: Dense
- speed: normal
  $name: Smoke speed
  $options:
  - calm: Calm
  - normal: Normal
  - lively: Lively
- tone: smoke
  $name: Tone
  $options:
  - smoke: Smoke (dark)
  - glow: Glow (light)
- thickness: normal
  $name: Halo thickness
  $description: How far the smoke reaches out from the taskbar pills.
  $options:
  - slim: Slim
  - normal: Normal
  - thick: Thick
  - extra: Extra
- style: fluid
  $name: Smoke style
  $options:
  - fluid: Fluid smoke (carries the texture look)
  - texture: Classic texture smoke
  - fluidFirst: First realistic fluid smoke
  - fluidCarried: Fluid smoke, texture carried by the flow
  - fluidSoft: Fluid smoke, softer
- mouse: gentle
  $name: Mouse reaction
  $options:
  - "off": "Off"
  - gentle: Gentle
  - strong: Strong
- reach: normal
  $name: Mouse reach
  $options:
  - small: Small
  - normal: Normal
  - large: Large
- clouds: true
  $name: Small dark clouds rising from the taskbar
*/
// ==/WindhawkModSettings==
// clang-format on

#include <d2d1.h>
#include <dwmapi.h>
#include <shellapi.h>
#include <uiautomation.h>
#include <windows.h>
#include <wrl/client.h>
#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <initializer_list>
#include <string>
#include <vector>
using Microsoft::WRL::ComPtr;
static const CLSID kClsidUIA = {
    0xff48dba4,
    0x60ef,
    0x4201,
    {0xaa, 0x87, 0x54, 0x10, 0x3e, 0xef, 0x59, 0x4e}};  // CLSID_CUIAutomation
static const IID kIidUIA = {
    0x30cbe57d,
    0xd9d0,
    0x452a,
    {0xab, 0x13, 0x7a, 0xc5, 0xac, 0x48, 0x25, 0xee}};  // IID_IUIAutomation

namespace {
constexpr int N = 192;
std::vector<float> g_alpha[3];
float g_mittel[3] = {
    .5f, .5f, .5f};  // Mean value per noise texture (force without drift)
float Hash(int x, int y, int sd) {
    unsigned h = (unsigned)x * 374761393u + (unsigned)y * 668265263u +
                 (unsigned)sd * 2246822519u;
    h = (h ^ (h >> 13)) * 1274126177u;
    return (float)((h ^ (h >> 16)) & 0xFFFFFFu) / 16777215.0f;
}
float VNoise(float x, float y, int p, int sd) {
    const int xi = (int)std::floor(x), yi = (int)std::floor(y);
    const float xf = x - xi, yf = y - yi, u = xf * xf * (3 - 2 * xf),
                v = yf * yf * (3 - 2 * yf);
    auto md = [p](int q) { return ((q % p) + p) % p; };
    const float a = Hash(md(xi), md(yi), sd), b = Hash(md(xi + 1), md(yi), sd),
                c = Hash(md(xi), md(yi + 1), sd),
                d = Hash(md(xi + 1), md(yi + 1), sd);
    return a + (b - a) * u + (c - a) * v + (a - b - c + d) * u * v;
}
float Fbm(float x, float y, int p, int sd) {
    float t = 0, amp = 0.5f;
    for (int o = 0; o < 5; ++o) {
        const int fi = 1 << o;
        t += amp * VNoise(x * fi, y * fi, p * fi, sd + o);
        amp *= 0.5f;
    }
    return t;
}
void Rauschen() {
    const int basis[3] = {4, 3, 6}, saat[3] = {11, 53, 97};
    for (int k = 0; k < 3; ++k) {
        g_alpha[k].assign((size_t)N * N, 0.f);
        for (int j = 0; j < N; ++j)
            for (int i = 0; i < N; ++i) {
                const int bs = basis[k], sd = saat[k];
                const float x = (float)i / N * bs, y = (float)j / N * bs;
                const float qx = Fbm(x, y, bs, sd),
                            qy = Fbm(x + 5.2f, y + 1.3f, bs, sd + 7);
                const float w = Fbm(x + 3.2f * qx, y + 3.2f * qy, bs, sd + 13);
                float al = std::max(0.f, std::min(1.f, (w - 0.32f) / 0.5f));
                g_alpha[k][(size_t)j * N + i] = al * al * (3 - 2 * al);
            }
    }
}
struct Pille {
    float l, t, r, b;
};
std::vector<Pille> g_pillen;   // used by Tick (screen pixels)
std::vector<Pille> g_pillenQ;  // written by the UIA thread (under g_lock)
CRITICAL_SECTION g_lock;
volatile LONG g_uiaBereit = 0;
std::atomic<bool> g_uiaStop{false};
std::atomic<bool> g_overlaySichtbar{false};
HANDLE g_uiaThread = nullptr;
float g_offT = 9, g_offB = 9;  // Gap between pill body and taskbar window (from
                               // UIA, measured without hover)
float g_blende = 0;            // smooth fade in/out
HWND g_hwnd = nullptr;
ComPtr<IUIAutomation> g_ua;
ComPtr<IUIAutomationElement> g_frame;
int MonitorBottom(HWND tray) {
    HMONITOR m = MonitorFromWindow(tray, MONITOR_DEFAULTTOPRIMARY);
    MONITORINFO mi{sizeof(mi)};
    GetMonitorInfoW(m, &mi);
    return mi.rcMonitor.bottom;
}
// Read pills from the taskbar's UI Automation tree (approx. once per second)
bool PillenLesen() {
    HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (!tray || !g_ua)
        return false;
    ComPtr<IUIAutomationElement> root;
    if (FAILED(g_ua->ElementFromHandle(tray, &root)) || !root)
        return false;
    ComPtr<IUIAutomationCacheRequest> cr;
    if (FAILED(g_ua->CreateCacheRequest(&cr)) || !cr)
        return false;
    cr->AddProperty(UIA_ControlTypePropertyId);
    cr->AddProperty(UIA_AutomationIdPropertyId);
    cr->AddProperty(UIA_BoundingRectanglePropertyId);
    VARIANT vt;
    VariantInit(&vt);
    vt.vt = VT_I4;
    vt.lVal = UIA_ButtonControlTypeId;
    VARIANT vi;
    VariantInit(&vi);
    vi.vt = VT_BSTR;
    vi.bstrVal = SysAllocString(L"TaskbarFrame");
    ComPtr<IUIAutomationCondition> cb, cf, tc;
    g_ua->CreatePropertyCondition(UIA_ControlTypePropertyId, vt, &cb);
    g_ua->CreatePropertyCondition(UIA_AutomationIdPropertyId, vi, &cf);
    VariantClear(&vi);
    if (!cb || !cf ||
        FAILED(g_ua->CreateOrCondition(cb.Get(), cf.Get(), &tc)) || !tc)
        return false;
    ComPtr<IUIAutomationElementArray> arr;
    if (FAILED(root->FindAllBuildCache(TreeScope_Descendants, tc.Get(),
                                       cr.Get(), &arr)) ||
        !arr)
        return false;
    int n = 0;
    arr->get_Length(&n);
    float wl = 1e9f, wr = -1, ml = 1e9f, mr = -1, tl = 1e9f, tr = -1,
          top = 1e9f, bot = -1;
    bool hatW = false;
    for (int i = 0; i < n; ++i) {
        ComPtr<IUIAutomationElement> e;
        if (FAILED(arr->GetElement(i, &e)) || !e)
            continue;
        CONTROLTYPEID ct = 0;
        e->get_CachedControlType(&ct);
        BSTR id = nullptr;
        e->get_CachedAutomationId(&id);
        std::wstring s = id ? id : L"";
        if (id)
            SysFreeString(id);
        RECT r{};
        e->get_CachedBoundingRectangle(&r);
        if (r.right - r.left <= 0)
            continue;
        if (s == L"TaskbarFrame") {
            g_frame = e;
            continue;
        }
        if (ct != UIA_ButtonControlTypeId)
            continue;
        if (s == L"WidgetsButton") {
            wl = (float)r.left;
            wr = (float)r.right;
            hatW = true;
            top = std::min(top, (float)r.top);
            bot = std::max(bot, (float)r.bottom);
        } else if (s == L"StartButton" || s == L"TaskViewButton" ||
                   s == L"SearchButton" || s.rfind(L"Appid:", 0) == 0) {
            ml = std::min(ml, (float)r.left);
            mr = std::max(mr, (float)r.right);
            top = std::min(top, (float)r.top);
            bot = std::max(bot, (float)r.bottom);
        } else if (s == L"SystemTrayIcon" || s == L"NotifyItemIcon") {
            tl = std::min(tl, (float)r.left);
            tr = std::max(tr, (float)r.right);
            top = std::min(top, (float)r.top);
            bot = std::max(bot, (float)r.bottom);
        }
    }
    if (bot < 0)
        return false;
    std::vector<Pille> p;
    const float
        t = 0,
        b = 0;  // y is taken per frame from the TaskbarFrame (auto-hide slide)
    if (hatW)
        p.push_back({wl, t, wr, b});
    if (mr > 0)
        p.push_back({ml - 1, t, mr + 48, b});
    if (tr > 0)
        p.push_back({tl - 16, t, tr + 22, b});
    if (p.empty())
        return false;
    {
        RECT wr{};
        if (GetWindowRect(tray, &wr))
            for (auto& q : p)
                if (q.l < wr.left - 4 || q.r > wr.right + 4 || q.r - q.l < 40)
                    return false;
    }  // implausible -> keep previous values
    EnterCriticalSection(&g_lock);
    g_pillenQ = p;
    LeaveCriticalSection(&g_lock);
    InterlockedExchange(&g_uiaBereit, 1);
    if (g_frame) {
        RECT fr{}, wr{};
        if (SUCCEEDED(g_frame->get_CachedBoundingRectangle(&fr)) &&
            GetWindowRect(tray, &wr) && fr.bottom > fr.top) {
            const float oT = (float)(fr.top - wr.top),
                        oB = (float)(wr.bottom - fr.bottom);
            if (oT >= 0 && oT <= 30 && oB >= 0 && oB <= 30) {
                EnterCriticalSection(&g_lock);
                g_offT = oT;
                g_offB = oB;
                LeaveCriticalSection(&g_lock);
            }
        }
    }
    return true;
}
// Smoke colors (0 violet 1 orange 2 red 3 pink 4 light blue 5 ice blue 6 green
// 7 gold)
constexpr int kFarben = 8;
static const float kRauchRgb[kFarben][3] = {
    {88.f, 52.f, 160.f},  {168.f, 84.f, 22.f}, {160.f, 30.f, 36.f},
    {200.f, 60.f, 120.f}, {34.f, 96.f, 150.f}, {70.f, 140.f, 185.f},
    {34.f, 128.f, 78.f},  {150.f, 108.f, 10.f}};
constexpr int kSlots = kFarben + 1;      // slot 8 = accent / rainbow
float g_eigen[3] = {88.f, 52.f, 160.f};  // color of slot 8
bool g_glanz = false;   // tone glow (light) instead of smoke (dark)
float g_opF = 1.f;      // opacity factor (1 = normal)
float g_dichteF = 1.f;  // density / dye gain
float g_tempoF = 1.f;   // simulation time scale
float g_reichF = 1.f;   // mouse reach (radius factor)
float g_rr = 1.f;       // reach squared (area factor in the solvers)
float g_dickeF = 1.f;   // halo thickness factor
float g_flMX = 30.f,
      g_flMY = 34.f;  // simulation margins (px), scaled by thickness
float g_dkTop = .982f, g_dkSide = .94f;  // dye decay per step
int g_farbModus = 0;                     // 0 named color, 1 accent, 2 rainbow
int g_farbe2 = -1;  // second color (index into kRauchRgb) or -1
struct Satz {
    const void* owner = nullptr;
    bool glanz = false;
    float eigen[3] = {-1.f, -1.f, -1.f};
    ComPtr<ID2D1Bitmap> bmp[kSlots][3];
    ComPtr<ID2D1BitmapBrush> br[kSlots][3];
} g_satz;
void BasisFarbe(int ti, float* o) {
    const float* q =
        ti >= kFarben ? g_eigen : kRauchRgb[std::clamp(ti, 0, kFarben - 1)];
    for (int i = 0; i < 3; ++i)
        o[i] = q[i];
}
void ToneRgb(float* c) {  // glow = lighter variant of the same color
    if (g_glanz)
        for (int i = 0; i < 3; ++i)
            c[i] = std::min(255.f, c[i] * 1.35f + 55.f);
}
int g_ti = 0;  // current smoke tint: 0 violet, 1 warning orange, 2 danger red
volatile LONG g_statusTi = 0;  // tint chosen in the settings
bool SlotBauen(ID2D1RenderTarget* rt, int ti) {
    float c[3];
    BasisFarbe(ti, c);
    ToneRgb(c);
    std::vector<uint32_t> px((size_t)N * N);
    for (int k = 0; k < 3; ++k) {
        for (size_t q = 0; q < px.size(); ++q) {
            const float al = g_alpha[k][q];
            px[q] = ((uint32_t)(al * 255 + .5f) << 24) |
                    ((uint32_t)(c[0] * al + .5f) << 16) |
                    ((uint32_t)(c[1] * al + .5f) << 8) |
                    (uint32_t)(c[2] * al + .5f);
        }
        if (FAILED(rt->CreateBitmap(
                D2D1::SizeU(N, N), px.data(), N * 4,
                D2D1::BitmapProperties(D2D1::PixelFormat(
                    DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)),
                &g_satz.bmp[ti][k])))
            return false;
        if (FAILED(rt->CreateBitmapBrush(
                g_satz.bmp[ti][k].Get(),
                D2D1::BitmapBrushProperties(
                    D2D1_EXTEND_MODE_WRAP, D2D1_EXTEND_MODE_WRAP,
                    D2D1_BITMAP_INTERPOLATION_MODE_LINEAR),
                &g_satz.br[ti][k])))
            return false;
    }
    return true;
}
bool Bereit(ID2D1RenderTarget* rt) {
    if (g_satz.owner != (const void*)rt || g_satz.glanz != g_glanz) {
        for (auto& r : g_satz.br)
            for (auto& b : r)
                b.Reset();
        for (auto& r : g_satz.bmp)
            for (auto& b : r)
                b.Reset();
        g_satz.owner = rt;
        g_satz.glanz = g_glanz;
        g_satz.eigen[0] = -1.f;
    }
    for (int ti = 0; ti < kFarben; ++ti)
        if (!g_satz.br[ti][2] && !SlotBauen(rt, ti))
            return false;
    if (!g_satz.br[kFarben][2] ||
        std::memcmp(g_satz.eigen, g_eigen, sizeof(g_eigen))) {
        if (!SlotBauen(rt, kFarben))
            return false;
        std::memcpy(g_satz.eigen, g_eigen, sizeof(g_eigen));
    }
    return true;
}
// color of slot 8 (Windows accent or slow rainbow), quantized so the
// textures are rebuilt only in small steps
void EigenfarbeAktualisieren() {
    if (g_farbModus == 0)
        return;
    float c[3] = {88.f, 52.f, 160.f};
    const double now = GetTickCount64() * 0.001;
    if (g_farbModus == 1) {
        static double last = -10;
        static float acc[3] = {88.f, 52.f, 160.f};
        if (now - last > 2.0) {
            last = now;
            DWORD col = 0;
            BOOL opaque = FALSE;
            if (SUCCEEDED(DwmGetColorizationColor(&col, &opaque))) {
                float rgb[3] = {(float)((col >> 16) & 255),
                                (float)((col >> 8) & 255), (float)(col & 255)};
                const float mx = std::max(rgb[0], std::max(rgb[1], rgb[2]));
                if (mx >= 1.f)
                    for (int i = 0; i < 3; ++i)
                        acc[i] = rgb[i] * 170.f / mx;
            }
        }
        for (int i = 0; i < 3; ++i)
            c[i] = acc[i];
    } else {  // slow hue cycle, about 90 s per round
        const float h = (float)std::fmod(now / 90.0, 1.0) * 6.f;
        const int k = (int)h;
        const float f = h - k, v = 170.f, sat = .7f;
        const float pq = v * (1 - sat), qq = v * (1 - sat * f),
                    tt = v * (1 - sat * (1 - f));
        const float t6[6][3] = {{v, tt, pq}, {qq, v, pq}, {pq, v, tt},
                                {pq, qq, v}, {tt, pq, v}, {v, pq, qq}};
        std::memcpy(c, t6[k % 6], sizeof(c));
    }
    for (int i = 0; i < 3; ++i)
        g_eigen[i] = std::round(c[i] / 4.f) * 4.f;
}
void Schicht(int k,
             float left,
             float top,
             float t,
             float op,
             int phase = 0) {  // Flow (billows/rises) instead of gliding
    static const float sc[3] = {1.15f, .8f, 1.6f}, vx[3] = {3, -4, 2},
                       vy[3] = {-6, -4, -8}, per[3] = {15, 11, 19},
                       ph0[3] = {0, .33f, .66f}, rot0[3] = {0, 7, -6},
                       ox[3] = {0, 40, 80}, oy[3] = {0, 20, 10};
    const float sk = 1.35f;
    float u = std::fmod(t / per[k] + .5f * phase + ph0[k], 1.f);
    if (u < 0)
        u += 1;
    const float a = 1 - std::fabs(2 * u - 1), d = (u - .5f) * per[k];
    const float ang =
        rot0[k] + (phase ? 11.f : 0.f) + std::sin(t * .05f + k * 2.f) * 4;
    g_satz.br[g_ti][k]->SetOpacity(std::min(1.f, op * a));
    g_satz.br[g_ti][k]->SetTransform(
        D2D1::Matrix3x2F::Scale(sc[k] * sk, sc[k] * sk) *
        D2D1::Matrix3x2F::Rotation(ang) *
        D2D1::Matrix3x2F::Translation(
            left + ox[k] + vx[k] * d + (phase ? 97.f : 0.f),
            top + oy[k] + vy[k] * d + (phase ? 53.f : 0.f)));
}
ID2D1Factory* g_fab = nullptr;
ComPtr<ID2D1DCRenderTarget> g_rt;
HDC g_memdc = nullptr;
HBITMAP g_dib = nullptr;
void* g_bits = nullptr;
int g_bw = 0, g_bh = 0;
bool Ziel(int w, int h) {
    if (g_dib && g_bw == w && g_bh == h)
        return true;
    if (!g_memdc)
        g_memdc = CreateCompatibleDC(nullptr);
    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
    bi.bmiHeader.biWidth = w;
    bi.bmiHeader.biHeight = -h;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    HBITMAP dib =
        CreateDIBSection(g_memdc, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!dib)
        return false;
    SelectObject(g_memdc, dib);
    if (g_dib)
        DeleteObject(g_dib);
    g_dib = dib;
    g_bits = bits;
    g_bw = w;
    g_bh = h;
    return true;
}
// Fast box blur (row-wise sliding sums, no strided column access)
void BoxBlur(uint32_t* px, int w, int h, int r) {
    if (r < 1 || w < 4 || h < 4)
        return;
    const int d = 2 * r + 1;
    // horizontal, per row
    for (int y = 0; y < h; ++y) {
        uint32_t* row = px + (size_t)y * w;
        static std::vector<uint32_t> cp;
        cp.assign(row, row + w);
        int s0 = 0, s1 = 0, s2 = 0, s3 = 0;
        for (int j = -r; j <= r; ++j) {
            const uint32_t v = cp[std::clamp(j, 0, w - 1)];
            s0 += v & 255;
            s1 += (v >> 8) & 255;
            s2 += (v >> 16) & 255;
            s3 += v >> 24;
        }
        for (int x = 0; x < w; ++x) {
            row[x] = (uint32_t)(s0 / d) | ((uint32_t)(s1 / d) << 8) |
                     ((uint32_t)(s2 / d) << 16) | ((uint32_t)(s3 / d) << 24);
            const uint32_t a = cp[std::min(x + r + 1, w - 1)],
                           b = cp[std::max(x - r, 0)];
            s0 += (int)(a & 255) - (int)(b & 255);
            s1 += (int)((a >> 8) & 255) - (int)((b >> 8) & 255);
            s2 += (int)((a >> 16) & 255) - (int)((b >> 16) & 255);
            s3 += (int)(a >> 24) - (int)(b >> 24);
        }
    }
    // vertical: per-column sums, row by row
    static std::vector<uint32_t> src;
    src.assign(px, px + (size_t)w * h);
    static std::vector<int> cs;
    cs.assign((size_t)w * 4, 0);
    for (int j = -r; j <= r; ++j) {
        const uint32_t* rw = &src[(size_t)std::clamp(j, 0, h - 1) * w];
        for (int x = 0; x < w; ++x) {
            const uint32_t v = rw[x];
            cs[x * 4] += v & 255;
            cs[x * 4 + 1] += (v >> 8) & 255;
            cs[x * 4 + 2] += (v >> 16) & 255;
            cs[x * 4 + 3] += v >> 24;
        }
    }
    for (int y = 0; y < h; ++y) {
        uint32_t* out = px + (size_t)y * w;
        const uint32_t* ra = &src[(size_t)std::min(y + r + 1, h - 1) * w];
        const uint32_t* rb = &src[(size_t)std::max(y - r, 0) * w];
        for (int x = 0; x < w; ++x) {
            out[x] = (uint32_t)(cs[x * 4] / d) |
                     ((uint32_t)(cs[x * 4 + 1] / d) << 8) |
                     ((uint32_t)(cs[x * 4 + 2] / d) << 16) |
                     ((uint32_t)(cs[x * 4 + 3] / d) << 24);
            const uint32_t a = ra[x], b = rb[x];
            cs[x * 4] += (int)(a & 255) - (int)(b & 255);
            cs[x * 4 + 1] += (int)((a >> 8) & 255) - (int)((b >> 8) & 255);
            cs[x * 4 + 2] += (int)((a >> 16) & 255) - (int)((b >> 16) & 255);
            cs[x * 4 + 3] += (int)(a >> 24) - (int)(b >> 24);
        }
    }
}
// Tiny dark clouds as particles: union of small circles, black fill, violet
// glow
struct Wolke {
    ComPtr<ID2D1Geometry> form;
};
Wolke g_wk[4];
bool g_wkBereit = false;
ComPtr<ID2D1Geometry> Vereinen(const std::vector<D2D1_ELLIPSE>& el) {
    ComPtr<ID2D1Geometry> akt;
    for (size_t i = 0; i < el.size(); ++i) {
        ComPtr<ID2D1EllipseGeometry> e;
        g_fab->CreateEllipseGeometry(el[i], &e);
        if (!e)
            return nullptr;
        if (!akt) {
            akt = e;
            continue;
        }
        ComPtr<ID2D1PathGeometry> pg;
        g_fab->CreatePathGeometry(&pg);
        ComPtr<ID2D1GeometrySink> sk;
        if (!pg || FAILED(pg->Open(&sk)))
            return nullptr;
        akt->CombineWithGeometry(e.Get(), D2D1_COMBINE_MODE_UNION, nullptr,
                                 0.1f, sk.Get());
        sk->Close();
        akt = pg;
    }
    return akt;
}
void WolkenBauen() {
    if (g_wkBereit || !g_fab)
        return;
    for (int v = 0; v < 4; ++v) {
        std::vector<D2D1_ELLIPSE> el;
        el.push_back(D2D1::Ellipse(D2D1::Point2F(0, 0), .62f, .6f));
        const int n = 6 + (v % 3);
        for (int k = 0; k < n; ++k) {
            const float ang = 6.2832f * k / n + v * .7f,
                        d = .62f + .08f * ((k * 5 + v * 3) % 4),
                        r = .36f + .07f * ((k * 3 + v) % 4);
            el.push_back(D2D1::Ellipse(
                D2D1::Point2F(std::cos(ang) * d, std::sin(ang) * d * .86f), r,
                r));
        }
        g_wk[v].form = Vereinen(el);
    }
    g_wkBereit = true;
}

// ---- Fluid simulation (stable fluids, Jos Stam), smoke as dye ----
constexpr float FL_C = 5.f;  // cell size px
float& FL_MX = g_flMX;       // margin left/right (scaled by thickness)
float& FL_MY = g_flMY;       // margin top/bottom
struct Fluid {
    int W = 0, H = 0;
    float seed = 0;
    std::vector<float> u, v, u0, v0, d, d0, p, dv, ox, oy, ox0, oy0, al;
    std::vector<uint32_t> px;
    ComPtr<ID2D1Bitmap> bmp;
    const void* owner = nullptr;
    void Init(int w, int h, float sd) {
        W = w;
        H = h;
        seed = sd;
        const size_t n = (size_t)w * h;
        for (auto* a :
             {&u, &v, &u0, &v0, &d, &d0, &p, &dv, &ox, &oy, &ox0, &oy0, &al})
            a->assign(n, 0.f);
        px.clear();
        bmp.Reset();
    }
};
std::vector<Fluid> g_fl;
inline float Quelle(float x,
                    float y,
                    float t,
                    int k = 0) {  // Noise field, bilinear (nearest-pixel caused
                                  // jitter), slowly drifting
    float fx = x * .55f + t * 1.2f, fy = y * .7f + t * 2.f;
    fx -= std::floor(fx / N) * N;
    fy -= std::floor(fy / N) * N;
    const int x0 = (int)fx, y0 = (int)fy, x1 = (x0 + 1) % N, y1 = (y0 + 1) % N;
    const float ax = fx - x0, ay = fy - y0;
    const float* T = g_alpha[k].data();
    return (T[y0 * N + x0] * (1 - ax) + T[y0 * N + x1] * ax) * (1 - ay) +
           (T[y1 * N + x0] * (1 - ax) + T[y1 * N + x1] * ax) * ay;
}
void FlAdv(Fluid& F,
           std::vector<float>& dst,
           const std::vector<float>& src,
           float dt) {
    const int W = F.W, H = F.H;
    for (int y = 0; y < H; ++y)
        for (int x = 0; x < W; ++x) {
            const int i = x + y * W;
            float px = x - dt * F.u[i], py = y - dt * F.v[i];
            px = std::clamp(px, 0.f, W - 1.001f);
            py = std::clamp(py, 0.f, H - 1.001f);
            const int x0 = (int)px, y0 = (int)py, j = x0 + y0 * W;
            const float fx = px - x0, fy = py - y0;
            dst[i] = (src[j] * (1 - fx) + src[j + 1] * fx) * (1 - fy) +
                     (src[j + W] * (1 - fx) + src[j + W + 1] * fx) * fy;
        }
}
void MittelBerechnen() {
    static bool ok = false;
    if (ok)
        return;
    for (int k = 0; k < 3; ++k) {
        double sm = 0;
        for (float a : g_alpha[k])
            sm += a;
        if (!g_alpha[k].empty())
            g_mittel[k] = (float)(sm / g_alpha[k].size());
    }
    ok = true;
}
std::atomic<bool> g_einstellungNeu{true};
float g_mausF = 1.f;
bool g_partikel = true;
int g_modus = 0;  // mode: subtle 1, strong 2, off 0
int StringSettingIndex(PCWSTR name,
                       std::initializer_list<PCWSTR> values,
                       int fallback) {
    PCWSTR s = Wh_GetStringSetting(name);
    int result = fallback, i = 0;
    if (s) {
        for (PCWSTR v : values) {
            if (!_wcsicmp(s, v))
                result = i;
            ++i;
        }
        Wh_FreeStringSetting(s);
    }
    return result;
}
void EffekteLesen() {  // Windhawk settings (at start and after a change)
    if (!g_einstellungNeu.exchange(false))
        return;
    const int m =
        StringSettingIndex(L"mouse", {L"off", L"gentle", L"strong"}, 1);
    g_mausF = m == 0 ? 0.f : m == 2 ? 2.f : 1.f;
    {  // appearance settings (unknown / missing values fall back to the defaults)
        static const float opac[] = {.17f, .30f, .50f, .70f, 1.f, 1.25f, 1.5f};
        g_opF =
            opac[StringSettingIndex(L"opacity",
                                    {L"faint", L"whisper", L"subtle", L"light",
                                     L"normal", L"strong", L"full"},
                                    4)];
        static const float dens[] = {.6f, 1.f, 1.5f};
        g_dichteF = dens[StringSettingIndex(
            L"density", {L"sparse", L"normal", L"dense"}, 1)];
        static const float spd[] = {.6f, 1.f, 1.6f};
        g_tempoF = spd[StringSettingIndex(L"speed",
                                          {L"calm", L"normal", L"lively"}, 1)];
        g_glanz = StringSettingIndex(L"tone", {L"smoke", L"glow"}, 0) == 1;
        static const float rch[] = {.7f, 1.f, 1.4f};
        g_reichF = rch[StringSettingIndex(L"reach",
                                          {L"small", L"normal", L"large"}, 1)];
        g_rr = g_reichF * g_reichF;
        static const float dik[] = {.7f, 1.f, 1.4f, 1.9f};
        g_dickeF = dik[StringSettingIndex(
            L"thickness", {L"slim", L"normal", L"thick", L"extra"}, 1)];
        g_flMX = 30.f * g_dickeF;
        g_flMY = 34.f * g_dickeF;
        g_dkTop = std::pow(.982f, g_tempoF / g_dickeF);
        g_dkSide = std::pow(.94f, g_tempoF / g_dickeF);
        g_farbe2 =
            StringSettingIndex(L"color2",
                               {L"none", L"violet", L"orange", L"red", L"pink",
                                L"lightblue", L"iceblue", L"green", L"gold"},
                               0) -
            1;  // same order as kRauchRgb
    }
    {
        static const wchar_t* namen[kFarben] = {
            L"violet",    L"orange",  L"red",   L"pink",
            L"lightblue", L"iceblue", L"green", L"gold"};
        PCWSTR f = Wh_GetStringSetting(L"color");
        int ti = 0;
        g_farbModus = 0;
        if (f) {
            for (int q = 0; q < kFarben; ++q)
                if (!_wcsicmp(f, namen[q]))
                    ti = q;
            if (!_wcsicmp(f, L"accent")) {
                g_farbModus = 1;
                ti = kFarben;
            } else if (!_wcsicmp(f, L"rainbow")) {
                g_farbModus = 2;
                ti = kFarben;
            }
            Wh_FreeStringSetting(f);
        }
        InterlockedExchange(&g_statusTi, ti);
    }
    g_partikel = Wh_GetIntSetting(L"clouds") != 0;
    g_modus = StringSettingIndex(
        L"style",
        {L"fluid", L"texture", L"fluidFirst", L"fluidCarried", L"fluidSoft"},
        0);
}
void FluidSchritt(Fluid& F,
                  float dt,
                  float t,
                  float mx,
                  float my,
                  float mvx,
                  float mvy,
                  float ix0,
                  float iy0,
                  float ix1,
                  float iy1) {
    MittelBerechnen();
    const int W = F.W, H = F.H;
    auto& u = F.u;
    auto& v = F.v;
    auto& d = F.d;
    mvx = std::clamp(mvx, -8.f, 8.f);
    mvy = std::clamp(mvy, -8.f, 8.f);
    for (int y = 1; y < H - 1; ++y)
        for (int x = 1; x < W - 1; ++x) {
            const int i = x + y * W;
            v[i] -= dt * 7.f * d[i];  // buoyancy
            const float n1 = Quelle(x * 1.6f + 17 + F.seed, y * 1.6f, t * .35f,
                                    1) -
                             g_mittel[1],
                        n2 = Quelle(x * 1.4f + F.seed, y * 1.4f + 53, t * .3f,
                                    2) -
                             g_mittel[2];  // subtract the mean, otherwise a
                                           // constant drift appears
            u[i] += dt * 26.f * n1;
            v[i] += dt * 16.f * n2;  // irregular turbulence
            const float ddx = x - mx, ddy = y - my, r2 = ddx * ddx + ddy * ddy;
            if (g_mausF > 0.f && r2 < 130.f * g_mausF * g_rr) {
                const float g =
                    std::exp(-r2 / (26.f * g_mausF * g_rr)) * g_mausF;
                u[i] += g * (mvx * 5.f + ddx * .55f);
                v[i] += g * (mvy * 5.f + ddy * .55f);
                d[i] *= 1.f - .16f * std::exp(-r2 / (22.f * g_rr)) * g_mausF;
            }  // Mouse drags and pushes the smoke, clearing the icons
            u[i] *= .985f;
            v[i] *= .985f;
        }
    // Vorticity confinement
    auto& w = F.p;
    for (int y = 1; y < H - 1; ++y)
        for (int x = 1; x < W - 1; ++x) {
            const int i = x + y * W;
            w[i] = .5f * ((v[i + 1] - v[i - 1]) - (u[i + W] - u[i - W]));
        }
    for (int y = 2; y < H - 2; ++y)
        for (int x = 2; x < W - 2; ++x) {
            const int i = x + y * W;
            const float gx = .5f * (std::fabs(w[i + 1]) - std::fabs(w[i - 1])),
                        gy = .5f * (std::fabs(w[i + W]) - std::fabs(w[i - W])),
                        l = std::sqrt(gx * gx + gy * gy) + 1e-5f;
            u[i] += dt * 2.2f * (gy / l) * w[i];
            v[i] -= dt * 2.2f * (gx / l) * w[i];
        }
    F.u0 = u;
    F.v0 = v;
    FlAdv(F, u, F.u0, dt * 8.f);
    FlAdv(F, v, F.v0, dt * 8.f);
    auto& pp = F.p;
    auto& dv = F.dv;
    for (int y = 1; y < H - 1; ++y)
        for (int x = 1; x < W - 1; ++x) {
            const int i = x + y * W;
            dv[i] = -.5f * (u[i + 1] - u[i - 1] + v[i + W] - v[i - W]);
            pp[i] = 0;
        }
    for (int k = 0; k < 10; ++k)
        for (int y = 1; y < H - 1; ++y)
            for (int x = 1; x < W - 1; ++x) {
                const int i = x + y * W;
                pp[i] =
                    (dv[i] + pp[i - 1] + pp[i + 1] + pp[i - W] + pp[i + W]) *
                    .25f;
            }
    for (int y = 1; y < H - 1; ++y)
        for (int x = 1; x < W - 1; ++x) {
            const int i = x + y * W;
            u[i] -= .5f * (pp[i + 1] - pp[i - 1]);
            v[i] -= .5f * (pp[i + W] - pp[i - W]);
        }
    F.d0 = d;
    FlAdv(F, d, F.d0, dt * 8.f);
    // Displacement field (cells): the smoke texture is actually carried by the
    // flow
    F.ox0 = F.ox;
    F.oy0 = F.oy;
    FlAdv(F, F.ox, F.ox0, dt * 8.f);
    FlAdv(F, F.oy, F.oy0, dt * 8.f);
    for (size_t i = 0; i < F.ox.size(); ++i) {
        F.ox[i] = std::clamp((F.ox[i] + u[i] * dt * 8.f) * .994f, -10.f, 10.f);
        F.oy[i] = std::clamp((F.oy[i] + v[i] * dt * 8.f) * .994f, -10.f, 10.f);
    }
    for (int y = 0; y < H; ++y)
        for (int x = 0; x < W; ++x) {
            const int i = x + y * W;
            const bool innen = x >= ix0 && x < ix1 && y >= iy0 && y < iy1;
            if (innen) {
                const float ziel =
                    (.12f + .85f * Quelle(x * 1.6f + F.seed * 2, y * 1.6f, t)) *
                    g_dichteF;
                d[i] += (ziel - d[i]) * .035f;
            }  // thinner at rest; replenishes
            else
                d[i] *=
                    (y < iy0)
                        ? g_dkTop
                        : g_dkSide;  // rises visibly at the top, fades quickly
                                     // at the sides/bottom (no double rounding)
        }
}
void FluidMalen(Fluid& F, ID2D1RenderTarget* rt, D2D1_RECT_F ziel, int ti) {
    float cT[3];
    BasisFarbe(ti, cT);
    ToneRgb(cT);
    const float* c = cT;
    const float hell[3] = {std::min(255.f, c[0] * 1.55f + 30),
                           std::min(255.f, c[1] * 1.5f + 18),
                           std::min(255.f, c[2] * 1.55f + 30)},
                dun[3] = {c[0] * .5f, c[1] * .5f, c[2] * .5f};
    const int W = F.W, H = F.H;
    const float iy0 = FL_MY / FL_C, iy1 = H - FL_MY / FL_C,
                ih = std::max(1.f, iy1 - iy0);
    // 1) Cell alpha (density, capsule fade-out, calm center)
    for (int y = 0; y < H; ++y)
        for (int x = 0; x < W; ++x) {
            const int i = x + y * W;
            const float a = std::min(1.f, F.d[i]);
            const float ix0c = FL_MX / FL_C, ix1c = W - FL_MX / FL_C,
                        rr = ih * .5f, cyc = iy0 + rr;
            const float qx = std::max(0.f,
                                      std::max(ix0c + rr - x, x - (ix1c - rr))),
                        qy = std::fabs(y - cyc);
            const float dist = std::max(0.f, std::sqrt(qx * qx + qy * qy) - rr);
            const float reich = ((y < cyc) ? 6.f : 3.f) * g_dickeF;
            float ef = std::clamp(1.f - dist / reich, 0.f, 1.f);
            ef = ef * ef * (3.f - 2.f * ef);
            float mask = 1.f;
            if (y >= iy0 && y < iy1) {
                const float v = (y - iy0) / ih;
                mask = .28f + .72f * std::pow(std::fabs(v - .5f) * 2.f,
                                              1.3f); /* keep icons clear */
            }
            F.al[i] = std::min(1.f, a * 2.2f) * ef * mask;
        }
    // 2) Pixels (2 px): texture smoke, texture coordinates offset by the
    // advected displacement field
    constexpr int R = 2;
    const int PW = (int)std::ceil(W * FL_C / R),
              PH = (int)std::ceil(H * FL_C / R);
    if ((int)F.px.size() != PW * PH) {
        F.px.assign((size_t)PW * PH, 0);
        F.bmp.Reset();
    }
    const float t =
        (float)std::fmod(GetTickCount64() * 0.001 * g_tempoF, 3600.0);
    struct Lg {
        int k;
        float sc, vx, vy, op;
    };
    static const Lg L[3] = {{0, 1.55f, 3.f, -6.f, .95f},
                            {1, 1.08f, -4.f, -4.f, .75f},
                            {2, 2.16f, 2.f, -8.f, .6f}};
    auto bil = [&](const std::vector<float>& f, float cx, float cy) {
        cx = std::clamp(cx, 0.f, W - 1.001f);
        cy = std::clamp(cy, 0.f, H - 1.001f);
        const int x0 = (int)cx, y0 = (int)cy, j = x0 + y0 * W;
        const float fx = cx - x0, fy = cy - y0;
        return (f[j] * (1 - fx) + f[j + 1] * fx) * (1 - fy) +
               (f[j + W] * (1 - fx) + f[j + W + 1] * fx) * fy;
    };
    auto tex = [&](int k, float u, float v) {
        u -= std::floor(u / N) * N;
        v -= std::floor(v / N) * N;
        const int x0 = (int)u, y0 = (int)v, x1 = (x0 + 1) % N,
                  y1 = (y0 + 1) % N;
        const float fx = u - x0, fy = v - y0;
        const float* T = g_alpha[k].data();
        return (T[y0 * N + x0] * (1 - fx) + T[y0 * N + x1] * fx) * (1 - fy) +
               (T[y1 * N + x0] * (1 - fx) + T[y1 * N + x1] * fx) * fy;
    };
    for (int py = 0; py < PH; ++py)
        for (int pxx = 0; pxx < PW; ++pxx) {
            const float cx = (pxx + .5f) * R / FL_C - .5f,
                        cy = (py + .5f) * R / FL_C - .5f;
            const float a0 = bil(F.al, cx, cy);
            uint32_t& o = F.px[(size_t)py * PW + pxx];
            if (a0 < .004f) {
                o = 0;
                continue;
            }
            const float dx = bil(F.ox, cx, cy) * FL_C,
                        dy = bil(F.oy, cx, cy) * FL_C, wx = pxx * R - dx,
                        wy = py * R - dy;
            float rest = 1.f, tz = 0.f;
            for (const Lg& l : L) {
                const float pr =
                    tex(l.k, wx / l.sc - l.vx * t, wy / l.sc - l.vy * t);
                rest *= 1.f - l.op * pr;
                if (l.k == 0)
                    tz = pr;
            }
            const float dens = 1.f - rest, a = std::min(.85f, dens * a0 * .95f),
                        m = std::min(1.f, dens * 1.1f + tz * .2f);
            const float r = (dun[0] + (hell[0] - dun[0]) * m) * a,
                        g = (dun[1] + (hell[1] - dun[1]) * m) * a,
                        b = (dun[2] + (hell[2] - dun[2]) * m) * a;
            o = ((uint32_t)(a * 255.f + .5f) << 24) |
                ((uint32_t)(r + .5f) << 16) | ((uint32_t)(g + .5f) << 8) |
                (uint32_t)(b + .5f);
        }
    if (F.owner != (const void*)rt) {
        F.bmp.Reset();
        F.owner = rt;
    }
    if (!F.bmp &&
        FAILED(rt->CreateBitmap(
            D2D1::SizeU(PW, PH),
            D2D1::BitmapProperties(D2D1::PixelFormat(
                DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)),
            &F.bmp)))
        return;
    F.bmp->CopyFromMemory(nullptr, F.px.data(), PW * 4);
    rt->DrawBitmap(
        F.bmp.Get(),
        D2D1::RectF(ziel.left, ziel.top, ziel.left + PW * R, ziel.top + PH * R),
        1.f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
}
namespace v211 {             // Smoke variant: simulation with mask
constexpr float FL_C = 5.f;  // cell size px
float& FL_MX = g_flMX;       // margin left/right (scaled by thickness)
float& FL_MY = g_flMY;       // margin top/bottom
struct Fluid {
    int W = 0, H = 0;
    float seed = 0;
    std::vector<float> u, v, u0, v0, d, d0, p, dv;
    std::vector<uint32_t> px;
    ComPtr<ID2D1Bitmap> bmp;
    ComPtr<ID2D1BitmapBrush> br;
    const void* owner = nullptr;
    void Init(int w, int h, float sd) {
        W = w;
        H = h;
        seed = sd;
        const size_t n = (size_t)w * h;
        for (auto* a : {&u, &v, &u0, &v0, &d, &d0, &p, &dv})
            a->assign(n, 0.f);
        px.assign(n, 0);
        bmp.Reset();
        br.Reset();
    }
};
std::vector<Fluid> g_fl;
inline float Quelle(float x,
                    float y,
                    float t,
                    int k = 0) {  // Noise field, bilinear (nearest-pixel caused
                                  // jitter), slowly drifting
    float fx = x * .55f + t * 1.2f, fy = y * .7f + t * 2.f;
    fx -= std::floor(fx / N) * N;
    fy -= std::floor(fy / N) * N;
    const int x0 = (int)fx, y0 = (int)fy, x1 = (x0 + 1) % N, y1 = (y0 + 1) % N;
    const float ax = fx - x0, ay = fy - y0;
    const float* T = g_alpha[k].data();
    return (T[y0 * N + x0] * (1 - ax) + T[y0 * N + x1] * ax) * (1 - ay) +
           (T[y1 * N + x0] * (1 - ax) + T[y1 * N + x1] * ax) * ay;
}
void FlAdv(Fluid& F,
           std::vector<float>& dst,
           const std::vector<float>& src,
           float dt) {
    const int W = F.W, H = F.H;
    for (int y = 0; y < H; ++y)
        for (int x = 0; x < W; ++x) {
            const int i = x + y * W;
            float px = x - dt * F.u[i], py = y - dt * F.v[i];
            px = std::clamp(px, 0.f, W - 1.001f);
            py = std::clamp(py, 0.f, H - 1.001f);
            const int x0 = (int)px, y0 = (int)py, j = x0 + y0 * W;
            const float fx = px - x0, fy = py - y0;
            dst[i] = (src[j] * (1 - fx) + src[j + 1] * fx) * (1 - fy) +
                     (src[j + W] * (1 - fx) + src[j + W + 1] * fx) * fy;
        }
}
void MittelBerechnen() {
    static bool ok = false;
    if (ok)
        return;
    for (int k = 0; k < 3; ++k) {
        double sm = 0;
        for (float a : g_alpha[k])
            sm += a;
        if (!g_alpha[k].empty())
            g_mittel[k] = (float)(sm / g_alpha[k].size());
    }
    ok = true;
}
void FluidSchritt(Fluid& F,
                  float dt,
                  float t,
                  float mx,
                  float my,
                  float mvx,
                  float mvy,
                  float ix0,
                  float iy0,
                  float ix1,
                  float iy1) {
    MittelBerechnen();
    const int W = F.W, H = F.H;
    auto& u = F.u;
    auto& v = F.v;
    auto& d = F.d;
    mvx = std::clamp(mvx, -8.f, 8.f);
    mvy = std::clamp(mvy, -8.f, 8.f);
    for (int y = 1; y < H - 1; ++y)
        for (int x = 1; x < W - 1; ++x) {
            const int i = x + y * W;
            v[i] -= dt * 7.f * d[i];  // buoyancy
            const float n1 = Quelle(x * 1.6f + 17 + F.seed, y * 1.6f, t * .35f,
                                    1) -
                             g_mittel[1],
                        n2 = Quelle(x * 1.4f + F.seed, y * 1.4f + 53, t * .3f,
                                    2) -
                             g_mittel[2];  // subtract the mean, otherwise a
                                           // constant drift appears
            u[i] += dt * 26.f * n1;
            v[i] += dt * 16.f * n2;  // irregular turbulence
            const float ddx = x - mx, ddy = y - my, r2 = ddx * ddx + ddy * ddy;
            if (g_mausF > 0.f && r2 < 90.f * g_mausF * g_rr) {
                const float g =
                    std::exp(-r2 / (18.f * g_mausF * g_rr)) * g_mausF;
                u[i] += g * (mvx * 4.f + ddx * .35f);
                v[i] += g * (mvy * 4.f + ddy * .35f);
                d[i] *= 1.f - .06f * std::exp(-r2 / (14.f * g_rr)) * g_mausF;
            }  // Subtle mouse interaction: drags and pushes the smoke
            u[i] *= .985f;
            v[i] *= .985f;
        }
    // Vorticity confinement
    auto& w = F.p;
    for (int y = 1; y < H - 1; ++y)
        for (int x = 1; x < W - 1; ++x) {
            const int i = x + y * W;
            w[i] = .5f * ((v[i + 1] - v[i - 1]) - (u[i + W] - u[i - W]));
        }
    for (int y = 2; y < H - 2; ++y)
        for (int x = 2; x < W - 2; ++x) {
            const int i = x + y * W;
            const float gx = .5f * (std::fabs(w[i + 1]) - std::fabs(w[i - 1])),
                        gy = .5f * (std::fabs(w[i + W]) - std::fabs(w[i - W])),
                        l = std::sqrt(gx * gx + gy * gy) + 1e-5f;
            u[i] += dt * 2.2f * (gy / l) * w[i];
            v[i] -= dt * 2.2f * (gx / l) * w[i];
        }
    F.u0 = u;
    F.v0 = v;
    FlAdv(F, u, F.u0, dt * 8.f);
    FlAdv(F, v, F.v0, dt * 8.f);
    auto& pp = F.p;
    auto& dv = F.dv;
    for (int y = 1; y < H - 1; ++y)
        for (int x = 1; x < W - 1; ++x) {
            const int i = x + y * W;
            dv[i] = -.5f * (u[i + 1] - u[i - 1] + v[i + W] - v[i - W]);
            pp[i] = 0;
        }
    for (int k = 0; k < 10; ++k)
        for (int y = 1; y < H - 1; ++y)
            for (int x = 1; x < W - 1; ++x) {
                const int i = x + y * W;
                pp[i] =
                    (dv[i] + pp[i - 1] + pp[i + 1] + pp[i - W] + pp[i + W]) *
                    .25f;
            }
    for (int y = 1; y < H - 1; ++y)
        for (int x = 1; x < W - 1; ++x) {
            const int i = x + y * W;
            u[i] -= .5f * (pp[i + 1] - pp[i - 1]);
            v[i] -= .5f * (pp[i + W] - pp[i - W]);
        }
    F.d0 = d;
    FlAdv(F, d, F.d0, dt * 8.f);
    for (int y = 0; y < H; ++y)
        for (int x = 0; x < W; ++x) {
            const int i = x + y * W;
            const bool innen = x >= ix0 && x < ix1 && y >= iy0 && y < iy1;
            if (innen) {
                const float ziel =
                    (.12f + .85f * Quelle(x * 1.6f + F.seed * 2, y * 1.6f, t)) *
                    g_dichteF;
                d[i] += (ziel - d[i]) * .035f;
            }  // thinner at rest; replenishes
            else
                d[i] *=
                    (y < iy0)
                        ? g_dkTop
                        : g_dkSide;  // rises visibly at the top, fades quickly
                                     // at the sides/bottom (no double rounding)
        }
}
void FluidMalen(Fluid& F, ID2D1RenderTarget* rt, D2D1_RECT_F ziel, int ti) {
    float cT[3];
    BasisFarbe(ti, cT);
    ToneRgb(cT);
    const float* c = cT;
    const float hell[3] = {std::min(255.f, c[0] * 1.55f + 30),
                           std::min(255.f, c[1] * 1.5f + 18),
                           std::min(255.f, c[2] * 1.55f + 30)},
                dun[3] = {c[0] * .55f, c[1] * .55f, c[2] * .55f};
    const int W = F.W, H = F.H;
    const float iy0 = FL_MY / FL_C, iy1 = H - FL_MY / FL_C,
                ih = std::max(1.f, iy1 - iy0);
    for (int y = 0; y < H; ++y)
        for (int x = 0; x < W; ++x) {
            const int i = x + y * W;
            const float a = std::min(1.f, F.d[i]);
            // Fade out softly around the pill shape (capsule) instead of at the
            // rectangle edge -> no ruler-straight edges
            const float ix0c = FL_MX / FL_C, ix1c = W - FL_MX / FL_C,
                        rr = ih * .5f, cyc = iy0 + rr;
            const float qx = std::max(0.f,
                                      std::max(ix0c + rr - x, x - (ix1c - rr))),
                        qy = std::fabs(y - cyc);
            const float dist = std::max(0.f, std::sqrt(qx * qx + qy * qy) -
                                                 rr);  // cells outside the pill
            const float reich =
                ((y < cyc) ? 6.f : 3.f) *
                g_dickeF;  // extends further upward, short at the sides/bottom
            float ef = std::clamp(1.f - dist / reich, 0.f, 1.f);
            ef = ef * ef * (3.f - 2.f * ef);
            float mask = 1.f;
            if (y >= iy0 && y < iy1) {
                const float v = (y - iy0) / ih;
                mask = .6f + .4f * std::pow(std::fabs(v - .5f) * 2.f, 1.5f);
            }  // center (icons) more subtle
            const float al = std::min(1.f, a * 2.2f) * ef * mask;
            (void)hell;
            (void)dun;  // mask; look = texture smoke
            const uint32_t A = (uint32_t)(al * 255.f + .5f);
            F.px[i] = (A << 24) | (A << 16) | (A << 8) | A;
        }
    if (F.owner != (const void*)rt) {
        F.bmp.Reset();
        F.br.Reset();
        F.owner = rt;
    }
    if (!F.bmp &&
        FAILED(rt->CreateBitmap(
            D2D1::SizeU(W, H),
            D2D1::BitmapProperties(D2D1::PixelFormat(
                DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)),
            &F.bmp)))
        return;
    F.bmp->CopyFromMemory(nullptr, F.px.data(), W * 4);
    if (!F.br)
        rt->CreateBitmapBrush(
            F.bmp.Get(),
            D2D1::BitmapBrushProperties(D2D1_EXTEND_MODE_CLAMP,
                                        D2D1_EXTEND_MODE_CLAMP,
                                        D2D1_BITMAP_INTERPOLATION_MODE_LINEAR),
            &F.br);
    if (!F.br)
        return;
    F.br->SetTransform(D2D1::Matrix3x2F::Scale((ziel.right - ziel.left) / W,
                                               (ziel.bottom - ziel.top) / H) *
                       D2D1::Matrix3x2F::Translation(ziel.left, ziel.top));
    ComPtr<ID2D1Layer> ly;
    if (FAILED(rt->CreateLayer(nullptr, &ly)) || !ly)
        return;
    rt->PushLayer(
        D2D1::LayerParameters(ziel, nullptr, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE,
                              D2D1::IdentityMatrix(), 1.f, F.br.Get()),
        ly.Get());
    const float t =
        (float)std::fmod(GetTickCount64() * 0.001 * g_tempoF, 3600.0);
    static const float opk[3] = {1.0f, .8f, .65f};
    for (int k = 0; k < 3; ++k)
        for (int ph = 0; ph < 2; ++ph) {
            Schicht(k, ziel.left, ziel.top, t, opk[k] * 1.3f, ph);
            rt->FillRectangle(ziel, g_satz.br[g_ti][k].Get());
        }
    rt->PopLayer();
}
}  // namespace v211
namespace v220 {             // Smoke variant: simulation carries the texture
constexpr float FL_C = 5.f;  // cell size px
float& FL_MX = g_flMX;       // margin left/right (scaled by thickness)
float& FL_MY = g_flMY;       // margin top/bottom
struct Fluid {
    int W = 0, H = 0;
    float seed = 0;
    std::vector<float> u, v, u0, v0, d, d0, p, dv, ox, oy, ox0, oy0, al;
    std::vector<uint32_t> px;
    ComPtr<ID2D1Bitmap> bmp;
    const void* owner = nullptr;
    void Init(int w, int h, float sd) {
        W = w;
        H = h;
        seed = sd;
        const size_t n = (size_t)w * h;
        for (auto* a :
             {&u, &v, &u0, &v0, &d, &d0, &p, &dv, &ox, &oy, &ox0, &oy0, &al})
            a->assign(n, 0.f);
        px.clear();
        bmp.Reset();
    }
};
std::vector<Fluid> g_fl;
inline float Quelle(float x,
                    float y,
                    float t,
                    int k = 0) {  // Noise field, bilinear (nearest-pixel caused
                                  // jitter), slowly drifting
    float fx = x * .55f + t * 1.2f, fy = y * .7f + t * 2.f;
    fx -= std::floor(fx / N) * N;
    fy -= std::floor(fy / N) * N;
    const int x0 = (int)fx, y0 = (int)fy, x1 = (x0 + 1) % N, y1 = (y0 + 1) % N;
    const float ax = fx - x0, ay = fy - y0;
    const float* T = g_alpha[k].data();
    return (T[y0 * N + x0] * (1 - ax) + T[y0 * N + x1] * ax) * (1 - ay) +
           (T[y1 * N + x0] * (1 - ax) + T[y1 * N + x1] * ax) * ay;
}
void FlAdv(Fluid& F,
           std::vector<float>& dst,
           const std::vector<float>& src,
           float dt) {
    const int W = F.W, H = F.H;
    for (int y = 0; y < H; ++y)
        for (int x = 0; x < W; ++x) {
            const int i = x + y * W;
            float px = x - dt * F.u[i], py = y - dt * F.v[i];
            px = std::clamp(px, 0.f, W - 1.001f);
            py = std::clamp(py, 0.f, H - 1.001f);
            const int x0 = (int)px, y0 = (int)py, j = x0 + y0 * W;
            const float fx = px - x0, fy = py - y0;
            dst[i] = (src[j] * (1 - fx) + src[j + 1] * fx) * (1 - fy) +
                     (src[j + W] * (1 - fx) + src[j + W + 1] * fx) * fy;
        }
}
void MittelBerechnen() {
    static bool ok = false;
    if (ok)
        return;
    for (int k = 0; k < 3; ++k) {
        double sm = 0;
        for (float a : g_alpha[k])
            sm += a;
        if (!g_alpha[k].empty())
            g_mittel[k] = (float)(sm / g_alpha[k].size());
    }
    ok = true;
}
void FluidSchritt(Fluid& F,
                  float dt,
                  float t,
                  float mx,
                  float my,
                  float mvx,
                  float mvy,
                  float ix0,
                  float iy0,
                  float ix1,
                  float iy1) {
    MittelBerechnen();
    const int W = F.W, H = F.H;
    auto& u = F.u;
    auto& v = F.v;
    auto& d = F.d;
    mvx = std::clamp(mvx, -8.f, 8.f);
    mvy = std::clamp(mvy, -8.f, 8.f);
    for (int y = 1; y < H - 1; ++y)
        for (int x = 1; x < W - 1; ++x) {
            const int i = x + y * W;
            v[i] -= dt * 7.f * d[i];  // buoyancy
            const float n1 = Quelle(x * 1.6f + 17 + F.seed, y * 1.6f, t * .35f,
                                    1) -
                             g_mittel[1],
                        n2 = Quelle(x * 1.4f + F.seed, y * 1.4f + 53, t * .3f,
                                    2) -
                             g_mittel[2];  // subtract the mean, otherwise a
                                           // constant drift appears
            u[i] += dt * 26.f * n1;
            v[i] += dt * 16.f * n2;  // irregular turbulence
            const float ddx = x - mx, ddy = y - my, r2 = ddx * ddx + ddy * ddy;
            if (g_mausF > 0.f && r2 < 90.f * g_mausF * g_rr) {
                const float g =
                    std::exp(-r2 / (18.f * g_mausF * g_rr)) * g_mausF;
                u[i] += g * (mvx * 4.f + ddx * .35f);
                v[i] += g * (mvy * 4.f + ddy * .35f);
                d[i] *= 1.f - .06f * std::exp(-r2 / (14.f * g_rr)) * g_mausF;
            }  // Subtle mouse interaction: drags and pushes the smoke
            u[i] *= .985f;
            v[i] *= .985f;
        }
    // Vorticity confinement
    auto& w = F.p;
    for (int y = 1; y < H - 1; ++y)
        for (int x = 1; x < W - 1; ++x) {
            const int i = x + y * W;
            w[i] = .5f * ((v[i + 1] - v[i - 1]) - (u[i + W] - u[i - W]));
        }
    for (int y = 2; y < H - 2; ++y)
        for (int x = 2; x < W - 2; ++x) {
            const int i = x + y * W;
            const float gx = .5f * (std::fabs(w[i + 1]) - std::fabs(w[i - 1])),
                        gy = .5f * (std::fabs(w[i + W]) - std::fabs(w[i - W])),
                        l = std::sqrt(gx * gx + gy * gy) + 1e-5f;
            u[i] += dt * 2.2f * (gy / l) * w[i];
            v[i] -= dt * 2.2f * (gx / l) * w[i];
        }
    F.u0 = u;
    F.v0 = v;
    FlAdv(F, u, F.u0, dt * 8.f);
    FlAdv(F, v, F.v0, dt * 8.f);
    auto& pp = F.p;
    auto& dv = F.dv;
    for (int y = 1; y < H - 1; ++y)
        for (int x = 1; x < W - 1; ++x) {
            const int i = x + y * W;
            dv[i] = -.5f * (u[i + 1] - u[i - 1] + v[i + W] - v[i - W]);
            pp[i] = 0;
        }
    for (int k = 0; k < 10; ++k)
        for (int y = 1; y < H - 1; ++y)
            for (int x = 1; x < W - 1; ++x) {
                const int i = x + y * W;
                pp[i] =
                    (dv[i] + pp[i - 1] + pp[i + 1] + pp[i - W] + pp[i + W]) *
                    .25f;
            }
    for (int y = 1; y < H - 1; ++y)
        for (int x = 1; x < W - 1; ++x) {
            const int i = x + y * W;
            u[i] -= .5f * (pp[i + 1] - pp[i - 1]);
            v[i] -= .5f * (pp[i + W] - pp[i - W]);
        }
    F.d0 = d;
    FlAdv(F, d, F.d0, dt * 8.f);
    // Displacement field (cells): the smoke texture is actually carried by the
    // flow
    F.ox0 = F.ox;
    F.oy0 = F.oy;
    FlAdv(F, F.ox, F.ox0, dt * 8.f);
    FlAdv(F, F.oy, F.oy0, dt * 8.f);
    for (size_t i = 0; i < F.ox.size(); ++i) {
        F.ox[i] = std::clamp((F.ox[i] + u[i] * dt * 8.f) * .994f, -10.f, 10.f);
        F.oy[i] = std::clamp((F.oy[i] + v[i] * dt * 8.f) * .994f, -10.f, 10.f);
    }
    for (int y = 0; y < H; ++y)
        for (int x = 0; x < W; ++x) {
            const int i = x + y * W;
            const bool innen = x >= ix0 && x < ix1 && y >= iy0 && y < iy1;
            if (innen) {
                const float ziel =
                    (.12f + .85f * Quelle(x * 1.6f + F.seed * 2, y * 1.6f, t)) *
                    g_dichteF;
                d[i] += (ziel - d[i]) * .035f;
            }  // thinner at rest; replenishes
            else
                d[i] *=
                    (y < iy0)
                        ? g_dkTop
                        : g_dkSide;  // rises visibly at the top, fades quickly
                                     // at the sides/bottom (no double rounding)
        }
}
void FluidMalen(Fluid& F, ID2D1RenderTarget* rt, D2D1_RECT_F ziel, int ti) {
    float cT[3];
    BasisFarbe(ti, cT);
    ToneRgb(cT);
    const float* c = cT;
    const float hell[3] = {std::min(255.f, c[0] * 1.55f + 30),
                           std::min(255.f, c[1] * 1.5f + 18),
                           std::min(255.f, c[2] * 1.55f + 30)},
                dun[3] = {c[0] * .5f, c[1] * .5f, c[2] * .5f};
    const int W = F.W, H = F.H;
    const float iy0 = FL_MY / FL_C, iy1 = H - FL_MY / FL_C,
                ih = std::max(1.f, iy1 - iy0);
    // 1) Cell alpha (density, capsule fade-out, calm center)
    for (int y = 0; y < H; ++y)
        for (int x = 0; x < W; ++x) {
            const int i = x + y * W;
            const float a = std::min(1.f, F.d[i]);
            const float ix0c = FL_MX / FL_C, ix1c = W - FL_MX / FL_C,
                        rr = ih * .5f, cyc = iy0 + rr;
            const float qx = std::max(0.f,
                                      std::max(ix0c + rr - x, x - (ix1c - rr))),
                        qy = std::fabs(y - cyc);
            const float dist = std::max(0.f, std::sqrt(qx * qx + qy * qy) - rr);
            const float reich = ((y < cyc) ? 6.f : 3.f) * g_dickeF;
            float ef = std::clamp(1.f - dist / reich, 0.f, 1.f);
            ef = ef * ef * (3.f - 2.f * ef);
            float mask = 1.f;
            if (y >= iy0 && y < iy1) {
                const float v = (y - iy0) / ih;
                mask = .6f + .4f * std::pow(std::fabs(v - .5f) * 2.f, 1.5f);
            }
            F.al[i] = std::min(1.f, a * 2.2f) * ef * mask;
        }
    // 2) Pixels (2 px): texture smoke, texture coordinates offset by the
    // advected displacement field
    constexpr int R = 2;
    const int PW = (int)std::ceil(W * FL_C / R),
              PH = (int)std::ceil(H * FL_C / R);
    if ((int)F.px.size() != PW * PH) {
        F.px.assign((size_t)PW * PH, 0);
        F.bmp.Reset();
    }
    const float t =
        (float)std::fmod(GetTickCount64() * 0.001 * g_tempoF, 3600.0);
    struct Lg {
        int k;
        float sc, vx, vy, op;
    };
    static const Lg L[3] = {{0, 1.55f, 3.f, -6.f, .95f},
                            {1, 1.08f, -4.f, -4.f, .75f},
                            {2, 2.16f, 2.f, -8.f, .6f}};
    auto bil = [&](const std::vector<float>& f, float cx, float cy) {
        cx = std::clamp(cx, 0.f, W - 1.001f);
        cy = std::clamp(cy, 0.f, H - 1.001f);
        const int x0 = (int)cx, y0 = (int)cy, j = x0 + y0 * W;
        const float fx = cx - x0, fy = cy - y0;
        return (f[j] * (1 - fx) + f[j + 1] * fx) * (1 - fy) +
               (f[j + W] * (1 - fx) + f[j + W + 1] * fx) * fy;
    };
    auto tex = [&](int k, float u, float v) {
        u -= std::floor(u / N) * N;
        v -= std::floor(v / N) * N;
        const int x0 = (int)u, y0 = (int)v, x1 = (x0 + 1) % N,
                  y1 = (y0 + 1) % N;
        const float fx = u - x0, fy = v - y0;
        const float* T = g_alpha[k].data();
        return (T[y0 * N + x0] * (1 - fx) + T[y0 * N + x1] * fx) * (1 - fy) +
               (T[y1 * N + x0] * (1 - fx) + T[y1 * N + x1] * fx) * fy;
    };
    for (int py = 0; py < PH; ++py)
        for (int pxx = 0; pxx < PW; ++pxx) {
            const float cx = (pxx + .5f) * R / FL_C - .5f,
                        cy = (py + .5f) * R / FL_C - .5f;
            const float a0 = bil(F.al, cx, cy);
            uint32_t& o = F.px[(size_t)py * PW + pxx];
            if (a0 < .004f) {
                o = 0;
                continue;
            }
            const float dx = bil(F.ox, cx, cy) * FL_C,
                        dy = bil(F.oy, cx, cy) * FL_C, wx = pxx * R - dx,
                        wy = py * R - dy;
            float rest = 1.f, tz = 0.f;
            for (const Lg& l : L) {
                const float pr =
                    tex(l.k, wx / l.sc - l.vx * t, wy / l.sc - l.vy * t);
                rest *= 1.f - l.op * pr;
                if (l.k == 0)
                    tz = pr;
            }
            const float dens = 1.f - rest, a = std::min(1.f, dens * a0 * 1.35f),
                        m = std::min(1.f, dens * 1.1f + tz * .2f);
            const float r = (dun[0] + (hell[0] - dun[0]) * m) * a,
                        g = (dun[1] + (hell[1] - dun[1]) * m) * a,
                        b = (dun[2] + (hell[2] - dun[2]) * m) * a;
            o = ((uint32_t)(a * 255.f + .5f) << 24) |
                ((uint32_t)(r + .5f) << 16) | ((uint32_t)(g + .5f) << 8) |
                (uint32_t)(b + .5f);
        }
    if (F.owner != (const void*)rt) {
        F.bmp.Reset();
        F.owner = rt;
    }
    if (!F.bmp &&
        FAILED(rt->CreateBitmap(
            D2D1::SizeU(PW, PH),
            D2D1::BitmapProperties(D2D1::PixelFormat(
                DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)),
            &F.bmp)))
        return;
    F.bmp->CopyFromMemory(nullptr, F.px.data(), PW * 4);
    rt->DrawBitmap(
        F.bmp.Get(),
        D2D1::RectF(ziel.left, ziel.top, ziel.left + PW * R, ziel.top + PH * R),
        1.f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
}
}  // namespace v220
namespace v221 {  // Smoke variant: simulation carries the texture, subtle
constexpr float FL_C = 5.f;  // cell size px
float& FL_MX = g_flMX;       // margin left/right (scaled by thickness)
float& FL_MY = g_flMY;       // margin top/bottom
struct Fluid {
    int W = 0, H = 0;
    float seed = 0;
    std::vector<float> u, v, u0, v0, d, d0, p, dv, ox, oy, ox0, oy0, al;
    std::vector<uint32_t> px;
    ComPtr<ID2D1Bitmap> bmp;
    const void* owner = nullptr;
    void Init(int w, int h, float sd) {
        W = w;
        H = h;
        seed = sd;
        const size_t n = (size_t)w * h;
        for (auto* a :
             {&u, &v, &u0, &v0, &d, &d0, &p, &dv, &ox, &oy, &ox0, &oy0, &al})
            a->assign(n, 0.f);
        px.clear();
        bmp.Reset();
    }
};
std::vector<Fluid> g_fl;
inline float Quelle(float x,
                    float y,
                    float t,
                    int k = 0) {  // Noise field, bilinear (nearest-pixel caused
                                  // jitter), slowly drifting
    float fx = x * .55f + t * 1.2f, fy = y * .7f + t * 2.f;
    fx -= std::floor(fx / N) * N;
    fy -= std::floor(fy / N) * N;
    const int x0 = (int)fx, y0 = (int)fy, x1 = (x0 + 1) % N, y1 = (y0 + 1) % N;
    const float ax = fx - x0, ay = fy - y0;
    const float* T = g_alpha[k].data();
    return (T[y0 * N + x0] * (1 - ax) + T[y0 * N + x1] * ax) * (1 - ay) +
           (T[y1 * N + x0] * (1 - ax) + T[y1 * N + x1] * ax) * ay;
}
void FlAdv(Fluid& F,
           std::vector<float>& dst,
           const std::vector<float>& src,
           float dt) {
    const int W = F.W, H = F.H;
    for (int y = 0; y < H; ++y)
        for (int x = 0; x < W; ++x) {
            const int i = x + y * W;
            float px = x - dt * F.u[i], py = y - dt * F.v[i];
            px = std::clamp(px, 0.f, W - 1.001f);
            py = std::clamp(py, 0.f, H - 1.001f);
            const int x0 = (int)px, y0 = (int)py, j = x0 + y0 * W;
            const float fx = px - x0, fy = py - y0;
            dst[i] = (src[j] * (1 - fx) + src[j + 1] * fx) * (1 - fy) +
                     (src[j + W] * (1 - fx) + src[j + W + 1] * fx) * fy;
        }
}
void MittelBerechnen() {
    static bool ok = false;
    if (ok)
        return;
    for (int k = 0; k < 3; ++k) {
        double sm = 0;
        for (float a : g_alpha[k])
            sm += a;
        if (!g_alpha[k].empty())
            g_mittel[k] = (float)(sm / g_alpha[k].size());
    }
    ok = true;
}
void FluidSchritt(Fluid& F,
                  float dt,
                  float t,
                  float mx,
                  float my,
                  float mvx,
                  float mvy,
                  float ix0,
                  float iy0,
                  float ix1,
                  float iy1) {
    MittelBerechnen();
    const int W = F.W, H = F.H;
    auto& u = F.u;
    auto& v = F.v;
    auto& d = F.d;
    mvx = std::clamp(mvx, -8.f, 8.f);
    mvy = std::clamp(mvy, -8.f, 8.f);
    for (int y = 1; y < H - 1; ++y)
        for (int x = 1; x < W - 1; ++x) {
            const int i = x + y * W;
            v[i] -= dt * 7.f * d[i];  // buoyancy
            const float n1 = Quelle(x * 1.6f + 17 + F.seed, y * 1.6f, t * .35f,
                                    1) -
                             g_mittel[1],
                        n2 = Quelle(x * 1.4f + F.seed, y * 1.4f + 53, t * .3f,
                                    2) -
                             g_mittel[2];  // subtract the mean, otherwise a
                                           // constant drift appears
            u[i] += dt * 26.f * n1;
            v[i] += dt * 16.f * n2;  // irregular turbulence
            const float ddx = x - mx, ddy = y - my, r2 = ddx * ddx + ddy * ddy;
            if (g_mausF > 0.f && r2 < 90.f * g_mausF * g_rr) {
                const float g =
                    std::exp(-r2 / (18.f * g_mausF * g_rr)) * g_mausF;
                u[i] += g * (mvx * 4.f + ddx * .35f);
                v[i] += g * (mvy * 4.f + ddy * .35f);
                d[i] *= 1.f - .06f * std::exp(-r2 / (14.f * g_rr)) * g_mausF;
            }  // Subtle mouse interaction: drags and pushes the smoke
            u[i] *= .985f;
            v[i] *= .985f;
        }
    // Vorticity confinement
    auto& w = F.p;
    for (int y = 1; y < H - 1; ++y)
        for (int x = 1; x < W - 1; ++x) {
            const int i = x + y * W;
            w[i] = .5f * ((v[i + 1] - v[i - 1]) - (u[i + W] - u[i - W]));
        }
    for (int y = 2; y < H - 2; ++y)
        for (int x = 2; x < W - 2; ++x) {
            const int i = x + y * W;
            const float gx = .5f * (std::fabs(w[i + 1]) - std::fabs(w[i - 1])),
                        gy = .5f * (std::fabs(w[i + W]) - std::fabs(w[i - W])),
                        l = std::sqrt(gx * gx + gy * gy) + 1e-5f;
            u[i] += dt * 2.2f * (gy / l) * w[i];
            v[i] -= dt * 2.2f * (gx / l) * w[i];
        }
    F.u0 = u;
    F.v0 = v;
    FlAdv(F, u, F.u0, dt * 8.f);
    FlAdv(F, v, F.v0, dt * 8.f);
    auto& pp = F.p;
    auto& dv = F.dv;
    for (int y = 1; y < H - 1; ++y)
        for (int x = 1; x < W - 1; ++x) {
            const int i = x + y * W;
            dv[i] = -.5f * (u[i + 1] - u[i - 1] + v[i + W] - v[i - W]);
            pp[i] = 0;
        }
    for (int k = 0; k < 10; ++k)
        for (int y = 1; y < H - 1; ++y)
            for (int x = 1; x < W - 1; ++x) {
                const int i = x + y * W;
                pp[i] =
                    (dv[i] + pp[i - 1] + pp[i + 1] + pp[i - W] + pp[i + W]) *
                    .25f;
            }
    for (int y = 1; y < H - 1; ++y)
        for (int x = 1; x < W - 1; ++x) {
            const int i = x + y * W;
            u[i] -= .5f * (pp[i + 1] - pp[i - 1]);
            v[i] -= .5f * (pp[i + W] - pp[i - W]);
        }
    F.d0 = d;
    FlAdv(F, d, F.d0, dt * 8.f);
    // Displacement field (cells): the smoke texture is actually carried by the
    // flow
    F.ox0 = F.ox;
    F.oy0 = F.oy;
    FlAdv(F, F.ox, F.ox0, dt * 8.f);
    FlAdv(F, F.oy, F.oy0, dt * 8.f);
    for (size_t i = 0; i < F.ox.size(); ++i) {
        F.ox[i] = std::clamp((F.ox[i] + u[i] * dt * 8.f) * .994f, -10.f, 10.f);
        F.oy[i] = std::clamp((F.oy[i] + v[i] * dt * 8.f) * .994f, -10.f, 10.f);
    }
    for (int y = 0; y < H; ++y)
        for (int x = 0; x < W; ++x) {
            const int i = x + y * W;
            const bool innen = x >= ix0 && x < ix1 && y >= iy0 && y < iy1;
            if (innen) {
                const float ziel =
                    (.12f + .85f * Quelle(x * 1.6f + F.seed * 2, y * 1.6f, t)) *
                    g_dichteF;
                d[i] += (ziel - d[i]) * .035f;
            }  // thinner at rest; replenishes
            else
                d[i] *=
                    (y < iy0)
                        ? g_dkTop
                        : g_dkSide;  // rises visibly at the top, fades quickly
                                     // at the sides/bottom (no double rounding)
        }
}
void FluidMalen(Fluid& F, ID2D1RenderTarget* rt, D2D1_RECT_F ziel, int ti) {
    float cT[3];
    BasisFarbe(ti, cT);
    ToneRgb(cT);
    const float* c = cT;
    const float hell[3] = {std::min(255.f, c[0] * 1.55f + 30),
                           std::min(255.f, c[1] * 1.5f + 18),
                           std::min(255.f, c[2] * 1.55f + 30)},
                dun[3] = {c[0] * .5f, c[1] * .5f, c[2] * .5f};
    const int W = F.W, H = F.H;
    const float iy0 = FL_MY / FL_C, iy1 = H - FL_MY / FL_C,
                ih = std::max(1.f, iy1 - iy0);
    // 1) Cell alpha (density, capsule fade-out, calm center)
    for (int y = 0; y < H; ++y)
        for (int x = 0; x < W; ++x) {
            const int i = x + y * W;
            const float a = std::min(1.f, F.d[i]);
            const float ix0c = FL_MX / FL_C, ix1c = W - FL_MX / FL_C,
                        rr = ih * .5f, cyc = iy0 + rr;
            const float qx = std::max(0.f,
                                      std::max(ix0c + rr - x, x - (ix1c - rr))),
                        qy = std::fabs(y - cyc);
            const float dist = std::max(0.f, std::sqrt(qx * qx + qy * qy) - rr);
            const float reich = ((y < cyc) ? 6.f : 3.f) * g_dickeF;
            float ef = std::clamp(1.f - dist / reich, 0.f, 1.f);
            ef = ef * ef * (3.f - 2.f * ef);
            float mask = 1.f;
            if (y >= iy0 && y < iy1) {
                const float v = (y - iy0) / ih;
                mask = .28f + .72f * std::pow(std::fabs(v - .5f) * 2.f,
                                              1.3f); /* keep icons clear */
            }
            F.al[i] = std::min(1.f, a * 2.2f) * ef * mask;
        }
    // 2) Pixels (2 px): texture smoke, texture coordinates offset by the
    // advected displacement field
    constexpr int R = 2;
    const int PW = (int)std::ceil(W * FL_C / R),
              PH = (int)std::ceil(H * FL_C / R);
    if ((int)F.px.size() != PW * PH) {
        F.px.assign((size_t)PW * PH, 0);
        F.bmp.Reset();
    }
    const float t =
        (float)std::fmod(GetTickCount64() * 0.001 * g_tempoF, 3600.0);
    struct Lg {
        int k;
        float sc, vx, vy, op;
    };
    static const Lg L[3] = {{0, 1.55f, 3.f, -6.f, .95f},
                            {1, 1.08f, -4.f, -4.f, .75f},
                            {2, 2.16f, 2.f, -8.f, .6f}};
    auto bil = [&](const std::vector<float>& f, float cx, float cy) {
        cx = std::clamp(cx, 0.f, W - 1.001f);
        cy = std::clamp(cy, 0.f, H - 1.001f);
        const int x0 = (int)cx, y0 = (int)cy, j = x0 + y0 * W;
        const float fx = cx - x0, fy = cy - y0;
        return (f[j] * (1 - fx) + f[j + 1] * fx) * (1 - fy) +
               (f[j + W] * (1 - fx) + f[j + W + 1] * fx) * fy;
    };
    auto tex = [&](int k, float u, float v) {
        u -= std::floor(u / N) * N;
        v -= std::floor(v / N) * N;
        const int x0 = (int)u, y0 = (int)v, x1 = (x0 + 1) % N,
                  y1 = (y0 + 1) % N;
        const float fx = u - x0, fy = v - y0;
        const float* T = g_alpha[k].data();
        return (T[y0 * N + x0] * (1 - fx) + T[y0 * N + x1] * fx) * (1 - fy) +
               (T[y1 * N + x0] * (1 - fx) + T[y1 * N + x1] * fx) * fy;
    };
    for (int py = 0; py < PH; ++py)
        for (int pxx = 0; pxx < PW; ++pxx) {
            const float cx = (pxx + .5f) * R / FL_C - .5f,
                        cy = (py + .5f) * R / FL_C - .5f;
            const float a0 = bil(F.al, cx, cy);
            uint32_t& o = F.px[(size_t)py * PW + pxx];
            if (a0 < .004f) {
                o = 0;
                continue;
            }
            const float dx = bil(F.ox, cx, cy) * FL_C,
                        dy = bil(F.oy, cx, cy) * FL_C, wx = pxx * R - dx,
                        wy = py * R - dy;
            float rest = 1.f, tz = 0.f;
            for (const Lg& l : L) {
                const float pr =
                    tex(l.k, wx / l.sc - l.vx * t, wy / l.sc - l.vy * t);
                rest *= 1.f - l.op * pr;
                if (l.k == 0)
                    tz = pr;
            }
            const float dens = 1.f - rest, a = std::min(.85f, dens * a0 * .95f),
                        m = std::min(1.f, dens * 1.1f + tz * .2f);
            const float r = (dun[0] + (hell[0] - dun[0]) * m) * a,
                        g = (dun[1] + (hell[1] - dun[1]) * m) * a,
                        b = (dun[2] + (hell[2] - dun[2]) * m) * a;
            o = ((uint32_t)(a * 255.f + .5f) << 24) |
                ((uint32_t)(r + .5f) << 16) | ((uint32_t)(g + .5f) << 8) |
                (uint32_t)(b + .5f);
        }
    if (F.owner != (const void*)rt) {
        F.bmp.Reset();
        F.owner = rt;
    }
    if (!F.bmp &&
        FAILED(rt->CreateBitmap(
            D2D1::SizeU(PW, PH),
            D2D1::BitmapProperties(D2D1::PixelFormat(
                DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)),
            &F.bmp)))
        return;
    F.bmp->CopyFromMemory(nullptr, F.px.data(), PW * 4);
    rt->DrawBitmap(
        F.bmp.Get(),
        D2D1::RectF(ziel.left, ziel.top, ziel.left + PW * R, ziel.top + PH * R),
        1.f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
}
}  // namespace v221
void Zeichnen() {
    if (g_pillen.empty()) {
        return;
    }
    EigenfarbeAktualisieren();
    {  // thickness changed: start the simulations fresh at the new size
        static float lastK = 1.f;
        if (lastK != g_dickeF) {
            lastK = g_dickeF;
            g_fl.clear();
            v211::g_fl.clear();
            v220::g_fl.clear();
            v221::g_fl.clear();
        }
    }
    const float kd = std::max(1.f, g_dickeF);
    const float pad = std::max(
        46.f,
        std::max(g_flMX, g_flMY) +
            12.f);  // glow 19 px + margin, never clipped at the window edge
    float L = 1e9f, T = 1e9f, R = -1e9f, B = -1e9f;
    for (auto& p : g_pillen) {
        L = std::min(L, p.l);
        R = std::max(R, p.r);
        T = std::min(T, p.t);
        B = std::max(B, p.b);
    }
    const int x0 = (int)std::floor(L - pad), y0 = (int)std::floor(T - pad),
              w = (int)std::ceil(R + pad) - x0,
              h = (int)std::ceil(B + pad) - y0;
    if (w < 10 || h < 10 || !Ziel(w, h)) {
        return;
    }
    if (!g_fab &&
        FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,
                                 __uuidof(ID2D1Factory), (void**)&g_fab))) {
        return;
    }
    if (!g_rt) {
        auto props = D2D1::RenderTargetProperties(
            D2D1_RENDER_TARGET_TYPE_DEFAULT,
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
                              D2D1_ALPHA_MODE_PREMULTIPLIED),
            96, 96, D2D1_RENDER_TARGET_USAGE_GDI_COMPATIBLE);
        if (FAILED(g_fab->CreateDCRenderTarget(&props, &g_rt))) {
            return;
        }
    }
    RECT rc{0, 0, w, h};
    if (FAILED(g_rt->BindDC(g_memdc, &rc))) {
        return;
    }
    if (!Bereit(g_rt.Get())) {
        g_rt.Reset();
        return;
    }
    const float t =
        (float)std::fmod(GetTickCount64() * 0.001 * g_tempoF, 3600.0);
    g_rt->BeginDraw();
    g_rt->Clear(D2D1::ColorF(0, 0, 0, 0));
    if (g_modus == 1) {  // classic texture smoke
        for (auto& p : g_pillen) {
            const D2D1_RECT_F r = D2D1::RectF(p.l - x0, p.t - y0,
                                              p.r - x0, p.b - y0);
            const float rad = (r.bottom - r.top) * 0.5f;
            // Glow only along the top/bottom of the pill, faded out at the
            // rounded ends (no double rounding beyond the sides)
            ComPtr<ID2D1Layer> endLy;
            ComPtr<ID2D1LinearGradientBrush> endB;
            ComPtr<ID2D1GradientStopCollection> endC;
            {
                const float wp = r.right - r.left,
                            e = std::min(wp * 0.45f, 78.f) / wp;
                D2D1_GRADIENT_STOP es[4] = {
                    {0.f, D2D1::ColorF(0, 0, 0, 0.f)},
                    {e, D2D1::ColorF(0, 0, 0, 1.f)},
                    {1.f - e, D2D1::ColorF(0, 0, 0, 1.f)},
                    {1.f, D2D1::ColorF(0, 0, 0, 0.f)}};
                if (SUCCEEDED(
                        g_rt->CreateGradientStopCollection(es, 4, &endC)) &&
                    endC &&
                    SUCCEEDED(g_rt->CreateLinearGradientBrush(
                        D2D1::LinearGradientBrushProperties(
                            D2D1::Point2F(r.left, 0),
                            D2D1::Point2F(r.right, 0)),
                        endC.Get(), &endB)) &&
                    endB && SUCCEEDED(g_rt->CreateLayer(nullptr, &endLy)) &&
                    endLy)
                    g_rt->PushLayer(
                        D2D1::LayerParameters(
                            D2D1::RectF(r.left - 40 * kd, r.top - 40 * kd,
                                        r.right + 40 * kd, r.bottom + 40 * kd),
                            nullptr, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE,
                            D2D1::IdentityMatrix(), 1.f, endB.Get()),
                        endLy.Get());
            }
            // Outer glow: soft smoke rings
            // Weak base glow WITHOUT mask (no gaps/cut-out blobs); the mask
            // only adds wisps on top
            for (int i = 1; i <= 16; ++i) {
                const float d0 = 1.1f * i, d = d0 * g_dickeF,
                            f = std::min(1.f, d0 / 2.5f) *
                                std::pow(std::max(0.f, 1.f - d0 / 18.f), 1.6f);
                Schicht(0, r.left, r.top, t + 3, .45f * f * g_dichteF, 0);
                g_rt->DrawRoundedRectangle(
                    D2D1::RoundedRect(D2D1::RectF(r.left - d, r.top - d,
                                                  r.right + d, r.bottom + d),
                                      rad + d, rad + d),
                    g_satz.br[g_ti][0].Get(), 2.2f * g_dickeF);
            }
            // Coarse, slowly drifting mask -> glow reaches far in places,
            // barely in others (no ruler-straight edge)
            ComPtr<ID2D1Layer> saumLy;
            g_rt->CreateLayer(nullptr, &saumLy);
            const bool saumMaske = saumLy && g_satz.br[g_ti][2];
            if (saumMaske) {
                g_satz.br[g_ti][2]->SetOpacity(1.f);
                g_satz.br[g_ti][2]->SetTransform(
                    D2D1::Matrix3x2F::Scale(3.2f, 2.2f) *
                    D2D1::Matrix3x2F::Translation(
                        r.left + std::fmod(t * 4.f, 614.f),
                        r.top - std::fmod(t * 2.f, 422.f)));
                g_rt->PushLayer(
                    D2D1::LayerParameters(
                        D2D1::RectF(r.left - 30 * kd, r.top - 30 * kd,
                                    r.right + 30 * kd, r.bottom + 30 * kd),
                        nullptr, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE,
                        D2D1::IdentityMatrix(), 1.f, g_satz.br[g_ti][2].Get()),
                    saumLy.Get());
            }
            for (int i = 1; i <= 20;
                 ++i) {  // 20 fine rings, ONE smoke pattern -> wisps instead of
                         // separate strokes
                const float d0 = .95f * i, d = d0 * g_dickeF,
                            f = std::min(1.f, d0 / 5.f) *
                                std::pow(std::max(0.f, 1.f - d0 / 20.f),
                                         1.3f);  // thin at the edge, dense at
                                                 // 4-6 px, then frays out
                for (int ph = 0; ph < 2; ++ph) {
                    Schicht(0, r.left, r.top, t + 3, 1.25f * f * g_dichteF,
                            ph);  // stronger; the mask thins it out
                    g_rt->DrawRoundedRectangle(
                        D2D1::RoundedRect(
                            D2D1::RectF(r.left - d, r.top - d, r.right + d,
                                        r.bottom + d),
                            rad + d, rad + d),
                        g_satz.br[g_ti][0].Get(), 2.0f * g_dickeF);
                }
            }
            if (saumMaske)
                g_rt->PopLayer();
            if (endLy && endB)
                g_rt->PopLayer();
        }
        if (FAILED(g_rt->EndDraw())) {
            g_rt.Reset();
            return;
        }
        BoxBlur(static_cast<uint32_t*>(g_bits), w, h,
                5);  // Blur the glow -> no hard contours
        BoxBlur(static_cast<uint32_t*>(g_bits), w, h, 4);
        g_rt->BindDC(g_memdc, &rc);
        g_rt->BeginDraw();
        for (auto& p : g_pillen) {
            const D2D1_RECT_F r = D2D1::RectF(p.l - x0, p.t - y0,
                                              p.r - x0, p.b - y0);
            const float rad = (r.bottom - r.top) * 0.5f;
            const D2D1_ROUNDED_RECT rr = D2D1::RoundedRect(r, rad, rad);
            // Inner haze: dense at the edge, only a hint in the center (icons)
            ComPtr<ID2D1PathGeometry> geo;
            g_fab->CreatePathGeometry(&geo);
            ComPtr<ID2D1RoundedRectangleGeometry> rg;
            g_fab->CreateRoundedRectangleGeometry(rr, &rg);
            D2D1_GRADIENT_STOP gs[5] = {{0.f, D2D1::ColorF(0, 0, 0, 1.0f)},
                                        {0.26f, D2D1::ColorF(0, 0, 0, 0.34f)},
                                        {0.5f, D2D1::ColorF(0, 0, 0, 0.14f)},
                                        {0.74f, D2D1::ColorF(0, 0, 0, 0.34f)},
                                        {1.f, D2D1::ColorF(0, 0, 0, 1.0f)}};
            ComPtr<ID2D1GradientStopCollection> gc;
            g_rt->CreateGradientStopCollection(gs, 5, &gc);
            ComPtr<ID2D1LinearGradientBrush> lb;
            if (gc)
                g_rt->CreateLinearGradientBrush(
                    D2D1::LinearGradientBrushProperties(
                        D2D1::Point2F(0, r.top), D2D1::Point2F(0, r.bottom)),
                    gc.Get(), &lb);
            ComPtr<ID2D1Layer> ly;
            g_rt->CreateLayer(nullptr, &ly);
            if (rg && lb && ly) {
                g_rt->PushLayer(D2D1::LayerParameters(
                                    D2D1::InfiniteRect(), rg.Get(),
                                    D2D1_ANTIALIAS_MODE_PER_PRIMITIVE,
                                    D2D1::IdentityMatrix(), 1.0f, lb.Get()),
                                ly.Get());
                static const float opk[3] = {.72f, .56f, .42f};
                for (int k = 0; k < 3; ++k)
                    for (int ph = 0; ph < 2; ++ph) {
                        Schicht(k, r.left, r.top, t, opk[k] * 1.45f * g_dichteF,
                                ph);
                        g_rt->FillRectangle(r, g_satz.br[g_ti][k].Get());
                    }
                g_rt->PopLayer();
            }
        }
    } else if (g_modus == 2) {  // simulation with mask
        // Real smoke (fluid simulation per pill): rises, swirls, drifts over
        // the edge, reacts to the mouse
        {
            static LARGE_INTEGER fq{}, lastS{};
            LARGE_INTEGER nowS;
            QueryPerformanceCounter(&nowS);
            if (!fq.QuadPart)
                QueryPerformanceFrequency(&fq);
            const float dtS =
                lastS.QuadPart
                    ? std::min(0.05f, (float)(nowS.QuadPart - lastS.QuadPart) /
                                          (float)fq.QuadPart)
                    : 1.f / 60.f;
            lastS = nowS;  // precise timing (no 15.6 ms timer granularity)
            static POINT mp{-99999, -99999};
            POINT cp{};
            GetCursorPos(&cp);
            const float mvx = (float)(cp.x - mp.x), mvy = (float)(cp.y - mp.y);
            const bool mOk = mp.x > -99990;
            mp = cp;
            if (v211::g_fl.size() != g_pillen.size())
                v211::g_fl.assign(g_pillen.size(), v211::Fluid());
            for (size_t pi = 0; pi < g_pillen.size(); ++pi) {
                const Pille& p = g_pillen[pi];
                v211::Fluid& F = v211::g_fl[pi];
                const float gx0 = p.l - v211::FL_MX,
                            gy0 = p.t - v211::FL_MY;
                const int W = (int)std::ceil((p.r - p.l + 2 * v211::FL_MX) /
                                             v211::FL_C),
                          H = (int)std::ceil((p.b - p.t + 2 * v211::FL_MY) /
                                             v211::FL_C);
                if (W != F.W || H != F.H)
                    F.Init(W, H, (float)pi * 41.7f);
                v211::FluidSchritt(
                    F, dtS * g_tempoF, t, (cp.x - gx0) / v211::FL_C,
                    (cp.y - gy0) / v211::FL_C, mOk ? mvx / v211::FL_C : 0.f,
                    mOk ? mvy / v211::FL_C : 0.f, v211::FL_MX / v211::FL_C,
                    v211::FL_MY / v211::FL_C, W - v211::FL_MX / v211::FL_C,
                    H - v211::FL_MY / v211::FL_C);
                v211::FluidMalen(
                    F, g_rt.Get(),
                    D2D1::RectF(gx0 - x0, gy0 - y0, gx0 - x0 + W * v211::FL_C,
                                gy0 - y0 + H * v211::FL_C),
                    g_ti);
            }
        }
    } else if (g_modus == 3) {  // simulation carries the texture
        // Real smoke (fluid simulation per pill): rises, swirls, drifts over
        // the edge, reacts to the mouse
        {
            static LARGE_INTEGER fq{}, lastS{};
            LARGE_INTEGER nowS;
            QueryPerformanceCounter(&nowS);
            if (!fq.QuadPart)
                QueryPerformanceFrequency(&fq);
            const float dtS =
                lastS.QuadPart
                    ? std::min(0.05f, (float)(nowS.QuadPart - lastS.QuadPart) /
                                          (float)fq.QuadPart)
                    : 1.f / 60.f;
            lastS = nowS;  // precise timing (no 15.6 ms timer granularity)
            static POINT mp{-99999, -99999};
            POINT cp{};
            GetCursorPos(&cp);
            const float mvx = (float)(cp.x - mp.x), mvy = (float)(cp.y - mp.y);
            const bool mOk = mp.x > -99990;
            mp = cp;
            if (v220::g_fl.size() != g_pillen.size())
                v220::g_fl.assign(g_pillen.size(), v220::Fluid());
            for (size_t pi = 0; pi < g_pillen.size(); ++pi) {
                const Pille& p = g_pillen[pi];
                v220::Fluid& F = v220::g_fl[pi];
                const float gx0 = p.l - v220::FL_MX,
                            gy0 = p.t - v220::FL_MY;
                const int W = (int)std::ceil((p.r - p.l + 2 * v220::FL_MX) /
                                             v220::FL_C),
                          H = (int)std::ceil((p.b - p.t + 2 * v220::FL_MY) /
                                             v220::FL_C);
                if (W != F.W || H != F.H)
                    F.Init(W, H, (float)pi * 41.7f);
                v220::FluidSchritt(
                    F, dtS * g_tempoF, t, (cp.x - gx0) / v220::FL_C,
                    (cp.y - gy0) / v220::FL_C, mOk ? mvx / v220::FL_C : 0.f,
                    mOk ? mvy / v220::FL_C : 0.f, v220::FL_MX / v220::FL_C,
                    v220::FL_MY / v220::FL_C, W - v220::FL_MX / v220::FL_C,
                    H - v220::FL_MY / v220::FL_C);
                v220::FluidMalen(
                    F, g_rt.Get(),
                    D2D1::RectF(gx0 - x0, gy0 - y0, gx0 - x0 + W * v220::FL_C,
                                gy0 - y0 + H * v220::FL_C),
                    g_ti);
            }
        }
    } else if (g_modus == 4) {  // simulation carries the texture, subtle
        // Real smoke (fluid simulation per pill): rises, swirls, drifts over
        // the edge, reacts to the mouse
        {
            static LARGE_INTEGER fq{}, lastS{};
            LARGE_INTEGER nowS;
            QueryPerformanceCounter(&nowS);
            if (!fq.QuadPart)
                QueryPerformanceFrequency(&fq);
            const float dtS =
                lastS.QuadPart
                    ? std::min(0.05f, (float)(nowS.QuadPart - lastS.QuadPart) /
                                          (float)fq.QuadPart)
                    : 1.f / 60.f;
            lastS = nowS;  // precise timing (no 15.6 ms timer granularity)
            static POINT mp{-99999, -99999};
            POINT cp{};
            GetCursorPos(&cp);
            const float mvx = (float)(cp.x - mp.x), mvy = (float)(cp.y - mp.y);
            const bool mOk = mp.x > -99990;
            mp = cp;
            if (v221::g_fl.size() != g_pillen.size())
                v221::g_fl.assign(g_pillen.size(), v221::Fluid());
            for (size_t pi = 0; pi < g_pillen.size(); ++pi) {
                const Pille& p = g_pillen[pi];
                v221::Fluid& F = v221::g_fl[pi];
                const float gx0 = p.l - v221::FL_MX,
                            gy0 = p.t - v221::FL_MY;
                const int W = (int)std::ceil((p.r - p.l + 2 * v221::FL_MX) /
                                             v221::FL_C),
                          H = (int)std::ceil((p.b - p.t + 2 * v221::FL_MY) /
                                             v221::FL_C);
                if (W != F.W || H != F.H)
                    F.Init(W, H, (float)pi * 41.7f);
                v221::FluidSchritt(
                    F, dtS * g_tempoF, t, (cp.x - gx0) / v221::FL_C,
                    (cp.y - gy0) / v221::FL_C, mOk ? mvx / v221::FL_C : 0.f,
                    mOk ? mvy / v221::FL_C : 0.f, v221::FL_MX / v221::FL_C,
                    v221::FL_MY / v221::FL_C, W - v221::FL_MX / v221::FL_C,
                    H - v221::FL_MY / v221::FL_C);
                v221::FluidMalen(
                    F, g_rt.Get(),
                    D2D1::RectF(gx0 - x0, gy0 - y0, gx0 - x0 + W * v221::FL_C,
                                gy0 - y0 + H * v221::FL_C),
                    g_ti);
            }
        }
    } else
    // Real smoke (fluid simulation per pill): rises, swirls, drifts over the
    // edge, reacts to the mouse
    {
        static LARGE_INTEGER fq{}, lastS{};
        LARGE_INTEGER nowS;
        QueryPerformanceCounter(&nowS);
        if (!fq.QuadPart)
            QueryPerformanceFrequency(&fq);
        const float dtS =
            lastS.QuadPart
                ? std::min(0.05f, (float)(nowS.QuadPart - lastS.QuadPart) /
                                      (float)fq.QuadPart)
                : 1.f / 60.f;
        lastS = nowS;  // precise timing (no 15.6 ms timer granularity)
        static POINT mp{-99999, -99999};
        POINT cp{};
        GetCursorPos(&cp);
        const float mvx = (float)(cp.x - mp.x), mvy = (float)(cp.y - mp.y);
        const bool mOk = mp.x > -99990;
        mp = cp;
        if (g_fl.size() != g_pillen.size())
            g_fl.assign(g_pillen.size(), Fluid());
        for (size_t pi = 0; pi < g_pillen.size(); ++pi) {
            const Pille& p = g_pillen[pi];
            Fluid& F = g_fl[pi];
            const float gx0 = p.l - FL_MX, gy0 = p.t - FL_MY;
            const int W = (int)std::ceil((p.r - p.l + 2 * FL_MX) / FL_C),
                      H = (int)std::ceil((p.b - p.t + 2 * FL_MY) / FL_C);
            if (W != F.W || H != F.H)
                F.Init(W, H, (float)pi * 41.7f);
            FluidSchritt(F, dtS * g_tempoF, t, (cp.x - gx0) / FL_C,
                         (cp.y - gy0) / FL_C, mOk ? mvx / FL_C : 0.f,
                         mOk ? mvy / FL_C : 0.f, FL_MX / FL_C, FL_MY / FL_C,
                         W - FL_MX / FL_C, H - FL_MY / FL_C);
            FluidMalen(F, g_rt.Get(),
                       D2D1::RectF(gx0 - x0, gy0 - y0, gx0 - x0 + W * FL_C,
                                   gy0 - y0 + H * FL_C),
                       g_ti);
        }
    }
    {  // Sparks (rising from the smoke) and mouse swirl
        struct Funke {
            float x, y, vx, vy, l, m;
        };
        static std::vector<Funke> fk;
        static ULONGLONG lastF = 0;
        static float acc = 0;
        const ULONGLONG nowF = GetTickCount64();
        const float dtF = lastF ? std::min(0.1f, (nowF - lastF) / 1000.f) : 0.f;
        lastF = nowF;
        float breite = 0;
        for (auto& p : g_pillen)
            breite += p.r - p.l;
        acc += g_partikel ? dtF * (1.2f + breite / 900.f) : 0.f;
        while (acc >= 1.f && !g_pillen.empty()) {
            acc -= 1.f;
            float tot = breite, pick = (float)(rand() % 10000) / 10000.f * tot;
            const Pille* sel = &g_pillen[0];
            for (auto& p : g_pillen) {
                if (pick < p.r - p.l) {
                    sel = &p;
                    break;
                }
                pick -= p.r - p.l;
            }
            const float rr0 = (sel->b - sel->t) * .5f,
                        xx = sel->l + rr0 +
                             (float)(rand() % 10000) / 10000.f *
                                 std::max(1.f, sel->r - sel->l - 2 * rr0);
            fk.push_back({xx, sel->t + (float)(rand() % 8),
                          ((float)(rand() % 200) - 100.f) / 14.f,
                          -(10.f + (float)(rand() % 14)), 0.f,
                          2.6f + (float)(rand() % 140) / 100.f});
        }
        for (size_t i = 0; i < fk.size();) {
            Funke& f = fk[i];
            f.l += dtF;
            f.x += (f.vx + 6.f * std::sin(f.l * 3.f + f.x)) * dtF;
            f.y += f.vy * dtF;
            if (f.l >= f.m || f.y < y0 + 6)
                fk.erase(fk.begin() + i);
            else
                ++i;
        }
        // Tiny dark clouds rise slowly from the taskbar (calm, ~30 fps), violet
        // glow, black fill
        WolkenBauen();
        float glow[kSlots][3];
        for (int q = 0; q < kSlots; ++q) {
            float bc[3];
            BasisFarbe(q, bc);
            for (int c = 0; c < 3; ++c)
                glow[q][c] = std::min(255.f, bc[c] * 1.55f + 40.f);
        }
        D2D1_MATRIX_3X2_F altT;
        g_rt->GetTransform(&altT);
        for (auto& f : fk) {
            const float u = f.l / f.m,
                        a = std::pow(std::sin(3.14159f * u), 0.8f) *
                            std::clamp((f.y - (y0 + 6)) / 18.f, 0.f, 1.f);
            if (a < .02f)
                continue;
            const Wolke& wk = g_wk[((int)(f.m * 10) + (int)f.x) & 3];
            if (!wk.form)
                continue;
            const float r =
                3.2f +
                f.m * 1.5f * (0.6f + 0.5f * u);  // grows slightly while rising
            static ComPtr<ID2D1SolidColorBrush> fl, gl, gl2;
            static const void* bOwner = nullptr;
            if (bOwner != (const void*)g_rt.Get()) {
                fl.Reset();
                gl.Reset();
                gl2.Reset();
                bOwner = g_rt.Get();
            }
            if (!fl)
                g_rt->CreateSolidColorBrush(D2D1::ColorF(0, 0, 0, 1), &fl);
            if (!gl)
                g_rt->CreateSolidColorBrush(D2D1::ColorF(0, 0, 0, 1), &gl);
            if (!gl2)
                g_rt->CreateSolidColorBrush(D2D1::ColorF(0, 0, 0, 1), &gl2);
            if (fl)
                fl->SetColor(D2D1::ColorF(.03f, .012f, .05f, .95f * a));
            if (gl)
                gl->SetColor(D2D1::ColorF(glow[g_ti][0] / 255.f,
                                          glow[g_ti][1] / 255.f,
                                          glow[g_ti][2] / 255.f, .55f * a));
            if (gl2)
                gl2->SetColor(D2D1::ColorF(glow[g_ti][0] / 255.f,
                                           glow[g_ti][1] / 255.f,
                                           glow[g_ti][2] / 255.f, .14f * a));
            g_rt->SetTransform(
                D2D1::Matrix3x2F::Rotation(f.l * 28.f * (f.vx > 0 ? 1.f : -1.f),
                                           D2D1::Point2F(0, 0)) *
                D2D1::Matrix3x2F::Scale(r, r) *
                D2D1::Matrix3x2F::Translation(f.x - x0, f.y - y0) * altT);
            if (gl2)
                g_rt->DrawGeometry(wk.form.Get(), gl2.Get(),
                                   5.f / r);  // soft glow
            if (fl)
                g_rt->FillGeometry(wk.form.Get(), fl.Get());
            if (gl)
                g_rt->DrawGeometry(wk.form.Get(), gl.Get(), 1.1f / r);
        }
        g_rt->SetTransform(altT);
    }
    if (FAILED(g_rt->EndDraw())) {
        g_rt.Reset();
        return;
    }
    if (g_modus == 1 && g_mausF > 0.f && g_bits &&
        GdiFlush()) {  // Texture smoke yields to the cursor (trail fades, smoke
                       // flows back within ~0.9 s)
        struct Sp {
            float x, y;
            ULONGLONG t;
        };
        static Sp spur[20];
        static int sn = 0;
        POINT cp{};
        GetCursorPos(&cp);
        const ULONGLONG jetzt = GetTickCount64();
        const float cx = (float)(cp.x - x0), cy = (float)(cp.y - y0);
        if (sn == 0 ||
            std::hypot(cx + x0 - spur[(sn - 1) % 20].x,
                       cy + y0 - spur[(sn - 1) % 20].y) > 4.f ||
            jetzt - spur[(sn - 1) % 20].t > 120) {
            spur[sn % 20] = {(float)cp.x, (float)cp.y, jetzt};
            ++sn;
        }
        const float R0 = 40.f * g_mausF * g_reichF;
        uint32_t* px = static_cast<uint32_t*>(g_bits);
        for (int k = 0; k < std::min(sn, 20); ++k) {
            const Sp& s = spur[k];
            const float alter = (jetzt - s.t) / 900.f;
            if (alter >= 1.f)
                continue;
            const float R = R0 * (1.f - .45f * alter),
                        staerke = (1.f - alter) * (1.f - alter);
            const float sx = s.x - x0, sy = s.y - y0;
            const int ax = std::max(0, (int)(sx - R)),
                      bx = std::min(w - 1, (int)(sx + R)),
                      ay = std::max(0, (int)(sy - R)),
                      by = std::min(h - 1, (int)(sy + R));
            for (int y = ay; y <= by; ++y)
                for (int x = ax; x <= bx; ++x) {
                    const float d = std::hypot(x - sx, y - sy) / R;
                    if (d >= 1.f)
                        continue;
                    const float sm = d * d * (3.f - 2.f * d),
                                f = 1.f - staerke * (1.f - sm) * .92f;
                    uint32_t& q = px[y * w + x];
                    if (!q)
                        continue;
                    const uint32_t a = (uint32_t)((q >> 24) * f),
                                   r = (uint32_t)(((q >> 16) & 255) * f),
                                   g = (uint32_t)(((q >> 8) & 255) * f),
                                   b = (uint32_t)((q & 255) * f);
                    q = (a << 24) | (r << 16) | (g << 8) | b;
                }
        }
    }
    if (g_bits && (g_opF > 1.f || (g_farbe2 >= 0 && g_ti == (int)g_statusTi)) &&
        GdiFlush()) {  // second color along each pill, opacity above 1
        const bool zwei = g_farbe2 >= 0 && g_ti == (int)g_statusTi;
        float c1[3], c2[3];
        BasisFarbe(g_ti, c1);
        ToneRgb(c1);
        BasisFarbe(g_farbe2 >= 0 ? g_farbe2 : 0, c2);
        ToneRgb(c2);
        const float lum1 =
            std::max(1.f, .3f * c1[0] + .59f * c1[1] + .11f * c1[2]);
        std::vector<float> tcol(w, 0.f);
        if (zwei)
            for (int x = 0; x < w; ++x) {
                const float ax = (float)(x + x0);
                float best = 1e9f, tt = 0.f;
                for (auto& p : g_pillen) {
                    const float dx = ax < p.l   ? p.l - ax
                                     : ax > p.r ? ax - p.r
                                                : 0.f;
                    if (dx < best) {
                        best = dx;
                        tt = std::clamp((ax - p.l) / std::max(1.f, p.r - p.l),
                                        0.f, 1.f);
                    }
                }
                tcol[x] = tt;
            }
        const float gain = std::max(1.f, g_opF);
        uint32_t* px = static_cast<uint32_t*>(g_bits);
        for (int y = 0; y < h; ++y)
            for (int x = 0; x < w; ++x) {
                uint32_t& q = px[(size_t)y * w + x];
                const uint32_t a = q >> 24;
                if (!a)
                    continue;
                float r = (float)((q >> 16) & 255), g = (float)((q >> 8) & 255),
                      b = (float)(q & 255);
                if (zwei && tcol[x] > .004f) {
                    const float inv = 255.f / a;
                    const float ur = r * inv, ug = g * inv, ub = b * inv;
                    const float sl = (.3f * ur + .59f * ug + .11f * ub) / lum1;
                    const float k = tcol[x];
                    r = (ur + (std::min(255.f, c2[0] * sl) - ur) * k) * a /
                        255.f;
                    g = (ug + (std::min(255.f, c2[1] * sl) - ug) * k) * a /
                        255.f;
                    b = (ub + (std::min(255.f, c2[2] * sl) - ub) * k) * a /
                        255.f;
                }
                float na = (float)a;
                if (gain > 1.f) {
                    na = std::min(255.f, na * gain);
                    const float sc = na / (float)a;
                    r *= sc;
                    g *= sc;
                    b *= sc;
                }
                r = std::min(r, na);
                g = std::min(g, na);
                b = std::min(b, na);
                q = ((uint32_t)(na + .5f) << 24) | ((uint32_t)(r + .5f) << 16) |
                    ((uint32_t)(g + .5f) << 8) | (uint32_t)(b + .5f);
            }
    }
    POINT dst{x0, y0}, src{0, 0};
    SIZE sz{w, h};
    BLENDFUNCTION bf{AC_SRC_OVER, 0,
                     (BYTE)std::lround(255 * std::clamp(g_blende, 0.f, 1.f) *
                                       std::min(1.f, g_opF)),
                     AC_SRC_ALPHA};
    UpdateLayeredWindow(g_hwnd, nullptr, &dst, &sz, g_memdc, &src, 0, &bf,
                        ULW_ALPHA);
}
bool Vollbild() {
    // Fullscreen = the foreground window covers its whole monitor (desktop and
    // taskbar never count)
    QUERY_USER_NOTIFICATION_STATE q{};
    if (SUCCEEDED(SHQueryUserNotificationState(&q)) &&
        (q == QUNS_RUNNING_D3D_FULL_SCREEN || q == QUNS_PRESENTATION_MODE))
        return true;
    HWND fg = GetForegroundWindow();
    if (!fg)
        return false;
    wchar_t cls[64] = {};
    GetClassNameW(fg, cls, 64);
    if (!wcscmp(cls, L"Progman") || !wcscmp(cls, L"WorkerW") ||
        !wcscmp(cls, L"Shell_TrayWnd") ||
        !wcscmp(cls, L"Shell_SecondaryTrayWnd"))
        return false;
    RECT wr{};
    MONITORINFO mi{sizeof(mi)};
    if (!GetWindowRect(fg, &wr) ||
        !GetMonitorInfoW(MonitorFromWindow(fg, MONITOR_DEFAULTTONEAREST), &mi))
        return false;
    return wr.left <= mi.rcMonitor.left && wr.top <= mi.rcMonitor.top &&
           wr.right >= mi.rcMonitor.right && wr.bottom >= mi.rcMonitor.bottom;
}
bool NahAnLeiste() {
    RECT wr{};
    POINT c{};
    HWND tr = FindWindowW(L"Shell_TrayWnd", nullptr);
    return tr && GetWindowRect(tr, &wr) && GetCursorPos(&c) &&
           c.y >= wr.top - 160 && c.y <= wr.bottom + 40;
}

DWORD WINAPI UiaThread(LPVOID) {
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (FAILED(CoCreateInstance(kClsidUIA, nullptr, CLSCTX_INPROC_SERVER,
                                kIidUIA, (void**)&g_ua))) {
        return 1;
    }
    while (!g_uiaStop) {
        RECT wr{};
        HWND tr = FindWindowW(L"Shell_TrayWnd", nullptr);
        const bool sichtbar =
            tr && GetWindowRect(tr, &wr) && wr.top < MonitorBottom(tr) - 6;
        bool leer;
        {
            EnterCriticalSection(&g_lock);
            leer = g_pillenQ.empty();
            LeaveCriticalSection(&g_lock);
        }
        if (sichtbar && leer) {
            Sleep(250);
            PillenLesen();
        }  // no pills yet (auto-hide: taskbar only visible while the mouse is
           // on it) -> read anyway; otherwise skip while the overlay is
           // hidden
        else if (sichtbar && g_overlaySichtbar && !NahAnLeiste()) {
            Sleep(700);
            if (NahAnLeiste())
                continue;
            PillenLesen();
        }  // not while hovering (dock magnification)
        for (int i = 0; i < 10 && !g_uiaStop; ++i)
            Sleep(100);
    }
    g_frame.Reset();
    g_ua.Reset();
    CoUninitialize();
    return 0;
}
void TimerTakt(bool sichtbar) {  // 15 ms while shown, 250 ms while hidden
    static bool zuletzt = true;
    g_overlaySichtbar = sichtbar;
    if (sichtbar == zuletzt)
        return;
    zuletzt = sichtbar;
    SetTimer(g_hwnd, 1, sichtbar ? 15 : 250, nullptr);
}
void Tick() {
    EffekteLesen();  // in every style
    const ULONGLONG now = GetTickCount64();
    // Dock magnification (icons grow on hover): don't re-read pill widths while
    // the cursor is on the taskbar No UIA call in Tick (it blocked ~300 ms
    // while the taskbar slid -> overlay froze). UIA runs on its own thread.
    float offT, offB;
    {
        EnterCriticalSection(&g_lock);
        const bool neu = InterlockedExchange(&g_uiaBereit, 0) != 0;
        auto q = g_pillenQ;
        offT = g_offT;
        offB = g_offB;
        LeaveCriticalSection(&g_lock);
        if (neu) {
            for (size_t i = 0; i < q.size(); ++i)
                if (i < g_pillen.size()) {
                    q[i].t = g_pillen[i].t;
                    q[i].b = g_pillen[i].b;
                }
            g_pillen = q;
        }
    }
    const HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr);
    bool da = !g_pillen.empty() && !Vollbild() && IsWindowVisible(tray);
    // Position = taskbar WINDOW (Shell_TrayWnd). It slides visibly when
    // hiding/showing (~160-280 ms); dock magnification does not change it. (UIA
    // only reports final positions.) The overlay follows the window frame by
    // frame.
    if (da) {
        RECT wr{};
        if (tray && GetWindowRect(tray, &wr) && wr.bottom > wr.top) {
            const int mb = MonitorBottom(tray);
            if (wr.top >= mb - 10)
                da = false;  // almost fully hidden: gone
            else
                for (auto& p : g_pillen) {
                    p.t = (float)wr.top + offT;
                    p.b = (float)wr.bottom - offB;
                }
        } else
            da = false;
    }
    const bool farbeWechselt = g_ti != (int)g_statusTi;
    const float ziel = (da && !farbeWechselt) ? 1.f : 0.f;
    if (!da)
        g_blende = 0.f;
    else
        g_blende += (ziel - g_blende) * 0.35f;
    if (farbeWechselt && g_blende < 0.06f)
        g_ti = (int)g_statusTi;  // color change: fade out, switch, fade back in
    if (g_blende < 0.01f) {
        ShowWindow(g_hwnd, SW_HIDE);
        TimerTakt(false);
        return;
    }
    TimerTakt(true);
    if (!IsWindowVisible(g_hwnd))
        ShowWindow(g_hwnd, SW_SHOWNOACTIVATE);
    static ULONGLONG zLetzt = 0;
    if (now - zLetzt > 1500) {
        zLetzt = now;
        SetWindowPos(g_hwnd, HWND_TOPMOST, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    }
    // draw every frame (60/s), otherwise the smoke stutters
    Zeichnen();
}
LRESULT CALLBACK Proc(HWND h, UINT m, WPARAM w, LPARAM l) {
    if (m == WM_TIMER) {
        Tick();
        return 0;
    }
    if (m == WM_NCHITTEST)
        return HTTRANSPARENT;
    if (m == WM_DESTROY)
        return 0;
    return DefWindowProcW(h, m, w, l);
}

static HANDLE g_thread = nullptr;
static DWORD g_threadId = 0;
DWORD WINAPI RauchThread(LPVOID) {
    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    SetThreadDpiAwarenessContext(
        DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);  // windhawk.exe has its
                                                      // own DPI setting -> make
                                                      // this thread physically
                                                      // DPI aware
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    InitializeCriticalSection(&g_lock);
    g_uiaThread = CreateThread(nullptr, 0, UiaThread, nullptr, 0, nullptr);
    Rauschen();
    HINSTANCE inst = GetModuleHandleW(nullptr);
    WNDCLASSEXW wc{sizeof(wc)};
    wc.hInstance = inst;
    wc.lpfnWndProc = Proc;
    wc.lpszClassName = L"RQTaskleisteRauchWH";
    RegisterClassExW(&wc);
    g_hwnd = CreateWindowExW(WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST |
                                 WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
                             wc.lpszClassName, L"", WS_POPUP, 0, 0, 1, 1,
                             nullptr, nullptr, inst, nullptr);
    if (!g_hwnd)
        return 1;
    SetTimer(g_hwnd, 1, 15, nullptr);
    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    g_uiaStop = true;
    KillTimer(g_hwnd, 1);
    DestroyWindow(g_hwnd);
    g_hwnd = nullptr;
    UnregisterClassW(wc.lpszClassName, inst);
    if (g_uiaThread) {
        WaitForSingleObject(g_uiaThread, 1500);
        CloseHandle(g_uiaThread);
        g_uiaThread = nullptr;
    }
    g_fl.clear();
    g_fl.shrink_to_fit();
    for (auto& r : g_satz.br)
        for (auto& b : r)
            b.Reset();
    for (auto& r : g_satz.bmp)
        for (auto& b : r)
            b.Reset();
    for (auto& w : g_wk)
        w.form.Reset();
    g_rt.Reset();
    if (g_fab) {
        g_fab->Release();
        g_fab = nullptr;
    }
    if (g_dib) {
        DeleteObject(g_dib);
        g_dib = nullptr;
    }
    if (g_memdc) {
        DeleteDC(g_memdc);
        g_memdc = nullptr;
    }
    DeleteCriticalSection(&g_lock);
    CoUninitialize();
    return 0;
}
}  // namespace

BOOL WhTool_ModInit() {
    g_thread = CreateThread(nullptr, 0, RauchThread, nullptr, 0, &g_threadId);
    return g_thread != nullptr;
}
void WhTool_ModSettingsChanged() {
    g_einstellungNeu = true;
}
void WhTool_ModUninit() {
    // Own -tool-mod process: stop the window thread, then terminate the process
    // hard (UIA teardown on normal exit -> access violation in
    // uiautomationcore)
    g_uiaStop = true;
    if (g_thread) {
        PostThreadMessageW(g_threadId, WM_QUIT, 0, 0);
        WaitForSingleObject(g_thread, 3000);
    }
    TerminateProcess(GetCurrentProcess(), 0);
}

// ---- Windhawk tool-mod launcher (runs the mod in its own windhawk.exe
// -tool-mod process) ----
bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

BOOL Wh_ModInit() {
    DWORD sessionId;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) &&
        sessionId == 0) {
        return FALSE;
    }

    bool isExcluded = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
    if (!argv) {
        Wh_Log(L"CommandLineToArgvW failed");
        return FALSE;
    }

    for (int i = 1; i < argc; i++) {
        if (wcscmp(argv[i], L"-service") == 0 ||
            wcscmp(argv[i], L"-service-start") == 0 ||
            wcscmp(argv[i], L"-service-stop") == 0) {
            isExcluded = true;
            break;
        }
    }

    for (int i = 1; i < argc - 1; i++) {
        if (wcscmp(argv[i], L"-tool-mod") == 0) {
            isToolModProcess = true;
            if (wcscmp(argv[i + 1], WH_MOD_ID) == 0) {
                isCurrentToolModProcess = true;
            }
            break;
        }
    }

    LocalFree(argv);

    if (isExcluded) {
        return FALSE;
    }

    if (isCurrentToolModProcess) {
        g_toolModProcessMutex =
            CreateMutex(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
        if (!g_toolModProcessMutex) {
            Wh_Log(L"CreateMutex failed");
            ExitProcess(1);
        }

        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            Wh_Log(L"Tool mod already running (%s)", WH_MOD_ID);
            ExitProcess(1);
        }

        if (!WhTool_ModInit()) {
            ExitProcess(1);
        }

        IMAGE_DOS_HEADER* dosHeader =
            (IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);
        IMAGE_NT_HEADERS* ntHeaders =
            (IMAGE_NT_HEADERS*)((BYTE*)dosHeader + dosHeader->e_lfanew);

        DWORD entryPointRVA = ntHeaders->OptionalHeader.AddressOfEntryPoint;
        void* entryPoint = (BYTE*)dosHeader + entryPointRVA;

        Wh_SetFunctionHook(entryPoint, (void*)EntryPoint_Hook, nullptr);
        return TRUE;
    }

    if (isToolModProcess) {
        return FALSE;
    }

    g_isToolModProcessLauncher = true;
    return TRUE;
}

void Wh_ModAfterInit() {
    if (!g_isToolModProcessLauncher) {
        return;
    }

    WCHAR currentProcessPath[MAX_PATH];
    switch (GetModuleFileName(nullptr, currentProcessPath,
                              ARRAYSIZE(currentProcessPath))) {
        case 0:
        case ARRAYSIZE(currentProcessPath):
            Wh_Log(L"GetModuleFileName failed");
            return;
    }

    WCHAR
    commandLine[MAX_PATH + 2 +
                (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) - 1];
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", currentProcessPath,
               WH_MOD_ID);

    HMODULE kernelModule = GetModuleHandle(L"kernelbase.dll");
    if (!kernelModule) {
        kernelModule = GetModuleHandle(L"kernel32.dll");
        if (!kernelModule) {
            Wh_Log(L"No kernelbase.dll/kernel32.dll");
            return;
        }
    }

    using CreateProcessInternalW_t = BOOL(WINAPI*)(
        HANDLE hUserToken, LPCWSTR lpApplicationName, LPWSTR lpCommandLine,
        LPSECURITY_ATTRIBUTES lpProcessAttributes,
        LPSECURITY_ATTRIBUTES lpThreadAttributes, WINBOOL bInheritHandles,
        DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory,
        LPSTARTUPINFOW lpStartupInfo,
        LPPROCESS_INFORMATION lpProcessInformation,
        PHANDLE hRestrictedUserToken);
    CreateProcessInternalW_t pCreateProcessInternalW =
        (CreateProcessInternalW_t)GetProcAddress(kernelModule,
                                                 "CreateProcessInternalW");
    if (!pCreateProcessInternalW) {
        Wh_Log(L"No CreateProcessInternalW");
        return;
    }

    STARTUPINFO si{
        .cb = sizeof(STARTUPINFO),
        .dwFlags = STARTF_FORCEOFFFEEDBACK,
    };
    PROCESS_INFORMATION pi;
    if (!pCreateProcessInternalW(nullptr, currentProcessPath, commandLine,
                                 nullptr, nullptr, FALSE, NORMAL_PRIORITY_CLASS,
                                 nullptr, nullptr, &si, &pi, nullptr)) {
        Wh_Log(L"CreateProcess failed");
        return;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

void Wh_ModSettingsChanged() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModSettingsChanged();
}

void Wh_ModUninit() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModUninit();
    ExitProcess(0);
}
