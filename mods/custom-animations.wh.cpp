// ==WindhawkMod==
// @id            custom-animations
// @name          Animation Mod
// @description   Ultra-smooth Open/Close/Minimize/Maximize animation
// @version       1.0.0
// @author        Shoaib Hassan
// @github        https://github.com/shoaibhassan2
// @include       *
# @compilerOptions -ldwmapi -lgdi32 -lgdiplus
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Genie & Magic Lamp Animation Mod

Replaces the default Windows minimize and restore animations with smooth geometry deformation effects.

- KDE Magic Lamp: fluid 4-direction spatial bend
- macOS Genie: cosine-eased suction effect to dock
- MacSine: liquid wobble motion during animation
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- duration_ms: 450
  $name: Animation Duration (ms)
  $description: Time it takes for the animation to complete.

- animation_style: KDE
  $name: Animation Style
  $description: Choose the visual style of the minimize effect.
  $options:
    - KDE: KDE Magic Lamp
    - MacPinch: macOS Genie (Pinch Effect)
    - MacSine: macOS Genie (Sine Wave Wobble)
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <dwmapi.h>
#include <math.h>
#include <atomic>
#include <unordered_map>
#include <mutex>
#include <gdiplus.h>

using namespace Gdiplus;

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
};

// --- THE VAULTS ---
std::unordered_map<HWND, std::pair<HBITMAP, void*>> g_SnapshotCache;
std::unordered_map<HWND, int> g_IconPositions; 
std::mutex g_CacheMutex;

std::once_flag g_gdiplusFlag;
ULONG_PTR g_gdiplusToken = 0;

void InitGdiPlus() {
    GdiplusStartupInput gdiplusStartupInput;
    GdiplusStartup(&g_gdiplusToken, &gdiplusStartupInput, NULL);
}

// --- SETTINGS ---
std::atomic<int> g_durationMs{450};
std::atomic<int> g_animationStyle{0}; // 0 = KDE, 1 = Mac Pinch, 2 = Mac Sine

void LoadSettings() {
    int ms = Wh_GetIntSetting(L"duration_ms");
    if (ms < 50) ms = 50;
    if (ms > 2000) ms = 2000;
    g_durationMs.store(ms, std::memory_order_relaxed);

    PCWSTR styleStr = Wh_GetStringSetting(L"animation_style");
    int style = 0; // Default to KDE (0)
    if (styleStr) {
        if (wcscmp(styleStr, L"MacPinch") == 0) {
            style = 1;
        } else if (wcscmp(styleStr, L"MacSine") == 0) {
            style = 2;
        }
        Wh_FreeStringSetting(styleStr); // Free memory allocated by Windhawk
    }
    g_animationStyle.store(style, std::memory_order_relaxed);
}

void SetDwmTransitions(HWND hWnd, BOOL enable) {
    BOOL disable = !enable;
    DwmSetWindowAttribute(hWnd, DWMWA_TRANSITIONS_FORCEDISABLED, &disable, sizeof(disable));
}

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
// ENGINE 2: macOS Genie Physics Logic (Pinch & Sine variants)
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
        // EFFECT_SINE: Liquid wobble effect
        effectX = sinf((height - y) / fullHeight * PI * 4.0f) * w.width / 14.0f * k;
    } else { 
        // EFFECT_DEFAULT: Classic sharp pinch effect
        effectX = sinf(((height - y) / fullHeight) * 2.0f * PI + PI) * (w.x + w.width * tx - (i.x + i.width * tx)) / 7.0f * k;
    }

    *outX = w.x + x + offsetX + effectX;
    *outY = w.y + y + offsetY;
}

// -------------------------------------------------------------------------
// Fast Render Thread
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
    
    BITMAPINFO bmi = {0};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = vWidth;
    bmi.bmiHeader.biHeight = -vHeight; 
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void* pTargetBits = nullptr;
    HBITMAP hTargetBmp = CreateDIBSection(hScreenDC, &bmi, DIB_RGB_COLORS, &pTargetBits, NULL, 0);
    HBITMAP hOldBmp = (HBITMAP)SelectObject(hMemDC, hTargetBmp);

    Bitmap srcBmp(data->width, data->height, data->width * 4, PixelFormat32bppPARGB, (BYTE*)data->pBits);
    Bitmap targetBmp(vWidth, vHeight, vWidth * 4, PixelFormat32bppPARGB, (BYTE*)pTargetBits);
    Graphics gfx(&targetBmp);
    
    gfx.SetInterpolationMode(InterpolationModeBilinear); 
    gfx.SetSmoothingMode(SmoothingModeAntiAlias); 

    Geometry wGeom = { (float)data->targetRect.left, (float)data->targetRect.top, (float)data->width, (float)data->height };
    Geometry iGeom = { (float)data->targetDockX - 20.0f, (float)screenHeight - 40.0f, 40.0f, 40.0f };

    int position = POS_BOTTOM;
    float dx = (iGeom.x + iGeom.width / 2.0f) - (wGeom.x + wGeom.width / 2.0f);
    float dy = (iGeom.y + iGeom.height / 2.0f) - (wGeom.y + wGeom.height / 2.0f);
    if (fabsf(dx) > fabsf(dy)) position = (dx > 0) ? POS_RIGHT : POS_LEFT;
    else position = (dy > 0) ? POS_BOTTOM : POS_TOP;

    const double totalMs = (double)g_durationMs.load(std::memory_order_relaxed);
    LARGE_INTEGER qpcFreq, qpcStart, qpcNow;
    QueryPerformanceFrequency(&qpcFreq);
    QueryPerformanceCounter(&qpcStart);

    BOOL firstFrame = TRUE;
    MSG msg;

    for (;;) {
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        QueryPerformanceCounter(&qpcNow);
        double elapsedMs = (qpcNow.QuadPart - qpcStart.QuadPart) * 1000.0 / qpcFreq.QuadPart;
        BOOL lastFrame = (elapsedMs >= totalMs);
        
        float raw_p = lastFrame ? 1.0f : (float)(elapsedMs / totalMs);
        float t;

        if (style == 1 || style == 2) { 
            float eased_p = 0.5f * (1.0f - cosf(raw_p * PI));
            t = data->isRising ? (1.0f - eased_p) : eased_p;
        } else { 
            t = data->isRising ? (1.0f - raw_p) : raw_p;
        }

        gfx.Clear(Color(0, 0, 0, 0));

        for (int y = 0; y < yTiles; y++) {
            for (int x = 0; x < xTiles; x++) {
                float tx1 = (float)x / xTiles, ty1 = (float)y / yTiles;
                float tx2 = (float)(x + 1) / xTiles, ty2 = (float)(y + 1) / yTiles;
                float x1, y1, x2, y2, x4, y4;
                
                if (style == 1 || style == 2) { 
                    CalculateLampVertexMacOS(tx1, ty1, t, wGeom, iGeom, style, &x1, &y1);
                    CalculateLampVertexMacOS(tx2, ty1, t, wGeom, iGeom, style, &x2, &y2);
                    CalculateLampVertexMacOS(tx1, ty2, t, wGeom, iGeom, style, &x4, &y4);
                } else { 
                    CalculateLampVertexKDE(tx1, ty1, t, wGeom, iGeom, position, &x1, &y1);
                    CalculateLampVertexKDE(tx2, ty1, t, wGeom, iGeom, position, &x2, &y2);
                    CalculateLampVertexKDE(tx1, ty2, t, wGeom, iGeom, position, &x4, &y4);
                }
                
                float padX = (x == xTiles - 1) ? 0.0f : 0.5f;
                float padY = (y == yTiles - 1) ? 0.0f : 0.5f;
                
                PointF dest[3] = {
                    PointF(x1 - vLeft, y1 - vTop), 
                    PointF(x2 + padX - vLeft, y2 - vTop), 
                    PointF(x4 - vLeft, y4 + padY - vTop)
                };
                
                gfx.DrawImage(&srcBmp, dest, 3, 
                    tx1 * wGeom.width, ty1 * wGeom.height, 
                    (tx2 - tx1) * wGeom.width, (ty2 - ty1) * wGeom.height, 
                    UnitPixel); 
            }
        }

        POINT ptDst = { vLeft, vTop };
        SIZE  sz    = { vWidth, vHeight };
        POINT ptSrc = { 0, 0 };
        BLENDFUNCTION bf = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
        
        UpdateLayeredWindow(hGhost, NULL, &ptDst, &sz, hMemDC, &ptSrc, 0, &bf, ULW_ALPHA);

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
    std::call_once(g_gdiplusFlag, InitGdiPlus);

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

    HDC hScreenDC = GetDC(NULL);
    
    BITMAPINFO bmi = {0};
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
        SetDwmTransitions(hWnd, FALSE);
        StartGenieAnim(hWnd, FALSE);
        return ShowWindow_Original(hWnd, nCmdShow);
    }
    else if (nCmdShow == SW_RESTORE || nCmdShow == SW_SHOWNORMAL) {
        if (IsIconic(hWnd)) {
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
            SetDwmTransitions(hWnd, FALSE);
            StartGenieAnim(hWnd, FALSE);
            return DefWindowProcW_Original(hWnd, Msg, wParam, lParam);
        }
        else if (cmd == SC_RESTORE && IsIconic(hWnd)) {
            if (GetAncestor(hWnd, GA_ROOT) != hWnd) {
                return DefWindowProcW_Original(hWnd, Msg, wParam, lParam);
            }
            SetDwmTransitions(hWnd, FALSE);
            LONG_PTR exStyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
            SetWindowLongPtrW(hWnd, GWL_EXSTYLE, exStyle | WS_EX_LAYERED);
            SetLayeredWindowAttributes(hWnd, 0, 0, LWA_ALPHA);
            LRESULT res = DefWindowProcW_Original(hWnd, Msg, wParam, lParam);
            StartGenieAnim(hWnd, TRUE);
            return res;
        }
    }
    return DefWindowProcW_Original(hWnd, Msg, wParam, lParam);
}

BOOL Wh_ModInit() {
    LoadSettings();
    Wh_SetFunctionHook((void*)DefWindowProcW, (void*)DefWindowProcW_Hook, (void**)&DefWindowProcW_Original);
    Wh_SetFunctionHook((void*)ShowWindow, (void*)ShowWindow_Hook, (void**)&ShowWindow_Original);
    return TRUE;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}

void Wh_ModUninit() {
    std::lock_guard<std::mutex> lock(g_CacheMutex);
    for (auto& pair : g_SnapshotCache) {
        DeleteObject(pair.second.first);
    }
    g_SnapshotCache.clear();
    g_IconPositions.clear();
    
    if (g_gdiplusToken != 0) {
        GdiplusShutdown(g_gdiplusToken);
        g_gdiplusToken = 0;
    }
}
