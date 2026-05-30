// ==WindhawkMod==
// @id              genie-minimize-animation
// @name            Genie Animation Mod 
// @description     macOS-style genie minimize animation (enhanced with Direct2D mesh warp)
// @version         2.0.0
// @author          drgutman (after an idea by lolstijl)
// @github          https://github.com/drgutman
// @include         *
// @compilerOptions -ldwmapi -lgdi32 -ld2d1
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Genie Animation Mod v2
A macOS-inspired genie minimize/restore animation with true asymmetric stretch.

Since I don't know C++ either, this mod was written in opencode by MiMo v2.5 and fixed by Gemini 3.1 Pro in aistudio.

I tried to get it as close as possible to the MacOS version, but there's still room for improvement. 

There's still some flicker sometimes, especially at very high or very low speeds, but tbh, I'm kinda' done with it (I spent half a day fixing some really bad flickering that I got when you minimized a window, only for it to start flickering when you restored it; it was a pain in the behind). 

It still doesn't have all the controls that I wanted to add (speed control for each part of the animation).

Pretty much every time, when you first minimise a program, it will minimise to the middle of the taskbar.

Sometimes it even refuses to work at all, I have no idea why.

I wanted to add a gif to show how it works, but my desktop is a total mess and I'm too ashamed to record it LOL

P.S. - I can't seem to be able to add a second github link, but "https://github.com/lolstijl" is the page of the original author

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- duration_ms: 450
  $name: Animation duration (ms)
  $description: >-
    How long the genie animation takes, in milliseconds.
    Clamped to 50-2000.
- bottom_width: 42
  $name: Bottom width (px)
  $description: >-
    How narrow the bottom of the window gets when it reaches the taskbar icon.
    Clamped to 4-200.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <dwmapi.h>
#include <d2d1.h>
#include <math.h>
#include <atomic>
#include <unordered_map>
#include <mutex>

// ============================================================================
// Types & Hooks
// ============================================================================

typedef LRESULT (WINAPI *DefWindowProcW_t)(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
DefWindowProcW_t DefWindowProcW_Original;

typedef BOOL (WINAPI *ShowWindow_t)(HWND hWnd, int nCmdShow);
ShowWindow_t ShowWindow_Original;

struct GhostAnimData {
    HWND hGhost;
    HWND hRealWnd;
    
    HBITMAP hSnapDIB;
    void* pSnapBits;
    
    HBITMAP hGhostDIB;
    void* pGhostBits;
    
    int w, h;
    
    int minX;
    int minY;
    int ghostW;
    int ghostH;
    
    int localStartX;
    int localDockX;
    int localDockY;
    
    BOOL isRising;
    LONG_PTR originalExStyle;
};

// ============================================================================
// State
// ============================================================================

std::unordered_map<HWND, HBITMAP> g_SnapshotCache;
std::unordered_map<HWND, int> g_IconPositions;
std::unordered_map<HWND, RECT> g_RectCache;
std::mutex g_CacheMutex;

std::atomic<int> g_durationMs{450};
std::atomic<int> g_bottomWidth{42};

ID2D1Factory* g_d2dFactory = nullptr;

// ============================================================================
// Helpers
// ============================================================================

void LoadSettings() {
    int ms = Wh_GetIntSetting(L"duration_ms");
    if (ms < 50) ms = 50;
    if (ms > 2000) ms = 2000;
    g_durationMs.store(ms, std::memory_order_relaxed);
    
    int bw = Wh_GetIntSetting(L"bottom_width");
    if (bw < 4) bw = 4;
    if (bw > 200) bw = 200;
    g_bottomWidth.store(bw, std::memory_order_relaxed);
}

void SetDwmTransitions(HWND hWnd, BOOL enable) {
    BOOL disable = !enable;
    DwmSetWindowAttribute(hWnd, DWMWA_TRANSITIONS_FORCEDISABLED, &disable, sizeof(disable));
}

static inline float curve(float f) {
    if (f < 0.0f) f = 0.0f;
    if (f > 1.0f) f = 1.0f;
    return (1.0f - cosf(f * 3.14159265f)) / 2.0f;
}

struct MeshVertex { float x, y; };

static ID2D1PathGeometry* CreateQuadGeo(
    ID2D1Factory* factory,
    float x0, float y0, float x1, float y1,
    float x2, float y2, float x3, float y3)
{
    ID2D1PathGeometry* geo = nullptr;
    factory->CreatePathGeometry(&geo);
    if (!geo) return nullptr;
    ID2D1GeometrySink* sink = nullptr;
    geo->Open(&sink);
    sink->BeginFigure(D2D1::Point2F(x0, y0), D2D1_FIGURE_BEGIN_FILLED);
    sink->AddLine(D2D1::Point2F(x1, y1));
    sink->AddLine(D2D1::Point2F(x2, y2));
    sink->AddLine(D2D1::Point2F(x3, y3));
    sink->EndFigure(D2D1_FIGURE_END_CLOSED);
    sink->Close();
    sink->Release();
    return geo;
}

// ============================================================================
// Animation Thread
// ============================================================================

DWORD WINAPI GhostAnimationThread(LPVOID lpParam) {
    GhostAnimData* data = (GhostAnimData*)lpParam;
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);

    const int MESH_ROWS_HD = 120; 

    HWND hGhost = data->hGhost;
    HDC hScreenDC = GetDC(NULL);
    HDC hFrameDC = CreateCompatibleDC(hScreenDC);
    HBITMAP hOldFrame = (HBITMAP)SelectObject(hFrameDC, data->hGhostDIB);

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
            rt->CreateBitmap(D2D1::SizeU(data->w, data->h), data->pSnapBits, data->w * 4, bmpProps, &snapshotBmp);
            if (snapshotBmp) {
                D2D1_BITMAP_BRUSH_PROPERTIES brushProps = D2D1::BitmapBrushProperties(
                    D2D1_EXTEND_MODE_CLAMP, D2D1_EXTEND_MODE_CLAMP,
                    D2D1_BITMAP_INTERPOLATION_MODE_LINEAR
                );
                rt->CreateBitmapBrush(snapshotBmp, &brushProps, nullptr, &bmpBrush);
            }
        }
    }

    const double totalMs = (double)g_durationMs.load(std::memory_order_relaxed);
    MeshVertex verts[MESH_ROWS_HD + 1][2];

    LARGE_INTEGER qpcFreq, qpcStart, qpcNow;
    QueryPerformanceFrequency(&qpcFreq);
    QueryPerformanceCounter(&qpcStart);

    bool firstFrameRendered = false;

    for (;;) {
        QueryPerformanceCounter(&qpcNow);
        double elapsedMs = (qpcNow.QuadPart - qpcStart.QuadPart) * 1000.0 / qpcFreq.QuadPart;
        BOOL lastFrame = (elapsedMs >= totalMs);
        float progress = lastFrame ? 1.0f : (float)(elapsedMs / totalMs);
        
        float t = data->isRising ? (1.0f - progress) : progress;

        float phase1End = 0.60f;
        float topY, bottomY;
        float squeeze = 0.0f; 

        if (t < phase1End) {
            float p1 = t / phase1End;
            float easeDrop = p1 * p1 * p1; 
            bottomY = (float)data->h + (float)(data->localDockY - data->h) * easeDrop;
            topY = 0.0f;
            
            squeeze = p1 * p1 * p1 * p1; 
        } else {
            float p2 = (t - phase1End) / (1.0f - phase1End);
            float easeGravity = p2 * p2 * p2; 
            bottomY = (float)data->localDockY;
            topY = (float)data->localDockY * easeGravity;
            squeeze = 1.0f;
        }
        
        if (topY > bottomY) topY = bottomY;

        float alphaF = 1.0f;
        if (t > 0.85f) alphaF = 1.0f - ((t - 0.85f) / 0.15f);
        if (alphaF < 0.0f) alphaF = 0.0f;

        float bottomWidth = (float)g_bottomWidth.load(std::memory_order_relaxed);
        float localH = bottomY - topY;
        
        for (int i = 0; i <= MESH_ROWS_HD; i++) {
            float rowRatio = (float)i / (float)MESH_ROWS_HD;
            float rowY_local = topY + rowRatio * localH; 
            
            float funnelDist = rowY_local / (float)data->localDockY;
            float c = curve(funnelDist); 
            
            float funnelW = (float)data->w + (bottomWidth - (float)data->w) * c;
            float funnelCX = ((float)data->localStartX + (float)data->w / 2.0f) + 
                             ((float)data->localDockX - ((float)data->localStartX + (float)data->w / 2.0f)) * c;
            
            float rectW = (float)data->w;
            float rectCX = (float)data->localStartX + (float)data->w / 2.0f;
            
            float curW = rectW + (funnelW - rectW) * squeeze;
            float curCX = rectCX + (funnelCX - rectCX) * squeeze;
            
            verts[i][0] = { curCX - curW / 2.0f, rowY_local };
            verts[i][1] = { curCX + curW / 2.0f, rowY_local };
        }

        if (rt && bmpBrush && localH > 0.5f) {
            RECT bindRect = { 0, 0, data->ghostW, data->ghostH };
            rt->BindDC(hFrameDC, &bindRect);
            rt->BeginDraw();
            rt->Clear(D2D1::ColorF(0, 0, 0, 0));
            rt->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

            ID2D1PathGeometry* outlineGeo = nullptr;
            g_d2dFactory->CreatePathGeometry(&outlineGeo);
            if (outlineGeo) {
                ID2D1GeometrySink* sink = nullptr;
                outlineGeo->Open(&sink);
                sink->BeginFigure(D2D1::Point2F(verts[0][0].x, verts[0][0].y), D2D1_FIGURE_BEGIN_FILLED);
                for (int i = 1; i <= MESH_ROWS_HD; i++) {
                    sink->AddLine(D2D1::Point2F(verts[i][0].x, verts[i][0].y));
                }
                for (int i = MESH_ROWS_HD; i >= 0; i--) {
                    sink->AddLine(D2D1::Point2F(verts[i][1].x, verts[i][1].y));
                }
                sink->EndFigure(D2D1_FIGURE_END_CLOSED);
                sink->Close();

                ID2D1Layer* layer = nullptr;
                rt->CreateLayer(&layer);
                D2D1_LAYER_PARAMETERS layerParams = D2D1::LayerParameters();
                layerParams.geometricMask = outlineGeo;
                layerParams.maskAntialiasMode = D2D1_ANTIALIAS_MODE_PER_PRIMITIVE;
                
                rt->PushLayer(&layerParams, layer);

                for (int i = 0; i < MESH_ROWS_HD; i++) {
                    float tlx = verts[i][0].x, tly = verts[i][0].y;
                    float trx = verts[i][1].x, try_ = verts[i][1].y;
                    float blx = verts[i+1][0].x, bly = verts[i+1][0].y;
                    float brx = verts[i+1][1].x, bry = verts[i+1][1].y;

                    ID2D1PathGeometry* qg = CreateQuadGeo(g_d2dFactory, tlx, tly - 0.5f, trx, try_ - 0.5f, brx, bry + 0.5f, blx, bly + 0.5f);
                    if (!qg) continue;

                    float srcY1 = (float)i / MESH_ROWS_HD * (float)data->h;
                    float srcY2 = (float)(i + 1) / MESH_ROWS_HD * (float)data->h;
                    float stripH = srcY2 - srcY1; if (stripH < 0.001f) stripH = 0.001f;

                    float avgL = (tlx + blx) / 2.0f;
                    float avgW = ((trx - tlx) + (brx - blx)) / 2.0f; if (avgW < 0.001f) avgW = 0.001f;
                    float avgH = ((bly - tly) + (bry - try_)) / 2.0f; if (avgH < 0.001f) avgH = 0.001f;

                    D2D1_MATRIX_3X2_F bmat;
                    bmat._11 = avgW / (float)data->w;   bmat._12 = 0.0f;
                    bmat._21 = 0.0f;                    bmat._22 = avgH / stripH;
                    bmat._31 = avgL;                    bmat._32 = tly - srcY1 * bmat._22;
                    bmpBrush->SetTransform(bmat);

                    rt->FillGeometry(qg, bmpBrush);
                    qg->Release();
                }

                rt->PopLayer();
                if (layer) layer->Release();
                outlineGeo->Release();
            }
            rt->EndDraw();
        } else if (localH > 0.5f) {
            HDC hSnapDC = CreateCompatibleDC(hScreenDC);
            HBITMAP hOldSnap = (HBITMAP)SelectObject(hSnapDC, data->hSnapDIB);
            StretchBlt(hFrameDC, data->localStartX, (int)topY, data->w, (int)localH, hSnapDC, 0, 0, data->w, data->h, SRCCOPY);
            SelectObject(hSnapDC, hOldSnap);
            DeleteDC(hSnapDC);
        }

        POINT ptDst = { data->minX, data->minY };
        SIZE sz = { data->ghostW, data->ghostH };
        POINT ptSrc = { 0, 0 };
        BLENDFUNCTION bf = {};
        bf.BlendOp = AC_SRC_OVER;
        bf.SourceConstantAlpha = (BYTE)(255.0f * alphaF);
        bf.AlphaFormat = AC_SRC_ALPHA;
        
        UpdateLayeredWindow(hGhost, NULL, &ptDst, &sz, hFrameDC, &ptSrc, 0, &bf, ULW_ALPHA);

        if (!firstFrameRendered) {
            ShowWindow(hGhost, SW_SHOWNOACTIVATE);
            firstFrameRendered = true;
        }

        if (lastFrame) break;
        DwmFlush();
    }

    if (bmpBrush) bmpBrush->Release();
    if (snapshotBmp) snapshotBmp->Release();
    if (rt) rt->Release();
    
    SelectObject(hFrameDC, hOldFrame);
    DeleteDC(hFrameDC);
    ReleaseDC(NULL, hScreenDC);
    
    DeleteObject(data->hSnapDIB);
    DeleteObject(data->hGhostDIB);

    if (data->isRising) {
        SetLayeredWindowAttributes(data->hRealWnd, 0, 0, LWA_ALPHA);
        ShowWindow_Original(data->hRealWnd, SW_RESTORE);
        SetLayeredWindowAttributes(data->hRealWnd, 0, 255, LWA_ALPHA);
    }
    
    if (!(data->originalExStyle & WS_EX_LAYERED))
        SetWindowLongPtrW(data->hRealWnd, GWL_EXSTYLE, data->originalExStyle);
    SetDwmTransitions(data->hRealWnd, TRUE);

    ShowWindow(hGhost, SW_HIDE);
    PostMessage(hGhost, WM_CLOSE, 0, 0);

    delete data;
    return 0;
}

// ============================================================================
// ShowGhost — Pre-render frame 1 on main thread (ONLY USED FOR MINIMIZE)
// ============================================================================
static void ShowGhost(GhostAnimData* data) {
    if (!data) return;
    HWND hGhost = data->hGhost;

    HDC hScreenDC = GetDC(NULL);
    HDC hFrameDC = CreateCompatibleDC(hScreenDC);
    HBITMAP hOldFrame = (HBITMAP)SelectObject(hFrameDC, data->hGhostDIB);

    HDC hSnapDC = CreateCompatibleDC(hScreenDC);
    HBITMAP hOldSnap = (HBITMAP)SelectObject(hSnapDC, data->hSnapDIB);

    // Blit the window snapshot into the exact physical location on the blank canvas
    BitBlt(hFrameDC, data->localStartX, 0, data->w, data->h, hSnapDC, 0, 0, SRCCOPY);

    SelectObject(hSnapDC, hOldSnap);
    DeleteDC(hSnapDC);

    POINT ptDst = { data->minX, data->minY };
    SIZE sz = { data->ghostW, data->ghostH };
    POINT ptSrc = { 0, 0 };
    BLENDFUNCTION bf = {};
    bf.BlendOp = AC_SRC_OVER;
    bf.SourceConstantAlpha = 255;
    bf.AlphaFormat = AC_SRC_ALPHA;

    // Push frame 0 to DWM instantly
    UpdateLayeredWindow(hGhost, NULL, &ptDst, &sz, hFrameDC, &ptSrc, 0, &bf, ULW_ALPHA);
    ShowWindow(hGhost, SW_SHOWNOACTIVATE);

    SelectObject(hFrameDC, hOldFrame);
    DeleteDC(hFrameDC);
    ReleaseDC(NULL, hScreenDC);
}


// ============================================================================
// Prepare — capture + allocate DIB + create ghost (all on main thread)
// ============================================================================

static GhostAnimData* Prepare(HWND hWnd, BOOL rising) {
    RECT rect;
    GetWindowRect(hWnd, &rect);

    if (rising) {
        std::lock_guard<std::mutex> lock(g_CacheMutex);
        if (g_RectCache.count(hWnd)) rect = g_RectCache[hWnd];
    } else {
        std::lock_guard<std::mutex> lock(g_CacheMutex);
        g_RectCache[hWnd] = rect;
    }

    int w = rect.right - rect.left;
    int h = rect.bottom - rect.top;
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
        if (g_IconPositions.count(hWnd)) learnedTargetX = g_IconPositions[hWnd];
    }

    int taskbarY = workArea.bottom;
    
    int bw = Wh_GetIntSetting(L"bottom_width");
    if (bw < 4) bw = 4;
    
    int dockLeft = learnedTargetX - bw / 2;
    int dockRight = learnedTargetX + bw / 2;
    int minX = (rect.left < dockLeft ? rect.left : dockLeft) - 50; 
    int maxX = (rect.right > dockRight ? rect.right : dockRight) + 50;
    
    int ghostW = maxX - minX;
    int ghostH = taskbarY - rect.top;
    if (ghostH < h) ghostH = h;
    
    if (ghostW <= 0 || ghostH <= 0) return nullptr;

    HDC hScreenDC = GetDC(NULL);

    BITMAPINFO bmiSnap = {};
    bmiSnap.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmiSnap.bmiHeader.biWidth = w;
    bmiSnap.bmiHeader.biHeight = -h;
    bmiSnap.bmiHeader.biPlanes = 1;
    bmiSnap.bmiHeader.biBitCount = 32;
    bmiSnap.bmiHeader.biCompression = BI_RGB;

    void* pSnapBits = nullptr;
    HBITMAP hSnapDIB = CreateDIBSection(hScreenDC, &bmiSnap, DIB_RGB_COLORS, &pSnapBits, NULL, 0);
    
    HDC hMemDC = CreateCompatibleDC(hScreenDC);
    HBITMAP hOldBmp = (HBITMAP)SelectObject(hMemDC, hSnapDIB);

    if (rising) {
        BOOL fromCache = FALSE;
        {
            std::lock_guard<std::mutex> lock(g_CacheMutex);
            if (g_SnapshotCache.count(hWnd)) {
                HDC hCacheDC = CreateCompatibleDC(hScreenDC);
                HBITMAP hOldCacheBmp = (HBITMAP)SelectObject(hCacheDC, g_SnapshotCache[hWnd]);
                BitBlt(hMemDC, 0, 0, w, h, hCacheDC, 0, 0, SRCCOPY);
                SelectObject(hCacheDC, hOldCacheBmp);
                DeleteDC(hCacheDC);
                DeleteObject(g_SnapshotCache[hWnd]);
                g_SnapshotCache.erase(hWnd);
                fromCache = TRUE;
            }
        }
        if (!fromCache) PrintWindow(hWnd, hMemDC, PW_CLIENTONLY | 0x00000002);
    } else {
        BitBlt(hMemDC, 0, 0, w, h, hScreenDC, rect.left, rect.top, SRCCOPY);
        std::lock_guard<std::mutex> lock(g_CacheMutex);
        if (g_SnapshotCache.count(hWnd)) DeleteObject(g_SnapshotCache[hWnd]);
        g_SnapshotCache[hWnd] = CreateCompatibleBitmap(hScreenDC, w, h);
        HDC hCacheDC = CreateCompatibleDC(hScreenDC);
        HBITMAP hOldCacheBmp = (HBITMAP)SelectObject(hCacheDC, g_SnapshotCache[hWnd]);
        BitBlt(hCacheDC, 0, 0, w, h, hMemDC, 0, 0, SRCCOPY);
        SelectObject(hCacheDC, hOldCacheBmp);
        DeleteDC(hCacheDC);
    }
    SelectObject(hMemDC, hOldBmp);
    DeleteDC(hMemDC);

    BYTE* pixels = (BYTE*)pSnapBits;
    for (int i = 0; i < w * h; i++) pixels[i * 4 + 3] = 255;

    BITMAPINFO bmiGhost = {};
    bmiGhost.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmiGhost.bmiHeader.biWidth = ghostW;
    bmiGhost.bmiHeader.biHeight = -ghostH;
    bmiGhost.bmiHeader.biPlanes = 1;
    bmiGhost.bmiHeader.biBitCount = 32;
    bmiGhost.bmiHeader.biCompression = BI_RGB;
    void* pGhostBits = nullptr;
    HBITMAP hGhostDIB = CreateDIBSection(hScreenDC, &bmiGhost, DIB_RGB_COLORS, &pGhostBits, NULL, 0);

    ReleaseDC(NULL, hScreenDC);

    HWND hGhost = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_TRANSPARENT,
        L"STATIC", NULL, WS_POPUP,
        minX, rect.top, ghostW, ghostH,
        NULL, NULL, NULL, NULL
    );

    GhostAnimData* data = new GhostAnimData();
    data->hGhost = hGhost;
    data->hRealWnd = hWnd;
    data->hSnapDIB = hSnapDIB;
    data->pSnapBits = pSnapBits;
    data->hGhostDIB = hGhostDIB;
    data->pGhostBits = pGhostBits;
    data->w = w;
    data->h = h;
    data->minX = minX;
    data->minY = rect.top;
    data->ghostW = ghostW;
    data->ghostH = ghostH;
    data->localStartX = rect.left - minX;
    data->localDockX = learnedTargetX - minX;
    data->localDockY = taskbarY - rect.top;
    data->isRising = rising;
    data->originalExStyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
    
    return data;
}

// ============================================================================
// StartThread — launch the animation thread
// ============================================================================

static void StartThread(GhostAnimData* data) {
    if (data) CreateThread(NULL, 0, GhostAnimationThread, data, 0, NULL);
}

// ============================================================================
// Hooks
// ============================================================================

BOOL WINAPI ShowWindow_Hook(HWND hWnd, int nCmdShow) {
    if (nCmdShow == SW_MINIMIZE || nCmdShow == SW_SHOWMINIMIZED || nCmdShow == SW_SHOWMINNOACTIVE) {
        if (g_d2dFactory) {
            SetDwmTransitions(hWnd, FALSE);
            GhostAnimData* data = Prepare(hWnd, FALSE);
            if (data) {
                // MINIMIZE ONLY: Push Frame 0 immediately on main thread to kill flicker!
                ShowGhost(data);
                
                LONG_PTR exStyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
                SetWindowLongPtrW(hWnd, GWL_EXSTYLE, exStyle | WS_EX_LAYERED);
                SetLayeredWindowAttributes(hWnd, 0, 0, LWA_ALPHA);
                StartThread(data);
            }
            return ShowWindow_Original(hWnd, nCmdShow);
        }
    }
    else if (nCmdShow == SW_RESTORE || nCmdShow == SW_SHOWNORMAL) {
        if (IsIconic(hWnd) && g_d2dFactory) {
            SetDwmTransitions(hWnd, FALSE);
            GhostAnimData* data = Prepare(hWnd, TRUE);
            if (data) {
                // RESTORE ONLY: Skip ShowGhost entirely. Let thread handle the popup!
                StartThread(data);
            }
            return TRUE;
        }
    }
    return ShowWindow_Original(hWnd, nCmdShow);
}

LRESULT WINAPI DefWindowProcW_Hook(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
    if (Msg == WM_DESTROY) {
        std::lock_guard<std::mutex> lock(g_CacheMutex);
        if (g_SnapshotCache.count(hWnd)) { DeleteObject(g_SnapshotCache[hWnd]); g_SnapshotCache.erase(hWnd); }
        if (g_IconPositions.count(hWnd)) g_IconPositions.erase(hWnd);
        if (g_RectCache.count(hWnd)) g_RectCache.erase(hWnd);
    }
    if (Msg == WM_SYSCOMMAND) {
        UINT cmd = wParam & 0xFFF0;
        if (cmd == SC_MINIMIZE && g_d2dFactory) {
            SetDwmTransitions(hWnd, FALSE);
            GhostAnimData* data = Prepare(hWnd, FALSE);
            if (data) {
                // MINIMIZE ONLY: Push Frame 0 immediately on main thread to kill flicker!
                ShowGhost(data);
                
                LONG_PTR exStyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
                SetWindowLongPtrW(hWnd, GWL_EXSTYLE, exStyle | WS_EX_LAYERED);
                SetLayeredWindowAttributes(hWnd, 0, 0, LWA_ALPHA);
                StartThread(data);
            }
            return DefWindowProcW_Original(hWnd, Msg, wParam, lParam);
        }
        else if (cmd == SC_RESTORE && IsIconic(hWnd) && g_d2dFactory) {
            SetDwmTransitions(hWnd, FALSE);
            GhostAnimData* data = Prepare(hWnd, TRUE);
            if (data) {
                // RESTORE ONLY: Skip ShowGhost entirely. Let thread handle the popup!
                StartThread(data);
            }
            return 0;
        }
    }
    return DefWindowProcW_Original(hWnd, Msg, wParam, lParam);
}

// ============================================================================
// Mod Lifecycle
// ============================================================================

BOOL Wh_ModInit() {
    LoadSettings();
    HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED,
        __uuidof(ID2D1Factory), reinterpret_cast<void**>(&g_d2dFactory));
    if (FAILED(hr)) { Wh_Log(L"D2D1CreateFactory failed: 0x%08X", hr); g_d2dFactory = nullptr; }
    Wh_SetFunctionHook((void*)DefWindowProcW, (void*)DefWindowProcW_Hook, (void**)&DefWindowProcW_Original);
    Wh_SetFunctionHook((void*)ShowWindow, (void*)ShowWindow_Hook, (void**)&ShowWindow_Original);
    return TRUE;
}

void Wh_ModSettingsChanged() { LoadSettings(); }

void Wh_ModUninit() {
    if (g_d2dFactory) { g_d2dFactory->Release(); g_d2dFactory = nullptr; }
    std::lock_guard<std::mutex> lock(g_CacheMutex);
    for (auto& pair : g_SnapshotCache) DeleteObject(pair.second);
    g_SnapshotCache.clear();
    g_IconPositions.clear();
    g_RectCache.clear();
}
