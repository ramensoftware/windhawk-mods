// ==WindhawkMod==
// @id          click-ripple
// @name        Click Ripple
// @description Expanding ripple on click: green for left click, red for right click.
// @version     1.0
// @author      franz
// @github      https://github.com/blacknnegro
// @include     explorer.exe
// @compilerOptions -lgdiplus -luser32 -lgdi32 -lwinmm
// @license     MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Click Ripple

An elegant expanding ripple with a soft halo, sinusoidal easing and
anti-aliasing, triggered by mouse clicks:

- **Left click** → **green** ripple.
- **Right click** → **red** ripple.

## Features
- ~125 FPS while ripples are active.
- Ease-in-out sinusoidal easing (smooth start and stop).
- Soft 3-layer halo + inner flash.
- Only injected into explorer.exe (requires a message loop for the hook).
- Multi-monitor aware.

## Limitations
- Does not capture clicks on elevated windows (UAC).
- Does not appear over exclusive fullscreen games.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- ColorLeft: "00FF00"
  $name: Left click color (hex RRGGBB)
  $description: Ripple color for left clicks, without the # prefix.
- ColorRight: "FF0000"
  $name: Right click color (hex RRGGBB)
  $description: Ripple color for right clicks, without the # prefix.
- MaxRadius: 130
  $name: Maximum radius (px)
  $description: Maximum distance the ripple reaches before fading out.
- DurationMs: 1100
  $name: Duration (ms)
  $description: Total animation duration in milliseconds.
- Thickness: 2
  $name: Stroke thickness (px)
  $description: Base line thickness (it thins out as the ripple expands).
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <gdiplus.h>
#include <mmsystem.h>
#include <mutex>
#include <vector>
#include <atomic>
#include <cmath>
#include <windhawk_api.h>

#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "winmm.lib")

using namespace Gdiplus;

// ---------- Global state ----------
struct Ripple {
    float x, y;
    DWORD startTime;
    float maxRadius;
    float duration;
    float thickness;
    Color color;
};

Color   g_colorLeft  = Color(255,   0, 255,   0); // green
Color   g_colorRight = Color(255, 255,   0,   0); // red
float   g_maxRadius  = 130.0f;
float   g_duration   = 1100.0f;
float   g_thickness  = 2.0f;

std::mutex          g_rippleMutex;
std::vector<Ripple> g_ripples;
bool                g_wasActive = false;

HANDLE              g_thread        = nullptr;
DWORD               g_threadId      = 0;
HANDLE              g_readyEvent    = nullptr;
std::atomic<bool>   g_stopThread    { false };
HANDLE              g_instanceMutex = nullptr;

HWND                g_overlayWnd    = nullptr;
HHOOK               g_mouseHook     = nullptr;
ULONG_PTR           g_gdiplusToken  = 0;

HDC                 g_memDC         = nullptr;
HBITMAP             g_dib           = nullptr;
void*               g_dibBits       = nullptr;
int                 g_vx = 0, g_vy = 0, g_vw = 0, g_vh = 0;

// ---------- Helpers ----------
static Color ParseHexColor(PCWSTR str, Color fallback) {
    if (!str || !*str) return fallback;
    unsigned long v = wcstoul(str, nullptr, 16);
    return Color(255,
                 (BYTE)((v >> 16) & 0xFF),
                 (BYTE)((v >> 8)  & 0xFF),
                 (BYTE)( v        & 0xFF));
}

// ---------- Settings ----------
void LoadSettings() {
    PCWSTR cL = Wh_GetStringSetting(L"ColorLeft");
    g_colorLeft = ParseHexColor(cL, Color(255, 0, 255, 0));
    if (cL) Wh_FreeStringSetting(cL);

    PCWSTR cR = Wh_GetStringSetting(L"ColorRight");
    g_colorRight = ParseHexColor(cR, Color(255, 255, 0, 0));
    if (cR) Wh_FreeStringSetting(cR);

    g_maxRadius = (float)Wh_GetIntSetting(L"MaxRadius", 130);
    g_duration  = (float)Wh_GetIntSetting(L"DurationMs", 1100);
    g_thickness = (float)Wh_GetIntSetting(L"Thickness", 2);
    if (g_duration  < 50)  g_duration  = 50;
    if (g_maxRadius < 1)   g_maxRadius = 1;
    if (g_thickness < 1)   g_thickness = 1;
}

// ---------- DIB ----------
bool InitDib() {
    g_vx = GetSystemMetrics(SM_XVIRTUALSCREEN);
    g_vy = GetSystemMetrics(SM_YVIRTUALSCREEN);
    g_vw = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    g_vh = GetSystemMetrics(SM_CYVIRTUALSCREEN);
    if (g_vw <= 0 || g_vh <= 0) return false;

    HDC screenDC = GetDC(nullptr);
    if (!screenDC) return false;

    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth       = g_vw;
    bmi.bmiHeader.biHeight      = -g_vh;
    bmi.bmiHeader.biPlanes      = 1;
    bmi.bmiHeader.biBitCount    = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    g_dib   = CreateDIBSection(screenDC, &bmi, DIB_RGB_COLORS, &g_dibBits, nullptr, 0);
    g_memDC = CreateCompatibleDC(screenDC);
    ReleaseDC(nullptr, screenDC);

    if (!g_dib || !g_memDC) return false;
    SelectObject(g_memDC, g_dib);
    return true;
}

// ---------- Drawing ----------
static inline BYTE ClampB(float v) {
    if (v < 0) return 0;
    if (v > 255) return 255;
    return (BYTE)v;
}

void DrawRipple(Graphics& gfx, const Ripple& r, float p) {
    // Ease-in-out sinusoidal easing.
    float eased  = 0.5f - 0.5f * cosf(p * 3.14159265f);
    float radius = r.maxRadius * eased;

    // Non-linear opacity curve for a natural fade.
    float alphaF = powf(1.0f - p, 1.3f);
    BYTE  baseA  = ClampB(255.0f * alphaF);
    if (baseA < 3) return;

    BYTE cr = r.color.GetR();
    BYTE cg = r.color.GetG();
    BYTE cb = r.color.GetB();

    // Dynamic thickness: thins out as the ripple grows.
    float thickness = r.thickness * (1.0f - 0.55f * p);
    if (thickness < 0.7f) thickness = 0.7f;

    // Soft halo: 3 concentric rings.
    for (int i = 3; i >= 1; --i) {
        float gThick = thickness * (1.0f + i * 2.0f);
        BYTE  gAlpha = ClampB(baseA * (0.12f / i));
        if (gAlpha < 2) continue;
        Pen gp(Color(gAlpha, cr, cg, cb), gThick);
        gp.SetAlignment(PenAlignmentCenter);
        gfx.DrawEllipse(&gp, r.x - radius, r.y - radius, radius * 2, radius * 2);
    }

    // Sharp main ring.
    {
        Pen pen(Color(baseA, cr, cg, cb), thickness);
        pen.SetAlignment(PenAlignmentCenter);
        gfx.DrawEllipse(&pen, r.x - radius, r.y - radius, radius * 2, radius * 2);
    }

    // Inner flash (first half only).
    if (p < 0.55f) {
        float ip     = p / 0.55f;
        float innerR = radius * (0.65f - 0.30f * ip);
        BYTE  innerA = ClampB(baseA * 0.35f * (1.0f - ip));
        if (innerA > 4 && innerR > 0.5f) {
            BYTE hr = (BYTE)(cr > 175 ? 255 : cr + 80);
            BYTE hg = (BYTE)(cg > 175 ? 255 : cg + 80);
            BYTE hb = (BYTE)(cb > 175 ? 255 : cb + 80);
            Pen ip2(Color(innerA, hr, hg, hb), 1.2f);
            ip2.SetAlignment(PenAlignmentCenter);
            gfx.DrawEllipse(&ip2, r.x - innerR, r.y - innerR, innerR * 2, innerR * 2);
        }
    }
}

// ---------- Render frame ----------
void RenderFrame() {
    if (!g_memDC || !g_overlayWnd) return;

    {
        Graphics gfx(g_memDC);
        gfx.SetSmoothingMode(SmoothingModeAntiAlias);
        gfx.SetPixelOffsetMode(PixelOffsetModeHalf);
        gfx.SetInterpolationMode(InterpolationModeHighQualityBicubic);

        gfx.SetCompositingMode(CompositingModeSourceCopy);
        gfx.Clear(Color(0, 0, 0, 0));
        gfx.SetCompositingMode(CompositingModeSourceOver);

        std::lock_guard<std::mutex> lock(g_rippleMutex);
        DWORD now = GetTickCount();
        for (auto it = g_ripples.begin(); it != g_ripples.end();) {
            DWORD elapsed = now - it->startTime;
            if (elapsed >= (DWORD)it->duration) {
                it = g_ripples.erase(it);
                continue;
            }
            float p = (float)elapsed / it->duration;
            DrawRipple(gfx, *it, p);
            ++it;
        }
    }

    HDC screenDC = GetDC(nullptr);
    if (!screenDC) return;

    POINT        ptSrc = { 0, 0 };
    SIZE         size  = { g_vw, g_vh };
    POINT        ptDst = { g_vx, g_vy };
    BLENDFUNCTION bf   = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };

    UpdateLayeredWindow(g_overlayWnd, screenDC, &ptDst, &size,
                        g_memDC, &ptSrc, 0, &bf, ULW_ALPHA);
    ReleaseDC(nullptr, screenDC);
}

// ---------- WndProc ----------
LRESULT CALLBACK OverlayWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_TIMER: {
        bool active;
        {
            std::lock_guard<std::mutex> lock(g_rippleMutex);
            active = !g_ripples.empty();
        }
        if (active || g_wasActive) {
            RenderFrame();
            g_wasActive = active;
        }
        return 0;
    }
    case WM_NCHITTEST:  return HTTRANSPARENT;
    case WM_ERASEBKGND: return 1;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

// ---------- Mouse hook ----------
LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION &&
        (wParam == WM_LBUTTONDOWN || wParam == WM_RBUTTONDOWN)) {

        MSLLHOOKSTRUCT* p = (MSLLHOOKSTRUCT*)lParam;
        Ripple r;
        r.x         = (float)(p->pt.x - g_vx);
        r.y         = (float)(p->pt.y - g_vy);
        r.startTime = GetTickCount();
        r.maxRadius = g_maxRadius;
        r.duration  = g_duration;
        r.thickness = g_thickness;
        r.color     = (wParam == WM_LBUTTONDOWN) ? g_colorLeft : g_colorRight;

        {
            std::lock_guard<std::mutex> lock(g_rippleMutex);
            g_ripples.push_back(r);
        }
        RenderFrame();
        g_wasActive = true;
    }
    return CallNextHookEx(g_mouseHook, nCode, wParam, lParam);
}

// ---------- Overlay thread ----------
DWORD WINAPI OverlayThread(LPVOID) {
    timeBeginPeriod(1);

    GdiplusStartupInput gsi;
    if (GdiplusStartup(&g_gdiplusToken, &gsi, nullptr) != Ok) {
        Wh_Log(L"GDI+ startup failed");
        SetEvent(g_readyEvent);
        timeEndPeriod(1);
        return 1;
    }
    if (!InitDib()) {
        Wh_Log(L"InitDib failed");
        GdiplusShutdown(g_gdiplusToken);
        g_gdiplusToken = 0;
        SetEvent(g_readyEvent);
        timeEndPeriod(1);
        return 1;
    }

    WNDCLASSEX wc = { sizeof(WNDCLASSEX) };
    wc.lpfnWndProc   = OverlayWndProc;
    wc.hInstance     = GetModuleHandle(nullptr);
    wc.lpszClassName = L"WindhawkClickRippleOverlay";
    wc.hCursor       = LoadCursor(nullptr, IDC_ARROW);
    RegisterClassEx(&wc);

    g_overlayWnd = CreateWindowEx(
        WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST |
        WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        wc.lpszClassName, L"ClickRippleOverlay", WS_POPUP,
        g_vx, g_vy, g_vw, g_vh,
        nullptr, nullptr, wc.hInstance, nullptr);

    if (!g_overlayWnd) {
        Wh_Log(L"CreateWindowEx failed: %lu", GetLastError());
        GdiplusShutdown(g_gdiplusToken);
        g_gdiplusToken = 0;
        SetEvent(g_readyEvent);
        timeEndPeriod(1);
        return 1;
    }

    ShowWindow(g_overlayWnd, SW_SHOWNOACTIVATE);
    UpdateWindow(g_overlayWnd);
    RenderFrame();

    g_mouseHook = SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProc,
                                   GetModuleHandle(nullptr), 0);
    if (!g_mouseHook)
        Wh_Log(L"SetWindowsHookEx failed: %lu", GetLastError());

    SetTimer(g_overlayWnd, 1, 8, nullptr);
    SetEvent(g_readyEvent);

    MSG msg;
    while (!g_stopThread.load() && GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (g_mouseHook)   { UnhookWindowsHookEx(g_mouseHook); g_mouseHook = nullptr; }
    if (g_overlayWnd)  { KillTimer(g_overlayWnd, 1); DestroyWindow(g_overlayWnd); g_overlayWnd = nullptr; }
    if (g_memDC)       { DeleteDC(g_memDC); g_memDC = nullptr; }
    if (g_dib)         { DeleteObject(g_dib); g_dib = nullptr; }
    if (g_gdiplusToken){ GdiplusShutdown(g_gdiplusToken); g_gdiplusToken = 0; }

    timeEndPeriod(1);
    return 0;
}

// ---------- Windhawk callbacks ----------
BOOL Wh_ModInit() {
    Wh_Log(L"Click Ripple init");

    g_instanceMutex = CreateMutex(nullptr, TRUE, L"Global\\WindhawkClickRipple");
    if (!g_instanceMutex || GetLastError() == ERROR_ALREADY_EXISTS) {
        Wh_Log(L"Another instance is already active.");
        if (g_instanceMutex) { CloseHandle(g_instanceMutex); g_instanceMutex = nullptr; }
        return FALSE;
    }

    LoadSettings();
    g_stopThread = false;
    g_wasActive  = false;

    g_readyEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    if (!g_readyEvent) return FALSE;

    g_thread = CreateThread(nullptr, 0, OverlayThread, nullptr, 0, &g_threadId);
    if (!g_thread) {
        CloseHandle(g_readyEvent); g_readyEvent = nullptr;
        return FALSE;
    }

    WaitForSingleObject(g_readyEvent, 5000);

    if (!g_overlayWnd) {
        Wh_Log(L"Overlay window could not be created");
        g_stopThread = true;
        if (g_threadId) PostThreadMessage(g_threadId, WM_QUIT, 0, 0);
        WaitForSingleObject(g_thread, 2000);
        CloseHandle(g_thread); g_thread = nullptr;
        CloseHandle(g_readyEvent); g_readyEvent = nullptr;
        return FALSE;
    }

    Wh_Log(L"Click Ripple ready");
    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Click Ripple uninit");

    g_stopThread = true;
    if (g_threadId) PostThreadMessage(g_threadId, WM_QUIT, 0, 0);
    if (g_thread) {
        WaitForSingleObject(g_thread, 3000);
        CloseHandle(g_thread);
        g_thread = nullptr;
    }
    if (g_readyEvent) { CloseHandle(g_readyEvent); g_readyEvent = nullptr; }

    {
        std::lock_guard<std::mutex> lock(g_rippleMutex);
        g_ripples.clear();
    }

    if (g_instanceMutex) {
        ReleaseMutex(g_instanceMutex);
        CloseHandle(g_instanceMutex);
        g_instanceMutex = nullptr;
    }
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    Wh_Log(L"Settings: L=%08X R=%08X r=%.0f dur=%.0f thk=%.0f",
           g_colorLeft.GetValue(), g_colorRight.GetValue(),
           g_maxRadius, g_duration, g_thickness);
}