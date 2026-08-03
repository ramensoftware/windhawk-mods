// ==WindhawkMod==
// @id              windows-app-open-close-animation
// @name            Windows app open/close animation
// @description     Windows app opening and closing animations (this may not work for some apps)
// @version         DEMO 1.0
// @author          Misael Antonio
// @github          https://github.com/
// @include         *
// @compilerOptions -ldwmapi -lgdi32 -ld2d1 -lole32 -loleaut32 -luuid -lshell32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Windows app opening and closing animations (this may not work for some apps)

Version: DEMO 1.0

![Modern genie demo](https://raw.githubusercontent.com/Misaelplayer263/Gifs-do-mod-para-o-windhawk/main/preview.gif.gif)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- duration_ms: 380
  $name: Duracao
- launch_animation: 1
  $name: Animar abertura
- close_animation: 1
  $name: Animar fechamento
- engine: modern
  $name: Estilo
  $options:
  - modern: Modern
  - classic: Classic
- tile_count: 12
  $name: Resolucao
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <dwmapi.h>
#include <d2d1.h>
#include <math.h>
#include <atomic>
#include <unordered_map>
#include <unordered_set>
#include <mutex>
#include <vector>
#include <string>
#include <algorithm>
#include <cwctype>
#include <uiautomation.h>

#ifndef DWMWA_EXTENDED_FRAME_BOUNDS
#define DWMWA_EXTENDED_FRAME_BOUNDS 9
#endif
#ifndef PW_RENDERFULLCONTENT
#define PW_RENDERFULLCONTENT 2
#endif
#ifndef CREATE_WAITABLE_TIMER_HIGH_RESOLUTION
#define CREATE_WAITABLE_TIMER_HIGH_RESOLUTION 0x00000002
#endif

#define PI 3.14159265f
#define TILE_MIN 8
#define TILE_MAX 18
#define TILE_DEFAULT 12

struct Geometry { float x, y, width, height; };

typedef LRESULT (WINAPI *DefWindowProcW_t)(HWND, UINT, WPARAM, LPARAM);
DefWindowProcW_t DefWindowProcW_Original;

typedef BOOL (WINAPI *ShowWindow_t)(HWND, int);
ShowWindow_t ShowWindow_Original;

typedef BOOL (WINAPI *ShowWindowAsync_t)(HWND, int);
ShowWindowAsync_t ShowWindowAsync_Original;

typedef BOOL (WINAPI *SetWindowPos_t)(HWND, HWND, int, int, int, int, UINT);
SetWindowPos_t SetWindowPos_Original;

typedef BOOL (WINAPI *DestroyWindow_t)(HWND);
DestroyWindow_t DestroyWindow_Original;

ID2D1Factory* g_d2dFactory = nullptr;

struct AnimData {
    HWND hWnd;
    HBITMAP hBmp;
    void* pBits;
    RECT srcRect;
    HMONITOR hMon;
    int w, h;
    int dockX;
    BOOL isRising;
    LONG_PTR origEx;
    BOOL cloakHidden;
    int durationMs;
    BOOL isClosing;
};

struct LaunchData {
    HWND hWnd;
    LONG_PTR origEx;
};

std::unordered_map<HWND, int> g_IconCache;
std::unordered_map<std::wstring, int> g_ProcessCache;
std::unordered_set<HWND> g_Seen;
std::unordered_set<HWND> g_Active;
std::mutex g_Mutex;

std::atomic<int>  g_Duration{380};
std::atomic<bool> g_Launch{true};
std::atomic<bool> g_Close{true};
std::atomic<bool> g_Classic{false};
std::atomic<int>  g_Tiles{TILE_DEFAULT};

std::atomic<bool> g_Unload{false};
std::atomic<int>  g_Workers{0};

void LoadSettings() {
    int ms = Wh_GetIntSetting(L"duration_ms");
    if (ms < 150) ms = 150;
    if (ms > 900) ms = 900;
    g_Duration.store(ms, std::memory_order_relaxed);

    g_Launch.store(Wh_GetIntSetting(L"launch_animation") != 0, std::memory_order_relaxed);
    g_Close.store(Wh_GetIntSetting(L"close_animation") != 0, std::memory_order_relaxed);

    PCWSTR eng = Wh_GetStringSetting(L"engine");
    g_Classic.store(wcscmp(eng, L"classic") == 0, std::memory_order_relaxed);
    Wh_FreeStringSetting(eng);

    int t = Wh_GetIntSetting(L"tile_count");
    if (t < TILE_MIN) t = TILE_MIN;
    if (t > TILE_MAX) t = TILE_MAX;
    g_Tiles.store(t, std::memory_order_relaxed);
}

void SetTransitions(HWND h, BOOL enable) {
    BOOL d = !enable;
    DwmSetWindowAttribute(h, DWMWA_TRANSITIONS_FORCEDISABLED, &d, sizeof(d));
}

void SetCloak(HWND h, BOOL c) {
    DwmSetWindowAttribute(h, DWMWA_CLOAK, &c, sizeof(c));
}

void UndoHide(HWND h, LONG_PTR origEx, BOOL cloaked) {
    if (cloaked) SetCloak(h, FALSE);
    else {
        SetLayeredWindowAttributes(h, 0, 255, LWA_ALPHA);
        if (!(origEx & WS_EX_LAYERED))
            SetWindowLongPtrW(h, GWL_EXSTYLE, origEx);
    }
    SetTransitions(h, TRUE);
}

bool ShouldAnimate(HWND h) {
    if (!h) return false;
    if (GetAncestor(h, GA_ROOT) != h) return false;

    WCHAR className[128] = {};
    GetClassNameW(h, className, 128);

    // Rejeita apenas diálogos clássicos
    if (wcscmp(className, L"#32770") == 0)
        return false;

    LONG_PTR style = GetWindowLongPtrW(h, GWL_STYLE);
    LONG_PTR exStyle = GetWindowLongPtrW(h, GWL_EXSTYLE);

    // Rejeita tool windows e diálogos modais clássicos
    if (exStyle & WS_EX_TOOLWINDOW) return false;
    if (exStyle & WS_EX_DLGMODALFRAME) return false;

    // Janelas com dono (quase sempre diálogos)
    if (GetWindow(h, GW_OWNER) != NULL) return false;

    // Tamanho mínimo
    RECT r;
    if (!GetWindowRect(h, &r)) return false;
    if ((r.right - r.left) < 60 || (r.bottom - r.top) < 50) return false;

    // Aceita janelas com caption OU ApplicationFrameWindow (UWP/modern apps)
    bool hasCaption = (style & WS_CAPTION) != 0;
    bool isModernFrame = (wcscmp(className, L"ApplicationFrameWindow") == 0);

    if (!hasCaption && !isModernFrame)
        return false;

    return true;
}

bool IsLaunchWindow(HWND h) {
    return ShouldAnimate(h);
}

HWND FindTray(HMONITOR mon) {
    HWND main = FindWindowW(L"Shell_TrayWnd", NULL);
    if (!mon || MonitorFromWindow(main, MONITOR_DEFAULTTOPRIMARY) == mon)
        return main;
    HWND sec = NULL;
    while ((sec = FindWindowExW(NULL, sec, L"Shell_SecondaryTrayWnd", NULL))) {
        if (MonitorFromWindow(sec, MONITOR_DEFAULTTONULL) == mon)
            return sec;
    }
    return main;
}

int GetDockX(HWND hWnd, HMONITOR mon) {
    // 1. Cache por HWND
    {
        std::lock_guard<std::mutex> lock(g_Mutex);
        auto it = g_IconCache.find(hWnd);
        if (it != g_IconCache.end()) return it->second;
    }

    // 2. Cursor em cima da taskbar = posição do clique (melhor caso)
    POINT pt;
    GetCursorPos(&pt);
    HWND tray = FindTray(mon);
    if (tray) {
        RECT tr;
        if (GetWindowRect(tray, &tr) && PtInRect(&tr, pt)) {
            std::lock_guard<std::mutex> lock(g_Mutex);
            g_IconCache[hWnd] = pt.x;
            return pt.x;
        }
    }

    // 3. Nome do processo
    std::wstring procName, procKey;
    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    if (pid) {
        HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
        if (hProc) {
            WCHAR path[MAX_PATH] = {};
            DWORD len = MAX_PATH;
            if (QueryFullProcessImageNameW(hProc, 0, path, &len)) {
                WCHAR* name = wcsrchr(path, L'\\');
                if (name) {
                    procName = name + 1;
                    size_t dot = procName.find(L'.');
                    if (dot != std::wstring::npos) procName.resize(dot);
                    std::transform(procName.begin(), procName.end(), procName.begin(), ::towlower);
                    procKey = procName;
                    if (mon) procKey += L"_" + std::to_wstring((size_t)mon);
                }
            }
            CloseHandle(hProc);
        }
    }

    // 4. Cache por processo
    {
        std::lock_guard<std::mutex> lock(g_Mutex);
        if (!procKey.empty()) {
            auto it = g_ProcessCache.find(procKey);
            if (it != g_ProcessCache.end()) {
                g_IconCache[hWnd] = it->second;
                return it->second;
            }
        }
    }

    // 5. UI Automation com matching mais forte + espera pelo botão aparecer
    int targetX = 0;
    bool found = false;

    MONITORINFO mi = { sizeof(mi) };
    GetMonitorInfoW(mon, &mi);

    DWORD align = 1;
    DWORD sz = sizeof(align);
    RegGetValueW(HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced",
        L"TaskbarAl", RRF_RT_REG_DWORD, NULL, &align, &sz);

    // Fallback mais neutro (não favorece Explorer)
    targetX = (align == 0)
        ? mi.rcMonitor.left + 200
        : mi.rcMonitor.left + (mi.rcMonitor.right - mi.rcMonitor.left) / 2;

    // Tenta até 3 vezes (importante para apps que acabaram de abrir)
    for (int attempt = 0; attempt < 3 && !found; attempt++) {
        if (attempt > 0) Sleep(60); // espera o botão aparecer na taskbar

        HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
        bool didInit = (hr == S_OK || hr == S_FALSE);

        if (SUCCEEDED(hr) || hr == RPC_E_CHANGED_MODE) {
            IUIAutomation* uia = nullptr;
            if (SUCCEEDED(CoCreateInstance(__uuidof(CUIAutomation), NULL, CLSCTX_INPROC_SERVER,
                                           __uuidof(IUIAutomation), (void**)&uia)) && uia) {

                IUIAutomationElement* trayEl = nullptr;
                if (tray && SUCCEEDED(uia->ElementFromHandle(tray, &trayEl)) && trayEl) {

                    WCHAR title[512] = {};
                    GetWindowTextW(hWnd, title, 512);
                    std::wstring titleLower = title;
                    std::transform(titleLower.begin(), titleLower.end(), titleLower.begin(), ::towlower);

                    std::wstring hint = procName;
                    if (procName == L"chrome")        hint = L"google chrome";
                    else if (procName == L"msedge")   hint = L"microsoft edge";
                    else if (procName == L"firefox")  hint = L"firefox";
                    else if (procName == L"brave")    hint = L"brave";
                    else if (procName == L"code")     hint = L"visual studio code";
                    else if (procName == L"notepad")  hint = L"bloco de notas";
                    else if (procName == L"explorer") hint = L"explorador de arquivos";
                    else if (procName == L"spotify")  hint = L"spotify";
                    else if (procName == L"discord")  hint = L"discord";
                    else if (procName == L"calculator" || procName == L"win32calc") hint = L"calculadora";
                    else if (procName == L"systemsettings") hint = L"configurações";
                    else if (procName == L"control")  hint = L"painel de controle";

                    VARIANT varBtn = { VT_I4 }; varBtn.lVal = UIA_ButtonControlTypeId;
                    VARIANT varList = { VT_I4 }; varList.lVal = UIA_ListItemControlTypeId;

                    IUIAutomationCondition* condBtn = nullptr;
                    IUIAutomationCondition* condList = nullptr;
                    IUIAutomationCondition* condOr = nullptr;

                    uia->CreatePropertyCondition(UIA_ControlTypePropertyId, varBtn, &condBtn);
                    uia->CreatePropertyCondition(UIA_ControlTypePropertyId, varList, &condList);
                    if (condBtn && condList)
                        uia->CreateOrCondition(condBtn, condList, &condOr);

                    IUIAutomationElementArray* arr = nullptr;
                    IUIAutomationCondition* useCond = condOr ? condOr : condBtn;

                    if (useCond && SUCCEEDED(trayEl->FindAll(TreeScope_Descendants, useCond, &arr)) && arr) {
                        int len = 0;
                        arr->get_Length(&len);
                        int bestScore = 0;

                        for (int i = 0; i < len && i < 60; i++) {
                            IUIAutomationElement* item = nullptr;
                            if (SUCCEEDED(arr->GetElement(i, &item)) && item) {
                                BSTR name = nullptr;
                                if (SUCCEEDED(item->get_CurrentName(&name)) && name) {
                                    std::wstring n = name;
                                    std::transform(n.begin(), n.end(), n.begin(), ::towlower);

                                    int score = 0;

                                    if (!titleLower.empty()) {
                                        if (n == titleLower) score += 2500;
                                        else if (n.find(titleLower) != std::wstring::npos) score += 1400;
                                        else if (titleLower.find(n) != std::wstring::npos && n.length() >= 4) score += 900;
                                    }

                                    if (!procName.empty() && n.find(procName) != std::wstring::npos) score += 800;
                                    if (!hint.empty() && n.find(hint) != std::wstring::npos) score += 1600;

                                    // Penaliza fortemente Explorer e botões do sistema
                                    if (n.find(L"explorador de arquivos") != std::wstring::npos ||
                                        n.find(L"file explorer") != std::wstring::npos) score -= 1500;
                                    if (n.find(L"iniciar") != std::wstring::npos || n.find(L"start") != std::wstring::npos) score -= 1200;
                                    if (n.find(L"pesquisar") != std::wstring::npos || n.find(L"search") != std::wstring::npos) score -= 1200;
                                    if (n.find(L"visão de tarefas") != std::wstring::npos || n.find(L"task view") != std::wstring::npos) score -= 1200;
                                    if (n.find(L"widgets") != std::wstring::npos) score -= 1200;

                                    if (score > bestScore) {
                                        RECT br;
                                        if (SUCCEEDED(item->get_CurrentBoundingRectangle(&br)) &&
                                            br.right > br.left &&
                                            br.left > mi.rcMonitor.left + 30 &&
                                            br.left < mi.rcMonitor.right - 50) {
                                            bestScore = score;
                                            targetX = br.left + (br.right - br.left) / 2;
                                            found = true;
                                        }
                                    }
                                    SysFreeString(name);
                                }
                                item->Release();
                            }
                        }
                        arr->Release();
                    }

                    if (condBtn) condBtn->Release();
                    if (condList) condList->Release();
                    if (condOr) condOr->Release();
                    trayEl->Release();
                }
                uia->Release();
            }
            if (didInit) CoUninitialize();
        }
    }

    {
        std::lock_guard<std::mutex> lock(g_Mutex);
        g_IconCache[hWnd] = targetX;
        if (!procKey.empty()) g_ProcessCache[procKey] = targetX;
    }
    return targetX;
}

static void Lamp(float tx, float ty, float p, const Geometry& w, const Geometry& i, float* ox, float* oy) {
    float split = 0.28f;
    float k = (p <= split) ? (p / split) : 1.0f;
    float j = (p > split) ? ((p - split) / (1.0f - split)) : 0.0f;

    float expand = (i.y - w.y - w.height);
    float fullH = (i.y - w.y) - expand * (1.0f - k);
    float height = fullH - j * fullH;

    float y = ty * height;
    float x = tx * i.width + tx * (w.width - i.width) * (1.0f - j) * (1.0f - ty)
            + tx * (w.width - i.width) * (1.0f - k) * ty;

    float offX = (i.x - w.x) * (y / (fullH + 0.1f)) * k + (i.x - w.x) * j;
    float offY = i.y - w.y - height - expand * (1.0f - k);
    float fx = sinf(((height - y) / fullH) * 2.0f * PI + PI)
             * (w.x + w.width * tx - (i.x + i.width * tx)) / 8.0f * k;

    *ox = w.x + x + offX + fx;
    *oy = w.y + y + offY;
}

static ID2D1PathGeometry* MakeQuad(ID2D1Factory* f, D2D1_POINT_2F a, D2D1_POINT_2F b, D2D1_POINT_2F c, D2D1_POINT_2F d) {
    ID2D1PathGeometry* g = nullptr;
    f->CreatePathGeometry(&g);
    if (!g) return nullptr;
    ID2D1GeometrySink* s = nullptr;
    g->Open(&s);
    s->BeginFigure(a, D2D1_FIGURE_BEGIN_FILLED);
    s->AddLine(b); s->AddLine(c); s->AddLine(d);
    s->EndFigure(D2D1_FIGURE_END_CLOSED);
    s->Close();
    s->Release();
    return g;
}

// ===================== MODERN ENGINE =====================
DWORD WINAPI AnimThreadModern(LPVOID param) {
    AnimData* d = (AnimData*)param;
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL);

    int pad = 80;
    int left   = d->srcRect.left - pad;
    int top    = d->srcRect.top  - pad;
    int right  = std::max(static_cast<int>(d->srcRect.right), d->dockX + 40) + pad;
    int bottom = d->srcRect.bottom + 120;

    MONITORINFO mi = { sizeof(mi) };
    GetMonitorInfoW(d->hMon, &mi);
    left   = std::max(left,   static_cast<int>(mi.rcMonitor.left));
    top    = std::max(top,    static_cast<int>(mi.rcMonitor.top));
    right  = std::min(right,  static_cast<int>(mi.rcMonitor.right));
    bottom = std::min(bottom, static_cast<int>(mi.rcMonitor.bottom));

    int gw = right - left;
    int gh = bottom - top;
    if (gw < 64) gw = 64;
    if (gh < 64) gh = 64;

    HDC screen = GetDC(NULL);
    HDC mem = CreateCompatibleDC(screen);

    BITMAPINFO bmi = {{0}};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = gw;
    bmi.bmiHeader.biHeight = -gh;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void* bits = nullptr;
    HBITMAP canvas = CreateDIBSection(screen, &bmi, DIB_RGB_COLORS, &bits, NULL, 0);
    HBITMAP old = (HBITMAP)SelectObject(mem, canvas);

    HWND ghost = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE,
        L"STATIC", NULL, WS_POPUP,
        left, top, gw, gh, NULL, NULL, NULL, NULL);

    ID2D1DCRenderTarget* rt = nullptr;
    ID2D1Bitmap* bmp = nullptr;
    ID2D1BitmapBrush* brush = nullptr;

    if (g_d2dFactory) {
        D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
            D2D1_RENDER_TARGET_TYPE_DEFAULT,
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
            0, 0, D2D1_RENDER_TARGET_USAGE_GDI_COMPATIBLE, D2D1_FEATURE_LEVEL_DEFAULT);
        g_d2dFactory->CreateDCRenderTarget(&props, &rt);
        if (rt) {
            D2D1_BITMAP_PROPERTIES bp = D2D1::BitmapProperties(
                D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED));
            rt->CreateBitmap(D2D1::SizeU(d->w, d->h), d->pBits, d->w * 4, bp, &bmp);
            if (bmp) {
                D2D1_BITMAP_BRUSH_PROPERTIES brp = D2D1::BitmapBrushProperties(
                    D2D1_EXTEND_MODE_CLAMP, D2D1_EXTEND_MODE_CLAMP, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
                rt->CreateBitmapBrush(bmp, &brp, nullptr, &brush);
            }
        }
    }

    bool ok = (rt && brush);
    Geometry wGeom = { (float)d->srcRect.left, (float)d->srcRect.top, (float)d->w, (float)d->h };

    float trayTop = (float)mi.rcMonitor.bottom - 48.0f;
    HWND tray = FindTray(d->hMon);
    if (tray) {
        RECT tr;
        if (GetWindowRect(tray, &tr) && tr.bottom > tr.top)
            trayTop = (float)tr.top;
    }
    Geometry iGeom = { (float)d->dockX - 12.0f, trayTop, 24.0f, 32.0f };

    const double dur = (double)d->durationMs;
    LARGE_INTEGER freq, start, now;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&start);

    int tiles = g_Tiles.load(std::memory_order_relaxed);
    std::vector<std::vector<D2D1_POINT_2F>> grid(tiles + 1, std::vector<D2D1_POINT_2F>(tiles + 1));

    const double interval = 13.3; // ~75 fps
    HANDLE timer = CreateWaitableTimerExW(nullptr, nullptr, CREATE_WAITABLE_TIMER_HIGH_RESOLUTION,
                                          TIMER_MODIFY_STATE | SYNCHRONIZE);
    if (!timer)
        timer = CreateWaitableTimerExW(nullptr, nullptr, 0, TIMER_MODIFY_STATE | SYNCHRONIZE);

    BOOL first = TRUE;

    if (ok) {
        for (;;) {
            QueryPerformanceCounter(&now);
            LARGE_INTEGER frameStart = now;
            double elapsed = (now.QuadPart - start.QuadPart) * 1000.0 / freq.QuadPart;
            BOOL last = (elapsed >= dur);

            float raw = (float)fmin(elapsed / dur, 1.0);
            float ease = raw * raw * (3.0f - 2.0f * raw);
            float t = d->isRising ? (1.0f - ease) : ease;

            RECT bind = { 0, 0, gw, gh };
            rt->BindDC(mem, &bind);
            rt->BeginDraw();
            rt->Clear(D2D1::ColorF(0, 0, 0, 0));
            rt->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);

            for (int y = 0; y <= tiles; y++) {
                for (int x = 0; x <= tiles; x++) {
                    float tx = (float)x / tiles, ty = (float)y / tiles;
                    float px, py;
                    Lamp(tx, ty, t, wGeom, iGeom, &px, &py);
                    grid[y][x] = D2D1::Point2F(px - left, py - top);
                }
            }

            for (int y = 0; y < tiles; y++) {
                for (int x = 0; x < tiles; x++) {
                    D2D1_POINT_2F p1 = grid[y][x], p2 = grid[y][x+1];
                    D2D1_POINT_2F p3 = grid[y+1][x], p4 = grid[y+1][x+1];
                    D2D1_POINT_2F c = {
                        (p1.x + p2.x + p3.x + p4.x) * 0.25f,
                        (p1.y + p2.y + p3.y + p4.y) * 0.25f
                    };

                    auto blo = [](D2D1_POINT_2F p, D2D1_POINT_2F cen) {
                        float dx = p.x - cen.x, dy = p.y - cen.y;
                        float len = sqrtf(dx*dx + dy*dy);
                        if (len < 0.001f) return p;
                        return D2D1::Point2F(p.x + dx/len * 0.3f, p.y + dy/len * 0.3f);
                    };

                    ID2D1PathGeometry* quad = MakeQuad(g_d2dFactory,
                        blo(p1, c), blo(p2, c), blo(p4, c), blo(p3, c));
                    if (quad) {
                        float sx = ((float)x / tiles) * wGeom.width;
                        float sy = ((float)y / tiles) * wGeom.height;
                        float sw = wGeom.width / tiles;
                        float sh = wGeom.height / tiles;
                        float m11 = (p2.x - p1.x) / sw, m12 = (p2.y - p1.y) / sw;
                        float m21 = (p3.x - p1.x) / sh, m22 = (p3.y - p1.y) / sh;
                        float m31 = p1.x - sx * m11 - sy * m21;
                        float m32 = p1.y - sx * m12 - sy * m22;
                        brush->SetTransform(D2D1::Matrix3x2F(m11, m12, m21, m22, m31, m32));
                        rt->FillGeometry(quad, brush);
                        quad->Release();
                    }
                }
            }
            rt->EndDraw();

            POINT dst = { left, top };
            SIZE sz = { gw, gh };
            POINT src = { 0, 0 };
            BLENDFUNCTION bf = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
            UpdateLayeredWindow(ghost, screen, &dst, &sz, mem, &src, 0, &bf, ULW_ALPHA);

            if (first) {
                ShowWindow(ghost, SW_SHOWNOACTIVATE);
                first = FALSE;
            }

            if (last || g_Unload.load(std::memory_order_relaxed)) break;

            if (timer) {
                LARGE_INTEGER after;
                QueryPerformanceCounter(&after);
                double spent = (after.QuadPart - frameStart.QuadPart) * 1000.0 / freq.QuadPart;
                double wait = interval - spent;
                if (wait > 1.0) {
                    LARGE_INTEGER due;
                    due.QuadPart = -(LONGLONG)(wait * 10000.0);
                    SetWaitableTimer(timer, &due, 0, nullptr, nullptr, FALSE);
                    WaitForSingleObject(timer, 30);
                } else {
                    SwitchToThread();
                }
            } else {
                Sleep(4);
            }
        }
    }

    if (timer) CloseHandle(timer);

    if (d->isRising) {
        if (d->cloakHidden) SetCloak(d->hWnd, FALSE);
        else {
            SetLayeredWindowAttributes(d->hWnd, 0, 255, LWA_ALPHA);
            if (!(d->origEx & WS_EX_LAYERED))
                SetWindowLongPtrW(d->hWnd, GWL_EXSTYLE, d->origEx);
        }
        SetTransitions(d->hWnd, TRUE);
    }

    if (brush) brush->Release();
    if (bmp) bmp->Release();
    if (rt) rt->Release();

    SelectObject(mem, old);
    DeleteObject(canvas);
    DeleteObject(d->hBmp);
    DeleteDC(mem);
    ReleaseDC(NULL, screen);
    DestroyWindow(ghost);

    // Fechamento real no final da animação
    if (d->isClosing && !g_Unload.load(std::memory_order_relaxed) && IsWindow(d->hWnd)) {
        SetPropW(d->hWnd, L"GenieCloseBypass", (HANDLE)1);
        PostMessageW(d->hWnd, WM_CLOSE, 0, 0);
        Sleep(50);
        if (IsWindow(d->hWnd))
            DestroyWindow_Original(d->hWnd);
    }

    {
        std::lock_guard<std::mutex> lock(g_Mutex);
        g_Active.erase(d->hWnd);
    }
    delete d;
    g_Workers.fetch_sub(1, std::memory_order_release);
    return 0;
}

// ===================== CLASSIC (usa modern por enquanto) =====================
DWORD WINAPI AnimThreadClassic(LPVOID param) {
    return AnimThreadModern(param);
}

// ===================== CAPTURE =====================
bool CaptureWindow(HWND hWnd, HBITMAP* outBmp, void** outBits, int* outW, int* outH, RECT* outRect) {
    RECT winRect;
    if (!GetWindowRect(hWnd, &winRect)) return false;

    RECT rect = winRect;
    RECT ext;
    if (SUCCEEDED(DwmGetWindowAttribute(hWnd, DWMWA_EXTENDED_FRAME_BOUNDS, &ext, sizeof(ext))))
        rect = ext;

    int w = rect.right - rect.left;
    int h = rect.bottom - rect.top;
    if (w < 32 || h < 32) return false;

    int ox = rect.left - winRect.left;
    int oy = rect.top  - winRect.top;
    int rawW = winRect.right - winRect.left;
    int rawH = winRect.bottom - winRect.top;

    HDC screen = GetDC(NULL);
    BITMAPINFO bmi = {{0}};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = w;
    bmi.bmiHeader.biHeight = -h;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void* bits = nullptr;
    HBITMAP bmp = CreateDIBSection(screen, &bmi, DIB_RGB_COLORS, &bits, NULL, 0);
    if (!bmp || !bits) {
        if (bmp) DeleteObject(bmp);
        ReleaseDC(NULL, screen);
        return false;
    }

    HDC temp = CreateCompatibleDC(screen);
    BITMAPINFO tbmi = bmi;
    tbmi.bmiHeader.biWidth = rawW;
    tbmi.bmiHeader.biHeight = -rawH;
    void* tbits = nullptr;
    HBITMAP tbmp = CreateDIBSection(screen, &tbmi, DIB_RGB_COLORS, &tbits, NULL, 0);
    if (tbmp && tbits) {
        HBITMAP old = (HBITMAP)SelectObject(temp, tbmp);
        PrintWindow(hWnd, temp, PW_RENDERFULLCONTENT);
        GdiFlush();
        DWORD* src = (DWORD*)tbits;
        DWORD* dst = (DWORD*)bits;
        for (int y = 0; y < h; y++) {
            for (int x = 0; x < w; x++) {
                int sx = x + ox, sy = y + oy;
                if (sx >= 0 && sx < rawW && sy >= 0 && sy < rawH) {
                    DWORD p = src[sy * rawW + sx];
                    BYTE a = (BYTE)((p >> 24) & 0xFF);
                    dst[y * w + x] = (a == 0) ? 0 : p;
                } else {
                    dst[y * w + x] = 0;
                }
            }
        }
        SelectObject(temp, old);
        DeleteObject(tbmp);
    }
    DeleteDC(temp);
    ReleaseDC(NULL, screen);

    *outBmp = bmp;
    *outBits = bits;
    *outW = w;
    *outH = h;
    *outRect = rect;
    return true;
}

bool StartAnim(HWND hWnd, BOOL rising, LONG_PTR origEx, BOOL cloaked, BOOL closing,
               HBITMAP preBmp = NULL, void* preBits = NULL, int preW = 0, int preH = 0, RECT preRect = {})
{
    HBITMAP bmp = preBmp;
    void* bits = preBits;
    int w = preW, h = preH;
    RECT rect = preRect;

    if (!bmp) {
        if (!CaptureWindow(hWnd, &bmp, &bits, &w, &h, &rect)) {
            if (rising) UndoHide(hWnd, origEx, cloaked);
            return false;
        }
    }

    bool blocked = g_Unload.load(std::memory_order_relaxed);
    if (!blocked) {
        std::lock_guard<std::mutex> lock(g_Mutex);
        blocked = !g_Active.insert(hWnd).second;
        if (!blocked) g_Seen.insert(hWnd);
    }
    if (blocked) {
        if (!preBmp) DeleteObject(bmp);
        if (rising) UndoHide(hWnd, origEx, cloaked);
        return false;
    }

    HMONITOR mon = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
    int dockX = GetDockX(hWnd, mon);

    AnimData* d = new AnimData();
    d->hWnd = hWnd;
    d->hBmp = bmp;
    d->pBits = bits;
    d->srcRect = rect;
    d->hMon = mon;
    d->w = w;
    d->h = h;
    d->dockX = dockX;
    d->isRising = rising;
    d->origEx = origEx;
    d->cloakHidden = cloaked;
    d->durationMs = g_Duration.load(std::memory_order_relaxed);
    d->isClosing = closing;

    g_Workers.fetch_add(1, std::memory_order_relaxed);

    LPTHREAD_START_ROUTINE proc = g_Classic.load(std::memory_order_relaxed)
        ? AnimThreadClassic : AnimThreadModern;

    HANDLE th = CreateThread(NULL, 0, proc, d, 0, NULL);
    if (th) {
        CloseHandle(th);
        return true;
    }

    g_Workers.fetch_sub(1, std::memory_order_release);
    if (!preBmp) DeleteObject(bmp);
    if (rising) UndoHide(hWnd, origEx, cloaked);
    delete d;
    std::lock_guard<std::mutex> lock(g_Mutex);
    g_Active.erase(hWnd);
    return false;
}

// ===================== LAUNCH =====================
DWORD WINAPI LaunchThread(LPVOID p) {
    LaunchData* ld = (LaunchData*)p;
    HWND h = ld->hWnd;
    LONG_PTR ex = ld->origEx;
    delete ld;

    Sleep(80);
    for (int i = 0; i < 20; i++) {
        if (!IsWindow(h) || g_Unload.load(std::memory_order_relaxed)) break;
        UINT c = 0;
        if (FAILED(DwmGetWindowAttribute(h, DWMWA_CLOAKED, &c, sizeof(c))) || !c) break;
        Sleep(30);
    }

    if (g_Unload.load(std::memory_order_relaxed) || !IsWindow(h) || IsIconic(h) || !IsWindowVisible(h)) {
        if (IsWindow(h)) UndoHide(h, ex, FALSE);
        g_Workers.fetch_sub(1, std::memory_order_release);
        return 0;
    }

    StartAnim(h, TRUE, ex, FALSE, FALSE);
    g_Workers.fetch_sub(1, std::memory_order_release);
    return 0;
}

bool LaunchPrepare(HWND h, int cmd, LONG_PTR* outEx) {
    if (g_Unload.load(std::memory_order_relaxed) || !g_Launch.load(std::memory_order_relaxed)) return false;
    if (cmd != SW_SHOW && cmd != SW_SHOWNORMAL && cmd != SW_SHOWDEFAULT && cmd != SW_SHOWMAXIMIZED) return false;
    if (IsWindowVisible(h) || IsIconic(h) || !IsLaunchWindow(h)) return false;
    {
        std::lock_guard<std::mutex> lock(g_Mutex);
        if (!g_Seen.insert(h).second) return false;
    }
    SetTransitions(h, FALSE);
    LONG_PTR ex = GetWindowLongPtrW(h, GWL_EXSTYLE);
    *outEx = ex;
    SetWindowLongPtrW(h, GWL_EXSTYLE, ex | WS_EX_LAYERED);
    SetLayeredWindowAttributes(h, 0, 0, LWA_ALPHA);
    return true;
}

void LaunchCommit(HWND h, LONG_PTR ex) {
    LaunchData* ld = new LaunchData{ h, ex };
    g_Workers.fetch_add(1, std::memory_order_relaxed);
    HANDLE th = CreateThread(NULL, 0, LaunchThread, ld, 0, NULL);
    if (th) CloseHandle(th);
    else {
        g_Workers.fetch_sub(1, std::memory_order_release);
        delete ld;
        UndoHide(h, ex, FALSE);
    }
}

// ===================== HOOKS =====================
BOOL WINAPI ShowWindow_Hook(HWND h, int cmd) {
    LONG_PTR ex;
    if (LaunchPrepare(h, cmd, &ex)) {
        BOOL r = ShowWindow_Original(h, cmd);
        LaunchCommit(h, ex);
        return r;
    }
    return ShowWindow_Original(h, cmd);
}

BOOL WINAPI ShowWindowAsync_Hook(HWND h, int cmd) {
    LONG_PTR ex;
    if (LaunchPrepare(h, cmd, &ex)) {
        BOOL r = ShowWindowAsync_Original(h, cmd);
        LaunchCommit(h, ex);
        return r;
    }
    return ShowWindowAsync_Original(h, cmd);
}

BOOL WINAPI SetWindowPos_Hook(HWND h, HWND after, int x, int y, int cx, int cy, UINT flags) {
    if (flags & SWP_SHOWWINDOW) {
        LONG_PTR ex;
        if (LaunchPrepare(h, SW_SHOW, &ex)) {
            BOOL r = SetWindowPos_Original(h, after, x, y, cx, cy, flags);
            LaunchCommit(h, ex);
            return r;
        }
    }
    return SetWindowPos_Original(h, after, x, y, cx, cy, flags);
}

BOOL WINAPI DestroyWindow_Hook(HWND h) {
    if (g_Unload.load(std::memory_order_relaxed))
        return DestroyWindow_Original(h);

    if (GetPropW(h, L"GenieCloseBypass"))
        return DestroyWindow_Original(h);

    // === Exceção especial para o Bloco de Notas ===
    // Se for notepad.exe, NÃO anima no DestroyWindow
    // (só anima quando vier de SC_CLOSE = botão X da janela)
    {
        DWORD pid = 0;
        GetWindowThreadProcessId(h, &pid);
        if (pid) {
            HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
            if (hProc) {
                WCHAR path[MAX_PATH] = {};
                DWORD len = MAX_PATH;
                if (QueryFullProcessImageNameW(hProc, 0, path, &len)) {
                    WCHAR* name = wcsrchr(path, L'\\');
                    if (name) {
                        std::wstring exe = name + 1;
                        std::transform(exe.begin(), exe.end(), exe.begin(), ::towlower);
                        if (exe == L"notepad.exe") {
                            CloseHandle(hProc);
                            return DestroyWindow_Original(h); // fecha normal, sem animação
                        }
                    }
                }
                CloseHandle(hProc);
            }
        }
    }
    // ==============================================

    if (g_Close.load(std::memory_order_relaxed) &&
        ShouldAnimate(h) &&
        IsWindowVisible(h) &&
        !IsIconic(h))
    {
        HBITMAP bmp = NULL;
        void* bits = NULL;
        int w = 0, hgt = 0;
        RECT rect = {};

        if (CaptureWindow(h, &bmp, &bits, &w, &hgt, &rect)) {
            SetTransitions(h, FALSE);
            SetCloak(h, TRUE);

            if (StartAnim(h, FALSE, GetWindowLongPtrW(h, GWL_EXSTYLE), TRUE, TRUE,
                          bmp, bits, w, hgt, rect))
            {
                return TRUE;
            }

            DeleteObject(bmp);
            SetCloak(h, FALSE);
            SetTransitions(h, TRUE);
        }
    }

    return DestroyWindow_Original(h);
}

LRESULT WINAPI DefWindowProcW_Hook(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_SYSCOMMAND) {
        UINT cmd = (UINT)(wParam & 0xFFF0);
        if (cmd == SC_CLOSE &&
            g_Close.load(std::memory_order_relaxed) &&
            ShouldAnimate(hWnd) &&
            IsWindowVisible(hWnd) &&
            !IsIconic(hWnd) &&
            !GetPropW(hWnd, L"GenieCloseBypass"))
        {
            HBITMAP bmp = NULL;
            void* bits = NULL;
            int w = 0, h = 0;
            RECT rect = {};
            if (CaptureWindow(hWnd, &bmp, &bits, &w, &h, &rect)) {
                SetTransitions(hWnd, FALSE);
                SetCloak(hWnd, TRUE);

                if (StartAnim(hWnd, FALSE, GetWindowLongPtrW(hWnd, GWL_EXSTYLE), TRUE, TRUE,
                              bmp, bits, w, h, rect))
                {
                    return 0;
                }

                DeleteObject(bmp);
                SetCloak(hWnd, FALSE);
                SetTransitions(hWnd, TRUE);
            }
        }
    }

    if (msg == WM_DESTROY) {
        std::lock_guard<std::mutex> lock(g_Mutex);
        g_IconCache.erase(hWnd);
        g_Seen.erase(hWnd);
        g_Active.erase(hWnd);
    }

    return DefWindowProcW_Original(hWnd, msg, wParam, lParam);
}

// ===================== INIT =====================
BOOL Wh_ModInit() {
    LoadSettings();

    if (FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, __uuidof(ID2D1Factory),
                                 reinterpret_cast<void**>(&g_d2dFactory))))
        g_d2dFactory = nullptr;

    Wh_SetFunctionHook((void*)DefWindowProcW, (void*)DefWindowProcW_Hook, (void**)&DefWindowProcW_Original);
    Wh_SetFunctionHook((void*)ShowWindow, (void*)ShowWindow_Hook, (void**)&ShowWindow_Original);
    Wh_SetFunctionHook((void*)ShowWindowAsync, (void*)ShowWindowAsync_Hook, (void**)&ShowWindowAsync_Original);
    Wh_SetFunctionHook((void*)SetWindowPos, (void*)SetWindowPos_Hook, (void**)&SetWindowPos_Original);
    Wh_SetFunctionHook((void*)DestroyWindow, (void*)DestroyWindow_Hook, (void**)&DestroyWindow_Original);

    return TRUE;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}

void Wh_ModBeforeUninit() {
    g_Unload.store(true, std::memory_order_relaxed);
    for (int i = 0; i < 150 && g_Workers.load(std::memory_order_acquire) > 0; ++i)
        Sleep(10);
}

void Wh_ModUninit() {
    if (g_d2dFactory) {
        g_d2dFactory->Release();
        g_d2dFactory = nullptr;
    }
    std::lock_guard<std::mutex> lock(g_Mutex);
    g_IconCache.clear();
    g_ProcessCache.clear();
    g_Seen.clear();
    g_Active.clear();
}