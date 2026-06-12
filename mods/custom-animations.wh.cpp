// ==WindhawkMod==
// @id            custom-animations
// @name          Animation & Wobbly Mod
// @description   Ultra-smooth Open/Close/Minimize animation combined with Wobbly Windows physics. Fully powered by Direct2D.
// @version       1.3.1
// @author        Shoaib Hassan
// @github        https://github.com/shoaibhassan2
// @include       *
// @compilerOptions -ldwmapi -lgdi32 -ld2d1 -luser32 -lcomctl32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Genie, Magic Lamp, Squash & Wobbly Animation Mod (Pure D2D Edition)

Replaces the default Windows minimize and restore animations with smooth geometry deformation effects.
Includes real-time wobbly window physics during dragging and resizing.
Both engines are now strictly powered by Direct2D for massive performance gains, low latency, and smooth anti-aliasing.

- KDE Magic Lamp: fluid 4-direction spatial bend
- macOS Genie: cosine-eased suction effect to dock
- MacSine: liquid wobble motion during animation
- Squash: scale and fade to/from taskbar icon
- Wobbly Windows: Interactive jelly physics when moving/resizing windows (D2D Ported).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- duration_ms: 450
  $name: Animation Duration (ms)
  $description: Time it takes for the main animation to complete.

- animation_style: KDE
  $name: Animation Style
  $description: Choose the visual style of the minimize effect.
  $options:
    - KDE: KDE Magic Lamp
    - MacPinch: macOS Genie (Pinch Effect)
    - MacSine: macOS Genie (Sine Wave Wobble)
    - Squash: KDE Squash (Scale + Fade to icon)

- bounce_enabled: true
  $name: Enable Bounce Effect on Restore
  $description: Adds an elastic pop effect when the window finishes restoring.

- bounce_strength: 30
  $name: Bounce Strength (1-100)
  $description: How intense the elasticity of the bounce is.

- bounce_duration: 300
  $name: Bounce Duration (ms)
  $description: How long the bounce effect lasts after the main animation finishes.

- wobbly_enabled: true
  $name: Enable Wobbly Windows
  $description: Wobble the window when moving or resizing it.

- wobbly_preset: 4
  $name: Wobbly Physics Preset
  $description: Adjust the stiffness and elasticity of the window dragging physics.
  $options:
    - 0: Very jelly / elastic
    - 1: Soft wobble
    - 2: Balanced smooth
    - 3: KDE-like default
    - 4: Very soft / floaty
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <dwmapi.h>
#include <d2d1.h>
#include <commctrl.h>
#include <tlhelp32.h>
#include <math.h>
#include <atomic>
#include <unordered_map>
#include <mutex>
#include <vector>

#ifndef DWMWA_EXTENDED_FRAME_BOUNDS
#define DWMWA_EXTENDED_FRAME_BOUNDS 9
#endif

#ifndef PW_RENDERFULLCONTENT
#define PW_RENDERFULLCONTENT 2
#endif

#define PI 3.14159265f
#define X_TILES 20
#define Y_TILES 20

// -------------------------------------------------------------------------
// Shared Structs & Typedefs
// -------------------------------------------------------------------------
struct Geometry { float x, y, width, height; };
enum IconPosition { POS_TOP, POS_BOTTOM, POS_LEFT, POS_RIGHT };

typedef LRESULT (WINAPI *DefWindowProcW_t)(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
DefWindowProcW_t DefWindowProcW_Original;

typedef BOOL (WINAPI *ShowWindow_t)(HWND hWnd, int nCmdShow);
ShowWindow_t ShowWindow_Original;

using CreateWindowExW_t = HWND(WINAPI*)(DWORD, LPCWSTR, LPCWSTR, DWORD, int, int, int, int, HWND, HMENU, HINSTANCE, LPVOID);
CreateWindowExW_t CreateWindowExW_Original;

using CreateWindowExA_t = HWND(WINAPI*)(DWORD, LPCSTR, LPCSTR, DWORD, int, int, int, int, HWND, HMENU, HINSTANCE, LPVOID);
CreateWindowExA_t CreateWindowExA_Original;

// -------------------------------------------------------------------------
// Globals
// -------------------------------------------------------------------------
ID2D1Factory* g_d2dFactory = nullptr;

struct GhostAnimData {
    HWND hGhost;
    HWND hRealWnd;
    HBITMAP hBitmap;
    void* pBits; 
    RECT targetRect;
    int width;
    int height;
    int targetDockX; 
    BOOL isRising;
    LONG_PTR originalExStyle;
    
    int durationMs;
    bool bounceEnabled;
    int bounceStrength;
    int bounceDurationMs;
};

std::unordered_map<HWND, std::pair<HBITMAP, void*>> g_SnapshotCache;
std::unordered_map<HWND, RECT> g_RectCache;
std::unordered_map<HWND, int> g_IconPositions; 
std::mutex g_CacheMutex;

std::atomic<int> g_durationMs{450};
std::atomic<int> g_animationStyle{0};
std::atomic<bool> g_bounceEnabled{true};
std::atomic<int> g_bounceStrength{30};
std::atomic<int> g_bounceDurationMs{300};

// -------------------------------------------------------------------------
// Wobbly Windows State
// -------------------------------------------------------------------------
bool g_engineInitialized = false;

UINT g_wmAttach = 0;
UINT g_wmDetach = 0;

struct Pair { float x, y; };

struct WobblyInfos {
    Pair origin[16], position[16], velocity[16], acceleration[16], buffer[16];
    bool constraint[16], wobblying, can_wobble_top, can_wobble_bottom, can_wobble_left, can_wobble_right;
};

struct ParameterSet { float stiffness, drag, moveFactor, minVelocity, maxVelocity, stopVelocity, minAcc, maxAcc, stopAcc; };
static const ParameterSet set_0 = { 0.15f, 0.80f, 0.10f, 0.0f, 1000.0f, 0.5f, 0.0f, 1000.0f, 0.5f };
static const ParameterSet set_1 = { 0.10f, 0.85f, 0.10f, 0.0f, 1000.0f, 0.5f, 0.0f, 1000.0f, 0.5f };
static const ParameterSet set_2 = { 0.06f, 0.90f, 0.10f, 0.0f, 1000.0f, 0.5f, 0.0f, 1000.0f, 0.5f };
static const ParameterSet set_3 = { 0.03f, 0.92f, 0.20f, 0.0f, 1000.0f, 0.5f, 0.0f, 1000.0f, 0.5f };
static const ParameterSet set_4 = { 0.01f, 0.97f, 0.25f, 0.0f, 1000.0f, 0.5f, 0.0f, 1000.0f, 0.5f };

static ParameterSet g_params = set_4;
std::atomic<bool> g_wobblyEnabled{true};

static HWND g_mainHwnd = NULL, g_overlayHwnd = NULL;
static int g_capX = 0, g_capY = 0, g_capW = 0, g_capH = 0;
static WobblyInfos g_wwi = { 0 };
static bool g_isMoving = false, g_isSettling = false;
static DWORD g_lastTick = 0;
static LONG_PTR g_oldExStyle = 0;
static int g_screenX, g_screenY, g_screenW, g_screenH;
static Geometry g_currentRect = { 0 };

ID2D1DCRenderTarget* g_wobblyRT = nullptr;
ID2D1BitmapBrush* g_wobblyBrush = nullptr;
HBITMAP g_wobblyTargetBmp = NULL;
void* g_wobblyTargetBits = nullptr;
HDC g_wobblyMemDC = NULL;


// -------------------------------------------------------------------------
// Utils
// -------------------------------------------------------------------------
void LoadSettings() {
    int ms = Wh_GetIntSetting(L"duration_ms");
    if (ms < 50) ms = 50;
    if (ms > 2000) ms = 2000;
    g_durationMs.store(ms, std::memory_order_relaxed);

    PCWSTR styleStr = Wh_GetStringSetting(L"animation_style");
    int style = 0;
    if (styleStr) {
        if (wcscmp(styleStr, L"MacPinch") == 0) style = 1;
        else if (wcscmp(styleStr, L"MacSine") == 0) style = 2;
        else if (wcscmp(styleStr, L"Squash") == 0) style = 3;
        Wh_FreeStringSetting(styleStr);
    }
    g_animationStyle.store(style, std::memory_order_relaxed);

    g_bounceEnabled.store(Wh_GetIntSetting(L"bounce_enabled") != 0, std::memory_order_relaxed);
    g_bounceStrength.store(Wh_GetIntSetting(L"bounce_strength"), std::memory_order_relaxed);
    g_bounceDurationMs.store(Wh_GetIntSetting(L"bounce_duration"), std::memory_order_relaxed);

    g_wobblyEnabled.store(Wh_GetIntSetting(L"wobbly_enabled") != 0, std::memory_order_relaxed);
    int preset = Wh_GetIntSetting(L"wobbly_preset");
    switch (preset) {
        case 0: g_params = set_0; break;
        case 1: g_params = set_1; break;
        case 2: g_params = set_2; break;
        case 3: g_params = set_3; break;
        case 4: default: g_params = set_4; break;
    }
}

void SetDwmTransitions(HWND hWnd, BOOL enable) {
    BOOL disable = !enable;
    DwmSetWindowAttribute(hWnd, DWMWA_TRANSITIONS_FORCEDISABLED, &disable, sizeof(disable));
}

static ID2D1PathGeometry* CreateQuadGeo(
    ID2D1Factory* factory,
    D2D1_POINT_2F p0, D2D1_POINT_2F p1,
    D2D1_POINT_2F p2, D2D1_POINT_2F p3)
{
    ID2D1PathGeometry* geo = nullptr;
    factory->CreatePathGeometry(&geo);
    if (!geo) return nullptr;
    ID2D1GeometrySink* sink = nullptr;
    geo->Open(&sink);
    sink->BeginFigure(p0, D2D1_FIGURE_BEGIN_FILLED);
    sink->AddLine(p1);
    sink->AddLine(p2);
    sink->AddLine(p3);
    sink->EndFigure(D2D1_FIGURE_END_CLOSED);
    sink->Close();
    sink->Release();
    return geo;
}

static D2D1_POINT_2F BloatPoint(D2D1_POINT_2F p, D2D1_POINT_2F c) {
    float dx = p.x - c.x; float dy = p.y - c.y;
    float len = sqrt(dx*dx + dy*dy);
    if (len < 0.001f) return p;
    return D2D1::Point2F(p.x + (dx/len)*0.5f, p.y + (dy/len)*0.5f);
}

// -------------------------------------------------------------------------
// Custom Animation Engine
// -------------------------------------------------------------------------
static inline float CubicEaseIn(float t) { return t * t * t; }
static inline float CubicEaseOut(float t) { float t1 = t - 1.0f; return t1 * t1 * t1 + 1.0f; }

static void CalculateLampVertexKDE(float tx, float ty, float p, const Geometry& w, const Geometry& i, int pos, float *outX, float *outY) {
    float quadX = tx * w.width;
    float quadY = ty * w.height;
    float quadFactor, offset, denom, p_progress, absX, targetX, targetY;

    if (pos == POS_BOTTOM) {
        float h3 = w.height * w.height * w.height;
        float maxY = i.y - w.y;
        quadFactor = quadY + (w.height - quadY) * p;
        offset = (i.y + quadY - w.y) * p * ((quadFactor * quadFactor * quadFactor) / h3);
        denom = i.y - w.y - quadY;
        p_progress = (denom != 0.0f) ? fminf(offset / denom, 1.0f) : 1.0f;
        p_progress = fabsf(p_progress);
        targetX = i.x + i.width * tx;
        absX = quadX + w.x;
        *outX = (targetX - absX) * p_progress + absX;
        *outY = w.y + fminf(maxY, quadY + offset);
    } else if (pos == POS_TOP) {
        float h3 = w.height * w.height * w.height;
        float minY = i.y + i.height - w.y;
        quadFactor = w.height - quadY + quadY * p;
        offset = (w.y - i.height + w.height + quadY - i.y) * p * ((quadFactor * quadFactor * quadFactor) / h3);
        denom = w.y - i.height - i.y + quadY;
        p_progress = (denom != 0.0f) ? fminf(offset / denom, 1.0f) : 1.0f;
        offset = -offset;
        p_progress = fabsf(p_progress);
        targetX = i.x + i.width * tx;
        absX = quadX + w.x;
        *outX = (targetX - absX) * p_progress + absX;
        *outY = w.y + fmaxf(minY, quadY + offset);
    } else if (pos == POS_LEFT) {
        float w3 = w.width * w.width * w.width;
        float minX = i.x + i.width - w.x;
        quadFactor = w.width - quadX + quadX * p;
        offset = (w.x - i.width + w.width + quadX - i.x) * p * ((quadFactor * quadFactor * quadFactor) / w3);
        denom = w.x - i.width - i.x + quadX;
        p_progress = (denom != 0.0f) ? fminf(offset / denom, 1.0f) : 1.0f;
        offset = -offset;
        p_progress = fabsf(p_progress);
        targetY = i.y + i.height * ty;
        float absY = quadY + w.y;
        *outY = (targetY - absY) * p_progress + absY;
        *outX = w.x + fmaxf(minX, quadX + offset);
    } else if (pos == POS_RIGHT) {
        float w3 = w.width * w.width * w.width;
        float maxX = i.x - w.x;
        quadFactor = quadX + (w.width - quadX) * p;
        offset = (i.x + quadX - w.x) * p * ((quadFactor * quadFactor * quadFactor) / w3);
        denom = i.x - w.x - quadX;
        p_progress = (denom != 0.0f) ? fminf(offset / denom, 1.0f) : 1.0f;
        p_progress = fabsf(p_progress);
        targetY = i.y + i.height * ty;
        float absY = quadY + w.y;
        *outY = (targetY - absY) * p_progress + absY;
        *outX = w.x + fminf(maxX, quadX + offset);
    }
}

static void CalculateLampVertexMacOS(float tx, float ty, float p, const Geometry& w, const Geometry& i, int style, float *outX, float *outY) {
    float split = 0.3f;
    float k = (p <= split) ? (p / split) : 1.0f;
    float j = (p > split) ? ((p - split) / (1.0f - split)) : 0.0f;

    float expandHeight = (i.y - w.y - w.height);
    float fullHeight = (i.y - w.y) - (expandHeight * (1.0f - k));
    float height = fullHeight - (j * fullHeight);

    float y = ty * height;
    float x = tx * (i.width) + tx * (w.width - i.width) * (1.0f - j) * (1.0f - ty) + tx * (w.width - i.width) * (1.0f - k) * ty;

    float offsetX = (i.x - w.x) * (y / (fullHeight + 0.1f)) * k + (i.x - w.x) * j;
    float offsetY = i.y - w.y - height - (expandHeight * (1.0f - k));

    float effectX;
    if (style == 2) { 
        effectX = sinf((height - y) / fullHeight * PI * 4.0f) * w.width / 14.0f * k;
    } else { 
        effectX = sinf(((height - y) / fullHeight) * 2.0f * PI + PI) * (w.x + w.width * tx - (i.x + i.width * tx)) / 7.0f * k;
    }

    *outX = w.x + x + offsetX + effectX;
    *outY = w.y + y + offsetY;
}

void ShowGhostSync(GhostAnimData* data) {
    if (!data || !data->hGhost) return;
    
    int vLeft = GetSystemMetrics(SM_XVIRTUALSCREEN);
    int vTop = GetSystemMetrics(SM_YVIRTUALSCREEN);
    int vWidth = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    int vHeight = GetSystemMetrics(SM_CYVIRTUALSCREEN);

    HDC hScreenDC = GetDC(NULL);
    HDC hMemDC = CreateCompatibleDC(hScreenDC);
    
    BITMAPINFO bmi = {{0}};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = vWidth;
    bmi.bmiHeader.biHeight = -vHeight; 
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void* pTargetBits = nullptr;
    HBITMAP hTargetBmp = CreateDIBSection(hScreenDC, &bmi, DIB_RGB_COLORS, &pTargetBits, NULL, 0);
    HBITMAP hOldBmp = (HBITMAP)SelectObject(hMemDC, hTargetBmp);

    HDC hSnapDC = CreateCompatibleDC(hScreenDC);
    HBITMAP hOldSnap = (HBITMAP)SelectObject(hSnapDC, data->hBitmap);
    
    BitBlt(hMemDC, data->targetRect.left - vLeft, data->targetRect.top - vTop, data->width, data->height, hSnapDC, 0, 0, SRCCOPY);
    
    SelectObject(hSnapDC, hOldSnap);
    DeleteDC(hSnapDC);

    POINT ptDst = { vLeft, vTop };
    SIZE sz = { vWidth, vHeight };
    POINT ptSrc = { 0, 0 };
    BLENDFUNCTION bf = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
    
    UpdateLayeredWindow(data->hGhost, NULL, &ptDst, &sz, hMemDC, &ptSrc, 0, &bf, ULW_ALPHA);
    ShowWindow(data->hGhost, SW_SHOWNOACTIVATE);

    SelectObject(hMemDC, hOldBmp);
    DeleteObject(hTargetBmp);
    DeleteDC(hMemDC);
    ReleaseDC(NULL, hScreenDC);
}

DWORD WINAPI GhostAnimationThread(LPVOID lpParam) {
    GhostAnimData* data = (GhostAnimData*)lpParam;
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);

    int style = g_animationStyle.load(std::memory_order_relaxed);
    int xTiles = (style == 1 || style == 2) ? 60 : 40;
    int yTiles = (style == 1 || style == 2) ? 60 : 40;

    int vLeft = GetSystemMetrics(SM_XVIRTUALSCREEN);
    int vTop = GetSystemMetrics(SM_YVIRTUALSCREEN);
    int vWidth = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    int vHeight = GetSystemMetrics(SM_CYVIRTUALSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    HWND hGhost = data->hGhost;

    HDC hScreenDC = GetDC(NULL);
    HDC hMemDC = CreateCompatibleDC(hScreenDC);
    
    BITMAPINFO bmi = {{0}};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = vWidth;
    bmi.bmiHeader.biHeight = -vHeight; 
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void* pTargetBits = nullptr;
    HBITMAP hTargetBmp = CreateDIBSection(hScreenDC, &bmi, DIB_RGB_COLORS, &pTargetBits, NULL, 0);
    HBITMAP hOldBmp = (HBITMAP)SelectObject(hMemDC, hTargetBmp);

    ID2D1DCRenderTarget* rt = nullptr;
    ID2D1Bitmap* snapshotBmp = nullptr;
    ID2D1BitmapBrush* bmpBrush = nullptr;

    if (g_d2dFactory) {
        D2D1_RENDER_TARGET_PROPERTIES rtProps = D2D1::RenderTargetProperties(
            D2D1_RENDER_TARGET_TYPE_SOFTWARE,
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
            0, 0, D2D1_RENDER_TARGET_USAGE_GDI_COMPATIBLE, D2D1_FEATURE_LEVEL_DEFAULT
        );
        g_d2dFactory->CreateDCRenderTarget(&rtProps, &rt);
        if (rt) {
            D2D1_BITMAP_PROPERTIES bmpProps = D2D1::BitmapProperties(
                D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)
            );
            rt->CreateBitmap(D2D1::SizeU(data->width, data->height), data->pBits, data->width * 4, bmpProps, &snapshotBmp);
            if (snapshotBmp) {
                D2D1_BITMAP_BRUSH_PROPERTIES brushProps = D2D1::BitmapBrushProperties(
                    D2D1_EXTEND_MODE_CLAMP, D2D1_EXTEND_MODE_CLAMP,
                    D2D1_BITMAP_INTERPOLATION_MODE_LINEAR
                );
                rt->CreateBitmapBrush(snapshotBmp, &brushProps, nullptr, &bmpBrush);
            }
        }
    }

    Geometry wGeom = { (float)data->targetRect.left, (float)data->targetRect.top, (float)data->width, (float)data->height };
    Geometry iGeom = { (float)data->targetDockX - 20.0f, (float)screenHeight - 40.0f, 40.0f, 40.0f };

    int position = POS_BOTTOM;
    float dx = (iGeom.x + iGeom.width / 2.0f) - (wGeom.x + wGeom.width / 2.0f);
    float dy = (iGeom.y + iGeom.height / 2.0f) - (wGeom.y + wGeom.height / 2.0f);
    if (fabsf(dx) > fabsf(dy)) position = (dx > 0) ? POS_RIGHT : POS_LEFT;
    else position = (dy > 0) ? POS_BOTTOM : POS_TOP;

    float targetScaleX = iGeom.width / wGeom.width;
    float targetScaleY = iGeom.height / wGeom.height;
    float startX = wGeom.x - vLeft;
    float startY = wGeom.y - vTop;
    float endX = iGeom.x - vLeft;
    float endY = iGeom.y - vTop;

    const double animDur = (double)data->durationMs;
    const double bounceDur = (data->isRising && data->bounceEnabled) ? (double)data->bounceDurationMs : 0.0;
    const double totalMs = animDur + bounceDur;

    LARGE_INTEGER qpcFreq, qpcStart, qpcNow;
    QueryPerformanceFrequency(&qpcFreq);
    QueryPerformanceCounter(&qpcStart);

    std::vector<std::vector<D2D1_POINT_2F>> grid(yTiles + 1, std::vector<D2D1_POINT_2F>(xTiles + 1));
    MSG msg;
    BOOL firstFrame = TRUE;

    for (;;) {
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg); DispatchMessage(&msg);
        }

        QueryPerformanceCounter(&qpcNow);
        double elapsedMs = (qpcNow.QuadPart - qpcStart.QuadPart) * 1000.0 / qpcFreq.QuadPart;
        BOOL lastFrame = (elapsedMs >= totalMs);
        
        float raw_p = (float)fmin(elapsedMs / animDur, 1.0);
        float t;

        if (style == 1 || style == 2) { 
            float eased_p = 0.5f * (1.0f - cosf(raw_p * PI));
            t = data->isRising ? (1.0f - eased_p) : eased_p;
        } else if (style == 3) {
            t = data->isRising ? CubicEaseOut(raw_p) : CubicEaseIn(raw_p);
        } else { 
            t = data->isRising ? (1.0f - raw_p) : raw_p;
        }

        float bounceScale = 1.0f;
        if (data->isRising && data->bounceEnabled && elapsedMs > animDur) {
            float tB = (float)((elapsedMs - animDur) / bounceDur);
            if (tB > 1.0f) tB = 1.0f;
            float amp = data->bounceStrength / 100.0f * 0.16f;
            bounceScale = 1.0f + amp * sinf(tB * PI);
        }

        if (rt && bmpBrush) {
            RECT bindRect = { 0, 0, vWidth, vHeight };
            rt->BindDC(hMemDC, &bindRect);
            rt->BeginDraw();
            rt->Clear(D2D1::ColorF(0, 0, 0, 0));
            rt->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

            float opacity = 1.0f;

            if (style == 3) {
                float transX, transY, scaleX, scaleY;
                if (data->isRising) {
                    transX = endX + (startX - endX) * t;
                    transY = endY + (startY - endY) * t;
                    scaleX = targetScaleX + (1.0f - targetScaleX) * t;
                    scaleY = targetScaleY + (1.0f - targetScaleY) * t;
                    opacity = t;
                } else {
                    transX = startX + (endX - startX) * t;
                    transY = startY + (endY - startY) * t;
                    scaleX = 1.0f + (targetScaleX - 1.0f) * t;
                    scaleY = 1.0f + (targetScaleY - 1.0f) * t;
                    opacity = 1.0f - t;
                }
                
                if (bounceScale != 1.0f) {
                    float currentW = scaleX * data->width;
                    float currentH = scaleY * data->height;
                    transX -= (currentW * (bounceScale - 1.0f)) / 2.0f;
                    transY -= (currentH * (bounceScale - 1.0f)) / 2.0f;
                    scaleX *= bounceScale;
                    scaleY *= bounceScale;
                }

                D2D1_MATRIX_3X2_F transform = D2D1::Matrix3x2F::Scale(scaleX, scaleY) * D2D1::Matrix3x2F::Translation(transX, transY);
                rt->SetTransform(transform);
                D2D1_RECT_F rect = D2D1::RectF(0, 0, wGeom.width, wGeom.height);
                rt->DrawBitmap(snapshotBmp, &rect);
                rt->SetTransform(D2D1::Matrix3x2F::Identity());
            } else {
                Geometry currentWGeom = wGeom;
                
                if (bounceScale != 1.0f) {
                    float centerX = wGeom.x + wGeom.width / 2.0f;
                    float centerY = wGeom.y + wGeom.height / 2.0f;
                    currentWGeom.width = wGeom.width * bounceScale;
                    currentWGeom.height = wGeom.height * bounceScale;
                    currentWGeom.x = centerX - currentWGeom.width / 2.0f;
                    currentWGeom.y = centerY - currentWGeom.height / 2.0f;
                }

                for (int y = 0; y <= yTiles; y++) {
                    for (int x = 0; x <= xTiles; x++) {
                        float tx = (float)x / xTiles, ty = (float)y / yTiles;
                        float px, py;
                        if (style == 1 || style == 2) { 
                            CalculateLampVertexMacOS(tx, ty, t, currentWGeom, iGeom, style, &px, &py);
                        } else { 
                            CalculateLampVertexKDE(tx, ty, t, currentWGeom, iGeom, position, &px, &py);
                        }
                        grid[y][x] = D2D1::Point2F(px - vLeft, py - vTop);
                    }
                }

                ID2D1PathGeometry* outlineGeo = nullptr;
                g_d2dFactory->CreatePathGeometry(&outlineGeo);
                if (outlineGeo) {
                    ID2D1GeometrySink* sink = nullptr;
                    outlineGeo->Open(&sink);
                    sink->BeginFigure(grid[0][0], D2D1_FIGURE_BEGIN_FILLED);
                    for (int x = 1; x <= xTiles; x++) sink->AddLine(grid[0][x]);
                    for (int y = 1; y <= yTiles; y++) sink->AddLine(grid[y][xTiles]);
                    for (int x = xTiles - 1; x >= 0; x--) sink->AddLine(grid[yTiles][x]);
                    for (int y = yTiles - 1; y >= 0; y--) sink->AddLine(grid[y][0]);
                    sink->EndFigure(D2D1_FIGURE_END_CLOSED);
                    sink->Close();

                    ID2D1Layer* layer = nullptr;
                    rt->CreateLayer(&layer);
                    D2D1_LAYER_PARAMETERS layerParams = D2D1::LayerParameters();
                    layerParams.geometricMask = outlineGeo;
                    layerParams.maskAntialiasMode = D2D1_ANTIALIAS_MODE_PER_PRIMITIVE;
                    rt->PushLayer(&layerParams, layer);

                    for (int y = 0; y < yTiles; y++) {
                        for (int x = 0; x < xTiles; x++) {
                            D2D1_POINT_2F p1 = grid[y][x];     
                            D2D1_POINT_2F p2 = grid[y][x+1];   
                            D2D1_POINT_2F p3 = grid[y+1][x];   
                            D2D1_POINT_2F p4 = grid[y+1][x+1]; 

                            D2D1_POINT_2F c = D2D1::Point2F((p1.x+p2.x+p3.x+p4.x)/4.0f, (p1.y+p2.y+p3.y+p4.y)/4.0f);
                            ID2D1PathGeometry* quadGeo = CreateQuadGeo(g_d2dFactory, 
                                BloatPoint(p1, c), BloatPoint(p2, c), BloatPoint(p4, c), BloatPoint(p3, c));

                            if (quadGeo) {
                                float sx = ((float)x / xTiles) * wGeom.width;
                                float sy = ((float)y / yTiles) * wGeom.height;
                                float sw = wGeom.width / xTiles;
                                float sh = wGeom.height / yTiles;

                                float m11 = (p2.x - p1.x) / sw;
                                float m12 = (p2.y - p1.y) / sw;
                                float m21 = (p3.x - p1.x) / sh;
                                float m22 = (p3.y - p1.y) / sh;
                                float m31 = p1.x - sx * m11 - sy * m21;
                                float m32 = p1.y - sx * m12 - sy * m22;

                                bmpBrush->SetTransform(D2D1::Matrix3x2F(m11, m12, m21, m22, m31, m32));
                                rt->FillGeometry(quadGeo, bmpBrush);
                                quadGeo->Release();
                            }
                        }
                    }
                    rt->PopLayer();
                    if (layer) layer->Release();
                    outlineGeo->Release();
                }
            }
            rt->EndDraw();

            POINT ptDst = { vLeft, vTop };
            SIZE sz = { vWidth, vHeight };
            POINT ptSrc = { 0, 0 };
            BLENDFUNCTION bf = { AC_SRC_OVER, 0, (BYTE)(opacity * 255.0f), AC_SRC_ALPHA };
            UpdateLayeredWindow(hGhost, NULL, &ptDst, &sz, hMemDC, &ptSrc, 0, &bf, ULW_ALPHA);
        }

        if (firstFrame) {
            ShowWindow(hGhost, SW_SHOWNOACTIVATE);
            firstFrame = FALSE;
        }
        if (lastFrame) break;
        DwmFlush();
    }

    if (data->isRising) {
        SetLayeredWindowAttributes(data->hRealWnd, 0, 255, LWA_ALPHA);
        if (!(data->originalExStyle & WS_EX_LAYERED)) {
            SetWindowLongPtrW(data->hRealWnd, GWL_EXSTYLE, data->originalExStyle);
        }
    }

    SetDwmTransitions(data->hRealWnd, TRUE);

    if (bmpBrush) bmpBrush->Release();
    if (snapshotBmp) snapshotBmp->Release();
    if (rt) rt->Release();

    SelectObject(hMemDC, hOldBmp);
    DeleteObject(hTargetBmp);
    DeleteObject(data->hBitmap);
    DeleteDC(hMemDC);
    ReleaseDC(NULL, hScreenDC);
    
    // Cross-thread teardown
    ShowWindow(hGhost, SW_HIDE);
    PostMessage(hGhost, WM_CLOSE, 0, 0);
    delete data;
    
    return 0;
}

GhostAnimData* PrepareGenieAnim(HWND hWnd, BOOL rising) {
    RECT winRect;
    GetWindowRect(hWnd, &winRect);
    
    RECT rect = winRect;
    RECT extRect;
    if (SUCCEEDED(DwmGetWindowAttribute(hWnd, DWMWA_EXTENDED_FRAME_BOUNDS, &extRect, sizeof(extRect)))) {
        rect = extRect;
    }

    if (rising) {
        std::lock_guard<std::mutex> lock(g_CacheMutex);
        if (g_RectCache.count(hWnd)) {
            rect = g_RectCache[hWnd];
        }
    } else {
        std::lock_guard<std::mutex> lock(g_CacheMutex);
        g_RectCache[hWnd] = rect;
    }

    int w = rect.right - rect.left;
    int h = rect.bottom - rect.top;

    int offsetX = rect.left - winRect.left;
    int offsetY = rect.top - winRect.top;
    
    int rawW = winRect.right - winRect.left;
    int rawH = winRect.bottom - winRect.top;

    if (w <= 0 || h <= 0) return nullptr;

    POINT pt;
    GetCursorPos(&pt);
    RECT workArea;
    SystemParametersInfoW(SPI_GETWORKAREA, 0, &workArea, 0);

    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int learnedTargetX = screenWidth / 2;

    if (!PtInRect(&workArea, pt)) {
        learnedTargetX = pt.x; 
        std::lock_guard<std::mutex> lock(g_CacheMutex);
        g_IconPositions[hWnd] = learnedTargetX;
    } else {
        std::lock_guard<std::mutex> lock(g_CacheMutex);
        if (g_IconPositions.count(hWnd)) {
            learnedTargetX = g_IconPositions[hWnd];
        }
    }

    GhostAnimData* data = new GhostAnimData();
    data->hRealWnd = hWnd;
    data->targetRect = rect;
    data->width = w;
    data->height = h;
    data->isRising = rising;
    data->targetDockX = learnedTargetX;
    data->originalExStyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);

    data->durationMs = g_durationMs.load(std::memory_order_relaxed);
    data->bounceEnabled = g_bounceEnabled.load(std::memory_order_relaxed);
    data->bounceStrength = g_bounceStrength.load(std::memory_order_relaxed);
    data->bounceDurationMs = g_bounceDurationMs.load(std::memory_order_relaxed);

    HDC hScreenDC = GetDC(NULL);
    
    BITMAPINFO bmi = {{0}};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = w;
    bmi.bmiHeader.biHeight = -h; 
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    data->hBitmap = CreateDIBSection(hScreenDC, &bmi, DIB_RGB_COLORS, &(data->pBits), NULL, 0);

    auto CopyAndFixAlpha = [&](void* srcBits, void* dstBits) {
        DWORD* src = (DWORD*)srcBits;
        DWORD* dst = (DWORD*)dstBits;
        for (int y = 0; y < h; y++) {
            for (int x = 0; x < w; x++) {
                int sx = x + offsetX;
                int sy = y + offsetY;
                
                if (sx >= 0 && sx < rawW && sy >= 0 && sy < rawH) {
                    DWORD p = src[sy * rawW + sx];
                    BYTE a = (p >> 24) & 0xFF;
                    
                    if (a == 0) {
                        dst[y * w + x] = 0;
                    } else if (a == 255) {
                        dst[y * w + x] = p;
                    } else {
                        BYTE r = (p >> 16) & 0xFF;
                        BYTE g = (p >> 8) & 0xFF;
                        BYTE b = p & 0xFF;
                        dst[y * w + x] = (a << 24) | (((r * a) / 255) << 16) | (((g * a) / 255) << 8) | ((b * a) / 255);
                    }
                } else {
                    dst[y * w + x] = 0;
                }
            }
        }
    };

    if (rising) {
        BOOL fromCache = FALSE;
        {
            std::lock_guard<std::mutex> lock(g_CacheMutex);
            if (g_SnapshotCache.count(hWnd)) {
                void* pCacheBits = g_SnapshotCache[hWnd].second;
                memcpy(data->pBits, pCacheBits, w * h * 4);

                DeleteObject(g_SnapshotCache[hWnd].first);
                g_SnapshotCache.erase(hWnd);
                fromCache = TRUE;
            }
        }
        if (!fromCache) {
            HDC hTempDC = CreateCompatibleDC(hScreenDC);
            BITMAPINFO bmiTemp = bmi;
            bmiTemp.bmiHeader.biWidth = rawW;
            bmiTemp.bmiHeader.biHeight = -rawH;
            
            void* pTempBits = nullptr;
            HBITMAP hTempBmp = CreateDIBSection(hScreenDC, &bmiTemp, DIB_RGB_COLORS, &pTempBits, NULL, 0);
            HBITMAP hOldTempBmp = (HBITMAP)SelectObject(hTempDC, hTempBmp);
            
            PrintWindow(hWnd, hTempDC, PW_CLIENTONLY | 0x00000002);
            GdiFlush();
            
            CopyAndFixAlpha(pTempBits, data->pBits);
            
            SelectObject(hTempDC, hOldTempBmp);
            DeleteObject(hTempBmp);
            DeleteDC(hTempDC);
        }
    } else {
        HDC hTempDC = CreateCompatibleDC(hScreenDC);
        BITMAPINFO bmiTemp = bmi;
        bmiTemp.bmiHeader.biWidth = rawW;
        bmiTemp.bmiHeader.biHeight = -rawH;
        
        void* pTempBits = nullptr;
        HBITMAP hTempBmp = CreateDIBSection(hScreenDC, &bmiTemp, DIB_RGB_COLORS, &pTempBits, NULL, 0);
        HBITMAP hOldTempBmp = (HBITMAP)SelectObject(hTempDC, hTempBmp);

        PrintWindow(hWnd, hTempDC, PW_RENDERFULLCONTENT);
        GdiFlush();

        CopyAndFixAlpha(pTempBits, data->pBits);

        SelectObject(hTempDC, hOldTempBmp);
        DeleteObject(hTempBmp);
        DeleteDC(hTempDC);

        std::lock_guard<std::mutex> lock(g_CacheMutex);
        if (g_SnapshotCache.count(hWnd)) {
            DeleteObject(g_SnapshotCache[hWnd].first);
        }
        
        void* pCacheBits = nullptr;
        HBITMAP hCacheBmp = CreateDIBSection(hScreenDC, &bmi, DIB_RGB_COLORS, &pCacheBits, NULL, 0);
        
        memcpy(pCacheBits, data->pBits, w * h * 4);
        
        g_SnapshotCache[hWnd] = { hCacheBmp, pCacheBits };
    }

    ReleaseDC(NULL, hScreenDC);
    
    int vLeft = GetSystemMetrics(SM_XVIRTUALSCREEN);
    int vTop = GetSystemMetrics(SM_YVIRTUALSCREEN);
    int vWidth = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    int vHeight = GetSystemMetrics(SM_CYVIRTUALSCREEN);

    data->hGhost = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_TRANSPARENT,
        L"STATIC", NULL, WS_POPUP,
        vLeft, vTop, vWidth, vHeight,
        NULL, NULL, NULL, NULL
    );

    return data;
}

// -------------------------------------------------------------------------
// Wobbly Windows Engine
// -------------------------------------------------------------------------
static void CaptureWindowForWobbly(HWND hwnd) {
    RECT rcWin, rcExt;
    GetWindowRect(hwnd, &rcWin);
    if (DwmGetWindowAttribute(hwnd, DWMWA_EXTENDED_FRAME_BOUNDS, &rcExt, sizeof(rcExt)) != S_OK) {
        rcExt = rcWin;
    }

    int rawW = rcWin.right - rcWin.left;
    int rawH = rcWin.bottom - rcWin.top;
    g_capX = rcExt.left - rcWin.left;
    g_capY = rcExt.top - rcWin.top;
    g_capW = rcExt.right - rcExt.left;
    g_capH = rcExt.bottom - rcExt.top;

    if (rawW <= 0 || rawH <= 0 || g_capW <= 0 || g_capH <= 0) return;

    HDC hdcScreen = GetDC(NULL);
    HDC hdcMem = CreateCompatibleDC(hdcScreen);
    BITMAPINFO bmi = {{sizeof(BITMAPINFOHEADER), rawW, -rawH, 1, 32, BI_RGB}};
    void* rawBits = nullptr;
    HBITMAP hbmRaw = CreateDIBSection(hdcMem, &bmi, DIB_RGB_COLORS, &rawBits, NULL, 0);
    HBITMAP hOldBmp = (HBITMAP)SelectObject(hdcMem, hbmRaw);
    
    PrintWindow(hwnd, hdcMem, PW_RENDERFULLCONTENT);

    // Pre-multiply alpha for D2D (fixes Win11 rounded corners)
    BYTE* pRaw = (BYTE*)rawBits;
    for (int i = 0; i < rawW * rawH; i++) {
        BYTE a = pRaw[i * 4 + 3];
        if (a == 255) continue;
        if (a == 0) {
            pRaw[i * 4 + 0] = 0;
            pRaw[i * 4 + 1] = 0;
            pRaw[i * 4 + 2] = 0;
        } else {
            pRaw[i * 4 + 0] = (pRaw[i * 4 + 0] * a) / 255;
            pRaw[i * 4 + 1] = (pRaw[i * 4 + 1] * a) / 255;
            pRaw[i * 4 + 2] = (pRaw[i * 4 + 2] * a) / 255;
        }
    }

    if (!g_wobblyMemDC) {
        g_wobblyMemDC = CreateCompatibleDC(hdcScreen);
        BITMAPINFO bmiScreen = {{sizeof(BITMAPINFOHEADER), g_screenW, -g_screenH, 1, 32, BI_RGB}};
        g_wobblyTargetBmp = CreateDIBSection(hdcScreen, &bmiScreen, DIB_RGB_COLORS, &g_wobblyTargetBits, NULL, 0);
        SelectObject(g_wobblyMemDC, g_wobblyTargetBmp);
    }

    if (!g_wobblyRT && g_d2dFactory) {
        D2D1_RENDER_TARGET_PROPERTIES rtProps = D2D1::RenderTargetProperties(
            D2D1_RENDER_TARGET_TYPE_SOFTWARE,
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
            0, 0, D2D1_RENDER_TARGET_USAGE_GDI_COMPATIBLE, D2D1_FEATURE_LEVEL_DEFAULT
        );
        g_d2dFactory->CreateDCRenderTarget(&rtProps, &g_wobblyRT);
    }

    if (g_wobblyRT) {
        if (g_wobblyBrush) { g_wobblyBrush->Release(); g_wobblyBrush = nullptr; }
        ID2D1Bitmap* pRawBmp = nullptr;
        D2D1_BITMAP_PROPERTIES bmpProps = D2D1::BitmapProperties(
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)
        );
        g_wobblyRT->CreateBitmap(D2D1::SizeU(rawW, rawH), rawBits, rawW * 4, bmpProps, &pRawBmp);
        
        if (pRawBmp) {
            D2D1_BITMAP_BRUSH_PROPERTIES brushProps = D2D1::BitmapBrushProperties(
                D2D1_EXTEND_MODE_CLAMP, D2D1_EXTEND_MODE_CLAMP, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR
            );
            g_wobblyRT->CreateBitmapBrush(pRawBmp, &brushProps, nullptr, &g_wobblyBrush);
            pRawBmp->Release();
        }
    }

    SelectObject(hdcMem, hOldBmp);
    DeleteObject(hbmRaw);
    DeleteDC(hdcMem);
    ReleaseDC(NULL, hdcScreen);
}

static void UpdateWobblyOrigin(float x, float y, float w, float h) {
    g_currentRect.x = x; g_currentRect.y = y; g_currentRect.width = w; g_currentRect.height = h;
    float x_length = w / 3.0f, y_length = h / 3.0f;
    Pair origine = { x, y };

    for (int j = 0; j < 4; ++j) {
        for (int i = 0; i < 4; ++i) {
            g_wwi.origin[j * 4 + i] = origine;
            if (i != 2) origine.x += x_length;
            else origine.x = w + x;
        }
        origine.x = x;
        if (j != 2) origine.y += y_length;
        else origine.y = h + y;
    }
}

static void InitWobblyInfo(float x, float y, float w, float h) {
    memset(&g_wwi, 0, sizeof(WobblyInfos));
    UpdateWobblyOrigin(x, y, w, h);
    for (int i = 0; i < 16; ++i) g_wwi.position[i] = g_wwi.origin[i];
    g_wwi.can_wobble_top = true; g_wwi.can_wobble_bottom = true;
    g_wwi.can_wobble_left = true; g_wwi.can_wobble_right = true;
    g_wwi.wobblying = true;
}

static void HeightRingLinearMean(Pair* data, Pair* buffer) {
    for (int i = 0; i < 16; ++i) {
        int x = i % 4, y = i / 4, weight = 0;
        float sum_x = 0, sum_y = 0;
        for (int dy = -1; dy <= 1; ++dy) {
            for (int dx = -1; dx <= 1; ++dx) {
                int nx = x + dx, ny = y + dy;
                if (nx >= 0 && nx <= 3 && ny >= 0 && ny <= 3) {
                    if (dx == 0 && dy == 0) continue;
                    sum_x += data[ny * 4 + nx].x; sum_y += data[ny * 4 + nx].y; weight++;
                }
            }
        }
        sum_x += data[i].x * weight; sum_y += data[i].y * weight;
        buffer[i].x = sum_x / (weight * 2.0f); buffer[i].y = sum_y / (weight * 2.0f);
    }
    for (int i = 0; i < 16; ++i) data[i] = buffer[i];
}

static void StepPhysics(float time) {
    float x_length = g_currentRect.width / 3.0f, y_length = g_currentRect.height / 3.0f;
    float acc_sum = 0.0f, vel_sum = 0.0f;

    for (int i = 0; i < 16; ++i) {
        if (g_wwi.constraint[i]) {
            g_wwi.acceleration[i].x = (g_wwi.origin[i].x - g_wwi.position[i].x) * g_params.stiffness;
            g_wwi.acceleration[i].y = (g_wwi.origin[i].y - g_wwi.position[i].y) * g_params.stiffness;
        } else {
            float ax = 0, ay = 0; int count = 0; int x = i % 4, y = i / 4;
            if (x < 3) { ax += (g_wwi.position[i + 1].x - g_wwi.position[i].x - x_length) * g_params.stiffness; ay += (g_wwi.position[i + 1].y - g_wwi.position[i].y) * g_params.stiffness; count++; }
            if (x > 0) { ax += (g_wwi.position[i - 1].x - g_wwi.position[i].x + x_length) * g_params.stiffness; ay += (g_wwi.position[i - 1].y - g_wwi.position[i].y) * g_params.stiffness; count++; }
            if (y < 3) { ax += (g_wwi.position[i + 4].x - g_wwi.position[i].x) * g_params.stiffness; ay += (g_wwi.position[i + 4].y - g_wwi.position[i].y - y_length) * g_params.stiffness; count++; }
            if (y > 0) { ax += (g_wwi.position[i - 4].x - g_wwi.position[i].x) * g_params.stiffness; ay += (g_wwi.position[i - 4].y - g_wwi.position[i].y + y_length) * g_params.stiffness; count++; }
            g_wwi.acceleration[i].x = ax / count; g_wwi.acceleration[i].y = ay / count;
        }
    }

    HeightRingLinearMean(g_wwi.acceleration, g_wwi.buffer);

    for (int i = 0; i < 16; ++i) {
        Pair acc = g_wwi.acceleration[i];
        if (fabsf(acc.x) < g_params.minAcc) acc.x = 0; if (fabsf(acc.y) < g_params.minAcc) acc.y = 0;
        if (fabsf(acc.x) > g_params.maxAcc) acc.x = acc.x > 0 ? g_params.maxAcc : -g_params.maxAcc;
        if (fabsf(acc.y) > g_params.maxAcc) acc.y = acc.y > 0 ? g_params.maxAcc : -g_params.maxAcc;

        g_wwi.velocity[i].x = acc.x * time + g_wwi.velocity[i].x * g_params.drag;
        g_wwi.velocity[i].y = acc.y * time + g_wwi.velocity[i].y * g_params.drag;
        acc_sum += fabsf(acc.x) + fabsf(acc.y);
    }

    HeightRingLinearMean(g_wwi.velocity, g_wwi.buffer);

    for (int i = 0; i < 16; ++i) {
        Pair vel = g_wwi.velocity[i];
        if (fabsf(vel.x) < g_params.minVelocity) vel.x = 0; if (fabsf(vel.y) < g_params.minVelocity) vel.y = 0;
        if (fabsf(vel.x) > g_params.maxVelocity) vel.x = vel.x > 0 ? g_params.maxVelocity : -g_params.maxVelocity;
        if (fabsf(vel.y) > g_params.maxVelocity) vel.y = vel.y > 0 ? g_params.maxVelocity : -g_params.maxVelocity;

        g_wwi.position[i].x += vel.x * time * g_params.moveFactor;
        g_wwi.position[i].y += vel.y * time * g_params.moveFactor;
        vel_sum += fabsf(vel.x) + fabsf(vel.y);
    }
    
    if (!g_wwi.can_wobble_top) { for (int i = 0; i < 4; ++i) for (int j = 0; j < 3; ++j) g_wwi.position[i + 4 * j].y = g_wwi.origin[i + 4 * j].y; }
    if (!g_wwi.can_wobble_bottom) { for (int i = 12; i < 16; ++i) for (int j = 0; j < 3; ++j) g_wwi.position[i - 4 * j].y = g_wwi.origin[i - 4 * j].y; }
    if (!g_wwi.can_wobble_left) { for (int i = 0; i < 16; i += 4) for (int j = 0; j < 3; ++j) g_wwi.position[i + j].x = g_wwi.origin[i + j].x; }
    if (!g_wwi.can_wobble_right) { for (int i = 3; i < 16; i += 4) for (int j = 0; j < 3; ++j) g_wwi.position[i - j].x = g_wwi.origin[i - j].x; }
    
    g_wwi.wobblying = !(acc_sum < g_params.stopAcc && vel_sum < g_params.stopVelocity);
}

static Pair ComputeBezierPoint(float tx, float ty) {
    float px[4] = { (1 - tx)*(1 - tx)*(1 - tx), 3*(1 - tx)*(1 - tx)*tx, 3*(1 - tx)*tx*tx, tx*tx*tx };
    float py[4] = { (1 - ty)*(1 - ty)*(1 - ty), 3*(1 - ty)*(1 - ty)*ty, 3*(1 - ty)*ty*ty, ty*ty*ty };
    Pair res = { 0.0f, 0.0f };
    for (int j = 0; j < 4; ++j) {
        for (int i = 0; i < 4; ++i) {
            res.x += px[i] * py[j] * g_wwi.position[i + j * 4].x;
            res.y += px[i] * py[j] * g_wwi.position[i + j * 4].y;
        }
    }
    return res;
}

static void DrawOverlayFrameD2D() {
    if (!g_wobblyRT || !g_wobblyBrush || !g_wobblyMemDC) return;

    RECT bindRect = { 0, 0, g_screenW, g_screenH };
    g_wobblyRT->BindDC(g_wobblyMemDC, &bindRect);
    g_wobblyRT->BeginDraw();
    g_wobblyRT->Clear(D2D1::ColorF(0, 0, 0, 0));
    g_wobblyRT->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

    float vLeft = (float)g_screenX;
    float vTop = (float)g_screenY;

    ID2D1PathGeometry* outlineGeo = nullptr;
    g_d2dFactory->CreatePathGeometry(&outlineGeo);
    if (outlineGeo) {
        ID2D1GeometrySink* sink = nullptr;
        outlineGeo->Open(&sink);
        Pair startP = ComputeBezierPoint(0.0f, 0.0f);
        sink->BeginFigure(D2D1::Point2F(startP.x - vLeft, startP.y - vTop), D2D1_FIGURE_BEGIN_FILLED);
        for (int x = 1; x <= X_TILES; x++) {
            Pair p = ComputeBezierPoint((float)x / X_TILES, 0.0f);
            sink->AddLine(D2D1::Point2F(p.x - vLeft, p.y - vTop));
        }
        for (int y = 1; y <= Y_TILES; y++) {
            Pair p = ComputeBezierPoint(1.0f, (float)y / Y_TILES);
            sink->AddLine(D2D1::Point2F(p.x - vLeft, p.y - vTop));
        }
        for (int x = X_TILES - 1; x >= 0; x--) {
            Pair p = ComputeBezierPoint((float)x / X_TILES, 1.0f);
            sink->AddLine(D2D1::Point2F(p.x - vLeft, p.y - vTop));
        }
        for (int y = Y_TILES - 1; y >= 0; y--) {
            Pair p = ComputeBezierPoint(0.0f, (float)y / Y_TILES);
            sink->AddLine(D2D1::Point2F(p.x - vLeft, p.y - vTop));
        }
        sink->EndFigure(D2D1_FIGURE_END_CLOSED);
        sink->Close();

        ID2D1Layer* layer = nullptr;
        g_wobblyRT->CreateLayer(&layer);
        D2D1_LAYER_PARAMETERS layerParams = D2D1::LayerParameters();
        layerParams.geometricMask = outlineGeo;
        layerParams.maskAntialiasMode = D2D1_ANTIALIAS_MODE_PER_PRIMITIVE;
        g_wobblyRT->PushLayer(&layerParams, layer);

        for (int y = 0; y < Y_TILES; y++) {
            for (int x = 0; x < X_TILES; x++) {
                float tx1 = (float)x / X_TILES, ty1 = (float)y / Y_TILES;
                float tx2 = (float)(x + 1) / X_TILES, ty2 = (float)(y + 1) / Y_TILES;
                
                Pair bp1 = ComputeBezierPoint(tx1, ty1);
                Pair bp2 = ComputeBezierPoint(tx2, ty1);
                Pair bp3 = ComputeBezierPoint(tx1, ty2);
                Pair bp4 = ComputeBezierPoint(tx2, ty2);

                D2D1_POINT_2F p1 = D2D1::Point2F(bp1.x - vLeft, bp1.y - vTop);
                D2D1_POINT_2F p2 = D2D1::Point2F(bp2.x - vLeft, bp2.y - vTop);
                D2D1_POINT_2F p3 = D2D1::Point2F(bp3.x - vLeft, bp3.y - vTop);
                D2D1_POINT_2F p4 = D2D1::Point2F(bp4.x - vLeft, bp4.y - vTop);

                D2D1_POINT_2F c = D2D1::Point2F((p1.x+p2.x+p3.x+p4.x)/4.0f, (p1.y+p2.y+p3.y+p4.y)/4.0f);
                ID2D1PathGeometry* quadGeo = CreateQuadGeo(g_d2dFactory, 
                    BloatPoint(p1, c), BloatPoint(p2, c), BloatPoint(p4, c), BloatPoint(p3, c));

                if (quadGeo) {
                    float sx = (float)g_capX + tx1 * g_capW;
                    float sy = (float)g_capY + ty1 * g_capH;
                    float sw = (float)g_capW / X_TILES;
                    float sh = (float)g_capH / Y_TILES;

                    float m11 = (p2.x - p1.x) / sw;
                    float m12 = (p2.y - p1.y) / sw;
                    float m21 = (p3.x - p1.x) / sh;
                    float m22 = (p3.y - p1.y) / sh;
                    float m31 = p1.x - sx * m11 - sy * m21;
                    float m32 = p1.y - sx * m12 - sy * m22;

                    g_wobblyBrush->SetTransform(D2D1::Matrix3x2F(m11, m12, m21, m22, m31, m32));
                    g_wobblyRT->FillGeometry(quadGeo, g_wobblyBrush);
                    quadGeo->Release();
                }
            }
        }
        g_wobblyRT->PopLayer();
        if (layer) layer->Release();
        outlineGeo->Release();
    }
    g_wobblyRT->EndDraw();

    HDC hdcScreen = GetDC(NULL); 
    POINT ptS = {0, 0};
    POINT ptW = {g_screenX, g_screenY}; 
    SIZE sz = {g_screenW, g_screenH}; 
    BLENDFUNCTION bl = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    
    UpdateLayeredWindow(g_overlayHwnd, hdcScreen, &ptW, &sz, g_wobblyMemDC, &ptS, 0, &bl, ULW_ALPHA);
    ReleaseDC(NULL, hdcScreen);
}

static void CleanupWobblyD2D() {
    if (g_wobblyBrush) { g_wobblyBrush->Release(); g_wobblyBrush = nullptr; }
}

static LRESULT CALLBACK OverlayProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    if (msg == WM_TIMER) {
        if (g_isMoving || g_isSettling) {
            if (!(GetWindowLongPtrW(g_mainHwnd, GWL_EXSTYLE) & WS_EX_LAYERED)) {
                SetWindowLongPtrW(g_mainHwnd, GWL_EXSTYLE, g_oldExStyle | WS_EX_LAYERED);
                SetLayeredWindowAttributes(g_mainHwnd, 0, 1, LWA_ALPHA);
            }
            DWORD now = GetTickCount();
            DWORD delta = now - g_lastTick;
            g_lastTick = now;

            if (delta > 64) delta = 64;
            while (delta > 0) { 
                DWORD dt = delta > 10 ? 10 : delta; 
                StepPhysics((float)dt); 
                delta -= dt; 
            }
            
            DrawOverlayFrameD2D();
            
            if (g_isSettling && !g_wwi.wobblying) {
                g_isSettling = false; 
                KillTimer(hwnd, 1); 
                ShowWindow(hwnd, SW_HIDE);
                
                SetLayeredWindowAttributes(g_mainHwnd, 0, 255, LWA_ALPHA);
                if (!(g_oldExStyle & WS_EX_LAYERED)) {
                    SetWindowLongPtrW(g_mainHwnd, GWL_EXSTYLE, g_oldExStyle);
                }
                CleanupWobblyD2D();
            }
        }
        return 0;
    }
    return DefWindowProc(hwnd, msg, wp, lp);
}

static void InitializeWobbly(void) {
    if (g_overlayHwnd) return;
    WNDCLASSA oc = {0}; 
    oc.lpfnWndProc = OverlayProc; 
    oc.hInstance = GetModuleHandle(NULL); 
    oc.lpszClassName = "WobblyOverlayWindow";
    RegisterClassA(&oc);
    
    g_screenX = GetSystemMetrics(SM_XVIRTUALSCREEN); 
    g_screenY = GetSystemMetrics(SM_YVIRTUALSCREEN);
    g_screenW = GetSystemMetrics(SM_CXVIRTUALSCREEN); 
    g_screenH = GetSystemMetrics(SM_CYVIRTUALSCREEN);
    
    g_overlayHwnd = CreateWindowExA(
        WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW, 
        "WobblyOverlayWindow", NULL, WS_POPUP, 
        g_screenX, g_screenY, g_screenW, g_screenH, 
        NULL, NULL, oc.hInstance, NULL
    );
}

static void OnEnterSizeMove(HWND hwnd) {
    if (!g_wobblyEnabled.load(std::memory_order_relaxed)) return;
    if (IsZoomed(hwnd) || IsIconic(hwnd)) return;
    
    if (!g_engineInitialized) {
        InitializeWobbly();
        g_engineInitialized = true;
    }

    g_mainHwnd = hwnd;

    if (!g_isSettling && !g_isMoving) {
        CaptureWindowForWobbly(hwnd);
        
        g_oldExStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
        SetWindowLongPtrW(hwnd, GWL_EXSTYLE, g_oldExStyle | WS_EX_LAYERED); 
        SetLayeredWindowAttributes(hwnd, 0, 1, LWA_ALPHA);
        
        RECT rcExt; 
        if (DwmGetWindowAttribute(hwnd, DWMWA_EXTENDED_FRAME_BOUNDS, &rcExt, sizeof(rcExt)) != S_OK) GetWindowRect(hwnd, &rcExt);
        InitWobblyInfo((float)rcExt.left, (float)rcExt.top, (float)(rcExt.right - rcExt.left), (float)(rcExt.bottom - rcExt.top));
    } else {
        RECT rcExt; 
        if (DwmGetWindowAttribute(hwnd, DWMWA_EXTENDED_FRAME_BOUNDS, &rcExt, sizeof(rcExt)) != S_OK) GetWindowRect(hwnd, &rcExt);
        UpdateWobblyOrigin((float)rcExt.left, (float)rcExt.top, (float)(rcExt.right - rcExt.left), (float)(rcExt.bottom - rcExt.top));
        for (int i = 0; i < 16; ++i) g_wwi.constraint[i] = false;
    }
    
    RECT rcExt; 
    if (DwmGetWindowAttribute(hwnd, DWMWA_EXTENDED_FRAME_BOUNDS, &rcExt, sizeof(rcExt)) != S_OK) GetWindowRect(hwnd, &rcExt);
    POINT pt; 
    GetCursorPos(&pt);
    
    float x_inc = (rcExt.right - rcExt.left) / 3.0f;
    float y_inc = (rcExt.bottom - rcExt.top) / 3.0f;
    int indx = (int)((pt.x - rcExt.left) / x_inc + 0.5f);
    int indy = (int)((pt.y - rcExt.top) / y_inc + 0.5f);
    
    if (indx < 0) indx = 0; if (indx > 3) indx = 3; 
    if (indy < 0) indy = 0; if (indy > 3) indy = 3;
    g_wwi.constraint[indy * 4 + indx] = true;
    
    g_isMoving = true; 
    g_isSettling = false; 
    g_lastTick = GetTickCount();
    
    ShowWindow(g_overlayHwnd, SW_SHOWNOACTIVATE); 
    SetTimer(g_overlayHwnd, 1, 16, NULL);
}

static void OnSizingMoving(HWND hwnd, LPRECT r) {
    if (g_isMoving) {
        RECT rExt, rWin; 
        if (DwmGetWindowAttribute(hwnd, DWMWA_EXTENDED_FRAME_BOUNDS, &rExt, sizeof(rExt)) != S_OK) GetWindowRect(hwnd, &rExt);
        GetWindowRect(hwnd, &rWin);
        
        int dx = rExt.left - rWin.left;
        int dy = rExt.top - rWin.top;
        float w = (float)(r->right - r->left) - (rWin.right - rExt.right) - dx;
        float h = (float)(r->bottom - r->top) - (rWin.bottom - rExt.bottom) - dy;
        
        UpdateWobblyOrigin((float)r->left + dx, (float)r->top + dy, w, h);
    }
}

static void OnWindowPosChanged(HWND hwnd, WINDOWPOS* wp) {
    if (hwnd != g_mainHwnd || g_isMoving) return;

    if (g_isSettling && wp) {
        if (wp->flags & SWP_NOMOVE && wp->flags & SWP_NOSIZE) return;
        
        RECT rExt, rWin; 
        if (DwmGetWindowAttribute(hwnd, DWMWA_EXTENDED_FRAME_BOUNDS, &rExt, sizeof(rExt)) == S_OK && GetWindowRect(hwnd, &rWin)) {
            int dx = rExt.left - rWin.left;
            int dy = rExt.top - rWin.top;
            float w = (float)wp->cx - (rWin.right - rExt.right) - dx;
            float h = (float)wp->cy - (rWin.bottom - rExt.bottom) - dy;
            
            UpdateWobblyOrigin((float)wp->x + dx, (float)wp->y + dy, w, h);
        }
    }
}

static void OnExitSizeMove(HWND hwnd) {
    if (g_isMoving) {
        g_isMoving = false; 
        g_isSettling = true;
        
        RECT rcExt; 
        if (DwmGetWindowAttribute(hwnd, DWMWA_EXTENDED_FRAME_BOUNDS, &rcExt, sizeof(rcExt)) != S_OK) GetWindowRect(hwnd, &rcExt);
        UpdateWobblyOrigin((float)rcExt.left, (float)rcExt.top, (float)(rcExt.right - rcExt.left), (float)(rcExt.bottom - rcExt.top));
    }
}

// -------------------------------------------------------------------------
// Hooks & Subclassing
// -------------------------------------------------------------------------
LRESULT CALLBACK WobblySubclassProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
    if (msg == WM_ENTERSIZEMOVE) {
        OnEnterSizeMove(hwnd);
    }
    else if (msg == WM_SIZING || msg == WM_MOVING) {
        OnSizingMoving(hwnd, (LPRECT)lParam);
    }
    else if (msg == WM_WINDOWPOSCHANGED) {
        OnWindowPosChanged(hwnd, (WINDOWPOS*)lParam);
    }
    else if (msg == WM_EXITSIZEMOVE) {
        OnExitSizeMove(hwnd);
    }
    else if (msg == WM_NCDESTROY) {
        RemoveWindowSubclass(hwnd, WobblySubclassProc, uIdSubclass);
    }
    return DefSubclassProc(hwnd, msg, wParam, lParam);
}

HWND WINAPI CreateWindowExW_Hook(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam) {
    HWND hwnd = CreateWindowExW_Original(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
    if (hwnd && (dwStyle & WS_CHILD) == 0) SetWindowSubclass(hwnd, WobblySubclassProc, 0, 0);
    return hwnd;
}

HWND WINAPI CreateWindowExA_Hook(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam) {
    HWND hwnd = CreateWindowExA_Original(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
    if (hwnd && (dwStyle & WS_CHILD) == 0) SetWindowSubclass(hwnd, WobblySubclassProc, 0, 0);
    return hwnd;
}

static LRESULT CALLBACK CrossThreadHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        CWPSTRUCT* pCwp = (CWPSTRUCT*)lParam;
        if (pCwp->message == g_wmAttach) SetWindowSubclass(pCwp->hwnd, WobblySubclassProc, 0, 0);
        else if (pCwp->message == g_wmDetach) RemoveWindowSubclass(pCwp->hwnd, WobblySubclassProc, 0);
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

static BOOL CALLBACK AttachSubclassEnumProc(HWND hwnd, LPARAM lParam) {
    LONG_PTR style = GetWindowLongPtrW(hwnd, GWL_STYLE);
    if ((style & WS_CHILD) == 0) {
        DWORD tid = GetWindowThreadProcessId(hwnd, NULL);
        HHOOK hHook = SetWindowsHookEx(WH_CALLWNDPROC, CrossThreadHookProc, NULL, tid);
        if (hHook) {
            SendMessageTimeoutW(hwnd, g_wmAttach, 0, 0, SMTO_ABORTIFHUNG | SMTO_NORMAL, 500, NULL);
            UnhookWindowsHookEx(hHook);
        }
    }
    return TRUE;
}

static BOOL CALLBACK DetachSubclassEnumProc(HWND hwnd, LPARAM lParam) {
    LONG_PTR style = GetWindowLongPtrW(hwnd, GWL_STYLE);
    if ((style & WS_CHILD) == 0) {
        DWORD tid = GetWindowThreadProcessId(hwnd, NULL);
        HHOOK hHook = SetWindowsHookEx(WH_CALLWNDPROC, CrossThreadHookProc, NULL, tid);
        if (hHook) {
            SendMessageTimeoutW(hwnd, g_wmDetach, 0, 0, SMTO_ABORTIFHUNG | SMTO_NORMAL, 500, NULL);
            UnhookWindowsHookEx(hHook);
        }
    }
    return TRUE;
}

void EnumerateAndSubclass(BOOL attach) {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (hSnapshot != INVALID_HANDLE_VALUE) {
        THREADENTRY32 te;
        te.dwSize = sizeof(THREADENTRY32);
        if (Thread32First(hSnapshot, &te)) {
            DWORD pid = GetCurrentProcessId();
            do {
                if (te.th32OwnerProcessID == pid) {
                    EnumThreadWindows(te.th32ThreadID, attach ? AttachSubclassEnumProc : DetachSubclassEnumProc, 0);
                }
            } while (Thread32Next(hSnapshot, &te));
        }
        CloseHandle(hSnapshot);
    }
}

BOOL WINAPI ShowWindow_Hook(HWND hWnd, int nCmdShow) {
    if (GetAncestor(hWnd, GA_ROOT) != hWnd) {
        return ShowWindow_Original(hWnd, nCmdShow);
    }

    if (nCmdShow == SW_MINIMIZE || nCmdShow == SW_SHOWMINIMIZED || nCmdShow == SW_SHOWMINNOACTIVE) {
        if (g_d2dFactory) {
            SetDwmTransitions(hWnd, FALSE);
            GhostAnimData* data = PrepareGenieAnim(hWnd, FALSE);
            if (data) {
                ShowGhostSync(data);
                
                LONG_PTR exStyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
                SetWindowLongPtrW(hWnd, GWL_EXSTYLE, exStyle | WS_EX_LAYERED);
                SetLayeredWindowAttributes(hWnd, 0, 0, LWA_ALPHA);
                
                CreateThread(NULL, 0, GhostAnimationThread, data, 0, NULL);
            }
        }
        return ShowWindow_Original(hWnd, nCmdShow);
    }
    else if (nCmdShow == SW_RESTORE || nCmdShow == SW_SHOWNORMAL) {
        if (IsIconic(hWnd) && g_d2dFactory) {
            SetDwmTransitions(hWnd, FALSE);
            
            GhostAnimData* data = PrepareGenieAnim(hWnd, TRUE);
            
            LONG_PTR exStyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
            SetWindowLongPtrW(hWnd, GWL_EXSTYLE, exStyle | WS_EX_LAYERED);
            SetLayeredWindowAttributes(hWnd, 0, 0, LWA_ALPHA);
            
            BOOL res = ShowWindow_Original(hWnd, nCmdShow);
            
            if (data) CreateThread(NULL, 0, GhostAnimationThread, data, 0, NULL);
            return res;
        }
    }
    return ShowWindow_Original(hWnd, nCmdShow);
}

LRESULT WINAPI DefWindowProcW_Hook(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
    if (Msg == WM_DESTROY) {
        std::lock_guard<std::mutex> lock(g_CacheMutex);
        if (g_SnapshotCache.count(hWnd)) {
            DeleteObject(g_SnapshotCache[hWnd].first);
            g_SnapshotCache.erase(hWnd);
        }
        if (g_IconPositions.count(hWnd)) {
            g_IconPositions.erase(hWnd);
        }
        if (g_RectCache.count(hWnd)) {
            g_RectCache.erase(hWnd);
        }
    }

    if (Msg == WM_SYSCOMMAND) {
        UINT cmd = wParam & 0xFFF0;
        if (cmd == SC_MINIMIZE) {
            if (GetAncestor(hWnd, GA_ROOT) != hWnd) {
                return DefWindowProcW_Original(hWnd, Msg, wParam, lParam);
            }
            if (g_d2dFactory) {
                SetDwmTransitions(hWnd, FALSE);
                GhostAnimData* data = PrepareGenieAnim(hWnd, FALSE);
                if (data) {
                    ShowGhostSync(data);
                    
                    LONG_PTR exStyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
                    SetWindowLongPtrW(hWnd, GWL_EXSTYLE, exStyle | WS_EX_LAYERED);
                    SetLayeredWindowAttributes(hWnd, 0, 0, LWA_ALPHA);
                    
                    CreateThread(NULL, 0, GhostAnimationThread, data, 0, NULL);
                }
            }
            return DefWindowProcW_Original(hWnd, Msg, wParam, lParam);
        }
        else if (cmd == SC_RESTORE && IsIconic(hWnd)) {
            if (GetAncestor(hWnd, GA_ROOT) != hWnd) {
                return DefWindowProcW_Original(hWnd, Msg, wParam, lParam);
            }
            if (g_d2dFactory) {
                SetDwmTransitions(hWnd, FALSE);
                
                GhostAnimData* data = PrepareGenieAnim(hWnd, TRUE);
                
                LONG_PTR exStyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
                SetWindowLongPtrW(hWnd, GWL_EXSTYLE, exStyle | WS_EX_LAYERED);
                SetLayeredWindowAttributes(hWnd, 0, 0, LWA_ALPHA);
                
                LRESULT res = DefWindowProcW_Original(hWnd, Msg, wParam, lParam);
                
                if (data) CreateThread(NULL, 0, GhostAnimationThread, data, 0, NULL);
                return res;
            }
        }
    }
    return DefWindowProcW_Original(hWnd, Msg, wParam, lParam);
}

// -------------------------------------------------------------------------
// Mod Lifecycle
// -------------------------------------------------------------------------
BOOL Wh_ModInit() {
    Wh_Log(L"Custom Animations & Wobbly Mod Loading...");
    LoadSettings();

    WCHAR exePath[MAX_PATH];
    if (GetModuleFileNameW(NULL, exePath, MAX_PATH)) {
        _wcslwr_s(exePath);
        if (wcsstr(exePath, L"dwm.exe") || wcsstr(exePath, L"csrss.exe") || 
            wcsstr(exePath, L"lsass.exe") || wcsstr(exePath, L"winlogon.exe")) {
            return FALSE;
        }
    }

    HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, __uuidof(ID2D1Factory), reinterpret_cast<void**>(&g_d2dFactory));
    if (FAILED(hr)) { g_d2dFactory = nullptr; }

    g_wmAttach = RegisterWindowMessageW(L"WobblyWindows_Attach");
    g_wmDetach = RegisterWindowMessageW(L"WobblyWindows_Detach");

    Wh_SetFunctionHook((void*)DefWindowProcW, (void*)DefWindowProcW_Hook, (void**)&DefWindowProcW_Original);
    Wh_SetFunctionHook((void*)ShowWindow, (void*)ShowWindow_Hook, (void**)&ShowWindow_Original);
    Wh_SetFunctionHook((void*)CreateWindowExW, (void*)CreateWindowExW_Hook, (void**)&CreateWindowExW_Original);
    Wh_SetFunctionHook((void*)CreateWindowExA, (void*)CreateWindowExA_Hook, (void**)&CreateWindowExA_Original);
    
    EnumerateAndSubclass(TRUE);

    return TRUE;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}

void Wh_ModUninit() {
    Wh_Log(L"Custom Animations & Wobbly Mod Unloading...");
    
    EnumerateAndSubclass(FALSE);

    if (g_engineInitialized) {
        CleanupWobblyD2D();
        if (g_wobblyRT) { g_wobblyRT->Release(); g_wobblyRT = nullptr; }
        if (g_wobblyMemDC) { DeleteDC(g_wobblyMemDC); g_wobblyMemDC = NULL; }
        if (g_wobblyTargetBmp) { DeleteObject(g_wobblyTargetBmp); g_wobblyTargetBmp = NULL; }
        if (g_overlayHwnd) DestroyWindow(g_overlayHwnd);
    }

    if (g_d2dFactory) {
        g_d2dFactory->Release();
        g_d2dFactory = nullptr;
    }
    
    std::lock_guard<std::mutex> lock(g_CacheMutex);
    for (auto& pair : g_SnapshotCache) {
        DeleteObject(pair.second.first);
    }
    g_SnapshotCache.clear();
    g_IconPositions.clear();
    g_RectCache.clear();
}
