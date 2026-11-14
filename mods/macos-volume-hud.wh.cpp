// ==WindhawkMod==
// @id              macos-volume-hud
// @name            macOS Volume HUD (Frosted Glass)
// @description     macOS style volume HUD for Windows 11: real frosted-glass blur, anti-aliased icon, quarter-step bar, soft shadow, DPI aware
// @version         5.0.0
// @author          DevPxxnpxn
// @include         explorer.exe
// @compilerOptions -lole32 -luuid -lgdi32 -luser32 -lshcore -ladvapi32 -ldwmapi
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModSettings==
/*
- volumeStep: 2
  $name: Volume step
  $description: Volume change per key press (%)
- hideDelay: 1500
  $name: Hide delay
  $description: HUD visibility duration (ms)
- bottomOffset: 120
  $name: Bottom offset
  $description: Distance from the bottom of the work area (px at 100% scale)
- opacity: 100
  $name: Opacity
  $description: Overall HUD opacity (30-100 %)
- theme: sierra
  $name: Theme
  $options:
  - sierra: Classic (light glass, dark icon)
  - dark: Dark glass
  - auto: Follow Windows app theme
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <shellscalingapi.h>
#include <dwmapi.h>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <cwchar>
#include <vector>

// --------------------------------------------------
// Constants
// --------------------------------------------------

static constexpr wchar_t WINDOW_CLASS[] = L"MacVolumeHUDGlass";
static constexpr UINT WM_VOLUME = WM_APP + 10;
static constexpr UINT TIMER_HIDE = 1;
static constexpr UINT TIMER_ANIMATE = 2;

static constexpr int   HUD_SIZE = 200;     // logical size of the square HUD
static constexpr float MARGIN   = 28.0f;   // logical margin around HUD for the shadow
static constexpr float CORNER   = 20.0f;   // logical corner radius

// --------------------------------------------------
// State
// --------------------------------------------------

static HWND   g_hwnd = nullptr;
static HHOOK  g_hook = nullptr;
static HANDLE g_thread = nullptr;
static HANDLE g_ready = nullptr;
static DWORD  g_threadId = 0;

static IAudioEndpointVolume* g_audio = nullptr;

// settings
static int g_step = 2;
static int g_hideDelay = 1500;
static int g_bottomOffset = 120;
static int g_targetAlpha = 255;
static int g_themeMode = 0;            // 0 sierra, 1 dark, 2 auto
static volatile bool g_baseDirty = true;

// runtime
static int  g_volume = 50;
static bool g_muted = false;
static int  g_alpha = 0;
static bool g_fadingOut = false;
static bool g_readySuccess = false;
static bool g_dark = false;

// geometry (physical pixels)
static float g_S = 1.0f;
static int g_hudPx = 0;
static int g_mPx = 0;
static int g_winW = 0;
static int g_winH = 0;
static int g_hudX = -100000;
static int g_hudY = -100000;

// surfaces
static HDC g_memDC = nullptr;
static HBITMAP g_dib = nullptr;
static HGDIOBJ g_oldBmp = nullptr;
static uint32_t* g_bits = nullptr;
static std::vector<uint32_t> g_bg;     // blurred backdrop (hudPx x hudPx)
static std::vector<uint32_t> g_base;   // premultiplied glass + shadow (winW x winH)
static bool g_haveBackdrop = false;

// --------------------------------------------------
// Small math helpers (avoid min/max macro issues)
// --------------------------------------------------

static inline float Clamp(float v, float lo, float hi) {
    return v < lo ? lo : (v > hi ? hi : v);
}

static inline float Clamp01(float v) {
    return Clamp(v, 0.0f, 1.0f);
}

static inline int ClampI(int v, int lo, int hi) {
    return v < lo ? lo : (v > hi ? hi : v);
}

struct Pt {
    float x, y;
};

// --------------------------------------------------
// Signed distance functions (logical units)
// --------------------------------------------------

static float sdBox(float px, float py, float cx, float cy, float hw, float hh, float r) {
    if (r > hw) r = hw;
    if (r > hh) r = hh;

    float qx = std::fabs(px - cx) - hw + r;
    float qy = std::fabs(py - cy) - hh + r;

    float ox = qx > 0.0f ? qx : 0.0f;
    float oy = qy > 0.0f ? qy : 0.0f;

    float m = qx > qy ? qx : qy;
    if (m > 0.0f) m = 0.0f;

    return std::sqrt(ox * ox + oy * oy) + m - r;
}

static float sdSegment(float px, float py, float ax, float ay, float bx, float by) {
    float pax = px - ax, pay = py - ay;
    float bax = bx - ax, bay = by - ay;
    float h = Clamp01((pax * bax + pay * bay) / (bax * bax + bay * bay));
    float dx = pax - bax * h;
    float dy = pay - bay * h;
    return std::sqrt(dx * dx + dy * dy);
}

static float sdPolygon(const Pt* v, int n, float px, float py) {
    float dx = px - v[0].x, dy = py - v[0].y;
    float d = dx * dx + dy * dy;
    float s = 1.0f;

    for (int i = 0, j = n - 1; i < n; j = i++) {
        float ex = v[j].x - v[i].x;
        float ey = v[j].y - v[i].y;
        float wx = px - v[i].x;
        float wy = py - v[i].y;

        float t = Clamp01((wx * ex + wy * ey) / (ex * ex + ey * ey));
        float bx = wx - ex * t;
        float by = wy - ey * t;
        float dd = bx * bx + by * by;
        if (dd < d) d = dd;

        bool c0 = py >= v[i].y;
        bool c1 = py < v[j].y;
        bool c2 = ex * wy > ey * wx;

        if ((c0 && c1 && c2) || (!c0 && !c1 && !c2)) s = -s;
    }

    return s * std::sqrt(d);
}

static inline float RoundRectSD(float hx, float hy, float yOffset) {
    return sdBox(hx, hy - yOffset, HUD_SIZE * 0.5f, HUD_SIZE * 0.5f,
                 HUD_SIZE * 0.5f, HUD_SIZE * 0.5f, CORNER);
}

// --------------------------------------------------
// Audio
// --------------------------------------------------

static bool AudioInit() {
    IMMDeviceEnumerator* enumerator = nullptr;
    IMMDevice* device = nullptr;

    HRESULT hr = CoCreateInstance(
        __uuidof(MMDeviceEnumerator),
        nullptr,
        CLSCTX_ALL,
        __uuidof(IMMDeviceEnumerator),
        reinterpret_cast<void**>(&enumerator)
    );

    if (FAILED(hr) || !enumerator) {
        return false;
    }

    hr = enumerator->GetDefaultAudioEndpoint(eRender, eMultimedia, &device);

    if (SUCCEEDED(hr)) {
        hr = device->Activate(
            __uuidof(IAudioEndpointVolume),
            CLSCTX_ALL,
            nullptr,
            reinterpret_cast<void**>(&g_audio)
        );
    }

    if (device) device->Release();
    enumerator->Release();

    return SUCCEEDED(hr) && g_audio;
}

static void AudioRead() {
    if (!g_audio) return;

    float volume = 0.0f;
    BOOL muted = FALSE;

    if (SUCCEEDED(g_audio->GetMasterVolumeLevelScalar(&volume))) {
        g_volume = static_cast<int>(volume * 100.0f + 0.5f);
    }

    if (SUCCEEDED(g_audio->GetMute(&muted))) {
        g_muted = muted != FALSE;
    }
}

static void AudioChange(WPARAM key) {
    if (!g_audio) return;

    if (key == VK_VOLUME_UP || key == VK_VOLUME_DOWN) {
        float volume = 0.0f;

        if (FAILED(g_audio->GetMasterVolumeLevelScalar(&volume))) {
            return;
        }

        float step = static_cast<float>(g_step) / 100.0f;
        volume += (key == VK_VOLUME_UP) ? step : -step;
        volume = Clamp01(volume);

        g_audio->SetMute(FALSE, nullptr);
        g_audio->SetMasterVolumeLevelScalar(volume, nullptr);
    } else if (key == VK_VOLUME_MUTE) {
        BOOL muted = FALSE;

        if (SUCCEEDED(g_audio->GetMute(&muted))) {
            g_audio->SetMute(!muted, nullptr);
        }
    }

    AudioRead();
}

// --------------------------------------------------
// Surface management
// --------------------------------------------------

static void FreeSurface() {
    if (g_memDC) {
        if (g_oldBmp) SelectObject(g_memDC, g_oldBmp);
        DeleteDC(g_memDC);
        g_memDC = nullptr;
    }

    if (g_dib) {
        DeleteObject(g_dib);
        g_dib = nullptr;
    }

    g_oldBmp = nullptr;
    g_bits = nullptr;
}

static bool AllocSurface() {
    FreeSurface();

    HDC screen = GetDC(nullptr);
    g_memDC = CreateCompatibleDC(screen);

    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = g_winW;
    bi.bmiHeader.biHeight = -g_winH;   // top-down
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;

    void* bits = nullptr;
    g_dib = CreateDIBSection(screen, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    ReleaseDC(nullptr, screen);

    if (!g_dib || !bits || !g_memDC) {
        FreeSurface();
        return false;
    }

    g_bits = static_cast<uint32_t*>(bits);
    g_oldBmp = SelectObject(g_memDC, g_dib);
    return true;
}

// Returns true when the geometry (size) changed
static bool SetupGeometry(HMONITOR monitor) {
    UINT dpiX = 96, dpiY = 96;

    if (FAILED(GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY))) {
        dpiX = 96;
    }

    float s = Clamp(static_cast<float>(dpiX) / 96.0f, 1.0f, 4.0f);

    int hudPx = static_cast<int>(std::lround(HUD_SIZE * s));
    int mPx = static_cast<int>(std::lround(MARGIN * s));

    bool changed = (hudPx != g_hudPx) || !g_dib;

    g_hudPx = hudPx;
    g_mPx = mPx;
    g_S = static_cast<float>(hudPx) / static_cast<float>(HUD_SIZE);
    g_winW = hudPx + 2 * mPx;
    g_winH = g_winW;

    return changed;
}

// --------------------------------------------------
// Backdrop capture + blur
// --------------------------------------------------

static void Blur1D(const uint32_t* src, uint32_t* dst, int n, int step, int r) {
    auto at = [&](int i) -> uint32_t {
        if (i < 0) i = 0;
        if (i >= n) i = n - 1;
        return src[static_cast<size_t>(i) * step];
    };

    int sb = 0, sg = 0, sr = 0;

    for (int i = -r; i <= r; i++) {
        uint32_t v = at(i);
        sb += v & 255;
        sg += (v >> 8) & 255;
        sr += (v >> 16) & 255;
    }

    const int div = 2 * r + 1;

    for (int i = 0; i < n; i++) {
        dst[static_cast<size_t>(i) * step] =
            0xFF000000u |
            (static_cast<uint32_t>(sr / div) << 16) |
            (static_cast<uint32_t>(sg / div) << 8) |
            static_cast<uint32_t>(sb / div);

        uint32_t a = at(i + r + 1);
        uint32_t b = at(i - r);

        sb += static_cast<int>(a & 255) - static_cast<int>(b & 255);
        sg += static_cast<int>((a >> 8) & 255) - static_cast<int>((b >> 8) & 255);
        sr += static_cast<int>((a >> 16) & 255) - static_cast<int>((b >> 16) & 255);
    }
}

static void FillFallbackBackdrop() {
    g_bg.assign(static_cast<size_t>(g_hudPx) * g_hudPx, 0xFF606068u);
    g_haveBackdrop = false;
}

static void CaptureBackdrop() {
    const int pad = static_cast<int>(std::lround(20.0f * g_S));
    const int cw = g_hudPx + 2 * pad;
    const int ch = cw;

    HDC screen = GetDC(nullptr);
    HDC mdc = CreateCompatibleDC(screen);

    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = cw;
    bi.bmiHeader.biHeight = -ch;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;

    void* bits = nullptr;
    HBITMAP bmp = CreateDIBSection(screen, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);

    bool ok = false;

    if (bmp && bits && mdc) {
        HGDIOBJ old = SelectObject(mdc, bmp);
        ok = BitBlt(mdc, 0, 0, cw, ch, screen, g_hudX - pad, g_hudY - pad, SRCCOPY) != FALSE;
        GdiFlush();

        if (ok) {
            std::vector<uint32_t> a(static_cast<uint32_t*>(bits),
                                    static_cast<uint32_t*>(bits) + static_cast<size_t>(cw) * ch);
            std::vector<uint32_t> b(a.size());

            const int radius = (std::max)(1, static_cast<int>(std::lround(9.0f * g_S)));

            for (int pass = 0; pass < 3; pass++) {
                for (int y = 0; y < ch; y++) {
                    Blur1D(a.data() + static_cast<size_t>(y) * cw,
                           b.data() + static_cast<size_t>(y) * cw, cw, 1, radius);
                }
                for (int x = 0; x < cw; x++) {
                    Blur1D(b.data() + x, a.data() + x, ch, cw, radius);
                }
            }

            g_bg.resize(static_cast<size_t>(g_hudPx) * g_hudPx);

            for (int y = 0; y < g_hudPx; y++) {
                for (int x = 0; x < g_hudPx; x++) {
                    g_bg[static_cast<size_t>(y) * g_hudPx + x] =
                        a[static_cast<size_t>(y + pad) * cw + (x + pad)];
                }
            }

            g_haveBackdrop = true;
        }

        SelectObject(mdc, old);
    }

    if (bmp) DeleteObject(bmp);
    if (mdc) DeleteDC(mdc);
    ReleaseDC(nullptr, screen);

    if (!ok) {
        FillFallbackBackdrop();
    }
}

// --------------------------------------------------
// Theme
// --------------------------------------------------

static bool ResolveDark() {
    if (g_themeMode == 1) return true;
    if (g_themeMode == 0) return false;

    DWORD value = 1;
    DWORD size = sizeof(value);

    LSTATUS st = RegGetValueW(
        HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
        L"AppsUseLightTheme",
        RRF_RT_REG_DWORD,
        nullptr,
        &value,
        &size
    );

    return st == ERROR_SUCCESS && value == 0;
}

// --------------------------------------------------
// Glass base: blur + tint + rim light + soft shadow
// --------------------------------------------------

static void BuildBase() {
    g_dark = ResolveDark();
    g_baseDirty = false;

    float tR, tG, tB, tA, sat, rimA, shA;

    if (g_dark) {
        tR = 24; tG = 24; tB = 28;
        tA = 0.60f; sat = 1.35f; rimA = 0.22f; shA = 0.45f;
    } else {
        tR = 132; tG = 128; tB = 136;
        tA = 0.45f; sat = 1.30f; rimA = 0.30f; shA = 0.35f;
    }

    const float S = g_S;
    const float sigma = 7.0f;
    const float shOffset = 5.0f;

    g_base.assign(static_cast<size_t>(g_winW) * g_winH, 0u);

    for (int y = 0; y < g_winH; y++) {
        const float hy = (static_cast<float>(y) + 0.5f - g_mPx) / S;

        for (int x = 0; x < g_winW; x++) {
            const float hx = (static_cast<float>(x) + 0.5f - g_mPx) / S;

            const float d = RoundRectSD(hx, hy, 0.0f);
            const float cov = Clamp01(0.5f - d * S);

            const float ds = RoundRectSD(hx, hy, shOffset);
            float sh = 0.0f;

            if (ds < 3.0f * sigma) {
                float t = ds > 0.0f ? ds : 0.0f;
                sh = shA * std::exp(-(t * t) / (2.0f * sigma * sigma));
            }

            const float A = cov + sh * (1.0f - cov);
            if (A <= 0.002f) continue;

            float r = 0, g = 0, b = 0;

            if (cov > 0.0f) {
                int bx = ClampI(x - g_mPx, 0, g_hudPx - 1);
                int by = ClampI(y - g_mPx, 0, g_hudPx - 1);

                uint32_t v = g_bg[static_cast<size_t>(by) * g_hudPx + bx];
                float pb = static_cast<float>(v & 255);
                float pg = static_cast<float>((v >> 8) & 255);
                float pr = static_cast<float>((v >> 16) & 255);

                // saturation boost (vibrancy)
                float lum = 0.299f * pr + 0.587f * pg + 0.114f * pb;
                pr = lum + (pr - lum) * sat;
                pg = lum + (pg - lum) * sat;
                pb = lum + (pb - lum) * sat;

                // tint
                r = pr * (1.0f - tA) + tR * tA;
                g = pg * (1.0f - tA) + tG * tA;
                b = pb * (1.0f - tA) + tB * tA;

                // vertical light gradient
                float grad = (0.5f - hy / HUD_SIZE) * 16.0f;
                r += grad; g += grad; b += grad;

                // rim light (1.6 px inner edge)
                if (d < 0.0f) {
                    float rim = Clamp01(1.0f - (-d * S) / 1.6f) * rimA;
                    r += (255.0f - r) * rim;
                    g += (255.0f - g) * rim;
                    b += (255.0f - b) * rim;
                }

                r = Clamp(r, 0.0f, 255.0f) * cov;
                g = Clamp(g, 0.0f, 255.0f) * cov;
                b = Clamp(b, 0.0f, 255.0f) * cov;
            }

            g_base[static_cast<size_t>(y) * g_winW + x] =
                (static_cast<uint32_t>(A * 255.0f + 0.5f) << 24) |
                (static_cast<uint32_t>(r + 0.5f) << 16) |
                (static_cast<uint32_t>(g + 0.5f) << 8) |
                static_cast<uint32_t>(b + 0.5f);
        }
    }
}

// --------------------------------------------------
// Element drawing (anti-aliased via SDF)
// --------------------------------------------------

static inline void Over(uint32_t* p, float r, float g, float b, float a) {
    if (a <= 0.0f) return;

    uint32_t v = *p;
    float pb = static_cast<float>(v & 255);
    float pg = static_cast<float>((v >> 8) & 255);
    float pr = static_cast<float>((v >> 16) & 255);

    pr += (r - pr) * a;
    pg += (g - pg) * a;
    pb += (b - pb) * a;

    *p = 0xFF000000u |
         (static_cast<uint32_t>(pr + 0.5f) << 16) |
         (static_cast<uint32_t>(pg + 0.5f) << 8) |
         static_cast<uint32_t>(pb + 0.5f);
}

template <class F>
static void ForBox(float x0, float y0, float x1, float y1, F fn) {
    int px0 = ClampI(static_cast<int>(std::floor(x0 * g_S)) + g_mPx, 0, g_winW - 1);
    int py0 = ClampI(static_cast<int>(std::floor(y0 * g_S)) + g_mPx, 0, g_winH - 1);
    int px1 = ClampI(static_cast<int>(std::ceil(x1 * g_S)) + g_mPx, 0, g_winW - 1);
    int py1 = ClampI(static_cast<int>(std::ceil(y1 * g_S)) + g_mPx, 0, g_winH - 1);

    for (int y = py0; y <= py1; y++) {
        float hy = (static_cast<float>(y) + 0.5f - g_mPx) / g_S;

        for (int x = px0; x <= px1; x++) {
            float hx = (static_cast<float>(x) + 0.5f - g_mPx) / g_S;
            fn(g_bits + static_cast<size_t>(y) * g_winW + x, hx, hy);
        }
    }
}

static inline float Cov(float d) {
    return Clamp01(0.5f - d * g_S);
}

static void DrawElements() {
    float ir, ig, ib, ia;

    if (g_dark) {
        ir = 255; ig = 255; ib = 255; ia = 0.95f;
    } else {
        ir = 24; ig = 8; ib = 8; ia = 0.88f;
    }

    // ---- Speaker body (rounded polygon) ----
    static const Pt spk[6] = {
        {53, 85}, {64, 85}, {84, 66}, {84, 112}, {64, 94}, {53, 94}
    };

    ForBox(46, 56, 94, 122, [&](uint32_t* p, float hx, float hy) {
        float d = sdPolygon(spk, 6, hx, hy) - 3.2f;
        Over(p, ir, ig, ib, ia * Cov(d));
    });

    if (g_muted) {
        // ---- X mark ----
        ForBox(94, 72, 128, 106, [&](uint32_t* p, float hx, float hy) {
            float d1 = sdSegment(hx, hy, 98, 78, 120, 100);
            float d2 = sdSegment(hx, hy, 120, 78, 98, 100);
            float d = (d1 < d2 ? d1 : d2) - 2.6f;
            Over(p, ir, ig, ib, ia * Cov(d));
        });
    } else {
        // ---- Sound waves ----
        const float cx = 88.0f, cy = 89.0f;
        const float TH = 0.6632f;       // 38 degrees
        const float cosTH = 0.7880f;
        const float sinTH = 0.6157f;
        const float radii[3] = {26.0f, 45.0f, 64.0f};
        const int thresholds[3] = {1, 34, 67};

        for (int i = 0; i < 3; i++) {
            const float r = radii[i];
            const float alpha = (g_volume >= thresholds[i]) ? ia : ia * 0.22f;

            ForBox(cx + r * cosTH - 5, cy - r * sinTH - 5,
                   cx + r + 5, cy + r * sinTH + 5,
                   [&](uint32_t* p, float hx, float hy) {
                float qx = hx - cx, qy = hy - cy;
                float len = std::sqrt(qx * qx + qy * qy);
                float a = std::atan2(qy, qx);
                float d;

                if (std::fabs(a) <= TH) {
                    d = std::fabs(len - r);
                } else {
                    float ex = cx + r * cosTH;
                    float e1 = std::hypot(hx - ex, hy - (cy - r * sinTH));
                    float e2 = std::hypot(hx - ex, hy - (cy + r * sinTH));
                    d = e1 < e2 ? e1 : e2;
                }

                d -= 2.5f;
                Over(p, ir, ig, ib, alpha * Cov(d));
            });
        }
    }

    // ---- Segmented bar (16 segments, quarter-step fill) ----
    const float barX = 19.0f, barY = 172.0f, barW = 162.0f, barH = 10.0f;

    if (!g_dark) {
        ForBox(barX - 1, barY - 1, barX + barW + 1, barY + barH + 1,
               [&](uint32_t* p, float hx, float hy) {
            float d = sdBox(hx, hy, barX + barW * 0.5f, barY + barH * 0.5f,
                            barW * 0.5f, barH * 0.5f, 1.5f);
            Over(p, 0, 0, 0, 0.92f * Cov(d));
        });
    }

    const float level = g_muted ? 0.0f : static_cast<float>(g_volume) * 16.0f / 100.0f;

    for (int i = 0; i < 16; i++) {
        float f = Clamp01(level - static_cast<float>(i));
        f = std::floor(f * 4.0f + 0.5f) / 4.0f;   // quarter steps

        const float sx = 21.0f + i * 10.0f;
        const float sy = 177.0f;

        ForBox(sx - 1, sy - 4, sx + 9, sy + 4, [&](uint32_t* p, float hx, float hy) {
            if (g_dark) {
                float de = sdBox(hx, hy, sx + 4.0f, sy, 4.0f, 3.0f, 1.2f);
                Over(p, 255, 255, 255, 0.26f * Cov(de));
            }

            if (f > 0.0f) {
                float w = 8.0f * f;
                float df = sdBox(hx, hy, sx + w * 0.5f, sy, w * 0.5f, 3.0f, 1.0f);
                Over(p, 255, 255, 255, 0.97f * Cov(df));
            }
        });
    }
}

// --------------------------------------------------
// Present
// --------------------------------------------------

static void Present() {
    if (!g_hwnd || !g_memDC) return;

    SIZE size = {g_winW, g_winH};
    POINT src = {0, 0};
    POINT dst = {g_hudX - g_mPx, g_hudY - g_mPx};

    BLENDFUNCTION bf = {};
    bf.BlendOp = AC_SRC_OVER;
    bf.SourceConstantAlpha = static_cast<BYTE>(ClampI(g_alpha, 0, 255));
    bf.AlphaFormat = AC_SRC_ALPHA;

    UpdateLayeredWindow(g_hwnd, nullptr, &dst, &size, g_memDC, &src, 0, &bf, ULW_ALPHA);
}

static void RenderFrame() {
    if (!g_bits || g_base.size() != static_cast<size_t>(g_winW) * g_winH) return;

    std::memcpy(g_bits, g_base.data(), g_base.size() * sizeof(uint32_t));
    DrawElements();
}

// --------------------------------------------------
// Show HUD
// --------------------------------------------------

static void HUDShow() {
    if (!g_hwnd) return;

    KillTimer(g_hwnd, TIMER_HIDE);
    KillTimer(g_hwnd, TIMER_ANIMATE);

    g_fadingOut = false;

    POINT cursor = {};
    GetCursorPos(&cursor);

    HMONITOR monitor = MonitorFromPoint(cursor, MONITOR_DEFAULTTOPRIMARY);

    MONITORINFO mi = {};
    mi.cbSize = sizeof(mi);
    if (!GetMonitorInfoW(monitor, &mi)) return;

    const bool wasVisible = IsWindowVisible(g_hwnd) != FALSE;
    const bool sizeChanged = SetupGeometry(monitor);

    const int newX = mi.rcWork.left +
        ((mi.rcWork.right - mi.rcWork.left) - g_hudPx) / 2;
    const int newY = mi.rcWork.bottom - g_hudPx -
        static_cast<int>(std::lround(g_bottomOffset * g_S));

    const bool moved = (newX != g_hudX) || (newY != g_hudY);

    if (sizeChanged || moved || !wasVisible || !g_haveBackdrop) {
        if (wasVisible) {
            ShowWindow(g_hwnd, SW_HIDE);
            DwmFlush();   // make sure the HUD is gone before we capture
        }

        g_hudX = newX;
        g_hudY = newY;
        g_alpha = 0;

        if (sizeChanged || !g_dib) {
            AllocSurface();
        }

        CaptureBackdrop();
        BuildBase();
    } else if (g_baseDirty) {
        BuildBase();
    }

    RenderFrame();
    Present();

    if (!IsWindowVisible(g_hwnd)) {
        SetWindowPos(
            g_hwnd, HWND_TOPMOST, 0, 0, 0, 0,
            SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW
        );
    }

    SetTimer(g_hwnd, TIMER_ANIMATE, 16, nullptr);
    SetTimer(g_hwnd, TIMER_HIDE, static_cast<UINT>(g_hideDelay), nullptr);
}

// --------------------------------------------------
// Keyboard hook
// --------------------------------------------------

static LRESULT CALLBACK KeyboardProc(int code, WPARAM wParam, LPARAM lParam) {
    if (code == HC_ACTION) {
        const KBDLLHOOKSTRUCT* key = reinterpret_cast<const KBDLLHOOKSTRUCT*>(lParam);

        bool isVolumeKey =
            key->vkCode == VK_VOLUME_UP ||
            key->vkCode == VK_VOLUME_DOWN ||
            key->vkCode == VK_VOLUME_MUTE;

        if (isVolumeKey) {
            if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
                if (g_hwnd) {
                    PostMessageW(g_hwnd, WM_VOLUME, key->vkCode, 0);
                }
                return 1;
            }

            if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP) {
                return 1;
            }
        }
    }

    return CallNextHookEx(g_hook, code, wParam, lParam);
}

// --------------------------------------------------
// Window procedure
// --------------------------------------------------

static LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_VOLUME:
        AudioChange(wParam);
        HUDShow();
        return 0;

    case WM_TIMER:
        if (wParam == TIMER_HIDE) {
            KillTimer(hwnd, TIMER_HIDE);
            g_fadingOut = true;
            SetTimer(hwnd, TIMER_ANIMATE, 16, nullptr);
            return 0;
        }

        if (wParam == TIMER_ANIMATE) {
            if (g_fadingOut) {
                g_alpha -= 24;

                if (g_alpha <= 0) {
                    g_alpha = 0;
                    KillTimer(hwnd, TIMER_ANIMATE);
                    ShowWindow(hwnd, SW_HIDE);
                    return 0;
                }
            } else {
                g_alpha += 42;

                if (g_alpha >= g_targetAlpha) {
                    g_alpha = g_targetAlpha;
                    KillTimer(hwnd, TIMER_ANIMATE);
                }
            }

            Present();
            return 0;
        }

        break;

    case WM_ERASEBKGND:
        return 1;

    case WM_NCHITTEST:
        return HTTRANSPARENT;
    }

    return DefWindowProcW(hwnd, message, wParam, lParam);
}

// --------------------------------------------------
// Worker thread
// --------------------------------------------------

static DWORD WINAPI WorkerThread(LPVOID) {
    g_threadId = GetCurrentThreadId();

    MSG msg = {};
    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);

    HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    if (FAILED(hr)) {
        SetEvent(g_ready);
        return 1;
    }

    if (!AudioInit()) {
        CoUninitialize();
        SetEvent(g_ready);
        return 1;
    }

    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.lpszClassName = WINDOW_CLASS;

    RegisterClassExW(&wc);

    g_hwnd = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE |
        WS_EX_LAYERED | WS_EX_TRANSPARENT,
        WINDOW_CLASS,
        L"macOS Volume HUD",
        WS_POPUP,
        0, 0, 1, 1,
        nullptr,
        nullptr,
        GetModuleHandleW(nullptr),
        nullptr
    );

    if (!g_hwnd) {
        g_audio->Release();
        g_audio = nullptr;
        UnregisterClassW(WINDOW_CLASS, GetModuleHandleW(nullptr));
        CoUninitialize();
        SetEvent(g_ready);
        return 1;
    }

    g_hook = SetWindowsHookExW(
        WH_KEYBOARD_LL,
        KeyboardProc,
        GetModuleHandleW(nullptr),
        0
    );

    if (!g_hook) {
        DestroyWindow(g_hwnd);
        g_hwnd = nullptr;

        g_audio->Release();
        g_audio = nullptr;

        UnregisterClassW(WINDOW_CLASS, GetModuleHandleW(nullptr));
        CoUninitialize();
        SetEvent(g_ready);
        return 1;
    }

    AudioRead();

    g_readySuccess = true;
    SetEvent(g_ready);

    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    if (g_hook) {
        UnhookWindowsHookEx(g_hook);
        g_hook = nullptr;
    }

    if (g_hwnd) {
        DestroyWindow(g_hwnd);
        g_hwnd = nullptr;
    }

    FreeSurface();
    g_bg.clear();
    g_bg.shrink_to_fit();
    g_base.clear();
    g_base.shrink_to_fit();

    if (g_audio) {
        g_audio->Release();
        g_audio = nullptr;
    }

    UnregisterClassW(WINDOW_CLASS, GetModuleHandleW(nullptr));
    CoUninitialize();

    return 0;
}

// --------------------------------------------------
// Settings
// --------------------------------------------------

static void LoadSettings() {
    g_step = ClampI(Wh_GetIntSetting(L"volumeStep"), 1, 20);
    g_hideDelay = ClampI(Wh_GetIntSetting(L"hideDelay"), 500, 10000);
    g_bottomOffset = ClampI(Wh_GetIntSetting(L"bottomOffset"), 0, 1000);

    int opacity = ClampI(Wh_GetIntSetting(L"opacity"), 30, 100);
    g_targetAlpha = opacity * 255 / 100;

    PCWSTR theme = Wh_GetStringSetting(L"theme");
    if (theme) {
        if (wcscmp(theme, L"dark") == 0) {
            g_themeMode = 1;
        } else if (wcscmp(theme, L"auto") == 0) {
            g_themeMode = 2;
        } else {
            g_themeMode = 0;
        }
        Wh_FreeStringSetting(theme);
    }

    g_baseDirty = true;
}

// --------------------------------------------------
// Windhawk lifecycle
// --------------------------------------------------

BOOL Wh_ModInit() {
    LoadSettings();

    g_ready = CreateEventW(nullptr, TRUE, FALSE, nullptr);

    if (!g_ready) {
        return FALSE;
    }

    g_thread = CreateThread(nullptr, 0, WorkerThread, nullptr, 0, &g_threadId);

    if (!g_thread) {
        CloseHandle(g_ready);
        g_ready = nullptr;
        return FALSE;
    }

    DWORD result = WaitForSingleObject(g_ready, 5000);

    if (result != WAIT_OBJECT_0 || !g_readySuccess) {
        if (g_threadId) {
            PostThreadMessageW(g_threadId, WM_QUIT, 0, 0);
        }

        WaitForSingleObject(g_thread, INFINITE);

        CloseHandle(g_thread);
        CloseHandle(g_ready);

        g_thread = nullptr;
        g_ready = nullptr;

        return FALSE;
    }

    return TRUE;
}

void Wh_ModUninit() {
    if (g_thread && g_threadId) {
        PostThreadMessageW(g_threadId, WM_QUIT, 0, 0);
        WaitForSingleObject(g_thread, INFINITE);

        CloseHandle(g_thread);
        g_thread = nullptr;
    }

    if (g_ready) {
        CloseHandle(g_ready);
        g_ready = nullptr;
    }
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}
