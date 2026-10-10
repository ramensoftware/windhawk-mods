// ==WindhawkMod==
// @id              simple-rounded-corners-overlay
// @name            Simple Rounded Corners Overlay
// @description     Draws rounded corner overlays on every monitor. Customizable color, size and roundness, hides on fullscreen apps.
// @version         1.1
// @author          NoMoreDg
// @github          https://github.com/NoMoreDg/Windhawk-Mods
// @include         windhawk.exe
// @compilerOptions -luser32 -lgdi32 -lshell32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Simple Rounded Corners
Just make the monitor rounded.

## Features
- Change the color & opacity
- Roundness & size
- Fullscreen detection ( Enabling it hides the overlay when a fullscreen app is focused )

## Source code
[View the source code on GitHub](https://github.com/NoMoreDg/Windhawk-Mods)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- Color: "#000000"
  $name: Corner color
  $description: Hex color, e.g. #000000
- Opacity: 100
  $name: Opacity (%)
  $description: 1-100
- Size: 30
  $name: Corner size (px)
  $description: 1-500
- Roundness: 100
  $name: Roundness
  $description: 100 = perfect circular curve, lower = squarer / tighter corner (1-100)
- HideOnFullscreen: true
  $name: Hide on fullscreen applications
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shellapi.h>
#include <vector>
#include <cmath>
#include <algorithm>

#define WM_APP_RELOAD (WM_APP + 1)

struct Settings {
    int r = 0, g = 0, b = 0;
    int opacity = 100;
    int size = 30;
    int roundness = 100;
    bool hideFs = true;
} g_s;

struct Overlay {
    HMONITOR mon;
    RECT rc;
    HWND wnd[4];   // 0=TL 1=TR 2=BL 3=BR
};

static std::vector<Overlay> g_overlays;
static HANDLE g_thread = nullptr;
static HANDLE g_ready = nullptr;
static DWORD g_threadId = 0;
static const wchar_t* kClass = L"WH_SimpleRoundedCornersOverlay";

static void LoadSettings() {
    PCWSTR c = Wh_GetStringSetting(L"Color");
    const wchar_t* p = c ? c : L"000000";
    if (*p == L'#') p++;
    unsigned long v = wcstoul(p, nullptr, 16) & 0xFFFFFF;
    Wh_FreeStringSetting(c);
    g_s.r = (v >> 16) & 0xFF;
    g_s.g = (v >> 8) & 0xFF;
    g_s.b = v & 0xFF;
    g_s.opacity = std::clamp(Wh_GetIntSetting(L"Opacity"), 1, 100);
    g_s.size = std::clamp(Wh_GetIntSetting(L"Size"), 1, 500);
    g_s.roundness = std::clamp(Wh_GetIntSetting(L"Roundness"), 1, 100);
    g_s.hideFs = Wh_GetIntSetting(L"HideOnFullscreen") != 0;
}

// Coverage mask for the top-left corner (255 = fully covered by overlay color)
static std::vector<BYTE> BuildMask(int S, int roundness) {
    double n = 2.0 + (100 - roundness) / 100.0 * 6.0;
    std::vector<BYTE> m((size_t)S * S);
    const int SS = 4;
    for (int y = 0; y < S; y++) {
        for (int x = 0; x < S; x++) {
            int out = 0;
            for (int j = 0; j < SS; j++) {
                for (int i = 0; i < SS; i++) {
                    double px = x + (i + 0.5) / SS;
                    double py = y + (j + 0.5) / SS;
                    double dx = (S - px) / S;
                    double dy = (S - py) / S;
                    double d = (n == 2.0) ? dx * dx + dy * dy
                                          : pow(dx, n) + pow(dy, n);
                    if (d > 1.0) out++;
                }
            }
            m[(size_t)y * S + x] = (BYTE)(out * 255 / (SS * SS));
        }
    }
    return m;
}

static LRESULT CALLBACK WndProc(HWND h, UINT msg, WPARAM w, LPARAM l) {
    switch (msg) {
        case WM_NCHITTEST: return HTTRANSPARENT;
        case WM_MOUSEACTIVATE: return MA_NOACTIVATE;
    }
    return DefWindowProcW(h, msg, w, l);
}

static HWND CreateCorner(int corner, int x, int y, int S, const std::vector<BYTE>& mask) {
    HWND h = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        kClass, L"", WS_POPUP, x, y, S, S, nullptr, nullptr, GetModuleHandleW(nullptr), nullptr);
    if (!h) return nullptr;

    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = S;
    bi.bmiHeader.biHeight = -S;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;

    HDC screen = GetDC(nullptr);
    HDC mem = CreateCompatibleDC(screen);
    void* bits = nullptr;
    HBITMAP bmp = CreateDIBSection(screen, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (bmp && bits) {
        HGDIOBJ old = SelectObject(mem, bmp);
        DWORD* px = (DWORD*)bits;
        for (int yy = 0; yy < S; yy++) {
            for (int xx = 0; xx < S; xx++) {
                int mx = (corner == 1 || corner == 3) ? S - 1 - xx : xx;
                int my = (corner >= 2) ? S - 1 - yy : yy;
                unsigned a = mask[(size_t)my * S + mx] * g_s.opacity / 100;
                unsigned r = g_s.r * a / 255;
                unsigned g = g_s.g * a / 255;
                unsigned b = g_s.b * a / 255;
                px[yy * S + xx] = (a << 24) | (r << 16) | (g << 8) | b;
            }
        }
        POINT dst = {x, y};
        SIZE sz = {S, S};
        POINT src = {0, 0};
        BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
        UpdateLayeredWindow(h, screen, &dst, &sz, mem, &src, 0, &bf, ULW_ALPHA);
        SelectObject(mem, old);
    }
    if (bmp) DeleteObject(bmp);
    DeleteDC(mem);
    ReleaseDC(nullptr, screen);
    return h;
}

static void DestroyOverlays() {
    for (auto& o : g_overlays)
        for (HWND h : o.wnd)
            if (h) DestroyWindow(h);
    g_overlays.clear();
}

static BOOL CALLBACK EnumMon(HMONITOR m, HDC, LPRECT, LPARAM lp) {
    MONITORINFO mi = {sizeof(mi)};
    if (GetMonitorInfoW(m, &mi)) {
        Overlay o = {};
        o.mon = m;
        o.rc = mi.rcMonitor;
        ((std::vector<Overlay>*)lp)->push_back(o);
    }
    return TRUE;
}

static std::vector<Overlay> QueryMonitors() {
    std::vector<Overlay> v;
    EnumDisplayMonitors(nullptr, nullptr, EnumMon, (LPARAM)&v);
    return v;
}

static void Rebuild() {
    DestroyOverlays();
    g_overlays = QueryMonitors();
    int S = g_s.size;
    auto mask = BuildMask(S, g_s.roundness);
    for (auto& o : g_overlays) {
        const RECT& r = o.rc;
        int mw = r.right - r.left, mh = r.bottom - r.top;
        int s = std::min(S, std::min(mw, mh));
        o.wnd[0] = CreateCorner(0, r.left,      r.top,        S, mask);
        o.wnd[1] = CreateCorner(1, r.right - S, r.top,        S, mask);
        o.wnd[2] = CreateCorner(2, r.left,      r.bottom - S, S, mask);
        o.wnd[3] = CreateCorner(3, r.right - S, r.bottom - S, S, mask);
        (void)s;
    }
}

static bool MonitorsChanged() {
    auto cur = QueryMonitors();
    if (cur.size() != g_overlays.size()) return true;
    for (size_t i = 0; i < cur.size(); i++) {
        if (cur[i].mon != g_overlays[i].mon ||
            !EqualRect(&cur[i].rc, &g_overlays[i].rc))
            return true;
    }
    return false;
}

static void Tick() {
    if (MonitorsChanged()) Rebuild();

    HMONITOR fsMon = nullptr;
    bool allFs = false;

    if (g_s.hideFs) {
        HWND fg = GetForegroundWindow();
        if (fg && !IsIconic(fg) && IsWindowVisible(fg)) {
            wchar_t cls[64] = {};
            GetClassNameW(fg, cls, 64);
            bool ignore = !wcscmp(cls, L"Progman") || !wcscmp(cls, L"WorkerW") ||
                          !wcscmp(cls, L"Shell_TrayWnd") || !wcscmp(cls, L"Shell_SecondaryTrayWnd") ||
                          !wcscmp(cls, kClass);
            if (!ignore) {
                HMONITOR m = MonitorFromWindow(fg, MONITOR_DEFAULTTONULL);
                if (m) {
                    MONITORINFO mi = {sizeof(mi)};
                    RECT wr;
                    if (GetMonitorInfoW(m, &mi) && GetWindowRect(fg, &wr)) {
                        bool covers = wr.left <= mi.rcMonitor.left && wr.top <= mi.rcMonitor.top &&
                                      wr.right >= mi.rcMonitor.right && wr.bottom >= mi.rcMonitor.bottom;
                        bool hasCaption = (GetWindowLongW(fg, GWL_STYLE) & WS_CAPTION) == WS_CAPTION;
                        if (covers && !hasCaption) fsMon = m;
                    }
                }
            }
        }
        QUERY_USER_NOTIFICATION_STATE st;
        if (SUCCEEDED(SHQueryUserNotificationState(&st)) && st == QUNS_RUNNING_D3D_FULL_SCREEN) {
            HMONITOR m = fg ? MonitorFromWindow(fg, MONITOR_DEFAULTTONULL) : nullptr;
            if (m) fsMon = m; else allFs = true;
        }
    }

    for (auto& o : g_overlays) {
        bool show = !(allFs || (fsMon && o.mon == fsMon));
        for (HWND h : o.wnd) {
            if (!h) continue;
            if (show)
                SetWindowPos(h, HWND_TOPMOST, 0, 0, 0, 0,
                             SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW);
            else if (IsWindowVisible(h))
                ShowWindow(h, SW_HIDE);
        }
    }
}

static DWORD WINAPI OverlayThread(LPVOID) {
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    MSG msg;
    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE); // create queue
    SetEvent(g_ready);

    WNDCLASSW wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.lpszClassName = kClass;
    RegisterClassW(&wc);

    LoadSettings();
    Rebuild();
    Tick();
    SetTimer(nullptr, 1, 250, nullptr);

    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        if (msg.message == WM_APP_RELOAD) {
            LoadSettings();
            Rebuild();
            Tick();
        } else if (msg.message == WM_TIMER) {
            Tick();
        } else {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }

    DestroyOverlays();
    UnregisterClassW(kClass, GetModuleHandleW(nullptr));
    return 0;
}

BOOL Wh_ModInit() {
    g_ready = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_thread = CreateThread(nullptr, 0, OverlayThread, nullptr, 0, &g_threadId);
    if (!g_thread) return FALSE;
    WaitForSingleObject(g_ready, 5000);
    return TRUE;
}

void Wh_ModSettingsChanged() {
    if (g_threadId) PostThreadMessageW(g_threadId, WM_APP_RELOAD, 0, 0);
}

void Wh_ModUninit() {
    if (g_thread) {
        PostThreadMessageW(g_threadId, WM_QUIT, 0, 0);
        WaitForSingleObject(g_thread, INFINITE);
        CloseHandle(g_thread);
        g_thread = nullptr;
    }
    if (g_ready) { CloseHandle(g_ready); g_ready = nullptr; }
}