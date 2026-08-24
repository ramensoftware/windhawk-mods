// ==WindhawkMod==
// @id              glass-cursors
// @name            Glass Cursors
// @description     Original DPI-aware translucent glass system cursors with a live animated loading indicator.
// @version         0.19.0
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
of scaling a 32 px bitmap, giving cleaner edges on high-DPI displays.

## Features
- Original white-outline / translucent-glass artwork.
- 25% and 50% glass fill variants with configurable interior RGB color.
- Automatic DPI-aware size selection, plus 32 / 48 / 64 / 96 px overrides.
- Replaces the standard Windows cursor roles.
- Busy and Working-in-background cursors use a live animated white glass spinner.
- Runs in a dedicated Windhawk tool process instead of inside Explorer.
- Disabling the mod restores the user's configured Windows cursor scheme.

## Display scaling
Windows system cursor slots hold one fixed-size cursor at a time. On mixed-DPI multi-monitor setups, the selected size can therefore look larger or smaller on a secondary display. Use the cursor resolution and artwork size settings to choose the best compromise for your setup.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- FillOpacity: "25"
  $name: Glass fill
  $description: Interior opacity of the glass cursor shapes.
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
  $description: RGB color of the translucent interior. Opacity is controlled separately by Glass fill.
- CursorSize: "0"
  $name: Cursor resolution
  $description: Automatic uses the current Windows cursor metric and system DPI.
  $options:
  - "0": Automatic (DPI-aware)
  - "32": 32 px
  - "48": 48 px
  - "64": 64 px
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
  $description: Shrinks the artwork inside the high-resolution cursor canvas without reducing edge quality.
  $options:
  - "65": Extra compact (65%)
  - "68": Compact (68%)
  - "75": Small (75%)
  - "82": Medium (82%)
  - "90": Large (90%)
  - "100": Full size (100%)
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shellapi.h>
#include <windhawk_api.h>
#include <windhawk_utils.h>
#include <stdint.h>
#include <algorithm>
#include <array>
#include <cmath>
#include <cstring>
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
    double r = 0.0;
    double g = 0.0;
    double b = 0.0;
    double a = 0.0;
};

class Surface {
public:
    Surface(int width, int height)
        : m_width(width), m_height(height), m_pixels(static_cast<size_t>(width) * height) {}

    int Width() const { return m_width; }
    int Height() const { return m_height; }

    void Blend(int x, int y, FColor c) {
        if (x < 0 || y < 0 || x >= m_width || y >= m_height || c.a <= 0.0) {
            return;
        }

        c.a = std::clamp(c.a, 0.0, 1.0);
        Pixel& dst = m_pixels[static_cast<size_t>(y) * m_width + x];

        const double outA = c.a + dst.a * (1.0 - c.a);
        if (outA <= 0.0) {
            dst = {};
            return;
        }

        dst.r = (c.r * c.a + dst.r * dst.a * (1.0 - c.a)) / outA;
        dst.g = (c.g * c.a + dst.g * dst.a * (1.0 - c.a)) / outA;
        dst.b = (c.b * c.a + dst.b * dst.a * (1.0 - c.a)) / outA;
        dst.a = outA;
    }

    const Pixel& At(int x, int y) const {
        return m_pixels[static_cast<size_t>(y) * m_width + x];
    }

private:
    int m_width;
    int m_height;
    std::vector<Pixel> m_pixels;
};

struct RenderedCursor {
    int size = 32;
    int hotspotX = 0;
    int hotspotY = 0;
    std::vector<uint32_t> pixels;
};

int g_fillOpacity = 25;
int g_glassRed = 33;
int g_glassGreen = 38;
int g_glassBlue = 46;
int g_cursorSize = 32;
int g_pointerStyle = 0;
int g_handStyle = 1;
int g_artworkScale = 68;
constexpr DWORD kAnimationDelay = 33;
HANDLE g_animationStopEvent = nullptr;
HANDLE g_animationThread = nullptr;
std::vector<HCURSOR> g_busyFrames;
std::vector<HCURSOR> g_workingFrames;
double g_renderOffsetX = 0.0;
double g_renderOffsetY = 0.0;

FColor White(double alpha = 1.0) {
    return {1.0, 1.0, 1.0, alpha};
}

FColor Glass(double alphaScale = 1.0) {
    return {g_glassRed / 255.0,
            g_glassGreen / 255.0,
            g_glassBlue / 255.0,
            std::clamp((g_fillOpacity / 100.0) * alphaScale, 0.0, 1.0)};
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

void DrawGlassPolygon(Surface& s,
                      const std::vector<Point>& points,
                      double outlineWidth,
                      double shadowOffset) {
    if (shadowOffset > 0.0) {
        std::vector<Point> shadowPoints = points;
        for (Point& p : shadowPoints) {
            p.x += shadowOffset;
            p.y += shadowOffset;
        }
        FillPolygon(s, shadowPoints, Shadow(0.22));
        StrokePolyline(s, shadowPoints, outlineWidth * 1.15, Shadow(0.15), true);
    }

    FillPolygon(s, points, Glass());
    StrokePolyline(s, points, outlineWidth, White(0.97), true);
}

void DrawGlassLine(Surface& s, Point a, Point b, double outerWidth) {
    DrawCapsule(s, {a.x + outerWidth * 0.18, a.y + outerWidth * 0.18},
                   {b.x + outerWidth * 0.18, b.y + outerWidth * 0.18},
                   outerWidth * 1.10, Shadow(0.20));
    DrawCapsule(s, a, b, outerWidth, White(0.97));
    const double innerWidth = std::max(1.0, outerWidth * 0.48);
    DrawCapsule(s, a, b, innerWidth, Glass());
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
    for (int i = 1; i <= steps; ++i) {
        const double t = i / static_cast<double>(steps);
        path.push_back(CubicPoint(start, c1, c2, end, t));
    }
}

Point PointerLocal(double tipX,
                   double tipY,
                   double axisX,
                   double axisY,
                   double u,
                   double v,
                   int n) {
    const double perpX = -axisY;
    const double perpY = axisX;
    return P(tipX + axisX * u + perpX * v,
             tipY + axisY * u + perpY * v,
             n);
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
        DrawCapsule(s, P(0.502, 0.300, n), P(0.502, 0.492, n),
                    lineWidth, White(0.92));
        DrawCapsule(s, P(0.612, 0.350, n), P(0.612, 0.505, n),
                    lineWidth, White(0.92));
        DrawCapsule(s, P(0.716, 0.402, n), P(0.716, 0.535, n),
                    lineWidth, White(0.92));
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
    DrawAnnulus(s, c, r + n * 0.040, r - n * 0.055, White(0.97));
    DrawAnnulus(s, c, r - n * 0.004, r - n * 0.040, Glass());
    DrawGlassLine(s, P(0.30, 0.30, n), P(0.70, 0.70, n), n * 0.085);
}

void RenderHelp(Surface& s, int n) {
    RenderArrowShape(s, n);
    const double w = n * 0.050;
    DrawGlassLine(s, P(0.61, 0.61, n), P(0.61, 0.66, n), w);
    DrawCapsule(s, P(0.61, 0.47, n), P(0.69, 0.42, n), w, White(0.97));
    DrawCapsule(s, P(0.69, 0.42, n), P(0.74, 0.49, n), w, White(0.97));
    DrawCapsule(s, P(0.74, 0.49, n), P(0.66, 0.57, n), w, White(0.97));
    DrawCircle(s, P(0.61, 0.73, n), w * 0.50, White(0.97));
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
    DrawCircle(s, P(0.50, 0.46, n), n * 0.115, White(0.96));
    DrawCircle(s, P(0.50, 0.46, n), n * 0.067, Glass(0.65));
}

void RenderPerson(Surface& s, int n) {
    DrawCircle(s, P(0.50, 0.32, n), n * 0.15, Shadow(0.18));
    DrawCircle(s, P(0.50, 0.30, n), n * 0.15 + n * 0.035, White(0.97));
    DrawCircle(s, P(0.50, 0.30, n), n * 0.15 - n * 0.012, Glass());

    std::vector<Point> shoulders = {
        P(0.20, 0.84, n), P(0.24, 0.67, n), P(0.35, 0.56, n),
        P(0.50, 0.52, n), P(0.65, 0.56, n), P(0.76, 0.67, n),
        P(0.80, 0.84, n)
    };
    StrokePolyline(s, shoulders, n * 0.095, White(0.97), false);
    StrokePolyline(s, shoulders, n * 0.047, Glass(), false);
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
        DrawCapsule(s, p0, p1, stroke, White(0.18 + 0.80 * fade));
        DrawCapsule(s, p0, p1, innerStroke, Glass(0.30 + 0.70 * fade));
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

RenderedCursor RenderRole(int size, DWORD role, int frame = 0) {
    const int fullCanvasSize = size * kSupersample;
    Surface surface(fullCanvasSize, fullCanvasSize);

    const Point hotspot = GetRoleHotspot(role);
    const double artworkScale = std::clamp(g_artworkScale / 100.0, 0.01, 1.0);
    const int n = std::max(1, static_cast<int>(std::lround(fullCanvasSize * artworkScale)));

    g_renderOffsetX = hotspot.x * (fullCanvasSize - n);
    g_renderOffsetY = hotspot.y * (fullCanvasSize - n);

    switch (role) {
        case CURSOR_NORMAL:
            RenderArrowShape(surface, n);
            break;
        case CURSOR_IBEAM:
            RenderIBeam(surface, n);
            break;
        case CURSOR_WAIT:
            RenderSpinner(surface, n, P(0.50, 0.50, n), n * 0.255, n * 0.041, frame % kSpinnerFrames);
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
            RenderSpinner(surface, n, P(0.805, 0.185, n), n * 0.115, n * 0.020, frame % kSpinnerFrames);
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

    RenderedCursor result;
    result.size = size;
    result.hotspotX = std::clamp(static_cast<int>(std::lround(size * hotspot.x)), 0, size - 1);
    result.hotspotY = std::clamp(static_cast<int>(std::lround(size * hotspot.y)), 0, size - 1);
    result.pixels = Downsample(surface, size);
    return result;
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
    for (int supported : {32, 48, 64, 96}) {
        if (configured == supported) {
            return supported;
        }
    }

    int desired = GetSystemMetrics(SM_CXCURSOR);
    int dpi = 96;
    HDC screenDc = GetDC(nullptr);
    if (screenDc) {
        const int measuredDpi = GetDeviceCaps(screenDc, LOGPIXELSX);
        if (measuredDpi > 0) {
            dpi = measuredDpi;
        }
        ReleaseDC(nullptr, screenDc);
    }
    desired = std::max(desired, MulDiv(32, dpi, 96));

    constexpr std::array<int, 4> sizes = {32, 48, 64, 96};
    int best = sizes[0];
    int bestDistance = std::abs(desired - best);
    for (int candidate : sizes) {
        const int distance = std::abs(desired - candidate);
        if (distance < bestDistance) {
            best = candidate;
            bestDistance = distance;
        }
    }
    return best;
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

    Wh_Log(L"size=%d fill=%d%% color=%d,%d,%d pointerStyle=%d handStyle=%d artwork=%d%%",
           g_cursorSize, g_fillOpacity, g_glassRed, g_glassGreen, g_glassBlue,
           g_pointerStyle, g_handStyle, g_artworkScale);
}

void ApplyStaticCursors() {
    const std::array<DWORD, 13> roles = {
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
    };

    for (DWORD role : roles) {
        SetRenderedSystemCursor(RenderRole(g_cursorSize, role), role);
    }

    SetRenderedSystemCursor(RenderRole(g_cursorSize, CURSOR_PERSON), CURSOR_PERSON);
}

void DestroyAnimationFrames() {
    for (HCURSOR cursor : g_busyFrames) {
        if (cursor) {
            DestroyCursor(cursor);
        }
    }
    for (HCURSOR cursor : g_workingFrames) {
        if (cursor) {
            DestroyCursor(cursor);
        }
    }
    g_busyFrames.clear();
    g_workingFrames.clear();
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

    return true;
}

bool PrepareAnimationFrames() {
    DestroyAnimationFrames();
    g_busyFrames.reserve(kSpinnerFrames);
    g_workingFrames.reserve(kSpinnerFrames);

    for (int frame = 0; frame < kSpinnerFrames; ++frame) {
        if (WaitForSingleObject(g_animationStopEvent, 0) == WAIT_OBJECT_0) {
            return false;
        }

        HCURSOR busyCursor =
            CreateCursorFromPixels(RenderRole(g_cursorSize, CURSOR_WAIT, frame));
        HCURSOR workingCursor =
            CreateCursorFromPixels(RenderRole(g_cursorSize, CURSOR_APPSTARTING, frame));
        if (!busyCursor || !workingCursor) {
            if (busyCursor) {
                DestroyCursor(busyCursor);
            }
            if (workingCursor) {
                DestroyCursor(workingCursor);
            }
            Wh_Log(L"Failed to create animation frame %d", frame);
            return false;
        }

        g_busyFrames.push_back(busyCursor);
        g_workingFrames.push_back(workingCursor);
    }

    SetPreparedSystemCursor(g_busyFrames.front(), CURSOR_WAIT);
    SetPreparedSystemCursor(g_workingFrames.front(), CURSOR_APPSTARTING);
    return true;
}

DWORD GetVisibleBusyCursorRole() {
    CURSORINFO cursorInfo = {sizeof(cursorInfo)};
    if (!GetCursorInfo(&cursorInfo) ||
        !(cursorInfo.flags & CURSOR_SHOWING) ||
        !cursorInfo.hCursor) {
        return 0;
    }

    for (DWORD cursorId : {CURSOR_WAIT, CURSOR_APPSTARTING}) {
        HCURSOR systemCursor = static_cast<HCURSOR>(LoadImageW(
            nullptr, MAKEINTRESOURCEW(cursorId), IMAGE_CURSOR, 0, 0, LR_SHARED));
        if (systemCursor && cursorInfo.hCursor == systemCursor) {
            return cursorId;
        }
    }

    return 0;
}

DWORD WINAPI AnimationThreadProc(LPVOID) {
    if (!PrepareAnimationFrames()) {
        return 0;
    }

    int frame = 0;
    while (WaitForSingleObject(g_animationStopEvent, 0) != WAIT_OBJECT_0) {
        const DWORD visibleRole = GetVisibleBusyCursorRole();
        if (visibleRole == CURSOR_WAIT) {
            SetPreparedSystemCursor(g_busyFrames[frame], CURSOR_WAIT);
            frame = (frame + 1) % kSpinnerFrames;
        } else if (visibleRole == CURSOR_APPSTARTING) {
            SetPreparedSystemCursor(g_workingFrames[frame], CURSOR_APPSTARTING);
            frame = (frame + 1) % kSpinnerFrames;
        }

        const DWORD delay = visibleRole ? kAnimationDelay : 100;
        if (WaitForSingleObject(g_animationStopEvent, delay) == WAIT_OBJECT_0) {
            break;
        }
    }

    return 0;
}

bool StartAnimation() {
    g_animationStopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_animationStopEvent) {
        Wh_Log(L"CreateEvent failed: %lu", GetLastError());
        return false;
    }

    g_animationThread = CreateThread(nullptr, 0, AnimationThreadProc, nullptr, 0, nullptr);
    if (!g_animationThread) {
        Wh_Log(L"CreateThread failed: %lu", GetLastError());
        CloseHandle(g_animationStopEvent);
        g_animationStopEvent = nullptr;
        return false;
    }

    return true;
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

    if (g_animationStopEvent) {
        CloseHandle(g_animationStopEvent);
        g_animationStopEvent = nullptr;
    }

    DestroyAnimationFrames();
}

void RestoreWindowsCursorScheme() {
    SystemParametersInfoW(SPI_SETCURSORS, 0, nullptr, SPIF_SENDCHANGE);
}

LONG WINAPI RestoreCursorsOnCrash(EXCEPTION_POINTERS*) {
    RestoreWindowsCursorScheme();
    return EXCEPTION_CONTINUE_SEARCH;
}

} // namespace

BOOL WhTool_ModInit() {
    SetUnhandledExceptionFilter(RestoreCursorsOnCrash);
    LoadSettings();
    ApplyStaticCursors();
    if (!StartAnimation()) {
        Wh_Log(L"Failed to start cursor animation");
    }
    return TRUE;
}

void WhTool_ModSettingsChanged() {
    StopAnimation();
    LoadSettings();
    ApplyStaticCursors();
    if (!StartAnimation()) {
        Wh_Log(L"Failed to restart cursor animation");
    }
}

void WhTool_ModUninit() {
    StopAnimation();
    RestoreWindowsCursorScheme();
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