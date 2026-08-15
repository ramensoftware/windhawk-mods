// ==WindhawkMod==
// @id            custom-animations-genie
// @name          MacOS Genie Effect minimize/restore 
// @version       1.0
// @author        Potassiumuncher
// @github        https://github.com/Potassiumuncher
// @description   This mod brings the MacOS Genie effect for minimizing and restoring apps into windows
// @include       *
// @compilerOptions -ldwmapi -lgdi32 -ld2d1 -luser32 -lole32 -loleaut32 -luuid
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
![Animation Preview](https://raw.githubusercontent.com/Potassiumuncher/MacOS-Animation-for-windows/99a9a78e9a06c49b282cc8e337854840a9f7fa73/Desktop2026.07.02-19.32.49.05-ezgif.com-video-to-gif-converter.gif)

This mod brings the MacOS Genie effect for minimizing and restoring apps into windows

*Known issue makes the windows with trasnlucent windows grey when minimizing and restoring
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- duration_ms: 450
  $name: Animation Duration (ms)
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
#include <thread>
#include <string>
#include <algorithm>
#include <uiautomation.h>

#ifndef DWMWA_EXTENDED_FRAME_BOUNDS
#define DWMWA_EXTENDED_FRAME_BOUNDS 9
#endif

#ifndef PW_RENDERFULLCONTENT
#define PW_RENDERFULLCONTENT 2
#endif

#define PI 3.14159265f

#define GHOST_X_TILES 20
#define GHOST_Y_TILES 20

#define COMPAT_TASKBAR_HEIGHT 70
#define COMPAT_ICON_SIZE      33

struct Geometry { float x, y, width, height; };

typedef LRESULT (WINAPI *DefWindowProcW_t)(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
DefWindowProcW_t DefWindowProcW_Original;

typedef BOOL (WINAPI *ShowWindow_t)(HWND hWnd, int nCmdShow);
ShowWindow_t ShowWindow_Original;

DWORD WINAPI GhostAnimationThread(LPVOID lpParam);
int GetTaskbarButtonX(HWND hWndApp, int fallbackX, HMONITOR hMon);

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
};

struct SnapCache { HBITMAP hBmp; void* pBits; int w; int h; };
std::unordered_map<HWND, SnapCache> g_SnapshotCache;
std::unordered_map<HWND, RECT> g_RectCache;
std::unordered_map<HWND, int> g_IconPositions;
std::mutex g_CacheMutex;

std::vector<HANDLE> g_animationThreads;
std::mutex g_threadsMutex;
std::vector<HWND> g_activeGhosts;
std::mutex g_ghostMutex;
std::atomic<bool> g_isUnloading{false};
HINSTANCE g_hInstance = NULL;

std::atomic<int> g_durationMs{450};

void LoadSettings() {
    int ms = Wh_GetIntSetting(L"duration_ms");
    if (ms < 50) ms = 50;
    if (ms > 2000) ms = 2000;
    g_durationMs.store(ms, std::memory_order_relaxed);
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

static D2D1_POINT_2F BloatPoint(D2D1_POINT_2F p, D2D1_POINT_2F c, float amount = 0.5f) {
    float dx = p.x - c.x; float dy = p.y - c.y;
    float len = sqrtf(dx*dx + dy*dy);
    if (len < 0.001f) return p;
    return D2D1::Point2F(p.x + (dx/len)*amount, p.y + (dy/len)*amount);
}

HWND FindTaskbarForMonitor(HMONITOR hMon) {
    HWND hMainTray = FindWindowW(L"Shell_TrayWnd", NULL);
    HMONITOR mainMon = MonitorFromWindow(hMainTray, MONITOR_DEFAULTTOPRIMARY);
    if (hMon == mainMon || !hMon) return hMainTray;

    HWND hSecTray = NULL;
    while ((hSecTray = FindWindowExW(NULL, hSecTray, L"Shell_SecondaryTrayWnd", NULL)) != NULL) {
        if (MonitorFromWindow(hSecTray, MONITOR_DEFAULTTONULL) == hMon) {
            return hSecTray;
        }
    }
    return hMainTray;
}

int GetTaskbarButtonX(HWND hWndApp, int fallbackX, HMONITOR hMon) {
    int targetX = fallbackX;
    bool uiaFound = false;
    
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    bool coInit = (hr == S_OK || hr == S_FALSE);

    if (hr == S_OK || hr == S_FALSE || hr == RPC_E_CHANGED_MODE) {
        IUIAutomation* pAutomation = nullptr;
        HRESULT hrUia = CoCreateInstance(__uuidof(CUIAutomation8), NULL, CLSCTX_INPROC_SERVER, __uuidof(IUIAutomation), (void**)&pAutomation);
        if (FAILED(hrUia)) {
            hrUia = CoCreateInstance(__uuidof(CUIAutomation), NULL, CLSCTX_INPROC_SERVER, __uuidof(IUIAutomation), (void**)&pAutomation);
        }

        if (SUCCEEDED(hrUia) && pAutomation) {
            HWND hTray = FindTaskbarForMonitor(hMon);
            if (hTray) {
                IUIAutomationElement* pTrayElement = nullptr;
                if (SUCCEEDED(pAutomation->ElementFromHandle(hTray, &pTrayElement)) && pTrayElement) {
                    
                    WCHAR titleW[512] = {0};
                    GetWindowTextW(hWndApp, titleW, 512);
                    std::wstring titleLower = titleW;
                    std::transform(titleLower.begin(), titleLower.end(), titleLower.begin(), ::towlower);

                    std::wstring procNameLower = L"";
                    WCHAR exePath[MAX_PATH] = {0};
                    if (GetModuleFileNameW(NULL, exePath, MAX_PATH)) {
                        WCHAR* name = wcsrchr(exePath, L'\\');
                        if (name) {
                            procNameLower = (name + 1);
                            size_t dotPos = procNameLower.find(L'.');
                            if (dotPos != std::wstring::npos) procNameLower = procNameLower.substr(0, dotPos);
                            std::transform(procNameLower.begin(), procNameLower.end(), procNameLower.begin(), ::towlower);
                        }
                    }

                    IUIAutomationCondition* pTrueCond = nullptr;
                    pAutomation->CreateTrueCondition(&pTrueCond);

                    IUIAutomationElementArray* pArray = nullptr;
                    if (pTrueCond && SUCCEEDED(pTrayElement->FindAll(TreeScope_Descendants, pTrueCond, &pArray)) && pArray) {
                        int length = 0;
                        pArray->get_Length(&length);
                        
                        MONITORINFO mi = {0};
                        mi.cbSize = sizeof(MONITORINFO);
                        GetMonitorInfoW(hMon, &mi);
                        int monRight = mi.rcMonitor.right;
                        
                        int bestScore = 0;

                        for (int i = 0; i < length; i++) {
                            IUIAutomationElement* pItem = nullptr;
                            if (SUCCEEDED(pArray->GetElement(i, &pItem)) && pItem) {
                                BSTR name;
                                if (SUCCEEDED(pItem->get_CurrentName(&name)) && name) {
                                    std::wstring uiaNameLower = name;
                                    std::transform(uiaNameLower.begin(), uiaNameLower.end(), uiaNameLower.begin(), ::towlower);
                                    
                                    if (!uiaNameLower.empty()) {
                                        int score = 0;
                                        
                                        if (titleLower == uiaNameLower) score += 1000;
                                        if (titleLower.find(uiaNameLower) != std::wstring::npos) score += 500;
                                        if (uiaNameLower.find(titleLower) != std::wstring::npos) score += 500;
                                        
                                        if (!procNameLower.empty() && uiaNameLower.find(procNameLower) != std::wstring::npos) score += 400;
                                        
                                        std::wstring currentWord;
                                        for (wchar_t c : titleLower) {
                                            if (iswalnum(c)) {
                                                currentWord += c;
                                            } else {
                                                if (currentWord.length() >= 4 && uiaNameLower.find(currentWord) != std::wstring::npos) score += 50;
                                                currentWord.clear();
                                            }
                                        }
                                        if (currentWord.length() >= 4 && uiaNameLower.find(currentWord) != std::wstring::npos) score += 50;

                                        if (uiaNameLower.find(L"start") != std::wstring::npos) score -= 500;
                                        if (uiaNameLower.find(L"search") != std::wstring::npos) score -= 500;
                                        if (uiaNameLower.find(L"task view") != std::wstring::npos) score -= 500;
                                        if (uiaNameLower.find(L"widgets") != std::wstring::npos) score -= 500;

                                        if (score > bestScore) {
                                            RECT bRect;
                                            if (SUCCEEDED(pItem->get_CurrentBoundingRectangle(&bRect))) {
                                                if (bRect.right > bRect.left && bRect.left < monRight - 50) {
                                                    bestScore = score;
                                                    targetX = bRect.left + (bRect.right - bRect.left) / 2;
                                                    uiaFound = true;
                                                }
                                            }
                                        }
                                    }
                                    SysFreeString(name);
                                }
                                pItem->Release();
                            }
                        }
                        pArray->Release();
                    }
                    if (pTrueCond) pTrueCond->Release();
                    pTrayElement->Release();
                }
            }
            pAutomation->Release();
        }
        if (coInit) CoUninitialize();
    }

    std::lock_guard<std::mutex> lock(g_CacheMutex);
    if (uiaFound) {
        g_IconPositions[hWndApp] = targetX;
    } else if (g_IconPositions.count(hWndApp)) {
        targetX = g_IconPositions[hWndApp];
    }
    
    return targetX;
}

static void CalculateLampVertexMacOS(float tx, float ty, float p, const Geometry& w, const Geometry& i, float *outX, float *outY) {
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

    float effectX = sinf(((height - y) / fullHeight) * 2.0f * PI + PI) * (w.x + w.width * tx - (i.x + i.width * tx)) / 7.0f * k;

    *outX = w.x + x + offsetX + effectX;
    *outY = w.y + y + offsetY;
}

void ShowGhostSync(GhostAnimData* data) {
    if (!data || !data->hGhost) return;
    
    int vLeft = GetSystemMetrics(SM_XVIRTUALSCREEN);
    int vTop = GetSystemMetrics(SM_YVIRTUALSCREEN);
    int vWidth = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    int vHeight = GetSystemMetrics(SM_CYVIRTUALSCREEN) - 1;

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

void SpawnAnimationThread(GhostAnimData* data) {
    std::lock_guard<std::mutex> lock(g_threadsMutex);
    if (g_isUnloading.load(std::memory_order_relaxed)) {
        delete data;
        return;
    }

    for (auto it = g_animationThreads.begin(); it != g_animationThreads.end(); ) {
        if (WaitForSingleObject(*it, 0) == WAIT_OBJECT_0) {
            CloseHandle(*it);
            it = g_animationThreads.erase(it);
        } else {
            ++it;
        }
    }
    
    HANDLE hThread = CreateThread(NULL, 0, GhostAnimationThread, data, 0, NULL);
    if (hThread) {
        g_animationThreads.push_back(hThread);
    } else {
        delete data;
    }
}

DWORD WINAPI GhostAnimationThread(LPVOID lpParam) {
    GhostAnimData* data = (GhostAnimData*)lpParam;
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);

    int xTiles = GHOST_X_TILES;
    int yTiles = GHOST_Y_TILES;

    int vLeft = GetSystemMetrics(SM_XVIRTUALSCREEN);
    int vTop = GetSystemMetrics(SM_YVIRTUALSCREEN);
    int vWidth = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    int vHeight = GetSystemMetrics(SM_CYVIRTUALSCREEN) - 1;

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
            D2D1_RENDER_TARGET_TYPE_DEFAULT,
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

    MONITORINFO mi = {0};
    mi.cbSize = sizeof(MONITORINFO);
    HMONITOR hMon = MonitorFromRect(&data->targetRect, MONITOR_DEFAULTTONEAREST);
    GetMonitorInfoW(hMon, &mi);

    UINT dpiX = 96, dpiY = 96;
    HMODULE hShcore = LoadLibraryW(L"Shcore.dll");
    if (hShcore) {
        typedef HRESULT (WINAPI *GetDpiForMonitor_t)(HMONITOR, int, UINT*, UINT*);
        auto pGetDpiForMonitor = (GetDpiForMonitor_t)GetProcAddress(hShcore, "GetDpiForMonitor");
        if (pGetDpiForMonitor) pGetDpiForMonitor(hMon, 0, &dpiX, &dpiY);
        FreeLibrary(hShcore);
    }
    float dpiScale = dpiY / 96.0f;
    float scaledTaskbarHeight = COMPAT_TASKBAR_HEIGHT * dpiScale;
    float scaledIconSize = COMPAT_ICON_SIZE * dpiScale;

    float iGeomTaskbarTop = (float)mi.rcMonitor.bottom - scaledTaskbarHeight;
    
    HWND hTrayGeom = FindTaskbarForMonitor(hMon);
    if (hTrayGeom) {
        RECT tr;
        if (GetWindowRect(hTrayGeom, &tr)) {
            int th = tr.bottom - tr.top;
            if (th > 0) {
                iGeomTaskbarTop = (float)tr.top;
            }
        }
    }
    
    float targetYTop = iGeomTaskbarTop;
    
    Geometry iGeom = {
        (float)data->targetDockX - 11.0f,
        targetYTop,
        22.0f,
        scaledIconSize
    };

    const double animDur = (double)data->durationMs;
    LARGE_INTEGER qpcFreq, qpcStart, qpcNow;
    QueryPerformanceFrequency(&qpcFreq);
    QueryPerformanceCounter(&qpcStart);

    std::vector<std::vector<D2D1_POINT_2F>> grid(yTiles + 1, std::vector<D2D1_POINT_2F>(xTiles + 1));
    BOOL firstFrame = TRUE;

    ID2D1PathGeometry* cachedOutlineGeo = nullptr;
    float cachedLeft = -1e9f, cachedRight = -1e9f, cachedTop = -1e9f, cachedBottom = -1e9f;

    if (!rt || !bmpBrush) {
        if (data->isRising) {
            SetLayeredWindowAttributes(data->hRealWnd, 0, 255, LWA_ALPHA);
            if (!(data->originalExStyle & WS_EX_LAYERED)) {
                SetWindowLongPtrW(data->hRealWnd, GWL_EXSTYLE, data->originalExStyle);
            }
        }
        SetDwmTransitions(data->hRealWnd, TRUE);
        
        if (bmpBrush) { bmpBrush->Release(); bmpBrush = nullptr; }
        if (snapshotBmp) { snapshotBmp->Release(); snapshotBmp = nullptr; }
        if (rt) { rt->Release(); rt = nullptr; }
        SelectObject(hMemDC, hOldBmp); DeleteObject(hTargetBmp); DeleteObject(data->hBitmap);
        DeleteDC(hMemDC); ReleaseDC(NULL, hScreenDC);
        
        PostMessage(hGhost, WM_CLOSE, 0, 0);
        delete data;
        return 0;
    }

    for (;;) {
        if (g_isUnloading.load(std::memory_order_relaxed)) break;

        QueryPerformanceCounter(&qpcNow);
        double elapsedMs = (qpcNow.QuadPart - qpcStart.QuadPart) * 1000.0 / qpcFreq.QuadPart;
        BOOL lastFrame = (elapsedMs >= animDur);
        
        float raw_p = (float)fmin(elapsedMs / animDur, 1.0);
        float eased_p = raw_p * raw_p * (3.0f - 2.0f * raw_p);
        float t = data->isRising ? (1.0f - eased_p) : eased_p;

        if (rt && bmpBrush) {
            RECT bindRect = { 0, 0, vWidth, vHeight };
            rt->BindDC(hMemDC, &bindRect);
            rt->BeginDraw();
            rt->Clear(D2D1::ColorF(0, 0, 0, 0));
            rt->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);

            for (int y = 0; y <= yTiles; y++) {
                for (int x = 0; x <= xTiles; x++) {
                    float tx = (float)x / xTiles, ty = (float)y / yTiles;
                    float px, py;
                    CalculateLampVertexMacOS(tx, ty, t, wGeom, iGeom, &px, &py);
                    grid[y][x] = D2D1::Point2F(px - vLeft, py - vTop);
                }
            }

            ID2D1PathGeometry* outlineGeo = nullptr;
            {
                float left   = grid[0][0].x, right  = grid[0][0].x;
                float top    = grid[0][0].y, bottom = grid[0][0].y;
                for (int gy = 0; gy <= yTiles; gy++) {
                    for (int gx = 0; gx <= xTiles; gx++) {
                        left   = fminf(left,   grid[gy][gx].x);
                        right  = fmaxf(right,  grid[gy][gx].x);
                        top    = fminf(top,    grid[gy][gx].y);
                        bottom = fmaxf(bottom, grid[gy][gx].y);
                    }
                }

                bool needRebuild = (cachedOutlineGeo == nullptr)
                    || fabsf(left   - cachedLeft)   > 0.5f
                    || fabsf(right  - cachedRight)  > 0.5f
                    || fabsf(top    - cachedTop)    > 0.5f
                    || fabsf(bottom - cachedBottom) > 0.5f;

                if (needRebuild) {
                    if (cachedOutlineGeo) { cachedOutlineGeo->Release(); cachedOutlineGeo = nullptr; }
                    cachedLeft = left; cachedRight = right; cachedTop = top; cachedBottom = bottom;

                    float w = right - left;
                    float h = bottom - top;
                    float r = fminf(8.0f, fminf(w, h) / 2.0f);
                    const float K = 0.5523f;
                    float L = left, T = top, R = right, B = bottom;

                    g_d2dFactory->CreatePathGeometry(&cachedOutlineGeo);
                    if (cachedOutlineGeo) {
                        ID2D1GeometrySink* sink = nullptr;
                        cachedOutlineGeo->Open(&sink);
                        sink->BeginFigure(D2D1::Point2F(L + r, T), D2D1_FIGURE_BEGIN_FILLED);
                        sink->AddLine(D2D1::Point2F(R - r, T));
                        { D2D1_BEZIER_SEGMENT s;
                          s.point1 = D2D1::Point2F(R - r + K*r, T);
                          s.point2 = D2D1::Point2F(R, T + r - K*r);
                          s.point3 = D2D1::Point2F(R, T + r);
                          sink->AddBezier(s); }
                        sink->AddLine(D2D1::Point2F(R, B - r));
                        { D2D1_BEZIER_SEGMENT s;
                          s.point1 = D2D1::Point2F(R, B - r + K*r);
                          s.point2 = D2D1::Point2F(R - r + K*r, B);
                          s.point3 = D2D1::Point2F(R - r, B);
                          sink->AddBezier(s); }
                        sink->AddLine(D2D1::Point2F(L + r, B));
                        { D2D1_BEZIER_SEGMENT s;
                          s.point1 = D2D1::Point2F(L + r - K*r, B);
                          s.point2 = D2D1::Point2F(L, B - r + K*r);
                          s.point3 = D2D1::Point2F(L, B - r);
                          sink->AddBezier(s); }
                        sink->AddLine(D2D1::Point2F(L, T + r));
                        { D2D1_BEZIER_SEGMENT s;
                          s.point1 = D2D1::Point2F(L, T + r - K*r);
                          s.point2 = D2D1::Point2F(L + r - K*r, T);
                          s.point3 = D2D1::Point2F(L + r, T);
                          sink->AddBezier(s); }
                        sink->EndFigure(D2D1_FIGURE_END_CLOSED);
                        sink->Close();
                        sink->Release();
                    }
                }
                outlineGeo = cachedOutlineGeo;
            }

            if (outlineGeo) {
                ID2D1Layer* layer = nullptr;
                rt->CreateLayer(&layer);
                D2D1_LAYER_PARAMETERS layerParams = D2D1::LayerParameters();
                layerParams.geometricMask = outlineGeo;
                layerParams.maskAntialiasMode = D2D1_ANTIALIAS_MODE_ALIASED;
                rt->PushLayer(&layerParams, layer);

                for (int y = 0; y < yTiles; y++) {
                    for (int x = 0; x < xTiles; x++) {
                        D2D1_POINT_2F p1 = grid[y][x];
                        D2D1_POINT_2F p2 = grid[y][x+1];
                        D2D1_POINT_2F p3 = grid[y+1][x];
                        D2D1_POINT_2F p4 = grid[y+1][x+1];

                        D2D1_POINT_2F c = D2D1::Point2F((p1.x+p2.x+p3.x+p4.x)/4.0f, (p1.y+p2.y+p3.y+p4.y)/4.0f);

                        float quadW = fmaxf(fabsf(p2.x - p1.x), fabsf(p4.x - p3.x));
                        float quadH = fmaxf(fabsf(p3.y - p1.y), fabsf(p4.y - p2.y));
                        float bloatAmt = fminf(quadW, quadH) * 0.04f;
                        bloatAmt = fmaxf(0.15f, fminf(0.35f, bloatAmt));

                        ID2D1PathGeometry* quadGeo = CreateQuadGeo(g_d2dFactory, 
                            BloatPoint(p1, c, bloatAmt), BloatPoint(p2, c, bloatAmt),
                            BloatPoint(p4, c, bloatAmt), BloatPoint(p3, c, bloatAmt));

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
            }
            rt->EndDraw();

            POINT ptDst = { vLeft, vTop };
            SIZE sz = { vWidth, vHeight };
            POINT ptSrc = { 0, 0 };
            BLENDFUNCTION bf = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
            UpdateLayeredWindow(hGhost, NULL, &ptDst, &sz, hMemDC, &ptSrc, 0, &bf, ULW_ALPHA);
        }

        if (firstFrame) {
            ShowWindow(hGhost, SW_SHOWNOACTIVATE);
            firstFrame = FALSE;
        }
        if (lastFrame) break;
    }

    if (cachedOutlineGeo) { cachedOutlineGeo->Release(); cachedOutlineGeo = nullptr; }

    if (data->isRising) {
        SetLayeredWindowAttributes(data->hRealWnd, 0, 255, LWA_ALPHA);
        if (!(data->originalExStyle & WS_EX_LAYERED)) {
            SetWindowLongPtrW(data->hRealWnd, GWL_EXSTYLE, data->originalExStyle);
        }
    }

    SetDwmTransitions(data->hRealWnd, TRUE);

    if (bmpBrush) { bmpBrush->Release(); bmpBrush = nullptr; }
    if (snapshotBmp) { snapshotBmp->Release(); snapshotBmp = nullptr; }
    if (rt) { rt->Release(); rt = nullptr; }

    SelectObject(hMemDC, hOldBmp);
    DeleteObject(hTargetBmp);
    DeleteObject(data->hBitmap);
    DeleteDC(hMemDC);
    ReleaseDC(NULL, hScreenDC);
    
    ShowWindow(hGhost, SW_HIDE);
    PostMessage(hGhost, WM_CLOSE, 0, 0);
    delete data;
    
    return 0;
}

static LRESULT CALLBACK GhostProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    if (msg == WM_CLOSE) {
        DestroyWindow(hwnd);
        return 0;
    } else if (msg == WM_NCDESTROY) {
        std::lock_guard<std::mutex> lock(g_ghostMutex);
        for (auto it = g_activeGhosts.begin(); it != g_activeGhosts.end(); ) {
            if (*it == hwnd) it = g_activeGhosts.erase(it);
            else ++it;
        }
    }
    return DefWindowProcA(hwnd, msg, wp, lp);
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

    MONITORINFO mi = {0};
    mi.cbSize = sizeof(MONITORINFO);
    HMONITOR hMon = MonitorFromRect(&rect, MONITOR_DEFAULTTONEAREST);
    GetMonitorInfoW(hMon, &mi);
    int monWidth = mi.rcMonitor.right - mi.rcMonitor.left;

    DWORD alignVal = 1;
    DWORD dataSize = sizeof(alignVal);
    RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced", L"TaskbarAl", RRF_RT_REG_DWORD, NULL, &alignVal, &dataSize);
    
    int learnedTargetX = (alignVal == 0) ? (mi.rcMonitor.left + 160) : (mi.rcMonitor.left + monWidth / 2);
    
    learnedTargetX = GetTaskbarButtonX(hWnd, learnedTargetX, hMon);

    GhostAnimData* data = new GhostAnimData();
    data->hRealWnd = hWnd;
    data->targetRect = rect;
    data->width = w;
    data->height = h;
    data->isRising = rising;
    data->targetDockX = learnedTargetX;
    data->originalExStyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
    data->durationMs = g_durationMs.load(std::memory_order_relaxed);

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
                SnapCache& c = g_SnapshotCache[hWnd];
                if (c.w == w && c.h == h) {
                    memcpy(data->pBits, c.pBits, w * h * 4);
                    fromCache = TRUE;
                }
                DeleteObject(c.hBmp);
                g_SnapshotCache.erase(hWnd);
            }
        }
        if (!fromCache) {
            DeleteObject(data->hBitmap);
            delete data;
            ReleaseDC(NULL, hScreenDC);
            return nullptr;
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
            DeleteObject(g_SnapshotCache[hWnd].hBmp);
        }
        
        void* pCacheBits = nullptr;
        HBITMAP hCacheBmp = CreateDIBSection(hScreenDC, &bmi, DIB_RGB_COLORS, &pCacheBits, NULL, 0);
        memcpy(pCacheBits, data->pBits, w * h * 4);
        g_SnapshotCache[hWnd] = { hCacheBmp, pCacheBits, w, h };
    }

    ReleaseDC(NULL, hScreenDC);
    
    int vLeft = GetSystemMetrics(SM_XVIRTUALSCREEN);
    int vTop = GetSystemMetrics(SM_YVIRTUALSCREEN);
    int vWidth = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    int vHeight = GetSystemMetrics(SM_CYVIRTUALSCREEN) - 1;

    data->hGhost = CreateWindowExA(
        WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT | WS_EX_TOPMOST | WS_EX_NOACTIVATE,
        "GhostWindowClass", NULL, WS_POPUP,
        vLeft, vTop, vWidth, vHeight,
        NULL, NULL, g_hInstance, NULL
    );

    if (!data->hGhost) {
        DeleteObject(data->hBitmap);
        delete data;
        return nullptr;
    }

    HWND hTray = FindWindowW(L"Shell_TrayWnd", NULL);
    if (hTray) {
        SetWindowPos(data->hGhost, hTray, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    }

    std::lock_guard<std::mutex> lock(g_ghostMutex);
    g_activeGhosts.push_back(data->hGhost);
    
    return data;
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
                SpawnAnimationThread(data);
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
            if (data) SpawnAnimationThread(data);
            return res;
        }
    }
    return ShowWindow_Original(hWnd, nCmdShow);
}

LRESULT WINAPI DefWindowProcW_Hook(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
    if (Msg == WM_DESTROY) {
        std::lock_guard<std::mutex> lock(g_CacheMutex);
        if (g_SnapshotCache.count(hWnd)) {
            DeleteObject(g_SnapshotCache[hWnd].hBmp);
            g_SnapshotCache.erase(hWnd);
        }
        if (g_IconPositions.count(hWnd)) { g_IconPositions.erase(hWnd); }
        if (g_RectCache.count(hWnd)) { g_RectCache.erase(hWnd); }
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
                    SpawnAnimationThread(data);
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
                if (data) SpawnAnimationThread(data);
                return res;
            }
        }
    }
    return DefWindowProcW_Original(hWnd, Msg, wParam, lParam);
}

BOOL Wh_ModInit() {
    LoadSettings();
    if (!g_hInstance) {
        GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, 
                           (LPCSTR)&Wh_ModInit, (HMODULE*)&g_hInstance);
    }

    WNDCLASSA gc = {0};
    gc.lpfnWndProc = GhostProc;
    gc.hInstance = g_hInstance;
    gc.lpszClassName = "GhostWindowClass";
    RegisterClassA(&gc);

    g_isUnloading.store(false, std::memory_order_relaxed);
    
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

    Wh_SetFunctionHook((void*)DefWindowProcW, (void*)DefWindowProcW_Hook, (void**)&DefWindowProcW_Original);
    Wh_SetFunctionHook((void*)ShowWindow, (void*)ShowWindow_Hook, (void**)&ShowWindow_Original);

    return TRUE;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}

void StopAndJoinAllThreads() {
    g_isUnloading.store(true, std::memory_order_relaxed);
    
    std::vector<HANDLE> threadsToJoin;
    {
        std::lock_guard<std::mutex> lock(g_threadsMutex);
        threadsToJoin = g_animationThreads;
        g_animationThreads.clear();
    }
    
    for (HANDLE hThread : threadsToJoin) {
        while (true) {
            DWORD res = MsgWaitForMultipleObjects(1, &hThread, FALSE, INFINITE, QS_ALLINPUT);
            if (res == WAIT_OBJECT_0) {
                break;
            } else if (res == WAIT_OBJECT_0 + 1) {
                MSG msg;
                while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                }
            } else {
                break;
            }
        }
        CloseHandle(hThread);
    }

    std::vector<HWND> ghostsToClose;
    {
        std::lock_guard<std::mutex> lock(g_ghostMutex);
        ghostsToClose = g_activeGhosts;
    }
    for (HWND hwnd : ghostsToClose) {
        if (IsWindow(hwnd)) {
            SendMessageTimeoutW(hwnd, WM_CLOSE, 0, 0, SMTO_ABORTIFHUNG, 200, NULL);
        }
    }
}

void Wh_ModUninit() {
    StopAndJoinAllThreads();

    if (g_hInstance) {
        UnregisterClassA("GhostWindowClass", g_hInstance);
        g_hInstance = NULL;
    }

    if (g_d2dFactory) {
        g_d2dFactory->Release();
        g_d2dFactory = nullptr;
    }
    
    std::lock_guard<std::mutex> lock(g_CacheMutex);
    for (auto& pair : g_SnapshotCache) {
        DeleteObject(pair.second.hBmp);
    }
    g_SnapshotCache.clear();
    g_IconPositions.clear();
    g_RectCache.clear();
}