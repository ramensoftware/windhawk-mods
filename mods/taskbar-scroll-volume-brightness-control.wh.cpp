// ==WindhawkMod==
// @id            taskbar-scroll-volume-brightness-control
// @name          Taskbar Scroll: Volume & Brightness Controller
// @description   Scroll over the right side of the taskbar to change volume, scroll over the left side to change brightness. Uses a custom in-taskbar UI that tracks your cursor.
// @version       1.1.0
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
                int maxBottom = std::max((int)tbRect.top, (int)(tbRect.bottom - g_scaledH));
                g_currentY = std::clamp((int)(g_lastCursorPt.y - (g_scaledH / 2)), (int)tbRect.top, maxBottom);
            }
            SetWindowPos(g_hOverlayWnd, HWND_TOPMOST, g_currentX, g_currentY, g_scaledW, g_scaledH, SWP_NOACTIVATE);
        }
        ShowWindow(g_hOverlayWnd, SW_SHOWNOACTIVATE);
    }

    SetTimer(g_hOverlayWnd, kOverlayHideTimerId, (UINT)settings.overlayDurationMs, nullptr);
    SetTimer(g_hOverlayWnd, kFrameTimerId, 16, nullptr);
}

// -----------------------------------------------------------------------
// WMI brightness fallback
// -----------------------------------------------------------------------

IWbemLocator* g_pWmiLocator = nullptr;
IWbemServices* g_pWmiServices = nullptr;

void InitWMI() {
    if (g_pWmiLocator) return;
    if (FAILED(CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER, IID_IWbemLocator, (LPVOID*)&g_pWmiLocator)) || !g_pWmiLocator) return;
    if (FAILED(g_pWmiLocator->ConnectServer(_bstr_t(L"ROOT\\WMI"), nullptr, nullptr, 0, 0, 0, 0, &g_pWmiServices)) || !g_pWmiServices) {
        g_pWmiLocator->Release();
        g_pWmiLocator = nullptr;
        return;
    }
    CoSetProxyBlanket(g_pWmiServices, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, nullptr, RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, nullptr, EOAC_NONE);
}

void CleanupWMI() {
    if (g_pWmiServices) { g_pWmiServices->Release(); g_pWmiServices = nullptr; }
    if (g_pWmiLocator) { g_pWmiLocator->Release(); g_pWmiLocator = nullptr; }
}

int AdjustBrightnessWMI(bool up, int notches, int stepPercent) {
    InitWMI();
    if (!g_pWmiServices) return -1;

    int resultPercent = -1;
    IEnumWbemClassObject* pEnumBrightness = nullptr;
    HRESULT hres = g_pWmiServices->ExecQuery(_bstr_t(L"WQL"), _bstr_t(L"SELECT * FROM WmiMonitorBrightness"), WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, nullptr, &pEnumBrightness);

    if (SUCCEEDED(hres) && pEnumBrightness) {
        IWbemClassObject* pObj = nullptr;
        ULONG uReturned = 0;
        if (pEnumBrightness->Next(WBEM_INFINITE, 1, &pObj, &uReturned) == S_OK && uReturned > 0) {
            VARIANT vtCurrent; VariantInit(&vtCurrent);
            VARIANT vtInstanceName; VariantInit(&vtInstanceName);

            if (SUCCEEDED(pObj->Get(L"CurrentBrightness", 0, &vtCurrent, nullptr, nullptr)) &&
                SUCCEEDED(pObj->Get(L"InstanceName", 0, &vtInstanceName, nullptr, nullptr)) &&
                vtInstanceName.vt == VT_BSTR) {
                
                int current = 50;
                if (vtCurrent.vt == VT_UI1) current = vtCurrent.bVal;
                else if (vtCurrent.vt == VT_I4) current = vtCurrent.lVal;
                else if (vtCurrent.vt == VT_UI4) current = (int)vtCurrent.ulVal;

                int step = stepPercent;
                if (step < 1) step = 1;
                int newVal = current + (up ? step * notches : -step * notches);
                if (newVal < 0) newVal = 0;
                if (newVal > 100) newVal = 100;

                std::wstring objectPath = L"WmiMonitorBrightnessMethods.InstanceName='";
                objectPath += vtInstanceName.bstrVal;
                objectPath += L"'";

                IWbemClassObject* pClass = nullptr;
                IWbemClassObject* pInParamsDef = nullptr;
                IWbemClassObject* pInParams = nullptr;
                IWbemClassObject* pOutParams = nullptr;

                g_pWmiServices->GetObject(_bstr_t(L"WmiMonitorBrightnessMethods"), 0, nullptr, &pClass, nullptr);
                if (pClass) pClass->GetMethod(L"WmiSetBrightness", 0, &pInParamsDef, nullptr);
                if (pInParamsDef) pInParamsDef->SpawnInstance(0, &pInParams);

                if (pInParams) {
                    VARIANT vtTimeout; VariantInit(&vtTimeout);
                    vtTimeout.vt = VT_I4; vtTimeout.lVal = 1;
                    pInParams->Put(L"Timeout", 0, &vtTimeout, 0);

                    VARIANT vtBrightness; VariantInit(&vtBrightness);
                    vtBrightness.vt = VT_UI1; vtBrightness.bVal = (BYTE)newVal;
                    pInParams->Put(L"Brightness", 0, &vtBrightness, 0);

                    hres = g_pWmiServices->ExecMethod(_bstr_t(objectPath.c_str()), _bstr_t(L"WmiSetBrightness"), 0, nullptr, pInParams, &pOutParams, nullptr);
                    if (SUCCEEDED(hres)) resultPercent = newVal;

                    VariantClear(&vtTimeout);
                    VariantClear(&vtBrightness);
                }

                if (pOutParams) pOutParams->Release();
                if (pInParams) pInParams->Release();
                if (pInParamsDef) pInParamsDef->Release();
                if (pClass) pClass->Release();
            }

            VariantClear(&vtCurrent);
            VariantClear(&vtInstanceName);
            pObj->Release();
        }
        pEnumBrightness->Release();
    }
    return resultPercent;
}

// -----------------------------------------------------------------------
// DDC/CI brightness
// -----------------------------------------------------------------------

struct MonitorCacheEntry {
    std::vector<PHYSICAL_MONITOR> physicalMonitors;
    bool ddcSupported = false;
};

std::unordered_map<HMONITOR, MonitorCacheEntry> g_monitorCache;

MonitorCacheEntry* GetOrOpenMonitorCache(HMONITOR hMonitor) {
    auto it = g_monitorCache.find(hMonitor);
    if (it != g_monitorCache.end()) return &it->second;

    MonitorCacheEntry entry;
    DWORD numPhysical = 0;
    if (GetNumberOfPhysicalMonitorsFromHMONITOR(hMonitor, &numPhysical) && numPhysical > 0) {
        entry.physicalMonitors.resize(numPhysical);
        if (GetPhysicalMonitorsFromHMONITOR(hMonitor, numPhysical, entry.physicalMonitors.data())) {
            DWORD minB = 0, curB = 0, maxB = 0;
            if (GetMonitorBrightness(entry.physicalMonitors[0].hPhysicalMonitor, &minB, &curB, &maxB)) {
                entry.ddcSupported = true;
            } else {
                entry.ddcSupported = false;
            }
        } else {
            entry.physicalMonitors.clear();
        }
    }

    auto inserted = g_monitorCache.emplace(hMonitor, std::move(entry));
    return &inserted.first->second;
}

void CloseAllMonitorCaches() {
    for (auto& [hMonitor, entry] : g_monitorCache) {
        if (!entry.physicalMonitors.empty()) {
            DestroyPhysicalMonitors((DWORD)entry.physicalMonitors.size(), entry.physicalMonitors.data());
        }
    }
    g_monitorCache.clear();
}

int AdjustBrightnessDDC(HMONITOR hMonitor, bool up, int notches, int stepPercent, bool* outDdcSupported) {
    MonitorCacheEntry* entry = GetOrOpenMonitorCache(hMonitor);
    *outDdcSupported = entry->ddcSupported;
    if (!entry->ddcSupported) return -1;

    int resultPercent = -1;
    bool anyFailure = false;

    for (auto& pm : entry->physicalMonitors) {
        if (g_stopping) break;
        DWORD minB = 0, curB = 0, maxB = 0;
        if (GetMonitorBrightness(pm.hPhysicalMonitor, &minB, &curB, &maxB)) {
            int range = (maxB > minB) ? (int)(maxB - minB) : 100;
            int step = (range * stepPercent * notches) / 100;
            if (step < notches) step = notches;

            int newVal = (int)curB + (up ? step : -step);
            if (newVal < (int)minB) newVal = (int)minB;
            if (newVal > (int)maxB) newVal = (int)maxB;

            if (SetMonitorBrightness(pm.hPhysicalMonitor, (DWORD)newVal)) {
                if (resultPercent < 0) {
                    resultPercent = range > 0 ? ((newVal - (int)minB) * 100) / range : 0;
                }
            } else {
                anyFailure = true;
            }
        } else {
            anyFailure = true;
        }
    }

    if (anyFailure && resultPercent < 0) {
        return -1;
    }

    return resultPercent;
}

// -----------------------------------------------------------------------
// Taskbar scroll handling
// -----------------------------------------------------------------------

int AccumulateNotches(int& accumulator, short delta) {
    accumulator += delta;
    int notches = accumulator / WHEEL_DELTA;
    accumulator -= notches * WHEEL_DELTA;
    return notches;
}

bool HandleTaskbarScroll(HWND hTaskbar, POINT pt, short delta) {
    Settings settings = GetSettingsSnapshot();
    RECT rc;
    if (!GetWindowRect(hTaskbar, &rc)) return false;

    if (settings.excludeSystemTray) {
        HWND hTrayNotify = FindWindowExW(hTaskbar, nullptr, L"TrayNotifyWnd", nullptr);
        RECT trayRect = {0};
        bool hasTrayRect = false;

        if (hTrayNotify && GetWindowRect(hTrayNotify, &trayRect) && !IsRectEmpty(&trayRect)) {
            hasTrayRect = true;
        }

        if (!hasTrayRect) {
            HWND hBridge = nullptr;
            while ((hBridge = FindWindowExW(hTaskbar, hBridge, L"Windows.UI.Composition.DesktopWindowContentBridge", nullptr)) != nullptr) {
                if (GetWindowRect(hBridge, &trayRect) && !IsRectEmpty(&trayRect)) {
                    if (trayRect.left == rc.left && trayRect.right == rc.right && trayRect.top == rc.top && trayRect.bottom == rc.bottom) {
                        continue;
                    }
                    hasTrayRect = true;
                    break;
                }
            }
        }
        
        if (!hasTrayRect) {
            HWND hClock = FindWindowExW(hTaskbar, nullptr, L"ClockButton", nullptr);
            if (hClock && GetWindowRect(hClock, &trayRect) && !IsRectEmpty(&trayRect)) {
                hasTrayRect = true;
            }
        }
        
        if (hasTrayRect && PtInRect(&trayRect, pt)) {
            return false;
        }
    }

    int width = rc.right - rc.left;
    int height = rc.bottom - rc.top;
    if (width <= 0 || height <= 0) return false;

    bool isHorizontal = width > height;
    bool rightSide = false;

    if (isHorizontal) {
        int splitX = (width * settings.splitPercent) / 100;
        rightSide = (pt.x - rc.left) >= splitX;
    } else {
        int splitY = (height * settings.splitPercent) / 100;
        rightSide = (pt.y - rc.top) >= splitY;
    }

    if (settings.invertSides) rightSide = !rightSide;

    static int s_volumeAccum = 0;
    static int s_brightnessAccum = 0;
    int notches = AccumulateNotches(rightSide ? s_volumeAccum : s_brightnessAccum, delta);
    if (notches == 0) return true;

    bool scrollUp = notches > 0;
    if (settings.reverseScrollDirection) scrollUp = !scrollUp;
    int absNotches = std::abs(notches);

    HMONITOR hMon = MonitorFromWindow(hTaskbar, MONITOR_DEFAULTTONEAREST);

    if (rightSide) {
        if (g_workerThreadId) {
            ScrollRequest* req = new ScrollRequest{scrollUp, absNotches, hMon, hTaskbar, pt};
            if (!PostThreadMessageW(g_workerThreadId, WM_APP_VOLUME_REQUEST, 0, (LPARAM)req)) delete req;
        }
    } else if (g_monitorThreadId) {
        ScrollRequest* req = new ScrollRequest{scrollUp, absNotches, hMon, hTaskbar, pt};
        if (!PostThreadMessageW(g_monitorThreadId, WM_APP_BRIGHTNESS_REQUEST, 0, (LPARAM)req)) delete req;
    }

    return true;
}

LRESULT CALLBACK TaskbarSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, DWORD_PTR dwRefData) {
    if (uMsg == WM_MOUSEWHEEL) {
        POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
        short delta = GET_WHEEL_DELTA_WPARAM(wParam);
        if (HandleTaskbarScroll(hWnd, pt, delta)) return 0;
    } else if (uMsg == WM_DISPLAYCHANGE) {
        if (g_monitorThreadId) {
            while (!PostThreadMessageW(g_monitorThreadId, WM_APP_CLEAR_MONITOR_CACHE, 0, 0) &&
                   WaitForSingleObject(g_hMonitorThread, 10) == WAIT_TIMEOUT) {}
        }
    } else if (uMsg == WM_NCDESTROY) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hWnd, TaskbarSubclassProc);
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

WNDPROC InputSiteWindowProc_Original = nullptr;
LRESULT CALLBACK InputSiteWindowProc_Hook(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg == WM_POINTERWHEEL) {
        HWND hRootWnd = GetAncestor(hWnd, GA_ROOT);
        if (hRootWnd) {
            WCHAR szClass[32];
            if (GetClassNameW(hRootWnd, szClass, 32)) {
                if (_wcsicmp(szClass, L"Shell_TrayWnd") == 0 || _wcsicmp(szClass, L"Shell_SecondaryTrayWnd") == 0) {
                    POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
                    short delta = GET_WHEEL_DELTA_WPARAM(wParam);
                    if (HandleTaskbarScroll(hRootWnd, pt, delta)) return 0;
                }
            }
        }
    }
    if (InputSiteWindowProc_Original) {
        return InputSiteWindowProc_Original(hWnd, uMsg, wParam, lParam);
    }
    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

void HookInputSite(HWND hWnd) {
    auto wndProc = (WNDPROC)GetWindowLongPtrW(hWnd, GWLP_WNDPROC);
    if (wndProc && wndProc != InputSiteWindowProc_Hook && !InputSiteWindowProc_Original) {
        WindhawkUtils::SetFunctionHook(wndProc, InputSiteWindowProc_Hook, &InputSiteWindowProc_Original);
        if (g_initialized) {
            Wh_ApplyHookOperations();
        }
    }
}

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Original = nullptr;
HWND WINAPI CreateWindowExW_Hook(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam) {
    HWND hWnd = CreateWindowExW_Original(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
    if (hWnd && ((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xffff)) {
        if (_wcsicmp(lpClassName, L"Shell_TrayWnd") == 0 || _wcsicmp(lpClassName, L"Shell_SecondaryTrayWnd") == 0) {
            WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, TaskbarSubclassProc, 0);
        }
    }
    return hWnd;
}

using CreateWindowInBand_t = HWND(WINAPI*)(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam, DWORD dwBand);
CreateWindowInBand_t CreateWindowInBand_Original = nullptr;
HWND WINAPI CreateWindowInBand_Hook(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam, DWORD dwBand) {
    HWND hWnd = CreateWindowInBand_Original(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam, dwBand);
    if (hWnd && ((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xffff)) {
        if (_wcsicmp(lpClassName, L"Windows.UI.Input.InputSite.WindowClass") == 0) {
            HWND hParent = GetParent(hWnd);
            if (hParent) {
                WCHAR szParentClass[64];
                if (GetClassNameW(hParent, szParentClass, ARRAYSIZE(szParentClass)) && _wcsicmp(szParentClass, L"Windows.UI.Composition.DesktopWindowContentBridge") == 0) {
                    HWND hTaskbar = GetAncestor(hParent, GA_ROOT);
                    if (hTaskbar) {
                        WCHAR szTaskbarClass[64];
                        if (GetClassNameW(hTaskbar, szTaskbarClass, ARRAYSIZE(szTaskbarClass)) && 
                            (_wcsicmp(szTaskbarClass, L"Shell_TrayWnd") == 0 || _wcsicmp(szTaskbarClass, L"Shell_SecondaryTrayWnd") == 0)) {
                            HookInputSite(hWnd);
                        }
                    }
                }
            }
        }
    }
    return hWnd;
}

// -----------------------------------------------------------------------
// Monitor thread
// -----------------------------------------------------------------------

DWORD WINAPI MonitorThreadProc(LPVOID) {
    MSG msg;
    PeekMessage(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);  

    HRESULT comInit = CoInitializeEx(nullptr, COINIT_MULTITHREADED);

    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        if (msg.hwnd == nullptr && msg.message == WM_APP_BRIGHTNESS_REQUEST) {
            ScrollRequest* req = (ScrollRequest*)msg.lParam;
            if (req) {
                std::vector<ScrollRequest*> deferred;
                MSG nextMsg;
                while (PeekMessageW(&nextMsg, nullptr, WM_APP_BRIGHTNESS_REQUEST, WM_APP_BRIGHTNESS_REQUEST, PM_REMOVE)) {
                    ScrollRequest* nextReq = (ScrollRequest*)nextMsg.lParam;
                    if (nextReq) {
                        if (nextReq->hMonitor == req->hMonitor) {
                            if (nextReq->scrollUp == req->scrollUp) req->notches += nextReq->notches;
                            else req->notches -= nextReq->notches;
                            req->cursorPt = nextReq->cursorPt; 
                            delete nextReq;
                        } else {
                            deferred.push_back(nextReq);
                        }
                    }
                }

                if (req->notches < 0) {
                    req->scrollUp = !req->scrollUp;
                    req->notches = -req->notches;
                }

                if (req->notches > 0) {
                    Settings settings = GetSettingsSnapshot();
                    bool ddcSupported = false;
                    int percent = AdjustBrightnessDDC(req->hMonitor, req->scrollUp, req->notches, settings.brightnessStep, &ddcSupported);

                    if (!ddcSupported && settings.enableWmiFallback) {
                        percent = AdjustBrightnessWMI(req->scrollUp, req->notches, settings.brightnessStep);
                    }

                    if (percent >= 0 && g_workerThreadId) {
                        ScrollResult* res = new ScrollResult{percent, req->hMonitor, req->hTaskbar, req->cursorPt, OverlayMode::Brightness};
                        while (!PostThreadMessageW(g_workerThreadId, WM_APP_BRIGHTNESS_RESULT, 0, (LPARAM)res) &&
                               WaitForSingleObject(g_hWorkerThread, 10) == WAIT_TIMEOUT) {}
                    }
                }
                delete req;

                DWORD currentThreadId = GetCurrentThreadId();
                for (ScrollRequest* d : deferred) {
                    while (!PostThreadMessageW(currentThreadId, WM_APP_BRIGHTNESS_REQUEST, 0, (LPARAM)d) && !g_stopping) {}
                }
            }
            continue;
        }
        if (msg.hwnd == nullptr && msg.message == WM_APP_CLEAR_MONITOR_CACHE) {
            CloseAllMonitorCaches();
            continue;
        }
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    MSG leftover;
    while (PeekMessageW(&leftover, nullptr, WM_APP_BRIGHTNESS_REQUEST, WM_APP_BRIGHTNESS_RESULT, PM_REMOVE)) {
        if (leftover.message == WM_APP_BRIGHTNESS_REQUEST) delete (ScrollRequest*)leftover.lParam;
        else if (leftover.message == WM_APP_BRIGHTNESS_RESULT) delete (ScrollResult*)leftover.lParam;
    }

    CleanupWMI();
    CloseAllMonitorCaches();
    if (SUCCEEDED(comInit)) CoUninitialize();
    return 0;
}

// -----------------------------------------------------------------------
// UI / Audio thread
// -----------------------------------------------------------------------

DWORD WINAPI WorkerThreadProc(LPVOID) {
    MSG msg;
    PeekMessage(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);
    
    HRESULT comInit = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    IMMDeviceEnumerator* pEnum = nullptr;

    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        if (msg.hwnd == nullptr && msg.message == WM_APP_VOLUME_REQUEST) {
            ScrollRequest* req = (ScrollRequest*)msg.lParam;
            if (req) {
                
                MSG nextMsg;
                while (PeekMessageW(&nextMsg, nullptr, WM_APP_VOLUME_REQUEST, WM_APP_VOLUME_REQUEST, PM_REMOVE)) {
                    ScrollRequest* nextReq = (ScrollRequest*)nextMsg.lParam;
                    if (nextReq) {
                        if (nextReq->scrollUp == req->scrollUp) req->notches += nextReq->notches;
                        else req->notches -= nextReq->notches;
                        req->cursorPt = nextReq->cursorPt; 
                        req->hTaskbar = nextReq->hTaskbar;
                        delete nextReq;
                    }
                }

                if (req->notches < 0) {
                    req->scrollUp = !req->scrollUp;
                    req->notches = -req->notches;
                }
                
                Settings settings = GetSettingsSnapshot();
                
                if (!pEnum) CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&pEnum);

                int percent = -1;
                if (pEnum && req->notches > 0) {
                    IMMDevice* pDevice = nullptr;
                    pEnum->GetDefaultAudioEndpoint(eRender, eMultimedia, &pDevice);
                    if (pDevice) {
                        IAudioEndpointVolume* pVol = nullptr;
                        pDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, NULL, (void**)&pVol);
                        if (pVol) {
                            float currentVol = 0.0f;
                            pVol->GetMasterVolumeLevelScalar(&currentVol);
                            float newVol = currentVol + (req->scrollUp ? 1.0f : -1.0f) * (req->notches * settings.volumeStep / 100.0f);
                            if (newVol < 0.0f) newVol = 0.0f;
                            if (newVol > 1.0f) newVol = 1.0f;
                            
                            pVol->SetMasterVolumeLevelScalar(newVol, NULL);
                            
                            BOOL isMuted = FALSE;
                            pVol->GetMute(&isMuted);
                            if (isMuted && newVol > 0.001f) pVol->SetMute(FALSE, NULL);
                            if (!isMuted && newVol <= 0.001f) pVol->SetMute(TRUE, NULL);
                            
                            percent = (int)(newVol * 100.0f + 0.5f);
                            pVol->Release();
                        }
                        pDevice->Release();
                    }
                }

                if (percent >= 0) {
                    ShowOverlay(req->hTaskbar, percent, req->cursorPt, OverlayMode::Volume);
                }
                delete req;
            }
            continue;
        }

        if (msg.hwnd == nullptr && msg.message == WM_APP_BRIGHTNESS_RESULT) {
            ScrollResult* res = (ScrollResult*)msg.lParam;
            if (res) {
                ShowOverlay(res->hTaskbar, res->percent, res->cursorPt, res->mode);
                delete res;
            }
            continue;
        }
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    MSG leftover;
    while (PeekMessageW(&leftover, nullptr, WM_APP_VOLUME_REQUEST, WM_APP_VOLUME_REQUEST, PM_REMOVE)) {
        delete (ScrollRequest*)leftover.lParam;
    }

    if (pEnum) pEnum->Release();
    if (g_hOverlayWnd) {
        DestroyWindow(g_hOverlayWnd);
        g_hOverlayWnd = nullptr;
    }
    
    if (SUCCEEDED(comInit)) CoUninitialize();
    return 0;
}

BOOL CALLBACK EnumWindowsInitProc(HWND hWnd, LPARAM) {
    DWORD dwProcessId = 0;
    if (!GetWindowThreadProcessId(hWnd, &dwProcessId) || dwProcessId != GetCurrentProcessId()) return TRUE;

    WCHAR szClass[32];
    if (GetClassNameW(hWnd, szClass, 32)) {
        if (_wcsicmp(szClass, L"Shell_TrayWnd") == 0 || _wcsicmp(szClass, L"Shell_SecondaryTrayWnd") == 0) {
            WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, TaskbarSubclassProc, 0);

            HWND hXamlIsland = FindWindowExW(hWnd, nullptr, L"Windows.UI.Composition.DesktopWindowContentBridge", nullptr);
            if (hXamlIsland) {
                HWND hInputSite = FindWindowExW(hXamlIsland, nullptr, L"Windows.UI.Input.InputSite.WindowClass", nullptr);
                if (hInputSite) HookInputSite(hInputSite);
            }
        }
    }
    return TRUE;
}

BOOL CALLBACK EnumWindowsUninitProc(HWND hWnd, LPARAM) {
    DWORD dwProcessId = 0;
    if (!GetWindowThreadProcessId(hWnd, &dwProcessId) || dwProcessId != GetCurrentProcessId()) return TRUE;

    WCHAR szClass[32];
    if (GetClassNameW(hWnd, szClass, 32)) {
        if (_wcsicmp(szClass, L"Shell_TrayWnd") == 0 || _wcsicmp(szClass, L"Shell_SecondaryTrayWnd") == 0) {
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(hWnd, TaskbarSubclassProc);
        }
    }
    return TRUE;
}

// -----------------------------------------------------------------------
// Mod entry points
// -----------------------------------------------------------------------

void Wh_ModAfterInit() {
    EnumWindows(EnumWindowsInitProc, 0);
}

BOOL Wh_ModInit() {
    g_initialized = false;
    g_stopping = false;
    InitializeCriticalSection(&g_settingsLock);
    LoadSettings();

    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    Gdiplus::Status gdiStatus = Gdiplus::GdiplusStartup(&g_gdiplusToken, &gdiplusStartupInput, nullptr);

    if (!WindhawkUtils::SetFunctionHook(CreateWindowExW, CreateWindowExW_Hook, &CreateWindowExW_Original)) {
        if (gdiStatus == Gdiplus::Ok) Gdiplus::GdiplusShutdown(g_gdiplusToken);
        DeleteCriticalSection(&g_settingsLock);
        return FALSE;
    }

    HMODULE user32Module = LoadLibraryExW(L"user32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (user32Module) {
        auto pCreateWindowInBand = (CreateWindowInBand_t)GetProcAddress(user32Module, "CreateWindowInBand");
        if (pCreateWindowInBand) WindhawkUtils::SetFunctionHook(pCreateWindowInBand, CreateWindowInBand_Hook, &CreateWindowInBand_Original);
    }

    g_hWorkerThread = CreateThread(nullptr, 0, WorkerThreadProc, nullptr, 0, &g_workerThreadId);
    if (!g_hWorkerThread) {
        if (gdiStatus == Gdiplus::Ok) Gdiplus::GdiplusShutdown(g_gdiplusToken);
        DeleteCriticalSection(&g_settingsLock);
        return FALSE;
    }

    g_hMonitorThread = CreateThread(nullptr, 0, MonitorThreadProc, nullptr, 0, &g_monitorThreadId);
    if (!g_hMonitorThread) {
        g_stopping = true;
        if (g_workerThreadId) {
            while (!PostThreadMessageW(g_workerThreadId, WM_QUIT, 0, 0) &&
                   WaitForSingleObject(g_hWorkerThread, 10) == WAIT_TIMEOUT) {}
        }
        WaitForSingleObject(g_hWorkerThread, INFINITE);
        CloseHandle(g_hWorkerThread);
        g_hWorkerThread = nullptr;
        g_workerThreadId = 0;
        
        if (gdiStatus == Gdiplus::Ok) Gdiplus::GdiplusShutdown(g_gdiplusToken);
        DeleteCriticalSection(&g_settingsLock);
        return FALSE;
    }

    g_initialized = true;
    return TRUE;
}

void Wh_ModUninit() {
    g_initialized = false;
    g_stopping = true;
    EnumWindows(EnumWindowsUninitProc, 0);

    if (g_workerThreadId) {
        while (!PostThreadMessageW(g_workerThreadId, WM_QUIT, 0, 0) &&
               WaitForSingleObject(g_hWorkerThread, 10) == WAIT_TIMEOUT) {}
    }
    if (g_hWorkerThread) {
        WaitForSingleObject(g_hWorkerThread, INFINITE);
        CloseHandle(g_hWorkerThread);
        g_hWorkerThread = nullptr;
        g_workerThreadId = 0;
    }

    if (g_monitorThreadId) {
        while (!PostThreadMessageW(g_monitorThreadId, WM_QUIT, 0, 0) &&
               WaitForSingleObject(g_hMonitorThread, 10) == WAIT_TIMEOUT) {}
    }
    if (g_hMonitorThread) {
        WaitForSingleObject(g_hMonitorThread, INFINITE);
        CloseHandle(g_hMonitorThread);
        g_hMonitorThread = nullptr;
        g_monitorThreadId = 0;
    }

    if (g_gdiplusToken) {
        Gdiplus::GdiplusShutdown(g_gdiplusToken);
        g_gdiplusToken = 0;
    }

    UnregisterClassW(kOverlayClassName, GetCurrentModuleHandle());
    g_classRegistered = false;
    DeleteCriticalSection(&g_settingsLock);
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}