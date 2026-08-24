// ==WindhawkMod==
// @id            taskbar-scroll-volume-brightness-control
// @name          Taskbar Scroll: Volume & Brightness Controller
// @description   Scroll over the right side of the taskbar to change volume, scroll over the left side to change brightness. Uses a custom in-taskbar UI that tracks your cursor.
// @version       1.0.9
// @author        Narayan Chetri
// @github        https://github.com/NarayanChetri
// @homepage      https://narayanchetri.dev
// @include       explorer.exe
// @compilerOptions -ldxva2 -lgdi32 -lgdiplus -ldwmapi -lcomctl32 -lshcore -lole32 -loleaut32 -lwbemuuid
// @license       GPL-3.0
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Scroll: Volume & Brightness

![Demo](https://raw.githubusercontent.com/NarayanChetri/Files/main/windhawk.gif)

Scroll your mouse wheel over the taskbar to control volume and brightness,
without leaving what you're doing.

- Scroll over the **right side** of the taskbar to change **volume**.
- Scroll over the **left side** of the taskbar to change **brightness**
  (works with external monitors over DDC/CI, and falls back to the WMI
  brightness interface for laptop-internal panels that don't support DDC/CI).
- A tiny, smooth in-taskbar overlay tracks your mouse cursor to show the 
  current percentage, completely replacing the bulky native Windows UI.
- Works on both the primary and secondary taskbars, and with the modern
  Windows 11 taskbar (pointer/touchpad wheel events included).

## Why this mod?
While other mods offer separate volume or brightness control, this mod provides a unified experience out-of-the-box. It features a custom, DPI-aware GDI+ overlay that physically tracks your cursor across the taskbar and natively suppresses the bulky Windows OSD flyouts without conflicting with other UI elements. 
*(Credit: The modern InputSite hooking scaffolding is derived from the Taskbar Scroll Actions mod by m417z).*

## Settings

All of the zone split, scroll direction, display duration/color, step amounts,
and system-tray exclusion behavior can be customized from the mod's settings page.

## Feedback

Issues and PRs welcome: https://github.com/NarayanChetri
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- splitPercent: 50
  $name: Split position (%)
  $description: >-
    Percentage from the edge of the taskbar where the volume/brightness
    zones split.
- invertSides: false
  $name: Invert zones
  $description: >-
    When enabled, the left side of the taskbar controls volume and the
    right side controls brightness.
- reverseScrollDirection: false
  $name: Reverse scroll direction
  $description: >-
    Scroll up to decrease and down to increase, useful if you use
    "natural" scrolling.
- volumeStep: 2
  $name: Volume step (%)
  $description: How much volume changes per scroll notch.
- brightnessStep: 5
  $name: Brightness step (%)
  $description: How much brightness changes per scroll notch.
- excludeSystemTray: true
  $name: Exclude system tray icons
  $description: >-
    Ignore scroll events over the system tray/notification area (clock,
    network, volume icon, etc.).
- showOverlay: true
  $name: Show overlay
  $description: Show a native-looking taskbar element with the current percentage.
- overlayDurationMs: 1000
  $name: Overlay display duration (ms)
  $description: How long the overlay stays on screen before fading out.
- accentColor: "FFBE5C"
  $name: Overlay accent color (hex RRGGBB)
  $description: Color of the progress bar and percentage text accent.
- enableWmiFallback: true
  $name: Enable WMI brightness fallback
  $description: >-
    For internal laptop displays that don't support DDC/CI, try adjusting
    brightness through the WMI brightness interface instead.
*/
// ==/WindhawkModSettings==

#define NOMINMAX
#include <windows.h>
#include <windowsx.h>
#include <shellscalingapi.h>
#include <physicalmonitorenumerationapi.h>
#include <highlevelmonitorconfigurationapi.h>
#include <gdiplus.h>
#include <dwmapi.h>
#include <wbemidl.h>
#include <comdef.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <vector>
#include <unordered_map>
#include <cmath>
#include <cwchar>
#include <algorithm>
#include <atomic>
#include <windhawk_utils.h>

#ifndef DWMWA_WINDOW_CORNER_PREFERENCE
#define DWMWA_WINDOW_CORNER_PREFERENCE 33
#endif
#ifndef DWMWCP_ROUND
#define DWMWCP_ROUND 2
#endif
#ifndef WM_POINTERWHEEL
#define WM_POINTERWHEEL 0x024E
#endif

// -----------------------------------------------------------------------
// Globals
// -----------------------------------------------------------------------
std::atomic<bool> g_initialized{false};
std::atomic<bool> g_stopping{false};
bool g_classRegistered = false;

// -----------------------------------------------------------------------
// Settings
// -----------------------------------------------------------------------

struct Settings {
    int splitPercent = 50;
    bool invertSides = false;
    bool reverseScrollDirection = false;
    int volumeStep = 2;
    int brightnessStep = 5;
    bool excludeSystemTray = true;
    bool showOverlay = true;
    int overlayDurationMs = 1000;
    Gdiplus::Color accentColor{255, 255, 190, 92};
    bool enableWmiFallback = true;
};

CRITICAL_SECTION g_settingsLock;
Settings g_settings;

Settings GetSettingsSnapshot() {
    EnterCriticalSection(&g_settingsLock);
    Settings copy = g_settings;
    LeaveCriticalSection(&g_settingsLock);
    return copy;
}

bool ParseHexColor(PCWSTR text, BYTE& r, BYTE& g, BYTE& b) {
    if (!text) return false;
    if (text[0] == L'#') text++;
    unsigned int ri = 0, gi = 0, bi = 0;
    if (swscanf_s(text, L"%2x%2x%2x", &ri, &gi, &bi) != 3) return false;
    r = (BYTE)ri;
    g = (BYTE)gi;
    b = (BYTE)bi;
    return true;
}

void LoadSettings() {
    Settings s;

    s.splitPercent = Wh_GetIntSetting(L"splitPercent");
    s.invertSides = Wh_GetIntSetting(L"invertSides") != 0;
    s.reverseScrollDirection = Wh_GetIntSetting(L"reverseScrollDirection") != 0;
    s.volumeStep = Wh_GetIntSetting(L"volumeStep");
    s.brightnessStep = Wh_GetIntSetting(L"brightnessStep");
    s.excludeSystemTray = Wh_GetIntSetting(L"excludeSystemTray") != 0;
    s.showOverlay = Wh_GetIntSetting(L"showOverlay") != 0;
    s.overlayDurationMs = Wh_GetIntSetting(L"overlayDurationMs");
    s.enableWmiFallback = Wh_GetIntSetting(L"enableWmiFallback") != 0;

    s.accentColor = Gdiplus::Color(255, 255, 190, 92);
    WindhawkUtils::StringSetting accentColorStr = WindhawkUtils::StringSetting::make(L"accentColor");
    BYTE r, g, b;
    if (ParseHexColor(accentColorStr.get(), r, g, b)) {
        s.accentColor = Gdiplus::Color(255, r, g, b);
    }

    if (s.splitPercent < 0) s.splitPercent = 0;
    if (s.splitPercent > 100) s.splitPercent = 100;
    if (s.volumeStep < 1) s.volumeStep = 1;
    if (s.volumeStep > 100) s.volumeStep = 100;
    if (s.brightnessStep < 1) s.brightnessStep = 1;
    if (s.brightnessStep > 100) s.brightnessStep = 100;
    if (s.overlayDurationMs < 200) s.overlayDurationMs = 200;
    if (s.overlayDurationMs > 5000) s.overlayDurationMs = 5000;

    EnterCriticalSection(&g_settingsLock);
    g_settings = s;
    LeaveCriticalSection(&g_settingsLock);
}

HINSTANCE GetCurrentModuleHandle() {
    HINSTANCE hInst = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       (LPCWSTR)&GetCurrentModuleHandle, &hInst);
    return hInst;
}

// -----------------------------------------------------------------------
// Thread / message plumbing
// -----------------------------------------------------------------------

constexpr UINT WM_APP_BRIGHTNESS_REQUEST = WM_APP + 1;
constexpr UINT WM_APP_BRIGHTNESS_RESULT  = WM_APP + 2;
constexpr UINT WM_APP_VOLUME_REQUEST     = WM_APP + 4;
constexpr UINT WM_APP_CLEAR_MONITOR_CACHE = WM_APP + 5;

enum class OverlayMode { Brightness, Volume };

struct ScrollRequest {
    bool scrollUp;
    int notches;
    HMONITOR hMonitor;
    HWND hTaskbar;
    POINT cursorPt;
};

struct ScrollResult {
    int percent;
    HMONITOR hMonitor;
    HWND hTaskbar;
    POINT cursorPt;
    OverlayMode mode;
};

HANDLE g_hWorkerThread = nullptr;
DWORD g_workerThreadId = 0;

HANDLE g_hMonitorThread = nullptr;
DWORD g_monitorThreadId = 0;

ULONG_PTR g_gdiplusToken = 0;

// -----------------------------------------------------------------------
// Overlay window
// -----------------------------------------------------------------------

constexpr wchar_t kOverlayClassName[] = L"WindhawkTaskbarScrollOverlay";
constexpr UINT_PTR kOverlayHideTimerId = 1;
constexpr UINT_PTR kFrameTimerId = 2;
constexpr int kOverlayWidth = 160;   
constexpr int kOverlayHeight = 34;

HWND g_hOverlayWnd = nullptr;

// Animation & Tracking state 
float g_animatedPercent = -1.0f;
int g_targetPercent = 0;
bool g_isHiding = false;
float g_opacity = 0.0f;
float g_targetOpacity = 255.0f;
HWND g_hTargetTaskbar = nullptr;
OverlayMode g_currentMode = OverlayMode::Brightness;

float g_scale = 1.0f;
int g_scaledW = 160;
int g_scaledH = 34;
POINT g_lastCursorPt = {0, 0};

int g_currentX = 0;
int g_currentY = 0;
int g_targetX = 0;
int g_targetY = 0;

void RoundedRectPathF(Gdiplus::GraphicsPath& path, float x, float y, float w, float h, float radius) {
    path.Reset();
    float d = radius * 2.0f;
    path.AddArc(x, y, d, d, 180.0f, 90.0f);
    path.AddArc(x + w - d, y, d, d, 270.0f, 90.0f);
    path.AddArc(x + w - d, y + h - d, d, d, 0.0f, 90.0f);
    path.AddArc(x, y + h - d, d, d, 90.0f, 90.0f);
    path.CloseFigure();
}

void PaintOverlay(HWND hWnd, HDC hdc) {
    RECT clientRc;
    GetClientRect(hWnd, &clientRc);
    int w = clientRc.right - clientRc.left;
    int h = clientRc.bottom - clientRc.top;
    if (w <= 0 || h <= 0) return;

    Settings settings = GetSettingsSnapshot();
    float scale = w / (float)kOverlayWidth;

    Gdiplus::Bitmap bmp(w, h, PixelFormat32bppARGB);
    Gdiplus::Graphics g(&bmp);
    g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
    g.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAliasGridFit);
    g.ScaleTransform(scale, scale);

    g.Clear(Gdiplus::Color(255, 28, 28, 30));

    Gdiplus::Pen borderPen(Gdiplus::Color(255, 65, 65, 65), 1.0f);
    g.DrawRectangle(&borderPen, 0, 0, kOverlayWidth - 1, kOverlayHeight - 1);

    Gdiplus::Color accent = settings.accentColor;
    Gdiplus::FontFamily fontFamily(L"Segoe UI");
    Gdiplus::Font labelFont(&fontFamily, 9.0f, Gdiplus::FontStyleRegular, Gdiplus::UnitPoint);
    Gdiplus::Font percentFont(&fontFamily, 9.5f, Gdiplus::FontStyleBold, Gdiplus::UnitPoint);
    Gdiplus::SolidBrush labelBrush(Gdiplus::Color(255, 170, 170, 175));
    Gdiplus::SolidBrush textBrush(Gdiplus::Color(255, 245, 245, 245));

    float clamped = g_animatedPercent < 0.0f ? 0.0f : (g_animatedPercent > 100.0f ? 100.0f : g_animatedPercent);
    const int pad = 12;

    Gdiplus::RectF headerRect((float)pad, 4.0f, (float)(kOverlayWidth - pad * 2), 16.0f);
    Gdiplus::StringFormat leftFormat;
    leftFormat.SetAlignment(Gdiplus::StringAlignmentNear);
    g.DrawString(g_currentMode == OverlayMode::Brightness ? L"Brightness" : L"Volume", -1, &labelFont, headerRect, &leftFormat, &labelBrush);

    Gdiplus::StringFormat rightFormat;
    rightFormat.SetAlignment(Gdiplus::StringAlignmentFar);
    wchar_t percentText[8];
    swprintf_s(percentText, L"%d%%", (int)(clamped + 0.5f));
    g.DrawString(percentText, -1, &percentFont, headerRect, &rightFormat, &textBrush);

    float barX = (float)pad, barY = kOverlayHeight - 11.0f, barW = kOverlayWidth - pad * 2.0f, barH = 5.0f;

    Gdiplus::GraphicsPath path;
    Gdiplus::SolidBrush trackBrush(Gdiplus::Color(255, 58, 58, 60));
    RoundedRectPathF(path, barX, barY, barW, barH, barH / 2.0f);
    g.FillPath(&trackBrush, &path);

    float fillW = (barW * clamped) / 100.0f;
    if (fillW > 0.1f) {
        if (fillW < barH) fillW = barH;
        Gdiplus::SolidBrush fillBrush(accent);
        RoundedRectPathF(path, barX, barY, fillW, barH, barH / 2.0f);
        g.FillPath(&fillBrush, &path);
    }

    Gdiplus::Graphics gScreen(hdc);
    gScreen.SetCompositingMode(Gdiplus::CompositingModeSourceCopy);
    gScreen.DrawImage(&bmp, 0, 0);
}

LRESULT CALLBACK OverlayWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            PaintOverlay(hWnd, hdc);
            EndPaint(hWnd, &ps);
            return 0;
        }
        case WM_TIMER:
            if (wParam == kOverlayHideTimerId) {
                KillTimer(hWnd, kOverlayHideTimerId);
                g_isHiding = true;
                g_targetOpacity = 0.0f;
            } else if (wParam == kFrameTimerId) {
                bool changed = false;

                if (std::abs(g_animatedPercent - (float)g_targetPercent) > 0.1f) {
                    g_animatedPercent += ((float)g_targetPercent - g_animatedPercent) * 0.15f;
                    changed = true;
                } else if (g_animatedPercent != (float)g_targetPercent) {
                    g_animatedPercent = (float)g_targetPercent;
                    changed = true;
                }

                if (std::abs(g_opacity - g_targetOpacity) > 1.0f) {
                    g_opacity += (g_targetOpacity - g_opacity) * 0.15f; 
                    SetLayeredWindowAttributes(hWnd, 0, (BYTE)g_opacity, LWA_ALPHA);
                } else if (g_opacity != g_targetOpacity) {
                    g_opacity = g_targetOpacity;
                    SetLayeredWindowAttributes(hWnd, 0, (BYTE)g_opacity, LWA_ALPHA);
                    if (g_opacity <= 0.0f) {
                        ShowWindow(hWnd, SW_HIDE);
                        KillTimer(hWnd, kFrameTimerId);
                        g_animatedPercent = -1.0f;
                    }
                }

                if (IsWindow(g_hTargetTaskbar)) {
                    RECT tbRect;
                    GetWindowRect(g_hTargetTaskbar, &tbRect);
                    
                    bool isHorizontal = (tbRect.right - tbRect.left) > (tbRect.bottom - tbRect.top);
                    
                    if (isHorizontal) {
                        g_targetX = g_lastCursorPt.x - (g_scaledW / 2);
                        int maxRight = std::max((int)tbRect.left, (int)(tbRect.right - g_scaledW));
                        g_targetX = std::clamp((int)g_targetX, (int)tbRect.left, maxRight);
                        g_targetY = tbRect.top + ((tbRect.bottom - tbRect.top) - g_scaledH) / 2;
                    } else {
                        g_targetX = tbRect.left + ((tbRect.right - tbRect.left) - g_scaledW) / 2;
                        g_targetY = g_lastCursorPt.y - (g_scaledH / 2);
                        int maxBottom = std::max((int)tbRect.top, (int)(tbRect.bottom - g_scaledH));
                        g_targetY = std::clamp((int)g_targetY, (int)tbRect.top, maxBottom);
                    }

                    if (g_currentX != g_targetX || g_currentY != g_targetY) {
                        int diffX = g_targetX - g_currentX;
                        int diffY = g_targetY - g_currentY;
                        int stepX = diffX == 0 ? 0 : (diffX > 0 ? std::max(1, (int)(diffX * 0.25f)) : std::min(-1, (int)(diffX * 0.25f)));
                        int stepY = diffY == 0 ? 0 : (diffY > 0 ? std::max(1, (int)(diffY * 0.25f)) : std::min(-1, (int)(diffY * 0.25f)));
                        
                        g_currentX += stepX;
                        g_currentY += stepY;
                        
                        SetWindowPos(hWnd, nullptr, g_currentX, g_currentY, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
                    }
                }

                if (changed) InvalidateRect(hWnd, nullptr, FALSE);
            }
            return 0;
        case WM_NCHITTEST:
            return HTTRANSPARENT;
        case WM_DESTROY:
            return 0;
    }
    return DefWindowProcW(hWnd, msg, wParam, lParam);
}

void EnsureOverlayWindow() {
    if (g_hOverlayWnd) return;

    if (!g_classRegistered) {
        WNDCLASSEXW wc = {sizeof(wc)};
        wc.style = CS_DROPSHADOW;
        wc.lpfnWndProc = OverlayWndProc;
        wc.hInstance = GetCurrentModuleHandle();
        wc.lpszClassName = kOverlayClassName;
        if (!RegisterClassExW(&wc)) return;
        g_classRegistered = true;
    }

    g_hOverlayWnd = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_LAYERED | WS_EX_TRANSPARENT,
        kOverlayClassName, L"", WS_POPUP, 0, 0, kOverlayWidth, kOverlayHeight,
        nullptr, nullptr, GetCurrentModuleHandle(), nullptr);

    if (g_hOverlayWnd) {
        DWORD pref = DWMWCP_ROUND;
        DwmSetWindowAttribute(g_hOverlayWnd, DWMWA_WINDOW_CORNER_PREFERENCE, &pref, sizeof(pref));
    }
}

void ShowOverlay(HWND hTaskbar, int percent, POINT cursorPt, OverlayMode mode) {
    if (percent < 0) return;

    Settings settings = GetSettingsSnapshot();
    if (!settings.showOverlay) return;

    EnsureOverlayWindow();
    if (!g_hOverlayWnd) return;

    g_lastCursorPt = cursorPt;
    g_targetPercent = percent;
    g_targetOpacity = 255.0f;
    g_isHiding = false;

    if (!hTaskbar || !IsWindow(hTaskbar)) hTaskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
    g_hTargetTaskbar = hTaskbar;

    if (g_currentMode != mode) {
        g_currentMode = mode;
        g_animatedPercent = (float)percent;
        
        if (IsWindow(g_hTargetTaskbar)) {
            RECT tbRect;
            GetWindowRect(g_hTargetTaskbar, &tbRect);
            bool isHorizontal = (tbRect.right - tbRect.left) > (tbRect.bottom - tbRect.top);
            if (isHorizontal) {
                int maxRight = std::max((int)tbRect.left, (int)(tbRect.right - g_scaledW));
                g_currentX = std::clamp((int)(g_lastCursorPt.x - (g_scaledW / 2)), (int)tbRect.left, maxRight);
                g_currentY = tbRect.top + ((tbRect.bottom - tbRect.top) - g_scaledH) / 2;
            } else {
                g_currentX = tbRect.left + ((tbRect.right - tbRect.left) - g_scaledW) / 2;
                int maxBottom = std::max((int)tbRect.top, (int)(tbRect.bottom - g_scaledH));
                g_currentY = std::clamp((int)(g_lastCursorPt.y - (g_scaledH / 2)), (int)tbRect.top, maxBottom);
            }
            SetWindowPos(g_hOverlayWnd, nullptr, g_currentX, g_currentY, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
        }
        InvalidateRect(g_hOverlayWnd, nullptr, FALSE);
    }

    if (!IsWindowVisible(g_hOverlayWnd)) {
        g_opacity = 0.0f;
        SetLayeredWindowAttributes(g_hOverlayWnd, 0, 0, LWA_ALPHA);
        if (g_animatedPercent < 0.0f) g_animatedPercent = (float)percent;

        if (IsWindow(g_hTargetTaskbar)) {
            MONITORINFO mi = {sizeof(mi)};
            HMONITOR mon = MonitorFromWindow(g_hTargetTaskbar, MONITOR_DEFAULTTONEAREST);
            GetMonitorInfoW(mon, &mi);
            
            UINT dpiX = 96, dpiY = 96;
            if (FAILED(GetDpiForMonitor(mon, MDT_EFFECTIVE_DPI, &dpiX, &dpiY)) || dpiX == 0) dpiX = 96;
            g_scale = dpiX / 96.0f;
            g_scaledW = std::max(1, (int)(kOverlayWidth * g_scale));
            g_scaledH = std::max(1, (int)(kOverlayHeight * g_scale));

            RECT tbRect;
            GetWindowRect(g_hTargetTaskbar, &tbRect);
            bool isHorizontal = (tbRect.right - tbRect.left) > (tbRect.bottom - tbRect.top);
            if (isHorizontal) {
                int maxRight = std::max((int)tbRect.left, (int)(tbRect.right - g_scaledW));
                g_currentX = std::clamp((int)(g_lastCursorPt.x - (g_scaledW / 2)), (int)tbRect.left, maxRight);
                g_currentY = tbRect.top + ((tbRect.bottom - tbRect.top) - g_scaledH) / 2;
            } else {
                g_currentX = tbRect.left + ((tbRect.right - tbRect.left) - g_scaledW) / 2;
                int maxBottom = std::max((int)tbRect.t
