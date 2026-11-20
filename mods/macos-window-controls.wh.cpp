// ==WindhawkMod==
// @id              macos-window-controls
// @name            macOS Window Controls
// @description     macOS-style traffic lights on an extra strip above every window
// @version         2.1.0
// @author          APARUP DATTA PINAK
// @github          https://github.com/pinakdatta2013-ops
// @include         notepad.exe
// @include         explorer.exe
// @compilerOptions -ldwmapi -lgdi32 -luser32 -ladvapi32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# macOS Window Controls

Adds a thin strip above every window with three macOS-style traffic lights
(close, minimize, maximize) on the left. The window's own content is not
touched.

- Anti-aliased, DPI-aware buttons
- Light / dark theme follows the system
- Buttons turn grey when the window is inactive
- Symbols appear when you hover the button group
- Drag the strip to move the window, double-click to maximize/restore
- If there is no room above the window (maximized or at the top of the
  screen) the buttons fall back to a compact overlay on the title bar
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- layout: auto
  $name: Layout
  $options:
  - auto: Strip above the window (compact fallback)
  - compact: Always compact on the title bar
- stripHeight: 32
  $name: Strip height (px at 100% scaling, 22-56)
- cornerRadius: 24
  $name: Strip corner radius (px at 100% scaling, 6-40)
*/
// ==/WindhawkModSettings==

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0A00
#endif

#include <windows.h>
#include <windowsx.h>
#include <dwmapi.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <new>
#include <vector>

static constexpr wchar_t CLASS_NAME[] = L"WH_MacOSControlsOverlay_V2";

// Logical sizes at 96 DPI.
static constexpr int COMPACT_W = 70;
static constexpr int BTN_RADIUS = 6;
static constexpr int BTN_CX[3] = {16, 36, 56};  // close, minimize, maximize

enum class Layout { Strip, Compact };

struct OverlayState {
    HWND owner = nullptr;
    UINT dpi = 96;
    int hover = -1;        // button under the cursor
    int pressed = -1;      // button being pressed
    bool groupHover = false;
    bool active = true;
    bool dark = true;
    Layout layout = Layout::Strip;
    int width = 0;         // physical pixels
    int height = 0;        // total window height = stripH + overlap
    int stripH = 0;        // visible strip height
    int overlap = 0;       // rows below the strip that hide the window's top
                           // border and fill its rounded-corner notches
    int winRadius = 0;     // the real window's corner radius
    COLORREF bg = RGB(32, 32, 32);  // sampled title-bar colour
    int cornerR = 16;      // strip corner radius at 100% scaling
    DWORD lastSample = 0;
};

struct OverlayEntry {
    HWND target;
    HWND overlay;
};

static std::atomic<bool> g_stop{false};
static std::atomic<bool> g_compactOnly{false};
static std::atomic<int> g_stripHeight{28};
static std::atomic<int> g_cornerRadius{16};
static std::atomic<bool> g_dark{true};

static HANDLE g_thread = nullptr;
static HANDLE g_stopEvent = nullptr;
static HWINEVENTHOOK g_locationHook = nullptr;
static HWINEVENTHOOK g_foregroundHook = nullptr;
static std::vector<OverlayEntry> g_entries;

// ---------------------------------------------------------------- helpers

static int Scale(int value, UINT dpi) {
    return MulDiv(value, static_cast<int>(dpi ? dpi : 96), 96);
}

static OverlayState* GetState(HWND hwnd) {
    return reinterpret_cast<OverlayState*>(
        GetWindowLongPtrW(hwnd, GWLP_USERDATA));
}

static void LoadSettings() {
    PCWSTR layout = Wh_GetStringSetting(L"layout");
    g_compactOnly = layout && wcscmp(layout, L"compact") == 0;
    if (layout) {
        Wh_FreeStringSetting(layout);
    }
    g_stripHeight = std::clamp(Wh_GetIntSetting(L"stripHeight"), 22, 56);
    g_cornerRadius = std::clamp(Wh_GetIntSetting(L"cornerRadius"), 6, 40);
}

static bool IsSystemDark() {
    DWORD value = 1;
    DWORD size = sizeof(value);
    if (RegGetValueW(
            HKEY_CURRENT_USER,
            L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
            L"AppsUseLightTheme", RRF_RT_REG_DWORD, nullptr, &value,
            &size) != ERROR_SUCCESS) {
        return false;
    }
    return value == 0;
}

// 0 = close, 1 = minimize, 2 = maximize/restore.
static int ButtonAt(const OverlayState* s, int x, int y) {
    if (!s) {
        return -1;
    }
    const int cy = s->stripH / 2;
    const int hr = Scale(9, s->dpi);
    for (int i = 0; i < 3; ++i) {
        const int dx = x - Scale(BTN_CX[i], s->dpi);
        const int dy = y - cy;
        if (dx * dx + dy * dy <= hr * hr) {
            return i;
        }
    }
    return -1;
}

// True when the cursor is anywhere inside the three-button group.
static bool InGroup(const OverlayState* s, int x, int y) {
    if (!s) {
        return false;
    }
    const int hr = Scale(9, s->dpi);
    return x >= Scale(BTN_CX[0], s->dpi) - hr &&
           x <= Scale(BTN_CX[2], s->dpi) + hr &&
           std::abs(y - s->stripH / 2) <= hr;
}

// ------------------------------------------------- anti-aliased drawing
// All drawing happens in a 32-bit premultiplied-alpha DIB that is handed to
// UpdateLayeredWindow, so edges are smooth and the window can be transparent.

static void BlendPixel(DWORD* buf, int w, int h, int x, int y,
                       COLORREF c, float a) {
    if (x < 0 || y < 0 || x >= w || y >= h || a <= 0.f) {
        return;
    }
    if (a > 1.f) {
        a = 1.f;
    }
    DWORD& d = buf[y * w + x];
    const float inv = 1.f - a;
    const float db = static_cast<float>(d & 0xFF);
    const float dg = static_cast<float>((d >> 8) & 0xFF);
    const float dr = static_cast<float>((d >> 16) & 0xFF);
    const float da = static_cast<float>((d >> 24) & 0xFF);
    const DWORD nb = static_cast<DWORD>(GetBValue(c) * a + db * inv + 0.5f);
    const DWORD ng = static_cast<DWORD>(GetGValue(c) * a + dg * inv + 0.5f);
    const DWORD nr = static_cast<DWORD>(GetRValue(c) * a + dr * inv + 0.5f);
    const DWORD na = static_cast<DWORD>(255.f * a + da * inv + 0.5f);
    d = (na << 24) | (nr << 16) | (ng << 8) | nb;
}

static void DrawDisc(DWORD* buf, int w, int h, float cx, float cy,
                     float r, COLORREF c) {
    const int x0 = static_cast<int>(std::floor(cx - r - 1));
    const int x1 = static_cast<int>(std::ceil(cx + r + 1));
    const int y0 = static_cast<int>(std::floor(cy - r - 1));
    const int y1 = static_cast<int>(std::ceil(cy + r + 1));
    for (int y = y0; y <= y1; ++y) {
        for (int x = x0; x <= x1; ++x) {
            const float dx = x + 0.5f - cx;
            const float dy = y + 0.5f - cy;
            const float dist = std::sqrt(dx * dx + dy * dy);
            BlendPixel(buf, w, h, x, y, c,
                       std::clamp(r + 0.5f - dist, 0.f, 1.f));
        }
    }
}

static void DrawLineAA(DWORD* buf, int w, int h, float ax, float ay,
                       float bx, float by, float thick, COLORREF c) {
    const int x0 = static_cast<int>(std::floor(std::min(ax, bx) - thick - 1));
    const int x1 = static_cast<int>(std::ceil(std::max(ax, bx) + thick + 1));
    const int y0 = static_cast<int>(std::floor(std::min(ay, by) - thick - 1));
    const int y1 = static_cast<int>(std::ceil(std::max(ay, by) + thick + 1));
    const float vx = bx - ax;
    const float vy = by - ay;
    const float len2 = vx * vx + vy * vy;
    for (int y = y0; y <= y1; ++y) {
        for (int x = x0; x <= x1; ++x) {
            const float px = x + 0.5f - ax;
            const float py = y + 0.5f - ay;
            float t = len2 > 0.f ? (px * vx + py * vy) / len2 : 0.f;
            t = std::clamp(t, 0.f, 1.f);
            const float dx = px - t * vx;
            const float dy = py - t * vy;
            const float dist = std::sqrt(dx * dx + dy * dy);
            BlendPixel(buf, w, h, x, y, c,
                       std::clamp(thick / 2.f + 0.5f - dist, 0.f, 1.f));
        }
    }
}

static COLORREF Darken(COLORREF c, float f) {
    return RGB(static_cast<BYTE>(GetRValue(c) * f),
               static_cast<BYTE>(GetGValue(c) * f),
               static_cast<BYTE>(GetBValue(c) * f));
}

static bool g_win11 = false;

static bool IsWindows11() {
    typedef LONG(WINAPI * RtlGetVersionFn)(PRTL_OSVERSIONINFOW);
    HMODULE nt = GetModuleHandleW(L"ntdll.dll");
    RtlGetVersionFn fn =
        nt ? reinterpret_cast<RtlGetVersionFn>(
                 reinterpret_cast<void*>(GetProcAddress(nt, "RtlGetVersion")))
           : nullptr;
    RTL_OSVERSIONINFOW vi{};
    vi.dwOSVersionInfoSize = sizeof(vi);
    return fn && fn(&vi) == 0 && vi.dwBuildNumber >= 22000;
}

static COLORREF Mix(COLORREF a, COLORREF b, float t) {
    t = std::clamp(t, 0.f, 1.f);
    auto ch = [t](int x, int y) {
        return static_cast<BYTE>(x + (y - x) * t + 0.5f);
    };
    return RGB(ch(GetRValue(a), GetRValue(b)), ch(GetGValue(a), GetGValue(b)),
               ch(GetBValue(a), GetBValue(b)));
}

static bool IsDarkColor(COLORREF c) {
    return 0.30f * GetRValue(c) + 0.59f * GetGValue(c) +
               0.11f * GetBValue(c) < 128.f;
}

static bool NearColor(COLORREF a, COLORREF b) {
    return std::abs(int(GetRValue(a)) - int(GetRValue(b))) <= 2 &&
           std::abs(int(GetGValue(a)) - int(GetGValue(b))) <= 2 &&
           std::abs(int(GetBValue(a)) - int(GetBValue(b))) <= 2;
}

// Reads the real title-bar colour (Mica, accent, white, ...) from the screen
// so the strip looks like part of the same window.
static bool SampleTitleColor(HWND target, const RECT& frame, UINT dpi,
                             COLORREF* out) {
    int x = frame.right - Scale(170, dpi);  // left of the caption buttons
    const int y = frame.top + Scale(9, dpi);
    if (x < frame.left + Scale(120, dpi)) {
        x = frame.left + (frame.right - frame.left) / 2;
    }

    HWND hit = WindowFromPoint(POINT{x, y});
    if (!hit || GetAncestor(hit, GA_ROOTOWNER) != target) {
        return false;  // something else covers that spot
    }

    HDC screen = GetDC(nullptr);
    if (!screen) {
        return false;
    }
    const COLORREF c = GetPixel(screen, x, y);
    ReleaseDC(nullptr, screen);
    if (c == CLR_INVALID) {
        return false;
    }
    *out = c;
    return true;
}

// Distance (px, positive = inside) from a point to a squircle (superellipse,
// exponent 4) corner of radius R. (px, py) are measured from the outer corner.
static float SquircleInside(float px, float py, float R) {
    const float n = 4.f;
    const float u = std::fabs(R - px) / R;
    const float v = std::fabs(R - py) / R;
    const float sum = std::pow(u, n) + std::pow(v, n);
    if (sum <= 1e-6f) {
        return R;
    }
    const float F = std::pow(sum, 1.f / n);
    const float common = std::pow(F, 1.f - n) / R;
    const float fx = common * std::pow(u, n - 1.f);
    const float fy = common * std::pow(v, n - 1.f);
    const float g = std::sqrt(fx * fx + fy * fy);
    if (g <= 1e-6f) {
        return R;
    }
    return (1.f - F) / g;
}

// Strip: squircle top corners, 1px border that continues the window's own
// border, and (below the strip) a few "overlap" rows that hide the window's
// top border line and fill the notches left by its rounded corners.
static void DrawStripBackground(DWORD* buf, int w, int h,
                                const OverlayState* s) {
    const float k = static_cast<float>(s->dpi) / 96.f;
    const float R =
        std::min(s->cornerR * k, static_cast<float>(s->stripH));
    const float bw = std::max(1.f, k);
    const COLORREF fill = s->bg;
    const COLORREF border =
        Mix(fill, IsDarkColor(fill) ? RGB(255, 255, 255) : RGB(0, 0, 0),
            0.14f);
    const int coverRows = std::max(1, Scale(1, s->dpi));
    const float winR = static_cast<float>(s->winRadius);

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            const float px = x + 0.5f;
            const float py = y + 0.5f;
            const float lx = std::min(px, w - px);  // distance to nearest side
            float inside = 0.f;
            float cover = 1.f;

            if (y < s->stripH) {
                inside = std::min(lx, py);
                if (py < R && lx < R) {
                    inside = SquircleInside(lx, py, R);
                }
            } else {
                const int t = y - s->stripH;
                inside = lx;
                if (t >= coverRows) {
                    // Only fill the notch outside the window's rounded corner.
                    cover = 0.f;
                    const float ty = t + 0.5f;
                    if (winR > 0.f && ty < winR && lx < winR) {
                        const float dx = winR - lx;
                        const float dy = winR - ty;
                        const float dist = std::sqrt(dx * dx + dy * dy);
                        cover = std::clamp(dist - winR + 1.f, 0.f, 1.f);
                    }
                }
            }

            const float a = std::clamp(inside + 0.5f, 0.f, 1.f) * cover;
            if (a <= 0.f) {
                continue;
            }
            const float b = std::clamp(bw + 0.5f - inside, 0.f, 1.f);
            BlendPixel(buf, w, h, x, y, Mix(fill, border, b), a);
        }
    }
}

static void Render(HWND hwnd) {
    OverlayState* s = GetState(hwnd);
    if (!s || s->width <= 0 || s->height <= 0) {
        return;
    }

    const int w = s->width;
    const int h = s->height;

    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = w;
    bi.bmiHeader.biHeight = -h;  // top-down
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;

    HDC screen = GetDC(nullptr);
    if (!screen) {
        return;
    }
    HDC mem = CreateCompatibleDC(screen);
    void* bits = nullptr;
    HBITMAP bmp = mem ? CreateDIBSection(screen, &bi, DIB_RGB_COLORS, &bits,
                                         nullptr, 0)
                      : nullptr;
    if (!mem || !bmp || !bits) {
        if (bmp) DeleteObject(bmp);
        if (mem) DeleteDC(mem);
        ReleaseDC(nullptr, screen);
        return;
    }

    HGDIOBJ old = SelectObject(mem, bmp);
    DWORD* buf = static_cast<DWORD*>(bits);
    std::fill(buf, buf + static_cast<size_t>(w) * h, 0u);  // transparent

    const float k = static_cast<float>(s->dpi) / 96.f;

    // Strip background (squircle corners, colour-matched to the title bar).
    if (s->layout == Layout::Strip) {
        DrawStripBackground(buf, w, h, s);
    }

    // Traffic lights.
    const COLORREF live[3] = {RGB(255, 95, 87), RGB(254, 188, 46),
                              RGB(40, 200, 64)};
    const COLORREF symbol[3] = {RGB(77, 0, 0), RGB(100, 62, 0),
                                RGB(0, 80, 0)};
    const COLORREF idle = s->dark ? RGB(88, 88, 88) : RGB(200, 200, 200);
    const bool colored = s->active || s->groupHover;

    const float r = static_cast<float>(Scale(BTN_RADIUS, s->dpi));
    const float cy = s->stripH / 2.0f;
    const float border = std::max(0.8f, k * 0.8f);
    const float d = 2.6f * k;
    const float thick = std::max(1.2f, 1.3f * k);

    for (int i = 0; i < 3; ++i) {
        const float cx = static_cast<float>(Scale(BTN_CX[i], s->dpi));
        COLORREF fill = colored ? live[i] : idle;
        if (s->pressed == i && s->hover == i) {
            fill = Darken(fill, 0.75f);
        }

        DrawDisc(buf, w, h, cx, cy, r, Darken(fill, 0.82f));  // border
        DrawDisc(buf, w, h, cx, cy, r - border, fill);

        if (s->groupHover && colored) {
            if (i == 0) {
                DrawLineAA(buf, w, h, cx - d, cy - d, cx + d, cy + d,
                           thick, symbol[i]);
                DrawLineAA(buf, w, h, cx + d, cy - d, cx - d, cy + d,
                           thick, symbol[i]);
            } else if (i == 1) {
                DrawLineAA(buf, w, h, cx - d - 0.5f, cy, cx + d + 0.5f, cy,
                           thick, symbol[i]);
            } else {
                DrawLineAA(buf, w, h, cx - d - 0.5f, cy, cx + d + 0.5f, cy,
                           thick, symbol[i]);
                DrawLineAA(buf, w, h, cx, cy - d - 0.5f, cx, cy + d + 0.5f,
                           thick, symbol[i]);
            }
        }
    }

    BLENDFUNCTION bf{};
    bf.BlendOp = AC_SRC_OVER;
    bf.SourceConstantAlpha = 255;
    bf.AlphaFormat = AC_SRC_ALPHA;
    POINT src{0, 0};
    SIZE size{w, h};
    UpdateLayeredWindow(hwnd, screen, nullptr, &size, mem, &src, 0, &bf,
                        ULW_ALPHA);

    SelectObject(mem, old);
    DeleteObject(bmp);
    DeleteDC(mem);
    ReleaseDC(nullptr, screen);
}

// ----------------------------------------------------------- overlay window

// Hit-test for the strip. The outer edge of the strip acts as the real
// window's resize border (top, top-left, top-right, left, right), so the
// window can be resized from the strip like a normal window.
static LRESULT StripHitTest(const OverlayState* s, int x, int y) {
    if (y >= s->stripH) {
        return HTTRANSPARENT;  // overlap rows belong to the real window
    }
    if (ButtonAt(s, x, y) >= 0) {
        return HTCLIENT;
    }

    const LONG_PTR style = GetWindowLongPtrW(s->owner, GWL_STYLE);
    if (!(style & WS_THICKFRAME) || IsZoomed(s->owner)) {
        return HTCLIENT;  // not resizable
    }

    const int edge = Scale(5, s->dpi);
    const int corner = Scale(12, s->dpi);
    const bool left = x < edge;
    const bool right = x >= s->width - edge;
    const bool top = y < edge;

    if ((top && x < corner) || (left && y < corner)) {
        return HTTOPLEFT;
    }
    if ((top && x >= s->width - corner) || (right && y < corner)) {
        return HTTOPRIGHT;
    }
    if (top) {
        return HTTOP;
    }
    if (left) {
        return HTLEFT;
    }
    if (right) {
        return HTRIGHT;
    }
    return HTCLIENT;
}

static void DoButtonAction(HWND owner, int button) {
    if (!IsWindow(owner)) {
        return;
    }
    if (button == 0) {
        PostMessageW(owner, WM_SYSCOMMAND, SC_CLOSE, 0);
    } else if (button == 1) {
        PostMessageW(owner, WM_SYSCOMMAND, SC_MINIMIZE, 0);
    } else if (button == 2) {
        PostMessageW(owner, WM_SYSCOMMAND,
                     IsZoomed(owner) ? SC_RESTORE : SC_MAXIMIZE, 0);
    }
}

static LRESULT CALLBACK OverlayProc(HWND hwnd, UINT msg, WPARAM wp,
                                    LPARAM lp) {
    OverlayState* s = GetState(hwnd);

    switch (msg) {
    case WM_MOUSEACTIVATE:
        return MA_NOACTIVATE;

    case WM_ERASEBKGND:
        return 1;

    case WM_PAINT:
        ValidateRect(hwnd, nullptr);  // content is drawn via UpdateLayeredWindow
        return 0;

    case WM_NCHITTEST: {
        if (!s) {
            return HTTRANSPARENT;
        }
        POINT pt{GET_X_LPARAM(lp), GET_Y_LPARAM(lp)};
        ScreenToClient(hwnd, &pt);
        if (s->layout == Layout::Strip) {
            return StripHitTest(s, pt.x, pt.y);
        }
        return ButtonAt(s, pt.x, pt.y) >= 0 ? HTCLIENT : HTTRANSPARENT;
    }

    case WM_SETCURSOR: {
        LPCWSTR cur = nullptr;
        switch (LOWORD(lp)) {
        case HTCLIENT:
            cur = (s && s->hover >= 0) ? IDC_HAND : IDC_ARROW;
            break;
        case HTTOP:
            cur = IDC_SIZENS;
            break;
        case HTLEFT:
        case HTRIGHT:
            cur = IDC_SIZEWE;
            break;
        case HTTOPLEFT:
            cur = IDC_SIZENWSE;
            break;
        case HTTOPRIGHT:
            cur = IDC_SIZENESW;
            break;
        }
        if (cur) {
            SetCursor(LoadCursorW(nullptr, cur));
            return TRUE;
        }
        break;
    }

    case WM_MOUSEMOVE: {
        if (!s) {
            break;
        }
        const int x = GET_X_LPARAM(lp);
        const int y = GET_Y_LPARAM(lp);
        const int hover = ButtonAt(s, x, y);
        const bool group = InGroup(s, x, y) || hover >= 0;
        if (s->hover != hover || s->groupHover != group) {
            s->hover = hover;
            s->groupHover = group;
            Render(hwnd);
        }
        TRACKMOUSEEVENT tme{};
        tme.cbSize = sizeof(tme);
        tme.dwFlags = TME_LEAVE;
        tme.hwndTrack = hwnd;
        TrackMouseEvent(&tme);
        return 0;
    }

    case WM_MOUSELEAVE:
        if (s && (s->hover != -1 || s->groupHover) && s->pressed < 0) {
            s->hover = -1;
            s->groupHover = false;
            Render(hwnd);
        }
        return 0;

    case WM_LBUTTONDOWN:
    case WM_LBUTTONDBLCLK: {
        if (!s || !IsWindow(s->owner)) {
            return 0;
        }
        const int button =
            ButtonAt(s, GET_X_LPARAM(lp), GET_Y_LPARAM(lp));
        if (button >= 0) {
            s->pressed = button;
            SetCapture(hwnd);
            Render(hwnd);
            return 0;
        }
        if (s->layout == Layout::Strip) {
            if (msg == WM_LBUTTONDBLCLK) {
                PostMessageW(s->owner, WM_SYSCOMMAND,
                             IsZoomed(s->owner) ? SC_RESTORE : SC_MAXIMIZE,
                             0);
            } else {
                // Let the strip drag the real window.
                POINT p{};
                GetCursorPos(&p);
                SetForegroundWindow(s->owner);
                ReleaseCapture();
                PostMessageW(s->owner, WM_NCLBUTTONDOWN, HTCAPTION,
                             MAKELPARAM(p.x, p.y));
            }
        }
        return 0;
    }

    case WM_LBUTTONUP: {
        if (!s) {
            return 0;
        }
        if (s->pressed >= 0) {
            const int pressed = s->pressed;
            const int button =
                ButtonAt(s, GET_X_LPARAM(lp), GET_Y_LPARAM(lp));
            s->pressed = -1;
            ReleaseCapture();
            Render(hwnd);
            if (button == pressed) {
                DoButtonAction(s->owner, button);
            }
        }
        return 0;
    }

    case WM_NCMOUSEMOVE:
        if (s && (s->hover != -1 || s->groupHover)) {
            s->hover = -1;
            s->groupHover = false;
            Render(hwnd);
        }
        break;

    case WM_NCLBUTTONDOWN:
        // Edge of the strip pressed: start the real window's resize loop.
        if (s && IsWindow(s->owner) && wp >= HTLEFT && wp <= HTTOPRIGHT) {
            SetForegroundWindow(s->owner);
            PostMessageW(s->owner, WM_NCLBUTTONDOWN, wp, lp);
            return 0;
        }
        break;

    case WM_NCDESTROY:
        if (s) {
            delete s;
            SetWindowLongPtrW(hwnd, GWLP_USERDATA, 0);
        }
        break;
    }

    return DefWindowProcW(hwnd, msg, wp, lp);
}

// ------------------------------------------------------- target windows

static bool IsTargetWindow(HWND hwnd) {
    if (!IsWindow(hwnd) || !IsWindowVisible(hwnd)) {
        return false;
    }
    if (GetWindow(hwnd, GW_OWNER) != nullptr) {
        return false;
    }

    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid != GetCurrentProcessId()) {
        return false;
    }

    const LONG_PTR style = GetWindowLongPtrW(hwnd, GWL_STYLE);
    const LONG_PTR exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);

    if ((style & WS_CAPTION) != WS_CAPTION) {
        return false;
    }
    if (exStyle & WS_EX_TOOLWINDOW) {
        return false;
    }
    return true;
}

static HWND FindOverlay(HWND target) {
    for (const auto& e : g_entries) {
        if (e.target == target) {
            return e.overlay;
        }
    }
    return nullptr;
}

static HWND CreateOverlay(HWND target) {
    // Owned popup: Windows keeps it above its owner and minimizes/restores
    // it together with the owner.
    HWND overlay = CreateWindowExW(
        WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_LAYERED, CLASS_NAME, L"",
        WS_POPUP, 0, 0, 1, 1, target, nullptr, GetModuleHandleW(nullptr),
        nullptr);
    if (!overlay) {
        return nullptr;
    }

    auto* state = new (std::nothrow) OverlayState{};
    if (!state) {
        DestroyWindow(overlay);
        return nullptr;
    }
    state->owner = target;
    state->dpi = GetDpiForWindow(target);
    if (state->dpi == 0) {
        state->dpi = 96;
    }
    SetWindowLongPtrW(overlay, GWLP_USERDATA,
                      reinterpret_cast<LONG_PTR>(state));
    return overlay;
}

static void HideOverlay(HWND overlay) {
    if (IsWindowVisible(overlay)) {
        ShowWindow(overlay, SW_HIDE);
    }
}

// Place the strip directly above the window frame. When there is no room
// (maximized, or window touching the top of the work area) fall back to a
// compact overlay centred in the real title bar.
static void PositionOverlay(HWND target, HWND overlay) {
    OverlayState* s = GetState(overlay);
    if (!s) {
        return;
    }

    DWORD cloaked = 0;
    DwmGetWindowAttribute(target, DWMWA_CLOAKED, &cloaked, sizeof(cloaked));
    if (!IsWindow(target) || !IsWindowVisible(target) || IsIconic(target) ||
        cloaked) {
        HideOverlay(overlay);
        return;
    }

    RECT frame{};
    if (FAILED(DwmGetWindowAttribute(target, DWMWA_EXTENDED_FRAME_BOUNDS,
                                     &frame, sizeof(frame))) &&
        !GetWindowRect(target, &frame)) {
        HideOverlay(overlay);
        return;
    }

    UINT dpi = GetDpiForWindow(target);
    if (dpi == 0) {
        dpi = 96;
    }

    if (frame.right - frame.left < Scale(150, dpi) ||
        frame.bottom <= frame.top) {
        HideOverlay(overlay);
        return;
    }

    MONITORINFO mi{};
    mi.cbSize = sizeof(mi);
    GetMonitorInfoW(MonitorFromWindow(target, MONITOR_DEFAULTTONEAREST),
                    &mi);

    // Hide for true fullscreen windows.
    if (frame.left <= mi.rcMonitor.left && frame.top <= mi.rcMonitor.top &&
        frame.right >= mi.rcMonitor.right &&
        frame.bottom >= mi.rcMonitor.bottom) {
        HideOverlay(overlay);
        return;
    }

    const int stripH = Scale(g_stripHeight.load(), dpi);
    const bool wantStrip = !g_compactOnly.load() && !IsZoomed(target) &&
                           frame.top - stripH >= mi.rcWork.top;
    const Layout layout = wantStrip ? Layout::Strip : Layout::Compact;

    int x = 0, y = 0, width = 0;
    if (layout == Layout::Strip) {
        x = frame.left;
        y = frame.top - stripH;
        width = frame.right - frame.left;
    } else {
        int centerY = 0;
        TITLEBARINFO ti{};
        ti.cbSize = sizeof(ti);
        if (GetTitleBarInfo(target, &ti) &&
            ti.rcTitleBar.bottom > ti.rcTitleBar.top &&
            ti.rcTitleBar.bottom - ti.rcTitleBar.top <= Scale(100, dpi)) {
            centerY = (ti.rcTitleBar.top + ti.rcTitleBar.bottom) / 2;
        } else {
            const int border =
                GetSystemMetricsForDpi(SM_CYSIZEFRAME, dpi) +
                GetSystemMetricsForDpi(SM_CXPADDEDBORDER, dpi);
            centerY = frame.top + border +
                      GetSystemMetricsForDpi(SM_CYCAPTION, dpi) / 2;
        }
        x = frame.left + Scale(2, dpi);
        y = centerY - stripH / 2;
        width = Scale(COMPACT_W, dpi);
    }

    const HWND fg = GetForegroundWindow();
    const bool active =
        fg && (fg == target || GetAncestor(fg, GA_ROOTOWNER) == target);
    // Overlap rows hide the window's own top border line and fill the
    // notches left by its rounded corners (Windows 11 only).
    const int winR =
        (layout == Layout::Strip && g_win11) ? Scale(8, dpi) : 0;
    const int coverRows = std::max(1, Scale(1, dpi));
    const int overlap =
        (layout == Layout::Strip) ? std::max(winR, coverRows) : 0;
    const int totalH = stripH + overlap;

    // Match the strip to the real title-bar colour.
    COLORREF bg = s->bg;
    if (layout == Layout::Strip) {
        const DWORD now = GetTickCount();
        const bool first = (s->width == 0) || (s->layout != layout);
        if (first || s->active != active || now - s->lastSample >= 700) {
            COLORREF sampled = 0;
            if (SampleTitleColor(target, frame, dpi, &sampled)) {
                if (first || !NearColor(sampled, s->bg)) {
                    bg = sampled;
                }
            } else if (first) {
                bg = g_dark.load() ? RGB(32, 32, 32) : RGB(243, 243, 243);
            }
            s->lastSample = now;
        }
    }
    const bool dark =
        (layout == Layout::Strip) ? IsDarkColor(bg) : g_dark.load();

    bool needRender = false;
    if (s->layout != layout) {
        s->layout = layout;
        s->hover = -1;
        s->groupHover = false;
        needRender = true;
    }
    if (s->width != width || s->height != totalH || s->stripH != stripH ||
        s->overlap != overlap || s->winRadius != winR || s->dpi != dpi ||
        s->active != active || s->dark != dark || s->bg != bg ||
        s->cornerR != g_cornerRadius.load()) {
        s->width = width;
        s->height = totalH;
        s->stripH = stripH;
        s->overlap = overlap;
        s->winRadius = winR;
        s->dpi = dpi;
        s->active = active;
        s->dark = dark;
        s->bg = bg;
        s->cornerR = g_cornerRadius.load();
        needRender = true;
    }

    SetWindowPos(overlay, nullptr, x, y, width, totalH,
                 SWP_NOACTIVATE | SWP_NOZORDER);
    if (needRender) {
        Render(overlay);
    }
    if (!IsWindowVisible(overlay)) {
        ShowWindow(overlay, SW_SHOWNOACTIVATE);
    }
}

// ----------------------------------------------------- events and scanning

static void CALLBACK WinEventProc(HWINEVENTHOOK, DWORD event, HWND hwnd,
                                  LONG idObject, LONG idChild, DWORD,
                                  DWORD) {
    if (event == EVENT_SYSTEM_FOREGROUND) {
        for (const auto& e : g_entries) {
            PositionOverlay(e.target, e.overlay);
        }
        return;
    }

    if (event != EVENT_OBJECT_LOCATIONCHANGE || idObject != OBJID_WINDOW ||
        idChild != CHILDID_SELF) {
        return;
    }

    HWND overlay = FindOverlay(hwnd);
    if (overlay) {
        PositionOverlay(hwnd, overlay);
    }
}

static BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM) {
    if (!IsTargetWindow(hwnd) || FindOverlay(hwnd)) {
        return TRUE;
    }
    HWND overlay = CreateOverlay(hwnd);
    if (overlay) {
        g_entries.push_back({hwnd, overlay});
        PositionOverlay(hwnd, overlay);
    }
    return TRUE;
}

static void UpdateWindows() {
    g_dark = IsSystemDark();
    EnumWindows(EnumWindowsProc, 0);

    for (auto it = g_entries.begin(); it != g_entries.end();) {
        if (!IsWindow(it->target) || !IsWindow(it->overlay)) {
            if (IsWindow(it->overlay)) {
                DestroyWindow(it->overlay);
            }
            it = g_entries.erase(it);
            continue;
        }
        PositionOverlay(it->target, it->overlay);
        ++it;
    }
}

static DWORD WINAPI WorkerThread(LPVOID) {
    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(wc);
    wc.style = CS_DBLCLKS;
    wc.lpfnWndProc = OverlayProc;
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);

    if (!RegisterClassExW(&wc) &&
        GetLastError() != ERROR_CLASS_ALREADY_EXISTS) {
        Wh_Log(L"RegisterClassExW failed: %lu", GetLastError());
        return 0;
    }

    g_locationHook = SetWinEventHook(
        EVENT_OBJECT_LOCATIONCHANGE, EVENT_OBJECT_LOCATIONCHANGE, nullptr,
        WinEventProc, GetCurrentProcessId(), 0, WINEVENT_OUTOFCONTEXT);

    g_foregroundHook = SetWinEventHook(
        EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, nullptr,
        WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);

    if (!g_locationHook) {
        Wh_Log(L"SetWinEventHook failed; polling only: %lu", GetLastError());
    }

    UpdateWindows();
    DWORD lastScan = GetTickCount();

    while (!g_stop.load()) {
        MSG msg{};
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                g_stop.store(true);
                break;
            }
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
        if (g_stop.load()) {
            break;
        }

        const DWORD interval = g_locationHook ? 500 : 150;
        const DWORD now = GetTickCount();
        if (now - lastScan >= interval) {
            UpdateWindows();
            lastScan = now;
        }

        // Wakes immediately on a message, or when Wh_ModUninit signals stop.
        const DWORD r = MsgWaitForMultipleObjects(1, &g_stopEvent, FALSE,
                                                  interval, QS_ALLINPUT);
        if (r == WAIT_OBJECT_0) {
            break;
        }
    }

    if (g_locationHook) {
        UnhookWinEvent(g_locationHook);
        g_locationHook = nullptr;
    }
    if (g_foregroundHook) {
        UnhookWinEvent(g_foregroundHook);
        g_foregroundHook = nullptr;
    }

    for (const auto& e : g_entries) {
        if (IsWindow(e.overlay)) {
            DestroyWindow(e.overlay);
        }
    }
    g_entries.clear();

    UnregisterClassW(CLASS_NAME, GetModuleHandleW(nullptr));
    return 0;
}

// ------------------------------------------------------------- Windhawk

BOOL Wh_ModInit() {
    LoadSettings();
    g_win11 = IsWindows11();
    g_stop.store(false);

    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_stopEvent) {
        return FALSE;
    }

    g_thread = CreateThread(nullptr, 0, WorkerThread, nullptr, 0, nullptr);
    if (!g_thread) {
        Wh_Log(L"CreateThread failed: %lu", GetLastError());
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
        return FALSE;
    }

    Wh_Log(L"macOS Window Controls 2.1.0 initialized");
    return TRUE;
}

void Wh_ModSettingsChanged() {
    LoadSettings();  // picked up by the next scan / reposition
}

void Wh_ModUninit() {
    g_stop.store(true);
    if (g_stopEvent) {
        SetEvent(g_stopEvent);
    }
    if (g_thread) {
        WaitForSingleObject(g_thread, INFINITE);
        CloseHandle(g_thread);
        g_thread = nullptr;
    }
    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }
    Wh_Log(L"macOS Window Controls unloaded");
}
