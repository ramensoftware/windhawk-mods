// ==WindhawkMod==
// @id              window-magnet
// @name            Window Magnet
// @description     Snaps windows to screen edges and other windows when dragged near them.
// @version         4.0.0
// @author          knchmpgn
// @github          https://github.com/knchmpgn/WindowMagnet.git
// @include         *
// @compilerOptions -lcomctl32 -ldwmapi
// ==/WindhawkMod==

// Drag-interception architecture (DispatchMessage/IsDialogMessage hooks ->
// per-UI-thread WH_CALLWNDPROC -> SetWindowSubclass on WM_ENTERSIZEMOVE ->
// snapping in WM_WINDOWPOSCHANGING) adapted from m417z's "Slick Window
// Arrangement" (https://github.com/ramensoftware/windhawk-mods), GNU GPL v3.
// Magnet logic modified: configurable screen edges (work area vs full
// monitor), inner-edge snapping added, fast-movement detection added.

// ==WindhawkModReadme==
/*
# Window Snapping

Snaps windows to screen edges and other windows when you drag them close.

## How it works
When a window enters an interactive move/size loop (WM_ENTERSIZEMOVE), the
mod subclasses it and adjusts the position in WM_WINDOWPOSCHANGING, which
the system sends *before* every position change. No symbol hooks, no
SetWindowPos recursion, no flicker.

## Settings
- **Attraction field (px)**: snap distance threshold (DPI-scaled).
- **Snap to screen edges**: monitor edges.
- **Use work area**: snap to the taskbar-excluding work area vs. the full monitor.
- **Outer window edges**: snap adjacent to other windows (side-by-side).
- **Inner window edges**: snap to the inside of overlapping windows' borders
  (stacked alignment), AquaSnap-style.
- **Keys to temporarily disable snapping**: hold the key(s) while dragging.
- **Disable during fast movement** + **threshold**: prevents choppy feel on
  quick throws.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- AttractionField: 10
  $name: Attraction field (px)
  $description: Distance in pixels at which a dragged window snaps to an edge.
- SnapToScreenEdges: true
  $name: Snap to screen edges
  $description: Snap to the edges of the monitor.
- SnapToWorkArea: true
  $name: Use work area (exclude taskbar)
  $description: Snap to the work area instead of the full monitor rectangle.
- SnapToOuterWindowEdges: true
  $name: Snap to outer window edges
  $description: Snap adjacent to other windows (side-by-side placement).
- SnapToInnerWindowEdges: true
  $name: Snap to inner window edges
  $description: Snap to the inside of overlapping windows' borders (align stacked windows).
- KeysToDisableSnapping:
  - Ctrl: false
  - Alt: false
  - Shift: true
  $name: Keys to temporarily disable snapping
  $description: Hold the selected key(s) while dragging to temporarily disable snapping.
- NoAttractionDuringFastMovements: true
  $name: Disable during fast movement
  $description: Temporarily disable snapping on quick mouse movements to avoid a choppy feel.
- FastMovementThreshold: 500
  $name: Fast movement threshold (px/frame)
  $description: Speed above which snapping is disabled (pixels per ~60fps frame).
*/
// ==/WindhawkModSettings==

#include <dwmapi.h>
#include <shellscalingapi.h>
#include <windowsx.h>

#include <atomic>
#include <climits>
#include <cmath>
#include <cstdlib>
#include <mutex>
#include <optional>
#include <set>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

struct {
    int attractionField;
    bool snapToScreenEdges;
    bool snapToWorkArea;
    bool snapToOuterWindowEdges;
    bool snapToInnerWindowEdges;
    bool keysToDisableSnappingCtrl;
    bool keysToDisableSnappingAlt;
    bool keysToDisableSnappingShift;
    bool noAttractionDuringFastMovements;
    int fastMovementThreshold;
} g_settings;

typedef DPI_AWARENESS_CONTEXT (WINAPI *GetThreadDpiAwarenessContext_t)();
GetThreadDpiAwarenessContext_t pGetThreadDpiAwarenessContext;

typedef DPI_AWARENESS_CONTEXT (WINAPI *SetThreadDpiAwarenessContext_t)(DPI_AWARENESS_CONTEXT dpiContext);
SetThreadDpiAwarenessContext_t pSetThreadDpiAwarenessContext;

typedef DPI_AWARENESS (WINAPI *GetAwarenessFromDpiAwarenessContext_t)(DPI_AWARENESS_CONTEXT value);
GetAwarenessFromDpiAwarenessContext_t pGetAwarenessFromDpiAwarenessContext;

typedef UINT (WINAPI *GetDpiForSystem_t)();
GetDpiForSystem_t pGetDpiForSystem;

typedef UINT (WINAPI *GetDpiForWindow_t)(HWND hwnd);
GetDpiForWindow_t pGetDpiForWindow;

typedef BOOL (WINAPI *IsWindowArranged_t)(HWND hwnd);
IsWindowArranged_t pIsWindowArranged;

typedef HRESULT (WINAPI *GetDpiForMonitor_t)(HMONITOR hmonitor, MONITOR_DPI_TYPE dpiType, UINT *dpiX, UINT *dpiY);
GetDpiForMonitor_t pGetDpiForMonitor;

BOOL IsWindowCloaked(HWND hwnd)
{
    BOOL isCloaked = FALSE;
    return SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED,
        &isCloaked, sizeof(isCloaked))) && isCloaked;
}

BOOL GetWindowFrameBounds(HWND hWnd, LPRECT lpRect)
{
    if (FAILED(DwmGetWindowAttribute(hWnd, DWMWA_EXTENDED_FRAME_BOUNDS, lpRect, sizeof(*lpRect))) &&
        !GetWindowRect(hWnd, lpRect)) {
        return FALSE;
    }

    if (pGetThreadDpiAwarenessContext && pGetAwarenessFromDpiAwarenessContext &&
        pGetAwarenessFromDpiAwarenessContext(pGetThreadDpiAwarenessContext()) == DPI_AWARENESS_PER_MONITOR_AWARE) {
        // No scaling is needed.
        return TRUE;
    }

    if (pSetThreadDpiAwarenessContext && pGetDpiForMonitor && pGetDpiForSystem) {
        auto prevThreadDpiAwarenessContext =
            pSetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

        HMONITOR monitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);

        MONITORINFO monitorInfo;
        monitorInfo.cbSize = sizeof(monitorInfo);
        GetMonitorInfo(monitor, &monitorInfo);
        OffsetRect(lpRect, -monitorInfo.rcMonitor.left, -monitorInfo.rcMonitor.top);

        UINT dpiFromX, dpiFromY;
        pGetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &dpiFromX, &dpiFromY);
        UINT dpiFrom = dpiFromX; // dpiFromX and dpiFromY are equal

        pSetThreadDpiAwarenessContext(prevThreadDpiAwarenessContext);

        UINT dpiTo = pGetDpiForSystem();

        lpRect->left = MulDiv(lpRect->left, dpiTo, dpiFrom);
        lpRect->top = MulDiv(lpRect->top, dpiTo, dpiFrom);
        lpRect->right = MulDiv(lpRect->right, dpiTo, dpiFrom);
        lpRect->bottom = MulDiv(lpRect->bottom, dpiTo, dpiFrom);

        GetMonitorInfo(monitor, &monitorInfo);
        OffsetRect(lpRect, monitorInfo.rcMonitor.left, monitorInfo.rcMonitor.top);
    }

    return TRUE;
}

class WindowMagnet {
public:
    WindowMagnet(HWND hTargetWnd) {
        CalculateMetrics(hTargetWnd);

        if (g_settings.snapToOuterWindowEdges) {
            InitialWndEnumProcParam enumParam;
            enumParam.hTargetWnd = hTargetWnd;
            EnumWindows(InitialWndEnumProc, (LPARAM)&enumParam);

            for (auto it = enumParam.windowRects.rbegin(); it != enumParam.windowRects.rend(); ++it) {
                const RECT& rc = *it;

                RemoveOverlappedTargets(magnetTargetsLeft, rc.left, rc.right, rc.top, rc.bottom);
                RemoveOverlappedTargets(magnetTargetsTop, rc.top, rc.bottom, rc.left, rc.right);
                RemoveOverlappedTargets(magnetTargetsRight, rc.left, rc.right, rc.top, rc.bottom);
                RemoveOverlappedTargets(magnetTargetsBottom, rc.top, rc.bottom, rc.left, rc.right);

                magnetTargetsLeft.emplace(rc.left, rc.top, rc.bottom);
                magnetTargetsTop.emplace(rc.top, rc.left, rc.right);
                magnetTargetsRight.emplace(rc.right, rc.top, rc.bottom);
                magnetTargetsBottom.emplace(rc.bottom, rc.left, rc.right);
            }
        }

        if (g_settings.snapToScreenEdges) {
            EnumDisplayMonitors(nullptr, nullptr, InitialMonitorEnumProc, (LPARAM)this);
        }
    }

    void MagnetMove(HWND hSourceWnd, int* x, int* y, int* cx, int* cy) {
        if (IsSnappingTemporarilyDisabled()) {
            return;
        }

        CalculateMetrics(hSourceWnd);

        RECT sourceRect = {
            *x + windowBorderRect.left,
            *y + windowBorderRect.top,
            *x + *cx - windowBorderRect.right,
            *y + *cy - windowBorderRect.bottom
        };

        // Gather inner-edge candidates first (they win ties).
        long bestX = LONG_MAX, bestY = LONG_MAX;
        int bestXd = INT_MAX, bestYd = INT_MAX;

        if (g_settings.snapToInnerWindowEdges) {
            InnerEnumParam p;
            p.exclude = hSourceWnd;
            p.source = sourceRect;
            p.cx = *cx;
            p.cy = *cy;
            p.border = windowBorderRect;
            p.field = magnetPixels;
            p.bestX = LONG_MAX; p.bestXd = INT_MAX;
            p.bestY = LONG_MAX; p.bestYd = INT_MAX;

            EnumWindows(InnerEnumProc, (LPARAM)&p);

            bestX = p.bestX; bestXd = p.bestXd;
            bestY = p.bestY; bestYd = p.bestYd;
        }

        if (g_settings.snapToOuterWindowEdges || g_settings.snapToScreenEdges) {
            // Outer candidates: our right edge against their left edge (and
            // screen right edge), our left edge against their right edge
            // (and screen left edge), etc.
            long targetLeft = FindClosestTarget(magnetTargetsLeft,
                sourceRect.right, sourceRect.top, sourceRect.bottom, magnetPixels);
            long targetRight = FindClosestTarget(magnetTargetsRight,
                sourceRect.left, sourceRect.top, sourceRect.bottom, magnetPixels);
            long targetTop = FindClosestTarget(magnetTargetsTop,
                sourceRect.bottom, sourceRect.left, sourceRect.right, magnetPixels);
            long targetBottom = FindClosestTarget(magnetTargetsBottom,
                sourceRect.top, sourceRect.left, sourceRect.right, magnetPixels);

            if (targetLeft != LONG_MAX) {
                int d = (int)labs(targetLeft - sourceRect.right);
                if (d < bestXd) {
                    bestXd = d;
                    bestX = targetLeft - *cx + windowBorderRect.right;
                }
            }
            if (targetRight != LONG_MAX) {
                int d = (int)labs(targetRight - sourceRect.left);
                if (d < bestXd) {
                    bestXd = d;
                    bestX = targetRight - windowBorderRect.left;
                }
            }
            if (targetTop != LONG_MAX) {
                int d = (int)labs(targetTop - sourceRect.bottom);
                if (d < bestYd) {
                    bestYd = d;
                    bestY = targetTop - *cy + windowBorderRect.bottom;
                }
            }
            if (targetBottom != LONG_MAX) {
                int d = (int)labs(targetBottom - sourceRect.top);
                if (d < bestYd) {
                    bestYd = d;
                    bestY = targetBottom - windowBorderRect.top;
                }
            }
        }

        int newX = *x;
        int newY = *y;
        if (bestX != LONG_MAX) newX = (int)bestX;
        if (bestY != LONG_MAX) newY = (int)bestY;

        if (newX != *x || newY != *y) {
            // Make sure the title bar is within a work area, otherwise
            // the window might become undraggable.
            RECT targetRect = {
                newX + windowBorderRect.left,
                newY + windowBorderRect.top,
                newX + *cx - windowBorderRect.right,
                newY + windowBorderRect.top + 1
            };

            if (IsRectInWorkArea(targetRect)) {
                *x = newX;
                *y = newY;
            }
        }
    }

private:
    UINT windowDpi = 0;
    bool windowMaximized = false;
    RECT windowBorderRect{};

    int magnetPixels;
    std::set<std::tuple<long, long, long>> magnetTargetsLeft;
    std::set<std::tuple<long, long, long>> magnetTargetsTop;
    std::set<std::tuple<long, long, long>> magnetTargetsRight;
    std::set<std::tuple<long, long, long>> magnetTargetsBottom;

    void CalculateMetrics(HWND hTargetWnd) {
        UINT prevWindowDpi = windowDpi;
        windowDpi = pGetDpiForWindow ? pGetDpiForWindow(hTargetWnd) : 0;

        bool prevWindowMaximized = windowMaximized;
        windowMaximized = IsZoomed(hTargetWnd);

        if (prevWindowDpi == windowDpi && prevWindowMaximized == windowMaximized) {
            return;
        }

        RECT rect, frame;
        if (GetWindowRect(hTargetWnd, &rect) && GetWindowFrameBounds(hTargetWnd, &frame)) {
            windowBorderRect.left = frame.left - rect.left;
            windowBorderRect.top = frame.top - rect.top;
            windowBorderRect.right = rect.right - frame.right;
            windowBorderRect.bottom = rect.bottom - frame.bottom;
        }

        magnetPixels = g_settings.attractionField;
        if (windowDpi) {
            magnetPixels = MulDiv(magnetPixels, windowDpi, 96);
        }
    }

    struct InitialWndEnumProcParam {
        HWND hTargetWnd;
        std::vector<RECT> windowRects;
    };

    static BOOL CALLBACK InitialWndEnumProc(HWND hWnd, LPARAM lParam) {
        InitialWndEnumProcParam& param = *(InitialWndEnumProcParam*)lParam;

        if (hWnd == param.hTargetWnd) {
            return TRUE;
        }

        if (!IsWindowVisible(hWnd) || IsWindowCloaked(hWnd) || IsIconic(hWnd)) {
            return TRUE;
        }

        if (GetWindowLong(hWnd, GWL_EXSTYLE) & (WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW)) {
            return TRUE;
        }

        RECT rc;
        if (!GetWindowFrameBounds(hWnd, &rc)) {
            return TRUE;
        }

        if (rc.left >= rc.right || rc.top >= rc.bottom) {
            return TRUE;
        }

        param.windowRects.push_back(rc);

        return TRUE;
    }

    static BOOL CALLBACK InitialMonitorEnumProc(HMONITOR monitor, HDC, LPRECT, LPARAM lParam) {
        WindowMagnet& windowMagnet = *(WindowMagnet*)lParam;

        MONITORINFO monitorInfo;
        monitorInfo.cbSize = sizeof(monitorInfo);
        GetMonitorInfo(monitor, &monitorInfo);

        RECT rc = g_settings.snapToWorkArea ? monitorInfo.rcWork : monitorInfo.rcMonitor;

        windowMagnet.magnetTargetsLeft.emplace(rc.right, rc.top, rc.bottom);
        windowMagnet.magnetTargetsTop.emplace(rc.bottom, rc.left, rc.right);
        windowMagnet.magnetTargetsRight.emplace(rc.left, rc.top, rc.bottom);
        windowMagnet.magnetTargetsBottom.emplace(rc.top, rc.left, rc.right);

        return TRUE;
    }

    static void RemoveOverlappedTargets(std::set<std::tuple<long, long, long>>& magnetTargets,
        long start, long end, long otherAxisStart, long otherAxisEnd) {
        for (auto it = magnetTargets.lower_bound(std::make_tuple(start, otherAxisStart, otherAxisStart));
            it != magnetTargets.end();) {
            long a = std::get<0>(*it);
            long b = std::get<1>(*it);
            long c = std::get<2>(*it);

            if (a > end || (a == end && b > otherAxisEnd)) {
                break;
            }

            if (otherAxisStart < c && otherAxisEnd > b) {
                it = magnetTargets.erase(it);

                if (otherAxisStart > b) {
                    magnetTargets.emplace(a, b, otherAxisStart);
                }

                if (otherAxisEnd < c) {
                    magnetTargets.emplace(a, otherAxisEnd, c);
                }
            }
            else {
                ++it;
            }
        }
    }

    static long FindClosestTarget(const std::set<std::tuple<long, long, long>>& magnetTargets,
        long source, long otherAxisStart, long otherAxisEnd, int magnetPixels) {
        long target = LONG_MAX;

        long iterStart = source - magnetPixels;
        long iterEnd = source + magnetPixels;

        for (auto it = magnetTargets.lower_bound(std::make_tuple(iterStart, otherAxisStart, otherAxisStart));
            it != magnetTargets.end();
            ++it) {
            long a = std::get<0>(*it);
            long b = std::get<1>(*it);
            long c = std::get<2>(*it);

            if (a > iterEnd || (a == iterEnd && b > otherAxisEnd)) {
                break;
            }

            if (target != LONG_MAX) {
                if (a == target) {
                    continue;
                }

                if (std::labs(source - a) >= std::labs(source - target)) {
                    break;
                }
            }

            if (otherAxisStart < c && otherAxisEnd > b) {
                target = a;
            }
        }

        return target;
    }

    // Live search for inner-edge candidates: same-edge alignment against
    // windows the dragged window currently overlaps.
    struct InnerEnumParam {
        HWND exclude;
        RECT source;    // dragged window frame rect
        int cx, cy;     // dragged window rect size
        RECT border;    // frame vs window rect difference
        int field;
        long bestX;
        int bestXd;
        long bestY;
        int bestYd;
    };

    static BOOL CALLBACK InnerEnumProc(HWND hWnd, LPARAM lParam) {
        InnerEnumParam& p = *(InnerEnumParam*)lParam;

        if (hWnd == p.exclude) {
            return TRUE;
        }

        if (!IsWindowVisible(hWnd) || IsWindowCloaked(hWnd) || IsIconic(hWnd)) {
            return TRUE;
        }

        if (GetWindowLong(hWnd, GWL_EXSTYLE) & (WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW)) {
            return TRUE;
        }

        RECT o;
        if (!GetWindowFrameBounds(hWnd, &o)) {
            return TRUE;
        }

        if (o.left >= o.right || o.top >= o.bottom) {
            return TRUE;
        }

        // Only windows overlapped by the dragged window can provide inner edges.
        if (!(o.left < p.source.right && o.right > p.source.left &&
              o.top < p.source.bottom && o.bottom > p.source.top)) {
            return TRUE;
        }

        int d;

        d = (int)std::labs(p.source.left - o.left);
        if (d <= p.field && d < p.bestXd) {
            p.bestXd = d;
            p.bestX = o.left - p.border.left;
        }

        d = (int)std::labs(p.source.right - o.right);
        if (d <= p.field && d < p.bestXd) {
            p.bestXd = d;
            p.bestX = o.right - p.cx + p.border.right;
        }

        d = (int)std::labs(p.source.top - o.top);
        if (d <= p.field && d < p.bestYd) {
            p.bestYd = d;
            p.bestY = o.top - p.border.top;
        }

        d = (int)std::labs(p.source.bottom - o.bottom);
        if (d <= p.field && d < p.bestYd) {
            p.bestYd = d;
            p.bestY = o.bottom - p.cy + p.border.bottom;
        }

        return TRUE;
    }

    struct IsRectInWorkAreaMonitorEnumProcParam {
        const RECT* rc;
        bool inWorkArea;
    };

    static BOOL CALLBACK IsRectInWorkAreaMonitorEnumProc(HMONITOR monitor, HDC, LPRECT, LPARAM lParam) {
        IsRectInWorkAreaMonitorEnumProcParam& param = *(IsRectInWorkAreaMonitorEnumProcParam*)lParam;

        MONITORINFO monitorInfo;
        monitorInfo.cbSize = sizeof(monitorInfo);
        GetMonitorInfo(monitor, &monitorInfo);

        const RECT& rcA = *param.rc;
        const RECT& rcB = monitorInfo.rcWork;

        if (rcA.left < rcB.right && rcA.right > rcB.left &&
            rcA.top < rcB.bottom && rcA.bottom > rcB.top) {
            param.inWorkArea = true;
            return FALSE;
        }

        return TRUE;
    }

    static bool IsRectInWorkArea(const RECT& rc) {
        IsRectInWorkAreaMonitorEnumProcParam param;
        param.rc = &rc;
        param.inWorkArea = false;
        EnumDisplayMonitors(nullptr, nullptr, IsRectInWorkAreaMonitorEnumProc, (LPARAM)&param);
        return param.inWorkArea;
    }

    static bool IsSnappingTemporarilyDisabled() {
        if (!g_settings.keysToDisableSnappingCtrl &&
            !g_settings.keysToDisableSnappingAlt &&
            !g_settings.keysToDisableSnappingShift) {
            return false;
        }

        return
            (!g_settings.keysToDisableSnappingCtrl || GetKeyState(VK_CONTROL) < 0) &&
            (!g_settings.keysToDisableSnappingAlt || GetKeyState(VK_MENU) < 0) &&
            (!g_settings.keysToDisableSnappingShift || GetKeyState(VK_SHIFT) < 0);
    }
};

class WindowMoving {
public:
    WindowMoving(HWND hTargetWnd) :
        windowMagnet(hTargetWnd) {}

    void PreProcessPos(HWND hTargetWnd, int* x, int* y, int* cx, int* cy) {
        bool skipMagnet = false;

        // Fast movement detection.
        if (g_settings.noAttractionDuringFastMovements) {
            DWORD now = GetTickCount();
            if (lastSpeed.valid) {
                DWORD dt = now - lastSpeed.tick;
                if (dt > 0) {
                    double dist = std::sqrt(
                        std::pow((double)(*x - lastSpeed.x), 2.0) +
                        std::pow((double)(*y - lastSpeed.y), 2.0));
                    double speed = dist / dt;  // px per ms
                    double threshold = g_settings.fastMovementThreshold / 16.0;  // px/frame -> px/ms
                    if (speed > threshold) {
                        skipMagnet = true;
                    }
                }
            }
            lastSpeed.valid = true;
            lastSpeed.tick = now;
            lastSpeed.x = *x;
            lastSpeed.y = *y;
        }

        MovingState state = GetCurrentMovingState(hTargetWnd, *x, *y);

        // If window state changes, e.g. the window is snapped, don't adjust
        // its position, which could interfere with the snapping and doesn't
        // make sense in general.
        if (lastState && lastState->isMaximized == state.isMaximized &&
            lastState->isMinimized == state.isMinimized &&
            lastState->isArranged == state.isArranged) {
            // Adjust window pos, which can be off in the DPI contexts:
            // DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE
            // DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2

            int lastDeltaX = GET_X_LPARAM(lastState->messagePos) - lastState->x;
            int lastDeltaY = GET_Y_LPARAM(lastState->messagePos) - lastState->y;

            int deltaX = GET_X_LPARAM(state.messagePos) - state.x;
            int deltaY = GET_Y_LPARAM(state.messagePos) - state.y;

            state.x -= lastDeltaX - deltaX;
            state.y -= lastDeltaY - deltaY;

            *x = state.x;
            *y = state.y;
        }

        lastState = state;

        if (!skipMagnet) {
            windowMagnet.MagnetMove(hTargetWnd, x, y, cx, cy);
        }
    }

    void ForgetLastPos() {
        lastState.reset();
        lastSpeed = SpeedState{};
    }

private:
    struct MovingState {
        bool isMinimized;
        bool isMaximized;
        bool isArranged;
        DWORD messagePos;
        int x;
        int y;
    };

    struct SpeedState {
        bool valid;
        DWORD tick;
        int x;
        int y;
        SpeedState() : valid(false), tick(0), x(0), y(0) {}
    };

    static MovingState GetCurrentMovingState(HWND hTargetWnd, int x, int y) {
        MovingState state;
        state.isMinimized = !!IsIconic(hTargetWnd);
        state.isMaximized = !!IsZoomed(hTargetWnd);
        state.isArranged = pIsWindowArranged && !!pIsWindowArranged(hTargetWnd);
        state.messagePos = GetMessagePos();
        state.x = x;
        state.y = y;
        return state;
    }

    std::optional<MovingState> lastState;
    SpeedState lastSpeed;
    WindowMagnet windowMagnet;
};

std::atomic<bool> g_uninitializing;
std::atomic<int> g_hookRefCount;
thread_local std::unordered_map<HWND, WindowMoving> g_winMoving;

UINT g_unsubclassRegisteredMessage = RegisterWindowMessage(
    L"Windhawk_Unsubclass_aqua-magnet-snap");
std::mutex g_subclassedWindowsMutex;
std::unordered_set<HWND> g_subclassedWindows;

thread_local HHOOK g_callWndProcHook;
std::mutex g_allCallWndProcHooksMutex;
std::unordered_set<HHOOK> g_allCallWndProcHooks;

LRESULT CALLBACK SubclassWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);

void hookRefCountScopeIncrement() {
    g_hookRefCount++;
}

void hookRefCountScopeDecrement() {
    g_hookRefCount--;
}

struct HookRefCountScope {
    HookRefCountScope() { hookRefCountScopeIncrement(); }
    ~HookRefCountScope() { hookRefCountScopeDecrement(); }
};

void UnsubclassWindow(HWND hWnd)
{
    RemoveWindowSubclass(hWnd, SubclassWndProc, 0);

    std::lock_guard<std::mutex> guard(g_subclassedWindowsMutex);

    auto it = g_subclassedWindows.find(hWnd);
    if (it != g_subclassedWindows.end()) {
        g_subclassedWindows.erase(it);
    }
}

void OnEnterSizeMove(HWND hWnd)
{
    if (g_settings.snapToScreenEdges ||
        g_settings.snapToOuterWindowEdges ||
        g_settings.snapToInnerWindowEdges) {
        g_winMoving.try_emplace(hWnd, hWnd);
    }
}

void OnExitSizeMove(HWND hWnd)
{
    g_winMoving.erase(hWnd);
    UnsubclassWindow(hWnd);
}

void OnWindowPosChanging(HWND hWnd, WINDOWPOS* windowPos)
{
    if ((windowPos->flags & (SWP_NOSIZE | SWP_NOMOVE)) == (SWP_NOSIZE | SWP_NOMOVE)) {
        return;
    }

    RECT rc;
    if (!GetWindowRect(hWnd, &rc)) {
        return;
    }

    int x = (windowPos->flags & SWP_NOMOVE) ? rc.left : windowPos->x;
    int y = (windowPos->flags & SWP_NOMOVE) ? rc.top : windowPos->y;
    int cx = (windowPos->flags & SWP_NOSIZE) ? (rc.right - rc.left) : windowPos->cx;
    int cy = (windowPos->flags & SWP_NOSIZE) ? (rc.bottom - rc.top) : windowPos->cy;

    bool posChanged = rc.left != x || rc.top != y;
    bool sizeChanged = rc.right - rc.left != cx || rc.bottom - rc.top != cy;

    if (!posChanged && !sizeChanged) {
        return;
    }

    auto it = g_winMoving.find(hWnd);
    if (it == g_winMoving.end()) {
        return;
    }

    WindowMoving& windowMoving = it->second;
    if (posChanged && !sizeChanged) {
        windowMoving.PreProcessPos(hWnd, &x, &y, &cx, &cy);

        if (!(windowPos->flags & SWP_NOMOVE)) {
            windowPos->x = x;
            windowPos->y = y;
        }

        if (!(windowPos->flags & SWP_NOSIZE)) {
            windowPos->cx = cx;
            windowPos->cy = cy;
        }
    }
    else {
        // Resize (or combined), don't snap.
        windowMoving.ForgetLastPos();
    }
}

LRESULT CALLBACK SubclassWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
    HookRefCountScope hookScope;

    switch (uMsg) {
    case WM_ENTERSIZEMOVE:
        OnEnterSizeMove(hWnd);
        break;

    case WM_EXITSIZEMOVE:
        OnExitSizeMove(hWnd);
        break;

    case WM_WINDOWPOSCHANGING:
        OnWindowPosChanging(hWnd, (WINDOWPOS*)lParam);
        break;

    case WM_NCDESTROY:
        UnsubclassWindow(hWnd);
        break;

    default:
        if (uMsg == g_unsubclassRegisteredMessage) {
            RemoveWindowSubclass(hWnd, SubclassWndProc, 0);
        }
        break;
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK CallWndProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    HookRefCountScope hookScope;

    if (nCode != HC_ACTION) {
        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }

    const CWPSTRUCT* cwp = (const CWPSTRUCT*)lParam;

    if (cwp->message == WM_ENTERSIZEMOVE) {
        WCHAR className[32];
        if (GetClassName(GetAncestor(cwp->hwnd, GA_ROOT), className, ARRAYSIZE(className)) &&
            (wcsicmp(className, L"Shell_TrayWnd") == 0 || wcsicmp(className, L"Shell_SecondaryTrayWnd") == 0)) {
            // Skip the taskbar as it causes weird behavior.
        }
        else {
            std::lock_guard<std::mutex> guard(g_subclassedWindowsMutex);
            if (!g_uninitializing && SetWindowSubclass(cwp->hwnd, SubclassWndProc, 0, 0)) {
                g_subclassedWindows.insert(cwp->hwnd);
            }
        }
    }

    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

void SetWindowHookForUiThreadIfNeeded(HWND hWnd)
{
    if (!g_callWndProcHook && IsWindowVisible(GetAncestor(hWnd, GA_ROOT))) {
        std::lock_guard<std::mutex> guard(g_allCallWndProcHooksMutex);
        if (!g_uninitializing) {
            DWORD dwThreadId = GetCurrentThreadId();
            HHOOK callWndProcHook = SetWindowsHookEx(WH_CALLWNDPROC, CallWndProc, nullptr, dwThreadId);
            if (callWndProcHook) {
                Wh_Log(L"SetWindowsHookEx succeeded for thread %u", dwThreadId);
                g_callWndProcHook = callWndProcHook;
                g_allCallWndProcHooks.insert(callWndProcHook);
            }
            else {
                Wh_Log(L"SetWindowsHookEx error for thread %u: %u", dwThreadId, GetLastError());
            }
        }
    }
}

using DispatchMessageA_t = decltype(&DispatchMessageA);
DispatchMessageA_t pOriginalDispatchMessageA;
LRESULT WINAPI DispatchMessageAHook(CONST MSG *lpMsg)
{
    HookRefCountScope hookScope;

    if (lpMsg && lpMsg->hwnd) {
        SetWindowHookForUiThreadIfNeeded(lpMsg->hwnd);
    }

    return pOriginalDispatchMessageA(lpMsg);
}

using DispatchMessageW_t = decltype(&DispatchMessageW);
DispatchMessageW_t pOriginalDispatchMessageW;
LRESULT WINAPI DispatchMessageWHook(CONST MSG *lpMsg)
{
    HookRefCountScope hookScope;

    if (lpMsg && lpMsg->hwnd) {
        SetWindowHookForUiThreadIfNeeded(lpMsg->hwnd);
    }

    return pOriginalDispatchMessageW(lpMsg);
}

using IsDialogMessageA_t = decltype(&IsDialogMessageA);
IsDialogMessageA_t pOriginalIsDialogMessageA;
LRESULT WINAPI IsDialogMessageAHook(HWND hDlg, LPMSG lpMsg)
{
    HookRefCountScope hookScope;

    if (hDlg) {
        SetWindowHookForUiThreadIfNeeded(hDlg);
    }

    return pOriginalIsDialogMessageA(hDlg, lpMsg);
}

using IsDialogMessageW_t = decltype(&IsDialogMessageW);
IsDialogMessageW_t pOriginalIsDialogMessageW;
LRESULT WINAPI IsDialogMessageWHook(HWND hDlg, LPMSG lpMsg)
{
    HookRefCountScope hookScope;

    if (hDlg) {
        SetWindowHookForUiThreadIfNeeded(hDlg);
    }

    return pOriginalIsDialogMessageW(hDlg, lpMsg);
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{
    switch (fdwReason) {
    case DLL_THREAD_DETACH:
        if (g_callWndProcHook) {
            std::lock_guard<std::mutex> guard(g_allCallWndProcHooksMutex);

            auto it = g_allCallWndProcHooks.find(g_callWndProcHook);
            if (it != g_allCallWndProcHooks.end()) {
                UnhookWindowsHookEx(g_callWndProcHook);
                g_allCallWndProcHooks.erase(it);
            }
        }
        break;
    }

    return TRUE;
}

void LoadSettings()
{
    g_settings.attractionField = Wh_GetIntSetting(L"AttractionField");
    g_settings.snapToScreenEdges = Wh_GetIntSetting(L"SnapToScreenEdges");
    g_settings.snapToWorkArea = Wh_GetIntSetting(L"SnapToWorkArea");
    g_settings.snapToOuterWindowEdges = Wh_GetIntSetting(L"SnapToOuterWindowEdges");
    g_settings.snapToInnerWindowEdges = Wh_GetIntSetting(L"SnapToInnerWindowEdges");
    g_settings.keysToDisableSnappingCtrl = Wh_GetIntSetting(L"KeysToDisableSnapping.Ctrl");
    g_settings.keysToDisableSnappingAlt = Wh_GetIntSetting(L"KeysToDisableSnapping.Alt");
    g_settings.keysToDisableSnappingShift = Wh_GetIntSetting(L"KeysToDisableSnapping.Shift");
    g_settings.noAttractionDuringFastMovements = Wh_GetIntSetting(L"NoAttractionDuringFastMovements");
    g_settings.fastMovementThreshold = Wh_GetIntSetting(L"FastMovementThreshold");
}

BOOL Wh_ModInit()
{
    Wh_Log(L"Init");

    LoadSettings();

    HMODULE hUser32 = LoadLibrary(L"user32.dll");
    if (hUser32) {
        pGetThreadDpiAwarenessContext = (GetThreadDpiAwarenessContext_t)GetProcAddress(hUser32, "GetThreadDpiAwarenessContext");
        pSetThreadDpiAwarenessContext = (SetThreadDpiAwarenessContext_t)GetProcAddress(hUser32, "SetThreadDpiAwarenessContext");
        pGetAwarenessFromDpiAwarenessContext = (GetAwarenessFromDpiAwarenessContext_t)GetProcAddress(hUser32, "GetAwarenessFromDpiAwarenessContext");
        pGetDpiForSystem = (GetDpiForSystem_t)GetProcAddress(hUser32, "GetDpiForSystem");
        pGetDpiForWindow = (GetDpiForWindow_t)GetProcAddress(hUser32, "GetDpiForWindow");
        pIsWindowArranged = (IsWindowArranged_t)GetProcAddress(hUser32, "IsWindowArranged");
    }

    HMODULE hShcore = LoadLibrary(L"shcore.dll");
    if (hShcore) {
        pGetDpiForMonitor = (GetDpiForMonitor_t)GetProcAddress(hShcore, "GetDpiForMonitor");
    }

    // DispatchMessageA/W could hopefully be enough to detect a message loop,
    // but DispatchMessageWorker, which implements DispatchMessageA/W, is
    // sometimes called directly by functions such as DialogBoxParam.
    Wh_SetFunctionHook((void*)DispatchMessageA, (void*)DispatchMessageAHook, (void**)&pOriginalDispatchMessageA);
    Wh_SetFunctionHook((void*)DispatchMessageW, (void*)DispatchMessageWHook, (void**)&pOriginalDispatchMessageW);
    Wh_SetFunctionHook((void*)IsDialogMessageA, (void*)IsDialogMessageAHook, (void**)&pOriginalIsDialogMessageA);
    Wh_SetFunctionHook((void*)IsDialogMessageW, (void*)IsDialogMessageWHook, (void**)&pOriginalIsDialogMessageW);

    return TRUE;
}

void Wh_ModUninit()
{
    Wh_Log(L"Uninit");

    g_uninitializing = true;

    std::unordered_set<HWND> subclassedWindows;
    {
        std::lock_guard<std::mutex> guard(g_subclassedWindowsMutex);
        subclassedWindows = g_subclassedWindows;
        g_subclassedWindows.clear();
    }

    for (HWND hWnd : subclassedWindows) {
        SendMessage(hWnd, g_unsubclassRegisteredMessage, 0, 0);
    }

    {
        std::lock_guard<std::mutex> guard(g_allCallWndProcHooksMutex);

        for (HHOOK hook : g_allCallWndProcHooks) {
            UnhookWindowsHookEx(hook);
        }

        g_allCallWndProcHooks.clear();
    }

    while (g_hookRefCount > 0) {
        Sleep(200);
    }
}

void Wh_ModSettingsChanged()
{
    Wh_Log(L"SettingsChanged");

    LoadSettings();
}
