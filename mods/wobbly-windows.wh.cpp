// ==WindhawkMod==
// @id            wobbly-windows
// @name          Wobbly Windows 
// @description   Adds KDE plasma inspired wobbly physics when moving or resizing windows 
// @version       1.0
// @author        potassiumuncher
// @github        https://github.com/Potassiumuncher
// @include       *
// @exclude       explorer.exe
// @license       GPL-2.0-or-later
// @compilerOptions -ldwmapi -lgdi32 -ld2d1 -luser32 -lcomctl32 -lwinmm
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Wobbly Windows 
(Please Configure Settings to your liking)

![Animation Preview](https://raw.githubusercontent.com/Potassiumuncher/MacOS-Animation-for-windows/main/Desktop2026.08.16-18.17.43.03-ezgif.com-video-to-gif-converter.gif)

Credit - KWin's Wobbly Windows for making this possible 

## Compatibility notes
Dose not work as intended in File Explorer, excluded for the time being until its resolved.

Added a setting to change corner radius so it is compatable with "Custom Window Corner Radius" mod (the default is 8 *windows default)

Added a setting to capture by Screen instead of printWindow so it is compatable with "Translucent windows" mod
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- wobble_while_dragging: true
  $name: Wobble While Dragging
  $description: Wobble the window while moving/dragging it.

- wobble_while_resizing: false
  $name: Wobble While Resizing (Work In Progress)
  $description: Wobble the window while resizing it.

- skip_backdrop_windows: false
  $name: Compatibility with "Translucent windows" mod (Experimental)
  $description: >-
    Capture Via Screen Instead Of PrintWindow (Turn this on if you really have to)

- wobbliness_level: 0
  $name: Wobbliness
  $description: >-
    0 is least wobbly, 4 is most wobbly
- advanced:
  - enable: false
    $name: Use advanced values
  - stiffness_pct: 15
    $name: Stiffness
    $description: >-
      Range 1-50, KDE default 15.
  - drag_pct: 80
    $name: Drag
    $description: >-
      Range 50-100, KDE default 80.
  - move_factor_pct: 10
    $name: Move Factor
    $description: >-
      Range 1-25, KDE default 10.
  $name: Advanced
  $description: >-
    When enabled, overrides stiffness, drag, and move factor on top of the
    Wobbliness level above.

- corner_radius: 8
  $name: Window Corner Radius (px)
  $description: >-
    Windows 11 default is 8px. 
    If you use the "Custom Window Corner Radius" mod, set this to the same value you configured there.
    
- tile_count: 0
  $name: Wobble Mesh Tile Count
  $description: >-
    Number of mesh tiles used per axis for the wobble effect. Higher
    values look smoother but cost more CPU/GPU. Set to 0 for automatic
    (tile count scales with window size, between 6 and 20). Any other
    value from 6 to 20 fixes the tile count for every window.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <dwmapi.h>
#include <d2d1.h>
#include <commctrl.h>
#include <tlhelp32.h>
#include <math.h>
#include <cstdlib>
#include <cstring>
#include <atomic>
#include <mutex>
#include <unordered_set>
#include <vector>

#ifndef DWMWA_EXTENDED_FRAME_BOUNDS
#define DWMWA_EXTENDED_FRAME_BOUNDS 9
#endif

#ifndef PW_RENDERFULLCONTENT
#define PW_RENDERFULLCONTENT 2
#endif

#ifndef DWMWA_WINDOW_CORNER_PREFERENCE
#define DWMWA_WINDOW_CORNER_PREFERENCE 33
#endif

#define WOBBLY_DWMWCP_DEFAULT    0u
#define WOBBLY_DWMWCP_DONOTROUND 1u
#define WOBBLY_DWMWCP_ROUND      2u
#define WOBBLY_DWMWCP_ROUNDSMALL 3u

#define X_TILES_MAX 20
#define Y_TILES_MAX 20
#define X_TILES_MIN 6
#define Y_TILES_MIN 6
#define TILE_TARGET_PX 48.0f

struct Geometry { float x, y, width, height; };

static int g_xTiles = X_TILES_MAX;
static int g_yTiles = Y_TILES_MAX;

using CreateWindowExW_t = HWND(WINAPI*)(DWORD, LPCWSTR, LPCWSTR, DWORD, int, int, int, int, HWND, HMENU, HINSTANCE, LPVOID);
CreateWindowExW_t CreateWindowExW_Original;

using CreateWindowExA_t = HWND(WINAPI*)(DWORD, LPCSTR, LPCSTR, DWORD, int, int, int, int, HWND, HMENU, HINSTANCE, LPVOID);
CreateWindowExA_t CreateWindowExA_Original;
ID2D1Factory* g_d2dFactory = nullptr;

std::recursive_mutex g_wobblyMutex;
std::atomic<bool> g_isUnloading{false};
HINSTANCE g_hInstance = NULL;
static bool g_classRegistered = false;

std::mutex g_subclassedSetMutex;
std::unordered_set<HWND> g_subclassedWindows;

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

static ParameterSet g_params = set_0;
std::atomic<bool> g_wobbleWhileDragging{true};
std::atomic<bool> g_wobbleWhileResizing{true};
std::atomic<int> g_cornerRadiusSetting{8};
std::atomic<int> g_tileCountSetting{0};
std::atomic<bool> g_captureTranslucentBackdrops{false};

static std::atomic<HWND> g_mainHwnd{NULL};
static std::atomic<HWND> g_overlayHwnd{NULL};
static int g_capX = 0, g_capY = 0, g_capW = 0, g_capH = 0;
static WobblyInfos g_wwi = {};
static bool g_isMoving = false, g_isSettling = false;
static LARGE_INTEGER g_lastTick = {};
static LARGE_INTEGER g_qpcFrequency = {};
static bool g_timerPeriodRaised = false;
static LONG_PTR g_oldExStyle = 0;
static int g_screenX, g_screenY, g_screenW, g_screenH;
static Geometry g_currentRect = {};
static Geometry g_resizeOriginRect = {};

static int g_wobblyBmpW = 0, g_wobblyBmpH = 0;

ID2D1DCRenderTarget* g_wobblyRT = nullptr;
ID2D1BitmapBrush* g_wobblyBrush = nullptr;
HBITMAP g_wobblyTargetBmp = NULL;
void* g_wobblyTargetBits = nullptr;
HDC g_wobblyMemDC = NULL;

static int g_overlayX = 0, g_overlayY = 0, g_overlayW = 0, g_overlayH = 0;
static BYTE* g_capturedBmp = nullptr;
static int g_capturedBmpW = 0, g_capturedBmpH = 0;
static size_t g_capturedBmpCap = 0;

void LoadSettings() {
    g_wobbleWhileDragging.store(Wh_GetIntSetting(L"wobble_while_dragging") != 0, std::memory_order_relaxed);
    g_wobbleWhileResizing.store(Wh_GetIntSetting(L"wobble_while_resizing") != 0, std::memory_order_relaxed);

    int wobblynessLevel = Wh_GetIntSetting(L"wobbliness_level");
    if (wobblynessLevel < 0) wobblynessLevel = 0;
    if (wobblynessLevel > 4) wobblynessLevel = 4;
    static const ParameterSet* levelSets[5] = { &set_0, &set_1, &set_2, &set_3, &set_4 };
    g_params = *levelSets[wobblynessLevel];

    if (Wh_GetIntSetting(L"advanced.enable") != 0) {
        int stiffness = Wh_GetIntSetting(L"advanced.stiffness_pct");
        if (stiffness < 1) stiffness = 1;
        if (stiffness > 50) stiffness = 50;
        int drag = Wh_GetIntSetting(L"advanced.drag_pct");
        if (drag < 50) drag = 50;
        if (drag > 100) drag = 100;
        int moveFactor = Wh_GetIntSetting(L"advanced.move_factor_pct");
        if (moveFactor < 1) moveFactor = 1;
        if (moveFactor > 25) moveFactor = 25;
        g_params.stiffness = stiffness / 100.0f;
        g_params.drag = drag / 100.0f;
        g_params.moveFactor = moveFactor / 100.0f;
    }

    int radius = Wh_GetIntSetting(L"corner_radius");
    if (radius < 0) radius = 0;
    if (radius > 64) radius = 64;
    g_cornerRadiusSetting.store(radius, std::memory_order_relaxed);

    int tileCount = Wh_GetIntSetting(L"tile_count");
    if (tileCount != 0) {
        if (tileCount < X_TILES_MIN) tileCount = X_TILES_MIN;
        if (tileCount > X_TILES_MAX) tileCount = X_TILES_MAX;
    }
    g_tileCountSetting.store(tileCount, std::memory_order_relaxed);

    g_captureTranslucentBackdrops.store(Wh_GetIntSetting(L"skip_backdrop_windows") != 0, std::memory_order_relaxed);
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
    if (!sink) { geo->Release(); return nullptr; }
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

static bool GetCornerRoundingForWindow(HWND hwnd, float* outRadiusPx) {
    int radiusSetting = g_cornerRadiusSetting.load(std::memory_order_relaxed);
    if (radiusSetting <= 0) return false;

    UINT pref = WOBBLY_DWMWCP_DEFAULT;
    HRESULT hr = DwmGetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &pref, sizeof(pref));
    if (FAILED(hr)) {
        return false;
    }
    if (pref == WOBBLY_DWMWCP_DONOTROUND) return false;

    *outRadiusPx = (float)radiusSetting;
    return true;
}

static void EnsureD2DFactory() {
    if (g_d2dFactory) return;
    HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, __uuidof(ID2D1Factory), reinterpret_cast<void**>(&g_d2dFactory));
    if (FAILED(hr)) {
        g_d2dFactory = nullptr;
        Wh_Log(L"D2D1CreateFactory failed, error %d", hr);
    }
}

static void CaptureWindowForWobbly(HWND hwnd) {
    EnsureD2DFactory();
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
    if (!hbmRaw || !rawBits) {
        Wh_Log(L"CreateDIBSection failed, skipping capture");
        if (hbmRaw) DeleteObject(hbmRaw);
        DeleteDC(hdcMem);
        ReleaseDC(NULL, hdcScreen);
        return;
    }
    HBITMAP hOldBmp = (HBITMAP)SelectObject(hdcMem, hbmRaw);

    bool alphaAlreadyOpaque = false;
    if (g_captureTranslucentBackdrops.load(std::memory_order_relaxed)) {
        BitBlt(hdcMem, 0, 0, rawW, rawH, hdcScreen, rcWin.left, rcWin.top, SRCCOPY);
        BYTE* pScreenCap = (BYTE*)rawBits;
        for (size_t i = 0; i < (size_t)rawW * rawH; i++) {
            pScreenCap[i * 4 + 3] = 255;
        }
        alphaAlreadyOpaque = true;
    } else {
        PrintWindow(hwnd, hdcMem, PW_RENDERFULLCONTENT);
    }
    float cornerRadiusPx = 8.0f;
    bool roundCorners = GetCornerRoundingForWindow(hwnd, &cornerRadiusPx);
    float capRadiusX = cornerRadiusPx;
    float capRadiusY = cornerRadiusPx;
    if (capRadiusX > g_capW / 2.0f) capRadiusX = g_capW / 2.0f;
    if (capRadiusY > g_capH / 2.0f) capRadiusY = g_capH / 2.0f;

    BYTE* pRaw = (BYTE*)rawBits;
    bool doCorners = roundCorners && capRadiusX > 0.5f && capRadiusY > 0.5f;
    int cornerZoneW = doCorners ? (int)ceilf(capRadiusX) + 1 : 0;
    int cornerZoneH = doCorners ? (int)ceilf(capRadiusY) + 1 : 0;

    if (!alphaAlreadyOpaque) {
        for (int y = 0; y < rawH; y++) {
            BYTE* row = pRaw + (size_t)y * rawW * 4;
            for (int x = 0; x < rawW; x++) {
                BYTE a = row[x * 4 + 3];
                if (row[x * 4 + 0] > a) row[x * 4 + 0] = a;
                if (row[x * 4 + 1] > a) row[x * 4 + 1] = a;
                if (row[x * 4 + 2] > a) row[x * 4 + 2] = a;
            }
        }
    }

    if (doCorners) {
        int yStartTop = g_capY < 0 ? 0 : g_capY;
        int yEndTop = g_capY + cornerZoneH; if (yEndTop > rawH) yEndTop = rawH;
        int yStartBot = g_capY + g_capH - cornerZoneH; if (yStartBot < 0) yStartBot = 0;
        int yEndBot = g_capY + g_capH; if (yEndBot > rawH) yEndBot = rawH;

        auto maskCorner = [&](int y0, int y1) {
            for (int y = y0; y < y1; y++) {
                BYTE* row = pRaw + (size_t)y * rawW * 4;
                float ly = (float)(y - g_capY);
                bool nearVEdge = (ly < capRadiusY) || (ly > g_capH - capRadiusY);
                if (!nearVEdge) continue;
                float nearestY = ly < capRadiusY ? capRadiusY : g_capH - capRadiusY;
                for (int side = 0; side < 2; side++) {
                    int xStart = side == 0 ? (g_capX < 0 ? 0 : g_capX)
                                            : (g_capX + g_capW - cornerZoneW < 0 ? 0 : g_capX + g_capW - cornerZoneW);
                    int xEnd = side == 0 ? (g_capX + cornerZoneW > rawW ? rawW : g_capX + cornerZoneW)
                                          : (g_capX + g_capW > rawW ? rawW : g_capX + g_capW);
                    for (int x = xStart; x < xEnd; x++) {
                        float lx = (float)(x - g_capX);
                        bool nearHEdge = (lx < capRadiusX) || (lx > g_capW - capRadiusX);
                        if (!nearHEdge) continue;
                        float nearestX = lx < capRadiusX ? capRadiusX : g_capW - capRadiusX;
                        float dx = (lx - nearestX) / capRadiusX;
                        float dy = (ly - nearestY) / capRadiusY;
                        if (dx * dx + dy * dy > 1.0f) {
                            row[x * 4 + 0] = 0; row[x * 4 + 1] = 0; row[x * 4 + 2] = 0; row[x * 4 + 3] = 0;
                        }
                    }
                }
            }
        };
        maskCorner(yStartTop, yEndTop);
        maskCorner(yStartBot, yEndBot);
    }

    g_screenX = GetSystemMetrics(SM_XVIRTUALSCREEN);
    g_screenY = GetSystemMetrics(SM_YVIRTUALSCREEN);
    g_screenW = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    g_screenH = GetSystemMetrics(SM_CYVIRTUALSCREEN);

    if (g_overlayHwnd) {
        SetWindowPos(g_overlayHwnd, NULL, g_screenX, g_screenY, g_screenW, g_screenH, SWP_NOZORDER | SWP_NOACTIVATE);
    }

    int tileCountSetting = g_tileCountSetting.load(std::memory_order_relaxed);
    if (tileCountSetting == 0) {
        int tilesForW = (int)(g_capW / TILE_TARGET_PX + 0.5f);
        int tilesForH = (int)(g_capH / TILE_TARGET_PX + 0.5f);
        g_xTiles = tilesForW < X_TILES_MIN ? X_TILES_MIN : (tilesForW > X_TILES_MAX ? X_TILES_MAX : tilesForW);
        g_yTiles = tilesForH < Y_TILES_MIN ? Y_TILES_MIN : (tilesForH > Y_TILES_MAX ? Y_TILES_MAX : tilesForH);
    } else {
        g_xTiles = tileCountSetting;
        g_yTiles = tileCountSetting;
    }

    int margin = (int)(g_capW * 0.18f + g_capH * 0.18f) + 96;
    int neededW = g_capW + margin * 2;
    int neededH = g_capH + margin * 2;
    if (neededW > g_screenW) neededW = g_screenW;
    if (neededH > g_screenH) neededH = g_screenH;

    if (g_wobblyBmpW != neededW || g_wobblyBmpH != neededH) {
        if (g_wobblyMemDC) { DeleteDC(g_wobblyMemDC); g_wobblyMemDC = NULL; }
        if (g_wobblyTargetBmp) { DeleteObject(g_wobblyTargetBmp); g_wobblyTargetBmp = NULL; }
        if (g_wobblyRT) { g_wobblyRT->Release(); g_wobblyRT = nullptr; }
        g_wobblyBmpW = neededW;
        g_wobblyBmpH = neededH;
    }

    if (!g_wobblyMemDC) {
        g_wobblyMemDC = CreateCompatibleDC(hdcScreen);
        BITMAPINFO bmiScreen = {{sizeof(BITMAPINFOHEADER), g_wobblyBmpW, -g_wobblyBmpH, 1, 32, BI_RGB}};
        g_wobblyTargetBmp = CreateDIBSection(hdcScreen, &bmiScreen, DIB_RGB_COLORS, &g_wobblyTargetBits, NULL, 0);
        SelectObject(g_wobblyMemDC, g_wobblyTargetBmp);
    }

    if (!g_wobblyRT && g_d2dFactory) {

        D2D1_RENDER_TARGET_PROPERTIES rtProps = D2D1::RenderTargetProperties(
            D2D1_RENDER_TARGET_TYPE_DEFAULT,
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
            0, 0, D2D1_RENDER_TARGET_USAGE_GDI_COMPATIBLE, D2D1_FEATURE_LEVEL_DEFAULT
        );
        g_d2dFactory->CreateDCRenderTarget(&rtProps, &g_wobblyRT);
    }

    size_t neededCap = (size_t)rawW * rawH * 4;
    if (neededCap > 0) {
        if (g_capturedBmpCap < neededCap) {
            BYTE* grown = (BYTE*)realloc(g_capturedBmp, neededCap);
            if (grown) { g_capturedBmp = grown; g_capturedBmpCap = neededCap; }
        }
        if (g_capturedBmp && g_capturedBmpCap >= neededCap) {
            memcpy(g_capturedBmp, rawBits, neededCap);
            g_capturedBmpW = rawW;
            g_capturedBmpH = rawH;
        }
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

static void InitWobblyInfo(float x, float y, float w, float h, bool isResize) {
    memset(&g_wwi, 0, sizeof(WobblyInfos));
    UpdateWobblyOrigin(x, y, w, h);
    for (int i = 0; i < 16; ++i) g_wwi.position[i] = g_wwi.origin[i];
    if (isResize) {
        g_wwi.can_wobble_top = false; g_wwi.can_wobble_bottom = false;
        g_wwi.can_wobble_left = false; g_wwi.can_wobble_right = false;
        g_resizeOriginRect.x = x; g_resizeOriginRect.y = y;
        g_resizeOriginRect.width = w; g_resizeOriginRect.height = h;
    } else {
        g_wwi.can_wobble_top = true; g_wwi.can_wobble_bottom = true;
        g_wwi.can_wobble_left = true; g_wwi.can_wobble_right = true;
    }
    g_wwi.wobblying = true;
}

static void HeightRingLinearMean(Pair* data, Pair* buffer) {
    auto corner = [&](int idx, int n0, int n1, int n2) {
        buffer[idx].x = (data[n0].x + data[n1].x + data[n2].x + 3.0f * data[idx].x) / 6.0f;
        buffer[idx].y = (data[n0].y + data[n1].y + data[n2].y + 3.0f * data[idx].y) / 6.0f;
    };
    corner(0, 1, 4, 5);
    corner(3, 2, 7, 6);
    corner(12, 13, 8, 9);
    corner(15, 14, 11, 10);

    auto edge5 = [&](int idx, int n0, int n1, int n2, int n3, int n4) {
        buffer[idx].x = (data[n0].x + data[n1].x + data[n2].x + data[n3].x + data[n4].x + 5.0f * data[idx].x) / 10.0f;
        buffer[idx].y = (data[n0].y + data[n1].y + data[n2].y + data[n3].y + data[n4].y + 5.0f * data[idx].y) / 10.0f;
    };
    edge5(1, 0, 2, 5, 4, 6);
    edge5(2, 1, 3, 6, 5, 7);
    edge5(13, 12, 14, 9, 8, 10);
    edge5(14, 13, 15, 10, 9, 11);
    edge5(4, 0, 8, 5, 1, 9);
    edge5(8, 4, 12, 9, 5, 13);
    edge5(7, 3, 11, 6, 2, 10);
    edge5(11, 7, 15, 10, 6, 14);

    auto interior8 = [&](int idx, int n0, int n1, int n2, int n3, int n4, int n5, int n6, int n7) {
        buffer[idx].x = (data[n0].x + data[n1].x + data[n2].x + data[n3].x + data[n4].x + data[n5].x + data[n6].x + data[n7].x + 8.0f * data[idx].x) / 16.0f;
        buffer[idx].y = (data[n0].y + data[n1].y + data[n2].y + data[n3].y + data[n4].y + data[n5].y + data[n6].y + data[n7].y + 8.0f * data[idx].y) / 16.0f;
    };
    interior8(5, 4, 6, 1, 9, 0, 2, 8, 10);
    interior8(6, 5, 7, 2, 10, 1, 3, 9, 11);
    interior8(9, 8, 10, 5, 13, 4, 6, 12, 14);
    interior8(10, 9, 11, 6, 14, 5, 7, 13, 15);

    for (int i = 0; i < 16; ++i) data[i] = buffer[i];
}

static void StepPhysics(float time) {
    float x_length = g_currentRect.width / 3.0f, y_length = g_currentRect.height / 3.0f;
    float acc_sum = 0.0f, vel_sum = 0.0f;
    float k = g_params.stiffness;

    for (int i = 0; i < 16; ++i) {
        if (g_wwi.constraint[i]) {
            g_wwi.acceleration[i].x = (g_wwi.origin[i].x - g_wwi.position[i].x) * k;
            g_wwi.acceleration[i].y = (g_wwi.origin[i].y - g_wwi.position[i].y) * k;
            continue;
        }

        int x = i % 4, y = i / 4;
        Pair pos = g_wwi.position[i];
        float ax = 0.0f, ay = 0.0f;
        int n = 0;

        if (x > 0) { Pair l = g_wwi.position[i - 1]; ax += (x_length - (pos.x - l.x)) * k; ay += (l.y - pos.y) * k; n++; }
        if (x < 3) { Pair r = g_wwi.position[i + 1]; ax += ((r.x - pos.x) - x_length) * k; ay += (r.y - pos.y) * k; n++; }
        if (y > 0) { Pair u = g_wwi.position[i - 4]; ax += (u.x - pos.x) * k; ay += (y_length - (pos.y - u.y)) * k; n++; }
        if (y < 3) { Pair d = g_wwi.position[i + 4]; ax += (d.x - pos.x) * k; ay += ((d.y - pos.y) - y_length) * k; n++; }

        g_wwi.acceleration[i].x = ax / n;
        g_wwi.acceleration[i].y = ay / n;
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
    float ux = 1.0f - tx, uy = 1.0f - ty;
    float ux2 = ux * ux, tx2 = tx * tx;
    float uy2 = uy * uy, ty2 = ty * ty;
    float px[4] = { ux2 * ux, 3.0f * ux2 * tx, 3.0f * ux * tx2, tx2 * tx };
    float py[4] = { uy2 * uy, 3.0f * uy2 * ty, 3.0f * uy * ty2, ty2 * ty };
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

    float minX = g_wwi.position[0].x, maxX = g_wwi.position[0].x;
    float minY = g_wwi.position[0].y, maxY = g_wwi.position[0].y;
    for (int i = 1; i < 16; ++i) {
        if (g_wwi.position[i].x < minX) minX = g_wwi.position[i].x;
        if (g_wwi.position[i].x > maxX) maxX = g_wwi.position[i].x;
        if (g_wwi.position[i].y < minY) minY = g_wwi.position[i].y;
        if (g_wwi.position[i].y > maxY) maxY = g_wwi.position[i].y;
    }
    int bulge = (int)((maxX - minX) * 0.02f + (maxY - minY) * 0.02f) + 4;

    g_overlayX = (int)minX - bulge;
    g_overlayY = (int)minY - bulge;
    int wantW = (int)(maxX - minX) + bulge * 2;
    int wantH = (int)(maxY - minY) + bulge * 2;
    if (wantW > g_screenW) wantW = g_screenW;
    if (wantH > g_screenH) wantH = g_screenH;
    if (wantW < 1) wantW = 1;
    if (wantH < 1) wantH = 1;

    if (wantW > g_wobblyBmpW || wantH > g_wobblyBmpH) {
        int growW = wantW + wantW / 4 + 64;
        int growH = wantH + wantH / 4 + 64;
        int newW = growW > g_wobblyBmpW ? growW : g_wobblyBmpW;
        int newH = growH > g_wobblyBmpH ? growH : g_wobblyBmpH;
        if (newW > g_screenW) newW = g_screenW;
        if (newH > g_screenH) newH = g_screenH;

        if (g_wobblyMemDC) { DeleteDC(g_wobblyMemDC); g_wobblyMemDC = NULL; }
        if (g_wobblyTargetBmp) { DeleteObject(g_wobblyTargetBmp); g_wobblyTargetBmp = NULL; }
        if (g_wobblyRT) { g_wobblyRT->Release(); g_wobblyRT = nullptr; }
        g_wobblyBmpW = newW;
        g_wobblyBmpH = newH;

        HDC hdcScreen2 = GetDC(NULL);
        g_wobblyMemDC = CreateCompatibleDC(hdcScreen2);
        BITMAPINFO bmiScreen = {{sizeof(BITMAPINFOHEADER), g_wobblyBmpW, -g_wobblyBmpH, 1, 32, BI_RGB}};
        g_wobblyTargetBmp = CreateDIBSection(hdcScreen2, &bmiScreen, DIB_RGB_COLORS, &g_wobblyTargetBits, NULL, 0);
        SelectObject(g_wobblyMemDC, g_wobblyTargetBmp);
        ReleaseDC(NULL, hdcScreen2);

        if (g_d2dFactory) {
            D2D1_RENDER_TARGET_PROPERTIES rtProps = D2D1::RenderTargetProperties(
                D2D1_RENDER_TARGET_TYPE_DEFAULT,
                D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
                0, 0, D2D1_RENDER_TARGET_USAGE_GDI_COMPATIBLE, D2D1_FEATURE_LEVEL_DEFAULT
            );
            g_d2dFactory->CreateDCRenderTarget(&rtProps, &g_wobblyRT);
        }
        if (g_wobblyRT && g_capturedBmp) {
            D2D1_BITMAP_PROPERTIES bmpProps = D2D1::BitmapProperties(
                D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)
            );
            ID2D1Bitmap* pRawBmp = nullptr;
            g_wobblyRT->CreateBitmap(D2D1::SizeU(g_capturedBmpW, g_capturedBmpH), g_capturedBmp, g_capturedBmpW * 4, bmpProps, &pRawBmp);
            if (pRawBmp) {
                if (g_wobblyBrush) { g_wobblyBrush->Release(); g_wobblyBrush = nullptr; }
                D2D1_BITMAP_BRUSH_PROPERTIES brushProps = D2D1::BitmapBrushProperties(
                    D2D1_EXTEND_MODE_CLAMP, D2D1_EXTEND_MODE_CLAMP, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR
                );
                g_wobblyRT->CreateBitmapBrush(pRawBmp, &brushProps, nullptr, &g_wobblyBrush);
                pRawBmp->Release();
            }
        }
        if (!g_wobblyRT || !g_wobblyBrush || !g_wobblyMemDC) return;
    }

    g_overlayW = wantW;
    g_overlayH = wantH;

    float vLeft = (float)g_overlayX;
    float vTop = (float)g_overlayY;

    static Pair gridPts[(X_TILES_MAX + 1) * (Y_TILES_MAX + 1)];
    int gridStride = g_xTiles + 1;
    for (int gy = 0; gy <= g_yTiles; gy++) {
        for (int gx = 0; gx <= g_xTiles; gx++) {
            Pair p = ComputeBezierPoint((float)gx / g_xTiles, (float)gy / g_yTiles);
            p.x -= vLeft; p.y -= vTop;
            gridPts[gy * gridStride + gx] = p;
        }
    }

    RECT bindRect = { 0, 0, g_overlayW, g_overlayH };
    g_wobblyRT->BindDC(g_wobblyMemDC, &bindRect);
    g_wobblyRT->BeginDraw();
    g_wobblyRT->Clear(D2D1::ColorF(0, 0, 0, 0));
    g_wobblyRT->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

    ID2D1PathGeometry* outlineGeo = nullptr;
    g_d2dFactory->CreatePathGeometry(&outlineGeo);
    ID2D1GeometrySink* sink = nullptr;
    if (outlineGeo) {
        outlineGeo->Open(&sink);
        if (!sink) {
            outlineGeo->Release();
            outlineGeo = nullptr;
        }
    }
    if (outlineGeo && sink) {
        Pair startP = gridPts[0];
        sink->BeginFigure(D2D1::Point2F(startP.x, startP.y), D2D1_FIGURE_BEGIN_FILLED);
        for (int x = 1; x <= g_xTiles; x++) {
            Pair p = gridPts[x];
            sink->AddLine(D2D1::Point2F(p.x, p.y));
        }
        for (int y = 1; y <= g_yTiles; y++) {
            Pair p = gridPts[y * gridStride + g_xTiles];
            sink->AddLine(D2D1::Point2F(p.x, p.y));
        }
        for (int x = g_xTiles - 1; x >= 0; x--) {
            Pair p = gridPts[g_yTiles * gridStride + x];
            sink->AddLine(D2D1::Point2F(p.x, p.y));
        }
        for (int y = g_yTiles - 1; y >= 0; y--) {
            Pair p = gridPts[y * gridStride];
            sink->AddLine(D2D1::Point2F(p.x, p.y));
        }
        sink->EndFigure(D2D1_FIGURE_END_CLOSED);
        sink->Close();
        sink->Release();

        ID2D1Layer* layer = nullptr;
        g_wobblyRT->CreateLayer(&layer);
        D2D1_LAYER_PARAMETERS layerParams = D2D1::LayerParameters();
        layerParams.geometricMask = outlineGeo;
        layerParams.maskAntialiasMode = D2D1_ANTIALIAS_MODE_PER_PRIMITIVE;
        g_wobblyRT->PushLayer(&layerParams, layer);

        for (int y = 0; y < g_yTiles; y++) {
            for (int x = 0; x < g_xTiles; x++) {
                float tx1 = (float)x / g_xTiles, ty1 = (float)y / g_yTiles;

                Pair bp1 = gridPts[y * gridStride + x];
                Pair bp2 = gridPts[y * gridStride + x + 1];
                Pair bp3 = gridPts[(y + 1) * gridStride + x];
                Pair bp4 = gridPts[(y + 1) * gridStride + x + 1];

                D2D1_POINT_2F p1 = D2D1::Point2F(bp1.x, bp1.y);
                D2D1_POINT_2F p2 = D2D1::Point2F(bp2.x, bp2.y);
                D2D1_POINT_2F p3 = D2D1::Point2F(bp3.x, bp3.y);
                D2D1_POINT_2F p4 = D2D1::Point2F(bp4.x, bp4.y);

                D2D1_POINT_2F c = D2D1::Point2F((p1.x+p2.x+p3.x+p4.x)/4.0f, (p1.y+p2.y+p3.y+p4.y)/4.0f);
                ID2D1PathGeometry* quadGeo = CreateQuadGeo(g_d2dFactory,
                    BloatPoint(p1, c), BloatPoint(p2, c), BloatPoint(p4, c), BloatPoint(p3, c));

                if (quadGeo) {
                    float sx = (float)g_capX + tx1 * g_capW;
                    float sy = (float)g_capY + ty1 * g_capH;
                    float sw = (float)g_capW / g_xTiles;
                    float sh = (float)g_capH / g_yTiles;

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
    POINT ptW = {g_overlayX, g_overlayY}; 
    SIZE sz = {g_overlayW, g_overlayH}; 
    BLENDFUNCTION bl = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    
    UpdateLayeredWindow(g_overlayHwnd, hdcScreen, &ptW, &sz, g_wobblyMemDC, &ptS, 0, &bl, ULW_ALPHA);
    ReleaseDC(NULL, hdcScreen);
}

static void CleanupWobblyD2D() {
    if (g_wobblyBrush) { g_wobblyBrush->Release(); g_wobblyBrush = nullptr; }
}

static void ReleaseTimerPeriodIfRaised() {
    if (g_timerPeriodRaised) {
        timeEndPeriod(1);
        g_timerPeriodRaised = false;
    }
}

static void ReadAndClearWobblyTrackingState(HWND* mainHwndOut, LONG_PTR* oldExStyleOut) {
    g_isMoving = false;
    g_isSettling = false;
    *mainHwndOut = g_mainHwnd;
    *oldExStyleOut = g_oldExStyle;
    g_mainHwnd = NULL;
}

static void TeardownWobblyResources() {
    if (g_overlayHwnd) {
        KillTimer(g_overlayHwnd, 1);
        ShowWindow(g_overlayHwnd, SW_HIDE);
    }
    CleanupWobblyD2D();
    {
        std::lock_guard<std::recursive_mutex> lock(g_wobblyMutex);
        if (g_wobblyRT) { g_wobblyRT->Release(); g_wobblyRT = nullptr; }
        if (g_wobblyMemDC) { DeleteDC(g_wobblyMemDC); g_wobblyMemDC = NULL; }
        if (g_wobblyTargetBmp) { DeleteObject(g_wobblyTargetBmp); g_wobblyTargetBmp = NULL; }
        if (g_capturedBmp) { free(g_capturedBmp); g_capturedBmp = nullptr; g_capturedBmpCap = 0; }
    }
    ReleaseTimerPeriodIfRaised();
}

static void RestoreWindowAfterWobbly(HWND mainHwnd, LONG_PTR oldExStyle) {
    if (mainHwnd && IsWindow(mainHwnd)) {
        SetLayeredWindowAttributes(mainHwnd, 0, 255, LWA_ALPHA);
        if (!(oldExStyle & WS_EX_LAYERED)) {
            LONG_PTR ex = GetWindowLongPtrW(mainHwnd, GWL_EXSTYLE);
            SetWindowLongPtrW(mainHwnd, GWL_EXSTYLE, ex & ~WS_EX_LAYERED);
        }
    }
}

static void FinishWobblyTracking() {
    HWND mainHwnd; LONG_PTR oldExStyle;
    ReadAndClearWobblyTrackingState(&mainHwnd, &oldExStyle);
    RestoreWindowAfterWobbly(mainHwnd, oldExStyle);
    TeardownWobblyResources();
}

static LRESULT CALLBACK OverlayProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    if (msg == g_wmDetach) {
        KillTimer(hwnd, 1);
        DestroyWindow(hwnd);
        return 0;
    } else if (msg == WM_CLOSE) {
        KillTimer(hwnd, 1);
        DestroyWindow(hwnd);
        return 0;
    } else if (msg == WM_NCDESTROY) {

        if (g_overlayHwnd == hwnd) g_overlayHwnd = NULL;
        HWND mainHwndToRestore = NULL; LONG_PTR oldExStyleToRestore = 0;
        bool wasTracking = false;
        {
            std::lock_guard<std::recursive_mutex> lock(g_wobblyMutex);
            if (g_isMoving || g_isSettling) {
                wasTracking = true;
                ReadAndClearWobblyTrackingState(&mainHwndToRestore, &oldExStyleToRestore);
            }
        }
        if (wasTracking) {
            RestoreWindowAfterWobbly(mainHwndToRestore, oldExStyleToRestore);
            TeardownWobblyResources();
        }
        return DefWindowProc(hwnd, msg, wp, lp);
    } else if (msg == WM_TIMER) {
        if (g_isUnloading.load(std::memory_order_relaxed)) {
            KillTimer(hwnd, 1);
            return 0;
        }
        std::unique_lock<std::recursive_mutex> lock(g_wobblyMutex);
        if (g_isMoving || g_isSettling) {
            if (!IsWindow(g_mainHwnd)) {
                g_isMoving = false;
                g_isSettling = false;
                KillTimer(hwnd, 1);
                ShowWindow(hwnd, SW_HIDE);
                CleanupWobblyD2D();
                ReleaseTimerPeriodIfRaised();
                g_mainHwnd = NULL;
                return 0;
            }
            LONG_PTR curExStyle = GetWindowLongPtrW(g_mainHwnd, GWL_EXSTYLE);
            if (!(curExStyle & WS_EX_LAYERED)) {
                SetWindowLongPtrW(g_mainHwnd, GWL_EXSTYLE, curExStyle | WS_EX_LAYERED);
                SetLayeredWindowAttributes(g_mainHwnd, 0, 1, LWA_ALPHA);
            }
            LARGE_INTEGER now;
            QueryPerformanceCounter(&now);
            float deltaMs = 0.0f;
            if (g_qpcFrequency.QuadPart > 0 && g_lastTick.QuadPart > 0) {
                deltaMs = (float)(now.QuadPart - g_lastTick.QuadPart) * 1000.0f / (float)g_qpcFrequency.QuadPart;
            }
            g_lastTick = now;

            if (deltaMs < 0.0f) deltaMs = 0.0f;
            if (deltaMs > 64.0f) deltaMs = 64.0f;
            while (deltaMs > 0.0f) {
                float dt = deltaMs > 10.0f ? 10.0f : deltaMs;
                StepPhysics(dt);
                deltaMs -= dt;
            }
            
            DrawOverlayFrameD2D();
            
            if (g_isSettling && !g_wwi.wobblying) {
                HWND mainHwnd; LONG_PTR oldExStyle;
                ReadAndClearWobblyTrackingState(&mainHwnd, &oldExStyle);
                lock.unlock();
                RestoreWindowAfterWobbly(mainHwnd, oldExStyle);
                TeardownWobblyResources();
                return 0;
            }
        }
        return 0;
    }
    return DefWindowProc(hwnd, msg, wp, lp);
}

static void InitializeWobbly(void) {
    if (g_overlayHwnd) return;
    if (!g_classRegistered) return;

    g_screenX = GetSystemMetrics(SM_XVIRTUALSCREEN); 
    g_screenY = GetSystemMetrics(SM_YVIRTUALSCREEN);
    g_screenW = GetSystemMetrics(SM_CXVIRTUALSCREEN); 
    g_screenH = GetSystemMetrics(SM_CYVIRTUALSCREEN);
    
    g_overlayHwnd = CreateWindowExA(
        WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW, 
        "WobblyOverlayWindow", NULL, WS_POPUP, 
        g_screenX, g_screenY, g_screenW, g_screenH, 
        NULL, NULL, g_hInstance, NULL
    );
}

static void OnEnterSizeMove(HWND hwnd) {

    POINT ptHit;
    GetCursorPos(&ptHit);
    LRESULT hit = SendMessageW(hwnd, WM_NCHITTEST, 0, MAKELPARAM(ptHit.x, ptHit.y));
    bool isResizeOp = (hit == HTLEFT || hit == HTRIGHT || hit == HTTOP || hit == HTBOTTOM ||
                        hit == HTTOPLEFT || hit == HTTOPRIGHT || hit == HTBOTTOMLEFT || hit == HTBOTTOMRIGHT);
    bool wobbleAllowed = isResizeOp
        ? g_wobbleWhileResizing.load(std::memory_order_relaxed)
        : g_wobbleWhileDragging.load(std::memory_order_relaxed);

    if (!wobbleAllowed) {

        std::lock_guard<std::recursive_mutex> lock(g_wobblyMutex);
        if (g_mainHwnd == hwnd && g_isSettling) FinishWobblyTracking();
        return;
    }

    if (IsZoomed(hwnd) || IsIconic(hwnd)) return;

    std::unique_lock<std::recursive_mutex> lock(g_wobblyMutex);

    for (int guardAttempt = 0; guardAttempt < 5; guardAttempt++) {
        HWND currentOverlay = g_overlayHwnd;
        if (!currentOverlay || GetWindowThreadProcessId(currentOverlay, nullptr) == GetCurrentThreadId()) break;
        lock.unlock();
        SendMessageTimeoutW(currentOverlay, g_wmDetach, 0, 0, SMTO_NORMAL | SMTO_NOTIMEOUTIFNOTHUNG, 100, NULL);
        lock.lock();
        if (g_overlayHwnd == currentOverlay) {
            g_overlayHwnd = NULL;
        }
    }

    if (g_overlayHwnd && GetWindowThreadProcessId(g_overlayHwnd, nullptr) != GetCurrentThreadId()) return;

    if ((g_isMoving || g_isSettling) && g_mainHwnd != NULL && g_mainHwnd != hwnd) return;

    if (!g_overlayHwnd) {
        InitializeWobbly();
    }

    if (!g_overlayHwnd) return;

    g_mainHwnd = hwnd;

    bool isFreshGrab = !g_isSettling && !g_isMoving;
    if (isFreshGrab) {
        g_oldExStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
        if (g_oldExStyle & WS_EX_LAYERED) {

            g_mainHwnd = NULL;
            return;
        }

        lock.unlock();
        CaptureWindowForWobbly(hwnd);
        lock.lock();

        if (!g_wobblyRT || !g_wobblyBrush) {
            g_mainHwnd = NULL;
            return;
        }

        RECT rcExt; 
        if (DwmGetWindowAttribute(hwnd, DWMWA_EXTENDED_FRAME_BOUNDS, &rcExt, sizeof(rcExt)) != S_OK) GetWindowRect(hwnd, &rcExt);

        InitWobblyInfo((float)rcExt.left, (float)rcExt.top, (float)(rcExt.right - rcExt.left), (float)(rcExt.bottom - rcExt.top), isResizeOp);
    } else {

        RECT rcExt; 
        if (DwmGetWindowAttribute(hwnd, DWMWA_EXTENDED_FRAME_BOUNDS, &rcExt, sizeof(rcExt)) != S_OK) GetWindowRect(hwnd, &rcExt);
        float x = (float)rcExt.left, y = (float)rcExt.top;
        float w = (float)(rcExt.right - rcExt.left), h = (float)(rcExt.bottom - rcExt.top);
        UpdateWobblyOrigin(x, y, w, h);
        for (int i = 0; i < 16; ++i) g_wwi.constraint[i] = false;

        if (isResizeOp) {
            g_wwi.can_wobble_top = false; g_wwi.can_wobble_bottom = false;
            g_wwi.can_wobble_left = false; g_wwi.can_wobble_right = false;
            g_resizeOriginRect.x = x; g_resizeOriginRect.y = y;
            g_resizeOriginRect.width = w; g_resizeOriginRect.height = h;
        } else {
            g_wwi.can_wobble_top = true; g_wwi.can_wobble_bottom = true;
            g_wwi.can_wobble_left = true; g_wwi.can_wobble_right = true;
        }
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
    if (g_qpcFrequency.QuadPart == 0) QueryPerformanceFrequency(&g_qpcFrequency);
    QueryPerformanceCounter(&g_lastTick);

    if (!g_timerPeriodRaised) {
        g_timerPeriodRaised = (timeBeginPeriod(1) == TIMERR_NOERROR);
    }
    
    DrawOverlayFrameD2D();
    ShowWindow(g_overlayHwnd, SW_SHOWNOACTIVATE); 
    SetTimer(g_overlayHwnd, 1, 16, NULL);

    if (isFreshGrab) {
        SetWindowLongPtrW(hwnd, GWL_EXSTYLE, g_oldExStyle | WS_EX_LAYERED); 
        SetLayeredWindowAttributes(hwnd, 0, 1, LWA_ALPHA);
    }
}

static void OnSizingMoving(HWND hwnd, LPRECT r) {
    std::lock_guard<std::recursive_mutex> lock(g_wobblyMutex);
    if (g_isMoving && g_mainHwnd == hwnd) {
        RECT rExt, rWin; 
        if (DwmGetWindowAttribute(hwnd, DWMWA_EXTENDED_FRAME_BOUNDS, &rExt, sizeof(rExt)) != S_OK) GetWindowRect(hwnd, &rExt);
        GetWindowRect(hwnd, &rWin);
        
        int dx = rExt.left - rWin.left;
        int dy = rExt.top - rWin.top;
        float w = (float)(r->right - r->left) - (rWin.right - rExt.right) - dx;
        float h = (float)(r->bottom - r->top) - (rWin.bottom - rExt.bottom) - dy;
        float x = (float)r->left + dx;
        float y = (float)r->top + dy;
        
        UpdateWobblyOrigin(x, y, w, h);

        if (y != g_resizeOriginRect.y) g_wwi.can_wobble_top = true;
        if (x != g_resizeOriginRect.x) g_wwi.can_wobble_left = true;
        if (x + w != g_resizeOriginRect.x + g_resizeOriginRect.width) g_wwi.can_wobble_right = true;
        if (y + h != g_resizeOriginRect.y + g_resizeOriginRect.height) g_wwi.can_wobble_bottom = true;
    }
}

static void OnWindowPosChanged(HWND hwnd, WINDOWPOS* wp) {
    if (hwnd != g_mainHwnd.load(std::memory_order_relaxed)) return;

    std::lock_guard<std::recursive_mutex> lock(g_wobblyMutex);
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
    std::lock_guard<std::recursive_mutex> lock(g_wobblyMutex);
    if (g_isMoving && g_mainHwnd == hwnd) {
        g_isMoving = false; 
        g_isSettling = true;
        
        RECT rcExt; 
        if (DwmGetWindowAttribute(hwnd, DWMWA_EXTENDED_FRAME_BOUNDS, &rcExt, sizeof(rcExt)) != S_OK) GetWindowRect(hwnd, &rcExt);
        float x = (float)rcExt.left, y = (float)rcExt.top;
        float w = (float)(rcExt.right - rcExt.left), h = (float)(rcExt.bottom - rcExt.top);
        UpdateWobblyOrigin(x, y, w, h);

        if (y != g_resizeOriginRect.y) g_wwi.can_wobble_top = true;
        if (x != g_resizeOriginRect.x) g_wwi.can_wobble_left = true;
        if (x + w != g_resizeOriginRect.x + g_resizeOriginRect.width) g_wwi.can_wobble_right = true;
        if (y + h != g_resizeOriginRect.y + g_resizeOriginRect.height) g_wwi.can_wobble_bottom = true;
    }
}

static void TrackSubclassedWindow(HWND hwnd) {
    std::lock_guard<std::mutex> lock(g_subclassedSetMutex);
    g_subclassedWindows.insert(hwnd);
}

static void UntrackSubclassedWindow(HWND hwnd) {
    std::lock_guard<std::mutex> lock(g_subclassedSetMutex);
    g_subclassedWindows.erase(hwnd);
}

LRESULT CALLBACK WobblySubclassProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
    if (msg == g_wmDetach) {

        bool needsRestore = false;
        LONG_PTR oldExStyle = 0;
        {
            std::lock_guard<std::recursive_mutex> lock(g_wobblyMutex);
            if (g_mainHwnd == hwnd && (g_isMoving || g_isSettling)) {
                needsRestore = true;
                oldExStyle = g_oldExStyle;
                g_isMoving = false;
                g_isSettling = false;
                g_mainHwnd = NULL;
            }
        }
        if (needsRestore) {
            RestoreWindowAfterWobbly(hwnd, oldExStyle);
        }
        RemoveWindowSubclass(hwnd, WobblySubclassProc, uIdSubclass);
        UntrackSubclassedWindow(hwnd);
        return DefSubclassProc(hwnd, msg, wParam, lParam);
    }
    else if (msg == WM_ENTERSIZEMOVE) {

        if (!g_isUnloading.load(std::memory_order_relaxed)) {
            OnEnterSizeMove(hwnd);
        }
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
        UntrackSubclassedWindow(hwnd);
    }
    return DefSubclassProc(hwnd, msg, wParam, lParam);
}

static bool IsInternalModClass(HWND hwnd) {
    char className[256];
    if (GetClassNameA(hwnd, className, sizeof(className))) {
        if (strcmp(className, "WobblyOverlayWindow") == 0) {
            return true;
        }
    }
    return false;
}

HWND WINAPI CreateWindowExW_Hook(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam) {
    HWND hwnd = CreateWindowExW_Original(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
    if (hwnd && (dwStyle & WS_CHILD) == 0 && !IsInternalModClass(hwnd)) {
        if (SetWindowSubclass(hwnd, WobblySubclassProc, 0, 0)) TrackSubclassedWindow(hwnd);
    }
    return hwnd;
}

HWND WINAPI CreateWindowExA_Hook(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam) {
    HWND hwnd = CreateWindowExA_Original(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
    if (hwnd && (dwStyle & WS_CHILD) == 0 && !IsInternalModClass(hwnd)) {
        if (SetWindowSubclass(hwnd, WobblySubclassProc, 0, 0)) TrackSubclassedWindow(hwnd);
    }
    return hwnd;
}

static LRESULT CALLBACK CrossThreadHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        CWPSTRUCT* pCwp = (CWPSTRUCT*)lParam;
        if (pCwp->message == g_wmAttach) {
            if (SetWindowSubclass(pCwp->hwnd, WobblySubclassProc, 0, 0)) TrackSubclassedWindow(pCwp->hwnd);
        }
        else if (pCwp->message == g_wmDetach) {
            RemoveWindowSubclass(pCwp->hwnd, WobblySubclassProc, 0);
            UntrackSubclassedWindow(pCwp->hwnd);
        }
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
        SendMessageTimeoutW(hwnd, g_wmDetach, 0, 0, SMTO_NORMAL | SMTO_NOTIMEOUTIFNOTHUNG, 500, NULL);
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


BOOL Wh_ModInit() {
    Wh_Log(L"Wobbly Windows Mod Loading...");
    LoadSettings();
    if (!g_hInstance) {
        GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, 
                           (LPCSTR)&Wh_ModInit, (HMODULE*)&g_hInstance);
    }

    g_isUnloading.store(false, std::memory_order_relaxed);
    g_isMoving = false;
    g_isSettling = false;
    g_mainHwnd = NULL;
    g_overlayHwnd = NULL;
    {
        std::lock_guard<std::mutex> lock(g_subclassedSetMutex);
        g_subclassedWindows.clear();
    }
    g_lastTick.QuadPart = 0;
    g_qpcFrequency.QuadPart = 0;
    g_timerPeriodRaised = false;
    g_wobblyBmpW = 0;
    g_wobblyBmpH = 0;
    g_capturedBmpW = 0;
    g_capturedBmpH = 0;
    
    WNDCLASSA oc = {};
    oc.lpfnWndProc = OverlayProc;
    oc.hInstance = g_hInstance;
    oc.lpszClassName = "WobblyOverlayWindow";
    if (RegisterClassA(&oc)) {
        g_classRegistered = true;
    } else {
        Wh_Log(L"RegisterClassA failed, error %u", GetLastError());
    }

    g_wmAttach = RegisterWindowMessageW(L"WobblyWindows_Attach");
    g_wmDetach = RegisterWindowMessageW(L"WobblyWindows_Detach");

    if (!Wh_SetFunctionHook((void*)CreateWindowExW, (void*)CreateWindowExW_Hook, (void**)&CreateWindowExW_Original)) {
        Wh_Log(L"Failed to hook CreateWindowExW; new windows created via the wide API won't wobble");
    }
    if (!Wh_SetFunctionHook((void*)CreateWindowExA, (void*)CreateWindowExA_Hook, (void**)&CreateWindowExA_Original)) {
        Wh_Log(L"Failed to hook CreateWindowExA; new windows created via the ANSI API won't wobble");
    }
    
    EnumerateAndSubclass(TRUE);

    return TRUE;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}

void Wh_ModBeforeUninit() {
    g_isUnloading.store(true, std::memory_order_relaxed);
}

void Wh_ModUninit() {
    Wh_Log(L"Wobbly Windows Mod Unloading...");

    std::vector<HWND> subclassedSnapshot;
    {
        std::lock_guard<std::mutex> lock(g_subclassedSetMutex);
        subclassedSnapshot.assign(g_subclassedWindows.begin(), g_subclassedWindows.end());
    }
    for (HWND hwnd : subclassedSnapshot) {
        if (IsWindow(hwnd)) {
            SendMessageW(hwnd, g_wmDetach, 0, 0);
        }
    }
    {
        std::lock_guard<std::mutex> lock(g_subclassedSetMutex);
        g_subclassedWindows.clear();
    }

    if (g_overlayHwnd) {
        HWND overlayToClose = g_overlayHwnd;
        if (IsWindow(overlayToClose)) {
            SendMessageW(overlayToClose, WM_CLOSE, 0, 0);
        }
        if (!IsWindow(overlayToClose)) {
            g_overlayHwnd = NULL;
        }
    }

    {
        std::lock_guard<std::recursive_mutex> lock(g_wobblyMutex);
        CleanupWobblyD2D();
        if (g_wobblyRT) { g_wobblyRT->Release(); g_wobblyRT = nullptr; }
        if (g_wobblyMemDC) { DeleteDC(g_wobblyMemDC); g_wobblyMemDC = NULL; }
        if (g_wobblyTargetBmp) { DeleteObject(g_wobblyTargetBmp); g_wobblyTargetBmp = NULL; }
        if (g_capturedBmp) { free(g_capturedBmp); g_capturedBmp = nullptr; g_capturedBmpCap = 0; }
        ReleaseTimerPeriodIfRaised();
    }

    if (g_hInstance) {
        if (g_classRegistered && !UnregisterClassA("WobblyOverlayWindow", g_hInstance)) {
            Wh_Log(L"UnregisterClassA failed, error %u", GetLastError());
        }
        g_classRegistered = false;
        g_hInstance = NULL;
    }

    if (g_d2dFactory) {
        g_d2dFactory->Release();
        g_d2dFactory = nullptr;
    }
}
