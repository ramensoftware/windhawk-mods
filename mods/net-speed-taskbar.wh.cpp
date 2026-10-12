// ==WindhawkMod==
// @id          net-speed-taskbar
// @name        Taskbar Network Speed Indicator
// @description A free-floating network speed widget you can place anywhere along the taskbar, featuring a sparkline chart and multiple layouts.
// @version     1.6
// @author      Narayan
// @github      https://github.com/NarayanChetri
// @homepage    https://narayanchetri.dev
// @include     explorer.exe
// @compilerOptions -lgdiplus -liphlpapi -lgdi32 -lshell32 -DWIN32_LEAN_AND_MEAN
// @architecture    x86-64
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Network Speed Indicator

A free-floating widget docked to the taskbar, featuring a real-time sparkline
chart and multiple visual layouts. Shows current download/upload speed,
refreshed every second.

Runs as a dedicated, isolated `explorer.exe` helper process (not the real
shell) using the "mods as tools" pattern, so a bug in it can't take down your
actual desktop. It targets `explorer.exe` rather than `windhawk.exe` because
the taskbar's own content lives in DWM's immersive Z-band
(`ZBID_IMMERSIVE_NOTIFICATION`), and `CreateWindowInBand` -- the only way to
place a window in that band -- appears to require the calling process to
actually be an `explorer.exe` image.

![Demo](https://raw.githubusercontent.com/NarayanChetri/Files/main/taskbar-internet-speed-mod.gif)

- Choose from different looks: Side-by-Side, Top-Down, Chart, or Minimal.
- Move the widget anywhere along your taskbar using the Horizontal Position setting.
- Click-through: it won't get in the way of your taskbar buttons.
- Recovers automatically on an Explorer crash/restart, not just a clean shell exit.
- Hides while a full-screen app or game covers the taskbar's monitor.

Note: [Taskbar Clock Customization](https://windhawk.net/mods/taskbar-clock-customization)
and [Taskbar System Info](https://windhawk.net/mods/taskbar-system-info) both
offer similar taskbar network readouts. This mod's differentiators are the
free horizontal placement anywhere along the taskbar and the sparkline chart
layouts.

A few consequences of running as a dedicated `explorer.exe` helper process,
worth knowing:

- A second **"Windows Explorer"** entry appears in Task Manager. Ending it
  won't relaunch the widget until the shell restarts or the mod is reloaded.
- Any other mod you have enabled with `@include explorer.exe` gets injected
  into this helper process too, since it genuinely is an `explorer.exe`. Most
  mods no-op there, but it can make Windhawk's per-process mod list and crash
  attribution look busier than expected.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- theme: Minimal
  $name: Visual Theme
  $options:
    - Side-by-Side: Side-by-Side
    - Top-Down: Top-Down (Compact)
    - Chart: Chart (Sparklines)
    - Minimal: Minimal (Text Only)
- horizontalPosition: 10
  $name: Horizontal position (%)
- verticalNudge: 0
  $name: Vertical nudge (px)
- updateIntervalMs: 1000
  $name: Update interval (ms)
- fontSize: 13
  $name: Font size
- opacity: 235
  $name: Opacity (0-255)
*/
// ==/WindhawkModSettings==

// iphlpapi.h only declares the netioapi APIs (GetIfEntry2 / MIB_IF_ROW2)
// once winsock2 types are already in scope. -DWIN32_LEAN_AND_MEAN in
// @compilerOptions stops windows.h from pulling in the legacy winsock.h,
// which is what actually avoids the winsock2.h/windows.h ordering warning.
// That flag also drops windows.h's normal auto-includes of shellapi.h
// (CommandLineToArgvW) and ole2.h (PROPID, via wtypes.h), so both are
// pulled in explicitly below.
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windhawk_utils.h>
#include <windows.h>
#include <shellapi.h>
#include <wtypes.h>
#include <gdiplus.h>
#include <iphlpapi.h>
#include <string>
#include <algorithm>
#include <atomic>
#include <vector>
#include <memory>
#include <mutex>
#include <cstdio>

using namespace Gdiplus;

#define WM_UPDATE_SETTINGS (WM_APP + 1)
#define WM_UPDATE_POSITION (WM_APP + 2)

namespace {

// ----------------------------------------------------------------------
// CreateWindowInBand: undocumented user32.dll export. Places a window in
// a specific DWM composition Z-band. ZBID_IMMERSIVE_NOTIFICATION is the
// band Shell_TrayWnd's own content lives in on Windows 11 -- plain
// HWND_TOPMOST windows (even from a separate process) live in a lower
// band and cannot render over the taskbar's own surface. This API is
// undocumented and not guaranteed stable across builds, so it's loaded
// dynamically with a safe fallback.
// ----------------------------------------------------------------------
enum ZBID {
    ZBID_DEFAULT = 0,
    ZBID_DESKTOP = 1,
    ZBID_UIACCESS = 2,
    ZBID_IMMERSIVE_IHM = 3,
    ZBID_IMMERSIVE_NOTIFICATION = 4,
    ZBID_IMMERSIVE_APPCHROME = 5,
    ZBID_IMMERSIVE_MOGO = 6,
    ZBID_IMMERSIVE_EDGY = 7,
    ZBID_IMMERSIVE_INACTIVEMOBODY = 8,
    ZBID_IMMERSIVE_INACTIVEDOCK = 9,
    ZBID_IMMERSIVE_ACTIVEMOBODY = 10,
    ZBID_IMMERSIVE_ACTIVEDOCK = 11,
    ZBID_IMMERSIVE_BACKGROUND = 12,
    ZBID_IMMERSIVE_SEARCH = 13,
    ZBID_GENUINE_WINDOWS = 14,
    ZBID_IMMERSIVE_RESTRICTED = 15,
    ZBID_SYSTEM_TOOLS = 16,
    ZBID_LOCK = 17,
    ZBID_ABOVELOCK_UX = 18,
};

typedef HWND(WINAPI* CreateWindowInBand_t)(
    DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle,
    int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu,
    HINSTANCE hInstance, LPVOID lpParam, DWORD dwBand);

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
std::atomic<bool> g_usingBandedWindow{false};
ULONG_PTR g_gdiplusToken = 0;
constexpr UINT_PTR kTimerId = 1001;

HANDLE g_hThread = nullptr;
DWORD g_threadId = 0;
HANDLE g_hQueueReady = nullptr;
HANDLE g_hExitEvent = nullptr;

HWINEVENTHOOK g_hLocationHook = nullptr;
HWINEVENTHOOK g_hDestroyHook = nullptr;
HWND g_hCachedTaskbar = nullptr;

RECT g_lastWidgetRect = {0, 0, 0, 0};
int g_currentWidgetWidth = 0;
int g_currentWidgetHeight = 0;

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

void LoadSettings() {
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

    float sc = dpi / 96.0f;
    newGdi->bgBrush = std::make_unique<Gdiplus::SolidBrush>(bgColor);
    newGdi->downBrush = std::make_unique<Gdiplus::SolidBrush>(downColor);
    newGdi->upBrush = std::make_unique<Gdiplus::SolidBrush>(upColor);
    newGdi->shadowBrush = std::make_unique<Gdiplus::SolidBrush>(Color(200, 0, 0, 0));
    newGdi->borderPen = std::make_unique<Gdiplus::Pen>(Color(60, 255, 255, 255), 1.0f * sc);
    newGdi->dividerPen = std::make_unique<Gdiplus::Pen>(Color(40, 255, 255, 255), 1.0f * sc);
    newGdi->downChartPen = std::make_unique<Gdiplus::Pen>(downColor, 1.5f * sc);
    newGdi->upChartPen = std::make_unique<Gdiplus::Pen>(upColor, 1.5f * sc);

    newGdi->downChartPen->SetLineJoin(LineJoinRound);
    newGdi->upChartPen->SetLineJoin(LineJoinRound);

    if (newGdi->IsValid()) {
        g_gdi = std::move(newGdi);
    } else {
        Wh_Log(L"RebuildGdiObjects: one or more GDI+ objects failed to construct");
    }
}

void GetThemeDimensions(const std::wstring& theme, int fontSize, int& width, int& height) {
    if (theme == L"Top-Down") {
        width = 75; height = 36;
    } else if (theme == L"Chart") {
        width = 160; height = 44;
    } else if (theme == L"Minimal") {
        width = 130; height = 20;
    } else {
        width = 140; height = 40;
    }

    // Base dimensions above assume the default 13px font; widen/heighten
    // proportionally so a larger "Font size" setting doesn't push text
    // outside a fixed box.
    float fontScale = fontSize / 13.0f;
    width = (int)(width * fontScale);
    height = (int)(height * fontScale);
}

// Uses GetIfEntry2 / MIB_IF_ROW2 (64-bit counters), so no 32-bit-wrap
// emulation is needed. A counter reset (adapter toggle, driver reset,
// resume from sleep) is instead caught in UpdateSpeedSample by comparing
// against the previous sample.
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

    MIB_IF_ROW2 row{};
    row.InterfaceIndex = g_bestIfIndex;
    if (GetIfEntry2(&row) != NO_ERROR || row.OperStatus != IfOperStatusUp) {
        return false;
    }

    *inOctets = row.InOctets;
    *outOctets = row.OutOctets;
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
    ULONG64 rawIn = 0, rawOut = 0;
    if (!GetTotalOctets(&rawIn, &rawOut)) {
        g_downBps = 0.0;
        g_upBps = 0.0;
        g_lastTick = 0;
    } else {
        ULONGLONG now = GetTickCount64();
        // A raw counter smaller than the previous sample means the adapter's
        // counters were reset, not that they wrapped (64-bit wrap isn't a
        // practical concern). Report 0 for that sample and resync.
        bool discontinuity = (g_lastTick == 0) || (rawIn < g_lastIn) || (rawOut < g_lastOut);

        if (!discontinuity) {
            double elapsedSec = (now - g_lastTick) / 1000.0;
            if (elapsedSec > 0.05) {
                g_downBps = (double)(rawIn - g_lastIn) / elapsedSec;
                g_upBps = (double)(rawOut - g_lastOut) / elapsedSec;
            }
        } else {
            g_downBps = 0.0;
            g_upBps = 0.0;
        }

        g_lastIn = rawIn;
        g_lastOut = rawOut;
        g_lastTick = now;
    }

    g_downHistory[g_historyIdx] = g_downBps;
    g_upHistory[g_historyIdx] = g_upBps;
    g_historyIdx = (g_historyIdx + 1) % kHistorySize;
}

// True when the foreground window exactly covers the taskbar's monitor,
// which is Explorer's own signal that a full-screen app/game is active
// (Shell_TrayWnd stays IsWindowVisible==TRUE, just pushed down in Z-order).
bool IsFullScreenBlockingTaskbar(HWND hTaskbar) {
    HWND fg = GetForegroundWindow();
    if (!fg || fg == GetShellWindow()) return false;

    HMONITOR mon = MonitorFromWindow(hTaskbar, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi{sizeof(mi)};
    RECT fgRect{};
    if (!GetMonitorInfo(mon, &mi) || !GetWindowRect(fg, &fgRect)) return false;

    return EqualRect(&fgRect, &mi.rcMonitor);
}

void RepositionWidget(HWND hWnd, const Settings& s) {
    if (!IsWindow(g_hCachedTaskbar) || !IsWindowVisible(g_hCachedTaskbar) ||
        IsFullScreenBlockingTaskbar(g_hCachedTaskbar)) {
        if (IsWindowVisible(hWnd)) ShowWindow(hWnd, SW_HIDE);
        return;
    }

    RECT tbRect{};
    if (!GetWindowRect(g_hCachedTaskbar, &tbRect)) {
        Wh_Log(L"RepositionWidget: GetWindowRect on taskbar failed, gle=%lu", GetLastError());
        return;
    }

    UINT dpi = GetDpiForWindow(g_hCachedTaskbar);
    if (dpi == 0) dpi = 96;

    if (dpi != g_currentDpi || !g_gdi || !g_gdi->IsValid()) {
        g_currentDpi = dpi;
        RebuildGdiObjects(s.fontSize, s.theme, dpi);
    }

    int baseW, baseH;
    GetThemeDimensions(s.theme, s.fontSize, baseW, baseH);

    int scaledWidth = MulDiv(baseW, dpi, 96);
    int scaledHeight = MulDiv(baseH, dpi, 96);
    g_currentWidgetWidth = scaledWidth;
    g_currentWidgetHeight = scaledHeight;

    int tbWidth = tbRect.right - tbRect.left;
    int tbHeight = tbRect.bottom - tbRect.top;

    int usableWidth = std::max(tbWidth - scaledWidth, 0);
    int x = tbRect.left + (int)((usableWidth * s.horizontalPosition) / 100.0);
    int y = tbRect.top + (tbHeight - scaledHeight) / 2 + s.verticalNudge;

    UINT flags = SWP_NOACTIVATE;
    if (!IsWindowVisible(hWnd)) flags |= SWP_SHOWWINDOW;

    RECT newRect = {x, y, x + scaledWidth, y + scaledHeight};
    HWND zOrderTarget = g_usingBandedWindow.load() ? HWND_TOP : HWND_TOPMOST;
    if (memcmp(&g_lastWidgetRect, &newRect, sizeof(RECT)) != 0 || (flags & SWP_SHOWWINDOW)) {
        if (!SetWindowPos(hWnd, zOrderTarget, x, y, scaledWidth, scaledHeight, flags)) {
            Wh_Log(L"RepositionWidget: SetWindowPos failed, gle=%lu", GetLastError());
        }
        g_lastWidgetRect = newRect;
    } else {
        // Re-assert Z-order every tick even when the rect hasn't moved --
        // Explorer raises Shell_TrayWnd within its own band on auto-hide
        // reveal, ABM_ACTIVATE, and returning from full-screen, which would
        // otherwise leave the widget stuck behind the taskbar.
        SetWindowPos(hWnd, zOrderTarget, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
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

void PaintWidgetContent(Graphics& graphics, int width, int height, const Settings& s, UINT dpi) {
    RectF bounds(0.0f, 0.0f, (float)width - 1.0f, (float)height - 1.0f);
    float sc = dpi / 96.0f;

    if (s.theme != L"Minimal") {
        GraphicsPath path;
        AddRoundRect(path, bounds, 8.0f * sc);
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

        graphics.DrawString(L"\x2193", -1, g_gdi->largeFont.get(), RectF(0, 0, halfW, h / 2 + 4 * sc), &format, g_gdi->downBrush.get());
        graphics.DrawString(strDown.c_str(), -1, g_gdi->font.get(), RectF(0, h / 2, halfW, h / 2 - 2 * sc), &format, g_gdi->downBrush.get());

        graphics.DrawString(L"\x2191", -1, g_gdi->largeFont.get(), RectF(halfW, 0, halfW, h / 2 + 4 * sc), &format, g_gdi->upBrush.get());
        graphics.DrawString(strUp.c_str(), -1, g_gdi->font.get(), RectF(halfW, h / 2, halfW, h / 2 - 2 * sc), &format, g_gdi->upBrush.get());

    } else if (s.theme == L"Top-Down") {
        format.SetAlignment(StringAlignmentNear);
        format.SetLineAlignment(StringAlignmentCenter);
        float halfH = h / 2.0f;

        graphics.DrawString(L"\x2193", -1, g_gdi->font.get(), RectF(8 * sc, 0, 16 * sc, halfH), &format, g_gdi->downBrush.get());
        graphics.DrawString(strDown.c_str(), -1, g_gdi->font.get(), RectF(22 * sc, 0, w - 22 * sc, halfH), &format, g_gdi->downBrush.get());

        graphics.DrawString(L"\x2191", -1, g_gdi->font.get(), RectF(8 * sc, halfH, 16 * sc, halfH), &format, g_gdi->upBrush.get());
        graphics.DrawString(strUp.c_str(), -1, g_gdi->font.get(), RectF(22 * sc, halfH, w - 22 * sc, halfH), &format, g_gdi->upBrush.get());

    } else if (s.theme == L"Chart") {
        format.SetAlignment(StringAlignmentNear);
        format.SetLineAlignment(StringAlignmentCenter);
        float halfH = h / 2.0f;
        float chartW = w * 0.40f;

        RectF chartDown(8.0f * sc, 6.0f * sc, chartW, halfH - 8.0f * sc);
        DrawSparkline(graphics, g_gdi->downChartPen.get(), g_downHistory, g_historyIdx, chartDown);
        graphics.DrawString((L"\x2193 " + strDown).c_str(), -1, g_gdi->font.get(), RectF(chartW + 16 * sc, 0, w - chartW - 16 * sc, halfH), &format, g_gdi->downBrush.get());

        RectF chartUp(8.0f * sc, halfH + 2.0f * sc, chartW, halfH - 8.0f * sc);
        DrawSparkline(graphics, g_gdi->upChartPen.get(), g_upHistory, g_historyIdx, chartUp);
        graphics.DrawString((L"\x2191 " + strUp).c_str(), -1, g_gdi->font.get(), RectF(chartW + 16 * sc, halfH, w - chartW - 16 * sc, halfH), &format, g_gdi->upBrush.get());

    } else if (s.theme == L"Minimal") {
        format.SetAlignment(StringAlignmentNear);
        format.SetLineAlignment(StringAlignmentCenter);
        float halfW = w / 2.0f;

        graphics.DrawString((L"\x2193 " + strDown).c_str(), -1, g_gdi->font.get(), RectF(1 * sc, 1 * sc, halfW, h), &format, g_gdi->shadowBrush.get());
        graphics.DrawString((L"\x2193 " + strDown).c_str(), -1, g_gdi->font.get(), RectF(0, 0, halfW, h), &format, g_gdi->downBrush.get());

        graphics.DrawString((L"\x2191 " + strUp).c_str(), -1, g_gdi->font.get(), RectF(halfW + 1 * sc, 1 * sc, halfW, h), &format, g_gdi->shadowBrush.get());
        graphics.DrawString((L"\x2191 " + strUp).c_str(), -1, g_gdi->font.get(), RectF(halfW, 0, halfW, h), &format, g_gdi->upBrush.get());
    }
}

void RenderWidget(HWND hWnd, int opacity) {
    if (!IsWindowVisible(hWnd)) return;

    int width = g_currentWidgetWidth;
    int height = g_currentWidgetHeight;
    if (width <= 0 || height <= 0 || !g_gdi || !g_gdi->IsValid()) return;

    HDC hdcScreen = GetDC(nullptr);
    HDC hdcMem = CreateCompatibleDC(hdcScreen);

    BITMAPINFO bmi{};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void* pBits = nullptr;
    HBITMAP hBitmap = CreateDIBSection(hdcScreen, &bmi, DIB_RGB_COLORS, &pBits, nullptr, 0);
    if (hBitmap) {
        HBITMAP hOldBitmap = (HBITMAP)SelectObject(hdcMem, hBitmap);

        {
            Graphics graphics(hdcMem);
            graphics.SetSmoothingMode(SmoothingModeAntiAlias);
            graphics.SetTextRenderingHint(TextRenderingHintAntiAlias);
            graphics.Clear(Color(0, 0, 0, 0));

            if (g_gdi && g_gdi->IsValid()) {
                Settings s = GetSafeSettings();
                PaintWidgetContent(graphics, width, height, s, g_currentDpi);
            }
        }

        RECT rc{};
        POINT ptDst{0, 0};
        if (GetWindowRect(hWnd, &rc)) {
            ptDst.x = rc.left;
            ptDst.y = rc.top;
        }
        SIZE sizeWnd{width, height};
        POINT ptSrc{0, 0};

        BLENDFUNCTION blend{};
        blend.BlendOp = AC_SRC_OVER;
        blend.SourceConstantAlpha = (BYTE)std::clamp(opacity, 0, 255);
        blend.AlphaFormat = AC_SRC_ALPHA;

        if (!UpdateLayeredWindow(hWnd, hdcScreen, &ptDst, &sizeWnd, hdcMem, &ptSrc, 0, &blend, ULW_ALPHA)) {
            Wh_Log(L"RenderWidget: UpdateLayeredWindow failed, gle=%lu", GetLastError());
        }

        SelectObject(hdcMem, hOldBitmap);
        DeleteObject(hBitmap);
    } else {
        Wh_Log(L"RenderWidget: CreateDIBSection failed, gle=%lu", GetLastError());
    }

    DeleteDC(hdcMem);
    ReleaseDC(nullptr, hdcScreen);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_UPDATE_SETTINGS: {
            Settings s = GetSafeSettings();
            RebuildGdiObjects(s.fontSize, s.theme, g_currentDpi);
            KillTimer(hWnd, kTimerId);
            SetTimer(hWnd, kTimerId, s.updateIntervalMs, nullptr);
            RepositionWidget(hWnd, s);
            RenderWidget(hWnd, s.opacity);
            return 0;
        }
        case WM_UPDATE_POSITION: {
            g_posUpdatePending.store(false);
            Settings s = GetSafeSettings();
            RepositionWidget(hWnd, s);
            RenderWidget(hWnd, s.opacity);
            return 0;
        }
        case WM_PAINT:
            ValidateRect(hWnd, nullptr);
            return 0;
        case WM_TIMER:
            if (wParam == kTimerId) {
                UpdateSpeedSample();
                if (!g_hCachedTaskbar || !IsWindow(g_hCachedTaskbar)) {
                    // Taskbar vanished without a destroy event (e.g. Explorer
                    // crashed rather than exiting cleanly). Tear down so the
                    // outer loop re-attaches to the new taskbar.
                    g_hCachedTaskbar = nullptr;
                    PostMessageW(hWnd, WM_CLOSE, 0, 0);
                    return 0;
                }
                Settings s = GetSafeSettings();
                RepositionWidget(hWnd, s);
                RenderWidget(hWnd, s.opacity);
            }
            return 0;
        case WM_ERASEBKGND:
            return 1;
        case WM_CLOSE:
            KillTimer(hWnd, kTimerId);
            DestroyWindow(hWnd);
            return 0;
        case WM_NCDESTROY: {
            {
                std::lock_guard<std::mutex> lock(g_wndLifecycleMutex);
                g_hWnd.store(nullptr);
                g_posUpdatePending.store(false);
            }
            PostQuitMessage(0);
            break; // Let DefWindowProc handle cleanup
        }
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

HWND CreateBandedWidgetWindow(DWORD exStyle, LPCWSTR className, DWORD style,
                               HINSTANCE hInstance) {
    static CreateWindowInBand_t pCreateWindowInBand =
        (CreateWindowInBand_t)GetProcAddress(GetModuleHandleW(L"user32.dll"),
                                              "CreateWindowInBand");

    if (pCreateWindowInBand) {
        HWND hwnd = pCreateWindowInBand(exStyle, className, L"", style, 0, 0,
                                         1, 1, nullptr, nullptr, hInstance,
                                         nullptr, ZBID_IMMERSIVE_NOTIFICATION);
        if (hwnd) {
            Wh_Log(L"CreateWindowInBand succeeded (ZBID_IMMERSIVE_NOTIFICATION)");
            g_usingBandedWindow.store(true);
            return hwnd;
        }
        Wh_Log(L"CreateWindowInBand failed, gle=%lu -- falling back to a "
               L"plain topmost window", GetLastError());
    } else {
        Wh_Log(L"CreateWindowInBand not exported by user32.dll on this "
               L"build -- falling back to a plain topmost window");
    }

    g_usingBandedWindow.store(false);
    return CreateWindowExW(exStyle | WS_EX_TOPMOST, className, L"", style, 0,
                            0, 1, 1, nullptr, nullptr, hInstance, nullptr);
}

DWORD WINAPI WidgetMessageLoop(LPVOID lpParam) {
    MSG msg;
    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);
    SetEvent(g_hQueueReady);

    WNDCLASSW wc{};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = GetCurrentModuleHandle();
    wc.hbrBackground = nullptr;
    LPCWSTR szClassName = L"WindhawkNetSpeedWidget";
    wc.lpszClassName = szClassName;

    if (!RegisterClassW(&wc)) {
        Wh_Log(L"RegisterClassW failed, gle=%lu", GetLastError());
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

        g_hCachedTaskbar = hTaskbar;
        memset(&g_lastWidgetRect, 0, sizeof(RECT));
        g_posUpdatePending.store(false);

        DWORD exStyle = WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE |
                         WS_EX_TRANSPARENT;

        HWND hwnd = nullptr;
        {
            std::lock_guard<std::mutex> lock(g_wndLifecycleMutex);
            if (g_exiting.load()) break;

            hwnd = CreateBandedWidgetWindow(exStyle, szClassName, WS_POPUP, wc.hInstance);
            if (hwnd) {
                g_hWnd.store(hwnd);
            }
        }
        if (!hwnd) {
            Wh_Log(L"CreateWindowExW failed, gle=%lu", GetLastError());
            if (WaitForSingleObject(g_hExitEvent, 1000) == WAIT_OBJECT_0) break;
            continue;
        }

        Wh_Log(L"Widget created (banded=%d), taskbar pid=%lu tid=%lu",
               (int)g_usingBandedWindow.load(), pid, tid);
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
        Wh_Log(L"Widget message loop exited, will retry if not shutting down");
    }

    UnregisterClassW(szClassName, wc.hInstance);
    return 0;
}

}  // namespace

BOOL WhTool_ModInit() {
    LoadSettings();

    GdiplusStartupInput gdiplusStartupInput;
    Status gdiStatus = GdiplusStartup(&g_gdiplusToken, &gdiplusStartupInput, nullptr);
    if (gdiStatus != Ok) {
        Wh_Log(L"GdiplusStartup failed, status=%d", (int)gdiStatus);
        return FALSE;
    }

    g_exiting.store(false);
    g_hExitEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_hQueueReady = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_hExitEvent || !g_hQueueReady) {
        Wh_Log(L"CreateEventW failed, gle=%lu", GetLastError());
        if (g_hExitEvent) { CloseHandle(g_hExitEvent); g_hExitEvent = nullptr; }
        if (g_hQueueReady) { CloseHandle(g_hQueueReady); g_hQueueReady = nullptr; }
        return FALSE;
    }

    g_hThread = CreateThread(nullptr, 0, WidgetMessageLoop, nullptr, 0, &g_threadId);
    if (!g_hThread) {
        Wh_Log(L"CreateThread failed, gle=%lu", GetLastError());
        return FALSE;
    }

    Wh_Log(L"WhTool_ModInit: worker thread started");
    return TRUE;
}

void WhTool_ModUninit() {
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

    // Released explicitly, BEFORE GdiplusShutdown.
    g_gdi.reset();

    if (g_gdiplusToken) {
        GdiplusShutdown(g_gdiplusToken);
        g_gdiplusToken = 0;
    }
}

void WhTool_ModSettingsChanged() {
    LoadSettings();
    HWND hwnd = g_hWnd.load();
    if (hwnd) PostMessageW(hwnd, WM_UPDATE_SETTINGS, 0, 0);
}

////////////////////////////////////////////////////////////////////////////////
// Windhawk tool mod implementation for mods which don't need to inject to
// other processes or hook other functions. Based on:
// https://github.com/ramensoftware/windhawk/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process
//
// Deviation from the verbatim snippet: only the real Explorer shell process
// (started with no command-line arguments) becomes the tool-mod launcher.
// Without this, every explorer.exe instance -- including short-lived
// /factory,{...} -Embedding COM-server instances and separate-process
// folder windows -- would spawn and immediately exit a throwaway tool-mod
// process on every launch.

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

BOOL Wh_ModInit() {
    DWORD sessionId;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) &&
        sessionId == 0) {
        return FALSE;
    }

    bool isExcluded = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
    if (!argv) {
        Wh_Log(L"CommandLineToArgvW failed");
        return FALSE;
    }

    for (int i = 1; i < argc; i++) {
        if (wcscmp(argv[i], L"-service") == 0 ||
            wcscmp(argv[i], L"-service-start") == 0 ||
            wcscmp(argv[i], L"-service-stop") == 0) {
            isExcluded = true;
            break;
        }
    }

    for (int i = 1; i < argc - 1; i++) {
        if (wcscmp(argv[i], L"-tool-mod") == 0) {
            isToolModProcess = true;
            if (wcscmp(argv[i + 1], WH_MOD_ID) == 0) {
                isCurrentToolModProcess = true;
            }
            break;
        }
    }

    LocalFree(argv);

    if (isExcluded) {
        return FALSE;
    }

    if (isCurrentToolModProcess) {
        g_toolModProcessMutex =
            CreateMutex(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
        if (!g_toolModProcessMutex) {
            Wh_Log(L"CreateMutex failed");
            ExitProcess(1);
        }

        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            Wh_Log(L"Tool mod already running (%s)", WH_MOD_ID);
            ExitProcess(1);
        }

        if (!WhTool_ModInit()) {
            ExitProcess(1);
        }

        IMAGE_DOS_HEADER* dosHeader =
            (IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);
        IMAGE_NT_HEADERS* ntHeaders =
            (IMAGE_NT_HEADERS*)((BYTE*)dosHeader + dosHeader->e_lfanew);

        DWORD entryPointRVA = ntHeaders->OptionalHeader.AddressOfEntryPoint;
        void* entryPoint = (BYTE*)dosHeader + entryPointRVA;

        Wh_SetFunctionHook(entryPoint, (void*)EntryPoint_Hook, nullptr);
        return TRUE;
    }

    // Only the shell instance (no arguments) should be treated as a
    // candidate launcher; -Embedding COM servers, folder windows, etc.
    // shouldn't spawn a tool-mod process of their own, and must return
    // FALSE here rather than fall through with g_isToolModProcessLauncher
    // left false -- otherwise Wh_ModUninit's ExitProcess(0) (meant only
    // for the -tool-mod process) would kill them when the mod unloads.
    if (isToolModProcess || argc != 1) {
        return FALSE;
    }

    g_isToolModProcessLauncher = true;
    return TRUE;
}

void Wh_ModAfterInit() {
    if (!g_isToolModProcessLauncher) {
        return;
    }

    WCHAR currentProcessPath[MAX_PATH];
    switch (GetModuleFileName(nullptr, currentProcessPath,
                              ARRAYSIZE(currentProcessPath))) {
        case 0:
        case ARRAYSIZE(currentProcessPath):
            Wh_Log(L"GetModuleFileName failed");
            return;
    }

    WCHAR
    commandLine[MAX_PATH + 2 +
                (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) - 1];
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", currentProcessPath,
               WH_MOD_ID);

    HMODULE kernelModule = GetModuleHandle(L"kernelbase.dll");
    if (!kernelModule) {
        kernelModule = GetModuleHandle(L"kernel32.dll");
        if (!kernelModule) {
            Wh_Log(L"No kernelbase.dll/kernel32.dll");
            return;
        }
    }

    using CreateProcessInternalW_t = BOOL(WINAPI*)(
        HANDLE hUserToken, LPCWSTR lpApplicationName, LPWSTR lpCommandLine,
        LPSECURITY_ATTRIBUTES lpProcessAttributes,
        LPSECURITY_ATTRIBUTES lpThreadAttributes, WINBOOL bInheritHandles,
        DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory,
        LPSTARTUPINFOW lpStartupInfo,
        LPPROCESS_INFORMATION lpProcessInformation,
        PHANDLE hRestrictedUserToken);
    CreateProcessInternalW_t pCreateProcessInternalW =
        (CreateProcessInternalW_t)GetProcAddress(kernelModule,
                                                 "CreateProcessInternalW");
    if (!pCreateProcessInternalW) {
        Wh_Log(L"No CreateProcessInternalW");
        return;
    }

    STARTUPINFO si{
        .cb = sizeof(STARTUPINFO),
        .dwFlags = STARTF_FORCEOFFFEEDBACK,
    };
    PROCESS_INFORMATION pi;
    if (!pCreateProcessInternalW(nullptr, currentProcessPath, commandLine,
                                 nullptr, nullptr, FALSE, NORMAL_PRIORITY_CLASS,
                                 nullptr, nullptr, &si, &pi, nullptr)) {
        Wh_Log(L"CreateProcess failed, gle=%lu", GetLastError());
        return;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

void Wh_ModSettingsChanged() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModSettingsChanged();
}

void Wh_ModUninit() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModUninit();
    ExitProcess(0);
}