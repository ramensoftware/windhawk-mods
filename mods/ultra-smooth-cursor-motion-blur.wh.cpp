// ==WindhawkMod==
// @id              ultra-smooth-cursor-motion-blur
// @name            Ultra Smooth Cursor Motion Blur
// @description     Buttery directional motion blur of the real mouse pointer
// @version         4.0.0
// @author          Grok
// @license         MIT
// @include         windhawk.exe
// @compilerOptions -std=c++17 -ld2d1 -lole32 -lgdi32 -lshell32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*...*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*...*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <d2d1.h>
#include <math.h>
#include <shellapi.h>
#include <deque>
#include <vector>

struct Sample {
    float x;
    float y;
    double t;
};

struct PointF {
    float x;
    float y;
    float age;
};

HWND g_overlayHwnd = NULL;
HANDLE g_threadHandle = NULL;
HHOOK g_mouseHook = NULL;
std::deque<Sample> g_history;

ID2D1Factory* g_pD2DFactory = nullptr;
ID2D1DCRenderTarget* g_pDCRenderTarget = nullptr;
ID2D1Bitmap* g_pCursorBmp = nullptr;

HDC g_hdcMem = NULL;
HBITMAP g_hBitmap = NULL;
void* g_dibBits = nullptr;
int g_bmpW = 0;
int g_bmpH = 0;
int g_dibStride = 0;

HCURSOR g_lastCursor = NULL;
int g_hotX = 0;
int g_hotY = 0;
int g_curW = 32;
int g_curH = 32;

int g_shutterMs = 58;
float g_opacity = 0.78f;
int g_blurRadius = 8;
int g_smoothness = 3;

static double NowMs() {
    static LARGE_INTEGER freq = {};
    static BOOL inited = FALSE;
    if (!inited) {
        QueryPerformanceFrequency(&freq);
        inited = TRUE;
    }
    LARGE_INTEGER c;
    QueryPerformanceCounter(&c);
    return (double)c.QuadPart * 1000.0 / (double)freq.QuadPart;
}

void LoadSettings() {
    g_shutterMs = Wh_GetIntSetting(L"shutter_ms");
    int opacityPct = Wh_GetIntSetting(L"opacity");
    g_blurRadius = Wh_GetIntSetting(L"blur_radius");
    g_smoothness = Wh_GetIntSetting(L"smoothness");

    if (g_shutterMs < 16) g_shutterMs = 16;
    if (g_shutterMs > 160) g_shutterMs = 160;
    if (opacityPct < 1) opacityPct = 78;
    if (opacityPct > 100) opacityPct = 100;
    g_opacity = opacityPct / 100.0f;
    if (g_blurRadius < 2) g_blurRadius = 2;
    if (g_blurRadius > 18) g_blurRadius = 18;
    if (g_smoothness < 0) g_smoothness = 0;
    if (g_smoothness > 4) g_smoothness = 4;
}

bool IsGameRunning() {
    HWND hwnd = GetForegroundWindow();
    if (!hwnd || hwnd == GetDesktopWindow()) return false;

    static HWND s_hwndProgman = FindWindowW(L"Progman", NULL);
    static HWND s_hwndWorkerW = FindWindowW(L"WorkerW", NULL);
    if (hwnd == s_hwndProgman || hwnd == s_hwndWorkerW) return false;

    QUERY_USER_NOTIFICATION_STATE state;
    if (SUCCEEDED(SHQueryUserNotificationState(&state))) {
        if (state == QUNS_RUNNING_D3D_FULL_SCREEN) return true;
    }

    RECT rcApp;
    GetWindowRect(hwnd, &rcApp);
    HMONITOR hMonitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = { sizeof(mi) };
    if (GetMonitorInfo(hMonitor, &mi)) {
        bool isFullscreen = (rcApp.left <= mi.rcMonitor.left &&
                             rcApp.top <= mi.rcMonitor.top &&
                             rcApp.right >= mi.rcMonitor.right &&
                             rcApp.bottom >= mi.rcMonitor.bottom);
        if (isFullscreen) {
            RECT rcClip;
            if (GetClipCursor(&rcClip)) {
                int vW = GetSystemMetrics(SM_CXVIRTUALSCREEN);
                int vH = GetSystemMetrics(SM_CYVIRTUALSCREEN);
                if ((rcClip.right - rcClip.left) < vW || (rcClip.bottom - rcClip.top) < vH)
                    return true;
            }
            CURSORINFO ci = { sizeof(CURSORINFO) };
            if (GetCursorInfo(&ci) && ci.flags == 0) return true;
        }
    }
    return false;
}

static void PushSample(float x, float y) {
    double t = NowMs();
    if (!g_history.empty()) {
        float dx = x - g_history.front().x;
        float dy = y - g_history.front().y;
        if (dx * dx + dy * dy < 0.09f) {
            g_history.front().t = t;
            return;
        }
    }
    Sample s = { x, y, t };
    g_history.push_front(s);
    while (g_history.size() > 512) g_history.pop_back();
}

static void PushScreenPoint(POINT pt) {
    int vX = GetSystemMetrics(SM_XVIRTUALSCREEN);
    int vY = GetSystemMetrics(SM_YVIRTUALSCREEN);
    PushSample((float)(pt.x - vX), (float)(pt.y - vY));
}

static LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && wParam == WM_MOUSEMOVE) {
        MSLLHOOKSTRUCT* info = (MSLLHOOKSTRUCT*)lParam;
        PushScreenPoint(info->pt);
    }
    return CallNextHookEx(g_mouseHook, nCode, wParam, lParam);
}

static void ExpireSamples() {
    double now = NowMs();
    double maxAge = (double)g_shutterMs;
    while (!g_history.empty() && (now - g_history.back().t) > maxAge)
        g_history.pop_back();
}

static PointF Catmull(const Sample& p0, const Sample& p1, const Sample& p2, const Sample& p3, float t) {
    float t2 = t * t;
    float t3 = t2 * t;
    PointF o;
    o.x = 0.5f * ((2.0f * p1.x) + (-p0.x + p2.x) * t +
                  (2.0f * p0.x - 5.0f * p1.x + 4.0f * p2.x - p3.x) * t2 +
                  (-p0.x + 3.0f * p1.x - 3.0f * p2.x + p3.x) * t3);
    o.y = 0.5f * ((2.0f * p1.y) + (-p0.y + p2.y) * t +
                  (2.0f * p0.y - 5.0f * p1.y + 4.0f * p2.y - p3.y) * t2 +
                  (-p0.y + 3.0f * p1.y - 3.0f * p2.y + p3.y) * t3);
    double now = NowMs();
    double tAge = p1.t + (p2.t - p1.t) * (double)t;
    o.age = (float)((now - tAge) / (double)g_shutterMs);
    if (o.age < 0.0f) o.age = 0.0f;
    if (o.age > 1.0f) o.age = 1.0f;
    return o;
}

static void Chaikin(std::vector<Sample>& pts) {
    if (pts.size() < 3) return;
    std::vector<Sample> next;
    next.reserve(pts.size() * 2);
    next.push_back(pts.front());
    for (size_t i = 0; i + 1 < pts.size(); ++i) {
        Sample a = pts[i], b = pts[i + 1], q, r;
        q.x = 0.75f * a.x + 0.25f * b.x;
        q.y = 0.75f * a.y + 0.25f * b.y;
        q.t = a.t * 0.75 + b.t * 0.25;
        r.x = 0.25f * a.x + 0.75f * b.x;
        r.y = 0.25f * a.y + 0.75f * b.y;
        r.t = a.t * 0.25 + b.t * 0.75;
        next.push_back(q);
        next.push_back(r);
    }
    next.push_back(pts.back());
    pts.swap(next);
}

static void BuildSmoothPath(std::vector<PointF>& out) {
    out.clear();
    if (g_history.size() < 2) return;

    std::vector<Sample> pts;
    pts.reserve(g_history.size());
    for (const auto& s : g_history) pts.push_back(s);

    for (int i = 0; i < g_smoothness; ++i) Chaikin(pts);

    const int n = (int)pts.size();
    if (n < 2) return;

    auto at = [&](int i) -> const Sample& {
        if (i < 0) return pts.front();
        if (i >= n) return pts.back();
        return pts[(size_t)i];
    };

    for (int i = 0; i <= n - 2; ++i) {
        const Sample& p0 = at(i - 1);
        const Sample& p1 = at(i);
        const Sample& p2 = at(i + 1);
        const Sample& p3 = at(i + 2);
        float dx = p2.x - p1.x, dy = p2.y - p1.y;
        float dist = sqrtf(dx * dx + dy * dy);
        int steps = (int)(dist / 1.25f + 0.5f);
        if (steps < 1) steps = 1;
        if (steps > 48) steps = 48;
        for (int s = 0; s < steps; ++s)
            out.push_back(Catmull(p0, p1, p2, p3, (float)s / (float)steps));
    }
}

static bool CaptureCursorBitmap() {
    if (!g_pDCRenderTarget) return false;

    CURSORINFO ci = { sizeof(CURSORINFO) };
    if (!GetCursorInfo(&ci) || !ci.hCursor) return g_pCursorBmp != nullptr;
    if (ci.hCursor == g_lastCursor && g_pCursorBmp) return true;

    ICONINFO ii = {};
    if (!GetIconInfo(ci.hCursor, &ii)) return g_pCursorBmp != nullptr;

    int w = GetSystemMetrics(SM_CXCURSOR);
    int h = GetSystemMetrics(SM_CYCURSOR);
    BITMAP bm = {};
    if (ii.hbmColor && GetObject(ii.hbmColor, sizeof(bm), &bm)) {
        w = bm.bmWidth;
        h = bm.bmHeight;
    } else if (ii.hbmMask && GetObject(ii.hbmMask, sizeof(bm), &bm)) {
        w = bm.bmWidth;
        h = bm.bmHeight / 2;
    }
    if (w < 1) w = 32;
    if (h < 1) h = 32;

    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = w;
    bmi.bmiHeader.biHeight = -h;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void* bitsBlack = nullptr;
    void* bitsWhite = nullptr;
    HDC hdcScreen = GetDC(NULL);
    HDC hdcMem = CreateCompatibleDC(hdcScreen);
    HBITMAP dibB = CreateDIBSection(hdcScreen, &bmi, DIB_RGB_COLORS, &bitsBlack, NULL, 0);
    HBITMAP dibW = CreateDIBSection(hdcScreen, &bmi, DIB_RGB_COLORS, &bitsWhite, NULL, 0);

    RECT fill = { 0, 0, w, h };
    HGDIOBJ old = SelectObject(hdcMem, dibB);
    HBRUSH brBlack = CreateSolidBrush(RGB(0, 0, 0));
    FillRect(hdcMem, &fill, brBlack);
    DeleteObject(brBlack);
    DrawIconEx(hdcMem, 0, 0, ci.hCursor, w, h, 0, NULL, DI_NORMAL);

    SelectObject(hdcMem, dibW);
    HBRUSH brWhite = CreateSolidBrush(RGB(255, 255, 255));
    FillRect(hdcMem, &fill, brWhite);
    DeleteObject(brWhite);
    DrawIconEx(hdcMem, 0, 0, ci.hCursor, w, h, 0, NULL, DI_NORMAL);

    int n = w * h;
    unsigned int* black = (unsigned int*)bitsBlack;
    unsigned int* white = (unsigned int*)bitsWhite;
    std::vector<unsigned int> out((size_t)n);

    int maxA = 0;
    for (int i = 0; i < n; ++i) {
        int a = (int)((black[i] >> 24) & 0xFFu);
        if (a > maxA) maxA = a;
    }

    if (maxA > 0) {
        for (int i = 0; i < n; ++i) {
            unsigned int c = black[i];
            unsigned int a = (c >> 24) & 0xFFu;
            unsigned int r = (c >> 16) & 0xFFu;
            unsigned int g = (c >> 8) & 0xFFu;
            unsigned int b = c & 0xFFu;
            out[(size_t)i] = (a << 24) | ((r * a / 255u) << 16) | ((g * a / 255u) << 8) | (b * a / 255u);
        }
    } else {
        for (int i = 0; i < n; ++i) {
            unsigned int bl = black[i];
            unsigned int wh = white[i];
            int br = (int)((bl >> 16) & 0xFFu);
            int bg = (int)((bl >> 8) & 0xFFu);
            int bb = (int)(bl & 0xFFu);
            int wr = (int)((wh >> 16) & 0xFFu);
            int wg = (int)((wh >> 8) & 0xFFu);
            int wb = (int)(wh & 0xFFu);
            int a = (255 - (wr - br) + 255 - (wg - bg) + 255 - (wb - bb)) / 3;
            if (a < 0) a = 0;
            if (a > 255) a = 255;
            out[(size_t)i] = ((unsigned int)a << 24) | ((unsigned int)br << 16) |
                             ((unsigned int)bg << 8) | (unsigned int)bb;
        }
    }

    if (g_pCursorBmp) {
        g_pCursorBmp->Release();
        g_pCursorBmp = nullptr;
    }

    D2D1_BITMAP_PROPERTIES bp = D2D1::BitmapProperties(
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED));
    HRESULT hr = g_pDCRenderTarget->CreateBitmap(
        D2D1::SizeU((UINT32)w, (UINT32)h), out.data(), (UINT32)(w * 4), &bp, &g_pCursorBmp);

    g_hotX = (int)ii.xHotspot;
    g_hotY = (int)ii.yHotspot;
    g_curW = w;
    g_curH = h;
    if (SUCCEEDED(hr) && g_pCursorBmp) g_lastCursor = ci.hCursor;

    SelectObject(hdcMem, old);
    DeleteObject(dibB);
    DeleteObject(dibW);
    DeleteDC(hdcMem);
    ReleaseDC(NULL, hdcScreen);
    if (ii.hbmColor) DeleteObject(ii.hbmColor);
    if (ii.hbmMask) DeleteObject(ii.hbmMask);
    return g_pCursorBmp != nullptr;
}

static bool EnsureBitmap(HDC hdcScreen, int w, int h) {
    if (w < 48) w = 48;
    if (h < 48) h = 48;
    if (g_hBitmap && g_bmpW >= w && g_bmpH >= h) return true;

    if (g_hBitmap) DeleteObject(g_hBitmap);
    if (g_hdcMem) DeleteDC(g_hdcMem);
    if (g_pDCRenderTarget) { g_pDCRenderTarget->Release(); g_pDCRenderTarget = nullptr; }
    if (g_pCursorBmp) { g_pCursorBmp->Release(); g_pCursorBmp = nullptr; }
    g_lastCursor = NULL;

    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = w;
    bmi.bmiHeader.biHeight = -h;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    g_dibBits = nullptr;
    g_hdcMem = CreateCompatibleDC(hdcScreen);
    g_hBitmap = CreateDIBSection(hdcScreen, &bmi, DIB_RGB_COLORS, &g_dibBits, NULL, 0);
    if (!g_hBitmap) return false;
    SelectObject(g_hdcMem, g_hBitmap);
    g_bmpW = w;
    g_bmpH = h;
    g_dibStride = w * 4;
    return true;
}

static void EnsureRT() {
    if (g_pDCRenderTarget || !g_pD2DFactory) return;
    D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
        D2D1_RENDER_TARGET_TYPE_DEFAULT,
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
        0, 0, D2D1_RENDER_TARGET_USAGE_NONE, D2D1_FEATURE_LEVEL_DEFAULT);
    g_pD2DFactory->CreateDCRenderTarget(&props, &g_pDCRenderTarget);
}

static void GaussianBlurPremul(unsigned char* src, int w, int h, int stride, int radius, float sigma) {
    if (radius < 1 || !src) return;
    int ksz = radius * 2 + 1;
    std::vector<float> kernel((size_t)ksz);
    float sum = 0.0f;
    for (int i = 0; i < ksz; ++i) {
        float x = (float)(i - radius);
        float v = expf(-(x * x) / (2.0f * sigma * sigma));
        kernel[(size_t)i] = v;
        sum += v;
    }
    for (int i = 0; i < ksz; ++i) kernel[(size_t)i] /= sum;

    std::vector<unsigned char> tmp((size_t)stride * h);

    for (int y = 0; y < h; ++y) {
        unsigned char* srow = src + y * stride;
        unsigned char* drow = tmp.data() + y * stride;
        for (int x = 0; x < w; ++x) {
            float b = 0, g = 0, r = 0, a = 0;
            for (int k = 0; k < ksz; ++k) {
                int xx = x + k - radius;
                if (xx < 0) xx = 0;
                if (xx >= w) xx = w - 1;
                float wgt = kernel[(size_t)k];
                unsigned char* p = srow + xx * 4;
                b += p[0] * wgt;
                g += p[1] * wgt;
                r += p[2] * wgt;
                a += p[3] * wgt;
            }
            drow[x * 4 + 0] = (unsigned char)(b + 0.5f);
            drow[x * 4 + 1] = (unsigned char)(g + 0.5f);
            drow[x * 4 + 2] = (unsigned char)(r + 0.5f);
            drow[x * 4 + 3] = (unsigned char)(a + 0.5f);
        }
    }

    for (int y = 0; y < h; ++y) {
        unsigned char* drow = src + y * stride;
        for (int x = 0; x < w; ++x) {
            float b = 0, g = 0, r = 0, a = 0;
            for (int k = 0; k < ksz; ++k) {
                int yy = y + k - radius;
                if (yy < 0) yy = 0;
                if (yy >= h) yy = h - 1;
                float wgt = kernel[(size_t)k];
                unsigned char* p = tmp.data() + yy * stride + x * 4;
                b += p[0] * wgt;
                g += p[1] * wgt;
                r += p[2] * wgt;
                a += p[3] * wgt;
            }
            drow[x * 4 + 0] = (unsigned char)(b + 0.5f);
            drow[x * 4 + 1] = (unsigned char)(g + 0.5f);
            drow[x * 4 + 2] = (unsigned char)(r + 0.5f);
            drow[x * 4 + 3] = (unsigned char)(a + 0.5f);
        }
    }
}

static D2D1::Matrix3x2F DirectionalScale(float ux, float uy, float s) {
    float m11 = 1.0f + (s - 1.0f) * ux * ux;
    float m12 = (s - 1.0f) * ux * uy;
    float m21 = (s - 1.0f) * uy * ux;
    float m22 = 1.0f + (s - 1.0f) * uy * uy;
    return D2D1::Matrix3x2F(m11, m12, m21, m22, 0.0f, 0.0f);
}

static void DrawSmear(ID2D1RenderTarget* rt, const std::vector<PointF>& path, int boxX, int boxY) {
    if (!g_pCursorBmp || path.size() < 2) return;

    D2D1_SIZE_F sz = g_pCursorBmp->GetSize();
    float cursorSpan = (float)(g_curW > g_curH ? g_curW : g_curH);
    if (cursorSpan < 8.0f) cursorSpan = 8.0f;

    size_t skip = 2;
    if (skip >= path.size()) skip = 1;

    for (size_t i = path.size() - 1; i >= skip; --i) {
        float tx, ty;
        if (i == 0) {
            tx = path[1].x - path[0].x;
            ty = path[1].y - path[0].y;
        } else if (i + 1 >= path.size()) {
            tx = path[i].x - path[i - 1].x;
            ty = path[i].y - path[i - 1].y;
        } else {
            tx = path[i - 1].x - path[i + 1].x;
            ty = path[i - 1].y - path[i + 1].y;
        }
        float len = sqrtf(tx * tx + ty * ty);
        float ux = 1.0f, uy = 0.0f;
        if (len > 0.001f) {
            ux = tx / len;
            uy = ty / len;
        }

        float stretch = 1.0f + fminf(len / cursorSpan * 2.4f, 3.2f);

        float fade = 1.0f - path[i].age;
        fade = fade * fade * (3.0f - 2.0f * fade);
        float alpha = g_opacity * 0.18f * fade;
        if (alpha < 0.01f) {
            if (i == 0) break;
            continue;
        }

        float x = path[i].x - (float)boxX;
        float y = path[i].y - (float)boxY;

        D2D1::Matrix3x2F m =
            D2D1::Matrix3x2F::Translation(-((float)g_hotX), -((float)g_hotY)) *
            DirectionalScale(ux, uy, stretch) *
            D2D1::Matrix3x2F::Translation(x, y);

        rt->SetTransform(m);
        rt->DrawBitmap(
            g_pCursorBmp,
            D2D1::RectF(0.0f, 0.0f, sz.width, sz.height),
            alpha,
            D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
        if (i == 0) break;
    }
    rt->SetTransform(D2D1::Matrix3x2F::Identity());
}

VOID CALLBACK SmearTimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime) {
    static DWORD lastFullscreenCheck = 0;
    static bool isGameCached = false;

    if (dwTime - lastFullscreenCheck > 400) {
        isGameCached = IsGameRunning();
        lastFullscreenCheck = dwTime;
    }

    POINT pt;
    GetCursorPos(&pt);
    PushScreenPoint(pt);
    ExpireSamples();

    if (isGameCached) {
        g_history.clear();
        ShowWindow(hwnd, SW_HIDE);
        return;
    }

    std::vector<PointF> path;
    BuildSmoothPath(path);
    if (path.size() < 3) {
        ShowWindow(hwnd, SW_HIDE);
        return;
    }

    float minX = path[0].x, maxX = path[0].x, minY = path[0].y, maxY = path[0].y;
    for (const auto& p : path) {
        if (p.x < minX) minX = p.x;
        if (p.x > maxX) maxX = p.x;
        if (p.y < minY) minY = p.y;
        if (p.y > maxY) maxY = p.y;
    }

    int pad = g_blurRadius * 3 + (g_curW > g_curH ? g_curW : g_curH) + 8;
    int boxX = (int)floorf(minX) - pad;
    int boxY = (int)floorf(minY) - pad;
    int boxW = (int)ceilf(maxX) - (int)floorf(minX) + pad * 2;
    int boxH = (int)ceilf(maxY) - (int)floorf(minY) + pad * 2;
    if (boxW < 64) boxW = 64;
    if (boxH < 64) boxH = 64;
    if (boxW > 1600) boxW = 1600;
    if (boxH > 1600) boxH = 1600;

    int vX = GetSystemMetrics(SM_XVIRTUALSCREEN);
    int vY = GetSystemMetrics(SM_YVIRTUALSCREEN);

    HDC hdcScreen = GetDC(NULL);
    if (!EnsureBitmap(hdcScreen, boxW, boxH)) {
        ReleaseDC(NULL, hdcScreen);
        return;
    }
    EnsureRT();
    if (!g_pDCRenderTarget) {
        ReleaseDC(NULL, hdcScreen);
        return;
    }

    RECT rc = { 0, 0, g_bmpW, g_bmpH };
    g_pDCRenderTarget->BindDC(g_hdcMem, &rc);
    g_pDCRenderTarget->BeginDraw();
    g_pDCRenderTarget->Clear(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f));

    CaptureCursorBitmap();
    DrawSmear(g_pDCRenderTarget, path, boxX, boxY);

    HRESULT hr = g_pDCRenderTarget->EndDraw();
    if (hr == D2DERR_RECREATE_TARGET) {
        g_pDCRenderTarget->Release();
        g_pDCRenderTarget = nullptr;
        if (g_pCursorBmp) { g_pCursorBmp->Release(); g_pCursorBmp = nullptr; }
        g_lastCursor = NULL;
    }

    if (g_dibBits) {
        float sigma = (float)g_blurRadius * 0.55f;
        if (sigma < 1.2f) sigma = 1.2f;
        GaussianBlurPremul((unsigned char*)g_dibBits, boxW, boxH, g_bmpW * 4, g_blurRadius, sigma);
    }

    SetWindowPos(hwnd, HWND_TOPMOST, vX + boxX, vY + boxY, boxW, boxH, SWP_NOACTIVATE | SWP_SHOWWINDOW);

    BLENDFUNCTION blend = { 0 };
    blend.BlendOp = AC_SRC_OVER;
    blend.SourceConstantAlpha = 255;
    blend.AlphaFormat = AC_SRC_ALPHA;

    POINT ptPos = { vX + boxX, vY + boxY };
    SIZE sizeWnd = { boxW, boxH };
    POINT ptSrc = { 0, 0 };
    UpdateLayeredWindow(hwnd, hdcScreen, &ptPos, &sizeWnd, g_hdcMem, &ptSrc, 0, &blend, ULW_ALPHA);
    ReleaseDC(NULL, hdcScreen);
}

DWORD WINAPI OverlayThreadProc(LPVOID lpParam) {
    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);

    D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &g_pD2DFactory);

    HINSTANCE hInstance = GetModuleHandle(NULL);
    const wchar_t CLASS_NAME[] = L"UltraSmoothCursorBlurClass";

    WNDCLASS wc = {};
    wc.lpfnWndProc = DefWindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    RegisterClass(&wc);

    g_overlayHwnd = CreateWindowEx(
        WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        CLASS_NAME, L"UltraSmoothOverlay", WS_POPUP,
        0, 0, 64, 64, NULL, NULL, hInstance, NULL);
    if (!g_overlayHwnd) return 0;

    g_mouseHook = SetWindowsHookExW(WH_MOUSE_LL, LowLevelMouseProc, GetModuleHandle(NULL), 0);

    POINT pt;
    GetCursorPos(&pt);
    PushScreenPoint(pt);
    SetTimer(g_overlayHwnd, 1, 8, SmearTimerProc);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (g_mouseHook) { UnhookWindowsHookEx(g_mouseHook); g_mouseHook = NULL; }
    if (g_pCursorBmp) { g_pCursorBmp->Release(); g_pCursorBmp = nullptr; }
    if (g_pDCRenderTarget) { g_pDCRenderTarget->Release(); g_pDCRenderTarget = nullptr; }
    if (g_pD2DFactory) { g_pD2DFactory->Release(); g_pD2DFactory = nullptr; }
    if (g_hBitmap) DeleteObject(g_hBitmap);
    if (g_hdcMem) DeleteDC(g_hdcMem);
    DestroyWindow(g_overlayHwnd);
    UnregisterClass(CLASS_NAME, hInstance);
    CoUninitialize();
    return 0;
}

BOOL WhTool_ModInit() {
    LoadSettings();
    g_threadHandle = CreateThread(NULL, 0, OverlayThreadProc, NULL, 0, NULL);
    return TRUE;
}

void WhTool_ModUninit() {
    if (g_overlayHwnd) PostMessage(g_overlayHwnd, WM_QUIT, 0, 0);
    if (g_threadHandle) {
        WaitForSingleObject(g_threadHandle, INFINITE);
        CloseHandle(g_threadHandle);
    }
}

void WhTool_ModSettingsChanged() {
    LoadSettings();
}

////////////////////////////////////////////////////////////////////////////////
// Windhawk tool mod implementation for mods which don't need to inject to other
// processes or hook other functions. Context:
// https://github.com/ramensoftware/windhawk/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process
//
// The mod will load and run in a dedicated windhawk.exe process.
//
// Paste the code below as part of the mod code, and use these callbacks:
// * WhTool_ModInit
// * WhTool_ModSettingsChanged
// * WhTool_ModUninit
//
// Currently, other callbacks are not supported.

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

    if (isExcluded) return FALSE;

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
        if (!WhTool_ModInit()) ExitProcess(1);

        IMAGE_DOS_HEADER* dosHeader = (IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);
        IMAGE_NT_HEADERS* ntHeaders = (IMAGE_NT_HEADERS*)((BYTE*)dosHeader + dosHeader->e_lfanew);
        DWORD entryPointRVA = ntHeaders->OptionalHeader.AddressOfEntryPoint;
        void* entryPoint = (BYTE*)dosHeader + entryPointRVA;
        Wh_SetFunctionHook(entryPoint, (void*)EntryPoint_Hook, nullptr);
        return TRUE;
    }

    if (isToolModProcess) return FALSE;
    g_isToolModProcessLauncher = true;
    return TRUE;
}

void Wh_ModAfterInit() {
    if (!g_isToolModProcessLauncher) return;

    WCHAR currentProcessPath[MAX_PATH];
    switch (GetModuleFileName(nullptr, currentProcessPath, ARRAYSIZE(currentProcessPath))) {
        case 0:
        case ARRAYSIZE(currentProcessPath):
            Wh_Log(L"GetModuleFileName failed");
            return;
    }

    WCHAR commandLine[MAX_PATH + 2 + (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) - 1];
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", currentProcessPath, WH_MOD_ID);

    HMODULE kernelModule = GetModuleHandle(L"kernelbase.dll");
    if (!kernelModule) {
        kernelModule = GetModuleHandle(L"kernel32.dll");
        if (!kernelModule) return;
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
        (CreateProcessInternalW_t)GetProcAddress(kernelModule, "CreateProcessInternalW");
    if (!pCreateProcessInternalW) return;

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
    if (g_isToolModProcessLauncher) return;
    WhTool_ModSettingsChanged();
}

void Wh_ModUninit() {
    if (g_isToolModProcessLauncher) return;
    WhTool_ModUninit();
    ExitProcess(0);
}