// ==WindhawkMod==
// @id              maximized-window-margins
// @name            Maximized Window Margins
// @description     Leaves a configurable gap around maximized windows and between snapped windows, macOS style
// @version         1.1
// @author          Mário Barbosa
// @github          https://github.com/mariogomesbarbosa
// @include         windhawk.exe
// @compilerOptions -lshcore -ldwmapi
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Maximized Window Margins

![Screenshot](https://raw.githubusercontent.com/mariogomesbarbosa/window-maximized-margin/main/images/preview.png)

Maximized windows no longer touch the screen edges. A configurable gap is kept
on every side, which pairs nicely with a floating dock (for example the
*Luminosity* or *DockLike* themes of **Windows 11 Taskbar Styler**). Windows
arranged side by side with Snap / Snap Layouts get a configurable gap between
them too.

## How it works

The mod runs as a *tool mod* (in its own `windhawk.exe` process, nothing is
injected into other programs). For each monitor and each edge with a margin
greater than zero, it registers an invisible
[AppBar](https://learn.microsoft.com/windows/win32/shell/application-desktop-toolbars)
of the configured thickness. Windows then reduces the work area, and every
maximized window fits inside it.

* Margins are set in DIPs and scaled per monitor DPI.
* AppBars are registered again when Explorer restarts, when displays change,
  and when the taskbar or another AppBar moves.
* Disabling the mod restores the original work area immediately.

## Gaps between snapped windows

![Gaps between snapped windows](https://raw.githubusercontent.com/mariogomesbarbosa/window-maximized-margin/main/images/snapped-windows.webp)

When two or more windows share the screen through Snap / Snap Layouts (halves,
thirds, quarters...), the mod keeps the "Gap between snapped windows" between
them. Edges that touch the screen keep the regular margins.

* Resizing works as usual: drag the gap itself, or the edge of a snapped
  window, and the windows on both sides resize together while the gap stays
  the same. The Windows 11 divider only shows up while snapped windows touch,
  so the mod provides its own, which works with any gap size.
* Set the gap to 0 to turn this off.
* Disabling the mod puts snapped windows back where Snap placed them.

Under the hood, the mod watches window moves through an out-of-context
WinEvent hook (still nothing injected) and fills each gap with an invisible
window that handles the dragging.

## Side effects

Since the work area itself shrinks, the gap also applies to:

* Snap / Snap Layouts (snapped windows keep the margin too).
* Desktop icon placement.
* Anything else that positions itself from the work area, such as pop-ups and
  notifications that apps place in a corner of the screen.

Real fullscreen apps (games, videos, F11) cover the whole monitor and are not
affected.

## Tips

* With a floating dock, the taskbar still reserves its own height at the
  bottom. The bottom margin is added on top of it, so a small value (0–10)
  is usually enough.
* Maximized windows have square corners in Windows 11. To round them, install
  *Custom Window Corner Radius* and set "Rounded corners for maximized and
  snapped windows" to "Round snapped and maximized windows". The "Corner
  radius" setting of that mod then applies to maximized windows too.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- top: 10
  $name: Top margin
  $description: Gap in pixels (at 100% scaling) between the top edge of the screen and maximized windows. 0 to 200.
- bottom: 10
  $name: Bottom margin
  $description: Gap in pixels (at 100% scaling) between the taskbar (or the bottom edge of the screen) and maximized windows. 0 to 200.
- left: 10
  $name: Left margin
  $description: Gap in pixels (at 100% scaling) between the left edge of the screen and maximized windows. 0 to 200.
- right: 10
  $name: Right margin
  $description: Gap in pixels (at 100% scaling) between the right edge of the screen and maximized windows. 0 to 200.
- snapGap: 10
  $name: Gap between snapped windows
  $description: Gap in pixels (at 100% scaling) between windows arranged side by side with Snap / Snap Layouts. Edges that touch the screen keep the margins above. 0 to 200, 0 disables it.
- monitors: all
  $name: Monitors
  $options:
  - all: All monitors
  - primary: Primary monitor only
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <windows.h>

#include <dwmapi.h>
#include <shellapi.h>
#include <shellscalingapi.h>

#include <algorithm>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
// Settings

struct Settings {
    int margin[4];  // Indexed by ABE_LEFT, ABE_TOP, ABE_RIGHT, ABE_BOTTOM.
    int snapGap;
    bool primaryOnly;
};

Settings g_settings;

constexpr int kMaxMargin = 200;

int ClampMargin(int value) {
    if (value < 0) {
        return 0;
    }
    if (value > kMaxMargin) {
        return kMaxMargin;
    }
    return value;
}

void LoadSettings() {
    g_settings.margin[ABE_LEFT] = ClampMargin(Wh_GetIntSetting(L"left"));
    g_settings.margin[ABE_TOP] = ClampMargin(Wh_GetIntSetting(L"top"));
    g_settings.margin[ABE_RIGHT] = ClampMargin(Wh_GetIntSetting(L"right"));
    g_settings.margin[ABE_BOTTOM] = ClampMargin(Wh_GetIntSetting(L"bottom"));
    g_settings.snapGap = ClampMargin(Wh_GetIntSetting(L"snapGap"));

    g_settings.primaryOnly =
        wcscmp(WindhawkUtils::StringSetting::make(L"monitors"), L"primary") ==
        0;
}

////////////////////////////////////////////////////////////////////////////////
// AppBar management. Everything below runs on the worker thread only.

constexpr WCHAR kWindowClass[] = L"WindhawkMaximizedWindowMargins_" WH_MOD_ID;

// Callback message for AppBar notifications (ABN_*).
constexpr UINT kAppBarCallbackMsg = WM_APP + 1;
// Posted by the mod callbacks to request a rebuild with fresh settings.
constexpr UINT kReloadSettingsMsg = WM_APP + 2;

constexpr UINT_PTR kRebuildTimerId = 1;
constexpr UINT_PTR kSyncTimerId = 2;
constexpr UINT_PTR kSnapTimerId = 3;
constexpr UINT kDebounceMs = 300;
// Lets Snap finish placing every window of a layout before adding the gap.
constexpr UINT kSnapDelayMs = 100;

struct AppBar {
    HWND hwnd;
    HMONITOR monitor;
    UINT edge;
    int thickness;  // Physical pixels.
    RECT rect;      // Last rect passed to ABM_SETPOS.
};

DWORD g_threadId;
HANDLE g_thread;
HWND g_controlWnd;
UINT g_taskbarCreatedMsg;
std::vector<AppBar> g_appBars;
bool g_positioning;

int ScaleForMonitor(int dips, HMONITOR monitor) {
    UINT dpiX = 96, dpiY = 96;
    if (FAILED(GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY))) {
        dpiX = 96;
    }
    return MulDiv(dips, dpiX, 96);
}

// Computes the AppBar rect for its edge, letting the shell move it out of the
// way of the taskbar and other AppBars. Returns false on failure.
bool QueryAppBarRect(const AppBar& bar, RECT* result) {
    MONITORINFO mi{.cbSize = sizeof(mi)};
    if (!GetMonitorInfo(bar.monitor, &mi)) {
        return false;
    }

    APPBARDATA abd{
        .cbSize = sizeof(abd),
        .hWnd = bar.hwnd,
        .uEdge = bar.edge,
        .rc = mi.rcMonitor,
    };

    // Proposed rect: a strip along the edge of the full monitor rect.
    switch (bar.edge) {
        case ABE_LEFT:
            abd.rc.right = abd.rc.left + bar.thickness;
            break;
        case ABE_TOP:
            abd.rc.bottom = abd.rc.top + bar.thickness;
            break;
        case ABE_RIGHT:
            abd.rc.left = abd.rc.right - bar.thickness;
            break;
        case ABE_BOTTOM:
            abd.rc.top = abd.rc.bottom - bar.thickness;
            break;
    }

    SHAppBarMessage(ABM_QUERYPOS, &abd);

    // The shell moves the edge that touches the screen border (e.g. above the
    // taskbar). Recompute the opposite side so the thickness is preserved.
    switch (bar.edge) {
        case ABE_LEFT:
            abd.rc.right = abd.rc.left + bar.thickness;
            break;
        case ABE_TOP:
            abd.rc.bottom = abd.rc.top + bar.thickness;
            break;
        case ABE_RIGHT:
            abd.rc.left = abd.rc.right - bar.thickness;
            break;
        case ABE_BOTTOM:
            abd.rc.top = abd.rc.bottom - bar.thickness;
            break;
    }

    *result = abd.rc;
    return true;
}

void PositionAppBar(AppBar& bar) {
    RECT rc;
    if (!QueryAppBarRect(bar, &rc)) {
        return;
    }

    // Avoid ABM_SETPOS when nothing changed: each SETPOS notifies every other
    // AppBar, so unconditional updates would ping-pong forever.
    if (EqualRect(&rc, &bar.rect)) {
        return;
    }

    APPBARDATA abd{
        .cbSize = sizeof(abd),
        .hWnd = bar.hwnd,
        .uEdge = bar.edge,
        .rc = rc,
    };
    SHAppBarMessage(ABM_SETPOS, &abd);

    bar.rect = abd.rc;
    SetWindowPos(bar.hwnd, nullptr, abd.rc.left, abd.rc.top,
                 abd.rc.right - abd.rc.left, abd.rc.bottom - abd.rc.top,
                 SWP_NOZORDER | SWP_NOACTIVATE);

    Wh_Log(L"AppBar edge=%u rect=(%d,%d,%d,%d)", bar.edge, abd.rc.left,
           abd.rc.top, abd.rc.right, abd.rc.bottom);
}

void PositionAllAppBars() {
    g_positioning = true;
    for (auto& bar : g_appBars) {
        PositionAppBar(bar);
    }
    g_positioning = false;
}

void DestroyAppBars() {
    for (auto& bar : g_appBars) {
        APPBARDATA abd{.cbSize = sizeof(abd), .hWnd = bar.hwnd};
        SHAppBarMessage(ABM_REMOVE, &abd);
        DestroyWindow(bar.hwnd);
    }
    g_appBars.clear();
}

HWND CreateHiddenWindow() {
    // Never shown. WS_EX_TOOLWINDOW keeps it out of Alt+Tab and the taskbar.
    return CreateWindowEx(WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE, kWindowClass,
                          nullptr, WS_POPUP, 0, 0, 0, 0, nullptr, nullptr,
                          GetModuleHandle(nullptr), nullptr);
}

struct DesiredAppBar {
    HMONITOR monitor;
    UINT edge;
    int thickness;
};

BOOL CALLBACK EnumMonitorsProc(HMONITOR monitor, HDC, LPRECT, LPARAM param) {
    auto& desired = *reinterpret_cast<std::vector<DesiredAppBar>*>(param);

    if (g_settings.primaryOnly) {
        MONITORINFO mi{.cbSize = sizeof(mi)};
        if (!GetMonitorInfo(monitor, &mi) ||
            !(mi.dwFlags & MONITORINFOF_PRIMARY)) {
            return TRUE;
        }
    }

    for (UINT edge = ABE_LEFT; edge <= ABE_BOTTOM; edge++) {
        if (g_settings.margin[edge] == 0) {
            continue;
        }

        desired.push_back({
            .monitor = monitor,
            .edge = edge,
            .thickness = ScaleForMonitor(g_settings.margin[edge], monitor),
        });
    }

    return TRUE;
}

void CreateAppBar(const DesiredAppBar& desired) {
    HWND hwnd = CreateHiddenWindow();
    if (!hwnd) {
        Wh_Log(L"CreateWindowEx failed: %u", GetLastError());
        return;
    }

    APPBARDATA abd{
        .cbSize = sizeof(abd),
        .hWnd = hwnd,
        .uCallbackMessage = kAppBarCallbackMsg,
    };
    if (!SHAppBarMessage(ABM_NEW, &abd)) {
        Wh_Log(L"ABM_NEW failed for edge %u", desired.edge);
        DestroyWindow(hwnd);
        return;
    }

    g_appBars.push_back({
        .hwnd = hwnd,
        .monitor = desired.monitor,
        .edge = desired.edge,
        .thickness = desired.thickness,
        .rect = {},
    });
}

// Brings the AppBars in line with the settings and the connected monitors.
// Only the bars that differ are touched, so the work area changes once per
// affected edge and maximized windows don't jump to full screen and back.
void SyncAppBars() {
    std::vector<DesiredAppBar> desired;
    EnumDisplayMonitors(nullptr, nullptr, EnumMonitorsProc,
                        reinterpret_cast<LPARAM>(&desired));

    g_positioning = true;

    std::erase_if(g_appBars, [&](const AppBar& bar) {
        bool wanted = std::ranges::any_of(desired, [&](const auto& d) {
            return d.monitor == bar.monitor && d.edge == bar.edge;
        });
        if (!wanted) {
            APPBARDATA abd{.cbSize = sizeof(abd), .hWnd = bar.hwnd};
            SHAppBarMessage(ABM_REMOVE, &abd);
            DestroyWindow(bar.hwnd);
        }
        return !wanted;
    });

    for (const auto& d : desired) {
        auto it = std::ranges::find_if(g_appBars, [&](const AppBar& bar) {
            return bar.monitor == d.monitor && bar.edge == d.edge;
        });
        if (it != g_appBars.end()) {
            it->thickness = d.thickness;
        } else {
            CreateAppBar(d);
        }
    }

    // ABM_SETPOS goes only to new bars and bars whose rect changed.
    PositionAllAppBars();
}

// Registers every AppBar from scratch. Needed after Explorer restarts, since
// the new taskbar doesn't know about the old registrations.
void RebuildAppBars() {
    DestroyAppBars();
    SyncAppBars();
}

////////////////////////////////////////////////////////////////////////////////
// Gaps between snapped windows. Also runs on the worker thread only.

constexpr WCHAR kSplitterClass[] =
    L"WindhawkMaximizedWindowMargins_Splitter_" WH_MOD_ID;

// Not declared in the headers shipped with Windhawk. Windows 10 1903+.
using IsWindowArranged_t = BOOL(WINAPI*)(HWND hwnd);
IsWindowArranged_t g_pIsWindowArranged;

// Some windows report a frame a pixel off the work area or their neighbor.
constexpr int kEdgeTolerance = 2;
// Smallest size a window can be shrunk to when resizing a pair together.
constexpr int kMinSnappedSize = 100;

struct SnappedWindow {
    HWND hwnd;
    RECT snapRect;  // Visible frame where Snap placed the window.
    RECT gapRect;   // Visible frame after the gap was applied.
};

// A window across one edge of the window being resized.
struct LinkedWindow {
    UINT edge;  // Edge of the resized window, ABE_*.
    HWND hwnd;
    RECT startRect;
};

// The user resizes a snapped window by its own border.
struct ResizeSession {
    HWND hwnd;
    RECT startRect;
    std::vector<LinkedWindow> linked;
    bool resized;
};

// Invisible window filling the gap between two snapped windows, so the gap
// can be dragged to resize both, whatever its size.
struct Splitter {
    HWND hwnd;
    UINT edge;     // ABE_RIGHT: vertical line, ABE_BOTTOM: horizontal line.
    HWND first;    // Window on the left of / above the line.
    HWND second;   // Window on the right of / below the line.
};

struct SplitterDrag {
    HWND hwnd;
    UINT edge;
    LONG startCursor;
    LONG startPos;  // `edge` of the windows before the line.
    LONG minPos;
    LONG maxPos;
    int gap;
    std::vector<std::pair<HWND, RECT>> before;
    std::vector<std::pair<HWND, RECT>> after;
};

std::vector<HWINEVENTHOOK> g_winEventHooks;
HWND g_moveSizeWnd;  // Window currently dragged or resized by the user.
std::vector<SnappedWindow> g_snappedWindows;
std::vector<HWND> g_pendingWindows;
ResizeSession g_resize;
std::vector<Splitter> g_splitters;
SplitterDrag g_splitterDrag;

UINT OppositeEdge(UINT edge) {
    return (edge + 2) % 4;
}

bool IsLeadingEdge(UINT edge) {
    return edge == ABE_LEFT || edge == ABE_TOP;
}

LONG GetEdge(const RECT& rc, UINT edge) {
    switch (edge) {
        case ABE_LEFT:
            return rc.left;
        case ABE_TOP:
            return rc.top;
        case ABE_RIGHT:
            return rc.right;
        default:
            return rc.bottom;
    }
}

void SetEdge(RECT& rc, UINT edge, LONG value) {
    switch (edge) {
        case ABE_LEFT:
            rc.left = value;
            break;
        case ABE_TOP:
            rc.top = value;
            break;
        case ABE_RIGHT:
            rc.right = value;
            break;
        default:
            rc.bottom = value;
            break;
    }
}

// Snapped windows touch each other by their visible frame, which excludes the
// invisible resize borders included in GetWindowRect.
bool GetFrameRect(HWND hwnd, RECT* rect) {
    return SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_EXTENDED_FRAME_BOUNDS,
                                           rect, sizeof(*rect)));
}

void SetFrameRect(HWND hwnd, const RECT& frame, UINT extraFlags = 0) {
    RECT window, current;
    if (!GetWindowRect(hwnd, &window) || !GetFrameRect(hwnd, &current)) {
        return;
    }

    RECT target{
        .left = frame.left - (current.left - window.left),
        .top = frame.top - (current.top - window.top),
        .right = frame.right + (window.right - current.right),
        .bottom = frame.bottom + (window.bottom - current.bottom),
    };
    SetWindowPos(hwnd, nullptr, target.left, target.top,
                 target.right - target.left, target.bottom - target.top,
                 SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE | extraFlags);
}

bool IsSnapped(HWND hwnd) {
    return g_pIsWindowArranged && IsWindowVisible(hwnd) && !IsIconic(hwnd) &&
           !IsZoomed(hwnd) && g_pIsWindowArranged(hwnd);
}

// Windows on another virtual desktop are cloaked.
bool IsCloaked(HWND hwnd) {
    DWORD cloaked = 0;
    return SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &cloaked,
                                           sizeof(cloaked))) &&
           cloaked;
}

auto FindSnappedWindow(HWND hwnd) {
    return std::ranges::find(g_snappedWindows, hwnd, &SnappedWindow::hwnd);
}

int GetGapForWindow(HWND hwnd) {
    return ScaleForMonitor(g_settings.snapGap,
                           MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST));
}

// Whether `other` sits across `edge` of `rect`, one gap away, sharing part of
// that edge.
bool IsAcrossEdge(const RECT& rect, UINT edge, const RECT& other, int gap) {
    LONG distance = GetEdge(other, OppositeEdge(edge)) - GetEdge(rect, edge);
    if (IsLeadingEdge(edge)) {
        distance = -distance;
    }
    if (abs(distance - gap) > kEdgeTolerance) {
        return false;
    }

    if (edge == ABE_LEFT || edge == ABE_RIGHT) {
        return rect.top < other.bottom && other.top < rect.bottom;
    }
    return rect.left < other.right && other.left < rect.right;
}

// Per-edge insets, indexed by ABE_*: half of the gap on the edges that face
// another window, 0 on the edges that touch the work area. Returns false if
// the gap doesn't apply to the window's monitor.
bool GetGapInsets(HWND hwnd, const RECT& frame, int insets[4]) {
    HMONITOR monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi{.cbSize = sizeof(mi)};
    if (!GetMonitorInfo(monitor, &mi)) {
        return false;
    }

    if (g_settings.primaryOnly && !(mi.dwFlags & MONITORINFOF_PRIMARY)) {
        return false;
    }

    int gap = ScaleForMonitor(g_settings.snapGap, monitor);
    // Odd gaps: left/top edges take the extra pixel, so that both halves of a
    // shared edge always add up to the full gap.
    int leadingInset = gap - gap / 2;
    int trailingInset = gap / 2;

    const RECT& work = mi.rcWork;
    insets[ABE_LEFT] =
        frame.left - work.left > kEdgeTolerance ? leadingInset : 0;
    insets[ABE_TOP] = frame.top - work.top > kEdgeTolerance ? leadingInset : 0;
    insets[ABE_RIGHT] =
        work.right - frame.right > kEdgeTolerance ? trailingInset : 0;
    insets[ABE_BOTTOM] =
        work.bottom - frame.bottom > kEdgeTolerance ? trailingInset : 0;
    return true;
}

// Finds a tracked window facing `edge` of a freshly snapped window, up to one
// gap away. Snap places a window right against the current frame of its
// neighbor, which already has its half of the gap applied.
bool FindFacingEdge(HWND hwnd,
                    const RECT& rect,
                    UINT edge,
                    int gap,
                    LONG* facingEdge) {
    UINT opposite = OppositeEdge(edge);
    bool vertical = edge == ABE_LEFT || edge == ABE_RIGHT;

    for (const auto& w : g_snappedWindows) {
        if (w.hwnd == hwnd) {
            continue;
        }

        const RECT& other = w.gapRect;
        bool overlaps = vertical
                            ? rect.top < other.bottom && other.top < rect.bottom
                            : rect.left < other.right && other.left < rect.right;
        if (!overlaps) {
            continue;
        }

        LONG distance = GetEdge(other, opposite) - GetEdge(rect, edge);
        if (IsLeadingEdge(edge)) {
            distance = -distance;
        }
        if (distance >= -kEdgeTolerance && distance <= gap + kEdgeTolerance) {
            *facingEdge = GetEdge(other, opposite);
            return true;
        }
    }

    return false;
}

// Pulls in the edges of a snapped window that face another window. Returns
// false if there's nothing to change.
bool ComputeGapRect(HWND hwnd, const RECT& snapRect, RECT* result) {
    int insets[4];
    if (!GetGapInsets(hwnd, snapRect, insets)) {
        return false;
    }

    RECT rc{
        .left = snapRect.left + insets[ABE_LEFT],
        .top = snapRect.top + insets[ABE_TOP],
        .right = snapRect.right - insets[ABE_RIGHT],
        .bottom = snapRect.bottom - insets[ABE_BOTTOM],
    };

    // Next to a window that already has the gap, keep the full gap from it
    // rather than half of it from the shared line.
    int gap = GetGapForWindow(hwnd);
    for (UINT edge = ABE_LEFT; edge <= ABE_BOTTOM; edge++) {
        LONG facingEdge;
        if (insets[edge] &&
            FindFacingEdge(hwnd, snapRect, edge, gap, &facingEdge)) {
            SetEdge(rc, edge,
                    IsLeadingEdge(edge) ? facingEdge + gap : facingEdge - gap);
        }
    }

    if (EqualRect(&rc, &snapRect) || rc.right <= rc.left ||
        rc.bottom <= rc.top) {
        return false;
    }

    *result = rc;
    return true;
}

// The reverse of ComputeGapRect, to restore windows resized with the gap.
RECT ComputeSnapRect(HWND hwnd, const RECT& gapRect) {
    int insets[4];
    if (!GetGapInsets(hwnd, gapRect, insets)) {
        return gapRect;
    }

    return {
        .left = gapRect.left - insets[ABE_LEFT],
        .top = gapRect.top - insets[ABE_TOP],
        .right = gapRect.right + insets[ABE_RIGHT],
        .bottom = gapRect.bottom + insets[ABE_BOTTOM],
    };
}

// Moves a tracked window while resizing a pair together, and records where it
// actually ended up (it may enforce a minimum size) as ours.
void SetTrackedFrame(HWND hwnd, RECT frame) {
    auto it = FindSnappedWindow(hwnd);
    if (it == g_snappedWindows.end()) {
        return;
    }

    if (EqualRect(&frame, &it->gapRect)) {
        return;
    }

    SetFrameRect(hwnd, frame);
    GetFrameRect(hwnd, &frame);

    it->gapRect = frame;
    it->snapRect = ComputeSnapRect(hwnd, frame);
}

void UpdateSnappedWindow(HWND hwnd) {
    auto it = FindSnappedWindow(hwnd);

    RECT frame;
    if (!IsSnapped(hwnd) || !GetFrameRect(hwnd, &frame)) {
        if (it != g_snappedWindows.end()) {
            g_snappedWindows.erase(it);
        }
        return;
    }

    // Our own move, or nothing changed since.
    if (it != g_snappedWindows.end() && EqualRect(&frame, &it->gapRect)) {
        return;
    }

    // Anything else is a fresh placement by Snap.
    RECT gapRect;
    if (!ComputeGapRect(hwnd, frame, &gapRect)) {
        if (it != g_snappedWindows.end()) {
            g_snappedWindows.erase(it);
        }
        return;
    }

    SetFrameRect(hwnd, gapRect);

    // The window may enforce a minimum size or refuse the move (e.g. it runs
    // elevated). Remember where it actually is, so the location change caused
    // by this move is recognized as ours and not handled again.
    if (!GetFrameRect(hwnd, &gapRect)) {
        gapRect = frame;
    }

    Wh_Log(L"Snapped window %p frame=(%d,%d,%d,%d) gap=(%d,%d,%d,%d)", hwnd,
           frame.left, frame.top, frame.right, frame.bottom, gapRect.left,
           gapRect.top, gapRect.right, gapRect.bottom);

    if (it != g_snappedWindows.end()) {
        it->snapRect = frame;
        it->gapRect = gapRect;
    } else {
        g_snappedWindows.push_back(
            {.hwnd = hwnd, .snapRect = frame, .gapRect = gapRect});
    }
}

////////////////////////////////////////////////////////////////////////////////
// Splitters

bool IsDragging() {
    return g_resize.hwnd || g_splitterDrag.hwnd;
}

void DestroySplitters() {
    for (const auto& s : g_splitters) {
        DestroyWindow(s.hwnd);
    }
    g_splitters.clear();
}

// The one of two windows that is lower in the z-order.
HWND GetLowerWindow(HWND a, HWND b) {
    for (HWND w = GetWindow(a, GW_HWNDNEXT); w; w = GetWindow(w, GW_HWNDNEXT)) {
        if (w == b) {
            return b;
        }
    }
    return a;
}

void CreateSplitter(UINT edge,
                    const SnappedWindow& first,
                    const SnappedWindow& second) {
    const RECT& a = first.gapRect;
    const RECT& b = second.gapRect;
    RECT rc = edge == ABE_RIGHT ? RECT{.left = a.right,
                                       .top = std::max(a.top, b.top),
                                       .right = b.left,
                                       .bottom = std::min(a.bottom, b.bottom)}
                                : RECT{.left = std::max(a.left, b.left),
                                       .top = a.bottom,
                                       .right = std::min(a.right, b.right),
                                       .bottom = b.top};
    if (rc.right <= rc.left || rc.bottom <= rc.top) {
        return;
    }

    // Layered with an almost zero alpha: invisible, but still hit-tested.
    HWND hwnd = CreateWindowEx(
        WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE, kSplitterClass,
        nullptr, WS_POPUP, 0, 0, 0, 0, nullptr, nullptr,
        GetModuleHandle(nullptr), nullptr);
    if (!hwnd) {
        Wh_Log(L"CreateWindowEx failed: %u", GetLastError());
        return;
    }
    SetLayeredWindowAttributes(hwnd, 0, 1, LWA_ALPHA);

    // Right below the pair: other windows above it keep covering the gap, and
    // near the edges the windows' own resize borders take precedence.
    SetWindowPos(hwnd, GetLowerWindow(first.hwnd, second.hwnd), rc.left,
                 rc.top, rc.right - rc.left, rc.bottom - rc.top,
                 SWP_NOACTIVATE | SWP_SHOWWINDOW);

    g_splitters.push_back({
        .hwnd = hwnd,
        .edge = edge,
        .first = first.hwnd,
        .second = second.hwnd,
    });
}

// Recreated from scratch every time: a new window always opens on the current
// virtual desktop, so they never end up stranded on another one.
void RefreshSplitters() {
    if (IsDragging()) {
        return;
    }

    DestroySplitters();

    for (const auto& a : g_snappedWindows) {
        if (IsCloaked(a.hwnd)) {
            continue;
        }

        int gap = GetGapForWindow(a.hwnd);
        for (const auto& b : g_snappedWindows) {
            if (b.hwnd == a.hwnd || IsCloaked(b.hwnd)) {
                continue;
            }

            for (UINT edge : {ABE_RIGHT, ABE_BOTTOM}) {
                if (IsAcrossEdge(a.gapRect, edge, b.gapRect, gap)) {
                    CreateSplitter(edge, a, b);
                }
            }
        }
    }
}

void BeginSplitterDrag(HWND hwnd) {
    auto s = std::ranges::find(g_splitters, hwnd, &Splitter::hwnd);
    if (s == g_splitters.end()) {
        return;
    }

    auto first = FindSnappedWindow(s->first);
    if (first == g_snappedWindows.end()) {
        return;
    }

    UINT edge = s->edge;
    UINT opposite = OppositeEdge(edge);
    HMONITOR monitor = MonitorFromWindow(s->first, MONITOR_DEFAULTTONEAREST);
    int minSize = ScaleForMonitor(kMinSnappedSize, monitor);

    POINT pt;
    GetCursorPos(&pt);

    g_splitterDrag = {
        .hwnd = hwnd,
        .edge = edge,
        .startCursor = edge == ABE_RIGHT ? pt.x : pt.y,
        .startPos = GetEdge(first->gapRect, edge),
        .gap = GetGapForWindow(s->first),
    };
    auto& drag = g_splitterDrag;
    drag.minPos = LONG_MIN;
    drag.maxPos = LONG_MAX;

    // Every window along the line moves, like with the Windows 11 divider.
    for (const auto& w : g_snappedWindows) {
        if (IsCloaked(w.hwnd) ||
            MonitorFromWindow(w.hwnd, MONITOR_DEFAULTTONEAREST) != monitor) {
            continue;
        }

        if (abs(GetEdge(w.gapRect, edge) - drag.startPos) <= kEdgeTolerance) {
            drag.before.push_back({w.hwnd, w.gapRect});
            drag.minPos =
                std::max(drag.minPos, GetEdge(w.gapRect, opposite) + minSize);
        } else if (abs(GetEdge(w.gapRect, opposite) -
                       (drag.startPos + drag.gap)) <= kEdgeTolerance) {
            drag.after.push_back({w.hwnd, w.gapRect});
            drag.maxPos = std::min(
                drag.maxPos, GetEdge(w.gapRect, edge) - minSize - drag.gap);
        }
    }

    SetCapture(hwnd);
}

void UpdateSplitterDrag() {
    auto& drag = g_splitterDrag;

    POINT pt;
    GetCursorPos(&pt);
    LONG cursor = drag.edge == ABE_RIGHT ? pt.x : pt.y;
    LONG pos = drag.startPos + (cursor - drag.startCursor);
    if (drag.minPos <= drag.maxPos) {
        pos = std::clamp(pos, drag.minPos, drag.maxPos);
    } else {
        pos = drag.startPos;
    }

    for (auto [hwnd, rc] : drag.before) {
        SetEdge(rc, drag.edge, pos);
        SetTrackedFrame(hwnd, rc);
    }
    for (auto [hwnd, rc] : drag.after) {
        SetEdge(rc, OppositeEdge(drag.edge), pos + drag.gap);
        SetTrackedFrame(hwnd, rc);
    }

    RECT splitterRect;
    GetWindowRect(drag.hwnd, &splitterRect);
    SetEdge(splitterRect, OppositeEdge(drag.edge), pos);
    SetEdge(splitterRect, drag.edge, pos + drag.gap);
    SetWindowPos(drag.hwnd, nullptr, splitterRect.left, splitterRect.top,
                 splitterRect.right - splitterRect.left,
                 splitterRect.bottom - splitterRect.top,
                 SWP_NOZORDER | SWP_NOACTIVATE);
}

void EndSplitterDrag() {
    if (!g_splitterDrag.hwnd) {
        return;
    }

    UpdateSplitterDrag();

    // Cleared first: ReleaseCapture sends WM_CAPTURECHANGED, which ends here.
    g_splitterDrag = {};
    ReleaseCapture();

    RefreshSplitters();
}

LRESULT CALLBACK SplitterWndProc(HWND hwnd,
                                 UINT msg,
                                 WPARAM wParam,
                                 LPARAM lParam) {
    switch (msg) {
        case WM_SETCURSOR: {
            auto s = std::ranges::find(g_splitters, hwnd, &Splitter::hwnd);
            bool vertical = s != g_splitters.end() && s->edge == ABE_RIGHT;
            SetCursor(LoadCursor(nullptr, vertical ? IDC_SIZEWE : IDC_SIZENS));
            return TRUE;
        }

        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;

        case WM_LBUTTONDOWN:
            BeginSplitterDrag(hwnd);
            return 0;

        case WM_MOUSEMOVE:
            if (g_splitterDrag.hwnd == hwnd) {
                UpdateSplitterDrag();
            }
            return 0;

        case WM_LBUTTONUP:
        case WM_CAPTURECHANGED:
            if (g_splitterDrag.hwnd == hwnd) {
                EndSplitterDrag();
            }
            return 0;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

////////////////////////////////////////////////////////////////////////////////
// Resizing a snapped window by its own border also moves its neighbors.

void BeginResizeSession(HWND hwnd) {
    g_resize = {};

    auto it = FindSnappedWindow(hwnd);
    if (it == g_snappedWindows.end()) {
        return;
    }

    g_resize.hwnd = hwnd;
    g_resize.startRect = it->gapRect;

    int gap = GetGapForWindow(hwnd);
    for (const auto& w : g_snappedWindows) {
        for (UINT edge = ABE_LEFT; edge <= ABE_BOTTOM; edge++) {
            if (w.hwnd != hwnd &&
                IsAcrossEdge(it->gapRect, edge, w.gapRect, gap)) {
                g_resize.linked.push_back(
                    {.edge = edge, .hwnd = w.hwnd, .startRect = w.gapRect});
            }
        }
    }

    // Our splitters sit in the gaps that are about to move.
    DestroySplitters();
}

void UpdateResizeSession() {
    RECT frame;
    if (!GetFrameRect(g_resize.hwnd, &frame)) {
        return;
    }

    int gap = GetGapForWindow(g_resize.hwnd);
    for (UINT edge = ABE_LEFT; edge <= ABE_BOTTOM; edge++) {
        // An edge is being resized when the opposite one stays in place. When
        // the window is moved instead, every edge changes.
        UINT opposite = OppositeEdge(edge);
        if (GetEdge(frame, opposite) != GetEdge(g_resize.startRect, opposite)) {
            continue;
        }

        LONG pos = GetEdge(frame, edge);
        if (pos != GetEdge(g_resize.startRect, edge)) {
            g_resize.resized = true;
        }

        for (const auto& link : g_resize.linked) {
            if (link.edge != edge) {
                continue;
            }

            RECT rc = link.startRect;
            SetEdge(rc, opposite, IsLeadingEdge(edge) ? pos - gap : pos + gap);
            if (rc.right - rc.left < kEdgeTolerance ||
                rc.bottom - rc.top < kEdgeTolerance) {
                continue;
            }
            SetTrackedFrame(link.hwnd, rc);
        }
    }
}

void EndResizeSession(HWND hwnd) {
    bool resized = g_resize.hwnd == hwnd && g_resize.resized;
    if (resized) {
        UpdateResizeSession();

        // The user chose this size: keep it as is, without adding the gap
        // again on top of it.
        auto it = FindSnappedWindow(hwnd);
        RECT frame;
        if (it != g_snappedWindows.end() && GetFrameRect(hwnd, &frame)) {
            it->gapRect = frame;
            it->snapRect = ComputeSnapRect(hwnd, frame);
        }
    }

    g_resize = {};
    g_moveSizeWnd = nullptr;

    if (!resized && GetAncestor(hwnd, GA_ROOT) == hwnd &&
        (IsSnapped(hwnd) || FindSnappedWindow(hwnd) != g_snappedWindows.end())) {
        // Possibly dragged into a Snap zone.
        g_pendingWindows.push_back(hwnd);
    }

    SetTimer(g_controlWnd, kSnapTimerId, kSnapDelayMs, nullptr);
}

////////////////////////////////////////////////////////////////////////////////
// Window events

void ProcessPendingWindows() {
    std::erase_if(g_snappedWindows,
                  [](const SnappedWindow& w) { return !IsWindow(w.hwnd); });

    std::vector<HWND> pending;
    pending.swap(g_pendingWindows);
    for (HWND hwnd : pending) {
        UpdateSnappedWindow(hwnd);
    }

    RefreshSplitters();
}

void QueueSnappedWindow(HWND hwnd) {
    if (std::ranges::find(g_pendingWindows, hwnd) == g_pendingWindows.end()) {
        g_pendingWindows.push_back(hwnd);
    }

    // Debounce: Snap Layouts move several windows in a row.
    SetTimer(g_controlWnd, kSnapTimerId, kSnapDelayMs, nullptr);
}

// Windows moved by a drag in progress are handled when it ends.
bool IsMovedByDrag(HWND hwnd) {
    if (std::ranges::find(g_resize.linked, hwnd, &LinkedWindow::hwnd) !=
        g_resize.linked.end()) {
        return true;
    }

    auto isHwnd = [hwnd](const auto& entry) { return entry.first == hwnd; };
    return std::ranges::any_of(g_splitterDrag.before, isHwnd) ||
           std::ranges::any_of(g_splitterDrag.after, isHwnd);
}

void CALLBACK WinEventProc(HWINEVENTHOOK,
                           DWORD event,
                           HWND hwnd,
                           LONG idObject,
                           LONG idChild,
                           DWORD,
                           DWORD) {
    if (!hwnd) {
        return;
    }

    switch (event) {
        case EVENT_SYSTEM_FOREGROUND:
            // The z-order changed: splitters go back below their windows.
            if (!g_snappedWindows.empty()) {
                SetTimer(g_controlWnd, kSnapTimerId, kSnapDelayMs, nullptr);
            }
            return;

        case EVENT_SYSTEM_MOVESIZESTART:
            g_moveSizeWnd = hwnd;
            BeginResizeSession(hwnd);
            return;

        case EVENT_SYSTEM_MOVESIZEEND:
            EndResizeSession(hwnd);
            return;
    }

    if (idObject != OBJID_WINDOW || idChild != CHILDID_SELF) {
        return;
    }

    if (hwnd == g_moveSizeWnd) {
        if (event == EVENT_OBJECT_LOCATIONCHANGE && g_resize.hwnd == hwnd) {
            UpdateResizeSession();
        }
        return;
    }

    if (IsMovedByDrag(hwnd)) {
        return;
    }

    // Tracked windows are queued for any change: moved, closed, hidden,
    // cloaked, or leaving Snap.
    if (FindSnappedWindow(hwnd) != g_snappedWindows.end()) {
        QueueSnappedWindow(hwnd);
        return;
    }

    // Location changes fire for every window move, so filter them out early.
    if ((event == EVENT_OBJECT_LOCATIONCHANGE || event == EVENT_OBJECT_SHOW) &&
        GetAncestor(hwnd, GA_ROOT) == hwnd && IsSnapped(hwnd)) {
        QueueSnappedWindow(hwnd);
    }
}

BOOL CALLBACK EnumSnappedWindowsProc(HWND hwnd, LPARAM) {
    if (IsSnapped(hwnd)) {
        g_pendingWindows.push_back(hwnd);
    }
    return TRUE;
}

void StartSnapGaps() {
    if (g_settings.snapGap == 0 || !g_winEventHooks.empty()) {
        return;
    }

    if (!g_pIsWindowArranged) {
        Wh_Log(L"IsWindowArranged not available, snapped window gaps disabled");
        return;
    }

    constexpr std::pair<DWORD, DWORD> kEventRanges[] = {
        {EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND},
        {EVENT_SYSTEM_MOVESIZESTART, EVENT_SYSTEM_MOVESIZEEND},
        {EVENT_OBJECT_DESTROY, EVENT_OBJECT_HIDE},
        {EVENT_OBJECT_LOCATIONCHANGE, EVENT_OBJECT_LOCATIONCHANGE},
        {EVENT_OBJECT_CLOAKED, EVENT_OBJECT_UNCLOAKED},
    };
    for (auto [first, last] : kEventRanges) {
        HWINEVENTHOOK hook =
            SetWinEventHook(first, last, nullptr, WinEventProc, 0, 0,
                            WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
        if (hook) {
            g_winEventHooks.push_back(hook);
        } else {
            Wh_Log(L"SetWinEventHook failed: %u", GetLastError());
        }
    }

    // Windows that were already snapped before the mod started.
    EnumWindows(EnumSnappedWindowsProc, 0);
    ProcessPendingWindows();
}

// Puts snapped windows back where Snap placed them.
void StopSnapGaps() {
    for (HWINEVENTHOOK hook : g_winEventHooks) {
        UnhookWinEvent(hook);
    }
    g_winEventHooks.clear();

    KillTimer(g_controlWnd, kSnapTimerId);
    g_pendingWindows.clear();
    g_moveSizeWnd = nullptr;
    g_resize = {};
    if (g_splitterDrag.hwnd) {
        g_splitterDrag = {};
        ReleaseCapture();
    }
    DestroySplitters();

    for (const auto& w : g_snappedWindows) {
        RECT frame;
        if (IsSnapped(w.hwnd) && GetFrameRect(w.hwnd, &frame) &&
            EqualRect(&frame, &w.gapRect)) {
            // Asynchronous, so a hung window can't block the mod from exiting.
            SetFrameRect(w.hwnd, w.snapRect, SWP_ASYNCWINDOWPOS);
        }
    }
    g_snappedWindows.clear();
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == kAppBarCallbackMsg) {
        // ABN_POSCHANGED: the taskbar or another AppBar moved. Debounce, since
        // every one of our bars receives it.
        if (wParam == ABN_POSCHANGED && !g_positioning) {
            SetTimer(g_controlWnd, kSyncTimerId, kDebounceMs, nullptr);
        }
        return 0;
    }

    if (hwnd != g_controlWnd) {
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    if (g_taskbarCreatedMsg && msg == g_taskbarCreatedMsg) {
        // Explorer restarted: all AppBar registrations were lost.
        Wh_Log(L"TaskbarCreated");
        SetTimer(hwnd, kRebuildTimerId, kDebounceMs, nullptr);
        return 0;
    }

    switch (msg) {
        case WM_DISPLAYCHANGE:
        case WM_DPICHANGED:
            SetTimer(hwnd, kSyncTimerId, kDebounceMs, nullptr);
            return 0;

        case WM_SETTINGCHANGE:
            // Monitor scaling changes arrive as a settings change.
            if (wParam == SPI_SETWORKAREA || wParam == SPI_SETNONCLIENTMETRICS ||
                wParam == 0) {
                SetTimer(hwnd, kSyncTimerId, kDebounceMs, nullptr);
            }
            return 0;

        case WM_TIMER:
            KillTimer(hwnd, wParam);
            if (wParam == kRebuildTimerId) {
                KillTimer(hwnd, kSyncTimerId);
                RebuildAppBars();
            } else if (wParam == kSyncTimerId) {
                // Follows monitor, scaling, taskbar and other AppBar changes.
                SyncAppBars();
            } else if (wParam == kSnapTimerId) {
                ProcessPendingWindows();
            }
            return 0;

        case kReloadSettingsMsg:
            // Snapped windows are recomputed from where Snap placed them, so
            // the old gap isn't added on top of the new one.
            StopSnapGaps();
            LoadSettings();
            SyncAppBars();
            StartSnapGaps();
            return 0;

        case WM_CLOSE:
            StopSnapGaps();
            DestroyAppBars();
            DestroyWindow(hwnd);
            return 0;

        case WM_DESTROY:
            g_controlWnd = nullptr;
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

DWORD WINAPI WorkerThread(LPVOID param) {
    HANDLE readyEvent = (HANDLE)param;

    // Work with physical pixels on every monitor.
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    WNDCLASS wc{
        .lpfnWndProc = WndProc,
        .hInstance = GetModuleHandle(nullptr),
        .lpszClassName = kWindowClass,
    };
    if (!RegisterClass(&wc)) {
        Wh_Log(L"RegisterClass failed: %u", GetLastError());
        SetEvent(readyEvent);
        return 1;
    }

    g_taskbarCreatedMsg = RegisterWindowMessage(L"TaskbarCreated");

    // A regular (hidden) top-level window, not message-only, so that it
    // receives broadcasts such as TaskbarCreated and WM_DISPLAYCHANGE.
    g_controlWnd = CreateHiddenWindow();
    if (!g_controlWnd) {
        Wh_Log(L"CreateWindowEx failed: %u", GetLastError());
        UnregisterClass(kWindowClass, GetModuleHandle(nullptr));
        SetEvent(readyEvent);
        return 1;
    }

    // Allow TaskbarCreated through UIPI in case Explorer runs elevated.
    ChangeWindowMessageFilterEx(g_controlWnd, g_taskbarCreatedMsg,
                                MSGFLT_ALLOW, nullptr);

    g_pIsWindowArranged = (IsWindowArranged_t)GetProcAddress(
        GetModuleHandle(L"user32.dll"), "IsWindowArranged");

    WNDCLASS splitterClass{
        .lpfnWndProc = SplitterWndProc,
        .hInstance = GetModuleHandle(nullptr),
        .hbrBackground = (HBRUSH)(COLOR_WINDOWTEXT + 1),
        .lpszClassName = kSplitterClass,
    };
    if (!RegisterClass(&splitterClass)) {
        // Snapped windows still get the gap, only the splitters are missing.
        Wh_Log(L"RegisterClass failed: %u", GetLastError());
    }

    SyncAppBars();
    StartSnapGaps();
    SetEvent(readyEvent);

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    DestroyAppBars();
    UnregisterClass(kSplitterClass, GetModuleHandle(nullptr));
    UnregisterClass(kWindowClass, GetModuleHandle(nullptr));
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
// Tool mod callbacks

BOOL WhTool_ModInit() {
    LoadSettings();

    HANDLE readyEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    if (!readyEvent) {
        return FALSE;
    }

    g_thread = CreateThread(nullptr, 0, WorkerThread, readyEvent, 0,
                            &g_threadId);
    if (!g_thread) {
        Wh_Log(L"CreateThread failed: %u", GetLastError());
        CloseHandle(readyEvent);
        return FALSE;
    }

    WaitForSingleObject(readyEvent, INFINITE);
    CloseHandle(readyEvent);

    // The worker exits right away if it couldn't set up its window. Report the
    // failure so the launcher ends the tool process instead of leaving it idle.
    if (!g_controlWnd) {
        WaitForSingleObject(g_thread, INFINITE);
        CloseHandle(g_thread);
        g_thread = nullptr;
        return FALSE;
    }

    return TRUE;
}

void WhTool_ModSettingsChanged() {
    if (g_controlWnd) {
        PostMessage(g_controlWnd, kReloadSettingsMsg, 0, 0);
    }
}

void WhTool_ModUninit() {
    if (!g_thread) {
        return;
    }

    if (g_controlWnd) {
        PostMessage(g_controlWnd, WM_CLOSE, 0, 0);
    } else {
        PostThreadMessage(g_threadId, WM_QUIT, 0, 0);
    }

    WaitForSingleObject(g_thread, INFINITE);
    CloseHandle(g_thread);
    g_thread = nullptr;
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
