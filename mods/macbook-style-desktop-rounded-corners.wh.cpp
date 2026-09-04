// ==WindhawkMod==
// @id              macbook-style-desktop-rounded-corners
// @name            MacBook Style Desktop Rounded Corners
// @description     Adds MacBook-style rounded black corners to every active monitor.
// @version         1.1.1
// @author          Jona Like It,Code It
// @github          https://github.com/Stunning-dev
// @include         %SystemRoot%\explorer.exe
// @compilerOptions -lgdiplus -luser32 -lgdi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# MacBook Style Desktop Rounded Corners

Creates click-through, topmost, layered overlay windows at the physical bounds of
each active display and draws only the corner cutouts. The default radius is 30
pixels, matching the requested desktop cut-off.
![image](https://i.imgur.com/FYtDHgr.png)
*Before*

![image](https://i.imgur.com/7nI3y2E.png)
*After*

## Notes

- The mod targets Explorer because Explorer owns the desktop shell lifetime.
- It does not modify system files or patch DWM.
- The overlays use `WS_EX_TOPMOST`, `WS_EX_LAYERED`, `WS_EX_TRANSPARENT`,
  `WS_EX_TOOLWINDOW`, and `WS_EX_NOACTIVATE`.
- The mod listens for display, device, DPI, and settings changes, and also polls
  the monitor layout briefly so unplug/replug events do not leave stale masks.
- With `HideWhenFullscreenAppIsActive` enabled, the masks are hidden while the
  foreground app covers a monitor, which helps avoid drawing over borderless
  fullscreen games and video.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- Radius: 30
  $name: Corner radius
  $description: Corner cut-off radius. Default is 30 pixels.
- ScaleRadiusByDpi: false
  $name: Scale radius by monitor DPI
  $description: When enabled, Radius is treated as a 96-DPI logical value and is scaled per monitor.
- Opacity: 255
  $name: Corner opacity
  $description: 0 is transparent, 255 is fully black.
- ColorRed: 0
  $name: Red
- ColorGreen: 0
  $name: Green
- ColorBlue: 0
  $name: Blue
- HideWhenFullscreenAppIsActive: true
  $name: Hide during fullscreen apps
  $description: Hides the corner masks when a foreground app covers an entire monitor.
- KeepTopmostIntervalMs: 2000
  $name: Topmost refresh interval
  $description: Periodically restores topmost placement and checks monitor layout. Use 0 to disable.
*/
// ==/WindhawkModSettings==

#define UNICODE
#define _UNICODE
#include <windows.h>
#include <gdiplus.h>

#include <algorithm>
#include <string>
#include <vector>

#pragma comment(lib, "gdiplus.lib")

namespace {

constexpr wchar_t kControllerClassName[] = L"WindhawkDesktopRoundedCornersController";
constexpr wchar_t kOverlayClassName[] = L"WindhawkDesktopRoundedCornersOverlay";
constexpr UINT_PTR kRefreshTimerId = 1;
constexpr UINT kRebuildMessage = WM_APP + 0x432;
constexpr UINT kRefreshMessage = WM_APP + 0x433;

struct Settings {
    int radius = 30;
    bool scaleRadiusByDpi = false;
    BYTE opacity = 255;
    BYTE red = 0;
    BYTE green = 0;
    BYTE blue = 0;
    bool hideWhenFullscreenAppIsActive = true;
    int keepTopmostIntervalMs = 2000;
};

struct MonitorInfo {
    RECT rc{};
    UINT dpi = 96;
};

struct OverlayWindow {
    HWND hwnd = nullptr;
    RECT rc{};
    UINT dpi = 96;
};

using SetThreadDpiAwarenessContext_t = DPI_AWARENESS_CONTEXT(WINAPI*)(DPI_AWARENESS_CONTEXT);
using GetDpiForMonitor_t = HRESULT(WINAPI*)(HMONITOR, int, UINT*, UINT*);

Settings g_settings;
HWND g_controllerWindow = nullptr;
HANDLE g_workerThread = nullptr;
DWORD g_workerThreadId = 0;
ULONG_PTR g_gdiplusToken = 0;
std::vector<OverlayWindow> g_overlays;
std::wstring g_monitorSignature;

int ClampInt(int value, int low, int high) {
    return std::max(low, std::min(value, high));
}

void LoadSettings() {
    g_settings.radius = ClampInt(Wh_GetIntSetting(L"Radius"), 1, 512);
    g_settings.scaleRadiusByDpi = Wh_GetIntSetting(L"ScaleRadiusByDpi") != 0;
    g_settings.opacity = static_cast<BYTE>(ClampInt(Wh_GetIntSetting(L"Opacity"), 0, 255));
    g_settings.red = static_cast<BYTE>(ClampInt(Wh_GetIntSetting(L"ColorRed"), 0, 255));
    g_settings.green = static_cast<BYTE>(ClampInt(Wh_GetIntSetting(L"ColorGreen"), 0, 255));
    g_settings.blue = static_cast<BYTE>(ClampInt(Wh_GetIntSetting(L"ColorBlue"), 0, 255));
    g_settings.hideWhenFullscreenAppIsActive =
        Wh_GetIntSetting(L"HideWhenFullscreenAppIsActive") != 0;
    g_settings.keepTopmostIntervalMs =
        ClampInt(Wh_GetIntSetting(L"KeepTopmostIntervalMs"), 0, 60000);
}

UINT GetMonitorDpi(HMONITOR monitor) {
    static auto getDpiForMonitor = []() -> GetDpiForMonitor_t {
        HMODULE shcore = LoadLibraryW(L"shcore.dll");
        if (!shcore) {
            return nullptr;
        }

        return reinterpret_cast<GetDpiForMonitor_t>(
            GetProcAddress(shcore, "GetDpiForMonitor"));
    }();

    if (getDpiForMonitor) {
        UINT dpiX = 96;
        UINT dpiY = 96;
        if (SUCCEEDED(getDpiForMonitor(monitor, 0 /* MDT_EFFECTIVE_DPI */, &dpiX, &dpiY)) &&
            dpiX > 0) {
            return dpiX;
        }
    }

    HDC dc = GetDC(nullptr);
    if (!dc) {
        return 96;
    }

    int dpi = GetDeviceCaps(dc, LOGPIXELSX);
    ReleaseDC(nullptr, dc);
    return dpi > 0 ? static_cast<UINT>(dpi) : 96;
}

BOOL CALLBACK EnumMonitorsProc(HMONITOR monitor, HDC, LPRECT, LPARAM data) {
    auto monitors = reinterpret_cast<std::vector<MonitorInfo>*>(data);

    MONITORINFO monitorInfo{};
    monitorInfo.cbSize = sizeof(monitorInfo);
    if (GetMonitorInfoW(monitor, &monitorInfo)) {
        monitors->push_back({monitorInfo.rcMonitor, GetMonitorDpi(monitor)});
    }

    return TRUE;
}

std::vector<MonitorInfo> GetMonitors() {
    std::vector<MonitorInfo> monitors;
    EnumDisplayMonitors(nullptr, nullptr, EnumMonitorsProc,
                        reinterpret_cast<LPARAM>(&monitors));
    return monitors;
}

std::wstring BuildMonitorSignature(const std::vector<MonitorInfo>& monitors) {
    std::wstring signature;
    for (const auto& monitor : monitors) {
        signature += std::to_wstring(monitor.rc.left) + L"," +
                     std::to_wstring(monitor.rc.top) + L"," +
                     std::to_wstring(monitor.rc.right) + L"," +
                     std::to_wstring(monitor.rc.bottom) + L"," +
                     std::to_wstring(monitor.dpi) + L";";
    }

    return signature;
}

bool IsShellWindow(HWND hwnd) {
    wchar_t className[128]{};
    GetClassNameW(hwnd, className, ARRAYSIZE(className));

    return wcscmp(className, L"Progman") == 0 ||
           wcscmp(className, L"WorkerW") == 0 ||
           wcscmp(className, L"Shell_TrayWnd") == 0 ||
           wcscmp(className, L"Shell_SecondaryTrayWnd") == 0;
}

bool ForegroundCoversMonitor(const RECT& monitorRect) {
    HWND foreground = GetForegroundWindow();
    if (!foreground || foreground == g_controllerWindow || IsShellWindow(foreground)) {
        return false;
    }

    for (const auto& overlay : g_overlays) {
        if (foreground == overlay.hwnd) {
            return false;
        }
    }

    RECT windowRect{};
    if (!GetWindowRect(foreground, &windowRect)) {
        return false;
    }

    return windowRect.left <= monitorRect.left &&
           windowRect.top <= monitorRect.top &&
           windowRect.right >= monitorRect.right &&
           windowRect.bottom >= monitorRect.bottom;
}

int RadiusForMonitor(UINT dpi, int width, int height) {
    int radius = g_settings.radius;
    if (g_settings.scaleRadiusByDpi) {
        radius = MulDiv(radius, static_cast<int>(dpi), 96);
    }

    return ClampInt(radius, 1, std::max(1, std::min(width, height) / 2));
}

void AddCornerMasks(Gdiplus::GraphicsPath& path, int width, int height, int radius) {
    const Gdiplus::REAL w = static_cast<Gdiplus::REAL>(width);
    const Gdiplus::REAL h = static_cast<Gdiplus::REAL>(height);
    const Gdiplus::REAL r = static_cast<Gdiplus::REAL>(radius);
    const Gdiplus::REAL d = r * 2.0f;

    path.StartFigure();
    path.AddLine(0.0f, 0.0f, r, 0.0f);
    path.AddArc(0.0f, 0.0f, d, d, 270.0f, -90.0f);
    path.AddLine(0.0f, r, 0.0f, 0.0f);
    path.CloseFigure();

    path.StartFigure();
    path.AddLine(w, 0.0f, w - r, 0.0f);
    path.AddArc(w - d, 0.0f, d, d, 270.0f, 90.0f);
    path.AddLine(w, r, w, 0.0f);
    path.CloseFigure();

    path.StartFigure();
    path.AddLine(w, h, w, h - r);
    path.AddArc(w - d, h - d, d, d, 0.0f, 90.0f);
    path.AddLine(w - r, h, w, h);
    path.CloseFigure();

    path.StartFigure();
    path.AddLine(0.0f, h, r, h);
    path.AddArc(0.0f, h - d, d, d, 90.0f, 90.0f);
    path.AddLine(0.0f, h - r, 0.0f, h);
    path.CloseFigure();
}

bool RenderOverlay(HWND hwnd, const RECT& rect, UINT dpi) {
    const int width = rect.right - rect.left;
    const int height = rect.bottom - rect.top;
    if (width <= 0 || height <= 0) {
        return false;
    }

    BITMAPINFO bitmapInfo{};
    bitmapInfo.bmiHeader.biSize = sizeof(bitmapInfo.bmiHeader);
    bitmapInfo.bmiHeader.biWidth = width;
    bitmapInfo.bmiHeader.biHeight = -height;
    bitmapInfo.bmiHeader.biPlanes = 1;
    bitmapInfo.bmiHeader.biBitCount = 32;
    bitmapInfo.bmiHeader.biCompression = BI_RGB;

    void* bits = nullptr;
    HBITMAP bitmap = CreateDIBSection(nullptr, &bitmapInfo, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!bitmap || !bits) {
        if (bitmap) {
            DeleteObject(bitmap);
        }
        return false;
    }

    ZeroMemory(bits, static_cast<SIZE_T>(width) * height * 4);

    HDC screenDc = GetDC(nullptr);
    HDC memDc = CreateCompatibleDC(screenDc);
    HGDIOBJ oldBitmap = SelectObject(memDc, bitmap);

    {
        Gdiplus::Graphics graphics(memDc);
        graphics.SetCompositingMode(Gdiplus::CompositingModeSourceOver);
        graphics.SetCompositingQuality(Gdiplus::CompositingQualityHighQuality);
        graphics.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
        graphics.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);
        graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);

        Gdiplus::GraphicsPath path;
        AddCornerMasks(path, width, height, RadiusForMonitor(dpi, width, height));

        Gdiplus::SolidBrush brush(Gdiplus::Color(
            g_settings.opacity, g_settings.red, g_settings.green, g_settings.blue));
        graphics.FillPath(&brush, &path);
    }

    POINT dstPoint{rect.left, rect.top};
    SIZE dstSize{width, height};
    POINT srcPoint{0, 0};
    BLENDFUNCTION blend{AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};

    BOOL ok = UpdateLayeredWindow(hwnd, screenDc, &dstPoint, &dstSize, memDc,
                                  &srcPoint, 0, &blend, ULW_ALPHA);

    SelectObject(memDc, oldBitmap);
    DeleteDC(memDc);
    ReleaseDC(nullptr, screenDc);
    DeleteObject(bitmap);

    return ok != FALSE;
}

LRESULT CALLBACK OverlayWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_NCHITTEST:
            return HTTRANSPARENT;

        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;

        case WM_ERASEBKGND:
            return TRUE;

        case WM_DISPLAYCHANGE:
        case WM_DEVICECHANGE:
        case WM_DPICHANGED:
        case WM_SETTINGCHANGE:
            if (g_controllerWindow) {
                PostMessageW(g_controllerWindow, kRebuildMessage, 0, 0);
            }
            return 0;
    }

    return DefWindowProcW(hwnd, message, wParam, lParam);
}

void DestroyOverlays() {
    for (const auto& overlay : g_overlays) {
        if (IsWindow(overlay.hwnd)) {
            DestroyWindow(overlay.hwnd);
        }
    }

    g_overlays.clear();
}

void RefreshOverlayVisibilityAndZOrder() {
    for (const auto& overlay : g_overlays) {
        bool hide = g_settings.hideWhenFullscreenAppIsActive &&
                    ForegroundCoversMonitor(overlay.rc);

        ShowWindow(overlay.hwnd, hide ? SW_HIDE : SW_SHOWNOACTIVATE);
        if (!hide) {
            SetWindowPos(overlay.hwnd, HWND_TOPMOST, overlay.rc.left, overlay.rc.top,
                         overlay.rc.right - overlay.rc.left,
                         overlay.rc.bottom - overlay.rc.top,
                         SWP_NOACTIVATE | SWP_SHOWWINDOW);
        }
    }
}

void RebuildOverlays(bool force) {
    auto monitors = GetMonitors();
    std::wstring signature = BuildMonitorSignature(monitors);
    if (!force && signature == g_monitorSignature) {
        RefreshOverlayVisibilityAndZOrder();
        return;
    }

    DestroyOverlays();
    g_monitorSignature = signature;

    for (const auto& monitor : monitors) {
        HWND hwnd = CreateWindowExW(
            WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TRANSPARENT |
                WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
            kOverlayClassName, L"Desktop Rounded Corners", WS_POPUP,
            monitor.rc.left, monitor.rc.top,
            monitor.rc.right - monitor.rc.left,
            monitor.rc.bottom - monitor.rc.top,
            nullptr, nullptr, GetModuleHandleW(nullptr), nullptr);

        if (!hwnd) {
            Wh_Log(L"Failed to create overlay window, error=%u", GetLastError());
            continue;
        }

        if (!RenderOverlay(hwnd, monitor.rc, monitor.dpi)) {
            Wh_Log(L"Failed to render overlay window, error=%u", GetLastError());
            DestroyWindow(hwnd);
            continue;
        }

        g_overlays.push_back({hwnd, monitor.rc, monitor.dpi});
    }

    RefreshOverlayVisibilityAndZOrder();
    Wh_Log(L"Rounded corner overlays rebuilt for %zu monitor(s)", g_overlays.size());
}

void RefreshTimer() {
    if (!g_controllerWindow) {
        return;
    }

    KillTimer(g_controllerWindow, kRefreshTimerId);
    if (g_settings.keepTopmostIntervalMs > 0) {
        SetTimer(g_controllerWindow, kRefreshTimerId,
                 static_cast<UINT>(g_settings.keepTopmostIntervalMs), nullptr);
    }
}

LRESULT CALLBACK ControllerWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_CREATE:
            RefreshTimer();
            PostMessageW(hwnd, kRebuildMessage, 0, 0);
            return 0;

        case WM_TIMER:
            if (wParam == kRefreshTimerId) {
                PostMessageW(hwnd, kRefreshMessage, 0, 0);
            }
            return 0;

        case WM_DISPLAYCHANGE:
        case WM_DEVICECHANGE:
        case WM_DPICHANGED:
        case WM_SETTINGCHANGE:
        case kRebuildMessage:
            RebuildOverlays(true);
            return 0;

        case kRefreshMessage:
            RebuildOverlays(false);
            return 0;

        case WM_DESTROY:
            KillTimer(hwnd, kRefreshTimerId);
            DestroyOverlays();
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProcW(hwnd, message, wParam, lParam);
}

bool RegisterWindowClasses() {
    HINSTANCE instance = GetModuleHandleW(nullptr);

    WNDCLASSEXW overlayClass{};
    overlayClass.cbSize = sizeof(overlayClass);
    overlayClass.lpfnWndProc = OverlayWndProc;
    overlayClass.hInstance = instance;
    overlayClass.lpszClassName = kOverlayClassName;
    overlayClass.hCursor = LoadCursorW(nullptr, IDC_ARROW);

    if (!RegisterClassExW(&overlayClass) &&
        GetLastError() != ERROR_CLASS_ALREADY_EXISTS) {
        return false;
    }

    WNDCLASSEXW controllerClass{};
    controllerClass.cbSize = sizeof(controllerClass);
    controllerClass.lpfnWndProc = ControllerWndProc;
    controllerClass.hInstance = instance;
    controllerClass.lpszClassName = kControllerClassName;

    return RegisterClassExW(&controllerClass) ||
           GetLastError() == ERROR_CLASS_ALREADY_EXISTS;
}

void MakeThreadPerMonitorDpiAware() {
    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    if (!user32) {
        return;
    }

    auto setThreadDpiAwarenessContext =
        reinterpret_cast<SetThreadDpiAwarenessContext_t>(
            GetProcAddress(user32, "SetThreadDpiAwarenessContext"));
    if (setThreadDpiAwarenessContext) {
        setThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    }
}

DWORD WINAPI WorkerThreadProc(void*) {
    MakeThreadPerMonitorDpiAware();

    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    if (Gdiplus::GdiplusStartup(&g_gdiplusToken, &gdiplusStartupInput, nullptr) !=
        Gdiplus::Ok) {
        Wh_Log(L"GDI+ initialization failed");
        return 0;
    }

    if (!RegisterWindowClasses()) {
        Wh_Log(L"Window class registration failed, error=%u", GetLastError());
        Gdiplus::GdiplusShutdown(g_gdiplusToken);
        g_gdiplusToken = 0;
        return 0;
    }

    g_controllerWindow = CreateWindowExW(
        WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE, kControllerClassName,
        L"Desktop Rounded Corners Controller", WS_POPUP,
        0, 0, 0, 0, nullptr, nullptr, GetModuleHandleW(nullptr), nullptr);

    if (!g_controllerWindow) {
        Wh_Log(L"Controller window creation failed, error=%u", GetLastError());
        Gdiplus::GdiplusShutdown(g_gdiplusToken);
        g_gdiplusToken = 0;
        return 0;
    }

    MSG message;
    while (GetMessageW(&message, nullptr, 0, 0) > 0) {
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }

    g_controllerWindow = nullptr;

    if (g_gdiplusToken) {
        Gdiplus::GdiplusShutdown(g_gdiplusToken);
        g_gdiplusToken = 0;
    }

    return 0;
}

}  // namespace

BOOL Wh_ModInit() {
    LoadSettings();

    g_workerThread = CreateThread(nullptr, 0, WorkerThreadProc, nullptr, 0,
                                  &g_workerThreadId);
    if (!g_workerThread) {
        Wh_Log(L"Worker thread creation failed, error=%u", GetLastError());
        return FALSE;
    }

    return TRUE;
}

void Wh_ModUninit() {
    if (g_controllerWindow) {
        PostMessageW(g_controllerWindow, WM_CLOSE, 0, 0);
    } else if (g_workerThreadId) {
        PostThreadMessageW(g_workerThreadId, WM_QUIT, 0, 0);
    }

    if (g_workerThread) {
        WaitForSingleObject(g_workerThread, 5000);
        CloseHandle(g_workerThread);
        g_workerThread = nullptr;
    }

    g_workerThreadId = 0;
}

void Wh_ModSettingsChanged() {
    LoadSettings();

    if (g_controllerWindow) {
        PostMessageW(g_controllerWindow, kRebuildMessage, 0, 0);
        PostMessageW(g_controllerWindow, kRefreshMessage, 0, 0);
    }
}
