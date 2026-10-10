// ==WindhawkMod==
// @id              selection-box-style
// @name            Selection Box Style
// @description     Custom drag-selection box for the desktop and File Explorer: colors, opacity, border, blur, and matching selected items
// @version         1.2
// @author          NoMoreDg
// @github          https://github.com/NoMoreDg/Windhawk-Mods
// @include         explorer.exe
// @compilerOptions -lcomctl32 -lgdi32 -lmsimg32 -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Selection box style
Changes the color/effect of the selection box on desktop&File explorer.

## Source code
[View the source code on GitHub](https://github.com/NoMoreDg/Windhawk-Mods)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- ApplyDesktop: true
  $name: Desktop
  $description: Style the selection box on the desktop
- ApplyExplorer: true
  $name: File Explorer
  $description: Style the selection box in File Explorer windows
- FillColor: "#0078D7"
  $name: Box color (hex)
- FillOpacity: 25
  $name: Box opacity (0-100)
- BorderColor: "#0078D7"
  $name: Border color (hex)
- BorderOpacity: 100
  $name: Border opacity (0-100)
- BorderWidth: 1
  $name: Border width in pixels (0-8)
  $description: 0 = no border
- Blur: false
  $name: Blur what is under the selection box
- BlurStrength: 10
  $name: Blur strength (1-30)
- StyleSelectedItems: true
  $name: Use the same box and border on selected items
  $description: Selected icons/files get the box color and border color set above
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <vector>
#include <algorithm>
#include <cwchar>

// ---------------------------------------------------------------- settings
struct Settings {
    bool desktop = true, explorer = true;
    COLORREF fill = RGB(0, 120, 215), border = RGB(0, 120, 215);
    int fillA = 64, borderA = 255, bw = 1;
    bool blur = false;
    int blurStrength = 10;
    bool styleItems = true;
};

static Settings g_s;
static CRITICAL_SECTION g_cs;
static UINT g_runMsg = 0;
static std::vector<HWND> g_lists;
static std::vector<HWND> g_parents;
static const UINT_PTR kTimer = 0x5E1B;

static COLORREF ParseColor(PCWSTR s, COLORREF def) {
    if (!s) return def;
    if (*s == L'#') s++;
    wchar_t* end = nullptr;
    unsigned long v = wcstoul(s, &end, 16);
    if (end == s) return def;
    return RGB((v >> 16) & 0xFF, (v >> 8) & 0xFF, v & 0xFF);
}

static void LoadSettings() {
    Settings s;
    s.desktop = Wh_GetIntSetting(L"ApplyDesktop") != 0;
    s.explorer = Wh_GetIntSetting(L"ApplyExplorer") != 0;

    PCWSTR c = Wh_GetStringSetting(L"FillColor");
    s.fill = ParseColor(c, RGB(0, 120, 215));
    Wh_FreeStringSetting(c);
    c = Wh_GetStringSetting(L"BorderColor");
    s.border = ParseColor(c, RGB(0, 120, 215));
    Wh_FreeStringSetting(c);

    s.fillA = std::clamp(Wh_GetIntSetting(L"FillOpacity"), 0, 100) * 255 / 100;
    s.borderA = std::clamp(Wh_GetIntSetting(L"BorderOpacity"), 0, 100) * 255 / 100;
    s.bw = std::clamp(Wh_GetIntSetting(L"BorderWidth"), 0, 8);
    s.blur = Wh_GetIntSetting(L"Blur") != 0;
    s.blurStrength = std::clamp(Wh_GetIntSetting(L"BlurStrength"), 1, 30);
    s.styleItems = Wh_GetIntSetting(L"StyleSelectedItems") != 0;

    EnterCriticalSection(&g_cs);
    g_s = s;
    LeaveCriticalSection(&g_cs);
}

static Settings GetSettings() {
    EnterCriticalSection(&g_cs);
    Settings s = g_s;
    LeaveCriticalSection(&g_cs);
    return s;
}

static double NowMs() {
    static LARGE_INTEGER f = {};
    if (!f.QuadPart) QueryPerformanceFrequency(&f);
    LARGE_INTEGER t;
    QueryPerformanceCounter(&t);
    return t.QuadPart * 1000.0 / (double)f.QuadPart;
}

// ---------------------------------------------------------------- per-list state
enum Kind { K_NONE, K_DESKTOP, K_EXPLORER };

struct TmpDib {
    HDC dc = nullptr;
    HBITMAP bmp = nullptr;
    HGDIOBJ old = nullptr;
    void* bits = nullptr;
    int w = 0, h = 0;
};

struct ListCtx {
    HWND h = nullptr;
    Kind kind = K_NONE;
    bool paintOn = false;

    // paint-time cache
    Settings ps;
    LRESULT pview = 0;

    // drag state
    Settings ds;
    bool dragging = false, moved = false, toggle = false, dirty = false;
    int tick = 0;
    double lastApply = 0;
    POINT start = {}, cur = {}, origin0 = {};
    RECT rect = {};
    std::vector<RECT> items;     // item bounds relative to origin0
    std::vector<char> base;      // selection when the drag began (for Ctrl/Shift)
    std::vector<char> state;     // selection we have applied so far

    // cached graphics
    HDC memDC = nullptr;
    DWORD fillPx = 0, borderPx = 0;
    HBITMAP fill1 = nullptr, border1 = nullptr;
    HBITMAP surf = nullptr;
    int surfW = 0, surfH = 0;
    DWORD surfPx = 0;

    // blur buffers
    TmpDib srcT, outT;
    std::vector<BYTE> scratch;

    // blurred desktop snapshot (small, BGRA)
    std::vector<BYTE> snap;
    int snapK = 1, snapW = 0, snapH = 0;
};

// ---------------------------------------------------------------- graphics helpers
static DWORD Premul(COLORREF c, int a) {
    unsigned r = GetRValue(c), g = GetGValue(c), b = GetBValue(c);
    return ((unsigned)a << 24) | ((r * a / 255) << 16) | ((g * a / 255) << 8) | (b * a / 255);
}

static HBITMAP MakeDib(HDC ref, int w, int h, void** bits) {
    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = w;
    bi.bmiHeader.biHeight = -h;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    return CreateDIBSection(ref, &bi, DIB_RGB_COLORS, bits, nullptr, 0);
}

static HBITMAP Make1x1(DWORD px) {
    void* bits = nullptr;
    HBITMAP b = MakeDib(nullptr, 1, 1, &bits);
    if (b && bits) *(DWORD*)bits = px;
    return b;
}

static void FreeDib(TmpDib& t) {
    if (t.dc) { SelectObject(t.dc, t.old); DeleteDC(t.dc); }
    if (t.bmp) DeleteObject(t.bmp);
    t = TmpDib();
}

static bool EnsureDib(TmpDib& t, HDC ref, int w, int h) {
    if (t.dc && t.w >= w && t.h >= h) return true;
    FreeDib(t);
    int nw = (w + 127) & ~127, nh = (h + 127) & ~127;
    void* bits = nullptr;
    HBITMAP b = MakeDib(ref, nw, nh, &bits);
    HDC dc = CreateCompatibleDC(ref);
    if (!b || !bits || !dc) {
        if (b) DeleteObject(b);
        if (dc) DeleteDC(dc);
        return false;
    }
    t.old = SelectObject(dc, b);
    t.dc = dc;
    t.bmp = b;
    t.bits = bits;
    t.w = nw;
    t.h = nh;
    return true;
}

static HDC GetMemDC(ListCtx* c) {
    if (!c->memDC) c->memDC = CreateCompatibleDC(nullptr);
    return c->memDC;
}

static void UpdateGfx(ListCtx* c, const Settings& s) {
    DWORD f = Premul(s.fill, s.fillA), b = Premul(s.border, s.borderA);
    if (!c->fill1 || f != c->fillPx) {
        if (c->fill1) DeleteObject(c->fill1);
        c->fill1 = Make1x1(f);
        c->fillPx = f;
    }
    if (!c->border1 || b != c->borderPx) {
        if (c->border1) DeleteObject(c->border1);
        c->border1 = Make1x1(b);
        c->borderPx = b;
    }
}

static void FreeSurface(ListCtx* c) {
    if (c->surf) { DeleteObject(c->surf); c->surf = nullptr; }
    c->surfW = c->surfH = 0;
}

static void FreeGfx(ListCtx* c) {
    FreeSurface(c);
    FreeDib(c->srcT);
    FreeDib(c->outT);
    std::vector<BYTE>().swap(c->scratch);
    if (c->fill1) { DeleteObject(c->fill1); c->fill1 = nullptr; }
    if (c->border1) { DeleteObject(c->border1); c->border1 = nullptr; }
    if (c->memDC) { DeleteDC(c->memDC); c->memDC = nullptr; }
}

static void Blend1(ListCtx* c, HDC hdc, HBITMAP bmp, RECT rc) {
    int w = rc.right - rc.left, h = rc.bottom - rc.top;
    if (w <= 0 || h <= 0 || !bmp) return;
    HDC mem = GetMemDC(c);
    HGDIOBJ old = SelectObject(mem, bmp);
    BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    AlphaBlend(hdc, rc.left, rc.top, w, h, mem, 0, 0, 1, 1, bf);
    SelectObject(mem, old);
}

// Large fills use a same-size surface so no stretching is needed.
static void FillSurface(ListCtx* c, HDC hdc, RECT rc) {
    int w = rc.right - rc.left, h = rc.bottom - rc.top;
    if (w <= 0 || h <= 0) return;
    if (!c->surf || c->surfW < w || c->surfH < h || c->surfPx != c->fillPx) {
        FreeSurface(c);
        int nw = (std::max(w, c->surfW) + 127) & ~127;
        int nh = (std::max(h, c->surfH) + 127) & ~127;
        void* bits = nullptr;
        HBITMAP b = MakeDib(nullptr, nw, nh, &bits);
        if (!b || !bits) {
            if (b) DeleteObject(b);
            Blend1(c, hdc, c->fill1, rc);
            return;
        }
        std::fill((DWORD*)bits, (DWORD*)bits + (size_t)nw * nh, c->fillPx);
        c->surf = b;
        c->surfW = nw;
        c->surfH = nh;
        c->surfPx = c->fillPx;
    }
    HDC mem = GetMemDC(c);
    HGDIOBJ old = SelectObject(mem, c->surf);
    BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    AlphaBlend(hdc, rc.left, rc.top, w, h, mem, 0, 0, w, h, bf);
    SelectObject(mem, old);
}

static void DrawBorder(ListCtx* c, HDC hdc, RECT rc, int bw) {
    RECT t = {rc.left, rc.top, rc.right, rc.top + bw};
    RECT b = {rc.left, rc.bottom - bw, rc.right, rc.bottom};
    RECT l = {rc.left, rc.top + bw, rc.left + bw, rc.bottom - bw};
    RECT r = {rc.right - bw, rc.top + bw, rc.right, rc.bottom - bw};
    Blend1(c, hdc, c->border1, t);
    Blend1(c, hdc, c->border1, b);
    Blend1(c, hdc, c->border1, l);
    Blend1(c, hdc, c->border1, r);
}

// Small box (selected item).
static void DrawItemBox(ListCtx* c, HDC hdc, RECT rc, const Settings& s) {
    int w = rc.right - rc.left, h = rc.bottom - rc.top;
    if (w <= 0 || h <= 0) return;
    UpdateGfx(c, s);
    int bw = (s.borderA > 0) ? std::min(s.bw, std::min(w, h) / 2) : 0;
    if (bw > 0) DrawBorder(c, hdc, rc, bw);
    if (s.fillA > 0) {
        RECT in = {rc.left + bw, rc.top + bw, rc.right - bw, rc.bottom - bw};
        Blend1(c, hdc, c->fill1, in);
    }
}

// Big box (selection rectangle). withFill=false when the fill was already mixed into a blur.
static void DrawMarqueeBox(ListCtx* c, HDC hdc, RECT R, RECT cl, const Settings& s, bool withFill) {
    int w = R.right - R.left, h = R.bottom - R.top;
    if (w <= 0 || h <= 0) return;
    UpdateGfx(c, s);
    int bw = (s.borderA > 0) ? std::min(s.bw, std::min(w, h) / 2) : 0;
    if (bw > 0) DrawBorder(c, hdc, R, bw);
    if (withFill && s.fillA > 0) {
        RECT in = {R.left + bw, R.top + bw, R.right - bw, R.bottom - bw};
        RECT vis;
        if (IntersectRect(&vis, &in, &cl)) FillSurface(c, hdc, vis);
    }
}

// ---------------------------------------------------------------- blur
static void BoxBlurH(const BYTE* in, BYTE* out, int w, int h, int r) {
    for (int y = 0; y < h; y++) {
        const BYTE* row = in + (size_t)y * w * 4;
        BYTE* orow = out + (size_t)y * w * 4;
        int sb = 0, sg = 0, sr = 0, cnt = 0;
        int hi = std::min(r, w - 1);
        for (int x = 0; x <= hi; x++) {
            sb += row[x * 4]; sg += row[x * 4 + 1]; sr += row[x * 4 + 2]; cnt++;
        }
        for (int x = 0; x < w; x++) {
            orow[x * 4] = (BYTE)(sb / cnt);
            orow[x * 4 + 1] = (BYTE)(sg / cnt);
            orow[x * 4 + 2] = (BYTE)(sr / cnt);
            orow[x * 4 + 3] = 255;
            int add = x + r + 1, rem = x - r;
            if (add < w) { sb += row[add * 4]; sg += row[add * 4 + 1]; sr += row[add * 4 + 2]; cnt++; }
            if (rem >= 0) { sb -= row[rem * 4]; sg -= row[rem * 4 + 1]; sr -= row[rem * 4 + 2]; cnt--; }
        }
    }
}

static void BoxBlurV(const BYTE* in, BYTE* out, int w, int h, int r) {
    for (int x = 0; x < w; x++) {
        int sb = 0, sg = 0, sr = 0, cnt = 0;
        int hi = std::min(r, h - 1);
        for (int y = 0; y <= hi; y++) {
            const BYTE* p = in + ((size_t)y * w + x) * 4;
            sb += p[0]; sg += p[1]; sr += p[2]; cnt++;
        }
        for (int y = 0; y < h; y++) {
            BYTE* o = out + ((size_t)y * w + x) * 4;
            o[0] = (BYTE)(sb / cnt); o[1] = (BYTE)(sg / cnt); o[2] = (BYTE)(sr / cnt); o[3] = 255;
            int add = y + r + 1, rem = y - r;
            if (add < h) { const BYTE* p = in + ((size_t)add * w + x) * 4; sb += p[0]; sg += p[1]; sr += p[2]; cnt++; }
            if (rem >= 0) { const BYTE* p = in + ((size_t)rem * w + x) * 4; sb -= p[0]; sg -= p[1]; sr -= p[2]; cnt--; }
        }
    }
}

// In-place blur of a small BGRA buffer (two H+V passes).
static void BlurSmall(BYTE* buf, int w, int h, int r) {
    std::vector<BYTE> tmp((size_t)w * h * 4);
    BoxBlurH(buf, tmp.data(), w, h, r);
    BoxBlurV(tmp.data(), buf, w, h, r);
    BoxBlurH(buf, tmp.data(), w, h, r);
    BoxBlurV(tmp.data(), buf, w, h, r);
}

// Shrinks a w*h BGRA image by k (sampling up to 4x4 pixels per block).
static void Downscale(const BYTE* S, int stridepx, int w, int h, int k, BYTE* A, int sw, int sh) {
    int step = std::max(1, k / 4);
    for (int sy = 0; sy < sh; sy++) {
        for (int sx = 0; sx < sw; sx++) {
            int xa = sx * k, xb = std::min(w, xa + k), ya = sy * k, yb = std::min(h, ya + k);
            int cb = 0, cg = 0, cr = 0, n = 0;
            for (int y = ya; y < yb; y += step)
                for (int x = xa; x < xb; x += step) {
                    const BYTE* p = S + ((size_t)y * stridepx + x) * 4;
                    cb += p[0]; cg += p[1]; cr += p[2]; n++;
                }
            if (n == 0) n = 1;
            BYTE* d = A + ((size_t)sy * sw + sx) * 4;
            d[0] = (BYTE)(cb / n); d[1] = (BYTE)(cg / n); d[2] = (BYTE)(cr / n); d[3] = 255;
        }
    }
}

// Upscales a blurred small image (bilinear, in plain code) into an opaque buffer, mixes the box color
// into it (inside `inner` only) and copies the result 1:1 to the window.
// The small image's pixel (0,0) corresponds to client position (ox, oy), one small pixel = k pixels.
static bool RenderBlur(ListCtx* c, HDC dst, RECT vis, const BYTE* small, int sw, int sh, int k,
                       int ox, int oy, RECT inner, const Settings& s) {
    int w = vis.right - vis.left, h = vis.bottom - vis.top;
    if (w <= 0 || h <= 0 || sw <= 0 || sh <= 0) return false;
    if (!EnsureDib(c->outT, dst, w, h)) return false;

    std::vector<int> x0(w), x1(w), wx(w);
    for (int x = 0; x < w; x++) {
        float fx = ((float)(vis.left + x - ox) + 0.5f) / k - 0.5f;
        if (fx < 0) fx = 0;
        int ix = (int)fx;
        if (ix >= sw - 1) { x0[x] = x1[x] = sw - 1; wx[x] = 0; }
        else { x0[x] = ix; x1[x] = ix + 1; wx[x] = (int)((fx - ix) * 256); }
    }

    int fa = s.fillA, ia = 255 - s.fillA;
    int fcol[3] = {GetBValue(s.fill), GetGValue(s.fill), GetRValue(s.fill)};  // B, G, R
    int stride = c->outT.w;

    for (int y = 0; y < h; y++) {
        float fy = ((float)(vis.top + y - oy) + 0.5f) / k - 0.5f;
        if (fy < 0) fy = 0;
        int iy = (int)fy, ya, yb, wy;
        if (iy >= sh - 1) { ya = yb = sh - 1; wy = 0; }
        else { ya = iy; yb = iy + 1; wy = (int)((fy - iy) * 256); }
        const BYTE* r0 = small + (size_t)ya * sw * 4;
        const BYTE* r1 = small + (size_t)yb * sw * 4;
        BYTE* o = (BYTE*)c->outT.bits + (size_t)y * stride * 4;
        int ay = vis.top + y;
        bool rowIn = fa > 0 && ay >= inner.top && ay < inner.bottom;
        for (int x = 0; x < w; x++) {
            bool useFill = rowIn && (vis.left + x) >= inner.left && (vis.left + x) < inner.right;
            for (int ch = 0; ch < 3; ch++) {
                int top = r0[x0[x] * 4 + ch] * (256 - wx[x]) + r0[x1[x] * 4 + ch] * wx[x];
                int bot = r1[x0[x] * 4 + ch] * (256 - wx[x]) + r1[x1[x] * 4 + ch] * wx[x];
                int v = (top * (256 - wy) + bot * wy) >> 16;
                if (useFill) v = (v * ia + fcol[ch] * fa + 127) / 255;
                o[x * 4 + ch] = (BYTE)v;
            }
            o[x * 4 + 3] = 255;
        }
    }
    BitBlt(dst, vis.left, vis.top, w, h, c->outT.dc, 0, 0, SRCCOPY);
    return true;
}

static void FlushDwm() {
    typedef HRESULT (WINAPI* Fn)();
    static Fn f = nullptr;
    static bool tried = false;
    if (!tried) {
        tried = true;
        HMODULE m = LoadLibraryW(L"dwmapi.dll");
        if (m) f = (Fn)GetProcAddress(m, "DwmFlush");
    }
    if (f) f();
}

static void FreeSnapshot(ListCtx* c) {
    std::vector<BYTE>().swap(c->snap);
    c->snapW = c->snapH = 0;
}

// Desktop: the paint DC has no wallpaper, so grab a clean screen copy once and blur it once.
static void TakeSnapshot(ListCtx* c, int strength) {
    FreeSnapshot(c);
    RECT cl;
    GetClientRect(c->h, &cl);
    int w = cl.right, h = cl.bottom;
    if (w <= 0 || h <= 0 || (long long)w * h > 16000000LL) return;

    UpdateWindow(c->h);
    FlushDwm();

    POINT o = {0, 0};
    ClientToScreen(c->h, &o);
    HDC scr = GetDC(nullptr);
    void* fbits = nullptr;
    HBITMAP full = MakeDib(scr, w, h, &fbits);
    HDC fdc = CreateCompatibleDC(scr);
    if (full && fbits && fdc) {
        HGDIOBJ fo = SelectObject(fdc, full);
        BitBlt(fdc, 0, 0, w, h, scr, o.x, o.y, SRCCOPY | CAPTUREBLT);
        GdiFlush();
        int k = ((long long)w * h > 4000000LL) ? 4 : 2;
        int sw = (w + k - 1) / k, sh = (h + k - 1) / k;
        c->snap.assign((size_t)sw * sh * 4, 0);
        Downscale((const BYTE*)fbits, w, w, h, k, c->snap.data(), sw, sh);
        BlurSmall(c->snap.data(), sw, sh, std::max(1, strength / k));
        c->snapW = sw;
        c->snapH = sh;
        c->snapK = k;
        SelectObject(fdc, fo);
    }
    if (fdc) DeleteDC(fdc);
    if (full) DeleteObject(full);
    ReleaseDC(nullptr, scr);
}

// Returns true when the blur (with the box color mixed in) was drawn.
static bool BlurRegion(ListCtx* c, HDC hdc, RECT vis, const Settings& s) {
    RECT R = c->rect;
    int rw = R.right - R.left, rh = R.bottom - R.top;
    int bw = (s.borderA > 0) ? std::min(s.bw, std::min(rw, rh) / 2) : 0;
    RECT inner = {R.left + bw, R.top + bw, R.right - bw, R.bottom - bw};

    if (!c->snap.empty())
        return RenderBlur(c, hdc, vis, c->snap.data(), c->snapW, c->snapH, c->snapK, 0, 0, inner, s);

    // Explorer: blur what was just painted under the box
    int w = vis.right - vis.left, h = vis.bottom - vis.top;
    if (w < 4 || h < 4) return false;
    int k = std::clamp((std::max(w, h) + 159) / 160, 1, 16);
    int sw = std::max(1, (w + k - 1) / k), sh = std::max(1, (h + k - 1) / k);
    if (!EnsureDib(c->srcT, hdc, w, h)) return false;
    BitBlt(c->srcT.dc, 0, 0, w, h, hdc, vis.left, vis.top, SRCCOPY);
    GdiFlush();
    c->scratch.assign((size_t)sw * sh * 4, 0);
    Downscale((const BYTE*)c->srcT.bits, c->srcT.w, w, h, k, c->scratch.data(), sw, sh);
    BlurSmall(c->scratch.data(), sw, sh, std::max(1, s.blurStrength / k));
    return RenderBlur(c, hdc, vis, c->scratch.data(), sw, sh, k, vis.left, vis.top, inner, s);
}

static void DrawMarquee(ListCtx* c, HDC hdc, const Settings& s) {
    RECT cl;
    GetClientRect(c->h, &cl);
    RECT vis;
    if (!IntersectRect(&vis, &c->rect, &cl)) return;
    bool blurred = false;
    if (s.blur) blurred = BlurRegion(c, hdc, vis, s);
    DrawMarqueeBox(c, hdc, c->rect, cl, s, !blurred);
}

// ---------------------------------------------------------------- drag logic
static Kind Classify(HWND h) {
    HWND root = GetAncestor(h, GA_ROOT);
    if (!root) return K_NONE;
    wchar_t cls[64] = {};
    GetClassNameW(root, cls, 64);
    if (!wcscmp(cls, L"Progman") || !wcscmp(cls, L"WorkerW")) return K_DESKTOP;
    if (!wcscmp(cls, L"CabinetWClass")) return K_EXPLORER;
    return K_NONE;
}

static bool KindEnabled(Kind k, const Settings& s) {
    return (k == K_DESKTOP && s.desktop) || (k == K_EXPLORER && s.explorer);
}

static POINT GetOrigin(HWND h) {
    POINT p = {0, 0};
    if (!SendMessageW(h, LVM_GETITEMPOSITION, 0, (LPARAM)&p)) p.x = p.y = 0;
    return p;
}

static void BeginDrag(ListCtx* c, POINT pt, bool ctrl, bool shift) {
    HWND h = c->h;
    c->ds = GetSettings();
    c->dragging = true;
    c->moved = false;
    c->dirty = false;
    c->tick = 0;
    c->lastApply = NowMs();
    c->start = pt;
    c->cur = pt;
    c->origin0 = GetOrigin(h);
    c->rect = RECT{0, 0, 0, 0};
    c->toggle = ctrl;

    // read every item's bounds once; later moves only do arithmetic
    int n = (int)SendMessageW(h, LVM_GETITEMCOUNT, 0, 0);
    if (n < 0) n = 0;
    c->items.assign((size_t)n, RECT{0, 0, 0, 0});
    for (int i = 0; i < n; i++) {
        RECT ir = {};
        ir.left = LVIR_BOUNDS;
        if (SendMessageW(h, LVM_GETITEMRECT, i, (LPARAM)&ir)) {
            OffsetRect(&ir, -c->origin0.x, -c->origin0.y);
            c->items[i] = ir;
        }
    }

    c->base.assign((size_t)n, 0);
    if (ctrl || shift) {
        for (int i = 0; i < n; i++)
            c->base[i] = (SendMessageW(h, LVM_GETITEMSTATE, i, LVIS_SELECTED) & LVIS_SELECTED) ? 1 : 0;
    } else {
        LVITEMW lvi = {};
        lvi.stateMask = LVIS_SELECTED;
        lvi.state = 0;
        SendMessageW(h, LVM_SETITEMSTATE, (WPARAM)-1, (LPARAM)&lvi);
    }
    c->state = c->base;

    if (GetFocus() != h) SetFocus(h);
    SetCapture(h);
    SetTimer(h, kTimer, 16, nullptr);
}

// Applies the selection for the current rectangle; only changed items get a message.
static void ApplySelection(ListCtx* c) {
    if (!c->moved) return;
    HWND h = c->h;
    POINT o = GetOrigin(h);
    RECT Rc = {c->rect.left - o.x, c->rect.top - o.y, c->rect.right - o.x, c->rect.bottom - o.y};
    size_t n = c->items.size();
    for (size_t i = 0; i < n; i++) {
        const RECT& ir = c->items[i];
        bool inter = ir.left < Rc.right && ir.right > Rc.left && ir.top < Rc.bottom && ir.bottom > Rc.top;
        bool b = c->base[i] != 0;
        bool want = c->toggle ? (b != inter) : (b || inter);
        if (want != (c->state[i] != 0)) {
            LVITEMW lvi = {};
            lvi.stateMask = LVIS_SELECTED;
            lvi.state = want ? LVIS_SELECTED : 0;
            SendMessageW(h, LVM_SETITEMSTATE, (WPARAM)i, (LPARAM)&lvi);
            c->state[i] = want ? 1 : 0;
        }
    }
}

static void EndDrag(ListCtx* c) {
    if (!c->dragging) return;
    if (c->moved && c->dirty) ApplySelection(c);
    c->dragging = false;
    KillTimer(c->h, kTimer);
    if (GetCapture() == c->h) ReleaseCapture();
    FreeSnapshot(c);
    FreeSurface(c);
    FreeDib(c->srcT);
    FreeDib(c->outT);
    RECT inv = c->rect;
    c->rect = RECT{0, 0, 0, 0};
    c->moved = false;
    c->dirty = false;
    std::vector<RECT>().swap(c->items);
    std::vector<char>().swap(c->base);
    std::vector<char>().swap(c->state);
    InflateRect(&inv, 12, 12);
    InvalidateRect(c->h, &inv, FALSE);
}

// Cheap per-mouse-move work: update the rectangle and repaint only the area it covers.
static void UpdateRect(ListCtx* c) {
    HWND h = c->h;
    if (!c->moved) {
        if (abs(c->cur.x - c->start.x) < 3 && abs(c->cur.y - c->start.y) < 3) return;
        c->moved = true;
        if (c->ds.blur && c->kind == K_DESKTOP) TakeSnapshot(c, c->ds.blurStrength);
    }
    POINT o = GetOrigin(h);
    POINT a = {c->start.x + (o.x - c->origin0.x), c->start.y + (o.y - c->origin0.y)};
    RECT R = {std::min(a.x, c->cur.x), std::min(a.y, c->cur.y),
              std::max(a.x, c->cur.x) + 1, std::max(a.y, c->cur.y) + 1};
    RECT inv;
    UnionRect(&inv, &c->rect, &R);
    c->rect = R;
    c->dirty = true;
    InflateRect(&inv, c->ds.bw + 2, c->ds.bw + 2);
    InvalidateRect(h, &inv, FALSE);
}

static void AutoScroll(ListCtx* c) {
    POINT pt;
    GetCursorPos(&pt);
    ScreenToClient(c->h, &pt);
    RECT cl;
    GetClientRect(c->h, &cl);
    int dx = 0, dy = 0;
    if (pt.y < cl.top) dy = -std::min(80, 24 + (int)(cl.top - pt.y) / 3);
    else if (pt.y >= cl.bottom) dy = std::min(80, 24 + (int)(pt.y - cl.bottom) / 3);
    if (pt.x < cl.left) dx = -std::min(80, 24 + (int)(cl.left - pt.x) / 3);
    else if (pt.x >= cl.right) dx = std::min(80, 24 + (int)(pt.x - cl.right) / 3);
    if (!dx && !dy) return;
    SendMessageW(c->h, LVM_SCROLL, dx, dy);
    c->cur = pt;
    UpdateRect(c);
}

// ---------------------------------------------------------------- subclass procs
static void Untrack(std::vector<HWND>& v, HWND h) {
    EnterCriticalSection(&g_cs);
    v.erase(std::remove(v.begin(), v.end(), h), v.end());
    LeaveCriticalSection(&g_cs);
}

static LRESULT CALLBACK ListSubclassProc(HWND h, UINT msg, WPARAM wp, LPARAM lp,
                                         UINT_PTR id, DWORD_PTR ref) {
    ListCtx* c = (ListCtx*)ref;
    switch (msg) {
        case WM_NCDESTROY: {
            EndDrag(c);
            FreeSnapshot(c);
            FreeGfx(c);
            Untrack(g_lists, h);
            RemoveWindowSubclass(h, ListSubclassProc, id);
            delete c;
            return DefSubclassProc(h, msg, wp, lp);
        }
        case WM_LBUTTONDOWN: {
            Settings s = GetSettings();
            c->kind = Classify(h);
            if (!KindEnabled(c->kind, s)) break;
            POINT pt = {GET_X_LPARAM(lp), GET_Y_LPARAM(lp)};
            LVHITTESTINFO hi = {};
            hi.pt = pt;
            SendMessageW(h, LVM_HITTEST, 0, (LPARAM)&hi);
            // only start on truly empty space; clicks on items stay native
            if (hi.iItem != -1 || !(hi.flags & LVHT_NOWHERE) || (hi.flags & LVHT_ONITEM)) break;
            bool ctrl = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
            bool shift = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
            BeginDrag(c, pt, ctrl, shift);
            return 0;
        }
        case WM_MOUSEMOVE:
            if (c->dragging) {
                c->cur.x = GET_X_LPARAM(lp);
                c->cur.y = GET_Y_LPARAM(lp);
                UpdateRect(c);
                if (c->moved && c->dirty) {
                    double now = NowMs();
                    if (now - c->lastApply >= 16.0) {
                        c->dirty = false;
                        c->lastApply = now;
                        ApplySelection(c);
                    }
                }
                return 0;
            }
            break;
        case WM_LBUTTONUP:
            if (c->dragging) { EndDrag(c); return 0; }
            break;
        case WM_CAPTURECHANGED:
        case WM_CANCELMODE:
            if (c->dragging) EndDrag(c);
            break;
        case WM_TIMER:
            if (wp == kTimer) {
                if (c->dragging) {
                    if (c->moved && c->dirty) {
                        c->dirty = false;
                        c->lastApply = NowMs();
                        ApplySelection(c);
                    }
                    if ((++c->tick % 3) == 0) AutoScroll(c);
                }
                return 0;
            }
            break;
    }
    return DefSubclassProc(h, msg, wp, lp);
}

static ListCtx* CtxFor(HWND hList) {
    DWORD_PTR ref = 0;
    if (GetWindowSubclass(hList, ListSubclassProc, 1, &ref)) return (ListCtx*)ref;
    return nullptr;
}

static LRESULT CALLBACK ParentSubclassProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp,
                                           UINT_PTR id, DWORD_PTR) {
    if (msg == WM_NCDESTROY) {
        Untrack(g_parents, hWnd);
        RemoveWindowSubclass(hWnd, ParentSubclassProc, id);
        return DefSubclassProc(hWnd, msg, wp, lp);
    }

    if (msg == WM_NOTIFY && lp) {
        NMHDR* nh = (NMHDR*)lp;
        if (nh->code == (UINT)NM_CUSTOMDRAW) {
            ListCtx* c = CtxFor(nh->hwndFrom);
            if (c) {
                LRESULT r = DefSubclassProc(hWnd, msg, wp, lp);
                NMLVCUSTOMDRAW* cd = (NMLVCUSTOMDRAW*)lp;
                HWND hList = nh->hwndFrom;

                switch (cd->nmcd.dwDrawStage) {
                    case CDDS_PREPAINT: {
                        c->ps = GetSettings();
                        c->kind = Classify(hList);
                        c->paintOn = KindEnabled(c->kind, c->ps);
                        if (c->paintOn && !(r & CDRF_SKIPDEFAULT)) {
                            if (c->ps.styleItems) {
                                c->pview = SendMessageW(hList, LVM_GETVIEW, 0, 0);
                                r |= CDRF_NOTIFYITEMDRAW;
                            }
                            if (c->dragging && c->moved) r |= CDRF_NOTIFYPOSTPAINT;
                        }
                        break;
                    }
                    case CDDS_ITEMPREPAINT: {
                        if (c->paintOn && c->ps.styleItems && !(r & CDRF_SKIPDEFAULT) &&
                            (cd->nmcd.uItemState & CDIS_SELECTED)) {
                            RECT ir = {};
                            ir.left = (c->pview == LV_VIEW_DETAILS) ? LVIR_SELECTBOUNDS : LVIR_BOUNDS;
                            if (SendMessageW(hList, LVM_GETITEMRECT, (WPARAM)cd->nmcd.dwItemSpec, (LPARAM)&ir))
                                DrawItemBox(c, cd->nmcd.hdc, ir, c->ps);
                            // hide the native selection/hover/focus look
                            cd->nmcd.uItemState &= ~(UINT)(CDIS_SELECTED | CDIS_HOT | CDIS_FOCUS | CDIS_MARKED);
                            r |= CDRF_NEWFONT;
                        }
                        break;
                    }
                    case CDDS_POSTPAINT: {
                        if (c->paintOn && c->dragging && c->moved)
                            DrawMarquee(c, cd->nmcd.hdc, c->ps);
                        break;
                    }
                }
                return r;
            }
        }
    }
    return DefSubclassProc(hWnd, msg, wp, lp);
}

// ---------------------------------------------------------------- run on the window's thread
struct RunParam { void (WINAPI* proc)(PVOID); PVOID arg; };

static LRESULT CALLBACK RunHookProc(int code, WPARAM wp, LPARAM lp) {
    if (code == HC_ACTION) {
        const CWPSTRUCT* cwp = (const CWPSTRUCT*)lp;
        if (cwp->message == g_runMsg) {
            RunParam* p = (RunParam*)cwp->lParam;
            p->proc(p->arg);
        }
    }
    return CallNextHookEx(nullptr, code, wp, lp);
}

static bool RunFromWindowThread(HWND hWnd, void (WINAPI* proc)(PVOID), PVOID arg) {
    DWORD tid = GetWindowThreadProcessId(hWnd, nullptr);
    if (!tid) return false;
    if (tid == GetCurrentThreadId()) { proc(arg); return true; }
    HHOOK hook = SetWindowsHookExW(WH_CALLWNDPROC, RunHookProc, nullptr, tid);
    if (!hook) return false;
    RunParam p = {proc, arg};
    SendMessageW(hWnd, g_runMsg, 0, (LPARAM)&p);
    UnhookWindowsHookEx(hook);
    return true;
}

static void WINAPI DoAttach(PVOID arg) {
    HWND h = (HWND)arg;
    DWORD_PTR dummy;
    if (GetWindowSubclass(h, ListSubclassProc, 1, &dummy)) return;

    ListCtx* c = new ListCtx();
    c->h = h;
    if (!SetWindowSubclass(h, ListSubclassProc, 1, (DWORD_PTR)c)) { delete c; return; }

    HWND p = GetParent(h);
    bool newParent = false;
    if (p && !GetWindowSubclass(p, ParentSubclassProc, 1, &dummy))
        newParent = SetWindowSubclass(p, ParentSubclassProc, 1, 0) != FALSE;

    EnterCriticalSection(&g_cs);
    g_lists.push_back(h);
    if (newParent) g_parents.push_back(p);
    LeaveCriticalSection(&g_cs);
}

static void WINAPI DoDetachList(PVOID arg) {
    HWND h = (HWND)arg;
    DWORD_PTR ref = 0;
    if (GetWindowSubclass(h, ListSubclassProc, 1, &ref)) {
        ListCtx* c = (ListCtx*)ref;
        EndDrag(c);
        FreeSnapshot(c);
        FreeGfx(c);
        RemoveWindowSubclass(h, ListSubclassProc, 1);
        delete c;
    }
    Untrack(g_lists, h);
}

static void WINAPI DoDetachParent(PVOID arg) {
    HWND p = (HWND)arg;
    RemoveWindowSubclass(p, ParentSubclassProc, 1);
    Untrack(g_parents, p);
}

// ---------------------------------------------------------------- hook + scanning
using CreateWindowExW_t = decltype(&CreateWindowExW);
static CreateWindowExW_t CreateWindowExW_Original;

static HWND WINAPI CreateWindowExW_Hook(DWORD ex, LPCWSTR cls, LPCWSTR name, DWORD style,
                                        int x, int y, int w, int h, HWND parent, HMENU menu,
                                        HINSTANCE inst, LPVOID param) {
    HWND hwnd = CreateWindowExW_Original(ex, cls, name, style, x, y, w, h, parent, menu, inst, param);
    if (hwnd && (style & WS_CHILD)) {
        wchar_t cn[32] = {};
        GetClassNameW(hwnd, cn, 32);
        if (!wcscmp(cn, L"SysListView32")) DoAttach(hwnd);
    }
    return hwnd;
}

static BOOL CALLBACK EnumChildProc(HWND h, LPARAM) {
    wchar_t cls[32] = {};
    GetClassNameW(h, cls, 32);
    if (!wcscmp(cls, L"SysListView32")) RunFromWindowThread(h, DoAttach, h);
    return TRUE;
}

static BOOL CALLBACK EnumTopProc(HWND h, LPARAM) {
    DWORD pid = 0;
    GetWindowThreadProcessId(h, &pid);
    if (pid == GetCurrentProcessId()) EnumChildWindows(h, EnumChildProc, 0);
    return TRUE;
}

static void InvalidateAllLists() {
    std::vector<HWND> copy;
    EnterCriticalSection(&g_cs);
    copy = g_lists;
    LeaveCriticalSection(&g_cs);
    for (HWND h : copy)
        if (IsWindow(h)) InvalidateRect(h, nullptr, TRUE);
}

BOOL Wh_ModInit() {
    InitializeCriticalSection(&g_cs);
    g_runMsg = RegisterWindowMessageW(L"Windhawk_RunFromWindowThread_selection-box-style");
    LoadSettings();
    Wh_SetFunctionHook((void*)CreateWindowExW, (void*)CreateWindowExW_Hook,
                       (void**)&CreateWindowExW_Original);
    EnumWindows(EnumTopProc, 0);
    return TRUE;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    InvalidateAllLists();
}

void Wh_ModUninit() {
    std::vector<HWND> lists, parents;
    EnterCriticalSection(&g_cs);
    lists = g_lists;
    parents = g_parents;
    LeaveCriticalSection(&g_cs);
    for (HWND h : lists)
        if (IsWindow(h)) RunFromWindowThread(h, DoDetachList, h);
    for (HWND p : parents)
        if (IsWindow(p)) RunFromWindowThread(p, DoDetachParent, p);
    for (HWND h : lists)
        if (IsWindow(h)) InvalidateRect(h, nullptr, TRUE);
    DeleteCriticalSection(&g_cs);
}