// ==WindhawkMod==
// @id              virtual-desktop-indicator
// @name            Virtual Desktop Workspace Indicator
// @description     Displays a visual indicator and vignette effect when switching virtual desktops
// @version         0.1
// @author          Disillusion
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lgdi32 -lgdiplus -ladvapi32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Virtual Desktop Workspace Indicator

Shows a small overlay indicator on screen displaying which virtual desktop
(workspace) is currently active. Supports two display styles:

- **Numbers**: `[1] 2 3 4`
- **Dots**: `⏺ ○ ○ ○`

When you switch desktops, a smooth vignette effect flashes on screen and the
indicator animates to the new position. All colors, sizes, positions, and
animation speeds are configurable.

**Requirements:** Windows 11 22H2 or later.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- position: top-left
  $name: Indicator Position
  $description: Screen corner for the indicator overlay.
  $options:
    - top-left: Top Left
    - top-center: Top Center
    - top-right: Top Right
    - bottom-left: Bottom Left
    - bottom-center: Bottom Center
    - bottom-right: Bottom Right
- offsetX: 20
  $name: Horizontal Offset (px)
  $description: Pixel offset from the chosen corner horizontally.
- offsetY: 12
  $name: Vertical Offset (px)
  $description: Pixel offset from the chosen corner vertically.
- indicatorStyle: dots
  $name: Indicator Style
  $description: Visual style for the desktop indicator.
  $options:
    - numbers: "Numbers: [1] 2 3 4"
    - dots: "Dots: ⏺ ○ ○ ○"
- indicatorSize: 14
  $name: Indicator Size (px)
  $description: Font size for numbers or dot diameter.
- indicatorActiveColor:
    - red: 255
    - green: 255
    - blue: 255
  $name: Active Indicator Color
- indicatorInactiveColor:
    - red: 128
    - green: 128
    - blue: 128
  $name: Inactive Indicator Color
- showBackground: true
  $name: Show Indicator Background
  $description: Draw a rounded-rect pill behind the indicator. Disable for fully transparent background.
- indicatorBgColor:
    - red: 0
    - green: 0
    - blue: 0
  $name: Indicator Background Color
- indicatorBgOpacity: 180
  $name: Background Opacity (0-255)
  $description: Opacity of the indicator background pill.
- indicatorAlwaysVisible: true
  $name: Always Show Indicator
  $description: "When off, indicator only appears during a desktop switch and then fades away."
- indicatorFadeDelay: 2000
  $name: Indicator Fade Delay (ms)
  $description: How long the indicator stays visible after a switch (when not always visible).
- indicatorAnimationSpeed: 200
  $name: Indicator Animation Speed (ms)
  $description: Duration of the highlight slide animation between desktops.
- vignetteEnabled: true
  $name: Enable Vignette Effect
  $description: Flash a vignette overlay when switching desktops.
- vignetteColor:
    - red: 0
    - green: 0
    - blue: 0
  $name: Vignette Color
- vignetteIntensity: 100
  $name: Vignette Intensity (0-255)
  $description: Maximum opacity of the vignette edges.
- vignetteFeather: 50
  $name: Vignette Feather (0-100)
  $description: "Controls gradient softness. 0 = hard edges (more screen covered), 100 = very soft (only corners)."
- vignetteAnimationSpeed: 400
  $name: Vignette Animation Duration (ms)
  $description: Total time for vignette fade-in + fade-out.
- allMonitors: false
  $name: Show on All Monitors
  $description: "When enabled, indicator and vignette appear on every monitor. Otherwise primary only."
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <gdiplus.h>
#include <cmath>
#include <cstring>
#include <vector>

using namespace Gdiplus;

// ============================================================================
// Virtual Desktop Helper (registry-based, works on all Windows 11 builds)
// ============================================================================

static const wchar_t* VD_REG_PATH =
    L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\VirtualDesktops";

class VirtualDesktopHelper {
public:
    bool Init() {
        int count = GetDesktopCount();
        int current = GetCurrentDesktopIndex();
        Wh_Log(L"VirtualDesktopHelper init (registry): count=%d, current=%d",
               count, current);
        return count > 0 && current >= 0;
    }

    void Release() {}

    int GetDesktopCount() {
        HKEY hKey;
        if (RegOpenKeyExW(HKEY_CURRENT_USER, VD_REG_PATH,
                          0, KEY_READ, &hKey) != ERROR_SUCCESS)
            return 0;

        DWORD dataSize = 0;
        DWORD type = 0;
        RegQueryValueExW(hKey, L"VirtualDesktopIDs", nullptr,
                         &type, nullptr, &dataSize);
        RegCloseKey(hKey);

        if (type != REG_BINARY || dataSize < 16) return 1;
        return (int)(dataSize / 16);
    }

    int GetCurrentDesktopIndex() {
        HKEY hKey;
        if (RegOpenKeyExW(HKEY_CURRENT_USER, VD_REG_PATH,
                          0, KEY_READ, &hKey) != ERROR_SUCCESS)
            return -1;

        BYTE currentGuid[16] = {};
        DWORD currentSize = 16;
        DWORD type = 0;
        if (RegQueryValueExW(hKey, L"CurrentVirtualDesktop", nullptr, &type,
                             currentGuid, &currentSize) != ERROR_SUCCESS ||
            type != REG_BINARY || currentSize != 16) {
            RegCloseKey(hKey);
            return -1;
        }

        DWORD allSize = 0;
        RegQueryValueExW(hKey, L"VirtualDesktopIDs", nullptr, &type,
                         nullptr, &allSize);
        if (type != REG_BINARY || allSize < 16) {
            RegCloseKey(hKey);
            return -1;
        }

        std::vector<BYTE> allGuids(allSize);
        if (RegQueryValueExW(hKey, L"VirtualDesktopIDs", nullptr, &type,
                             allGuids.data(), &allSize) != ERROR_SUCCESS) {
            RegCloseKey(hKey);
            return -1;
        }
        RegCloseKey(hKey);

        int count = (int)(allSize / 16);
        for (int i = 0; i < count; i++) {
            if (memcmp(currentGuid, allGuids.data() + i * 16, 16) == 0)
                return i;
        }
        return -1;
    }
};

// ============================================================================
// Settings
// ============================================================================

enum class IndicatorPosition {
    TopLeft, TopCenter, TopRight,
    BottomLeft, BottomCenter, BottomRight
};

enum class IndicatorStyle {
    Numbers,
    Dots
};

struct Settings {
    IndicatorPosition position = IndicatorPosition::TopLeft;
    int offsetX = 20;
    int offsetY = 12;
    IndicatorStyle style = IndicatorStyle::Dots;
    int indicatorSize = 14;

    BYTE activeR = 255, activeG = 255, activeB = 255;
    BYTE inactiveR = 128, inactiveG = 128, inactiveB = 128;

    bool showBackground = true;
    BYTE bgR = 0, bgG = 0, bgB = 0;
    BYTE bgOpacity = 180;

    bool alwaysVisible = true;
    int fadeDelay = 2000;
    int animationSpeed = 200;

    bool vignetteEnabled = true;
    BYTE vigR = 0, vigG = 0, vigB = 0;
    BYTE vignetteIntensity = 100;
    int vignetteFeather = 50;
    int vignetteAnimSpeed = 400;

    bool allMonitors = false;
};

// ============================================================================
// Globals
// ============================================================================

static Settings g_settings;
static VirtualDesktopHelper g_vdHelper;
static ULONG_PTR g_gdiplusToken = 0;

static int g_currentDesktop = 0;
static int g_previousDesktop = 0;
static int g_desktopCount = 1;

// Animation state
static float g_indicatorAnimProgress = 1.0f;
static float g_vignetteAlpha = 0.0f;
static float g_indicatorAlpha = 1.0f;

enum class VignetteState { Idle, FadeIn, FadeOut };
static VignetteState g_vignetteState = VignetteState::Idle;

enum class IndicatorFadeState { Visible, Waiting, FadingOut, Hidden };
static IndicatorFadeState g_indicatorFadeState = IndicatorFadeState::Visible;
static DWORD g_switchTimestamp = 0;

// Windows
static const wchar_t* INDICATOR_CLASS = L"WHVDIndicator";
static const wchar_t* VIGNETTE_CLASS  = L"WHVDVignette";

struct OverlayWindow {
    HWND hwnd = nullptr;
    int monitorIndex = 0;
    RECT monitorRect = {};
    HBITMAP hCachedVignette = nullptr;
    int cachedVigW = 0;
    int cachedVigH = 0;
};

static std::vector<OverlayWindow> g_indicatorWindows;
static std::vector<OverlayWindow> g_vignetteWindows;

static const UINT_PTR TIMER_POLL    = 1;
static const UINT_PTR TIMER_ANIMATE = 2;

static HWND g_msgWindow = nullptr;
static bool g_animating = false;

// Worker thread
static HANDLE g_workerThread = nullptr;
static DWORD g_workerThreadId = 0;

// ============================================================================
// Monitor enumeration
// ============================================================================

struct MonitorInfo {
    HMONITOR hMonitor;
    RECT rect;
    bool isPrimary;
};

static std::vector<MonitorInfo> g_monitors;

static BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC, LPRECT, LPARAM) {
    MONITORINFO mi = {};
    mi.cbSize = sizeof(mi);
    if (GetMonitorInfoW(hMonitor, &mi)) {
        MonitorInfo info;
        info.hMonitor = hMonitor;
        info.rect = mi.rcMonitor;
        info.isPrimary = (mi.dwFlags & MONITORINFOF_PRIMARY) != 0;
        g_monitors.push_back(info);
    }
    return TRUE;
}

static void EnumerateMonitors() {
    g_monitors.clear();
    EnumDisplayMonitors(nullptr, nullptr, MonitorEnumProc, 0);
    if (g_monitors.empty()) {
        MonitorInfo fallback;
        fallback.hMonitor = nullptr;
        fallback.rect = {0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)};
        fallback.isPrimary = true;
        g_monitors.push_back(fallback);
    }
}

// ============================================================================
// Rendering helpers
// ============================================================================

static float EaseInOutCubic(float t) {
    return t < 0.5f ? 4.0f * t * t * t : 1.0f - powf(-2.0f * t + 2.0f, 3.0f) / 2.0f;
}

static void RenderIndicatorBitmap(Gdiplus::Bitmap** ppBmp, int* pWidth, int* pHeight,
                                   int desktopCount, int currentDesktop, int previousDesktop,
                                   float animProgress) {
    int size = g_settings.indicatorSize;
    int padding = size / 2;
    int spacing = (int)(size * 1.4f);
    int totalWidth, totalHeight;

    if (g_settings.style == IndicatorStyle::Dots) {
        totalWidth = padding * 2 + desktopCount * size + (desktopCount - 1) * (spacing - size);
        totalHeight = padding * 2 + size;
    } else {
        totalWidth = padding * 2 + desktopCount * (size + 4) + (desktopCount - 1) * 2;
        totalHeight = padding * 2 + size + 4;
    }

    if (totalWidth < 1) totalWidth = 1;
    if (totalHeight < 1) totalHeight = 1;

    auto* bmp = new Gdiplus::Bitmap(totalWidth, totalHeight, PixelFormat32bppARGB);
    Gdiplus::Graphics g(bmp);
    g.SetSmoothingMode(SmoothingModeAntiAlias);
    g.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAliasGridFit);
    g.Clear(Gdiplus::Color(0, 0, 0, 0));

    // Background pill
    if (g_settings.showBackground && g_settings.bgOpacity > 0) {
        Gdiplus::SolidBrush bgBrush(Gdiplus::Color(g_settings.bgOpacity,
                                                      g_settings.bgR, g_settings.bgG, g_settings.bgB));
        int radius = totalHeight / 2;
        Gdiplus::GraphicsPath path;
        path.AddArc(0, 0, radius * 2, totalHeight - 1, 180, 90);
        path.AddArc(totalWidth - 1 - radius * 2, 0, radius * 2, totalHeight - 1, 270, 90);
        path.AddArc(totalWidth - 1 - radius * 2, 0, radius * 2, totalHeight - 1, 0, 90);
        path.AddArc(0, 0, radius * 2, totalHeight - 1, 90, 90);
        path.CloseFigure();
        g.FillPath(&bgBrush, &path);
    }

    float easedProgress = EaseInOutCubic(animProgress);

    if (g_settings.style == IndicatorStyle::Dots) {
        for (int i = 0; i < desktopCount; i++) {
            int cx = padding + i * spacing + size / 2;
            int cy = totalHeight / 2;

            float activeAmount = 0.0f;
            if (i == currentDesktop) activeAmount = easedProgress;
            if (i == previousDesktop) { float prev = 1.0f - easedProgress; if (prev > activeAmount) activeAmount = prev; }

            BYTE r = (BYTE)(g_settings.inactiveR + (g_settings.activeR - g_settings.inactiveR) * activeAmount);
            BYTE gv = (BYTE)(g_settings.inactiveG + (g_settings.activeG - g_settings.inactiveG) * activeAmount);
            BYTE b = (BYTE)(g_settings.inactiveB + (g_settings.activeB - g_settings.inactiveB) * activeAmount);

            float dotSize = size * (0.5f + 0.5f * activeAmount);
            Gdiplus::SolidBrush brush(Gdiplus::Color(255, r, gv, b));
            g.FillEllipse(&brush,
                          (float)cx - dotSize / 2.0f, (float)cy - dotSize / 2.0f,
                          dotSize, dotSize);
        }
    } else {
        // Numbers style
        Gdiplus::FontFamily fontFamily(L"Segoe UI");
        Gdiplus::Font font(&fontFamily, (Gdiplus::REAL)size, FontStyleBold, UnitPixel);
        Gdiplus::StringFormat sf;
        sf.SetAlignment(StringAlignmentCenter);
        sf.SetLineAlignment(StringAlignmentCenter);

        int cellWidth = size + 4;
        for (int i = 0; i < desktopCount; i++) {
            float activeAmount = 0.0f;
            if (i == currentDesktop) activeAmount = easedProgress;
            if (i == previousDesktop) { float prev = 1.0f - easedProgress; if (prev > activeAmount) activeAmount = prev; }

            BYTE r = (BYTE)(g_settings.inactiveR + (g_settings.activeR - g_settings.inactiveR) * activeAmount);
            BYTE gv = (BYTE)(g_settings.inactiveG + (g_settings.activeG - g_settings.inactiveG) * activeAmount);
            BYTE b = (BYTE)(g_settings.inactiveB + (g_settings.activeB - g_settings.inactiveB) * activeAmount);

            int x = padding + i * (cellWidth + 2);
            Gdiplus::RectF rect((Gdiplus::REAL)x, 0, (Gdiplus::REAL)cellWidth, (Gdiplus::REAL)totalHeight);

            if (activeAmount > 0.3f) {
                Gdiplus::SolidBrush hlBrush(Gdiplus::Color(
                    (BYTE)(60 * activeAmount), r, gv, b));
                Gdiplus::RectF hlRect(rect.X - 2, rect.Y + padding / 2.0f,
                                       rect.Width + 4, rect.Height - (float)padding);
                float hlRadius = 4.0f;
                Gdiplus::GraphicsPath hlPath;
                hlPath.AddArc(hlRect.X, hlRect.Y, hlRadius * 2, hlRadius * 2, 180, 90);
                hlPath.AddArc(hlRect.X + hlRect.Width - hlRadius * 2, hlRect.Y,
                               hlRadius * 2, hlRadius * 2, 270, 90);
                hlPath.AddArc(hlRect.X + hlRect.Width - hlRadius * 2,
                               hlRect.Y + hlRect.Height - hlRadius * 2,
                               hlRadius * 2, hlRadius * 2, 0, 90);
                hlPath.AddArc(hlRect.X, hlRect.Y + hlRect.Height - hlRadius * 2,
                               hlRadius * 2, hlRadius * 2, 90, 90);
                hlPath.CloseFigure();
                g.FillPath(&hlBrush, &hlPath);
            }

            wchar_t num[4];
            swprintf(num, 4, L"%d", i + 1);
            Gdiplus::SolidBrush textBrush(Gdiplus::Color(255, r, gv, b));
            g.DrawString(num, -1, &font, rect, &sf, &textBrush);
        }
    }

    *ppBmp = bmp;
    *pWidth = totalWidth;
    *pHeight = totalHeight;
}

static void RenderVignetteBitmap(Gdiplus::Bitmap** ppBmp, int width, int height, float alpha) {
    auto* bmp = new Gdiplus::Bitmap(width, height, PixelFormat32bppARGB);

    if (alpha <= 0.001f) {
        *ppBmp = bmp;
        return;
    }

    BYTE maxAlpha = (BYTE)(g_settings.vignetteIntensity * alpha);
    float cx = (float)width / 2.0f;
    float cy = (float)height / 2.0f;

    // Feather: 0 = hard edges (vignette starts near center)
    //        100 = soft (vignette only at extreme corners)
    float feather = g_settings.vignetteFeather / 100.0f;
    if (feather < 0.0f) feather = 0.0f;
    if (feather > 1.0f) feather = 1.0f;
    float innerRadius = feather * 0.8f; // gradient starts here (0 to 0.8)

    // Per-pixel vignette using LockBits for guaranteed full coverage
    Gdiplus::BitmapData bd;
    Gdiplus::Rect lockRect(0, 0, width, height);
    bmp->LockBits(&lockRect, Gdiplus::ImageLockModeWrite, PixelFormat32bppARGB, &bd);

    BYTE vR = g_settings.vigR;
    BYTE vG = g_settings.vigG;
    BYTE vB = g_settings.vigB;

    for (int y = 0; y < height; y++) {
        BYTE* row = (BYTE*)bd.Scan0 + y * bd.Stride;
        float dy = ((float)y - cy) / cy; // -1 to 1
        float dy2 = dy * dy;
        for (int x = 0; x < width; x++) {
            float dx = ((float)x - cx) / cx; // -1 to 1
            float dist = sqrtf(dx * dx + dy2);

            // Map distance to alpha: 0 inside innerRadius, ramp up outside
            float t = 0.0f;
            if (dist > innerRadius) {
                t = (dist - innerRadius) / (1.414f - innerRadius);
                if (t > 1.0f) t = 1.0f;
                t = t * t; // ease-in for smoother gradient
            }

            BYTE a = (BYTE)(maxAlpha * t);
            row[x * 4 + 0] = (BYTE)(vB * a / 255); // B (premultiplied)
            row[x * 4 + 1] = (BYTE)(vG * a / 255); // G (premultiplied)
            row[x * 4 + 2] = (BYTE)(vR * a / 255); // R (premultiplied)
            row[x * 4 + 3] = a;                      // A
        }
    }

    bmp->UnlockBits(&bd);
    *ppBmp = bmp;
}

// ============================================================================
// Overlay window update
// ============================================================================

static void UpdateIndicatorWindow(OverlayWindow& ow) {
    Gdiplus::Bitmap* bmp = nullptr;
    int bmpW = 0, bmpH = 0;

    RenderIndicatorBitmap(&bmp, &bmpW, &bmpH,
                           g_desktopCount, g_currentDesktop, g_previousDesktop,
                           g_indicatorAnimProgress);
    if (!bmp) return;

    RECT& mr = ow.monitorRect;
    int monW = mr.right - mr.left;
    int x = 0, y = 0;

    switch (g_settings.position) {
        case IndicatorPosition::TopLeft:
            x = mr.left + g_settings.offsetX;
            y = mr.top + g_settings.offsetY;
            break;
        case IndicatorPosition::TopCenter:
            x = mr.left + (monW - bmpW) / 2 + g_settings.offsetX;
            y = mr.top + g_settings.offsetY;
            break;
        case IndicatorPosition::TopRight:
            x = mr.right - bmpW - g_settings.offsetX;
            y = mr.top + g_settings.offsetY;
            break;
        case IndicatorPosition::BottomLeft:
            x = mr.left + g_settings.offsetX;
            y = mr.bottom - bmpH - g_settings.offsetY;
            break;
        case IndicatorPosition::BottomCenter:
            x = mr.left + (monW - bmpW) / 2 + g_settings.offsetX;
            y = mr.bottom - bmpH - g_settings.offsetY;
            break;
        case IndicatorPosition::BottomRight:
            x = mr.right - bmpW - g_settings.offsetX;
            y = mr.bottom - bmpH - g_settings.offsetY;
            break;
    }

    HDC hdcScreen = GetDC(nullptr);
    HDC hdcMem = CreateCompatibleDC(hdcScreen);
    HBITMAP hBmp = nullptr;
    bmp->GetHBITMAP(Gdiplus::Color(0, 0, 0, 0), &hBmp);
    HBITMAP hOld = (HBITMAP)SelectObject(hdcMem, hBmp);

    POINT ptPos = {x, y};
    SIZE szWnd = {bmpW, bmpH};
    POINT ptSrc = {0, 0};

    BYTE srcAlpha = 255;
    if (!g_settings.alwaysVisible) {
        srcAlpha = (BYTE)(255 * g_indicatorAlpha);
    }

    BLENDFUNCTION blend = {};
    blend.BlendOp = AC_SRC_OVER;
    blend.SourceConstantAlpha = srcAlpha;
    blend.AlphaFormat = AC_SRC_ALPHA;

    UpdateLayeredWindow(ow.hwnd, hdcScreen, &ptPos, &szWnd, hdcMem, &ptSrc, 0, &blend, ULW_ALPHA);

    SelectObject(hdcMem, hOld);
    DeleteObject(hBmp);
    DeleteDC(hdcMem);
    ReleaseDC(nullptr, hdcScreen);
    delete bmp;
}

static void EnsureVignetteCache(OverlayWindow& ow) {
    RECT& mr = ow.monitorRect;
    int w = mr.right - mr.left;
    int h = mr.bottom - mr.top;

    if (ow.hCachedVignette && ow.cachedVigW == w && ow.cachedVigH == h)
        return;

    if (ow.hCachedVignette) {
        DeleteObject(ow.hCachedVignette);
        ow.hCachedVignette = nullptr;
    }

    Gdiplus::Bitmap* bmp = nullptr;
    RenderVignetteBitmap(&bmp, w, h, 1.0f);
    if (!bmp) return;

    bmp->GetHBITMAP(Gdiplus::Color(0, 0, 0, 0), &ow.hCachedVignette);
    ow.cachedVigW = w;
    ow.cachedVigH = h;
    delete bmp;
}

static void UpdateVignetteWindow(OverlayWindow& ow) {
    RECT& mr = ow.monitorRect;
    int w = mr.right - mr.left;
    int h = mr.bottom - mr.top;

    EnsureVignetteCache(ow);
    if (!ow.hCachedVignette) return;

    HDC hdcScreen = GetDC(nullptr);
    HDC hdcMem = CreateCompatibleDC(hdcScreen);
    HBITMAP hOld = (HBITMAP)SelectObject(hdcMem, ow.hCachedVignette);

    POINT ptPos = {mr.left, mr.top};
    SIZE szWnd = {w, h};
    POINT ptSrc = {0, 0};

    BYTE alpha = (BYTE)(255 * g_vignetteAlpha);
    BLENDFUNCTION blend = {};
    blend.BlendOp = AC_SRC_OVER;
    blend.SourceConstantAlpha = alpha;
    blend.AlphaFormat = AC_SRC_ALPHA;

    UpdateLayeredWindow(ow.hwnd, hdcScreen, &ptPos, &szWnd, hdcMem, &ptSrc, 0, &blend, ULW_ALPHA);

    SelectObject(hdcMem, hOld);
    DeleteDC(hdcMem);
    ReleaseDC(nullptr, hdcScreen);
}

static void UpdateAllIndicators() {
    for (auto& ow : g_indicatorWindows) {
        UpdateIndicatorWindow(ow);
    }
}

static void UpdateAllVignettes() {
    for (auto& ow : g_vignetteWindows) {
        UpdateVignetteWindow(ow);
    }
}

// ============================================================================
// Animation
// ============================================================================

static void StartAnimation() {
    if (!g_animating && g_msgWindow) {
        g_animating = true;
        SetTimer(g_msgWindow, TIMER_ANIMATE, 16, nullptr);
    }
}

static void StopAnimation() {
    if (g_animating && g_msgWindow) {
        g_animating = false;
        KillTimer(g_msgWindow, TIMER_ANIMATE);
    }
}

static void OnDesktopChanged(int newDesktop) {
    Wh_Log(L"OnDesktopChanged: %d -> %d", g_currentDesktop, newDesktop);
    g_previousDesktop = g_currentDesktop;
    g_currentDesktop = newDesktop;
    g_switchTimestamp = GetTickCount();
    g_indicatorAnimProgress = 0.0f;

    if (g_settings.vignetteEnabled) {
        g_vignetteState = VignetteState::FadeIn;
        g_vignetteAlpha = 0.0f;
    }

    if (!g_settings.alwaysVisible) {
        g_indicatorAlpha = 1.0f;
        g_indicatorFadeState = IndicatorFadeState::Visible;
        for (auto& ow : g_indicatorWindows) {
            ShowWindow(ow.hwnd, SW_SHOWNOACTIVATE);
        }
    }

    StartAnimation();
}

static void AnimationTick() {
    bool needsUpdate = false;
    float dt = 16.0f;

    if (g_indicatorAnimProgress < 1.0f) {
        float step = dt / (float)(g_settings.animationSpeed > 1 ? g_settings.animationSpeed : 1);
        float newProg = g_indicatorAnimProgress + step;
        g_indicatorAnimProgress = (newProg < 1.0f) ? newProg : 1.0f;
        needsUpdate = true;
    }

    float vigHalfDuration = (float)g_settings.vignetteAnimSpeed / 2.0f;
    if (vigHalfDuration < 1.0f) vigHalfDuration = 1.0f;

    if (g_vignetteState == VignetteState::FadeIn) {
        g_vignetteAlpha += dt / vigHalfDuration;
        if (g_vignetteAlpha >= 1.0f) {
            g_vignetteAlpha = 1.0f;
            g_vignetteState = VignetteState::FadeOut;
        }
        needsUpdate = true;
    } else if (g_vignetteState == VignetteState::FadeOut) {
        g_vignetteAlpha -= dt / vigHalfDuration;
        if (g_vignetteAlpha <= 0.0f) {
            g_vignetteAlpha = 0.0f;
            g_vignetteState = VignetteState::Idle;
            UpdateAllVignettes();
        }
        needsUpdate = true;
    }

    if (!g_settings.alwaysVisible) {
        if (g_indicatorFadeState == IndicatorFadeState::Visible) {
            DWORD elapsed = GetTickCount() - g_switchTimestamp;
            if (elapsed >= (DWORD)g_settings.fadeDelay) {
                g_indicatorFadeState = IndicatorFadeState::FadingOut;
            }
        } else if (g_indicatorFadeState == IndicatorFadeState::FadingOut) {
            g_indicatorAlpha -= dt / 300.0f;
            if (g_indicatorAlpha <= 0.0f) {
                g_indicatorAlpha = 0.0f;
                g_indicatorFadeState = IndicatorFadeState::Hidden;
                for (auto& ow : g_indicatorWindows) {
                    ShowWindow(ow.hwnd, SW_HIDE);
                }
            }
            needsUpdate = true;
        }
    }

    if (needsUpdate) {
        UpdateAllIndicators();
        if (g_settings.vignetteEnabled && g_vignetteState != VignetteState::Idle) {
            UpdateAllVignettes();
        }
    }

    bool indicatorDone = g_indicatorAnimProgress >= 1.0f;
    bool vignetteDone = g_vignetteState == VignetteState::Idle;
    bool fadeDone = g_settings.alwaysVisible ||
                    g_indicatorFadeState == IndicatorFadeState::Hidden ||
                    g_indicatorFadeState == IndicatorFadeState::Visible;

    if (g_indicatorFadeState == IndicatorFadeState::Visible && !g_settings.alwaysVisible) {
        fadeDone = false;
    }

    if (indicatorDone && vignetteDone && fadeDone) {
        StopAnimation();
        if (g_vignetteAlpha <= 0.0f) {
            UpdateAllVignettes();
        }
    }
}

// ============================================================================
// Window creation / destruction
// ============================================================================

static LRESULT CALLBACK MsgWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_TIMER:
            if (wParam == TIMER_POLL) {
                int count = g_vdHelper.GetDesktopCount();
                int current = g_vdHelper.GetCurrentDesktopIndex();

                if (count > 0 && current >= 0 &&
                    (count != g_desktopCount || current != g_currentDesktop)) {
                    Wh_Log(L"Desktop change: count %d->%d, current %d->%d",
                           g_desktopCount, count, g_currentDesktop, current);
                    bool desktopChanged = (current != g_currentDesktop);
                    g_desktopCount = count;

                    if (desktopChanged) {
                        OnDesktopChanged(current);
                    } else {
                        UpdateAllIndicators();
                    }
                }
            } else if (wParam == TIMER_ANIMATE) {
                AnimationTick();
            }
            return 0;

        case WM_DISPLAYCHANGE:
            EnumerateMonitors();
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

static LRESULT CALLBACK OverlayWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

static void CreateOverlayWindows() {
    EnumerateMonitors();

    DWORD exStyle = WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TRANSPARENT |
                    WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE;
    DWORD style = WS_POPUP;

    for (size_t i = 0; i < g_monitors.size(); i++) {
        if (!g_settings.allMonitors && !g_monitors[i].isPrimary) continue;

        HWND hInd = CreateWindowExW(exStyle, INDICATOR_CLASS, L"",
                                     style, 0, 0, 1, 1,
                                     nullptr, nullptr, nullptr, nullptr);
        if (hInd) {
            OverlayWindow ow;
            ow.hwnd = hInd;
            ow.monitorIndex = (int)i;
            ow.monitorRect = g_monitors[i].rect;
            g_indicatorWindows.push_back(ow);

            if (g_settings.alwaysVisible) {
                ShowWindow(hInd, SW_SHOWNOACTIVATE);
            }
        }

        if (g_settings.vignetteEnabled) {
            HWND hVig = CreateWindowExW(exStyle, VIGNETTE_CLASS, L"",
                                         style, 0, 0, 1, 1,
                                         nullptr, nullptr, nullptr, nullptr);
            if (hVig) {
                OverlayWindow ow;
                ow.hwnd = hVig;
                ow.monitorIndex = (int)i;
                ow.monitorRect = g_monitors[i].rect;
                g_vignetteWindows.push_back(ow);
                ShowWindow(hVig, SW_SHOWNOACTIVATE);
            }
        }
    }

    g_indicatorAnimProgress = 1.0f;
    UpdateAllIndicators();
    UpdateAllVignettes();
}

static void DestroyOverlayWindows() {
    for (auto& ow : g_indicatorWindows) {
        if (ow.hwnd) DestroyWindow(ow.hwnd);
    }
    g_indicatorWindows.clear();

    for (auto& ow : g_vignetteWindows) {
        if (ow.hCachedVignette) DeleteObject(ow.hCachedVignette);
        if (ow.hwnd) DestroyWindow(ow.hwnd);
    }
    g_vignetteWindows.clear();
}

// ============================================================================
// Settings loading
// ============================================================================

static void LoadSettings() {
    PCWSTR posStr = Wh_GetStringSetting(L"position");
    if (wcscmp(posStr, L"top-left") == 0)         g_settings.position = IndicatorPosition::TopLeft;
    else if (wcscmp(posStr, L"top-center") == 0)   g_settings.position = IndicatorPosition::TopCenter;
    else if (wcscmp(posStr, L"top-right") == 0)    g_settings.position = IndicatorPosition::TopRight;
    else if (wcscmp(posStr, L"bottom-left") == 0)  g_settings.position = IndicatorPosition::BottomLeft;
    else if (wcscmp(posStr, L"bottom-center") == 0)g_settings.position = IndicatorPosition::BottomCenter;
    else if (wcscmp(posStr, L"bottom-right") == 0) g_settings.position = IndicatorPosition::BottomRight;
    else                                           g_settings.position = IndicatorPosition::TopLeft;
    Wh_FreeStringSetting(posStr);

    g_settings.offsetX = Wh_GetIntSetting(L"offsetX");
    g_settings.offsetY = Wh_GetIntSetting(L"offsetY");

    PCWSTR styleStr = Wh_GetStringSetting(L"indicatorStyle");
    g_settings.style = (wcscmp(styleStr, L"numbers") == 0)
        ? IndicatorStyle::Numbers : IndicatorStyle::Dots;
    Wh_FreeStringSetting(styleStr);

    g_settings.indicatorSize = Wh_GetIntSetting(L"indicatorSize");
    if (g_settings.indicatorSize < 6) g_settings.indicatorSize = 14;

    g_settings.activeR   = (BYTE)Wh_GetIntSetting(L"indicatorActiveColor.red");
    g_settings.activeG   = (BYTE)Wh_GetIntSetting(L"indicatorActiveColor.green");
    g_settings.activeB   = (BYTE)Wh_GetIntSetting(L"indicatorActiveColor.blue");
    g_settings.inactiveR = (BYTE)Wh_GetIntSetting(L"indicatorInactiveColor.red");
    g_settings.inactiveG = (BYTE)Wh_GetIntSetting(L"indicatorInactiveColor.green");
    g_settings.inactiveB = (BYTE)Wh_GetIntSetting(L"indicatorInactiveColor.blue");

    g_settings.showBackground = Wh_GetIntSetting(L"showBackground");
    g_settings.bgR       = (BYTE)Wh_GetIntSetting(L"indicatorBgColor.red");
    g_settings.bgG       = (BYTE)Wh_GetIntSetting(L"indicatorBgColor.green");
    g_settings.bgB       = (BYTE)Wh_GetIntSetting(L"indicatorBgColor.blue");
    g_settings.bgOpacity = (BYTE)Wh_GetIntSetting(L"indicatorBgOpacity");

    g_settings.alwaysVisible = Wh_GetIntSetting(L"indicatorAlwaysVisible");
    g_settings.fadeDelay     = Wh_GetIntSetting(L"indicatorFadeDelay");
    g_settings.animationSpeed= Wh_GetIntSetting(L"indicatorAnimationSpeed");

    g_settings.vignetteEnabled  = Wh_GetIntSetting(L"vignetteEnabled");
    g_settings.vigR             = (BYTE)Wh_GetIntSetting(L"vignetteColor.red");
    g_settings.vigG             = (BYTE)Wh_GetIntSetting(L"vignetteColor.green");
    g_settings.vigB             = (BYTE)Wh_GetIntSetting(L"vignetteColor.blue");
    g_settings.vignetteIntensity= (BYTE)Wh_GetIntSetting(L"vignetteIntensity");
    g_settings.vignetteFeather  = Wh_GetIntSetting(L"vignetteFeather");
    g_settings.vignetteAnimSpeed= Wh_GetIntSetting(L"vignetteAnimationSpeed");

    g_settings.allMonitors = Wh_GetIntSetting(L"allMonitors");
}

// ============================================================================
// Worker thread (owns all windows, timers, and the message loop)
// ============================================================================

static DWORD WINAPI WorkerThreadProc(LPVOID) {
    Wh_Log(L"Worker thread started");

    // Register window classes on this thread
    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = OverlayWndProc;
    wc.hInstance = nullptr;
    wc.lpszClassName = INDICATOR_CLASS;
    RegisterClassExW(&wc);

    wc.lpszClassName = VIGNETTE_CLASS;
    RegisterClassExW(&wc);

    WNDCLASSEXW msgWc = {};
    msgWc.cbSize = sizeof(msgWc);
    msgWc.lpfnWndProc = MsgWndProc;
    msgWc.hInstance = nullptr;
    msgWc.lpszClassName = L"WHVDMsgWindow";
    RegisterClassExW(&msgWc);

    g_msgWindow = CreateWindowExW(0, L"WHVDMsgWindow", L"",
                                   0, 0, 0, 0, 0,
                                   HWND_MESSAGE, nullptr, nullptr, nullptr);

    CreateOverlayWindows();

    if (g_msgWindow) {
        SetTimer(g_msgWindow, TIMER_POLL, 20, nullptr);
        Wh_Log(L"Poll timer started on worker thread");
    } else {
        Wh_Log(L"ERROR: Failed to create message window");
    }

    // Run message loop
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    Wh_Log(L"Worker thread message loop exited");

    // Cleanup on this thread
    StopAnimation();

    if (g_msgWindow) {
        KillTimer(g_msgWindow, TIMER_POLL);
        DestroyWindow(g_msgWindow);
        g_msgWindow = nullptr;
    }

    DestroyOverlayWindows();

    UnregisterClassW(INDICATOR_CLASS, nullptr);
    UnregisterClassW(VIGNETTE_CLASS, nullptr);
    UnregisterClassW(L"WHVDMsgWindow", nullptr);

    return 0;
}

// ============================================================================
// Mod lifecycle
// ============================================================================

BOOL Wh_ModInit() {
    Wh_Log(L"Init - Virtual Desktop Indicator");

    LoadSettings();

    Gdiplus::GdiplusStartupInput gdipInput;
    if (Gdiplus::GdiplusStartup(&g_gdiplusToken, &gdipInput, nullptr) != Gdiplus::Ok) {
        Wh_Log(L"Failed to initialize GDI+");
        return FALSE;
    }

    if (!g_vdHelper.Init()) {
        Wh_Log(L"Failed to initialize virtual desktop helper");
    }

    g_desktopCount = g_vdHelper.GetDesktopCount();
    g_currentDesktop = g_vdHelper.GetCurrentDesktopIndex();
    if (g_desktopCount < 1) g_desktopCount = 1;
    if (g_currentDesktop < 0) g_currentDesktop = 0;
    g_previousDesktop = g_currentDesktop;

    // Start worker thread with its own message loop
    g_workerThread = CreateThread(nullptr, 0, WorkerThreadProc, nullptr, 0, &g_workerThreadId);
    if (!g_workerThread) {
        Wh_Log(L"Failed to create worker thread");
        return FALSE;
    }

    Wh_Log(L"Virtual Desktop Indicator initialized (desktops: %d, current: %d)",
            g_desktopCount, g_currentDesktop);
    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Uninit - Virtual Desktop Indicator");

    // Signal worker thread to exit
    if (g_workerThreadId) {
        PostThreadMessage(g_workerThreadId, WM_QUIT, 0, 0);
    }
    if (g_workerThread) {
        WaitForSingleObject(g_workerThread, 5000);
        CloseHandle(g_workerThread);
        g_workerThread = nullptr;
        g_workerThreadId = 0;
    }

    g_vdHelper.Release();

    if (g_gdiplusToken) {
        Gdiplus::GdiplusShutdown(g_gdiplusToken);
        g_gdiplusToken = 0;
    }
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    Wh_Log(L"Settings changed - reloading");
    *bReload = TRUE;
    return TRUE;
}
