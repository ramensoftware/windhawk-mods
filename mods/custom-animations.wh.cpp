// ==WindhawkMod==
// @id            custom-animations
// @name          Animation Mod
// @description   Ultra-smooth Open/Close/Minimize/Maximize animation with optional Bounce physics (Direct2D Accelerated)
// @version       1.2.0
// @author        Shoaib Hassan (D2D Port)
// @github        https://github.com/shoaibhassan2
// @include       *
// @compilerOptions -ldwmapi -lgdi32 -ld2d1
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Genie, Magic Lamp & Squash Animation Mod (Direct2D Edition)

Replaces the default Windows minimize and restore animations with smooth geometry deformation effects.
Now powered by Direct2D for massive performance gains and smooth anti-aliasing.

- KDE Magic Lamp: fluid 4-direction spatial bend
- macOS Genie: cosine-eased suction effect to dock
- MacSine: liquid wobble motion during animation
- Squash: scale and fade to/from taskbar icon (KDE-style squash effect)
- Optional elastic bounce physics on window restore
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
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <dwmapi.h>
#include <d2d1.h>
#include <math.h>
#include <atomic>
#include <unordered_map>
#include <mutex>
#include <vector>

#define PI 3.14159265f

typedef LRESULT (WINAPI *DefWindowProcW_t)(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
DefWindowProcW_t DefWindowProcW_Original;

typedef BOOL (WINAPI *ShowWindow_t)(HWND hWnd, int nCmdShow);
ShowWindow_t ShowWindow_Original;

struct Geometry { float x, y, width, height; };
enum IconPosition { POS_TOP, POS_BOTTOM, POS_LEFT, POS_RIGHT };

struct GhostAnimData {
    HWND hRealWnd;
    HBITMAP hBitmap;
    void* pBits; 
    RECT targetRect;
    int width;
    int height;
    int targetDockX; 
    BOOL isRising;
    LONG_PTR originalExStyle;
    
    // Captured config
    int durationMs;
    bool bounceEnabled;
    int bounceStrength;
    int bounceDurationMs;
};

// --- THE VAULTS ---
std::unordered_map<HWND, std::pair<HBITMAP, void*>> g_SnapshotCache;
std::unordered_map<HWND, int> g_IconPositions; 
std::mutex g_CacheMutex;

ID2D1Factory* g_d2dFactory = nullptr;

// --- SETTINGS ---
std::atomic<int> g_durationMs{450};
std::atomic<int> g_animationStyle{0}; // 0 = KDE, 1 = Mac Pinch, 2 = Mac Sine, 3 = Squash
std::atomic<bool> g_bounceEnabled{true};
std::atomic<int> g_bounceStrength{30};
std::atomic<int> g_bounceDurationMs{300};

void LoadSettings() {
    int ms = Wh_GetIntSetting(L"duration_ms");
    if (ms < 50) ms = 50;
    if (ms > 2000) ms = 2000;
    g_durationMs.store(ms, std::memory_order_relaxed);

    PCWSTR styleStr = Wh_GetStringSetting(L"animation_style");
    int style = 0; // Default to KDE
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
}

void SetDwmTransitions(HWND hWnd, BOOL enable) {
    BOOL disable = !enable;
    DwmSetWindowAttribute(hWnd, DWMWA_TRANSITIONS_FORCEDISABLED, &disable, sizeof(disable));
}

// -------------------------------------------------------------------------
// Direct2D Helpers
// -------------------------------------------------------------------------
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
// Easing functions for Squash animation
// -------------------------------------------------------------------------
static inline float CubicEaseIn(float t) { return t * t * t; }
static inline float CubicEaseOut(float t) { float t1 = t - 1.0f; return t1 * t1 * t1 + 1.0f; }

// -------------------------------------------------------------------------
// ENGINE 1: KDE Physics Logic
// -------------------------------------------------------------------------
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

// -------------------------------------------------------------------------
// ENGINE 2: macOS Genie Physics Logic
// -------------------------------------------------------------------------
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

// -------------------------------------------------------------------------
// Direct2D Render Thread
// -------------------------------------------------------------------------
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

    HWND hGhost = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_TRANSPARENT,
        L"STATIC", NULL, WS_POPUP,
        vLeft, vTop, vWidth, vHeight,
        NULL, NULL, NULL, NULL
    );

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

    // Direct2D Initialization
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
    BOOL firstFrame = TRUE;
    MSG msg;

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

        // --- Calculate Physics Bounce Matrix ---
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
                // ----- SQUASH ANIMATION (D2D Matrix Math) -----
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
                
                // Inject Bounce Translation and Scaling
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
                // ----- TILE-BASED DEFORMATION (D2D Mesh + Bounce) -----
                Geometry currentWGeom = wGeom;
                
                // Inject Bounce by expanding target geometry
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

                // 1. Build an outline mask to ensure the outer edge is razor sharp
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

                    // 2. Map the texture into the grid geometry
                    for (int y = 0; y < yTiles; y++) {
                        for (int x = 0; x < xTiles; x++) {
                            D2D1_POINT_2F p1 = grid[y][x];     // TL
                            D2D1_POINT_2F p2 = grid[y][x+1];   // TR
                            D2D1_POINT_2F p3 = grid[y+1][x];   // BL
                            D2D1_POINT_2F p4 = grid[y+1][x+1]; // BR

                            // Bloat points slightly outward from center to ensure overlap/prevent internal tearing
                            D2D1_POINT_2F c = D2D1::Point2F((p1.x+p2.x+p3.x+p4.x)/4.0f, (p1.y+p2.y+p3.y+p4.y)/4.0f);
                            ID2D1PathGeometry* quadGeo = CreateQuadGeo(g_d2dFactory, 
                                BloatPoint(p1, c), BloatPoint(p2, c), BloatPoint(p4, c), BloatPoint(p3, c));

                            if (quadGeo) {
                                float sx = ((float)x / xTiles) * wGeom.width;
                                float sy = ((float)y / yTiles) * wGeom.height;
                                float sw = wGeom.width / xTiles;
                                float sh = wGeom.height / yTiles;

                                // Build Affine matrix mapping (sx,sy) to p1, (sx+sw, sy) to p2, etc.
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
    DestroyWindow(hGhost);
    delete data;
    return 0;
}

// -------------------------------------------------------------------------
// Core Setup Engine & Tracking Logic
// -------------------------------------------------------------------------
void StartGenieAnim(HWND hWnd, BOOL rising) {
    RECT rect;
    GetWindowRect(hWnd, &rect);
    RECT extRect;
    if (SUCCEEDED(DwmGetWindowAttribute(hWnd, DWMWA_EXTENDED_FRAME_BOUNDS, &extRect, sizeof(extRect)))) {
        rect = extRect;
    }

    int w = rect.right - rect.left;
    int h = rect.bottom - rect.top;

    if (w <= 0 || h <= 0) return;

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

    // Capture atomic configs per animation thread
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

    HDC hMemDC = CreateCompatibleDC(hScreenDC);
    data->hBitmap = CreateDIBSection(hScreenDC, &bmi, DIB_RGB_COLORS, &(data->pBits), NULL, 0);
    HBITMAP hOldBmp = (HBITMAP)SelectObject(hMemDC, data->hBitmap);

    if (rising) {
        BOOL fromCache = FALSE;
        {
            std::lock_guard<std::mutex> lock(g_CacheMutex);
            if (g_SnapshotCache.count(hWnd)) {
                HDC hCacheDC = CreateCompatibleDC(hScreenDC);
                HBITMAP hOldCacheBmp = (HBITMAP)SelectObject(hCacheDC, g_SnapshotCache[hWnd].first);
                BitBlt(hMemDC, 0, 0, w, h, hCacheDC, 0, 0, SRCCOPY);
                SelectObject(hCacheDC, hOldCacheBmp);
                DeleteDC(hCacheDC);

                DeleteObject(g_SnapshotCache[hWnd].first);
                g_SnapshotCache.erase(hWnd);
                fromCache = TRUE;
            }
        }
        if (!fromCache) {
            PrintWindow(hWnd, hMemDC, PW_CLIENTONLY | 0x00000002);
        }
    } else {
        BitBlt(hMemDC, 0, 0, w, h, hScreenDC, rect.left, rect.top, SRCCOPY);

        std::lock_guard<std::mutex> lock(g_CacheMutex);
        if (g_SnapshotCache.count(hWnd)) {
            DeleteObject(g_SnapshotCache[hWnd].first);
        }
        
        void* pCacheBits = nullptr;
        HBITMAP hCacheBmp = CreateDIBSection(hScreenDC, &bmi, DIB_RGB_COLORS, &pCacheBits, NULL, 0);
        HDC hCacheDC = CreateCompatibleDC(hScreenDC);
        HBITMAP hOldCacheBmp = (HBITMAP)SelectObject(hCacheDC, hCacheBmp);
        BitBlt(hCacheDC, 0, 0, w, h, hMemDC, 0, 0, SRCCOPY);
        SelectObject(hCacheDC, hOldCacheBmp);
        DeleteDC(hCacheDC);
        
        g_SnapshotCache[hWnd] = { hCacheBmp, pCacheBits };
    }

    // Direct2D uses premultiplied alpha - force an alpha channel injection
    BYTE* pixels = (BYTE*)data->pBits;
    for (int i = 0; i < w * h; i++) {
        pixels[i * 4 + 3] = 255;
    }

    SelectObject(hMemDC, hOldBmp);
    DeleteDC(hMemDC);
    ReleaseDC(NULL, hScreenDC);
    CreateThread(NULL, 0, GhostAnimationThread, data, 0, NULL);
}

// -------------------------------------------------------------------------
// Hooks
// -------------------------------------------------------------------------
BOOL WINAPI ShowWindow_Hook(HWND hWnd, int nCmdShow) {
    if (GetAncestor(hWnd, GA_ROOT) != hWnd) {
        return ShowWindow_Original(hWnd, nCmdShow);
    }

    if (nCmdShow == SW_MINIMIZE || nCmdShow == SW_SHOWMINIMIZED || nCmdShow == SW_SHOWMINNOACTIVE) {
        if (g_d2dFactory) {
            SetDwmTransitions(hWnd, FALSE);
            StartGenieAnim(hWnd, FALSE);
        }
        return ShowWindow_Original(hWnd, nCmdShow);
    }
    else if (nCmdShow == SW_RESTORE || nCmdShow == SW_SHOWNORMAL) {
        if (IsIconic(hWnd) && g_d2dFactory) {
            SetDwmTransitions(hWnd, FALSE);
            LONG_PTR exStyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
            SetWindowLongPtrW(hWnd, GWL_EXSTYLE, exStyle | WS_EX_LAYERED);
            SetLayeredWindowAttributes(hWnd, 0, 0, LWA_ALPHA);
            BOOL res = ShowWindow_Original(hWnd, nCmdShow);
            StartGenieAnim(hWnd, TRUE);
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
    }

    if (Msg == WM_SYSCOMMAND) {
        UINT cmd = wParam & 0xFFF0;
        if (cmd == SC_MINIMIZE) {
            if (GetAncestor(hWnd, GA_ROOT) != hWnd) {
                return DefWindowProcW_Original(hWnd, Msg, wParam, lParam);
            }
            if (g_d2dFactory) {
                SetDwmTransitions(hWnd, FALSE);
                StartGenieAnim(hWnd, FALSE);
            }
            return DefWindowProcW_Original(hWnd, Msg, wParam, lParam);
        }
        else if (cmd == SC_RESTORE && IsIconic(hWnd)) {
            if (GetAncestor(hWnd, GA_ROOT) != hWnd) {
                return DefWindowProcW_Original(hWnd, Msg, wParam, lParam);
            }
            if (g_d2dFactory) {
                SetDwmTransitions(hWnd, FALSE);
                LONG_PTR exStyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
                SetWindowLongPtrW(hWnd, GWL_EXSTYLE, exStyle | WS_EX_LAYERED);
                SetLayeredWindowAttributes(hWnd, 0, 0, LWA_ALPHA);
                LRESULT res = DefWindowProcW_Original(hWnd, Msg, wParam, lParam);
                StartGenieAnim(hWnd, TRUE);
                return res;
            }
        }
    }
    return DefWindowProcW_Original(hWnd, Msg, wParam, lParam);
}

BOOL Wh_ModInit() {
    LoadSettings();
    HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, __uuidof(ID2D1Factory), reinterpret_cast<void**>(&g_d2dFactory));
    if (FAILED(hr)) { g_d2dFactory = nullptr; }
    
    Wh_SetFunctionHook((void*)DefWindowProcW, (void*)DefWindowProcW_Hook, (void**)&DefWindowProcW_Original);
    Wh_SetFunctionHook((void*)ShowWindow, (void*)ShowWindow_Hook, (void**)&ShowWindow_Original);
    return TRUE;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}

void Wh_ModUninit() {
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
}
