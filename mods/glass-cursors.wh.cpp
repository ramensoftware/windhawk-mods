// ==WindhawkMod==
// @id              glass-cursors
// @name            Glass Cursors
// @description     Original DPI-aware translucent glass system cursors with a live animated loading indicator.
// @version         0.26.7
// @author          fizixes
// @github          https://github.com/fizixes
// @license         MIT
// @include         windhawk.exe
// @compilerOptions -luser32 -lgdi32 -lshell32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Glass Cursors

A translucent glass cursor set rendered by the mod. The pointer and other cursor roles are
original artwork. The hand silhouette is adapted from Bootstrap Icons' MIT-licensed hand-index
icon and restyled into the same translucent glass treatment.

The cursor geometry is drawn directly at the selected output resolution instead
of scaling a 32 px bitmap. Artwork size is normalized from each role's actual rendered bounds so pointer, text, resize, and other roles scale consistently.

## Features
- Original white-outline / translucent-glass artwork.
- Four glass material styles: Clear, Acrylic, Mica, and Mica Alt.
- 25% and 50% glass fill variants with configurable interior RGB color tint.
- Automatic Windows cursor-metric sizing, plus common manual canvas overrides from 32 to 96 px.
- Replaces the standard Windows cursor roles.
- Busy and Working-in-background cursors use a live animated white glass spinner.
- Runs in a dedicated Windhawk tool process instead of inside Explorer.
- Disabling the mod restores the user's configured Windows cursor scheme.
- Respects hidden/suppressed cursor state and defers cursor-slot self-healing while the cursor is hidden/suppressed or a foreground application is fullscreen.
- Acrylic and the default Mica/Mica Alt mode sample live content under the cursor at the refresh rate of the monitor under the cursor, while stationary unchanged captures back off adaptively.

## Display scaling
Windows system cursor slots hold one fixed-size cursor at a time. Automatic follows the current Windows cursor metric; manual cursor resolution changes the bitmap canvas. Artwork size is normalized from the visible bounds of each cursor role so the same percentage produces comparable apparent sizing across pointer, text, resize, and other cursors.

Acrylic is a CPU-raster approximation inspired by Windows Acrylic's documented layering model: sampled backdrop, blur, exclusion/luminosity treatment, color tint, and fine noise. Mica and Mica Alt intentionally apply their stronger opaque tinting to the live sampled backdrop. Dynamic capture is adaptive, cursor replacement is change-gated, and cached artwork/reusable GDI surfaces are used to minimize update latency.

## Compatibility
When Jaali's `mac-magnifying-cursor` mod is present, Glass Cursors pauses its own cursor updates while the magnifying overlay is visible and reapplies the glass cursor set after the magnifier releases the system cursor slots.

Hand silhouette derived from Bootstrap Icons, Copyright (c) 2019-2024 The Bootstrap Authors, MIT License.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- FillOpacity: "25"
  $name: Glass fill
  $description: Clear uses this as true interior opacity. Sampled Acrylic/Mica use it as material density/tint strength because their backdrop is precomposited into the cursor bitmap.
  $options:
  - "25": 25% fill
  - "50": 50% fill
- GlassColor:
  - Red: 33
    $name: Red (0-255)
  - Green: 38
    $name: Green (0-255)
  - Blue: 46
    $name: Blue (0-255)
  $name: Internal glass color
  $description: RGB tint of the glass interior. Glass fill controls opacity for Clear and material density for sampled styles.
- GlassStyle: acrylic
  $name: Glass style
  $description: Clear is lightweight translucent glass. Acrylic, Mica, and Mica Alt sample and process the live content beneath the cursor.
  $options:
  - clear: Clear
  - acrylic: Acrylic
  - mica: Mica
  - micaAlt: Mica Alt
- BlurStrength: 8
  $name: Blur strength
  $description: Base backdrop blur radius. 0 disables blur. Values from 0 to 24 are recommended.
- BlurScale: 100
  $name: Blur scale (%)
  $description: "Multiplies the material-specific blur radius after cursor-resolution scaling. 100% keeps the default profile. Range 25-300."
- CursorSize: "0"
  $name: Cursor resolution
  $description: Automatic uses the current Windows system cursor metric. Manual values set the cursor bitmap canvas size; Artwork size controls the visible geometry within that canvas.
  $options:
  - "0": Automatic (Windows cursor metric)
  - "32": 32 px
  - "40": 40 px
  - "48": 48 px
  - "56": 56 px
  - "64": 64 px
  - "72": 72 px
  - "80": 80 px
  - "96": 96 px
- PointerStyle: sharpRounded
  $name: Pointer style
  $description: Choose the pointer silhouette and notch geometry.
  $options:
  - sharpRounded: Sharp outer edge + curved notch
  - cleanRounded: Rounded outer edge + curved notch
  - symmetricSharp: Symmetric pointed notch
  - symmetricCurved: Symmetric continuous curved notch
- HandStyle: separated
  $name: Hand style
  $description: Select the hand design while keeping the same outer silhouette.
  $options:
  - separated: Glass hand - finger separators (default)
  - clean: Glass hand - clean silhouette
- ArtworkScale: "68"
  $name: Artwork size
  $description: Sets the visible artwork size after normalizing each cursor role to its actual rendered bounds, so the same percentage has comparable visual size across roles.
  $options:
  - "65": Extra compact (65%)
  - "68": Compact (68%)
  - "75": Small (75%)
  - "82": Medium (82%)
  - "90": Large (90%)
  - "100": Full size (100%)
*/
// ==/WindhawkModSettings==

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <shellapi.h>
#include <windhawk_api.h>
#include <windhawk_utils.h>
#include <stdint.h>
#include <algorithm>
#include <array>
#include <atomic>
#include <cmath>
#include <climits>
#include <cstring>
#include <cwchar>
#include <vector>

namespace {

constexpr DWORD CURSOR_NORMAL      = 32512u;
constexpr DWORD CURSOR_IBEAM       = 32513u;
constexpr DWORD CURSOR_WAIT        = 32514u;
constexpr DWORD CURSOR_CROSS       = 32515u;
constexpr DWORD CURSOR_UP          = 32516u;
constexpr DWORD CURSOR_SIZENWSE    = 32642u;
constexpr DWORD CURSOR_SIZENESW    = 32643u;
constexpr DWORD CURSOR_SIZEWE      = 32644u;
constexpr DWORD CURSOR_SIZENS      = 32645u;
constexpr DWORD CURSOR_SIZEALL     = 32646u;
constexpr DWORD CURSOR_NO          = 32648u;
constexpr DWORD CURSOR_HAND        = 32649u;
constexpr DWORD CURSOR_APPSTARTING = 32650u;
constexpr DWORD CURSOR_HELP        = 32651u;
constexpr DWORD CURSOR_PIN         = 32671u;
constexpr DWORD CURSOR_PERSON      = 32672u;

constexpr int kSupersample = 4;
constexpr int kSpinnerFrames = 24;
constexpr double kPi = 3.14159265358979323846;
constexpr UINT kReapplyCursorsMessage = WM_APP + 0x471;
constexpr wchar_t kCursorWindowClassName[] = L"WindhawkGlassCursorsWindow";

struct Point {
    double x;
    double y;
};

struct FColor {
    double r;
    double g;
    double b;
    double a;
};

struct Pixel {
    float r = 0.0f;
    float g = 0.0f;
    float b = 0.0f;
    float a = 0.0f;
};

class Surface {
public:
    Surface(int width, int height)
        : m_width(width),
          m_height(height),
          m_pixels(static_cast<size_t>(width) * height),
          m_glassMask(static_cast<size_t>(width) * height, 0),
          m_protectedMask(static_cast<size_t>(width) * height, 0) {}

    int Width() const { return m_width; }
    int Height() const { return m_height; }

    void Blend(int x, int y, FColor c) {
        if (x < 0 || y < 0 || x >= m_width || y >= m_height || c.a <= 0.0) {
            return;
        }

        c.a = std::clamp(c.a, 0.0, 1.0);
        Pixel& dst = m_pixels[static_cast<size_t>(y) * m_width + x];

        const float srcA = static_cast<float>(c.a);
        const float outA = srcA + dst.a * (1.0f - srcA);
        if (outA <= 0.0f) {
            dst = {};
            return;
        }

        dst.r = (static_cast<float>(c.r) * srcA +
                 dst.r * dst.a * (1.0f - srcA)) / outA;
        dst.g = (static_cast<float>(c.g) * srcA +
                 dst.g * dst.a * (1.0f - srcA)) / outA;
        dst.b = (static_cast<float>(c.b) * srcA +
                 dst.b * dst.a * (1.0f - srcA)) / outA;
        dst.a = outA;
    }

    void MarkGlass(int x, int y, double coverage = 1.0) {
        MarkMask(m_glassMask, x, y, coverage);
    }

    void MarkProtected(int x, int y, double coverage = 1.0) {
        MarkMask(m_protectedMask, x, y, coverage);
    }

    const Pixel& At(int x, int y) const {
        return m_pixels[static_cast<size_t>(y) * m_width + x];
    }

    uint8_t GlassMaskAt(int x, int y) const {
        return m_glassMask[static_cast<size_t>(y) * m_width + x];
    }

    uint8_t ProtectedMaskAt(int x, int y) const {
        return m_protectedMask[static_cast<size_t>(y) * m_width + x];
    }

private:
    void MarkMask(std::vector<uint8_t>& mask, int x, int y, double coverage) {
        if (x < 0 || y < 0 || x >= m_width || y >= m_height) {
            return;
        }
        uint8_t value = 0;
        if (coverage >= 1.0) {
            value = 255;
        } else if (coverage > 0.0) {
            value = static_cast<uint8_t>(std::clamp(
                std::lround(coverage * 255.0), 0l, 255l));
        }
        uint8_t& dst = mask[static_cast<size_t>(y) * m_width + x];
        dst = std::max(dst, value);
    }

    int m_width;
    int m_height;
    std::vector<Pixel> m_pixels;
    std::vector<uint8_t> m_glassMask;
    std::vector<uint8_t> m_protectedMask;
};

struct RenderedCursor {
    int size = 32;
    int hotspotX = 0;
    int hotspotY = 0;
    double artworkScale = 1.0;
    std::vector<uint32_t> pixels;
    std::vector<uint8_t> glassMask;
    std::vector<uint8_t> protectedMask;
};

int g_fillOpacity = 25;
int g_glassRed = 33;
int g_glassGreen = 38;
int g_glassBlue = 46;
int g_cursorSize = 32;
int g_pointerStyle = 0;
int g_handStyle = 1;
int g_artworkScale = 68;
int g_glassStyle = 1;
int g_blurStrength = 8;
int g_blurScale = 100;
constexpr DWORD kAnimationDelay = 33;
HANDLE g_animationStopEvent = nullptr;
HANDLE g_animationReadyEvent = nullptr;
HANDLE g_animationThread = nullptr;
std::atomic<HWND> g_cursorWindow{nullptr};
std::atomic<bool> g_animationThreadInitialized{false};
std::atomic<bool> g_animationFramesReady{false};
std::atomic<bool> g_reapplyMessagePending{false};
std::atomic<bool> g_forceSystemReapplyPending{false};
std::atomic<bool> g_cursorReapplyDeferred{false};
std::atomic<bool> g_forceCursorReapplyDeferred{false};
int g_animationFrame = 0;
std::vector<HCURSOR> g_busyFrames;
std::vector<HCURSOR> g_workingFrames;
double g_renderOffsetX = 0.0;
double g_renderOffsetY = 0.0;
HCURSOR g_expectedNormalCursor = nullptr;
LPTOP_LEVEL_EXCEPTION_FILTER g_previousExceptionFilter = nullptr;

constexpr DWORD kIdlePollInterval = 350;
constexpr DWORD kHiddenCursorPollInterval = 50;
constexpr DWORD kMagnifierPollInterval = 50;
constexpr UINT kMaxDynamicRefreshRate = 500;

struct DynamicCaptureSurface {
    HDC screenDc = nullptr;
    HDC memoryDc = nullptr;
    HBITMAP bitmap = nullptr;
    HGDIOBJ oldBitmap = nullptr;
    uint32_t* bits = nullptr;
    int width = 0;
    int height = 0;
};

struct DynamicGlassState {
    DynamicCaptureSurface capture;
    std::vector<uint32_t> lastAppliedPixels;
    std::vector<uint32_t> blurScratch;
    std::vector<uint32_t> blurredPixels;
    std::vector<uint32_t> workingPixels;
    DWORD lastRole = 0;
    POINT lastAppliedPosition = {};
    bool hasLastAppliedPosition = false;
    DWORD lastBackdropCaptureTick = 0;
    RECT virtualDesktop = {};
    bool virtualDesktopValid = false;
    DWORD lastEnvironmentQueryTick = 0;
    bool highContrast = false;
    bool batterySaver = false;
    unsigned unchangedCaptureCount = 0;
    int width = 0;
    int height = 0;
    int padding = 0;
    int blurRadius = 0;
};

DynamicGlassState g_dynamicGlass;

struct DynamicCursorFactory {
    HBITMAP colorBitmap = nullptr;
    HBITMAP maskBitmap = nullptr;
    void* colorBits = nullptr;
    int size = 0;
};

DynamicCursorFactory g_dynamicCursorFactory;

constexpr std::array<DWORD, 16> kCursorRoles = {
    CURSOR_NORMAL, CURSOR_IBEAM, CURSOR_WAIT, CURSOR_CROSS,
    CURSOR_UP, CURSOR_SIZENWSE, CURSOR_SIZENESW, CURSOR_SIZEWE,
    CURSOR_SIZENS, CURSOR_SIZEALL, CURSOR_NO, CURSOR_HAND,
    CURSOR_APPSTARTING, CURSOR_HELP, CURSOR_PIN, CURSOR_PERSON,
};

std::array<RenderedCursor, kCursorRoles.size()> g_renderedCursorCache;
std::array<bool, kCursorRoles.size()> g_renderedCursorCacheValid{};
std::array<HCURSOR, kCursorRoles.size()> g_expectedRoleHandles{};
std::array<double, kCursorRoles.size()> g_roleScaleNormalization{};
std::array<bool, kCursorRoles.size()> g_roleScaleNormalizationValid{};

int CursorRoleIndex(DWORD role) {
    for (size_t i = 0; i < kCursorRoles.size(); ++i) {
        if (kCursorRoles[i] == role) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

void ClearRenderedCursorCache() {
    g_renderedCursorCacheValid.fill(false);
}

void ClearRoleScaleNormalization() {
    g_roleScaleNormalizationValid.fill(false);
}

void InvalidateRoleScaleNormalization(DWORD role) {
    const int index = CursorRoleIndex(role);
    if (index >= 0) {
        g_roleScaleNormalizationValid[static_cast<size_t>(index)] = false;
    }
}

void InvalidateRenderedCursor(DWORD role) {
    const int index = CursorRoleIndex(role);
    if (index >= 0) {
        g_renderedCursorCacheValid[static_cast<size_t>(index)] = false;
    }
}

void CacheRenderedCursor(DWORD role, const RenderedCursor& cursor) {
    const int index = CursorRoleIndex(role);
    if (index < 0) {
        return;
    }
    g_renderedCursorCache[static_cast<size_t>(index)] = cursor;
    g_renderedCursorCacheValid[static_cast<size_t>(index)] = true;
}

const RenderedCursor* GetCachedRenderedCursor(DWORD role) {
    const int index = CursorRoleIndex(role);
    if (index < 0 || !g_renderedCursorCacheValid[static_cast<size_t>(index)]) {
        return nullptr;
    }
    return &g_renderedCursorCache[static_cast<size_t>(index)];
}

bool g_magnifierWasActive = false;
HMONITOR g_dynamicRefreshMonitor = nullptr;
DWORD g_dynamicCaptureInterval = 16;
DWORD g_dynamicRefreshLastQueryTick = 0;

struct Bounds {
    double minX;
    double minY;
    double maxX;
    double maxY;
};

struct GlassMaterialProfile {
    double bodyOpacityScale;
    double rimWidth;
    double rimStrength;
    double highlightStrength;
    double darkEdgeStrength;
    double gradientStrength;
    double noiseStrength;
    double streakStrength;
    double frostedAmount;
    double tintStrength;
};

double Clamp01(double value) {
    return std::clamp(value, 0.0, 1.0);
}

double SmoothStep(double edge0, double edge1, double x) {
    if (edge0 == edge1) {
        return x < edge0 ? 0.0 : 1.0;
    }
    const double t = Clamp01((x - edge0) / (edge1 - edge0));
    return t * t * (3.0 - 2.0 * t);
}

double HashNoise01(int x, int y) {
    uint32_t h = 2166136261u;
    h = (h ^ static_cast<uint32_t>(x) * 16777619u) * 2166136261u;
    h = (h ^ static_cast<uint32_t>(y) * 374761393u) * 668265263u;
    h ^= h >> 13;
    h *= 1274126177u;
    h ^= h >> 16;
    return (h & 0x00FFFFFFu) / static_cast<double>(0x01000000u);
}

Bounds GetBounds(const std::vector<Point>& points) {
    Bounds bounds{points[0].x, points[0].y, points[0].x, points[0].y};
    for (const Point& p : points) {
        bounds.minX = std::min(bounds.minX, p.x);
        bounds.minY = std::min(bounds.minY, p.y);
        bounds.maxX = std::max(bounds.maxX, p.x);
        bounds.maxY = std::max(bounds.maxY, p.y);
    }
    return bounds;
}

GlassMaterialProfile GetClearGlassMaterialProfile() {
    // Clear is the intentionally stylized lightweight glass option. Sampled
    // styles bypass this profile and are filled flat until their dynamic pass.
    return {0.78, 7.2, 0.36, 0.20, 0.10, 0.10, 0.02, 0.18, 0.00, 0.95};
}

FColor EvaluateGlassMaterial(double x,
                             double y,
                             const Bounds& bounds,
                             double edgeInset,
                             double maxInset) {
    // Sampled styles replace the interior later. Avoid running Clear's
    // gradients/noise/expensive edge-light calculations during base rasterization.
    if (g_glassStyle != 0) {
        return {g_glassRed / 255.0,
                g_glassGreen / 255.0,
                g_glassBlue / 255.0,
                g_fillOpacity / 100.0};
    }

    const GlassMaterialProfile profile = GetClearGlassMaterialProfile();
    const double width = std::max(1.0, bounds.maxX - bounds.minX);
    const double height = std::max(1.0, bounds.maxY - bounds.minY);
    const double nx = Clamp01((x - bounds.minX) / width);
    const double ny = Clamp01((y - bounds.minY) / height);
    const double rim = 1.0 - SmoothStep(0.0, profile.rimWidth, edgeInset);
    const double lightAxis = Clamp01(1.0 - (0.58 * nx + 0.78 * ny));
    const double darkAxis = Clamp01(0.60 * nx + 0.85 * ny);
    const double diagonalGradient = 0.5 - (0.55 * nx + 0.45 * ny - 0.5);
    const double noise = HashNoise01(static_cast<int>(std::floor(x)), static_cast<int>(std::floor(y))) - 0.5;
    const double streakAxis = ny - (0.58 * nx + 0.06);
    const double streak = std::exp(-(streakAxis * streakAxis) / 0.010);

    const double baseAlpha = g_fillOpacity / 100.0;
    double alpha = baseAlpha * profile.bodyOpacityScale;
    alpha *= 1.0 + profile.gradientStrength * diagonalGradient + profile.noiseStrength * noise * 0.60;
    alpha = Clamp01(alpha);

    double r = (g_glassRed / 255.0) * profile.tintStrength;
    double g = (g_glassGreen / 255.0) * profile.tintStrength;
    double b = (g_glassBlue / 255.0) * profile.tintStrength;

    const double frost = profile.frostedAmount * (0.70 + 0.60 * std::max(0.0, noise));
    r = r * (1.0 - frost) + frost;
    g = g * (1.0 - frost) + frost;
    b = b * (1.0 - frost) + frost;

    const double brighten = profile.rimStrength * rim +
                            profile.highlightStrength * lightAxis +
                            profile.streakStrength * streak;
    r = r + (1.0 - r) * Clamp01(brighten);
    g = g + (1.0 - g) * Clamp01(brighten);
    b = b + (1.0 - b) * Clamp01(brighten);

    const double darken = profile.darkEdgeStrength * darkAxis;
    r *= (1.0 - darken);
    g *= (1.0 - darken);
    b *= (1.0 - darken);

    const double centerSoftness = Clamp01(edgeInset / std::max(1.0, maxInset));
    alpha *= 0.96 + 0.04 * centerSoftness;

    return {Clamp01(r), Clamp01(g), Clamp01(b), alpha};
}

FColor White(double alpha = 1.0) {
    return {1.0, 1.0, 1.0, alpha};
}

FColor Shadow(double alpha = 0.28) {
    return {0.0, 0.0, 0.0, alpha};
}

double DistanceToSegment(double px, double py, Point a, Point b) {
    const double dx = b.x - a.x;
    const double dy = b.y - a.y;
    const double length2 = dx * dx + dy * dy;
    if (length2 <= 1e-12) {
        return std::hypot(px - a.x, py - a.y);
    }

    const double t = std::clamp(((px - a.x) * dx + (py - a.y) * dy) / length2,
                                0.0, 1.0);
    const double qx = a.x + t * dx;
    const double qy = a.y + t * dy;
    return std::hypot(px - qx, py - qy);
}

bool PointInPolygon(double x, double y, const std::vector<Point>& points) {
    bool inside = false;
    const size_t count = points.size();
    for (size_t i = 0, j = count - 1; i < count; j = i++) {
        const Point& pi = points[i];
        const Point& pj = points[j];
        const bool intersects = ((pi.y > y) != (pj.y > y)) &&
            (x < (pj.x - pi.x) * (y - pi.y) / (pj.y - pi.y + 1e-20) + pi.x);
        if (intersects) {
            inside = !inside;
        }
    }
    return inside;
}

void FillPolygon(Surface& s, const std::vector<Point>& points, FColor color) {
    if (points.size() < 3) {
        return;
    }

    double minX = points[0].x, maxX = points[0].x;
    double minY = points[0].y, maxY = points[0].y;
    for (const Point& p : points) {
        minX = std::min(minX, p.x);
        maxX = std::max(maxX, p.x);
        minY = std::min(minY, p.y);
        maxY = std::max(maxY, p.y);
    }

    const int x0 = std::max(0, static_cast<int>(std::floor(minX)));
    const int x1 = std::min(s.Width() - 1, static_cast<int>(std::ceil(maxX)));
    const int y0 = std::max(0, static_cast<int>(std::floor(minY)));
    const int y1 = std::min(s.Height() - 1, static_cast<int>(std::ceil(maxY)));

    for (int y = y0; y <= y1; ++y) {
        for (int x = x0; x <= x1; ++x) {
            if (PointInPolygon(x + 0.5, y + 0.5, points)) {
                s.Blend(x, y, color);
            }
        }
    }
}

double DistanceToPolygonEdge(double x, double y, const std::vector<Point>& points) {
    double best = 1e9;
    for (size_t i = 0; i < points.size(); ++i) {
        const Point& a = points[i];
        const Point& b = points[(i + 1) % points.size()];
        best = std::min(best, DistanceToSegment(x, y, a, b));
    }
    return best;
}

void FillGlassPolygon(Surface& s, const std::vector<Point>& points) {
    if (points.size() < 3) {
        return;
    }

    const Bounds bounds = GetBounds(points);
    const int x0 = std::max(0, static_cast<int>(std::floor(bounds.minX)));
    const int x1 = std::min(s.Width() - 1, static_cast<int>(std::ceil(bounds.maxX)));
    const int y0 = std::max(0, static_cast<int>(std::floor(bounds.minY)));
    const int y1 = std::min(s.Height() - 1, static_cast<int>(std::ceil(bounds.maxY)));
    const double maxInset = std::max(1.0, std::min(bounds.maxX - bounds.minX, bounds.maxY - bounds.minY) * 0.5);

    for (int y = y0; y <= y1; ++y) {
        for (int x = x0; x <= x1; ++x) {
            const double px = x + 0.5;
            const double py = y + 0.5;
            if (!PointInPolygon(px, py, points)) {
                continue;
            }
            const double edgeInset = DistanceToPolygonEdge(px, py, points);
            s.MarkGlass(x, y);
            s.Blend(x, y, EvaluateGlassMaterial(px, py, bounds, edgeInset, maxInset));
        }
    }
}

void FillGlassCapsule(Surface& s, Point a, Point b, double width,
                      double alphaScale = 1.0,
                      bool flatClear = false) {
    const double radius = width * 0.5;
    const int x0 = std::max(0, static_cast<int>(std::floor(std::min(a.x, b.x) - radius - 1)));
    const int x1 = std::min(s.Width() - 1, static_cast<int>(std::ceil(std::max(a.x, b.x) + radius + 1)));
    const int y0 = std::max(0, static_cast<int>(std::floor(std::min(a.y, b.y) - radius - 1)));
    const int y1 = std::min(s.Height() - 1, static_cast<int>(std::ceil(std::max(a.y, b.y) + radius + 1)));
    const Bounds bounds{std::min(a.x, b.x) - radius, std::min(a.y, b.y) - radius,
                        std::max(a.x, b.x) + radius, std::max(a.y, b.y) + radius};

    for (int y = y0; y <= y1; ++y) {
        for (int x = x0; x <= x1; ++x) {
            const double px = x + 0.5;
            const double py = y + 0.5;
            const double dist = DistanceToSegment(px, py, a, b);
            if (dist > radius) {
                continue;
            }
            const double edgeInset = radius - dist;
            FColor material = flatClear && g_glassStyle == 0
                ? FColor{g_glassRed / 255.0, g_glassGreen / 255.0,
                         g_glassBlue / 255.0, g_fillOpacity / 100.0}
                : EvaluateGlassMaterial(px, py, bounds, edgeInset, radius);
            material.a *= Clamp01(alphaScale);
            s.MarkGlass(x, y);
            s.Blend(x, y, material);
        }
    }
}

void FillGlassCircle(Surface& s, Point center, double radius,
                     double alphaScale = 1.0) {
    const int x0 = std::max(0, static_cast<int>(std::floor(center.x - radius - 1)));
    const int x1 = std::min(s.Width() - 1, static_cast<int>(std::ceil(center.x + radius + 1)));
    const int y0 = std::max(0, static_cast<int>(std::floor(center.y - radius - 1)));
    const int y1 = std::min(s.Height() - 1, static_cast<int>(std::ceil(center.y + radius + 1)));
    const Bounds bounds{center.x - radius, center.y - radius,
                        center.x + radius, center.y + radius};
    const double r2 = radius * radius;

    for (int y = y0; y <= y1; ++y) {
        for (int x = x0; x <= x1; ++x) {
            const double px = x + 0.5;
            const double py = y + 0.5;
            const double dx = px - center.x;
            const double dy = py - center.y;
            const double d2 = dx * dx + dy * dy;
            if (d2 > r2) {
                continue;
            }
            const double edgeInset = radius - std::sqrt(d2);
            FColor material = g_glassStyle == 0
                ? FColor{g_glassRed / 255.0, g_glassGreen / 255.0,
                         g_glassBlue / 255.0, g_fillOpacity / 100.0}
                : EvaluateGlassMaterial(px, py, bounds, edgeInset, radius);
            material.a *= Clamp01(alphaScale);
            s.MarkGlass(x, y);
            s.Blend(x, y, material);
        }
    }
}

void FillGlassAnnulus(Surface& s, Point center, double outerRadius,
                      double innerRadius, double alphaScale = 1.0) {
    const int x0 = std::max(0, static_cast<int>(std::floor(center.x - outerRadius - 1)));
    const int x1 = std::min(s.Width() - 1, static_cast<int>(std::ceil(center.x + outerRadius + 1)));
    const int y0 = std::max(0, static_cast<int>(std::floor(center.y - outerRadius - 1)));
    const int y1 = std::min(s.Height() - 1, static_cast<int>(std::ceil(center.y + outerRadius + 1)));
    const Bounds bounds{center.x - outerRadius, center.y - outerRadius,
                        center.x + outerRadius, center.y + outerRadius};
    const double outer2 = outerRadius * outerRadius;
    const double inner2 = innerRadius * innerRadius;
    const double maxInset = std::max(1.0, (outerRadius - innerRadius) * 0.5);

    for (int y = y0; y <= y1; ++y) {
        for (int x = x0; x <= x1; ++x) {
            const double px = x + 0.5;
            const double py = y + 0.5;
            const double dx = px - center.x;
            const double dy = py - center.y;
            const double d2 = dx * dx + dy * dy;
            if (d2 > outer2 || d2 < inner2) {
                continue;
            }
            const double distance = std::sqrt(d2);
            const double edgeInset = std::min(outerRadius - distance,
                                              distance - innerRadius);
            FColor material = g_glassStyle == 0
                ? FColor{g_glassRed / 255.0, g_glassGreen / 255.0,
                         g_glassBlue / 255.0, g_fillOpacity / 100.0}
                : EvaluateGlassMaterial(px, py, bounds, edgeInset, maxInset);
            material.a *= Clamp01(alphaScale);
            s.MarkGlass(x, y);
            s.Blend(x, y, material);
        }
    }
}

void DrawCircle(Surface& s, Point center, double radius, FColor color) {
    const int x0 = std::max(0, static_cast<int>(std::floor(center.x - radius - 1)));
    const int x1 = std::min(s.Width() - 1, static_cast<int>(std::ceil(center.x + radius + 1)));
    const int y0 = std::max(0, static_cast<int>(std::floor(center.y - radius - 1)));
    const int y1 = std::min(s.Height() - 1, static_cast<int>(std::ceil(center.y + radius + 1)));
    const double r2 = radius * radius;

    for (int y = y0; y <= y1; ++y) {
        for (int x = x0; x <= x1; ++x) {
            const double dx = x + 0.5 - center.x;
            const double dy = y + 0.5 - center.y;
            if (dx * dx + dy * dy <= r2) {
                s.Blend(x, y, color);
            }
        }
    }
}

void DrawAnnulus(Surface& s, Point center, double outerRadius, double innerRadius, FColor color) {
    const int x0 = std::max(0, static_cast<int>(std::floor(center.x - outerRadius - 1)));
    const int x1 = std::min(s.Width() - 1, static_cast<int>(std::ceil(center.x + outerRadius + 1)));
    const int y0 = std::max(0, static_cast<int>(std::floor(center.y - outerRadius - 1)));
    const int y1 = std::min(s.Height() - 1, static_cast<int>(std::ceil(center.y + outerRadius + 1)));
    const double outer2 = outerRadius * outerRadius;
    const double inner2 = innerRadius * innerRadius;

    for (int y = y0; y <= y1; ++y) {
        for (int x = x0; x <= x1; ++x) {
            const double dx = x + 0.5 - center.x;
            const double dy = y + 0.5 - center.y;
            const double d2 = dx * dx + dy * dy;
            if (d2 <= outer2 && d2 >= inner2) {
                s.Blend(x, y, color);
            }
        }
    }
}

void DrawCapsule(Surface& s, Point a, Point b, double width, FColor color) {
    const double radius = width * 0.5;
    const int x0 = std::max(0, static_cast<int>(std::floor(std::min(a.x, b.x) - radius - 1)));
    const int x1 = std::min(s.Width() - 1, static_cast<int>(std::ceil(std::max(a.x, b.x) + radius + 1)));
    const int y0 = std::max(0, static_cast<int>(std::floor(std::min(a.y, b.y) - radius - 1)));
    const int y1 = std::min(s.Height() - 1, static_cast<int>(std::ceil(std::max(a.y, b.y) + radius + 1)));

    for (int y = y0; y <= y1; ++y) {
        for (int x = x0; x <= x1; ++x) {
            if (DistanceToSegment(x + 0.5, y + 0.5, a, b) <= radius) {
                s.Blend(x, y, color);
            }
        }
    }
}

void MarkProtectedCircle(Surface& s, Point center, double radius) {
    const int x0 = std::max(0, static_cast<int>(std::floor(center.x - radius - 1)));
    const int x1 = std::min(s.Width() - 1, static_cast<int>(std::ceil(center.x + radius + 1)));
    const int y0 = std::max(0, static_cast<int>(std::floor(center.y - radius - 1)));
    const int y1 = std::min(s.Height() - 1, static_cast<int>(std::ceil(center.y + radius + 1)));
    const double r2 = radius * radius;
    for (int y = y0; y <= y1; ++y) {
        for (int x = x0; x <= x1; ++x) {
            const double dx = x + 0.5 - center.x;
            const double dy = y + 0.5 - center.y;
            if (dx * dx + dy * dy <= r2) {
                s.MarkProtected(x, y);
            }
        }
    }
}

void MarkProtectedAnnulus(Surface& s, Point center, double outerRadius,
                          double innerRadius) {
    const int x0 = std::max(0, static_cast<int>(std::floor(center.x - outerRadius - 1)));
    const int x1 = std::min(s.Width() - 1, static_cast<int>(std::ceil(center.x + outerRadius + 1)));
    const int y0 = std::max(0, static_cast<int>(std::floor(center.y - outerRadius - 1)));
    const int y1 = std::min(s.Height() - 1, static_cast<int>(std::ceil(center.y + outerRadius + 1)));
    const double outer2 = outerRadius * outerRadius;
    const double inner2 = innerRadius * innerRadius;
    for (int y = y0; y <= y1; ++y) {
        for (int x = x0; x <= x1; ++x) {
            const double dx = x + 0.5 - center.x;
            const double dy = y + 0.5 - center.y;
            const double d2 = dx * dx + dy * dy;
            if (d2 <= outer2 && d2 >= inner2) {
                s.MarkProtected(x, y);
            }
        }
    }
}

void MarkProtectedCapsule(Surface& s, Point a, Point b, double width) {
    const double radius = width * 0.5;
    const int x0 = std::max(0, static_cast<int>(std::floor(std::min(a.x, b.x) - radius - 1)));
    const int x1 = std::min(s.Width() - 1, static_cast<int>(std::ceil(std::max(a.x, b.x) + radius + 1)));
    const int y0 = std::max(0, static_cast<int>(std::floor(std::min(a.y, b.y) - radius - 1)));
    const int y1 = std::min(s.Height() - 1, static_cast<int>(std::ceil(std::max(a.y, b.y) + radius + 1)));
    for (int y = y0; y <= y1; ++y) {
        for (int x = x0; x <= x1; ++x) {
            if (DistanceToSegment(x + 0.5, y + 0.5, a, b) <= radius) {
                s.MarkProtected(x, y);
            }
        }
    }
}

void MarkProtectedCapsuleRing(Surface& s, Point a, Point b,
                              double outerWidth, double innerWidth) {
    const double outerRadius = outerWidth * 0.5;
    const double innerRadius = innerWidth * 0.5;
    const int x0 = std::max(0, static_cast<int>(std::floor(std::min(a.x, b.x) - outerRadius - 1)));
    const int x1 = std::min(s.Width() - 1, static_cast<int>(std::ceil(std::max(a.x, b.x) + outerRadius + 1)));
    const int y0 = std::max(0, static_cast<int>(std::floor(std::min(a.y, b.y) - outerRadius - 1)));
    const int y1 = std::min(s.Height() - 1, static_cast<int>(std::ceil(std::max(a.y, b.y) + outerRadius + 1)));
    for (int y = y0; y <= y1; ++y) {
        for (int x = x0; x <= x1; ++x) {
            const double distance = DistanceToSegment(x + 0.5, y + 0.5, a, b);
            if (distance <= outerRadius && distance >= innerRadius) {
                s.MarkProtected(x, y);
            }
        }
    }
}

void StrokePolyline(Surface& s,
                    const std::vector<Point>& points,
                    double width,
                    FColor color,
                    bool closed) {
    if (points.size() < 2) {
        return;
    }

    for (size_t i = 1; i < points.size(); ++i) {
        DrawCapsule(s, points[i - 1], points[i], width, color);
    }
    if (closed) {
        DrawCapsule(s, points.back(), points.front(), width, color);
    }
    for (const Point& p : points) {
        DrawCircle(s, p, width * 0.5, color);
    }
}

void FillGlassPolyline(Surface& s,
                       const std::vector<Point>& points,
                       double width,
                       bool closed,
                       double alphaScale = 1.0) {
    if (points.size() < 2) {
        return;
    }
    for (size_t i = 1; i < points.size(); ++i) {
        FillGlassCapsule(s, points[i - 1], points[i], width, alphaScale, true);
    }
    if (closed) {
        FillGlassCapsule(s, points.back(), points.front(), width, alphaScale, true);
    }
    for (const Point& p : points) {
        FillGlassCircle(s, p, width * 0.5, alphaScale);
    }
}

void MarkProtectedPolyline(Surface& s,
                           const std::vector<Point>& points,
                           double width,
                           bool closed) {
    if (points.size() < 2) {
        return;
    }
    for (size_t i = 1; i < points.size(); ++i) {
        MarkProtectedCapsule(s, points[i - 1], points[i], width);
    }
    if (closed) {
        MarkProtectedCapsule(s, points.back(), points.front(), width);
    }
    for (const Point& p : points) {
        MarkProtectedCircle(s, p, width * 0.5);
    }
}

void MarkProtectedPolylineRing(Surface& s,
                               const std::vector<Point>& points,
                               double outerWidth,
                               double innerWidth,
                               bool closed) {
    if (points.size() < 2) {
        return;
    }
    for (size_t i = 1; i < points.size(); ++i) {
        MarkProtectedCapsuleRing(s, points[i - 1], points[i],
                                 outerWidth, innerWidth);
    }
    if (closed) {
        MarkProtectedCapsuleRing(s, points.back(), points.front(),
                                 outerWidth, innerWidth);
    }
    for (const Point& p : points) {
        MarkProtectedAnnulus(s, p, outerWidth * 0.5, innerWidth * 0.5);
    }
}

void DrawGlassPolygon(Surface& s,
                      const std::vector<Point>& points,
                      double outlineWidth,
                      double shadowOffset) {
    if (shadowOffset > 0.0 && g_glassStyle == 0) {
        std::vector<Point> shadowPoints = points;
        for (Point& p : shadowPoints) {
            p.x += shadowOffset;
            p.y += shadowOffset;
        }
        FillPolygon(s, shadowPoints, Shadow(0.22));
        StrokePolyline(s, shadowPoints, outlineWidth * 1.15, Shadow(0.15), true);
    }

    FillGlassPolygon(s, points);
    MarkProtectedPolyline(s, points, outlineWidth, true);
    StrokePolyline(s, points, outlineWidth, White(0.97), true);
}

void DrawGlassLine(Surface& s, Point a, Point b, double outerWidth) {
    if (g_glassStyle == 0) {
        DrawCapsule(s, {a.x + outerWidth * 0.18, a.y + outerWidth * 0.18},
                       {b.x + outerWidth * 0.18, b.y + outerWidth * 0.18},
                       outerWidth * 1.10, Shadow(0.20));
    }
    const double innerWidth = std::max(1.0, outerWidth * 0.52);
    MarkProtectedCapsuleRing(s, a, b, outerWidth, innerWidth);
    DrawCapsule(s, a, b, outerWidth, White(0.97));
    FillGlassCapsule(s, a, b, innerWidth);
}

std::vector<uint32_t> Downsample(const Surface& hi, int outSize) {
    std::vector<uint32_t> out(static_cast<size_t>(outSize) * outSize, 0);
    const int factor = kSupersample;

    for (int y = 0; y < outSize; ++y) {
        for (int x = 0; x < outSize; ++x) {
            double sumA = 0.0;
            double sumPr = 0.0;
            double sumPg = 0.0;
            double sumPb = 0.0;

            for (int sy = 0; sy < factor; ++sy) {
                for (int sx = 0; sx < factor; ++sx) {
                    const Pixel& p = hi.At(x * factor + sx, y * factor + sy);
                    sumA += p.a;
                    sumPr += p.r * p.a;
                    sumPg += p.g * p.a;
                    sumPb += p.b * p.a;
                }
            }

            const double samples = static_cast<double>(factor * factor);
            const double a = sumA / samples;
            double r = 0.0, g = 0.0, b = 0.0;
            if (sumA > 1e-8) {
                r = sumPr / sumA;
                g = sumPg / sumA;
                b = sumPb / sumA;
            }

            const BYTE A = static_cast<BYTE>(std::clamp(std::lround(a * 255.0), 0l, 255l));
            const BYTE R = static_cast<BYTE>(std::clamp(std::lround(r * 255.0), 0l, 255l));
            const BYTE G = static_cast<BYTE>(std::clamp(std::lround(g * 255.0), 0l, 255l));
            const BYTE B = static_cast<BYTE>(std::clamp(std::lround(b * 255.0), 0l, 255l));
            out[static_cast<size_t>(y) * outSize + x] =
                (static_cast<uint32_t>(A) << 24) |
                (static_cast<uint32_t>(R) << 16) |
                (static_cast<uint32_t>(G) << 8) |
                static_cast<uint32_t>(B);
        }
    }

    return out;
}

std::vector<uint8_t> DownsampleGlassMask(const Surface& hi, int outSize) {
    std::vector<uint8_t> out(static_cast<size_t>(outSize) * outSize, 0);
    const int factor = kSupersample;
    const unsigned samples = static_cast<unsigned>(factor * factor);

    for (int y = 0; y < outSize; ++y) {
        for (int x = 0; x < outSize; ++x) {
            unsigned sum = 0;
            for (int sy = 0; sy < factor; ++sy) {
                for (int sx = 0; sx < factor; ++sx) {
                    sum += hi.GlassMaskAt(x * factor + sx, y * factor + sy);
                }
            }
            out[static_cast<size_t>(y) * outSize + x] =
                static_cast<uint8_t>((sum + samples / 2) / samples);
        }
    }

    return out;
}

std::vector<uint8_t> DownsampleProtectedMask(const Surface& hi, int outSize) {
    std::vector<uint8_t> out(static_cast<size_t>(outSize) * outSize, 0);
    const int factor = kSupersample;
    const unsigned samples = static_cast<unsigned>(factor * factor);

    for (int y = 0; y < outSize; ++y) {
        for (int x = 0; x < outSize; ++x) {
            unsigned sum = 0;
            for (int sy = 0; sy < factor; ++sy) {
                for (int sx = 0; sx < factor; ++sx) {
                    sum += hi.ProtectedMaskAt(x * factor + sx, y * factor + sy);
                }
            }
            out[static_cast<size_t>(y) * outSize + x] =
                static_cast<uint8_t>((sum + samples / 2) / samples);
        }
    }

    return out;
}

Point P(double x, double y, int canvasSize) {
    return {g_renderOffsetX + x * canvasSize,
            g_renderOffsetY + y * canvasSize};
}

Point CubicPoint(Point p0, Point p1, Point p2, Point p3, double t) {
    const double u = 1.0 - t;
    const double uu = u * u;
    const double tt = t * t;
    return {
        uu * u * p0.x + 3.0 * uu * t * p1.x + 3.0 * u * tt * p2.x + tt * t * p3.x,
        uu * u * p0.y + 3.0 * uu * t * p1.y + 3.0 * u * tt * p2.y + tt * t * p3.y
    };
}

void AppendCubic(std::vector<Point>& path, Point c1, Point c2, Point end, int steps = 8) {
    if (path.empty()) {
        path.push_back(end);
        return;
    }

    const Point start = path.back();
    const double controlLength =
        std::hypot(c1.x - start.x, c1.y - start.y) +
        std::hypot(c2.x - c1.x, c2.y - c1.y) +
        std::hypot(end.x - c2.x, end.y - c2.y);

    // Curve subdivision is based on supersampled path length rather than a
    // fixed handful of points. This keeps small/scaled cursors looking like
    // vector artwork instead of revealing the polygonal Bezier approximation.
    const int adaptiveSteps = std::clamp(
        static_cast<int>(std::ceil(controlLength / 2.0)), steps, 64);

    for (int i = 1; i <= adaptiveSteps; ++i) {
        const double t = i / static_cast<double>(adaptiveSteps);
        path.push_back(CubicPoint(start, c1, c2, end, t));
    }
}

void RenderArrowShapeSymmetricSharp(Surface& s, int n) {
    // Symmetric pointer with a pointed inner notch.
    const double ow = n * 0.046;
    const double sh = n * 0.014;

    const Point tip = P(0.235, 0.085, n);
    const Point notch = P(0.475, 0.605, n);

    const auto mirror = [&](Point point) -> Point {
        const double dx = notch.x - tip.x;
        const double dy = notch.y - tip.y;
        const double len2 = dx * dx + dy * dy;
        if (len2 <= 1e-12) return point;
        const double rx = point.x - tip.x;
        const double ry = point.y - tip.y;
        const double t = (rx * dx + ry * dy) / len2;
        const Point projection = {tip.x + t * dx, tip.y + t * dy};
        return {2.0 * projection.x - point.x, 2.0 * projection.y - point.y};
    };

    std::vector<Point> leftSide;
    leftSide.reserve(40);
    leftSide.push_back(tip);
    AppendCubic(leftSide,
                P(0.205, 0.090, n),
                P(0.180, 0.125, n),
                P(0.180, 0.175, n), 5);
    leftSide.push_back(P(0.180, 0.665, n));
    AppendCubic(leftSide,
                P(0.180, 0.735, n),
                P(0.225, 0.785, n),
                P(0.285, 0.775, n), 6);
    AppendCubic(leftSide,
                P(0.340, 0.758, n),
                P(0.405, 0.650, n),
                notch, 6);

    std::vector<Point> pts = leftSide;
    pts.reserve(leftSide.size() * 2);
    for (size_t i = leftSide.size() - 1; i-- > 1;) {
        pts.push_back(mirror(leftSide[i]));
    }
    DrawGlassPolygon(s, pts, ow, sh);
}


void RenderArrowShapeSymmetricCurved(Surface& s, int n) {
    // Symmetric pointer with a single continuous cubic Bezier forming the
    // inner notch. The curve crosses the symmetry axis without a center vertex.
    const double ow = n * 0.046;
    const double sh = n * 0.014;

    const Point tip = P(0.235, 0.085, n);
    const Point notch = P(0.475, 0.605, n);

    const auto mirror = [&](Point point) -> Point {
        const double dx = notch.x - tip.x;
        const double dy = notch.y - tip.y;
        const double len2 = dx * dx + dy * dy;
        if (len2 <= 1e-12) return point;
        const double rx = point.x - tip.x;
        const double ry = point.y - tip.y;
        const double t = (rx * dx + ry * dy) / len2;
        const Point projection = {tip.x + t * dx, tip.y + t * dy};
        return {2.0 * projection.x - point.x,
                2.0 * projection.y - point.y};
    };

    std::vector<Point> leftSide;
    leftSide.reserve(40);
    leftSide.push_back(tip);
    AppendCubic(leftSide,
                P(0.205, 0.090, n),
                P(0.180, 0.125, n),
                P(0.180, 0.175, n), 5);
    leftSide.push_back(P(0.180, 0.665, n));
    AppendCubic(leftSide,
                P(0.180, 0.735, n),
                P(0.225, 0.785, n),
                P(0.285, 0.775, n), 6);

    // Follow the outer contour to the notch lip, then transition into a
    // single continuous cubic curve across the indentation.
    const Point outerCurveStart = leftSide.back();
    const Point outerControl1 = P(0.340, 0.758, n);
    const Point outerControl2 = P(0.405, 0.650, n);
    constexpr double kLipT = 0.50;

    leftSide.push_back(CubicPoint(outerCurveStart,
                                  outerControl1,
                                  outerControl2,
                                  notch,
                                  0.25));
    const Point leftLip = CubicPoint(outerCurveStart,
                                     outerControl1,
                                     outerControl2,
                                     notch,
                                     kLipT);
    leftSide.push_back(leftLip);

    const Point rightLip = mirror(leftLip);

    // Continue the outer contour tangent into the notch so the transition
    // remains smooth at the lip.
    const double u = 1.0 - kLipT;
    const Point tangent = {
        3.0 * u * u * (outerControl1.x - outerCurveStart.x) +
        6.0 * u * kLipT * (outerControl2.x - outerControl1.x) +
        3.0 * kLipT * kLipT * (notch.x - outerControl2.x),
        3.0 * u * u * (outerControl1.y - outerCurveStart.y) +
        6.0 * u * kLipT * (outerControl2.y - outerControl1.y) +
        3.0 * kLipT * kLipT * (notch.y - outerControl2.y)
    };

    // Scale the control tangent so the cubic reaches the intended notch depth
    // while crossing the symmetry axis with a smooth tangent.
    const double axisDx = notch.x - tip.x;
    const double axisDy = notch.y - tip.y;
    const double axisLength = std::hypot(axisDx, axisDy);
    const double axisX = axisDx / axisLength;
    const double axisY = axisDy / axisLength;

    const auto axisPosition = [&](Point point) -> double {
        return (point.x - tip.x) * axisX + (point.y - tip.y) * axisY;
    };

    const double lipAxis = axisPosition(leftLip);
    const double notchAxis = axisPosition(notch);
    const double tangentAxis = tangent.x * axisX + tangent.y * axisY;
    const double tangentScale = std::abs(tangentAxis) > 1e-12
        ? (4.0 * (notchAxis - lipAxis)) / (3.0 * tangentAxis)
        : 0.0;

    const Point leftControl = {
        leftLip.x + tangent.x * tangentScale,
        leftLip.y + tangent.y * tangentScale
    };
    const Point rightControl = mirror(leftControl);

    std::vector<Point> pts = leftSide;
    pts.reserve(leftSide.size() * 2 + 16);

    // One uninterrupted cubic runs from the left lip to the right lip. The
    // notch point controls depth but is not a path endpoint.
    AppendCubic(pts,
                leftControl,
                rightControl,
                rightLip,
                14);

    // Continue along the mirrored outer contour, skipping the lip because it
    // is already the endpoint of the notch cubic.
    for (size_t i = leftSide.size() - 1; i-- > 1;) {
        pts.push_back(mirror(leftSide[i]));
    }

    DrawGlassPolygon(s, pts, ow, sh);
}

void RenderArrowShapeCleanRounded(Surface& s, int n) {
    // Smooth rounded outer contour with a curved inner notch.
    const double ow = n * 0.044;
    const double sh = n * 0.013;

    const Point tip = P(0.235, 0.085, n);
    const Point notch = P(0.475, 0.605, n);

    const auto mirror = [&](Point point) -> Point {
        const double dx = notch.x - tip.x;
        const double dy = notch.y - tip.y;
        const double len2 = dx * dx + dy * dy;
        if (len2 <= 1e-12) return point;
        const double rx = point.x - tip.x;
        const double ry = point.y - tip.y;
        const double t = (rx * dx + ry * dy) / len2;
        const Point projection = {tip.x + t * dx, tip.y + t * dy};
        return {2.0 * projection.x - point.x, 2.0 * projection.y - point.y};
    };

    const Point leftLip = P(0.414, 0.650, n);
    const Point rightLip = mirror(leftLip);

    std::vector<Point> leftSide;
    leftSide.reserve(64);
    leftSide.push_back(tip);
    AppendCubic(leftSide,
                P(0.190, 0.100, n),
                P(0.176, 0.245, n),
                P(0.180, 0.525, n), 14);
    AppendCubic(leftSide,
                P(0.181, 0.675, n),
                P(0.225, 0.785, n),
                P(0.310, 0.775, n), 12);
    AppendCubic(leftSide,
                P(0.360, 0.770, n),
                P(0.400, 0.695, n),
                leftLip, 10);

    std::vector<Point> pts = leftSide;
    pts.reserve(leftSide.size() * 2 + 24);

    const double dx = notch.x - tip.x;
    const double dy = notch.y - tip.y;
    const double axisLength = std::hypot(dx, dy);
    const double perpX = -dy / axisLength;
    const double perpY = dx / axisLength;
    const Point notchLeftTangent = {
        notch.x - perpX * n * 0.026,
        notch.y - perpY * n * 0.026
    };
    const Point notchRightTangent = mirror(notchLeftTangent);

    AppendCubic(pts,
                P(0.438, 0.638, n),
                notchLeftTangent,
                notch, 10);
    AppendCubic(pts,
                notchRightTangent,
                mirror(P(0.438, 0.638, n)),
                rightLip, 10);

    for (size_t i = leftSide.size() - 1; i-- > 1;) {
        pts.push_back(mirror(leftSide[i]));
    }
    DrawGlassPolygon(s, pts, ow, sh);
}

void RenderArrowShapeSharpRounded(Surface& s, int n) {
    // Sharper outer walls with rounded tips and corners plus a curved notch.
    const double ow = n * 0.043;
    const double sh = n * 0.012;

    const Point tip = P(0.235, 0.085, n);
    const Point notch = P(0.475, 0.605, n);

    const auto mirror = [&](Point point) -> Point {
        const double dx = notch.x - tip.x;
        const double dy = notch.y - tip.y;
        const double len2 = dx * dx + dy * dy;
        if (len2 <= 1e-12) return point;
        const double rx = point.x - tip.x;
        const double ry = point.y - tip.y;
        const double t = (rx * dx + ry * dy) / len2;
        const Point projection = {tip.x + t * dx, tip.y + t * dy};
        return {2.0 * projection.x - point.x, 2.0 * projection.y - point.y};
    };

    const Point leftLip = P(0.414, 0.650, n);
    const Point rightLip = mirror(leftLip);

    std::vector<Point> leftSide;
    leftSide.reserve(48);
    leftSide.push_back(tip);
    AppendCubic(leftSide,
                P(0.205, 0.088, n),
                P(0.187, 0.112, n),
                P(0.184, 0.155, n), 5);
    leftSide.push_back(P(0.181, 0.600, n));
    AppendCubic(leftSide,
                P(0.181, 0.700, n),
                P(0.225, 0.775, n),
                P(0.300, 0.775, n), 7);
    leftSide.push_back(P(0.365, 0.720, n));
    AppendCubic(leftSide,
                P(0.390, 0.697, n),
                P(0.405, 0.670, n),
                leftLip, 5);

    std::vector<Point> pts = leftSide;
    pts.reserve(leftSide.size() * 2 + 16);

    const double dx = notch.x - tip.x;
    const double dy = notch.y - tip.y;
    const double axisLength = std::hypot(dx, dy);
    const double perpX = -dy / axisLength;
    const double perpY = dx / axisLength;
    const Point leftTangent = {
        notch.x - perpX * n * 0.022,
        notch.y - perpY * n * 0.022
    };
    const Point rightTangent = mirror(leftTangent);

    AppendCubic(pts,
                P(0.435, 0.637, n),
                leftTangent,
                notch, 7);
    AppendCubic(pts,
                rightTangent,
                mirror(P(0.435, 0.637, n)),
                rightLip, 7);

    for (size_t i = leftSide.size() - 1; i-- > 1;) {
        pts.push_back(mirror(leftSide[i]));
    }
    DrawGlassPolygon(s, pts, ow, sh);
}

void RenderArrowShape(Surface& s, int n) {
    switch (g_pointerStyle) {
        case 1:
            RenderArrowShapeCleanRounded(s, n);
            break;
        case 2:
            RenderArrowShapeSymmetricSharp(s, n);
            break;
        case 3:
            RenderArrowShapeSymmetricCurved(s, n);
            break;
        default:
            RenderArrowShapeSharpRounded(s, n);
            break;
    }
}

void RenderHandShape(Surface& s, int n) {
    // Hand silhouette adapted from Bootstrap Icons' MIT-licensed hand-index
    // icon, scaled and rendered with the glass treatment.
    static constexpr double kHand[][2] = {
        {0.3788,0.0922}, {0.3619,0.1084}, {0.3450,0.1470},
        {0.3450,0.4233}, {0.2831,0.4375}, {0.2663,0.4457},
        {0.2456,0.4680}, {0.2363,0.4944}, {0.2363,0.5248},
        {0.2513,0.6711}, {0.2606,0.7056}, {0.3338,0.8275},
        {0.3506,0.8458}, {0.3750,0.8580}, {0.6938,0.8580},
        {0.7181,0.8458}, {0.7369,0.8238}, {0.8100,0.6812},
        {0.8194,0.6426}, {0.8325,0.5025}, {0.8269,0.4578},
        {0.8156,0.4355}, {0.7894,0.4091}, {0.7594,0.3969},
        {0.7181,0.3969}, {0.6975,0.3705}, {0.6675,0.3482},
        {0.6375,0.3421}, {0.6113,0.3442}, {0.5925,0.3178},
        {0.5569,0.2995}, {0.5231,0.2954}, {0.5025,0.2975},
        {0.5006,0.1470}, {0.4894,0.1166}, {0.4669,0.0922},
        {0.4463,0.0820}, {0.4069,0.0800}
    };

    std::vector<Point> pts;
    pts.reserve(sizeof(kHand) / sizeof(kHand[0]));
    for (const auto& p : kHand) {
        pts.push_back(P(p[0], p[1], n));
    }

    DrawGlassPolygon(s, pts, n * 0.034, n * 0.010);

    if (g_handStyle == 1) {
        // Thin separators distinguish the fingers without overpowering the
        // translucent interior.
        const double lineWidth = n * 0.017;
        const Point sep1a = P(0.502, 0.300, n);
        const Point sep1b = P(0.502, 0.492, n);
        const Point sep2a = P(0.612, 0.350, n);
        const Point sep2b = P(0.612, 0.505, n);
        const Point sep3a = P(0.716, 0.402, n);
        const Point sep3b = P(0.716, 0.535, n);
        MarkProtectedCapsule(s, sep1a, sep1b, lineWidth);
        MarkProtectedCapsule(s, sep2a, sep2b, lineWidth);
        MarkProtectedCapsule(s, sep3a, sep3b, lineWidth);
        DrawCapsule(s, sep1a, sep1b, lineWidth, White(0.92));
        DrawCapsule(s, sep2a, sep2b, lineWidth, White(0.92));
        DrawCapsule(s, sep3a, sep3b, lineWidth, White(0.92));
    }
}

void RenderIBeam(Surface& s, int n) {
    // Narrower glass pill. The thin white rim leaves most of the body available
    // for the selected 25% / 50% translucent fill.
    const double ow = n * 0.022;
    const double sh = n * 0.007;

    std::vector<Point> pts;
    pts.reserve(48);
    pts.push_back(P(0.430, 0.205, n));
    AppendCubic(pts,
                P(0.430, 0.145, n),
                P(0.455, 0.114, n),
                P(0.500, 0.114, n), 8);
    AppendCubic(pts,
                P(0.545, 0.114, n),
                P(0.570, 0.145, n),
                P(0.570, 0.205, n), 8);
    pts.push_back(P(0.570, 0.795, n));
    AppendCubic(pts,
                P(0.570, 0.855, n),
                P(0.545, 0.886, n),
                P(0.500, 0.886, n), 8);
    AppendCubic(pts,
                P(0.455, 0.886, n),
                P(0.430, 0.855, n),
                P(0.430, 0.795, n), 8);
    pts.push_back(P(0.430, 0.205, n));

    DrawGlassPolygon(s, pts, ow, sh);
}

void RenderCross(Surface& s, int n) {
    const double w = n * 0.075;
    DrawGlassLine(s, P(0.18, 0.50, n), P(0.82, 0.50, n), w);
    DrawGlassLine(s, P(0.50, 0.18, n), P(0.50, 0.82, n), w);
}

void RenderArrowHead(Surface& s, Point tip, Point left, Point right, double ow) {
    std::vector<Point> pts = {tip, left, right};
    DrawGlassPolygon(s, pts, ow, ow * 0.25);
}

void RenderUp(Surface& s, int n) {
    const double w = n * 0.075;
    DrawGlassLine(s, P(0.50, 0.24, n), P(0.50, 0.82, n), w);
    RenderArrowHead(s, P(0.50, 0.10, n), P(0.30, 0.34, n), P(0.70, 0.34, n), n * 0.045);
}

void RenderResize(Surface& s, int n, int mode) {
    const double w = n * 0.064;
    if (mode == 0) { // NW-SE
        DrawGlassLine(s, P(0.25, 0.25, n), P(0.75, 0.75, n), w);
        RenderArrowHead(s, P(0.13, 0.13, n), P(0.36, 0.18, n), P(0.18, 0.36, n), n * 0.040);
        RenderArrowHead(s, P(0.87, 0.87, n), P(0.64, 0.82, n), P(0.82, 0.64, n), n * 0.040);
    } else if (mode == 1) { // NE-SW
        DrawGlassLine(s, P(0.75, 0.25, n), P(0.25, 0.75, n), w);
        RenderArrowHead(s, P(0.87, 0.13, n), P(0.82, 0.36, n), P(0.64, 0.18, n), n * 0.040);
        RenderArrowHead(s, P(0.13, 0.87, n), P(0.18, 0.64, n), P(0.36, 0.82, n), n * 0.040);
    } else if (mode == 2) { // WE
        DrawGlassLine(s, P(0.26, 0.50, n), P(0.74, 0.50, n), w);
        RenderArrowHead(s, P(0.10, 0.50, n), P(0.34, 0.31, n), P(0.34, 0.69, n), n * 0.040);
        RenderArrowHead(s, P(0.90, 0.50, n), P(0.66, 0.31, n), P(0.66, 0.69, n), n * 0.040);
    } else { // NS
        DrawGlassLine(s, P(0.50, 0.26, n), P(0.50, 0.74, n), w);
        RenderArrowHead(s, P(0.50, 0.10, n), P(0.31, 0.34, n), P(0.69, 0.34, n), n * 0.040);
        RenderArrowHead(s, P(0.50, 0.90, n), P(0.31, 0.66, n), P(0.69, 0.66, n), n * 0.040);
    }
}

void RenderMove(Surface& s, int n) {
    const double w = n * 0.060;
    DrawGlassLine(s, P(0.50, 0.22, n), P(0.50, 0.78, n), w);
    DrawGlassLine(s, P(0.22, 0.50, n), P(0.78, 0.50, n), w);
    RenderArrowHead(s, P(0.50, 0.08, n), P(0.35, 0.28, n), P(0.65, 0.28, n), n * 0.035);
    RenderArrowHead(s, P(0.50, 0.92, n), P(0.35, 0.72, n), P(0.65, 0.72, n), n * 0.035);
    RenderArrowHead(s, P(0.08, 0.50, n), P(0.28, 0.35, n), P(0.28, 0.65, n), n * 0.035);
    RenderArrowHead(s, P(0.92, 0.50, n), P(0.72, 0.35, n), P(0.72, 0.65, n), n * 0.035);
}

void RenderUnavailable(Surface& s, int n) {
    const Point c = P(0.50, 0.50, n);
    const double r = n * 0.30;
    DrawAnnulus(s, {c.x + n * 0.012, c.y + n * 0.012},
                r + n * 0.042, r - n * 0.045, Shadow(0.18));
    MarkProtectedAnnulus(s, c, r + n * 0.040, r - n * 0.004);
    MarkProtectedAnnulus(s, c, r - n * 0.040, r - n * 0.055);
    DrawAnnulus(s, c, r + n * 0.040, r - n * 0.055, White(0.97));
    FillGlassAnnulus(s, c, r - n * 0.004, r - n * 0.040);
    DrawGlassLine(s, P(0.30, 0.30, n), P(0.70, 0.70, n), n * 0.085);
}

void RenderHelp(Surface& s, int n) {
    RenderArrowShape(s, n);
    const double w = n * 0.050;
    DrawGlassLine(s, P(0.61, 0.61, n), P(0.61, 0.66, n), w);
    const Point q0 = P(0.61, 0.47, n);
    const Point q1 = P(0.69, 0.42, n);
    const Point q2 = P(0.74, 0.49, n);
    const Point q3 = P(0.66, 0.57, n);
    const Point dot = P(0.61, 0.73, n);
    MarkProtectedCapsule(s, q0, q1, w);
    MarkProtectedCapsule(s, q1, q2, w);
    MarkProtectedCapsule(s, q2, q3, w);
    MarkProtectedCircle(s, dot, w * 0.50);
    DrawCapsule(s, q0, q1, w, White(0.97));
    DrawCapsule(s, q1, q2, w, White(0.97));
    DrawCapsule(s, q2, q3, w, White(0.97));
    DrawCircle(s, dot, w * 0.50, White(0.97));
}

void RenderPin(Surface& s, int n) {
    const double ow = n * 0.045;
    std::vector<Point> body = {
        P(0.50, 0.90, n), P(0.32, 0.63, n), P(0.28, 0.48, n),
        P(0.31, 0.33, n), P(0.40, 0.23, n), P(0.50, 0.20, n),
        P(0.60, 0.23, n), P(0.69, 0.33, n), P(0.72, 0.48, n),
        P(0.68, 0.63, n)
    };
    DrawGlassPolygon(s, body, ow, n * 0.018);
    const Point holeCenter = P(0.50, 0.46, n);
    MarkProtectedAnnulus(s, holeCenter, n * 0.115, n * 0.067);
    DrawCircle(s, holeCenter, n * 0.115, White(0.96));
    FillGlassCircle(s, holeCenter, n * 0.067, 0.65);
}

void RenderPerson(Surface& s, int n) {
    DrawCircle(s, P(0.50, 0.32, n), n * 0.15, Shadow(0.18));
    const Point headCenter = P(0.50, 0.30, n);
    const double headOuter = n * 0.15 + n * 0.035;
    const double headInner = n * 0.15 - n * 0.012;
    MarkProtectedAnnulus(s, headCenter, headOuter, headInner);
    DrawCircle(s, headCenter, headOuter, White(0.97));
    FillGlassCircle(s, headCenter, headInner);

    std::vector<Point> shoulders = {
        P(0.20, 0.84, n), P(0.24, 0.67, n), P(0.35, 0.56, n),
        P(0.50, 0.52, n), P(0.65, 0.56, n), P(0.76, 0.67, n),
        P(0.80, 0.84, n)
    };
    MarkProtectedPolylineRing(s, shoulders, n * 0.095, n * 0.047, false);
    StrokePolyline(s, shoulders, n * 0.095, White(0.97), false);
    FillGlassPolyline(s, shoulders, n * 0.047, false);
}

void RenderSpinner(Surface& s,
                   int n,
                   Point center,
                   double radius,
                   double dotRadius,
                   int frame) {
    // Continuous rotating glass arc instead of a dotted activity wheel.
    constexpr int segments = 30;
    const double span = kPi * 1.58;
    const double rotation = (frame % kSpinnerFrames) * (2.0 * kPi / kSpinnerFrames);
    const double stroke = dotRadius * 2.15;
    const double innerStroke = std::max(1.0, stroke * 0.48);

    for (int i = 0; i < segments; ++i) {
        const double f0 = i / static_cast<double>(segments);
        const double f1 = (i + 1) / static_cast<double>(segments);
        const double a0 = rotation - span + span * f0;
        const double a1 = rotation - span + span * f1;
        const double fade = 0.12 + 0.88 * f1;

        const Point p0 = {
            center.x + std::cos(a0) * radius,
            center.y + std::sin(a0) * radius
        };
        const Point p1 = {
            center.x + std::cos(a1) * radius,
            center.y + std::sin(a1) * radius
        };

        DrawCapsule(s,
                    {p0.x + stroke * 0.12, p0.y + stroke * 0.12},
                    {p1.x + stroke * 0.12, p1.y + stroke * 0.12},
                    stroke * 1.08,
                    Shadow(0.10 * fade));
        MarkProtectedCapsuleRing(s, p0, p1, stroke, innerStroke);
        DrawCapsule(s, p0, p1, stroke, White(0.18 + 0.80 * fade));
        FillGlassCapsule(s, p0, p1, innerStroke, 0.30 + 0.70 * fade, true);
    }
}

Point GetRoleHotspot(DWORD role) {
    switch (role) {
        case CURSOR_NORMAL:
        case CURSOR_APPSTARTING:
        case CURSOR_HELP:
            return {0.235, 0.085};
        case CURSOR_HAND:
            return {0.420, 0.085};
        case CURSOR_UP:
            return {0.500, 0.100};
        case CURSOR_PIN:
            return {0.500, 0.900};
        default:
            return {0.500, 0.500};
    }
}

void RenderRoleGeometry(Surface& surface, int n, DWORD role, int frame) {
    switch (role) {
        case CURSOR_NORMAL:
            RenderArrowShape(surface, n);
            break;
        case CURSOR_IBEAM:
            RenderIBeam(surface, n);
            break;
        case CURSOR_WAIT:
            RenderSpinner(surface, n, P(0.50, 0.50, n), n * 0.255,
                          n * 0.041, frame % kSpinnerFrames);
            break;
        case CURSOR_CROSS:
            RenderCross(surface, n);
            break;
        case CURSOR_UP:
            RenderUp(surface, n);
            break;
        case CURSOR_SIZENWSE:
            RenderResize(surface, n, 0);
            break;
        case CURSOR_SIZENESW:
            RenderResize(surface, n, 1);
            break;
        case CURSOR_SIZEWE:
            RenderResize(surface, n, 2);
            break;
        case CURSOR_SIZENS:
            RenderResize(surface, n, 3);
            break;
        case CURSOR_SIZEALL:
            RenderMove(surface, n);
            break;
        case CURSOR_NO:
            RenderUnavailable(surface, n);
            break;
        case CURSOR_HAND:
            RenderHandShape(surface, n);
            break;
        case CURSOR_APPSTARTING:
            RenderArrowShape(surface, n);
            RenderSpinner(surface, n, P(0.805, 0.185, n), n * 0.115,
                          n * 0.020, frame % kSpinnerFrames);
            break;
        case CURSOR_HELP:
            RenderHelp(surface, n);
            break;
        case CURSOR_PIN:
            RenderPin(surface, n);
            break;
        case CURSOR_PERSON:
            RenderPerson(surface, n);
            break;
    }
}

bool FindSurfaceArtworkBounds(const Surface& surface, Bounds& bounds) {
    bool found = false;
    double minX = static_cast<double>(surface.Width());
    double minY = static_cast<double>(surface.Height());
    double maxX = 0.0;
    double maxY = 0.0;

    for (int y = 0; y < surface.Height(); ++y) {
        for (int x = 0; x < surface.Width(); ++x) {
            if (surface.GlassMaskAt(x, y) == 0 &&
                surface.ProtectedMaskAt(x, y) == 0) {
                continue;
            }
            found = true;
            minX = std::min(minX, static_cast<double>(x));
            minY = std::min(minY, static_cast<double>(y));
            maxX = std::max(maxX, static_cast<double>(x + 1));
            maxY = std::max(maxY, static_cast<double>(y + 1));
        }
    }

    if (found) {
        bounds = {minX, minY, maxX, maxY};
    }
    return found;
}

double ResolveRoleScaleNormalization(int fullCanvasSize,
                                     DWORD role,
                                     int frame,
                                     const Point& hotspot,
                                     double targetHotspotX,
                                     double targetHotspotY) {
    const int roleIndex = CursorRoleIndex(role);
    if (roleIndex >= 0 &&
        g_roleScaleNormalizationValid[static_cast<size_t>(roleIndex)]) {
        return g_roleScaleNormalization[static_cast<size_t>(roleIndex)];
    }

    Surface probe(fullCanvasSize, fullCanvasSize);
    const int probeN = fullCanvasSize;
    g_renderOffsetX = targetHotspotX - hotspot.x * probeN;
    g_renderOffsetY = targetHotspotY - hotspot.y * probeN;
    RenderRoleGeometry(probe, probeN, role, frame);

    Bounds bounds = {};
    double normalization = 1.0;
    if (FindSurfaceArtworkBounds(probe, bounds)) {
        const double width = std::max(1.0, bounds.maxX - bounds.minX);
        const double height = std::max(1.0, bounds.maxY - bounds.minY);
        const double extent = std::max(width, height);

        // ArtworkScale should describe the visible cursor, not the abstract 0..1
        // design coordinate system. Normalize every role to a common 94% visual
        // box at 100%, leaving a small antialiasing margin around the canvas.
        normalization = (fullCanvasSize * 0.94) / extent;

        // Scaling is performed around the hotspot. Cap the normalization so a
        // role whose hotspot sits near an edge (pointer, pin, etc.) cannot be
        // enlarged past the available canvas and silently clipped.
        double maxFit = 1e9;
        const double leftDistance = targetHotspotX - bounds.minX;
        const double rightDistance = bounds.maxX - targetHotspotX;
        const double topDistance = targetHotspotY - bounds.minY;
        const double bottomDistance = bounds.maxY - targetHotspotY;
        if (leftDistance > 0.0) {
            maxFit = std::min(maxFit, targetHotspotX / leftDistance);
        }
        if (rightDistance > 0.0) {
            maxFit = std::min(
                maxFit, (fullCanvasSize - targetHotspotX) / rightDistance);
        }
        if (topDistance > 0.0) {
            maxFit = std::min(maxFit, targetHotspotY / topDistance);
        }
        if (bottomDistance > 0.0) {
            maxFit = std::min(
                maxFit, (fullCanvasSize - targetHotspotY) / bottomDistance);
        }
        if (maxFit < 1e8) {
            normalization = std::min(normalization, maxFit * 0.98);
        }
        normalization = std::clamp(normalization, 0.25, 2.5);
    }

    if (roleIndex >= 0) {
        g_roleScaleNormalization[static_cast<size_t>(roleIndex)] = normalization;
        g_roleScaleNormalizationValid[static_cast<size_t>(roleIndex)] = true;
    }
    return normalization;
}

RenderedCursor RenderRole(int size, DWORD role, int frame = 0) {
    const int fullCanvasSize = size * kSupersample;
    const Point hotspot = GetRoleHotspot(role);
    const int hotspotX = std::clamp(
        static_cast<int>(std::floor(size * hotspot.x)), 0, size - 1);
    const int hotspotY = std::clamp(
        static_cast<int>(std::floor(size * hotspot.y)), 0, size - 1);
    const double targetHotspotX =
        (static_cast<double>(hotspotX) + 0.5) * kSupersample;
    const double targetHotspotY =
        (static_cast<double>(hotspotY) + 0.5) * kSupersample;

    const double artworkScale =
        std::clamp(g_artworkScale / 100.0, 0.01, 1.0);
    const double roleNormalization = ResolveRoleScaleNormalization(
        fullCanvasSize, role, frame, hotspot, targetHotspotX, targetHotspotY);
    const int n = std::max(1, static_cast<int>(std::lround(
        fullCanvasSize * artworkScale * roleNormalization)));

    Surface surface(fullCanvasSize, fullCanvasSize);
    g_renderOffsetX = targetHotspotX - hotspot.x * n;
    g_renderOffsetY = targetHotspotY - hotspot.y * n;
    RenderRoleGeometry(surface, n, role, frame);

    RenderedCursor result;
    result.size = size;
    result.hotspotX = hotspotX;
    result.hotspotY = hotspotY;
    result.artworkScale = artworkScale * roleNormalization;
    result.pixels = Downsample(surface, size);
    result.glassMask = DownsampleGlassMask(surface, size);
    result.protectedMask = DownsampleProtectedMask(surface, size);
    return result;
}

void DestroyDynamicCursorFactory() {
    if (g_dynamicCursorFactory.maskBitmap) {
        DeleteObject(g_dynamicCursorFactory.maskBitmap);
        g_dynamicCursorFactory.maskBitmap = nullptr;
    }
    if (g_dynamicCursorFactory.colorBitmap) {
        DeleteObject(g_dynamicCursorFactory.colorBitmap);
        g_dynamicCursorFactory.colorBitmap = nullptr;
    }
    g_dynamicCursorFactory.colorBits = nullptr;
    g_dynamicCursorFactory.size = 0;
}

bool EnsureDynamicCursorFactory(int size) {
    if (g_dynamicCursorFactory.colorBitmap &&
        g_dynamicCursorFactory.maskBitmap &&
        g_dynamicCursorFactory.colorBits &&
        g_dynamicCursorFactory.size == size) {
        return true;
    }

    DestroyDynamicCursorFactory();

    BITMAPV5HEADER bi = {};
    bi.bV5Size = sizeof(BITMAPV5HEADER);
    bi.bV5Width = size;
    bi.bV5Height = -size;
    bi.bV5Planes = 1;
    bi.bV5BitCount = 32;
    bi.bV5Compression = BI_BITFIELDS;
    bi.bV5RedMask = 0x00FF0000;
    bi.bV5GreenMask = 0x0000FF00;
    bi.bV5BlueMask = 0x000000FF;
    bi.bV5AlphaMask = 0xFF000000;

    HDC hdc = GetDC(nullptr);
    void* bits = nullptr;
    HBITMAP colorBitmap = CreateDIBSection(
        hdc, reinterpret_cast<BITMAPINFO*>(&bi), DIB_RGB_COLORS,
        &bits, nullptr, 0);
    ReleaseDC(nullptr, hdc);
    if (!colorBitmap || !bits) {
        if (colorBitmap) {
            DeleteObject(colorBitmap);
        }
        return false;
    }

    const size_t maskRowBytes = static_cast<size_t>((size + 15) / 16) * 2;
    std::vector<BYTE> maskBits(maskRowBytes * size, 0);
    HBITMAP maskBitmap = CreateBitmap(size, size, 1, 1, maskBits.data());
    if (!maskBitmap) {
        DeleteObject(colorBitmap);
        return false;
    }

    g_dynamicCursorFactory.colorBitmap = colorBitmap;
    g_dynamicCursorFactory.maskBitmap = maskBitmap;
    g_dynamicCursorFactory.colorBits = bits;
    g_dynamicCursorFactory.size = size;
    return true;
}

HCURSOR CreateDynamicCursorFromPixels(const RenderedCursor& cursor,
                                      const std::vector<uint32_t>& pixels) {
    const size_t expectedPixels =
        static_cast<size_t>(cursor.size) * static_cast<size_t>(cursor.size);
    if (pixels.size() != expectedPixels ||
        !EnsureDynamicCursorFactory(cursor.size)) {
        return nullptr;
    }

    std::memcpy(g_dynamicCursorFactory.colorBits,
                pixels.data(),
                pixels.size() * sizeof(uint32_t));

    ICONINFO iconInfo = {};
    iconInfo.fIcon = FALSE;
    iconInfo.xHotspot = cursor.hotspotX;
    iconInfo.yHotspot = cursor.hotspotY;
    iconInfo.hbmMask = g_dynamicCursorFactory.maskBitmap;
    iconInfo.hbmColor = g_dynamicCursorFactory.colorBitmap;
    return CreateIconIndirect(&iconInfo);
}

HCURSOR CreateCursorFromPixels(const RenderedCursor& cursor) {
    BITMAPV5HEADER bi = {};
    bi.bV5Size = sizeof(BITMAPV5HEADER);
    bi.bV5Width = cursor.size;
    bi.bV5Height = -cursor.size;
    bi.bV5Planes = 1;
    bi.bV5BitCount = 32;
    bi.bV5Compression = BI_BITFIELDS;
    bi.bV5RedMask = 0x00FF0000;
    bi.bV5GreenMask = 0x0000FF00;
    bi.bV5BlueMask = 0x000000FF;
    bi.bV5AlphaMask = 0xFF000000;

    void* bits = nullptr;
    HDC hdc = GetDC(nullptr);
    HBITMAP colorBitmap = CreateDIBSection(
        hdc, reinterpret_cast<BITMAPINFO*>(&bi), DIB_RGB_COLORS, &bits, nullptr, 0);
    ReleaseDC(nullptr, hdc);

    if (!colorBitmap || !bits) {
        if (colorBitmap) {
            DeleteObject(colorBitmap);
        }
        return nullptr;
    }

    std::memcpy(bits, cursor.pixels.data(), cursor.pixels.size() * sizeof(uint32_t));

    const size_t maskRowBytes = static_cast<size_t>((cursor.size + 15) / 16) * 2;
    std::vector<BYTE> maskBits(maskRowBytes * cursor.size, 0);
    HBITMAP maskBitmap = CreateBitmap(cursor.size, cursor.size, 1, 1, maskBits.data());
    if (!maskBitmap) {
        DeleteObject(colorBitmap);
        return nullptr;
    }

    ICONINFO iconInfo = {};
    iconInfo.fIcon = FALSE;
    iconInfo.xHotspot = cursor.hotspotX;
    iconInfo.yHotspot = cursor.hotspotY;
    iconInfo.hbmMask = maskBitmap;
    iconInfo.hbmColor = colorBitmap;

    HCURSOR result = CreateIconIndirect(&iconInfo);
    DeleteObject(maskBitmap);
    DeleteObject(colorBitmap);
    return result;
}

bool SetRenderedSystemCursor(const RenderedCursor& rendered, DWORD cursorId) {
    HCURSOR cursor = CreateCursorFromPixels(rendered);
    if (!cursor) {
        Wh_Log(L"CreateIconIndirect failed for cursor %lu", cursorId);
        return false;
    }

    // SetSystemCursor takes ownership of the cursor handle when successful.
    if (!SetSystemCursor(cursor, cursorId)) {
        Wh_Log(L"SetSystemCursor failed for cursor %lu: %lu", cursorId, GetLastError());
        DestroyCursor(cursor);
        return false;
    }

    HCURSOR installedCursor = static_cast<HCURSOR>(LoadImageW(
        nullptr, MAKEINTRESOURCEW(cursorId), IMAGE_CURSOR, 0, 0, LR_SHARED));
    const int roleIndex = CursorRoleIndex(cursorId);
    if (roleIndex >= 0) {
        g_expectedRoleHandles[static_cast<size_t>(roleIndex)] = installedCursor;
    }
    if (cursorId == CURSOR_NORMAL) {
        g_expectedNormalCursor = installedCursor;
    }

    return true;
}

int GetIntegerChoiceSetting(PCWSTR name, int fallback) {
    WindhawkUtils::StringSetting rawValue =
        WindhawkUtils::StringSetting::make(name);
    if (!*rawValue) {
        return fallback;
    }

    wchar_t* end = nullptr;
    const long parsed = wcstol(rawValue.get(), &end, 10);
    return end && *end == L'\0' ? static_cast<int>(parsed) : fallback;
}

int ResolveCursorSize() {
    const int configured = GetIntegerChoiceSetting(L"CursorSize", 0);
    if (configured > 0) {
        return std::clamp(configured, 16, 256);
    }

    // SetSystemCursor replaces a system cursor slot, so Automatic should follow
    // the slot's current nominal size instead of synthesizing another size from
    // DPI. GetSystemMetrics already reports the cursor metric Windows is using
    // for this session and also respects non-default system cursor sizing.
    const int width = GetSystemMetrics(SM_CXCURSOR);
    const int height = GetSystemMetrics(SM_CYCURSOR);
    const int desired = std::max(width, height);
    return std::clamp(desired > 0 ? desired : 32, 16, 256);
}

void LoadSettings() {
    g_fillOpacity = GetIntegerChoiceSetting(L"FillOpacity", 25);
    if (g_fillOpacity != 25 && g_fillOpacity != 50) {
        g_fillOpacity = 25;
    }

    g_glassRed = std::clamp(Wh_GetIntSetting(L"GlassColor.Red"), 0, 255);
    g_glassGreen = std::clamp(Wh_GetIntSetting(L"GlassColor.Green"), 0, 255);
    g_glassBlue = std::clamp(Wh_GetIntSetting(L"GlassColor.Blue"), 0, 255);

    g_cursorSize = ResolveCursorSize();

    WindhawkUtils::StringSetting glassStyle =
        WindhawkUtils::StringSetting::make(L"GlassStyle");
    if (wcscmp(glassStyle.get(), L"clear") == 0) {
        g_glassStyle = 0;
    } else if (wcscmp(glassStyle.get(), L"mica") == 0) {
        g_glassStyle = 2;
    } else if (wcscmp(glassStyle.get(), L"micaAlt") == 0) {
        g_glassStyle = 3;
    } else {
        g_glassStyle = 1;
    }

    g_blurStrength = std::clamp(Wh_GetIntSetting(L"BlurStrength"), 0, 24);
    const int configuredBlurScale = Wh_GetIntSetting(L"BlurScale");
    g_blurScale = configuredBlurScale > 0
        ? std::clamp(configuredBlurScale, 25, 300)
        : 100;

    WindhawkUtils::StringSetting pointerStyle =
        WindhawkUtils::StringSetting::make(L"PointerStyle");
    if (wcscmp(pointerStyle.get(), L"cleanRounded") == 0) {
        g_pointerStyle = 1;
    } else if (wcscmp(pointerStyle.get(), L"symmetricSharp") == 0) {
        g_pointerStyle = 2;
    } else if (wcscmp(pointerStyle.get(), L"symmetricCurved") == 0) {
        g_pointerStyle = 3;
    } else {
        g_pointerStyle = 0;
    }

    WindhawkUtils::StringSetting handStyle =
        WindhawkUtils::StringSetting::make(L"HandStyle");
    g_handStyle = wcscmp(handStyle.get(), L"clean") == 0 ? 0 : 1;

    g_artworkScale = GetIntegerChoiceSetting(L"ArtworkScale", 68);
    if (g_artworkScale != 65 && g_artworkScale != 68 &&
        g_artworkScale != 75 && g_artworkScale != 82 &&
        g_artworkScale != 90 && g_artworkScale != 100) {
        g_artworkScale = 68;
    }

    Wh_Log(L"size=%d fill=%d%% style=%d blur=%d scale=%d%% color=%d,%d,%d pointerStyle=%d handStyle=%d artwork=%d%%",
           g_cursorSize, g_fillOpacity, g_glassStyle, g_blurStrength,
           g_blurScale, g_glassRed, g_glassGreen, g_glassBlue,
           g_pointerStyle, g_handStyle, g_artworkScale);
}


struct DynamicMaterialProfile {
    double blurScale;
    double tintBase;
    double tintFillScale;
    double luminosityOpacity;
    double exclusionOpacity;
    double noiseStrength;
};

bool IsSampledGlassStyle() {
    return g_glassStyle >= 1 && g_glassStyle <= 3;
}

DynamicMaterialProfile GetDynamicMaterialProfile() {
    switch (g_glassStyle) {
        case 2: // Mica: restrained, strongly tint-dominant surface.
            return {0.72, 0.72, 0.22, 0.58, 0.00, 0.00};
        case 3: // Mica Alt: same family with stronger tinting.
            return {0.72, 0.84, 0.20, 0.68, 0.00, 0.00};
        case 1: // Acrylic: blur + luminosity/exclusion + tint + fine noise.
        default:
            return {1.00, 0.12, 0.62, 0.24, 0.12, 0.018};
    }
}

int ResolveDynamicBlurRadius(const RenderedCursor& cursor) {
    if (g_blurStrength <= 0) {
        return 0;
    }

    const DynamicMaterialProfile profile = GetDynamicMaterialProfile();
    // Blur should scale with the visible artwork, not transparent canvas
    // padding. Otherwise shrinking ArtworkScale makes the material progressively
    // blurrier relative to the cursor itself.
    const double resolutionScale =
        (cursor.size * cursor.artworkScale) / 48.0;
    const double userScale = g_blurScale / 100.0;
    const int radius = static_cast<int>(std::lround(
        g_blurStrength * resolutionScale * profile.blurScale * userScale));
    return std::clamp(radius, 1, 48);
}

void RefreshMaterialEnvironment() {
    const DWORD now = GetTickCount();
    if (g_dynamicGlass.lastEnvironmentQueryTick &&
        now - g_dynamicGlass.lastEnvironmentQueryTick < 2000) {
        return;
    }
    g_dynamicGlass.lastEnvironmentQueryTick = now;

    HIGHCONTRASTW highContrast = {};
    highContrast.cbSize = sizeof(highContrast);
    g_dynamicGlass.highContrast =
        SystemParametersInfoW(SPI_GETHIGHCONTRAST, sizeof(highContrast),
                              &highContrast, 0) &&
        (highContrast.dwFlags & HCF_HIGHCONTRASTON);

    SYSTEM_POWER_STATUS power = {};
    g_dynamicGlass.batterySaver =
        GetSystemPowerStatus(&power) && power.SystemStatusFlag != 0;
}

bool DynamicMaterialsAllowed() {
    RefreshMaterialEnvironment();
    return !g_dynamicGlass.highContrast;
}

void RefreshVirtualDesktopRect() {
    if (g_dynamicGlass.virtualDesktopValid) {
        return;
    }

    const int left = GetSystemMetrics(SM_XVIRTUALSCREEN);
    const int top = GetSystemMetrics(SM_YVIRTUALSCREEN);
    g_dynamicGlass.virtualDesktop = {
        left,
        top,
        left + GetSystemMetrics(SM_CXVIRTUALSCREEN),
        top + GetSystemMetrics(SM_CYVIRTUALSCREEN),
    };
    g_dynamicGlass.virtualDesktopValid = true;
}

DWORD ResolveDynamicCaptureInterval(const POINT& cursorPos) {
    RefreshMaterialEnvironment();

    const HMONITOR monitor = MonitorFromPoint(cursorPos, MONITOR_DEFAULTTONEAREST);
    const DWORD now = GetTickCount();
    const bool refreshCache = monitor != g_dynamicRefreshMonitor ||
                              now - g_dynamicRefreshLastQueryTick >= 2000;
    if (refreshCache) {
        g_dynamicRefreshMonitor = monitor;
        g_dynamicRefreshLastQueryTick = now;

        MONITORINFOEXW monitorInfo = {};
        monitorInfo.cbSize = sizeof(monitorInfo);
        DEVMODEW mode = {};
        mode.dmSize = sizeof(mode);

        UINT refreshRate = 60;
        if (monitor && GetMonitorInfoW(monitor, &monitorInfo) &&
            EnumDisplaySettingsW(monitorInfo.szDevice, ENUM_CURRENT_SETTINGS, &mode) &&
            mode.dmDisplayFrequency > 1) {
            refreshRate = mode.dmDisplayFrequency;
        }

        // Keep the lightweight worker wake cadence synchronized with the actual
        // monitor refresh rate. Expensive backdrop captures are throttled
        // separately when the pointer is stationary, so backing off here would
        // make the worker blind to newly-started cursor movement for 33-66 ms.
        const UINT maxRate = g_dynamicGlass.batterySaver
            ? std::min<UINT>(60, kMaxDynamicRefreshRate)
            : kMaxDynamicRefreshRate;
        refreshRate = std::clamp<UINT>(refreshRate, 24, maxRate);
        g_dynamicCaptureInterval = std::max<DWORD>(
            1, static_cast<DWORD>(std::lround(1000.0 / refreshRate)));
    }

    return g_dynamicCaptureInterval;
}

DWORD ResolveStationaryBackdropCaptureInterval() {
    DWORD interval = g_dynamicCaptureInterval;
    if (g_dynamicGlass.unchangedCaptureCount >= 40) {
        interval = std::max<DWORD>(interval, 66);
    } else if (g_dynamicGlass.unchangedCaptureCount >= 12) {
        interval = std::max<DWORD>(interval, 33);
    } else if (g_dynamicGlass.unchangedCaptureCount >= 4) {
        interval = std::max<DWORD>(interval, 16);
    }
    return interval;
}

bool StopRequested() {
    return g_animationStopEvent &&
           WaitForSingleObject(g_animationStopEvent, 0) == WAIT_OBJECT_0;
}

void ResetDynamicGlassState(bool releaseCaptureSurface = false) {
    g_dynamicGlass.lastAppliedPixels.clear();
    g_dynamicGlass.blurScratch.clear();
    g_dynamicGlass.blurredPixels.clear();
    g_dynamicGlass.workingPixels.clear();
    g_dynamicGlass.lastRole = 0;
    g_dynamicGlass.lastAppliedPosition = {};
    g_dynamicGlass.hasLastAppliedPosition = false;
    g_dynamicGlass.lastBackdropCaptureTick = 0;
    g_dynamicGlass.virtualDesktop = {};
    g_dynamicGlass.virtualDesktopValid = false;
    g_dynamicGlass.lastEnvironmentQueryTick = 0;
    g_dynamicGlass.highContrast = false;
    g_dynamicGlass.batterySaver = false;
    g_dynamicGlass.unchangedCaptureCount = 0;
    g_dynamicGlass.width = 0;
    g_dynamicGlass.height = 0;
    g_dynamicGlass.padding = 0;
    g_dynamicGlass.blurRadius = 0;
    g_dynamicRefreshMonitor = nullptr;
    g_dynamicCaptureInterval = 16;
    g_dynamicRefreshLastQueryTick = 0;

    if (!releaseCaptureSurface) {
        return;
    }

    DestroyDynamicCursorFactory();

    DynamicCaptureSurface& capture = g_dynamicGlass.capture;
    if (capture.memoryDc && capture.oldBitmap) {
        SelectObject(capture.memoryDc, capture.oldBitmap);
        capture.oldBitmap = nullptr;
    }
    if (capture.bitmap) {
        DeleteObject(capture.bitmap);
        capture.bitmap = nullptr;
    }
    if (capture.memoryDc) {
        DeleteDC(capture.memoryDc);
        capture.memoryDc = nullptr;
    }
    if (capture.screenDc) {
        ReleaseDC(nullptr, capture.screenDc);
        capture.screenDc = nullptr;
    }
    capture.bits = nullptr;
    capture.width = 0;
    capture.height = 0;
}

bool EnsureDynamicCaptureSurface(int width, int height) {
    DynamicCaptureSurface& capture = g_dynamicGlass.capture;
    if (capture.bitmap && capture.bits && capture.width == width &&
        capture.height == height) {
        return true;
    }

    if (!capture.screenDc) {
        capture.screenDc = GetDC(nullptr);
        if (!capture.screenDc) {
            return false;
        }
    }

    if (!capture.memoryDc) {
        capture.memoryDc = CreateCompatibleDC(capture.screenDc);
        if (!capture.memoryDc) {
            ReleaseDC(nullptr, capture.screenDc);
            capture.screenDc = nullptr;
            return false;
        }
    }

    if (capture.bitmap) {
        if (capture.oldBitmap) {
            SelectObject(capture.memoryDc, capture.oldBitmap);
            capture.oldBitmap = nullptr;
        }
        DeleteObject(capture.bitmap);
        capture.bitmap = nullptr;
        capture.bits = nullptr;
    }

    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void* bits = nullptr;
    capture.bitmap = CreateDIBSection(capture.screenDc, &bmi, DIB_RGB_COLORS,
                                      &bits, nullptr, 0);
    if (!capture.bitmap || !bits) {
        if (capture.bitmap) {
            DeleteObject(capture.bitmap);
            capture.bitmap = nullptr;
        }
        return false;
    }

    capture.oldBitmap = SelectObject(capture.memoryDc, capture.bitmap);
    if (!capture.oldBitmap || capture.oldBitmap == HGDI_ERROR) {
        DeleteObject(capture.bitmap);
        capture.bitmap = nullptr;
        capture.bits = nullptr;
        capture.oldBitmap = nullptr;
        return false;
    }

    capture.bits = static_cast<uint32_t*>(bits);
    capture.width = width;
    capture.height = height;
    return true;
}

bool CaptureLiveBackdrop(const RenderedCursor& cursor, const POINT& cursorPos) {
    const int blurRadius = ResolveDynamicBlurRadius(cursor);
    const int padding = std::max(4, blurRadius * 2 + 2);
    const int width = cursor.size + padding * 2;
    const int height = cursor.size + padding * 2;

    if (!EnsureDynamicCaptureSurface(width, height)) {
        return false;
    }

    RefreshVirtualDesktopRect();
    const RECT& virtualDesktop = g_dynamicGlass.virtualDesktop;
    DynamicCaptureSurface& capture = g_dynamicGlass.capture;
    std::fill(capture.bits,
              capture.bits + static_cast<size_t>(width) * height, 0u);

    // POINT/RECT coordinates are LONG on Win32. Keep all screen-coordinate
    // calculations in LONG so std::min/std::max don't receive mixed int/LONG
    // arguments under Windhawk's libc++ toolchain.
    const LONG requestedLeft =
        cursorPos.x - static_cast<LONG>(cursor.hotspotX) - static_cast<LONG>(padding);
    const LONG requestedTop =
        cursorPos.y - static_cast<LONG>(cursor.hotspotY) - static_cast<LONG>(padding);

    const LONG sourceLeft = std::max(requestedLeft, virtualDesktop.left);
    const LONG sourceTop = std::max(requestedTop, virtualDesktop.top);
    const LONG sourceRight = std::min(
        requestedLeft + static_cast<LONG>(width), virtualDesktop.right);
    const LONG sourceBottom = std::min(
        requestedTop + static_cast<LONG>(height), virtualDesktop.bottom);
    const LONG copyWidthLong = sourceRight - sourceLeft;
    const LONG copyHeightLong = sourceBottom - sourceTop;
    if (copyWidthLong <= 0 || copyHeightLong <= 0) {
        return false;
    }

    const int copyWidth = static_cast<int>(copyWidthLong);
    const int copyHeight = static_cast<int>(copyHeightLong);
    const int destX = static_cast<int>(sourceLeft - requestedLeft);
    const int destY = static_cast<int>(sourceTop - requestedTop);
    if (!BitBlt(capture.memoryDc, destX, destY, copyWidth, copyHeight,
                capture.screenDc, sourceLeft, sourceTop,
                SRCCOPY | CAPTUREBLT)) {
        return false;
    }

    const int validLeft = destX;
    const int validTop = destY;
    const int validRight = destX + copyWidth - 1;
    const int validBottom = destY + copyHeight - 1;
    for (int y = 0; y < height; ++y) {
        const int sampleY = std::clamp(y, validTop, validBottom);
        for (int x = 0; x < width; ++x) {
            if (x >= validLeft && x <= validRight &&
                y >= validTop && y <= validBottom) {
                continue;
            }
            const int sampleX = std::clamp(x, validLeft, validRight);
            capture.bits[static_cast<size_t>(y) * width + x] =
                capture.bits[static_cast<size_t>(sampleY) * width + sampleX];
        }
    }

    g_dynamicGlass.width = width;
    g_dynamicGlass.height = height;
    g_dynamicGlass.padding = padding;
    g_dynamicGlass.blurRadius = blurRadius;
    return true;
}

bool CaptureDynamicBackdrop(const RenderedCursor& cursor,
                            const POINT& cursorPos) {
    return CaptureLiveBackdrop(cursor, cursorPos);
}

bool DynamicBackdropChanged(DWORD role, const POINT& cursorPos) {
    const DynamicCaptureSurface& capture = g_dynamicGlass.capture;
    const size_t pixelCount =
        static_cast<size_t>(g_dynamicGlass.width) * g_dynamicGlass.height;
    const std::vector<uint32_t>& previous = g_dynamicGlass.lastAppliedPixels;
    if (role != g_dynamicGlass.lastRole || previous.size() != pixelCount ||
        !pixelCount || !capture.bits || !g_dynamicGlass.hasLastAppliedPosition) {
        return true;
    }

    const int movement =
        std::abs(cursorPos.x - g_dynamicGlass.lastAppliedPosition.x) +
        std::abs(cursorPos.y - g_dynamicGlass.lastAppliedPosition.y);

    // Cursor movement must regenerate the material even on a uniform backdrop
    // because Acrylic's noise lives in screen space. This also removes the old
    // one-pixel "sticky" case.
    if (movement > 0) {
        return true;
    }


    uint64_t absoluteDifference = 0;
    size_t materiallyChanged = 0;
    size_t samples = 0;
    const int width = g_dynamicGlass.width;
    const int height = g_dynamicGlass.height;

    for (int y = 0; y < height; y += 2) {
        for (int x = 0; x < width; x += 2) {
            const size_t index = static_cast<size_t>(y) * width + x;
            const uint32_t a = capture.bits[index];
            const uint32_t b = previous[index];
            const int db = std::abs(static_cast<int>(a & 0xFF) -
                                    static_cast<int>(b & 0xFF));
            const int dg = std::abs(static_cast<int>((a >> 8) & 0xFF) -
                                    static_cast<int>((b >> 8) & 0xFF));
            const int dr = std::abs(static_cast<int>((a >> 16) & 0xFF) -
                                    static_cast<int>((b >> 16) & 0xFF));
            const int delta = dr + dg + db;
            absoluteDifference += static_cast<uint64_t>(delta);
            if (delta >= 24) {
                ++materiallyChanged;
            }
            ++samples;
        }
    }

    if (!samples) {
        return false;
    }

    const double meanChannelDelta =
        absoluteDifference / static_cast<double>(samples * 3);
    const double changedRatio = materiallyChanged / static_cast<double>(samples);
    return meanChannelDelta >= 1.25 || changedRatio >= 0.015;
}

void BoxBlurHorizontal(const uint32_t* source,
                       std::vector<uint32_t>& destination,
                       int width,
                       int height,
                       int radius) {
    const size_t pixelCount = static_cast<size_t>(width) * height;
    destination.resize(pixelCount);
    if (!source || !pixelCount) {
        return;
    }

    for (int y = 0; y < height; ++y) {
        int64_t sumR = 0, sumG = 0, sumB = 0;
        int count = 0;

        for (int x = -radius; x <= radius; ++x) {
            const int sampleX = std::clamp(x, 0, width - 1);
            const uint32_t pixel = source[static_cast<size_t>(y) * width + sampleX];
            sumB += pixel & 0xFF;
            sumG += (pixel >> 8) & 0xFF;
            sumR += (pixel >> 16) & 0xFF;
            ++count;
        }

        for (int x = 0; x < width; ++x) {
            destination[static_cast<size_t>(y) * width + x] =
                (static_cast<uint32_t>(sumR / count) << 16) |
                (static_cast<uint32_t>(sumG / count) << 8) |
                static_cast<uint32_t>(sumB / count);

            const int removeX = std::clamp(x - radius, 0, width - 1);
            const int addX = std::clamp(x + radius + 1, 0, width - 1);
            const uint32_t removePixel =
                source[static_cast<size_t>(y) * width + removeX];
            const uint32_t addPixel =
                source[static_cast<size_t>(y) * width + addX];
            sumB += static_cast<int>(addPixel & 0xFF) -
                    static_cast<int>(removePixel & 0xFF);
            sumG += static_cast<int>((addPixel >> 8) & 0xFF) -
                    static_cast<int>((removePixel >> 8) & 0xFF);
            sumR += static_cast<int>((addPixel >> 16) & 0xFF) -
                    static_cast<int>((removePixel >> 16) & 0xFF);
        }
    }
}

void BoxBlurVertical(const uint32_t* source,
                     std::vector<uint32_t>& destination,
                     int width,
                     int height,
                     int radius) {
    const size_t pixelCount = static_cast<size_t>(width) * height;
    destination.resize(pixelCount);
    if (!source || !pixelCount) {
        return;
    }

    for (int x = 0; x < width; ++x) {
        int64_t sumR = 0, sumG = 0, sumB = 0;
        int count = 0;

        for (int y = -radius; y <= radius; ++y) {
            const int sampleY = std::clamp(y, 0, height - 1);
            const uint32_t pixel = source[static_cast<size_t>(sampleY) * width + x];
            sumB += pixel & 0xFF;
            sumG += (pixel >> 8) & 0xFF;
            sumR += (pixel >> 16) & 0xFF;
            ++count;
        }

        for (int y = 0; y < height; ++y) {
            destination[static_cast<size_t>(y) * width + x] =
                (static_cast<uint32_t>(sumR / count) << 16) |
                (static_cast<uint32_t>(sumG / count) << 8) |
                static_cast<uint32_t>(sumB / count);

            const int removeY = std::clamp(y - radius, 0, height - 1);
            const int addY = std::clamp(y + radius + 1, 0, height - 1);
            const uint32_t removePixel =
                source[static_cast<size_t>(removeY) * width + x];
            const uint32_t addPixel =
                source[static_cast<size_t>(addY) * width + x];
            sumB += static_cast<int>(addPixel & 0xFF) -
                    static_cast<int>(removePixel & 0xFF);
            sumG += static_cast<int>((addPixel >> 8) & 0xFF) -
                    static_cast<int>((removePixel >> 8) & 0xFF);
            sumR += static_cast<int>((addPixel >> 16) & 0xFF) -
                    static_cast<int>((removePixel >> 16) & 0xFF);
        }
    }
}

void BlurDynamicBackdrop() {
    const int radius = g_dynamicGlass.blurRadius;
    const int width = g_dynamicGlass.width;
    const int height = g_dynamicGlass.height;
    const DynamicCaptureSurface& capture = g_dynamicGlass.capture;
    const size_t pixelCount = static_cast<size_t>(width) * height;

    if (!capture.bits || !pixelCount) {
        g_dynamicGlass.blurredPixels.clear();
        return;
    }

    if (radius <= 0) {
        g_dynamicGlass.blurredPixels.assign(capture.bits,
                                            capture.bits + pixelCount);
        return;
    }

    BoxBlurHorizontal(capture.bits, g_dynamicGlass.blurScratch,
                      width, height, radius);
    BoxBlurVertical(g_dynamicGlass.blurScratch.data(),
                    g_dynamicGlass.blurredPixels,
                    width, height, radius);

    const int secondRadius = std::max(1, radius / 2);
    BoxBlurHorizontal(g_dynamicGlass.blurredPixels.data(),
                      g_dynamicGlass.blurScratch,
                      width, height, secondRadius);
    BoxBlurVertical(g_dynamicGlass.blurScratch.data(),
                    g_dynamicGlass.blurredPixels,
                    width, height, secondRadius);
}

void ApplyDynamicGlassBackdrop(const RenderedCursor& cursor,
                               const POINT& screenPos,
                               std::vector<uint32_t>& outputPixels) {
    if (cursor.glassMask.size() != cursor.pixels.size() ||
        cursor.protectedMask.size() != cursor.pixels.size() ||
        g_dynamicGlass.blurredPixels.empty()) {
        return;
    }

    outputPixels = cursor.pixels;

    const DynamicMaterialProfile profile = GetDynamicMaterialProfile();
    const int width = g_dynamicGlass.width;
    const int padding = g_dynamicGlass.padding;
    const double fill = g_fillOpacity / 100.0;
    const double tintAmount = Clamp01(profile.tintBase + fill * profile.tintFillScale);
    const double tintR = g_glassRed / 255.0;
    const double tintG = g_glassGreen / 255.0;
    const double tintB = g_glassBlue / 255.0;
    const double tintLum = 0.2126 * tintR + 0.7152 * tintG + 0.0722 * tintB;

    for (int y = 0; y < cursor.size; ++y) {
        for (int x = 0; x < cursor.size; ++x) {
            const size_t cursorIndex = static_cast<size_t>(y) * cursor.size + x;
            const double mask = cursor.glassMask[cursorIndex] / 255.0;
            const double protectedCoverage =
                cursor.protectedMask[cursorIndex] / 255.0;
            if (mask <= 0.01 || protectedCoverage > 0.01) {
                continue;
            }

            const uint32_t original = cursor.pixels[cursorIndex];
            const int originalA = (original >> 24) & 0xFF;

            const size_t backdropIndex =
                static_cast<size_t>(y + padding) * width + (x + padding);
            const uint32_t backdrop = g_dynamicGlass.blurredPixels[backdropIndex];
            double b = (backdrop & 0xFF) / 255.0;
            double g = ((backdrop >> 8) & 0xFF) / 255.0;
            double r = ((backdrop >> 16) & 0xFF) / 255.0;

            const double sourceLum = 0.2126 * r + 0.7152 * g + 0.0722 * b;

            if (g_glassStyle == 1) {
                const double exclusionGray = tintLum;
                const double exR = r + exclusionGray - 2.0 * r * exclusionGray;
                const double exG = g + exclusionGray - 2.0 * g * exclusionGray;
                const double exB = b + exclusionGray - 2.0 * b * exclusionGray;
                r = r * (1.0 - profile.exclusionOpacity) +
                    exR * profile.exclusionOpacity;
                g = g * (1.0 - profile.exclusionOpacity) +
                    exG * profile.exclusionOpacity;
                b = b * (1.0 - profile.exclusionOpacity) +
                    exB * profile.exclusionOpacity;

                const double currentLum = 0.2126 * r + 0.7152 * g + 0.0722 * b;
                const double targetLum =
                    currentLum * (1.0 - profile.luminosityOpacity) +
                    tintLum * profile.luminosityOpacity;
                const double lumDelta = targetLum - currentLum;
                r = Clamp01(r + lumDelta);
                g = Clamp01(g + lumDelta);
                b = Clamp01(b + lumDelta);

                r = r * (1.0 - tintAmount) + tintR * tintAmount;
                g = g * (1.0 - tintAmount) + tintG * tintAmount;
                b = b * (1.0 - tintAmount) + tintB * tintAmount;

                // Additive grayscale noise behaves more like a composited fine
                // noise texture than the old luminance-dependent multiplier.
                const double noise = HashNoise01(
                    screenPos.x - cursor.hotspotX + x,
                    screenPos.y - cursor.hotspotY + y) - 0.5;
                const double noiseDelta = noise * profile.noiseStrength;
                r = Clamp01(r + noiseDelta);
                g = Clamp01(g + noiseDelta);
                b = Clamp01(b + noiseDelta);
            } else {
                const double targetLum =
                    sourceLum * (1.0 - profile.luminosityOpacity) +
                    tintLum * profile.luminosityOpacity;
                const double lumDelta = targetLum - sourceLum;
                r = Clamp01(r + lumDelta);
                g = Clamp01(g + lumDelta);
                b = Clamp01(b + lumDelta);

                r = r * (1.0 - tintAmount) + tintR * tintAmount;
                g = g * (1.0 - tintAmount) + tintG * tintAmount;
                b = b * (1.0 - tintAmount) + tintB * tintAmount;
            }

            // Sampled styles precompose their backdrop into RGB. Keep material
            // coverage opaque enough that Windows doesn't blend the real desktop
            // through it a second time. FillOpacity therefore controls material
            // density/tint for sampled styles rather than literal output alpha.
            const int outA = std::clamp(
                static_cast<int>(std::lround(mask * 255.0)), originalA, 255);
            const int outR = static_cast<int>(std::lround(Clamp01(r) * 255.0));
            const int outG = static_cast<int>(std::lround(Clamp01(g) * 255.0));
            const int outB = static_cast<int>(std::lround(Clamp01(b) * 255.0));
            outputPixels[cursorIndex] =
                (static_cast<uint32_t>(outA) << 24) |
                (static_cast<uint32_t>(outR) << 16) |
                (static_cast<uint32_t>(outG) << 8) |
                static_cast<uint32_t>(outB);
        }
    }
}

bool CursorInfoIsVisible(const CURSORINFO& cursorInfo) {
    // CURSOR_SUPPRESSED is used when Windows deliberately stops drawing the
    // pointer (for example during touch/pen input). Treat it the same as hidden
    // even if another flag is also present.
    return (cursorInfo.flags & CURSOR_SHOWING) != 0 &&
           (cursorInfo.flags & CURSOR_SUPPRESSED) == 0 &&
           cursorInfo.hCursor != nullptr;
}

bool IsForegroundWindowFullscreen() {
    HWND foreground = GetForegroundWindow();
    if (!foreground) {
        return false;
    }

    foreground = GetAncestor(foreground, GA_ROOT);
    if (!foreground || foreground == g_cursorWindow.load() ||
        foreground == GetShellWindow() || foreground == GetDesktopWindow() ||
        !IsWindowVisible(foreground) || IsIconic(foreground)) {
        return false;
    }

    RECT windowRect = {};
    if (!GetWindowRect(foreground, &windowRect)) {
        return false;
    }

    HMONITOR monitor =
        MonitorFromWindow(foreground, MONITOR_DEFAULTTONULL);
    if (!monitor) {
        return false;
    }

    MONITORINFO monitorInfo = {sizeof(monitorInfo)};
    if (!GetMonitorInfoW(monitor, &monitorInfo)) {
        return false;
    }

    // GetWindowRect can include a small invisible resize border. A tiny
    // tolerance accepts borderless/exclusive fullscreen windows without
    // mistaking ordinary maximized windows that stop at the work area for
    // fullscreen applications.
    constexpr LONG kFullscreenTolerance = 2;
    const RECT& monitorRect = monitorInfo.rcMonitor;
    return windowRect.left <= monitorRect.left + kFullscreenTolerance &&
           windowRect.top <= monitorRect.top + kFullscreenTolerance &&
           windowRect.right >= monitorRect.right - kFullscreenTolerance &&
           windowRect.bottom >= monitorRect.bottom - kFullscreenTolerance;
}

bool ShouldDeferCursorReapply() {
    if (IsForegroundWindowFullscreen()) {
        return true;
    }

    CURSORINFO cursorInfo = {sizeof(cursorInfo)};
    return GetCursorInfo(&cursorInfo) && !CursorInfoIsVisible(cursorInfo);
}

void DeferCursorReapply(bool forceSystemReapply) {
    g_cursorReapplyDeferred.store(true);
    if (forceSystemReapply) {
        g_forceCursorReapplyDeferred.store(true);
    }
}

DWORD CursorRoleFromHandle(HCURSOR cursor) {
    if (!cursor) {
        return 0;
    }
    for (size_t i = 0; i < kCursorRoles.size(); ++i) {
        if (g_expectedRoleHandles[i] &&
            cursor == g_expectedRoleHandles[i]) {
            return kCursorRoles[i];
        }
    }
    return 0;
}

DWORD GetVisibleSystemCursorRole() {
    CURSORINFO cursorInfo = {sizeof(cursorInfo)};
    if (!GetCursorInfo(&cursorInfo) || !CursorInfoIsVisible(cursorInfo)) {
        return 0;
    }
    return CursorRoleFromHandle(cursorInfo.hCursor);
}

bool UpdateDynamicGlassCursor(DWORD role, const POINT& cursorPos) {
    if (!IsSampledGlassStyle() || !DynamicMaterialsAllowed() ||
        StopRequested() || !role || role == CURSOR_WAIT ||
        role == CURSOR_APPSTARTING) {
        return false;
    }

    const RenderedCursor* cached = GetCachedRenderedCursor(role);
    if (!cached) {
        return false;
    }

    const DWORD now = GetTickCount();
    const bool roleChanged = !g_dynamicGlass.hasLastAppliedPosition ||
                             g_dynamicGlass.lastRole != role;
    const bool moved = roleChanged ||
                       cursorPos.x != g_dynamicGlass.lastAppliedPosition.x ||
                       cursorPos.y != g_dynamicGlass.lastAppliedPosition.y;

    if (moved) {
        // Movement is always latency-sensitive. Never let a previous stationary
        // backoff delay the first frame after the pointer starts moving again.
        g_dynamicGlass.unchangedCaptureCount = 0;
    } else {
        // Keep polling cursor position at monitor cadence, but throttle the
        // expensive BitBlt/compare path after repeated unchanged stationary
        // captures. This preserves instant movement response without wasting
        // capture/blur work on a static desktop.
        const DWORD stationaryInterval =
            ResolveStationaryBackdropCaptureInterval();
        if (g_dynamicGlass.lastBackdropCaptureTick &&
            now - g_dynamicGlass.lastBackdropCaptureTick < stationaryInterval) {
            return false;
        }
    }

    g_dynamicGlass.lastBackdropCaptureTick = now;
    if (!CaptureDynamicBackdrop(*cached, cursorPos)) {
        return false;
    }

    if (!DynamicBackdropChanged(role, cursorPos)) {
        ++g_dynamicGlass.unchangedCaptureCount;
        return false;
    }
    g_dynamicGlass.unchangedCaptureCount = 0;

    BlurDynamicBackdrop();
    ApplyDynamicGlassBackdrop(*cached, cursorPos, g_dynamicGlass.workingPixels);

    HCURSOR cursor =
        CreateDynamicCursorFromPixels(*cached, g_dynamicGlass.workingPixels);
    if (!cursor) {
        Wh_Log(L"CreateIconIndirect failed for dynamic cursor %lu", role);
        return false;
    }

    // Capture/material work takes long enough for an application to hide or
    // replace its cursor between the initial snapshot and installation. Recheck
    // at the last possible moment so a stale dynamic frame cannot resurrect a
    // cursor that a fullscreen application just hid.
    CURSORINFO latestInfo = {sizeof(latestInfo)};
    if (!GetCursorInfo(&latestInfo) || !CursorInfoIsVisible(latestInfo) ||
        CursorRoleFromHandle(latestInfo.hCursor) != role) {
        DestroyCursor(cursor);
        return false;
    }

    if (!SetSystemCursor(cursor, role)) {
        Wh_Log(L"SetSystemCursor failed for dynamic cursor %lu: %lu",
               role, GetLastError());
        DestroyCursor(cursor);
        return false;
    }

    HCURSOR installedCursor = static_cast<HCURSOR>(LoadImageW(
        nullptr, MAKEINTRESOURCEW(role), IMAGE_CURSOR, 0, 0, LR_SHARED));
    const int roleIndex = CursorRoleIndex(role);
    if (roleIndex >= 0) {
        g_expectedRoleHandles[static_cast<size_t>(roleIndex)] = installedCursor;
    }
    if (role == CURSOR_NORMAL) {
        g_expectedNormalCursor = installedCursor;
    }

    const size_t pixelCount =
        static_cast<size_t>(g_dynamicGlass.width) * g_dynamicGlass.height;
    g_dynamicGlass.lastAppliedPixels.assign(
        g_dynamicGlass.capture.bits,
        g_dynamicGlass.capture.bits + pixelCount);
    g_dynamicGlass.lastRole = role;
    g_dynamicGlass.lastAppliedPosition = cursorPos;
    g_dynamicGlass.hasLastAppliedPosition = true;
    return true;
}

bool UpdateDynamicGlassCursor() {
    CURSORINFO cursorInfo = {sizeof(cursorInfo)};
    if (!GetCursorInfo(&cursorInfo) || !CursorInfoIsVisible(cursorInfo)) {
        return false;
    }

    const DWORD role = CursorRoleFromHandle(cursorInfo.hCursor);
    if (!role) {
        return false;
    }

    // CURSORINFO already includes the screen position from the same snapshot as
    // the visibility/handle state. Using it avoids a second cursor-state query
    // and prevents a hide/move race between GetCursorInfo and GetCursorPos.
    return UpdateDynamicGlassCursor(role, cursorInfo.ptScreenPos);
}

HWND FindMacMagnifyingCursorWindow() {
    return FindWindowW(L"WindhawkShakeCursorExclusiveOverlay", nullptr);
}

bool IsMacMagnifyingCursorActive(HWND overlay) {
    return overlay && IsWindow(overlay) && IsWindowVisible(overlay);
}

bool NormalCursorSlotChanged() {
    if (!g_expectedNormalCursor) {
        return false;
    }

    HCURSOR current = static_cast<HCURSOR>(LoadImageW(
        nullptr, MAKEINTRESOURCEW(CURSOR_NORMAL), IMAGE_CURSOR, 0, 0, LR_SHARED));
    return current && current != g_expectedNormalCursor;
}

constexpr std::array<DWORD, 14> kStaticCursorRoles = {
    CURSOR_NORMAL,
    CURSOR_IBEAM,
    CURSOR_CROSS,
    CURSOR_UP,
    CURSOR_SIZENWSE,
    CURSOR_SIZENESW,
    CURSOR_SIZEWE,
    CURSOR_SIZENS,
    CURSOR_SIZEALL,
    CURSOR_NO,
    CURSOR_HAND,
    CURSOR_HELP,
    CURSOR_PIN,
    CURSOR_PERSON,
};

bool StaticCursorCacheComplete() {
    for (DWORD role : kStaticCursorRoles) {
        if (!GetCachedRenderedCursor(role)) {
            return false;
        }
    }
    return true;
}

void ApplyStaticCursors(bool forceSystemReapply) {
    for (DWORD role : kStaticCursorRoles) {
        if (StopRequested()) {
            return;
        }

        const RenderedCursor* rendered = GetCachedRenderedCursor(role);
        const bool needsRerender = !rendered;
        if (needsRerender) {
            RenderedCursor fresh = RenderRole(g_cursorSize, role);
            CacheRenderedCursor(role, fresh);
            rendered = GetCachedRenderedCursor(role);
        }
        if (rendered && (needsRerender || forceSystemReapply)) {
            SetRenderedSystemCursor(*rendered, role);
        }
    }
}

void DestroyCursorFrameSet(std::vector<HCURSOR>& frames) {
    for (HCURSOR cursor : frames) {
        if (cursor) {
            DestroyCursor(cursor);
        }
    }
    frames.clear();
}

void DestroyAnimationFrames() {
    DestroyCursorFrameSet(g_busyFrames);
    DestroyCursorFrameSet(g_workingFrames);
}

bool SetPreparedSystemCursor(HCURSOR source, DWORD cursorId) {
    if (!source) {
        return false;
    }

    HCURSOR cursorCopy = static_cast<HCURSOR>(CopyIcon(source));
    if (!cursorCopy) {
        Wh_Log(L"CopyIcon failed for cursor %lu: %lu", cursorId, GetLastError());
        return false;
    }

    if (!SetSystemCursor(cursorCopy, cursorId)) {
        Wh_Log(L"SetSystemCursor failed for cursor %lu: %lu", cursorId, GetLastError());
        DestroyCursor(cursorCopy);
        return false;
    }

    const int roleIndex = CursorRoleIndex(cursorId);
    if (roleIndex >= 0) {
        g_expectedRoleHandles[static_cast<size_t>(roleIndex)] =
            static_cast<HCURSOR>(LoadImageW(
                nullptr, MAKEINTRESOURCEW(cursorId), IMAGE_CURSOR, 0, 0, LR_SHARED));
    }

    return true;
}

bool PrepareAnimationFrameSet(std::vector<HCURSOR>& frames, DWORD role) {
    DestroyCursorFrameSet(frames);
    frames.reserve(kSpinnerFrames);

    for (int frame = 0; frame < kSpinnerFrames; ++frame) {
        if (WaitForSingleObject(g_animationStopEvent, 0) == WAIT_OBJECT_0) {
            DestroyCursorFrameSet(frames);
            return false;
        }

        HCURSOR cursor =
            CreateCursorFromPixels(RenderRole(g_cursorSize, role, frame));
        if (!cursor) {
            Wh_Log(L"Failed to create animation frame %d for cursor %lu",
                   frame, role);
            DestroyCursorFrameSet(frames);
            return false;
        }
        frames.push_back(cursor);
    }

    return frames.size() == kSpinnerFrames;
}

bool PrepareAnimationFrames(bool rebuildBusy, bool rebuildWorking) {
    if (rebuildBusy && !PrepareAnimationFrameSet(g_busyFrames, CURSOR_WAIT)) {
        return false;
    }
    if (rebuildWorking &&
        !PrepareAnimationFrameSet(g_workingFrames, CURSOR_APPSTARTING)) {
        return false;
    }

    if (g_busyFrames.size() != kSpinnerFrames ||
        g_workingFrames.size() != kSpinnerFrames) {
        return false;
    }

    return SetPreparedSystemCursor(g_busyFrames.front(), CURSOR_WAIT) &&
           SetPreparedSystemCursor(g_workingFrames.front(),
                                   CURSOR_APPSTARTING);
}

struct SettingsSnapshot {
    int fillOpacity;
    int glassRed;
    int glassGreen;
    int glassBlue;
    int cursorSize;
    int pointerStyle;
    int handStyle;
    int artworkScale;
    int glassStyle;
    int blurStrength;
    int blurScale;
};

SettingsSnapshot CaptureSettingsSnapshot() {
    return {
        g_fillOpacity,
        g_glassRed,
        g_glassGreen,
        g_glassBlue,
        g_cursorSize,
        g_pointerStyle,
        g_handStyle,
        g_artworkScale,
        g_glassStyle,
        g_blurStrength,
        g_blurScale,
    };
}

bool GlobalArtworkChanged(const SettingsSnapshot& before,
                         const SettingsSnapshot& after) {
    const bool clearFamilyChanged =
        (before.glassStyle == 0) != (after.glassStyle == 0);
    return before.fillOpacity != after.fillOpacity ||
           before.glassRed != after.glassRed ||
           before.glassGreen != after.glassGreen ||
           before.glassBlue != after.glassBlue ||
           before.cursorSize != after.cursorSize ||
           before.artworkScale != after.artworkScale ||
           clearFamilyChanged;
}

void RebuildCursorSet(bool forceSystemReapply = true) {
    const SettingsSnapshot before = CaptureSettingsSnapshot();
    const bool hadStaticCache = StaticCursorCacheComplete();
    const bool hadBusyFrames = g_busyFrames.size() == kSpinnerFrames;
    const bool hadWorkingFrames = g_workingFrames.size() == kSpinnerFrames;

    LoadSettings();
    const SettingsSnapshot after = CaptureSettingsSnapshot();

    const bool globalArtworkChanged =
        !hadStaticCache || GlobalArtworkChanged(before, after);
    const bool pointerChanged = before.pointerStyle != after.pointerStyle;
    const bool handChanged = before.handStyle != after.handStyle;
    const bool sizeChanged = before.cursorSize != after.cursorSize;

    g_animationFramesReady.store(false);
    ResetDynamicGlassState(sizeChanged || !IsSampledGlassStyle());

    // Preserve expected cursor handles for material-only/settings-only rebuilds.
    // Clearing them without reinstalling every cached cursor makes role detection
    // fail, which silently disables Acrylic/Mica/Mica Alt dynamic updates.
    if (forceSystemReapply) {
        g_expectedRoleHandles.fill(nullptr);
    }

    if (globalArtworkChanged) {
        ClearRenderedCursorCache();
        ClearRoleScaleNormalization();
    } else {
        if (pointerChanged) {
            InvalidateRenderedCursor(CURSOR_NORMAL);
            InvalidateRenderedCursor(CURSOR_HELP);
            InvalidateRoleScaleNormalization(CURSOR_NORMAL);
            InvalidateRoleScaleNormalization(CURSOR_HELP);
            InvalidateRoleScaleNormalization(CURSOR_APPSTARTING);
        }
        if (handChanged) {
            InvalidateRenderedCursor(CURSOR_HAND);
        }
    }

    ApplyStaticCursors(forceSystemReapply);
    if (StopRequested()) {
        return;
    }

    const bool rebuildBusy = !hadBusyFrames || globalArtworkChanged;
    const bool rebuildWorking =
        !hadWorkingFrames || globalArtworkChanged || pointerChanged;

    bool framesReady = false;
    if (rebuildBusy || rebuildWorking) {
        framesReady = PrepareAnimationFrames(rebuildBusy, rebuildWorking);
    } else if (forceSystemReapply) {
        framesReady = SetPreparedSystemCursor(g_busyFrames.front(), CURSOR_WAIT) &&
                      SetPreparedSystemCursor(g_workingFrames.front(),
                                              CURSOR_APPSTARTING);
    } else {
        framesReady = hadBusyFrames && hadWorkingFrames;
    }

    g_animationFramesReady.store(framesReady);
    g_animationFrame = 0;

    if (!framesReady && !StopRequested()) {
        Wh_Log(L"Animation frames unavailable, static cursors only");
    }

    // Blur strength/scale and Acrylic<->Mica changes reuse
    // the rasterized geometry. Hand changes rerasterize only the hand; pointer
    // changes rerasterize pointer/help and the working-background frames.
    if (IsSampledGlassStyle() && DynamicMaterialsAllowed() && !StopRequested()) {
        UpdateDynamicGlassCursor();
    }
}

void QueueCursorReapply(HWND hWnd, bool forceSystemReapply = true) {
    if (forceSystemReapply) {
        g_forceSystemReapplyPending.store(true);
    }
    if (g_reapplyMessagePending.exchange(true)) {
        return;
    }

    if (!PostMessageW(hWnd, kReapplyCursorsMessage, 0, 0)) {
        g_reapplyMessagePending.store(false);
        g_forceSystemReapplyPending.store(false);
        Wh_Log(L"Failed to queue cursor reapply: %lu", GetLastError());
    }
}

LRESULT CALLBACK CursorWindowProc(HWND hWnd, UINT message, WPARAM wParam,
                                  LPARAM lParam) {
    switch (message) {
        case WM_SETTINGCHANGE: {
            const PCWSTR settingName =
                lParam ? reinterpret_cast<PCWSTR>(lParam) : nullptr;
            const bool materialSettingChanged =
                wParam == SPI_SETHIGHCONTRAST ||
                (settingName &&
                 (wcscmp(settingName, L"ImmersiveColorSet") == 0 ||
                  wcscmp(settingName, L"UserPreferencesMask") == 0));
            if (wParam == SPI_SETCURSORS || materialSettingChanged) {
                const bool forceSystemReapply = wParam == SPI_SETCURSORS;
                if (ShouldDeferCursorReapply()) {
                    DeferCursorReapply(forceSystemReapply);
                    return 0;
                }
                QueueCursorReapply(hWnd, forceSystemReapply);
                return 0;
            }
            break;
        }

        case WM_THEMECHANGED:
            if (ShouldDeferCursorReapply()) {
                DeferCursorReapply(false);
            } else {
                QueueCursorReapply(hWnd, false);
            }
            return 0;

        case WM_DISPLAYCHANGE:
            // Exclusive fullscreen transitions commonly generate a display-mode
            // change. Reinstalling global cursor slots at that exact moment can
            // undo the application's blank/hidden cursor. Defer until the
            // foreground is no longer fullscreen and the cursor is visible again.
            if (ShouldDeferCursorReapply()) {
                DeferCursorReapply(true);
            } else {
                QueueCursorReapply(hWnd, true);
            }
            return 0;

        case kReapplyCursorsMessage: {
            const bool forceSystemReapply =
                g_forceSystemReapplyPending.exchange(false);
            g_reapplyMessagePending.store(false);

            if (ShouldDeferCursorReapply()) {
                DeferCursorReapply(forceSystemReapply);
                return 0;
            }

            RebuildCursorSet(forceSystemReapply);
            return 0;
        }
    }

    return DefWindowProcW(hWnd, message, wParam, lParam);
}

DWORD WINAPI AnimationThreadProc(LPVOID) {
    HINSTANCE instance = GetModuleHandleW(nullptr);
    WNDCLASSW windowClass = {};
    windowClass.lpfnWndProc = CursorWindowProc;
    windowClass.hInstance = instance;
    windowClass.lpszClassName = kCursorWindowClassName;

    const ATOM classAtom = RegisterClassW(&windowClass);
    if (!classAtom) {
        Wh_Log(L"RegisterClass failed: %lu", GetLastError());
        g_animationThreadInitialized.store(false);
        SetEvent(g_animationReadyEvent);
        WaitForSingleObject(g_animationStopEvent, INFINITE);
        return 0;
    }

    // A hidden top-level window is used instead of HWND_MESSAGE because
    // WM_SETTINGCHANGE is broadcast only to top-level windows.
    HWND window = CreateWindowExW(
        WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        kCursorWindowClassName,
        L"",
        WS_POPUP,
        0,
        0,
        0,
        0,
        nullptr,
        nullptr,
        instance,
        nullptr);

    if (!window) {
        Wh_Log(L"CreateWindowEx failed: %lu", GetLastError());
        g_animationThreadInitialized.store(false);
        SetEvent(g_animationReadyEvent);
        WaitForSingleObject(g_animationStopEvent, INFINITE);
        UnregisterClassW(kCursorWindowClassName, instance);
        return 0;
    }

    g_cursorWindow.store(window);
    g_animationThreadInitialized.store(true);
    SetEvent(g_animationReadyEvent);

    // All rendering happens on the worker thread. Initialization only waits for
    // the worker window to exist, so expensive supersampled rendering doesn't
    // block WhTool_ModInit.
    RebuildCursorSet();

    HWND magnifierWindow = nullptr;
    DWORD lastMagnifierDiscoveryTick = 0;
    DWORD lastCursorIntegrityCheckTick = 0;
    bool fullscreenWasActive = false;
    bool cursorWasHidden = false;

    for (;;) {
        const DWORD now = GetTickCount();
        if (!magnifierWindow || !IsWindow(magnifierWindow)) {
            if (!lastMagnifierDiscoveryTick ||
                now - lastMagnifierDiscoveryTick >= 500) {
                magnifierWindow = FindMacMagnifyingCursorWindow();
                lastMagnifierDiscoveryTick = now;
            }
        }

        const bool magnifierPresent =
            magnifierWindow && IsWindow(magnifierWindow);
        const bool magnifierActive =
            IsMacMagnifyingCursorActive(magnifierWindow);
        if (magnifierActive) {
            g_magnifierWasActive = true;
        }

        const bool fullscreenActive = IsForegroundWindowFullscreen();
        if (fullscreenActive) {
            fullscreenWasActive = true;
        }

        CURSORINFO cursorInfo = {sizeof(cursorInfo)};
        const bool haveCursorInfo =
            !magnifierActive && GetCursorInfo(&cursorInfo);
        const bool cursorVisible =
            haveCursorInfo && CursorInfoIsVisible(cursorInfo);
        const DWORD visibleRole = cursorVisible
            ? CursorRoleFromHandle(cursorInfo.hCursor)
            : 0;
        const DWORD visibleBusyRole =
            g_animationFramesReady.load() &&
            (visibleRole == CURSOR_WAIT || visibleRole == CURSOR_APPSTARTING)
                ? visibleRole
                : 0;

        DWORD delay = kIdlePollInterval;
        if (magnifierActive) {
            delay = kMagnifierPollInterval;
        } else if (visibleBusyRole) {
            delay = kAnimationDelay;
        } else if (!cursorVisible) {
            // A hidden cursor needs no material work, but keep a cheap 20 Hz
            // visibility poll so the glass cursor resumes promptly when an
            // application shows it again or exits fullscreen.
            delay = kHiddenCursorPollInterval;
        } else if (IsSampledGlassStyle() && DynamicMaterialsAllowed() &&
                   cursorVisible) {
            delay = ResolveDynamicCaptureInterval(cursorInfo.ptScreenPos);
        } else if (magnifierPresent) {
            delay = kMagnifierPollInterval;
        }

        const DWORD waitResult = MsgWaitForMultipleObjects(
            1,
            &g_animationStopEvent,
            FALSE,
            delay,
            QS_ALLINPUT);

        if (waitResult == WAIT_OBJECT_0) {
            break;
        }

        if (waitResult == WAIT_OBJECT_0 + 1) {
            MSG msg;
            while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            }
            continue;
        }

        if (waitResult != WAIT_TIMEOUT) {
            continue;
        }

        const DWORD afterWait = GetTickCount();
        if (!magnifierWindow || !IsWindow(magnifierWindow)) {
            if (!lastMagnifierDiscoveryTick ||
                afterWait - lastMagnifierDiscoveryTick >= 500) {
                magnifierWindow = FindMacMagnifyingCursorWindow();
                lastMagnifierDiscoveryTick = afterWait;
            }
        }

        if (IsMacMagnifyingCursorActive(magnifierWindow)) {
            g_magnifierWasActive = true;
            continue;
        }

        if (g_magnifierWasActive) {
            g_magnifierWasActive = false;
            RebuildCursorSet();
            continue;
        }

        const bool fullscreenActiveNow = IsForegroundWindowFullscreen();
        if (fullscreenActiveNow) {
            fullscreenWasActive = true;
        }

        CURSORINFO currentInfo = {sizeof(currentInfo)};
        if (!GetCursorInfo(&currentInfo) || !CursorInfoIsVisible(currentInfo)) {
            // Hidden/suppressed means the application or Windows explicitly does
            // not want a system cursor drawn. Most importantly, do not run slot
            // self-healing while hidden: applications sometimes implement hide
            // behavior by temporarily blanking/replacing a system cursor.
            cursorWasHidden = true;
            continue;
        }

        // Do not apply deferred cursor-slot changes until the fullscreen owner
        // has actually released the foreground AND Windows is drawing a cursor
        // again. This prevents an exit transition from resurrecting the pointer
        // while the application still intends it to remain hidden.
        if (!fullscreenActiveNow &&
            (fullscreenWasActive || cursorWasHidden ||
             g_cursorReapplyDeferred.load())) {
            const bool cameFromFullscreen = fullscreenWasActive;
            fullscreenWasActive = false;
            cursorWasHidden = false;

            const bool hadDeferredReapply =
                g_cursorReapplyDeferred.exchange(false);
            const bool forceDeferredReapply =
                g_forceCursorReapplyDeferred.exchange(false);
            const bool cursorSlotChanged = NormalCursorSlotChanged();
            if (hadDeferredReapply || cursorSlotChanged) {
                RebuildCursorSet(forceDeferredReapply || cursorSlotChanged);
                continue;
            }

            if (cameFromFullscreen) {
                // Force the next sampled frame to use the post-fullscreen
                // desktop instead of accepting a stale pre-transition snapshot.
                ResetDynamicGlassState(false);
            }
        }

        const DWORD currentRole = CursorRoleFromHandle(currentInfo.hCursor);

        // Do not fight a fullscreen application's cursor ownership. In
        // particular, exclusive-mode transitions can temporarily replace/blank
        // global cursor slots. We still render/update our cursor when the app is
        // visibly using one of our known system roles, but integrity repair is
        // deferred until fullscreen exits.
        if (!fullscreenActiveNow &&
            (!lastCursorIntegrityCheckTick ||
             afterWait - lastCursorIntegrityCheckTick >= kIdlePollInterval)) {
            lastCursorIntegrityCheckTick = afterWait;
            if (NormalCursorSlotChanged()) {
                RebuildCursorSet();
                continue;
            }
        }

        if (g_animationFramesReady.load()) {
            if (currentRole == CURSOR_WAIT) {
                SetPreparedSystemCursor(
                    g_busyFrames[static_cast<size_t>(g_animationFrame)],
                    CURSOR_WAIT);
                g_animationFrame = (g_animationFrame + 1) % kSpinnerFrames;
                continue;
            }
            if (currentRole == CURSOR_APPSTARTING) {
                SetPreparedSystemCursor(
                    g_workingFrames[static_cast<size_t>(g_animationFrame)],
                    CURSOR_APPSTARTING);
                g_animationFrame = (g_animationFrame + 1) % kSpinnerFrames;
                continue;
            }
        }

        if (IsSampledGlassStyle() && currentRole) {
            UpdateDynamicGlassCursor(currentRole, currentInfo.ptScreenPos);
        }
    }

    DestroyAnimationFrames();
    ResetDynamicGlassState(true);
    g_animationFramesReady.store(false);
    g_reapplyMessagePending.store(false);
    g_forceSystemReapplyPending.store(false);
    g_cursorReapplyDeferred.store(false);
    g_forceCursorReapplyDeferred.store(false);
    g_magnifierWasActive = false;
    g_expectedNormalCursor = nullptr;

    HWND windowToDestroy = g_cursorWindow.exchange(nullptr);
    if (windowToDestroy) {
        DestroyWindow(windowToDestroy);
    }

    UnregisterClassW(kCursorWindowClassName, instance);
    return 0;
}

bool StartAnimation() {
    g_animationStopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_animationStopEvent) {
        Wh_Log(L"CreateEvent failed: %lu", GetLastError());
        return false;
    }

    g_animationReadyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_animationReadyEvent) {
        Wh_Log(L"CreateEvent for animation readiness failed: %lu", GetLastError());
        CloseHandle(g_animationStopEvent);
        g_animationStopEvent = nullptr;
        return false;
    }

    g_animationThreadInitialized.store(false);
    g_animationThread =
        CreateThread(nullptr, 0, AnimationThreadProc, nullptr, 0, nullptr);
    if (!g_animationThread) {
        Wh_Log(L"CreateThread failed: %lu", GetLastError());
        CloseHandle(g_animationReadyEvent);
        g_animationReadyEvent = nullptr;
        CloseHandle(g_animationStopEvent);
        g_animationStopEvent = nullptr;
        return false;
    }

    HANDLE startupHandles[] = {g_animationReadyEvent, g_animationThread};
    const DWORD waitResult =
        WaitForMultipleObjects(ARRAYSIZE(startupHandles), startupHandles,
                               FALSE, 5000);
    const bool initialized =
        waitResult == WAIT_OBJECT_0 && g_animationThreadInitialized.load();

    if (initialized) {
        CloseHandle(g_animationReadyEvent);
        g_animationReadyEvent = nullptr;
        return true;
    }

    if (waitResult == WAIT_OBJECT_0 + 1) {
        Wh_Log(L"Animation thread exited during initialization");
    } else if (waitResult == WAIT_TIMEOUT) {
        Wh_Log(L"Animation thread initialization timed out");
    } else if (waitResult != WAIT_OBJECT_0) {
        Wh_Log(L"Waiting for animation thread initialization failed: %lu",
               GetLastError());
    }

    SetEvent(g_animationStopEvent);
    WaitForSingleObject(g_animationThread, INFINITE);
    CloseHandle(g_animationThread);
    g_animationThread = nullptr;

    CloseHandle(g_animationReadyEvent);
    g_animationReadyEvent = nullptr;

    CloseHandle(g_animationStopEvent);
    g_animationStopEvent = nullptr;

    DestroyAnimationFrames();
    return false;
}

void StopAnimation() {
    if (g_animationStopEvent) {
        SetEvent(g_animationStopEvent);
    }

    if (g_animationThread) {
        WaitForSingleObject(g_animationThread, INFINITE);
        CloseHandle(g_animationThread);
        g_animationThread = nullptr;
    }

    if (g_animationReadyEvent) {
        CloseHandle(g_animationReadyEvent);
        g_animationReadyEvent = nullptr;
    }

    if (g_animationStopEvent) {
        CloseHandle(g_animationStopEvent);
        g_animationStopEvent = nullptr;
    }

    g_cursorWindow.store(nullptr);
    g_animationThreadInitialized.store(false);
    g_animationFramesReady.store(false);
    g_reapplyMessagePending.store(false);
    g_forceSystemReapplyPending.store(false);
    g_cursorReapplyDeferred.store(false);
    g_forceCursorReapplyDeferred.store(false);
    DestroyAnimationFrames();
    ResetDynamicGlassState(true);
}

void RestoreWindowsCursorScheme() {
    SystemParametersInfoW(SPI_SETCURSORS, 0, nullptr, SPIF_SENDCHANGE);
}

LONG WINAPI RestoreCursorsOnCrash(EXCEPTION_POINTERS* exceptionInfo) {
    RestoreWindowsCursorScheme();
    if (g_previousExceptionFilter &&
        g_previousExceptionFilter != RestoreCursorsOnCrash) {
        return g_previousExceptionFilter(exceptionInfo);
    }
    return EXCEPTION_CONTINUE_SEARCH;
}

} // namespace

BOOL WhTool_ModInit() {
    g_previousExceptionFilter =
        SetUnhandledExceptionFilter(RestoreCursorsOnCrash);

    if (!StartAnimation()) {
        Wh_Log(L"Failed to start cursor worker thread");
        RestoreWindowsCursorScheme();
        SetUnhandledExceptionFilter(g_previousExceptionFilter);
        g_previousExceptionFilter = nullptr;
        return FALSE;
    }

    return TRUE;
}

void WhTool_ModSettingsChanged() {
    HWND window = g_cursorWindow.load();
    if (!window) {
        Wh_Log(L"Cursor worker window is unavailable");
        return;
    }

    QueueCursorReapply(window, false);
}

void WhTool_ModUninit() {
    StopAnimation();
    RestoreWindowsCursorScheme();
    SetUnhandledExceptionFilter(g_previousExceptionFilter);
    g_previousExceptionFilter = nullptr;
}

////////////////////////////////////////////////////////////////////////////////
// Windhawk tool mod implementation for mods which don't need to inject to other
// processes or hook other functions. Context:
// https://github.com/ramensoftware/windhawk/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process
//
// The mod will load and run in a dedicated windhawk.exe process.
//
// Paste the code below as part of the mod code, and use these callbacks:
// * WhTool_ModInit
// * WhTool_ModSettingsChanged
// * WhTool_ModUninit
//
// Currently, other callbacks are not supported.

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

    if (isToolModProcess) {
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
        Wh_Log(L"CreateProcess failed");
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
