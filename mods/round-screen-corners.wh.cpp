// ==WindhawkMod==
// @id              round-screen-corners
// @name            True Rounded Corners
// @description     Pins rounded-corner masks to each monitor's work-area corners, so any maximized window underneath appears rounded -- no per-window tracking needed
// @version         1.0
// @author          mursman
// @github          https://github.com/mursman
// @twitter         https://twitter.com/murs_man
// @homepage        https://behance.net/mursman
// @include         explorer.exe
// @compilerOptions -lgdi32 -luser32 -lkernel32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Rounded Corners Overlay (Screen-anchored)

Simpler alternative to per-window tracking: since a maximized window is
always flush with its monitor's work area, this pins four small
click-through, topmost, layered "corner mask" windows to the work-area
corners of every monitor -- once, at startup, plus whenever display
configuration changes (resolution, monitor added/removed, taskbar
moved/resized).

Whatever window happens to be maximized underneath a corner automatically
appears to have that corner clipped round. Works instantly for any app,
any process, with no window enumeration loop at all.

Trade-off vs. per-window tracking: this only rounds corners that touch a
monitor's work-area edge. A maximized window on a single monitor gets all
four corners rounded (since it fills the whole work area). If you ever use
a window that's large but not technically maximized, or an edge-snapped
half-screen window, only the corners that land on the monitor's own
corners will be rounded -- snapped windows won't get their inner corners
touched by this mod, which is usually what you want anyway (only the
screen-facing outer corners are visually squared off by DWM to begin
with).

Same limitation as before on fill color: the corner cutout is a solid
color, not a live capture of what's underneath. Tune `fill_color` to match
your wallpaper/taskbar area.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- radius: 14
  $name: Corner radius (px)
- fill_r: 0
  $name: Fill color - Red
- fill_g: 0
  $name: Fill color - Green
- fill_b: 0
  $name: Fill color - Blue
- recheck_interval_ms: 1000
  $name: Re-check interval (ms)
  $description: How often to re-assert topmost and check for display/monitor changes.
- cover_taskbar: true
  $name: Round the whole physical screen (covers taskbar corners too)
  $description: On = masks pin to the monitor's actual physical corners, so the whole display looks uniformly rounded, taskbar included. Off = masks stop at the work area (taskbar excluded), only rounding maximized-window corners.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <vector>
#include <cmath>

struct Settings {
    int radius;
    BYTE fillR, fillG, fillB;
    int recheckIntervalMs;
    bool coverTaskbar;
} g_settings;

void LoadSettings() {
    g_settings.radius = Wh_GetIntSetting(L"radius");
    if (g_settings.radius < 2) g_settings.radius = 2;
    if (g_settings.radius > 100) g_settings.radius = 100;
    g_settings.fillR = (BYTE)Wh_GetIntSetting(L"fill_r");
    g_settings.fillG = (BYTE)Wh_GetIntSetting(L"fill_g");
    g_settings.fillB = (BYTE)Wh_GetIntSetting(L"fill_b");
    g_settings.recheckIntervalMs = Wh_GetIntSetting(L"recheck_interval_ms");
    if (g_settings.recheckIntervalMs < 200) g_settings.recheckIntervalMs = 200;
    g_settings.coverTaskbar = Wh_GetIntSetting(L"cover_taskbar") != 0;
}

const wchar_t* kMaskClassName = L"WhScreenCornerMaskWnd";
const wchar_t* kWorkerClassName = L"WhScreenCornerWorkerWnd";
const UINT_PTR kTimerId = 1;

enum Corner { TL = 0, TR = 1, BL = 2, BR = 3 };

struct MonitorMasks {
    HMONITOR monitor;
    RECT workRect;
    HWND masks[4];
};

std::vector<MonitorMasks> g_monitors;
HANDLE g_hThread = nullptr;
DWORD g_threadId = 0;
HWND g_hWorkerWnd = nullptr;

LRESULT CALLBACK MaskWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    return DefWindowProcW(hWnd, msg, wParam, lParam);
}

void PaintCornerMask(HWND hWnd, Corner corner, int radius) {
    HDC screenDC = GetDC(nullptr);
    HDC memDC = CreateCompatibleDC(screenDC);

    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = radius;
    bmi.bmiHeader.biHeight = -radius; // top-down
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void* bits = nullptr;
    HBITMAP hBmp = CreateDIBSection(memDC, &bmi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!hBmp || !bits) {
        if (hBmp) DeleteObject(hBmp);
        DeleteDC(memDC);
        ReleaseDC(nullptr, screenDC);
        return;
    }

    HBITMAP hOldBmp = (HBITMAP)SelectObject(memDC, hBmp);

    double cx, cy;
    switch (corner) {
        case TL: cx = radius; cy = radius; break;
        case TR: cx = 0;      cy = radius; break;
        case BL: cx = radius; cy = 0;      break;
        default: cx = 0;      cy = 0;      break; // BR
    }

    struct Pixel { BYTE b, g, r, a; };
    Pixel* px = (Pixel*)bits;

    for (int y = 0; y < radius; y++) {
        for (int x = 0; x < radius; x++) {
            double dx = x + 0.5 - cx;
            double dy = y + 0.5 - cy;
            double dist = sqrt(dx * dx + dy * dy);

            Pixel& p = px[y * radius + x];
            if (dist > radius) {
                p.b = g_settings.fillB;
                p.g = g_settings.fillG;
                p.r = g_settings.fillR;
                p.a = 255;
            } else {
                p.b = p.g = p.r = p.a = 0;
            }
        }
    }

    POINT ptSrc = {0, 0};
    SIZE size = {radius, radius};
    BLENDFUNCTION blend = {};
    blend.BlendOp = AC_SRC_OVER;
    blend.SourceConstantAlpha = 255;
    blend.AlphaFormat = AC_SRC_ALPHA;

    UpdateLayeredWindow(hWnd, screenDC, nullptr, &size, memDC, &ptSrc, 0,
                         &blend, ULW_ALPHA);

    SelectObject(memDC, hOldBmp);
    DeleteObject(hBmp);
    DeleteDC(memDC);
    ReleaseDC(nullptr, screenDC);
}

HWND CreateCornerMask(Corner corner, int x, int y, int radius) {
    HWND hWnd = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW |
            WS_EX_NOACTIVATE | WS_EX_TOPMOST,
        kMaskClassName, nullptr, WS_POPUP, x, y, radius, radius, nullptr,
        nullptr, GetModuleHandleW(nullptr), nullptr);
    if (hWnd) {
        PaintCornerMask(hWnd, corner, radius);
        ShowWindow(hWnd, SW_SHOWNOACTIVATE);
    }
    return hWnd;
}

void PositionMonitorMasks(MonitorMasks& mm, int radius) {
    const RECT& r = mm.workRect;
    int x[4], y[4];
    x[TL] = r.left;              y[TL] = r.top;
    x[TR] = r.right - radius;    y[TR] = r.top;
    x[BL] = r.left;              y[BL] = r.bottom - radius;
    x[BR] = r.right - radius;    y[BR] = r.bottom - radius;

    for (int i = 0; i < 4; i++) {
        if (mm.masks[i]) {
            SetWindowPos(mm.masks[i], HWND_TOPMOST, x[i], y[i], radius,
                         radius, SWP_NOACTIVATE);
        }
    }
}

void DestroyMonitorMasks(MonitorMasks& mm) {
    for (int i = 0; i < 4; i++) {
        if (mm.masks[i]) {
            DestroyWindow(mm.masks[i]);
            mm.masks[i] = nullptr;
        }
    }
}

BOOL CALLBACK MonitorEnumProc(HMONITOR hMon, HDC, LPRECT, LPARAM lParam) {
    auto* list = (std::vector<std::pair<HMONITOR, RECT>>*)lParam;
    MONITORINFO mi = {sizeof(mi)};
    if (GetMonitorInfoW(hMon, &mi)) {
        // Use the full physical monitor rect (not just the work area) when
        // cover_taskbar is on, so corners are pinned to the actual screen
        // edges regardless of where the taskbar sits.
        RECT r = g_settings.coverTaskbar ? mi.rcMonitor : mi.rcWork;
        list->push_back({hMon, r});
    }
    return TRUE;
}

void RebuildMonitorMasks() {
    std::vector<std::pair<HMONITOR, RECT>> current;
    EnumDisplayMonitors(nullptr, nullptr, MonitorEnumProc, (LPARAM)&current);

    int radius = g_settings.radius;

    // Remove monitors that no longer exist or whose work area changed.
    for (size_t i = 0; i < g_monitors.size();) {
        bool stillPresent = false;
        RECT newRect = {};
        for (auto& c : current) {
            if (c.first == g_monitors[i].monitor) {
                stillPresent = true;
                newRect = c.second;
                break;
            }
        }
        if (!stillPresent ||
            memcmp(&newRect, &g_monitors[i].workRect, sizeof(RECT)) != 0) {
            DestroyMonitorMasks(g_monitors[i]);
            g_monitors.erase(g_monitors.begin() + i);
        } else {
            i++;
        }
    }

    // Add new/changed monitors.
    for (auto& c : current) {
        bool exists = false;
        for (auto& mm : g_monitors) {
            if (mm.monitor == c.first) { exists = true; break; }
        }
        if (!exists) {
            MonitorMasks mm = {};
            mm.monitor = c.first;
            mm.workRect = c.second;
            mm.masks[TL] = CreateCornerMask(TL, c.second.left, c.second.top, radius);
            mm.masks[TR] = CreateCornerMask(TR, c.second.right - radius, c.second.top, radius);
            mm.masks[BL] = CreateCornerMask(BL, c.second.left, c.second.bottom - radius, radius);
            mm.masks[BR] = CreateCornerMask(BR, c.second.right - radius, c.second.bottom - radius, radius);
            g_monitors.push_back(mm);
        }
    }
}

void PeriodicRecheck() {
    // Cheap check: re-verify monitor list/work areas still match. This
    // catches resolution changes, taskbar resize/move, monitor
    // connect/disconnect even without relying solely on WM_DISPLAYCHANGE.
    std::vector<std::pair<HMONITOR, RECT>> current;
    EnumDisplayMonitors(nullptr, nullptr, MonitorEnumProc, (LPARAM)&current);

    bool changed = (current.size() != g_monitors.size());
    if (!changed) {
        for (auto& c : current) {
            bool found = false;
            for (auto& mm : g_monitors) {
                if (mm.monitor == c.first &&
                    memcmp(&mm.workRect, &c.second, sizeof(RECT)) == 0) {
                    found = true;
                    break;
                }
            }
            if (!found) { changed = true; break; }
        }
    }

    if (changed) {
        RebuildMonitorMasks();
    } else {
        // Just re-assert topmost so other fullscreen/topmost apps can't
        // permanently bury the masks.
        for (auto& mm : g_monitors) {
            PositionMonitorMasks(mm, g_settings.radius);
        }
    }
}

LRESULT CALLBACK WorkerWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_TIMER:
            if (wParam == kTimerId) {
                PeriodicRecheck();
            }
            return 0;
        case WM_DISPLAYCHANGE:
            RebuildMonitorMasks();
            return 0;
        case WM_DESTROY:
            KillTimer(hWnd, kTimerId);
            for (auto& mm : g_monitors) {
                DestroyMonitorMasks(mm);
            }
            g_monitors.clear();
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcW(hWnd, msg, wParam, lParam);
}

DWORD WINAPI WorkerThreadProc(LPVOID) {
    HINSTANCE hInst = GetModuleHandleW(nullptr);

    WNDCLASSEXW maskClass = {sizeof(maskClass)};
    maskClass.lpfnWndProc = MaskWndProc;
    maskClass.hInstance = hInst;
    maskClass.lpszClassName = kMaskClassName;
    RegisterClassExW(&maskClass);

    WNDCLASSEXW workerClass = {sizeof(workerClass)};
    workerClass.lpfnWndProc = WorkerWndProc;
    workerClass.hInstance = hInst;
    workerClass.lpszClassName = kWorkerClassName;
    RegisterClassExW(&workerClass);

    g_hWorkerWnd = CreateWindowExW(0, kWorkerClassName, nullptr, WS_POPUP, 0,
                                    0, 0, 0, nullptr, nullptr, hInst, nullptr);
    if (!g_hWorkerWnd) return 1;

    RebuildMonitorMasks();
    SetTimer(g_hWorkerWnd, kTimerId, g_settings.recheckIntervalMs, nullptr);

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    return 0;
}

BOOL Wh_ModInit() {
    LoadSettings();
    g_hThread = CreateThread(nullptr, 0, WorkerThreadProc, nullptr, 0, &g_threadId);
    return g_hThread != nullptr;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    if (g_hWorkerWnd) {
        PostMessageW(g_hWorkerWnd, WM_DISPLAYCHANGE, 0, 0);
    }
}

void Wh_ModUninit() {
    if (g_hWorkerWnd) {
        PostMessageW(g_hWorkerWnd, WM_DESTROY, 0, 0);
    }
    if (g_hThread) {
        WaitForSingleObject(g_hThread, 3000);
        CloseHandle(g_hThread);
    }
}