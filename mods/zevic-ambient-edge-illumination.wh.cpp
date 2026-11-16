// ==WindhawkMod==
// @id              zevic-ambient-edge-illumination
// @name            Ambient Screen Edge Glow
// @description     Sony Bravia-inspired ambient edge illumination with smooth orbiting light beams.
// @version         1.1.0
// @author          Abdul Ahad
// @github          https://github.com/ahadtabc/zevic
// @homepage        https://github.com/ahadtabc/zevic
// @include         explorer.exe
// @compilerOptions -lgdi32 -ldwmapi -luser32 -lgdiplus -lshlwapi -lole32 -lshell32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# ZEVIC — Ambient Screen Glow

A luxurious, high-performance screen edge illumination mod inspired by Sony Bravia cinema displays and AMOLED edge lighting. A sleek light beam orbits smoothly along the screen perimeter with zero lag and zero distraction.

### Quick Start
- **Toggle On / Off**: Press `Ctrl + Shift + Z` anytime.
- **Default Theme**: Sony Bravia Cyan (Electric Blue).
- **Default Speed**: 8 seconds per loop.
- **Master Edge Inset**: `-3 px` (pre-calibrated for a seamless bezel-flush fit).

### Key Features
- **11 Curated Color Presets**: Sony Bravia Cyan, Pure White, Cyberpunk Neon, AMOLED Gold, Plasma Violet, Emerald Neon, Ice Blue, Warm White, Rose Gold, Subtle Amber, and RGB Rainbow.
- **Custom RGB Mode**: Full 24-bit Red, Green, and Blue manual color adjustments.
- **360° Chromatic Hue Glide**: Smooth automated rainbow color flow around the full spectrum with customizable speed.
- **Configurable Beams**: 1 (Single), 2 (Dual opposite), 3 (Triangle), or 4 (Cross) orbiting beams.
- **Clockwise & Anti-Clockwise**: Switch orbit direction with one click.
- **Dual-Pass Atmospheric Halo**: Glowing beam head with soft atmospheric light spread and optical taper.
- **Sharp & Rounded Corner Styles**: Choose between razor-sharp 90° AMOLED corners or customizable curved bezel corners.
- **Bezel & Taskbar Calibration**: Calibrate Master Inset (`-3 px` default) and individual 4-edge offsets (Top, Bottom, Left, Right) for perfect taskbar and bezel alignment.
- **Ambient Base Line**: Optional subtle static perimeter border with configurable opacity and thickness.
- **Multi-Monitor Support**: Automatically spawns and animates hardware-synchronized overlays across all connected displays.
- **Smart Game Auto-Pause**: Automatically detects fullscreen 3D games and applications, pausing overlays to eliminate GPU/CPU overhead.
- **Battery Saver / Power Intelligence**: Detects laptop battery vs AC power and optimizes refresh rate (30 FPS on battery, 60 FPS on AC).
- **Zero Distraction & Click-Through**: Fully transparent (`WS_EX_TRANSPARENT`), non-activating (`WS_EX_NOACTIVATE`), and never intercepts mouse clicks or steals keyboard focus.

---
*Created by **Abdul Ahad** · [GitHub Repository](https://github.com/ahadtabc/zevic) · [LinkedIn Profile](https://www.linkedin.com/in/abdul-ahadit)*
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- enabled: true
  $name: "Enable Glow"
  $description: "Master toggle for edge illumination (or press Ctrl+Shift+Z)."

- colorPreset: "sony_bravia_cyan"
  $name: "Color Preset"
  $description: "Select signature color theme or custom RGB."
  $options:
    - sony_bravia_cyan: "Sony Bravia Cyan (Default)"
    - pure_white: "Pure White"
    - rgb: "RGB Rainbow"
    - cyberpunk: "Cyberpunk Neon"
    - amoled_gold: "AMOLED Gold"
    - plasma_violet: "Plasma Violet"
    - emerald_neon: "Emerald Neon"
    - ice_blue: "Ice Blue"
    - warm_white: "Warm White"
    - rose_gold: "Rose Gold"
    - amber: "Subtle Amber"
    - custom: "Custom RGB"

- colorCycle: false
  $name: "Continuous Hue Glide"
  $description: "Cycle smoothly through 360° spectrum colors."

- colorCycleSpeedSec: 10
  $name: "Hue Cycle Speed (s)"
  $description: "Seconds for one full color rotation."

- illuminationBeams: "single"
  $name: "Beams"
  $description: "Number of light beams orbiting the screen."
  $options:
    - single: "Single"
    - dual: "Dual"
    - triple: "Triple"
    - quad: "Quad"

- direction: "clockwise"
  $name: "Direction"
  $description: "Orbit direction around the screen."
  $options:
    - clockwise: "Clockwise"
    - anticlockwise: "Anti-Clockwise"

- speedSec: 8
  $name: "Rotation Speed (s)"
  $description: "Seconds for one full loop around the screen (default 8)."

- edgeInset: -3
  $name: "Master Edge Inset (px)"
  $description: "Edge calibration in pixels (default -3 for zero bezel gap)."

- sparkLength: 80
  $name: "Beam Length (px)"
  $description: "Length of the glowing head streak (default 80)."

- sparkThickness: 3
  $name: "Beam Thickness (px)"
  $description: "Line thickness in pixels (default 3)."

- sparkGlowBlur: 15
  $name: "Glow Halo Spread"
  $description: "Soft atmospheric glow halo (default 15)."

- tailMultiplier: 25
  $name: "Tail Length (tenths)"
  $description: "Tail multiplier relative to beam head (25 = 2.5x)."

- tailFadeEnd: 0
  $name: "Tail End Fade (%)"
  $description: "Tail tip opacity percentage (default 0 for smooth fade)."

- sparkBrightness: 100
  $name: "Brightness (%)"
  $description: "Maximum beam brightness (default 100)."

- cornerStyle: "sharp"
  $name: "Corner Style"
  $description: "Sharp 90° corners or rounded bezel curves."
  $options:
    - sharp: "Sharp (90°)"
    - rounded: "Rounded"

- cornerRadius: 0
  $name: "Corner Radius (px)"
  $description: "Corner radius when Corner Style is Rounded."

- edgeOffsetTop: 0
  $name: "Top Offset (px)"
  $description: "Top border fine-tuning in pixels."

- edgeOffsetBottom: 0
  $name: "Bottom Offset (px)"
  $description: "Bottom border fine-tuning (e.g. taskbar alignment)."

- edgeOffsetLeft: 0
  $name: "Left Offset (px)"
  $description: "Left border fine-tuning in pixels."

- edgeOffsetRight: 0
  $name: "Right Offset (px)"
  $description: "Right border fine-tuning in pixels."

- showBaseLine: true
  $name: "Show Base Line"
  $description: "Faint static ambient border line around the screen."

- baseLineOpacity: 8
  $name: "Base Line Opacity (%)"
  $description: "Ambient border line opacity percentage (default 8)."

- baseLineThickness: 2
  $name: "Base Line Thickness (px)"
  $description: "Ambient border line thickness in pixels (default 2)."

- hideInFullscreen: false
  $name: "Pause in Fullscreen"
  $description: "Automatically hide glow in fullscreen games and apps."

- batterySaverAware: true
  $name: "Battery Saver"
  $description: "Optimize refresh rate on laptop battery to save power."

- color:
  - red: 0
  - green: 210
  - blue: 255
  $name: "Custom RGB Color"
  $description: "RGB values used when Color Preset is set to Custom."
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windowsx.h>
#include <gdiplus.h>
#include <dwmapi.h>
#include <shellapi.h>
#include <shlwapi.h>
#include <cmath>
#include <vector>
#include <chrono>
#include <atomic>

using namespace Gdiplus;

// ═══════════════════════════════════════════════════════════════════════════
// AUTHOR & METADATA IDENTIFIERS
// ═══════════════════════════════════════════════════════════════════════════
#define ZEVIC_AUTHOR_NAME       L"Abdul Ahad"
#define ZEVIC_GITHUB_URL        L"https://github.com/ahadtabc/zevic"
#define ZEVIC_LINKEDIN_URL      L"https://www.linkedin.com/in/abdul-ahadit"
#define ZEVIC_WINDOW_CLASS      L"Zevic_AmbientEdgeGlow_Overlay"
#define ZEVIC_HOTKEY_ID         0x5A45 // 'ZE' (Zevic Edge)

// ═══════════════════════════════════════════════════════════════════════════
// DATA STRUCTURES & CONFIGURATION
// ═══════════════════════════════════════════════════════════════════════════
struct PointD {
    double x;
    double y;
};

struct ModSettings {
    bool enabled = true;
    double rotationTimeSec = 8.0;
    bool clockwise = true;
    double sparkLengthPx = 80.0;
    double sparkThickness = 3.0;
    double sparkBrightness = 1.0;
    double sparkGlowBlur = 15.0;
    double tailLengthMultiplier = 2.5;
    double tailFadeEnd = 0.0;
    int sparkCount = 1;
    BYTE red = 0;
    BYTE green = 210;
    BYTE blue = 255;
    bool rgbRainbow = false;
    bool colorCycle = false;
    double colorCycleSpeedSec = 10.0;
    bool showBaseLine = true;
    double baseLineOpacity = 0.08;
    double baseLineThickness = 2.0;
    double cornerRadiusPx = 0.0;
    double edgeInset = -3.0;
    double edgeOffsetTop = 0.0;
    double edgeOffsetBottom = 0.0;
    double edgeOffsetLeft = 0.0;
    double edgeOffsetRight = 0.0;
    bool hideInFullscreen = false;
    bool batterySaverAware = true;
};

static ModSettings g_settings;
static std::atomic<bool> g_threadRunning(false);
static HANDLE g_hOverlayThread = nullptr;
static DWORD g_overlayThreadId = 0;
static ULONG_PTR g_gdiplusToken = 0;

// Per-monitor overlay state
struct MonitorOverlay {
    HWND hWnd;
    RECT rcMonitor;
    int width;
    int height;
    HDC hdcMem;
    HBITMAP hBitmap;
    void* pBits;
    HGDIOBJ hOldBitmap;
};

static std::vector<MonitorOverlay> g_monitors;

// ═══════════════════════════════════════════════════════════════════════════
// POWER INTELLIGENCE (Laptop Battery / Battery Saver Detection)
// ═══════════════════════════════════════════════════════════════════════════
static bool IsRunningOnBattery() {
    SYSTEM_POWER_STATUS sps;
    if (GetSystemPowerStatus(&sps)) {
        // BatteryFlag 128 = No system battery (desktop PC)
        if (sps.BatteryFlag != 128 && sps.BatteryFlag != 255) {
            // ACLineStatus == 0 means running on laptop battery
            if (sps.ACLineStatus == 0) return true;
        }
    }
    return false;
}

// ═══════════════════════════════════════════════════════════════════════════
// COLOR CONVERSION HELPERS
// ═══════════════════════════════════════════════════════════════════════════
static Color HsvToColor(double h, double s, double v, BYTE alpha = 255) {
    int hi = (int)(h * 6.0) % 6;
    double f = h * 6.0 - std::floor(h * 6.0);
    double p = v * (1.0 - s);
    double q = v * (1.0 - f * s);
    double t = v * (1.0 - (1.0 - f) * s);

    double r = 0, g = 0, b = 0;
    switch (hi) {
        case 0: r = v; g = t; b = p; break;
        case 1: r = q; g = v; b = p; break;
        case 2: r = p; g = v; b = t; break;
        case 3: r = p; g = q; b = v; break;
        case 4: r = t; g = p; b = v; break;
        default: r = v; g = p; b = q; break;
    }

    return Color(alpha, (BYTE)(r * 255.0), (BYTE)(g * 255.0), (BYTE)(b * 255.0));
}

// ═══════════════════════════════════════════════════════════════════════════
// PERIMETER GEOMETRY MATHEMATICS
// ═══════════════════════════════════════════════════════════════════════════
static PointD PerimeterToPoint(double distance, double left, double top, double right, double bottom,
                               double pathW, double pathH, double cr, double perimeter) {
    distance = std::fmod(distance, perimeter);
    if (distance < 0) distance += perimeter;

    if (cr <= 0.001) {
        // Pure sharp 90-degree rectangle joint (AMOLED perfection)
        if (distance <= pathW)
            return { left + distance, top };
        distance -= pathW;

        if (distance <= pathH)
            return { right, top + distance };
        distance -= pathH;

        if (distance <= pathW)
            return { right - distance, bottom };
        distance -= pathW;

        return { left, bottom - distance };
    }

    // Curved Rounded Rectangle Geometry
    const double PI_CONST = 3.14159265358979323846;
    double straightW = pathW - 2.0 * cr;
    double straightH = pathH - 2.0 * cr;
    double arcLen = PI_CONST * cr / 2.0;

    // Top straight
    if (distance <= straightW)
        return { left + cr + distance, top };
    distance -= straightW;

    // Top-Right arc
    if (distance <= arcLen) {
        double angle = -PI_CONST / 2.0 + (distance / cr);
        return { right - cr + std::cos(angle) * cr,
                 top + cr + std::sin(angle) * cr };
    }
    distance -= arcLen;

    // Right straight
    if (distance <= straightH)
        return { right, top + cr + distance };
    distance -= straightH;

    // Bottom-Right arc
    if (distance <= arcLen) {
        double angle = (distance / cr);
        return { right - cr + std::cos(angle) * cr,
                 bottom - cr + std::sin(angle) * cr };
    }
    distance -= arcLen;

    // Bottom straight
    if (distance <= straightW)
        return { right - cr - distance, bottom };
    distance -= straightW;

    // Bottom-Left arc
    if (distance <= arcLen) {
        double angle = PI_CONST / 2.0 + (distance / cr);
        return { left + cr + std::cos(angle) * cr,
                 bottom - cr + std::sin(angle) * cr };
    }
    distance -= arcLen;

    // Left straight
    if (distance <= straightH)
        return { left, bottom - cr - distance };
    distance -= straightH;

    // Top-Left arc
    double angle = PI_CONST + (distance / cr);
    return { left + cr + std::cos(angle) * cr,
             top + cr + std::sin(angle) * cr };
}

static void DrawSegment(Graphics& g, Pen* haloPen, Pen& segPen, double d0, double d1,
                        double left, double top, double right, double bottom,
                        double pathW, double pathH, double cr, double perimeter) {
    d0 = std::fmod(d0, perimeter); if (d0 < 0) d0 += perimeter;
    d1 = std::fmod(d1, perimeter); if (d1 < 0) d1 += perimeter;

    if (cr <= 0.001) {
        double corners[4] = { pathW, pathW + pathH, 2.0 * pathW + pathH, 2.0 * pathW + 2.0 * pathH };
        for (int i = 0; i < 4; i++) {
            double c = corners[i];
            bool crosses = (d1 > d0 && c > d0 + 0.001 && c < d1 - 0.001) ||
                           (d0 > d1 && (c > d0 + 0.001 || c < d1 - 0.001));
            if (crosses) {
                PointD p0 = PerimeterToPoint(d0, left, top, right, bottom, pathW, pathH, cr, perimeter);
                PointD pC = PerimeterToPoint(c, left, top, right, bottom, pathW, pathH, cr, perimeter);
                PointD p1 = PerimeterToPoint(d1, left, top, right, bottom, pathW, pathH, cr, perimeter);

                if (haloPen) {
                    g.DrawLine(haloPen, (REAL)p0.x, (REAL)p0.y, (REAL)pC.x, (REAL)pC.y);
                    g.DrawLine(haloPen, (REAL)pC.x, (REAL)pC.y, (REAL)p1.x, (REAL)p1.y);
                }
                g.DrawLine(&segPen, (REAL)p0.x, (REAL)p0.y, (REAL)pC.x, (REAL)pC.y);
                g.DrawLine(&segPen, (REAL)pC.x, (REAL)pC.y, (REAL)p1.x, (REAL)p1.y);
                return;
            }
        }
    }

    PointD ptA = PerimeterToPoint(d0, left, top, right, bottom, pathW, pathH, cr, perimeter);
    PointD ptB = PerimeterToPoint(d1, left, top, right, bottom, pathW, pathH, cr, perimeter);
    if (haloPen) {
        g.DrawLine(haloPen, (REAL)ptA.x, (REAL)ptA.y, (REAL)ptB.x, (REAL)ptB.y);
    }
    g.DrawLine(&segPen, (REAL)ptA.x, (REAL)ptA.y, (REAL)ptB.x, (REAL)ptB.y);
}

// ═══════════════════════════════════════════════════════════════════════════
// FULLSCREEN GAME / 3D APP DETECTION
// ═══════════════════════════════════════════════════════════════════════════
static bool IsFullscreenAppRunning() {
    HWND fg = GetForegroundWindow();
    if (!fg) return false;

    // Ignore Windows desktop and taskbar surfaces
    wchar_t cls[64] = { 0 };
    if (GetClassNameW(fg, cls, 64)) {
        if (wcscmp(cls, L"Progman") == 0 ||
            wcscmp(cls, L"WorkerW") == 0 ||
            wcscmp(cls, L"Shell_TrayWnd") == 0 ||
            wcscmp(cls, L"Shell_SecondaryTrayWnd") == 0 ||
            wcscmp(cls, ZEVIC_WINDOW_CLASS) == 0) {
            return false;
        }
    }

    // Check if foreground window covers any screen entirely
    RECT fgRect;
    if (GetWindowRect(fg, &fgRect)) {
        HMONITOR hMon = MonitorFromWindow(fg, MONITOR_DEFAULTTONEAREST);
        MONITORINFO mi = { sizeof(MONITORINFO) };
        if (GetMonitorInfoW(hMon, &mi)) {
            if (fgRect.left <= mi.rcMonitor.left &&
                fgRect.top <= mi.rcMonitor.top &&
                fgRect.right >= mi.rcMonitor.right &&
                fgRect.bottom >= mi.rcMonitor.bottom) {
                if (!IsIconic(fg) && IsWindowVisible(fg)) {
                    return true;
                }
            }
        }
    }
    return false;
}

// ═══════════════════════════════════════════════════════════════════════════
// RENDERING ENGINE (Per-Frame GDI+ with 32-bit PARGB DIBSection)
// ═══════════════════════════════════════════════════════════════════════════
static void RenderOverlayFrame(MonitorOverlay& mon, double progress, double nowSeconds) {
    if (mon.width <= 20 || mon.height <= 20 || !mon.hdcMem || !mon.pBits) return;

    // Check fullscreen auto-pause
    if (g_settings.hideInFullscreen && IsFullscreenAppRunning()) {
        ZeroMemory(mon.pBits, mon.width * mon.height * 4);
        POINT ptSrc = { 0, 0 };
        POINT ptDst = { mon.rcMonitor.left, mon.rcMonitor.top };
        SIZE sz = { mon.width, mon.height };
        BLENDFUNCTION blend = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
        UpdateLayeredWindow(mon.hWnd, NULL, &ptDst, &sz, mon.hdcMem, &ptSrc, 0, &blend, ULW_ALPHA);
        return;
    }

    // Direct Pre-multiplied ARGB Bitmap wrapper over the DIBSection memory
    Bitmap bitmap(mon.width, mon.height, mon.width * 4, PixelFormat32bppPARGB, (BYTE*)mon.pBits);
    Graphics g(&bitmap);
    g.SetSmoothingMode(SmoothingModeAntiAlias);
    g.Clear(Color(0, 0, 0, 0)); // fully transparent

    if (!g_settings.enabled) {
        POINT ptSrc = { 0, 0 };
        POINT ptDst = { mon.rcMonitor.left, mon.rcMonitor.top };
        SIZE sz = { mon.width, mon.height };
        BLENDFUNCTION blend = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
        UpdateLayeredWindow(mon.hWnd, NULL, &ptDst, &sz, mon.hdcMem, &ptSrc, 0, &blend, ULW_ALPHA);
        return;
    }

    double baseThickness = std::max(1.0, std::min(10.0, g_settings.sparkThickness));
    double halfThick = baseThickness / 2.0;

    // Master edge inset and 4-edge offsets
    double inset = g_settings.edgeInset;
    double left = halfThick + inset + g_settings.edgeOffsetLeft;
    double top = halfThick + inset + g_settings.edgeOffsetTop;
    double right = std::max(left + 20.0, (double)mon.width - halfThick - inset - g_settings.edgeOffsetRight);
    double bottom = std::max(top + 20.0, (double)mon.height - halfThick - inset - g_settings.edgeOffsetBottom);
    double pathW = right - left;
    double pathH = bottom - top;

    double cr = std::max(0.0, std::min(g_settings.cornerRadiusPx, std::min(pathW, pathH) / 4.0));
    const double PI_CONST = 3.14159265358979323846;
    double perimeter = (cr > 0.001)
        ? (2.0 * (pathW - 2.0 * cr) + 2.0 * (pathH - 2.0 * cr) + 2.0 * PI_CONST * cr)
        : (2.0 * pathW + 2.0 * pathH);

    if (perimeter <= 0) return;

    // 1. Draw Ambient Base Line Border
    if (g_settings.showBaseLine && g_settings.baseLineOpacity > 0.001) {
        BYTE baseAlpha = (BYTE)(255.0 * std::max(0.0, std::min(1.0, g_settings.baseLineOpacity)));
        Color baseCol;
        if (g_settings.rgbRainbow) {
            baseCol = HsvToColor(std::fmod(nowSeconds * 0.08, 1.0), 0.85, 1.0, baseAlpha);
        } else {
            baseCol = Color(baseAlpha, g_settings.red, g_settings.green, g_settings.blue);
        }

        REAL bt = (REAL)std::max(0.5, std::min(10.0, g_settings.baseLineThickness));
        Pen basePen(baseCol, bt);

        if (cr <= 0.001) {
            g.DrawRectangle(&basePen, (REAL)left, (REAL)top, (REAL)pathW, (REAL)pathH);
        } else {
            GraphicsPath path;
            REAL d = (REAL)(cr * 2.0);
            path.AddArc((REAL)left, (REAL)top, d, d, 180.0f, 90.0f);
            path.AddArc((REAL)(right - d), (REAL)top, d, d, 270.0f, 90.0f);
            path.AddArc((REAL)(right - d), (REAL)(bottom - d), d, d, 0.0f, 90.0f);
            path.AddArc((REAL)left, (REAL)(bottom - d), d, d, 90.0f, 90.0f);
            path.CloseFigure();
            g.DrawPath(&basePen, &path);
        }
    }

    // 2. Draw Clockwise / Anti-Clockwise Travelling Spark Beams
    double sparkLengthPx = std::max(20.0, g_settings.sparkLengthPx);
    double tailLengthPx = sparkLengthPx * std::max(0.5, std::min(6.0, g_settings.tailLengthMultiplier));
    double totalLength = sparkLengthPx + tailLengthPx;
    int sparkCount = std::max(1, std::min(4, g_settings.sparkCount));

    Color currentSparkColor(255, g_settings.red, g_settings.green, g_settings.blue);
    if (!g_settings.rgbRainbow && g_settings.colorCycle) {
        double cycleSpeed = std::max(1.0, g_settings.colorCycleSpeedSec);
        double hue = std::fmod(nowSeconds / cycleSpeed, 1.0);
        currentSparkColor = HsvToColor(hue, 0.85, 1.0);
    }

    int segments = g_settings.rgbRainbow ? 48 : 36;
    double rgbTimePhase = (nowSeconds / 3.0) * (g_settings.clockwise ? 1.0 : -1.0);

    for (int i = 0; i < sparkCount; i++) {
        double sparkProgress = std::fmod(progress + (double)i / sparkCount, 1.0);
        if (sparkProgress < 0) sparkProgress += 1.0;
        double headDist = sparkProgress * perimeter;

        for (int s = 1; s <= segments; s++) {
            double fracPrev = (double)(s - 1) / segments;
            double fracCurr = (double)s / segments;

            double distPrev = headDist - (1.0 - fracPrev) * totalLength;
            double distCurr = headDist - (1.0 - fracCurr) * totalLength;

            double headRegionFrac = sparkLengthPx / totalLength;
            double segOpacity;
            double segThickness;

            if ((1.0 - fracCurr) <= headRegionFrac) {
                // Head: full luminous brightness
                segOpacity = g_settings.sparkBrightness;
                segThickness = baseThickness;
            } else {
                // Tail: smooth optical taper
                double tailFrac = ((1.0 - fracCurr) - headRegionFrac) / (1.0 - headRegionFrac);
                segOpacity = g_settings.sparkBrightness * (1.0 - tailFrac * (1.0 - g_settings.tailFadeEnd));
                segThickness = baseThickness * (1.0 - tailFrac * 0.45);
            }

            BYTE a = (BYTE)(255.0 * std::max(0.0, std::min(1.0, segOpacity)));
            if (a > 2) {
                Color segBaseColor;
                if (g_settings.rgbRainbow) {
                    double hue = std::fmod(rgbTimePhase + (fracCurr * 1.25), 1.0);
                    if (hue < 0) hue += 1.0;
                    segBaseColor = HsvToColor(hue, 1.0, 1.0, a);
                } else {
                    segBaseColor = Color(a, currentSparkColor.GetR(), currentSparkColor.GetG(), currentSparkColor.GetB());
                }

                Pen* haloPenPtr = nullptr;
                Pen haloPen(Color((BYTE)(a * 0.28), segBaseColor.GetR(), segBaseColor.GetG(), segBaseColor.GetB()), (REAL)(segThickness + 3.5));
                if (fracCurr > 0.40 && g_settings.sparkGlowBlur > 5) {
                    haloPen.SetStartCap(LineCapFlat);
                    haloPen.SetEndCap(LineCapFlat);
                    haloPenPtr = &haloPen;
                }

                Pen segPen(segBaseColor, (REAL)std::max(0.5, segThickness));
                segPen.SetStartCap(LineCapFlat);
                segPen.SetEndCap(LineCapFlat);

                DrawSegment(g, haloPenPtr, segPen, distPrev, distCurr,
                            left, top, right, bottom, pathW, pathH, cr, perimeter);
            }
        }
    }

    // Submit pre-multiplied 32-bit ARGB surface directly to Windows Desktop Window Manager (DWM)
    POINT ptSrc = { 0, 0 };
    POINT ptDst = { mon.rcMonitor.left, mon.rcMonitor.top };
    SIZE sz = { mon.width, mon.height };
    BLENDFUNCTION blend = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
    UpdateLayeredWindow(mon.hWnd, NULL, &ptDst, &sz, mon.hdcMem, &ptSrc, 0, &blend, ULW_ALPHA);
}

// ═══════════════════════════════════════════════════════════════════════════
// MULTI-MONITOR OVERLAY MANAGEMENT & WIN32 MESSAGE LOOP
// ═══════════════════════════════════════════════════════════════════════════
static BOOL CALLBACK EnumMonitorsProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData) {
    auto* monitorsList = reinterpret_cast<std::vector<RECT>*>(dwData);
    if (lprcMonitor) {
        monitorsList->push_back(*lprcMonitor);
    }
    return TRUE;
}

static void CreateMonitorOverlays() {
    std::vector<RECT> monitorRects;
    EnumDisplayMonitors(NULL, NULL, EnumMonitorsProc, (LPARAM)&monitorRects);

    HDC hdcScreen = GetDC(NULL);

    for (const auto& rc : monitorRects) {
        int w = rc.right - rc.left;
        int h = rc.bottom - rc.top;

        // Subtract 1 physical pixel from height so Windows never misidentifies this
        // transparent click-through overlay as an exclusive fullscreen game or media app
        int physicalH = std::max(100, h - 1);

        HWND hWnd = CreateWindowExW(
            WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
            ZEVIC_WINDOW_CLASS,
            L"ZEVIC Ambient Glow Overlay",
            WS_POPUP | WS_VISIBLE,
            rc.left, rc.top, w, physicalH,
            NULL, NULL, GetModuleHandle(NULL), NULL
        );

        if (!hWnd) continue;

        // Create 32-bit DIB section
        BITMAPINFO bmi;
        ZeroMemory(&bmi, sizeof(bmi));
        bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth = w;
        bmi.bmiHeader.biHeight = -physicalH; // Top-down
        bmi.bmiHeader.biPlanes = 1;
        bmi.bmiHeader.biBitCount = 32;
        bmi.bmiHeader.biCompression = BI_RGB;

        void* pBits = nullptr;
        HBITMAP hBmp = CreateDIBSection(hdcScreen, &bmi, DIB_RGB_COLORS, &pBits, NULL, 0);
        HDC hdcMem = CreateCompatibleDC(hdcScreen);
        HGDIOBJ hOld = SelectObject(hdcMem, hBmp);

        MonitorOverlay mon;
        mon.hWnd = hWnd;
        mon.rcMonitor = rc;
        mon.width = w;
        mon.height = physicalH;
        mon.hdcMem = hdcMem;
        mon.hBitmap = hBmp;
        mon.pBits = pBits;
        mon.hOldBitmap = hOld;

        SetWindowPos(hWnd, HWND_TOPMOST, rc.left, rc.top, w, physicalH,
                     SWP_NOACTIVATE | SWP_SHOWWINDOW | SWP_NOOWNERZORDER);

        g_monitors.push_back(mon);
    }

    ReleaseDC(NULL, hdcScreen);
}

static void DestroyMonitorOverlays() {
    for (auto& mon : g_monitors) {
        if (mon.hWnd && IsWindow(mon.hWnd)) {
            DestroyWindow(mon.hWnd);
        }
        if (mon.hdcMem) {
            SelectObject(mon.hdcMem, mon.hOldBitmap);
            DeleteDC(mon.hdcMem);
        }
        if (mon.hBitmap) {
            DeleteObject(mon.hBitmap);
        }
    }
    g_monitors.clear();
}

static LRESULT CALLBACK OverlayWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_WINDOWPOSCHANGING: {
            // Keep topmost z-order locked above Windows taskbar (Shell_TrayWnd)
            auto* pos = (WINDOWPOS*)lParam;
            pos->hwndInsertAfter = HWND_TOPMOST;
            return 0;
        }
        case WM_DISPLAYCHANGE: {
            // Display resolution, scaling, or monitor count changed
            DestroyMonitorOverlays();
            CreateMonitorOverlays();
            return 0;
        }
        case WM_HOTKEY: {
            if (wParam == ZEVIC_HOTKEY_ID) {
                g_settings.enabled = !g_settings.enabled;
            }
            return 0;
        }
        case WM_DESTROY:
            return 0;
    }
    return DefWindowProcW(hWnd, msg, wParam, lParam);
}

// ═══════════════════════════════════════════════════════════════════════════
// DEDICATED OVERLAY THREAD (60 FPS VSync Animation Loop)
// ═══════════════════════════════════════════════════════════════════════════
static DWORD WINAPI OverlayThreadProc(LPVOID lpParam) {
    WNDCLASSEXW wc = { sizeof(WNDCLASSEXW) };
    wc.lpfnWndProc = OverlayWndProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = ZEVIC_WINDOW_CLASS;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    RegisterClassExW(&wc);

    CreateMonitorOverlays();

    // Register global hotkey: Ctrl + Shift + Z
    RegisterHotKey(NULL, ZEVIC_HOTKEY_ID, MOD_CONTROL | MOD_SHIFT, 'Z');

    auto prevTime = std::chrono::high_resolution_clock::now();
    double progress = 0.0;
    int topmostCounter = 0;

    while (g_threadRunning.load()) {
        MSG msg;
        while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                g_threadRunning.store(false);
                break;
            }
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }

        if (!g_threadRunning.load()) break;

        auto now = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> dt = now - prevTime;
        prevTime = now;

        double elapsed = dt.count();
        if (elapsed <= 0.0 || elapsed > 0.5) elapsed = 0.016;

        double rotationTime = std::max(1.0, std::min(60.0, g_settings.rotationTimeSec));
        double delta = elapsed / rotationTime;
        if (!g_settings.clockwise) delta = -delta;

        progress = std::fmod(progress + delta, 1.0);
        if (progress < 0) progress += 1.0;

        auto nowSeconds = std::chrono::duration_cast<std::chrono::duration<double>>(now.time_since_epoch()).count();

        for (auto& mon : g_monitors) {
            RenderOverlayFrame(mon, progress, nowSeconds);
        }

        // Periodically guarantee overlay is above taskbar (Shell_TrayWnd)
        topmostCounter++;
        if (topmostCounter >= 60) {
            topmostCounter = 0;
            for (auto& mon : g_monitors) {
                SetWindowPos(mon.hWnd, HWND_TOPMOST, 0, 0, 0, 0,
                             SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOOWNERZORDER);
            }
        }

        // Adaptive frame rate: 30 FPS on laptop battery, 60 FPS on AC / desktop
        bool onBattery = g_settings.batterySaverAware && IsRunningOnBattery();
        DWORD sleepMs = onBattery ? 33 : 16;
        Sleep(sleepMs);
    }

    UnregisterHotKey(NULL, ZEVIC_HOTKEY_ID);
    DestroyMonitorOverlays();
    UnregisterClassW(ZEVIC_WINDOW_CLASS, GetModuleHandle(NULL));

    return 0;
}

// ═══════════════════════════════════════════════════════════════════════════
// WINDHAWK SETTINGS LOADER
// ═══════════════════════════════════════════════════════════════════════════
static void LoadSettings() {
    g_settings.enabled = Wh_GetIntSetting(L"enabled") != 0;

    // 1. Color Preset Dropdown / Toggle
    PCWSTR presetStr = Wh_GetStringSetting(L"colorPreset");
    if (presetStr) {
        if (wcscmp(presetStr, L"rgb") == 0) {
            g_settings.rgbRainbow = true;
            g_settings.red = 0; g_settings.green = 210; g_settings.blue = 255;
        } else if (wcscmp(presetStr, L"pure_white") == 0) {
            g_settings.rgbRainbow = false;
            g_settings.red = 255; g_settings.green = 255; g_settings.blue = 255;
        } else if (wcscmp(presetStr, L"sony_bravia_cyan") == 0) {
            g_settings.rgbRainbow = false;
            g_settings.red = 0; g_settings.green = 210; g_settings.blue = 255;
        } else if (wcscmp(presetStr, L"cyberpunk") == 0) {
            g_settings.rgbRainbow = false;
            g_settings.red = 224; g_settings.green = 64; g_settings.blue = 251;
        } else if (wcscmp(presetStr, L"amoled_gold") == 0) {
            g_settings.rgbRainbow = false;
            g_settings.red = 255; g_settings.green = 184; g_settings.blue = 0;
        } else if (wcscmp(presetStr, L"plasma_violet") == 0) {
            g_settings.rgbRainbow = false;
            g_settings.red = 138; g_settings.green = 43; g_settings.blue = 226;
        } else if (wcscmp(presetStr, L"emerald_neon") == 0) {
            g_settings.rgbRainbow = false;
            g_settings.red = 0; g_settings.green = 255; g_settings.blue = 163;
        } else if (wcscmp(presetStr, L"ice_blue") == 0) {
            g_settings.rgbRainbow = false;
            g_settings.red = 160; g_settings.green = 231; g_settings.blue = 255;
        } else if (wcscmp(presetStr, L"warm_white") == 0) {
            g_settings.rgbRainbow = false;
            g_settings.red = 255; g_settings.green = 236; g_settings.blue = 210;
        } else if (wcscmp(presetStr, L"rose_gold") == 0) {
            g_settings.rgbRainbow = false;
            g_settings.red = 255; g_settings.green = 182; g_settings.blue = 193;
        } else if (wcscmp(presetStr, L"amber") == 0) {
            g_settings.rgbRainbow = false;
            g_settings.red = 255; g_settings.green = 160; g_settings.blue = 64;
        } else {
            // "custom"
            g_settings.rgbRainbow = false;
            g_settings.red = (BYTE)Wh_GetIntSetting(L"color.red");
            g_settings.green = (BYTE)Wh_GetIntSetting(L"color.green");
            g_settings.blue = (BYTE)Wh_GetIntSetting(L"color.blue");
        }
        Wh_FreeStringSetting(presetStr);
    } else {
        // Default glow color: Sony Bravia Cyan (#00D2FF)
        g_settings.rgbRainbow = false;
        int r = Wh_GetIntSetting(L"color.red");
        int g = Wh_GetIntSetting(L"color.green");
        int b = Wh_GetIntSetting(L"color.blue");
        if (r == 0 && g == 0 && b == 0) {
            g_settings.red = 0;
            g_settings.green = 210;
            g_settings.blue = 255;
        } else {
            g_settings.red = (BYTE)r;
            g_settings.green = (BYTE)g;
            g_settings.blue = (BYTE)b;
        }
    }

    // 2. Continuous 360° Hue Glide
    g_settings.colorCycle = Wh_GetIntSetting(L"colorCycle") != 0;
    int cycleSec = Wh_GetIntSetting(L"colorCycleSpeedSec");
    g_settings.colorCycleSpeedSec = (cycleSec <= 0) ? 10.0 : (double)cycleSec;

    // 3. Illumination Beams (Single, Dual, Triple, Quad)
    PCWSTR beamsStr = Wh_GetStringSetting(L"illuminationBeams");
    if (!beamsStr) beamsStr = Wh_GetStringSetting(L"sparkCount");
    if (beamsStr) {
        if (wcscmp(beamsStr, L"single") == 0 || wcscmp(beamsStr, L"1") == 0) {
            g_settings.sparkCount = 1;
        } else if (wcscmp(beamsStr, L"dual") == 0 || wcscmp(beamsStr, L"2") == 0) {
            g_settings.sparkCount = 2;
        } else if (wcscmp(beamsStr, L"triple") == 0 || wcscmp(beamsStr, L"3") == 0) {
            g_settings.sparkCount = 3;
        } else if (wcscmp(beamsStr, L"quad") == 0 || wcscmp(beamsStr, L"4") == 0) {
            g_settings.sparkCount = 4;
        } else {
            int c = _wtoi(beamsStr);
            g_settings.sparkCount = (c < 1) ? 1 : (c > 4 ? 4 : c);
        }
        Wh_FreeStringSetting(beamsStr);
    } else {
        int count = Wh_GetIntSetting(L"illuminationBeams");
        if (count <= 0) count = Wh_GetIntSetting(L"sparkCount");
        g_settings.sparkCount = (count < 1) ? 1 : (count > 4 ? 4 : count);
    }

    // 4. Direction (Clockwise / Anti-Clockwise)
    PCWSTR dirStr = Wh_GetStringSetting(L"direction");
    if (dirStr) {
        g_settings.clockwise = (wcscmp(dirStr, L"anticlockwise") != 0 &&
                                wcscmp(dirStr, L"anti-clockwise") != 0 &&
                                wcscmp(dirStr, L"ccw") != 0);
        Wh_FreeStringSetting(dirStr);
    } else {
        int cw = Wh_GetIntSetting(L"clockwise");
        g_settings.clockwise = (cw != 0);
    }

    // 5. Speed & Size
    int speedSec = Wh_GetIntSetting(L"speedSec");
    g_settings.rotationTimeSec = (speedSec <= 0) ? 8.0 : (double)speedSec;

    int length = Wh_GetIntSetting(L"sparkLength");
    g_settings.sparkLengthPx = (length <= 0) ? 80.0 : (double)length;

    int thickness = Wh_GetIntSetting(L"sparkThickness");
    g_settings.sparkThickness = (thickness <= 0) ? 3.0 : (double)thickness;

    int tailMult = Wh_GetIntSetting(L"tailMultiplier");
    g_settings.tailLengthMultiplier = (tailMult <= 0) ? 2.5 : ((double)tailMult / 10.0);

    int fade = Wh_GetIntSetting(L"tailFadeEnd");
    g_settings.tailFadeEnd = (fade < 0) ? 0.0 : ((double)fade / 100.0);

    int brightness = Wh_GetIntSetting(L"sparkBrightness");
    if (brightness <= 0) brightness = 100;
    g_settings.sparkBrightness = (double)brightness / 100.0;

    int glow = Wh_GetIntSetting(L"sparkGlowBlur");
    g_settings.sparkGlowBlur = (glow < 0) ? 15.0 : (double)glow;

    // 6. Corner Style & Curvature
    PCWSTR cornerStr = Wh_GetStringSetting(L"cornerStyle");
    if (cornerStr) {
        if (wcscmp(cornerStr, L"sharp") == 0) {
            g_settings.cornerRadiusPx = 0.0;
        } else {
            int cr = Wh_GetIntSetting(L"cornerRadius");
            g_settings.cornerRadiusPx = (cr <= 0) ? 12.0 : (double)cr;
        }
        Wh_FreeStringSetting(cornerStr);
    } else {
        g_settings.cornerRadiusPx = 0.0;
    }

    // 7. Master Edge Inset & 4-Edge Calibration Offsets
    PCWSTR insetStr = Wh_GetStringSetting(L"edgeInset");
    if (insetStr) {
        g_settings.edgeInset = _wtof(insetStr);
        Wh_FreeStringSetting(insetStr);
    } else {
        int insetVal = Wh_GetIntSetting(L"edgeInset");
        g_settings.edgeInset = (insetVal == 0) ? -3.0 : (double)insetVal;
    }
    g_settings.edgeOffsetTop = (double)Wh_GetIntSetting(L"edgeOffsetTop");
    g_settings.edgeOffsetBottom = (double)Wh_GetIntSetting(L"edgeOffsetBottom");
    g_settings.edgeOffsetLeft = (double)Wh_GetIntSetting(L"edgeOffsetLeft");
    g_settings.edgeOffsetRight = (double)Wh_GetIntSetting(L"edgeOffsetRight");

    // 8. Base Line
    g_settings.showBaseLine = Wh_GetIntSetting(L"showBaseLine") != 0;
    int baseOpacity = Wh_GetIntSetting(L"baseLineOpacity");
    g_settings.baseLineOpacity = (baseOpacity <= 0) ? 0.08 : ((double)baseOpacity / 100.0);
    int baseThick = Wh_GetIntSetting(L"baseLineThickness");
    g_settings.baseLineThickness = (baseThick <= 0) ? 2.0 : (double)baseThick;

    // 9. Fullscreen Auto-Pause & Battery Saver
    g_settings.hideInFullscreen = Wh_GetIntSetting(L"hideInFullscreen") != 0;
    g_settings.batterySaverAware = Wh_GetIntSetting(L"batterySaverAware") != 0;
}

// ═══════════════════════════════════════════════════════════════════════════
// WINDHAWK MOD ENTRY POINTS
// ═══════════════════════════════════════════════════════════════════════════

// The mod is being initialized, load settings, start animation thread.
BOOL Wh_ModInit() {
    Wh_Log(L"ZEVIC Ambient Edge Glow by %s initializing...", ZEVIC_AUTHOR_NAME);

    // Initialize GDI+
    GdiplusStartupInput gdiplusStartupInput;
    GdiplusStartup(&g_gdiplusToken, &gdiplusStartupInput, NULL);

    LoadSettings();

    // Launch hardware-synchronized animation thread in explorer.exe
    g_threadRunning.store(true);
    g_hOverlayThread = CreateThread(NULL, 0, OverlayThreadProc, NULL, 0, &g_overlayThreadId);
    if (!g_hOverlayThread) {
        Wh_Log(L"Failed to create ZEVIC overlay thread.");
        return FALSE;
    }

    Wh_Log(L"ZEVIC initialization complete. GitHub: %s | Profile: %s", ZEVIC_GITHUB_URL, ZEVIC_LINKEDIN_URL);
    return TRUE;
}

// The mod is being unloaded, free all allocated resources.
void Wh_ModUninit() {
    Wh_Log(L"ZEVIC uninitializing...");

    if (g_threadRunning.load()) {
        g_threadRunning.store(false);
        if (g_overlayThreadId) {
            PostThreadMessageW(g_overlayThreadId, WM_QUIT, 0, 0);
        }
        if (g_hOverlayThread) {
            WaitForSingleObject(g_hOverlayThread, 2000);
            CloseHandle(g_hOverlayThread);
            g_hOverlayThread = nullptr;
        }
    }

    if (g_gdiplusToken) {
        GdiplusShutdown(g_gdiplusToken);
        g_gdiplusToken = 0;
    }

    Wh_Log(L"ZEVIC uninitialized successfully.");
}

// The mod settings were changed, reload them immediately.
void Wh_ModSettingsChanged() {
    Wh_Log(L"ZEVIC SettingsChanged — applying live updates...");
    LoadSettings();
}
