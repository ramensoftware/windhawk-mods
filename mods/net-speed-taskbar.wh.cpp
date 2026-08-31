// ==WindhawkMod==
// @id          net-speed-taskbar
// @name        Taskbar Network Speed Indicator
// @description Shows live download/upload speed near the taskbar.
// @version     1.0
// @author      Narayan
// @github      https://github.com/NarayanChetri
// @homepage    https://narayanchetri.dev
// @include     explorer.exe
// @compilerOptions -lgdiplus -liphlpapi -lgdi32 -luser32 -lws2_32
// @architecture    x86-64
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Network Speed Indicator

Draws a small floating widget docked to the taskbar that shows your current
download and upload internet speed, refreshed every second.

![Demo](https://raw.githubusercontent.com/NarayanChetri/Files/main/taskbar-speed-indicator.gif)

- Move the widget anywhere along your taskbar using the Horizontal Position setting.
- You can click right through it, so it won't get in the way of your taskbar buttons.
- Smoothly follows your taskbar if you have auto-hide enabled.
- Always stays visible on top of the taskbar.
- Shows your actual internet speed, ignoring background virtual networks or VPN clutter.
- Choose from different looks: Side-by-Side, Top-Down, Chart, or Minimal.
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
- horizontalPosition: 7
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

#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")

#include <winsock2.h>
#include <ws2ipdef.h>
#include <windows.h>
#include <gdiplus.h>
#include <iphlpapi.h>
#include <netioapi.h>
#include <string>
#include <algorithm>
#include <atomic>
#include <vector>

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
CRITICAL_SECTION g_settingsLock;

std::atomic<HWND> g_hWnd{nullptr};
std::atomic<bool> g_posUpdatePending{false};
ULONG_PTR g_gdiplusToken = 0;
UINT_PTR g_timerId = 1001;

HANDLE g_hThread = nullptr;
DWORD g_threadId = 0;
HWINEVENTHOOK g_hLocationHook = nullptr;

ULONGLONG g_lastTick = 0;
ULONG64 g_lastIn = 0;
ULONG64 g_lastOut = 0;
double g_downBps = 0.0;
double g_upBps = 0.0;

DWORD g_bestIfIndex = 0;
ULONGLONG g_lastRouteCheck = 0;

const wchar_t* kClassName = L"WindhawkNetSpeedWidgetWnd";

const size_t kHistorySize = 20;
std::vector<double> g_downHistory(kHistorySize, 0.0);
std::vector<double> g_upHistory(kHistorySize, 0.0);
size_t g_historyIdx = 0;

Gdiplus::Font* g_font = nullptr;
Gdiplus::Font* g_largeFont = nullptr;
Gdiplus::SolidBrush* g_bgBrush = nullptr;
Gdiplus::SolidBrush* g_downBrush = nullptr;
Gdiplus::SolidBrush* g_upBrush = nullptr;
Gdiplus::SolidBrush* g_shadowBrush = nullptr;
Gdiplus::Pen* g_borderPen = nullptr;
Gdiplus::Pen* g_dividerPen = nullptr;
Gdiplus::Pen* g_downChartPen = nullptr;
Gdiplus::Pen* g_upChartPen = nullptr;

Settings GetSafeSettings() {
    Settings s;
    EnterCriticalSection(&g_settingsLock);
    s = g_settings;
    LeaveCriticalSection(&g_settingsLock);
    return s;
}

void LoadSettingsLocked() {
    EnterCriticalSection(&g_settingsLock);
    
    PCWSTR pTheme = Wh_GetStringSetting(L"theme");
    g_settings.theme = pTheme ? pTheme : L"Minimal";
    if (pTheme) Wh_FreeStringSetting(pTheme);
    
    g_settings.horizontalPosition = std::clamp((int)Wh_GetIntSetting(L"horizontalPosition"), 0, 100);
    g_settings.verticalNudge = (int)Wh_GetIntSetting(L"verticalNudge");
    g_settings.updateIntervalMs = std::max((int)Wh_GetIntSetting(L"updateIntervalMs"), 200);
    g_settings.fontSize = std::max((int)Wh_GetIntSetting(L"fontSize"), 6);
    g_settings.opacity = std::clamp((int)Wh_GetIntSetting(L"opacity"), 0, 255);
    LeaveCriticalSection(&g_settingsLock);
}

void RebuildGdiObjects(int fontSize, const std::wstring& theme) {
    delete g_font;
    delete g_largeFont;
    delete g_bgBrush;
    delete g_downBrush;
    delete g_upBrush;
    delete g_shadowBrush;
    delete g_borderPen;
    delete g_dividerPen;
    delete g_downChartPen;
    delete g_upChartPen;

    Gdiplus::FontFamily fontFamily(L"Segoe UI");
    g_font = new Gdiplus::Font(&fontFamily, (Gdiplus::REAL)fontSize, FontStyleRegular, UnitPixel);
    g_largeFont = new Gdiplus::Font(&fontFamily, (Gdiplus::REAL)fontSize * 1.3f, FontStyleBold, UnitPixel);
    
    Color bgColor = Color(240, 18, 20, 22);
    Color downColor = Color(255, 64, 169, 255); 
    Color upColor = Color(255, 100, 230, 90);   
    
    if (theme == L"Minimal") {
        bgColor = Color(0, 0, 0, 0);
        downColor = Color(255, 255, 255, 255);
        upColor = Color(255, 200, 200, 200);
    }

    g_bgBrush = new Gdiplus::SolidBrush(bgColor);
    g_downBrush = new Gdiplus::SolidBrush(downColor);
    g_upBrush = new Gdiplus::SolidBrush(upColor);
    g_shadowBrush = new Gdiplus::SolidBrush(Color(200, 0, 0, 0));
    g_borderPen = new Gdiplus::Pen(Color(60, 255, 255, 255), 1.0f);
    g_dividerPen = new Gdiplus::Pen(Color(40, 255, 255, 255), 1.0f);
    g_downChartPen = new Gdiplus::Pen(downColor, 1.5f);
    g_upChartPen = new Gdiplus::Pen(upColor, 1.5f);
    
    g_downChartPen->SetLineJoin(LineJoinRound);
    g_upChartPen->SetLineJoin(LineJoinRound);
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
        IPAddr destAddr = inet_addr("8.8.8.8");
        GetBestInterface(destAddr, &g_bestIfIndex);
        g_lastRouteCheck = now;
    }

    PMIB_IF_TABLE2 table = nullptr;
    if (GetIfTable2(&table) != NO_ERROR) {
        return false;
    }

    ULONG64 in = 0, out = 0;
    for (ULONG i = 0; i < table->NumEntries; i++) {
        const MIB_IF_ROW2& row = table->Table[i];
        if (g_bestIfIndex != 0 && row.InterfaceIndex != g_bestIfIndex) continue;
        if (row.Type == IF_TYPE_SOFTWARE_LOOPBACK || row.Type == IF_TYPE_TUNNEL) continue;
        if (row.OperStatus != IfOperStatusUp) continue;
        in += row.InOctets;
        out += row.OutOctets;
    }

    FreeMibTable(table);
    *inOctets = in;
    *outOctets = out;
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
    if (!GetTotalOctets(&in, &out)) return;

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

    g_downHistory[g_historyIdx] = g_downBps;
    g_upHistory[g_historyIdx] = g_upBps;
    g_historyIdx = (g_historyIdx + 1) % kHistorySize;
}

void RepositionWidget(HWND hWnd, const Settings& s) {
    static HWND hTaskbar = nullptr;
    if (!hTaskbar || !IsWindow(hTaskbar)) {
        hTaskbar = FindWindow(L"Shell_TrayWnd", nullptr);
    }

    if (!hTaskbar) {
        if (IsWindowVisible(hWnd)) ShowWindow(hWnd, SW_HIDE);
        return;
    }

    if (GetWindow(hWnd, GW_OWNER) != hTaskbar) {
        SetWindowLongPtr(hWnd, GWLP_HWNDPARENT, (LONG_PTR)hTaskbar);
    }

    if (!IsWindowVisible(hTaskbar)) {
        if (IsWindowVisible(hWnd)) ShowWindow(hWnd, SW_HIDE);
        return;
    }

    RECT tbRect{};
    GetWindowRect(hTaskbar, &tbRect);

    int tbWidth = tbRect.right - tbRect.left;
    int tbHeight = tbRect.bottom - tbRect.top;

    int baseW, baseH;
    GetThemeDimensions(s.theme, baseW, baseH);

    UINT dpi = GetDpiForWindow(hTaskbar);
    if (dpi == 0) dpi = 96;
    int scaledWidth = MulDiv(baseW, dpi, 96);
    int scaledHeight = MulDiv(baseH, dpi, 96);

    int usableWidth = std::max(tbWidth - scaledWidth, 0);
    int x = tbRect.left + (int)((usableWidth * s.horizontalPosition) / 100.0);
    int y = tbRect.top + (tbHeight - scaledHeight) / 2 + s.verticalNudge;

    UINT flags = SWP_NOACTIVATE | SWP_NOZORDER;
    if (!IsWindowVisible(hWnd)) flags |= SWP_SHOWWINDOW;

    SetWindowPos(hWnd, nullptr, x, y, scaledWidth, scaledHeight, flags);
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
    
    std::vector<PointF> pts;
    pts.reserve(hist.size());
    float step = bounds.Width / (float)(hist.size() - 1);
    
    for (size_t i = 0; i < hist.size(); ++i) {
        size_t idx = (head + i) % hist.size();
        float x = bounds.X + (float)i * step;
        float y = bounds.Y + bounds.Height - (float)((hist[idx] / maxVal) * bounds.Height);
        pts.push_back(PointF(x, y));
    }
    g.DrawLines(pen, pts.data(), pts.size());
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

    if (!g_font || !g_largeFont || !g_bgBrush) {
        EndPaint(hWnd, &ps);
        return;
    }

    Settings s = GetSafeSettings();
    RectF bounds(0.0f, 0.0f, (float)rc.right - 1.0f, (float)rc.bottom - 1.0f);

    if (s.theme != L"Minimal") {
        GraphicsPath path;
        AddRoundRect(path, bounds, 8.0f);
        graphics.FillPath(g_bgBrush, &path);
        if (g_borderPen) graphics.DrawPath(g_borderPen, &path);
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
        if (g_dividerPen) graphics.DrawLine(g_dividerPen, halfW, h * 0.2f, halfW, h * 0.8f);

        graphics.DrawString(L"\x2193", -1, g_largeFont, RectF(0, 0, halfW, h / 2 + 4), &format, g_downBrush);
        graphics.DrawString(strDown.c_str(), -1, g_font, RectF(0, h / 2, halfW, h / 2 - 2), &format, g_downBrush);

        graphics.DrawString(L"\x2191", -1, g_largeFont, RectF(halfW, 0, halfW, h / 2 + 4), &format, g_upBrush);
        graphics.DrawString(strUp.c_str(), -1, g_font, RectF(halfW, h / 2, halfW, h / 2 - 2), &format, g_upBrush);

    } else if (s.theme == L"Top-Down") {
        format.SetAlignment(StringAlignmentNear);
        format.SetLineAlignment(StringAlignmentCenter);
        float halfH = h / 2.0f;
        
        graphics.DrawString(L"\x2193", -1, g_font, RectF(8, 0, 16, halfH), &format, g_downBrush);
        graphics.DrawString(strDown.c_str(), -1, g_font, RectF(22, 0, w - 22, halfH), &format, g_downBrush);
        
        graphics.DrawString(L"\x2191", -1, g_font, RectF(8, halfH, 16, halfH), &format, g_upBrush);
        graphics.DrawString(strUp.c_str(), -1, g_font, RectF(22, halfH, w - 22, halfH), &format, g_upBrush);

    } else if (s.theme == L"Chart") {
        format.SetAlignment(StringAlignmentNear);
        format.SetLineAlignment(StringAlignmentCenter);
        float halfH = h / 2.0f;
        float chartW = w * 0.40f;
        
        RectF chartDown(8.0f, 6.0f, chartW, halfH - 8.0f);
        DrawSparkline(graphics, g_downChartPen, g_downHistory, g_historyIdx, chartDown);
        graphics.DrawString((L"\x2193 " + strDown).c_str(), -1, g_font, RectF(chartW + 16, 0, w - chartW - 16, halfH), &format, g_downBrush);

        RectF chartUp(8.0f, halfH + 2.0f, chartW, halfH - 8.0f);
        DrawSparkline(graphics, g_upChartPen, g_upHistory, g_historyIdx, chartUp);
        graphics.DrawString((L"\x2191 " + strUp).c_str(), -1, g_font, RectF(chartW + 16, halfH, w - chartW - 16, halfH), &format, g_upBrush);

    } else if (s.theme == L"Minimal") {
        format.SetAlignment(StringAlignmentNear);
        format.SetLineAlignment(StringAlignmentCenter);
        float halfW = w / 2.0f;

        graphics.DrawString((L"\x2193 " + strDown).c_str(), -1, g_font, RectF(1, 1, halfW, h), &format, g_shadowBrush);
        graphics.DrawString((L"\x2193 " + strDown).c_str(), -1, g_font, RectF(0, 0, halfW, h), &format, g_downBrush);

        graphics.DrawString((L"\x2191 " + strUp).c_str(), -1, g_font, RectF(halfW + 1, 1, halfW, h), &format, g_shadowBrush);
        graphics.DrawString((L"\x2191 " + strUp).c_str(), -1, g_font, RectF(halfW, 0, halfW, h), &format, g_upBrush);
    }

    EndPaint(hWnd, &ps);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_UPDATE_SETTINGS: {
            Settings s = GetSafeSettings();
            RebuildGdiObjects(s.fontSize, s.theme);
            KillTimer(hWnd, g_timerId);
            SetTimer(hWnd, g_timerId, s.updateIntervalMs, nullptr);
            SetLayeredWindowAttributes(hWnd, 0, (BYTE)s.opacity, LWA_ALPHA);
            RepositionWidget(hWnd, s);
            InvalidateRect(hWnd, nullptr, TRUE);
            return 0;
        }
        case WM_UPDATE_POSITION:
            g_posUpdatePending.store(false);
            RepositionWidget(hWnd, GetSafeSettings());
            return 0;
        case WM_PAINT:
            PaintWidget(hWnd);
            return 0;
        case WM_TIMER:
            if (wParam == g_timerId) {
                UpdateSpeedSample();
                RepositionWidget(hWnd, GetSafeSettings());
                InvalidateRect(hWnd, nullptr, TRUE);
            }
            return 0;
        case WM_ERASEBKGND:
            return 1;
        case WM_CLOSE:
            KillTimer(hWnd, g_timerId);
            DestroyWindow(hWnd);
            return 0;
        case WM_DESTROY:
            delete g_font; delete g_largeFont; delete g_bgBrush;
            delete g_downBrush; delete g_upBrush; delete g_shadowBrush;
            delete g_borderPen; delete g_dividerPen;
            delete g_downChartPen; delete g_upChartPen;
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

void CALLBACK TaskbarEventProc(HWINEVENTHOOK hook, DWORD event, HWND hwnd, LONG idObj, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime) {
    if (idObj == OBJID_WINDOW) {
        static HWND hTaskbar = nullptr;
        if (!hTaskbar || !IsWindow(hTaskbar)) {
            hTaskbar = FindWindow(L"Shell_TrayWnd", nullptr);
        }
        if (hwnd == hTaskbar) {
            if (event == EVENT_OBJECT_DESTROY) {
                hTaskbar = nullptr;
                HWND widget = g_hWnd.load();
                if (widget) ShowWindow(widget, SW_HIDE);
            } else if (event == EVENT_OBJECT_LOCATIONCHANGE) {
                HWND widget = g_hWnd.load();
                if (widget && !g_posUpdatePending.exchange(true)) {
                    PostMessage(widget, WM_UPDATE_POSITION, 0, 0);
                }
            }
        }
    }
}

void CreateWidgetWindow() {
    WNDCLASS wc{};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.lpszClassName = kClassName;
    wc.hbrBackground = nullptr;
    
    if (!RegisterClass(&wc) && GetLastError() != ERROR_CLASS_ALREADY_EXISTS) {
        return;
    }

    DWORD exStyle = WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_TRANSPARENT;
    HWND hTaskbar = FindWindow(L"Shell_TrayWnd", nullptr);

    HWND hwnd = CreateWindowEx(exStyle, kClassName, L"", WS_POPUP, 0, 0,
                               1, 1, hTaskbar, nullptr, GetModuleHandle(nullptr), nullptr);

    if (!hwnd) return;
    
    g_hWnd.store(hwnd);
    PostMessage(hwnd, WM_UPDATE_SETTINGS, 0, 0);
}

DWORD WINAPI WidgetMessageLoop(LPVOID lpParam) {
    CreateWidgetWindow();

    g_hLocationHook = SetWinEventHook(EVENT_OBJECT_DESTROY, EVENT_OBJECT_LOCATIONCHANGE,
                                      nullptr, TaskbarEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    if (g_hLocationHook) UnhookWinEvent(g_hLocationHook);
    g_hWnd.store(nullptr);
    return 0;
}

}  // namespace

BOOL Wh_ModInit() {
    InitializeCriticalSection(&g_settingsLock);
    LoadSettingsLocked();

    GdiplusStartupInput gdiplusStartupInput;
    GdiplusStartup(&g_gdiplusToken, &gdiplusStartupInput, nullptr);

    UpdateSpeedSample();
    g_hThread = CreateThread(nullptr, 0, WidgetMessageLoop, nullptr, 0, &g_threadId);

    return TRUE;
}

void Wh_ModUninit() {
    if (g_threadId) PostThreadMessage(g_threadId, WM_QUIT, 0, 0);
    
    if (g_hThread) {
        if (WaitForSingleObject(g_hThread, 2000) == WAIT_TIMEOUT) {
            TerminateThread(g_hThread, 0);
        }
        CloseHandle(g_hThread);
        g_hThread = nullptr;
        g_threadId = 0;
    }

    UnregisterClass(kClassName, GetModuleHandle(nullptr));
    if (g_gdiplusToken) {
        GdiplusShutdown(g_gdiplusToken);
        g_gdiplusToken = 0;
    }
    DeleteCriticalSection(&g_settingsLock);
}

void Wh_ModSettingsChanged() {
    LoadSettingsLocked();
    HWND hwnd = g_hWnd.load();
    if (hwnd) PostMessage(hwnd, WM_UPDATE_SETTINGS, 0, 0);
}