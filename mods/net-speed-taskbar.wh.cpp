// ==WindhawkMod==
// @id          net-speed-taskbar
// @name        Taskbar Network Speed Indicator
// @description A free-floating network speed widget you can place anywhere along the taskbar, featuring a sparkline chart and multiple layouts.
// @version     1.1
// @author      Narayan
// @github      https://github.com/NarayanChetri
// @homepage    https://narayanchetri.dev
// @include     explorer.exe
// @compilerOptions -lgdiplus -liphlpapi
// @architecture    x86-64
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Network Speed Indicator

Unlike the existing "Taskbar Clock Customization" mod, this is a free-floating widget that can be placed anywhere along the taskbar, featuring a real-time sparkline chart and multiple distinct visual layouts. It shows your current download and upload internet speed, refreshed every second.

![Demo](https://raw.githubusercontent.com/NarayanChetri/Files/main/taskbar-speed-indicator.gif)

- Choose from different looks: Side-by-Side, Top-Down, Chart, or Minimal.
- Move the widget anywhere along your taskbar using the Horizontal Position setting.
- You can click right through it, so it won't get in the way of your taskbar buttons.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- theme: Minimal
  $name: Visual Theme
  $description: >-
    Select the widget's structural layout and style.
  $options:
    - Side-by-Side: Side-by-Side
    - Top-Down: Top-Down (Compact)
    - Chart: Chart (Sparklines)
    - Minimal: Minimal (Text Only)
- horizontalPosition: 10
  $name: Horizontal position (%)
  $description: >-
    Where the widget sits along the taskbar's width. 0 = left edge, 100 =
    right edge, 50 = centered. Enter any value 0-100.
- verticalNudge: 0
  $name: Vertical nudge (px)
  $description: Fine-tune vertical centering within the taskbar, in pixels. Positive moves down.
- updateIntervalMs: 1000
  $name: Update interval (ms)
  $description: How often the speed reading refreshes.
- fontSize: 13
  $name: Font size
- opacity: 235
  $name: Opacity (0-255)
  $description: Overall widget transparency. 255 = fully opaque.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <objbase.h>
#include <gdiplus.h>
#include <iphlpapi.h>
#include <string>
#include <algorithm>
#include <atomic>
#include <vector>
#include <memory>
#include <mutex>
#include <cstdio>
#include <windhawk_utils.h>

using namespace Gdiplus;

#define WM_UPDATE_SETTINGS (WM_APP + 1)
#define WM_UPDATE_POSITION (WM_APP + 2)

namespace {

struct Settings {
    std::wstring theme;
    int horizontalPosition;
    int verticalNudge;
    int updateIntervalMs;
    int fontSize;
    int opacity;
};

Settings g_settings;
std::mutex g_settingsMutex;

std::atomic<HWND> g_hWnd{nullptr};
std::mutex g_wndLifecycleMutex;

std::atomic<bool> g_posUpdatePending{false};
std::atomic<bool> g_exiting{false};
ULONG_PTR g_gdiplusToken = 0;
constexpr UINT_PTR kTimerId = 1001;

HANDLE g_hThread = nullptr;
DWORD g_threadId = 0;
HANDLE g_hQueueReady = nullptr;
HANDLE g_hExitEvent = nullptr;

HWINEVENTHOOK g_hLocationHook = nullptr;
HWINEVENTHOOK g_hDestroyHook = nullptr;
DWORD g_taskbarPid = 0;
DWORD g_taskbarTid = 0;
HWND g_hCachedTaskbar = nullptr;

RECT g_lastWidgetRect = {0, 0, 0, 0};

ULONGLONG g_lastTick = 0;
ULONG64 g_lastIn = 0;
ULONG64 g_lastOut = 0;
double g_downBps = 0.0;
double g_upBps = 0.0;

DWORD g_bestIfIndex = 0;
ULONGLONG g_lastRouteCheck = 0;

const size_t kHistorySize = 20;
std::vector<double> g_downHistory(kHistorySize, 0.0);
std::vector<double> g_upHistory(kHistorySize, 0.0);
size_t g_historyIdx = 0;

struct GdiObjects {
    std::unique_ptr<Gdiplus::Font> font;
    std::unique_ptr<Gdiplus::Font> largeFont;
    std::unique_ptr<Gdiplus::SolidBrush> bgBrush;
    std::unique_ptr<Gdiplus::SolidBrush> downBrush;
    std::unique_ptr<Gdiplus::SolidBrush> upBrush;
    std::unique_ptr<Gdiplus::SolidBrush> shadowBrush;
    std::unique_ptr<Gdiplus::Pen> borderPen;
    std::unique_ptr<Gdiplus::Pen> dividerPen;
    std::unique_ptr<Gdiplus::Pen> downChartPen;
    std::unique_ptr<Gdiplus::Pen> upChartPen;

    bool IsValid() const {
        return font && largeFont && bgBrush && downBrush && upBrush && 
               shadowBrush && borderPen && dividerPen && downChartPen && upChartPen;
    }
};

[[clang::no_destroy]] std::unique_ptr<GdiObjects> g_gdi;
UINT g_currentDpi = 96;

HINSTANCE GetCurrentModuleHandle() {
    HINSTANCE hInst = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | 
                       GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       (LPCWSTR)&GetCurrentModuleHandle, &hInst);
    return hInst;
}

Settings GetSafeSettings() {
    std::lock_guard<std::mutex> lock(g_settingsMutex);
    return g_settings;
}

void LoadSettingsLocked() {
    std::lock_guard<std::mutex> lock(g_settingsMutex);
    g_settings.theme = WindhawkUtils::StringSetting::make(L"theme").get();
    g_settings.horizontalPosition = std::clamp((int)Wh_GetIntSetting(L"horizontalPosition"), 0, 100);
    g_settings.verticalNudge = std::clamp((int)Wh_GetIntSetting(L"verticalNudge"), -1000, 1000);
    g_settings.updateIntervalMs = std::max((int)Wh_GetIntSetting(L"updateIntervalMs"), 200);
    g_settings.fontSize = std::max((int)Wh_GetIntSetting(L"fontSize"), 6);
    g_settings.opacity = std::clamp((int)Wh_GetIntSetting(L"opacity"), 0, 255);
}

void RebuildGdiObjects(int fontSize, const std::wstring& theme, UINT dpi) {
    auto newGdi = std::make_unique<GdiObjects>();

    Gdiplus::FontFamily fontFamily(L"Segoe UI");
    float scaledFontSize = (float)MulDiv(fontSize, dpi, 96);

    newGdi->font = std::make_unique<Gdiplus::Font>(&fontFamily, scaledFontSize, FontStyleRegular, UnitPixel);
    newGdi->largeFont = std::make_unique<Gdiplus::Font>(&fontFamily, scaledFontSize * 1.3f, FontStyleBold, UnitPixel);
    
    Color bgColor = Color(240, 18, 20, 22);
    Color downColor = Color(255, 64, 169, 255); 
    Color upColor = Color(255, 100, 230, 90);   
    
    if (theme == L"Minimal") {
        bgColor = Color(0, 0, 0, 0);
        downColor = Color(255, 255, 255, 255);
        upColor = Color(255, 200, 200, 200);
    }

    newGdi->bgBrush = std::make_unique<Gdiplus::SolidBrush>(bgColor);
    newGdi->downBrush = std::make_unique<Gdiplus::SolidBrush>(downColor);
    newGdi->upBrush = std::make_unique<Gdiplus::SolidBrush>(upColor);
    newGdi->shadowBrush = std::make_unique<Gdiplus::SolidBrush>(Color(200, 0, 0, 0));
    newGdi->borderPen = std::make_unique<Gdiplus::Pen>(Color(60, 255, 255, 255), 1.0f);
    newGdi->dividerPen = std::make_unique<Gdiplus::Pen>(Color(40, 255, 255, 255), 1.0f);
    newGdi->downChartPen = std::make_unique<Gdiplus::Pen>(downColor, 1.5f);
    newGdi->upChartPen = std::make_unique<Gdiplus::Pen>(upColor, 1.5f);
    
    newGdi->downChartPen->SetLineJoin(LineJoinRound);
    newGdi->upChartPen->SetLineJoin(LineJoinRound);

    if (newGdi->IsValid()) {
        g_gdi = std::move(newGdi);
    }
}

void GetThemeDimensions(const std::wstring& theme, int& width, int& height) {
    if (theme == L"Top-Down") {
        width = 75; height = 36;
    } else if (theme == L"Chart") {
        width = 160; height = 44;
    } else if (theme == L"Minimal") {
        width = 130; height = 20;
    } else {
        width = 140; height = 40;
    }
}

bool GetTotalOctets(ULONG64* inOctets, ULONG64* outOctets) {
    ULONGLONG now = GetTickCount64();
    if (g_bestIfIndex == 0 || (now - g_lastRouteCheck) > 10000) {
        IPAddr destAddr = 0x08080808; 
        DWORD oldIndex = g_bestIfIndex;
        if (GetBestInterface(destAddr, &g_bestIfIndex) != NO_ERROR) {
            g_bestIfIndex = 0;
        } else if (oldIndex != 0 && oldIndex != g_bestIfIndex) {
            g_lastTick = 0; 
        }
        g_lastRouteCheck = now;
    }

    if (g_bestIfIndex == 0) return false;

    MIB_IFROW row{};
    row.dwIndex = g_bestIfIndex;
    if (GetIfEntry(&row) != NO_ERROR) {
        return false;
    }

    if (row.dwOperStatus != MIB_IF_OPER_STATUS_OPERATIONAL && row.dwOperStatus != MIB_IF_OPER_STATUS_CONNECTED) {
        return false;
    }

    *inOctets = row.dwInOctets;
    *outOctets = row.dwOutOctets;
    return true;
}

std::wstring FormatSpeed(double bytesPerSec) {
    wchar_t buf[32];
    if (bytesPerSec >= 1024.0 * 1024.0) {
        swprintf_s(buf, L"%.1f MB/s", bytesPerSec / (1024.0 * 1024.0));
    } else if (bytesPerSec >= 1024.0) {
        swprintf_s(buf, L"%.0f KB/s", bytesPerSec / 1024.0);
    } else {
        swprintf_s(buf, L"%.0f B/s", bytesPerSec);
    }
    return buf;
}

void UpdateSpeedSample() {
    ULONG64 in = 0, out = 0;
    if (!GetTotalOctets(&in, &out)) {
        g_downBps = 0.0;
        g_upBps = 0.0;
        g_lastTick = 0;
    } else {
        ULONGLONG now = GetTickCount64();
        if (g_lastTick != 0) {
            double elapsedSec = (now - g_lastTick) / 1000.0;
            if (elapsedSec > 0.05) {
                g_downBps = (in >= g_lastIn) ? (in - g_lastIn) / elapsedSec : 0.0;
                g_upBps = (out >= g_lastOut) ? (out - g_lastOut) / elapsedSec : 0.0;
            }
        }
        g_lastIn = in;
        g_lastOut = out;
        g_lastTick = now;
    }

    g_downHistory[g_historyIdx] = g_downBps;
    g_upHistory[g_historyIdx] = g_upBps;
    g_historyIdx = (g_historyIdx + 1) % kHistorySize;
}

void RepositionWidget(HWND hWnd, const Settings& s) {
    if (!IsWindow(g_hCachedTaskbar) || !IsWindowVisible(g_hCachedTaskbar)) {
        if (IsWindowVisible(hWnd)) ShowWindow(hWnd, SW_HIDE);
        return;
    }

    RECT tbRect{};
    if (!GetWindowRect(g_hCachedTaskbar, &tbRect)) return;

    UINT dpi = GetDpiForWindow(g_hCachedTaskbar);
    if (dpi == 0) dpi = 96;
    
    if (dpi != g_currentDpi || !g_gdi || !g_gdi->IsValid()) {
        g_currentDpi = dpi;
        RebuildGdiObjects(s.fontSize, s.theme, dpi);
    }

    int baseW, baseH;
    GetThemeDimensions(s.theme, baseW, baseH);

    int scaledWidth = MulDiv(baseW, dpi, 96);
    int scaledHeight = MulDiv(baseH, dpi, 96);

    int tbWidth = tbRect.right - tbRect.left;
    int tbHeight = tbRect.bottom - tbRect.top;

    int usableWidth = std::max(tbWidth - scaledWidth, 0);
    int x = tbRect.left + (int)((usableWidth * s.horizontalPosition) / 100.0);
    int y = tbRect.top + (tbHeight - scaledHeight) / 2 + s.verticalNudge;

    UINT flags = SWP_NOACTIVATE | SWP_NOZORDER;
    if (!IsWindowVisible(hWnd)) flags |= SWP_SHOWWINDOW;

    RECT newRect = {x, y, x + scaledWidth, y + scaledHeight};
    if (memcmp(&g_lastWidgetRect, &newRect, sizeof(RECT)) != 0 || (flags & SWP_SHOWWINDOW)) {
        SetWindowPos(hWnd, nullptr, x, y, scaledWidth, scaledHeight, flags);
        g_lastWidgetRect = newRect;
    }
}

void AddRoundRect(GraphicsPath& path, const RectF& r, float d) {
    path.AddArc(r.X, r.Y, d, d, 180.0f, 90.0f);
    path.AddArc(r.X + r.Width - d, r.Y, d, d, 270.0f, 90.0f);
    path.AddArc(r.X + r.Width - d, r.Y + r.Height - d, d, d, 0.0f, 90.0f);
    path.AddArc(r.X, r.Y + r.Height - d, d, d, 90.0f, 90.0f);
    path.CloseFigure();
}

void DrawSparkline(Graphics& g, Pen* pen, const std::vector<double>& hist, size_t head, RectF bounds) {
    double maxVal = 1024.0; 
    for (double v : hist) maxVal = std::max(maxVal, v);
    
    PointF pts[kHistorySize];
    float step = bounds.Width / (float)(kHistorySize - 1);
    
    for (size_t i = 0; i < kHistorySize; ++i) {
        size_t idx = (head + i) % kHistorySize;
        float x = bounds.X + (float)i * step;
        float y = bounds.Y + bounds.Height - (float)((hist[idx] / maxVal) * bounds.Height);
        pts[i] = PointF(x, y);
    }
    g.DrawLines(pen, pts, kHistorySize);
}

void PaintWidget(HWND hWnd) {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);
    RECT rc;
    GetClientRect(hWnd, &rc);

    Graphics graphics(hdc);
    graphics.SetSmoothingMode(SmoothingModeAntiAlias);
    graphics.SetTextRenderingHint(TextRenderingHintClearTypeGridFit);
    graphics.Clear(Color(0, 0, 0, 0));

    if (!g_gdi || !g_gdi->IsValid()) {
        EndPaint(hWnd, &ps);
        return;
    }

    Settings s = GetSafeSettings();
    RectF bounds(0.0f, 0.0f, (float)rc.right - 1.0f, (float)rc.bottom - 1.0f);

    if (s.theme != L"Minimal") {
        GraphicsPath path;
        AddRoundRect(path, bounds, 8.0f);
        graphics.FillPath(g_gdi->bgBrush.get(), &path);
        if (g_gdi->borderPen) graphics.DrawPath(g_gdi->borderPen.get(), &path);
    }

    StringFormat format;
    float w = bounds.Width;
    float h = bounds.Height;
    std::wstring strDown = FormatSpeed(g_downBps);
    std::wstring strUp = FormatSpeed(g_upBps);

    if (s.theme == L"Side-by-Side") {
        format.SetAlignment(StringAlignmentCenter);
        format.SetLineAlignment(StringAlignmentCenter);
        float halfW = w / 2.0f;
        if (g_gdi->dividerPen) graphics.DrawLine(g_gdi->dividerPen.get(), halfW, h * 0.2f, halfW, h * 0.8f);

        graphics.DrawString(L"\x2193", -1, g_gdi->largeFont.get(), RectF(0, 0, halfW, h / 2 + 4), &format, g_gdi->downBrush.get());
        graphics.DrawString(strDown.c_str(), -1, g_gdi->font.get(), RectF(0, h / 2, halfW, h / 2 - 2), &format, g_gdi->downBrush.get());

        graphics.DrawString(L"\x2191", -1, g_gdi->largeFont.get(), RectF(halfW, 0, halfW, h / 2 + 4), &format, g_gdi->upBrush.get());
        graphics.DrawString(strUp.c_str(), -1, g_gdi->font.get(), RectF(halfW, h / 2, halfW, h / 2 - 2), &format, g_gdi->upBrush.get());

    } else if (s.theme == L"Top-Down") {
        format.SetAlignment(StringAlignmentNear);
        format.SetLineAlignment(StringAlignmentCenter);
        float halfH = h / 2.0f;
        
        graphics.DrawString(L"\x2193", -1, g_gdi->font.get(), RectF(8, 0, 16, halfH), &format, g_gdi->downBrush.get());
        graphics.DrawString(strDown.c_str(), -1, g_gdi->font.get(), RectF(22, 0, w - 22, halfH), &format, g_gdi->downBrush.get());
        
        graphics.DrawString(L"\x2191", -1, g_gdi->font.get(), RectF(8, halfH, 16, halfH), &format, g_gdi->upBrush.get());
        graphics.DrawString(strUp.c_str(), -1, g_gdi->font.get(), RectF(22, halfH, w - 22, halfH), &format, g_gdi->upBrush.get());

    } else if (s.theme == L"Chart") {
        format.SetAlignment(StringAlignmentNear);
        format.SetLineAlignment(StringAlignmentCenter);
        float halfH = h / 2.0f;
        float chartW = w * 0.40f;
        
        RectF chartDown(8.0f, 6.0f, chartW, halfH - 8.0f);
        DrawSparkline(graphics, g_gdi->downChartPen.get(), g_downHistory, g_historyIdx, chartDown);
        graphics.DrawString((L"\x2193 " + strDown).c_str(), -1, g_gdi->font.get(), RectF(chartW + 16, 0, w - chartW - 16, halfH), &format, g_gdi->downBrush.get());

        RectF chartUp(8.0f, halfH + 2.0f, chartW, halfH - 8.0f);
        DrawSparkline(graphics, g_gdi->upChartPen.get(), g_upHistory, g_historyIdx, chartUp);
        graphics.DrawString((L"\x2191 " + strUp).c_str(), -1, g_gdi->font.get(), RectF(chartW + 16, halfH, w - chartW - 16, halfH), &format, g_gdi->upBrush.get());

    } else if (s.theme == L"Minimal") {
        format.SetAlignment(StringAlignmentNear);
        format.SetLineAlignment(StringAlignmentCenter);
        float halfW = w / 2.0f;

        graphics.DrawString((L"\x2193 " + strDown).c_str(), -1, g_gdi->font.get(), RectF(1, 1, halfW, h), &format, g_gdi->shadowBrush.get());
        graphics.DrawString((L"\x2193 " + strDown).c_str(), -1, g_gdi->font.get(), RectF(0, 0, halfW, h), &format, g_gdi->downBrush.get());

        graphics.DrawString((L"\x2191 " + strUp).c_str(), -1, g_gdi->font.get(), RectF(halfW + 1, 1, halfW, h), &format, g_gdi->shadowBrush.get());
        graphics.DrawString((L"\x2191 " + strUp).c_str(), -1, g_gdi->font.get(), RectF(halfW, 0, halfW, h), &format, g_gdi->upBrush.get());
    }

    EndPaint(hWnd, &ps);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_UPDATE_SETTINGS: {
            Settings s = GetSafeSettings();
            RebuildGdiObjects(s.fontSize, s.theme, g_currentDpi);
            KillTimer(hWnd, kTimerId);
            SetTimer(hWnd, kTimerId, s.updateIntervalMs, nullptr);
            SetLayeredWindowAttributes(hWnd, 0, (BYTE)s.opacity, LWA_ALPHA);
            RepositionWidget(hWnd, s);
            InvalidateRect(hWnd, nullptr, FALSE);
            return 0;
        }
        case WM_UPDATE_POSITION: {
            g_posUpdatePending.store(false);
            RepositionWidget(hWnd, GetSafeSettings());
            InvalidateRect(hWnd, nullptr, TRUE);
            return 0;
        }
        case WM_PAINT:
            PaintWidget(hWnd);
            return 0;
        case WM_TIMER:
            if (wParam == kTimerId) {
                UpdateSpeedSample();
                if (g_hCachedTaskbar && IsWindow(g_hCachedTaskbar)) {
                    RepositionWidget(hWnd, GetSafeSettings());
                    InvalidateRect(hWnd, nullptr, FALSE);
                }
            }
            return 0;
        case WM_ERASEBKGND:
            return 1;
        case WM_CLOSE:
            KillTimer(hWnd, kTimerId);
            DestroyWindow(hWnd);
            return 0;
        case WM_NCDESTROY: {
            std::lock_guard<std::mutex> lock(g_wndLifecycleMutex);
            g_hWnd.store(nullptr);
            PostQuitMessage(0);
            return 0;
        }
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

DWORD WINAPI WidgetMessageLoop(LPVOID lpParam) {
    MSG msg;
    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);
    SetEvent(g_hQueueReady);

    WNDCLASSW wc{};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = GetCurrentModuleHandle();
    wc.hbrBackground = nullptr;
    
    wchar_t szClassName[64];
    swprintf_s(szClassName, L"WindhawkNetSpeed_%lu", GetCurrentThreadId());
    wc.lpszClassName = szClassName;
    
    if (!RegisterClassW(&wc)) {
        return 0; 
    }

    while (!g_exiting.load()) {
        HWND hTaskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
        if (!hTaskbar) {
            if (WaitForSingleObject(g_hExitEvent, 1000) == WAIT_OBJECT_0) break;
            continue;
        }

        DWORD pid = 0;
        DWORD tid = GetWindowThreadProcessId(hTaskbar, &pid);
        
        if (pid != GetCurrentProcessId()) {
            if (WaitForSingleObject(g_hExitEvent, 2000) == WAIT_OBJECT_0) break;
            continue;
        }

        g_hCachedTaskbar = hTaskbar;
        g_taskbarPid = pid;
        g_taskbarTid = tid;
        memset(&g_lastWidgetRect, 0, sizeof(RECT));

        DWORD exStyle = WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_TRANSPARENT;
        
        HWND hwnd = nullptr;
        {
            std::lock_guard<std::mutex> lock(g_wndLifecycleMutex);
            if (g_exiting.load()) break;

            hwnd = CreateWindowExW(exStyle, szClassName, L"", WS_POPUP, 0, 0,
                                   1, 1, hTaskbar, nullptr, wc.hInstance, nullptr);
            if (!hwnd) {
                if (WaitForSingleObject(g_hExitEvent, 1000) == WAIT_OBJECT_0) break;
                continue;
            }
            g_hWnd.store(hwnd);
        }
        
        PostMessageW(hwnd, WM_UPDATE_SETTINGS, 0, 0);

        g_hDestroyHook = SetWinEventHook(EVENT_OBJECT_DESTROY, EVENT_OBJECT_DESTROY, nullptr, 
            [](HWINEVENTHOOK, DWORD, HWND hwndHook, LONG idObj, LONG, DWORD, DWORD) {
                if (idObj == OBJID_WINDOW && hwndHook == g_hCachedTaskbar) {
                    g_hCachedTaskbar = nullptr;
                    HWND widget = g_hWnd.load();
                    if (widget) {
                        PostMessageW(widget, WM_CLOSE, 0, 0); 
                    }
                }
            }, pid, tid, WINEVENT_OUTOFCONTEXT);

        g_hLocationHook = SetWinEventHook(EVENT_OBJECT_LOCATIONCHANGE, EVENT_OBJECT_LOCATIONCHANGE, nullptr, 
            [](HWINEVENTHOOK, DWORD, HWND hwndHook, LONG idObj, LONG, DWORD, DWORD) {
                if (idObj == OBJID_WINDOW && hwndHook == g_hCachedTaskbar) {
                    HWND widget = g_hWnd.load();
                    if (widget && !g_posUpdatePending.exchange(true)) {
                        PostMessageW(widget, WM_UPDATE_POSITION, 0, 0);
                    }
                }
            }, pid, tid, WINEVENT_OUTOFCONTEXT);

        while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
        
        if (g_hLocationHook) { UnhookWinEvent(g_hLocationHook); g_hLocationHook = nullptr; }
        if (g_hDestroyHook) { UnhookWinEvent(g_hDestroyHook); g_hDestroyHook = nullptr; }
        
        g_hCachedTaskbar = nullptr;
    }

    UnregisterClassW(szClassName, wc.hInstance);
    return 0;
}

}  // namespace

BOOL Wh_ModInit() {
    LoadSettingsLocked();

    GdiplusStartupInput gdiplusStartupInput;
    if (GdiplusStartup(&g_gdiplusToken, &gdiplusStartupInput, nullptr) != Ok) {
        return FALSE;
    }

    g_exiting.store(false);
    g_hExitEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_hQueueReady = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_hThread = CreateThread(nullptr, 0, WidgetMessageLoop, nullptr, 0, &g_threadId);

    return g_hThread != nullptr;
}

void Wh_ModUninit() {
    g_exiting.store(true);
    if (g_hExitEvent) {
        SetEvent(g_hExitEvent);
    }

    if (g_hThread) {
        WaitForSingleObject(g_hQueueReady, INFINITE); 
        
        {
            std::lock_guard<std::mutex> lock(g_wndLifecycleMutex);
            HWND hwnd = g_hWnd.load();
            if (hwnd) {
                PostMessageW(hwnd, WM_CLOSE, 0, 0);
            }
        }
        PostThreadMessageW(g_threadId, WM_QUIT, 0, 0); 
        
        WaitForSingleObject(g_hThread, INFINITE);      
        CloseHandle(g_hThread);
        g_hThread = nullptr;
        g_threadId = 0;
    }

    if (g_hQueueReady) {
        CloseHandle(g_hQueueReady);
        g_hQueueReady = nullptr;
    }

    if (g_hExitEvent) {
        CloseHandle(g_hExitEvent);
        g_hExitEvent = nullptr;
    }

    g_gdi.reset();

    if (g_gdiplusToken) {
        GdiplusShutdown(g_gdiplusToken);
        g_gdiplusToken = 0;
    }
}

void Wh_ModSettingsChanged() {
    LoadSettingsLocked();
    HWND hwnd = g_hWnd.load();
    if (hwnd) PostMessageW(hwnd, WM_UPDATE_SETTINGS, 0, 0);
}