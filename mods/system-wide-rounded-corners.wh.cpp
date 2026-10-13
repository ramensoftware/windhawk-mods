// ==WindhawkMod==
// @id              system-wide-rounded-corners
// @name            System-Wide Rounded Corners
// @description     Blacks out the corners of every monitor so the screen looks like it has rounded corners, like a Mac
// @version         1.0
// @author          Meowl
// @github          https://github.com/Darius-Varnelis
// @include         explorer.exe
// @compilerOptions -luser32 -lgdi32 -lshcore -ldwmapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# System-Wide Rounded Corners

![System-Wide Rounded Corners](https://raw.githubusercontent.com/Darius-Varnelis/system-wide-rounded-corners/main/Screen_rounded_corners.png)

Makes your displays look like they have rounded corners, like a MacBook screen.
The pixels in each corner are covered with pure black, anti-aliased overlays.

* Works on every connected monitor and rebuilds itself when monitors are added,
  removed, rearranged or rescaled.
* The overlays are click-through, never take focus, and don't appear in Alt+Tab.
* By default they are hidden from screenshots and screen recordings.
* The mouse cursor slides *under* the rounded corners instead of being drawn
  on top of them, just like on a real rounded display.
* Optionally gets out of the way on fullscreen content (games, videos,
  presentations), per monitor: a fullscreen video on one screen doesn't remove
  the corners on the others.

## Limitations
* Some shell surfaces that live in higher z-order bands (Start menu, Action
  Center, the Alt+Tab switcher) can draw over the corners while they're open.
* Exclusive-fullscreen games bypass the desktop compositor, so the corners
  disappear there. Borderless-windowed games are fine.
* Inside a corner, animated cursors show their first frame only.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- radius: 12
  $name: Corner radius (px)
  $description: Radius at 100% display scaling. 10-16 looks closest to a Mac.
- scaleWithDpi: true
  $name: Scale radius with display scaling
  $description: At 150% scaling a 12 px radius becomes 18 px, and so on
- topCorners: true
  $name: Round top corners
- bottomCorners: true
  $name: Round bottom corners
- cursorUnderCorners: true
  $name: Draw mouse cursor under the corners
  $description: When the cursor reaches a corner it's hidden behind the black mask instead of drawn on top of it
- hideInFullscreen: true
  $name: Hide corners on fullscreen content
  $description: Removes the corners from a monitor while a game, video or other app is fullscreen on it. Ordinary maximized windows keep the corners.
- hideFromCapture: true
  $name: Hide corners from screenshots and recordings
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <dwmapi.h>
#include <shellscalingapi.h>

#include <algorithm>
#include <cstring>
#include <vector>

#ifndef WDA_EXCLUDEFROMCAPTURE
#define WDA_EXCLUDEFROMCAPTURE 0x00000011
#endif

struct {
    int radius;
    bool scaleWithDpi;
    bool topCorners;
    bool bottomCorners;
    bool cursorUnderCorners;
    bool hideInFullscreen;
    bool hideFromCapture;
} g_settings;

constexpr wchar_t kCornerClass[] = L"WhScreenRoundedCorners_Corner";
constexpr wchar_t kControllerClass[] = L"WhScreenRoundedCorners_Controller";
constexpr wchar_t kMutexName[] = L"Local\\WhScreenRoundedCorners";

constexpr UINT WM_APP_RELOAD = WM_APP + 1;
constexpr UINT WM_APP_FULLSCREEN = WM_APP + 3;
constexpr UINT_PTR kTimerZOrder = 1;
constexpr UINT_PTR kTimerRebuild = 2;
constexpr UINT_PTR kTimerCursor = 3;

HINSTANCE g_hInst;
HANDLE g_mutex;
HANDLE g_thread;
HANDLE g_readyEvent;
HWND g_controller;
HWINEVENTHOOK g_foregroundHook;

struct CornerWindow {
    HWND hwnd;
    RECT rect;
    HMONITOR monitor;
    bool hidden;  // hidden because its monitor shows fullscreen content
};
std::vector<CornerWindow> g_corners;

// Watches the foreground app's windows for fullscreen transitions (e.g. F11 or
// a video player's fullscreen button, which don't change the foreground).
HWINEVENTHOOK g_locationHook;
DWORD g_locationHookPid;
bool g_fullscreenCheckPending;

// Cursor stand-in state (overlay thread only; the low-level mouse hook runs on
// this thread too).
HWND g_cursorWnd;
HHOOK g_mouseHook;
HCURSOR g_renderedCursor;
POINT g_cursorHotspot;
SIZE g_cursorSize;
bool g_cursorWndVisible;
bool g_systemCursorHidden;

// Magnification API, loaded dynamically; used only to hide/show the system
// cursor globally.
using MagInitialize_t = BOOL(WINAPI*)();
using MagUninitialize_t = BOOL(WINAPI*)();
using MagShowSystemCursor_t = BOOL(WINAPI*)(BOOL);
HMODULE g_magModule;
MagUninitialize_t g_MagUninitialize;
MagShowSystemCursor_t g_MagShowSystemCursor;
bool g_magReady;

enum Corner { TopLeft = 0, TopRight = 1, BottomLeft = 2, BottomRight = 3 };

void LoadSettings() {
    int r = Wh_GetIntSetting(L"radius");
    g_settings.radius = r < 0 ? 0 : (r > 200 ? 200 : r);
    g_settings.scaleWithDpi = Wh_GetIntSetting(L"scaleWithDpi");
    g_settings.topCorners = Wh_GetIntSetting(L"topCorners");
    g_settings.bottomCorners = Wh_GetIntSetting(L"bottomCorners");
    g_settings.cursorUnderCorners = Wh_GetIntSetting(L"cursorUnderCorners");
    g_settings.hideInFullscreen = Wh_GetIntSetting(L"hideInFullscreen");
    g_settings.hideFromCapture = Wh_GetIntSetting(L"hideFromCapture");
}

// ---------------------------------------------------------------------------
// Magnification (system cursor visibility)
// ---------------------------------------------------------------------------

void InitMagnification() {
    g_magModule = LoadLibraryW(L"Magnification.dll");
    if (!g_magModule) {
        Wh_Log(L"Magnification.dll not available");
        return;
    }
    auto magInitialize = reinterpret_cast<MagInitialize_t>(
        GetProcAddress(g_magModule, "MagInitialize"));
    g_MagUninitialize = reinterpret_cast<MagUninitialize_t>(
        GetProcAddress(g_magModule, "MagUninitialize"));
    g_MagShowSystemCursor = reinterpret_cast<MagShowSystemCursor_t>(
        GetProcAddress(g_magModule, "MagShowSystemCursor"));

    if (magInitialize && g_MagShowSystemCursor && magInitialize()) {
        g_magReady = true;
    } else {
        Wh_Log(L"MagInitialize failed: %u", GetLastError());
    }
}

void UninitMagnification() {
    if (g_magReady && g_MagUninitialize) {
        g_MagUninitialize();
    }
    g_magReady = false;
    if (g_magModule) {
        FreeLibrary(g_magModule);
        g_magModule = nullptr;
    }
}

void SetSystemCursorHidden(bool hide) {
    if (!g_magReady || hide == g_systemCursorHidden) {
        return;
    }
    if (g_MagShowSystemCursor(hide ? FALSE : TRUE)) {
        g_systemCursorHidden = hide;
    } else {
        Wh_Log(L"MagShowSystemCursor failed: %u", GetLastError());
    }
}

// ---------------------------------------------------------------------------
// Corner overlays
// ---------------------------------------------------------------------------

// Builds an r x r premultiplied-alpha bitmap: opaque black outside the
// quarter circle, transparent inside, with 4x4 supersampled anti-aliasing.
HBITMAP CreateCornerBitmap(HDC hdc, int r, int corner) {
    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
    bi.bmiHeader.biWidth = r;
    bi.bmiHeader.biHeight = -r;  // top-down
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;

    void* bits = nullptr;
    HBITMAP bmp =
        CreateDIBSection(hdc, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!bmp || !bits) {
        return nullptr;
    }

    auto* px = static_cast<DWORD*>(bits);
    constexpr int SS = 4;
    const double rr = static_cast<double>(r) * r;

    for (int y = 0; y < r; y++) {
        for (int x = 0; x < r; x++) {
            // Map to top-left-corner coordinates; circle center is (r, r).
            int tx = (corner & 1) ? r - 1 - x : x;
            int ty = (corner & 2) ? r - 1 - y : y;

            int outside = 0;
            for (int sy = 0; sy < SS; sy++) {
                for (int sx = 0; sx < SS; sx++) {
                    double dx = r - (tx + (sx + 0.5) / SS);
                    double dy = r - (ty + (sy + 0.5) / SS);
                    if (dx * dx + dy * dy > rr) {
                        outside++;
                    }
                }
            }

            // Black with premultiplied alpha: only the alpha byte is set.
            DWORD a = outside * 255 / (SS * SS);
            px[y * r + x] = a << 24;
        }
    }

    return bmp;
}

void CreateCornerWindow(HMONITOR monitor, int x, int y, int r, int corner) {
    HWND hwnd = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST | WS_EX_TOOLWINDOW |
            WS_EX_NOACTIVATE,
        kCornerClass, L"", WS_POPUP, x, y, r, r, nullptr, nullptr, g_hInst,
        nullptr);
    if (!hwnd) {
        Wh_Log(L"CreateWindowExW failed: %u", GetLastError());
        return;
    }

    HDC screen = GetDC(nullptr);
    HDC mem = CreateCompatibleDC(screen);
    HBITMAP bmp = CreateCornerBitmap(screen, r, corner);
    if (bmp) {
        HGDIOBJ old = SelectObject(mem, bmp);
        POINT dst{x, y};
        SIZE size{r, r};
        POINT src{0, 0};
        BLENDFUNCTION blend{AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
        UpdateLayeredWindow(hwnd, screen, &dst, &size, mem, &src, 0, &blend,
                            ULW_ALPHA);
        SelectObject(mem, old);
        DeleteObject(bmp);
    }
    DeleteDC(mem);
    ReleaseDC(nullptr, screen);

    if (g_settings.hideFromCapture) {
        SetWindowDisplayAffinity(hwnd, WDA_EXCLUDEFROMCAPTURE);
    }

    ShowWindow(hwnd, SW_SHOWNOACTIVATE);
    g_corners.push_back(
        CornerWindow{hwnd, RECT{x, y, x + r, y + r}, monitor, false});
}

BOOL CALLBACK MonitorEnumProc(HMONITOR monitor, HDC, LPRECT rc, LPARAM) {
    int r = g_settings.radius;
    if (g_settings.scaleWithDpi) {
        UINT dpiX = 96, dpiY = 96;
        if (SUCCEEDED(
                GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY))) {
            r = MulDiv(r, dpiX, 96);
        }
    }

    int w = rc->right - rc->left;
    int h = rc->bottom - rc->top;
    if (r <= 0 || r * 2 > w || r * 2 > h) {
        return TRUE;
    }

    if (g_settings.topCorners) {
        CreateCornerWindow(monitor, rc->left, rc->top, r, TopLeft);
        CreateCornerWindow(monitor, rc->right - r, rc->top, r, TopRight);
    }
    if (g_settings.bottomCorners) {
        CreateCornerWindow(monitor, rc->left, rc->bottom - r, r, BottomLeft);
        CreateCornerWindow(monitor, rc->right - r, rc->bottom - r, r,
                           BottomRight);
    }
    return TRUE;
}

void DestroyCorners() {
    for (const CornerWindow& c : g_corners) {
        DestroyWindow(c.hwnd);
    }
    g_corners.clear();
}

// The cursor stand-in sits just below the corners; everything else is below
// both.
void ReassertTopmost() {
    constexpr UINT flags = SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE |
                           SWP_NOOWNERZORDER | SWP_NOSENDCHANGING;
    if (g_cursorWnd && g_cursorWndVisible) {
        SetWindowPos(g_cursorWnd, HWND_TOPMOST, 0, 0, 0, 0, flags);
    }
    for (const CornerWindow& c : g_corners) {
        SetWindowPos(c.hwnd, HWND_TOPMOST, 0, 0, 0, 0, flags);
    }
}

// ---------------------------------------------------------------------------
// Cursor stand-in
// ---------------------------------------------------------------------------

// Renders the cursor into the stand-in window. Drawing it on black and on
// white recovers per-pixel alpha for every cursor type (alpha, monochrome
// and inverting).
bool RenderCursorImage(HCURSOR cursor) {
    ICONINFO ii{};
    if (!GetIconInfo(cursor, &ii)) {
        return false;
    }

    BITMAP bm{};
    int w = 0, h = 0;
    if (ii.hbmColor && GetObjectW(ii.hbmColor, sizeof(bm), &bm)) {
        w = bm.bmWidth;
        h = bm.bmHeight;
    } else if (ii.hbmMask && GetObjectW(ii.hbmMask, sizeof(bm), &bm)) {
        w = bm.bmWidth;
        h = bm.bmHeight / 2;  // AND mask stacked on XOR mask
    }
    POINT hotspot{static_cast<LONG>(ii.xHotspot),
                  static_cast<LONG>(ii.yHotspot)};
    if (ii.hbmColor) {
        DeleteObject(ii.hbmColor);
    }
    if (ii.hbmMask) {
        DeleteObject(ii.hbmMask);
    }
    if (w <= 0 || h <= 0) {
        return false;
    }

    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
    bi.bmiHeader.biWidth = w;
    bi.bmiHeader.biHeight = -h;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;

    HDC screen = GetDC(nullptr);
    HDC mem = CreateCompatibleDC(screen);
    void *blackBits = nullptr, *whiteBits = nullptr, *outBits = nullptr;
    HBITMAP black =
        CreateDIBSection(screen, &bi, DIB_RGB_COLORS, &blackBits, nullptr, 0);
    HBITMAP white =
        CreateDIBSection(screen, &bi, DIB_RGB_COLORS, &whiteBits, nullptr, 0);
    HBITMAP out =
        CreateDIBSection(screen, &bi, DIB_RGB_COLORS, &outBits, nullptr, 0);

    bool ok = black && white && out && blackBits && whiteBits && outBits;
    if (ok) {
        size_t n = static_cast<size_t>(w) * h;
        memset(blackBits, 0x00, n * 4);
        memset(whiteBits, 0xFF, n * 4);

        HGDIOBJ old = SelectObject(mem, black);
        DrawIconEx(mem, 0, 0, cursor, w, h, 0, nullptr, DI_NORMAL);
        SelectObject(mem, white);
        DrawIconEx(mem, 0, 0, cursor, w, h, 0, nullptr, DI_NORMAL);
        GdiFlush();

        auto* b = static_cast<DWORD*>(blackBits);
        auto* wh = static_cast<DWORD*>(whiteBits);
        auto* o = static_cast<DWORD*>(outBits);
        for (size_t i = 0; i < n; i++) {
            int maxDiff = 0;
            bool inverted = false;
            for (int shift = 0; shift <= 16; shift += 8) {
                int bc = (b[i] >> shift) & 0xFF;
                int wc = (wh[i] >> shift) & 0xFF;
                if (wc < bc) {
                    inverted = true;
                }
                maxDiff = std::max(maxDiff, wc - bc);
            }

            DWORD pixel;
            if (inverted) {
                // XOR (inverting) pixel, e.g. the text I-beam. Near the black
                // corners it would look light, so draw it opaque as it
                // appears on black.
                pixel = 0xFF000000 | (b[i] & 0x00FFFFFF);
            } else {
                DWORD a = 255 - maxDiff;
                pixel = a << 24;
                for (int shift = 0; shift <= 16; shift += 8) {
                    DWORD c = std::min<DWORD>((b[i] >> shift) & 0xFF, a);
                    pixel |= c << shift;
                }
            }
            o[i] = pixel;
        }

        SelectObject(mem, out);
        SIZE size{w, h};
        POINT src{0, 0};
        BLENDFUNCTION blend{AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
        ok = UpdateLayeredWindow(g_cursorWnd, screen, nullptr, &size, mem,
                                 &src, 0, &blend, ULW_ALPHA);
        SelectObject(mem, old);
    }

    DeleteDC(mem);
    if (black) {
        DeleteObject(black);
    }
    if (white) {
        DeleteObject(white);
    }
    if (out) {
        DeleteObject(out);
    }
    ReleaseDC(nullptr, screen);

    if (ok) {
        g_renderedCursor = cursor;
        g_cursorHotspot = hotspot;
        g_cursorSize = SIZE{w, h};
    }
    return ok;
}

void HideCursorStandIn() {
    // Real cursor back first, then remove the copy, to avoid a blink.
    SetSystemCursorHidden(false);
    if (g_cursorWndVisible) {
        ShowWindow(g_cursorWnd, SW_HIDE);
        g_cursorWndVisible = false;
        KillTimer(g_controller, kTimerCursor);
    }
}

// `predicted` is where the cursor is about to move (from the mouse hook, which
// runs before Windows moves and draws the real cursor). Without it, the
// current position is used.
void UpdateCursorOverlay(const POINT* predicted = nullptr) {
    if (!g_settings.cursorUnderCorners || !g_magReady || !g_cursorWnd ||
        g_corners.empty()) {
        HideCursorStandIn();
        return;
    }

    CURSORINFO ci{sizeof(ci)};
    if (!GetCursorInfo(&ci) || !ci.hCursor) {
        HideCursorStandIn();
        return;
    }

    // Respect apps that hide the cursor themselves (only detectable while we
    // aren't the ones hiding it).
    if (!g_systemCursorHidden && !(ci.flags & CURSOR_SHOWING)) {
        HideCursorStandIn();
        return;
    }

    if (ci.hCursor != g_renderedCursor && !RenderCursorImage(ci.hCursor)) {
        HideCursorStandIn();
        return;
    }

    POINT pos = predicted ? *predicted : ci.ptScreenPos;
    RECT cursorRect{pos.x - g_cursorHotspot.x, pos.y - g_cursorHotspot.y, 0,
                    0};
    cursorRect.right = cursorRect.left + g_cursorSize.cx;
    cursorRect.bottom = cursorRect.top + g_cursorSize.cy;

    bool touchesCorner = false;
    for (const CornerWindow& c : g_corners) {
        RECT tmp;
        if (!c.hidden && IntersectRect(&tmp, &cursorRect, &c.rect)) {
            touchesCorner = true;
            break;
        }
    }
    if (!touchesCorner) {
        HideCursorStandIn();
        return;
    }

    SetWindowPos(g_cursorWnd, nullptr, cursorRect.left, cursorRect.top, 0, 0,
                 SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
    if (!g_cursorWndVisible) {
        // Show the copy first, then hide the real cursor, to avoid a blink.
        ShowWindow(g_cursorWnd, SW_SHOWNOACTIVATE);
        g_cursorWndVisible = true;
        ReassertTopmost();
        // Catches shape changes while the mouse is still (e.g. a busy cursor
        // or a resize arrow appearing), at roughly one frame of latency.
        SetTimer(g_controller, kTimerCursor, 16, nullptr);
    }
    SetSystemCursorHidden(true);
}

// The hook reports the target position before Windows clips it to the
// screen; clip it the same way (approximately) so a push into a corner maps
// to the corner pixel.
POINT ClampToMonitor(POINT pt) {
    HMONITOR monitor = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi{sizeof(mi)};
    if (monitor && GetMonitorInfoW(monitor, &mi)) {
        pt.x = std::clamp(pt.x, mi.rcMonitor.left, mi.rcMonitor.right - 1);
        pt.y = std::clamp(pt.y, mi.rcMonitor.top, mi.rcMonitor.bottom - 1);
    }
    return pt;
}

// Cheap pre-check so the hook does almost nothing away from the corners.
bool IsNearCorner(POINT pt) {
    int margin = std::max<int>({g_cursorSize.cx, g_cursorSize.cy, 64});
    for (const CornerWindow& c : g_corners) {
        if (c.hidden) {
            continue;
        }
        RECT zone = c.rect;
        InflateRect(&zone, margin, margin);
        if (PtInRect(&zone, pt)) {
            return true;
        }
    }
    return false;
}

LRESULT CALLBACK MouseHookProc(int code, WPARAM wParam, LPARAM lParam) {
    // Low-level hooks run synchronously on the installing thread (this one)
    // *before* the cursor moves, so swapping to the stand-in here hides the
    // real cursor before it's ever drawn inside a corner. Coordinates are
    // per-monitor-aware physical pixels, matching the corner rects.
    if (code == HC_ACTION && wParam == WM_MOUSEMOVE) {
        auto* info = reinterpret_cast<const MSLLHOOKSTRUCT*>(lParam);
        POINT pt = ClampToMonitor(info->pt);
        if (g_cursorWndVisible || IsNearCorner(pt)) {
            UpdateCursorOverlay(&pt);
        }
    }
    return CallNextHookEx(nullptr, code, wParam, lParam);
}

void UpdateMouseHook() {
    bool want = g_settings.cursorUnderCorners && g_magReady;
    if (want && !g_mouseHook) {
        g_mouseHook =
            SetWindowsHookExW(WH_MOUSE_LL, MouseHookProc, g_hInst, 0);
        if (!g_mouseHook) {
            Wh_Log(L"SetWindowsHookExW failed: %u", GetLastError());
        }
    } else if (!want && g_mouseHook) {
        UnhookWindowsHookEx(g_mouseHook);
        g_mouseHook = nullptr;
    }
}

// ---------------------------------------------------------------------------
// Fullscreen detection
// ---------------------------------------------------------------------------

bool IsShellBackgroundWindow(HWND hwnd) {
    if (hwnd == GetShellWindow()) {
        return true;
    }
    wchar_t cls[64];
    if (!GetClassNameW(hwnd, cls, ARRAYSIZE(cls))) {
        return false;
    }
    return !wcscmp(cls, L"Progman") || !wcscmp(cls, L"WorkerW") ||
           !wcscmp(cls, L"Shell_TrayWnd") ||
           !wcscmp(cls, L"Shell_SecondaryTrayWnd");
}

// Returns the monitor that `hwnd` fills completely with fullscreen content,
// or null. Ordinary maximized windows never count, even with an auto-hiding
// taskbar where they also cover the whole monitor.
HMONITOR GetFullscreenMonitor(HWND hwnd) {
    if (!hwnd) {
        return nullptr;
    }
    hwnd = GetAncestor(hwnd, GA_ROOT);
    if (!hwnd || !IsWindowVisible(hwnd) || IsIconic(hwnd) ||
        IsShellBackgroundWindow(hwnd)) {
        return nullptr;
    }

    DWORD cloaked = 0;
    if (SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &cloaked,
                                        sizeof(cloaked))) &&
        cloaked) {
        return nullptr;
    }

    // Fullscreen content has no window frame at all. Ordinary windows keep
    // their resize frame (WS_THICKFRAME) even when maximized, including apps
    // that draw their own title bar and drop WS_CAPTION (Electron apps,
    // Spotify, Discord, VS Code, ...), which with an auto-hiding taskbar
    // cover the whole monitor when maximized. Real fullscreen modes strip
    // both: browser F11 and video fullscreen (which can stay WS_MAXIMIZE
    // while doing so), borderless games and video players.
    LONG style = GetWindowLongW(hwnd, GWL_STYLE);
    if ((style & WS_THICKFRAME) || (style & WS_CAPTION) == WS_CAPTION) {
        return nullptr;
    }

    RECT wr;
    if (!GetWindowRect(hwnd, &wr)) {
        return nullptr;
    }
    HMONITOR monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONULL);
    MONITORINFO mi{sizeof(mi)};
    if (!monitor || !GetMonitorInfoW(monitor, &mi)) {
        return nullptr;
    }
    const RECT& m = mi.rcMonitor;
    if (wr.left <= m.left && wr.top <= m.top && wr.right >= m.right &&
        wr.bottom >= m.bottom) {
        return monitor;
    }
    return nullptr;
}

void UpdateFullscreenState() {
    g_fullscreenCheckPending = false;

    std::vector<HMONITOR> fullscreen;
    if (g_settings.hideInFullscreen) {
        // The foreground app, plus whatever is on top of each monitor's
        // center (covers a fullscreen video left playing on another screen
        // while you work on this one).
        if (HMONITOR m = GetFullscreenMonitor(GetForegroundWindow())) {
            fullscreen.push_back(m);
        }
        for (const CornerWindow& c : g_corners) {
            if (std::find(fullscreen.begin(), fullscreen.end(), c.monitor) !=
                fullscreen.end()) {
                continue;
            }
            MONITORINFO mi{sizeof(mi)};
            if (!GetMonitorInfoW(c.monitor, &mi)) {
                continue;
            }
            POINT center{(mi.rcMonitor.left + mi.rcMonitor.right) / 2,
                         (mi.rcMonitor.top + mi.rcMonitor.bottom) / 2};
            if (GetFullscreenMonitor(WindowFromPoint(center)) == c.monitor) {
                fullscreen.push_back(c.monitor);
            }
        }
    }

    bool changed = false;
    for (CornerWindow& c : g_corners) {
        bool hide = std::find(fullscreen.begin(), fullscreen.end(),
                              c.monitor) != fullscreen.end();
        if (hide != c.hidden) {
            ShowWindow(c.hwnd, hide ? SW_HIDE : SW_SHOWNOACTIVATE);
            c.hidden = hide;
            changed = true;
        }
    }
    if (changed) {
        UpdateCursorOverlay();
    }
}

void CALLBACK LocationChangeProc(HWINEVENTHOOK, DWORD, HWND hwnd,
                                 LONG idObject, LONG idChild, DWORD, DWORD) {
    // Only top-level window moves/resizes; ignore carets, cursors, controls.
    if (!hwnd || idObject != OBJID_WINDOW || idChild != CHILDID_SELF ||
        GetAncestor(hwnd, GA_ROOT) != hwnd) {
        return;
    }
    if (!g_fullscreenCheckPending) {
        g_fullscreenCheckPending = true;
        PostMessageW(g_controller, WM_APP_FULLSCREEN, 0, 0);
    }
}

// Scopes the location-change hook to the foreground process so we aren't
// woken up by every window move on the system.
void WatchForegroundProcess() {
    DWORD pid = 0;
    if (g_settings.hideInFullscreen) {
        if (HWND fg = GetForegroundWindow()) {
            GetWindowThreadProcessId(fg, &pid);
        }
    }
    if (pid == g_locationHookPid) {
        return;
    }
    if (g_locationHook) {
        UnhookWinEvent(g_locationHook);
        g_locationHook = nullptr;
    }
    g_locationHookPid = pid;
    if (pid) {
        g_locationHook = SetWinEventHook(
            EVENT_OBJECT_LOCATIONCHANGE, EVENT_OBJECT_LOCATIONCHANGE, nullptr,
            LocationChangeProc, pid, 0,
            WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
    }
}

// ---------------------------------------------------------------------------
// Controller
// ---------------------------------------------------------------------------

void RebuildCorners() {
    DestroyCorners();
    EnumDisplayMonitors(nullptr, nullptr, MonitorEnumProc, 0);
    Wh_Log(L"Created %zu corner windows", g_corners.size());
    UpdateFullscreenState();
    UpdateCursorOverlay();
}

void CALLBACK ForegroundProc(HWINEVENTHOOK, DWORD, HWND, LONG, LONG, DWORD,
                             DWORD) {
    ReassertTopmost();
    WatchForegroundProcess();
    UpdateFullscreenState();
}

LRESULT CALLBACK ControllerWndProc(HWND hwnd, UINT msg, WPARAM wParam,
                                   LPARAM lParam) {
    switch (msg) {
        case WM_CREATE:
            SetTimer(hwnd, kTimerZOrder, 1000, nullptr);
            return 0;

        // Monitors added/removed/moved or scaling changed: debounce, since
        // these arrive in bursts while Windows reconfigures displays.
        case WM_DISPLAYCHANGE:
        case WM_SETTINGCHANGE:
            SetTimer(hwnd, kTimerRebuild, 500, nullptr);
            break;

        case WM_TIMER:
            if (wParam == kTimerZOrder) {
                ReassertTopmost();
                UpdateFullscreenState();  // safety net for missed events
            } else if (wParam == kTimerRebuild) {
                KillTimer(hwnd, kTimerRebuild);
                g_renderedCursor = nullptr;  // cursor size may follow DPI
                RebuildCorners();
            } else if (wParam == kTimerCursor) {
                UpdateCursorOverlay();
            }
            return 0;

        case WM_APP_FULLSCREEN:
            UpdateFullscreenState();
            return 0;

        case WM_APP_RELOAD:
            LoadSettings();
            UpdateMouseHook();
            WatchForegroundProcess();
            RebuildCorners();
            return 0;

        case WM_DESTROY:
            HideCursorStandIn();
            KillTimer(hwnd, kTimerZOrder);
            KillTimer(hwnd, kTimerRebuild);
            if (g_mouseHook) {
                UnhookWindowsHookEx(g_mouseHook);
                g_mouseHook = nullptr;
            }
            if (g_locationHook) {
                UnhookWinEvent(g_locationHook);
                g_locationHook = nullptr;
                g_locationHookPid = 0;
            }
            DestroyCorners();
            if (g_cursorWnd) {
                DestroyWindow(g_cursorWnd);
                g_cursorWnd = nullptr;
            }
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

DWORD WINAPI OverlayThread(LPVOID) {
    // Physical-pixel coordinates on every monitor.
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    InitMagnification();

    WNDCLASSW wc{};
    wc.lpfnWndProc = DefWindowProcW;
    wc.hInstance = g_hInst;
    wc.lpszClassName = kCornerClass;
    RegisterClassW(&wc);

    wc.lpfnWndProc = ControllerWndProc;
    wc.lpszClassName = kControllerClass;
    RegisterClassW(&wc);

    // Hidden top-level window (not message-only) so it receives
    // WM_DISPLAYCHANGE / WM_SETTINGCHANGE broadcasts.
    g_controller = CreateWindowExW(WS_EX_TOOLWINDOW, kControllerClass, L"",
                                   WS_POPUP, 0, 0, 0, 0, nullptr, nullptr,
                                   g_hInst, nullptr);

    // The cursor copy is never captured: real cursors aren't part of
    // screenshots either.
    g_cursorWnd = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST | WS_EX_TOOLWINDOW |
            WS_EX_NOACTIVATE,
        kCornerClass, L"", WS_POPUP, 0, 0, 1, 1, nullptr, nullptr, g_hInst,
        nullptr);
    if (g_cursorWnd) {
        SetWindowDisplayAffinity(g_cursorWnd, WDA_EXCLUDEFROMCAPTURE);
    }

    g_foregroundHook =
        SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND,
                        nullptr, ForegroundProc, 0, 0, WINEVENT_OUTOFCONTEXT);

    if (g_controller) {
        UpdateMouseHook();
        WatchForegroundProcess();
        RebuildCorners();
    }
    SetEvent(g_readyEvent);

    MSG msg;
    while (g_controller && GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    if (g_foregroundHook) {
        UnhookWinEvent(g_foregroundHook);
        g_foregroundHook = nullptr;
    }
    UninitMagnification();
    UnregisterClassW(kCornerClass, g_hInst);
    UnregisterClassW(kControllerClass, g_hInst);
    return 0;
}

// Explorer can run several processes (e.g. separate folder windows); only the
// one hosting the desktop/taskbar should draw the corners.
bool IsShellProcess() {
    HWND shell = GetShellWindow();
    if (shell) {
        DWORD pid = 0;
        GetWindowThreadProcessId(shell, &pid);
        return pid == GetCurrentProcessId();
    }
    PCWSTR cmd = GetCommandLineW();
    return !wcsstr(cmd, L"/factory") && !wcsstr(cmd, L"-Embedding");
}

BOOL Wh_ModInit() {
    if (!IsShellProcess()) {
        return TRUE;
    }

    g_mutex = CreateMutexW(nullptr, FALSE, kMutexName);
    if (!g_mutex || GetLastError() == ERROR_ALREADY_EXISTS) {
        if (g_mutex) {
            CloseHandle(g_mutex);
            g_mutex = nullptr;
        }
        return TRUE;
    }

    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       reinterpret_cast<LPCWSTR>(&Wh_ModInit), &g_hInst);

    LoadSettings();

    g_readyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_thread = CreateThread(nullptr, 0, OverlayThread, nullptr, 0, nullptr);
    if (g_thread) {
        WaitForSingleObject(g_readyEvent, 5000);
    }
    return TRUE;
}

void Wh_ModUninit() {
    if (g_thread) {
        if (g_controller) {
            PostMessageW(g_controller, WM_CLOSE, 0, 0);
        }
        WaitForSingleObject(g_thread, 5000);
        CloseHandle(g_thread);
        g_thread = nullptr;
    }
    g_controller = nullptr;
    if (g_readyEvent) {
        CloseHandle(g_readyEvent);
        g_readyEvent = nullptr;
    }
    if (g_mutex) {
        CloseHandle(g_mutex);
        g_mutex = nullptr;
    }
}

void Wh_ModSettingsChanged() {
    if (g_controller) {
        PostMessageW(g_controller, WM_APP_RELOAD, 0, 0);
    }
}
